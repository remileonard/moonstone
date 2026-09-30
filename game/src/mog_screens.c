/*
 * mog_screens.c — écrans à pointeur de mog (LAB_04CF) : inventaire du
 * chevalier (9), vainqueur d'un duel (1), trésor du dragon (10), échanges
 * et achats (2, 3, 5, 6, 8, 11), traduits de amiga_asm/mog.asm.
 *
 * L'écran est fait de deux panneaux (le chevalier à gauche, l'autre
 * chevalier, la créature ou la boutique à droite) couverts d'icônes ;
 * chaque icône est une zone (SECSTRT_14, 24 octets : +4 l, +6 h, +8 texte
 * à montrer, +12 x, +14 y, +16 identifiant, +20 genre, +22 case de
 * l'inventaire). Le pointeur est un sprite matériel (LAB_0E78) mené au
 * joystick à chaque VBL (LAB_057D, voir mog_screen_vbl).
 */
#include "mog_private.h"
#include "mog_screens.h"
#include "mog_text.h"
#include "mog_encounter.h"
#include "mog_map.h"
#include "mog_vbl.h"
#include "mog_boot.h"
#include "ix_mog_names.h"

#include <stdio.h>

#define VM (m->eng.vm)

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }
static int16_t sw(uint16_t v) { return (int16_t)v; }

#define KIND (rl(m, MOG_v_ScreenKind))
#define COPPER_SPRITES 0x7F6AEu            /* EXT_0023 : pointeurs de sprites de la copper list */

/* ------------------------------------------------------------------ */
/* Sprite du pointeur                                                  */
/* ------------------------------------------------------------------ */

/* LAB_0E7C : mots de contrôle du sprite a0 (x, y) */
static void sprite_ctl(MogCombat *m, uint32_t a0, uint16_t d0, uint16_t d1)
{
    d0 = (uint16_t)(d0 + 0x80);
    d1 = (uint16_t)(d1 + 0x2C);
    wb(m, a0, (uint8_t)d1);
    wb(m, a0 + 1, (uint8_t)(d0 >> 1));
    int16_t h = sw(rw(m, a0 - 2));
    if (h < 0)
        h = (int16_t)-h;
    uint16_t d2 = (uint16_t)(d1 + h);
    wb(m, a0 + 2, (uint8_t)d2);
    uint8_t b = rb(m, a0 + 3) & (uint8_t)~7;
    if (d1 & 0x100) b |= 4;
    if (d2 & 0x100) b |= 2;
    if (d0 & 1) b |= 1;
    wb(m, a0 + 3, b);
}

/* LAB_0E78 : sprite n en (x, y), et son jumeau s'il est attaché */
static void sprite_move(MogCombat *m, uint16_t n, uint16_t x, uint16_t y)
{
    ww(m, MOG_v_SpritePair, 0);
    uint32_t a0 = rl(m, MOG_t_SpriteData + (uint32_t)(n << 2)) + 2;
    if (sw(rw(m, a0 - 2)) < 0) {
        ww(m, MOG_v_SpritePair, (uint16_t)(n & 1 ? n - 1 : n + 1));
        ww(m, MOG_v_SpriteX, x);
        ww(m, MOG_v_SpriteY, y);
    }
    sprite_ctl(m, a0, x, y);
    uint16_t pair = rw(m, MOG_v_SpritePair);                /* LAB_0E80 */
    if (pair) {
        a0 = rl(m, MOG_t_SpriteData + (uint32_t)(pair << 2)) + 2;
        ww(m, MOG_v_SpritePair, 0);
        sprite_ctl(m, a0, rw(m, MOG_v_SpriteX), rw(m, MOG_v_SpriteY));
    }
}

/* LAB_0E85 : sprite n = données a0 (et a2 pour le jumeau) dans la copper
 * list a1 (registres SPRxPT) */
static void sprite_set(MogCombat *m, uint16_t n, uint32_t a0, uint32_t a1, uint32_t a2)
{
    ww(m, MOG_v_SpritePair, 0);
    uint32_t a1_saved = a1;
    if (sw(rw(m, a0)) < 0)
        ww(m, MOG_v_SpritePair, (uint16_t)(n & 1 ? n - 1 : n + 1));
    for (;;) {                                          /* LAB_0E88 */
        wl(m, MOG_t_SpriteData + (uint32_t)(n << 2), a0);
        a0 += 2;
        uint16_t reg = (uint16_t)((n << 2) + 0x120);
        uint32_t a = a1;
        while (rw(m, a) != reg)
            a += 4;
        a += 2;
        ww(m, a, (uint16_t)(a0 >> 16));
        ww(m, a + 4, (uint16_t)a0);
        uint16_t pair = rw(m, MOG_v_SpritePair);
        if (!pair)
            return;
        ww(m, MOG_v_SpritePair, 0);
        n = pair;
        a1 = a1_saved;
        a0 = a2;
    }
}

/* LAB_0575 : pointeur montré, mené par l'interruption d'image (LAB_057D) */
static void pointer_on(MogCombat *m)
{
    if (rw(m, MOG_v_PointerHidden))
        return;
    ww(m, MOG_v_PointerHidden, 1);
    sprite_set(m, 0, rl(m, MOG_v_PointerSprite), COPPER_SPRITES, rl(m, MOG_v_PointerSprite2));   /* LAB_0E77 */
    sprite_move(m, 0, rw(m, MOG_v_PointerX), rw(m, MOG_v_PointerY));
    ww(m, MOG_v_PointerOn, 1);
    uint32_t a0 = MOG_t_VblTasks;                         /* serveur d'interruption */
    while (rl(m, a0))
        a0 += 4;
    wl(m, a0, MOG_Vbl_Pointer);
    wl(m, MOG_v_PointerHook, a0);
}

/* LAB_057B : pointeur caché */
static void pointer_off(MogCombat *m)
{
    if (!rw(m, MOG_v_PointerHidden))
        return;
    ww(m, MOG_v_PointerHidden, 0);
    sprite_set(m, 0, MOG_t_SpriteEmpty, COPPER_SPRITES, 0);  /* LAB_0E76 */
    wl(m, rl(m, MOG_v_PointerHook), 0);
}

void mog_screen_vbl(MogCombat *m)
{
    if (!rw(m, MOG_v_PointerHidden))
        return;
    /* LAB_057D : joystick du chevalier LAB_068B (11 = 1 : port 0) */
    ww(m, MOG_v_PointerOn, 1);
    ww(m, MOG_v_Joy0, m->joy[0]);                     /* LAB_00EE */
    ww(m, MOG_v_Joy1, m->joy[1]);
    uint16_t d1 = m->joy[1];
    if (rb(m, rl(m, MOG_v_ScreenKnight) + 11) == 1)
        d1 = m->joy[0];
    uint16_t x = rw(m, MOG_v_PointerX), y = rw(m, MOG_v_PointerY);
    if (d1 & 1) x = (uint16_t)(x + 2);
    if (d1 & 2) x = (uint16_t)(x - 2);
    if (d1 & 4) y = (uint16_t)(y + 2);
    if (d1 & 8) y = (uint16_t)(y - 2);
    if (d1 & 0x10)
        ww(m, MOG_v_PointerOn, 0);
    if (!(sw(x) < 0x13B)) x = 0x13A;
    if (sw(x) < 0) x = 0;
    if (!(sw(y) < 0xC4)) y = 0xC3;
    if (sw(y) < 0) y = 0;
    ww(m, MOG_v_PointerX, x);
    ww(m, MOG_v_PointerY, y);
    sprite_move(m, 0, x, y);
}

void mog_pointer_boot(MogCombat *m)
{
    /* LAB_0572 : po.cel -> sprite 0 (SECSTRT_37 dans SECSTRT_43) */
    uint32_t cel = rl(m, MOG_v_DrawPlanes);
    mog_load_cel(VM, MOG_s_PoCel, cel);
    uint32_t a3 = MOG_b_PointerSprite;
    wl(m, MOG_v_PointerSpriteBoot, a3);
    uint32_t a1 = rl(m, cel + 2) + rl(m, cel + 10);
    uint16_t h = rw(m, cel + 16);
    uint32_t a2 = a1 + (uint32_t)(uint16_t)(h * 2);
    if (!(rb(m, cel + 19) & 8)) {
        ww(m, a3, h);
        wl(m, a3 + 2, 0);
        a3 += 6;
        for (uint32_t i = 0; i < h; i++, a3 += 4) {
            ww(m, a3, rw(m, a1 + 2 * i));
            ww(m, a3 + 2, rw(m, a2 + 2 * i));
        }
        /* CLR.L de fin : avec po.cel (18 lignes), 2 octets de plus que les
         * 80 de SECSTRT_43. Sur l'Amiga, ce bloc de mémoire chip est à part ;
         * ici les sections se suivent et SECSTRT_44 (voie 0 du son) serait
         * écrasée : on s'arrête à la fin du bloc. */
        for (uint32_t i = 0; i < 4 && a3 + i < MOG_b_PointerSprite + 80; i++)
            wb(m, a3 + i, 0);
    }
    wl(m, MOG_v_PointerSprite, rl(m, MOG_v_PointerSpriteBoot));
    wl(m, MOG_v_PointerSprite2, rl(m, MOG_v_PointerSprite2Boot));
}

