/*
 * prog_intro.h — l'intro de program (défilement, scènes) traduite de
 * amiga_asm/program.asm sur la mémoire d'origine (prog_intro.c).
 */
#ifndef PROG_INTRO_H
#define PROG_INTRO_H

#include <stdint.h>
#include "ix_vm.h"
#include "mog_blit.h"

typedef struct ProgIntro ProgIntro;

struct ProgIntro {
    IxVM *vm;
    MogBlitter blt;
    uint16_t colour[32];            /* COLOR00-31 */
    unsigned long vbls;
    /* Fin de chaque VBL (serveurs passés) : image à montrer, entrées */
    void (*vbl)(ProgIntro *p);
    /* SECSTRT_1 : départ de la musique (NULL : rien) */
    void (*music_start)(ProgIntro *p);
    /* Serveur VBL de la musique LAB_005C (NULL : rien) */
    void (*music_vbl)(ProgIntro *p);
    void *user;
};

/* LAB_0552 : une VBL (interruption LAB_0331 : LAB_0379, SECSTRT_15) */
void prog_wait_vbl(ProgIntro *p);

/* LAB_05A5 : défilement vertical des tuiles jusqu'à LAB_05B8 = 1000 */
void prog_scene_05a5(ProgIntro *p);

#endif /* PROG_INTRO_H */
