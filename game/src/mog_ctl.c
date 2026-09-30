/*
 * mog_ctl.c — contrôleurs du combat de mog (Combat_RunControllers et
 * contrôleurs de t_Controllers), traduits de amiga_asm/mog.asm.
 *
 * Un contrôleur reçoit l'objet (A0) de l'entité dont le script est fini
 * ou qui a eu un contact ; il range l'objet courant dans LAB_0633 et le
 * script choisi dans v_CtlScript, puis Ctl_Return renvoie A0 = script
 * (-1 : rien, 0 : détruire l'entité) et D0-D3 = X, hauteur, profondeur,
 * direction de l'objet. Les variables globales d'origine sont tenues à
 * jour aux mêmes adresses (tools/mog_difftest.py compare toute la mémoire).
 */
#include "mog_private.h"
#include "ix_mog_names.h"
#include "mog_struct.h"
#include "mog_sound.h"

#include <stdio.h>
#include <string.h>

#define VM (m->eng.vm)


static int16_t sw(uint16_t v) { return (int16_t)v; }

void mog_message(MogCombat *m, const char *t)
{
    if (m->out.message)
        m->out.message(m->out.user, t);
}

void mog_sound(MogCombat *m, int n)
{
    mog_snd_effect(m, n);                               /* LAB_0AA2 */
}



static void bclr(MogCombat *m, uint32_t a, int bit)
{
    ix_wb(VM, a, (uint8_t)(ix_rb(VM, a) & ~(1u << (bit & 7))));
}

static void bset(MogCombat *m, uint32_t a, int bit)
{
    ix_wb(VM, a, (uint8_t)(ix_rb(VM, a) | (1u << (bit & 7))));
}

/* SUB.W #n,80(A1) : points de vie */
static void hurt(MogCombat *m, uint32_t obj, uint16_t n)
{
    ix_ww(VM, obj + OBJ_HP, (uint16_t)(ix_rw(VM, obj + OBJ_HP) - n));
}

static int16_t hp(MogCombat *m, uint32_t obj) { return sw(ix_rw(VM, obj + OBJ_HP)); }

/* Ctl_Return [LAB_02BA] */
CtlResult mog_ctl_return(MogCombat *m)
{
    uint32_t a1 = ix_rl(VM, MOG_v_CurObj);
    CtlResult r;
    r.script = ix_rl(VM, MOG_v_CtlScript);
    r.x = ix_rw(VM, a1 + 4);
    r.h = ix_rw(VM, a1 + 6);
    r.d = ix_rw(VM, a1 + 8);
    r.dir = ix_rb(VM, a1 + 10);
    return r;
}

CtlResult mog_set_script(MogCombat *m, uint32_t script)
{
    ix_wl(VM, MOG_v_CtlScript, script);
    return mog_ctl_return(m);
}

/* Col_SpanOverlap [LAB_03CA] : 1 si [d0,d1] et [d2,d3] se chevauchent
 * (comparaisons 32 bits : BMI = bit de signe de la différence). */
int mog_span(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
    if ((d2 - d0) & 0x80000000u)                    /* LAB_03CC */
        return (int32_t)d3 >= (int32_t)d0;
    if ((d2 - d1) & 0x80000000u)
        return 1;
    if ((d3 - d0) & 0x80000000u)
        return 0;
    return !((int32_t)d3 >= (int32_t)d1);
}

/* LAB_030D : relance l'entité de l'objet `obj` sur `script`. */
void mog_restart_entity(MogCombat *m, uint32_t obj, uint32_t script)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (!en)
        return;
    uint32_t c = ix_rl(VM, en + ENT_CTX);
    for (int i = 0; i < 36; i++)
        ix_wb(VM, c + (uint32_t)i, 0);
    ix_wl(VM, en + ENT_PC, script);
    ix_wb(VM, en + ENT_BUSY, 1);
}

/* ------------------------------------------------------------------ */
/* Déplacement : obstacles, bords, décor                               */
/* ------------------------------------------------------------------ */

/* LAB_03B3 : profondeurs à 10 près ou moins */
static int near_depth(MogCombat *m, uint32_t a0, uint32_t a1)
{
    int16_t d = (int16_t)(ix_rw(VM, a0 + 8) - ix_rw(VM, a1 + 8));
    if (d < 0)
        d = (int16_t)-d;
    return !(d > 10);
}

