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
void mog_boot_tables_0155(IxVM *vm);    /* LAB_0155 */

/* LAB_0303 (sans Col_InitHitFile) : banques CEL, opcodes, tampons. */
void mog_boot_engine(IxVM *vm);

/* LAB_020F : tables de réaction du chevalier humain (LAB_0621/LAB_0622). */
void mog_boot_reactions(IxVM *vm);

/* Fichiers (LAB_0BB5 / LAB_0BD7 / LAB_0BFF) : nom = chaîne en mémoire de
 * mog ; L23_0001A et L23_0000E (taille) mis à jour comme l'original. */
typedef struct {
    uint8_t *data;
    size_t   len, pos;
} MogFile;
int      mog_file_open(IxVM *vm, uint32_t name, MogFile *f);
uint32_t mog_file_read(IxVM *vm, MogFile *f, uint32_t dst, uint32_t n);
void     mog_file_close(MogFile *f);
/* LAB_0CC2 : décompression LZSS de n octets de src vers dst. */
uint32_t mog_unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst);

/* SECSTRT_30 / SECSTRT_28 : tampons graphiques (LAB_0D40), table
 * d'inversion des bits LAB_0CD9. */
void     mog_boot_graphics(IxVM *vm);

/* LAB_013A : décors de combat (fichier « Test ») à rl(LAB_05B9 + 8). */
void     mog_boot_backgrounds(IxVM *vm);

/* Chargeurs (noms : chaînes en mémoire de mog ; fichiers lus par
 * moon_file_read, moon_init() doit avoir été appelé). */
void     mog_load_cel(IxVM *vm, uint32_t name, uint32_t dest);        /* LAB_0CBB */
uint32_t mog_cel_size(IxVM *vm, uint32_t name);                       /* LAB_0CB6 */
void     mog_hit_init(IxVM *vm);                                      /* Col_InitHitFile */
int      mog_load_hit_cel(IxVM *vm, uint32_t name, uint32_t dest);    /* Col_LoadHitData */
void     mog_boot_knight_cels(IxVM *vm);                              /* LAB_0115 */
void     mog_load_enemy_knight(IxVM *vm);                             /* LAB_0116 */

/* LAB_012C (polices, écrans de message, sons) et LAB_0128 (carte) */
void mog_boot_ui(IxVM *vm);
void mog_boot_map(IxVM *vm);

#endif /* MOG_BOOT_H */
