/*
 * mog_loop.c — boucle de combat de mog (Combat_Run [LAB_0036], Combat_Loop
 * [LAB_0037]) et services communs aux boucles (écrans, attentes),
 * traduits de amiga_asm/mog.asm.
 *
 * Deux usages : mog_combat_run fait tout le combat comme l'original
 * (dessins dans les écrans de mog, attentes par mog_wait_vbls) ;
 * mog_combat_begin / mog_combat_frame laissent l'affichage à l'hôte
 * (mog_fight.c).
 */
#include "mog_private.h"
#include "ix_mog_names.h"
#include "mog_vbl.h"
#include "mog_screens.h"

#ifdef MOG_VBL_TRACE
#include <stdio.h>
#include <stdlib.h>
#endif

#define VM (m->eng.vm)

static void set_planes(MogCombat *m, uint32_t d0)       /* L00_0908E */
{
    for (uint32_t i = 0; i < 5; i++, d0 += 0x1F40)
        ix_wl(VM, MOG_t_DestPlanes + 4 * i, d0);
}

/* LAB_0D71 + LAB_0416 : attente de la VBL, écran dessiné montré (copper),
 * écrans et piles de zones à restaurer échangés, dessins vers LAB_0D92. */
void mog_swap_screens(MogCombat *m)
{
    mog_wait_vbls(m, 1);                                /* LAB_0D77 */
    uint32_t s = ix_rl(VM, MOG_v_DrawPlanes);
    for (uint32_t p = 0; p < 5; p++) {
        uint32_t d0 = s + p * 0x1F40;
        ix_ww(VM, MOG_COPPER_BPL + 8 * p, (uint16_t)(d0 >> 16));
        ix_ww(VM, MOG_COPPER_BPL + 8 * p + 4, (uint16_t)d0);
    }
    ix_wl(VM, MOG_v_DrawPlanes, ix_rl(VM, MOG_v_ShowPlanes));
    ix_wl(VM, MOG_v_ShowPlanes, s);
    uint32_t a = ix_rl(VM, MOG_v_RestoreFront);
    ix_wl(VM, MOG_v_RestoreFront, ix_rl(VM, MOG_v_RestoreBack));
    ix_wl(VM, MOG_v_RestoreBack, a);
    ix_wl(VM, MOG_v_RestoreNext, ix_rl(VM, MOG_v_RestoreFront));
    set_planes(m, ix_rl(VM, MOG_v_DrawPlanes));
    ix_ww(VM, MOG_v_RestoreCount, 0);
}

/* LAB_0D07 : copie d'un rectangle (blitter) */
static void blit_copy(MogCombat *m, uint32_t a0, uint32_t a1, uint16_t amod, uint16_t dmod,
                      uint16_t w, uint16_t h)
{
    MogBlitter *b = &m->blt;
    b->afwm = b->alwm = 0xFFFF;
    b->apt = a0;
    b->dpt = a1;
    b->amod = (int16_t)amod;
    b->dmod = (int16_t)dmod;
    b->con0 = 0x09F0;
    b->con1 = 0;
    mog_blitter_run(VM, b, (uint16_t)(h << 6 | w));
}

/* LAB_03A2 : zone (x, y, l, h) recopiée du décor LAB_05C0 dans LAB_0D92 */
static void restore_area(MogCombat *m, uint32_t a6)
{
    int16_t d0 = (int16_t)ix_rw(VM, a6), d1 = (int16_t)ix_rw(VM, a6 + 2);
    if (d1 >= 0xC8)
        return;
    int16_t d2 = (int16_t)(ix_rw(VM, a6 + 4) + d0);
    d0 = (int16_t)((d0 >> 3) & ~1);
    if (d0 >= 0x28)
        return;
    d2 = (int16_t)((d2 >> 3) & ~1);
    if (d2 < 0)
        return;
    d2 = (int16_t)((int16_t)(d2 - d0 + 2) >> 1);
    int16_t d3 = (int16_t)ix_rw(VM, a6 + 6);
    if ((int16_t)(d3 + d1) <= 0)
        return;
    if (d0 < 0) {
        d2 = (int16_t)(d2 + (d0 >> 1));
        if (d2 <= 0)
            return;
        d0 = 0;
    }
    int16_t d4 = (int16_t)(d0 + d2 + d2 - 0x28);        /* LAB_03A3 */
    if (d4 > 0) {
        d2 = (int16_t)(d2 - (d4 >> 1));
        if (d2 <= 0)
            return;
    }
    if (d1 < 0) {                                       /* LAB_03A4 */
        d3 = (int16_t)(d3 + d1);
        if (d3 <= 0)
            return;
        d1 = 0;
    }
    d4 = (int16_t)(d1 + d3 - 0xC8);                     /* LAB_03A5 */
    if (d4 > 0) {
        d3 = (int16_t)(d3 - d4);
        if (d3 <= 0)
            return;
    }
    uint16_t off = (uint16_t)((uint16_t)d1 * 40u + (uint16_t)d0);   /* LAB_03A6 */
    uint32_t a0 = ix_rl(VM, MOG_v_BgPlanes) + (uint32_t)(int32_t)(int16_t)off;
    uint32_t a1 = ix_rl(VM, MOG_v_DrawPlanes) + (uint32_t)(int32_t)(int16_t)off;
    uint16_t mod = (uint16_t)(40 - d2 - d2);
    for (int p = 0; p < 5; p++, a0 += 8000, a1 += 8000)
        blit_copy(m, a0, a1, mod, mod, (uint16_t)d2, (uint16_t)d3);
}

