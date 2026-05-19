# Documentation des animations d'introduction et de fin — Moonstone / Amiga 68000

> Document de référence pour le portage C des scènes cinématiques.
> Sources analysées : `amiga_asm/program.asm`, `amiga_asm/mog.asm`.

---

## 1. Vue d'ensemble

Moonstone comporte deux séquences cinématiques gérées entièrement par le binaire `program` :

| Séquence | Routine principale | Déclencheur | Durée approximative |
|---|---|---|---|
| **Introduction** | `LAB_0185` ([program.asm#L3300](program.asm#L3300)) | Lancement en mode 1 joueur (joystick absent au boot) | ~60–90 s |
| **Fin de partie** | `LAB_003B` ([program.asm#L749](program.asm#L749)) | Victoire d'un chevalier avec la Moonstone à Stonehenge | ~30–40 s |

Les deux séquences partagent le même moteur de sprites/entités décrit au §3.

---

## 2. Moteur commun d'animation

### 2.1 Résumé de l'architecture

```
SECSTRT_0 (point d'entrée)
  │
  ├─ mode 1 joueur ──▶ LAB_0185  (intro complète)
  │
  └─ mode 2 joueurs ──▶ LAB_0001 ──▶ LAB_018E (chargement rapide)
                                   ──▶ LAB_0036 / 0037 / 0039 (scènes interactives)
                                   ──▶ LAB_003B (cinématique de fin)
```

### 2.2 Pool d'entités — `LAB_0282`

40 entrées × 42 octets (`$2A`) = **1 680 octets** ([program.asm#L5024](program.asm#L5024)).

| Offset | Taille | Champ | Rôle |
|---|---|---|---|
| 0 | byte | `active` | 1 = entité active |
| 1 | byte | `running` | 1 = script en cours |
| 2 | long | `script_pc` | Pointeur courant dans le bytecode |
| 6 | word | `base_x` | Position X de base |
| 8 | word | `base_y` | Position Y de base |
| 10 | word | `vel_y` | Vélocité Y (gravité) |
| 12 | word | `screen_x` | Position X calculée ce frame |
| 14 | word | `screen_y` | Position Y calculée ce frame |
| 16 | word | `sprite_w` | Largeur du sprite courant |
| 18 | word | `sprite_h` | Hauteur du sprite courant |
| 20 | byte | `frame_param` | Sous-frame actuel |
| 21 | byte | `last_param` | Sous-frame précédent |
| 22 | byte | `direction` | Flags : bit0=flip_x, bit1=flip_y |
| 24 | long | `entity_id` | Identifiant unique |
| 28 | long | `asset_table` | Pointeur vers la table de 8 pointeurs CEL |
| 32 | byte | `script_type` | Index dans `LAB_011B` (dispatch table) |
| 36 | long | `frame_state` | Pointeur vers le bloc `LAB_0284` (état frame) |
| 40 | word | `visible` | 0 = caché |

**Bloc frame_state** (`LAB_0284`, 40 slots × 48 octets = 1 920 octets) :

| Offset | Taille | Champ | Rôle |
|---|---|---|---|
| 0 | byte | `speed` | Compteur vitesse (ticks par frame) |
| 1 | byte | `active` | 1 = frame active |
| 2 | long | `inner_pc` | Pointeur sauvé pour boucle interne |
| 6 | byte | `loop_count` | Compteur pour `LOOP_INIT` |
| 7 | byte | `looping` | Flag boucle |
| 8 | long | `loop_addr` | Adresse de retour boucle |
| 12 | long | `death_addr` | Adresse animation de mort (`$FD`) |
| 16 | byte | `anim_changed` | Flag changement d'animation |
| 20 | long | `next_anim` | Adresse de la prochaine animation |
| 26 | byte | `has_callback` | Flag callback |
| 27 | byte | `callback_timer` | Décompte callback |
| 28 | long | `target_addr` | Cible ou adresse callback |
| 42 | byte | `respawn_flag` | Flag respawn |

### 2.3 Format de bytecode d'animation — instruction 6 octets

L'interpréteur est `LAB_01F1` ([program.asm#L4208](program.asm#L4208)).

Chaque instruction fait exactement **6 octets** :

```
Offset 0 : opcode (1 octet)
  bit7 = 0  →  opcode de dessin (index CEL × 4)
  bit7 = 1  →  opcode de contrôle (valeur & $7F = offset dans LAB_0288)
Offset 1 : param / sous-frame (1 octet)
Offset 2 : x_delta signé (1 octet)
Offset 3 : flags de rendu (1 octet)
  bit4 = 1  →  dessiner sur les deux buffers (double-buffer)
  bit5 = 1  →  utiliser le masque (mask buffer)
Offsets 4-5 : y_pos signé (1 word)
```

**Opcodes de contrôle** (bit7=1) :

| Valeur hex | Nom | Description |
|---|---|---|
| `$FF` | `END_FRAME` | Fin du frame courant → attendre `speed` ticks |
| `$FE` | `LOOP_IDLE` | Retour à l'adresse idle `8(A5)` → boucle infinie |
| `$FD` | `DEATH` | Saut à l'animation de mort `32(A5)` |
| `$80` | `SET_DIR` | Changer le flag direction `22(A1)` |
| `$84` | `COND_JUMP` | Saut conditionnel / changement d'animation |
| `$88` | `SET_SPEED` | Régler le compteur vitesse `0(A5)` |
| `$8C` | `SKIP_8` | Avancer le PC de 8 octets |
| `$94` | `LOOP_INIT` | Init compteur boucle `6(A5)`, sauver adresse `8(A5)` |
| `$A0` | `MOVE_DELTA` | Déplacer l'acteur (delta X/Y avec flags direction) |
| `$A4` | `ADVANCE_4` | Avancer le PC de 4 octets (no-op 4 octets) |
| `$B4` | `CALL_EXT` | Appel fonction externe (4 octets adresse après les 2 octets d'opcode) |
| `$BC` | `SKIP_6_A` | Avancer PC de 6 octets (no-op) |
| `$C0` | `KILL` | Désactiver l'entité (`0(A1)` = 0) |
| `$C4` | `SET_ASSET_TABLE` | Changer la table d'assets `28(A1)` |
| `$CC` | `BRANCH_IF_ZERO` | Branchement si variable nulle |
| `$D0` | `BRANCH_IF_NONZERO` | Branchement si variable non nulle |
| `$D4` | `RESET_FRAME_STATE` | Effacer toute la structure A5 (48 octets) |

**Opcodes de dessin** (bit7=0, valeur = `N×4`) :

Valeur `N×4` → `MOVEA.L 0(A0, N×4), A0` → pointeur vers le CEL N de l'entité.
Valeurs valides : `$00, $04, $08, $0C, $10, $14, $18, $1C` = 8 CEL possibles.

**Note :** l'opcode `$B4` (CALL_EXT) est spécial : il consomme 6 octets supplémentaires pour l'adresse de la fonction. La taille totale de l'instruction est donc **10 octets** (2 + 4 d'adresse + 4 de padding/données).

### 2.4 Boucle principale de rendu — `LAB_0007`

```asm
LAB_0007:          ; boucle infinie de jeu  [program.asm#L195]
    JSR LAB_0017   ; sauver timestamp
    JSR LAB_01E8   ; tick entités (avancer les scripts)
    TST.W LAB_0120 ; flag "quit" ?
    BEQ.S LAB_0008
    RTS
LAB_0008:
    JSR LAB_01EC   ; calculer positions écran / trier sprites
    JSR LAB_0262   ; afficher tous les sprites (IMAGEXCEL)
    JSR LAB_0242   ; sync VBL / copper list
    JSR LAB_000F   ; aiguillage de décor (si changement de scène)
    BSR LAB_0018   ; attendre le nombre de ticks cible
    JMP LAB_0007
```

**Sortie :** `LAB_0014` ([program.asm#L263](program.asm#L263)) positionne `LAB_0120 = 1` pour provoquer la sortie de la boucle.

### 2.5 Spawner d'entité — `LAB_0015`

```asm
LAB_0015:          ; [program.asm#L269]
    ; Paramètres d'entrée :
    ;   A0 = pointeur vers le script bytecode
    ;   A2 = pointeur vers la table de CEL (LAB_0276)
    ;   D0 = base_x, D1 = base_y, D2 = vel_y
    ;   D3 = direction (flip flags), D5 = script_type
    ; Résultat : nouvelle entité dans LAB_0282
    MOVEA.L #LAB_0276, A2
    ...
    JSR LAB_01DA   ; trouver un slot libre, initialiser l'entité
    RTS
```

### 2.6 Format des nœuds de texte

Les textes d'intro et de fin utilisent une **liste chaînée de nœuds** de 12 octets chacun :

```
struct TextNode {
    char  *text_ptr;    /* +0 : pointeur vers chaîne ASCII null-terminée */
    uint32_t y_pos;     /* +4 : position Y sur l'écran (en pixels) */
    uint16_t mode;      /* +8 : $0001 = centré, $0000 = gauche */
    TextNode *next;     /* +10 : pointeur vers le prochain nœud, NULL = fin */
};
```

La routine `LAB_028F` ([program.asm#L5133](program.asm#L5133)) parcourt la liste et appelle `LAB_04B4` pour chaque caractère via la table de glyphes `LAB_00F8` (police `bold.f`).

La routine `LAB_0054` ([program.asm#L1047](program.asm#L1047)) fait la même chose mais copie d'abord le framebuffer, permettant une transition visuelle.

---

## 3. Scène d'introduction (`LAB_0185`)

### 3.1 Point d'entrée

```
SECSTRT_0  (line 108)
  │
  ├─ bit $80 de LAB_0005 = 0  →  LAB_0185 (mode 1 joueur / demo)
  └─ bit $80 de LAB_0005 = 1  →  LAB_0001 (mode 2 joueurs — intro rapide)
```

**Mode 1 joueur** (`LAB_0185`, [program.asm#L3300](program.asm#L3300)) :
Séquence complète de ~8 écrans cinématiques avec chargement progressif.

**Mode 2 joueurs** (`LAB_0001`, [program.asm#L156](program.asm#L156)) :
- Affiche l'écran de chargement rapide (`LAB_00A2` via `LAB_0054`)
- Appelle `LAB_018E` ([program.asm#L3489](program.asm#L3489)) = version abrégée de l'intro
- Lance directement les scènes interactives

### 3.2 Séquence de chargement — `LAB_0185`

La routine charge et affiche les scènes dans cet ordre :

```
1.  Écran logo Mindscape         (LAB_0184 / "mindscape" PIV)
2.  Écran intro : Stonehenge nuit (LAB_0506, fond LAB_00C9 = bg8.PIV)  ← fond 1
3.  Scène bg1a (coucher de soleil)                                       ← fond 2
4.  Scène bg1b (plaine medievale)  + palette LAB_01CE                    ← fond 3
5.  Scène bg1c (forêt)             + palette LAB_01CF                    ← fond 4
6.  Scène bg2  (ville Highwood)    + palette LAB_01D0                    ← fond 5
7.  Scène bg2a (ville Waterdeep)   + palette LAB_01D1                    ← fond 6
8.  Scène bg3  (sanctuaire)        + palette LAB_01D2                    ← fond 7
```

Pour chaque scène, la routine :
1. Copie le buffer source via `LAB_026C` (`MOVEA.L src, D0; JSR LAB_026C`)
2. Décompresse le PIV avec `LAB_0496` (LZSS) → `LAB_0491` (post-traitement)
3. Copie la palette via `LAB_025B`
4. Attend l'appui d'une touche ou un timeout (`LAB_05A1`) ; vérifie `LAB_05E7` pour un skip

Chargement des sprites CEL (dans l'ordre où ils apparaissent dans `SECSTRT_8`) :

| Label | Fichier | CEL index | Frames | Scène |
|---|---|---|---|---|
| `SECSTRT_8+0` | `au1.cel` | 0 | 92 | Intro (toutes scènes) |
| `LAB_0163` | `li1.cel` | 1 | 30 | Scène bg1a |
| `LAB_0164` | `da1.cel` | 2 | 52 | Scène bg1b |
| `LAB_0165` | `dw1.cel` | 3 | 53 | Scène bg1c |
| `LAB_0166` | `ha1.cel` | 4 | 22 | Scène bg2 |
| `LAB_0167` | `bg1.cel` | 5 | — | Fond (décor) |
| `LAB_0168` | `co1.cel` | 6 | 25 | Scène bg2a |
| `LAB_0169` | `dg1.cel` | 7 | 55 | Scène bg3 |
| `LAB_016A` | `Klift1.CEL` | 8 | 55 | Intro |

Chargement musique : `LAB_0183+1` → `LAB_0390` (cache de fichiers) puis `LAB_03B2` + `LAB_03DA` → `LAB_0190` (décompresseur RNC1) pour charger `music.cmp`.

### 3.3 Textes de l'introduction

#### Écran de chargement — `LAB_00A2` ([program.asm#L1669](program.asm#L1669))

Affiché par `LAB_0054` dans le mode 2 joueurs :

```
TextNode chain LAB_00A2 :
  → LAB_00A6 : "The ceremony of the"   y=$4b (75px)
  → LAB_00A7 : "Moonstone"             y=$5f (95px)
  → LAB_00A8 : "is about to begin"     y=$73 (115px)
  → LAB_00A9 : "Loading ..."           y=$b4 (180px) + fin (next=NULL)
```

#### Texte d'introduction des druides — `LAB_00AA` ([program.asm#L1696](program.asm#L1696))

Affiché à la fin de la séquence introductive (ligne 147-148) :

```
TextNode chain LAB_00AA :
  → LAB_00B5 : "The druids sent their"          y=$37 (55px)
  → LAB_00B6 : "best knights to Stonehenge"     y=$4b (75px)
  → LAB_00B7 : "so they may be dubbed"          y=$5f (95px)
  → LAB_00B8 : "into the"                       y=$73 (115px)
  → LAB_00B9 : "Quest for the "                 y=$87 (135px)
  → LAB_00BA : "MOONSTONE"                      y=$af (175px) + fin
```

### 3.4 Animation d'introduction des chevaliers — `LAB_00E3`

Spawné par `LAB_001A` ([program.asm#L305](program.asm#L305)) via `LAB_0015`.

**Description :** 4 chevaliers (sprites index `$10` frame `$1b`) descendent depuis le haut de l'écran vers leur position finale.

```asm
; Bytecode LAB_00E3  [program.asm#L2351]
; 25 positions, y allant de $ffe4 (=−28, hors écran haut) vers $03 (=3)
; chaque instruction : opcode=$10 (CEL 4), frame=$1b, x_delta=$XX, flags=$00, y=$ffe4..$03

  $10 $1b $5f $00 $fff4  ; CEL4 frame27, x=+95, y=-12
  $10 $1b $59 $00 $fff4  ; ...
  ...                    ; 25 frames de descente (pas de 5 pixels en Y)
  $10 $1c $f1 $00 $fff4  ; transition vers frame suivante (accélération)
  ...
  $10 $1f $f0 $00 $ffe7  ; atterrissage
  $10 $20 $f0 $00 $ffe5
  ...
  $88 $08                ; SET_SPEED = 8 ticks/frame (ralentit)
  $10 $23 $f2 $00 $ffe4  ; position finale (boucle terminale)
  $ff $ff                ; fin de séquence
```

**Assets impliqués :**
- CEL slot 4 dans la table d'assets (`$10` = N=4, soit index 4 × 4 = 16 → `0(A0,$10)`)
- Utilisé sur le fond `bg3.PIV` (sanctuaire, palette `LAB_01CD`)

**Spawn depuis `LAB_001A` :**

```asm
LAB_001A:          ; combat_init_p1  [program.asm#L305]
    JSR LAB_01E4   ; reset entity pool
    JSR LAB_0258   ; stop music
    JSR LAB_0262   ; clear screen
    MOVE.L LAB_00CA, LAB_00C6   ; fond = bg3 (sanctuaire)
    JSR LAB_0263   ; blitter le fond
    MOVE.L #LAB_01CD, LAB_011D  ; palette active = sanctuaire
    MOVE.W #$0004, LAB_011E     ; bitplane mode 4
    BSR LAB_0030   ; spawn sprites de l'arène
    BSR LAB_001F   ; lancer la boucle d'animation overworld
    LEA  LAB_00E3, A0
    BSR  LAB_0015  ; spawn animation LAB_00E3 (4 chevaliers descendent)
    JSR  LAB_0007  ; boucle principale (attend la fin)
    MOVE.L LAB_00C7, LAB_00C6
    RTS
```

### 3.5 Ressources de la scène d'introduction

| Type | Fichier | Rôle |
|---|---|---|
| PIV fond | `bg1a.PIV` | Coucher de soleil / paysage |
| PIV fond | `bg1b.PIV` | Plaine médiévale |
| PIV fond | `bg1c.PIV` | Forêt nocturne |
| PIV fond | `bg2.PIV` | Ville (Highwood) |
| PIV fond | `bg2a.PIV` | Ville (Waterdeep) |
| PIV fond | `bg3.PIV` | Sanctuaire des druides |
| PIV fond | `bg4.PIV` | Scène intermédiaire |
| PIV fond | `bg7.PIV` | Arène de combat (intro) |
| PIV fond | `bg8.PIV` | Stonehenge nuit (intro) |
| PIV logo | `mindscape` | Logo éditeur (écran splash) |
| CEL sprite | `au1.cel` | Chevalier bleu (92 frames) |
| CEL sprite | `li1.cel` | Chevalier vert (30 frames) |
| CEL sprite | `da1.cel` | Démon / créature (52 frames) |
| CEL sprite | `dw1.cel` | Nain / autre (53 frames) |
| CEL sprite | `ha1.cel` | Healer / sorcier (22 frames) |
| CEL sprite | `co1.cel` | Logo / titre (25 frames) |
| CEL sprite | `dg1.cel` | Dragon (55 frames) |
| CEL sprite | `Klift1.CEL` | Animation d'arrivée (55 frames) |
| Tileset | `intro.stile` | Tuiles de décor intro |
| Musique | `music.cmp` | Module ProTracker (RNC1) |
| Police | `bold.f` | Police de texte |

### 3.6 Schéma de la séquence d'introduction complète

```
SECSTRT_0
│
├── (1j) ──▶ LAB_0185  ────────────────────────────────────────────────────────────
│           │ Écran Mindscape logo (LAB_0184 / "mindscape")                        │
│           │ Écran bg8 nuit (LAB_0506/LAB_00C9)                                   │
│           │ Scène bg1a  +  palette LAB_01CB  [↵ bouton ou timeout]               │
│           │ Scène bg1b  +  palette LAB_01CE  [↵ bouton ou timeout]               │
│           │ Scène bg1c  +  palette LAB_01CF  [↵ bouton ou timeout]               │
│           │ Scène bg2   +  palette LAB_01D0  [↵ bouton ou timeout]               │
│           │ Scène bg2a  +  palette LAB_01D1  [↵ bouton ou timeout]               │
│           │ Scène bg3   +  palette LAB_01D2  [↵ bouton ou timeout]               │
│           │ Chargement music.cmp (RNC1 → ProTracker)                             │
│           └─ Fin → LAB_05A5 (init jeu) ──────────────────────────────────────────
│
└── (2j) ──▶ LAB_0001  ──▶ LAB_0054 (texte "The ceremony of the Moonstone...")
                       ──▶ LAB_018E  (chargement rapide : bg5+bg8 + palettes)
                       ──▶ LAB_0036  (scène bg2, overworld intro)
                       ──▶ LAB_0037  (scène overworld complète)
                       ──▶ LAB_0039  (rounds de jeu)
                       ──▶ LAB_003B  (cinématique de fin → §4)
```

---

## 4. Scène de fin (`LAB_003B`)

### 4.1 Point d'entrée et condition de déclenchement

`LAB_003B` ([program.asm#L749](program.asm#L749)) est appelée depuis la boucle principale du mode 2 joueurs (ligne 164) une fois que :
1. Tous les rounds interactifs sont terminés
2. Un chevalier a remporté la Moonstone et l'a apportée à Stonehenge pendant la bonne phase de lune

### 4.2 Séquence complète de `LAB_003B`

```asm
LAB_003B:                           ; [program.asm#L749]
    JSR  LAB_0258                   ; 1. Stopper la musique / sprites
    JSR  LAB_01E4                   ; 2. Reset pool d'entités
    MOVE.L LAB_00C7, LAB_00C6       ; 3. Buffer actif = overworld map (LAB_00C7)
    JSR  LAB_05AB                   ; 4. Setup overworld final (voir §4.3)
    MOVE.L #LAB_01CB, LAB_011D      ; 5. Palette active = joueur 1 (LAB_01CB)
    JSR  LAB_0010                   ; 6. Chargement palette (mode 4)
    MOVE.L #$0000000f, D0
    JSR  LAB_054F                   ; 7. Attendre 15 frames
    MOVE.W #$0000, LAB_0120         ; 8. Réinitialiser le flag de sortie
    MOVE.W #$0000, LAB_011F         ; 9. Réinitialiser état affichage
    MOVE.W #$0004, LAB_011E         ; 10. Mode 4 bitplanes
    MOVE.L #$00000000, LAB_0123     ; 11. Timer de scène = 0
    MOVE.L #$00000006, LAB_00D0     ; 12. Ticks/frame = 6
    MOVEA.L #LAB_00EE, A0
    JSR  LAB_0015                   ; 13. Spawn animation Stonehenge (LAB_00EE)
    JSR  LAB_05AC                   ; 14. Attendre fin animation (poll LAB_05E6)
    JSR  LAB_0007                   ; 15. Boucle principale (run jusqu'à sortie)
    JSR  LAB_025F                   ; 16. Fade out palette
    JSR  LAB_01E4                   ; 17. Reset pool d'entités
    MOVE.L LAB_00C9, LAB_00C6       ; 18. Buffer = overworld (LAB_00C9)
    JSR  LAB_0263                   ; 19. Blitter le fond final
    LEA  LAB_01CC, A0
    JSR  LAB_0260                   ; 20. Charger la palette de fin (LAB_01CC)
    MOVEQ #100, D0
    JSR  LAB_054F                   ; 21. Attendre 100 frames (~2 s)
    MOVE.L LAB_056C, D0
    JSR  LAB_026C                   ; 22. Charger le buffer de victoire (vmusic)
    LEA  LAB_00B0, A0
    JSR  LAB_028F                   ; 23. Afficher le texte de fin (voir §4.5)
    JSR  LAB_0262                   ; 24. Afficher tous les sprites
    MOVE.L #$000001f4, D0
    JSR  LAB_054F                   ; 25. Attendre 500 frames (~10 s)
    LEA  LAB_026D, A0
    MOVEQ #6, D0
    JSR  LAB_0576                   ; 26. Fade palette en 6 étapes (vmusic.cmp)
    MOVEQ #90, D0
    JSR  LAB_054F                   ; 27. Attendre 90 frames (~1.8 s)
    RTS                             ; 28. Retour → retour au menu principal
```

### 4.3 Initialisation de l'overworld final — `LAB_05AB`

```asm
LAB_05AB:           ; [program.asm#L10922]
    JSR  LAB_024B   ; sauvegarder état palette
    JSR  LAB_0262   ; vider l'écran
    MOVEA.L SECSTRT_30, A0
    JSR  LAB_054D   ; copier le framebuffer principal
    MOVE.L LAB_00C6, LAB_05D7   ; sauver le buffer actif
    LEA  LAB_05D6, A0
    MOVE.L LAB_00C8, (A0)+      ; sauver buffers : C8 (overworld 1)
    MOVE.L LAB_00C9, (A0)+      ; C9 (overworld 2)
    MOVE.L LAB_00CA, (A0)+      ; CA (overworld 3)
    MOVE.L #$00000002, LAB_00D0  ; ticks/frame = 2 (transition rapide)
    MOVE.L #$00000000, LAB_0123  ; timer = 0
    MOVE.W #$03e8, LAB_05B8     ; compteur principal = 1000
    MOVE.W #$0002, LAB_05B8+2   ; step initial = 2
    MOVE.W #$0000, LAB_05BD
    MOVE.W #$0008, LAB_05BC     ; 8 lignes d'overworld à afficher
    MOVE.W #$0000, LAB_05E6     ; flag fin = 0
    MOVE.W #$0009, LAB_05B8+2   ; 9 phases de la cinématique
    JSR  LAB_05BA               ; init affichage overworld
    JSR  LAB_05B7               ; dessiner l'overworld
    JSR  LAB_0262               ; flush
    RTS
```

### 4.4 Animation Stonehenge — `LAB_00EE`

`LAB_00EE` ([program.asm#L2545](program.asm#L2545)) est un script d'animation en 9 phases représentant la construction progressive du rituel de Stonehenge. Chaque phase ajoute des pierres (sprites) jusqu'à la formation complète.

**Mécanisme de phases :**
- `LAB_05AF` ([program.asm#L10968](program.asm#L10968)) : décrémente le compteur de phases `LAB_05B8+2`
- `LAB_05AE` ([program.asm#L10965](program.asm#L10965)) : positionne `LAB_05E6 = 1` → signal de fin pour `LAB_05AC`
- `LAB_05AC` ([program.asm#L10944](program.asm#L10944)) : boucle en attendant `LAB_05E6 = 1`

**Structure des phases :**

```
LAB_00EE:
  ; Phase 1 : 1 pierre
  $B4 $00  DC.L LAB_05AF       ; CALL_EXT → décrémente compteur de phases
  $08 $00 $1d $00 $ff $ef      ; CEL $08 frame 0, x=+29, y=-17
  $ff $00                      ; END_FRAME

  ; Phase 2 : 4 pierres
  $B4 $00  DC.L LAB_05AF
  $08 $04 $19 $00 $ff $ee      ; CEL $08 frame 4, x=+25, y=-18
  $08 $02 $06 $00 $ff $e3      ; CEL $08 frame 2, x=+6,  y=-29
  $08 $01 $fb $00 $ff $e6      ; CEL $08 frame 1, x=-5,  y=-26
  $08 $03 $0d $00 $00 $02      ; CEL $08 frame 3, x=+13, y=+2
  $ff $00

  ; Phase 3 : 5 pierres
  $B4 $00  DC.L LAB_05AF
  $08 $07 $f7 $00 $ff $f0
  $08 $06 $f7 $00 $ff $de
  $08 $05 $eb $00 $ff $e3
  $08 $08 $18 $00 $ff $e9
  $ff $00

  ; Phase 4 : 5 pierres supplémentaires
  $B4 $00  DC.L LAB_05AF
  $08 $0c $0b $00 $ff $f2
  $08 $0b $20 $00 $ff $e7
  $08 $0a $f5 $00 $ff $da
  $08 $09 $da $00 $ff $da
  $08 $0e $33 $00 $00 $0e
  $ff $00

  ; Phase 5 : 10 pierres
  $B4 $00  DC.L LAB_05AF
  $08 $0f $32 $00 $00 $10
  $08 $0f $c8 $00 $ff $e5
  $08 $0f $d5 $00 $ff $d9
  $08 $0f $e8 $00 $ff $cf
  $08 $0f $e9 $00 $ff $e5
  $08 $0f $df $00 $ff $f6
  $08 $0f $ef $00 $00 $01
  $08 $0f $f4 $00 $00 $0f
  $08 $0f $fc $00 $00 $0d
  $08 $0f $05 $00 $ff $fd
  $08 $0f $03 $00 $ff $f6
  $08 $0f $05 $00 $ff $ee
  $08 $0f $1e $00 $ff $e9
  $08 $0f $1c $00 $00 $07
  $08 $0f $34 $00 $ff $dd
  $ff $00

  ; Phases 6, 7, 8 : variantes de la formation complète
  ; (mêmes sprites, positions légèrement décalées → effet de "vibration" / glow)
  $B4 $00  DC.L LAB_05AF
  ... (structure similaire) ...
  $ff $00

  ; Phase 9 (dernière) :
  $88 $28   ; SET_SPEED = 40 ticks/frame (ralentissement dramatique)
  ... (tous les sprites Stonehenge) ...
  $ff $00

  ; Épilogue : appel LAB_05AE (flag fin)
  $B4 $00  DC.L LAB_05AE       ; CALL_EXT → positionne LAB_05E6 = 1
  ... (dernière pose) ...
  $ff $ff                      ; fin de séquence (KILL / $FF $FF)
```

**Table des sprites Stonehenge** (CEL `$08`, frames `$00...$18`) :

| Frame | Description probable |
|---|---|
| `$00` | Pierre debout seule (grande) |
| `$01` | Pierre debout (variante gauche) |
| `$02` | Pierre debout (variante droite) |
| `$03` | Pierre inclinée |
| `$04` | Pierre linteau (horizontal) |
| `$05`..`$08` | Supports/arches secondaires |
| `$09`..`$0E` | Pierres intérieures (cercle central) |
| `$0F`..`$14` | Formation extérieure complète |
| `$15`..`$18` | Effets lumineux / glow de la Moonstone |

### 4.5 Texte de fin — `LAB_00B0`

```
TextNode chain LAB_00B0 :  [program.asm#L1725]
  → LAB_00BB : "And so, the tale of the"         y=$37 (55px)
  → LAB_00BC : "Moonstone and the courage"        y=$4b (75px)
  → LAB_00BD : "of the knights that fought"       y=$5f (95px)
  → LAB_00BE : "for it is passed on from"         y=$73 (115px)
  → LAB_00BF : "one generation to the next."      y=$87 (135px) + fin
```

Affiché sur le fond `LAB_00C9` (overworld final) avec la palette `LAB_01CC` (joueur 2 / palette dorée).

### 4.6 Ressources de la scène de fin

| Type | Fichier / Label | Rôle |
|---|---|---|
| Mémoire tampon | `LAB_00C7` → `LAB_00C9` | Buffers de l'overworld (overworld final) |
| Palette | `LAB_01CB` | Palette joueur 1 (chargée depuis `bg3.PIV`) |
| Palette | `LAB_01CC` | Palette joueur 2 / fin (fond de résolution) |
| Palette fondu | `LAB_026D` | Palette cible pour le fondu final (6 étapes) |
| CEL sprite | Slot `$08` (CEL index 2) | Sprites de Stonehenge (19 frames `$00..$18`) |
| Musique | `vmusic.cmp` | Module ProTracker victoire (RNC1) |
| Police | `bold.f` | Texte de fin |

### 4.7 Variables d'état de la cinématique de fin

| Variable | Valeur initiale | Rôle |
|---|---|---|
| `LAB_05B8` | `$03E8` (1000) | Compteur principal overworld |
| `LAB_05B8+2` | `$0009` (9) | Nombre de phases restantes (décrémenté par `LAB_05AF`) |
| `LAB_05BC` | `$0008` (8) | Nombre de lignes overworld à afficher |
| `LAB_05BD` | `$0000` | Offset courant d'affichage |
| `LAB_05E6` | `$0000` | Flag fin cinématique (mis à 1 par `LAB_05AE`) |
| `LAB_05D6..+8` | — | Sauvegarde des 3 pointeurs de buffer overworld |
| `LAB_05D7` | — | Buffer actif sauvegardé |

---

## 5. Structures C proposées

### 5.1 Instruction de bytecode

```c
/* Format fixe 6 octets pour les opcodes de dessin (bit7=0) */
typedef struct {
    uint8_t  cel_slot;     /* index CEL × 4 (0, 4, 8, …, 28) */
    uint8_t  frame_idx;    /* sous-frame dans le fichier CEL */
    int8_t   x_delta;      /* décalage X signé */
    uint8_t  flags;        /* bit4=double_buf, bit5=mask */
    int16_t  y_pos;        /* position Y signée */
} DrawInstr;               /* 6 octets */

/* Opcode de contrôle générique */
typedef struct {
    uint8_t  opcode;       /* $80..$FF — opcode de contrôle */
    uint8_t  param;        /* paramètre de l'opcode */
    uint8_t  data[4];      /* données additionnelles (selon opcode) */
} CtrlInstr;               /* 6 octets */

/* Opcode CALL_EXT ($B4) — consomme 10 octets au total */
typedef struct {
    uint8_t  opcode;       /* $B4 */
    uint8_t  param;        /* $00 */
    void     (*fn)(void);  /* pointeur de fonction (4 octets, big-endian) */
    uint8_t  _pad[4];      /* suite de la prochaine instruction */
} CallExtInstr;            /* 10 octets (chevauchement avec l'instruction suivante) */
```

### 5.2 Entité d'animation

```c
#define ENTITY_COUNT 40
#define CEL_SLOTS     8

typedef struct {
    uint8_t   active;          /* +0  : 1 = active */
    uint8_t   running;         /* +1  : 1 = script en cours */
    uint8_t  *script_pc;       /* +2  : pointeur courant dans le bytecode */
    uint16_t  base_x;          /* +6  : position X de base */
    uint16_t  base_y;          /* +8  : position Y de base */
    int16_t   vel_y;           /* +10 : vélocité Y */
    int16_t   screen_x;        /* +12 : position X écran */
    int16_t   screen_y;        /* +14 : position Y écran */
    uint16_t  sprite_w;        /* +16 : largeur sprite courant */
    uint16_t  sprite_h;        /* +18 : hauteur sprite courant */
    uint8_t   frame_param;     /* +20 : index sous-frame actuel */
    uint8_t   last_param;      /* +21 : index sous-frame précédent */
    uint8_t   direction;       /* +22 : flip flags (bit0=flip_x) */
    uint8_t   _pad23;
    uint32_t  entity_id;       /* +24 : identifiant unique */
    void     *asset_table[CEL_SLOTS]; /* +28 : pointeurs vers les CEL */
    uint8_t   script_type;     /* +32 : index dispatch table LAB_011B */
    uint8_t   _pad33[3];
    void     *frame_state;     /* +36 : pointeur vers FrameState */
    uint16_t  visible;         /* +40 : 0 = caché */
    uint8_t   _pad42[4];
} Entity;                      /* 42 octets alignés */

typedef struct {
    uint8_t   speed;           /* +0  : ticks/frame */
    uint8_t   active;          /* +1  : frame active */
    uint8_t  *inner_pc;        /* +2  : PC sauvé pour boucle interne */
    uint8_t   loop_count;      /* +6  : compteur LOOP_INIT */
    uint8_t   looping;         /* +7  : flag boucle */
    uint8_t  *loop_addr;       /* +8  : adresse retour boucle */
    uint8_t  *death_addr;      /* +12 : adresse animation mort */
    uint8_t   anim_changed;    /* +16 : flag changement d'anim */
    uint8_t   _pad17[3];
    uint8_t  *next_anim;       /* +20 : prochaine animation */
    uint8_t   has_callback;    /* +26 : flag callback */
    uint8_t   callback_timer;  /* +27 : décompte callback */
    uint8_t  *target_addr;     /* +28 : cible ou adresse callback */
    uint8_t   respawn_flag;    /* +42 : flag respawn */
    uint8_t   _padN[5];
} FrameState;                  /* 48 octets */
```

### 5.3 Nœud de texte

```c
typedef struct TextNode {
    const char    *text;       /* chaîne ASCII null-terminée */
    uint32_t       y_pos;      /* position Y en pixels */
    uint16_t       mode;       /* $0001=centré, $0000=gauche */
    struct TextNode *next;     /* prochain nœud (NULL = fin) */
} TextNode;                    /* 12 octets */
```

### 5.4 Contexte de cinématique de fin

```c
typedef struct {
    uint16_t main_counter;     /* LAB_05B8    : compteur principal (1000→0) */
    uint16_t phase_step;       /* LAB_05B8+2  : phases restantes (9→0) */
    uint16_t display_rows;     /* LAB_05BC    : lignes overworld affichées */
    uint16_t row_offset;       /* LAB_05BD    : offset courant */
    uint16_t end_flag;         /* LAB_05E6    : 0=en cours, 1=terminé */
    void     *saved_buffers[3];/* LAB_05D6..8 : buffers overworld sauvegardés */
    void     *saved_active;    /* LAB_05D7    : buffer actif sauvegardé */
} EndingCinematicState;
```

---

## 6. Résumé des flux d'exécution

### 6.1 Flux intro (mode 1 joueur)

```
SECSTRT_0
  ↓
LAB_038F   (init cache fichiers)
LAB_SECSTRT_29  (config hardware Amiga : bitplanes, copper, DMA)
LAB_0006   (init renderer IMAGEXCEL)
LAB_0044   (init audio)
LAB_SECSTRT_10  (init opcode dispatch table LAB_0288)
LAB_0051   (init entité 0 → script LAB_0014 = flag stop)
  ↓
LAB_025F   (fade to black)
LAB_0185   (intro complète)
  ↓
  → pour chaque scène bg1..bg8 :
       LAB_026C (load buffer source)
       LAB_0402 (décompresser PIV → destination)
       LAB_025B (copier palette)
       LAB_05A1 (attendre bouton/timeout, skip possible via LAB_05E7)
  → LAB_0390 (charger music.cmp)
  → LAB_03B2 / LAB_03DA / LAB_0190 (décompresser RNC1 → ProTracker)
  ↓
LAB_05A5   (init état jeu)
LAB_001B   (combat init p1)
LAB_001C   (combat init p2)
LAB_001A   (spawn chevaliers → LAB_00E3 : descente)
              → LAB_0007 (boucle animation)
LAB_002C / 002D / 002F / 002E  (boutiques, rencontres, sanctuaire)
LAB_0054 (LAB_00AA) : texte "The druids sent their best knights..."
```

### 6.2 Flux fin de partie

```
LAB_003B
  ↓
LAB_01E4   (reset pool entités)
LAB_05AB   (overworld setup final)
  ↓  (LAB_0015 avec LAB_00EE)
Spawn LAB_00EE
  ↓
LAB_05AC   (boucle d'attente)
  ↓  pour chaque frame :
       LAB_01E8  (tick scripts)
            → chaque phase : CALL_EXT → LAB_05AF (décrément phases)
       LAB_01EC  (rendu sprites)
       LAB_0262  (flush écran)
  ↓  (quand LAB_00EE appelle LAB_05AE → LAB_05E6=1)
LAB_05AD   (restaure buffers overworld)
  ↓
LAB_0007   (boucle finale — attend exit)
LAB_025F   (fade out)
→ Transition vers fond LAB_00C9 + palette LAB_01CC
LAB_028F (LAB_00B0) : texte "And so, the tale of the Moonstone..."
  ↓
Attente 500 frames
LAB_0576   (fade palette LAB_026D en 6 étapes + démarrage vmusic.cmp)
Attente 90 frames
  ↓
RTS → retour au menu principal (LAB_0000 → SECSTRT_4)
```

---

*Document généré par analyse statique de `program.asm` (11 319 lignes).
Références principales : `LAB_0185` (intro), `LAB_003B` (fin), `LAB_00E3`/`LAB_00EE` (scripts d'animation), `LAB_01F1` (interpréteur bytecode).*
