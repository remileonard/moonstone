/*
 * moon-anim-viewer — generic viewer for all IMAGEXCEL animation scripts.
 *
 * Loads all 162 auto-generated scripts from moon_anim_registry.h and lets
 * you browse them with the left/right arrow keys (or P/N).  For scripts
 * whose CEL slot mappings are known (e.g. dw1_walk, kn_walk) the sprite is
 * rendered; for unknown slots an orange placeholder rectangle is shown.
 *
 * Controls:
 *   →  / N      — next animation
 *   ←  / P      — previous animation
 *   SPACE        — force advance to the next step immediately
 *   R            — restart current animation from the beginning
 *   F            — toggle frame-by-frame (pause/resume)
 *   B            — toggle bounding-box display
 *   ESC / Q      — quit
 *
 * Usage:
 *   moon-anim-viewer <asset_directory> [<script_name>]
 *
 *   asset_directory  — folder containing *.cel / *.ob files (and bg2.PIV).
 *   script_name      — optional: start on this animation (e.g. "dw1_walk").
 *                      If omitted, starts on the first entry.
 */

#include "imagexcel.h"
#include "moon_assets.h"
#include "moon_anim_registry.h"  /* includes all 162 scripts + AnimEntry table */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* ------------------------------------------------------------------ */
/* Fallback palette (32 Amiga OCS colours as 0x00RRGGBB)              */
/* ------------------------------------------------------------------ */

static const uint32_t fallback_pal32[32] = {
    0x000000, 0x111188, 0x2222AA, 0x3333CC,
    0x004400, 0x005500, 0x006600, 0x007700,
    0x220000, 0x440000, 0x660000, 0x880000,
    0x222222, 0x444444, 0x666666, 0x888888,
    0xAAAAAA, 0xBBBBBB, 0xCCCCCC, 0xDDDDDD,
    0xEEEEEE, 0xFFFFFF, 0x884400, 0xAA6622,
    0xCC8844, 0xEEAA66, 0x004488, 0x2266AA,
    0x4488CC, 0x66AAEE, 0xFF8800, 0xFFCC00,
};

#ifdef HAVE_SDL2

/* ------------------------------------------------------------------ */
/* Embedded 8×8 bitmap font (ASCII 32–127, public domain VGA font)    */
/*                                                                     */
/* Each entry is 8 bytes — one per row.  Bit 0 of each byte is the    */
/* leftmost pixel (LSB-first).                                         */
/* ------------------------------------------------------------------ */

