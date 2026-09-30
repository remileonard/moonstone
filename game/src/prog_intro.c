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
#include "ix_program_names.h"
#include "mog_struct.h"
#include "ix_data.h"
#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    if (!rw(p, PROGRAM_v_RightButtonOff) && !(rw(p, PROGRAM_v_RightButtonCount) & 0x8000))
        ww(p, PROGRAM_v_RightButtonCount, (uint16_t)(rw(p, PROGRAM_v_RightButtonCount) - 1));
    for (uint32_t a = PROGRAM_t_VblServers; rl(p, a); a += 4) {
        uint32_t s = rl(p, a);
        if (s == PROGRAM_Vbl_Screen)
            prog_vbl_colours(VM, p->colour);
        else if (s == PROGRAM_Vbl_Music)
            prog_music_vbl(p);
        else {
            static int warned;
            if (!warned++)
                fprintf(stderr, "prog : serveur VBL inconnu %06X\n", s);
        }
    }
}

/* LAB_0598 (fondu de la musique, prog_vbl.c) : AUDxVOL */
static ProgIntro *s_cur;

static void audvol(int voice, uint16_t vol)
{
    if (s_cur)
        prog_music_volume(s_cur, voice, vol);
}

void prog_wait_vbl(ProgIntro *p)
{
    s_cur = p;
    prog_audvol_hook = audvol;
    /* LAB_0331 (VBL) ; LAB_034D (souris) n'est pas repris */
    if (rw(p, PROGRAM_v_VblWait))
        wl(p, PROGRAM_v_VblCounter, rl(p, PROGRAM_v_VblCounter) + 1);
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
    wl(p, PROGRAM_v_FrameStartVbl, rl(p, PROGRAM_v_VblCounter));
}

static void frame_wait(ProgIntro *p)
{
    uint32_t d0 = rl(p, PROGRAM_v_VblCounter) - rl(p, PROGRAM_v_FrameStartVbl);
    int32_t d1 = (int32_t)(rl(p, PROGRAM_v_FrameVbls) + rl(p, PROGRAM_v_FrameVblsExtra) - d0);
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
    uint32_t s = rl(p, PROGRAM_v_DrawPlanes);
    for (uint32_t i = 0; i < 5; i++) {
        uint32_t a = s + i * PLANE;
        ww(p, COPPER_BPL + 8 * i, (uint16_t)(a >> 16));
        ww(p, COPPER_BPL + 8 * i + 4, (uint16_t)a);
    }
    wl(p, PROGRAM_v_DrawPlanes, rl(p, PROGRAM_v_ShowPlanes));
    wl(p, PROGRAM_v_ShowPlanes, s);
}

/* LAB_0262 : écran montré, listes LAB_0279 / LAB_027A échangées, plans de
 * dessin LAB_04D9-LAB_04DD (LAB_026C / LAB_04A6) sur le nouvel LAB_056C */
static void swap_screens(ProgIntro *p)
{
    show_screen(p);
    uint32_t d0 = rl(p, PROGRAM_v_RestoreFront);
    wl(p, PROGRAM_v_RestoreFront, rl(p, PROGRAM_v_RestoreBack));
    wl(p, PROGRAM_v_RestoreBack, d0);
    wl(p, PROGRAM_v_RestoreNext, rl(p, PROGRAM_v_RestoreFront));
    uint32_t s = rl(p, PROGRAM_v_DrawPlanes);
    static const uint32_t pl[5] = { PROGRAM_t_DestPlanes, PROGRAM_v_DestPlane1, PROGRAM_v_DestPlane2,
                                    PROGRAM_v_DestPlane3, PROGRAM_v_DestPlane4 };
    for (uint32_t i = 0; i < 5; i++)
        wl(p, pl[i], s + i * PLANE);
    p->a1 = s;
}

/* LAB_024B : listes de zones à restaurer (LAB_0286, LAB_0287) vidées */
static void fill_lists(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x820; i++)
        ix_wb(VM, PROGRAM_b_RestoreA + i, 0xFF);
}

