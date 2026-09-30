/*
 * mog_game.c — le jeu de mog complet, porté en C (voir mog_game.h).
 *
 * La VBL de l'original (interruption) fait ici, dans l'ordre : compteur
 * v_VblCounter, serveur des couleurs LAB_0E5D (fondus, rotations,
 * pulsations), entrées (joysticks, clavier -> SECSTRT_21 / LAB_0B91),
 * image montrée (plans pointés par la copper list, sprite du pointeur),
 * attente de 1/50 s chez l'hôte ; puis mog_wait_vbls mène le pointeur
 * (LAB_057D).
 */
#include "mog_game.h"
#include "mog_boot.h"
#include "mog_encounter.h"
#include "mog_map.h"
#include "mog_screens.h"
#include "mog_vbl.h"
#include "mog_sound.h"
#include "mog_private.h"
#include "ix_mog_syms.h"

#include <string.h>
#include <stdio.h>

#define VM (&g->vm)

/* ------------------------------------------------------------------ */
/* Image                                                               */
/* ------------------------------------------------------------------ */

static uint32_t argb(uint16_t c)
{
    return 0xFF000000u | (uint32_t)((c >> 8) & 15) * 0x110000u
         | (uint32_t)((c >> 4) & 15) * 0x1100u | (uint32_t)(c & 15) * 0x11u;
}

/* Sprite 0 (pointeur, 2 plans, couleurs 17 à 19) en (LAB_097F, LAB_0980) */
static void draw_pointer(MogGame *g)
{
    if (!ix_rw(VM, MOG_LAB_097C))
        return;
    uint32_t s = ix_rl(VM, MOG_LAB_097D);
    int h = ix_rw(VM, s);
    int x0 = (int16_t)ix_rw(VM, MOG_LAB_097F), y0 = (int16_t)ix_rw(VM, MOG_LAB_0980);
    for (int r = 0; r < h; r++) {
        int y = y0 + r;
        if (y < 0 || y >= MOG_GAME_H)
            continue;
        uint16_t a = ix_rw(VM, s + 6 + 4u * (unsigned)r);
        uint16_t b = ix_rw(VM, s + 8 + 4u * (unsigned)r);
        for (int k = 0; k < 16; k++) {
            int x = x0 + k;
            if (x < 0 || x >= MOG_GAME_W)
                continue;
            int c = ((a >> (15 - k)) & 1) | ((b >> (15 - k)) & 1) << 1;
            if (c)
                g->fb[y * MOG_GAME_W + x] = argb(g->colour[16 + c]);
        }
    }
}

void mog_game_render(MogGame *g)
{
    mog_screen(VM, g->colour, g->fb);
    draw_pointer(g);
}

/* ------------------------------------------------------------------ */
/* VBL                                                                 */
/* ------------------------------------------------------------------ */

/* Caractère -> code de touche de mog (indice dans LAB_0D99) */
static int key_code(MogGame *g, int c)
{
    if (c >= 'a' && c <= 'z')
        c -= 32;
    for (uint32_t i = 1; i < 0x60; i++)
        if (ix_rb(VM, MOG_LAB_0D99 + i) == (uint8_t)c)
            return (int)i;
    return 0;
}

/* Une VBL de son : serveur du pilote (LAB_0F73), puis 1/50 s de Paula */
static void sound_vbl(MogGame *g)
{
    if (!g->m.audio)
        return;
    mog_snd_vbl(&g->m);
    static int16_t buf[2 * 4096];
    int n = (g->audio_rate + g->audio_frac) / 50;
    g->audio_frac = (g->audio_rate + g->audio_frac) % 50;
    if (n > 4096)
        n = 4096;
    mog_snd_mix(&g->m, buf, n, g->audio_rate);
    g->audio(g->user, buf, n);
}

static void vbl(void *u)
{
    MogGame *g = u;
    ix_wl(VM, MOG_v_VblCounter, ix_rl(VM, MOG_v_VblCounter) + 1);
    mog_vbl_colours(VM, NULL);                          /* LAB_0E5D */
    uint32_t pal = ix_rl(VM, MOG_LAB_0E93);             /* registres couleur */
    for (int i = 0; i < 32; i++)
        g->colour[i] = ix_rw(VM, pal + 2u * (unsigned)i);
    sound_vbl(g);                                       /* LAB_0F73 */
    if (!g->vbl)
        return;
    mog_game_render(g);
    MogGameInput in = { { 0, 0 }, 0, 0 };
    g->vbl(g->user, g, &in);
    g->m.joy[0] = in.joy[0];
    g->m.joy[1] = in.joy[1];
    if (in.key) {                                       /* LAB_0B66 : touche appuyée */
        int k = key_code(g, in.key);
        if (k) {
            ix_ww(VM, MOG_SECSTRT_21, (uint16_t)k);
            ix_wb(VM, MOG_LAB_0B91 + (uint32_t)k, 1);
        }
    }
    if (in.quit)
        g->quit = 1;
}

