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

#define KIND (rl(m, MOG_LAB_068F))
#define MOG_EXT_0023 0x7F6AEu              /* pointeurs de sprites de la copper list */

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
    ww(m, MOG_LAB_0E82, 0);
    uint32_t a0 = rl(m, MOG_LAB_0E8C + (uint32_t)(n << 2)) + 2;
    if (sw(rw(m, a0 - 2)) < 0) {
        ww(m, MOG_LAB_0E82, (uint16_t)(n & 1 ? n - 1 : n + 1));
        ww(m, MOG_L37_00172, x);
        ww(m, MOG_LAB_0E84, y);
    }
    sprite_ctl(m, a0, x, y);
    uint16_t pair = rw(m, MOG_LAB_0E82);                /* LAB_0E80 */
    if (pair) {
        a0 = rl(m, MOG_LAB_0E8C + (uint32_t)(pair << 2)) + 2;
        ww(m, MOG_LAB_0E82, 0);
        sprite_ctl(m, a0, rw(m, MOG_L37_00172), rw(m, MOG_LAB_0E84));
    }
}

/* LAB_0E85 : sprite n = données a0 (et a2 pour le jumeau) dans la copper
 * list a1 (registres SPRxPT) */
static void sprite_set(MogCombat *m, uint16_t n, uint32_t a0, uint32_t a1, uint32_t a2)
{
    ww(m, MOG_LAB_0E82, 0);
    uint32_t a1_saved = a1;
    if (sw(rw(m, a0)) < 0)
        ww(m, MOG_LAB_0E82, (uint16_t)(n & 1 ? n - 1 : n + 1));
    for (;;) {                                          /* LAB_0E88 */
        wl(m, MOG_LAB_0E8C + (uint32_t)(n << 2), a0);
        a0 += 2;
        uint16_t reg = (uint16_t)((n << 2) + 0x120);
        uint32_t a = a1;
        while (rw(m, a) != reg)
            a += 4;
        a += 2;
        ww(m, a, (uint16_t)(a0 >> 16));
        ww(m, a + 4, (uint16_t)a0);
        uint16_t pair = rw(m, MOG_LAB_0E82);
        if (!pair)
            return;
        ww(m, MOG_LAB_0E82, 0);
        n = pair;
        a1 = a1_saved;
        a0 = a2;
    }
}

/* LAB_0575 : pointeur montré, mené par l'interruption d'image (LAB_057D) */
static void pointer_on(MogCombat *m)
{
    if (rw(m, MOG_LAB_097C))
        return;
    ww(m, MOG_LAB_097C, 1);
    sprite_set(m, 0, rl(m, MOG_LAB_097D), MOG_EXT_0023, rl(m, MOG_LAB_097E));   /* LAB_0E77 */
    sprite_move(m, 0, rw(m, MOG_LAB_097F), rw(m, MOG_LAB_0980));
    ww(m, MOG_LAB_0981, 1);
    uint32_t a0 = MOG_LAB_0B96;                         /* serveur d'interruption */
    while (rl(m, a0))
        a0 += 4;
    wl(m, a0, MOG_LAB_057D);
    wl(m, MOG_LAB_0982, a0);
}

/* LAB_057B : pointeur caché */
static void pointer_off(MogCombat *m)
{
    if (!rw(m, MOG_LAB_097C))
        return;
    ww(m, MOG_LAB_097C, 0);
    sprite_set(m, 0, MOG_SECSTRT_38, MOG_EXT_0023, 0);  /* LAB_0E76 */
    wl(m, rl(m, MOG_LAB_0982), 0);
}

void mog_screen_vbl(MogCombat *m)
{
    if (!rw(m, MOG_LAB_097C))
        return;
    /* LAB_057D : joystick du chevalier LAB_068B (11 = 1 : port 0) */
    ww(m, MOG_LAB_0981, 1);
    ww(m, MOG_LAB_062F, m->joy[0]);                     /* LAB_00EE */
    ww(m, MOG_LAB_0630, m->joy[1]);
    uint16_t d1 = m->joy[1];
    if (rb(m, rl(m, MOG_LAB_068B) + 11) == 1)
        d1 = m->joy[0];
    uint16_t x = rw(m, MOG_LAB_097F), y = rw(m, MOG_LAB_0980);
    if (d1 & 1) x = (uint16_t)(x + 2);
    if (d1 & 2) x = (uint16_t)(x - 2);
    if (d1 & 4) y = (uint16_t)(y + 2);
    if (d1 & 8) y = (uint16_t)(y - 2);
    if (d1 & 0x10)
        ww(m, MOG_LAB_0981, 0);
    if (!(sw(x) < 0x13B)) x = 0x13A;
    if (sw(x) < 0) x = 0;
    if (!(sw(y) < 0xC4)) y = 0xC3;
    if (sw(y) < 0) y = 0;
    ww(m, MOG_LAB_097F, x);
    ww(m, MOG_LAB_0980, y);
    sprite_move(m, 0, x, y);
}

void mog_pointer_boot(MogCombat *m)
{
    /* LAB_0572 : po.cel -> sprite 0 (SECSTRT_37 dans SECSTRT_43) */
    uint32_t cel = rl(m, MOG_LAB_0D92);
    mog_load_cel(VM, MOG_LAB_0983, cel);
    uint32_t a3 = MOG_SECSTRT_43;
    wl(m, MOG_LAB_0E8D, a3);
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
        wl(m, a3, 0);
    }
    wl(m, MOG_LAB_097D, rl(m, MOG_LAB_0E8D));
    wl(m, MOG_LAB_097E, rl(m, MOG_LAB_0E8E));
}

/* ------------------------------------------------------------------ */
/* Zones                                                               */
/* ------------------------------------------------------------------ */

/* LAB_044E : zones et modèle LAB_0A58 effacés */
static void clear_zones(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_SECSTRT_14);
    for (uint32_t i = 0; i < 0x960; i++)
        wb(m, a0 + i, 0);
    for (uint32_t i = 0; i < 24; i++)
        wb(m, MOG_LAB_0A58 + i, 0);
}

/* LAB_0448 : le modèle LAB_0A58 copié dans la première zone libre */
static void add_zone(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_SECSTRT_14);                /* LAB_044B */
    int i;
    for (i = 0; i < 98 && rw(m, a0 + 4); i++)
        a0 += 24;
    if (i == 98)
        return;
    for (uint32_t k = 0; k < 24; k++)
        wb(m, a0 + k, rb(m, MOG_LAB_0A58 + k));
}

/* LAB_0451 : zone sous le point (x, y) : son texte est montré ; renvoie
 * la zone (0 : aucune) */
