/*
 * packbits_piv.c — PIV background decoder
 *
 * Moonstone background images (.PIV) come in two variants:
 *
 * IFF/ILBM variant:
 *   Standard IFF FORM/ILBM container with BMHD, CMAP and BODY chunks.
 *   The BODY is PackBits-compressed, one row per plane per scan line.
 *   Reference: LAB_0434 / LAB_043A-LAB_0440 in program.asm.
 *
 * Custom Mindscape variant (magic word 0x0004 or 0x0005):
 *   File layout (all big-endian):
 *     offset 0 : word  = plane count (4 or 5)
 *     offset 2 : long  = compressed body size in bytes
 *     offset 6 : 32 bytes (4-plane) or 64 bytes (5-plane) palette
 *                Each entry is a 16-bit word.  The hardware colour register
 *                value = stored_word << 1 (LAB_03FF / LAB_03F4 transform).
 *     offset 6+palette_bytes : LAB_0408 bit-level compressed bitstream
 *                carrying all bitplanes (4 or 5) sequentially.
 *
 * LAB_0408 algorithm (bit-level entropy/RLE coder, one plane = 64000 bits):
 *   State:  src bit-pointer (D5 = bit index 7..0 within current byte, A0)
 *           dst bit-pointer (D6 = bit index 7..0 within current byte, A1)
 *           D7 = remaining output bits (= 64000 = 0xFA00 at start of each plane)
 *   Opcodes (2 bits each, read MSB-first):
 *     00: long literal — read 4 bits N (0..15), copy N*4+16 bits verbatim.
 *     01: short literal — copy 4 bits verbatim.
 *     10: short run — read 1 bit (fill), read 6 bits N (0..63), fill N+1 bits.
 *     11: long run — read 1 bit (size_sel), read 1 bit (fill),
 *             if size_sel=0: read 3 bits N, count = (N+1)*128
 *             if size_sel=1: read 4 bits N, count = (N+1)*1152
 *             fill 'count' bits.
 *   After producing 64000 output bits: ADDQ.L #1,A0 (advance src one byte
 *   to the next byte boundary), then D5 is reset to 7 for the next plane.
 *
 * Pixel layout (both variants):
 *   Plane-sequential: bitplane 0 occupies bytes [0 .. row_bytes*h - 1],
 *   bitplane 1 occupies [row_bytes*h .. 2*row_bytes*h - 1], etc.
 *   Within each plane, rows are stored top-to-bottom, MSB first.
 *
 * PackBits algorithm (IFF BODY rows):
 *   byte >= 0x00 : copy the next (byte+1) literal bytes.
 *   byte == 0x80 : NOP (skip).
 *   byte  < 0x00 : repeat the next byte (1 - byte) times.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* LAB_0408  —  Mindscape bit-level coder for custom PIV bitplanes    */
/* ------------------------------------------------------------------ */

/*
 * Bit I/O context.  Both source and destination use the same convention:
 * bit_pos = 7 means the MSB of the current byte is next to be
 * read/written; bit_pos = 0 means the LSB.  When bit_pos wraps below 0
 * the byte pointer advances and bit_pos resets to 7.
 */
typedef struct {
    const uint8_t *src;       /* pointer to current source byte     */
    int            src_bit;   /* next source bit position (7..0)    */
    uint8_t       *dst;       /* pointer to current destination byte */
    int            dst_bit;   /* next destination bit position (7..0)*/
    int32_t        remaining; /* output bits still to produce        */
} Lab0408Ctx;

/* Read one bit from the source stream (MSB of current byte first). */
static inline int lab_read_bit(Lab0408Ctx *c)
{
    int val = ((*c->src) >> c->src_bit) & 1;
    if (--c->src_bit < 0) {
        c->src_bit = 7;
        c->src++;
    }
    return val;
}

/*
 * Read (n+1) bits MSB-first and return them as an unsigned integer.
 * Mirrors LAB_0430: loops DBF D4 (= n+1 iterations), shifting D3 left
 * after each bit, then a final LSR to undo the last shift.
 */
static int lab_read_bits(Lab0408Ctx *c, int n)
{
    int d3 = 0;
    for (int i = 0; i <= n; i++) {
        d3 |= lab_read_bit(c);
        if (i < n)
            d3 <<= 1;
    }
    return d3;
}

/* Write one bit to the destination stream (MSB of current byte first). */
static inline void lab_write_bit(Lab0408Ctx *c, int val)
{
    if (val)
        *c->dst |= (uint8_t)(1u << c->dst_bit);
    else
        *c->dst &= (uint8_t)~(1u << c->dst_bit);
    c->remaining--;
    if (--c->dst_bit < 0) {
        c->dst_bit = 7;
        c->dst++;
    }
}

