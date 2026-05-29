/*
 * moon-anim-dw1 — display the druid walking animation for dw1.cel.
 *
 * The animation script LAB_00D8 from program.asm is embedded verbatim as a
 * byte array.  This is the druid walking scene used in the game intro
 * (LAB_001B / LAB_0024): frame 0 = body, frames 1-8 = legs cycling,
 * frames 9-12 = torch flame cycling.  The sprite traverses from the left
 * edge of the screen to the right edge over 38 animation steps.
 *
 * Controls:
 *   ESC / Q  — quit
 *   SPACE    — force advance to the next step immediately
 *   R        — restart from the beginning of the script
 *
 * The background palette is loaded from bg2.PIV when available (the same
 * PIV used by moon_intro.c / plan_druids_walking).  A built-in 32-colour
 * fallback palette is used when the file cannot be found.
 *
 * Usage: moon-anim-dw1 <asset_directory>
 *   where <asset_directory> is the folder containing dw1.cel (and optionally
 *   bg2.PIV).
 */

#include "imagexcel.h"
#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* ------------------------------------------------------------------ */
/* Embedded animation script — LAB_00D8 from program.asm              */
/*                                                                     */
/* Source bytes extracted from program.asm lines 2153-2200 (DC.L      */
/* longwords, big-endian).  748 bytes total, 38 animation steps.      */
/* Opcode 0x0C = slot 3 = dw1.cel.                                    */
/* Frame layout: 0=body, 1-8=legs (cycle), 9-12=flame (cycle).       */
/* Steps 37-38 have no flame (druid nearly off-screen right).         */
/* ------------------------------------------------------------------ */

