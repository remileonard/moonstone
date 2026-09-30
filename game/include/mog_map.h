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
    MOG_MAP_UNPORTED,       /* routine d'origine pas encore portée         */
    MOG_MAP_WIN             /* Pierre de lune rendue : fin (SECSTRT_5 lance
                             * « program » ; indicateurs en EXT_000e $3E0) */
};

/* LAB_0DAB : carte dessinée pour le chevalier dont c'est le tour. */
void mog_map_enter(MogCombat *m);
/* Une image de la carte (LAB_0DAD ... LAB_0DBC). */
int  mog_map_frame(MogCombat *m);
/* LAB_0DCB : le dragon entre sur la carte (à partir de la 3e manche). */
void mog_map_dragon(MogCombat *m);

/* LAB_0442 : nombre d0 écrit en décimal en a2 ; renvoie la fin */
uint32_t mog_number(MogCombat *m, uint32_t d0, uint32_t a2);
/* LAB_0E02, LAB_0E05, LAB_0E06 : effets des objets magiques */
void mog_map_0E02(MogCombat *m);
void mog_map_0E05(MogCombat *m, uint32_t a0);
void mog_map_0E06(MogCombat *m);

#endif /* MOG_MAP_H */
