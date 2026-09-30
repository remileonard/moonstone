/*
 * ix_mog_names.h — noms des labels de amiga_asm/mog.asm utilisés par le C
 * (générés par tools/ix_names.py depuis game/data/mog_names.txt ;
 * ne pas modifier).
 */
#ifndef IX_MOG_NAMES_H
#define IX_MOG_NAMES_H

#include "ix_mog_syms.h"

#define MOG_s_Dragon5Cel MOG_LAB_0122                        /* "Dragon5.cel" */
#define MOG_s_Test MOG_LAB_013B                              /* "Test" */
#define MOG_v_TextWidth MOG_LAB_0441                         /* largeur de la chaîne (LAB_043D) */
#define MOG_t_ChipBuffers MOG_LAB_05B8                       /* tampons en mémoire chip (+0 décor, +4 CEL du chevalier, +8 CEL des créatures...) */
#define MOG_t_FastBuffers MOG_LAB_05B9                       /* tampons en mémoire fast (+8 décors « Test », +84 texte de collide.hit, +88 points d'impact...) */
#define MOG_b_BloCel MOG_LAB_05BB                            /* CEL blo.cel (banque d'effets LAB_0648) */
#define MOG_v_ChipFree MOG_LAB_05BC                          /* bloc chip du lanceur, puis début de sa partie libre */
#define MOG_v_ChipSize MOG_LAB_05BD                          /* taille du bloc chip */
#define MOG_v_FastFree MOG_LAB_05BE                          /* bloc fast du lanceur, puis début de sa partie libre */
#define MOG_v_FastSize MOG_LAB_05BF                          /* taille du bloc fast */
#define MOG_v_BgPlanes MOG_LAB_05C0                          /* plans du décor (5 × 8000 octets) */
#define MOG_v_SheetPlanes MOG_LAB_05C1                       /* planche d'images décodée (blocs $03xx du terrain, icônes du menu) */
#define MOG_b_Piv MOG_LAB_05C2                               /* tampon d'un PIV compressé avant décodage */
#define MOG_v_Objects MOG_LAB_05C3                           /* les 20 objets du combat (IX_OBJECT_SIZE octets chacun) */
#define MOG_v_CursorCel MOG_LAB_05C4                         /* CEL du curseur (menu) ou de la carte (LAB_05E2+4) */
#define MOG_v_Players MOG_LAB_05C5                           /* nombre de joueurs humains (1 à 4) */
#define MOG_v_MapCreatures MOG_LAB_05C6                      /* créatures (repaires) de la carte : 24 × 20 octets (= LAB_05B9+68) */
#define MOG_b_SoundsKnight MOG_LAB_05C7                      /* banque de sons du chevalier (kn.a) */
#define MOG_b_SoundsCreature MOG_LAB_05C8                    /* banque de sons de la créature (une à la fois) */
#define MOG_b_SoundsReplay MOG_LAB_05C9                      /* banque Re.a */
#define MOG_b_SoundsWizard MOG_LAB_05CA                      /* banque Wz.a */
#define MOG_b_SoundsRatmen MOG_LAB_05CB                      /* banque Ra.a */
#define MOG_v_ReversedKnight MOG_LAB_05D1                    /* chevalier aux commandes inversées (LAB_05D3) */
#define MOG_v_Defender MOG_LAB_05D2                          /* chevalier attaqué (peut fuir, LAB_0058) */
#define MOG_v_ReversedOn MOG_LAB_05D3                        /* commandes inversées actives */
#define MOG_v_NameCursorChar MOG_LAB_05D4                    /* caractère du curseur de saisie du nom */
#define MOG_v_NameCursorAnim MOG_LAB_05D5                    /* animation du curseur (LAB_00E0) */
#define MOG_v_NameLength MOG_LAB_05D6                        /* position dans le nom saisi */
#define MOG_v_NameEditing MOG_LAB_05D7                       /* saisie du nom en cours */
#define MOG_v_MenuCursorX MOG_LAB_05D8                       /* X du curseur du menu */
#define MOG_v_MenuCursorY MOG_LAB_05D9                       /* Y du curseur du menu */
#define MOG_v_MenuCursorY0 MOG_LAB_05DA                      /* Y de la première ligne du menu */
#define MOG_v_PlayersSaved MOG_LAB_05DB                      /* nombre de joueurs gardé pendant un entraînement */
#define MOG_v_KnightsDown MOG_LAB_05DC                       /* chevaliers à terre : bit 0 le premier, bit 1 le second */
#define MOG_v_Unused05DD MOG_LAB_05DD                        /* mis à $FF au démarrage */
#define MOG_v_Unused05DE MOG_LAB_05DE                        /* mis à $FF au démarrage */
#define MOG_v_CreatureLoaded MOG_LAB_05DF                    /* créature dont les CEL sont chargées (n° de contrôleur, $FF aucune) */
#define MOG_t_BankEnemy MOG_LAB_05E0                         /* banque CEL de l'adversaire (5 CEL) */
#define MOG_t_BankKnight MOG_LAB_05E1                        /* banque CEL du chevalier (kn1..kn5) */
#define MOG_t_BankMap MOG_LAB_05E2                           /* banque CEL de la carte */
#define MOG_t_FontBank MOG_LAB_05E3                          /* CEL des polices : +0 petite (LAB_078C), +16 grande (LAB_078B) */
#define MOG_t_PalTownA MOG_LAB_05E5                          /* palette d'une ville (LAB_04C5) */
#define MOG_t_PalTownB MOG_LAB_05E6                          /* palette d'une ville (écran suivant) */
#define MOG_v_MessageText MOG_LAB_05E7                       /* enregistrements de texte de l'écran de message */
#define MOG_v_TerrainNextSwamp MOG_LAB_05E8                  /* terrain suivant (marais, LAB_07B8) */
#define MOG_v_TerrainNextWater MOG_LAB_05E9                  /* terrain suivant (eau, LAB_07B6) */
#define MOG_v_TerrainNextGlade MOG_LAB_05EA                  /* terrain suivant (clairière, LAB_07B7) */
#define MOG_v_TerrainNextForest MOG_LAB_05EB                 /* terrain suivant (forêt, LAB_07B9 ; octet) */
#define MOG_v_FoesToBeat MOG_LAB_05EC                        /* adversaires à vaincre */
#define MOG_v_FoesAtOnce MOG_LAB_05ED                        /* adversaires présents à la fois */
#define MOG_v_FoesEntered MOG_LAB_05EE                       /* adversaires entrés */
#define MOG_v_EntrySide MOG_LAB_05EF                         /* côté d'entrée alterné (0 / 1) */
#define MOG_v_NextFoeFn MOG_LAB_05F0                         /* routine « adversaire suivant » */
#define MOG_v_FoeKitFn MOG_LAB_05F1                          /* routine d'équipement de l'adversaire */
#define MOG_v_PlayerObj MOG_LAB_05F2                         /* objet du chevalier du joueur */
#define MOG_v_Unused05F3 MOG_LAB_05F3                        /* mis à 1 par la nouvelle partie */
#define MOG_v_DemonCompanion MOG_LAB_05F4                    /* entité du compagnon du Démon (hors gel) */
#define MOG_t_KnightAttacks MOG_LAB_05F5                     /* attaques du chevalier (9 directions) */
#define MOG_t_KnightScripts MOG_LAB_05F6                     /* scripts du chevalier */
#define MOG_t_KnightDamage MOG_LAB_05F7                      /* dégâts du chevalier */
#define MOG_t_KnightField50 MOG_LAB_05F8                     /* table +50 de l'objet du chevalier */
#define MOG_t_TroggSpearScripts MOG_LAB_05F9                 /* scripts du Trogg à lance (LAB_0176) */
#define MOG_t_TroggADamage MOG_LAB_05FA                      /* dégâts du Trogg (LAB_0169) */
#define MOG_t_TroggAAttacks MOG_LAB_05FB                     /* attaques du Trogg (LAB_0169) */
#define MOG_t_TroggAScripts MOG_LAB_05FC                     /* scripts du Trogg (LAB_0169) */
#define MOG_t_TroggBDamage MOG_LAB_05FD                      /* dégâts du Trogg à hache (LAB_0170) */
#define MOG_t_TroggBAttacks MOG_LAB_05FE                     /* attaques du Trogg à hache (LAB_0170) */
#define MOG_t_TroggBScripts MOG_LAB_05FF                     /* scripts du Trogg à hache (LAB_0170) */
#define MOG_t_PassingKnightScripts MOG_LAB_0600              /* scripts du chevalier de passage (LAB_018B) */
#define MOG_t_RatmenDamage MOG_LAB_0601                      /* dégâts des hommes-rats */
#define MOG_t_RatmenScripts MOG_LAB_0602                     /* scripts des hommes-rats */
#define MOG_t_DragonDamage MOG_LAB_0603                      /* dégâts du Dragon (LAB_0195) */
#define MOG_t_DragonScripts MOG_LAB_0604                     /* scripts du Dragon (LAB_0195) */
#define MOG_t_BalokDamage MOG_LAB_0605                       /* dégâts de Balok (LAB_0198) */
#define MOG_t_MudmenDamage MOG_LAB_0606                      /* dégâts des Mudmen (LAB_019D) */
#define MOG_t_MudmenScripts MOG_LAB_0607                     /* scripts des Mudmen */
#define MOG_t_MudmenWalk MOG_LAB_0608                        /* marche des Mudmen */
#define MOG_t_TrollDamage MOG_LAB_0609                       /* dégâts du Troll (LAB_019F) */
#define MOG_t_TrollScripts MOG_LAB_060A                      /* scripts du Troll */
#define MOG_t_TrollWalk MOG_LAB_060B                         /* marche du Troll */
#define MOG_t_TroggAWalk MOG_LAB_060C                        /* marche du Trogg (LAB_0169) */
#define MOG_t_TroggSpearWalk MOG_LAB_060D                    /* marche du Trogg à lance */
#define MOG_t_TroggBWalk MOG_LAB_060E                        /* marche du Trogg à hache */
#define MOG_t_DragonWalk MOG_LAB_060F                        /* marche du Dragon */
#define MOG_t_KnightWalk MOG_LAB_0610                        /* marche du chevalier */
#define MOG_t_PassingKnightWalk MOG_LAB_0611                 /* marche du chevalier de passage */
#define MOG_t_RatmenWalk MOG_LAB_0612                        /* marche des hommes-rats */
#define MOG_t_KnightObjects MOG_LAB_0613                     /* objets des 4 chevaliers (IX_OBJECT_SIZE chacun) */
#define MOG_v_Knight2Obj MOG_LAB_0614                        /* objet du deuxième chevalier */
#define MOG_v_DragonObj MOG_LAB_0617                         /* objet du Dragon */
#define MOG_t_KnightInventories MOG_LAB_0618                 /* inventaires des 4 chevaliers (24 octets chacun) */
#define MOG_t_DragonInventory MOG_LAB_0619                   /* inventaire du Dragon */
#define MOG_v_EntryDepth3 MOG_LAB_061A                       /* profondeur d'entrée du rang 3 (et du Démon) */
#define MOG_v_EntryDepth2 MOG_LAB_061B                       /* profondeur d'entrée du rang 2 */
#define MOG_v_EntryDepth1 MOG_LAB_061C                       /* profondeur d'entrée du rang 1 */
#define MOG_v_StepX MOG_LAB_061E                             /* pas de marche en X du contrôleur */
#define MOG_v_StepY MOG_LAB_061F                             /* pas de marche en Y */
#define MOG_v_EnemyActed MOG_LAB_0620                        /* un adversaire a agi cette image (LAB_0240) */
#define MOG_t_HitByFn MOG_LAB_0621                           /* réaction au coup reçu, par contrôleur de l'attaquant */
#define MOG_t_HitFn MOG_LAB_0622                             /* réaction après avoir touché, par contrôleur de la cible */
#define MOG_v_DragonState MOG_LAB_0623                       /* état du Dragon (bits) */
#define MOG_v_DragonDist MOG_LAB_0624                        /* distance du Dragon au chevalier */
#define MOG_v_FaceX MOG_LAB_0625                             /* X de la cible (tourner vers elle) */
#define MOG_v_FaceY MOG_LAB_0626                             /* Y de la cible */
#define MOG_v_LeapState MOG_LAB_0627                         /* état de la créature qui bondit (LAB_029F) */
#define MOG_v_ChaseStepY MOG_LAB_0628                        /* pas de poursuite en Y */
#define MOG_v_ChaseStepX MOG_LAB_0629                        /* pas de poursuite en X */
#define MOG_v_LeapSteps MOG_LAB_062A                         /* pas restants du bond */
#define MOG_v_KnightAiFlags MOG_LAB_062B                     /* drapeaux du chevalier géré par l'ordinateur */
#define MOG_v_KnightAiDelay MOG_LAB_062C                     /* délai du chevalier géré par l'ordinateur */
#define MOG_v_LeapToggle MOG_LAB_062D                        /* bascule 0 / 1 de la créature qui bondit */
#define MOG_t_Trajectory MOG_LAB_062E                        /* trajectoire de l'objet vers la cible (LAB_02D3) */
#define MOG_v_Joy0 MOG_LAB_062F                              /* manette du port 0 (LAB_00EE) */
#define MOG_v_Joy1 MOG_LAB_0630                              /* manette du port 1 */
#define MOG_v_SpawnCount MOG_LAB_0631                        /* entités parcourues (LAB_0009) */
#define MOG_v_LoadPtr MOG_LAB_0632                           /* adresse de chargement suivante (CEL, inventaire affiché) */
#define MOG_v_CurObj MOG_LAB_0633                            /* objet courant du contrôleur */
#define MOG_v_TargetObj MOG_LAB_0634                         /* cible de l'objet courant */
#define MOG_v_WalkDx MOG_LAB_0635                            /* déplacement X de la marche */
#define MOG_v_WalkDir MOG_LAB_0636                           /* direction de la marche (0-3) */
#define MOG_v_WalkObj MOG_LAB_0637                           /* objet qui marche */
#define MOG_v_BBoxX1 MOG_LAB_0638                            /* boîte englobante : X droit (moteur) */
#define MOG_v_BBoxX0 MOG_LAB_0639                            /* boîte englobante : X gauche */
#define MOG_v_BBoxY0 MOG_LAB_063A                            /* boîte englobante : Y haut */
#define MOG_v_BBoxY1 MOG_LAB_063B                            /* boîte englobante : Y bas */
#define MOG_v_BBoxSet MOG_LAB_063C                           /* boîte englobante commencée */
#define MOG_v_LoopIndex MOG_LAB_063D                         /* index de l'entité dans la boucle du moteur */
#define MOG_v_RestoreFront MOG_LAB_063E                      /* pile des zones à restaurer de cette image */
#define MOG_v_RestoreBack MOG_LAB_063F                       /* pile de l'image précédente (échangées) */
#define MOG_v_LoopEntity MOG_LAB_0640                        /* entité courante de la boucle */
#define MOG_v_RestoreNext MOG_LAB_0641                       /* pile des zones d'écran à restaurer : prochaine entrée */
#define MOG_v_DrawScreen MOG_LAB_0642                        /* écran de dessin (copie de LAB_0D92) */
#define MOG_v_ListBody MOG_LAB_0643                          /* liste des frames de corps (collisions) */
#define MOG_v_ListStrike MOG_LAB_0644                        /* liste des frames de frappe */
#define MOG_v_RestoreCount MOG_LAB_0645                      /* nombre de zones à restaurer */
#define MOG_t_Banks MOG_LAB_0647                             /* banques CEL du moteur : LAB_05E1, LAB_05E0, LAB_05E2, LAB_0648 */
#define MOG_t_BankBlo MOG_LAB_0648                           /* banque CEL des effets (blo.cel) */
#define MOG_b_EntitySwap MOG_LAB_064A                        /* entité temporaire (tri par profondeur) */
#define MOG_t_Contexts MOG_LAB_064B                          /* contextes des scripts (IX_CTX_SIZE chacun) */
#define MOG_v_ShadowCtx MOG_LAB_064C                         /* contexte des ombres */
#define MOG_b_RestoreA MOG_LAB_064D                          /* pile de zones à restaurer A */
#define MOG_b_RestoreB MOG_LAB_064E                          /* pile de zones à restaurer B */
#define MOG_t_KnightDist MOG_LAB_0651                        /* distances aux autres chevaliers (triées, LAB_0DED) */
#define MOG_t_KnightByDist MOG_LAB_0652                      /* les autres chevaliers dans l'ordre des distances */
#define MOG_v_PlaceDigit MOG_LAB_0653                        /* chiffre de la ligne du menu des lieux */
#define MOG_v_TurnKnight MOG_LAB_0654                        /* chevalier dont c'est le tour (0-3) */
#define MOG_v_MovesUsed MOG_LAB_0655                         /* déplacements faits ce tour */
#define MOG_v_MoveDirs MOG_LAB_0656                          /* directions du pas (bits 0 droite, 1 gauche, 2 bas, 3 haut, 4 feu) */
#define MOG_v_MapEdgeBits MOG_LAB_0657                       /* directions encore possibles au bord de la carte (LAB_0E07) */
#define MOG_v_MapColoursOn MOG_LAB_0658                      /* effets de couleur de la carte actifs */
#define MOG_v_MovesQuarter MOG_LAB_0659                      /* quart des déplacements du tour */
#define MOG_v_MovesThreeQuarters MOG_LAB_065A                /* trois quarts des déplacements */
#define MOG_v_Boots2On MOG_LAB_065C                          /* bottes : chevalier replacé (LAB_0E05) */
#define MOG_v_BootsOn MOG_LAB_065E                           /* bottes actives (pas de combat au repaire) */
#define MOG_v_BootsPos MOG_LAB_065F                          /* position mémorisée (X, Y) pour les bottes */
#define MOG_v_Boots2Flag MOG_LAB_0660                        /* position mémorisée par LAB_0E05 */
#define MOG_v_WaterGlow MOG_LAB_0661                         /* pulsation de l'eau de la carte */
#define MOG_v_MapKeysOff MOG_LAB_0662                        /* touches de la carte ignorées */
#define MOG_v_DeadPlayers MOG_LAB_0663                       /* chevaliers des joueurs morts (fin de partie) */
#define MOG_b_MapIcons MOG_LAB_0664                          /* CEL des icônes de la carte */
#define MOG_v_MovesMax MOG_LAB_0665                          /* déplacements du tour (16 × mouvement) */
#define MOG_v_DragonDiveTimer MOG_LAB_0666                   /* compteur du vol du dragon (100 images) */
#define MOG_v_DragonFlying MOG_LAB_0667                      /* le dragon survole la carte */
#define MOG_v_MovesMark1 MOG_LAB_0668                        /* repère de déplacements franchi (1) */
#define MOG_v_MovesMark2 MOG_LAB_0669                        /* repère franchi (2) */
#define MOG_v_MovesMark3 MOG_LAB_066A                        /* repère franchi (3) */
#define MOG_v_AiPotionUsed MOG_LAB_066B                      /* le chevalier noir a bu une potion ce tour */
#define MOG_v_AiTownX MOG_LAB_066C                           /* ville visée par le chevalier noir : X (mot), 0 aucune */
#define MOG_v_AiTownY MOG_LAB_066D                           /* ville visée : Y */
#define MOG_v_SavedCurObj MOG_LAB_066E                       /* objet courant gardé pendant les entités */
#define MOG_v_DistTownA MOG_LAB_066F                         /* distance à la ville (12, 7) */
#define MOG_v_DistTownB MOG_LAB_0670                         /* distance à la ville (37, 20) */
#define MOG_t_BankMapDragon MOG_LAB_0671                     /* banque CEL du dragon sur la carte */
#define MOG_t_CreaturesByDist MOG_LAB_0672                   /* créatures errantes triées par distance (6 octets) */
#define MOG_v_AiCreature MOG_LAB_0673                        /* créature visée par le chevalier noir */
#define MOG_v_SteerReady MOG_LAB_0674                        /* ligne vers la cible calculée */
#define MOG_v_SteerDirX MOG_LAB_0675                         /* bit de direction en X */
#define MOG_v_SteerDirY MOG_LAB_0676                         /* bit de direction en Y */
#define MOG_v_SteerError MOG_LAB_0677                        /* erreur de la ligne de Bresenham */
#define MOG_v_SteerDx MOG_LAB_0678                           /* |dx| */
#define MOG_v_SteerDy MOG_LAB_0679                           /* |dy| */
#define MOG_v_SteerMajorY MOG_LAB_067A                       /* Y est l'axe principal (-1) */
#define MOG_v_CreaturesSorted MOG_LAB_067B                   /* créatures triées ce tour */
#define MOG_v_AiCreatureGone MOG_LAB_067C                    /* la créature visée a disparu */
#define MOG_v_TownTextX MOG_LAB_067D                         /* X du texte suivant (villes) */
#define MOG_v_TownTextY MOG_LAB_067E                         /* Y du texte suivant */
#define MOG_v_TownTextFlags MOG_LAB_067F                     /* drapeaux du texte suivant */
#define MOG_v_LivesIconAlt MOG_LAB_0680                      /* variante de l'icône des vies */
#define MOG_v_IconFrame MOG_LAB_0681                         /* icône dessinée : frame */
#define MOG_v_IconX MOG_LAB_0682                             /* X */
#define MOG_v_IconY MOG_LAB_0683                             /* Y */
#define MOG_v_IconStep MOG_LAB_0684                          /* pas entre deux icônes */
#define MOG_v_IconKind MOG_LAB_0685                          /* genre de la zone */
#define MOG_v_IconId MOG_LAB_0686                            /* identifiant de la zone */
#define MOG_v_IconSlot MOG_LAB_0687                          /* case de l'inventaire */
#define MOG_v_IconTexts MOG_LAB_0688                         /* textes des zones (LAB_0699 ou LAB_069A) */
#define MOG_v_LootTaken MOG_LAB_0689                         /* butin pris */
#define MOG_v_ScreenKindSaved MOG_LAB_068A                   /* genre d'écran gardé */
#define MOG_v_ScreenKnight MOG_LAB_068B                      /* chevalier du panneau de gauche */
#define MOG_v_ScreenInventory MOG_LAB_068C                   /* son inventaire */
#define MOG_v_ScreenOther MOG_LAB_068D                       /* objet du panneau de droite */
#define MOG_v_ScreenOtherInv MOG_LAB_068E                    /* son inventaire */
#define MOG_v_ScreenKind MOG_LAB_068F                        /* genre d'écran (1 duel, 2 repaire, 9 inventaire, 10 dragon...) */
#define MOG_t_ShopInventory MOG_LAB_0690                     /* inventaire du marchand */
#define MOG_t_ShopPrices MOG_LAB_0691                        /* prix des objets de la boutique */
#define MOG_t_TextsTrain MOG_LAB_0692                        /* textes « améliorer » des caractéristiques */
#define MOG_t_TextsInventory MOG_LAB_0693                    /* textes de l'inventaire (écran 9) */
#define MOG_t_TextsLoot MOG_LAB_0694                         /* textes du butin (écrans 1, 2, 8, 10) */
#define MOG_t_TextsShop MOG_LAB_0695                         /* textes de la boutique (écrans 5, 6) */
#define MOG_t_TextsScreen3 MOG_LAB_0696                      /* textes de l'écran 3 */
#define MOG_t_TextsScreen6 MOG_LAB_0697                      /* textes du panneau de gauche de l'écran 6 */
#define MOG_t_TextsDefault MOG_LAB_0698                      /* textes par défaut */
#define MOG_b_ZoneTextsLeft MOG_LAB_0699                     /* textes des zones du panneau de gauche (27 × 14 octets) */
#define MOG_b_ZoneTextsRight MOG_LAB_069A                    /* textes des zones du panneau de droite */
#define MOG_t_PlacesReached MOG_SECSTRT_2                    /* lieux atteints (8 octets : objet ou position, genre) */
#define MOG_v_PlaceFound MOG_LAB_069C                        /* dernier lieu atteint (genre, -1 aucun) */
#define MOG_v_PlaceFoundX MOG_LAB_069D                       /* X du dernier lieu atteint */
#define MOG_t_UnderDragon MOG_LAB_069E                       /* chevaliers sous le dragon */
#define MOG_t_Places MOG_LAB_069F                            /* lieux de la carte (icône, X, Y ; -1 fin) */
#define MOG_t_TrainCostByPlayers MOG_SECSTRT_4               /* expérience requise selon le nombre de joueurs */
#define MOG_t_MenuTexts MOG_LAB_06AE                         /* textes du menu */
#define MOG_t_MenuGoreText MOG_LAB_06B3                      /* texte de la ligne « Gore » (On / Off) */
#define MOG_v_NameEdited MOG_LAB_06B4                        /* nom en cours de saisie */
#define MOG_s_SirRichard MOG_LAB_06B5                        /* nom du chevalier 2 */
#define MOG_s_SirGodber MOG_LAB_06B6                         /* nom du chevalier 1 */
#define MOG_s_SirJeffrey MOG_LAB_06B7                        /* nom du chevalier 3 */
#define MOG_s_SirEdward MOG_LAB_06B8                         /* nom du chevalier 4 */
#define MOG_s_PlayersCount MOG_LAB_06B9                      /* nombre de joueurs écrit */
#define MOG_s_On MOG_LAB_06BA                                /* "On" */
#define MOG_s_Off MOG_LAB_06BB                               /* "Off" */
#define MOG_v_Round MOG_LAB_06C0                             /* manche (jours) */
#define MOG_v_LevelCycle MOG_LAB_06C1                        /* cycle de niveau des créatures (0-7) */
#define MOG_t_LevelCycle MOG_LAB_06C2                        /* niveaux du cycle ("-/.010./") */
#define MOG_t_FleeMessage MOG_LAB_06C3                       /* message de fuite */
#define MOG_s_FleeName MOG_LAB_06C8                          /* nom du chevalier qui fuit */
#define MOG_t_EndLostMessage MOG_LAB_06CD                    /* message de fin (Démon non vaincu) */
#define MOG_t_EndWonMessage MOG_LAB_06D1                     /* message de fin (partie gagnée) */
#define MOG_v_Gore MOG_LAB_06DA                              /* Gore (sang) activé */
#define MOG_v_MenuRedraw MOG_LAB_06DB                        /* menu à redessiner */
#define MOG_v_MenuLine MOG_LAB_06DC                          /* ligne du menu */
#define MOG_t_MenuLineY MOG_LAB_06DD                         /* Y des lignes du menu */
#define MOG_v_TrainCost MOG_LAB_06DE                         /* expérience pour une caractéristique */
#define MOG_t_DemonLairMessage MOG_LAB_06E1                  /* message du repaire du Démon */
#define MOG_t_GameOverMessage MOG_LAB_06E6                   /* message de fin de partie */
#define MOG_t_SelectKnightText MOG_LAB_06E7                  /* titre du choix des chevaliers */
#define MOG_t_DemonKeyMessage MOG_LAB_06E8                   /* message : clé du repaire manquante */
#define MOG_v_ChooseKnightObj MOG_LAB_06F7                   /* objet du chevalier à choisir */
#define MOG_v_ChooseLeft MOG_LAB_06F8                        /* joueurs restant à choisir */
#define MOG_v_KnightsFree MOG_LAB_06F9                       /* chevaliers libres (bits) */
#define MOG_t_MenuPalette MOG_LAB_06FD                       /* palette du menu */
#define MOG_t_KnightChoiceX MOG_LAB_0702                     /* X des quatre chevaliers du choix */
#define MOG_v_KnightChoice MOG_LAB_0703                      /* chevalier sous le curseur du choix */
#define MOG_v_TownPlanes MOG_LAB_0704                        /* plans de l'image de la ville (= LAB_05C1) */
#define MOG_v_BoldFont MOG_LAB_0705                          /* grande police (copie de LAB_05E3+16) */
#define MOG_t_NextDayText MOG_LAB_0709                       /* texte de l'écran « jour suivant » */
#define MOG_s_MiC MOG_LAB_070E                               /* "mi.c" (icônes de la carte) */
#define MOG_t_TownWaitMessage MOG_LAB_070F                   /* message d'attente avant la ville */
#define MOG_t_TownWaitText MOG_LAB_0713                      /* son enregistrement de texte (nom de la ville) */
#define MOG_v_KnightCtlFn MOG_LAB_0714                       /* contrôleur du chevalier humain (LAB_029F ; changé si copie) */
#define MOG_s_HighWoodPiv MOG_LAB_0715                       /* "HighWood.piv" */
#define MOG_s_WaterDeepPiv MOG_LAB_0716                      /* "WaterDeep.piv" */
#define MOG_s_Highwood MOG_LAB_071B                          /* "Highwood" */
#define MOG_s_Waterdeep MOG_LAB_071C                         /* "Waterdeep" */
#define MOG_v_LoadingTextIndex MOG_LAB_071D                  /* texte suivant de l'écran de chargement (0-13) */
#define MOG_t_LoadingTexts MOG_LAB_071E                      /* textes de l'écran de chargement */
#define MOG_v_TerrainMode MOG_LAB_076D                       /* 2 : terrain du repaire (LAB_076E) au lieu du cycle */
#define MOG_v_LairTerrain MOG_LAB_076E                       /* fichier .t du repaire */
#define MOG_s_Kn1Ob MOG_LAB_076F                             /* "kn1.ob" */
#define MOG_s_Kn2Ob MOG_LAB_0770                             /* "kn2.ob" */
#define MOG_s_Kn3Ob MOG_LAB_0771                             /* "kn3.ob" */
#define MOG_s_Kn4Ob MOG_LAB_0772                             /* "kn4.ob" */
#define MOG_s_Kn5Ob MOG_LAB_0773                             /* "Kn5.ob" */
#define MOG_s_BloCel MOG_LAB_0774                            /* "blo.cel" */
#define MOG_s_He1Ob MOG_LAB_0775                             /* "He1.ob" */
#define MOG_s_He2Ob MOG_LAB_0776                             /* "He2.ob" */
#define MOG_s_He3Ob MOG_LAB_0777                             /* "He3.ob" */
#define MOG_s_TroggAxe1Cel MOG_LAB_0778                      /* "TroggAxe1.cel" */
#define MOG_s_TroggAxe2Cel MOG_LAB_0779                      /* "TroggAxe2.cel" */
#define MOG_s_TroggSpear1Cel MOG_LAB_077A                    /* "TroggSpear1.cel" */
#define MOG_s_TroggSpear2Cel MOG_LAB_077B                    /* "TroggSpear2.cel" */
#define MOG_s_Ratmen1Cel MOG_LAB_077C                        /* "Ratmen1.cel" */
#define MOG_s_Ratmen2Cel MOG_LAB_077D                        /* "Ratmen2.cel" */
#define MOG_s_Be1C MOG_LAB_077E                              /* "be1.c" */
#define MOG_s_Be2C MOG_LAB_077F                              /* "be2.c" */
#define MOG_s_Dragon1Cel MOG_LAB_0780                        /* "Dragon1.cel" */
#define MOG_s_Dragon2Cel MOG_LAB_0781                        /* "Dragon2.cel" */
#define MOG_s_Mudmen1Cel MOG_LAB_0782                        /* "Mudmen1.cel" */
#define MOG_s_Mudmen2Cel MOG_LAB_0783                        /* "Mudmen2.cel" */
#define MOG_s_KiCel MOG_LAB_0784                             /* "ki.cel" */
#define MOG_s_Balok1Cel MOG_LAB_0785                         /* "Balok1.cel" */
#define MOG_s_Balok3Cel MOG_LAB_0786                         /* "Balok3.cel" */
#define MOG_s_Balok2Cel MOG_LAB_0787                         /* "Balok2.cel" */
#define MOG_s_Wi1C MOG_LAB_0788                              /* "wi1.c" */
#define MOG_s_Wi1P MOG_LAB_0789                              /* "wi1.p" */
#define MOG_s_Wi2P MOG_LAB_078A                              /* "wi2.p" */
#define MOG_s_BoldF MOG_LAB_078B                             /* "bold.f" */
#define MOG_s_SmallFont MOG_LAB_078C                         /* "Small.font" */
#define MOG_s_SelCel MOG_LAB_07AD                            /* "Sel.cel" */
#define MOG_s_MessagePiv MOG_LAB_07AE                        /* "message.piv" */
#define MOG_s_ChPiv MOG_LAB_07AF                             /* "ch.piv" */
#define MOG_s_Demon1Cel MOG_LAB_07B0                         /* "Demon1.cel" */
#define MOG_s_Demon2Cel MOG_LAB_07B1                         /* "Demon2.cel" */
#define MOG_s_Demon3Cel MOG_LAB_07B2                         /* "Demon3.cel" */
#define MOG_s_Demon4Cel MOG_LAB_07B3                         /* "Demon4.cel" */
#define MOG_s_Troll1Cel MOG_LAB_07B4                         /* "Troll1.cel" */
#define MOG_s_Troll2Cel MOG_LAB_07B5                         /* "Troll2.cel" */
#define MOG_t_TerrainsWater MOG_LAB_07B6                     /* 8 terrains de l'eau (Wa*.t) */
#define MOG_t_TerrainsGlade MOG_LAB_07B7                     /* 8 terrains de la clairière (GL*.t) */
#define MOG_t_TerrainsSwamp MOG_LAB_07B8                     /* 8 terrains du marais (Sw*.t) */
#define MOG_t_TerrainsForest MOG_LAB_07B9                    /* 8 terrains de la forêt (FO*.t) */
#define MOG_t_EntriesA MOG_LAB_07BA                          /* positions d'entrée des adversaires (Troggs) */
#define MOG_t_EntriesB MOG_LAB_07BB                          /* positions d'entrée (LAB_0189) */
#define MOG_t_EntriesC MOG_LAB_07BC                          /* positions d'entrée (hommes-rats, Démon) */
#define MOG_t_LairCreature MOG_LAB_07BD                      /* créature de chacun des 24 repaires */
#define MOG_t_LairPos MOG_LAB_07BE                           /* position (X, Y) des repaires */
#define MOG_t_LairPlace MOG_LAB_07BF                         /* type de lieu des repaires (décor du combat) */
#define MOG_t_LairTerrain MOG_LAB_07C0                       /* fichier .t des repaires */
#define MOG_x_KnightStand MOG_LAB_07DB                       /* chevalier au repos (+22 ; aussi attaque 0 et réaction 0) */
#define MOG_x_KnightRecoil MOG_LAB_07DC                      /* chevalier après avoir touché (+26 ; aussi réactions 4 et 7) */
#define MOG_x_KnightWalkH0 MOG_LAB_07DD                      /* marche horizontale, phase 0 (t_KnightWalk+0) */
#define MOG_x_KnightWalkH1 MOG_LAB_07DE                      /* marche horizontale, phase 1 (t_KnightWalk+4) */
#define MOG_x_KnightWalkH2 MOG_LAB_07DF                      /* marche horizontale, phase 2 (t_KnightWalk+8) */
#define MOG_x_KnightWalkH3 MOG_LAB_07E0                      /* marche horizontale, phase 3 (t_KnightWalk+12) */
#define MOG_x_KnightWalkUp0 MOG_LAB_07E1                     /* marche vers le haut, phase 0 (t_KnightWalk+32) */
#define MOG_x_KnightWalkUp1 MOG_LAB_07E2                     /* marche vers le haut, phase 1 (t_KnightWalk+36) */
#define MOG_x_KnightWalkUp2 MOG_LAB_07E3                     /* marche vers le haut, phase 2 (t_KnightWalk+40) */
#define MOG_x_KnightWalkUp3 MOG_LAB_07E4                     /* marche vers le haut, phase 3 (t_KnightWalk+44) */
#define MOG_x_KnightWalkDown0 MOG_LAB_07E5                   /* marche vers le bas, phase 0 (t_KnightWalk+64) */
#define MOG_x_KnightWalkDown1 MOG_LAB_07E6                   /* marche vers le bas, phase 1 (t_KnightWalk+68) */
#define MOG_x_KnightWalkDown2 MOG_LAB_07E7                   /* marche vers le bas, phase 2 (t_KnightWalk+72) */
#define MOG_x_KnightWalkDown3 MOG_LAB_07E8                   /* marche vers le bas, phase 3 (t_KnightWalk+76) */
#define MOG_x_KnightAtk5 MOG_LAB_07E9                        /* attaque 5 (t_KnightAttacks+20) */
#define MOG_x_KnightAtk3 MOG_LAB_07EA                        /* attaque 3 (t_KnightAttacks+12) */
#define MOG_x_ThrownObject MOG_LAB_07EB                      /* objet lancé (LAB_02CA, contrôleur 52) */
#define MOG_x_ProjectileFly MOG_LAB_07EC                     /* objet projeté en vol (LAB_02CB) */
#define MOG_x_KnightAtk2 MOG_LAB_07ED                        /* attaque 2 (t_KnightAttacks+8) */
#define MOG_x_KnightAtk8 MOG_LAB_07EE                        /* attaque 8 (t_KnightAttacks+32) */
#define MOG_x_KnightAtk1 MOG_LAB_07EF                        /* attaque 1 (t_KnightAttacks+4) */
#define MOG_x_KnightRatReact7 MOG_LAB_07F0                   /* réaction 7 du chevalier face aux hommes-rats */
#define MOG_x_KnightAtk6 MOG_LAB_07F1                        /* attaque 6 (t_KnightAttacks+24) */
#define MOG_x_KnightRatAtk4 MOG_LAB_07F2                     /* attaque 4 du chevalier face aux hommes-rats */
#define MOG_x_KnightAtk7 MOG_LAB_07F3                        /* attaque 7 (t_KnightAttacks+28) */
#define MOG_x_KnightAtk4 MOG_LAB_07F4                        /* attaque 4 (t_KnightAttacks+16) */
#define MOG_x_KnightReact8 MOG_LAB_07F5                      /* réaction à l'attaque 8 (t_KnightScripts+32) */
#define MOG_x_KnightReact1 MOG_LAB_07F6                      /* réaction à l'attaque 1 (t_KnightScripts+4) */
#define MOG_x_KnightThrown MOG_LAB_07F7                      /* chevalier projeté (hommes-rats, Démon) */
#define MOG_x_KnightDie MOG_LAB_07F8                         /* chevalier tué */
#define MOG_x_KnightDieHead MOG_LAB_07F9                     /* chevalier tué (coup 8 ; décapité) */
#define MOG_x_KnightReleased MOG_LAB_07FA                    /* chevalier relâché par un homme-rat */
#define MOG_x_KnightDropped MOG_LAB_07FB                     /* chevalier lâché (Démon, Dragon) */
#define MOG_x_KnightEnter MOG_LAB_07FC                       /* entrée du chevalier dans le combat */
#define MOG_x_KnightCrushed MOG_LAB_07FD                     /* chevalier écrasé (créature qui bondit) */
#define MOG_x_KnightDragonReact MOG_LAB_07FE                 /* réaction 2 / 8 du chevalier face au Dragon */
#define MOG_x_TroggAStand MOG_LAB_0800                       /* Trogg (LAB_0169) au repos (+22, +26) */
#define MOG_x_TroggAAtk2 MOG_LAB_0801                        /* attaque 2 (t_TroggAAttacks+8) */
#define MOG_x_TroggAAtk6 MOG_LAB_0802                        /* attaque 6 (t_TroggAAttacks+24) */
#define MOG_x_TroggAWalkH0 MOG_LAB_0803                      /* marche horizontale, phase 0 (t_TroggAWalk+0) */
#define MOG_x_TroggAWalkH1 MOG_LAB_0804                      /* marche horizontale, phase 1 (t_TroggAWalk+4) */
#define MOG_x_TroggAWalkH2 MOG_LAB_0805                      /* marche horizontale, phase 2 (t_TroggAWalk+8) */
#define MOG_x_TroggAWalkUp0 MOG_LAB_0806                     /* marche vers le haut, phase 0 (t_TroggAWalk+32) */
#define MOG_x_TroggAWalkUp1 MOG_LAB_0807                     /* marche vers le haut, phase 1 (t_TroggAWalk+36) */
#define MOG_x_TroggAWalkUp2 MOG_LAB_0808                     /* marche vers le haut, phase 2 (t_TroggAWalk+40) */
#define MOG_x_TroggAWalkUp3 MOG_LAB_0809                     /* marche vers le haut, phase 3 (t_TroggAWalk+44) */
#define MOG_x_TroggAWalkDown0 MOG_LAB_080A                   /* marche vers le bas, phase 0 (t_TroggAWalk+64) */
#define MOG_x_TroggAWalkDown1 MOG_LAB_080B                   /* marche vers le bas, phase 1 (t_TroggAWalk+68) */
#define MOG_x_TroggAWalkDown2 MOG_LAB_080C                   /* marche vers le bas, phase 2 (t_TroggAWalk+72) */
#define MOG_x_TroggAWalkDown3 MOG_LAB_080D                   /* marche vers le bas, phase 3 (t_TroggAWalk+76) */
#define MOG_x_TroggAReact8 MOG_LAB_080E                      /* réaction à l'attaque 8 (t_TroggAScripts+32) */
#define MOG_x_TroggAReact2 MOG_LAB_080F                      /* réaction à l'attaque 2 (t_TroggAScripts+8) */
#define MOG_x_TroggAReact1 MOG_LAB_0810                      /* réaction à l'attaque 1 (t_TroggAScripts+4) */
#define MOG_x_TroggSpearReact8 MOG_LAB_0814                  /* réaction à l'attaque 8 (t_TroggSpearScripts+32) */
#define MOG_x_TroggSpearReact2 MOG_LAB_0815                  /* réaction à l'attaque 2 (t_TroggSpearScripts+8) */
#define MOG_x_TroggSpearReact1 MOG_LAB_0816                  /* réaction à l'attaque 1 (t_TroggSpearScripts+4) */
#define MOG_x_TroggSpearStand MOG_LAB_081A                   /* Trogg à lance au repos (+22, +26) */
#define MOG_x_TroggLunge MOG_LAB_081B                        /* Trogg : attaque 4 de près (LAB_0236) */
#define MOG_x_TroggKill MOG_LAB_081C                         /* Trogg : achève sa cible */
#define MOG_x_TroggSpearWalkH0 MOG_LAB_081D                  /* marche horizontale, phase 0 (t_TroggSpearWalk+0) */
#define MOG_x_TroggSpearWalkH1 MOG_LAB_081E                  /* marche horizontale, phase 1 (t_TroggSpearWalk+4) */
#define MOG_x_TroggSpearWalkH2 MOG_LAB_081F                  /* marche horizontale, phase 2 (t_TroggSpearWalk+8) */
#define MOG_x_TroggSpearWalkUp0 MOG_LAB_0820                 /* marche vers le haut, phase 0 (t_TroggSpearWalk+32) */
#define MOG_x_TroggSpearWalkUp1 MOG_LAB_0821                 /* marche vers le haut, phase 1 (t_TroggSpearWalk+36) */
#define MOG_x_TroggSpearWalkUp2 MOG_LAB_0822                 /* marche vers le haut, phase 2 (t_TroggSpearWalk+40) */
#define MOG_x_TroggSpearWalkUp3 MOG_LAB_0823                 /* marche vers le haut, phase 3 (t_TroggSpearWalk+44) */
#define MOG_x_TroggSpearWalkDown0 MOG_LAB_0824               /* marche vers le bas, phase 0 (t_TroggSpearWalk+64) */
#define MOG_x_TroggSpearWalkDown1 MOG_LAB_0825               /* marche vers le bas, phase 1 (t_TroggSpearWalk+68) */
#define MOG_x_TroggSpearWalkDown2 MOG_LAB_0826               /* marche vers le bas, phase 2 (t_TroggSpearWalk+72) */
#define MOG_x_TroggSpearWalkDown3 MOG_LAB_0827               /* marche vers le bas, phase 3 (t_TroggSpearWalk+76) */
#define MOG_x_TroggBStand MOG_LAB_0828                       /* Trogg à hache (LAB_0170) au repos (+22, +26) */
#define MOG_x_TroggBAtk2 MOG_LAB_0829                        /* attaque 2 (t_TroggBAttacks+8) */
#define MOG_x_TroggBAtk6 MOG_LAB_082A                        /* attaque 6 (t_TroggBAttacks+24) */
#define MOG_x_TroggBWalkH0 MOG_LAB_082B                      /* marche horizontale, phase 0 (t_TroggBWalk+0) */
#define MOG_x_TroggBWalkH1 MOG_LAB_082C                      /* marche horizontale, phase 1 (t_TroggBWalk+4) */
#define MOG_x_TroggBWalkH2 MOG_LAB_082D                      /* marche horizontale, phase 2 (t_TroggBWalk+8) */
#define MOG_x_TroggBWalkUp0 MOG_LAB_082E                     /* marche vers le haut, phase 0 (t_TroggBWalk+32) */
#define MOG_x_TroggBWalkUp1 MOG_LAB_082F                     /* marche vers le haut, phase 1 (t_TroggBWalk+36) */
#define MOG_x_TroggBWalkUp2 MOG_LAB_0830                     /* marche vers le haut, phase 2 (t_TroggBWalk+40) */
#define MOG_x_TroggBWalkUp3 MOG_LAB_0831                     /* marche vers le haut, phase 3 (t_TroggBWalk+44) */
#define MOG_x_TroggBWalkDown0 MOG_LAB_0832                   /* marche vers le bas, phase 0 (t_TroggBWalk+64) */
#define MOG_x_TroggBWalkDown1 MOG_LAB_0833                   /* marche vers le bas, phase 1 (t_TroggBWalk+68) */
#define MOG_x_TroggBWalkDown2 MOG_LAB_0834                   /* marche vers le bas, phase 2 (t_TroggBWalk+72) */
#define MOG_x_TroggBWalkDown3 MOG_LAB_0835                   /* marche vers le bas, phase 3 (t_TroggBWalk+76) */
#define MOG_x_TroggBReact8 MOG_LAB_0836                      /* réaction à l'attaque 8 (t_TroggBScripts+32) */
#define MOG_x_TroggBReact2 MOG_LAB_0837                      /* réaction à l'attaque 2 (t_TroggBScripts+8) */
#define MOG_x_TroggBReact1 MOG_LAB_0838                      /* réaction à l'attaque 1 (t_TroggBScripts+4) */
#define MOG_x_PassingKnightStand MOG_LAB_0840                /* chevalier de passage au repos (+22 ; aussi réaction 4) */
#define MOG_x_PassingKnightRecoil MOG_LAB_0844               /* chevalier de passage après avoir touché (+26) */
#define MOG_x_PassingKnightReact8 MOG_LAB_0845               /* réaction à l'attaque 8 (t_PassingKnightScripts+32) */
#define MOG_x_PassingKnightReact2 MOG_LAB_0847               /* réaction à l'attaque 2 (t_PassingKnightScripts+8) */
#define MOG_x_PassingKnightKillBack MOG_LAB_0849             /* chevalier de passage : achève (de dos) */
#define MOG_x_PassingKnightHitFront MOG_LAB_084A             /* chevalier de passage touché (même sens) */
#define MOG_x_PassingKnightHitBack MOG_LAB_084B              /* chevalier de passage touché (sens opposé) */
#define MOG_x_PassingKnightKill MOG_LAB_084E                 /* chevalier de passage : achève (de face) */
#define MOG_x_RatmenStand MOG_LAB_084F                       /* homme-rat au repos (+22, +26) */
#define MOG_x_RatJump MOG_LAB_0850                           /* homme-rat : saut */
#define MOG_x_RatFall MOG_LAB_0851                           /* homme-rat : chute */
#define MOG_x_RatmenWalkUp0 MOG_LAB_0852                     /* marche vers le haut, phase 0 (t_RatmenWalk+32) */
#define MOG_x_RatmenWalkUp1 MOG_LAB_0853                     /* marche vers le haut, phase 1 (t_RatmenWalk+36) */
#define MOG_x_RatmenWalkUp2 MOG_LAB_0854                     /* marche vers le haut, phase 2 (t_RatmenWalk+40) */
#define MOG_x_RatmenWalkUp3 MOG_LAB_0855                     /* marche vers le haut, phase 3 (t_RatmenWalk+44) */
#define MOG_x_RatAtk4 MOG_LAB_0857                           /* homme-rat : attaque 4 */
#define MOG_x_RatHit4 MOG_LAB_0858                           /* homme-rat touché par l'attaque 4 */
#define MOG_x_RatAtk8 MOG_LAB_0859                           /* homme-rat : attaque 8 */
#define MOG_x_RatHit8 MOG_LAB_085A                           /* homme-rat touché par l'attaque 8 */
#define MOG_x_RatOnKnight MOG_LAB_085B                       /* homme-rat accroché au chevalier */
#define MOG_x_RatLetGo MOG_LAB_085C                          /* homme-rat lâche prise */
#define MOG_x_RatShaken MOG_LAB_085D                         /* homme-rat secoué (le chevalier se dégage) */
#define MOG_x_RatHitByKnight8 MOG_LAB_085E                   /* homme-rat touché (coup 8) */
#define MOG_x_RatHitByKnight4 MOG_LAB_085F                   /* homme-rat touché (coup 4) */
#define MOG_x_RatmenReact8 MOG_LAB_0860                      /* réaction à l'attaque 8 (t_RatmenScripts+32) */
#define MOG_x_RatmenReact1 MOG_LAB_0862                      /* réaction à l'attaque 1 (t_RatmenScripts+4) */
#define MOG_x_RatWaitFar MOG_LAB_0863                        /* homme-rat attend (loin) */
#define MOG_x_RatWaitNear MOG_LAB_0864                       /* homme-rat attend (près) */
#define MOG_x_RatBite MOG_LAB_0865                           /* homme-rat mord */
#define MOG_x_RatBiting MOG_LAB_0866                         /* homme-rat mord encore */
#define MOG_x_RatBiteShaken MOG_LAB_0867                     /* homme-rat secoué pendant la morsure */
#define MOG_x_RatBiteEnd MOG_LAB_0868                        /* homme-rat : fin de morsure */
#define MOG_x_RatBiteKill MOG_LAB_0869                       /* homme-rat : morsure mortelle */
#define MOG_x_RatmenReact2 MOG_LAB_086A                      /* réaction à l'attaque 2 (t_RatmenScripts+8) */
#define MOG_x_RatHitInAir MOG_LAB_086B                       /* homme-rat touché en l'air */
#define MOG_x_RatmenWalkH0 MOG_LAB_086C                      /* marche horizontale, phase 0 (t_RatmenWalk+0) */
#define MOG_x_RatmenWalkH1 MOG_LAB_086D                      /* marche horizontale, phase 1 (t_RatmenWalk+4) */
#define MOG_x_RatmenWalkH2 MOG_LAB_086E                      /* marche horizontale, phase 2 (t_RatmenWalk+8) */
#define MOG_x_RatmenWalkH3 MOG_LAB_086F                      /* marche horizontale, phase 3 (t_RatmenWalk+12) */
#define MOG_x_DemonCompanionEnter MOG_LAB_0870               /* compagnon du Démon (entrée) */
#define MOG_x_DragonAtk8 MOG_LAB_0872                        /* Dragon : attaque 8 */
#define MOG_x_DragonWalkUp0 MOG_LAB_0873                     /* marche vers le haut, phase 0 (t_DragonWalk+32) */
#define MOG_x_DragonWalkUp1 MOG_LAB_0874                     /* marche vers le haut, phase 1 (t_DragonWalk+36) */
#define MOG_x_DragonWalkUp2 MOG_LAB_0875                     /* marche vers le haut, phase 2 (t_DragonWalk+40) */
#define MOG_x_DragonWalkUp3 MOG_LAB_0876                     /* marche vers le haut, phase 3 (t_DragonWalk+44) */
#define MOG_x_DragonWalkUp4 MOG_LAB_0877                     /* marche vers le haut, phase 4 (t_DragonWalk+48) */
#define MOG_x_DragonWalkDown0 MOG_LAB_0879                   /* marche vers le bas, phase 0 (t_DragonWalk+64) */
#define MOG_x_DragonWalkDown1 MOG_LAB_087A                   /* marche vers le bas, phase 1 (t_DragonWalk+68) */
#define MOG_x_DragonWalkDown2 MOG_LAB_087B                   /* marche vers le bas, phase 2 (t_DragonWalk+72) */
#define MOG_x_DragonWalkDown3 MOG_LAB_087C                   /* marche vers le bas, phase 3 (t_DragonWalk+76) */
#define MOG_x_DragonWalkDown4 MOG_LAB_087D                   /* marche vers le bas, phase 4 (t_DragonWalk+80) */
#define MOG_x_DragonFire MOG_LAB_087E                        /* Dragon : souffle de feu */
#define MOG_x_DragonReact2 MOG_LAB_087F                      /* réaction à l'attaque 2 (t_DragonScripts+8) */
#define MOG_x_DragonPartStand MOG_LAB_0880                   /* parties du Dragon au repos (+22, +26) */
#define MOG_x_DragonPartAtk MOG_LAB_0881                     /* partie du Dragon : attaque */
#define MOG_x_DragonStand MOG_LAB_0882                       /* Dragon au repos (+22, +26 ; LAB_0195) */
#define MOG_x_DragonAtk4 MOG_LAB_0883                        /* Dragon : attaque 4 (de près) */
#define MOG_x_DragonKill MOG_LAB_0884                        /* Dragon : achève sa cible */
#define MOG_x_DragonFlame MOG_LAB_0886                       /* flamme devant le Dragon (contrôleur 40) */
#define MOG_x_DragonPartDie MOG_LAB_0887                     /* partie du Dragon : mort du Dragon */
#define MOG_x_BalokStand MOG_LAB_0888                        /* Balok au repos (+22) */
#define MOG_x_LeaperLand MOG_LAB_088A                        /* créature qui bondit : atterrit */
#define MOG_x_LeaperFly MOG_LAB_088B                         /* créature qui bondit : en vol */
#define MOG_x_LeaperAtk8 MOG_LAB_088D                        /* créature qui bondit : attaque 8 */
#define MOG_x_LeaperRecoil8 MOG_LAB_088E                     /* créature qui bondit : après l'attaque 8 */
#define MOG_x_BalokRecoil MOG_LAB_088F                       /* Balok après avoir touché (+26) */
#define MOG_x_LeaperGrab MOG_LAB_0890                        /* créature qui bondit : saisie (attaque $20) */
#define MOG_x_LeaperHit MOG_LAB_0891                         /* créature qui bondit touchée */
#define MOG_x_LeaperHold MOG_LAB_0893                        /* créature qui bondit : tient sa proie */
#define MOG_x_LeaperRelease MOG_LAB_0894                     /* créature qui bondit : lâche sa proie */
#define MOG_x_LeaperEatA MOG_LAB_0895                        /* créature qui bondit : dévore (A) */
#define MOG_x_LeaperEatB MOG_LAB_0896                        /* créature qui bondit : dévore (B) */
#define MOG_x_MudmanMove MOG_LAB_0897                        /* Mudman : avance */
#define MOG_x_MudmanDive MOG_LAB_0898                        /* Mudman : replonge */
#define MOG_x_MudmanSurface MOG_LAB_0899                     /* Mudman : émerge */
#define MOG_x_MudmenStand MOG_LAB_089A                       /* Mudmen au repos (+22, +26) */
#define MOG_x_MudmenWalkH1 MOG_LAB_089B                      /* marche horizontale, phase 1 (t_MudmenWalk+4) */
#define MOG_x_MudmenWalkH0 MOG_LAB_089C                      /* marche horizontale, phase 0 (t_MudmenWalk+0) */
#define MOG_x_MudmenWalkH3 MOG_LAB_089D                      /* marche horizontale, phase 3 (t_MudmenWalk+12) */
#define MOG_x_MudmanAtk MOG_LAB_089E                         /* Mudman : attaque de près */
#define MOG_x_MudmanGrip MOG_LAB_089F                        /* Mudman : agrippe */
#define MOG_x_MudmanShaken MOG_LAB_08A0                      /* Mudman : le chevalier se dégage */
#define MOG_x_MudmanSwallow MOG_LAB_08A1                     /* Mudman : avale */
#define MOG_x_MudmanDrag MOG_LAB_08A2                        /* Mudman : entraîne le chevalier */
#define MOG_x_MudmanHit MOG_LAB_08A3                         /* Mudman touché (aussi toutes ses réactions) */
#define MOG_x_TrollStand MOG_LAB_08A5                        /* Troll au repos (+22, +26) */
#define MOG_x_TrollWalkH0 MOG_LAB_08A6                       /* marche horizontale, phase 0 (t_TrollWalk+0) */
#define MOG_x_TrollWalkH1 MOG_LAB_08A7                       /* marche horizontale, phase 1 (t_TrollWalk+4) */
#define MOG_x_TrollWalkH2 MOG_LAB_08A8                       /* marche horizontale, phase 2 (t_TrollWalk+8) */
#define MOG_x_TrollWalkH3 MOG_LAB_08A9                       /* marche horizontale, phase 3 (t_TrollWalk+12) */
#define MOG_x_TrollAtk8 MOG_LAB_08AA                         /* Troll : attaque 8 */
#define MOG_x_TrollAtk20 MOG_LAB_08AB                        /* Troll : attaque $20 */
#define MOG_x_TrollHit MOG_LAB_08AC                          /* Troll touché */
#define MOG_x_DemonStand MOG_LAB_08AE                        /* Démon au repos (+22) */
#define MOG_x_DemonCompanion MOG_LAB_08AF                    /* compagnon du Démon (LAB_0EEB) */
#define MOG_x_DemonRecoil MOG_LAB_08B0                       /* Démon après avoir touché (+26) */
#define MOG_x_DemonAtk20 MOG_LAB_08B4                        /* Démon : attaque $20 */
#define MOG_x_DemonAtk8 MOG_LAB_08B5                         /* Démon : attaque 8 */
#define MOG_x_DemonAtk4 MOG_LAB_08B6                         /* Démon : attaque 4 */
#define MOG_x_DemonReachA MOG_LAB_08B7                       /* Démon : tend les bras (état 5) */
#define MOG_x_DemonHold MOG_LAB_08B8                         /* Démon : tient le chevalier */
#define MOG_x_DemonReachB MOG_LAB_08B9                       /* Démon : tend les bras (état 4) */
#define MOG_x_DemonThrow MOG_LAB_08BA                        /* Démon : projette le chevalier */
#define MOG_x_DemonGrabB MOG_LAB_08BB                        /* Démon : saisit le chevalier à portée (état 4) */
#define MOG_x_DemonGrabA MOG_LAB_08BC                        /* Démon : saisit le chevalier à portée (état 5) */
#define MOG_x_DemonStruck MOG_LAB_08BD                       /* Démon touché */
#define MOG_x_Splash MOG_LAB_08BF                            /* éclaboussure au point d'impact (contrôleur 40) */
#define MOG_s_SirBanner MOG_LAB_08C0                         /* "SIR BANNER" */
#define MOG_s_SirDwain MOG_LAB_08C1                          /* "SIR DWAIN" */
#define MOG_s_SirBalain MOG_LAB_08C2                         /* "SIR BALAIN" */
#define MOG_s_SirEdward2 MOG_LAB_08C3                        /* "SIR EDWARD" */
#define MOG_v_PlaceType MOG_LAB_08C4                         /* type de lieu du combat (0 clairière, 4 forêt, 8 marais, 12 eau) */
#define MOG_t_LairTreasure MOG_LAB_08C5                      /* trésor des repaires : seuils de tirage et genres */
#define MOG_v_Lair MOG_LAB_08C6                              /* repaire (créature de la carte) courant */
#define MOG_t_BackStepsA MOG_LAB_08CC                        /* pas du recul (A) */
#define MOG_t_BackStepsB MOG_LAB_08CD                        /* pas du recul (B, Démon) */
#define MOG_v_BackSteps MOG_LAB_08CE                         /* table de pas du recul en cours (LAB_0211) */
#define MOG_v_BackStepIndex MOG_LAB_08CF                     /* pas suivant du recul (-1 : aucun) */
#define MOG_v_BackDir MOG_LAB_08D0                           /* sens du recul */
#define MOG_t_PalGlade MOG_LAB_08D1                          /* palette du combat en clairière */
#define MOG_t_PalForest MOG_LAB_08D2                         /* palette du combat en forêt */
#define MOG_t_PalWater MOG_LAB_08D3                          /* palette du combat sur l'eau */
#define MOG_t_PalSwamp MOG_LAB_08D4                          /* palette du combat au marais */
#define MOG_t_PalCombatBase MOG_LAB_08D5                     /* couleurs communes des combats (46 octets) */
#define MOG_t_FightPalette MOG_LAB_08D6                      /* palette du combat seul (mog_fight) */
#define MOG_t_PalBlack MOG_LAB_08D8                          /* palette noire (fondus) */
#define MOG_t_PalCombat MOG_LAB_08D9                         /* palette du combat construite */
#define MOG_v_HoverX MOG_LAB_08DA                            /* X du point testé sur les zones */
#define MOG_v_HoverY MOG_LAB_08DB                            /* Y du point testé */
#define MOG_v_GlyphWidth MOG_LAB_08DC                        /* largeur de la lettre courante */
#define MOG_v_GlyphHeight MOG_LAB_08DD                       /* hauteur de la lettre courante */
#define MOG_v_TextX MOG_LAB_08DE                             /* X de la lettre suivante */
#define MOG_v_TextY MOG_LAB_08DF                             /* Y de la ligne */
#define MOG_v_TextLineX MOG_LAB_08E0                         /* X de début de ligne */
#define MOG_v_TextTopY MOG_LAB_08E1                          /* Y de la première ligne */
#define MOG_v_TextChar MOG_LAB_08E2                          /* caractère courant (pointeur dans la chaîne) */
#define MOG_v_TextRecord MOG_LAB_08E3                        /* enregistrement de texte courant */
#define MOG_t_TextRecord MOG_LAB_08E4                        /* enregistrement de texte temporaire (LAB_0431) */
#define MOG_v_TextRight MOG_LAB_08E5                         /* bord droit pour centrer / aligner (320) */
#define MOG_v_TextRightMargin MOG_LAB_08E6                   /* marge retirée du bord droit */
#define MOG_t_FontGlyphFrame MOG_LAB_08E7                    /* frame de la police pour chaque caractère (depuis ' ') */
#define MOG_s_0123456789 MOG_LAB_08E8                        /* "0123456789 " */
#define MOG_b_TextLine MOG_LAB_08E9                          /* ligne de texte composée */
#define MOG_s_May MOG_LAB_08EA                               /* " may ... " */
#define MOG_s_EnterLair MOG_LAB_08F1                         /* "Enter Lair" */
#define MOG_s_BattleWith MOG_LAB_08F2                        /* "Battle with " */
#define MOG_t_PlaceNames MOG_LAB_08F4                        /* noms des lieux (icônes $15 et plus) */
#define MOG_v_PlacesMenuX MOG_LAB_08F5                       /* X du menu des lieux */
#define MOG_v_PlacesMenuY MOG_LAB_08F6                       /* Y de la ligne suivante du menu */
#define MOG_v_AiBuyPrice MOG_LAB_08F7                        /* achat envisagé par le chevalier noir : prix */
#define MOG_v_AiBuyItem MOG_LAB_08F8                         /* objet (genre) */
#define MOG_v_AiBuyExtra MOG_LAB_08F9                        /* épée gardée ($18) ou second objet */
#define MOG_t_CellSlow MOG_LAB_08FA                          /* ralentissement de chaque case de la carte (40 × 25) */
#define MOG_t_CellPlace MOG_LAB_08FB                         /* type de lieu de chaque case */
#define MOG_t_DragonMapScripts MOG_LAB_08FC                  /* scripts du dragon sur la carte */
#define MOG_x_DragonMapDive MOG_LAB_0900                     /* dragon qui descend sur la carte */
#define MOG_t_GoldMessages MOG_LAB_0905                      /* messages « or trouvé » */
#define MOG_v_GoldMessageIndex MOG_LAB_0906                  /* message d'or suivant (0-2) */
#define MOG_t_ItemMessages MOG_LAB_0907                      /* messages « objet trouvé » */
#define MOG_v_ItemMessageIndex MOG_LAB_0908                  /* message d'objet suivant (0-3) */
#define MOG_v_EventMessage MOG_LAB_0909                      /* message de l'événement */
#define MOG_t_StatGains MOG_LAB_090A                         /* caractéristiques (décalage objet, message) */
#define MOG_v_EventKind MOG_LAB_090B                         /* événement : 1 objet, 2 or, 3 caractéristique, 4 envoûtement */
#define MOG_v_EventStat MOG_LAB_090C                         /* caractéristique gagnée */
#define MOG_v_LastItem MOG_LAB_090D                          /* dernier objet trouvé (jamais deux fois de suite) */
#define MOG_t_FindItems MOG_LAB_090E                         /* objets à trouver (seuil, genre) */
#define MOG_t_ItemNames MOG_LAB_090F                         /* noms des objets (genre, chaîne) */
#define MOG_v_PurseFrame MOG_LAB_091A                        /* image des bourses */
#define MOG_s_HeaPiv MOG_LAB_091B                            /* "HEA.piv" */
#define MOG_s_MysPiv MOG_LAB_091C                            /* "MYS.piv" */
#define MOG_s_MysCel MOG_LAB_091D                            /* "mys.cel" */
#define MOG_s_AsYouRingTheBellAtThe MOG_LAB_091E             /* "As you ring the bell at the" */
#define MOG_t_BewitchedMessage MOG_LAB_092D                  /* message d'envoûtement */
#define MOG_t_WizardItemText MOG_LAB_092E                    /* texte du magicien : objet */
#define MOG_b_FoundName MOG_LAB_092F                         /* "                              " */
#define MOG_t_WizardGoldText MOG_LAB_0930                    /* texte du magicien : or */
#define MOG_b_GoldText MOG_LAB_0931                          /* "              " */
#define MOG_t_WizardStatText MOG_LAB_0932                    /* texte du magicien : caractéristique */
#define MOG_t_WizardNoneText MOG_LAB_0933                    /* texte du magicien : rien */
#define MOG_t_WizardCurseText MOG_LAB_0934                   /* texte du magicien : envoûtement */
#define MOG_t_HealerText MOG_LAB_0935                        /* texte d'accueil du guérisseur */
#define MOG_t_ShopCancelled MOG_LAB_0938                     /* texte : rien acheté (annulé) */
#define MOG_t_ShopNoGold MOG_LAB_0939                        /* texte : aucune pièce proposée */
#define MOG_t_HealerTooLittle MOG_LAB_093A                   /* texte : trop peu d'or */
#define MOG_t_HealerHealed MOG_LAB_093C                      /* texte : soigné */
#define MOG_t_HealerCured MOG_LAB_093D                       /* texte : soigné et guéri du poison */
#define MOG_t_MasterText MOG_LAB_0947                        /* texte d'accueil du maître d'armes */
#define MOG_t_MasterMaxed MOG_LAB_0956                       /* texte : caractéristiques au maximum */
#define MOG_t_MasterLost MOG_LAB_0959                        /* texte : caractéristique perdue */
#define MOG_t_MasterGainTexts MOG_LAB_095B                   /* textes du gain de caractéristique */
#define MOG_t_MasterLossTexts MOG_LAB_095C                   /* textes de la perte */
#define MOG_s_YourGold MOG_LAB_0971                          /* "Your Gold" */
#define MOG_s_Donation MOG_LAB_0972                          /* "Donation" */
#define MOG_v_RandomSeed MOG_LAB_0973                        /* graine du tirage (LAB_0004 : une des quatre de LAB_0974) */
#define MOG_t_RandomSeeds MOG_LAB_0974                       /* graines de départ */
#define MOG_b_NumberText MOG_LAB_0975                        /* nombre écrit (bourses) */
#define MOG_v_GoldOffered MOG_LAB_0976                       /* or proposé */
#define MOG_v_GoldLeft MOG_LAB_0977                          /* or du chevalier pendant l'offre */
#define MOG_x_WizardA MOG_LAB_0978                           /* magicien (scène de l'événement, A) */
#define MOG_x_WizardB MOG_LAB_0979                           /* magicien (scène de l'événement, B) */
#define MOG_x_WizardStand MOG_LAB_097A                       /* magicien au repos */
#define MOG_t_KnightAiSkill MOG_LAB_097B                     /* seuil de réussite du chevalier ordinateur par manche */
#define MOG_v_PointerHidden MOG_LAB_097C                     /* pointeur caché */
#define MOG_v_PointerSprite MOG_LAB_097D                     /* données du sprite du pointeur */
#define MOG_v_PointerSprite2 MOG_LAB_097E                    /* données du sprite jumeau */
#define MOG_v_PointerX MOG_LAB_097F                          /* X du pointeur */
#define MOG_v_PointerY MOG_LAB_0980                          /* Y du pointeur */
#define MOG_v_PointerOn MOG_LAB_0981                         /* pointeur mené par la VBL */
#define MOG_v_PointerHook MOG_LAB_0982                       /* serveur d'interruption du pointeur */
#define MOG_s_PoCel MOG_LAB_0983                             /* "po.cel" */
#define MOG_v_ScreenChanged MOG_LAB_0984                     /* inventaire changé (écran à refaire) */
#define MOG_v_PanelX MOG_LAB_0985                            /* X du panneau (0 gauche, 160 droite) */
#define MOG_v_IconCel MOG_LAB_0986                           /* CEL des icônes des écrans */
#define MOG_b_ScreenNumber MOG_LAB_0987                      /* nombre écrit (écrans) */
#define MOG_v_Screen0988 MOG_LAB_0988                        /* remis à zéro au panneau du chevalier */
#define MOG_v_Screen0989 MOG_LAB_0989                        /* remis à zéro au panneau du chevalier */
#define MOG_t_PanelFrames MOG_LAB_098A                       /* cadres des panneaux (frame, X ; -1 fin) */
#define MOG_s_DrinkHealingPotion MOG_LAB_098B                /* "Drink Healing potion" */
#define MOG_s_UseGemOfSeeing MOG_LAB_098C                    /* "Use Gem of Seeing" */
#define MOG_s_RingOfProtection MOG_LAB_098D                  /* "Ring of Protection" */
#define MOG_s_TalismanOfTheWyrm MOG_LAB_098E                 /* "Talisman of the Wyrm" */
#define MOG_s_CastScrollOfHaste MOG_LAB_098F                 /* "Cast scroll of Haste" */
#define MOG_s_CastScrollOfAquisition MOG_LAB_0990            /* "Cast scroll of Aquisition" */
#define MOG_s_CastScrollOfTheHawk MOG_LAB_0991               /* "Cast scroll of the Hawk" */
#define MOG_s_CastScrollOfTheWyrm MOG_LAB_0992               /* "Cast scroll of the Wyrm" */
#define MOG_s_CastScrollOfProtection MOG_LAB_0993            /* "Cast scroll of Protection" */
#define MOG_s_NewMoonMoonstone MOG_LAB_0994                  /* "New moon Moonstone" */
#define MOG_s_FullMoonstone MOG_LAB_0995                     /* "Full Moonstone" */
#define MOG_s_HalfMoonstone MOG_LAB_0996                     /* "Half Moonstone" */
#define MOG_s_KeyToTheValley MOG_LAB_0997                    /* "Key to the Valley" */
#define MOG_s_PotionOfHealing MOG_LAB_0998                   /* "Potion of Healing" */
#define MOG_s_GemOfSeeing MOG_LAB_0999                       /* "Gem of Seeing" */
#define MOG_s_ScrollOfHaste MOG_LAB_099A                     /* "Scroll of Haste" */
#define MOG_s_ScrollOfAquisition MOG_LAB_099B                /* "Scroll of Aquisition" */
#define MOG_s_ScrollOfTheHawk MOG_LAB_099C                   /* "Scroll of the Hawk" */
#define MOG_s_ScrollOfTheWyrm MOG_LAB_099D                   /* "Scroll of the Wyrm" */
#define MOG_s_ScrollOfProtection MOG_LAB_099E                /* "Scroll of Protection" */
#define MOG_s_OfferPotionOfHealing MOG_LAB_099F              /* "Offer potion of Healing" */
#define MOG_s_OfferGemOfSeeing MOG_LAB_09A0                  /* "Offer Gem of Seeing" */
#define MOG_s_OfferRingOfProtection MOG_LAB_09A1             /* "Offer Ring of Protection" */
#define MOG_s_OfferTalismanOfTheWyrm MOG_LAB_09A2            /* "Offer Talisman of the Wyrm" */
#define MOG_s_OfferScrollOfHaste MOG_LAB_09A3                /* "Offer scroll of Haste" */
#define MOG_s_OfferScrollOfAquisition MOG_LAB_09A4           /* "Offer scroll of Aquisition" */
#define MOG_s_OfferScrollOfTheHawk MOG_LAB_09A5              /* "Offer scroll of the Hawk" */
#define MOG_s_OfferScrollOfTheWyrm MOG_LAB_09A6              /* "Offer scroll of the Wyrm" */
#define MOG_s_OfferScrollOfProtection MOG_LAB_09A7           /* "Offer scroll of Protection" */
#define MOG_s_IncreaseStrength MOG_LAB_09A8                  /* "Increase Strength" */
#define MOG_s_IncreaseEndurance MOG_LAB_09A9                 /* "Increase Endurance" */
#define MOG_s_IncreaseConstitution MOG_LAB_09AA              /* "Increase Constitution" */
#define MOG_s_Strength MOG_LAB_09AB                          /* "Strength" */
#define MOG_s_Endurance MOG_LAB_09AC                         /* "Endurance" */
#define MOG_s_Constitution MOG_LAB_09AD                      /* "Constitution" */
#define MOG_s_LifePoints MOG_LAB_09AE                        /* "Life points" */
#define MOG_s_Gold MOG_LAB_09AF                              /* "Gold" */
#define MOG_s_Dagger MOG_LAB_09B0                            /* "Dagger" */
#define MOG_s_PaddedArmour MOG_LAB_09B1                      /* "Padded armour" */
#define MOG_s_ChainMail MOG_LAB_09B2                         /* "Chain mail" */
#define MOG_s_PlateArmour MOG_LAB_09B3                       /* "Plate armour" */
#define MOG_s_BattleArmour MOG_LAB_09B4                      /* "Battle armour" */
#define MOG_s_LongSword MOG_LAB_09B5                         /* "Long sword" */
#define MOG_s_BroadSword MOG_LAB_09B6                        /* "Broad sword" */
#define MOG_s_ClaymoreSword MOG_LAB_09B7                     /* "Claymore sword" */
#define MOG_s_SwordOfSharpness MOG_LAB_09B8                  /* "Sword of Sharpness" */
#define MOG_s_TakePotionOfHealing MOG_LAB_09B9               /* "Take Potion of Healing" */
#define MOG_s_TakeGemOfSeeing MOG_LAB_09BA                   /* "Take Gem of Seeing" */
#define MOG_s_TakeRingOfProtection MOG_LAB_09BB              /* "Take Ring of Protection" */
#define MOG_s_TakeTalismanOfTheWyrm MOG_LAB_09BC             /* "Take Talisman of the Wyrm" */
#define MOG_s_TakeScrollOfHaste MOG_LAB_09BD                 /* "Take scroll of Haste" */
#define MOG_s_TakeScrollOfAquisition MOG_LAB_09BE            /* "Take scroll of Aquisition" */
#define MOG_s_TakeScrollOfTheHawk MOG_LAB_09BF               /* "Take scroll of the Hawk" */
#define MOG_s_TakeScrollOfTheWyrm MOG_LAB_09C0               /* "Take scroll of the Wyrm" */
#define MOG_s_TakeScrollOfProtection MOG_LAB_09C1            /* "Take scroll of Protection" */
#define MOG_s_TakeGold MOG_LAB_09C2                          /* "Take Gold" */
#define MOG_s_TakeDaggers MOG_LAB_09C3                       /* "Take Daggers" */
#define MOG_s_TakeClaymoreSword MOG_LAB_09C4                 /* "Take Claymore sword" */
#define MOG_s_TakeSwordOfSharpness MOG_LAB_09C5              /* "Take Sword of Sharpness" */
#define MOG_s_TakeBroadSword MOG_LAB_09C6                    /* "Take Broad sword" */
#define MOG_s_TakeBattleArmour MOG_LAB_09C7                  /* "Take Battle armour" */
#define MOG_s_TakePlateArmour MOG_LAB_09C8                   /* "Take Plate armour" */
#define MOG_s_TakeChainmail MOG_LAB_09C9                     /* "Take Chainmail" */
#define MOG_s_TakeKeyToTheValley MOG_LAB_09CA                /* "Take Key to the Valley" */
#define MOG_s_TakeMoonstone MOG_LAB_09CB                     /* "Take Moonstone" */
#define MOG_s_LifePointsLeft MOG_LAB_09CC                    /* "Life points left" */
#define MOG_s_BuyBroadSwordFor10Gp MOG_LAB_09CD              /* "Buy Broad Sword for 10 GP" */
#define MOG_s_BuyClaymoreSwordFor25Gp MOG_LAB_09CE           /* "Buy Claymore sword for 25 GP" */
#define MOG_s_BuyChainmailFor30Gp MOG_LAB_09CF               /* "Buy Chainmail for 30 GP" */
#define MOG_s_BuyPlateArmourFor50Gp MOG_LAB_09D0             /* "Buy Plate armour for 50 GP" */
#define MOG_s_BuyBattleArmourFor75Gp MOG_LAB_09D1            /* "Buy Battle armour for 75 GP" */
#define MOG_s_BuyADaggerFor2Gp MOG_LAB_09D2                  /* "Buy a dagger for 2 GP" */
#define MOG_s_BuySwordOfSharpnessFor52Gp MOG_LAB_09D3        /* "Buy Sword of Sharpness for 52 GP" */
#define MOG_s_BuyKeyFor12Gp MOG_LAB_09D4                     /* "Buy Key for 12 GP" */
#define MOG_s_BuyPotionOfHealingFor20Gp MOG_LAB_09D5         /* "Buy Potion of healing for 20 GP" */
#define MOG_s_BuyGemOfSeeingFor32Gp MOG_LAB_09D6             /* "Buy gem of seeing for 32 GP" */
#define MOG_s_BuyRingOfProtectionFor40Gp MOG_LAB_09D7        /* "Buy ring of protection for 40 GP" */
#define MOG_s_BuyTalismanFor52Gp MOG_LAB_09D8                /* "Buy Talisman for 52 GP" */
#define MOG_s_BuyScrollOfHasteFor36Gp MOG_LAB_09D9           /* "Buy scroll of Haste for 36 GP" */
#define MOG_s_BuyScrollOfAquisitionFor52Gp MOG_LAB_09DA      /* "Buy scroll of Aquisition for 52 GP" */
#define MOG_s_BuyScrollOfTheHawkFor52Gp MOG_LAB_09DB         /* "Buy scroll of the Hawk for 52 GP" */
#define MOG_s_BuyScrollOfTheWyrmFor40Gp MOG_LAB_09DC         /* "Buy scroll of the Wyrm for 40 GP" */
#define MOG_s_BuyScrollOfProtectionFor24Gp MOG_LAB_09DD      /* "Buy scroll of Protection for 24 GP" */
#define MOG_s_BuyMoonstoneFor20Gp MOG_LAB_09DE               /* "Buy Moonstone for 20 GP" */
#define MOG_s_SwordOfSharpness2 MOG_LAB_09DF                 /* "Sword of Sharpness" */
#define MOG_s_SellKeyFor6Gp MOG_LAB_09E0                     /* "Sell Key for 6 GP" */
#define MOG_s_SellPotionOfHealingFor10Gp MOG_LAB_09E1        /* "Sell Potion of healing for 10 GP" */
#define MOG_s_SellGemOfSeeingFor16Gp MOG_LAB_09E2            /* "Sell gem of seeing for 16 GP" */
#define MOG_s_SellRingFor20Gp MOG_LAB_09E3                   /* "Sell ring for 20 GP" */
#define MOG_s_SellTalismanFor26Gp MOG_LAB_09E4               /* "Sell Talisman for 26 GP" */
#define MOG_s_SellScrollOfHasteFor16Gp MOG_LAB_09E5          /* "Sell scroll of Haste for 16 GP" */
#define MOG_s_SellScrollOfAquisitionFor26G MOG_LAB_09E6      /* "Sell scroll of Aquisition for 26 GP" */
#define MOG_s_SellScrollOfTheHawkFor26Gp MOG_LAB_09E7        /* "Sell scroll of the Hawk for 26 GP" */
#define MOG_s_SellScrollOfTheWyrmFor20Gp MOG_LAB_09E8        /* "Sell scroll of the Wyrm for 20 GP" */
#define MOG_s_SellScrollOfProtectionFor12G MOG_LAB_09E9      /* "Sell scroll of Protection for 12 GP" */
#define MOG_s_SellMoonstoneFor10Gp MOG_LAB_09EA              /* "Sell Moonstone for 10 GP" */
#define MOG_s_Space MOG_LAB_09EB                             /* " " */
#define MOG_t_ExitZoneText MOG_LAB_09EE                      /* texte de la zone « sortie » */
#define MOG_t_NextKnightText MOG_LAB_09EF                    /* texte du bouton « chevalier suivant » */
#define MOG_t_ScreenPalette MOG_LAB_09F0                     /* palette des écrans à pointeur */
#define MOG_t_PanelKnightColours MOG_LAB_09F1                /* couleurs des chevaliers des panneaux */
#define MOG_v_HitRowBytes MOG_LAB_0A56                       /* octets d'une ligne de la frame testée (Col_PixelHit) */
#define MOG_s_KnA MOG_SECSTRT_17                             /* "kn.a" */
#define MOG_s_BeA MOG_LAB_0AB7                               /* "Be.a" */
#define MOG_s_BaA MOG_LAB_0AB8                               /* "Ba.a" */
#define MOG_s_DrA MOG_LAB_0AB9                               /* "Dr.a" */
#define MOG_s_ToA MOG_LAB_0ABA                               /* "To.a" */
#define MOG_s_TrA MOG_LAB_0ABB                               /* "Tr.a" */
#define MOG_s_GuA MOG_LAB_0ABD                               /* "Gu.a" */
#define MOG_s_WzA MOG_LAB_0ABE                               /* "Wz.a" */
#define MOG_s_RaA MOG_LAB_0ABF                               /* "Ra.a" */
#define MOG_s_MuA MOG_LAB_0AC0                               /* "Mu.a" */
#define MOG_s_ReA MOG_LAB_0AC1                               /* "Re.a" */
#define MOG_s_HeA MOG_LAB_0AC2                               /* "He.a" */
#define MOG_v_FileSize MOG_L23_0000E                         /* taille du fichier ouvert (répertoire) */
#define MOG_v_FileError MOG_L23_0001A                        /* 0 ou -1 : fichier introuvable (LAB_0BB5) */
#define MOG_v_CelLastName MOG_LAB_0CC9                       /* nom de la dernière CEL chargée (LAB_0CBB) */
#define MOG_v_CelLastSize MOG_LAB_0CCA                       /* place occupée par cette CEL (LAB_0CB6 la relit) */
#define MOG_t_BitReverse MOG_LAB_0CD9                        /* octet aux bits inversés (256 octets, frames retournées) */
#define MOG_v_ScreenStride MOG_SECSTRT_29                    /* largeur d'une ligne d'écran en octets */
#define MOG_t_DestPlanes MOG_LAB_0CFF                        /* 5 pointeurs de plans de destination (dessin, décodage des PIV) */
#define MOG_v_CelPlanesMax MOG_LAB_0D04                      /* dernier plan dessiné (4 : 5 plans) */
#define MOG_v_BlitByCpu MOG_LAB_0D05                         /* copie des plans par le processeur (textes, écrans) plutôt que le blitter */
#define MOG_v_CelHeader MOG_LAB_0D1C                         /* copie de l'en-tête CEL (10 octets) : mot nombre de frames */
#define MOG_v_CelHeaderPacked MOG_LAB_0D1D                   /* en-tête CEL +2 : taille compressée */
#define MOG_v_CelHeaderBits MOG_LAB_0D1F                     /* en-tête CEL +6 : taille décompressée en bits */
#define MOG_v_CelPlaneMask MOG_LAB_0D21                      /* masque des plans de la frame (table des frames +9) */
#define MOG_v_CelSkipLeft MOG_LAB_0D22                       /* octets sautés à gauche (coupe) */
#define MOG_v_CelSkipRight MOG_LAB_0D23                      /* octets sautés à droite (coupe) */
#define MOG_v_CelClipLeft MOG_LAB_0D24                       /* frame coupée au bord gauche */
#define MOG_v_CelClipRight MOG_LAB_0D25                      /* frame coupée au bord droit */
#define MOG_v_ClipHeight MOG_LAB_0D26                        /* hauteur de la zone de dessin (200) */
#define MOG_v_ClipWidth MOG_LAB_0D27                         /* largeur de la zone de dessin en octets (40) */
#define MOG_v_CelDestOffset MOG_LAB_0D28                     /* décalage ajouté à l'adresse de destination */
#define MOG_v_CelPlaneSize MOG_LAB_0D29                      /* taille d'un plan de la frame */
#define MOG_v_CelShift MOG_LAB_0D2A                          /* décalage en pixels (0-15) ; premier mot d'une liste copper */
#define MOG_v_GfxTableB MOG_LAB_0D3D                         /* table de conversion de pixels ($222E octets) */
#define MOG_v_GfxTableA MOG_LAB_0D3E                         /* table de conversion de pixels ($1000 octets, dans b_Gfx) */
#define MOG_v_GfxTableC MOG_LAB_0D3F                         /* table de conversion de pixels ($1000 octets) */
#define MOG_v_CelPlanesBuf MOG_LAB_0D40                      /* tampon de la frame : 5 plans + masque de $12C0 octets */
#define MOG_v_GfxTableD MOG_LAB_0D41                         /* table de conversion de pixels */
#define MOG_v_ScreenStrideSet MOG_LAB_0D4C                   /* non nul : largeur de ligne v_ScreenStride au lieu de 40 */
#define MOG_v_GfxReady MOG_LAB_0D4D                          /* tables graphiques prêtes (sinon SECSTRT_30 au premier dessin) */
#define MOG_b_Gfx MOG_SECSTRT_32                             /* bloc des tables graphiques et du tampon de frame */
#define MOG_b_Unpack MOG_LAB_0D4F                            /* tampon des données compressées (41 244 octets) */
#define MOG_v_PalFadeTarget MOG_SECSTRT_39                   /* palette visée par le fondu (0 : aucun) */
#define MOG_v_PalFadeDelay MOG_LAB_0E91                      /* VBL entre deux pas du fondu */
#define MOG_v_PalFadeCount MOG_LAB_0E92                      /* VBL avant le pas suivant du fondu */
#define MOG_v_PalCurrent MOG_LAB_0E93                        /* palette courante (pointeur) */
#define MOG_t_ColourCycles MOG_LAB_0E94                      /* rotations de couleurs (LAB_0E56) */
#define MOG_t_ColourGlows MOG_LAB_0E95                       /* pulsations de couleurs */
#define MOG_s_GoldPieces MOG_LAB_0F47                        /* "     gold pieces." */
#define MOG_s_GoldPieces2 MOG_LAB_0F49                       /* "     gold pieces." */
#define MOG_s_GoldPieces3 MOG_LAB_0F4B                       /* "     gold pieces." */
#define MOG_s_TavPiv MOG_LAB_0F4D                            /* "tav.piv" */
#define MOG_s_DicePiv MOG_LAB_0F4E                           /* "dice.piv" */
#define MOG_s_DiceCel MOG_LAB_0F4F                           /* "dice.cel" */
#define MOG_s_Hen1P MOG_LAB_0F50                             /* "Hen1.p" */
#define MOG_s_Hen1C MOG_LAB_0F51                             /* "Hen1.c" */
#define MOG_v_Voice0 MOG_SECSTRT_44                          /* voie 0 du pilote (148 octets) */
#define MOG_v_Voice1 MOG_LAB_0F66                            /* voie 1 */
#define MOG_v_Voice2 MOG_LAB_0F67                            /* voie 2 */
#define MOG_v_Voice3 MOG_LAB_0F68                            /* voie 3 */
#define MOG_v_SoundFading MOG_LAB_0FC4                       /* fondu en cours : volume des voies baissé (LAB_0FC2) */

#endif
