/*
 * prog_run.c — une scène de l'intro de program en C (prog_intro.c) menée
 * VBL par VBL depuis une image mémoire, pour tools/prog_lockstep.py.
 *
 *   prog_run <mémoire> <scène> <fichier_image>
 *
 * <scène> : « 05a5 ». À la fin de chaque VBL, la mémoire est écrite dans
 * <fichier_image>, puis « V » sur stdout ; une ligne est lue sur stdin
 * (« Q » : arrêt). « M » : départ de la musique (SECSTRT_1) ; « END » : fin
 * de la scène (mémoire écrite).
 */
#include "prog_intro.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static IxVM vm;
static const char *dump_path;

static void dump(void)
{
    FILE *f = fopen(dump_path, "wb");
    if (!f) { perror(dump_path); exit(1); }
    fwrite(vm.mem, 1, vm.size, f);
    fclose(f);
}

static void on_vbl(ProgIntro *p)
{
    (void)p;
    dump();
    printf("V\n");
    fflush(stdout);
    char line[64];
    if (!fgets(line, sizeof line, stdin) || line[0] == 'Q')
        exit(0);
}

static void on_music(ProgIntro *p)
{
    (void)p;
    printf("M\n");
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage : prog_run mémoire scène fichier_image\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    vm.mem = malloc((size_t)n);
    vm.size = vm.heap = (uint32_t)n;
    if (!vm.mem || fread(vm.mem, 1, (size_t)n, f) != (size_t)n) return 1;
    fclose(f);
    dump_path = argv[2 + 1];

    static ProgIntro p;
    p.vm = &vm;
    p.vbl = on_vbl;
    p.music_start = on_music;
    if (!strcmp(argv[2], "05a5"))
        prog_scene_05a5(&p);
    else {
        fprintf(stderr, "scène inconnue : %s\n", argv[2]);
        return 2;
    }
    dump();
    printf("END\n");
    return 0;
}