/* LAB_03AC : a0 (qui bouge) contre a1 */
static void block_by(MogCombat *m, uint32_t a0, uint32_t a1)
{
    const uint32_t bits = MOG_t_BlockBits;
    int d6, d5;
    uint16_t dx = ix_rw(VM, MOG_v_WalkDx);

    if (near_depth(m, a0, a1)) {
        int go = 1;
        d6 = 0;
        if (ix_rw(VM, MOG_v_WalkDir) != 1) {
            d6 = 1;
            if (sw(ix_rw(VM, a0 + OBJ_X)) < sw(ix_rw(VM, a1 + OBJ_X)))
                go = 0;
        } else if (sw(ix_rw(VM, a0 + OBJ_X)) > sw(ix_rw(VM, a1 + OBJ_X))) {
            go = 0;
        }
        if (go) {                                       /* LAB_03AE */
            d5 = mog_span((uint16_t)(ix_rw(VM, a0 + OBJ_BOX_X0) + dx),
                              (uint16_t)(ix_rw(VM, a0 + OBJ_BOX_X1) + dx),
                              ix_rw(VM, a1 + OBJ_BOX_X0), ix_rw(VM, a1 + OBJ_BOX_X1));
            d5 += mog_span(ix_rw(VM, a0 + OBJ_BOX_Y0), ix_rw(VM, a0 + OBJ_BOX_Y1),
                               ix_rw(VM, a1 + OBJ_BOX_Y0), ix_rw(VM, a1 + OBJ_BOX_Y1));
            if (d5 == 2)
                bclr(m, bits, d6);
        }
    }
    /* LAB_03AF */
    d5 = mog_span(ix_rw(VM, a0 + OBJ_BOX_X0), ix_rw(VM, a0 + OBJ_BOX_X1),
                      ix_rw(VM, a1 + OBJ_BOX_X0), ix_rw(VM, a1 + OBJ_BOX_X1));
    int16_t d2 = (int16_t)(ix_rw(VM, a0 + OBJ_DEPTH) - ix_rw(VM, a1 + OBJ_DEPTH));
    if (d2 < 0) {
        d6 = 2;
        d2 = (int16_t)-d2;
    } else {
        d6 = 3;
    }
    if (!(d2 > 20)) {
        d5 += mog_span(ix_rw(VM, a0 + OBJ_BOX_Y0), ix_rw(VM, a0 + OBJ_BOX_Y1),
                           ix_rw(VM, a1 + OBJ_BOX_Y0), ix_rw(VM, a1 + OBJ_BOX_Y1));
        if (d5 == 2)
            bclr(m, bits, d6);
    }
}

/* LAB_03A9 : directions bloquées par les autres combattants ; renvoie le
 * masque des directions permises (bits 0-3 de L00_08671). */
uint16_t mog_blocked_dirs(MogCombat *m, uint32_t a0, uint16_t dx, uint16_t dir)
{
    ix_wl(VM, MOG_v_WalkObj, a0);
    ix_ww(VM, MOG_v_WalkDx, dx);
    ix_ww(VM, MOG_v_WalkDir, (uint16_t)(dir & 3));
    ix_ww(VM, MOG_v_AllowedDirs, 0x001F);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, en))
            continue;
        uint32_t a1 = ix_rl(VM, en + ENT_OBJ);
        if (a1 == a0)
            continue;
        if (ix_rw(VM, a1 + OBJ_BOX_X0) == 0 || hp(m, a1) <= 0)
            continue;
        block_by(m, a0, a1);
    }
    return ix_rw(VM, MOG_v_AllowedDirs);
}

/* LAB_0215 : bords de l'arène */
void mog_arena_bounds(MogCombat *m, uint32_t a0)
{
    int16_t d0 = (ix_rb(VM, a0 + OBJ_FACING) & 2) ? -25 : 25;
    d0 = (int16_t)(d0 + ix_rw(VM, a0 + OBJ_X));
    int16_t d1 = (int16_t)(9 + ix_rw(VM, a0 + OBJ_DEPTH));
    if (d0 < 10)
        bclr(m, a0 + OBJ_BLOCKED, 1);
    if (d0 > 320)
        bclr(m, a0 + OBJ_BLOCKED, 0);
    if (d1 > 155)
        bclr(m, a0 + OBJ_BLOCKED, 2);
    if (d1 < 30)
        bclr(m, a0 + OBJ_BLOCKED, 3);
}

