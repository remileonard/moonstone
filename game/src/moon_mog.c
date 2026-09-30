/*
 * moon_mog.c — le jeu complet de mog porté en C (mog_game.c) dans la
 * fenêtre SDL : une VBL = image présentée, 1/50 s, entrées lues.
 *
 * Au lancement, l'intro de l'original (program, prog_intro.c) : une touche
 * pendant le générique la saute (comme l'original), Entrée ou le bouton
 * de la manette la fait défiler sans attendre.
 *
 * Commandes : flèches (ou manette) = joystick, Espace / Ctrl / bouton de
 * la manette = feu ; clavier de l'Amiga : lettres et chiffres (nom des
 * chevaliers, E = fin du tour, 1-9 = choix d'un lieu, Q = abandon), Tab =
 * barre d'espace (inventaire sur la carte, pause en combat), Entrée,
 * retour arrière ; Échap = quitter.
 */
#include "moon_game.h"
#include "moon_hal.h"
#include "mog_game.h"
#include "mog_map.h"
#include "prog_intro.h"
#include "prog_vbl.h"
#include "ix_program_syms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {                                  /* codes SDL_Scancode */
    SC_A = 4, SC_D = 7, SC_E = 8, SC_F = 9, SC_S = 22, SC_W = 26, SC_I = 12, SC_Q = 20, SC_Z = 29, SC_1 = 30, SC_9 = 38,
    SC_0 = 39, SC_RETURN = 40, SC_BACKSPACE = 42, SC_TAB = 43, SC_MINUS = 45, SC_SPACE = 44, SC_RIGHT = 79, SC_LEFT = 80, SC_DOWN = 81,
    SC_UP = 82, SC_LCTRL = 224, SC_RCTRL = 228
};

static MogGame s_game;
static uint8_t s_prev[512];

static int down(const MoonInput *in, int sc)
{
    return in->keys && sc < in->n_keys && in->keys[sc];
}

/* Touche nouvellement appuyée (front montant) */
static int pressed(const MoonInput *in, int sc)
{
    int d = down(in, sc);
    int p = d && !s_prev[sc];
    s_prev[sc] = (uint8_t)d;
    return p;
}

static void host_vbl(void *user, MogGame *g, MogGameInput *out)
{
    GameCtx *ctx = user;
    hal_present(g->fb);
    hal_vbl_wait();
    MoonInput *in = &ctx->input;
    if (hal_poll(in) && (in->quit || in->escape)) {
        hal_quit();
        exit(0);
    }
    const MoonJoy *j = &in->joy[0];
    uint16_t v = 0;
    if (j->right || down(in, SC_RIGHT)) v |= MOG_JOY_RIGHT;
    if (j->left || down(in, SC_LEFT))   v |= MOG_JOY_LEFT;
    if (j->down || down(in, SC_DOWN))   v |= MOG_JOY_DOWN;
    if (j->up || down(in, SC_UP))       v |= MOG_JOY_UP;
    if (j->fire || down(in, SC_SPACE) || down(in, SC_LCTRL) || down(in, SC_RCTRL))
        v |= MOG_JOY_FIRE;
    out->joy[0] = out->joy[1] = v;      /* chevalier 1 : port 1 (et 0) */
    if (g->mode == 2) {                 /* entraînement : chevalier 2 au port 0 */
        const MoonJoy *j2 = &in->joy[1];
        uint16_t w = 0;
        if (j2->right || down(in, SC_D)) w |= MOG_JOY_RIGHT;
        if (j2->left || down(in, SC_A))  w |= MOG_JOY_LEFT;
        if (j2->down || down(in, SC_S))  w |= MOG_JOY_DOWN;
        if (j2->up || down(in, SC_W))    w |= MOG_JOY_UP;
        if (j2->fire || down(in, SC_F))  w |= MOG_JOY_FIRE;
        out->joy[0] = w;
    }

    /* Clavier de l'Amiga : lettres et chiffres tels quels (noms des
     * chevaliers ; E fin du tour, Q abandon, 1-9 lieux), Tab = barre
     * d'espace, Entrée, retour arrière */
    int key = 0;
    for (int sc = SC_A; sc <= SC_Z; sc++)
        if (pressed(in, sc))
            key = 'A' + (sc - SC_A);
    for (int sc = SC_1; sc <= SC_9; sc++)
        if (pressed(in, sc))
            key = '1' + (sc - SC_1);
    if (pressed(in, SC_0))
        key = '0';
    if (pressed(in, SC_MINUS))
        key = '-';
    if (pressed(in, SC_TAB))
        key = ' ';
    if (pressed(in, SC_RETURN))
        key = '\r';
    if (pressed(in, SC_BACKSPACE))
        key = '\b';
    out->key = key;
}

