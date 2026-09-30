/*
 * prog_blit.h — blitter, dessin des CEL et tampons graphiques de program
 * (prog_blit.c, prog_gfx.c : code de mog compilé aux adresses de program).
 */
#ifndef PROG_BLIT_H
#define PROG_BLIT_H

#include "mog_blit.h"
#include "mog_boot.h"

void prog_blitter_run(IxVM *vm, MogBlitter *b, uint16_t size);
void prog_draw_cel(IxVM *vm, MogBlitter *b, uint32_t cel, uint16_t frame,
                   uint16_t x, uint16_t y);
void prog_boot_graphics(IxVM *vm);

/* prog_files.c : LAB_0390 / LAB_03B2 / LAB_03DA, Unpack_Lzss, LAB_0496 /
 * LAB_0491 (mêmes codes que mog) */
int prog_file_open(IxVM *vm, uint32_t name, MogFile *f);
uint32_t prog_file_read(IxVM *vm, MogFile *f, uint32_t dst, uint32_t n);
void prog_file_close(MogFile *f);
uint32_t prog_unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst);
void prog_load_cel(IxVM *vm, uint32_t name, uint32_t dest);
uint32_t prog_cel_size(IxVM *vm, uint32_t name);

#endif /* PROG_BLIT_H */
