/*
 * moon_ending.c — Ending sequence for Moonstone
 *
 * Triggered when a knight wins the Moonstone at Stonehenge.
 * Matches LAB_003B / the ending cinematic described in
 * DOC_ANIMATIONS_INTRO_FIN.md §4.
 */

#include "moon_ending.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>

static const char *s_ending_names[MAX_PLAYERS] = {
    "SIR RICHARD",
    "SIR GODBER",
    "SIR JEFFREY",
    "SIR EDWARD",
};

static const uint32_t s_ending_colors[MAX_PLAYERS] = {
    0xFF4444FFu,
    0xFFFF4444u,
    0xFF44FF44u,
    0xFFFFFF44u,
};

/* Ending text lines */
static const char *s_ending_lines[] = {
    "The quest is complete.",
    "",
    "The Moonstone has been claimed",
    "and the land is at peace.",
    "",
    "A new age begins...",
};
#define NUM_ENDING_LINES 6

static int show_lines(GameCtx *ctx, const char **lines, int n,
                      uint32_t color, int hold_frames)
{
    render_clear(ctx->fb, 0xFF000000u);
    int y = (GAME_H - n * 12) / 2;
    for (int i = 0; i < n; i++) {
        render_text_centered(ctx->fb, lines[i], y, color);
        y += 12;
    }
    for (int f = 0; f < hold_frames; f++) {
        hal_present(ctx->fb);
        hal_vbl_wait();
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape ||
                ctx->input.enter || ctx->input.space ||
                ctx->input.joy[0].fire) return 1;
        }
    }
    return 0;
}

/* Fade screen from current to black over @steps frames */
static void fade_to_black(GameCtx *ctx, int steps)
{
    uint32_t save[GAME_W * GAME_H];
    memcpy(save, ctx->fb, sizeof(save));
    for (int s = steps; s >= 0; s--) {
        for (int i = 0; i < GAME_W * GAME_H; i++) {
            uint8_t r = (uint8_t)(((save[i] >> 16) & 0xFF) * s / steps);
            uint8_t g = (uint8_t)(((save[i] >>  8) & 0xFF) * s / steps);
            uint8_t b = (uint8_t)(( save[i]        & 0xFF) * s / steps);
            ctx->fb[i] = 0xFF000000u | ((uint32_t)r << 16)
                       | ((uint32_t)g << 8) | b;
        }
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}

void game_run_ending(GameCtx *ctx)
{
    /* Find the winning knight */
    int winner = -1;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (ctx->knights[i].has_moonstone) { winner = i; break; }
    }
    if (winner < 0) winner = ctx->current_knight;

    hal_music_stop();

    /* Try to show ending background */
    MoonPiv *piv = moon_piv_load("bg4.PIV");
    if (!piv) piv = moon_piv_load("bg4.piv");
    if (piv) {
        render_piv_full(piv, ctx->fb);
        moon_piv_free(piv);
    } else {
        render_clear(ctx->fb, 0xFF000010u);
    }

    /* Fade in */
    for (int s = 1; s <= 20; s++) {
        uint32_t tmp[GAME_W * GAME_H];
        for (int i = 0; i < GAME_W * GAME_H; i++) {
            uint8_t r = (uint8_t)(((ctx->fb[i] >> 16) & 0xFF) * s / 20);
            uint8_t g = (uint8_t)(((ctx->fb[i] >>  8) & 0xFF) * s / 20);
            uint8_t b = (uint8_t)(( ctx->fb[i]        & 0xFF) * s / 20);
            tmp[i] = 0xFF000000u | ((uint32_t)r << 16)
                   | ((uint32_t)g << 8) | b;
        }
        hal_present(tmp);
        hal_vbl_wait();
    }

    hal_delay(1000);

    /* Winner announcement */
    char winner_line[64];
    snprintf(winner_line, sizeof(winner_line), "%s",
             s_ending_names[winner]);
    const char *announce[] = {
        "VICTORY!",
        "",
        winner_line,
        "has claimed the MOONSTONE!"
    };
    show_lines(ctx, announce, 4,
               s_ending_colors[winner], 180);

    fade_to_black(ctx, 20);

    /* Ending text */
    show_lines(ctx, s_ending_lines, NUM_ENDING_LINES, 0xFFCCCCCCu, 250);

    fade_to_black(ctx, 20);

    /* "The End" card */
    const char *the_end[] = { "THE END" };
    show_lines(ctx, the_end, 1, 0xFFFFFF44u, 200);

    fade_to_black(ctx, 20);

    ctx->state = STATE_QUIT;
}
