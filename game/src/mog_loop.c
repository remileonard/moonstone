/*
 * mog_loop.c — une image de combat (Combat_Loop [LAB_0037]), partie
 * logique, traduite de amiga_asm/mog.asm.
 *
 * L'affichage (bascule des écrans LAB_0D71, restauration du décor
 * LAB_039E, indicateur de PV) est laissé à l'hôte : le moteur lui envoie
 * les dessins de frames (IxHost.draw) ; ici ne sont tenues que les
 * variables de mémoire qui en dépendent.
 */
#include "mog_private.h"
#include "ix_mog_syms.h"
#include "mog_vbl.h"

#define VM (m->eng.vm)


/* LAB_0416 (partie mémoire) : écrans échangés (LAB_0D71 : LAB_0D92 <->
 * SECSTRT_35), les deux piles de zones à restaurer aussi ; la nouvelle
 * pile est vide ; dessins vers LAB_0D92. */
static void swap_scrap(MogCombat *m)
{
    uint32_t s = ix_rl(VM, MOG_LAB_0D92);
    for (uint32_t p = 0; p < 5; p++) {                  /* copper : écran montré */
        uint32_t d0 = s + p * 0x1F40;
        ix_ww(VM, MOG_COPPER_BPL + 8 * p, (uint16_t)(d0 >> 16));
        ix_ww(VM, MOG_COPPER_BPL + 8 * p + 4, (uint16_t)d0);
    }
    ix_wl(VM, MOG_LAB_0D92, ix_rl(VM, MOG_SECSTRT_35));
    ix_wl(VM, MOG_SECSTRT_35, s);
    uint32_t a = ix_rl(VM, MOG_LAB_063E);
    ix_wl(VM, MOG_LAB_063E, ix_rl(VM, MOG_LAB_063F));
    ix_wl(VM, MOG_LAB_063F, a);
    ix_wl(VM, MOG_LAB_0641, ix_rl(VM, MOG_LAB_063E));
    static const uint32_t planes[5] = { MOG_LAB_0CFF, MOG_LAB_0D00, MOG_LAB_0D01,
                                        MOG_LAB_0D02, MOG_LAB_0D03 };
    for (uint32_t i = 0, d0 = ix_rl(VM, MOG_LAB_0D92); i < 5; i++, d0 += 0x1F40)
        ix_wl(VM, planes[i], d0);                       /* L00_0908E */
    ix_ww(VM, MOG_LAB_0645, 0);
}

/* LAB_039E (partie mémoire) : compte des zones restaurées (au plus 45). */
static void count_restored(MogCombat *m)
{
    ix_wl(VM, MOG_LAB_0631, 0);
    ix_wl(VM, MOG_LAB_0642, ix_rl(VM, MOG_LAB_0D92));
    uint32_t a6 = ix_rl(VM, MOG_LAB_063E);
    while (ix_rw(VM, a6 + 4) != 0xFFFF && ix_rl(VM, MOG_LAB_0631) != 0x2D
           && ix_rw(VM, a6 + 6) && ix_rw(VM, a6 + 4)) {
        a6 += 8;
        ix_wl(VM, MOG_LAB_0631, ix_rl(VM, MOG_LAB_0631) + 1);
    }
}

/* Combat_CheckEnd [LAB_004B] (sans la pause clavier) : joueur à terre ->
 * fin dans 50 images. */
static void check_end(MogCombat *m)
{
    uint32_t a2 = MOG_v_Combatants;
    if (ix_rb(VM, a2 + 8) && (int16_t)ix_rw(VM, ix_rl(VM, a2) + 80) < 0) {
        ix_wb(VM, a2 + 16, 0x32);
        ix_wb(VM, a2 + 8, 0);
    }
}

