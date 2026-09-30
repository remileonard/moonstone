/*
 * mog_encounter.c — préparation d'un combat de mog : nouvelle partie
 * (chevaliers, tables), décor (PIV, terrain .t), chargement des créatures,
 * routines de t_CreatureInit, palette. Traduit de amiga_asm/mog.asm.
 *
 * Tout se passe dans la mémoire de mog, écrans compris (plans de bits de
 * LAB_05C0, LAB_05C1, LAB_0D92, SECSTRT_35) : l'hôte n'a qu'à les lire.
 * Les fondus de palette sont menés par mog_vbl.c ; les attentes sont omises.
 */
#include "mog_private.h"
#include "mog_boot.h"
#include "mog_encounter.h"
#include "mog_text.h"
#include "ix_mog_names.h"
#include "mog_struct.h"
#include "mog_sound.h"
#include "moon_assets.h"

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
void mog_fade_to(MogCombat *m, uint32_t a)
{
    wl(m, MOG_v_PalFadeTarget, a);
    ww(m, MOG_v_PalFadeDelay, 2);
    ww(m, MOG_v_PalFadeCount, 2);
    mog_wait_vbls(m, 0x24);
    if (!m->palette)
        return;
    uint16_t c[32];
    for (int i = 0; i < 32; i++)
        c[i] = rw(m, a + 2u * (unsigned)i);
    m->palette(m->out.user, c);
}

/* LAB_03F0 : fondu au noir (LAB_08D8), sans toucher au son */
void mog_fade_black(MogCombat *m)
{
    mog_fade_to(m, MOG_t_PalBlack);
}

/* LAB_03F1 : fondu au noir (LAB_08D8, volume baissé : LAB_0FC4), puis
 * LAB_0AA9 (voies libérées, son $A7 sur chacune) */
void mog_fade_out(MogCombat *m)
{
    wl(m, MOG_v_PalFadeTarget, MOG_t_PalBlack);
    ww(m, MOG_v_PalFadeDelay, 2);
    ww(m, MOG_v_PalFadeCount, 2);
    ww(m, MOG_v_SoundFading, 1);
    mog_wait_vbls(m, 0x24);
    ww(m, MOG_v_SoundFading, 0);
    ww(m, MOG_v_SndMuted, 0);
    for (int i = 0; i < 4; i++)
        mog_sound(m, 0xA7);
}


/* ------------------------------------------------------------------ */
/* Écrans                                                              */
/* ------------------------------------------------------------------ */

/* L00_0908E : plans de destination du décodage (LAB_0CFF-LAB_0D03) */
void mog_set_planes(MogCombat *m, uint32_t d0)
{
    static const uint32_t v[5] = { MOG_t_DestPlanes, MOG_v_DestPlane1, MOG_v_DestPlane2,
                                   MOG_v_DestPlane3, MOG_v_DestPlane4 };
    for (int i = 0; i < 5; i++, d0 += PLANE)
        wl(m, v[i], d0);
}

/* LAB_0D72 : écran (5 plans) à zéro */
void mog_clear_screen(MogCombat *m, uint32_t a0)
{
    for (uint32_t i = 0; i < 5 * PLANE; i++)
        wb(m, a0 + i, 0);
}

/* LAB_0419 : copie d'écran (blitter, 5 plans) */
static void copy_screen(MogCombat *m, uint32_t a0, uint32_t a1)
{
    wl(m, MOG_v_CopySrc, a0 + 5 * PLANE);
    wl(m, MOG_v_CopyDst, a1 + 5 * PLANE);
    copy(m, a1, a0, 5 * PLANE);
}

static void show_background(MogCombat *m);

/* LAB_0C21 : décodage du PIV en a0 (en place) vers les plans LAB_0CFF ;
 * palette -> LAB_0D2B. En-tête : mot plans (4 ou 5), long taille
 * compressée, 16 ou 32 couleurs. */
void mog_piv_decode(MogCombat *m, uint32_t a0)
{
    wl(m, MOG_v_PivData, a0);
    uint32_t n = 32;
    ww(m, MOG_v_PivPlanes, rw(m, a0));
    if (rw(m, a0) != 4)
        n = 64;
    uint32_t src = a0 + 6;
    copy(m, MOG_t_PivPalette, src, n);
    src += n;
    for (uint32_t i = 0; i < n / 2; i++)
        ww(m, MOG_t_PivPalette + 2 * i, moon_piv_colour(rw(m, MOG_t_PivPalette + 2 * i)));
    uint32_t len = rl(m, a0 + 2);
    uint32_t k = (uint32_t)(uint16_t)(len - 1) + 1;     /* DBF sur le mot */
    for (uint32_t i = 0; i < k; i++)
        wb(m, a0 + 2 + i, rb(m, src + i));
    ww(m, MOG_v_GfxReady, 1);
    mog_unpack(VM, a0 + 2, len, rl(m, MOG_t_DestPlanes));
}

/* Copie du PIV n° off de LAB_05B9 dans le tampon LAB_05C2, puis décodage
 * vers l'écran `dest` (variable contenant son adresse). */
static void piv_to(MogCombat *m, uint32_t dest, uint32_t off, uint32_t len)
{
    mog_set_planes(m, rl(m, dest));
    copy(m, rl(m, MOG_b_Piv), rl(m, MOG_t_FastBuffers + off), len);
    mog_piv_decode(m, rl(m, MOG_b_Piv));
}

/* LAB_0C27 : image `name` (fichier PIV) chargée en a1 puis décodée vers
 * les plans LAB_0CFF ; palette -> LAB_0D2B. */
void mog_load_picture(MogCombat *m, uint32_t name, uint32_t a1)
{
    wl(m, MOG_v_PivData, a1);
    MogFile f;
    mog_file_open(VM, name, &f);
    mog_file_read(VM, &f, a1, 6);
    uint32_t n = 32;
    ww(m, MOG_v_PivPlanes, rw(m, a1));
    if (rw(m, a1) != 4)
        n = 64;
    mog_file_read(VM, &f, MOG_t_PivPalette, n);
    for (uint32_t i = 0; i < n / 2; i++)
        ww(m, MOG_t_PivPalette + 2 * i, moon_piv_colour(rw(m, MOG_t_PivPalette + 2 * i)));
    uint32_t len = rl(m, a1 + 2);
    mog_file_read(VM, &f, a1 + 2, len);
    mog_file_close(&f);
    ww(m, MOG_v_GfxReady, 1);                             /* LAB_0C2B */
    mog_unpack(VM, a1 + 2, len, rl(m, MOG_t_DestPlanes));
}

void mog_show_background(MogCombat *m) { show_background(m); }

/* LAB_0418 : décor LAB_05C0 recopié dans les deux écrans */
static void show_background(MogCombat *m)
{
    copy_screen(m, rl(m, MOG_v_BgPlanes), rl(m, MOG_v_ShowPlanes));
    copy_screen(m, rl(m, MOG_v_BgPlanes), rl(m, MOG_v_DrawPlanes));
}

/* LAB_0134 : sons des canaux, écran de message (LAB_0138), phrase suivante
 * de LAB_071E, palette. */
