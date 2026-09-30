/*
 * ix_program_names.h — noms des labels de amiga_asm/program.asm utilisés par le C
 * (générés par tools/ix_names.py depuis game/data/program_names.txt ;
 * ne pas modifier).
 */
#ifndef IX_PROGRAM_NAMES_H
#define IX_PROGRAM_NAMES_H

#include "ix_program_syms.h"

#define PROGRAM_t_TheEndText PROGRAM_LAB_0002                /* écran « The End » */
#define PROGRAM_v_EndFlags PROGRAM_LAB_0005                  /* drapeaux de la fin, écrits par mog ($3E0 : $80 | chevalier | lieu) */
#define PROGRAM_Ctl_ScriptEnd PROGRAM_LAB_0014               /* contrôleur : fin du script (LAB_0120) */
#define PROGRAM_t_ProcessionCircle PROGRAM_LAB_0023          /* procession du cercle vu de haut (LAB_001A) */
#define PROGRAM_t_ProcessionForest PROGRAM_LAB_0024          /* procession des druides dans la forêt (LAB_001B) */
#define PROGRAM_t_ProcessionStonehenge PROGRAM_LAB_0025      /* procession à Stonehenge (LAB_001C) */
#define PROGRAM_v_ProcessionSide PROGRAM_LAB_0026            /* côté de l'entrée suivante (LAB_001F) */
#define PROGRAM_v_ProcessionIndex PROGRAM_L00_00602          /* personnage suivant de la procession */
#define PROGRAM_v_FrameCount PROGRAM_LAB_0028                /* images de la scène */
#define PROGRAM_v_ProcessionCount PROGRAM_LAB_0029           /* personnages de la procession */
#define PROGRAM_t_Procession PROGRAM_L00_0060A               /* liste des personnages de la procession */
#define PROGRAM_v_ProcessionGap PROGRAM_L00_0060E            /* images entre deux personnages */
#define PROGRAM_Call_EndGlows PROGRAM_LAB_0032               /* fin : pulsations des couleurs */
#define PROGRAM_v_EndGlowA PROGRAM_LAB_0033                  /* pulsation de la fin (couleur 12) */
#define PROGRAM_v_EndGlowB PROGRAM_LAB_0034                  /* pulsation de la fin (couleur 15) */
#define PROGRAM_v_EndGlowC PROGRAM_LAB_0035                  /* pulsation de la fin (couleur 23) */
#define PROGRAM_t_ProcessionEnding PROGRAM_LAB_003A          /* procession de la fin (LAB_0037) */
#define PROGRAM_v_OverlayMode PROGRAM_LAB_003E               /* mode du cercle de pierres (2 : ...) */
#define PROGRAM_Call_Lightning PROGRAM_LAB_003F              /* éclair */
#define PROGRAM_Call_Storm PROGRAM_LAB_0040                  /* orage (six éclairs) */
#define PROGRAM_t_FlashLightning PROGRAM_LAB_0042            /* palette de l'éclair */
#define PROGRAM_t_FlashStorm PROGRAM_LAB_0043                /* palette de l'orage */
#define PROGRAM_v_CelArea PROGRAM_LAB_0045                   /* zone des CEL de l'intro */
#define PROGRAM_s_MessagePiv PROGRAM_LAB_0052                /* "message.piv" */
#define PROGRAM_s_BoldF PROGRAM_LAB_0053                     /* "bold.f" */
#define PROGRAM_Vbl_Music PROGRAM_LAB_005C                   /* serveur VBL : une image de la musique */
#define PROGRAM_v_MusicSpeedCount PROGRAM_LAB_005E           /* compteur de vitesse (octet) */
#define PROGRAM_v_MusicSpeedCount2 PROGRAM_LAB_005F          /* second compteur (octet) */
#define PROGRAM_v_MusicServer PROGRAM_LAB_0060               /* serveur VBL de la musique (retiré à la fin) */
#define PROGRAM_t_Vibrato PROGRAM_LAB_0094                   /* table du vibrato (LAB_007E) */
#define PROGRAM_t_Periods PROGRAM_LAB_0095                   /* périodes des notes (arpèges) */
#define PROGRAM_v_MusicSpeed PROGRAM_LAB_0096                /* vitesse (6) */
#define PROGRAM_v_MusicPosition PROGRAM_L01_0069F            /* position dans la liste */
#define PROGRAM_v_MusicRow PROGRAM_L01_006A0                 /* ligne dans le motif */
#define PROGRAM_v_MusicTick PROGRAM_LAB_0099                 /* image dans la ligne */
#define PROGRAM_v_MusicFilter PROGRAM_L01_006A3              /* filtre (inversé par la commande E0) */
#define PROGRAM_v_MusicDmaOn PROGRAM_L01_006A4               /* voies à lancer (DMACON) */
#define PROGRAM_t_SampleStarts PROGRAM_LAB_009C              /* début de chaque instrument (31) */
#define PROGRAM_v_Chan0 PROGRAM_LAB_009D                     /* voie 0 du lecteur */
#define PROGRAM_v_Chan1 PROGRAM_LAB_009E                     /* voie 1 */
#define PROGRAM_v_Chan2 PROGRAM_LAB_009F                     /* voie 2 */
#define PROGRAM_v_Chan3 PROGRAM_LAB_00A0                     /* voie 3 */
#define PROGRAM_v_TextScreen PROGRAM_SECSTRT_2               /* écran de texte en cours (LAB_0054) */
#define PROGRAM_t_EndingText PROGRAM_LAB_00A2                /* écran de texte de la fin */
#define PROGRAM_t_IntroText PROGRAM_LAB_00AA                 /* écran de texte de l'intro */
#define PROGRAM_t_LastScreenText PROGRAM_LAB_00B0            /* dernier écran de la fin (LAB_003B) */
#define PROGRAM_v_ChipFree PROGRAM_LAB_00C2                  /* bloc chip du lanceur (LAB_0044 le découpe) */
#define PROGRAM_v_ChipSize PROGRAM_LAB_00C3                  /* taille du bloc chip */
#define PROGRAM_v_FastFree PROGRAM_LAB_00C4                  /* bloc fast du lanceur */
#define PROGRAM_v_FastSize PROGRAM_LAB_00C5                  /* taille du bloc fast */
#define PROGRAM_v_BgPlanes PROGRAM_LAB_00C6                  /* plans du décor courant */
#define PROGRAM_v_BgSceneA PROGRAM_LAB_00C7                  /* décor d'une scène (plans) */
#define PROGRAM_v_BgSceneB PROGRAM_LAB_00C8                  /* décor d'une scène */
#define PROGRAM_v_BgSceneC PROGRAM_LAB_00C9                  /* décor d'une scène (« mindscape ») */
#define PROGRAM_v_BgSceneD PROGRAM_LAB_00CA                  /* décor d'une scène */
#define PROGRAM_b_PivBg4 PROGRAM_LAB_00CB                    /* PIV compressé : bg4 / bg5 */
#define PROGRAM_b_PivBg5a PROGRAM_LAB_00CC                   /* PIV compressé : bg5a */
#define PROGRAM_b_PivBg3 PROGRAM_LAB_00CD                    /* PIV compressé : bg3 */
#define PROGRAM_b_PivBg2 PROGRAM_LAB_00CE                    /* PIV compressé : bg2 */
#define PROGRAM_b_PivBg2a PROGRAM_LAB_00CF                   /* PIV compressé : bg2a */
#define PROGRAM_v_FrameVbls PROGRAM_LAB_00D0                 /* VBL par image (avec LAB_0123) */
#define PROGRAM_v_SceneOverlay PROGRAM_LAB_00D1              /* cercle de pierres dessiné par-dessus */
#define PROGRAM_x_Druid PROGRAM_LAB_00D2                     /* grand druide (LAB_002C, plan 8) */
#define PROGRAM_x_DruidKnighting PROGRAM_LAB_00D3            /* druide qui adoube (LAB_002E) */
#define PROGRAM_x_DruidClose PROGRAM_LAB_00D4                /* grand druide de près (LAB_002C, plan 9) */
#define PROGRAM_x_EndingA PROGRAM_LAB_00D5                   /* fin : premier plan (LAB_0039) */
#define PROGRAM_x_KnightStonehenge PROGRAM_LAB_00D6          /* le chevalier à Stonehenge (LAB_002D) */
#define PROGRAM_x_CircleDruidD9 PROGRAM_LAB_00D9             /* druide du cercle (miroir) */
#define PROGRAM_x_CircleDruidDA PROGRAM_LAB_00DA             /* druide du cercle */
#define PROGRAM_x_CircleDruidDB PROGRAM_LAB_00DB             /* druide du cercle */
#define PROGRAM_x_CircleDruidDC PROGRAM_LAB_00DC             /* druide du cercle */
#define PROGRAM_x_CircleDruidDD PROGRAM_LAB_00DD             /* druide du cercle */
#define PROGRAM_x_CircleDruidDF PROGRAM_LAB_00DF             /* druide du cercle */
#define PROGRAM_x_CircleDruidE1 PROGRAM_LAB_00E1             /* druide du cercle */
#define PROGRAM_x_CentralDruid PROGRAM_LAB_00E3              /* druide central (LAB_001A) */
#define PROGRAM_x_CircleIdle PROGRAM_LAB_00E4                /* druides en cercle au repos */
#define PROGRAM_x_KnightEntersCircle PROGRAM_LAB_00E5        /* chevalier entrant dans le cercle */
#define PROGRAM_x_EndingStonehenge PROGRAM_LAB_00E6          /* fin : retour à Stonehenge */
#define PROGRAM_x_EndingCharA PROGRAM_LAB_00E8               /* fin : personnage (LAB_0037) */
#define PROGRAM_x_EndingCharB PROGRAM_LAB_00E9               /* fin : personnage (LAB_0037, LAB_0039) */
#define PROGRAM_x_EndingCharC PROGRAM_LAB_00EA               /* fin : personnage (LAB_0037) */
#define PROGRAM_x_EndingB PROGRAM_LAB_00EB                   /* fin : deuxième plan (LAB_0039) */
#define PROGRAM_x_EndingC PROGRAM_LAB_00EC                   /* fin : troisième plan (LAB_0039) */
#define PROGRAM_x_EndingCircle PROGRAM_LAB_00ED              /* fin : le cercle des druides */
#define PROGRAM_x_EndingMoonRise PROGRAM_LAB_00EE            /* fin : la montée vers la lune (LAB_003B) */
#define PROGRAM_v_LeftDepth PROGRAM_LAB_00EF                 /* profondeur des entités à gauche */
#define PROGRAM_v_RightDepth PROGRAM_LAB_00F0                /* profondeur des entités à droite */
#define PROGRAM_v_GlyphWidth PROGRAM_LAB_00F1                /* largeur de la lettre courante */
#define PROGRAM_v_GlyphHeight PROGRAM_LAB_00F2               /* hauteur de la lettre courante */
#define PROGRAM_v_TextX PROGRAM_LAB_00F3                     /* X de la lettre suivante */
#define PROGRAM_v_TextY PROGRAM_LAB_00F4                     /* Y de la ligne */
#define PROGRAM_v_TextLineX PROGRAM_LAB_00F5                 /* X de début de ligne */
#define PROGRAM_v_TextTopY PROGRAM_LAB_00F6                  /* Y de la première ligne */
#define PROGRAM_v_TextChar PROGRAM_LAB_00F7                  /* caractère courant (pointeur dans la chaîne) */
#define PROGRAM_t_FontGlyphFrame PROGRAM_LAB_00F8            /* frame de la police pour chaque caractère */
#define PROGRAM_v_TileH PROGRAM_LAB_00F9                     /* tuile : hauteur */
#define PROGRAM_v_TileW PROGRAM_LAB_00FA                     /* tuile : largeur */
#define PROGRAM_v_TileWCopy PROGRAM_LAB_00FB                 /* largeur copiée */
#define PROGRAM_v_TileHClip PROGRAM_LAB_00FC                 /* hauteur après découpe */
#define PROGRAM_t_TextBackgrounds PROGRAM_SECSTRT_3          /* fonds des écrans de texte */
#define PROGRAM_t_FontBank PROGRAM_LAB_011A                  /* banques de polices (+16 grande police, +20 fond des textes) */
#define PROGRAM_t_Controllers PROGRAM_LAB_011B               /* contrôleurs des entités */
#define PROGRAM_v_RestoreCount PROGRAM_LAB_011C              /* zones restaurées */
#define PROGRAM_v_ScenePalette PROGRAM_LAB_011D              /* palette de la scène */
#define PROGRAM_v_ScenePaletteMode PROGRAM_LAB_011E          /* 2 fondu, 3 vers le noir */
#define PROGRAM_v_ScenePaletteDone PROGRAM_LAB_011F          /* palette de la scène posée */
#define PROGRAM_v_ScriptEnded PROGRAM_LAB_0120               /* un script est fini (LAB_0014) */
#define PROGRAM_v_OverlayCel PROGRAM_LAB_0121                /* CEL du cercle de pierres */
#define PROGRAM_v_FrameStartVbl PROGRAM_LAB_0122             /* compteur de VBL au début de l'image */
#define PROGRAM_v_FrameVblsExtra PROGRAM_LAB_0123            /* VBL par image en plus */
#define PROGRAM_b_Music PROGRAM_LAB_0124                     /* module de musique (music.cmp puis vmusic.cmp) */
#define PROGRAM_s_Au1Cel PROGRAM_SECSTRT_8                   /* "au1.cel" */
#define PROGRAM_s_Li1Cel PROGRAM_LAB_0163                    /* "li1.cel" */
#define PROGRAM_s_Da1Cel PROGRAM_LAB_0164                    /* "da1.cel" */
#define PROGRAM_s_Dw1Cel PROGRAM_LAB_0165                    /* "dw1.cel" */
#define PROGRAM_s_Ha1Cel PROGRAM_LAB_0166                    /* "ha1.cel" */
#define PROGRAM_s_Ov1Cel PROGRAM_LAB_0167                    /* "ov1.cel" */
#define PROGRAM_s_Co1Cel PROGRAM_LAB_0168                    /* "co1.cel" */
#define PROGRAM_s_Dg1Cel PROGRAM_LAB_0169                    /* "dg1.cel" */
#define PROGRAM_s_Klift1Cel PROGRAM_LAB_016A                 /* "klift1.cel" */
#define PROGRAM_s_Bg4Piv PROGRAM_L08_0004B                   /* "bg4.piv" */
#define PROGRAM_s_Bg5aPiv PROGRAM_L08_00053                  /* "bg5a.piv" */
#define PROGRAM_s_Bg3Piv PROGRAM_LAB_016D                    /* "bg3.piv" */
#define PROGRAM_s_Bg2aPiv PROGRAM_LAB_016E                   /* "bg2a.piv" */
#define PROGRAM_s_Bg2Piv PROGRAM_L08_0006D                   /* "bg2.piv" */
#define PROGRAM_s_Bg5Piv PROGRAM_L08_00075                   /* "bg5.piv" */
#define PROGRAM_s_Bg7Piv PROGRAM_L08_0007D                   /* "bg7.piv" */
#define PROGRAM_s_Bg8Piv PROGRAM_L08_00085                   /* "bg8.piv" */
#define PROGRAM_s_Bg1aPiv PROGRAM_LAB_017E                   /* "bg1a.piv" */
#define PROGRAM_s_Bg1cPiv PROGRAM_L08_00105                  /* "bg1c.piv" */
#define PROGRAM_s_Bg1bPiv PROGRAM_LAB_0181                   /* "bg1b.piv" */
#define PROGRAM_s_IntroStile PROGRAM_L08_00117               /* "intro.stile" */
#define PROGRAM_s_CoStile PROGRAM_L08_00123                  /* "co.stile" */
#define PROGRAM_s_Mindscape PROGRAM_LAB_0184                 /* "mindscape" */
#define PROGRAM_s_MusicCmp PROGRAM_LAB_018D                  /* "music.cmp" */
#define PROGRAM_s_VmusicCmp PROGRAM_LAB_01B6                 /* "vmusic.cmp" */
#define PROGRAM_v_EndColourA PROGRAM_SECSTRT_9               /* couleur de la fin selon le chevalier (12) */
#define PROGRAM_v_EndColourB PROGRAM_LAB_01C9                /* couleur de la fin (15) */
#define PROGRAM_v_EndColourC PROGRAM_LAB_01CA                /* couleur de la fin (23) */
#define PROGRAM_t_PalForest PROGRAM_LAB_01CB                 /* palette de la forêt (bg1a ; défilement) */
#define PROGRAM_t_PalDruid PROGRAM_LAB_01CC                  /* palette du grand druide (bg1c) */
#define PROGRAM_t_PalCircle PROGRAM_LAB_01CD                 /* palette du cercle vu de haut (bg1b) */
#define PROGRAM_t_PalBg4 PROGRAM_LAB_01CE                    /* palette de bg4 / bg5 */
#define PROGRAM_t_PalBg5a PROGRAM_LAB_01CF                   /* palette de bg5a (adoubement) */
#define PROGRAM_t_PalBg3 PROGRAM_LAB_01D0                    /* palette de bg3 */
#define PROGRAM_t_PalBg2 PROGRAM_LAB_01D1                    /* palette de bg2 (Stonehenge) */
#define PROGRAM_t_PalBg2a PROGRAM_LAB_01D2                   /* palette de bg2a (le chevalier à Stonehenge) */
#define PROGRAM_IxOp80_SetDir PROGRAM_LAB_0215               /* commande : sens */
#define PROGRAM_IxOp84_Jump PROGRAM_LAB_0218                 /* commande : saut / suite */
#define PROGRAM_IxOp88_Hold PROGRAM_LAB_021A                 /* commande : répétition du groupe */
#define PROGRAM_IxOp8C_Skip PROGRAM_LAB_021E                 /* commande : +8 */
#define PROGRAM_IxOp94_Loop PROGRAM_LAB_021F                 /* commande : boucle */
#define PROGRAM_IxOp98_Nop PROGRAM_LAB_0220                  /* commande sans effet (RTS) */
#define PROGRAM_IxOpA4_Sound PROGRAM_LAB_0221                /* commande : son */
#define PROGRAM_IxOpA0_Move PROGRAM_LAB_0222                 /* commande : déplacement */
#define PROGRAM_IxOpB0_Nop PROGRAM_LAB_022C                  /* commande sans effet */
#define PROGRAM_IxOpAC_Nop PROGRAM_LAB_022D                  /* commande sans effet */
#define PROGRAM_IxOpB4_Call PROGRAM_LAB_022F                 /* commande : appel d'une routine */
#define PROGRAM_IxOpC8_Skip6 PROGRAM_LAB_0232                /* commande : +6 */
#define PROGRAM_IxOpB8_Skip6 PROGRAM_LAB_0233                /* commande : +6 */
#define PROGRAM_IxOpBC_Skip6 PROGRAM_LAB_0234                /* commande : +6 */
#define PROGRAM_IxOpC0_End PROGRAM_LAB_0235                  /* commande : fin de l'entité */
#define PROGRAM_IxOpC4_SetBank PROGRAM_LAB_0236              /* commande : planches */
#define PROGRAM_IxOpCC_IfZero PROGRAM_LAB_0237               /* commande : test (saut si nul) */
#define PROGRAM_IxOpD0_IfNonZero PROGRAM_LAB_023B            /* commande : test (saut si non nul) */
#define PROGRAM_IxOpD4_Reset PROGRAM_LAB_023F                /* commande : remise à zéro */
#define PROGRAM_IxOpA8_Nop PROGRAM_LAB_0241                  /* commande sans effet */
#define PROGRAM_v_FadeSpeed PROGRAM_LAB_0261                 /* vitesse du fondu (LAB_0260) */
#define PROGRAM_v_CopySrc PROGRAM_LAB_0266                   /* copie d'écran : source */
#define PROGRAM_v_CopyDst PROGRAM_LAB_0267                   /* copie d'écran : destination */
#define PROGRAM_t_PalBlack PROGRAM_LAB_026D                  /* palette noire (fondus) */
#define PROGRAM_v_BBoxX1 PROGRAM_SECSTRT_11                  /* boîte englobante : X droit */
#define PROGRAM_v_BBoxX0 PROGRAM_LAB_0270                    /* boîte englobante : X gauche */
#define PROGRAM_v_BBoxY0 PROGRAM_LAB_0271                    /* boîte englobante : Y haut */
#define PROGRAM_v_BBoxY1 PROGRAM_LAB_0272                    /* boîte englobante : Y bas */
#define PROGRAM_v_BBoxSet PROGRAM_LAB_0273                   /* boîte englobante commencée */
#define PROGRAM_t_PalCurrent PROGRAM_LAB_0274                /* palette courante (SECSTRT_31) */
#define PROGRAM_t_Banks PROGRAM_LAB_0276                     /* planches de CEL des entités */
#define PROGRAM_v_NoFreeEntity PROGRAM_LAB_0277              /* aucune entité libre (2) */
#define PROGRAM_v_EntIndex PROGRAM_LAB_0278                  /* entité en cours de dessin */
#define PROGRAM_v_RestoreFront PROGRAM_LAB_0279              /* zones à restaurer de cette image */
#define PROGRAM_v_RestoreBack PROGRAM_LAB_027A               /* zones de l'image précédente */
#define PROGRAM_v_EntRenderPtr PROGRAM_LAB_027B              /* entité dessinée */
#define PROGRAM_v_RestoreNext PROGRAM_LAB_027C               /* zone suivante à noter */
#define PROGRAM_v_DrawScreen PROGRAM_LAB_027D                /* écran de dessin */
#define PROGRAM_v_ListBody PROGRAM_LAB_027E                  /* liste des frames (A) */
#define PROGRAM_v_ListStrike PROGRAM_LAB_027F                /* liste des frames (B) */
#define PROGRAM_t_EntSheets PROGRAM_LAB_0281                 /* planches des entités */
#define PROGRAM_t_Entities PROGRAM_LAB_0282                  /* 40 entités de 42 octets */
#define PROGRAM_b_EntitySwap PROGRAM_LAB_0283                /* entité temporaire (tri) */
#define PROGRAM_t_Contexts PROGRAM_LAB_0284                  /* contextes des scripts ($30 octets) */
#define PROGRAM_v_ShadowCtx PROGRAM_LAB_0285                 /* contexte des ombres */
#define PROGRAM_b_RestoreA PROGRAM_LAB_0286                  /* zones à restaurer A */
#define PROGRAM_b_RestoreB PROGRAM_LAB_0287                  /* zones à restaurer B */
#define PROGRAM_t_IxOpcodes PROGRAM_LAB_0288                 /* commandes des scripts (octet >= $80) */
#define PROGRAM_v_TextRecord PROGRAM_LAB_0296                /* enregistrement de texte courant */
#define PROGRAM_v_TextWidth PROGRAM_LAB_029A                 /* largeur de la chaîne (LAB_0297) */
#define PROGRAM_Irq_Level1 PROGRAM_LAB_0326                  /* interruption de niveau 1 */
#define PROGRAM_Irq_Level2 PROGRAM_LAB_032A                  /* interruption de niveau 2 (clavier) */
#define PROGRAM_Irq_Level3 PROGRAM_LAB_0331                  /* interruption de niveau 3 (VBL) */
#define PROGRAM_Irq_Level4 PROGRAM_LAB_0337                  /* interruption de niveau 4 (son) */
#define PROGRAM_Irq_Level5 PROGRAM_LAB_033C                  /* interruption de niveau 5 */
#define PROGRAM_Irq_Level6 PROGRAM_LAB_033F                  /* interruption de niveau 6 */
#define PROGRAM_v_KeyPressed PROGRAM_SECSTRT_16              /* dernière touche (code) */
#define PROGRAM_v_VblWait PROGRAM_LAB_0363                   /* attente de VBL active */
#define PROGRAM_v_Joy0Dat PROGRAM_LAB_0368                   /* JOY0DAT gardé */
#define PROGRAM_v_Joy1Dat PROGRAM_LAB_0369                   /* JOY1DAT gardé */
#define PROGRAM_t_KeysDown PROGRAM_LAB_036D                  /* touches enfoncées (128 octets) */
#define PROGRAM_t_VblServers PROGRAM_LAB_0372                /* serveurs de la VBL */
#define PROGRAM_v_VblCounter PROGRAM_LAB_0379                /* compteur de VBL */
#define PROGRAM_v_FileSize PROGRAM_L18_0000E                 /* taille du fichier ouvert (répertoire) (jumeau) */
#define PROGRAM_v_FileError PROGRAM_L18_0001A                /* 0 ou -1 : fichier introuvable (LAB_0BB5) (jumeau) */
#define PROGRAM_v_RightButtonOff PROGRAM_L18_0003A           /* bouton droit ignoré */
#define PROGRAM_v_RightButtonCount PROGRAM_LAB_038D          /* compteur du bouton droit (POTGOR) */
#define PROGRAM_v_PivData PROGRAM_SECSTRT_20                 /* PIV en cours de décodage */
#define PROGRAM_v_PivPlanes PROGRAM_LAB_0434                 /* plans du PIV décodé (4 ou 5) */
#define PROGRAM_t_GfxSeedD PROGRAM_LAB_046C                  /* données des tables (LAB_046F) */
#define PROGRAM_t_GfxSeedE PROGRAM_LAB_046E                  /* données des tables */
#define PROGRAM_v_CelLastName PROGRAM_LAB_04A3               /* nom de la dernière CEL chargée (LAB_0CBB) (jumeau) */
#define PROGRAM_v_CelLastSize PROGRAM_LAB_04A4               /* place occupée par cette CEL (LAB_0CB6 la relit) (jumeau) */
#define PROGRAM_t_BitReverse PROGRAM_LAB_04B3                /* octet aux bits inversés */
#define PROGRAM_v_ScreenStride PROGRAM_SECSTRT_24            /* largeur d'une ligne d'écran en octets (jumeau) */
#define PROGRAM_t_DestPlanes PROGRAM_LAB_04D9                /* 5 pointeurs de plans de destination */
#define PROGRAM_v_DestPlane1 PROGRAM_LAB_04DA                /* plan de destination 1 */
#define PROGRAM_v_DestPlane2 PROGRAM_LAB_04DB                /* plan de destination 2 */
#define PROGRAM_v_DestPlane3 PROGRAM_LAB_04DC                /* plan de destination 3 */
#define PROGRAM_v_DestPlane4 PROGRAM_LAB_04DD                /* plan de destination 4 */
#define PROGRAM_v_CelPlanesMax PROGRAM_LAB_04DE              /* dernier plan dessiné (4 : 5 plans) */
#define PROGRAM_v_BlitByCpu PROGRAM_LAB_04DF                 /* copie des plans par le processeur */
#define PROGRAM_v_CelHeader PROGRAM_LAB_04F7                 /* copie de l'en-tête CEL (10 octets) : mot nombre de frames (jumeau) */
#define PROGRAM_v_CelHeaderPacked PROGRAM_LAB_04F8           /* en-tête CEL +2 : taille compressée (jumeau) */
#define PROGRAM_v_CelHeaderBits PROGRAM_LAB_04FA             /* en-tête CEL +6 : taille décompressée en bits (jumeau) */
#define PROGRAM_v_CelPlaneMask PROGRAM_LAB_04FC              /* masque des plans de la frame (table des frames +9) (jumeau) */
#define PROGRAM_v_CelSkipLeft PROGRAM_LAB_04FD               /* octets sautés à gauche (coupe) (jumeau) */
#define PROGRAM_v_CelSkipRight PROGRAM_LAB_04FE              /* octets sautés à droite (coupe) (jumeau) */
#define PROGRAM_v_CelClipLeft PROGRAM_LAB_04FF               /* frame coupée au bord gauche (jumeau) */
#define PROGRAM_v_CelClipRight PROGRAM_LAB_0500              /* frame coupée au bord droit (jumeau) */
#define PROGRAM_v_ClipHeight PROGRAM_LAB_0501                /* hauteur de la zone de dessin (200) */
#define PROGRAM_v_ClipWidth PROGRAM_LAB_0502                 /* largeur de la zone de dessin en octets (40) */
#define PROGRAM_v_CelDestOffset PROGRAM_LAB_0503             /* décalage ajouté à l'adresse de destination */
#define PROGRAM_v_CelPlaneSize PROGRAM_LAB_0504              /* taille d'un plan de la frame */
#define PROGRAM_v_CelShift PROGRAM_LAB_0505                  /* décalage en pixels (0-15) ; premier mot d'une liste copper (jumeau) */
#define PROGRAM_t_PivPalette PROGRAM_LAB_0506                /* palette du dernier PIV décodé */
#define PROGRAM_t_GfxSeedA PROGRAM_LAB_0507                  /* données des tables de pixels (LAB_04E4) */
#define PROGRAM_t_GfxTablesA PROGRAM_LAB_0508                /* pointeurs des tables de pixels (A) */
#define PROGRAM_t_GfxTablesB PROGRAM_LAB_0510                /* pointeurs des tables de pixels (B) */
#define PROGRAM_v_GfxTableB PROGRAM_LAB_0518                 /* table de conversion de pixels */
#define PROGRAM_v_GfxTableA PROGRAM_LAB_0519                 /* table de conversion de pixels */
#define PROGRAM_v_GfxTableC PROGRAM_LAB_051A                 /* table de conversion de pixels */
#define PROGRAM_v_CelPlanesBuf PROGRAM_LAB_051B              /* tampon de la frame (retournement) */
#define PROGRAM_v_GfxTableD PROGRAM_LAB_051C                 /* table de conversion de pixels (jumeau) */
#define PROGRAM_t_GfxSeedB PROGRAM_LAB_051D                  /* données des tables de pixels (LAB_04EC) */
#define PROGRAM_t_GfxSeedC PROGRAM_LAB_051E                  /* données des tables de pixels */
#define PROGRAM_v_ScreenStrideSet PROGRAM_LAB_0527           /* non nul : largeur de ligne v_ScreenStride au lieu de 40 (jumeau) */
#define PROGRAM_v_GfxReady PROGRAM_LAB_0528                  /* tables graphiques prêtes */
#define PROGRAM_b_Gfx PROGRAM_SECSTRT_27                     /* bloc des tables graphiques et du tampon de frame (jumeau) */
#define PROGRAM_b_Unpack PROGRAM_LAB_052A                    /* tampon des données compressées (41 244 octets) (jumeau) */
#define PROGRAM_v_ShowPlanes PROGRAM_SECSTRT_30              /* plans de l'écran montré */
#define PROGRAM_v_DrawPlanes PROGRAM_LAB_056C                /* plans de l'écran de dessin */
#define PROGRAM_t_PalDisplay PROGRAM_LAB_056E                /* palette de départ de l'affichage */
#define PROGRAM_v_DisplayInit PROGRAM_LAB_056F               /* valeur de départ de l'affichage (LAB_0570) */
#define PROGRAM_v_CopperList PROGRAM_LAB_0570                /* liste copper */
#define PROGRAM_Vbl_Screen PROGRAM_LAB_057D                  /* serveur VBL : écran montré (LAB_054C) */
#define PROGRAM_t_MusicFade PROGRAM_LAB_059D                 /* fondu de la musique (volumes) */
#define PROGRAM_t_ScrollSpeedLimits PROGRAM_LAB_05A9         /* bornes de vitesse selon la position */
#define PROGRAM_t_ScrollSpeeds PROGRAM_L31_00674             /* vitesses du défilement */
#define PROGRAM_Call_ScrollEnd PROGRAM_LAB_05AE              /* fin du défilement */
#define PROGRAM_Call_ScrollSlow PROGRAM_LAB_05AF             /* défilement ralenti */
#define PROGRAM_v_CreditIndex PROGRAM_LAB_05B0               /* texte suivant du générique (0-5) */
#define PROGRAM_t_CreditTexts PROGRAM_LAB_05B1               /* textes du générique */
#define PROGRAM_v_ScrollPos PROGRAM_LAB_05B8                 /* position dans la carte (lignes ; 0-1000) */
#define PROGRAM_v_ScrollSpeed PROGRAM_L31_00850              /* lignes par image */
#define PROGRAM_v_ScrollRows PROGRAM_LAB_05BC                /* lignes de tuiles montrées */
#define PROGRAM_v_ScrollTopRow PROGRAM_LAB_05BD              /* première ligne de la fenêtre */
#define PROGRAM_t_TileBankBase PROGRAM_L31_0090C             /* premier numéro de tuile de chaque planche */
#define PROGRAM_v_ScrollRow PROGRAM_LAB_05C3                 /* ligne de tuiles en cours */
#define PROGRAM_v_ScrollRowsLeft PROGRAM_L31_009DC           /* lignes restantes */
#define PROGRAM_v_MusicFading PROGRAM_SECSTRT_32             /* fondu de la musique demandé */
#define PROGRAM_v_PalFadeTarget PROGRAM_LAB_05CF             /* palette visée par le fondu */
#define PROGRAM_v_PalFadeDelay PROGRAM_LAB_05D0              /* VBL entre deux pas du fondu */
#define PROGRAM_v_PalFadeCount PROGRAM_LAB_05D1              /* VBL avant le pas suivant */
#define PROGRAM_v_PalCurrent PROGRAM_LAB_05D2                /* palette courante (pointeur) */
#define PROGRAM_t_ColourCycles PROGRAM_LAB_05D3              /* rotations de couleurs (LAB_0E56) (jumeau) */
#define PROGRAM_t_ColourGlows PROGRAM_LAB_05D4               /* pulsations de couleurs (jumeau) */
#define PROGRAM_b_TileMap PROGRAM_SECSTRT_33                 /* carte de tuiles (intro.stile / co.stile) */
#define PROGRAM_t_TileBanks PROGRAM_LAB_05D6                 /* planches de tuiles (3) */
#define PROGRAM_v_TileScreen PROGRAM_LAB_05D7                /* écran des tuiles */
#define PROGRAM_v_TileSrcX PROGRAM_LAB_05D8                  /* X de la tuile dans la planche */
#define PROGRAM_v_TileSrcY PROGRAM_LAB_05D9                  /* Y de la tuile dans la planche */
#define PROGRAM_v_TileX PROGRAM_LAB_05DA                     /* X de destination */
#define PROGRAM_v_TileY PROGRAM_LAB_05DB                     /* Y de destination */
#define PROGRAM_v_TileSheet PROGRAM_LAB_05DC                 /* planche de la tuile */
#define PROGRAM_v_TileDst PROGRAM_LAB_05DD                   /* écran de destination */
#define PROGRAM_v_TileIndex PROGRAM_LAB_05DE                 /* numéro de la tuile */
#define PROGRAM_v_TileClip05DF PROGRAM_LAB_05DF              /* découpe (remis à zéro) */
#define PROGRAM_v_TileSkipRows PROGRAM_LAB_05E0              /* octets sautés en haut (découpe) */
#define PROGRAM_v_TileClip05E1 PROGRAM_LAB_05E1              /* découpe (remis à zéro) */
#define PROGRAM_v_TileClip05E2 PROGRAM_LAB_05E2              /* découpe (remis à zéro) */
#define PROGRAM_v_TileClip05E3 PROGRAM_LAB_05E3              /* découpe (remis à zéro) */
#define PROGRAM_v_TileSrcPtr PROGRAM_LAB_05E4                /* source de la copie */
#define PROGRAM_v_TileDstPtr PROGRAM_LAB_05E5                /* destination de la copie */
#define PROGRAM_v_ScrollDone PROGRAM_LAB_05E6                /* fin du défilement (LAB_05AE) */
#define PROGRAM_v_IntroSkipped PROGRAM_LAB_05E7              /* intro sautée par une touche */

#endif
