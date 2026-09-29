/*
 * mog_private.h — routines partagées entre les fichiers du combat (mog_*.c).
 */
#ifndef MOG_PRIVATE_H
#define MOG_PRIVATE_H

#include "mog_combat.h"

/* LAB_0215 : efface les bits de 63(objet) qui sortiraient de l'arène. */
void mog_arena_bounds(MogCombat *m, uint32_t obj);

/* LAB_0319 : gèle / dégèle l'entité de l'objet (48(entité) ^= 1). */
void mog_toggle_freeze(MogCombat *m, uint32_t obj);

void mog_sound(MogCombat *m, int n);
void mog_message(MogCombat *m, const char *t);

#endif /* MOG_PRIVATE_H */
