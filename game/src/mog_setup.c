/*
 * mog_setup.c — entrée des adversaires en cours de combat (LAB_0174 et
 * les routines « adversaire suivant » de LAB_05F0), équipement des
 * créatures (routines de LAB_05F1), traduits de amiga_asm/mog.asm.
 */
#include "mog_private.h"
#include "ix_mog_names.h"

#include <stdio.h>

#define VM (m->eng.vm)

static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }

/* Équipement commun : tables de l'objet et caractéristiques. */
typedef struct {
    uint32_t attacks, scripts, damage, walk;      /* 34, 30, 42, 46 (0 = inchangé) */
    uint8_t  ctl, port;                           /* 77, 11 (port 0 = inchangé) */
} Kit;

static void apply(MogCombat *m, uint32_t a1, const Kit *k)
{
    if (k->attacks) wl(m, a1 + 34, k->attacks);
    if (k->scripts) wl(m, a1 + 30, k->scripts);
    if (k->damage)  wl(m, a1 + 42, k->damage);
    if (k->walk)    wl(m, a1 + 46, k->walk);
    wl(m, a1 + 38, MOG_t_BankEnemy);
    wb(m, a1 + 77, k->ctl);
    if (k->port)
        wb(m, a1 + 11, k->port);
}

/* Routine d'équipement (LAB_05F1) pour l'objet a1. 0 si non portée. */
static int creature_setup(MogCombat *m, uint32_t fn, uint32_t a1)
{
    switch (fn) {
    case MOG_LAB_0166:                                  /* RTS */
        return 1;
    case MOG_LAB_0169: {                                /* Troggs */
        Kit k = { MOG_t_TroggAAttacks, MOG_t_TroggAScripts, MOG_t_TroggADamage, MOG_t_TroggAWalk, 0x18, 4 };
        apply(m, a1, &k);
        wl(m, a1 + 22, MOG_x_TroggAStand);
        wl(m, a1 + 26, MOG_x_TroggAStand);
        ww(m, a1 + 116, 0x64);
        ww(m, a1 + 118, 0x5A);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 80, 0x14);
        ww(m, a1 + 84, 0x14);
        return 1;
    }
    case MOG_LAB_0170: {
        Kit k = { MOG_t_TroggBAttacks, MOG_t_TroggBScripts, MOG_t_TroggBDamage, MOG_t_TroggBWalk, 0x1C, 4 };
        apply(m, a1, &k);
        wl(m, a1 + 22, MOG_x_TroggBStand);
        wl(m, a1 + 26, MOG_x_TroggBStand);
        ww(m, a1 + 116, 0x46);
        ww(m, a1 + 118, 0x41);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 80, 0x14);
        ww(m, a1 + 84, 0x14);
        return 1;
    }
    case MOG_LAB_0176: {
        wl(m, a1 + 30, MOG_t_TroggSpearScripts);
        wl(m, a1 + 46, MOG_t_TroggSpearWalk);
        wl(m, a1 + 38, MOG_t_BankEnemy);
        wb(m, a1 + 77, 0x20);
        wb(m, a1 + 11, 4);
        wl(m, a1 + 22, MOG_x_TroggSpearStand);
        wl(m, a1 + 26, MOG_x_TroggSpearStand);
        ww(m, a1 + 116, 0x82);
        ww(m, a1 + 118, 0x78);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 80, 0x0F);
        ww(m, a1 + 84, 0x0F);
        return 1;
    }
    case MOG_LAB_018B:
        wl(m, a1 + 30, MOG_t_PassingKnightScripts);
        wl(m, a1 + 46, MOG_t_PassingKnightWalk);
        wl(m, a1 + 38, MOG_t_BankEnemy);
        wb(m, a1 + 77, 0);
        wb(m, a1 + 11, 4);
        wl(m, a1 + 26, MOG_x_PassingKnightRecoil);
        wl(m, a1 + 22, MOG_x_PassingKnightStand);
        ww(m, a1 + 80, 0x0A);
        ww(m, a1 + 84, 0x0A);
        ww(m, a1 + 116, 2);
        ww(m, a1 + 120, 0x0A);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 118, 1);
        return 1;
    case MOG_LAB_018F: {                                /* hommes-rats */
        wl(m, a1 + 30, MOG_t_RatmenScripts);
        wl(m, a1 + 46, MOG_t_RatmenWalk);
        wl(m, a1 + 42, MOG_t_RatmenDamage);
        wl(m, a1 + 38, MOG_t_BankEnemy);
        wb(m, a1 + 77, 0x24);
        wb(m, a1 + 11, 4);
        wl(m, a1 + 22, MOG_x_RatmenStand);
        wl(m, a1 + 26, MOG_x_RatmenStand);
        ww(m, a1 + 80, 5);
        ww(m, a1 + 84, 5);
        ww(m, a1 + 116, 0x28);
        ww(m, a1 + 118, 0x1E);
        ww(m, a1 + 120, 5);
        uint32_t a4 = MOG_t_RatmenDamage;
        wl(m, a4 + 8, 1);
        wl(m, a4 + 4, 3);
        uint16_t level = ix_rw(VM, MOG_v_Combatants + 18);
        if (level == 0x2D) {
            ww(m, a1 + 80, 7);
            ww(m, a1 + 84, 7);
            wl(m, a4 + 8, 3);
            wl(m, a4 + 4, 6);
        }
        if (level == 0x31) {                            /* LAB_0190 */
            ww(m, a1 + 80, 0x0C);
            ww(m, a1 + 84, 0x0C);
            wl(m, a4 + 8, 5);
            wl(m, a4 + 4, 8);
        }
        return 1;
    }
    case MOG_LAB_0198:
        wl(m, a1 + 38, MOG_t_BankEnemy);
        wl(m, a1 + 22, MOG_x_BalokStand);
        wl(m, a1 + 26, MOG_x_BalokRecoil);
        wl(m, a1 + 42, MOG_t_BalokDamage);
        wb(m, a1 + 77, 0x30);
        ww(m, a1 + 80, 0x1E);
        ww(m, a1 + 84, 0x1E);
        wb(m, a1 + 10, 1);
        ww(m, a1 + 120, 0x0A);
        ww(m, a1 + 118, 0x3C);
        ww(m, a1 + 116, 0x50);
        return 1;
    case MOG_LAB_019D:
        wl(m, a1 + 46, MOG_t_MudmenWalk);
        wl(m, a1 + 42, MOG_t_MudmenDamage);
        wl(m, a1 + 38, MOG_t_BankEnemy);
        wl(m, a1 + 30, MOG_t_MudmenScripts);
        wl(m, a1 + 22, MOG_x_MudmenStand);
        wl(m, a1 + 26, MOG_x_MudmenStand);
        ww(m, a1 + 80, 0x1E);
        ww(m, a1 + 84, 0x1E);
        wb(m, a1 + 77, 4);
        wb(m, a1 + 11, 4);
        ww(m, a1 + 116, 0x50);
        ww(m, a1 + 118, 0x4B);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 104, 0);
        return 1;
    case MOG_LAB_019F:
        wl(m, a1 + 22, MOG_x_TrollStand);
        wl(m, a1 + 26, MOG_x_TrollStand);
        wl(m, a1 + 46, MOG_t_TrollWalk);
        wl(m, a1 + 30, MOG_t_TrollScripts);
        wl(m, a1 + 42, MOG_t_TrollDamage);
        wl(m, a1 + 38, MOG_t_BankEnemy);
        ww(m, a1 + 80, 0x28);
        ww(m, a1 + 84, 0x28);
        wb(m, a1 + 77, 0x40);
        wb(m, a1 + 11, 4);
        ww(m, a1 + 116, 0x96);
        ww(m, a1 + 120, 5);
        ww(m, a1 + 118, 0x5A);
        return 1;
    }
    return 0;
}

