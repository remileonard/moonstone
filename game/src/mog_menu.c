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
#include "ix_mog_names.h"
#include "mog_struct.h"

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
        wb(m, MOG_b_RestoreA + i, 0xFF);
}

/* LAB_03EB : palette courante noire (33 mots) */
static void black(MogCombat *m)
{
    uint32_t a2 = rl(m, MOG_v_PalCurrent);
    for (unsigned i = 0; i < 33; i++)
        ww(m, a2 + 2 * i, 0);
}

/* LAB_0129 : fond du menu (PIV rl(LAB_05B9 + 56)) dans SECSTRT_35 */
static void menu_background(MogCombat *m)
{
    black(m);
    mog_clear_screen(m, rl(m, MOG_v_ShowPlanes));
    mog_set_planes(m, rl(m, MOG_v_ShowPlanes));
    uint32_t s = rl(m, MOG_t_FastBuffers + 56), d = rl(m, MOG_v_DrawPlanes);
    for (uint32_t i = 0; i < 0x25F7; i++)
        wb(m, d + i, rb(m, s + i));
    mog_piv_decode(m, rl(m, MOG_v_DrawPlanes));
}

/* LAB_00C6 : nombre de joueurs + d0, de 1 à 4 ; texte « Players » */
static void add_players(MogCombat *m, int16_t d0)
{
    uint16_t n = (uint16_t)(rw(m, MOG_v_Players) + d0);
    ww(m, MOG_v_Players, n);
    if (!n)
        ww(m, MOG_v_Players, 1);
    else if ((int16_t)n > 4)
        ww(m, MOG_v_Players, 4);
    ww(m, MOG_v_TrainCost, rw(m, MOG_t_TrainCostByPlayers + 2u * (uint16_t)(rw(m, MOG_v_Players) - 1)));
}

/* LAB_00C4 : menu dessiné (curseur à la ligne LAB_06DC), montré */
static void menu_draw(MogCombat *m)
{
    mog_set_planes(m, rl(m, MOG_v_DrawPlanes));
    mog_copy_screen(m, rl(m, MOG_v_BgPlanes), rl(m, MOG_v_DrawPlanes));
    clear_scraps(m);
    ww(m, MOG_v_MenuCursorY, rw(m, MOG_t_MenuLineY + 2u * rw(m, MOG_v_MenuLine)));
    ww(m, MOG_v_BlitByCpu, 1);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_v_Combatants + CMB_FONT), 0x49, 5, 10);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_v_CursorCel), 0, rw(m, MOG_v_MenuCursorX), rw(m, MOG_v_MenuCursorY));
    ww(m, MOG_v_BlitByCpu, 0);
    mog_number(m, rw(m, MOG_v_Players), MOG_s_PlayersCount);   /* LAB_0442 */
    wl(m, MOG_t_MenuGoreText, rl(m, MOG_v_Gore) ? MOG_s_Off : MOG_s_On);
    mog_text_records(m, MOG_t_MenuTexts);                  /* LAB_0432 */
    mog_swap_screens(m);                                /* LAB_0416 */
    mog_wait_vbls(m, 10);
}

/* LAB_00C2 : ligne « Gore » : bascule LAB_06DA */
static int toggle_gore(MogCombat *m)
{
    if (rw(m, MOG_v_MenuLine) != 1)
        return 0;
    wl(m, MOG_v_Gore, rl(m, MOG_v_Gore) ^ 1);
    return 1;
}

