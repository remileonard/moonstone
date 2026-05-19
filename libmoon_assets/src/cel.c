/*
 * cel.c — CEL sprite sheet loader for libmoon_assets.
 *
 * CEL file format (observed from LAB_04A8 / LAB_049C in program.asm,
 * confirmed by §6.2.3 of DOC_TECHNIQUE.md):
 *
 * Global header (10 bytes):
 *   word[0]   = frame_count  (number of animation frames)
 *   long[2..5]= compressed pixel data size (bytes)
 *   long[6..9]= reserved
 *
 * frames × (per-frame-header, 10 bytes each):
 *     long  = offset_from_data_start (byte offset into decompressed pixel data)
 *     word  = width   (pixels)
 *     word  = height  (rows)
 *     byte  = toggle_flags       (bit 0 = draw toggle)
 *     byte  = planes_mask        (bit n = bitplane n active)
 *
 * All values are big-endian.
 *
 * After the frame table, the LZSS-compressed pixel data begins as a single
 * stream; the per-frame offsets index into the decompressed output.
 * Pixel data is planar, up to 5 planes, interleaved row-by-row.
 */

#include "moon_private.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal decoder (also used by ob.c)                               */
/* ------------------------------------------------------------------ */

MoonCel *cel_decode(const uint8_t *buf, size_t len)
{
    if (len < 10)
        return NULL;

    /*
     * Global header (10 bytes, program.asm lines 8596-8604):
     *   word[0..1]  = frame_count
     *   long[2..5]  = compressed pixel data size (bytes)
     *   long[6..9]  = reserved / unknown
     *
     * Frame table starts at offset 10; each entry is 10 bytes
     * (MULU #$000a,D0 at lines 8606/8638/8645/8663/8690):
     *   long [0..3] = pixel_data_offset (into decompressed pixel buffer)
     *   word [4..5] = width  (pixels)
     *   word [6..7] = height (rows)
     *   byte [8]    = toggle_flags (bit 0 is the draw-toggle)
     *   byte [9]    = planes_mask  (bit n = 1 → bitplane n is active)
     *
     * Compressed pixel data begins at offset 10 + frame_count * 10.
     */
    uint16_t frame_count = (uint16_t)((buf[0] << 8) | buf[1]);
    uint32_t comp_size   = ((uint32_t)buf[2] << 24) | ((uint32_t)buf[3] << 16) |
                           ((uint32_t)buf[4] <<  8) |  (uint32_t)buf[5];

    if (frame_count == 0)
        return NULL;

    /* Validate frame table fits in file */
    size_t frame_table_end = (size_t)10 + (size_t)frame_count * 10;
    if (frame_table_end > len)
        return NULL;

    /* Locate compressed data */
    const uint8_t *comp_data = buf + frame_table_end;
    size_t comp_avail = len - frame_table_end;
    if (comp_size > (uint32_t)comp_avail)
        comp_size = (uint32_t)comp_avail;   /* clamp to available bytes */

    /* Estimate decompressed size: actual size is stored in comp_size but
     * that is the COMPRESSED size; decompressed can be up to ~4× larger. */
    size_t decomp_max = (size_t)comp_size * 4 + 65536;
    uint8_t *pixels = (uint8_t *)malloc(decomp_max);
    if (!pixels)
        return NULL;

    int decomp_len = moon_lzss_decompress(comp_data, (size_t)comp_size,
                                          pixels, decomp_max);
    if (decomp_len < 0) {
        free(pixels);
        return NULL;
    }

    MoonCel *cel = (MoonCel *)calloc(1, sizeof(MoonCel));
    if (!cel) {
        free(pixels);
        return NULL;
    }
    cel->frame_count = (int)frame_count;
    cel->frames = (MoonCelFrame *)calloc((size_t)frame_count, sizeof(MoonCelFrame));
    if (!cel->frames) {
        free(pixels);
        free(cel);
        return NULL;
    }

    for (int i = 0; i < (int)frame_count; i++) {
        const uint8_t *m = buf + 10 + (size_t)i * 10;   /* stride = 10 */
        uint32_t frame_off   = ((uint32_t)m[0] << 24) | ((uint32_t)m[1] << 16) |
                               ((uint32_t)m[2] <<  8) |  (uint32_t)m[3];
        uint16_t w           = (uint16_t)((m[4] << 8) | m[5]);
        uint16_t h           = (uint16_t)((m[6] << 8) | m[7]);
        uint8_t  toggle_flags = m[8];   /* bit 0 = draw toggle */
        uint8_t  planes_mask  = m[9];   /* bit n = bitplane n active */

        MoonCelFrame *f = &cel->frames[i];
        f->width      = w;
        f->height     = h;
        f->draw_flags = toggle_flags;
        f->minterm    = 0;   /* not stored in this format */

        /*
         * Count active bitplanes from the mask (bits 0..4 checked by
         * LAB_04AB: LSR.W #1,D6; BCS.S LAB_04AC; DBF D7,LAB_04AB).
         */
        {
            int p = 0;
            for (int b = 0; b < 5; b++)
                if (planes_mask & (1u << b)) p++;
            f->planes = (p >= 1 && p <= 5) ? (uint8_t)p : 5;
        }

        /* Row stride per plane, word-aligned */
        int row_bytes  = ((w + 15) / 16) * 2;
        size_t frame_size = (size_t)f->planes * (size_t)row_bytes * (size_t)h;

        if (frame_off + frame_size > (size_t)decomp_len) {
            /* Clamp gracefully for oversized references */
            frame_size = (frame_off < (size_t)decomp_len)
                         ? (size_t)decomp_len - frame_off : 0;
        }

        f->data = (uint8_t *)calloc(1, frame_size ? frame_size : 1);
        if (f->data && frame_size)
            memcpy(f->data, pixels + frame_off, frame_size);
    }

    free(pixels);
    return cel;
}

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

MoonCel *moon_cel_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonCel *cel = cel_decode(buf, len);
    free(buf);
    return cel;
}

void moon_cel_free(MoonCel *cel)
{
    if (!cel)
        return;
    for (int i = 0; i < cel->frame_count; i++)
        free(cel->frames[i].data);
    free(cel->frames);
    free(cel);
}
