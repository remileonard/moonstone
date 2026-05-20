/*
 * moon_entity.h — Entity pool and animation bytecode interpreter
 *
 * Re-implements the Amiga job-manager / entity system described in
 * DOC_ANIMATIONS_INTRO_FIN.md §2 and DOC_TECHNIQUE.md §4.
 *
 * The entity pool holds 40 entries of 42 bytes each.  Each entry runs
 * a 6-byte bytecode script driven by LAB_01F1 semantics (see §2.3 of
 * DOC_ANIMATIONS_INTRO_FIN.md for the full opcode table).
 */

#ifndef MOON_ENTITY_H
#define MOON_ENTITY_H

#include "moon_assets.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Constants                                                           */
/* ------------------------------------------------------------------ */

#define ENTITY_POOL_SIZE  40    /* maximum simultaneous entities      */
#define CEL_SLOTS          8    /* asset table slots per entity        */

/* ------------------------------------------------------------------ */
/* Frame state (matches LAB_0284 layout, 48 bytes)                    */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t  speed;          /* +0  : ticks-per-frame counter        */
    uint8_t  active;         /* +1  : 1 = this frame state is active */
    uint8_t *inner_pc;       /* +2  : saved PC for inner loop        */
    uint8_t  loop_count;     /* +6  : counter for LOOP_INIT          */
    uint8_t  looping;        /* +7  : non-zero while looping         */
    uint8_t *loop_addr;      /* +8  : loop-back address              */
    uint8_t *death_addr;     /* +12 : address of death animation     */
    uint8_t  anim_changed;   /* +16 : flag: animation changed        */
    uint8_t *next_anim;      /* +20 : pointer to next animation      */
    uint8_t  has_callback;   /* +26 : flag: callback pending         */
    uint8_t  callback_timer; /* +27 : countdown for callback         */
    uint8_t *target_addr;    /* +28 : callback target / cond branch  */
    uint8_t  respawn_flag;   /* +42 : respawn flag                   */
    uint8_t  tick_count;     /* current tick within speed interval   */
} FrameState;

/* ------------------------------------------------------------------ */
/* Entity (matches LAB_0282 layout, 42 bytes)                         */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t      active;                  /* +0  */
    uint8_t      running;                 /* +1  */
    uint8_t     *script_pc;               /* +2  */
    int16_t      base_x;                  /* +6  */
    int16_t      base_y;                  /* +8  */
    int16_t      vel_y;                   /* +10 */
    int16_t      screen_x;               /* +12 */
    int16_t      screen_y;               /* +14 */
    uint16_t     sprite_w;               /* +16 */
    uint16_t     sprite_h;               /* +18 */
    uint8_t      frame_param;            /* +20 */
    uint8_t      last_param;             /* +21 */
    uint8_t      direction;              /* +22 : bit0=flip_x, bit1=flip_y */
    uint8_t      _pad23;
    uint32_t     entity_id;              /* +24 */
    void        *asset_table[CEL_SLOTS]; /* +28 : pointers to MoonCel/MoonOb */
    uint8_t      script_type;            /* +32 */
    FrameState  *frame_state;            /* +36 */
    uint16_t     visible;                /* +40 */
} Entity;

/* ------------------------------------------------------------------ */
/* Render callback                                                     */
/* ------------------------------------------------------------------ */

/*
 * EntityDrawFn — called by the bytecode interpreter when a draw opcode
 * is executed for entity @e drawing frame @sub_frame from CEL slot
 * @cel_slot, at screen position (x, y) with @flags.
 *
 * The game code provides a concrete implementation that writes into the
 * current framebuffer via moon_render.
 */
typedef void (*EntityDrawFn)(Entity *e,
                             int cel_slot, int sub_frame,
                             int x, int y, int flags);

/*
 * EntityCallFn — called when the CALL_EXT ($B4) opcode fires.
 * @param : the parameter byte from the bytecode.
 * @addr  : the 4-byte function address from the bytecode.
 * Return non-zero to signal the calling entity should be killed.
 */
typedef int (*EntityCallFn)(Entity *e, uint8_t param, uint32_t addr);

/* ------------------------------------------------------------------ */
/* Pool management                                                     */
/* ------------------------------------------------------------------ */

/** entity_pool_clear — reset all entity slots to inactive. */
void entity_pool_clear(void);

/**
 * entity_spawn — find a free slot and initialise a new entity.
 *
 * @script      : pointer to the bytecode script.
 * @asset_table : array of CEL_SLOTS pointers (MoonCel * or MoonOb *).
 * @base_x      : initial world X position.
 * @base_y      : initial world Y position.
 * @vel_y       : initial Y velocity.
 * @direction   : initial direction flags (bit0=flip_x, bit1=flip_y).
 * @script_type : dispatch type index.
 *
 * Returns a pointer to the new entity, or NULL if the pool is full.
 */
Entity *entity_spawn(uint8_t *script,
                     void   **asset_table,
                     int16_t  base_x, int16_t base_y,
                     int16_t  vel_y,
                     uint8_t  direction,
                     uint8_t  script_type);

/**
 * entity_spawn_mirrored — like entity_spawn but sets flip_x on direction.
 */
Entity *entity_spawn_mirrored(uint8_t *script,
                              void   **asset_table,
                              int16_t  base_x, int16_t base_y,
                              int16_t  vel_y,
                              uint8_t  direction,
                              uint8_t  script_type);

/**
 * entity_find_by_id — look up an entity in the pool by its entity_id.
 * Returns NULL if not found.
 */
Entity *entity_find_by_id(uint32_t id);

/**
 * entity_kill — deactivate an entity (active = 0).
 */
void entity_kill(Entity *e);

/* ------------------------------------------------------------------ */
/* Tick / interpreter                                                  */
/* ------------------------------------------------------------------ */

/**
 * entity_set_draw_callback — register the draw function used by the
 * bytecode interpreter when executing draw opcodes.
 */
void entity_set_draw_callback(EntityDrawFn fn);

/**
 * entity_set_call_callback — register the external-call handler for
 * CALL_EXT ($B4) opcodes.
 */
void entity_set_call_callback(EntityCallFn fn);

/**
 * entities_tick — advance all active entities by one interpreter step.
 *
 * Call once per game frame.  Entities whose scripts finish (END_ANIM)
 * are deactivated.  Returns the number of still-active entities.
 */
int entities_tick(void);

/**
 * entities_all_done — returns non-zero when all active entities have
 * finished their current animation (running == 0 for all active ones).
 */
int entities_all_done(void);

/**
 * entity_get_pool — direct access to the pool array (for rendering).
 * @count : receives ENTITY_POOL_SIZE.
 */
Entity *entity_get_pool(int *count);

#ifdef __cplusplus
}
#endif

#endif /* MOON_ENTITY_H */
