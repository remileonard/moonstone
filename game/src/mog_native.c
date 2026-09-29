/*
 * mog_native.c — routines natives appelées par les scripts du combat
 * (opcode $B0), traduites de amiga_asm/mog.asm.
 *
 * $B0 passe D0-D3 = X, hauteur, profondeur, direction de l'entité,
 * A1 = son objet, A2 = ses banques CEL.
 */
#include "mog_private.h"
#include "ix_mog_syms.h"

#include <stdio.h>

#define VM (m->eng.vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }

/* LAB_04A1 : 8 pas d'un registre à décalage (bit entrant = bit 4 ^ bit 1) */
uint32_t mog_random(MogCombat *m)
{
    uint32_t d0 = ix_rl(VM, MOG_LAB_0973);
    for (int i = 0; i < 8; i++) {
        uint32_t b = ((d0 >> 4) ^ (d0 >> 1)) & 1u;
        d0 = (d0 >> 1) | (b << 31);
    }
    ix_wl(VM, MOG_LAB_0973, d0);
    return d0;
}

/* LAB_0319 */
void mog_toggle_freeze(MogCombat *m, uint32_t obj)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (en)
        ix_ww(VM, en + 48, (uint16_t)(ix_rw(VM, en + 48) ^ 1));
}

/* LAB_031B : l'entité de l'objet disparaît (objet libéré). */
void mog_kill_entity_of(MogCombat *m, uint32_t obj)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (!en)
        return;
    ix_ww(VM, en, 0);
    ix_wl(VM, ix_rl(VM, en + 24), 0);
}

/* LAB_0006 : fin du combat dans 35 images */
void mog_end_combat(MogCombat *m)
{
    if (ix_rb(VM, MOG_v_Combatants + 8)) {
        ix_wb(VM, MOG_v_Combatants + 16, 0x23);
        ix_wb(VM, MOG_v_Combatants + 8, 0);
    }
}

/* LAB_0005 : un adversaire de moins ; le suivant entre, ou le combat
 * se termine (joueur LAB_05F2 mort, ou plus personne). */
static void opponent_down(MogCombat *m)
{
    uint32_t a0 = ix_rl(VM, MOG_LAB_05F2);
    if (sw(ix_rw(VM, a0 + 80)) <= 0) {
        mog_end_combat(m);
        return;
    }
    ix_ww(VM, MOG_LAB_05EE, (uint16_t)(ix_rw(VM, MOG_LAB_05EE) - 1));
    int16_t left = sw(ix_rw(VM, MOG_LAB_05EC));
    ix_ww(VM, MOG_LAB_05EC, (uint16_t)(left - 1));
    if (!(left > 1) && ix_rw(VM, MOG_LAB_05EE) == 0) {   /* BGT sur SUBI */
        mog_end_combat(m);
        return;
    }
    for (;;) {                                          /* LAB_0008 */
        if (ix_rw(VM, MOG_LAB_05ED) == ix_rw(VM, MOG_LAB_05EE))
            return;
        uint32_t fn = ix_rl(VM, MOG_LAB_05F0);
        if (sw(ix_rw(VM, MOG_LAB_05EC)) <= 0)
            return;
        if (!mog_next_opponent(m, fn))
            return;
    }
}

/* LAB_000A : gèle (ou dégèle) tout le monde sauf LAB_05F4 ; et LAB_0617. */
static void freeze_others(MogCombat *m)
{
    uint32_t obj = ix_rl(VM, MOG_LAB_05C3);
    for (int i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (obj != ix_rl(VM, MOG_LAB_05F4))
            mog_toggle_freeze(m, obj);
    mog_toggle_freeze(m, MOG_LAB_0617);
}

/* LAB_0211 : recul (table de pas LAB_08CE, sens LAB_08D0) */
static void knockback(MogCombat *m, uint32_t a1)
{
    ix_ww(VM, MOG_LAB_08CF, (uint16_t)(ix_rw(VM, MOG_LAB_08CF) + 1));
    ix_wl(VM, MOG_LAB_0633, a1);
    uint32_t a0 = a1;
    ix_ww(VM, a0 + 62, 0);
    uint8_t dir = ix_rb(VM, MOG_LAB_08D0);
    ix_wb(VM, a0 + 10, dir);
    int bit = dir == 1 ? 0 : 1;                         /* LAB_0213 : vers la droite */
    ix_wb(VM, a0 + 63, (uint8_t)(ix_rb(VM, a0 + 63) | (1u << bit)));
    mog_arena_bounds(m, a0);
    if (!(ix_rb(VM, a0 + 63) & (1u << bit)))
        return;
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_0633));
    uint32_t tab = ix_rl(VM, MOG_LAB_08CE);
    uint16_t k = (uint16_t)(ix_rw(VM, MOG_LAB_08CF) << 1);
    uint16_t step = ix_rw(VM, tab + (uint32_t)(int32_t)sw(k));
    uint16_t x = ix_rw(VM, en + 6);
    ix_ww(VM, en + 6, (uint16_t)(bit ? x - step : x + step));
}