/* LAB_00BA : haut / bas (lignes), gauche / droite (joueurs, gore) */
static int menu_move(MogCombat *m)
{
    uint16_t d1 = rw(m, MOG_v_Joy1);
    if (!d1)
        return 0;
    if (d1 & 8) {
        ww(m, MOG_v_MenuLine, (uint16_t)(rw(m, MOG_v_MenuLine) - 1));
        if ((int16_t)rw(m, MOG_v_MenuLine) < 0) {
            ww(m, MOG_v_MenuLine, 0);
            ww(m, MOG_v_MenuRedraw, 1);
        }
        return 1;
    }
    if (d1 & 4) {
        ww(m, MOG_v_MenuLine, (uint16_t)(rw(m, MOG_v_MenuLine) + 1));
        if ((int16_t)rw(m, MOG_v_MenuLine) > 3) {
            ww(m, MOG_v_MenuLine, 3);
            ww(m, MOG_v_MenuRedraw, 1);
        }
        return 1;
    }
    if (d1 & 3) {
        if (rw(m, MOG_v_MenuLine))
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
    mog_load_cel(VM, MOG_s_SelCel, rl(m, MOG_v_SheetPlanes));
    wl(m, MOG_v_Combatants + CMB_FONT, rl(m, MOG_t_FontBank + 16));
    menu_background(m);
    mog_copy_screen(m, rl(m, MOG_v_ShowPlanes), rl(m, MOG_v_DrawPlanes));
    mog_copy_screen(m, rl(m, MOG_v_ShowPlanes), rl(m, MOG_v_BgPlanes));
    /* Prot_Copylock : disquette d'origine, LAB_0714 = LAB_029F inchangé */
    clear_scraps(m);
    ww(m, MOG_v_MenuCursorX, 0x32);
    ww(m, MOG_v_MenuCursorY, 0x30);
    ww(m, MOG_v_MenuCursorY0, 0x30);
    wl(m, MOG_v_Combatants + CMB_FONT, rl(m, MOG_t_FontBank + 16));
    add_players(m, 0);
    mog_swap_screens(m);
    wl(m, MOG_v_CursorCel, rl(m, MOG_v_SheetPlanes));
    menu_draw(m);
    mog_fade_to(m, MOG_t_PivPalette);
    wl(m, MOG_v_KnightCtlFn, MOG_Ctl_Leaper);
    for (;;) {                                          /* LAB_00B5 */
        busy_tick(m);
        uint16_t d1 = mog_read_joy(m);
        if (!d1)
            continue;
        if (d1 & MOG_JOY_FIRE) {
            uint16_t row = rw(m, MOG_v_MenuLine);
            if (row == 3) {                             /* LAB_00B9 */
                ww(m, MOG_v_Combatants + CMB_CHOOSERS, rw(m, MOG_v_Players));
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
    mog_clear_screen(m, rl(m, MOG_v_DrawPlanes));
    mog_set_planes(m, rl(m, MOG_v_DrawPlanes));
    mog_text_records(m, MOG_t_SelectKnightText);
    for (uint16_t d7 = 0; d7 < 4; d7++)
        if (rb(m, MOG_v_KnightsFree) & (1u << d7))
            mog_draw_cel(VM, &m->blt, rl(m, MOG_v_CursorCel), (uint16_t)(d7 + 2),
                         rw(m, MOG_t_KnightChoiceX + 2u * d7), 0x50);
    mog_draw_cel(VM, &m->blt, rl(m, MOG_v_CursorCel), 1,
                 rw(m, MOG_t_KnightChoiceX + 2u * rw(m, MOG_v_KnightChoice)), 0x50);
    if (rw(m, MOG_v_NameEditing))
        mog_text(m, rl(m, MOG_v_NameEdited), 0x32, 0x32, 0);   /* LAB_0431 */
    mog_swap_screens(m);
    if (!rw(m, MOG_v_NameEditing))
        mog_wait_vbls(m, 6);
}

/* LAB_00C9 : nom du chevalier tapé au clavier (13 lettres au plus,
 * curseur LAB_05D4 ; Retour ou feu : fin, retour arrière : effacer) */
static void enter_name(MogCombat *m)
{
    do                                                  /* feu relâché */
        busy_tick(m);
    while (mog_read_joy(m) & MOG_JOY_FIRE);
    ww(m, MOG_v_NameEditing, 1);
    mog_clear_keys(m);                                  /* LAB_0B82 */
    clear_scraps(m);
    wl(m, MOG_v_NameCursorAnim, MOG_Menu_CursorAnim);
    wb(m, MOG_v_NameCursorChar, 0x5C);
    uint32_t a0 = rl(m, MOG_v_NameEdited);
    uint16_t d0 = 0;
    while (rb(m, a0 + d0) != 0x20 && rb(m, a0 + d0))
        d0++;
    ww(m, MOG_v_NameLength, d0);
    for (;;) {
        /* LAB_00CE : curseur, LAB_05D5 (LAB_00E0), touche oubliée */
        wb(m, rl(m, MOG_v_NameEdited) + rw(m, MOG_v_NameLength), rb(m, MOG_v_NameCursorChar));
        knights_draw(m);
        ww(m, MOG_v_KeyPressed, 0);
        uint16_t k;
        for (;;) {                                      /* LAB_00CC */
            busy_tick(m);
            if (mog_read_joy(m) & MOG_JOY_FIRE)
                goto done;
            k = rw(m, MOG_v_KeyPressed);
            if (k)
                break;
        }
        if (k == 0x1C)
            goto done;
        if (k == 0x0E) {                                /* LAB_00CF */
            uint32_t n = rl(m, MOG_v_NameEdited);
            wb(m, n + rw(m, MOG_v_NameLength), 0x20);
            ww(m, MOG_v_NameLength, (uint16_t)(rw(m, MOG_v_NameLength) - 1));
            if ((int16_t)rw(m, MOG_v_NameLength) < 0)
                ww(m, MOG_v_NameLength, 0);
            wb(m, n + rw(m, MOG_v_NameLength), 0x20);
            continue;
        }
        uint8_t c = rb(m, MOG_t_KeyChars + k);            /* LAB_0D8D */
        if (!c)
            continue;
        if ((int16_t)rw(m, MOG_v_NameLength) >= 13) {       /* nom plein : éclair rouge */
            mog_wait_vbls(m, 1);
            m->color00 = 0x10C00;
            mog_wait_vbls(m, 2);
            m->color00 = 0x10000;
            continue;
        }
        wb(m, rl(m, MOG_v_NameEdited) + rw(m, MOG_v_NameLength), c);
        ww(m, MOG_v_NameLength, (uint16_t)(rw(m, MOG_v_NameLength) + 1));
    }
done:                                                   /* LAB_00D2 */
    wb(m, rl(m, MOG_v_NameEdited) + rw(m, MOG_v_NameLength), 0);
    ww(m, MOG_v_NameEditing, 0);
}

/* LAB_00E5 : chevalier d0 donné au joueur a1 (nom, contrôle au joystick) */
static void take_knight(MogCombat *m, uint16_t d0, uint32_t a1)
{
    static const uint32_t names[4] = { MOG_s_SirGodber, MOG_s_SirRichard, MOG_s_SirJeffrey, MOG_s_SirEdward };
    mog_wait_vbls(m, 4);
    if (d0 > 3)
        return;
    wl(m, MOG_v_NameEdited, names[d0]);
    enter_name(m);
    wb(m, MOG_v_KnightsFree, (uint8_t)(rb(m, MOG_v_KnightsFree) & ~(1u << d0)));
    wl(m, a1 + OBJ_KNIGHT, d0);
    wb(m, a1 + OBJ_PORT, 2);
}

void mog_choose_knights(MogCombat *m)
{
    mog_clear_screen(m, rl(m, MOG_v_BgPlanes));
    mog_show_background(m);                             /* LAB_0418 */
    ww(m, MOG_v_ChooseLeft, rw(m, MOG_v_Players));
    wb(m, MOG_v_KnightsFree, 0x0F);
    wl(m, MOG_v_ChooseKnightObj, MOG_t_KnightObjects);
    ww(m, MOG_v_KnightChoice, 0);
    wl(m, MOG_v_CursorCel, rl(m, MOG_v_SheetPlanes));
    mog_set_planes(m, rl(m, MOG_v_DrawPlanes));
    knights_draw(m);
    mog_fade_to(m, MOG_t_MenuPalette);
    wl(m, MOG_v_LowHpGlowA, mog_glow(VM, 0x0F, 0x88, 1, 0));
    for (;;) {                                          /* LAB_00D4 */
        busy_tick(m);
        uint16_t d1 = mog_read_joy(m);
        if (d1 & MOG_JOY_FIRE) {
            take_knight(m, rw(m, MOG_v_KnightChoice), rl(m, MOG_v_ChooseKnightObj));
            ww(m, MOG_v_ChooseLeft, (uint16_t)(rw(m, MOG_v_ChooseLeft) - 1));
            if (!rw(m, MOG_v_ChooseLeft))
                break;
            wl(m, MOG_v_ChooseKnightObj, rl(m, MOG_v_ChooseKnightObj) + 0x84);
            uint16_t d0 = 0;                            /* LAB_00DD */
            while (!(rb(m, MOG_v_KnightsFree) & (1u << (d0 & 7))))
                d0++;
            ww(m, MOG_v_KnightChoice, d0);
            knights_draw(m);
        } else if (d1 & MOG_JOY_LEFT) {
            int16_t d0 = (int16_t)rw(m, MOG_v_KnightChoice);
            while (--d0 >= 0)
                if (rb(m, MOG_v_KnightsFree) & (1u << d0)) {
                    ww(m, MOG_v_KnightChoice, (uint16_t)d0);
                    knights_draw(m);
                    break;
                }
        } else if (d1 & MOG_JOY_RIGHT) {
            uint16_t d0 = rw(m, MOG_v_KnightChoice);
            while (++d0 != 4)
                if (rb(m, MOG_v_KnightsFree) & (1u << d0)) {
                    ww(m, MOG_v_KnightChoice, d0);
                    knights_draw(m);
                    break;
                }
        }
    }
    wl(m, rl(m, MOG_v_LowHpGlowA), 0);                      /* LAB_00DA */
    mog_fade_black(m);
}