static uint32_t zone_at(MogCombat *m, uint16_t x, uint16_t y)
{
    ww(m, MOG_LAB_08DA, x);
    ww(m, MOG_LAB_08DB, y);
    for (uint32_t a0 = rl(m, MOG_SECSTRT_14); rw(m, a0 + 4); a0 += 24) {
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
    uint32_t a1 = MOG_LAB_0A58;
    uint32_t e = rl(m, MOG_LAB_0986) + (uint32_t)(int32_t)sw((uint16_t)(rw(m, MOG_LAB_0681) * 10));
    ww(m, a1 + 4, rw(m, e + 14));
    ww(m, a1 + 6, rw(m, e + 16));
    ww(m, a1 + 12, (uint16_t)(rw(m, MOG_LAB_0682) + rw(m, MOG_LAB_0985)));
    ww(m, a1 + 14, rw(m, MOG_LAB_0683));
    uint32_t d0 = rl(m, MOG_LAB_0686);
    wl(m, a1 + 16, d0);
    ww(m, a1 + 22, rw(m, MOG_LAB_0687));
    ww(m, a1 + 20, rw(m, MOG_LAB_0685));
    wl(m, a1 + 8, rl(m, MOG_LAB_0688) + (uint32_t)(uint16_t)(d0 >> 2) * 14u);
    add_zone(m);
}

/* LAB_0516 / LAB_0517 : d7 icônes LAB_0681 en (LAB_0682, LAB_0683), pas
 * LAB_0684, chacune avec sa zone */
static void icons(MogCombat *m, uint32_t d7, int check_kind)
{
    if (check_kind && KIND == 3)
        ww(m, MOG_LAB_0685, 0x0C);
    ww(m, MOG_LAB_0D05, 1);
    if (!(uint16_t)d7)
        return;
    for (uint32_t i = (uint16_t)(d7 - 1) + 1u; i > 0; i--) {
        mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_0986), rw(m, MOG_LAB_0681),
                     (uint16_t)(rw(m, MOG_LAB_0682) + rw(m, MOG_LAB_0985)), rw(m, MOG_LAB_0683));
        icon_zone(m);
        ww(m, MOG_LAB_0682, (uint16_t)(rw(m, MOG_LAB_0682) + rw(m, MOG_LAB_0684)));
    }
}

/* Réglage des variables d'icône */
static void icon_set(MogCombat *m, uint16_t f, uint16_t x, uint16_t y, uint16_t step,
                     uint32_t id, uint16_t kind, uint16_t slot)
{
    ww(m, MOG_LAB_0681, f);
    ww(m, MOG_LAB_0682, x);
    ww(m, MOG_LAB_0683, y);
    ww(m, MOG_LAB_0684, step);
    wl(m, MOG_LAB_0686, id);
    ww(m, MOG_LAB_0685, kind);
    ww(m, MOG_LAB_0687, slot);
}

/* ------------------------------------------------------------------ */
/* Panneaux                                                            */
/* ------------------------------------------------------------------ */

/* LAB_0523 : zone « sortie » (identifiant 7) du grand bouton en (d1, d2) */
static void exit_zone(MogCombat *m, uint16_t d1, uint16_t d2)
{
    uint32_t a0 = MOG_LAB_0A58;
    ww(m, a0 + 12, (uint16_t)(d1 + rw(m, MOG_LAB_0985)));
    ww(m, a0 + 14, d2);
    ww(m, a0 + 4, 0x19);
    ww(m, a0 + 6, 0x75);
    wl(m, a0 + 8, MOG_LAB_09EE);
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
        uint32_t cel = rl(m, MOG_LAB_0986);
        uint32_t e = cel + (uint32_t)(int32_t)sw((uint16_t)(d0 * 10));
        int original = rb(m, e + 18) == 1;
        if (original ? d3 != 0 : d3 == 0)
            ix_flip_frame(&m->eng, cel, d0);            /* Cel_FlipFrame */
        mog_draw_cel(VM, &m->blt, cel, d0, (uint16_t)(d1 + rw(m, MOG_LAB_0985)), d2);
    }
}

/* LAB_04EA : fond (LAB_05C0 effacé) et cadres des panneaux */
static void draw_frames(MogCombat *m)
{
    mog_set_planes(m, rl(m, MOG_LAB_05C0));
    mog_clear_screen(m, rl(m, MOG_LAB_05C0));
    ww(m, MOG_LAB_0985, 0);
    uint32_t a1 = MOG_LAB_04F3;
    for (uint32_t a2 = MOG_LAB_098A; sw(rw(m, a2)) >= 0; a2 += 2)
        if (rw(m, a2) == (uint16_t)KIND) {
            ww(m, MOG_LAB_0985, 0x4A);
            a1 = MOG_L00_0AFE4;
            break;
        }
    draw_list(m, a1);
}

/* Numéro (LAB_0442) écrit en (x + LAB_0985, y) */
static void number_at(MogCombat *m, uint32_t v, uint16_t x, uint16_t y)
{
    mog_number(m, v, MOG_LAB_0987);
    mog_text(m, MOG_LAB_0987, (uint16_t)(x + rw(m, MOG_LAB_0985)), y, 0);
}

