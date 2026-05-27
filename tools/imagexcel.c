/*
 * imagexcel.c — IMAGEXCEL sprite animation engine for Moonstone.
 *
 * Implements the interpreter loop from LAB_01F1/LAB_01F3/LAB_01F7 in
 * program.asm.  See docs/DOC_MOTEUR_IMAGEXCEL.md and imagexcel.h for the
 * full specification.
 *
 * Control-opcode sizes are derived from the assembly dispatch table at
 * LAB_0288 (program.asm:3980–4004) by reading the ADDI.L advances in each
 * handler:
 *
 *   opcode  handler    advance   notes
 *   0x80    LAB_0215   +2        direction set/toggle
 *   0x84    LAB_0218   +6        jump/call variant
 *   0x88    LAB_021A   +2        SET_SPEED
 *   0x8C    LAB_021E   +8        skip
 *   0x90    (none)     +2        safe default
 *   0x94    LAB_021F   +2        SET_LOOP_COUNT
 *   0x98    LAB_0220   +2        NOP (no advance in asm; +2 to be safe)
 *   0x9C    LAB_0220   +2        NOP (same)
 *   0xA0    LAB_0222   +8        MOVE_DELTA
 *   0xA4    LAB_0221   +4        skip
 *   0xA8    LAB_0241   +2        safe default
 *   0xAC    LAB_022D   +2        NOP (same)
 *   0xB0    LAB_022C   +2        NOP (same)
 *   0xB4    LAB_022F   +6        CALL (advance at LAB_0231)
 *   0xB8    LAB_0233   +6        skip
 *   0xBC    LAB_0234   +6        skip
 *   0xC0    LAB_0235   +2        clear entity type
 *   0xC4    LAB_0236   +2        SET_ASSET_TABLE
 *   0xC8    LAB_0232   +6        skip
 *   0xCC    LAB_0237   +8        (uses 8(A6) data)
 *   0xD0    LAB_023B   +8        conditional jump
 *   0xD4    LAB_023F   +8        conditional jump
 *   other              +2        safe default
 */

#include "imagexcel.h"

#include <string.h>
#include <stdint.h>

/* ------------------------------------------------------------------ */
/* Pixel extraction (planar CEL data, same logic as moon_view_cel.c)   */
/* ------------------------------------------------------------------ */

/*
 * ix_get_pixel — extract a colour index from a CEL frame's planar data.
 *
 * CEL pixel data is plane-sequential (plane 0 all rows, plane 1 all rows …).
 * Within each plane rows are packed MSB-first.  Returns 0 (transparent) for
 * out-of-bounds coordinates.
 */
static int ix_get_pixel(const MoonCelFrame *fr, int x, int y)
{
    if (x < 0 || x >= (int)fr->width || y < 0 || y >= (int)fr->height)
        return 0;
    int row_bytes = (((int)fr->width + 15) / 16) * 2;
    int idx = 0;
    for (int pl = 0; pl < (int)fr->planes; pl++) {
        size_t off = (size_t)pl  * (size_t)fr->height * (size_t)row_bytes
                   + (size_t)y   * (size_t)row_bytes
                   + (size_t)(x / 8);
        int bit = (fr->data[off] >> (7 - (x % 8))) & 1;
        idx |= (bit << pl);
    }
    return idx;
}

/* ------------------------------------------------------------------ */
/* Sprite blit                                                         */
/* ------------------------------------------------------------------ */

/*
 * ix_blit_frame — blit a single CEL frame to an ARGB8888 framebuffer.
 *
 * Pixels with palette index 0 are transparent (cookie-cut mask, matching the
 * Amiga BLIT_MASK behaviour from LAB_04B4 in program.asm).
 *
 * @fr       — CEL frame to draw.
 * @palette  — 32-entry ARGB8888 palette (index 0 = transparent).
 * @fb       — destination framebuffer (packed ARGB8888 rows).
 * @fb_w     — framebuffer width  in pixels.
 * @fb_h     — framebuffer height in pixels.
 * @dst_x    — destination X (may be negative; clipped).
 * @dst_y    — destination Y (may be negative; clipped).
 * @flip_h   — non-zero → mirror the frame horizontally.
 */
