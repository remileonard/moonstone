/*
 * moon_tool.h — helpers shared by the command-line tools (moon-info,
 * moon-dump, moon-view).  Every file is read through libmoon_assets.
 */
#ifndef MOON_TOOL_H
#define MOON_TOOL_H

#include "moon_assets.h"

#include <stdio.h>
#include <string.h>

/* moon_init() on the directory of `path`; its file name goes to `name`. */
static inline int tool_open(const char *path, char *name, size_t n)
{
    char dir[512] = ".";
    const char *base = path;
    for (const char *p = path; *p; p++)
        if (*p == '/' || *p == '\\')
            base = p + 1;
    if (base != path) {
        size_t len = (size_t)(base - path - 1);
        if (len >= sizeof dir)
            len = sizeof dir - 1;
        memcpy(dir, path, len);
        dir[len] = '\0';
        if (!len)
            strcpy(dir, "/");
    }
    snprintf(name, n, "%s", base);
    return moon_init(dir);
}

static inline const char *tool_kind_name(MoonFileKind k)
{
    switch (k) {
    case MOON_KIND_CEL:     return "CEL sprite sheet";
    case MOON_KIND_PIV:     return "PIV picture";
    case MOON_KIND_MOD:     return "module (RNC-packed)";
    case MOON_KIND_STILE:   return "tile map (stile)";
    case MOON_KIND_SFX:     return "sound bank";
    case MOON_KIND_TERRAIN: return "terrain";
    case MOON_KIND_TESTMAP: return "overworld pictures (test)";
    case MOON_KIND_HIT:     return "hit boxes (collide.hit)";
    default:                return "unknown";
    }
}

/* Amiga $0RGB -> 0xRRGGBB */
static inline uint32_t tool_rgb(uint16_t c)
{
    uint32_t r = (c >> 8) & 15, g = (c >> 4) & 15, b = c & 15;
    return (r * 17) << 16 | (g * 17) << 8 | b * 17;
}

/* Colour index of a CEL pixel.  Only the planes of the mask are stored,
 * one after the other: the k-th stored plane is bit k of the mask's k-th
 * set bit (as the blitter of LAB_0CDA draws them). */
static inline int tool_cel_pixel(const MoonCelFrame *fr, int x, int y)
{
    int row_bytes = (fr->width + 15) / 16 * 2, idx = 0, k = 0;
    for (int b = 0; b < 5; b++) {
        if (!(fr->plane_mask & (1u << b)))
            continue;
        size_t off = ((size_t)k++ * fr->height + (size_t)y) * (size_t)row_bytes + (size_t)x / 8;
        idx |= ((fr->data[off] >> (7 - x % 8)) & 1) << b;
    }
    return idx;
}

/* Colour index of a PIV pixel (planes one after the other). */
static inline int tool_piv_pixel(const MoonPiv *piv, int x, int y)
{
    int row_bytes = (piv->width + 15) / 16 * 2, idx = 0;
    for (int pl = 0; pl < piv->planes; pl++) {
        size_t off = ((size_t)pl * piv->height + (size_t)y) * (size_t)row_bytes + (size_t)x / 8;
        idx |= ((piv->bitmap[off] >> (7 - x % 8)) & 1) << pl;
    }
    return idx;
}

#endif /* MOON_TOOL_H */
