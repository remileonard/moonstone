/*
 * libmoon_assets — Moonstone asset loading library
 *
 * Loads and decompresses the original Mindscape/Amiga Moonstone assets
 * without any file conversion: .cel, .PIV, .stile, .cmp, .ob
 *
 * File formats supported:
 *   - CEL  : sprite sheets (LZSS compressed, proprietary Mindscape header)
 *   - PIV  : background bitmaps (proprietary Mindscape, LZSS body)
 *   - STILE: tile maps (2-bit RLE)
 *   - CMP  : ProTracker modules (RNC ProPack 1 compressed)
 *   - OB   : character sprite sheets (same format as CEL, LZSS compressed)
 *
 * All multi-byte values in Mindscape files are big-endian (Amiga/68000).
 */

#ifndef MOON_ASSETS_H
#define MOON_ASSETS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Library lifecycle                                                   */
/* ------------------------------------------------------------------ */

/**
 * moon_init - Initialise the library with a path to the asset directory.
 * @asset_dir: path to the folder containing Moonstone asset files.
 * Returns 0 on success, -1 on error.
 */
int moon_init(const char *asset_dir);

/** moon_shutdown - Release all cached assets and free library state. */
void moon_shutdown(void);

/* ------------------------------------------------------------------ */
/* CEL — sprite sheets                                                 */
/* ------------------------------------------------------------------ */

/**
 * MoonCelFrame - metadata + pixel data for a single animation frame.
 *
 * Pixels are 1-bit-per-plane planar data, stored plane-sequential:
 *   bitplane 0 occupies bytes [0 .. row_bytes*height - 1],
 *   bitplane 1 occupies bytes [row_bytes*height .. 2*row_bytes*height - 1],
 *   …
 * where row_bytes = ((width + 15) / 16) * 2.
 * Within each plane, rows are top-to-bottom and the MSB of each byte is
 * the left-most pixel.
 *
 * Total size of `data`:
 *   planes * ((width + 15) / 16) * 2 * height  bytes
 */
typedef struct {
    uint16_t width;        /* frame width in pixels */
    uint16_t height;       /* frame height in lines  */
    uint8_t  planes;       /* number of bitplanes active (1..5) */
    uint8_t  draw_flags;   /* blit flags from CEL header */
    uint8_t  minterm;      /* blitter minterm byte */
    uint8_t *data;         /* planar pixel data (owned by MoonCel) */
} MoonCelFrame;

/**
 * MoonCel - a complete CEL sprite file.
 */
typedef struct {
    int           frame_count; /* total number of frames */
    MoonCelFrame *frames;      /* array of frame_count frames */
} MoonCel;

/**
 * moon_cel_load - load and decompress a CEL file.
 * @name: filename relative to the asset directory (e.g. "au1.cel").
 * Returns a pointer to a newly allocated MoonCel, or NULL on error.
 * The returned object is owned by the caller; free with moon_cel_free().
 * Results are cached: calling with the same name returns the same pointer
 * (refcount incremented).
 */
MoonCel *moon_cel_load(const char *name);

/** moon_cel_free - release a MoonCel obtained from moon_cel_load(). */
void moon_cel_free(MoonCel *cel);

/* ------------------------------------------------------------------ */
/* PIV — background bitmaps                                            */
/* ------------------------------------------------------------------ */

/**
 * MoonPiv - a decoded PIV background bitmap.
 *
 * Bitmap data is plane-sequential (matching Amiga hardware layout and
 * the LAB_0408 / LAB_043A output in program.asm):
 *   bitplane 0 occupies bytes [0 .. row_bytes*height - 1],
 *   bitplane 1 occupies bytes [row_bytes*height .. 2*row_bytes*height - 1],
 *   …
 * where row_bytes = ((width+15)/16)*2.  MSB of each byte is left-most pixel.
 *
 * Palette entries are Amiga 12-bit colors: 0x0RGB (4 bits per channel,
 * only the lower 12 bits are significant).
 */
