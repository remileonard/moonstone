/*
 * moon_combat.c — Combat mode for Moonstone
 *
 * Implements the real-time combat as described in DOC_MODE_COMBAT.md.
 *
 * Combat states (LAB_068F):
 *   1 = PvP knight vs knight
 *   2 = PvE knight vs creature
 *   3 = Mystic (Mythral)
 *   5 = Arena in city
 *  10 = Valley of Gods (boss)
 *
 * Controls — joystick moves the knight when fire is NOT held.
 * When fire IS held, joystick direction selects the attack (numpad layout,
 * facing right; horizontal axes invert when facing left per DOC_MODE_COMBAT §5):
 *
 *   NW (7) + fire → Blocage (Block)
 *   N  (8) + fire → Défense spéciale (Special defense)
 *   NE (9) + fire → Coup en avant (Forward blow — long range)
 *   W  (4) + fire → Coup vers l'arrière (Backward blow)
 *   E  (6) + fire → Balancement (Swing — sweep forward)
 *   SW (1) + fire → Lancer le couteau (Throw knife)
 *   S  (2) + fire → Coup de hache (Axe blow — 2× damage, slow)
 *   SE (3) + fire → Coup vers le haut (Upward blow)
 *
 * When the knight faces LEFT the horizontal axes are inverted so that
 * "forward" always points toward the opponent (LAB_057D §5, "le 1 devient 3").
 *
 * HP stagger: when HP ≤ 10 the knight vacille (staggers) — a periodic
 * wobble is applied and incoming hits trigger the CSTATE_STAGGER state
 * (DOC_MODE_COMBAT §7).
 *
 * Sprite assets (DOC_MODE_COMBAT.md §3.3, DOC_MODE_OVERWORLD.md §1.2):
 *   dw1.cel   — knight combatant sprites (53 frames)
 *   da1.cel   — damage / attack overlays (52 frames)
 *   au1.cel   — enemy/creature sprites (92 frames)
 *   co1.cel   — complementary icons / effects (25 frames)
 *   kn*.ob    — per-faction knight sprites (may not be available)
 *   ch.piv    — combat background
 */

#include "moon_combat.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Combat constants                                                    */
/* ------------------------------------------------------------------ */

/* Arena Y bounds (no jumping — movement is planar, like original ASM) */
#define ARENA_Y_MIN      90   /* topmost allowed Y in the arena        */
#define ARENA_Y_MAX      170  /* bottommost allowed Y in the arena     */
#define COMBAT_GROUND_Y  150  /* default Y start position              */

/* Movement speed (matches LAB_057D: ±2 per VBL tick)                 */
#define MOVE_SPEED       2

/* Attack ranges per move type — horizontal pixel distance             */
#define RANGE_FORWARD   40   /* Coup en avant, Swing                  */
#define RANGE_BACKWARD  36   /* Coup vers l'arrière                   */
#define RANGE_UP        32   /* Coup vers le haut                     */
#define RANGE_KNIFE     60   /* Lancer le couteau (projectile range)  */
#define RANGE_AXE       28   /* Coup de hache (short, powerful)       */
#define RANGE_BLOCK      0   /* Blocage — no attack                   */
#define RANGE_SPECIAL    0   /* Défense spéciale — no attack          */

/* Damage per attack type                                              */
#define DMG_SWING       10
#define DMG_AXE         20   /* 2× base damage per manual             */
#define DMG_FORWARD     12
#define DMG_BACKWARD    12
#define DMG_UP          10
#define DMG_KNIFE        8   /* projectile, requires daggers          */

/* State durations (frames)                                            */
#define DUR_SWING       14
#define DUR_AXE         24   /* slow windup per manual                */
#define DUR_FORWARD     12
#define DUR_BACKWARD    12
#define DUR_UP          12
#define DUR_KNIFE       10
#define DUR_BLOCK       18
#define DUR_SPECIAL     14
#define DUR_HIT          8
#define DUR_STAGGER     10   /* brief stagger after hit at low HP     */

/* Low-HP threshold that triggers stagger (LAB_0032 §7)               */
#define LOW_HP_THRESHOLD 10

/* ------------------------------------------------------------------ */
/* Attack type — determined by joystick direction + fire              */
/* ------------------------------------------------------------------ */

typedef enum {
    ATTACK_NONE    = 0,
    ATTACK_SWING,      /* E (right/forward) + fire: Balancement       */
    ATTACK_AXE,        /* S (down)          + fire: Coup de hache     */
    ATTACK_FORWARD,    /* NE                + fire: Coup en avant      */
    ATTACK_BACKWARD,   /* W (left/backward) + fire: Coup vers arrière */
    ATTACK_UP,         /* SE                + fire: Coup vers le haut */
    ATTACK_KNIFE,      /* SW (back+down)    + fire: Lancer le couteau */
    ATTACK_BLOCK,      /* NW (back+up)      + fire: Blocage           */
    ATTACK_SPECIAL,    /* N (up)            + fire: Défense spéciale  */
} AttackType;

/* ------------------------------------------------------------------ */
/* Combatant state                                                     */
/* ------------------------------------------------------------------ */

typedef enum {
    CSTATE_IDLE    = 0,
    CSTATE_WALK,
    CSTATE_ATTACK,
    CSTATE_BLOCK,
    CSTATE_HIT,
    CSTATE_STAGGER,  /* low-HP wobble (HP ≤ LOW_HP_THRESHOLD)        */
    CSTATE_DEAD
} CombatState;

/* Number of valid states for array sizing                            */
#define CSTATE_COUNT 7

typedef struct {
    int          hp;
    int          max_hp;
    int          x;          /* screen X of combatant                */
    int          y;          /* screen Y of combatant                */
    int          facing;     /* 1 = right, -1 = left                 */
    CombatState  state;
    int          state_timer;/* frames remaining in current state     */
    AttackType   attack_type;/* current attack (valid while attacking) */
    int          hit_flash;  /* frames to show hit flash             */
    int          stagger_tick;/* oscillation counter for low-HP wobble*/
    int          human;      /* 1 = player-controlled                */
    int          knight_idx; /* index into ctx->knights[]            */
    const char  *name;
    /* Hitbox collision fields — used by the collide.hit engine       */
    const char    *cel_name; /* sprite filename key for collide.hit  */
    const MoonCel *cel;      /* current CEL (non-owning pointer)     */
    int            is_knight;/* 1 = uses s_knight_ranges             */
} Combatant;

/* ------------------------------------------------------------------ */
/* Combat backgrounds                                                  */
/* ------------------------------------------------------------------ */

/*
 * combat_bg_for_node — return the PIV background file for a combat.
 *
 * Per DOC_MODE_COMBAT §2.1 (LAB_012C in mog.asm), ALL combats load
 * "ch.piv" as the universal combat background.  The original game had
 * no per-node PIV — visual variety came from palette indices applied
 * over ch.piv.  We try that canonical name first; callers fall back
 * gracefully if the file is absent.
 *
 * node_type 0x1b/0x1c have dedicated backgrounds in the doc, so keep
 * the specialised names for those states.
 */
static const char *combat_bg_for_node(int node_type)
{
    switch (node_type) {
    case 0x1b:             return "MYS.piv";
    case 0x1c:             return "bg4.piv";
    default:               return "ch.piv";   /* universal per LAB_012C */
    }
}

