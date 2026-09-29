/*
 * mog_step.c — exécute une routine portée du combat de mog sur une image
 * mémoire, pour tools/mog_difftest.py.
 *
 *   mog_step <mémoire> <routine> <joy0> <joy1> <sortie> [données]
 *
 * <mémoire> : octets de l'adresse 0 à la fin de l'espace du jeu (image
 * prise dans tools/mog_ref.py). <routine> : Combat_RunControllers,
 * Ix_RunEntities, Combat_Collisions, Combat_Run (mog_combat_begin), une
 * routine de t_CreatureInit (LAB_0164...), ou plusieurs séparées par « + ».
 * [données] : dossier des fichiers du jeu (pour les rencontres).
 * Écrit la mémoire après la routine dans <sortie> ; sur stdout :
 *   S n        son         M texte     message du jeu
 *   V c n      son n sur le canal c     P c0..c31   palette
 *   E erreurs défauts
 */
#include "mog_combat.h"
#include "mog_encounter.h"
#include "moon_assets.h"
#include "ix_mog_syms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void sound(void *u, int n) { (void)u; printf("S %d\n", n); }
static void message(void *u, const char *t) { (void)u; printf("M %s\n", t); }
static void voice(void *u, int ch, int n) { (void)u; printf("V %d %d\n", ch, n); }
static void palette(void *u, const uint16_t *c)
{
    (void)u;
    printf("P");
    for (int i = 0; i < 32; i++)
        printf(" %03X", c[i]);
    printf("\n");
}

int main(int argc, char **argv)
{
    if (argc < 6) {
        fprintf(stderr, "usage : mog_step mémoire routine joy0 joy1 sortie\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    IxVM vm = { malloc((size_t)n), 0, (uint32_t)n, (uint32_t)n };
    if (!vm.mem || fread(vm.mem, 1, (size_t)n, f) != (size_t)n) { fclose(f); return 1; }
    fclose(f);

    if (argc > 6 && moon_init(argv[6]) != 0) {
        fprintf(stderr, "données introuvables : %s\n", argv[6]);
        return 1;
    }
    static const struct { const char *name; uint32_t fn; } enc[] = {
        { "LAB_0164", MOG_LAB_0164 }, { "LAB_0168", MOG_LAB_0168 }, { "LAB_016A", MOG_LAB_016A },
        { "LAB_0175", MOG_LAB_0175 }, { "LAB_0188", MOG_LAB_0188 }, { "LAB_018C", MOG_LAB_018C },
        { "LAB_0192", MOG_LAB_0192 }, { "LAB_0196", MOG_LAB_0196 }, { "LAB_019A", MOG_LAB_019A },
        { "LAB_019E", MOG_LAB_019E }, { "LAB_01A0", MOG_LAB_01A0 },
    };

    IxHost host = { NULL, NULL, NULL, sound, NULL, message };
    MogCombat m;
    mog_combat_init(&m, &vm, &host);
    m.voice = voice;
    m.palette = palette;
    m.joy[0] = (uint16_t)strtoul(argv[3], NULL, 0);
    m.joy[1] = (uint16_t)strtoul(argv[4], NULL, 0);

    /* routines séparées par des « + », exécutées dans l'ordre */
    char list[256];
    snprintf(list, sizeof list, "%s", argv[2]);
    for (char *r = strtok(list, "+"); r; r = strtok(NULL, "+")) {
        if (!strcmp(r, "Combat_RunControllers"))
            mog_run_controllers(&m);
        else if (!strcmp(r, "Ix_RunEntities"))
            ix_run_entities(&m.eng);
        else if (!strcmp(r, "Combat_Collisions"))
            mog_collisions(&m);
        else if (!strcmp(r, "Combat_Run"))
            mog_combat_begin(&m);
        else {
            unsigned i;
            for (i = 0; i < sizeof enc / sizeof enc[0]; i++)
                if (!strcmp(r, enc[i].name)) {
                    mog_encounter_init(&m, enc[i].fn);
                    break;
                }
            if (i < sizeof enc / sizeof enc[0])
                continue;
            fprintf(stderr, "routine inconnue : %s\n", r);
            return 2;
        }
    }

    FILE *o = fopen(argv[5], "wb");
    if (!o) { perror(argv[5]); return 1; }
    fwrite(vm.mem, 1, vm.size, o);
    fclose(o);
    printf("E %lu %lu\n", m.errors + m.eng.errors, ix_vm_faults);
    free(vm.mem);
    return 0;
}
