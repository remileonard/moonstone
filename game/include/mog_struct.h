/*
 * mog_struct.h — disposition des enregistrements de mog en mémoire.
 *
 * Le portage travaille sur la mémoire d'origine (big-endian, adresses de
 * l'Amiga) : un enregistrement n'est pas une struct C mais une adresse, et
 * ses champs se lisent avec ix_rb / ix_rw / ix_rl à adresse + décalage.
 * Ces énumérations nomment les décalages : `ix_rw(vm, obj + OBJ_HP)` au lieu
 * de `ix_rw(vm, obj + 80)`. La taille (b, w, l) de chaque champ est notée
 * en commentaire.
 */
#ifndef MOG_STRUCT_H
#define MOG_STRUCT_H

/* ------------------------------------------------------------------ */
/* Objet combattant (IX_OBJECT_SIZE = 132 octets) : chevaliers           */
/* (t_KnightObjects), Dragon, créatures et objets du combat (v_Objects).  */
/* ------------------------------------------------------------------ */
enum {
    OBJ_USED        = 0,    /* l  1 : occupé (Ent_Spawn)                        */
    OBJ_X           = 4,    /* w  X                                             */
    OBJ_HEIGHT      = 6,    /* w  hauteur (vers le haut)                        */
    OBJ_DEPTH       = 8,    /* w  profondeur (Y à l'écran)                      */
    OBJ_FACING      = 10,   /* b  1 tourné à droite, 3 à gauche                 */
    OBJ_PORT        = 11,   /* b  manette (1 : port 0, 2 : port 1, 4 : aucune)  */
    OBJ_WALK_PHASE  = 12,   /* b  phase de marche (0-3)                          */
    OBJ_COUNTDOWN   = 13,   /* b  compte à rebours (LAB_01C8)                    */
    OBJ_HIT         = 14,   /* l  objet touché (0 : aucun)                       */
    OBJ_HIT_BY      = 18,   /* l  objet qui l'a touché (0 : aucun)               */
    OBJ_STAND       = 22,   /* l  script au repos                               */
    OBJ_RECOIL      = 26,   /* l  script après avoir touché                      */
    OBJ_REACTIONS   = 30,   /* l  table : script de réaction à l'attaque k       */
    OBJ_ATTACKS     = 34,   /* l  table : script de l'attaque k                  */
    OBJ_BANKS       = 38,   /* l  table des banques CEL                          */
    OBJ_DAMAGE      = 42,   /* l  table : dégâts de l'attaque k                  */
    OBJ_WALK        = 46,   /* l  table : scripts de marche                      */
    OBJ_PARRY       = 50,   /* l  table : attaque qui pare l'attaque k           */
    OBJ_KNIGHT      = 54,   /* l  chevalier 0-3 ; 4 ordinateur ; 5 Dragon        */
    OBJ_BOX_X0      = 58,   /* w  boîte de la dernière image : X gauche          */
    OBJ_BOX_X1      = 60,   /* w  X droit                                        */
    OBJ_INPUT       = 62,   /* w  commandes (bits de direction, $10 feu)         */
    OBJ_BLOCKED     = 63,   /* b  (octet bas de OBJ_INPUT) directions possibles  */
    OBJ_ATTACK      = 64,   /* w  attaque en cours (0, 4 ... $20)                */
    OBJ_CELL_X      = 66,   /* w  case de la carte (X / 8)                       */
    OBJ_CELL_Y      = 68,   /* w  case de la carte (Y / 8)                       */
    OBJ_STRENGTH    = 70,   /* b  force (0-5)                                    */
    OBJ_CONSTITUTION = 71,  /* b  constitution (0-5)                             */
    OBJ_ENDURANCE   = 72,   /* b  endurance (0-5)                                */
    OBJ_LIVES       = 73,   /* b  vies (-1 : mort)                               */
    OBJ_GOLD        = 74,   /* w  or                                             */
    OBJ_DAGGERS     = 76,   /* b  dagues                                         */
    OBJ_CONTROLLER  = 77,   /* b  contrôleur (décalage dans t_Controllers)       */
    OBJ_EXPERIENCE  = 78,   /* w  expérience (combats gagnés)                    */
    OBJ_HP          = 80,   /* w  points de vie                                  */
    OBJ_BEWITCHED   = 82,   /* b  envoûté (tours à passer)                       */
    OBJ_LUCK        = 83,   /* b  ajouté au tirage des événements ($FF : aucun)  */
    OBJ_HP_MAX      = 84,   /* w  points de vie maximum (LAB_0013)               */
    OBJ_MOVEMENT    = 86,   /* b  mouvement sur la carte (LAB_0019)              */
    OBJ_WEAPON      = 88,   /* l  arme ($16 longue ... $19 épée d'acuité)         */
    OBJ_ARMOUR      = 92,   /* l  armure ($1B rembourrée ... $1E de bataille)     */
    OBJ_INVENTORY   = 96,   /* l  inventaire (INV_*)                              */
    OBJ_TARGET      = 100,  /* l  cible sur la carte (chevalier noir, Dragon)     */
    OBJ_AI_FLAGS    = 104,  /* b  drapeaux des contrôleurs                        */
    OBJ_AI_FLAGS2   = 105,  /* b                                                  */
    OBJ_AI_COUNT    = 106,  /* w  compteur des contrôleurs                        */
    OBJ_NAME        = 108,  /* l  nom                                             */
    OBJ_BOX_Y0      = 112,  /* w  boîte de la dernière image : Y haut             */
    OBJ_BOX_Y1      = 114,  /* w  Y bas                                           */
    OBJ_REACH       = 116,  /* w  distance d'attaque (ordinateur)                 */
    OBJ_TOO_CLOSE   = 118,  /* w  distance en dessous de laquelle il recule       */
    OBJ_DEPTH_REACH = 120,  /* w  écart de profondeur toléré                      */
    OBJ_IMPACT_X    = 122,  /* w  point d'impact (Col_PixelHit)                  */
    OBJ_IMPACT_Y    = 124,  /* w                                                  */
    OBJ_MAP_X       = 126,  /* w  position sur la carte                           */
    OBJ_MAP_Y       = 128,  /* w                                                  */
    OBJ_POISONED    = 130   /* b  empoisonné (le guérisseur soigne)               */
};

