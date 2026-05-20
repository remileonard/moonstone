/*
 * moon_menu.c — Knight selection menu for Moonstone
 *
 * Allows players to choose which knights participate and how many
 * human players there are. Matches the SECSTRT_4 menu described in
 * DOC_TECHNIQUE.md §3.1 ("JMP SECSTRT_4").
 *
 * Controls:
 *   Up/Down   — move selection cursor
 *   Left/Right — change value
 *   Fire/Enter — confirm selection
 *   Escape     — back / quit
 */

#include "moon_menu.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Knight names and colours                                            */
/* ------------------------------------------------------------------ */

static const char *s_knight_names[MAX_PLAYERS] = {
    "SIR RICHARD",   /* blue   */
    "SIR GODBER",    /* red    */
    "SIR JEFFREY",   /* green  */
    "SIR EDWARD",    /* yellow */
};

static const uint32_t s_knight_colors[MAX_PLAYERS] = {
    0xFF4444FFu,  /* blue   */
    0xFFFF4444u,  /* red    */
    0xFF44FF44u,  /* green  */
    0xFFFFFF44u,  /* yellow */
};

/* Player type labels */
static const char *s_player_types[] = {
    "CPU",
    "HUMAN",
};

/* ------------------------------------------------------------------ */
/* Menu layout                                                         */
/* ------------------------------------------------------------------ */

#define MENU_TITLE_Y   20
#define MENU_START_Y   50
#define MENU_LINE_H    20

/* ------------------------------------------------------------------ */
/* Draw menu                                                           */
/* ------------------------------------------------------------------ */

static void draw_menu(GameCtx *ctx, int selected)
{
    render_clear(ctx->fb, 0xFF000000u);

    /* Title */
    render_text_centered(ctx->fb, "MOONSTONE", MENU_TITLE_Y,      0xFFFFFF00u);
    render_text_centered(ctx->fb, "A Hard Day's Knight",
                         MENU_TITLE_Y + 12, 0xFFCCCCCCu);
    render_text_centered(ctx->fb, "--- SELECT PLAYERS ---",
                         MENU_TITLE_Y + 26, 0xFF888888u);

    /* Knight rows */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        int y = MENU_START_Y + i * MENU_LINE_H;
        /* Cursor */
        if (i == selected)
            render_text(ctx->fb, ">", 20, y, 0xFFFFFFFFu);
        /* Knight name */
        render_text(ctx->fb, s_knight_names[i], 32, y, s_knight_colors[i]);
        /* Type (CPU / HUMAN) */
        int human = ctx->knights[i].human;
        render_text(ctx->fb, s_player_types[human ? 1 : 0],
                    200, y, 0xFFAAAAAAu);
        /* Active indicator */
        if (ctx->knights[i].active)
            render_text(ctx->fb, "[ON] ", 260, y, 0xFF44FF44u);
        else
            render_text(ctx->fb, "[OFF]", 260, y, 0xFF444444u);
    }

    /* Separator */
    render_fill_rect(ctx->fb, 20, MENU_START_Y + MAX_PLAYERS * MENU_LINE_H,
                     GAME_W - 40, 1, 0xFF444444u);

    /* Start row */
    int start_y = MENU_START_Y + (MAX_PLAYERS + 1) * MENU_LINE_H;
    if (selected == MAX_PLAYERS)
        render_text(ctx->fb, ">", 20, start_y, 0xFFFFFFFFu);
    render_text_centered(ctx->fb, "START GAME", start_y, 0xFFFFFFFFu);

    /* Instructions */
    render_text_centered(ctx->fb, "LEFT/RIGHT: change  FIRE: confirm",
                         GAME_H - 24, 0xFF666666u);
    render_text_centered(ctx->fb, "ESC: quit",
                         GAME_H - 14, 0xFF666666u);
}

/* ------------------------------------------------------------------ */
/* Default player setup                                                */
/* ------------------------------------------------------------------ */

