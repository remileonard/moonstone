/*
 * mog_game.h — le jeu de mog complet, porté en C : démarrage, nouvelle
 * partie, carte, lieux, écrans, combats (mog_game.c).
 *
 * L'hôte fournit une VBL (entrées, affichage, cadence de 50 Hz) : tout le
 * reste est le programme d'origine, sur sa propre mémoire.
 */
#ifndef MOG_GAME_INCLUDED
#define MOG_GAME_INCLUDED

#include "mog_combat.h"

#define MOG_GAME_W 320
#define MOG_GAME_H 200

typedef struct MogGame MogGame;

/* Entrées lues par l'hôte à chaque VBL */
typedef struct {
    uint16_t joy[2];        /* bits MOG_JOY_* des ports 0 et 1          */
    int      key;           /* caractère appuyé depuis la VBL d'avant (0) */
    int      quit;          /* fermer le jeu                            */
} MogGameInput;

struct MogGame {
    IxVM      vm;
    MogCombat m;
    uint16_t  colour[32];               /* registres couleur (LAB_0E5D) */
    uint32_t  fb[MOG_GAME_W * MOG_GAME_H];
    int       quit;
    /* Une VBL de l'hôte : présenter fb, attendre 1/50 s, lire les entrées. */
    void    (*vbl)(void *user, MogGame *g, MogGameInput *in);
    void     *user;
};

/* Démarrage (SECSTRT_0 ... LAB_0152 / LAB_0156) et nouvelle partie à un
 * joueur (LAB_01AE, LAB_01BE, LAB_020F, LAB_03F1, SECSTRT_36) : le
 * chevalier 1 au joystick (port 1), les trois autres chevaliers noirs.
 * Hors écran : vbl peut rester NULL (outils de comparaison). */
int  mog_game_boot(MogGame *g);
/* La partie jusqu'à sa fin : MOG_MAP_OVER, MOG_MAP_WIN, ou -1 (quitter). */
int  mog_game_run(MogGame *g);
/* Image montrée (plans de la copper list, couleurs, pointeur) dans fb. */
void mog_game_render(MogGame *g);

#endif /* MOG_GAME_INCLUDED */
