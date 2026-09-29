/*
 * mog_town.c — lieux de la carte de mog (LAB_007B) : villes et marchés,
 * auberges, repaire du Démon, temple de la Pierre de lune... traduits de
 * amiga_asm/mog.asm.
 *
 * Les menus des villes sont des boucles d'attente sans VBL (le pointeur
 * bouge par l'interruption LAB_057D) : on y compte une VBL par tour, puis
 * le rendez-vous de l'hôte (frame_start).
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

#define CUR (rl(m, MOG_LAB_0633))

static void todo(MogCombat *m, const char *what)
{
    char buf[160];
    snprintf(buf, sizeof buf, "carte : %s non porté", what);
    mog_message(m, buf);
    m->errors++;
}

/* Tour d'une boucle d'attente de l'original : une VBL, puis l'hôte */
static void busy_tick(MogCombat *m)
{
    mog_wait_vbls(m, 1);
    if (m->frame_start)
        m->frame_start(m->out.user);
}

/* LAB_0D8D sur SECSTRT_21 : D0 = touche << 16 | caractère */
static uint32_t key_char(MogCombat *m)
{
    uint16_t k = rw(m, MOG_SECSTRT_21);
    return (uint32_t)k << 16 | rb(m, MOG_LAB_0D99 + k);
}

/* Zone modèle LAB_0A58 : champs x, y, l, h, texte, identifiant, genre, case */
static void zone_xy(MogCombat *m, uint16_t x, uint16_t y) { ww(m, MOG_LAB_0A58 + 12, x); ww(m, MOG_LAB_0A58 + 14, y); }
static void zone_wh(MogCombat *m, uint16_t w, uint16_t h) { ww(m, MOG_LAB_0A58 + 4, w); ww(m, MOG_LAB_0A58 + 6, h); }
static void zone_id(MogCombat *m, uint32_t id) { wl(m, MOG_LAB_0A58 + 16, id); }

/* LAB_0F8C sur les quatre canaux : sons n .. n + 3 */
static void voices(MogCombat *m, int n)
{
    for (int ch = 0; ch < 4; ch++)
        if (m->voice)
            m->voice(m->out.user, ch, n + ch);
}

/* ------------------------------------------------------------------ */
/* Guérisseur et maître d'armes : or proposé (LAB_0495)                */
/* ------------------------------------------------------------------ */

/* LAB_049D : fond LAB_05C0 dans l'écran de dessin */
static void back_to_draw(MogCombat *m)
{
    mog_copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
}

/* LAB_049B : bourses (LAB_0977 au chevalier, LAB_0976 proposé), boutons */
static void offer_draw(MogCombat *m)
{
    back_to_draw(m);
    ww(m, MOG_LAB_0D05, 1);
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    uint32_t cel = rl(m, MOG_LAB_05C2);
    uint16_t f = rw(m, MOG_LAB_091A);
    mog_draw_cel(VM, &m->blt, cel, f, 2, 0xA2);
    mog_draw_cel(VM, &m->blt, cel, f, 0x10E, 0xA2);
    mog_draw_cel(VM, &m->blt, cel, 2, 0xAD, 0xB9);
    mog_draw_cel(VM, &m->blt, cel, 3, 0x83, 0xB9);
    mog_draw_cel(VM, &m->blt, cel, 4, 0x90, 0xA9);
    mog_draw_cel(VM, &m->blt, cel, 5, 0xA2, 0xA9);
    mog_text(m, MOG_LAB_0972, 0x106, 0xBE, 4);
    mog_text(m, MOG_LAB_0971, 2, 0xBE, 0);
    mog_number(m, rw(m, MOG_LAB_0977), MOG_LAB_0975);  /* D0.h = 0 (LAB_0431) */
    mog_text(m, MOG_LAB_0975, 0x14, 0xAF, 0);
    mog_number(m, rw(m, MOG_LAB_0976), MOG_LAB_0975);
    mog_text(m, MOG_LAB_0975, 0x120, 0xAF, 0);
    mog_swap_screens(m);                                /* LAB_0416 */
    ww(m, MOG_LAB_0D05, 0);
}

/* LAB_0495 : or proposé, pièce par pièce (d0 : image des bourses) ;
 * renvoie le bouton (2 : annuler, 3 : accepter) */