static const uint8_t dw1_script[] = {  /* LAB_00D8 — 748 bytes, 38 steps */
    /* step 1:  leg=1  flame=12 body_x=+148 */
    0x0C, 0x01, 0xFB, 0x20, 0x00, 0x91, 0x0C, 0x00, 0xDC, 0x20, 0x00, 0x94,
    0x0C, 0x0C, 0xC5, 0x20, 0x00, 0x92, 0xFF, 0x00,
    /* step 2:  leg=2  flame=11 body_x=+137 */
    0x0C, 0x02, 0xFB, 0x20,
    0x00, 0x8E, 0x0C, 0x00, 0xDD, 0x20, 0x00, 0x89, 0x0C, 0x0B, 0xC6, 0x20,
    0x00, 0x86, 0xFF, 0x00,
    /* step 3:  leg=3  flame=10 body_x=+128 */
    0x0C, 0x03, 0xFB, 0x20, 0x00, 0x8B, 0x0C, 0x00,
    0xDD, 0x20, 0x00, 0x80, 0x0C, 0x0A, 0xC7, 0x20, 0x00, 0x7E, 0xFF, 0x00,
    /* step 4:  leg=4  flame=9  body_x=+116 */
    0x0C, 0x04, 0xFB, 0x20,
    0x00, 0x76, 0x0C, 0x00, 0xDD, 0x20, 0x00, 0x74, 0x0C, 0x09, 0xC7, 0x20,
    0x00, 0x73, 0xFF, 0x00,
    /* step 5:  leg=5  flame=12 body_x=+109 */
    0x0C, 0x05, 0xFB, 0x20, 0x00, 0x6C, 0x0C, 0x00,
    0xDD, 0x20, 0x00, 0x6D, 0x0C, 0x0C, 0xC7, 0x20, 0x00, 0x6B, 0xFF, 0x00,
    /* step 6:  leg=6  flame=11 body_x=+100 */
    0x0C, 0x06, 0xFB, 0x20,
    0x00, 0x6A, 0x0C, 0x00, 0xDD, 0x20, 0x00, 0x64, 0x0C, 0x0B, 0xC5, 0x20,
    0x00, 0x62, 0xFF, 0x00,
    /* step 7:  leg=7  flame=10 body_x=+90 */
    0x0C, 0x07, 0xFB, 0x20, 0x00, 0x65, 0x0C, 0x00,
    0xDD, 0x20, 0x00, 0x5A, 0x0C, 0x0A, 0xC5, 0x20, 0x00, 0x58, 0xFF, 0x00,
    /* step 8:  leg=8  flame=9  body_x=+80 */
    0x0C, 0x08, 0xFC, 0x20,
    0x00, 0x52, 0x0C, 0x00, 0xDE, 0x20, 0x00, 0x50, 0x0C, 0x09, 0xC7, 0x20,
    0x00, 0x4E, 0xFF, 0x00,
    /* step 9:  leg=1  flame=9  body_x=+76 */
    0x0C, 0x01, 0xFC, 0x20, 0x00, 0x4B, 0x0C, 0x00,
    0xDE, 0x20, 0x00, 0x4C, 0x0C, 0x09, 0xC5, 0x20, 0x00, 0x4A, 0xFF, 0x00,
    /* step 10: leg=2  flame=10 body_x=+68 */
    0x0C, 0x02, 0xFC, 0x20,
    0x00, 0x49, 0x0C, 0x00, 0xDE, 0x20, 0x00, 0x44, 0x0C, 0x0A, 0xC7, 0x20,
    0x00, 0x43, 0xFF, 0x00,
    /* step 11: leg=3  flame=11 body_x=+58 */
    0x0C, 0x03, 0xFC, 0x20, 0x00, 0x46, 0x0C, 0x00,
    0xDE, 0x20, 0x00, 0x3A, 0x0C, 0x0B, 0xC6, 0x20, 0x00, 0x38, 0xFF, 0x00,
    /* step 12: leg=4  flame=12 body_x=+47 */
    0x0C, 0x04, 0xFC, 0x20,
    0x00, 0x32, 0x0C, 0x00, 0xDE, 0x20, 0x00, 0x2F, 0x0C, 0x0C, 0xC6, 0x20,
    0x00, 0x2D, 0xFF, 0x00,
    /* step 13: leg=5  flame=9  body_x=+40 */
    0x0C, 0x05, 0xFC, 0x20, 0x00, 0x27, 0x0C, 0x00,
    0xDE, 0x20, 0x00, 0x28, 0x0C, 0x09, 0xC6, 0x20, 0x00, 0x26, 0xFF, 0x00,
    /* step 14: leg=6  flame=10 body_x=+29 */
    0x0C, 0x06, 0xFC, 0x20,
    0x00, 0x24, 0x0C, 0x00, 0xDE, 0x20, 0x00, 0x1D, 0x0C, 0x0A, 0xC7, 0x20,
    0x00, 0x1B, 0xFF, 0x00,
    /* step 15: leg=7  flame=11 body_x=+19 */
    0x0C, 0x07, 0xFD, 0x20, 0x00, 0x1F, 0x0C, 0x00,
    0xDF, 0x20, 0x00, 0x13, 0x0C, 0x0B, 0xC7, 0x20, 0x00, 0x10, 0xFF, 0x00,
    /* step 16: leg=8  flame=12 body_x=+8 */
    0x0C, 0x08, 0xFC, 0x20,
    0x00, 0x0B, 0x0C, 0x00, 0xDF, 0x20, 0x00, 0x08, 0x0C, 0x0C, 0xC7, 0x20,
    0x00, 0x06, 0xFF, 0x00,
    /* step 17: leg=1  flame=9  body_x=+5 */
    0x0C, 0x01, 0xFC, 0x20, 0x00, 0x03, 0x0C, 0x00,
    0xDF, 0x20, 0x00, 0x05, 0x0C, 0x09, 0xC5, 0x20, 0x00, 0x03, 0xFF, 0x00,
    /* step 18: leg=2  flame=10 body_x=-4 */
    0x0C, 0x02, 0xFC, 0x20,
    0x00, 0x01, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0xFC, 0x0C, 0x0A, 0xC6, 0x20,
    0xFF, 0xFA, 0xFF, 0x00,
    /* step 19: leg=3  flame=11 body_x=-16 */
    0x0C, 0x03, 0xFC, 0x20, 0xFF, 0xFD, 0x0C, 0x00,
    0xDF, 0x20, 0xFF, 0xF0, 0x0C, 0x0B, 0xC6, 0x20, 0xFF, 0xEE, 0xFF, 0x00,
    /* step 20: leg=4  flame=12 body_x=-24 */
    0x0C, 0x04, 0xFD, 0x20,
    0xFF, 0xEB, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0xE8, 0x0C, 0x0C, 0xC6, 0x20,
    0xFF, 0xE6, 0xFF, 0x00,
    /* step 21: leg=5  flame=9  body_x=-30 */
    0x0C, 0x05, 0xFC, 0x20, 0xFF, 0xE1, 0x0C, 0x00,
    0xDE, 0x20, 0xFF, 0xE2, 0x0C, 0x09, 0xC6, 0x20, 0xFF, 0xE0, 0xFF, 0x00,
    /* step 22: leg=6  flame=10 body_x=-41 */
    0x0C, 0x06, 0xFC, 0x20,
    0xFF, 0xDD, 0x0C, 0x00, 0xDE, 0x20, 0xFF, 0xD7, 0x0C, 0x0A, 0xC6, 0x20,
    0xFF, 0xD4, 0xFF, 0x00,
    /* step 23: leg=7  flame=11 body_x=-49 */
    0x0C, 0x07, 0xFC, 0x20, 0xFF, 0xDA, 0x0C, 0x00,
    0xDE, 0x20, 0xFF, 0xCF, 0x0C, 0x0B, 0xC5, 0x20, 0xFF, 0xCC, 0xFF, 0x00,
    /* step 24: leg=8  flame=12 body_x=-59 */
    0x0C, 0x08, 0xFC, 0x20,
    0xFF, 0xC7, 0x0C, 0x00, 0xDE, 0x20, 0xFF, 0xC5, 0x0C, 0x0C, 0xC6, 0x20,
    0xFF, 0xC2, 0xFF, 0x00,
    /* step 25: leg=1  flame=9  body_x=-63 */
    0x0C, 0x01, 0xFD, 0x20, 0xFF, 0xBF, 0x0C, 0x00,
    0xDF, 0x20, 0xFF, 0xC1, 0x0C, 0x09, 0xC6, 0x20, 0xFF, 0xBF, 0xFF, 0x00,
    /* step 26: leg=2  flame=10 body_x=-71 */
    0x0C, 0x02, 0xFD, 0x20,
    0xFF, 0xBD, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0xB9, 0x0C, 0x0A, 0xC6, 0x20,
    0xFF, 0xB7, 0xFF, 0x00,
    /* step 27: leg=3  flame=11 body_x=-84 */
    0x0C, 0x03, 0xFD, 0x20, 0xFF, 0xB8, 0x0C, 0x00,
    0xDF, 0x20, 0xFF, 0xAC, 0x0C, 0x0B, 0xC6, 0x20, 0xFF, 0xAA, 0xFF, 0x00,
    /* step 28: leg=4  flame=12 body_x=-92 */
    0x0C, 0x04, 0xFD, 0x20,
    0xFF, 0xA7, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0xA4, 0x0C, 0x0C, 0xC6, 0x20,
    0xFF, 0xA3, 0xFF, 0x00,
    /* step 29: leg=5  flame=9  body_x=-98 */
    0x0C, 0x05, 0xFD, 0x20, 0xFF, 0x9D, 0x0C, 0x00,
    0xDE, 0x20, 0xFF, 0x9E, 0x0C, 0x09, 0xC5, 0x20, 0xFF, 0x9C, 0xFF, 0x00,
    /* step 30: leg=6  flame=10 body_x=-109 */
    0x0C, 0x06, 0xFC, 0x20,
    0xFF, 0x9A, 0x0C, 0x00, 0xDE, 0x20, 0xFF, 0x93, 0x0C, 0x0A, 0xC5, 0x20,
    0xFF, 0x91, 0xFF, 0x00,
    /* step 31: leg=7  flame=11 body_x=-118 */
    0x0C, 0x07, 0xFC, 0x20, 0xFF, 0x96, 0x0C, 0x00,
    0xDE, 0x20, 0xFF, 0x8A, 0x0C, 0x0B, 0xC5, 0x20, 0xFF, 0x88, 0xFF, 0x00,
    /* step 32: leg=8  flame=12 body_x=-126 */
    0x0C, 0x08, 0xFD, 0x20,
    0xFF, 0x84, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0x82, 0x0C, 0x0C, 0xC7, 0x20,
    0xFF, 0x80, 0xFF, 0x00,
    /* step 33: leg=1  flame=9  body_x=-132 */
    0x0C, 0x01, 0xFD, 0x20, 0xFF, 0x7B, 0x0C, 0x00,
    0xDF, 0x20, 0xFF, 0x7C, 0x0C, 0x09, 0xC7, 0x20, 0xFF, 0x7A, 0xFF, 0x00,
    /* step 34: leg=2  flame=10 body_x=-140 */
    0x0C, 0x02, 0xFD, 0x20,
    0xFF, 0x79, 0x0C, 0x00, 0xE0, 0x20, 0xFF, 0x74, 0x0C, 0x0A, 0xC5, 0x20,
    0xFF, 0x73, 0xFF, 0x00,
    /* step 35: leg=3  flame=11 body_x=-150 */
    0x0C, 0x03, 0xFD, 0x20, 0xFF, 0x76, 0x0C, 0x00,
    0xDF, 0x20, 0xFF, 0x6A, 0x0C, 0x0B, 0xC5, 0x20, 0xFF, 0x68, 0xFF, 0x00,
    /* step 36: leg=4  flame=12 body_x=-162 */
    0x0C, 0x04, 0xFD, 0x20,
    0xFF, 0x61, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0x5E, 0x0C, 0x0C, 0xC6, 0x20,
    0xFF, 0x5C, 0xFF, 0x00,
    /* step 37: leg=5  (no flame)  body_x=-171 */
    0x0C, 0x05, 0xFC, 0x20, 0xFF, 0x53, 0x0C, 0x00,
    0xDE, 0x20, 0xFF, 0x55, 0xFF, 0x00,
    /* step 38: leg=6  (no flame)  body_x=-181 */
    0x0C, 0x06, 0xFC, 0x20,
    0xFF, 0x52, 0x0C, 0x00, 0xDF, 0x20, 0xFF, 0x4B, 0xFF, 0xFF   /* end of script */
};

