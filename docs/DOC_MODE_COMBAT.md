# Documentation — Mode de jeu : Combat

> Document de conception décrivant la séquence de combat de Moonstone —
> A Hard Days Knight, telle qu'implémentée dans le binaire `mog` (sources
> `mog.asm`), avec appui sur la documentation technique (`DOC_TECHNIQUE.md`).
>
> Ce document couvre uniquement la phase de combat déclenchée lorsque le
> joueur, depuis la carte overworld, entre sur un nœud PvE (créature),
> PvP (autre chevalier) ou Dragon. Les boutiques, temples, tavernes et
> la carte overworld sont hors périmètre.

---

## 1. Description de la boucle d'initialisation

### 1.1 Déclenchement depuis la carte

Lorsque le joueur déplace son chevalier sur un nœud de la carte, la routine
de navigation overworld (`LAB_0037`, `mog.asm#L617`) détecte la collision avec
le nœud. En fonction du type du nœud (champ `54(knight)` ou code de nœud),
le programme saute vers l'un des déclencheurs suivants :

| Code nœud | Valeur | Branche         | Type de rencontre                        |
|-----------|--------|-----------------|------------------------------------------|
| `0x01`    | 1      | `LAB_004F`      | Duel PvP (chevalier vs chevalier)         |
| `0x02`    | 2      | `LAB_005B`      | Créature PvE (monstre sur la carte)       |
| `0x1b`    | 27     | `LAB_00A1`      | Sorcier Mythral (Mystic)                  |
| `0x1c`    | 28     | `LAB_009D`      | Vallée des Dieux (boss final)             |
| `0x21`    | 33     | `LAB_004F`      | Duel PvP alternatif (même routine)        |

Pour les combats PvE (`LAB_005B`) :

```asm
LAB_005B:
    MOVE.L  A1,LAB_08C6     ; sauvegarder le pointeur vers la créature courante
    JSR     LAB_0DC8        ; vérifier disponibilité de la créature
    TST.W   LAB_065E        ; créature déjà vaincue ?
    BNE.W   LAB_005D        ; si oui : sauter vers la phase de butin
    BSR.W   LAB_0065        ; effacer l'écran
    JSR     LAB_01A3        ; initialiser la créature (dispatch via LAB_08C8)
    JSR     LAB_0036        ; ?
    ...
    MOVEQ   #2,D0
    JSR     LAB_04CF        ; entrer dans l'état 2 (combat créature)
```

### 1.2 Machine à états : `LAB_04CF` et `LAB_068F`

La variable `LAB_068F` (long) définit l'**état courant** de la machine à états
du jeu interactif. Les états pertinents pour le combat sont :

| `LAB_068F` | Constante | Rôle                                      | Routine     |
|------------|-----------|-------------------------------------------|-------------|
| `1`        | STATE_PVP | Combat PvP chevalier vs chevalier         | `LAB_04D7`  |
| `2`        | STATE_PVE | Combat contre une créature PvE            | `LAB_04D6`  |
| `3`        | STATE_MYS | Sorcier Mythral (Mystic)                  | `LAB_058F`  |
| `5`        | STATE_SHOP | Boutique d'achat d'armes/armures en ville       | `LAB_0522`  |
| `10` (0xa) | STATE_GOD | Vallée des Dieux (boss)                   | `LAB_0590`  |

La fonction `LAB_04CF` (`mog.asm#L10547`) est le **point d'entrée principal**
de tous les combats. Elle :

```asm
LAB_04CF:
    MOVE.W  #$0001,LAB_0D05     ; flag "combat actif"
    MOVE.L  4(A0),LAB_0986      ; sauvegarder contexte
    MOVE.W  #$0000,LAB_0689     ; réinitialiser flag adversaire
    MOVE.L  D0,LAB_068F         ; stocker le nouvel état
    MOVE.W  #$0000,LAB_0984     ; réinitialiser flag fin de combat
    JSR     LAB_03F0             ; fondu palette → noir
    JSR     LAB_0575             ; activer lecture joystick (joueur)
    JSR     LAB_0588             ; initialiser toutes les tables d'animation
    JSR     LAB_04D4             ; initialiser les combattants (voir §1.3)
```

### 1.3 Initialisation des combattants : `LAB_04D4`

`LAB_04D4` (`mog.asm#L10587`) est le cœur de l'initialisation du combat.
Elle :

1. Appelle `LAB_044E` : vide le pool d'entités (`SECSTRT_14`, 0x95f octets)
   et réinitialise `LAB_0A58` (24 octets de structure scrolling/tile).
2. Appelle `LAB_04EA` : prépare les buffers de fond (double-buffer).
3. Copie le fond de carte (`LAB_05C0 → LAB_0D92`) via `LAB_0419`.
4. Appelle `LAB_058A` : initialise les pointeurs combattants :
   - `LAB_068B` = pointeur vers les données du joueur (chevalier courant)
   - `LAB_068C` = pointeur vers les données de stats du joueur (`96(knight)`)
   - `LAB_068D` = pointeur vers l'entité adverse (ennemi ou autre chevalier)
   - `LAB_068E` = pointeur vers les stats de l'adversaire
5. Appelle `LAB_03A7` : vide le buffer de dessin (`LAB_064D`, 0x2cf octets).
6. Initialise la table de tuiles combat (`LAB_0699`) dans `LAB_0688`.
7. Charge la bonne table de décor selon l'état `LAB_068F` (voir §2).
8. Entre dans la boucle de combat `LAB_04D0`.

### 1.4 Boucle principale : `LAB_04D0`

```asm
LAB_04D0:                              ; boucle de combat principale
    MOVEQ   #0,D0
    MOVEQ   #0,D1
    MOVE.W  LAB_097F,D0                ; position X curseur/joueur
    MOVE.W  LAB_0980,D1                ; position Y curseur/joueur
    JSR     LAB_0451                   ; détecter collision/action
    TST.L   D0
    BEQ.S   LAB_04D1                   ; rien → frame suivante
    TST.W   LAB_0981                   ; input verrouillé ?
    BNE.S   LAB_04D1
    BSR.W   LAB_052A                   ; évaluer la fin du combat (voir §7)
    TST.W   LAB_0984                   ; combat terminé ?
    BNE.S   LAB_04D2                   ; oui → sortie
LAB_04D1:
    JSR     LAB_0416                   ; afficher sprites
    JSR     LAB_039E                   ; timer frame (synchronisation VBL)
    BRA.S   LAB_04D0                   ; reboucler
LAB_04D2:
    JSR     LAB_03F0                   ; fondu palette → noir
    JSR     LAB_057B                   ; désactiver lecture joystick
    ...
    RTS
```

