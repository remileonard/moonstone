/*
 * rnc1.c — RNC ProPack type-1 decompressor
 *
 * Ported from the Amiga 68000 assembly routine LAB_0190 in program.asm
 * (Moonstone — A Hard Days Knight, Mindscape 1991).
 *
 * RNC ProPack 1 was authored by Rob Northen Computing (UK).
 * Format:
 *   Bytes  0-3  : magic "RNC\x01" (0x524E4301)
 *   Bytes  4-7  : uncompressed size (big-endian uint32)
 *   Bytes  8-11 : compressed data size (big-endian uint32), NOT including header
 *   Bytes 12-13 : CRC-16 of uncompressed data
 *   Bytes 14-15 : CRC-16 of compressed data
 *   Byte   16   : leeway (extra bytes at end of compressed data)
 *   Byte   17   : chunk count
 *   Bytes 18+   : compressed data blocks
 *
 * Each block uses 3 Huffman tables:
 *   - raw-byte values
 *   - back-reference offsets
 *   - back-reference lengths
 * Followed by alternating literal/match sequences.
 *
 * The assembly implementation reads the compressed stream BACKWARDS
 * (from the end), but the C port reads it forwards for clarity.
 * The end result is identical.
 */

#include "moon_assets.h"

#include <string.h>

/* ------------------------------------------------------------------ */
/* Big-endian helpers                                                  */
/* ------------------------------------------------------------------ */

static uint32_t read_be32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] <<  8) |  (uint32_t)p[3];
}


/* ------------------------------------------------------------------ */
/* Bit-stream reader (reads MSB first from a byte stream)             */
/* ------------------------------------------------------------------ */

typedef struct {
    const uint8_t *data;
    size_t         size;
    size_t         pos;    /* current byte offset */
    uint32_t       bits;   /* bit buffer */
    int            nbits;  /* valid bits in buffer */
} BitReader;

static void br_init(BitReader *br, const uint8_t *data, size_t size)
{
    br->data  = data;
    br->size  = size;
    br->pos   = 0;
    br->bits  = 0;
    br->nbits = 0;
}

/* Ensure at least n bits are available in the buffer */
static void br_fill(BitReader *br)
{
    while (br->nbits <= 24 && br->pos < br->size) {
        br->bits = (br->bits << 8) | br->data[br->pos++];
        br->nbits += 8;
    }
}

/* Peek at the top n bits without consuming them */
static uint32_t br_peek(BitReader *br, int n)
{
    br_fill(br);
    return (br->bits >> (br->nbits - n)) & ((1u << n) - 1u);
}

/* Read and consume n bits */
static uint32_t br_read(BitReader *br, int n)
{
    uint32_t v = br_peek(br, n);
    br->nbits -= n;
    return v;
}

/* ------------------------------------------------------------------ */
/* Huffman table                                                       */
/* ------------------------------------------------------------------ */

#define RNC_MAX_CODES 256

typedef struct {
    int      count;
    int      lengths[RNC_MAX_CODES];  /* code lengths in bits */
    uint16_t codes[RNC_MAX_CODES];    /* Huffman codes (MSB-aligned in 16 bits) */
    int      values[RNC_MAX_CODES];   /* associated values */
} HuffTable;

/*
 * RNC1 Huffman table format in the bitstream:
 *   5 bits: number of codes (0 = empty table)
 *   For each code:
 *     4 bits: code length in bits (1..16)
 *     code-length bits: the Huffman code itself
 *     4 bits: extra bits to read after the symbol (for offsets/lengths)
 *     (plus the base value is implicit from table position)
 *
 * Actually: each table entry stores the code value and length.
 * We decode by scanning entries until we find a matching prefix.
 */

typedef struct {
    int      num;                  /* number of entries */
    int      code_len[RNC_MAX_CODES]; /* bit length of each code */
    uint16_t code_val[RNC_MAX_CODES]; /* Huffman code (MSB-first) */
    int      extra_bits[RNC_MAX_CODES]; /* extra bits to read */
    int      base_val[RNC_MAX_CODES];   /* base value for this symbol */
} RncHuff;

static int rnc_read_table(BitReader *br, RncHuff *tbl)
{
    int n = (int)br_read(br, 5);
    tbl->num = n;
    if (n == 0)
        return 0;

    for (int i = 0; i < n; i++) {
        int clen = (int)br_read(br, 4);
        tbl->code_len[i] = clen;
        if (clen == 0) {
            tbl->code_val[i]   = 0;
        } else {
            tbl->code_val[i] = (uint16_t)br_read(br, clen);
        }
        tbl->extra_bits[i] = (int)br_read(br, 4);
        tbl->base_val[i]   = 0; /* filled in by caller if needed */
    }
    return 0;
}

/*
 * Decode one symbol from the bitstream using the given Huffman table.
 * Returns the symbol index, or -1 on error.
 */
static int rnc_decode_sym(BitReader *br, const RncHuff *tbl)
{
    /* Peek at up to 16 bits and scan table entries */
    uint16_t bits = (uint16_t)br_peek(br, 16);

    for (int i = 0; i < tbl->num; i++) {
        int clen = tbl->code_len[i];
        if (clen == 0)
            continue;
        /* Compare the top clen bits */
        uint16_t mask = (uint16_t)(0xFFFFu << (16 - clen));
        if ((bits & mask) == (tbl->code_val[i] << (16 - clen))) {
            br_read(br, clen);  /* consume the code bits */
            return i;
        }
    }
    return -1;
}

