/*
 * ix_view.c — visionneuse du moteur IMAGEXCEL C (game/src/ix_engine.c)
 * avec les vraies données du jeu.
 *
 * Le chevalier est monté comme dans mog : banques CEL LAB_05E1 (kn1..kn3,
 * kn4, Kn5.ob, cf. LAB_0115 / LAB_0120), objet rempli comme LAB_0167,
 * scripts de repos, de marche (LAB_0610) et d'attaque (LAB_05F5).
 *
 *   moon-ix-view <dossier_données>                 fenêtre SDL
 *   moon-ix-view <dossier_données> --png sortie.png [images]
 *                                                  planche d'images, sans écran
 *                                                  (IXV_FULL=1 : écran entier)
 *
 * Touches : 1-8 attaques, flèches marche, espace inverse le sens,
 *           N parcourt les 259 scripts de mog, Échap quitte.
 */
#include "ix_engine.h"
#include "ix_data.h"
#include "ix_mog_syms.h"
#include "moon_assets.h"
#include "moon_hal.h"
#include "moon_render.h"

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Banques CEL : poignée (adresse virtuelle) -> sprites décodés         */
/* ------------------------------------------------------------------ */

#define MAX_CELS 16

typedef struct {
    uint32_t  handle;
    MoonCel  *cel;
    char      name[32];
} CelSlot;

typedef struct {
    IxVM      vm;
    IxEngine  eng;
    CelSlot   cels[MAX_CELS];
    int       ncels;
    uint32_t  palette[32];
    uint32_t  bg[GAME_W * GAME_H];      /* décor + dessins permanents */
    uint32_t  fb[GAME_W * GAME_H];
    uint32_t  knight_obj;
    uint32_t  knight;                   /* entité */
    int       browse;                   /* index de script parcouru (-1 : non) */
    unsigned long draws, missing;
} View;

static const CelSlot *find_cel(const View *v, uint32_t handle)
{
    for (int i = 0; i < v->ncels; i++)
        if (v->cels[i].handle == handle)
            return &v->cels[i];
    return NULL;
}

/* Charge une CEL et lui attribue une poignée dans la mémoire virtuelle. */
static uint32_t load_cel(View *v, const char *name)
{
    if (v->ncels == MAX_CELS)
        return 0;
    MoonCel *cel = moon_cel_load(name);
    if (!cel) {
        fprintf(stderr, "ix-view : %s introuvable\n", name);
        return 0;
    }
    CelSlot *s = &v->cels[v->ncels++];
    s->cel = cel;
    s->handle = ix_vm_alloc(&v->vm, 16);
    snprintf(s->name, sizeof s->name, "%s", name);
    return s->handle;
}

/* ------------------------------------------------------------------ */
/* Hôte du moteur                                                       */
/* ------------------------------------------------------------------ */

static int h_frame_info(void *u, uint32_t cel, int frame, int *w, int *h)
{
    const CelSlot *s = find_cel(u, cel);
    if (!s || frame >= s->cel->frame_count)
        return 0;
    *w = s->cel->frames[frame].width;
    *h = s->cel->frames[frame].height;
    return 1;
}

static void h_draw(void *u, uint32_t cel, int frame, int x, int y, int flipped, int bg)
{
    View *v = u;
    const CelSlot *s = find_cel(v, cel);
    if (!s || frame >= s->cel->frame_count) {
        v->missing++;
        return;
    }
    v->draws++;
    render_cel_frame(&s->cel->frames[frame], v->palette, bg ? v->bg : v->fb,
                     (int16_t)x, (int16_t)y, BLIT_MASK | (flipped ? BLIT_FLIP_X : 0));
}

static void h_sound(void *u, int n) { (void)u; (void)n; }
static void h_call(void *u, uint32_t r, uint32_t en) { (void)u; (void)r; (void)en; }
static void h_message(void *u, const char *t) { (void)u; fprintf(stderr, "ix : %s\n", t); }

static const IxHost host = { NULL, h_frame_info, h_draw, h_sound, h_call, h_message };