/* LAB_04FE : inventaire rl(LAB_0632) (24 octets) en icônes */
static void draw_inventory(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_LAB_0632);
    uint32_t d7 = rb(m, a0 + 2);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 9, 0x57, 0xA4, 0x10, 0x4C, 5, 2);
        icons(m, d7, 1);
    }
    a0 = rl(m, MOG_LAB_0632);                           /* LAB_0500 */
    d7 = rb(m, a0 + 6);
    if (d7) {
        if (!(d7 < 4)) d7 = 3;
        icon_set(m, 3, 0x21, 0x95, 0x0C, 0x50, 1, 6);
        icons(m, d7, 1);
    }
    a0 = rl(m, MOG_LAB_0632);                           /* LAB_0502 */
    d7 = rb(m, a0 + 8);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 0x0A, 0x45, 0x85, 0x14, 0x54, 1, 8);
        icons(m, d7, 1);
    }
    ww(m, MOG_LAB_0687, 0x14);                          /* LAB_0504 : clés */
    wl(m, MOG_LAB_0686, 0x44);
    a0 = rl(m, MOG_LAB_0632);
    uint8_t keys = rb(m, a0 + 20);
    if (keys) {
        static const struct { uint16_t f, x, kind; } k[4] = {
            { 5, 0x4C, 0x11 }, { 6, 0x5E, 0x21 }, { 7, 0x70, 0x41 }, { 8, 0x82, 0x81 },
        };
        for (int i = 0; i < 4; i++)
            if (keys & (1 << i)) {
                ww(m, MOG_LAB_0681, k[i].f);
                ww(m, MOG_LAB_0682, k[i].x);
                ww(m, MOG_LAB_0683, 0x6F);
                ww(m, MOG_LAB_0685, k[i].kind);
                icons(m, 1, 0);
            }
    }
    ww(m, MOG_LAB_0687, 0x16);                          /* LAB_0508 : sorts */
    ww(m, MOG_LAB_0682, 0x67);
    ww(m, MOG_LAB_0683, 0x6F);
    a0 = rl(m, MOG_LAB_0632);
    uint8_t spells = rb(m, a0 + 22);
    if (spells) {
        static const struct { uint16_t f; uint32_t id; uint16_t kind; } k[4] = {
            { 2, 0x38, 0x11 }, { 1, 0x3C, 0x21 }, { 0, 0x40, 0x41 }, { 0, 0x38, 0x81 },
        };
        for (int i = 0; i < 4; i++)
            if (spells & (1 << i)) {
                ww(m, MOG_LAB_0681, k[i].f);
                wl(m, MOG_LAB_0686, k[i].id);
                ww(m, MOG_LAB_0685, k[i].kind);
                icons(m, 1, 0);
            }
    }
    a0 = rl(m, MOG_LAB_0632);                           /* LAB_050C : potions */
    d7 = rb(m, a0);
    if (d7) {
        if (d7 > 4) d7 = 4;
        icon_set(m, 4, 0x1F, 0xA5, 0x0D, 0x48, 5, 0);
        icons(m, d7, 1);
    }
    /* LAB_050E : objets 10 à 18 (cinq mots), deux icônes au plus */
    a0 = rl(m, MOG_LAB_0632);
    uint16_t d1 = 0x1E, d2 = 0xB7;
    ww(m, MOG_LAB_0514, d1);
    ww(m, MOG_LAB_0687, 0x0A);
    ww(m, MOG_LAB_0685, 5);
    wl(m, MOG_LAB_0686, 0x58);
    uint32_t a1 = a0 + 10;
    uint16_t d0 = 0x0B;
    ww(m, MOG_L00_0B85C, 0x0B);
    for (int i = 0; i < 5; i++, a1 += 2) {
        int16_t d5 = (int8_t)rb(m, a1);
        if (d5) {
            ww(m, MOG_LAB_0684, 0x0B);
            /* LAB_0511 */
            if (!(d5 < 3))
                d5 = 2;
            d5--;
            for (;;) {                                  /* LAB_0513 */
                ww(m, MOG_LAB_0681, d0);
                ww(m, MOG_LAB_0682, d1);
                ww(m, MOG_LAB_0683, d2);
                icons(m, 1, 1);
                ww(m, MOG_LAB_0682, (uint16_t)(d1 + rw(m, MOG_LAB_0684)));
                ww(m, MOG_LAB_0684, 4);
                d0 = 0x10;
                d1 = rw(m, MOG_LAB_0682);
                if (--d5 < 0)
                    break;
            }
            ww(m, MOG_LAB_0514, (uint16_t)(rw(m, MOG_LAB_0514) + 0x19));
            d1 = rw(m, MOG_LAB_0514);
        }
        ww(m, MOG_L00_0B85C, (uint16_t)(rw(m, MOG_L00_0B85C) + 1));
        d0 = rw(m, MOG_L00_0B85C);
        ww(m, MOG_LAB_0687, (uint16_t)(rw(m, MOG_LAB_0687) + 2));
        wl(m, MOG_LAB_0686, rl(m, MOG_LAB_0686) + 4);
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
    uint32_t a0 = rl(m, MOG_LAB_0632);
    mog_knight_defence(m, a0);
    mog_knight_hp(m, a0);
    mog_text(m, rl(m, a0 + 108), (uint16_t)(0x3C + rw(m, MOG_LAB_0985)), 0x18, 0);
    a0 = rl(m, MOG_LAB_0632);
    stat_icons(m, rb(m, a0 + 70), 0x26, 0x29, 0x23, 0, 0, 3, 0x46);
    stat_icons(m, rb(m, a0 + 72), 0x28, 0x29, 0x31, 0, 4, 3, 0x48);
    stat_icons(m, rb(m, a0 + 71), 0x27, 0x29, 0x2A, 0, 8, 3, 0x47);
    stat_icons(m, 1, 0x2A, 0x59, 0x2A, 0, 0x10, 2, 0x4A);
    draw_list(m, MOG_LAB_04F5);
    a0 = rl(m, MOG_LAB_0632);
    number_at(m, rw(m, a0 + 78), 0x70, 0x23);
    a0 = rl(m, MOG_LAB_0632);
    wl(m, MOG_LAB_0987, 0);
    number_at(m, rw(m, a0 + 74), 0x70, 0x2A);
    ww(m, MOG_LAB_04F6, 0x46);
    ww(m, MOG_L00_0B042, 0x23);
    for (int i = 0; i < 3; i++) {                       /* LAB_04F9 */
        a0 = rl(m, MOG_LAB_0632);
        uint32_t v = rb(m, a0 + (uint32_t)(int32_t)sw(rw(m, MOG_LAB_04F6)));
        wl(m, MOG_LAB_0987, 0);
        number_at(m, v, 0x3F, rw(m, MOG_L00_0B042));
        ww(m, MOG_LAB_04F6, (uint16_t)(rw(m, MOG_LAB_04F6) + 1));
        ww(m, MOG_L00_0B042, (uint16_t)(rw(m, MOG_L00_0B042) + 7));
    }
    a0 = rl(m, MOG_LAB_0632);
    int32_t lives = (int8_t)rb(m, a0 + 73);
    if (lives < 0)
        lives = 5;
    uint16_t f = (uint16_t)(0x11 + rw(m, MOG_LAB_0680));
    if (rb(m, a0 + 82))
        f = (uint16_t)(f + 2);
    stat_icons(m, (uint32_t)lives, f, 0x29, 0x3B, 0x13, 0x0C, 3, 0x49);
    a0 = rl(m, MOG_LAB_0632);
    wl(m, MOG_LAB_0987, 0);
    wl(m, MOG_LAB_0988, 0);
    wl(m, MOG_LAB_0989, 0);
    uint32_t e = mog_number(m, rw(m, a0 + 80), MOG_LAB_0987);
    wb(m, e, 0x2F);
    mog_number(m, rw(m, rl(m, MOG_LAB_0632) + 84), e + 1);
    mog_text(m, MOG_LAB_0987, (uint16_t)(0x70 + rw(m, MOG_LAB_0985)), 0x31, 0);
    a0 = rl(m, MOG_LAB_0632);
    stat_icons(m, rb(m, a0 + 76), 0x15, 0x26, 0x4E, 0x0A, 0x14, 3, 0x4C);
    a0 = rl(m, MOG_LAB_0632);
    uint32_t w = rl(m, a0 + 88);
    if ((int32_t)w > 0x19)
        w = 0x19;
    stat_icons(m, 1, (uint16_t)w, 0x2B, 0x5F, 0, ((w - 0x16) << 2) + 0x28, 2, 0x58);
    a0 = rl(m, MOG_LAB_0632);
    uint32_t ar = rl(m, a0 + 92);
    stat_icons(m, 1, (uint16_t)ar, 0x1D, 0x70, 0, ((ar - 0x1B) << 2) + 0x18, 2, 0x5C);
    wl(m, MOG_LAB_0632, rl(m, rl(m, MOG_LAB_0632) + 96));
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
    wl(m, MOG_LAB_0987, 0);
    uint32_t e = mog_number(m, d0, MOG_LAB_0987);
    static const char gp[] = " gp.";
    for (int i = 0; i < 5; i++)
        wb(m, e + (uint32_t)i, (uint8_t)gp[i]);
    mog_text(m, MOG_LAB_0987, (uint16_t)(0x5C + rw(m, MOG_LAB_0985)), 0x5B, 0);
}

