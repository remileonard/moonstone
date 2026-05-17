/*
 * moon-mod-info — display metadata of a (possibly compressed) SoundTracker
 *                 module file.
 *
 * Usage: moon-mod-info <file.cmp>
 *
 * Decompresses the module (if RNC-compressed) and prints ProTracker/
 * SoundTracker metadata: title, pattern count, sample list, BPM.
 *
 * ProTracker MOD format:
 *   Offset   0: 20-byte song title
 *   Offset  20: 31 × 30-byte sample headers
 *   Offset 950: song length (1 byte)
 *   Offset 951: restart position (1 byte, Noisetracker; or 0x7F for PT)
 *   Offset 952: 128-byte pattern table
 *   Offset 1080: 4-byte magic ("M.K." or "FLT4" or "FLT8" etc.)
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static uint16_t be16(const uint8_t *p) {
    return (uint16_t)((p[0] << 8) | p[1]);
}

typedef struct {
    char     name[23];
    uint16_t length;      /* words */
    uint8_t  finetune;
    uint8_t  volume;
    uint16_t loop_start;  /* words */
    uint16_t loop_len;    /* words */
} SampleInfo;

static void print_mod_info(const uint8_t *mod, size_t sz)
{
    if (sz < 1084) {
        printf("  (too small for a valid MOD: %zu bytes)\n", sz);
        return;
    }

    /* Title */
    char title[21];
    memcpy(title, mod, 20);
    title[20] = '\0';
    printf("  Title   : \"%s\"\n", title);

    /* Magic (at 1080) */
    char magic[5];
    memcpy(magic, mod + 1080, 4);
    magic[4] = '\0';
    printf("  Magic   : %s\n", magic);

    /* Detect number of channels */
    int channels = 4;
    if (memcmp(mod + 1080, "FLT8", 4) == 0 ||
        memcmp(mod + 1080, "8CHN", 4) == 0)
        channels = 8;
    else if (memcmp(mod + 1080, "6CHN", 4) == 0)
        channels = 6;
    printf("  Channels: %d\n", channels);

    /* Sample headers (31 samples) */
    int total_samples = 0;
    for (int i = 0; i < 31; i++) {
        const uint8_t *sh = mod + 20 + i * 30;
        char sname[23];
        memcpy(sname, sh, 22);
        sname[22] = '\0';
        uint16_t slen = be16(sh + 22);
        if (slen > 0) {
            total_samples++;
            printf("  Sample %2d: %-22s  len=%u words  ft=%d  vol=%d\n",
                   i + 1, sname, (unsigned)slen, (int8_t)(sh[24] & 0x0F),
                   sh[25]);
        }
    }
    printf("  Samples : %d non-empty\n", total_samples);

    /* Song length and pattern table */
    uint8_t song_len = mod[950];
    printf("  Length  : %d patterns in sequence\n", (int)song_len);

    /* Max pattern number */
    uint8_t max_pat = 0;
    for (int i = 0; i < 128; i++)
        if (mod[952 + i] > max_pat)
            max_pat = mod[952 + i];
    printf("  Patterns: %d unique\n", (int)max_pat + 1);

    /* Estimate BPM: default for SoundTracker/ProTracker is 125 BPM */
    printf("  BPM     : 125 (default; VBL-based at 50 Hz PAL)\n");

    /* Total file size check */
    size_t expected = 1084 + (size_t)(max_pat + 1) * (size_t)(64 * channels * 4);
    for (int i = 0; i < 31; i++) {
        const uint8_t *sh = mod + 20 + i * 30;
        expected += (size_t)be16(sh + 22) * 2;
    }
    printf("  Expected: ~%zu bytes  (actual: %zu bytes)\n", expected, sz);
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-mod-info <file.cmp>\n");
        return 1;
    }

    const char *path = argv[1];

    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || (long)fread(buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f); free(buf); return 1;
    }
    fclose(f);

    printf("File     : %s  (%ld bytes)\n", path, sz);

    uint8_t *mod_data = NULL;
    size_t   mod_size = 0;

    /* Check for RNC compression */
    if ((size_t)sz >= 4 && buf[0] == 'R' && buf[1] == 'N' &&
        buf[2] == 'C'  && buf[3] == 0x01) {
        uint32_t uncomp = ((uint32_t)buf[4]<<24)|((uint32_t)buf[5]<<16)|
                          ((uint32_t)buf[6]<<8)|(uint32_t)buf[7];
        printf("Compressed: RNC ProPack 1  (%ld → %u bytes)\n", sz, uncomp);
        mod_size = (size_t)uncomp + 16;
        mod_data = (uint8_t *)malloc(mod_size);
        if (mod_data) {
            int r = moon_rnc1_decompress(buf, (size_t)sz, mod_data, mod_size);
            if (r < 0) {
                fprintf(stderr, "  Warning: RNC decompression failed\n");
                free(mod_data);
                mod_data = buf;
                mod_size = (size_t)sz;
                buf      = NULL;
            } else {
                mod_size = (size_t)r;
            }
        }
    } else {
        mod_data = buf;
        mod_size = (size_t)sz;
        buf      = NULL;
    }

    print_mod_info(mod_data, mod_size);

    if (mod_data != buf)
        free(mod_data);
    free(buf);
    return 0;
}
