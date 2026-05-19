/*
 * moon-view-cel — display all frames from a Moonstone CEL sprite file as an atlas.
 *
 * Usage: moon-view-cel <file.cel>
 *
 * With SDL2: opens a window showing all frames laid out in a grid.
 * Without SDL2: prints an ASCII-art representation of every frame to stdout.
 *
 * The CEL pixel data is planar (Amiga bitplane format). Each pixel is
 * reconstructed from all bitplanes and mapped through a default 16-colour
 * palette (EGA-like Amiga OCS fallback when no PIV palette is available).
 */

#include "moon_assets.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

/* Extract pixel colour index from a single frame's planar data.
 *
 * CEL pixel data is plane-sequential (matching LAB_04A8 / LAB_04AC in
 * program.asm where each active plane's rows are read consecutively):
 *   plane 0: rows 0..height-1
 *   plane 1: rows 0..height-1
 *   …
 * Within each plane the MSB of each byte is the left-most pixel.
 */
static int get_pixel(const MoonCelFrame *fr, int x, int y)
{
    if (x < 0 || x >= (int)fr->width || y < 0 || y >= (int)fr->height)
        return 0;
    int row_bytes = (((int)fr->width + 15) / 16) * 2;
    int pixel_idx = 0;
    for (int pl = 0; pl < (int)fr->planes; pl++) {
        size_t byte_off = (size_t)pl * (size_t)fr->height * (size_t)row_bytes
                        + (size_t)y  * (size_t)row_bytes
                        + (size_t)(x / 8);
        int bit = (fr->data[byte_off] >> (7 - (x % 8))) & 1;
        pixel_idx |= (bit << pl);
    }
    return pixel_idx;
}

/* ------------------------------------------------------------------ */
/* Atlas layout helpers                                                */
/* ------------------------------------------------------------------ */

/*
 * Choose a grid of `cols` × `rows` cells that packs all `n` frames with
 * a roughly square layout.
 */
static void atlas_grid(int n, int *out_cols, int *out_rows)
{
    int cols = (int)ceil(sqrt((double)n));
    if (cols < 1) cols = 1;
    int rows = (n + cols - 1) / cols;
    *out_cols = cols;
    *out_rows = rows;
}

/*
 * Pick the largest integer scale such that (w*scale <= max_w) and
 * (h*scale <= max_h).  Returns at least 1.
 */
static int best_scale(int w, int h, int max_w, int max_h)
{
    int s = 1;
    while ((w * (s + 1)) <= max_w && (h * (s + 1)) <= max_h)
        s++;
    return s;
}

/* ------------------------------------------------------------------ */
/* SDL2 atlas renderer                                                 */
/* ------------------------------------------------------------------ */

#ifdef HAVE_SDL2
static void show_sdl_atlas(const MoonCel *cel, const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    int n = cel->frame_count;
    int cols, rows;
    atlas_grid(n, &cols, &rows);

    /* Maximum frame dimensions determine cell size */
    int max_w = 1, max_h = 1;
    for (int i = 0; i < n; i++) {
        if ((int)cel->frames[i].width  > max_w) max_w = cel->frames[i].width;
        if ((int)cel->frames[i].height > max_h) max_h = cel->frames[i].height;
    }

    int pad      = 2;               /* pixels between frames */
    int cell_w   = max_w + pad;
    int cell_h   = max_h + pad;
    int atlas_w  = cols * cell_w + pad;
    int atlas_h  = rows * cell_h + pad;

    /* Fit in a 1280×800 desktop window using integer scaling */
    int scale = best_scale(atlas_w, atlas_h, 1280, 800);

    /* Build the atlas pixel buffer (dark background) */
    uint32_t *pixels = (uint32_t *)calloc((size_t)(atlas_w * atlas_h), 4);
    if (!pixels) {
        SDL_Quit();
        return;
    }
    for (int i = 0; i < atlas_w * atlas_h; i++)
        pixels[i] = 0x222222;

    for (int fi = 0; fi < n; fi++) {
        int col = fi % cols;
        int row = fi / cols;
        int ox  = pad + col * cell_w;
        int oy  = pad + row * cell_h;

        const MoonCelFrame *fr = &cel->frames[fi];
        if (!fr->data) continue;

        for (int y = 0; y < (int)fr->height; y++) {
            for (int x = 0; x < (int)fr->width; x++) {
                int idx = get_pixel(fr, x, y);
                pixels[(oy + y) * atlas_w + (ox + x)] = default_pal16[idx & 15];
            }
        }
    }

    SDL_Window   *win = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                            atlas_w * scale, atlas_h * scale,
                            SDL_WINDOW_SHOWN);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,
                            SDL_PIXELFORMAT_RGB888,
                            SDL_TEXTUREACCESS_STATIC,
                            atlas_w, atlas_h);
    SDL_UpdateTexture(tex, NULL, pixels, atlas_w * 4);
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
#endif /* HAVE_SDL2 */

