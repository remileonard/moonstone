/*
 * ix_trace.c — trace du moteur IMAGEXCEL C, pour tools/ix_difftest.py.
 *
 *   ix_trace <mémoire> <images> <zone_scrap> [<fichier_dump_final>]
 *
 * <mémoire> : image brute préparée par ix_difftest.py (octets à partir de
 * IX_VM_BASE). Avant chaque image : listes de frames vidées, pile des
 * zones à restaurer remise à <zone_scrap>. Sortie (stdout), une ligne par
 * événement :
 *   F n                 début de l'image n
 *   D cel frame x y flip bg   dessin
 *   S n                 son          C routine        appel natif $B0
 *   M texte             message      H crc32          empreinte mémoire
 */
#include "ix_engine.h"
#include "ix_mog_syms.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int frame_info(void *u, uint32_t cel, int frame, int *w, int *h)
{
    (void)u;
    /* dimensions factices, identiques dans ix_difftest.py */
    *w = 16 + (int)((frame * 7u + (cel & 0xFFu) * 3u) % 48u);
    *h = 20 + (int)((frame * 5u) % 40u);
    return 1;
}

static void draw(void *u, uint32_t cel, int frame, int x, int y, int flipped, int bg)
{
    (void)u;
    printf("D %08X %d %d %d %d %d\n", cel, frame, (int16_t)x, (int16_t)y, flipped, bg);
}

static void sound(void *u, int n) { (void)u; printf("S %d\n", n); }
static void call(void *u, uint32_t r, uint32_t en) { (void)u; (void)en; printf("C %08X\n", r); }
static void message(void *u, const char *t) { (void)u; printf("M %s\n", t); }

static uint32_t crc32(const uint8_t *p, size_t n)
{
    uint32_t c = 0xFFFFFFFFu;
    for (size_t i = 0; i < n; i++) {
        c ^= p[i];
        for (int k = 0; k < 8; k++)
            c = (c >> 1) ^ (0xEDB88320u & (0u - (c & 1u)));
    }
    return ~c;
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "usage : ix_trace mémoire images zone_scrap [dump]\n");
        return 2;
    }
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END);
    long n = ftell(f);
    fseek(f, 0, SEEK_SET);
    IxVM vm = { malloc((size_t)n), IX_VM_BASE, (uint32_t)n, 0 };
    if (!vm.mem || fread(vm.mem, 1, (size_t)n, f) != (size_t)n) { fclose(f); return 1; }
    fclose(f);

    IxHost host = { NULL, frame_info, draw, sound, call, message };
    IxLayout lay;
    IxEngine e;
    ix_layout_mog(&lay);
    ix_engine_init(&e, &vm, &host, &lay);

    int frames = atoi(argv[2]);
    uint32_t scrap = (uint32_t)strtoul(argv[3], NULL, 0);
    for (int i = 0; i < frames; i++) {
        printf("F %d\n", i);
        ix_clear_frame_lists(&e);
        ix_wl(&vm, lay.scrap_ptr, scrap);
        ix_ww(&vm, lay.scrap_count, 0);
        ix_run_entities(&e);
        printf("H %08X\n", crc32(vm.mem, vm.size));
    }
    if (argc > 4) {
        FILE *o = fopen(argv[4], "wb");
        if (o) { fwrite(vm.mem, 1, vm.size, o); fclose(o); }
    }
    printf("E %lu %lu\n", e.errors, ix_vm_faults);
    free(vm.mem);
    return 0;
}
