/*
 * moon_game.h — Game state machine and top-level orchestration
 *
 * Manages the overall flow of Moonstone:
 *   INTRO → MENU → OVERWORLD → (COMBAT | TOWN | SHOP | WIZARD |
 *   VILLAGE | STONEHENGE | VALLEY) → ENDING
 */

#ifndef MOON_GAME_H
#define MOON_GAME_H

#include "moon_hal.h"
#include "moon_render.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Game states                                                         */
/* ------------------------------------------------------------------ */

typedef enum {
    STATE_INTRO       = 0,
    STATE_MENU        = 1,
    STATE_OVERWORLD   = 2,
    STATE_COMBAT      = 3,
    STATE_TOWN        = 4,
    STATE_SHOP        = 5,
    STATE_WIZARD      = 6,
    STATE_VILLAGE     = 7,
    STATE_STONEHENGE  = 8,
    STATE_ENDING      = 9,
    STATE_QUIT        = 10,
    STATE_VALLEY      = 11   /* Valley of the Gods (node 0x1c, LAB_009D) */
} GameState;

/* ------------------------------------------------------------------ */
/* Player / knight data                                                */
/* ------------------------------------------------------------------ */

#define MAX_PLAYERS  4

typedef enum {
    KNIGHT_RICHARD = 0,  /* SIR RICHARD  — blue   */
    KNIGHT_GODBER  = 1,  /* SIR GODBER   — red    */
    KNIGHT_JEFFREY = 2,  /* SIR JEFFREY  — green  */
    KNIGHT_EDWARD  = 3   /* SIR EDWARD   — yellow */
} KnightId;

typedef struct {
    KnightId id;
    int      active;       /* 1 = participating in this game          */
    int      human;        /* 1 = human-controlled, 0 = AI            */
    int      hp;           /* current hit points                      */
    int      max_hp;       /* maximum hit points                      */
    int      xp;           /* experience points                       */
    int      gold;         /* gold coins                              */
    int      relics;       /* number of relics collected              */
    int      has_moonstone;/* 1 = this knight holds the Moonstone     */
    int      map_x;        /* current overworld node X                */
    int      map_y;        /* current overworld node Y                */
    int      node_idx;     /* current overworld node index            */
    int      dead;         /* 1 = eliminated                          */
    /* Inventory: simple bitmask of collected items */
    uint32_t items;

    /* Combat stats (mog.asm KnightStruct offsets 70,71,72) */
    int      strength;     /* Force      (1–5)                        */
    int      constitution; /* Constitution (1–5)                      */
    int      endurance;    /* Endurance  (1–5) — governs move range   */

    /* Valley of Gods keys: bitmask of 4 keys (bits 0–3).
     * Equivalent to inventaire[20] in the assembler (LAB_069F §1.7) */
    uint8_t  keys;         /* 0x0f = all 4 keys collected             */

    /* Wizard tower visit counter (83(knight) in KnightStruct).
     * 0 = never visited; set to 70 after first visit so subsequent
     * visits are more likely to yield a malus (frog). */
    uint8_t  wizard_visited;

    /* Turn-based movement budget (steps_remaining decrements as the
     * knight moves; when 0 the player must act or pass their turn). */
    int      steps_remaining;

    /* Flag: this knight has ended their overworld turn this round. */
    int      turn_done;

    /* Is this knight currently transformed into a frog? */
    int      is_frog;

    /* Black-knight flag: 1 = this slot is an AI enemy black knight */
    int      is_black_knight;
} Knight;

/* ------------------------------------------------------------------ */
/* Game context                                                        */
/* ------------------------------------------------------------------ */