/* LAB_0171 : premier des 20 objets libres, marqué occupé. S'il n'y en a
 * pas, l'adresse qui suit le 20e (non marquée), comme l'original. */
uint32_t mog_alloc_object(MogCombat *m)
{
    uint32_t a1 = ix_rl(VM, MOG_v_Objects);
    for (int i = 0; i < 20; i++, a1 += IX_OBJECT_SIZE)
        if (!ix_rl(VM, a1)) {
            wl(m, a1, 1);
            return a1;
        }
    return a1;
}

/* LAB_01A5-LAB_01A7 : profondeurs d'entrée (trois rangs) */
static uint16_t entry_depth(MogCombat *m, int rank)
{
    uint16_t v = ix_rw(VM, MOG_LAB_0A98);
    uint16_t d = (uint16_t)(200 - v);
    uint16_t r;
    uint32_t var;
    if (rank == 3) {
        r = (uint16_t)((d >> 1) + v - 0x2F);
        var = MOG_v_EntryDepth3;
    } else if (rank == 2) {
        r = (uint16_t)((d >> 2) + v - 0x2F);
        var = MOG_v_EntryDepth2;
    } else {
        uint16_t t = (uint16_t)(d >> 1);
        r = (uint16_t)(t + (t >> 1) + v - 0x2F);
        var = MOG_v_EntryDepth1;
    }
    ww(m, var, r);
    return r;
}