/* ------------------------------------------------------------------ */
/* Inventaire (24 octets, OBJ_INVENTORY) : nombre de chaque objet        */
/* ------------------------------------------------------------------ */
enum {
    INV_POTIONS     = 0,    /* b  potions de guérison                            */
    INV_GEMS        = 2,    /* b  gemmes de clairvoyance                         */
    INV_SHARP_SWORD = 4,    /* b  épée d'acuité                                  */
    INV_RINGS       = 6,    /* b  anneaux de protection (+20 PV)                 */
    INV_TALISMANS   = 8,    /* b  talismans du Wyrm                              */
    INV_HASTE       = 10,   /* b  parchemins de rapidité (déplacement doublé)    */
    INV_HAWK        = 12,   /* b  parchemins de l'épervier                       */
    INV_ACQUISITION = 14,   /* b  parchemins d'acquisition (butin volé)          */
    INV_WYRM        = 16,   /* b  parchemins du Wyrm                             */
    INV_PROTECTION  = 18,   /* b  parchemins de protection (fuite possible)      */
    INV_KEYS        = 20,   /* b  clés (bits 0-3) ; $0F : le repaire du Démon     */
    INV_MOONSTONES  = 22    /* b  pierres de lune (bits : nouvelle, pleine, demi) */
};

