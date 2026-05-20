/*
 * moon_shop.c — Shop mode for Moonstone
 *
 * Simple buy/sell interface. Items use a bitmask in Knight.items.
 */

#include "moon_shop.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"

#include <string.h>
#include <stdio.h>

/* Item definitions */
typedef struct {
    const char *name;
    int         cost;
    uint32_t    flag;
} ShopItem;

static const ShopItem s_items[] = {
    { "Health Potion  (heal 30hp)",  15, 0x01 },
    { "Magic Shield   (+20 def)",    40, 0x02 },
    { "Valley Key     (enter gods)", 60, 0x04 },
    { "Enchanted Sword(+10 atk)",    50, 0x08 },
    { "Armour Plate   (+15 def)",    35, 0x10 },
};
#define NUM_ITEMS 5

static void draw_shop(GameCtx *ctx, int selected)
{
    render_clear(ctx->fb, 0xFF000000u);
    render_fill_rect(ctx->fb, 20, 10, GAME_W - 40, GAME_H - 20, 0xCC111111u);
    render_text_centered(ctx->fb, "-- SHOP --", 16, 0xFFFFFF00u);

    Knight *k = &ctx->knights[ctx->current_knight];
    char gold_str[32];
    snprintf(gold_str, sizeof(gold_str), "Gold: %d", k->gold);
    render_text(ctx->fb, gold_str, 24, 28, 0xFFFFAA00u);

    for (int i = 0; i < NUM_ITEMS; i++) {
        int y = 44 + i * 16;
        int owned = (k->items & s_items[i].flag) != 0;
        uint32_t col;
        if (owned)        col = 0xFF444444u;
        else if (i == selected) col = 0xFFFFFFFFu;
        else              col = 0xFF888888u;
        if (i == selected) render_text(ctx->fb, ">", 24, y, 0xFFFFFFFFu);
        render_text(ctx->fb, s_items[i].name, 34, y, col);
        char cost_s[16];
        snprintf(cost_s, sizeof(cost_s), "%dg", s_items[i].cost);
        render_text(ctx->fb, cost_s, GAME_W - 50, y, 0xFFFFAA00u);
        if (owned) render_text(ctx->fb, "[OWNED]", GAME_W - 90, y, 0xFF444444u);
    }

    /* Leave option */
    int leave_y = 44 + NUM_ITEMS * 16 + 8;
    if (selected == NUM_ITEMS)
        render_text(ctx->fb, ">", 24, leave_y, 0xFFFFFFFFu);
    render_text_centered(ctx->fb, "Leave shop", leave_y,
                         selected == NUM_ITEMS ? 0xFFFFFFFFu : 0xFF888888u);

    render_text_centered(ctx->fb, "FIRE: buy  ESC: leave",
                         GAME_H - 12, 0xFF555555u);
}

void game_run_shop(GameCtx *ctx)
{
    int selected = 0;
    int total = NUM_ITEMS + 1;
    int prev_up = 0, prev_down = 0, prev_fire = 0;

    while (ctx->state == STATE_SHOP) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_TOWN;
                return;
            }
        }

        int cur_up   = ctx->input.joy[0].up   || (ctx->input.keys && ctx->input.keys[82]);
        int cur_down = ctx->input.joy[0].down  || (ctx->input.keys && ctx->input.keys[81]);
        int cur_fire = ctx->input.joy[0].fire  || ctx->input.enter || ctx->input.space;

        if (cur_up   && !prev_up)   selected = (selected - 1 + total) % total;
        if (cur_down && !prev_down) selected = (selected + 1) % total;

        if (cur_fire && !prev_fire) {
            if (selected == NUM_ITEMS) {
                ctx->state = STATE_TOWN;
                return;
            } else {
                Knight *k = &ctx->knights[ctx->current_knight];
                const ShopItem *item = &s_items[selected];
                if (!(k->items & item->flag) && k->gold >= item->cost) {
                    k->gold -= item->cost;
                    k->items |= item->flag;
                    /* Apply effect immediately for health potion */
                    if (item->flag == 0x01) {
                        k->hp += 30;
                        if (k->hp > k->max_hp) k->hp = k->max_hp;
                        k->items &= ~item->flag; /* consumed */
                    }
                }
            }
        }

        prev_up   = cur_up;
        prev_down = cur_down;
        prev_fire = cur_fire;

        draw_shop(ctx, selected);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}