typedef struct {
    GameState  state;
    GameState  next_state;    /* pending state transition             */
    int        running;       /* non-zero while game loop is active   */

    int        num_players;
    Knight     knights[MAX_PLAYERS];

    int        round;         /* current game round / turn number     */
    int        current_knight;/* index into knights[] for active turn */

    /* asset directory (from command-line argument) */
    char       asset_dir[512];

    /* current active node type for combat/town/etc. triggers */
    int        node_type;
    int        node_target_knight; /* opponent index for PvP          */

    /* PVE combat context — set by overworld when entering STATE_COMBAT
     * from a creature node (node_type == 0x02).
     *
     * pve_creature_type : high word of LAB_07BD entry = offset into
     *   LAB_08C8 handler table, which selects CEL file and AI behaviour:
     *     0x00 → He (enemy knight, He1.ob/He2.ob/He3.ob)
     *     0x04 → Mudmen (Mudmen1.cel / Mudmen2.cel)
     *     0x08 → Balok  (Balok1.cel / Balok2.cel / Balok3.cel)
     *     0x0c → Ratmen generic (Ratmen1.cel)
     *     0x14 → Dragon (Dragon1.cel / Dragon2.cel)
     *     0x18 → TroggAxe  (TroggAxe1.cel / TroggAxe2.cel)
     *     0x1c → TroggSpear (TroggSpear1.cel / TroggSpear2.cel)
     *     0x20 → Demon (Demon1.cel…Demon4.cel)
     *     0x24 → Ratmen variant (Ratmen1.cel)
     *     0x30 → Troll (Troll1.cel / Troll2.cel)
     *     0x40 → Demon/Selene (Sel.cel / Demon4.cel)
     *
     * pve_node_group : which of the 4 overworld groups this node belongs
     *   to (0=fol/west, 1=wal/north, 2=swl/central, 3=gll/northwest).
     *   Determines the sound effect group (fol?.t / wal?.t / swl?.t / gll?.t)
     *   and a hint for background palette selection.
     *
     * pve_node_defense : low word of LAB_07BD = creature defence value
     *   used to scale combat difficulty independently of the knight level.
     */
    int        pve_creature_type;  /* LAB_08C8 byte offset (0x00..0x40) */
    int        pve_node_group;     /* 0=fol, 1=wal, 2=swl, 3=gll       */
    int        pve_node_defense;   /* creature defence value             */

    /* ------------------------------------------------------------------ */
    /* Dragon state (LAB_0617 / LAB_0DCB — mog.asm §4)                    */
    /* The dragon appears after round >= 2, flies autonomously and         */
    /* triggers combat when it collides with a knight.                     */
    /* ------------------------------------------------------------------ */
    int        dragon_active;     /* 1 = dragon is flying on the map       */
    int        dragon_x;          /* current X position (pixels)           */
    int        dragon_y;          /* current Y position (pixels)           */
    int        dragon_vx;         /* X velocity (+2 or -2, inverted at edges) */
    int        dragon_countdown;  /* 100→0: approach phase then pursuit    */
    int        dragon_target;     /* index of target knight (–1 = none)    */
    int        dragon_frame;      /* animation frame (0–15, dg1.cel 34–41) */
    int        dragon_tick;       /* tick counter for frame advance        */

    /* ------------------------------------------------------------------ */
    /* Black knights (IA enemies, LAB_01AE — mog.asm §5)                  */
    /* Count = 4 − num_human_players.  Black knights occupy the unused     */
    /* knights[] slots (is_black_knight=1, human=0).  Their positions and  */
    /* dead/alive state are stored in knights[].map_x/y/dead like any      */
    /* other knight; they participate in the normal turn sequence.         */
    /* ------------------------------------------------------------------ */
    int        bk_count;           /* 4 − num_human_players (0..4)        */
    int        black_knight_target[4]; /* PVE creature-node target, indexed
                                         by knights[] slot (0..3)         */

    /* framebuffer — ARGB8888, GAME_W × GAME_H */
    uint32_t   fb[GAME_W * GAME_H];

    /* current 32-entry ARGB palette */
    uint32_t   palette[MAX_PALETTE];

    /* input snapshot */
    MoonInput  input;

    /* quit-on-escape flag */
    int        quit_on_escape;
} GameCtx;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/**
 * game_init — initialise the game context.
 *
 * @ctx       : caller-allocated GameCtx to initialise.
 * @asset_dir : path to the Moonstone asset directory.
 * @scale     : SDL2 display scale factor (1, 2, or 3).
 *
 * Returns 0 on success, -1 on failure.
 */
int game_init(GameCtx *ctx, const char *asset_dir, int scale);

/** game_shutdown — release all resources. */
void game_shutdown(GameCtx *ctx);

/**
 * game_run — enter the main game loop.
 *
 * Runs until ctx->state == STATE_QUIT or the window is closed.
 * This function blocks until the game ends.
 */
void game_run(GameCtx *ctx);

/* State entry points (implemented in separate .c files) */
void game_run_intro      (GameCtx *ctx);
void game_run_menu       (GameCtx *ctx);
void game_run_overworld  (GameCtx *ctx);
void game_run_combat     (GameCtx *ctx);
void game_run_town       (GameCtx *ctx);
void game_run_shop       (GameCtx *ctx);
void game_run_wizard     (GameCtx *ctx);
void game_run_village    (GameCtx *ctx);
void game_run_stonehenge (GameCtx *ctx);
void game_run_valley     (GameCtx *ctx);
void game_run_ending     (GameCtx *ctx);

/* Palette fade helpers used by multiple states */
void game_fade_out(GameCtx *ctx, int steps);
void game_fade_in (GameCtx *ctx, const uint32_t *target_pal, int steps);

/* Common rendering helpers shared across states */
void game_render_background(GameCtx *ctx, const char *piv_name);
void game_render_text_screen(GameCtx *ctx,
                             const char **lines, int n_lines,
                             uint32_t color);

#ifdef __cplusplus
}
#endif

#endif /* MOON_GAME_H */
