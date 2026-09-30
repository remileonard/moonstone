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
    p->a1 = s;
}

/* LAB_024B : listes de zones à restaurer (LAB_0286, LAB_0287) vidées */
static void fill_lists(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x820; i++)
        ix_wb(VM, PROGRAM_LAB_0286 + i, 0xFF);
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
    p->a1 = a1;
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

/* ---------------------------------------------------------- palettes */

/* LAB_0576 : fondu vers la palette a0, un pas toutes les d0 VBL */
static void fade_to(ProgIntro *p, uint32_t a0, uint16_t d0)
{
    wl(p, PROGRAM_LAB_05CF, a0);
    ww(p, PROGRAM_LAB_05D0, d0);
    ww(p, PROGRAM_LAB_05D1, d0);
}

/* LAB_0258 : registres de couleur et palette courante à 0 ; la boucle
 * (DBNE après une écriture de 0) fait 33 tours : un mot de plus. */
static void black(ProgIntro *p)
{
    uint32_t a2 = rl(p, PROGRAM_LAB_05D2);
    for (unsigned i = 0; i < 33; i++) {
        if (i < 32)
            p->colour[i] = 0;
        ww(p, a2 + 2 * i, 0);
    }
}

/* LAB_025D : palette a0 copiée dans la palette courante */
static void copy_palette(ProgIntro *p, uint32_t a0)
{
    uint32_t a1 = rl(p, PROGRAM_LAB_05D2);
    for (unsigned i = 0; i < 32; i++)
        ww(p, a1 + 2 * i, rw(p, a0 + 2 * i));
    p->a1 = a1 + 64;
}

/* LAB_01B7 : 32 couleurs de a0 vers a1 */
static void copy_colours(ProgIntro *p, uint32_t a0, uint32_t a1)
{
    for (unsigned i = 0; i < 32; i++)
        ww(p, a1 + 2 * i, rw(p, a0 + 2 * i));
    p->a1 = a1 + 64;
}

/* L10_00D6A : 40000 octets (écran de 5 plans) */
static void copy_long(ProgIntro *p, uint32_t a0, uint32_t a1)
{
    for (uint32_t i = 0; i < 40000; i += 4)
        wl(p, a1 + i, rl(p, a0 + i));
    p->a1 = a1 + 40000;
}

/* LAB_0263 : décor LAB_00C6 dans les deux écrans */
static void show_background(ProgIntro *p)
{
    copy_screen(p, rl(p, PROGRAM_LAB_00C6), rl(p, PROGRAM_SECSTRT_30));
    copy_screen(p, rl(p, PROGRAM_LAB_00C6), rl(p, PROGRAM_LAB_056C));
}

/* ------------------------------------------------------------ entités */

/* 40 entités de 42 octets (LAB_0282) : +0 active, +1 script en cours,
 * +2 script, +6/+8/+10 x / y / profondeur, +12/+14 position à l'écran,
 * +16/+18 taille, +20 drapeaux, +21 frame, +22 sens (1 ou 3), +24 A1 de
 * l'appelant, +28 planches, +32 contrôleur, +36 bloc de travail (48
 * octets, LAB_0284), +40 cachée. */
#define ENT 42u
#define NENT 40u

static uint32_t ent_addr(unsigned i) { return PROGRAM_LAB_0282 + i * ENT; }

/* LAB_01E4 : entités effacées, blocs de travail attachés */
static void ent_reset(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x690; i++)
        ix_wb(VM, PROGRAM_LAB_0282 + i, 0);
    for (uint32_t i = 0; i < 0x30; i++)
        ix_wb(VM, PROGRAM_LAB_0284 + i, 0);
    fill_lists(p);
    for (unsigned i = 0; i < NENT; i++)
        wl(p, ent_addr(i) + 36, PROGRAM_LAB_0284 + i * 0x30u);
    p->a1 = PROGRAM_LAB_0284 + NENT * 0x30u;
}

/* LAB_01DA : première entité libre (sinon LAB_0277 = 2) */
static void ent_spawn(ProgIntro *p, uint32_t script, uint32_t banks, uint16_t x, uint16_t y,
                      uint16_t depth, uint8_t dir, uint8_t ctl)
{
    for (unsigned i = 0; i < NENT; i++) {
        uint32_t a6 = ent_addr(i);
        if (ix_rb(VM, a6))
            continue;
        uint32_t a5 = rl(p, a6 + 36);
        for (uint32_t k = 0; k < 0x30; k++)
            ix_wb(VM, a5 + k, 0);
        wl(p, a6 + 2, script);
        wl(p, a6 + 24, p->a1);
        wl(p, a6 + 28, banks);
        ww(p, a6 + 6, x);
        ww(p, a6 + 8, y);
        ww(p, a6 + 10, depth);
        ix_wb(VM, a6 + 22, dir);
        ix_wb(VM, a6 + 32, ctl);
        ix_wb(VM, a6 + 0, 1);
        ix_wb(VM, a6 + 1, 1);
        return;
    }
    ww(p, PROGRAM_LAB_0277, 2);
}

/* LAB_0015 / LAB_0016 : personnage de gauche (x 160, sens 1) ou de droite
 * (x 120, sens 3) */
static void spawn_left(ProgIntro *p, uint32_t script)
{
    ent_spawn(p, script, PROGRAM_LAB_0276, 0xA0, 0,
              (uint16_t)(100 + rw(p, PROGRAM_LAB_00EF)), 1, 0);
}

static void spawn_right(ProgIntro *p, uint32_t script)
{
    ent_spawn(p, script, PROGRAM_LAB_0276, 0x78, 0,
              (uint16_t)(100 + rw(p, PROGRAM_LAB_00F0)), 3, 0);
}

/* LAB_01E8 : contrôleurs des entités dont le script est fini
 * (table LAB_011B ; LAB_0014 : fin de la scène, entité libérée) */
static void ent_controllers(ProgIntro *p)
{
    for (unsigned i = 0; i < NENT; i++) {
        uint32_t a6 = ent_addr(i);
        if (!ix_rb(VM, a6) || ix_rb(VM, a6 + 1))
            continue;
        uint32_t fn = rl(p, PROGRAM_LAB_011B + ix_rb(VM, a6 + 32));
        p->a1 = fn;
        if (fn == PROGRAM_LAB_0014) {
            ww(p, PROGRAM_LAB_0120, 1);
            ww(p, a6, 0);
        } else
            fprintf(stderr, "prog : contrôleur inconnu %06X\n", fn);
    }
}

/* LAB_020E : tri à bulles sur la profondeur (mot non signé), échanges par
 * LAB_0283 */
static void ent_sort(ProgIntro *p)
{
    int swapped;
    do {
        swapped = 0;
        for (unsigned k = 0; k < NENT - 1; k++) {
            uint32_t a0 = ent_addr(k);
            if (rw(p, a0 + ENT + 10) >= rw(p, a0 + 10))
                continue;
            for (uint32_t b = 0; b < ENT; b++)
                ix_wb(VM, PROGRAM_LAB_0283 + b, ix_rb(VM, a0 + b));
            for (uint32_t b = 0; b < ENT; b++)
                ix_wb(VM, a0 + b, ix_rb(VM, a0 + ENT + b));
            for (uint32_t b = 0; b < ENT; b++)
                ix_wb(VM, a0 + ENT + b, ix_rb(VM, PROGRAM_LAB_0283 + b));
            swapped = 1;
        }
    } while (swapped);
}

/* LAB_04A8 (Cel_FlipFrame, comme ix_engine.c) : frame retournée en place */
static void flip_frame(ProgIntro *p, uint32_t cel, unsigned frame)
{
    if ((int)frame >= (int16_t)rw(p, cel))
        return;
    uint32_t fe = cel + 10 + frame * 10;
    uint32_t a2 = rl(p, cel + 2) + rl(p, fe);
    uint16_t w = rw(p, fe + 4);
    uint16_t wr = (uint16_t)((w + 15) & 0xFFF0);
    uint16_t pad = (uint16_t)(wr - w);
    uint16_t bpr = (uint16_t)(wr >> 3);
    uint16_t h = rw(p, fe + 6);
    if (ix_rb(VM, fe + 8) & 1)
        ix_wb(VM, fe + 8, (uint8_t)(pad << 4));
    else
        ix_wb(VM, fe + 8, 1);
    uint8_t planes = ix_rb(VM, fe + 9);
    wl(p, PROGRAM_LAB_0504, (uint32_t)(uint16_t)(bpr << 1) * h);
    uint32_t tmp = rl(p, PROGRAM_LAB_051B);
    for (int pl = 0; pl < 5; pl++) {
        if (!(planes & (1u << pl)))
            continue;
        for (unsigned y = 0; y < h; y++, a2 += bpr) {
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, tmp + bpr - 1 - i, ix_rb(VM, PROGRAM_LAB_04B3 + ix_rb(VM, a2 + i)));
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, a2 + i, ix_rb(VM, tmp + i));
        }
    }
}

/* LAB_020B : taille de la frame ; retournée si son sens diffère */
static void frame_info(ProgIntro *p, uint32_t cel, uint16_t frame, uint32_t a1)
{
    uint32_t f = cel + (uint32_t)(int32_t)(int16_t)(uint16_t)(frame * 10);
    ww(p, a1 + 16, rw(p, f + 14));
    ww(p, a1 + 18, rw(p, f + 16));
    uint8_t d3 = ix_rb(VM, f + 18);
    if (d3 != 1)
        d3 = 3;
    if (d3 != ix_rb(VM, a1 + 22))
        flip_frame(p, cel, frame);                      /* LAB_026E */
}

