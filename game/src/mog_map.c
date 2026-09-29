/*
 * mog_map.c — carte du monde de mog (boucle LAB_0DAB), traduite de
 * amiga_asm/mog.asm : tour de chaque chevalier, déplacement (LAB_0DA6,
 * ralentissement par case LAB_08FA), lieux atteints (LAB_0069), dessin
 * des chevaliers et des lieux, dragon (LAB_0DCB / LAB_0DCF), chevaliers
 * noirs (LAB_0DDF...), fin de tour et nouvelle manche (LAB_0029).
 *
 * Tout est dessiné dans les écrans de mog (mog_blit.c) ; les attentes
 * passent par mog_wait_vbls (l'hôte y montre l'écran).
 */
#include "mog_private.h"
#include "mog_map.h"
#include "mog_text.h"
#include "mog_boot.h"
#include "mog_vbl.h"
#include "mog_encounter.h"
#include "ix_mog_syms.h"

#include <stdio.h>

#define VM (m->eng.vm)

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }
static int16_t sw(uint16_t v) { return (int16_t)v; }

#define CUR (rl(m, MOG_LAB_0633))       /* chevalier dont c'est le tour */

static void todo(MogCombat *m, const char *what)
{
    char t[96];
    snprintf(t, sizeof t, "carte : %s non porté", what);
    mog_message(m, t);
    m->errors++;
}

/* LAB_0CDA avec la CEL des icônes de la carte (LAB_0664) */
static void icon(MogCombat *m, uint16_t frame, uint16_t x, uint16_t y)
{
    mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_0664), frame, x, y);
}

/* L00_0908E : plans de destination des dessins */
static void set_planes(MogCombat *m, uint32_t d0)
{
    for (uint32_t i = 0; i < 5; i++, d0 += 0x1F40)
        wl(m, MOG_LAB_0CFF + 4 * i, d0);
}

/* LAB_0419 : copie d'écran (5 plans) par le blitter */
static void copy_screen(MogCombat *m, uint32_t a0, uint32_t a1)
{
    MogBlitter *b = &m->blt;
    wl(m, MOG_LAB_0424, a0);
    wl(m, MOG_LAB_0425, a1);
    for (int p = 0; p < 5; p++) {
        b->apt = rl(m, MOG_LAB_0424);
        b->dpt = rl(m, MOG_LAB_0425);
        b->amod = 0;
        b->dmod = 0;
        b->afwm = b->alwm = 0xFFFF;
        b->con0 = 0x09F0;
        b->con1 = 0;
        mog_blitter_run(VM, b, 0x3214);
        wl(m, MOG_LAB_0424, rl(m, MOG_LAB_0424) + 0x1F40);
        wl(m, MOG_LAB_0425, rl(m, MOG_LAB_0425) + 0x1F40);
    }
}

/* LAB_0B82 : clavier remis à zéro */
static void clear_keys(MogCombat *m)
{
    for (uint32_t i = 0; i < 128; i++)
        wb(m, MOG_LAB_0B91 + i, 0);
    ww(m, MOG_SECSTRT_21, 0);
}


/* ------------------------------------------------------------------ */
/* Cases et collisions                                                 */
/* ------------------------------------------------------------------ */

/* LAB_0E22 : case (8 × 8) du centre bas de l'icône du chevalier */
static void knight_cell(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_LAB_0664);
    uint16_t d2 = rw(m, a0 + 14), d3 = rw(m, a0 + 16);
    d2 >>= 1;
    a0 = CUR;
    uint16_t x = (uint16_t)(rw(m, a0 + 126) + d2), y = (uint16_t)(rw(m, a0 + 128) + d3);
    ww(m, a0 + 66, (uint16_t)(x >> 3));
    ww(m, a0 + 68, (uint16_t)(y >> 3));
}

/* LAB_0E20 : index de la case (40 par ligne) */
static uint16_t cell_index(MogCombat *m)
{
    knight_cell(m);
    uint32_t a0 = CUR;
    uint16_t d1 = (uint16_t)(rw(m, a0 + 68) * 40 + rw(m, a0 + 66));
    ww(m, MOG_LAB_0E21, d1);
    return d1;
}

/* LAB_0E52 : type de lieu (décor des combats) de la case */
static void cell_place(MogCombat *m)
{
    wl(m, MOG_LAB_076D, 0);
    uint16_t d1 = cell_index(m);
    wl(m, MOG_LAB_08C4, rb(m, MOG_LAB_08FB + (uint32_t)(int32_t)sw(d1)));
}

/* LAB_0066 : dimensions de l'icône n° d0 */
static void icon_size(MogCombat *m, uint16_t d0)
{
    uint32_t e = rl(m, MOG_LAB_0664) + (uint32_t)(int32_t)sw((uint16_t)(d0 * 10));
    ww(m, MOG_LAB_05AE, rw(m, e + 14));
    ww(m, MOG_LAB_05AF, rw(m, e + 16));
}

/* LAB_0067 : l'icône d0 en (d1, d2) recouvre-t-elle celle du chevalier
 * (icône 0) en (d3, d4) ? Renvoie D5 (2 : oui). */
static int overlap(MogCombat *m, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3, uint16_t d4)
{
    ww(m, MOG_LAB_05B2, d0);
    ww(m, MOG_LAB_05B3, d1);
    ww(m, MOG_LAB_05B4, d2);
    ww(m, MOG_LAB_05B5, d3);
    ww(m, MOG_LAB_05B6, d4);
    icon_size(m, d0);
    ww(m, MOG_LAB_05B0, rw(m, MOG_LAB_05AF));
    ww(m, MOG_LAB_05B1, rw(m, MOG_LAB_05AE));
    icon_size(m, 0);
    int d5 = 0;
    uint16_t a = rw(m, MOG_LAB_05B5), b = rw(m, MOG_LAB_05B3);
    d5 += mog_span(b, (uint16_t)(b + rw(m, MOG_LAB_05B1)), a, (uint16_t)(a + rw(m, MOG_LAB_05AE)));
    if (d5) {
        uint16_t c = rw(m, MOG_LAB_05B4), d = rw(m, MOG_LAB_05B6);
        d5 += mog_span(c, (uint16_t)(c + rw(m, MOG_LAB_05B0)), d, (uint16_t)(d + rw(m, MOG_LAB_05AF)));
    }
    return d5;
}

/* LAB_0079 : icône du chevalier a0 (à terre : 42) */
static void knight_icon(MogCombat *m, uint32_t a0)
{
    uint16_t d0 = (uint16_t)(rl(m, a0 + 54) + 5);
    if (!((int8_t)rb(m, a0 + 73) > 0))
        d0 = 0x2A;
    icon(m, d0, rw(m, a0 + 126), rw(m, a0 + 128));
}

/* LAB_0069 : lieux, chevaliers, créatures atteints par le chevalier ;
 * liste SECSTRT_2 (objet ou position, type), icônes redessinées. */
