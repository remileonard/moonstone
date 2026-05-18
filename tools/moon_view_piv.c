/*
 * moon-view-piv — decode and display a Moonstone PIV background image.
 *
 * Usage: moon-view-piv <file.PIV>
 *
 * With SDL2: opens a window and displays the background.
 * Without SDL2: prints palette + frame info to stdout.
 *
 * The bitmap is planar (Amiga-style). SDL output converts each pixel
 * index → palette entry → RGB for display.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* Convert Amiga 12-bit to 32-bit ARGB */
static uint32_t amiga_to_argb(uint16_t c)
{
    uint8_t r = (uint8_t)(((c >> 8) & 0xF) * 17);
    uint8_t g = (uint8_t)(((c >> 4) & 0xF) * 17);
    uint8_t b = (uint8_t)(( c       & 0xF) * 17);
    return (uint32_t)(0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

/* Extract pixel index from planar bitmap.
 *
 * Pixel data is stored plane-sequential (matching the Amiga hardware
 * layout and the LAB_0408 / LAB_043A output):
 *   bitplane 0: rows 0..height-1  (byte 0 .. row_bytes*height-1)
 *   bitplane 1: rows 0..height-1  (next row_bytes*height bytes)
 *   …
 * Within each plane, MSB of each byte is the left-most pixel.
 */
static int get_pixel(const MoonPiv *piv, int x, int y)
{
    int row_bytes = ((piv->width + 15) / 16) * 2;
    int pixel_idx = 0;
    for (int pl = 0; pl < piv->planes; pl++) {
        size_t byte_off = (size_t)pl  * (size_t)piv->height * (size_t)row_bytes
                        + (size_t)y   * (size_t)row_bytes
                        + (size_t)(x / 8);
        int bit = (piv->bitmap[byte_off] >> (7 - (x % 8))) & 1;
        pixel_idx |= (bit << pl);
    }
    return pixel_idx;
}

#ifdef HAVE_SDL2
static void show_sdl(MoonPiv *piv, const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return;
    }

    SDL_Window   *win = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            piv->width, piv->height, SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,
                            SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_STATIC,
                            piv->width, piv->height);

    uint32_t *pixels = (uint32_t *)malloc((size_t)piv->width * (size_t)piv->height * 4);
    for (int y = 0; y < piv->height; y++) {
        for (int x = 0; x < piv->width; x++) {
            int idx = get_pixel(piv, x, y);
            pixels[y * piv->width + x] = amiga_to_argb(piv->palette[idx & 31]);
        }
    }

    SDL_UpdateTexture(tex, NULL, pixels, piv->width * 4);
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
#endif

static void print_info(const MoonPiv *piv, const char *path)
{
    printf("File    : %s\n", path);
    printf("Size    : %dx%d\n", piv->width, piv->height);
    printf("Planes  : %d (%d colours)\n", piv->planes, 1 << piv->planes);
    printf("Palette :\n");
    int colours = 1 << piv->planes;
    for (int i = 0; i < colours && i < 32; i++) {
        uint16_t c = piv->palette[i];
        uint8_t r = (uint8_t)(((c >> 8) & 0xF) * 17);
        uint8_t g = (uint8_t)(((c >> 4) & 0xF) * 17);
        uint8_t b = (uint8_t)(( c       & 0xF) * 17);
        printf("  %2d: #%03X  (#%02X%02X%02X)\n", i, (unsigned)c, r, g, b);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-view-piv <file.PIV>\n");
        return 1;
    }

    const char *path = argv[1];

    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", path); return 1; }
    fseek(f, 0, SEEK_END); long sz = ftell(f); rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || (long)fread(buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f); free(buf); return 1;
    }
    fclose(f);

    MoonPiv *piv = moon_piv_load_from_buffer(buf, (size_t)sz);
    free(buf);

    if (!piv) {
        fprintf(stderr, "Error: failed to parse '%s' as PIV\n", path);
        return 1;
    }

    print_info(piv, path);

#ifdef HAVE_SDL2
    {
        char title[256];
        snprintf(title, sizeof(title), "moon-view-piv: %s", path);
        show_sdl(piv, title);
    }
#endif

    moon_piv_free(piv);
    return 0;
}
