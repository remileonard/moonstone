/*
 * mog_encounter.c — préparation d'un combat de mog : nouvelle partie
 * (chevaliers, tables), décor (PIV, terrain .t), chargement des créatures,
 * routines de t_CreatureInit, palette. Traduit de amiga_asm/mog.asm.
 *
 * Tout se passe dans la mémoire de mog, écrans compris (plans de bits de
 * LAB_05C0, LAB_05C1, LAB_0D92, SECSTRT_35) : l'hôte n'a qu'à les lire.
 * Les messages affichés pendant le chargement (LAB_0432), les fondus de
 * palette (LAB_0E55) et les attentes sont omis.
 */
#include "mog_private.h"
#include "mog_boot.h"
#include "mog_encounter.h"
#include "ix_mog_syms.h"

#include <stdio.h>

#define VM (m->eng.vm)

#define PLANE 0x1F40u           /* 40 octets × 200 lignes */

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }
static int16_t sw(uint16_t v) { return (int16_t)v; }

static void copy(MogCombat *m, uint32_t dst, uint32_t src, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        wb(m, dst + i, rb(m, src + i));
}

/* LAB_03F2 : fondu vers la palette a (LAB_0E55 ; mené par l'interruption
 * d'image, l'hôte reçoit directement la palette d'arrivée). */
static void palette_out(MogCombat *m, uint32_t a)
{
    wl(m, MOG_SECSTRT_39, a);
    ww(m, MOG_LAB_0E91, 2);
    ww(m, MOG_LAB_0E92, 2);
    if (!m->palette)
        return;
    uint16_t c[32];
    for (int i = 0; i < 32; i++)
        c[i] = rw(m, a + 2u * (unsigned)i);
    m->palette(m->out.user, c);
}

/* ------------------------------------------------------------------ */
/* Écrans                                                              */
/* ------------------------------------------------------------------ */

/* L00_0908E : plans de destination du décodage (LAB_0CFF-LAB_0D03) */
static void set_planes(MogCombat *m, uint32_t d0)
{
    static const uint32_t v[5] = { MOG_LAB_0CFF, MOG_LAB_0D00, MOG_LAB_0D01,
                                   MOG_LAB_0D02, MOG_LAB_0D03 };
    for (int i = 0; i < 5; i++, d0 += PLANE)
        wl(m, v[i], d0);
}

/* LAB_0D72 : écran (5 plans) à zéro */
static void clear_screen(MogCombat *m, uint32_t a0)
{
    for (uint32_t i = 0; i < 5 * PLANE; i++)
        wb(m, a0 + i, 0);
}

/* LAB_0419 : copie d'écran (blitter, 5 plans) */
static void copy_screen(MogCombat *m, uint32_t a0, uint32_t a1)
{
    wl(m, MOG_LAB_0424, a0 + 5 * PLANE);
    wl(m, MOG_LAB_0425, a1 + 5 * PLANE);
    copy(m, a1, a0, 5 * PLANE);
}

/* LAB_0C21 : décodage du PIV en a0 (en place) vers les plans LAB_0CFF ;
 * palette -> LAB_0D2B. En-tête : mot plans (4 ou 5), long taille
 * compressée, 16 ou 32 couleurs. */
static void piv_decode(MogCombat *m, uint32_t a0)
{
    wl(m, MOG_SECSTRT_25, a0);
    uint32_t n = 32;
    ww(m, MOG_LAB_0C59, rw(m, a0));
    if (rw(m, a0) != 4)
        n = 64;
    uint32_t src = a0 + 6;
    copy(m, MOG_LAB_0D2B, src, n);
    src += n;
    for (uint32_t i = 0; i < n / 2; i++) {
        uint16_t c = rw(m, MOG_LAB_0D2B + 2 * i);
        if (c & 0x8000)
            c &= 0x7FFF;
        else
            c = (uint16_t)(c << 1);
        ww(m, MOG_LAB_0D2B + 2 * i, c);
    }
    uint32_t len = rl(m, a0 + 2);
    uint32_t k = (uint32_t)(uint16_t)(len - 1) + 1;     /* DBF sur le mot */
    for (uint32_t i = 0; i < k; i++)
        wb(m, a0 + 2 + i, rb(m, src + i));
    ww(m, MOG_LAB_0D4D, 1);
    mog_unpack(VM, a0 + 2, len, rl(m, MOG_LAB_0CFF));
}

/* Copie du PIV n° off de LAB_05B9 dans le tampon LAB_05C2, puis décodage
 * vers l'écran `dest` (variable contenant son adresse). */
static void piv_to(MogCombat *m, uint32_t dest, uint32_t off, uint32_t len)
{
    set_planes(m, rl(m, dest));
    copy(m, rl(m, MOG_LAB_05C2), rl(m, MOG_LAB_05B9 + off), len);
    piv_decode(m, rl(m, MOG_LAB_05C2));
}

/* LAB_0418 : décor LAB_05C0 recopié dans les deux écrans */
static void show_background(MogCombat *m)
{
    copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_SECSTRT_35));
    copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
}

/* LAB_0432 (sans l'affichage) : texte à enregistrements chaînés (long
 * chaîne, X, Y, octet, drapeaux, long suivant). */
static void show_text(MogCombat *m, uint32_t a0)
{
    ww(m, MOG_LAB_0D05, 1);
    if (a0) {
        wl(m, MOG_LAB_08E3, a0);
        while ((a0 = rl(m, MOG_LAB_08E3)) != 0) {
            if (rl(m, MOG_v_Combatants + 10) == rl(m, MOG_LAB_05E3 + 16))    /* LAB_043B */
                wb(m, a0 + 9, rb(m, a0 + 9) | 8);
            wl(m, MOG_LAB_08E3, rl(m, a0 + 10));
        }
    }
    ww(m, MOG_LAB_0D05, 0);
}

/* LAB_0134 : sons des canaux, écran de message (LAB_0138), phrase suivante
 * de LAB_071E, palette. */
static void loading_screen(MogCombat *m)
{
    for (int ch = 0; ch < 4; ch++)                      /* LAB_0133 */
        if (m->voice)
            m->voice(m->out.user, ch, 0x6E + ch);
    /* LAB_0138 */
    uint32_t a2 = rl(m, MOG_LAB_0E93);                  /* LAB_03EB : noir */
    for (int i = 0; i < 33; i++)
        ww(m, a2 + 2u * (unsigned)i, 0);
    clear_screen(m, rl(m, MOG_SECSTRT_35));
    set_planes(m, rl(m, MOG_SECSTRT_35));
    copy(m, rl(m, MOG_LAB_0D92), rl(m, MOG_LAB_05B9 + 52), 0xE6F);
    piv_decode(m, rl(m, MOG_LAB_0D92));

    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3 + 16));
    uint16_t n = rw(m, MOG_LAB_071D);
    show_text(m, rl(m, MOG_LAB_071E + (uint32_t)(uint16_t)(n << 2)));
    n = (uint16_t)(n + 1);
    if (!(sw(n) < 14))
        n = 0;
    ww(m, MOG_LAB_071D, n);
    palette_out(m, MOG_LAB_0D2B);                       /* LAB_03F2 */
}

