/*
 * mog_ai.c — contrôleurs d'IA du combat de mog (t_Controllers), traduits
 * de amiga_asm/mog.asm.
 *
 * Ces routines se passent souvent des valeurs par registres d'une routine
 * à l'autre (ex. D1 calculé par LAB_0F24 puis relu par le contrôleur) :
 * elles travaillent alors sur un jeu de registres explicite (Regs), avec
 * les tailles d'opération du 68000 (mot : seuls les 16 bits bas changent).
 */
#include "mog_private.h"
#include "ix_mog_syms.h"

#define VM (m->eng.vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }
static int16_t hp(MogCombat *m, uint32_t obj) { return sw(ix_rw(VM, obj + 80)); }

static void hurt(MogCombat *m, uint32_t obj, uint16_t n)
{
    ix_ww(VM, obj + 80, (uint16_t)(ix_rw(VM, obj + 80) - n));
}

/* ------------------------------------------------------------------ */
/* LAB_0226 (contrôleur 0) : chevalier qui traverse l'écran             */
/* ------------------------------------------------------------------ */

static CtlResult passing_knight(MogCombat *m, uint32_t a0)
{
    uint32_t a1 = a0;
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_ww(VM, a1 + 62, 0);
    ix_ww(VM, a1 + 64, 0x20);
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));

    if (ix_rl(VM, a1 + 18)) {                           /* LAB_0230 : touché */
        uint32_t att = ix_rl(VM, a1 + 18);
        if (ix_rb(VM, att + 77) != 0) {
            hurt(m, a1, mog_knight_damage(m, att));
            return mog_react_script(m, att, a1);
        }
    }
    if (ix_rl(VM, a1 + 14)) {                           /* LAB_0231 : a touché */
        uint32_t tgt = ix_rl(VM, a1 + 14);
        if (ix_rb(VM, tgt + 77) != 0) {
            if (ix_rl(VM, tgt + 14) || hp(m, tgt) > 0 || ix_rl(VM, MOG_LAB_06DA))
                return mog_set_script(m, ix_rl(VM, a1 + 26));      /* LAB_0233 */
            mog_kill_entity_of(m, ix_rl(VM, MOG_LAB_0634));
            ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);   /* LAB_000D */
            mog_end_combat(m);
            return mog_set_script(m, ix_rb(VM, a1 + 10) != ix_rb(VM, tgt + 10)
                                     ? MOG_LAB_084E : MOG_LAB_0849);
        }
    }

    /* LAB_0228 : sorti de l'écran -> revient de l'autre côté */
    uint32_t a2 = ix_rl(VM, MOG_LAB_0634);
    int16_t x = sw(ix_rw(VM, a1 + 4));
    int wrap = 0;
    if (ix_rb(VM, a1 + 10) != 3) {
        if (x >= 0x154) {
            ix_ww(VM, a1 + 4, 0x17C);
            ix_wb(VM, a1 + 10, 3);
            wrap = 1;
        }
    } else if (x < 0) {
        ix_ww(VM, a1 + 4, 0xFFCE);
        ix_wb(VM, a1 + 10, 1);
        wrap = 1;
    }
    if (wrap) {                                         /* LAB_022A */
        uint16_t t = (uint16_t)(ix_rw(VM, MOG_LAB_0234) ^ 1);
        ix_ww(VM, MOG_LAB_0234, t);
        if (t)
            ix_ww(VM, a1 + 8, ix_rw(VM, a2 + 8));
        else
            ix_ww(VM, a1 + 8, (uint16_t)(ix_rw(VM, a2 + 8) + ((m->vhposr & 7u) << 2)));
        uint8_t n = (uint8_t)((mog_random(m) & 0x0F) | 5);   /* LAB_022C */
        ix_wb(VM, a1 + 13, n);
        ix_wb(VM, a1 + 13, (uint8_t)(n - 1));
        if ((uint8_t)(n - 1) != 0)
            return mog_ctl_return(m);
    }

    /* LAB_022D : pas de marche suivant */
    uint8_t ph = (uint8_t)(ix_rb(VM, a1 + 12) + 1);
    if (ph == 4)
        ph = 0;
    ix_wb(VM, a1 + 12, ph);
    a1 = ix_rl(VM, MOG_LAB_0633);
    int16_t k = (int8_t)ix_rb(VM, a1 + 12);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a1 + 46) + (uint32_t)(int32_t)(int16_t)(k << 2)));
    uint16_t dx = ix_rw(VM, MOG_L00_05C20 + (uint32_t)(int32_t)(int16_t)(k << 1));
    if (ix_rb(VM, a1 + 10) & 2)
        dx = (uint16_t)-dx;
    ix_ww(VM, a1 + 4, (uint16_t)(ix_rw(VM, a1 + 4) + dx));
    return mog_ctl_return(m);
}

/* ------------------------------------------------------------------ */
/* Déplacement commun des créatures (LAB_0F1A-LAB_0F32, LAB_02BB-02C8) */
/* ------------------------------------------------------------------ */

/* Registres de données qui passent d'une routine à l'autre (opérations
 * sur mot : les 16 bits hauts sont conservés). */
typedef struct {
    uint32_t d0, d1, d6, d7;
} Regs;

static void setw(uint32_t *r, uint16_t v) { *r = (*r & 0xFFFF0000u) | v; }
static void setb(uint32_t *r, uint8_t v)  { *r = (*r & 0xFFFFFF00u) | v; }

static uint32_t me(MogCombat *m)  { return ix_rl(VM, MOG_LAB_0633); }
static uint32_t foe(MogCombat *m) { return ix_rl(VM, MOG_LAB_0634); }

static void bset63(MogCombat *m, uint32_t obj, int bit)
{
    ix_wb(VM, obj + 63, (uint8_t)(ix_rb(VM, obj + 63) | (1u << bit)));
}

/* |a - b| sur mot (NEG.W si négatif) */
static uint16_t absw(uint16_t a, uint16_t b)
{
    uint16_t d = (uint16_t)(a - b);
    return (d & 0x8000) ? (uint16_t)-d : d;
}

/* LAB_02BB : D0 = 1 si l'écart de profondeur <= 120(moi) ; D1.w = écart */
static void near_in_depth(MogCombat *m, Regs *r)
{
    uint32_t a1 = me(m), a0 = foe(m);
    r->d0 = 0;
    setw(&r->d1, absw(ix_rw(VM, a0 + 8), ix_rw(VM, a1 + 8)));
    if (!(sw((uint16_t)r->d1) > sw(ix_rw(VM, a1 + 120))))
        r->d0 = 1;
}

/* LAB_02BF : D0 = 1 si l'écart en X <= D7.w ; D1.w = écart */
static void near_in_x(MogCombat *m, Regs *r)
{
    uint32_t a1 = me(m), a0 = foe(m);
    r->d0 = 0;
    setw(&r->d1, absw(ix_rw(VM, a0 + 4), ix_rw(VM, a1 + 4)));
    if (!(sw((uint16_t)r->d1) > sw((uint16_t)r->d7)))
        r->d0 = 1;
}

/* LAB_02C2 : D0.w = |X moi - X adversaire|, D1 = 1 si adversaire à droite */
static void dist_x(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m), a1 = foe(m);
    r->d1 = 0;
    uint16_t d = (uint16_t)(ix_rw(VM, a0 + 4) - ix_rw(VM, a1 + 4));
    if (d & 0x8000) {
        d = (uint16_t)-d;
        r->d1 = 1;
    }
    setw(&r->d0, d);
}

/* LAB_02C4 : se tourne vers l'adversaire */
static void face_foe(MogCombat *m, Regs *r)
{
    uint32_t a1 = me(m), a0 = foe(m);
    setw(&r->d0, ix_rw(VM, a1 + 4));
    ix_wb(VM, a1 + 10, sw((uint16_t)r->d0) < sw(ix_rw(VM, a0 + 4)) ? 1 : 3);
}

/* LAB_02C8 : D0 = 1 si l'adversaire est à droite, sinon 3 */
static void foe_side(MogCombat *m, Regs *r)
{
    uint32_t a1 = me(m), a0 = foe(m);
    setw(&r->d0, ix_rw(VM, a1 + 4));
    r->d0 = sw((uint16_t)r->d0) < sw(ix_rw(VM, a0 + 4)) ? 1 : 3;
}

/* LAB_0F24 : approche de l'adversaire ; pose les bits de direction dans
 * 63(moi). D0 = 0 : à portée (D1.w = distance) ; D0 = 1 : se déplacer. */
static void approach(MogCombat *m, Regs *r)
{
    ix_ww(VM, MOG_LAB_0F3A, 0);
    ix_ww(VM, MOG_LAB_0F3B, 0);
    ix_ww(VM, MOG_LAB_0F3D, 0);
    ix_ww(VM, MOG_LAB_0F3C, 0);
    uint32_t a0 = me(m), a1 = foe(m);
    face_foe(m, r);
    ix_ww(VM, MOG_LAB_0F3F, 0);
    near_in_depth(m, r);
    if ((uint16_t)r->d0) {
        ix_ww(VM, MOG_LAB_0F3D, 1);
    } else {
        setw(&r->d1, ix_rw(VM, a0 + 8));
        bset63(m, a0, sw((uint16_t)r->d1) > sw(ix_rw(VM, a1 + 8)) ? 3 : 2);
        ix_ww(VM, MOG_LAB_0F3F, 1);
    }
    setw(&r->d7, ix_rw(VM, a0 + 118));                  /* LAB_0F27 */
    near_in_x(m, r);
    if ((uint16_t)r->d0) {                              /* LAB_0F2B : trop près */
        a0 = me(m);
        a1 = foe(m);
        setw(&r->d0, ix_rw(VM, a0 + 4));
        setw(&r->d1, (uint16_t)(ix_rw(VM, a1 + 4) - (uint16_t)r->d0));
        bset63(m, a0, (r->d1 & 0x8000) ? 0 : 1);
        r->d0 = 1;
        return;
    }
    setw(&r->d7, ix_rw(VM, a0 + 116));
    near_in_x(m, r);
    if ((uint16_t)r->d0) {
        r->d0 = ix_rw(VM, MOG_LAB_0F3F);
        return;
    }
    foe_side(m, r);                                     /* LAB_0F28 */
    setw(&r->d1, ix_rw(VM, a0 + 116));
    bset63(m, a0, (uint16_t)r->d0 == 1 ? 0 : 1);
    r->d0 = 1;
}

/* LAB_0F2E : D7 = +1 si le bit de la direction regardée est posé, sinon -1 */
static void walk_sense(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m);
    setb(&r->d0, ix_rb(VM, a0 + 10));
    int bit = (uint8_t)r->d0 == 1 ? 0 : 1;
    r->d7 = (ix_rb(VM, a0 + 63) & (1u << bit)) ? 1u : 0xFFFFFFFFu;
}

/* Pas (dx, dy) de la table `tab` pour la phase 12(moi) */
static void step_from(MogCombat *m, Regs *r, uint32_t tab)
{
    uint32_t a0 = me(m);
    setb(&r->d0, ix_rb(VM, a0 + 12));
    setw(&r->d0, (uint16_t)((uint16_t)r->d0 << 2));
    uint32_t e = tab + (uint32_t)(int32_t)sw((uint16_t)r->d0);
    ix_ww(VM, MOG_LAB_0F3A, ix_rw(VM, e));
    ix_ww(VM, MOG_LAB_0F3B, ix_rw(VM, e + 2));
}