static uint16_t offer(MogCombat *m, uint16_t d0)
{
    ww(m, MOG_LAB_0977, rw(m, CUR + 74));
    ww(m, MOG_LAB_0976, 0);
    ww(m, MOG_LAB_091A, d0);
    mog_clear_zones(m);                                 /* LAB_044E */
    zone_xy(m, 0x90, 0xA9);
    zone_wh(m, 0x0E, 8);
    wl(m, MOG_LAB_0A58 + 8, 0);
    zone_id(m, 1);
    ww(m, MOG_LAB_0A58 + 20, 4);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    mog_add_zone(m);
    ww(m, MOG_LAB_0A58 + 12, 0xA2);
    ww(m, MOG_LAB_0A58 + 20, 5);
    mog_add_zone(m);
    zone_xy(m, 0x83, 0xB9);
    zone_wh(m, 0x14, 8);
    ww(m, MOG_LAB_0A58 + 20, 3);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    mog_add_zone(m);
    zone_xy(m, 0xAD, 0xB9);
    zone_wh(m, 0x20, 8);
    ww(m, MOG_LAB_0A58 + 20, 2);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    mog_add_zone(m);
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3));
    ww(m, MOG_LAB_0D05, 1);
    offer_draw(m);
    for (;;) {                                          /* LAB_0496 */
        busy_tick(m);
        uint32_t z = mog_zone_at(m, rw(m, MOG_LAB_097F), rw(m, MOG_LAB_0980));
        if (!(uint16_t)z || rw(m, MOG_LAB_0981))
            continue;
        uint16_t k = rw(m, z + 20);
        switch (k) {
        case 2:                                         /* LAB_0497 */
            ww(m, MOG_LAB_0976, 0);
            ww(m, MOG_LAB_0977, 0);
            return k;
        case 3:                                         /* LAB_0498 */
            ww(m, CUR + 74, rw(m, MOG_LAB_0977));
            return k;
        case 4:                                         /* LAB_0499 : reprendre */
            if (!rw(m, MOG_LAB_0976))
                continue;
            ww(m, MOG_LAB_0976, (uint16_t)(rw(m, MOG_LAB_0976) - 1));
            ww(m, MOG_LAB_0977, (uint16_t)(rw(m, MOG_LAB_0977) + 1));
            break;
        case 5:                                         /* LAB_049A : donner */
            if (!rw(m, MOG_LAB_0977))
                continue;
            ww(m, MOG_LAB_0976, (uint16_t)(rw(m, MOG_LAB_0976) + 1));
            ww(m, MOG_LAB_0977, (uint16_t)(rw(m, MOG_LAB_0977) - 1));
            break;
        default:
            continue;
        }
        offer_draw(m);
        mog_wait_vbls(m, 2);
    }
}

/* Entrée commune de LAB_047C et LAB_048E : image, pointeur, texte a0,
 * sons, attente du feu ; puis l'or proposé (bourses d0) */
static uint16_t shop_enter(MogCombat *m, uint32_t picture, uint32_t text, int snd, uint16_t d0, int blit_wait)
{
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3));
    mog_set_planes(m, rl(m, MOG_LAB_05C0));
    mog_load_picture(m, picture, rl(m, MOG_LAB_05C2)); /* LAB_0C27 */
    mog_load_cel(VM, MOG_LAB_091D, rl(m, MOG_LAB_05C2));   /* LAB_049C */
    mog_fade_black(m);                                  /* LAB_03F0 */
    ww(m, MOG_LAB_097F, 0xA0);
    ww(m, MOG_LAB_0980, 0xAA);
    mog_pointer_on(m);                                  /* LAB_0575 */
    mog_show_background(m);                             /* LAB_0418 */
    (void)blit_wait;                                    /* LAB_0D1B : blitter libre */
    mog_swap_screens(m);                                /* LAB_0416 */
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    mog_text_records(m, text);                          /* LAB_0432 */
    mog_swap_screens(m);
    voices(m, snd);                                     /* LAB_0F8C */
    mog_fade_to(m, MOG_LAB_0D2B);                       /* LAB_03F2 */
    mog_wait_fire(m);                                   /* LAB_00EC */
    uint16_t k = offer(m, d0);                          /* LAB_0495 */
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    back_to_draw(m);                                    /* LAB_049D / LAB_0419 */
    return k;
}