/* LAB_0264 : copie d'écran (5 plans) par le blitter */
static void copy_screen(ProgIntro *p, uint32_t a0, uint32_t a1)
{
    MogBlitter *b = &p->blt;
    wl(p, PROGRAM_v_CopySrc, a0);
    wl(p, PROGRAM_v_CopyDst, a1);
    b->apt = a0;
    b->dpt = a1;
    b->amod = b->dmod = 0;
    b->afwm = b->alwm = 0xFFFF;
    b->con0 = 0x09F0;
    b->con1 = 0;
    for (int i = 0; i < 5; i++) {
        prog_blitter_run(VM, b, 0x3214);
        wl(p, PROGRAM_v_CopySrc, rl(p, PROGRAM_v_CopySrc) + PLANE);
        wl(p, PROGRAM_v_CopyDst, rl(p, PROGRAM_v_CopyDst) + PLANE);
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
    uint16_t h = rw(p, PROGRAM_v_TileH);
    if ((int16_t)(uint16_t)(h + y) > 200) {
        ww(p, PROGRAM_v_TileHClip, (uint16_t)(200 - y));
    } else {
        wl(p, PROGRAM_v_TileSkipRows, 0);
        ww(p, PROGRAM_v_TileClip05E1, 0);
        if ((int16_t)y < 0) {
            uint16_t d5 = (uint16_t)~y;
            ww(p, PROGRAM_v_TileHClip, (uint16_t)(h - d5));
            wl(p, PROGRAM_v_TileSkipRows, (uint16_t)((uint16_t)(d5 << 5) + (uint16_t)(d5 << 3)));
            ww(p, PROGRAM_v_TileY, 0);
            ww(p, PROGRAM_v_TileClip05E3, 0);
        }
    }
    wl(p, PROGRAM_v_TileClip05DF, 0);
    if ((int16_t)x < 0) {
        ww(p, PROGRAM_v_TileX, 0);
        ww(p, PROGRAM_v_TileClip05E2, 0);
    }
}

/* LAB_05C5 : tuile d2 de la planche a0 en (d0, d1) dans l'écran a1 */
static void draw_tile(ProgIntro *p, uint16_t x, uint16_t y, uint16_t tile,
                      uint32_t bank, uint32_t screen)
{
    ww(p, PROGRAM_v_TileX, x);
    ww(p, PROGRAM_v_TileY, y);
    if ((int16_t)y >= 200)
        return;
    wl(p, PROGRAM_v_TileSheet, bank);
    wl(p, PROGRAM_v_TileDst, screen);
    ww(p, PROGRAM_v_TileIndex, tile);
    ww(p, PROGRAM_v_TileHClip, rw(p, PROGRAM_v_TileH));
    ww(p, PROGRAM_v_TileWCopy, rw(p, PROGRAM_v_TileW));
    clip(p, x, y);
    /* LAB_05CD : coin de la tuile (10 par ligne de 32 × 25) */
    uint16_t t = (uint16_t)(rw(p, PROGRAM_v_TileIndex) & 0xFF);
    uint16_t ty = (uint16_t)(t / 10 * 25), tx = (uint16_t)(t % 10 * 32);
    ww(p, PROGRAM_v_TileSrcX, tx);
    ww(p, PROGRAM_v_TileSrcY, ty);
    uint32_t a0 = rl(p, PROGRAM_v_TileSheet) + pixel_offset(tx, ty) + rl(p, PROGRAM_v_TileSkipRows);
    wl(p, PROGRAM_v_TileSrcPtr, a0);
    uint32_t a1 = rl(p, PROGRAM_v_TileDst)
                + pixel_offset(rw(p, PROGRAM_v_TileX), rw(p, PROGRAM_v_TileY));
    wl(p, PROGRAM_v_TileDstPtr, a1);
    for (int i = 0; i < 5; i++) {
        blit_copy(p, rl(p, PROGRAM_v_TileSrcPtr), rl(p, PROGRAM_v_TileDstPtr), 0x24, 0x24, 2,
                  rw(p, PROGRAM_v_TileHClip));
        wl(p, PROGRAM_v_TileSrcPtr, rl(p, PROGRAM_v_TileSrcPtr) + PLANE);
        wl(p, PROGRAM_v_TileDstPtr, rl(p, PROGRAM_v_TileDstPtr) + PLANE);
    }
}

/* LAB_05BA : lignes LAB_05BD... (LAB_05BC lignes) de la fenêtre sur la
 * carte, à la position LAB_05B8 (en lignes d'écran) */
static void draw_rows(ProgIntro *p)
{
    uint16_t pos = rw(p, PROGRAM_v_ScrollPos);
    uint16_t q = (uint16_t)(pos / 25), r = (uint16_t)(pos % 25);
    uint16_t d1 = (uint16_t)(q * 20);
    ww(p, PROGRAM_v_ScrollRow, r);
    ww(p, PROGRAM_v_ScrollRowsLeft, (uint16_t)(25 - r));
    uint16_t bd = rw(p, PROGRAM_v_ScrollTopRow);
    d1 = (uint16_t)(d1 + (uint16_t)(20u * bd));
    uint32_t a0 = PROGRAM_b_TileMap + (uint32_t)(int32_t)(int16_t)d1;
    uint16_t x = 0;
    uint16_t y = (uint16_t)((uint16_t)(bd * 25u) - rw(p, PROGRAM_v_ScrollRow));
    uint16_t rows = rw(p, PROGRAM_v_ScrollRows);
    for (uint16_t d7 = 0;;) {
        int32_t t = (int16_t)rw(p, a0);              /* EXT.L */
        uint16_t k = (uint16_t)((uint32_t)t / 80u) * 4u;
        uint32_t bank = rl(p, PROGRAM_t_TileBanks + k);
        uint16_t tile = (uint16_t)(t - (int32_t)rl(p, PROGRAM_t_TileBankBase + k));
        draw_tile(p, x, y, tile, bank, rl(p, PROGRAM_v_TileScreen));
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
    copy_screen(p, rl(p, PROGRAM_v_TileScreen), rl(p, PROGRAM_v_DrawPlanes));
}

/* LAB_05C1 : l'écran monte de L31_00850 lignes, 2 lignes de tuiles en bas */
static void scroll_up(ProgIntro *p)
{
    uint16_t sp = rw(p, PROGRAM_v_ScrollSpeed);
    uint16_t d0 = (uint16_t)((uint16_t)(sp << 5) + (uint16_t)(sp << 3));
    uint32_t a1 = rl(p, PROGRAM_v_TileScreen);
    uint32_t a0 = rl(p, PROGRAM_v_TileScreen) + (uint32_t)(int32_t)(int16_t)d0;
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy(p, a0, a1, 0, 0, 20, (uint16_t)(200 - rw(p, PROGRAM_v_ScrollSpeed)));
    ww(p, PROGRAM_v_ScrollTopRow, 7);
    ww(p, PROGRAM_v_ScrollRows, 2);
    draw_rows(p);
}

/* LAB_05BF : l'écran descend, 2 lignes de tuiles en haut */
static void scroll_down(ProgIntro *p)
{
    uint16_t sp = rw(p, PROGRAM_v_ScrollSpeed);
    uint16_t d0 = (uint16_t)((uint16_t)(sp << 5) + (uint16_t)(sp << 3));
    uint32_t a0 = rl(p, PROGRAM_v_TileScreen);
    uint32_t a1 = rl(p, PROGRAM_v_TileScreen) + (uint32_t)(int32_t)(int16_t)d0;
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy_desc(p, a0, a1, 0, 0, 20, (uint16_t)(200 - rw(p, PROGRAM_v_ScrollSpeed)));
    ww(p, PROGRAM_v_ScrollTopRow, 0);
    ww(p, PROGRAM_v_ScrollRows, 2);
    draw_rows(p);
}

/* LAB_05B2 : d1 bit 2 = avance (LAB_05B8 croît, borné à 1000), bit 3 =
 * recul (borné à 0) ; aux bornes, la fenêtre entière est redessinée */
static void scroll(ProgIntro *p, uint16_t d1)
{
    uint16_t d0 = rw(p, PROGRAM_v_ScrollSpeed);
    if (d1 & 4) {
        uint16_t pos = (uint16_t)(rw(p, PROGRAM_v_ScrollPos) + d0);
        ww(p, PROGRAM_v_ScrollPos, pos);
        if ((int16_t)pos > 1000) {
            ww(p, PROGRAM_v_ScrollPos, 1000);
            ww(p, PROGRAM_v_ScrollTopRow, 0);
            ww(p, PROGRAM_v_ScrollRows, 8);
            draw_rows(p);
        } else
            scroll_up(p);
    } else if (d1 & 8) {
        uint16_t pos = (uint16_t)(rw(p, PROGRAM_v_ScrollPos) - d0);
        ww(p, PROGRAM_v_ScrollPos, pos);
        if (pos & 0x8000) {
            ww(p, PROGRAM_v_ScrollPos, 0);
            ww(p, PROGRAM_v_ScrollTopRow, 0);
            ww(p, PROGRAM_v_ScrollRows, 8);
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
    wl(p, PROGRAM_v_PalFadeTarget, a0);
    ww(p, PROGRAM_v_PalFadeDelay, d0);
    ww(p, PROGRAM_v_PalFadeCount, d0);
}

/* LAB_0258 : registres de couleur et palette courante à 0 ; la boucle
 * (DBNE après une écriture de 0) fait 33 tours : un mot de plus. */
static void black(ProgIntro *p)
{
    uint32_t a2 = rl(p, PROGRAM_v_PalCurrent);
    for (unsigned i = 0; i < 33; i++) {
        if (i < 32)
            p->colour[i] = 0;
        ww(p, a2 + 2 * i, 0);
    }
}

/* LAB_025D : palette a0 copiée dans la palette courante */
static void copy_palette(ProgIntro *p, uint32_t a0)
{
    uint32_t a1 = rl(p, PROGRAM_v_PalCurrent);
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
    copy_screen(p, rl(p, PROGRAM_v_BgPlanes), rl(p, PROGRAM_v_ShowPlanes));
    copy_screen(p, rl(p, PROGRAM_v_BgPlanes), rl(p, PROGRAM_v_DrawPlanes));
}

/* ------------------------------------------------------------ entités */

/* 40 entités de 42 octets (LAB_0282) : champs ENT_ du moteur de mog
 * (mog_struct.h ; ENT_OBJ est ici l'A1 de l'appelant), puis PENT_HIDDEN ;
 * contextes de 48 octets (LAB_0284) : champs CTX_, puis PCTX_EXTRA. */
#define NENT 40u

static uint32_t ent_addr(unsigned i) { return PROGRAM_t_Entities + i * PENT_SIZE; }

/* LAB_01E4 : entités effacées, blocs de travail attachés */
static void ent_reset(ProgIntro *p)
{
    for (uint32_t i = 0; i < NENT * PENT_SIZE; i++)
        ix_wb(VM, PROGRAM_t_Entities + i, 0);
    for (uint32_t i = 0; i < PCTX_SIZE; i++)
        ix_wb(VM, PROGRAM_t_Contexts + i, 0);
    fill_lists(p);
    for (unsigned i = 0; i < NENT; i++)
        wl(p, ent_addr(i) + ENT_CTX, PROGRAM_t_Contexts + i * PCTX_SIZE);
    p->a1 = PROGRAM_t_Contexts + NENT * PCTX_SIZE;
}

/* LAB_01DA : première entité libre (sinon LAB_0277 = 2) */
static void ent_spawn(ProgIntro *p, uint32_t script, uint32_t banks, uint16_t x, uint16_t y,
                      uint16_t depth, uint8_t dir, uint8_t ctl)
{
    for (unsigned i = 0; i < NENT; i++) {
        uint32_t a6 = ent_addr(i);
        if (ix_rb(VM, a6 + ENT_ACTIVE))
            continue;
        uint32_t a5 = rl(p, a6 + ENT_CTX);
        for (uint32_t k = 0; k < PCTX_SIZE; k++)
            ix_wb(VM, a5 + k, 0);
        wl(p, a6 + ENT_PC, script);
        wl(p, a6 + ENT_OBJ, p->a1);
        wl(p, a6 + ENT_BANKS, banks);
        ww(p, a6 + ENT_X, x);
        ww(p, a6 + ENT_HEIGHT, y);
        ww(p, a6 + ENT_DEPTH, depth);
        ix_wb(VM, a6 + ENT_DIR, dir);
        ix_wb(VM, a6 + ENT_CTL, ctl);
        ix_wb(VM, a6 + ENT_ACTIVE, 1);
        ix_wb(VM, a6 + ENT_BUSY, 1);
        return;
    }
    ww(p, PROGRAM_v_NoFreeEntity, 2);
}

/* LAB_0015 / LAB_0016 : personnage de gauche (x 160, sens 1) ou de droite
 * (x 120, sens 3) */
static void spawn_left(ProgIntro *p, uint32_t script)
{
    ent_spawn(p, script, PROGRAM_t_Banks, 0xA0, 0,
              (uint16_t)(100 + rw(p, PROGRAM_v_LeftDepth)), 1, 0);
}

static void spawn_right(ProgIntro *p, uint32_t script)
{
    ent_spawn(p, script, PROGRAM_t_Banks, 0x78, 0,
              (uint16_t)(100 + rw(p, PROGRAM_v_RightDepth)), 3, 0);
}

/* LAB_01E8 : contrôleurs des entités dont le script est fini
 * (table LAB_011B ; LAB_0014 : fin de la scène, entité libérée) */
static void ent_controllers(ProgIntro *p)
{
    for (unsigned i = 0; i < NENT; i++) {
        uint32_t a6 = ent_addr(i);
        if (!ix_rb(VM, a6 + ENT_ACTIVE) || ix_rb(VM, a6 + ENT_BUSY))
            continue;
        uint32_t fn = rl(p, PROGRAM_t_Controllers + ix_rb(VM, a6 + ENT_CTL));
        p->a1 = fn;
        if (fn == PROGRAM_Ctl_ScriptEnd) {
            ww(p, PROGRAM_v_ScriptEnded, 1);
            ww(p, a6 + ENT_ACTIVE, 0);
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
            if (rw(p, a0 + PENT_SIZE + ENT_DEPTH) >= rw(p, a0 + ENT_DEPTH))
                continue;
            for (uint32_t b = 0; b < PENT_SIZE; b++)
                ix_wb(VM, PROGRAM_b_EntitySwap + b, ix_rb(VM, a0 + b));
            for (uint32_t b = 0; b < PENT_SIZE; b++)
                ix_wb(VM, a0 + b, ix_rb(VM, a0 + PENT_SIZE + b));
            for (uint32_t b = 0; b < PENT_SIZE; b++)
                ix_wb(VM, a0 + PENT_SIZE + b, ix_rb(VM, PROGRAM_b_EntitySwap + b));
            swapped = 1;
        }
    } while (swapped);
}

/* LAB_04A8 (Cel_FlipFrame, comme ix_engine.c) : frame retournée en place */
static void flip_frame(ProgIntro *p, uint32_t cel, unsigned frame)
{
    if ((int)frame >= (int16_t)rw(p, cel + CEL_FRAMES))
        return;
    uint32_t fe = cel + CEL_TABLE + frame * CEL_ENTRY_SIZE;
    uint32_t a2 = rl(p, cel + CEL_PIXELS) + rl(p, fe + CELF_OFFSET);
    uint16_t w = rw(p, fe + CELF_W);
    uint16_t wr = (uint16_t)((w + 15) & 0xFFF0);
    uint16_t pad = (uint16_t)(wr - w);
    uint16_t bpr = (uint16_t)(wr >> 3);
    uint16_t h = rw(p, fe + CELF_H);
    if (ix_rb(VM, fe + CELF_FLAGS) & 1)
        ix_wb(VM, fe + CELF_FLAGS, (uint8_t)(pad << 4));
    else
        ix_wb(VM, fe + CELF_FLAGS, 1);
    uint8_t planes = ix_rb(VM, fe + CELF_PLANES);
    wl(p, PROGRAM_v_CelPlaneSize, (uint32_t)(uint16_t)(bpr << 1) * h);
    uint32_t tmp = rl(p, PROGRAM_v_CelPlanesBuf);
    for (int pl = 0; pl < 5; pl++) {
        if (!(planes & (1u << pl)))
            continue;
        for (unsigned y = 0; y < h; y++, a2 += bpr) {
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, tmp + bpr - 1 - i, ix_rb(VM, PROGRAM_t_BitReverse + ix_rb(VM, a2 + i)));
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, a2 + i, ix_rb(VM, tmp + i));
        }
    }
}

/* LAB_020B : taille de la frame ; retournée si son sens diffère */
static void frame_info(ProgIntro *p, uint32_t cel, uint16_t frame, uint32_t a1)
{
    uint32_t f = cel + (uint32_t)(int32_t)(int16_t)(uint16_t)(frame * CEL_ENTRY_SIZE);
    ww(p, a1 + ENT_W, rw(p, f + CEL_TABLE + CELF_W));
    ww(p, a1 + ENT_H, rw(p, f + CEL_TABLE + CELF_H));
    uint8_t d3 = ix_rb(VM, f + CEL_TABLE + CELF_FLAGS);
    if (d3 != 1)
        d3 = 3;
    if (d3 != ix_rb(VM, a1 + ENT_DIR))
        flip_frame(p, cel, frame);                      /* LAB_026E */
}

/* LAB_024F : rectangle englobant des frames dessinées */
static void bbox(ProgIntro *p, uint16_t x, uint16_t w, uint16_t y, uint16_t h)
{
    if (!rw(p, PROGRAM_v_BBoxSet)) {
        ww(p, PROGRAM_v_BBoxX0, x);
        ww(p, PROGRAM_v_BBoxX1, x);
        ww(p, PROGRAM_v_BBoxY0, y);
        ww(p, PROGRAM_v_BBoxY1, y);
        ww(p, PROGRAM_v_BBoxSet, 1);
    }
    if (!((int16_t)x > (int16_t)rw(p, PROGRAM_v_BBoxX0)))
        ww(p, PROGRAM_v_BBoxX0, x);
    uint16_t x2 = (uint16_t)(x + w);
    if (!((int16_t)x2 < (int16_t)rw(p, PROGRAM_v_BBoxX1)))
        ww(p, PROGRAM_v_BBoxX1, x2);
    if (!((int16_t)y > (int16_t)rw(p, PROGRAM_v_BBoxY0)))
        ww(p, PROGRAM_v_BBoxY0, y);
    uint16_t y2 = (uint16_t)(y + h);
    if (!((int16_t)y2 < (int16_t)rw(p, PROGRAM_v_BBoxY1)))
        ww(p, PROGRAM_v_BBoxY1, y2);
}

/* LAB_026C / LAB_04A6 : plans de dessin LAB_04D9-LAB_04DD */
static void set_planes(ProgIntro *p, uint32_t s)
{
    static const uint32_t pl[5] = { PROGRAM_t_DestPlanes, PROGRAM_v_DestPlane1, PROGRAM_v_DestPlane2,
                                    PROGRAM_v_DestPlane3, PROGRAM_v_DestPlane4 };
    for (uint32_t i = 0; i < 5; i++)
        wl(p, pl[i], s + i * PLANE);
}

/* LAB_022F (routine du script, registres préservés) */
static void flash(ProgIntro *p, uint32_t pal, uint32_t n)
{
    set_palette(p, pal);
    wait_vbls(p, n);
    set_palette(p, rl(p, PROGRAM_v_PalCurrent));
}

static void script_call(ProgIntro *p, uint32_t fn)
{
    if (fn == PROGRAM_Call_Lightning)                         /* éclair */
        flash(p, PROGRAM_t_FlashLightning, 8);
    else if (fn == PROGRAM_Call_Storm) {                  /* orage : LAB_0041 × 6 */
        static const uint8_t w[7] = { 20, 2, 20, 5, 10, 5, 50 };
        for (int i = 0; i < 6; i++) {
            wait_vbls(p, w[i]);
            flash(p, PROGRAM_t_FlashStorm, 2);
        }
        wait_vbls(p, w[6]);
    } else if (fn == PROGRAM_Call_EndGlows) {                /* fin : pulsations */
        wl(p, PROGRAM_v_EndGlowA, prog_glow(VM, 12, rw(p, PROGRAM_v_EndColourA), 5, 0));
        wl(p, PROGRAM_v_EndGlowB, prog_glow(VM, 15, rw(p, PROGRAM_v_EndColourB), 5, 0));
        wl(p, PROGRAM_v_EndGlowC, prog_glow(VM, 23, rw(p, PROGRAM_v_EndColourC), 5, 0));
    } else if (fn == PROGRAM_Call_ScrollEnd)                  /* fin du défilement */
        ww(p, PROGRAM_v_ScrollDone, 1);
    else if (fn == PROGRAM_Call_ScrollSlow)                    /* défilement ralenti */
        ww(p, PROGRAM_v_ScrollSpeed, (uint16_t)(rw(p, PROGRAM_v_ScrollSpeed) - 1));
    else
        fprintf(stderr, "prog : routine de script inconnue %06X\n", fn);
}

/* Commandes du script (octet >= $80, table LAB_0288) */
static void ent_command(ProgIntro *p, uint32_t a1, uint32_t a6, uint8_t op)
{
    uint32_t h = rl(p, PROGRAM_t_IxOpcodes + op);
    uint32_t a5 = rl(p, a1 + ENT_CTX);
    uint8_t b1 = ix_rb(VM, a6 + 1);
    if (h == PROGRAM_IxOp80_SetDir) {                        /* sens */
        if (b1 == 0xFF)
            ix_wb(VM, a1 + ENT_DIR, (uint8_t)(ix_rb(VM, a1 + ENT_DIR) ^ 2));
        else
            ix_wb(VM, a1 + ENT_DIR, b1);
        wl(p, a1 + ENT_PC, a6 + 2);
    } else if (h == PROGRAM_IxOp84_Jump) {                 /* saut / suite */
        if (b1 == 3) {
            wl(p, a1 + ENT_PC, rl(p, a6 + 2));
            return;
        }
        wl(p, a5 + CTX_JUMP_PC, rl(p, a6 + 2));
        ix_wb(VM, a5 + CTX_JUMP_ON, 1);
        wl(p, a1 + ENT_PC, a6 + 6);
    } else if (h == PROGRAM_IxOp88_Hold) {                 /* répétition du groupe */
        uint8_t n = b1;
        if (!n) {
            n = (uint8_t)(rl(p, PROGRAM_v_VblCounter) & 0x1F);
            if (!n)
                n = 1;
        }
        ix_wb(VM, a5 + CTX_HOLD_N, n);
        ix_wb(VM, a5 + CTX_HOLD_ON, 1);
        wl(p, a1 + ENT_PC, a6 + 2);
        wl(p, a5 + CTX_HOLD_PC, a6 + 2);
    } else if (h == PROGRAM_IxOp8C_Skip) {
        wl(p, a1 + ENT_PC, a6 + 8);
    } else if (h == PROGRAM_IxOp94_Loop) {                 /* boucle */
        ix_wb(VM, a5 + CTX_LOOP_N, b1);
        ix_wb(VM, a5 + CTX_LOOP_ON, 1);
        wl(p, a1 + ENT_PC, a6 + 2);
        wl(p, a5 + CTX_LOOP_PC, a6 + 2);
    } else if (h == PROGRAM_IxOpA4_Sound) {
        wl(p, a1 + ENT_PC, a6 + 4);
    } else if (h == PROGRAM_IxOpA0_Move) {                 /* déplacement */
        if (b1 & 0x40) {
            ww(p, a1 + ENT_X, rw(p, a6 + 2));
            ww(p, a1 + ENT_HEIGHT, rw(p, a6 + 4));
            ww(p, a1 + ENT_DEPTH, rw(p, a6 + 6));
        } else {
            uint16_t d0 = rw(p, a6 + 2);
            int add = ix_rb(VM, a1 + ENT_DIR) == 3 ? !(b1 & 1) : (b1 & 1);
            ww(p, a1 + ENT_X, (uint16_t)(add ? rw(p, a1 + ENT_X) + d0 : rw(p, a1 + ENT_X) - d0));
            d0 = rw(p, a6 + 4);
            ww(p, a1 + ENT_HEIGHT, (uint16_t)(b1 & 8 ? rw(p, a1 + ENT_HEIGHT) - d0 : rw(p, a1 + ENT_HEIGHT) + d0));
            d0 = rw(p, a6 + 6);
            ww(p, a1 + ENT_DEPTH, (uint16_t)(b1 & 0x20 ? rw(p, a1 + ENT_DEPTH) - d0 : rw(p, a1 + ENT_DEPTH) + d0));
        }
        wl(p, a1 + ENT_PC, a6 + 8);
    } else if (h == PROGRAM_IxOpB4_Call) {                 /* appel */
        if (b1)
            fprintf(stderr, "prog : LAB_0280[%u] non repris\n", b1);
        else
            script_call(p, rl(p, a6 + 2));
        wl(p, a1 + ENT_PC, rl(p, a1 + ENT_PC) + 6);
    } else if (h == PROGRAM_IxOpC8_Skip6 || h == PROGRAM_IxOpB8_Skip6 || h == PROGRAM_IxOpBC_Skip6) {
        wl(p, a1 + ENT_PC, a6 + 6);
    } else if (h == PROGRAM_IxOpC0_End) {                 /* fin de l'entité */
        ww(p, a1 + ENT_ACTIVE, 0);
        wl(p, a1 + ENT_PC, a6 + 2);
    } else if (h == PROGRAM_IxOpC4_SetBank) {                 /* planches */
        wl(p, a1 + ENT_BANKS, rl(p, PROGRAM_t_EntSheets + (uint32_t)(uint16_t)((b1 - 1) << 2)));
        wl(p, a1 + ENT_PC, a6 + 2);
    } else if (h == PROGRAM_IxOpCC_IfZero || h == PROGRAM_IxOpD0_IfNonZero) {    /* test */
        uint32_t a = rl(p, a1 + ENT_OBJ) + (uint32_t)(int32_t)(int16_t)rw(p, a6 + 2);
        uint32_t v = b1 & 1 ? ix_rb(VM, a) : b1 & 2 ? rw(p, a) : rl(p, a);
        if ((h == PROGRAM_IxOpCC_IfZero) == !v)
            wl(p, a1 + ENT_PC, rl(p, a6 + 4));
        else
            wl(p, a1 + ENT_PC, a6 + 8);
    } else if (h == PROGRAM_IxOpD4_Reset) {
        for (uint32_t k = 0; k < PCTX_SIZE; k++)
            ix_wb(VM, a5 + k, 0);
        wl(p, a1 + ENT_PC, a6 + 2);
    } else {
        fprintf(stderr, "prog : commande de script %02X (%06X) sans effet\n", op | 0x80, h);
        wl(p, a1, 0);                   /* l'original boucle sans fin */
    }
}

/* LAB_0201 : fin d'un groupe de frames ($FF) : répétitions, retours */
static void ent_group_end(ProgIntro *p, uint32_t a1, uint32_t a6, uint16_t d1, uint16_t d2)
{
    uint32_t a5 = rl(p, a1 + ENT_CTX);
    if (ix_rb(VM, a5 + CTX_HOLD_ON)) {
        uint8_t c = (uint8_t)(ix_rb(VM, a5 + CTX_HOLD_N) - 1);
        ix_wb(VM, a5 + CTX_HOLD_N, c);
        if (c) {
            wl(p, a1 + ENT_PC, rl(p, a5 + CTX_HOLD_PC));
            return;
        }
    }
    ix_wb(VM, a5 + CTX_HOLD_ON, 0);
    if (ix_rb(VM, a5 + CTX_PHY_ON)) {
        uint8_t c = (uint8_t)(ix_rb(VM, a5 + CTX_PHY_N) - 1);
        ix_wb(VM, a5 + CTX_PHY_N, c);
        if (!(c & 0x80))
            return;                                     /* LAB_022E : RTS */
    } else {
        ix_wb(VM, a5 + CTX_PHY_ON, 0);
        if (ix_rb(VM, a5 + PCTX_EXTRA)) {
            fprintf(stderr, "prog : bloc +42 non repris\n");
            ww(p, a1 + ENT_X, d1);
            ww(p, a1 + ENT_DEPTH, d2);
            if (!(uint16_t)a5)
                return;
        }
    }
    ix_wb(VM, a5 + PCTX_EXTRA, 0);
    if (ix_rb(VM, a5 + CTX_JUMP_ON)) {
        ix_wb(VM, a5 + CTX_JUMP_ON, 0);
        wl(p, a1 + ENT_PC, rl(p, a5 + CTX_JUMP_PC));
        return;
    }
    uint8_t b1 = ix_rb(VM, a6 + 1);
    if (b1 == 0xFF || b1 == 0xFE) {
        if (ix_rb(VM, a5 + CTX_LOOP_ON)) {
            uint8_t c = (uint8_t)(ix_rb(VM, a5 + CTX_LOOP_N) - 1);
            ix_wb(VM, a5 + CTX_LOOP_N, c);
            if (c) {
                wl(p, a1 + ENT_PC, rl(p, a5 + CTX_LOOP_PC));
                return;
            }
        }
        ix_wb(VM, a5 + CTX_LOOP_ON, 0);
        if (b1 == 0xFF) {
            ix_wb(VM, a1 + ENT_BUSY, 0);                       /* script fini */
            return;
        }
    }
    wl(p, a1 + ENT_PC, rl(p, a1 + ENT_PC) + 2);
}

/* LAB_01F1 : script de l'entité a1 pour cette image (frames jusqu'à $FF) */
static void ent_script(ProgIntro *p, uint32_t a1)
{
    while (rl(p, a1 + ENT_PC)) {                             /* LAB_01F2 */
        uint16_t d1 = 0, d2 = 0;
        uint32_t a6;
        uint8_t op;
        for (;;) {                                      /* LAB_01F3 */
            a6 = rl(p, a1 + ENT_PC);
            op = ix_rb(VM, a6);
            if (op == 0xFF) {
                ent_group_end(p, a1, a6, d1, d2);
                return;
            }
            if (op == 0xFD)
                wl(p, a1 + ENT_PC, rl(p, rl(p, a1 + ENT_CTX) + CTX_RESUME_PC));
            else if (op == 0xFE)
                wl(p, a1 + ENT_PC, rl(p, rl(p, a1 + ENT_CTX) + CTX_LOOP_PC));
            else if (op & 0x80)
                ent_command(p, a1, a6, op & 0x7F);
            else
                break;
        }
        /* frame : planche, n°, dy, drapeaux, dx */
        uint32_t cel = rl(p, rl(p, a1 + ENT_BANKS) + (op & 0x1F));
        uint8_t fr = ix_rb(VM, a6 + 1);
        ix_wb(VM, a1 + ENT_FRAME, fr);
        frame_info(p, cel, fr, a1);
        d2 = (uint16_t)(int16_t)(int8_t)ix_rb(VM, a6 + 2);
        if (ix_rb(VM, a1 + ENT_DIR) & 2)
            d1 = (uint16_t)(rw(p, a1 + ENT_X) - rw(p, a6 + 4) - rw(p, a1 + ENT_W));
        else
            d1 = (uint16_t)(rw(p, a6 + 4) + rw(p, a1 + ENT_X));
        d2 = (uint16_t)(d2 + rw(p, a1 + ENT_HEIGHT) + rw(p, a1 + ENT_DEPTH));
        ww(p, a1 + ENT_DRAW_X, d1);
        ww(p, a1 + ENT_DRAW_Y, d2);
        uint8_t fl = ix_rb(VM, a6 + 3);
        if (!(fl & 0x40))
            bbox(p, d1, rw(p, a1 + ENT_W), d2, rw(p, a1 + ENT_H));
        ix_wb(VM, a1 + ENT_FLAGS, fl);
        uint16_t d0 = ix_rb(VM, a1 + ENT_FRAME);
        if (fl & 0x10) {                                /* aussi dans le décor */
            set_planes(p, rl(p, PROGRAM_v_BgPlanes));
            prog_draw_cel(VM, &p->blt, cel, d0, d1, d2);
            set_planes(p, rl(p, PROGRAM_v_DrawPlanes));
        } else {                                        /* zone à restaurer */
            uint32_t a5 = rl(p, PROGRAM_v_RestoreNext);
            ww(p, a5 + SCR_X, rw(p, a1 + ENT_DRAW_X));
            ww(p, a5 + SCR_Y, rw(p, a1 + ENT_DRAW_Y));
            ww(p, a5 + SCR_W, rw(p, a1 + ENT_W));
            ww(p, a5 + SCR_H, rw(p, a1 + ENT_H));
            ww(p, a5 + SCR_SIZE + SCR_W, 0xFFFF);
            wl(p, PROGRAM_v_RestoreNext, a5 + SCR_SIZE);
        }
        static const uint32_t lists[2] = { PROGRAM_v_ListBody, PROGRAM_v_ListStrike };
        for (int k = 1; k >= 0; k--)
            if (fl & (1u << k)) {
                uint32_t a4 = rl(p, lists[k]);
                wl(p, a4 + DL_CEL, cel);
                ww(p, a4 + DL_FRAME, d0);
                ww(p, a4 + DL_X, d1);
                ww(p, a4 + DL_Y, d2);
                wl(p, a4 + DL_SIZE + DL_CEL, 0);
                wl(p, lists[k], a4 + DL_SIZE);
            }
        ww(p, PROGRAM_v_BlitByCpu, fl & 0x20 ? 1 : 0);
        prog_draw_cel(VM, &p->blt, cel, d0, d1, d2);
        wl(p, a1 + ENT_PC, rl(p, a1 + ENT_PC) + 6);
    }
}

/* LAB_01EC : entités triées, scripts joués (+18 du bloc : double) */
static void ent_render(ProgIntro *p)
{
    ent_sort(p);
    uint32_t a1 = PROGRAM_t_Entities;
    for (unsigned i = 0; i < NENT; i++, a1 += PENT_SIZE) {
        ww(p, PROGRAM_v_EntIndex, (uint16_t)i);
        wl(p, PROGRAM_v_EntRenderPtr, a1);
        if (!ix_rb(VM, a1 + ENT_ACTIVE) || rw(p, a1 + PENT_HIDDEN))
            continue;
        uint32_t a5 = rl(p, a1 + ENT_CTX);
        ww(p, PROGRAM_v_BBoxSet, 0);
        if (!ix_rb(VM, a5 + CTX_SHADOW_ON)) {
            ent_script(p, a1);
            continue;
        }
        for (uint32_t b = 0; b < PENT_SIZE; b++)
            ix_wb(VM, PROGRAM_b_EntitySwap + b, ix_rb(VM, a1 + b));
        ww(p, PROGRAM_b_EntitySwap + ENT_HEIGHT, 0);
        wl(p, PROGRAM_b_EntitySwap + ENT_PC, rl(p, a5 + CTX_SHADOW_PC));
        wl(p, PROGRAM_b_EntitySwap + ENT_CTX, PROGRAM_v_ShadowCtx);
        ent_script(p, PROGRAM_b_EntitySwap);
        ent_script(p, a1);
    }
    ww(p, PROGRAM_v_EntIndex, NENT);
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
    uint32_t a0 = rl(p, PROGRAM_v_BgPlanes) + (uint32_t)(int32_t)off;
    uint32_t a1 = rl(p, PROGRAM_v_DrawPlanes) + (uint32_t)(int32_t)off;
    uint16_t mod = (uint16_t)(40 - d2 - d2);
    for (int i = 0; i < 5; i++, a0 += PLANE, a1 += PLANE)
        blit_copy(p, a0, a1, mod, mod, (uint16_t)d2, (uint16_t)d3);
    p->a1 = a1 - PLANE;
}

/* LAB_0242 : zones de la liste LAB_0279 (au plus 130) restaurées */
static void restore_areas(ProgIntro *p)
{
    wl(p, PROGRAM_v_RestoreCount, 0);
    wl(p, PROGRAM_v_DrawScreen, rl(p, PROGRAM_v_DrawPlanes));
    for (uint32_t a6 = rl(p, PROGRAM_v_RestoreFront);; a6 += 8) {
        if (rw(p, a6 + 4) == 0xFFFF || rl(p, PROGRAM_v_RestoreCount) == 130
            || !rw(p, a6 + 6) || !rw(p, a6 + 4))
            return;
        restore_area(p, a6);
        wl(p, PROGRAM_v_RestoreCount, rl(p, PROGRAM_v_RestoreCount) + 1);
    }
}

/* LAB_003C : cercle de pierres par-dessus (LAB_0121 ; LAB_003E = 2 :
 * deux pierres seulement) */
static void overlay(ProgIntro *p)
{
    uint32_t cel = rl(p, PROGRAM_v_OverlayCel);
    ww(p, PROGRAM_v_BlitByCpu, 1);
    prog_draw_cel(VM, &p->blt, cel, 0, 0x0000, 0x38);
    prog_draw_cel(VM, &p->blt, cel, 3, 0x0033, 0x86);
    if (rw(p, PROGRAM_v_OverlayMode) != 2) {
        prog_draw_cel(VM, &p->blt, cel, 1, 0x009D, 0x68);
        prog_draw_cel(VM, &p->blt, cel, 2, 0x0129, 0x52);
    }
    ww(p, PROGRAM_v_BlitByCpu, 0);
}

/* LAB_000F : palette de la scène, une fois (LAB_011E : 2 fondu, 3 vers
 * le noir, 5 noir, autre : LAB_011D d'un coup) */
static void scene_palette(ProgIntro *p)
{
    ww(p, PROGRAM_v_ScenePaletteDone, 1);
    uint32_t pal = rl(p, PROGRAM_v_ScenePalette);
    switch (rw(p, PROGRAM_v_ScenePaletteMode)) {
    case 2: fade_to(p, pal, 2); break;
    case 5: black(p); break;
    case 3: fade_to(p, PROGRAM_t_PalBlack, 2); break;
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
    if (stop_on_end && rw(p, PROGRAM_v_ScriptEnded))
        return 1;
    ent_render(p);
    if (rw(p, PROGRAM_v_SceneOverlay))
        overlay(p);
    swap_screens(p);
    restore_areas(p);
    if (!rw(p, PROGRAM_v_ScenePaletteDone))
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
    while ((ww(p, PROGRAM_v_FrameCount, (uint16_t)(rw(p, PROGRAM_v_FrameCount) + 1)),
            rw(p, PROGRAM_v_FrameCount) != 16));
}

/* LAB_001D : un personnage de la liste L00_0060A toutes les
 * 16 - L00_0060E images */
static void procession(ProgIntro *p)
{
    do {
        ww(p, PROGRAM_v_FrameCount, rw(p, PROGRAM_v_ProcessionGap));
        p->a1 = rl(p, PROGRAM_t_Procession);
        spawn_left(p, rl(p, p->a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(rw(p, PROGRAM_v_ProcessionIndex) << 2)));
        run_frames(p);
        ww(p, PROGRAM_v_ProcessionIndex, (uint16_t)(rw(p, PROGRAM_v_ProcessionIndex) + 1));
    } while (rw(p, PROGRAM_v_ProcessionCount) != rw(p, PROGRAM_v_ProcessionIndex));
}

/* LAB_001F : même chose, à gauche et à droite en alternance (LAB_0026) */
static void procession2(ProgIntro *p)
{
    do {
        ww(p, PROGRAM_v_FrameCount, 0);
        p->a1 = rl(p, PROGRAM_t_Procession);
        uint32_t a0 = rl(p, p->a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(rw(p, PROGRAM_v_ProcessionIndex) << 2));
        uint16_t s = (uint16_t)(rw(p, PROGRAM_v_ProcessionSide) ^ 1);
        ww(p, PROGRAM_v_ProcessionSide, s);
        if (s)
            spawn_left(p, a0);
        else
            spawn_right(p, a0);
        run_frames(p);
        ww(p, PROGRAM_v_ProcessionIndex, (uint16_t)(rw(p, PROGRAM_v_ProcessionIndex) + 1));
    } while (rw(p, PROGRAM_v_ProcessionCount) != rw(p, PROGRAM_v_ProcessionIndex));
}

/* ------------------------------------------------------------- scènes */

void prog_scene_05a5(ProgIntro *p)
{
    fill_lists(p);
    ww(p, PROGRAM_t_PalForest + 18, 0x0A00);
    ww(p, PROGRAM_t_PalForest + 20, 0x0600);
    ww(p, PROGRAM_t_PalForest + 22, 0x0300);
    ww(p, PROGRAM_t_PalForest + 24, 0x0FC6);
    set_palette(p, PROGRAM_t_PalForest);
    do {
        /* vitesse selon la position : LAB_05A9 (bornes), L31_00674 */
        uint32_t d0 = 0;
        while ((int16_t)rw(p, PROGRAM_t_ScrollSpeedLimits + d0) < (int16_t)rw(p, PROGRAM_v_ScrollPos))
            d0 += 2;
        ww(p, PROGRAM_v_ScrollSpeed, rw(p, PROGRAM_t_ScrollSpeeds + d0));
        frame_mark(p);
        scroll(p, 4);
        swap_screens(p);
        if (rw(p, PROGRAM_v_ScrollSpeed) == 4)
            prog_music_start(p);                        /* SECSTRT_1 */
        frame_wait(p);
    } while ((int16_t)rw(p, PROGRAM_v_ScrollPos) < 1000);
    copy_screen(p, rl(p, PROGRAM_v_ShowPlanes), rl(p, PROGRAM_v_DrawPlanes));
    copy_screen(p, rl(p, PROGRAM_v_ShowPlanes), rl(p, PROGRAM_v_BgPlanes));
}

/* LAB_001B : procession des druides dans la forêt */
void prog_scene_001b(ProgIntro *p)
{
    ent_reset(p);
    ww(p, PROGRAM_v_ProcessionGap, 4);
    ww(p, PROGRAM_v_ScenePaletteDone, 1);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ProcessionIndex, 0);
    wl(p, PROGRAM_t_Procession, PROGRAM_t_ProcessionForest);
    ww(p, PROGRAM_v_ProcessionCount, 5);
    wl(p, PROGRAM_v_FrameVbls, 8);
    procession(p);
}

/* LAB_001C : arrivée à Stonehenge (pierres par-dessus, LAB_003C) */
void prog_scene_001c(ProgIntro *p)
{
    black(p);
    ent_reset(p);
    copy_long(p, rl(p, PROGRAM_b_PivBg2), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalBg2);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ProcessionIndex, 0);
    wl(p, PROGRAM_t_Procession, PROGRAM_t_ProcessionStonehenge);
    ww(p, PROGRAM_v_ProcessionCount, 5);
    ww(p, PROGRAM_v_ProcessionGap, 8);
    ww(p, PROGRAM_v_SceneOverlay, 1);
    procession(p);
    ww(p, PROGRAM_v_SceneOverlay, 0);
    wl(p, PROGRAM_v_FrameVbls, 6);
}

/* LAB_0174 : décors et palettes des scènes suivantes */
void prog_scene_0174(ProgIntro *p)
{
    copy_long(p, rl(p, PROGRAM_b_PivBg4), rl(p, PROGRAM_v_BgSceneB));
    copy_colours(p, PROGRAM_t_PalBg4, PROGRAM_t_PalForest);
    copy_long(p, rl(p, PROGRAM_b_PivBg5a), rl(p, PROGRAM_v_BgSceneC));
    copy_colours(p, PROGRAM_t_PalBg5a, PROGRAM_t_PalDruid);
    copy_long(p, rl(p, PROGRAM_b_PivBg3), rl(p, PROGRAM_v_BgSceneD));
    copy_colours(p, PROGRAM_t_PalBg3, PROGRAM_t_PalCircle);
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
    static const uint32_t c[3] = { PROGRAM_x_CircleDruidDC, PROGRAM_x_CircleDruidDD, PROGRAM_x_CircleDruidDA };
    ent_reset(p);
    black(p);
    swap_screens(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneD));
    show_background(p);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalCircle);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    spawn_left(p, c[0]);
    spawn_left(p, c[1]);
    spawn_left(p, c[2]);
    spawn_right(p, c[0]);
    spawn_right(p, c[1]);
    spawn_right(p, PROGRAM_x_CircleDruidD9);
    ww(p, PROGRAM_v_ProcessionIndex, 0);
    wl(p, PROGRAM_t_Procession, PROGRAM_t_ProcessionCircle);
    ww(p, PROGRAM_v_ProcessionCount, 4);
    ww(p, PROGRAM_v_ProcessionGap, 0);
    procession2(p);
    spawn_left(p, PROGRAM_x_CentralDruid);
    run_until_end(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneA));
}