/* LAB_051B : la créature LAB_08C6 (butin sur la carte) */
static void draw_creature(MogCombat *m)
{
    wl(m, MOG_LAB_0688, MOG_LAB_069A);
    ww(m, MOG_LAB_0985, 0x96);
    ww(m, MOG_LAB_0D05, 1);
    uint32_t cel = rl(m, MOG_LAB_0986);
    uint16_t off = rw(m, MOG_LAB_0985);
    mog_draw_cel(VM, &m->blt, cel, 0x1F, (uint16_t)(0x3A + off), 0x2E);
    mog_draw_cel(VM, &m->blt, cel, 0x20, (uint16_t)(0x4C + off), 0x21);
    mog_draw_cel(VM, &m->blt, cel, 0x21, (uint16_t)(0x3A + off), 0x3C);
    uint32_t a0 = rl(m, MOG_LAB_08C6);
    if (rw(m, a0 + 8))
        creature_gold(m, rw(m, a0 + 8));
    magic_sword(m, rl(m, rl(m, MOG_LAB_08C6)));
    wl(m, MOG_LAB_0632, rl(m, MOG_LAB_068E));
    draw_inventory(m);
}

/* LAB_051D : le dragon (trésor) */
static void draw_dragon(MogCombat *m)
{
    wl(m, MOG_LAB_0632, rl(m, MOG_LAB_0617 + 96));
    draw_inventory(m);
    magic_sword(m, rl(m, MOG_LAB_0632));
    uint16_t g = rw(m, MOG_LAB_0617 + 74);
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
    mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_0986), 0x2C, 0x92, 0x47);
    uint32_t a0 = MOG_LAB_0A58;
    ww(m, a0 + 12, 0x92);
    ww(m, a0 + 14, 0x47);
    ww(m, a0 + 4, 0x14);
    ww(m, a0 + 6, 0x14);
    wl(m, a0 + 8, MOG_LAB_09EF);
    wl(m, a0 + 16, 0);
    ww(m, a0 + 20, 0);
    ww(m, a0 + 22, 0);
    add_zone(m);
}

/* LAB_04E1 : couleurs des chevaliers des deux panneaux (LAB_09F1) */
static void panel_colours(MogCombat *m)
{
    uint32_t a1 = rl(m, MOG_LAB_068B), a0 = MOG_LAB_09F1;
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
        a1 = rl(m, MOG_LAB_068D);
    }
}

/* ------------------------------------------------------------------ */
/* Textes des zones                                                    */
/* ------------------------------------------------------------------ */

