/*
 * ix_data.h — types des données IMAGEXCEL générées par tools/ix_scripts.py
 * (game/data/ix_program.c, game/data/ix_mog.c).
 */
#ifndef IX_DATA_H
#define IX_DATA_H

#include <stdint.h>

/* Relocation : le mot long big-endian à `offset` est un offset dans le
 * hunk `target` ; le chargeur le remplace par l'adresse réelle. */
typedef struct {
    uint32_t offset;
    uint16_t target;
} IxReloc;

typedef struct {
    int             index;       /* numéro du hunk dans l'exécutable   */
    const uint8_t  *data;
    uint32_t        size;
    const IxReloc  *relocs;
    int             reloc_count;
} IxHunk;

/* Script ou table de scripts : (hunk, offset). */
typedef struct {
    const char *name;            /* label dans amiga_asm/<bin>.asm     */
    int         hunk;
    uint32_t    offset;
} IxSymbol;

#define IX_DECLARE(bin)                                   \
    extern const IxHunk   ix_##bin##_hunks[];             \
    extern const int      ix_##bin##_hunk_count;          \
    extern const IxSymbol ix_##bin##_scripts[];           \
    extern const int      ix_##bin##_script_count;        \
    extern const IxSymbol ix_##bin##_tables[];            \
    extern const int      ix_##bin##_table_count;

IX_DECLARE(program)
IX_DECLARE(mog)

#endif /* IX_DATA_H */