/* LAB_01A9 : profondeur d'entrée (rangs 3, 2, 1 à tour de rôle), puis
 * entité de l'objet a1 sur `script`. */
void mog_enter_object_with(MogCombat *m, uint32_t a1, uint32_t script)
{
    uint16_t n;
    do {
        n = (uint16_t)((ix_rw(VM, MOG_LAB_01AD) + 1) & 3);
        ww(m, MOG_LAB_01AD, n);
    } while (!n);
    ww(m, a1 + 8, entry_depth(m, n));
    ix_start_entity(&m->eng, script, a1, ix_rl(VM, a1 + 38),
                    (int16_t)ix_rw(VM, a1 + 4), (int16_t)ix_rw(VM, a1 + 6),
                    (int16_t)ix_rw(VM, a1 + 8), ix_rb(VM, a1 + 10), ix_rb(VM, a1 + 77));
}

/* LAB_01A8 : idem sur le script de repos 22(objet). */
void mog_enter_object(MogCombat *m, uint32_t a1)
{
    mog_enter_object_with(m, a1, ix_rl(VM, a1 + 22));
}

/* LAB_0174 : nouvel adversaire décrit par l'enregistrement `rec`
 * (X, hauteur, profondeur, direction). */
int mog_spawn_opponent(MogCombat *m, uint32_t rec)
{
    ww(m, MOG_v_FoesEntered, (uint16_t)(ix_rw(VM, MOG_v_FoesEntered) + 1));
    uint32_t a1 = mog_alloc_object(m);
    ww(m, a1 + 4, ix_rw(VM, rec));
    ww(m, a1 + 6, ix_rw(VM, rec + 2));
    ww(m, a1 + 8, ix_rw(VM, rec + 4));
    wb(m, a1 + 10, (uint8_t)ix_rw(VM, rec + 6));
    uint32_t fn = ix_rl(VM, MOG_v_FoeKitFn);
    if (!creature_setup(m, fn, a1)) {
        char t[64];
        snprintf(t, sizeof t, "LAB_05F1 non portée : %08X", fn);
        mog_message(m, t);
        m->errors++;
        return 0;
    }
    mog_enter_object(m, a1);
    return 1;
}

/* Deux positions d'entrée en alternance (LAB_05EF) */
static int spawn_alternating(MogCombat *m, uint32_t table)
{
    uint16_t t = (uint16_t)(ix_rw(VM, MOG_v_EntrySide) ^ 1);
    ww(m, MOG_v_EntrySide, t);
    return mog_spawn_opponent(m, t ? table + 8 : table);
}

/* Routine « adversaire suivant » de LAB_05F0. 0 si non portée. */
int mog_next_opponent(MogCombat *m, uint32_t fn)
{
    switch (fn) {
    case MOG_LAB_0166: return 1;                        /* RTS (duel) */
    case MOG_LAB_016B: return spawn_alternating(m, MOG_t_EntriesA);
    case MOG_LAB_0189: return spawn_alternating(m, MOG_t_EntriesB);
    case MOG_LAB_018D:
    case MOG_LAB_019B: return spawn_alternating(m, MOG_t_EntriesC);
    case MOG_LAB_0197: return mog_spawn_opponent(m, MOG_LAB_0199);
    }
    char t[64];
    snprintf(t, sizeof t, "LAB_05F0 non portée : %08X", fn);
    mog_message(m, t);
    m->errors++;
    return 0;
}