static void step_left(MogCombat *m, Regs *r, uint32_t tab)       /* LAB_0F1A */
{
    walk_sense(m, r);
    step_from(m, r, tab);
    r->d6 = 0;
    ix_ww(VM, MOG_LAB_0F3A, (uint16_t)-ix_rw(VM, MOG_LAB_0F3A));
    if (ix_rw(VM, MOG_LAB_0F3C))
        ix_ww(VM, MOG_LAB_0F3B, ix_rw(VM, MOG_LAB_0F3C));
}

static void step_right(MogCombat *m, Regs *r, uint32_t tab)      /* LAB_0F1C */
{
    walk_sense(m, r);
    step_from(m, r, tab);
    r->d6 = 0;
    if (ix_rw(VM, MOG_LAB_0F3C))
        ix_ww(VM, MOG_LAB_0F3B, ix_rw(VM, MOG_LAB_0F3C));
}

static void step_up(MogCombat *m, Regs *r, uint32_t tab)         /* LAB_0F1E */
{
    r->d7 = 1;
    step_from(m, r, tab);
    r->d6 = 32;
    ix_ww(VM, MOG_LAB_0F3B, (uint16_t)-ix_rw(VM, MOG_LAB_0F3B));
    ix_ww(VM, MOG_LAB_0F3C, 0xFFFB);
}

static void step_down(MogCombat *m, Regs *r, uint32_t tab)       /* LAB_0F1F */
{
    r->d7 = 1;
    step_from(m, r, tab);
    ix_ww(VM, MOG_LAB_0F3C, 5);
    r->d6 = 64;
}

/* LAB_0F32 : phase suivante (sens D7) ayant un script dans 46(moi)[D6] */
static void next_phase(MogCombat *m, Regs *r, uint32_t a0)
{
    for (int guard = 0; guard < 64; guard++) {
        ix_wb(VM, a0 + 12, (uint8_t)((ix_rb(VM, a0 + 12) + (uint8_t)r->d7) & 7));
        r->d0 = (uint32_t)ix_rb(VM, a0 + 12) << 2;
        r->d0 += r->d6;
        if (ix_rl(VM, ix_rl(VM, a0 + 46) + (uint32_t)(int32_t)sw((uint16_t)r->d0))) {
            ix_ww(VM, MOG_LAB_0F3E, (uint16_t)r->d6);
            return;
        }
    }
    m->errors++;                                        /* l'original boucle sans fin */
}

/* LAB_0F20 : applique le pas (LAB_0F3A/0F3B) si les autres le permettent */
static CtlResult walk(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m);
    next_phase(m, r, a0);
    uint16_t allowed = mog_blocked_dirs(m, a0, ix_rw(VM, MOG_LAB_0F3A), ix_rb(VM, a0 + 10));
    a0 = me(m);
    uint16_t bits = (uint16_t)(ix_rw(VM, a0 + 62) & allowed);
    ix_ww(VM, a0 + 62, bits);
    if (!bits) {
        ix_wb(VM, a0 + 12, 0);
        return mog_set_script(m, ix_rl(VM, a0 + 22));
    }
    if (!(bits & 3))
        ix_ww(VM, MOG_LAB_0F3A, 0);
    if (!(bits & 0x0C))
        ix_ww(VM, MOG_LAB_0F3B, 0);
    uint16_t k = (uint16_t)((ix_rb(VM, a0 + 12) << 2) + ix_rw(VM, MOG_LAB_0F3E));  /* LAB_0F23 */
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a0 + 46) + (uint32_t)(int32_t)sw(k)));
    ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + ix_rw(VM, MOG_LAB_0F3A)));
    ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + ix_rw(VM, MOG_LAB_0F3B)));
    return mog_ctl_return(m);
}

/* Bits de direction de 63(moi) -> pas, puis LAB_0F20 (tables : horizontal,
 * haut, bas). Suite LAB_0239-LAB_023D et équivalents. */
static CtlResult walk_by_bits(MogCombat *m, Regs *r, uint32_t th, uint32_t tu, uint32_t td,
                              int reset_d0)
{
    uint32_t a0 = me(m);
    if (reset_d0)                                       /* MOVEQ #0,D0 / MOVE.B 12(A0),D0 */
        r->d0 = ix_rb(VM, a0 + 12);
    if (ix_rb(VM, a0 + 63) & 8) step_up(m, r, tu);
    if (ix_rb(VM, a0 + 63) & 4) step_down(m, r, td);
    if (ix_rb(VM, a0 + 63) & 1) step_right(m, r, th);
    if (ix_rb(VM, a0 + 63) & 2) step_left(m, r, th);
    return walk(m, r);
}

/* LAB_04A3 : aléatoire 0-99 (à peu près) */
static uint32_t random100(MogCombat *m)
{
    uint32_t d0 = mog_random(m) & 0x7F;
    if (d0 >= 100)
        d0 -= 0x1B;
    return d0;
}

/* ------------------------------------------------------------------ */
/* LAB_0236 (contrôleurs 24, 28, 32)                                   */
/* ------------------------------------------------------------------ */

static CtlResult creature_0236(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    uint32_t a1 = a0;
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 22));
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));

    uint32_t a62 = a0;                                  /* objet effacé en LAB_0237 */
    if (ix_rl(VM, a0 + 18)) {                           /* LAB_0246 : touché */
        uint32_t att = ix_rl(VM, a1 + 18);
        ix_wb(VM, a1 + 106, 0);
        uint8_t c = ix_rb(VM, att + 77);
        uint16_t dmg;
        if (c == 0x0C || c == 0x20)
            dmg = mog_knight_damage(m, att);
        else if (c == 0x34)
            dmg = 3;
        else
            return mog_ctl_return(m);
        hurt(m, a1, dmg);
        return mog_react_script(m, att, a1);
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_024B : a touché */
        ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 26));
        ix_wb(VM, a1 + 106, 10);
        uint32_t tgt = ix_rl(VM, a1 + 14);
        if (ix_rb(VM, tgt + 77) == 0x0C) {
            if (!ix_rl(VM, tgt + 14) && hp(m, tgt) <= 0) {
                ix_wl(VM, MOG_v_CtlScript, 0xFFFFFFFFu);
                if (ix_rb(VM, a1 + 77) == 0x20 && !ix_rl(VM, MOG_LAB_06DA)) {
                    mog_kill_entity_of(m, tgt);
                    return mog_set_script(m, MOG_LAB_081C);
                }
            }
            if (ix_rb(VM, a1 + 77) == 0x20)             /* LAB_024C */
                ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 22));
            return mog_ctl_return(m);
        }
        a62 = tgt;                                      /* (l'original efface la cible) */
    }

    /* LAB_0237 */
    ix_ww(VM, a62 + 62, 0);
    ix_ww(VM, a62 + 64, 0);
    approach(m, &r);
    int attack = 0;
    if (!(uint16_t)r.d0) {
        attack = 1;
    } else if (ix_rw(VM, MOG_LAB_0F3D)) {
        dist_x(m, &r);
        setw(&r.d1, (uint16_t)r.d0);
        attack = 1;
    }
    if (attack) {                                       /* LAB_023E */
        a0 = me(m);
        int16_t d = sw((uint16_t)r.d1);
        if (d > sw(ix_rw(VM, a0 + 118))) {
            uint32_t atk = ix_rl(VM, a0 + 34);
            uint32_t a2 = foe(m);
            if (hp(m, a2) <= 0) {
                if (ix_rl(VM, MOG_LAB_06DA))
                    return mog_ctl_return(m);
                if (!(d > 100)) {
                    if (ix_rw(VM, MOG_LAB_0620))
                        return mog_ctl_return(m);
                    if (ix_rb(VM, a0 + 106)) {
                        ix_wb(VM, a0 + 106, (uint8_t)(ix_rb(VM, a0 + 106) - 1));
                        return mog_ctl_return(m);
                    }
                    ix_ww(VM, MOG_LAB_0620, 1);         /* LAB_0240 */
                    goto thrust;
                }
            } else {
                if (ix_rb(VM, a0 + 106)) {              /* LAB_0241 */
                    ix_wb(VM, a0 + 106, (uint8_t)(ix_rb(VM, a0 + 106) - 1));
                    return mog_ctl_return(m);
                }
                if (ix_rb(VM, a0 + 77) == 0x20) {       /* LAB_0242 */
                    if (!(d > sw(ix_rw(VM, a0 + 116)))) {
                        ix_wb(VM, a0 + 106, 0x14);
                        ix_ww(VM, a0 + 64, 4);
                        return mog_set_script(m, MOG_LAB_081B);
                    }
                } else if (!(d > 100)) {                /* LAB_0243 */
                    if (sw((uint16_t)random100(m)) > 30)
                        goto thrust;
                    if (ix_rw(VM, ix_rl(VM, MOG_LAB_05F2) + 64) != 0x10)
                        goto thrust;
                    goto swing;
                } else {
                    goto swing;
                }
            }
            goto move;
thrust:                                                 /* LAB_0244 */
            ix_wb(VM, a0 + 106, 10);
            ix_ww(VM, a0 + 64, 8);
            return mog_set_script(m, ix_rl(VM, atk + 8));
swing:                                                  /* LAB_0245 */
            if (d > 120)
                goto move;
            ix_wb(VM, a0 + 106, 10);
            ix_ww(VM, a0 + 64, 0x20);
            return mog_set_script(m, ix_rl(VM, atk + 32));
        }
    }
move:                                                   /* LAB_0238 */
    a0 = me(m);
    if (!ix_rb(VM, a0 + 63))
        return mog_ctl_return(m);
    return walk_by_bits(m, &r, MOG_LAB_024E, MOG_LAB_024F, MOG_LAB_0250, 1);
}

/* ------------------------------------------------------------------ */
/* Trajectoires (table LAB_0301 : 6 × 20 octets)                       */
/* ------------------------------------------------------------------ */

/* DIVS / DIVU du 68000 : D1 (32 bits) / D0.w ; quotient dans le mot bas,
 * reste dans le mot haut ; en cas de débordement D1 est inchangé. */
static uint32_t divs(uint32_t d1, uint16_t d0)
{
    int32_t n = (int32_t)d1, d = (int16_t)d0;
    if (d == 0)
        return d1;                                      /* (exception sur 68000) */
    int32_t q = n / d, r = n % d;
    if (q < -32768 || q > 32767)
        return d1;
    return ((uint32_t)(uint16_t)r << 16) | (uint16_t)q;
}

static uint32_t divu(uint32_t d1, uint16_t d0)
{
    if (d0 == 0)
        return d1;
    uint32_t q = d1 / d0, r = d1 % d0;
    if (q > 0xFFFF)
        return d1;
    return (r << 16) | q;
}

static uint32_t extl(uint32_t v) { return (uint32_t)(int32_t)(int16_t)v; }

/* LAB_02F6 / LAB_02F8 : trajectoire de l'objet LAB_062E vers la cible
 * décrite dans LAB_062E (voir LAB_02D3). */
