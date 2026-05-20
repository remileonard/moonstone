/*
 * moon_stonehenge.c — Stonehenge and Valley of Gods for Moonstone
 *
 * Stonehenge (node 0x1b): to claim the Moonstone, the knight must
 *   enter with all relics (or after defeating the Valley Gods).
 *
 * Valley of Gods (node 0x1c): final boss battle — triggers the ending
 *   sequence on victory.
 */

#include "moon_stonehenge.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>

static void draw_stonehenge_screen(GameCtx *ctx,
                                   const char **lines, int n,
                                   uint32_t color)
{
    /* Try stonehenge background */
    MoonPiv *piv = moon_piv_load("bg4.PIV");
    if (!piv) piv = moon_piv_load("bg4.piv");
    if (piv) { render_piv_full(piv, ctx->fb); moon_piv_free(piv); }
    else       render_clear(ctx->fb, 0xFF050510u);

    render_fill_rect(ctx->fb, 10, 10, GAME_W - 20, GAME_H - 20, 0xAA000000u);

    int y = (GAME_H - n * 12) / 2;
    for (int i = 0; i < n; i++) {
        render_text_centered(ctx->fb, lines[i], y, color);
        y += 12;
    }
    render_text_centered(ctx->fb, "Press FIRE to continue",
                         GAME_H - 12, 0xFF555555u);
}

static void wait_for_fire(GameCtx *ctx)
{
    for (;;) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) return;
        }
        if (ctx->input.joy[0].fire || ctx->input.enter || ctx->input.space)
            return;
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}

void game_run_stonehenge(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];

    if (ctx->node_type == 0x1c) {
        /* Valley of Gods — final boss combat */
        const char *pre_lines[] = {
            "VALLEY OF THE GODS",
            "",
            "The gods await...",
            "Only the strongest knight",
            "may claim the Moonstone."
        };
        draw_stonehenge_screen(ctx, pre_lines, 5, 0xFFFFCC44u);
        hal_present(ctx->fb);
        wait_for_fire(ctx);

        /* Trigger final boss combat */
        ctx->node_type = 0x1c;
        ctx->state = STATE_COMBAT;
        return;
    }

    /* Stonehenge (0x1b) */
    if (k->has_moonstone) {
        /* Knight already has the Moonstone — trigger ending */
        ctx->state = STATE_ENDING;
        return;
    }

    /* Check if knight qualifies to claim the Moonstone */
    int can_claim = (k->relics >= 2) || (k->xp >= 150);

    if (can_claim) {
        const char *claim_lines[] = {
            "STONEHENGE",
            "",
            "The Moonstone glows...",
            "You are worthy!",
            "The Moonstone is yours!"
        };
        draw_stonehenge_screen(ctx, claim_lines, 5, 0xFFFFFFAAu);
        hal_present(ctx->fb);
        wait_for_fire(ctx);

        k->has_moonstone = 1;
        ctx->state = STATE_ENDING;
    } else {
        char relic_msg[64];
        snprintf(relic_msg, sizeof(relic_msg),
                 "You have %d relics. Need more...", k->relics);
        const char *not_ready[] = {
            "STONEHENGE",
            "",
            "The Moonstone remains dark.",
            relic_msg,
            "Grow stronger first."
        };
        draw_stonehenge_screen(ctx, not_ready, 5, 0xFF888888u);
        hal_present(ctx->fb);
        wait_for_fire(ctx);
        ctx->state = STATE_OVERWORLD;
    }
}
