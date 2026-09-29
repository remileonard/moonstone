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
#include "mog_combat.h"
#include "ix_mog_syms.h"

#include <stdio.h>
#include <string.h>

#define VM (m->eng.vm)

typedef struct {
    uint32_t script;     /* A0 */
    uint16_t x, h, d;    /* D0-D2 */
    uint8_t  dir;        /* D3 */
} CtlResult;

static int16_t sw(uint16_t v) { return (int16_t)v; }

static void msg(MogCombat *m, const char *t)
{
    const IxHost *h = m->eng.host;
    if (h && h->message)
        h->message(h->user, t);
}

static void sound(MogCombat *m, int n)
{
    const IxHost *h = m->eng.host;
    if (h && h->sound)
        h->sound(h->user, n);
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
    ix_ww(VM, obj + 80, (uint16_t)(ix_rw(VM, obj + 80) - n));
}

static int16_t hp(MogCombat *m, uint32_t obj) { return sw(ix_rw(VM, obj + 80)); }

/* Ctl_Return [LAB_02BA] */
static CtlResult ctl_return(MogCombat *m)
{
    uint32_t a1 = ix_rl(VM, MOG_LAB_0633);
    CtlResult r;
    r.script = ix_rl(VM, MOG_v_CtlScript);
    r.x = ix_rw(VM, a1 + 4);
    r.h = ix_rw(VM, a1 + 6);
    r.d = ix_rw(VM, a1 + 8);
    r.dir = ix_rb(VM, a1 + 10);
    return r;
}

static CtlResult set_script(MogCombat *m, uint32_t script)
{
    ix_wl(VM, MOG_v_CtlScript, script);
    return ctl_return(m);
}

/* Col_SpanOverlap [LAB_03CA] : 1 si [d0,d1] et [d2,d3] se chevauchent
 * (comparaisons 32 bits : BMI = bit de signe de la différence). */
static int span_overlap(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
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
static void restart_entity(MogCombat *m, uint32_t obj, uint32_t script)
{
    uint32_t en = ix_find_entity(&m->eng, obj);
    if (!en)
        return;
    uint32_t c = ix_rl(VM, en + 36);
    for (int i = 0; i < 36; i++)
        ix_wb(VM, c + (uint32_t)i, 0);
    ix_wl(VM, en + 2, script);
    ix_wb(VM, en + 1, 1);
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
    const uint32_t bits = MOG_L00_08671;
    int d6, d5;
    uint16_t dx = ix_rw(VM, MOG_LAB_0635);

    if (near_depth(m, a0, a1)) {
        int go = 1;
        d6 = 0;
        if (ix_rw(VM, MOG_LAB_0636) != 1) {
            d6 = 1;
            if (sw(ix_rw(VM, a0 + 4)) < sw(ix_rw(VM, a1 + 4)))
                go = 0;
        } else if (sw(ix_rw(VM, a0 + 4)) > sw(ix_rw(VM, a1 + 4))) {
            go = 0;
        }
        if (go) {                                       /* LAB_03AE */
            d5 = span_overlap((uint16_t)(ix_rw(VM, a0 + 58) + dx),
                              (uint16_t)(ix_rw(VM, a0 + 60) + dx),
                              ix_rw(VM, a1 + 58), ix_rw(VM, a1 + 60));
            d5 += span_overlap(ix_rw(VM, a0 + 112), ix_rw(VM, a0 + 114),
                               ix_rw(VM, a1 + 112), ix_rw(VM, a1 + 114));
            if (d5 == 2)
                bclr(m, bits, d6);
        }
    }
    /* LAB_03AF */
    d5 = span_overlap(ix_rw(VM, a0 + 58), ix_rw(VM, a0 + 60),
                      ix_rw(VM, a1 + 58), ix_rw(VM, a1 + 60));
    int16_t d2 = (int16_t)(ix_rw(VM, a0 + 8) - ix_rw(VM, a1 + 8));
    if (d2 < 0) {
        d6 = 2;
        d2 = (int16_t)-d2;
    } else {
        d6 = 3;
    }
    if (!(d2 > 20)) {
        d5 += span_overlap(ix_rw(VM, a0 + 112), ix_rw(VM, a0 + 114),
                           ix_rw(VM, a1 + 112), ix_rw(VM, a1 + 114));
        if (d5 == 2)
            bclr(m, bits, d6);
    }
}

/* LAB_03A9 : directions bloquées par les autres combattants ; renvoie le
 * masque des directions permises (bits 0-3 de L00_08671). */
static uint16_t blocked_dirs(MogCombat *m, uint32_t a0, uint16_t dx, uint16_t dir)
{
    ix_wl(VM, MOG_LAB_0637, a0);
    ix_ww(VM, MOG_LAB_0635, dx);
    ix_ww(VM, MOG_LAB_0636, (uint16_t)(dir & 3));
    ix_ww(VM, MOG_LAB_03B6, 0x001F);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, en))
            continue;
        uint32_t a1 = ix_rl(VM, en + 24);
        if (a1 == a0)
            continue;
        if (ix_rw(VM, a1 + 58) == 0 || hp(m, a1) <= 0)
            continue;
        block_by(m, a0, a1);
    }
    return ix_rw(VM, MOG_LAB_03B6);
}

