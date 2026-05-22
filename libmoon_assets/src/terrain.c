/*
 * terrain.c — .t terrain file loader for libmoon_assets.
 *
 * .t files (FO1.t–FO8.t, Sw1.t–Sw8.t, GL1.t–GL8.t, Wa1.t–Wa8.t) store
 * the combat arena layout for a given terrain type.  They are LZSS
 * compressed (same algorithm as .cel and .piv files).
 *
 * Loading procedure mirrors mog.asm LAB_0A6D (lines 19091–19118):
 *
 *   1. Load the raw file and LZSS-decompress it into a working buffer.
 *
 *   2. Read the obstacle count (uint16_be at offset 0).
 *
 *   3. Parse N × 8-byte collision entries that follow:
 *        word[0] x_left  — screen X left edge
 *        word[1] x_right — screen X right edge
 *        word[2] y_depth — Y depth threshold (LAB_0A71 BCLR blocking logic)
 *        word[3] extra   — ancillary word (not used by collision engine)
 *
 *   4. Parse the 2400-byte visual object block that immediately follows the
 *      obstacle table (LAB_0A6E copies exactly 0x960 = 2400 bytes to
 *      LAB_0A83; SECSTRT_12 then iterates over 6-byte records until it sees
 *      a record whose type high byte is 0xFF):
 *        word[0] type   — high byte: 0xFF=end, 0xFE=skip, 0x03=char sprites,
 *                                    0x04 or other=terrain sprites
 *                         low  byte: sprite index within the bank
 *        word[1] x      — screen X
 *        word[2] y      — screen Y
 *
 * Files are optional; moon_terrain_load() returns NULL if the file is
 * absent, allowing the game to use the LAB_0A6C fallback obstacle entry.
 */

#include "moon_private.h"

#include <stdlib.h>
#include <string.h>

/* Maximum decompressed buffer size.  The original game allocated exactly
 * 0x960 bytes for the visual buffer (LAB_0A83) plus the obstacle table
 * before it.  A generous upper bound covers all known .t variants. */
#define TERRAIN_DECOMP_MAX  16384

/* Size of the visual display block (LAB_0A6E: DBF D1,LAB_0A6E with D1 =
 * 0x95f, so the loop runs 0x960 = 2400 times). */
#define TERRAIN_VISUAL_SIZE 2400

/* Maximum number of visual object records in the 2400-byte block. */
#define TERRAIN_OBJECT_MAX  (TERRAIN_VISUAL_SIZE / 6)

MoonTerrain *moon_terrain_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t raw_len;
    uint8_t *raw = moon_file_read(name, &raw_len);
    if (!raw)
        return NULL;

    /* Decompress the LZSS payload.  The entire file is the LZSS stream
     * (no extra header, matching the LAB_0CC0 loader). */
    uint8_t *decomp = (uint8_t *)malloc(TERRAIN_DECOMP_MAX);
    if (!decomp) {
        free(raw);
        return NULL;
    }

    int decomp_len = moon_lzss_decompress(raw, raw_len, decomp,
                                          TERRAIN_DECOMP_MAX);
    free(raw);

    if (decomp_len < 2) {
        free(decomp);
        return NULL;
    }

    /* ---- Parse obstacle count (uint16_be at offset 0) ---- */
    int n_obs = (int)(((uint16_t)decomp[0] << 8) | (uint16_t)decomp[1]);

    /* Validate: obstacle table must fit in the decompressed buffer */
    size_t obs_table_end = (size_t)2 + (size_t)n_obs * 8;
    if (obs_table_end + TERRAIN_VISUAL_SIZE > (size_t)decomp_len) {
        /* Clamp to available data */
        if (obs_table_end > (size_t)decomp_len) {
            n_obs = (int)((size_t)(decomp_len - 2) / 8);
            obs_table_end = (size_t)2 + (size_t)n_obs * 8;
        }
    }

    /* ---- Parse obstacle entries ---- */
    MoonTerrainObstacle *obstacles = NULL;
    if (n_obs > 0) {
        obstacles = (MoonTerrainObstacle *)malloc(
            (size_t)n_obs * sizeof(MoonTerrainObstacle));
        if (!obstacles) {
            free(decomp);
            return NULL;
        }
        const uint8_t *p = decomp + 2;
        for (int i = 0; i < n_obs; i++, p += 8) {
            obstacles[i].x_left  = (int16_t)(((uint16_t)p[0] << 8) | p[1]);
            obstacles[i].x_right = (int16_t)(((uint16_t)p[2] << 8) | p[3]);
            obstacles[i].y_depth = (int16_t)(((uint16_t)p[4] << 8) | p[5]);
            obstacles[i].extra   = (int16_t)(((uint16_t)p[6] << 8) | p[7]);
        }
    }

    /* ---- Parse visual objects from the 2400-byte block ---- */
    /* The block starts immediately after the obstacle table.  We read
     * 6-byte records until we see a type high byte of 0xFF (end marker)
     * or exhaust the 2400-byte region. */
    MoonTerrainObject tmp_objects[TERRAIN_OBJECT_MAX];
    int n_obj = 0;

    size_t visual_start = obs_table_end;
    size_t visual_avail = (size_t)decomp_len - visual_start;
    if (visual_avail > TERRAIN_VISUAL_SIZE)
        visual_avail = TERRAIN_VISUAL_SIZE;

    const uint8_t *vp = decomp + visual_start;
    size_t vi = 0;
    while (vi + 6 <= visual_avail && n_obj < TERRAIN_OBJECT_MAX) {
        uint8_t type_hi = vp[vi];
        uint8_t type_lo = vp[vi + 1];

        if (type_hi == 0xFF)   /* end-of-list marker */
            break;

        if (type_hi != 0xFE) { /* 0xFE = skip (no render) */
            int16_t ox = (int16_t)(((uint16_t)vp[vi + 2] << 8) | vp[vi + 3]);
            int16_t oy = (int16_t)(((uint16_t)vp[vi + 4] << 8) | vp[vi + 5]);
            tmp_objects[n_obj].sprite_bank = type_hi;
            tmp_objects[n_obj].sprite_idx  = type_lo;
            tmp_objects[n_obj].x           = ox;
            tmp_objects[n_obj].y           = oy;
            n_obj++;
        }
        vi += 6;
    }

    free(decomp);

    /* ---- Assemble result struct ---- */
    MoonTerrain *terrain = (MoonTerrain *)calloc(1, sizeof(MoonTerrain));
    if (!terrain) {
        free(obstacles);
        return NULL;
    }

    terrain->n_obstacles = n_obs;
    terrain->obstacles   = obstacles;

    if (n_obj > 0) {
        terrain->objects = (MoonTerrainObject *)malloc(
            (size_t)n_obj * sizeof(MoonTerrainObject));
        if (!terrain->objects) {
            free(obstacles);
            free(terrain);
            return NULL;
        }
        memcpy(terrain->objects, tmp_objects,
               (size_t)n_obj * sizeof(MoonTerrainObject));
    }
    terrain->n_objects = n_obj;

    return terrain;
}

void moon_terrain_free(MoonTerrain *terrain)
{
    if (!terrain)
        return;
    free(terrain->obstacles);
    free(terrain->objects);
    free(terrain);
}
