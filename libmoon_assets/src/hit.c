/*
 * hit.c — collide.hit hitbox file loader for libmoon_assets.
 *
 * collide.hit is a plain ASCII text file loaded once at combat initialisation
 * (LAB_03DA, mog.asm line 8340).  It defines the hit-test points for every
 * attacking sprite used in combat.
 *
 * --------------------------------------------------------------------------
 * Text file format (one section per sprite):
 *
 *   <sprite_name>\n
 *   <frame_line_0>\n
 *   <frame_line_1>\n
 *   ...
 *   99\n
 *
 * Frame line with no hit points:
 *   00\n
 *
 * Frame line with n hit points:
 *   CC TT XXX₁YYY₁XXX₂YYY₂…XXXₙYYYₙ\n
 *
 *   CC  — 2-digit decimal count (01–98).  "99" terminates the section.
 *   TT  — 2-digit decimal hit-type / weight byte (matches LAB_0A55 table).
 *          Separated from CC by a single ASCII space.
 *   XXX — 3-digit decimal x offset (0–255) from sprite origin.
 *   YYY — 3-digit decimal y offset (0–255) from sprite origin.
 *          XYY pairs are concatenated with no separator between them; the
 *          whole coordinate string is separated from TT by one ASCII space.
 *
 * Parsing mirrors LAB_03D9 (2-digit read), LAB_03D8 (3-digit read), and
 * the main loop LAB_03D2 (mog.asm lines 8280–8317).
 *
 * --------------------------------------------------------------------------
 * Binary structure stored in memory by the original engine (LAB_0A4D buffer):
 *
 *   n == 0:  [0x00]
 *   n >  0:  [n:1][type:1][max_dx:1][max_dy:1][dx₀:1][dy₀:1]…[dx_{n-1}:1][dy_{n-1}:1]
 *
 * max_dx / max_dy are computed by the parser (not present in the text file).
 *
 * This C implementation reproduces the same logic and fills MoonHitFrame
 * accordingly.
 */

#include "moon_private.h"

#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Read a 2-digit decimal integer from *p and advance *p by 2.
 * Returns the value (0–99), or -1 if fewer than 2 bytes remain.
 * Mirrors LAB_03D9 (mog.asm line 8332). */
static int read2(const uint8_t **p, const uint8_t *end)
{
    if (*p + 2 > end)
        return -1;
    int v = ((*p)[0] - '0') * 10 + ((*p)[1] - '0');
    *p += 2;
    return v;
}

/* Read a 3-digit decimal integer from *p and advance *p by 3.
 * Returns the value (0–255), or -1 if fewer than 3 bytes remain.
 * Mirrors LAB_03D8 (mog.asm line 8321). */
static int read3(const uint8_t **p, const uint8_t *end)
{
    if (*p + 3 > end)
        return -1;
    int v = ((((*p)[0] - '0') * 10) + ((*p)[1] - '0')) * 10 + ((*p)[2] - '0');
    *p += 3;
    return v;
}

/* Skip one byte (space, newline, or other delimiter). */
static void skip1(const uint8_t **p, const uint8_t *end)
{
    if (*p < end)
        (*p)++;
}

/* ------------------------------------------------------------------ */
/* Count helpers (two-pass parse)                                      */
/* ------------------------------------------------------------------ */

/*
 * count_frames_in_section — count the number of frame lines before the "99"
 * terminator.  Does NOT advance p; p must already point to the first frame
 * line of the section (character after the sprite-name newline).
 */
static int count_frames_in_section(const uint8_t *p, const uint8_t *end)
{
    int frames = 0;
    while (p < end) {
        int cnt = read2(&p, end);
        if (cnt < 0 || cnt == 99)
            break;
        frames++;
        if (cnt == 0) {
            skip1(&p, end); /* newline after "00" */
            continue;
        }
        skip1(&p, end);            /* space after CC */
        if (p + 2 > end) break;
        p += 2;                    /* TT digits */
        skip1(&p, end);            /* space after TT */
        if (p + (size_t)(cnt * 6) > end) break;
        p += (size_t)(cnt * 6);    /* XXX YYY pairs */
        skip1(&p, end);            /* newline */
    }
    return frames;
}

/*
 * count_sprites_in_file — scan the whole buffer and return the number of
 * complete sprite sections (each terminated by "99").
 */
static int count_sprites_in_file(const uint8_t *p, const uint8_t *end)
{
    int count = 0;
    while (p < end) {
        /* skip sprite name until '\n' */
        while (p < end && *p != '\n')
            p++;
        if (p >= end)
            break;
        p++; /* skip '\n' after name */

        /* scan frame lines until "99" or end */
        int found_term = 0;
        while (!found_term && p < end) {
            int cnt = read2(&p, end);
            if (cnt < 0)
                goto done;
            if (cnt == 99) {
                count++;
                found_term = 1;
                break;
            }
            if (cnt == 0) {
                skip1(&p, end);
                continue;
            }
            skip1(&p, end);
            if (p + 2 > end) goto done;
            p += 2;
            skip1(&p, end);
            if (p + (size_t)(cnt * 6) > end) goto done;
            p += (size_t)(cnt * 6);
            skip1(&p, end);
        }
    }
done:
    return count;
}

