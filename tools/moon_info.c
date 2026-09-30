/*
 * moon-info — describe Moonstone game files, read through libmoon_assets.
 *
 * Usage: moon-info [-v] <file> [file ...]
 *
 * The kind of a file comes from its name (moon_file_kind).  For each kind:
 *   CEL      frames (size, plane mask, flags)
 *   PIV      size, planes and palette (colour swatches on a terminal)
 *   module   title, order list, patterns, samples
 *   stile    number of tiles
 *   sound    bank size
 *   terrain  obstacles and placed objects
 *   test     the pictures of the container
 *   hit      hit-box sections (sprite, frames)
 * -v lists every frame, obstacle, object, sample or section.
 */
#include "moon_tool.h"
#include "moon_testmap.h"

#include <stdlib.h>
#include <unistd.h>

static int verbose, ansi;

static void palette(const uint16_t *pal, int n)
{
    for (int i = 0; i < n; i++) {
        uint32_t rgb = tool_rgb(pal[i]);
        printf("  %2d  $%03X  #%06X", i, (unsigned)pal[i], (unsigned)rgb);
        if (ansi)
            printf("  \033[48;2;%u;%u;%um    \033[0m", (unsigned)(rgb >> 16),
                   (unsigned)(rgb >> 8 & 255), (unsigned)(rgb & 255));
        printf("\n");
    }
}

static void info_cel(const char *name)
{
    MoonCel *cel = moon_cel_load(name);
    if (!cel) {
        printf("  (not decoded)\n");
        return;
    }
    int w = 0, h = 0;
    for (int i = 0; i < cel->frame_count; i++) {
        if (cel->frames[i].width > w) w = cel->frames[i].width;
        if (cel->frames[i].height > h) h = cel->frames[i].height;
    }
    printf("  frames  : %d (up to %dx%d)\n", cel->frame_count, w, h);
    if (verbose)
        for (int i = 0; i < cel->frame_count; i++) {
            const MoonCelFrame *f = &cel->frames[i];
            printf("  [%3d] %3dx%-3d planes $%02X flags $%02X\n",
                   i, f->width, f->height, f->plane_mask, f->draw_flags);
        }
    moon_cel_free(cel);
}

static void info_piv(const MoonPiv *piv)
{
    printf("  picture : %dx%d, %d planes\n", piv->width, piv->height, piv->planes);
    palette(piv->palette, 1 << piv->planes);
}

static void info_mod(const char *name)
{
    MoonMod *mod = moon_mod_load(name);
    if (!mod) {
        printf("  (not decoded)\n");
        return;
    }
    int used = 0;
    for (int i = 0; i < mod->sample_count; i++)
        if (mod->samples[i].length_words)
            used++;
    printf("  title   : \"%s\"\n", mod->title);
    printf("  order   : %u positions, %u patterns\n",
           (unsigned)mod->song_length, (unsigned)mod->pattern_count);
    printf("  samples : %d used\n", used);
    if (verbose)
        for (int i = 0; i < mod->sample_count; i++) {
            const MoonModSample *s = &mod->samples[i];
            if (!s->length_words)
                continue;
            printf("  %2d %-22s %5u words  vol %2u  fine %+d  loop %u+%u\n", i + 1,
                   s->name, s->length_words, s->volume, s->finetune,
                   s->repeat_offset_words, s->repeat_length_words);
        }
    moon_mod_free(mod);
}

static void info_terrain(const char *name)
{
    MoonTerrain *t = moon_terrain_load(name);
    if (!t) {
        printf("  (not decoded)\n");
        return;
    }
    printf("  terrain : %d obstacles, %d objects\n", t->n_obstacles, t->n_objects);
    if (verbose) {
        for (int i = 0; i < t->n_obstacles; i++)
            printf("  obstacle x %d..%d depth %d\n", t->obstacles[i].x_left,
                   t->obstacles[i].x_right, t->obstacles[i].y_depth);
        for (int i = 0; i < t->n_objects; i++)
            printf("  object   bank $%02X #%u at (%d, %d)\n", t->objects[i].sprite_bank,
                   t->objects[i].sprite_idx, t->objects[i].x, t->objects[i].y);
    }
    moon_terrain_free(t);
}

static void info_testmap(const char *name)
{
    for (int i = 0;; i++) {
        MoonPiv *piv = moon_testmap_load_piv(name, i);
        if (!piv)
            break;
        printf(" picture %d\n", i);
        info_piv(piv);
        moon_piv_free(piv);
    }
}

static void info_hit(const char *name)
{
    MoonHit *hit = moon_hit_load(name);
    if (!hit) {
        printf("  (not decoded)\n");
        return;
    }
    printf("  sections: %d\n", hit->sprite_count);
    if (verbose)
        for (int i = 0; i < hit->sprite_count; i++)
            printf("  %-16s %d frames\n", hit->sprites[i].name, hit->sprites[i].frame_count);
    moon_hit_free(hit);
}

static void info(const char *path)
{
    char name[256];
    tool_open(path, name, sizeof name);
    size_t size = 0;
    uint8_t *raw = moon_file_read(name, &size);
    MoonFileKind kind = moon_file_kind(name);
    printf("%s  (%zu bytes) : %s\n", path, size, tool_kind_name(kind));
    if (!raw) {
        printf("  (cannot read)\n\n");
        return;
    }
    switch (kind) {
    case MOON_KIND_CEL:
        info_cel(name);
        break;
    case MOON_KIND_PIV: {
        MoonPiv *piv = moon_piv_load_from_buffer(raw, size);
        if (piv)
            info_piv(piv);
        else
            printf("  (not decoded)\n");
        moon_piv_free(piv);
        break;
    }
    case MOON_KIND_MOD:
        info_mod(name);
        break;
    case MOON_KIND_STILE:
        printf("  tiles   : %zu\n", size / 2);
        break;
    case MOON_KIND_SFX:
        printf("  samples : %zu bytes of 8-bit PCM\n", size);
        break;
    case MOON_KIND_TERRAIN:
        info_terrain(name);
        break;
    case MOON_KIND_TESTMAP:
        info_testmap(name);
        break;
    case MOON_KIND_HIT:
        info_hit(name);
        break;
    default:
        printf("  magic   : %02X %02X %02X %02X\n", raw[0], size > 1 ? raw[1] : 0,
               size > 2 ? raw[2] : 0, size > 3 ? raw[3] : 0);
    }
    free(raw);
    printf("\n");
}

int main(int argc, char *argv[])
{
    int i = 1;
    if (i < argc && !strcmp(argv[i], "-v"))
        verbose = 1, i++;
    if (i >= argc) {
        fprintf(stderr, "Usage: moon-info [-v] <file> [file ...]\n");
        return 1;
    }
    ansi = isatty(STDOUT_FILENO);
    for (; i < argc; i++)
        info(argv[i]);
    return 0;
}
