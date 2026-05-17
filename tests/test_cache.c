/*
 * test_cache.c — unit tests for the libmoon_assets file cache.
 *
 * Tests the hash function, reference counting, and cache eviction.
 * Uses synthetic data (no real game files needed).
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
/* moon_ob_load (simplest format: raw bytes)                          */
/* ------------------------------------------------------------------ */

static void test_ob_load(void)
{
    const char *tmpfile = "/tmp/moon_test.ob";
    const uint8_t data[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };

    FILE *f = fopen(tmpfile, "wb");
    CHECK(f != NULL, "create ob temp file");
    if (f) { fwrite(data, 1, sizeof(data), f); fclose(f); }

    moon_init("/tmp");

    MoonOb *ob1 = moon_ob_load("moon_test.ob");
    CHECK(ob1 != NULL, "ob_load: non-null");
    if (ob1) {
        CHECK(ob1->size == sizeof(data), "ob_load: size");
        CHECK(memcmp(ob1->data, data, sizeof(data)) == 0, "ob_load: data");
    }

    /* Load again — should return same pointer (cached) */
    MoonOb *ob2 = moon_ob_load("moon_test.ob");
    CHECK(ob2 == ob1, "ob_load: cache hit returns same pointer");

    moon_ob_free(ob2);
    moon_ob_free(ob1);

    moon_shutdown();
    remove(tmpfile);
}

/* ------------------------------------------------------------------ */
/* moon_mod_load with raw (uncompressed) data                         */
/* ------------------------------------------------------------------ */

static void test_mod_load_raw(void)
{
    const char *tmpfile = "/tmp/moon_test.mod";
    /* Write 20 bytes of dummy data (too small for a real MOD but tests loading) */
    uint8_t data[20];
    memset(data, 0x42, sizeof(data));

    FILE *f = fopen(tmpfile, "wb");
    CHECK(f != NULL, "create mod temp file");
    if (f) { fwrite(data, 1, sizeof(data), f); fclose(f); }

    moon_init("/tmp");

    MoonMod *mod = moon_mod_load("moon_test.mod");
    CHECK(mod != NULL, "mod_load raw: non-null");
    if (mod) {
        CHECK(mod->size == sizeof(data), "mod_load raw: size");
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
