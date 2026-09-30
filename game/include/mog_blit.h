/*
 * mog_blit.h — blitter de l'Amiga (mode copie) et dessin des frames CEL
 * de mog (LAB_0CDA) dans les plans de bits de sa mémoire (mog_blit.c).
 */
#ifndef MOG_BLIT_H
#define MOG_BLIT_H

#include <stdint.h>
#include "ix_vm.h"

typedef struct {
    uint16_t con0, con1, afwm, alwm;
    uint32_t apt, bpt, cpt, dpt;
    int16_t  amod, bmod, cmod, dmod;
    uint16_t adat, bdat, cdat;
    uint16_t olda, oldb;        /* mots précédents des canaux A et B (barillet) */
    unsigned long count;
} MogBlitter;

/* Écriture de BLTSIZE : exécute le blit décrit par les registres. */
void mog_blitter_run(IxVM *vm, MogBlitter *b, uint16_t size);

/* LAB_0CDA : frame `frame` de la CEL `cel` en (x, y) dans les plans
 * LAB_0CFF-LAB_0D03 (découpage LAB_0D26 / LAB_0D27, décalage LAB_0D28),
 * masque des plans présents, plans absents effacés. */
void mog_draw_cel(IxVM *vm, MogBlitter *b, uint32_t cel, uint16_t frame,
                  uint16_t x, uint16_t y);

#endif /* MOG_BLIT_H */