static const uint8_t g_font8x8[96][8] = {
    /* 32 ' '  */ { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00 },
    /* 33 '!'  */ { 0x18,0x3C,0x3C,0x18,0x18,0x00,0x18,0x00 },
    /* 34 '"'  */ { 0x36,0x36,0x00,0x00,0x00,0x00,0x00,0x00 },
    /* 35 '#'  */ { 0x36,0x36,0x7F,0x36,0x7F,0x36,0x36,0x00 },
    /* 36 '$'  */ { 0x0C,0x3E,0x03,0x1E,0x30,0x1F,0x0C,0x00 },
    /* 37 '%'  */ { 0x00,0x63,0x33,0x18,0x0C,0x66,0x63,0x00 },
    /* 38 '&'  */ { 0x1C,0x36,0x1C,0x6E,0x3B,0x33,0x6E,0x00 },
    /* 39 '\'' */ { 0x06,0x06,0x03,0x00,0x00,0x00,0x00,0x00 },
    /* 40 '('  */ { 0x18,0x0C,0x06,0x06,0x06,0x0C,0x18,0x00 },
    /* 41 ')'  */ { 0x06,0x0C,0x18,0x18,0x18,0x0C,0x06,0x00 },
    /* 42 '*'  */ { 0x00,0x66,0x3C,0xFF,0x3C,0x66,0x00,0x00 },
    /* 43 '+'  */ { 0x00,0x0C,0x0C,0x3F,0x0C,0x0C,0x00,0x00 },
    /* 44 ','  */ { 0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x06 },
    /* 45 '-'  */ { 0x00,0x00,0x00,0x3F,0x00,0x00,0x00,0x00 },
    /* 46 '.'  */ { 0x00,0x00,0x00,0x00,0x00,0x0C,0x0C,0x00 },
    /* 47 '/'  */ { 0x60,0x30,0x18,0x0C,0x06,0x03,0x01,0x00 },
    /* 48 '0'  */ { 0x3E,0x63,0x73,0x7B,0x6F,0x67,0x3E,0x00 },
    /* 49 '1'  */ { 0x0C,0x0E,0x0C,0x0C,0x0C,0x0C,0x3F,0x00 },
    /* 50 '2'  */ { 0x1E,0x33,0x30,0x1C,0x06,0x33,0x3F,0x00 },
    /* 51 '3'  */ { 0x1E,0x33,0x30,0x1C,0x30,0x33,0x1E,0x00 },
    /* 52 '4'  */ { 0x38,0x3C,0x36,0x33,0x7F,0x30,0x78,0x00 },
    /* 53 '5'  */ { 0x3F,0x03,0x1F,0x30,0x30,0x33,0x1E,0x00 },
    /* 54 '6'  */ { 0x1C,0x06,0x03,0x1F,0x33,0x33,0x1E,0x00 },
    /* 55 '7'  */ { 0x3F,0x33,0x30,0x18,0x0C,0x0C,0x0C,0x00 },
    /* 56 '8'  */ { 0x1E,0x33,0x33,0x1E,0x33,0x33,0x1E,0x00 },
    /* 57 '9'  */ { 0x1E,0x33,0x33,0x3E,0x30,0x18,0x0E,0x00 },
    /* 58 ':'  */ { 0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x00 },
    /* 59 ';'  */ { 0x00,0x0C,0x0C,0x00,0x00,0x0C,0x0C,0x06 },
    /* 60 '<'  */ { 0x18,0x0C,0x06,0x03,0x06,0x0C,0x18,0x00 },
    /* 61 '='  */ { 0x00,0x00,0x3F,0x00,0x00,0x3F,0x00,0x00 },
    /* 62 '>'  */ { 0x06,0x0C,0x18,0x30,0x18,0x0C,0x06,0x00 },
    /* 63 '?'  */ { 0x1E,0x33,0x30,0x18,0x0C,0x00,0x0C,0x00 },
    /* 64 '@'  */ { 0x3E,0x63,0x7B,0x7B,0x7B,0x03,0x1E,0x00 },
    /* 65 'A'  */ { 0x0C,0x1E,0x33,0x33,0x3F,0x33,0x33,0x00 },
    /* 66 'B'  */ { 0x3F,0x66,0x66,0x3E,0x66,0x66,0x3F,0x00 },
    /* 67 'C'  */ { 0x3C,0x66,0x03,0x03,0x03,0x66,0x3C,0x00 },
    /* 68 'D'  */ { 0x1F,0x36,0x66,0x66,0x66,0x36,0x1F,0x00 },
    /* 69 'E'  */ { 0x7F,0x46,0x16,0x1E,0x16,0x46,0x7F,0x00 },
    /* 70 'F'  */ { 0x7F,0x46,0x16,0x1E,0x16,0x06,0x0F,0x00 },
    /* 71 'G'  */ { 0x3C,0x66,0x03,0x03,0x73,0x66,0x7C,0x00 },
    /* 72 'H'  */ { 0x33,0x33,0x33,0x3F,0x33,0x33,0x33,0x00 },
    /* 73 'I'  */ { 0x1E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00 },
    /* 74 'J'  */ { 0x78,0x30,0x30,0x30,0x33,0x33,0x1E,0x00 },
    /* 75 'K'  */ { 0x67,0x66,0x36,0x1E,0x36,0x66,0x67,0x00 },
    /* 76 'L'  */ { 0x0F,0x06,0x06,0x06,0x46,0x66,0x7F,0x00 },
    /* 77 'M'  */ { 0x63,0x77,0x7F,0x7F,0x6B,0x63,0x63,0x00 },
    /* 78 'N'  */ { 0x63,0x67,0x6F,0x7B,0x73,0x63,0x63,0x00 },
    /* 79 'O'  */ { 0x1C,0x36,0x63,0x63,0x63,0x36,0x1C,0x00 },
    /* 80 'P'  */ { 0x3F,0x66,0x66,0x3E,0x06,0x06,0x0F,0x00 },
    /* 81 'Q'  */ { 0x1E,0x33,0x33,0x33,0x3B,0x1E,0x38,0x00 },
    /* 82 'R'  */ { 0x3F,0x66,0x66,0x3E,0x36,0x66,0x67,0x00 },
    /* 83 'S'  */ { 0x1E,0x33,0x07,0x0E,0x38,0x33,0x1E,0x00 },
    /* 84 'T'  */ { 0x3F,0x2D,0x0C,0x0C,0x0C,0x0C,0x1E,0x00 },
    /* 85 'U'  */ { 0x33,0x33,0x33,0x33,0x33,0x33,0x3F,0x00 },
    /* 86 'V'  */ { 0x33,0x33,0x33,0x33,0x33,0x1E,0x0C,0x00 },
    /* 87 'W'  */ { 0x63,0x63,0x63,0x6B,0x7F,0x77,0x63,0x00 },
    /* 88 'X'  */ { 0x63,0x63,0x36,0x1C,0x1C,0x36,0x63,0x00 },
    /* 89 'Y'  */ { 0x33,0x33,0x33,0x1E,0x0C,0x0C,0x1E,0x00 },
    /* 90 'Z'  */ { 0x7F,0x63,0x31,0x18,0x4C,0x66,0x7F,0x00 },
    /* 91 '['  */ { 0x1E,0x06,0x06,0x06,0x06,0x06,0x1E,0x00 },
    /* 92 '\\' */ { 0x03,0x06,0x0C,0x18,0x30,0x60,0x40,0x00 },
    /* 93 ']'  */ { 0x1E,0x18,0x18,0x18,0x18,0x18,0x1E,0x00 },
    /* 94 '^'  */ { 0x08,0x1C,0x36,0x63,0x00,0x00,0x00,0x00 },
    /* 95 '_'  */ { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF },
    /* 96 '`'  */ { 0x0C,0x0C,0x18,0x00,0x00,0x00,0x00,0x00 },
    /* 97 'a'  */ { 0x00,0x00,0x1E,0x30,0x3E,0x33,0x6E,0x00 },
    /* 98 'b'  */ { 0x07,0x06,0x06,0x3E,0x66,0x66,0x3B,0x00 },
    /* 99 'c'  */ { 0x00,0x00,0x1E,0x33,0x03,0x33,0x1E,0x00 },
    /* 100 'd' */ { 0x38,0x30,0x30,0x3E,0x33,0x33,0x6E,0x00 },
    /* 101 'e' */ { 0x00,0x00,0x1E,0x33,0x3F,0x03,0x1E,0x00 },
    /* 102 'f' */ { 0x1C,0x36,0x06,0x0F,0x06,0x06,0x0F,0x00 },
    /* 103 'g' */ { 0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x1F },
    /* 104 'h' */ { 0x07,0x06,0x36,0x6E,0x66,0x66,0x67,0x00 },
    /* 105 'i' */ { 0x0C,0x00,0x0E,0x0C,0x0C,0x0C,0x1E,0x00 },
    /* 106 'j' */ { 0x30,0x00,0x30,0x30,0x30,0x33,0x33,0x1E },
    /* 107 'k' */ { 0x07,0x06,0x66,0x36,0x1E,0x36,0x67,0x00 },
    /* 108 'l' */ { 0x0E,0x0C,0x0C,0x0C,0x0C,0x0C,0x1E,0x00 },
    /* 109 'm' */ { 0x00,0x00,0x33,0x7F,0x7F,0x6B,0x63,0x00 },
    /* 110 'n' */ { 0x00,0x00,0x1F,0x33,0x33,0x33,0x33,0x00 },
    /* 111 'o' */ { 0x00,0x00,0x1E,0x33,0x33,0x33,0x1E,0x00 },
    /* 112 'p' */ { 0x00,0x00,0x3B,0x66,0x66,0x3E,0x06,0x0F },
    /* 113 'q' */ { 0x00,0x00,0x6E,0x33,0x33,0x3E,0x30,0x78 },
    /* 114 'r' */ { 0x00,0x00,0x3B,0x6E,0x66,0x06,0x0F,0x00 },
    /* 115 's' */ { 0x00,0x00,0x3E,0x03,0x1E,0x30,0x1F,0x00 },
    /* 116 't' */ { 0x08,0x0C,0x3E,0x0C,0x0C,0x2C,0x18,0x00 },
    /* 117 'u' */ { 0x00,0x00,0x33,0x33,0x33,0x33,0x6E,0x00 },
    /* 118 'v' */ { 0x00,0x00,0x33,0x33,0x33,0x1E,0x0C,0x00 },
    /* 119 'w' */ { 0x00,0x00,0x63,0x6B,0x7F,0x7F,0x36,0x00 },
    /* 120 'x' */ { 0x00,0x00,0x63,0x36,0x1C,0x36,0x63,0x00 },
    /* 121 'y' */ { 0x00,0x00,0x33,0x33,0x33,0x3E,0x30,0x1F },
    /* 122 'z' */ { 0x00,0x00,0x3F,0x19,0x0C,0x26,0x3F,0x00 },
    /* 123 '{' */ { 0x38,0x0C,0x0C,0x07,0x0C,0x0C,0x38,0x00 },
    /* 124 '|' */ { 0x18,0x18,0x18,0x00,0x18,0x18,0x18,0x00 },
    /* 125 '}' */ { 0x07,0x0C,0x0C,0x38,0x0C,0x0C,0x07,0x00 },
    /* 126 '~' */ { 0x6E,0x3B,0x00,0x00,0x00,0x00,0x00,0x00 },
    /* 127 DEL */ { 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF },
};

