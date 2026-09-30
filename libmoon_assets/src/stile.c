/*
 * stile.c — STILE tile map loader for libmoon_assets.
 *
 * A .stile file (intro.stile, co.stile) is not compressed: its 960 bytes
 * are read as they are (LAB_0185 in program.asm reads up to 1000 bytes
 * with LAB_03B2 and decodes nothing) and hold a map of big-endian tile numbers, one word per
 * tile, used by the intro's vertical scroll (LAB_05A5).
 */

#include "moon_private.h"

#include <stdlib.h>

MoonStile *moon_stile_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonStile *stile = (MoonStile *)calloc(1, sizeof(MoonStile));
    if (!stile) {
        free(buf);
        return NULL;
    }
    stile->size = len;
    stile->data = buf;
    return stile;
}

uint16_t moon_stile_tile(const MoonStile *stile, size_t index)
{
    if (!stile || index * 2 + 1 >= stile->size)
        return 0;
    return (uint16_t)(stile->data[index * 2] << 8 | stile->data[index * 2 + 1]);
}

void moon_stile_free(MoonStile *stile)
{
    if (!stile)
        return;
    free(stile->data);
    free(stile);
}
