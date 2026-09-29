/*
 * mog_boot.h — mémoire de mog prête pour le combat (voir mog_boot.c).
 */
#ifndef MOG_BOOT_H
#define MOG_BOOT_H

#include "ix_vm.h"

#define MOG_CHIP_BLOCK  0x00008000u   /* A1 du lanceur (LAB_05BC)      */
#define MOG_CHIP_SIZE   0x0005BF18u
#define MOG_FAST_SIZE   0x0005654Du   /* A0 du lanceur (LAB_05BE)      */
#define MOG_STACK_SIZE  0x4000u

/* Charge mog dans `vm` (mémoire depuis l'adresse 0, disposition du banc
 * tools/mog_ref.py) et découpe les blocs du lanceur (LAB_0004). */
int mog_boot_memory(IxVM *vm);

/* LAB_0152 / LAB_0156 : tables d'attaques, de scripts, de dégâts, de
 * marche des combattants. */
void mog_boot_tables(IxVM *vm);

/* LAB_0303 (sans Col_InitHitFile) : banques CEL, opcodes, tampons. */
void mog_boot_engine(IxVM *vm);

/* LAB_020F : tables de réaction du chevalier humain (LAB_0621/LAB_0622). */
void mog_boot_reactions(IxVM *vm);

/* Chargeurs (noms : chaînes en mémoire de mog ; fichiers lus par
 * moon_file_read, moon_init() doit avoir été appelé). */
void     mog_load_cel(IxVM *vm, uint32_t name, uint32_t dest);        /* LAB_0CBB */
uint32_t mog_cel_size(IxVM *vm, uint32_t name);                       /* LAB_0CB6 */
void     mog_hit_init(IxVM *vm);                                      /* Col_InitHitFile */
int      mog_load_hit_cel(IxVM *vm, uint32_t name, uint32_t dest);    /* Col_LoadHitData */
void     mog_boot_knight_cels(IxVM *vm);                              /* LAB_0115 */
void     mog_load_enemy_knight(IxVM *vm);                             /* LAB_0116 */

#endif /* MOG_BOOT_H */
