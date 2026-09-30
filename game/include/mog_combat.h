/*
 * mog_combat.h — combat de mog porté en C : contrôleurs (joueur, IA),
 * collisions, boucle. Travaille, comme le moteur de scripts (ix_engine.h),
 * sur la mémoire de mog aux adresses d'origine.
 * Référence : docs/DOC_MOTEUR_COMBAT_MOG.md.
 */
#ifndef MOG_COMBAT_H
#define MOG_COMBAT_H

#include <stdint.h>
#include "ix_engine.h"
#include "mog_blit.h"

/* Bits du joystick (LAB_00EE) */
enum {
    MOG_JOY_RIGHT = 0x01, MOG_JOY_LEFT = 0x02, MOG_JOY_DOWN = 0x04,
    MOG_JOY_UP = 0x08, MOG_JOY_FIRE = 0x10
};

typedef struct {
    IxEngine       eng;       /* moteur de scripts ; eng.vm = mémoire de mog */
    IxHost         eng_host;  /* hôte donné au moteur : $B0 -> mog_native    */
    IxHost         out;       /* hôte du jeu : dessin, sons, messages        */
    uint16_t       joy[2];    /* ports 0 et 1 (LAB_062F / LAB_0630)          */
    uint16_t       vhposr;    /* position du faisceau lue par l'IA comme
                                 source de hasard (registre VHPOSR)         */
    unsigned long  errors;    /* routine d'origine non portée, etc.          */
    MogBlitter     blt;       /* blitter (dessins dans les plans de bits)    */
    int            planes;    /* 1 : le moteur dessine dans les écrans de mog
                                 (LAB_0CDA), sinon via out.draw          */
    /* Sorties propres au combat (facultatives) : */
    void (*voice)(void *user, int channel, int n);     /* LAB_0F8C : son sur un canal */
    void (*palette)(void *user, const uint16_t *rgb);  /* LAB_0D8A : 32 couleurs $0RGB */
    /* LAB_0D77 : attente d'une VBL. NULL : v_VblCounter + 1 (comme le banc) ;
     * le jeu y montre l'écran, mène les couleurs et lit les entrées. */
    void (*wait_vbl)(void *user);
    /* Combat_FrameStart : début d'une image (combat, carte) ; facultatif. */
    void (*frame_start)(void *user);
    /* Attentes actives de l'original (feu, touche) : le temps passe
     * (interruptions) ; l'hôte y fait une VBL (NULL : rien). */
    void (*idle)(void *user);
    /* Puce audio (mog_sound.c) ; NULL : pas de son, mémoire intacte. */
    struct MogAudio *audio;
} MogCombat;

/* Prépare la structure sur une mémoire déjà chargée. `host` : dessin, sons,
 * messages (frame_info et call sont ignorés : CEL lues en mémoire, routines
 * natives portées). La structure ne doit plus être déplacée ensuite. */
void mog_combat_init(MogCombat *m, IxVM *vm, const IxHost *host);

/* LAB_04A1 : générateur pseudo-aléatoire (état LAB_0973). */
uint32_t mog_random(MogCombat *m);

/* Routine native appelée par l'opcode $B0 pour l'entité `en`.
 * Renvoie 0 si elle n'est pas portée. */
int mog_native(MogCombat *m, uint32_t routine, uint32_t en);

/* Combat_RunControllers [LAB_0322] : décisions (joystick, IA) -> scripts. */
void mog_run_controllers(MogCombat *m);

/* Combat_Collisions [LAB_03BE] (+ Combat_ClearFrameLists) : contacts
 * frappe/corps au pixel près -> liens 14/18 des objets. */
void mog_collisions(MogCombat *m);

/* Combat_Run [LAB_0036] : mise en route du combat (après sa préparation). */
void mog_combat_begin(MogCombat *m);

/* Combat_FrameStart [LAB_031E] : LAB_0321 = v_VblCounter. */
void mog_frame_start(MogCombat *m);
/* Tour d'une attente active : rendez-vous (frame_start) puis idle. */
void mog_idle(MogCombat *m);

/* LAB_0D77 / Hw_WaitVbls : attente de n VBL. */
void mog_wait_vbls(MogCombat *m, unsigned n);
/* Combat_FrameWait [LAB_031F] : complète l'image à v_FrameVbls VBL. */
void mog_frame_wait(MogCombat *m);

/* LAB_0D71 + LAB_0416 : écran dessiné montré, écrans échangés. */
void mog_swap_screens(MogCombat *m);

/* Combat_Run [LAB_0036] complet : toutes les images jusqu'à la fin,
 * dessins dans les écrans de mog, puis Combat_CheckKO et remise en état. */
void mog_combat_run(MogCombat *m);

/* Une image de Combat_Loop [LAB_0037] (partie logique : contrôleurs,
 * moteur, collisions, fin). Renvoie 0 quand le combat est terminé. */
int mog_combat_frame(MogCombat *m);

#endif /* MOG_COMBAT_H */
