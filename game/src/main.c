/*
 * main.c — Moonstone, porté de l'original Amiga en C.
 *
 * Usage : moonstone [données] [échelle] [--mog | --fin [drapeaux] |
 *                                        --combat rencontre|all [lieu]]
 *
 *   données : dossier des fichiers du jeu d'origine (défaut : dossier
 *             courant) ; échelle : 1 à 4 (défaut 2), image 320 × 200.
 *
 * Sans option (ou --mog) : le jeu, comme sur l'Amiga : intro, menu,
 * choix des chevaliers, partie, fin après une victoire.
 *
 * --fin : la séquence de fin (Pierre de lune rendue) sans jouer ;
 * drapeaux = EXT_0007 écrit par mog ($80 | chevalier | lieu, défaut $91).
 *
 * --combat : combats seuls, à la suite (Échap pour quitter). rencontre =
 * index de t_CreatureInit (0 chevaliers de passage, 4 Mudmen, 8 Démon,
 * 12 chevalier noir, 20 Dragon, 24/28 Troggs, 32 Troggs à lance, 36
 * hommes-rats, 48 Balok, 64 Troll) ou « all » pour les enchaîner ;
 * lieu = 0 forêt, 1 friche, 2 marais, 3 plaine.
 */
#include "moon_game.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char *asset_dir = ".";
    int scale = 2;
    int combat = -1, combat_all = 0, place = -1, ending = -1;
    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "--mog")) {
            argc = i;
            break;
        } else if (!strcmp(argv[i], "--fin")) {         /* la fin, sans partie */
            ending = i + 1 < argc ? (int)strtol(argv[i + 1], NULL, 0) : 0x91;
            argc = i;
            break;
        } else if (!strcmp(argv[i], "--combat") && i + 1 < argc) {
            combat_all = !strcmp(argv[i + 1], "all");
            combat = combat_all ? 0 : atoi(argv[i + 1]);
            if (i + 2 < argc)
                place = atoi(argv[i + 2]);
            argc = i;
            break;
        }
    if (argc >= 2)
        asset_dir = argv[1];
    if (argc >= 3) {
        scale = atoi(argv[2]);
        if (scale < 1) scale = 1;
        if (scale > 4) scale = 4;
    }

    printf("Moonstone — A Hard Day's Knight\n");
    printf("Asset directory : %s\n", asset_dir);
    printf("Display scale   : %d× (%dx%d)\n", scale, 320 * scale, 200 * scale);

    static GameCtx ctx;
    if (game_init(&ctx, asset_dir, scale) != 0) {
        fprintf(stderr, "Failed to initialise game.\n");
        return 1;
    }
    if (ending >= 0)
        game_run_mog_ending(&ctx, ending | 0x80);
    else if (combat >= 0)
        game_run_combats(&ctx, combat, combat_all, place);
    else
        game_run_mog(&ctx);
    game_shutdown(&ctx);
    return 0;
}
