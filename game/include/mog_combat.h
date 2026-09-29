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

/* Bits du joystick (LAB_00EE) */
enum {
    MOG_JOY_RIGHT = 0x01, MOG_JOY_LEFT = 0x02, MOG_JOY_DOWN = 0x04,
    MOG_JOY_UP = 0x08, MOG_JOY_FIRE = 0x10
};

typedef struct {
    IxEngine       eng;       /* moteur de scripts ; eng.vm = mémoire de mog */
    uint16_t       joy[2];    /* ports 0 et 1 (LAB_062F / LAB_0630)          */
    unsigned long  errors;    /* routine d'origine non portée, etc.          */
} MogCombat;

/* Prépare la structure sur une mémoire déjà chargée. */
void mog_combat_init(MogCombat *m, IxVM *vm, const IxHost *host);

/* Combat_RunControllers [LAB_0322] : décisions (joystick, IA) -> scripts. */
void mog_run_controllers(MogCombat *m);

#endif /* MOG_COMBAT_H */
