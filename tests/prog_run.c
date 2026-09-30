/*
 * prog_run.c — une scène de l'intro de program en C (prog_intro.c) menée
 * VBL par VBL depuis une image mémoire, pour tools/prog_lockstep.py.
 *
 *   prog_run <mémoire> <scène> <fichier_image> [A1 [données fast]]
 *
 * <scène> : « 05a5 », « 001b », « 001c », « 0174 », « 001a », « 002c »,
 * « 002d », « 002f », « 002e », « 0054 », « intro » (SECSTRT_0 entier :
 * démarrage avec le bloc fast à <fast>, fichiers de <données>) ;
 * A1 (hexadécimal) : registre à l'entrée. À la fin de chaque VBL, la mémoire est écrite dans
 * <fichier_image>, puis « V » sur stdout ; une ligne est lue sur stdin
 * (« Q » : arrêt). « M » : départ de la musique (SECSTRT_1) ; « END » : fin
 * de la scène (mémoire écrite).
 */
#include "prog_intro.h"
#include "ix_program_syms.h"
#include "moon_assets.h"

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

static void scene_0054(ProgIntro *p) { prog_text_screen(p, PROGRAM_LAB_00AA); }

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
    if (argc > 4)
        p.a1 = (uint32_t)strtoul(argv[4], NULL, 16);
    static const struct { const char *n; void (*f)(ProgIntro *); } sc[] = {
        { "05a5", prog_scene_05a5 }, { "001b", prog_scene_001b },
        { "001c", prog_scene_001c }, { "0174", prog_scene_0174 },
        { "001a", prog_scene_001a }, { "002c", prog_scene_002c },
        { "002d", prog_scene_002d }, { "002f", prog_scene_002f },
        { "002e", prog_scene_002e }, { "0054", scene_0054 },
    };
    if (!strcmp(argv[2], "mem")) {                      /* mémoire initiale en C */
        static IxVM v;
        uint32_t fast;
        if (prog_boot_memory(&v, &fast) < 0)
            return 1;
        vm = v;
        dump();
        printf("fast %x\n", fast);
        return 0;
    }
    if (!strcmp(argv[2], "intro")) {
        if (argc < 7 || moon_init(argv[5]) != 0)
            return 2;
        prog_boot(&p, 0x8000, 0x6B000 - 0x8000, (uint32_t)strtoul(argv[6], NULL, 16), 0x60000);
        prog_intro(&p);
        dump();
        printf("END\n");
        return 0;
    }
    size_t i = 0;
    while (i < sizeof sc / sizeof *sc && strcmp(sc[i].n, argv[2]))
        i++;
    if (i < sizeof sc / sizeof *sc)
        sc[i].f(&p);
    else {
        fprintf(stderr, "scène inconnue : %s\n", argv[2]);
        return 2;
    }
    dump();
    printf("END\n");
    return 0;
}