/* ------------------------------------------------------------------ */
/* Creature type → asset mapping (LAB_08C8, mog.asm §3.3)            */
/* ------------------------------------------------------------------ */

/*
 * creature_cel_for_type — primary CEL/OB filename for the given
 * creature type index (= byte offset into LAB_08C8 handler table).
 *
 * Complete mapping confirmed by tracing the LAB_08C8 fill (lines 4345-4358)
 * and each handler to its sprite-loading routine and filename DC.B strings
 * (LAB_0775..LAB_07B5).  Primary sprite (used here) + secondary sprite listed:
 *
 *   0x00 → be1.c / be2.c                    Trogg War Beast  LAB_0188→LAB_0123
 *   0x04 → Mudmen1.cel / Mudmen2.cel         Mudmen           LAB_019A→LAB_011E
 *   0x08 → Demon1.cel / Demon2..4.cel        Demon/Gardien    LAB_01A0→LAB_0125
 *   0x0c → He1.ob / He2.ob / He3.ob          Enemy Knight     LAB_0164→LAB_0116
 *   0x10 → He1.ob / He2.ob / He3.ob          Enemy Knight     LAB_0164→LAB_0116
 *   0x14 → Dragon1.cel / Dragon2.cel         Dragon           LAB_0192→LAB_0121
 *   0x18 → TroggAxe1.cel / TroggAxe2.cel     TroggAxe (fort)  LAB_0168→LAB_011A
 *   0x1c → TroggAxe1.cel / TroggAxe2.cel     TroggAxe (faible)LAB_016A→LAB_011A
 *   0x20 → TroggSpear1.cel / TroggSpear2.cel TroggSpear       LAB_0175→LAB_0118
 *   0x24 → Ratmen1.cel / Ratmen2.cel         Ratmen           LAB_018C→LAB_011C
 *   0x30 → Balok1.cel / Balok2.cel / Balok3  Balok            LAB_0196→LAB_011F
 *   0x38 → He1.ob / He2.ob / He3.ob          Enemy Knight     LAB_0164→LAB_0116
 *   0x40 → Troll1.cel / Troll2.cel           Troll            LAB_019E→LAB_0126
 *
 * Note: il n'existe PAS de TroggHammer dans le jeu original.  Les variantes
 * Trogg du code assembleur sont uniquement : War Beast (0x00), Axe fort (0x18),
 * Axe faible (0x1c) et Spear (0x20).  Les entrées 0x1c et 0x18 chargent les
 * mêmes sprites (TroggAxe1/2.cel) ; la seule différence est les HP de départ
 * (LAB_0169 : 100/90 PV pour 0x18 ; LAB_0170 : 70/65 PV pour 0x1c).
 * Les trois entrées EnemyKnight (0x0c, 0x10, 0x38) pointent toutes vers le
 * même gestionnaire LAB_0164 — c'est le design original du programmeur.
 */
static const char *creature_cel_for_type(int ctype)
{
    switch (ctype) {
    case 0x00: return "be1.c";
    case 0x04: return "Mudmen1.cel";
    case 0x08: return "Demon1.cel";
    case 0x0c: return "He1.ob";
    case 0x10: return "He1.ob";
    case 0x14: return "Dragon1.cel";
    case 0x18: return "TroggAxe1.cel";
    case 0x1c: return "TroggAxe1.cel";  /* mêmes sprites que 0x18, HP inférieurs (LAB_0170) */
    case 0x20: return "TroggSpear1.cel";
    case 0x24: return "Ratmen1.cel";
    case 0x30: return "Balok1.cel";
    case 0x38: return "He1.ob";
    case 0x40: return "Troll1.cel";
    default:   return "Ratmen1.cel";    /* ne devrait pas être atteint */
    }
}

/*
 * creature_name_for_type — display name matching the creature type,
 * aligned with the PveNode name strings in moon_overworld.c.
 */
static const char *creature_name_for_type(int ctype)
{
    switch (ctype) {
    case 0x00: return "TROGG WAR BEAST";
    case 0x04: return "MUDMEN";
    case 0x08: return "DEMON";
    case 0x0c: return "ENEMY KNIGHT";
    case 0x10: return "ENEMY KNIGHT";
    case 0x14: return "DRAGON";
    case 0x18: return "TROGGAXE";
    case 0x1c: return "TROGGAXE";
    case 0x20: return "TROGGSPEAR";
    case 0x24: return "RATMEN";
    case 0x30: return "BALOK";
    case 0x38: return "ENEMY KNIGHT";
    case 0x40: return "TROLL";
    default:   return "UNKNOWN";        /* ne devrait pas être atteint */
    }
}

/* ------------------------------------------------------------------ */
/* Multi-enemy wave system                                             */
/* ------------------------------------------------------------------ */

/*
 * Maximum number of simultaneous enemies on screen — matches the
 * maximum spawn config from LAB_01BC/LAB_01BD (3 creatures).
 */
#define MAX_COMBAT_ENEMIES  3

/*
 * Difficulty tables based on knight strength (1–5).
 *
 * total_to_kill : how many creatures must die to win the fight.
 *   Faithful to LAB_01B7's spawn types (1, 3, or 3) weighted toward
 *   higher totals at higher levels.
 *
 * simultaneous : how many enemies are on screen at once.
 *   LAB_01BC spawns 3 simultaneously; at low levels only 1 at a time.
 */
static int pve_total_to_kill(int strength)
{
    if (strength <= 1) return 1;
    if (strength <= 2) return 2;
    if (strength <= 3) return 3;
    if (strength <= 4) return 4;
    return 5;
}

static int pve_simultaneous(int strength)
{
    if (strength <= 2) return 1;
    if (strength <= 4) return 2;
    return 3;
}

/*
 * spawn_enemy — initialise a new enemy combatant at the right-side
 * spawn point.  Slightly randomise X start to distinguish waves.
 */
static void spawn_enemy(Combatant *e, int spawn_idx,
                        const char *name, int base_hp)
{
    memset(e, 0, sizeof(*e));
    e->hp         = base_hp;
    e->max_hp     = base_hp;
    /* Spread enemies horizontally so they don't stack */
    e->x          = 220 + spawn_idx * 20;
    e->y          = COMBAT_GROUND_Y;
    e->facing     = -1;
    e->state      = CSTATE_IDLE;
    e->human      = 0;
    e->name       = name;
}

/* ------------------------------------------------------------------ */
/* Combat sprite state                                                 */
/* ------------------------------------------------------------------ */

/*
 * Frame ranges within dw1.cel (53 frames) for each combat state.
 *
 * dw1.cel is the generic knight sprite sheet available on the game disk.
 * The exact animation layout is inferred from typical 2D combat games:
 *   0-5   : idle (standing)
 *   6-11  : walk
 *   12-17 : attack (sword swing / forward attack)
 *   18-20 : upward / aerial attack frames
 *   21-23 : block
 *   24-27 : hit reaction / stagger
 *   28-32 : death
 *
 * au1.cel (92 frames) is used for enemies/creatures:
 *   0-7   : idle
 *   8-19  : walk
 *   20-31 : attack
 *   32-39 : hit / stagger
 *   40-52 : death
 */
