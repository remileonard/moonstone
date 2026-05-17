/*
 * moon-view-stile — display decompressed tilemap data from a .stile file.
 *
 * Usage: moon-view-stile <file.stile>
 *
 * Decompresses the 2-bit RLE data and dumps byte statistics to stdout,
 * or (with SDL2) visualises each tile as a colour block.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-view-stile <file.stile>\n");
        return 1;
    }

    const char *path = argv[1];

    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || (long)fread(buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f); free(buf); return 1;
    }
    fclose(f);

    size_t dst_cap = (size_t)sz * 8 + 4096;
    uint8_t *data  = (uint8_t *)calloc(1, dst_cap);
    if (!data) { free(buf); return 1; }

    int written = moon_rle_stile_decompress(buf, (size_t)sz, data, dst_cap);
    free(buf);

    if (written < 0) {
        fprintf(stderr, "Error: stile decompression failed\n");
        free(data);
        return 1;
    }

    printf("File         : %s\n", path);
    printf("Compressed   : %ld bytes\n", sz);
    printf("Decompressed : %d bytes\n", written);

    /* Histogram of byte values */
    int hist[256] = {0};
    for (int i = 0; i < written; i++)
        hist[data[i]]++;
    printf("Unique bytes : ");
    int unique = 0;
    for (int i = 0; i < 256; i++)
        if (hist[i]) unique++;
    printf("%d\n", unique);

    /* Try to guess tile geometry: common Amiga tile = 16x16 px × 4 planes
     * = 16/8 × 16 × 4 = 128 bytes per tile */
    int tile_bytes = 128;
    if (written % tile_bytes == 0) {
        int tiles = written / tile_bytes;
        printf("Tiles (16x16x4): %d\n", tiles);
    } else if (written % 160 == 0) { /* 16x16 × 5 planes */
        int tiles = written / 160;
        printf("Tiles (16x16x5): %d\n", tiles);
    }

    free(data);
    return 0;
}