/* LAB_002C : le grand druide (deux plans) */
void prog_scene_002c(ProgIntro *p)
{
    black(p);
    swap_screens(p);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ent_reset(p);
    spawn_left(p, PROGRAM_x_Druid);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneB));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalForest);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    run_until_end(p);
    black(p);
    ent_reset(p);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    spawn_left(p, PROGRAM_x_DruidClose);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneC));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalDruid);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    run_until_end(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneA));
}

/* LAB_002D : le chevalier à Stonehenge */
void prog_scene_002d(ProgIntro *p)
{
    ww(p, PROGRAM_v_SceneOverlay, 1);
    ww(p, PROGRAM_v_OverlayMode, 2);
    ent_reset(p);
    black(p);
    copy_long(p, rl(p, PROGRAM_b_PivBg2a), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalBg2a);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    spawn_left(p, PROGRAM_x_KnightStonehenge);
    run_until_end(p);
    ww(p, PROGRAM_v_SceneOverlay, 0);
    ww(p, PROGRAM_v_OverlayMode, 0);
}

/* LAB_002F : le cercle, vu de haut (seconde fois) */
void prog_scene_002f(ProgIntro *p)
{
    static const uint32_t c[5] = { PROGRAM_x_CircleDruidDC, PROGRAM_x_CircleDruidDD, PROGRAM_x_CircleDruidDB,
                                   PROGRAM_x_CircleDruidDF, PROGRAM_x_CircleDruidE1 };
    ent_reset(p);
    black(p);
    swap_screens(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneD));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalCircle);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    circle(p, c, 5);
    spawn_left(p, PROGRAM_x_CircleIdle);
    spawn_left(p, PROGRAM_x_KnightEntersCircle);
    wl(p, PROGRAM_v_FrameVbls, 8);
    run_until_end(p);
    wl(p, PROGRAM_v_FrameVbls, 6);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneA));
}