typedef struct {
    int first;  /* first frame in this state's animation */
    int count;  /* number of frames in this state's animation */
} FrameRange;

static const FrameRange s_knight_ranges[] = {
    /* CSTATE_IDLE    */ { 0,  6 },
    /* CSTATE_WALK    */ { 6,  6 },
    /* CSTATE_ATTACK  */ { 12, 6 },
    /* CSTATE_BLOCK   */ { 21, 3 },
    /* CSTATE_HIT     */ { 24, 4 },
    /* CSTATE_STAGGER */ { 24, 4 },  /* reuse hit frames for stagger */
    /* CSTATE_DEAD    */ { 28, 5 },
};

static const FrameRange s_creature_ranges[] = {
    /* CSTATE_IDLE    */ { 0,  8 },
    /* CSTATE_WALK    */ { 8,  12 },
    /* CSTATE_ATTACK  */ { 20, 12 },
    /* CSTATE_BLOCK   */ { 0,  8 },
    /* CSTATE_HIT     */ { 32, 8 },
    /* CSTATE_STAGGER */ { 32, 8 },  /* reuse hit frames for stagger */
    /* CSTATE_DEAD    */ { 40, 13 },
};
#define NUM_STATES_KNIGHT   ((int)(sizeof(s_knight_ranges)   / sizeof(s_knight_ranges[0])))
#define NUM_STATES_CREATURE ((int)(sizeof(s_creature_ranges) / sizeof(s_creature_ranges[0])))

/* Per-combatant animation sub-frame (index within current state's range).
 * Index 0 = player, indices 1..MAX_COMBAT_ENEMIES = enemy slots.    */
#define ANIM_SLOTS  (1 + MAX_COMBAT_ENEMIES)
static int s_anim_frame[ANIM_SLOTS];
static int s_anim_tick [ANIM_SLOTS];
#define COMBAT_ANIM_SPEED  4  /* ticks per animation frame */

/* Parsed collide.hit data — loaded once at combat start, freed at end.
 * NULL when the file is absent (fallback to geometric hit checks).   */
static MoonHit *s_hit = NULL;

/* ------------------------------------------------------------------ */
/* Draw a combatant using CEL sprite (or rectangle fallback)          */
/* ------------------------------------------------------------------ */

static void draw_combatant_cel(uint32_t *fb,
                                const Combatant *c,
                                int combatant_idx,
                                const MoonCel *cel,
                                const uint32_t *palette,
                                int is_knight,
                                uint32_t fallback_color)
{
    /* Advance animation sub-frame */
    s_anim_tick[combatant_idx]++;
    if (s_anim_tick[combatant_idx] >= COMBAT_ANIM_SPEED) {
        s_anim_tick[combatant_idx] = 0;
        s_anim_frame[combatant_idx]++;
    }

    /* Determine frame range for current state */
    int state_idx = (int)c->state;
    const FrameRange *ranges    = is_knight ? s_knight_ranges   : s_creature_ranges;
    int               num_states = is_knight ? NUM_STATES_KNIGHT : NUM_STATES_CREATURE;
    if (state_idx < 0 || state_idx >= num_states)
        state_idx = CSTATE_IDLE;

    const FrameRange *rng = &ranges[state_idx];

    /* Clamp sub-frame to range; freeze on last frame when dead */
    if (c->state == CSTATE_DEAD) {
        s_anim_frame[combatant_idx] =
            (rng->count > 0) ? (rng->count - 1) : 0;
    } else if (rng->count > 0) {
        s_anim_frame[combatant_idx] %= rng->count;
    } else {
        s_anim_frame[combatant_idx] = 0;
    }

    int frame_idx = rng->first + s_anim_frame[combatant_idx];

    /* Low-HP stagger: apply a small horizontal wobble so the knight
     * "vacille" (staggers) as described in the game manual and
     * DOC_MODE_COMBAT §7.  The wobble is a ±2px oscillation driven by
     * the combatant's stagger_tick counter. */
    int stagger_dx = 0;
    if (c->hp > 0 && c->hp <= LOW_HP_THRESHOLD) {
        /* sin-like: +2, +2, 0, -2, -2, 0, ... over 6 ticks */
        int phase = ((int)c->stagger_tick / 4) % 6;
        stagger_dx = (phase < 2) ? 2 : (phase < 4) ? -2 : 0;
    }

    if (cel && cel->frame_count > 0 && frame_idx < cel->frame_count) {
        /* Centre sprite horizontally on c->x, align bottom to c->y */
        const MoonCelFrame *fr = &cel->frames[frame_idx];
        int sw = (int)fr->width;
        int sh = (int)fr->height;
        int dx = c->x + stagger_dx - sw / 2;
        int dy = c->y - sh;

        /* Mirror left-facing combatants */
        int flags = BLIT_MASK;
        if (c->facing < 0) flags |= BLIT_FLIP_X;

        /* Additive white flash on hit */
        if (c->hit_flash > 0)
            flags |= BLIT_ADDITIVE;

        render_cel_frame(fr, palette, fb, dx, dy, flags);
    } else {
        /* Fallback: coloured rectangle */
        int w = 16, h = 32;
        int x = c->x + stagger_dx - w / 2;
        int y = c->y - h;

        render_fill_rect(fb, x, y, w, h, fallback_color);
        render_fill_rect(fb, x + 2, y - 12, 12, 12, fallback_color);

        if (c->hit_flash > 0)
            render_fill_rect(fb, x - 2, y - 14, w + 4, h + 14, 0x88FFFFFFu);
    }
}

/* ------------------------------------------------------------------ */
/* Combat logic helpers                                                */
/* ------------------------------------------------------------------ */

/* Forward declaration — defined later in this file */
static int check_hit(const Combatant *attacker, const Combatant *defender);

/*
 * combatant_abs_frame — compute the absolute CEL frame index for a
 * combatant given its current state and animation sub-frame counter.
 * slot = 0 for player, 1..MAX for enemies.
 */
static int combatant_abs_frame(const Combatant *c, int slot)
{
    const FrameRange *ranges     = c->is_knight ? s_knight_ranges : s_creature_ranges;
    int               num_states = c->is_knight ? NUM_STATES_KNIGHT : NUM_STATES_CREATURE;
    int               st         = (int)c->state;

    if (st < 0 || st >= num_states)
        st = CSTATE_IDLE;
    const FrameRange *rng = &ranges[st];

    int sub = s_anim_frame[slot];
    if (c->state == CSTATE_DEAD) {
        sub = (rng->count > 0) ? rng->count - 1 : 0;
    } else if (rng->count > 0) {
        sub %= rng->count;
    } else {
        sub = 0;
    }
    return rng->first + sub;
}

/*
 * cel_pixel_hit — test whether pixel (px, py) in Amiga planar bitplane
 * data of 'fr' is non-transparent (at least one bitplane has a set bit).
 *
 * Replicates the LAB_03E7 pixel test from mog.asm.
 * px/py are relative to the sprite's top-left corner.
 */
