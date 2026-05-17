/*
 * test_decompressors.c — unit tests for libmoon_assets decompressors.
 *
 * Tests each decompressor with hand-crafted inputs and verifies the output.
 * No external files are required.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_failed = 0;
static int g_passed = 0;

#define CHECK(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL [%s]: %s  (line %d)\n", __func__, msg, __LINE__); \
        g_failed++; \
    } else { \
        g_passed++; \
    } \
} while(0)

/* ------------------------------------------------------------------ */
/* PackBits tests                                                       */
/* ------------------------------------------------------------------ */

static void test_packbits_literal(void)
{
    /* 3 literal bytes: ctrl=2 (copy 3), then 'A','B','C' */
    const uint8_t src[] = { 0x02, 'A', 'B', 'C' };
    uint8_t dst[8] = {0};
    int n = moon_packbits_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 3, "literal: byte count");
    CHECK(dst[0] == 'A' && dst[1] == 'B' && dst[2] == 'C', "literal: content");
}

static void test_packbits_rle(void)
{
    /* Run of 4 'X': ctrl=-3 (1 - (-3) = 4), then 'X' */
    const uint8_t src[] = { (uint8_t)(-3), 'X' };
    uint8_t dst[8] = {0};
    int n = moon_packbits_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 4, "rle: byte count");
    CHECK(dst[0]=='X' && dst[1]=='X' && dst[2]=='X' && dst[3]=='X', "rle: content");
}

static void test_packbits_nop(void)
{
    /* NOP (0x80) followed by a single literal */
    const uint8_t src[] = { 0x80, 0x00, 'Z' };
    uint8_t dst[4] = {0};
    int n = moon_packbits_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 1, "nop: byte count");
    CHECK(dst[0] == 'Z', "nop: content");
}

static void test_packbits_empty(void)
{
    const uint8_t src[] = {0};
    uint8_t dst[4] = {0};
    int n = moon_packbits_decompress(src, 0, dst, sizeof(dst));
    CHECK(n == 0, "empty input");
}

/* ------------------------------------------------------------------ */
/* LZSS tests                                                          */
/* ------------------------------------------------------------------ */

static void test_lzss_all_literals(void)
{
    /*
     * Control byte 0x00 (all 8 bits = 0 → 8 literals) followed by
     * 8 literal bytes.
     */
    const uint8_t src[] = { 0x00, 1, 2, 3, 4, 5, 6, 7, 8 };
    uint8_t dst[16] = {0};
    int n = moon_lzss_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 8, "lzss literals: count");
    for (int i = 0; i < 8; i++)
        CHECK(dst[i] == (uint8_t)(i + 1), "lzss literals: value");
}

static void test_lzss_backref(void)
{
    /*
     * Write 4 literals, then a back-reference that copies 3 bytes from -2.
     *
     * Control byte 1: 0b00001111 (bits: MSB 0,0,0,0,1,1,1,1)
     *   - bits 7..4 = 0 → 4 literals: 'A','B','C','D'
     *   - bit  3    = 1 → back-ref
     *
     * Wait, control byte 0x0F = 0b00001111 means bits are processed MSB first:
     *   bit7=0: literal A
     *   bit6=0: literal B
     *   bit5=0: literal C
     *   bit4=0: literal D
     *   bit3=1: back-ref
     *   bit2=1: back-ref
     *   bit1=1: back-ref
     *   bit0=1: back-ref
     *
     * For a back-ref with offset=2, length=3:
     *   B1[7:3] = (34-3) = 31 → 0b11111000 = 0xF8
     *   B1[2:0] = offset >> 8 = 0  (offset < 256)
     *   B2 = offset & 0xFF = 2
     *   → B1 = 0xF8, B2 = 0x02
     *
     * After 4 literals 'ABCD', output is [A,B,C,D].
     * Back-ref offset=2, length=3: copy from out[-2] = 'C','D','C'
     *
     * Control byte: 0b00010000 = 0x10 → bit 3 = 1 (back-ref), rest literal
     * Actually let's build a simple test:
     *
     * ctrl = 0b1000_0000 (bit7=1 → back-ref, rest literal)
     * back-ref: B1=0xF8, B2=0x01 → offset=1, length=3 → fill with last byte
     *
     * But we need existing output for back-ref. Use 2 control bytes:
     * ctrl1 = 0b0100_0000: bit7=0 (literal), bit6=1 (back-ref)
     * → literal 'X', then back-ref B1=0xF8, B2=0x01 → offset=1, len=3 → 'X','X','X'
     */
    const uint8_t src[] = {
        0x40,           /* ctrl: bit7=0 (lit), bit6=1 (back-ref) */
        'X',            /* literal 'X' */
        0xF8, 0x01      /* back-ref: B1=0xF8 (len=34-31=3, hi-offset=0), B2=1 (offset=1) */
    };
    uint8_t dst[16] = {0};
    int n = moon_lzss_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 4, "lzss backref: count");
    CHECK(dst[0] == 'X', "lzss backref: literal");
    CHECK(dst[1] == 'X' && dst[2] == 'X' && dst[3] == 'X', "lzss backref: copied");
}