/* LAB_024F : rectangle englobant des frames dessinées */
static void bbox(ProgIntro *p, uint16_t x, uint16_t w, uint16_t y, uint16_t h)
{
    if (!rw(p, PROGRAM_LAB_0273)) {
        ww(p, PROGRAM_LAB_0270, x);
        ww(p, PROGRAM_SECSTRT_11, x);
        ww(p, PROGRAM_LAB_0271, y);
        ww(p, PROGRAM_LAB_0272, y);
        ww(p, PROGRAM_LAB_0273, 1);
    }
    if (!((int16_t)x > (int16_t)rw(p, PROGRAM_LAB_0270)))
        ww(p, PROGRAM_LAB_0270, x);
    uint16_t x2 = (uint16_t)(x + w);
    if (!((int16_t)x2 < (int16_t)rw(p, PROGRAM_SECSTRT_11)))
        ww(p, PROGRAM_SECSTRT_11, x2);
    if (!((int16_t)y > (int16_t)rw(p, PROGRAM_LAB_0271)))
        ww(p, PROGRAM_LAB_0271, y);
    uint16_t y2 = (uint16_t)(y + h);
    if (!((int16_t)y2 < (int16_t)rw(p, PROGRAM_LAB_0272)))
        ww(p, PROGRAM_LAB_0272, y2);
}

/* LAB_026C / LAB_04A6 : plans de dessin LAB_04D9-LAB_04DD */
static void set_planes(ProgIntro *p, uint32_t s)
{
    static const uint32_t pl[5] = { PROGRAM_LAB_04D9, PROGRAM_LAB_04DA, PROGRAM_LAB_04DB,
                                    PROGRAM_LAB_04DC, PROGRAM_LAB_04DD };
    for (uint32_t i = 0; i < 5; i++)
        wl(p, pl[i], s + i * PLANE);
}

/* LAB_022F (routine du script, registres préservés) */
static void flash(ProgIntro *p, uint32_t pal, uint32_t n)
{
    set_palette(p, pal);
    wait_vbls(p, n);
    set_palette(p, rl(p, PROGRAM_LAB_05D2));
}

static void script_call(ProgIntro *p, uint32_t fn)
{
    if (fn == PROGRAM_LAB_003F)                         /* éclair */
        flash(p, PROGRAM_LAB_0042, 8);
    else if (fn == PROGRAM_LAB_0040) {                  /* orage : LAB_0041 × 6 */
        static const uint8_t w[7] = { 20, 2, 20, 5, 10, 5, 50 };
        for (int i = 0; i < 6; i++) {
            wait_vbls(p, w[i]);
            flash(p, PROGRAM_LAB_0043, 2);
        }
        wait_vbls(p, w[6]);
    } else
        fprintf(stderr, "prog : routine de script inconnue %06X\n", fn);
}

/* Commandes du script (octet >= $80, table LAB_0288) */
static void ent_command(ProgIntro *p, uint32_t a1, uint32_t a6, uint8_t op)
{
    uint32_t h = rl(p, PROGRAM_LAB_0288 + op);
    uint32_t a5 = rl(p, a1 + 36);
    uint8_t b1 = ix_rb(VM, a6 + 1);
    if (h == PROGRAM_LAB_0215) {                        /* sens */
        if (b1 == 0xFF)
            ix_wb(VM, a1 + 22, (uint8_t)(ix_rb(VM, a1 + 22) ^ 2));
        else
            ix_wb(VM, a1 + 22, b1);
        wl(p, a1 + 2, a6 + 2);
    } else if (h == PROGRAM_LAB_0218) {                 /* saut / suite */
        if (b1 == 3) {
            wl(p, a1 + 2, rl(p, a6 + 2));
            return;
        }
        wl(p, a5 + 12, rl(p, a6 + 2));
        ix_wb(VM, a5 + 16, 1);
        wl(p, a1 + 2, a6 + 6);
    } else if (h == PROGRAM_LAB_021A) {                 /* répétition du groupe */
        uint8_t n = b1;
        if (!n) {
            n = (uint8_t)(rl(p, PROGRAM_LAB_0379) & 0x1F);
            if (!n)
                n = 1;
        }
        ix_wb(VM, a5, n);
        ix_wb(VM, a5 + 1, 1);
        wl(p, a1 + 2, a6 + 2);
        wl(p, a5 + 2, a6 + 2);
    } else if (h == PROGRAM_LAB_021E) {
        wl(p, a1 + 2, a6 + 8);
    } else if (h == PROGRAM_LAB_021F) {                 /* boucle */
        ix_wb(VM, a5 + 6, b1);
        ix_wb(VM, a5 + 7, 1);
        wl(p, a1 + 2, a6 + 2);
        wl(p, a5 + 8, a6 + 2);
    } else if (h == PROGRAM_LAB_0221) {
        wl(p, a1 + 2, a6 + 4);
    } else if (h == PROGRAM_LAB_0222) {                 /* déplacement */
        if (b1 & 0x40) {
            ww(p, a1 + 6, rw(p, a6 + 2));
            ww(p, a1 + 8, rw(p, a6 + 4));
            ww(p, a1 + 10, rw(p, a6 + 6));
        } else {
            uint16_t d0 = rw(p, a6 + 2);
            int add = ix_rb(VM, a1 + 22) == 3 ? !(b1 & 1) : (b1 & 1);
            ww(p, a1 + 6, (uint16_t)(add ? rw(p, a1 + 6) + d0 : rw(p, a1 + 6) - d0));
            d0 = rw(p, a6 + 4);
            ww(p, a1 + 8, (uint16_t)(b1 & 8 ? rw(p, a1 + 8) - d0 : rw(p, a1 + 8) + d0));
            d0 = rw(p, a6 + 6);
            ww(p, a1 + 10, (uint16_t)(b1 & 0x20 ? rw(p, a1 + 10) - d0 : rw(p, a1 + 10) + d0));
        }
        wl(p, a1 + 2, a6 + 8);
    } else if (h == PROGRAM_LAB_022F) {                 /* appel */
        if (b1)
            fprintf(stderr, "prog : LAB_0280[%u] non repris\n", b1);
        else
            script_call(p, rl(p, a6 + 2));
        wl(p, a1 + 2, rl(p, a1 + 2) + 6);
    } else if (h == PROGRAM_LAB_0232 || h == PROGRAM_LAB_0233 || h == PROGRAM_LAB_0234) {
        wl(p, a1 + 2, a6 + 6);
    } else if (h == PROGRAM_LAB_0235) {                 /* fin de l'entité */
        ww(p, a1, 0);
        wl(p, a1 + 2, a6 + 2);
    } else if (h == PROGRAM_LAB_0236) {                 /* planches */
        wl(p, a1 + 28, rl(p, PROGRAM_LAB_0281 + (uint32_t)(uint16_t)((b1 - 1) << 2)));
        wl(p, a1 + 2, a6 + 2);
    } else if (h == PROGRAM_LAB_0237 || h == PROGRAM_LAB_023B) {    /* test */
        uint32_t a = rl(p, a1 + 24) + (uint32_t)(int32_t)(int16_t)rw(p, a6 + 2);
        uint32_t v = b1 & 1 ? ix_rb(VM, a) : b1 & 2 ? rw(p, a) : rl(p, a);
        if ((h == PROGRAM_LAB_0237) == !v)
            wl(p, a1 + 2, rl(p, a6 + 4));
        else
            wl(p, a1 + 2, a6 + 8);
    } else if (h == PROGRAM_LAB_023F) {
        for (uint32_t k = 0; k < 0x30; k++)
            ix_wb(VM, a5 + k, 0);
        wl(p, a1 + 2, a6 + 2);
    } else {
        fprintf(stderr, "prog : commande de script %02X (%06X) sans effet\n", op | 0x80, h);
        wl(p, a1, 0);                   /* l'original boucle sans fin */
    }
}

/* LAB_0201 : fin d'un groupe de frames ($FF) : répétitions, retours */
static void ent_group_end(ProgIntro *p, uint32_t a1, uint32_t a6, uint16_t d1, uint16_t d2)
{
    uint32_t a5 = rl(p, a1 + 36);
    if (ix_rb(VM, a5 + 1)) {
        uint8_t c = (uint8_t)(ix_rb(VM, a5) - 1);
        ix_wb(VM, a5, c);
        if (c) {
            wl(p, a1 + 2, rl(p, a5 + 2));
            return;
        }
    }
    ix_wb(VM, a5 + 1, 0);
    if (ix_rb(VM, a5 + 26)) {
        uint8_t c = (uint8_t)(ix_rb(VM, a5 + 27) - 1);
        ix_wb(VM, a5 + 27, c);
        if (!(c & 0x80))
            return;                                     /* LAB_022E : RTS */
    } else {
        ix_wb(VM, a5 + 26, 0);
        if (ix_rb(VM, a5 + 42)) {
            fprintf(stderr, "prog : bloc +42 non repris\n");
            ww(p, a1 + 6, d1);
            ww(p, a1 + 10, d2);
            if (!(uint16_t)a5)
                return;
        }
    }
    ix_wb(VM, a5 + 42, 0);
    if (ix_rb(VM, a5 + 16)) {
        ix_wb(VM, a5 + 16, 0);
        wl(p, a1 + 2, rl(p, a5 + 12));
        return;
    }
    uint8_t b1 = ix_rb(VM, a6 + 1);
    if (b1 == 0xFF || b1 == 0xFE) {
        if (ix_rb(VM, a5 + 7)) {
            uint8_t c = (uint8_t)(ix_rb(VM, a5 + 6) - 1);
            ix_wb(VM, a5 + 6, c);
            if (c) {
                wl(p, a1 + 2, rl(p, a5 + 8));
                return;
            }
        }
        ix_wb(VM, a5 + 7, 0);
        if (b1 == 0xFF) {
            ix_wb(VM, a1 + 1, 0);                       /* script fini */
            return;
        }
    }
    wl(p, a1 + 2, rl(p, a1 + 2) + 2);
}

