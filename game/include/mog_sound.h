/*
 * mog_sound.h — pilote de sons de mog et puce Paula (mog_sound.c).
 */
#ifndef MOG_SOUND_H
#define MOG_SOUND_H

#include "mog_combat.h"

typedef struct MogAudio MogAudio;

typedef struct {
    uint32_t lc;            /* AUDxLC (verrouillé)                */
    uint16_t len, per, vol; /* AUDxLEN (mots), AUDxPER, AUDxVOL   */
    uint32_t ptr, words;    /* bloc en cours                      */
    uint32_t pos;           /* octet lu dans le bloc              */
    double   frac;
    int      on, start_irq;
} MogPaulaCh;

struct MogAudio {
    MogPaulaCh ch[4];
    uint16_t   dmacon, intena, intreq;
    int        in_irq;
};

/* LAB_0F89 (voies) et LAB_0FD4 (échantillons des instruments) :
 * au démarrage, avec ou sans son (mémoire de l'original). */
void mog_snd_init(MogCombat *m);
void mog_snd_relocate(MogCombat *m);
/* LAB_0F8C : son n sur la voie ch ; LAB_0AA2 : effet sur une voie libre. */
void mog_snd_play(MogCombat *m, int n, int ch);
void mog_snd_effect(MogCombat *m, int n);
/* LAB_0F73 : une VBL du pilote. */
void mog_snd_vbl(MogCombat *m);
/* LAB_0F6F : interruption audio de la voie ch (tests). */
void mog_snd_irq_voice(MogCombat *m, int ch);
/* Paula : `frames` échantillons stéréo 16 bits à `rate` Hz. */
void mog_snd_mix(MogCombat *m, int16_t *out, int frames, int rate);

#endif /* MOG_SOUND_H */
