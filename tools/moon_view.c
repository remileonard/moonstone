/*
 * moon-view — show a Moonstone picture or sprite sheet (SDL2).
 *
 * Usage: moon-view <file> [--pal <file.piv>] [--index N] [--bmp <out.bmp>]
 *
 *   CEL (.cel .ob .c .f .font)  every frame, as an atlas; colours from the
 *                               palette of --pal (else a default one),
 *                               colour 0 (transparent) in dark grey
 *   PIV (.piv .p mindscape)     the picture with its own palette
 *   test                        its pictures: LEFT / RIGHT, from --index
 *
 * --bmp writes the image to a BMP file instead of opening a window.
 * Keys: LEFT / RIGHT (test), ESC / Q quit.
 */
#include "moon_tool.h"
#include "moon_testmap.h"

#include <SDL2/SDL.h>
#include <stdlib.h>

/* Default sprite palette when no --pal is given */
static const uint16_t default_pal[32] = {
    0x000, 0xAAA, 0x008, 0x00F, 0x080, 0x0F0, 0x088, 0x0FF,
    0x800, 0xF00, 0x808, 0xF0F, 0x880, 0xFF0, 0x888, 0xFFF,
    0x444, 0x666, 0x048, 0x48C, 0x484, 0x8C8, 0x468, 0x8CC,
    0x642, 0xC84, 0x646, 0xC8C, 0x864, 0xCA6, 0x999, 0xDDD,
};

static SDL_Surface *new_surface(int w, int h)
{
    return SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
}

static SDL_Surface *render_piv(const MoonPiv *piv)
{
    SDL_Surface *s = new_surface(piv->width, piv->height);
    if (!s)
        return NULL;
    for (int y = 0; y < piv->height; y++) {
        uint32_t *row = (uint32_t *)((uint8_t *)s->pixels + y * s->pitch);
        for (int x = 0; x < piv->width; x++)
            row[x] = 0xFF000000u | tool_rgb(piv->palette[tool_piv_pixel(piv, x, y)]);
    }
    return s;
}

static SDL_Surface *render_cel(const MoonCel *cel, const uint16_t *pal)
{
    int n = cel->frame_count, cw = 1, ch = 1, pad = 2;
    for (int i = 0; i < n; i++) {
        if (cel->frames[i].width > cw) cw = cel->frames[i].width;
        if (cel->frames[i].height > ch) ch = cel->frames[i].height;
    }
    int cols = 1;
    while (cols * cols < n)
        cols++;
    int rows = (n + cols - 1) / cols;
    cw += pad, ch += pad;
    SDL_Surface *s = new_surface(cols * cw + pad, rows * ch + pad);
    if (!s)
        return NULL;
    SDL_FillRect(s, NULL, 0xFF101018u);
    for (int i = 0; i < n; i++) {
        const MoonCelFrame *f = &cel->frames[i];
        int ox = pad + i % cols * cw, oy = pad + i / cols * ch;
        for (int y = 0; y < f->height; y++) {
            uint32_t *row = (uint32_t *)((uint8_t *)s->pixels + (oy + y) * s->pitch) + ox;
            for (int x = 0; x < f->width; x++) {
                int c = f->data ? tool_cel_pixel(f, x, y) : 0;
                row[x] = c ? 0xFF000000u | tool_rgb(pal[c]) : 0xFF303030u;
            }
        }
    }
    return s;
}

/* Picture to show: `index` for the test container. */
static SDL_Surface *render(const char *name, MoonFileKind kind, const uint16_t *pal, int index)
{
    SDL_Surface *s = NULL;
    if (kind == MOON_KIND_CEL) {
        MoonCel *cel = moon_cel_load(name);
        if (cel && cel->frame_count)
            s = render_cel(cel, pal);
        moon_cel_free(cel);
    } else {
        MoonPiv *piv = kind == MOON_KIND_TESTMAP ? moon_testmap_load_piv(name, index)
                                                : moon_piv_load(name);
        if (piv)
            s = render_piv(piv);
        moon_piv_free(piv);
    }
    return s;
}

