/*
 * main.c — Entry point for the Moonstone game executable
 *
 * Usage: moonstone [asset_dir] [scale] [--combat rencontre|all [lieu]] [--mog]
 *
 *   asset_dir : path to the directory containing Moonstone game data
 *               (PIV, CEL, CMP files).  Defaults to current directory.
 *   scale     : integer display scale factor (default: 2).
 *
 * The game runs at the original Amiga resolution of 320×200 pixels
 * scaled up by the given factor.
 *
 * --combat : combats seuls, à la suite (Échap pour quitter). rencontre =
 * index de t_CreatureInit (0 chevaliers de passage, 4 Mudmen, 8 Démon,
 * 12 chevalier noir, 20 Dragon, 24/28 Troggs, 32 Troggs à lance, 36
 * hommes-rats, 48 Balok, 64 Troll) ou « all » pour les enchaîner ;
 * lieu = 0 forêt, 1 friche, 2 marais, 3 plaine.
 *
 * --mog : le jeu complet porté de l'original (carte, lieux, écrans,
 * combats ; moon_mog.c).
 */

#include "moon_game.h"
#include "moon_combat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    const char *asset_dir = ".";
    int scale = 2;

    int combat = -1, combat_all = 0, place = -1, mog = 0, ending = -1;
    for (int i = 1; i < argc; i++)
        if (!strcmp(argv[i], "--mog")) {
            mog = 1;
            argc = i;
            break;
        } else if (!strcmp(argv[i], "--fin")) {    /* la fin, sans partie */
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

    if (argc >= 2) {
        asset_dir = argv[1];
    }
    if (argc >= 3) {
        scale = atoi(argv[2]);
        if (scale < 1) scale = 1;
        if (scale > 4) scale = 4;
    }

    printf("Moonstone — A Hard Day's Knight\n");
    printf("Asset directory : %s\n", asset_dir);
    printf("Display scale   : %d× (%dx%d)\n",
           scale, 320 * scale, 200 * scale);

    GameCtx ctx;
    if (game_init(&ctx, asset_dir, scale) != 0) {
        fprintf(stderr, "Failed to initialise game.\n");
        return 1;
    }

    if (ending >= 0) {
        game_run_mog_ending(&ctx, ending | 0x80);
    } else if (mog) {
        game_run_mog(&ctx);
    } else if (combat >= 0) {
        static const int all[] = { 12, 24, 20, 36, 48, 4, 64, 32, 0, 28, 8 };
        Knight *k = &ctx.knights[0];
        k->id = KNIGHT_RICHARD;
        k->active = k->human = 1;
        k->strength = k->constitution = k->endurance = 2;
        k->lives = 5;
        k->gold = 50;
        ctx.current_knight = 0;
        ctx.node_target_knight = -1;
        for (int n = 0; ctx.running; n++) {
            ctx.node_type = 0x02;
            ctx.pve_creature_type = combat_all ? all[n % 11] : combat;
            ctx.pve_node_group = place >= 0 ? place : n % 4;
            ctx.state = STATE_COMBAT;
            k->hp = 0;                          /* PV au maximum */
            k->dead = 0;
            if (k->lives <= 0)
                k->lives = 5;
            game_run_combat(&ctx);
            if (ctx.input.quit || ctx.input.escape)
                break;
        }
    } else {
        game_run(&ctx);
    }
    game_shutdown(&ctx);

    return 0;
}