---

## 2. Construction des décors — PIV et Stiles

### 2.1 Fichiers PIV de fond de combat

Les fichiers `.PIV` sont au format propriétaire Mindscape : `word[0]` = nombre
de plans (4 ou 5), suivi de la palette (32 mots Amiga 12-bit), puis du body
**compressé LZSS** (même algorithme que les `.cel`, `LAB_049C`).

| Label       | Fichier         | Plans | Rôle                                                              |
|-------------|-----------------|-------|-------------------------------------------------------------------|
| `LAB_07AF`  | `ch.piv`        | —     | **Fond principal de combat** — fond commun à tous les combats PvP et PvE chargé dans `LAB_012C` via `JSR LAB_0BB5` |
| `LAB_07AE`  | `message.piv`   | 4     | Fond de l'écran de messages post-combat (dialogue, butin)         |
| `LAB_091B`  | `HEA.piv`       | —     | Fond du temple de soin (état 9, `LAB_0767`)                       |
| `LAB_091C`  | `MYS.piv`       | —     | Fond du sorcier Mythral — état 3 (`mog.asm#L9612`)                |

Le chargement du fond `ch.piv` et de `message.piv` est réalisé dans
`LAB_012C` (`mog.asm#L2772`) :

```asm
LAB_012C:
    JSR     LAB_03EB            ; éteindre DMA audio (couper le son)
    LEA     LAB_07AE,A0         ; "message.piv"
    JSR     LAB_0BB5            ; charger le PIV → buffer LAB_05B9+52
    ...
    LEA     LAB_07AF,A0         ; "ch.piv"
    JSR     LAB_0BB5            ; charger le PIV → buffer LAB_05B9+56
    ...
    LEA     LAB_078B,A0         ; "bold.f"
    JSR     LAB_0CBB            ; charger la police de combat
    MOVEQ   #2,D0
    JSR     LAB_0100            ; sélectionner 2 plans (?)
    JSR     LAB_0AA7            ; démarrer SoundTracker combat
```

### 2.2 Tileset `.stile` — décors de tuiles

Le fichier `co.stile` (960 octets, magic `0000 0000`) est le **tileset de
décors de combat**. Il est décompressé par `LAB_0448` / `SECSTRT_21`
(décompresseur bitplane RLE 2 bits, `program.asm#L7923`) :

| Fichier        | Taille | Magic       | Rôle                                           |
|----------------|--------|-------------|------------------------------------------------|
| `co.stile`     | 960    | `0000 0000` | Tileset de décors de combat (combat background tileset) |
| `intro.stile`  | 960    | `0000 0001` | Tileset de l'intro (hors combat)               |

Les opcodes du décompresseur 2 bits :
- `00` = remplissage nul (zéros)
- `01` = copie 4 octets littéraux
- `10` = back-reference courte (longueur 5 bits + 8 = 8..38 octets)
- `11` = back-reference longue variable

### 2.3 Tables de tuiles animées et de fond

La routine `LAB_04D4` initialise deux tables de tuiles selon l'état de jeu :

```asm
; Phase 1 — table des tuiles "joueur" (chevalier, animations simples)
MOVE.L  #LAB_0699,LAB_0688  ; table tuiles joueur (94 longs × 4 = 376 octets)

; Dispatch selon LAB_068F (état combat)
LEA     LAB_098A,A2         ; table de thresholds état → type de fond
...
; Phase 2 — table de tuiles "adversaire/décor"
MOVE.L  #LAB_069A,LAB_0688  ; table tuiles adversaire (94 longs)
MOVE.W  #$0096,LAB_0985     ; 150 tuiles décor
```

Les tables `LAB_0699` et `LAB_069A` (chacune 94 longs + 1 word en BSS,
`mog.asm#L12953-12958`) contiennent les **pointeurs vers les frames de tuiles**
peuplés dynamiquement par `LAB_04F8`.

### 2.4 Tables de pointeurs d'animation par état

La routine `LAB_0588` (`mog.asm#L12107`) initialise six tables de 27 longs
chacune en mémoire BSS (de `LAB_0692` à `LAB_0698`) avec des pointeurs vers
des chaînes de texte d'UI (items, stats) propres à chaque état de combat :

| Label       | État `LAB_068F` | Rôle                                              |
|-------------|-----------------|---------------------------------------------------|
| `LAB_0692`  | Tous états  | Table textes côté joueur — stats (Strength/Endurance/Constitution/Life points/Gold) + noms d'items ; les 3 premiers entrées ("Increase Strength/Endurance/Constitution") remplacent optionnellement les tiles quand le chevalier a assez d'or et que la stat n'est pas à 5 |
| `LAB_0693`  | Temple (9)      | Table UI temple — textes de soin / skill          |
| `LAB_0694`  | PvP (1)         | Table UI combat chevalier vs chevalier            |
| `LAB_0695`  | —               | Table UI auxiliaire                               |
| `LAB_0696`  | Mystic (3)      | Table UI sorcier Mythral — offres d'items         |
| `LAB_0697`  | Boutique (6)    | Table UI boutique/inventaire                      |
| `LAB_0698`  | PvE (2)         | **Table UI combat créature** — menu butin         |

La routine `LAB_058A` (`mog.asm#L12288`) sélectionne la bonne table selon
`LAB_068F` et la place dans `LAB_0699` pour la boucle d'affichage.

### 2.5 Palettes de combat

Les palettes Amiga 12-bit sont chargées par `LAB_03F3` (`mog.asm#L8565`),
qui copie `LAB_0D2B` → `LAB_08D9` puis applique la palette selon l'index D0 :