/* LAB_0588 : tables des textes montrés au survol (une par genre d'écran) */
static void text_tables(MogCombat *m)
{
    wl(m, MOG_LAB_0694 + 0, MOG_LAB_09AB);
    wl(m, MOG_LAB_0694 + 4, MOG_LAB_09AC);
    wl(m, MOG_LAB_0694 + 8, MOG_LAB_09AD);
    wl(m, MOG_LAB_0694 + 12, MOG_LAB_09CC);
    wl(m, MOG_LAB_0694 + 16, MOG_LAB_09C2);
    wl(m, MOG_LAB_0694 + 20, MOG_LAB_09C3);
    wl(m, MOG_LAB_0694 + 24, MOG_LAB_09B1);
    wl(m, MOG_LAB_0694 + 28, MOG_LAB_09C9);
    wl(m, MOG_LAB_0694 + 32, MOG_LAB_09C8);
    wl(m, MOG_LAB_0694 + 36, MOG_LAB_09C7);
    wl(m, MOG_LAB_0694 + 40, MOG_LAB_09B5);
    wl(m, MOG_LAB_0694 + 44, MOG_LAB_09C6);
    wl(m, MOG_LAB_0694 + 48, MOG_LAB_09C4);
    wl(m, MOG_LAB_0694 + 52, MOG_LAB_09C5);
    wl(m, MOG_LAB_0694 + 56, MOG_LAB_09CB);
    wl(m, MOG_LAB_0694 + 60, MOG_LAB_09CB);
    wl(m, MOG_LAB_0694 + 64, MOG_LAB_09CB);
    wl(m, MOG_LAB_0694 + 68, MOG_LAB_09CA);
    wl(m, MOG_LAB_0694 + 72, MOG_LAB_09B9);
    wl(m, MOG_LAB_0694 + 76, MOG_LAB_09BA);
    wl(m, MOG_LAB_0694 + 80, MOG_LAB_09BB);
    wl(m, MOG_LAB_0694 + 84, MOG_LAB_09BC);
    wl(m, MOG_LAB_0694 + 88, MOG_LAB_09BD);
    wl(m, MOG_LAB_0694 + 96, MOG_LAB_09BE);
    wl(m, MOG_LAB_0694 + 92, MOG_LAB_09BF);
    wl(m, MOG_LAB_0694 + 100, MOG_LAB_09C0);
    wl(m, MOG_LAB_0694 + 104, MOG_LAB_09C1);
    wl(m, MOG_LAB_0692 + 0, MOG_LAB_09A8);
    wl(m, MOG_LAB_0692 + 4, MOG_LAB_09A9);
    wl(m, MOG_LAB_0692 + 8, MOG_LAB_09AA);
    wl(m, MOG_LAB_0692 + 12, MOG_LAB_09AE);
    wl(m, MOG_LAB_0692 + 16, MOG_LAB_09AF);
    wl(m, MOG_LAB_0692 + 20, MOG_LAB_09B0);
    wl(m, MOG_LAB_0692 + 36, MOG_LAB_09B4);
    wl(m, MOG_LAB_0692 + 32, MOG_LAB_09B3);
    wl(m, MOG_LAB_0692 + 28, MOG_LAB_09B2);
    wl(m, MOG_LAB_0692 + 24, MOG_LAB_09B1);
    wl(m, MOG_LAB_0692 + 48, MOG_LAB_09B7);
    wl(m, MOG_LAB_0692 + 40, MOG_LAB_09B5);
    wl(m, MOG_LAB_0692 + 52, MOG_LAB_09B8);
    wl(m, MOG_LAB_0692 + 44, MOG_LAB_09B6);
    wl(m, MOG_LAB_0692 + 56, MOG_LAB_0994);
    wl(m, MOG_LAB_0692 + 60, MOG_LAB_0995);
    wl(m, MOG_LAB_0692 + 64, MOG_LAB_0996);
    wl(m, MOG_LAB_0692 + 68, MOG_LAB_0997);
    wl(m, MOG_LAB_0692 + 72, MOG_LAB_098B);
    wl(m, MOG_LAB_0692 + 76, MOG_LAB_098C);
    wl(m, MOG_LAB_0692 + 80, MOG_LAB_098D);
    wl(m, MOG_LAB_0692 + 84, MOG_LAB_098E);
    wl(m, MOG_LAB_0692 + 88, MOG_LAB_098F);
    wl(m, MOG_LAB_0692 + 96, MOG_LAB_0990);
    wl(m, MOG_LAB_0692 + 92, MOG_LAB_0991);
    wl(m, MOG_LAB_0692 + 100, MOG_LAB_0992);
    wl(m, MOG_LAB_0692 + 104, MOG_LAB_0993);
    wl(m, MOG_LAB_0693 + 0, MOG_LAB_09AB);
    wl(m, MOG_LAB_0693 + 4, MOG_LAB_09AC);
    wl(m, MOG_LAB_0693 + 8, MOG_LAB_09AD);
    wl(m, MOG_LAB_0693 + 12, MOG_LAB_09AE);
    wl(m, MOG_LAB_0693 + 16, MOG_LAB_09AF);
    wl(m, MOG_LAB_0693 + 20, MOG_LAB_09B0);
    wl(m, MOG_LAB_0693 + 24, MOG_LAB_09B1);
    wl(m, MOG_LAB_0693 + 28, MOG_LAB_09B2);
    wl(m, MOG_LAB_0693 + 32, MOG_LAB_09B3);
    wl(m, MOG_LAB_0693 + 36, MOG_LAB_09B4);
    wl(m, MOG_LAB_0693 + 40, MOG_LAB_09B5);
    wl(m, MOG_LAB_0693 + 44, MOG_LAB_09B6);
    wl(m, MOG_LAB_0693 + 48, MOG_LAB_09B7);
    wl(m, MOG_LAB_0693 + 52, MOG_LAB_09B8);
    wl(m, MOG_LAB_0693 + 56, MOG_LAB_0994);
    wl(m, MOG_LAB_0693 + 60, MOG_LAB_0995);
    wl(m, MOG_LAB_0693 + 64, MOG_LAB_0996);
    wl(m, MOG_LAB_0693 + 68, MOG_LAB_0997);
    wl(m, MOG_LAB_0693 + 72, MOG_LAB_098B);
    wl(m, MOG_LAB_0693 + 76, MOG_LAB_098C);
    wl(m, MOG_LAB_0693 + 80, MOG_LAB_098D);
    wl(m, MOG_LAB_0693 + 84, MOG_LAB_098E);
    wl(m, MOG_LAB_0693 + 88, MOG_LAB_098F);
    wl(m, MOG_LAB_0693 + 96, MOG_LAB_0990);
    wl(m, MOG_LAB_0693 + 92, MOG_LAB_0991);
    wl(m, MOG_LAB_0693 + 100, MOG_LAB_0992);
    wl(m, MOG_LAB_0693 + 104, MOG_LAB_0993);
    wl(m, MOG_LAB_0698 + 0, MOG_LAB_09AB);
    wl(m, MOG_LAB_0698 + 4, MOG_LAB_09AC);
    wl(m, MOG_LAB_0698 + 8, MOG_LAB_09AD);
    wl(m, MOG_LAB_0698 + 12, MOG_LAB_09AE);
    wl(m, MOG_LAB_0698 + 16, MOG_LAB_09AF);
    wl(m, MOG_LAB_0698 + 20, MOG_LAB_09B0);
    wl(m, MOG_LAB_0698 + 24, MOG_LAB_09B1);
    wl(m, MOG_LAB_0698 + 28, MOG_LAB_09B2);
    wl(m, MOG_LAB_0698 + 32, MOG_LAB_09B3);
    wl(m, MOG_LAB_0698 + 36, MOG_LAB_09B4);
    wl(m, MOG_LAB_0698 + 40, MOG_LAB_09B5);
    wl(m, MOG_LAB_0698 + 44, MOG_LAB_09B6);
    wl(m, MOG_LAB_0698 + 48, MOG_LAB_09B7);
    wl(m, MOG_LAB_0698 + 52, MOG_LAB_09B8);
    wl(m, MOG_LAB_0698 + 56, MOG_LAB_0994);
    wl(m, MOG_LAB_0698 + 60, MOG_LAB_0995);
    wl(m, MOG_LAB_0698 + 64, MOG_LAB_0996);
    wl(m, MOG_LAB_0698 + 68, MOG_LAB_0997);
    wl(m, MOG_LAB_0698 + 72, MOG_LAB_0998);
    wl(m, MOG_LAB_0698 + 76, MOG_LAB_0999);
    wl(m, MOG_LAB_0698 + 80, MOG_LAB_098D);
    wl(m, MOG_LAB_0698 + 84, MOG_LAB_098E);
    wl(m, MOG_LAB_0698 + 88, MOG_LAB_099A);
    wl(m, MOG_LAB_0698 + 96, MOG_LAB_099B);
    wl(m, MOG_LAB_0698 + 92, MOG_LAB_099C);
    wl(m, MOG_LAB_0698 + 100, MOG_LAB_099D);
    wl(m, MOG_LAB_0698 + 104, MOG_LAB_099E);
    for (uint32_t i = 0; i < 27; i++)                   /* LAB_0589 */
        wl(m, MOG_LAB_0696 + 4 * i, MOG_LAB_09EB);
    wl(m, MOG_LAB_0696 + 56, MOG_LAB_0994);
    wl(m, MOG_LAB_0696 + 60, MOG_LAB_0995);
    wl(m, MOG_LAB_0696 + 64, MOG_LAB_0996);
    wl(m, MOG_LAB_0696 + 68, MOG_LAB_0997);
    wl(m, MOG_LAB_0696 + 72, MOG_LAB_099F);
    wl(m, MOG_LAB_0696 + 76, MOG_LAB_09A0);
    wl(m, MOG_LAB_0696 + 80, MOG_LAB_09A1);
    wl(m, MOG_LAB_0696 + 84, MOG_LAB_09A2);
    wl(m, MOG_LAB_0696 + 88, MOG_LAB_09A3);
    wl(m, MOG_LAB_0696 + 96, MOG_LAB_09A4);
    wl(m, MOG_LAB_0696 + 92, MOG_LAB_09A5);
    wl(m, MOG_LAB_0696 + 100, MOG_LAB_09A6);
    wl(m, MOG_LAB_0696 + 104, MOG_LAB_09A7);
    wl(m, MOG_LAB_0695 + 44, MOG_LAB_09CD);
    wl(m, MOG_LAB_0695 + 48, MOG_LAB_09CE);
    wl(m, MOG_LAB_0695 + 28, MOG_LAB_09CF);
    wl(m, MOG_LAB_0695 + 32, MOG_LAB_09D0);
    wl(m, MOG_LAB_0695 + 36, MOG_LAB_09D1);
    wl(m, MOG_LAB_0695 + 20, MOG_LAB_09D2);
    wl(m, MOG_LAB_0695 + 52, MOG_LAB_09D3);
    wl(m, MOG_LAB_0695 + 68, MOG_LAB_09D4);
    wl(m, MOG_LAB_0695 + 72, MOG_LAB_09D5);
    wl(m, MOG_LAB_0695 + 76, MOG_LAB_09D6);
    wl(m, MOG_LAB_0695 + 80, MOG_LAB_09D7);
    wl(m, MOG_LAB_0695 + 84, MOG_LAB_09D8);
    wl(m, MOG_LAB_0695 + 88, MOG_LAB_09D9);
    wl(m, MOG_LAB_0695 + 96, MOG_LAB_09DA);
    wl(m, MOG_LAB_0695 + 92, MOG_LAB_09DB);
    wl(m, MOG_LAB_0695 + 100, MOG_LAB_09DC);
    wl(m, MOG_LAB_0695 + 104, MOG_LAB_09DD);
    wl(m, MOG_LAB_0695 + 56, MOG_LAB_09DE);
    wl(m, MOG_LAB_0695 + 60, MOG_LAB_09DE);
    wl(m, MOG_LAB_0695 + 64, MOG_LAB_09DE);
    wl(m, MOG_LAB_0697 + 52, MOG_LAB_09DF);
    wl(m, MOG_LAB_0697 + 68, MOG_LAB_09E0);
    wl(m, MOG_LAB_0697 + 72, MOG_LAB_09E1);
    wl(m, MOG_LAB_0697 + 76, MOG_LAB_09E2);
    wl(m, MOG_LAB_0697 + 80, MOG_LAB_09E3);
    wl(m, MOG_LAB_0697 + 84, MOG_LAB_09E4);
    wl(m, MOG_LAB_0697 + 88, MOG_LAB_09E5);
    wl(m, MOG_LAB_0697 + 96, MOG_LAB_09E6);
    wl(m, MOG_LAB_0697 + 92, MOG_LAB_09E7);
    wl(m, MOG_LAB_0697 + 100, MOG_LAB_09E8);
    wl(m, MOG_LAB_0697 + 104, MOG_LAB_09E9);
    wl(m, MOG_LAB_0697 + 56, MOG_LAB_09EA);
    wl(m, MOG_LAB_0697 + 60, MOG_LAB_09EA);
    wl(m, MOG_LAB_0697 + 64, MOG_LAB_09EA);
    ww(m, MOG_LAB_0691 + 0, 0x0014);
    ww(m, MOG_LAB_0691 + 2, 0x0020);
    ww(m, MOG_LAB_0691 + 4, 0x0034);
    ww(m, MOG_LAB_0691 + 6, 0x0028);
    ww(m, MOG_LAB_0691 + 8, 0x0034);
    ww(m, MOG_LAB_0691 + 10, 0x0024);
    ww(m, MOG_LAB_0691 + 12, 0x0034);
    ww(m, MOG_LAB_0691 + 14, 0x0034);
    ww(m, MOG_LAB_0691 + 16, 0x0028);
    ww(m, MOG_LAB_0691 + 18, 0x0018);
    ww(m, MOG_LAB_0691 + 20, 0x000c);
    ww(m, MOG_LAB_0691 + 22, 0x0014);}

