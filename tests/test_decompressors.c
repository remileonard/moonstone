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

static void test_lzss_window(void)
{
    /* A back-reference 2 bytes before the output start reads the window */
    const uint8_t src[] = { 0x80, 0xF8, 0x02 };   /* offset 2, length 3 */
    uint8_t buf[8] = { 'P', 'Q' };
    int n = moon_lzss_decompress_window(src, sizeof(src), buf, 2, sizeof(buf));
    CHECK(n == 3, "lzss window: count");
    CHECK(buf[2] == 'P' && buf[3] == 'Q' && buf[4] == 'P', "lzss window: content");
    CHECK(moon_lzss_decompress(src, sizeof(src), buf, sizeof(buf)) == -1,
          "lzss: reference before the output is an error");
}

static void test_lzss_offset0(void)
{
    /* Offset 0 copies each byte onto itself: the output keeps its bytes */
    const uint8_t src[] = { 0x40, 'X', 0xF8, 0x00 };
    uint8_t dst[8] = { 0, 1, 2, 3, 4 };
    int n = moon_lzss_decompress(src, sizeof(src), dst, sizeof(dst));
    CHECK(n == 4, "lzss offset 0: count");
    CHECK(dst[0] == 'X' && dst[1] == 1 && dst[2] == 2 && dst[3] == 3,
          "lzss offset 0: bytes kept");
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
    printf("=== LZSS ===\n");
    test_lzss_all_literals();
    test_lzss_backref();
    test_lzss_empty();
    test_lzss_window();
    test_lzss_offset0();

    printf("\nPassed: %d  Failed: %d\n", g_passed, g_failed);
    return g_failed ? 1 : 0;
}
