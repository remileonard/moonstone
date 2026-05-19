/*
 * moon-view-stile — display all tiles from a Moonstone .stile file as an atlas.
 *
 * Usage: moon-view-stile <file.stile>
 *
 * With SDL2: opens a window showing every tile laid out in a grid atlas.
 * Without SDL2: prints an ASCII-art representation of every tile to stdout.
 *
 * Tile geometry assumed:
 *   16×16 pixels, plane-sequential within each tile.
 *   4 planes → 128 bytes / tile  (detected when decompressed_size % 128 == 0)
 *   5 planes → 160 bytes / tile  (detected when decompressed_size % 160 == 0)
 *   Fallback : 1 plane, treat the whole buffer as a single tile.
 *
 * Pixels are mapped through a default 16/32-colour Amiga-style palette.
 */

#include "moon_assets.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_SDL2
#include <SDL2/SDL.h>
#endif

/* Default 32-colour Amiga-style palette */
static const uint32_t default_pal32[32] = {
    0x000000, 0xAAAAAA, 0x000088, 0x0000FF,
    0x008800, 0x00FF00, 0x008888, 0x00FFFF,
    0x880000, 0xFF0000, 0x880088, 0xFF00FF,
    0x888800, 0xFFFF00, 0x888888, 0xFFFFFF,
    0x444444, 0xCCCCCC, 0x4444AA, 0x4444FF,
    0x44AA44, 0x44FF44, 0x44AAAA, 0x44FFFF,
    0xAA4444, 0xFF4444, 0xAA44AA, 0xFF44FF,
    0xAAAA44, 0xFFFF44, 0xAAAAAA, 0xFFFFFF,
};

/* ------------------------------------------------------------------ */
/* Tile pixel extractor                                                */
/* ------------------------------------------------------------------ */

#define TILE_W 16
#define TILE_H 16

/*
 * Extract the colour index of pixel (x, y) from tile number `tile_idx`.
 *
 * Within each tile the data is plane-sequential:
 *   [plane 0 rows 0..15][plane 1 rows 0..15]…
 * row_bytes = TILE_W / 8 = 2.
 */
static int get_tile_pixel(const uint8_t *data, int total_bytes,
                          int tile_bytes, int planes,
                          int tile_idx, int x, int y)
{
    int row_bytes       = TILE_W / 8;                /* 2 bytes per row per plane */
    int tile_plane_bytes = row_bytes * TILE_H;        /* 32 bytes per plane */
    int tile_off        = tile_idx * tile_bytes;

    if (tile_off + tile_bytes > total_bytes)
        return 0;

    int pixel = 0;
    for (int pl = 0; pl < planes; pl++) {
        int off = tile_off + pl * tile_plane_bytes + y * row_bytes + x / 8;
        int bit = (data[off] >> (7 - (x % 8))) & 1;
        pixel |= (bit << pl);
    }
    return pixel;
}

/* ------------------------------------------------------------------ */
/* Atlas layout helpers (same as in moon_view_cel.c)                   */
/* ------------------------------------------------------------------ */

static void atlas_grid(int n, int *out_cols, int *out_rows)
{
    int cols = (int)ceil(sqrt((double)n));
    if (cols < 1) cols = 1;
    *out_cols = cols;
    *out_rows = (n + cols - 1) / cols;
}

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
static void show_sdl_atlas(const uint8_t *data, int total_bytes,
                           int tile_count, int tile_bytes, int planes,
                           const char *title)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "SDL_Init error: %s\n", SDL_GetError());
        return;
    }

    int cols, rows;
    atlas_grid(tile_count, &cols, &rows);

    int pad     = 2;
    int cell_w  = TILE_W + pad;
    int cell_h  = TILE_H + pad;
    int atlas_w = cols * cell_w + pad;
    int atlas_h = rows * cell_h + pad;

    int scale = best_scale(atlas_w, atlas_h, 1280, 800);

    uint32_t *pixels = (uint32_t *)calloc((size_t)(atlas_w * atlas_h), 4);
    if (!pixels) { SDL_Quit(); return; }
    for (int i = 0; i < atlas_w * atlas_h; i++)
        pixels[i] = 0x222222;

    int pal_size = 1 << planes;
    if (pal_size > 32) pal_size = 32;

    for (int ti = 0; ti < tile_count; ti++) {
        int col = ti % cols;
        int row = ti / cols;
        int ox  = pad + col * cell_w;
        int oy  = pad + row * cell_h;

        for (int y = 0; y < TILE_H; y++) {
            for (int x = 0; x < TILE_W; x++) {
                int idx = get_tile_pixel(data, total_bytes,
                                         tile_bytes, planes,
                                         ti, x, y);
                pixels[(oy + y) * atlas_w + (ox + x)] =
                    default_pal32[idx % pal_size];
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
static void show_ascii_tile(const uint8_t *data, int total_bytes,
                            int tile_bytes, int planes, int tile_idx)
{
    static const char shades[] = " .-=+*#%@";
    int n_shades = (int)(sizeof(shades) - 1);
    int max_idx  = (1 << planes) - 1;
    if (max_idx < 1) max_idx = 1;

    printf("--- tile %d ---\n", tile_idx);
    for (int y = 0; y < TILE_H; y++) {
        for (int x = 0; x < TILE_W; x++) {
            int idx   = get_tile_pixel(data, total_bytes, tile_bytes, planes,
                                       tile_idx, x, y);
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
        fprintf(stderr, "Usage: moon-view-stile <file.stile>\n");
        return 1;
    }

    const char *path = argv[1];

    /* Split into directory and filename for moon_init */
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
    MoonStile *stile = moon_stile_load(asset_file);
    moon_shutdown();

    if (!stile || stile->size == 0) {
        fprintf(stderr, "Error: failed to decode stile file '%s'\n", path);
        if (stile) moon_stile_free(stile);
        return 1;
    }

    int written = (int)stile->size;

    printf("File         : %s\n", path);
    printf("Decompressed : %d bytes\n", written);

    /* Auto-detect tile geometry */
    int planes     = 0;
    int tile_bytes = 0;

    if (written % 160 == 0) {
        planes     = 5;
        tile_bytes = 160;   /* 16×16 × 5 planes */
    } else if (written % 128 == 0) {
        planes     = 4;
        tile_bytes = 128;   /* 16×16 × 4 planes */
    } else {
        /* Fallback: treat the whole buffer as a single raw tile, 1 plane */
        planes     = 1;
        tile_bytes = written > 0 ? written : 1;
    }

    int tile_count = written / tile_bytes;

    printf("Tile format  : %dx%d × %d planes = %d bytes/tile\n",
           TILE_W, TILE_H, planes, tile_bytes);
    printf("Tile count   : %d\n", tile_count);

#ifdef HAVE_SDL2
    {
        char title[256];
        snprintf(title, sizeof(title),
                 "moon-view-stile: %s  (%d tiles, %dx%d, %d planes)",
                 path, tile_count, TILE_W, TILE_H, planes);
        show_sdl_atlas(stile->data, written, tile_count, tile_bytes, planes, title);
    }
#else
    for (int ti = 0; ti < tile_count; ti++)
        show_ascii_tile(stile->data, written, tile_bytes, planes, ti);
#endif

    moon_stile_free(stile);
    return 0;
}
