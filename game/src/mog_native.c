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

/* LAB_0427 : tremblement d'écran (tâche VBL LAB_042A, table LAB_0430) */
void mog_shake(MogCombat *m)
{
    uint32_t a0 = MOG_LAB_0B96;                         /* 1re tâche VBL libre */
    while (ix_rl(VM, a0) && a0 < MOG_LAB_0B96 + 0x400)
        a0 += 4;
    ix_wl(VM, MOG_L00_0915E, a0);
    ix_ww(VM, MOG_LAB_042D, 3);
    ix_ww(VM, MOG_L00_0915A, 3);
    ix_wl(VM, MOG_LAB_042C, MOG_LAB_0430);
    ix_wl(VM, a0, MOG_LAB_042A);
}

/* SECSTRT_16 / LAB_0A9B-LAB_0A9D : son n sur le canal ch (LAB_0F8C) */
void mog_voice(MogCombat *m, int ch, int n)
{
    ix_wb(VM, MOG_LAB_0AA6, (uint8_t)(ix_rb(VM, MOG_LAB_0AA6) | (1u << ch)));
    if (m->voice)
        m->voice(m->out.user, ch, n);
}

/* LAB_0A9E-LAB_0AA0 : libère le canal ch, son $A7 */
static void voice_off(MogCombat *m, int ch)
{
    ix_wb(VM, MOG_LAB_0AA6, (uint8_t)(ix_rb(VM, MOG_LAB_0AA6) & (0x0F & ~(1u << ch))));
    mog_sound(m, 0xA7);
}

/* LAB_0D8A (vers les registres couleur) puis LAB_03EE (copie dans LAB_0E93) */
static void set_palette(MogCombat *m, uint32_t a0)
{
    uint16_t c[32];
    for (int i = 0; i < 32; i++)
        c[i] = ix_rw(VM, a0 + 2u * (unsigned)i);
    mog_wait_vbls(m, 1);                                /* LAB_0D8A : LAB_0D77 */
    if (m->palette)
        m->palette(m->out.user, c);
    uint32_t a1 = ix_rl(VM, MOG_LAB_0E93);
    for (int i = 0; i < 32; i++)
        ix_ww(VM, a1 + 2u * (unsigned)i, c[i]);
}

/* LAB_0EF7 : le Démon projette le chevalier (LAB_05F2) derrière lui */
static void demon_throw(MogCombat *m)
{
    uint32_t k = ix_rl(VM, MOG_LAB_05F2);
    mog_toggle_freeze(m, k);
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_05F2));
    uint32_t a1 = ix_rl(VM, MOG_LAB_01A1);
    uint8_t dir = (uint8_t)(ix_rb(VM, a1 + 10) ^ 2);
    ix_wb(VM, en + 22, dir);
    ix_ww(VM, en + 10, ix_rw(VM, a1 + 8));
    uint16_t x = ix_rw(VM, a1 + 4);
    x = (uint16_t)(dir == 1 ? x - 0x89 : x + 0x89);
    if (!((int16_t)x < 0x140))
        x = 0x13F;
    if ((int16_t)x < 0)
        x = 1;
    ix_ww(VM, en + 6, x);
    a1 = ix_rl(VM, MOG_LAB_05F2);
    uint32_t script = ix_rl(VM, a1 + 22);
    if (!((int16_t)ix_rw(VM, a1 + 80) > 0)) {           /* mort : fondu au noir */
        for (int i = 0; i < 6; i++)
            ix_ww(VM, MOG_LAB_08D9 + 4 + 2u * (unsigned)i, 0);
        set_palette(m, MOG_LAB_08D9);
        script = MOG_LAB_07F7;
    }
    mog_restart_entity(m, ix_rl(VM, MOG_LAB_05F2), script);
}