/* ------------------------------------------------------------------ */
/* Terrain (.t)                                                        */
/* ------------------------------------------------------------------ */

/* LAB_0A6A : coin du bloc n° d0 dans la planche (10 blocs de 32 × 25
 * par ligne). */
static void tile_origin(MogCombat *m, uint16_t d0, uint16_t *x, uint16_t *y)
{
    d0 &= 0xFF;
    *y = (uint16_t)((d0 / 10) * 25);
    *x = (uint16_t)((d0 % 10) * 32);
    ww(m, MOG_LAB_0A86, *x);
    ww(m, MOG_LAB_0A87, *y);
}

/* LAB_0A6B : octet du point (x, y) dans un plan ; décalage x & 15 */
static uint16_t plane_offset(uint16_t x, uint16_t y, uint16_t *shift)
{
    *shift = (uint16_t)(x & 15);
    return (uint16_t)(((x >> 4) + y * 20) << 1);
}

/* LAB_0A60 : masque du bloc (OU des 5 plans) dans SECSTRT_15, lignes de
 * 6 octets dont le dernier mot est nul. */
static void tile_mask(MogCombat *m)
{
    for (uint32_t i = 0; i < 200; i++)
        wb(m, MOG_SECSTRT_15 + i, 0);
    wl(m, MOG_LAB_0A97, MOG_SECSTRT_15);
    uint16_t sh;
    uint16_t off = plane_offset(rw(m, MOG_LAB_0A86), rw(m, MOG_LAB_0A87), &sh);
    uint32_t a0 = rl(m, MOG_LAB_0A8A);
    for (int p = 0; p < 5; p++, a0 += PLANE)
        for (uint32_t r = 0, d0 = off, d2 = 0; r < 25; r++, d0 = (uint16_t)(d0 + 40), d2 += 6) {
            uint32_t b = MOG_SECSTRT_15 + d2;
            wl(m, b, rl(m, b) | rl(m, a0 + (uint32_t)(int32_t)sw((uint16_t)d0)));
            ww(m, b + 4, 0);
        }
}

/* LAB_0A66 : découpage vertical (bas à 200 ; haut : une ligne de moins
 * que nécessaire est sautée, comme l'original) ; X négatif ramené à 0. */
static void tile_clip(MogCombat *m, int16_t x, int16_t y)
{
    int16_t h = sw(rw(m, MOG_LAB_0A79));
    if ((int16_t)(h + y) > 200) {
        ww(m, MOG_LAB_0A7C, (uint16_t)(200 - y));
    } else {
        wl(m, MOG_LAB_0A8E, 0);
        ww(m, MOG_LAB_0A8F, 0);
        if (y < 0) {
            uint16_t d5 = (uint16_t)~y;
            ww(m, MOG_LAB_0A7C, (uint16_t)(h - d5));
            wl(m, MOG_LAB_0A8E, (uint32_t)d5 * 40u);
            ww(m, MOG_LAB_0A89, 0);
            ww(m, MOG_LAB_0A91, 0);
            ww(m, MOG_LAB_0A8F, (uint16_t)(d5 * 6));
        }
    }
    ww(m, MOG_LAB_0A95, 0xFFFF);
    ww(m, MOG_LAB_0A96, 0);
    wl(m, MOG_LAB_0A8D, 0);
    if (x < 0) {
        ww(m, MOG_LAB_0A88, 0);
        ww(m, MOG_LAB_0A90, 0);
    }
}

/* LAB_0A64 : bloc n° tile de la planche a0 posé en (x, y) sur l'écran
 * a1, couleur 0 transparente (blitter : D = A | ~B & C). */
static void draw_tile(MogCombat *m, uint32_t a0, uint32_t a1, uint16_t x, uint16_t y, uint16_t tile)
{
    ww(m, MOG_LAB_0A84, 0);
    ww(m, MOG_LAB_0A85, 0);
    ww(m, MOG_LAB_0A88, x);
    ww(m, MOG_LAB_0A89, y);
    wl(m, MOG_LAB_0A8A, a0);
    wl(m, MOG_LAB_0A8B, a1);
    ww(m, MOG_LAB_0A8C, tile & 0xFF);
    ww(m, MOG_LAB_0A7C, rw(m, MOG_LAB_0A79));
    ww(m, MOG_LAB_0A7B, rw(m, MOG_LAB_0A7A));
    tile_clip(m, (int16_t)x, (int16_t)y);
    uint16_t h = rw(m, MOG_LAB_0A7C);
    uint16_t words = (uint16_t)((rw(m, MOG_LAB_0A7A) + 16) >> 4);
    ww(m, MOG_LAB_0A94, (uint16_t)(h << 6 | words));
    uint16_t tx, ty, sh;
    tile_origin(m, rw(m, MOG_LAB_0A8C), &tx, &ty);
    uint32_t src = a0 + plane_offset(tx, ty, &sh) + rl(m, MOG_LAB_0A8E);
    wl(m, MOG_LAB_0A92, src);
    tile_mask(m);
    uint32_t mask = MOG_SECSTRT_15 + rw(m, MOG_LAB_0A8F);
    wl(m, MOG_LAB_0A97, mask);
    uint32_t dst = a1 + plane_offset(rw(m, MOG_LAB_0A88), rw(m, MOG_LAB_0A89), &sh);
    wl(m, MOG_LAB_0A93, dst);
    uint16_t con = (uint16_t)(sh << 12);
    ww(m, MOG_LAB_0A84, (uint16_t)(con + 0x0FF2));
    ww(m, MOG_LAB_0A85, con);
    uint16_t amod = rw(m, MOG_LAB_0A7D), cmod = rw(m, MOG_LAB_0A80);
    for (int p = 0; p < 5; p++) {                       /* LAB_0A65 */
        uint32_t a = src, b = mask, d = dst;
        for (unsigned r = 0; r < (h ? h : 1024u); r++) {
            uint32_t pa = 0, pb = 0;
            for (unsigned k = 0; k < words; k++, a += 2, b += 2, d += 2) {
                uint32_t wa = rw(m, a);
                if (k == 0)
                    wa &= rw(m, MOG_LAB_0A95);
                if (k == words - 1u)
                    wa &= rw(m, MOG_LAB_0A96);
                uint32_t wbm = rw(m, b);
                uint16_t sa = (uint16_t)((pa << 16 | wa) >> sh);
                uint16_t sb = (uint16_t)((pb << 16 | wbm) >> sh);
                pa = wa;
                pb = wbm;
                ww(m, d, (uint16_t)(sa | (~sb & rw(m, d))));
            }
            a += amod;
            b += rw(m, MOG_LAB_0A7E);
            d += cmod;
        }
        src += PLANE;
        dst += PLANE;
    }
    wl(m, MOG_LAB_0A92, src);
    wl(m, MOG_LAB_0A93, dst);
}

/* SECSTRT_12 : objets du terrain (mots drapeaux|bloc, X, Y ; $FF00 fin,
 * $FE00 sauté) posés sur LAB_05C0 ; planche LAB_05C1 pour $03xx, LAB_0D92
 * sinon. */
