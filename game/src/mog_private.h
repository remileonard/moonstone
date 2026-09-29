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

/* Écrans (mog_encounter.c) : L00_0908E, LAB_0D72, LAB_0C21, LAB_0305,
 * LAB_03F2 (fondu vers la palette a, 36 VBL) */
void mog_set_planes(MogCombat *m, uint32_t d0);
void mog_clear_screen(MogCombat *m, uint32_t a0);
void mog_piv_decode(MogCombat *m, uint32_t a0);
void mog_reset_entities(MogCombat *m);
void mog_fade_to(MogCombat *m, uint32_t a);
void mog_fade_out(MogCombat *m);                  /* LAB_03F1 */
void mog_fade_black(MogCombat *m);                /* LAB_03F0 */
void mog_load_picture(MogCombat *m, uint32_t name, uint32_t a1);  /* LAB_0C27 */
void mog_show_background(MogCombat *m);           /* LAB_0418 */
void mog_loading_screen(MogCombat *m);            /* LAB_0134 */
/* LAB_0AB5 : banque de sons `name` (en-tête sauté) en rl(dst_var) */
void mog_load_sounds(MogCombat *m, uint32_t name, uint32_t dst_var, uint32_t n);
/* LAB_039E : fonds sauvés remis en place (mog_loop.c) */
void mog_restore_areas(MogCombat *m);
/* LAB_00EC : attente du feu ; LAB_0136 / LAB_0137 : écran de message
 * (mog_map.c) */
void mog_wait_fire(MogCombat *m);
void mog_message_screen(MogCombat *m, uint32_t a0, int dim);
/* mog_map.c : LAB_0DC8, SECSTRT_36, LAB_0065, LAB_0DBD, LAB_0B82, LAB_00EE */
void mog_map_colours_off(MogCombat *m);
void mog_back_to_map(MogCombat *m);
void mog_before_combat(MogCombat *m);
void mog_select_knight(MogCombat *m);
void mog_clear_keys(MogCombat *m);
uint16_t mog_read_joy(MogCombat *m);
uint32_t mog_pick_stat(MogCombat *m);             /* LAB_0469 : D1 */
uint32_t mog_d100(MogCombat *m);
void mog_random_event(MogCombat *m);              /* LAB_045E */
void mog_find_item(MogCombat *m, int d3);         /* LAB_0471 */
void mog_find_gold(MogCombat *m, int d3);         /* LAB_046C */
/* LAB_04AC : contrôleur du jeu de dés (mog_town.c) */
int mog_gamble_ctl(MogCombat *m, uint32_t a0, CtlResult *out);
/* LAB_04C4 : fin de la scène du sacrifice (mog_town.c) */
int mog_sacrifice_ctl(MogCombat *m, uint32_t a0, CtlResult *out);                  /* LAB_04A3 */
/* LAB_007B : ville, temple... de genre d0 (mog_town.c) ; renvoie D0 */
int mog_town(MogCombat *m, uint32_t d0);
/* LAB_001C : a0 prend le butin de a1 (mog_map.c) */
void mog_loot(MogCombat *m, uint32_t a0, uint32_t a1);

/* LAB_0DCF : contrôleur du dragon sur la carte (mog_map.c) */
int mog_map_dragon_ctl(MogCombat *m, uint32_t a0, CtlResult *out);

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
