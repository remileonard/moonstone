/*
 * moon_hal.c — SDL2 Hardware Abstraction Layer for Moonstone
 */

#include "moon_hal.h"

#include <SDL2/SDL.h>
#ifdef HAVE_SDL2_MIXER
#include "SDL2_mixer.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Module state                                                        */
/* ------------------------------------------------------------------ */

static SDL_Window   *s_window   = NULL;
static SDL_Renderer *s_renderer = NULL;
static SDL_Texture  *s_texture  = NULL;
static int           s_scale    = 2;

/* Timing: target 50 Hz (PAL) = 20 ms per frame */
#define TARGET_MS_PER_FRAME 20u
static uint32_t s_last_frame_ticks = 0;

#ifdef HAVE_SDL2_MIXER
static Mix_Music *s_music    = NULL;
static uint8_t   *s_mod_buf  = NULL;  /* owned copy of MOD data for SDL_RWops */
static size_t     s_mod_len  = 0;

/* SFX channel pool — mirrors Amiga Paula's 4 DMA audio channels.
 * We halt each channel before reuse so the previous Mix_Chunk can be
 * freed safely.  Mix_HaltChannel() acquires the audio lock internally,
 * ensuring the mixer thread has finished reading the old chunk data
 * before we free it.                                                  */
#define SFX_CHANNELS 4
static Mix_Chunk *s_sfx_slots[SFX_CHANNELS];
static int        s_sfx_next = 0;
#endif

/* ------------------------------------------------------------------ */
/* Lifecycle                                                           */
/* ------------------------------------------------------------------ */

int hal_init(const char *title, int scale)
{
    s_scale = (scale < 1) ? 1 : scale;

    Uint32 flags = SDL_INIT_VIDEO | SDL_INIT_TIMER;
#ifdef HAVE_SDL2_MIXER
    flags |= SDL_INIT_AUDIO;
#endif

    if (SDL_Init(flags) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return -1;
    }

#ifdef HAVE_SDL2_MIXER
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        fprintf(stderr, "SDL_mixer warning: %s\n", Mix_GetError());
        /* non-fatal — continue without audio */
    }
    Mix_AllocateChannels(SFX_CHANNELS);
    Mix_VolumeMusic(MIX_MAX_VOLUME);
