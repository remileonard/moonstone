/*
 * prog_intro.c — scènes de l'intro de program traduites de
 * amiga_asm/program.asm, sur la mémoire d'origine (mêmes adresses).
 *
 * LAB_05A5 : défilement vertical d'une carte de tuiles 32 × 25 (SECSTRT_33,
 * 10 tuiles par ligne, planches LAB_05D6) dessinée dans l'écran LAB_05D7,
 * puis recopiée dans l'écran de dessin et montrée (LAB_0262).
 */
#include "prog_intro.h"
#include "prog_blit.h"
#include "prog_vbl.h"
#include "ix_program_syms.h"

#include <stdio.h>

#define VM (p->vm)
#define PLANE 8000u                     /* $1F40 : un plan de 320 × 200 */
#define COPPER_BPL 0x7F6B0u             /* EXT_0016... : pointeurs de plans */

static uint16_t rw(ProgIntro *p, uint32_t a) { return ix_rw(VM, a); }
static uint32_t rl(ProgIntro *p, uint32_t a) { return ix_rl(VM, a); }
static void ww(ProgIntro *p, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wl(ProgIntro *p, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }

/* ---------------------------------------------------------------- VBL */

/* SECSTRT_15 : compte à rebours du moteur du lecteur (LAB_0316 : CIA
 * seulement), puis les serveurs de la liste LAB_0372 */
static void servers(ProgIntro *p)
{
    if (!rw(p, PROGRAM_L18_0003A) && !(rw(p, PROGRAM_LAB_038D) & 0x8000))
        ww(p, PROGRAM_LAB_038D, (uint16_t)(rw(p, PROGRAM_LAB_038D) - 1));
    for (uint32_t a = PROGRAM_LAB_0372; rl(p, a); a += 4) {
        uint32_t s = rl(p, a);
        if (s == PROGRAM_LAB_057D)
            prog_vbl_colours(VM, p->colour);
        else if (s == PROGRAM_LAB_005C) {
            if (p->music_vbl)
                p->music_vbl(p);
        } else {
            static int warned;
            if (!warned++)
                fprintf(stderr, "prog : serveur VBL inconnu %06X\n", s);
        }
    }
}

void prog_wait_vbl(ProgIntro *p)
{
    /* LAB_0331 (VBL) ; LAB_034D (souris) n'est pas repris */
    if (rw(p, PROGRAM_LAB_0363))
        wl(p, PROGRAM_LAB_0379, rl(p, PROGRAM_LAB_0379) + 1);
    servers(p);
    p->vbls++;
    if (p->vbl)
        p->vbl(p);
}

/* LAB_054F : d0 VBL */
static void wait_vbls(ProgIntro *p, uint32_t n)
{
    while (n--)
        prog_wait_vbl(p);
}

/* LAB_0017 / LAB_0018 : cadence, LAB_00D0 + LAB_0123 VBL par image */
static void frame_mark(ProgIntro *p)
{
    wl(p, PROGRAM_LAB_0122, rl(p, PROGRAM_LAB_0379));
}

static void frame_wait(ProgIntro *p)
{
    uint32_t d0 = rl(p, PROGRAM_LAB_0379) - rl(p, PROGRAM_LAB_0122);
    int32_t d1 = (int32_t)(rl(p, PROGRAM_LAB_00D0) + rl(p, PROGRAM_LAB_0123) - d0);
    wait_vbls(p, d1 < 0 ? 0 : (uint32_t)d1);
}

/* LAB_0565 : une VBL puis la palette a0 dans COLOR00-31 */
static void set_palette(ProgIntro *p, uint32_t a0)
{
    prog_wait_vbl(p);
    for (int i = 0; i < 32; i++)
        p->colour[i] = rw(p, a0 + 2u * (unsigned)i);
}

/* ------------------------------------------------------------- écrans */

/* LAB_054C : VBL, l'écran LAB_056C montré (copper), échangé avec SECSTRT_30 */
static void show_screen(ProgIntro *p)
{
    prog_wait_vbl(p);
    uint32_t s = rl(p, PROGRAM_LAB_056C);
    for (uint32_t i = 0; i < 5; i++) {
        uint32_t a = s + i * PLANE;
        ww(p, COPPER_BPL + 8 * i, (uint16_t)(a >> 16));
        ww(p, COPPER_BPL + 8 * i + 4, (uint16_t)a);
    }
    wl(p, PROGRAM_LAB_056C, rl(p, PROGRAM_SECSTRT_30));
    wl(p, PROGRAM_SECSTRT_30, s);
}

/* LAB_0262 : écran montré, listes LAB_0279 / LAB_027A échangées, plans de
 * dessin LAB_04D9-LAB_04DD (LAB_026C / LAB_04A6) sur le nouvel LAB_056C */
static void swap_screens(ProgIntro *p)
{
    show_screen(p);
    uint32_t d0 = rl(p, PROGRAM_LAB_0279);
    wl(p, PROGRAM_LAB_0279, rl(p, PROGRAM_LAB_027A));
    wl(p, PROGRAM_LAB_027A, d0);
    wl(p, PROGRAM_LAB_027C, rl(p, PROGRAM_LAB_0279));
    uint32_t s = rl(p, PROGRAM_LAB_056C);
    static const uint32_t pl[5] = { PROGRAM_LAB_04D9, PROGRAM_LAB_04DA, PROGRAM_LAB_04DB,
                                    PROGRAM_LAB_04DC, PROGRAM_LAB_04DD };
    for (uint32_t i = 0; i < 5; i++)
        wl(p, pl[i], s + i * PLANE);
}

/* LAB_0264 : copie d'écran (5 plans) par le blitter */
static void copy_screen(ProgIntro *p, uint32_t a0, uint32_t a1)
{
    MogBlitter *b = &p->blt;
    wl(p, PROGRAM_LAB_0266, a0);
    wl(p, PROGRAM_LAB_0267, a1);
    b->apt = a0;
    b->dpt = a1;
    b->amod = b->dmod = 0;
    b->afwm = b->alwm = 0xFFFF;
    b->con0 = 0x09F0;
    b->con1 = 0;
    for (int i = 0; i < 5; i++) {
        prog_blitter_run(VM, b, 0x3214);
        wl(p, PROGRAM_LAB_0266, rl(p, PROGRAM_LAB_0266) + PLANE);
        wl(p, PROGRAM_LAB_0267, rl(p, PROGRAM_LAB_0267) + PLANE);
    }
}

/* LAB_04E1 : copie A -> D, modulos d0 / d1, d2 mots × d3 lignes */
static void blit_copy(ProgIntro *p, uint32_t a0, uint32_t a1, uint16_t d0, uint16_t d1,
                      uint16_t d2, uint16_t d3)
{
    MogBlitter *b = &p->blt;
    b->afwm = b->alwm = 0xFFFF;
    b->apt = a0;
    b->dpt = a1;
    b->amod = (int16_t)d0;
    b->dmod = (int16_t)d1;
    b->con0 = 0x09F0;
    b->con1 = 0;
    prog_blitter_run(VM, b, (uint16_t)((d3 << 6) | d2));
}

/* MULU puis SUB.W : seul le mot bas est soustrait (sans retenue) */
static uint32_t mulu_subw(uint16_t a, uint16_t n)
{
    uint32_t r = (uint32_t)a * n;
    return (r & 0xFFFF0000u) | (uint16_t)(r - a);
}

/* LAB_04E2 : même copie en descendant (recouvrement vers le bas) */
static void blit_copy_desc(ProgIntro *p, uint32_t a0, uint32_t a1, uint16_t d0, uint16_t d1,
                           uint16_t d2, uint16_t d3)
{
    MogBlitter *b = &p->blt;
    b->afwm = b->alwm = 0xFFFF;
    b->amod = (int16_t)d0;
    b->dmod = (int16_t)d1;
    b->con0 = 0x09F0;
    b->con1 = 2;
    uint32_t d4 = (uint32_t)(uint16_t)(d2 + d2) * d3;
    uint32_t d5 = mulu_subw(d0, d3) + d4;
    uint32_t d6 = mulu_subw(d1, d3) + d4;
    b->apt = a0 + d5 - 2;
    b->dpt = a1 + d6 - 2;
    prog_blitter_run(VM, b, (uint16_t)((d3 << 6) | d2));
}

/* ------------------------------------------------------------- tuiles */

/* LAB_05CC : octet du point (x, y) dans un plan de 40 octets par ligne */
static uint16_t pixel_offset(uint16_t x, uint16_t y)
{
    uint16_t d7 = (uint16_t)((uint16_t)(y << 4) + (uint16_t)(y << 2));
    return (uint16_t)((uint16_t)((x >> 4) + d7) << 1);
}

/* LAB_05C8 : découpage vertical (hauteur LAB_00FC, lignes sautées
 * LAB_05E0) et horizontal ; en haut, une ligne de moins que prévu est
 * sautée (~y au lieu de -y), comme dans l'original. */
static void clip(ProgIntro *p, uint16_t x, uint16_t y)
{
    uint16_t h = rw(p, PROGRAM_LAB_00F9);
    if ((int16_t)(uint16_t)(h + y) > 200) {
        ww(p, PROGRAM_LAB_00FC, (uint16_t)(200 - y));
    } else {
        wl(p, PROGRAM_LAB_05E0, 0);
        ww(p, PROGRAM_LAB_05E1, 0);
        if ((int16_t)y < 0) {
            uint16_t d5 = (uint16_t)~y;
            ww(p, PROGRAM_LAB_00FC, (uint16_t)(h - d5));
            wl(p, PROGRAM_LAB_05E0, (uint16_t)((uint16_t)(d5 << 5) + (uint16_t)(d5 << 3)));
            ww(p, PROGRAM_LAB_05DB, 0);
            ww(p, PROGRAM_LAB_05E3, 0);
        }
    }
    wl(p, PROGRAM_LAB_05DF, 0);
    if ((int16_t)x < 0) {
        ww(p, PROGRAM_LAB_05DA, 0);
        ww(p, PROGRAM_LAB_05E2, 0);
    }
}

/* LAB_05C5 : tuile d2 de la planche a0 en (d0, d1) dans l'écran a1 */
static void draw_tile(ProgIntro *p, uint16_t x, uint16_t y, uint16_t tile,
                      uint32_t bank, uint32_t screen)
{
    ww(p, PROGRAM_LAB_05DA, x);
    ww(p, PROGRAM_LAB_05DB, y);
    if ((int16_t)y >= 200)
        return;
    wl(p, PROGRAM_LAB_05DC, bank);
    wl(p, PROGRAM_LAB_05DD, screen);
    ww(p, PROGRAM_LAB_05DE, tile);
    ww(p, PROGRAM_LAB_00FC, rw(p, PROGRAM_LAB_00F9));
    ww(p, PROGRAM_LAB_00FB, rw(p, PROGRAM_LAB_00FA));
    clip(p, x, y);
    /* LAB_05CD : coin de la tuile (10 par ligne de 32 × 25) */
    uint16_t t = (uint16_t)(rw(p, PROGRAM_LAB_05DE) & 0xFF);
    uint16_t ty = (uint16_t)(t / 10 * 25), tx = (uint16_t)(t % 10 * 32);
    ww(p, PROGRAM_LAB_05D8, tx);
    ww(p, PROGRAM_LAB_05D9, ty);
    uint32_t a0 = rl(p, PROGRAM_LAB_05DC) + pixel_offset(tx, ty) + rl(p, PROGRAM_LAB_05E0);
    wl(p, PROGRAM_LAB_05E4, a0);
    uint32_t a1 = rl(p, PROGRAM_LAB_05DD)
                + pixel_offset(rw(p, PROGRAM_LAB_05DA), rw(p, PROGRAM_LAB_05DB));
    wl(p, PROGRAM_LAB_05E5, a1);
    for (int i = 0; i < 5; i++) {
        blit_copy(p, rl(p, PROGRAM_LAB_05E4), rl(p, PROGRAM_LAB_05E5), 0x24, 0x24, 2,
                  rw(p, PROGRAM_LAB_00FC));
        wl(p, PROGRAM_LAB_05E4, rl(p, PROGRAM_LAB_05E4) + PLANE);
        wl(p, PROGRAM_LAB_05E5, rl(p, PROGRAM_LAB_05E5) + PLANE);
    }
}

/* LAB_05BA : lignes LAB_05BD... (LAB_05BC lignes) de la fenêtre sur la
 * carte, à la position LAB_05B8 (en lignes d'écran) */
static void draw_rows(ProgIntro *p)
{
    uint16_t pos = rw(p, PROGRAM_LAB_05B8);
    uint16_t q = (uint16_t)(pos / 25), r = (uint16_t)(pos % 25);
    uint16_t d1 = (uint16_t)(q * 20);
    ww(p, PROGRAM_LAB_05C3, r);
    ww(p, PROGRAM_L31_009DC, (uint16_t)(25 - r));
    uint16_t bd = rw(p, PROGRAM_LAB_05BD);
    d1 = (uint16_t)(d1 + (uint16_t)(20u * bd));
    uint32_t a0 = PROGRAM_SECSTRT_33 + (uint32_t)(int32_t)(int16_t)d1;
    uint16_t x = 0;
    uint16_t y = (uint16_t)((uint16_t)(bd * 25u) - rw(p, PROGRAM_LAB_05C3));
    uint16_t rows = rw(p, PROGRAM_LAB_05BC);
    for (uint16_t d7 = 0;;) {
        int32_t t = (int16_t)rw(p, a0);              /* EXT.L */
        uint16_t k = (uint16_t)((uint32_t)t / 80u) * 4u;
        uint32_t bank = rl(p, PROGRAM_LAB_05D6 + k);
        uint16_t tile = (uint16_t)(t - (int32_t)rl(p, PROGRAM_L31_0090C + k));
        draw_tile(p, x, y, tile, bank, rl(p, PROGRAM_LAB_05D7));
        a0 += 2;
        x = (uint16_t)(x + 32);
        if (x <= 0x13F)
            continue;
        y = (uint16_t)(y + 25);
        x = 0;
        if ((int16_t)++d7 >= (int16_t)rows)
            break;
    }
}

/* LAB_05B7 : l'écran des tuiles recopié dans l'écran de dessin */
static void tiles_to_draw(ProgIntro *p)
{
    copy_screen(p, rl(p, PROGRAM_LAB_05D7), rl(p, PROGRAM_LAB_056C));
}

/* LAB_05C1 : l'écran monte de L31_00850 lignes, 2 lignes de tuiles en bas */
static void scroll_up(ProgIntro *p)
{
    uint16_t sp = rw(p, PROGRAM_L31_00850);
    uint16_t d0 = (uint16_t)((uint16_t)(sp << 5) + (uint16_t)(sp << 3));
    uint32_t a1 = rl(p, PROGRAM_LAB_05D7);
    uint32_t a0 = rl(p, PROGRAM_LAB_05D7) + (uint32_t)(int32_t)(int16_t)d0;
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy(p, a0, a1, 0, 0, 20, (uint16_t)(200 - rw(p, PROGRAM_L31_00850)));
    ww(p, PROGRAM_LAB_05BD, 7);
    ww(p, PROGRAM_LAB_05BC, 2);
    draw_rows(p);
}

/* LAB_05BF : l'écran descend, 2 lignes de tuiles en haut */
static void scroll_down(ProgIntro *p)
{
    uint16_t sp = rw(p, PROGRAM_L31_00850);
    uint16_t d0 = (uint16_t)((uint16_t)(sp << 5) + (uint16_t)(sp << 3));
    uint32_t a0 = rl(p, PROGRAM_LAB_05D7);
    uint32_t a1 = rl(p, PROGRAM_LAB_05D7) + (uint32_t)(int32_t)(int16_t)d0;
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy_desc(p, a0, a1, 0, 0, 20, (uint16_t)(200 - rw(p, PROGRAM_L31_00850)));
    ww(p, PROGRAM_LAB_05BD, 0);
    ww(p, PROGRAM_LAB_05BC, 2);
    draw_rows(p);
}

/* LAB_05B2 : d1 bit 2 = avance (LAB_05B8 croît, borné à 1000), bit 3 =
 * recul (borné à 0) ; aux bornes, la fenêtre entière est redessinée */
static void scroll(ProgIntro *p, uint16_t d1)
{
    uint16_t d0 = rw(p, PROGRAM_L31_00850);
    if (d1 & 4) {
        uint16_t pos = (uint16_t)(rw(p, PROGRAM_LAB_05B8) + d0);
        ww(p, PROGRAM_LAB_05B8, pos);
        if ((int16_t)pos > 1000) {
            ww(p, PROGRAM_LAB_05B8, 1000);
            ww(p, PROGRAM_LAB_05BD, 0);
            ww(p, PROGRAM_LAB_05BC, 8);
            draw_rows(p);
        } else
            scroll_up(p);
    } else if (d1 & 8) {
        uint16_t pos = (uint16_t)(rw(p, PROGRAM_LAB_05B8) - d0);
        ww(p, PROGRAM_LAB_05B8, pos);
        if (pos & 0x8000) {
            ww(p, PROGRAM_LAB_05B8, 0);
            ww(p, PROGRAM_LAB_05BD, 0);
            ww(p, PROGRAM_LAB_05BC, 8);
            draw_rows(p);
        } else
            scroll_down(p);
    } else
        return;
    tiles_to_draw(p);
}

/* ------------------------------------------------------------- scènes */

void prog_scene_05a5(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x820; i++)                /* LAB_024B */
        ix_wb(VM, PROGRAM_LAB_0286 + i, 0xFF);
    ww(p, PROGRAM_LAB_01CB + 18, 0x0A00);
    ww(p, PROGRAM_LAB_01CB + 20, 0x0600);
    ww(p, PROGRAM_LAB_01CB + 22, 0x0300);
    ww(p, PROGRAM_LAB_01CB + 24, 0x0FC6);
    set_palette(p, PROGRAM_LAB_01CB);
    do {
        /* vitesse selon la position : LAB_05A9 (bornes), L31_00674 */
        uint32_t d0 = 0;
        while ((int16_t)rw(p, PROGRAM_LAB_05A9 + d0) < (int16_t)rw(p, PROGRAM_LAB_05B8))
            d0 += 2;
        ww(p, PROGRAM_L31_00850, rw(p, PROGRAM_L31_00674 + d0));
        frame_mark(p);
        scroll(p, 4);
        swap_screens(p);
        if (rw(p, PROGRAM_L31_00850) == 4 && p->music_start)
            p->music_start(p);                          /* SECSTRT_1 */
        frame_wait(p);
    } while ((int16_t)rw(p, PROGRAM_LAB_05B8) < 1000);
    copy_screen(p, rl(p, PROGRAM_SECSTRT_30), rl(p, PROGRAM_LAB_056C));
    copy_screen(p, rl(p, PROGRAM_SECSTRT_30), rl(p, PROGRAM_LAB_00C6));
}