/* LAB_002E : l'adoubement */
void prog_scene_002e(ProgIntro *p)
{
    ent_reset(p);
    black(p);
    copy_long(p, rl(p, PROGRAM_b_PivBg5a), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalBg5a);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    spawn_left(p, PROGRAM_x_DruidKnighting);
    run_until_end(p);
}

/* ------------------------------------------------------------- texte */

/* Lettre c de la police rl(LAB_011A + 16) : frame (table LAB_00F8),
 * largeur LAB_00F1 et hauteur LAB_00F2 */
static uint16_t glyph(ProgIntro *p, uint8_t c)
{
    uint16_t f = ix_rb(VM, PROGRAM_t_FontGlyphFrame + (uint8_t)(c - 0x20));
    uint32_t e = rl(p, PROGRAM_t_FontBank + 16) + (uint32_t)(int32_t)(int16_t)(uint16_t)(f * 10);
    ww(p, PROGRAM_v_GlyphWidth, rw(p, e + 14));
    ww(p, PROGRAM_v_GlyphHeight, rw(p, e + 16));
    return f;
}

/* LAB_0297 : largeur de la chaîne LAB_00F7 (lettres rapprochées de 2) */
static uint16_t text_width(ProgIntro *p)
{
    ww(p, PROGRAM_v_TextWidth, 0);
    for (uint32_t a = rl(p, PROGRAM_v_TextChar); ix_rb(VM, a); a++) {
        glyph(p, ix_rb(VM, a));
        ww(p, PROGRAM_v_TextWidth, (uint16_t)(rw(p, PROGRAM_v_TextWidth) + rw(p, PROGRAM_v_GlyphWidth) - 2));
    }
    return rw(p, PROGRAM_v_TextWidth);
}