/* LAB_0A71 : obstacles du décor (table SECSTRT_14, 8 octets par obstacle) */
static void terrain_obstacles(MogCombat *m, uint32_t a0, uint16_t dx, uint16_t dy)
{
    uint16_t limit = (uint16_t)(dy + ix_rw(VM, a0 + OBJ_DEPTH));
    ix_ww(VM, MOG_v_ObstacleLimit, limit);
    ix_ww(VM, MOG_v_ObstacleLimit, (uint16_t)(ix_rw(VM, MOG_v_ObstacleLimit) + 0x2F));
    uint16_t x0 = (uint16_t)(ix_rw(VM, a0 + OBJ_BOX_X0) + dx);
    uint16_t x1 = (uint16_t)(ix_rw(VM, a0 + OBJ_BOX_X1) + dx);
    ix_ww(VM, MOG_v_ObstacleX0, x0);
    ix_ww(VM, MOG_v_ObstacleX1, x1);

    uint32_t a1 = ix_rl(VM, MOG_b_Obstacles);
    uint32_t n = (uint32_t)(uint16_t)(ix_rw(VM, a1) - 1) + 1;   /* DBF */
    a1 += 2;
    for (uint32_t i = 0; i < n; i++, a1 += 8) {
        if (!mog_span(ix_rw(VM, a1), ix_rw(VM, a1 + 2),
                          ix_rw(VM, MOG_v_ObstacleX0), ix_rw(VM, MOG_v_ObstacleX1)))
            continue;
        uint16_t y = ix_rw(VM, a1 + 4);
        uint16_t by = ix_rw(VM, a0 + OBJ_BOX_Y1);
        if (mog_span(0x1E, y, by, (uint16_t)(by + 1))) {
            int16_t lx0 = sw(ix_rw(VM, MOG_v_ObstacleX0));
            int16_t lx1 = sw(ix_rw(VM, MOG_v_ObstacleX1));
            if (ix_rb(VM, a0 + OBJ_FACING) & 2) {
                if (!(lx1 < sw(ix_rw(VM, a0 + OBJ_BOX_X0))))
                    bclr(m, a0 + OBJ_BLOCKED, 1);
            } else if (!(lx0 > sw(ix_rw(VM, a0 + OBJ_BOX_X1)))) {
                bclr(m, a0 + OBJ_BLOCKED, 0);
            }
        }
        if (!(sw(y) < sw(ix_rw(VM, MOG_v_ObstacleLimit))))    /* LAB_0A74 */
            bclr(m, a0 + OBJ_BLOCKED, 3);
    }
}

/* ------------------------------------------------------------------ */
/* Réactions du chevalier humain                                       */
/* ------------------------------------------------------------------ */

/* LAB_020E : script de réaction 30(objet)[attaque adverse] */
CtlResult mog_react_script(MogCombat *m, uint32_t a0, uint32_t a1)
{
    int16_t k = sw(ix_rw(VM, a0 + OBJ_ATTACK));
    return mog_set_script(m, ix_rl(VM, ix_rl(VM, a1 + 30) + (uint32_t)(int32_t)k));
}

/* Réponse par l'attaque en cours 34(objet)[64(objet)] (parade réussie). */
CtlResult mog_own_attack(MogCombat *m)
{
    uint32_t a1 = ix_rl(VM, MOG_v_CurObj);
    int16_t k = sw(ix_rw(VM, a1 + OBJ_ATTACK));
    return mog_set_script(m, ix_rl(VM, ix_rl(VM, a1 + OBJ_ATTACKS) + (uint32_t)(int32_t)k));
}

/* LAB_01E6 : parade ? (LAB_01EB = 1) — 50(objet)[attaque adverse] doit
 * valoir l'attaque en cours. */
void mog_check_parry(MogCombat *m, uint32_t a1)
{
    ix_ww(VM, MOG_v_Parried, 0);
    uint32_t a0 = ix_rl(VM, a1 + OBJ_HIT_BY);
    uint32_t d0 = ix_rl(VM, ix_rl(VM, a1 + OBJ_PARRY) + (uint32_t)(int32_t)sw(ix_rw(VM, a0 + OBJ_ATTACK)));
    uint16_t d1 = ix_rw(VM, a1 + OBJ_ATTACK);
    if ((uint16_t)d0 != d1)
        return;
    ix_ww(VM, MOG_v_Parried, 1);
    if (d1 == 0x1C) {                                   /* LAB_01E9 */
        ix_ww(VM, MOG_v_Parried, 0);
        if (!(ix_rb(VM, a1 + OBJ_AI_FLAGS) & 0x80)) {
            ix_ww(VM, MOG_v_Parried, 1);
            bset(m, a1 + OBJ_AI_FLAGS, 7);
        }
        return;
    }
    if (ix_rb(VM, a1 + OBJ_FACING) == ix_rb(VM, a0 + OBJ_FACING)) {
        ix_ww(VM, MOG_v_Parried, 0);
        return;
    }
    mog_sound(m, 0x11);                                     /* LAB_01E7 */
}

