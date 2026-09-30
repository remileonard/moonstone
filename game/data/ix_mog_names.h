/*
 * ix_mog_names.h — noms des labels de amiga_asm/mog.asm utilisés par le C
 * (générés par tools/ix_names.py depuis game/data/mog_names.txt ;
 * ne pas modifier).
 */
#ifndef IX_MOG_NAMES_H
#define IX_MOG_NAMES_H

#include "ix_mog_syms.h"

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
#define MOG_b_MapCreatures MOG_LAB_05C6                      /* 24 créatures de la carte (20 octets chacune) */
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
#define MOG_v_TerrainNextGlade MOG_LAB_05E8                  /* terrain suivant (clairière, LAB_07B8) */
#define MOG_v_TerrainNextWater MOG_LAB_05E9                  /* terrain suivant (eau, LAB_07B6) */
#define MOG_v_TerrainNextForest MOG_LAB_05EA                 /* terrain suivant (forêt, LAB_07B7) */
#define MOG_v_TerrainNextSwamp MOG_LAB_05EB                  /* terrain suivant (marais, LAB_07B9 ; octet) */
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
#define MOG_t_FightPalette MOG_LAB_08D6                      /* palette du combat seul (mog_fight) */
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
#define MOG_v_HitRowBytes MOG_LAB_0A56                       /* octets d'une ligne de la frame testée (Col_PixelHit) */
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
#define MOG_v_Voice0 MOG_SECSTRT_44                          /* voie 0 du pilote (148 octets) */
#define MOG_v_Voice1 MOG_LAB_0F66                            /* voie 1 */
#define MOG_v_Voice2 MOG_LAB_0F67                            /* voie 2 */
#define MOG_v_Voice3 MOG_LAB_0F68                            /* voie 3 */
#define MOG_v_SoundFading MOG_LAB_0FC4                       /* fondu en cours : volume des voies baissé (LAB_0FC2) */

#endif