/* LAB_0215 : bords de l'arène */
static void arena_bounds(MogCombat *m, uint32_t a0)
{
    int16_t d0 = (ix_rb(VM, a0 + 10) & 2) ? -25 : 25;
    d0 = (int16_t)(d0 + ix_rw(VM, a0 + 4));
    int16_t d1 = (int16_t)(9 + ix_rw(VM, a0 + 8));
    if (d0 < 10)
        bclr(m, a0 + 63, 1);
    if (d0 > 320)
        bclr(m, a0 + 63, 0);
    if (d1 > 155)
        bclr(m, a0 + 63, 2);
    if (d1 < 30)
        bclr(m, a0 + 63, 3);
}

/* LAB_0A71 : obstacles du décor (table SECSTRT_14, 8 octets par obstacle) */
static void terrain_obstacles(MogCombat *m, uint32_t a0, uint16_t dx, uint16_t dy)
{
    uint16_t limit = (uint16_t)(dy + ix_rw(VM, a0 + 8));
    ix_ww(VM, MOG_SECSTRT_13, limit);
    ix_ww(VM, MOG_SECSTRT_13, (uint16_t)(ix_rw(VM, MOG_SECSTRT_13) + 0x2F));
    uint16_t x0 = (uint16_t)(ix_rw(VM, a0 + 58) + dx);
    uint16_t x1 = (uint16_t)(ix_rw(VM, a0 + 60) + dx);
    ix_ww(VM, MOG_LAB_0A77, x0);
    ix_ww(VM, MOG_LAB_0A78, x1);

    uint32_t a1 = ix_rl(VM, MOG_SECSTRT_14);
    uint32_t n = (uint32_t)(uint16_t)(ix_rw(VM, a1) - 1) + 1;   /* DBF */
    a1 += 2;
    for (uint32_t i = 0; i < n; i++, a1 += 8) {
        if (!span_overlap(ix_rw(VM, a1), ix_rw(VM, a1 + 2),
                          ix_rw(VM, MOG_LAB_0A77), ix_rw(VM, MOG_LAB_0A78)))
            continue;
        uint16_t y = ix_rw(VM, a1 + 4);
        uint16_t by = ix_rw(VM, a0 + 114);
        if (span_overlap(0x1E, y, by, (uint16_t)(by + 1))) {
            int16_t lx0 = sw(ix_rw(VM, MOG_LAB_0A77));
            int16_t lx1 = sw(ix_rw(VM, MOG_LAB_0A78));
            if (ix_rb(VM, a0 + 10) & 2) {
                if (!(lx1 < sw(ix_rw(VM, a0 + 58))))
                    bclr(m, a0 + 63, 1);
            } else if (!(lx0 > sw(ix_rw(VM, a0 + 60)))) {
                bclr(m, a0 + 63, 0);
            }
        }
        if (!(sw(y) < sw(ix_rw(VM, MOG_SECSTRT_13))))    /* LAB_0A74 */
            bclr(m, a0 + 63, 3);
    }
}

