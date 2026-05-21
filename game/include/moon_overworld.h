/*
 * moon_overworld.h
 */
#ifndef MOON_OVERWORLD_H
#define MOON_OVERWORLD_H
#include "moon_game.h"
#ifdef __cplusplus
extern "C" {
#endif
void game_run_overworld(GameCtx *ctx);

/*
 * overworld_pve_take_key — award the Valley key held by a defeated
 * PVE creature (node_idx) to knight k.  Called by moon_combat.c
 * after a PVE victory (node_type == 0x02).
 * Returns 1 if a key was awarded, 0 otherwise.
 */
int overworld_pve_take_key(int node_idx, Knight *k);

#ifdef __cplusplus
}
#endif
#endif /* MOON_OVERWORLD_H */