static int cel_pixel_hit(const MoonCelFrame *fr, int px, int py)
{
    if (!fr || !fr->data) return 0;
    int w = (int)fr->width;
    int h = (int)fr->height;
    if (px < 0 || px >= w || py < 0 || py >= h) return 0;

    /* Amiga planar format: each row is padded to a 16-bit boundary.  */
    int row_bytes  = ((w + 15) / 16) * 2;
    int plane_bytes = row_bytes * h;
    int byte_off   = py * row_bytes + (px >> 3);
    int bit_mask   = 1 << (7 - (px & 7));   /* MSB = leftmost pixel  */

    for (int p = 0; p < (int)fr->planes; p++) {
        if (fr->data[p * plane_bytes + byte_off] & bit_mask)
            return 1;
    }
    return 0;
}

/*
 * check_hit_hitdata — pixel-accurate hitbox collision from collide.hit.
 *
 * Mirrors assembly routine LAB_03DB / LAB_03E1–LAB_03EA in mog.asm.
 *
 * Parameters:
 *  atk           — attacker combatant
 *  atk_cel_name  — sprite key used to look up collide.hit entry
 *  atk_slot      — animation slot index (0=player, 1+n=enemy n)
 *  def           — defender combatant
 *  def_slot      — defender animation slot index
 *
 * Returns:
 *   1  — collision detected
 *   0  — no collision
 *  -1  — collide.hit data unavailable for this sprite (caller: use fallback)
 */
static int check_hit_hitdata(const Combatant *atk, int atk_slot,
                              const Combatant *def, int def_slot)
{
    if (!s_hit || !atk->cel || !def->cel) return -1;

    /* Locate attacker hitbox data in collide.hit */
    const MoonHitSprite *hs = moon_hit_find(s_hit, atk->cel_name);
    if (!hs) return -1;

    /* Absolute CEL frame index for attacker and defender */
    int atk_fi = combatant_abs_frame(atk, atk_slot);
    int def_fi = combatant_abs_frame(def, def_slot);

    if (atk_fi < 0 || atk_fi >= hs->frame_count) return -1;
    const MoonHitFrame *hf = &hs->frames[atk_fi];
    if (hf->n_points == 0) return 0;   /* frame has no hitbox points  */

    /* Defender frame dimensions */
    if (def_fi < 0 || def_fi >= def->cel->frame_count) return -1;
    const MoonCelFrame *def_fr = &def->cel->frames[def_fi];
    int dw = (int)def_fr->width;
    int dh = (int)def_fr->height;

    /* Top-left corner of each sprite (c->x is horizontal centre,
     * c->y is bottom edge).                                           */
    int atk_tl_x = atk->x - (int)atk->cel->frames[atk_fi].width / 2;
    int atk_tl_y = atk->y - (int)atk->cel->frames[atk_fi].height;

    int def_tl_x = def->x - dw / 2;
    int def_tl_y = def->y - dh;

    /* Is the attacker horizontally flipped (facing left)?            */
    int flipped   = (atk->facing < 0);
    int flip_w    = (int)atk->cel->frames[atk_fi].width;

    /* Broad-phase AABB test (LAB_03E1–LAB_03E3):
     * attacker hitbox bounding box vs defender sprite bounding box.  */
    int atk_min_x, atk_max_x;
    if (!flipped) {
        atk_min_x = atk_tl_x;
        atk_max_x = atk_tl_x + (int)hf->max_dx;
    } else {
        atk_min_x = atk_tl_x + flip_w - (int)hf->max_dx;
        atk_max_x = atk_tl_x + flip_w;
    }
    int atk_min_y = atk_tl_y;
    int atk_max_y = atk_tl_y + (int)hf->max_dy;

    /* No overlap → early out */
    if (atk_max_x < def_tl_x || atk_min_x > def_tl_x + dw) return 0;
    if (atk_max_y < def_tl_y || atk_min_y > def_tl_y + dh) return 0;

    /* Fine-phase: per-point pixel test (LAB_03E4–LAB_03EA)          */
    for (int i = 0; i < (int)hf->n_points; i++) {
        int dx = (int)hf->points[i].dx;
        int dy = (int)hf->points[i].dy;

        if (flipped)
            dx = flip_w - dx;

        int abs_x = atk_tl_x + dx;
        int abs_y = atk_tl_y + dy;

        /* Must be inside the defender sprite bounding box */
        if (abs_x < def_tl_x || abs_x >= def_tl_x + dw) continue;
        if (abs_y < def_tl_y || abs_y >= def_tl_y + dh) continue;

        /* Pixel test in defender's planar bitmap */
        int px = abs_x - def_tl_x;
        int py = abs_y - def_tl_y;
        if (cel_pixel_hit(def_fr, px, py))
            return 1;
    }
    return 0;
}

/*
 * combat_check_hit — collision check wrapper.
 *
 * Uses the pixel-accurate collide.hit engine when data is available;
 * falls back to the geometric check_hit() when not.
 */
static int combat_check_hit(const Combatant *atk, int atk_slot,
                             const Combatant *def, int def_slot)
{
    int result = check_hit_hitdata(atk, atk_slot, def, def_slot);
    if (result >= 0)
        return result;
    /* Fallback to geometric check */
    return check_hit(atk, def);
}

/*
 * check_hit — test whether the attacker's current attack lands on the
 * defender.
 *
 * The reach and valid Y window depend on the attack type:
 *  - Knife:    long horizontal reach (RANGE_KNIFE) in facing direction
 *  - Backward: reach is in the OPPOSITE direction to facing
 *  - Upward:   reach is forward+down, also hits wider Y
 *  - Axe:      short reach but wide Y window (downward strike)
 *  - Forward:  long reach in facing direction
 *  - Swing:    standard forward reach
 *  - Block/Special: no damage, never hits
 */
static int check_hit(const Combatant *attacker, const Combatant *defender)
{
    if (attacker->attack_type == ATTACK_BLOCK ||
        attacker->attack_type == ATTACK_SPECIAL ||
        attacker->attack_type == ATTACK_NONE)
        return 0;

    int fwd = attacker->facing; /* +1 right, -1 left */
    int reach, fwd_dir;

    switch (attacker->attack_type) {
    case ATTACK_BACKWARD:
        reach   = RANGE_BACKWARD;
        fwd_dir = -fwd; /* attacks behind the knight */
        break;
    case ATTACK_KNIFE:
        reach   = RANGE_KNIFE;
        fwd_dir = fwd;
        break;
    case ATTACK_FORWARD:
        reach   = RANGE_FORWARD;
        fwd_dir = fwd;
        break;
    case ATTACK_AXE:
        reach   = RANGE_AXE;
        fwd_dir = fwd;
        break;
    case ATTACK_UP:
        reach   = RANGE_UP;
        fwd_dir = fwd;
        break;
    default: /* ATTACK_SWING */
        reach   = RANGE_FORWARD;
        fwd_dir = fwd;
        break;
    }

    /* Horizontal centre of the attack hitbox */
    int ax = attacker->x + fwd_dir * reach;
    int dx = defender->x - ax;
    int dy = defender->y - attacker->y;

    /* Axe blow: wide Y window (downward sweep)                       */
    int y_lo = -48, y_hi = 32;
    if (attacker->attack_type == ATTACK_AXE)
        y_hi = 60;

    return (dx > -reach && dx < reach && dy > y_lo && dy < y_hi);
}

/*
 * resolve_attack_damage — damage dealt by the given attack type.
 * The axe blow deals 2× base damage per the game manual.
 */