/* LAB_01F1 : script de l'entité a1 pour cette image (frames jusqu'à $FF) */
static void ent_script(ProgIntro *p, uint32_t a1)
{
    while (rl(p, a1 + 2)) {                             /* LAB_01F2 */
        uint16_t d1 = 0, d2 = 0;
        uint32_t a6;
        uint8_t op;
        for (;;) {                                      /* LAB_01F3 */
            a6 = rl(p, a1 + 2);
            op = ix_rb(VM, a6);
            if (op == 0xFF) {
                ent_group_end(p, a1, a6, d1, d2);
                return;
            }
            if (op == 0xFD)
                wl(p, a1 + 2, rl(p, rl(p, a1 + 36) + 32));
            else if (op == 0xFE)
                wl(p, a1 + 2, rl(p, rl(p, a1 + 36) + 8));
            else if (op & 0x80)
                ent_command(p, a1, a6, op & 0x7F);
            else
                break;
        }
        /* frame : planche, n°, dy, drapeaux, dx */
        uint32_t cel = rl(p, rl(p, a1 + 28) + (op & 0x1F));
        uint8_t fr = ix_rb(VM, a6 + 1);
        ix_wb(VM, a1 + 21, fr);
        frame_info(p, cel, fr, a1);
        d2 = (uint16_t)(int16_t)(int8_t)ix_rb(VM, a6 + 2);
        if (ix_rb(VM, a1 + 22) & 2)
            d1 = (uint16_t)(rw(p, a1 + 6) - rw(p, a6 + 4) - rw(p, a1 + 16));
        else
            d1 = (uint16_t)(rw(p, a6 + 4) + rw(p, a1 + 6));
        d2 = (uint16_t)(d2 + rw(p, a1 + 8) + rw(p, a1 + 10));
        ww(p, a1 + 12, d1);
        ww(p, a1 + 14, d2);
        uint8_t fl = ix_rb(VM, a6 + 3);
        if (!(fl & 0x40))
            bbox(p, d1, rw(p, a1 + 16), d2, rw(p, a1 + 18));
        ix_wb(VM, a1 + 20, fl);
        uint16_t d0 = ix_rb(VM, a1 + 21);
        if (fl & 0x10) {                                /* aussi dans le décor */
            set_planes(p, rl(p, PROGRAM_LAB_00C6));
            prog_draw_cel(VM, &p->blt, cel, d0, d1, d2);
            set_planes(p, rl(p, PROGRAM_LAB_056C));
        } else {                                        /* zone à restaurer */
            uint32_t a5 = rl(p, PROGRAM_LAB_027C);
            ww(p, a5, rw(p, a1 + 12));
            ww(p, a5 + 2, rw(p, a1 + 14));
            ww(p, a5 + 4, rw(p, a1 + 16));
            ww(p, a5 + 6, rw(p, a1 + 18));
            ww(p, a5 + 12, 0xFFFF);
            wl(p, PROGRAM_LAB_027C, a5 + 8);
        }
        static const uint32_t lists[2] = { PROGRAM_LAB_027E, PROGRAM_LAB_027F };
        for (int k = 1; k >= 0; k--)
            if (fl & (1u << k)) {
                uint32_t a4 = rl(p, lists[k]);
                wl(p, a4, cel);
                ww(p, a4 + 4, d0);
                ww(p, a4 + 6, d1);
                ww(p, a4 + 8, d2);
                wl(p, a4 + 10, 0);
                wl(p, lists[k], a4 + 10);
            }
        ww(p, PROGRAM_LAB_04DF, fl & 0x20 ? 1 : 0);
        prog_draw_cel(VM, &p->blt, cel, d0, d1, d2);
        wl(p, a1 + 2, rl(p, a1 + 2) + 6);
    }
}

/* LAB_01EC : entités triées, scripts joués (+18 du bloc : double) */
static void ent_render(ProgIntro *p)
{
    ent_sort(p);
    uint32_t a1 = PROGRAM_LAB_0282;
    for (unsigned i = 0; i < NENT; i++, a1 += ENT) {
        ww(p, PROGRAM_LAB_0278, (uint16_t)i);
        wl(p, PROGRAM_LAB_027B, a1);
        if (!ix_rb(VM, a1) || rw(p, a1 + 40))
            continue;
        uint32_t a5 = rl(p, a1 + 36);
        ww(p, PROGRAM_LAB_0273, 0);
        if (!ix_rb(VM, a5 + 18)) {
            ent_script(p, a1);
            continue;
        }
        for (uint32_t b = 0; b < ENT; b++)
            ix_wb(VM, PROGRAM_LAB_0283 + b, ix_rb(VM, a1 + b));
        ww(p, PROGRAM_LAB_0283 + 8, 0);
        wl(p, PROGRAM_LAB_0283 + 2, rl(p, a5 + 20));
        wl(p, PROGRAM_LAB_0283 + 36, PROGRAM_LAB_0285);
        ent_script(p, PROGRAM_LAB_0283);
        ent_script(p, a1);
    }
    ww(p, PROGRAM_LAB_0278, NENT);
    p->a1 = a1;
}

/* LAB_0246 : zone (x, y, l, h) recopiée du décor LAB_00C6 dans l'écran */
static void restore_area(ProgIntro *p, uint32_t a6)
{
    int16_t d0 = (int16_t)rw(p, a6), d1 = (int16_t)rw(p, a6 + 2);
    if (d1 >= 200)
        return;
    int16_t d2 = (int16_t)(rw(p, a6 + 4) + d0);
    d0 = (int16_t)((d0 >> 3) & ~1);
    if (d0 >= 40)
        return;
    d2 = (int16_t)((d2 >> 3) & ~1);
    if (d2 < 0)
        return;
    d2 = (int16_t)((d2 - d0 + 2) >> 1);
    int16_t d3 = (int16_t)rw(p, a6 + 6);
    if ((int16_t)(d3 + d1) <= 0)
        return;
    if (d0 < 0) {
        d0 = (int16_t)(d0 >> 1);
        d2 = (int16_t)(d2 + d0);
        if (d2 <= 0)
            return;
        d0 = 0;
    }
    int16_t d4 = (int16_t)(d0 + d2 + d2 - 40);
    if (d4 > 0) {
        d2 = (int16_t)(d2 - (d4 >> 1));
        if (d2 <= 0)
            return;
    }
    if (d1 < 0) {
        d3 = (int16_t)(d3 + d1);
        if (d3 <= 0)
            return;
        d1 = 0;
    }
    d4 = (int16_t)(d1 + d3 - 200);
    if (d4 > 0) {
        d3 = (int16_t)(d3 - d4);
        if (d3 <= 0)
            return;
    }
    int16_t off = (int16_t)(d0 + (uint16_t)(d1 * 40));
    uint32_t a0 = rl(p, PROGRAM_LAB_00C6) + (uint32_t)(int32_t)off;
    uint32_t a1 = rl(p, PROGRAM_LAB_056C) + (uint32_t)(int32_t)off;
    uint16_t mod = (uint16_t)(40 - d2 - d2);
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy(p, a0, a1, mod, mod, (uint16_t)d2, (uint16_t)d3);
    p->a1 = a1 - PLANE;
}

/* LAB_0242 : zones de la liste LAB_0279 (au plus 130) restaurées */
static void restore_areas(ProgIntro *p)
{
    wl(p, PROGRAM_LAB_011C, 0);
    wl(p, PROGRAM_LAB_027D, rl(p, PROGRAM_LAB_056C));
    for (uint32_t a6 = rl(p, PROGRAM_LAB_0279);; a6 += 8) {
        if (rw(p, a6 + 4) == 0xFFFF || rl(p, PROGRAM_LAB_011C) == 130
            || !rw(p, a6 + 6) || !rw(p, a6 + 4))
            return;
        restore_area(p, a6);
        wl(p, PROGRAM_LAB_011C, rl(p, PROGRAM_LAB_011C) + 1);
    }
}

/* LAB_003C : cercle de pierres par-dessus (LAB_0121 ; LAB_003E = 2 :
 * deux pierres seulement) */
static void overlay(ProgIntro *p)
{
    uint32_t cel = rl(p, PROGRAM_LAB_0121);
    ww(p, PROGRAM_LAB_04DF, 1);
    prog_draw_cel(VM, &p->blt, cel, 0, 0x0000, 0x38);
    prog_draw_cel(VM, &p->blt, cel, 3, 0x0033, 0x86);
    if (rw(p, PROGRAM_LAB_003E) != 2) {
        prog_draw_cel(VM, &p->blt, cel, 1, 0x009D, 0x68);
        prog_draw_cel(VM, &p->blt, cel, 2, 0x0129, 0x52);
    }
    ww(p, PROGRAM_LAB_04DF, 0);
}

