/*
 * lzss_cel.c — Mindscape LZSS decompressor (.cel, .ob, .c, .f, .PIV, .t)
 *
 * Port of LAB_049C in program.asm / LAB_0CC2 in mog.asm (Moonstone,
 * Mindscape 1991).
 *
 * Algorithm:
 *   Read a control byte; its 8 bits (MSB first) announce 8 tokens.
 *   The end of the input is tested before each token.
 *     0 → literal: copy 1 byte from input to output.
 *     1 → back-reference: read the big-endian word W;
 *           offset = W & 0x7FF, length = 34 - (W >> 11)  (3..34 bytes);
 *           copy `length` bytes, one at a time, from output - offset.
 *
 * The original writes straight into memory: a back-reference may reach
 * bytes that precede the output (moon_lzss_decompress_window gives them),
 * and offset 0 copies each byte onto itself (the output keeps what it
 * held).
 */

#include "moon_assets.h"

int moon_lzss_decompress_window(const uint8_t *src, size_t src_len,
                                uint8_t *buf, size_t start, size_t buf_len)
{
    const uint8_t *p   = src;
    const uint8_t *end = src + src_len;
    size_t         out = start;

    if (start > buf_len)
        return -1;
    while (p < end) {
        uint8_t ctrl = *p++;
        for (int bits = 0; bits < 8 && p < end; bits++, ctrl <<= 1) {
            if (!(ctrl & 0x80)) {
                if (out >= buf_len)
                    return -1;
                buf[out++] = *p++;
                continue;
            }
            if (p + 2 > end)
                return -1;
            unsigned w      = (unsigned)p[0] << 8 | p[1];
            size_t   offset = w & 0x7FF;
            size_t   length = 34 - (w >> 11);
            p += 2;
            if (offset > out || out + length > buf_len)
                return -1;
            for (size_t i = 0; i < length; i++, out++)
                buf[out] = buf[out - offset];
        }
    }
    return (int)(out - start);
}

int moon_lzss_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len)
{
    return moon_lzss_decompress_window(src, src_len, dst, 0, dst_len);
}
