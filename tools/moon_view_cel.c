/*
 * moon-view-cel — render a frame from a Moonstone CEL sprite file.
 *
 * Usage: moon-view-cel <file.cel> [frame_index]
 *
 * With SDL2: opens a window and displays the frame.
 * Without SDL2: prints an ASCII-art representation to stdout.
 *
 * The CEL pixel data is planar (Amiga bitplane format). To display it,
 * each pixel is reconstructed from all bitplanes and mapped through a
 * default palette (EGA-like 16-colour fallback when no PIV palette is
 * available).
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* Default 16-colour Amiga-style palette (OCS hardware default) */
static const uint32_t default_pal16[16] = {
    0x000000, 0xAAAAAA, 0x000088, 0x0000FF,
    0x008800, 0x00FF00, 0x008888, 0x00FFFF,
    0x880000, 0xFF0000, 0x880088, 0xFF00FF,
    0x888800, 0xFFFF00, 0x888888, 0xFFFFFF,
};

/* Extract pixel index from planar data */
static int get_pixel(const MoonCelFrame *fr, int x, int y)
{
    if (x < 0 || x >= (int)fr->width || y < 0 || y >= (int)fr->height)
        return 0;
    int row_words = ((int)fr->width + 15) / 16;
    int row_bytes = row_words * 2;
    int pixel_idx = 0;
    for (int pl = 0; pl < (int)fr->planes; pl++) {
        size_t byte_off = (size_t)(y * fr->planes + pl) * (size_t)row_bytes
                        + (size_t)(x / 8);
        int bit_off = 7 - (x % 8);
        if (fr->data) {
            int bit = (fr->data[byte_off] >> bit_off) & 1;
            pixel_idx |= (bit << pl);
        }
    }
    return pixel_idx;
}

#ifdef HAVE_SDL2
static void show_sdl(const MoonCelFrame *fr, const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    int scale = 2;
    SDL_Window   *win = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            (int)fr->width * scale, (int)fr->height * scale,
                            SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    SDL_Texture *tex = SDL_CreateTexture(ren,
                           SDL_PIXELFORMAT_RGB888,
                           SDL_TEXTUREACCESS_STATIC,
                           (int)fr->width, (int)fr->height);

    uint32_t *pixels = (uint32_t *)malloc((size_t)fr->width * (size_t)fr->height * 4);

    for (int y = 0; y < (int)fr->height; y++) {
        for (int x = 0; x < (int)fr->width; x++) {
            int idx = get_pixel(fr, x, y);
            pixels[y * (int)fr->width + x] = default_pal16[idx & 15];
        }
    }

    SDL_UpdateTexture(tex, NULL, pixels, (int)fr->width * 4);
    free(pixels);

    int running = 1;
    SDL_Event ev;
    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) running = 0;
            if (ev.type == SDL_KEYDOWN &&
                ev.key.keysym.sym == SDLK_ESCAPE) running = 0;
        }
        SDL_RenderClear(ren);
        SDL_RenderCopy(ren, tex, NULL, NULL);
        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
}
#else /* !HAVE_SDL2 */

static void show_ascii(const MoonCelFrame *fr)
{
    static const char shades[] = " .-=+*#%@";
    int n_shades = (int)(sizeof(shades) - 1);

    printf("Frame: %dx%d  planes=%d\n", fr->width, fr->height, fr->planes);
    for (int y = 0; y < (int)fr->height; y++) {
        for (int x = 0; x < (int)fr->width; x++) {
            int idx = get_pixel(fr, x, y);
            int shade = (idx * (n_shades - 1)) / ((1 << fr->planes) - 1);
            if (shade < 0) shade = 0;
            if (shade >= n_shades) shade = n_shades - 1;
            putchar(shades[shade]);
        }
        putchar('\n');
    }
}

#endif /* HAVE_SDL2 */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-view-cel <file.cel> [frame_index]\n");
        return 1;
    }

    const char *path  = argv[1];
    int         frame = 0;
    if (argc >= 3)
        frame = atoi(argv[2]);

    /* Load raw file and decode */
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || (long)fread(buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f); free(buf); return 1;
    }
    fclose(f);

    moon_init(".");
    MoonCel *cel = NULL;

    /* Decode from buffer directly */
    /* We need a minimal decode path: write to tmp file and use moon_cel_load,
     * or expose an internal function. Here we use a temporary approach. */
    {
        /* Write to /tmp for loading via the library API */
        char tmpname[64];
        snprintf(tmpname, sizeof(tmpname), "/tmp/moon_cel_%d.cel", (int)getpid());
        FILE *tmp = fopen(tmpname, "wb");
        if (tmp) {
            fwrite(buf, 1, (size_t)sz, tmp);
            fclose(tmp);
            moon_init("/tmp");
            /* Extract just the filename */
            char fname[64];
            snprintf(fname, sizeof(fname), "moon_cel_%d.cel", (int)getpid());
            cel = moon_cel_load(fname);
            remove(tmpname);
        }
    }
    free(buf);

    if (!cel || cel->frame_count == 0) {
        fprintf(stderr, "Error: failed to decode CEL file\n");
        moon_shutdown();
        return 1;
    }

    if (frame < 0 || frame >= cel->frame_count) {
        fprintf(stderr, "Error: frame %d out of range (0..%d)\n",
                frame, cel->frame_count - 1);
        moon_cel_free(cel);
        moon_shutdown();
        return 1;
    }

    printf("File   : %s\n", path);
    printf("Frames : %d\n", cel->frame_count);

    const MoonCelFrame *fr = &cel->frames[frame];
    printf("Frame  : %d  (%dx%d, %d planes)\n",
           frame, fr->width, fr->height, fr->planes);

#ifdef HAVE_SDL2
    {
        char title[128];
        snprintf(title, sizeof(title), "moon-view-cel: %s [frame %d]", path, frame);
        show_sdl(fr, title);
    }
#else
    show_ascii(fr);
#endif

    moon_cel_free(cel);
    moon_shutdown();
    return 0;
}