static void draw_terrain(MogCombat *m)
{
    ww(m, MOG_LAB_0A81, 0);
    for (;;) {
        uint32_t a5 = rl(m, MOG_LAB_0A83) + (uint32_t)(int32_t)sw(rw(m, MOG_LAB_0A81));
        uint16_t w = rw(m, a5), f = w & 0xFF00;
        if (f == 0xFF00)
            return;
        if (f != 0xFE00) {
            uint32_t a0 = f == 0x0300 ? rl(m, MOG_LAB_05C1) : rl(m, MOG_LAB_0D92);
            draw_tile(m, a0, rl(m, MOG_LAB_05C0), rw(m, a5 + 2), rw(m, a5 + 4), w);
        }
        ww(m, MOG_LAB_0A81, (uint16_t)(rw(m, MOG_LAB_0A81) + 6));
    }
}

/* LAB_0A6C : terrain sans obstacle (une barrière de fond à Y = $63) */
static void terrain_default(MogCombat *m)
{
    static const uint16_t v[5] = { 1, 0, 0x135, 0x63, 0x0A };
    uint32_t a1 = rl(m, MOG_SECSTRT_14);
    for (int i = 0; i < 5; i++)
        ww(m, a1 + 2u * (unsigned)i, v[i]);
    ww(m, MOG_LAB_0A98, 0x63);
}

/* LAB_0A6D : terrain `name` (LAB_0CC0 : taille puis données compressées) :
 * obstacles (mot nombre, enregistrements de 8 octets) dans SECSTRT_14,
 * objets à dessiner dans LAB_0A83 ; LAB_0A98 = Y maximal des obstacles. */
static void terrain_load(MogCombat *m, uint32_t name)
{
    ww(m, MOG_LAB_0A98, 0x1E);
    uint32_t a1 = rl(m, MOG_SECSTRT_14), a2 = rl(m, MOG_LAB_0A83);
    MogFile f;
    mog_file_open(VM, name, &f);
    mog_file_read(VM, &f, a2, 4);
    uint32_t len = rl(m, a2);
    mog_file_read(VM, &f, a2, len);
    mog_file_close(&f);
    ww(m, MOG_LAB_0D4D, 1);
    mog_unpack(VM, a2, len, a1);

    uint16_t n = rw(m, a1);
    copy(m, rl(m, MOG_LAB_0A83), a1 + 2 + (uint32_t)n * 8u, 0x960);
    uint32_t a0 = a1 + 2;
    for (uint32_t i = 0; i <= n; i++, a0 += 8) {        /* DBF : n + 1 */
        uint16_t y = rw(m, a0 + 4);
        if (!(sw(y) < sw(rw(m, MOG_LAB_0A98))))
            ww(m, MOG_LAB_0A98, y);
    }
    draw_terrain(m);
}

/* Terrain suivant de la table `tbl` (8 fichiers, compteur `ctr`), ou celui
 * de la carte (LAB_076D = 2 : LAB_076E). */
static void terrain_next(MogCombat *m, uint32_t tbl, uint32_t ctr, int byte_ctr)
{
    if (rl(m, MOG_LAB_076D) == 2) {
        terrain_load(m, rl(m, MOG_LAB_076E));
        return;
    }
    uint16_t c = byte_ctr ? rb(m, ctr) : rw(m, ctr);
    terrain_load(m, rl(m, tbl + (uint32_t)(uint16_t)(c << 2)));
    if (byte_ctr)
        wb(m, ctr, (uint8_t)((c + 1) & 7));
    else
        ww(m, ctr, (uint16_t)((c + 1) & 7));
}

/* LAB_0150 : planche de blocs commune (PIV +12) dans LAB_0D92 */
static void tiles_common(MogCombat *m) { piv_to(m, MOG_LAB_0D92, 12, 0x5149); }

/* LAB_0142 : décor +32 dans LAB_05C0 */
static void background_32(MogCombat *m) { piv_to(m, MOG_LAB_05C0, 32, 0x51C4); }

