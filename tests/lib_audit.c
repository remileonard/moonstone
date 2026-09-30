/*
 * lib_audit.c — audit de libmoon_assets : chaque fichier du jeu est lu par
 * la bibliothèque et par les décodeurs du portage (vérifiés contre
 * l'original sur le banc), puis les résultats sont comparés.
 *
 *   lib_audit <données>
 *
 * Références : les décodeurs du portage d'avant la bibliothèque, figés
 * dans lib_audit_ref.c (vérifiés contre l'original sur les bancs) :
 *   LZSS         ref_unpack (LAB_0CC2 / Unpack_Lzss)
 *   CEL, OB, .c  LAB_0CBB : en-tête, table des frames, ref_unpack
 *   PIV, .p      LAB_0402 / LAB_0C27 : en-tête, palette (bit 15), LZSS
 *   Test         PIV à la suite (LAB_013A)
 *   RNC (.cmp)   ref_rnc_unpack (Unpack_Rnc1 de program)
 *   MOD          octets décompressés, lus comme le lecteur (LAB_0061)
 *   stile        octets bruts (LAB_0185 : LAB_03B2 sans décodage)
 *   terrain .t   LAB_0A6D : long taille, LZSS, obstacles, objets
 *   .a           octets bruts (LAB_0AB5)
 *   collide.hit  ref_hit_parse (Col_LoadHitData)
 *
 * N'écrit rien : rapport des écarts sur la sortie standard.
 */
#define _POSIX_C_SOURCE 200809L
#include "moon_assets.h"
#include "moon_testmap.h"
#include "mog_boot.h"
#include "prog_intro.h"
#include "lib_audit_ref.h"
#include "ix_mog_syms.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define VM_SIZE   0x01000000u
#define NAME_AT   0x00000100u
#define SRC_AT    0x00100000u
#define DST_AT    0x00800000u

static IxVM s_vm;
static int s_files, s_ok, s_bad, s_skip;
static struct { const char *kind; int ok, bad; } s_kinds[16];

static void count_kind(const char *kind, int ok)
{
    int i = 0;
    while (s_kinds[i].kind && strcmp(s_kinds[i].kind, kind))
        i++;
    s_kinds[i].kind = kind;
    if (ok > 0)
        s_kinds[i].ok++;
    else
        s_kinds[i].bad++;
}

static uint16_t be16(const uint8_t *p) { return (uint16_t)(p[0] << 8 | p[1]); }
static uint32_t be32(const uint8_t *p) { return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3]; }

static void put_bytes(uint32_t a, const uint8_t *b, size_t n)
{
    for (size_t i = 0; i < n; i++)
        ix_wb(&s_vm, a + (uint32_t)i, b[i]);
}

static void put_name(IxVM *vm, uint32_t a, const char *n)
{
    size_t i = 0;
    for (; n[i]; i++)
        ix_wb(vm, a + (uint32_t)i, (uint8_t)n[i]);
    ix_wb(vm, a + (uint32_t)i, 0);
}

static void clear_vm(void)
{
    memset(s_vm.mem, 0, s_vm.size);
}

/* Premier octet différent entre la bibliothèque et la référence (VM) */
static long first_diff(const uint8_t *lib, uint32_t ref, size_t n)
{
    for (size_t i = 0; i < n; i++)
        if (lib[i] != ix_rb(&s_vm, ref + (uint32_t)i))
            return (long)i;
    return -1;
}

static void verdict(const char *kind, const char *name, int ok, const char *fmt, ...)
    __attribute__((format(printf, 4, 5)));

#include <stdarg.h>
static void verdict(const char *kind, const char *name, int ok, const char *fmt, ...)
{
    s_files++;
    count_kind(kind, ok);
    if (ok > 0) {
        s_ok++;
        return;
    }
    if (ok < 0)
        s_skip++;
    else
        s_bad++;
    printf("%-8s %-18s %s ", kind, name, ok < 0 ? "SANS OBJET" : "ÉCART");
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    printf("\n");
}

