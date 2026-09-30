/*
 * prog_blit.c — blitter et LAB_0CDA de mog (mog_blit.c) compilés pour
 * program : mêmes routines à d'autres adresses (jumeaux trouvés par
 * tools/asm_twins.py ; en-tête généré par tools/prog_twins.py).
 */
#include "prog_twin_blit.h"
#define mog_boot_graphics prog_boot_graphics
#include "mog_blit.c"
