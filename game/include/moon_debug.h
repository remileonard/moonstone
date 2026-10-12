/*
 * moon_debug.h — mode debug du combat (touche G) : boîtes des sprites,
 * frames de frappe et de corps avec les points d'impact de collide.hit,
 * script et prochain opcode de chaque entité, PV des deux combattants,
 * images par seconde. Lecture seule : le jeu n'est pas modifié.
 */
#ifndef MOON_DEBUG_H
#define MOON_DEBUG_H

#include <stdint.h>
#include "mog_combat.h"

/* Branche le relevé sur le combat (MogCombat.debug). */
void moon_debug_attach(MogCombat *m);

/* Combat en cours : une image de combat dans les 200 dernières ms. */
int  moon_debug_in_combat(uint32_t now_ms);

void moon_debug_toggle(void);
int  moon_debug_enabled(void);

/* Une image montrée (compte des images par seconde) ; si le mode est
 * actif, un combat en cours et fb non NULL, y dessine le relevé
 * (320 × 200). */
void moon_debug_draw(uint32_t *fb, uint32_t now_ms);

#endif /* MOON_DEBUG_H */
