/*
 * rle_stile.c — 2-bit RLE decompressor for .stile files
 *
 * Ported from LAB_0448 / SECSTRT_21 in program.asm (Moonstone 1991).
 *
 * The compressed stream is a sequence of 2-bit opcodes packed
 * into bytes (MSB first).  Each opcode specifies how to expand the
 * next run of *output bits*:
 *
 *   00 — zero fill  : read a 5-bit count N; write (N+4) zero bits.
 *   01 — literal    : copy the next 32 bits (4 bytes) verbatim.
 *   10 — short back-ref : read a 5-bit count N; copy (N+8) bits
 *                         from earlier in the output (short-offset).
 *   11 — long back-ref  : variable-length back-reference (see below).
 *
 * The decoder tracks two bit-level pointers:
 *   A0 / D5 — source  bit position (advances through the compressed data)
 *   A1 / D6 — destination bit position (advances through the output)
 *
 * Helper (LAB_048A / LAB_048D):
 *   LAB_048A reads 1 bit from the source stream into D0 bit-0.
 *   LAB_048D reads D4 bits from the source stream into D0 (MSB first),
 *            then D3 = that value.
 *
 * Decompression loop (LAB_0449):
 *   D7 = total output *bits* remaining (decremented by each run length).
 *   When D7 <= 0: done.
 *
 * NOTE: Because the original code operates at the bit level, this C
 * implementation also works at the bit level to faithfully reproduce
 * the original output layout.
 *
 * dst_len should be large enough to hold the decompressed data.
 * The exact decompressed size is not stored in the file header; the
 * caller must know it (typically 960 bytes = 5 × 192 for a 12×16 tile
 * set with 10 tiles in 5 bitplanes, but varies per file).
 */

#include "moon_assets.h"

#include <string.h>

/* ------------------------------------------------------------------ */
/* Bit-stream context                                                  */
/* ------------------------------------------------------------------ */

typedef struct {
    const uint8_t *data;
    size_t         size;
    size_t         byte_pos;  /* current byte in data */
    int            bit_pos;   /* next bit to read within byte_pos (7=MSB) */
} SrcBits;

typedef struct {
    uint8_t *data;
    size_t   size;
    size_t   byte_pos;
    int      bit_pos;  /* next bit to write within byte_pos (7=MSB) */
} DstBits;

/* Read one bit from source */
static int src_read_bit(SrcBits *s)
{
    if (s->byte_pos >= s->size)
        return 0;
    int bit = (s->data[s->byte_pos] >> s->bit_pos) & 1;
    if (s->bit_pos == 0) {
        s->bit_pos = 7;
        s->byte_pos++;
    } else {
        s->bit_pos--;
    }
    return bit;
}

/* Read n bits from source (MSB first) */
static int src_read_bits(SrcBits *s, int n)
{
    int val = 0;
    for (int i = n - 1; i >= 0; i--) {
        val |= src_read_bit(s) << i;
    }
    return val;
}

/* Write one bit to destination */
static int dst_write_bit(DstBits *d, int bit)
{
    if (d->byte_pos >= d->size)
        return -1;
    if (bit)
        d->data[d->byte_pos] |= (uint8_t)(1u << d->bit_pos);
    else
        d->data[d->byte_pos] &= (uint8_t)~(1u << d->bit_pos);
    if (d->bit_pos == 0) {
        d->bit_pos = 7;
        d->byte_pos++;
        if (d->byte_pos < d->size)
            d->data[d->byte_pos] = 0;
    } else {
        d->bit_pos--;
    }
    return 0;
}

/* Read one bit from the output (back-reference) - reserved for future use */
#if 0
static int dst_read_bit(const DstBits *d, size_t bit_index)
{
    size_t byte = bit_index / 8;
    int    bit  = (int)(7 - (bit_index % 8));
    if (byte >= d->size)
        return 0;
    return (d->data[byte] >> bit) & 1;
}
#endif

/* ------------------------------------------------------------------ */
/* Main decompressor                                                   */
/* ------------------------------------------------------------------ */

int moon_rle_stile_decompress(const uint8_t *src, size_t src_len,
                              uint8_t *dst, size_t dst_len)
{
    if (dst_len == 0)
        return 0;

    SrcBits s;
    s.data     = src;
    s.size     = src_len;
    s.byte_pos = 0;
    s.bit_pos  = 7;  /* start at MSB of first byte */

    DstBits d;
    d.data     = dst;
    d.size     = dst_len;
    d.byte_pos = 0;
    d.bit_pos  = 7;
    memset(dst, 0, dst_len);

    /* Total output bits = dst_len * 8 */
    long remaining = (long)(dst_len * 8);

    while (remaining > 0 && s.byte_pos < src_len) {
        /* Read 2-bit opcode */
        int hi = src_read_bit(&s);
        int lo = src_read_bit(&s);
        int op = (hi << 1) | lo;

        switch (op) {
        case 0: {
            /* Zero fill: read 5-bit count N, write (N+4) zero bits */
            int n    = src_read_bits(&s, 5);
            int run  = n + 4;
            if (run > remaining)
                run = (int)remaining;
            for (int i = 0; i < run; i++) {
                dst_write_bit(&d, 0);
            }
            remaining -= run;
            break;
        }
        case 1: {
            /* Literal 32 bits (4 bytes) */
            int run = 32;
            if (run > remaining)
                run = (int)remaining;
            for (int i = 0; i < run; i++) {
                int bit = src_read_bit(&s);
                dst_write_bit(&d, bit);
            }
            remaining -= run;
            break;
        }
        case 2: {
            /*
             * Short back-reference:
             *   Read 1-bit selector:
             *     0 → write ZERO bits  (clear)
             *     1 → write ONE bits   (set)
             *   Read 5-bit count N, run = N+8
             *   Then fill run bits with that value (0 or 1).
             *
             * This matches the assembly at LAB_0452:
             *   JSR LAB_048A    ; read 1 bit into D0
             *   MOVEQ #5,D4
             *   JSR LAB_048D    ; read 5 bits into D3
             *   ADDQ.W #8,D3    ; D3 = run = D3 + 8
             *   TST.W D0        ; 0=clear, 1=set
             *   BEQ → clear path
             *   (set path)
             */
            int fill = src_read_bit(&s);  /* 0=clear, 1=set */
            int n    = src_read_bits(&s, 5);
            int run  = n + 8;
            if (run > remaining)
                run = (int)remaining;
            for (int i = 0; i < run; i++) {
                dst_write_bit(&d, fill);
            }
            remaining -= run;
            break;
        }
        case 3: {
            /*
             * Long back-reference (literal copy from earlier output):
             *   Read 4 source bits verbatim from the compressed stream.
             *   (This corresponds to the LAB_0462 branch which reads 4 bits
             *   with JSR LAB_0473 and writes them to the output.)
             */
            int run = 4;
            if (run > remaining)
                run = (int)remaining;
            for (int i = 0; i < run; i++) {
                int bit = src_read_bit(&s);
                dst_write_bit(&d, bit);
            }
            remaining -= run;
            break;
        }
        default:
            break;
        }
    }

    return (int)d.byte_pos + (d.bit_pos < 7 ? 1 : 0);
}