/* LAB_000F : palette de la scène, une fois (LAB_011E : 2 fondu, 3 vers
 * le noir, 5 noir, autre : LAB_011D d'un coup) */
static void scene_palette(ProgIntro *p)
{
    ww(p, PROGRAM_LAB_011F, 1);
    uint32_t pal = rl(p, PROGRAM_LAB_011D);
    switch (rw(p, PROGRAM_LAB_011E)) {
    case 2: fade_to(p, pal, 2); break;
    case 5: black(p); break;
    case 3: fade_to(p, PROGRAM_LAB_026D, 2); break;
    default:
        set_palette(p, pal);
        copy_palette(p, pal);
        break;
    }
}

/* Une image : contrôleurs (LAB_0007 seulement), scripts, écran */
static int scene_frame(ProgIntro *p, int stop_on_end)
{
    frame_mark(p);
    ent_controllers(p);
    if (stop_on_end && rw(p, PROGRAM_LAB_0120))
        return 1;
    ent_render(p);
    if (rw(p, PROGRAM_LAB_00D1))
        overlay(p);
    swap_screens(p);
    restore_areas(p);
    if (!rw(p, PROGRAM_LAB_011F))
        scene_palette(p);
    frame_wait(p);
    return 0;
}

/* LAB_0007 : jusqu'à la fin d'un script (LAB_0014 : LAB_0120) */
static void run_until_end(ProgIntro *p)
{
    while (!scene_frame(p, 1))
        ;
}

/* LAB_000B : images jusqu'à LAB_0028 = 16 */
static void run_frames(ProgIntro *p)
{
    do
        scene_frame(p, 0);
    while ((ww(p, PROGRAM_LAB_0028, (uint16_t)(rw(p, PROGRAM_LAB_0028) + 1)),
            rw(p, PROGRAM_LAB_0028) != 16));
}

/* LAB_001D : un personnage de la liste L00_0060A toutes les
 * 16 - L00_0060E images */
static void procession(ProgIntro *p)
{
    do {
        ww(p, PROGRAM_LAB_0028, rw(p, PROGRAM_L00_0060E));
        p->a1 = rl(p, PROGRAM_L00_0060A);
        spawn_left(p, rl(p, p->a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(rw(p, PROGRAM_L00_00602) << 2)));
        run_frames(p);
        ww(p, PROGRAM_L00_00602, (uint16_t)(rw(p, PROGRAM_L00_00602) + 1));
    } while (rw(p, PROGRAM_LAB_0029) != rw(p, PROGRAM_L00_00602));
}

/* LAB_001F : même chose, à gauche et à droite en alternance (LAB_0026) */
static void procession2(ProgIntro *p)
{
    do {
        ww(p, PROGRAM_LAB_0028, 0);
        p->a1 = rl(p, PROGRAM_L00_0060A);
        uint32_t a0 = rl(p, p->a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(rw(p, PROGRAM_L00_00602) << 2));
        uint16_t s = (uint16_t)(rw(p, PROGRAM_LAB_0026) ^ 1);
        ww(p, PROGRAM_LAB_0026, s);
        if (s)
            spawn_left(p, a0);
        else
            spawn_right(p, a0);
        run_frames(p);
        ww(p, PROGRAM_L00_00602, (uint16_t)(rw(p, PROGRAM_L00_00602) + 1));
    } while (rw(p, PROGRAM_LAB_0029) != rw(p, PROGRAM_L00_00602));
}

/* ------------------------------------------------------------- scènes */

void prog_scene_05a5(ProgIntro *p)
{
    fill_lists(p);
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

/* LAB_001B : procession des druides dans la forêt */
void prog_scene_001b(ProgIntro *p)
{
    ent_reset(p);
    ww(p, PROGRAM_L00_0060E, 4);
    ww(p, PROGRAM_LAB_011F, 1);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_L00_00602, 0);
    wl(p, PROGRAM_L00_0060A, PROGRAM_LAB_0024);
    ww(p, PROGRAM_LAB_0029, 5);
    wl(p, PROGRAM_LAB_00D0, 8);
    procession(p);
}

/* LAB_001C : arrivée à Stonehenge (pierres par-dessus, LAB_003C) */
void prog_scene_001c(ProgIntro *p)
{
    black(p);
    ent_reset(p);
    copy_long(p, rl(p, PROGRAM_LAB_00CE), rl(p, PROGRAM_LAB_00C6));
    show_background(p);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01D1);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_L00_00602, 0);
    wl(p, PROGRAM_L00_0060A, PROGRAM_LAB_0025);
    ww(p, PROGRAM_LAB_0029, 5);
    ww(p, PROGRAM_L00_0060E, 8);
    ww(p, PROGRAM_LAB_00D1, 1);
    procession(p);
    ww(p, PROGRAM_LAB_00D1, 0);
    wl(p, PROGRAM_LAB_00D0, 6);
}

/* LAB_0174 : décors et palettes des scènes suivantes */
void prog_scene_0174(ProgIntro *p)
{
    copy_long(p, rl(p, PROGRAM_LAB_00CB), rl(p, PROGRAM_LAB_00C8));
    copy_colours(p, PROGRAM_LAB_01CE, PROGRAM_LAB_01CB);
    copy_long(p, rl(p, PROGRAM_LAB_00CC), rl(p, PROGRAM_LAB_00C9));
    copy_colours(p, PROGRAM_LAB_01CF, PROGRAM_LAB_01CC);
    copy_long(p, rl(p, PROGRAM_LAB_00CD), rl(p, PROGRAM_LAB_00CA));
    copy_colours(p, PROGRAM_LAB_01D0, PROGRAM_LAB_01CD);
}

/* LAB_0030 / LAB_0031 : druides en cercle, de chaque côté */
static void circle(ProgIntro *p, const uint32_t *s, int n)
{
    for (int i = 0; i < n; i++)
        spawn_left(p, s[i]);
    for (int i = 0; i < n; i++)
        spawn_right(p, s[i]);
}

/* LAB_001A : le cercle, vu de haut */
void prog_scene_001a(ProgIntro *p)
{
    static const uint32_t c[3] = { PROGRAM_LAB_00DC, PROGRAM_LAB_00DD, PROGRAM_LAB_00DA };
    ent_reset(p);
    black(p);
    swap_screens(p);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00CA));
    show_background(p);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01CD);
    ww(p, PROGRAM_LAB_0120, 0);
    spawn_left(p, c[0]);
    spawn_left(p, c[1]);
    spawn_left(p, c[2]);
    spawn_right(p, c[0]);
    spawn_right(p, c[1]);
    spawn_right(p, PROGRAM_LAB_00D9);
    ww(p, PROGRAM_L00_00602, 0);
    wl(p, PROGRAM_L00_0060A, PROGRAM_LAB_0023);
    ww(p, PROGRAM_LAB_0029, 4);
    ww(p, PROGRAM_L00_0060E, 0);
    procession2(p);
    spawn_left(p, PROGRAM_LAB_00E3);
    run_until_end(p);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00C7));
}

/* LAB_002C : le grand druide (deux plans) */
void prog_scene_002c(ProgIntro *p)
{
    black(p);
    swap_screens(p);
    ww(p, PROGRAM_LAB_011F, 0);
    ent_reset(p);
    spawn_left(p, PROGRAM_LAB_00D2);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00C8));
    show_background(p);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01CB);
    ww(p, PROGRAM_LAB_011E, 4);
    ww(p, PROGRAM_LAB_0120, 0);
    run_until_end(p);
    black(p);
    ent_reset(p);
    ww(p, PROGRAM_LAB_011F, 0);
    spawn_left(p, PROGRAM_LAB_00D4);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00C9));
    show_background(p);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01CC);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    run_until_end(p);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00C7));
}

/* LAB_002D : le chevalier à Stonehenge */
void prog_scene_002d(ProgIntro *p)
{
    ww(p, PROGRAM_LAB_00D1, 1);
    ww(p, PROGRAM_LAB_003E, 2);
    ent_reset(p);
    black(p);
    copy_long(p, rl(p, PROGRAM_LAB_00CF), rl(p, PROGRAM_LAB_00C6));
    show_background(p);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01D2);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    spawn_left(p, PROGRAM_LAB_00D6);
    run_until_end(p);
    ww(p, PROGRAM_LAB_00D1, 0);
    ww(p, PROGRAM_LAB_003E, 0);
}

/* LAB_002F : le cercle, vu de haut (seconde fois) */
void prog_scene_002f(ProgIntro *p)
{
    static const uint32_t c[5] = { PROGRAM_LAB_00DC, PROGRAM_LAB_00DD, PROGRAM_LAB_00DB,
                                   PROGRAM_LAB_00DF, PROGRAM_LAB_00E1 };
    ent_reset(p);
    black(p);
    swap_screens(p);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00CA));
    show_background(p);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01CD);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    circle(p, c, 5);
    spawn_left(p, PROGRAM_LAB_00E4);
    spawn_left(p, PROGRAM_LAB_00E5);
    wl(p, PROGRAM_LAB_00D0, 8);
    run_until_end(p);
    wl(p, PROGRAM_LAB_00D0, 6);
    wl(p, PROGRAM_LAB_00C6, rl(p, PROGRAM_LAB_00C7));
}