/* LAB_0204 : D0 >> 8(96(objet)) */
static uint16_t scale_down(MogCombat *m, uint32_t a1, uint16_t d0)
{
    unsigned s = ix_rb(VM, ix_rl(VM, a1 + OBJ_INVENTORY) + INV_TALISMANS) & 63u;
    return s >= 16 ? 0 : (uint16_t)(d0 >> s);
}

/* LAB_021B : dégâts infligés par a0 (« KNIGHT DAMAGE ») */
uint16_t mog_knight_damage(MogCombat *m, uint32_t a0)
{
    uint16_t k = ix_rw(VM, a0 + OBJ_ATTACK);
    uint16_t d0 = (uint16_t)ix_rl(VM, ix_rl(VM, a0 + OBJ_DAMAGE) + (uint32_t)(int32_t)sw(k));
    d0 = (uint16_t)(d0 + ix_rb(VM, a0 + OBJ_STRENGTH));
    uint32_t w = ix_rl(VM, a0 + OBJ_WEAPON);
    if (w == 0x17) d0 = (uint16_t)(d0 + 2);
    if (w == 0x18) d0 = (uint16_t)(d0 + 3);
    if (w == 0x19) d0 = (uint16_t)(d0 + 5);
    if (k == 0x20)
        d0 = (uint16_t)(d0 << 1);
    uint16_t d2 = ix_rw(VM, MOG_v_Combatants + CMB_MOON);
    uint8_t d1 = ix_rb(VM, ix_rl(VM, a0 + OBJ_INVENTORY) + INV_MOONSTONES);
    if (d1) {
        int dbl = 0;
        if ((d1 & 1) && d2 == 0x2E) dbl = 1;
        else if ((d1 & 8) && d2 == 0x2E) dbl = 1;
        else if ((d1 & 2) && d2 == 0x2D) dbl = 1;
        else if ((d1 & 4) && d2 == 0x31) dbl = 1;
        if (dbl)
            d0 = (uint16_t)(d0 << 1);
    }
    mog_message(m, "KNIGHT DAMAGE:");
    return d0;
}

/* LAB_020B */
static CtlResult r_020B(MogCombat *m, uint32_t a0, uint32_t a1)
{
    if (hp(m, a1) > 0) {                                /* LAB_020D */
        hurt(m, a1, mog_knight_damage(m, a0));
        return mog_react_script(m, a0, a1);
    }
    ix_wl(VM, MOG_v_CtlScript, MOG_x_KnightDie);
    if (ix_rw(VM, a0 + OBJ_ATTACK) == 8)
        ix_wl(VM, MOG_v_CtlScript, MOG_x_KnightDieHead);
    return mog_ctl_return(m);
}

static CtlResult r_0201(MogCombat *m, uint32_t a0, uint32_t a1, uint16_t d0)
{
    hurt(m, a1, scale_down(m, a1, d0));                 /* LAB_0202 */
    return mog_react_script(m, a0, a1);
}