/* LAB_028F : enregistrements de texte (long chaîne, mot x, mot y, octet,
 * octet drapeaux : bit 0 centré, bit 1 zone à restaurer ; long suivant),
 * masque LAB_04DF levé */
static void text_records(ProgIntro *p, uint32_t a0)
{
    ww(p, PROGRAM_v_BlitByCpu, 1);
    if (a0) {
        wl(p, PROGRAM_v_TextRecord, a0);
        do {
            uint32_t a2 = rl(p, PROGRAM_v_TextRecord);
            wl(p, PROGRAM_v_TextChar, rl(p, a2));
            ww(p, PROGRAM_v_TextX, rw(p, a2 + 4));
            ww(p, PROGRAM_v_TextLineX, rw(p, a2 + 4));
            ww(p, PROGRAM_v_TextY, rw(p, a2 + 6));
            ww(p, PROGRAM_v_TextTopY, rw(p, a2 + 6));
            if (ix_rb(VM, a2 + 9) & 1) {
                uint16_t x = (uint16_t)((uint16_t)(320 - text_width(p)) >> 1);
                ww(p, PROGRAM_v_TextX, x);
                ww(p, PROGRAM_v_TextLineX, x);
            }
            for (;;) {                                  /* LAB_0291 */
                uint32_t a = rl(p, PROGRAM_v_TextChar);
                uint8_t c = ix_rb(VM, a);
                if (!c)
                    break;
                wl(p, PROGRAM_v_TextChar, a + 1);
                uint16_t f = glyph(p, c);
                uint16_t x = rw(p, PROGRAM_v_TextX), y = rw(p, PROGRAM_v_TextY);
                if (ix_rb(VM, rl(p, PROGRAM_v_TextRecord) + TXT_FLAGS) & 2) {  /* LAB_02A0 */
                    uint32_t a5 = rl(p, PROGRAM_v_RestoreNext);
                    ww(p, a5 + SCR_X, x);
                    ww(p, a5 + SCR_Y, y);
                    ww(p, a5 + SCR_W, rw(p, PROGRAM_v_GlyphWidth));
                    ww(p, a5 + SCR_H, rw(p, PROGRAM_v_GlyphHeight));
                    ww(p, a5 + SCR_SIZE + SCR_W, 0xFFFF);
                    wl(p, PROGRAM_v_RestoreNext, a5 + SCR_SIZE);
                }
                prog_draw_cel(VM, &p->blt, rl(p, PROGRAM_t_FontBank + 16), f, x, y);
                x = (uint16_t)(rw(p, PROGRAM_v_TextX) + rw(p, PROGRAM_v_GlyphWidth) - 2);
                ww(p, PROGRAM_v_TextX, x);
                if ((int16_t)x >= 320) {
                    ww(p, PROGRAM_v_TextX, rw(p, PROGRAM_v_TextLineX));
                    y = (uint16_t)(rw(p, PROGRAM_v_TextY) + rw(p, PROGRAM_v_GlyphHeight));
                    ww(p, PROGRAM_v_TextY, y);
                    if ((int16_t)y >= 200)
                        ww(p, PROGRAM_v_TextY, rw(p, PROGRAM_v_TextTopY));
                }
            }
            wl(p, PROGRAM_v_TextRecord, rl(p, rl(p, PROGRAM_v_TextRecord) + 10));
        } while (rl(p, PROGRAM_v_TextRecord));
    }
    ww(p, PROGRAM_v_BlitByCpu, 0);
}

/* LAB_03FC : PIV en a0 décodé (en place) vers les plans LAB_04D9 ;
 * palette -> LAB_0506 (comme LAB_0C21 de mog) */
static void piv_decode(ProgIntro *p, uint32_t a0)
{
    wl(p, PROGRAM_v_PivData, a0);
    uint32_t n = 32;
    ww(p, PROGRAM_v_PivPlanes, rw(p, a0));
    if (rw(p, a0) != 4)
        n = 64;
    uint32_t src = a0 + 6;
    for (uint32_t i = 0; i < n; i++)
        ix_wb(VM, PROGRAM_t_PivPalette + i, ix_rb(VM, src + i));
    src += n;
    for (uint32_t i = 0; i < n / 2; i++)
        ww(p, PROGRAM_t_PivPalette + 2 * i, moon_piv_colour(rw(p, PROGRAM_t_PivPalette + 2 * i)));
    uint32_t len = rl(p, a0 + 2);
    uint32_t k = (uint32_t)(uint16_t)(len - 1) + 1;     /* DBF sur le mot */
    for (uint32_t i = 0; i < k; i++)
        ix_wb(VM, a0 + 2 + i, ix_rb(VM, src + i));
    ww(p, PROGRAM_v_GfxReady, 1);                         /* LAB_0406 */
    prog_unpack(VM, a0 + 2, len, rl(p, PROGRAM_t_DestPlanes));
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
    fade_to(p, a0, (uint16_t)rl(p, PROGRAM_v_FadeSpeed));
    wait_vbls(p, rl(p, PROGRAM_v_FadeSpeed) << 4);
}

/* LAB_025F : fondu au noir (LAB_026D), 36 VBL */
void prog_fade_black(ProgIntro *p)
{
    fade_to(p, PROGRAM_t_PalBlack, 2);
    wait_vbls(p, 36);
}

/* LAB_0054 : écran de texte a0 sur le fond PIV rl(LAB_011A + 20) */
void prog_text_screen(ProgIntro *p, uint32_t a0)
{
    wl(p, PROGRAM_v_TextScreen, a0);
    black(p);
    clear_screen(p, rl(p, PROGRAM_v_ShowPlanes));
    set_planes(p, rl(p, PROGRAM_v_ShowPlanes));
    uint32_t src = rl(p, PROGRAM_t_FontBank + 20), dst = rl(p, PROGRAM_v_DrawPlanes);
    for (uint32_t i = 0; i < 0x10E7; i++)
        ix_wb(VM, dst + i, ix_rb(VM, src + i));
    piv_decode(p, rl(p, PROGRAM_v_DrawPlanes));
    text_records(p, rl(p, PROGRAM_v_TextScreen));
    static const uint16_t pal[5] = { 0x0800, 0x0600, 0x0400, 0x0000, 0x0200 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_t_PivPalette + 2 + 2u * (unsigned)i, pal[i]);
    fade_wait(p, PROGRAM_t_PivPalette);
}

/* ---------------------------------------------------------- démarrage */

/* LAB_0402 : image PIV `name` lue à a1 puis décodée vers les plans
 * LAB_04D9 ; palette -> LAB_0506 */
static void load_picture(ProgIntro *p, uint32_t name, uint32_t a1)
{
    MogFile f;
    wl(p, PROGRAM_v_PivData, a1);
    prog_file_open(VM, name, &f);
    prog_file_read(VM, &f, a1, 6);
    uint32_t n = 32;
    ww(p, PROGRAM_v_PivPlanes, rw(p, a1));
    if (rw(p, a1) != 4)
        n = 64;
    prog_file_read(VM, &f, PROGRAM_t_PivPalette, n);
    for (uint32_t i = 0; i < n / 2; i++)
        ww(p, PROGRAM_t_PivPalette + 2 * i, moon_piv_colour(rw(p, PROGRAM_t_PivPalette + 2 * i)));
    uint32_t len = rl(p, a1 + 2);
    prog_file_read(VM, &f, a1 + 2, len);
    prog_file_close(&f);
    ww(p, PROGRAM_v_GfxReady, 1);                         /* LAB_0406 */
    prog_unpack(VM, a1 + 2, len, rl(p, PROGRAM_t_DestPlanes));
}

/* LAB_025B : palette LAB_0506 copiée en a0 */
static void save_palette(ProgIntro *p, uint32_t a0)
{
    for (uint32_t i = 0; i < 32; i++)
        ww(p, a0 + 2 * i, rw(p, PROGRAM_t_PivPalette + 2 * i));
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
    uint16_t d0 = rw(p, PROGRAM_v_EndFlags);
    for (int k = 0; k < 4; k++)
        if (d0 & (1u << bit[k])) {
            for (int i = 0; i < 4; i++)
                ww(p, a0 + 16 + 2u * (unsigned)i, t[k][i]);
            return;
        }
}

