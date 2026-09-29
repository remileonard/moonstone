/*
 * mog_map.h — carte du monde de mog (mog_map.c).
 */
#ifndef MOG_MAP_H
#define MOG_MAP_H

#include "mog_combat.h"

enum {
    MOG_MAP_CONTINUE = 0,   /* image suivante (LAB_0DAD)                  */
    MOG_MAP_ENTER,          /* carte redessinée : mog_map_enter (LAB_0DAB) */
    MOG_MAP_OVER,           /* fin de partie (LAB_0064)                    */
    MOG_MAP_UNPORTED        /* routine d'origine pas encore portée         */
};

/* LAB_0DAB : carte dessinée pour le chevalier dont c'est le tour. */
void mog_map_enter(MogCombat *m);
/* Une image de la carte (LAB_0DAD ... LAB_0DBC). */
int  mog_map_frame(MogCombat *m);
/* LAB_0DCB : le dragon entre sur la carte (à partir de la 3e manche). */
void mog_map_dragon(MogCombat *m);

#endif /* MOG_MAP_H */
