# Moteur IMAGEXCEL — Documentation du rendu de sprites

> Analyse strictement fondée sur la lecture du code assembleur `program.asm`
> (désassembly 68000 de Moonstone, Mindscape 1991).  
> Aucune inférence ou extrapolation : chaque affirmation est référencée
> par un label et un numéro de ligne dans `program.asm`.  
> La démarche suit le fichier **`dw1.cel`** depuis la disquette jusqu'à
> l'écran, conformément à la demande de l'issue.

---

## 1. Identification du moteur

La signature textuelle présente dans le binaire (lignes 9646–9672 de
`program.asm`, section `S_26`) déclare :

```text
IMAGEXCEL Code Module: SPRITE, Copyright 1988 by IMAGEXCEL Ve
ndor Code: ITE, copyright 1988 by IMAGEXCEL Programmer 1
```

Le moteur s'appelle **IMAGEXCEL**. Il gère le chargement, le clipping et le
rendu planaire des sprites en format `.cel` via le blitter Amiga.

---

## 2. Chargement de `dw1.cel` — de la disquette à la RAM chip

### 2.1 Table des noms de fichiers (`SECSTRT_8` / `LAB_0165`)

La section `SECSTRT_8` (vers la ligne 3168) contient une table de noms de
fichiers encodés en ASCII. Le désassembleur IRA les interprète comme des
instructions mais ce sont des données brutes. Le label `LAB_0165` pointe
sur la chaîne `"dw1.cel\0"` (octets `64 77 31 2E 63 65 6C 00`).

### 2.2 Allocation de la zone mémoire chip (`LAB_0044`)

La routine `LAB_0044` (lignes 920–960) gère l'allocation des zones de mémoire
chip utilisées par le moteur. Pour `dw1.cel` en particulier :

```asm
; Extrait de LAB_0044 (ligne ~940)
MOVE.L   LAB_00C4,D0          ; D0 = adresse courante du pool chip (valeur runtime)
ADDI.L   #$00058116,LAB_00C4  ; avance le pool de 0x58116 = 360 726 octets
LEA      LAB_011A,A0
MOVE.L   D0,LAB_0045          ; LAB_0045 = adresse de début de la zone dw1.cel
ADDI.L   #$00007530,D0        ; +30 000 octets
MOVE.L   D0,0(A0)             ; LAB_011A[0] = zone suivante
; ...
```

**Résultat** : `LAB_0045` reçoit la valeur originale de `LAB_00C4` au moment
de l'exécution. C'est l'adresse en RAM chip où seront déposées les données
décompressées de `dw1.cel`.

La valeur exacte de `LAB_00C4` dépend du contexte d'exécution (initialisation
au démarrage), mais elle se situe dans la zone de mémoire chip allouée au
moteur IMAGEXCEL et est distincte des zones occupées par les autres CEL.

### 2.3 Décompression et enregistrement du pointeur (`LAB_0185`)

La routine `LAB_0185` (lignes 3296+) charge tous les assets de l'introduction
dans l'ordre. Pour `dw1.cel` (lignes ~3461–3488) :

```asm
; Chargement de dw1.cel dans LAB_0185
MOVEA.L  LAB_0045,A1          ; A1 = destination = valeur de LAB_0045
LEA      LAB_0276,A0          ; A0 = table d'assets
MOVE.L   A1,12(A0)            ; LAB_0276+12 = pointeur sur les données dw1.cel
LEA      LAB_0165,A0          ; A0 = nom de fichier "dw1.cel"
JSR      LAB_0496             ; décompression LZSS → A1 (destination)
```

Après cet appel, les données CEL de `dw1.cel` sont décompressées à l'adresse
`*LAB_0045` en mémoire chip, et le pointeur vers ces données est stocké à
`LAB_0276 + 12` (offset 12 = position 3 × 4 octets dans la table).

### 2.4 Table d'assets `LAB_0276`

`LAB_0276` est déclarée en BSS (ligne 5028) comme `DS.L 10` : 10 emplacements
de 4 octets (10 pointeurs longs). Elle est remplie au runtime par `LAB_0185`
dans cet ordre :

| Offset (octets) | Fichier   | Source en mémoire     |
|----------------:|-----------|-----------------------|
| 0               | `au1.cel` | `LAB_00C2`            |
| 4               | `li1.cel` | fin de `au1.cel`      |
| 8               | `ha1.cel` | fin de `da1.cel`      |
| 12              | `dw1.cel` | `LAB_0045`            |
| 16              | `da1.cel` | fin de `li1.cel`      |

> **Note** : `ov1.cel` n'est **pas** dans `LAB_0276`. Il est chargé séparément
> à `LAB_00CF + 0x9C40`, pointeur stocké dans `LAB_0121`
> (ligne ~3490 de `program.asm`).

---

## 3. Utilisation de `dw1.cel` dans les scripts d'animation

### 3.1 Table de dispatch des entités (`LAB_0281`)

Lors de l'initialisation (`SECSTRT_10`, ligne ~3962), `LAB_0281` est remplie
de 7 copies de `#LAB_0276` :

```asm
; SECSTRT_10
LEA      LAB_0281,A0
MOVE.L   #LAB_0276,(A0)+   ; entité 1 → LAB_0276
MOVE.L   #LAB_0276,(A0)+   ; entité 2 → LAB_0276
; ... (7 fois au total)
```

Chaque entité d'animation possède un champ à l'offset 28 qui pointe sur sa
table d'assets. Par défaut, toutes les entités partagent `LAB_0276`.
L'opcode `$C4` (`LAB_0236`, cf. §4.1) permet de pointer une entité sur une
autre entrée de `LAB_0281`.

### 3.2 Format d'une instruction de dessin (6 octets)

L'interpréteur de scripts (`LAB_01F1`, ligne 4208) traite chaque instruction.
Pour les opcodes de dessin (bit 7 = 0), l'instruction fait **6 octets** :

```
Offset  Taille  Rôle
──────  ──────  ─────────────────────────────────────────────────────────
  0     1 octet  opcode = offset dans la table d'assets (exemple : $0C)
  1     1 octet  frame_index : index de la frame dans le fichier CEL
  2     1 octet  y_delta (signé) : ajouté à base_y + vel_y → coordonnée Y écran
  3     1 octet  flags : bit 4 = dessiner dans les deux buffers,
                         bit 5 = activer le mode masque (LAB_04DF)
  4–5   2 octets x_pos (signé, big-endian) : ajouté à base_x → coordonnée X écran
```