static void ix_blit_frame(const MoonCelFrame *fr,
                           const uint32_t *palette,
                           uint32_t *fb, int fb_w, int fb_h,
                           int dst_x, int dst_y, int flip_h)
{
    if (!fr || !fr->data) return;

    int fw = (int)fr->width;
    int fh = (int)fr->height;

    for (int fy = 0; fy < fh; fy++) {
        int py = dst_y + fy;
        if (py < 0 || py >= fb_h) continue;

        for (int fx = 0; fx < fw; fx++) {
            int px = dst_x + (flip_h ? (fw - 1 - fx) : fx);
            if (px < 0 || px >= fb_w) continue;

            int idx = ix_get_pixel(fr, fx, fy);
            if (idx == 0) continue; /* transparent */

            fb[py * fb_w + px] = palette[idx];
        }
    }
}

/* ------------------------------------------------------------------ */
/* Control-opcode size table                                           */
/* ------------------------------------------------------------------ */

/*
 * ctrl_op_size — return the total byte size (including the opcode byte)
 * of a control instruction (opcode >= 0x80).
 *
 * Derived from the ADDI.L advances in each handler in program.asm.
 */
static int ctrl_op_size(uint8_t op)
{
    /* strip bit 7 to get the table offset (same as assembly BCLR #7,D0) */
    switch (op) {
    case 0x80: return 2;  /* LAB_0215: direction set/toggle   */
    case 0x84: return 6;  /* LAB_0218: jump variant           */
    case 0x88: return 2;  /* LAB_021A: SET_SPEED              */
    case 0x8C: return 8;  /* LAB_021E: skip 8                 */
    case 0x94: return 2;  /* LAB_021F: SET_LOOP_COUNT         */
    case 0x98: return 2;  /* LAB_0220: NOP                    */
    case 0x9C: return 2;  /* LAB_0220: NOP                    */
    case 0xA0: return 8;  /* LAB_0222: MOVE_DELTA             */
    case 0xA4: return 4;  /* LAB_0221: skip 4                 */
    case 0xA8: return 2;  /* LAB_0241: NOP (just RTS)         */
    case 0xAC: return 2;  /* LAB_022D: NOP                    */
    case 0xB0: return 2;  /* LAB_022C: NOP                    */
    case 0xB4: return 6;  /* LAB_022F: CALL (advance at 0231) */
    case 0xB8: return 6;  /* LAB_0233: skip 6                 */
    case 0xBC: return 6;  /* LAB_0234: skip 6                 */
    case 0xC0: return 2;  /* LAB_0235: clear entity type      */
    case 0xC4: return 2;  /* LAB_0236: SET_ASSET_TABLE        */
    case 0xC8: return 6;  /* LAB_0232: skip 6                 */
    case 0xCC: return 8;  /* LAB_0237: 8 bytes                */
    case 0xD0: return 8;  /* LAB_023B: conditional jump       */
    case 0xD4: return 2;  /* LAB_023F: clear scratch + adv 2  */
    default:   return 2;  /* safe default                     */
    }
}

/* ------------------------------------------------------------------ */
/* API implementation                                                  */
/* ------------------------------------------------------------------ */

void ix_entity_init(IxEntity *e, const uint8_t *script)
{
    memset(e, 0, sizeof(*e));
    e->script      = script;
    e->frame_start = script;
    e->speed       = 1;
    e->timer       = 1;
    e->finished    = 0;
}

/*
 * ix_entity_draw — execute draw and control instructions for the current
 * animation step.
 *
 * Mirrors the inner loop at LAB_01F3 in program.asm: processes opcodes from
 * frame_start until a 0xFF byte is encountered (end-of-step).
 *
 * For each draw instruction (opcode < 0x80):
 *   byte 0: opcode      — slot = (opcode & 0x1F) / 4
 *   byte 1: frame_index
 *   byte 2: y_delta     — signed 8-bit; extended to 16-bit (EXT.W in asm)
 *   byte 3: flags       — draw flags (dual-buf / mask bits)
 *   bytes 4-5: x_pos    — signed 16-bit word
 *
 *   screen_x = base_x + x_pos
 *   screen_y = base_y + vel_y + (int8_t)y_delta
 *
 * Control opcodes implemented:
 *   0x80: SET_DIRECTION — toggle or set direction flag (LAB_0215)
 *   0x88: SET_SPEED     — set animation speed (LAB_021A)
 *   0x94: SET_LOOP_COUNT — init loop counter and record loop-back PC (LAB_021F)
 *   0xA0: MOVE_DELTA   — move/set base_x, base_y, vel_y (LAB_0222)
 *   0xC0: KILL         — mark entity finished (LAB_0235)
 *   All other control opcodes are skipped by their documented byte size.
 */
