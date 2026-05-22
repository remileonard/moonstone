/*
 * moon_hal.h — Hardware Abstraction Layer for Moonstone port
 *
 * Wraps SDL2 for window management, framebuffer presentation,
 * event handling, and audio playback.
 *
 * The game runs internally at 320×200 (Amiga OCS resolution).
 * The HAL scales the framebuffer to fill the SDL2 window.
 */

#ifndef MOON_HAL_H
#define MOON_HAL_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Game constants                                                      */
/* ------------------------------------------------------------------ */

#define GAME_W   320
#define GAME_H   200

/* ------------------------------------------------------------------ */
/* Input                                                               */
/* ------------------------------------------------------------------ */

/*
 * MoonInput — current state of controls for up to 4 players.
 *
 * Matches the joystick register layout used in program.asm (§6.5):
 * each field is 1 when the direction/button is held this frame.
 */
typedef struct {
    uint8_t up;
    uint8_t down;
    uint8_t left;
    uint8_t right;
    uint8_t fire;    /* fire / action button */
    uint8_t fire2;   /* secondary button     */
} MoonJoy;

typedef struct {
    MoonJoy  joy[4];          /* joystick / gamepad for players 1-4  */
    uint8_t  escape;          /* ESC key — quit / pause              */
    uint8_t  enter;           /* Enter / Return                      */
    uint8_t  space;           /* Space bar                           */
    uint8_t  quit;            /* window close event                  */
    /* raw SDL keyboard state for direct queries */
    const uint8_t *keys;      /* SDL_GetKeyboardState snapshot        */
    int       n_keys;
} MoonInput;

/* ------------------------------------------------------------------ */
/* HAL lifecycle                                                       */
/* ------------------------------------------------------------------ */

/**
 * hal_init — initialise SDL2 window + renderer + audio.
 * @title   : window title string.
 * @scale   : integer display scale factor (1 = 320×200, 2 = 640×400, …).
 * Returns 0 on success, -1 on failure.
 */
int  hal_init(const char *title, int scale);

/** hal_quit — destroy window and shut down SDL2. */
void hal_quit(void);

/* ------------------------------------------------------------------ */
/* Framebuffer                                                         */
/* ------------------------------------------------------------------ */

/**
 * hal_present — upload pixels to the GPU texture and flip to screen.
 * @pixels : ARGB8888 pixel array, GAME_W × GAME_H entries.
 */
void hal_present(const uint32_t *pixels);

/**
 * hal_clear — fill the internal framebuffer with a solid colour.
 * @fb    : pixel array to clear (GAME_W × GAME_H).
 * @color : ARGB8888 fill colour.
 */
void hal_clear(uint32_t *fb, uint32_t color);

/* ------------------------------------------------------------------ */
/* Timing                                                              */
/* ------------------------------------------------------------------ */

/**
 * hal_vbl_wait — throttle to the target frame rate (50 Hz PAL).
 *
 * Call once per game loop iteration after hal_present().
 * On Amiga the game synced to the 50 Hz VBL; we replicate that here.
 */
void hal_vbl_wait(void);

/** hal_ticks — milliseconds elapsed since hal_init(). */
uint32_t hal_ticks(void);

/** hal_delay — sleep for @ms milliseconds. */
void hal_delay(uint32_t ms);

/* ------------------------------------------------------------------ */
/* Events / input                                                      */
/* ------------------------------------------------------------------ */

/**
 * hal_poll — pump SDL events and update the MoonInput snapshot.
 * @inp : receives the current input state.
 * Returns 0 normally, 1 if the application should quit.
 */
int hal_poll(MoonInput *inp);

/* ------------------------------------------------------------------ */
/* Audio                                                               */
/* ------------------------------------------------------------------ */

/**
 * hal_music_play_raw — play a raw ProTracker MOD in memory.
 * @mod_data : decompressed MOD bytes.
 * @mod_len  : byte count.
 * @loop     : non-zero to loop forever.
 * Returns 0 on success, -1 on failure (SDL2_mixer not available).
 */
int  hal_music_play_raw(const uint8_t *mod_data, size_t mod_len, int loop);

/** hal_music_stop — stop any currently playing music. */
void hal_music_stop(void);

/** hal_music_pause / hal_music_resume */
void hal_music_pause(void);
void hal_music_resume(void);

/** hal_music_set_volume — 0..128 (MIX_MAX_VOLUME). */
void hal_music_set_volume(int vol);

/* ------------------------------------------------------------------ */
/* Sound effects                                                       */
/* ------------------------------------------------------------------ */

/**
 * hal_sfx_play — play a raw 8-bit signed mono PCM sample.
 *
 * Mirrors Amiga Paula playback (LAB_0F8C in mog.asm).  The sample data
 * is in the Amiga native format: signed 8-bit PCM, no header.  The HAL
 * converts it to the SDL audio format and plays it on an available SFX
 * channel.
 *
 * @pcm    : pointer to 8-bit signed PCM data.
 * @len    : number of bytes.
 * @freq_hz: playback frequency in Hz.  Use 8363 for the default Amiga
 *           PAL period (428 = C-3: 3546895 / 428 ≈ 8287 Hz; the game
 *           consistently uses 8363 as the base rate).
 *
 * Returns 0 on success, -1 if audio is not available or the call fails.
 * Gracefully no-ops when SDL2_mixer is not compiled in.
 */
int hal_sfx_play(const uint8_t *pcm, size_t len, int freq_hz);

/**
 * hal_sfx_stop_all — stop all currently playing SFX and free resources.
 *
 * Call at scene transitions or when SFX assets are about to be freed.
 */
void hal_sfx_stop_all(void);

/* ------------------------------------------------------------------ */
/* Palette fade helpers                                                */
/* ------------------------------------------------------------------ */

/**
 * hal_fade_to_black — gradually fade an ARGB palette to black.
 * @pal   : array of ARGB32 palette entries (modified in-place).
 * @n     : number of entries.
 * @steps : number of frames over which to fade.
 */
void hal_fade_to_black(uint32_t *pal, int n, int steps);

/**
 * hal_fade_from_black — gradually reveal a target palette from black.
 * @pal    : array to fill (modified in-place, starts at 0).
 * @target : target ARGB32 palette.
 * @n      : number of entries.
 * @steps  : number of frames over which to fade.
 */
void hal_fade_from_black(uint32_t *pal, const uint32_t *target, int n, int steps);

#ifdef __cplusplus
}
#endif

#endif /* MOON_HAL_H */