/* ------------------------------------------------------------------ */
/* Zones                                                               */
/* ------------------------------------------------------------------ */

/* LAB_044E : zones et modèle LAB_0A58 effacés */
static void clear_zones(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_b_Obstacles);
    for (uint32_t i = 0; i < 0x960; i++)
        wb(m, a0 + i, 0);
    for (uint32_t i = 0; i < 24; i++)
        wb(m, MOG_t_ZoneTemplate + i, 0);
}

/* LAB_0448 : le modèle LAB_0A58 copié dans la première zone libre */
static void add_zone(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_b_Obstacles);                /* LAB_044B */
    int i;
    for (i = 0; i < 98 && rw(m, a0 + 4); i++)
        a0 += 24;
    if (i == 98)
        return;
    for (uint32_t k = 0; k < 24; k++)
        wb(m, a0 + k, rb(m, MOG_t_ZoneTemplate + k));
}

/* LAB_0451 : zone sous le point (x, y) : son texte est montré ; renvoie
 * la zone (0 : aucune) */
static uint32_t zone_at(MogCombat *m, uint16_t x, uint16_t y)
{
    ww(m, MOG_v_HoverX, x);
    ww(m, MOG_v_HoverY, y);
    for (uint32_t a0 = rl(m, MOG_b_Obstacles); rw(m, a0 + 4); a0 += 24) {
        int d5 = mog_span(x, (uint16_t)(x + 1), rw(m, a0 + 12),
                          (uint16_t)(rw(m, a0 + 12) + rw(m, a0 + 4)));
        d5 += mog_span(y, (uint16_t)(y + 1), rw(m, a0 + 14),
                       (uint16_t)(rw(m, a0 + 14) + rw(m, a0 + 6)));
        if (d5 != 2)
            continue;
        if (rl(m, a0 + 8))
            mog_text_records(m, rl(m, a0 + 8));
        return a0;
    }
    return 0;
}

/* LAB_051A : zone de l'icône dessinée (LAB_0681-LAB_0688) */
static void icon_zone(MogCombat *m)
{
    uint32_t a1 = MOG_t_ZoneTemplate;
    uint32_t e = rl(m, MOG_v_IconCel) + (uint32_t)(int32_t)sw((uint16_t)(rw(m, MOG_v_IconFrame) * 10));
    ww(m, a1 + 4, rw(m, e + 14));
    ww(m, a1 + 6, rw(m, e + 16));
    ww(m, a1 + 12, (uint16_t)(rw(m, MOG_v_IconX) + rw(m, MOG_v_PanelX)));
    ww(m, a1 + 14, rw(m, MOG_v_IconY));
    uint32_t d0 = rl(m, MOG_v_IconId);
    wl(m, a1 + 16, d0);
    ww(m, a1 + 22, rw(m, MOG_v_IconSlot));
    ww(m, a1 + 20, rw(m, MOG_v_IconKind));
    wl(m, a1 + 8, rl(m, MOG_v_IconTexts) + (uint32_t)(uint16_t)(d0 >> 2) * 14u);
    add_zone(m);
}

/* LAB_0516 / LAB_0517 : d7 icônes LAB_0681 en (LAB_0682, LAB_0683), pas
 * LAB_0684, chacune avec sa zone */
static void icons(MogCombat *m, uint32_t d7, int check_kind)
{
    if (check_kind && KIND == 3)
        ww(m, MOG_v_IconKind, 0x0C);
    ww(m, MOG_v_BlitByCpu, 1);
    if (!(uint16_t)d7)
        return;
    for (uint32_t i = (uint16_t)(d7 - 1) + 1u; i > 0; i--) {
        mog_draw_cel(VM, &m->blt, rl(m, MOG_v_IconCel), rw(m, MOG_v_IconFrame),
                     (uint16_t)(rw(m, MOG_v_IconX) + rw(m, MOG_v_PanelX)), rw(m, MOG_v_IconY));
        icon_zone(m);
        ww(m, MOG_v_IconX, (uint16_t)(rw(m, MOG_v_IconX) + rw(m, MOG_v_IconStep)));
    }
}

/* Réglage des variables d'icône */
static void icon_set(MogCombat *m, uint16_t f, uint16_t x, uint16_t y, uint16_t step,
                     uint32_t id, uint16_t kind, uint16_t slot)
{
    ww(m, MOG_v_IconFrame, f);
    ww(m, MOG_v_IconX, x);
    ww(m, MOG_v_IconY, y);
    ww(m, MOG_v_IconStep, step);
    wl(m, MOG_v_IconId, id);
    ww(m, MOG_v_IconKind, kind);
    ww(m, MOG_v_IconSlot, slot);
}

/* ------------------------------------------------------------------ */
/* Panneaux                                                            */
/* ------------------------------------------------------------------ */

/* LAB_0523 : zone « sortie » (identifiant 7) du grand bouton en (d1, d2) */
static void exit_zone(MogCombat *m, uint16_t d1, uint16_t d2)
{
    uint32_t a0 = MOG_t_ZoneTemplate;
    ww(m, a0 + 12, (uint16_t)(d1 + rw(m, MOG_v_PanelX)));
    ww(m, a0 + 14, d2);
    ww(m, a0 + 4, 0x19);
    ww(m, a0 + 6, 0x75);
    wl(m, a0 + 8, MOG_t_ExitZoneText);
    wl(m, a0 + 16, 7);
    ww(m, a0 + 20, 0);
    ww(m, a0 + 22, 0);
    add_zone(m);
}

/* LAB_04ED : liste d'icônes (frame, x, y, retournée), $FFFF à la fin */
static void draw_list(MogCombat *m, uint32_t a1)
{
    for (;;) {
        uint16_t d0 = rw(m, a1);
        if (sw(d0) < 0)
            return;
        uint16_t d1 = rw(m, a1 + 2), d2 = rw(m, a1 + 4);
        if (d0 == 0x1A)
            exit_zone(m, d1, d2);
        uint16_t d3 = rw(m, a1 + 6);
        a1 += 8;
        uint32_t cel = rl(m, MOG_v_IconCel);
        uint32_t e = cel + (uint32_t)(int32_t)sw((uint16_t)(d0 * 10));
        int original = rb(m, e + 18) == 1;
        if (original ? d3 != 0 : d3 == 0)
            ix_flip_frame(&m->eng, cel, d0);            /* Cel_FlipFrame */
        mog_draw_cel(VM, &m->blt, cel, d0, (uint16_t)(d1 + rw(m, MOG_v_PanelX)), d2);
    }
}

/* LAB_04EA : fond (LAB_05C0 effacé) et cadres des panneaux */
static void draw_frames(MogCombat *m)
{
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    mog_clear_screen(m, rl(m, MOG_v_BgPlanes));
    ww(m, MOG_v_PanelX, 0);
    uint32_t a1 = MOG_t_FramesRight;
    for (uint32_t a2 = MOG_t_PanelFrames; sw(rw(m, a2)) >= 0; a2 += 2)
        if (rw(m, a2) == (uint16_t)KIND) {
            ww(m, MOG_v_PanelX, 0x4A);
            a1 = MOG_t_FramesLeft;
            break;
        }
    draw_list(m, a1);
}

/* Numéro (LAB_0442) écrit en (x + LAB_0985, y) */
static void number_at(MogCombat *m, uint32_t v, uint16_t x, uint16_t y)
{
    mog_number(m, v, MOG_b_ScreenNumber);
    mog_text(m, MOG_b_ScreenNumber, (uint16_t)(x + rw(m, MOG_v_PanelX)), y, 0);
}

