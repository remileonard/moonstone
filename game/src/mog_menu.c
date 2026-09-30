/*
 * mog_menu.c — menu du début de partie de mog et choix des chevaliers,
 * traduits de amiga_asm/mog.asm.
 *
 * Prot_CopylockCheck : fondu, LAB_012D (écran du menu, CEL LAB_07AD),
 * protection Copylock (chiffrée : sur une disquette d'origine elle laisse
 * LAB_0714 = LAB_029F, le contrôleur du chevalier humain), puis le menu
 * (LAB_00B5) : ligne LAB_06DC 0 « Players » (1 à 4, LAB_05C5), 1 « Gore »
 * (LAB_06DA), 2 « Practice » (duel d'entraînement), 3 « Select Knight ».
 * LAB_00D3 : chaque joueur choisit un des quatre chevaliers et tape son
 * nom (LAB_00C9).
 */
#include "mog_private.h"
#include "mog_menu.h"
#include "mog_map.h"
#include "mog_screens.h"
#include "mog_text.h"
#include "mog_vbl.h"
#include "mog_boot.h"
#include "ix_mog_syms.h"

#define VM (m->eng.vm)

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }

/* Tour d'une boucle d'attente de l'original : une VBL, puis l'hôte */
static void busy_tick(MogCombat *m)
{
    mog_wait_vbls(m, 1);
    if (m->frame_start)
        m->frame_start(m->out.user);
}

/* LAB_03A7 : zones à restaurer LAB_064D vidées */
static void clear_scraps(MogCombat *m)
{
    for (uint32_t i = 0; i < 0x2D0; i++)
        wb(m, MOG_LAB_064D + i, 0xFF);
}

/* LAB_03EB : palette courante noire (33 mots) */
static void black(MogCombat *m)
{
    uint32_t a2 = rl(m, MOG_LAB_0E93);
    for (unsigned i = 0; i < 33; i++)
        ww(m, a2 + 2 * i, 0);
}

/* LAB_0129 : fond du menu (PIV rl(LAB_05B9 + 56)) dans SECSTRT_35 */
static void menu_background(MogCombat *m)
{
    black(m);
    mog_clear_screen(m, rl(m, MOG_SECSTRT_35));
    mog_set_planes(m, rl(m, MOG_SECSTRT_35));
    uint32_t s = rl(m, MOG_LAB_05B9 + 56), d = rl(m, MOG_LAB_0D92);
    for (uint32_t i = 0; i < 0x25F7; i++)
        wb(m, d + i, rb(m, s + i));
    mog_piv_decode(m, rl(m, MOG_LAB_0D92));
}

/* LAB_00C6 : nombre de joueurs + d0, de 1 à 4 ; texte « Players » */
static void add_players(MogCombat *m, int16_t d0)
{
    uint16_t n = (uint16_t)(rw(m, MOG_LAB_05C5) + d0);
    ww(m, MOG_LAB_05C5, n);
    if (!n)
        ww(m, MOG_LAB_05C5, 1);
    else if ((int16_t)n > 4)
        ww(m, MOG_LAB_05C5, 4);
    ww(m, MOG_LAB_06DE, rw(m, MOG_SECSTRT_4 + 2u * (uint16_t)(rw(m, MOG_LAB_05C5) - 1)));
}

/* LAB_00C4 : menu dessiné (curseur à la ligne LAB_06DC), montré */
static void menu_draw(MogCombat *m)
{
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    mog_copy_screen(m, rl(m, MOG_LAB_05C0), rl(m, MOG_LAB_0D92));
    clear_scraps(m);
    ww(m, MOG_LAB_05D9, rw(m, MOG_LAB_06DD + 2u * rw(m, MOG_LAB_06DC)));
    ww(m, MOG_LAB_0D05, 1);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_v_Combatants + 10), 0x49, 5, 10);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_05C4), 0, rw(m, MOG_LAB_05D8), rw(m, MOG_LAB_05D9));
    ww(m, MOG_LAB_0D05, 0);
    mog_number(m, rw(m, MOG_LAB_05C5), MOG_LAB_06B9);   /* LAB_0442 */
    wl(m, MOG_LAB_06B3, rl(m, MOG_LAB_06DA) ? MOG_LAB_06BB : MOG_LAB_06BA);
    mog_text_records(m, MOG_LAB_06AE);                  /* LAB_0432 */
    mog_swap_screens(m);                                /* LAB_0416 */
    mog_wait_vbls(m, 10);
}

