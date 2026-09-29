/*
 * mog_run.c — programme de mog en C mené pas à pas depuis une image mémoire,
 * pour tools/mog_lockstep.py (comparaison avec l'original image par image).
 *
 *   mog_run <mémoire> <programme> <fichier_image> <données>
 *
 * <programme> : « map » (carte du monde, LAB_0DAB). À chaque début d'image
 * (Combat_FrameStart), la mémoire est écrite dans <fichier_image>, puis
 * « F » sur stdout ; une ligne est lue sur stdin : « J joy0 joy1 » (image
 * suivante) ou « Q ». Autres sorties : « M texte », « S n », « END code ».
 */
#include "mog_combat.h"
#include "mog_map.h"
#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static IxVM vm;
static MogCombat m;
static const char *dump_path;

static void message(void *u, const char *t) { (void)u; printf("M %s\n", t); fflush(stdout); }
static void sound(void *u, int n) { (void)u; printf("S %d\n", n); }

static void frame_start(void *u)
{
    (void)u;
    FILE *f = fopen(dump_path, "wb");
    if (!f) { perror(dump_path); exit(1); }
    fwrite(vm.mem, 1, vm.size, f);
    fclose(f);
    printf("F\n");
    fflush(stdout);
    char line[128];
    if (!fgets(line, sizeof line, stdin) || line[0] == 'Q')
        exit(0);
    unsigned a = 0, b = 0;
    if (sscanf(line, "J %u %u", &a, &b) == 2) {
        m.joy[0] = (uint16_t)a;
        m.joy[1] = (uint16_t)b;
    }
}

int main(int argc, char **argv)
{
    if (argc < 5) {
        fprintf(stderr, "usage : mog_run mémoire programme fichier_image données\n");
        return 2;
    }
    if (moon_init(argv[4]) != 0)
        return 1;
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    vm.mem = malloc((size_t)n);
    vm.size = vm.heap = (uint32_t)n;
    if (!vm.mem || fread(vm.mem, 1, (size_t)n, f) != (size_t)n) return 1;
    fclose(f);
    dump_path = argv[3];

    IxHost host = { NULL, NULL, NULL, sound, NULL, message };
    mog_combat_init(&m, &vm, &host);
    m.frame_start = frame_start;
    int code = 0;
    if (!strcmp(argv[2], "map")) {
        mog_map_enter(&m);
        for (;;) {
            int ev = mog_map_frame(&m);
            if (ev == MOG_MAP_ENTER)
                mog_map_enter(&m);
            else if (ev != MOG_MAP_CONTINUE) {
                code = ev;
                break;
            }
        }
    }
    printf("END %d\n", code);
    return 0;
}
