/*
 * cel.c — CEL sprite sheet loader for libmoon_assets.
 *
 * CEL file format (observed from LAB_04A8 / LAB_049C in program.asm,
 * confirmed by §6.2.3 of DOC_TECHNIQUE.md):
 *
 * Global header (10 bytes):
 *   word[0]   = frame_count  (number of animation frames)
 *   long[2..5]= compressed pixel data size (bytes)
 *   long[6..9]= decompressed pixel data size, in bits (LAB_0CB6 reserves
 *               (size >> 3) + 0x168 bytes for the pixels)
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
 * Pixel data is planar, up to 5 planes, one plane after the other.
 * A frame whose plane mask is 0 has no pixels (nothing is drawn).
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
     * Global header (10 bytes, LAB_0CBB in mog.asm):
     *   word[0..1]  = frame_count
     *   long[2..5]  = compressed pixel data size (bytes)
     *   long[6..9]  = decompressed pixel data size (bits)
     *
     * Frame table starts at offset 10; each entry is 10 bytes:
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
    uint32_t pix_bits    = ((uint32_t)buf[6] << 24) | ((uint32_t)buf[7] << 16) |
                           ((uint32_t)buf[8] <<  8) |  (uint32_t)buf[9];

    if (frame_count == 0)
        return NULL;

    size_t frame_table_end = (size_t)10 + (size_t)frame_count * 10;
    if (frame_table_end > len || comp_size > len - frame_table_end)
        return NULL;
    const uint8_t *comp_data = buf + frame_table_end;

    /* The original reserves (bits >> 3) + 0x168 bytes (LAB_0CB6) */
    size_t decomp_max = (size_t)(pix_bits >> 3) + 0x168;
    uint8_t *pixels = (uint8_t *)calloc(1, decomp_max);
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

        MoonCelFrame *f = &cel->frames[i];
        f->width      = w;
        f->height     = h;
        f->draw_flags = m[8];   /* bit 0 = draw toggle */
        f->plane_mask = m[9];   /* bit n = bitplane n active */
        f->offset     = frame_off;
        f->minterm    = 0;      /* not stored in this format */

        /* Active bitplanes: bits 0..4 of the mask (LAB_04AB); 0 = none */
        int p = 0;
        for (int b = 0; b < 5; b++)
            if (f->plane_mask & (1u << b)) p++;
        f->planes = (uint8_t)p;

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