/* ------------------------------------------------------------------ */
/* Text rendering helpers                                              */
/* ------------------------------------------------------------------ */

static void sdl_draw_char(SDL_Renderer *ren, int x, int y, char ch,
                           uint8_t r, uint8_t g, uint8_t b)
{
    unsigned int idx = (unsigned int)(unsigned char)ch;
    if (idx < 32 || idx > 127) idx = '?';
    const uint8_t *glyph = g_font8x8[idx - 32];

    SDL_SetRenderDrawColor(ren, r, g, b, 255);
    for (int row = 0; row < 8; row++) {
        uint8_t bits = glyph[row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x01u << col))
                SDL_RenderDrawPoint(ren, x + col, y + row);
        }
    }
}

static void sdl_draw_text(SDL_Renderer *ren, int x, int y, const char *text,
                           uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; text[i]; i++)
        sdl_draw_char(ren, x + i * 8, y, text[i], r, g, b);
}

/* ------------------------------------------------------------------ */
/* Instruction decode helper                                           */
/* ------------------------------------------------------------------ */

static void describe_step(const IxEntity *e, char *buf, int bufsize)
{
    if (!e->frame_start || e->finished) {
        snprintf(buf, (size_t)bufsize, "(done)");
        return;
    }

    const uint8_t *pc = e->frame_start;
    int pos = 0;
    int first = 1;

    while (pos < bufsize - 1) {
        uint8_t op = pc[0];
        if (op == IX_STEP_END) break;

        if (!first && pos < bufsize - 3) {
            buf[pos++] = ' ';
            buf[pos++] = '|';
            buf[pos++] = ' ';
        }
        first = 0;

        if (op & 0x80) {
            const char *name = "CTL";
            switch (op) {
            case IX_OP_SET_DIRECTION:   name = "SETDIR"; break;
            case IX_OP_SET_SPEED:       name = "SPEED";  break;
            case IX_OP_SET_LOOP_COUNT:  name = "LOOP";   break;
            case IX_OP_MOVE_DELTA:      name = "MOVE";   break;
            case IX_OP_KILL:            name = "KILL";   break;
            default:                                     break;
            }
            pos += snprintf(buf + pos, (size_t)(bufsize - pos),
                            "%s(%02X)", name, op);
            pc += ix_ctrl_op_size(op);
        } else {
            int slot      = (op & 0x1F) / 4;
            int frame_idx = pc[1];
            int y_delta   = (int8_t)pc[2];
            int flags     = pc[3];
            int x_pos     = (int16_t)((pc[4] << 8) | pc[5]);
            pos += snprintf(buf + pos, (size_t)(bufsize - pos),
                            "DRAW(s=%d f=%d x=%+d y=%+d fl=%02X)",
                            slot, frame_idx, x_pos, y_delta, flags);
            pc += 6;
        }
    }
    if (first)
        snprintf(buf, (size_t)bufsize, "(empty)");
    else
        buf[pos] = '\0';
}

