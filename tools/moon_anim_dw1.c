/*
 * moon-anim-dw1 — display the druid walking animation for dw1.cel.
 *
 * The animation script LAB_00D8 from program.asm is embedded verbatim as a
 * byte array.  This is the druid walking scene used in the game intro
 * (LAB_001B / LAB_0024): frame 0 = body, frames 1-8 = legs cycling,
 * frames 9-12 = torch flame cycling.  The sprite traverses from the right
 * edge of the screen to the left edge over 38 animation steps.
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
 *   SPACE    — advance one animation step immediately
 *   F        — toggle frame-by-frame mode (pause/resume auto-advance)
 *   B        — toggle bounding-box display
 *   R        — restart from the beginning of the script
 *
 * Debug features:
 *   - Green bounding boxes around each sprite drawn in the current step.
 *   - Info panel below the sprite area: step label, decoded instructions,
 *     and current mode indicators.
 *   - Animation loops automatically when the script reaches its end.
 *   - Frame-by-frame mode lets you inspect each step individually.
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
/* IX_DRAW(3) = slot 3 = dw1.cel.                                     */
/* Frame layout: 0=body, 1-8=legs (cycle), 9-12=flame (cycle).       */
/* Steps 37-38 have no flame (druid nearly off-screen right).         */
/* ------------------------------------------------------------------ */