static void loading_screen(MogCombat *m)
{
    for (int ch = 0; ch < 4; ch++)                      /* LAB_0133 */
        mog_snd_play(m, 0x6E + ch, ch);
    /* LAB_0138 */
    uint32_t a2 = rl(m, MOG_v_PalCurrent);                  /* LAB_03EB : noir */
    for (int i = 0; i < 33; i++)
        ww(m, a2 + 2u * (unsigned)i, 0);
    mog_clear_screen(m, rl(m, MOG_v_ShowPlanes));
    mog_set_planes(m, rl(m, MOG_v_ShowPlanes));
    copy(m, rl(m, MOG_v_DrawPlanes), rl(m, MOG_t_FastBuffers + 52), 0xE6F);
    mog_piv_decode(m, rl(m, MOG_v_DrawPlanes));

    wl(m, MOG_v_Combatants + CMB_FONT, rl(m, MOG_t_FontBank + 16));
    uint16_t n = rw(m, MOG_v_LoadingTextIndex);
    mog_text_records(m, rl(m, MOG_t_LoadingTexts + (uint32_t)(uint16_t)(n << 2)));   /* LAB_0432 */
    n = (uint16_t)(n + 1);
    if (!(sw(n) < 14))
        n = 0;
    ww(m, MOG_v_LoadingTextIndex, n);
    mog_fade_to(m, MOG_t_PivPalette);                       /* LAB_03F2 */
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
    ww(m, MOG_v_TileSrcX, *x);
    ww(m, MOG_v_TileSrcY, *y);
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
        wb(m, MOG_t_TileMasks + i, 0);
    wl(m, MOG_v_TileMask, MOG_t_TileMasks);
    uint16_t sh;
    uint16_t off = plane_offset(rw(m, MOG_v_TileSrcX), rw(m, MOG_v_TileSrcY), &sh);
    uint32_t a0 = rl(m, MOG_v_TileSheet);
    for (int p = 0; p < 5; p++, a0 += PLANE)
        for (uint32_t r = 0, d0 = off, d2 = 0; r < 25; r++, d0 = (uint16_t)(d0 + 40), d2 += 6) {
            uint32_t b = MOG_t_TileMasks + d2;
            wl(m, b, rl(m, b) | rl(m, a0 + (uint32_t)(int32_t)sw((uint16_t)d0)));
            ww(m, b + 4, 0);
        }
}

/* LAB_0A66 : découpage vertical (bas à 200 ; haut : une ligne de moins
 * que nécessaire est sautée, comme l'original) ; X négatif ramené à 0. */
static void tile_clip(MogCombat *m, int16_t x, int16_t y)
{
    int16_t h = sw(rw(m, MOG_v_TileH));
    if ((int16_t)(h + y) > 200) {
        ww(m, MOG_v_TileHClip, (uint16_t)(200 - y));
    } else {
        wl(m, MOG_v_TileSkipRows, 0);
        ww(m, MOG_v_TileMaskSkip, 0);
        if (y < 0) {
            uint16_t d5 = (uint16_t)~y;
            ww(m, MOG_v_TileHClip, (uint16_t)(h - d5));
            wl(m, MOG_v_TileSkipRows, (uint32_t)d5 * 40u);
            ww(m, MOG_v_TileY, 0);
            ww(m, MOG_v_TileClip0A91, 0);
            ww(m, MOG_v_TileMaskSkip, (uint16_t)(d5 * 6));
        }
    }
    ww(m, MOG_v_TileFirstMask, 0xFFFF);
    ww(m, MOG_v_TileLastMask, 0);
    wl(m, MOG_v_TileClip0A8D, 0);
    if (x < 0) {
        ww(m, MOG_v_TileX, 0);
        ww(m, MOG_v_TileClip0A90, 0);
    }
}

/* LAB_0A64 : bloc n° tile de la planche a0 posé en (x, y) sur l'écran
 * a1, couleur 0 transparente (blitter : D = A | ~B & C). */
static void draw_tile(MogCombat *m, uint32_t a0, uint32_t a1, uint16_t x, uint16_t y, uint16_t tile)
{
    ww(m, MOG_v_TileCon0B, 0);
    ww(m, MOG_v_TileCon1, 0);
    ww(m, MOG_v_TileX, x);
    ww(m, MOG_v_TileY, y);
    wl(m, MOG_v_TileSheet, a0);
    wl(m, MOG_v_TileDst, a1);
    ww(m, MOG_v_TileIndex, tile & 0xFF);
    ww(m, MOG_v_TileHClip, rw(m, MOG_v_TileH));
    ww(m, MOG_v_TileWCopy, rw(m, MOG_v_TileW));
    tile_clip(m, (int16_t)x, (int16_t)y);
    uint16_t h = rw(m, MOG_v_TileHClip);
    uint16_t words = (uint16_t)((rw(m, MOG_v_TileW) + 16) >> 4);
    ww(m, MOG_v_TileBltSize, (uint16_t)(h << 6 | words));
    uint16_t tx, ty, sh;
    tile_origin(m, rw(m, MOG_v_TileIndex), &tx, &ty);
    uint32_t src = a0 + plane_offset(tx, ty, &sh) + rl(m, MOG_v_TileSkipRows);
    wl(m, MOG_v_TileSrcPtr, src);
    tile_mask(m);
    uint32_t mask = MOG_t_TileMasks + rw(m, MOG_v_TileMaskSkip);
    wl(m, MOG_v_TileMask, mask);
    uint32_t dst = a1 + plane_offset(rw(m, MOG_v_TileX), rw(m, MOG_v_TileY), &sh);
    wl(m, MOG_v_TileDstPtr, dst);
    uint16_t con = (uint16_t)(sh << 12);
    ww(m, MOG_v_TileCon0B, (uint16_t)(con + 0x0FF2));
    ww(m, MOG_v_TileCon1, con);
    uint16_t amod = rw(m, MOG_v_TileSrcMod), cmod = rw(m, MOG_v_TileDstMod);
    for (int p = 0; p < 5; p++) {                       /* LAB_0A65 */
        uint32_t a = src, b = mask, d = dst;
        for (unsigned r = 0; r < (h ? h : 1024u); r++) {
            uint32_t pa = 0, pb = 0;
            for (unsigned k = 0; k < words; k++, a += 2, b += 2, d += 2) {
                uint32_t wa = rw(m, a);
                if (k == 0)
                    wa &= rw(m, MOG_v_TileFirstMask);
                if (k == words - 1u)
                    wa &= rw(m, MOG_v_TileLastMask);
                uint32_t wbm = rw(m, b);
                uint16_t sa = (uint16_t)((pa << 16 | wa) >> sh);
                uint16_t sb = (uint16_t)((pb << 16 | wbm) >> sh);
                pa = wa;
                pb = wbm;
                ww(m, d, (uint16_t)(sa | (~sb & rw(m, d))));
            }
            a += amod;
            b += rw(m, MOG_v_TileMaskMod);
            d += cmod;
        }
        src += PLANE;
        dst += PLANE;
    }
    wl(m, MOG_v_TileSrcPtr, src);
    wl(m, MOG_v_TileDstPtr, dst);
}

/* SECSTRT_12 : objets du terrain (mots drapeaux|bloc, X, Y ; $FF00 fin,
 * $FE00 sauté) posés sur LAB_05C0 ; planche LAB_05C1 pour $03xx, LAB_0D92
 * sinon. */
static void draw_terrain(MogCombat *m)
{
    ww(m, MOG_v_TerrainObjPos, 0);
    for (;;) {
        uint32_t a5 = rl(m, MOG_b_TerrainObjects) + (uint32_t)(int32_t)sw(rw(m, MOG_v_TerrainObjPos));
        uint16_t w = rw(m, a5), f = w & 0xFF00;
        if (f == 0xFF00)
            return;
        if (f != 0xFE00) {
            uint32_t a0 = f == 0x0300 ? rl(m, MOG_v_SheetPlanes) : rl(m, MOG_v_DrawPlanes);
            draw_tile(m, a0, rl(m, MOG_v_BgPlanes), rw(m, a5 + 2), rw(m, a5 + 4), w);
        }
        ww(m, MOG_v_TerrainObjPos, (uint16_t)(rw(m, MOG_v_TerrainObjPos) + 6));
    }
}

/* LAB_0A6C : terrain sans obstacle (une barrière de fond à Y = $63) */
static void terrain_default(MogCombat *m)
{
    static const uint16_t v[5] = { 1, 0, 0x135, 0x63, 0x0A };
    uint32_t a1 = rl(m, MOG_b_Obstacles);
    for (int i = 0; i < 5; i++)
        ww(m, a1 + 2u * (unsigned)i, v[i]);
    ww(m, MOG_v_ObstacleMaxY, 0x63);
}

/* LAB_0A6D : terrain `name` (LAB_0CC0 : taille puis données compressées) :
 * obstacles (mot nombre, enregistrements de 8 octets) dans SECSTRT_14,
 * objets à dessiner dans LAB_0A83 ; LAB_0A98 = Y maximal des obstacles. */
