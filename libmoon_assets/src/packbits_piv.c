/*
 * packbits_piv.c — PIV background decoder (PackBits + IFF/ILBM or custom)
 *
 * Moonstone background images (.PIV / .piv) are stored as IFF/ILBM files
 * with BODY chunks compressed using the PackBits algorithm (also known as
 * ByteRun1 in IFF terminology).
 *
 * This file provides:
 *   1. moon_packbits_decompress() — raw PackBits decompressor.
 *   2. moon_piv_load()           — full PIV file parser (also handles
 *                                  the custom Mindscape-header variant).
 *
 * IFF/ILBM structure (standard):
 *   "FORM" + 4-byte-size + "ILBM"
 *   Chunks: "BMHD" (header), "CMAP" (palette), "BODY" (bitmap body)
 *
 * Custom PIV variant (observed magic 0x0005 or 0x0004):
 *   word[0]   = plane count (4 or 5)
 *   word[1]   = flags (ignored)
 *   32 × word = palette (Amiga 12-bit, $0RGB each)
 *   body      = PackBits-compressed plane-interleaved bitmap
 *
 * PackBits algorithm:
 *   byte >= 0x00 : copy the next (byte+1) literal bytes.
 *   byte == 0x80 : NOP (skip).
 *   byte  < 0x00 : repeat the next byte (1 - byte) times.
 *
 * Reference: LAB_0434 / LAB_043A-LAB_0440 in program.asm.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* PackBits decompressor                                               */
/* ------------------------------------------------------------------ */

int moon_packbits_decompress(const uint8_t *src, size_t src_len,
                             uint8_t *dst, size_t dst_len)
{
    const uint8_t *p   = src;
    const uint8_t *end = src + src_len;
    uint8_t       *out = dst;
    uint8_t       *out_end = dst + dst_len;

    while (p < end && out < out_end) {
        int8_t ctrl = (int8_t)*p++;

        if (ctrl == -128) {
            /* NOP — skip */
            continue;
        }

        if (ctrl >= 0) {
            /* Literal run: copy (ctrl+1) bytes verbatim */
            int n = ctrl + 1;
            if (p + n > end)
                n = (int)(end - p);
            for (int i = 0; i < n && out < out_end; i++)
                *out++ = *p++;
        } else {
            /* Run-length: repeat the next byte (1 - ctrl) times */
            if (p >= end)
                break;
            int n = 1 - (int)ctrl;
            uint8_t fill = *p++;
            for (int i = 0; i < n && out < out_end; i++)
                *out++ = fill;
        }
    }

    return (int)(out - dst);
}

/* ------------------------------------------------------------------ */
/* Big-endian helpers                                                  */
/* ------------------------------------------------------------------ */

static uint32_t be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |  (uint32_t)p[3];
}

static uint16_t be16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

/* ------------------------------------------------------------------ */
/* IFF/ILBM parser helpers                                             */
/* ------------------------------------------------------------------ */

#define FOURCC(a,b,c,d) ((uint32_t)((a)<<24|(b)<<16|(c)<<8|(d)))

#define CC_FORM  FOURCC('F','O','R','M')
#define CC_ILBM  FOURCC('I','L','B','M')
#define CC_BMHD  FOURCC('B','M','H','D')
#define CC_CMAP  FOURCC('C','M','A','P')
#define CC_BODY  FOURCC('B','O','D','Y')

/* BMHD chunk (20 bytes) */
typedef struct {
    uint16_t w, h;
    int16_t  x, y;
    uint8_t  n_planes;
    uint8_t  masking;
    uint8_t  compression; /* 0=none, 1=PackBits */
    uint8_t  pad;
    uint16_t transparent_color;
    uint8_t  x_aspect, y_aspect;
    uint16_t page_w, page_h;
} BmhdChunk;

static int parse_bmhd(const uint8_t *p, BmhdChunk *hdr)
{
    hdr->w           = be16(p);      p += 2;
    hdr->h           = be16(p);      p += 2;
    hdr->x           = (int16_t)be16(p); p += 2;
    hdr->y           = (int16_t)be16(p); p += 2;
    hdr->n_planes    = *p++;
    hdr->masking     = *p++;
    hdr->compression = *p++;
    hdr->pad         = *p++;
    hdr->transparent_color = be16(p); p += 2;
    hdr->x_aspect    = *p++;
    hdr->y_aspect    = *p++;
    hdr->page_w      = be16(p);      p += 2;
    hdr->page_h      = be16(p);
    return 0;
}

