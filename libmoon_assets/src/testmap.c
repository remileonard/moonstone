/*
 * testmap.c — Loader for the "Test" multi-PIV container (libmoon_assets).
 *
 * The "Test" file is a plain concatenation of 9 Mindscape-PIV images.
 * Each image starts immediately after the previous one.  The start of each
 * image is identified by its 2-byte plane-count magic word (0x0004 or
 * 0x0005), followed by a 4-byte compressed-body length, then the palette,
 * then the compressed bitstream (LAB_0408 algorithm).
 *
 * Layout of a single Mindscape-PIV within the container:
 *   +0  word  plane_count   (4 or 5; used as magic to locate next image)
 *   +2  long  comp_size     compressed body size in bytes
 *   +6  ...   palette       32 bytes (4 planes) or 64 bytes (5 planes)
 *   +6+palette_bytes  ...   LAB_0408 compressed bitstream (comp_size bytes)
 *
 * Total image size = 6 + palette_bytes + comp_size.
 *
 * Reference: mog.asm LAB_013A (loader), LAB_0142 (PIV#5 = overworld map),
 *            packbits_piv.c (LAB_0C21 / moon_piv_load_from_buffer).
 */

#include "moon_private.h"
#include "moon_testmap.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static uint16_t read_u16_be(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static uint32_t read_u32_be(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) |
           ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |
            (uint32_t)p[3];
}

/*
 * seek_to_image — walk the container and return the byte offset of image
 * number img_index, or (size_t)-1 if the index is out of range.
 */
static size_t seek_to_image(const uint8_t *data, size_t len, int img_index)
{
    size_t offset = 0;
    int    idx    = 0;

    while (offset + 6 <= len) {
        uint16_t magic     = read_u16_be(data + offset);
        uint32_t comp_size = read_u32_be(data + offset + 2);

        if (magic != 4 && magic != 5)
            return (size_t)-1;   /* not a valid PIV start */

        if (idx == img_index)
            return offset;

        int palette_bytes = (magic == 5) ? 64 : 32;
        size_t img_size   = 6u + (size_t)palette_bytes + (size_t)comp_size;

        if (offset + img_size > len)
            return (size_t)-1;   /* truncated file */

        offset += img_size;
        idx++;
    }

    return (size_t)-1;   /* img_index beyond end of container */
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

MoonPiv *moon_testmap_load_piv_from_buffer(const uint8_t *data, size_t len,
                                           int img_index)
{
    if (!data || img_index < 0)
        return NULL;

    size_t off = seek_to_image(data, len, img_index);
    if (off == (size_t)-1)
        return NULL;

    /* Determine size of this single image so we pass only its slice */
    uint32_t comp_size    = read_u32_be(data + off + 2);
    uint16_t magic        = read_u16_be(data + off);
    int      palette_bytes = (magic == 5) ? 64 : 32;
    size_t   img_size     = 6u + (size_t)palette_bytes + (size_t)comp_size;

    if (off + img_size > len)
        return NULL;

    return moon_piv_load_from_buffer(data + off, img_size);
}

MoonPiv *moon_testmap_load_piv(const char *name, int img_index)
{
    if (!g_ctx.initialised || !name || img_index < 0)
        return NULL;

    size_t    file_len;
    uint8_t  *buf = moon_file_read(name, &file_len);
    if (!buf)
        return NULL;

    MoonPiv *piv = moon_testmap_load_piv_from_buffer(buf, file_len, img_index);
    free(buf);
    return piv;
}