/* ------------------------------------------------------------ LZSS / CEL */

static void audit_cel(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    if (!raw || len < 10) {
        verdict("CEL", name, 0, "fichier illisible");
        free(raw);
        return;
    }
    clear_vm();                                         /* référence */
    uint16_t n = be16(raw);
    uint32_t table = 10 + (uint32_t)n * 10, packed = be32(raw + 2);
    uint32_t pix = DST_AT + table;
    put_bytes(DST_AT, raw, len < table ? len : table);
    if (table + packed <= len) {
        put_bytes(SRC_AT, raw + table, packed);
        ref_unpack(&s_vm, SRC_AT, packed, pix);
    }
    free(raw);
    MoonCel *c = moon_cel_load(name);
    if (!c) {
        verdict("CEL", name, 0, "bibliothèque : échec du chargement (%u frames)", n);
        return;
    }
    char why[256] = "";
    if ((uint16_t)c->frame_count != n)
        snprintf(why, sizeof why, "frames : bib %d, réf %u", c->frame_count, n);
    for (int i = 0; !*why && i < c->frame_count; i++) {
        uint32_t fe = DST_AT + 10 + (uint32_t)i * 10;
        uint32_t off = ix_rl(&s_vm, fe);
        uint16_t w = ix_rw(&s_vm, fe + 4), h = ix_rw(&s_vm, fe + 6);
        uint8_t fl = ix_rb(&s_vm, fe + 8), mask = ix_rb(&s_vm, fe + 9);
        int planes = 0;
        for (int b = 0; b < 5; b++)
            planes += (mask >> b) & 1;
        MoonCelFrame *f = &c->frames[i];
        if (f->width != w || f->height != h)
            snprintf(why, sizeof why, "frame %d : taille bib %ux%u, réf %ux%u", i,
                     f->width, f->height, w, h);
        else if (f->draw_flags != fl)
            snprintf(why, sizeof why, "frame %d : octet 8 bib %02X, réf %02X", i, f->draw_flags, fl);
        else if (f->planes != planes || f->plane_mask != mask || f->offset != off)
            snprintf(why, sizeof why, "frame %d : plans bib %u, réf %d (masque %02X)", i,
                     f->planes, planes, mask);
        else {
            size_t sz = (size_t)planes * (size_t)(((w + 15) / 16) * 2) * h;
            long d = first_diff(f->data, pix + off, sz);
            if (d >= 0)
                snprintf(why, sizeof why, "frame %d : pixels différents à l'octet %ld / %zu", i, d, sz);
        }
    }
    verdict("CEL", name, !*why, "%s", why);
    moon_cel_free(c);
}

/* Décompression LZSS seule, comparée sur un flux : src en VM, n octets */
static void audit_lzss(const char *kind, const char *name, const uint8_t *stream, size_t n)
{
    clear_vm();
    put_bytes(SRC_AT, stream, n);
    uint32_t ref = ref_unpack(&s_vm, SRC_AT, (uint32_t)n, DST_AT);
    size_t cap = (size_t)ref + 65536;
    uint8_t *out = calloc(1, cap);
    int got = moon_lzss_decompress(stream, n, out, cap);
    long d = got >= 0 ? first_diff(out, DST_AT, (size_t)(got < (int)ref ? got : (int)ref)) : -1;
    verdict(kind, name, got == (int)ref && d < 0, "LZSS : bib %d octets, réf %u%s",
            got, ref, d >= 0 ? ", contenu différent" : "");
    free(out);
}

/* ------------------------------------------------------------------ PIV */

/* PIV de l'original (LAB_0402 / LAB_0C27) à `off` dans buf ; renvoie sa
 * taille dans le fichier, 0 si ce n'est pas un PIV */