#endif

    s_window = SDL_CreateWindow(
        title ? title : "Moonstone — A Hard Days Knight",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        GAME_W * s_scale, GAME_H * s_scale,
        SDL_WINDOW_SHOWN);
    if (!s_window) {
        fprintf(stderr, "SDL_CreateWindow error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    s_renderer = SDL_CreateRenderer(s_window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!s_renderer) {
        /* fallback: software renderer */
        s_renderer = SDL_CreateRenderer(s_window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!s_renderer) {
        fprintf(stderr, "SDL_CreateRenderer error: %s\n", SDL_GetError());
        SDL_DestroyWindow(s_window);
        s_window = NULL;
        SDL_Quit();
        return -1;
    }

    SDL_RenderSetLogicalSize(s_renderer, GAME_W, GAME_H);
    SDL_SetRenderDrawColor(s_renderer, 0, 0, 0, 255);

    s_texture = SDL_CreateTexture(s_renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        GAME_W, GAME_H);
    if (!s_texture) {
        fprintf(stderr, "SDL_CreateTexture error: %s\n", SDL_GetError());
        SDL_DestroyRenderer(s_renderer);
        SDL_DestroyWindow(s_window);
        s_window   = NULL;
        s_renderer = NULL;
        SDL_Quit();
        return -1;
    }

    s_last_frame_ticks = SDL_GetTicks();
    return 0;
}

void hal_quit(void)
{
#ifdef HAVE_SDL2_MIXER
    hal_sfx_stop_all();
    hal_music_stop();
    Mix_CloseAudio();
    free(s_mod_buf);
    s_mod_buf = NULL;
    s_mod_len = 0;
#endif

    if (s_texture)  { SDL_DestroyTexture(s_texture);   s_texture  = NULL; }
    if (s_renderer) { SDL_DestroyRenderer(s_renderer); s_renderer = NULL; }
    if (s_window)   { SDL_DestroyWindow(s_window);     s_window   = NULL; }
    SDL_Quit();
}

/* ------------------------------------------------------------------ */
/* Framebuffer                                                         */
/* ------------------------------------------------------------------ */

void hal_present(const uint32_t *pixels)
{
    if (!s_texture) return;
    SDL_UpdateTexture(s_texture, NULL, pixels, GAME_W * (int)sizeof(uint32_t));
    SDL_RenderClear(s_renderer);
    SDL_RenderCopy(s_renderer, s_texture, NULL, NULL);
    SDL_RenderPresent(s_renderer);
}

void hal_clear(uint32_t *fb, uint32_t color)
{
    int n = GAME_W * GAME_H;
    for (int i = 0; i < n; i++)
        fb[i] = color;
}

/* ------------------------------------------------------------------ */
/* Timing                                                              */
/* ------------------------------------------------------------------ */

void hal_vbl_wait(void)
{
    uint32_t now = SDL_GetTicks();
    uint32_t elapsed = now - s_last_frame_ticks;
    if (elapsed < TARGET_MS_PER_FRAME) {
        SDL_Delay(TARGET_MS_PER_FRAME - elapsed);
    }
    s_last_frame_ticks = SDL_GetTicks();
}

uint32_t hal_ticks(void)
{
    return SDL_GetTicks();
}

void hal_delay(uint32_t ms)
{
    SDL_Delay(ms);
}

/* ------------------------------------------------------------------ */
/* Events / input                                                      */
/* ------------------------------------------------------------------ */

int hal_poll(MoonInput *inp)
{
    SDL_Event ev;

    /* Preserve keys pointer across the poll */
    memset(inp, 0, sizeof(*inp));

    while (SDL_PollEvent(&ev)) {
        switch (ev.type) {
        case SDL_QUIT:
            inp->quit = 1;
            return 1;
        case SDL_KEYDOWN:
            switch (ev.key.keysym.sym) {
            case SDLK_ESCAPE: inp->escape = 1; break;
            case SDLK_RETURN:
            case SDLK_KP_ENTER: inp->enter = 1; break;
            case SDLK_SPACE:    inp->space = 1; break;
            default: break;
            }
            break;
        default:
            break;
        }
    }

    /* Keyboard state snapshot */
    inp->keys   = SDL_GetKeyboardState(&inp->n_keys);

    /* Map keyboard to player 1 joystick (WASD + Z/X) */
    if (inp->keys[SDL_SCANCODE_W] || inp->keys[SDL_SCANCODE_UP])
        inp->joy[0].up = 1;
    if (inp->keys[SDL_SCANCODE_S] || inp->keys[SDL_SCANCODE_DOWN])
        inp->joy[0].down = 1;
    if (inp->keys[SDL_SCANCODE_A] || inp->keys[SDL_SCANCODE_LEFT])
        inp->joy[0].left = 1;
    if (inp->keys[SDL_SCANCODE_D] || inp->keys[SDL_SCANCODE_RIGHT])
        inp->joy[0].right = 1;
    if (inp->keys[SDL_SCANCODE_Z] || inp->keys[SDL_SCANCODE_LCTRL])
        inp->joy[0].fire = 1;
    if (inp->keys[SDL_SCANCODE_X] || inp->keys[SDL_SCANCODE_LALT])
        inp->joy[0].fire2 = 1;

    /* Check for quit via Escape or window close */
    if (inp->escape || inp->quit)
        return 1;

    /* Gamepad / joystick support (basic) */
    int num_joysticks = SDL_NumJoysticks();
    for (int p = 0; p < num_joysticks && p < 4; p++) {
        SDL_Joystick *js = SDL_JoystickOpen(p);
        if (!js) continue;
        int ax = SDL_JoystickGetAxis(js, 0);
        int ay = SDL_JoystickGetAxis(js, 1);
        if (ax < -8000) inp->joy[p].left  = 1;
        if (ax >  8000) inp->joy[p].right = 1;
        if (ay < -8000) inp->joy[p].up    = 1;
        if (ay >  8000) inp->joy[p].down  = 1;
        inp->joy[p].fire  = (uint8_t)SDL_JoystickGetButton(js, 0);
        inp->joy[p].fire2 = (uint8_t)SDL_JoystickGetButton(js, 1);
        SDL_JoystickClose(js);
    }

    return 0;
}

/* ------------------------------------------------------------------ */
/* Audio                                                               */
/* ------------------------------------------------------------------ */

int hal_music_play_raw(const uint8_t *mod_data, size_t mod_len, int loop)
{
#ifdef HAVE_SDL2_MIXER
    hal_music_stop();

    /* Copy the data so the SDL_RWops remains valid for the lifetime of
     * the music object. */
    free(s_mod_buf);
    s_mod_buf = (uint8_t *)malloc(mod_len);
    if (!s_mod_buf) return -1;
    memcpy(s_mod_buf, mod_data, mod_len);
    s_mod_len = mod_len;

    SDL_RWops *rw = SDL_RWFromMem(s_mod_buf, (int)s_mod_len);
    if (!rw) {
        fprintf(stderr, "SDL_RWFromMem error: %s\n", SDL_GetError());
        return -1;
    }

    s_music = Mix_LoadMUS_RW(rw, 1 /* freesrc */);
    if (!s_music) {
        fprintf(stderr, "Mix_LoadMUS_RW error: %s\n", Mix_GetError());
        return -1;
    }

    int loops = loop ? -1 : 1;
    if (Mix_PlayMusic(s_music, loops) < 0) {
        fprintf(stderr, "Mix_PlayMusic error: %s\n", Mix_GetError());
        return -1;
    }
    return 0;
#else
    (void)mod_data; (void)mod_len; (void)loop;
    return -1;
#endif
}

void hal_music_stop(void)
{
#ifdef HAVE_SDL2_MIXER
    Mix_HaltMusic();
    if (s_music) {
        Mix_FreeMusic(s_music);
        s_music = NULL;
    }
#endif
}

void hal_music_pause(void)
{
#ifdef HAVE_SDL2_MIXER
    Mix_PauseMusic();
#endif
}

void hal_music_resume(void)
{
#ifdef HAVE_SDL2_MIXER
    Mix_ResumeMusic();
#endif
}

void hal_music_set_volume(int vol)
{
#ifdef HAVE_SDL2_MIXER
    Mix_VolumeMusic(vol);
#else
    (void)vol;
#endif
}

/* ------------------------------------------------------------------ */
/* Sound effects                                                       */
/* ------------------------------------------------------------------ */

#ifdef HAVE_SDL2_MIXER
/*
 * build_mix_chunk — wrap raw Amiga 8-bit signed PCM in a Mix_Chunk.
 *
 * WAV 8-bit format uses UNSIGNED samples (0–255).  The Amiga uses SIGNED
 * 8-bit (-128–127), so every byte is offset by 128 before being wrapped
 * in the WAV container.  SDL_mixer then resamples to the output format
 * (44100 Hz, signed 16-bit stereo) automatically.
 */
static Mix_Chunk *build_mix_chunk(const uint8_t *pcm, size_t len, int freq_hz)
{
    /* 44-byte RIFF PCM WAV header */
    uint32_t data_size   = (uint32_t)len;
    uint32_t chunk_size  = 36u + data_size;
    uint32_t byte_rate   = (uint32_t)freq_hz; /* 1 ch × 1 byte/sample */

    uint8_t hdr[44];
    /* RIFF chunk */
    memcpy(hdr,     "RIFF", 4);
    hdr[4]  = (uint8_t)(chunk_size);
    hdr[5]  = (uint8_t)(chunk_size >> 8);
    hdr[6]  = (uint8_t)(chunk_size >> 16);
    hdr[7]  = (uint8_t)(chunk_size >> 24);
    memcpy(hdr + 8, "WAVE", 4);
    /* fmt  sub-chunk */
    memcpy(hdr + 12, "fmt ", 4);
    hdr[16] = 16; hdr[17] = 0; hdr[18] = 0; hdr[19] = 0; /* chunk size */
    hdr[20] = 1;  hdr[21] = 0;                             /* PCM */
    hdr[22] = 1;  hdr[23] = 0;                             /* mono */
    hdr[24] = (uint8_t)freq_hz;        hdr[25] = (uint8_t)(freq_hz >> 8);
    hdr[26] = (uint8_t)(freq_hz >> 16); hdr[27] = (uint8_t)(freq_hz >> 24);
    hdr[28] = (uint8_t)byte_rate;       hdr[29] = (uint8_t)(byte_rate >> 8);
    hdr[30] = (uint8_t)(byte_rate >> 16); hdr[31] = (uint8_t)(byte_rate >> 24);
    hdr[32] = 1;  hdr[33] = 0; /* block align */
    hdr[34] = 8;  hdr[35] = 0; /* bits per sample */
    /* data sub-chunk */
    memcpy(hdr + 36, "data", 4);
    hdr[40] = (uint8_t)data_size;        hdr[41] = (uint8_t)(data_size >> 8);
    hdr[42] = (uint8_t)(data_size >> 16); hdr[43] = (uint8_t)(data_size >> 24);

    /* Allocate WAV = header + unsigned 8-bit PCM */
    uint8_t *wav = (uint8_t *)malloc(44u + len);
    if (!wav) return NULL;
    memcpy(wav, hdr, 44);
    /* Convert signed 8-bit → unsigned 8-bit (add 128) */
    for (size_t i = 0; i < len; i++)
        wav[44 + i] = (uint8_t)((int)(int8_t)pcm[i] + 128);

    SDL_RWops *rw = SDL_RWFromMem(wav, (int)(44u + len));
    Mix_Chunk *chunk = rw ? Mix_LoadWAV_RW(rw, 1 /* freesrc */) : NULL;
    /* SDL_RWFromMem does NOT take ownership of wav; free it ourselves.
     * Mix_LoadWAV_RW has already decoded the WAV into its own buffer. */
    free(wav);
    return chunk;
}
#endif /* HAVE_SDL2_MIXER */

int hal_sfx_play(const uint8_t *pcm, size_t len, int freq_hz)
{
#ifdef HAVE_SDL2_MIXER
    if (!pcm || len == 0) return -1;
    if (freq_hz <= 0) freq_hz = 8363;

    Mix_Chunk *chunk = build_mix_chunk(pcm, len, freq_hz);
    if (!chunk) return -1;

    int ch = s_sfx_next;
    s_sfx_next = (ch + 1) % SFX_CHANNELS;

    /* Halt the channel first so SDL_mixer is done reading the old chunk,
     * then free the old chunk safely before assigning the new one.      */
    Mix_HaltChannel(ch);
    if (s_sfx_slots[ch]) {
        Mix_FreeChunk(s_sfx_slots[ch]);
        s_sfx_slots[ch] = NULL;
    }
    s_sfx_slots[ch] = chunk;
    Mix_PlayChannel(ch, chunk, 0);
    return 0;
#else
    (void)pcm; (void)len; (void)freq_hz;
    return -1;
#endif
}

void hal_sfx_stop_all(void)
{
#ifdef HAVE_SDL2_MIXER
    for (int i = 0; i < SFX_CHANNELS; i++) {
        Mix_HaltChannel(i);
        if (s_sfx_slots[i]) {
            Mix_FreeChunk(s_sfx_slots[i]);
            s_sfx_slots[i] = NULL;
        }
    }
    s_sfx_next = 0;
#endif
}

/* ------------------------------------------------------------------ */
/* Palette fade helpers                                                */
/* ------------------------------------------------------------------ */

void hal_fade_to_black(uint32_t *pal, int n, int steps)
{
    if (steps < 1) steps = 1;
    for (int s = steps - 1; s >= 0; s--) {
        for (int i = 0; i < n; i++) {
            uint8_t a = (uint8_t)((pal[i] >> 24) & 0xFF);
            uint8_t r = (uint8_t)((pal[i] >> 16) & 0xFF);
            uint8_t g = (uint8_t)((pal[i] >>  8) & 0xFF);
            uint8_t b = (uint8_t)( pal[i]        & 0xFF);
            r = (uint8_t)(r * s / steps);
            g = (uint8_t)(g * s / steps);
            b = (uint8_t)(b * s / steps);
            pal[i] = ((uint32_t)a << 24) | ((uint32_t)r << 16)
                   | ((uint32_t)g << 8)  | b;
        }
    }
    /* Ensure end state is fully black */
    for (int i = 0; i < n; i++)
        pal[i] = pal[i] & 0xFF000000u; /* preserve alpha, zero RGB */
}

void hal_fade_from_black(uint32_t *pal, const uint32_t *target, int n, int steps)
{
    if (steps < 1) steps = 1;
    for (int s = 1; s <= steps; s++) {
        for (int i = 0; i < n; i++) {
            uint8_t tr = (uint8_t)((target[i] >> 16) & 0xFF);
            uint8_t tg = (uint8_t)((target[i] >>  8) & 0xFF);
            uint8_t tb = (uint8_t)( target[i]        & 0xFF);
            uint8_t r  = (uint8_t)(tr * s / steps);
            uint8_t g  = (uint8_t)(tg * s / steps);
            uint8_t b  = (uint8_t)(tb * s / steps);
            pal[i] = 0xFF000000u | ((uint32_t)r << 16)
                   | ((uint32_t)g << 8) | b;
        }
    }
}
