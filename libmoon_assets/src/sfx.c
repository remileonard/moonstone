/*
 * sfx.c — .a audio sample bank loader for libmoon_assets.
 *
 * .a files are flat binary blobs of raw 8-bit signed PCM audio data in
 * Amiga Paula native format.  There is no file header; the entire file
 * is concatenated sample data.  The boundaries of individual samples
 * within the bank are NOT stored in the file — they are defined by the
 * LAB_10A3 descriptor table hardcoded in the game binary (mog.asm #31115).
 *
 * Loading procedure mirrors the Amiga assembly:
 *
 *   LAB_0AA7  (mog.asm #19329) — combat audio initialisation; installs
 *             VBL and audio-DMA interrupt handlers, then loads Re.a.
 *
 *   LAB_0AAA…LAB_0AB4  (mog.asm #19338–19421) — one routine per creature
 *             type; each calls the common dispatcher LAB_0AB5 with a
 *             destination buffer pointer and expected byte count.
 *
 *   LAB_0AB5  (mog.asm #19423) — the common file-load dispatcher:
 *             A0 = filename, A1 = destination buffer, D0 = byte count.
 *             Opens the file via LAB_0BB5 (floppy hash-directory lookup),
 *             reads D0 bytes with LAB_0BD7, then closes.
 *
 *   LAB_0FD4  (mog.asm #29138) — post-load pointer relocation; iterates
 *             over sub-ranges of LAB_10A3 and converts each entry's +6
 *             field from a file-relative PCM offset into an absolute
 *             Amiga memory pointer by adding the buffer base address.
 *
 * In this C implementation no pointer relocation is needed; callers
 * access individual samples directly as:
 *
 *   uint8_t *start = sfx->data + pcm_offset;  // offset from LAB_10A3[i]+6
 *   size_t   bytes = (size_t)length_words * 2; // length from LAB_10A3[i]+4
 *
 * .a files are optional — they are not part of the repository and must be
 * extracted from the original floppy disk image.  moon_sfx_load() returns
 * NULL (not an error) when the file is absent, allowing the game to run
 * silently without the affected sounds.
 */

#include "moon_private.h"

#include <stdlib.h>

MoonSfx *moon_sfx_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonSfx *sfx = malloc(sizeof(MoonSfx));
    if (!sfx) {
        free(buf);
        return NULL;
    }

    sfx->size = len;
    sfx->data = buf;
    return sfx;
}

void moon_sfx_free(MoonSfx *sfx)
{
    if (!sfx)
        return;
    free(sfx->data);
    free(sfx);
}
