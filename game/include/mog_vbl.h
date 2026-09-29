/*
 * mog_vbl.h — couleurs tenues par l'interruption d'image de mog et écran
 * montré (mog_vbl.c).
 */
#ifndef MOG_VBL_H
#define MOG_VBL_H

#include <stdint.h>
#include "ix_vm.h"

#define MOG_COPPER_BPL 0x7F6B0u     /* EXT_0024 : pointeurs de plans (copper) */

/* LAB_0E5D (couleurs) : une VBL. `colour` (registres COLOR00-31) est mis à
 * jour quand la palette courante change. */
void mog_vbl_colours(IxVM *vm, uint16_t colour[32]);

/* Écran montré (plans de la copper list) en ARGB8888, 320 × 200. */
void mog_screen(const IxVM *vm, const uint16_t colour[32], uint32_t *argb);

/* LAB_0E5A / LAB_0E56 : pulsation et rotation de couleurs (inscription) */
uint32_t mog_glow(IxVM *vm, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3);
uint32_t mog_cycle(IxVM *vm, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

#endif /* MOG_VBL_H */