/* ------------------------------------------------------------------ */
/* Chevalier                                                            */
/* ------------------------------------------------------------------ */

static const uint32_t attacks[9] = {       /* LAB_05F5 (posée par LAB_0167) */
    MOG_LAB_07DB, MOG_LAB_07EF, MOG_LAB_07ED, MOG_LAB_07EA, MOG_LAB_07F4,
    MOG_LAB_07E9, MOG_LAB_07F1, MOG_LAB_07F3, MOG_LAB_07EE
};
static const uint32_t walk[3][4] = {       /* LAB_0610 : horizontal, haut, bas */
    { MOG_LAB_07DD, MOG_LAB_07DE, MOG_LAB_07DF, MOG_LAB_07E0 },
    { MOG_LAB_07E1, MOG_LAB_07E2, MOG_LAB_07E3, MOG_LAB_07E4 },
    { MOG_LAB_07E5, MOG_LAB_07E6, MOG_LAB_07E7, MOG_LAB_07E8 },
};

static int setup(View *v)
{
    IxLayout lay;
    if (ix_vm_load(&v->vm, &ix_mog_image, 1u << 20) < 0)
        return -1;
    ix_layout_mog(&lay);
    IxHost *h = malloc(sizeof *h);
    *h = host;
    h->user = v;
    ix_engine_init(&v->eng, &v->vm, h, &lay);
    ix_reset_entities(&v->eng);

    /* LAB_0115 / LAB_0120 : banques du chevalier */
    static const char *kn[5] = { "kn1.ob", "kn2.ob", "kn3.ob", "kn4.ob", "Kn5.ob" };
    for (int i = 0; i < 5; i++)
        ix_wl(&v->vm, MOG_LAB_05E1 + 4u * (uint32_t)i, load_cel(v, kn[i]));

    /* 20 objets (LAB_05C3) pour Ent_Spawn / $B8 */
    uint32_t objs = ix_vm_alloc(&v->vm, 20 * IX_OBJECT_SIZE);
    ix_wl(&v->vm, MOG_LAB_05C3, objs);

    v->knight = ix_spawn(&v->eng, MOG_LAB_07DB, MOG_LAB_05E1, 110, 0, 150, 0, 0);
    v->knight_obj = ix_rl(&v->vm, v->knight + 24);
    uint32_t o = v->knight_obj;                         /* LAB_0167 */
    ix_wl(&v->vm, o + 22, MOG_LAB_07DB);
    ix_wl(&v->vm, o + 26, MOG_LAB_07DC);
    ix_ww(&v->vm, o + 116, 100);
    ix_ww(&v->vm, o + 120, 4);
    ix_ww(&v->vm, o + 118, 80);

    MoonPiv *piv = moon_piv_load("ch.piv");
    if (piv) {
        render_piv_full(piv, v->bg);
        render_build_palette(piv->palette, 1 << piv->planes, v->palette);
        moon_piv_free(piv);
    } else {
        for (int i = 0; i < 32; i++)
            v->palette[i] = 0xFF000000u | (uint32_t)(i * 0x080808);
        for (int i = 0; i < GAME_W * GAME_H; i++)
            v->bg[i] = 0xFF203040u;
    }
    return 0;
}

/* Relance le chevalier sur un script (l'entité garde position et sens). */
static void play(View *v, uint32_t script)
{
    uint32_t en = v->knight;
    ix_wl(&v->vm, en + 2, script);
    for (uint32_t i = 0; i < IX_CTX_SIZE; i++)
        ix_wb(&v->vm, ix_rl(&v->vm, en + 36) + i, 0);
    ix_wb(&v->vm, en + 1, 1);
}

static int busy(View *v) { return ix_rb(&v->vm, v->knight + 1); }

