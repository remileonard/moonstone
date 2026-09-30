/*
 * prog_blit.h — blitter, dessin des CEL et tampons graphiques de program
 * (prog_blit.c, prog_gfx.c : code de mog compilé aux adresses de program).
 */
#ifndef PROG_BLIT_H
#define PROG_BLIT_H

#include "mog_blit.h"

void prog_blitter_run(IxVM *vm, MogBlitter *b, uint16_t size);
void prog_draw_cel(IxVM *vm, MogBlitter *b, uint32_t cel, uint16_t frame,
                   uint16_t x, uint16_t y);
void prog_boot_graphics(IxVM *vm);

#endif /* PROG_BLIT_H */