/* Unpack_Rnc1 : décompression RNC en place à a0 (décodeur de
 * libmoon_assets). L'original écrit la sortie à rebours sous a0 + 12 +
 * taille + $100, la recopie en a0 puis met à zéro le reste jusqu'à cette
 * limite ; renvoie la taille (0 si ce n'est pas un fichier RNC). */
static uint32_t rnc_unpack(ProgIntro *p, uint32_t a0)
{
    IxVM *vm = VM;
    if (rl(p, a0) != 0x524E4301)
        return 0;
    uint32_t unpacked = rl(p, a0 + 4), packed = rl(p, a0 + 8);
    uint32_t end = a0 + 12 + unpacked + 0x100;
    if (!ix_vm_ok(vm, a0, end - a0) || !ix_vm_ok(vm, a0, 12 + packed))
        return 0;
    uint8_t *out = malloc(unpacked ? unpacked : 1);
    int n = out ? moon_rnc1_decompress(ix_vm_ptr(vm, a0), 12 + packed, out, unpacked) : -1;
    if (n > 0) {
        memcpy(ix_vm_ptr(vm, a0), out, (size_t)n);
        memset(ix_vm_ptr(vm, a0 + (uint32_t)n), 0, end - (a0 + (uint32_t)n));
    }
    free(out);
    return n > 0 ? (uint32_t)n : 0;
}

/* LAB_0325 (partie mémoire) : état de la souris, vecteurs d'interruption */
static void interrupts_init(ProgIntro *p)
{
    prog_wait_vbl(p);
    ix_wb(VM, PROGRAM_v_Joy0Dat, 0);                     /* JOY0DAT */
    ix_wb(VM, PROGRAM_v_Joy1Dat, 0);
    static const uint32_t v[6] = { PROGRAM_Irq_Level1, PROGRAM_Irq_Level2, PROGRAM_Irq_Level3,
                                   PROGRAM_Irq_Level4, PROGRAM_Irq_Level5, PROGRAM_Irq_Level6 };
    for (uint32_t i = 0; i < 6; i++)
        wl(p, 0x64 + 4 * i, v[i]);
}

/* SECSTRT_29 : mémoire chip à zéro, copper list (sprites vides), écrans */
static void display_init(ProgIntro *p)
{
    for (uint32_t a = 0x6BEFA; a != 0x80000; a += 2)    /* EXT_000f */
        ww(p, a, 0);
    uint32_t a0 = 0x7F6AE, a1 = rl(p, PROGRAM_v_CopperList);
    uint32_t d1 = PROGRAM_v_DisplayInit, d2 = d1 >> 16;
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
    uint32_t s = rl(p, PROGRAM_v_ShowPlanes);
    for (uint32_t i = 0; i < 5; i++) {
        ww(p, COPPER_BPL + 8 * i, (uint16_t)((s + i * PLANE) >> 16));
        ww(p, COPPER_BPL + 8 * i + 4, (uint16_t)(s + i * PLANE));
    }
    clear_screen(p, rl(p, PROGRAM_v_DrawPlanes));
    clear_screen(p, rl(p, PROGRAM_v_ShowPlanes));
    prog_wait_vbl(p);
    set_palette(p, PROGRAM_t_PalDisplay);
}

/* LAB_0044 : découpage des blocs chip (LAB_00C2) et fast (LAB_00C4) */
static void buffers_init(ProgIntro *p)
{
    uint32_t d0 = rl(p, PROGRAM_v_ChipFree);
    wl(p, PROGRAM_v_ChipFree, d0 + 0x4536C);
    wl(p, PROGRAM_t_TextBackgrounds, d0);
    wl(p, PROGRAM_v_BgPlanes, d0);
    wl(p, PROGRAM_v_BgSceneA, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_t_TextBackgrounds + 4, d0);
    wl(p, PROGRAM_v_BgSceneB, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_t_TextBackgrounds + 8, d0);
    wl(p, PROGRAM_v_BgSceneC, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_t_TextBackgrounds + 8, d0);
    wl(p, PROGRAM_v_BgSceneD, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_b_Music, d0);
    d0 = rl(p, PROGRAM_v_FastFree);
    wl(p, PROGRAM_v_FastFree, d0 + 0x58116);
    wl(p, PROGRAM_v_CelArea, d0);
    d0 += 0x7530;
    wl(p, PROGRAM_t_FontBank + 0, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_t_FontBank + 4, d0);
    d0 += 0x9C40;
    wl(p, PROGRAM_t_FontBank + 20, d0);
    d0 += 0x10E6;
    wl(p, PROGRAM_t_FontBank + 16, d0);
    d0 += 0x5F50;
    static const uint32_t bg[5] = { PROGRAM_b_PivBg4, PROGRAM_b_PivBg5a, PROGRAM_b_PivBg3,
                                    PROGRAM_b_PivBg2, PROGRAM_b_PivBg2a };
    for (int i = 0; i < 5; i++, d0 += 0x9C40)
        wl(p, bg[i], d0);
}

/* SECSTRT_10 : planches LAB_0281, commandes de script LAB_0288, listes */
static void engine_init(ProgIntro *p)
{
    for (uint32_t i = 0; i < 7; i++)
        wl(p, PROGRAM_t_EntSheets + 4 * i, PROGRAM_t_Banks);
    static const struct { uint32_t off, fn; } op[] = {
        { 0, PROGRAM_IxOp80_SetDir }, { 4, PROGRAM_IxOp84_Jump }, { 8, PROGRAM_IxOp88_Hold },
        { 12, PROGRAM_IxOp8C_Skip }, { 20, PROGRAM_IxOp94_Loop }, { 24, PROGRAM_IxOp98_Nop },
        { 28, PROGRAM_IxOp98_Nop }, { 36, PROGRAM_IxOpA4_Sound }, { 32, PROGRAM_IxOpA0_Move },
        { 48, PROGRAM_IxOpB0_Nop }, { 44, PROGRAM_IxOpAC_Nop }, { 52, PROGRAM_IxOpB4_Call },
        { 72, PROGRAM_IxOpC8_Skip6 }, { 56, PROGRAM_IxOpB8_Skip6 }, { 60, PROGRAM_IxOpBC_Skip6 },
        { 64, PROGRAM_IxOpC0_End }, { 68, PROGRAM_IxOpC4_SetBank }, { 76, PROGRAM_IxOpCC_IfZero },
        { 80, PROGRAM_IxOpD0_IfNonZero }, { 84, PROGRAM_IxOpD4_Reset }, { 40, PROGRAM_IxOpA8_Nop },
    };
    for (unsigned i = 0; i < sizeof op / sizeof op[0]; i++)
        wl(p, PROGRAM_t_IxOpcodes + op[i].off, op[i].fn);
    wl(p, PROGRAM_v_RestoreBack, PROGRAM_b_RestoreA);
    wl(p, PROGRAM_v_RestoreFront, PROGRAM_b_RestoreB);
}

/* SECSTRT_25 (suite) : tables de conversion de pixels LAB_04E4 (LAB_0507),
 * LAB_04EC (LAB_051D), LAB_04EE (LAB_051E) et masques LAB_046F */
static void gfx_tables(ProgIntro *p)
{
    IxVM *vm = VM;
    uint32_t a1 = rl(p, PROGRAM_v_GfxTableA);
    for (uint32_t k = 0; k < 8; k++)
        wl(p, PROGRAM_t_GfxTablesA + 4 * k, a1 + k * 0x200);
    uint32_t a0 = PROGRAM_t_GfxSeedA;
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
    a0 = PROGRAM_t_GfxSeedB;                              /* LAB_04EC */
    a1 = rl(p, PROGRAM_v_GfxTableB);
    for (;; a0 += 4) {
        uint16_t d0 = rw(p, a0);
        if (d0 == 0x63)
            break;
        ww(p, a1 + (uint32_t)(int32_t)(int16_t)(uint16_t)(d0 + d0), rw(p, a0 + 2));
    }
    a1 = rl(p, PROGRAM_v_GfxTableC);                       /* LAB_04EE */
    for (uint32_t k = 0; k < 8; k++)
        wl(p, PROGRAM_t_GfxTablesB + 4 * k, a1 + k * 0x200);
    a0 = PROGRAM_t_GfxSeedC;
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
    a0 = PROGRAM_t_GfxSeedD;                              /* LAB_046F */
    for (uint16_t d1 = 0xF0; (int16_t)d1 <= 0xFF; d1 = (uint16_t)(d1 + 0xF))
        for (int d0 = 7; d0 >= 0; d0--) {
            uint16_t d2 = (uint16_t)((d1 >> d0) | (d1 << ((16 - d0) & 15)));
            ix_wb(vm, a0++, (uint8_t)d2);
            ix_wb(vm, a0++, (uint8_t)(d2 >> 8));
        }
    a0 = PROGRAM_t_GfxSeedE;
    for (int d0 = 7; d0 >= 0; d0--, a0 += 4)
        wl(p, a0, d0 ? (0xFFFF0000u >> d0) | (0xFFFF0000u << (32 - d0)) : 0xFFFF0000u);
}

/* SECSTRT_0 jusqu'à l'intro : écran, graphismes, tampons, moteur,
 * palette LAB_0274 (serveur LAB_057D), police et fond de texte (LAB_0051) */
void prog_boot(ProgIntro *p, uint32_t chip, uint32_t chip_size, uint32_t fast, uint32_t fast_size)
{
    wl(p, PROGRAM_v_ChipFree, chip);
    wl(p, PROGRAM_v_ChipSize, chip_size);
    wl(p, PROGRAM_v_FastFree, fast);
    wl(p, PROGRAM_v_FastSize, fast_size);
    ww(p, PROGRAM_v_EndFlags, rw(p, 0x3E0));              /* EXT_0007 : drapeaux de mog */
    display_init(p);                                    /* SECSTRT_29 */
    prog_boot_graphics(VM);                             /* SECSTRT_25 */
    gfx_tables(p);
    ww(p, PROGRAM_v_CelPlanesMax, 4);                         /* LAB_0006 : SECSTRT_23 */
    for (uint32_t i = 0; i < 256; i++) {                /*   LAB_04B0 */
        uint8_t b = 0;
        for (int k = 0; k < 8; k++)
            if (i & (1u << k))
                b |= (uint8_t)(0x80 >> k);
        ix_wb(VM, PROGRAM_t_BitReverse + i, b);
    }
    ww(p, PROGRAM_v_ClipWidth, 40);                        /*   LAB_04A7 */
    ww(p, PROGRAM_v_ClipHeight, 200);
    ww(p, PROGRAM_v_CelDestOffset, 0);
    swap_screens(p);
    buffers_init(p);                                    /* LAB_0044 */
    engine_init(p);                                     /* SECSTRT_10 */
    wl(p, PROGRAM_v_PalCurrent, PROGRAM_t_PalCurrent);          /* SECSTRT_31 */
    uint32_t a = PROGRAM_t_VblServers;
    while (rl(p, a))
        a += 4;
    wl(p, a, PROGRAM_Vbl_Screen);
    wl(p, PROGRAM_t_Controllers, PROGRAM_Ctl_ScriptEnd);
    prog_load_cel(VM, PROGRAM_s_BoldF, rl(p, PROGRAM_t_FontBank + 16));  /* LAB_0051 */
    MogFile f;
    prog_file_open(VM, PROGRAM_s_MessagePiv, &f);
    prog_file_read(VM, &f, rl(p, PROGRAM_t_FontBank + 20), 0x10E6);
    prog_file_close(&f);
}

/* ------------------------------------------------ chargement, générique */

/* LAB_05A3 / LAB_05A4 : touche (SECSTRT_16) -> intro sautée (LAB_05E7) ;
 * fondu vers LAB_01CB, zones de texte restaurées */
static void credit_end(ProgIntro *p)
{
    if (rw(p, PROGRAM_v_KeyPressed))
        ww(p, PROGRAM_v_IntroSkipped, 1);
    wl(p, PROGRAM_v_FadeSpeed, 2);
    static const uint16_t c[5] = { 0x0000, 0x0FED, 0x0DC9, 0x0B95, 0x0842 };
    static const uint8_t o[5] = { 10, 18, 20, 22, 24 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_t_PalForest + o[i], c[i]);
    fade_to(p, PROGRAM_t_PalForest, 2);
    restore_areas(p);
}

