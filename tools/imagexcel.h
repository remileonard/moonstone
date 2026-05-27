/*
 * imagexcel.h — IMAGEXCEL sprite animation engine for Moonstone.
 *
 * Implements the interpreter loop from LAB_01F1/LAB_01F3 (program.asm) in C.
 * Script format, entity layout, and opcode semantics are documented in
 * docs/DOC_MOTEUR_IMAGEXCEL.md.
 *
 * Each entity owns a pointer into a byte-code animation script and a speed
 * counter.  On every tick the entity is drawn; when the counter expires the
 * interpreter advances past the current end-of-step marker (FF XX) to the
 * start of the next step.
 *
 * Only the opcodes that appear in the dw1.cel scripts (and the minimum
 * required to parse the script safely) are implemented here.  Unknown
 * control opcodes are skipped using the size table derived from the
 * assembly.
 */

#ifndef IMAGEXCEL_H
#define IMAGEXCEL_H

#include "moon_assets.h"
#include <stdint.h>

/* Number of CEL slots in the asset table (mirrors LAB_0276 DS.L 10).
 * A draw opcode byte encodes the slot as: slot_index = (opcode & 0x1F) / 4. */
#define IX_SLOTS 10

/* ------------------------------------------------------------------ */
/* CEL asset table                                                     */
/* ------------------------------------------------------------------ */

/* IxCelSlots — maps a slot index (0..IX_SLOTS-1) to a loaded MoonCel.
 * NULL entries are silently skipped when a draw instruction references
 * that slot. */
typedef struct {
    MoonCel *cel[IX_SLOTS];
} IxCelSlots;

/* ------------------------------------------------------------------ */
/* Animation entity                                                    */
/* ------------------------------------------------------------------ */

typedef struct {
    int16_t base_x;    /* screen X origin of this entity (6(A1) in asm)     */
    int16_t base_y;    /* screen Y origin of this entity (8(A1) in asm)     */
    int16_t vel_y;     /* vertical velocity added to y_delta (10(A1))       */
    uint8_t direction; /* 0=normal, bit1=horizontal flip (22(A1))           */

    uint8_t  speed;    /* VBL ticks per animation step (0(A5))              */
    int      timer;    /* remaining ticks before advancing to the next step */

    /* Loop counter state — SET_LOOP_COUNT (0x94) / FF FE handling */
    uint8_t        loop_count;  /* remaining loop iterations (6(A5))        */
    uint8_t        loop_active; /* non-zero while a loop is in progress (7(A5)) */
    const uint8_t *loop_pc;     /* loop-back address (8(A5))                */

    const uint8_t *script;      /* first byte of the full animation script  */
    const uint8_t *frame_start; /* start of the current step in the script  */

    int finished; /* 1 after FF FF (end-of-script) has been seen           */
} IxEntity;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/*
 * ix_entity_init — attach an animation script to an entity.
 *
 * Sets frame_start = script, speed = 1, timer = 1, finished = 0.
 * The entity position (base_x/base_y/vel_y/direction) must be set by
 * the caller after this call.
 */
void ix_entity_init(IxEntity *e, const uint8_t *script);

/*
 * ix_entity_draw — render the current animation step to a framebuffer.
 *
 * Processes the script from frame_start, executing control instructions
 * (SET_SPEED, CALL-skip, etc.) and blitting each draw instruction's CEL
 * frame to fb.  Stops at the first FF byte (end-of-step marker).
 *
 * Parameters:
 *   slots   — CEL asset table; slot 3 must point to dw1.cel for 0x0C opcodes.
 *   palette — 32-entry ARGB8888 palette; index 0 is transparent (skipped).
 *   fb      — destination ARGB8888 framebuffer (fb_w × fb_h pixels).
 *   fb_w    — framebuffer width  (typically 320).
 *   fb_h    — framebuffer height (typically 200).
 */
void ix_entity_draw(IxEntity *e, const IxCelSlots *slots,
                    const uint32_t *palette, uint32_t *fb,
                    int fb_w, int fb_h);

/*
 * ix_entity_advance — move to the next animation step.
 *
 * Scans forward from frame_start past the FF XX end-of-step pair to the
 * first byte of the next step.  Handles:
 *   FF 00  → advance to next step (PC += 2)
 *   FF FE  → loop back to script start
 *   FF FF  → end of script; sets finished = 1; returns 1
 *
 * Also resets the timer to e->speed.
 * Returns 1 if end-of-script was reached, 0 otherwise.
 */
int ix_entity_advance(IxEntity *e);

/*
 * ix_entity_tick — convenience: draw + conditionally advance.
 *
 * Calls ix_entity_draw(), decrements the timer, and calls
 * ix_entity_advance() when the timer reaches zero.
 *
 * Returns 1 if the script has finished, 0 otherwise.
 */
int ix_entity_tick(IxEntity *e, const IxCelSlots *slots,
                   const uint32_t *palette, uint32_t *fb,
                   int fb_w, int fb_h);

#endif /* IMAGEXCEL_H */
