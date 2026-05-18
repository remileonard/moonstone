/*
 * moon_assets.c — main library: file cache, API implementation
 *
 * Implements moon_init/moon_shutdown and all moon_*_load/free functions.
 *
 * File cache:
 *   Uses a hash table with 72 buckets (same as the original SECSTRT_19
 *   cache in program.asm §6.1.2).  The hash function mirrors the
 *   Mindscape implementation:
 *       h = (h * 13 + c) & 0x07FF
 *       bucket = h % 72
 *   Each bucket holds a linked list of CacheEntry nodes.
 *   Assets are reference-counted; moon_*_free() decrements the count
 *   and frees when it reaches zero.
 */

#include "moon_assets.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Declare the function defined in packbits_piv.c */
MoonPiv *moon_piv_load_from_buffer(const uint8_t *buf, size_t len);

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

#define CACHE_BUCKETS 72
#define NAME_MAX_LEN  64

/* Asset type tag for the cache */
typedef enum {
    ASSET_CEL   = 1,
    ASSET_PIV   = 2,
    ASSET_STILE = 3,
    ASSET_MOD   = 4,
    ASSET_OB    = 5,
} AssetType;

typedef struct CacheEntry {
    char                name[NAME_MAX_LEN];
    AssetType           type;
    void               *asset;
    int                 refcount;
    struct CacheEntry  *next;
} CacheEntry;

static struct {
    char         asset_dir[512];
    CacheEntry  *buckets[CACHE_BUCKETS];
    int          initialised;
} g_ctx;

/* ------------------------------------------------------------------ */
/* Hash function (mirrors Mindscape SECSTRT_19)                        */
/* ------------------------------------------------------------------ */

static int cache_hash(const char *name)
{
    unsigned int h = 0;
    for (const char *p = name; *p; p++) {
        unsigned char c = (unsigned char)tolower((unsigned char)*p);
        h = ((h * 13u) + c) & 0x07FFu;
    }
    return (int)(h % CACHE_BUCKETS);
}

/* ------------------------------------------------------------------ */
/* Cache lookup / insert                                               */
/* ------------------------------------------------------------------ */

static CacheEntry *cache_find(const char *name, AssetType type)
{
    int bucket = cache_hash(name);
    for (CacheEntry *e = g_ctx.buckets[bucket]; e; e = e->next) {
        if (e->type == type && strcmp(e->name, name) == 0)
            return e;
    }
    return NULL;
}

static CacheEntry *cache_insert(const char *name, AssetType type, void *asset)
{
    CacheEntry *e = (CacheEntry *)calloc(1, sizeof(CacheEntry));
    if (!e)
        return NULL;
    strncpy(e->name, name, NAME_MAX_LEN - 1);
    e->type     = type;
    e->asset    = asset;
    e->refcount = 1;

    int bucket = cache_hash(name);
    e->next = g_ctx.buckets[bucket];
    g_ctx.buckets[bucket] = e;
    return e;
}