static void terrain_load(MogCombat *m, uint32_t name)
{
    ww(m, MOG_v_ObstacleMaxY, 0x1E);
    uint32_t a1 = rl(m, MOG_b_Obstacles), a2 = rl(m, MOG_b_TerrainObjects);
    MogFile f;
    mog_file_open(VM, name, &f);
    mog_file_read(VM, &f, a2, 4);
    uint32_t len = rl(m, a2);
    mog_file_read(VM, &f, a2, len);
    mog_file_close(&f);
    ww(m, MOG_v_GfxReady, 1);
    mog_unpack(VM, a2, len, a1);

    uint16_t n = rw(m, a1);
    copy(m, rl(m, MOG_b_TerrainObjects), a1 + 2 + (uint32_t)n * 8u, 0x960);
    uint32_t a0 = a1 + 2;
    for (uint32_t i = 0; i <= n; i++, a0 += 8) {        /* DBF : n + 1 */
        uint16_t y = rw(m, a0 + 4);
        if (!(sw(y) < sw(rw(m, MOG_v_ObstacleMaxY))))
            ww(m, MOG_v_ObstacleMaxY, y);
    }
    draw_terrain(m);
}

/* Terrain suivant de la table `tbl` (8 fichiers, compteur `ctr`), ou celui
 * de la carte (LAB_076D = 2 : LAB_076E). */
static void terrain_next(MogCombat *m, uint32_t tbl, uint32_t ctr, int byte_ctr)
{
    if (rl(m, MOG_v_TerrainMode) == 2) {
        terrain_load(m, rl(m, MOG_v_LairTerrain));
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
static void tiles_common(MogCombat *m) { piv_to(m, MOG_v_DrawPlanes, 12, 0x5149); }

/* LAB_0142 : décor +32 dans LAB_05C0 */
static void background_32(MogCombat *m) { piv_to(m, MOG_v_BgPlanes, 32, 0x51C4); }

/* LAB_013C : décor et terrain selon le type de lieu LAB_08C4 */
static void setup_scenery(MogCombat *m)
{
    mog_clear_screen(m, rl(m, MOG_v_BgPlanes));
    terrain_default(m);
    switch (rl(m, MOG_v_PlaceType)) {
    case 4:                                             /* LAB_0147 */
        piv_to(m, MOG_v_SheetPlanes, 8, 0x5958);             /* LAB_014A */
        tiles_common(m);
        piv_to(m, MOG_v_BgPlanes, 24, 0x6395);
        terrain_next(m, MOG_t_TerrainsForest, MOG_v_TerrainNextForest, 1);
        break;
    case 0:                                             /* LAB_0144 */
        piv_to(m, MOG_v_SheetPlanes, 8, 0x5958);
        tiles_common(m);
        piv_to(m, MOG_v_BgPlanes, 28, 0x51A5);
        terrain_next(m, MOG_t_TerrainsGlade, MOG_v_TerrainNextGlade, 0);
        break;
    case 8:                                             /* LAB_013D */
        piv_to(m, MOG_v_SheetPlanes, 20, 0x4658);            /* LAB_014C */
        tiles_common(m);
        piv_to(m, MOG_v_BgPlanes, 36, 0x4C0A);
        terrain_next(m, MOG_t_TerrainsSwamp, MOG_v_TerrainNextSwamp, 0);
        break;
    case 12:                                            /* LAB_0140 */
        piv_to(m, MOG_v_SheetPlanes, 16, 0x3A55);            /* LAB_014E */
        tiles_common(m);
        background_32(m);
        terrain_next(m, MOG_t_TerrainsWater, MOG_v_TerrainNextWater, 0);
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

void mog_loading_screen(MogCombat *m) { loading_screen(m); }

void mog_load_sounds(MogCombat *m, uint32_t name, uint32_t dst_var, uint32_t n)
{
    load_sounds(m, name, dst_var, n);
}

static uint32_t bank(unsigned i) { return MOG_t_BankEnemy + 4u * i; }

static uint32_t cel_after(MogCombat *m, unsigned i, uint32_t name)
{
    return rl(m, bank(i)) + mog_cel_size(VM, name);
}

/* LAB_05E1[4] = Kn5.ob chargé en rl(LAB_05B9 + 44) (LAB_0120 / LAB_0127) */
static void knight_extra(MogCombat *m)
{
    wl(m, MOG_t_BankKnight + 16, rl(m, MOG_t_FastBuffers + 44));
    mog_load_cel(VM, MOG_s_Kn5Ob, rl(m, MOG_t_BankKnight + 16));
}

/* Chargeur de la créature (LAB_0116, LAB_0118...) */
static void load_creature(MogCombat *m, uint32_t fn)
{
    uint32_t a1;
    switch (fn) {
    case MOG_Load_EnemyKnight:
        mog_load_enemy_knight(VM);
        break;
    case MOG_Load_TroggAxe:                                  /* Troggs à hache */
    case MOG_Load_TroggSpear:                                  /* Troggs à lance */
        if (rb(m, MOG_v_CreatureLoaded) == (fn == MOG_Load_TroggAxe ? 0x18 : 0x20))
            break;
        wb(m, MOG_v_CreatureLoaded, fn == MOG_Load_TroggAxe ? 0x18 : 0x20);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        if (fn == MOG_Load_TroggAxe) {
            mog_load_cel(VM, MOG_s_TroggAxe1Cel, a1);
            a1 = cel_after(m, 0, MOG_s_TroggAxe1Cel);
            wl(m, bank(1), a1);
            mog_load_hit_cel(VM, MOG_s_TroggAxe2Cel, a1);
        } else {
            mog_load_cel(VM, MOG_s_TroggSpear1Cel, a1);
            a1 = cel_after(m, 0, MOG_s_TroggSpear1Cel);
            for (unsigned i = 1; i < 5; i++)
                wl(m, bank(i), a1);
            mog_load_hit_cel(VM, MOG_s_TroggSpear2Cel, a1);
        }
        load_sounds(m, MOG_s_TrA, MOG_b_SoundsCreature, 0xAB28);     /* LAB_0AB1 */
        break;
    case MOG_Load_Ratmen:                                  /* Ratmen */
        if (rb(m, MOG_v_CreatureLoaded) == 0x24)
            break;
        wb(m, MOG_v_CreatureLoaded, 0x24);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_s_Ratmen1Cel, a1);
        a1 = cel_after(m, 0, MOG_s_Ratmen1Cel);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_s_Ratmen2Cel, a1);
        load_sounds(m, MOG_s_RaA, MOG_b_SoundsRatmen, 0xD508);     /* LAB_0AB2 */
        break;
    case MOG_Load_Mudmen:                                  /* Mudmen */
        wb(m, MOG_v_CreatureLoaded, 4);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_s_Mudmen1Cel, a1);
        a1 = rl(m, MOG_t_FastBuffers + 44);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_s_Mudmen2Cel, a1);
        load_sounds(m, MOG_s_MuA, MOG_b_SoundsCreature, 0xB690);     /* LAB_0AB3 */
        break;
    case MOG_Load_Balok:                                  /* Balok */
        if (rb(m, MOG_v_CreatureLoaded) != 0x30) {
            wb(m, MOG_v_CreatureLoaded, 0x30);
            a1 = rl(m, MOG_t_ChipBuffers + 8);
            wl(m, bank(0), a1);
            mog_load_hit_cel(VM, MOG_s_Balok1Cel, a1);
            wl(m, bank(1), cel_after(m, 0, MOG_s_Balok1Cel));
            mog_load_cel(VM, MOG_s_Balok3Cel, rl(m, bank(1)));
            a1 = cel_after(m, 1, MOG_s_Balok3Cel);
            wl(m, bank(2), a1);
            mog_load_cel(VM, MOG_s_Balok2Cel, a1);
            load_sounds(m, MOG_s_BaA, MOG_b_SoundsCreature, 0xBDCC); /* LAB_0AAB */
        }
        knight_extra(m);
        break;
    case MOG_Load_Dragon:                                  /* Dragon */
        wb(m, MOG_v_CreatureLoaded, 0x14);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_s_Dragon1Cel, a1);
        a1 = cel_after(m, 0, MOG_s_Dragon1Cel);
        wl(m, bank(1), a1);
        mog_load_hit_cel(VM, MOG_s_Dragon2Cel, a1);
        a1 = cel_after(m, 1, MOG_s_Dragon2Cel);
        wl(m, bank(4), a1);
        mog_load_hit_cel(VM, MOG_s_Dragon5Cel, a1);
        load_sounds(m, MOG_s_DrA, MOG_b_SoundsCreature, 0xC140);     /* LAB_0AAC */
        knight_extra(m);
        break;
    case MOG_Load_PassingKnight:                                  /* chevalier de passage */
        if (rb(m, MOG_v_CreatureLoaded) == 0)
            break;
        wb(m, MOG_v_CreatureLoaded, 0);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_s_Be1C, a1);
        a1 = cel_after(m, 0, MOG_s_Be1C);
        wl(m, bank(1), a1);
        mog_load_cel(VM, MOG_s_Be2C, a1);
        load_sounds(m, MOG_s_BeA, MOG_b_SoundsCreature, 0x57BE);     /* LAB_0AAD */
        break;
    case MOG_Load_Demon:                                  /* Démon */
        background_32(m);
        wb(m, MOG_v_CreatureLoaded, 8);
        a1 = rl(m, MOG_t_FastBuffers + 44);
        wl(m, bank(3), a1);
        mog_load_cel(VM, MOG_s_Demon4Cel, a1);
        a1 = rl(m, MOG_t_FastBuffers + 44) + mog_cel_size(VM, MOG_s_Demon4Cel);
        wl(m, bank(4), a1);
        mog_load_cel(VM, MOG_s_Demon1Cel, a1);
        a1 = rl(m, MOG_t_ChipBuffers + 8);
        wl(m, bank(0), a1);
        mog_load_hit_cel(VM, MOG_s_Demon2Cel, a1);
        a1 = cel_after(m, 0, MOG_s_Demon2Cel);
        wl(m, bank(2), a1);
        mog_load_hit_cel(VM, MOG_s_Demon3Cel, a1);
        load_sounds(m, MOG_s_GuA, MOG_b_SoundsCreature, 0xB27C);     /* LAB_0AAE */
        break;
    case MOG_Load_Troll:                                  /* Troll */
        if (rb(m, MOG_v_CreatureLoaded) != 0x40) {
            wb(m, MOG_v_CreatureLoaded, 0x40);
            wl(m, MOG_v_LoadPtr, rl(m, MOG_t_ChipBuffers + 8));
            wl(m, bank(0), rl(m, MOG_v_LoadPtr));
            mog_load_hit_cel(VM, MOG_s_Troll1Cel, rl(m, MOG_v_LoadPtr));
            wl(m, MOG_v_LoadPtr, rl(m, MOG_v_LoadPtr) + mog_cel_size(VM, MOG_s_Troll1Cel));
            wl(m, bank(1), rl(m, MOG_v_LoadPtr));
            mog_load_hit_cel(VM, MOG_s_Troll2Cel, rl(m, MOG_v_LoadPtr));
        }
        knight_extra(m);
        load_sounds(m, MOG_s_ToA, MOG_b_SoundsCreature, 0xBBC8);     /* LAB_0AAF */
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Objets                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0167 : tables et portées du chevalier */
static void knight_kit(MogCombat *m, uint32_t a1)
{
    wl(m, a1 + OBJ_ATTACKS, MOG_t_KnightAttacks);
    wl(m, a1 + OBJ_REACTIONS, MOG_t_KnightScripts);
    wl(m, a1 + OBJ_DAMAGE, MOG_t_KnightDamage);
    wl(m, a1 + OBJ_WALK, MOG_t_KnightWalk);
    wl(m, a1 + OBJ_PARRY, MOG_t_KnightField50);
    wl(m, a1 + OBJ_STAND, MOG_x_KnightStand);
    wl(m, a1 + OBJ_RECOIL, MOG_x_KnightRecoil);
    wl(m, a1 + OBJ_BANKS, MOG_t_BankKnight);
    ww(m, a1 + OBJ_REACH, 0x64);
    ww(m, a1 + OBJ_DEPTH_REACH, 4);
    ww(m, a1 + OBJ_TOO_CLOSE, 0x50);
}