static void start_flight(MogCombat *m)
{
    uint32_t a0 = MOG_LAB_0301;
    uint32_t key = ix_rl(VM, MOG_LAB_062E);
    int i;
    for (i = 0; i < 6; i++, a0 += 20)
        if (ix_rl(VM, a0) == key || !ix_rl(VM, a0))
            break;
    if (i == 6)
        return;
    uint32_t a2 = me(m), a1 = MOG_LAB_062E;
    ix_ww(VM, a2 + 126, ix_rw(VM, 0x0A));   /* bug d'origine : vecteurs 68000 */
    ix_ww(VM, a2 + 128, ix_rw(VM, 0x0C));
    uint16_t dz = (uint16_t)(ix_rw(VM, a1 + 8) - ix_rw(VM, a1 + 14));
    if (dz & 0x8000)
        dz = (uint16_t)-dz;
    uint32_t d0, d1;
    ix_wl(VM, a0, ix_rl(VM, a1));
    ix_ww(VM, a0 + 4, ix_rw(VM, a1 + 16));
    if (sw(dz) > 5) {
        d0 = ix_rw(VM, a1 + 16);
        d1 = (uint16_t)(ix_rw(VM, a1 + 8) - ix_rw(VM, a1 + 14));
        if (!(d1 & 0x8000)) {
            d1 = divs(extl(d1) << 8, (uint16_t)d0);
            d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + d1);
            ix_ww(VM, a0 + 6, (uint16_t)d1);
            d1 = divs(extl(d1), (uint16_t)(d0 - 1));
            ix_ww(VM, a0 + 8, (uint16_t)d1);
        } else {                                        /* LAB_02FA */
            ix_ww(VM, a0 + 6, 0);
            d1 = divs(extl(d1) << 8, (uint16_t)d0);
            d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + d1);
            d1 = divs(extl(d1), (uint16_t)(d0 - 1));
            ix_ww(VM, a0 + 8, (uint16_t)-d1);
        }
    } else {                                            /* LAB_02FB */
        d0 = (uint16_t)((uint16_t)(ix_rw(VM, a1 + 16) + 1) >> 1);
        d1 = (uint16_t)(ix_rw(VM, a1 + 18) << 8);
        d1 = divu(d1, (uint16_t)d0);
        d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + d1);
        ix_ww(VM, a0 + 6, (uint16_t)d1);
        d1 = divu(extl(d1), (uint16_t)(d0 - 1));
        ix_ww(VM, a0 + 8, (uint16_t)d1);
    }
    d0 = ix_rw(VM, a1 + 16);                            /* LAB_02FC */
    d1 = divs(extl((uint16_t)((uint16_t)(ix_rw(VM, a1 + 10) - ix_rw(VM, a1 + 4)) << 6)), (uint16_t)d0);
    ix_ww(VM, a0 + 10, (uint16_t)d1);
    d1 = divs(extl((uint16_t)((uint16_t)(ix_rw(VM, a1 + 12) - ix_rw(VM, a1 + 6)) << 6)), (uint16_t)d0);
    ix_ww(VM, a0 + 12, (uint16_t)d1);
    ix_ww(VM, a0 + 14, (uint16_t)(ix_rw(VM, a1 + 4) << 6));
    ix_ww(VM, a0 + 16, (uint16_t)(ix_rw(VM, a1 + 6) << 6));
    ix_ww(VM, a0 + 18, (uint16_t)(ix_rw(VM, a1 + 8) << 8));
}

/* LAB_02FD : un pas de la trajectoire de `obj` ; D1/D2/D3.w = X,
 * profondeur, hauteur. D0 = -1 (aucune), 1 (terminée), 0. */
typedef struct {
    uint32_t d0, d1, d2, d3;
} Flight;

static void fly(MogCombat *m, uint32_t obj, Flight *f)
{
    uint32_t a0 = MOG_LAB_0301;
    int i;
    for (i = 0; i < 6; i++, a0 += 20)
        if (ix_rl(VM, a0) == obj)
            break;
    if (i == 6) {
        f->d0 = 0xFFFFFFFFu;
        return;
    }
    uint16_t vz = ix_rw(VM, a0 + 6), g = ix_rw(VM, a0 + 8);
    ix_ww(VM, a0 + 6, (uint16_t)(vz - g));
    ix_ww(VM, a0 + 18, (uint16_t)(ix_rw(VM, a0 + 18) - vz));
    ix_ww(VM, a0 + 14, (uint16_t)(ix_rw(VM, a0 + 14) + ix_rw(VM, a0 + 10)));
    ix_ww(VM, a0 + 16, (uint16_t)(ix_rw(VM, a0 + 16) + ix_rw(VM, a0 + 12)));
    setw(&f->d1, (uint16_t)(sw(ix_rw(VM, a0 + 14)) >> 6));
    setw(&f->d2, (uint16_t)(sw(ix_rw(VM, a0 + 16)) >> 6));
    setw(&f->d3, (uint16_t)(sw(ix_rw(VM, a0 + 18)) >> 8));
    uint16_t n = (uint16_t)(ix_rw(VM, a0 + 4) - 1);
    ix_ww(VM, a0 + 4, n);
    if (!n) {
        ix_wl(VM, a0, 0);
        f->d0 = 1;
    } else {
        f->d0 = 0;
    }
}

/* LAB_02D3 : cible du saut (joueur, décalé de ±D7) dans LAB_062E */
static void aim_jump(MogCombat *m, uint32_t a0, uint16_t d7)
{
    uint32_t a1 = me(m), a2 = foe(m);
    if (!((uint16_t)(ix_rw(VM, a2 + 4) - ix_rw(VM, a1 + 4)) & 0x8000))
        d7 = (uint16_t)-d7;
    uint32_t a3 = MOG_LAB_062E;
    ix_wl(VM, a3, a1);
    ix_ww(VM, a3 + 4, ix_rw(VM, a1 + 4));
    ix_ww(VM, a3 + 6, ix_rw(VM, a1 + 8));
    ix_ww(VM, a3 + 8, ix_rw(VM, a1 + 6));
    ix_ww(VM, a3 + 10, (uint16_t)(ix_rw(VM, a2 + 4) + d7));
    ix_ww(VM, a1 + 126, (uint16_t)(ix_rw(VM, a2 + 4) + d7));
    ix_ww(VM, a3 + 12, ix_rw(VM, a2 + 8));
    ix_ww(VM, a0 + 128, ix_rw(VM, a2 + 8));
    ix_ww(VM, a3 + 14, ix_rw(VM, a2 + 6));
    uint16_t dx = absw(ix_rw(VM, a3 + 10), ix_rw(VM, a3 + 4));
    ix_ww(VM, MOG_LAB_02DA, dx);
    uint16_t dd = absw(ix_rw(VM, a3 + 12), ix_rw(VM, a3 + 6));
    if (!(sw(dd) < sw(ix_rw(VM, MOG_LAB_02DA))))
        ix_ww(VM, MOG_LAB_02DA, dd);
    uint16_t dist = ix_rw(VM, MOG_LAB_02DA);
    ix_ww(VM, MOG_LAB_0629, (uint16_t)(dist >> 1));
    ix_ww(VM, MOG_LAB_0628, (uint16_t)(dist >> 3));
    if (!(sw(ix_rw(VM, MOG_LAB_0629)) > 2))
        ix_ww(VM, MOG_LAB_0629, 3);
    if (!(sw(ix_rw(VM, MOG_LAB_0628)) > 4)) {
        ix_ww(VM, MOG_LAB_0628, 4);
        ix_ww(VM, MOG_LAB_0629, 10);
    }
    ix_ww(VM, a3 + 16, ix_rw(VM, MOG_LAB_0628));
    ix_ww(VM, a3 + 18, ix_rw(VM, MOG_LAB_0629));
    ix_ww(VM, a1 + 106, ix_rw(VM, MOG_LAB_0628));
}

/* LAB_0F33 : script de marche 46(moi)[phase*4 + LAB_0F3E] */
static CtlResult walk_script(MogCombat *m)
{
    uint32_t a0 = me(m);
    uint16_t k = (uint16_t)((ix_rb(VM, a0 + 12) << 2) + ix_rw(VM, MOG_LAB_0F3E));
    return mog_set_script(m, ix_rl(VM, ix_rl(VM, a0 + 46) + (uint32_t)(int32_t)sw(k)));
}

/* ------------------------------------------------------------------ */
/* LAB_0251 (contrôleur 36) : hommes-rats                              */
/* ------------------------------------------------------------------ */

static void bset8(MogCombat *m, uint32_t a, int bit)
{
    ix_wb(VM, a, (uint8_t)(ix_rb(VM, a) | (1u << bit)));
}

static void bclr8(MogCombat *m, uint32_t a, int bit)
{
    ix_wb(VM, a, (uint8_t)(ix_rb(VM, a) & ~(1u << bit)));
}

static int btst8(MogCombat *m, uint32_t a, int bit) { return (ix_rb(VM, a) >> bit) & 1; }

/* LAB_00EE : lit les deux joysticks ; renvoie D1 = port 1 */
static uint16_t poll_joystick(MogCombat *m)
{
    ix_ww(VM, MOG_LAB_062F, m->joy[0]);
    ix_ww(VM, MOG_LAB_0630, m->joy[1]);
    return m->joy[1];
}

/* Libère le chevalier tenu : dégel, liens effacés, script. */
static void release_knight(MogCombat *m, uint32_t k, uint32_t script)
{
    ix_wl(VM, k + 14, 0);
    ix_wl(VM, k + 18, 0);
    mog_restart_entity(m, k, script);
}

static CtlResult rat_waiting(MogCombat *m, Regs *r);

/* LAB_025A : le rat suit sa trajectoire */
static CtlResult rat_flying(MogCombat *m, Regs *r, Flight *f)
{
    fly(m, me(m), f);
    uint32_t a0 = me(m);
    ix_ww(VM, a0 + 4, (uint16_t)f->d1);
    ix_ww(VM, a0 + 8, (uint16_t)f->d2);
    ix_ww(VM, a0 + 6, (uint16_t)f->d3);
    if ((uint16_t)f->d0) {
        if (btst8(m, a0 + 104, 2)) {                    /* LAB_025D */
            ix_wb(VM, a0 + 104, 0);
            bset8(m, a0 + 104, 3);
            return rat_waiting(m, r);                   /* suite en LAB_025E */
        }
        ix_ww(VM, a0 + 6, 0);
        ix_wb(VM, a0 + 104, 0);
        return mog_ctl_return(m);
    }
    r->d7 = 1;                                          /* LAB_025B */
    r->d6 = sw(ix_rw(VM, a0 + 6)) <= -10 ? 32 : 0;
    next_phase(m, r, a0);
    return walk_script(m);
}

static CtlResult rat_jump(MogCombat *m, Regs *r);
static CtlResult rat_waiting(MogCombat *m, Regs *r);

/* LAB_025E : rat au sol près du chevalier, compte à rebours 106 */
static CtlResult rat_waiting(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m);
    uint16_t n = (uint16_t)(ix_rw(VM, a0 + 106) - 1);
    ix_ww(VM, a0 + 106, n);
    if (!n) {                                           /* LAB_0260 */
        ix_wb(VM, a0 + 104, 0);
        bclr8(m, MOG_LAB_062B, 2);
        return rat_jump(m, r);
    }
    dist_x(m, r);
    return mog_set_script(m, sw((uint16_t)r->d0) < 60 ? MOG_LAB_0864 : MOG_LAB_0863);
}