/* Une image du combat : 6 VBL, listes vidées, scripts exécutés. */
static void frame(View *v)
{
    IxEngine *e = &v->eng;
    ix_wl(&v->vm, e->lay.vbl_counter, ix_rl(&v->vm, e->lay.vbl_counter) + 6);
    ix_clear_frame_lists(e);
    ix_wl(&v->vm, e->lay.scrap_ptr, MOG_LAB_064E);   /* LAB_0416 : LAB_063E -> LAB_0641 */
    ix_ww(&v->vm, e->lay.scrap_count, 0);
    memcpy(v->fb, v->bg, sizeof v->fb);
    ix_run_entities(e);
    /* le tri par profondeur déplace les entités : on suit l'objet */
    v->knight = ix_find_entity(e, v->knight_obj);
}

/* ------------------------------------------------------------------ */
/* PNG (blocs « stored », sans dépendance)                              */
/* ------------------------------------------------------------------ */

static uint32_t crc_tab[256];

static uint32_t crc(uint32_t c, const uint8_t *p, size_t n)
{
    if (!crc_tab[1])
        for (uint32_t i = 0; i < 256; i++) {
            uint32_t k = i;
            for (int j = 0; j < 8; j++)
                k = (k >> 1) ^ (0xEDB88320u & (0u - (k & 1u)));
            crc_tab[i] = k;
        }
    c = ~c;
    while (n--)
        c = crc_tab[(c ^ *p++) & 0xFF] ^ (c >> 8);
    return ~c;
}

static void be32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24); p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);  p[3] = (uint8_t)v;
}

static void chunk(FILE *f, const char *type, const uint8_t *d, uint32_t n)
{
    uint8_t b[4];
    be32(b, n);
    fwrite(b, 1, 4, f);
    uint32_t c = crc(0, (const uint8_t *)type, 4);
    c = crc(c, d, n);
    fwrite(type, 1, 4, f);
    fwrite(d, 1, n, f);
    be32(b, c);
    fwrite(b, 1, 4, f);
}

static int write_png(const char *path, const uint32_t *px, int w, int h)
{
    FILE *f = fopen(path, "wb");
    if (!f)
        return -1;
    static const uint8_t sig[8] = { 137, 80, 78, 71, 13, 10, 26, 10 };
    fwrite(sig, 1, 8, f);
    uint8_t ihdr[13] = { 0 };
    be32(ihdr, (uint32_t)w);
    be32(ihdr + 4, (uint32_t)h);
    ihdr[8] = 8; ihdr[9] = 2;                           /* RGB 8 bits */
    chunk(f, "IHDR", ihdr, 13);

    size_t raw_n = (size_t)h * (size_t)(w * 3 + 1);
    uint8_t *raw = malloc(raw_n);
    for (int y = 0; y < h; y++) {
        uint8_t *r = raw + (size_t)y * (size_t)(w * 3 + 1);
        *r++ = 0;
        for (int x = 0; x < w; x++) {
            uint32_t c = px[(size_t)y * (size_t)w + (size_t)x];
            *r++ = (uint8_t)(c >> 16); *r++ = (uint8_t)(c >> 8); *r++ = (uint8_t)c;
        }
    }
    size_t blocks = (raw_n + 65534) / 65535;
    size_t z_n = 2 + raw_n + blocks * 5 + 4;
    uint8_t *z = malloc(z_n), *q = z;
    *q++ = 0x78; *q++ = 0x01;
    uint32_t a = 1, b = 0;
    for (size_t off = 0; off < raw_n; off += 65535) {
        size_t n = raw_n - off < 65535 ? raw_n - off : 65535;
        *q++ = off + n == raw_n;
        *q++ = (uint8_t)n; *q++ = (uint8_t)(n >> 8);
        *q++ = (uint8_t)~n; *q++ = (uint8_t)(~n >> 8);
        memcpy(q, raw + off, n);
        q += n;
        for (size_t i = 0; i < n; i++) {
            a = (a + raw[off + i]) % 65521u;
            b = (b + a) % 65521u;
        }
    }
    be32(q, (b << 16) | a);
    chunk(f, "IDAT", z, (uint32_t)z_n);
    chunk(f, "IEND", NULL, 0);
    free(raw);
    free(z);
    return fclose(f);
}

/* Planche : le chevalier enchaîne repos, marche puis chaque attaque ; une
 * vignette 160×100 (autour du chevalier) par image. */