/* LAB_0195 : Dragon (tête et corps) */
static void dragon_kit(MogCombat *m, uint32_t a1)
{
    wl(m, a1 + OBJ_DAMAGE, MOG_t_DragonDamage);
    wl(m, a1 + OBJ_BANKS, MOG_t_BankEnemy);
    wl(m, a1 + OBJ_REACTIONS, MOG_t_DragonScripts);
    wl(m, a1 + OBJ_WALK, MOG_t_DragonWalk);
    wl(m, a1 + OBJ_STAND, MOG_x_DragonStand);
    wl(m, a1 + OBJ_RECOIL, MOG_x_DragonStand);
    ww(m, a1 + OBJ_REACH, 0x3C);
    ww(m, a1 + OBJ_TOO_CLOSE, 0x14);
    ww(m, a1 + OBJ_DEPTH_REACH, 5);
    ww(m, a1 + OBJ_HP, 0x78);
    ww(m, a1 + OBJ_HP_MAX, 0x78);
    wb(m, a1 + OBJ_CONTROLLER, 0x14);
    wb(m, a1 + OBJ_FACING, 1);
    wb(m, a1 + OBJ_PORT, 4);
}

/* LAB_0305 : entités, contextes et listes de frames remis à zéro */
void mog_reset_entities(MogCombat *m)
{
    for (uint32_t i = 0; i < 500; i++)
        wb(m, MOG_t_Entities + i, 0);
    for (uint32_t i = 0; i < 36; i++)
        wb(m, MOG_t_Contexts + i, 0);
    for (uint32_t i = 0; i < 0x2D0; i++)                /* LAB_03A7 */
        wb(m, MOG_b_RestoreA + i, 0xFF);
    ix_clear_frame_lists(&m->eng);
    for (uint32_t i = 0; i < 10; i++) {
        uint32_t en = MOG_t_Entities + i * IX_ENTITY_SIZE;
        wl(m, en + ENT_LIST_STRIKE, MOG_t_StrikeFrames + i * 80);
        wl(m, en + ENT_LIST_BODY, MOG_t_BodyFrames + i * 80);
        wl(m, en + ENT_CTX, MOG_t_Contexts + i * IX_CTX_SIZE);
    }
    mog_clear_hit_links(m);
    wl(m, MOG_v_HitDataEnd, rl(m, MOG_v_HitDataEndBase));
    wl(m, MOG_v_HitByCelNext, rl(m, MOG_v_HitByCelBase));
}

/* LAB_02CE : les 20 objets de LAB_05C3 à zéro */
static void clear_objects(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_v_Objects);
    for (uint32_t i = 0; i < 0xA50; i++)
        wb(m, a0 + i, 0);
}

/* LAB_01A4 : le chevalier du joueur (LAB_0633) entre à droite */
static void player_enters(MogCombat *m)
{
    uint32_t a1 = rl(m, MOG_v_CurObj);
    wl(m, MOG_v_PlayerObj, a1);
    wl(m, MOG_v_Combatants, a1);
    ww(m, a1 + 4, 0xFA);
    ww(m, a1 + 6, 0);
    ww(m, a1 + 8, 0x64);
    wb(m, a1 + 10, 3);
    knight_kit(m, a1);
    mog_enter_object_with(m, a1, MOG_x_KnightEnter);
}

/* LAB_015F : états des quatre chevaliers remis à zéro */
static void reset_knights(MogCombat *m)
{
    uint32_t a1 = MOG_t_KnightObjects;
    for (int i = 0; i < 4; i++, a1 += IX_OBJECT_SIZE) {
        wb(m, a1 + OBJ_WALK_PHASE, 0);
        wb(m, a1 + OBJ_COUNTDOWN, 0);
        wl(m, a1 + OBJ_HIT, 0);
        wl(m, a1 + OBJ_HIT_BY, 0);
        ww(m, a1 + OBJ_AI_FLAGS, 0);
        ww(m, a1 + OBJ_ATTACK, 0);
        ww(m, a1 + OBJ_INPUT, 0);
    }
}

/* LAB_016F : écran de chargement, décor, objets, joueur */
static void prepare(MogCombat *m)
{
    loading_screen(m);                                  /* LAB_0134 */
    setup_scenery(m);                                   /* LAB_013C */
    clear_objects(m);                                   /* LAB_02CE */
    mog_reset_entities(m);                                  /* LAB_0305 */
    for (uint32_t i = 0; i < 0x78; i++)                 /* LAB_02F2 */
        wb(m, MOG_t_Trajectories + i, 0);
    ww(m, MOG_v_EntrySide, 0);
    player_enters(m);                                   /* LAB_01A4 */
    ww(m, MOG_v_MudState, 0);
}