/* LAB_0256 : saute vers le chevalier */
static CtlResult rat_jump(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m);
    aim_jump(m, a0, ix_rw(VM, a0 + 116));
    uint16_t d0 = (uint16_t)(ix_rw(VM, MOG_LAB_0628) >> 1);
    if (sw(d0) < 8)
        d0 = 8;
    a0 = me(m);
    ix_ww(VM, MOG_LAB_062E + 16, d0);
    if (!(sw(ix_rw(VM, MOG_LAB_02DA)) > 40)) {
        ix_wb(VM, a0 + 104, (uint8_t)(ix_rb(VM, a0 + 104) | 0x80));
        ix_ww(VM, MOG_LAB_062E + 18, 2);
        start_flight(m);
        r->d7 = 1;
        r->d6 = 32;
        next_phase(m, r, me(m));
        return walk_script(m);
    }
    start_flight(m);                                    /* LAB_0258 */
    a0 = me(m);                                         /* LAB_0259 */
    ix_wb(VM, a0 + 104, (uint8_t)(ix_rb(VM, a0 + 104) | 1));
    return mog_set_script(m, MOG_LAB_0850);
}

static CtlResult ratman(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    Flight f = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_wl(VM, MOG_LAB_0633, a0);
    uint32_t a1 = a0;
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));

    if (ix_rl(VM, a1 + 18)) {                           /* LAB_026B : touché */
        a0 = ix_rl(VM, a1 + 18);
        if (ix_rb(VM, a0 + 77) == 0x0C) {
            uint16_t k = ix_rw(VM, a0 + 64);
            if (btst8(m, a1 + 104, 0)) {                /* LAB_026F : en l'air */
                mog_message(m, "Rat hit in AIR");
                if (k == 0x18) {                        /* LAB_0270 */
                    ix_wl(VM, MOG_v_CtlScript, MOG_LAB_086B);
                    ix_wb(VM, a1 + 104, 0);
                    ix_ww(VM, a1 + 6, 0);
                    ix_ww(VM, a1 + 80, 0xFFFF);
                    return mog_ctl_return(m);
                }
                hurt(m, a1, mog_knight_damage(m, foe(m)));
                ix_wb(VM, a1 + 104, 0);
                ix_ww(VM, a1 + 6, 0);
                return mog_set_script(m, MOG_LAB_0862);
            }
            if (k == 0x10 || k == 0x1C) {               /* LAB_026D */
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0860);
                ix_wb(VM, a1 + 104, 0);
                ix_ww(VM, a1 + 6, 0);
                ix_ww(VM, a1 + 80, 0xFFFF);
                return mog_ctl_return(m);
            }
            mog_message(m, "Rat hit on GROUND:");       /* LAB_026E */
            a0 = foe(m);
            hurt(m, a1, mog_knight_damage(m, a0));
            return mog_react_script(m, a0, a1);
        }
    } else if (ix_rl(VM, a1 + 14)) {                    /* LAB_0273 : a touché */
        a0 = ix_rl(VM, a1 + 14);
        if (ix_rb(VM, a0 + 77) != 0x24) {
            if (btst8(m, a1 + 104, 0)) {                /* LAB_0278 : agrippe */
                if (btst8(m, MOG_LAB_062B, 5))
                    return rat_flying(m, &r, &f);
                ix_wb(VM, a1 + 104, 0);
                ix_wb(VM, a1 + 104, (uint8_t)(ix_rb(VM, a1 + 104) | 0x20));
                bset8(m, MOG_LAB_062B, 5);
                ix_ww(VM, a1 + 106, (uint16_t)(int16_t)(int8_t)(uint8_t)(ix_rb(VM, a0 + 72) + 6));
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_085B);
                ix_ww(VM, a1 + 6, 0);
                mog_toggle_freeze(m, a0);
                return mog_ctl_return(m);
            }
            if (btst8(m, a1 + 104, 3)) {                /* LAB_0279 */
                ix_ww(VM, a1 + 104, 0);
                bset8(m, a1 + 105, 2);
                bset8(m, MOG_LAB_062B, 3);
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0865);
                mog_toggle_freeze(m, a0);
                return mog_ctl_return(m);
            }
            ix_wl(VM, MOG_v_CtlScript, 0xFFFFFFFFu);
            ix_ww(VM, a1 + 104, 0);
            ix_ww(VM, MOG_LAB_062C, 15);
            if (ix_rb(VM, a0 + 10) == ix_rb(VM, a1 + 10)) {   /* LAB_02C6 */
                uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_05F2));
                if (en) {
                    ix_wb(VM, en + 22, (uint8_t)(ix_rb(VM, en + 22) ^ 2));
                    ix_wb(VM, ix_rl(VM, MOG_LAB_05F2) + 10, ix_rb(VM, en + 22));
                }
            }
            if (ix_rw(VM, a1 + 64) == 8)
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_085A);
            if (ix_rw(VM, a1 + 64) == 4)
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0858);
            return mog_ctl_return(m);
        }
    }

    /* LAB_0252 (A0 : moi, ou l'autre objet quand on vient d'un contact) */
    ix_ww(VM, a0 + 64, 0);
    uint8_t s104 = ix_rb(VM, a1 + 104);
    if ((s104 & 0x80) || (s104 & 1))
        return rat_flying(m, &r, &f);
    if (s104 & 0x20) {                                  /* LAB_0264 : sur le chevalier */
        uint16_t j = poll_joystick(m);
        if ((j & 0x10) && (j & 4)) {                    /* LAB_0266 : secoué */
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_085D);
            ix_wb(VM, a1 + 104, 0);
            bset8(m, a1 + 105, 0);
            return mog_ctl_return(m);
        }
        uint16_t n = (uint16_t)(ix_rw(VM, a1 + 106) - 1);   /* LAB_0265 */
        ix_ww(VM, a1 + 106, n);
        if (n)
            return mog_set_script(m, MOG_LAB_085B);
        ix_wb(VM, a1 + 104, 0);                         /* LAB_0267 */
        bset8(m, a1 + 104, 4);
        return mog_set_script(m, MOG_LAB_085C);
    }
    if (btst8(m, a1 + 105, 0)) {                        /* LAB_026A : lâché */
        mog_toggle_freeze(m, ix_rl(VM, MOG_LAB_05F2));
        uint32_t k = ix_rl(VM, MOG_LAB_05F2);
        release_knight(m, k, ix_rl(VM, k + 26));
        bclr8(m, MOG_LAB_062B, 5);
        return mog_set_script(m, 0);
    }
    if (s104 & 0x10) {                                  /* LAB_0268 : saute du chevalier */
        bclr8(m, MOG_LAB_062B, 5);
        uint32_t t = MOG_LAB_062E;
        a1 = me(m);
        ix_ww(VM, a1 + 106, 0x11);
        ix_wl(VM, t, a1);
        ix_ww(VM, t + 4, ix_rw(VM, a1 + 4));
        ix_ww(VM, t + 6, ix_rw(VM, a1 + 8));
        ix_ww(VM, t + 8, (uint16_t)(ix_rw(VM, a1 + 6) + 0xFFA6));
        ix_ww(VM, t + 10, (uint16_t)(ix_rw(VM, a1 + 4) + 0x96));
        ix_ww(VM, t + 12, ix_rw(VM, a1 + 8));
        ix_ww(VM, t + 14, ix_rw(VM, a1 + 6));
        ix_ww(VM, t + 16, 0x11);
        ix_ww(VM, t + 18, 0x78);
        start_flight(m);
        a1 = me(m);
        ix_ww(VM, a1 + 104, 0);
        bset8(m, a1 + 104, 0);
        ix_wb(VM, a1 + 12, 0);
        ix_ww(VM, a1 + 6, 0xFFBA);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0851);
        uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_05F2));
        ix_wb(VM, en + 22, ix_rb(VM, a1 + 10));
        uint32_t k = ix_rl(VM, MOG_LAB_05F2);
        ix_wl(VM, k + 14, 0);
        ix_wl(VM, k + 18, 0);
        uint32_t s = ix_rl(VM, k + 22);
        uint16_t old = ix_rw(VM, k + 80);
        ix_ww(VM, k + 80, (uint16_t)(old - 5));
        if (!(sw(old) > 5))                             /* BGT après SUBI */
            s = MOG_LAB_07F7;
        mog_restart_entity(m, k, s);
        mog_toggle_freeze(m, foe(m));
        return mog_ctl_return(m);
    }
    if (s104 & 8)                                       /* LAB_025E */
        return rat_waiting(m, &r);
    if (btst8(m, a1 + 105, 2)) {                        /* LAB_0261 : mord */
        uint16_t j = poll_joystick(m);
        if ((j & 0x10) && (j & 4)) {
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0867);
            a1 = me(m);
            uint16_t old = ix_rw(VM, a1 + 80);
            uint16_t d = mog_knight_damage(m, foe(m));
            ix_ww(VM, a1 + 80, (uint16_t)(old - d));
            if (sw(old) > sw(d))                        /* BGT après SUB */
                return mog_ctl_return(m);
            mog_toggle_freeze(m, foe(m));
            release_knight(m, foe(m), MOG_LAB_07FA);
            bclr8(m, MOG_LAB_062B, 3);
            return mog_set_script(m, MOG_LAB_0868);
        }
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0866);       /* LAB_0262 */
        uint32_t k = foe(m);
        uint16_t old = ix_rw(VM, k + 80);
        ix_ww(VM, k + 80, (uint16_t)(old - 1));
        if (!(sw(old) > 1))
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0869);
        return mog_ctl_return(m);
    }

    face_foe(m, &r);
    near_in_depth(m, &r);
    if ((uint16_t)r.d0) {
        if (btst8(m, MOG_LAB_062B, 5) || btst8(m, MOG_LAB_062B, 3))
            return mog_ctl_return(m);
        if (hp(m, foe(m)) <= 0)
            return mog_ctl_return(m);
        if (ix_rw(VM, MOG_LAB_062C)) {
            ix_ww(VM, MOG_LAB_062C, (uint16_t)(ix_rw(VM, MOG_LAB_062C) - 1));
            return mog_ctl_return(m);
        }
        dist_x(m, &r);                                  /* LAB_0253 */
        if (!(sw((uint16_t)r.d0) > 40)) {
            ix_ww(VM, a1 + 64, 8);
            return mog_set_script(m, MOG_LAB_0859);
        }
        if (!(sw((uint16_t)r.d0) > 50)) {
            ix_ww(VM, a1 + 64, 4);
            return mog_set_script(m, MOG_LAB_0857);
        }
    }

    /* LAB_0255 */
    a0 = me(m);
    ix_ww(VM, a0 + 64, 0);
    ix_wb(VM, a0 + 12, 0);
    if (btst8(m, MOG_LAB_062B, 2))
        return rat_jump(m, &r);
    bset8(m, MOG_LAB_062B, 2);
    ix_wb(VM, a0 + 104, (uint8_t)(ix_rb(VM, a0 + 104) | 4));
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_05F4));
    a1 = me(m);
    ix_ww(VM, a1 + 106, 0x1E);
    uint32_t t = MOG_LAB_062E;
    ix_wl(VM, t, a1);
    ix_ww(VM, t + 4, ix_rw(VM, a1 + 4));
    ix_ww(VM, t + 6, ix_rw(VM, a1 + 8));
    ix_ww(VM, t + 8, 0xFFEC);
    ix_ww(VM, t + 10, ix_rw(VM, en + 6));
    ix_ww(VM, t + 12, ix_rw(VM, en + 10));
    ix_ww(VM, t + 14, ix_rw(VM, en + 8));
    ix_ww(VM, t + 12, (uint16_t)(ix_rw(VM, t + 12) + 3));
    ix_ww(VM, t + 16, 14);
    ix_ww(VM, t + 18, 0);
    start_flight(m);
    a0 = me(m);                                         /* LAB_0259 */
    ix_wb(VM, a0 + 104, (uint8_t)(ix_rb(VM, a0 + 104) | 1));
    return mog_set_script(m, MOG_LAB_0850);
}