/* ------------------------------------------------------------------ */
/* Fallback palette — 32-colour Amiga-style OCS                       */
/* ------------------------------------------------------------------ */

static const uint32_t fallback_pal32[32] = {
    0x000000, 0x111111, 0x222222, 0x333333,
    0x444444, 0x555555, 0x666666, 0x777777,
    0x888888, 0x999999, 0xAAAAAA, 0xBBBBBB,
    0xCCCCCC, 0xDDDDDD, 0xEEEEEE, 0xFFFFFF,
    0x880000, 0xFF0000, 0x008800, 0x00FF00,
    0x000088, 0x0000FF, 0x888800, 0xFFFF00,
    0x880088, 0xFF00FF, 0x008888, 0x00FFFF,
    0x884400, 0xFF8800, 0x004488, 0x0088FF,
};

/* ------------------------------------------------------------------ */
/* Step label strings for the window title                            */
/* ------------------------------------------------------------------ */

static const char *step_labels[] = {
    "Step  1/38 — leg=1, flame=12, body_x=+148",
    "Step  2/38 — leg=2, flame=11, body_x=+137",
    "Step  3/38 — leg=3, flame=10, body_x=+128",
    "Step  4/38 — leg=4, flame=9,  body_x=+116",
    "Step  5/38 — leg=5, flame=12, body_x=+109",
    "Step  6/38 — leg=6, flame=11, body_x=+100",
    "Step  7/38 — leg=7, flame=10, body_x=+90",
    "Step  8/38 — leg=8, flame=9,  body_x=+80",
    "Step  9/38 — leg=1, flame=9,  body_x=+76",
    "Step 10/38 — leg=2, flame=10, body_x=+68",
    "Step 11/38 — leg=3, flame=11, body_x=+58",
    "Step 12/38 — leg=4, flame=12, body_x=+47",
    "Step 13/38 — leg=5, flame=9,  body_x=+40",
    "Step 14/38 — leg=6, flame=10, body_x=+29",
    "Step 15/38 — leg=7, flame=11, body_x=+19",
    "Step 16/38 — leg=8, flame=12, body_x=+8",
    "Step 17/38 — leg=1, flame=9,  body_x=+5",
    "Step 18/38 — leg=2, flame=10, body_x=-4",
    "Step 19/38 — leg=3, flame=11, body_x=-16",
    "Step 20/38 — leg=4, flame=12, body_x=-24",
    "Step 21/38 — leg=5, flame=9,  body_x=-30",
    "Step 22/38 — leg=6, flame=10, body_x=-41",
    "Step 23/38 — leg=7, flame=11, body_x=-49",
    "Step 24/38 — leg=8, flame=12, body_x=-59",
    "Step 25/38 — leg=1, flame=9,  body_x=-63",
    "Step 26/38 — leg=2, flame=10, body_x=-71",
    "Step 27/38 — leg=3, flame=11, body_x=-84",
    "Step 28/38 — leg=4, flame=12, body_x=-92",
    "Step 29/38 — leg=5, flame=9,  body_x=-98",
    "Step 30/38 — leg=6, flame=10, body_x=-109",
    "Step 31/38 — leg=7, flame=11, body_x=-118",
    "Step 32/38 — leg=8, flame=12, body_x=-126",
    "Step 33/38 — leg=1, flame=9,  body_x=-132",
    "Step 34/38 — leg=2, flame=10, body_x=-140",
    "Step 35/38 — leg=3, flame=11, body_x=-150",
    "Step 36/38 — leg=4, flame=12, body_x=-162",
    "Step 37/38 — leg=5 (no flame), body_x=-171",
    "Step 38/38 — leg=6 (no flame), body_x=-181",
};
#define N_STEPS ((int)(sizeof(step_labels) / sizeof(step_labels[0])))