/* Compteurs d'adversaires : LAB_05ED (présents à la fois), LAB_05EC (à
 * vaincre), LAB_05EE (entrés) ; routines « suivant » et « équipement ». */
static void opponents(MogCombat *m, uint16_t ed, uint16_t ec, uint32_t next, uint32_t kit)
{
    ww(m, MOG_v_FoesAtOnce, ed);
    ww(m, MOG_v_FoesToBeat, ec);
    ww(m, MOG_v_FoesEntered, 0);
    wl(m, MOG_v_NextFoeFn, next);
    if (kit)
        wl(m, MOG_v_FoeKitFn, kit);
    ww(m, MOG_v_FrameVbls, 6);
}

/* LAB_0177 : nombre d'adversaires selon la force du joueur */
static void scale_opponents(MogCombat *m)
{
    ww(m, MOG_v_FoeScale, 0);
    uint32_t a0 = rl(m, MOG_v_CurObj);
    if (!((int8_t)rb(m, a0 + OBJ_STRENGTH) <= 3))
        ww(m, MOG_v_FoesAtOnce, (uint16_t)(rw(m, MOG_v_FoesAtOnce) + 1));
    int16_t maxhp = sw(rw(m, a0 + OBJ_HP_MAX));
    if (!(maxhp < 0x1E)) {
        ww(m, MOG_v_FoesToBeat, (uint16_t)(rw(m, MOG_v_FoesToBeat) + 1));
        ww(m, MOG_v_FoeScale, 1);
    }
    if (!(maxhp < 0x3C)) {
        ww(m, MOG_v_FoesAtOnce, (uint16_t)(rw(m, MOG_v_FoesAtOnce) + 1));
        ww(m, MOG_v_FoeScale, 2);
    }
    if (!(maxhp < 0x5A)) {
        ww(m, MOG_v_FoeScale, 3);
        ww(m, MOG_v_FoesToBeat, (uint16_t)(rw(m, MOG_v_FoesToBeat) + 1));
    }
    if (rl(m, MOG_v_TerrainMode) == 2)
        ww(m, MOG_v_FoesToBeat, rw(m, rl(m, MOG_v_Lair) + 6));
    if (rl(m, MOG_v_NextFoeFn) == MOG_Next_Balok)
        ww(m, MOG_v_FoesAtOnce, 1);
    if (rl(m, MOG_v_NextFoeFn) == MOG_Next_EntriesC2)
        ww(m, MOG_v_FoesAtOnce, 1);
    if (rl(m, MOG_v_FoeKitFn) == MOG_Kit_Troll && !(sw(rw(m, MOG_v_FoesAtOnce)) <= 2))
        ww(m, MOG_v_FoesAtOnce, 2);
    if (sw(rw(m, MOG_v_FoesToBeat)) <= 0)
        return;
    a0 = rl(m, MOG_v_CurObj);
    ww(m, a0 + OBJ_ATTACK, 8);
    uint16_t d0 = mog_knight_damage(m, a0);
    d0 = (uint16_t)(d0 + (rw(m, a0 + OBJ_HP_MAX) >> 2));
    d0 = (uint16_t)(d0 >> 1);
    d0 = (uint16_t)(d0 - 6);
    if (sw(d0) < 0)
        d0 = 0;
    if (!(sw(d0) < 0x10))
        d0 = 0x0F;
    d0 >>= 1;
    uint32_t f1 = rl(m, MOG_v_FoeKitFn);
    for (unsigned i = 0; i < 8; i++) {
        if (rl(m, MOG_t_FoeScaledKits + 4 * i) != f1)
            continue;
        int16_t v = (int8_t)rb(m, MOG_t_FoeScaling + i * 8 + d0);
        int16_t d1 = (int16_t)(rw(m, MOG_v_FoesToBeat) - v);
        if (d1 > 0)
            ww(m, MOG_v_FoesToBeat, (uint16_t)d1);
        return;
    }
}

/* LAB_016D : les LAB_05ED premiers adversaires entrent (enregistrements
 * successifs de la table a0) */
static void first_opponents(MogCombat *m, uint32_t a0)
{
    uint32_t k = (uint32_t)(uint16_t)(rw(m, MOG_v_FoesAtOnce) - 1) + 1;
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
    uint32_t k = rl(m, a1 + OBJ_KNIGHT);
    put(m, a0, c[k <= 3 ? k : 4], 3);
}