/* Une table de 27 enregistrements de texte (chaîne, X 0, Y 3, drapeaux) */
static void fill_texts(MogCombat *m, uint32_t dst, uint32_t src, uint16_t flag)
{
    uint16_t fl = (uint16_t)(3 | (src != MOG_LAB_0698 ? flag : 0));
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
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3));
    uint32_t a1 = rl(m, MOG_v_Combatants);
    wl(m, MOG_LAB_068B, a1);
    wl(m, MOG_LAB_068C, rl(m, a1 + 96));
    uint32_t kind = KIND;
    if (kind == 1 || kind == 8 || kind == 0x0B) {
        a1 = rl(m, MOG_v_Combatants + 4);
        wl(m, MOG_LAB_068D, a1);
        wl(m, MOG_LAB_068E, rl(m, a1 + 96));
    }
    uint32_t a0 = MOG_LAB_0698;
    if (kind == 6) a0 = MOG_LAB_0697;
    if (kind == 3) a0 = MOG_LAB_0696;
    if (kind == 2) {
        a0 = MOG_LAB_0698;
        wl(m, MOG_LAB_068D, rl(m, MOG_LAB_08C6));
        wl(m, MOG_LAB_068E, rl(m, rl(m, MOG_LAB_08C6)));
    }
    if (kind == 0x0A) {
        a0 = MOG_LAB_0698;
        wl(m, MOG_LAB_068D, MOG_LAB_0617);
        wl(m, MOG_LAB_068E, rl(m, MOG_LAB_0617 + 96));
    }
    if (kind == 9) a0 = MOG_LAB_0693;
    fill_texts(m, MOG_LAB_0699, a0, 0x10);
    /* expérience suffisante : les caractéristiques non au maximum
     * proposent leur amélioration (LAB_0692) */
    uint32_t k = rl(m, MOG_LAB_068B);
    if (!(sw(rw(m, MOG_LAB_06DE)) > sw(rw(m, k + 78)))) {
        uint32_t t = MOG_LAB_0699, n = MOG_LAB_0692;
        for (int i = 0; i < 3; i++, t += 14, n += 4)
            if (rb(m, k + 0x46 + (uint32_t)i) != 5) {
                wl(m, t, rl(m, n));
                ww(m, t + 8, rw(m, t + 8) ^ 0x40);
            }
    }
    a0 = MOG_LAB_0698;
    if (kind == 0x0A || kind == 1) a0 = MOG_LAB_0694;
    if (kind == 2)
        a0 = rw(m, MOG_LAB_065E) ? MOG_LAB_0698 : MOG_LAB_0694;
    if (kind == 5 || kind == 6) a0 = MOG_LAB_0695;
    if (kind == 8) a0 = MOG_LAB_0694;
    fill_texts(m, MOG_LAB_069A, a0, 0x20);
}

/* ------------------------------------------------------------------ */
/* Écran                                                               */
/* ------------------------------------------------------------------ */