/* ------------------------------------------------------------------ */
/* Réactions du chevalier humain                                       */
/* ------------------------------------------------------------------ */

/* LAB_020E : script de réaction 30(objet)[attaque adverse] */
static CtlResult react_script(MogCombat *m, uint32_t a0, uint32_t a1)
{
    int16_t k = sw(ix_rw(VM, a0 + 64));
    return set_script(m, ix_rl(VM, ix_rl(VM, a1 + 30) + (uint32_t)(int32_t)k));
}

/* Réponse par l'attaque en cours 34(objet)[64(objet)] (parade réussie). */
static CtlResult own_attack(MogCombat *m)
{
    uint32_t a1 = ix_rl(VM, MOG_LAB_0633);
    int16_t k = sw(ix_rw(VM, a1 + 64));
    return set_script(m, ix_rl(VM, ix_rl(VM, a1 + 34) + (uint32_t)(int32_t)k));
}

/* LAB_01E6 : parade ? (LAB_01EB = 1) — 50(objet)[attaque adverse] doit
 * valoir l'attaque en cours. */
static void check_parry(MogCombat *m, uint32_t a1)
{
    ix_ww(VM, MOG_LAB_01EB, 0);
    uint32_t a0 = ix_rl(VM, a1 + 18);
    uint32_t d0 = ix_rl(VM, ix_rl(VM, a1 + 50) + (uint32_t)(int32_t)sw(ix_rw(VM, a0 + 64)));
    uint16_t d1 = ix_rw(VM, a1 + 64);
    if ((uint16_t)d0 != d1)
        return;
    ix_ww(VM, MOG_LAB_01EB, 1);
    if (d1 == 0x1C) {                                   /* LAB_01E9 */
        ix_ww(VM, MOG_LAB_01EB, 0);
        if (!(ix_rb(VM, a1 + 104) & 0x80)) {
            ix_ww(VM, MOG_LAB_01EB, 1);
            bset(m, a1 + 104, 7);
        }
        return;
    }
    if (ix_rb(VM, a1 + 10) == ix_rb(VM, a0 + 10)) {
        ix_ww(VM, MOG_LAB_01EB, 0);
        return;
    }
    sound(m, 0x11);                                     /* LAB_01E7 */
}

/* LAB_0204 : D0 >> 8(96(objet)) */
static uint16_t scale_down(MogCombat *m, uint32_t a1, uint16_t d0)
{
    unsigned s = ix_rb(VM, ix_rl(VM, a1 + 96) + 8) & 63u;
    return s >= 16 ? 0 : (uint16_t)(d0 >> s);
}

/* LAB_021B : dégâts infligés par a0 (« KNIGHT DAMAGE ») */
static uint16_t knight_damage(MogCombat *m, uint32_t a0)
{
    uint16_t k = ix_rw(VM, a0 + 64);
    uint16_t d0 = (uint16_t)ix_rl(VM, ix_rl(VM, a0 + 42) + (uint32_t)(int32_t)sw(k));
    d0 = (uint16_t)(d0 + ix_rb(VM, a0 + 70));
    uint32_t w = ix_rl(VM, a0 + 88);
    if (w == 0x17) d0 = (uint16_t)(d0 + 2);
    if (w == 0x18) d0 = (uint16_t)(d0 + 3);
    if (w == 0x19) d0 = (uint16_t)(d0 + 5);
    if (k == 0x20)
        d0 = (uint16_t)(d0 << 1);
    uint16_t d2 = ix_rw(VM, MOG_v_Combatants + 18);
    uint8_t d1 = ix_rb(VM, ix_rl(VM, a0 + 96) + 22);
    if (d1) {
        int dbl = 0;
        if ((d1 & 1) && d2 == 0x2E) dbl = 1;
        else if ((d1 & 8) && d2 == 0x2E) dbl = 1;
        else if ((d1 & 2) && d2 == 0x2D) dbl = 1;
        else if ((d1 & 4) && d2 == 0x31) dbl = 1;
        if (dbl)
            d0 = (uint16_t)(d0 << 1);
    }
    msg(m, "KNIGHT DAMAGE:");
    return d0;
}

