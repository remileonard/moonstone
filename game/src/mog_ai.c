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
static CtlResult walk_by_bits(MogCombat *m, Regs *r, uint32_t th, uint32_t tu, uint32_t td)
{
    uint32_t a0 = me(m);
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
    return walk_by_bits(m, &r, MOG_LAB_024E, MOG_LAB_024F, MOG_LAB_0250);
}

int mog_ai_controller(MogCombat *m, uint32_t fn, uint32_t obj, CtlResult *out)
{
    switch (fn) {
    case MOG_LAB_0226: *out = passing_knight(m, obj); return 1;
    case MOG_LAB_0236: *out = creature_0236(m, obj); return 1;
    }
    return 0;
}