static void set_palette(MogCombat *m, uint32_t type)
{
    copy(m, MOG_t_PalCombat, MOG_t_PivPalette, 64);
    wl(m, MOG_v_PaletteKind, type);
    uint32_t a0 = MOG_t_PalCombat + 0x12;
    switch (type) {
    case 0: {
        static const uint16_t v[] = { 0x776, 0x443, 0x731, 0x520, 0x300, 0x09A, 0xC00 };
        put(m, a0, v, 7);
        break;
    }
    case 12:
        knight_colours(m, a0, rl(m, MOG_v_Combatants + CMB_OPPONENT));
        break;
    case 24:
    case 32: {
        static const uint16_t v12[] = { 0x025, 0x004, 0x001, 0x830, 0x400, 0xF80, 0xC00 };
        static const uint16_t v0[]  = { 0x104, 0x102, 0x000, 0x600, 0x300, 0x693, 0xC00 };
        static const uint16_t vx[]  = { 0x500, 0x200, 0x000, 0xB40, 0x610, 0x895, 0xC00 };
        uint32_t g = rl(m, MOG_v_PlaceType);
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
        put(m, MOG_t_PalCombat + 0x3A, w, 3);
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
        copy(m, a0, MOG_t_PalCombatBase, 46);
        break;
    case 4: {
        static const uint16_t v[] = { 0x332, 0xCCB, 0xB81, 0x851, 0x630, 0xF52, 0x900 };
        put(m, a0, v, 7);
        break;
    }
    }
    /* LAB_0401 */
    mog_fade_out(m);                                    /* LAB_03F1 */
    show_background(m);                                 /* LAB_0418 */
    knight_colours(m, MOG_t_PalCombat + 0x0C, rl(m, MOG_v_Combatants));
    if (rl(m, MOG_v_PaletteKind) != 8) {                     /* LAB_0409 */
        static const uint16_t hi[] = { 0xFFD, 0x998, 0x776, 0x443 };
        uint32_t a1 = 0;
        switch (rl(m, MOG_v_PlaceType)) {
        case 0:  a1 = MOG_t_PalGlade; break;
        case 4:  a1 = MOG_t_PalForest; break;
        case 8:  put(m, MOG_t_PalCombat + 2, hi, 4); a1 = MOG_t_PalSwamp; break;
        case 12: put(m, MOG_t_PalCombat + 2, hi, 4); a1 = MOG_t_PalWater; break;
        }
        if (a1)
            copy(m, MOG_t_PalCombat + 0x20, a1, 26);
    }
    ww(m, MOG_t_PalCombat, 0);
    if (rl(m, MOG_v_PaletteKind) != 20)
        ww(m, MOG_t_PalCombat + 30, 0xC00);
    mog_fade_to(m, MOG_t_PalCombat);                       /* LAB_03F2 */
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
    case MOG_Enc_EnemyKnight:                                  /* chevalier adverse */
        prepare(m);
        load_creature(m, MOG_Load_EnemyKnight);
        a1 = rl(m, MOG_v_Combatants + CMB_OPPONENT);
        wl(m, MOG_v_TargetObj, a1);
        ww(m, a1 + OBJ_X, 0x1E);
        ww(m, a1 + OBJ_HEIGHT, 0);
        ww(m, a1 + OBJ_DEPTH, 0x4B);
        wb(m, a1 + OBJ_FACING, 1);
        knight_kit(m, a1);
        wl(m, a1 + OBJ_BANKS, MOG_t_BankEnemy);
        mog_enter_object_with(m, a1, MOG_x_KnightEnter);
        opponents(m, 1, 1, MOG_Kit_None, 0);
        set_palette(m, 12);
        return 1;
    case MOG_Enc_TroggAxe:                                  /* Troggs à hache */
    case MOG_Enc_TroggAxe2:                                  /* Troggs à hache (2) */
        prepare(m);
        horde(m, MOG_Load_TroggAxe, 1, 3, MOG_Next_EntriesA,
              fn == MOG_Enc_TroggAxe ? MOG_Kit_TroggA : MOG_Kit_TroggB, MOG_t_EntriesA, 0x18);
        return 1;
    case MOG_Enc_TroggSpear:                                  /* Troggs à lance */
        prepare(m);
        load_creature(m, MOG_Load_TroggSpear);
        a0 = rl(m, rl(m, MOG_v_CurObj) + OBJ_ATTACKS);
        wl(m, a0 + 28, MOG_x_KnightAtk7);
        wl(m, a0 + 16, MOG_x_KnightAtk7);
        opponents(m, 1, 3, MOG_Next_EntriesA, MOG_Kit_TroggSpear);
        scale_opponents(m);
        first_opponents(m, MOG_t_EntriesA);
        set_palette(m, 0x20);
        return 1;
    case MOG_Enc_PassingKnights:                                  /* chevaliers de passage */
        prepare(m);
        load_creature(m, MOG_Load_PassingKnight);
        a1 = rl(m, MOG_v_CurObj);
        wl(m, rl(m, a1 + OBJ_ATTACKS) + 28, MOG_x_KnightAtk7);
        a0 = rl(m, a1 + OBJ_REACTIONS);
        wl(m, a0 + 32, MOG_x_PassingKnightHitFront);
        wl(m, a0 + 28, MOG_x_PassingKnightHitFront);
        opponents(m, 1, 3, MOG_Next_EntriesB, MOG_Kit_PassingKnight);
        scale_opponents(m);
        first_opponents(m, MOG_t_EntriesB);
        set_palette(m, 0);
        return 1;
    case MOG_Enc_Ratmen: {                                /* Ratmen */
        prepare(m);
        load_creature(m, MOG_Load_Ratmen);
        a0 = rl(m, rl(m, MOG_v_CurObj) + OBJ_ATTACKS);
        wl(m, a0 + 16, MOG_x_KnightRatAtk4);
        wl(m, a0 + 28, MOG_x_KnightRatReact7);
        opponents(m, 2, 2, MOG_Next_EntriesC, MOG_Kit_Ratmen);
        wl(m, MOG_v_KnightAiFlags, 0);
        uint16_t v = rw(m, MOG_v_ObstacleMaxY);                /* LAB_01A5 */
        uint16_t d = (uint16_t)((((uint16_t)(200 - v)) >> 1) + v - 0x2F);
        ww(m, MOG_v_EntryDepth3, d);
        wl(m, MOG_v_DemonCompanion, ix_spawn(&m->eng, MOG_x_DemonCompanionEnter, MOG_t_BankEnemy, 0xA0,
                                     (int16_t)(d - 0xC8), (int16_t)d, 1, 0x28));
        scale_opponents(m);
        first_opponents(m, MOG_t_EntriesC);
        set_palette(m, 0x24);
        return 1;
    }
    case MOG_Enc_Dragon: {                                /* Dragon */
        prepare(m);
        load_creature(m, MOG_Load_Dragon);
        a0 = rl(m, rl(m, MOG_v_CurObj) + OBJ_REACTIONS);
        wl(m, a0 + 4, MOG_x_KnightReact8);
        wl(m, a0 + 8, MOG_x_KnightDragonReact);
        wl(m, a0 + 32, MOG_x_KnightDragonReact);
        wl(m, a0 + 20, MOG_x_KnightDropped);
        wl(m, MOG_t_DragonDamage + 8, 0x1E);
        wl(m, MOG_t_DragonDamage + 32, 0x1E);
        wl(m, MOG_t_DragonDamage + 20, 0x0A);
        wl(m, MOG_t_DragonDamage + 4, 0x0A);
        a1 = MOG_v_DragonObj;
        ww(m, a1 + OBJ_X, 0x50);
        ww(m, a1 + OBJ_HEIGHT, 0xFFD8);
        ww(m, a1 + OBJ_DEPTH, 0x64);
        wb(m, a1 + OBJ_FACING, 1);
        dragon_kit(m, a1);
        ww(m, MOG_v_DragonState, 0);
        mog_enter_object(m, a1);
        static const uint16_t depth[2] = { 0x50, 0x78 };
        static const uint32_t var[2] = { MOG_v_DragonPartA, MOG_v_DragonPartB };
        for (int i = 0; i < 2; i++) {
            a1 = mog_alloc_object(m);
            wl(m, var[i], a1);
            ww(m, a1 + OBJ_X, 5);
            ww(m, a1 + OBJ_HEIGHT, 0);
            ww(m, a1 + OBJ_DEPTH, depth[i]);
            ww(m, a1 + OBJ_HP, 0x32);
            ww(m, a1 + OBJ_HP_MAX, 0x32);
            dragon_kit(m, a1);
            wb(m, a1 + OBJ_CONTROLLER, 0x2C);
            wl(m, a1 + OBJ_STAND, MOG_x_DragonPartStand);
            wl(m, a1 + OBJ_RECOIL, MOG_x_DragonPartStand);
            ww(m, a1 + OBJ_DEPTH_REACH, 0x0A);
            mog_enter_object(m, a1);
        }
        opponents(m, 1, 1, MOG_Kit_None, MOG_Kit_None);
        set_palette(m, 0x14);
        return 1;
    }
    case MOG_Enc_Balok:                                  /* Balok */
        prepare(m);
        load_creature(m, MOG_Load_Balok);
        wl(m, rl(m, rl(m, MOG_v_TargetObj) + OBJ_REACTIONS) + 8, MOG_x_KnightDropped);
        opponents(m, 1, 2, MOG_Next_Balok, MOG_Kit_Balok);
        scale_opponents(m);
        first_opponents(m, MOG_t_EntriesDragon);
        set_palette(m, 0x30);
        return 1;
    case MOG_Enc_Mudmen:                                  /* Mudmen */
        wl(m, MOG_v_PlaceType, 8);
        prepare(m);
        load_creature(m, MOG_Load_Mudmen);
        opponents(m, 1, 2, MOG_Next_EntriesC2, MOG_Kit_Mudmen);
        scale_opponents(m);
        mog_next_opponent(m, MOG_Next_EntriesC2);
        set_palette(m, 4);
        return 1;
    case MOG_Enc_Troll:                                  /* Troll */
        prepare(m);
        load_creature(m, MOG_Load_Troll);
        wl(m, rl(m, rl(m, MOG_v_TargetObj) + OBJ_REACTIONS) + 8, MOG_x_KnightDropped);
        opponents(m, 1, 1, MOG_Next_EntriesA, MOG_Kit_Troll);
        scale_opponents(m);
        mog_next_opponent(m, MOG_Next_EntriesA);
        set_palette(m, 0x40);
        return 1;
    case MOG_Enc_Demon:                                  /* Démon */
        loading_screen(m);
        load_creature(m, MOG_Load_Demon);
        reset_knights(m);
        clear_objects(m);
        mog_reset_entities(m);
        terrain_default(m);
        player_enters(m);
        wl(m, rl(m, rl(m, MOG_v_CurObj) + OBJ_REACTIONS) + 32, MOG_x_KnightDropped);
        a1 = mog_alloc_object(m);
        wl(m, MOG_v_DemonObj, a1);
        wl(m, a1 + OBJ_STAND, MOG_x_DemonStand);
        wl(m, a1 + OBJ_RECOIL, MOG_x_DemonRecoil);
        wl(m, a1 + OBJ_BANKS, MOG_t_BankEnemy);
        ww(m, a1 + OBJ_HP, 0x8C);
        wb(m, a1 + OBJ_CONTROLLER, 8);
        wb(m, a1 + OBJ_PORT, 4);
        ww(m, a1 + OBJ_X, 0x64);
        ww(m, a1 + OBJ_HEIGHT, 5);
        ww(m, a1 + OBJ_DEPTH, 0x64);
        ww(m, a1 + OBJ_DEPTH_REACH, 2);
        ww(m, a1 + OBJ_TOO_CLOSE, 0x5A);
        ww(m, a1 + OBJ_REACH, 0x5F);
        wb(m, a1 + OBJ_FACING, 1);
        mog_enter_object(m, a1);
        wl(m, MOG_v_DemonCompanionObj, mog_alloc_object(m));
        opponents(m, 1, 1, MOG_Kit_None, 0);
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
    wb(m, a1 + OBJ_STRENGTH, 1);
    wb(m, a1 + OBJ_CONSTITUTION, 1);
    wb(m, a1 + OBJ_ENDURANCE, 1);
    wb(m, a1 + OBJ_LIVES, 5);
    ww(m, a1 + OBJ_HP, 0x14);
    wl(m, a1 + OBJ_ARMOUR, 0x1B);
    wl(m, a1 + OBJ_WEAPON, 0x16);
    ww(m, a1 + OBJ_EXPERIENCE, 0);
    wb(m, a1 + OBJ_DAGGERS, 0x0A);
    ww(m, a1 + OBJ_GOLD, 0x0A);
    wb(m, a1 + OBJ_LUCK, 0xFF);
    wb(m, a1 + OBJ_POISONED, 0);
    wl(m, a1 + OBJ_TARGET, 0);
    uint32_t a0 = rl(m, a1 + OBJ_INVENTORY);
    for (uint32_t i = 0; i < 24; i++)
        wb(m, a0 + i, 0);
}

/* LAB_0013 : PV maximum (constitution, potion 6, armure) */
void mog_knight_hp(MogCombat *m, uint32_t a0)
{
    uint16_t d1 = (uint16_t)(rb(m, a0 + OBJ_CONSTITUTION) * 10);
    uint32_t a1 = rl(m, a0 + OBJ_INVENTORY);
    if (rb(m, a1 + 4))
        wl(m, a0 + OBJ_WEAPON, 0x19);
    d1 = (uint16_t)(d1 + rb(m, a1 + 6) * 20);
    uint32_t armour = rl(m, a0 + OBJ_ARMOUR);
    if (armour == 0x1C) d1 = (uint16_t)(d1 + 10);
    if (armour == 0x1D) d1 = (uint16_t)(d1 + 20);
    if (armour == 0x1E) d1 = (uint16_t)(d1 + 30);
    d1 = (uint16_t)(d1 + 10);
    ww(m, a0 + OBJ_HP_MAX, d1);
    if (!(sw(d1) > sw(rw(m, a0 + OBJ_HP))))
        ww(m, a0 + OBJ_HP, d1);
}

/* LAB_0019 : défense (endurance, armure) */
void mog_knight_defence(MogCombat *m, uint32_t a0)
{
    uint32_t armour = rl(m, a0 + OBJ_ARMOUR);
    uint8_t b = (uint8_t)(rb(m, a0 + OBJ_ENDURANCE) << 1);
    if (armour == 0x1C) b = (uint8_t)(b + 2);
    if (armour == 0x1E) b = (uint8_t)(b + 2);
    wb(m, a0 + OBJ_MOVEMENT, (uint8_t)(b + 4));
}

void mog_update_knight(MogCombat *m, uint32_t a0)
{
    mog_knight_hp(m, a0);
    mog_knight_defence(m, a0);
}

/* LAB_01AE : partie commune (chevaliers noirs, dragon, tables) */
static void new_game_01ae(MogCombat *m)
{
    ww(m, MOG_v_DeadPlayers, 0);
    ww(m, MOG_v_ReversedOn, 0);
    ww(m, MOG_v_MovesUsed, 0);
    ww(m, MOG_v_TurnKnight, 0);
    ww(m, MOG_v_Unused05F3, 1);
    ww(m, MOG_v_Combatants + CMB_MOON, 0x2D);
    ww(m, MOG_v_Combatants + CMB_DAY, 0);
    ww(m, MOG_v_Round, 0);
    wl(m, MOG_v_Combatants, MOG_t_KnightObjects);
    wl(m, MOG_v_TargetObj, MOG_t_KnightObjects);
    wb(m, MOG_v_Combatants + CMB_ACTIVE, 0);
    static const uint32_t ai[4] = { MOG_s_SirBanner, MOG_s_SirDwain, MOG_s_SirBalain, MOG_s_SirEdward2 };
    static const uint16_t pos[4][2] = { { 0x0F, 0x64 }, { 0x12C, 0x64 }, { 0xA0, 0x14 }, { 0xA0, 0xB4 } };
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_t_KnightObjects + i * IX_OBJECT_SIZE;
        wl(m, a1 + OBJ_NAME, ai[i]);
        wb(m, a1 + OBJ_CONTROLLER, 0x10);
        wb(m, a1 + OBJ_PORT, 4);
        wl(m, a1 + OBJ_KNIGHT, 4);
        ww(m, a1 + OBJ_MAP_X, pos[i][0]);
        ww(m, a1 + OBJ_MAP_Y, pos[i][1]);
    }
    wb(m, MOG_v_DragonObj + OBJ_LIVES, 1);
    dragon_kit(m, MOG_v_DragonObj);
    ww(m, MOG_v_FrameVbls, 6);

    static const struct { uint8_t off; uint32_t fn; } ctl[] = {
        { 36, MOG_Ctl_Ratman }, { 4, MOG_Ctl_Mudman }, { 0, MOG_Ctl_PassingKnight }, { 16, MOG_Ctl_KnightAi },
        { 56, MOG_Ctl_HumanKnight }, { 20, MOG_Ctl_Dragon }, { 8, MOG_Ctl_Demon },
        { 12, MOG_Ctl_HumanKnight }, { 24, MOG_Ctl_Trogg }, { 28, MOG_Ctl_Trogg },
        { 32, MOG_Ctl_Trogg }, { 40, MOG_Ctl_Inert }, { 44, MOG_Ctl_DragonPart }, { 52, MOG_Ctl_Projectile },
        { 48, MOG_Ctl_Leaper }, { 64, MOG_Ctl_Troll }, { 68, MOG_Ctl_Croupier },
    };
    for (unsigned i = 0; i < sizeof ctl / sizeof ctl[0]; i++)
        wl(m, MOG_t_Controllers + ctl[i].off, ctl[i].fn);
    static const struct { uint8_t off; uint32_t fn; } init[] = {
        { 36, MOG_Enc_Ratmen }, { 4, MOG_Enc_Mudmen }, { 0, MOG_Enc_PassingKnights }, { 16, MOG_Enc_EnemyKnight },
        { 56, MOG_Enc_EnemyKnight }, { 20, MOG_Enc_Dragon }, { 8, MOG_Enc_Demon }, { 12, MOG_Enc_EnemyKnight },
        { 24, MOG_Enc_TroggAxe }, { 32, MOG_Enc_TroggSpear }, { 28, MOG_Enc_TroggAxe2 }, { 48, MOG_Enc_Balok },
        { 64, MOG_Enc_Troll },
    };
    for (unsigned i = 0; i < sizeof init / sizeof init[0]; i++)
        wl(m, MOG_t_CreatureInit + init[i].off, init[i].fn);
    wb(m, MOG_v_DragonObj + OBJ_CONTROLLER, 0x14);
    wb(m, MOG_v_DragonObj + OBJ_FACING, 1);
    wl(m, MOG_v_DragonObj + OBJ_INVENTORY, MOG_t_DragonInventory);
    wl(m, MOG_v_DragonObj + OBJ_KNIGHT, 5);
    wl(m, MOG_v_CurObj, MOG_v_DragonObj);
}