| Valeur D0 | Type de combat                          | Palette approximative           |
|-----------|-----------------------------------------|---------------------------------|
| `0x00`    | Chevalier ennemi (He/noir)              | Palette sombre (nuit)           |
| `0x0c`    | PvP standard                            | Palette bleue-verte             |
| `0x14`    | Ratmen / TroggAxe                       | Palette verte forestière        |
| `0x18`    | TroggSpear                              | Palette boisée                  |
| `0x20`    | TroggSpear variante                     | —                               |
| `0x24`    | Dragon                                  | Palette rouge/orange (feu)      |
| `0x30`    | Mudmen                                  | Palette boueuse                 |
| `0x40`    | Balok / Démons                          | Palette violette/sombre         |

---

## 3. Initialisation des monstres

### 3.1 Table de créatures : `LAB_05C6` / `LAB_08C6`

Au démarrage de la partie, `LAB_01B6` (`mog.asm#L4442`) initialise les 24
entrées de la table de créatures overworld. Chaque entrée fait **20 octets
(0x14)** :

| Offset | Taille | Contenu                                         |
|--------|--------|-------------------------------------------------|
| `0`    | long   | Pointeur vers les données de loot de la créature (`LAB_05B9+72`) |
| `4`    | long   | Stats de combat : word haut = ATK, word bas = DEF (table `LAB_07BD`) |
| `8`    | word   | Clef de la Vallée portée (`≠ 0` = possède la clef) |
| `10`   | word   | X overworld (table `LAB_07BE`, word haut)       |
| `12`   | word   | Y overworld (table `LAB_07BE`, word bas)        |
| `14`   | word   | Hitbox / type de créature (table `LAB_07BF`)    |
| `16`   | long   | Pointeur vers le fichier son de la créature (table `LAB_07C0`) |

`LAB_08C6` est le **pointeur courant** vers l'entrée de la créature en cours
d'interaction ; il est mis à jour à chaque nœud PvE (`MOVE.L A1,LAB_08C6`
dans `LAB_005B`, `mog.asm#L918`).

### 3.2 Affectation aléatoire du type de créature : `LAB_01B7`

Au démarrage de chaque créature (`LAB_01B5` → `LAB_01B7`, `mog.asm#L4452`),
le moteur tire un nombre pseudo-aléatoire (0..127) et le compare à la table
seuil `LAB_08C5` :

```asm
LAB_08C5:
    DC.L  $00320001  ; seuil 50  → type 1 (Hefalump / chevalier ennemi)
    DC.L  $00460002  ; seuil 70  → type 2 (Ratmen / Ours)
    DC.L  $005a0003  ; seuil 90  → type 3 (Dragon + Hefalump)
    DC.L  $00640004  ; seuil 100 → type 4 (autre)
```

| Type | Probabilité approx. | Créatures spawnées                                              |
|------|---------------------|-----------------------------------------------------------------|
| 1    | 50 %                | 1 Hefalump (chevalier ennemi) via `LAB_046C`                   |
| 2    | 20 %                | 3 créatures « Be » (Ratmen/Ours) via `LAB_0471` ×3            |
| 3    | 20 %                | 1 Hefalump + 2 créatures « Be » (`LAB_046C` + `LAB_0471` ×2) |
| 4    | 10 %                | (type réservé, RTS immédiat)                                    |

### 3.3 Table de dispatch `LAB_08C8` — initialisation par type de créature

La routine `LAB_01A3` (`mog.asm#L4195`) lit le type de créature stocké dans
`4(LAB_08C6)` et utilise **`LAB_08C8`** comme table de sauts (18 longs) :

```asm
LAB_01A3:
    MOVEA.L LAB_08C6,A0         ; entrée créature courante
    MOVE.L  #$00000002,LAB_076D ; activer mode PvE
    MOVE.L  16(A0),LAB_076E     ; stocker son de la créature
    MOVEQ   #0,D0
    MOVE.W  14(A0),D0            ; lire type/hitbox
    MOVE.L  D0,LAB_08C4          ; mémoriser LAB_08C4 = type courant
    MOVE.W  4(A0),D0             ; lire stats
    EXT.L   D0
    LEA     LAB_08C8,A1          ; table de dispatch
    MOVEA.L 0(A1,D0.L),A1       ; charger le handler
    JSR     (A1)                 ; appeler l'initialiseur
```

| Offset dans `LAB_08C8` | Handler    | Créature / ennemi                       | Fichiers CEL/OB chargés                        |
|------------------------|------------|-----------------------------------------|------------------------------------------------|
| `0x00`                 | `LAB_0188` | Chevalier ennemi (Hefalump)             | `He1.ob`, `He2.ob`, `He3.ob` (`LAB_0123`)     |
| `0x04`                 | `LAB_019A` | Mudmen                                  | `Mudmen1.cel`, `Mudmen2.cel` (`LAB_011E`)      |
| `0x08`                 | `LAB_01A0` | Démon / Gardien                         | `Demon1.cel`…`Demon4.cel` (`LAB_0125` → `LAB_07B0..B3`) |
| `0x0c`                 | `LAB_0164` | Créature générique (Ratmen/TroggAxe)    | `Ratmen1.cel`, `Ratmen2.cel` (via `LAB_0116`) |
| `0x14`                 | `LAB_0192` | Dragon                                  | `Dragon1.cel`, `Dragon2.cel` (`LAB_0121` → `LAB_0780`) |
| `0x18`                 | `LAB_0168` | TroggAxe                                | `TroggAxe1.cel`, `TroggAxe2.cel` (`LAB_011A` → `LAB_0778`) |
| `0x1c`                 | `LAB_016A` | TroggAxe variante (même sprites, frames différentes) | `TroggAxe1.cel`, `TroggAxe2.cel` (`LAB_011A` → `LAB_0778`) |
| `0x20`                 | `LAB_0175` | TroggSpear                              | `TroggSpear1.cel`, `TroggSpear2.cel` (`LAB_0118` → `LAB_077A`) |
| `0x24`                 | `LAB_018C` | Ratmen                                  | `Ratmen1.cel`, `Ratmen2.cel` (`LAB_011C` → `LAB_077C`) |
| `0x30`                 | `LAB_0196` | Balok                                   | `Balok1.cel`, `Balok2.cel`, `Balok3.cel` (`LAB_011F` → `LAB_0785`) |
| `0x40`                 | `LAB_019E` | Troll                                   | `Troll1.cel`, `Troll2.cel` (`LAB_0126` → `LAB_07B4`) |

