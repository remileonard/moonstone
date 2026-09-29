/*
 * mog_private.h — routines partagées entre les fichiers du combat (mog_*.c).
 */
#ifndef MOG_PRIVATE_H
#define MOG_PRIVATE_H

#include "mog_combat.h"

typedef struct {
    uint32_t script;     /* A0 */
    uint16_t x, h, d;    /* D0-D2 */
    uint8_t  dir;        /* D3 */
} CtlResult;

/* Ctl_Return [LAB_02BA] : A0 = v_CtlScript, D0-D3 depuis l'objet LAB_0633 */
CtlResult mog_ctl_return(MogCombat *m);
/* v_CtlScript = script, puis Ctl_Return */
CtlResult mog_set_script(MogCombat *m, uint32_t script);
/* LAB_020E : script de réaction 30(a1)[64(a0)] */
CtlResult mog_react_script(MogCombat *m, uint32_t a0, uint32_t a1);
/* Réponse par l'attaque en cours 34(objet)[64(objet)] de LAB_0633 */
CtlResult mog_own_attack(MogCombat *m);
/* LAB_021B : dégâts infligés par l'objet a0 */
uint16_t mog_knight_damage(MogCombat *m, uint32_t a0);
/* LAB_01E6 : parade (LAB_01EB = 1) */
void mog_check_parry(MogCombat *m, uint32_t a1);
/* LAB_030D : relance l'entité de l'objet sur un script */
void mog_restart_entity(MogCombat *m, uint32_t obj, uint32_t script);
/* LAB_03A9 : directions permises par les autres combattants */
uint16_t mog_blocked_dirs(MogCombat *m, uint32_t a0, uint16_t dx, uint16_t dir);
/* Col_SpanOverlap [LAB_03CA] */
int mog_span(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3);

/* LAB_0215 : efface les bits de 63(objet) qui sortiraient de l'arène. */
void mog_arena_bounds(MogCombat *m, uint32_t obj);

/* LAB_0319 : gèle / dégèle l'entité de l'objet (48(entité) ^= 1). */
void mog_toggle_freeze(MogCombat *m, uint32_t obj);

/* LAB_0006 : fin du combat dans 35 images */
void mog_end_combat(MogCombat *m);
/* LAB_031B : l'entité de l'objet disparaît, l'objet est libéré */
void mog_kill_entity_of(MogCombat *m, uint32_t obj);

/* Routine « adversaire suivant » de LAB_05F0 (mog_setup.c). 0 si non portée. */
int mog_next_opponent(MogCombat *m, uint32_t fn);
/* LAB_01A8 : profondeur d'entrée, entité sur le script de repos ;
 * LAB_01A9 : idem sur un script donné. */
void mog_enter_object(MogCombat *m, uint32_t obj);
void mog_enter_object_with(MogCombat *m, uint32_t obj, uint32_t script);

/* LAB_0171 : objet libre (marqué occupé) */
uint32_t mog_alloc_object(MogCombat *m);
/* LAB_0174 : adversaire décrit par l'enregistrement rec */
int mog_spawn_opponent(MogCombat *m, uint32_t rec);
/* Combat_ClearHitLinks [LAB_0161] */
void mog_clear_hit_links(MogCombat *m);

/* LAB_028E : déplacement du Dragon (mog_ai.c ; aussi routine $B0) */
void mog_dragon_move(MogCombat *m);

/* Contrôleurs d'IA (mog_ai.c) : 1 si `fn` est porté (*out rempli). */
int mog_ai_controller(MogCombat *m, uint32_t fn, uint32_t obj, CtlResult *out);

void mog_sound(MogCombat *m, int n);
/* LAB_0427 : tremblement d'écran */
void mog_shake(MogCombat *m);
/* Son n sur le canal ch (SECSTRT_16 / LAB_0A9B-0A9D -> LAB_0F8C) */
void mog_voice(MogCombat *m, int ch, int n);
void mog_message(MogCombat *m, const char *t);

#endif /* MOG_PRIVATE_H */