/* LAB_01BE : chevaliers des joueurs, inventaires, équipement */
static void new_game_01be(MogCombat *m)
{
    uint16_t players = rw(m, MOG_v_Players);
    for (uint16_t d0 = 0; d0 != players && d0 < 4; d0++) {
        uint32_t a1 = MOG_t_KnightObjects + d0 * IX_OBJECT_SIZE;
        static const uint32_t ai_p[4] = { MOG_s_SirGodber, MOG_s_SirRichard, MOG_s_SirJeffrey, MOG_s_SirEdward };
        static const uint16_t pos_p[4][2] = { { 0x0A, 0x0A }, { 0x12C, 0x05 }, { 0x1A, 0xB4 }, { 0x12C, 0xB9 } };
        uint32_t k = rl(m, a1 + OBJ_KNIGHT);
        if (k > 3)
            continue;
        wl(m, a1 + OBJ_NAME, ai_p[k]);
        ww(m, a1 + OBJ_MAP_X, pos_p[k][0]);
        ww(m, a1 + OBJ_MAP_Y, pos_p[k][1]);
    }
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_t_KnightObjects + i * IX_OBJECT_SIZE;
        wl(m, a1 + OBJ_INVENTORY, MOG_t_KnightInventories + 24 * i);
        knight_defaults(m, a1);
        knight_kit(m, a1);
    }
    for (unsigned i = 0; i < 4; i++) {
        uint32_t a1 = MOG_t_KnightObjects + i * IX_OBJECT_SIZE;
        ww(m, a1 + OBJ_CELL_X, (uint16_t)(rw(m, a1 + OBJ_MAP_X) >> 3));
        ww(m, a1 + OBJ_CELL_Y, (uint16_t)(rw(m, a1 + OBJ_MAP_Y) >> 3));
    }
}

