/*
 * ix_vm.c — chargement d'une image mémoire Amiga (voir ix_vm.h).
 */
#include "ix_vm.h"

#include <stdlib.h>
#include <string.h>

unsigned long ix_vm_faults;

int ix_vm_load(IxVM *vm, const IxImage *img, uint32_t extra)
{
    memset(vm, 0, sizeof *vm);
    vm->size = img->total_size + extra;
    vm->mem  = calloc(1, vm->size);
    if (!vm->mem)
        return -1;
    vm->heap = IX_VM_BASE + img->total_size;

    for (int i = 0; i < img->hunk_count; i++) {
        const IxHunk *h = &img->hunks[i];
        if (h->data)
            memcpy(ix_vm_ptr(vm, h->va), h->data, h->data_size);
    }
    for (int i = 0; i < img->hunk_count; i++) {
        const IxHunk *h = &img->hunks[i];
        for (int r = 0; r < h->reloc_count; r++) {
            uint32_t at = h->va + h->relocs[r].offset;
            ix_wl(vm, at, ix_rl(vm, at) + img->hunks[h->relocs[r].target].va);
        }
    }
    return 0;
}

void ix_vm_free(IxVM *vm)
{
    free(vm->mem);
    memset(vm, 0, sizeof *vm);
}

uint32_t ix_vm_alloc(IxVM *vm, uint32_t n)
{
    n = (n + 3u) & ~3u;
    if (vm->heap - IX_VM_BASE + n > vm->size)
        return 0;
    uint32_t va = vm->heap;
    vm->heap += n;
    return va;
}