/* LAB_0419 : copie d'écran (5 plans) */
static void copy_screen(MogCombat *m, uint32_t a0, uint32_t a1)
{
    MogBlitter *b = &m->blt;
    wl(m, MOG_LAB_0424, a0);
    wl(m, MOG_LAB_0425, a1);
    for (int p = 0; p < 5; p++) {
        b->apt = rl(m, MOG_LAB_0424);
        b->dpt = rl(m, MOG_LAB_0425);
        b->amod = b->dmod = 0;
        b->afwm = b->alwm = 0xFFFF;
        b->con0 = 0x09F0;
        b->con1 = 0;
        mog_blitter_run(VM, b, 0x3214);
        wl(m, MOG_LAB_0424, rl(m, MOG_LAB_0424) + 0x1F40);
        wl(m, MOG_LAB_0425, rl(m, MOG_LAB_0425) + 0x1F40);
    }
}

/* LAB_04D4 : l'écran construit (panneaux, icônes, zones), puis montré */
static void build(MogCombat *m)
{
    for (;;) {
        ww(m, MOG_LAB_0D05, 1);
        clear_zones(m);                                 /* LAB_044E */
        draw_frames(m);                                 /* LAB_04EA */
        copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
        mog_set_planes(m, rl(m, MOG_LAB_0D92));
        select_panels(m);                               /* LAB_058A */
        for (uint32_t i = 0; i < 0x2D0; i++)            /* LAB_03A7 */
            wb(m, MOG_LAB_064D + i, 0xFF);
        wl(m, MOG_LAB_0688, MOG_LAB_0699);
        wl(m, MOG_LAB_0632, rl(m, MOG_LAB_068B));
        ww(m, MOG_LAB_0985, 0);
        ww(m, MOG_LAB_0680, 0);
        for (uint32_t a2 = MOG_LAB_098A; sw(rw(m, a2)) >= 0; a2 += 2)
            if (rw(m, a2) == (uint16_t)KIND) {
                ww(m, MOG_LAB_0985, 0x4A);
                break;
            }
        draw_knight(m);                                 /* LAB_04F8 */
        wl(m, MOG_LAB_0688, MOG_LAB_069A);
        ww(m, MOG_LAB_0985, 0x96);
        uint32_t kind = KIND;
        if (kind == 2) {
            draw_creature(m);                           /* LAB_051B */
        } else if (kind == 1) {
            if (!((int8_t)rb(m, rl(m, MOG_LAB_068D) + 73) > 0))
                ww(m, MOG_LAB_0689, 0);
            if (rw(m, MOG_LAB_0689)) {                  /* butin pris : inventaire */
                wl(m, MOG_LAB_068F, 9);
                continue;
            }
            wl(m, MOG_LAB_0632, rl(m, MOG_LAB_068D));
            ww(m, MOG_LAB_0680, 1);
            draw_knight(m);
        } else if (kind == 0x0B || (kind == 8 && !rw(m, MOG_LAB_0689))) {
            wl(m, MOG_LAB_0632, rl(m, MOG_LAB_068D));
            ww(m, MOG_LAB_0680, 1);
            draw_knight(m);
            next_button(m);                             /* LAB_0524 */
        } else if (kind == 8) {                         /* LAB_04DC */
            ww(m, MOG_LAB_0689, 0);
            wl(m, MOG_LAB_068F, rl(m, MOG_LAB_068A));
            continue;
        } else {
            if (kind == 5)
                draw_shop(m);                           /* LAB_0522 */
            if (kind == 6) {
                wl(m, MOG_LAB_0632, MOG_LAB_0690);
                draw_inventory(m);                      /* LAB_04FE */
                magic_sword(m, rl(m, MOG_LAB_0632));
            }
            if (kind == 0x0A)
                draw_dragon(m);                         /* LAB_051D */
        }
        break;
    }
    panel_colours(m);                                   /* LAB_04E1 */
    mog_swap_screens(m);                                /* LAB_0416 */
    copy_screen(m, rl(m, MOG_SECSTRT_35), rl(m, MOG_LAB_0D92));
    uint16_t c[32];                                     /* LAB_0D8A, LAB_03EE */
    for (int i = 0; i < 32; i++)
        c[i] = rw(m, MOG_LAB_09F0 + 2u * (unsigned)i);
    mog_wait_vbls(m, 1);
    if (m->palette)
        m->palette(m->out.user, c);
    uint32_t cur = rl(m, MOG_LAB_0E93);
    for (uint32_t i = 0; i < 64; i++)
        wb(m, cur + i, rb(m, MOG_LAB_09F0 + i));
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
        ww(m, MOG_LAB_0526, 0);
    uint32_t k;
    do {
        uint16_t n = (uint16_t)((rw(m, MOG_LAB_0526) + 1) & 3);
        ww(m, MOG_LAB_0526, n);
        k = rl(m, MOG_LAB_0525 + 4u * n);
    } while (k == rl(m, MOG_LAB_068B));
    wl(m, MOG_v_Combatants + 4, k);
    wl(m, MOG_LAB_068D, k);
    wl(m, MOG_LAB_068E, rl(m, k + 96));
}

static void inc(MogCombat *m, uint32_t a) { wb(m, a, (uint8_t)(rb(m, a) + 1)); }
static void dec(MogCombat *m, uint32_t a) { wb(m, a, (uint8_t)(rb(m, a) - 1)); }

/* LAB_0542 : échange fait, chevalier remis à jour */
static void traded(MogCombat *m)
{
    ww(m, MOG_LAB_0689, (uint16_t)(rw(m, MOG_LAB_0689) + 1));
    uint32_t a0 = rl(m, MOG_LAB_068B);
    mog_knight_hp(m, a0);
    mog_knight_defence(m, a0);
    build(m);
}

/* LAB_0551 : l'épée magique prise (d1 = 4) */
static void take_sword(MogCombat *m, uint32_t a1)
{
    dec(m, a1 + 4);
    wl(m, rl(m, MOG_LAB_068B) + 88, 0x19);
    if (KIND != 2) {
        inc(m, rl(m, MOG_LAB_068C) + 4);
        wl(m, rl(m, MOG_LAB_068D) + 88, 0x16);
    }
    traded(m);
}

/* LAB_053F : prendre un objet de l'autre panneau */
static void take(MogCombat *m, uint16_t flags, uint16_t d1, uint8_t d3)
{
    uint32_t a0 = rl(m, MOG_LAB_068C), a1 = rl(m, MOG_LAB_068E);
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
        uint32_t k = rl(m, MOG_LAB_068B);
        ww(m, k + 80, (uint16_t)(rw(m, k + 80) + 0x14));
        mog_knight_hp(m, k);
    }
    traded(m);
}