static void host_audio(void *user, const int16_t *stereo, int frames)
{
    (void)user;
    hal_audio_stream_push(stereo, frames);
}

/* ------------------------------------------------------------ intro */

static uint32_t s_fb[320 * 200];
static int s_rate, s_fast;

static void intro_vbl(ProgIntro *p)
{
    GameCtx *ctx = p->user;
    if (s_fast)
        return;                                         /* défilement rapide */
    if (s_rate > 0) {
        static int16_t buf[2 * 2048];
        int n = s_rate / 50;
        prog_music_mix(p, buf, n, s_rate);
        hal_audio_stream_push(buf, n);
    }
    prog_screen(p->vm, p->colour, s_fb);
    hal_present(s_fb);
    hal_vbl_wait();
    MoonInput *in = &ctx->input;
    if (hal_poll(in) && (in->quit || in->escape)) {
        hal_quit();
        exit(0);
    }
    int any = 0;
    for (int sc = 4; sc < 232 && sc < in->n_keys; sc++)
        if (pressed(in, sc)) {
            any = 1;
            if (sc == SC_RETURN)
                s_fast = 1;
        }
    if (in->joy[0].fire)
        s_fast = 1;
    if (any)                                            /* LAB_0342 : touche notée */
        ix_ww(p->vm, PROGRAM_SECSTRT_16, 1);
}

/* program : démarrage et intro, jusqu'au chargement de mog */
static void run_intro(GameCtx *ctx)
{
    static IxVM vm;
    static ProgIntro p;
    uint32_t fast;
    memset(&p, 0, sizeof p);
    if (prog_boot_memory(&vm, &fast) < 0) {
        fprintf(stderr, "intro : mémoire de program impossible\n");
        return;
    }
    p.vm = &vm;
    p.vbl = intro_vbl;
    p.user = ctx;
    p.potgor = 0xFFFF;                                  /* bouton droit relâché */
    s_fast = 0;
    prog_boot(&p, PROG_CHIP_BLOCK, PROG_CHIP_SIZE, fast, PROG_FAST_SIZE);
    prog_intro(&p);
    ix_vm_free(&vm);
}

void game_run_mog(GameCtx *ctx)
{
    s_rate = hal_audio_stream_open();
    run_intro(ctx);
    for (;;) {
        memset(&s_game, 0, sizeof s_game);
        s_game.vbl = host_vbl;
        s_game.user = ctx;
        s_game.seed = (int)(hal_ticks() & 3);           /* LAB_04A5 : faisceau */
        s_game.audio_rate = s_rate;
        s_game.audio = s_game.audio_rate ? host_audio : NULL;
        if (mog_game_boot(&s_game) < 0) {
            fprintf(stderr, "démarrage de mog impossible (fichiers du jeu ?)\n");
            return;
        }
        int r = mog_game_run(&s_game);
        printf("partie finie : %s\n", r == MOG_MAP_WIN ? "Pierre de lune rendue"
                                     : r == MOG_MAP_OVER ? "fin de partie" : "arrêt");
        if (r < 0)
            return;
    }
}
