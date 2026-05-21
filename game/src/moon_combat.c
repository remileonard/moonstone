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
} Combatant;

/* ------------------------------------------------------------------ */
/* Combat backgrounds                                                  */
/* ------------------------------------------------------------------ */

/* Maps node_type to a background PIV file (from DOC_MODE_COMBAT.md §2) */
static const char *combat_bg_for_type(int node_type)
{
    switch (node_type) {
    case 0x01: case 0x21: return "bg5a.PIV"; /* PvP */
    case 0x02:             return "bg3.PIV";  /* creature */
    case 0x1b:             return "bg4.PIV";  /* Stonehenge/Mythral */
    case 0x1c:             return "bg4.PIV";  /* Valley of Gods */
    default:               return "bg2a.PIV"; /* default/arena */
    }
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

/* Per-combatant animation sub-frame (index within current state's range) */
static int s_anim_frame[2] = { 0, 0 }; /* [0]=player, [1]=enemy */
static int s_anim_tick [2] = { 0, 0 };
#define COMBAT_ANIM_SPEED  4  /* ticks per animation frame */

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

static void draw_hp_bar(uint32_t *fb, int x, int y, int hp, int max_hp,
                        uint32_t color, const char *name)
{
    int bar_w = 80;
    int filled = (max_hp > 0) ? bar_w * hp / max_hp : 0;
    if (filled < 0) filled = 0;
    if (filled > bar_w) filled = bar_w;

    /* Background */
    render_fill_rect(fb, x, y, bar_w, 8, 0xFF222222u);
    /* Filled portion — colour shifts to red when critically low */
    uint32_t bar_color = (hp <= LOW_HP_THRESHOLD) ? 0xFFFF2200u : color;
    render_fill_rect(fb, x, y, filled, 8, bar_color);
    /* Border */
    render_fill_rect(fb, x,          y, bar_w, 1, 0xFF888888u);
    render_fill_rect(fb, x, y + 7,        bar_w, 1, 0xFF888888u);

    /* Name label above the bar */
    render_text(fb, name, x, y - 10, color);
}

/* ------------------------------------------------------------------ */
/* Combat logic helpers                                                */
/* ------------------------------------------------------------------ */

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
    /* Set up combatants */
    Combatant player, enemy;
    memset(&player, 0, sizeof(player));
    memset(&enemy,  0, sizeof(enemy));

    Knight *pk = &ctx->knights[ctx->current_knight];
    player.hp         = pk->hp;
    player.max_hp     = pk->max_hp;
    player.x          = 80;
    player.y          = COMBAT_GROUND_Y;
    player.facing     = 1;
    player.state      = CSTATE_IDLE;
    player.human      = 1;
    player.knight_idx = ctx->current_knight;
    player.name       = (const char *[]){ "RICHARD","GODBER","JEFFREY","EDWARD" }[pk->id];

    /* Enemy HP scales with combat type and knight strength (difficulty) */
    int enemy_hp = 40 + pk->strength * 8;   /* base difficulty        */
    const char *enemy_name = "CREATURE";
    if (ctx->node_type == 0x01 || ctx->node_type == 0x21) {
        enemy_name = "KNIGHT";
        enemy_hp   = 60 + pk->strength * 10;
    } else if (ctx->node_type == 0x1c) {
        enemy_name = "VALLEY GOD";
        enemy_hp   = 120 + pk->strength * 16;
    }
    enemy.hp      = enemy_hp;
    enemy.max_hp  = enemy_hp;
    enemy.x       = 240;
    enemy.y       = COMBAT_GROUND_Y;
    enemy.facing  = -1;
    enemy.state   = CSTATE_IDLE;
    enemy.human   = 0;
    enemy.name    = enemy_name;

    /* Load background */
    const char *bg_name = combat_bg_for_type(ctx->node_type);
    uint32_t bg[GAME_W * GAME_H];
    uint32_t bg_palette[MAX_PALETTE];
    MoonPiv *piv = moon_piv_load(bg_name);
    if (!piv) {
        const char *alt = bg_name;
        /* Try lowercase variant */
        char lc_name[64];
        int li = 0;
        for (; alt[li] && li < 63; li++)
            lc_name[li] = (char)(alt[li] >= 'A' && alt[li] <= 'Z'
                           ? alt[li] + 32 : alt[li]);
        lc_name[li] = '\0';
        piv = moon_piv_load(lc_name);
    }
    /* Also try ch.piv — the dedicated combat background */
    if (!piv) piv = moon_piv_load("ch.piv");
    if (!piv) piv = moon_piv_load("ch.PIV");
    if (piv) {
        render_piv_full(piv, bg);
        int pal_size = 1 << piv->planes;
        if (pal_size > MAX_PALETTE) pal_size = MAX_PALETTE;
        render_build_palette(piv->palette, pal_size, bg_palette);
        moon_piv_free(piv);
    } else {
        for (int i = 0; i < GAME_W * GAME_H; i++)
            bg[i] = 0xFF080808u;
        /* Fallback neutral palette */
        for (int i = 0; i < MAX_PALETTE; i++)
            bg_palette[i] = 0xFF808080u | 0xFF000000u;
    }

    /* Load knight sprite (dw1.cel — generic knight, 53 frames) */
    MoonCel *knight_cel = moon_cel_load("dw1.cel");
    if (!knight_cel) knight_cel = moon_cel_load("dw1.CEL");

    /* Load enemy/creature sprite (au1.cel — 92 frames) */
    MoonCel *enemy_cel  = moon_cel_load("au1.cel");
    if (!enemy_cel) enemy_cel = moon_cel_load("au1.CEL");

    /* For PvP, enemy is also a knight */
    int enemy_is_knight = (ctx->node_type == 0x01 || ctx->node_type == 0x21);
    if (enemy_is_knight && !enemy_cel) {
        /* Re-use the knight sprite for the opponent */
        enemy_cel = knight_cel;
    }

    /* Reset per-combat animation counters */
    s_anim_frame[0] = s_anim_frame[1] = 0;
    s_anim_tick [0] = s_anim_tick [1] = 0;

    static const uint32_t knight_colors[MAX_PLAYERS] = {
        0xFF4444FFu, 0xFFFF4444u, 0xFF44FF44u, 0xFFFFFF44u
    };

    /* Knight strength for AI difficulty (default 1 if uninitialised) */
    int knight_strength = pk->strength > 0 ? pk->strength : 1;

    /* Track previous fire state to detect the rising edge              */
    int prev_fire = 0;

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
                    /* Attack/block finished — clear attack type */
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

                        /* Resolve hit immediately for melee attacks.
                         * Knife could be a projectile but we resolve it
                         * instantly here for simplicity. */
                        if (enemy.state != CSTATE_DEAD &&
                            check_hit(&player, &enemy)) {
                            /* Block/Special reduce incoming damage to 0;
                             * a blocking enemy absorbs the hit. */
                            int blocked = (enemy.state == CSTATE_BLOCK);
                            if (!blocked) {
                                int dmg = resolve_attack_damage(at);
                                enemy.hp -= dmg;
                                if (enemy.hp < 0) enemy.hp = 0;
                                enemy.hit_flash  = 6;
                                /* Stagger when critically low */
                                if (enemy.hp <= LOW_HP_THRESHOLD && enemy.hp > 0) {
                                    enemy.state       = CSTATE_STAGGER;
                                    enemy.state_timer = DUR_STAGGER;
                                } else {
                                    enemy.state       = CSTATE_HIT;
                                    enemy.state_timer = DUR_HIT;
                                }
                            }
                        }
                    }
                } else {
                    /* Fire not held: move the knight in the arena.
                     * Movement is purely planar (horizontal + vertical),
                     * there is NO jumping per the game manual and
                     * DOC_MODE_COMBAT §5. */
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

        /* Advance stagger oscillation counter (drives the wobble)      */
        if (player.hp > 0 && player.hp <= LOW_HP_THRESHOLD)
            player.stagger_tick++;
        if (enemy.hp  > 0 && enemy.hp  <= LOW_HP_THRESHOLD)
            enemy.stagger_tick++;

        /* ---- AI update ---- */
        if (enemy.state != CSTATE_DEAD) {
            ai_update(&enemy, &player, knight_strength);

            /* Resolve AI attack hitting the player */
            if (enemy.state == CSTATE_ATTACK &&
                check_hit(&enemy, &player)) {
                int blocked = (player.state == CSTATE_BLOCK);
                if (!blocked) {
                    int dmg = resolve_attack_damage(enemy.attack_type);
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
        }

        /* ---- Clamp positions to arena bounds ---- */
        if (player.x < 8)            player.x = 8;
        if (player.x > GAME_W - 8)   player.x = GAME_W - 8;
        if (player.y < ARENA_Y_MIN)  player.y = ARENA_Y_MIN;
        if (player.y > ARENA_Y_MAX)  player.y = ARENA_Y_MAX;
        if (enemy.x  < 8)            enemy.x  = 8;
        if (enemy.x  > GAME_W - 8)   enemy.x  = GAME_W - 8;
        if (enemy.y  < ARENA_Y_MIN)  enemy.y  = ARENA_Y_MIN;
        if (enemy.y  > ARENA_Y_MAX)  enemy.y  = ARENA_Y_MAX;

        /* Decrement hit flash */
        if (player.hit_flash > 0) player.hit_flash--;
        if (enemy.hit_flash  > 0) enemy.hit_flash--;

        /* ---- Death check ---- */
        if (player.hp <= 0) player.state = CSTATE_DEAD;
        if (enemy.hp  <= 0) enemy.state  = CSTATE_DEAD;

        /* ---- Render ---- */
        memcpy(ctx->fb, bg, sizeof(bg));

        draw_combatant_cel(ctx->fb, &enemy,  1,
                           enemy_is_knight ? knight_cel : enemy_cel,
                           bg_palette,
                           enemy_is_knight,
                           0xFFCC4444u);
        draw_combatant_cel(ctx->fb, &player, 0,
                           knight_cel,
                           bg_palette,
                           1 /* is_knight */,
                           knight_colors[ctx->current_knight]);

        /* HP bars — always visible per issue requirement               */
        draw_hp_bar(ctx->fb, 10, 12,
                    player.hp, player.max_hp,
                    knight_colors[ctx->current_knight], player.name);
        draw_hp_bar(ctx->fb, GAME_W - 90, 12,
                    enemy.hp, enemy.max_hp,
                    0xFFCC4444u, enemy.name);

        /* Low-HP warning (knight "vacille" signal to player)           */
        if (player.hp > 0 && player.hp <= LOW_HP_THRESHOLD)
            render_text_centered(ctx->fb, "STAGGERING!", 28, 0xFFFF8800u);

        /* Current attack label (debug / UX feedback)                  */
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
        } else if (enemy.state == CSTATE_DEAD) {
            render_text_centered(ctx->fb, "VICTORY!",      GAME_H / 2,      0xFF44FF44u);
            render_text_centered(ctx->fb, "Press any key", GAME_H / 2 + 12, 0xFF888888u);
        }

        hal_present(ctx->fb);
        hal_vbl_wait();

        /* ---- Combat end ---- */
        if (player.state == CSTATE_DEAD || enemy.state == CSTATE_DEAD) {
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
                pk->hp = 0;
                pk->dead = 1;
                /* Gold penalty */
                pk->gold /= 2;
            } else {
                /* Victory: restore some HP, gain gold */
                pk->hp = player.hp;
                pk->gold += 20;
                pk->xp   += 50;

                /* PVE victory: award Valley key if the creature had one
                 * (node_target_knight stores the PVE node index when
                 *  node_type == 0x02 and it was a creature combat).    */
                if (ctx->node_type == 0x02) {
                    extern int g_pve_node_hit; /* set in moon_overworld.c */
                    (void)g_pve_node_hit;
                    /* Award key via node_target_knight if in range */
                    int pve_idx = ctx->node_target_knight;
                    if (pve_idx >= 0 && pve_idx < 8) {
                        /* Resolve key award — the PVE table is in
                         * moon_overworld.c; we read it through the
                         * exported helper. */
                        extern int overworld_pve_take_key(int node_idx, Knight *k);
                        overworld_pve_take_key(pve_idx, pk);
                    }
                }
            }

            /* Determine return state.
             * Valley of Gods (0x1c): return to STATE_VALLEY so
             * game_run_valley can resolve the outcome (victory or
             * defeat).  node_target_knight carries the result flag:
             *   0 = player won, 1 = player lost. */
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
    moon_cel_free(knight_cel);
    if (enemy_cel != knight_cel)
        moon_cel_free(enemy_cel);
}
