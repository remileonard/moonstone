/*
 * ix_engine.h — moteur de scripts IMAGEXCEL du combat (mog), porté
 * routine par routine depuis amiga_asm/mog.asm.
 *
 * Le moteur travaille sur la mémoire virtuelle de mog (ix_vm.h) : les
 * entités (t_Entities), leurs contextes, les objets, les scripts et les
 * tables y sont aux mêmes adresses et au même format que sur l'Amiga.
 * Référence : docs/DOC_MOTEUR_COMBAT_MOG.md.
 *
 * Ce qui dépend du reste du jeu passe par IxHost : dessin des frames CEL,
 * dimensions d'une frame, sons, routines natives appelées par $B0.
 */
#ifndef IX_ENGINE_H
#define IX_ENGINE_H

#include <stdint.h>
#include "ix_vm.h"

/* Taille des structures en mémoire (octets). */
#define IX_ENTITY_SIZE   50
#define IX_ENTITY_COUNT  10
#define IX_CTX_SIZE      36
#define IX_OBJECT_SIZE   132

typedef struct {
    void *user;
    /* Dimensions de la frame `frame` de la CEL `cel` (adresse virtuelle
     * rangée dans les tables de banques). Renvoie 0 si inconnue.
     * NULL : la CEL est en mémoire au format d'origine ; dimensions lues
     * et frame retournée en place, comme Ix_FrameInfo. */
    int  (*frame_info)(void *user, uint32_t cel, int frame, int *w, int *h);
    /* Dessin d'une frame : (x, y) coin haut-gauche ; flipped = image
     * retournée (entité tournée à gauche) ; background = dessin permanent
     * dans le décor (drapeau bit 4), sinon à l'écran. */
    void (*draw)(void *user, uint32_t cel, int frame, int x, int y,
                 int flipped, int background);
    void (*sound)(void *user, int n);                         /* $A4 */
    /* $B0 : routine native `routine` (adresse virtuelle) appelée pour
     * l'entité `entity` (D0-D3 = X/hauteur/profondeur/direction, A1 =
     * objet, A2 = banques CEL : lisibles dans l'entité). */
    void (*call)(void *user, uint32_t routine, uint32_t entity);
    void (*message)(void *user, const char *text);           /* traces du jeu */
} IxHost;

/* Adresses des variables du moteur dans la mémoire de mog. */
typedef struct {
    uint32_t entities;      /* t_Entities                      */
    uint32_t temp_entity;   /* LAB_064A (tri, passe des ombres) */
    uint32_t contexts;      /* LAB_064B (10 × 36)              */
    uint32_t shadow_ctx;    /* LAB_064C                        */
    uint32_t strike_lists;  /* t_StrikeFrames (10 × 80)        */
    uint32_t body_lists;    /* t_BodyFrames (10 × 80)          */
    uint32_t bank_tables;   /* LAB_0647 (5 pointeurs)          */
    uint32_t debug_flag;    /* LAB_06DA (long)                 */
    uint32_t vbl_counter;   /* v_VblCounter (long)             */
    uint32_t flag_0d05;     /* LAB_0D05 (mot)                  */
    uint32_t combatants;    /* v_Combatants                    */
    uint32_t objects_ptr;   /* LAB_05C3 : pointeur vers 20 objets */
    uint32_t scrap_ptr;     /* LAB_0641 : pile des zones à restaurer */
    uint32_t scrap_count;   /* LAB_0645 (mot)                  */
    uint32_t list_body;     /* LAB_0643 : liste « corps » courante  */
    uint32_t list_strike;   /* LAB_0644 : liste « frappe » courante */
    uint32_t bbox_x0;       /* LAB_0639 */
    uint32_t bbox_x1;       /* LAB_0638 */
    uint32_t bbox_y0;       /* LAB_063A */
    uint32_t bbox_y1;       /* LAB_063B */
    uint32_t bbox_set;      /* LAB_063C */
    uint32_t loop_index;    /* LAB_063D : compteur de Ix_RunEntities */
    uint32_t loop_entity;   /* LAB_0640 : entité courante            */
    uint32_t phys_moved;    /* LAB_037F : Ix_Physics a déplacé       */
    uint32_t flip_buffer;   /* LAB_0D40 : pointeur du tampon de retournement */
    uint32_t bitrev;        /* LAB_0CD9 : table d'inversion des bits d'un octet */
    uint32_t flip_size;     /* LAB_0D29                              */
} IxLayout;

typedef struct {
    IxVM          *vm;
    const IxHost  *host;
    IxLayout       lay;
    unsigned long  errors;  /* opcodes invalides, débordements…      */
} IxEngine;

/* Adresses de mog (d'après game/data/ix_mog_syms.h). */
void ix_layout_mog(IxLayout *lay);

void ix_engine_init(IxEngine *e, IxVM *vm, const IxHost *host, const IxLayout *lay);

/* LAB_0305 (partie moteur) : entités à zéro, contextes et listes reliés. */
void ix_reset_entities(IxEngine *e);

/* Ix_RunEntities [LAB_0328] : tri par profondeur, scripts secondaires,
 * une étape de script par entité active. */
void ix_run_entities(IxEngine *e);

/* Ix_Step [LAB_032E] : une étape pour l'entité (ou l'entité temporaire). */
void ix_step(IxEngine *e, uint32_t entity);

/* LAB_0310 : occupe une entité libre avec ce script ; renvoie son adresse
 * (0 si aucune libre, comme « Cannot ALLOCATE a TASK »). */
uint32_t ix_start_entity(IxEngine *e, uint32_t script, uint32_t object,
                         uint32_t banks, int x, int height, int depth,
                         int dir, int controller);

/* Ent_Spawn [LAB_02D0] : nouvel objet (dans les 20 de LAB_05C3) + entité ;
 * renvoie l'objet. */
uint32_t ix_spawn(IxEngine *e, uint32_t script, uint32_t banks, int x,
                  int height, int depth, int dir, int controller);

/* LAB_0315 : entité dont l'objet est `object` (0 si aucune). */
uint32_t ix_find_entity(IxEngine *e, uint32_t object);

/* Combat_ClearFrameLists [LAB_03C7]. */
void ix_clear_frame_lists(IxEngine *e);

#endif /* IX_ENGINE_H */