/* LAB_04FE : inventaire rl(LAB_0632) (24 octets) en icônes */
static void draw_inventory(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_v_LoadPtr);
    uint32_t d7 = rb(m, a0 + 2);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 9, 0x57, 0xA4, 0x10, 0x4C, 5, 2);
        icons(m, d7, 1);
    }
    a0 = rl(m, MOG_v_LoadPtr);                           /* LAB_0500 */
    d7 = rb(m, a0 + 6);
    if (d7) {
        if (!(d7 < 4)) d7 = 3;
        icon_set(m, 3, 0x21, 0x95, 0x0C, 0x50, 1, 6);
        icons(m, d7, 1);
    }
    a0 = rl(m, MOG_v_LoadPtr);                           /* LAB_0502 */
    d7 = rb(m, a0 + 8);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 0x0A, 0x45, 0x85, 0x14, 0x54, 1, 8);
        icons(m, d7, 1);
    }
    ww(m, MOG_v_IconSlot, 0x14);                          /* LAB_0504 : clés */
    wl(m, MOG_v_IconId, 0x44);
    a0 = rl(m, MOG_v_LoadPtr);
    uint8_t keys = rb(m, a0 + 20);
    if (keys) {
        static const struct { uint16_t f, x, kind; } k[4] = {
            { 5, 0x4C, 0x11 }, { 6, 0x5E, 0x21 }, { 7, 0x70, 0x41 }, { 8, 0x82, 0x81 },
        };
        for (int i = 0; i < 4; i++)
            if (keys & (1 << i)) {
                ww(m, MOG_v_IconFrame, k[i].f);
                ww(m, MOG_v_IconX, k[i].x);
                ww(m, MOG_v_IconY, 0x6F);
                ww(m, MOG_v_IconKind, k[i].kind);
                icons(m, 1, 0);
            }
    }
    ww(m, MOG_v_IconSlot, 0x16);                          /* LAB_0508 : sorts */
    ww(m, MOG_v_IconX, 0x67);
    ww(m, MOG_v_IconY, 0x6F);
    a0 = rl(m, MOG_v_LoadPtr);
    uint8_t spells = rb(m, a0 + 22);
    if (spells) {
        static const struct { uint16_t f; uint32_t id; uint16_t kind; } k[4] = {
            { 2, 0x38, 0x11 }, { 1, 0x3C, 0x21 }, { 0, 0x40, 0x41 }, { 0, 0x38, 0x81 },
        };
        for (int i = 0; i < 4; i++)
            if (spells & (1 << i)) {
                ww(m, MOG_v_IconFrame, k[i].f);
                wl(m, MOG_v_IconId, k[i].id);
                ww(m, MOG_v_IconKind, k[i].kind);
                icons(m, 1, 0);
            }
    }
    a0 = rl(m, MOG_v_LoadPtr);                           /* LAB_050C : potions */
    d7 = rb(m, a0);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 4, 0x1F, 0xA5, 0x0D, 0x48, 5, 0);
        icons(m, d7, 1);
    }
    /* LAB_050E : objets 10 à 18 (cinq mots), deux icônes au plus */
    a0 = rl(m, MOG_v_LoadPtr);
    uint16_t d1 = 0x1E, d2 = 0xB7;
    ww(m, MOG_v_InvIconX, d1);
    ww(m, MOG_v_IconSlot, 0x0A);
    ww(m, MOG_v_IconKind, 5);
    wl(m, MOG_v_IconId, 0x58);
    uint32_t a1 = a0 + 10;
    uint16_t d0 = 0x0B;
    ww(m, MOG_v_InvIconFrame, 0x0B);
    for (int i = 0; i < 5; i++, a1 += 2) {
        int16_t d5 = (int8_t)rb(m, a1);
        if (d5) {
            ww(m, MOG_v_IconStep, 0x0B);
            /* LAB_0511 */
            if (!(d5 < 3))
                d5 = 2;
            d5--;
            for (;;) {                                  /* LAB_0513 */
                ww(m, MOG_v_IconFrame, d0);
                ww(m, MOG_v_IconX, d1);
                ww(m, MOG_v_IconY, d2);
                icons(m, 1, 1);
                ww(m, MOG_v_IconX, (uint16_t)(d1 + rw(m, MOG_v_IconStep)));
                ww(m, MOG_v_IconStep, 4);
                d0 = 0x10;
                d1 = rw(m, MOG_v_IconX);
                if (--d5 < 0)
                    break;
            }
            ww(m, MOG_v_InvIconX, (uint16_t)(rw(m, MOG_v_InvIconX) + 0x19));
            d1 = rw(m, MOG_v_InvIconX);
        }
        ww(m, MOG_v_InvIconFrame, (uint16_t)(rw(m, MOG_v_InvIconFrame) + 1));
        d0 = rw(m, MOG_v_InvIconFrame);
        ww(m, MOG_v_IconSlot, (uint16_t)(rw(m, MOG_v_IconSlot) + 2));
        wl(m, MOG_v_IconId, rl(m, MOG_v_IconId) + 4);
    }
}

/* Barre de caractéristique (LAB_0516) */
static void stat_icons(MogCombat *m, uint32_t d7, uint16_t f, uint16_t x, uint16_t y,
                       uint16_t step, uint32_t id, uint16_t kind, uint16_t slot)
{
    icon_set(m, f, x, y, step, id, kind, slot);
    icons(m, d7, 1);
}

/* LAB_04F8 : panneau du chevalier rl(LAB_0632) : nom, force, endurance,
 * constitution, expérience, or, caractéristiques, vies, PV, dagues,
 * arme, armure, puis son inventaire */
static void draw_knight(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_v_LoadPtr);
    mog_knight_defence(m, a0);
    mog_knight_hp(m, a0);
    mog_text(m, rl(m, a0 + 108), (uint16_t)(0x3C + rw(m, MOG_v_PanelX)), 0x18, 0);
    a0 = rl(m, MOG_v_LoadPtr);
    stat_icons(m, rb(m, a0 + 70), 0x26, 0x29, 0x23, 0, 0, 3, 0x46);
    stat_icons(m, rb(m, a0 + 72), 0x28, 0x29, 0x31, 0, 4, 3, 0x48);
    stat_icons(m, rb(m, a0 + 71), 0x27, 0x29, 0x2A, 0, 8, 3, 0x47);
    stat_icons(m, 1, 0x2A, 0x59, 0x2A, 0, 0x10, 2, 0x4A);
    draw_list(m, MOG_t_KnightPanelIcons);
    a0 = rl(m, MOG_v_LoadPtr);
    number_at(m, rw(m, a0 + 78), 0x70, 0x23);
    a0 = rl(m, MOG_v_LoadPtr);
    wl(m, MOG_b_ScreenNumber, 0);
    number_at(m, rw(m, a0 + 74), 0x70, 0x2A);
    ww(m, MOG_v_StatOffset, 0x46);
    ww(m, MOG_v_StatY, 0x23);
    for (int i = 0; i < 3; i++) {                       /* LAB_04F9 */
        a0 = rl(m, MOG_v_LoadPtr);
        uint32_t v = rb(m, a0 + (uint32_t)(int32_t)sw(rw(m, MOG_v_StatOffset)));
        wl(m, MOG_b_ScreenNumber, 0);
        number_at(m, v, 0x3F, rw(m, MOG_v_StatY));
        ww(m, MOG_v_StatOffset, (uint16_t)(rw(m, MOG_v_StatOffset) + 1));
        ww(m, MOG_v_StatY, (uint16_t)(rw(m, MOG_v_StatY) + 7));
    }
    a0 = rl(m, MOG_v_LoadPtr);
    int32_t lives = (int8_t)rb(m, a0 + 73);
    if (lives < 0)
        lives = 5;
    uint16_t f = (uint16_t)(0x11 + rw(m, MOG_v_LivesIconAlt));
    if (rb(m, a0 + 82))
        f = (uint16_t)(f + 2);
    stat_icons(m, (uint32_t)lives, f, 0x29, 0x3B, 0x13, 0x0C, 3, 0x49);
    a0 = rl(m, MOG_v_LoadPtr);
    wl(m, MOG_b_ScreenNumber, 0);
    wl(m, MOG_v_Screen0988, 0);
    wl(m, MOG_v_Screen0989, 0);
    uint32_t e = mog_number(m, rw(m, a0 + 80), MOG_b_ScreenNumber);
    wb(m, e, 0x2F);
    mog_number(m, rw(m, rl(m, MOG_v_LoadPtr) + 84), e + 1);
    mog_text(m, MOG_b_ScreenNumber, (uint16_t)(0x70 + rw(m, MOG_v_PanelX)), 0x31, 0);
    a0 = rl(m, MOG_v_LoadPtr);
    stat_icons(m, rb(m, a0 + 76), 0x15, 0x26, 0x4E, 0x0A, 0x14, 3, 0x4C);
    a0 = rl(m, MOG_v_LoadPtr);
    uint32_t w = rl(m, a0 + 88);
    if ((int32_t)w > 0x19)
        w = 0x19;
    stat_icons(m, 1, (uint16_t)w, 0x2B, 0x5F, 0, ((w - 0x16) << 2) + 0x28, 2, 0x58);
    a0 = rl(m, MOG_v_LoadPtr);
    uint32_t ar = rl(m, a0 + 92);
    stat_icons(m, 1, (uint16_t)ar, 0x1D, 0x70, 0, ((ar - 0x1B) << 2) + 0x18, 2, 0x5C);
    wl(m, MOG_v_LoadPtr, rl(m, rl(m, MOG_v_LoadPtr) + 96));
    draw_inventory(m);
}

/* LAB_051F : épée magique de l'inventaire a0 */
static void magic_sword(MogCombat *m, uint32_t a0)
{
    if (!rb(m, a0 + 4))
        return;
    stat_icons(m, 1, 0x19, 0x2B, 0x5F, 0, 0x34, 1, 4);
}

/* LAB_0521 : or (d0) de la créature ou du dragon */
static void creature_gold(MogCombat *m, uint16_t d0)
{
    stat_icons(m, 1, 0x25, 0x57, 0x40, 0, 0x10, 2, 0x4A);
    wl(m, MOG_b_ScreenNumber, 0);
    uint32_t e = mog_number(m, d0, MOG_b_ScreenNumber);
    static const char gp[] = " gp.";
    for (int i = 0; i < 5; i++)
        wb(m, e + (uint32_t)i, (uint8_t)gp[i]);
    mog_text(m, MOG_b_ScreenNumber, (uint16_t)(0x5C + rw(m, MOG_v_PanelX)), 0x5B, 0);
}

