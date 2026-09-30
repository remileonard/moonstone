/*
 * mog_gfx.c — tampons graphiques de mog (SECSTRT_30 / SECSTRT_28), à part
 * pour être compilé aussi pour program (prog_gfx.c : routines jumelles).
 */
#include "mog_boot.h"
#include "ix_mog_names.h"

/* SECSTRT_30 (LAB_0D08) et SECSTRT_28 (LAB_0CD6) : tampons graphiques pris
 * dans SECSTRT_32 (dont LAB_0D40, tampon de retournement des frames),
 * table d'inversion des bits d'un octet LAB_0CD9, 5 plans (LAB_0D04). */
void mog_boot_graphics(IxVM *vm)
{
    ix_ww(vm, MOG_v_GfxReady, 1);
    uint32_t d0 = MOG_b_Gfx;
    ix_wl(vm, MOG_v_GfxTableA, d0);
    d0 += 0x1000;
    ix_wl(vm, MOG_v_GfxTableB, d0);
    d0 += 0x222E;
    ix_wl(vm, MOG_v_GfxTableC, d0);
    d0 += 0x1000;
    ix_wl(vm, MOG_v_CelPlanesBuf, d0);
    d0 += 0x4B00 + 0x12C0;
    ix_wl(vm, MOG_v_GfxTableD, d0);
    ix_ww(vm, MOG_v_CelPlanesMax, 4);
    for (unsigned i = 0; i < 256; i++) {
        uint8_t r = 0;
        for (int b = 0; b < 8; b++)
            if (i & (1u << b))
                r |= (uint8_t)(0x80 >> b);
        ix_wb(vm, MOG_t_BitReverse + i, r);
    }
}