/* ------------------------------------------------------------------ */
/* Placeholder boxes for slots with no CEL loaded                     */
/* ------------------------------------------------------------------ */

/*
 * draw_unmapped_slots — draw an orange outline rectangle for each draw
 * instruction in the current step whose slot has no CEL loaded.
 * Uses approximate position: screen_x = base_x + x_pos,
 * screen_y = base_y + vel_y + y_delta.  Box size is 24×24 (scaled).
 */
static void draw_unmapped_slots(SDL_Renderer *ren, const IxEntity *e,
                                 const IxCelSlots *slots, int scale)
{
    if (!e->frame_start || e->finished) return;

    const uint8_t *pc = e->frame_start;
    SDL_SetRenderDrawColor(ren, 255, 140, 0, 255);  /* orange */

    while (1) {
        uint8_t op = pc[0];
        if (op == IX_STEP_END) break;

        if (op & 0x80) {
            pc += ix_ctrl_op_size(op);
        } else {
            int slot    = (op & 0x1F) / 4;
            int y_delta = (int8_t)pc[2];
            int x_pos   = (int16_t)((pc[4] << 8) | pc[5]);

            if (slot < IX_SLOTS && !slots->cel[slot]) {
                int sx = ((int)e->base_x + x_pos) * scale;
                int sy = ((int)e->base_y + (int)e->vel_y + y_delta) * scale;
                SDL_Rect r = { sx, sy, 24 * scale, 24 * scale };
                SDL_RenderDrawRect(ren, &r);
            }
            pc += 6;
        }
    }
}