void ix_entity_draw(IxEntity *e, const IxCelSlots *slots,
                    const uint32_t *palette, uint32_t *fb,
                    int fb_w, int fb_h)
{
    if (e->finished || !e->frame_start) return;

    const uint8_t *pc = e->frame_start;

    for (;;) {
        uint8_t op = pc[0];

        /* End-of-step marker — stop drawing, do not advance frame_start here */
        if (op == 0xFF) break;

        if (op & 0x80) {
            /* Control instruction */
            switch (op) {

            case 0x80:
                /* SET_DIRECTION (LAB_0215):
                 * param == 0xFF → toggle direction bit 1
                 * otherwise     → set direction to param directly */
                if (pc[1] == 0xFF)
                    e->direction ^= 0x02;
                else
                    e->direction = pc[1];
                break;

            case 0x88:
                /* SET_SPEED (LAB_021A): byte 1 = new speed value.
                 * param == 0 → derive speed from global VBL counter (not
                 * available here; fall back to 1). */
                if (pc[1] != 0)
                    e->speed = pc[1];
                else if (e->speed == 0)
                    e->speed = 1;
                /* SET_SPEED also records the loop-back PC (right after the
                 * opcode) so that FF FE can loop to it.  We reuse loop_pc
                 * for this purpose, matching 2(A5) = PC+2 in the assembly. */
                e->loop_pc     = pc + 2;
                e->loop_active = 1;
                break;

            case 0x94:
                /* SET_LOOP_COUNT (LAB_021F):
                 * byte 1 = iteration count; loop-back address = PC+2. */
                e->loop_count  = pc[1];
                e->loop_active = 1;
                e->loop_pc     = pc + 2;
                break;

            case 0xA0: {
                /* MOVE_DELTA (LAB_0222):
                 * byte 1 = flags; bytes 2-3 = x value (word); 4-5 = y;
                 * 6-7 = vel_y.
                 *
                 * bit 6 of flags: 1 = absolute set, 0 = delta add/subtract.
                 * In delta mode:
                 *   bit 0: x direction (0=subtract, 1=add; inverted if
                 *          direction == 3)
                 *   bit 3: y direction (0=add, 1=subtract)
                 *   bit 5: vel_y direction (0=add, 1=subtract) */
                int16_t xv = (int16_t)((pc[2] << 8) | pc[3]);
                int16_t yv = (int16_t)((pc[4] << 8) | pc[5]);
                int16_t vv = (int16_t)((pc[6] << 8) | pc[7]);
                uint8_t fl = pc[1];

                if (fl & 0x40) {
                    /* absolute mode */
                    e->base_x = xv;
                    e->base_y = yv;
                    e->vel_y  = vv;
                } else {
                    /* delta mode — x */
                    if (e->direction == 3) {
                        /* direction==3: bit 0 inverted */
                        if (fl & 0x01) e->base_x = (int16_t)(e->base_x - xv);
                        else           e->base_x = (int16_t)(e->base_x + xv);
                    } else {
                        if (fl & 0x01) e->base_x = (int16_t)(e->base_x + xv);
                        else           e->base_x = (int16_t)(e->base_x - xv);
                    }
                    /* delta mode — y */
                    if (fl & 0x08) e->base_y = (int16_t)(e->base_y - yv);
                    else           e->base_y = (int16_t)(e->base_y + yv);
                    /* delta mode — vel_y */
                    if (fl & 0x20) e->vel_y = (int16_t)(e->vel_y - vv);
                    else           e->vel_y = (int16_t)(e->vel_y + vv);
                }
                break;
            }

            case 0xC0:
                /* KILL / clear entity type (LAB_0235): mark finished */
                e->finished = 1;
                return;

            default:
                /* All other control instructions: skip by documented size */
                break;
            }

            pc += ctrl_op_size(op);
        } else {
            /* Draw instruction — 6 bytes (LAB_01F7 / LAB_0200) */
            int  slot      = (op & 0x1F) / 4;
            int  frame_idx = pc[1];
            int  y_delta   = (int8_t)pc[2]; /* sign-extend byte → int (EXT.W) */
            /* pc[3] = flags byte (dual-buf / mask bits — not needed for blit) */
            int  x_pos     = (int16_t)((pc[4] << 8) | pc[5]); /* signed word */

            int screen_x = (int)e->base_x + x_pos;
            int screen_y = (int)e->base_y + (int)e->vel_y + y_delta;

            /* Fetch the CEL from the asset table */
            const MoonCel *cel = (slot < IX_SLOTS) ? slots->cel[slot] : NULL;
            if (cel && frame_idx < cel->frame_count) {
                const MoonCelFrame *fr = &cel->frames[frame_idx];
                ix_blit_frame(fr, palette, fb, fb_w, fb_h,
                              screen_x, screen_y, e->direction & 0x02);
            }

            pc += 6;
        }
    }
}

