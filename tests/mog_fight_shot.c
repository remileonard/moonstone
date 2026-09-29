/*
 * mog_fight_shot.c — combat de mog sans écran (game/src/mog_fight.c),
 * joueur piloté par un plan simple (s'aligne, s'approche, frappe) ;
 * images PNG (× 2) enregistrées en cours de combat.
 *
 *   mog_fight_shot <données> <préfixe> <rencontre> <lieu> [images] [pas]
 *
 * <rencontre> : index de t_CreatureInit (12 chevalier, 24 Troggs...) ;
 * <lieu> : 0 plaine, 4 forêt, 8 marais, 12 friche. Écrit
 * <préfixe>_NNNN.png toutes les <pas> images.
 */
#include "mog_fight.h"
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
static uint16_t player_joy(MogFight *f, unsigned *hold, uint16_t *cur)
{
    static const uint16_t moves[] = { 0, 1, 2, 4, 8, 5, 9, 6, 10 };
    IxVM *vm = &f->vm;
    if (*hold) {
        (*hold)--;
        return *cur;
    }
    int16_t mx = (int16_t)ix_rw(vm, f->player + 4), md = (int16_t)ix_rw(vm, f->player + 8);
    uint32_t best = 0;
    int bestd = 1 << 30;
    for (int i = 0; i < 10; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * 50;
        if (!ix_rb(vm, en))
            continue;
        uint32_t o = ix_rl(vm, en + 24);
        if (o == f->player || (int16_t)ix_rw(vm, o + 80) <= 0)
            continue;
        int d = abs((int16_t)ix_rw(vm, o + 4) - mx);
        if (d < bestd) {
            bestd = d;
            best = o;
        }
    }
    uint16_t j = 0;
    int r = rand() % 100;
    if (best) {
        int dx = (int16_t)ix_rw(vm, best + 4) - mx;
        int dd = (int16_t)ix_rw(vm, best + 8) - md;
        if (r < 8)
            j = moves[rand() % 9];
        else if (abs(dd) > 3 && r < 55)
            j = dd > 0 ? MOG_JOY_DOWN : MOG_JOY_UP;
        else if (abs(dx) > 70 && r < 85)
            j = dx > 0 ? MOG_JOY_RIGHT : MOG_JOY_LEFT;
        else
            j = (uint16_t)(MOG_JOY_FIRE | moves[rand() % 9]);
    }
    *cur = j;
    *hold = (unsigned)(rand() % 4);
    return j;
}

int main(int argc, char **argv)
{
    if (argc < 5) {
        fprintf(stderr, "usage : mog_fight_shot données préfixe rencontre lieu [images] [pas]\n");
        return 2;
    }
    if (moon_init(argv[1]) != 0)
        return 1;
    static MogFight f;
    MogFightSetup s = { 0 };
    s.knight = 0;
    s.strength = 2; s.constitution = 2; s.endurance = 2;
    s.lives = 3; s.gold = 10; s.daggers = 5;
    s.place = atoi(argv[4]);
    s.encounter = atoi(argv[3]);
    s.opponent = (s.encounter == 12 || s.encounter == 16 || s.encounter == 56) ? 1 : -1;
    int frames = argc > 5 ? atoi(argv[5]) : 200;
    int step = argc > 6 ? atoi(argv[6]) : 20;
    srand(1);
    if (mog_fight_start(&f, &s) < 0) {
        fprintf(stderr, "combat non préparé\n");
        return 1;
    }
    static uint32_t fb[MOG_SCREEN_W * MOG_SCREEN_H], big[MOG_SCREEN_W * 2 * MOG_SCREEN_H * 2];
    unsigned hold = 0;
    uint16_t cur = 0;
    int n;
    for (n = 0; n < frames; n++) {
        if (n % step == 0) {
            mog_fight_render(&f, fb);
            for (int y = 0; y < MOG_SCREEN_H * 2; y++)
                for (int x = 0; x < MOG_SCREEN_W * 2; x++)
                    big[y * MOG_SCREEN_W * 2 + x] = fb[(y / 2) * MOG_SCREEN_W + x / 2];
            char path[512];
            snprintf(path, sizeof path, "%s_%04d.png", argv[2], n);
            write_png(path, big, MOG_SCREEN_W * 2, MOG_SCREEN_H * 2);
        }
        uint16_t j = player_joy(&f, &hold, &cur);
        if (getenv("MOG_DEBUG")) {
            printf("%4d joy %02X draws %lu :", n, j, f.draws);
            for (int i = 0; i < 10; i++) {
                uint32_t en = MOG_t_Entities + (uint32_t)i * 50;
                if (!ix_rb(&f.vm, en))
                    continue;
                uint32_t o = ix_rl(&f.vm, en + 24);
                printf(" [ctl%d x%d h%d d%d pv%d]", ix_rb(&f.vm, en + 32), (int16_t)ix_rw(&f.vm, en + 6),
                       (int16_t)ix_rw(&f.vm, en + 8), (int16_t)ix_rw(&f.vm, en + 10), (int16_t)ix_rw(&f.vm, o + 80));
            }
            printf("\n");
        }
        if (!mog_fight_frame(&f, j, 0))
            break;
    }
    printf("%d images, PV joueur %d/%d, %s, %lu dessins, %lu erreurs, %lu défauts\n",
           n, mog_fight_player_hp(&f), mog_fight_player_max_hp(&f),
           f.running ? "en cours" : (mog_fight_won(&f) ? "gagné" : "perdu"),
           f.draws, f.m.errors + f.m.eng.errors, ix_vm_faults);
    return 0;
}