/* LAB_020B */
static CtlResult r_020B(MogCombat *m, uint32_t a0, uint32_t a1)
{
    if (hp(m, a1) > 0) {                                /* LAB_020D */
        hurt(m, a1, knight_damage(m, a0));
        return react_script(m, a0, a1);
    }
    ix_wl(VM, MOG_v_CtlScript, MOG_LAB_07F8);
    if (ix_rw(VM, a0 + 64) == 8)
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_07F9);
    return ctl_return(m);
}

static CtlResult r_0201(MogCombat *m, uint32_t a0, uint32_t a1, uint16_t d0)
{
    hurt(m, a1, scale_down(m, a1, d0));                 /* LAB_0202 */
    return react_script(m, a0, a1);
}

/* Réaction au coup reçu, selon le contrôleur de l'attaquant (LAB_0621). */
static CtlResult react_hit_by(MogCombat *m, uint32_t fn, uint32_t a0, uint32_t a1,
                              uint16_t idx, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_LAB_0206:
        hurt(m, a1, 5);
        if (!ix_rl(VM, a1 + 14) && hp(m, a1) <= 0 && !ix_rl(VM, MOG_LAB_06DA)) {
            uint32_t att = ix_rl(VM, a1 + 18);
            uint32_t s = ix_rb(VM, a1 + 10) != ix_rb(VM, att + 10) ? MOG_LAB_084E : MOG_LAB_0849;
            restart_entity(m, att, s);
            return set_script(m, 0);
        } else {                                        /* LAB_0209 */
            uint32_t att = ix_rl(VM, a1 + 18);
            return set_script(m, ix_rb(VM, a1 + 10) == ix_rb(VM, att + 10)
                                 ? MOG_LAB_084A : MOG_LAB_084B);
        }
    case MOG_LAB_020B:
        return r_020B(m, a0, a1);
    case MOG_LAB_01F9: {
        uint16_t k = ix_rw(VM, a0 + 64);
        if (k == 0x20 || k == 4) {
            hurt(m, a1, k == 0x20 ? 10 : 8);
            ix_wb(VM, a1 + 10, (uint8_t)(ix_rb(VM, a0 + 10) ^ 2));   /* LAB_01FC */
        } else {
            hurt(m, a1, 10);
        }
        return react_script(m, a0, a1);
    }
    case MOG_LAB_01FD:
        hurt(m, a1, 7);
        if (ix_rw(VM, a0 + 64) == 0x20) {
            a1 = ix_rl(VM, MOG_LAB_0633);
            if (hp(m, a1) <= 0)
                return set_script(m, MOG_LAB_07FD);
        }
        return react_script(m, a0, a1);
    case MOG_LAB_0200:
        ix_ww(VM, a1 + 8, (uint16_t)(ix_rw(VM, a0 + 8) - 1));
        if (ix_rw(VM, a0 + 64) == 4)
            return r_0201(m, a0, a1, 0x14);
        ix_ww(VM, a0 + 64, 0x20);                       /* LAB_0201 */
        return r_0201(m, a0, a1, 0x1E);
    case MOG_LAB_0201:
        ix_ww(VM, a0 + 64, 0x20);
        return r_0201(m, a0, a1, 0x1E);
    case MOG_LAB_0203:
        hurt(m, a1, scale_down(m, a1, (uint16_t)(idx - 10)));
        ix_wb(VM, MOG_LAB_08D0, 1);
        ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CC);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_07FB);
        ix_wb(VM, a1 + 10, 3);
        return ctl_return(m);
    case MOG_LAB_0205:
        if (hp(m, a1) <= 0)
            return set_script(m, MOG_LAB_07F9);         /* LAB_01F5 */
        check_parry(m, a1);
        if (!ix_rw(VM, MOG_LAB_01EB))
            return r_020B(m, a0, a1);
        return own_attack(m);
    case MOG_LAB_01ED:
        if (ix_rw(VM, a0 + 64) == 8)
            ix_wb(VM, a1 + 10, (uint8_t)(ix_rb(VM, a0 + 10) ^ 2));
        hurt(m, a1, 5);
        return react_script(m, a0, a1);
    case MOG_LAB_01EF: {
        uint16_t k = ix_rw(VM, a0 + 64);
        if (k == 4 || k == 8) {
            uint32_t dmg = ix_rl(VM, ix_rl(VM, a0 + 42) + k);
            hurt(m, a1, (uint16_t)dmg);
            if (k == 4) {
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_085F);
                ix_wb(VM, a1 + 130, 1);
                return ctl_return(m);
            }
            return set_script(m, MOG_LAB_085E);
        }
        return r_020B(m, a0, a1);
    }
    case MOG_LAB_01F2:
        if (hp(m, a1) <= 0)                             /* LAB_01F4 */
            return set_script(m, ix_rb(VM, a0 + 77) == 0x18 ? MOG_LAB_07F9 : MOG_LAB_07F8);
        check_parry(m, a1);
        if (ix_rw(VM, MOG_LAB_01EB))
            return own_attack(m);
        a0 = ix_rl(VM, a1 + 18);                        /* LAB_01F3 */
        hurt(m, a1, (uint16_t)ix_rl(VM, ix_rl(VM, a0 + 42)
                                     + (uint32_t)(int32_t)sw(ix_rw(VM, a0 + 64))));
        return react_script(m, a0, a1);
    case MOG_LAB_01F6:
        check_parry(m, a1);
        if (ix_rw(VM, MOG_LAB_01EB))
            return set_script(m, MOG_LAB_07F3);
        hurt(m, a1, 3);
        if (!ix_rl(VM, a1 + 14) && hp(m, a1) <= 0 && !ix_rl(VM, MOG_LAB_06DA)) {
            restart_entity(m, ix_rl(VM, a1 + 18), MOG_LAB_081C);
            return set_script(m, 0);
        }
        return react_script(m, a0, a1);
    }
    *ok = 0;
    return ctl_return(m);
}