### 3.4 Initialisation du chevalier joueur : `LAB_01A4`

Le chevalier du joueur courant (`LAB_0633`) est initialisé par `LAB_01A4`
(`mog.asm#L4208`) :

```asm
LAB_01A4:
    MOVEA.L LAB_0633,A1       ; pointeur vers struct knight (132 octets)
    MOVE.L  A1,LAB_05F2       ; sauvegarder
    LEA     LAB_05E4,A2       ; table d'entités du joueur
    MOVE.L  A1,0(A2)          ; entité[0] = chevalier joueur
    MOVE.W  #$00fa,4(A1)      ; X initial = 250 (centre-droite)
    MOVE.W  #$0000,6(A1)      ; Y initial = 0
    MOVE.W  #$0064,8(A1)      ; vitesse = 100
    MOVE.B  #$03,10(A1)       ; nombre de joueurs actifs = 3
    JSR     LAB_0167          ; régler les pointeurs de script d'animation
    MOVEA.L #LAB_07FC,A0      ; table d'animation par défaut
    BRA.W   LAB_01A9          ; charger le bon kn.ob selon la faction
```

La structure knight (132 octets, `LAB_0613`, stride `0x84 = 132`) :

| Offset | Taille | Champ                              |
|--------|--------|------------------------------------|
| `4`    | word   | X position pixel                   |
| `6`    | word   | Y position pixel                   |
| `8`    | word   | Vitesse de déplacement             |
| `10`   | byte   | Nombre de joueurs                  |
| `11`   | byte   | Flags (bit 0 = joueur 2 actif)     |
| `54`   | long   | Faction (0=Richard, 1=Godber, 2=Jeffrey, 3=Edward) |
| `70`   | byte   | Strength level (moonstone power)   |
| `71`   | byte   | Endurance level                    |
| `72`   | byte   | Constitution level                 |
| `73`   | byte   | Skill level (0..5)                 |
| `74`   | word   | Armour bonus                       |
| `76`   | byte   | Nombre d'items en poche            |
| `77`   | byte   | Type d'arme (0=dagger, >0=épée)    |
| `78`   | word   | Score / étape overworld            |
| `80`   | word   | HP courants                        |
| `84`   | word   | HP max                             |
| `88`   | long   | Bonus attaque                      |
| `92`   | long   | Bonus défense                      |
| `96`   | long   | Pointeur vers stats étendues       |
| `100`  | long   | Pointeur vers dernier adversaire   |
| `126`  | word   | X pixel courant (during combat)    |
| `128`  | word   | Y pixel courant (during combat)    |

### 3.5 Initialisation des 4 chevaliers joueurs : `LAB_01C4`

```asm
LAB_01C4:
    MOVE.L  A2,96(A1)         ; assigner zone stats étendue (LAB_0618+n*0x18)
    BSR.W   LAB_01C6          ; init commune (skill=5, HP=20, etc.)
    JSR     LAB_0167          ; script d'animation
    ADDA.L  #$00000018,A2     ; avancer ptr stats étendue (24 octets)
    ADDA.L  #$00000084,A1     ; avancer ptr knight (132 octets)
    DBF     D7,LAB_01C4       ; boucle ×4
```

`LAB_01C6` (`mog.asm#L4555`) applique les valeurs initiales communes :

| Champ                 | Valeur initiale |
|-----------------------|-----------------|
| `70(A1)` Strength     | 1               |
| `71(A1)` Endurance    | 1               |
| `72(A1)` Constitution | 1               |
| `73(A1)` Skill        | 5               |
| `80(A1)` HP courant   | 20              |
| `92(A1)` Bonus déf    | 27 (0x1b)       |
| `88(A1)` Bonus atk    | 22 (0x16)       |
| `78(A1)` Score        | 0               |
| `76(A1)` Items        | 10 (0x0a)       |
| `83(A1)` Armure       | 0xFF (aucune)   |

---

## 4. L'IA

### 4.1 Architecture générale

L'IA des créatures est un **automate de comportement** déclenché à chaque
frame par la boucle `LAB_04D0`. Deux niveaux coexistent :

1. **IA de déplacement** — `LAB_0E0C` et `LAB_0E07` : déplace l'adversaire
   vers le chevalier joueur en calculant ΔX/ΔY.
2. **IA d'attaque** — scripts de collision (`LAB_03A7`, `LAB_03AC`…`LAB_03AF`) :
   déclenchent une attaque quand la distance est suffisamment faible.

### 4.2 IA de déplacement : `LAB_0E0C`

```asm
LAB_0E0C:                          ; appelée chaque frame pour l'adversaire IA
    MOVE.W  #$0000,LAB_0656        ; réinitialiser flag
    MOVEA.L LAB_0633,A0            ; chevalier joueur
    TST.W   LAB_0674               ; flag "IA active" ?
    BNE.W   LAB_0E13               ; déjà actif → vérifier état
    MOVE.W  #$0001,LAB_0674
    MOVE.W  126(A0),D0             ; X joueur
    MOVE.W  128(A0),D1             ; Y joueur
    TST.L   100(A0)                ; adversaire précédent ?
    BEQ.S   LAB_0E0D
    MOVEA.L 100(A0),A1             ; utiliser sa position
    MOVE.W  126(A1),D2
    MOVE.W  128(A1),D3
    BRA.S   LAB_0E0F
LAB_0E0E:
    MOVEA.L LAB_0673,A0            ; dernier nœud ciblé
    MOVE.W  10(A0),D2              ; X cible
    MOVE.W  12(A0),D3              ; Y cible
LAB_0E0F:
    MOVEQ   #1,D4
    SUB.W   D0,D2                  ; ΔX
    BPL.W   LAB_0E10
    NEG.W   D2                     ; |ΔX|
    ...
```