static int resolve_attack_damage(AttackType t)
{
    switch (t) {
    case ATTACK_AXE:      return DMG_AXE;
    case ATTACK_FORWARD:  return DMG_FORWARD;
    case ATTACK_BACKWARD: return DMG_BACKWARD;
    case ATTACK_UP:       return DMG_UP;
    case ATTACK_KNIFE:    return DMG_KNIFE;
    default:              return DMG_SWING;
    }
}

/*
 * resolve_attack_duration — animation frames for the given attack.
 * Axe blow has the longest wind-up per the game manual.
 */
static int resolve_attack_duration(AttackType t)
{
    switch (t) {
    case ATTACK_AXE:      return DUR_AXE;
    case ATTACK_FORWARD:  return DUR_FORWARD;
    case ATTACK_BACKWARD: return DUR_BACKWARD;
    case ATTACK_UP:       return DUR_UP;
    case ATTACK_KNIFE:    return DUR_KNIFE;
    case ATTACK_BLOCK:    return DUR_BLOCK;
    case ATTACK_SPECIAL:  return DUR_SPECIAL;
    default:              return DUR_SWING;
    }
}

/*
 * decode_attack — map joystick direction + facing direction to an
 * AttackType.
 *
 * The "numpad" layout (facing RIGHT):
 *   NW(7)+fire → Block    N(8)+fire → Special   NE(9)+fire → Forward
 *   W(4)+fire  → Backward                        E(6)+fire → Swing
 *   SW(1)+fire → Knife    S(2)+fire → Axe        SE(3)+fire → Up
 *
 * When facing LEFT the horizontal axes are inverted so that:
 *   "forward" and "backward" always match the knight's facing direction.
 * ("le 1 devient 3, etc." — DOC_MODE_COMBAT §5)
 */
static AttackType decode_attack(int joy_up, int joy_down,
                                 int joy_right, int joy_left,
                                 int facing)
{
    /* Apply horizontal inversion for left-facing knight */
    int joy_fwd  = (facing > 0) ? joy_right : joy_left;
    int joy_back = (facing > 0) ? joy_left  : joy_right;

    if (joy_up && joy_fwd)   return ATTACK_FORWARD;  /* NE (facing right) */
    if (joy_up && joy_back)  return ATTACK_BLOCK;    /* NW (facing right) */
    if (joy_up)              return ATTACK_SPECIAL;  /* N */
    if (joy_down && joy_fwd) return ATTACK_UP;       /* SE (facing right) */
    if (joy_down && joy_back)return ATTACK_KNIFE;    /* SW (facing right) */
    if (joy_down)            return ATTACK_AXE;      /* S */
    if (joy_fwd)             return ATTACK_SWING;    /* E (forward sweep) */
    if (joy_back)            return ATTACK_BACKWARD; /* W (backward blow) */

    /* Fire with no direction → default swing */
    return ATTACK_SWING;
}

/*
 * ai_update — update the AI combatant's state.
 *
 * The AI difficulty is based on the player knight's strength:
 *  - Strength 1-2: simple creature (move + basic swing)
 *  - Strength 3-4: more aggressive (uses axe and forward attacks)
 *  - Strength 5  : smart enemy knight (uses full attack set)
 *
 * The AI also tracks the distance both horizontally and vertically so
 * that it can chase the player across the 2D arena.
 */