/* Réaction après avoir touché, selon le contrôleur de la cible (LAB_0622). */
static CtlResult react_hit(MogCombat *m, uint32_t fn, uint32_t a0, uint32_t a1, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_LAB_01E1: {
        uint8_t c = ix_rb(VM, a0 + 77);
        if (c == 0x0C || c == 0x10) {                   /* LAB_01E4 */
            if (hp(m, a0) > 0) {
                if (ix_rw(VM, a0 + 64) == 0x1C)         /* LAB_01E5 */
                    return set_script(m, 0xFFFFFFFFu);
            } else if (ix_rw(VM, a1 + 64) == 8 && !ix_rl(VM, MOG_LAB_06DA)) {
                return set_script(m, 0xFFFFFFFFu);
            }
        }
        if (ix_rw(VM, a0 + 64) == 0x18)                 /* LAB_01E2 */
            return set_script(m, 0xFFFFFFFFu);
        return set_script(m, ix_rl(VM, a1 + 26));       /* LAB_01E3 */
    }
    case MOG_LAB_0201:
        ix_ww(VM, a0 + 64, 0x20);
        return r_0201(m, a0, a1, 0x1E);
    }
    *ok = 0;
    return ctl_return(m);
}

/* ------------------------------------------------------------------ */
/* Ctl_HumanKnight [LAB_01CA]                                          */
/* ------------------------------------------------------------------ */

/* LAB_00EA : joystick du chevalier (11(objet) = 1 : port 0, sinon port 1) */
static uint16_t read_joystick(MogCombat *m, uint32_t obj)
{
    ix_ww(VM, MOG_LAB_062F, m->joy[0]);                 /* LAB_00EE */
    ix_ww(VM, MOG_LAB_0630, m->joy[1]);
    return ix_rb(VM, obj + 11) == 1 ? m->joy[0] : m->joy[1];
}

