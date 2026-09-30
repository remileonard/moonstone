/*
 * lib_audit_ref.h — décodeurs du portage tels qu'ils étaient avant de
 * passer par libmoon_assets, vérifiés contre l'original sur les bancs
 * (tools/mog_bootcheck.py, mog_lockstep.py, prog_lockstep.py). Figés ici
 * comme référence indépendante de la bibliothèque pour lib_audit.
 */
#ifndef LIB_AUDIT_REF_H
#define LIB_AUDIT_REF_H

#include <stdint.h>
#include "ix_vm.h"

/* LAB_0CC2 : LZSS de src (n octets) vers dst ; octets écrits */
uint32_t ref_unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst);

/* Unpack_Rnc1 de program, en place à a0 ; tables lues dans `prog` (la
 * mémoire de program) ; renvoie la taille décompressée */
uint32_t ref_rnc_unpack(IxVM *vm, uint32_t a0);

/* Col_LoadHitData (sans le chargement de la CEL) : points d'impact de
 * `name` rangés à LAB_0A4D ; -1 si absent de collide.hit */
int ref_hit_parse(IxVM *vm, uint32_t name, uint32_t dest);

#endif
