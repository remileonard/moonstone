/*
 * moon-view-test — Display all PIV images contained in the Moonstone "Test"
 *                  multi-image container file.
 *
 * Usage:
 *   moon-view-test <Test_file>
 *
 * Prints info for all images found.  With SDL2, displays each image in a
 * window; press SPACE or RIGHT arrow for the next image, LEFT for previous,
 * 0-9 to jump directly, ESC/Q to quit.
 *
 * Known image index → content (from mog.asm analysis):
 *   8 — overworld map background (320×200, 5 bitplanes, LAB_0142)
 */

#include "moon_assets.h"
#include "moon_testmap.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* ------------------------------------------------------------------ */
/* Helpers shared with moon-view-piv                                   */
/* ------------------------------------------------------------------ */

static uint32_t amiga_to_argb(uint16_t c)
{
    uint8_t r = (uint8_t)(((c >> 8) & 0xF) * 17);
    uint8_t g = (uint8_t)(((c >> 4) & 0xF) * 17);
    uint8_t b = (uint8_t)(( c       & 0xF) * 17);
    return (uint32_t)(0xFF000000u | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b);
}

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

static void print_info(const MoonPiv *piv, int idx, const char *filename)
{
    printf("--- Image %d from '%s' ---\n", idx, filename);
    printf("  Size   : %dx%d  Planes: %d (%d colours)\n",
           piv->width, piv->height, piv->planes, 1 << piv->planes);
    int colours = 1 << piv->planes;
    if (colours > 32) colours = 32;
    for (int i = 0; i < colours; i++) {
        uint16_t c = piv->palette[i];
        uint8_t  r = (uint8_t)(((c >> 8) & 0xF) * 17);
        uint8_t  g = (uint8_t)(((c >> 4) & 0xF) * 17);
        uint8_t  b = (uint8_t)(( c       & 0xF) * 17);
        printf("  pal[%2d] = #%03X  (#%02X%02X%02X)\n", i, (unsigned)c, r, g, b);
    }
}

/* ------------------------------------------------------------------ */
/* Load entire container into memory                                   */
/* ------------------------------------------------------------------ */

static uint8_t *g_buf      = NULL;
static size_t   g_buf_size = 0;

static int load_container(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) { fprintf(stderr, "Error: cannot open '%s'\n", path); return 0; }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    if (sz <= 0) { fclose(f); return 0; }
    g_buf = (uint8_t *)malloc((size_t)sz);
    if (!g_buf || (long)fread(g_buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f); free(g_buf); g_buf = NULL; return 0;
    }
    fclose(f);
    g_buf_size = (size_t)sz;
    return 1;
}

/* ------------------------------------------------------------------ */
/* SDL display                                                          */
/* ------------------------------------------------------------------ */

#ifdef HAVE_SDL2

static uint32_t *piv_to_pixels(const MoonPiv *piv)
{
    uint32_t *px = (uint32_t *)malloc(
        (size_t)piv->width * (size_t)piv->height * sizeof(uint32_t));
    if (!px) return NULL;
    for (int y = 0; y < piv->height; y++)
        for (int x = 0; x < piv->width; x++) {
            int idx = get_pixel(piv, x, y);
            px[y * piv->width + x] =
                amiga_to_argb(piv->palette[idx & ((1 << piv->planes) - 1)]);
        }
    return px;
}

