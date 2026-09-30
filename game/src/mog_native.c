/*
 * mog_native.c — routines natives appelées par les scripts du combat
 * (opcode $B0), traduites de amiga_asm/mog.asm.
 *
 * $B0 passe D0-D3 = X, hauteur, profondeur, direction de l'entité,
 * A1 = son objet, A2 = ses banques CEL.
 */
#include "mog_private.h"
#include "ix_mog_names.h"
#include "mog_struct.h"
#include "mog_sound.h"

#include <stdio.h>

#define VM (m->eng.vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }

/* LAB_04A1 : 8 pas d'un registre à décalage (bit entrant = bit 4 ^ bit 1) */
uint32_t mog_random(MogCombat *m)
{
    uint32_t d0 = ix_rl(VM, MOG_v_RandomSeed);
    for (int i = 0; i < 8; i++) {
        uint32_t b = ((d0 >> 4) ^ (d0 >> 1)) & 1u;
        d0 = (d0 >> 1) | (b << 31);
    }
    ix_wl(VM, MOG_v_RandomSeed, d0);
    return d0;
}

/* LAB_0319 */
void mog_toggle_freeze(MogCombat *m, uint32_t obj)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (en)
        ix_ww(VM, en + ENT_FROZEN, (uint16_t)(ix_rw(VM, en + ENT_FROZEN) ^ 1));
}

/* LAB_031B : l'entité de l'objet disparaît (objet libéré). */
void mog_kill_entity_of(MogCombat *m, uint32_t obj)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (!en)
        return;
    ix_ww(VM, en, 0);
    ix_wl(VM, ix_rl(VM, en + ENT_OBJ), 0);
}

/* LAB_0006 : fin du combat dans 35 images */
void mog_end_combat(MogCombat *m)
{
    if (ix_rb(VM, MOG_v_Combatants + CMB_ACTIVE)) {
        ix_wb(VM, MOG_v_Combatants + CMB_END_DELAY, 0x23);
        ix_wb(VM, MOG_v_Combatants + CMB_ACTIVE, 0);
    }
}

/* LAB_0005 : un adversaire de moins ; le suivant entre, ou le combat
 * se termine (joueur LAB_05F2 mort, ou plus personne). */
static void opponent_down(MogCombat *m)
{
    uint32_t a0 = ix_rl(VM, MOG_v_PlayerObj);
    if (sw(ix_rw(VM, a0 + OBJ_HP)) <= 0) {
        mog_end_combat(m);
        return;
    }
    ix_ww(VM, MOG_v_FoesEntered, (uint16_t)(ix_rw(VM, MOG_v_FoesEntered) - 1));
    int16_t left = sw(ix_rw(VM, MOG_v_FoesToBeat));
    ix_ww(VM, MOG_v_FoesToBeat, (uint16_t)(left - 1));
    if (!(left > 1) && ix_rw(VM, MOG_v_FoesEntered) == 0) {   /* BGT sur SUBI */
        mog_end_combat(m);
        return;
    }
    for (;;) {                                          /* LAB_0008 */
        if (ix_rw(VM, MOG_v_FoesAtOnce) == ix_rw(VM, MOG_v_FoesEntered))
            return;
        uint32_t fn = ix_rl(VM, MOG_v_NextFoeFn);
        if (sw(ix_rw(VM, MOG_v_FoesToBeat)) <= 0)
            return;
        if (!mog_next_opponent(m, fn))
            return;
    }
}

/* LAB_000A : gèle (ou dégèle) tout le monde sauf LAB_05F4 ; et LAB_0617. */
static void freeze_others(MogCombat *m)
{
    uint32_t obj = ix_rl(VM, MOG_v_Objects);
    for (int i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (obj != ix_rl(VM, MOG_v_DemonCompanion))
            mog_toggle_freeze(m, obj);
    mog_toggle_freeze(m, MOG_v_DragonObj);
}

/* LAB_0211 : recul (table de pas LAB_08CE, sens LAB_08D0) */
static void knockback(MogCombat *m, uint32_t a1)
{
    ix_ww(VM, MOG_v_BackStepIndex, (uint16_t)(ix_rw(VM, MOG_v_BackStepIndex) + 1));
    ix_wl(VM, MOG_v_CurObj, a1);
    uint32_t a0 = a1;
    ix_ww(VM, a0 + OBJ_INPUT, 0);
    uint8_t dir = ix_rb(VM, MOG_v_BackDir);
    ix_wb(VM, a0 + OBJ_FACING, dir);
    int bit = dir == 1 ? 0 : 1;                         /* LAB_0213 : vers la droite */
    ix_wb(VM, a0 + OBJ_BLOCKED, (uint8_t)(ix_rb(VM, a0 + OBJ_BLOCKED) | (1u << bit)));
    mog_arena_bounds(m, a0);
    if (!(ix_rb(VM, a0 + OBJ_BLOCKED) & (1u << bit)))
        return;
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_v_CurObj));
    uint32_t tab = ix_rl(VM, MOG_v_BackSteps);
    uint16_t k = (uint16_t)(ix_rw(VM, MOG_v_BackStepIndex) << 1);
    uint16_t step = ix_rw(VM, tab + (uint32_t)(int32_t)sw(k));
    uint16_t x = ix_rw(VM, en + ENT_X);
    ix_ww(VM, en + ENT_X, (uint16_t)(bit ? x - step : x + step));
}