static size_t ref_piv(const uint8_t *buf, size_t len, size_t off, uint16_t pal[32],
                      int *planes, uint32_t *out_len)
{
    if (off + 6 > len)
        return 0;
    uint16_t pl = be16(buf + off);
    if (pl != 4 && pl != 5)
        return 0;
    uint32_t n = pl == 4 ? 32 : 64;
    uint32_t body = be32(buf + off + 2);
    if (off + 6 + n + body > len)
        return 0;
    for (uint32_t i = 0; i < n / 2; i++) {
        uint16_t c = be16(buf + off + 6 + 2 * i);
        pal[i] = c & 0x8000 ? (uint16_t)(c & 0x7FFF) : (uint16_t)(c << 1);
    }
    clear_vm();
    put_bytes(SRC_AT, buf + off + 6 + n, body);
    *out_len = ref_unpack(&s_vm, SRC_AT, body, DST_AT);
    *planes = pl;
    return 6 + n + body;
}

static void compare_piv(const char *kind, const char *name, const MoonPiv *p,
                        const uint16_t pal[32], int planes, uint32_t out_len)
{
    if (!p) {
        verdict(kind, name, 0, "bibliothèque : échec du chargement");
        return;
    }
    char why[256] = "";
    int n = planes == 4 ? 16 : 32;
    if (p->planes != planes)
        snprintf(why, sizeof why, "plans bib %d, réf %d", p->planes, planes);
    for (int i = 0; !*why && i < n; i++)
        if (p->palette[i] != pal[i])
            snprintf(why, sizeof why, "couleur %d : bib %03X, réf %03X", i, p->palette[i], pal[i]);
    if (!*why) {
        size_t sz = (size_t)planes * 8000;
        if (out_len < sz)
            snprintf(why, sizeof why, "réf : %u octets seulement", out_len);
        else {
            long d = first_diff(p->bitmap, DST_AT, sz);
            if (d >= 0)
                snprintf(why, sizeof why, "image : premier écart à l'octet %ld (plan %ld)", d, d / 8000);
        }
    }
    verdict(kind, name, !*why, "%s", why);
}

static void audit_piv(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    uint16_t pal[32];
    int planes = 0;
    uint32_t out_len = 0;
    if (!raw || !ref_piv(raw, len, 0, pal, &planes, &out_len)) {
        verdict("PIV", name, -1, "pas un PIV de l'original (en-tête)");
        free(raw);
        return;
    }
    MoonPiv *p = moon_piv_load(name);
    compare_piv("PIV", name, p, pal, planes, out_len);
    moon_piv_free(p);
    free(raw);
}

/* « Test » : PIV à la suite (LAB_013A) */
static void audit_testmap(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    if (!raw)
        return;
    size_t off = 0;
    for (int i = 0; off < len; i++) {
        uint16_t pal[32];
        int planes = 0;
        uint32_t out_len = 0;
        size_t sz = ref_piv(raw, len, off, pal, &planes, &out_len);
        if (!sz)
            break;
        char nm[64];
        snprintf(nm, sizeof nm, "%s[%d]", name, i);
        MoonPiv *p = moon_testmap_load_piv(name, i);
        /* la référence a été recalculée : ref_piv a rempli DST_AT */
        compare_piv("PIV", nm, p, pal, planes, out_len);
        moon_piv_free(p);
        off += sz;
    }
    free(raw);
}

/* ------------------------------------------------------------- RNC, MOD */

static IxVM s_pvm;
static uint32_t s_fast;

