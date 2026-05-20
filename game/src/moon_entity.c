/*
 * moon_entity.c — Entity pool and animation bytecode interpreter
 *
 * Re-implements LAB_0282 entity pool and LAB_01F1 bytecode interpreter
 * from program.asm (§2.3 of DOC_ANIMATIONS_INTRO_FIN.md).
 */

#include "moon_entity.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static Entity     s_pool[ENTITY_POOL_SIZE];
static FrameState s_frame_states[ENTITY_POOL_SIZE];
static uint32_t   s_next_id = 1;

static EntityDrawFn s_draw_cb = NULL;
static EntityCallFn s_call_cb = NULL;

/* ------------------------------------------------------------------ */
/* Pool management                                                     */
/* ------------------------------------------------------------------ */

void entity_pool_clear(void)
{
    memset(s_pool,         0, sizeof(s_pool));
    memset(s_frame_states, 0, sizeof(s_frame_states));
    for (int i = 0; i < ENTITY_POOL_SIZE; i++)
        s_pool[i].frame_state = &s_frame_states[i];
    s_next_id = 1;
}

static Entity *find_free_slot(void)
{
    for (int i = 0; i < ENTITY_POOL_SIZE; i++) {
        if (!s_pool[i].active)
            return &s_pool[i];
    }
    return NULL;
}

Entity *entity_spawn(uint8_t *script,
                     void   **asset_table,
                     int16_t  base_x, int16_t base_y,
                     int16_t  vel_y,
                     uint8_t  direction,
                     uint8_t  script_type)
{
    Entity *e = find_free_slot();
    if (!e) {
        fprintf(stderr, "entity_spawn: pool exhausted\n");
        return NULL;
    }

    int idx = (int)(e - s_pool);
    memset(e, 0, sizeof(Entity));
    e->frame_state = &s_frame_states[idx];
    memset(e->frame_state, 0, sizeof(FrameState));

    e->active      = 1;
    e->running     = 1;
    e->script_pc   = script;
    e->base_x      = base_x;
    e->base_y      = base_y;
    e->vel_y       = vel_y;
    e->direction   = direction;
    e->script_type = script_type;
    e->visible     = 1;
    e->entity_id   = s_next_id++;

    if (asset_table) {
        for (int i = 0; i < CEL_SLOTS; i++)
            e->asset_table[i] = asset_table[i];
    }

    return e;
}

Entity *entity_spawn_mirrored(uint8_t *script,
                              void   **asset_table,
                              int16_t  base_x, int16_t base_y,
                              int16_t  vel_y,
                              uint8_t  direction,
                              uint8_t  script_type)
{
    return entity_spawn(script, asset_table, base_x, base_y,
                        vel_y, direction | 0x01 /* flip_x */, script_type);
}

Entity *entity_find_by_id(uint32_t id)
{
    for (int i = 0; i < ENTITY_POOL_SIZE; i++) {
        if (s_pool[i].active && s_pool[i].entity_id == id)
            return &s_pool[i];
    }
    return NULL;
}

void entity_kill(Entity *e)
{
    if (e) e->active = 0;
}

/* ------------------------------------------------------------------ */
/* Callbacks                                                           */
/* ------------------------------------------------------------------ */

void entity_set_draw_callback(EntityDrawFn fn) { s_draw_cb = fn; }
void entity_set_call_callback(EntityCallFn fn) { s_call_cb = fn; }

/* ------------------------------------------------------------------ */
/* Bytecode interpreter — single entity tick                          */
/* ------------------------------------------------------------------ */

/*
 * Advance entity @e's script by one step.
 *
 * The bytecode format (6-byte instructions) is described in
 * DOC_ANIMATIONS_INTRO_FIN.md §2.3.
 *
 * Returns 1 if the entity is still running, 0 if it has stopped.
 */
