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

/* LAB_0575 / LAB_057B : pointeur montré / caché ; LAB_044E, LAB_0448,
 * LAB_0451 : zones cliquables (modèle LAB_0A58) ; LAB_0419 : copie d'écran. */
void     mog_pointer_on(MogCombat *m);
void     mog_pointer_off(MogCombat *m);
void     mog_clear_zones(MogCombat *m);
void     mog_add_zone(MogCombat *m);
uint32_t mog_zone_at(MogCombat *m, uint16_t x, uint16_t y);
void     mog_copy_screen(MogCombat *m, uint32_t a0, uint32_t a1);

#endif /* MOG_SCREENS_H */