/* LAB_039E : zones notées à l'image d'avant restaurées (au plus 45) */
void mog_restore_areas(MogCombat *m)
{
    ix_wl(VM, MOG_v_SpawnCount, 0);
    ix_wl(VM, MOG_v_DrawScreen, ix_rl(VM, MOG_v_DrawPlanes));
    uint32_t a6 = ix_rl(VM, MOG_v_RestoreFront);
    while (ix_rw(VM, a6 + 4) != 0xFFFF && ix_rl(VM, MOG_v_SpawnCount) != 0x2D
           && ix_rw(VM, a6 + 6) && ix_rw(VM, a6 + 4)) {
        if (m->planes)
            restore_area(m, a6);
        a6 += 8;
        ix_wl(VM, MOG_v_SpawnCount, ix_rl(VM, MOG_v_SpawnCount) + 1);
    }
}

/* LAB_0042 : couleurs de pulsation du chevalier a1 (SECSTRT_1...) */
static void hp_colours(MogCombat *m, uint32_t a1)
{
    static const uint16_t c[5][3] = {
        { 0x00C, 0x009, 0x006 }, { 0xFA0, 0xE70, 0xC50 }, { 0xAE8, 0x6B5, 0x473 },
        { 0xD00, 0x900, 0x500 }, { 0x408, 0x305, 0x003 },
    };
    uint32_t k = ix_rl(VM, a1 + 54);
    const uint16_t *v = c[k <= 3 ? k : 4];
    for (int i = 0; i < 3; i++)
        ix_ww(VM, MOG_t_HpColours + 2u * (unsigned)i, v[i]);
}

/* Combat_LowHpIndicator [LAB_003E] : PV <= 10, couleurs du chevalier qui
 * pulsent (6-8 le joueur, 9-11 le chevalier adverse) */
static void low_hp(MogCombat *m)
{
    static const uint32_t var[2][3] = {
        { MOG_v_LowHpGlowA, MOG_v_LowHpGlowB, MOG_v_LowHpGlowC },
        { MOG_v_FoeLowHpGlowA, MOG_v_FoeLowHpGlowB, MOG_v_FoeLowHpGlowC },
    };
    for (int w = 0; w < 2; w++) {
        if (w == 1 && ix_rl(VM, MOG_v_PaletteKind) != 12 && ix_rl(VM, MOG_v_PaletteKind) != 16)
            break;
        if (ix_rl(VM, var[w][0]))
            continue;
        uint32_t a1 = ix_rl(VM, MOG_v_Combatants + 4u * (unsigned)w);
        if ((int16_t)ix_rw(VM, a1 + 80) > 10)
            continue;
        hp_colours(m, a1);
        for (int i = 0; i < 3; i++)
            ix_wl(VM, var[w][i], mog_glow(VM, (uint16_t)(6 + 3 * w + i),
                                          ix_rw(VM, MOG_t_HpColours + 2u * (unsigned)i), 1, 0));
    }
}

