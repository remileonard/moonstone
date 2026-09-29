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

#define VM (m->eng.vm)

/* LAB_0416 (partie mémoire) : écrans échangés (LAB_0D71 : LAB_0D92 <->
 * SECSTRT_35), les deux piles de zones à restaurer aussi ; la nouvelle
 * pile est vide. */
static void swap_scrap(MogCombat *m)
{
    uint32_t s = ix_rl(VM, MOG_LAB_0D92);
    ix_wl(VM, MOG_LAB_0D92, ix_rl(VM, MOG_SECSTRT_35));
    ix_wl(VM, MOG_SECSTRT_35, s);
    uint32_t a = ix_rl(VM, MOG_LAB_063E);
    ix_wl(VM, MOG_LAB_063E, ix_rl(VM, MOG_LAB_063F));
    ix_wl(VM, MOG_LAB_063F, a);
    ix_wl(VM, MOG_LAB_0641, ix_rl(VM, MOG_LAB_063E));
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
    if (ix_rb(VM, MOG_LAB_05DF) == 4)                   /* LAB_0412 */
        mog_message(m, "Turning on colour glow");
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