/*
 * Fill 'count' output bits with 'val' (0 or 1).
 * Uses bulk byte writes (MOVE.B D1,(A1)+ path from LAB_0413/LAB_041B)
 * whenever the output pointer is byte-aligned (dst_bit == 7).
 */
static void lab_fill_run(Lab0408Ctx *c, int val, int32_t count)
{
    uint8_t fill_byte = val ? 0xFFu : 0x00u;

    while (count > 0 && c->remaining > 0) {
        /* Bulk byte write when output is byte-aligned and ≥ 8 bits left */
        if (c->dst_bit == 7 && count >= 8 && c->remaining >= 8) {
            int32_t chunks = count / 8;
            if (chunks > c->remaining / 8)
                chunks = c->remaining / 8;
            for (int32_t i = 0; i < chunks; i++) {
                *c->dst++ = fill_byte;
                c->remaining -= 8;
                count -= 8;
            }
        } else {
            lab_write_bit(c, val);
            count--;
        }
    }
}

/*
 * Copy 'count' bits verbatim from source to destination.
 * Mirrors LAB_0426 (literal bit-copy loop).
 */
static void lab_copy_bits(Lab0408Ctx *c, int count)
{
    for (int i = 0; i < count && c->remaining > 0; i++)
        lab_write_bit(c, lab_read_bit(c));
}

/*
 * Decompress one bitplane (exactly 64000 bits = 8000 bytes) from the
 * LAB_0408 bitstream.
 *
 * @p_src  : on entry, *p_src points to the current source byte.
 *           On return, *p_src has been advanced past this plane's data
 *           (including the LAB_0407 "ADDQ.L #1,A0" skip byte).
 * @dst    : output buffer; must be at least 8000 bytes, pre-zeroed or
 *           fully written by the decoder (every bit is set or cleared).
 */
static void lab0408_decomp_plane(const uint8_t **p_src, uint8_t *dst)
{
    Lab0408Ctx c;
    c.src      = *p_src;
    c.src_bit  = 7;   /* D5 = 7 at start of each plane (MOVEQ #7,D5) */
    c.dst      = dst;
    c.dst_bit  = 7;   /* D6 = 7 at start (MOVEQ #7,D6) */
    c.remaining = 64000; /* D7 = 0xFA00 (MOVE.L #$0000fa00,D7) */

    while (c.remaining > 0) {
        /* Read 2-bit opcode (MSB first): first bit contributes 2 if set,
         * second bit contributes 1 if set — matching the assembly. */
        int b0 = lab_read_bit(&c);
        int b1 = lab_read_bit(&c);
        int opcode = (b0 << 1) | b1;

        if (opcode == 0) {
            /* Opcode 00 (LAB_042C): long literal
             * Read 4 bits N (0..15); copy N*4+16 bits. */
            int n = lab_read_bits(&c, 3);       /* 4 bits via D4=3 */
            lab_copy_bits(&c, n * 4 + 16);

        } else if (opcode == 1) {
            /* Opcode 01 (LAB_0425): short literal — copy 4 bits. */
            lab_copy_bits(&c, 4);

        } else if (opcode == 2) {
            /* Opcode 10 (LAB_0420): short run.
             * Read fill bit; read 6 bits N (0..63); fill N+1 bits. */
            int fill = lab_read_bit(&c);
            int n    = lab_read_bits(&c, 5);    /* 6 bits via D4=5 */
            lab_fill_run(&c, fill, (int32_t)(n + 1));

        } else {
            /* Opcode 11: long run.
             * Read size_sel bit (determines run-length encoding range).
             * Read fill bit.
             * size_sel=0: read 3 bits N → count = (N+1)*128   (128..1024)
             * size_sel=1: read 4 bits N → count = (N+1)*1152  (1152..18432) */
            int size_sel = lab_read_bit(&c);   /* saved D0 / (A7) */
            int fill     = lab_read_bit(&c);   /* D0 at LAB_0410  */
            int d4       = size_sel ? 3 : 2;
            int n        = lab_read_bits(&c, d4);
            int32_t count = (int32_t)(n + 1) * (size_sel ? 1152 : 128);
            lab_fill_run(&c, fill, count);
        }
    }

    /* LAB_0407: ADDQ.L #1,A0 — advance past the current (partial) source
     * byte so the next plane starts on a fresh byte boundary. */
    *p_src = c.src + 1;
}

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
                /* PackBits per row per plane (standard IFF/ILBM layout).
                 * Store plane-sequential in memory to match the Amiga
                 * hardware layout (as used by the custom PIV decoder and
                 * referenced by LAB_043A–LAB_043C in program.asm where
                 * each plane's rows go to separate plane-buffer pointers). */
                const uint8_t *src = ck_data;
                const uint8_t *src_end = ck_data + ck_sz;

                for (int y = 0; y < h && src < src_end; y++) {
                    for (int pl = 0; pl < planes && src < src_end; pl++) {
                        /* Plane-sequential: plane pl, row y */
                        uint8_t *row = piv->bitmap
                                     + (size_t)pl  * (size_t)h * (size_t)row_bytes
                                     + (size_t)y   * (size_t)row_bytes;
                        int written = 0;
                        while (written < row_bytes && src < src_end) {
                            int8_t ctrl = (int8_t)*src++;
                            if (ctrl == -128) {
                                continue;
                            } else if (ctrl >= 0) {
                                int n = ctrl + 1;
                                for (int i = 0; i < n && src < src_end && written < row_bytes; i++)
                                    row[written++] = *src++;
                            } else {
                                int n = 1 - (int)ctrl;
                                uint8_t fill = *src++;
                                for (int i = 0; i < n && written < row_bytes; i++)
                                    row[written++] = fill;
                            }
                        }
                    }
                }
            }
            break;
        }
    }

    return piv;
}