static int entity_step(Entity *e)
{
    if (!e->active || !e->running || !e->script_pc)
        return 0;

    FrameState *fs = e->frame_state;

    /* Speed throttle: wait `speed` ticks between frames */
    if (fs->active && fs->speed > 0) {
        fs->tick_count++;
        if (fs->tick_count < fs->speed)
            return 1;
        fs->tick_count = 0;
    }

    while (e->running && e->script_pc) {
        uint8_t *pc  = e->script_pc;
        uint8_t  op  = pc[0];
        uint8_t  par = pc[1];

        /* Draw opcode: bit7 == 0 → CEL slot index = op >> 2 */
        if (!(op & 0x80)) {
            int cel_slot  = (op >> 2) & 0x07;
            int sub_frame = par;
            int8_t  xd    = (int8_t)pc[2];
            uint8_t fl    = pc[3];
            int16_t yd    = (int16_t)(((uint16_t)pc[4] << 8) | pc[5]);

            int blit_flags = 0;
            if (e->direction & 0x01) blit_flags |= 0x01; /* flip_x */
            if (e->direction & 0x02) blit_flags |= 0x02; /* flip_y */
            if (fl & 0x04)           blit_flags |= 0x04; /* mask   */

            int sx = e->base_x + xd;
            int sy = e->base_y + yd;
            e->screen_x = (int16_t)sx;
            e->screen_y = (int16_t)sy;

            if (e->visible && s_draw_cb)
                s_draw_cb(e, cel_slot, sub_frame, sx, sy, blit_flags);

            e->script_pc += 6;
            return 1; /* consumed one draw frame → stop stepping */
        }

        /* Control opcode */
        switch (op) {
        case 0xFF: {
            /* END_FRAME (par=0x00) or END_ANIM (par=0xFF) */
            if (par == 0xFF) {
                e->running = 0;
                return 0;
            }
            /* END_FRAME / LOOP_BACK (par=0xFE) */
            if (par == 0xFE) {
                if (fs->loop_count > 0) {
                    fs->loop_count--;
                    if (fs->loop_count > 0 && fs->loop_addr) {
                        e->script_pc = fs->loop_addr;
                    } else {
                        e->script_pc += 2;
                    }
                } else {
                    e->script_pc += 2;
                }
                return 1;
            }
            /* END_FRAME — wait speed ticks */
            e->script_pc += 2;
            fs->tick_count = 0;
            return 1;
        }

        case 0xFE: /* LOOP_IDLE — jump to loop_addr */
            if (fs->loop_addr)
                e->script_pc = fs->loop_addr;
            else
                e->script_pc += 2;
            return 1;

        case 0xFD: /* DEATH — jump to death_addr */
            if (fs->death_addr)
                e->script_pc = fs->death_addr;
            else
                e->running = 0;
            return 1;

        case 0x80: /* SET_DIR */
            if (par == 0xFF)
                e->direction ^= 0x02;
            else
                e->direction = par;
            e->script_pc += 2;
            break;

        case 0x84: /* SET_NEXT_ANIM / COND_JUMP */
            if (par == 3) {
                uint32_t addr = ((uint32_t)pc[2] << 24) | ((uint32_t)pc[3] << 16)
                              | ((uint32_t)pc[4] << 8)  |  (uint32_t)pc[5];
                e->script_pc = (uint8_t *)(uintptr_t)addr;
            } else {
                uint32_t addr = ((uint32_t)pc[2] << 24) | ((uint32_t)pc[3] << 16)
                              | ((uint32_t)pc[4] << 8)  |  (uint32_t)pc[5];
                fs->next_anim    = (uint8_t *)(uintptr_t)addr;
                fs->anim_changed = 1;
                e->script_pc    += 6;
            }
            break;

        case 0x88: /* SET_SPEED */
            fs->speed      = par;
            fs->loop_addr  = pc + 2;
            fs->active     = 1;
            fs->tick_count = 0;
            e->script_pc  += 2;
            break;

        case 0x8C: /* SKIP_8 */
            e->script_pc += 8;
            break;

        case 0x94: /* LOOP_INIT */
            fs->loop_count = par;
            fs->looping    = 1;
            fs->loop_addr  = pc + 2;
            e->script_pc  += 2;
            break;

        case 0x98: /* NOP_98 */
        case 0x9C: /* NOP_9C */
        case 0xAC: /* NOP_AC */
        case 0xB0: /* NOP_B0 */
            e->script_pc += 2;
            break;

        case 0xA0: /* MOVE_DELTA */
            {
                int8_t dx = (int8_t)pc[2];
                int8_t dy_b = (int8_t)pc[3];
                /* flags in pc[1] determine which deltas to apply */
                e->base_x = (int16_t)(e->base_x + dx);
                e->base_y = (int16_t)(e->base_y + dy_b);
                e->script_pc += 8;
            }
            break;

        case 0xA4: /* ADVANCE_4 */
            e->script_pc += 4;
            break;

        case 0xA8: /* UNK_A8 */
            e->script_pc += 6;
            break;

        case 0xB4: /* CALL_EXT */
            {
                uint32_t addr = ((uint32_t)pc[2] << 24) | ((uint32_t)pc[3] << 16)
                              | ((uint32_t)pc[4] << 8)  |  (uint32_t)pc[5];
                e->script_pc += 6;
                if (s_call_cb) {
                    int kill = s_call_cb(e, par, addr);
                    if (kill) { e->active = 0; return 0; }
                }
            }
            break;

        case 0xB8: /* SKIP_6A */
        case 0xBC: /* SKIP_6B */
        case 0xC8: /* SKIP_6C */
            e->script_pc += 6;
            break;

        case 0xC0: /* KILL */
            e->active = 0;
            return 0;

        case 0xC4: /* SET_ASSET_TABLE — not fully impl.; advance */
            e->script_pc += 2;
            break;

        case 0xCC: /* BRANCH_IF_ZERO */
            {
                uint32_t addr = ((uint32_t)pc[4] << 24) | ((uint32_t)pc[5] << 16)
                              | ((uint32_t)pc[6] << 8)  |  (uint32_t)pc[7];
                /* simplified: always take the skip path (PC += 8) */
                (void)addr;
                e->script_pc += 8;
            }
            break;

        case 0xD0: /* BRANCH_IF_NONZERO */
            {
                uint32_t addr = ((uint32_t)pc[4] << 24) | ((uint32_t)pc[5] << 16)
                              | ((uint32_t)pc[6] << 8)  |  (uint32_t)pc[7];
                (void)addr;
                e->script_pc += 8;
            }
            break;

        case 0xD4: /* UNK_D4 */
            e->script_pc += 6;
            break;

        default:
            /* Unknown opcode — skip 2 bytes and continue */
            e->script_pc += 2;
            break;
        }

        /* Check if animation changed mid-step */
        if (fs->anim_changed && fs->next_anim) {
            e->script_pc     = fs->next_anim;
            fs->next_anim    = NULL;
            fs->anim_changed = 0;
        }
    }

    return e->running;
}

/* ------------------------------------------------------------------ */
/* Pool tick                                                           */
/* ------------------------------------------------------------------ */

int entities_tick(void)
{
    int active = 0;
    for (int i = 0; i < ENTITY_POOL_SIZE; i++) {
        if (!s_pool[i].active) continue;
        entity_step(&s_pool[i]);
        if (s_pool[i].active) active++;
    }
    return active;
}

int entities_all_done(void)
{
    for (int i = 0; i < ENTITY_POOL_SIZE; i++) {
        if (s_pool[i].active && s_pool[i].running)
            return 0;
    }
    return 1;
}

Entity *entity_get_pool(int *count)
{
    if (count) *count = ENTITY_POOL_SIZE;
    return s_pool;
}