/* ------------------------------------------------------------------ */
/* LAB_0EC2 (contrôleur 64)                                            */
/* ------------------------------------------------------------------ */

/* LAB_0F34 : éclaboussure (LAB_08BF, contrôleur 40) au point d'impact */
static void splash(MogCombat *m, uint32_t a0)
{
    ix_wl(VM, MOG_t_Controllers + 40, MOG_LAB_02D2);
    ix_spawn(&m->eng, MOG_LAB_08BF, MOG_LAB_0648, sw(ix_rw(VM, a0 + 122)), 0,
             sw(ix_rw(VM, a0 + 124)), ix_rb(VM, ix_rl(VM, MOG_LAB_05F2) + 10), 0x28);
}

/* LAB_0EC6 / LAB_0EC7 : pas horizontal de la table L40_00662 */
static void step_x_0662(MogCombat *m, Regs *r, int left)
{
    walk_sense(m, r);
    step_from(m, r, MOG_L40_00662);
    if (left)
        ix_ww(VM, MOG_LAB_0F3A, (uint16_t)-ix_rw(VM, MOG_LAB_0F3A));
}

static CtlResult creature_0EC2(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_ww(VM, a0 + 62, 0);
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    if (ix_rl(VM, a0 + 14))                             /* LAB_0ECB */
        return mog_set_script(m, ix_rl(VM, a0 + 26));
    if (ix_rl(VM, a0 + 18)) {                           /* LAB_0ECC */
        uint16_t d = mog_knight_damage(m, ix_rl(VM, a0 + 18));
        a0 = me(m);
        hurt(m, a0, d);
        splash(m, a0);
        return mog_set_script(m, MOG_LAB_08AC);
    }
    if (hp(m, ix_rl(VM, MOG_LAB_05F2)) <= 0)
        return mog_ctl_return(m);
    approach(m, &r);
    if (!(uint16_t)r.d0) {                              /* LAB_0EC8 : attaque */
        dist_x(m, &r);
        a0 = me(m);
        int16_t d = sw((uint16_t)r.d0);
        if (!(d < 100) && d < 150) {                    /* LAB_0ECA */
            if (ix_rw(VM, a0 + 64) != 0x20) {
                ix_ww(VM, a0 + 64, 0x20);
                return mog_set_script(m, MOG_LAB_08AB);
            }
        }
        ix_ww(VM, a0 + 64, 8);                          /* LAB_0EC9 */
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08AA);
        ix_wb(VM, MOG_LAB_08D0, ix_rb(VM, a0 + 10));
        ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CC);
        return mog_ctl_return(m);
    }
    r.d6 = 0;
    a0 = me(m);
    if (ix_rb(VM, a0 + 63) & 1)
        step_x_0662(m, &r, 0);
    else if (ix_rb(VM, a0 + 63) & 2)
        step_x_0662(m, &r, 1);
    if (ix_rb(VM, a0 + 63) & 8) {                       /* LAB_0EC4 */
        walk_sense(m, &r);
        ix_ww(VM, MOG_LAB_0F3B, 0xFFFB);
    } else if (ix_rb(VM, a0 + 63) & 4) {
        walk_sense(m, &r);
        ix_ww(VM, MOG_LAB_0F3B, 5);
    }
    return walk(m, &r);
}

/* ------------------------------------------------------------------ */
/* LAB_0EFF (contrôleurs 16 et 56) : chevalier géré par l'ordinateur   */
/* ------------------------------------------------------------------ */

/* Tirage 0-99 comparé au seuil du niveau (LAB_097B[LAB_06C0]) :
 * -1, 0 ou 1 comme CMP.B (octets signés). */
static int vs_level(MogCombat *m, Regs *r)
{
    r->d0 = random100(m);
    int8_t d0 = (int8_t)r->d0;
    int8_t t = (int8_t)ix_rb(VM, MOG_LAB_097B + (uint32_t)(int32_t)sw(ix_rw(VM, MOG_LAB_06C0)));
    return d0 < t ? -1 : d0 > t;
}

static CtlResult attack(MogCombat *m, uint32_t a0, uint32_t atk, uint16_t kind, uint32_t off)
{
    ix_ww(VM, a0 + 64, kind);
    return mog_set_script(m, ix_rl(VM, atk + off));
}

static CtlResult ai_knight(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_LAB_0633, a0);
    uint32_t a1 = a0;
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 22));
    ix_ww(VM, a0 + 62, 0);
    if (a0 == ix_rl(VM, MOG_LAB_05F2))
        ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_v_Combatants + 4));
    else
        ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));

    if (ix_rl(VM, a0 + 18)) {                           /* LAB_0F14 : touché */
        a1 = me(m);
        if (hp(m, a1) > 0) {
            uint32_t att = ix_rl(VM, a1 + 18);
            mog_check_parry(m, a1);
            if (ix_rw(VM, MOG_LAB_01EB))
                return mog_own_attack(m);
            a1 = me(m);                                 /* LAB_0F15 / LAB_0F16 */
            hurt(m, a1, mog_knight_damage(m, att));
            return mog_react_script(m, att, a1);
        }
        return mog_set_script(m, MOG_LAB_07F9);
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_0F17 : a touché */
        uint32_t tgt = ix_rl(VM, a0 + 14);
        uint16_t k = ix_rw(VM, a0 + 64);
        if ((ix_rb(VM, tgt + 77) == 0x0C || k == 0x20 || k == 4)
            && ix_rw(VM, tgt + 64) == 0x1C)             /* LAB_0F19 */
            return mog_set_script(m, 0xFFFFFFFFu);
        return mog_set_script(m, ix_rl(VM, a0 + 26));   /* LAB_0F18 */
    }

    ix_ww(VM, MOG_LAB_0F38, ix_rw(VM, a0 + 64));
    approach(m, &r);
    int engage = 0;
    if (!(uint16_t)r.d0) {
        engage = 1;
    } else if (ix_rw(VM, MOG_LAB_0F3D)) {
        dist_x(m, &r);
        setw(&r.d1, (uint16_t)r.d0);
        engage = 1;
    }
    if (engage) {                                       /* LAB_0F08 */
        a0 = me(m);
        uint32_t atk = ix_rl(VM, a0 + 34);
        uint32_t a2 = foe(m);
        int16_t d = sw((uint16_t)r.d1);
        uint16_t prev = ix_rw(VM, MOG_LAB_0F38);
        if (hp(m, a2) <= 0) {
            ix_wb(VM, a0 + 106, 0);
            if (!(d > 90)) {
                if (!ix_rw(VM, MOG_LAB_0620))
                    return attack(m, a0, atk, 8, 8);
                return mog_ctl_return(m);
            }
            goto move;
        }
        if (!(ix_rb(VM, a0 + 104) & 0x80)                /* LAB_0F0A : parade ? */
            && vs_level(m, &r) >= 0
            && ix_rb(VM, a2 + 10) != ix_rb(VM, a0 + 10)) {
            uint16_t fk = ix_rw(VM, a2 + 64);
            if (fk == 8) {
                dist_x(m, &r);
                if (!(sw((uint16_t)r.d0) > 120))
                    return attack(m, a0, atk, 0x10, 16);
            } else if (fk == 0x20 || fk == 4) {         /* LAB_0F0B */
                dist_x(m, &r);
                if (!(sw((uint16_t)r.d0) > 120))
                    return attack(m, a0, atk, 0x1C, 28);
            }
        }
        if (!(vs_level(m, &r) > 0)) {                       /* LAB_0F0D : bêtise */
            r.d0 = mog_random(m);
            setw(&r.d0, (uint16_t)(r.d0 & 7));
            r.d1 = 0x5A;
            setw(&r.d1, (uint16_t)(0x5A + (uint16_t)r.d0));
            mog_message(m, " Stupid ");
            goto move;
        }
        if (!(d > 90) && prev != 8)                     /* LAB_0F0E */
            return attack(m, a0, atk, 8, 8);
        if (!(d > 95) && prev != 0x20)                  /* LAB_0F0F */
            return attack(m, a0, atk, 0x20, 32);
        if (!(d > 100) && (!ix_rb(VM, a0 + 76) || prev != 4))   /* LAB_0F10 */
            return attack(m, a0, atk, 4, 4);
        if (ix_rb(VM, a0 + 76))                         /* LAB_0F12 : lancer */
            return attack(m, a0, atk, 0x0C, 12);
    }
move:                                                   /* LAB_0F02 */
    a0 = me(m);
    if (!ix_rb(VM, a0 + 63))
        return mog_ctl_return(m);
    ix_wb(VM, a0 + 104, (uint8_t)(ix_rb(VM, a0 + 104) & 0x7F));
    return walk_by_bits(m, &r, MOG_SECSTRT_41, MOG_LAB_0F36, MOG_LAB_0F37, 0);
}

/* ------------------------------------------------------------------ */
/* SECSTRT_40 (contrôleur 4) : Mudmen                                  */
/* ------------------------------------------------------------------ */

/* LAB_0EB6 : état commun des Mudmen (octet de poids fort du mot) */
#define MUD_STATE MOG_LAB_0EB6