static void find_places(MogCombat *m)
{
    for (uint32_t i = 0; i < 40; i++)
        wb(m, MOG_SECSTRT_2 + i, 0);
    uint32_t a2 = MOG_SECSTRT_2;
    ww(m, MOG_LAB_069C, 0xFFFF);
    uint32_t a1 = CUR;
    uint16_t d3 = rw(m, a1 + 126), d4 = rw(m, a1 + 128);
    if (!rw(m, MOG_LAB_065E)) {
        for (uint32_t a0 = MOG_LAB_069F;; ) {            /* LAB_006B */
            uint16_t d0 = rw(m, a0), d1 = rw(m, a0 + 2), d2 = rw(m, a0 + 4);
            a0 += 6;
            if (sw(d0) < 0)
                break;
            if (overlap(m, d0, d1, d2, d3, d4) != 2)
                continue;
            uint32_t k = rl(m, a1 + 54);
            if ((d0 == 0x15 && k != 0) || (d0 == 0x16 && k != 1)
                || (d0 == 0x17 && k != 2) || (d0 == 0x18 && k != 3))
                continue;                               /* château d'un autre */
            ww(m, MOG_LAB_069D, d1);
            ww(m, MOG_LAB_069C, d0);
            icon(m, d0, d1, d2);
            ww(m, a2, d1);
            ww(m, a2 + 2, d2);
            wl(m, a2 + 4, (uint32_t)(int32_t)sw(d0));
            a2 += 8;
        }
        /* LAB_0070 : autres chevaliers */
        uint32_t a0 = MOG_LAB_0613;
        for (int i = 0; i < 4; i++, a0 += IX_OBJECT_SIZE) {
            if (a0 == a1)
                continue;
            if (overlap(m, 0, rw(m, a1 + 126), rw(m, a1 + 128), rw(m, a0 + 126), rw(m, a0 + 128)) != 2)
                continue;
            knight_icon(m, a0);
            wl(m, a2, a0);
            wl(m, a2 + 4, (int8_t)rb(m, a0 + 73) > 0 ? 1 : 0x21);
            a2 += 8;
        }
        /* chevaliers sous le dragon */
        uint32_t a3 = MOG_LAB_069E;
        for (int i = 0; i < 4; i++)
            wl(m, a3 + 4u * (unsigned)i, 0);
        if (rw(m, MOG_LAB_0667)) {
            a0 = MOG_LAB_0613;
            for (int i = 0; i < 4; i++, a0 += IX_OBJECT_SIZE) {    /* LAB_0074 */
                uint32_t d = MOG_LAB_0617;
                if (overlap(m, 0x25 - 0x11, (uint16_t)(rw(m, d + 4) - 0x0A), rw(m, d + 8),
                            rw(m, a0 + 126), rw(m, a0 + 128)) != 2)
                    continue;
                wl(m, a3, a0);
                a3 += 4;
                knight_icon(m, a0);
            }
        }
    }
    /* LAB_0076 : créatures errantes (24 enregistrements de LAB_05B9 + 68) */
    uint32_t a0 = rl(m, MOG_LAB_05B9 + 68);
    for (int i = 0; i < 24; i++, a0 += 20) {
        if ((int32_t)rl(m, a0 + 10) < 0)
            continue;
        if (overlap(m, 31, rw(m, a0 + 10), rw(m, a0 + 12), rw(m, a1 + 126), rw(m, a1 + 128)) != 2)
            continue;
        icon(m, 31, rw(m, a0 + 10), rw(m, a0 + 12));
        wl(m, a2, a0);
        wl(m, a2 + 4, 2);
        a2 += 8;
    }
}

/* ------------------------------------------------------------------ */
/* Déplacement                                                         */
/* ------------------------------------------------------------------ */

/* LAB_0E07 : bords de la carte (bits de LAB_0657 effacés) */
static void map_edges(MogCombat *m)
{
    uint32_t a0 = CUR;
    int16_t x = sw(rw(m, a0 + 126)), y = sw(rw(m, a0 + 128));
    uint8_t b = rb(m, MOG_LAB_0657);
    if (!(x > 0)) b &= (uint8_t)~2;
    if (!(x < 0x136)) b &= (uint8_t)~1;
    if (!(y > 0)) b &= (uint8_t)~8;
    if (!(y < 0xBE)) b &= (uint8_t)~4;
    wb(m, MOG_LAB_0657, b);
}

/* LAB_0DA6 : un pas selon LAB_0656, case, lieux atteints */
static void move_knight(MogCombat *m)
{
    if (rw(m, MOG_LAB_0656)) {
        map_edges(m);
        uint16_t d0 = rw(m, MOG_LAB_0656);
        uint32_t a0 = CUR;
        if (d0 & 1) ww(m, a0 + 126, (uint16_t)(rw(m, a0 + 126) + 1));
        if (d0 & 2) ww(m, a0 + 126, (uint16_t)(rw(m, a0 + 126) - 1));
        if (d0 & 8) ww(m, a0 + 128, (uint16_t)(rw(m, a0 + 128) - 1));
        if (d0 & 4) ww(m, a0 + 128, (uint16_t)(rw(m, a0 + 128) + 1));
    }
    knight_cell(m);
    find_places(m);
}

/* LAB_0DD8 : case lente (LAB_08FA) : une image sur 2^n sans mouvement */
static void terrain_slow(MogCombat *m)
{
    ww(m, MOG_L36_008E4, 0);
    if (rw(m, MOG_LAB_065E) || rw(m, MOG_LAB_065C))
        return;
    uint16_t d1 = cell_index(m);
    int8_t d7 = (int8_t)rb(m, MOG_LAB_08FA + (uint32_t)(int32_t)sw(d1));
    if (!d7)
        return;
    uint16_t c = (uint16_t)(rw(m, MOG_LAB_0DDA) + 1);
    ww(m, MOG_LAB_0DDA, c);
    if (c & (uint16_t)d7)
        ww(m, MOG_L36_008E4, 1);
}

/* ------------------------------------------------------------------ */
/* Dessin                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0DA3 : lieux (icône 20) dans le décor LAB_05C0 */
static void draw_places(MogCombat *m)
{
    set_planes(m, rl(m, MOG_LAB_05C0));
    wl(m, MOG_LAB_08C6, rl(m, MOG_LAB_05B9 + 68));
    for (int i = 0; i < 24; i++) {
        uint32_t a0 = rl(m, MOG_LAB_08C6);
        if (sw(rw(m, a0 + 10)) >= 0)
            icon(m, 0x14, rw(m, a0 + 10), rw(m, a0 + 12));
        wl(m, MOG_LAB_08C6, rl(m, MOG_LAB_08C6) + 20);
    }
}

/* LAB_0D9E : les autres chevaliers (à terre : 33 ; 82 : +43) */
static void draw_other_knights(MogCombat *m)
{
    uint32_t a1 = CUR, a0 = MOG_LAB_0613;
    for (int i = 0; i < 4; i++, a0 += IX_OBJECT_SIZE) {
        if (a0 == a1)
            continue;
        uint16_t d0 = (uint16_t)rl(m, a0 + 54);
        if (!((int8_t)rb(m, a0 + 73) > 0))
            d0 = 0x21;
        else if (rb(m, a0 + 82))
            d0 = (uint16_t)(d0 + 0x2B);
        icon(m, d0, rw(m, a0 + 126), rw(m, a0 + 128));
    }
}

/* LAB_0D9B : chevalier dont c'est le tour, puis entités (dragon) */
static void draw_current(MogCombat *m)
{
    uint32_t a1 = CUR;
    uint16_t d0 = (uint16_t)(rl(m, a1 + 54) + 5);
    if (rw(m, MOG_LAB_065E)) d0 = (uint16_t)(d0 + 5);
    if (rw(m, MOG_LAB_065C)) d0 = (uint16_t)(d0 + 10);
    icon(m, d0, rw(m, a1 + 126), rw(m, a1 + 128));
    wl(m, MOG_LAB_066E, rl(m, MOG_LAB_0633));
    mog_run_controllers(m);
    ix_run_entities(&m->eng);
    wl(m, MOG_LAB_0633, rl(m, MOG_LAB_066E));
}

/* ------------------------------------------------------------------ */
/* Tour d'un chevalier                                                 */
/* ------------------------------------------------------------------ */

/* LAB_0DBD : chevalier n° LAB_0654 ; points de déplacement 16 × 86(chev.) */
static void select_knight(MogCombat *m)
{
    uint32_t k = MOG_LAB_0613 + (uint32_t)rw(m, MOG_LAB_0654) * IX_OBJECT_SIZE;
    wl(m, MOG_LAB_0633, k);
    wl(m, MOG_v_Combatants, k);
    wb(m, k + 11, 2);
    if (rl(m, k + 54) != 4)
        wb(m, k + 77, 0x0C);
    uint16_t d0 = (uint16_t)(rb(m, k + 86) << 4);
    ww(m, MOG_LAB_0665, d0);
    ww(m, MOG_LAB_0659, (uint16_t)(d0 >> 2));
    ww(m, MOG_LAB_065A, (uint16_t)((d0 >> 1) + rw(m, MOG_LAB_0659)));
    ww(m, MOG_LAB_0668, 0);
    ww(m, MOG_LAB_0669, 0);
    ww(m, MOG_LAB_066A, 0);
    cell_place(m);
}

/* LAB_0DC8 : fin des effets de couleur de la carte, fondu au noir */
static void map_colours_off(MogCombat *m)
{
    if (rw(m, MOG_LAB_0658)) {
        wl(m, rl(m, MOG_LAB_0661), 0);                  /* LAB_0E59 */
        wl(m, rl(m, MOG_LAB_0DDE), 0);
        ww(m, MOG_LAB_0658, 0);
    }
    wl(m, MOG_SECSTRT_39, MOG_LAB_08D8);                /* LAB_03F0 */
    ww(m, MOG_LAB_0E91, 2);
    ww(m, MOG_LAB_0E92, 2);
    mog_wait_vbls(m, 0x24);
}