static const uint8_t dw1_script[] = {  /* LAB_00D8 — 748 bytes, 38 steps */
    /* step 1:  leg=1  flame=12 body_x=+148 */
    IX_DRAW(3), 0x01, 0xFB, 0x20, 0x00, 0x91,  /* legs  fr1  y=-5  x=+145 */
    IX_DRAW(3), 0x00, 0xDC, 0x20, 0x00, 0x94,  /* body  fr0  y=-36 x=+148 */
    IX_DRAW(3), 0x0C, 0xC5, 0x20, 0x00, 0x92,  /* flame fr12 y=-59 x=+146 */
    IX_STEP_NEXT,
    /* step 2:  leg=2  flame=11 body_x=+137 */
    IX_DRAW(3), 0x02, 0xFB, 0x20, 0x00, 0x8E,  /* legs  fr2  y=-5  x=+142 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x89,  /* body  fr0  y=-35 x=+137 */
    IX_DRAW(3), 0x0B, 0xC6, 0x20, 0x00, 0x86,  /* flame fr11 y=-58 x=+134 */
    IX_STEP_NEXT,
    /* step 3:  leg=3  flame=10 body_x=+128 */
    IX_DRAW(3), 0x03, 0xFB, 0x20, 0x00, 0x8B,  /* legs  fr3  y=-5  x=+139 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x80,  /* body  fr0  y=-35 x=+128 */
    IX_DRAW(3), 0x0A, 0xC7, 0x20, 0x00, 0x7E,  /* flame fr10 y=-57 x=+126 */
    IX_STEP_NEXT,
    /* step 4:  leg=4  flame=9  body_x=+116 */
    IX_DRAW(3), 0x04, 0xFB, 0x20, 0x00, 0x76,  /* legs  fr4  y=-5  x=+118 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x74,  /* body  fr0  y=-35 x=+116 */
    IX_DRAW(3), 0x09, 0xC7, 0x20, 0x00, 0x73,  /* flame fr9  y=-57 x=+115 */
    IX_STEP_NEXT,
    /* step 5:  leg=5  flame=12 body_x=+109 */
    IX_DRAW(3), 0x05, 0xFB, 0x20, 0x00, 0x6C,  /* legs  fr5  y=-5  x=+108 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x6D,  /* body  fr0  y=-35 x=+109 */
    IX_DRAW(3), 0x0C, 0xC7, 0x20, 0x00, 0x6B,  /* flame fr12 y=-57 x=+107 */
    IX_STEP_NEXT,
    /* step 6:  leg=6  flame=11 body_x=+100 */
    IX_DRAW(3), 0x06, 0xFB, 0x20, 0x00, 0x6A,  /* legs  fr6  y=-5  x=+106 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x64,  /* body  fr0  y=-35 x=+100 */
    IX_DRAW(3), 0x0B, 0xC5, 0x20, 0x00, 0x62,  /* flame fr11 y=-59 x=+98  */
    IX_STEP_NEXT,
    /* step 7:  leg=7  flame=10 body_x=+90 */
    IX_DRAW(3), 0x07, 0xFB, 0x20, 0x00, 0x65,  /* legs  fr7  y=-5  x=+101 */
    IX_DRAW(3), 0x00, 0xDD, 0x20, 0x00, 0x5A,  /* body  fr0  y=-35 x=+90  */
    IX_DRAW(3), 0x0A, 0xC5, 0x20, 0x00, 0x58,  /* flame fr10 y=-59 x=+88  */
    IX_STEP_NEXT,
    /* step 8:  leg=8  flame=9  body_x=+80 */
    IX_DRAW(3), 0x08, 0xFC, 0x20, 0x00, 0x52,  /* legs  fr8  y=-4  x=+82  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x50,  /* body  fr0  y=-34 x=+80  */
    IX_DRAW(3), 0x09, 0xC7, 0x20, 0x00, 0x4E,  /* flame fr9  y=-57 x=+78  */
    IX_STEP_NEXT,
    /* step 9:  leg=1  flame=9  body_x=+76 */
    IX_DRAW(3), 0x01, 0xFC, 0x20, 0x00, 0x4B,  /* legs  fr1  y=-4  x=+75  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x4C,  /* body  fr0  y=-34 x=+76  */
    IX_DRAW(3), 0x09, 0xC5, 0x20, 0x00, 0x4A,  /* flame fr9  y=-59 x=+74  */
    IX_STEP_NEXT,
    /* step 10: leg=2  flame=10 body_x=+68 */
    IX_DRAW(3), 0x02, 0xFC, 0x20, 0x00, 0x49,  /* legs  fr2  y=-4  x=+73  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x44,  /* body  fr0  y=-34 x=+68  */
    IX_DRAW(3), 0x0A, 0xC7, 0x20, 0x00, 0x43,  /* flame fr10 y=-57 x=+67  */
    IX_STEP_NEXT,
    /* step 11: leg=3  flame=11 body_x=+58 */
    IX_DRAW(3), 0x03, 0xFC, 0x20, 0x00, 0x46,  /* legs  fr3  y=-4  x=+70  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x3A,  /* body  fr0  y=-34 x=+58  */
    IX_DRAW(3), 0x0B, 0xC6, 0x20, 0x00, 0x38,  /* flame fr11 y=-58 x=+56  */
    IX_STEP_NEXT,
    /* step 12: leg=4  flame=12 body_x=+47 */
    IX_DRAW(3), 0x04, 0xFC, 0x20, 0x00, 0x32,  /* legs  fr4  y=-4  x=+50  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x2F,  /* body  fr0  y=-34 x=+47  */
    IX_DRAW(3), 0x0C, 0xC6, 0x20, 0x00, 0x2D,  /* flame fr12 y=-58 x=+45  */
    IX_STEP_NEXT,
    /* step 13: leg=5  flame=9  body_x=+40 */
    IX_DRAW(3), 0x05, 0xFC, 0x20, 0x00, 0x27,  /* legs  fr5  y=-4  x=+39  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x28,  /* body  fr0  y=-34 x=+40  */
    IX_DRAW(3), 0x09, 0xC6, 0x20, 0x00, 0x26,  /* flame fr9  y=-58 x=+38  */
    IX_STEP_NEXT,
    /* step 14: leg=6  flame=10 body_x=+29 */
    IX_DRAW(3), 0x06, 0xFC, 0x20, 0x00, 0x24,  /* legs  fr6  y=-4  x=+36  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0x00, 0x1D,  /* body  fr0  y=-34 x=+29  */
    IX_DRAW(3), 0x0A, 0xC7, 0x20, 0x00, 0x1B,  /* flame fr10 y=-57 x=+27  */
    IX_STEP_NEXT,
    /* step 15: leg=7  flame=11 body_x=+19 */
    IX_DRAW(3), 0x07, 0xFD, 0x20, 0x00, 0x1F,  /* legs  fr7  y=-3  x=+31  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0x00, 0x13,  /* body  fr0  y=-33 x=+19  */
    IX_DRAW(3), 0x0B, 0xC7, 0x20, 0x00, 0x10,  /* flame fr11 y=-57 x=+16  */
    IX_STEP_NEXT,
    /* step 16: leg=8  flame=12 body_x=+8 */
    IX_DRAW(3), 0x08, 0xFC, 0x20, 0x00, 0x0B,  /* legs  fr8  y=-4  x=+11  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0x00, 0x08,  /* body  fr0  y=-33 x=+8   */
    IX_DRAW(3), 0x0C, 0xC7, 0x20, 0x00, 0x06,  /* flame fr12 y=-57 x=+6   */
    IX_STEP_NEXT,
    /* step 17: leg=1  flame=9  body_x=+5 */
    IX_DRAW(3), 0x01, 0xFC, 0x20, 0x00, 0x03,  /* legs  fr1  y=-4  x=+3   */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0x00, 0x05,  /* body  fr0  y=-33 x=+5   */
    IX_DRAW(3), 0x09, 0xC5, 0x20, 0x00, 0x03,  /* flame fr9  y=-59 x=+3   */
    IX_STEP_NEXT,
    /* step 18: leg=2  flame=10 body_x=-4 */
    IX_DRAW(3), 0x02, 0xFC, 0x20, 0x00, 0x01,  /* legs  fr2  y=-4  x=+1   */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xFC,  /* body  fr0  y=-33 x=-4   */
    IX_DRAW(3), 0x0A, 0xC6, 0x20, 0xFF, 0xFA,  /* flame fr10 y=-58 x=-6   */
    IX_STEP_NEXT,
    /* step 19: leg=3  flame=11 body_x=-16 */
    IX_DRAW(3), 0x03, 0xFC, 0x20, 0xFF, 0xFD,  /* legs  fr3  y=-4  x=-3   */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xF0,  /* body  fr0  y=-33 x=-16  */
    IX_DRAW(3), 0x0B, 0xC6, 0x20, 0xFF, 0xEE,  /* flame fr11 y=-58 x=-18  */
    IX_STEP_NEXT,
    /* step 20: leg=4  flame=12 body_x=-24 */
    IX_DRAW(3), 0x04, 0xFD, 0x20, 0xFF, 0xEB,  /* legs  fr4  y=-3  x=-21  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xE8,  /* body  fr0  y=-33 x=-24  */
    IX_DRAW(3), 0x0C, 0xC6, 0x20, 0xFF, 0xE6,  /* flame fr12 y=-58 x=-26  */
    IX_STEP_NEXT,
    /* step 21: leg=5  flame=9  body_x=-30 */
    IX_DRAW(3), 0x05, 0xFC, 0x20, 0xFF, 0xE1,  /* legs  fr5  y=-4  x=-31  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0xE2,  /* body  fr0  y=-34 x=-30  */
    IX_DRAW(3), 0x09, 0xC6, 0x20, 0xFF, 0xE0,  /* flame fr9  y=-58 x=-32  */
    IX_STEP_NEXT,
    /* step 22: leg=6  flame=10 body_x=-41 */
    IX_DRAW(3), 0x06, 0xFC, 0x20, 0xFF, 0xDD,  /* legs  fr6  y=-4  x=-35  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0xD7,  /* body  fr0  y=-34 x=-41  */
    IX_DRAW(3), 0x0A, 0xC6, 0x20, 0xFF, 0xD4,  /* flame fr10 y=-58 x=-44  */
    IX_STEP_NEXT,
    /* step 23: leg=7  flame=11 body_x=-49 */
    IX_DRAW(3), 0x07, 0xFC, 0x20, 0xFF, 0xDA,  /* legs  fr7  y=-4  x=-38  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0xCF,  /* body  fr0  y=-34 x=-49  */
    IX_DRAW(3), 0x0B, 0xC5, 0x20, 0xFF, 0xCC,  /* flame fr11 y=-59 x=-52  */
    IX_STEP_NEXT,
    /* step 24: leg=8  flame=12 body_x=-59 */
    IX_DRAW(3), 0x08, 0xFC, 0x20, 0xFF, 0xC7,  /* legs  fr8  y=-4  x=-57  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0xC5,  /* body  fr0  y=-34 x=-59  */
    IX_DRAW(3), 0x0C, 0xC6, 0x20, 0xFF, 0xC2,  /* flame fr12 y=-58 x=-62  */
    IX_STEP_NEXT,
    /* step 25: leg=1  flame=9  body_x=-63 */
    IX_DRAW(3), 0x01, 0xFD, 0x20, 0xFF, 0xBF,  /* legs  fr1  y=-3  x=-65  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xC1,  /* body  fr0  y=-33 x=-63  */
    IX_DRAW(3), 0x09, 0xC6, 0x20, 0xFF, 0xBF,  /* flame fr9  y=-58 x=-65  */
    IX_STEP_NEXT,
    /* step 26: leg=2  flame=10 body_x=-71 */
    IX_DRAW(3), 0x02, 0xFD, 0x20, 0xFF, 0xBD,  /* legs  fr2  y=-3  x=-67  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xB9,  /* body  fr0  y=-33 x=-71  */
    IX_DRAW(3), 0x0A, 0xC6, 0x20, 0xFF, 0xB7,  /* flame fr10 y=-58 x=-73  */
    IX_STEP_NEXT,
    /* step 27: leg=3  flame=11 body_x=-84 */
    IX_DRAW(3), 0x03, 0xFD, 0x20, 0xFF, 0xB8,  /* legs  fr3  y=-3  x=-72  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xAC,  /* body  fr0  y=-33 x=-84  */
    IX_DRAW(3), 0x0B, 0xC6, 0x20, 0xFF, 0xAA,  /* flame fr11 y=-58 x=-86  */
    IX_STEP_NEXT,
    /* step 28: leg=4  flame=12 body_x=-92 */
    IX_DRAW(3), 0x04, 0xFD, 0x20, 0xFF, 0xA7,  /* legs  fr4  y=-3  x=-89  */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0xA4,  /* body  fr0  y=-33 x=-92  */
    IX_DRAW(3), 0x0C, 0xC6, 0x20, 0xFF, 0xA3,  /* flame fr12 y=-58 x=-93  */
    IX_STEP_NEXT,
    /* step 29: leg=5  flame=9  body_x=-98 */
    IX_DRAW(3), 0x05, 0xFD, 0x20, 0xFF, 0x9D,  /* legs  fr5  y=-3  x=-99  */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0x9E,  /* body  fr0  y=-34 x=-98  */
    IX_DRAW(3), 0x09, 0xC5, 0x20, 0xFF, 0x9C,  /* flame fr9  y=-59 x=-100 */
    IX_STEP_NEXT,
    /* step 30: leg=6  flame=10 body_x=-109 */
    IX_DRAW(3), 0x06, 0xFC, 0x20, 0xFF, 0x9A,  /* legs  fr6  y=-4  x=-102 */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0x93,  /* body  fr0  y=-34 x=-109 */
    IX_DRAW(3), 0x0A, 0xC5, 0x20, 0xFF, 0x91,  /* flame fr10 y=-59 x=-111 */
    IX_STEP_NEXT,
    /* step 31: leg=7  flame=11 body_x=-118 */
    IX_DRAW(3), 0x07, 0xFC, 0x20, 0xFF, 0x96,  /* legs  fr7  y=-4  x=-106 */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0x8A,  /* body  fr0  y=-34 x=-118 */
    IX_DRAW(3), 0x0B, 0xC5, 0x20, 0xFF, 0x88,  /* flame fr11 y=-59 x=-120 */
    IX_STEP_NEXT,
    /* step 32: leg=8  flame=12 body_x=-126 */
    IX_DRAW(3), 0x08, 0xFD, 0x20, 0xFF, 0x84,  /* legs  fr8  y=-3  x=-124 */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0x82,  /* body  fr0  y=-33 x=-126 */
    IX_DRAW(3), 0x0C, 0xC7, 0x20, 0xFF, 0x80,  /* flame fr12 y=-57 x=-128 */
    IX_STEP_NEXT,
    /* step 33: leg=1  flame=9  body_x=-132 */
    IX_DRAW(3), 0x01, 0xFD, 0x20, 0xFF, 0x7B,  /* legs  fr1  y=-3  x=-133 */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0x7C,  /* body  fr0  y=-33 x=-132 */
    IX_DRAW(3), 0x09, 0xC7, 0x20, 0xFF, 0x7A,  /* flame fr9  y=-57 x=-134 */
    IX_STEP_NEXT,
    /* step 34: leg=2  flame=10 body_x=-140 */
    IX_DRAW(3), 0x02, 0xFD, 0x20, 0xFF, 0x79,  /* legs  fr2  y=-3  x=-135 */
    IX_DRAW(3), 0x00, 0xE0, 0x20, 0xFF, 0x74,  /* body  fr0  y=-32 x=-140 */
    IX_DRAW(3), 0x0A, 0xC5, 0x20, 0xFF, 0x73,  /* flame fr10 y=-59 x=-141 */
    IX_STEP_NEXT,
    /* step 35: leg=3  flame=11 body_x=-150 */
    IX_DRAW(3), 0x03, 0xFD, 0x20, 0xFF, 0x76,  /* legs  fr3  y=-3  x=-138 */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0x6A,  /* body  fr0  y=-33 x=-150 */
    IX_DRAW(3), 0x0B, 0xC5, 0x20, 0xFF, 0x68,  /* flame fr11 y=-59 x=-152 */
    IX_STEP_NEXT,
    /* step 36: leg=4  flame=12 body_x=-162 */
    IX_DRAW(3), 0x04, 0xFD, 0x20, 0xFF, 0x61,  /* legs  fr4  y=-3  x=-159 */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0x5E,  /* body  fr0  y=-33 x=-162 */
    IX_DRAW(3), 0x0C, 0xC6, 0x20, 0xFF, 0x5C,  /* flame fr12 y=-58 x=-164 */
    IX_STEP_NEXT,
    /* step 37: leg=5  (no flame)  body_x=-171 */
    IX_DRAW(3), 0x05, 0xFC, 0x20, 0xFF, 0x53,  /* legs  fr5  y=-4  x=-173 */
    IX_DRAW(3), 0x00, 0xDE, 0x20, 0xFF, 0x55,  /* body  fr0  y=-34 x=-171 */
    IX_STEP_NEXT,
    /* step 38: leg=6  (no flame)  body_x=-181 */
    IX_DRAW(3), 0x06, 0xFC, 0x20, 0xFF, 0x52,  /* legs  fr6  y=-4  x=-174 */
    IX_DRAW(3), 0x00, 0xDF, 0x20, 0xFF, 0x4B,  /* body  fr0  y=-33 x=-181 */
    IX_STEP_SCRIPT_END                          /* end of script */
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

/*
 * sdl_draw_char — draw a single 8×8 character at (x, y) on the renderer.
 * Characters outside ASCII 32–127 are rendered as '?'.
 */
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

/*
 * sdl_draw_text — draw a NUL-terminated string starting at (x, y).
 * Characters are 8×8 pixels with no spacing between them.
 */
static void sdl_draw_text(SDL_Renderer *ren, int x, int y, const char *text,
                           uint8_t r, uint8_t g, uint8_t b)
{
    for (int i = 0; text[i]; i++)
        sdl_draw_char(ren, x + i * 8, y, text[i], r, g, b);
}

/* ------------------------------------------------------------------ */
/* Instruction decode helper                                           */
/* ------------------------------------------------------------------ */

/*
 * describe_step — write a human-readable summary of the draw instructions
 * in the current animation step into buf.  Control instructions within
 * the step are shown as their opcode name.
 */
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
            /* Control instruction */
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
            /* Draw instruction */
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
/* Entity restart helper                                               */
/* ------------------------------------------------------------------ */

static void restart_entity(IxEntity *e)
{
    ix_entity_init(e, dw1_script);
    e->base_x    = 160;
    e->base_y    = 0;
    e->vel_y     = 100;
    /* Direction 1: sprite faces left (flip_h=1); x = base_x + x_pos.
     * x_pos goes from +148 to −181, so the druid enters from the right
     * edge and walks off the left edge — matching the original game. */
    e->direction = 1;
}

#endif /* HAVE_SDL2 */

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
    /* Direction 1: sprite faces left (flip_h=1, since frame draw_flags=0x20→
     * frame_dir=3≠1); x formula = base_x + x_pos.  x_pos runs from +148 to
     * −181 → the druid enters from the right edge and walks left, matching
     * the original Amiga intro (LAB_001B / LAB_0024, direction=1). */
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

    const int SCALE  = 2;
    const int INFO_H = 44; /* height of the debug info panel below the sprite area */

    SDL_Window *win = SDL_CreateWindow("moon-anim-dw1",
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          FB_W * SCALE, FB_H * SCALE + INFO_H,
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

    /* Destination rect for the sprite area (2× scaled) */
    SDL_Rect sprite_dst = { 0, 0, FB_W * SCALE, FB_H * SCALE };

    /* Update window title with the first step label */
    {
        char title[256];
        snprintf(title, sizeof(title), "moon-anim-dw1 [%s]  B=bbox F=frame R=restart ESC=quit",
                 step_labels[0]);
        SDL_SetWindowTitle(win, title);
    }

    int running    = 1;
    int paused     = 0; /* frame-by-frame mode when 1 */
    int show_bbox  = 1; /* draw bounding boxes when 1 */
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
                    /* Advance one step immediately (works in both modes) */
                    if (ix_entity_advance(&entity)) {
                        restart_entity(&entity);
                        step_index = 0;
                    } else {
                        step_index = (step_index + 1) % N_STEPS;
                    }
                } else if (k == SDLK_f) {
                    /* Toggle frame-by-frame mode */
                    paused = !paused;
                } else if (k == SDLK_b) {
                    /* Toggle bounding-box display */
                    show_bbox = !show_bbox;
                } else if (k == SDLK_r) {
                    /* Restart script */
                    restart_entity(&entity);
                    step_index = 0;
                }
            }
        }

        if (!running) break;

        /* Clear framebuffer to dark grey */
        memset(fb, 0x11, (size_t)(FB_W * FB_H) * 4);

        if (!paused) {
            /* Normal mode: draw + conditionally advance via timer */
            const uint8_t *prev_frame = entity.frame_start;
            if (ix_entity_tick(&entity, &slots, palette, fb, FB_W, FB_H)) {
                /* Script finished — loop back to start */
                restart_entity(&entity);
                step_index = 0;
            } else if (entity.frame_start != prev_frame) {
                step_index = (step_index + 1) % N_STEPS;
            }
        } else {
            /* Paused mode: draw the current step without advancing */
            ix_entity_draw(&entity, &slots, palette, fb, FB_W, FB_H);
        }

        /* ---- SDL rendering ---- */

        /* Upload framebuffer and blit to sprite area */
        SDL_UpdateTexture(tex, NULL, fb, FB_W * 4);
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, &sprite_dst);

        /* Draw bounding boxes (scaled, green outlines) */
        if (show_bbox) {
            IxBBox bboxes[16];
            int nb = ix_entity_get_bboxes(&entity, &slots, bboxes, 16);
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

        /* Draw info panel background */
        {
            SDL_Rect panel = { 0, FB_H * SCALE, FB_W * SCALE, INFO_H };
            SDL_SetRenderDrawColor(ren, 0x22, 0x22, 0x22, 255);
            SDL_RenderFillRect(ren, &panel);
        }

        /* Info line 1: step label + current instruction decode */
        {
            char instr_buf[256];
            describe_step(&entity, instr_buf, (int)sizeof(instr_buf));

            char line1[512];
            const char *lbl = step_labels[step_index < N_STEPS
                                          ? step_index : N_STEPS - 1];
            snprintf(line1, sizeof(line1), "%s", lbl);
            sdl_draw_text(ren, 4, FB_H * SCALE + 4, line1, 255, 220, 80);

            sdl_draw_text(ren, 4, FB_H * SCALE + 14, instr_buf, 180, 220, 255);
        }

        /* Info line 3: mode indicators + controls */
        {
            char line3[256];
            snprintf(line3, sizeof(line3),
                     "%s  bbox:%s  |  SPACE=step  F=frame  B=bbox  R=restart  ESC=quit",
                     paused ? "[PAUSED]" : "[LOOP]",
                     show_bbox ? "ON" : "OFF");
            sdl_draw_text(ren, 4, FB_H * SCALE + 28, line3, 140, 200, 140);
        }

        SDL_RenderPresent(ren);

        /* Cap to ~50 Hz */
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