/* LAB_00C2 : ligne « Gore » : bascule LAB_06DA */
static int toggle_gore(MogCombat *m)
{
    if (rw(m, MOG_LAB_06DC) != 1)
        return 0;
    wl(m, MOG_LAB_06DA, rl(m, MOG_LAB_06DA) ^ 1);
    return 1;
}

/* LAB_00BA : haut / bas (lignes), gauche / droite (joueurs, gore) */
static int menu_move(MogCombat *m)
{
    uint16_t d1 = rw(m, MOG_LAB_0630);
    if (!d1)
        return 0;
    if (d1 & 8) {
        ww(m, MOG_LAB_06DC, (uint16_t)(rw(m, MOG_LAB_06DC) - 1));
        if ((int16_t)rw(m, MOG_LAB_06DC) < 0) {
            ww(m, MOG_LAB_06DC, 0);
            ww(m, MOG_LAB_06DB, 1);
        }
        return 1;
    }
    if (d1 & 4) {
        ww(m, MOG_LAB_06DC, (uint16_t)(rw(m, MOG_LAB_06DC) + 1));
        if ((int16_t)rw(m, MOG_LAB_06DC) > 3) {
            ww(m, MOG_LAB_06DC, 3);
            ww(m, MOG_LAB_06DB, 1);
        }
        return 1;
    }
    if (d1 & 3) {
        if (rw(m, MOG_LAB_06DC))
            return toggle_gore(m);
        add_players(m, d1 & 2 ? -1 : 1);
        return 1;
    }
    return 0;
}

int mog_menu(MogCombat *m)
{
    mog_fade_out(m);                                    /* LAB_03F1 */
    /* LAB_012D : LAB_0100 (disquette 2) sans objet */
    mog_load_cel(VM, MOG_LAB_07AD, rl(m, MOG_LAB_05C1));
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3 + 16));
    menu_background(m);
    mog_copy_screen(m, rl(m, MOG_SECSTRT_35), rl(m, MOG_LAB_0D92));
    mog_copy_screen(m, rl(m, MOG_SECSTRT_35), rl(m, MOG_LAB_05C0));
    /* Prot_Copylock : disquette d'origine, LAB_0714 = LAB_029F inchangé */
    clear_scraps(m);
    ww(m, MOG_LAB_05D8, 0x32);
    ww(m, MOG_LAB_05D9, 0x30);
    ww(m, MOG_LAB_05DA, 0x30);
    wl(m, MOG_v_Combatants + 10, rl(m, MOG_LAB_05E3 + 16));
    add_players(m, 0);
    mog_swap_screens(m);
    wl(m, MOG_LAB_05C4, rl(m, MOG_LAB_05C1));
    menu_draw(m);
    mog_fade_to(m, MOG_LAB_0D2B);
    wl(m, MOG_LAB_0714, MOG_LAB_029F);
    for (;;) {                                          /* LAB_00B5 */
        busy_tick(m);
        uint16_t d1 = mog_read_joy(m);
        if (!d1)
            continue;
        if (d1 & MOG_JOY_FIRE) {
            uint16_t row = rw(m, MOG_LAB_06DC);
            if (row == 3) {                             /* LAB_00B9 */
                ww(m, MOG_v_Combatants + 14, rw(m, MOG_LAB_05C5));
                mog_fade_black(m);
                return 3;
            }
            if (row == 2) {                             /* LAB_00B8 */
                mog_fade_black(m);
                return 2;
            }
            if (toggle_gore(m)) {
                menu_draw(m);
                continue;
            }
        }
        if (menu_move(m))
            menu_draw(m);
    }
}