/* LAB_002E : l'adoubement */
void prog_scene_002e(ProgIntro *p)
{
    ent_reset(p);
    black(p);
    copy_long(p, rl(p, PROGRAM_LAB_00CC), rl(p, PROGRAM_LAB_00C6));
    show_background(p);
    wl(p, PROGRAM_LAB_011D, PROGRAM_LAB_01CF);
    ww(p, PROGRAM_LAB_0120, 0);
    ww(p, PROGRAM_LAB_011F, 0);
    ww(p, PROGRAM_LAB_011E, 4);
    spawn_left(p, PROGRAM_LAB_00D3);
    run_until_end(p);
}

/* ------------------------------------------------------------- texte */

/* Lettre c de la police rl(LAB_011A + 16) : frame (table LAB_00F8),
 * largeur LAB_00F1 et hauteur LAB_00F2 */
static uint16_t glyph(ProgIntro *p, uint8_t c)
{
    uint16_t f = ix_rb(VM, PROGRAM_LAB_00F8 + (uint8_t)(c - 0x20));
    uint32_t e = rl(p, PROGRAM_LAB_011A + 16) + (uint32_t)(int32_t)(int16_t)(uint16_t)(f * 10);
    ww(p, PROGRAM_LAB_00F1, rw(p, e + 14));
    ww(p, PROGRAM_LAB_00F2, rw(p, e + 16));
    return f;
}

/* LAB_0297 : largeur de la chaîne LAB_00F7 (lettres rapprochées de 2) */
static uint16_t text_width(ProgIntro *p)
{
    ww(p, PROGRAM_LAB_029A, 0);
    for (uint32_t a = rl(p, PROGRAM_LAB_00F7); ix_rb(VM, a); a++) {
        glyph(p, ix_rb(VM, a));
        ww(p, PROGRAM_LAB_029A, (uint16_t)(rw(p, PROGRAM_LAB_029A) + rw(p, PROGRAM_LAB_00F1) - 2));
    }
    return rw(p, PROGRAM_LAB_029A);
}

/* LAB_028F : enregistrements de texte (long chaîne, mot x, mot y, octet,
 * octet drapeaux : bit 0 centré, bit 1 zone à restaurer ; long suivant),
 * masque LAB_04DF levé */
static void text_records(ProgIntro *p, uint32_t a0)
{
    ww(p, PROGRAM_LAB_04DF, 1);
    if (a0) {
        wl(p, PROGRAM_LAB_0296, a0);
        do {
            uint32_t a2 = rl(p, PROGRAM_LAB_0296);
            wl(p, PROGRAM_LAB_00F7, rl(p, a2));
            ww(p, PROGRAM_LAB_00F3, rw(p, a2 + 4));
            ww(p, PROGRAM_LAB_00F5, rw(p, a2 + 4));
            ww(p, PROGRAM_LAB_00F4, rw(p, a2 + 6));
            ww(p, PROGRAM_LAB_00F6, rw(p, a2 + 6));
            if (ix_rb(VM, a2 + 9) & 1) {
                uint16_t x = (uint16_t)((uint16_t)(320 - text_width(p)) >> 1);
                ww(p, PROGRAM_LAB_00F3, x);
                ww(p, PROGRAM_LAB_00F5, x);
            }
            for (;;) {                                  /* LAB_0291 */
                uint32_t a = rl(p, PROGRAM_LAB_00F7);
                uint8_t c = ix_rb(VM, a);
                if (!c)
                    break;
                wl(p, PROGRAM_LAB_00F7, a + 1);
                uint16_t f = glyph(p, c);
                uint16_t x = rw(p, PROGRAM_LAB_00F3), y = rw(p, PROGRAM_LAB_00F4);
                if (ix_rb(VM, rl(p, PROGRAM_LAB_0296) + 9) & 2) {  /* LAB_02A0 */
                    uint32_t a5 = rl(p, PROGRAM_LAB_027C);
                    ww(p, a5, x);
                    ww(p, a5 + 2, y);
                    ww(p, a5 + 4, rw(p, PROGRAM_LAB_00F1));
                    ww(p, a5 + 6, rw(p, PROGRAM_LAB_00F2));
                    ww(p, a5 + 12, 0xFFFF);
                    wl(p, PROGRAM_LAB_027C, a5 + 8);
                }
                prog_draw_cel(VM, &p->blt, rl(p, PROGRAM_LAB_011A + 16), f, x, y);
                x = (uint16_t)(rw(p, PROGRAM_LAB_00F3) + rw(p, PROGRAM_LAB_00F1) - 2);
                ww(p, PROGRAM_LAB_00F3, x);
                if ((int16_t)x >= 320) {
                    ww(p, PROGRAM_LAB_00F3, rw(p, PROGRAM_LAB_00F5));
                    y = (uint16_t)(rw(p, PROGRAM_LAB_00F4) + rw(p, PROGRAM_LAB_00F2));
                    ww(p, PROGRAM_LAB_00F4, y);
                    if ((int16_t)y >= 200)
                        ww(p, PROGRAM_LAB_00F4, rw(p, PROGRAM_LAB_00F6));
                }
            }
            wl(p, PROGRAM_LAB_0296, rl(p, rl(p, PROGRAM_LAB_0296) + 10));
        } while (rl(p, PROGRAM_LAB_0296));
    }
    ww(p, PROGRAM_LAB_04DF, 0);
}

/* LAB_03FC : PIV en a0 décodé (en place) vers les plans LAB_04D9 ;
 * palette -> LAB_0506 (comme LAB_0C21 de mog) */
static void piv_decode(ProgIntro *p, uint32_t a0)
{
    wl(p, PROGRAM_SECSTRT_20, a0);
    uint32_t n = 32;
    ww(p, PROGRAM_LAB_0434, rw(p, a0));
    if (rw(p, a0) != 4)
        n = 64;
    uint32_t src = a0 + 6;
    for (uint32_t i = 0; i < n; i++)
        ix_wb(VM, PROGRAM_LAB_0506 + i, ix_rb(VM, src + i));
    src += n;
    for (uint32_t i = 0; i < n / 2; i++) {
        uint16_t c = rw(p, PROGRAM_LAB_0506 + 2 * i);
        c = c & 0x8000 ? (uint16_t)(c & 0x7FFF) : (uint16_t)(c << 1);
        ww(p, PROGRAM_LAB_0506 + 2 * i, c);
    }
    uint32_t len = rl(p, a0 + 2);
    uint32_t k = (uint32_t)(uint16_t)(len - 1) + 1;     /* DBF sur le mot */
    for (uint32_t i = 0; i < k; i++)
        ix_wb(VM, a0 + 2 + i, ix_rb(VM, src + i));
    ww(p, PROGRAM_LAB_0528, 1);                         /* LAB_0406 */
    prog_unpack(VM, a0 + 2, len, rl(p, PROGRAM_LAB_04D9));
}

/* LAB_054D : écran à zéro */
static void clear_screen(ProgIntro *p, uint32_t a0)
{
    for (uint32_t i = 0; i < 5 * PLANE; i += 4)
        wl(p, a0 + i, 0);
}

/* LAB_0260 : fondu vers a0 (vitesse LAB_0261), attente 16 × LAB_0261 */
static void fade_wait(ProgIntro *p, uint32_t a0)
{
    fade_to(p, a0, (uint16_t)rl(p, PROGRAM_LAB_0261));
    wait_vbls(p, rl(p, PROGRAM_LAB_0261) << 4);
}

/* LAB_025F : fondu au noir (LAB_026D), 36 VBL */
void prog_fade_black(ProgIntro *p)
{
    fade_to(p, PROGRAM_LAB_026D, 2);
    wait_vbls(p, 36);
}

/* LAB_0054 : écran de texte a0 sur le fond PIV rl(LAB_011A + 20) */
void prog_text_screen(ProgIntro *p, uint32_t a0)
{
    wl(p, PROGRAM_SECSTRT_2, a0);
    black(p);
    clear_screen(p, rl(p, PROGRAM_SECSTRT_30));
    set_planes(p, rl(p, PROGRAM_SECSTRT_30));
    uint32_t src = rl(p, PROGRAM_LAB_011A + 20), dst = rl(p, PROGRAM_LAB_056C);
    for (uint32_t i = 0; i < 0x10E7; i++)
        ix_wb(VM, dst + i, ix_rb(VM, src + i));
    piv_decode(p, rl(p, PROGRAM_LAB_056C));
    text_records(p, rl(p, PROGRAM_SECSTRT_2));
    static const uint16_t pal[5] = { 0x0800, 0x0600, 0x0400, 0x0000, 0x0200 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_LAB_0506 + 2 + 2u * (unsigned)i, pal[i]);
    fade_wait(p, PROGRAM_LAB_0506);
}

/* ---------------------------------------------------------- démarrage */

/* LAB_0402 : image PIV `name` lue à a1 puis décodée vers les plans
 * LAB_04D9 ; palette -> LAB_0506 */
static void load_picture(ProgIntro *p, uint32_t name, uint32_t a1)
{
    MogFile f;
    wl(p, PROGRAM_SECSTRT_20, a1);
    prog_file_open(VM, name, &f);
    prog_file_read(VM, &f, a1, 6);
    uint32_t n = 32;
    ww(p, PROGRAM_LAB_0434, rw(p, a1));
    if (rw(p, a1) != 4)
        n = 64;
    prog_file_read(VM, &f, PROGRAM_LAB_0506, n);
    for (uint32_t i = 0; i < n / 2; i++) {
        uint16_t c = rw(p, PROGRAM_LAB_0506 + 2 * i);
        c = c & 0x8000 ? (uint16_t)(c & 0x7FFF) : (uint16_t)(c << 1);
        ww(p, PROGRAM_LAB_0506 + 2 * i, c);
    }
    uint32_t len = rl(p, a1 + 2);
    prog_file_read(VM, &f, a1 + 2, len);
    prog_file_close(&f);
    ww(p, PROGRAM_LAB_0528, 1);                         /* LAB_0406 */
    prog_unpack(VM, a1 + 2, len, rl(p, PROGRAM_LAB_04D9));
}

