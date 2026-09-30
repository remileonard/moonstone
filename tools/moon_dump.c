/*
 * moon-dump — unpack a Moonstone game file to a raw binary file.
 *
 * Usage: moon-dump <input> <output.bin> [index]
 *
 * By kind (moon_file_kind):
 *   CEL      the unpacked pixels (LZSS body), frame table not included
 *   PIV      the unpacked bitmap (planes one after the other)
 *   module   the unpacked .mod (RNC), readable by a tracker
 *   test     the bitmap of picture `index` (default 0)
 * Other files are not packed: they are copied as they are.
 */
#include "moon_tool.h"
#include "moon_testmap.h"

#include <stdlib.h>

static uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

/* CEL: 10-byte header (frames, packed size, unpacked size in bits), frame
 * table of 10 bytes per frame, LZSS pixels. */
static uint8_t *unpack_cel(const uint8_t *raw, size_t len, size_t *out)
{
    if (len < 10)
        return NULL;
    size_t start = 10 + (size_t)(raw[0] << 8 | raw[1]) * 10;
    uint32_t packed = be32(raw + 2);
    size_t cap = (be32(raw + 6) >> 3) + 0x168;
    if (start > len || packed > len - start)
        return NULL;
    uint8_t *dst = calloc(1, cap);
    int n = dst ? moon_lzss_decompress(raw + start, packed, dst, cap) : -1;
    if (n < 0) {
        free(dst);
        return NULL;
    }
    *out = (size_t)n;
    return dst;
}

static uint8_t *piv_bitmap(MoonPiv *piv, size_t *out)
{
    if (!piv)
        return NULL;
    *out = (size_t)piv->planes * piv->height * ((piv->width + 15) / 16 * 2);
    uint8_t *dst = malloc(*out);
    if (dst)
        memcpy(dst, piv->bitmap, *out);
    moon_piv_free(piv);
    return dst;
}

static uint8_t *unpack_mod(const uint8_t *raw, size_t len, size_t *out)
{
    if (len < 12 || be32(raw) != 0x524E4301u)
        return NULL;
    size_t cap = be32(raw + 4);
    uint8_t *dst = malloc(cap ? cap : 1);
    int n = dst ? moon_rnc1_decompress(raw, len, dst, cap) : -1;
    if (n < 0) {
        free(dst);
        return NULL;
    }
    *out = (size_t)n;
    return dst;
}

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4) {
        fprintf(stderr, "Usage: moon-dump <input> <output.bin> [index]\n");
        return 1;
    }
    char name[256];
    tool_open(argv[1], name, sizeof name);
    size_t len = 0, out = 0;
    uint8_t *raw = moon_file_read(name, &len);
    if (!raw) {
        fprintf(stderr, "Error: cannot read '%s'\n", argv[1]);
        return 1;
    }
    MoonFileKind kind = moon_file_kind(name);
    uint8_t *dst;
    switch (kind) {
    case MOON_KIND_CEL:     dst = unpack_cel(raw, len, &out); break;
    case MOON_KIND_PIV:     dst = piv_bitmap(moon_piv_load_from_buffer(raw, len), &out); break;
    case MOON_KIND_MOD:     dst = unpack_mod(raw, len, &out); break;
    case MOON_KIND_TESTMAP:
        dst = piv_bitmap(moon_testmap_load_piv_from_buffer(raw, len, argc > 3 ? atoi(argv[3]) : 0), &out);
        break;
    default:
        dst = raw, raw = NULL, out = len;
    }
    free(raw);
    if (!dst) {
        fprintf(stderr, "Error: '%s' not decoded (%s)\n", argv[1], tool_kind_name(kind));
        return 1;
    }
    FILE *f = fopen(argv[2], "wb");
    if (!f || fwrite(dst, 1, out, f) != out) {
        fprintf(stderr, "Error: cannot write '%s'\n", argv[2]);
        return 1;
    }
    fclose(f);
    printf("%s (%s) : %zu -> %zu bytes, written to %s\n", argv[1], tool_kind_name(kind),
           len, out, argv[2]);
    free(dst);
    return 0;
}