/* ------------------------------------------------------------------ */
/* ASCII-art fallback (no SDL2)                                        */
/* ------------------------------------------------------------------ */

#ifndef HAVE_SDL2
static void show_ascii_frame(const MoonCelFrame *fr, int index)
{
    static const char shades[] = " .-=+*#%@";
    int n_shades = (int)(sizeof(shades) - 1);
    int max_idx  = (1 << fr->planes) - 1;
    if (max_idx < 1) max_idx = 1;

    printf("--- frame %d  (%dx%d, %d planes) ---\n",
           index, fr->width, fr->height, fr->planes);
    for (int y = 0; y < (int)fr->height; y++) {
        for (int x = 0; x < (int)fr->width; x++) {
            int idx   = get_pixel(fr, x, y);
            int shade = (idx * (n_shades - 1)) / max_idx;
            if (shade < 0) shade = 0;
            if (shade >= n_shades) shade = n_shades - 1;
            putchar(shades[shade]);
        }
        putchar('\n');
    }
}
#endif /* !HAVE_SDL2 */

/* ------------------------------------------------------------------ */
/* Entry point                                                         */
/* ------------------------------------------------------------------ */

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-view-cel <file.cel>\n");
        return 1;
    }

    const char *path = argv[1];

    /* Split path into directory and filename so moon_init can locate the file */
    char asset_dir[512]  = ".";
    char asset_file[256] = "";
    {
        const char *slash  = strrchr(path, '/');
        const char *bslash = strrchr(path, '\\');
        const char *sep    = (slash > bslash) ? slash : bslash;
        if (sep) {
            int dir_len = (int)(sep - path);
            if (dir_len > (int)sizeof(asset_dir) - 1)
                dir_len = (int)sizeof(asset_dir) - 1;
            strncpy(asset_dir, path, (size_t)dir_len);
            asset_dir[dir_len] = '\0';
            strncpy(asset_file, sep + 1, sizeof(asset_file) - 1);
        } else {
            strncpy(asset_file, path, sizeof(asset_file) - 1);
        }
        asset_file[sizeof(asset_file) - 1] = '\0';
    }

    moon_init(asset_dir);
    MoonCel *cel = moon_cel_load(asset_file);

    if (!cel || cel->frame_count == 0) {
        fprintf(stderr, "Error: failed to decode CEL file '%s'\n", path);
        moon_shutdown();
        return 1;
    }

    printf("File   : %s\n", path);
    printf("Frames : %d\n", cel->frame_count);
    for (int i = 0; i < cel->frame_count; i++) {
        const MoonCelFrame *fr = &cel->frames[i];
        printf("  [%3d] %3dx%-3d  planes=%d\n",
               i, fr->width, fr->height, fr->planes);
    }

#ifdef HAVE_SDL2
    {
        char title[256];
        snprintf(title, sizeof(title), "moon-view-cel: %s  (%d frames)",
                 path, cel->frame_count);
        show_sdl_atlas(cel, title);
    }
#else
    for (int i = 0; i < cel->frame_count; i++)
        show_ascii_frame(&cel->frames[i], i);
#endif

    moon_cel_free(cel);
    moon_shutdown();
    return 0;
}