/* LAB_051B : la créature LAB_08C6 (butin sur la carte) */
static void draw_creature(MogCombat *m)
{
    wl(m, MOG_v_IconTexts, MOG_b_ZoneTextsRight);
    ww(m, MOG_v_PanelX, 0x96);
    ww(m, MOG_v_BlitByCpu, 1);
    uint32_t cel = rl(m, MOG_v_IconCel);
    uint16_t off = rw(m, MOG_v_PanelX);
    mog_draw_cel(VM, &m->blt, cel, 0x1F, (uint16_t)(0x3A + off), 0x2E);
    mog_draw_cel(VM, &m->blt, cel, 0x20, (uint16_t)(0x4C + off), 0x21);
    mog_draw_cel(VM, &m->blt, cel, 0x21, (uint16_t)(0x3A + off), 0x3C);
    uint32_t a0 = rl(m, MOG_v_Lair);
    if (rw(m, a0 + 8))
        creature_gold(m, rw(m, a0 + 8));
    magic_sword(m, rl(m, rl(m, MOG_v_Lair)));
    wl(m, MOG_v_LoadPtr, rl(m, MOG_v_ScreenOtherInv));
    draw_inventory(m);
}

/* LAB_051D : le dragon (trésor) */
static void draw_dragon(MogCombat *m)
{
    wl(m, MOG_v_LoadPtr, rl(m, MOG_v_DragonObj + 96));
    draw_inventory(m);
    magic_sword(m, rl(m, MOG_v_LoadPtr));
    uint16_t g = rw(m, MOG_v_DragonObj + 74);
    if (g)
        creature_gold(m, g);
}

/* LAB_0522 : la boutique (armures, armes, dagues) */
static void draw_shop(MogCombat *m)
{
    stat_icons(m, 1, 0x1C, 0x17, 0x91, 0, 0x1C, 0x1A, 0x5C);
    stat_icons(m, 1, 0x1D, 0x40, 0x91, 0, 0x20, 0x2A, 0x5C);
    stat_icons(m, 1, 0x1E, 0x6A, 0x90, 0, 0x24, 0x4A, 0x5C);
    stat_icons(m, 1, 0x17, 0x32, 0x6B, 0, 0x2C, 0x1A, 0x58);
    stat_icons(m, 1, 0x18, 0x2F, 0x7D, 0, 0x30, 0x2A, 0x58);
    stat_icons(m, 13, 0x15, 0x20, 0x59, 9, 0x14, 0x0A, 0x4C);
}

/* LAB_0524 : bouton « suivant » (autre chevalier, identifiant LAB_09EF) */
static void next_button(MogCombat *m)
{
    mog_draw_cel(VM, &m->blt, rl(m, MOG_v_IconCel), 0x2C, 0x92, 0x47);
    uint32_t a0 = MOG_t_ZoneTemplate;
    ww(m, a0 + 12, 0x92);
    ww(m, a0 + 14, 0x47);
    ww(m, a0 + 4, 0x14);
    ww(m, a0 + 6, 0x14);
    wl(m, a0 + 8, MOG_t_NextKnightText);
    wl(m, a0 + 16, 0);
    ww(m, a0 + 20, 0);
    ww(m, a0 + 22, 0);
    add_zone(m);
}

/* LAB_04E1 : couleurs des chevaliers des deux panneaux (LAB_09F1) */
static void panel_colours(MogCombat *m)
{
    uint32_t a1 = rl(m, MOG_v_ScreenKnight), a0 = MOG_t_PanelKnightColours;
    for (int n = 0; n < 2; n++) {
        static const uint16_t c[5][2] = {
            { 0x03F, 0x028 }, { 0xFB0, 0xB60 }, { 0x4C3, 0x160 }, { 0xF00, 0x800 }, { 0x027, 0x003 },
        };
        uint32_t k = rl(m, a1 + 54);
        const uint16_t *v = c[k <= 3 ? k : 4];
        ww(m, a0, v[0]);
        ww(m, a0 + 2, v[1]);
        a0 += 4;
        uint32_t kind = KIND;
        if (kind != 1 && kind != 8 && kind != 0x0B)
            return;
        a1 = rl(m, MOG_v_ScreenOther);
    }
}

/* ------------------------------------------------------------------ */
/* Textes des zones                                                    */
/* ------------------------------------------------------------------ */