/* LAB_0487 : message a0 montré, puis sortie */
static void shop_leave(MogCombat *m, uint32_t text)
{
    mog_text_records(m, text);                          /* LAB_0432 */
    mog_swap_screens(m);                                /* LAB_0416 */
    mog_wait_vbls(m, 0x32);
    mog_wait_fire(m);                                   /* LAB_00EC */
    mog_fade_out(m);                                    /* LAB_03F1 */
    mog_pointer_off(m);                                 /* LAB_057B */
}

/* LAB_048E : guérisseur (10 pièces : PV, 15 : une vie ; poison guéri) */
static void healer(MogCombat *m)
{
    uint16_t k = shop_enter(m, MOG_LAB_091B, MOG_LAB_0935, 0x79, 1, 0);
    if (k != 3) {
        shop_leave(m, MOG_LAB_0938);                    /* LAB_0485 */
        return;
    }
    uint16_t d1 = rw(m, MOG_LAB_0976);
    if (!d1) {
        shop_leave(m, MOG_LAB_0939);                    /* LAB_0484 */
        return;
    }
    unsigned d2 = 0;
    for (;;) {                                          /* LAB_048F */
        uint32_t a0 = CUR;
        if ((int16_t)rw(m, MOG_LAB_0976) < 0x0A)
            break;
        if (rb(m, a0 + 130)) {
            wb(m, a0 + 130, 0);
            d2 |= 1;
        }
        if (rw(m, a0 + 80) != rw(m, a0 + 84)) {        /* LAB_0490 */
            ww(m, a0 + 80, rw(m, a0 + 84));
            ww(m, MOG_LAB_0976, (uint16_t)(rw(m, MOG_LAB_0976) - 0x0A));
            d2 |= 2;
            continue;
        }
        if (rb(m, a0 + 73) == 5)                        /* LAB_0491 */
            break;
        wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        ww(m, MOG_LAB_0976, (uint16_t)(rw(m, MOG_LAB_0976) - 0x0F));
        d2 |= 4;
    }
    uint32_t t = MOG_LAB_093A;                          /* LAB_0492 */
    if ((int16_t)d1 > 9)
        t = (d2 & 1) ? MOG_LAB_093D : MOG_LAB_093C;
    shop_leave(m, t);
}

/* LAB_0489 : l'entraînement réussit-il ? (d100 + bonus selon l'or) */
static int training_ok(MogCombat *m)
{
    uint32_t d0 = mog_d100(m);                          /* LAB_04A3 */
    int16_t d1 = (int16_t)rw(m, MOG_LAB_0976);
    uint32_t a0 = MOG_LAB_048D;
    for (int i = 0; i < 6; i++) {
        if (d1 <= (int16_t)rw(m, a0))
            break;
        a0 += 4;
    }
    uint16_t v = (uint16_t)(d0 + rw(m, a0 + 2));
    return !((int16_t)v < 0x32);
}

/* LAB_047C : maître d'armes (une caractéristique gagnée ou perdue) */
static void trainer(MogCombat *m)
{
    uint16_t k = shop_enter(m, MOG_LAB_091C, MOG_LAB_0947, 0x74, 0, 1);
    if (!rw(m, MOG_LAB_0976)) {
        shop_leave(m, MOG_LAB_0939);                    /* LAB_0484 */
        return;
    }
    if (k != 3) {
        shop_leave(m, MOG_LAB_0938);                    /* LAB_0485 */
        return;
    }
    uint32_t d1 = mog_pick_stat(m);                     /* LAB_0469 */
    uint32_t a1 = CUR;          /* (toutes au maximum : A1 d'avant, pris = CUR) */
    uint32_t tbl;
    if (training_ok(m)) {
        if (!d1) {
            shop_leave(m, MOG_LAB_0956);
            return;
        }
        wb(m, a1 + d1, (uint8_t)(rb(m, a1 + d1) + 1)); /* LAB_047D */
        if (d1 == 0x47)
            ww(m, a1 + 80, (uint16_t)(rw(m, a1 + 80) + 0x0A));
        mog_knight_hp(m, a1);                           /* LAB_047E : LAB_0013 */
        mog_knight_defence(m, a1);                      /* LAB_0019 */
        tbl = MOG_LAB_095B;
    } else {
        if (rb(m, a1 + d1) == 1) {                      /* LAB_047F */
            shop_leave(m, MOG_LAB_0959);                /* LAB_0486 */
            return;
        }
        wb(m, a1 + d1, (uint8_t)(rb(m, a1 + d1) - 1));
        uint32_t a0 = CUR;
        if (d1 == 0x47) {
            uint16_t v = (uint16_t)(rw(m, a0 + 80) - 0x0A);
            ww(m, a0 + 80, v);
            if (!((int16_t)v > 0))
                ww(m, a0 + 80, 1);
        }
        mog_knight_defence(m, a0);                      /* LAB_0480 : LAB_0019 */
        mog_knight_hp(m, a0);                           /* LAB_0013 */
        tbl = MOG_LAB_095C;
    }
    wl(m, MOG_LAB_0488, tbl);
    for (uint32_t a0 = tbl, i = 0; i < 3; i++, a0 += 8)    /* LAB_0481 */
        if (rl(m, a0) == d1) {
            shop_leave(m, rl(m, a0 + 4));               /* LAB_0483 */
            return;
        }
    shop_leave(m, MOG_LAB_0959);                        /* LAB_0486 */
}