/* LAB_0DC5 : palette de la carte, eau qui scintille, dragon */
static void map_colours_on(MogCombat *m)
{
    if (!rw(m, MOG_LAB_0658)) {
        wl(m, MOG_SECSTRT_39, MOG_LAB_0D2B);            /* LAB_03F2 */
        ww(m, MOG_LAB_0E91, 2);
        ww(m, MOG_LAB_0E92, 2);
        mog_wait_vbls(m, 0x24);
        ww(m, MOG_LAB_0658, 1);
        wl(m, MOG_LAB_0661, mog_glow(VM, 0x1F, 0xFF, 1, 0));
        wl(m, MOG_LAB_0DDE, mog_cycle(VM, 0x15, 0x17, 1, 0x18));
    }
    if (!rw(m, MOG_LAB_0667))
        mog_map_dragon(m);                              /* LAB_0DCB */
}

/* ------------------------------------------------------------------ */
/* Dragon                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0DCB : à partir de la 3e manche, le dragon survole la carte et
 * poursuit un chevalier vivant tiré au hasard. */
void mog_map_dragon(MogCombat *m)
{
    if (sw(rw(m, MOG_LAB_06C0)) < 2)
        return;
    uint32_t d = MOG_LAB_0617;
    if ((int8_t)rb(m, d + 73) < 0)
        return;
    mog_reset_entities(m);                              /* LAB_0305 */
    wl(m, MOG_t_Controllers + 40, MOG_LAB_0DCF);
    for (uint32_t i = 0; i < 5; i++)
        wl(m, MOG_LAB_0671 + 4 * i, rl(m, MOG_LAB_0664));
    ww(m, d + 4, 0x0A);
    ww(m, d + 6, 0);
    ww(m, d + 8, 0x64);
    wb(m, d + 10, 3);
    wl(m, d + 38, MOG_LAB_0671);
    wb(m, d + 77, 0x28);
    wb(m, d + 12, 0);
    wl(m, d + 46, MOG_LAB_08FC);
    uint32_t script = rl(m, MOG_LAB_08FC);
    ix_start_entity(&m->eng, script, d, MOG_LAB_0671, sw(rw(m, script + 4)),
                    sw(rw(m, script + 6)), sw(rw(m, script + 8)), 3, 0x28);
    ww(m, MOG_LAB_0DDC, 2);
    ww(m, MOG_LAB_0666, 0x64);
    ww(m, MOG_LAB_0667, 1);
    uint32_t k;
    do                                                  /* LAB_0DCC */
        k = MOG_LAB_0613 + (mog_random(m) & 3) * IX_OBJECT_SIZE;
    while (!((int8_t)rb(m, k + 73) > 0));
    wl(m, d + 100, k);
}

/* LAB_0DCF (contrôleur 40 sur la carte) : vol du dragon, qui descend
 * vers la ligne de sa proie 40 images sur 100. */
int mog_map_dragon_ctl(MogCombat *m, uint32_t a0, CtlResult *out)
{
    wl(m, MOG_LAB_0633, a0);
    int16_t t = (int16_t)(rw(m, MOG_LAB_0666) - 1);
    ww(m, MOG_LAB_0666, (uint16_t)t);
    if (t < 0)
        ww(m, MOG_LAB_0666, 0x64);
    uint16_t dx = rw(m, MOG_LAB_0DDC);
    if (!(sw(rw(m, MOG_LAB_0666)) > 0x3C)) {
        ww(m, a0 + 4, (uint16_t)(rw(m, a0 + 4) + dx));
        *out = mog_set_script(m, MOG_LAB_0900);
        return 1;
    }
    ww(m, a0 + 4, (uint16_t)(rw(m, a0 + 4) + dx));
    uint16_t d5 = rw(m, MOG_L36_008E8);
    uint32_t prey = rl(m, MOG_LAB_0617 + 100);
    int16_t d0 = sw(rw(m, prey + 128)), y = sw(rw(m, a0 + 8));
    if (d0 != y) {
        if (!(d0 > y))
            d5 = (uint16_t)-d5;
        ww(m, a0 + 8, (uint16_t)(rw(m, a0 + 8) + d5));
    }
    if (sw(rw(m, a0 + 4)) > 0x15E) {                    /* LAB_0DD3 */
        ww(m, a0 + 4, 0x159);
        wb(m, a0 + 10, rb(m, a0 + 10) ^ 2);
        ww(m, MOG_LAB_0DDC, (uint16_t)-rw(m, MOG_LAB_0DDC));
    }
    if (!(sw(rw(m, a0 + 4)) > -20)) {
        ww(m, a0 + 4, 0xFFF6);
        wb(m, a0 + 10, rb(m, a0 + 10) ^ 2);
        ww(m, MOG_LAB_0DDC, (uint16_t)-rw(m, MOG_LAB_0DDC));
    }
    if (sw(rw(m, a0 + 8)) > 0xC8)                       /* LAB_0DD5 */
        ww(m, a0 + 8, 0);
    if (sw(rw(m, a0 + 8)) < 0)
        ww(m, a0 + 8, 0xC8);
    uint8_t f = (uint8_t)((rb(m, a0 + 12) + 1) & 15);
    wb(m, a0 + 12, f);
    *out = mog_set_script(m, rl(m, MOG_LAB_08FC + (uint32_t)(int32_t)(int8_t)f * 4));
    return 1;
}

/* ------------------------------------------------------------------ */
/* Chevaliers noirs (ordinateur)                                       */
/* ------------------------------------------------------------------ */

/* LAB_0DFF : distance |dx| + |dy| */
static uint32_t distance(uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3)
{
    int16_t a = (int16_t)(d2 - d0), b = (int16_t)(d3 - d1);
    if (a < 0) a = (int16_t)-a;
    if (b < 0) b = (int16_t)-b;
    return (uint32_t)(int32_t)(int16_t)(a + b);
}

/* LAB_0DDF : les 24 créatures triées par distance (une fois par tour) */
static void sort_creatures(MogCombat *m)
{
    if (rw(m, MOG_LAB_067B))
        return;
    ww(m, MOG_LAB_067B, 1);
    uint32_t a0 = rl(m, MOG_LAB_05C6), a1 = CUR, a2 = MOG_LAB_0672;
    uint16_t x = rw(m, a1 + 126), y = rw(m, a1 + 128);
    for (int i = 0; i < 24; i++, a0 += 20, a2 += 6) {
        uint16_t d3;
        if (sw(rw(m, a0 + 10)) < 0)
            d3 = 0xFFFF;
        else
            d3 = (uint16_t)distance(x, y, rw(m, a0 + 10), rw(m, a0 + 12));
        ww(m, a2, d3);
        wl(m, a2 + 2, a0);
    }
    for (int d0 = 0x16; d0 >= 0; d0--) {                /* LAB_0DE6 */
        uint32_t e = MOG_LAB_0672;
        for (int d1 = d0; d1 >= 0; d1--, e += 6) {
            uint16_t d2 = rw(m, e);
            if (sw(d2) >= 0 && d2 < rw(m, e + 6))
                continue;
            ww(m, e, rw(m, e + 6));                     /* LAB_0DE8 */
            ww(m, e + 6, d2);
            uint32_t t = rl(m, e + 2);
            wl(m, e + 2, rl(m, e + 8));
            wl(m, e + 8, t);
        }
    }
}

/* LAB_0DEA : une des trois créatures les plus proches (hors la 1re) */
static void pick_creature(MogCombat *m)
{
    uint32_t d0;
    do
        d0 = mog_random(m) & 3;
    while (!d0);
    wl(m, MOG_LAB_0673, rl(m, MOG_LAB_0672 + d0 * 6 + 2));
    ww(m, MOG_LAB_067C, 0);
    if (sw(rw(m, rl(m, MOG_LAB_0673) + 10)) < 0)
        ww(m, MOG_LAB_067C, 1);
}

/* LAB_0465 : nombre de caractéristiques au maximum (5) */
static uint16_t maxed_stats(MogCombat *m)
{
    uint32_t a0 = CUR;
    uint16_t d0 = 0;
    for (int i = 70; i <= 72; i++)
        if (!((int8_t)rb(m, a0 + (uint32_t)i) < 5))
            d0++;
    return d0;
}

/* LAB_0469 : caractéristique à augmenter (0 : toutes au maximum) ;
 * *entry = décalage choisi dans LAB_090A */
