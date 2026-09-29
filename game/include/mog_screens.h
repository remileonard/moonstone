/*
 * mog_screens.h — écrans à pointeur de mog (LAB_04CF, mog_screens.c).
 */
#ifndef MOG_SCREENS_H
#define MOG_SCREENS_H

#include "mog_combat.h"

/* LAB_04CF : écran de genre kind (1 vainqueur d'un duel, 9 inventaire,
 * 10 trésor du dragon, ...) mené jusqu'à la sortie. */
void mog_screen_run(MogCombat *m, uint32_t kind);
/* LAB_057D : part de l'interruption d'image qui mène le pointeur. */
void mog_screen_vbl(MogCombat *m);
/* LAB_0572 : sprite du pointeur préparé au démarrage. */
void mog_pointer_boot(MogCombat *m);

#endif /* MOG_SCREENS_H */