/*
 * ix_entity_advance — move frame_start past the current FF XX pair.
 *
 * Mirrors LAB_0201/LAB_0205/LAB_0207 in program.asm:
 *   FF 00  → advance PC by 2, continue at next step
 *   FF FE  → loop: if loop_active and loop_count > 0, decrement and jump
 *            back to loop_pc; otherwise clear loop_active and advance by 2
 *   FF FF  → end of script (same loop check as FF FE; if no pending loop,
 *            set finished = 1; return 1)
 *
 * The timer is reset to e->speed after advancing.
 */
int ix_entity_advance(IxEntity *e)
{
    if (e->finished || !e->frame_start) return 1;

    /* Scan forward to the FF end-of-step marker */
    const uint8_t *pc = e->frame_start;
    for (;;) {
        uint8_t op = pc[0];
        if (op == 0xFF) break;
        if (op & 0x80)
            pc += ctrl_op_size(op);
        else
            pc += 6; /* draw instruction */
    }

    /* pc now points at 0xFF; read the second byte */
    uint8_t term = pc[1];

    if (term == 0xFF) {
        /* FF FF — end of script (LAB_0208/LAB_0209)
         * Check loop counter first: if active, loop back instead */
        if (e->loop_active && e->loop_count > 0) {
            e->loop_count--;
            if (e->loop_count > 0 && e->loop_pc) {
                e->frame_start = e->loop_pc;
            } else {
                e->loop_active = 0;
                e->finished    = 1;
                e->timer       = 1;
                return 1;
            }
        } else {
            e->loop_active = 0;
            e->finished    = 1;
            e->timer       = 1;
            return 1;
        }
    } else if (term == 0xFE) {
        /* FF FE — loop (LAB_0205/LAB_0206)
         * If loop_active and loop_count > 0, loop back; otherwise advance */
        if (e->loop_active && e->loop_count > 0) {
            e->loop_count--;
            if (e->loop_count > 0 && e->loop_pc) {
                e->frame_start = e->loop_pc;
            } else {
                e->loop_active = 0;
                e->frame_start = pc + 2;
            }
        } else {
            e->loop_active = 0;
            /* Fall back: loop to script start (original behaviour) */
            e->frame_start = e->script;
        }
    } else {
        /* FF 00 (or any other second byte) — advance to next step (LAB_0207)
         * PC += 2 to skip the FF XX pair */
        e->frame_start = pc + 2;
    }

    e->timer = (int)e->speed;
    if (e->timer < 1) e->timer = 1;
    return 0;
}

int ix_entity_tick(IxEntity *e, const IxCelSlots *slots,
                   const uint32_t *palette, uint32_t *fb,
                   int fb_w, int fb_h)
{
    ix_entity_draw(e, slots, palette, fb, fb_w, fb_h);

    if (e->finished) return 1;

    e->timer--;
    if (e->timer <= 0)
        return ix_entity_advance(e);

    return 0;
}
