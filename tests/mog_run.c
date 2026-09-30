/*
 * mog_run.c — programme de mog en C mené pas à pas depuis une image mémoire,
 * pour tools/mog_lockstep.py (comparaison avec l'original image par image).
 *
 *   mog_run <mémoire> <programme> <fichier_image> <données>
 *
 * <programme> : « map » (carte du monde, LAB_0DAB) ; « newgame » :
 * démarrage et nouvelle partie en C (mog_game_boot), mémoire écrite dans
 * <fichier_image> (<mémoire> ignorée). À chaque début d'image
 * (Combat_FrameStart), la mémoire est écrite dans <fichier_image>, puis
 * « F » sur stdout ; une ligne est lue sur stdin : « J joy0 joy1 » (image
 * suivante ; « J joy0 joy1 touche » : touche
 * écrite dans SECSTRT_21) ou « Q ». Autres sorties : « M texte », « S n », « END code ».
 */
#include "mog_combat.h"
#include "mog_map.h"
#include "mog_game.h"
#include "mog_sound.h"
#include "mog_menu.h"
#include "mog_encounter.h"
#include "mog_boot.h"
#include "../game/src/mog_private.h"
#include "moon_assets.h"
#include "ix_mog_names.h"

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
    unsigned a = 0, b = 0, k = 0;
    int n = sscanf(line, "J %u %u %u", &a, &b, &k);
    if (n >= 2) {
        m.joy[0] = (uint16_t)a;
        m.joy[1] = (uint16_t)b;
    }
    if (n == 3)                                     /* touche (SECSTRT_21) */
        ix_ww(&vm, MOG_SECSTRT_21, (uint16_t)k);
}

int main(int argc, char **argv)
{
    if (argc < 5) {
        fprintf(stderr, "usage : mog_run mémoire programme fichier_image données\n");
        return 2;
    }
    if (moon_init(argv[4]) != 0)
        return 1;
    if (!strcmp(argv[2], "newgame")) {
        static MogGame g;
        if (mog_game_boot(&g) < 0)
            return 1;
        FILE *o = fopen(argv[3], "wb");
        if (!o) { perror(argv[3]); return 1; }
        fwrite(g.vm.mem, 1, g.vm.size, o);
        fclose(o);
        printf("taille %u\n", g.vm.size);
        return 0;
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
    dump_path = argv[3];

    IxHost host = { NULL, NULL, NULL, sound, NULL, message };
    mog_combat_init(&m, &vm, &host);
    m.frame_start = frame_start;
    int code = 0;
    if (!strcmp(argv[2], "snd")) {
        /* Pilote de sons (tools/mog_sndcheck.py) : « P n voie » (LAB_0F8C),
         * « V » (LAB_0F73), « D a n » (n octets en a, hexadécimal),
         * « R » (registres Paula LC LEN PER VOL des 4 voies), « Q ». */
        static MogAudio audio;
        m.audio = &audio;
        mog_snd_init(&m);
        char line[128];
        while (fgets(line, sizeof line, stdin)) {
            unsigned a = 0, b = 0;
            if (line[0] == 'P' && sscanf(line + 1, "%u %u", &a, &b) == 2)
                mog_snd_play(&m, (int)a, (int)b);
            else if (line[0] == 'V')
                mog_snd_vbl(&m);
            else if (line[0] == 'I' && sscanf(line + 1, "%u", &a) == 1)
                mog_snd_irq_voice(&m, (int)a);
            else if (line[0] == 'D' && sscanf(line + 1, "%x %u", &a, &b) == 2) {
                for (unsigned i = 0; i < b; i++)
                    printf("%02x", ix_rb(&vm, a + i));
                printf("\n");
            } else if (line[0] == 'R') {
                for (int c = 0; c < 4; c++)
                    printf("%08x %04x %04x %04x ", audio.ch[c].lc, audio.ch[c].len,
                           audio.ch[c].per, audio.ch[c].vol);
                printf("\n");
            } else if (line[0] == 'Q')
                break;
            fflush(stdout);
        }
        return 0;
    }
    if (!strcmp(argv[2], "menu")) {
        /* LAB_0001 après les tables : menu, choix des chevaliers, carte */
        int row;
        while ((row = mog_menu(&m)) == 2) {             /* LAB_0002 : entraînement */
            printf("M menu %d\n", row);
            mog_practice(&m);
            mog_combat_run(&m);
            ix_ww(&vm, MOG_LAB_05C5, ix_rw(&vm, MOG_LAB_05DB));
            mog_boot_tables(&vm);
        }
        printf("M menu %d\n", row);
        mog_new_game_full(&m);                          /* LAB_01AE */
        for (uint32_t i = 0; i < 4; i++)                /* LAB_0011 */
            mog_update_knight(&m, MOG_LAB_0613 + i * IX_OBJECT_SIZE);
        mog_choose_knights(&m);                         /* LAB_00D3 */
        mog_new_game_players(&m);                       /* LAB_01BE */
        mog_boot_reactions(&vm);                        /* LAB_020F */
        mog_fade_out(&m);                               /* LAB_03F1 */
        mog_back_to_map(&m);                            /* SECSTRT_36 */
    }
    if (!strcmp(argv[2], "map") || !strcmp(argv[2], "menu")) {
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
