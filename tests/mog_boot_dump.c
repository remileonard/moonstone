/*
 * mog_boot_dump.c — mémoire de mog préparée par mog_boot.c, pour
 * tools/mog_bootcheck.py.
 *
 *   mog_boot_dump <sortie> <dossier_données>
 */
#include "mog_boot.h"
#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage : mog_boot_dump sortie dossier_données\n");
        return 2;
    }
    if (moon_init(argv[2]) != 0)
        return 1;
    IxVM vm;
    if (mog_boot_memory(&vm) < 0)
        return 1;
    mog_boot_engine(&vm);                               /* LAB_0303 */
    mog_hit_init(&vm);
    mog_boot_knight_cels(&vm);                          /* LAB_0115 */
    mog_boot_tables(&vm);
    FILE *o = fopen(argv[1], "wb");
    if (!o) { perror(argv[1]); return 1; }
    fwrite(vm.mem, 1, vm.size, o);
    fclose(o);
    printf("%lu défauts\n", ix_vm_faults);
    ix_vm_free(&vm);
    return 0;
}
