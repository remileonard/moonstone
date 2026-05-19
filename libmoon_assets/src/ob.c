/*
 * ob.c — OB character sprite sheet loader for libmoon_assets.
 *
 * The .ob file format is structurally identical to .cel:
 *
 *   Global header (10 bytes, big-endian):
 *     word[0..1]  = frame_count
 *     long[2..5]  = compressed pixel body size (bytes)
 *     long[6..9]  = reserved
 *
 *   Frame table: frame_count × 10 bytes
 *     long [0..3] = pixel_data_offset (into decompressed pixel buffer)
 *     word [4..5] = width  (pixels)
 *     word [6..7] = height (rows)
 *     byte [8]    = toggle_flags
 *     byte [9]    = planes_mask
 *
 *   Compressed pixel body (LZSS, same algorithm as .cel / LAB_049C)
 *
 * Confirmed by program.asm LAB_0496 (lines 8596-8650) which uses the
 * same header-parsing and LZSS decompression (JSR LAB_049C) as the
 * CEL loader, and by mog.asm LAB_0CBB which uses LAB_0CC2 — a variant
 * of the same LZSS algorithm with identical encoding.
 *
 * MoonOb has an identical memory layout to MoonCel and is decoded by
 * the shared cel_decode() function from cel.c.
 */

#include "moon_private.h"

#include <stdlib.h>

MoonOb *moon_ob_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    /* .ob files use the same format as .cel files */
    MoonCel *cel = cel_decode(buf, len);
    free(buf);
    if (!cel)
        return NULL;

    /*
     * MoonOb has an identical layout to MoonCel (frame_count + frames).
     * Cast directly: both structs have the same fields in the same order.
     */
    return (MoonOb *)(void *)cel;
}

void moon_ob_free(MoonOb *ob)
{
    if (!ob)
        return;
    /* MoonOb and MoonCel share the same memory layout */
    MoonCel *cel = (MoonCel *)(void *)ob;
    for (int i = 0; i < cel->frame_count; i++)
        free(cel->frames[i].data);
    free(cel->frames);
    free(cel);
}
