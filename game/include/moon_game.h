/*
 * moon_game.h — hôte SDL du jeu porté de l'original : fenêtre, entrées,
 * son (moon_hal), puis program (intro, fin) et mog (menu, partie) en C.
 */
#ifndef MOON_GAME_H
#define MOON_GAME_H

#include "moon_hal.h"
#include <stdint.h>

typedef struct {
    char      asset_dir[512];   /* fichiers du jeu d'origine          */
    int       running;
    uint32_t  fb[GAME_W * GAME_H];
    MoonInput input;
} GameCtx;

int  game_init(GameCtx *ctx, const char *asset_dir, int scale);
void game_shutdown(GameCtx *ctx);

/* Le jeu : intro (program), menu et parties (mog), fin après une victoire */
void game_run_mog(GameCtx *ctx);
/* La fin de l'original (EXT_0007 = flags) sans partie, puis le jeu */
void game_run_mog_ending(GameCtx *ctx, int flags);
/* Combats seuls (mog_fight) : rencontre `encounter` (octet de
 * t_CreatureInit), toutes à la suite si all, lieu (0-3, -1 : tour à tour) */
void game_run_combats(GameCtx *ctx, int encounter, int all, int place);

#endif /* MOON_GAME_H */