/* LAB_02CA : lance un objet (LAB_07EB, contrôleur 52) */
static void throw_object(MogCombat *m, uint32_t en, uint32_t a1)
{
    ix_wb(VM, a1 + OBJ_DAGGERS, (uint8_t)(ix_rb(VM, a1 + OBJ_DAGGERS) - 1));
    uint32_t o = ix_spawn(&m->eng, MOG_x_ThrownObject, ix_rl(VM, en + ENT_BANKS),
                          sw(ix_rw(VM, en + ENT_X)), sw(ix_rw(VM, en + ENT_HEIGHT)),
                          sw(ix_rw(VM, en + ENT_DEPTH)), ix_rb(VM, en + ENT_DIR), 52);
    ix_wl(VM, o + OBJ_DAMAGE, MOG_t_ThrownDamage);
    ix_ww(VM, o + OBJ_ATTACK, 0x0C);
    ix_wb(VM, o + OBJ_CONTROLLER, 0x34);
}

/* LAB_02E8 / LAB_02E9 : sons en séquence (tables de mots terminées par -1) */
static void sound_sequence(MogCombat *m, uint32_t tab)
{
    uint16_t k = (uint16_t)(ix_rw(VM, MOG_v_SoundSeqPos) + 2);
    ix_ww(VM, MOG_v_SoundSeqPos, k);
    if (ix_rw(VM, tab + (uint32_t)(int32_t)sw(k)) == 0xFFFF) {
        ix_ww(VM, MOG_v_SoundSeqPos, 0);
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
    uint32_t a0 = MOG_t_VblTasks;                         /* 1re tâche VBL libre */
    while (ix_rl(VM, a0) && a0 < MOG_t_VblTasks + 0x400)
        a0 += 4;
    ix_wl(VM, MOG_v_ShakePlanes, a0);
    ix_ww(VM, MOG_v_ShakeDelay, 3);
    ix_ww(VM, MOG_v_ShakeCount, 3);
    ix_wl(VM, MOG_v_ShakeTable, MOG_t_ShakeOffsets);
    ix_wl(VM, a0, MOG_Vbl_Shake);
}

/* SECSTRT_16 / LAB_0A9B-LAB_0A9D : son n sur le canal ch (LAB_0F8C) */
void mog_voice(MogCombat *m, int ch, int n)
{
    ix_wb(VM, MOG_v_SndMuted, (uint8_t)(ix_rb(VM, MOG_v_SndMuted) | (1u << ch)));
    mog_snd_play(m, n, ch);                             /* LAB_0F8C */
}

/* LAB_0A9E-LAB_0AA0 : libère le canal ch, son $A7 */
static void voice_off(MogCombat *m, int ch)
{
    ix_wb(VM, MOG_v_SndMuted, (uint8_t)(ix_rb(VM, MOG_v_SndMuted) & (0x0F & ~(1u << ch))));
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
    uint32_t a1 = ix_rl(VM, MOG_v_PalCurrent);
    for (int i = 0; i < 32; i++)
        ix_ww(VM, a1 + 2u * (unsigned)i, c[i]);
}

/* LAB_0EF7 : le Démon projette le chevalier (LAB_05F2) derrière lui */
static void demon_throw(MogCombat *m)
{
    uint32_t k = ix_rl(VM, MOG_v_PlayerObj);
    mog_toggle_freeze(m, k);
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_v_PlayerObj));
    uint32_t a1 = ix_rl(VM, MOG_v_DemonObj);
    uint8_t dir = (uint8_t)(ix_rb(VM, a1 + OBJ_FACING) ^ 2);
    ix_wb(VM, en + ENT_DIR, dir);
    ix_ww(VM, en + ENT_DEPTH, ix_rw(VM, a1 + OBJ_DEPTH));
    uint16_t x = ix_rw(VM, a1 + OBJ_X);
    x = (uint16_t)(dir == 1 ? x - 0x89 : x + 0x89);
    if (!((int16_t)x < 0x140))
        x = 0x13F;
    if ((int16_t)x < 0)
        x = 1;
    ix_ww(VM, en + ENT_X, x);
    a1 = ix_rl(VM, MOG_v_PlayerObj);
    uint32_t script = ix_rl(VM, a1 + OBJ_STAND);
    if (!((int16_t)ix_rw(VM, a1 + OBJ_HP) > 0)) {           /* mort : fondu au noir */
        for (int i = 0; i < 6; i++)
            ix_ww(VM, MOG_t_PalCombat + 4 + 2u * (unsigned)i, 0);
        set_palette(m, MOG_t_PalCombat);
        script = MOG_x_KnightThrown;
    }
    mog_restart_entity(m, ix_rl(VM, MOG_v_PlayerObj), script);
}