/* LAB_025B : palette LAB_0506 copiée en a0 */
static void save_palette(ProgIntro *p, uint32_t a0)
{
    for (uint32_t i = 0; i < 32; i++)
        ww(p, a0 + 2 * i, rw(p, PROGRAM_LAB_0506 + 2 * i));
}

/* LAB_01BE : quatre couleurs (a0 + 16) selon les drapeaux LAB_0005
 * (écrits par mog en fin de partie, $3E0) */
static void tint(ProgIntro *p, uint32_t a0)
{
    static const uint16_t t[4][4] = {
        { 0x005D, 0x0028, 0x0016, 0x0003 }, { 0x0FA0, 0x0B40, 0x0930, 0x0710 },
        { 0x0E00, 0x0900, 0x0600, 0x0300 }, { 0x00C5, 0x0082, 0x0061, 0x0040 },
    };
    static const uint8_t bit[4] = { 4, 5, 3, 6 };
    uint16_t d0 = rw(p, PROGRAM_LAB_0005);
    for (int k = 0; k < 4; k++)
        if (d0 & (1u << bit[k])) {
            for (int i = 0; i < 4; i++)
                ww(p, a0 + 16 + 2u * (unsigned)i, t[k][i]);
            return;
        }
}

/* Unpack_Rnc1 : décompression RNC (méthode 2, à rebours) en place ; la
 * fin du tampon source est remise à zéro. Tables lues dans program. */
typedef struct { ProgIntro *p; uint32_t a6, a3; uint8_t d3; } Rnc;

static int rnc_bit(Rnc *r)                               /* LAB_01A1 */
{
    int c = r->d3 >> 7;
    r->d3 = (uint8_t)(r->d3 << 1);
    if (r->d3)
        return c;
    uint8_t b = ix_rb(r->p->vm, --r->a6);
    int c2 = b >> 7;
    r->d3 = (uint8_t)((b << 1) | c);
    return c2;
}

static uint16_t rnc_bits(Rnc *r, uint16_t v, int n)
{
    while (n--)
        v = (uint16_t)((v << 1) | rnc_bit(r));
    return v;
}

static uint32_t be32(ProgIntro *p, uint32_t a) { return rl(p, a); }

static uint32_t rnc_unpack(ProgIntro *p, uint32_t a0)
{
    IxVM *vm = VM;
    uint32_t a1 = a0, d0 = 0;
    Rnc r = { p, 0, 0, 0 };
    if (be32(p, a0) == 0x524E4301) {
        uint32_t a4 = a0 + 12;
        uint32_t a2 = a4 + be32(p, a0 + 4) + 0x100;
        r.a3 = a2;
        r.a6 = a4 + be32(p, a0 + 8);
        r.d3 = ix_rb(vm, --r.a6);
        for (;;) {
            /* LAB_0198 : octets littéraux */
            if (rnc_bit(&r)) {
                uint16_t d5 = 0;
                if (rnc_bit(&r)) {
                    int16_t d1 = 3;
                    for (;;) {
                        int8_t nb = (int8_t)ix_rb(vm, PROGRAM_LAB_019F + (uint32_t)d1);
                        uint16_t d2 = (uint16_t)~(uint16_t)(0xFFFF << nb);
                        d5 = rnc_bits(&r, 0, nb);
                        if (!d1 || d5 != d2)
                            break;
                        d1--;
                    }
                    d5 = (uint16_t)(d5 + (int8_t)ix_rb(vm, PROGRAM_LAB_01A0 + (uint32_t)d1));
                }
                for (uint32_t k = 0; k <= d5; k++)       /* LAB_019D */
                    ix_wb(vm, --r.a3, ix_rb(vm, --r.a6));
            }
            if ((int32_t)(r.a6 - a4) <= 0)
                break;
            /* LAB_01A3 : longueur */
            int16_t c = 3;
            while (c >= 0 && rnc_bit(&r))
                c--;
            uint16_t i0 = (uint16_t)(c + 1);
            uint16_t d6 = 0;
            int8_t nb = (int8_t)ix_rb(vm, PROGRAM_LAB_01A8 + i0);
            if (nb)
                d6 = rnc_bits(&r, 0, nb);
            d6 = (uint16_t)(d6 + (int8_t)ix_rb(vm, PROGRAM_L08_008B3 + i0));
            /* LAB_01AA : distance */
            uint16_t d7 = 0;
            if (d6 == 2) {
                int n = 6;
                uint16_t add = 0;
                if (rnc_bit(&r)) {
                    n = 9;
                    add = 64;
                }
                d7 = (uint16_t)(rnc_bits(&r, 0, n) + add);
            } else {
                int16_t e = 1;
                while (e >= 0 && rnc_bit(&r))
                    e--;
                uint16_t j = (uint16_t)(e + 1);
                int8_t m = (int8_t)ix_rb(vm, PROGRAM_LAB_01B0 + j);
                d7 = rnc_bits(&r, 0, m + 1);
                d7 = (uint16_t)(d7 + rw(p, PROGRAM_LAB_01B1 + 2u * j));
            }
            d6 = (uint16_t)(d6 - 1);
            uint32_t src = r.a3 + (uint32_t)(int32_t)(int16_t)d7 + (uint32_t)(int32_t)(int16_t)d6;
            if (!d7)
                src = r.a3 + 1;
            for (uint32_t k = 0; k <= d6; k++)           /* LAB_0192 */
                ix_wb(vm, --r.a3, ix_rb(vm, --src));
        }
        d0 = a2 - r.a3;
        a0 = r.a3;
    }
    /* LAB_01B2 */
    uint32_t d2 = a0 - a1;
    if (d0) {
        for (uint32_t k = 0; k < d0; k++)
            ix_wb(vm, a1++, ix_rb(vm, a0++));
        do
            ix_wb(vm, a1++, 0);
        while (--d2);
    }
    return d0;
}

/* LAB_0325 (partie mémoire) : état de la souris, vecteurs d'interruption */
static void interrupts_init(ProgIntro *p)
{
    prog_wait_vbl(p);
    ix_wb(VM, PROGRAM_LAB_0368, 0);                     /* JOY0DAT */
    ix_wb(VM, PROGRAM_LAB_0369, 0);
    static const uint32_t v[6] = { PROGRAM_LAB_0326, PROGRAM_LAB_032A, PROGRAM_LAB_0331,
                                   PROGRAM_LAB_0337, PROGRAM_LAB_033C, PROGRAM_LAB_033F };
    for (uint32_t i = 0; i < 6; i++)
        wl(p, 0x64 + 4 * i, v[i]);
}

/* SECSTRT_29 : mémoire chip à zéro, copper list (sprites vides), écrans */
static void display_init(ProgIntro *p)
{
    for (uint32_t a = 0x6BEFA; a != 0x80000; a += 2)    /* EXT_000f */
        ww(p, a, 0);
    uint32_t a0 = 0x7F6AE, a1 = rl(p, PROGRAM_LAB_0570);
    uint32_t d1 = PROGRAM_LAB_056F, d2 = d1 >> 16;
    for (;;) {
        uint32_t d0 = rl(p, a1);
        a1 += 4;
        if ((int32_t)d0 >= 0x01200000 && (int32_t)d0 < 0x01400000) {
            d0 = (d0 & 0xFFFF0000u) | d2;
            if (d0 & (1u << 17))
                d0 = (d0 & 0xFFFF0000u) | (d1 & 0xFFFF);
        }
        wl(p, a0, d0);
        a0 += 4;
        if (d0 == 0xFFFFFFFEu)
            break;
    }
    interrupts_init(p);
    uint32_t s = rl(p, PROGRAM_SECSTRT_30);
    for (uint32_t i = 0; i < 5; i++) {
        ww(p, COPPER_BPL + 8 * i, (uint16_t)((s + i * PLANE) >> 16));
        ww(p, COPPER_BPL + 8 * i + 4, (uint16_t)(s + i * PLANE));
    }
    clear_screen(p, rl(p, PROGRAM_LAB_056C));
    clear_screen(p, rl(p, PROGRAM_SECSTRT_30));
    prog_wait_vbl(p);
    set_palette(p, PROGRAM_LAB_056E);
}