**Calcul des coordonnées écran** (code `LAB_01F7`, ligne ~4246) :

```asm
MOVE.B   2(A6),D2          ; D2 = y_delta (octet signé)
EXT.W    D2                ; extension de signe → mot signé
MOVE.W   4(A6),D1          ; D1 = x_pos (mot signé)
ADD.W    6(A1),D1          ; D1 += entity.base_x  → screen_x
ADD.W    8(A1),D2          ; D2 += entity.base_y
ADD.W    10(A1),D2         ; D2 += entity.vel_y   → screen_y
```

Le résultat est transmis à `LAB_04B4` : **D1 = X en pixels**, **D2 = Y en lignes**.

### 3.3 Sélection de `dw1.cel` via l'opcode `$0C`

Dans l'interpréteur (`LAB_01F7`) :

```asm
MOVEA.L  28(A1),A0         ; A0 = entity.asset_table_ptr = LAB_0276
ANDI.B   #$1f,D0           ; D0 = opcode & 0x1F (masque les bits redondants)
MOVEA.L  0(A0,D0.W),A0    ; A0 = LAB_0276[D0] = pointeur CEL
```

Avec `D0 = $0C` (12 décimal) : `LAB_0276 + 12` = pointeur sur `dw1.cel` ✓

### 3.4 Opcodes de contrôle (bit 7 = 1)

Les octets avec le bit 7 à 1 sont des opcodes de contrôle, dispatchés via
la table `LAB_0288` (23 entrées × 4 octets, remplie par `SECSTRT_10`) :

| Opcode (hex) | Offset dans LAB_0288 | Routine    | Comportement résumé |
|:---:|:---:|---|---|
| `$80` | 0  | `LAB_0215` | Change le flag de direction (bit 1 de entity+22) |
| `$84` | 4  | `LAB_0218` | Avance le PC de 3 octets (skip) |
| `$88` | 8  | `LAB_021A` | **SET_SPEED** : définit la vitesse d'animation (param=vitesse, avance PC de 2) |
| `$8C` | 12 | `LAB_021E` | Avance le PC de 8 octets |
| `$94` | 20 | `LAB_021F` | SET_LOOP_COUNT : initialise le compteur de répétition |
| `$98` | 24 | `LAB_0220` | NOP |
| `$9C` | 28 | `LAB_0220` | NOP |
| `$A0` | 32 | `LAB_0222` | **MOVE_DELTA** : déplace base_x/base_y/vel_y (absolu ou delta) |
| `$A4` | 36 | `LAB_0221` | Lit 1 octet param et avance PC de 4 |
| `$A8` | 40 | `LAB_0241` | (non analysé ici) |
| `$AC` | 44 | `LAB_022D` | NOP |
| `$B0` | 48 | `LAB_022C` | NOP |
| `$B4` | 52 | `LAB_022F` | CALL : appelle une routine externe (avance PC de 6) |
| `$BC` | 56 | `LAB_0233` | Avance PC de 6 |
| `$C0` | 60 | `LAB_0234` | Avance PC de 6 |
| `$C4` | 64 | `LAB_0235` | Remet à zéro entity[0] et avance PC de 2 |
| `$C8` | 68 | `LAB_0236` | **SET_ASSET_TABLE** : change entity.asset_table_ptr |
| `$CC` | 72 | `LAB_0232` | Avance PC de 6 |
| `$D0` | 76 | `LAB_0237` | Saut conditionnel si variable = 0 (avance PC de 8 sinon) |
| `$D4` | 80 | `LAB_023B` | Saut conditionnel si variable ≠ 0 (avance PC de 8 sinon) |
| `$D8` | 84 | `LAB_023F` | (non analysé ici) |

L'opcode `$FF` (sans bit 7 forcément…) est traité séparément dans
l'interpréteur (`LAB_01F3`) : il déclenche la logique de fin d'animation
(`LAB_0201`) qui peut boucler ou terminer selon l'état `anim_state`.

### 3.5 Exemple concret : décodage de `LAB_00E6` (frame 1)

`LAB_00E6` (ligne 2384) est le script d'animation qui produit le sprite
composé de plusieurs éléments de `dw1.cel` dans l'introduction. Voici le
décodage des 16 premiers octets :

```
Octets bruts : 88 0A  0C 00 9C 00 00 44  0C 02 EF 00 00 38  FF 00
```

**Instruction 1** (2 octets `88 0A`) :
- Opcode `$88` → `LAB_021A` = SET_SPEED
- Param `$0A` = 10 → `anim_state.speed = 10` (ticks avant passage à la frame suivante)
- Avance le PC de 2

**Instruction 2** (6 octets `0C 00 9C 00 00 44`) :
- Opcode `$0C` → draw `dw1.cel`
- `frame_index = $00` = frame 0
- `y_delta = $9C` → extension de signe → `$FF9C` = **−100** (signed 16 bits)
- `flags = $00`
- `x_pos = $0044` = **+68**
- screen_x = entity.base_x + 68 ; screen_y = entity.base_y + entity.vel_y − 100

**Instruction 3** (6 octets `0C 02 EF 00 00 38`) :
- Opcode `$0C` → draw `dw1.cel`
- `frame_index = $02` = frame 2
- `y_delta = $EF` → `$FFEF` = **−17**
- `flags = $00`
- `x_pos = $0038` = **+56**
- screen_x = entity.base_x + 56 ; screen_y = entity.base_y + entity.vel_y − 17

**Instruction 4** (2 octets `FF 00`) :
- Opcode `$FF` → fin du frame courant (`LAB_0201`)

### 3.6 Sprite composite : plusieurs instructions par frame

Les frames 2 et suivantes dans `LAB_00E6` montrent qu'une seule "frame
d'animation" peut contenir **plusieurs instructions de dessin `$0C`**,
chacune positionnant une frame différente de `dw1.cel` à un (x,y) différent.
C'est le mécanisme de **sprite composite** : l'entité est assemblée à partir
de plusieurs morceaux du même fichier CEL, chacun avec sa position et son
index de frame propres.

Exemple (frame 2, octets suivants dans `LAB_00E6`) :