/* LAB_0588 : tables des textes montrés au survol (une par genre d'écran) */
static void text_tables(MogCombat *m)
{
    wl(m, MOG_t_TextsLoot + 0, MOG_s_Strength);
    wl(m, MOG_t_TextsLoot + 4, MOG_s_Endurance);
    wl(m, MOG_t_TextsLoot + 8, MOG_s_Constitution);
    wl(m, MOG_t_TextsLoot + 12, MOG_s_LifePointsLeft);
    wl(m, MOG_t_TextsLoot + 16, MOG_s_TakeGold);
    wl(m, MOG_t_TextsLoot + 20, MOG_s_TakeDaggers);
    wl(m, MOG_t_TextsLoot + 24, MOG_s_PaddedArmour);
    wl(m, MOG_t_TextsLoot + 28, MOG_s_TakeChainmail);
    wl(m, MOG_t_TextsLoot + 32, MOG_s_TakePlateArmour);
    wl(m, MOG_t_TextsLoot + 36, MOG_s_TakeBattleArmour);
    wl(m, MOG_t_TextsLoot + 40, MOG_s_LongSword);
    wl(m, MOG_t_TextsLoot + 44, MOG_s_TakeBroadSword);
    wl(m, MOG_t_TextsLoot + 48, MOG_s_TakeClaymoreSword);
    wl(m, MOG_t_TextsLoot + 52, MOG_s_TakeSwordOfSharpness);
    wl(m, MOG_t_TextsLoot + 56, MOG_s_TakeMoonstone);
    wl(m, MOG_t_TextsLoot + 60, MOG_s_TakeMoonstone);
    wl(m, MOG_t_TextsLoot + 64, MOG_s_TakeMoonstone);
    wl(m, MOG_t_TextsLoot + 68, MOG_s_TakeKeyToTheValley);
    wl(m, MOG_t_TextsLoot + 72, MOG_s_TakePotionOfHealing);
    wl(m, MOG_t_TextsLoot + 76, MOG_s_TakeGemOfSeeing);
    wl(m, MOG_t_TextsLoot + 80, MOG_s_TakeRingOfProtection);
    wl(m, MOG_t_TextsLoot + 84, MOG_s_TakeTalismanOfTheWyrm);
    wl(m, MOG_t_TextsLoot + 88, MOG_s_TakeScrollOfHaste);
    wl(m, MOG_t_TextsLoot + 96, MOG_s_TakeScrollOfAquisition);
    wl(m, MOG_t_TextsLoot + 92, MOG_s_TakeScrollOfTheHawk);
    wl(m, MOG_t_TextsLoot + 100, MOG_s_TakeScrollOfTheWyrm);
    wl(m, MOG_t_TextsLoot + 104, MOG_s_TakeScrollOfProtection);
    wl(m, MOG_t_TextsTrain + 0, MOG_s_IncreaseStrength);
    wl(m, MOG_t_TextsTrain + 4, MOG_s_IncreaseEndurance);
    wl(m, MOG_t_TextsTrain + 8, MOG_s_IncreaseConstitution);
    wl(m, MOG_t_TextsTrain + 12, MOG_s_LifePoints);
    wl(m, MOG_t_TextsTrain + 16, MOG_s_Gold);
    wl(m, MOG_t_TextsTrain + 20, MOG_s_Dagger);
    wl(m, MOG_t_TextsTrain + 36, MOG_s_BattleArmour);
    wl(m, MOG_t_TextsTrain + 32, MOG_s_PlateArmour);
    wl(m, MOG_t_TextsTrain + 28, MOG_s_ChainMail);
    wl(m, MOG_t_TextsTrain + 24, MOG_s_PaddedArmour);
    wl(m, MOG_t_TextsTrain + 48, MOG_s_ClaymoreSword);
    wl(m, MOG_t_TextsTrain + 40, MOG_s_LongSword);
    wl(m, MOG_t_TextsTrain + 52, MOG_s_SwordOfSharpness);
    wl(m, MOG_t_TextsTrain + 44, MOG_s_BroadSword);
    wl(m, MOG_t_TextsTrain + 56, MOG_s_NewMoonMoonstone);
    wl(m, MOG_t_TextsTrain + 60, MOG_s_FullMoonstone);
    wl(m, MOG_t_TextsTrain + 64, MOG_s_HalfMoonstone);
    wl(m, MOG_t_TextsTrain + 68, MOG_s_KeyToTheValley);
    wl(m, MOG_t_TextsTrain + 72, MOG_s_DrinkHealingPotion);
    wl(m, MOG_t_TextsTrain + 76, MOG_s_UseGemOfSeeing);
    wl(m, MOG_t_TextsTrain + 80, MOG_s_RingOfProtection);
    wl(m, MOG_t_TextsTrain + 84, MOG_s_TalismanOfTheWyrm);
    wl(m, MOG_t_TextsTrain + 88, MOG_s_CastScrollOfHaste);
    wl(m, MOG_t_TextsTrain + 96, MOG_s_CastScrollOfAquisition);
    wl(m, MOG_t_TextsTrain + 92, MOG_s_CastScrollOfTheHawk);
    wl(m, MOG_t_TextsTrain + 100, MOG_s_CastScrollOfTheWyrm);
    wl(m, MOG_t_TextsTrain + 104, MOG_s_CastScrollOfProtection);
    wl(m, MOG_t_TextsInventory + 0, MOG_s_Strength);
    wl(m, MOG_t_TextsInventory + 4, MOG_s_Endurance);
    wl(m, MOG_t_TextsInventory + 8, MOG_s_Constitution);
    wl(m, MOG_t_TextsInventory + 12, MOG_s_LifePoints);
    wl(m, MOG_t_TextsInventory + 16, MOG_s_Gold);
    wl(m, MOG_t_TextsInventory + 20, MOG_s_Dagger);
    wl(m, MOG_t_TextsInventory + 24, MOG_s_PaddedArmour);
    wl(m, MOG_t_TextsInventory + 28, MOG_s_ChainMail);
    wl(m, MOG_t_TextsInventory + 32, MOG_s_PlateArmour);
    wl(m, MOG_t_TextsInventory + 36, MOG_s_BattleArmour);
    wl(m, MOG_t_TextsInventory + 40, MOG_s_LongSword);
    wl(m, MOG_t_TextsInventory + 44, MOG_s_BroadSword);
    wl(m, MOG_t_TextsInventory + 48, MOG_s_ClaymoreSword);
    wl(m, MOG_t_TextsInventory + 52, MOG_s_SwordOfSharpness);
    wl(m, MOG_t_TextsInventory + 56, MOG_s_NewMoonMoonstone);
    wl(m, MOG_t_TextsInventory + 60, MOG_s_FullMoonstone);
    wl(m, MOG_t_TextsInventory + 64, MOG_s_HalfMoonstone);
    wl(m, MOG_t_TextsInventory + 68, MOG_s_KeyToTheValley);
    wl(m, MOG_t_TextsInventory + 72, MOG_s_DrinkHealingPotion);
    wl(m, MOG_t_TextsInventory + 76, MOG_s_UseGemOfSeeing);
    wl(m, MOG_t_TextsInventory + 80, MOG_s_RingOfProtection);
    wl(m, MOG_t_TextsInventory + 84, MOG_s_TalismanOfTheWyrm);
    wl(m, MOG_t_TextsInventory + 88, MOG_s_CastScrollOfHaste);
    wl(m, MOG_t_TextsInventory + 96, MOG_s_CastScrollOfAquisition);
    wl(m, MOG_t_TextsInventory + 92, MOG_s_CastScrollOfTheHawk);
    wl(m, MOG_t_TextsInventory + 100, MOG_s_CastScrollOfTheWyrm);
    wl(m, MOG_t_TextsInventory + 104, MOG_s_CastScrollOfProtection);
    wl(m, MOG_t_TextsDefault + 0, MOG_s_Strength);
    wl(m, MOG_t_TextsDefault + 4, MOG_s_Endurance);
    wl(m, MOG_t_TextsDefault + 8, MOG_s_Constitution);
    wl(m, MOG_t_TextsDefault + 12, MOG_s_LifePoints);
    wl(m, MOG_t_TextsDefault + 16, MOG_s_Gold);
    wl(m, MOG_t_TextsDefault + 20, MOG_s_Dagger);
    wl(m, MOG_t_TextsDefault + 24, MOG_s_PaddedArmour);
    wl(m, MOG_t_TextsDefault + 28, MOG_s_ChainMail);
    wl(m, MOG_t_TextsDefault + 32, MOG_s_PlateArmour);
    wl(m, MOG_t_TextsDefault + 36, MOG_s_BattleArmour);
    wl(m, MOG_t_TextsDefault + 40, MOG_s_LongSword);
    wl(m, MOG_t_TextsDefault + 44, MOG_s_BroadSword);
    wl(m, MOG_t_TextsDefault + 48, MOG_s_ClaymoreSword);
    wl(m, MOG_t_TextsDefault + 52, MOG_s_SwordOfSharpness);
    wl(m, MOG_t_TextsDefault + 56, MOG_s_NewMoonMoonstone);
    wl(m, MOG_t_TextsDefault + 60, MOG_s_FullMoonstone);
    wl(m, MOG_t_TextsDefault + 64, MOG_s_HalfMoonstone);
    wl(m, MOG_t_TextsDefault + 68, MOG_s_KeyToTheValley);
    wl(m, MOG_t_TextsDefault + 72, MOG_s_PotionOfHealing);
    wl(m, MOG_t_TextsDefault + 76, MOG_s_GemOfSeeing);
    wl(m, MOG_t_TextsDefault + 80, MOG_s_RingOfProtection);
    wl(m, MOG_t_TextsDefault + 84, MOG_s_TalismanOfTheWyrm);
    wl(m, MOG_t_TextsDefault + 88, MOG_s_ScrollOfHaste);
    wl(m, MOG_t_TextsDefault + 96, MOG_s_ScrollOfAquisition);
    wl(m, MOG_t_TextsDefault + 92, MOG_s_ScrollOfTheHawk);
    wl(m, MOG_t_TextsDefault + 100, MOG_s_ScrollOfTheWyrm);
    wl(m, MOG_t_TextsDefault + 104, MOG_s_ScrollOfProtection);
    for (uint32_t i = 0; i < 27; i++)                   /* LAB_0589 */
        wl(m, MOG_t_TextsScreen3 + 4 * i, MOG_s_Space);
    wl(m, MOG_t_TextsScreen3 + 56, MOG_s_NewMoonMoonstone);
    wl(m, MOG_t_TextsScreen3 + 60, MOG_s_FullMoonstone);
    wl(m, MOG_t_TextsScreen3 + 64, MOG_s_HalfMoonstone);
    wl(m, MOG_t_TextsScreen3 + 68, MOG_s_KeyToTheValley);
    wl(m, MOG_t_TextsScreen3 + 72, MOG_s_OfferPotionOfHealing);
    wl(m, MOG_t_TextsScreen3 + 76, MOG_s_OfferGemOfSeeing);
    wl(m, MOG_t_TextsScreen3 + 80, MOG_s_OfferRingOfProtection);
    wl(m, MOG_t_TextsScreen3 + 84, MOG_s_OfferTalismanOfTheWyrm);
    wl(m, MOG_t_TextsScreen3 + 88, MOG_s_OfferScrollOfHaste);
    wl(m, MOG_t_TextsScreen3 + 96, MOG_s_OfferScrollOfAquisition);
    wl(m, MOG_t_TextsScreen3 + 92, MOG_s_OfferScrollOfTheHawk);
    wl(m, MOG_t_TextsScreen3 + 100, MOG_s_OfferScrollOfTheWyrm);
    wl(m, MOG_t_TextsScreen3 + 104, MOG_s_OfferScrollOfProtection);
    wl(m, MOG_t_TextsShop + 44, MOG_s_BuyBroadSwordFor10Gp);
    wl(m, MOG_t_TextsShop + 48, MOG_s_BuyClaymoreSwordFor25Gp);
    wl(m, MOG_t_TextsShop + 28, MOG_s_BuyChainmailFor30Gp);
    wl(m, MOG_t_TextsShop + 32, MOG_s_BuyPlateArmourFor50Gp);
    wl(m, MOG_t_TextsShop + 36, MOG_s_BuyBattleArmourFor75Gp);
    wl(m, MOG_t_TextsShop + 20, MOG_s_BuyADaggerFor2Gp);
    wl(m, MOG_t_TextsShop + 52, MOG_s_BuySwordOfSharpnessFor52Gp);
    wl(m, MOG_t_TextsShop + 68, MOG_s_BuyKeyFor12Gp);
    wl(m, MOG_t_TextsShop + 72, MOG_s_BuyPotionOfHealingFor20Gp);
    wl(m, MOG_t_TextsShop + 76, MOG_s_BuyGemOfSeeingFor32Gp);
    wl(m, MOG_t_TextsShop + 80, MOG_s_BuyRingOfProtectionFor40Gp);
    wl(m, MOG_t_TextsShop + 84, MOG_s_BuyTalismanFor52Gp);
    wl(m, MOG_t_TextsShop + 88, MOG_s_BuyScrollOfHasteFor36Gp);
    wl(m, MOG_t_TextsShop + 96, MOG_s_BuyScrollOfAquisitionFor52Gp);
    wl(m, MOG_t_TextsShop + 92, MOG_s_BuyScrollOfTheHawkFor52Gp);
    wl(m, MOG_t_TextsShop + 100, MOG_s_BuyScrollOfTheWyrmFor40Gp);
    wl(m, MOG_t_TextsShop + 104, MOG_s_BuyScrollOfProtectionFor24Gp);
    wl(m, MOG_t_TextsShop + 56, MOG_s_BuyMoonstoneFor20Gp);
    wl(m, MOG_t_TextsShop + 60, MOG_s_BuyMoonstoneFor20Gp);
    wl(m, MOG_t_TextsShop + 64, MOG_s_BuyMoonstoneFor20Gp);
    wl(m, MOG_t_TextsScreen6 + 52, MOG_s_SwordOfSharpness2);
    wl(m, MOG_t_TextsScreen6 + 68, MOG_s_SellKeyFor6Gp);
    wl(m, MOG_t_TextsScreen6 + 72, MOG_s_SellPotionOfHealingFor10Gp);
    wl(m, MOG_t_TextsScreen6 + 76, MOG_s_SellGemOfSeeingFor16Gp);
    wl(m, MOG_t_TextsScreen6 + 80, MOG_s_SellRingFor20Gp);
    wl(m, MOG_t_TextsScreen6 + 84, MOG_s_SellTalismanFor26Gp);
    wl(m, MOG_t_TextsScreen6 + 88, MOG_s_SellScrollOfHasteFor16Gp);
    wl(m, MOG_t_TextsScreen6 + 96, MOG_s_SellScrollOfAquisitionFor26G);
    wl(m, MOG_t_TextsScreen6 + 92, MOG_s_SellScrollOfTheHawkFor26Gp);
    wl(m, MOG_t_TextsScreen6 + 100, MOG_s_SellScrollOfTheWyrmFor20Gp);
    wl(m, MOG_t_TextsScreen6 + 104, MOG_s_SellScrollOfProtectionFor12G);
    wl(m, MOG_t_TextsScreen6 + 56, MOG_s_SellMoonstoneFor10Gp);
    wl(m, MOG_t_TextsScreen6 + 60, MOG_s_SellMoonstoneFor10Gp);
    wl(m, MOG_t_TextsScreen6 + 64, MOG_s_SellMoonstoneFor10Gp);
    ww(m, MOG_t_ShopPrices + 0, 0x0014);
    ww(m, MOG_t_ShopPrices + 2, 0x0020);
    ww(m, MOG_t_ShopPrices + 4, 0x0034);
    ww(m, MOG_t_ShopPrices + 6, 0x0028);
    ww(m, MOG_t_ShopPrices + 8, 0x0034);
    ww(m, MOG_t_ShopPrices + 10, 0x0024);
    ww(m, MOG_t_ShopPrices + 12, 0x0034);
    ww(m, MOG_t_ShopPrices + 14, 0x0034);
    ww(m, MOG_t_ShopPrices + 16, 0x0028);
    ww(m, MOG_t_ShopPrices + 18, 0x0018);
    ww(m, MOG_t_ShopPrices + 20, 0x000c);
    ww(m, MOG_t_ShopPrices + 22, 0x0014);}