/* Réaction au coup reçu, selon le contrôleur de l'attaquant (LAB_0621). */
static CtlResult react_hit_by(MogCombat *m, uint32_t fn, uint32_t a0, uint32_t a1,
                              uint16_t idx, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_React_Hit5:
        hurt(m, a1, 5);
        if (!ix_rl(VM, a1 + OBJ_HIT) && hp(m, a1) <= 0 && !ix_rl(VM, MOG_v_Gore)) {
            uint32_t att = ix_rl(VM, a1 + OBJ_HIT_BY);
            uint32_t s = ix_rb(VM, a1 + OBJ_FACING) != ix_rb(VM, att + 10) ? MOG_x_PassingKnightKill : MOG_x_PassingKnightKillBack;
            mog_restart_entity(m, att, s);
            return mog_set_script(m, 0);
        } else {                                        /* LAB_0209 */
            uint32_t att = ix_rl(VM, a1 + OBJ_HIT_BY);
            return mog_set_script(m, ix_rb(VM, a1 + OBJ_FACING) == ix_rb(VM, att + 10)
                                 ? MOG_x_PassingKnightHitFront : MOG_x_PassingKnightHitBack);
        }
    case MOG_React_Default:
        return r_020B(m, a0, a1);
    case MOG_React_Hit20: {
        uint16_t k = ix_rw(VM, a0 + OBJ_ATTACK);
        if (k == 0x20 || k == 4) {
            hurt(m, a1, k == 0x20 ? 10 : 8);
            ix_wb(VM, a1 + OBJ_FACING, (uint8_t)(ix_rb(VM, a0 + OBJ_FACING) ^ 2));   /* LAB_01FC */
        } else {
            hurt(m, a1, 10);
        }
        return mog_react_script(m, a0, a1);
    }
    case MOG_React_Hit7:
        hurt(m, a1, 7);
        if (ix_rw(VM, a0 + OBJ_ATTACK) == 0x20) {
            a1 = ix_rl(VM, MOG_v_CurObj);
            if (hp(m, a1) <= 0)
                return mog_set_script(m, MOG_x_KnightCrushed);
        }
        return mog_react_script(m, a0, a1);
    case MOG_React_Pushed:
        ix_ww(VM, a1 + OBJ_DEPTH, (uint16_t)(ix_rw(VM, a0 + OBJ_DEPTH) - 1));
        if (ix_rw(VM, a0 + OBJ_ATTACK) == 4)
            return r_0201(m, a0, a1, 0x14);
        ix_ww(VM, a0 + OBJ_ATTACK, 0x20);                       /* LAB_0201 */
        return r_0201(m, a0, a1, 0x1E);
    case MOG_React_Hit30:
        ix_ww(VM, a0 + OBJ_ATTACK, 0x20);
        return r_0201(m, a0, a1, 0x1E);
    case MOG_React_Hit10:
        hurt(m, a1, scale_down(m, a1, (uint16_t)(idx - 10)));
        ix_wb(VM, MOG_v_BackDir, 1);
        ix_wl(VM, MOG_v_BackSteps, MOG_t_BackStepsA);
        ix_wl(VM, MOG_v_CtlScript, MOG_x_KnightDropped);
        ix_wb(VM, a1 + OBJ_FACING, 3);
        return mog_ctl_return(m);
    case MOG_React_HitOrDie:
        if (hp(m, a1) <= 0)
            return mog_set_script(m, MOG_x_KnightDieHead);         /* LAB_01F5 */
        mog_check_parry(m, a1);
        if (!ix_rw(VM, MOG_v_Parried))
            return r_020B(m, a0, a1);
        return mog_own_attack(m);
    case MOG_React_Hit8:
        if (ix_rw(VM, a0 + OBJ_ATTACK) == 8)
            ix_wb(VM, a1 + OBJ_FACING, (uint8_t)(ix_rb(VM, a0 + OBJ_FACING) ^ 2));
        hurt(m, a1, 5);
        return mog_react_script(m, a0, a1);
    case MOG_React_Hit4Or8: {
        uint16_t k = ix_rw(VM, a0 + OBJ_ATTACK);
        if (k == 4 || k == 8) {
            uint32_t dmg = ix_rl(VM, ix_rl(VM, a0 + OBJ_DAMAGE) + k);
            hurt(m, a1, (uint16_t)dmg);
            if (k == 4) {
                ix_wl(VM, MOG_v_CtlScript, MOG_x_RatHitByKnight4);
                ix_wb(VM, a1 + OBJ_POISONED, 1);
                return mog_ctl_return(m);
            }
            return mog_set_script(m, MOG_x_RatHitByKnight8);
        }
        return r_020B(m, a0, a1);
    }
    case MOG_React_KnightHitBy:
        if (hp(m, a1) <= 0)                             /* LAB_01F4 */
            return mog_set_script(m, ix_rb(VM, a0 + OBJ_CONTROLLER) == 0x18 ? MOG_x_KnightDieHead : MOG_x_KnightDie);
        mog_check_parry(m, a1);
        if (ix_rw(VM, MOG_v_Parried))
            return mog_own_attack(m);
        a0 = ix_rl(VM, a1 + OBJ_HIT_BY);                        /* LAB_01F3 */
        hurt(m, a1, (uint16_t)ix_rl(VM, ix_rl(VM, a0 + OBJ_DAMAGE)
                                     + (uint32_t)(int32_t)sw(ix_rw(VM, a0 + OBJ_ATTACK))));
        return mog_react_script(m, a0, a1);
    case MOG_React_Parry:
        mog_check_parry(m, a1);
        if (ix_rw(VM, MOG_v_Parried))
            return mog_set_script(m, MOG_x_KnightAtk7);
        hurt(m, a1, 3);
        if (!ix_rl(VM, a1 + OBJ_HIT) && hp(m, a1) <= 0 && !ix_rl(VM, MOG_v_Gore)) {
            mog_restart_entity(m, ix_rl(VM, a1 + OBJ_HIT_BY), MOG_x_TroggKill);
            return mog_set_script(m, 0);
        }
        return mog_react_script(m, a0, a1);
    }
    *ok = 0;
    return mog_ctl_return(m);
}