static CtlResult mudman(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_ww(VM, a0 + 62, 0);

    if (ix_rl(VM, a0 + 18)) {                           /* LAB_0EB5 : touché */
        uint16_t d = mog_knight_damage(m, ix_rl(VM, a0 + 18));
        ix_wb(VM, a0 + 13, 0x14);
        hurt(m, a0, d);
        return mog_set_script(m, MOG_LAB_08A3);
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_0EB4 : agrippe */
        mog_toggle_freeze(m, ix_rl(VM, a0 + 14));
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_089F);
        bset8(m, MUD_STATE, 0);
        bset8(m, a0 + 104, 0);
        ix_wb(VM, a0 + 13, 0x28);
        return mog_ctl_return(m);
    }
    uint32_t a1 = foe(m);
    if (btst8(m, MUD_STATE, 2))
        return mog_ctl_return(m);

    if (btst8(m, MUD_STATE, 0)) {                       /* LAB_0EAC : tient le chevalier */
        if (!btst8(m, a0 + 104, 0))
            return mog_ctl_return(m);
        uint8_t n = (uint8_t)(ix_rb(VM, a0 + 13) - 1);
        ix_wb(VM, a0 + 13, n);
        if (n) {
            uint16_t j = poll_joystick(m);
            if ((j & 0x10) && (j & 4)) {                /* dégagé */
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08A0);
                bclr8(m, MUD_STATE, 0);
                bclr8(m, a0 + 104, 0);
                bset8(m, MUD_STATE, 6);
                bset8(m, a0 + 104, 6);
                return mog_ctl_return(m);
            }
            return mog_set_script(m, MOG_LAB_089F);     /* LAB_0EAE */
        }
        bclr8(m, MUD_STATE, 0);                         /* LAB_0EAF : englouti */
        bset8(m, MUD_STATE, 1);
        bset8(m, a0 + 104, 1);
        ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);   /* LAB_000D */
        goto swallow;
    }
    if (btst8(m, MUD_STATE, 1)) {
swallow:                                                /* LAB_0EB0 */
        if (btst8(m, a0 + 104, 1)) {
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08A1);
            mog_kill_entity_of(m, foe(m));
            bclr8(m, MUD_STATE, 1);
            bclr8(m, a0 + 104, 1);
        }
        return mog_ctl_return(m);
    }
    if (btst8(m, MUD_STATE, 6)) {                       /* LAB_0EB2 : lâche */
        if (btst8(m, a0 + 104, 6)) {
            bclr8(m, MUD_STATE, 6);
            bclr8(m, a0 + 104, 6);
            mog_toggle_freeze(m, foe(m));
            uint32_t k = foe(m);
            /* l'original passe A0 = chevalier, A1 = script à LAB_030D, qui
             * attend l'inverse : sans effet */
            mog_restart_entity(m, ix_rl(VM, k + 22), k);
            a0 = me(m);
            hurt(m, a0, 1);
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08A3);
        }
        return mog_ctl_return(m);
    }
    if (!btst8(m, a0 + 104, 3)) {                       /* LAB_0EA6 : sous terre */
        uint8_t n = (uint8_t)(ix_rb(VM, a0 + 13) - 1);
        ix_wb(VM, a0 + 13, n);
        if (!n)
            return mog_ctl_return(m);
        ix_ww(VM, a0 + 4, ix_rw(VM, a1 + 4));
        ix_ww(VM, a0 + 8, ix_rw(VM, a1 + 8));
        uint16_t dx = 0x4B;
        ix_wb(VM, a0 + 10, 3);
        if (!(sw(ix_rw(VM, a0 + 4)) < 0xA0)) {
            ix_wb(VM, a0 + 10, 1);
            dx = 0xFFB5;
        }
        ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + dx));
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0897);
        bset8(m, a0 + 104, 3);
        bset8(m, a0 + 104, 4);
        return mog_ctl_return(m);
    }
    if (btst8(m, a0 + 104, 4)) {                        /* LAB_0EAA : surgit */
        near_in_depth(m, &r);
        if ((uint16_t)r.d0) {
            dist_x(m, &r);
            int16_t d = sw((uint16_t)r.d0);
            if (!(d < 20) && !(d > 80)) {
                mog_toggle_freeze(m, foe(m));
                ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08A2);
                bset8(m, MUD_STATE, 2);
                return mog_ctl_return(m);
            }
        }
        bclr8(m, a0 + 104, 4);                          /* LAB_0EAB */
        return mog_set_script(m, MOG_LAB_0899);
    }

    approach(m, &r);
    int16_t d = 0;
    int far = 0;
    if (!(uint16_t)r.d0) {
        d = sw((uint16_t)r.d1);
    } else if (ix_rw(VM, MOG_LAB_0F3D)) {
        dist_x(m, &r);
        setw(&r.d1, (uint16_t)r.d0);
        d = sw((uint16_t)r.d1);
    } else {
        far = 1;
    }
    if (!far && d > 0x32) {                             /* LAB_0E98 */
        if (d < 0x4B) {                                 /* LAB_0EA9 : replonge */
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0898);
            ix_wb(VM, me(m) + 104, 0);
            bset8(m, me(m) + 104, 3);
            bset8(m, me(m) + 104, 4);
            return mog_ctl_return(m);
        }
        if (d < 0x64) {                                 /* LAB_0EA5 */
            near_in_depth(m, &r);
            if ((uint16_t)r.d0)
                return mog_set_script(m, MOG_LAB_089E);
        }
    }

    /* LAB_0E99 : marche */
    a0 = me(m);
    r.d6 = 0;
    r.d7 = 1;
    next_phase(m, &r, a0);
    uint8_t st = ix_rb(VM, a0 + 63);
    if (st & 1) {                                       /* LAB_0EA1 */
        setb(&r.d0, ix_rb(VM, a0 + 12));
        ix_ww(VM, MOG_LAB_0F3A, ix_rw(VM, MOG_L40_0040C + (uint32_t)(int32_t)sw((uint16_t)((uint16_t)r.d0 << 1))));
    }
    if (st & 2) {                                       /* LAB_0EA2 */
        setb(&r.d0, ix_rb(VM, a0 + 12));
        ix_ww(VM, MOG_LAB_0F3A, ix_rw(VM, MOG_L40_0040C + (uint32_t)(int32_t)sw((uint16_t)((uint16_t)r.d0 << 1))));
        ix_ww(VM, MOG_LAB_0F3A, (uint16_t)-ix_rw(VM, MOG_LAB_0F3A));
    }
    if (st & 8)
        ix_ww(VM, MOG_LAB_0F3B, 0xFFFE);
    if (st & 4)
        ix_ww(VM, MOG_LAB_0F3B, 2);
    a0 = me(m);                                         /* LAB_0E9D */
    uint16_t allowed = mog_blocked_dirs(m, a0, ix_rw(VM, MOG_LAB_0F3A), ix_rb(VM, a0 + 10));
    a0 = me(m);
    uint16_t bits = (uint16_t)(ix_rw(VM, a0 + 62) & allowed);
    ix_ww(VM, a0 + 62, bits);
    if (!(bits & 3))
        ix_ww(VM, MOG_LAB_0F3A, 0);
    if (!(bits & 0x0C))
        ix_ww(VM, MOG_LAB_0F3B, 0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a0 + 46) + (uint32_t)ix_rb(VM, a0 + 12) * 4u));
    ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + ix_rw(VM, MOG_LAB_0F3A)));
    ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + ix_rw(VM, MOG_LAB_0F3B)));
    if (!ix_rl(VM, MOG_LAB_0F3A)) {                     /* TST.L : 0F3A et 0F3B */
        ix_wb(VM, a0 + 12, 0);
        ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    }
    return mog_ctl_return(m);
}

/* ------------------------------------------------------------------ */
/* LAB_0ED2 (contrôleur 8) : Démon (et son compagnon LAB_01A2)         */
/* ------------------------------------------------------------------ */

#define DEMON_STATE MOG_LAB_0EEA            /* octet de poids fort */

/* Le compagnon suit le Démon (X et profondeur de l'entité). */
static void demon_sync(MogCombat *m)
{
    uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_01A2));
    if (en) {
        uint32_t a0 = me(m);
        ix_ww(VM, en + 6, ix_rw(VM, a0 + 4));
        ix_ww(VM, en + 10, ix_rw(VM, a0 + 8));
    }
}

/* LAB_0EDA */
static CtlResult demon_done(MogCombat *m)
{
    demon_sync(m);
    return mog_ctl_return(m);
}

/* LAB_0ED5 : pas de 5 selon les bits de 63(moi) */
static CtlResult demon_walk(MogCombat *m, Regs *r)
{
    uint32_t a0 = me(m);
    r->d6 = 0;
    r->d0 = 0;
    r->d1 = 0;
    uint8_t st = ix_rb(VM, a0 + 63);
    if (st & 1)
        setw(&r->d0, 5);
    else if (st & 2)
        setw(&r->d0, 0xFFFB);
    if (st & 8)
        setw(&r->d1, 0xFFFB);
    else if (st & 4)
        setw(&r->d1, 5);
    ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + (uint16_t)r->d0));
    ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + (uint16_t)r->d1));
    return demon_done(m);
}

/* LAB_0EE1 / LAB_0EE3 : fin de prise ; si le chevalier est tenu (bit 6),
 * il est relâché (LAB_07FB). */
static void demon_release(MogCombat *m, uint8_t wait)
{
    if (!btst8(m, DEMON_STATE, 6))
        return;
    uint32_t a0 = ix_rl(VM, MOG_LAB_01A1);
    ix_wb(VM, MOG_LAB_08D0, ix_rb(VM, a0 + 10));
    mog_toggle_freeze(m, ix_rl(VM, MOG_LAB_05F2));
    mog_restart_entity(m, ix_rl(VM, MOG_LAB_05F2), MOG_LAB_07FB);
    bclr8(m, DEMON_STATE, 6);
    ix_wb(VM, a0 + 106, wait);
}

/* LAB_0EE5 / LAB_0EE7 : saisie si le chevalier est à la bonne distance */
static CtlResult demon_grab(MogCombat *m, Regs *r, uint32_t a0, uint32_t script,
                            int16_t lo, int16_t hi, uint32_t grab, uint8_t wait)
{
    ix_wl(VM, MOG_v_CtlScript, script);
    dist_x(m, r);
    int16_t d = sw((uint16_t)r->d0);
    if (!(d > hi) && !(d < lo)) {
        mog_toggle_freeze(m, ix_rl(VM, MOG_LAB_05F2));
        ix_wl(VM, MOG_v_CtlScript, grab);
        bset8(m, DEMON_STATE, 6);
        ix_wb(VM, a0 + 106, wait);
    }
    return demon_done(m);
}

