/*
 * moon_town.c — Town mode for Moonstone (Highwood / Waterdeep)
 *
 * Towns offer 5 menu options (DOC_MODE_OVERWORLD.md §1.6 Nœud 5):
 *   1. Arena  (combat PvE, gain gold)
 *   2. Shop   (buy/sell equipment)
 *   3. Wizard (skill upgrade)
 *   4. Forge  (weapon/armour upgrade)
 *   5. Leave
 */

#include "moon_town.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>

static const char *s_town_options[] = {
    "1. Arena combat",
    "2. Shop",
    "3. Wizard (skill upgrade)",
    "4. Forge (weapon upgrade)",
    "5. Leave town",
};
#define NUM_TOWN_OPTIONS 5

static const char *town_name_for_type(int type)
{
    if (type == 0x19) return "HIGHWOOD";
    if (type == 0x1a) return "WATERDEEP";
    return "CITY";
}

static void draw_town(GameCtx *ctx, int selected)
{
    render_clear(ctx->fb, 0xFF000000u);

    /* Try to show town background */
    MoonPiv *piv = moon_piv_load("bg2.PIV");
    if (!piv) piv = moon_piv_load("bg2.piv");
    if (piv) {
        render_piv_full(piv, ctx->fb);
        moon_piv_free(piv);
    }

    /* Dark overlay */
    render_fill_rect(ctx->fb, 30, 20, GAME_W - 60, GAME_H - 40, 0xCC000000u);

    const char *name = town_name_for_type(ctx->node_type);
    char title[64];
    snprintf(title, sizeof(title), "Welcome to %s", name);
    render_text_centered(ctx->fb, title, 28, 0xFFFFFF00u);

    for (int i = 0; i < NUM_TOWN_OPTIONS; i++) {
        int y = 50 + i * 18;
        uint32_t col = (i == selected) ? 0xFFFFFFFFu : 0xFF888888u;
        if (i == selected)
            render_text(ctx->fb, ">", 34, y, 0xFFFFFFFFu);
        render_text(ctx->fb, s_town_options[i], 44, y, col);
    }

    /* Knight HP/gold */
    Knight *k = &ctx->knights[ctx->current_knight];
    char info[64];
    snprintf(info, sizeof(info), "HP: %d/%d  GOLD: %d",
             k->hp, k->max_hp, k->gold);
    render_text_centered(ctx->fb, info, GAME_H - 14, 0xFF888888u);
}

void game_run_town(GameCtx *ctx)
{
    int selected = 0;
    int prev_up = 0, prev_down = 0, prev_fire = 0;

    while (ctx->state == STATE_TOWN) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_OVERWORLD;
                return;
            }
        }

        int cur_up   = ctx->input.joy[0].up   || (ctx->input.keys && ctx->input.keys[82]);
        int cur_down = ctx->input.joy[0].down  || (ctx->input.keys && ctx->input.keys[81]);
        int cur_fire = ctx->input.joy[0].fire  || ctx->input.enter || ctx->input.space;

        if (cur_up   && !prev_up)   selected = (selected - 1 + NUM_TOWN_OPTIONS) % NUM_TOWN_OPTIONS;
        if (cur_down && !prev_down) selected = (selected + 1) % NUM_TOWN_OPTIONS;

        if (cur_fire && !prev_fire) {
            switch (selected) {
            case 0: /* Arena */
                ctx->node_type = 0x02; /* PvE creature */
                ctx->state = STATE_COMBAT;
                return;
            case 1: /* Shop */
                ctx->state = STATE_SHOP;
                return;
            case 2: /* Wizard */
                ctx->state = STATE_WIZARD;
                return;
            case 3: /* Forge — upgrade weapon (simple: +5 max_hp for now) */
                {
                    Knight *k = &ctx->knights[ctx->current_knight];
                    if (k->gold >= 30) {
                        k->gold   -= 30;
                        k->max_hp += 10;
                        if (k->hp > k->max_hp) k->hp = k->max_hp;
                    }
                }
                break;
            case 4: /* Leave */
                ctx->state = STATE_OVERWORLD;
                return;
            }
        }

        prev_up   = cur_up;
        prev_down = cur_down;
        prev_fire = cur_fire;

        draw_town(ctx, selected);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}