### 4.3 IA de détection des limites de l'arène : `LAB_0E07`

`LAB_0E07` vérifie que l'adversaire reste dans les bornes de l'arène
(320×190 pixels) et retire le droit de se déplacer dans la direction
correspondante si une bordure est atteinte :

```asm
CMP.W  #$0000,D0 → BCLR #1,LAB_0657  ; sortie gauche (bit 1 = mouvement gauche interdit)
CMP.W  #$0136,D0 → BCLR #0,LAB_0657  ; sortie droite
CMP.W  #$0000,D1 → BCLR #3,LAB_0657  ; sortie haut
CMP.W  #$00be,D1 → BCLR #2,LAB_0657  ; sortie bas (0xBE = 190)
```

### 4.4 IA de collision et résolution de combat : `LAB_03A7` / `LAB_03AC`

`LAB_03A7` (`mog.asm#L8015`) est le **gestionnaire de collision par entité**.
Pour chaque paire d'entités actives, il compare les hitboxes et détermine
si un coup porte :

```asm
LAB_03A7:
    LEA     LAB_064D,A0       ; vider buffer de collision
    MOVE.L  #$000002cf,D0
    ...
    JSR     LAB_03AC          ; évaluer une collision individuelle
```

`LAB_03AC` compare :
- `58(A0)`, `60(A0)` (hitbox attaquant, avec bonus vitesse `LAB_0635`)
- `58(A1)`, `60(A1)` (hitbox défenseur)
- `112(A0)`, `114(A0)` (hitbox secondaire attaquant)
- `112(A1)`, `114(A1)` (hitbox secondaire défenseur)

Si la collision est confirmée (`D5 == 2`), le bit correspondant dans
`LAB_03B6` est effacé → cela signale un **coup reçu** pour cet adversaire.

### 4.5 IA de comportement par type de créature

Chaque type de créature possède son propre **script de comportement** stocké
sous forme de bytecode dans les tables `LAB_0600`…`LAB_0611`. Ces scripts
définissent les séquences d'animation, les délais d'attaque et les patterns
de déplacement. Par exemple :

| Table       | Créature     | Comportement principal                             |
|-------------|--------------|-----------------------------------------------------|
| `LAB_0600`  | He (knight)  | Marche → pause → attaque épée → retraite            |
| `LAB_0602`  | He (variante)| Attaque plus agressive (bonus D0=3 ou 5 selon arme) |
| `LAB_0603`  | Dragon       | Déplacement lent + attaque de feu                   |
| `LAB_0604`  | Dragon       | Animation de feu continue                           |
| `LAB_0605`  | Mudmen       | Déplacement erratique + frappes multiples           |
| `LAB_0606`  | Mudmen var.  | Deux animaux simultanés                             |
| `LAB_060A`  | Balok        | Alternance gauche/droite + attaque massive          |

Le script Dragon tient compte du niveau d'armure du joueur
(champ `18(A0)` = dernier type de décor) pour adapter ses dommages :

```asm
LAB_0190:
    CMPI.W  #$002d,18(A0)    ; épée du joueur = Broad Sword ?
    BNE.S   LAB_0191
    MOVE.W  #$0007,80(A1)    ; augmenter HP dragon (attaque joueur moins efficace)
    MOVE.W  #$0007,84(A1)
LAB_0191:
    CMPI.W  #$0031,18(A0)    ; épée = Claymore ?
    BNE.S   ...
    MOVE.W  #$000c,80(A1)    ; HP encore plus élevés
```

---

## 5. Gestion des inputs du joueur

### 5.1 Activation et désactivation de la lecture joystick

La lecture du joystick est activée par `LAB_0575` (`mog.asm#L12020`) et
désactivée par `LAB_057B` (`mog.asm#L12047`) :

```asm
LAB_0575:                        ; activer inputs
    TST.W   LAB_097C
    BNE.W   LAB_057A             ; déjà actif
    MOVE.W  #$0001,LAB_097C      ; flag "input actif"
    JSR     LAB_0E75             ; configurer CIA joystick
    ...
    LEA     LAB_0B96,A0          ; table des callbacks VBL
    ...
    MOVE.L  #LAB_057D,-4(A0)    ; installer handler joystick dans VBL
    MOVE.L  A0,LAB_0982          ; sauvegarder slot

LAB_057B:                        ; désactiver inputs
    TST.W   LAB_097C
    BEQ.S   LAB_057C
    MOVE.W  #$0000,LAB_097C      ; désactiver
    JSR     LAB_0E76             ; déconfigurer CIA
    MOVEA.L LAB_0982,A0
    CLR.L   (A0)                 ; retirer handler VBL
```

### 5.2 Handler joystick VBL : `LAB_057D`

`LAB_057D` (`mog.asm#L12058`) est appelé à chaque VBL pendant le combat.
Il lit l'état du joystick via `LAB_00EE`, puis met à jour les positions
`LAB_097F` (X) et `LAB_0980` (Y) du chevalier joueur :

```asm
LAB_057D:
    MOVE.W  #$0001,LAB_0981      ; flag "input en cours"
    JSR     LAB_00EE             ; lire joystick → D0=déplacement, D1=boutons
    MOVEA.L LAB_068B,A0          ; chevalier joueur
    CMPI.B  #$01,11(A0)          ; joueur 2 actif ?
    BNE.S   LAB_057E
    MOVE.W  D0,D1                ; joystick J2 = même input
LAB_057E:
    BTST    #0,D1                ; bouton fire / direction droite ?
    BEQ.S   LAB_057F
    ADDI.W  #$0002,LAB_097F      ; X += 2
LAB_057F:
    BTST    #1,D1                ; gauche ?
    BEQ.S   LAB_0580
    SUBI.W  #$0002,LAB_097F      ; X -= 2
LAB_0580:
    BTST    #2,D1                ; bas ?
    BEQ.S   LAB_0581
    ADDI.W  #$0002,LAB_0980      ; Y += 2
LAB_0581:
    BTST    #3,D1                ; haut ?
    BEQ.S   LAB_0582
    SUBI.W  #$0002,LAB_0980      ; Y -= 2
LAB_0582:
    BTST    #4,D1                ; bouton feu/attaque ?
    BEQ.S   LAB_0583
    MOVE.W  #$0000,LAB_0981      ; verrouiller input (attaque en cours)
```