/* Combats seuls : LAB_01AE sans les trésors, LAB_01BE, LAB_0011 */
void mog_new_game(MogCombat *m)
{
    new_game_01ae(m);
    new_game_01be(m);
    for (unsigned i = 0; i < 4; i++)
        mog_update_knight(m, MOG_t_KnightObjects + i * IX_OBJECT_SIZE);
}

/* LAB_01B7 : trésor du repaire LAB_08C6 (LAB_08C5 : seuils, genres) */
static void lair_treasure(MogCombat *m)
{
    int16_t d0 = (int16_t)mog_d100(m);
    uint32_t a0 = MOG_t_LairTreasure;
    for (int d7 = 3; d7 >= 0; d7--) {
        int16_t t = (int16_t)rw(m, a0);
        a0 += 2;
        if (d0 <= t)
            break;
        a0 += 2;
    }
    switch (rw(m, a0)) {                                /* LAB_01BA */
    case 1:                                             /* LAB_01BB */
        mog_find_gold(m, 1);
        break;
    case 2:                                             /* LAB_01BC */
        mog_find_item(m, 1);
        mog_find_item(m, 1);
        mog_find_item(m, 1);
        break;
    case 3:                                             /* LAB_01BD */
        mog_find_gold(m, 1);
        mog_find_item(m, 1);
        mog_find_item(m, 1);
        break;
    }
}

/* LAB_01AE complet (nouvelle partie) : trésor du dragon, boutique
 * LAB_0690, repaires (LAB_05B9+68 : 24 × 20 octets, inventaires en
 * LAB_05B9+72) et leurs gardiens (LAB_07BD-LAB_07C0) */
void mog_new_game_full(MogCombat *m)
{
    new_game_01ae(m);
    for (int i = 0; i < 4; i++)                         /* D3 = 0 : le dragon */
        mog_find_item(m, 0);
    mog_find_gold(m, 0);
    for (uint32_t i = 0; i < 24; i++)
        wb(m, MOG_t_ShopInventory + i, 0);
    for (int i = 0; i < 6; i++)                         /* LAB_01B0 */
        mog_find_item(m, 2);
    wb(m, MOG_t_ShopInventory + 8, (uint8_t)(rb(m, MOG_t_ShopInventory + 8) + 2));
    uint32_t inv = rl(m, MOG_t_FastBuffers + 72), lairs = rl(m, MOG_t_FastBuffers + 68);
    for (uint32_t i = 0; i < 0x240; i++)
        wb(m, inv + i, 0);
    for (uint32_t i = 0; i < 0x1E0; i++)
        wb(m, lairs + i, 0);
    wl(m, MOG_v_Lair, lairs);
    for (uint32_t i = 0; i < 24; i++) {                 /* LAB_01B3 */
        wl(m, lairs + 20 * i, inv + 24 * i);
        ww(m, lairs + 20 * i + 8, 0);
    }
    uint32_t d0;                                        /* LAB_01B4 : les quatre clés */
    do
        d0 = mog_random(m) & 7;
    while ((int8_t)d0 > 5);
    d0 *= 20;
    static const uint8_t key[4] = { 8, 4, 2, 1 };
    for (uint32_t g = 0; g < 4; g++)
        wb(m, rl(m, lairs + 120 * g + d0) + INV_KEYS, key[g]);
    for (int i = 0; i < 24; i++) {                      /* LAB_01B5 */
        lair_treasure(m);
        wl(m, MOG_v_Lair, rl(m, MOG_v_Lair) + 20);
    }
    wl(m, MOG_v_Lair, lairs);                         /* LAB_01B6 */
    for (uint32_t i = 0; i < 24; i++) {
        uint32_t a0 = lairs + 20 * i;
        wl(m, a0 + 4, rl(m, MOG_t_LairCreature + 4 * i));
        ww(m, a0 + 10, rw(m, MOG_t_LairPos + 4 * i));
        ww(m, a0 + 12, rw(m, MOG_t_LairPos + 4 * i + 2));
        ww(m, a0 + 14, rw(m, MOG_t_LairPlace + 2 * i));
        wl(m, a0 + 16, rl(m, MOG_t_LairTerrain + 4 * i));
    }
}

void mog_new_game_players(MogCombat *m)
{
    new_game_01be(m);                                   /* LAB_01BE */
}

/* LAB_0002 (sans Combat_Run) : entraînement du menu, duel entre les
 * chevaliers 1 (joystick 2) et 2 (joystick 1), LAB_05C5 gardé dans
 * LAB_05DB ; puis LAB_0165 (comme LAB_0164, l'adversaire au joystick). */
void mog_practice(MogCombat *m)
{
    ww(m, MOG_v_PlayersSaved, rw(m, MOG_v_Players));
    ww(m, MOG_v_Combatants + CMB_CHOOSERS, 2);
    ww(m, MOG_v_Players, 2);
    mog_new_game_full(m);                               /* LAB_01AE */
    wl(m, MOG_v_Combatants, MOG_t_KnightObjects);
    wl(m, MOG_v_Combatants + CMB_OPPONENT, MOG_v_Knight2Obj);
    wb(m, MOG_t_KnightObjects + OBJ_CONTROLLER, 0x0C);
    wb(m, MOG_t_KnightObjects + OBJ_PORT, 2);
    wl(m, MOG_t_KnightObjects + OBJ_KNIGHT, 0);
    wb(m, MOG_v_Knight2Obj + OBJ_CONTROLLER, 0x0C);
    wb(m, MOG_v_Knight2Obj + OBJ_PORT, 1);
    wl(m, MOG_v_Knight2Obj + OBJ_KNIGHT, 2);
    mog_boot_reactions(VM);                             /* LAB_020F */
    loading_screen(m);                                  /* LAB_0134 */
    wl(m, MOG_v_PlaceType, 12);
    setup_scenery(m);                                   /* LAB_013C */
    mog_new_game_players(m);                            /* LAB_01BE */
    /* LAB_0165 */
    reset_knights(m);                                   /* LAB_015F */
    clear_objects(m);                                   /* LAB_02CE */
    mog_reset_entities(m);                              /* LAB_0305 */
    load_creature(m, MOG_Load_EnemyKnight);
    wl(m, MOG_v_CurObj, MOG_t_KnightObjects);
    player_enters(m);                                   /* LAB_01A4 */
    uint32_t a1 = MOG_v_Knight2Obj;
    wl(m, MOG_v_TargetObj, a1);
    ww(m, a1 + OBJ_X, 0x1E);
    ww(m, a1 + OBJ_HEIGHT, 0);
    wb(m, a1 + OBJ_FACING, 1);
    knight_kit(m, a1);                                  /* LAB_0167 */
    wl(m, a1 + OBJ_BANKS, MOG_t_BankEnemy);
    wb(m, a1 + OBJ_PORT, 1);
    mog_enter_object_with(m, a1, MOG_x_KnightEnter);         /* LAB_01A9 */
    opponents(m, 1, 1, MOG_Kit_None, 0);
    wl(m, MOG_v_PlaceType, 12);
    set_palette(m, 12);                                 /* LAB_03F3 */
    for (uint32_t i = 0; i < 4; i++)                    /* LAB_0011 */
        mog_update_knight(m, MOG_t_KnightObjects + i * IX_OBJECT_SIZE);
}