/* ------------------------------------------------------------------ */
/* Main                                                                */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    const char *asset_dir = (argc > 1) ? argv[1] : ".";

    moon_init(asset_dir);

    /* Load dw1.cel */
    MoonCel *dw1 = moon_cel_load("dw1.cel");
    if (!dw1) {
        fprintf(stderr, "moon-anim-dw1: failed to load dw1.cel from '%s'\n",
                asset_dir);
        moon_shutdown();
        return 1;
    }
    printf("dw1.cel loaded: %d frames\n", dw1->frame_count);

    /* Build CEL slot table — dw1.cel lives at slot 3 (opcode 0x0C / 4 = 3) */
    IxCelSlots slots;
    memset(&slots, 0, sizeof(slots));
    slots.cel[3] = dw1;

    /* Try to load the background palette from bg2.PIV */
    uint32_t palette[32];
    memcpy(palette, fallback_pal32, sizeof(palette));

    MoonPiv *bg2 = moon_piv_load("bg2.PIV");
    if (!bg2) bg2 = moon_piv_load("bg2.piv"); /* case-insensitive fallback */
    if (bg2) {
        /* MoonPiv.palette is a fixed 32-entry array of Amiga 12-bit 0x0RGB values */
        for (int i = 0; i < 32; i++) {
            uint16_t c = bg2->palette[i]; /* Amiga OCS 12-bit: 0x0RGB */
            uint8_t r  = ((c >> 8) & 0x0F) * 17;
            uint8_t g  = ((c >> 4) & 0x0F) * 17;
            uint8_t b  = ((c >> 0) & 0x0F) * 17;
            palette[i] = ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
        }
        printf("bg2.PIV palette loaded (32 colours)\n");
    } else {
        printf("bg2.PIV not found — using fallback palette\n");
    }
    if (bg2) moon_piv_free(bg2);

    /* Framebuffer: 320×200, ARGB8888 */
    const int FB_W = 320;
    const int FB_H = 200;
    uint32_t *fb   = (uint32_t *)calloc((size_t)(FB_W * FB_H), 4);
    if (!fb) {
        fprintf(stderr, "moon-anim-dw1: out of memory\n");
        moon_cel_free(dw1);
        moon_shutdown();
        return 1;
    }

    /* Initialise the animation entity.
     *
     * Matches the Amiga game entity table (LAB_0015 / LAB_0024 in program.asm):
     *   base_x = 0xA0 = 160, base_y = 0, vel_y = 0x64 = 100, direction = 1
     *
     * The original game uses direction=1 so the druid walks right→left
     * (x_pos goes from +148 down to -181 relative to base_x=160).
     * We use direction=3 here so the x formula becomes
     *   blit_x = base_x - x_pos - frame_width
     * which reverses the traversal to left→right across the 320-px screen.
     *
     * screen_y range: base_y + vel_y + y_delta
     *   flame : 0 + 100 + (−59) = 41  (upper part of screen)
     *   body  : 0 + 100 + (−36) = 64
     *   legs  : 0 + 100 + (−5)  = 95
     */
    IxEntity entity;
    ix_entity_init(&entity, dw1_script);
    entity.base_x    = 160;
    entity.base_y    = 0;
    entity.vel_y     = 100;
    /* Direction 3: sprite faces right; x formula is base_x − x_pos − width,
     * producing a left-to-right traversal across the 320-pixel screen. */
    entity.direction = 3;

    int step_index = 0; /* which step label to show */