/* ------------------------------------------------------------------ */
/* Créature de la carte / repaire (20 octets, v_MapCreatures)            */
/* ------------------------------------------------------------------ */
enum {
    LAIR_INVENTORY  = 0,    /* l  trésor (inventaire)                            */
    LAIR_CREATURE   = 4,    /* w  genre (décalage dans t_CreatureInit)           */
    LAIR_FOES       = 6,    /* w  adversaires à vaincre                          */
    LAIR_GOLD       = 8,    /* w  or du trésor                                   */
    LAIR_X          = 10,   /* w  position (-1 : plus de créature)               */
    LAIR_Y          = 12,   /* w                                                 */
    LAIR_PLACE      = 14,   /* w  type de lieu du combat                         */
    LAIR_TERRAIN    = 16    /* l  fichier .t                                     */
};

/* ------------------------------------------------------------------ */
/* Combattants (v_Combatants)                                          */
/* ------------------------------------------------------------------ */
enum {
    CMB_PLAYER      = 0,    /* l  objet du joueur (ou premier chevalier)         */
    CMB_OPPONENT    = 4,    /* l  objet de l'adversaire                          */
    CMB_ACTIVE      = 8,    /* b  combat en cours                                */
    CMB_FONT        = 10,   /* l  police courante                                */
    CMB_CHOOSERS    = 14,   /* w  joueurs qui choisissent un chevalier           */
    CMB_END_DELAY   = 16,   /* b  images avant la fin du combat                  */
    CMB_MOON        = 18,   /* w  phase de la lune ($2D-$31)                     */
    CMB_DAY         = 20    /* w  jour                                           */
};

/* ------------------------------------------------------------------ */
/* Entité du moteur IMAGEXCEL (IX_ENTITY_SIZE = 50 octets, t_Entities)   */
/* ------------------------------------------------------------------ */
enum {
    ENT_ACTIVE      = 0,    /* b                                                 */
    ENT_BUSY        = 1,    /* b  script en cours                                */
    ENT_PC          = 2,    /* l  pointeur de script                             */
    ENT_X           = 6,    /* w                                                 */
    ENT_HEIGHT      = 8,    /* w                                                 */
    ENT_DEPTH       = 10,   /* w                                                 */
    ENT_DRAW_X      = 12,   /* w  position de dessin                             */
    ENT_DRAW_Y      = 14,   /* w                                                 */
    ENT_W           = 16,   /* w  taille de la frame                             */
    ENT_H           = 18,   /* w                                                 */
    ENT_FRAME       = 21,   /* b                                                 */
    ENT_DIR         = 22,   /* b  bit 1 : tourné à gauche                        */
    ENT_OBJ         = 24,   /* l  objet                                          */
    ENT_BANKS       = 28,   /* l  table des banques CEL                          */
    ENT_CTL         = 32,   /* b  contrôleur                                     */
    ENT_CTX         = 36,   /* l  contexte (CTX_*)                               */
    ENT_LIST_STRIKE = 40,   /* l  frames de frappe                               */
    ENT_LIST_BODY   = 44,   /* l  frames de corps                                */
    ENT_FROZEN      = 48    /* w  gelée                                          */
};

/* Contexte d'un script (IX_CTX_SIZE = 36 octets, ENT_CTX) */
enum {
    CTX_HOLD_N = 0, CTX_HOLD_ON = 1, CTX_HOLD_PC = 2,
    CTX_LOOP_N = 6, CTX_LOOP_ON = 7, CTX_LOOP_PC = 8,
    CTX_JUMP_PC = 12, CTX_JUMP_ON = 16,
    CTX_SHADOW_ON = 18, CTX_SHADOW_PC = 20,
    CTX_PHY_A = 24, CTX_PHY_FLAGS = 25, CTX_PHY_ON = 26, CTX_PHY_N = 27,
    CTX_PHY_VY = 28, CTX_PHY_VYMAX = 29, CTX_PHY_VX = 30, CTX_PHY_VXMAX = 31,
    CTX_RESUME_PC = 32
};

