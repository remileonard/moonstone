/*
 * test_cache.c — unit tests for the libmoon_assets library lifecycle and loaders.
 *
 * Tests moon_init/moon_shutdown, moon_file_read, moon_ob_load, and
 * moon_mod_load using synthetic data (no real game files needed).
 *
 * Note: asset caching is not part of the library; these tests verify
 * that each load call returns a valid, independently-owned object.
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
/* moon_init / moon_shutdown basic lifecycle                           */
/* ------------------------------------------------------------------ */

static void test_init_shutdown(void)
{
    int r = moon_init("/tmp");
    CHECK(r == 0, "moon_init returns 0");
    moon_shutdown();
    /* Double shutdown should be safe */
    moon_shutdown();
}

static void test_init_null(void)
{
    int r = moon_init(NULL);
    CHECK(r == -1, "moon_init(NULL) returns -1");
}

/* ------------------------------------------------------------------ */
/* moon_file_read                                                      */
/* ------------------------------------------------------------------ */

static void test_file_read(void)
{
    /* Write a small temp file and read it back */
    const char *tmpfile = "/tmp/moon_test_asset.bin";
    const char content[] = "hello moonstone\n";

    FILE *f = fopen(tmpfile, "wb");
    CHECK(f != NULL, "create temp file");
    if (f) {
        fwrite(content, 1, sizeof(content) - 1, f);
        fclose(f);
    }

    moon_init("/tmp");

    size_t sz = 0;
    uint8_t *buf = moon_file_read("moon_test_asset.bin", &sz);
    CHECK(buf != NULL, "file_read: buffer non-null");
    CHECK(sz == sizeof(content) - 1, "file_read: size matches");
    if (buf) {
        CHECK(memcmp(buf, content, sz) == 0, "file_read: content matches");
        free(buf);
    }

    moon_shutdown();
    remove(tmpfile);
}

/* ------------------------------------------------------------------ */
/* moon_ob_load — minimal valid CEL/OB format file                    */
/* ------------------------------------------------------------------ */

static void test_ob_load(void)
{
    const char *tmpfile = "/tmp/moon_test.ob";

    /*
     * Build a minimal valid OB file (same format as CEL):
     *   Header (10 bytes): frame_count=1, comp_size=18, reserved=0
     *   Frame table (10 bytes): offset=0, width=8, height=1,
     *                           toggle_flags=0, planes_mask=0x1F (5 planes)
     *   LZSS body (18 bytes): two control blocks of 8 zero literals each
     *     → decompresses to 16 bytes; frame needs 5*2*1 = 10 bytes.
     */
    static const uint8_t data[] = {
        /* global header */
        0x00, 0x01,             /* frame_count = 1 */
        0x00, 0x00, 0x00, 0x12, /* comp_size = 18 */
        0x00, 0x00, 0x00, 0x00, /* reserved */
        /* frame table entry 0 */
        0x00, 0x00, 0x00, 0x00, /* pixel_data_offset = 0 */
        0x00, 0x08,             /* width = 8 */
        0x00, 0x01,             /* height = 1 */
        0x00,                   /* toggle_flags = 0 */
        0x1F,                   /* planes_mask = 0x1F (5 planes) */
        /* LZSS body (18 bytes): 2 × (ctrl=0x00 + 8 zero literals) */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    FILE *f = fopen(tmpfile, "wb");
    CHECK(f != NULL, "create ob temp file");
    if (f) { fwrite(data, 1, sizeof(data), f); fclose(f); }

    moon_init("/tmp");

    MoonOb *ob1 = moon_ob_load("moon_test.ob");
    CHECK(ob1 != NULL, "ob_load: non-null");
    if (ob1) {
        CHECK(ob1->frame_count == 1, "ob_load: frame_count");
        CHECK(ob1->frames != NULL, "ob_load: frames non-null");
        if (ob1->frames) {
            CHECK(ob1->frames[0].width  == 8, "ob_load: frame width");
            CHECK(ob1->frames[0].height == 1, "ob_load: frame height");
            CHECK(ob1->frames[0].planes == 5, "ob_load: frame planes");
        }
    }

    /* Load again — each call returns an independent object */
    MoonOb *ob2 = moon_ob_load("moon_test.ob");
    CHECK(ob2 != NULL, "ob_load second call: non-null");
    if (ob2) {
        CHECK(ob2->frame_count == 1, "ob_load second call: frame_count");
    }

    moon_ob_free(ob2);
    moon_ob_free(ob1);

    moon_shutdown();
    remove(tmpfile);
}

/* ------------------------------------------------------------------ */
/* moon_mod_load with a minimal synthetic ProTracker MOD              */
/* ------------------------------------------------------------------ */

static void test_mod_load_raw(void)
{
    const char *tmpfile = "/tmp/moon_test.mod";

    /*
     * Build a minimal valid ProTracker MOD:
     *   - 20-byte title "TEST MOD"
     *   - 31 × 30-byte sample headers (all zero → empty samples)
     *   - song_length = 1, restart_pos = 0
     *   - order[0] = 0, order[1..127] = 0  → 1 pattern referenced
     *   - magic "M.K."
     *   - 1 pattern (1024 zero bytes)
     *   Total: 1084 + 1024 = 2108 bytes
     */
    const size_t MOD_SIZE = 1084 + 1024;
    uint8_t *data = (uint8_t *)calloc(1, MOD_SIZE);
    if (!data) { fprintf(stderr, "SKIP test_mod_load_raw: alloc failed\n"); return; }

    /* Title */
    memcpy(data, "TEST MOD", 8);
    /* song_length = 1 */
    data[950] = 1;
    /* order[0] = 0 (already zero) */
    /* magic "M.K." at offset 1080 */
    data[1080] = 'M'; data[1081] = '.'; data[1082] = 'K'; data[1083] = '.';
    /* pattern data: already zero */

    FILE *f = fopen(tmpfile, "wb");
    CHECK(f != NULL, "create mod temp file");
    if (f) { fwrite(data, 1, MOD_SIZE, f); fclose(f); }
    free(data);

    moon_init("/tmp");

    MoonMod *mod = moon_mod_load("moon_test.mod");
    CHECK(mod != NULL, "mod_load: non-null");
    if (mod) {
        CHECK(strncmp(mod->title, "TEST MOD", 8) == 0, "mod_load: title");
        CHECK(mod->song_length   == 1,  "mod_load: song_length");
        CHECK(mod->pattern_count == 1,  "mod_load: pattern_count");
        CHECK(mod->sample_count  == 31, "mod_load: sample_count");
        CHECK(mod->pattern_data  != NULL, "mod_load: pattern_data non-null");
        /* Load again — each call returns an independent object */
        MoonMod *mod2 = moon_mod_load("moon_test.mod");
        CHECK(mod2 != NULL, "mod_load second call: non-null");
        if (mod2) {
            CHECK(mod2->song_length == 1, "mod_load second call: song_length");
            moon_mod_free(mod2);
        }
        moon_mod_free(mod);
    }

    moon_shutdown();
    remove(tmpfile);
}

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

int main(void)
{
    printf("=== Cache / lifecycle ===\n");
    test_init_shutdown();
    test_init_null();

    printf("=== File I/O ===\n");
    test_file_read();

    printf("=== OB loader ===\n");
    test_ob_load();

    printf("=== MOD loader ===\n");
    test_mod_load_raw();

    printf("\nPassed: %d  Failed: %d\n", g_passed, g_failed);
    return g_failed ? 1 : 0;
}
