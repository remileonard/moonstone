/*
 * mog_gfx.c — tampons graphiques de mog (SECSTRT_30 / SECSTRT_28), à part
 * pour être compilé aussi pour program (prog_gfx.c : routines jumelles).
 */
#include "mog_boot.h"
#include "ix_mog_syms.h"

/* SECSTRT_30 (LAB_0D08) et SECSTRT_28 (LAB_0CD6) : tampons graphiques pris
 * dans SECSTRT_32 (dont LAB_0D40, tampon de retournement des frames),
 * table d'inversion des bits d'un octet LAB_0CD9, 5 plans (LAB_0D04). */
void mog_boot_graphics(IxVM *vm)
{
    ix_ww(vm, MOG_LAB_0D4D, 1);
    uint32_t d0 = MOG_SECSTRT_32;
    ix_wl(vm, MOG_LAB_0D3E, d0);
    d0 += 0x1000;
    ix_wl(vm, MOG_LAB_0D3D, d0);
    d0 += 0x222E;
    ix_wl(vm, MOG_LAB_0D3F, d0);
    d0 += 0x1000;
    ix_wl(vm, MOG_LAB_0D40, d0);
    d0 += 0x4B00 + 0x12C0;
    ix_wl(vm, MOG_LAB_0D41, d0);
    ix_ww(vm, MOG_LAB_0D04, 4);
    for (unsigned i = 0; i < 256; i++) {
        uint8_t r = 0;
        for (int b = 0; b < 8; b++)
            if (i & (1u << b))
                r |= (uint8_t)(0x80 >> b);
        ix_wb(vm, MOG_LAB_0CD9 + i, r);
    }
}