/* LAB_0048 : pulsations des PV arrêtées */
static void low_hp_off(MogCombat *m)
{
    static const uint32_t var[2][3] = {
        { MOG_v_LowHpGlowA, MOG_v_LowHpGlowB, MOG_v_LowHpGlowC },
        { MOG_v_FoeLowHpGlowA, MOG_v_FoeLowHpGlowB, MOG_v_FoeLowHpGlowC },
    };
    for (int w = 0; w < 2; w++)
        if (ix_rl(VM, var[w][0]))
            for (int i = 0; i < 3; i++)
                ix_wl(VM, ix_rl(VM, var[w][i]), 0);
}

/* LAB_0B82 : clavier remis à zéro */
static void clear_keys(MogCombat *m)
{
    for (uint32_t i = 0; i < 128; i++)
        ix_wb(VM, MOG_t_KeysDown + i, 0);
    ix_ww(VM, MOG_v_KeyPressed, 0);
}

/* Combat_CheckEnd [LAB_004B] : joueur à terre -> fin dans 50 images ;
 * espace : pause jusqu'à la touche suivante. */
static void check_end(MogCombat *m)
{
    uint32_t a2 = MOG_v_Combatants;
    if (ix_rb(VM, a2 + 8) && (int16_t)ix_rw(VM, ix_rl(VM, a2) + 80) < 0) {
        ix_wb(VM, a2 + 16, 0x32);
        ix_wb(VM, a2 + 8, 0);
    }
    if (!m->planes)
        return;
    uint16_t key = ix_rw(VM, MOG_v_KeyPressed);
    if (ix_rb(VM, MOG_t_KeyChars + key) == 0x20) {       /* LAB_0D8D */
        clear_keys(m);
        while (!ix_rw(VM, MOG_v_KeyPressed) && m->wait_vbl)
            mog_wait_vbls(m, 1);
    }
    clear_keys(m);
}