static int sheet(View *v, const char *path, int frames)
{
    const int full = getenv("IXV_FULL") != NULL;
    const int TW = full ? GAME_W : 160, TH = full ? GAME_H : 100, COLS = full ? 4 : 8;
    int rows = (frames + COLS - 1) / COLS;
    uint32_t *out = calloc((size_t)TW * TH * COLS * (size_t)rows, 4);
    int seq = 0, n = 0;
    play(v, attacks[0]);
    for (int i = 0; i < frames; i++) {
        if (!busy(v) || n++ > 40) {                     /* script fini */
            seq++;
            n = 0;
            if (seq <= 4)
                play(v, walk[0][(seq - 1) & 3]);
            else if (seq - 5 < 8)
                play(v, attacks[seq - 4]);
            else
                play(v, attacks[0]);
        }
        frame(v);
        int ox = full ? 0 : 30, oy = full ? 0 : 100;
        int tx = (i % COLS) * TW, ty = (i / COLS) * TH;
        for (int y = 0; y < TH; y++)
            for (int x = 0; x < TW; x++)
                out[(size_t)(ty + y) * TW * COLS + (size_t)(tx + x)] =
                    v->fb[(oy + y) * GAME_W + ox + x];
    }
    int r = write_png(path, out, TW * COLS, TH * rows);
    free(out);
    printf("%s : %d images, %lu dessins, %lu frames inconnues, %lu erreurs moteur, %lu défauts mémoire\n",
           path, frames, v->draws, v->missing, v->eng.errors, ix_vm_faults);
    return r;
}

static int interactive(View *v)
{
    if (hal_init("Moonstone — moteur IMAGEXCEL", 3) < 0)
        return 1;
    MoonInput in;
    int acc = 0, phase = 0, last_space = 0, last_s = 0;
    uint32_t t0 = hal_ticks();
    for (;;) {
        if (hal_poll(&in) || in.escape || in.quit)
            break;
        const MoonJoy *j = &in.joy[0];
        if (in.space && !last_space)
            ix_wb(&v->vm, v->knight + 22, ix_rb(&v->vm, v->knight + 22) ^ 2);
        last_space = in.space;
        int s = in.keys && in.keys[SDL_SCANCODE_N];
        if (s && !last_s) {
            v->browse = (v->browse + 1) % ix_mog_image.script_count;
            play(v, ix_mog_image.scripts[v->browse]);
            printf("script %d : $%08X\n", v->browse, ix_mog_image.scripts[v->browse]);
        }
        last_s = s;
        for (int k = 0; k < 8 && in.keys; k++)
            if (in.keys[SDL_SCANCODE_1 + k] && !busy(v))
                play(v, attacks[k + 1]);
        if (!busy(v)) {
            if (j->left || j->right) {
                ix_wb(&v->vm, v->knight + 22, j->left ? 2 : 0);
                play(v, walk[0][phase++ & 3]);
            } else if (j->up || j->down) {
                play(v, walk[j->up ? 1 : 2][phase++ & 3]);
            } else if (v->browse < 0) {
                play(v, attacks[0]);
            }
        }
        /* une image du combat toutes les 6 VBL (v_FrameVbls) */
        uint32_t now = hal_ticks();
        acc += (int)(now - t0);
        t0 = now;
        if (acc >= 120) {
            acc -= 120;
            if (acc > 240)
                acc = 0;
            frame(v);
        }
        hal_present(v->fb);
        hal_vbl_wait();
    }
    hal_quit();
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "usage : moon-ix-view dossier_données [--png fichier.png [images]]\n");
        return 2;
    }
    if (moon_init(argv[1]) != 0) {
        fprintf(stderr, "ix-view : dossier %s illisible\n", argv[1]);
        return 1;
    }
    static View v;
    v.browse = -1;
    if (setup(&v) < 0 || !v.knight)
        return 1;
    if (argc > 3 && !strcmp(argv[2], "--png"))
        return sheet(&v, argv[3], argc > 4 ? atoi(argv[4]) : 96) != 0;
    return interactive(&v);
}
