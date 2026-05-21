/*
 * moon_overworld.c — Overworld map mode for Moonstone
 *
 * Implements the overworld as described in DOC_MODE_OVERWORLD.md.
 *
 * Key features implemented here:
 *  1. PVE combat nodes (type 0x02) at fixed positions (§1.7)
 *  2. Turn-by-turn mode: each player moves up to endurance×20 pixels,
 *     then chooses to interact, view inventory, or pass their turn (§3)
 *  3. Dragon spawns from round >= 2, flies autonomously, triggers
 *     combat on collision with a knight (§4)
 *  4. Black knights (IA) patrol the map and trigger PvP when they
 *     reach a player (§5)
 *  5. Math the Wizard (0x1e) → STATE_WIZARD (handled in moon_wizard.c)
 *  6. Valley of Gods (0x1c) → requires all 4 keys (handled in
 *     moon_valley.c)
 *
 * The map is displayed using dw1.PIV (320×200 background).
 * Sprite assets (DOC_MODE_OVERWORLD.md §1.2):
 *   ov1.cel  — 4 frames — overworld node icons
 *   li1.cel  — 30 frames — location/place icons
 *   dg1.cel  — 55 frames — dragon flying on map
 *   ha1.cel  — 22 frames — hawk / map decoration
 *   co1.cel  — 25 frames — complementary icons
 *   da1.cel  — 52 frames — damage/animated decoration
 *   kn1..4.ob — knight sprites per faction (kn1.ob is a stub)
 *   Kn5.ob   — black knight sprite (LAB_0773)
 */

#include "moon_overworld.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/* Static node table (LAB_069F)                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    int      type;
    int      x;
    int      y;
    const char *name;
} MapNode;

static const MapNode s_nodes[] = {
    /* type   x    y   name                     */
    { 0x15,  18,  11, "Village of Richard"      },
    { 0x16, 286,  11, "Village of Godber"       },
    { 0x17,   0, 187, "Village of Jeffrey"      },
    { 0x18, 303, 192, "Village of Edward"       },
    { 0x19,  82,  28, "Highwood"                },
    { 0x1a, 277, 143, "Waterdeep"               },
    { 0x1b,  88, 155, "Stonehenge"              },
    { 0x1c, 152,  97, "Valley of the Gods"      },
    { 0x1e, 217,  11, "Math the Wizard"         },
};
#define NUM_NODES ((int)(sizeof(s_nodes)/sizeof(s_nodes[0])))

#define NODE_PROXIMITY  16   /* pixel radius to trigger node */

/* ------------------------------------------------------------------ */
/* PVE creature nodes (type 0x02, §1.7)                               */
/*                                                                     */
/* 24 entries at fixed overworld positions, sourced from LAB_07BE     */
/* (mog.asm).  Keys are assigned randomly at game start (LAB_01B4):   */
/* one key per group of 6 creatures (groups 0-5, 6-11, 12-17, 18-23) */
/* so key_bit is -1 here and will be resolved at runtime.             */
/* A node disappears once the creature's key AND loot are exhausted   */
/* (LAB_005F: MOVE.L #$ffffffff, 10(A0)).                             */
/* ------------------------------------------------------------------ */

typedef struct {
    int x;
    int y;
    int key_bit;      /* –1 = dynamic (assigned randomly at game start)  */
    int alive;        /* 1 = still present on the map                   */
    const char *name;
    int creature_type;/* LAB_07BD high word = byte offset into LAB_08C8  */
    int creature_def; /* LAB_07BD low  word = creature defence value      */
    int group;        /* sound/region group: 0=fol, 1=wal, 2=swl, 3=gll */
} PveNode;

/*
 * 24 creature nodes — positions extracted from LAB_07BE (mog.asm).
 * Each DC.L $XXXXYYYY encodes X=high-word, Y=low-word.
 *
 * creature_type / creature_def come from LAB_07BD (mog.asm):
 *   DC.L $TTTTDDDD  where TTTT = high word (type/handler offset),
 *                         DDDD = low  word (defence value)
 *
 * creature_type values (LAB_08C8 offsets, confirmed by LAB_01AE fill loop):
 *   0x00 = He   (enemy knight, He1.ob)
 *   0x04 = Mudmen (Mudmen1.cel)
 *   0x08 = Balok  (Balok1.cel)
 *   0x0c = Ratmen generic (Ratmen1.cel)
 *   0x14 = Dragon (Dragon1.cel)
 *   0x18 = TroggAxe  (TroggAxe1.cel)
 *   0x1c = TroggSpear (TroggSpear1.cel)
 *   0x20 = Demon (Demon1.cel)
 *   0x24 = Ratmen variant (Ratmen1.cel)
 *   0x30 = Troll (Troll1.cel)
 *   0x40 = Demon/Selene (Sel.cel)
 *
 * Groups (per LAB_07C0 sound table): fol=0-5, wal=6-11, swl=12-17, gll=18-23.
 */
