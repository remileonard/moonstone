/*
 * moon-dump — decompress a Moonstone asset to a raw binary file.
 *
 * Usage: moon-dump <input> <output.bin>
 *
 * Detects the compression format and writes the decompressed bytes.
 * Useful for extracting .cmp modules to raw .mod format for a tracker.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint32_t be32(const uint8_t *p) {
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|(uint32_t)p[3];
}
static uint16_t be16(const uint8_t *p) {
    return (uint16_t)((p[0]<<8)|p[1]);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: moon-dump <input> <output.bin>\n");
        return 1;
    }

    const char *inpath  = argv[1];
    const char *outpath = argv[2];

    /* Read input */
    FILE *fin = fopen(inpath, "rb");
    if (!fin) {
        fprintf(stderr, "Error: cannot open '%s'\n", inpath);
        return 1;
    }
    fseek(fin, 0, SEEK_END);
    long src_len = ftell(fin);
    rewind(fin);

    uint8_t *src = (uint8_t *)malloc((size_t)src_len);
    if (!src || (long)fread(src, 1, (size_t)src_len, fin) != src_len) {
        fprintf(stderr, "Error: read failed\n");
        fclose(fin);
        free(src);
        return 1;
    }
    fclose(fin);

    uint8_t *dst     = NULL;
    int      dst_len = 0;
    const char *algo = "unknown";

    if ((size_t)src_len >= 4 && be32(src) == 0x524E4301u) {
        /* RNC (Unpack_Rnc1 of program) */
        uint32_t uncomp_size = be32(src + 4);
        size_t   dst_cap     = (size_t)uncomp_size + 16;
        dst = (uint8_t *)malloc(dst_cap);
        if (!dst) { free(src); return 1; }
        dst_len = moon_rnc1_decompress(src, (size_t)src_len, dst, dst_cap);
        algo = "RNC (Unpack_Rnc1)";
    } else if ((size_t)src_len >= 6 && (be16(src) == 0x0004 || be16(src) == 0x0005)) {
        /* PIV / .p: header, palette, LZSS body (piv.c) */
        int planes = (int)be16(src);
        size_t body_offset = 6 + (size_t)(planes == 4 ? 16 : 32) * 2;
        uint32_t body_len = be32(src + 2);
        if ((size_t)src_len >= body_offset && body_len <= (size_t)src_len - body_offset) {
            size_t dst_cap = (size_t)planes * 8000;
            dst = (uint8_t *)calloc(1, dst_cap);
            if (!dst) { free(src); return 1; }
            dst_len = moon_lzss_decompress(src + body_offset, body_len, dst, dst_cap);
        }
        algo = "PIV LZSS";
    } else {
        /* CEL: 10-byte header, frame table, LZSS pixels (cel.c) */
        if ((size_t)src_len >= 10) {
            size_t data_offset = 10 + (size_t)be16(src) * 10;
            uint32_t comp_len = be32(src + 2);
            if (data_offset <= (size_t)src_len && comp_len <= (size_t)src_len - data_offset) {
                size_t dst_cap = (size_t)(be32(src + 6) >> 3) + 0x168;
                dst = (uint8_t *)calloc(1, dst_cap);
                if (!dst) { free(src); return 1; }
                dst_len = moon_lzss_decompress(src + data_offset, comp_len, dst, dst_cap);
                algo = "CEL LZSS";
            }
        }
        if (!dst) {
            /* Fallback: just copy raw */
            dst = src; src = NULL;
            dst_len = (int)src_len;
            algo = "raw (no compression detected)";
        }
    }

    if (dst_len < 0) {
        fprintf(stderr, "Error: decompression failed (%s)\n", algo);
        free(dst); free(src);
        return 1;
    }

    /* Write output */
    FILE *fout = fopen(outpath, "wb");
    if (!fout) {
        fprintf(stderr, "Error: cannot write '%s'\n", outpath);
        free(dst); free(src);
        return 1;
    }
    fwrite(dst, 1, (size_t)dst_len, fout);
    fclose(fout);

    printf("Decompressed : %ld bytes → %d bytes  (%s)\n",
           src_len, dst_len, algo);
    printf("Written to   : %s\n", outpath);

    free(dst);
    free(src);
    return 0;
}
