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
/* Opcode definitions                                                  */
/* ------------------------------------------------------------------ */

/* Draw opcode helper — encodes a slot index as (slot * 4).
 * The resulting byte always has bit 7 clear (< 0x80). */
#define IX_DRAW(slot)           ((uint8_t)((slot) << 2))

/* End-of-step marker byte */
#define IX_STEP_END             0xFF

/* Second byte of the FF XX end-of-step pair */
#define IX_TERM_NEXT            0x00  /* FF 00 — advance to next step      */
#define IX_TERM_LOOP            0xFE  /* FF FE — loop back to loop_pc      */
#define IX_TERM_SCRIPT_END      0xFF  /* FF FF — end of script             */

/* Compound step-separator helpers — combine IX_STEP_END with its
 * terminator byte into a single, self-documenting token.  Use these
 * instead of the raw IX_STEP_END, IX_TERM_* pairs in script arrays. */
#define IX_STEP_NEXT            IX_STEP_END, IX_TERM_NEXT
#define IX_STEP_LOOP            IX_STEP_END, IX_TERM_LOOP
#define IX_STEP_SCRIPT_END      IX_STEP_END, IX_TERM_SCRIPT_END

/* Control opcodes (bit 7 set, i.e. op >= 0x80) */
#define IX_OP_SET_DIRECTION     0x80  /* LAB_0215: direction set/toggle    */
#define IX_OP_JUMP_VARIANT      0x84  /* LAB_0218: jump variant            */
#define IX_OP_SET_SPEED         0x88  /* LAB_021A: set animation speed     */
#define IX_OP_SKIP8             0x8C  /* LAB_021E: skip 8-byte block       */
#define IX_OP_SET_LOOP_COUNT    0x94  /* LAB_021F: init loop counter       */
#define IX_OP_NOP_98            0x98  /* LAB_0220: NOP                     */
#define IX_OP_NOP_9C            0x9C  /* LAB_0220: NOP                     */
#define IX_OP_MOVE_DELTA        0xA0  /* LAB_0222: move/set base_x/y/vel   */
#define IX_OP_SKIP4             0xA4  /* LAB_0221: skip 4-byte block       */
#define IX_OP_NOP_A8            0xA8  /* LAB_0241: NOP (just RTS)          */
#define IX_OP_NOP_AC            0xAC  /* LAB_022D: NOP                     */
#define IX_OP_NOP_B0            0xB0  /* LAB_022C: NOP                     */
#define IX_OP_CALL              0xB4  /* LAB_022F/0231: subroutine call    */
#define IX_OP_SKIP6_B8          0xB8  /* LAB_0233: skip 6-byte block       */
#define IX_OP_SKIP6_BC          0xBC  /* LAB_0234: skip 6-byte block       */
#define IX_OP_KILL              0xC0  /* LAB_0235: clear entity type       */
#define IX_OP_SET_ASSET_TABLE   0xC4  /* LAB_0236: set asset table         */
#define IX_OP_SKIP6_C8          0xC8  /* LAB_0232: skip 6-byte block       */
#define IX_OP_UNK_CC            0xCC  /* LAB_0237: 8-byte instruction      */
#define IX_OP_COND_JUMP         0xD0  /* LAB_023B: conditional jump        */
#define IX_OP_CLEAR_SCRATCH     0xD4  /* LAB_023F: clear scratch + adv 2   */

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
/* Bounding-box query                                                  */
/* ------------------------------------------------------------------ */

/* IxBBox — screen-space bounding box of a single drawn sprite.       */
typedef struct {
    int x;  /* left edge (pixels; may be negative if clipped)         */
    int y;  /* top  edge (pixels; may be negative if clipped)         */
    int w;  /* width  in pixels                                        */
    int h;  /* height in pixels                                        */
} IxBBox;

/* ------------------------------------------------------------------ */
/* API                                                                 */
/* ------------------------------------------------------------------ */

/*
 * ix_ctrl_op_size — return the byte size (including the opcode byte)
 * of a control instruction (op >= 0x80).
 *
 * Derived from the ADDI.L advances in each handler in program.asm.
 */
int ix_ctrl_op_size(uint8_t op);

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

/*
 * ix_entity_get_bboxes — compute the screen-space bounding boxes for all
 * sprites in the current animation step without modifying the entity.
 *
 * Mirrors the scan done by ix_entity_draw() but outputs bounding rectangles
 * instead of blitting pixels.  Control instructions (SET_DIRECTION,
 * MOVE_DELTA) within the step are simulated on a local copy of the entity
 * state so that the result is accurate even when in-step transforms occur.
 *
 * @e         — entity (read-only).
 * @slots     — CEL asset table.
 * @bboxes    — output array; must have room for at least max_bboxes entries.
 * @max_bboxes — maximum number of entries to write.
 *
 * Returns the number of bounding boxes written (0 if the entity is done).
 */
int ix_entity_get_bboxes(const IxEntity *e, const IxCelSlots *slots,
                          IxBBox *bboxes, int max_bboxes);

#endif /* IMAGEXCEL_H */
