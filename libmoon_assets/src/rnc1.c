/*
 * rnc1.c — RNC decompressor of Moonstone's .cmp music files
 *
 * Port of Unpack_Rnc1 (LAB_0190) in program.asm (Moonstone — A Hard Days
 * Knight, Mindscape 1991), which decompresses music.cmp and vmusic.cmp.
 *
 * Format (all big-endian):
 *   Bytes  0-3  : magic "RNC\x01" (0x524E4301)
 *   Bytes  4-7  : unpacked size
 *   Bytes  8-11 : packed data size (after this 12-byte header)
 *   Bytes 12+   : packed data
 *
 * There are no CRCs and no Huffman tables: the packed data is read
 * BACKWARDS, from its last byte, as a bit stream (MSB first), and the
 * output is written backwards from its end.  Each round:
 *   bit 1       : a run of literal bytes; a second bit 1 gives its length
 *                 from the escape tables LAB_019F / LAB_01A0, otherwise
 *                 one byte;
 *   (end when all packed bytes are read)
 *   length      : unary prefix (up to 4 bits) selecting LAB_01A8 / L08_008B3;
 *   distance    : length 2 → 6 bits, or 1 + 9 bits (+ 64);
 *                 otherwise a unary prefix (up to 2 bits) selecting
 *                 LAB_01B0 (bit count - 1) / LAB_01B1 (base);
 *   the match copies bytes already written (higher addresses).
 */

#include "moon_assets.h"

#include <string.h>

static uint32_t read_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |  (uint32_t)p[3];
}

/* Tables of program.asm (LAB_019F, LAB_01A0, LAB_01A8, L08_008B3,
 * LAB_01B0, LAB_01B1) */
static const uint8_t  k_lit_bits[4]  = { 0x0A, 0x03, 0x02, 0x02 };
static const uint8_t  k_lit_base[4]  = { 0x0E, 0x07, 0x04, 0x01 };
static const uint8_t  k_len_bits[5]  = { 0x0A, 0x02, 0x01, 0x00, 0x00 };
static const uint8_t  k_len_base[5]  = { 0x0A, 0x06, 0x04, 0x03, 0x02 };
static const uint8_t  k_dist_bits[3] = { 0x0B, 0x04, 0x07 };
static const uint16_t k_dist_base[3] = { 0x0120, 0x0000, 0x0020 };

/* Backward bit reader: LAB_01A1 (ADD.B D3,D3 / MOVE.B -(A6),D3 / ADDX.B) */
typedef struct {
    const uint8_t *start;   /* first packed byte */
    const uint8_t *p;       /* one past the next byte to load */
    uint8_t        d3;
    int            error;
} RncBits;

static int rnc_bit(RncBits *r)
{
    int c = r->d3 >> 7;
    r->d3 = (uint8_t)(r->d3 << 1);
    if (r->d3)
        return c;
    if (r->p <= r->start) {
        r->error = 1;
        return 0;
    }
    uint8_t b = *--r->p;
    r->d3 = (uint8_t)((b << 1) | c);
    return b >> 7;
}

static unsigned rnc_bits(RncBits *r, int n)
{
    unsigned v = 0;
    while (n--)
        v = (v << 1) | (unsigned)rnc_bit(r);
    return v & 0xFFFF;
}

int moon_rnc1_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len)
{
    if (src_len < 12 || read_be32(src) != 0x524E4301u)
        return -1;
    uint32_t unpacked = read_be32(src + 4);
    uint32_t packed   = read_be32(src + 8);
    if (packed == 0 || packed > src_len - 12 || unpacked > dst_len)
        return -1;

    RncBits r = { src + 12, src + 12 + packed, 0, 0 };
    r.d3 = *--r.p;
    size_t out = unpacked;               /* written backwards: dst[--out] */

    for (;;) {
        /* LAB_0198 : literal bytes */
        if (rnc_bit(&r)) {
            unsigned n = 0;
            if (rnc_bit(&r)) {
                int d1 = 3;
                for (;;) {
                    unsigned all = (1u << k_lit_bits[d1]) - 1;
                    n = rnc_bits(&r, k_lit_bits[d1]);
                    if (!d1 || n != all)
                        break;
                    d1--;
                }
                n = (n + k_lit_base[d1]) & 0xFFFF;
            }
            for (unsigned k = 0; k <= n; k++) {   /* LAB_019D */
                if (!out || r.p <= r.start)
                    return -1;
                dst[--out] = *--r.p;
            }
        }
        if (r.error)
            return -1;
        if (r.p <= r.start)
            break;

        /* LAB_01A3 : match length */
        int c = 3;
        while (c >= 0 && rnc_bit(&r))
            c--;
        unsigned i0  = (unsigned)(c + 1);
        unsigned len = k_len_bits[i0] ? rnc_bits(&r, k_len_bits[i0]) : 0;
        len = (len + k_len_base[i0]) & 0xFFFF;

        /* LAB_01AA : distance */
        unsigned dist;
        if (len == 2) {
            dist = rnc_bit(&r) ? rnc_bits(&r, 9) + 64 : rnc_bits(&r, 6);
        } else {
            int e = 1;
            while (e >= 0 && rnc_bit(&r))
                e--;
            unsigned j = (unsigned)(e + 1);
            dist = rnc_bits(&r, k_dist_bits[j] + 1) + k_dist_base[j];
        }
        dist &= 0xFFFF;
        if (r.error)
            return -1;

        /* LAB_0192 : copy len bytes from out + dist + len - 1, backwards;
         * distance 0 repeats the last byte written */
        size_t from = dist ? out + dist + len - 1 : out + 1;
        if (from > unpacked || len > out || from < len)
            return -1;
        for (unsigned k = 0; k < len; k++)
            dst[--out] = dst[--from];
    }

    /* LAB_01B2 : the output starts at the lowest byte written */
    size_t n = unpacked - out;
    if (out)
        memmove(dst, dst + out, n);
    return (int)n;
}