/* ------------------------------------------------------------------ */
/* Public API                                                          */
/* ------------------------------------------------------------------ */

MoonHit *moon_hit_load(const char *name)
{
    size_t   file_size = 0;
    uint8_t *raw       = moon_file_read(name, &file_size);
    if (!raw)
        return NULL;

    const uint8_t *buf = raw;
    const uint8_t *end = buf + file_size;
    const uint8_t *p   = buf;

    /* ---- Pass 1: count sprite sections ---- */
    int n_sprites = count_sprites_in_file(p, end);
    if (n_sprites <= 0) {
        free(raw);
        return NULL;
    }

    /* ---- Allocate top-level structure ---- */
    MoonHit *hit = calloc(1, sizeof(*hit));
    if (!hit) {
        free(raw);
        return NULL;
    }
    hit->sprites = calloc((size_t)n_sprites, sizeof(MoonHitSprite));
    if (!hit->sprites) {
        free(hit);
        free(raw);
        return NULL;
    }

    /* ---- Pass 2: parse each section ---- */
    int si = 0;
    p = buf;

    while (si < n_sprites && p < end) {
        MoonHitSprite *sp = &hit->sprites[si];

        /* Read sprite name (up to '\n') */
        const uint8_t *name_start = p;
        while (p < end && *p != '\n')
            p++;
        size_t nlen = (size_t)(p - name_start);
        if (nlen >= sizeof(sp->name))
            nlen = sizeof(sp->name) - 1;
        memcpy(sp->name, name_start, nlen);
        sp->name[nlen] = '\0';
        if (p < end)
            p++; /* skip '\n' */

        /* Count frames then allocate */
        sp->frame_count = count_frames_in_section(p, end);
        if (sp->frame_count > 0) {
            sp->frames = calloc((size_t)sp->frame_count, sizeof(MoonHitFrame));
            if (!sp->frames) {
                hit->sprite_count = si;
                moon_hit_free(hit);
                free(raw);
                return NULL;
            }
        }

        /* Parse frame lines */
        int fi = 0;
        while (fi < sp->frame_count && p < end) {
            int cnt = read2(&p, end);
            if (cnt < 0 || cnt == 99)
                break;

            MoonHitFrame *fr = &sp->frames[fi++];
            fr->n_points = (uint8_t)cnt;

            /* skip delimiter (newline for cnt==0, space for cnt>0) */
            skip1(&p, end);

            if (cnt == 0)
                continue; /* frame has no hit points */

            /* read type byte */
            int type = read2(&p, end);
            fr->type = (type >= 0) ? (uint8_t)type : 0;
            skip1(&p, end); /* space after TT */

            /* allocate and read dx/dy pairs */
            fr->points = calloc((size_t)cnt, sizeof(MoonHitPoint));
            if (!fr->points) {
                hit->sprite_count = si + 1;
                moon_hit_free(hit);
                free(raw);
                return NULL;
            }

            uint8_t max_dx = 0, max_dy = 0;
            for (int i = 0; i < cnt; i++) {
                int dx = read3(&p, end);
                int dy = read3(&p, end);
                fr->points[i].dx = (dx >= 0) ? (uint8_t)dx : 0;
                fr->points[i].dy = (dy >= 0) ? (uint8_t)dy : 0;
                if (fr->points[i].dx > max_dx) max_dx = fr->points[i].dx;
                if (fr->points[i].dy > max_dy) max_dy = fr->points[i].dy;
            }
            fr->max_dx = max_dx;
            fr->max_dy = max_dy;

            skip1(&p, end); /* newline at end of coordinate line */
        }

        /* consume "99" terminator newline (read2 already consumed "99") */
        skip1(&p, end);

        si++;
    }

    hit->sprite_count = si;
    free(raw);
    return hit;
}

const MoonHitSprite *moon_hit_find(const MoonHit *hit, const char *name)
{
    if (!hit || !name)
        return NULL;
    for (int i = 0; i < hit->sprite_count; i++) {
        if (strcmp(hit->sprites[i].name, name) == 0)
            return &hit->sprites[i];
    }
    return NULL;
}

void moon_hit_free(MoonHit *hit)
{
    if (!hit)
        return;
    if (hit->sprites) {
        for (int i = 0; i < hit->sprite_count; i++) {
            MoonHitSprite *sp = &hit->sprites[i];
            if (sp->frames) {
                for (int j = 0; j < sp->frame_count; j++)
                    free(sp->frames[j].points);
                free(sp->frames);
            }
        }
        free(hit->sprites);
    }
    free(hit);
}