/* LAB_0044 : découpage des blocs chip (LAB_00C2) et fast (LAB_00C4) */
static void buffers_init(ProgIntro *p)
{
    uint32_t d0 = rl(p, PROGRAM_LAB_00C2);
    wl(p, PROGRAM_LAB_00C2, d0 + 0x4536C);
    wl(p, PROGRAM_SECSTRT_3, d0);
    wl(p, PROGRAM_LAB_00C6, d0);
    wl(p, PROGRAM_LAB_00C7, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_SECSTRT_3 + 4, d0);
    wl(p, PROGRAM_LAB_00C8, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_SECSTRT_3 + 8, d0);
    wl(p, PROGRAM_LAB_00C9, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_SECSTRT_3 + 8, d0);
    wl(p, PROGRAM_LAB_00CA, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_LAB_0124, d0);
    d0 = rl(p, PROGRAM_LAB_00C4);
    wl(p, PROGRAM_LAB_00C4, d0 + 0x58116);
    wl(p, PROGRAM_LAB_0045, d0);
    d0 += 0x7530;
    wl(p, PROGRAM_LAB_011A + 0, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_LAB_011A + 4, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_LAB_011A + 20, d0);
    d0 += 0x10E6;
    wl(p, PROGRAM_LAB_011A + 16, d0);
    d0 += 0x5F50;
    static const uint32_t bg[5] = { PROGRAM_LAB_00CB, PROGRAM_LAB_00CC, PROGRAM_LAB_00CD,
                                    PROGRAM_LAB_00CE, PROGRAM_LAB_00CF };
    for (int i = 0; i < 5; i++, d0 += 0x9C40)
        wl(p, bg[i], d0);
}

/* SECSTRT_10 : planches LAB_0281, commandes de script LAB_0288, listes */
static void engine_init(ProgIntro *p)
{
    for (uint32_t i = 0; i < 7; i++)
        wl(p, PROGRAM_LAB_0281 + 4 * i, PROGRAM_LAB_0276);
    static const struct { uint32_t off, fn; } op[] = {
        { 0, PROGRAM_LAB_0215 }, { 4, PROGRAM_LAB_0218 }, { 8, PROGRAM_LAB_021A },
        { 12, PROGRAM_LAB_021E }, { 20, PROGRAM_LAB_021F }, { 24, PROGRAM_LAB_0220 },
        { 28, PROGRAM_LAB_0220 }, { 36, PROGRAM_LAB_0221 }, { 32, PROGRAM_LAB_0222 },
        { 48, PROGRAM_LAB_022C }, { 44, PROGRAM_LAB_022D }, { 52, PROGRAM_LAB_022F },
        { 72, PROGRAM_LAB_0232 }, { 56, PROGRAM_LAB_0233 }, { 60, PROGRAM_LAB_0234 },
        { 64, PROGRAM_LAB_0235 }, { 68, PROGRAM_LAB_0236 }, { 76, PROGRAM_LAB_0237 },
        { 80, PROGRAM_LAB_023B }, { 84, PROGRAM_LAB_023F }, { 40, PROGRAM_LAB_0241 },
    };
    for (unsigned i = 0; i < sizeof op / sizeof op[0]; i++)
        wl(p, PROGRAM_LAB_0288 + op[i].off, op[i].fn);
    wl(p, PROGRAM_LAB_027A, PROGRAM_LAB_0286);
    wl(p, PROGRAM_LAB_0279, PROGRAM_LAB_0287);
}

/* SECSTRT_25 (suite) : tables de conversion de pixels LAB_04E4 (LAB_0507),
 * LAB_04EC (LAB_051D), LAB_04EE (LAB_051E) et masques LAB_046F */
static void gfx_tables(ProgIntro *p)
{
    IxVM *vm = VM;
    uint32_t a1 = rl(p, PROGRAM_LAB_0519);
    for (uint32_t k = 0; k < 8; k++)
        wl(p, PROGRAM_LAB_0508 + 4 * k, a1 + k * 0x200);
    uint32_t a0 = PROGRAM_LAB_0507;
    do {                                                /* LAB_04E4 */
        uint32_t a2 = a0;
        for (unsigned d0 = 0; d0 < 256; d0++) {
            uint32_t q = a0;
            uint8_t d7 = ix_rb(vm, q++), d6 = ix_rb(vm, q++), d2 = 0;
            for (unsigned i = 0; i <= d7; i++) {
                uint8_t d1 = ix_rb(vm, q++);
                d2 = (uint8_t)(d2 >> 1);
                if ((d0 >> (d1 & 31)) & 1)
                    d2 |= 0x80;
            }
            ix_wb(vm, a1, d2);
            d2 = 0;
            for (unsigned i = 0; i <= d6; i++) {
                uint8_t d1 = ix_rb(vm, q++);
                d2 = (uint8_t)(d2 << 1);
                if ((d0 >> (d1 & 31)) & 1)
                    d2 |= 1;
            }
            int8_t n = (int8_t)(uint8_t)(d7 + d6 + 2);
            if (n < 8)
                d2 = (uint8_t)(d2 << (8 - n));
            ix_wb(vm, a1 + 0x100, d2);
            a1++;
            a2 = q;
        }
        a1 += 0x100;
        a0 = a2;
    } while (ix_rb(vm, a0) != 0x63);
    a0 = PROGRAM_LAB_051D;                              /* LAB_04EC */
    a1 = rl(p, PROGRAM_LAB_0518);
    for (;; a0 += 4) {
        uint16_t d0 = rw(p, a0);
        if (d0 == 0x63)
            break;
        ww(p, a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(d0 + d0), rw(p, a0 + 2));
    }
    a1 = rl(p, PROGRAM_LAB_051A);                       /* LAB_04EE */
    for (uint32_t k = 0; k < 8; k++)
        wl(p, PROGRAM_LAB_0510 + 4 * k, a1 + k * 0x200);
    a0 = PROGRAM_LAB_051E;
    uint16_t d6 = 9;
    for (int d7 = 0; d7 < 8; d7++) {
        for (unsigned d0 = 0; d0 < 256; d0++) {
            uint16_t d2 = 0;
            for (unsigned i = 0; i < d6; i++) {
                uint8_t d1 = ix_rb(vm, a0 + i);
                d2 = (uint16_t)(d2 << 1);
                if ((d0 >> (d1 & 31)) & 1)
                    d2 |= 1;
            }
            unsigned sh = (uint16_t)(16 - d6) & 63;
            d2 = sh >= 16 ? 0 : (uint16_t)(d2 << sh);
            ww(p, a1, d2);
            a1 += 2;
        }
        a0 += (uint32_t)(int32_t)(int16_t)d6;
        d6++;
    }
    a0 = PROGRAM_LAB_046C;                              /* LAB_046F */
    for (uint16_t d1 = 0xF0; (int16_t)d1 <= 0xFF; d1 = (uint16_t)(d1 + 0xF))
        for (int d0 = 7; d0 >= 0; d0--) {
            uint16_t d2 = (uint16_t)((d1 >> d0) | (d1 << ((16 - d0) & 15)));
            ix_wb(vm, a0++, (uint8_t)d2);
            ix_wb(vm, a0++, (uint8_t)(d2 >> 8));
        }
    a0 = PROGRAM_LAB_046E;
    for (int d0 = 7; d0 >= 0; d0--, a0 += 4)
        wl(p, a0, d0 ? (0xFFFF0000u >> d0) | (0xFFFF0000u << (32 - d0)) : 0xFFFF0000u);
}

/* SECSTRT_0 jusqu'à l'intro : écran, graphismes, tampons, moteur,
 * palette LAB_0274 (serveur LAB_057D), police et fond de texte (LAB_0051) */
void prog_boot(ProgIntro *p, uint32_t chip, uint32_t chip_size, uint32_t fast, uint32_t fast_size)
{
    wl(p, PROGRAM_LAB_00C2, chip);
    wl(p, PROGRAM_LAB_00C3, chip_size);
    wl(p, PROGRAM_LAB_00C4, fast);
    wl(p, PROGRAM_LAB_00C5, fast_size);
    ww(p, PROGRAM_LAB_0005, rw(p, 0x3E0));              /* EXT_0007 : drapeaux de mog */
    display_init(p);                                    /* SECSTRT_29 */
    prog_boot_graphics(VM);                             /* SECSTRT_25 */
    gfx_tables(p);
    ww(p, PROGRAM_LAB_04DE, 4);                         /* LAB_0006 : SECSTRT_23 */
    for (uint32_t i = 0; i < 256; i++) {                /*   LAB_04B0 */
        uint8_t b = 0;
        for (int k = 0; k < 8; k++)
            if (i & (1u << k))
                b |= (uint8_t)(0x80 >> k);
        ix_wb(VM, PROGRAM_LAB_04B3 + i, b);
    }
    ww(p, PROGRAM_LAB_0502, 40);                        /*   LAB_04A7 */
    ww(p, PROGRAM_LAB_0501, 200);
    ww(p, PROGRAM_LAB_0503, 0);
    swap_screens(p);
    buffers_init(p);                                    /* LAB_0044 */
    engine_init(p);                                     /* SECSTRT_10 */
    wl(p, PROGRAM_LAB_05D2, PROGRAM_LAB_0274);          /* SECSTRT_31 */
    uint32_t a = PROGRAM_LAB_0372;
    while (rl(p, a))
        a += 4;
    wl(p, a, PROGRAM_LAB_057D);
    wl(p, PROGRAM_LAB_011B, PROGRAM_LAB_0014);
    prog_load_cel(VM, PROGRAM_LAB_0053, rl(p, PROGRAM_LAB_011A + 16));  /* LAB_0051 */
    MogFile f;
    prog_file_open(VM, PROGRAM_LAB_0052, &f);
    prog_file_read(VM, &f, rl(p, PROGRAM_LAB_011A + 20), 0x10E6);
    prog_file_close(&f);
}

/* ------------------------------------------------ chargement, générique */

/* LAB_05A3 / LAB_05A4 : touche (SECSTRT_16) -> intro sautée (LAB_05E7) ;
 * fondu vers LAB_01CB, zones de texte restaurées */