/* Une table de 27 enregistrements de texte (chaîne, X 0, Y 3, drapeaux) */
static void fill_texts(MogCombat *m, uint32_t dst, uint32_t src, uint16_t flag)
{
    uint16_t fl = (uint16_t)(3 | (src != MOG_t_TextsDefault ? flag : 0));
    for (int i = 0; i < 27; i++, dst += 14, src += 4) {
        wl(m, dst, rl(m, src));
        ww(m, dst + 4, 0);
        ww(m, dst + 6, 3);
        ww(m, dst + 8, fl);
        wl(m, dst + 10, 0);
    }
}

/* LAB_058A : chevaliers et inventaires des deux panneaux, textes */
static void select_panels(MogCombat *m)
{
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    uint32_t a1 = rl(m, MOG_v_Combatants);
    wl(m, MOG_v_ScreenKnight, a1);
    wl(m, MOG_v_ScreenInventory, rl(m, a1 + 96));
    uint32_t kind = KIND;
    if (kind == 1 || kind == 8 || kind == 0x0B) {
        a1 = rl(m, MOG_v_Combatants + 4);
        wl(m, MOG_v_ScreenOther, a1);
        wl(m, MOG_v_ScreenOtherInv, rl(m, a1 + 96));
    }
    uint32_t a0 = MOG_t_TextsDefault;
    if (kind == 6) a0 = MOG_t_TextsScreen6;
    if (kind == 3) a0 = MOG_t_TextsScreen3;
    if (kind == 2) {
        a0 = MOG_t_TextsDefault;
        wl(m, MOG_v_ScreenOther, rl(m, MOG_v_Lair));
        wl(m, MOG_v_ScreenOtherInv, rl(m, rl(m, MOG_v_Lair)));
    }
    if (kind == 0x0A) {
        a0 = MOG_t_TextsDefault;
        wl(m, MOG_v_ScreenOther, MOG_v_DragonObj);
        wl(m, MOG_v_ScreenOtherInv, rl(m, MOG_v_DragonObj + 96));
    }
    if (kind == 9) a0 = MOG_t_TextsInventory;
    fill_texts(m, MOG_b_ZoneTextsLeft, a0, 0x10);
    /* expérience suffisante : les caractéristiques non au maximum
     * proposent leur amélioration (LAB_0692) */
    uint32_t k = rl(m, MOG_v_ScreenKnight);
    if (!(sw(rw(m, MOG_v_TrainCost)) > sw(rw(m, k + 78)))) {
        uint32_t t = MOG_b_ZoneTextsLeft, n = MOG_t_TextsTrain;
        for (int i = 0; i < 3; i++, t += 14, n += 4)
            if (rb(m, k + 0x46 + (uint32_t)i) != 5) {
                wl(m, t, rl(m, n));
                ww(m, t + 8, rw(m, t + 8) ^ 0x40);
            }
    }
    a0 = MOG_t_TextsDefault;
    if (kind == 0x0A || kind == 1) a0 = MOG_t_TextsLoot;
    if (kind == 2)
        a0 = rw(m, MOG_v_BootsOn) ? MOG_t_TextsDefault : MOG_t_TextsLoot;
    if (kind == 5 || kind == 6) a0 = MOG_t_TextsShop;
    if (kind == 8) a0 = MOG_t_TextsLoot;
    fill_texts(m, MOG_b_ZoneTextsRight, a0, 0x20);
}

/* ------------------------------------------------------------------ */
/* Écran                                                               */
/* ------------------------------------------------------------------ */

/* LAB_0419 : copie d'écran (5 plans) */
static void copy_screen(MogCombat *m, uint32_t a0, uint32_t a1)
{
    MogBlitter *b = &m->blt;
    wl(m, MOG_v_CopySrc, a0);
    wl(m, MOG_v_CopyDst, a1);
    for (int p = 0; p < 5; p++) {
        b->apt = rl(m, MOG_v_CopySrc);
        b->dpt = rl(m, MOG_v_CopyDst);
        b->amod = b->dmod = 0;
        b->afwm = b->alwm = 0xFFFF;
        b->con0 = 0x09F0;
        b->con1 = 0;
        mog_blitter_run(VM, b, 0x3214);
        wl(m, MOG_v_CopySrc, rl(m, MOG_v_CopySrc) + 0x1F40);
        wl(m, MOG_v_CopyDst, rl(m, MOG_v_CopyDst) + 0x1F40);
    }
}

/* LAB_04D4 : l'écran construit (panneaux, icônes, zones), puis montré */
static void build(MogCombat *m)
{
    for (;;) {
        ww(m, MOG_v_BlitByCpu, 1);
        clear_zones(m);                                 /* LAB_044E */
        draw_frames(m);                                 /* LAB_04EA */
        copy_screen(m, rl(m, MOG_v_BgPlanes), rl(m, MOG_v_DrawPlanes));
        mog_set_planes(m, rl(m, MOG_v_DrawPlanes));
        select_panels(m);                               /* LAB_058A */
        for (uint32_t i = 0; i < 0x2D0; i++)            /* LAB_03A7 */
            wb(m, MOG_b_RestoreA + i, 0xFF);
        wl(m, MOG_v_IconTexts, MOG_b_ZoneTextsLeft);
        wl(m, MOG_v_LoadPtr, rl(m, MOG_v_ScreenKnight));
        ww(m, MOG_v_PanelX, 0);
        ww(m, MOG_v_LivesIconAlt, 0);
        for (uint32_t a2 = MOG_t_PanelFrames; sw(rw(m, a2)) >= 0; a2 += 2)
            if (rw(m, a2) == (uint16_t)KIND) {
                ww(m, MOG_v_PanelX, 0x4A);
                break;
            }
        draw_knight(m);                                 /* LAB_04F8 */
        wl(m, MOG_v_IconTexts, MOG_b_ZoneTextsRight);
        ww(m, MOG_v_PanelX, 0x96);
        uint32_t kind = KIND;
        if (kind == 2) {
            draw_creature(m);                           /* LAB_051B */
        } else if (kind == 1) {
            if (!((int8_t)rb(m, rl(m, MOG_v_ScreenOther) + 73) > 0))
                ww(m, MOG_v_LootTaken, 0);
            if (rw(m, MOG_v_LootTaken)) {                  /* butin pris : inventaire */
                wl(m, MOG_v_ScreenKind, 9);
                continue;
            }
            wl(m, MOG_v_LoadPtr, rl(m, MOG_v_ScreenOther));
            ww(m, MOG_v_LivesIconAlt, 1);
            draw_knight(m);
        } else if (kind == 0x0B || (kind == 8 && !rw(m, MOG_v_LootTaken))) {
            wl(m, MOG_v_LoadPtr, rl(m, MOG_v_ScreenOther));
            ww(m, MOG_v_LivesIconAlt, 1);
            draw_knight(m);
            next_button(m);                             /* LAB_0524 */
        } else if (kind == 8) {                         /* LAB_04DC */
            ww(m, MOG_v_LootTaken, 0);
            wl(m, MOG_v_ScreenKind, rl(m, MOG_v_ScreenKindSaved));
            continue;
        } else {
            if (kind == 5)
                draw_shop(m);                           /* LAB_0522 */
            if (kind == 6) {
                wl(m, MOG_v_LoadPtr, MOG_t_ShopInventory);
                draw_inventory(m);                      /* LAB_04FE */
                magic_sword(m, rl(m, MOG_v_LoadPtr));
            }
            if (kind == 0x0A)
                draw_dragon(m);                         /* LAB_051D */
        }
        break;
    }
    panel_colours(m);                                   /* LAB_04E1 */
    mog_swap_screens(m);                                /* LAB_0416 */
    copy_screen(m, rl(m, MOG_v_ShowPlanes), rl(m, MOG_v_DrawPlanes));
    uint16_t c[32];                                     /* LAB_0D8A, LAB_03EE */
    for (int i = 0; i < 32; i++)
        c[i] = rw(m, MOG_t_ScreenPalette + 2u * (unsigned)i);
    mog_wait_vbls(m, 1);
    if (m->palette)
        m->palette(m->out.user, c);
    uint32_t cur = rl(m, MOG_v_PalCurrent);
    for (uint32_t i = 0; i < 64; i++)
        wb(m, cur + i, rb(m, MOG_t_ScreenPalette + i));
}