/* ------------------------------------------------ choix des chevaliers */

/* LAB_00E0 : chevaliers libres (LAB_06F9), curseur LAB_0703, nom en cours */
static void knights_draw(MogCombat *m)
{
    mog_clear_screen(m, rl(m, MOG_LAB_0D92));
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    mog_text_records(m, MOG_LAB_06E7);
    for (uint16_t d7 = 0; d7 < 4; d7++)
        if (rb(m, MOG_LAB_06F9) & (1u << d7))
            mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_05C4), (uint16_t)(d7 + 2),
                         rw(m, MOG_LAB_0702 + 2u * d7), 0x50);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_LAB_05C4), 1,
                 rw(m, MOG_LAB_0702 + 2u * rw(m, MOG_LAB_0703)), 0x50);
    if (rw(m, MOG_LAB_05D7))
        mog_text(m, rl(m, MOG_LAB_06B4), 0x32, 0x32, 0);   /* LAB_0431 */
    mog_swap_screens(m);
    if (!rw(m, MOG_LAB_05D7))
        mog_wait_vbls(m, 6);
}

/* LAB_00C9 : nom du chevalier tapé au clavier (13 lettres au plus,
 * curseur LAB_05D4 ; Retour ou feu : fin, retour arrière : effacer) */
static void enter_name(MogCombat *m)
{
    do                                                  /* feu relâché */
        busy_tick(m);
    while (mog_read_joy(m) & MOG_JOY_FIRE);
    ww(m, MOG_LAB_05D7, 1);
    mog_clear_keys(m);                                  /* LAB_0B82 */
    clear_scraps(m);
    wl(m, MOG_LAB_05D5, MOG_LAB_00E0);
    wb(m, MOG_LAB_05D4, 0x5C);
    uint32_t a0 = rl(m, MOG_LAB_06B4);
    uint16_t d0 = 0;
    while (rb(m, a0 + d0) != 0x20 && rb(m, a0 + d0))
        d0++;
    ww(m, MOG_LAB_05D6, d0);
    for (;;) {
        /* LAB_00CE : curseur, LAB_05D5 (LAB_00E0), touche oubliée */
        wb(m, rl(m, MOG_LAB_06B4) + rw(m, MOG_LAB_05D6), rb(m, MOG_LAB_05D4));
        knights_draw(m);
        ww(m, MOG_SECSTRT_21, 0);
        uint16_t k;
        for (;;) {                                      /* LAB_00CC */
            busy_tick(m);
            if (mog_read_joy(m) & MOG_JOY_FIRE)
                goto done;
            k = rw(m, MOG_SECSTRT_21);
            if (k)
                break;
        }
        if (k == 0x1C)
            goto done;
        if (k == 0x0E) {                                /* LAB_00CF */
            uint32_t n = rl(m, MOG_LAB_06B4);
            wb(m, n + rw(m, MOG_LAB_05D6), 0x20);
            ww(m, MOG_LAB_05D6, (uint16_t)(rw(m, MOG_LAB_05D6) - 1));
            if ((int16_t)rw(m, MOG_LAB_05D6) < 0)
                ww(m, MOG_LAB_05D6, 0);
            wb(m, n + rw(m, MOG_LAB_05D6), 0x20);
            continue;
        }
        uint8_t c = rb(m, MOG_LAB_0D99 + k);            /* LAB_0D8D */
        if (!c)
            continue;
        if ((int16_t)rw(m, MOG_LAB_05D6) >= 13) {       /* nom plein : éclair rouge */
            mog_wait_vbls(m, 1);
            m->color00 = 0x10C00;
            mog_wait_vbls(m, 2);
            m->color00 = 0x10000;
            continue;
        }
        wb(m, rl(m, MOG_LAB_06B4) + rw(m, MOG_LAB_05D6), c);
        ww(m, MOG_LAB_05D6, (uint16_t)(rw(m, MOG_LAB_05D6) + 1));
    }
done:                                                   /* LAB_00D2 */
    wb(m, rl(m, MOG_LAB_06B4) + rw(m, MOG_LAB_05D6), 0);
    ww(m, MOG_LAB_05D7, 0);
}