static PveNode s_pve_nodes[] = {
    /* x,   y, key, alive, name,         ctype, cdef, grp */
    /* Entries 0-5  : "fol" group (western region) */
    {  24, 112, -1, 1, "RATMEN",        0x24, 0x0a, 0 }, /* $00180070 */
    { 104, 120, -1, 1, "TROGGAXE",      0x18, 0x0a, 0 }, /* $00680078 */
    { 120, 144, -1, 1, "TROGGAXE",      0x18, 0x0e, 0 }, /* $00780090 */
    {  80, 184, -1, 1, "RATMEN",        0x24, 0x0e, 0 }, /* $005000b8 */
    {  24, 160, -1, 1, "RATMEN",        0x24, 0x0e, 0 }, /* $001800a0 */
    {  48, 136, -1, 1, "TROGGAXE",      0x18, 0x0d, 0 }, /* $00300088 */
    /* Entries 6-11 : "wal" group (north/northeast region) */
    { 296,  32, -1, 1, "TROLL",         0x30, 0x04, 1 }, /* $01280020 */
    { 232,  16, -1, 1, "TROLL",         0x30, 0x03, 1 }, /* $00e80010 */
    { 176,  48, -1, 1, "TROLL",         0x30, 0x05, 1 }, /* $00b00030 */
    { 240,  48, -1, 1, "TROGGSPEAR",    0x1c, 0x0a, 1 }, /* $00f00030 */
    { 216,  72, -1, 1, "TROGGSPEAR",    0x1c, 0x0a, 1 }, /* $00d80048 */
    { 272,  80, -1, 1, "TROGGSPEAR",    0x1c, 0x0a, 1 }, /* $01100050 */
    /* Entries 12-17: "swl" group (central/eastern region) */
    { 208, 104, -1, 1, "MUDMEN",        0x04, 0x05, 2 }, /* $00d00068 */
    { 248, 120, -1, 1, "MUDMEN",        0x04, 0x05, 2 }, /* $00f80078 */
    { 168, 136, -1, 1, "MUDMEN",        0x04, 0x05, 2 }, /* $00a80088 */
    { 152, 176, -1, 1, "DEMON",         0x40, 0x07, 2 }, /* $009800b0 */
    { 232, 176, -1, 1, "MUDMEN",        0x04, 0x06, 2 }, /* $00e800b0 */
    { 288, 176, -1, 1, "DEMON",         0x40, 0x06, 2 }, /* $012000b0 */
    /* Entries 18-23: "gll" group (northwest/north-central region) */
    {  24,  24, -1, 1, "ENEMY KNIGHT",  0x00, 0x08, 3 }, /* $00180018 */
    {  96,  16, -1, 1, "DEMON",         0x20, 0x05, 3 }, /* $00600010 */
    { 136,  40, -1, 1, "DEMON",         0x20, 0x04, 3 }, /* $00880028 */
    {  32,  64, -1, 1, "ENEMY KNIGHT",  0x00, 0x08, 3 }, /* $00200040 */
    {  80,  64, -1, 1, "TROGGAXE",      0x18, 0x0d, 3 }, /* $00500040 */
    {  96,  88, -1, 1, "ENEMY KNIGHT",  0x00, 0x08, 3 }, /* $00600058 */
};
#define NUM_PVE_NODES ((int)(sizeof(s_pve_nodes)/sizeof(s_pve_nodes[0])))

/* ------------------------------------------------------------------ */
/* Movement budget per turn                                            */
/* Endurance (1–5) × 20 pixels = 20..100 steps per turn.             */
/* ------------------------------------------------------------------ */
#define STEPS_PER_ENDURANCE  20
#define DEFAULT_ENDURANCE     2   /* fallback if field not set */

static int steps_for_knight(const Knight *k)
{
    int endurance = (k->endurance >= 1) ? k->endurance : DEFAULT_ENDURANCE;
    return endurance * STEPS_PER_ENDURANCE;
}

/* ------------------------------------------------------------------ */
/* Knight sprite colours (small dot to represent each knight)         */
/* ------------------------------------------------------------------ */

static const uint32_t s_knight_dot_colors[MAX_PLAYERS] = {
    0xFF4444FFu,
    0xFFFF4444u,
    0xFF44FF44u,
    0xFFFFFF44u,
};

/* Starting positions (near their respective villages) */
static const int s_start_x[MAX_PLAYERS] = { 18, 286,   0, 303 };
static const int s_start_y[MAX_PLAYERS] = { 11,  11, 187, 192 };

/* ------------------------------------------------------------------ */
/* CEL / OB sprite assets                                              */
/* ------------------------------------------------------------------ */

static const char *s_knight_ob_names[MAX_PLAYERS] = {
    "kn1.ob", "kn2.ob", "kn3.ob", "kn4.ob"
};

static MoonCel *s_ov_cel   = NULL;
static MoonCel *s_li_cel   = NULL;
static MoonCel *s_dg_cel   = NULL;
static MoonCel *s_ha_cel   = NULL;
static MoonCel *s_kn_ob[MAX_PLAYERS];
static MoonCel *s_kn5_ob   = NULL; /* black knight sprite (Kn5.ob) */

static uint32_t s_ov_palette[MAX_PALETTE];

/* Dragon animation constants (uses dg1.cel frames 34–41) */
#define DG_FRAME_BASE    34
#define DG_FRAME_COUNT    8
#define DG_ANIM_SPEED     4
#define DG_PROXIMITY     18   /* collision radius with knights */
#define DG_SPEED_X        2
#define DG_SPEED_Y        1
#define DG_COUNTDOWN_INIT 100

/* Black knight constants */
#define BK_MAX            4   /* absolute maximum (4 - 0 human players)   */
#define BK_SPEED          1   /* pixels per tick                           */
#define BK_PROXIMITY     14   /* combat trigger radius (pixels)            */
#define BK_ATTACK_CHANCE 25   /* % chance to pick a knight target per turn */

/*
 * Fixed starting positions for the 4 BK slots (LAB_01AE, mog.asm):
 *   LAB_0613: x=0x000f=15,  y=0x0064=100
 *   LAB_0614: x=0x012c=300, y=0x0064=100
 *   LAB_0615: x=0x00a0=160, y=0x0014=20
 *   LAB_0616: x=0x00a0=160, y=0x00b4=180
 */
static const int s_bk_start_x[BK_MAX] = { 15, 300, 160, 160 };
static const int s_bk_start_y[BK_MAX] = { 100, 100,  20, 180 };

/* Walk animation */
static int s_kn_frame  = 0;
static int s_kn_tick   = 0;
#define KN_ANIM_SPEED  6

/* ------------------------------------------------------------------ */
/* Rendering                                                           */
/* ------------------------------------------------------------------ */

static uint32_t s_map_bg[GAME_W * GAME_H];
static int      s_map_loaded = 0;