/* Réaction après avoir touché, selon le contrôleur de la cible (LAB_0622). */
static CtlResult react_hit(MogCombat *m, uint32_t fn, uint32_t a0, uint32_t a1, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_React_KnightHit: {
        uint8_t c = ix_rb(VM, a0 + OBJ_CONTROLLER);
        if (c == 0x0C || c == 0x10) {                   /* LAB_01E4 */
            if (hp(m, a0) > 0) {
                if (ix_rw(VM, a0 + OBJ_ATTACK) == 0x1C)         /* LAB_01E5 */
                    return mog_set_script(m, 0xFFFFFFFFu);
            } else if (ix_rw(VM, a1 + OBJ_ATTACK) == 8 && !ix_rl(VM, MOG_v_Gore)) {
                return mog_set_script(m, 0xFFFFFFFFu);
            }
        }
        if (ix_rw(VM, a0 + OBJ_ATTACK) == 0x18)                 /* LAB_01E2 */
            return mog_set_script(m, 0xFFFFFFFFu);
        return mog_set_script(m, ix_rl(VM, a1 + OBJ_RECOIL));       /* LAB_01E3 */
    }
    case MOG_React_Hit30:
        ix_ww(VM, a0 + OBJ_ATTACK, 0x20);
        return r_0201(m, a0, a1, 0x1E);
    }
    *ok = 0;
    return mog_ctl_return(m);
}

/* ------------------------------------------------------------------ */
/* Ctl_HumanKnight [LAB_01CA]                                          */
/* ------------------------------------------------------------------ */

/* LAB_00EA : joystick du chevalier (11(objet) = 1 : port 0, sinon port 1) */
static uint16_t read_joystick(MogCombat *m, uint32_t obj)
{
    ix_ww(VM, MOG_v_Joy0, m->joy[0]);                 /* LAB_00EE */
    ix_ww(VM, MOG_v_Joy1, m->joy[1]);
    return ix_rb(VM, obj + 11) == 1 ? m->joy[0] : m->joy[1];
}

/* Ctl_HumanAttack [LAB_01DD] */
static CtlResult human_attack(MogCombat *m, uint32_t a1)
{
    uint16_t d0 = (uint16_t)((ix_rw(VM, a1 + OBJ_INPUT) & 0xFFEF) << 1);
    uint32_t tab = ix_rb(VM, a1 + OBJ_FACING) == 3 ? MOG_t_AttackStickL : MOG_t_AttackStickR;
    d0 = ix_rw(VM, tab + (uint32_t)(int32_t)sw(d0));
    ix_ww(VM, a1 + OBJ_ATTACK, d0);
    return mog_set_script(m, ix_rl(VM, ix_rl(VM, a1 + OBJ_ATTACKS) + (uint32_t)(int32_t)sw(d0)));
}