### 5.3 Bornes de déplacement

Après lecture du joystick, les positions sont contraintes :

| Variable    | Min   | Max (hex)  | Description          |
|-------------|-------|------------|----------------------|
| `LAB_097F`  | 0     | `0x013a`   | X (0..314 px)        |
| `LAB_0980`  | 0     | `0x00c3`   | Y (0..195 px)        |

### 5.4 Correspondance états / inputs

Le tableau suivant résume les correspondances entre l'état courant
(`LAB_068F`) et les inputs reconnus dans la boucle `LAB_007D`…`LAB_0089`
(`mog.asm#L1230`), qui gère la **carte overworld** avant le combat :

| Touche (raw) | Hex  | Action en combat overworld          |
|--------------|------|--------------------------------------|
| `A`          | 0x41 | Déplacer chevalier vers la gauche   |
| `M`          | 0x4d | Action « map » (vérifier inventaire)|
| `R`          | 0x52 | Recharger (Ratmen spécial)          |
| `W`          | 0x57 | Monter / attaquer en haut           |
| `S`          | 0x53 | Attaquer (strike)                   |
| `H`          | 0x48 | Recul (hit back)                    |
| `G`          | 0x47 | Gagner (validate victory)           |
| `Q`          | 0x51 | Quitter / ne rien faire             |

En combat actif (boucle `LAB_04D0`), seul le joystick est utilisé ;
les touches clavier ne sont pas lues pendant la boucle de combat.

---

## 6. Le son

### 6.1 Musique de combat — SoundTracker

La musique de fond pendant le combat est le module SoundTracker
`vmusic.cmp` (60 994 octets, compressé RNC1). Le démarrage du moteur
musical est effectué par `LAB_0AA7` (`mog.asm#L19329`), qui :

1. Installe le handler VBL du SoundTracker (`LAB_0F73`) dans la table VBL.
2. Installe l'IRQ audio 4 (`LAB_0F69`) pour la mise à jour des samples.
3. Charge les samples PCM via `LAB_0AB5` (musique `Re.a` = replay module).
4. Réinitialise `LAB_0AA6` = 0 (aucun canal actif).

Le module `music.cmp` (88 187 octets) est utilisé pour l'overworld ;
`vmusic.cmp` est utilisé pour les combats et les séquences spéciales.

### 6.2 Effets sonores — fichiers `.t`

Les effets sonores de combat sont stockés dans des fichiers `.t` (samples
PCM bruts 8-bit Amiga). Ils sont chargés en mémoire lors de l'init de
chaque type de créature (table `LAB_07C0`, `mog.asm#L13887`) :

#### Sons des chevaliers et créatures (table `LAB_07C0`)

| Index | Fichier   | Label      | Rôle                                |
|-------|-----------|------------|-------------------------------------|
| 0     | `fol1.t`  | `LAB_07C1` | Bruit de pas type 1 (sol mou)       |
| 1     | `fol2.t`  | `LAB_07C2` | Bruit de pas type 2                 |
| 2     | `fol3.t`  | `LAB_07C3` | Bruit de pas type 3                 |
| 3     | `fol4.t`  | `LAB_07C4` | Bruit de pas type 4                 |
| 4     | `fol5.t`  | `LAB_07C5` | Bruit de pas type 5                 |
| 5     | `fol6.t`  | `LAB_07C6` | Bruit de pas type 6                 |
| 6     | `wal1.t`  | `LAB_07C7` | Son de frappe/contact type 1        |
| 7     | `wal2.t`  | `LAB_07C8` | Son de frappe type 2                |
| 8     | `wal3.t`  | `LAB_07C9` | Son de frappe type 3                |
| 9     | `wal4.t`  | `LAB_07CA` | Son de frappe type 4                |
| 10    | `wal5.t`  | `LAB_07CB` | Son de frappe type 5                |
| 11    | `wal6.t`  | `LAB_07CC` | Son de frappe type 6                |
| 12    | `swl1.t`  | `LAB_07CD` | Sifflement épée type 1              |
| 13    | `swl2.t`  | `LAB_07CE` | Sifflement épée type 2              |
| 14    | `swl3.t`  | `LAB_07CF` | Sifflement épée type 3              |
| 15    | `swl4.t`  | `LAB_07D0` | Sifflement épée type 4              |
| 16    | `swl5.t`  | `LAB_07D1` | Sifflement épée type 5              |
| 17    | `swl6.t`  | `LAB_07D2` | Sifflement épée type 6              |
| 18    | `gll1.t`  | `LAB_07D3` | Grognement/impact type 1            |
| 19    | `gll2.t`  | `LAB_07D4` | Grognement/impact type 2            |
| 20    | `gll3.t`  | `LAB_07D5` | Grognement/impact type 3            |
| 21    | `gll4.t`  | `LAB_07D6` | Grognement/impact type 4            |
| 22    | `gll5.t`  | `LAB_07D7` | Grognement/impact type 5            |
| 23    | `gll6.t`  | `LAB_07D8` | Grognement/impact type 6            |

#### Sons des armes du joueur (tables `LAB_07B6`…`LAB_07B9`)

Quatre catégories de sons d'armes sont sélectionnées selon l'arme équipée :

| Table      | Fichiers      | Arme correspondante          |
|------------|---------------|------------------------------|
| `LAB_07B9` | `FO1.t`…`FO8.t` | Dague / attaque de base     |
| `LAB_07B8` | `Sw1.t`…`Sw8.t` | Épée longue / Broad Sword   |
| `LAB_07B7` | `GL1.t`…`GL8.t` | Claymore                    |
| `LAB_07B6` | `Wa1.t`…`Wa8.t` | Sword of Sharpness           |

Chaque table contient 8 pointeurs vers des fichiers son `.t` ; le moteur
sélectionne l'un d'eux de façon cyclique (compteur `LAB_0AA5` modulo 8)
pour varier les sons d'attaque.