static void show(SDL_Surface *s, const char *title, SDL_Window **win, SDL_Renderer **ren)
{
    int scale = 1;
    while (s->w * (scale + 1) <= 1280 && s->h * (scale + 1) <= 800)
        scale++;
    if (!*win) {
        *win = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                s->w * scale, s->h * scale, 0);
        *ren = SDL_CreateRenderer(*win, -1, 0);
    } else {
        SDL_SetWindowTitle(*win, title);
        SDL_SetWindowSize(*win, s->w * scale, s->h * scale);
    }
    SDL_Texture *tex = SDL_CreateTextureFromSurface(*ren, s);
    SDL_RenderClear(*ren);
    SDL_RenderCopy(*ren, tex, NULL, NULL);
    SDL_RenderPresent(*ren);
    SDL_DestroyTexture(tex);
}

int main(int argc, char *argv[])
{
    const char *path = NULL, *pal_path = NULL, *bmp = NULL;
    int index = 0;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--pal") && i + 1 < argc)
            pal_path = argv[++i];
        else if (!strcmp(argv[i], "--bmp") && i + 1 < argc)
            bmp = argv[++i];
        else if (!strcmp(argv[i], "--index") && i + 1 < argc)
            index = atoi(argv[++i]);
        else
            path = argv[i];
    }
    if (!path) {
        fprintf(stderr, "Usage: moon-view <file> [--pal <file.piv>] [--index N] [--bmp <out.bmp>]\n");
        return 1;
    }

    uint16_t pal[32];
    memcpy(pal, default_pal, sizeof pal);
    char name[256];
    if (pal_path) {
        tool_open(pal_path, name, sizeof name);
        MoonPiv *piv = moon_piv_load(name);
        if (!piv) {
            fprintf(stderr, "Error: cannot read the palette of '%s'\n", pal_path);
            return 1;
        }
        memcpy(pal, piv->palette, sizeof pal);
        moon_piv_free(piv);
    }

    tool_open(path, name, sizeof name);
    MoonFileKind kind = moon_file_kind(name);
    if (kind != MOON_KIND_CEL && kind != MOON_KIND_PIV && kind != MOON_KIND_TESTMAP) {
        fprintf(stderr, "Error: '%s' is not a picture (%s)\n", path, tool_kind_name(kind));
        return 1;
    }
    SDL_Surface *s = render(name, kind, pal, index);
    if (!s) {
        fprintf(stderr, "Error: '%s' not decoded\n", path);
        return 1;
    }
    if (bmp) {
        int r = SDL_SaveBMP(s, bmp);
        SDL_FreeSurface(s);
        if (r)
            fprintf(stderr, "Error: %s\n", SDL_GetError());
        return r ? 1 : 0;
    }

    if (SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL error: %s\n", SDL_GetError());
        return 1;
    }
    SDL_Window *win = NULL;
    SDL_Renderer *ren = NULL;
    char title[320];
    for (int running = 1; running;) {
        if (s) {
            if (kind == MOON_KIND_TESTMAP)
                snprintf(title, sizeof title, "moon-view: %s [%d]", path, index);
            else
                snprintf(title, sizeof title, "moon-view: %s", path);
            show(s, title, &win, &ren);
            SDL_FreeSurface(s);
            s = NULL;
        }
        SDL_Event ev;
        if (!SDL_WaitEvent(&ev))
            break;
        if (ev.type == SDL_QUIT)
            running = 0;
        else if (ev.type == SDL_WINDOWEVENT && ev.window.event == SDL_WINDOWEVENT_EXPOSED)
            s = render(name, kind, pal, index);
        else if (ev.type == SDL_KEYDOWN) {
            SDL_Keycode k = ev.key.keysym.sym;
            if (k == SDLK_ESCAPE || k == SDLK_q)
                running = 0;
            else if (kind == MOON_KIND_TESTMAP && (k == SDLK_RIGHT || k == SDLK_LEFT)) {
                int next = index + (k == SDLK_RIGHT ? 1 : -1);
                SDL_Surface *t = next >= 0 ? render(name, kind, pal, next) : NULL;
                if (t)
                    s = t, index = next;
            }
        }
    }
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