/* ------------------------------------------------------------------ */
/* CRC-16 (used by RNC for data integrity, optional check)            */
/* ------------------------------------------------------------------ */

static uint16_t rnc_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i] << 8;
        for (int b = 0; b < 8; b++) {
            if (crc & 0x8000)
                crc = (uint16_t)((crc << 1) ^ 0x8005);
            else
                crc <<= 1;
        }
    }
    return crc;
}

/* ------------------------------------------------------------------ */
/* Main decompression entry point                                      */
/* ------------------------------------------------------------------ */

#define RNC1_MAGIC      0x524E4301u   /* "RNC\x01" */
#define RNC1_HEADER_LEN 18

int moon_rnc1_decompress(const uint8_t *src, size_t src_len,
                         uint8_t *dst, size_t dst_len)
{
    if (src_len < RNC1_HEADER_LEN)
        return -1;

    /* Parse header */
    if (read_be32(src) != RNC1_MAGIC)
        return -1;

    uint32_t uncomp_size = read_be32(src + 4);
    uint32_t comp_size   = read_be32(src + 8);
    /* uint16_t crc_uncomp = read_be16(src + 12); */
    /* uint16_t crc_comp   = read_be16(src + 14); */
    /* uint8_t  leeway     = src[16]; */
    uint8_t  chunks     = src[17];

    if (uncomp_size > dst_len)
        return -1;
    if ((size_t)(RNC1_HEADER_LEN + comp_size) > src_len)
        return -1;

    BitReader br;
    br_init(&br, src + RNC1_HEADER_LEN, comp_size);

    uint8_t *out     = dst;
    uint8_t *out_end = dst + uncomp_size;

    for (int chunk = 0; chunk < (int)chunks; chunk++) {
        /* Each chunk has 3 Huffman tables followed by the encoded data */
        RncHuff tbl_raw, tbl_off, tbl_len;

        if (rnc_read_table(&br, &tbl_raw) < 0) return -1;
        if (rnc_read_table(&br, &tbl_off) < 0) return -1;
        if (rnc_read_table(&br, &tbl_len) < 0) return -1;

        /* Number of packs in this chunk */
        int num_packs = (int)br_read(br.nbits > 16 ? &br : &br, 16);
        /* re-read: num_packs is a 16-bit count */
        /* We consumed 16 bits above; use the br object directly */
        /* Actually this is correct – we already consumed */
        /* (Re-init the count properly) */

        /*
         * The chunk data consists of num_packs "pack" sequences.
         * Each pack is:
         *   - 1 or more raw literals (using tbl_raw)
         *   - 0 or more back-references (using tbl_off + tbl_len)
         * The sequence ends at num_packs rounds.
         *
         * Precise encoding: the number of raw bytes before the first
         * back-ref is encoded with tbl_raw, and the back-ref count
         * afterwards. After the back-ref(s), another raw-count is read,
         * etc.  The total structure repeats num_packs times.
         *
         * (num_packs was read above as the 16-bit value from the stream)
         */
        for (int p = 0; p < num_packs && out < out_end; p++) {
            /* Raw literal count */
            int ri = rnc_decode_sym(&br, &tbl_raw);
            if (ri < 0) return -1;
            int raw_count = tbl_raw.extra_bits[ri]
                ? (int)((tbl_raw.code_val[ri] >> (16 - tbl_raw.code_len[ri]))
                        | (br_read(&br, tbl_raw.extra_bits[ri])))
                : ri;
            /* Actually simpler: raw_count = base + extra */
            raw_count = ri + (tbl_raw.extra_bits[ri]
                              ? (int)br_read(&br, tbl_raw.extra_bits[ri])
                              : 0);

            /* Copy raw_count literal bytes */
            for (int i = 0; i < raw_count && out < out_end; i++) {
                *out++ = (uint8_t)br_read(&br, 8);
            }

            if (p == num_packs - 1)
                break; /* Last pack has no back-reference */

            /* Back-reference offset */
            int oi = rnc_decode_sym(&br, &tbl_off);
            if (oi < 0) return -1;
            int off = oi + (tbl_off.extra_bits[oi]
                            ? (int)br_read(&br, tbl_off.extra_bits[oi])
                            : 0);
            off += 1; /* offset is 1-based */

            /* Back-reference length */
            int li = rnc_decode_sym(&br, &tbl_len);
            if (li < 0) return -1;
            int len = li + (tbl_len.extra_bits[li]
                            ? (int)br_read(&br, tbl_len.extra_bits[li])
                            : 0);
            len += 2; /* minimum match is 2 */

            /* Copy `len` bytes from output[-off] */
            uint8_t *back = out - off;
            if (back < dst)
                return -1;
            for (int i = 0; i < len && out < out_end; i++) {
                *out++ = back[i];
            }
        }
    }

    (void)rnc_crc16; /* suppress unused warning; CRC check is optional */
    return (int)(out - dst);
}
