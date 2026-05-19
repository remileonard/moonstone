/*
 * stile.c — STILE tilemap loader for libmoon_assets.
 *
 * STILE files store tile bitmap data compressed with a 2-bit RLE scheme
 * (LAB_0448 in program.asm).  The decompressor is in rle_stile.c.
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

    /* Stile files have a small header; the compressed data follows directly */
    size_t decomp_max = len * 8 + 4096; /* RLE can expand significantly */
    uint8_t *data = (uint8_t *)calloc(1, decomp_max);
    if (!data) {
        free(buf);
        return NULL;
    }

    int written = moon_rle_stile_decompress(buf, len, data, decomp_max);
    free(buf);
    if (written < 0) {
        free(data);
        return NULL;
    }

    MoonStile *stile = (MoonStile *)calloc(1, sizeof(MoonStile));
    if (!stile) {
        free(data);
        return NULL;
    }
    stile->size = (size_t)written;
    stile->data = data;
    return stile;
}

void moon_stile_free(MoonStile *stile)
{
    if (!stile)
        return;
    free(stile->data);
    free(stile);
}
