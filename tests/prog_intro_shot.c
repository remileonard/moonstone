/*
 * prog_intro_shot.c — l'intro de program en C, sans écran : mémoire bâtie
 * en C (prog_boot_memory), démarrage, intro jusqu'au chargement de mog ;
 * image montrée toutes les N VBL (<préfixe>_NNNNN.png), musique dans
 * <préfixe>.wav (stéréo 16 bits, 44100 Hz).
 *
 *   prog_intro_shot <données> <préfixe> [N [drapeaux]]
 *
 * drapeaux : EXT_0007 ($3E0) écrit par mog ; bit 7 : la fin au lieu de l'intro.
 */
#include "prog_intro.h"
#include "prog_vbl.h"
#include "moon_assets.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* PNG (blocs « stored ») */
static uint32_t crc_tab[256];

static uint32_t crc(uint32_t c, const uint8_t *p, size_t n)
{
    if (!crc_tab[1])
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t k = i;
            for (int j = 0; j < 8; j++)
                k = (k >> 1) ^ (0xEDB88320u & (0u - (k & 1u)));
            crc_tab[i] = k;
        }
    c = ~c;
    while (n--)
        c = crc_tab[(c ^ *p++) & 0xFF] ^ (c >> 8);
    return ~c;
}

static void be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);  p[3] = (uint8_t)v;
}

static void chunk(FILE *f, const char *type, const uint8_t *d, uint32_t n)
{
    uint8_t b[4];
    be32(b, n);
    fwrite(b, 1, 4, f);
    uint32_t c = crc(0, (const uint8_t *)type, 4);
    c = crc(c, d, n);
    fwrite(type, 1, 4, f);
    fwrite(d, 1, n, f);
    be32(b, c);
    fwrite(b, 1, 4, f);
}

static int write_png(const char *path, const uint32_t *px, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    static const uint8_t sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    fwrite(sig, 1, 8, f);
    uint8_t ihdr[13] = { 0 };
    be32(ihdr, (uint32_t)w);
    be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8; ihdr[9] = 2;
    chunk(f, "IHDR", ihdr, 13);
    size_t raw_n = (size_t)h * (size_t)(w * 3 + 1);
    uint8_t *raw = malloc(raw_n);
    for (int y = 0; y < h; y++) {
        uint8_t *r = raw + (size_t)y * (size_t)(w * 3 + 1);
        *r++ = 0;
        for (int x = 0; x < w; x++) {
            uint32_t c = px[(size_t)y * (size_t)w + (size_t)x];
            *r++ = (uint8_t)(c >> 16); *r++ = (uint8_t)(c >> 8); *r++ = (uint8_t)c;
        }
    }
    size_t nblk = (raw_n + 65534) / 65535;
    size_t zn = 2 + raw_n + nblk * 5 + 4;
    uint8_t *z = malloc(zn), *q = z;
    *q++ = 0x78; *q++ = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t i = 0; i < raw_n; i++) {
        a = (a + raw[i]) % 65521u;
        b = (b + a) % 65521u;
    }
    for (size_t off = 0; off < raw_n; off += 65535) {
        size_t n = raw_n - off < 65535 ? raw_n - off : 65535;
        *q++ = off + n == raw_n;
        *q++ = (uint8_t)n; *q++ = (uint8_t)(n >> 8);
        *q++ = (uint8_t)~n; *q++ = (uint8_t)(~n >> 8);
        memcpy(q, raw + off, n);
        q += n;
    }
    be32(q, b << 16 | a);
    q += 4;
    chunk(f, "IDAT", z, (uint32_t)(q - z));
    chunk(f, "IEND", NULL, 0);
    free(raw);
    free(z);
    fclose(f);
    return 0;
}

/* Son de la partie : <préfixe>.wav (stéréo 16 bits, 44100 Hz) */
static void wav_header(FILE *f, uint32_t frames)
{
    uint8_t h[44];
    uint32_t data = frames * 4;
    memcpy(h, "RIFF", 4);
    uint32_t v[] = { 36 + data };
    memcpy(h + 4, v, 4);
    memcpy(h + 8, "WAVEfmt ", 8);
    uint32_t fmt[] = { 16, 0x00020001u, 44100, 44100 * 4, 0x00100004u };
    memcpy(h + 16, fmt, 20);
    memcpy(h + 36, "data", 4);
    memcpy(h + 40, &data, 4);
    fseek(f, 0, SEEK_SET);
    fwrite(h, 1, 44, f);
    fseek(f, 0, SEEK_END);
}

static const char *prefix;
static int every = 100;
static FILE *wav;
static uint32_t wav_frames;
static uint32_t fb[320 * 200];

static void on_vbl(ProgIntro *p)
{
    static int16_t buf[882 * 2];
    prog_music_mix(p, buf, 882, 44100);
    fwrite(buf, 4, 882, wav);
    wav_frames += 882;
    if (p->vbls % (unsigned long)every == 0) {
        char path[512];
        prog_screen(p->vm, p->colour, fb);
        snprintf(path, sizeof path, "%s_%05lu.png", prefix, p->vbls);
        write_png(path, fb, 320, 200);
    }
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage : prog_intro_shot données préfixe [N]\n");
        return 2;
    }
    if (moon_init(argv[1]) != 0)
        return 1;
    prefix = argv[2];
    if (argc > 3)
        every = atoi(argv[3]);
    char path[512];
    snprintf(path, sizeof path, "%s.wav", prefix);
    wav = fopen(path, "wb+");
    if (!wav)
        return 1;
    wav_header(wav, 0);
    static IxVM vm;
    static ProgIntro p;
    uint32_t fast;
    if (prog_boot_memory(&vm, &fast) < 0)
        return 1;
    if (argc > 4)
        ix_ww(&vm, 0x3E0, (uint16_t)strtol(argv[4], NULL, 0));
    p.vm = &vm;
    p.vbl = on_vbl;
    p.potgor = 0xFFFF;
    prog_boot(&p, PROG_CHIP_BLOCK, PROG_CHIP_SIZE, fast, PROG_FAST_SIZE);
    prog_intro(&p);
    wav_header(wav, wav_frames);
    fclose(wav);
    printf("%lu VBL\n", p.vbls);
    return 0;
}