/* LAB_013C : décor et terrain selon le type de lieu LAB_08C4 */
static void setup_scenery(MogCombat *m)
{
    clear_screen(m, rl(m, MOG_LAB_05C0));
    terrain_default(m);
    switch (rl(m, MOG_LAB_08C4)) {
    case 4:                                             /* LAB_0147 */
        piv_to(m, MOG_LAB_05C1, 8, 0x5958);             /* LAB_014A */
        tiles_common(m);
        piv_to(m, MOG_LAB_05C0, 24, 0x6395);
        terrain_next(m, MOG_LAB_07B9, MOG_LAB_05EB, 1);
        break;
    case 0:                                             /* LAB_0144 */
        piv_to(m, MOG_LAB_05C1, 8, 0x5958);
        tiles_common(m);
        piv_to(m, MOG_LAB_05C0, 28, 0x51A5);
        terrain_next(m, MOG_LAB_07B7, MOG_LAB_05EA, 0);
        break;
    case 8:                                             /* LAB_013D */
        piv_to(m, MOG_LAB_05C1, 20, 0x4658);            /* LAB_014C */
        tiles_common(m);
        piv_to(m, MOG_LAB_05C0, 36, 0x4C0A);
        terrain_next(m, MOG_LAB_07B8, MOG_LAB_05E8, 0);
        break;
    case 12:                                            /* LAB_0140 */
        piv_to(m, MOG_LAB_05C1, 16, 0x3A55);            /* LAB_014E */
        tiles_common(m);
        background_32(m);
        terrain_next(m, MOG_LAB_07B6, MOG_LAB_05E9, 0);
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Créatures : CEL, points d'impact, sons                              */
/* ------------------------------------------------------------------ */

/* LAB_0AB5 : banque de sons (en-tête de $20 octets sauté) */
static void load_sounds(MogCombat *m, uint32_t name, uint32_t dst_var, uint32_t n)
{
    MogFile f;
    mog_file_open(VM, name, &f);
    f.pos = f.len < 0x20 ? f.len : 0x20;
    mog_file_read(VM, &f, rl(m, dst_var), n);
    mog_file_close(&f);
}

static uint32_t bank(unsigned i) { return MOG_LAB_05E0 + 4u * i; }

static uint32_t cel_after(MogCombat *m, unsigned i, uint32_t name)
{
    return rl(m, bank(i)) + mog_cel_size(VM, name);
}

/* LAB_05E1[4] = Kn5.ob chargé en rl(LAB_05B9 + 44) (LAB_0120 / LAB_0127) */
static void knight_extra(MogCombat *m)
{
    wl(m, MOG_LAB_05E1 + 16, rl(m, MOG_LAB_05B9 + 44));
    mog_load_cel(VM, MOG_LAB_0773, rl(m, MOG_LAB_05E1 + 16));
}

/* Chargeur de la créature (LAB_0116, LAB_0118...) */
static void load_creature(MogCombat *m, uint32_t fn)
{
    uint32_t a1;
    switch (fn) {
    case MOG_LAB_0116:
        mog_load_enemy_knight(VM);
        break;
    case MOG_LAB_011A:                                  /* Troggs à hache */
    case MOG_LAB_0118:                                  /* Troggs à lance */
        if (rb(m, MOG_LAB_05DF) == (fn == MOG_LAB_011A ? 0x18 : 0x20))
            break;
        wb(m, MOG_LAB_05DF, fn == MOG_LAB_011A ? 0x18 : 0x20);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        if (fn == MOG_LAB_011A) {
            mog_load_cel(VM, MOG_LAB_0778, a1);
            a1 = cel_after(m, 0, MOG_LAB_0778);
            wl(m, bank(1), a1);
            mog_load_hit_cel(VM, MOG_LAB_0779, a1);
        } else {
            mog_load_cel(VM, MOG_LAB_077A, a1);
            a1 = cel_after(m, 0, MOG_LAB_077A);
            for (unsigned i = 1; i < 5; i++)
                wl(m, bank(i), a1);
            mog_load_hit_cel(VM, MOG_LAB_077B, a1);
        }
        load_sounds(m, MOG_LAB_0ABB, MOG_LAB_05C8, 0xAB28);     /* LAB_0AB1 */
        break;
    case MOG_LAB_011C:                                  /* Ratmen */
        if (rb(m, MOG_LAB_05DF) == 0x24)
            break;
        wb(m, MOG_LAB_05DF, 0x24);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_LAB_077C, a1);
        a1 = cel_after(m, 0, MOG_LAB_077C);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_LAB_077D, a1);
        load_sounds(m, MOG_LAB_0ABF, MOG_LAB_05CB, 0xD508);     /* LAB_0AB2 */
        break;
    case MOG_LAB_011E:                                  /* Mudmen */
        wb(m, MOG_LAB_05DF, 4);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_LAB_0782, a1);
        a1 = rl(m, MOG_LAB_05B9 + 44);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_LAB_0783, a1);
        load_sounds(m, MOG_LAB_0AC0, MOG_LAB_05C8, 0xB690);     /* LAB_0AB3 */
        break;
    case MOG_LAB_011F:                                  /* Balok */
        if (rb(m, MOG_LAB_05DF) != 0x30) {
            wb(m, MOG_LAB_05DF, 0x30);
            a1 = rl(m, MOG_LAB_05B8 + 8);
            wl(m, bank(0), a1);
            mog_load_hit_cel(VM, MOG_LAB_0785, a1);
            wl(m, bank(1), cel_after(m, 0, MOG_LAB_0785));
            mog_load_cel(VM, MOG_LAB_0786, rl(m, bank(1)));
            a1 = cel_after(m, 1, MOG_LAB_0786);
            wl(m, bank(2), a1);
            mog_load_cel(VM, MOG_LAB_0787, a1);
            load_sounds(m, MOG_LAB_0AB8, MOG_LAB_05C8, 0xBDCC); /* LAB_0AAB */
        }
        knight_extra(m);
        break;
    case MOG_LAB_0121:                                  /* Dragon */
        wb(m, MOG_LAB_05DF, 0x14);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_LAB_0780, a1);
        a1 = cel_after(m, 0, MOG_LAB_0780);
        wl(m, bank(1), a1);
        mog_load_hit_cel(VM, MOG_LAB_0781, a1);
        a1 = cel_after(m, 1, MOG_LAB_0781);
        wl(m, bank(4), a1);
        mog_load_hit_cel(VM, MOG_LAB_0122, a1);
        load_sounds(m, MOG_LAB_0AB9, MOG_LAB_05C8, 0xC140);     /* LAB_0AAC */
        knight_extra(m);
        break;
    case MOG_LAB_0123:                                  /* chevalier de passage */
        if (rb(m, MOG_LAB_05DF) == 0)
            break;
        wb(m, MOG_LAB_05DF, 0);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_LAB_077E, a1);
        a1 = cel_after(m, 0, MOG_LAB_077E);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_LAB_077F, a1);
        load_sounds(m, MOG_LAB_0AB7, MOG_LAB_05C8, 0x57BE);     /* LAB_0AAD */
        break;
    case MOG_LAB_0125:                                  /* Démon */
        background_32(m);
        wb(m, MOG_LAB_05DF, 8);
        a1 = rl(m, MOG_LAB_05B9 + 44);
        wl(m, bank(3), a1);
        mog_load_cel(VM, MOG_LAB_07B3, a1);
        a1 = rl(m, MOG_LAB_05B9 + 44) + mog_cel_size(VM, MOG_LAB_07B3);
        wl(m, bank(4), a1);
        mog_load_cel(VM, MOG_LAB_07B0, a1);
        a1 = rl(m, MOG_LAB_05B8 + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_LAB_07B1, a1);
        a1 = cel_after(m, 0, MOG_LAB_07B1);
        wl(m, bank(2), a1);
        mog_load_hit_cel(VM, MOG_LAB_07B2, a1);
        load_sounds(m, MOG_LAB_0ABD, MOG_LAB_05C8, 0xB27C);     /* LAB_0AAE */
        break;
    case MOG_LAB_0126:                                  /* Troll */
        if (rb(m, MOG_LAB_05DF) != 0x40) {
            wb(m, MOG_LAB_05DF, 0x40);
            wl(m, MOG_LAB_0632, rl(m, MOG_LAB_05B8 + 8));
            wl(m, bank(0), rl(m, MOG_LAB_0632));
            mog_load_hit_cel(VM, MOG_LAB_07B4, rl(m, MOG_LAB_0632));
            wl(m, MOG_LAB_0632, rl(m, MOG_LAB_0632) + mog_cel_size(VM, MOG_LAB_07B4));
            wl(m, bank(1), rl(m, MOG_LAB_0632));
            mog_load_hit_cel(VM, MOG_LAB_07B5, rl(m, MOG_LAB_0632));
        }
        knight_extra(m);
        load_sounds(m, MOG_LAB_0ABA, MOG_LAB_05C8, 0xBBC8);     /* LAB_0AAF */
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Objets                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0167 : tables et portées du chevalier */
static void knight_kit(MogCombat *m, uint32_t a1)
{
    wl(m, a1 + 34, MOG_LAB_05F5);
    wl(m, a1 + 30, MOG_LAB_05F6);
    wl(m, a1 + 42, MOG_LAB_05F7);
    wl(m, a1 + 46, MOG_LAB_0610);
    wl(m, a1 + 50, MOG_LAB_05F8);
    wl(m, a1 + 22, MOG_LAB_07DB);
    wl(m, a1 + 26, MOG_LAB_07DC);
    wl(m, a1 + 38, MOG_LAB_05E1);
    ww(m, a1 + 116, 0x64);
    ww(m, a1 + 120, 4);
    ww(m, a1 + 118, 0x50);
}

/* LAB_0195 : Dragon (tête et corps) */
static void dragon_kit(MogCombat *m, uint32_t a1)
{
    wl(m, a1 + 42, MOG_LAB_0603);
    wl(m, a1 + 38, MOG_LAB_05E0);
    wl(m, a1 + 30, MOG_LAB_0604);
    wl(m, a1 + 46, MOG_LAB_060F);
    wl(m, a1 + 22, MOG_LAB_0882);
    wl(m, a1 + 26, MOG_LAB_0882);
    ww(m, a1 + 116, 0x3C);
    ww(m, a1 + 118, 0x14);
    ww(m, a1 + 120, 5);
    ww(m, a1 + 80, 0x78);
    ww(m, a1 + 84, 0x78);
    wb(m, a1 + 77, 0x14);
    wb(m, a1 + 10, 1);
    wb(m, a1 + 11, 4);
}