```
88 02           → SET_SPEED 2
0C 07 9C 00 00 5B  → dw1.cel frame 7,  x=+91, y_delta=−100
0C 08 A9 00 00 3E  → dw1.cel frame 8,  x=+62, y_delta=−87
0C 09 C1 00 00 3C  → dw1.cel frame 9,  x=+60, y_delta=−63
0C 0A D9 00 00 36  → dw1.cel frame 10, x=+54, y_delta=−39
0C 0B EC 00 00 25  → dw1.cel frame 11, x=+37, y_delta=−20
FF 00 ...
```

5 fragments de `dw1.cel` sont dessinés pour former un seul personnage.

#### 3.6.1 Variante multi-fichiers : chevaliers en combat (`mog.asm`)

Le mécanisme composite va plus loin en combat : les chevaliers utilisent
**plusieurs fichiers `.ob` distincts** chargés dans des slots CEL différents.
Un chevalier occupe **une seule entrée** dans la table des 40 acteurs — il n'y
a pas d'acteur séparé par partie du corps. C'est le script de cet acteur
unique qui émet plusieurs instructions DRAW pointant chacune vers un slot CEL
différent.

**Chargement des fichiers `.ob` dans les slots CEL (`LAB_0116`, `mog.asm#L2429`)**

Appelée à l'initialisation d'un combat avec chevalier joueur
(`LAB_0164` → `LAB_016F` → `LAB_0116`) :

| Slot | Label      | Fichier    | Contenu                    |
|------|------------|------------|----------------------------|
| 0    | `LAB_0775` | `He1.ob`   | Corps / torse du chevalier |
| 1    | `LAB_0776` | `He2.ob`   | Jambes                     |
| 2    | `LAB_0777` | `He3.ob`   | Bras et épée               |

Pour un ennemi chevalier (`LAB_011A` → `LAB_011B`), seuls les slots 0
(`He1.ob`) et 1 (`He2.ob`) sont chargés.

**Pointeurs de scripts d'animation (`LAB_0167`, `mog.asm#L3575`)**

La routine `LAB_0167` remplit les pointeurs de scripts dans la structure de
l'acteur :

| Offset | Pointeur   | Script utilisé              |
|--------|------------|-----------------------------|
| `22`   | `LAB_07DB` | Déplacement direction 1     |
| `26`   | `LAB_07DC` | Déplacement direction 2     |
| `30`   | `LAB_05F6` | Marche (walk)               |
| `34`   | `LAB_05F5` | Attaque                     |
| `38`   | `LAB_05E1` | Script courant (repos)      |
| `42`   | `LAB_05F7` | Mort                        |
| `46`   | `LAB_0610` | Chute                       |
| `50`   | `LAB_05F8` | Dégâts reçus                |

**Exemple de script composite multi-slots : `LAB_07FC` (`mog.asm#L14234`)**

Script d'animation par défaut du chevalier joueur. Chaque frame enchaîne
plusieurs DRAW sur des slots différents :

```
; Fragment d'une frame de LAB_07FC
00 03 2E 00 FF 6D   → slot 0 (He1.ob), frame 3,  y=+46,  x=−147
00 02 F7 00 FF 71   → slot 0 (He1.ob), frame 2,  y=−9,   x=−143
0C 01 F9 00 FF 7C   → slot 3,          frame 1,  y=−7,   x=−132
FF 00               → fin de frame
```

La règle `slot = opcode >> 2` s'applique ici aussi :
`$00`→slot 0 (`He1.ob`), `$04`→slot 1 (`He2.ob`), `$08`→slot 2 (`He3.ob`),
`$0C`→slot 3.

**Résumé**

```
Table des 40 acteurs IMAGEXCEL
  └─ acteur[n]  ← UN SEUL acteur par combattant
       ├─ struct (132 octets) : position X/Y, HP, faction…
       └─ script courant (ex. LAB_07DC)
            ├─ DRAW slot0 frame_x  → He1.ob (corps)
            ├─ DRAW slot1 frame_y  → He2.ob (jambes)
            ├─ DRAW slot2 frame_z  → He3.ob (épée)
            └─ FF 00 (fin de frame)
```

L'interpréteur `LAB_01F1` (`program.asm#L4208`) parcourt le script et dessine
les trois morceaux l'un après l'autre, donnant l'illusion d'un personnage
composite animé.

---

## 4. Pipeline de rendu IMAGEXCEL

### 4.1 Initialisation (`LAB_0006`)

La routine `LAB_0006` (ligne ~193) effectue l'initialisation du moteur :

```asm
LAB_0006:
    MOVEQ    #5,D7              ; 5 plans bitmap
    JSR      SECSTRT_23         ; init IMAGEXCEL : D7-1 → LAB_04DE (compteur de plans)
    MOVE.W   #$0000,D0          ; X gauche = 0 (octet)
    MOVE.W   #$0000,D1          ; Y haut   = 0 (ligne)
    MOVE.W   #$0028,D2          ; X droit  = 40 (octets = 320 pixels)
    MOVE.W   #$00c8,D3          ; Y bas    = 200 (lignes)
    JSR      LAB_04A7           ; définit le viewport de découpage
    JSR      LAB_0262           ; présente le frame (échange double-buffer)
    RTS
```

**`SECSTRT_23`** (ligne 8804) :  
- Sauvegarde D0–D7/A0–A6 sur la pile
- `SUBQ.W #1,D7` puis `MOVE.W D7,LAB_04DE` → `LAB_04DE = 4` (compteur DBF pour 5 plans)
- Appelle `LAB_04B0` : génère la table de bit-reverse de 256 entrées à `LAB_04B3`
- Restaure les registres

**`LAB_04A7`** (ligne 8818) — définit le rectangle de viewport :

```asm
LAB_04A7:
    ; D0 = X gauche (octets), D1 = Y haut, D2 = X droit (octets), D3 = Y bas
    SUB.W   D0,D2              ; D2 = largeur en octets
    MOVE.W  D2,LAB_0502        ; LAB_0502 = viewport_width_bytes (40)
    SUB.W   D1,D3              ; D3 = hauteur en lignes
    MOVE.W  D3,LAB_0501        ; LAB_0501 = viewport_height (200)
    MULU    #$0028,D1          ; D1 = Y_haut × 40
    ADD.W   D0,D1              ; D1 += X_gauche
    MOVE.W  D1,LAB_0503        ; LAB_0503 = offset linéaire du coin supérieur gauche
    RTS
```

### 4.2 Point d'entrée `LAB_04B4`

