/*
 * moon_game.h — Game state machine and top-level orchestration
 *
 * Manages the overall flow of Moonstone:
 *   INTRO → MENU → OVERWORLD → (COMBAT | TOWN | SHOP | WIZARD |
 *   VILLAGE | STONEHENGE) → ENDING
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
    STATE_QUIT        = 10
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