static void credit_end(ProgIntro *p)
{
    if (rw(p, PROGRAM_SECSTRT_16))
        ww(p, PROGRAM_LAB_05E7, 1);
    wl(p, PROGRAM_LAB_0261, 2);
    static const uint16_t c[5] = { 0x0000, 0x0FED, 0x0DC9, 0x0B95, 0x0842 };
    static const uint8_t o[5] = { 10, 18, 20, 22, 24 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_LAB_01CB + o[i], c[i]);
    fade_to(p, PROGRAM_LAB_01CB, 2);
    restore_areas(p);
}

static void credit_colours(ProgIntro *p, uint16_t v)
{
    static const uint8_t o[5] = { 10, 18, 20, 22, 24 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_LAB_01CB + o[i], v);
}

/* LAB_059E : tuiles du haut du défilement (lune) comme fond */
static void credits_start(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x80; i++)                 /* LAB_035E */
        ix_wb(VM, PROGRAM_LAB_036D + i, 0);
    ww(p, PROGRAM_SECSTRT_16, 0);
    prog_fade_black(p);
    fill_lists(p);
    swap_screens(p);
    clear_screen(p, rl(p, PROGRAM_SECSTRT_30));
    clear_screen(p, rl(p, PROGRAM_LAB_056C));
    clear_screen(p, rl(p, PROGRAM_LAB_00C6));
    wl(p, PROGRAM_LAB_05D7, rl(p, PROGRAM_LAB_00C6));
    wl(p, PROGRAM_LAB_05D6, rl(p, PROGRAM_LAB_00C8));
    wl(p, PROGRAM_LAB_05D6 + 4, rl(p, PROGRAM_LAB_00C9));
    wl(p, PROGRAM_LAB_05D6 + 8, rl(p, PROGRAM_LAB_00CA));
    ww(p, PROGRAM_LAB_05B8, 0);
    ww(p, PROGRAM_LAB_05BD, 0);
    ww(p, PROGRAM_LAB_05BC, 8);
    draw_rows(p);
    show_background(p);
    swap_screens(p);                                    /* LAB_05A2 */
    credit_end(p);
}

/* LAB_059F : logo (frame $49 de la police) */
static void credits_logo(ProgIntro *p)
{
    credit_colours(p, 0);
    fade_to(p, PROGRAM_LAB_01CB, 0);
    set_planes(p, rl(p, PROGRAM_LAB_056C));
    ww(p, PROGRAM_LAB_04DF, 1);
    prog_draw_cel(VM, &p->blt, rl(p, PROGRAM_LAB_011A + 16), 0x49, 9, 0x3C);
    ww(p, PROGRAM_LAB_04DF, 0);
    swap_screens(p);
    wait_vbls(p, 8);
    credit_end(p);
}

/* LAB_05A1 : texte suivant du générique (LAB_05B1[LAB_05B0], 6 textes) */
static void credits_text(ProgIntro *p)
{
    credit_colours(p, 0x0FFF);
    wl(p, PROGRAM_LAB_0261, 1);
    fade_wait(p, PROGRAM_LAB_01CB);
    set_planes(p, rl(p, PROGRAM_LAB_056C));
    uint16_t n = rw(p, PROGRAM_LAB_05B0);
    if ((int16_t)n < 6) {
        text_records(p, rl(p, PROGRAM_LAB_05B1 + (uint32_t)(uint16_t)(n << 2)));
        ww(p, PROGRAM_LAB_05B0, (uint16_t)(n + 1));
        credit_colours(p, 0);
        fade_to(p, PROGRAM_LAB_01CB, 0);
    }
    swap_screens(p);
    credit_end(p);
}

/* LAB_05A0 : éclair blanc, fond remis, puis texte */
static void credits_flash(ProgIntro *p)
{
    credit_colours(p, 0x0FFF);
    wl(p, PROGRAM_LAB_0261, 1);
    fade_wait(p, PROGRAM_LAB_01CB);
    wait_vbls(p, 2);
    credit_colours(p, 0);
    fade_to(p, PROGRAM_LAB_01CB, 0);
    swap_screens(p);
    show_background(p);
    credits_text(p);
}

static int skipped(ProgIntro *p) { return rw(p, PROGRAM_LAB_05E7) != 0; }

/* LAB_0185 : chargement des décors, CEL et musique, générique par-dessus.
 * Renvoie 1 si l'intro est sautée (LAB_05E7). */
int prog_loading(ProgIntro *p)
{
    black(p);
    set_planes(p, rl(p, PROGRAM_SECSTRT_30));
    load_picture(p, PROGRAM_LAB_0184, rl(p, PROGRAM_LAB_00C9));     /* « mindscape » */
    fade_wait(p, PROGRAM_LAB_0506);
    set_planes(p, rl(p, PROGRAM_LAB_00C8));
    load_picture(p, PROGRAM_LAB_017E, rl(p, PROGRAM_LAB_00C4));
    save_palette(p, PROGRAM_LAB_01CB);
    MogFile f;
    prog_file_open(VM, PROGRAM_L08_00117, &f);                      /* intro.stile */
    prog_file_read(VM, &f, PROGRAM_SECSTRT_33, 1000);
    prog_file_close(&f);
    credits_start(p);
    set_planes(p, rl(p, PROGRAM_LAB_00C9));
    load_picture(p, PROGRAM_L08_00105, rl(p, PROGRAM_LAB_00C4));
    credits_logo(p);
    if (skipped(p))
        return 1;
    set_planes(p, rl(p, PROGRAM_LAB_00CA));
    load_picture(p, PROGRAM_LAB_0181, rl(p, PROGRAM_LAB_00C4));
    credits_flash(p);
    if (skipped(p))
        return 1;
    static const struct { uint32_t screen, name, pal; int tint; } bg[5] = {
        { PROGRAM_LAB_00CB, PROGRAM_L08_0004B, PROGRAM_LAB_01CE, 0 },
        { PROGRAM_LAB_00CC, PROGRAM_L08_00053, PROGRAM_LAB_01CF, 1 },
        { PROGRAM_LAB_00CD, PROGRAM_LAB_016D, PROGRAM_LAB_01D0, 1 },
        { PROGRAM_LAB_00CE, PROGRAM_L08_0006D, PROGRAM_LAB_01D1, 1 },
        { PROGRAM_LAB_00CF, PROGRAM_LAB_016E, PROGRAM_LAB_01D2, 1 },
    };
    for (int i = 0; i < 5; i++) {
        set_planes(p, rl(p, bg[i].screen));
        load_picture(p, bg[i].name, rl(p, PROGRAM_LAB_00C4));
        save_palette(p, bg[i].pal);
        if (bg[i].tint)
            tint(p, bg[i].pal);
        credits_text(p);
        if (skipped(p))
            return 1;
    }
    /* planches de CEL LAB_0276 : 0, 1, 4, 2 à la suite dans le bloc chip */
    static const struct { uint32_t name; uint32_t slot; } cel[4] = {
        { PROGRAM_SECSTRT_8, 0 }, { PROGRAM_LAB_0163, 4 },
        { PROGRAM_LAB_0164, 16 }, { PROGRAM_LAB_0166, 8 },
    };
    uint32_t a1 = rl(p, PROGRAM_LAB_00C2);
    for (int i = 0; i < 4; i++) {
        wl(p, PROGRAM_LAB_0276 + cel[i].slot, a1);
        prog_load_cel(VM, cel[i].name, a1);
        a1 = rl(p, PROGRAM_LAB_0276 + cel[i].slot) + prog_cel_size(VM, cel[i].name);
        credits_text(p);
        if (skipped(p))
            return 1;
    }
    a1 = rl(p, PROGRAM_LAB_0045);
    wl(p, PROGRAM_LAB_0276 + 12, a1);
    prog_load_cel(VM, PROGRAM_LAB_0165, a1);
    prog_cel_size(VM, PROGRAM_LAB_0165);
    a1 = rl(p, PROGRAM_LAB_00CF) + 0x9C40;
    wl(p, PROGRAM_LAB_0121, a1);
    prog_load_cel(VM, PROGRAM_LAB_0167, a1);
    prog_file_open(VM, PROGRAM_LAB_018D, &f);                       /* music.cmp */
    prog_file_read(VM, &f, rl(p, PROGRAM_LAB_0124), 0x159D9);
    prog_file_close(&f);
    rnc_unpack(p, rl(p, PROGRAM_LAB_0124));
    return 0;
}

/* SECSTRT_0 après le démarrage : l'intro entière, jusqu'au chargement de
 * mog (LAB_0000). LAB_005B : serveur de la musique retiré. */
void prog_intro(ProgIntro *p)
{
    wl(p, PROGRAM_LAB_0060, 0);
    wl(p, PROGRAM_LAB_0123, 0);
    wl(p, PROGRAM_LAB_00D0, 2);
    prog_fade_black(p);
    if (prog_loading(p) || skipped(p))
        return;
    prog_scene_05a5(p);
    wl(p, PROGRAM_LAB_0123, 4);
    prog_scene_001b(p);
    prog_scene_001c(p);
    prog_scene_0174(p);
    prog_scene_001a(p);
    prog_scene_002c(p);
    prog_scene_002d(p);
    prog_scene_002f(p);
    prog_scene_002e(p);
    prog_fade_black(p);
    prog_text_screen(p, PROGRAM_LAB_00AA);
    wait_vbls(p, 420);
    prog_fade_black(p);
    wl(p, rl(p, PROGRAM_LAB_0060), 0);                  /* LAB_005B */
}
