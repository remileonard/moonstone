/*
 * moon_game.c — Game state machine and top-level orchestration
 */

#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Init / shutdown                                                     */
/* ------------------------------------------------------------------ */

int game_init(GameCtx *ctx, const char *asset_dir, int scale)
{
    if (!ctx) return -1;
    memset(ctx, 0, sizeof(*ctx));

    if (asset_dir) {
        strncpy(ctx->asset_dir, asset_dir, sizeof(ctx->asset_dir) - 1);
        ctx->asset_dir[sizeof(ctx->asset_dir) - 1] = '\0';
    }

    /* Initialise libmoon_assets */
    if (moon_init(ctx->asset_dir) != 0) {
        fprintf(stderr, "moon_init failed for '%s'\n", ctx->asset_dir);
        return -1;
    }

    /* Initialise SDL2 HAL */
    if (hal_init("Moonstone — A Hard Days Knight", scale) != 0) {
        moon_shutdown();
        return -1;
    }

    ctx->state         = STATE_INTRO;
    ctx->next_state    = STATE_INTRO;
    ctx->running       = 1;
    ctx->quit_on_escape = 1;

    /* Clear framebuffer to black */
    render_clear(ctx->fb, 0xFF000000u);

    return 0;
}

void game_shutdown(GameCtx *ctx)
{
    if (!ctx) return;
    hal_music_stop();
    hal_quit();
    moon_shutdown();
    ctx->running = 0;
}

/* ------------------------------------------------------------------ */
/* Palette fade helpers                                                */
/* ------------------------------------------------------------------ */

void game_fade_out(GameCtx *ctx, int steps)
{
    if (steps < 1) steps = 16;
    uint32_t saved_pal[MAX_PALETTE];
    render_copy_palette(ctx->palette, saved_pal, MAX_PALETTE);

    for (int s = steps; s >= 0; s--) {
        for (int j = 0; j < GAME_W * GAME_H; j++) {
            uint8_t r = (uint8_t)(((ctx->fb[j] >> 16) & 0xFF) * s / steps);
            uint8_t g = (uint8_t)(((ctx->fb[j] >>  8) & 0xFF) * s / steps);
            uint8_t b = (uint8_t)(( ctx->fb[j]        & 0xFF) * s / steps);
            ctx->fb[j] = 0xFF000000u | ((uint32_t)r << 16)
                       | ((uint32_t)g << 8) | b;
        }
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
    render_clear(ctx->fb, 0xFF000000u);
    hal_present(ctx->fb);
    /* copy faded (black) palette back */
    memset(ctx->palette, 0, MAX_PALETTE * sizeof(uint32_t));
    for (int i = 0; i < MAX_PALETTE; i++)
        ctx->palette[i] = 0xFF000000u;
}

void game_fade_in(GameCtx *ctx, const uint32_t *target_pal, int steps)
{
    if (steps < 1) steps = 16;
    for (int s = 1; s <= steps; s++) {
        render_blend_palette(ctx->palette, target_pal, ctx->palette,
                             MAX_PALETTE, s * 256 / steps);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
    render_copy_palette(target_pal, ctx->palette, MAX_PALETTE);
}

/* ------------------------------------------------------------------ */
/* Shared render helpers                                               */
/* ------------------------------------------------------------------ */

void game_render_background(GameCtx *ctx, const char *piv_name)
{
    if (!piv_name) return;
    MoonPiv *piv = moon_piv_load(piv_name);
    if (!piv) {
        fprintf(stderr, "game_render_background: cannot load '%s'\n", piv_name);
        return;
    }
    render_piv_full(piv, ctx->fb);
    render_build_palette(piv->palette,
                         1 << piv->planes > MAX_PALETTE ? MAX_PALETTE : 1 << piv->planes,
                         ctx->palette);
    moon_piv_free(piv);
}

void game_render_text_screen(GameCtx *ctx,
                             const char **lines, int n_lines,
                             uint32_t color)
{
    render_clear(ctx->fb, 0xFF000000u);
    int y = (GAME_H - n_lines * 10) / 2;
    for (int i = 0; i < n_lines; i++) {
        render_text_centered(ctx->fb, lines[i], y, color);
        y += 10;
    }
}

/* ------------------------------------------------------------------ */
/* Main game loop                                                      */
/* ------------------------------------------------------------------ */

void game_run(GameCtx *ctx)
{
    ctx->running = 1;

    while (ctx->running) {
        /* Poll events */
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || (ctx->quit_on_escape && ctx->input.escape))
                ctx->state = STATE_QUIT;
        }

        switch (ctx->state) {
        case STATE_INTRO:
            game_run_intro(ctx);
            if (ctx->state == STATE_INTRO)
                ctx->state = STATE_MENU;
            break;

        case STATE_MENU:
            game_run_menu(ctx);
            if (ctx->state == STATE_MENU)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_OVERWORLD:
            game_run_overworld(ctx);
            break;

        case STATE_COMBAT:
            game_run_combat(ctx);
            if (ctx->state == STATE_COMBAT)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_TOWN:
            game_run_town(ctx);
            if (ctx->state == STATE_TOWN)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_SHOP:
            game_run_shop(ctx);
            if (ctx->state == STATE_SHOP)
                ctx->state = STATE_TOWN;
            break;

        case STATE_WIZARD:
            game_run_wizard(ctx);
            if (ctx->state == STATE_WIZARD)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_VILLAGE:
            game_run_village(ctx);
            if (ctx->state == STATE_VILLAGE)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_STONEHENGE:
            game_run_stonehenge(ctx);
            if (ctx->state == STATE_STONEHENGE)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_VALLEY:
            game_run_valley(ctx);
            if (ctx->state == STATE_VALLEY)
                ctx->state = STATE_OVERWORLD;
            break;

        case STATE_ENDING:
            game_run_ending(ctx);
            ctx->state = STATE_QUIT;
            break;

        case STATE_QUIT:
        default:
            ctx->running = 0;
            break;
        }
    }
}