typedef struct {
    int      planes;        /* 4 or 5 bitplanes */
    int      width;         /* pixels per row (typically 320) */
    int      height;        /* rows (typically 200) */
    uint8_t *bitmap;        /* planar pixel data */
    uint16_t palette[32];   /* Amiga 12-bit palette ($0RGB), up to 32 entries */
} MoonPiv;

/**
 * moon_piv_load - load and decode a PIV background file.
 * @name: filename (e.g. "bg1a.PIV").
 * Returns a newly allocated MoonPiv, or NULL on error.
 */
MoonPiv *moon_piv_load(const char *name);

/**
 * moon_piv_load_from_buffer - decode a PIV from a memory buffer.
 * @buf: pointer to raw file bytes.
 * @len: byte count.
 * Returns a newly allocated MoonPiv (caller must free with moon_piv_free()),
 * or NULL on error. This object is NOT cached.
 */
MoonPiv *moon_piv_load_from_buffer(const uint8_t *buf, size_t len);

/** moon_piv_free - release a MoonPiv. */
void moon_piv_free(MoonPiv *piv);

/* ------------------------------------------------------------------ */
/* STILE — tile maps                                                   */
/* ------------------------------------------------------------------ */

/**
 * MoonStile - decompressed tilemap data.
 *
 * `data` holds the raw decompressed bitplane data for all tiles.
 * The layout mirrors the original Amiga bitplane format.
 */
typedef struct {
    size_t   size;  /* byte count of `data` */
    uint8_t *data;  /* decompressed bitplane bytes */
} MoonStile;

/**
 * moon_stile_load - load and decompress a .stile file.
 * @name: filename (e.g. "intro.stile").
 * Returns a newly allocated MoonStile, or NULL on error.
 */
MoonStile *moon_stile_load(const char *name);

/** moon_stile_free - release a MoonStile. */
void moon_stile_free(MoonStile *stile);

/* ------------------------------------------------------------------ */
/* CMP — ProTracker modules (RNC ProPack 1 compressed)                */
/* ------------------------------------------------------------------ */

/**
 * MoonModSample - descriptor and PCM data for one ProTracker sample slot.
 *
 * Matches the 30-byte sample header layout of a ProTracker MOD file
 * (big-endian Amiga format).  All length/loop fields are in WORDS
 * (multiply by 2 to get byte counts).
 */
typedef struct {
    char     name[23];            /* null-terminated sample name (22 chars max) */
    uint16_t length_words;        /* sample length in words (bytes = length_words * 2) */
    int8_t   finetune;            /* signed fine-tune value: -8..+7                   */
    uint8_t  volume;              /* playback volume: 0..64                           */
    uint16_t repeat_offset_words; /* loop start position, in words                   */
    uint16_t repeat_length_words; /* loop length in words (1 = no loop)              */
    uint8_t *data;                /* PCM sample data (signed 8-bit), may be NULL     */
} MoonModSample;

/**
 * MoonMod - a fully parsed ProTracker MOD module.
 *
 * The decompressed .cmp file is a standard 31-sample, 4-channel ProTracker
 * module (as confirmed by program.asm LAB_0061: offset 0x3B8 = 952 for the
 * pattern order table, 0x43C = 1084 for pattern data, 31-sample loop at
 * LAB_0064).  All fields are decoded from the raw big-endian MOD header.
 *
 * Pattern data layout: pattern_count × 64 rows × 4 channels × 4 bytes.
 * Each 4-byte channel word:
 *   [sample_hi(4)] [period_hi(4)] [period_lo(8)] [sample_lo(4)] [effect(12)]
 */
typedef struct {
    char          title[21];          /* null-terminated module title (20 chars max) */
    uint8_t       song_length;        /* number of valid entries in order[]          */
    uint8_t       restart_position;   /* restart position (ProTracker field)         */
    uint8_t       order[128];         /* pattern order / position table              */
    uint32_t      pattern_count;      /* number of distinct patterns                 */
    uint8_t      *pattern_data;       /* raw pattern data: pattern_count × 1024 B   */
    int           sample_count;       /* always 31 for Moonstone modules             */
    MoonModSample samples[31];        /* sample descriptors (index 0 = slot 1)      */
} MoonMod;