```asm
LAB_04B4:
    TST.W   LAB_0528            ; flag d'initialisation déjà fait ?
    BNE.S   LAB_04B5            ; oui → aller directement au rendu
    MOVEM.L D0-D2/A0,-(A7)
    JSR     SECSTRT_25          ; non → init unique des buffers internes
    MOVEM.L (A7)+,D0-D2/A0
                                ; tombe dans LAB_04B5
```

**Registres d'entrée pour `LAB_04B4`** :
- `A0` = pointeur sur les données CEL décompressées (début de l'en-tête)
- `D0` = index de frame (entier non négatif)
- `D1` = coordonnée X en pixels (signée 16 bits)
- `D2` = coordonnée Y en lignes (signée 16 bits)

### 4.3 Logique principale `LAB_04B5`

#### Étape 1 — Validation de l'index de frame

```asm
LAB_04B5:
    TST.W   D0
    BLT.W   LAB_04CB            ; D0 < 0 → abandon
    CMP.W   (A0),D0             ; D0 >= frame_count ?
    BGE.W   LAB_04CB            ; oui → abandon
```

#### Étape 2 — Calcul du pointeur sur les données de la frame

```asm
    LSL.W   #1,D0               ; D0 *= 2
    MOVE.W  D0,D4
    LSL.W   #2,D0               ; D0 *= 4  →  D0 = frame_index * 8
    ADD.W   D4,D0               ; D0 += frame_index * 2  →  D0 = frame_index * 10
    MOVEA.L 2(A0),A2            ; A2 = pixel_data_base (pointeur dans en-tête global)
    ADDA.L  #$0000000a,A0       ; A0 += 10 (saute l'en-tête global, arrive à la table de frames)
    LEA     0(A0,D0.W),A0       ; A0 = &frame_table[frame_index]
    MOVE.L  (A0)+,D4            ; D4 = frame.pixel_data_offset
    LEA     0(A2,D4.L),A2      ; A2 = adresse réelle des pixels de cette frame
    MOVE.W  (A0)+,D5            ; D5 = frame.width_pixels (brut)
    ADDI.W  #$000f,D5
    ANDI.W  #$fff0,D5           ; D5 arrondi au multiple de 16 supérieur
    LSR.W   #4,D5               ; D5 = row_words (mots de 16 bits par ligne)
    MOVE.W  (A0)+,D4            ; D4 = frame.height (nombre de lignes)
```

#### Étape 3 — Lecture des métadonnées de frame et ajustement X

```asm
    CLR.W   D0
    MOVE.B  (A0)+,D0            ; D0 = frame.toggle_flags
    LSR.W   #4,D0               ; D0 = toggle_flags >> 4 (correction de décalage X)
    SUB.W   D0,D1               ; D1 (X en pixels) -= correction
    MOVE.B  (A0)+,D6            ; D6 = frame.planes_mask
    MOVE.B  D6,LAB_04FC         ; sauvegarde pour la phase de dessin
```

Le champ `toggle_flags` est un octet de la table de frames (décrit en §6).
Son nibble haut code une correction de position X pré-calculée par la routine
de flip horizontal (`LAB_04A8`) ; voir §5.3.

#### Étape 4 — Calcul de la taille du bloc planaire

```asm
    CLR.L   LAB_04FD
    CLR.L   LAB_04FE
    MOVEM.W D4-D5,-(A7)         ; sauvegardes (height, row_words)
    LSL.W   #1,D5               ; D5 = row_bytes (row_words × 2)
    MULU    D4,D5               ; D5 = taille d'un plan = row_bytes × height
    MOVE.L  D5,LAB_0504         ; LAB_0504 = plane_size (en octets)
    MOVEM.W (A7)+,D4-D5         ; restauration
```

#### Étape 5 — Découpage vertical (axe Y)

```asm
    TST.W   D2
    BGE.W   LAB_04B6            ; Y >= 0 : pas de clip supérieur
    ; Clip supérieur : sprite commence avant le haut de l'écran
    ADD.W   D2,D4               ; D4 = height + D2 (D2 est négatif → réduction)
    BLE.W   LAB_04CB            ; sprite entièrement hors écran → abandon
    MOVE.W  D5,D6
    NEG.W   D2
    MULU    D2,D6               ; D6 = (-D2) × row_words = offset en mots vers début visible
    LSL.W   #1,D6               ; D6 = offset en octets
    LEA     0(A2,D6.W),A2       ; avance A2 vers la première ligne visible
    MOVEQ   #0,D2

LAB_04B6:
    CMP.W   LAB_0501,D2         ; D2 >= viewport_height (200) ?
    BGE.W   LAB_04CB            ; oui → abandon
    MOVE.W  D2,D6
    ADD.W   D4,D6               ; D6 = Y + height
    SUB.W   LAB_0501,D6         ; D6 = (Y + height) - 200
    BLE.W   LAB_04B7            ; pas de clip inférieur
    SUB.W   D6,D4               ; D4 = height - dépassement (clip bas)
```

#### Étape 6 — Calcul de la position X et découpage horizontal

```asm
LAB_04B7:
    MOVE.W  D1,D6
    ANDI.W  #$000f,D6           ; D6 = D1 & 0x0F = décalage sub-16 bits
    MOVE.W  D6,LAB_0505         ; sauvegarde pour le calcul des masques de bord
    ANDI.W  #$fff0,D1           ; aligne D1 sur un multiple de 16 pixels
    ASR.W   #3,D1               ; D1 = offset en octets depuis le bord gauche
    ; D1 est maintenant l'offset X en octets (aligné sur un mot de 16 bits)
    ; D6 = LAB_0505 = décalage résiduel en bits (0..15) pour le barrel shifter
```

Le clip gauche se traite si `D1 < 0` ; le clip droit si `D1 + row_words×2 >
LAB_0502` (40). Les flags `LAB_04FF` (clip gauche) et `LAB_0500` (clip droit)
sont positionnés et la largeur `D5` réduite en conséquence.

#### Étape 7 — Calcul de l'offset linéaire dans le framebuffer

```asm
LAB_04BA:
    MULU    #$0028,D2           ; D2 = Y × 40 (40 octets/ligne = 320 pixels/8)
LAB_04BB:
    ADD.W   D2,D1               ; D1 = Y×40 + X_bytes = offset linéaire
    ; D1 += LAB_0503 appliqué plus tard dans la phase cookie-cut
```

> **Remarque** : si `LAB_0527` est non nul (mode non standard), le stride
> est lu depuis `SECSTRT_24` au lieu d'être fixé à 40.

---

## 5. Phase 1 : copie des données planaires dans le buffer intermédiaire

### 5.1 Buffer intermédiaire `LAB_051B`

`SECSTRT_25` (ligne 9375) est la routine d'initialisation unique des buffers
IMAGEXCEL. Elle calcule `LAB_051B` (routine `LAB_04E3`) :

```asm
LAB_04E3:
    MOVE.L  D0,LAB_0519         ; D0 = SECSTRT_27 (zone mémoire de travail)
    ADDI.L  #$00001000,D0
    MOVE.L  D0,LAB_0518
    ADDI.L  #$0000222e,D0
    MOVE.L  D0,LAB_051A
    ADDI.L  #$00001000,D0
    MOVE.L  D0,LAB_051B         ; buffer intermédiaire de rendu
    ADDI.L  #$00004b00,D0       ; + 4 × 0x12C0 (4 plans supplémentaires)
    ADDI.L  #$000012c0,D0       ; + 1 plan (0x12C0 = 4800 octets par plan)
    MOVE.L  D0,LAB_051C
```

`LAB_051B` est un buffer de **5 × 4 800 = 24 000 octets** structuré comme :

```
LAB_051B + 0 × 0x12C0  = buffer plan 0  (4800 octets = 40 × 120 lignes max)
LAB_051B + 1 × 0x12C0  = buffer plan 1
LAB_051B + 2 × 0x12C0  = buffer plan 2
LAB_051B + 3 × 0x12C0  = buffer plan 3
LAB_051B + 4 × 0x12C0  = buffer plan 4
LAB_051B + 5 × 0x12C0  = buffer masque composite (généré en §6)
```

### 5.2 Boucle de copie planaire (`LAB_04BC`)

```asm
; Avant la boucle :
MOVEA.L  LAB_051B,A1        ; A1 = début du buffer intermédiaire (plan 0)
MOVE.L   LAB_0504,D3        ; D3 = plane_size (taille d'un plan en octets)
MOVE.B   LAB_04FC,D6        ; D6 = planes_mask (quels plans rendre)
MOVE.W   LAB_04DE,D7        ; D7 = 4 (num_planes - 1, pour DBF)
; Ajustements des pointeurs source A2 selon le clipping...

LAB_04BC:                   ; début de boucle sur les 5 plans
    MOVEM.W  D4-D5,-(A7)
    LSR.W    #1,D6          ; décale planes_mask → le bit sorti dans CF
    BCC.W    LAB_04BF       ; si bit = 0 : plan non actif, on saute
    TST.W    LAB_04DF       ; mode masque activé ?
    BEQ.S    LAB_04BD
    JSR      LAB_04CF       ; oui → kernel masqué
    BRA.S    LAB_04BE
LAB_04BD:
    JSR      LAB_04CC       ; non → kernel copie directe
LAB_04BE:
    LEA      0(A2,D3.L),A2  ; A2 += plane_size → plan suivant dans la source
LAB_04BF:
    ADDA.L   #$000012C0,A1  ; A1 += 0x12C0 → slot suivant dans le buffer intermédiaire
    MOVEM.W  (A7)+,D4-D5
    DBF      D7,LAB_04BC    ; boucle pour D7 = 4..0 (5 itérations)
```

### 5.3 Kernel `LAB_04CC` — copie directe A→D

`LAB_04CC` (ligne 9242) copie les données d'un plan de `dw1.cel` (A2) vers
le slot du buffer intermédiaire (A1), sans masque :

```asm
LAB_04CC:
    JSR      LAB_04D7           ; attente blitter
    MOVE.L   A2,BLTAPTH         ; source A = données pixel du plan (CEL)
    MOVE.L   A1,BLTDPTH         ; dest D   = buffer intermédiaire
    MOVE.W   D0,BLTAMOD         ; modulo source (ajustement clip)
    MOVE.W   #$0002,BLTDMOD     ; modulo dest = 2 octets (un mot sentinel par ligne)
    MOVE.W   #$09F0,BLTCON0     ; LF=$F0 → D=A  (copie canal A vers D)
    MOVE.W   #$0000,BLTCON1
    MOVE.W   #$FFFF,BLTAFWM
    MOVE.W   #$FFFF,BLTALWM
    ; Calcul des masques de bord (LAB_0505 = décalage sub-16 bits) :
    MOVEQ    #0,D1
    MOVE.W   #$FFFF,D1          ; D1 = 0x0000FFFF
    MOVE.W   LAB_0505,D2
    LSL.L    D2,D1              ; D1 = $FFFF << sous-décalage → masque de bord
    TST.W    LAB_0500           ; clip droit ?
    BEQ.W    LAB_04CD
    MOVE.W   D1,BLTALWM         ; masque dernier mot (bord droit)
LAB_04CD:
    TST.W    LAB_04FF           ; clip gauche ?
    BEQ.W    LAB_04CE
    SWAP     D1
    MOVE.W   D1,BLTAFWM         ; masque premier mot (bord gauche)
LAB_04CE:
    LSL.W    #6,D4              ; D4 = height << 6
    OR.W     D5,D4              ; D4 = (height<<6) | row_words → BLTSIZE
    MOVE.W   D4,BLTSIZE         ; déclenche le blitter
    RTS
```

Le format `BLTSIZE` Amiga : bits 15–6 = hauteur, bits 5–0 = largeur en mots.
`$09F0` = `0000 1001 1111 0000` : USEA=1, USED=1, LF=$F0 → D = A.

### 5.4 Kernel `LAB_04CF` — copie avec masquage des bords

`LAB_04CF` (ligne 9269) est utilisé quand le flag `LAB_04DF` est positionné
(bit 5 du champ `flags` de l'instruction de dessin). Il effectue une copie
CPU de la source dans le buffer intermédiaire, puis applique un masque de bord
sur les premier et dernier mots de chaque ligne selon `LAB_0505`, `LAB_0500`
et `LAB_04FF`. Ce kernel est une copie software (boucles `DBF`) suivie d'une
opération AND en mémoire pour les bords.

### 5.5 Attente blitter `LAB_04D7`

```asm
LAB_04D7:
    BTST    #6,DMACONR          ; bit 6 de DMACONR = blitter busy
    BNE.S   LAB_04D7            ; boucle jusqu'à ce que le blitter soit libre
    RTS
```

---

## 6. Phase 2 : génération du masque composite et cookie-cut final

Après la boucle de copie planaire, les 5 plans du sprite se trouvent dans les
5 slots de `LAB_051B`. Le code génère ensuite un **masque composite** (OR de
tous les plans) puis effectue le **cookie-cut** de chaque plan dans le
framebuffer de destination.

### 6.1 Mise en place des pointeurs et calcul du BLTSIZE

```asm
MOVEM.W  D4-D5,-(A7)            ; save (height_clipped, row_words)
LSL.W    #6,D4                  ; D4 = height_clipped << 6
OR.W     D5,D4                  ; D4 = BLTSIZE final
MOVEA.L  LAB_051B,A0            ; A0 = plan 0
MOVE.L   #$000012C0,D0
LEA      0(A0,D0.W),A1          ; A1 = plan 1
LEA      0(A1,D0.W),A2          ; A2 = plan 2
LEA      0(A2,D0.W),A3          ; A3 = plan 3
LEA      0(A3,D0.W),A4          ; A4 = plan 4
LEA      0(A4,D0.W),A5          ; A5 = slot après plan 4 → buffer masque
JSR      LAB_04D7               ; attente blitter
```

### 6.2 Deux passes de blitter pour construire le masque composite

**Passe 1** (plans 0, 1, 2 → masque partiel en A5) :

```asm
; D0 initialé à $0100 (USED=1 seulement)
; Pour chaque plan actif dans planes_mask :
;   plan 0 actif → D0 |= $08F0  (USEA=1, LF=$F0 → D |= A)
;   plan 1 actif → D0 |= $04CC  (USEB=1, LF=$CC → D |= B)
;   plan 2 actif → D0 |= $02AA  (USEC=1, LF=$AA → D |= C)
MOVE.W   D0,BLTCON0
MOVE.L   A0,BLTAPTH             ; A = plan 0
MOVE.L   A1,BLTBPTH             ; B = plan 1
MOVE.L   A2,BLTCPTH             ; C = plan 2
MOVE.L   A5,BLTDPTH             ; D = buffer masque
MOVE.W   D4,BLTSIZE             ; déclenche passe 1
```

Les mintermes `$F0`, `$CC`, `$AA` réalisent D = A OR B OR C (pour les plans
actifs) : D vaut 1 partout où l'un des trois plans vaut 1.

**Passe 2** (plans 3, 4 complètent le masque dans A5) :

```asm
; D0 = $03AA (USEB=1+USEC=1+USED=1)
;   plan 3 actif → D0 |= $08F0
;   plan 4 actif → D0 |= $04CC
JSR      LAB_04D7               ; attente fin passe 1
MOVE.W   D0,BLTCON0
MOVE.L   A3,BLTAPTH             ; A = plan 3
MOVE.L   A4,BLTBPTH             ; B = plan 4
MOVE.L   A5,BLTCPTH             ; C = résultat passe 1
MOVE.L   A5,BLTDPTH             ; D = buffer masque (mis à jour)
MOVE.W   D4,BLTSIZE             ; déclenche passe 2
```

Après ces deux passes, `A5` (= `LAB_051B + 5 × 0x12C0`) contient le masque
composite : un bit vaut 1 partout où **au moins un** des 5 plans du sprite
vaut 1. Ce masque délimite la zone opaque du sprite.

### 6.3 Nettoyage des mots sentinelles

Après les deux passes de génération de masque, une boucle efface les mots en
fin de chaque ligne dans les 6 buffers (5 plans + masque). Ces mots
sentinelles sont nécessaires au barrel shifter pour les blits en mode cookie-cut.

### 6.4 Cookie-cut final : boucle sur les 5 plans (`LAB_04C8`/`LAB_04C9`)

```asm
LAB_04C8:
    MOVE.W   D7,BLTAMOD         ; modulo A (0 ou 2 selon clip droit)
    MOVE.W   D7,BLTBMOD         ; modulo B (idem)
    MOVE.W   D3,BLTCMOD         ; modulo C = stride dest (40 - row_words×2)
    MOVE.W   D3,BLTDMOD         ; modulo D = idem
    MOVE.W   #$000c,D7
    LSL.W    D7,D6              ; D6 = sous-décalage << 12 (champ ASH de BLTCON0)
    MOVE.W   D6,D4              ; sauvegarde du décalage seul
    ORI.W    #$0ff2,D6          ; D6 = BLTCON0 cookie-cut (ASH | $0FF2)
    MOVE.W   D6,BLTCON0
    MOVE.W   D4,BLTCON1         ; BLTCON1 = décalage seul (barrel shifter)
    ORI.W    #$0722,D4          ; BLTCON0 alternatif pour plans non actifs
    MOVE.W   #$ffff,BLTAFWM
    MOVE.W   #$ffff,BLTALWM
    LEA      LAB_04D9,A0        ; A0 = tableau des 5 pointeurs de plans dest
    MOVEA.L  LAB_051B,A1        ; A1 = plan 0 du buffer intermédiaire
    MOVEA.L  A1,A4
    ADDA.L   #$00005DC0,A4      ; A4 = masque composite (LAB_051B + 5×0x12C0)
    MOVE.B   LAB_04FC,D7        ; D7 = planes_mask
    MOVE.W   LAB_04DE,D3        ; D3 = 4 (compteur DBF)

LAB_04C9:
    MOVEA.L  (A0)+,A5           ; A5 = prochain pointeur de plan destination
    LEA      0(A5,D5.W),A5      ; A5 += offset linéaire (X + Y×40)
    BSR.W    LAB_04D7           ; attente blitter
    MOVE.W   D6,BLTCON0         ; BLTCON0 = cookie-cut avec décalage
    LSR.W    #1,D7              ; teste plans_mask pour ce plan
    BCS.W    LAB_04CA           ; plan actif → garder BLTCON0 = $0FF2
    MOVE.W   D4,BLTCON0         ; plan inactif → BLTCON0 = $0722

LAB_04CA:
    MOVE.L   A1,BLTAPTH         ; A = plan sprite (buffer intermédiaire)
    MOVE.L   A4,BLTBPTH         ; B = masque composite
    MOVE.L   A5,BLTCPTH         ; C = plan destination (fond)
    MOVE.L   A5,BLTDPTH         ; D = plan destination (écriture)
    MOVE.W   D1,BLTSIZE         ; déclenche le blit cookie-cut
    ADDA.L   #$000012C0,A1      ; avance au plan suivant dans le buffer intermédiaire
    DBF      D3,LAB_04C9        ; boucle sur les 5 plans
```

**Minterme utilisé pour les plans actifs (`$0FF2`)**  
BLTCON0 = `(décalage << 12) | $0FF2` → LF = `$F2` = `1111 0010`.

Avec A = données sprite, B = masque composite, C = fond :
- Si B = 1 et A = 1 → D = 1 (pixel sprite allumé)
- Si B = 1 et A = 0 → D = 0 (pixel sprite éteint dans ce plan)
- Si B = 0 → A et C sont nécessairement dans un état cohérent : D = C (fond conservé)

Cette logique est l'équivalent d'un **cookie-cut** : la zone du masque est
remplacée par les données sprite ; le fond est conservé en dehors.

**Minterme pour les plans non actifs (`$0722`)**  
LF = `$22` = D = C AND NOT_B → efface la zone du masque dans le plan, conserve le fond
ailleurs. Cela garantit que les plans marqués inactifs dans `planes_mask`
n'affichent pas de données résiduelles dans la zone du sprite.

---

## 7. Gestion du double-buffer

### 7.1 Les deux framebuffers

Le moteur utilise deux framebuffers de **5 × 8 000 = 40 000 octets** chacun
(5 plans × 40 octets/ligne × 200 lignes) :
- `SECSTRT_30` : buffer affiché par le copper (buffer **front**)
- `LAB_056C` : buffer de rendu actif (buffer **back**)

### 7.2 `LAB_026C` — définition des pointeurs de plans

`LAB_026C` (ligne 4967) reçoit D0 = adresse de base d'un framebuffer et
calcule les 5 adresses de plans (stride 0x1F40 = 8 000 octets) :

```asm
LAB_026C:
    MOVEA.L  D0,A1              ; A1 = plan 0
    ADDI.L   #$00001F40,D0
    MOVEA.L  D0,A2              ; A2 = plan 1 = base + 8000
    ADDI.L   #$00001F40,D0
    MOVEA.L  D0,A3              ; A3 = plan 2 = base + 16000
    ADDI.L   #$00001F40,D0
    MOVEA.L  D0,A4              ; A4 = plan 3 = base + 24000
    ADDI.L   #$00001F40,D0
    MOVEA.L  D0,A5              ; A5 = plan 4 = base + 32000
    JSR      LAB_04A6           ; stocke A1..A5 dans LAB_04D9..LAB_04DD
    RTS
```

`LAB_04A6` (ligne 8811) :

```asm
LAB_04A6:
    MOVE.L   A1,LAB_04D9        ; pointeur plan 0 dest
    MOVE.L   A2,LAB_04DA        ; pointeur plan 1 dest
    MOVE.L   A3,LAB_04DB        ; pointeur plan 2 dest
    MOVE.L   A4,LAB_04DC        ; pointeur plan 3 dest
    MOVE.L   A5,LAB_04DD        ; pointeur plan 4 dest
    RTS
```

`LAB_04D9`..`LAB_04DD` sont les 5 pointeurs utilisés par `LAB_04C9` pour
écrire dans le framebuffer de destination.

### 7.3 `LAB_0262` — présentation du frame et échange des buffers

```asm
LAB_0262:
    JSR     LAB_054C            ; met à jour le copper list (affiche l'ancien back buffer)
    MOVE.L  LAB_0279,D0
    MOVE.L  LAB_027A,LAB_0279
    MOVE.L  D0,LAB_027A         ; échange des pointeurs de sprite buffer
    MOVE.L  LAB_0279,LAB_027C   ; LAB_027C = nouveau front
    MOVE.L  LAB_056C,D0
    BSR.W   LAB_026C            ; D0 = LAB_056C → met à jour LAB_04D9..DD
    RTS
```

`LAB_054C` (ligne 10210) copie les adresses de `LAB_056C` dans les registres
du copper list (`EXT_0016..EXT_001F`, i.e. `BPL1PTH`..`BPL5PTL`) puis échange
`LAB_056C` ↔ `SECSTRT_30`. Ainsi :
- L'ancien back buffer (où l'on vient de dessiner) devient le buffer affiché
- L'ancien front buffer devient le nouveau buffer de rendu

---

## 8. Rendu en miroir horizontal (`LAB_04A8`)

### 8.1 Table de bit-reverse (`LAB_04B0` / `LAB_04B3`)

`LAB_04B0` (ligne 8852) génère, à l'initialisation, une table de 256 entrées
à `LAB_04B3` où chaque entrée `i` contient la valeur de `i` avec les 8 bits
inversés (bit 0 ↔ bit 7, bit 1 ↔ bit 6, etc.) :

```asm
LAB_04B0:
    MOVE.W   #$00ff,D7          ; 256 entrées
    CLR.W    D0
    LEA      LAB_04B3,A0
LAB_04B1:
    MOVE.W   D0,-(A7)
    MOVEQ    #7,D6              ; 8 bits
LAB_04B2:
    LSR.W    #1,D0              ; D0 >> 1 → bit dans CF
    ROXL.W   #1,D1              ; insère CF à gauche de D1
    DBF      D6,LAB_04B2
    MOVE.B   D1,(A0)+           ; stocke l'octet inversé
    MOVE.W   (A7)+,D0
    ADDQ.W   #1,D0
    DBF      D7,LAB_04B1
    RTS
```

### 8.2 Bit-reverse in-place (`LAB_04AC`)

`LAB_04AC` (ligne 8869) est appelée depuis `LAB_04A8` (la variante de rendu
miroir). Elle modifie **en place** les données pixels du CEL en inversant
l'ordre des bits dans chaque octet de chaque ligne, pour chaque plan actif :

```asm
LAB_04AC:
    MOVEA.L  LAB_051B,A0        ; A0 = buffer intermédiaire (zone temporaire)
    LEA      LAB_04B3,A1        ; A1 = table de bit-reverse
    LEA      0(A0,D5.W),A0     ; A0 = LAB_051B + row_bytes (fin du row temp)
    MOVE.W   D4,D3
    SUBQ.W   #1,D3              ; D3 = height - 1

LAB_04AD:
    MOVE.L   A0,-(A7)
    MOVE.L   A2,-(A7)           ; sauvegarde A2 (début de ligne source)
    MOVE.W   D5,D2
    SUBQ.W   #1,D2

LAB_04AE:                       ; pour chaque octet de la ligne source :
    MOVE.B   (A2)+,D0           ; lit l'octet source
    MOVE.B   0(A1,D0.W),D0     ; bit-reverse via table
    MOVE.B   D0,-(A0)           ; écrit en ordre inversé dans le buffer temp
    DBF      D2,LAB_04AE

    MOVEA.L  (A7)+,A2           ; restaure A2 = début de ligne source
    MOVE.W   D5,D2
    SUBQ.W   #1,D2

LAB_04AF:                       ; copie le buffer temp vers la source
    MOVE.B   (A0)+,(A2)+
    DBF      D2,LAB_04AF

    MOVEA.L  (A7)+,A0
    DBF      D3,LAB_04AD        ; ligne suivante
    DBF      D7,LAB_04AB        ; plan suivant
    RTS
```

**Effet** : pour chaque ligne de chaque plan actif, les octets sont inversés
bit-à-bit **et** écrits en ordre miroir, puis recopiés en place. Le résultat
net est un **flip horizontal** des données pixel du plan.

### 8.3 Le mécanisme `toggle_flags` (octet d'état de flip)

Le premier octet `toggle_flags` de chaque entrée de la table de frames
(offset +8, cf. §9.2) sert de registre d'état :

- **bit 0** : vaut 0 si les données sont en état "normal", 1 si déjà flippées.
- **nibble haut (bits 7–4)** : cache la valeur du padding `D7 = (width_rounded - width_raw)`,
  calculée une seule fois par `LAB_04A8` et réutilisée par `LAB_04B5` comme
  correction de position X (soustraction `SUB.W D0,D1`).

Lors de chaque appel à `LAB_04A8` :
- Si `toggle_flags & 1 == 0` (données actuellement normales) : on flip et on
  écrit `1` dans `toggle_flags[0]`
- Si `toggle_flags & 1 == 1` (déjà flippé) : on calcule `D7 << 4` et on
  l'écrit dans le nibble haut de `toggle_flags`

Cela permet de **ne flipper qu'une seule fois** quand l'état change, et de
réutiliser la correction pré-calculée lors des frames suivantes.

`LAB_020B` (ligne 4406) est appelé avant chaque dessin d'entité. Il compare
le `toggle_flags` de la frame avec le flag de direction de l'entité
(`22(A1)`). Si la valeur diffère, il appelle `LAB_026E` → `LAB_04A8` pour
effectuer le flip in-place.

---

## 9. Format des données CEL décompressées

### 9.1 En-tête global (10 octets)

```
Offset  Taille  Contenu
──────  ──────  ─────────────────────────────────────────────
  0      2       frame_count : nombre total de frames (ex. 0x35 = 53 pour dw1.cel)
  2      4       pixel_data_base : adresse absolue en RAM chip de début des données pixel
                 (utilisé comme base pour les offsets dans la table de frames)
  6      4       (non utilisé par LAB_04B5)
```

### 9.2 Table de frames (frame_count × 10 octets)

Immédiatement après l'en-tête, pour chaque frame :

```
Offset  Taille  Contenu
──────  ──────  ─────────────────────────────────────────────
  0      4       pixel_data_offset : offset depuis pixel_data_base vers les données de cette frame
  4      2       width_pixels : largeur en pixels (non arrondie)
  6      2       height_lines : hauteur en lignes
  8      1       toggle_flags : bit 0 = état flip ; nibble haut = correction X cachée
  9      1       planes_mask  : bit n = 1 → le plan n est actif (0..4)
```

Les structures sont accédées via `ADDA.L #$0a,A0; LEA 0(A0,D0.W),A0` avec
`D0 = frame_index × 10`.

### 9.3 Données pixel planaires

Les données pixel d'une frame sont organisées en **planaire entrelacé** :
```
[ plan 0, ligne 0 ]  [ plan 0, ligne 1 ]  ...  [ plan 0, ligne H-1 ]
[ plan 1, ligne 0 ]  [ plan 1, ligne 1 ]  ...  [ plan 1, ligne H-1 ]
...
[ plan 4, ligne 0 ]  [ plan 4, ligne 1 ]  ...  [ plan 4, ligne H-1 ]
```

Chaque ligne d'un plan occupe `row_bytes = ((width + 15) / 16) × 2` octets.
La taille totale d'une frame dans le buffer est `planes × row_bytes × height`.

Le stride entre le début d'un plan et le début du suivant est `LAB_0504 =
row_bytes × height` (calculé lors du rendu).

---

## 10. Récapitulatif : chemin complet de `dw1.cel` jusqu'à l'écran

```
Disquette
  └─ "dw1.cel" (compressé LZSS)
       │
       ▼
  LAB_0185 : JSR LAB_0496 (décompression LZSS → A1 = *LAB_0045)
       │
       ├─► LAB_0276+12 ← pointeur sur les données CEL décompressées
       │
       ▼
  Script d'animation (ex. LAB_00E6) :
    opcode $0C 00 9C 00 00 44    (6 octets)
       │
       ▼
  LAB_01F7 (interpréteur) :
    A0 = LAB_0276[12] = *LAB_0045  (pointeur CEL dw1.cel)
    D0 = 0 (frame_index)
    D1 = entity.base_x + x_pos    (screen_x en pixels)
    D2 = entity.base_y + vel_y + y_delta  (screen_y en lignes)
       │
       ▼
  LAB_04B4 → LAB_04B5 :
    - validation index de frame
    - calcul pixel_data_ptr, width, height, toggle_flags, planes_mask
    - clipping Y et X
    - calcul offset linéaire dans le framebuffer
       │
       ▼
  Boucle LAB_04BC (5 plans) :
    LAB_04CC ou LAB_04CF : copie plan CEL → buffer intermédiaire LAB_051B
       │
       ▼
  Passes LAB_04C0–LAB_04C4 :
    deux blits → masque composite OR dans LAB_051B+0x5DC0
       │
       ▼
  Boucle LAB_04C9 (5 plans) :
    BLTCON0 = (shift<<12)|$0FF2 (cookie-cut)
    A = plan sprite (LAB_051B), B = masque, C = D = fond (LAB_04D9..DD)
    → écriture dans le back-buffer (LAB_056C)
       │
       ▼
  LAB_0262 → LAB_054C :
    échange front/back → le frame rendu apparaît à l'écran (copper list)
```
