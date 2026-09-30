/*
 * piv.c — PIV background bitmap loader for libmoon_assets.
 *
 * File layout (all big-endian), as read by LAB_0402 in program.asm and
 * LAB_0C27 in mog.asm (the .PIV and .p pictures, and each image of the
 * "Test" container):
 *   offset 0 : word  = plane count (4 → 16 colours, 5 → 32 colours)
 *   offset 2 : long  = compressed body size (bytes)
 *   offset 6 : palette — 16 or 32 words; a word with bit 15 set is used
 *              as it is (bit 15 cleared), any other is shifted left once
 *              (LAB_03F4 / LAB_03FF: BCLR #15,D0; BNE; LSL.W #1,D0)
 *   then     : LZSS body (LAB_049C / LAB_0CC2) giving the bitplanes of a
 *              320×200 image one after the other (8000 bytes each).
 */

#include "moon_private.h"

#include <stdlib.h>
#include <string.h>

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |  (uint32_t)p[3];
}

static uint16_t be16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

MoonPiv *moon_piv_load_from_buffer(const uint8_t *buf, size_t len)
{
    if (!buf || len < 6)
        return NULL;
    int planes = (int)be16(buf);
    if (planes != 4 && planes != 5)
        return NULL;

    uint32_t body_size = be32(buf + 2);
    int pal_count = planes == 4 ? 16 : 32;
    size_t body_at = 6 + (size_t)pal_count * 2;
    if (body_at > len || body_size > len - body_at)
        return NULL;

    MoonPiv *piv = (MoonPiv *)calloc(1, sizeof(MoonPiv));
    if (!piv)
        return NULL;
    piv->planes = planes;
    piv->width  = 320;
    piv->height = 200;
    for (int i = 0; i < pal_count; i++) {
        uint16_t raw = be16(buf + 6 + i * 2);
        piv->palette[i] = raw & 0x8000u ? (uint16_t)(raw & 0x7FFFu)
                                        : (uint16_t)(raw << 1);
    }

    size_t bitmap_size = (size_t)planes * 8000;
    piv->bitmap = (uint8_t *)calloc(1, bitmap_size);
    if (!piv->bitmap) {
        free(piv);
        return NULL;
    }
    if (moon_lzss_decompress(buf + body_at, body_size, piv->bitmap, bitmap_size) <= 0) {
        moon_piv_free(piv);
        return NULL;
    }
    return piv;
}

MoonPiv *moon_piv_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonPiv *piv = moon_piv_load_from_buffer(buf, len);
    free(buf);
    return piv;
}

void moon_piv_free(MoonPiv *piv)
{
    if (!piv)
        return;
    free(piv->bitmap);
    free(piv);
}