#### Sons de combat spéciaux

| Fichier   | Label     | Rôle                                       |
|-----------|-----------|--------------------------------------------|
| `kn.a`    | `SECSTRT_17` | Son du chevalier joueur (marche/cri)    |
| `Be.a`    | `LAB_0AB7` | Son de créature Be (Ratmen/Ours)           |
| `Ba.a`    | `LAB_0AB8` | Son de Balok                               |
| `Dr.a`    | `LAB_0AB9` | Son du Dragon                              |
| `To.a`    | `LAB_0ABA` | Son du Troll                               |
| `Tr.a`    | `LAB_0ABB` | Son du TroggAxe/TroggSpear                 |
| `Wn.a`    | `LAB_0ABC` | Son du Sorcier (Wn = Wyvern ?)             |
| `Gu.a`    | `LAB_0ABD` | Son du garde (Guard)                       |
| `Wz.a`    | `LAB_0ABE` | Son du Wizard (Mythral)                    |
| `Ra.a`    | `LAB_0ABF` | Son du Ratman                              |
| `Mu.a`    | `LAB_0AC0` | Son du Mudman                              |
| `Re.a`    | `LAB_0AC1` | Module de replay (SoundTracker engine)     |
| `He.a`    | `LAB_0AC2` | Son du Hefalump (chevalier ennemi)         |

### 6.3 Moteur de lecture sonore : `LAB_0F8C`

`LAB_0F8C` (`mog.asm#L28189`) est la routine de lecture d'un effet sonore.
Elle prend en entrée :
- `D0` = index du sample (dans `LAB_1098`)
- `D1` = canal audio Paula (0..3)

Elle programme les registres hardware Amiga :
- `$DFF0A0..DFF0D0` = DMA audio canaux 0..3
- `$DFF0A8..DFF0D8` = longueur du sample
- `$DFF0AA..DFF0DA` = période (fréquence)
- `$DFF150` = `DMACON` (activer canal)

Les quatre canaux audio Paula sont alloués via `LAB_0AA2` /
`SECSTRT_16` (`mog.asm#L19274`) qui tournent sur les canaux libres
(bit `LAB_0AA6` = masque de canaux occupés, 4 bits).

### 6.4 Arrêt du son et fondu

`LAB_03EB` (`mog.asm#L8510`) coupe immédiatement tous les canaux audio :

```asm
LAB_03EB:
    MOVEA.L #$00dff180,A1    ; registre couleur (palette) → DMA
    MOVEA.L LAB_0E93,A2      ; buffer palette courant
    MOVEQ   #32,D0
LAB_03EC:
    MOVE.W  #$0000,(A1)+     ; vider palette
    MOVE.W  #$0000,(A2)+     ; vider buffer
    DBNE    D0,LAB_03EC
```

`LAB_0AA9` (`mog.asm#L19345`) coupe proprement les quatre canaux
audio en appelant séquentiellement `LAB_0A9E`…`LAB_0AA1` (one per channel).

---

## 7. La fin du combat

### 7.1 Détection de fin : `LAB_052A`