static CtlResult demon(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_ww(VM, a0 + 62, 0);
    demon_sync(m);

    if (ix_rl(VM, a0 + 18)) {                           /* LAB_0EF2 : touché */
        ix_ww(VM, DEMON_STATE, 0);
        mog_message(m, "DEMON STRUCK");
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08BD);
        hurt(m, a0, mog_knight_damage(m, ix_rl(VM, a0 + 18)));
        dist_x(m, &r);
        if (!(sw((uint16_t)r.d0) > 60)) {               /* recule */
            uint32_t k = foe(m);
            a0 = me(m);
            ix_ww(VM, a0 + 4, ix_rw(VM, k + 4));
            uint16_t dx = ix_rb(VM, a0 + 10) == 3 ? 60 : (uint16_t)-60;
            ix_ww(VM, a0 + 4, (uint16_t)(ix_rw(VM, a0 + 4) + dx));
        }
        return demon_done(m);
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_0EF5 : a touché */
        ix_wb(VM, a0 + 106, 5);
        ix_ww(VM, DEMON_STATE, 0);
        mog_message(m, "DEMON HIT");
        ix_wl(VM, MOG_v_CtlScript, 0xFFFFFFFFu);
        return demon_done(m);
    }
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    if (btst8(m, DEMON_STATE, 0)) {                     /* LAB_0EE1 */
        bclr8(m, DEMON_STATE, 0);
        bset8(m, DEMON_STATE, 4);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08B8);
        demon_release(m, 3);
        return demon_done(m);
    }
    if (btst8(m, DEMON_STATE, 3)) {                     /* LAB_0EE3 */
        bclr8(m, DEMON_STATE, 3);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08BA);
        demon_release(m, 5);
        return demon_done(m);
    }
    if (btst8(m, DEMON_STATE, 1)) {                     /* LAB_0EE9 */
        bclr8(m, DEMON_STATE, 1);
        return demon_done(m);
    }
    if (btst8(m, DEMON_STATE, 4)) {                     /* LAB_0EE7 */
        bset8(m, DEMON_STATE, 3);
        bclr8(m, DEMON_STATE, 4);
        return demon_grab(m, &r, a0, MOG_LAB_08B9, 0x82, 0x96, MOG_LAB_08BB, 5);
    }
    if (btst8(m, DEMON_STATE, 5)) {                     /* LAB_0EE5 */
        bset8(m, DEMON_STATE, 0);
        bclr8(m, DEMON_STATE, 5);
        return demon_grab(m, &r, a0, MOG_LAB_08B7, 0x78, 0x8C, MOG_LAB_08BC, 4);
    }
    if (hp(m, foe(m)) <= 0)
        return mog_ctl_return(m);

    approach(m, &r);                                    /* LAB_0ED4 */
    if ((uint16_t)r.d0) {
        if (!ix_rw(VM, MOG_LAB_0F3D))
            return demon_walk(m, &r);
        dist_x(m, &r);
        setw(&r.d1, (uint16_t)r.d0);
    }
    a0 = me(m);                                         /* LAB_0EDC */
    if (ix_rb(VM, a0 + 106)) {
        uint8_t n = (uint8_t)(ix_rb(VM, a0 + 106) - 1);
        ix_wb(VM, a0 + 106, n);
        if (n)
            return demon_walk(m, &r);
    }
    int16_t d = sw((uint16_t)r.d1);
    if (!(d > 100)) {                                   /* LAB_0EDD */
        ix_ww(VM, a0 + 64, 0x20);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08B4);
        ix_wb(VM, MOG_LAB_08D0, ix_rb(VM, a0 + 10));
        ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CC);
        ix_wb(VM, a0 + 106, 6);
        return mog_ctl_return(m);
    }
    if (!(d > 0x82)) {                                  /* LAB_0EDE */
        ix_ww(VM, a0 + 64, 8);
        bset8(m, DEMON_STATE, 1);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08B5);
        ix_wb(VM, a0 + 106, 6);
        return demon_done(m);
    }
    if (!(d > 0x8C)) {                                  /* LAB_0EDF */
        ix_ww(VM, a0 + 64, 4);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_08B6);
        bset8(m, DEMON_STATE, 5);
        ix_wb(VM, MOG_LAB_08D0, ix_rb(VM, a0 + 10));
        ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CD);
        ix_wb(VM, a0 + 106, 5);
    }
    return demon_walk(m, &r);                           /* LAB_0EE0 */
}

/* ------------------------------------------------------------------ */
/* LAB_029F (contrôleur 48) : créature qui bondit sur le chevalier     */
/* ------------------------------------------------------------------ */

#define LEAP_STATE MOG_LAB_0627

static CtlResult leaper(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    Flight f = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    uint32_t a1 = foe(m);

    if (btst8(m, LEAP_STATE, 1)) {                      /* LAB_02A9 : en vol */
        ix_wb(VM, a0 + 105, 0);
        fly(m, me(m), &f);
        a0 = me(m);
        ix_ww(VM, a0 + 4, (uint16_t)f.d1);
        ix_ww(VM, a0 + 8, (uint16_t)f.d2);
        ix_ww(VM, a0 + 6, (uint16_t)f.d3);
        uint16_t left = (uint16_t)(ix_rw(VM, MOG_LAB_062A) - 1);
        ix_ww(VM, MOG_LAB_062A, left);
        if (left) {
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_088B);
            uint16_t half = (uint16_t)(ix_rw(VM, MOG_LAB_0628) >> 1);
            if (sw(half) < sw(left) || sw((uint16_t)f.d3) < -40)
                return mog_ctl_return(m);
            dist_x(m, &r);
            if (sw((uint16_t)r.d0) > 10)
                return mog_ctl_return(m);
            ix_ww(VM, a0 + 4, ix_rw(VM, a1 + 4));       /* écrase le chevalier */
            ix_ww(VM, a0 + 8, ix_rw(VM, a1 + 8));
            mog_restart_entity(m, a1, MOG_LAB_07FD);
            ix_ww(VM, ix_rl(VM, MOG_v_Combatants) + 80, 0xFFFF);
            a0 = me(m);
        }
        ix_wb(VM, a0 + 105, 5);                         /* LAB_02AA : atterrit */
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_088A);
        ix_ww(VM, a0 + 6, 0);
        ix_wb(VM, LEAP_STATE, 0);
        if (sw(ix_rw(VM, MOG_LAB_0629)) < 8) {
            mog_sound(m, 0x2F);                         /* LAB_02B0 */
            return mog_ctl_return(m);
        }
        mog_shake(m);
        mog_sound(m, 0x2D);                             /* LAB_02AF */
        mog_sound(m, 0x2E);
        return mog_ctl_return(m);
    }
    if (ix_rl(VM, a0 + 18)) {                           /* LAB_02B1 : touché */
        hurt(m, a0, mog_knight_damage(m, ix_rl(VM, a0 + 18)));
        splash(m, a0);
        return mog_set_script(m, MOG_LAB_0891);
    }
    if (ix_rl(VM, a0 + 14)) {                           /* LAB_02B2 : a touché */
        uint32_t tgt = ix_rl(VM, a0 + 14);
        uint16_t k = ix_rw(VM, a0 + 64);
        if (k == 0x20) {                                /* LAB_02B4 : saisit */
            mog_toggle_freeze(m, tgt);
            ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0893);
            bset8(m, LEAP_STATE, 0);
            return mog_ctl_return(m);
        }
        return mog_set_script(m, k == 8 ? MOG_LAB_088E : ix_rl(VM, a0 + 26));
    }
    if (btst8(m, LEAP_STATE, 0)) {                      /* LAB_02B5 */
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0894);
        bclr8(m, LEAP_STATE, 0);
        bset8(m, LEAP_STATE, 5);
        return mog_ctl_return(m);
    }
    if (btst8(m, LEAP_STATE, 5)) {                      /* LAB_02B8 : lâche */
        uint32_t k = ix_rl(VM, MOG_LAB_05F2);
        if (hp(m, k) <= 0) {                            /* LAB_02B6 */
            uint16_t t = (uint16_t)(ix_rw(VM, MOG_LAB_062D) ^ 1);
            ix_ww(VM, MOG_LAB_062D, t);
            ix_wl(VM, MOG_v_CtlScript, t ? MOG_LAB_0895 : MOG_LAB_0896);
            ix_wb(VM, LEAP_STATE, 0);
            bset8(m, LEAP_STATE, 6);
            return mog_ctl_return(m);
        }
        ix_wl(VM, k + 14, 0);
        ix_wl(VM, k + 18, 0);
        mog_toggle_freeze(m, k);
        mog_restart_entity(m, k, ix_rl(VM, k + 22));
        uint32_t en = ix_find_entity(&m->eng, ix_rl(VM, MOG_LAB_05F2));
        uint32_t s = me(m);
        uint16_t dx = (ix_rb(VM, s + 10) & 2) ? (uint16_t)-0x4B : 0x4B;
        ix_ww(VM, en + 6, (uint16_t)(ix_rw(VM, s + 4) + dx));
        ix_wb(VM, LEAP_STATE, 0);
        return mog_set_script(m, ix_rl(VM, a0 + 26));
    }
    if (hp(m, a1) <= 0)
        return mog_ctl_return(m);
    ix_ww(VM, MOG_LAB_0625, ix_rw(VM, a1 + 4));
    ix_ww(VM, MOG_LAB_0626, ix_rw(VM, a1 + 8));
    ix_wb(VM, a0 + 10, sw(ix_rw(VM, a0 + 4)) < sw(ix_rw(VM, MOG_LAB_0625)) ? 1 : 3);
    near_in_depth(m, &r);
    if (r.d0) {
        ix_wb(VM, LEAP_STATE, 0);
        dist_x(m, &r);
        int16_t d = sw((uint16_t)r.d0);
        if (!(d > 70)) {
            ix_wb(VM, a0 + 105, 0);
        } else if (!(d > 80)) {                         /* LAB_02A2 */
            if (ix_rw(VM, a0 + 64) != 8) {
                ix_wl(VM, MOG_v_CtlScript, MOG_LAB_088D);
                ix_ww(VM, a0 + 64, 8);
                ix_wb(VM, MOG_LAB_08D0, ix_rb(VM, a0 + 10));
                ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CC);
                return mog_ctl_return(m);
            }
            ix_ww(VM, a0 + 64, 0x20);                   /* LAB_02A4 */
            return mog_set_script(m, MOG_LAB_0890);
        } else if (!(d > 120)) {
            ix_ww(VM, a0 + 64, 0x20);
            return mog_set_script(m, MOG_LAB_0890);
        } else if (!ix_rb(VM, ix_rl(VM, MOG_LAB_05F2) + 76) && !(d > 180)) {
            return mog_ctl_return(m);                   /* LAB_02A5 */
        }
    }
    aim_jump(m, a0, 0x50);                              /* LAB_02A6 : bondit */
    if (!(sw(ix_rw(VM, MOG_LAB_0628)) > 3))
        return mog_ctl_return(m);
    a0 = me(m);
    if (ix_rb(VM, a0 + 105)) {
        uint8_t n = (uint8_t)(ix_rb(VM, a0 + 105) - 1);
        ix_wb(VM, a0 + 105, n);
        if (n)
            return mog_ctl_return(m);
    }
    ix_ww(VM, a0 + 64, 0);                              /* LAB_02A7 */
    uint16_t steps = ix_rw(VM, MOG_LAB_0628);
    if (sw(steps) > 20)
        steps = 20;
    ix_ww(VM, MOG_LAB_062E + 16, steps);
    ix_ww(VM, MOG_LAB_062A, steps);
    ix_ww(VM, MOG_LAB_062E + 18, ix_rw(VM, MOG_LAB_0629));
    start_flight(m);
    ix_wb(VM, LEAP_STATE, 0);
    bset8(m, LEAP_STATE, 1);
    return mog_set_script(m, MOG_LAB_088A);
}

/* ------------------------------------------------------------------ */
/* LAB_027A (contrôleur 20) : Dragon ; LAB_0298 (contrôleur 44)        */
/* ------------------------------------------------------------------ */

#define DRAGON_STATE MOG_LAB_0623

/* LAB_029E : les deux compagnons (LAB_0193, LAB_0194) suivent la
 * profondeur du Dragon. */
static CtlResult dragon_done(MogCombat *m)
{
    uint32_t a2 = ix_rl(VM, MOG_LAB_0193), a3 = ix_rl(VM, MOG_LAB_0194);
    uint16_t d = (uint16_t)(ix_rw(VM, MOG_LAB_0617 + 8) + 10);
    ix_ww(VM, a2 + 8, d);
    ix_ww(VM, a3 + 8, (uint16_t)(d - 30));
    return mog_ctl_return(m);
}

