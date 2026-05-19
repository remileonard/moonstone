# Documentation des animations d'introduction et de fin — Moonstone / Amiga 68000

> Document de référence pour le portage C des scènes cinématiques.
> Sources analysées : `amiga_asm/program.asm`, `amiga_asm/mog.asm`.

---

## 1. Vue d'ensemble

Moonstone comporte deux séquences cinématiques gérées entièrement par le binaire `program` :

| Séquence | Routine principale | Déclencheur | Durée approximative |
|---|---|---|---|
| **Introduction** | `LAB_0185` ([program.asm#L3300](program.asm#L3300)) | Lancement du programme (avant le menu de sélection) | ~60–90 s |
| **Fin de partie** | `LAB_003B` ([program.asm#L749](program.asm#L749)) | Victoire d'un chevalier avec la Moonstone à Stonehenge | ~30–40 s |

Les deux séquences partagent le même moteur de sprites/entités décrit au §3.

---

## 2. Moteur commun d'animation

### 2.1 Résumé de l'architecture

```
SECSTRT_0 (point d'entrée)
  │
  └─ LAB_025F  ──▶ LAB_0185  (séquence d'introduction complète)
                     │
                     └─ SECSTRT_4 (menu de sélection)
                          │
                          └─ ... (jeu) ... ──▶ LAB_003B (cinématique de fin)
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

**Opcodes de contrôle** (bit7=1) — voir table complète en §7.1 :

| Valeur hex | Nom | Description |
|---|---|---|
| `$FF` | `END_FRAME` / `END_ANIM` | Fin du frame courant (param $00) ou fin d'animation (param $FF) |
| `$FE` | `LOOP_IDLE` | Retour à l'adresse idle `8(A5)` → boucle infinie |
| `$FD` | `DEATH` | Saut à l'animation de mort `32(A5)` |
| `$80` | `SET_DIR` | Changer le flag direction `22(A1)` |
| `$84` | `COND_JUMP` | Saut conditionnel / changement d'animation |
| `$88` | `SET_SPEED` | Régler le compteur vitesse `0(A5)`, stocker loop_addr |
| `$8C` | `SKIP_8` | Avancer le PC de 8 octets |
| `$94` | `LOOP_INIT` | Init compteur boucle `6(A5)`, sauver adresse `8(A5)` |
| `$A0` | `MOVE_DELTA` | Déplacer l'acteur (delta X/Y avec flags direction) |
| `$A4` | `ADVANCE_4` | Avancer le PC de 4 octets |
| `$B4` | `CALL_EXT` | Appel fonction externe (6 octets total : op+param+4-byte addr) |
| `$C0` | `KILL` | Désactiver l'entité (`0(A1)` = 0) |
| `$C4` | `SET_ASSET_TABLE` | Changer la table d'assets `28(A1)` |
| `$CC` | `BRANCH_IF_ZERO` | Branchement si variable nulle |
| `$D0` | `BRANCH_IF_NONZERO` | Branchement si variable non nulle |

**Opcodes de dessin** (bit7=0, valeur = `N×4`) :

Valeur `N×4` → `MOVEA.L 0(A0, N×4), A0` → pointeur vers le CEL N de l'entité.
Valeurs valides : `$00, $04, $08, $0C, $10, $14, $18, $1C` = 8 CEL possibles.

**Note :** L'opcode `$B4` (CALL_EXT) fait exactement **6 octets** : `$B4 param [4 octets adresse]`. L'adresse de la fonction cible occupe les octets 2–5.

**Table complète des opcodes de contrôle** (taille en octets = nombre d'octets consommés dans le PC) :

| Valeur hex | Taille | Nom | Description détaillée |
|---|---|---|---|
| `$FF $00` | 2 | `END_FRAME` | Fin du frame courant ; attendre `speed` ticks VBL puis avancer PC+2 |
| `$FF $FF` | 2 | `END_ANIM` | Désactiver l'entité (entity.running=0) |
| `$FF $FE` | 2 | `END_FRAME/LOOP_BACK` | Décrémenter loop_count ; si >0 → PC=loop_addr ; sinon PC+2 |
| `$FE xx` | 2 | `LOOP_IDLE` | PC = FrameState.loop_addr (saut immédiat, sans décompte) |
| `$FD xx` | 2 | `DEATH` | PC = FrameState.death_addr |
| `$80` | 2 | `SET_DIR` | entity.direction = param (ou EORI #2 si param=$FF) |
| `$84` | 6 | `SET_NEXT_ANIM` | Si param=3 → PC=addr ; sinon FrameState.next_anim=addr, anim_changed=1 |
| `$88` | 2 | `SET_SPEED` | FrameState.speed=param ; FrameState.loop_addr=PC+2 ; active=1 |
| `$8C` | 8 | `SKIP_8` | PC += 8 (saute 8 octets) |
| `$94` | 2 | `LOOP_INIT` | FrameState.loop_count=param ; FrameState.looping=1 ; loop_addr=PC+2 |
| `$98` | 2 | `NOP_98` | No-op (placeholder, ne modifie pas PC → ne pas utiliser) |
| `$9C` | 2 | `NOP_9C` | No-op (idem) |
| `$A0` | 8 | `MOVE_DELTA` | Modifier base_x/base_y/vel_y selon flags et deltas (Δx, Δy, Δv) |
| `$A4` | 4 | `ADVANCE_4` | PC += 4 (paramètre ignoré) |
| `$A8` | 6 | `UNK_A8` | Handler LAB_0241 (non documenté) |
| `$AC` | 2 | `NOP_AC` | No-op |
| `$B0` | 2 | `NOP_B0` | No-op |
| `$B4` | 6 | `CALL_EXT` | Appeler fonction à addr(bytes 2–5). Si param=0 : appel direct avec registres entité |
| `$B8` | 6 | `SKIP_6A` | PC += 6 |
| `$BC` | 6 | `SKIP_6B` | PC += 6 |
| `$C0` | 2 | `KILL` | entity.active=0 (cache sans désactiver) |
| `$C4` | 2 | `SET_ASSET_TABLE` | entity.asset_table = LAB_0281[param-1] |
| `$C8` | 6 | `SKIP_6C` | PC += 6 |
| `$CC` | 8 | `BRANCH_IF_ZERO` | Si *(entity_id + offset) == 0 → PC=addr ; sinon PC+=8 |
| `$D0` | 8 | `BRANCH_IF_NONZERO` | Si *(entity_id + offset) != 0 → PC=addr ; sinon PC+=8 |
| `$D4` | 6 | `UNK_D4` | Handler LAB_023F (non documenté) |

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

## 3. Scène d'introduction

L'introduction se déroule en deux grandes phases :

1. **Phase de chargement et d'affichage** (`LAB_0185`) : charge progressivement les assets depuis le disque et affiche successivement le logo, la lune et les écrans de crédits. Pendant l'affichage de chaque crédit, les backgrounds des scènes cinématiques suivantes sont chargés en arrière-plan.
2. **Phase cinématique** (SECSTRT_0, après LAB_0185) : enchaîne 7 routines d'animation (`LAB_05A5`, `LAB_001B`, `LAB_001C`, `LAB_001A`, `LAB_002C`, `LAB_002D`, `LAB_002F`, `LAB_002E`) qui constituent les plans 5 à 18 du film d'introduction.

### 3.1 Point d'entrée

```
SECSTRT_0  [program.asm#L108]
  │
  JSR LAB_025F          →  fondu au noir
  JSR LAB_0185          →  chargement + logo + lune + crédits + musique
  JSR LAB_05A5          →  scroll cinématique (défilé vers la forêt)
  JSR LAB_001B          →  plan 5a : continuation du défilement
  JSR LAB_001C          →  plan 6  : druides marchant
  JSR LAB_0174          →  transition de palettes
  JSR LAB_001A          →  plan 7  : druides vers Stonehenge
  JSR LAB_002C          →  plans 8-9 : vue dessus + druides en cercle
  JSR LAB_002D          →  plans 10-12 : plongée + contre-plongée + éclair
  JSR LAB_002F          →  plans 13-15 : chevalier + main + entrée dans le cercle
  JSR LAB_002E          →  plans 16-17 : druide + chevalier agenouillé
  JSR LAB_025F          →  fondu au noir
  LEA LAB_00AA, A0
  JSR LAB_0054          →  plan 18 : carton texte de la quête
  JSR LAB_054F (420 fr) →  attente
  JSR LAB_025F          →  fondu au noir
  JSR LAB_005B          →  stop musique
  → SECSTRT_4 (menu de sélection)
```

### 3.2 Séquence de chargement — `LAB_0185` [program.asm#L3300]

#### Mécanisme de décompression PIV

`LAB_026C(D0=dest)` enregistre les 5 pointeurs de bitplanes dans `LAB_04D9..DD`
(chaque plan fait 0x1F40 = 8000 octets, soit 320×200/8 × 5 plans = 40 000 octets).
`LAB_0402(A0=nom_fichier, A1=buf_compressé)` lit le fichier dans `A1`, puis appelle
`LAB_049C` (LZSS) avec destination = `LAB_04D9` (positionné par le `LAB_026C` précédent).
**Un seul appel `LAB_026C` suffit à fixer la destination pour le `LAB_0402` suivant.**

#### Ordre de chargement et affectation des buffers

| Étape | Routine | Fichier → Buffer | Affichage à cet instant |
|---|---|---|---|
| 1 | `LAB_026C(SECSTRT_30)` + `LAB_0402(LAB_0184)` | `mindscape` → **SECSTRT_30** | Palette `LAB_0506` → **logo Mindscape visible** |
| 2 | `LAB_026C(LAB_00C8)` + `LAB_0402(LAB_017E)` | `bg1a.piv` → **LAB_00C8** | Palette `LAB_01CB` ; charge `intro.stile` ; `LAB_059E` initialise le défilement |
| 3 | `LAB_026C(LAB_00C9)` + `LAB_0402(LAB_0180+1)` | `bg1c.piv` → **LAB_00C9** | `LAB_059F` → **affiche la lune** sur fond noir |
| 4 | `LAB_026C(LAB_00CA)` + `LAB_0402(LAB_0181)` | `bg1b.piv` → **LAB_00CA** | `LAB_05A0` → flash blanc puis fondu au noir |
| 5 | `LAB_026C(LAB_00CB)` + `LAB_0402(LAB_016B+1)` | `bg4.piv` → **LAB_00CB** | Affichage : **crédit 0** (`LAB_00FD`) sur fond lune — bg4.piv est chargé en mémoire pour la cinématique, pas affiché |
| 6 | `LAB_026C(LAB_00CC)` + `LAB_0402(LAB_016C+1)` | `bg5a.piv` → **LAB_00CC** | Affichage : **crédit 1** (`LAB_00FF`) sur fond lune — bg5a.piv chargé en mémoire |
| 7 | `LAB_026C(LAB_00CD)` + `LAB_0402(LAB_016D)` | `bg3.piv` → **LAB_00CD** | Affichage : **crédit 2** (`LAB_0102`) sur fond lune — bg3.piv chargé en mémoire |
| 8 | `LAB_026C(LAB_00CE)` + `LAB_0402(LAB_016F+1)` | `bg2.piv` → **LAB_00CE** | Affichage : **crédit 3** (`LAB_0105`) sur fond lune — bg2.piv chargé en mémoire |
| 9 | `LAB_026C(LAB_00CF)` + `LAB_0402(LAB_016E)` | `bg2a.piv` → **LAB_00CF** | Affichage : **crédit 4** (`LAB_010A`) sur fond lune — bg2a.piv chargé en mémoire |
| 10 | Chargement `au1.cel` → zone `LAB_00C2` | — | Affichage : **crédit 5** (`LAB_0107`) sur fond lune pendant le chargement CEL |
| 11 | Chargement `li1.cel`, `da1.cel`, `dw1.cel`, `ha1.cel`, `bg1.cel` | — | Chargements en chaîne (fond lune toujours affiché) |
| 12 | `LAB_0183+1` → `music.cmp` → `LAB_0124` | Musique ProTracker (RNC1) | `BRA.W LAB_0190` → démarre la lecture musicale |

**Note importante :** Durant les étapes 5 à 11, le fond visible à l'écran est **uniquement le fond lune** (`bg1c.piv` / `LAB_00C9`). Les PIVs chargés aux étapes 5-9 (bg4, bg5a, bg3, bg2, bg2a) sont préchargés en mémoire pour être utilisés ultérieurement par les scènes cinématiques ; ils ne sont pas affichés lors des crédits.

#### Récapitulatif des buffers bitmap (40 Ko chacun)

| Buffer | Contenu PIV | Palette associée | Utilisation réelle |
|---|---|---|---|
| `SECSTRT_30` | `mindscape` | `LAB_0506` | Logo éditeur (plan 1) |
| `LAB_00C8` | `bg1a.piv` | `LAB_01CB` | Scroll ciel/crépuscule (plan 5) + fond plan 8 (LAB_002C ph1) |
| `LAB_00C9` | `bg1c.piv` | `LAB_01CC` | **Fond lune + crédits (plans 2-4)** + fond plan 9 (LAB_002C ph2) |
| `LAB_00CA` | `bg1b.piv` | `LAB_01CD` | Fond plans 7 et 13-15 (LAB_001A, LAB_002F) |
| `LAB_00CB` | `bg4.piv` | `LAB_01CE` | Préchargé pendant crédit 0 — fond cinématique ultérieur |
| `LAB_00CC` | `bg5a.piv` | `LAB_01CF` | Préchargé pendant crédit 1 — fond plans 16-17 (LAB_002E) |
| `LAB_00CD` | `bg3.piv` | `LAB_01D0` | Préchargé pendant crédit 2 — fond cinématique ultérieur |
| `LAB_00CE` | `bg2.piv` | `LAB_01D1` | Préchargé pendant crédit 3 — fond plan 6 (LAB_001C druides marchant) |
| `LAB_00CF` | `bg2a.piv` | `LAB_01D2` | Préchargé pendant crédit 4 — fond plans 10-12 (LAB_002D éclairs) |

### 3.3 Écrans de crédits — `LAB_05A1` / `LAB_05B1`

`LAB_05A1` ([program.asm#L10839](program.asm#L10839)) incrémente le compteur `LAB_05B0` (0→6)
et affiche le nœud de texte indexé dans la table `LAB_05B1`. Au-delà de 6 appels,
seul le fondu est effectué (aucun texte). Les textes sont superposés en incrustation sur
le **fond lune** (`LAB_00C9` / `bg1c.piv`) — qui reste le seul fond affiché durant toute
la phase de crédits. Les textes sont rendus via `LAB_028F`.

```
Table LAB_05B1 :
  index 0 : LAB_00FD  →  "created by"       / "Rob Anderson"
  index 1 : LAB_00FF  →  "Programmed by"    / "Rob Anderson" / "Kevin Hoare"
  index 2 : LAB_0102  →  "Artwork by"       / "Rob Anderson" / "Dennis Turner"
  index 3 : LAB_0105  →  "Music and Sound by" / "Richard Joseph"
  index 4 : LAB_010A  →  "Additional Art by" / "Steve Leney"
  index 5 : LAB_0107  →  "Design by"        / "Rob Anderson" / "Todd Prescott"
```

Chaînes de texte embarquées (data section, label `LAB_010C`) :
- `LAB_010C` : "MOONSTONE\0", "Mindscape\0", "presents\0"
- `LAB_010D` : "created by\0"
- `LAB_010E` : "Design by\0"
- `LAB_010F` : "Artwork by\0"
- `LAB_0110` : "Programmed by\0"
- `LAB_0111` : "Music and Sound by\0"
- `LAB_0112` : "Additional Art by\0"
- `LAB_0113` : "Rob Anderson\0"
- `LAB_0114` : "Todd Prescott\0"
- `LAB_0115` : "Dennis Turner\0"
- `LAB_0116` : "Richard Joseph\0"
- `LAB_0117` : "Kevin Hoare\0"
- `LAB_0118` : "Steve Leney\0"

#### Texte d'introduction des druides — `LAB_00AA` ([program.asm#L1696](program.asm#L1696))

Affiché via `LAB_0054` à la fin de la cinématique (plan 18) :

```
TextNode chain LAB_00AA :
  → LAB_00B5 : "The druids sent their"          y=$37 (55px)
  → LAB_00B6 : "best knights to Stonehenge"     y=$4b (75px)
  → LAB_00B7 : "so they may be dubbed"          y=$5f (95px)
  → LAB_00B8 : "into the"                       y=$73 (115px)
  → LAB_00B9 : "Quest for the "                 y=$87 (135px)
  → LAB_00BA : "MOONSTONE"                      y=$af (175px) + fin
```

### 3.4 Routines de transition et d'affichage spéciales

#### `LAB_059E` — Initialisation du défilement 3 buffers

Appelée après le chargement de `bg1a.piv`. Initialise le système de défilement vertical
avec les trois buffers `LAB_00C8` (bg1a = ciel/crépuscule), `LAB_00C9` (bg1c = forêt nocturne),
`LAB_00CA` (bg1b = plaine médiévale). Les pointeurs sont mémorisés dans `LAB_05D6..+8`.
Paramètres initiaux : position = 0, vitesse = 8, affichage via `LAB_05BA` + `LAB_0263`.

#### `LAB_059F` — Affichage de la lune

Appelée après le chargement de `bg1c.piv`. Efface la palette (fond noir), bascule
l'affichage sur le buffer secondaire `LAB_056C`, puis appelle `LAB_04B4` pour dessiner
le sprite de la lune :

```asm
LEA  LAB_011A, A0
MOVEA.L 16(A0), A0   ; données du sprite (zone LAB_011A[4])
MOVE.L  #$49, D0     ; frame index = 0x49 = 73
MOVE.W  #9,   D1     ; y = 9 px
MOVE.W  #60,  D2     ; largeur = 60 px
JSR LAB_04B4         ; rendu du sprite
```

Après 8 frames d'attente, `LAB_05A4` applique un dégradé de palette bleu nuit
(valeurs 0xFED → 0x842) simulant le ciel nocturne autour de la lune.

#### `LAB_05A0` — Flash blanc (transition)

Appelée après le chargement de `bg1b.piv`. Applique toutes les entrées de palette à
`0xFFF` (blanc pur) pendant 2 frames, puis fonde vers le noir. Ce flash sert de transition
visuelle entre la scène de la lune et les écrans de crédits.

#### `LAB_0174` — Transfert de buffers bitmap pour la cinématique de défilement

Appelée entre `LAB_001C` et `LAB_001A` (entre plans 6 et 7). Copie les bitmaps préchargés
des buffers secondaires vers les buffers primaires utilisés par le moteur de défilement,
afin que les fonds soient prêts pour les scènes cinématiques suivantes :

```asm
LAB_0174:
  copy_pixels LAB_00CB → LAB_00C8  ; bg4 (préchargé) → buffer ciel/crépuscule
  blend_palette LAB_01CE → LAB_01CB
  copy_pixels LAB_00CC → LAB_00C9  ; bg5a (préchargé) → buffer forêt nocturne
  blend_palette LAB_01CF → LAB_01CC
  copy_pixels LAB_00CD → LAB_00CA  ; bg3 (préchargé) → buffer plaine médiévale
  blend_palette LAB_01D0 → LAB_01CD
```

#### `LAB_0040` + `LAB_0041` — Effet d'éclair (6 flashes)

Utilisé dans le bytecode `LAB_00D6` (plan 12). Lance 6 flashs successifs :

```asm
LAB_0040:
  attendre 20 frames
  LAB_0041 (flash 1) : palette tout-0xFFF → attendre 2 frames → restaurer palette
  attendre 2 frames
  LAB_0041 (flash 2)
  attendre 20 frames
  LAB_0041 (flash 3)
  ... (flashes 4, 5, 6 avec délais décroissants)
  attendre 50 frames
  RTS
```

`LAB_0042` contient la palette tout-blanc (16 × `0x0FFF`) utilisée par `LAB_0041`.

#### `LAB_0032` — Flash unique

Un seul flash blanc (appelle `LAB_0041` une fois). Utilisé dans les scripts
`LAB_00E6` et `LAB_00D4` pour simuler des éclairs ponctuels.

### 3.5 Plans cinématiques — détail des 19 scènes

Après `LAB_0185`, SECSTRT_0 enchaîne les routines suivantes. Chaque routine met en
place un fond, une palette et spawne les entités via `LAB_0015`/`LAB_0016`, puis appelle
`LAB_0007` (boucle de jeu) jusqu'à la fin de l'animation (`LAB_0120 = 1`).

#### Plan 5 — Déclenchement de la musique + défilement vers la forêt (`LAB_05A5` / `LAB_05A6`)

```asm
LAB_05A5:                            ; [program.asm#L10871]
  JSR LAB_024B                       ; sauvegarde palette
  LEA LAB_01CB, A0                   ; palette bg1a (ciel)
  ; set couleurs ciel crépusculaire (0x0a00, 0x0600, 0x0300, 0x0fc6)
  JSR LAB_0565                       ; applique palette
  ; → LAB_05A6 : boucle de défilement

LAB_05A6:                            ; animation scroll 0 → 1000
  ; table vitesses LAB_05A9 : [0x000a, 0x0021, 0x0073, 0x00ae, 0x00eb, 0x013a]
  ; vitesses correspondantes  : [1, 2, 5, 5, 3, 1]
  CMPI.W #4, LAB_05B8+2
  BNE.S  skip_music
  JSR SECSTRT_1                      ; démarrer musique quand vitesse = 4
skip_music:
  ; avancer LAB_05B8 de (vitesse courante) par frame
  ; jusqu'à LAB_05B8 = 0x3E8 (1000) → fin du défilement
```

Les 3 buffers défilent du haut vers le bas : **bg1a** (ciel/crépuscule) → **bg1c** (forêt
nocturne) → **bg1b** (plaine médiévale). Le scroll s'accélère puis décélère pour un
effet cinématique.

#### Plan 5a — Continuation (`LAB_001B`) [program.asm#L326]

Spawne 5 entités `LAB_00D8` (table `LAB_0024`) via `LAB_001D`, sans changement de fond.
`LAB_002B+2 = 4` (paramètre de spawn), `ticks = 8`. Correspond à la transition entre
le défilement panoramique et la première scène animée.

#### Plan 6 — Druides marchant (`LAB_001C`) [program.asm#L337]

```asm
LAB_001C:
  JSR LAB_0258                       ; stop musique temporaire
  JSR LAB_01E4                       ; reset pool entités
  copy_pixels LAB_00CE → LAB_00C6   ; fond = bg2.piv
  JSR LAB_0263                       ; blitter fond
  MOVE.L #LAB_01D1, LAB_011D         ; palette bg2
  ; spawne 5 entités LAB_00D7 (table LAB_0025) via LAB_001D
  ; LAB_002B+2 = 8, LAB_00D1 = 1
  BSR LAB_001D
  MOVE.L #6, LAB_00D0                ; ticks = 6
  RTS
```

5 instances du script `LAB_00D7` (cycle de marche) sur le fond `bg2.piv`,
palette `LAB_01D1`.

#### Plan 7 — Druides vers Stonehenge (`LAB_001A`) [program.asm#L305]

```asm
LAB_001A:
  JSR LAB_01E4 ; JSR LAB_0258 ; JSR LAB_0262
  MOVE.L LAB_00CA, LAB_00C6          ; fond = bg1b.piv (plaine)
  JSR LAB_0263
  MOVE.L #LAB_01CD, LAB_011D         ; palette bg1b
  MOVE.W #4, LAB_011E
  BSR LAB_0030                       ; spawn entités de décor (voir §3.6)
  BSR LAB_001F                       ; spawn entités symétriques via LAB_0023
  LEA LAB_00E3, A0
  BSR LAB_0015                       ; spawn animation LAB_00E3 (druide central)
  JSR LAB_0007                       ; boucle principale
  MOVE.L LAB_00C7, LAB_00C6
  RTS
```

Fond `bg1b.piv`, palette `LAB_01CD`. `LAB_0030` spawne les entités `LAB_00DC`,
`LAB_00DD`, `LAB_00DA` + leurs miroirs `LAB_00DC`, `LAB_00DD`, `LAB_00D9`.
`LAB_001F` + table `LAB_0023` spawne `LAB_00DE × 2` + `LAB_00E0 × 2`.
Puis `LAB_00E3` (druide/chevalier en approche).

#### Plans 8-9 — Vue du dessus de Stonehenge + druides en cercle (`LAB_002C`) [program.asm#L446]

**Phase 1 (plan 8)** — script `LAB_00D2` sur fond `bg1a.piv`, palette `LAB_01CB` :

```asm
JSR LAB_01E4 ; JSR LAB_0258 ; JSR LAB_0262
MOVEA.L #LAB_00D2, A0 ; BSR LAB_0015   ; spawn LAB_00D2
MOVE.L LAB_00C8, LAB_00C6              ; fond = bg1a.piv
JSR LAB_0263
MOVE.L #LAB_01CB, LAB_011D             ; palette bg1a
MOVE.W #0, LAB_0120
JSR LAB_0007
```

**Phase 2 (plan 9)** — script `LAB_00D4` sur fond `bg1c.piv`, palette `LAB_01CC` :

```asm
JSR LAB_0258 ; JSR LAB_01E4
MOVEA.L #LAB_00D4, A0 ; BSR LAB_0015   ; spawn LAB_00D4
MOVE.L LAB_00C9, LAB_00C6              ; fond = bg1c.piv
JSR LAB_0263
MOVE.L #LAB_01CC, LAB_011D             ; palette bg1c
MOVE.W #4, LAB_011E
JSR LAB_0007
```

#### Plans 10-12 — Plongée, contre-plongée, éclairs (`LAB_002D`) [program.asm#L472]

```asm
LAB_002D:
  MOVE.W #1, LAB_00D1 ; MOVE.W #2, LAB_003E
  JSR LAB_01E4 ; JSR LAB_0258
  copy_pixels LAB_00CF → LAB_00C6   ; fond = bg2a.piv
  JSR LAB_0263
  MOVE.L #LAB_01D2, LAB_011D         ; palette bg2a
  MOVEA.L #LAB_00D6, A0
  JSR LAB_0015                       ; spawn LAB_00D6
  JSR LAB_0007
  MOVE.W #0, LAB_00D1 ; MOVE.W #0, LAB_003E
  RTS
```

Fond `bg2a.piv`, palette `LAB_01D2`. Le script `LAB_00D6` dessine 2 sprites puis
appelle `CALL_EXT LAB_0040` (6 flashes d'éclair — voir §3.4) pour simuler la foudre
du plan 12.

#### Plans 13-15 — Chevalier, main, entrée dans le cercle (`LAB_002F`) [program.asm#L506]

```asm
LAB_002F:
  JSR LAB_01E4 ; JSR LAB_0258 ; JSR LAB_0262
  MOVE.L LAB_00CA, LAB_00C6          ; fond = bg1b.piv (plaine/Stonehenge)
  JSR LAB_0263
  MOVE.L #LAB_01CD, LAB_011D         ; palette bg1b
  MOVE.W #0, LAB_011F ; MOVE.W #4, LAB_011E
  BSR LAB_0031                       ; spawn entités cérémonie complète (10 entités)
  MOVEA.L #LAB_00E4, A0 ; JSR LAB_0015  ; animation idle
  MOVEA.L #LAB_00E5, A0 ; JSR LAB_0015  ; animation en mouvement
  MOVE.L #8, LAB_00D0
  JSR LAB_0007
  MOVE.L #6, LAB_00D0
  MOVE.L LAB_00C7, LAB_00C6
  RTS
```

Fond `bg1b.piv`, palette `LAB_01CD`. `LAB_0031` spawne le cercle complet de
cérémonie : `LAB_00DC`, `LAB_00DD`, `LAB_00DB`, `LAB_00DF`, `LAB_00E1` + leurs 5 miroirs.
`LAB_00E4` = animation idle des druides en cercle.
`LAB_00E5` = animation du chevalier entrant dans le cercle.

#### Plans 16-17 — Druide et chevalier agenouillé, imposition de la main (`LAB_002E`) [program.asm#L491]

```asm
LAB_002E:
  JSR LAB_01E4 ; JSR LAB_0258
  copy_pixels LAB_00CC → LAB_00C6   ; fond = bg5a.piv
  JSR LAB_0263
  MOVE.L #LAB_01CF, LAB_011D         ; palette bg5a
  MOVEA.L #LAB_00D3, A0
  JSR LAB_0015                       ; spawn script LAB_00D3
  JSR LAB_0007
  RTS
```

Fond `bg5a.piv`, palette `LAB_01CF`. Script `LAB_00D3` : animation du druide posant
la main sur l'épaule du chevalier agenouillé.

#### Plan 18 — Carton texte de la quête

```asm
; SECSTRT_0 [program.asm#L147]
JSR LAB_025F          ; fondu au noir
LEA LAB_00AA, A0
JSR LAB_0054          ; affiche texte "The druids sent their best knights…"
MOVE.L #$1A4, D0
JSR LAB_054F          ; attente 420 frames (~8,4 s à 50 Hz)
JSR LAB_025F          ; fondu au noir
JSR LAB_005B          ; arrêt musique
```

`LAB_0054` ([program.asm#L1047](program.asm#L1047)) copie 0x10E6 octets depuis
`LAB_011A[20]` vers `LAB_056C`, puis affiche la chaîne `LAB_00AA` via `LAB_028F`
avec une palette sombre (0x0800 → 0x0000 dégradé).

### 3.6 Routines de spawn de décor — `LAB_0030` et `LAB_0031`

Ces deux routines spawne des groupes d'entités de décor. Elles sont utilisées dans
différents contextes :

**`LAB_0030`** ([program.asm#L526](program.asm#L526)) — Groupe réduit (6 entités) :

```asm
LAB_0030:
  MOVEA.L #LAB_00DC, A0 ; JSR LAB_0015   ; entité DC (côté A)
  MOVEA.L #LAB_00DD, A0 ; JSR LAB_0015   ; entité DD (côté A)
  MOVEA.L #LAB_00DA, A0 ; JSR LAB_0015   ; entité DA (côté A)
  MOVEA.L #LAB_00DC, A0 ; JSR LAB_0016   ; entité DC miroir (côté B)
  MOVEA.L #LAB_00DD, A0 ; JSR LAB_0016   ; entité DD miroir (côté B)
  MOVEA.L #LAB_00D9, A0 ; JSR LAB_0016   ; entité D9 miroir (côté B)
  RTS
```

Utilisé dans :
- **Plan 7 intro** (`LAB_001A`) : druides positionnés sur la plaine avant l'approche de Stonehenge
- **Scènes de combat** (mog.asm) : construction des décors d'arène — **les mêmes entités
  servent à décorer les arènes de combat**. Voir note dans `DOC_TECHNIQUE.md`.

**`LAB_0031`** ([program.asm#L540](program.asm#L540)) — Groupe complet cérémonie (10 entités) :

```asm
LAB_0031:
  MOVEA.L #LAB_00DC, A0 ; JSR LAB_0015
  MOVEA.L #LAB_00DD, A0 ; JSR LAB_0015
  MOVEA.L #LAB_00DB, A0 ; JSR LAB_0015
  MOVEA.L #LAB_00DF, A0 ; JSR LAB_0015
  MOVEA.L #LAB_00E1, A0 ; JSR LAB_0015
  ; + 5 miroirs via LAB_0016 (DC, DD, DB, DF, E1)
  RTS
```

Utilisé uniquement dans les plans 13-15 (`LAB_002F`) et en mode 2 joueurs (`LAB_0036`).
Spawne le cercle complet de cérémonie autour de Stonehenge, incluant les entités
spécifiques `LAB_00DB`, `LAB_00DF`, `LAB_00E1` non présentes dans `LAB_0030`.

### 3.7 Ressources de la scène d'introduction

| Type | Fichier | Buffer / Label | Rôle |
|---|---|---|---|
| PIV logo | `mindscape` | `SECSTRT_30` | Logo éditeur (plan 1) |
| PIV fond | `bg1a.piv` | `LAB_00C8` | Ciel crépusculaire (scroll + plan 8) |
| PIV fond | `bg1c.piv` | `LAB_00C9` | Forêt nocturne (lune + plan 9) |
| PIV fond | `bg1b.piv` | `LAB_00CA` | Plaine médiévale (plans 7, 13-15) |
| PIV fond | `bg4.piv` | `LAB_00CB` | Préchargé pendant crédits — fond cinématique |
| PIV fond | `bg5a.piv` | `LAB_00CC` | Préchargé pendant crédits — fond plans 16-17 |
| PIV fond | `bg3.piv` | `LAB_00CD` | Préchargé pendant crédits — fond cinématique |
| PIV fond | `bg2.piv` | `LAB_00CE` | Préchargé pendant crédits — fond plan 6 (druides marchant) |
| PIV fond | `bg2a.piv` | `LAB_00CF` | Préchargé pendant crédits — fond plans 10-12 (éclairs) |
| CEL sprite | `au1.cel` | zone `LAB_00C2` | Chevalier / personnage principal |
| CEL sprite | `li1.cel` | zone `LAB_00C2+` | Personnage secondaire |
| CEL sprite | `da1.cel` | zone `LAB_00C2++` | Druide / personnage |
| CEL sprite | `dw1.cel` | — | Personnage |
| CEL sprite | `ha1.cel` | — | Personnage |
| CEL sprite | `bg1.cel` | — | Décor (fond animé) |
| Tileset | `intro.stile` | `SECSTRT_33` | Tuiles du défilement intro |
| Musique | `music.cmp` | `LAB_0124` | Module ProTracker (RNC1) |
| Police | `bold.f` | `LAB_00F8` | Police de texte (glyphes) |

### 3.8 Schéma de la séquence d'introduction complète

```
SECSTRT_0 [L108]
│
LAB_025F ──▶ LAB_0185 [L3300]
               │
               ├─ SECSTRT_30 ← mindscape.piv              [plan 1 : logo Mindscape]
               ├─ LAB_00C8  ← bg1a.piv + LAB_059E (init scroll)
               ├─ LAB_00C9  ← bg1c.piv + LAB_059F         [plan 2 : lune — fond UNIQUE pour crédits]
               ├─ LAB_00CA  ← bg1b.piv + LAB_05A0 (flash blanc→noir)
               │                                           [plan 2 → 3 : transition]
               ├─ LAB_00CB  ← bg4.piv  (préchargé)        [plan 3-4 affiché : titre + crédits 0-5 sur fond lune]
               ├─ LAB_00CC  ← bg5a.piv (préchargé)
               ├─ LAB_00CD  ← bg3.piv  (préchargé)
               ├─ LAB_00CE  ← bg2.piv  (préchargé)
               ├─ LAB_00CF  ← bg2a.piv (préchargé)
               ├─ au1+li1+da1+dw1+ha1+bg1.cel (préchargés pendant crédits)
               └─ music.cmp → LAB_0190 (démarrage musique)

LAB_05A5 / LAB_05A6  [plan 5 : scroll ciel→forêt→plaine + déclenchement musique]
LAB_001B             [plan 5a : continuation scroll, 5× LAB_00D8]
LAB_001C             [plan 6 : druides marchant, fond bg2.piv, 5× LAB_00D7]
LAB_0174             [transfert buffers bitmap pour la cinématique suivante]
LAB_001A             [plan 7 : druides vers Stonehenge, fond bg1b.piv]
                         LAB_0030 (6 entités décor) + LAB_001F (LAB_0023) + LAB_00E3
LAB_002C             [plans 8-9 : vue dessus Stonehenge]
                         Phase 1 : fond bg1a.piv, script LAB_00D2
                         Phase 2 : fond bg1c.piv, script LAB_00D4
LAB_002D             [plans 10-12 : plongée + contre-plongée + éclairs]
                         fond bg2a.piv, script LAB_00D6 → LAB_0040 (6 éclairs)
LAB_002F             [plans 13-15 : chevalier, main, entrée dans cercle]
                         fond bg1b.piv, LAB_0031 (10 entités) + LAB_00E4 + LAB_00E5
LAB_002E             [plans 16-17 : druide + chevalier agenouillé]
                         fond bg5a.piv, script LAB_00D3
LAB_025F             [fondu au noir]
LAB_0054(LAB_00AA)   [plan 18 : carton texte "The druids sent their best knights…"]
LAB_054F(420 frames) [attente ~8,4 s]
LAB_025F             [fondu au noir]
LAB_005B             [arrêt musique]
  → SECSTRT_4 (menu de sélection)
```

---

## 4. Scène de fin (`LAB_003B`)

### 4.1 Point d'entrée et condition de déclenchement

`LAB_003B` ([program.asm#L749](program.asm#L749)) est appelée depuis la boucle principale du jeu (ligne 164) une fois que :
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

/* Opcode CALL_EXT ($B4) — 6 octets total */
typedef struct {
    uint8_t  opcode;       /* $B4 */
    uint8_t  param;        /* $00 = appel direct avec registres entité */
    void     (*fn)(void);  /* pointeur de fonction (4 octets, big-endian) */
} CallExtInstr;            /* 6 octets */
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

### 6.1 Flux intro

```
SECSTRT_0  [L108]
  ↓
LAB_038F        (init cache fichiers)
SECSTRT_29      (config hardware Amiga : bitplanes, copper, DMA)
LAB_0006        (init renderer IMAGEXCEL)
LAB_0044        (init audio + allocation mémoire sprite)
SECSTRT_10      (init table de dispatch opcodes LAB_0288)
LAB_0051        (init entité 0 → script LAB_0014 = flag stop)
  ↓
LAB_025F        (fondu au noir)
LAB_0185        (chargement progressif + affichage intro)
  ↓
  PLAN 1 : mindscape.piv → SECSTRT_30 ; palette LAB_0506 → logo Mindscape
  bg1a.piv → LAB_00C8 ; LAB_059E (init défilement 3 buffers, pos=0)
  PLAN 2 : bg1c.piv → LAB_00C9 ; LAB_059F (sprite lune, x=73 y=9)
  bg1b.piv → LAB_00CA ; LAB_05A0 (flash blanc → noir)
  PLANS 3-4 (crédits sur fond lune — préchargement des fonds cinématiques en parallèle) :
     bg4.piv  → LAB_00CB  (préchargé) + LAB_05A1 → affiche crédit 0 sur fond lune
     bg5a.piv → LAB_00CC  (préchargé) + LAB_05A1 → affiche crédit 1 sur fond lune
     bg3.piv  → LAB_00CD  (préchargé) + LAB_05A1 → affiche crédit 2 sur fond lune
     bg2.piv  → LAB_00CE  (préchargé) + LAB_05A1 → affiche crédit 3 sur fond lune
     bg2a.piv → LAB_00CF  (préchargé) + LAB_05A1 → affiche crédit 4 sur fond lune
     [charg. au1.cel]     (préchargé) + LAB_05A1 → affiche crédit 5 sur fond lune
  Chargement CEL : au1, li1, da1, dw1, ha1, bg1 (en arrière-plan)
  music.cmp → LAB_0190 (décompression RNC1 → ProTracker ; démarrage musique)
  ↓
PLAN 5  : LAB_05A5/05A6 (scroll ciel→forêt→plaine 0→1000 ; musique déclenchée)
PLAN 5a : LAB_001B     (5× LAB_00D8, continuation)
PLAN 6  : LAB_001C     (bg2.piv, 5× LAB_00D7 = druides marchant)
          LAB_0174     (transfert buffers bitmap pour cinématique suivante)
PLAN 7  : LAB_001A     (bg1b.piv, LAB_0030 + LAB_001F/LAB_0023 + LAB_00E3)
PLAN 8  : LAB_002C ph1 (bg1a.piv, script LAB_00D2)
PLAN 9  : LAB_002C ph2 (bg1c.piv, script LAB_00D4)
PLANS 10-12 : LAB_002D (bg2a.piv, LAB_00D6 → LAB_0040 = 6 éclairs)
PLANS 13-15 : LAB_002F (bg1b.piv, LAB_0031 + LAB_00E4 + LAB_00E5)
PLANS 16-17 : LAB_002E (bg5a.piv, script LAB_00D3)
PLAN 18 : LAB_025F → LAB_0054(LAB_00AA) : "The druids sent their best knights…"
          LAB_054F(420 frames) → LAB_025F → LAB_005B (stop musique)
  ↓
→ SECSTRT_4 (menu de sélection)
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
Références principales : `LAB_0185` (intro), `LAB_003B` (fin), `LAB_01F1` (interpréteur bytecode).*
