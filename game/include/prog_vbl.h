/*
 * prog_vbl.h — couleurs de l'interruption d'image de program (LAB_057D)
 * et écran montré (prog_vbl.c, jumelle de mog_vbl.c).
 */
#ifndef PROG_VBL_H
#define PROG_VBL_H

#include <stdint.h>
#include "ix_vm.h"

void prog_vbl_colours(IxVM *vm, uint16_t colour[32]);
void prog_screen(const IxVM *vm, const uint16_t colour[32], uint32_t *argb);
uint32_t prog_glow(IxVM *vm, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3);
uint32_t prog_cycle(IxVM *vm, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3);

/* AUDxVOL écrits par LAB_0598 (NULL : ignorés) */
extern void (*prog_audvol_hook)(int voice, uint16_t vol);

#endif /* PROG_VBL_H */