static void load_map_background(void)
{
    if (s_map_loaded) return;

    MoonPiv *piv = moon_piv_load("dw1.PIV");
    if (!piv) piv = moon_piv_load("dw1.piv");
    if (piv) {
        render_piv_full(piv, s_map_bg);
        int pal_size = 1 << piv->planes;
        if (pal_size > MAX_PALETTE) pal_size = MAX_PALETTE;
        render_build_palette(piv->palette, pal_size, s_ov_palette);
        moon_piv_free(piv);
    } else {
        for (int i = 0; i < GAME_W * GAME_H; i++)
            s_map_bg[i] = 0xFF082808u;
        for (int i = 0; i < MAX_PALETTE; i++)
            s_ov_palette[i] = 0xFF808080u | 0xFF000000u;
        for (int n = 0; n < NUM_NODES; n++)
            render_fill_rect(s_map_bg, s_nodes[n].x - 3, s_nodes[n].y - 3,
                             7, 7, 0xFF888888u);
    }

    s_ov_cel = moon_cel_load("ov1.cel");
    if (!s_ov_cel) s_ov_cel = moon_cel_load("ov1.CEL");

    s_li_cel = moon_cel_load("li1.cel");
    if (!s_li_cel) s_li_cel = moon_cel_load("li1.CEL");

    s_dg_cel = moon_cel_load("dg1.cel");
    if (!s_dg_cel) s_dg_cel = moon_cel_load("dg1.CEL");

    s_ha_cel = moon_cel_load("ha1.cel");
    if (!s_ha_cel) s_ha_cel = moon_cel_load("ha1.CEL");

    for (int i = 0; i < MAX_PLAYERS; i++) {
        s_kn_ob[i] = NULL;
        MoonOb *ob = moon_ob_load(s_knight_ob_names[i]);
        if (ob && ob->frame_count > 0)
            s_kn_ob[i] = (MoonCel *)ob;
        else
            moon_ob_free(ob);
    }

    /* Black knight sprite (Kn5.ob) */
    {
        MoonOb *ob = moon_ob_load("Kn5.ob");
        if (!ob) ob = moon_ob_load("kn5.ob");
        if (ob && ob->frame_count > 0)
            s_kn5_ob = (MoonCel *)ob;
        else
            moon_ob_free(ob);
    }

    s_map_loaded = 1;
}

/* li1.cel frame for a given static node type */
static int node_li_frame(int node_type)
{
    switch (node_type) {
    case 0x15: return  0;
    case 0x16: return  4;
    case 0x17: return  8;
    case 0x18: return 12;
    case 0x19: return 16;
    case 0x1a: return 20;
    case 0x1b: return 24;
    case 0x1c: return 28;
    case 0x1e: return  2;
    default:   return  0;
    }
}

static int node_icon_frame(int node_type)
{
    switch (node_type) {
    case 0x15: case 0x16: case 0x17: case 0x18: return 0;
    case 0x19: case 0x1a:                        return 2;
    case 0x1b: case 0x1c:                        return 3;
    case 0x1e:                                   return 3;
    default:                                     return 0;
    }
}

/* ------------------------------------------------------------------ */
/* Dragon initialisation and update (§4)                              */
/* ------------------------------------------------------------------ */

static void dragon_init(GameCtx *ctx)
{
    ctx->dragon_active    = 1;
    ctx->dragon_x         = 10;
    ctx->dragon_y         = 100;
    ctx->dragon_vx        = DG_SPEED_X;
    ctx->dragon_countdown = DG_COUNTDOWN_INIT;
    ctx->dragon_frame     = 0;
    ctx->dragon_tick      = 0;

    /* Choose a random active player as initial target */
    ctx->dragon_target = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (ctx->knights[i].active && !ctx->knights[i].dead) {
            ctx->dragon_target = i;
            break;
        }
    }
}

/*
 * dragon_update — advance dragon position one tick.
 * Phase 1 (countdown > 60): approach mode, move horizontally.
 * Phase 2 (countdown ≤ 60): pursuit mode, track target knight Y.
 * Returns the index of the knight collided with, or –1.
 */
