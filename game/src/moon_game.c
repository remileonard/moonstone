/*
 * moon_game.c — mise en route de l'hôte : fichiers du jeu, fenêtre SDL.
 */
#include "moon_game.h"
#include "moon_assets.h"

#include <stdio.h>
#include <string.h>

int game_init(GameCtx *ctx, const char *asset_dir, int scale)
{
    memset(ctx, 0, sizeof *ctx);
    if (asset_dir)
        strncpy(ctx->asset_dir, asset_dir, sizeof ctx->asset_dir - 1);
    if (moon_init(ctx->asset_dir) != 0) {
        fprintf(stderr, "moon_init failed for '%s'\n", ctx->asset_dir);
        return -1;
    }
    if (hal_init("Moonstone — A Hard Days Knight", scale) != 0) {
        moon_shutdown();
        return -1;
    }
    ctx->running = 1;
    return 0;
}

void game_shutdown(GameCtx *ctx)
{
    hal_music_stop();
    hal_quit();
    moon_shutdown();
    ctx->running = 0;
}