static uint32_t pick_stat(MogCombat *m, uint32_t *entry)
{
    for (;;) {
        if (maxed_stats(m) == 3)
            return 0;
        uint32_t d0 = mog_random(m) & 15;
        if (d0 > 8)
            d0 -= 7;
        d0 <<= 3;
        uint32_t d1 = rl(m, MOG_LAB_090A + d0);
        if (entry)
            *entry = d0;
        if (rb(m, CUR + (uint32_t)(int32_t)sw((uint16_t)d1)) != 5)
            return d1;
    }
}

/* LAB_0E2B : expérience (78) convertie en caractéristique */
static void train(MogCombat *m)
{
    uint32_t a0 = CUR;
    if (sw(rw(m, MOG_LAB_06DE)) > sw(rw(m, a0 + 78)))
        return;
    uint32_t d1 = pick_stat(m, NULL);
    if (!(uint16_t)d1)
        return;
    a0 = CUR;
    uint32_t a = a0 + (uint32_t)(int32_t)sw((uint16_t)d1);
    wb(m, a, (uint8_t)(rb(m, a) + 1));
    ww(m, a0 + 78, (uint16_t)(rw(m, a0 + 78) - rw(m, MOG_LAB_06DE)));
}

/* LAB_052F : vies et PV rendus */
static void heal(MogCombat *m, uint32_t a0)
{
    wb(m, a0 + 130, 0);
    if (rw(m, a0 + 84) == rw(m, a0 + 80)) {
        wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        if (!((int8_t)rb(m, a0 + 73) < 6))
            wb(m, a0 + 73, 5);
    }
    ww(m, a0 + 80, rw(m, a0 + 84));
}

/* LAB_0E23 : potion (inventaire 0) si peu de vies ou de PV */
static void use_potion(MogCombat *m)
{
    ww(m, MOG_LAB_066B, 0);
    uint32_t a0 = CUR;
    if ((int8_t)rb(m, a0 + 73) > 3 && !(sw(rw(m, a0 + 80)) < sw((uint16_t)(rw(m, a0 + 84) >> 2))))
        return;
    uint32_t a1 = rl(m, a0 + 96);
    if (rb(m, a1)) {
        wb(m, a1, (uint8_t)(rb(m, a1) - 1));
        heal(m, a0);
    }
}

/* LAB_05A1 : quatre sons, 8 VBL */
static void magic_sound(MogCombat *m)
{
    for (int n = 0xA3; n <= 0xA6; n++)
        mog_sound(m, n);
    mog_wait_vbls(m, 8);
}

/* LAB_001C : l'objet a0 prend le butin de a1 (inventaire, or, arme) */
void mog_loot(MogCombat *m, uint32_t a0, uint32_t a1)
{
    int d5 = 0;
    if (rb(m, a0 + 77) == 0x14) {                       /* dragon : moitié de l'or */
        uint16_t d0 = (uint16_t)(rw(m, a1 + 74) >> 1);
        ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) + d0));
        ww(m, a1 + 74, d0);
    }
    uint32_t a2 = rl(m, a1 + 96), a3 = rl(m, a0 + 96);
    if (rb(m, a1 + 73)) {                               /* vivant : un objet */
        for (uint32_t a4 = MOG_LAB_0028;; a4 += 2) {    /* LAB_001E */
            uint16_t d0 = rw(m, a4);
            if (d0 == 0xFFFF)
                break;
            if (!rb(m, a2 + d0))
                continue;
            d5 = 1;
            if (d0 == 0x16 || d0 == 0x14) {             /* LAB_0027 */
                ww(m, a3 + d0, rw(m, a3 + d0) | rw(m, a2 + d0));
                ww(m, a2 + d0, 0);
                break;
            }
            wb(m, a2 + d0, (uint8_t)(rb(m, a2 + d0) - 1));
            wb(m, a3 + d0, (uint8_t)(rb(m, a3 + d0) + 1));
            if (!rb(m, a2 + d0) && d0 == 4)
                wl(m, a1 + 88, 0x16);
            break;
        }
        if (!d5) {                                      /* LAB_0021 */
            uint16_t d0 = (uint16_t)(rw(m, a1 + 74) >> 1);
            ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) + d0));
            ww(m, a1 + 74, d0);
        }
    } else {                                            /* à terre : tout */
        for (uint32_t a4 = MOG_LAB_0028;; a4 += 2) {    /* LAB_0023 */
            uint16_t d0 = rw(m, a4);
            if (d0 == 0xFFFF)
                break;
            if (d0 == 0x16 || d0 == 0x14) {
                ww(m, a3 + d0, rw(m, a3 + d0) | rw(m, a2 + d0));
                ww(m, a2 + d0, 0);
                continue;
            }
            if (d0 == 4) {                              /* LAB_0026 */
                if (!rb(m, a2 + d0))
                    continue;
                wl(m, a1 + 88, 0x19);
            }
            wb(m, a3 + d0, (uint8_t)(rb(m, a2 + d0) + rb(m, a3 + d0)));
            ww(m, a2 + d0, 0);
        }
    }
    uint32_t k = MOG_LAB_0613;                          /* LAB_0020 : LAB_0011 */
    for (int i = 0; i < 4; i++, k += IX_OBJECT_SIZE)
        mog_update_knight(m, k);
}

/* LAB_0DF8 / LAB_0DFC : décisions au hasard (D0 entier, mot haut compris) */
static uint32_t chance_0DF8(MogCombat *m)
{
    uint32_t r = mog_random(m);
    r = (r & 0xFFFF0000u) | (r & 0x7F);
    return (uint16_t)r > 0x19 ? r : 2;
}

static int chance_0DFC(MogCombat *m)
{
    uint32_t r = mog_random(m);
    int32_t d1 = (int32_t)((r & 0xFFFF0000u) | (r & 0x7F));
    return d1 > 0x28;
}

/* LAB_0DED : cible parmi les autres chevaliers, du plus proche au plus loin */
static void pick_knight(MogCombat *m)
{
    for (uint32_t i = 0; i < 16; i++)
        wb(m, MOG_LAB_0651 + i, 0);
    uint32_t a1 = CUR, a2 = MOG_LAB_0651, a3 = MOG_LAB_0652, a0 = MOG_LAB_0613;
    for (int i = 0; i < 4; i++, a0 += IX_OBJECT_SIZE) {
        if (a0 == a1)
            continue;
        uint32_t d3 = distance(rw(m, a0 + 126), rw(m, a0 + 128), rw(m, a1 + 126), rw(m, a1 + 128));
        wl(m, a3, a0);
        a3 += 4;
        wl(m, a2, d3);
        a2 += 4;
    }
    int swapped;
    do {                                                /* LAB_0DF1 */
        swapped = 0;
        for (uint32_t i = 0; i < 2; i++) {
            uint32_t p = MOG_LAB_0651 + 4 * i, q = MOG_LAB_0652 + 4 * i;
            if (!(rl(m, p + 4) < rl(m, p)))
                continue;
            uint32_t t = rl(m, p + 4);
            wl(m, p + 4, rl(m, p));
            wl(m, p, t);
            t = rl(m, q + 4);
            wl(m, q + 4, rl(m, q));
            wl(m, q, t);
            swapped = 1;
        }
    } while (swapped);
    if (rl(m, a1 + 100))
        return;
    a0 = MOG_LAB_0652;
    for (int i = 0; i < 4; i++, a0 += 4) {              /* LAB_0DF5 */
        uint32_t k = rl(m, a0);
        if (k == a1 || rl(m, k + 54) == 4)
            continue;
        if (!rw(m, MOG_LAB_067C)) {
            uint32_t d0 = chance_0DF8(m);
            if ((uint16_t)d0 != 2) {
                if (d0)
                    continue;
                if (chance_0DFC(m))
                    continue;
            }
        }
        wl(m, a1 + 100, k);                             /* LAB_0DF6 */
        return;
    }
}

/* LAB_0E27 : objet 14 contre la cible : son butin */
static void steal(MogCombat *m)
{
    uint32_t a0 = CUR;
    if (!rl(m, a0 + 100))
        return;
    uint32_t a1 = rl(m, a0 + 96);
    if (!rb(m, a1 + 14))
        return;
    wb(m, a1 + 14, (uint8_t)(rb(m, a1 + 14) - 1));
    uint32_t foe = rl(m, a0 + 100);
    magic_sound(m);
    mog_loot(m, a0, foe);
}