#ifdef HAVE_SDL2
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
        free(fb);
        moon_cel_free(dw1);
        moon_shutdown();
        return 1;
    }

    const int SCALE = 2;
    SDL_Window *win = SDL_CreateWindow("moon-anim-dw1",
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          FB_W * SCALE, FB_H * SCALE,
                          SDL_WINDOW_SHOWN);
    if (!win) {
        fprintf(stderr, "SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        free(fb);
        moon_cel_free(dw1);
        moon_shutdown();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,
                            SDL_PIXELFORMAT_RGB888,
                            SDL_TEXTUREACCESS_STREAMING,
                            FB_W, FB_H);

    /* Update window title with the first step label */
    {
        char title[256];
        snprintf(title, sizeof(title), "moon-anim-dw1 — %s",
                 step_labels[step_index < N_STEPS ? step_index : N_STEPS - 1]);
        SDL_SetWindowTitle(win, title);
    }

    int running = 1;
    SDL_Event ev;

    /* ~50 Hz to match the original Amiga VBL rate */
    const Uint32 FRAME_MS = 20;

    while (running) {
        Uint32 t0 = SDL_GetTicks();

        /* Handle events */
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) {
                running = 0;
            } else if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;
                if (k == SDLK_ESCAPE || k == SDLK_q) {
                    running = 0;
                } else if (k == SDLK_SPACE) {
                    /* Force advance to next step */
                    entity.timer = 1;
                    if (ix_entity_advance(&entity)) running = 0;
                    else {
                        step_index++;
                        char title[256];
                        snprintf(title, sizeof(title),
                                 "moon-anim-dw1 — %s",
                                 step_labels[step_index < N_STEPS
                                             ? step_index : N_STEPS - 1]);
                        SDL_SetWindowTitle(win, title);
                    }
                } else if (k == SDLK_r) {
                    /* Restart script */
                    ix_entity_init(&entity, dw1_script);
                    entity.base_x    = 160;
                    entity.base_y    = 0;
                    entity.vel_y     = 100;
                    entity.direction = 3;
                    step_index = 0;
                    SDL_SetWindowTitle(win, "moon-anim-dw1 — restarted");
                }
            }
        }

        if (!running) break;

        /* Clear framebuffer */
        memset(fb, 0x11, (size_t)(FB_W * FB_H) * 4); /* dark grey background */

        /* Tick the entity: draw + conditionally advance */
        const uint8_t *prev_frame_start = entity.frame_start;
        if (ix_entity_tick(&entity, &slots, palette, fb, FB_W, FB_H)) {
            running = 0; /* script finished */
        } else if (entity.frame_start != prev_frame_start) {
            /* ix_entity_advance moved to a new step */
            step_index++;
            char title[256];
            snprintf(title, sizeof(title),
                     "moon-anim-dw1 — %s",
                     step_labels[step_index < N_STEPS
                                 ? step_index : N_STEPS - 1]);
            SDL_SetWindowTitle(win, title);
        }

        /* Upload framebuffer to SDL texture and present */
        SDL_UpdateTexture(tex, NULL, fb, FB_W * 4);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);

        /* Cap to ~50 Hz */
        Uint32 elapsed = SDL_GetTicks() - t0;
        if (elapsed < FRAME_MS)
            SDL_Delay(FRAME_MS - elapsed);
    }

    printf("Script finished (or user quit).\n");

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();

#else /* no SDL2 — ASCII debug dump */

    printf("SDL2 not available — printing step info to stdout.\n\n");
    int step = 0;
    while (!entity.finished) {
        printf("Step %d: %s\n",
               step, step < N_STEPS ? step_labels[step] : "(extra)");
        /* Advance through each speed tick (just count them) */
        for (int t = 0; t < (int)entity.speed && !entity.finished; t++)
            ix_entity_advance(&entity);
        step++;
    }
    printf("Script finished.\n");

#endif /* HAVE_SDL2 */

    free(fb);
    moon_cel_free(dw1);
    moon_shutdown();
    return 0;
}
