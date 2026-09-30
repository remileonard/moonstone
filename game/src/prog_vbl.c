/*
 * prog_vbl.c — couleurs de l'interruption d'image de program (LAB_057D),
 * jumelle de celle de mog (LAB_0E5D) : mog_vbl.c compilé avec les adresses
 * de program. Seul écart : au pas de fondu, LAB_057D appelle LAB_0598
 * (volume de la musique) quand SECSTRT_32 est levé.
 */
#define PROG_TWIN
#include "prog_twin_vbl.h"
#include "prog_vbl.h"

#define mog_glow prog_glow
#define mog_cycle prog_cycle

/* Registres AUDxVOL écrits par LAB_0598 (musique de l'intro) */
void (*prog_audvol_hook)(int voice, uint16_t vol);

static void audvol(int voice, uint16_t vol)
{
    if (prog_audvol_hook)
        prog_audvol_hook(voice, vol);
}

/* LAB_0598 : SECSTRT_32 = 1 → LAB_0592 (volumes coupés puis voies 2 et 3
 * remontées au volume de la voie 1, comparaisons de l'original) ; sinon
 * baisse de 4 des volumes LAB_059D, bornés à 0. */
static void music_fade(IxVM *vm)
{
    uint32_t a = PROGRAM_LAB_059D;
    if (ix_rw(vm, PROGRAM_SECSTRT_32) == 1) {
        for (int v = 0; v < 4; v++)
            audvol(v, 0);
        uint16_t d0 = ix_rw(vm, a), d1 = ix_rw(vm, a + 2);
        int again;
        do {
            again = 0;
            uint16_t ref[4] = { d0, d1, d1, d1 };
            for (int v = 0; v < 4; v++)
                if (ix_rw(vm, a + 2 * v) != ref[v]) {
                    uint16_t n = (uint16_t)(ix_rw(vm, a + 2 * v) + 1);
                    ix_ww(vm, a + 2 * v, n);
                    audvol(v, n);
                    again = 1;
                }
        } while (again);
        return;
    }
    for (int v = 0; v < 4; v++) {
        int16_t n = (int16_t)(ix_rw(vm, a + 2 * v) - 4);
        if (n < 0)
            n = 0;
        ix_ww(vm, a + 2 * v, (uint16_t)n);
        audvol(v, (uint16_t)n);
    }
}

#define VBL_FADE_STEP(vm) do { \
        if (ix_rw(vm, PROGRAM_SECSTRT_32)) music_fade(vm); } while (0)

#include "mog_vbl.c"