/* LAB_0562 : achat (panneau de droite) ou vente au marchand LAB_0690 */
static void trade_shop(MogCombat *m, uint16_t d1, uint8_t d3)
{
    uint32_t a0 = rl(m, MOG_LAB_068C), a1 = MOG_LAB_0690, a2 = rl(m, MOG_LAB_068B);
    uint32_t a3 = MOG_LAB_0691;
    int16_t price = sw(rw(m, a3 + d1));
    if (!(sw(rw(m, MOG_LAB_097F)) < 0xA0)) {
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
        wl(m, rl(m, MOG_LAB_068B) + 88, 0x16);
    mog_knight_hp(m, a2);
    build(m);
    mog_message(m, "Selling");
}

/* LAB_052D : objet utilisé (potions, sorts, anneaux...) */
static void use(MogCombat *m, uint16_t flags, uint16_t d1, uint8_t d3)
{
    ww(m, MOG_LAB_053B, 0xFFFF);
    uint32_t a0 = rl(m, MOG_LAB_068C);
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
    ww(m, MOG_LAB_053B, d1);
    if (KIND == 3) {
        mog_sound(m, 0x9C);
        dec(m, a0 + d1);
        ww(m, MOG_LAB_053B, d1);
        ww(m, MOG_LAB_0984, 1);
        build(m);
        return;
    }
    uint32_t k = rl(m, MOG_LAB_068B);
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
            ww(m, MOG_LAB_0665, (uint16_t)(rw(m, MOG_LAB_0665) << 1));
        } else {
            bad_sound(m);
            ww(m, MOG_LAB_05D3, 1);
            ww(m, MOG_LAB_0665, (uint16_t)(rw(m, MOG_LAB_0665) >> 1));
        }
        break;
    case 0x0E:                                          /* échange avec un autre */
        wl(m, MOG_LAB_068A, KIND);
        good_sound(m);
        next_knight(m, 1);
        mog_screen_run(m, 8);
        return;
    case 2:
        good_sound(m);
        mog_map_0E02(m);
        ww(m, MOG_LAB_0984, 1);
        break;
    case 0x0C:
        if (d100(m) > 0x0F) {
            good_sound(m);
            mog_map_0E05(m, k);
        } else {
            bad_sound(m);
            mog_map_0E06(m);
        }
        ww(m, MOG_LAB_0984, 1);
        break;
    case 0x10:
        good_sound(m);
        ww(m, MOG_LAB_053C, 1);
        next_knight(m, 1);
        mog_screen_run(m, 0x0B);
        return;
    case 0x12:
        ww(m, MOG_LAB_0984, 1);
        if (d100(m) > 0x0A) {
            good_sound(m);
        } else {
            bad_sound(m);
            ww(m, MOG_LAB_05D3, 1);
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
        ww(m, MOG_LAB_0984, 1);
        return;
    }
    if (rl(m, z + 8) == MOG_LAB_09EF) {                 /* chevalier suivant */
        next_knight(m, 0);
        build(m);
        return;
    }
    uint16_t flags = rw(m, rl(m, z + 8) + 8);
    uint16_t d1 = rw(m, z + 22), d2 = rw(m, z + 20);
    uint8_t d3 = (uint8_t)((d2 & 0xF0) >> 4);
    d2 &= 0x0F;
    uint32_t a0 = rl(m, MOG_LAB_068B), a1 = rl(m, MOG_LAB_068D);
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
            ww(m, a0 + 78, (uint16_t)(rw(m, a0 + 78) - rw(m, MOG_LAB_06DE)));
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
            ww(m, MOG_LAB_0689, (uint16_t)(rw(m, MOG_LAB_0689) + 1));
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
        a1 = rl(m, MOG_LAB_068C);
        mog_sound(m, 0x9C);
        if (d1 == 4)
            wl(m, a0 + 88, 0x16);
        dec(m, a1 + d1);
        ww(m, MOG_LAB_053B, d1);
        ww(m, MOG_LAB_0984, 1);
        traded(m);
        return;
    }
    if (!(flags & 0x20))
        return;
    switch (d1) {                                       /* LAB_054A */
    case 0x4A: {                                        /* or */
        uint32_t g = KIND == 2 ? rl(m, MOG_LAB_08C6) + 8 : a1 + 74;
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
            ww(m, MOG_LAB_0689, (uint16_t)(rw(m, MOG_LAB_0689) + 1));
        }
        build(m);
        return;
    }
    case 0x5C: {                                        /* armure */
        uint32_t d5 = rl(m, a0 + 92);
        if ((int32_t)d5 < (int32_t)rl(m, a1 + 92)) {
            wl(m, a0 + 92, rl(m, a1 + 92));
            uint16_t v = rw(m, MOG_LAB_054D + (d5 - 0x1B));
            ww(m, a1 + 80, (uint16_t)(rw(m, a1 + 80) - v));
            ww(m, a0 + 80, (uint16_t)(rw(m, a0 + 80) + v));
            wl(m, a1 + 92, 0x1B);
            ww(m, MOG_LAB_0689, (uint16_t)(rw(m, MOG_LAB_0689) + 1));
        }
        build(m);
        return;
    }
    case 0x58: {                                        /* arme */
        uint32_t d5 = rl(m, a0 + 88);
        if (d5 == 0x19) {
            take_sword(m, rl(m, MOG_LAB_068E));
            return;
        }
        if ((int32_t)d5 < (int32_t)rl(m, a1 + 88)) {
            wl(m, a0 + 88, rl(m, a1 + 88));
            wl(m, a1 + 88, d5);
            ww(m, MOG_LAB_0689, (uint16_t)(rw(m, MOG_LAB_0689) + 1));
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
    ww(m, MOG_LAB_0D05, 1);                             /* LAB_04CF */
    wl(m, MOG_LAB_0986, rl(m, MOG_LAB_05E2 + 4));
    ww(m, MOG_LAB_0689, 0);
    wl(m, MOG_LAB_068F, kind);
    ww(m, MOG_LAB_0984, 0);
    mog_fade_black(m);                                  /* LAB_03F0 */
    pointer_on(m);                                      /* LAB_0575 */
    text_tables(m);                                     /* LAB_0588 */
    build(m);                                           /* LAB_04D4 */
    for (;;) {                                          /* LAB_04D0 */
        if (m->frame_start)             /* point de rendez-vous (pas dans mog) */
            m->frame_start(m->out.user);
        uint32_t z = zone_at(m, rw(m, MOG_LAB_097F), rw(m, MOG_LAB_0980));
        if (z && !rw(m, MOG_LAB_0981)) {
            click(m, z);
            if (rw(m, MOG_LAB_0984))
                break;
        }
        mog_swap_screens(m);
        mog_restore_areas(m);
    }
    mog_fade_black(m);                                  /* LAB_04D2 */
    pointer_off(m);
    if (rw(m, MOG_LAB_053C)) {
        wl(m, MOG_LAB_0617 + 100, rl(m, MOG_v_Combatants + 4));
        ww(m, MOG_LAB_053C, 0);
    }
    ww(m, MOG_LAB_0D05, 0);
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