/* LAB_0305 : entités, contextes et listes de frames remis à zéro */
static void reset_entities(MogCombat *m)
{
    for (uint32_t i = 0; i < 500; i++)
        wb(m, MOG_t_Entities + i, 0);
    for (uint32_t i = 0; i < 36; i++)
        wb(m, MOG_LAB_064B + i, 0);
    for (uint32_t i = 0; i < 0x2D0; i++)                /* LAB_03A7 */
        wb(m, MOG_LAB_064D + i, 0xFF);
    ix_clear_frame_lists(&m->eng);
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t en = MOG_t_Entities + i * IX_ENTITY_SIZE;
        wl(m, en + 40, MOG_t_StrikeFrames + i * 80);
        wl(m, en + 44, MOG_t_BodyFrames + i * 80);
        wl(m, en + 36, MOG_LAB_064B + i * IX_CTX_SIZE);
    }
    mog_clear_hit_links(m);
    wl(m, MOG_LAB_0A4D, rl(m, MOG_LAB_0A4F));
    wl(m, MOG_LAB_0A4E, rl(m, MOG_LAB_0A50));
}

/* LAB_02CE : les 20 objets de LAB_05C3 à zéro */
static void clear_objects(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_LAB_05C3);
    for (uint32_t i = 0; i < 0xA50; i++)
        wb(m, a0 + i, 0);
}

/* LAB_01A4 : le chevalier du joueur (LAB_0633) entre à droite */
static void player_enters(MogCombat *m)
{
    uint32_t a1 = rl(m, MOG_LAB_0633);
    wl(m, MOG_LAB_05F2, a1);
    wl(m, MOG_v_Combatants, a1);
    ww(m, a1 + 4, 0xFA);
    ww(m, a1 + 6, 0);
    ww(m, a1 + 8, 0x64);
    wb(m, a1 + 10, 3);
    knight_kit(m, a1);
    mog_enter_object_with(m, a1, MOG_LAB_07FC);
}

/* LAB_015F : états des quatre chevaliers remis à zéro */
static void reset_knights(MogCombat *m)
{
    uint32_t a1 = MOG_LAB_0613;
    for (int i = 0; i < 4; i++, a1 += IX_OBJECT_SIZE) {
        wb(m, a1 + 12, 0);
        wb(m, a1 + 13, 0);
        wl(m, a1 + 14, 0);
        wl(m, a1 + 18, 0);
        ww(m, a1 + 104, 0);
        ww(m, a1 + 64, 0);
        ww(m, a1 + 62, 0);
    }
}

/* LAB_016F : écran de chargement, décor, objets, joueur */
static void prepare(MogCombat *m)
{
    loading_screen(m);                                  /* LAB_0134 */
    setup_scenery(m);                                   /* LAB_013C */
    clear_objects(m);                                   /* LAB_02CE */
    reset_entities(m);                                  /* LAB_0305 */
    for (uint32_t i = 0; i < 0x78; i++)                 /* LAB_02F2 */
        wb(m, MOG_LAB_0301 + i, 0);
    ww(m, MOG_LAB_05EF, 0);
    player_enters(m);                                   /* LAB_01A4 */
    ww(m, MOG_LAB_0EB6, 0);
}

/* Compteurs d'adversaires : LAB_05ED (présents à la fois), LAB_05EC (à
 * vaincre), LAB_05EE (entrés) ; routines « suivant » et « équipement ». */
static void opponents(MogCombat *m, uint16_t ed, uint16_t ec, uint32_t next, uint32_t kit)
{
    ww(m, MOG_LAB_05ED, ed);
    ww(m, MOG_LAB_05EC, ec);
    ww(m, MOG_LAB_05EE, 0);
    wl(m, MOG_LAB_05F0, next);
    if (kit)
        wl(m, MOG_LAB_05F1, kit);
    ww(m, MOG_v_FrameVbls, 6);
}

/* LAB_0177 : nombre d'adversaires selon la force du joueur */
static void scale_opponents(MogCombat *m)
{
    ww(m, MOG_LAB_0186, 0);
    uint32_t a0 = rl(m, MOG_LAB_0633);
    if (!((int8_t)rb(m, a0 + 70) <= 3))
        ww(m, MOG_LAB_05ED, (uint16_t)(rw(m, MOG_LAB_05ED) + 1));
    int16_t maxhp = sw(rw(m, a0 + 84));
    if (!(maxhp < 0x1E)) {
        ww(m, MOG_LAB_05EC, (uint16_t)(rw(m, MOG_LAB_05EC) + 1));
        ww(m, MOG_LAB_0186, 1);
    }
    if (!(maxhp < 0x3C)) {
        ww(m, MOG_LAB_05ED, (uint16_t)(rw(m, MOG_LAB_05ED) + 1));
        ww(m, MOG_LAB_0186, 2);
    }
    if (!(maxhp < 0x5A)) {
        ww(m, MOG_LAB_0186, 3);
        ww(m, MOG_LAB_05EC, (uint16_t)(rw(m, MOG_LAB_05EC) + 1));
    }
    if (rl(m, MOG_LAB_076D) == 2)
        ww(m, MOG_LAB_05EC, rw(m, rl(m, MOG_LAB_08C6) + 6));
    if (rl(m, MOG_LAB_05F0) == MOG_LAB_0197)
        ww(m, MOG_LAB_05ED, 1);
    if (rl(m, MOG_LAB_05F0) == MOG_LAB_019B)
        ww(m, MOG_LAB_05ED, 1);
    if (rl(m, MOG_LAB_05F1) == MOG_LAB_019F && !(sw(rw(m, MOG_LAB_05ED)) <= 2))
        ww(m, MOG_LAB_05ED, 2);
    if (sw(rw(m, MOG_LAB_05EC)) <= 0)
        return;
    a0 = rl(m, MOG_LAB_0633);
    ww(m, a0 + 64, 8);
    uint16_t d0 = mog_knight_damage(m, a0);
    d0 = (uint16_t)(d0 + (rw(m, a0 + 84) >> 2));
    d0 = (uint16_t)(d0 >> 1);
    d0 = (uint16_t)(d0 - 6);
    if (sw(d0) < 0)
        d0 = 0;
    if (!(sw(d0) < 0x10))
        d0 = 0x0F;
    d0 >>= 1;
    uint32_t f1 = rl(m, MOG_LAB_05F1);
    for (unsigned i = 0; i < 8; i++) {
        if (rl(m, MOG_LAB_0187 + 4 * i) != f1)
            continue;
        int16_t v = (int8_t)rb(m, MOG_LAB_0185 + i * 8 + d0);
        int16_t d1 = (int16_t)(rw(m, MOG_LAB_05EC) - v);
        if (d1 > 0)
            ww(m, MOG_LAB_05EC, (uint16_t)d1);
        return;
    }
}

/* LAB_016D : les LAB_05ED premiers adversaires entrent (enregistrements
 * successifs de la table a0) */
static void first_opponents(MogCombat *m, uint32_t a0)
{
    uint32_t k = (uint32_t)(uint16_t)(rw(m, MOG_LAB_05ED) - 1) + 1;
    for (uint32_t i = 0; i < k; i++, a0 += 8)          /* (A0)+ dans LAB_0174 */
        mog_spawn_opponent(m, a0);
}