static int dragon_update(GameCtx *ctx)
{
    if (!ctx->dragon_active) return -1;

    /* Animate */
    ctx->dragon_tick++;
    if (ctx->dragon_tick >= DG_ANIM_SPEED) {
        ctx->dragon_tick = 0;
        ctx->dragon_frame = (ctx->dragon_frame + 1) % DG_FRAME_COUNT;
    }

    if (ctx->dragon_countdown > 0)
        ctx->dragon_countdown--;

    /* Horizontal movement */
    ctx->dragon_x += ctx->dragon_vx;

    /* Vertical pursuit (phase 2 only) */
    if (ctx->dragon_countdown <= 60 && ctx->dragon_target >= 0) {
        Knight *tgt = &ctx->knights[ctx->dragon_target];
        if (tgt->active && !tgt->dead) {
            int dy = tgt->map_y - ctx->dragon_y;
            if (dy > 0)       ctx->dragon_y += DG_SPEED_Y;
            else if (dy < 0)  ctx->dragon_y -= DG_SPEED_Y;
        }
    }

    /* Bounce off edges */
    if (ctx->dragon_x > 350)  { ctx->dragon_x = 349; ctx->dragon_vx = -DG_SPEED_X; }
    if (ctx->dragon_x < -20)  { ctx->dragon_x = -19; ctx->dragon_vx =  DG_SPEED_X; }
    if (ctx->dragon_y > GAME_H) ctx->dragon_y = 0;
    if (ctx->dragon_y < 0)     ctx->dragon_y = GAME_H;

    /* Collision detection with active knights */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Knight *k = &ctx->knights[i];
        if (!k->active || k->dead) continue;
        int dx = ctx->dragon_x - k->map_x;
        int dy = ctx->dragon_y - k->map_y;
        if (dx * dx + dy * dy <= DG_PROXIMITY * DG_PROXIMITY)
            return i;
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* Black knight AI movement (§5)                                      */
/* ------------------------------------------------------------------ */

/*
 * black_knight_init — set up BK slots according to the assembly logic
 * (LAB_01AE, LAB_00E5).
 *
 * The assembly initialises 4 knight structs with faction=4 (black
 * knight).  During character selection each human player overwrites
 * one slot with their chosen faction (0-3).  The remaining slots keep
 * faction=4 and become the active black knights.
 *
 * Here we populate the unused knights[] slots (those not already active
 * as human players) as black knights so that they participate in the
 * normal per-round turn sequence just like human knights.
 */
static void black_knight_init(GameCtx *ctx)
{
    int human_count = 0;
    for (int i = 0; i < MAX_PLAYERS; i++)
        if (ctx->knights[i].active && ctx->knights[i].human)
            human_count++;

    ctx->bk_count = 4 - human_count;
    if (ctx->bk_count < 0) ctx->bk_count = 0;
    if (ctx->bk_count > BK_MAX) ctx->bk_count = BK_MAX;

    /* Populate free knights[] slots as black knights.
     * Use black_knight_target[i] indexed by the knight slot. */
    int b = 0;
    for (int i = 0; i < MAX_PLAYERS && b < ctx->bk_count; i++) {
        if (ctx->knights[i].active) continue; /* already a human player */
        Knight *bk = &ctx->knights[i];
        bk->active          = 1;
        bk->human           = 0;
        bk->is_black_knight = 1;
        bk->map_x           = s_bk_start_x[b];
        bk->map_y           = s_bk_start_y[b];
        bk->endurance       = DEFAULT_ENDURANCE;
        bk->strength        = 2;
        bk->constitution    = 2;
        bk->max_hp          = 30;
        bk->hp              = 30;
        bk->dead            = 0;
        bk->turn_done       = 0;
        bk->steps_remaining = 0;
        ctx->black_knight_target[i] = -1; /* no creature target yet */
        b++;
    }
}

/*
 * bk_pick_creature_target — pick a creature-node target for black
 * knight b.  Mirrors LAB_0DEB / LAB_0DE0 (mog.asm):
 *
 * 1. Sort all 24 alive creature nodes by Manhattan distance from the BK
 *    (LAB_0DE0–0DE9: bubble sort, 0xffff for dead nodes).
 * 2. Pick uniformly from ranks 1, 2 or 3 (not rank 0 = closest).
 *    The assembly uses: ANDI #3, rand; BEQ retry → result ∈ {1,2,3}.
 *
 * Returns a PVE node index, or -1 if no nodes are alive.
 */
static int bk_pick_creature_target(int bx, int by)
{
    /* Build (distance, index) array */
    int dist[NUM_PVE_NODES];
    int idx[NUM_PVE_NODES];
    int alive_count = 0;

    for (int n = 0; n < NUM_PVE_NODES; n++) {
        if (!s_pve_nodes[n].alive) {
            dist[n] = 0x7fff;
        } else {
            int dx = s_pve_nodes[n].x - bx;
            int dy = s_pve_nodes[n].y - by;
            if (dx < 0) dx = -dx;
            if (dy < 0) dy = -dy;
            dist[n] = dx + dy;
            alive_count++;
        }
        idx[n] = n;
    }
    if (alive_count == 0) return -1;

    /* Bubble-sort ascending by distance (mirrors LAB_0DE6–0DE9) */
    for (int i = 0; i < NUM_PVE_NODES - 1; i++) {
        for (int j = 0; j < NUM_PVE_NODES - 1 - i; j++) {
            if (dist[j] > dist[j + 1]) {
                int td = dist[j]; dist[j] = dist[j+1]; dist[j+1] = td;
                int ti = idx[j];  idx[j]  = idx[j+1];  idx[j+1]  = ti;
            }
        }
    }

    /* Pick randomly from slots 1, 2, 3 (not slot 0 = absolute closest).
     * Assembly: ANDI #3, rand; BEQ retry → picks 1, 2, or 3. */
    int top = (alive_count < 4) ? alive_count : 4; /* slots 0..3 available */
    if (top <= 1) return idx[0]; /* only one alive node, take it */

    /* Randomly choose from slots 1..top-1 */
    int slot = 1 + (rand() % (top - 1));
    return idx[slot];
}

/*
 * bk_turn_step — advance a single black knight one movement step during
 * its overworld turn.  Called once per tick while the BK's turn is active.
 *
 * Mirrors the assembly turn dispatcher for faction=4 knights (LAB_0DAD):
 *   1. Ensure creature-node target (LAB_0DEA / bk_pick_creature_target).
 *   2. 20 % chance to lock on to the nearest human knight (LAB_0DF8).
 *   3. Move one pixel toward current target (LAB_0E0C).
 *   4. Consume one step from steps_remaining; end turn when exhausted.
 *   5. Combat trigger if the BK has closed to BK_PROXIMITY of a
 *      locked-on human knight.
 *   6. Re-target when the creature node is reached.
 *
 * Returns the index of the human knight that was reached (PvP trigger),
 * or -1 if nobody was hit this step.
 */
static int bk_turn_step(GameCtx *ctx, int ki)
{
    Knight *bk = &ctx->knights[ki];
    int bx = bk->map_x;
    int by = bk->map_y;

    /* ---- Step 1: ensure creature target ---- */
    if (ctx->black_knight_target[ki] < 0 ||
        !s_pve_nodes[ctx->black_knight_target[ki]].alive) {
        ctx->black_knight_target[ki] = bk_pick_creature_target(bx, by);
    }

    /* ---- Step 2: maybe lock on to a human knight (LAB_0DF5/0DF8) ----
     * 20 % chance per step to switch target to the nearest human knight. */
    int attack_target = -1;
    if ((rand() % 100) < BK_ATTACK_CHANCE) {
        int best_i = -1, best_d = 0x7fffffff;
        for (int i = 0; i < MAX_PLAYERS; i++) {
            Knight *k = &ctx->knights[i];
            if (!k->active || k->dead || k->is_black_knight) continue;
            int dx = bx - k->map_x;
            int dy = by - k->map_y;
            int d  = dx * dx + dy * dy;
            if (d < best_d) { best_d = d; best_i = i; }
        }
        if (best_i >= 0)
            attack_target = best_i;
    }

    /* ---- Step 3: determine movement target (attack > creature) ---- */
    int tx, ty;
    if (attack_target >= 0) {
        tx = ctx->knights[attack_target].map_x;
        ty = ctx->knights[attack_target].map_y;
    } else if (ctx->black_knight_target[ki] >= 0) {
        tx = s_pve_nodes[ctx->black_knight_target[ki]].x;
        ty = s_pve_nodes[ctx->black_knight_target[ki]].y;
    } else {
        /* No valid target — end turn */
        bk->turn_done = 1;
        return -1;
    }

    /* ---- Bresenham-style single-pixel step (LAB_0E0C) ---- */
    if (bk->map_x < tx)       bk->map_x++;
    else if (bk->map_x > tx)  bk->map_x--;
    if (bk->map_y < ty)       bk->map_y++;
    else if (bk->map_y > ty)  bk->map_y--;

    /* Clamp to map */
    if (bk->map_x < 0)        bk->map_x = 0;
    if (bk->map_x >= GAME_W)  bk->map_x = GAME_W - 1;
    if (bk->map_y < 0)        bk->map_y = 0;
    if (bk->map_y >= GAME_H)  bk->map_y = GAME_H - 1;

    bx = bk->map_x;
    by = bk->map_y;

    bk->steps_remaining--;

    /* ---- Step 4: combat trigger (closed to locked-on knight) ---- */
    if (attack_target >= 0) {
        int dx2 = bx - ctx->knights[attack_target].map_x;
        int dy2 = by - ctx->knights[attack_target].map_y;
        if (dx2 * dx2 + dy2 * dy2 <= BK_PROXIMITY * BK_PROXIMITY) {
            bk->dead = 1; /* BK leaves the map after triggering combat */
            ctx->node_target_knight = ki;
            return attack_target;
        }
    }

    /* ---- Step 5: reached creature node → pick a new one next step ---- */
    if (ctx->black_knight_target[ki] >= 0) {
        int nn = ctx->black_knight_target[ki];
        int dx3 = bx - s_pve_nodes[nn].x;
        int dy3 = by - s_pve_nodes[nn].y;
        if (dx3 * dx3 + dy3 * dy3 <= BK_PROXIMITY * BK_PROXIMITY)
            ctx->black_knight_target[ki] = -1;
    }

    if (bk->steps_remaining <= 0)
        bk->turn_done = 1;

    return -1;
}

/* ------------------------------------------------------------------ */
/* Node interaction helpers                                            */
/* ------------------------------------------------------------------ */

static int check_static_node(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];
    for (int n = 0; n < NUM_NODES; n++) {
        int dx = k->map_x - s_nodes[n].x;
        int dy = k->map_y - s_nodes[n].y;
        if (dx * dx + dy * dy <= (NODE_PROXIMITY / 2) * (NODE_PROXIMITY / 2))
            return n;
    }
    return -1;
}

