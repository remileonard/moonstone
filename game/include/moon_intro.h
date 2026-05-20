/*
 * moon_intro.h — introduction sequence
 */

#ifndef MOON_INTRO_H
#define MOON_INTRO_H

#include "moon_game.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Entry point — called from game_run() when state == STATE_INTRO */
void game_run_intro(GameCtx *ctx);

#ifdef __cplusplus
}
#endif

#endif /* MOON_INTRO_H */