static void ai_update(Combatant *ai, const Combatant *player,
                       int knight_strength)
{
    if (ai->state == CSTATE_DEAD ||
        ai->state == CSTATE_HIT  ||
        ai->state == CSTATE_STAGGER) return;

    if (ai->state_timer > 0) { ai->state_timer--; return; }

    int dx = player->x - ai->x;
    int dy = player->y - ai->y;
    ai->facing = (dx > 0) ? 1 : -1;
    int dist_x = dx > 0 ? dx : -dx;
    int dist_y = dy > 0 ? dy : -dy;

    /* Chase the player both horizontally and vertically */
    if (dist_x > RANGE_FORWARD + 8 || dist_y > 20) {
        if (dist_x > 4)
            ai->x += ai->facing * MOVE_SPEED;
        if (dist_y > 4)
            ai->y += (dy > 0 ? 1 : -1) * MOVE_SPEED;
        ai->state = CSTATE_WALK;
        return;
    }

    /* Choose attack based on difficulty / strength level */
    AttackType chosen;
    if (knight_strength >= 5) {
        /* Smart enemy: vary attacks                                    */
        /* Simple deterministic rotation based on tick so we don't need
         * rand() (which would affect reproducibility in tests).        */
        static int s_ai_attack_cycle = 0;
        s_ai_attack_cycle = (s_ai_attack_cycle + 1) % 4;
        static const AttackType smart_attacks[] = {
            ATTACK_FORWARD, ATTACK_AXE, ATTACK_SWING, ATTACK_BACKWARD
        };
        chosen = smart_attacks[s_ai_attack_cycle];
    } else if (knight_strength >= 3) {
        /* Moderate: alternates between swing and axe */
        static int s_ai_mod_cycle = 0;
        s_ai_mod_cycle = (s_ai_mod_cycle + 1) % 2;
        chosen = s_ai_mod_cycle ? ATTACK_AXE : ATTACK_SWING;
    } else {
        /* Basic: always swing */
        chosen = ATTACK_SWING;
    }

    ai->state        = CSTATE_ATTACK;
    ai->attack_type  = chosen;
    ai->state_timer  = resolve_attack_duration(chosen);
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_combat(GameCtx *ctx)
{
    Knight *pk = &ctx->knights[ctx->current_knight];

    /* Knight strength drives difficulty (default 1 if uninitialised) */
    int knight_strength = pk->strength > 0 ? pk->strength : 1;

    /* ---------------------------------------------------------------- */
    /* Set up player combatant                                          */
    /* ---------------------------------------------------------------- */
    Combatant player;
    memset(&player, 0, sizeof(player));
    player.hp         = pk->hp;
    player.max_hp     = pk->max_hp;
    player.x          = 80;
    player.y          = COMBAT_GROUND_Y;
    player.facing     = 1;
    player.state      = CSTATE_IDLE;
    player.human      = 1;
    player.knight_idx = ctx->current_knight;
    player.name       = (const char *[]){ "RICHARD","GODBER","JEFFREY","EDWARD" }[pk->id];

    /* ---------------------------------------------------------------- */
    /* Determine enemy type, CEL, name and HP                          */
    /* ---------------------------------------------------------------- */

    /* Is this a PvP fight (knight vs knight)?                         */
    int enemy_is_knight = (ctx->node_type == 0x01 || ctx->node_type == 0x21);

    /* Creature type comes from the PVE context set by the overworld.
     * For non-PVE combats (PvP, valley) pve_creature_type is ignored.
     * We use it here only when node_type == 0x02.                     */
    int creature_type = (ctx->node_type == 0x02) ? ctx->pve_creature_type : -1;

    const char *enemy_cel_name;
    const char *enemy_name;
    int         base_enemy_hp;

    if (enemy_is_knight) {
        /* PvP: enemy uses the same knight sprites                     */
        enemy_cel_name  = "dw1.cel";   /* will be replaced by knight_cel */
        enemy_name      = "KNIGHT";
        base_enemy_hp   = 60 + knight_strength * 10;
    } else if (ctx->node_type == 0x1c) {
        enemy_cel_name  = "au1.cel";
        enemy_name      = "VALLEY GOD";
        base_enemy_hp   = 120 + knight_strength * 16;
    } else if (ctx->node_type == 0x02) {
        /* PVE creature — select CEL from the creature type extracted   *
         * from LAB_07BD (mog.asm) during overworld node setup.         */
        enemy_cel_name  = creature_cel_for_type(creature_type);
        enemy_name      = creature_name_for_type(creature_type);
        /* HP scales with node defence value (pve_node_defense) and     *
         * knight strength.  LAB_07BD low word is creature defence;     *
         * we use it as a base multiplier (× 4) on top of a strength   *
         * bonus, matching the original stat scaling.                   */
        int cdef        = (ctx->pve_node_defense > 0) ? ctx->pve_node_defense : 5;
        base_enemy_hp   = cdef * 4 + knight_strength * 6;
        if (base_enemy_hp < 20) base_enemy_hp = 20;
    } else {
        enemy_cel_name  = "au1.cel";
        enemy_name      = "CREATURE";
        base_enemy_hp   = 40 + knight_strength * 8;
    }

    /* ---------------------------------------------------------------- */
    /* PVE wave system — LAB_01B7 spawn type probability table         */
    /*                                                                  */
    /* From mog.asm LAB_08C5:                                          */
    /*   rand 0..49  (50 %) → type 1 : 1 creature                     */
    /*   rand 50..69 (20 %) → type 2 : 3 creatures simultaneously     */
    /*   rand 70..89 (20 %) → type 3 : 1 + 2 = 3 total, 1 at a time  */
    /*   rand 90..99 (10 %) → type 4 : reserved (treated as type 1)   */
    /*                                                                  */
    /* We map knight strength → difficulty, scaling both the total    */
    /* number of kills required and simultaneous enemies on screen.    */
    /* ---------------------------------------------------------------- */
    int enemies_total      = pve_total_to_kill(knight_strength);
    int enemies_simul      = pve_simultaneous(knight_strength);
    int enemies_killed     = 0;   /* creatures defeated so far         */
    int enemies_spawned    = 0;   /* creatures spawned so far          */

    /* For non-PVE combat (PvP, Valley) always use a single enemy      */
    if (ctx->node_type != 0x02) {
        enemies_total  = 1;
        enemies_simul  = 1;
    }
    /* Type 0x00 (TROGG WAR BEAST) : LAB_0188 sets LAB_05EC=3 and      *
     * LAB_05ED=1 → 3 beasts must be killed, one at a time.            */
    else if (ctx->pve_creature_type == 0x00) {
        enemies_total  = 3;
        enemies_simul  = 1;
    }
    /* Types 0x0c/0x10/0x38 (ENEMY KNIGHT) : LAB_0164 sets             *
     * LAB_05EC=1 and LAB_05ED=1 → single duel.                        */
    else if (ctx->pve_creature_type == 0x0c ||
             ctx->pve_creature_type == 0x10 ||
             ctx->pve_creature_type == 0x38) {
        enemies_total  = 1;
        enemies_simul  = 1;
    }

    /* Enemy combatant array — up to MAX_COMBAT_ENEMIES simultaneous   */
    Combatant enemies[MAX_COMBAT_ENEMIES];
    memset(enemies, 0, sizeof(enemies));

    /* Spawn initial wave of enemies                                    */
    {
        int to_spawn = enemies_simul;
        if (to_spawn > enemies_total) to_spawn = enemies_total;
        for (int i = 0; i < to_spawn; i++) {
            spawn_enemy(&enemies[i], i, enemy_name, base_enemy_hp);
            enemies_spawned++;
        }
    }

    /* ---------------------------------------------------------------- */
    /* Load background (ch.piv — universal combat bg per LAB_012C)     */
    /* ---------------------------------------------------------------- */
    const char *bg_name = combat_bg_for_node(ctx->node_type);
    uint32_t bg[GAME_W * GAME_H];
    uint32_t bg_palette[MAX_PALETTE];
    MoonPiv *piv = moon_piv_load(bg_name);
    if (!piv) {
        /* Try the other common name variant */
        if (bg_name[0] == 'c' || bg_name[0] == 'C')
            piv = moon_piv_load("ch.PIV");
        else
            piv = moon_piv_load("ch.piv");
    }
    if (piv) {
        render_piv_full(piv, bg);
        int pal_size = 1 << piv->planes;
        if (pal_size > MAX_PALETTE) pal_size = MAX_PALETTE;
        render_build_palette(piv->palette, pal_size, bg_palette);
        moon_piv_free(piv);
    } else {
        for (int i = 0; i < GAME_W * GAME_H; i++)
            bg[i] = 0xFF080808u;
        for (int i = 0; i < MAX_PALETTE; i++)
            bg_palette[i] = 0xFF808080u | 0xFF000000u;
    }

    /* ---------------------------------------------------------------- */
    /* Load knight sprite (dw1.cel — generic knight, 53 frames)        */
    /* dw1.cel is always loaded for the player combatant.              */
    /* The player's knight may also load their faction OB file:        */
    /*   kn1.ob … kn4.ob (LAB_01A9, LAB_07FC in mog.asm).             */
    /* We try the faction-specific OB first and fall back to dw1.cel.  */
    /* ---------------------------------------------------------------- */
    static const char *s_faction_ob[MAX_PLAYERS] = {
        "kn1.ob", "kn2.ob", "kn3.ob", "kn4.ob"
    };
    MoonCel *knight_cel = NULL;
    {
        int ki = ctx->current_knight;
        if (ki >= 0 && ki < MAX_PLAYERS)
            knight_cel = moon_cel_load(s_faction_ob[ki]);
        if (!knight_cel) knight_cel = moon_cel_load("dw1.cel");
        if (!knight_cel) knight_cel = moon_cel_load("dw1.CEL");
    }

    /* ---------------------------------------------------------------- */
    /* Load creature/enemy CEL (from creature type, with fallbacks)    */
    /* ---------------------------------------------------------------- */
    MoonCel *enemy_cel = moon_cel_load(enemy_cel_name);
    if (!enemy_cel) {
        /* Try lowercase variant of the filename                       */
        char lc[64];
        int li = 0;
        for (; enemy_cel_name[li] && li < 63; li++)
            lc[li] = (char)(enemy_cel_name[li] >= 'A' && enemy_cel_name[li] <= 'Z'
                            ? enemy_cel_name[li] + 32 : enemy_cel_name[li]);
        lc[li] = '\0';
        enemy_cel = moon_cel_load(lc);
    }
    if (!enemy_cel) enemy_cel = moon_cel_load("au1.cel");
    if (!enemy_cel) enemy_cel = moon_cel_load("au1.CEL");

    /* For PvP the enemy uses the same knight sprites                  */
    if (enemy_is_knight && !enemy_cel)
        enemy_cel = knight_cel;

    /* ---------------------------------------------------------------- */
    /* Load collide.hit hitbox data (LAB_0A57 in mog.asm)              */
    /* NULL if the file is unavailable — geometric fallback is used.   */
    /* ---------------------------------------------------------------- */
    s_hit = moon_hit_load("collide.hit");
    if (!s_hit) s_hit = moon_hit_load("COLLIDE.HIT");

    /* Determine the CEL name used by the player for collide.hit lookup.
     * We use the same precedence as the CEL load above.               */
    const char *player_cel_name = "dw1.cel";
    {
        int ki = ctx->current_knight;
        static const char *s_fob_names[MAX_PLAYERS] = {
            "kn1.ob", "kn2.ob", "kn3.ob", "kn4.ob"
        };
        if (ki >= 0 && ki < MAX_PLAYERS) {
            /* kn1.ob is a 5-byte stub in the release data set; the
             * collide.hit key we want is the actual loaded name.      */
            if (knight_cel)
                player_cel_name = s_fob_names[ki];
        }
    }

    /* Assign CEL data to player combatant for collision engine        */
    player.cel_name  = player_cel_name;
    player.cel       = knight_cel;
    player.is_knight = 1;

    /* Assign CEL data to initially-spawned enemies                    */
    {
        int to_spawn = (enemies_simul < enemies_total) ? enemies_simul : enemies_total;
        const MoonCel  *ecl   = enemy_is_knight ? knight_cel : enemy_cel;
        const char     *ename = enemy_is_knight ? player_cel_name : enemy_cel_name;
        for (int i = 0; i < to_spawn; i++) {
            enemies[i].cel_name  = ename;
            enemies[i].cel       = ecl;
            enemies[i].is_knight = enemy_is_knight;
        }
    }

    /* ---------------------------------------------------------------- */
    /* Reset per-combat animation counters                              */
    /* ---------------------------------------------------------------- */
    for (int i = 0; i < ANIM_SLOTS; i++) {
        s_anim_frame[i] = 0;
        s_anim_tick [i] = 0;
    }

    static const uint32_t knight_colors[MAX_PLAYERS] = {
        0xFF4444FFu, 0xFFFF4444u, 0xFF44FF44u, 0xFFFFFF44u
    };

    /* Track previous fire state to detect rising edge (LAB_057D)      */
    int prev_fire = 0;

    /* ================================================================ */
    /* Main combat loop                                                 */
    /* ================================================================ */
    while (ctx->state == STATE_COMBAT) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_OVERWORLD;
                goto combat_cleanup;
            }
        }

        /* ---- Read joystick / keyboard inputs ---- */
        int joy_up    = ctx->input.joy[0].up    || (ctx->input.keys && ctx->input.keys[82]);
        int joy_down  = ctx->input.joy[0].down  || (ctx->input.keys && ctx->input.keys[81]);
        int joy_left  = ctx->input.joy[0].left  || (ctx->input.keys && ctx->input.keys[80]);
        int joy_right = ctx->input.joy[0].right || (ctx->input.keys && ctx->input.keys[79]);
        int cur_fire  = ctx->input.joy[0].fire  || ctx->input.space;

        /* ---- Player input ---- */
        if (player.state != CSTATE_DEAD &&
            player.state != CSTATE_HIT  &&
            player.state != CSTATE_STAGGER) {

            if (player.state_timer > 0) {
                player.state_timer--;
                if (player.state_timer == 0) {
                    player.attack_type = ATTACK_NONE;
                    player.state       = CSTATE_IDLE;
                }
            } else {
                if (cur_fire) {
                    /* Fire held: direction + fire = specific attack move.
                     * Only trigger on the rising edge of the fire button
                     * (LAB_057D: fire clears LAB_0981 → action triggered). */
                    if (!prev_fire) {
                        AttackType at = decode_attack(joy_up, joy_down,
                                                      joy_right, joy_left,
                                                      player.facing);
                        player.attack_type  = at;
                        player.state        = (at == ATTACK_BLOCK ||
                                               at == ATTACK_SPECIAL)
                                              ? CSTATE_BLOCK : CSTATE_ATTACK;
                        player.state_timer  = resolve_attack_duration(at);

                        /* Test hit against all active enemies */
                        for (int ei = 0; ei < MAX_COMBAT_ENEMIES; ei++) {
                            if (enemies[ei].state == CSTATE_DEAD) continue;
                            if (enemies[ei].hp    <= 0)            continue;
                            if (combat_check_hit(&player, 0, &enemies[ei], 1 + ei)) {
                                int blocked = (enemies[ei].state == CSTATE_BLOCK);
                                if (!blocked) {
                                    int dmg = resolve_attack_damage(at);
                                    enemies[ei].hp -= dmg;
                                    if (enemies[ei].hp < 0) enemies[ei].hp = 0;
                                    enemies[ei].hit_flash = 6;
                                    if (enemies[ei].hp <= LOW_HP_THRESHOLD
                                        && enemies[ei].hp > 0) {
                                        enemies[ei].state       = CSTATE_STAGGER;
                                        enemies[ei].state_timer = DUR_STAGGER;
                                    } else {
                                        enemies[ei].state       = CSTATE_HIT;
                                        enemies[ei].state_timer = DUR_HIT;
                                    }
                                }
                            }
                        }
                    }
                } else {
                    /* Fire not held: move the knight in the arena.
                     * Movement is purely planar (no jumping) per the
                     * game manual and DOC_MODE_COMBAT §5.             */
                    player.state = CSTATE_IDLE;

                    if (joy_left) {
                        player.x     -= MOVE_SPEED;
                        player.facing = -1;
                        player.state  = CSTATE_WALK;
                    }
                    if (joy_right) {
                        player.x     += MOVE_SPEED;
                        player.facing = 1;
                        player.state  = CSTATE_WALK;
                    }
                    if (joy_up) {
                        player.y -= MOVE_SPEED;
                        player.state = CSTATE_WALK;
                    }
                    if (joy_down) {
                        player.y += MOVE_SPEED;
                        player.state = CSTATE_WALK;
                    }
                }
            }
        } else if (player.state == CSTATE_HIT || player.state == CSTATE_STAGGER) {
            if (player.state_timer > 0) {
                player.state_timer--;
            } else {
                player.state       = CSTATE_IDLE;
                player.attack_type = ATTACK_NONE;
            }
        }

        prev_fire = cur_fire;

        /* Advance player stagger oscillation counter                   */
        if (player.hp > 0 && player.hp <= LOW_HP_THRESHOLD)
            player.stagger_tick++;

        /* ---- AI update for each active enemy ---- */
        for (int ei = 0; ei < MAX_COMBAT_ENEMIES; ei++) {
            Combatant *e = &enemies[ei];

            if (e->state == CSTATE_DEAD && e->hp <= 0) continue;

            /* Advance stagger oscillation */
            if (e->hp > 0 && e->hp <= LOW_HP_THRESHOLD)
                e->stagger_tick++;

            if (e->state != CSTATE_DEAD)
                ai_update(e, &player, knight_strength);

            /* Resolve AI attack hitting the player */
            if (e->state == CSTATE_ATTACK && combat_check_hit(e, 1 + ei, &player, 0)) {
                int blocked = (player.state == CSTATE_BLOCK);
                if (!blocked) {
                    int dmg = resolve_attack_damage(e->attack_type);
                    player.hp -= dmg;
                    if (player.hp < 0) player.hp = 0;
                    player.hit_flash = 6;
                    if (player.hp <= LOW_HP_THRESHOLD && player.hp > 0) {
                        player.state       = CSTATE_STAGGER;
                        player.state_timer = DUR_STAGGER;
                    } else {
                        player.state       = CSTATE_HIT;
                        player.state_timer = DUR_HIT;
                    }
                }
            }

            /* Detect newly-dead enemies and count kills                */
            if (e->hp <= 0 && e->state != CSTATE_DEAD) {
                e->state = CSTATE_DEAD;
                enemies_killed++;
                /* Spawn a replacement if the wave is not exhausted     */
                if (enemies_spawned < enemies_total) {
                    spawn_enemy(e, ei, enemy_name, base_enemy_hp);
                    enemies_spawned++;
                    /* Restore collision fields cleared by spawn_enemy()  */
                    e->cel_name  = enemy_is_knight ? player_cel_name : enemy_cel_name;
                    e->cel       = enemy_is_knight ? knight_cel : enemy_cel;
                    e->is_knight = enemy_is_knight;
                    /* Reset animation slot for this enemy slot          */
                    s_anim_frame[1 + ei] = 0;
                    s_anim_tick [1 + ei] = 0;
                }
            }
        }

        /* ---- Clamp positions to arena bounds ---- */
        if (player.x < 8)           player.x = 8;
        if (player.x > GAME_W - 8)  player.x = GAME_W - 8;
        if (player.y < ARENA_Y_MIN) player.y = ARENA_Y_MIN;
        if (player.y > ARENA_Y_MAX) player.y = ARENA_Y_MAX;
        for (int ei = 0; ei < MAX_COMBAT_ENEMIES; ei++) {
            Combatant *e = &enemies[ei];
            if (e->x < 8)           e->x = 8;
            if (e->x > GAME_W - 8)  e->x = GAME_W - 8;
            if (e->y < ARENA_Y_MIN) e->y = ARENA_Y_MIN;
            if (e->y > ARENA_Y_MAX) e->y = ARENA_Y_MAX;
        }

        /* Decrement hit flash */
        if (player.hit_flash > 0) player.hit_flash--;
        for (int ei = 0; ei < MAX_COMBAT_ENEMIES; ei++)
            if (enemies[ei].hit_flash > 0) enemies[ei].hit_flash--;

        /* ---- Death check for player ---- */
        if (player.hp <= 0) player.state = CSTATE_DEAD;

        /* ---- Victory check: all kills done and no living enemies ---- */
        int all_dead = 1;
        for (int ei = 0; ei < MAX_COMBAT_ENEMIES; ei++) {
            if (enemies[ei].state != CSTATE_DEAD && enemies[ei].hp > 0)
                all_dead = 0;
        }
        int victory = (all_dead && enemies_killed >= enemies_total);

        /* ---- Render ---- */
        memcpy(ctx->fb, bg, sizeof(bg));

        /* Draw all active enemies (back-to-front, right to left)       */
        for (int ei = MAX_COMBAT_ENEMIES - 1; ei >= 0; ei--) {
            if (enemies[ei].state == CSTATE_DEAD && enemies[ei].hp <= 0)
                continue;
            draw_combatant_cel(ctx->fb, &enemies[ei], 1 + ei,
                               enemy_is_knight ? knight_cel : enemy_cel,
                               bg_palette,
                               enemy_is_knight,
                               0xFFCC4444u);
        }

        /* Draw player */
        draw_combatant_cel(ctx->fb, &player, 0,
                           knight_cel,
                           bg_palette,
                           1 /* is_knight */,
                           knight_colors[ctx->current_knight]);

        /* Wave counter (PVE only): show remaining kills                */
        if (ctx->node_type == 0x02 && enemies_total > 1) {
            char wave_buf[32];
            int remaining = enemies_total - enemies_killed;
            if (remaining < 0) remaining = 0;
            snprintf(wave_buf, sizeof(wave_buf), "LEFT: %d/%d",
                     enemies_killed, enemies_total);
            render_text(ctx->fb, wave_buf, 10, 24, 0xFFFFDD88u);
        }

        /* Low-HP warning (knight "vacille" per DOC_MODE_COMBAT §7)     */
        if (player.hp > 0 && player.hp <= LOW_HP_THRESHOLD)
            render_text_centered(ctx->fb, "STAGGERING!", 28, 0xFFFF8800u);

        /* Current attack label (UX feedback)                          */
        if (player.state == CSTATE_ATTACK || player.state == CSTATE_BLOCK) {
            static const char *attack_names[] = {
                "", "SWING", "AXE BLOW", "FORWARD BLOW",
                "BACKWARD BLOW", "UPWARD BLOW", "KNIFE THROW",
                "BLOCK", "SPECIAL DEFENSE"
            };
            int idx = (int)player.attack_type;
            if (idx > 0 && idx < 9)
                render_text_centered(ctx->fb, attack_names[idx],
                                     GAME_H - 20, 0xFFFFDD44u);
        }

        /* Combat result messages */
        if (player.state == CSTATE_DEAD) {
            render_text_centered(ctx->fb, "YOU DIED",      GAME_H / 2,      0xFFFF2222u);
            render_text_centered(ctx->fb, "Press any key", GAME_H / 2 + 12, 0xFF888888u);
        } else if (victory) {
            render_text_centered(ctx->fb, "VICTORY!",      GAME_H / 2,      0xFF44FF44u);
            render_text_centered(ctx->fb, "Press any key", GAME_H / 2 + 12, 0xFF888888u);
        }

        hal_present(ctx->fb);
        hal_vbl_wait();

        /* ---- Combat end ---- */
        if (player.state == CSTATE_DEAD || victory) {
            /* Wait for keypress */
            int waited = 0;
            while (waited < 150) {
                if (hal_poll(&ctx->input)) break;
                if (ctx->input.enter || ctx->input.space ||
                    ctx->input.joy[0].fire) break;
                hal_present(ctx->fb);
                hal_vbl_wait();
                waited++;
            }

            /* Apply combat results */
            if (player.state == CSTATE_DEAD) {
                pk->hp   = 0;
                pk->dead = 1;
                pk->gold /= 2;
            } else {
                /* Victory: restore some HP, gain gold */
                pk->hp    = player.hp;
                pk->gold += 20;
                pk->xp   += 50;

                /* PVE victory: award Valley key if the creature had one
                 * (node_target_knight stores the PVE node index when
                 *  node_type == 0x02 and it was a creature combat).    */
                if (ctx->node_type == 0x02) {
                    int pve_idx = ctx->node_target_knight;
                    if (pve_idx >= 0 && pve_idx < 24) {
                        extern int overworld_pve_take_key(int node_idx, Knight *k);
                        overworld_pve_take_key(pve_idx, pk);
                    }
                }
            }

            /* Determine return state.
             * Valley of Gods (0x1c): return to STATE_VALLEY so
             * game_run_valley can resolve the outcome (victory or
             * defeat).  node_target_knight carries the result flag. */
            if (ctx->node_type == 0x1c) {
                ctx->node_target_knight = (player.state == CSTATE_DEAD) ? 1 : 0;
                ctx->state = STATE_VALLEY;
            } else {
                ctx->state = STATE_OVERWORLD;
            }
            goto combat_cleanup;
        }
    }

combat_cleanup:
    if (s_hit) { moon_hit_free(s_hit); s_hit = NULL; }
    moon_cel_free(knight_cel);
    if (enemy_cel != knight_cel)
        moon_cel_free(enemy_cel);
}