/* Combat_Run [LAB_0036] : mise en route (partie mémoire). */
void mog_combat_begin(MogCombat *m)
{
    ix_ww(VM, MOG_LAB_0620, 0);
    ix_ww(VM, MOG_L00_072FA, 0);
    ix_wl(VM, MOG_LAB_05A5, 0);
    ix_wl(VM, MOG_LAB_05A8, 0);
    for (uint32_t i = 0; i < 128; i++)                  /* LAB_0B82 : clavier */
        ix_wb(VM, MOG_LAB_0B91 + i, 0);
    ix_ww(VM, MOG_SECSTRT_21, 0);
    ix_wb(VM, MOG_v_Combatants + 8, 1);
    ix_wl(VM, MOG_LAB_05F2, ix_rl(VM, MOG_v_Combatants));
    for (uint32_t i = 0; i < 0x78; i++)                 /* LAB_02F2 : trajectoires */
        ix_wb(VM, MOG_LAB_0301 + i, 0);
    ix_ww(VM, MOG_LAB_05AB, 1);
    ix_wl(VM, MOG_LAB_05AC, ix_rl(VM, MOG_v_VblCounter));
    swap_scrap(m);                                      /* LAB_0416 */
    uint32_t obj = ix_rl(VM, MOG_LAB_05C3);             /* LAB_000A : tout gelé */
    for (int i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (obj != ix_rl(VM, MOG_LAB_05F4))
            mog_toggle_freeze(m, obj);
    mog_toggle_freeze(m, MOG_LAB_0617);
    if (ix_rb(VM, MOG_LAB_05DF) == 4) {                 /* LAB_0412 */
        mog_message(m, "Turning on colour glow");
        if (m->palette) {                               /* LAB_0D8A */
            uint16_t c[32];
            for (int i = 0; i < 32; i++)
                c[i] = ix_rw(VM, MOG_LAB_08D9 + 2u * (unsigned)i);
            m->palette(m->out.user, c);
        }
        uint32_t cur = ix_rl(VM, MOG_LAB_0E93);         /* LAB_03EE */
        for (uint32_t i = 0; i < 64; i++)
            ix_wb(VM, cur + i, ix_rb(VM, MOG_LAB_08D9 + i));
        /* LAB_0E5A : couleur 14 pulsant vers $100, vitesse 2 */
        uint32_t a0 = MOG_LAB_0E95;
        int i;
        for (i = 0; i < 6 && ix_rl(VM, a0); i++)
            a0 += 12;
        if (i < 6) {
            ix_ww(VM, a0, 14);
            ix_ww(VM, a0 + 2, 0x100);
            ix_ww(VM, a0 + 4, 2);
            ix_ww(VM, a0 + 6, 2);
            ix_ww(VM, a0 + 8, ix_rw(VM, cur + 28));
            ix_ww(VM, a0 + 10, 0);
            ix_wl(VM, MOG_LAB_0414, a0);
        }
    }
}

int mog_combat_frame(MogCombat *m)
{
    ix_wl(VM, MOG_LAB_0321, ix_rl(VM, MOG_v_VblCounter));   /* Combat_FrameStart */
    mog_run_controllers(m);
    ix_run_entities(&m->eng);
    swap_scrap(m);
    mog_collisions(m);
    count_restored(m);
    check_end(m);

    /* suite de Combat_Loop : combat actif, puis encore 16(v_Combatants)
     * images */
    uint32_t a2 = MOG_v_Combatants;
    if (ix_rb(VM, a2 + 8))
        return 1;
    uint8_t n = (uint8_t)(ix_rb(VM, a2 + 16) - 1);
    ix_wb(VM, a2 + 16, n);
    return n != 0;
}

void mog_wait_vbls(MogCombat *m, unsigned n)
{
    while (n--) {
        if (m->wait_vbl)
            m->wait_vbl(m->out.user);
        else
            ix_wl(VM, MOG_v_VblCounter, ix_rl(VM, MOG_v_VblCounter) + 1);
    }
}

void mog_frame_wait(MogCombat *m)
{
    int32_t d0 = (int32_t)(ix_rl(VM, MOG_v_VblCounter) - ix_rl(VM, MOG_LAB_0321));
    int32_t d1 = (int16_t)ix_rw(VM, MOG_v_FrameVbls) - d0;
    if (d1 < 0)
        d1 = 0;
    mog_wait_vbls(m, (unsigned)d1);
}