/* ------------------------------------------------------------------ */
/* Clics                                                               */
/* ------------------------------------------------------------------ */

/* LAB_05A0 : sort raté (deux sons, 8 VBL) */
static void bad_sound(MogCombat *m)
{
    mog_sound(m, 0xA1);
    mog_sound(m, 0xA2);
    mog_wait_vbls(m, 8);
}

/* LAB_05A1 : sort réussi */
static void good_sound(MogCombat *m)
{
    for (int n = 0xA3; n <= 0xA6; n++)
        mog_sound(m, n);
    mog_wait_vbls(m, 8);
}

static uint32_t d100(MogCombat *m)                      /* LAB_04A3 */
{
    uint32_t d0 = mog_random(m) & 0x7F;
    if (d0 >= 0x64)
        d0 -= 0x1B;
    return d0;
}

/* LAB_0527 / LAB_0528 : chevalier suivant (autre que LAB_068B) en face */
static void next_knight(MogCombat *m, int reset)
{
    if (reset)
        ww(m, MOG_v_NextKnight, 0);
    uint32_t k;
    do {
        uint16_t n = (uint16_t)((rw(m, MOG_v_NextKnight) + 1) & 3);
        ww(m, MOG_v_NextKnight, n);
        k = rl(m, MOG_t_Knights + 4u * n);
    } while (k == rl(m, MOG_v_ScreenKnight));
    wl(m, MOG_v_Combatants + 4, k);
    wl(m, MOG_v_ScreenOther, k);
    wl(m, MOG_v_ScreenOtherInv, rl(m, k + 96));
}

static void inc(MogCombat *m, uint32_t a) { wb(m, a, (uint8_t)(rb(m, a) + 1)); }
static void dec(MogCombat *m, uint32_t a) { wb(m, a, (uint8_t)(rb(m, a) - 1)); }

/* LAB_0542 : échange fait, chevalier remis à jour */
static void traded(MogCombat *m)
{
    ww(m, MOG_v_LootTaken, (uint16_t)(rw(m, MOG_v_LootTaken) + 1));
    uint32_t a0 = rl(m, MOG_v_ScreenKnight);
    mog_knight_hp(m, a0);
    mog_knight_defence(m, a0);
    build(m);
}

/* LAB_0551 : l'épée magique prise (d1 = 4) */
static void take_sword(MogCombat *m, uint32_t a1)
{
    dec(m, a1 + 4);
    wl(m, rl(m, MOG_v_ScreenKnight) + 88, 0x19);
    if (KIND != 2) {
        inc(m, rl(m, MOG_v_ScreenInventory) + 4);
        wl(m, rl(m, MOG_v_ScreenOther) + 88, 0x16);
    }
    traded(m);
}

/* LAB_053F : prendre un objet de l'autre panneau */
static void take(MogCombat *m, uint16_t flags, uint16_t d1, uint8_t d3)
{
    uint32_t a0 = rl(m, MOG_v_ScreenInventory), a1 = rl(m, MOG_v_ScreenOtherInv);
    if (!(flags & 0x20))
        return;
    mog_sound(m, 0x9C);
    if (d1 == 0x16 || d1 == 0x14) {                     /* LAB_0541 : clés, sorts */
        uint8_t v = rb(m, a1 + d1);
        wb(m, a1 + d1, 0);
        wb(m, a0 + d1, rb(m, a0 + d1) | v);
        traded(m);
        return;
    }
    (void)d3;
    if (d1 == 4) {
        take_sword(m, a1);
        return;
    }
    dec(m, a1 + d1);
    inc(m, a0 + d1);
    if (d1 == 6) {
        uint32_t k = rl(m, MOG_v_ScreenKnight);
        ww(m, k + 80, (uint16_t)(rw(m, k + 80) + 0x14));
        mog_knight_hp(m, k);
    }
    traded(m);
}

/* LAB_0562 : achat (panneau de droite) ou vente au marchand LAB_0690 */
static void trade_shop(MogCombat *m, uint16_t d1, uint8_t d3)
{
    uint32_t a0 = rl(m, MOG_v_ScreenInventory), a1 = MOG_t_ShopInventory, a2 = rl(m, MOG_v_ScreenKnight);
    uint32_t a3 = MOG_t_ShopPrices;
    int16_t price = sw(rw(m, a3 + d1));
    if (!(sw(rw(m, MOG_v_PointerX)) < 0xA0)) {
        mog_message(m, "Purchasing");
        if (price > sw(rw(m, a2 + 74)))
            return;
        ww(m, a2 + 74, (uint16_t)(rw(m, a2 + 74) - price));
        if (d1 == 0x16 || d1 == 0x14) {                 /* LAB_0566 */
            wb(m, a0 + d1, rb(m, a0 + d1) | d3);
            wb(m, a1 + d1, rb(m, a1 + d1) ^ d3);
            build(m);
            return;
        }
        dec(m, a1 + d1);
        inc(m, a0 + d1);
        if (d1 == 6) {
            a0 = a2;
            ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + 0x14));
        }
        mog_knight_hp(m, a0);                           /* LAB_0563 (sic : a0) */
        build(m);
        return;
    }
    if (d1 == 0x16 || d1 == 0x14) {                     /* LAB_0567 */
        wb(m, a1 + d1, rb(m, a1 + d1) | d3);
        wb(m, a0 + d1, rb(m, a0 + d1) ^ d3);
    } else {
        dec(m, a0 + d1);
        inc(m, a1 + d1);
    }
    int16_t gold = (int16_t)(rw(m, a2 + 74) + ((uint16_t)price >> 1));   /* LAB_0569 */
    if (gold > 0x96)
        gold = 0x96;
    ww(m, a2 + 74, (uint16_t)gold);
    if (d1 == 4)
        wl(m, rl(m, MOG_v_ScreenKnight) + 88, 0x16);
    mog_knight_hp(m, a2);
    build(m);
    mog_message(m, "Selling");
}

/* LAB_052D : objet utilisé (potions, sorts, anneaux...) */
static void use(MogCombat *m, uint16_t flags, uint16_t d1, uint8_t d3)
{
    ww(m, MOG_v_UsedItem, 0xFFFF);
    uint32_t a0 = rl(m, MOG_v_ScreenInventory);
    if (KIND == 6) {
        trade_shop(m, d1, d3);
        return;
    }
    if (!(flags & 0x10)) {
        take(m, flags, d1, d3);
        return;
    }
    mog_message(m, "Casting magic");
    dec(m, a0 + d1);
    ww(m, MOG_v_UsedItem, d1);
    if (KIND == 3) {
        mog_sound(m, 0x9C);
        dec(m, a0 + d1);
        ww(m, MOG_v_UsedItem, d1);
        ww(m, MOG_v_ScreenChanged, 1);
        build(m);
        return;
    }
    uint32_t k = rl(m, MOG_v_ScreenKnight);
    switch (d1) {
    case 0:                                             /* potion */
        mog_sound(m, 0x9C);
        wb(m, k + 130, 0);                              /* LAB_052F */
        if (rw(m, k + 84) == rw(m, k + 80)) {
            inc(m, k + 73);
            if (!((int8_t)rb(m, k + 73) < 6))
                wb(m, k + 73, 5);
        }
        ww(m, k + 80, rw(m, k + 84));
        break;
    case 0x0A:                                          /* bottes */
        if (d100(m) > 0x0A) {
            good_sound(m);
            ww(m, MOG_v_MovesMax, (uint16_t)(rw(m, MOG_v_MovesMax) << 1));
        } else {
            bad_sound(m);
            ww(m, MOG_v_ReversedOn, 1);
            ww(m, MOG_v_MovesMax, (uint16_t)(rw(m, MOG_v_MovesMax) >> 1));
        }
        break;
    case 0x0E:                                          /* échange avec un autre */
        wl(m, MOG_v_ScreenKindSaved, KIND);
        good_sound(m);
        next_knight(m, 1);
        mog_screen_run(m, 8);
        return;
    case 2:
        good_sound(m);
        mog_map_0E02(m);
        ww(m, MOG_v_ScreenChanged, 1);
        break;
    case 0x0C:
        if (d100(m) > 0x0F) {
            good_sound(m);
            mog_map_0E05(m, k);
        } else {
            bad_sound(m);
            mog_map_0E06(m);
        }
        ww(m, MOG_v_ScreenChanged, 1);
        break;
    case 0x10:
        good_sound(m);
        ww(m, MOG_v_ScreenRedo, 1);
        next_knight(m, 1);
        mog_screen_run(m, 0x0B);
        return;
    case 0x12:
        ww(m, MOG_v_ScreenChanged, 1);
        if (d100(m) > 0x0A) {
            good_sound(m);
        } else {
            bad_sound(m);
            ww(m, MOG_v_ReversedOn, 1);
            bad_sound(m);
        }
        break;
    }
    build(m);                                           /* LAB_053A */
}

