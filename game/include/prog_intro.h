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

typedef struct {
    uint32_t lc;            /* AUDxLC (latché)              */
    uint16_t len, per, vol; /* AUDxLEN (mots), PER, VOL     */
    uint32_t ptr, words, pos;
    double   frac;
    int      on;
} ProgPaulaCh;

typedef struct {
    ProgPaulaCh ch[4];
    uint16_t dmacon;
} ProgPaula;

struct ProgIntro {
    IxVM *vm;
    MogBlitter blt;
    uint16_t colour[32];            /* COLOR00-31 */
    unsigned long vbls;
    uint32_t a1;                    /* registre A1 (gardé dans les entités) */
    /* Fin de chaque VBL (serveurs passés) : image à montrer, entrées */
    void (*vbl)(ProgIntro *p);
    ProgPaula paula;                /* voies audio (musique)             */
    uint16_t potgor;                /* POTGOR ($DFF016) : bit 10 = bouton
                                       droit relâché (banc : 0)          */
    void *user;
};

/* LAB_0552 : une VBL (interruption LAB_0331 : LAB_0379, SECSTRT_15) */
void prog_wait_vbl(ProgIntro *p);

/* LAB_05A5 : défilement vertical des tuiles jusqu'à LAB_05B8 = 1000 */
void prog_scene_05a5(ProgIntro *p);

/* Scènes suivantes (SECSTRT_0, dans cet ordre ; LAB_0123 = 4 avant) */
void prog_scene_001b(ProgIntro *p);
void prog_scene_001c(ProgIntro *p);
void prog_scene_0174(ProgIntro *p);
void prog_scene_001a(ProgIntro *p);
void prog_scene_002c(ProgIntro *p);
void prog_scene_002d(ProgIntro *p);
void prog_scene_002f(ProgIntro *p);
void prog_scene_002e(ProgIntro *p);

/* LAB_025F : fondu au noir (36 VBL) ; LAB_0054 : écran de texte a0 */
void prog_fade_black(ProgIntro *p);
void prog_text_screen(ProgIntro *p, uint32_t a0);

#define PROG_CHIP_BLOCK 0x00008000u   /* A1 du lanceur */
#define PROG_CHIP_SIZE  (0x6B000u - PROG_CHIP_BLOCK)
#define PROG_FAST_SIZE  0x00060000u   /* D0 du lanceur */

/* Mémoire initiale (hunks de program relogés) ; *fast : bloc fast (A0) */
int prog_boot_memory(IxVM *vm, uint32_t *fast);

/* SECSTRT_0 : démarrage (blocs du lanceur), puis l'intro jusqu'à mog */
void prog_boot(ProgIntro *p, uint32_t chip, uint32_t chip_size, uint32_t fast, uint32_t fast_size);
int prog_loading(ProgIntro *p);
/* Unpack_Rnc1 : décompression RNC en place à a0 ; renvoie la taille */
uint32_t prog_rnc_unpack(ProgIntro *p, uint32_t a0);
/* LAB_0001 : la fin (mog gagné : EXT_0007 = $3E0, bit 7), prog_intro y va
 * d'elle-même */
void prog_ending(ProgIntro *p);
void prog_intro(ProgIntro *p);

/* prog_music.c : SECSTRT_1 (départ), LAB_005C (une VBL), mixage Paula */
void prog_music_start(ProgIntro *p);
void prog_music_vbl(ProgIntro *p);
void prog_music_volume(ProgIntro *p, int voice, uint16_t vol);
void prog_music_mix(ProgIntro *p, int16_t *out, int frames, int rate);

#endif /* PROG_INTRO_H */