/* LAB_0EEB : compagnon du Démon (objet LAB_01A2, script LAB_08AF) */
static void demon_companion(MogCombat *m, uint32_t banks)
{
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_v_DemonObj));
    uint32_t a1 = ix_rl(VM, MOG_v_DemonCompanionObj);
    uint16_t x = ix_rw(VM, en + ENT_X), h = ix_rw(VM, en + ENT_HEIGHT);
    uint8_t dir = ix_rb(VM, en + ENT_DIR);
    ix_ww(VM, a1 + 4, x);
    ix_ww(VM, a1 + 6, h);
    ix_ww(VM, a1 + 8, x);                               /* (X aussi en profondeur) */
    ix_wb(VM, a1 + 10, dir);
    ix_start_entity(&m->eng, MOG_x_DemonCompanion, a1, banks, (int16_t)x, (int16_t)h,
                    (int16_t)x, dir, 8);
}

int mog_native(MogCombat *m, uint32_t routine, uint32_t en)
{
    uint32_t obj = ix_rl(VM, en + ENT_OBJ);
    switch (routine) {
    case MOG_Call_FoeDown: opponent_down(m); return 1;
    case MOG_Call_EndCombat: mog_end_combat(m); return 1;
    case MOG_Call_FreezeOthers: freeze_others(m); return 1;
    case MOG_Call_PlayerDies:                                  /* le joueur meurt */
        ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);
        return 1;
    case MOG_Call_BackStop: ix_ww(VM, MOG_v_BackStepIndex, 0xFFFF); return 1;
    case MOG_Call_BackStep: knockback(m, obj); return 1;
    case MOG_Call_Throw: throw_object(m, en, obj); return 1;
    case MOG_Call_EnemyActed: ix_ww(VM, MOG_v_EnemyActed, 1); return 1;
    case MOG_Call_SoundRnd1E: mog_sound(m, rnd_dec(m, 3) + 0x1E); return 1;
    case MOG_Call_SoundRnd02DE:
        mog_sound(m, rnd_dec(m, 3) + 0x18);
        mog_sound(m, (int)(mog_random(m) & 3) + 0x14);
        return 1;
    case MOG_Call_SoundRnd5B: mog_sound(m, (int)(mog_random(m) & 1) + 0x5B); return 1;
    case MOG_Call_SoundRnd6A: mog_sound(m, (int)(mog_random(m) & 1) + 0x6A); return 1;
    case MOG_Call_SoundRnd61: mog_sound(m, (int)(mog_random(m) & 3) + 0x61); return 1;
    case MOG_Call_SoundRnd65: mog_sound(m, (int)(mog_random(m) & 1) + 0x65); return 1;
    case MOG_Call_SoundRnd67: mog_sound(m, rnd_dec(m, 3) + 0x67); return 1;
    case MOG_Call_Footstep: {                                /* pas */
        uint16_t n = (uint16_t)(ix_rw(VM, MOG_v_NativeCount) + 1);
        if (!(sw(n) < 5))
            n = 0;
        ix_ww(VM, MOG_v_NativeCount, n);
        mog_sound(m, (n + 4) & 0xFF);
        return 1;
    }
    case MOG_Call_Shake: mog_shake(m); return 1;
    case MOG_Call_Countdown: {                                /* compte à rebours 13(objet) */
        int8_t n = (int8_t)(ix_rb(VM, obj + 13) - 1);
        ix_wb(VM, obj + 13, (uint8_t)n);
        if (n < 0) {
            mog_random(m);                              /* (valeur non utilisée) */
            ix_wb(VM, obj + 13, 0x1E);
        }
        return 1;
    }
    case MOG_Call_DragonMove: mog_dragon_move(m); return 1;
    case MOG_Call_Voice1D: mog_voice(m, 0, 0x1D); return 1;
    case MOG_Call_Voice1B: mog_voice(m, 1, 0x1B); return 1;
    case MOG_Call_Sound30_12: mog_sound(m, 0x30); mog_sound(m, 0x12); return 1;
    case MOG_Call_DemonSounds:                                 /* deux sons de LAB_0ECF */
        for (int k = 0; k < 2; k++)
            mog_sound(m, ix_rb(VM, MOG_t_DemonSounds + (mog_random(m) & 7)));
        return 1;
    case MOG_Call_SoundSeqB: sound_sequence(m, MOG_t_SoundSeqB); return 1;
    case MOG_Call_VoiceOff0: voice_off(m, 0); return 1;
    case MOG_Call_VoiceOff1: voice_off(m, 1); return 1;
    case MOG_Call_VoiceOff2: voice_off(m, 2); return 1;
    case MOG_Call_Voice50: mog_voice(m, 0, 0x50); return 1;
    case MOG_Call_MudmenSounds: {                                /* Mudmen : deux sons */
        static const uint8_t pair[4][2] = { { 0x57, 0x94 }, { 0x58, 0x95 },
                                            { 0x59, 0x96 }, { 0x97, 0x98 } };
        int k = (int)(mog_random(m) & 3);
        mog_sound(m, pair[k][0]);
        mog_sound(m, pair[k][1]);
        return 1;
    }
    case MOG_Call_SoundRnd0EBD:
        mog_sound(m, ix_rb(VM, MOG_t_TrollSounds + (mog_random(m) & 3)));
        return 1;
    case MOG_Call_TrollSound: {
        uint16_t n = (uint16_t)((ix_rw(VM, MOG_v_TrollSoundIndex) + 1) & 3);
        ix_ww(VM, MOG_v_TrollSoundIndex, n);
        if (!n)
            mog_sound(m, 0x5A);
        return 1;
    }
    case MOG_Call_DemonCompanion: demon_companion(m, ix_rl(VM, en + ENT_BANKS)); return 1;
    case MOG_Call_DemonCompanion2: {
        uint32_t e2 = ix_find_entity(&m->eng, ix_rl(VM, MOG_v_DemonCompanionObj));
        uint32_t e1 = ix_find_entity(&m->eng, ix_rl(VM, MOG_v_DemonObj));
        ix_wb(VM, e2 + 22, ix_rb(VM, e1 + 22));
        return 1;
    }
    case MOG_Call_DemonCompanionKill: mog_kill_entity_of(m, ix_rl(VM, MOG_v_DemonCompanionObj)); return 1;
    case MOG_Call_Voice4A:
        mog_voice(m, 0, 0x4A);
        mog_voice(m, 1, 0x4B);
        mog_voice(m, 2, 0x4C);
        mog_voice(m, 3, 0x4D);
        return 1;
    case MOG_Call_SoundRnd0EEF: {
        int k = (int)(mog_random(m) & 3);
        int base = k == 1 ? 0x42 : k == 2 ? 0x44 : 0x40;
        mog_sound(m, base);
        mog_sound(m, base + 1);
        return 1;
    }
    case MOG_Call_DemonFx: {
        mog_message(m, "DEMON HIT");
        uint32_t k = ix_rl(VM, MOG_v_PlayerObj);
        ix_ww(VM, k + OBJ_HP, (uint16_t)(ix_rw(VM, k + OBJ_HP) - 10));
        mog_toggle_freeze(m, k);
        return 1;
    }
    case MOG_Call_DemonThrow: demon_throw(m); return 1;
    case MOG_Call_SacrificeSound:                                  /* sacrifice : son au hasard */
        if (!((int16_t)mog_d100(m) > 0x32))
            mog_sound(m, 0x9D);                         /* LAB_0AA2 */
        return 1;
    case MOG_Call_DiceSound:                                  /* dés qui roulent : son */
        /* l'original tire un nombre (perdu : MOVEQ #0,D0) puis joue
         * LAB_04BB[0] sur le canal 3 (LAB_0A9D) */
        mog_random(m);
        mog_voice(m, 3, ix_rb(VM, MOG_t_DiceSounds));
        return 1;
    case MOG_Call_SoundSeqA: sound_sequence(m, MOG_t_SoundSeqA); return 1;
    }
    return 0;
}
