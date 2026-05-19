/*
 * mod.c — ProTracker MOD / CMP music loader for libmoon_assets.
 *
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
 *
 * .cmp files are the same ProTracker MOD compressed with RNC ProPack 1.
 * The RNC decompressor is in rnc1.c.
 */

#include "moon_private.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Constants                                                          */
/* ------------------------------------------------------------------ */

#define MOD_TITLE_LEN       20
#define MOD_SAMPLE_COUNT    31
#define MOD_SAMPLE_DESCR    30   /* bytes per sample descriptor */
#define MOD_SAMPLE_NAME     22   /* bytes of name in descriptor  */
#define MOD_SONG_LEN_OFF   950
#define MOD_RESTART_OFF    951
#define MOD_ORDER_OFF      952
#define MOD_ORDER_COUNT    128
#define MOD_HEADER_SIZE   1084  /* = 1080 (magic offset) + 4 (magic) */
#define MOD_PATTERN_BYTES 1024  /* 64 rows × 4 channels × 4 bytes */

/* ------------------------------------------------------------------ */
/* Internal decoder                                                   */
/* ------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------ */
/* Public API                                                         */
/* ------------------------------------------------------------------ */

MoonMod *moon_mod_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

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
    return mod;
}

void moon_mod_free(MoonMod *mod)
{
    if (!mod)
        return;
    free(mod->pattern_data);
    for (int i = 0; i < mod->sample_count; i++)
        free(mod->samples[i].data);
    free(mod);
}
