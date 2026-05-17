/*
 * lzss_cel.c — Mindscape LZSS decompressor for .cel / .f files
 *
 * Ported from LAB_049C in program.asm (Moonstone, Mindscape 1991).
 *
 * Algorithm:
 *   Read a control byte (8 bits, processed MSB-first).
 *   For each bit in the control byte:
 *     0 → literal: copy 1 byte from input to output.
 *     1 → back-reference:
 *           Read 2 bytes B1 and B2.
 *           offset = ((B1 & 0x07) << 8) | B2   (11-bit window offset, 1-based)
 *           length = 34 - (B1 >> 3)             (3..34 bytes)
 *           Copy `length` bytes from output[-offset] to output.
 *
 * Input format:
 *   The source buffer contains:
 *     word[0]  = frame_count (big-endian)  -- CEL header, consumed by caller
 *     long[1]  = data_offset               -- CEL header, consumed by caller
 *     ...      = the LZSS stream
 *
 * This function operates on the raw LZSS stream (after any headers are
 * stripped by the caller).
 */

#include "moon_assets.h"

int moon_lzss_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len)
{
    const uint8_t *src_end = src + src_len;
    uint8_t       *out     = dst;
    uint8_t       *out_end = dst + dst_len;
    const uint8_t *p       = src;

    while (p < src_end && out < out_end) {
        /* Read control byte — 8 flags, processed MSB first */
        uint8_t ctrl = *p++;
        int     bits = 8;

        while (bits-- > 0 && p < src_end && out < out_end) {
            if (ctrl & 0x80) {
                /* Back-reference */
                if (p + 2 > src_end)
                    return -1;

                uint8_t b1 = *p++;
                uint8_t b2 = *p++;

                int offset = ((b1 & 0x07) << 8) | b2;  /* 11-bit, 0-based */
                int length = 34 - (b1 >> 3);            /* 3..34 bytes */

                if (offset == 0) {
                    /* offset 0: fill with current output byte (uncommon) */
                    if (out == dst)
                        return -1;
                    uint8_t fill = *(out - 1);
                    for (int i = 0; i < length && out < out_end; i++)
                        *out++ = fill;
                } else {
                    /* Normal back-reference */
                    if (out - dst < offset)
                        return -1;  /* back-ref before start of output */
                    uint8_t *back = out - offset;
                    for (int i = 0; i < length && out < out_end; i++)
                        *out++ = back[i];
                }
            } else {
                /* Literal byte */
                *out++ = *p++;
            }

            ctrl <<= 1;
        }
    }

    return (int)(out - dst);
}