static void credit_colours(ProgIntro *p, uint16_t v)
{
    static const uint8_t o[5] = { 10, 18, 20, 22, 24 };
    for (int i = 0; i < 5; i++)
        ww(p, PROGRAM_t_PalForest + o[i], v);
}

/* LAB_059E : tuiles du haut du défilement (lune) comme fond */
static void credits_start(ProgIntro *p)
{
    for (uint32_t i = 0; i < 0x80; i++)                 /* LAB_035E */
        ix_wb(VM, PROGRAM_t_KeysDown + i, 0);
    ww(p, PROGRAM_v_KeyPressed, 0);
    prog_fade_black(p);
    fill_lists(p);
    swap_screens(p);
    clear_screen(p, rl(p, PROGRAM_v_ShowPlanes));
    clear_screen(p, rl(p, PROGRAM_v_DrawPlanes));
    clear_screen(p, rl(p, PROGRAM_v_BgPlanes));
    wl(p, PROGRAM_v_TileScreen, rl(p, PROGRAM_v_BgPlanes));
    wl(p, PROGRAM_t_TileBanks, rl(p, PROGRAM_v_BgSceneB));
    wl(p, PROGRAM_t_TileBanks + 4, rl(p, PROGRAM_v_BgSceneC));
    wl(p, PROGRAM_t_TileBanks + 8, rl(p, PROGRAM_v_BgSceneD));
    ww(p, PROGRAM_v_ScrollPos, 0);
    ww(p, PROGRAM_v_ScrollTopRow, 0);
    ww(p, PROGRAM_v_ScrollRows, 8);
    draw_rows(p);
    show_background(p);
    swap_screens(p);                                    /* LAB_05A2 */
    credit_end(p);
}

/* LAB_059F : logo (frame $49 de la police) */
static void credits_logo(ProgIntro *p)
{
    credit_colours(p, 0);
    fade_to(p, PROGRAM_t_PalForest, 0);
    set_planes(p, rl(p, PROGRAM_v_DrawPlanes));
    ww(p, PROGRAM_v_BlitByCpu, 1);
    prog_draw_cel(VM, &p->blt, rl(p, PROGRAM_t_FontBank + 16), 0x49, 9, 0x3C);
    ww(p, PROGRAM_v_BlitByCpu, 0);
    swap_screens(p);
    wait_vbls(p, 8);
    credit_end(p);
}

/* LAB_05A1 : texte suivant du générique (LAB_05B1[LAB_05B0], 6 textes) */
static void credits_text(ProgIntro *p)
{
    credit_colours(p, 0x0FFF);
    wl(p, PROGRAM_v_FadeSpeed, 1);
    fade_wait(p, PROGRAM_t_PalForest);
    set_planes(p, rl(p, PROGRAM_v_DrawPlanes));
    uint16_t n = rw(p, PROGRAM_v_CreditIndex);
    if ((int16_t)n < 6) {
        text_records(p, rl(p, PROGRAM_t_CreditTexts + (uint32_t)(uint16_t)(n << 2)));
        ww(p, PROGRAM_v_CreditIndex, (uint16_t)(n + 1));
        credit_colours(p, 0);
        fade_to(p, PROGRAM_t_PalForest, 0);
    }
    swap_screens(p);
    credit_end(p);
}

/* LAB_05A0 : éclair blanc, fond remis, puis texte */
static void credits_flash(ProgIntro *p)
{
    credit_colours(p, 0x0FFF);
    wl(p, PROGRAM_v_FadeSpeed, 1);
    fade_wait(p, PROGRAM_t_PalForest);
    wait_vbls(p, 2);
    credit_colours(p, 0);
    fade_to(p, PROGRAM_t_PalForest, 0);
    swap_screens(p);
    show_background(p);
    credits_text(p);
}

static int skipped(ProgIntro *p) { return rw(p, PROGRAM_v_IntroSkipped) != 0; }

/* LAB_0185 : chargement des décors, CEL et musique, générique par-dessus.
 * Renvoie 1 si l'intro est sautée (LAB_05E7). */
int prog_loading(ProgIntro *p)
{
    black(p);
    set_planes(p, rl(p, PROGRAM_v_ShowPlanes));
    load_picture(p, PROGRAM_s_Mindscape, rl(p, PROGRAM_v_BgSceneC));     /* « mindscape » */
    fade_wait(p, PROGRAM_t_PivPalette);
    set_planes(p, rl(p, PROGRAM_v_BgSceneB));
    load_picture(p, PROGRAM_s_Bg1aPiv, rl(p, PROGRAM_v_FastFree));
    save_palette(p, PROGRAM_t_PalForest);
    MogFile f;
    prog_file_open(VM, PROGRAM_s_IntroStile, &f);                      /* intro.stile */
    prog_file_read(VM, &f, PROGRAM_b_TileMap, 1000);
    prog_file_close(&f);
    credits_start(p);
    set_planes(p, rl(p, PROGRAM_v_BgSceneC));
    load_picture(p, PROGRAM_s_Bg1cPiv, rl(p, PROGRAM_v_FastFree));
    credits_logo(p);
    if (skipped(p))
        return 1;
    set_planes(p, rl(p, PROGRAM_v_BgSceneD));
    load_picture(p, PROGRAM_s_Bg1bPiv, rl(p, PROGRAM_v_FastFree));
    credits_flash(p);
    if (skipped(p))
        return 1;
    static const struct { uint32_t screen, name, pal; int tint; } bg[5] = {
        { PROGRAM_b_PivBg4, PROGRAM_s_Bg4Piv, PROGRAM_t_PalBg4, 0 },
        { PROGRAM_b_PivBg5a, PROGRAM_s_Bg5aPiv, PROGRAM_t_PalBg5a, 1 },
        { PROGRAM_b_PivBg3, PROGRAM_s_Bg3Piv, PROGRAM_t_PalBg3, 1 },
        { PROGRAM_b_PivBg2, PROGRAM_s_Bg2Piv, PROGRAM_t_PalBg2, 1 },
        { PROGRAM_b_PivBg2a, PROGRAM_s_Bg2aPiv, PROGRAM_t_PalBg2a, 1 },
    };
    for (int i = 0; i < 5; i++) {
        set_planes(p, rl(p, bg[i].screen));
        load_picture(p, bg[i].name, rl(p, PROGRAM_v_FastFree));
        save_palette(p, bg[i].pal);
        if (bg[i].tint)
            tint(p, bg[i].pal);
        credits_text(p);
        if (skipped(p))
            return 1;
    }
    /* planches de CEL LAB_0276 : 0, 1, 4, 2 à la suite dans le bloc chip */
    static const struct { uint32_t name; uint32_t slot; } cel[4] = {
        { PROGRAM_s_Au1Cel, 0 }, { PROGRAM_s_Li1Cel, 4 },
        { PROGRAM_s_Da1Cel, 16 }, { PROGRAM_s_Ha1Cel, 8 },
    };
    uint32_t a1 = rl(p, PROGRAM_v_ChipFree);
    for (int i = 0; i < 4; i++) {
        wl(p, PROGRAM_t_Banks + cel[i].slot, a1);
        prog_load_cel(VM, cel[i].name, a1);
        a1 = rl(p, PROGRAM_t_Banks + cel[i].slot) + prog_cel_size(VM, cel[i].name);
        credits_text(p);
        if (skipped(p))
            return 1;
    }
    a1 = rl(p, PROGRAM_v_CelArea);
    wl(p, PROGRAM_t_Banks + 12, a1);
    prog_load_cel(VM, PROGRAM_s_Dw1Cel, a1);
    prog_cel_size(VM, PROGRAM_s_Dw1Cel);
    a1 = rl(p, PROGRAM_b_PivBg2a) + 0x9C40;
    wl(p, PROGRAM_v_OverlayCel, a1);
    prog_load_cel(VM, PROGRAM_s_Ov1Cel, a1);
    prog_file_open(VM, PROGRAM_s_MusicCmp, &f);                       /* music.cmp */
    prog_file_read(VM, &f, rl(p, PROGRAM_b_Music), 0x159D9);
    prog_file_close(&f);
    rnc_unpack(p, rl(p, PROGRAM_b_Music));
    return 0;
}

/* ------------------------------------------------------------------ fin */

/* LAB_01B9 : couleurs de la fin (LAB_01D2, SECSTRT_9...) selon le lieu de
 * la victoire (LAB_0005 bits 1, 0, 2) */
static void ending_colours(ProgIntro *p)
{
    static const uint16_t c[3][6] = {
        { 0x0F80, 0x0C50, 0x0920, 0x0C50, 0x0920, 0x0700 },
        { 0x0000, 0x0222, 0x0444, 0x0111, 0x0333, 0x0555 },
        { 0x0B40, 0x0D60, 0x0F80, 0x0D60, 0x0F80, 0x0FA0 },
    };
    static const uint8_t bit[3] = { 1, 0, 2 };
    uint16_t d0 = rw(p, PROGRAM_v_EndFlags);
    for (int k = 0; k < 3; k++)
        if (d0 & (1u << bit[k])) {
            ww(p, PROGRAM_t_PalBg2a + 24, c[k][0]);
            ww(p, PROGRAM_t_PalBg2a + 30, c[k][1]);
            ww(p, PROGRAM_t_PalBg2a + 46, c[k][2]);
            ww(p, PROGRAM_v_EndColourA, c[k][3]);
            ww(p, PROGRAM_v_EndColourB, c[k][4]);
            ww(p, PROGRAM_v_EndColourC, c[k][5]);
            return;
        }
}

/* LAB_018E : décors, CEL et musique de la fin (sans générique) */
static void ending_loading(ProgIntro *p)
{
    static const struct { uint32_t screen, name, pal; int tint; } bg[4] = {
        { PROGRAM_b_PivBg4, PROGRAM_s_Bg5Piv, PROGRAM_t_PalBg4, 1 },
        { PROGRAM_b_PivBg5a, PROGRAM_s_Bg5aPiv, PROGRAM_t_PalBg5a, 1 },
        { PROGRAM_b_PivBg3, PROGRAM_s_Bg3Piv, PROGRAM_t_PalBg3, 1 },
        { PROGRAM_b_PivBg2a, PROGRAM_s_Bg2aPiv, PROGRAM_t_PalBg2a, 2 },
    };
    for (int i = 0; i < 4; i++) {
        set_planes(p, rl(p, bg[i].screen));
        load_picture(p, bg[i].name, rl(p, PROGRAM_v_FastFree));
        save_palette(p, bg[i].pal);
        if (bg[i].tint == 2)
            ending_colours(p);
        tint(p, bg[i].pal);
    }
    clear_screen(p, rl(p, PROGRAM_v_BgSceneB));
    set_planes(p, rl(p, PROGRAM_v_BgSceneB));
    load_picture(p, PROGRAM_s_Bg7Piv, rl(p, PROGRAM_v_FastFree));
    save_palette(p, PROGRAM_t_PalForest);
    set_planes(p, rl(p, PROGRAM_v_BgSceneC));
    load_picture(p, PROGRAM_s_Bg8Piv, rl(p, PROGRAM_v_FastFree));
    save_palette(p, PROGRAM_t_PalDruid);
    /* planches LAB_0276 : 0, 1, 3, 2, 4 à la suite dans le bloc chip */
    static const struct { uint32_t name, slot; } cel[5] = {
        { PROGRAM_s_Dg1Cel, 0 }, { PROGRAM_s_Li1Cel, 4 }, { PROGRAM_s_Ha1Cel, 12 },
        { PROGRAM_s_Co1Cel, 8 }, { PROGRAM_s_Da1Cel, 16 },
    };
    uint32_t a1 = rl(p, PROGRAM_v_ChipFree);
    for (int i = 0; i < 5; i++) {
        wl(p, PROGRAM_t_Banks + cel[i].slot, a1);
        prog_load_cel(VM, cel[i].name, a1);
        a1 = rl(p, PROGRAM_t_Banks + cel[i].slot) + prog_cel_size(VM, cel[i].name);
    }
    a1 = rl(p, PROGRAM_v_BgSceneD);
    wl(p, PROGRAM_t_Banks + 20, a1);
    prog_load_cel(VM, PROGRAM_s_Klift1Cel, a1);
    prog_cel_size(VM, PROGRAM_s_Klift1Cel);
    a1 = rl(p, PROGRAM_b_PivBg2);
    wl(p, PROGRAM_v_OverlayCel, a1);
    prog_load_cel(VM, PROGRAM_s_Ov1Cel, a1);
    MogFile f;
    prog_file_open(VM, PROGRAM_s_CoStile, &f);          /* co.stile */
    prog_file_read(VM, &f, PROGRAM_b_TileMap, 1000);
    prog_file_close(&f);
    prog_file_open(VM, PROGRAM_s_VmusicCmp, &f);           /* vmusic.cmp */
    prog_file_read(VM, &f, rl(p, PROGRAM_b_Music), 0xEFA0);
    prog_file_close(&f);
    rnc_unpack(p, rl(p, PROGRAM_b_Music));
}