/* LAB_0E29 : objet 10 si la cible est loin : déplacement doublé */
static void boots(MogCombat *m)
{
    uint32_t a0 = CUR, a2 = rl(m, a0 + 96);
    if (!rb(m, a2 + 10) || !rl(m, a0 + 100))
        return;
    uint32_t a1 = rl(m, a0 + 100);
    int16_t d3 = (int16_t)distance(rw(m, a0 + 126), rw(m, a0 + 128), rw(m, a1 + 126), rw(m, a1 + 128));
    int16_t d0 = (int16_t)((int8_t)rb(m, CUR + 86) << 4);
    if (d3 < d0)
        return;
    magic_sound(m);
    wb(m, a2 + 10, (uint8_t)(rb(m, a2 + 10) - 1));
    ww(m, MOG_LAB_0665, (uint16_t)(rw(m, MOG_LAB_0665) << 1));
}

/* LAB_0E2D : achat envisagé (LAB_08F7 prix, LAB_08F8 genre, LAB_08F9) */
static int want_to_buy(MogCombat *m)
{
    wl(m, MOG_LAB_066C, 0);
    uint32_t a0 = CUR;
    int16_t gold = sw(rw(m, a0 + 74));
    if (gold <= 0x0A)
        return 0;
    if (gold > 0x19 && !((int8_t)rb(m, a0 + 73) > 2)) {
        ww(m, MOG_LAB_08F7, 0x19);                      /* une vie */
        ww(m, MOG_LAB_08F8, 0x49);
        return 1;
    }
    uint32_t armour = rl(m, a0 + 92);
    if (!(gold < 0x4B)) {
        if (armour != 0x1E) {
            ww(m, MOG_LAB_08F7, 0x4B);
            ww(m, MOG_LAB_08F8, 0x5C);
            wl(m, MOG_LAB_08F9, 0x1E);
            return 1;
        }
    } else if (!(gold < 0x32)) {
        if (!((int32_t)armour >= 0x1D)) {
            ww(m, MOG_LAB_08F7, 0x32);
            ww(m, MOG_LAB_08F8, 0x5C);
            wl(m, MOG_LAB_08F9, 0x1D);
            return 1;
        }
    } else if (!(gold < 0x1E)) {
        if (!((int32_t)armour >= 0x1C)) {
            ww(m, MOG_LAB_08F7, 0x1E);
            ww(m, MOG_LAB_08F8, 0x5C);
            wl(m, MOG_LAB_08F9, 0x1C);
            return 1;
        }
    }
    /* LAB_0E31 : armes (l'épée longue est notée, puis remplacée par
     * l'épée si elle suffit : LAB_08F9 garde alors $18) */
    uint32_t weapon = rl(m, a0 + 88);
    if (!(gold < 0x19)) {
        if (weapon == 0x18)
            goto daggers;
        ww(m, MOG_LAB_08F7, 0x19);
        ww(m, MOG_LAB_08F8, 0x58);
        wl(m, MOG_LAB_08F9, 0x18);
    }
    if (!(gold < 0x0A) && !((int32_t)weapon >= 0x17)) { /* LAB_0E32 */
        ww(m, MOG_LAB_08F7, 0x0A);
        ww(m, MOG_LAB_08F8, 0x58);
        wl(m, 0x58, 0x17);                              /* EXT_0005 : sic */
        return 1;
    }
daggers:
    if ((int8_t)rb(m, a0 + 76) > 5)                     /* LAB_0E33 */
        return 0;
    ww(m, MOG_LAB_08F7, 2);
    ww(m, MOG_LAB_08F8, 0x4C);
    ww(m, MOG_LAB_08F9, 0);
    return 1;
}

/* LAB_0E35 : la ville la plus proche (cases (12, 7) et (37, 20)) */
static void nearest_town(MogCombat *m)
{
    uint32_t a0 = CUR;
    uint16_t cx = rw(m, a0 + 66), cy = rw(m, a0 + 68);
    ww(m, MOG_LAB_066F, (uint16_t)distance(cx, cy, 0x0C, 0x07));
    uint16_t d3 = (uint16_t)distance(cx, cy, 0x25, 0x14);
    ww(m, MOG_LAB_0670, d3);
    wl(m, MOG_LAB_066C, sw(d3) < sw(rw(m, MOG_LAB_066F)) ? 0x0129009Du : 0x005E002Fu);
}

/* LAB_0E1C : d1 est-il dans la liste SECSTRT_2 (au décalage d2) ? */
static int in_places(MogCombat *m, uint32_t d1, uint32_t d2)
{
    int d0 = 0;
    uint32_t a2 = MOG_SECSTRT_2;
    for (int i = 0; i < 7 && rl(m, a2); i++, a2 += 8)
        if (rl(m, a2 + d2) == d1)
            d0 = 1;
    return d0;
}

/* LAB_0E37 : achat du chevalier noir en ville */
static void buy(MogCombat *m)
{
    uint32_t a0 = CUR;
    ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) - rw(m, MOG_LAB_08F7)));
    switch (rw(m, MOG_LAB_08F8)) {
    case 0x58:
        wl(m, a0 + 88, rl(m, MOG_LAB_08F9));
        break;
    case 0x5C:
        wl(m, a0 + 92, rl(m, MOG_LAB_08F9));
        mog_update_knight(m, a0);                       /* LAB_0013, LAB_0019 */
        break;
    case 0x49:
        ww(m, a0 + 80, rw(m, a0 + 84));
        wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        break;
    case 0x4C:
        for (;;) {                                      /* LAB_0E3A (sic) */
            wb(m, a0 + 76, (uint8_t)(rb(m, a0 + 76) + 1));
            if (sw(rw(m, a0 + 74)) < 2 || rw(m, a0 + 76) == 0x0A)
                break;
            wb(m, a0 + 74, (uint8_t)(rb(m, a0 + 74) - 2));
        }
        break;
    }
}

/* SECSTRT_36 : retour sur la carte (entités, couleurs, chevalier du tour,
 * PV maximum, clavier) */
static void back_to_map(MogCombat *m)
{
    mog_reset_entities(m);                              /* LAB_0305 */
    map_colours_off(m);                                 /* LAB_0DC8 */
    select_knight(m);                                   /* LAB_0DBD */
    uint32_t k = MOG_LAB_0613;                          /* LAB_0011 */
    for (int i = 0; i < 4; i++, k += IX_OBJECT_SIZE)
        mog_update_knight(m, k);
    clear_keys(m);                                      /* LAB_0B82 */
}

/* LAB_0065 : remise à zéro avant un combat */
static void before_combat(MogCombat *m)
{
    mog_reset_entities(m);                              /* LAB_0305 */
    uint32_t a0 = rl(m, MOG_LAB_05C3);                  /* LAB_02CE */
    for (uint32_t i = 0; i < 0xA50; i++)
        wb(m, a0 + i, 0);
    mog_boot_tables_0155(VM);                           /* LAB_0155 */
    wl(m, MOG_SECSTRT_39, MOG_LAB_08D8);                /* LAB_03F0 */
    ww(m, MOG_LAB_0E91, 2);
    ww(m, MOG_LAB_0E92, 2);
    mog_wait_vbls(m, 0x24);
    clear_keys(m);                                      /* LAB_0B82 */
    wl(m, MOG_LAB_05F4, 0);
    ww(m, MOG_LAB_0667, 0);
}

/* Combat_StartPvP [LAB_004F] : duel entre les chevaliers a0 et a1 (joueurs
 * au joystick 2 puis 1, chevaliers noirs à l'ordinateur), butin ou écran
 * du vainqueur. Renvoie MOG_MAP_UNPORTED si un écran non porté suit. */