static void cache_remove(const char *name, AssetType type)
{
    int bucket = cache_hash(name);
    CacheEntry **prev = &g_ctx.buckets[bucket];
    for (CacheEntry *e = *prev; e; prev = &e->next, e = e->next) {
        if (e->type == type && strcmp(e->name, name) == 0) {
            *prev = e->next;
            free(e);
            return;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Library lifecycle                                                   */
/* ------------------------------------------------------------------ */

int moon_init(const char *asset_dir)
{
    if (!asset_dir)
        return -1;
    memset(&g_ctx, 0, sizeof(g_ctx));
    strncpy(g_ctx.asset_dir, asset_dir, sizeof(g_ctx.asset_dir) - 1);
    /* Remove trailing slash if present */
    size_t len = strlen(g_ctx.asset_dir);
    if (len > 0 && (g_ctx.asset_dir[len - 1] == '/' ||
                    g_ctx.asset_dir[len - 1] == '\\'))
        g_ctx.asset_dir[len - 1] = '\0';
    g_ctx.initialised = 1;
    return 0;
}

static void free_asset(AssetType type, void *asset)
{
    if (!asset)
        return;
    switch (type) {
    case ASSET_CEL: {
        MoonCel *c = (MoonCel *)asset;
        for (int i = 0; i < c->frame_count; i++)
            free(c->frames[i].data);
        free(c->frames);
        free(c);
        break;
    }
    case ASSET_PIV: {
        MoonPiv *p = (MoonPiv *)asset;
        free(p->bitmap);
        free(p);
        break;
    }
    case ASSET_STILE: {
        MoonStile *s = (MoonStile *)asset;
        free(s->data);
        free(s);
        break;
    }
    case ASSET_MOD: {
        MoonMod *m = (MoonMod *)asset;
        free(m->pattern_data);
        for (int i = 0; i < m->sample_count; i++)
            free(m->samples[i].data);
        free(m);
        break;
    }
    case ASSET_OB: {
        MoonCel *c = (MoonCel *)asset;   /* MoonOb has identical layout */
        for (int i = 0; i < c->frame_count; i++)
            free(c->frames[i].data);
        free(c->frames);
        free(c);
        break;
    }
    }
}

void moon_shutdown(void)
{
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        CacheEntry *e = g_ctx.buckets[i];
        while (e) {
            CacheEntry *next = e->next;
            free_asset(e->type, e->asset);
            free(e);
            e = next;
        }
        g_ctx.buckets[i] = NULL;
    }
    g_ctx.initialised = 0;
}

/* ------------------------------------------------------------------ */
/* File I/O helper                                                     */
/* ------------------------------------------------------------------ */

uint8_t *moon_file_read(const char *name, size_t *out_size)
{
    char path[768];
    if (g_ctx.asset_dir[0]) {
        snprintf(path, sizeof(path), "%s/%s", g_ctx.asset_dir, name);
    } else {
        strncpy(path, name, sizeof(path) - 1);
    }

    FILE *f = fopen(path, "rb");
    if (!f) {
        /* Try lower-case name */
        char lower[NAME_MAX_LEN];
        strncpy(lower, name, NAME_MAX_LEN - 1);
        for (char *p = lower; *p; p++)
            *p = (char)tolower((unsigned char)*p);
        if (g_ctx.asset_dir[0])
            snprintf(path, sizeof(path), "%s/%s", g_ctx.asset_dir, lower);
        else
            strncpy(path, lower, sizeof(path) - 1);
        f = fopen(path, "rb");
    }
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }

    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    if ((long)fread(buf, 1, (size_t)sz, f) != sz) {
        free(buf);
        fclose(f);
        return NULL;
    }
    fclose(f);

    if (out_size)
        *out_size = (size_t)sz;
    return buf;
}

/* ------------------------------------------------------------------ */
/* CEL loader                                                          */
/* ------------------------------------------------------------------ */

/*
 * CEL file format (observed from LAB_04A8 / LAB_049C in program.asm,
 * confirmed by §6.2.3 of DOC_TECHNIQUE.md):
 *
 * Global header (10 bytes):
 *   word[0]   = frame_count  (number of animation frames)
 *   long[1]   = data_offset  (byte offset to start of LZSS-compressed frame data)
 *   byte[6..9]= reserved     (4 padding bytes)
 *
 * frames × (per-frame-header, 11 bytes each):
 *     long  = offset_from_data_start (byte offset into decompressed pixel data)
 *     word  = width   (pixels)
 *     word  = height  (rows)
 *     byte  = planes_or_flags  (bitmask of active bitplanes)
 *     byte  = draw_flags       (& 0x01 = "cached" toggle)
 *     byte  = blit_minterm     (blitter minterm byte)
 *
 * All values are big-endian.
 *
 * After the header array, starting at data_offset, the LZSS-compressed pixel
 * data begins as a single stream; the per-frame offsets index into the
 * decompressed output.  Pixel data is planar, 5 planes, interleaved row-by-row.
 */

static MoonCel *cel_decode(const uint8_t *buf, size_t len)
{
    if (len < 10)
        return NULL;

    /*
     * Global header (10 bytes, program.asm lines 8596-8604):
     *   word[0..1]  = frame_count
     *   long[2..5]  = compressed pixel data size (bytes) — NOT an offset
     *   long[6..9]  = reserved / unknown
     *
     * Frame table starts at offset 10; each entry is 10 bytes
     * (MULU #$000a,D0 at lines 8606/8638/8645/8663/8690):
     *   long [0..3] = pixel_data_offset (into decompressed pixel buffer)
     *   word [4..5] = width  (pixels)
     *   word [6..7] = height (rows)
     *   byte [8]    = toggle_flags (bit 0 is the draw-toggle)
     *   byte [9]    = planes_mask  (bit n = 1 → bitplane n is active)
     *
     * Compressed pixel data begins at offset 10 + frame_count * 10.
     */
    uint16_t frame_count = (uint16_t)((buf[0] << 8) | buf[1]);
    uint32_t comp_size   = ((uint32_t)buf[2] << 24) | ((uint32_t)buf[3] << 16) |
                           ((uint32_t)buf[4] <<  8) |  (uint32_t)buf[5];

    if (frame_count == 0)
        return NULL;

    /* Validate frame table fits in file */
    size_t frame_table_end = (size_t)10 + (size_t)frame_count * 10;
    if (frame_table_end > len)
        return NULL;

    /* Locate compressed data */
    const uint8_t *comp_data = buf + frame_table_end;
    size_t comp_avail = len - frame_table_end;
    if (comp_size > (uint32_t)comp_avail)
        comp_size = (uint32_t)comp_avail;   /* clamp to available bytes */

    /* Estimate decompressed size: actual size is stored in comp_size but
     * that is the COMPRESSED size; decompressed can be up to ~4× larger. */
    size_t decomp_max = (size_t)comp_size * 4 + 65536;
    uint8_t *pixels = (uint8_t *)malloc(decomp_max);
    if (!pixels)
        return NULL;

    int decomp_len = moon_lzss_decompress(comp_data, (size_t)comp_size,
                                          pixels, decomp_max);
    if (decomp_len < 0) {
        free(pixels);
        return NULL;
    }

    MoonCel *cel = (MoonCel *)calloc(1, sizeof(MoonCel));
    if (!cel) {
        free(pixels);
        return NULL;
    }
    cel->frame_count = (int)frame_count;
    cel->frames = (MoonCelFrame *)calloc((size_t)frame_count, sizeof(MoonCelFrame));
    if (!cel->frames) {
        free(pixels);
        free(cel);
        return NULL;
    }

    for (int i = 0; i < (int)frame_count; i++) {
        const uint8_t *m = buf + 10 + (size_t)i * 10;   /* stride = 10 */
        uint32_t frame_off   = ((uint32_t)m[0] << 24) | ((uint32_t)m[1] << 16) |
                               ((uint32_t)m[2] <<  8) |  (uint32_t)m[3];
        uint16_t w           = (uint16_t)((m[4] << 8) | m[5]);
        uint16_t h           = (uint16_t)((m[6] << 8) | m[7]);
        uint8_t  toggle_flags = m[8];   /* bit 0 = draw toggle */
        uint8_t  planes_mask  = m[9];   /* bit n = bitplane n active */

        MoonCelFrame *f = &cel->frames[i];
        f->width      = w;
        f->height     = h;
        f->draw_flags = toggle_flags;
        f->minterm    = 0;   /* not stored in this format */

        /*
         * Count active bitplanes from the mask (bits 0..4 checked by
         * LAB_04AB: LSR.W #1,D6; BCS.S LAB_04AC; DBF D7,LAB_04AB).
         */
        {
            int p = 0;
            for (int b = 0; b < 5; b++)
                if (planes_mask & (1u << b)) p++;
            f->planes = (p >= 1 && p <= 5) ? (uint8_t)p : 5;
        }

        /* Row stride per plane, word-aligned */
        int row_bytes  = ((w + 15) / 16) * 2;
        size_t frame_size = (size_t)f->planes * (size_t)row_bytes * (size_t)h;

        if (frame_off + frame_size > (size_t)decomp_len) {
            /* Clamp gracefully for oversized references */
            frame_size = (frame_off < (size_t)decomp_len)
                         ? (size_t)decomp_len - frame_off : 0;
        }

        f->data = (uint8_t *)calloc(1, frame_size ? frame_size : 1);
        if (f->data && frame_size)
            memcpy(f->data, pixels + frame_off, frame_size);
    }

    free(pixels);
    return cel;
}

MoonCel *moon_cel_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    CacheEntry *e = cache_find(name, ASSET_CEL);
    if (e) {
        e->refcount++;
        return (MoonCel *)e->asset;
    }

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonCel *cel = cel_decode(buf, len);
    free(buf);
    if (!cel)
        return NULL;

    cache_insert(name, ASSET_CEL, cel);
    return cel;
}