static void run_sdl(const char *filename, int total_images)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return;
    }

    /* Initial image */
    int cur = 0;
    MoonPiv *piv = moon_testmap_load_piv_from_buffer(g_buf, g_buf_size, cur);
    if (!piv) {
        fprintf(stderr, "Error: failed to decode image 0\n");
        SDL_Quit();
        return;
    }

    int win_w = piv->width;
    int win_h = piv->height;

    char title[256];
    snprintf(title, sizeof(title),
             "moon-view-test: %s  [%d/%d]  SPACE/→ next  ← prev  ESC quit",
             filename, cur, total_images - 1);

    SDL_Window   *win = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            win_w, win_h, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,
                            SDL_PIXELFORMAT_ARGB8888,
                            SDL_TEXTUREACCESS_STATIC,
                            win_w, win_h);

    /* Build texture for current image */
    uint32_t *pixels = piv_to_pixels(piv);
    if (pixels) {
        SDL_UpdateTexture(tex, NULL, pixels, win_w * 4);
        free(pixels);
    }
    moon_piv_free(piv);

    int running  = 1;
    int needs_reload = 0;
    SDL_Event ev;

    while (running) {
        while (SDL_PollEvent(&ev)) {
            if (ev.type == SDL_QUIT) { running = 0; break; }
            if (ev.type == SDL_KEYDOWN) {
                SDL_Keycode k = ev.key.keysym.sym;
                if (k == SDLK_ESCAPE || k == SDLK_q) { running = 0; break; }
                if ((k == SDLK_SPACE || k == SDLK_RIGHT) && cur < total_images - 1) {
                    cur++; needs_reload = 1;
                }
                if (k == SDLK_LEFT && cur > 0) {
                    cur--; needs_reload = 1;
                }
                /* 0-9 direct jump */
                if (k >= SDLK_0 && k <= SDLK_9) {
                    int target = (int)(k - SDLK_0);
                    if (target < total_images) { cur = target; needs_reload = 1; }
                }
            }
        }

        if (needs_reload) {
            needs_reload = 0;
            MoonPiv *npiv = moon_testmap_load_piv_from_buffer(
                                g_buf, g_buf_size, cur);
            if (npiv) {
                print_info(npiv, cur, filename);

                SDL_DestroyTexture(tex);
                SDL_DestroyRenderer(ren);
                SDL_DestroyWindow(win);

                int nw = npiv->width;
                int nh = npiv->height;

                snprintf(title, sizeof(title),
                         "moon-view-test: %s  [%d/%d]  SPACE/→ next  ← prev  ESC quit",
                         filename, cur, total_images - 1);

                win = SDL_CreateWindow(title,
                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                          nw, nh, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
                ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
                tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_ARGB8888,
                                        SDL_TEXTUREACCESS_STATIC, nw, nh);

                uint32_t *px = piv_to_pixels(npiv);
                if (px) { SDL_UpdateTexture(tex, NULL, px, nw * 4); free(px); }
                moon_piv_free(npiv);
            }
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
#endif /* HAVE_SDL2 */

/* ------------------------------------------------------------------ */
/* main                                                                 */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr,
                "Usage: moon-view-test <Test_file>\n"
                "\n"
                "Displays all PIV images packed inside the Moonstone 'Test' container.\n"
                "Image 8 = overworld map background (320x200, 5 bitplanes).\n"
                "\n"
                "Keys (SDL window):\n"
                "  SPACE / RIGHT  — next image\n"
                "  LEFT           — previous image\n"
                "  0..9           — jump to image N\n"
                "  ESC / Q        — quit\n");
        return 1;
    }

    const char *path = argv[1];

    if (!load_container(path)) return 1;

    /* Count how many images the container holds */
    int total = 0;
    for (;;) {
        MoonPiv *p = moon_testmap_load_piv_from_buffer(g_buf, g_buf_size, total);
        if (!p) break;
        moon_piv_free(p);
        total++;
    }

    if (total == 0) {
        fprintf(stderr, "Error: no valid PIV images found in '%s'\n", path);
        free(g_buf);
        return 1;
    }

    printf("'%s': %d image(s) found\n\n", path, total);

    /* Print info for all images */
    for (int i = 0; i < total; i++) {
        MoonPiv *p = moon_testmap_load_piv_from_buffer(g_buf, g_buf_size, i);
        if (p) { print_info(p, i, path); moon_piv_free(p); }
    }

#ifdef HAVE_SDL2
    run_sdl(path, total);
#endif

    free(g_buf);
    return 0;
}
