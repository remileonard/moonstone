/*
 * mog_text.h — textes de mog (mog_text.c).
 */
#ifndef MOG_TEXT_H
#define MOG_TEXT_H

#include "mog_combat.h"

/* LAB_0432 : enregistrements de texte chaînés à partir de a0 */
void mog_text_records(MogCombat *m, uint32_t a0);
/* LAB_0431 : chaîne en (x, y) avec les drapeaux (octet de poids faible) */
void mog_text(MogCombat *m, uint32_t str, uint16_t x, uint16_t y, uint16_t flags);
/* LAB_043D : largeur de la chaîne LAB_08E2 */
uint16_t mog_text_width(MogCombat *m, uint8_t flags);

#endif /* MOG_TEXT_H */