void moon_cel_free(MoonCel *cel)
{
    if (!cel)
        return;
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        for (CacheEntry *e = g_ctx.buckets[i]; e; e = e->next) {
            if (e->type == ASSET_CEL && e->asset == cel) {
                if (--e->refcount <= 0) {
                    cache_remove(e->name, ASSET_CEL);
                    free_asset(ASSET_CEL, cel);
                }
                return;
            }
        }
    }
    /* Not in cache — free directly */
    free_asset(ASSET_CEL, cel);
}

/* ------------------------------------------------------------------ */
/* PIV loader                                                          */
/* ------------------------------------------------------------------ */

MoonPiv *moon_piv_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    CacheEntry *e = cache_find(name, ASSET_PIV);
    if (e) {
        e->refcount++;
        return (MoonPiv *)e->asset;
    }

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonPiv *piv = moon_piv_load_from_buffer(buf, len);
    free(buf);
    if (!piv)
        return NULL;

    cache_insert(name, ASSET_PIV, piv);
    return piv;
}

void moon_piv_free(MoonPiv *piv)
{
    if (!piv)
        return;
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        for (CacheEntry *e = g_ctx.buckets[i]; e; e = e->next) {
            if (e->type == ASSET_PIV && e->asset == piv) {
                if (--e->refcount <= 0) {
                    cache_remove(e->name, ASSET_PIV);
                    free_asset(ASSET_PIV, piv);
                }
                return;
            }
        }
    }
    free_asset(ASSET_PIV, piv);
}