/* Decode custom Mindscape PIV format (magic 0x0004 or 0x0005).
 *
 * File layout (program.asm SECSTRT_20 / LAB_03FC / LAB_03FA):
 *   offset 0 : word  = plane count (4 → 16 colours, 5 → 32 colours)
 *   offset 2 : long  = compressed body size (bytes)
 *   offset 6 : palette — 32 bytes (4-plane) or 64 bytes (5-plane)
 *              Each 2-byte word is a raw Amiga hardware colour register
 *              value stored as (actual_$0RGB >> 1); multiply by 2 to
 *              recover the real colour (LAB_03F4 / LAB_03FF: LSL.W #1,D0).
 *   offset 6+palette_bytes : LAB_0408-compressed bitstream for all planes.
 */
static MoonPiv *piv_from_custom(const uint8_t *buf, size_t len)
{
    if (len < 8)
        return NULL;

    int planes = (int)be16(buf);         /* word[0] = plane count */
    if (planes != 4 && planes != 5)
        return NULL;

    uint32_t body_size = be32(buf + 2);  /* long[1..4] = compressed body bytes */

    /* Palette: 32 bytes (4-plane = 16 × 2-byte entries)
     *         or 64 bytes (5-plane = 32 × 2-byte entries)
     * Starts at offset 6, matching ADDQ.L #6,A0 in SECSTRT_20 / LAB_03FC. */
    int pal_bytes = (planes == 4) ? 32 : 64;
    int pal_count = pal_bytes / 2;
    if ((size_t)(6 + pal_bytes) > len)
        return NULL;

    uint16_t pal[32];
    memset(pal, 0, sizeof(pal));
    for (int i = 0; i < pal_count; i++) {
        uint16_t raw = be16(buf + 6 + i * 2);
        /* LAB_03F4 / LAB_03FF: BCLR #15,D0; BNE skip; LSL.W #1,D0
         * For all standard Amiga colours bit15=0, so always apply LSL. */
        if (raw & 0x8000u)
            pal[i] = raw & 0x7FFFu;   /* bit15 was set: clear it, no shift */
        else
            pal[i] = (uint16_t)(raw << 1); /* normal case: double the value */
    }

    /* Body (LAB_0408 bitstream) starts immediately after the palette. */
    const uint8_t *body = buf + 6 + pal_bytes;
    size_t body_avail   = len - (size_t)(6 + pal_bytes);
    if (body_size > (uint32_t)body_avail)
        body_size = (uint32_t)body_avail;   /* clamp to available data */

    /* Output: plane-sequential bitmap for 320×200 */
    int w = 320, h = 200;
    int row_bytes   = ((w + 15) / 16) * 2;        /* = 40 bytes */
    size_t plane_bytes  = (size_t)row_bytes * (size_t)h;   /* = 8000 bytes */
    size_t bitmap_size  = (size_t)planes * plane_bytes;    /* 32000 or 40000 */

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

    /* Decompress one plane at a time; LAB_03FA calls LAB_0408 four or five
     * times with A1 = LAB_04D9 + plane * 0x1F40 (= plane * 8000). */
    const uint8_t *src = body;
    for (int pl = 0; pl < planes; pl++) {
        uint8_t *plane_dst = piv->bitmap + (size_t)pl * plane_bytes;
        lab0408_decomp_plane(&src, plane_dst);
        /* Guard: do not read past the end of the body buffer */
        if (src > body + body_size)
            break;
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