/* ------------------------------------------------------------------ */
/* Palette (LAB_03F3)                                                  */
/* ------------------------------------------------------------------ */

static void put(MogCombat *m, uint32_t a0, const uint16_t *v, int n)
{
    for (int i = 0; i < n; i++)
        ww(m, a0 + 2u * (unsigned)i, v[i]);
}

/* LAB_0403 : trois couleurs du chevalier a1 (selon 54(a1)) */
static void knight_colours(MogCombat *m, uint32_t a0, uint32_t a1)
{
    static const uint16_t c[5][3] = {
        { 0x00A, 0x007, 0x004 }, { 0xF80, 0xC50, 0xA30 }, { 0x8C6, 0x593, 0x251 },
        { 0xF22, 0xB22, 0x700 }, { 0x206, 0x103, 0x001 },
    };
    uint32_t k = rl(m, a1 + 54);
    put(m, a0, c[k <= 3 ? k : 4], 3);
}

static void set_palette(MogCombat *m, uint32_t type)
{
    copy(m, MOG_LAB_08D9, MOG_LAB_0D2B, 64);
    wl(m, MOG_LAB_0411, type);
    uint32_t a0 = MOG_LAB_08D9 + 0x12;
    switch (type) {
    case 0: {
        static const uint16_t v[] = { 0x776, 0x443, 0x731, 0x520, 0x300, 0x09A, 0xC00 };
        put(m, a0, v, 7);
        break;
    }
    case 12:
        knight_colours(m, a0, rl(m, MOG_v_Combatants + 4));
        break;
    case 24:
    case 32: {
        static const uint16_t v12[] = { 0x025, 0x004, 0x001, 0x830, 0x400, 0xF80, 0xC00 };
        static const uint16_t v0[]  = { 0x104, 0x102, 0x000, 0x600, 0x300, 0x693, 0xC00 };
        static const uint16_t vx[]  = { 0x500, 0x200, 0x000, 0xB40, 0x610, 0x895, 0xC00 };
        uint32_t g = rl(m, MOG_LAB_08C4);
        put(m, a0, g == 12 ? v12 : g == 0 ? v0 : vx, 7);
        break;
    }
    case 36: {
        static const uint16_t v[] = { 0x653, 0x942, 0x720, 0x500, 0xA96, 0x875, 0xC00 };
        put(m, a0, v, 7);
        break;
    }
    case 20: {
        static const uint16_t v[] = { 0xC00, 0x976, 0x700, 0x500, 0x754, 0xC30, 0xA00 };
        static const uint16_t w[] = { 0xFC0, 0xF80, 0xC50 };
        put(m, a0, v, 7);
        put(m, MOG_LAB_08D9 + 0x3A, w, 3);
        break;
    }
    case 48: {
        static const uint16_t v[] = { 0xF96, 0xC63, 0x930, 0x842, 0x521, 0xF63, 0xC00 };
        put(m, a0, v, 7);
        break;
    }
    case 64: {
        static const uint16_t v[] = { 0x55A, 0x347, 0x123, 0x001, 0xF00, 0x800 };
        put(m, a0, v, 6);
        break;
    }
    case 8:
        copy(m, a0, MOG_LAB_08D5, 46);
        break;
    case 4: {
        static const uint16_t v[] = { 0x332, 0xCCB, 0xB81, 0x851, 0x630, 0xF52, 0x900 };
        put(m, a0, v, 7);
        break;
    }
    }
    /* LAB_0401 */
    show_background(m);                                 /* LAB_0418 */
    knight_colours(m, MOG_LAB_08D9 + 0x0C, rl(m, MOG_v_Combatants));
    if (rl(m, MOG_LAB_0411) != 8) {                     /* LAB_0409 */
        static const uint16_t hi[] = { 0xFFD, 0x998, 0x776, 0x443 };
        uint32_t a1 = 0;
        switch (rl(m, MOG_LAB_08C4)) {
        case 0:  a1 = MOG_LAB_08D1; break;
        case 4:  a1 = MOG_LAB_08D2; break;
        case 8:  put(m, MOG_LAB_08D9 + 2, hi, 4); a1 = MOG_LAB_08D4; break;
        case 12: put(m, MOG_LAB_08D9 + 2, hi, 4); a1 = MOG_LAB_08D3; break;
        }
        if (a1)
            copy(m, MOG_LAB_08D9 + 0x20, a1, 26);
    }
    ww(m, MOG_LAB_08D9, 0);
    if (rl(m, MOG_LAB_0411) != 20)
        ww(m, MOG_LAB_08D9 + 30, 0xC00);
    palette_out(m, MOG_LAB_08D9);                       /* LAB_03F2 */
}

/* ------------------------------------------------------------------ */
/* Routines de t_CreatureInit                                          */
/* ------------------------------------------------------------------ */

/* Troggs, Ratmen... : même schéma (LAB_0168, LAB_016A, LAB_0175...) */
static void horde(MogCombat *m, uint32_t loader, uint16_t ed, uint16_t ec,
                  uint32_t next, uint32_t kit, uint32_t table, uint32_t pal)
{
    load_creature(m, loader);
    opponents(m, ed, ec, next, kit);
    scale_opponents(m);
    first_opponents(m, table);
    set_palette(m, pal);
}

