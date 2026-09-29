/*
 * mog_fight.h — un combat de mog complet (mog_fight.c) : mémoire de mog
 * démarrée une fois, préparation de la rencontre, images, rendu.
 * Aucune dépendance à SDL : moon_combat.c (jeu) et les outils sans écran
 * s'en servent de la même façon.
 */
#ifndef MOG_FIGHT_H
#define MOG_FIGHT_H

#include <stdint.h>
#include "mog_combat.h"

#define MOG_SCREEN_W 320
#define MOG_SCREEN_H 200

/* Types de lieu (LAB_08C4) : décor et terrain */
enum {
    MOG_PLACE_GRASS  = 0,   /* GL?.t  */
    MOG_PLACE_FOREST = 4,   /* FO?.t  */
    MOG_PLACE_SWAMP  = 8,   /* Sw?.t  */
    MOG_PLACE_WASTE  = 12   /* Wa?.t  */
};

/* Rencontres : index (en octets) dans t_CreatureInit */
enum {
    MOG_ENC_PASSING_KNIGHTS = 0,   /* LAB_0188 */
    MOG_ENC_MUDMEN   = 4,          /* LAB_019A */
    MOG_ENC_DEMON    = 8,          /* LAB_01A0 */
    MOG_ENC_KNIGHT   = 12,         /* LAB_0164 (aussi 16, 56) */
    MOG_ENC_DRAGON   = 20,         /* LAB_0192 */
    MOG_ENC_TROGGS   = 24,         /* LAB_0168 */
    MOG_ENC_TROGGS2  = 28,         /* LAB_016A */
    MOG_ENC_SPEARS   = 32,         /* LAB_0175 */
    MOG_ENC_RATMEN   = 36,         /* LAB_018C */
    MOG_ENC_BALOK    = 48,         /* LAB_0196 */
    MOG_ENC_TROLL    = 64          /* LAB_019E */
};

typedef struct {
    int knight;                 /* chevalier du joueur 0..3 (couleur)   */
    int strength, constitution, endurance;   /* 70, 71, 72 (1..)        */
    int hp;                     /* PV actuels (80) ; <= 0 : maximum      */
    int gold, lives, daggers;   /* 74, 73, 76                            */
    int place;                  /* MOG_PLACE_*                           */
    int encounter;              /* MOG_ENC_*                             */
    int opponent;               /* chevalier adverse 0..3, -1 : aucun    */
    int opponent_human;         /* 1 : adversaire au joystick 2          */
    int opp_strength, opp_constitution, opp_endurance, opp_hp;
} MogFightSetup;

typedef struct {
    IxVM      vm;
    MogCombat m;
    int       booted;
    int       running;          /* 0 : combat terminé                   */
    unsigned  frame;
    uint32_t  player;           /* objet du chevalier du joueur          */
    uint32_t  foe;              /* objet du chevalier adverse (ou 0)     */
    uint16_t  pal[32];          /* palette $0RGB                         */
    uint8_t   bg[MOG_SCREEN_W * MOG_SCREEN_H];    /* décor (index)       */
    uint8_t   pix[MOG_SCREEN_W * MOG_SCREEN_H];   /* image courante      */
    unsigned long draws;
    /* sorties facultatives */
    void     *user;
    void    (*sound)(void *user, int n);
    void    (*voice)(void *user, int channel, int n);
} MogFight;

/* Démarre la mémoire de mog (moon_init() fait). 0 si réussi. */
int  mog_fight_boot(MogFight *f);

/* Prépare le combat décrit par s (mog_fight_boot au besoin). 0 si réussi. */
int  mog_fight_start(MogFight *f, const MogFightSetup *s);

/* Une image de combat (joysticks MOG_JOY_*). Renvoie 0 une fois fini. */
int  mog_fight_frame(MogFight *f, uint16_t joy0, uint16_t joy1);

/* Image courante en ARGB8888 (320 × 200). */
void mog_fight_render(const MogFight *f, uint32_t *fb);

/* Résultats (objet du joueur) */
int  mog_fight_player_hp(const MogFight *f);
int  mog_fight_player_max_hp(const MogFight *f);
int  mog_fight_player_gold(const MogFight *f);
int  mog_fight_won(const MogFight *f);
int  mog_fight_foe_hp(const MogFight *f);       /* chevalier adverse */

/* Nombre de VBL (1/50 s) par image de combat (v_FrameVbls). */
int  mog_fight_frame_vbls(const MogFight *f);

#endif /* MOG_FIGHT_H */