static void setup_default_knights(GameCtx *ctx)
{
    for (int i = 0; i < MAX_PLAYERS; i++) {
        ctx->knights[i].id      = (KnightId)i;
        ctx->knights[i].hp      = 100;
        ctx->knights[i].max_hp  = 100;
        ctx->knights[i].xp      = 0;
        ctx->knights[i].gold    = 50;
        ctx->knights[i].relics  = 0;
        ctx->knights[i].map_x   = 0;
        ctx->knights[i].map_y   = 0;
        ctx->knights[i].node_idx = 0;
        ctx->knights[i].dead    = 0;
        ctx->knights[i].items   = 0;
        ctx->knights[i].has_moonstone = 0;
    }
    /* Default: player 1 is human, others are CPU */
    ctx->knights[0].active = 1;
    ctx->knights[0].human  = 1;
    ctx->knights[1].active = 1;
    ctx->knights[1].human  = 0;
    ctx->knights[2].active = 1;
    ctx->knights[2].human  = 0;
    ctx->knights[3].active = 1;
    ctx->knights[3].human  = 0;
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_menu(GameCtx *ctx)
{
    setup_default_knights(ctx);

    int selected = 0;
    int total_rows = MAX_PLAYERS + 1; /* 4 knights + "START GAME" */

    /* Debounce: track previous frame's directional state */
    int prev_up   = 0;
    int prev_down = 0;
    int prev_left = 0;
    int prev_right= 0;
    int prev_fire = 0;

    draw_menu(ctx, selected);
    hal_present(ctx->fb);

    for (;;) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_QUIT;
                return;
            }
        }

        int cur_up    = ctx->input.joy[0].up    || (ctx->input.keys &&
                            ctx->input.keys[18]); /* SDL_SCANCODE_UP */
        int cur_down  = ctx->input.joy[0].down  || (ctx->input.keys &&
                            ctx->input.keys[81]); /* SDL_SCANCODE_DOWN */
        int cur_left  = ctx->input.joy[0].left  || (ctx->input.keys &&
                            ctx->input.keys[80]); /* SDL_SCANCODE_LEFT */
        int cur_right = ctx->input.joy[0].right || (ctx->input.keys &&
                            ctx->input.keys[79]); /* SDL_SCANCODE_RIGHT */
        int cur_fire  = ctx->input.joy[0].fire  || ctx->input.enter ||
                        ctx->input.space;

        /* Navigation */
        if (cur_up && !prev_up) {
            selected = (selected - 1 + total_rows) % total_rows;
        }
        if (cur_down && !prev_down) {
            selected = (selected + 1) % total_rows;
        }

        if (selected < MAX_PLAYERS) {
            /* Left / right toggle type or active */
            if (cur_left && !prev_left) {
                ctx->knights[selected].human ^= 1;
            }
            if (cur_right && !prev_right) {
                ctx->knights[selected].active ^= 1;
                /* Ensure at least one active */
                int any_active = 0;
                for (int i = 0; i < MAX_PLAYERS; i++)
                    if (ctx->knights[i].active) any_active = 1;
                if (!any_active)
                    ctx->knights[selected].active = 1;
            }
            if (cur_fire && !prev_fire) {
                /* Fire on knight row: toggle human/CPU */
                ctx->knights[selected].human ^= 1;
            }
        } else {
            /* "START GAME" row */
            if (cur_fire && !prev_fire) {
                /* Count active players */
                ctx->num_players = 0;
                for (int i = 0; i < MAX_PLAYERS; i++)
                    if (ctx->knights[i].active) ctx->num_players++;
                if (ctx->num_players == 0) ctx->num_players = 1;

                /* Find first active knight */
                ctx->current_knight = 0;
                for (int i = 0; i < MAX_PLAYERS; i++) {
                    if (ctx->knights[i].active) {
                        ctx->current_knight = i;
                        break;
                    }
                }
                ctx->state = STATE_OVERWORLD;
                return;
            }
        }

        prev_up    = cur_up;
        prev_down  = cur_down;
        prev_left  = cur_left;
        prev_right = cur_right;
        prev_fire  = cur_fire;

        draw_menu(ctx, selected);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}