int mog_encounter_init(MogCombat *m, uint32_t fn)
{
    uint32_t a0, a1;
    switch (fn) {
    case MOG_LAB_0164:                                  /* chevalier adverse */
        prepare(m);
        load_creature(m, MOG_LAB_0116);
        a1 = rl(m, MOG_v_Combatants + 4);
        wl(m, MOG_LAB_0634, a1);
        ww(m, a1 + 4, 0x1E);
        ww(m, a1 + 6, 0);
        ww(m, a1 + 8, 0x4B);
        wb(m, a1 + 10, 1);
        knight_kit(m, a1);
        wl(m, a1 + 38, MOG_LAB_05E0);
        mog_enter_object_with(m, a1, MOG_LAB_07FC);
        opponents(m, 1, 1, MOG_LAB_0166, 0);
        set_palette(m, 12);
        return 1;
    case MOG_LAB_0168:                                  /* Troggs à hache */
    case MOG_LAB_016A:                                  /* Troggs à hache (2) */
        prepare(m);
        horde(m, MOG_LAB_011A, 1, 3, MOG_LAB_016B,
              fn == MOG_LAB_0168 ? MOG_LAB_0169 : MOG_LAB_0170, MOG_LAB_07BA, 0x18);
        return 1;
    case MOG_LAB_0175:                                  /* Troggs à lance */
        prepare(m);
        load_creature(m, MOG_LAB_0118);
        a0 = rl(m, rl(m, MOG_LAB_0633) + 34);
        wl(m, a0 + 28, MOG_LAB_07F3);
        wl(m, a0 + 16, MOG_LAB_07F3);
        opponents(m, 1, 3, MOG_LAB_016B, MOG_LAB_0176);
        scale_opponents(m);
        first_opponents(m, MOG_LAB_07BA);
        set_palette(m, 0x20);
        return 1;
    case MOG_LAB_0188:                                  /* chevaliers de passage */
        prepare(m);
        load_creature(m, MOG_LAB_0123);
        a1 = rl(m, MOG_LAB_0633);
        wl(m, rl(m, a1 + 34) + 28, MOG_LAB_07F3);
        a0 = rl(m, a1 + 30);
        wl(m, a0 + 32, MOG_LAB_084A);
        wl(m, a0 + 28, MOG_LAB_084A);
        opponents(m, 1, 3, MOG_LAB_0189, MOG_LAB_018B);
        scale_opponents(m);
        first_opponents(m, MOG_LAB_07BB);
        set_palette(m, 0);
        return 1;
    case MOG_LAB_018C: {                                /* Ratmen */
        prepare(m);
        load_creature(m, MOG_LAB_011C);
        a0 = rl(m, rl(m, MOG_LAB_0633) + 34);
        wl(m, a0 + 16, MOG_LAB_07F2);
        wl(m, a0 + 28, MOG_LAB_07F0);
        opponents(m, 2, 2, MOG_LAB_018D, MOG_LAB_018F);
        wl(m, MOG_LAB_062B, 0);
        uint16_t v = rw(m, MOG_LAB_0A98);                /* LAB_01A5 */
        uint16_t d = (uint16_t)((((uint16_t)(200 - v)) >> 1) + v - 0x2F);
        ww(m, MOG_LAB_061A, d);
        wl(m, MOG_LAB_05F4, ix_spawn(&m->eng, MOG_LAB_0870, MOG_LAB_05E0, 0xA0,
                                     (int16_t)(d - 0xC8), (int16_t)d, 1, 0x28));
        scale_opponents(m);
        first_opponents(m, MOG_LAB_07BC);
        set_palette(m, 0x24);
        return 1;
    }
    case MOG_LAB_0192: {                                /* Dragon */
        prepare(m);
        load_creature(m, MOG_LAB_0121);
        a0 = rl(m, rl(m, MOG_LAB_0633) + 30);
        wl(m, a0 + 4, MOG_LAB_07F5);
        wl(m, a0 + 8, MOG_LAB_07FE);
        wl(m, a0 + 32, MOG_LAB_07FE);
        wl(m, a0 + 20, MOG_LAB_07FB);
        wl(m, MOG_LAB_0603 + 8, 0x1E);
        wl(m, MOG_LAB_0603 + 32, 0x1E);
        wl(m, MOG_LAB_0603 + 20, 0x0A);
        wl(m, MOG_LAB_0603 + 4, 0x0A);
        a1 = MOG_LAB_0617;
        ww(m, a1 + 4, 0x50);
        ww(m, a1 + 6, 0xFFD8);
        ww(m, a1 + 8, 0x64);
        wb(m, a1 + 10, 1);
        dragon_kit(m, a1);
        ww(m, MOG_LAB_0623, 0);
        mog_enter_object(m, a1);
        static const uint16_t depth[2] = { 0x50, 0x78 };
        static const uint32_t var[2] = { MOG_LAB_0193, MOG_LAB_0194 };
        for (int i = 0; i < 2; i++) {
            a1 = mog_alloc_object(m);
            wl(m, var[i], a1);
            ww(m, a1 + 4, 5);
            ww(m, a1 + 6, 0);
            ww(m, a1 + 8, depth[i]);
            ww(m, a1 + 80, 0x32);
            ww(m, a1 + 84, 0x32);
            dragon_kit(m, a1);
            wb(m, a1 + 77, 0x2C);
            wl(m, a1 + 22, MOG_LAB_0880);
            wl(m, a1 + 26, MOG_LAB_0880);
            ww(m, a1 + 120, 0x0A);
            mog_enter_object(m, a1);
        }
        opponents(m, 1, 1, MOG_LAB_0166, MOG_LAB_0166);
        set_palette(m, 0x14);
        return 1;
    }
    case MOG_LAB_0196:                                  /* Balok */
        prepare(m);
        load_creature(m, MOG_LAB_011F);
        wl(m, rl(m, rl(m, MOG_LAB_0634) + 30) + 8, MOG_LAB_07FB);
        opponents(m, 1, 2, MOG_LAB_0197, MOG_LAB_0198);
        scale_opponents(m);
        first_opponents(m, MOG_LAB_0199);
        set_palette(m, 0x30);
        return 1;
    case MOG_LAB_019A:                                  /* Mudmen */
        wl(m, MOG_LAB_08C4, 8);
        prepare(m);
        load_creature(m, MOG_LAB_011E);
        opponents(m, 1, 2, MOG_LAB_019B, MOG_LAB_019D);
        scale_opponents(m);
        mog_next_opponent(m, MOG_LAB_019B);
        set_palette(m, 4);
        return 1;
    case MOG_LAB_019E:                                  /* Troll */
        prepare(m);
        load_creature(m, MOG_LAB_0126);
        wl(m, rl(m, rl(m, MOG_LAB_0634) + 30) + 8, MOG_LAB_07FB);
        opponents(m, 1, 1, MOG_LAB_016B, MOG_LAB_019F);
        scale_opponents(m);
        mog_next_opponent(m, MOG_LAB_016B);
        set_palette(m, 0x40);
        return 1;
    case MOG_LAB_01A0:                                  /* Démon */
        loading_screen(m);
        load_creature(m, MOG_LAB_0125);
        reset_knights(m);
        clear_objects(m);
        reset_entities(m);
        terrain_default(m);
        player_enters(m);
        wl(m, rl(m, rl(m, MOG_LAB_0633) + 30) + 32, MOG_LAB_07FB);
        a1 = mog_alloc_object(m);
        wl(m, MOG_LAB_01A1, a1);
        wl(m, a1 + 22, MOG_LAB_08AE);
        wl(m, a1 + 26, MOG_LAB_08B0);
        wl(m, a1 + 38, MOG_LAB_05E0);
        ww(m, a1 + 80, 0x8C);
        wb(m, a1 + 77, 8);
        wb(m, a1 + 11, 4);
        ww(m, a1 + 4, 0x64);
        ww(m, a1 + 6, 5);
        ww(m, a1 + 8, 0x64);
        ww(m, a1 + 120, 2);
        ww(m, a1 + 118, 0x5A);
        ww(m, a1 + 116, 0x5F);
        wb(m, a1 + 10, 1);
        mog_enter_object(m, a1);
        wl(m, MOG_LAB_01A2, mog_alloc_object(m));
        opponents(m, 1, 1, MOG_LAB_0166, 0);
        set_palette(m, 8);
        return 1;
    }
    char t[64];
    snprintf(t, sizeof t, "rencontre non portée : %08X", fn);
    mog_message(m, t);
    m->errors++;
    return 0;
}

/* ------------------------------------------------------------------ */
/* Nouvelle partie                                                     */
/* ------------------------------------------------------------------ */

