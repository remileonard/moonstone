/*
 * mog_encounter.h — préparation d'un combat de mog (mog_encounter.c).
 */
#ifndef MOG_ENCOUNTER_H
#define MOG_ENCOUNTER_H

#include "mog_combat.h"

/* LAB_01AE (partie combat), LAB_01BE, LAB_0011 : chevaliers de départ,
 * tables des contrôleurs (t_Controllers) et des rencontres
 * (t_CreatureInit). */
void mog_new_game(MogCombat *m);

/* LAB_0013 + LAB_0019 : PV maximum et défense du chevalier `obj` d'après
 * sa force, son armure et son inventaire. */
void mog_update_knight(MogCombat *m, uint32_t obj);
void mog_knight_hp(MogCombat *m, uint32_t obj);         /* LAB_0013 */
void mog_knight_defence(MogCombat *m, uint32_t obj);    /* LAB_0019 */

/* Routine de t_CreatureInit (LAB_0164, LAB_0168...) : décor, créatures,
 * adversaires, palette ; le combat démarre ensuite par mog_combat_begin.
 * Renvoie 0 si la routine n'est pas portée. */
int mog_encounter_init(MogCombat *m, uint32_t fn);

#endif /* MOG_ENCOUNTER_H */