static void audit_cmp(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    if (!raw)
        return;
    /* référence : Unpack_Rnc1 de program, en place dans son bloc fast */
    uint32_t a0 = s_fast;
    for (size_t i = 0; i < len; i++)
        ix_wb(&s_pvm, a0 + (uint32_t)i, raw[i]);
    uint32_t ref = ref_rnc_unpack(&s_pvm, a0);
    size_t cap = (size_t)ref + 65536;
    uint8_t *out = calloc(1, cap);
    int got = moon_rnc1_decompress(raw, len, out, cap);
    long d = -1;
    if (got > 0)
        for (uint32_t i = 0; i < ref && i < (uint32_t)got; i++)
            if (out[i] != ix_rb(&s_pvm, a0 + i)) {
                d = (long)i;
                break;
            }
    verdict("RNC", name, got == (int)ref && d < 0, "bib %d octets, réf %u%s",
            got, ref, d >= 0 ? ", contenu différent" : "");
    /* MOD : champs lus par le lecteur (LAB_0061 / LAB_006D) */
    MoonMod *m = moon_mod_load(name);
    char why[256] = "";
    if (!m)
        snprintf(why, sizeof why, "bibliothèque : échec du chargement");
    else {
        uint8_t songlen = ix_rb(&s_pvm, a0 + 0x3B6);
        if (m->song_length != songlen)
            snprintf(why, sizeof why, "longueur : bib %d, réf %u", m->song_length, songlen);
        for (int i = 0; !*why && i < 128; i++)
            if (m->order[i] != ix_rb(&s_pvm, a0 + 0x3B8 + (uint32_t)i))
                snprintf(why, sizeof why, "ordre %d différent", i);
        for (int i = 0; !*why && i < 31; i++) {
            uint32_t sd = a0 + 20 + (uint32_t)i * 30;
            const MoonModSample *s = &m->samples[i];
            if (s->length_words != ix_rw(&s_pvm, sd + 22) || s->volume != ix_rb(&s_pvm, sd + 25)
                || s->repeat_offset_words != ix_rw(&s_pvm, sd + 26)
                || s->repeat_length_words != ix_rw(&s_pvm, sd + 28))
                snprintf(why, sizeof why, "instrument %d différent", i + 1);
        }
    }
    verdict("MOD", name, !*why, "%s", why);
    moon_mod_free(m);
    free(out);
    free(raw);
}

/* ------------------------------------------------------ stile, .a, .t */

static void audit_stile(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    MoonStile *s = moon_stile_load(name);
    char why[256] = "";
    if (!s)
        snprintf(why, sizeof why, "bibliothèque : échec du chargement");
    else if (s->size != len || memcmp(s->data, raw, len))
        snprintf(why, sizeof why, "bib : %zu octets ; l'original lit les %zu octets bruts "
                 "(carte de tuiles, mots)", s->size, len);
    else
        for (size_t i = 0; !*why && i < len / 2; i++)
            if (moon_stile_tile(s, i) != be16(raw + 2 * i))
                snprintf(why, sizeof why, "tuile %zu différente", i);
    verdict("STILE", name, !*why, "%s", why);
    moon_stile_free(s);
    free(raw);
}

static void audit_sfx(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    MoonSfx *s = moon_sfx_load(name);
    verdict("SFX", name, s && s->size == len && !memcmp(s->data, raw, len),
            "bib %zu octets, fichier %zu", s ? s->size : 0, len);
    moon_sfx_free(s);
    free(raw);
}