/* ------------------------------------------------------------------ */
/* Villes                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0130 : image de la ville `name` dans LAB_0704 (= LAB_05C1),
 * palette LAB_05B7, fond LAB_05C0 */
static void town_picture(MogCombat *m, uint32_t name)
{
    mog_load_picture(m, name, rl(m, MOG_LAB_05C2));     /* LAB_0C27 */
    for (uint32_t i = 0; i < 32; i++)                   /* LAB_0422 */
        ww(m, MOG_LAB_05B7 + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    mog_fade_black(m);                                  /* LAB_03F0 */
    mog_copy_screen(m, rl(m, MOG_LAB_0704), rl(m, MOG_LAB_05C0));  /* LAB_0419 */
    mog_show_background(m);                             /* LAB_0418 */
}

/* LAB_012E / LAB_012F : écran d'attente puis image de la ville */
static void town_enter(MogCombat *m, uint32_t text, uint32_t name)
{
    /* LAB_0100 (disquette 3) : sans objet */
    wl(m, MOG_LAB_0713, text);
    mog_message_screen(m, MOG_LAB_070F, 0);             /* LAB_0136 */
    wl(m, MOG_LAB_0704, rl(m, MOG_LAB_05C1));
    mog_set_planes(m, rl(m, MOG_LAB_0704));
    town_picture(m, name);
}

/* LAB_009B (d = 0) / LAB_009C (d = 1) : les cinq enseignes */
static void town_zones(MogCombat *m, int kind)
{
    static const uint16_t ys[2][5] = {
        { 0x1E, 0x42, 0x6A, 0x8C, 0xB7 },
        { 0x1A, 0x3F, 0x65, 0x86, 0xB6 },
    };
    static const uint16_t hs[2][5] = {
        { 0x10, 0x10, 0x10, 0x1A, 0x0C },
        { 0x10, 0x10, 0x10, 0x1F, 0x0C },
    };
    mog_clear_zones(m);                                 /* LAB_044E */
    zone_xy(m, kind ? 0 : 0x100, ys[kind][0]);
    zone_wh(m, 0x40, 0x10);
    wl(m, MOG_LAB_0A58 + 8, 0);
    ww(m, MOG_LAB_0A58 + 20, 1);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    for (int i = 0; i < 5; i++) {
        ww(m, MOG_LAB_0A58 + 14, ys[kind][i]);
        ww(m, MOG_LAB_0A58 + 6, hs[kind][i]);
        zone_id(m, (uint32_t)i + 1);
        mog_add_zone(m);                                /* LAB_0448 */
    }
}

/* LAB_00B2 : fin du tour, retour sur la carte (SECSTRT_36) */
static int leave(MogCombat *m)
{
    ww(m, MOG_LAB_0655, rw(m, MOG_LAB_0665));
    mog_back_to_map(m);
    return MOG_MAP_ENTER;                               /* D0 = $FFFF (LAB_0011) */
}

/* LAB_00B1 : inventaire (écran 9) puis LAB_00B2 */
static int inventory_leave(MogCombat *m)
{
    mog_screen_run(m, 9);
    return leave(m);
}

/* LAB_008A (d = 1, $1A) / LAB_0093 (d = 0, $19) : menu de la ville */
static int town_menu(MogCombat *m, int kind)
{
    if (kind)
        town_enter(m, MOG_LAB_071C, MOG_LAB_0716);      /* LAB_012F */
    else
        town_enter(m, MOG_LAB_071B, MOG_LAB_0715);      /* LAB_012E */
    for (;;) {
        ww(m, MOG_LAB_097F, kind ? 0x1E : 0x122);       /* LAB_008B / LAB_0094 */
        ww(m, MOG_LAB_0980, 0x64);
        mog_pointer_on(m);                              /* LAB_0575 */
        town_zones(m, kind);
        mog_copy_screen(m, rl(m, MOG_LAB_0704), rl(m, MOG_LAB_05C0));
        mog_show_background(m);                         /* LAB_0418 */
        mog_fade_to(m, MOG_LAB_05B7);                   /* LAB_03F2 */
        mog_clear_keys(m);                              /* LAB_0B82 */
        uint32_t id = 0, screen = 0;
        for (;;) {                                      /* LAB_008C / LAB_0095 */
            busy_tick(m);
            uint32_t z = mog_zone_at(m, rw(m, MOG_LAB_097F), rw(m, MOG_LAB_0980));
            if ((uint16_t)z && (mog_read_joy(m) & MOG_JOY_FIRE)) {   /* TST.W D0 */
                id = rl(m, z + 16);
                if (id >= 1 && id <= 5)
                    break;
            }
            uint32_t c = key_char(m);                   /* LAB_008D / LAB_0096 */
            if ((uint16_t)c == 0x20) {
                screen = kind ? c : 9;
                break;
            }
        }
        if (screen) {                                   /* barre d'espace */
            mog_fade_black(m);
            mog_pointer_off(m);
            mog_screen_run(m, screen);
            continue;
        }
        switch (id) {
        case 1:                                         /* LAB_0090 / LAB_0098 */
            mog_fade_black(m);
            mog_pointer_off(m);
            mog_screen_run(m, 5);
            break;
        case 2:                                         /* LAB_008F / LAB_0097 */
            mog_pointer_off(m);
            todo(m, "LAB_04A6 (jeu de hasard)");
            return MOG_MAP_UNPORTED;
        case 3:                                         /* LAB_0091 / LAB_009A */
            mog_fade_black(m);
            mog_pointer_off(m);
            healer(m);                                  /* LAB_048E */
            mog_fade_black(m);
            break;
        case 4:
            if (kind) {                                 /* LAB_008E */
                mog_pointer_off(m);
                trainer(m);                             /* LAB_047C */
                mog_fade_black(m);
                break;
            }
            mog_fade_black(m);                          /* LAB_0099 */
            mog_pointer_off(m);
            mog_screen_run(m, 6);
            break;
        default:                                        /* 5 : LAB_0092 */
            mog_pointer_off(m);
            return leave(m);
        }
    }
}

/* ------------------------------------------------------------------ */
/* LAB_007B                                                            */
/* ------------------------------------------------------------------ */

int mog_town(MogCombat *m, uint32_t d0)
{
    mog_map_colours_off(m);                             /* LAB_0DC8 */
    ww(m, MOG_LAB_0D04, 4);                             /* SECSTRT_28, D7 = 5 */
    ww(m, MOG_LAB_0D4C, 0);
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3));
    mog_select_knight(m);                               /* LAB_0113 : LAB_0DBD */
    mog_before_combat(m);                               /* LAB_0065 */
    switch ((uint16_t)d0) {
    case 0x15: case 0x16: case 0x17: case 0x18: {       /* LAB_00B0 : auberge */
        uint32_t a0 = CUR;
        if ((int8_t)rb(m, a0 + 73) < 3)
            wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        return inventory_leave(m);
    }
    case 0x19:
        return town_menu(m, 0);                         /* LAB_0093 */
    case 0x1A:
        return town_menu(m, 1);                         /* LAB_008A */
    case 0x1B:
        todo(m, "LAB_00A1 (temple de la Pierre de lune)");
        return MOG_MAP_UNPORTED;
    case 0x1C:
        todo(m, "LAB_009D (repaire du Démon)");
        return MOG_MAP_UNPORTED;
    case 0x1E:
        todo(m, "LAB_007C (LAB_0456)");
        return MOG_MAP_UNPORTED;
    case 0x21:
        todo(m, "LAB_007B $21 (duel)");
        return MOG_MAP_UNPORTED;
    }
    return (uint16_t)d0 ? MOG_MAP_ENTER : 0;            /* D0 inchangé */
}