static CtlResult human_knight(MogCombat *m, uint32_t a0)
{
    uint32_t a1 = a0;
    ix_wl(VM, MOG_v_CurObj, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + OBJ_STAND));

    if (ix_rl(VM, a0 + OBJ_HIT_BY)) {                           /* LAB_01EC */
        uint32_t att = ix_rl(VM, a1 + OBJ_HIT_BY);
        uint16_t idx = ix_rb(VM, att + OBJ_CONTROLLER);
        uint32_t fn = ix_rl(VM, MOG_t_HitByFn + idx);
        int ok;
        CtlResult r = react_hit_by(m, fn, att, a1, idx, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "réaction (touché) non portée : %08X", fn);
            mog_message(m, t);
            m->errors++;
        }
        return r;
    }
    if (ix_rl(VM, a0 + OBJ_HIT)) {                           /* LAB_01E0 */
        uint32_t tgt = ix_rl(VM, a1 + OBJ_HIT);
        uint32_t fn = ix_rl(VM, MOG_t_HitFn + ix_rb(VM, tgt + OBJ_CONTROLLER));
        int ok;
        CtlResult r = react_hit(m, fn, tgt, a1, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "réaction (a touché) non portée : %08X", fn);
            mog_message(m, t);
            m->errors++;
        }
        return r;
    }

    ix_ww(VM, a1 + OBJ_ATTACK, 0);
    uint16_t d0 = read_joystick(m, a0);
    ix_ww(VM, a1 + OBJ_INPUT, d0);
    if (!d0)
        return mog_ctl_return(m);
    if (a0 == ix_rl(VM, MOG_v_ReversedKnight) && ix_rw(VM, MOG_v_ReversedOn)) {   /* commandes inversées */
        if (d0 & 0x0C)
            d0 ^= 0x0C;
        if (d0 & 0x03)
            d0 ^= 0x03;
        ix_ww(VM, a1 + OBJ_INPUT, d0);
    }
    if (ix_rb(VM, a1 + OBJ_BLOCKED) & 0x10)
        return human_attack(m, a1);

    ix_ww(VM, MOG_v_StepX, 0);
    ix_ww(VM, MOG_v_StepY, 0);
    ix_wb(VM, MOG_v_WalkPhase, ix_rb(VM, a1 + OBJ_WALK_PHASE));
    uint8_t phase = (uint8_t)((ix_rb(VM, a1 + OBJ_WALK_PHASE) + 1) & 3);
    ix_wb(VM, a1 + OBJ_WALK_PHASE, phase);
    uint8_t st = ix_rb(VM, a1 + OBJ_BLOCKED);
    if (st & 8)                                         /* LAB_01DB */
        ix_ww(VM, MOG_v_StepY, (uint16_t)-ix_rw(VM, MOG_t_WalkStepUp + 2u * phase));
    else if (st & 4)                                    /* LAB_01DC */
        ix_ww(VM, MOG_v_StepY, ix_rw(VM, MOG_t_WalkStepDown + 2u * phase));
    if (st & 3) {                                       /* LAB_01D9 */
        ix_wb(VM, a1 + OBJ_FACING, (st & 1) ? 1 : 3);
        uint16_t dx = ix_rw(VM, MOG_t_WalkStepX + 2u * phase);
        if (ix_rb(VM, a1 + OBJ_FACING) & 2)
            dx = (uint16_t)-dx;
        ix_ww(VM, MOG_v_StepX, dx);
    }

    /* LAB_01D2 */
    a0 = ix_rl(VM, MOG_v_CurObj);
    uint16_t allowed = mog_blocked_dirs(m, a0, ix_rw(VM, MOG_v_StepX), ix_rb(VM, a0 + OBJ_FACING));
    ix_ww(VM, a0 + OBJ_INPUT, (uint16_t)(ix_rw(VM, a0 + OBJ_INPUT) & allowed));
    mog_arena_bounds(m, a0);
    terrain_obstacles(m, a0, ix_rw(VM, MOG_v_StepX), ix_rw(VM, MOG_v_StepY));

    uint16_t d1 = ix_rw(VM, a0 + OBJ_INPUT);
    uint16_t dx = ix_rw(VM, MOG_v_StepX), dy = ix_rw(VM, MOG_v_StepY);
    int moved = 0;
    uint16_t group = 0;
    if (d1 & 8) { ix_ww(VM, a0 + OBJ_DEPTH, (uint16_t)(ix_rw(VM, a0 + OBJ_DEPTH) + dy)); moved = 1; group = 0x20; }
    if (d1 & 4) { ix_ww(VM, a0 + OBJ_DEPTH, (uint16_t)(ix_rw(VM, a0 + OBJ_DEPTH) + dy)); moved = 1; group = 0x40; }
    if (d1 & 2) { ix_ww(VM, a0 + OBJ_X, (uint16_t)(ix_rw(VM, a0 + OBJ_X) + dx)); moved = 1; group = 0; }
    if (d1 & 1) { ix_ww(VM, a0 + OBJ_X, (uint16_t)(ix_rw(VM, a0 + OBJ_X) + dx)); moved = 1; group = 0; }
    if (!moved) {
        ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + OBJ_STAND));
        ix_wb(VM, a0 + OBJ_WALK_PHASE, ix_rb(VM, MOG_v_WalkPhase));
        return mog_ctl_return(m);
    }
    a1 = ix_rl(VM, MOG_v_CurObj);                       /* LAB_01D7 */
    bclr(m, a1 + OBJ_AI_FLAGS, 7);
    uint16_t k = (uint16_t)(group + (ix_rb(VM, a1 + OBJ_WALK_PHASE) << 2));
    return mog_set_script(m, ix_rl(VM, ix_rl(VM, a1 + OBJ_WALK) + (uint32_t)(int32_t)sw(k)));
}

/* ------------------------------------------------------------------ */

static void fwd_draw(void *u, uint32_t cel, int frame, int x, int y, int flipped, int bg)
{
    MogCombat *m = u;
    if (m->planes) {                                    /* LAB_0339 / LAB_033E */
        if (bg) {
            for (uint32_t i = 0, d0 = ix_rl(VM, MOG_v_BgPlanes); i < 5; i++, d0 += 0x1F40)
                ix_wl(VM, MOG_t_DestPlanes + 4 * i, d0);
            mog_draw_cel(VM, &m->blt, cel, (uint16_t)frame, (uint16_t)x, (uint16_t)y);
            for (uint32_t i = 0, d0 = ix_rl(VM, MOG_v_DrawPlanes); i < 5; i++, d0 += 0x1F40)
                ix_wl(VM, MOG_t_DestPlanes + 4 * i, d0);
        } else {
            mog_draw_cel(VM, &m->blt, cel, (uint16_t)frame, (uint16_t)x, (uint16_t)y);
        }
        return;
    }
    if (m->out.draw)
        m->out.draw(m->out.user, cel, frame, x, y, flipped, bg);
}