static void audit_terrain(const char *name)
{
    size_t len;
    uint8_t *raw = moon_file_read(name, &len);
    if (!raw || len < 4)
        return;
    uint32_t body = be32(raw);
    if (body + 4 > len) {
        verdict("TERRAIN", name, -1, "pas un terrain (taille %u > fichier %zu)", body, len);
        free(raw);
        return;
    }
    clear_vm();
    put_bytes(SRC_AT, raw + 4, body);
    uint32_t got = ref_unpack(&s_vm, SRC_AT, body, DST_AT);
    uint16_t n = ix_rw(&s_vm, DST_AT);
    MoonTerrain *t = moon_terrain_load(name);
    char why[256] = "";
    if (!t)
        snprintf(why, sizeof why, "bibliothèque : échec (réf : %u obstacles, %u octets)", n, got);
    else if (t->n_obstacles != n)
        snprintf(why, sizeof why, "obstacles : bib %d, réf %u", t->n_obstacles, n);
    else {
        for (int i = 0; !*why && i < n; i++) {
            uint32_t e = DST_AT + 2 + (uint32_t)i * 8;
            const MoonTerrainObstacle *o = &t->obstacles[i];
            if ((uint16_t)o->x_left != ix_rw(&s_vm, e) || (uint16_t)o->x_right != ix_rw(&s_vm, e + 2)
                || (uint16_t)o->y_depth != ix_rw(&s_vm, e + 4) || (uint16_t)o->extra != ix_rw(&s_vm, e + 6))
                snprintf(why, sizeof why, "obstacle %d différent", i);
        }
        /* objets (SECSTRT_12) : $FF.. fin, $FE.. sauté */
        uint32_t a = DST_AT + 2 + (uint32_t)n * 8;
        int k = 0;
        for (uint32_t r = 0; !*why && r < 0x960; r += 6) {
            uint16_t w = ix_rw(&s_vm, a + r);
            if ((w & 0xFF00) == 0xFF00)
                break;
            if ((w & 0xFF00) == 0xFE00)
                continue;
            if (k >= t->n_objects) {
                snprintf(why, sizeof why, "objets : bib %d, réf plus", t->n_objects);
                break;
            }
            const MoonTerrainObject *o = &t->objects[k++];
            if (o->sprite_bank != (w >> 8) || o->sprite_idx != (w & 0xFF)
                || (uint16_t)o->x != ix_rw(&s_vm, a + r + 2) || (uint16_t)o->y != ix_rw(&s_vm, a + r + 4))
                snprintf(why, sizeof why, "objet %d différent", k - 1);
        }
        if (!*why && k != t->n_objects)
            snprintf(why, sizeof why, "objets : bib %d, réf %d", t->n_objects, k);
    }
    verdict("TERRAIN", name, !*why, "%s", why);
    moon_terrain_free(t);
    free(raw);
}

/* ---------------------------------------------------------- collide.hit */

static void audit_hit(void)
{
    static IxVM mvm;
    if (mog_boot_memory(&mvm) < 0)
        return;
    mog_hit_init(&mvm);
    MoonHit *h = moon_hit_load("collide.hit");
    if (!h) {
        verdict("HIT", "collide.hit", 0, "bibliothèque : échec du chargement");
        return;
    }
    int ok_all = 1;
    for (int s = 0; s < h->sprite_count; s++) {
        const MoonHitSprite *sp = &h->sprites[s];
        uint32_t start = ix_rl(&mvm, MOG_LAB_0A4D);
        put_name(&mvm, MOG_CHIP_BLOCK + 0x100, sp->name);
        if (ref_hit_parse(&mvm, MOG_CHIP_BLOCK + 0x100, MOG_CHIP_BLOCK + 0x1000) < 0) {
            verdict("HIT", sp->name, 0, "absent pour l'original");
            ok_all = 0;
            continue;
        }
        char why[256] = "";
        uint32_t a = start;
        int f = 0;
        for (; !*why; f++) {
            if (a >= ix_rl(&mvm, MOG_LAB_0A4D))
                break;
            if (f >= sp->frame_count) {
                snprintf(why, sizeof why, "frames : bib %d, réf plus", sp->frame_count);
                break;
            }
            const MoonHitFrame *fr = &sp->frames[f];
            uint8_t np = ix_rb(&mvm, a++);
            if (fr->n_points != np) {
                snprintf(why, sizeof why, "frame %d : points bib %u, réf %u", f, fr->n_points, np);
                break;
            }
            if (!np)
                continue;
            uint8_t ty = ix_rb(&mvm, a), mx = ix_rb(&mvm, a + 1), my = ix_rb(&mvm, a + 2);
            a += 3;
            if (fr->type != ty || fr->max_dx != mx || fr->max_dy != my)
                snprintf(why, sizeof why, "frame %d : type/max bib %u/%u/%u, réf %u/%u/%u", f,
                         fr->type, fr->max_dx, fr->max_dy, ty, mx, my);
            for (int i = 0; !*why && i < np; i++, a += 2)
                if (fr->points[i].dx != ix_rb(&mvm, a) || fr->points[i].dy != ix_rb(&mvm, a + 1))
                    snprintf(why, sizeof why, "frame %d : point %d différent", f, i);
        }
        if (!*why && f != sp->frame_count)
            snprintf(why, sizeof why, "frames : bib %d, réf %d", sp->frame_count, f);
        verdict("HIT", sp->name, !*why, "%s", why);
        ok_all &= !*why;
    }
    moon_hit_free(h);
}

