/*
 * moon_wizard.h — Math the Wizard tower event
 *
 * This header exports the wizard tower's map-node descriptor so that
 * moon_overworld.c can reference it without hardcoding wizard-specific
 * values.  All wizard logic lives in moon_wizard.c.
 *
 * Node data (LAB_069F §1.6, mog.asm LAB_007C):
 *   type  0x1e  "Visit Math the Wizard"
 *   X=217, Y=11 (top-edge, between Richard and Godber villages)
 *   li1.cel frame 2, ov1.cel frame 3
 */
#ifndef MOON_WIZARD_H
#define MOON_WIZARD_H
#include "moon_game.h"
#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Wizard tower map-node descriptor (owned by this module)             */
/* ------------------------------------------------------------------ */

#define WIZARD_NODE_TYPE   0x1e
#define WIZARD_NODE_X      217
#define WIZARD_NODE_Y       11
#define WIZARD_NODE_NAME   "Math the Wizard"
#define WIZARD_LI_FRAME      2   /* li1.cel sprite frame on the overworld map */
#define WIZARD_ICON_FRAME    3   /* ov1.cel fallback icon frame               */

/* ------------------------------------------------------------------ */
/* Public API                                                           */
/* ------------------------------------------------------------------ */

void game_run_wizard(GameCtx *ctx);

#ifdef __cplusplus
}
#endif
#endif /* MOON_WIZARD_H */
