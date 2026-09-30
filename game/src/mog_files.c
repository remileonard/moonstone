/*
 * mog_files.c — fichiers (LAB_0BB5 / LAB_0BD7 / LAB_0BFF), décompression
 * LZSS (LAB_0CC2) et chargement des CEL (LAB_0CBB / LAB_0CB6) de mog,
 * séparés de mog_boot.c pour être aussi compilés pour program
 * (prog_files.c).
 */
#include "mog_boot.h"
#include "ix_mog_syms.h"
#include "moon_assets.h"

#include <stdlib.h>

static void copy(IxVM *vm, uint32_t dst, uint32_t src, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        ix_wb(vm, dst + i, ix_rb(vm, src + i));
}

/* ------------------------------------------------------------------ */
/* Fichiers (LAB_0BB5 / LAB_0BD7 / LAB_0BFF)                           */
/* ------------------------------------------------------------------ */

/* Ouvre le fichier dont le nom (chaîne en mémoire) est à `name`.
 * L23_0001A = 0 / -1, L23_0000E = taille (lue dans le répertoire). */
int mog_file_open(IxVM *vm, uint32_t name, MogFile *f)
{
    char n[64];
    unsigned i;
    for (i = 0; i < sizeof n - 1 && ix_rb(vm, name + i); i++)
        n[i] = (char)ix_rb(vm, name + i);
    n[i] = 0;
    f->pos = 0;
    f->data = moon_file_read(n, &f->len);
    if (!f->data) {
        f->len = 0;
        ix_ww(vm, MOG_L23_0001A, 0xFFFF);
        return -1;
    }
    ix_ww(vm, MOG_L23_0001A, 0);
    ix_wl(vm, MOG_L23_0000E, (uint32_t)f->len);
    return 0;
}

/* Lit n octets à l'adresse dst ; renvoie le nombre lu. */
uint32_t mog_file_read(IxVM *vm, MogFile *f, uint32_t dst, uint32_t n)
{
    uint32_t k = 0;
    for (; k < n && f->pos < f->len; k++)
        ix_wb(vm, dst + k, f->data[f->pos++]);
    return k;
}

void mog_file_close(MogFile *f)
{
    free(f->data);
    f->data = NULL;
}

/* LAB_0CC2 : décompression LZSS de `n` octets à `src` vers `dst` ;
 * renvoie le nombre d'octets écrits. Décodeur de libmoon_assets, sur la
 * mémoire même : les copies arrière peuvent lire avant `dst`, comme
 * l'original (fenêtre = toute la mémoire sous `dst`). */
uint32_t mog_unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst)
{
    if (!ix_vm_ok(vm, src, n) || !ix_vm_ok(vm, dst, 0))
        return 0;
    int r = moon_lzss_decompress_window(ix_vm_ptr(vm, src), n, vm->mem,
                                        dst - vm->base, vm->size);
    return r < 0 ? 0 : (uint32_t)r;
}

/* LAB_0CBB : charge la CEL `name` à `dest` : en-tête (10 octets), table
 * des frames, pixels décompressés ; 2(dest) = adresse des pixels. */
void mog_load_cel(IxVM *vm, uint32_t name, uint32_t dest)
{
    MogFile f;
    ix_wl(vm, MOG_LAB_0CC9, name);
    ix_wl(vm, MOG_LAB_0CCA, dest);
    mog_file_open(vm, name, &f);
    mog_file_read(vm, &f, dest, 10);
    copy(vm, MOG_LAB_0D1C, dest, 10);
    uint32_t table = (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u;
    uint32_t data = dest + 10 + table;
    mog_file_read(vm, &f, dest + 10, table);
    ix_wl(vm, dest + 2, data);
    uint32_t packed = ix_rl(vm, MOG_LAB_0D1D);
    mog_file_read(vm, &f, data, packed);
    mog_file_close(&f);
    ix_ww(vm, MOG_LAB_0D4D, 1);
    copy(vm, MOG_LAB_0D4F, data, packed);
    uint32_t pix = dest + 10 + (uint32_t)(int32_t)(int16_t)table;
    ix_wl(vm, dest + 2, pix);
    uint32_t n = mog_unpack(vm, MOG_LAB_0D4F, packed, pix);
    uint32_t d1 = (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u;
    d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + 10);
    ix_wl(vm, MOG_LAB_0CCA, n + d1);
}

/* LAB_0CB6 : place occupée par la CEL `name` une fois chargée. */
uint32_t mog_cel_size(IxVM *vm, uint32_t name)
{
    if (name == ix_rl(vm, MOG_LAB_0CC9))                /* LAB_0CB7 */
        return (ix_rl(vm, MOG_LAB_0CCA) + 1) & 0xFFFFFFFEu;
    MogFile f;
    mog_file_open(vm, name, &f);
    mog_file_read(vm, &f, MOG_LAB_0D1C, 10);
    mog_file_close(&f);
    return (ix_rl(vm, MOG_LAB_0D1F) >> 3) + 0x168 + (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u + 10;
}