/* ------------------------------------------------------------------ */
/* STILE loader                                                        */
/* ------------------------------------------------------------------ */

MoonStile *moon_stile_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    CacheEntry *e = cache_find(name, ASSET_STILE);
    if (e) {
        e->refcount++;
        return (MoonStile *)e->asset;
    }

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    /* Stile files have a small header; the compressed data follows directly */
    size_t decomp_max = len * 8 + 4096; /* RLE can expand significantly */
    uint8_t *data = (uint8_t *)calloc(1, decomp_max);
    if (!data) {
        free(buf);
        return NULL;
    }

    int written = moon_rle_stile_decompress(buf, len, data, decomp_max);
    free(buf);
    if (written < 0) {
        free(data);
        return NULL;
    }

    MoonStile *stile = (MoonStile *)calloc(1, sizeof(MoonStile));
    if (!stile) {
        free(data);
        return NULL;
    }
    stile->size = (size_t)written;
    stile->data = data;

    cache_insert(name, ASSET_STILE, stile);
    return stile;
}

void moon_stile_free(MoonStile *stile)
{
    if (!stile)
        return;
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        for (CacheEntry *e = g_ctx.buckets[i]; e; e = e->next) {
            if (e->type == ASSET_STILE && e->asset == stile) {
                if (--e->refcount <= 0) {
                    cache_remove(e->name, ASSET_STILE);
                    free_asset(ASSET_STILE, stile);
                }
                return;
            }
        }
    }
    free_asset(ASSET_STILE, stile);
}

/* ------------------------------------------------------------------ */
/* MOD / CMP loader                                                    */
/* ------------------------------------------------------------------ */

/*
 * ProTracker MOD format (31-sample, 4-channel) as used by Moonstone:
 *
 *   Offset    0 : title (20 bytes, null-padded)
 *   Offset   20 : sample headers, 31 × 30 bytes each:
 *                   name[22], length(u16), finetune(u8), volume(u8),
 *                   repeat_offset(u16), repeat_length(u16)
 *   Offset  950 : song_length (u8)  — number of valid entries in order[]
 *   Offset  951 : restart_position (u8)
 *   Offset  952 : order[128] (u8 each)
 *   Offset 1080 : magic "M.K." (4 bytes)
 *   Offset 1084 : pattern data, (max_pattern+1) × 64 rows × 4 ch × 4 B
 *   After patterns: sample PCM data (signed 8-bit), concatenated
 *
 * Confirmed by program.asm LAB_0061:
 *   ADDA.L #$0003B8,A1  → A1 points to order[0] at offset 952
 *   ADDI.L #$00043C,D2  → pattern data starts at 1084 = 952+128+4
 *   MOVEQ  #30,D0       → loop for 31 sample descriptors
 *   ADDA.L #$1E,A0      → descriptor stride = 30 bytes
 */

