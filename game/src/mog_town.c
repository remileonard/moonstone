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
#include "ix_mog_names.h"
#include "mog_sound.h"

#include <stdio.h>

#define VM (m->eng.vm)

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }

#define CUR (rl(m, MOG_v_CurObj))

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
        mog_snd_play(m, n + ch, ch);                    /* LAB_0F8C */
}

/* ------------------------------------------------------------------ */
/* Guérisseur et maître d'armes : or proposé (LAB_0495)                */
/* ------------------------------------------------------------------ */

/* LAB_049D : fond LAB_05C0 dans l'écran de dessin */
static void back_to_draw(MogCombat *m)
{
    mog_copy_screen(m, rl(m, MOG_v_BgPlanes), rl(m, MOG_LAB_0D92));
}

/* LAB_049B : bourses (LAB_0977 au chevalier, LAB_0976 proposé), boutons */
static void offer_draw(MogCombat *m)
{
    back_to_draw(m);
    ww(m, MOG_v_BlitByCpu, 1);
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    uint32_t cel = rl(m, MOG_b_Piv);
    uint16_t f = rw(m, MOG_v_PurseFrame);
    mog_draw_cel(VM, &m->blt, cel, f, 2, 0xA2);
    mog_draw_cel(VM, &m->blt, cel, f, 0x10E, 0xA2);
    mog_draw_cel(VM, &m->blt, cel, 2, 0xAD, 0xB9);
    mog_draw_cel(VM, &m->blt, cel, 3, 0x83, 0xB9);
    mog_draw_cel(VM, &m->blt, cel, 4, 0x90, 0xA9);
    mog_draw_cel(VM, &m->blt, cel, 5, 0xA2, 0xA9);
    mog_text(m, MOG_s_Donation, 0x106, 0xBE, 4);
    mog_text(m, MOG_s_YourGold, 2, 0xBE, 0);
    mog_number(m, rw(m, MOG_v_GoldLeft), MOG_b_NumberText);  /* D0.h = 0 (LAB_0431) */
    mog_text(m, MOG_b_NumberText, 0x14, 0xAF, 0);
    mog_number(m, rw(m, MOG_v_GoldOffered), MOG_b_NumberText);
    mog_text(m, MOG_b_NumberText, 0x120, 0xAF, 0);
    mog_swap_screens(m);                                /* LAB_0416 */
    ww(m, MOG_v_BlitByCpu, 0);
}

/* LAB_0495 : or proposé, pièce par pièce (d0 : image des bourses) ;
 * renvoie le bouton (2 : annuler, 3 : accepter) */
