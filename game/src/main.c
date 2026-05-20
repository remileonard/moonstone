/*
 * main.c — Entry point for the Moonstone game executable
 *
 * Usage: moonstone [asset_dir] [scale]
 *
 *   asset_dir : path to the directory containing Moonstone game data
 *               (PIV, CEL, CMP files).  Defaults to current directory.
 *   scale     : integer display scale factor (default: 2).
 *
 * The game runs at the original Amiga resolution of 320×200 pixels
 * scaled up by the given factor.
 */

#include "moon_game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char *asset_dir = ".";
    int scale = 2;

    if (argc >= 2) {
        asset_dir = argv[1];
    }
    if (argc >= 3) {
        scale = atoi(argv[2]);
        if (scale < 1) scale = 1;
        if (scale > 4) scale = 4;
    }

    printf("Moonstone — A Hard Day's Knight\n");
    printf("Asset directory : %s\n", asset_dir);
    printf("Display scale   : %d× (%dx%d)\n",
           scale, 320 * scale, 200 * scale);

    GameCtx ctx;
    if (game_init(&ctx, asset_dir, scale) != 0) {
        fprintf(stderr, "Failed to initialise game.\n");
        return 1;
    }

    game_run(&ctx);
    game_shutdown(&ctx);

    return 0;
}
