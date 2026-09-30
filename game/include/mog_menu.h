/*
 * mog_menu.h — menu du début de partie et choix des chevaliers (mog_menu.c).
 */
#ifndef MOG_MENU_H
#define MOG_MENU_H

#include "mog_combat.h"

/* Prot_CopylockCheck et le menu (LAB_00B5) : renvoie la ligne choisie,
 * 2 (« Practice ») ou 3 (« Select Knight » : partie de LAB_05C5 joueurs). */
int  mog_menu(MogCombat *m);
/* LAB_00D3 : chaque joueur prend un chevalier (LAB_0613 + 0x84 × n) et
 * tape son nom. */
void mog_choose_knights(MogCombat *m);

#endif /* MOG_MENU_H */