/* LAB_028E : déplacement du Dragon (aussi routine $B0) */
void mog_dragon_move(MogCombat *m)
{
    Regs r = { 0, 0, 0, 0 };
    uint32_t a0 = MOG_LAB_0617;
    ix_ww(VM, a0 + 62, 0);
    ix_wl(VM, MOG_LAB_0633, a0);
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    int low = btst8(m, DRAGON_STATE, 6);
    if (low) {
        ix_ww(VM, MOG_L00_06960, ix_rw(VM, a0 + 116));
        ix_ww(VM, MOG_LAB_0295, ix_rw(VM, a0 + 118));
        ix_ww(VM, a0 + 116, 2);
        ix_ww(VM, a0 + 118, 1);
    }
    approach(m, &r);
    a0 = me(m);
    ix_wb(VM, a0 + 10, 1);
    uint16_t st = ix_rw(VM, a0 + 62);
    if (st & 1) {
        uint16_t x = (uint16_t)(ix_rw(VM, a0 + 4) + 5);
        if (sw(x) < 100)
            ix_ww(VM, a0 + 4, x);
    }
    if (st & 2) {
        uint16_t x = (uint16_t)(ix_rw(VM, a0 + 4) - 5);
        if (sw(x) > 30)
            ix_ww(VM, a0 + 4, x);
    }
    if (st & 8)
        ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) - 5));
    if (st & 4)
        ix_ww(VM, a0 + 8, (uint16_t)(ix_rw(VM, a0 + 8) + 5));
    uint32_t en = ix_find_entity(&m->eng, me(m));       /* LAB_0293 */
    uint32_t a1 = me(m);
    ix_ww(VM, en + 6, ix_rw(VM, a1 + 4));
    ix_ww(VM, en + 10, ix_rw(VM, a1 + 8));
    if (low) {                                          /* (écrit dans l'entité) */
        ix_ww(VM, en + 116, ix_rw(VM, MOG_L00_06960));
        ix_ww(VM, en + 118, ix_rw(VM, MOG_LAB_0295));
    }
}

/* Envol : trajectoire vers (X = 100, profondeur du chevalier) */
static CtlResult dragon_takeoff(MogCombat *m, uint32_t walk_off, uint16_t steps,
                                uint16_t vz, uint16_t height)
{
    uint32_t a1 = MOG_LAB_0617;
    ix_wb(VM, a1 + 12, 0);
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a1 + 46) + walk_off));
    uint32_t a2 = foe(m), t = MOG_LAB_062E;
    ix_ww(VM, a1 + 106, steps);
    ix_wl(VM, t, a1);
    ix_ww(VM, t + 4, ix_rw(VM, a1 + 4));
    ix_ww(VM, t + 6, ix_rw(VM, a1 + 8));
    ix_ww(VM, t + 8, ix_rw(VM, a1 + 6));
    ix_ww(VM, t + 10, 100);
    ix_ww(VM, t + 12, ix_rw(VM, a2 + 8));
    ix_ww(VM, t + 14, vz);
    ix_ww(VM, t + 16, steps);
    ix_ww(VM, t + 18, height);
    start_flight(m);
    return dragon_done(m);
}

/* LAB_0297 : flamme (LAB_0886, contrôleur 40) devant le Dragon */
static void dragon_fire(MogCombat *m)
{
    ix_wl(VM, MOG_t_Controllers + 40, MOG_LAB_02D2);
    uint32_t a1 = MOG_LAB_0617;
    ix_spawn(&m->eng, MOG_LAB_0886, MOG_LAB_05E0, (int16_t)(ix_rw(VM, a1 + 4) + 0x37), 0,
             (int16_t)(ix_rw(VM, a1 + 8) + 5), 1, 0x28);
}

static CtlResult dragon(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    Flight f = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_wl(VM, MOG_LAB_0633, a0);
    uint32_t a1 = a0;
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));

    if (ix_rl(VM, a1 + 18)) {                           /* LAB_0287 : touché */
        uint32_t att = ix_rl(VM, a1 + 18);
        uint8_t c = ix_rb(VM, att + 77);
        if (c == 0x0C || c == 0x34) {
            bset8(m, DRAGON_STATE, 7);
            if (c == 0x0C) {                            /* LAB_0289 */
                hurt(m, a1, mog_knight_damage(m, att));
                ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a1 + 30)
                                                     + (uint32_t)(int32_t)sw(ix_rw(VM, att + 64))));
            } else {                                    /* LAB_028B */
                ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, ix_rl(VM, a1 + 30) + 12));
                hurt(m, a1, 3);
            }
            splash(m, MOG_LAB_0617);                    /* LAB_028A */
            return mog_ctl_return(m);
        }
        a1 = ix_find_entity(&m->eng, MOG_LAB_0617);
        if (ix_rb(VM, a1 + 1))                          /* LAB_0288 */
            return mog_set_script(m, 0xFFFFFFFFu);
    } else if (ix_rl(VM, a1 + 14)) {                    /* LAB_028C : a touché */
        if (ix_rw(VM, a1 + 64) == 4) {                  /* LAB_028D */
            mog_kill_entity_of(m, ix_rl(VM, a1 + 14));
            return mog_set_script(m, MOG_LAB_0884);
        }
        return mog_set_script(m, 0xFFFFFFFFu);
    } else {
        ix_ww(VM, a1 + 64, 0);
    }

    /* LAB_027B */
    if (btst8(m, DRAGON_STATE, 4)) {                    /* LAB_027D : en vol */
        uint16_t n = (uint16_t)(ix_rw(VM, a1 + 106) - 1);
        ix_ww(VM, a1 + 106, n);
        if (!n)
            bclr8(m, DRAGON_STATE, 4);
        uint32_t k = foe(m);                            /* LAB_027E */
        uint16_t d = ix_rw(VM, a1 + 8);
        ix_ww(VM, a1 + 8, (uint16_t)(sw(d) < sw(ix_rw(VM, k + 8)) ? d + 5 : d - 5));
        fly(m, a1, &f);                                 /* LAB_0280 */
        a1 = MOG_LAB_0617;
        ix_ww(VM, a1 + 4, (uint16_t)f.d1);
        ix_ww(VM, a1 + 6, (uint16_t)f.d3);
        uint8_t ph = (uint8_t)(ix_rb(VM, a1 + 12) + 1);
        ix_wb(VM, a1 + 12, ph);
        if (!((int8_t)ph < 8))
            ph = 7;
        uint32_t wt = ix_rl(VM, a1 + 46) + (uint32_t)ph * 4u;
        ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, wt + (btst8(m, DRAGON_STATE, 5) ? 32 : 64)));
        return dragon_done(m);
    }
    mog_dragon_move(m);
    a1 = me(m);
    dist_x(m, &r);
    ix_ww(VM, MOG_LAB_0624, (uint16_t)r.d0);
    if (!(sw((uint16_t)r.d0) >= 0x8C)) {
        if (!btst8(m, DRAGON_STATE, 5)) {               /* s'envole (haut) */
            bset8(m, DRAGON_STATE, 4);
            bset8(m, DRAGON_STATE, 5);
            return dragon_takeoff(m, 32, 13, 0xFFBA, 0x50);
        }
    } else if (btst8(m, DRAGON_STATE, 5)) {             /* LAB_027C : (bas) */
        bset8(m, DRAGON_STATE, 4);
        bclr8(m, DRAGON_STATE, 5);
        return dragon_takeoff(m, 64, 9, 0xFFE2, 0x78);
    }

    /* LAB_0283 : attaques */
    bclr8(m, DRAGON_STATE, 6);
    if (hp(m, foe(m)) <= 0 || !ix_rw(VM, MOG_LAB_0F3D))
        return dragon_done(m);
    if (!btst8(m, DRAGON_STATE, 5)) {                   /* LAB_0286 */
        ix_ww(VM, a1 + 64, 8);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0872);
        return dragon_done(m);
    }
    if (!(sw(ix_rw(VM, MOG_LAB_0624)) > 70) && !btst8(m, DRAGON_STATE, 7)) {
        bclr8(m, DRAGON_STATE, 7);                      /* LAB_0285 */
        ix_ww(VM, a1 + 64, 4);
        ix_wl(VM, MOG_v_CtlScript, MOG_LAB_0883);
        return dragon_done(m);
    }
    bclr8(m, DRAGON_STATE, 7);                          /* LAB_0284 : feu */
    ix_ww(VM, a1 + 64, 0x20);
    ix_wl(VM, MOG_v_CtlScript, MOG_LAB_087E);
    bset8(m, DRAGON_STATE, 6);
    dragon_fire(m);
    return dragon_done(m);
}

/* LAB_0298 (contrôleur 44) */
static CtlResult dragon_part(MogCombat *m, uint32_t a0)
{
    Regs r = { 0, 0, 0, 0 };
    ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a0 + 22));
    ix_wl(VM, MOG_LAB_0633, a0);
    uint32_t a1 = a0;
    ix_wl(VM, MOG_LAB_0634, ix_rl(VM, MOG_LAB_05F2));
    uint32_t k = foe(m);
    if (ix_rl(VM, a1 + 18))                             /* LAB_029B */
        return mog_set_script(m, ix_rl(VM, a1 + 22));
    if (ix_rl(VM, a1 + 14)) {                           /* LAB_029C */
        if (ix_rb(VM, ix_rl(VM, a1 + 14) + 77) != 0x14) {
            ix_wb(VM, MOG_LAB_08D0, 1);
            ix_wl(VM, MOG_LAB_08CE, MOG_LAB_08CC);
            ix_wl(VM, MOG_v_CtlScript, ix_rl(VM, a1 + 22));
        }
        return mog_ctl_return(m);
    }
    if (hp(m, MOG_LAB_0617) <= 0)
        return mog_set_script(m, MOG_LAB_0887);
    if (hp(m, k) > 0) {                                 /* LAB_0299 */
        near_in_depth(m, &r);
        if ((uint16_t)r.d0 && !(sw(ix_rw(VM, k + 4)) > 100)) {
            ix_ww(VM, a1 + 64, 0x14);
            return mog_set_script(m, MOG_LAB_0881);
        }
    }
    return dragon_done(m);
}

/* ------------------------------------------------------------------ */

int mog_ai_controller(MogCombat *m, uint32_t fn, uint32_t obj, CtlResult *out)
{
    switch (fn) {
    case MOG_LAB_0226: *out = passing_knight(m, obj); return 1;
    case MOG_LAB_0236: *out = creature_0236(m, obj); return 1;
    case MOG_LAB_0251: *out = ratman(m, obj); return 1;
    case MOG_LAB_0EC2: *out = creature_0EC2(m, obj); return 1;
    case MOG_LAB_0EFF: *out = ai_knight(m, obj); return 1;
    case MOG_SECSTRT_40: *out = mudman(m, obj); return 1;
    case MOG_LAB_0ED2: *out = demon(m, obj); return 1;
    case MOG_LAB_029F: *out = leaper(m, obj); return 1;
    case MOG_LAB_027A: *out = dragon(m, obj); return 1;
    case MOG_LAB_0298: *out = dragon_part(m, obj); return 1;
    case MOG_LAB_0DCF: return mog_map_dragon_ctl(m, obj, out);
    }
    return 0;
}