/* ------------------------------------------------------------------ */

static int ext_is(const char *n, const char *e)
{
    const char *d = strrchr(n, '.');
    if (!d)
        return 0;
    for (d++; *d && *e; d++, e++)
        if (tolower((unsigned char)*d) != *e)
            return 0;
    return !*d && !*e;
}

static int cmp_names(const void *a, const void *b)
{
    return strcmp(*(char *const *)a, *(char *const *)b);
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage : lib_audit données\n");
        return 2;
    }
    if (moon_init(argv[1]) != 0)
        return 1;
    s_vm.mem = calloc(1, VM_SIZE);
    s_vm.size = s_vm.heap = VM_SIZE;
    if (prog_boot_memory(&s_pvm, &s_fast) < 0)
        return 1;

    DIR *d = opendir(argv[1]);
    if (!d)
        return 1;
    char *names[512];
    int n = 0;
    struct dirent *e;
    while ((e = readdir(d)) && n < 512)
        if (e->d_name[0] != '.')
            names[n++] = strdup(e->d_name);
    closedir(d);
    qsort(names, (size_t)n, sizeof *names, cmp_names);

    for (int i = 0; i < n; i++) {
        const char *nm = names[i];
        if (ext_is(nm, "cel") || ext_is(nm, "ob") || ext_is(nm, "c")
            || ext_is(nm, "f") || ext_is(nm, "font"))
            audit_cel(nm);
        else if (ext_is(nm, "piv") || ext_is(nm, "p"))
            audit_piv(nm);
        else if (ext_is(nm, "cmp"))
            audit_cmp(nm);
        else if (ext_is(nm, "stile"))
            audit_stile(nm);
        else if (ext_is(nm, "a"))
            audit_sfx(nm);
        else if (ext_is(nm, "t"))
            audit_terrain(nm);
        else if (!strcasecmp(nm, "test"))
            audit_testmap(nm);
        else if (!strcasecmp(nm, "mindscape")) {
            audit_piv(nm);
        } else if (!strcasecmp(nm, "collide.hit"))
            audit_hit();
        else
            printf("%-8s %-18s non audité (format inconnu)\n", "?", nm);
    }
    /* flux LZSS seuls : le corps de chaque CEL */
    for (int i = 0; i < n; i++)
        if (ext_is(names[i], "cel") || ext_is(names[i], "ob") || ext_is(names[i], "c")
            || ext_is(names[i], "f") || ext_is(names[i], "font")) {
            size_t len;
            uint8_t *raw = moon_file_read(names[i], &len);
            if (raw && len > 10) {
                size_t start = 10 + (size_t)be16(raw) * 10;
                uint32_t comp = be32(raw + 2);
                if (start + comp <= len)
                    audit_lzss("LZSS", names[i], raw + start, comp);
            }
            free(raw);
        }
    printf("\n");
    for (int i = 0; s_kinds[i].kind; i++)
        printf("%-8s %3d identiques, %3d écarts\n", s_kinds[i].kind, s_kinds[i].ok, s_kinds[i].bad);
    printf("\n%d vérifications : %d identiques, %d écarts, %d sans objet\n",
           s_files, s_ok, s_bad, s_skip);
    return 0;
}