/* Ctl_HumanAttack [LAB_01DD] */
static CtlResult human_attack(MogCombat *m, uint32_t a1)
{
    uint16_t d0 = (uint16_t)((ix_rw(VM, a1 + 62) & 0xFFEF) << 1);
    uint32_t tab = ix_rb(VM, a1 + 10) == 3 ? MOG_t_AttackStickL : MOG_t_AttackStickR;
    d0 = ix_rw(VM, tab + (uint32_t)(int32_t)sw(d0));
    ix_ww(VM, a1 + 64, d0);
    return set_script(m, ix_rl(VM, ix_rl(VM, a1 + 34) + (uint32_t)(int32_t)sw(d0)));
}

static CtlResult human_knight(MogCombat *m, uint32_t a0)
{
    uint32_t a1 = a0;
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 22));

    if (ix_rl(VM, a0 + 18)) {                           /* LAB_01EC */
        uint32_t att = ix_rl(VM, a1 + 18);
        uint16_t idx = ix_rb(VM, att + 77);
        uint32_t fn = ix_rl(VM, MOG_LAB_0621 + idx);
        int ok;
        CtlResult r = react_hit_by(m, fn, att, a1, idx, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "réaction (touché) non portée : %08X", fn);
            msg(m, t);
            m->errors++;
        }
        return r;
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_01E0 */
        uint32_t tgt = ix_rl(VM, a1 + 14);
        uint32_t fn = ix_rl(VM, MOG_LAB_0622 + ix_rb(VM, tgt + 77));
        int ok;
        CtlResult r = react_hit(m, fn, tgt, a1, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "réaction (a touché) non portée : %08X", fn);
            msg(m, t);
            m->errors++;
        }
        return r;
    }

    ix_ww(VM, a1 + 64, 0);
    uint16_t d0 = read_joystick(m, a0);
    ix_ww(VM, a1 + 62, d0);
    if (!d0)
        return ctl_return(m);
    if (a0 == ix_rl(VM, MOG_LAB_05D1) && ix_rw(VM, MOG_LAB_05D3)) {   /* commandes inversées */
        if (d0 & 0x0C)
            d0 ^= 0x0C;
        if (d0 & 0x03)
            d0 ^= 0x03;
        ix_ww(VM, a1 + 62, d0);
    }
    if (ix_rb(VM, a1 + 63) & 0x10)
        return human_attack(m, a1);

    ix_ww(VM, MOG_LAB_061E, 0);
    ix_ww(VM, MOG_LAB_061F, 0);
    ix_wb(VM, MOG_LAB_01D8, ix_rb(VM, a1 + 12));
    uint8_t phase = (uint8_t)((ix_rb(VM, a1 + 12) + 1) & 3);
    ix_wb(VM, a1 + 12, phase);
    uint8_t st = ix_rb(VM, a1 + 63);
    if (st & 8)                                         /* LAB_01DB */
        ix_ww(VM, MOG_LAB_061F, (uint16_t)-ix_rw(VM, MOG_t_WalkStepUp + 2u * phase));
    else if (st & 4)                                    /* LAB_01DC */
        ix_ww(VM, MOG_LAB_061F, ix_rw(VM, MOG_t_WalkStepDown + 2u * phase));
    if (st & 3) {                                       /* LAB_01D9 */
        ix_wb(VM, a1 + 10, (st & 1) ? 1 : 3);
        uint16_t dx = ix_rw(VM, MOG_t_WalkStepX + 2u * phase);
        if (ix_rb(VM, a1 + 10) & 2)
            dx = (uint16_t)-dx;
        ix_ww(VM, MOG_LAB_061E, dx);
    }

    /* LAB_01D2 */
    a0 = ix_rl(VM, MOG_LAB_0633);
    uint16_t allowed = blocked_dirs(m, a0, ix_rw(VM, MOG_LAB_061E), ix_rb(VM, a0 + 10));
    ix_ww(VM, a0 + 62, (uint16_t)(ix_rw(VM, a0 + 62) & allowed));
    arena_bounds(m, a0);
    terrain_obstacles(m, a0, ix_rw(VM, MOG_LAB_061E), ix_rw(VM, MOG_LAB_061F));

    uint16_t d1 = ix_rw(VM, a0 + 62);
    uint16_t dx = ix_rw(VM, MOG_LAB_061E), dy = ix_rw(VM, MOG_LAB_061F);
    int moved = 0;
    uint16_t group = 0;
    if (d1 & 8) { ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + dy)); moved = 1; group = 0x20; }
    if (d1 & 4) { ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + dy)); moved = 1; group = 0x40; }
    if (d1 & 2) { ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + dx)); moved = 1; group = 0; }
    if (d1 & 1) { ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + dx)); moved = 1; group = 0; }
    if (!moved) {
        ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
        ix_wb(VM, a0 + 12, ix_rb(VM, MOG_LAB_01D8));
        return ctl_return(m);
    }
    a1 = ix_rl(VM, MOG_LAB_0633);                       /* LAB_01D7 */
    bclr(m, a1 + 104, 7);
    uint16_t k = (uint16_t)(group + (ix_rb(VM, a1 + 12) << 2));
    return set_script(m, ix_rl(VM, ix_rl(VM, a1 + 46) + (uint32_t)(int32_t)sw(k)));
}