/* ------------------------------------------------------------------ */
/* CEL en mémoire (LAB_0CBB) : en-tête, puis frames de 10 octets         */
/* ------------------------------------------------------------------ */
enum {
    CEL_FRAMES      = 0,    /* w  nombre de frames                               */
    CEL_PIXELS      = 2,    /* l  adresse des pixels (fichier : taille compressée) */
    CEL_BITS        = 6,    /* l  taille décompressée en bits                    */
    CEL_TABLE       = 10,   /*    première entrée de frame                       */
    CEL_ENTRY_SIZE  = 10
};
enum {
    CELF_OFFSET     = 0,    /* l  décalage dans les pixels                       */
    CELF_W          = 4,    /* w                                                 */
    CELF_H          = 6,    /* w                                                 */
    CELF_FLAGS      = 8,    /* b  bit 0 sens d'origine ; sinon remplissage << 4  */
    CELF_PLANES     = 9     /* b  masque des plans                               */
};

/* ------------------------------------------------------------------ */
/* Zone d'un écran à pointeur (24 octets, b_Obstacles / t_ZoneTemplate)  */
/* ------------------------------------------------------------------ */
enum {
    ZONE_W          = 4,    /* w                                                 */
    ZONE_H          = 6,    /* w                                                 */
    ZONE_TEXT       = 8,    /* l  texte montré au survol                         */
    ZONE_X          = 12,   /* w                                                 */
    ZONE_Y          = 14,   /* w                                                 */
    ZONE_ID         = 16,   /* l  identifiant                                    */
    ZONE_KIND       = 20,   /* w  genre                                          */
    ZONE_SLOT       = 22,   /* w  case de l'inventaire                           */
    ZONE_SIZE       = 24
};

/* Enregistrement de texte (LAB_0432) */
enum {
    TXT_STRING      = 0,    /* l                                                 */
    TXT_X           = 4,    /* w                                                 */
    TXT_Y           = 6,    /* w                                                 */
    TXT_FLAGS_WORD  = 8,    /* w  drapeaux (écrits en mot)                       */
    TXT_FLAGS       = 9,    /* b  1 centré, 2 zone notée, 4 à droite, 8 serré    */
    TXT_NEXT        = 10,   /* l  enregistrement suivant (0 : fin)               */
    TXT_SIZE        = 14
};

/* Demande de vol (t_Trajectory, 20 octets, lue par LAB_02F6) */
enum {
    TRAJ_OBJ        = 0,    /* l  objet qui vole                                 */
    TRAJ_X          = 4,    /* w  départ                                         */
    TRAJ_DEPTH      = 6,    /* w                                                 */
    TRAJ_HEIGHT     = 8,    /* w                                                 */
    TRAJ_TO_X       = 10,   /* w  arrivée                                        */
    TRAJ_TO_DEPTH   = 12,   /* w                                                 */
    TRAJ_TO_HEIGHT  = 14,   /* w                                                 */
    TRAJ_STEPS      = 16,   /* w  nombre de pas                                  */
    TRAJ_ARC        = 18    /* w  hauteur de l'arc (vol à plat)                  */
};

/* Vol en cours (t_Trajectories, 6 × 20 octets, LAB_02FD) ; les
 * positions sont en virgule fixe (X/profondeur << 6, hauteur << 8) */
enum {
    FLY_OBJ         = 0,    /* l  objet (0 : libre)                              */
    FLY_COUNT       = 4,    /* w  pas restants                                   */
    FLY_VZ          = 6,    /* w  vitesse verticale                              */
    FLY_GRAVITY     = 8,    /* w                                                 */
    FLY_VX          = 10,   /* w                                                 */
    FLY_VD          = 12,   /* w  vitesse en profondeur                          */
    FLY_X           = 14,   /* w                                                 */
    FLY_DEPTH       = 16,   /* w                                                 */
    FLY_HEIGHT      = 18,   /* w                                                 */
    FLY_SIZE        = 20
};

#endif /* MOG_STRUCT_H */