static uint16_t offer(MogCombat *m, uint16_t d0)
{
    ww(m, MOG_v_GoldLeft, rw(m, CUR + 74));
    ww(m, MOG_v_GoldOffered, 0);
    ww(m, MOG_v_PurseFrame, d0);
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
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    ww(m, MOG_v_BlitByCpu, 1);
    offer_draw(m);
    for (;;) {                                          /* LAB_0496 */
        busy_tick(m);
        uint32_t z = mog_zone_at(m, rw(m, MOG_v_PointerX), rw(m, MOG_v_PointerY));
        if (!(uint16_t)z || rw(m, MOG_v_PointerOn))
            continue;
        uint16_t k = rw(m, z + 20);
        switch (k) {
        case 2:                                         /* LAB_0497 */
            ww(m, MOG_v_GoldOffered, 0);
            ww(m, MOG_v_GoldLeft, 0);
            return k;
        case 3:                                         /* LAB_0498 */
            ww(m, CUR + 74, rw(m, MOG_v_GoldLeft));
            return k;
        case 4:                                         /* LAB_0499 : reprendre */
            if (!rw(m, MOG_v_GoldOffered))
                continue;
            ww(m, MOG_v_GoldOffered, (uint16_t)(rw(m, MOG_v_GoldOffered) - 1));
            ww(m, MOG_v_GoldLeft, (uint16_t)(rw(m, MOG_v_GoldLeft) + 1));
            break;
        case 5:                                         /* LAB_049A : donner */
            if (!rw(m, MOG_v_GoldLeft))
                continue;
            ww(m, MOG_v_GoldOffered, (uint16_t)(rw(m, MOG_v_GoldOffered) + 1));
            ww(m, MOG_v_GoldLeft, (uint16_t)(rw(m, MOG_v_GoldLeft) - 1));
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
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    mog_load_picture(m, picture, rl(m, MOG_b_Piv)); /* LAB_0C27 */
    mog_load_cel(VM, MOG_s_MysCel, rl(m, MOG_b_Piv));   /* LAB_049C */
    mog_fade_black(m);                                  /* LAB_03F0 */
    ww(m, MOG_v_PointerX, 0xA0);
    ww(m, MOG_v_PointerY, 0xAA);
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
    uint16_t k = shop_enter(m, MOG_s_HeaPiv, MOG_t_HealerText, 0x79, 1, 0);
    if (k != 3) {
        shop_leave(m, MOG_t_ShopCancelled);                    /* LAB_0485 */
        return;
    }
    uint16_t d1 = rw(m, MOG_v_GoldOffered);
    if (!d1) {
        shop_leave(m, MOG_t_ShopNoGold);                    /* LAB_0484 */
        return;
    }
    unsigned d2 = 0;
    for (;;) {                                          /* LAB_048F */
        uint32_t a0 = CUR;
        if ((int16_t)rw(m, MOG_v_GoldOffered) < 0x0A)
            break;
        if (rb(m, a0 + 130)) {
            wb(m, a0 + 130, 0);
            d2 |= 1;
        }
        if (rw(m, a0 + 80) != rw(m, a0 + 84)) {        /* LAB_0490 */
            ww(m, a0 + 80, rw(m, a0 + 84));
            ww(m, MOG_v_GoldOffered, (uint16_t)(rw(m, MOG_v_GoldOffered) - 0x0A));
            d2 |= 2;
            continue;
        }
        if (rb(m, a0 + 73) == 5)                        /* LAB_0491 */
            break;
        wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        ww(m, MOG_v_GoldOffered, (uint16_t)(rw(m, MOG_v_GoldOffered) - 0x0F));
        d2 |= 4;
    }
    uint32_t t = MOG_t_HealerTooLittle;                          /* LAB_0492 */
    if ((int16_t)d1 > 9)
        t = (d2 & 1) ? MOG_t_HealerCured : MOG_t_HealerHealed;
    shop_leave(m, t);
}

/* LAB_0489 : l'entraînement réussit-il ? (d100 + bonus selon l'or) */
static int training_ok(MogCombat *m)
{
    uint32_t d0 = mog_d100(m);                          /* LAB_04A3 */
    int16_t d1 = (int16_t)rw(m, MOG_v_GoldOffered);
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
    uint16_t k = shop_enter(m, MOG_s_MysPiv, MOG_t_MasterText, 0x74, 0, 1);
    if (!rw(m, MOG_v_GoldOffered)) {
        shop_leave(m, MOG_t_ShopNoGold);                    /* LAB_0484 */
        return;
    }
    if (k != 3) {
        shop_leave(m, MOG_t_ShopCancelled);                    /* LAB_0485 */
        return;
    }
    uint32_t d1 = mog_pick_stat(m);                     /* LAB_0469 */
    uint32_t a1 = CUR;          /* (toutes au maximum : A1 d'avant, pris = CUR) */
    uint32_t tbl;
    if (training_ok(m)) {
        if (!d1) {
            shop_leave(m, MOG_t_MasterMaxed);
            return;
        }
        wb(m, a1 + d1, (uint8_t)(rb(m, a1 + d1) + 1)); /* LAB_047D */
        if (d1 == 0x47)
            ww(m, a1 + 80, (uint16_t)(rw(m, a1 + 80) + 0x0A));
        mog_knight_hp(m, a1);                           /* LAB_047E : LAB_0013 */
        mog_knight_defence(m, a1);                      /* LAB_0019 */
        tbl = MOG_t_MasterGainTexts;
    } else {
        if (rb(m, a1 + d1) == 1) {                      /* LAB_047F */
            shop_leave(m, MOG_t_MasterLost);                /* LAB_0486 */
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
        tbl = MOG_t_MasterLossTexts;
    }
    wl(m, MOG_LAB_0488, tbl);
    for (uint32_t a0 = tbl, i = 0; i < 3; i++, a0 += 8)    /* LAB_0481 */
        if (rl(m, a0) == d1) {
            shop_leave(m, rl(m, a0 + 4));               /* LAB_0483 */
            return;
        }
    shop_leave(m, MOG_t_MasterLost);                        /* LAB_0486 */
}

/* ------------------------------------------------------------------ */
/* Jeu de hasard (LAB_04A6)                                            */
/* ------------------------------------------------------------------ */

/* LAB_03EB : couleurs à zéro (33 mots, comme l'original) */
static void black_palette(MogCombat *m)
{
    uint32_t a2 = rl(m, MOG_v_PalCurrent);
    for (int i = 0; i < 33; i++)
        ww(m, a2 + 2u * (unsigned)i, 0);
}

/* LAB_0D8A (une VBL, registres couleur) puis LAB_03EE (palette courante) */
static void load_palette(MogCombat *m, uint32_t a0)
{
    uint16_t c[32];
    for (int i = 0; i < 32; i++)
        c[i] = rw(m, a0 + 2u * (unsigned)i);
    mog_wait_vbls(m, 1);
    if (m->palette)
        m->palette(m->out.user, c);
    uint32_t a1 = rl(m, MOG_v_PalCurrent);
    for (int i = 0; i < 32; i++)
        ww(m, a1 + 2u * (unsigned)i, c[i]);
}

/* LAB_041F : 40000 octets */
static void copy_40000(MogCombat *m, uint32_t src, uint32_t dst)
{
    for (uint32_t i = 0; i < 40000; i++)
        wb(m, dst + i, rb(m, src + i));
}

/* LAB_02CE : objets effacés */
static void clear_objects(MogCombat *m)
{
    uint32_t a0 = rl(m, MOG_v_Objects);
    for (uint32_t i = 0; i < 0xA50; i++)
        wb(m, a0 + i, 0);
}

/* LAB_04C5 : couleurs d0.. de la palette a0 selon le chevalier du tour */
static void knight_colours(MogCombat *m, uint32_t a0, uint32_t d0)
{
    static const uint16_t c[4][5] = {
        { 0x05D, 0x03B, 0x028, 0x016, 0x003 },          /* 0 */
        { 0xFA0, 0xC60, 0xB40, 0x930, 0x710 },          /* 1 */
        { 0x0C5, 0x0A3, 0x082, 0x061, 0x040 },          /* 2 */
        { 0xE00, 0xB00, 0x900, 0x600, 0x300 },          /* 3 */
    };
    uint32_t k = rl(m, CUR + 54);
    if (k > 3)
        return;
    for (uint32_t i = 0; i < 5; i++)
        ww(m, a0 + d0 + 2 * i, c[k][i]);
}

/* LAB_04AC : contrôleur du croupier : feu sur une mise (zone 1, genre =
 * mise) ou sur « partir » (zone 2) */
int mog_gamble_ctl(MogCombat *m, uint32_t a0, CtlResult *out)
{
    wl(m, MOG_v_CurObj, a0);
    if (rw(m, MOG_LAB_0F5B)) {                          /* LAB_04B1 */
        ww(m, MOG_LAB_0F5B, 2);
        *out = mog_set_script(m, 0);
        return 1;
    }
    if (mog_read_joy(m) & MOG_JOY_FIRE) {               /* LAB_04AE */
        uint32_t z = mog_zone_at(m, rw(m, MOG_v_PointerX), rw(m, MOG_v_PointerY));
        if ((uint16_t)z) {
            if (rl(m, z + 16) == 1) {                   /* LAB_04B0 */
                ww(m, MOG_LAB_0F62, rw(m, z + 20));
                uint32_t k = rl(m, MOG_LAB_0F59);
                uint16_t bet = rw(m, MOG_LAB_0F62);
                if (!((int16_t)bet > (int16_t)rw(m, k + 74))) {
                    ww(m, k + 74, (uint16_t)(rw(m, k + 74) - bet));
                    ww(m, MOG_LAB_0F5B, 1);
                    ww(m, MOG_LAB_0F5D, 3);
                    *out = mog_set_script(m, MOG_LAB_0F55);
                    return 1;
                }
            } else if (rl(m, z + 16) == 2) {
                ww(m, MOG_LAB_04AF, 1);
            }
        }
    } else {
        ww(m, MOG_LAB_0F5B, 0);
    }
    ww(m, MOG_LAB_0F5D, 6);                             /* LAB_04AD */
    *out = mog_set_script(m, MOG_LAB_0F54);
    return 1;
}

/* LAB_04B7 : « gp. » après le nombre */
static void gp(MogCombat *m, uint32_t a2)
{
    static const char s[] = " gp.";
    for (int i = 0; i < 5; i++)
        wb(m, a2 + (uint32_t)i, (uint8_t)s[i]);
}

/* LAB_04B4 : trois dés (0 à 5), triés (LAB_04BC) ; gain selon LAB_0F63 */
static void roll(MogCombat *m)
{
    for (uint32_t i = 0; i < 3; i++) {
        uint32_t d0;
        do
            d0 = mog_random(m) & 7;
        while ((int8_t)d0 > 5);
        wb(m, MOG_LAB_0F5F + i, (uint8_t)d0);
    }
    black_palette(m);                                   /* LAB_03EB */
    mog_pointer_off(m);                                 /* LAB_057B */
    copy_40000(m, rl(m, MOG_LAB_0F5C), rl(m, MOG_v_BgPlanes));   /* LAB_041F */
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    uint32_t cel = rl(m, MOG_LAB_0F5A);
    mog_draw_cel(VM, &m->blt, cel, rb(m, MOG_LAB_0F5F), 0x73, 0x0F);
    mog_draw_cel(VM, &m->blt, cel, rb(m, MOG_LAB_0F60), 0x31, 0x26);
    mog_draw_cel(VM, &m->blt, cel, rb(m, MOG_LAB_0F61), 0x4B, 0x58);
    for (int sw = 1; sw;) {                             /* LAB_04BC */
        sw = 0;
        for (uint32_t i = 0; i < 2; i++) {
            uint8_t a = rb(m, MOG_LAB_0F5F + i), b = rb(m, MOG_LAB_0F5F + i + 1);
            if (b < a) {
                wb(m, MOG_LAB_0F5F + i, b);
                wb(m, MOG_LAB_0F5F + i + 1, a);
                sw = 1;
            }
        }
    }
    uint32_t dice = rl(m, MOG_LAB_0F5F), a1 = MOG_LAB_0F63;
    int win = 0;
    for (int i = 0; i < 12; i++, a1 += 6)
        if (rl(m, a1) == dice) {
            win = 1;
            break;
        }
    uint32_t k = rl(m, MOG_LAB_0F59);
    if (win) {                                          /* LAB_04B8 */
        uint32_t d0 = (uint32_t)rw(m, MOG_LAB_0F62) * rb(m, a1 + 4);
        ww(m, k + 74, (uint16_t)(rw(m, k + 74) + d0));
        gp(m, mog_number(m, d0, MOG_s_GoldPieces2));
        gp(m, mog_number(m, (d0 & 0xFFFF0000u) | rw(m, k + 74), MOG_s_GoldPieces3));
        mog_text_records(m, MOG_LAB_0F43);
    } else {
        gp(m, mog_number(m, rw(m, MOG_LAB_0F62), MOG_s_GoldPieces));
        gp(m, mog_number(m, rw(m, k + 74), MOG_s_GoldPieces3));
        mog_text_records(m, MOG_LAB_0F42);
    }
    mog_show_background(m);                             /* LAB_04B9 : LAB_0418 */
    mog_wait_vbls(m, 1);                                /* LAB_0D77 */
    load_palette(m, MOG_t_PalTownB);
    mog_wait_vbls(m, 20);
    mog_wait_fire(m);                                   /* LAB_00EC */
}

/* LAB_04A6 : le jeu de dés (mises par le croupier, LAB_04AC) */
static void gamble(MogCombat *m)
{
    if (!((int16_t)rw(m, CUR + 74) > 0))
        return;
    ww(m, MOG_v_CreatureLoaded, 0xFFFF);                        /* LAB_04A7 */
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    mog_set_planes(m, rl(m, MOG_b_Piv));
    mog_load_picture(m, MOG_s_TavPiv, rl(m, MOG_LAB_0D92));
    for (uint32_t i = 0; i < 32; i++)                   /* LAB_0422 */
        ww(m, MOG_t_PalTownA + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    wl(m, MOG_LAB_0F5C, rl(m, MOG_t_ChipBuffers + 8));
    mog_set_planes(m, rl(m, MOG_LAB_0F5C));
    mog_load_picture(m, MOG_s_DicePiv, rl(m, MOG_LAB_0D92));
    for (uint32_t i = 0; i < 32; i++)
        ww(m, MOG_t_PalTownB + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    wl(m, MOG_LAB_0F5A, rl(m, MOG_t_ChipBuffers + 8) + 0x9C40);
    mog_load_cel(VM, MOG_s_DiceCel, rl(m, MOG_LAB_0F5A));
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    wl(m, MOG_LAB_0F59, CUR);
    mog_clear_zones(m);                                 /* LAB_044E */
    zone_xy(m, 0x10C, 0x26);
    zone_wh(m, 0x2B, 0x18);
    wl(m, MOG_LAB_0A58 + 8, 0);
    zone_id(m, 1);
    ww(m, MOG_LAB_0A58 + 20, 1);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    mog_add_zone(m);
    static const uint16_t ys[4] = { 0x42, 0x5E, 0x79, 0x93 };
    for (int i = 0; i < 4; i++) {
        ww(m, MOG_LAB_0A58 + 14, ys[i]);
        ww(m, MOG_LAB_0A58 + 20, (uint16_t)(i + 2));
        mog_add_zone(m);
    }
    zone_xy(m, 0x10C, 0xB5);
    zone_wh(m, 0x2D, 0x10);
    zone_id(m, 2);
    ww(m, MOG_LAB_0A58 + 22, 0x4A);
    mog_add_zone(m);
    mog_fade_black(m);                                  /* LAB_03F0 */
    ww(m, MOG_LAB_04AF, 0);
    knight_colours(m, MOG_t_PalTownA, 0x0C);              /* LAB_04C5 */
    mog_voice(m, 0, 0x7D);                              /* SECSTRT_16 */
    mog_voice(m, 1, 0x7E);                              /* LAB_0A9B */
    mog_voice(m, 2, 0x7F);                              /* LAB_0A9C */
    for (;;) {                                          /* LAB_04A8 */
        black_palette(m);                               /* LAB_03EB */
        ww(m, MOG_v_PointerX, 0x118);
        mog_pointer_on(m);
        if (!((int16_t)rw(m, rl(m, MOG_LAB_0F59) + 74) > 0))
            break;
        clear_objects(m);                               /* LAB_02CE */
        mog_reset_entities(m);                          /* LAB_0305 */
        copy_40000(m, rl(m, MOG_b_Piv), rl(m, MOG_v_BgPlanes));
        mog_show_background(m);                         /* LAB_0418 */
        mog_swap_screens(m);                            /* LAB_0416 */
        mog_wait_vbls(m, 1);                            /* LAB_0D77 */
        load_palette(m, MOG_t_PalTownA);
        ww(m, MOG_LAB_0F5B, 0);
        ix_spawn(&m->eng, MOG_LAB_0F54, MOG_LAB_0F5A, 0xA0, 0, 0x64, 1, 68);  /* LAB_04AB */
        ww(m, MOG_LAB_0F5D, 5);
        int rolled = 0;
        for (;;) {                                      /* LAB_04A9 */
            if (m->frame_start)         /* point de rendez-vous (pas dans mog) */
                m->frame_start(m->out.user);
            mog_run_controllers(m);
            ix_run_entities(&m->eng);
            mog_swap_screens(m);
            mog_restore_areas(m);
            wl(m, MOG_LAB_0F5E, 0);
            uint32_t g = (uint32_t)(int32_t)(int16_t)rw(m, rl(m, MOG_LAB_0F59) + 74);
            mog_number(m, g, MOG_LAB_0F5E);
            mog_text(m, MOG_LAB_0F5E, 0x11A, 0x0E, 2);
            if (rw(m, MOG_LAB_0F5B) == 2) {
                rolled = 1;
                break;
            }
            mog_wait_vbls(m, rw(m, MOG_LAB_0F5D));
            if (rw(m, MOG_LAB_04AF))
                break;
        }
        if (!rolled)
            break;
        roll(m);                                        /* LAB_04B2 : LAB_04B4 */
        if ((int16_t)rw(m, rl(m, MOG_LAB_0F59) + 74) < 0)
            ww(m, MOG_LAB_04AF, 1);
    }
    mog_pointer_off(m);                                 /* LAB_04AA */
    mog_fade_out(m);                                    /* LAB_03F1 */
    wl(m, MOG_v_CurObj, rl(m, MOG_LAB_0F59));
}

/* ------------------------------------------------------------------ */
/* Villes                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0130 : image de la ville `name` dans LAB_0704 (= LAB_05C1),
 * palette LAB_05B7, fond LAB_05C0 */
static void town_picture(MogCombat *m, uint32_t name)
{
    mog_load_picture(m, name, rl(m, MOG_b_Piv));     /* LAB_0C27 */
    for (uint32_t i = 0; i < 32; i++)                   /* LAB_0422 */
        ww(m, MOG_LAB_05B7 + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    mog_fade_black(m);                                  /* LAB_03F0 */
    mog_copy_screen(m, rl(m, MOG_v_TownPlanes), rl(m, MOG_v_BgPlanes));  /* LAB_0419 */
    mog_show_background(m);                             /* LAB_0418 */
}

/* LAB_012E / LAB_012F : écran d'attente puis image de la ville */
static void town_enter(MogCombat *m, uint32_t text, uint32_t name)
{
    /* LAB_0100 (disquette 3) : sans objet */
    wl(m, MOG_t_TownWaitText, text);
    mog_message_screen(m, MOG_t_TownWaitMessage, 0);             /* LAB_0136 */
    wl(m, MOG_v_TownPlanes, rl(m, MOG_v_SheetPlanes));
    mog_set_planes(m, rl(m, MOG_v_TownPlanes));
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
    ww(m, MOG_v_MovesUsed, rw(m, MOG_v_MovesMax));
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
        town_enter(m, MOG_s_Waterdeep, MOG_s_WaterDeepPiv);      /* LAB_012F */
    else
        town_enter(m, MOG_s_Highwood, MOG_s_HighWoodPiv);      /* LAB_012E */
    for (;;) {
        ww(m, MOG_v_PointerX, kind ? 0x1E : 0x122);       /* LAB_008B / LAB_0094 */
        ww(m, MOG_v_PointerY, 0x64);
        mog_pointer_on(m);                              /* LAB_0575 */
        town_zones(m, kind);
        mog_copy_screen(m, rl(m, MOG_v_TownPlanes), rl(m, MOG_v_BgPlanes));
        mog_show_background(m);                         /* LAB_0418 */
        mog_fade_to(m, MOG_LAB_05B7);                   /* LAB_03F2 */
        mog_clear_keys(m);                              /* LAB_0B82 */
        uint32_t id = 0, screen = 0;
        for (;;) {                                      /* LAB_008C / LAB_0095 */
            busy_tick(m);
            uint32_t z = mog_zone_at(m, rw(m, MOG_v_PointerX), rw(m, MOG_v_PointerY));
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
            gamble(m);                                  /* LAB_04A6 */
            mog_fade_black(m);
            break;
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
/* Repaire du Démon, temple de la Pierre de lune                       */
/* ------------------------------------------------------------------ */

/* LAB_0DCA : touches, chevalier du tour, un des 4 symboles (22 de
 * l'inventaire) gagné au hasard ; renvoie D0 (0 à 3) */
static int demon_reward(MogCombat *m)
{
    mog_clear_keys(m);                                  /* LAB_0B82 */
    mog_select_knight(m);                               /* LAB_0DBD */
    uint32_t d0 = mog_random(m) & 3;                    /* LAB_04A1 */
    uint32_t a0 = rl(m, CUR + 96);
    wb(m, a0 + 22, (uint8_t)(rb(m, a0 + 22) | (1u << d0)));
    return (int)d0;
}

/* LAB_009D : repaire du Démon (objet 20 = $0F requis) */
static int demon_lair(MogCombat *m)
{
    if (rb(m, rl(m, CUR + 96) + 20) != 0x0F) {
        mog_message_screen(m, MOG_t_DemonKeyMessage, 0);         /* LAB_0136 */
        mog_wait_fire(m);                               /* LAB_00EC */
        mog_back_to_map(m);                             /* LAB_00B3 : SECSTRT_36 */
        return MOG_MAP_ENTER;
    }
    mog_select_knight(m);                               /* LAB_009E */
    mog_encounter_init(m, MOG_LAB_01A0);
    mog_combat_run(m);
    wl(m, MOG_v_MapKeysOff, 0);
    if (rb(m, MOG_v_KnightsDown) & 1) {                      /* vaincu */
        mog_select_knight(m);
        uint32_t d1 = mog_pick_stat(m);                 /* LAB_0469 */
        uint32_t a0 = CUR;
        wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) - 2));
        if (rb(m, a0 + d1) != 1)
            wb(m, a0 + d1, (uint8_t)(rb(m, a0 + d1) - 1));
        return inventory_leave(m);                      /* LAB_00B1 */
    }
    uint32_t a0 = CUR;                                  /* LAB_00A0 */
    ww(m, a0 + 78, (uint16_t)(rw(m, a0 + 78) + 3));
    wb(m, rl(m, a0 + 96) + 20, 0);
    mog_message_screen(m, MOG_t_DemonLairMessage, 1);             /* LAB_0137 */
    mog_wait_fire(m);
    mog_wait_vbls(m, 10);
    return demon_reward(m) ? MOG_MAP_ENTER : 0;         /* JMP LAB_0DCA */
}

/* LAB_04CA : 4 couleurs de la palette a0 (+d0) selon le chevalier */
static void knight_colours4(MogCombat *m, uint32_t a0, uint32_t d0)
{
    static const uint16_t c[4][4] = {
        { 0x05D, 0x028, 0x016, 0x003 },
        { 0xFA0, 0xB40, 0x930, 0x710 },
        { 0x0C5, 0x082, 0x061, 0x040 },
        { 0xE00, 0x900, 0x600, 0x300 },
    };
    uint32_t k = rl(m, CUR + 54);
    if (k > 3)
        return;
    for (uint32_t i = 0; i < 4; i++)
        ww(m, a0 + d0 + 2 * i, c[k][i]);
}

int mog_sacrifice_ctl(MogCombat *m, uint32_t a0, CtlResult *out)
{
    wl(m, MOG_v_CurObj, a0);
    ww(m, MOG_LAB_0F58, 1);
    *out = mog_set_script(m, 0);
    return 1;
}

/* LAB_04BF : le sacrifice au temple (scène animée) */
static void sacrifice(MogCombat *m)
{
    clear_objects(m);                                   /* LAB_02CE */
    mog_reset_entities(m);                              /* LAB_0305 */
    /* LAB_0100 (disquette 3) : sans objet */
    mog_message_screen(m, MOG_SECSTRT_42, 1);           /* LAB_0137 */
    wb(m, MOG_v_CreatureLoaded, 0xFF);
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    mog_load_picture(m, MOG_s_Hen1P, rl(m, MOG_LAB_0D92));
    for (uint32_t i = 0; i < 32; i++)                   /* LAB_0422 */
        ww(m, MOG_t_PalTownA + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    wl(m, MOG_LAB_0F5A, rl(m, MOG_t_ChipBuffers + 8));
    mog_load_cel(VM, MOG_s_Hen1C, rl(m, MOG_LAB_0F5A));
    mog_load_sounds(m, MOG_s_HeA, MOG_b_SoundsCreature, 0x2F78);    /* LAB_0AB0 */
    wl(m, MOG_t_Controllers + 40, MOG_LAB_04C4);
    ix_spawn(&m->eng, MOG_LAB_0F57, MOG_LAB_0F5A, 0xA0, 0, 0x64, 1, 40);
    ix_spawn(&m->eng, MOG_LAB_0F56, MOG_LAB_0F5A, 0xA0, 0, 0x64, 1, 40);
    mog_fade_out(m);                                    /* LAB_03F1 */
    mog_show_background(m);                             /* LAB_0418 */
    mog_swap_screens(m);                                /* LAB_0416 */
    mog_run_controllers(m);
    ix_run_entities(&m->eng);
    mog_swap_screens(m);
    mog_restore_areas(m);                               /* LAB_039E */
    knight_colours4(m, MOG_t_PalTownA, 0x10);             /* LAB_04CA */
    mog_voice(m, 0, 0x9E);                              /* SECSTRT_16 */
    mog_voice(m, 1, 0x9F);                              /* LAB_0A9B */
    mog_voice(m, 2, 0xA0);                              /* LAB_0A9C */
    wl(m, MOG_v_PalFadeTarget, MOG_t_PalTownA);                /* LAB_0E55 : fondu, 4 */
    ww(m, MOG_v_PalFadeDelay, 4);
    ww(m, MOG_v_PalFadeCount, 4);
    ww(m, MOG_LAB_0F58, 0);
    ww(m, MOG_LAB_0F5D, 6);
    for (;;) {                                          /* LAB_04C0 */
        mog_run_controllers(m);
        if (rw(m, MOG_LAB_0F58))
            break;
        ix_run_entities(&m->eng);
        mog_swap_screens(m);
        mog_restore_areas(m);
        mog_wait_vbls(m, 6);
    }
    mog_fade_out(m);                                    /* LAB_04C1 : LAB_03F1 */
}

/* LAB_00A1 : temple de la Pierre de lune : au bon moment (v_Combatants
 * +18) avec le bon symbole, c'est la fin ; sinon un sacrifice possible
 * (écran 3, LAB_04BF). */
static int temple(MogCombat *m)
{
    uint16_t d0 = rw(m, MOG_v_Combatants + 18);
    uint32_t a1 = rl(m, MOG_v_Combatants);
    uint8_t d1 = rb(m, rl(m, a1 + 96) + 22);
    int win = ((d1 & 4) && d0 == 0x2E) || ((d1 & 8) && d0 == 0x2E)
           || ((d1 & 2) && d0 == 0x2D) || ((d1 & 1) && d0 == 0x31);
    if (win) {                                          /* LAB_00A8 */
        uint16_t d7 = 0;
        if (d0 == 0x2E) d7 |= 1;
        if (d0 == 0x2D) d7 |= 4;
        if (d0 == 0x31) d7 |= 2;
        uint32_t k = rl(m, a1 + 54);
        if (k == 3) d7 |= 8;
        if (k == 0) d7 |= 0x10;
        if (k == 1) d7 |= 0x20;
        if (k == 2) d7 |= 0x40;
        mog_message_screen(m, MOG_t_EndWonMessage, 0);         /* LAB_0136 */
        mog_wait_vbls(m, 20);
        mog_wait_fire(m);
        /* LAB_0100 (disquette 1) : sans objet */
        ww(m, 0x3E0, (uint16_t)(d7 | 0x80));            /* EXT_000e */
        return MOG_MAP_WIN;                             /* SECSTRT_5 « program » */
    }
    mog_message_screen(m, MOG_t_EndLostMessage, 1);             /* LAB_00A5 : LAB_0137 */
    mog_wait_fire(m);
    mog_fade_black(m);
    ww(m, MOG_LAB_053B, 0xFFFF);
    mog_screen_run(m, 3);
    if (rw(m, MOG_LAB_053B) != 0xFFFF) {
        sacrifice(m);                                   /* LAB_04BF */
        uint32_t a0 = rl(m, MOG_v_Combatants);
        if (rb(m, a0 + 73) != 5)
            wb(m, a0 + 73, (uint8_t)(rb(m, a0 + 73) + 1));
        mog_knight_hp(m, a0);                           /* LAB_0013 */
        ww(m, a0 + 80, rw(m, a0 + 84));
        wb(m, a0 + 130, 0);
    }
    mog_fade_black(m);                                  /* LAB_00A7 */
    return inventory_leave(m);                          /* LAB_00B1 */
}

/* ------------------------------------------------------------------ */
/* La sorcière (genre $1E : LAB_0456)                                  */
/* ------------------------------------------------------------------ */

/* LAB_049E : lignes de texte (octet : nombre, puis chaînes) tous les 10 */
static void text_lines(MogCombat *m, uint32_t a0, uint16_t x, uint16_t y, uint16_t flags)
{
    ww(m, MOG_v_TownTextX, x);
    ww(m, MOG_v_TownTextY, y);
    ww(m, MOG_v_TownTextFlags, flags);
    uint16_t d7 = (uint16_t)(rb(m, a0++) - 1);
    do {
        mog_text(m, a0, rw(m, MOG_v_TownTextX), rw(m, MOG_v_TownTextY), rw(m, MOG_v_TownTextFlags));
        while (rb(m, a0++))
            ;
        ww(m, MOG_v_TownTextY, (uint16_t)(rw(m, MOG_v_TownTextY) + 10));
    } while (d7-- != 0);
}

/* LAB_0131 : écran d'attente, décors LAB_078A / LAB_0789, CEL LAB_0788 */
static void witch_load(MogCombat *m)
{
    /* LAB_0100 (disquette 3) : sans objet */
    mog_loading_screen(m);                              /* LAB_0134 */
    mog_set_planes(m, rl(m, MOG_b_Piv));
    mog_load_picture(m, MOG_s_Wi2P, rl(m, MOG_LAB_0D92));
    for (uint32_t i = 0; i < 32; i++)
        ww(m, MOG_t_PalTownA + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    mog_set_planes(m, rl(m, MOG_v_SheetPlanes));
    mog_load_picture(m, MOG_s_Wi1P, rl(m, MOG_LAB_0D92));
    for (uint32_t i = 0; i < 32; i++)
        ww(m, MOG_t_PalTownB + 2 * i, rw(m, MOG_LAB_0D2B + 2 * i));
    if (rb(m, MOG_v_CreatureLoaded) != 0x3C) {
        wb(m, MOG_v_CreatureLoaded, 0x3C);
        wl(m, MOG_v_LoadPtr, rl(m, MOG_t_ChipBuffers + 8));
        for (uint32_t i = 0; i < 5; i++)
            wl(m, MOG_t_BankEnemy + 4 * i, rl(m, MOG_v_LoadPtr));
        mog_load_cel(VM, MOG_s_Wi1C, rl(m, MOG_v_LoadPtr));
    }
    mog_load_sounds(m, MOG_s_WzA, MOG_b_SoundsWizard, 0xD6D8);    /* LAB_0132 : LAB_0AB4 */
}

/* Scène animée jusqu'au feu (LAB_0457, LAB_0458, LAB_045D) */
static void scene_until_fire(MogCombat *m, unsigned vbls)
{
    do {
        if (m->frame_start)             /* point de rendez-vous (pas dans mog) */
            m->frame_start(m->out.user);
        ix_run_entities(&m->eng);
        mog_swap_screens(m);                            /* LAB_0416 */
        mog_restore_areas(m);                           /* LAB_039E */
        mog_wait_vbls(m, vbls);
    } while (!(mog_read_joy(m) & MOG_JOY_FIRE));        /* LAB_00EE */
}

/* LAB_0456 : la sorcière : présentation, événement (LAB_045E), réponse */
static void witch(MogCombat *m)
{
    witch_load(m);                                      /* LAB_0131 */
    mog_fade_black(m);
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
    clear_objects(m);                                   /* LAB_02CE */
    mog_reset_entities(m);                              /* LAB_0305 */
    copy_40000(m, rl(m, MOG_b_Piv), rl(m, MOG_v_BgPlanes));   /* LAB_041F */
    knight_colours(m, MOG_t_PalTownA, 0x0C);              /* LAB_04C5 */
    ix_spawn(&m->eng, MOG_x_WizardStand, MOG_t_BankEnemy, 0xA0, 0, 0x64, 1, 40);
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    text_lines(m, MOG_s_AsYouRingTheBellAtThe, 0x96, 5, 0);            /* LAB_049E */
    mog_show_background(m);                             /* LAB_0418 */
    mog_swap_screens(m);
    ix_run_entities(&m->eng);
    mog_swap_screens(m);
    voices(m, 0x80);                                    /* LAB_0F8C */
    wl(m, MOG_v_PalFadeTarget, MOG_t_PalTownA);                /* LAB_0E55 */
    ww(m, MOG_v_PalFadeDelay, 2);
    ww(m, MOG_v_PalFadeCount, 2);
    scene_until_fire(m, 5);                             /* LAB_0457 */

    mog_fade_black(m);
    black_palette(m);                                   /* LAB_03EB */
    mog_reset_entities(m);
    clear_objects(m);
    mog_random_event(m);                                /* LAB_045E */
    copy_40000(m, rl(m, MOG_v_SheetPlanes), rl(m, MOG_v_BgPlanes));
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    text_lines(m, rl(m, MOG_v_EventMessage), 0x32, 0x82, 1);
    mog_show_background(m);
    ix_spawn(&m->eng, MOG_x_WizardA, MOG_t_BankEnemy, 0xA0, 0, 0x64, 1, 40);
    ix_spawn(&m->eng, MOG_x_WizardB, MOG_t_BankEnemy, 0xA0, 0, 0x64, 1, 40);
    mog_swap_screens(m);
    wl(m, MOG_v_PalFadeTarget, MOG_t_PalTownB);
    ww(m, MOG_v_PalFadeDelay, 2);
    ww(m, MOG_v_PalFadeCount, 2);
    scene_until_fire(m, 6);                             /* LAB_0458 */

    clear_objects(m);
    mog_reset_entities(m);
    copy_40000(m, rl(m, MOG_b_Piv), rl(m, MOG_v_BgPlanes));
    black_palette(m);
    mog_set_planes(m, rl(m, MOG_v_BgPlanes));
    ix_spawn(&m->eng, MOG_x_WizardStand, MOG_t_BankEnemy, 0xA0, 0, 0x64, 1, 40);
    uint32_t t = MOG_t_WizardNoneText;
    switch (rw(m, MOG_v_EventKind)) {
    case 2: t = MOG_t_WizardGoldText; break;
    case 3: t = MOG_t_WizardStatText; break;
    case 1: t = MOG_t_WizardItemText; break;
    case 4: t = MOG_t_WizardCurseText; break;
    }
    text_lines(m, t, 0x96, 5, 0);
    mog_show_background(m);
    mog_swap_screens(m);
    load_palette(m, MOG_t_PalTownA);                      /* LAB_0D8A, LAB_03EE */
    scene_until_fire(m, 4);                             /* LAB_045D */
    wb(m, CUR + 83, 0x46);
    mog_fade_out(m);                                    /* LAB_03F1 */
}

/* ------------------------------------------------------------------ */
/* LAB_007B                                                            */
/* ------------------------------------------------------------------ */

int mog_town(MogCombat *m, uint32_t d0)
{
    mog_map_colours_off(m);                             /* LAB_0DC8 */
    ww(m, MOG_v_CelPlanesMax, 4);                             /* SECSTRT_28, D7 = 5 */
    ww(m, MOG_v_ScreenStrideSet, 0);
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_t_FontBank));
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
        return temple(m);                               /* LAB_00A1 */
    case 0x1C:
        return demon_lair(m);                           /* LAB_009D */
    case 0x1E:                                          /* LAB_007C */
        witch(m);                                       /* LAB_0456 */
        return inventory_leave(m);                      /* écran 9, LAB_00B2 */
    case 0x21:
        /* inatteignable : LAB_0E45 mène $21 au duel avant LAB_007B */
        todo(m, "LAB_007B $21 (duel)");
        return MOG_MAP_UNPORTED;
    }
    return (uint16_t)d0 ? MOG_MAP_ENTER : 0;            /* D0 inchangé */
}