#define MOD_TITLE_LEN       20
#define MOD_SAMPLE_COUNT    31
#define MOD_SAMPLE_DESCR    30   /* bytes per sample descriptor */
#define MOD_SAMPLE_NAME     22   /* bytes of name in descriptor  */
#define MOD_SONG_LEN_OFF   950
#define MOD_RESTART_OFF    951
#define MOD_ORDER_OFF      952
#define MOD_ORDER_COUNT    128
#define MOD_MAGIC_OFF     1080
#define MOD_HEADER_SIZE   1084  /* = 1080 + 4 (magic) */
#define MOD_PATTERN_BYTES 1024  /* 64 rows × 4 channels × 4 bytes */

static MoonMod *mod_decode(const uint8_t *data, size_t len)
{
    if (len < MOD_HEADER_SIZE)
        return NULL;

    /* Determine pattern count from the order table */
    uint8_t song_length = data[MOD_SONG_LEN_OFF];
    if (song_length == 0 || song_length > MOD_ORDER_COUNT)
        song_length = MOD_ORDER_COUNT; /* clamp gracefully */

    uint8_t max_pattern = 0;
    for (int i = 0; i < MOD_ORDER_COUNT; i++) {
        uint8_t p = data[MOD_ORDER_OFF + i];
        if (p > max_pattern)
            max_pattern = p;
    }
    uint32_t pattern_count = (uint32_t)max_pattern + 1;

    /* Verify pattern data fits */
    size_t pattern_data_size = (size_t)pattern_count * MOD_PATTERN_BYTES;
    if (MOD_HEADER_SIZE + pattern_data_size > len)
        return NULL;

    MoonMod *mod = (MoonMod *)calloc(1, sizeof(MoonMod));
    if (!mod)
        return NULL;

    /* Title */
    memcpy(mod->title, data, MOD_TITLE_LEN);
    mod->title[MOD_TITLE_LEN] = '\0';

    mod->song_length       = song_length;
    mod->restart_position  = data[MOD_RESTART_OFF];
    memcpy(mod->order, data + MOD_ORDER_OFF, MOD_ORDER_COUNT);
    mod->pattern_count     = pattern_count;
    mod->sample_count      = MOD_SAMPLE_COUNT;

    /* Copy pattern data */
    mod->pattern_data = (uint8_t *)malloc(pattern_data_size);
    if (!mod->pattern_data) {
        free(mod);
        return NULL;
    }
    memcpy(mod->pattern_data, data + MOD_HEADER_SIZE, pattern_data_size);

    /* Parse sample descriptors and copy PCM data */
    size_t sample_offset = MOD_HEADER_SIZE + pattern_data_size;
    for (int i = 0; i < MOD_SAMPLE_COUNT; i++) {
        const uint8_t *desc = data + MOD_TITLE_LEN + (size_t)i * MOD_SAMPLE_DESCR;
        MoonModSample  *s   = &mod->samples[i];

        memcpy(s->name, desc, MOD_SAMPLE_NAME);
        s->name[MOD_SAMPLE_NAME] = '\0';

        s->length_words        = (uint16_t)((desc[22] << 8) | desc[23]);
        /* finetune: lower nibble, signed 4-bit two's-complement */
        {
            uint8_t ft = desc[24] & 0x0Fu;
            s->finetune = (ft < 8u) ? (int8_t)ft : (int8_t)((int)ft - 16);
        }
        s->volume              = desc[25];
        s->repeat_offset_words = (uint16_t)((desc[26] << 8) | desc[27]);
        s->repeat_length_words = (uint16_t)((desc[28] << 8) | desc[29]);

        size_t sample_bytes = (size_t)s->length_words * 2u;
        if (sample_bytes > 0 && sample_offset + sample_bytes <= len) {
            s->data = (uint8_t *)malloc(sample_bytes);
            if (s->data)
                memcpy(s->data, data + sample_offset, sample_bytes);
        }
        sample_offset += sample_bytes;
    }

    return mod;
}