static void test_lzss_empty(void)
{
    uint8_t dst[4] = {0};
    int n = moon_lzss_decompress(NULL, 0, dst, sizeof(dst));
    CHECK(n == 0, "lzss empty input");
}

/* ------------------------------------------------------------------ */
/* RLE stile tests                                                     */
/* ------------------------------------------------------------------ */

static void test_rle_zero_fill(void)
{
    /*
     * Opcode 00 (zero fill): 2-bit opcode 00, then 5-bit count N.
     * Writes (N+4) zero bits.
     *
     * We pack: bits [1,0] = opcode 00, bits [00000] = N=0 → 4 zero bits.
     * Byte: 0b00_00000_? → first 7 bits = 0, last bit = ?
     * Byte = 0x00, bit_pos=7..0
     * But we only read 7 bits (2 opcode + 5 count), leaving 1 bit in the byte.
     *
     * For simplicity: byte 0x00 = 0b00000000
     *   - Read 2 bits (opcode): 00 = zero fill
     *   - Read 5 bits (count): 00000 = 0 → run = 4 bits
     *   → output: 4 zero bits = half a byte
     */
    const uint8_t src[] = { 0x00 };
    uint8_t dst[4] = { 0xFF, 0xFF, 0xFF, 0xFF };  /* pre-fill non-zero */
    int n = moon_rle_stile_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n >= 0, "rle zero fill: no error");
    if (n > 0) {
        /* First 4 bits of output should be 0 */
        CHECK((dst[0] & 0xF0) == 0x00, "rle zero fill: upper nibble zeroed");
    }
}

static void test_rle_literal(void)
{
    /*
     * Opcode 01 (literal 32 bits): copy 32 bits from stream.
     * Byte 0 = 0b01??????
     * Pack: opcode=01 (bits 7..6), then 32 data bits.
     *
     * 2 bits opcode + 32 data bits = 34 bits = 5 bytes (rounded up).
     *
     * Byte 0: 0b01_AAAAAA  (opcode=01, A=top 6 bits of data)
     * Bytes 1-4: remaining 26 bits + 6 padding bits
     *
     * Let data = 0xDEADBEEF = 0b11011110_10101101_10111110_11101111
     *
     * Pack into stream (MSB first):
     * bit7: opcode hi = 0
     * bit6: opcode lo = 1
     * bits 5..0: data[31..26] = 0b110111 = 0x37 (6 bits of 0xDE>>2)
     *
     * byte0 = 0b01_110111 = 0x77
     * byte1 = data[25..18] = 0b10_101101 (bits 25..18 of 0xDEADBEEF)
     *
     * Let me just use all 0xFF as data:
     * data = 32 bits of 1s
     *
     * byte0 = 0b01_111111 = 0x7F
     * bytes 1-4 = 0xFF, 0xFF, 0xFF, 0xFF (but only 26 more bits needed)
     * byte4's top 6 bits come from data, bottom 2 are padding
     *
     * byte0 = 0x7F (opcode=01, 6 bits of 1)
     * byte1 = 0xFF (8 bits of 1)
     * byte2 = 0xFF (8 bits of 1)
     * byte3 = 0xFF (8 bits of 1)
     * byte4 = 0b11111100 = 0xFC (6 bits of 1 from data, 2 bits padding)
     *
     * Total data bits read: 6+8+8+8+6 = 36? No: 32 bits from 5 bytes
     */

    /*
     * Simpler: build the stream bit by bit.
     * opcode=01: 2 bits → 0b01
     * data = 32 bits of 0b1010_1010_1010_1010_1010_1010_1010_1010
     * total = 34 bits = 5 bytes (with 6 padding bits at end)
     *
     * byte 0: 0b01_101010 = 0x6A
     * byte 1: 0b10101010 = 0xAA
     * byte 2: 0b10101010 = 0xAA
     * byte 3: 0b10101010 = 0xAA
     * byte 4: 0b10_000000 = 0x80 (last 2 bits of data + padding)
     */
    const uint8_t src[] = { 0x6A, 0xAA, 0xAA, 0xAA, 0x80 };
    uint8_t dst[8] = {0};
    int n = moon_rle_stile_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n >= 0, "rle literal: no error");
    if (n >= 4) {
        /* Expected output: 0xAA, 0xAA, 0xAA, 0xAA (32 alternating bits) */
        CHECK(dst[0] == 0xAA, "rle literal: byte 0");
        CHECK(dst[1] == 0xAA, "rle literal: byte 1");
        CHECK(dst[2] == 0xAA, "rle literal: byte 2");
        CHECK(dst[3] == 0xAA, "rle literal: byte 3");
    }
}

/* ------------------------------------------------------------------ */
/* Cache tests                                                         */
/* ------------------------------------------------------------------ */

/* (Cache test — see test_cache.c for dedicated cache unit tests) */

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("=== PackBits ===\n");
    test_packbits_literal();
    test_packbits_rle();
    test_packbits_nop();
    test_packbits_empty();

    printf("=== LZSS ===\n");
    test_lzss_all_literals();
    test_lzss_backref();
    test_lzss_empty();

    printf("=== RLE Stile ===\n");
    test_rle_zero_fill();
    test_rle_literal();

    printf("\nPassed: %d  Failed: %d\n", g_passed, g_failed);
    return g_failed ? 1 : 0;
}