static int pvp(MogCombat *m, uint32_t a0, uint32_t a1)
{
    map_colours_off(m);                                 /* LAB_0DC8 */
    wl(m, MOG_v_Combatants, a0);
    wl(m, MOG_v_Combatants + 4, a1);
    int fight = !rb(m, a1 + 82) && rb(m, a1 + 73);
    if (fight && rl(m, a1 + 54) != 4) {                 /* LAB_0058 */
        wl(m, MOG_LAB_05D2, a1);
        if (rb(m, rl(m, a1 + 96) + 18)) {
            todo(m, "LAB_0058 (fuite du défenseur, LAB_04CF 9)");
            return MOG_MAP_UNPORTED;
        }
    }
    if (fight) {
        a0 = rl(m, MOG_v_Combatants);
        a1 = rl(m, MOG_v_Combatants + 4);
        if (rl(m, a0 + 54) == 4 && rl(m, a1 + 54) == 4) {
            back_to_map(m);
            return 0;
        }
        uint8_t port = 2;                               /* LAB_0050 */
        if (rl(m, a0 + 54) != 4) {
            wb(m, a0 + 77, 0x0C);
            wb(m, a0 + 11, port);
            port = 1;
        }
        if (rl(m, a1 + 54) != 4) {
            wb(m, a1 + 77, 0x0C);
            wb(m, a1 + 11, port);
        }
        before_combat(m);                               /* LAB_0065 */
        mog_encounter_init(m, MOG_LAB_0164);
        mog_combat_run(m);
        ww(m, MOG_LAB_05AD, 0);
        a0 = rl(m, MOG_v_Combatants);
        a1 = rl(m, MOG_v_Combatants + 4);
        if (rb(m, MOG_LAB_05DC) == 3) {                 /* LAB_0055 : les deux à terre */
            if (rl(m, a0 + 54) != 4) {
                todo(m, "LAB_04CF 9 (après un double K.-O.)");
                return MOG_MAP_UNPORTED;
            }
            back_to_map(m);
            return 0;
        }
        if (rb(m, MOG_LAB_05DC) & 1) {                  /* le premier à terre */
            ww(m, MOG_LAB_05AD, 1);
            wl(m, MOG_v_Combatants + 4, a0);
            wl(m, MOG_v_Combatants, a1);
            a0 = rl(m, MOG_v_Combatants);
            a1 = rl(m, MOG_v_Combatants + 4);
        }
    }
    if (rl(m, a0 + 54) == 4) {                          /* LAB_0057 */
        ww(m, a0 + 78, (uint16_t)(rw(m, a0 + 78) + 1));
        mog_loot(m, a0, a1);
        back_to_map(m);
        return 0;
    }
    todo(m, "LAB_04CF 1 (écran du vainqueur)");
    return MOG_MAP_UNPORTED;
}

/* LAB_0E17 : arrivé en ville, sur la cible ou sur la créature visée */
static int arrived(MogCombat *m)
{
    if (rl(m, MOG_LAB_066C)) {
        if (in_places(m, 0x19, 4) || in_places(m, 0x1A, 4)) {
            ww(m, MOG_LAB_0655, rw(m, MOG_LAB_0665));
            buy(m);
            wl(m, MOG_LAB_066C, 0);
        }
        return 0;
    }
    uint32_t a0 = CUR, d1 = rl(m, a0 + 100);
    if (d1) {
        if (!in_places(m, d1, 0))
            return 0;
        int ev = pvp(m, CUR, rl(m, CUR + 100));
        ww(m, MOG_LAB_0655, rw(m, MOG_LAB_0665));
        if (ev)
            return ev;
    }
    if (in_places(m, rl(m, MOG_LAB_0673), 0))           /* LAB_0E1A */
        ww(m, MOG_LAB_0655, rw(m, MOG_LAB_0665));
    return 0;
}

/* LAB_0E0C : direction vers la cible (ligne de Bresenham) */
static uint16_t steer(MogCombat *m)
{
    ww(m, MOG_LAB_0656, 0);
    uint32_t a0 = CUR;
    if (!rw(m, MOG_LAB_0674)) {
        ww(m, MOG_LAB_0674, 1);
        uint16_t d0 = rw(m, a0 + 126), d1 = rw(m, a0 + 128), d2, d3;
        if (rl(m, a0 + 100)) {
            uint32_t a1 = rl(m, a0 + 100);
            d2 = rw(m, a1 + 126);
            d3 = rw(m, a1 + 128);
        } else if (rl(m, MOG_LAB_066C)) {
            d2 = rw(m, MOG_LAB_066C);
            d3 = rw(m, MOG_LAB_066D);
        } else {
            uint32_t c = rl(m, MOG_LAB_0673);
            d2 = rw(m, c + 10);
            d3 = rw(m, c + 12);
        }
        uint16_t d4 = 1, d5 = 4, d6 = 0, d7;
        d2 = (uint16_t)(d2 - d0);
        if (sw(d2) < 0) { d2 = (uint16_t)-d2; d4 = 2; }
        d3 = (uint16_t)(d3 - d1);
        if (sw(d3) < 0) { d3 = (uint16_t)-d3; d5 = 8; }
        d7 = d2;
        if (sw(d2) < sw(d3)) { d6 = 0xFFFF; d7 = d3; }
        ww(m, MOG_LAB_0675, d4);
        ww(m, MOG_LAB_0676, d5);
        ww(m, MOG_LAB_0677, d7);
        ww(m, MOG_LAB_0678, d2);
        ww(m, MOG_LAB_0679, d3);
        ww(m, MOG_LAB_067A, d6);
    }
    uint16_t d0, d1;
    if (sw(rw(m, MOG_LAB_067A)) >= 0) {                 /* LAB_0E13 */
        d0 = rw(m, MOG_LAB_0675);
        d1 = (uint16_t)(rw(m, MOG_LAB_0677) - rw(m, MOG_LAB_0679));
        if (sw(d1) < 0) {
            d1 = (uint16_t)(d1 + rw(m, MOG_LAB_0678));
            d0 |= rw(m, MOG_LAB_0676);
        }
    } else {                                            /* LAB_0E15 */
        d0 = rw(m, MOG_LAB_0676);
        d1 = (uint16_t)(rw(m, MOG_LAB_0677) - rw(m, MOG_LAB_0678));
        if (sw(d1) < 0) {
            d1 = (uint16_t)(d1 + rw(m, MOG_LAB_0679));
            d0 |= rw(m, MOG_LAB_0675);
        }
    }
    ww(m, MOG_LAB_0677, d1);
    ww(m, MOG_LAB_0656, d0);
    return d0;
}

/* ------------------------------------------------------------------ */
/* Manches                                                             */
/* ------------------------------------------------------------------ */

/* LAB_04A3 : tirage de 0 à 99 */
static uint32_t d100(MogCombat *m)
{
    uint32_t d0 = mog_random(m) & 0x7F;
    if (d0 >= 0x64)
        d0 -= 0x1B;
    return d0;
}

/* LAB_0442 : nombre (< 1000) écrit en a2 (« 3 espaces » d'abord) ;
 * renvoie la fin */
uint32_t mog_number(MogCombat *m, uint32_t d0, uint32_t a2)
{
    for (int i = 0; i < 3; i++)
        wb(m, a2 + (uint32_t)i, 0x20);
    wb(m, a2 + 3, 0);
    uint32_t d1 = d0, d2 = d0;
    int16_t q = (int16_t)((int32_t)d0 / 100);
    if (q) {
        wb(m, a2++, rb(m, MOG_LAB_08E8 + (uint32_t)(int32_t)q));
        d2 = (d2 & 0xFFFF0000u) | (uint16_t)(d2 - (uint16_t)(q & 0xFF) * 100u);
    }
    q = (int16_t)((int32_t)d2 / 10);                    /* LAB_0443 */
    if (q) {
        wb(m, a2++, rb(m, MOG_LAB_08E8 + (uint32_t)(int32_t)q));
        d2 = (d2 & 0xFFFF0000u) | (uint16_t)(d2 - (uint16_t)(q & 0xFF) * 10u);
    } else if (!((int32_t)d1 < 0x64)) {                 /* LAB_0444 */
        wb(m, a2++, rb(m, MOG_LAB_08E8));
    }
    wb(m, a2++, rb(m, MOG_LAB_08E8 + (uint32_t)(int32_t)(int16_t)d2));   /* LAB_0445 */
    return a2;
}

/* LAB_046C : or trouvé (10 à 31) : pour le chevalier (d3 = 0, message
 * LAB_0931 « n gold ») ou pour la créature LAB_08C6 */
static void find_gold(MogCombat *m, int d3)
{
    uint32_t d0 = mog_random(m) & 0x1F;
    if (d0 > 0x15)
        d0 -= 0x0A;
    d0 += 0x0A;
    if (d3) {
        uint32_t a0 = rl(m, MOG_LAB_08C6);
        ww(m, a0 + 8, (uint16_t)(rw(m, a0 + 8) + d0));
        return;
    }
    uint32_t a0 = CUR;
    ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) + d0));
    mog_number(m, d0, MOG_LAB_0931);
    uint32_t a2 = MOG_LAB_0931;
    for (int i = 0; i < 5 && rb(m, a2); i++)
        a2++;
    static const char gold[] = " gold";
    for (int i = 0; i < 6; i++)
        wb(m, a2 + (uint32_t)i, (uint8_t)gold[i]);
    ww(m, MOG_LAB_090B, 2);
}

