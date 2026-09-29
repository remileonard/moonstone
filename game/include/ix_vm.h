/*
 * ix_vm.h — mémoire virtuelle 32 bits big-endian d'un binaire Amiga.
 *
 * L'image générée (game/data/ix_<bin>.c) est chargée à partir de
 * IX_VM_BASE : hunks bout à bout, BSS à zéro, relocations appliquées.
 * Les routines portées manipulent cette mémoire exactement comme le code
 * 68000 d'origine (mêmes adresses, mêmes tailles, big-endian).
 */
#ifndef IX_VM_H
#define IX_VM_H

#include <stdint.h>
#include "ix_data.h"

typedef struct {
    uint8_t  *mem;       /* mem[0] = adresse IX_VM_BASE          */
    uint32_t  size;      /* octets alloués (image + zone libre)  */
    uint32_t  heap;      /* prochaine adresse libre (ix_vm_alloc) */
} IxVM;

/* Charge l'image ; `extra` octets de zone libre sont ajoutés après
 * (objets, tables créées par le portage). 0 = succès. */
int  ix_vm_load(IxVM *vm, const IxImage *img, uint32_t extra);
void ix_vm_free(IxVM *vm);

/* Réserve n octets à zéro dans la zone libre ; renvoie l'adresse (0 si plein). */
uint32_t ix_vm_alloc(IxVM *vm, uint32_t n);

/* Accès big-endian. Une adresse hors image lit 0 et n'écrit rien
 * (signalé par ix_vm_faults). */
extern unsigned long ix_vm_faults;

static inline int ix_vm_ok(const IxVM *vm, uint32_t va, uint32_t n)
{
    return va >= IX_VM_BASE && va - IX_VM_BASE + n <= vm->size;
}

static inline uint8_t *ix_vm_ptr(const IxVM *vm, uint32_t va)
{
    return vm->mem + (va - IX_VM_BASE);
}

static inline uint8_t ix_rb(const IxVM *vm, uint32_t va)
{
    if (!ix_vm_ok(vm, va, 1)) { ix_vm_faults++; return 0; }
    return ix_vm_ptr(vm, va)[0];
}

static inline uint16_t ix_rw(const IxVM *vm, uint32_t va)
{
    if (!ix_vm_ok(vm, va, 2)) { ix_vm_faults++; return 0; }
    const uint8_t *p = ix_vm_ptr(vm, va);
    return (uint16_t)(p[0] << 8 | p[1]);
}

static inline uint32_t ix_rl(const IxVM *vm, uint32_t va)
{
    if (!ix_vm_ok(vm, va, 4)) { ix_vm_faults++; return 0; }
    const uint8_t *p = ix_vm_ptr(vm, va);
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static inline void ix_wb(IxVM *vm, uint32_t va, uint8_t v)
{
    if (!ix_vm_ok(vm, va, 1)) { ix_vm_faults++; return; }
    ix_vm_ptr(vm, va)[0] = v;
}

static inline void ix_ww(IxVM *vm, uint32_t va, uint16_t v)
{
    if (!ix_vm_ok(vm, va, 2)) { ix_vm_faults++; return; }
    uint8_t *p = ix_vm_ptr(vm, va);
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)v;
}

static inline void ix_wl(IxVM *vm, uint32_t va, uint32_t v)
{
    if (!ix_vm_ok(vm, va, 4)) { ix_vm_faults++; return; }
    uint8_t *p = ix_vm_ptr(vm, va);
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)v;
}

#endif /* IX_VM_H */