/* Combat_Run [LAB_0036] : mise en route. */
void mog_combat_begin(MogCombat *m)
{
    ix_ww(VM, MOG_v_EnemyActed, 0);
    ix_ww(VM, MOG_v_SoundSeqPos, 0);
    ix_wl(VM, MOG_v_LowHpGlowA, 0);
    ix_wl(VM, MOG_v_FoeLowHpGlowA, 0);
    clear_keys(m);                                      /* LAB_0B82 */
    ix_wb(VM, MOG_v_Combatants + 8, 1);
    ix_wl(VM, MOG_v_PlayerObj, ix_rl(VM, MOG_v_Combatants));
    for (uint32_t i = 0; i < 0x78; i++)                 /* LAB_02F2 : trajectoires */
        ix_wb(VM, MOG_t_Trajectories + i, 0);
    ix_ww(VM, MOG_v_CombatStarted, 1);
    ix_wl(VM, MOG_v_CombatStartVbl, ix_rl(VM, MOG_v_VblCounter));
    mog_swap_screens(m);                                /* LAB_0416 */
    uint32_t obj = ix_rl(VM, MOG_v_Objects);             /* LAB_000A : tout gelé */
    for (int i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (obj != ix_rl(VM, MOG_v_DemonCompanion))
            mog_toggle_freeze(m, obj);
    mog_toggle_freeze(m, MOG_v_DragonObj);
    if (ix_rb(VM, MOG_v_CreatureLoaded) == 4) {                 /* LAB_0412 */
        mog_message(m, "Turning on colour glow");
        uint16_t c[32];
        for (int i = 0; i < 32; i++)
            c[i] = ix_rw(VM, MOG_t_PalCombat + 2u * (unsigned)i);
        mog_wait_vbls(m, 1);                            /* LAB_0D8A */
        if (m->palette)
            m->palette(m->out.user, c);
        uint32_t cur = ix_rl(VM, MOG_v_PalCurrent);         /* LAB_03EE */
        for (uint32_t i = 0; i < 64; i++)
            ix_wb(VM, cur + i, ix_rb(VM, MOG_t_PalCombat + i));
        ix_wl(VM, MOG_v_CombatGlow, mog_glow(VM, 14, 0x100, 2, 0));
    }
}

/* Fin de Combat_Loop : combat actif, puis encore 16(v_Combatants) images */
static int still_running(MogCombat *m)
{
    uint32_t a2 = MOG_v_Combatants;
    if (ix_rb(VM, a2 + 8))
        return 1;
    uint8_t n = (uint8_t)(ix_rb(VM, a2 + 16) - 1);
    ix_wb(VM, a2 + 16, n);
    return n != 0;
}

int mog_combat_frame(MogCombat *m)
{
    mog_frame_start(m);
    mog_run_controllers(m);
    ix_run_entities(&m->eng);
    mog_swap_screens(m);
    mog_collisions(m);
    mog_restore_areas(m);
    check_end(m);
    return still_running(m);
}

/* Combat_CheckKO [LAB_000E] : à terre, une vie de moins et PV rendus
 * (LAB_05DC bit 0 : le premier, bit 1 : le second) */
static void check_ko(MogCombat *m)
{
    ix_ww(VM, MOG_v_KnightsDown, 0);
    uint32_t a0 = ix_rl(VM, MOG_v_Combatants), a1 = ix_rl(VM, MOG_v_Combatants + 4);
    if (!((int16_t)ix_rw(VM, a0 + 80) > 0)) {
        ix_wb(VM, MOG_v_KnightsDown, ix_rb(VM, MOG_v_KnightsDown) | 1);
        ix_ww(VM, a0 + 80, ix_rw(VM, a0 + 84));
        ix_wb(VM, a0 + 73, (uint8_t)(ix_rb(VM, a0 + 73) - 1));
    }
    if (!((int16_t)ix_rw(VM, a1 + 80) > 0)) {
        ix_ww(VM, a1 + 80, ix_rw(VM, a1 + 84));
        ix_wb(VM, a1 + 73, (uint8_t)(ix_rb(VM, a1 + 73) - 1));
        ix_wb(VM, MOG_v_KnightsDown, ix_rb(VM, MOG_v_KnightsDown) | 2);
    }
}

void mog_combat_run(MogCombat *m)
{
    int planes = m->planes;
    m->planes = 1;
    mog_combat_begin(m);
    do {                                                /* Combat_Loop */
        mog_frame_start(m);
        mog_run_controllers(m);
        ix_run_entities(&m->eng);
        mog_swap_screens(m);
        mog_collisions(m);
        mog_restore_areas(m);
        low_hp(m);
        check_end(m);
        mog_frame_wait(m);
    } while (still_running(m));
    ix_ww(VM, MOG_v_SndMuted, 0);                         /* LAB_0AA9 */
    for (int i = 0; i < 4; i++)
        mog_sound(m, 0xA7);
    check_ko(m);
    if (ix_rb(VM, MOG_v_CreatureLoaded) == 4)                   /* LAB_0114 : LAB_0413 */
        ix_wl(VM, ix_rl(VM, MOG_v_CombatGlow), 0);
    low_hp_off(m);                                      /* LAB_0048 */
    ix_ww(VM, MOG_v_MovesUsed, ix_rw(VM, MOG_v_MovesMax));
    ix_ww(VM, MOG_v_ReversedOn, 0);
    mog_fade_out(m);                                    /* LAB_03F1 */
    mog_reset_entities(m);                              /* LAB_0305 */
    if (ix_rl(VM, MOG_v_TerrainMode) == 2)
        ix_ww(VM, ix_rl(VM, MOG_v_Lair) + 6, ix_rw(VM, MOG_v_FoesToBeat));
    m->planes = planes;
}

void mog_idle(MogCombat *m)
{
    if (m->frame_start)
        m->frame_start(m->out.user);
    if (m->idle)
        m->idle(m->out.user);
}

void mog_frame_start(MogCombat *m)
{
    if (m->frame_start)
        m->frame_start(m->out.user);
    ix_wl(VM, MOG_v_FrameStartVbl, ix_rl(VM, MOG_v_VblCounter));
}

void mog_wait_vbls(MogCombat *m, unsigned n)
{
#ifdef MOG_VBL_TRACE
    if (n && getenv("MOG_VBLTRACE"))
        printf("W %u\n", n);
#endif
    while (n--) {
        if (m->wait_vbl)
            m->wait_vbl(m->out.user);
        else
            ix_wl(VM, MOG_v_VblCounter, ix_rl(VM, MOG_v_VblCounter) + 1);
        mog_screen_vbl(m);                              /* LAB_057D (serveur) */
    }
}

void mog_frame_wait(MogCombat *m)
{
    int32_t d0 = (int32_t)(ix_rl(VM, MOG_v_VblCounter) - ix_rl(VM, MOG_v_FrameStartVbl));
    int32_t d1 = (int16_t)ix_rw(VM, MOG_v_FrameVbls) - d0;
    if (d1 < 0)
        d1 = 0;
    mog_wait_vbls(m, (unsigned)d1);
}
