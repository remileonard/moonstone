/*
 * moon_render.h — Rendering helpers for the Moonstone port
 *
 * Converts Amiga planar pixel data (CEL, PIV, STILE) to ARGB8888 pixels
 * and provides sprite-blit, background-fill, and text-rendering routines
 * that write into a 320×200 ARGB8888 framebuffer.
 */

#ifndef MOON_RENDER_H
#define MOON_RENDER_H

#include "moon_assets.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Screen dimensions (Amiga OCS PAL low-res)                          */
/* ------------------------------------------------------------------ */

#define SCREEN_W  320
#define SCREEN_H  200

/* Maximum palette entries (5 bitplanes → 32 colours) */
#define MAX_PALETTE 32

/* ------------------------------------------------------------------ */
/* Palette helpers                                                     */
/* ------------------------------------------------------------------ */

/**
 * render_amiga_color — convert a 12-bit Amiga $0RGB colour word to
 * a 32-bit ARGB8888 value (alpha = 0xFF, fully opaque).
 */
uint32_t render_amiga_color(uint16_t amiga_col);

/**
 * render_build_palette — expand an array of Amiga 12-bit words to
 * ARGB8888 and store in @out[0..n-1].
 * @amiga_pal : Amiga 12-bit palette words ($0RGB).
 * @n         : number of entries to convert.
 * @out       : destination array (caller-allocated, at least n × 4 bytes).
 */
void render_build_palette(const uint16_t *amiga_pal, int n, uint32_t *out);

/**
 * render_copy_palette — copy palette from @src to @dst (n entries).
 */
void render_copy_palette(const uint32_t *src, uint32_t *dst, int n);

/**
 * render_blend_palette — linearly blend @a and @b into @dst.
 * @t : blend factor 0..256 (0 = full @a, 256 = full @b).
 */
void render_blend_palette(const uint32_t *a, const uint32_t *b,
                          uint32_t *dst, int n, int t);

/* ------------------------------------------------------------------ */
/* Framebuffer utilities                                               */
/* ------------------------------------------------------------------ */

/**
 * render_clear — fill the whole 320×200 framebuffer with @color (ARGB8888).
 */
void render_clear(uint32_t *fb, uint32_t color);

/**
 * render_fill_rect — fill a rectangle in the framebuffer.
 */
void render_fill_rect(uint32_t *fb, int x, int y, int w, int h, uint32_t color);

/* ------------------------------------------------------------------ */
/* PIV background                                                      */
/* ------------------------------------------------------------------ */

/**
 * render_piv — decode a PIV background into the framebuffer.
 *
 * The PIV bitmap is plane-sequential (5 or 4 bitplanes); each pixel
 * index is looked up in the PIV palette.  The result is written into
 * @fb at (dst_x, dst_y), clipped to the screen boundaries.
 *
 * @piv   : decoded PIV asset.
 * @fb    : destination ARGB8888 framebuffer (SCREEN_W × SCREEN_H).
 * @dst_x : left edge of destination rectangle.
 * @dst_y : top edge of destination rectangle.
 * @src_x, src_y : top-left pixel within the PIV to start copying from.
 * @copy_w, copy_h : region size (use piv->width / piv->height to copy all).
 */
void render_piv(const MoonPiv *piv,
                uint32_t *fb,
                int dst_x, int dst_y,
                int src_x, int src_y,
                int copy_w, int copy_h);

/**
 * render_piv_full — convenience wrapper: copy the entire PIV at (0,0).
 */
void render_piv_full(const MoonPiv *piv, uint32_t *fb);

/* ------------------------------------------------------------------ */
/* CEL / OB sprite blit                                                */
/* ------------------------------------------------------------------ */

/* Blit flags — match the CEL draw_flags field from the asset header */
#define BLIT_FLIP_X   0x01   /* mirror horizontally                  */
#define BLIT_FLIP_Y   0x02   /* mirror vertically                    */
#define BLIT_MASK     0x04   /* index 0 is transparent (colour key)  */
#define BLIT_ADDITIVE 0x08   /* additive blend (flash effect)        */

/**
 * render_cel_frame — blit a single CEL or OB frame into the framebuffer.
 *
 * @frame   : single animation frame (planar data).
 * @palette : ARGB8888 palette used to map pixel indices.
 * @fb      : destination framebuffer.
 * @dst_x   : left pixel position (may be negative → clipped).
 * @dst_y   : top pixel position (may be negative → clipped).
 * @flags   : combination of BLIT_* flags.
 */
void render_cel_frame(const MoonCelFrame *frame,
                      const uint32_t     *palette,
                      uint32_t           *fb,
                      int dst_x, int dst_y,
                      int flags);

/**
 * render_cel — convenience wrapper: pick frame @frame_idx from a MoonCel
 * and blit it using render_cel_frame().
 *
 * @cel       : loaded CEL (or OB) sprite sheet; if NULL the call is a no-op.
 * @frame_idx : animation frame to render (clamped to [0, frame_count-1]).
 * @palette   : ARGB8888 palette (32 entries).
 * @fb        : destination framebuffer.
 * @dst_x     : left pixel position.
 * @dst_y     : top pixel position.
 * @flags     : combination of BLIT_* flags.
 */
void render_cel(const MoonCel  *cel,
                int             frame_idx,
                const uint32_t *palette,
                uint32_t       *fb,
                int dst_x, int dst_y,
                int flags);

/* ------------------------------------------------------------------ */
/* Text rendering (built-in 8×8 font fallback)                        */
/* ------------------------------------------------------------------ */

/**
 * render_text — draw an ASCII string using the built-in 8×8 bitmap font.
 *
 * @fb    : destination framebuffer.
 * @text  : null-terminated ASCII string.
 * @x, y  : top-left pixel of the first character.
 * @color : ARGB8888 foreground colour; background is always transparent.
 */
void render_text(uint32_t *fb, const char *text, int x, int y, uint32_t color);

/**
 * render_text_centered — draw text horizontally centred at x = SCREEN_W/2.
 */
void render_text_centered(uint32_t *fb, const char *text, int y, uint32_t color);

/* ------------------------------------------------------------------ */
/* Scroll helper                                                       */
/* ------------------------------------------------------------------ */

/**
 * render_scroll_vertical — compose a vertically-scrolled view from
 * three stacked 320×200 backgrounds.
 *
 * The three buffers are treated as a single 320×600 vertical strip
 * (buf0 at top, buf1 in the middle, buf2 at bottom). @scroll_y selects
 * which 320×200 slice of that strip is copied into @fb.
 *
 * @buf0, buf1, buf2 : ARGB8888 framebuffers, each 320×200.
 * @fb               : destination 320×200 framebuffer.
 * @scroll_y         : vertical scroll position (0..399).
 */
void render_scroll_vertical(const uint32_t *buf0,
                            const uint32_t *buf1,
                            const uint32_t *buf2,
                            uint32_t *fb,
                            int scroll_y);

#ifdef __cplusplus
}
#endif

#endif /* MOON_RENDER_H */