/**
 * moon_mod_load - load, decompress and parse a .cmp music file.
 * @name: filename (e.g. "music.cmp").
 * Returns a newly allocated MoonMod, or NULL on error.
 */
MoonMod *moon_mod_load(const char *name);

/** moon_mod_free - release a MoonMod obtained from moon_mod_load(). */
void moon_mod_free(MoonMod *mod);

/* ------------------------------------------------------------------ */
/* OB — character sprite sheets (same format as CEL)                  */
/* ------------------------------------------------------------------ */

/**
 * MoonOb - a decoded .ob character object sprite sheet.
 *
 * The .ob format is structurally identical to the .cel format: a 10-byte
 * global header (frame_count word + two longs), a frame table of
 * frame_count × 10-byte entries, and an LZSS-compressed pixel body.
 * Loading is handled by the same decoder used for .cel files.
 *
 * Frame pixel data layout is identical to MoonCelFrame — see the CEL
 * section above for the full description of the planar format.
 */
typedef struct {
    int           frame_count; /* total number of animation frames */
    MoonCelFrame *frames;      /* array of frame_count frames (same layout as CEL) */
} MoonOb;

/**
 * moon_ob_load - load and decode a .ob character sprite file.
 * @name: filename (e.g. "kn1.ob").
 * Returns a newly allocated MoonOb, or NULL on error.
 */
MoonOb *moon_ob_load(const char *name);

/** moon_ob_free - release a MoonOb obtained from moon_ob_load(). */
void moon_ob_free(MoonOb *ob);

/* ------------------------------------------------------------------ */
/* Generic raw file access                                             */
/* ------------------------------------------------------------------ */

/**
 * moon_file_read - read a raw file from the asset directory.
 * @name: filename relative to the asset directory.
 * @out_size: receives the file size on success.
 * Returns a malloc'd buffer (caller must free), or NULL on error.
 */
uint8_t *moon_file_read(const char *name, size_t *out_size);

/* ------------------------------------------------------------------ */
/* Decompressors (also usable standalone)                              */
/* ------------------------------------------------------------------ */

/**
 * moon_rnc1_decompress - decompress a RNC ProPack type-1 buffer.
 * @src: pointer to compressed data (including 18-byte RNC header).
 * @src_len: total byte count of src.
 * @dst: output buffer (must be large enough for the decompressed data).
 * @dst_len: capacity of dst.
 * Returns the number of bytes written to dst, or -1 on error.
 */
int moon_rnc1_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len);

/**
 * moon_lzss_decompress - decompress a Mindscape LZSS-compressed buffer.
 * @src: compressed data.
 * @src_len: byte count of src.
 * @dst: output buffer.
 * @dst_len: capacity of dst.
 * Returns the number of bytes written, or -1 on error.
 */
int moon_lzss_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len);

/**
 * moon_rle_stile_decompress - decompress 2-bit RLE bitplane data.
 * @src: compressed data.
 * @src_len: byte count of src.
 * @dst: output buffer.
 * @dst_len: capacity of dst.
 * Returns the number of bytes written, or -1 on error.
 */
int moon_rle_stile_decompress(const uint8_t *src, size_t src_len,
                              uint8_t *dst, size_t dst_len);

/**
 * moon_packbits_decompress - decompress PackBits-encoded data.
 * @src: compressed data.
 * @src_len: byte count.
 * @dst: output buffer.
 * @dst_len: capacity.
 * Returns the number of bytes written, or -1 on error.
 */
int moon_packbits_decompress(const uint8_t *src, size_t src_len,
                             uint8_t *dst, size_t dst_len);

#ifdef __cplusplus
}
#endif

#endif /* MOON_ASSETS_H */
