/*
 * moon_village.c — Village mode for Moonstone
 *
 * Each knight has a home village (nodes 0x15–0x18).
 * Visiting own village: restore HP + increase skill.
 * Visiting other village: no effect.
 */

#include "moon_village.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"

#include <string.h>
#include <stdio.h>

static const char *s_village_names[MAX_PLAYERS] = {
    "Richard's Village",
    "Godber's Village",
    "Jeffrey's Village",
    "Edward's Village",
};

static void draw_village(GameCtx *ctx, const char *message)
{
    render_clear(ctx->fb, 0xFF000000u);
    render_fill_rect(ctx->fb, 20, 20, GAME_W - 40, GAME_H - 40, 0xCC111108u);

    /* Determine which village this is */
    int village_faction = ctx->node_type - 0x15; /* 0x15→0, 0x16→1, … */
    if (village_faction < 0 || village_faction >= MAX_PLAYERS)
        village_faction = 0;

    render_text_centered(ctx->fb, s_village_names[village_faction],
                         30, 0xFFFFFF00u);
    render_text_centered(ctx->fb, message, GAME_H / 2, 0xFFCCCCCCu);
    render_text_centered(ctx->fb, "Press FIRE to leave",
                         GAME_H - 20, 0xFF666666u);
}

void game_run_village(GameCtx *ctx)
{
    int village_faction = ctx->node_type - 0x15;
    if (village_faction < 0 || village_faction >= MAX_PLAYERS)
        village_faction = 0;

    Knight *k = &ctx->knights[ctx->current_knight];
    const char *message;
    char msg_buf[128];

    if (k->id == (KnightId)village_faction) {
        /* Own village: heal + skill bonus */
        k->hp = k->max_hp;
        if (k->xp / 50 < 5) {
            k->xp += 25;
        }
        snprintf(msg_buf, sizeof(msg_buf),
                 "Welcome home! HP restored. Skill: %d", k->xp / 50);
        message = msg_buf;
    } else {
        message = "This is not your village.";
    }

    draw_village(ctx, message);
    hal_present(ctx->fb);

    /* Wait for fire / enter */
    for (;;) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_OVERWORLD;
                return;
            }
        }
        if (ctx->input.joy[0].fire || ctx->input.enter || ctx->input.space) {
            ctx->state = STATE_OVERWORLD;
            return;
        }
        draw_village(ctx, message);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}
