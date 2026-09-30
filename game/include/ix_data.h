/*
 * ix_data.h — images mémoire des binaires Amiga pour le moteur IMAGEXCEL
 * (générées par tools/ix_scripts.py dans game/data/).
 */
#ifndef IX_DATA_H
#define IX_DATA_H

#include <stddef.h>
#include <stdint.h>

#define IX_VM_BASE 0x00100000u   /* adresse virtuelle du premier hunk */

enum { IX_HUNK_CODE, IX_HUNK_DATA, IX_HUNK_BSS };

/* Relocation : le mot long big-endian à `offset` (dans le hunk) est un
 * offset dans le hunk `target` ; ix_vm_load() y ajoute l'adresse virtuelle
 * du hunk cible. */
typedef struct {
    uint32_t offset;
    uint16_t target;
} IxReloc;

typedef struct {
    int             type;        /* IX_HUNK_CODE / DATA / BSS            */
    uint32_t        va;          /* adresse virtuelle                     */
    uint32_t        size;        /* taille allouée (BSS compris)          */
    const uint8_t  *data;        /* contenu initial (NULL pour un BSS)    */
    uint32_t        data_size;
    const IxReloc  *relocs;
    int             reloc_count;
} IxHunk;

typedef struct {
    const char      *name;
    const IxHunk    *hunks;
    int              hunk_count;
    uint32_t         total_size; /* octets à partir de IX_VM_BASE         */
    const uint32_t  *scripts;    /* adresses des scripts identifiés       */
    int              script_count;
} IxImage;

extern const IxImage ix_program_image;
extern const IxImage ix_mog_image;

#endif /* IX_DATA_H */