static void fwd_sound(void *u, int n)
{
    mog_snd_effect(u, n);                               /* $A4 : LAB_0AA2 */
}

static void fwd_message(void *u, const char *t)
{
    MogCombat *m = u;
    if (m->out.message)
        m->out.message(m->out.user, t);
}

static void native_call(void *u, uint32_t routine, uint32_t en)
{
    MogCombat *m = u;
    if (!mog_native(m, routine, en)) {
        char t[64];
        snprintf(t, sizeof t, "routine $B0 non portée : %08X", routine);
        fwd_message(m, t);
        m->errors++;
    }
}

void mog_combat_init(MogCombat *m, IxVM *vm, const IxHost *host)
{
    IxLayout lay;
    memset(m, 0, sizeof *m);
    if (host)
        m->out = *host;
    m->eng_host.user = m;
    m->eng_host.frame_info = NULL;                      /* CEL en mémoire */
    m->eng_host.draw = fwd_draw;
    m->eng_host.sound = fwd_sound;
    m->eng_host.call = native_call;
    m->eng_host.message = fwd_message;
    ix_layout_mog(&lay);
    ix_engine_init(&m->eng, vm, &m->eng_host, &lay);
}

/* LAB_02CB (contrôleur 52) : objet projeté (LAB_07EC), détruit au contact
 * ou une fois sorti de l'écran. */
static CtlResult projectile(MogCombat *m, uint32_t a0)
{
    ix_wl(VM, MOG_v_CurObj, a0);
    ix_wl(VM, MOG_v_CtlScript, MOG_x_ProjectileFly);
    int16_t x = sw(ix_rw(VM, a0 + 4));
    int gone;
    if (ix_rl(VM, a0 + 14))
        gone = 1;
    else if (ix_rb(VM, a0 + 10) == 1)
        gone = x >= 0x14A;
    else
        gone = x <= 0 && (int16_t)-x <= 10;             /* LAB_02CC */
    return gone ? mog_set_script(m, 0) : mog_ctl_return(m);     /* LAB_02D1 */
}

/* LAB_02D2 (contrôleur 40) : objet inerte, contacts effacés. */
static CtlResult inert(MogCombat *m, uint32_t a0)
{
    ix_wl(VM, MOG_v_CurObj, a0);
    ix_wl(VM, a0 + 18, 0);
    ix_wl(VM, a0 + 14, 0);
    return mog_set_script(m, 0xFFFFFFFFu);
}

static CtlResult run_controller(MogCombat *m, uint32_t fn, uint32_t obj, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_Ctl_HumanKnight:
        return human_knight(m, obj);
    case MOG_Ctl_Projectile:
        return projectile(m, obj);
    case MOG_Ctl_Inert:
        return inert(m, obj);
    }
    CtlResult r;
    if (mog_ai_controller(m, fn, obj, &r))
        return r;
    *ok = 0;
    r.script = 0xFFFFFFFFu;
    return r;
}

/* Combat_RunControllers [LAB_0322] */
void mog_run_controllers(MogCombat *m)
{
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, en))
            continue;
        uint32_t obj = ix_rl(VM, en + ENT_OBJ);
        if (!ix_rl(VM, obj + 14) && !ix_rl(VM, obj + 18) && ix_rb(VM, en + ENT_BUSY))
            continue;
        uint32_t fn = ix_rl(VM, MOG_t_Controllers + ix_rb(VM, en + ENT_CTL));
        int ok;
        CtlResult r = run_controller(m, fn, obj, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "contrôleur non porté : %08X", fn);
            mog_message(m, t);
            m->errors++;
            continue;
        }
        if (r.script == 0xFFFFFFFFu)
            continue;
        if (r.script == 0) {                            /* LAB_0325 */
            ix_ww(VM, en, 0);
            ix_wl(VM, ix_rl(VM, en + ENT_OBJ), 0);
            mog_message(m, "TASK & TABLE OFF");
            continue;
        }
        ix_wl(VM, en + ENT_PC, r.script);
        ix_ww(VM, en + ENT_X, r.x);
        ix_ww(VM, en + ENT_HEIGHT, r.h);
        ix_ww(VM, en + ENT_DEPTH, r.d);
        ix_wb(VM, en + ENT_DIR, r.dir);
        ix_wb(VM, en + ENT_BUSY, 1);
    }
}