static int check_pve_node(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];
    for (int n = 0; n < NUM_PVE_NODES; n++) {
        if (!s_pve_nodes[n].alive) continue;
        int dx = k->map_x - s_pve_nodes[n].x;
        int dy = k->map_y - s_pve_nodes[n].y;
        if (dx * dx + dy * dy <= NODE_PROXIMITY * NODE_PROXIMITY)
            return n;
    }
    return -1;
}

static void handle_static_node(GameCtx *ctx, int node_idx)
{
    int type = s_nodes[node_idx].type;
    ctx->node_type = type;

    switch (type) {
    case 0x15: case 0x16: case 0x17: case 0x18:
        ctx->state = STATE_VILLAGE;
        break;
    case 0x19: case 0x1a:
        ctx->state = STATE_TOWN;
        break;
    case 0x1b:
        ctx->state = STATE_STONEHENGE;
        break;
    case 0x1c:
        /* Valley of Gods — delegated to moon_valley.c (LAB_009D) */
        ctx->state = STATE_VALLEY;
        break;
    case 0x1e:
        ctx->state = STATE_WIZARD;
        break;
    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Draw helpers                                                        */
/* ------------------------------------------------------------------ */

static void draw_node_icon(GameCtx *ctx, int nx, int ny, int type)
{
    if (s_li_cel && s_li_cel->frame_count > 0) {
        int fr = node_li_frame(type);
        if (fr >= s_li_cel->frame_count) fr = 0;
        render_cel(s_li_cel, fr, s_ov_palette, ctx->fb,
                   nx - (int)s_li_cel->frames[fr].width  / 2,
                   ny - (int)s_li_cel->frames[fr].height / 2,
                   BLIT_MASK);
    } else if (s_ov_cel && s_ov_cel->frame_count > 0) {
        int fr = node_icon_frame(type);
        if (fr >= s_ov_cel->frame_count) fr = 0;
        render_cel(s_ov_cel, fr, s_ov_palette, ctx->fb,
                   nx - (int)s_ov_cel->frames[fr].width  / 2,
                   ny - (int)s_ov_cel->frames[fr].height / 2,
                   BLIT_MASK);
    } else {
        render_fill_rect(ctx->fb, nx - 2, ny - 2, 5, 5, 0xFF888844u);
    }
}

static void draw_overworld(GameCtx *ctx)
{
    memcpy(ctx->fb, s_map_bg, sizeof(s_map_bg));

    /* Walk animation tick */
    s_kn_tick++;
    if (s_kn_tick >= KN_ANIM_SPEED) {
        s_kn_tick  = 0;
        s_kn_frame = (s_kn_frame + 1) & 7;
    }

    /* ---- Static node icons ---- */
    for (int n = 0; n < NUM_NODES; n++)
        draw_node_icon(ctx, s_nodes[n].x, s_nodes[n].y, s_nodes[n].type);

    /* ---- PVE creature nodes (type 0x02) ----
     * Global map display mirrors LAB_0DA3 (mog.asm): MOVE.W #$0014,D0 →
     * frame 20 of li1.cel is the generic creature icon on the full map.
     * (Frame 31 is used only in LAB_0077 for the proximity-highlight pass.)
     */
    for (int n = 0; n < NUM_PVE_NODES; n++) {
        if (!s_pve_nodes[n].alive) continue;
        int nx = s_pve_nodes[n].x;
        int ny = s_pve_nodes[n].y;
        if (s_li_cel && s_li_cel->frame_count > 0) {
            /* frame 20 (0x14): generic creature map icon (LAB_0DA3) */
            int fr = 20;
            if (fr >= s_li_cel->frame_count) fr = s_li_cel->frame_count - 1;
            render_cel(s_li_cel, fr, s_ov_palette, ctx->fb,
                       nx - (int)s_li_cel->frames[fr].width  / 2,
                       ny - (int)s_li_cel->frames[fr].height / 2,
                       BLIT_MASK);
        } else {
            /* Fallback: small red diamond when CEL is unavailable */
            render_fill_rect(ctx->fb, nx - 3, ny - 3, 7, 7, 0xFFAA2200u);
            render_fill_rect(ctx->fb, nx - 1, ny - 1, 3, 3, 0xFFFF4400u);
        }
    }

    /* ---- Dragon ---- */
    if (ctx->dragon_active && s_dg_cel && s_dg_cel->frame_count > 0) {
        int cel_frame = DG_FRAME_BASE + (ctx->dragon_frame % DG_FRAME_COUNT);
        if (cel_frame >= s_dg_cel->frame_count)
            cel_frame = s_dg_cel->frame_count - 1;
        int fw = (int)s_dg_cel->frames[cel_frame].width;
        int fh = (int)s_dg_cel->frames[cel_frame].height;
        render_cel(s_dg_cel, cel_frame, s_ov_palette, ctx->fb,
                   ctx->dragon_x - fw / 2, ctx->dragon_y - fh / 2,
                   BLIT_MASK);
    } else if (ctx->dragon_active) {
        /* Fallback purple diamond */
        render_fill_rect(ctx->fb,
                         ctx->dragon_x - 4, ctx->dragon_y - 4,
                         9, 9, 0xFF880088u);
    }

    /* ---- Black knights ---- */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Knight *bk = &ctx->knights[i];
        if (!bk->active || !bk->is_black_knight || bk->dead) continue;
        int bx = bk->map_x;
        int by = bk->map_y;
        if (s_kn5_ob && s_kn5_ob->frame_count > 0) {
            int fr = s_kn_frame % s_kn5_ob->frame_count;
            int fw = (int)s_kn5_ob->frames[fr].width;
            int fh = (int)s_kn5_ob->frames[fr].height;
            render_cel(s_kn5_ob, fr, s_ov_palette, ctx->fb,
                       bx - fw / 2, by - fh, BLIT_MASK);
        } else {
            /* Fallback: dark red 5×5 dot */
            render_fill_rect(ctx->fb, bx - 2, by - 2, 5, 5, 0xFF660000u);
        }
    }

    /* ---- Player knights (skip black knights, drawn separately above) ---- */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Knight *k = &ctx->knights[i];
        if (!k->active || k->dead || k->is_black_knight) continue;
        int kx = k->map_x, ky = k->map_y;

        if (s_kn_ob[i] && s_kn_ob[i]->frame_count > 0) {
            int fr  = s_kn_frame % s_kn_ob[i]->frame_count;
            int fw  = (int)s_kn_ob[i]->frames[fr].width;
            int fh  = (int)s_kn_ob[i]->frames[fr].height;
            int flip = (kx < GAME_W / 2) ? 0 : BLIT_FLIP_X;
            render_cel(s_kn_ob[i], fr, s_ov_palette, ctx->fb,
                       kx - fw / 2, ky - fh, flip | BLIT_MASK);
        } else {
            render_fill_rect(ctx->fb, kx - 2, ky - 2, 5, 5,
                             s_knight_dot_colors[i]);
        }
    }

    /* ---- HUD (top bar) ---- */
    Knight *k = &ctx->knights[ctx->current_knight];
    char hud[128];
    render_fill_rect(ctx->fb, 0, 0, GAME_W, 9, 0xAA000000u);
    if (k->is_black_knight) {
        snprintf(hud, sizeof(hud), "BLACK KNIGHT  Steps:%d", k->steps_remaining);
        render_text(ctx->fb, hud, 2, 1, 0xFF880000u);
    } else {
        snprintf(hud, sizeof(hud),
                 "%s  HP:%d  GOLD:%d  Keys:%d/4  Steps:%d",
                 (const char *[]){ "RICHARD","GODBER","JEFFREY","EDWARD" }[k->id],
                 k->hp, k->gold, __builtin_popcount(k->keys & 0x0f),
                 k->steps_remaining);
        render_text(ctx->fb, hud, 2, 1, s_knight_dot_colors[ctx->current_knight]);
    }

    /* ---- Node name tooltip ---- */
    for (int n = 0; n < NUM_NODES; n++) {
        int dx = k->map_x - s_nodes[n].x;
        int dy = k->map_y - s_nodes[n].y;
        if (dx * dx + dy * dy <= NODE_PROXIMITY * NODE_PROXIMITY) {
            render_fill_rect(ctx->fb, 0, GAME_H - 10, GAME_W, 10, 0xAA000000u);
            render_text_centered(ctx->fb, s_nodes[n].name,
                                 GAME_H - 9, 0xFFFFFF88u);
            break;
        }
    }
    /* PVE node tooltip */
    for (int n = 0; n < NUM_PVE_NODES; n++) {
        if (!s_pve_nodes[n].alive) continue;
        int dx = k->map_x - s_pve_nodes[n].x;
        int dy = k->map_y - s_pve_nodes[n].y;
        if (dx * dx + dy * dy <= NODE_PROXIMITY * NODE_PROXIMITY) {
            render_fill_rect(ctx->fb, 0, GAME_H - 10, GAME_W, 10, 0xAA000000u);
            render_text_centered(ctx->fb, s_pve_nodes[n].name,
                                 GAME_H - 9, 0xFFFF8844u);
            break;
        }
    }

    /* ---- Turn action hint (bottom, human players only) ---- */
    if (!k->is_black_knight) {
        if (k->steps_remaining > 0) {
            render_fill_rect(ctx->fb, 0, GAME_H - 20, GAME_W, 9, 0x88000000u);
            render_text_centered(ctx->fb,
                                 "FIRE=Interact  I=Inventory  SPACE=Pass turn",
                                 GAME_H - 20, 0xFF888888u);
        } else {
            render_fill_rect(ctx->fb, 0, GAME_H - 20, GAME_W, 9, 0x88000000u);
            render_text_centered(ctx->fb,
                                 "No steps left.  FIRE=Interact  SPACE=End turn",
                                 GAME_H - 20, 0xFFAA6666u);
        }
    } /* end !is_black_knight hints */
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_overworld(GameCtx *ctx)
{
    /* ---- One-time game initialisation ---- */
    static int overworld_initialised = 0;
    if (!overworld_initialised) {
        /* Initialise knight positions and default stats */
        for (int i = 0; i < MAX_PLAYERS; i++) {
            Knight *k = &ctx->knights[i];
            if (!k->active) continue;
            if (k->map_x == 0 && k->map_y == 0) {
                k->map_x = s_start_x[i];
                k->map_y = s_start_y[i];
            }
            if (k->endurance < 1)    k->endurance    = DEFAULT_ENDURANCE;
            if (k->strength < 1)     k->strength     = 1;
            if (k->constitution < 1) k->constitution = 1;
            if (k->max_hp <= 0)      k->max_hp       = 30;
            if (k->hp <= 0)          k->hp           = k->max_hp;
        }
        /* Initialise black knights */
        black_knight_init(ctx);
        overworld_initialised = 1;
    }

    load_map_background();

    /* ---- Begin new round: reset turn flags and movement budgets ---- */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        Knight *k = &ctx->knights[i];
        if (!k->active || k->dead) continue;
        k->turn_done       = 0;
        k->steps_remaining = steps_for_knight(k);
    }
    ctx->current_knight = 0;
    /* Find the first active, living knight */
    while (ctx->current_knight < MAX_PLAYERS &&
           (!ctx->knights[ctx->current_knight].active ||
             ctx->knights[ctx->current_knight].dead))
        ctx->current_knight++;
    if (ctx->current_knight >= MAX_PLAYERS) {
        ctx->state = STATE_ENDING;
        return;
    }

    /* ---- Dragon: spawn after round >= 2 (LAB_0DCB §4.1) ---- */
    if (ctx->round >= 2 && !ctx->dragon_active)
        dragon_init(ctx);

    /* ---- Start overworld music ---- */
    {
        size_t raw_len = 0;
        uint8_t *raw = moon_file_read("vmusic.cmp", &raw_len);
        if (!raw) raw = moon_file_read("music.cmp", &raw_len);
        if (raw && raw_len > 8) {
            uint32_t ulen = ((uint32_t)raw[4] << 24) | ((uint32_t)raw[5] << 16)
                          | ((uint32_t)raw[6] << 8)  |  (uint32_t)raw[7];
            uint8_t *mod = (uint8_t *)malloc(ulen + 4);
            if (mod) {
                int n = moon_rnc1_decompress(raw, raw_len, mod, ulen + 4);
                if (n > 0) hal_music_play_raw(mod, (size_t)n, 1);
                free(mod);
            }
        }
        free(raw);
    }

    /* ================================================================
     * Main overworld loop
     * Each human player gets their turn in sequence (§3 / §6).
     * After all human players have finished, one round ends.
     * ================================================================ */
    while (ctx->state == STATE_OVERWORLD) {

        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_QUIT;
                hal_music_stop();
                return;
            }
        }

        Knight *k = &ctx->knights[ctx->current_knight];

        /* ---- Process current knight's turn ---- */
        if (!k->turn_done) {

            if (k->human) {
                /* ---- Human player controls ---- */
                int moved  = 0;
                int spd    = 2;

                if (k->steps_remaining > 0) {
                    int dx = 0, dy = 0;
                    if (ctx->input.joy[0].left  ||
                        (ctx->input.keys && ctx->input.keys[80])) dx = -spd;
                    if (ctx->input.joy[0].right ||
                        (ctx->input.keys && ctx->input.keys[79])) dx =  spd;
                    if (ctx->input.joy[0].up    ||
                        (ctx->input.keys && ctx->input.keys[82])) dy = -spd;
                    if (ctx->input.joy[0].down  ||
                        (ctx->input.keys && ctx->input.keys[81])) dy =  spd;

                    if (dx || dy) {
                        k->map_x += dx;
                        k->map_y += dy;
                        /* Clamp to map */
                        if (k->map_x < 0)       k->map_x = 0;
                        if (k->map_x >= GAME_W)  k->map_x = GAME_W - 1;
                        if (k->map_y < 0)        k->map_y = 0;
                        if (k->map_y >= GAME_H)  k->map_y = GAME_H - 1;

                        k->steps_remaining -= spd;
                        if (k->steps_remaining < 0) k->steps_remaining = 0;
                        moved = 1;
                    }
                }
                (void)moved;

                /* FIRE: interact with static or PVE node */
                int just_fire = (ctx->input.joy[0].fire || ctx->input.enter);
                if (just_fire) {
                    int static_node = check_static_node(ctx);
                    if (static_node >= 0) {
                        handle_static_node(ctx, static_node);
                        if (ctx->state != STATE_OVERWORLD) {
                            hal_music_stop();
                            return;
                        }
                        /* After returning from event, end this player's turn */
                        k->turn_done = 1;
                    } else {
                        int pve_node = check_pve_node(ctx);
                        if (pve_node >= 0) {
                            /* Store PVE node index and creature metadata
                             * for combat resolution (moon_combat.c).
                             * creature_type / group / defense come from
                             * the LAB_07BD-derived PveNode table. */
                            ctx->node_type           = 0x02;
                            ctx->node_target_knight  = pve_node;
                            ctx->pve_creature_type   = s_pve_nodes[pve_node].creature_type;
                            ctx->pve_node_group      = s_pve_nodes[pve_node].group;
                            ctx->pve_node_defense    = s_pve_nodes[pve_node].creature_def;
                            ctx->state               = STATE_COMBAT;
                            hal_music_stop();
                            return;
                        }
                    }
                }

                /* SPACE: pass turn */
                if (ctx->input.space) {
                    k->turn_done = 1;
                }

                /* I key (SDL scancode 12): view inventory — placeholder */
                if (ctx->input.keys && ctx->input.keys[12]) {
                    /* TODO: inventory screen (currently no-op) */
                }

            } else {
                /* ---- AI-controlled knight ---- */
                if (k->is_black_knight) {
                    /* Black knight: BK AI (creature wander + player attack) */
                    int hit = bk_turn_step(ctx, ctx->current_knight);
                    if (hit >= 0) {
                        ctx->node_type      = 0x01; /* PvP */
                        ctx->current_knight = hit;
                        ctx->state          = STATE_COMBAT;
                        hal_music_stop();
                        return;
                    }
                } else {
                /* ---- Generic CPU knight: move toward nearest PVE node ---- */
                int best_n = -1, best_d = 0x7fffffff;
                for (int n = 0; n < NUM_PVE_NODES; n++) {
                    if (!s_pve_nodes[n].alive) continue;
                    int dx = s_pve_nodes[n].x - k->map_x;
                    int dy = s_pve_nodes[n].y - k->map_y;
                    int d  = dx * dx + dy * dy;
                    if (d < best_d) { best_d = d; best_n = n; }
                }
                if (best_n >= 0 && k->steps_remaining > 0) {
                    int tx = s_pve_nodes[best_n].x;
                    int ty = s_pve_nodes[best_n].y;
                    if (k->map_x < tx)      k->map_x++;
                    else if (k->map_x > tx) k->map_x--;
                    if (k->map_y < ty)      k->map_y++;
                    else if (k->map_y > ty) k->map_y--;
                    k->steps_remaining--;
                } else {
                    k->turn_done = 1;
                }
                /* AI auto-interacts with static nodes */
                if (k->steps_remaining <= 0 || best_n < 0) {
                    int sn = check_static_node(ctx);
                    if (sn >= 0) handle_static_node(ctx, sn);
                    k->turn_done = 1;
                }
                } /* end generic CPU */
            }

            /* Steps exhausted → auto-end turn */
            if (k->steps_remaining <= 0)
                k->turn_done = 1;
        }

        /* ---- Advance to next active knight if current turn is done ---- */
        if (k->turn_done) {
            int next = ctx->current_knight + 1;
            while (next < MAX_PLAYERS &&
                   (!ctx->knights[next].active ||
                     ctx->knights[next].dead   ||
                     ctx->knights[next].turn_done))
                next++;

            if (next >= MAX_PLAYERS) {
                /* All players have had their turn → new round */
                ctx->round++;
                hal_music_stop();
                /* Dragon: activate if round just reached 2 */
                if (ctx->round >= 2 && !ctx->dragon_active)
                    dragon_init(ctx);
                /* Return to let the outer loop call us again for the
                 * new round (rounds are re-entered through game_run). */
                return;
            }
            ctx->current_knight = next;
        }

        /* ---- Dragon update (autonomous movement + combat trigger) ---- */
        if (ctx->dragon_active) {
            int hit = dragon_update(ctx);
            if (hit >= 0) {
                ctx->node_type           = 0x02; /* dragon = PVE combat */
                ctx->node_target_knight  = hit;
                ctx->current_knight      = hit;
                /* Dragon creature type = 0x14 (LAB_0192, Dragon1.cel/Dragon2.cel)
                 * per the LAB_08C8 handler table fill in LAB_01AE (mog.asm). */
                ctx->pve_creature_type   = 0x14; /* Dragon */
                ctx->pve_node_group      = 0;    /* dragon uses fol sound group */
                ctx->pve_node_defense    = 0x08;
                ctx->state               = STATE_COMBAT;
                hal_music_stop();
                /* Reactivate dragon after combat (it is never destroyed) */
                ctx->dragon_active = 1;
                dragon_init(ctx);
                return;
            }
        }

        draw_overworld(ctx);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }

    hal_music_stop();
}

/* ------------------------------------------------------------------ */
/* PVE node key award (called from moon_combat.c after victory)       */
/* ------------------------------------------------------------------ */

/*
 * overworld_pve_take_key — award the Valley key from a defeated PVE
 * creature to the winning knight.  Sets the creature to dead if
 * it has no more loot/key.
 *
 * Returns 1 if a key was awarded, 0 otherwise.
 */
int overworld_pve_take_key(int node_idx, Knight *k)
{
    if (node_idx < 0 || node_idx >= NUM_PVE_NODES) return 0;
    PveNode *pn = &s_pve_nodes[node_idx];
    if (!pn->alive) return 0;

    int awarded = 0;
    if (pn->key_bit >= 0) {
        /* Award the corresponding key bit */
        k->keys |= (uint8_t)(1u << pn->key_bit);
        pn->key_bit = -1;   /* key taken */
        awarded = 1;
    }

    /* Mark creature as defeated once key is taken (no loot system yet) */
    pn->alive = 0;
    return awarded;
}