MoonMod *moon_mod_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    CacheEntry *e = cache_find(name, ASSET_MOD);
    if (e) {
        e->refcount++;
        return (MoonMod *)e->asset;
    }

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    /* Decompress if RNC1-compressed */
    uint8_t *raw      = NULL;
    size_t   raw_size = 0;
    int      is_rnc   = (len >= 4 && buf[0] == 'R' && buf[1] == 'N' &&
                         buf[2] == 'C' && buf[3] == 0x01);

    if (is_rnc) {
        uint32_t uncomp = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) |
                          ((uint32_t)buf[6] <<  8) |  (uint32_t)buf[7];
        raw_size = (size_t)uncomp + 16; /* small headroom */
        raw = (uint8_t *)malloc(raw_size);
        if (!raw) {
            free(buf);
            return NULL;
        }
        int written = moon_rnc1_decompress(buf, len, raw, raw_size);
        free(buf);
        if (written < 0) {
            free(raw);
            return NULL;
        }
        raw_size = (size_t)written;
    } else {
        raw      = buf;
        raw_size = len;
        buf      = NULL;
    }

    MoonMod *mod = mod_decode(raw, raw_size);
    free(raw);

    if (!mod)
        return NULL;

    cache_insert(name, ASSET_MOD, mod);
    return mod;
}

void moon_mod_free(MoonMod *mod)
{
    if (!mod)
        return;
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        for (CacheEntry *e = g_ctx.buckets[i]; e; e = e->next) {
            if (e->type == ASSET_MOD && e->asset == mod) {
                if (--e->refcount <= 0) {
                    cache_remove(e->name, ASSET_MOD);
                    free_asset(ASSET_MOD, mod);
                }
                return;
            }
        }
    }
    free_asset(ASSET_MOD, mod);
}

/* ------------------------------------------------------------------ */
/* OB loader                                                           */
/* ------------------------------------------------------------------ */

/*
 * The .ob file format is structurally identical to .cel:
 *
 *   Global header (10 bytes, big-endian):
 *     word[0..1]  = frame_count
 *     long[2..5]  = compressed pixel body size (bytes)
 *     long[6..9]  = reserved
 *
 *   Frame table: frame_count × 10 bytes
 *     long [0..3] = pixel_data_offset (into decompressed pixel buffer)
 *     word [4..5] = width  (pixels)
 *     word [6..7] = height (rows)
 *     byte [8]    = toggle_flags
 *     byte [9]    = planes_mask
 *
 *   Compressed pixel body (LZSS, same algorithm as .cel / LAB_049C)
 *
 * Confirmed by program.asm LAB_0496 (lines 8596-8650) which uses the
 * same header-parsing and LZSS decompression (JSR LAB_049C) as the
 * CEL loader, and by mog.asm LAB_0CBB which uses LAB_0CC2 — a variant
 * of the same LZSS algorithm with identical encoding.
 */

MoonOb *moon_ob_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    CacheEntry *e = cache_find(name, ASSET_OB);
    if (e) {
        e->refcount++;
        return (MoonOb *)e->asset;
    }

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    /* .ob files use the same format as .cel files */
    MoonCel *cel = cel_decode(buf, len);
    free(buf);
    if (!cel)
        return NULL;

    /*
     * MoonOb has an identical layout to MoonCel (frame_count + frames).
     * Cast directly: both structs have the same fields in the same order.
     */
    MoonOb *ob = (MoonOb *)(void *)cel;

    cache_insert(name, ASSET_OB, ob);
    return ob;
}

void moon_ob_free(MoonOb *ob)
{
    if (!ob)
        return;
    for (int i = 0; i < CACHE_BUCKETS; i++) {
        for (CacheEntry *e = g_ctx.buckets[i]; e; e = e->next) {
            if (e->type == ASSET_OB && e->asset == ob) {
                if (--e->refcount <= 0) {
                    cache_remove(e->name, ASSET_OB);
                    free_asset(ASSET_OB, ob);
                }
                return;
            }
        }
    }
    free_asset(ASSET_OB, ob);
}