/* LAB_0471 : objet trouvé (table LAB_090E), jamais deux fois de suite ;
 * d3 : 0 le chevalier (nom dans LAB_092F), 1 la créature, 2 LAB_0690 */
static void find_item(MogCombat *m, int d3)
{
    uint32_t d0, a0, a1;
    for (;;) {
        d0 = d100(m);
        a0 = MOG_LAB_090E;
        for (int d7 = 11; d7 >= 0; d7--) {
            uint32_t t = rl(m, a0);
            a0 += 4;
            if ((int32_t)d0 <= (int32_t)t)
                break;
            a0 += 4;
        }
        d0 = rl(m, a0);                                 /* LAB_0473 */
        if (d0 == rl(m, MOG_LAB_090D))
            continue;
        wl(m, MOG_LAB_090D, d0);
        a1 = CUR;
        a0 = rl(m, a1 + 96);
        if (d3 == 1) {
            a1 = rl(m, MOG_LAB_08C6);
            a0 = rl(m, a1);
        } else if (d3 == 2) {
            a0 = MOG_LAB_0690;
        }
        if (d0 == 4) {                                  /* LAB_0475 : l'épée magique */
            if (rb(m, a0 + 4))
                continue;
            if (!d3)
                wl(m, a1 + 88, 0x19);
        }
        break;
    }
    wb(m, a0 + d0, (uint8_t)(rb(m, a0 + d0) + 1));
    if (!d3) {
        if (d0 == 6) {
            uint32_t k = CUR;
            ww(m, k + 80, (uint16_t)(rw(m, k + 80) + 0x14));
            mog_knight_hp(m, k);
        }
        uint32_t a4 = MOG_LAB_090F;                     /* LAB_0477 : nom */
        uint16_t id = (uint16_t)rl(m, MOG_LAB_090D);
        while (rw(m, a4) != id)
            a4 += 6;
        a4 = rl(m, a4 + 2);
        uint32_t a5 = MOG_LAB_092F;
        uint8_t c;
        do {
            c = rb(m, a4++);
            wb(m, a5++, c);
        } while (c);
    }
    ww(m, MOG_LAB_090B, 1);
}

/* LAB_045E : événement de la manche d'un chevalier noir : objet, gain de
 * caractéristique, or, ou envoûtement (82 = 3) */
static void random_event(MogCombat *m)
{
    for (;;) {
        wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3));
        uint32_t a0 = CUR;
        uint32_t d0 = d100(m);
        d0 = (d0 & 0xFFFFFF00u) | (uint8_t)(d0 + rb(m, a0 + 83));     /* ADD.B */
        if ((int32_t)d0 <= 0x1E) {                      /* LAB_0461 */
            find_item(m, 0);
            ww(m, MOG_LAB_0908, (uint16_t)((rw(m, MOG_LAB_0908) + 1) & 3));
            wl(m, MOG_LAB_0909, rl(m, MOG_LAB_0907 + (uint32_t)(uint16_t)(rw(m, MOG_LAB_0908) << 2)));
            return;
        }
        if ((int32_t)d0 <= 0x46) {                      /* LAB_0462 */
            uint32_t entry = 0;
            uint32_t d1 = pick_stat(m, &entry);
            if (!(uint16_t)d1)
                continue;
            uint32_t k = CUR, a = k + (uint32_t)(int32_t)d1;
            wb(m, a, (uint8_t)(rb(m, a) + 1));
            wl(m, MOG_LAB_0909, rl(m, MOG_LAB_090A + entry + 4));
            if ((uint16_t)d1 == 0x47) {
                mog_knight_hp(m, CUR);
                ww(m, CUR + 80, (uint16_t)(rw(m, CUR + 80) + 0x0A));
            }
            if ((uint16_t)d1 == 0x48)
                mog_knight_defence(m, CUR);
            ww(m, MOG_LAB_090C, (uint16_t)d1);
            ww(m, MOG_LAB_090B, 3);
            return;
        }
        if ((int32_t)d0 <= 0x5A) {                      /* LAB_045F */
            find_gold(m, 0);
            uint16_t n = (uint16_t)(rw(m, MOG_LAB_0906) + 1);
            if (!(sw(n) < 3))
                n = 0;
            ww(m, MOG_LAB_0906, n);
            wl(m, MOG_LAB_0909, rl(m, MOG_LAB_0905 + (uint32_t)(uint16_t)(n << 2)));
            return;
        }
        if (rb(m, a0 + 83) == 0xFF)
            continue;
        wb(m, a0 + 82, 3);
        ww(m, MOG_LAB_090B, 4);
        wl(m, MOG_LAB_0909, MOG_LAB_092D);
        return;
    }
}

/* LAB_0030 : chaque chevalier noir vivant tente sa chance (LAB_045E) ;
 * sort (130) : une vie de moins */
static void black_knights_events(MogCombat *m)
{
    wl(m, MOG_LAB_0035, rl(m, MOG_LAB_0633));
    uint32_t a0 = MOG_LAB_0613;
    for (int i = 0; i < 4; i++, a0 += IX_OBJECT_SIZE) {
        if (rl(m, a0 + 54) == 4) {
            wb(m, a0 + 83, 0xFF);
            wl(m, MOG_LAB_0633, a0);
            if ((int8_t)rb(m, a0 + 73) > 0) {
                if (sw(rw(m, a0 + 74)) < 0)
                    ww(m, a0 + 74, 0);
                random_event(m);                        /* LAB_045E */
            }
        }
        if (rb(m, a0 + 130))
            wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) - 1));
    }
    wl(m, MOG_LAB_0633, rl(m, MOG_LAB_0035));
}

/* LAB_0029 : nouvelle manche (saison tous les 4 jours), PV regagnés */
static void new_round(MogCombat *m)
{
    wl(m, MOG_LAB_05C4, rl(m, MOG_LAB_05E2 + 4));
    uint32_t v = MOG_v_Combatants;
    ww(m, v + 20, (uint16_t)(rw(m, v + 20) + 1));
    if (!(sw(rw(m, v + 20)) <= 3)) {
        ww(m, MOG_LAB_06C0, (uint16_t)(rw(m, MOG_LAB_06C0) + 1));
        ww(m, v + 20, 0);
        ww(m, MOG_LAB_06C1, (uint16_t)((rw(m, MOG_LAB_06C1) + 1) & 7));
        black_knights_events(m);
        ww(m, v + 18, (uint16_t)(int16_t)(int8_t)rb(m, MOG_LAB_06C2 + rw(m, MOG_LAB_06C1)));
    }
    uint32_t a0 = MOG_LAB_0613;
    for (int i = 0; i < 5; i++, a0 += IX_OBJECT_SIZE) {  /* LAB_002B (dragon compris) */
        if (rb(m, a0 + 83) != 0xFF) {
            int8_t t = (int8_t)(rb(m, a0 + 83) - 0x0A);
            wb(m, a0 + 83, (uint8_t)(t < 0 ? 0 : t));
            if (rb(m, a0 + 82))
                wb(m, a0 + 82, (uint8_t)(rb(m, a0 + 82) - 1));
        }
        uint16_t d1 = (uint16_t)(rw(m, a0 + 84) - rw(m, a0 + 80));   /* LAB_002D */
        if (d1)
            d1 = (uint16_t)((d1 >> 2) | 1);
        ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + d1));
        if (!(sw(rw(m, a0 + 84)) > sw(rw(m, a0 + 80))))
            ww(m, a0 + 80, rw(m, a0 + 84));
    }
}

/* LAB_012B : écran « Next Day » avec l'image de la saison */
static void next_day_screen(MogCombat *m)
{
    /* LAB_0129 */
    uint32_t a2 = rl(m, MOG_LAB_0E93);                  /* LAB_03EB */
    for (int i = 0; i < 33; i++)
        ww(m, a2 + 2u * (unsigned)i, 0);
    mog_clear_screen(m, rl(m, MOG_SECSTRT_35));
    mog_set_planes(m, rl(m, MOG_SECSTRT_35));
    uint32_t src = rl(m, MOG_LAB_05B9 + 56), dst = rl(m, MOG_LAB_0D92);
    for (uint32_t i = 0; i < 0x25F7; i++)
        wb(m, dst + i, rb(m, src + i));
    mog_piv_decode(m, rl(m, MOG_LAB_0D92));
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3 + 16));
    mog_text_records(m, MOG_LAB_0709);
    uint16_t d0 = rw(m, MOG_v_Combatants + 18);
    ww(m, MOG_LAB_0D05, 1);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_05E2 + 4), d0, 0x77, 0x0C);
    ww(m, MOG_LAB_0D05, 0);
    mog_fade_to(m, MOG_LAB_0D2B);
}

