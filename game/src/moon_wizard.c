/*
 * moon_wizard.c — Wizard's Tower (Math the Wizard) for Moonstone
 *
 * The wizard can heal the knight and upgrade skill_level.
 * (DOC_MODE_OVERWORLD.md §1.6 node 0x1e "Math the Wizard")
 */

#include "moon_wizard.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"

#include <string.h>
#include <stdio.h>

static const char *s_wizard_options[] = {
    "Heal (cost: 20 gold)",
    "Upgrade Skill (cost: 50 gold)",
    "Leave",
};
#define NUM_WIZARD_OPTIONS 3

static void draw_wizard(GameCtx *ctx, int selected)
{
    render_clear(ctx->fb, 0xFF000000u);
    render_fill_rect(ctx->fb, 20, 10, GAME_W - 40, GAME_H - 20, 0xCC080820u);
    render_text_centered(ctx->fb, "MATH THE WIZARD", 16, 0xFFAA44FFu);
    render_text_centered(ctx->fb, "\"Greetings, brave knight!\"",
                         28, 0xFF888888u);

    Knight *k = &ctx->knights[ctx->current_knight];
    char info[64];
    snprintf(info, sizeof(info), "HP: %d/%d  SKILL: %d  GOLD: %d",
             k->hp, k->max_hp, k->xp / 50, k->gold);
    render_text_centered(ctx->fb, info, 40, 0xFF888888u);

    for (int i = 0; i < NUM_WIZARD_OPTIONS; i++) {
        int y = 60 + i * 20;
        uint32_t col = (i == selected) ? 0xFFFFFFFFu : 0xFF888888u;
        if (i == selected) render_text(ctx->fb, ">", 30, y, 0xFFAA44FFu);
        render_text(ctx->fb, s_wizard_options[i], 40, y, col);
    }
}

void game_run_wizard(GameCtx *ctx)
{
    int selected = 0;
    int prev_up = 0, prev_down = 0, prev_fire = 0;

    while (ctx->state == STATE_WIZARD) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = (ctx->node_type == 0x1e)
                             ? STATE_OVERWORLD : STATE_TOWN;
                return;
            }
        }

        int cur_up   = ctx->input.joy[0].up   || (ctx->input.keys && ctx->input.keys[82]);
        int cur_down = ctx->input.joy[0].down  || (ctx->input.keys && ctx->input.keys[81]);
        int cur_fire = ctx->input.joy[0].fire  || ctx->input.enter || ctx->input.space;

        if (cur_up   && !prev_up)   selected = (selected - 1 + NUM_WIZARD_OPTIONS) % NUM_WIZARD_OPTIONS;
        if (cur_down && !prev_down) selected = (selected + 1) % NUM_WIZARD_OPTIONS;

        if (cur_fire && !prev_fire) {
            Knight *k = &ctx->knights[ctx->current_knight];
            switch (selected) {
            case 0: /* Heal */
                if (k->gold >= 20) {
                    k->gold -= 20;
                    k->hp    = k->max_hp;
                }
                break;
            case 1: /* Upgrade skill */
                if (k->gold >= 50) {
                    k->gold -= 50;
                    k->xp   += 50;
                }
                break;
            case 2: /* Leave */
                ctx->state = (ctx->node_type == 0x1e)
                             ? STATE_OVERWORLD : STATE_TOWN;
                return;
            }
        }

        prev_up   = cur_up;
        prev_down = cur_down;
        prev_fire = cur_fire;

        draw_wizard(ctx, selected);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}