/* Attente active de l'original : les interruptions continuent (une VBL,
 * sans le pointeur, que mog_wait_vbls mène d'ordinaire). */
static void idle(void *u)
{
    MogGame *g = u;
    vbl(g);
    mog_screen_vbl(&g->m);                              /* LAB_057D */
}

static void message(void *u, const char *t) { (void)u; (void)t; }

/* ------------------------------------------------------------------ */
/* Démarrage                                                           */
/* ------------------------------------------------------------------ */

int mog_game_boot(MogGame *g)
{
    memset(&g->vm, 0, sizeof g->vm);
    if (mog_boot_memory(VM) < 0)
        return -1;
    /* LAB_04A5 : graine du hasard selon le faisceau (VHPOSR & 3) */
    ix_wl(VM, MOG_LAB_0973, ix_rl(VM, MOG_LAB_0974 + 4u * (unsigned)(g->seed & 3)));
    mog_boot_graphics(VM);                              /* SECSTRT_30, SECSTRT_28 */
    ix_wl(VM, MOG_LAB_0E93, MOG_LAB_08D6);              /* LAB_0E53 : palette courante */
    mog_boot_ui(VM);                                    /* LAB_012C */
    mog_boot_engine(VM);                                /* LAB_0303 */
    mog_boot_map(VM);                                   /* LAB_0128 */
    mog_hit_init(VM);
    mog_boot_knight_cels(VM);                           /* LAB_0115 */
    mog_boot_backgrounds(VM);                           /* LAB_013A */
    ix_ww(VM, MOG_LAB_05C5, 1);                         /* un joueur */
    mog_boot_tables(VM);                                /* LAB_0152 / LAB_0156 */

    static const IxHost host = { NULL, NULL, NULL, NULL, NULL, message };
    IxHost h = host;
    h.user = g;
    mog_combat_init(&g->m, VM, &h);
    g->m.wait_vbl = vbl;
    g->m.idle = g->vbl ? idle : NULL;
    static MogAudio audio;                              /* Paula */
    if (g->audio && g->audio_rate > 0) {
        memset(&audio, 0, sizeof audio);
        g->m.audio = &audio;
    }
    mog_snd_init(&g->m);                                /* LAB_0AA7 : LAB_0F89 */
    mog_snd_relocate(&g->m);                            /*   LAB_0FD4 */
    mog_pointer_boot(&g->m);                            /* LAB_0572 */

    MogCombat *m = &g->m;
    mog_new_game_full(m);                               /* LAB_01AE */
    for (uint32_t i = 0; i < 4; i++)                    /* LAB_0011 */
        mog_update_knight(m, MOG_LAB_0613 + i * IX_OBJECT_SIZE);
    ix_wl(VM, MOG_LAB_06B4, MOG_LAB_06B6);              /* nom du chevalier 1 */
    ix_wl(VM, MOG_LAB_0613 + 54, 0);                    /* chevalier 1 : joueur */
    ix_wb(VM, MOG_LAB_0613 + 11, 2);                    /* joystick (port 1) */
    mog_new_game_players(m);                            /* LAB_01BE */
    mog_boot_reactions(VM);                             /* LAB_020F */
    mog_fade_out(m);                                    /* LAB_03F1 */
    mog_back_to_map(m);                                 /* SECSTRT_36 */
    return 0;
}

/* ------------------------------------------------------------------ */
/* Partie                                                              */
/* ------------------------------------------------------------------ */

int mog_game_run(MogGame *g)
{
    MogCombat *m = &g->m;
    mog_map_enter(m);                                   /* LAB_0DAB */
    while (!g->quit) {
        int ev = mog_map_frame(m);
        if (ev == MOG_MAP_ENTER)
            mog_map_enter(m);
        else if (ev != MOG_MAP_CONTINUE)
            return ev;
    }
    return -1;
}