/* LAB_052A : clic sur la zone z */
static void click(MogCombat *m, uint32_t z)
{
    if (rl(m, z + 16) == 7) {                           /* sortie */
        ww(m, MOG_v_ScreenChanged, 1);
        return;
    }
    if (rl(m, z + 8) == MOG_t_NextKnightText) {                 /* chevalier suivant */
        next_knight(m, 0);
        build(m);
        return;
    }
    uint16_t flags = rw(m, rl(m, z + 8) + 8);
    uint16_t d1 = rw(m, z + 22), d2 = rw(m, z + 20);
    uint8_t d3 = (uint8_t)((d2 & 0xF0) >> 4);
    d2 &= 0x0F;
    uint32_t a0 = rl(m, MOG_v_ScreenKnight), a1 = rl(m, MOG_v_ScreenOther);
    switch (d2) {
    case 5:
        use(m, flags, d1, d3);
        return;
    case 1:
        if (KIND == 6) {
            trade_shop(m, d1, d3);
            return;
        }
        take(m, flags, d1, d3);
        return;
    case 3:                                             /* LAB_0544 : caractéristiques */
        if (flags & 0x40) {
            mog_message(m, "Increasing Ability");
            d1 = rw(m, z + 22);
            inc(m, a0 + d1);
            if (d1 == 0x47)
                ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + 0x0A));
            ww(m, a0 + 78, (uint16_t)(rw(m, a0 + 78) - rw(m, MOG_v_TrainCost)));
            mog_knight_hp(m, a0);
            mog_knight_defence(m, a0);
            build(m);
            return;                                     /* registres perdus : fin */
        }
        if ((flags & 0x20) && d1 == 0x4C && rb(m, a0 + 76) != 0x0A && rb(m, a1 + 76)) {
            while (rb(m, a0 + 76) != 0x0A && rb(m, a1 + 76)) {   /* dagues */
                dec(m, a1 + 76);
                inc(m, a0 + 76);
            }
            ww(m, MOG_v_LootTaken, (uint16_t)(rw(m, MOG_v_LootTaken) + 1));
            build(m);
        }
        return;
    case 0x0A:                                          /* LAB_0558 : boutique */
        if (d1 == 0x5C) {
            static const struct { uint16_t price; uint32_t armour; uint16_t hp; } a[3] = {
                { 0x1E, 0x1C, 10 }, { 0x32, 0x1D, 20 }, { 0x4B, 0x1E, 30 },
            };
            for (int i = 0; i < 3; i++) {
                if (!(d3 & (1 << i)))
                    continue;
                if (sw(rw(m, a0 + 74)) < (int16_t)a[i].price)
                    return;
                ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) - a[i].price));
                wl(m, a0 + 92, a[i].armour);
                ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + a[i].hp));
                mog_knight_hp(m, a0);
                build(m);
                return;
            }
        } else if (d1 == 0x58) {
            static const struct { uint16_t price; uint32_t w; } w[2] = { { 0x0A, 0x17 }, { 0x19, 0x18 } };
            for (int i = 0; i < 2; i++) {
                if (!(d3 & (1 << i)))
                    continue;
                if (sw(rw(m, a0 + 74)) < (int16_t)w[i].price || (int32_t)rl(m, a0 + 88) >= (int32_t)w[i].w)
                    return;
                ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) - w[i].price));
                wl(m, a0 + 88, w[i].w);
                build(m);
                return;
            }
        } else if (d1 == 0x4C) {
            if (sw(rw(m, a0 + 74)) < 2 || !((int8_t)rb(m, a0 + 76) < 0x0A))
                return;
            ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) - 2));
            inc(m, a0 + 76);
            build(m);
        }
        return;
    case 0x0C:                                          /* LAB_053D */
        a1 = rl(m, MOG_v_ScreenInventory);
        mog_sound(m, 0x9C);
        if (d1 == 4)
            wl(m, a0 + 88, 0x16);
        dec(m, a1 + d1);
        ww(m, MOG_v_UsedItem, d1);
        ww(m, MOG_v_ScreenChanged, 1);
        traded(m);
        return;
    }
    if (!(flags & 0x20))
        return;
    switch (d1) {                                       /* LAB_054A */
    case 0x4A: {                                        /* or */
        uint32_t g = KIND == 2 ? rl(m, MOG_v_Lair) + 8 : a1 + 74;
        if (rw(m, g)) {
            for (;;) {
                if (rw(m, a0 + 74) == 0x96)
                    break;
                ww(m, g, (uint16_t)(rw(m, g) - 1));
                ww(m, a0 + 74, (uint16_t)(rw(m, a0 + 74) + 1));
                if (!rw(m, g)) {
                    mog_message(m, "Taking gold");
                    break;
                }
            }
            ww(m, MOG_v_LootTaken, (uint16_t)(rw(m, MOG_v_LootTaken) + 1));
        }
        build(m);
        return;
    }
    case 0x5C: {                                        /* armure */
        uint32_t d5 = rl(m, a0 + 92);
        if ((int32_t)d5 < (int32_t)rl(m, a1 + 92)) {
            wl(m, a0 + 92, rl(m, a1 + 92));
            uint16_t v = rw(m, MOG_t_ZoneValues + (d5 - 0x1B));
            ww(m, a1 + 80, (uint16_t)(rw(m, a1 + 80) - v));
            ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + v));
            wl(m, a1 + 92, 0x1B);
            ww(m, MOG_v_LootTaken, (uint16_t)(rw(m, MOG_v_LootTaken) + 1));
        }
        build(m);
        return;
    }
    case 0x58: {                                        /* arme */
        uint32_t d5 = rl(m, a0 + 88);
        if (d5 == 0x19) {
            take_sword(m, rl(m, MOG_v_ScreenOtherInv));
            return;
        }
        if ((int32_t)d5 < (int32_t)rl(m, a1 + 88)) {
            wl(m, a0 + 88, rl(m, a1 + 88));
            wl(m, a1 + 88, d5);
            ww(m, MOG_v_LootTaken, (uint16_t)(rw(m, MOG_v_LootTaken) + 1));
        }
        build(m);
        return;
    }
    }
}

/* ------------------------------------------------------------------ */
/* Boucle                                                              */
/* ------------------------------------------------------------------ */

void mog_screen_run(MogCombat *m, uint32_t kind)
{
    ww(m, MOG_v_BlitByCpu, 1);                             /* LAB_04CF */
    wl(m, MOG_v_IconCel, rl(m, MOG_t_BankMap + 4));
    ww(m, MOG_v_LootTaken, 0);
    wl(m, MOG_v_ScreenKind, kind);
    ww(m, MOG_v_ScreenChanged, 0);
    mog_fade_black(m);                                  /* LAB_03F0 */
    pointer_on(m);                                      /* LAB_0575 */
    text_tables(m);                                     /* LAB_0588 */
    build(m);                                           /* LAB_04D4 */
    for (;;) {                                          /* LAB_04D0 */
        if (m->frame_start)             /* point de rendez-vous (pas dans mog) */
            m->frame_start(m->out.user);
        uint32_t z = zone_at(m, rw(m, MOG_v_PointerX), rw(m, MOG_v_PointerY));
        if (z && !rw(m, MOG_v_PointerOn)) {
            click(m, z);
            if (rw(m, MOG_v_ScreenChanged))
                break;
        }
        mog_swap_screens(m);
        mog_restore_areas(m);
    }
    mog_fade_black(m);                                  /* LAB_04D2 */
    pointer_off(m);
    if (rw(m, MOG_v_ScreenRedo)) {
        wl(m, MOG_v_DragonObj + 100, rl(m, MOG_v_Combatants + 4));
        ww(m, MOG_v_ScreenRedo, 0);
    }
    ww(m, MOG_v_BlitByCpu, 0);
}

/* ------------------------------------------------------------------ */
/* Pour les autres écrans (villes : mog_town.c)                        */
/* ------------------------------------------------------------------ */

void mog_pointer_on(MogCombat *m) { pointer_on(m); }
void mog_pointer_off(MogCombat *m) { pointer_off(m); }
void mog_clear_zones(MogCombat *m) { clear_zones(m); }
void mog_add_zone(MogCombat *m) { add_zone(m); }
uint32_t mog_zone_at(MogCombat *m, uint16_t x, uint16_t y) { return zone_at(m, x, y); }
void mog_copy_screen(MogCombat *m, uint32_t a0, uint32_t a1) { copy_screen(m, a0, a1); }