/* Début d'une scène de la fin : décor `bg` (copié ou non), palette `pal`
 * (LAB_011E = mode) */
static void ending_scene(ProgIntro *p, uint32_t bg, uint32_t pal, uint16_t mode)
{
    ent_reset(p);
    black(p);
    copy_long(p, rl(p, bg), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, pal);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, mode);
}

/* LAB_0036 : retour à Stonehenge, puis le cercle des druides */
static void ending_0036(ProgIntro *p)
{
    static const uint32_t c[5] = { PROGRAM_x_CircleDruidDC, PROGRAM_x_CircleDruidDD, PROGRAM_x_CircleDruidDB,
                                   PROGRAM_x_CircleDruidDF, PROGRAM_x_CircleDruidE1 };
    ww(p, PROGRAM_v_SceneOverlay, 1);
    ww(p, PROGRAM_v_OverlayMode, 2);
    ent_reset(p);
    prog_fade_black(p);
    swap_screens(p);
    copy_long(p, rl(p, PROGRAM_b_PivBg2a), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalBg2a);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 2);
    wl(p, PROGRAM_v_FrameVbls, 6);
    spawn_left(p, PROGRAM_x_EndingStonehenge);
    prog_music_start(p);                                /* SECSTRT_1 */
    run_until_end(p);
    wl(p, rl(p, PROGRAM_v_EndGlowA), 0);                  /* LAB_0579 */
    wl(p, rl(p, PROGRAM_v_EndGlowB), 0);
    wl(p, rl(p, PROGRAM_v_EndGlowC), 0);
    prog_fade_black(p);
    ww(p, PROGRAM_v_SceneOverlay, 0);
    ww(p, PROGRAM_v_OverlayMode, 0);
    wl(p, PROGRAM_v_FrameVblsExtra, 4);
    ent_reset(p);
    swap_screens(p);
    copy_long(p, rl(p, PROGRAM_b_PivBg3), rl(p, PROGRAM_v_BgPlanes));
    show_background(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalBg3);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 2);
    wl(p, PROGRAM_v_FrameVbls, 8);
    circle(p, c, 5);                                    /* LAB_0031 */
    spawn_left(p, PROGRAM_x_EndingCircle);
    run_until_end(p);
    wl(p, PROGRAM_v_FrameVbls, 6);
}

/* LAB_0037 : la procession (LAB_003A), puis deux personnages */
static void ending_0037(ProgIntro *p)
{
    wl(p, PROGRAM_v_FrameVblsExtra, 0);
    ending_scene(p, PROGRAM_b_PivBg4, PROGRAM_t_PalBg4, 4);
    spawn_left(p, PROGRAM_x_EndingCharB);
    spawn_left(p, PROGRAM_x_EndingCharC);
    ww(p, PROGRAM_v_ProcessionIndex, 0);
    wl(p, PROGRAM_t_Procession, PROGRAM_t_ProcessionEnding);
    ww(p, PROGRAM_v_ProcessionCount, 5);
    ww(p, PROGRAM_v_ProcessionGap, 0);
    wl(p, PROGRAM_v_FrameVbls, 6);
    ww(p, PROGRAM_v_LeftDepth, 5);
    ww(p, PROGRAM_v_RightDepth, 15);
    procession2(p);                                     /* LAB_001F */
    for (int i = 0; i < 41; i++) {                      /* LAB_0038 */
        frame_mark(p);
        ent_controllers(p);
        ent_render(p);
        swap_screens(p);
        restore_areas(p);
        frame_wait(p);
    }
    ww(p, PROGRAM_v_LeftDepth, 0);
    ww(p, PROGRAM_v_RightDepth, 0);
    wl(p, PROGRAM_v_FrameVbls, 8);
    ent_reset(p);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    spawn_left(p, PROGRAM_x_EndingCharB);
    spawn_left(p, PROGRAM_x_EndingCharA);
    run_until_end(p);
}

/* LAB_0039 : trois plans (LAB_00D5, LAB_00E9 + LAB_00EB, LAB_00EC) */
static void ending_0039(ProgIntro *p)
{
    ending_scene(p, PROGRAM_b_PivBg5a, PROGRAM_t_PalBg5a, 4);
    spawn_left(p, PROGRAM_x_EndingA);
    run_until_end(p);
    ending_scene(p, PROGRAM_b_PivBg4, PROGRAM_t_PalBg4, 4);
    spawn_left(p, PROGRAM_x_EndingCharB);
    spawn_left(p, PROGRAM_x_EndingB);
    run_until_end(p);
    ending_scene(p, PROGRAM_b_PivBg5a, PROGRAM_t_PalBg5a, 4);
    spawn_left(p, PROGRAM_x_EndingC);
    run_until_end(p);
    clear_screen(p, rl(p, PROGRAM_v_BgPlanes));
    clear_screen(p, rl(p, PROGRAM_v_DrawPlanes));
}

/* LAB_05AB : défilement de tuiles (carte co.stile) préparé en bas */
static void ending_scroll_start(ProgIntro *p)
{
    fill_lists(p);
    swap_screens(p);
    clear_screen(p, rl(p, PROGRAM_v_ShowPlanes));
    wl(p, PROGRAM_v_TileScreen, rl(p, PROGRAM_v_BgPlanes));
    wl(p, PROGRAM_t_TileBanks, rl(p, PROGRAM_v_BgSceneB));
    wl(p, PROGRAM_t_TileBanks + 4, rl(p, PROGRAM_v_BgSceneC));
    wl(p, PROGRAM_t_TileBanks + 8, rl(p, PROGRAM_v_BgSceneD));
    wl(p, PROGRAM_v_FrameVbls, 2);
    wl(p, PROGRAM_v_FrameVblsExtra, 0);
    ww(p, PROGRAM_v_ScrollPos, 1000);
    ww(p, PROGRAM_v_ScrollSpeed, 2);
    ww(p, PROGRAM_v_ScrollTopRow, 0);
    ww(p, PROGRAM_v_ScrollRows, 8);
    ww(p, PROGRAM_v_ScrollDone, 0);
    ww(p, PROGRAM_v_ScrollSpeed, 9);
    draw_rows(p);
    tiles_to_draw(p);
    swap_screens(p);
}

/* LAB_05AC : la vue remonte (script : LAB_05AF ralentit, LAB_05AE
 * arrête) jusqu'à LAB_05B8 < 200 */
static void ending_scroll(ProgIntro *p)
{
    while (!rw(p, PROGRAM_v_ScrollDone)) {
        frame_mark(p);
        scroll(p, 8);
        ent_controllers(p);
        ent_render(p);
        swap_screens(p);
        frame_wait(p);
        if ((int16_t)rw(p, PROGRAM_v_ScrollPos) < 200)
            break;
    }
    copy_screen(p, rl(p, PROGRAM_v_ShowPlanes), rl(p, PROGRAM_v_DrawPlanes));
    copy_screen(p, rl(p, PROGRAM_v_ShowPlanes), rl(p, PROGRAM_v_BgPlanes));
}

/* LAB_003B : la montée vers la lune, puis le dernier écran (LAB_00B0) */
static void ending_003b(ProgIntro *p)
{
    black(p);
    ent_reset(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneA));
    ending_scroll_start(p);
    wl(p, PROGRAM_v_ScenePalette, PROGRAM_t_PalForest);
    set_palette(p, PROGRAM_t_PalForest);                   /* LAB_0010 */
    copy_palette(p, PROGRAM_t_PalForest);
    wait_vbls(p, 15);
    ww(p, PROGRAM_v_ScriptEnded, 0);
    ww(p, PROGRAM_v_ScenePaletteDone, 0);
    ww(p, PROGRAM_v_ScenePaletteMode, 4);
    wl(p, PROGRAM_v_FrameVblsExtra, 0);
    wl(p, PROGRAM_v_FrameVbls, 6);
    spawn_left(p, PROGRAM_x_EndingMoonRise);
    ending_scroll(p);
    run_until_end(p);
    prog_fade_black(p);
    ent_reset(p);
    wl(p, PROGRAM_v_BgPlanes, rl(p, PROGRAM_v_BgSceneC));
    show_background(p);
    fade_wait(p, PROGRAM_t_PalDruid);
    wait_vbls(p, 100);
    set_planes(p, rl(p, PROGRAM_v_DrawPlanes));
    text_records(p, PROGRAM_t_LastScreenText);
    swap_screens(p);
    wait_vbls(p, 500);
    fade_to(p, PROGRAM_t_PalBlack, 6);
    wait_vbls(p, 90);
}

/* LAB_0001 de SECSTRT_0 (EXT_0007 bit 7 : mog a été gagné) : la fin,
 * « The End », puis mog de nouveau */
void prog_ending(ProgIntro *p)
{
    prog_text_screen(p, PROGRAM_t_EndingText);
    ending_loading(p);
    wl(p, PROGRAM_v_FrameVblsExtra, 2);
    ending_0036(p);
    ending_0037(p);
    ending_0039(p);
    ending_003b(p);
    prog_text_screen(p, PROGRAM_t_TheEndText);              /* « The End » */
    wait_vbls(p, 25);
    prog_fade_black(p);
}

/* SECSTRT_0 après le démarrage : l'intro entière, jusqu'au chargement de
 * mog (LAB_0000). LAB_005B : serveur de la musique retiré. */
void prog_intro(ProgIntro *p)
{
    if (rw(p, PROGRAM_v_EndFlags) & 0x80) {               /* partie gagnée */
        prog_ending(p);
        return;
    }
    wl(p, PROGRAM_v_MusicServer, 0);
    wl(p, PROGRAM_v_FrameVblsExtra, 0);
    wl(p, PROGRAM_v_FrameVbls, 2);
    prog_fade_black(p);
    if (prog_loading(p) || skipped(p))
        return;
    prog_scene_05a5(p);
    wl(p, PROGRAM_v_FrameVblsExtra, 4);
    prog_scene_001b(p);
    prog_scene_001c(p);
    prog_scene_0174(p);
    prog_scene_001a(p);
    prog_scene_002c(p);
    prog_scene_002d(p);
    prog_scene_002f(p);
    prog_scene_002e(p);
    prog_fade_black(p);
    prog_text_screen(p, PROGRAM_t_IntroText);
    wait_vbls(p, 420);
    prog_fade_black(p);
    wl(p, rl(p, PROGRAM_v_MusicServer), 0);                  /* LAB_005B */
}

/* Mémoire de program comme la donne le lanceur (même disposition que
 * tools/prog_ref.py) : hunks relogés, bloc fast à la suite, pile. */
int prog_boot_memory(IxVM *vm, uint32_t *fast)
{
    uint32_t end = IX_VM_BASE + ix_program_image.total_size;
    *fast = (end + 0xFFF) & ~0xFFFu;
    uint32_t top = ((*fast + PROG_FAST_SIZE + 0xFFF) & ~0xFFFu) + 0x4000 + 0x1000;
    if (ix_vm_load_at(vm, &ix_program_image, 0, top - end) < 0)
        return -1;
    vm->heap = top;
    return 0;
}