#endif /* HAVE_SDL2 */

/* ------------------------------------------------------------------ */
/* CEL slot management                                                 */
/* ------------------------------------------------------------------ */

static MoonCel  *g_loaded_cels[IX_SLOTS];
static IxCelSlots g_slots;
static IxEntity   g_entity;
static int        g_step_index = 0;
static int        g_anim_index = 0;

static void free_cels(void)
{
    for (int i = 0; i < IX_SLOTS; i++) {
        if (g_loaded_cels[i]) {
            moon_cel_free(g_loaded_cels[i]);
            g_loaded_cels[i] = NULL;
        }
    }
    memset(&g_slots, 0, sizeof(g_slots));
}

static void load_animation(int idx)
{
    free_cels();

    const AnimEntry *e = &g_anim_registry[idx];

    for (int i = 0; i < IX_SLOTS; i++) {
        if (e->cel_files[i]) {
            MoonCel *cel = moon_cel_load(e->cel_files[i]);
            if (cel) {
                g_loaded_cels[i] = cel;
                g_slots.cel[i]   = cel;
            } else {
                fprintf(stderr,
                        "moon-anim-viewer: warning: failed to load '%s' for slot %d\n",
                        e->cel_files[i], i);
            }
        }
    }

    ix_entity_init(&g_entity, e->script);
    g_entity.base_x    = e->base_x;
    g_entity.base_y    = e->base_y;
    g_entity.vel_y     = e->vel_y;
    g_entity.direction = e->direction;
    g_step_index       = 0;
}

