/*
 * moon_mog.c — le jeu complet de mog porté en C (mog_game.c) dans la
 * fenêtre SDL : une VBL = image présentée, 1/50 s, entrées lues.
 *
 * Commandes : flèches (ou manette) = joystick, Espace / Ctrl / Z / bouton
 * de la manette = feu ; touches de l'Amiga : I ou Tab = barre d'espace
 * (inventaire sur la carte, pause en combat), E = fin du tour, 1-9 =
 * choix d'un lieu, Q = abandon de la partie ; Échap = quitter.
 */
#include "moon_game.h"
#include "moon_hal.h"
#include "mog_game.h"
#include "mog_map.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {                                  /* codes SDL_Scancode */
    SC_A = 4, SC_E = 8, SC_I = 12, SC_Q = 20, SC_Z = 29, SC_1 = 30, SC_9 = 38,
    SC_TAB = 43, SC_SPACE = 44, SC_RIGHT = 79, SC_LEFT = 80, SC_DOWN = 81,
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

    int key = 0;
    if (pressed(in, SC_I) | pressed(in, SC_TAB))
        key = ' ';
    if (pressed(in, SC_E))
        key = 'E';
    if (pressed(in, SC_Q))
        key = 'Q';
    for (int sc = SC_1; sc <= SC_9; sc++)
        if (pressed(in, sc))
            key = '1' + (sc - SC_1);
    out->key = key;
}

void game_run_mog(GameCtx *ctx)
{
    for (;;) {
        memset(&s_game, 0, sizeof s_game);
        s_game.vbl = host_vbl;
        s_game.user = ctx;
        s_game.seed = (int)(hal_ticks() & 3);           /* LAB_04A5 : faisceau */
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