/* Decode IFF/ILBM file into a MoonPiv */
static MoonPiv *piv_from_ilbm(const uint8_t *buf, size_t len)
{
    if (len < 12)
        return NULL;
    if (be32(buf) != CC_FORM)
        return NULL;
    if (be32(buf + 8) != CC_ILBM)
        return NULL;

    MoonPiv   *piv  = NULL;
    BmhdChunk  hdr;
    int        got_bmhd = 0;
    uint16_t   pal[32];
    int        pal_count = 0;
    memset(&hdr, 0, sizeof(hdr));
    memset(pal, 0, sizeof(pal));

    const uint8_t *p   = buf + 12;  /* skip "FORM" + size + "ILBM" */
    const uint8_t *end = buf + len;

    while (p + 8 <= end) {
        uint32_t ck_id  = be32(p);
        uint32_t ck_sz  = be32(p + 4);
        const uint8_t *ck_data = p + 8;
        /* Chunks are word-padded */
        uint32_t ck_padded = ck_sz + (ck_sz & 1);
        p = ck_data + ck_padded;

        if (ck_id == CC_BMHD) {
            if (ck_sz < 20)
                return NULL;
            parse_bmhd(ck_data, &hdr);
            got_bmhd = 1;
        } else if (ck_id == CC_CMAP) {
            /* 3 bytes per colour: R, G, B (each 0..255, use high nibble) */
            pal_count = (int)(ck_sz / 3);
            if (pal_count > 32)
                pal_count = 32;
            for (int i = 0; i < pal_count; i++) {
                uint8_t r = ck_data[i * 3 + 0];
                uint8_t g = ck_data[i * 3 + 1];
                uint8_t b = ck_data[i * 3 + 2];
                /* Convert to Amiga 12-bit: keep high nibble of each channel */
                pal[i] = (uint16_t)(((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4));
            }
        } else if (ck_id == CC_BODY) {
            if (!got_bmhd)
                return NULL;

            int planes = hdr.n_planes;
            int w = hdr.w;
            int h = hdr.h;
            /* Row byte-width per plane (rounded up to word boundary) */
            int row_bytes = ((w + 15) / 16) * 2;
            size_t bitmap_size = (size_t)planes * (size_t)row_bytes * (size_t)h;

            piv = (MoonPiv *)calloc(1, sizeof(MoonPiv));
            if (!piv)
                return NULL;
            piv->planes = planes;
            piv->width  = w;
            piv->height = h;
            piv->bitmap = (uint8_t *)calloc(1, bitmap_size);
            if (!piv->bitmap) {
                free(piv);
                return NULL;
            }
            memcpy(piv->palette, pal, sizeof(pal));

            if (hdr.compression == 0) {
                /* Uncompressed */
                size_t copy = ck_sz < bitmap_size ? ck_sz : bitmap_size;
                memcpy(piv->bitmap, ck_data, copy);
            } else {
                /* PackBits per row, per plane (standard IFF/ILBM layout) */
                const uint8_t *src = ck_data;
                const uint8_t *src_end = ck_data + ck_sz;
                uint8_t *out = piv->bitmap;
                uint8_t *out_end = piv->bitmap + bitmap_size;

                for (int y = 0; y < h && src < src_end; y++) {
                    for (int pl = 0; pl < planes && src < src_end; pl++) {
                        uint8_t *row = out + (size_t)((y * planes + pl) * row_bytes);
                        int written = 0;
                        while (written < row_bytes && src < src_end) {
                            int8_t ctrl = (int8_t)*src++;
                            if (ctrl == -128) {
                                continue;
                            } else if (ctrl >= 0) {
                                int n = ctrl + 1;
                                for (int i = 0; i < n && src < src_end && written < row_bytes; i++) {
                                    row[written++] = *src++;
                                }
                            } else {
                                int n = 1 - (int)ctrl;
                                uint8_t fill = *src++;
                                for (int i = 0; i < n && written < row_bytes; i++)
                                    row[written++] = fill;
                            }
                        }
                        (void)out_end;
                    }
                }
            }
            break;
        }
    }

    return piv;
}

/* Decode custom Mindscape PIV format (magic 0x0004 or 0x0005) */
static MoonPiv *piv_from_custom(const uint8_t *buf, size_t len)
{
    if (len < 4)
        return NULL;

    int planes = (int)be16(buf);       /* word[0] = plane count */
    if (planes != 4 && planes != 5)
        return NULL;

    /* word[1] = flags/unknown, skip */
    const uint8_t *p = buf + 4;

    /* Palette: 32 words, each Amiga 12-bit $0RGB */
    int pal_count = (planes == 4) ? 16 : 32;
    if (p + pal_count * 2 > buf + len)
        return NULL;

    uint16_t pal[32];
    for (int i = 0; i < pal_count; i++) {
        pal[i] = be16(p);
        p += 2;
    }

    /* Body: PackBits compressed, row × planes interleaved */
    int w = 320, h = 200;
    int row_bytes = ((w + 15) / 16) * 2;
    size_t bitmap_size = (size_t)planes * (size_t)row_bytes * (size_t)h;

    MoonPiv *piv = (MoonPiv *)calloc(1, sizeof(MoonPiv));
    if (!piv)
        return NULL;
    piv->planes = planes;
    piv->width  = w;
    piv->height = h;
    piv->bitmap = (uint8_t *)calloc(1, bitmap_size);
    if (!piv->bitmap) {
        free(piv);
        return NULL;
    }
    memcpy(piv->palette, pal, sizeof(pal));

    size_t body_len = (size_t)((buf + len) - p);
    int written = moon_packbits_decompress(p, body_len, piv->bitmap, bitmap_size);
    if (written < 0) {
        free(piv->bitmap);
        free(piv);
        return NULL;
    }

    return piv;
}

/* ------------------------------------------------------------------ */
/* moon_piv_load — load a PIV file                                    */
/* ------------------------------------------------------------------ */

MoonPiv *moon_piv_load_from_buffer(const uint8_t *buf, size_t len)
{
    if (len < 4)
        return NULL;

    uint32_t magic4 = be32(buf);
    uint16_t magic2 = be16(buf);

    if (magic4 == CC_FORM) {
        return piv_from_ilbm(buf, len);
    } else if (magic2 == 0x0004 || magic2 == 0x0005) {
        return piv_from_custom(buf, len);
    }

    /* Unknown format — try IFF anyway (maybe just wrong magic detection) */
    return piv_from_ilbm(buf, len);
}