static void restart_animation(void)
{
    const AnimEntry *e = &g_anim_registry[g_anim_index];
    ix_entity_init(&g_entity, e->script);
    g_entity.base_x    = e->base_x;
    g_entity.base_y    = e->base_y;
    g_entity.vel_y     = e->vel_y;
    g_entity.direction = e->direction;
    g_step_index       = 0;
}

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    const char *asset_dir   = (argc > 1) ? argv[1] : ".";
    const char *start_name  = (argc > 2) ? argv[2] : NULL;

    moon_init(asset_dir);

    /* Find the starting index */
    g_anim_index = 0;
    if (start_name) {
        for (int i = 0; i < ANIM_REGISTRY_COUNT; i++) {
            if (strcmp(g_anim_registry[i].name, start_name) == 0) {
                g_anim_index = i;
                break;
            }
        }
    }

    memset(g_loaded_cels, 0, sizeof(g_loaded_cels));
    memset(&g_slots, 0, sizeof(g_slots));
    load_animation(g_anim_index);

    /* Try to load the background palette from bg2.PIV */
    uint32_t palette[32];
    memcpy(palette, fallback_pal32, sizeof(palette));

    MoonPiv *bg2 = moon_piv_load("bg2.PIV");
    if (!bg2) bg2 = moon_piv_load("bg2.piv");
    if (bg2) {
        for (int i = 0; i < 32; i++) {
            uint16_t c = bg2->palette[i];
            uint8_t r  = ((c >> 8) & 0x0F) * 17;
            uint8_t g  = ((c >> 4) & 0x0F) * 17;
            uint8_t b  = ((c >> 0) & 0x0F) * 17;
            palette[i] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
        printf("bg2.PIV palette loaded (32 colours)\n");
        moon_piv_free(bg2);
    }

    printf("moon-anim-viewer: %d animations loaded\n", ANIM_REGISTRY_COUNT);
    printf("Starting on: %s (%d/%d)\n",
           g_anim_registry[g_anim_index].name,
           g_anim_index + 1, ANIM_REGISTRY_COUNT);

    const int FB_W = 320;
    const int FB_H = 200;
    uint32_t *fb   = (uint32_t *)calloc((size_t)(FB_W * FB_H), 4);
    if (!fb) {
        fprintf(stderr, "moon-anim-viewer: out of memory\n");
        free_cels();
        moon_shutdown();
        return 1;
    }

#ifdef HAVE_SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        free(fb);
        free_cels();
        moon_shutdown();
        return 1;
    }

    const int SCALE  = 2;
    const int INFO_H = 54;

    SDL_Window *win = SDL_CreateWindow("moon-anim-viewer",
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          FB_W * SCALE, FB_H * SCALE + INFO_H,
                          SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        free(fb);
        free_cels();
        moon_shutdown();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,
                            SDL_PIXELFORMAT_RGB888,
                            SDL_TEXTUREACCESS_STREAMING,
                            FB_W, FB_H);

    SDL_Rect sprite_dst = { 0, 0, FB_W * SCALE, FB_H * SCALE };

    int running   = 1;
    int paused    = 0;
    int show_bbox = 1;
    SDL_Event ev;

    const Uint32 FRAME_MS = 20;  /* ~50 Hz */

    while (running) {
        Uint32 t0 = SDL_GetTicks();

        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;
                if (k == SDLK_ESCAPE || k == SDLK_q) {
                    running = 0;
                } else if (k == SDLK_RIGHT || k == SDLK_n) {
                    /* Next animation */
                    g_anim_index = (g_anim_index + 1) % ANIM_REGISTRY_COUNT;
                    load_animation(g_anim_index);
                } else if (k == SDLK_LEFT || k == SDLK_p) {
                    /* Previous animation */
                    g_anim_index = (g_anim_index + ANIM_REGISTRY_COUNT - 1)
                                   % ANIM_REGISTRY_COUNT;
                    load_animation(g_anim_index);
                } else if (k == SDLK_SPACE) {
                    if (ix_entity_advance(&g_entity)) {
                        restart_animation();
                    } else {
                        g_step_index++;
                    }
                } else if (k == SDLK_f) {
                    paused = !paused;
                } else if (k == SDLK_b) {
                    show_bbox = !show_bbox;
                } else if (k == SDLK_r) {
                    restart_animation();
                }
            }
        }

        if (!running) break;

        /* Update window title */
        {
            char title[256];
            snprintf(title, sizeof(title),
                     "moon-anim-viewer [%d/%d] %s",
                     g_anim_index + 1, ANIM_REGISTRY_COUNT,
                     g_anim_registry[g_anim_index].name);
            SDL_SetWindowTitle(win, title);
        }

        memset(fb, 0x11, (size_t)(FB_W * FB_H) * 4);

        if (!paused) {
            const uint8_t *prev_frame = g_entity.frame_start;
            if (ix_entity_tick(&g_entity, &g_slots, palette, fb, FB_W, FB_H)) {
                restart_animation();
            } else if (g_entity.frame_start != prev_frame) {
                g_step_index++;
            }
        } else {
            ix_entity_draw(&g_entity, &g_slots, palette, fb, FB_W, FB_H);
        }

        SDL_UpdateTexture(tex, NULL, fb, FB_W * 4);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, &sprite_dst);

        /* Bounding boxes (green) for mapped slots */
        if (show_bbox) {
            IxBBox bboxes[16];
            int nb = ix_entity_get_bboxes(&g_entity, &g_slots, bboxes, 16);
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 255);
            for (int i = 0; i < nb; i++) {
                SDL_Rect r = {
                    bboxes[i].x * SCALE,
                    bboxes[i].y * SCALE,
                    bboxes[i].w * SCALE,
                    bboxes[i].h * SCALE
                };
                SDL_RenderDrawRect(ren, &r);
            }
        }

        /* Orange placeholder boxes for unmapped slots */
        draw_unmapped_slots(ren, &g_entity, &g_slots, SCALE);

        /* Info panel background */
        {
            SDL_Rect panel = { 0, FB_H * SCALE, FB_W * SCALE, INFO_H };
            SDL_SetRenderDrawColor(ren, 0x22, 0x22, 0x22, 255);
            SDL_RenderFillRect(ren, &panel);
        }

        /* Info line 1: animation index + name */
        {
            char line1[256];
            snprintf(line1, sizeof(line1), "[%d/%d] %s  step:%d",
                     g_anim_index + 1, ANIM_REGISTRY_COUNT,
                     g_anim_registry[g_anim_index].name,
                     g_step_index);
            sdl_draw_text(ren, 4, FB_H * SCALE + 4, line1, 255, 220, 80);
        }

        /* Info line 2: current draw instructions */
        {
            char instr_buf[256];
            describe_step(&g_entity, instr_buf, (int)sizeof(instr_buf));
            sdl_draw_text(ren, 4, FB_H * SCALE + 16, instr_buf, 180, 220, 255);
        }

        /* Info line 3: loaded CEL slots */
        {
            char slots_buf[128] = "cels:";
            int  pos = 5;
            for (int i = 0; i < IX_SLOTS; i++) {
                if (g_slots.cel[i]) {
                    pos += snprintf(slots_buf + pos, sizeof(slots_buf) - (size_t)pos,
                                    " %d=%s", i,
                                    g_anim_registry[g_anim_index].cel_files[i]);
                }
            }
            if (pos == 5)
                snprintf(slots_buf + 5, sizeof(slots_buf) - 5, " (none mapped)");
            sdl_draw_text(ren, 4, FB_H * SCALE + 28, slots_buf, 160, 200, 160);
        }

        /* Info line 4: mode indicators + controls */
        {
            char line4[256];
            snprintf(line4, sizeof(line4),
                     "%s bbox:%s | N/P=next/prev R=restart SPACE=step F=frame ESC=quit",
                     paused ? "[PAUSED]" : "[LOOP]",
                     show_bbox ? "ON" : "OFF");
            sdl_draw_text(ren, 4, FB_H * SCALE + 40, line4, 140, 140, 140);
        }

        SDL_RenderPresent(ren);

        Uint32 elapsed = SDL_GetTicks() - t0;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    printf("User quit.\n");

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

#else /* no SDL2 — ASCII debug dump */

    printf("SDL2 not available — printing step info to stdout.\n\n");
    printf("Animation: %s\n", g_anim_registry[g_anim_index].name);
    int step = 0;
    while (!g_entity.finished) {
        printf("Step %d\n", step);
        for (int t = 0; t < (int)g_entity.speed && !g_entity.finished; t++)
            ix_entity_advance(&g_entity);
        step++;
    }
    printf("Script finished (%d steps).\n", step);

#endif /* HAVE_SDL2 */

    free(fb);
    free_cels();
    moon_shutdown();
    return 0;
}