/* LAB_00EC : attente du bouton de feu (appui puis relâché) */
static void wait_fire(MogCombat *m)
{
    if (!m->wait_vbl)
        return;                                         /* banc : sans objet */
    while (!(m->joy[1] & MOG_JOY_FIRE))
        mog_wait_vbls(m, 1);
    while (m->joy[1] & MOG_JOY_FIRE)
        mog_wait_vbls(m, 1);
}

/* ------------------------------------------------------------------ */
/* Boucle                                                              */
/* ------------------------------------------------------------------ */

void mog_map_enter(MogCombat *m)
{
    m->planes = 1;                                      /* dessins dans les écrans */
    clear_keys(m);                                      /* LAB_0DAB */
    mog_set_planes(m, rl(m, MOG_LAB_05C0));
    uint32_t src = rl(m, MOG_LAB_05B9 + 92), dst = rl(m, MOG_LAB_05C2);
    for (uint32_t i = 0; i < 0x8A03; i++)
        wb(m, dst + i, rb(m, src + i));
    mog_piv_decode(m, rl(m, MOG_LAB_05C2));
    draw_places(m);                                     /* LAB_0DA3 */
    draw_other_knights(m);                              /* LAB_0D9E */
    copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_SECSTRT_35));   /* LAB_0418 */
    copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    select_knight(m);                                   /* LAB_0DBD */
    move_knight(m);                                     /* LAB_0DA6 */
    draw_current(m);                                    /* LAB_0D9B */
    mog_swap_screens(m);                                    /* LAB_0416 */
    map_colours_on(m);                                  /* LAB_0DC5 */
    ww(m, MOG_v_FrameVbls, 2);
}

/* Fin du tour du chevalier (LAB_0DBA) : le suivant, nouvelle manche après
 * le 4e ; renvoie MOG_MAP_ENTER pour redessiner la carte, MOG_MAP_OVER
 * si tous les joueurs sont morts. */
static int end_turn(MogCombat *m)
{
    for (;;) {
        ww(m, MOG_LAB_0660, 0);                         /* LAB_0E04 */
        ww(m, MOG_LAB_065E, 0);
        ww(m, MOG_LAB_065C, 0);
        ww(m, MOG_LAB_0654, (uint16_t)(rw(m, MOG_LAB_0654) + 1));
        ww(m, MOG_LAB_0655, 0);
        ww(m, MOG_LAB_0654, rw(m, MOG_LAB_0654) & 3);
        if (!rw(m, MOG_LAB_0654)) {
            new_round(m);                               /* LAB_0029 */
            map_colours_off(m);                         /* LAB_0DC8 */
            next_day_screen(m);                         /* LAB_012B */
            wait_fire(m);                               /* LAB_00EC */
            uint32_t a2 = rl(m, MOG_LAB_0E93);          /* LAB_03EB */
            for (int i = 0; i < 33; i++)
                ww(m, a2 + 2u * (unsigned)i, 0);
        }
        select_knight(m);                               /* LAB_0DBB */
        uint32_t a0 = CUR;
        wl(m, a0 + 100, 0);
        wl(m, MOG_LAB_066C, 0);
        ww(m, MOG_LAB_0674, 0);
        ww(m, MOG_LAB_067B, 0);
        if ((int8_t)rb(m, a0 + 82) > 0)
            continue;                                   /* ensorcelé */
        if ((int8_t)rb(m, a0 + 73) > 0)
            return MOG_MAP_ENTER;
        if (rl(m, a0 + 54) == 4)
            continue;
        ww(m, MOG_LAB_0663, (uint16_t)(rw(m, MOG_LAB_0663) + 1));
        if (rw(m, MOG_LAB_0663) == rw(m, MOG_LAB_05C5))
            return MOG_MAP_OVER;                        /* LAB_0064 */
    }
}

int mog_map_frame(MogCombat *m)
{
    mog_frame_start(m);                                 /* Combat_FrameStart */
    uint32_t a0 = CUR;
    uint16_t d1 = 0;
    int cpu = rl(m, a0 + 54) == 4;
    if (cpu) {
        sort_creatures(m);                              /* LAB_0DDF */
        pick_creature(m);                               /* LAB_0DEA */
        train(m);                                       /* LAB_0E2B */
        use_potion(m);                                  /* LAB_0E23 */
        if (!rw(m, MOG_LAB_066B)) {
            pick_knight(m);                             /* LAB_0DED */
            steal(m);                                   /* LAB_0E27 */
            boots(m);                                   /* LAB_0E29 */
            if (want_to_buy(m))                         /* LAB_0E2D */
                nearest_town(m);                        /* LAB_0E35 */
        }
        int ev = arrived(m);                            /* LAB_0E17 */
        if (ev)
            return ev;
        terrain_slow(m);                                /* LAB_0DD8 */
        ww(m, MOG_LAB_0655, (uint16_t)(rw(m, MOG_LAB_0655) + 1));
        if (!rw(m, MOG_L36_008E4))
            d1 = steer(m);                              /* LAB_0E0C */
    } else {
        terrain_slow(m);
        if (!rw(m, MOG_L36_008E4)) {
            ww(m, MOG_LAB_062F, m->joy[0]);             /* LAB_00EE */
            ww(m, MOG_LAB_0630, m->joy[1]);
            d1 = m->joy[1];
        }
    }
    ww(m, MOG_LAB_0656, d1);                            /* LAB_0DB0 */
    int skip_keys = 0;
    if (!rw(m, MOG_LAB_065C) && !rw(m, MOG_LAB_065E)) {
        if (rl(m, MOG_LAB_0662))
            skip_keys = 1;                              /* vers LAB_0DB4 */
        else if ((d1 & 15) && rl(m, CUR + 54) != 4)
            ww(m, MOG_LAB_0655, (uint16_t)(rw(m, MOG_LAB_0655) + 1));
    }
    if (!skip_keys) {                                   /* LAB_0DB1 */
        uint16_t key = rw(m, MOG_SECSTRT_21);
        uint8_t c = rb(m, MOG_LAB_0D99 + key);          /* LAB_0D8D */
        if (rl(m, CUR + 54) != 4) {
            if (c == 0x20) {
                map_colours_off(m);
                todo(m, "LAB_04CF (écran d'inventaire)");
                clear_keys(m);
                return MOG_MAP_UNPORTED;
            }
            if (c == 0x45)
                ww(m, MOG_LAB_0655, rw(m, MOG_LAB_0665));
        }
        if (c == 0x51)
            return MOG_MAP_OVER;                        /* LAB_0064 */
    }
    if (rw(m, MOG_LAB_0656) & 0x10) {                   /* LAB_0DB4 : feu */
        if (rw(m, MOG_LAB_065C)) {
            ww(m, MOG_LAB_0660, 0);
            ww(m, MOG_LAB_065E, 0);
            ww(m, MOG_LAB_065C, 0);
        }
        if (rl(m, MOG_SECSTRT_2)) {                     /* LAB_0E3D */
            todo(m, "LAB_0E3D (lieu, rencontre)");
            return MOG_MAP_UNPORTED;
        }
    }
    uint32_t d = MOG_LAB_0617;                          /* LAB_0DB6 : le dragon */
    if (!((int8_t)rb(m, d + 73) < 0) && rw(m, MOG_LAB_0667) && rl(m, d + 100) == CUR) {
        for (uint32_t i = 0; i < 4; i++)
            if (rl(m, MOG_LAB_069E + 4 * i) == CUR) {
                map_colours_off(m);
                todo(m, "LAB_0083 (le dragon attaque)");
                return MOG_MAP_UNPORTED;
            }
    }
    if (!(sw(rw(m, MOG_LAB_0655)) < sw(rw(m, MOG_LAB_0665)))) {   /* LAB_0DB9 */
        ww(m, MOG_LAB_0663, 0);
        return end_turn(m);
    }
    move_knight(m);                                     /* LAB_0DBC */
    draw_current(m);
    mog_swap_screens(m);
    copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
    mog_frame_wait(m);
    return MOG_MAP_CONTINUE;
}