/* LAB_00E5 : chevalier d0 donné au joueur a1 (nom, contrôle au joystick) */
static void take_knight(MogCombat *m, uint16_t d0, uint32_t a1)
{
    static const uint32_t names[4] = { MOG_LAB_06B6, MOG_LAB_06B5, MOG_LAB_06B7, MOG_LAB_06B8 };
    mog_wait_vbls(m, 4);
    if (d0 > 3)
        return;
    wl(m, MOG_LAB_06B4, names[d0]);
    enter_name(m);
    wb(m, MOG_LAB_06F9, (uint8_t)(rb(m, MOG_LAB_06F9) & ~(1u << d0)));
    wl(m, a1 + 54, d0);
    wb(m, a1 + 11, 2);
}

void mog_choose_knights(MogCombat *m)
{
    mog_clear_screen(m, rl(m, MOG_LAB_05C0));
    mog_show_background(m);                             /* LAB_0418 */
    ww(m, MOG_LAB_06F8, rw(m, MOG_LAB_05C5));
    wb(m, MOG_LAB_06F9, 0x0F);
    wl(m, MOG_LAB_06F7, MOG_LAB_0613);
    ww(m, MOG_LAB_0703, 0);
    wl(m, MOG_LAB_05C4, rl(m, MOG_LAB_05C1));
    mog_set_planes(m, rl(m, MOG_LAB_0D92));
    knights_draw(m);
    mog_fade_to(m, MOG_LAB_06FD);
    wl(m, MOG_LAB_05A5, mog_glow(VM, 0x0F, 0x88, 1, 0));
    for (;;) {                                          /* LAB_00D4 */
        busy_tick(m);
        uint16_t d1 = mog_read_joy(m);
        if (d1 & MOG_JOY_FIRE) {
            take_knight(m, rw(m, MOG_LAB_0703), rl(m, MOG_LAB_06F7));
            ww(m, MOG_LAB_06F8, (uint16_t)(rw(m, MOG_LAB_06F8) - 1));
            if (!rw(m, MOG_LAB_06F8))
                break;
            wl(m, MOG_LAB_06F7, rl(m, MOG_LAB_06F7) + 0x84);
            uint16_t d0 = 0;                            /* LAB_00DD */
            while (!(rb(m, MOG_LAB_06F9) & (1u << (d0 & 7))))
                d0++;
            ww(m, MOG_LAB_0703, d0);
            knights_draw(m);
        } else if (d1 & MOG_JOY_LEFT) {
            int16_t d0 = (int16_t)rw(m, MOG_LAB_0703);
            while (--d0 >= 0)
                if (rb(m, MOG_LAB_06F9) & (1u << d0)) {
                    ww(m, MOG_LAB_0703, (uint16_t)d0);
                    knights_draw(m);
                    break;
                }
        } else if (d1 & MOG_JOY_RIGHT) {
            uint16_t d0 = rw(m, MOG_LAB_0703);
            while (++d0 != 4)
                if (rb(m, MOG_LAB_06F9) & (1u << d0)) {
                    ww(m, MOG_LAB_0703, d0);
                    knights_draw(m);
                    break;
                }
        }
    }
    wl(m, rl(m, MOG_LAB_05A5), 0);                      /* LAB_00DA */
    mog_fade_black(m);
}
