/*
 * moon-info — display metadata about any Moonstone asset file.
 *
 * Usage: moon-info <file> [file ...]
 *
 * Detects and prints the format, compression algorithm, and key
 * attributes of each asset without loading the full game.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* Big-endian helpers */
static uint32_t be32(const uint8_t *p) {
    return ((uint32_t)p[0]<<24)|((uint32_t)p[1]<<16)|((uint32_t)p[2]<<8)|(uint32_t)p[3];
}
static uint16_t be16(const uint8_t *p) {
    return (uint16_t)((p[0]<<8)|p[1]);
}

static void inspect_file(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);

    if (sz < 4) {
        printf("%-40s  %ld bytes  (too small to identify)\n", path, sz);
        fclose(f);
        return;
    }

    uint8_t hdr[32];
    size_t nread = fread(hdr, 1, sizeof(hdr), f);
    fclose(f);

    printf("File    : %s  (%ld bytes)\n", path, sz);

    if (nread >= 4 && be32(hdr) == 0x524E4301u) {
        /* RNC, Unpack_Rnc1 of program: 12-byte header */
        uint32_t uncomp = (nread >= 8)  ? be32(hdr + 4) : 0;
        uint32_t comp   = (nread >= 12) ? be32(hdr + 8) : 0;
        float    ratio  = (comp > 0) ? (float)uncomp / (float)comp : 0.0f;
        printf("Format  : CMP — RNC (\"RNC\\x01\", 12-byte header)\n");
        printf("Algo    : backward bit stream, literals + matches (LAB_0190)\n");
        printf("Packed  : %u bytes → %u bytes (ratio %.2f)\n",
               comp, uncomp, ratio);
    } else if (nread >= 6 && (be16(hdr) == 0x0004 || be16(hdr) == 0x0005)) {
        /* Mindscape PIV (also the .p pictures) */
        int planes = (int)be16(hdr);
        printf("Format  : PIV Mindscape background (320x200)\n");
        printf("Algo    : LZSS (LAB_0402 / LAB_0C27)\n");
        printf("Planes  : %d (%d colours)\n", planes, 1 << planes);
        printf("Packed  : %u bytes → %d bytes\n", be32(hdr + 2), 8000 * planes);
    } else if (nread >= 10) {
        /* CEL / .ob / .c / .f — frame count, packed size, unpacked bits */
        uint16_t frame_count = be16(hdr);
        uint32_t packed = be32(hdr + 2);
        long data_offset = 10 + 10L * frame_count;
        if (frame_count > 0 && data_offset + (long)packed <= sz) {
            printf("Format  : CEL Mindscape sprite sheet\n");
            printf("Algo    : LZSS (window 2 KB, len 3..34) — LAB_049C\n");
            printf("Frames  : %u\n", (unsigned)frame_count);
            printf("Packed  : %u bytes at 0x%04lX → %u bytes\n",
                   packed, data_offset, be32(hdr + 6) >> 3);
        } else {
            printf("Format  : Unknown / raw data\n");
            printf("Magic   : %02X %02X %02X %02X ...\n",
                   hdr[0], hdr[1], hdr[2], hdr[3]);
        }
    } else {
        printf("Format  : Unknown\n");
        printf("Magic   : %02X %02X %02X %02X\n",
               hdr[0], hdr[1], hdr[2], hdr[3]);
    }
    printf("\n");
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-info <file> [file ...]\n");
        return 1;
    }
    for (int i = 1; i < argc; i++)
        inspect_file(argv[i]);
    return 0;
}