/* LAB_02CA : lance un objet (LAB_07EB, contrôleur 52) */
static void throw_object(MogCombat *m, uint32_t en, uint32_t a1)
{
    ix_wb(VM, a1 + 76, (uint8_t)(ix_rb(VM, a1 + 76) - 1));
    uint32_t o = ix_spawn(&m->eng, MOG_LAB_07EB, ix_rl(VM, en + 28),
                          sw(ix_rw(VM, en + 6)), sw(ix_rw(VM, en + 8)),
                          sw(ix_rw(VM, en + 10)), ix_rb(VM, en + 22), 52);
    ix_wl(VM, o + 42, MOG_LAB_0302);
    ix_ww(VM, o + 64, 0x0C);
    ix_wb(VM, o + 77, 0x34);
}

/* LAB_02E8 / LAB_02E9 : sons en séquence (tables de mots terminées par -1) */
static void sound_sequence(MogCombat *m, uint32_t tab)
{
    uint16_t k = (uint16_t)(ix_rw(VM, MOG_L00_072FA) + 2);
    ix_ww(VM, MOG_L00_072FA, k);
    if (ix_rw(VM, tab + (uint32_t)(int32_t)sw(k)) == 0xFFFF) {
        ix_ww(VM, MOG_L00_072FA, 0);
        k = 0;
    }
    mog_sound(m, ix_rw(VM, tab + (uint32_t)(int32_t)sw(k)) & 0xFF);
}

/* rnd & mask, moins 1 si non nul (LAB_02DC, LAB_02DE, LAB_02E4) */
static int rnd_dec(MogCombat *m, uint32_t mask)
{
    uint32_t d0 = mog_random(m) & mask;
    return d0 ? (int)d0 - 1 : 0;
}

int mog_native(MogCombat *m, uint32_t routine, uint32_t en)
{
    uint32_t obj = ix_rl(VM, en + 24);
    switch (routine) {
    case MOG_LAB_0005: opponent_down(m); return 1;
    case MOG_LAB_0006: mog_end_combat(m); return 1;
    case MOG_LAB_000A: freeze_others(m); return 1;
    case MOG_LAB_000D:                                  /* le joueur meurt */
        ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);
        return 1;
    case MOG_LAB_0210: ix_ww(VM, MOG_LAB_08CF, 0xFFFF); return 1;
    case MOG_LAB_0211: knockback(m, obj); return 1;
    case MOG_LAB_02CA: throw_object(m, en, obj); return 1;
    case MOG_LAB_02DB: ix_ww(VM, MOG_LAB_0620, 1); return 1;
    case MOG_LAB_02DC: mog_sound(m, rnd_dec(m, 3) + 0x1E); return 1;
    case MOG_LAB_02DE:
        mog_sound(m, rnd_dec(m, 3) + 0x18);
        mog_sound(m, (int)(mog_random(m) & 3) + 0x14);
        return 1;
    case MOG_LAB_02E0: mog_sound(m, (int)(mog_random(m) & 1) + 0x5B); return 1;
    case MOG_LAB_02E1: mog_sound(m, (int)(mog_random(m) & 1) + 0x6A); return 1;
    case MOG_LAB_02E2: mog_sound(m, (int)(mog_random(m) & 3) + 0x61); return 1;
    case MOG_LAB_02E3: mog_sound(m, (int)(mog_random(m) & 1) + 0x65); return 1;
    case MOG_LAB_02E4: mog_sound(m, rnd_dec(m, 3) + 0x67); return 1;
    case MOG_LAB_02E6: {                                /* pas */
        uint16_t n = (uint16_t)(ix_rw(VM, MOG_LAB_02EC) + 1);
        if (!(sw(n) < 5))
            n = 0;
        ix_ww(VM, MOG_LAB_02EC, n);
        mog_sound(m, (n + 4) & 0xFF);
        return 1;
    }
    case MOG_LAB_02E8: sound_sequence(m, MOG_LAB_02EF); return 1;
    case MOG_LAB_02E9: sound_sequence(m, MOG_LAB_02EE); return 1;
    }
    return 0;
}
