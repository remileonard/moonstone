/*
 * moon-anim-dw1 — display all IMAGEXCEL animation steps for dw1.cel.
 *
 * The animation script LAB_00E6 from program.asm is embedded verbatim as a
 * byte array.  Each animation step is shown until its speed timer expires,
 * then the interpreter advances to the next step.
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
/* Embedded animation script — LAB_00E6 from program.asm              */
/*                                                                     */
/* Source bytes extracted from program.asm lines 2384-2509.           */
/* Opcode 0x0C = slot 3 = dw1.cel.                                    */
/* CALL instruction (0xB4) addresses are zeroed (not needed for       */
/* display; the interpreter skips the full 6-byte instruction).       */
/* ------------------------------------------------------------------ */

static const uint8_t dw1_script[] = {
    /* ---- Step 1: SET_SPEED 10, 2 sprites ---- */
    0x88, 0x0A,                         /* SET_SPEED 10                         */
    0x0C, 0x00, 0x9C, 0x00, 0x00, 0x44, /* dw1[0],  y_delta=-100, x=+68        */
    0x0C, 0x02, 0xEF, 0x00, 0x00, 0x38, /* dw1[2],  y_delta=-17,  x=+56        */
    0xFF, 0x00,                         /* end step                              */

    /* ---- Step 2: SET_SPEED 2, 5 sprites ---- */
    0x88, 0x02,                         /* SET_SPEED 2                           */
    0x0C, 0x07, 0x9C, 0x00, 0x00, 0x5B, /* dw1[7],  y_delta=-100, x=+91        */
    0x0C, 0x08, 0xA9, 0x00, 0x00, 0x3E, /* dw1[8],  y_delta=-87,  x=+62        */
    0x0C, 0x09, 0xC1, 0x00, 0x00, 0x3C, /* dw1[9],  y_delta=-63,  x=+60        */
    0x0C, 0x0A, 0xD9, 0x00, 0x00, 0x36, /* dw1[10], y_delta=-39,  x=+54        */
    0x0C, 0x0B, 0xEC, 0x00, 0x00, 0x25, /* dw1[11], y_delta=-20,  x=+37        */
    0xFF, 0x00,                         /* end step                              */
    0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, /* CALL LAB_0032 (skipped)              */

    /* ---- Step 3a: SET_SPEED 1, 5 sprites ---- */
    0x88, 0x01,                         /* SET_SPEED 1                           */
    0x0C, 0x0E, 0xA6, 0x00, 0x00, 0x36, /* dw1[14], y_delta=-90,  x=+54        */
    0x0C, 0x0F, 0xA6, 0x00, 0x00, 0x59, /* dw1[15], y_delta=-90,  x=+89        */
    0x0C, 0x10, 0x9F, 0x00, 0x00, 0x61, /* dw1[16], y_delta=-97,  x=+97        */
    0x0C, 0x0D, 0xC5, 0x00, 0x00, 0x27, /* dw1[13], y_delta=-59,  x=+39        */
    0x0C, 0x0C, 0xDA, 0x00, 0x00, 0x00, /* dw1[12], y_delta=-38,  x=0          */
    0xFF, 0x00,                         /* end step                              */

    /* ---- Step 3b: speed inherited (1), 5 sprites ---- */
    0x0C, 0x11, 0xA3, 0x00, 0x00, 0x35, /* dw1[17], y_delta=-93,  x=+53        */
    0x0C, 0x12, 0xBA, 0x00, 0x00, 0x27, /* dw1[18], y_delta=-70,  x=+39        */
    0x0C, 0x15, 0xD6, 0x00, 0x00, 0x27, /* dw1[21], y_delta=-42,  x=+39        */
    0x0C, 0x14, 0xD5, 0x00, 0x00, 0x12, /* dw1[20], y_delta=-43,  x=+18        */
    0x0C, 0x13, 0xCA, 0x00, 0xFF, 0xF0, /* dw1[19], y_delta=-54,  x=-16        */
    /* NOTE: 0xFF 0xF0 above is the x_pos word (-16), NOT an FF end marker.     */
    /* The parser reads all 6 draw-instruction bytes atomically.                 */
    0xFF, 0x00,                         /* end step                              */
    0xB4, 0x00, 0x00, 0x00, 0x00, 0x00, /* CALL LAB_0040 (skipped)              */

    /* ---- Step 4: SET_SPEED 20, 5 sprites ---- */
    0x88, 0x14,                         /* SET_SPEED 20                          */
    0x0C, 0x11, 0xA3, 0x00, 0x00, 0x35, /* dw1[17], y_delta=-93,  x=+53        */
    0x0C, 0x12, 0xBA, 0x00, 0x00, 0x27, /* dw1[18], y_delta=-70,  x=+39        */
    0x0C, 0x15, 0xD6, 0x00, 0x00, 0x27, /* dw1[21], y_delta=-42,  x=+39        */
    0x0C, 0x14, 0xD5, 0x00, 0x00, 0x12, /* dw1[20], y_delta=-43,  x=+18        */
    0x0C, 0x13, 0xCA, 0x00, 0xFF, 0xF0, /* dw1[19], y_delta=-54,  x=-16        */
    0xFF, 0xFF                          /* end of script                         */
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
    "Step 1: speed=10 — 2 sprites (frames 0,2)",
    "Step 2: speed=2  — 5 sprites (frames 7-11)",
    "Step 3a: speed=1 — 5 sprites (frames 12-16)",
    "Step 3b: speed=1 — 5 sprites (frames 17-21)",
    "Step 4: speed=20 — 5 sprites (frames 17-21, final)",
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

    /* Initialise the animation entity
     *
     * Position chosen so all sprite pieces fall within the 320×200 window:
     *   base_x = 60,  base_y = 200,  vel_y = 0
     *
     * screen_y range: base_y + vel_y + y_delta = 200 + 0 + (-100 .. -17)
     *               = 100 .. 183  — fully visible
     * screen_x range: base_x + x_pos = 60 + (-16 .. +97)
     *               = 44 .. 157   — fully visible
     */
    IxEntity entity;
    ix_entity_init(&entity, dw1_script);
    entity.base_x    = 60;
    entity.base_y    = 200;
    entity.vel_y     = 0;
    /* Direction 1 matches the actual Amiga game (LAB_0015: MOVE.W #$0001,D3).
     * Frames with draw_flags==1 are drawn as-is; frames with draw_flags==3
     * are flipped horizontally (LAB_020B comparison logic). */
    entity.direction = 1;

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
                    entity.base_x    = 60;
                    entity.base_y    = 200;
                    entity.vel_y     = 0;
                    entity.direction = 0;
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