/* LAB_0EEB : compagnon du Démon (objet LAB_01A2, script LAB_08AF) */
static void demon_companion(MogCombat *m, uint32_t banks)
{
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_01A1));
    uint32_t a1 = ix_rl(VM, MOG_LAB_01A2);
    uint16_t x = ix_rw(VM, en + 6), h = ix_rw(VM, en + 8);
    uint8_t dir = ix_rb(VM, en + 22);
    ix_ww(VM, a1 + 4, x);
    ix_ww(VM, a1 + 6, h);
    ix_ww(VM, a1 + 8, x);                               /* (X aussi en profondeur) */
    ix_wb(VM, a1 + 10, dir);
    ix_start_entity(&m->eng, MOG_LAB_08AF, a1, banks, (int16_t)x, (int16_t)h,
                    (int16_t)x, dir, 8);
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
    case MOG_LAB_0427: mog_shake(m); return 1;
    case MOG_LAB_01C8: {                                /* compte à rebours 13(objet) */
        int8_t n = (int8_t)(ix_rb(VM, obj + 13) - 1);
        ix_wb(VM, obj + 13, (uint8_t)n);
        if (n < 0) {
            mog_random(m);                              /* (valeur non utilisée) */
            ix_wb(VM, obj + 13, 0x1E);
        }
        return 1;
    }
    case MOG_LAB_028E: mog_dragon_move(m); return 1;
    case MOG_LAB_02AC: mog_voice(m, 0, 0x1D); return 1;
    case MOG_LAB_02AD: mog_voice(m, 1, 0x1B); return 1;
    case MOG_LAB_02AE: mog_sound(m, 0x30); mog_sound(m, 0x12); return 1;
    case MOG_LAB_0ED0:                                 /* deux sons de LAB_0ECF */
        for (int k = 0; k < 2; k++)
            mog_sound(m, ix_rb(VM, MOG_LAB_0ECF + (mog_random(m) & 7)));
        return 1;
    case MOG_LAB_02E8: sound_sequence(m, MOG_LAB_02EF); return 1;
    case MOG_LAB_0A9E: voice_off(m, 0); return 1;
    case MOG_LAB_0A9F: voice_off(m, 1); return 1;
    case MOG_LAB_0AA0: voice_off(m, 2); return 1;
    case MOG_LAB_0EB8: mog_voice(m, 0, 0x50); return 1;
    case MOG_LAB_0EB9: {                                /* Mudmen : deux sons */
        static const uint8_t pair[4][2] = { { 0x57, 0x94 }, { 0x58, 0x95 },
                                            { 0x59, 0x96 }, { 0x97, 0x98 } };
        int k = (int)(mog_random(m) & 3);
        mog_sound(m, pair[k][0]);
        mog_sound(m, pair[k][1]);
        return 1;
    }
    case MOG_LAB_0EBD:
        mog_sound(m, ix_rb(VM, MOG_LAB_0EC0 + (mog_random(m) & 3)));
        return 1;
    case MOG_LAB_0EBE: {
        uint16_t n = (uint16_t)((ix_rw(VM, MOG_LAB_0EC1) + 1) & 3);
        ix_ww(VM, MOG_LAB_0EC1, n);
        if (!n)
            mog_sound(m, 0x5A);
        return 1;
    }
    case MOG_LAB_0EEB: demon_companion(m, ix_rl(VM, en + 28)); return 1;
    case MOG_LAB_0EEC: {
        uint32_t e2 = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_01A2));
        uint32_t e1 = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_01A1));
        ix_wb(VM, e2 + 22, ix_rb(VM, e1 + 22));
        return 1;
    }
    case MOG_LAB_0EED: mog_kill_entity_of(m, ix_rl(VM, MOG_LAB_01A2)); return 1;
    case MOG_LAB_0EEE:
        mog_voice(m, 0, 0x4A);
        mog_voice(m, 1, 0x4B);
        mog_voice(m, 2, 0x4C);
        mog_voice(m, 3, 0x4D);
        return 1;
    case MOG_LAB_0EEF: {
        int k = (int)(mog_random(m) & 3);
        int base = k == 1 ? 0x42 : k == 2 ? 0x44 : 0x40;
        mog_sound(m, base);
        mog_sound(m, base + 1);
        return 1;
    }
    case MOG_LAB_0EF6: {
        mog_message(m, "DEMON HIT");
        uint32_t k = ix_rl(VM, MOG_LAB_05F2);
        ix_ww(VM, k + 80, (uint16_t)(ix_rw(VM, k + 80) - 10));
        mog_toggle_freeze(m, k);
        return 1;
    }
    case MOG_LAB_0EF7: demon_throw(m); return 1;
    case MOG_LAB_02E9: sound_sequence(m, MOG_LAB_02EE); return 1;
    }
    return 0;
}