/* ------------------------------------------------------------------ */

void mog_combat_init(MogCombat *m, IxVM *vm, const IxHost *host)
{
    IxLayout lay;
    memset(m, 0, sizeof *m);
    ix_layout_mog(&lay);
    ix_engine_init(&m->eng, vm, host, &lay);
}

/* LAB_02CB (contrôleur 52) : objet projeté (LAB_07EC), détruit au contact
 * ou une fois sorti de l'écran. */
static CtlResult projectile(MogCombat *m, uint32_t a0)
{
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, MOG_LAB_07EC);
    int16_t x = sw(ix_rw(VM, a0 + 4));
    int gone;
    if (ix_rl(VM, a0 + 14))
        gone = 1;
    else if (ix_rb(VM, a0 + 10) == 1)
        gone = x >= 0x14A;
    else
        gone = x <= 0 && (int16_t)-x <= 10;             /* LAB_02CC */
    return gone ? set_script(m, 0) : ctl_return(m);     /* LAB_02D1 */
}

/* LAB_02D2 (contrôleur 40) : objet inerte, contacts effacés. */
static CtlResult inert(MogCombat *m, uint32_t a0)
{
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, a0 + 18, 0);
    ix_wl(VM, a0 + 14, 0);
    return set_script(m, 0xFFFFFFFFu);
}

static CtlResult run_controller(MogCombat *m, uint32_t fn, uint32_t obj, int *ok)
{
    *ok = 1;
    switch (fn) {
    case MOG_Ctl_HumanKnight:
        return human_knight(m, obj);
    case MOG_LAB_02CB:
        return projectile(m, obj);
    case MOG_LAB_02D2:
        return inert(m, obj);
    }
    *ok = 0;
    CtlResult r = { 0xFFFFFFFFu, 0, 0, 0, 0 };
    return r;
}

/* Combat_RunControllers [LAB_0322] */
void mog_run_controllers(MogCombat *m)
{
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, en))
            continue;
        uint32_t obj = ix_rl(VM, en + 24);
        if (!ix_rl(VM, obj + 14) && !ix_rl(VM, obj + 18) && ix_rb(VM, en + 1))
            continue;
        uint32_t fn = ix_rl(VM, MOG_t_Controllers + ix_rb(VM, en + 32));
        int ok;
        CtlResult r = run_controller(m, fn, obj, &ok);
        if (!ok) {
            char t[64];
            snprintf(t, sizeof t, "contrôleur non porté : %08X", fn);
            msg(m, t);
            m->errors++;
            continue;
        }
        if (r.script == 0xFFFFFFFFu)
            continue;
        if (r.script == 0) {                            /* LAB_0325 */
            ix_ww(VM, en, 0);
            ix_wl(VM, ix_rl(VM, en + 24), 0);
            msg(m, "TASK & TABLE OFF");
            continue;
        }
        ix_wl(VM, en + 2, r.script);
        ix_ww(VM, en + 6, r.x);
        ix_ww(VM, en + 8, r.h);
        ix_ww(VM, en + 10, r.d);
        ix_wb(VM, en + 22, r.dir);
        ix_wb(VM, en + 1, 1);
    }
}
