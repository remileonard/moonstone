/*
 * mog_game_shot.c — le jeu complet (mog_game.c) sans écran, mené par un
 * script d'entrées, images PNG en chemin : vérification du branchement.
 *
 *   mog_game_shot <données> <préfixe> <script>
 *
 * Script, une commande par ligne : « n joy [touche] » (n VBL avec ce
 * joystick, touche appuyée à la première) ou « shot » (image).
 */
#include "mog_game.h"
#include "moon_assets.h"
#include "ix_mog_syms.h"

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

/* Joueur : s'aligne en profondeur sur l'adversaire le plus proche,
 * s'approche, frappe (direction au hasard) ; un geste dure quelques
 * images. */
static FILE *script;
static const char *prefix;
static int left, cur_joy, cur_key, shots;
static long vbls;

static void shot(MogGame *g)
{
    char path[512];
    snprintf(path, sizeof path, "%s_%03d.png", prefix, shots++);
    write_png(path, g->fb, MOG_GAME_W, MOG_GAME_H);
    printf("%s (VBL %ld) pointeur %s %d,%d couleurs 17-19 %03X %03X %03X\n", path, vbls,
           ix_rw(&g->vm, MOG_LAB_097C) ? "oui" : "non",
           (int16_t)ix_rw(&g->vm, MOG_LAB_097F), (int16_t)ix_rw(&g->vm, MOG_LAB_0980),
           g->colour[17], g->colour[18], g->colour[19]);
    uint32_t sp = ix_rl(&g->vm, MOG_LAB_097D);
    printf("  sprite %X h %u mots %04X %04X %04X %04X\n", sp, ix_rw(&g->vm, sp),
           ix_rw(&g->vm, sp + 6), ix_rw(&g->vm, sp + 8), ix_rw(&g->vm, sp + 10), ix_rw(&g->vm, sp + 12));
}

static void vbl(void *u, MogGame *g, MogGameInput *in)
{
    (void)u;
    vbls++;
    char line[128];
    while (!left) {
        if (!fgets(line, sizeof line, script)) {
            shot(g);
            printf("fin du script\n");
            exit(0);
        }
        if (!strncmp(line, "shot", 4)) {
            shot(g);
            continue;
        }
        char k = 0;
        int n = 0, j = 0;
        if (sscanf(line, "%d %d %c", &n, &j, &k) < 1 || n <= 0)
            continue;
        left = n;
        cur_joy = j;
        cur_key = k == 'I' ? ' ' : k;          /* I : barre d'espace */
    }
    left--;
    in->joy[0] = in->joy[1] = (uint16_t)cur_joy;
    in->key = cur_key;
    cur_key = 0;
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage : mog_game_shot données préfixe script\n");
        return 2;
    }
    if (moon_init(argv[1]) != 0)
        return 1;
    prefix = argv[2];
    script = fopen(argv[3], "r");
    if (!script) { perror(argv[3]); return 1; }
    static MogGame g;
    g.vbl = vbl;
    if (mog_game_boot(&g) < 0)
        return 1;
    int r = mog_game_run(&g);
    printf("fin de partie %d (VBL %ld)\n", r, vbls);
    return 0;
}