/* LAB_01C6 : caractéristiques de départ d'un chevalier */
static void knight_defaults(MogCombat *m, uint32_t a1)
{
    wb(m, a1 + 70, 1);
    wb(m, a1 + 71, 1);
    wb(m, a1 + 72, 1);
    wb(m, a1 + 73, 5);
    ww(m, a1 + 80, 0x14);
    wl(m, a1 + 92, 0x1B);
    wl(m, a1 + 88, 0x16);
    ww(m, a1 + 78, 0);
    wb(m, a1 + 76, 0x0A);
    ww(m, a1 + 74, 0x0A);
    wb(m, a1 + 83, 0xFF);
    wb(m, a1 + 130, 0);
    wl(m, a1 + 100, 0);
    uint32_t a0 = rl(m, a1 + 96);
    for (uint32_t i = 0; i < 24; i++)
        wb(m, a0 + i, 0);
}

void mog_update_knight(MogCombat *m, uint32_t a0)
{
    /* LAB_0013 : PV maximum */
    uint16_t d1 = (uint16_t)(rb(m, a0 + 71) * 10);
    uint32_t a1 = rl(m, a0 + 96);
    if (rb(m, a1 + 4))
        wl(m, a0 + 88, 0x19);
    d1 = (uint16_t)(d1 + rb(m, a1 + 6) * 20);
    uint32_t armour = rl(m, a0 + 92);
    if (armour == 0x1C) d1 = (uint16_t)(d1 + 10);
    if (armour == 0x1D) d1 = (uint16_t)(d1 + 20);
    if (armour == 0x1E) d1 = (uint16_t)(d1 + 30);
    d1 = (uint16_t)(d1 + 10);
    ww(m, a0 + 84, d1);
    if (!(sw(d1) > sw(rw(m, a0 + 80))))
        ww(m, a0 + 80, d1);
    /* LAB_0019 : défense */
    uint8_t b = (uint8_t)(rb(m, a0 + 72) << 1);
    if (armour == 0x1C) b = (uint8_t)(b + 2);
    if (armour == 0x1E) b = (uint8_t)(b + 2);
    wb(m, a0 + 86, (uint8_t)(b + 4));
}

void mog_new_game(MogCombat *m)
{
    /* LAB_01AE (partie combat ; boutiques et carte laissées au jeu) */
    ww(m, MOG_LAB_0663, 0);
    ww(m, MOG_LAB_05D3, 0);
    ww(m, MOG_LAB_0655, 0);
    ww(m, MOG_LAB_0654, 0);
    ww(m, MOG_LAB_05F3, 1);
    ww(m, MOG_v_Combatants + 18, 0x2D);
    ww(m, MOG_v_Combatants + 20, 0);
    ww(m, MOG_LAB_06C0, 0);
    wl(m, MOG_v_Combatants, MOG_LAB_0613);
    wl(m, MOG_LAB_0634, MOG_LAB_0613);
    wb(m, MOG_v_Combatants + 8, 0);
    static const uint32_t ai[4] = { MOG_LAB_08C0, MOG_LAB_08C1, MOG_LAB_08C2, MOG_LAB_08C3 };
    static const uint16_t pos[4][2] = { { 0x0F, 0x64 }, { 0x12C, 0x64 }, { 0xA0, 0x14 }, { 0xA0, 0xB4 } };
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_LAB_0613 + i * IX_OBJECT_SIZE;
        wl(m, a1 + 108, ai[i]);
        wb(m, a1 + 77, 0x10);
        wb(m, a1 + 11, 4);
        wl(m, a1 + 54, 4);
        ww(m, a1 + 126, pos[i][0]);
        ww(m, a1 + 128, pos[i][1]);
    }
    wb(m, MOG_LAB_0617 + 73, 1);
    dragon_kit(m, MOG_LAB_0617);
    ww(m, MOG_v_FrameVbls, 6);

    static const struct { uint8_t off; uint32_t fn; } ctl[] = {
        { 36, MOG_LAB_0251 }, { 4, MOG_SECSTRT_40 }, { 0, MOG_LAB_0226 }, { 16, MOG_LAB_0EFF },
        { 56, MOG_Ctl_HumanKnight }, { 20, MOG_LAB_027A }, { 8, MOG_LAB_0ED2 },
        { 12, MOG_Ctl_HumanKnight }, { 24, MOG_LAB_0236 }, { 28, MOG_LAB_0236 },
        { 32, MOG_LAB_0236 }, { 40, MOG_LAB_02D2 }, { 44, MOG_LAB_0298 }, { 52, MOG_LAB_02CB },
        { 48, MOG_LAB_029F }, { 64, MOG_LAB_0EC2 }, { 68, MOG_LAB_04AC },
    };
    for (unsigned i = 0; i < sizeof ctl / sizeof ctl[0]; i++)
        wl(m, MOG_t_Controllers + ctl[i].off, ctl[i].fn);
    static const struct { uint8_t off; uint32_t fn; } init[] = {
        { 36, MOG_LAB_018C }, { 4, MOG_LAB_019A }, { 0, MOG_LAB_0188 }, { 16, MOG_LAB_0164 },
        { 56, MOG_LAB_0164 }, { 20, MOG_LAB_0192 }, { 8, MOG_LAB_01A0 }, { 12, MOG_LAB_0164 },
        { 24, MOG_LAB_0168 }, { 32, MOG_LAB_0175 }, { 28, MOG_LAB_016A }, { 48, MOG_LAB_0196 },
        { 64, MOG_LAB_019E },
    };
    for (unsigned i = 0; i < sizeof init / sizeof init[0]; i++)
        wl(m, MOG_t_CreatureInit + init[i].off, init[i].fn);
    wb(m, MOG_LAB_0617 + 77, 0x14);
    wb(m, MOG_LAB_0617 + 10, 1);
    wl(m, MOG_LAB_0617 + 96, MOG_LAB_0619);
    wl(m, MOG_LAB_0617 + 54, 5);
    wl(m, MOG_LAB_0633, MOG_LAB_0617);

    /* LAB_01BE : chevaliers des joueurs, inventaires, équipement */
    uint16_t players = rw(m, MOG_LAB_05C5);
    for (uint16_t d0 = 0; d0 != players && d0 < 4; d0++) {
        uint32_t a1 = MOG_LAB_0613 + d0 * IX_OBJECT_SIZE;
        static const uint32_t ai_p[4] = { MOG_LAB_06B6, MOG_LAB_06B5, MOG_LAB_06B7, MOG_LAB_06B8 };
        static const uint16_t pos_p[4][2] = { { 0x0A, 0x0A }, { 0x12C, 0x05 }, { 0x1A, 0xB4 }, { 0x12C, 0xB9 } };
        uint32_t k = rl(m, a1 + 54);
        if (k > 3)
            continue;
        wl(m, a1 + 108, ai_p[k]);
        ww(m, a1 + 126, pos_p[k][0]);
        ww(m, a1 + 128, pos_p[k][1]);
    }
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_LAB_0613 + i * IX_OBJECT_SIZE;
        wl(m, a1 + 96, MOG_LAB_0618 + 24 * i);
        knight_defaults(m, a1);
        knight_kit(m, a1);
    }
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_LAB_0613 + i * IX_OBJECT_SIZE;
        ww(m, a1 + 66, (uint16_t)(rw(m, a1 + 126) >> 3));
        ww(m, a1 + 68, (uint16_t)(rw(m, a1 + 128) >> 3));
        mog_update_knight(m, a1);                       /* LAB_0011 */
    }
}