`LAB_052A` (`mog.asm#L11482`) est appelée à chaque frame depuis
`LAB_04D0` pour détecter si le combat est terminé. Elle examine
le champ `16(A0)` (type d'animation courante) et `20(A0)` (état
de l'entité) :

```asm
LAB_052A:
    CMPI.L  #$00000007,16(A0)     ; type 7 = mort/sortie ?
    BNE.S   LAB_052B
    MOVE.W  #$0001,LAB_0984       ; flag fin de combat
    RTS
LAB_052B:
    CMPI.L  #LAB_09EF,8(A0)       ; entité = objet de fin ?
    BNE.S   LAB_052C
    BSR.W   LAB_0528              ; traiter objet final
    BSR.W   LAB_04D4              ; réinitialiser combat
    RTS
LAB_052C:
    ; dispatcher selon le résultat (D2 low nibble = outcome code)
    MOVE.W  20(A2),D2
    ANDI.W  #$000f,D2             ; outcome code
    CMP.W   #$0005,D2  → LAB_052D ; victoire joueur (hit)
    CMP.W   #$0001,D2  → LAB_053F ; défaite joueur
    CMP.W   #$0003,D2  → LAB_0544 ; match nul
    CMP.W   #$000a,D2  → LAB_0558 ; victoire Claymore
    CMP.W   #$000c,D2  → LAB_053D ; fuite adversaire
```

### 7.2 Variable `LAB_0984`

`LAB_0984` (word, `mog.asm#L17173`) est le **flag de sortie de
la boucle de combat** :
- `0` = combat en cours
- `1` = combat terminé (victoire, défaite, fuite ou match nul)

Quand `LAB_0984 == 1`, `LAB_04D0` sort de la boucle et appelle `LAB_04D2`.

### 7.3 Résultats possibles du combat

| Code `D2 & 0xF` | Label     | Résultat                                                      |
|-----------------|-----------|---------------------------------------------------------------|
| `0x05`          | `LAB_052D` | **Victoire joueur** : met à jour stats, butin, skill         |
| `0x01`          | `LAB_053F` | **Défaite joueur** : perte de skill et d'items               |
| `0x03`          | `LAB_0544` | **Match nul** : aucune conséquence                           |
| `0x0a`          | `LAB_0558` | **Victoire Claymore** : double l'or (`LAB_0665 × 2`)         |
| `0x0c`          | `LAB_053D` | **Fuite adversaire** : halve l'or, `LAB_053B` conservé      |
| `0x0e`          | `LAB_0533` | **Transition vers armurier** (état 8), `LAB_04CF(8)`         |
| `0x10`          | `LAB_0537` | **Transition vers taverne** (état 11), `LAB_04CF(11)`        |
| `0x02`          | `LAB_0534` | **Réinitialiser combat** : `LAB_0E02`, `LAB_0984=1`          |
| `0x0c+flag`     | `LAB_0536` | **IA succès/échec** : `LAB_0E05` ou `LAB_0E06`              |

### 7.4 Victoire contre une créature PvE

Quand le joueur gagne le combat PvE (état 2, `LAB_052D`) :

```asm
LAB_052D:
    MOVE.W  #$ffff,LAB_053B          ; invalider adversaire précédent
    MOVEA.L LAB_068C,A0              ; stats joueur
    MOVEA.L LAB_068E,A1              ; stats adversaire
    CMPI.L  #$00000006,LAB_068F      ; état boutique ?
    BEQ.W   LAB_0562
    BTST    #4,D0                    ; hit final ?
    BEQ.W   LAB_053F
    ...
    SUBI.B  #$01,0(A0,D1.W)          ; réduire stats adversaire
    MOVE.W  D1,LAB_053B              ; mémoriser item volé
    CMPI.L  #$00000003,LAB_068F      ; état Mystic ?
    BNE.S   LAB_052E
    MOVE.W  #$009c,D0
    JSR     LAB_0AA2                 ; jouer son de victoire (canal libre)
    MOVE.W  #$0001,LAB_0984          ; fin de combat
    BRA.W   LAB_053A
```

Ensuite :
- Si l'adversaire est un chevalier (`skill > 0`) : transfert d'un seul item
  via `LAB_001C` (écran de butin `program.asm`).
- Si `skill == 0` : transfert total des items via `LAB_0022`.
- Si la créature est dans le pool PvE (`LAB_08C6`) : `LAB_0E03` enregistre
  sa mort (`126(knight) = LAB_065F`).

### 7.5 Mort du chevalier joueur : `LAB_000E`

Si les HP courants du joueur tombent à ≤ 0 :

```asm
LAB_000E:                            ; respawn/mort
    hp_current = hp_max              ; restaurer les HP
    skill -= 1                       ; perdre 1 niveau de skill
    if skill < 0 : game_over
```

`LAB_052F` (`mog.asm#L11548`) gère la **progression de skill** :
si `hp_current == hp_max` après un combat (pleine santé = victoire nette),
le skill est incrémenté de 1 (plafonné à 5).

### 7.6 Marquage de la créature vaincue : `LAB_005F`

Après une victoire PvE, `LAB_005F` (`mog.asm#L948`) vérifie si la créature
doit être marquée comme définitivement vaincue :

```asm
LAB_005F:
    TST.W   8(A0)          ; la créature porte-t-elle encore la clef ?
    BEQ.S   LAB_0060       ; non → vérifier si butin épuisé aussi
    MOVEQ   #1,D0          ; clef toujours présente → ne pas marquer
LAB_0060:
    MOVEA.L 0(A0),A1       ; pointeur vers zone de butin
    MOVE.L  #$00000017,D7  ; 24 octets à vérifier
LAB_0061:
    TST.B   (A1)+          ; encore du butin ?
    BEQ.S   LAB_0062
    MOVEQ   #1,D0          ; butin présent → ne pas marquer
LAB_0062:
    DBF     D7,LAB_0061
    TST.W   D0
    BNE.S   LAB_0063
    MOVE.L  #$ffffffff,10(A0)  ; marquer créature comme vaincue définitivement
```

Une créature est marquée vaincue (`10(A0) = $FFFFFFFF`) uniquement quand
**son butin ET sa clef sont tous les deux épuisés**.

### 7.7 Transition après combat

Après la sortie de `LAB_04D2`, le programme revient dans la logique
de navigation overworld. Si `LAB_053C == 1` (transition vers l'armurier
ou la taverne), le chevalier est téléporté vers l'état correspondant.
Sinon, le jeu reprend la carte overworld depuis la position courante
du chevalier.

---

## 8. Récapitulatif des assets de combat

### 8.1 Assets chargés en permanence pendant le combat

| Fichier       | Type  | Label      | Rôle                                    |
|---------------|-------|------------|-----------------------------------------|
| `ch.piv`      | PIV   | `LAB_07AF` | Fond de combat (320×200, 5 plans)       |
| `message.piv` | PIV   | `LAB_07AE` | Fond écran de messages / butin          |
| `co.stile`    | STILE | —          | Tileset décors de combat (RLE 2 bits)   |
| `bold.f`      | CEL   | `LAB_078B` | Police de caractères bold (textes butin/inventaire) |
| `Small.font`  | font  | `LAB_078C` | Police de caractères petite             |
| `vmusic.cmp`  | CMP   | —          | Module SoundTracker musique de combat   |

### 8.2 Assets chargés selon le type de créature

| Type              | Fichiers CEL/OB chargés                              | Handler     |
|-------------------|------------------------------------------------------|-------------|
| Chevalier ennemi  | `He1.ob`, `He2.ob`, `He3.ob`                        | `LAB_0188`  |
| Mudmen            | `Mudmen1.cel`, `Mudmen2.cel`                        | `LAB_019A`  |
| Balok (boss)      | `Balok1.cel`, `Balok2.cel`, `Balok3.cel`            | `LAB_01A0`  |
| Ratmen            | `Ratmen1.cel`, `Ratmen2.cel`                        | `LAB_0164`  |
| TroggAxe          | `TroggAxe1.cel`, `TroggAxe2.cel`                    | `LAB_0168`  |
| TroggSpear        | `TroggSpear1.cel`, `TroggSpear2.cel`                | `LAB_016A`  |
| Démons            | `Demon1.cel`, `Demon2.cel`, `Demon3.cel`, `Demon4.cel` | `LAB_0175` |
| Ratmen (variante) | `Ratmen1.cel`, `Ratmen2.cel`                        | `LAB_018C`  |
| Dragon            | `Dragon1.cel`, `Dragon2.cel`                        | `LAB_0192`  |
| Troll             | `Troll1.cel`, `Troll2.cel`                          | `LAB_0196`  |
| Sélène/Démon      | `Sel.cel`                                           | `LAB_019E`  |

### 8.3 Fichiers sons `.t` de combat

Voir §6.2. Les fichiers `.t` sont des **samples PCM 8-bit** lus directement
par les registres DMA Paula (`$DFF0A0`…`$DFF0D0`). Chaque table
(`LAB_07B6`…`LAB_07B9`, `LAB_07C0`) contient des pointeurs chargés
en mémoire CHIP pour accès DMA.
