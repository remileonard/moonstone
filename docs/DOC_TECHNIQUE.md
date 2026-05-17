# Documentation technique — `program.asm` (Moonstone / Amiga, 68000)

> Analyse statique de [program.asm](program.asm) (11 319 lignes, ~194 Kio,
> désassemblé par IRA V2.11). Document destiné à préparer un portage C.
>
> **Avertissement** — IRA est un désassembleur sans informations symboliques :
> tous les labels sont `LAB_xxxx` / `SECSTRT_n` / `EXT_xxxx`. Les rôles
> indiqués sont **inférés** d'après le pattern d'instructions et les registres
> hardware touchés. Quand l'inférence est incertaine elle est marquée *(?)*.
> Tous les numéros de ligne renvoient à `program.asm`.

---

## 1. Vue d'ensemble

Le fichier est le binaire principal de **Moonstone — A Hard Days Knight**
(Mindscape, 1991), désassemblé en source 68000 (syntaxe Motorola, dialecte
vasm/devpac).

Caractéristiques globales :

- **Cible** : Amiga OCS, 68000, 1 Mio RAM (chip + fast), démarrage *boot-block*
  ou exécutable Amiga « hunk » avec section overlay.
- **Mode** : programme « hardware-bashing » — il prend le contrôle total de la
  machine. **Aucun appel** à `exec.library`, `dos.library`, `intuition.library`
  ou `graphics.library` n'est présent (vérifié par `grep`). Toutes les I/O
  sont faites directement sur les registres Custom Chip et CIA.
- **Affichage** : 320 × 200, 5 plans bitmap (32 couleurs), PAL standard. Voir
  §6.2.
- **Audio** : lecteur de modules SoundTracker / NoiseTracker 31 instruments
  (cf. §6.3), chargé depuis `music.cmp` / `vmusic.cmp`.
- **I/O disque** : pilote *trackdisk* maison (MFM brut, sync `$4489`, sans
  trackdisk.device), capable de lire des disquettes au format propriétaire
  Mindscape ; voir §6.4.
- **Décompression** : trois algorithmes distincts sont présents dans `program.asm` :
  - **RNC ProPack type 1** (`LAB_0190`) — utilisé pour `music.cmp` / `vmusic.cmp` (Huffman + magic `$524E4301`)
  - **LZSS** (`LAB_049C`) — utilisé pour les sprites `.cel` (fenêtre 2 Ko, longueur 2..34 octets)
  - **Bitplane RLE** (`LAB_0448` / `SECSTRT_21`) — utilisé pour les décors `.stile` (opcodes 2 bits)
  Les fonds d'écran `.PIV` sont au format IFF/ILBM avec PackBits standard (décodé par `LAB_0434`).
  Les fichiers `.CEL` sont des sprites propriétaires avec en-tête décrit en §10.16.
- **Pas d'auto-modification de code** identifiée (à confirmer pour les
  patchs de copper list / blitter list construits dynamiquement, qui ne
  sont pas du « SMC » au sens strict).

---

## 2. Carte des sections (mémoire logique)

Le source contient 34 sections (`SECTION S_0` … `S_33`). Découpage typique
d'un exécutable Amiga « hunk » où chaque section devient un *hunk*.
Tailles approximatives (lignes ASM, indicatif uniquement) :

| Sect | Type           | Lignes        | Lignes ≈ | Rôle inféré                                                                  |
|------|----------------|---------------|----------|------------------------------------------------------------------------------|
| S_0  | CODE           | [108–1080](program.asm#L108-L1080)        |  973 | **Point d'entrée / orchestration globale** (§3)                              |
| S_1  | CODE,**CHIP**  | [1081–1664](program.asm#L1081-L1664)      |  584 | **Player audio** (SoundTracker 31-instruments, §6.3)                         |
| S_2  | DATA           | [1665–2744](program.asm#L1665-L2744)      | 1080 | Textes : crédits, titres, chaînes UI                                          |
| S_3  | BSS            | [2745–2773](program.asm#L2745-L2773)      |   29 | Variables globales générales (`LAB_011A`–`LAB_0124`)                          |
| S_4  | CODE           | [2776–3119](program.asm#L2776-L3119)      |  344 | **Loader / relocator de hunks** Amiga (§3.2)                                  |
| S_5  | DATA           | [3122–3138](program.asm#L3122-L3138)      |   17 | Constantes du loader (taille hunks)                                          |
| S_6  | DATA           | [3141–3159](program.asm#L3141-L3159)      |   19 | Pointeurs du loader                                                          |
| S_7  | BSS            | [3162–3164](program.asm#L3162-L3164)      |    3 | Buffer 512 octets (`DS.L 128`)                                              |
| S_8  | CODE           | [3168–3941](program.asm#L3168-L3941)      |  774 | **Table de fichiers + dispatcher de chargement** (§6.1)                       |
| S_9  | BSS            | [3944–3967](program.asm#L3944-L3967)      |   24 | Buffers de chargement / état UI                                              |
| S_10 | CODE           | [3971–5004](program.asm#L3971-L5004)      | 1034 | **Gestionnaire de jobs / events / animations** (§5)                            |
| S_11 | BSS            | [5007–5062](program.asm#L5007-L5062)      |   56 | État du job manager                                                          |
| S_12 | CODE           | [5066–5288](program.asm#L5066-L5288)      |  223 | Routines diverses ; *(?)* gestion d'inventaire / objets                         |
| S_13 | CODE           | [5292–6161](program.asm#L5292-L6161)      |  870 | **Pilote trackdisk MFM** (§6.4)                                              |
| S_14 | DATA           | [6164–6173](program.asm#L6164-L6173)      |   10 | État courant de la disquette (numéro de piste, etc.)                          |
| S_15 | CODE           | [6177–6589](program.asm#L6177-L6589)      |  413 | **Pilote interruptions** (INT1-6) + clavier + joystick + VBL (§6.5)            |
| S_16 | DATA           | [6592–6637](program.asm#L6592-L6637)      |   46 | Drapeaux interruptions ; pointeurs handlers                                  |
| S_17 | BSS            | [6641–6656](program.asm#L6641-L6656)      |   16 | Buffer scancodes (table 128 octets `LAB_036C` etc.)                          |
| S_18 | CODE           | [6660–7327](program.asm#L6660-L7327)      |  668 | **Cache de fichiers** : hash de nom, table d'allocations (§6.1)                |
| S_19 | BSS            | [7330–7356](program.asm#L7330-L7356)      |   27 | Table de hachage / *file directory cache*                                    |
| S_20 | CODE           | [7360–7920](program.asm#L7360-L7920)      |  561 | **Logique de jeu — combat / mouvement personnage** *(?)*                       |
| S_21 | CODE           | [7923–8790](program.asm#L7923-L8790)      |  868 | **Logique de jeu — IA / scripts ennemis** *(?)*                                |
| S_22 | DATA           | [8793–8800](program.asm#L8793-L8800)      |    8 | Petite table de constantes                                                   |
| S_23 | CODE           | [8804–9352](program.asm#L8804-L9352)      |  549 | **Moteur sprite IMAGEXCEL** (CEL renderer, blitter, §6.2)                     |
| S_24 | DATA           | [9355–9371](program.asm#L9355-L9371)      |   17 | État du blitter (modulos, sauvegardes)                                       |
| S_25 | CODE           | [9375–9642](program.asm#L9375-L9642)      |  268 | **Initialisation IMAGEXCEL** + double-buffer setup                            |
| S_26 | DATA           | [9645–9824](program.asm#L9645-L9824)      |  180 | Texte copyright IMAGEXCEL + table de bit-reverse                              |
| S_27 | BSS,**CHIP**   | [9828–9832](program.asm#L9828-L9832)      |    5 | *(?)* Buffer de travail blitter / sprite                                      |
| S_28 | DATA           | [9836–9893](program.asm#L9836-L9893)      |   58 | Petites tables (probable état IMAGEXCEL)                                     |
| S_29 | CODE           | [9897–10434](program.asm#L9897-L10434)    |  538 | **Initialisation custom-chip globale** : copper, DMACON, bitplanes (§6.5)     |
| S_30 | DATA,**CHIP**  | [10437–10472](program.asm#L10437-L10472)  |   36 | **Bitmap principal** ($1F40 × 5 plans) + copper list                          |
| S_31 | CODE           | [10475–11257](program.asm#L10475-L11257)  |  783 | **Cycling palette / fade / ramp volumes audio** (§6.3, §6.5)                  |
| S_32 | DATA           | [11260–11276](program.asm#L11260-L11276)  |   17 | Constantes palette                                                           |
| S_33 | BSS            | [11279–11319](program.asm#L11279-L11319)  |   41 | Variables globales fin                                                       |

Total CODE ≈ 9 250 lignes ; DATA ≈ 1 470 ; BSS ≈ 200.

### 2.1 Mapping mémoire physique (extrait du fichier)

Tête de fichier ([program.asm#L4-L37](program.asm#L4-L37)) :

| Symbole         | Adresse      | Sens                                                              |
|-----------------|--------------|-------------------------------------------------------------------|
| `AUTO_INT1..6`  | `$64`–`$78`  | Vecteurs d'autovecteurs 68000 (interruptions niveau 1 à 6)         |
| `TRAP_15`       | `$BC`        | Vecteur trap #15                                                  |
| `EXT_0007`      | `$3E0`       | *(?)* Variable d'environnement Amiga / vecteur ROM                 |
| `EXT_0008..b`   | `$3F0`–`$3FE`| Adresses spéciales du loader (`hunk_overlay`-like)                 |
| `EXT_000c`      | `$400`       | Adresse de saut « bootstrap final »                               |
| `EXT_000d`      | `$3820`      | *(?)*                                                              |
| `EXT_000e`      | `$614A`      | *(?)*                                                              |
| `EXT_000f`      | `$6BEFA`     | **Base buffer écran travail** (clear `$6BEFA → $80000`, §6.2)      |
| `EXT_0010..23`  | `$7F682+`    | **Slots de la copper list** : pointeurs BPL1PTH/L à BPL5PTH/L      |
| `EXT_0014..1f`  | `$7F6AA+`    | Slots BPLxPT mis à jour à chaque flip                              |
| `EXT_0020..23`  | `$7FFCA+`    | *(?)* État du trackdisk                                            |
| `CIAA_*`        | `$BFE001+`   | Ports CIA-A (clavier, joystick fire, parallel low)                |
| `CIAB_*`        | `$BFD000+`   | Ports CIA-B (moteur floppy, sélection lecteur, série)             |
| `HARDBASE`      | `$DFF000`    | Custom Chips Agnus/Denise/Paula                                   |

La copper list réside à **`$7F6AE`** (cf. [program.asm#L9964](program.asm#L9964) :
`MOVE.L #$0007f6ae,COP1LCH`). Le bitmap principal est en chip RAM dans
`SECSTRT_30` (`SECTION S_30,DATA,CHIP`).

---

## 3. Point d'entrée et séquence d'initialisation

### 3.1 `SECSTRT_0` — entrée du programme

[program.asm#L108-L170](program.asm#L108-L170)

```text
SECSTRT_0:
    MOVE.L  A1,LAB_00C2          ; sauvegarde args reçus de DOS (A0/A1/D0/D1)
    MOVE.L  D1,LAB_00C3
    MOVE.L  A0,LAB_00C4
    MOVE.L  D0,LAB_00C5
    JSR     LAB_038F             ; init cache de fichiers (S_18)
    MOVE.W  EXT_0007,LAB_0005    ; lit drapeau hardware ($3E0)
    JSR     SECSTRT_29           ; INIT CUSTOM-CHIP / video / IRQ      (§6.5)
    JSR     SECSTRT_25           ; INIT IMAGEXCEL sprite engine         (§6.2)
    JSR     LAB_0006             ; ?? clear screen / pose palette
    JSR     LAB_0044             ; init random / divers
    JSR     SECSTRT_10           ; INIT job manager / event tables      (§5)
    LEA     LAB_0274,A0
    JSR     SECSTRT_31           ; INIT engine palette/volume ramp      (§6.3)
    LEA     LAB_011B,A0
    MOVE.L  #LAB_0014,(A0)
    JSR     LAB_0051             ; ??
    MOVE.W  LAB_0005,D0
    ANDI.W  #$0080,D0
    CMP.W   #$0080,D0
    BEQ.W   LAB_0001             ; branche "diag" si bit 7 de EXT_0007 = 1
    ...
    JSR     LAB_025F             ; → boucle d'attente VBL              (§5)
    JSR     LAB_0185             ; INTRO / loader cinématique          (§4)
    TST.W   LAB_05E7
    BNE.W   LAB_0000             ; sortie si flag erreur
    JSR     LAB_05A5             ; ?? préparation niveau
    MOVE.L  #$00000004,LAB_0123  ; mode jeu = 4
    JSR     LAB_001B / LAB_001C / LAB_0174 / ...   ; chaînes init modules
    LEA     LAB_00AA,A0
    JSR     LAB_0054             ; charge un asset par nom
    MOVE.L  #$000001A4,D0
    JSR     LAB_054F             ; wait(420 ticks ?)
    JSR     LAB_005B             ; STOP music (clear AUD volumes)
LAB_0000:
    LEA     LAB_0004,A0          ; "Mog" + saut sur SECSTRT_4 → relance hunk-loader
    JMP     SECSTRT_4
```

**Constatation importante** : `JMP SECSTRT_4` à la fin pointe vers le
*loader de hunks* qui peut **recharger un nouvel exécutable** : c'est le
mécanisme d'**overlay** Amiga, le binaire utilise probablement la version
*overlay* du protocole hunk (les hunks $3F0 / $3F2 sont reconnus en
[program.asm#L2952-L2977](program.asm#L2952-L2977)).

### 3.2 `SECSTRT_4` — loader de hunks (overlay manager)

[program.asm#L2776-L3119](program.asm#L2776-L3119)

Le code reconnaît explicitement les identifiants Amiga hunks :

| Valeur     | Hunk Amiga          | Action                                                |
|------------|---------------------|-------------------------------------------------------|
| `$3E9`     | `HUNK_CODE`         | Charge bloc, mémorise taille                          |
| `$3EA`     | `HUNK_DATA`         | Charge bloc, mémorise taille                          |
| `$3EB`     | `HUNK_BSS`          | Alloue (NOP physique, juste taille)                   |
| `$3EC`     | `HUNK_RELOC32`      | Applique table de relocations 32 bits                 |
| `$3F0`     | `HUNK_OVERLAY`      | Démarre la table d'overlays                           |
| `$3F2`     | `HUNK_END`          | Fin de unité                                          |
| `$3EB`/`5` | *fast/chip mem hint*| Le bit 30 du mot taille indique chip ou fast          |

Voir [program.asm#L2952-L3014](program.asm#L2952-L3014). La routine sait
allouer en chip RAM (`LAB_0155`) ou en fast RAM (`LAB_0154`) selon le bit 30
du mot taille, exactement comme `LoadSeg()`.

C'est donc un **runtime exécutable Amiga complet ré-implémenté**, capable
de charger en mémoire les segments suivants depuis la disquette en bypassant
totalement l'OS.

---

## 4. Boucle principale / orchestration

La « boucle principale » de Moonstone est en réalité une **machine à états
pilotée par un job-manager** :

1. `LAB_0123` est la variable d'état (mode courant : intro=2, jeu=4, etc.).
2. `SECSTRT_10` enregistre dans `LAB_0288` une *vtable* de fonctions
   ([program.asm#L3971-L4002](program.asm#L3971-L4002)) — chaque slot
   (offset 0, 4, 8, … 84) pointe vers `LAB_0215`, `LAB_0218`, `LAB_021A` …
   `LAB_023F`. Ce sont les **handlers de message** du gestionnaire d'objets.
3. `LAB_0282` est un tableau de **40 entrées de 0x2A octets** = pool
   d'« acteurs / sprites animés » :
   - offset 0  : actif (`B`)
   - offset 1  : *en cours* (`B`)
   - offset 2-5: pointeur script
   - offset 6-7: coord X
   - offset 8-9: coord Y
   - offset 10-11: vélocité/anim
   - offset 22 : direction
   - offset 24-27: ID
   - offset 28-31: pointeur cible
   - offset 32 : ?
   - offset 36-39: pointeur frame courante
   - offset 40 : flag
4. `LAB_025F` (appelé partout) est le **point de synchronisation VBL** :
   il attend `LAB_0367 == 0` qui est réinitialisé par INT3 (§6.5).

### 4.1 Job manager — `SECSTRT_10`

Voir [program.asm#L4060-L4140](program.asm#L4060-L4140).

Fonctions exposées :

- `LAB_01D5` : créer un nouvel acteur (cherche slot libre)
- `LAB_01D7` : remplacer l'animation d'un acteur existant
- `LAB_01DA` : variante « find by free + assign »
- `LAB_01DE` : recherche d'acteur par ID
- `LAB_01E1+` : EORI sur flag 40 → toggle visibility (?)

Le moteur tourne ces 40 entrées à chaque frame, exécute le script attaché
(sortes de tokens : `MOVE_TO`, `WAIT`, `CHANGE_ANIM`, `KILL`, …).

---

## 5. Modèle d'exécution / synchronisation

### 5.1 Liste de hooks VBL — `LAB_0372`

Plusieurs sections enregistrent à l'init un callback dans la table
`LAB_0372` :

- `SECSTRT_1` (audio) — [program.asm#L1086-L1095](program.asm#L1086-L1095)
- `SECSTRT_31` (cycling/ramp) — [program.asm#L10481-L10489](program.asm#L10481-L10489)
- `SECSTRT_15` (« vrai » VBL service) — appelée depuis INT3
  [program.asm#L6341-L6342](program.asm#L6341-L6342)

À chaque VBL (INT3 = `LAB_0331`), `SECSTRT_15` (à ne pas confondre avec sa
section) parcourt `LAB_0372` et exécute tous les callbacks tant que
l'entrée est non nulle ([program.asm#L6219-L6219](program.asm#L6219-L6219)
et boucle en [program.asm#L6181-L6197](program.asm#L6181-L6197)).

→ équivalent en C : registre `vbl_callback_t cbs[N]` exécuté en début de
frame.

### 5.2 Compteur de temps

`LAB_0364` = compteur décrémenté à chaque VBL ; `LAB_054F` (souvent appelé)
exécute essentiellement `LAB_0364 = D0 ; while (LAB_0364 != 0) ;`. C'est
le *« sleep ticks »* en pas de 1/50e (PAL) du jeu.

---

## 6. Sous-systèmes hardware

### 6.1 Chargement de fichiers

#### 6.1.1 Table des noms — `SECSTRT_8`

[program.asm#L3168-L3220](program.asm#L3168-L3220)

L'octet brut est de l'ASCII de noms de 8 caractères (`au1.cel\0`,
`co1.cel\0`, `da1.cel\0`, `dw1.cel\0`, `ha1.cel\0`, `li1.cel\0`,
`ov1.cel\0`, `dg1.cel\0`, `kn1.ob\0\0`, …). IRA les a interprétés à tort
comme des instructions, mais en lisant les octets c'est bien la table de
noms du répertoire workspace (au1.cel à vmusic.cmp).

Chaque entrée fait 8 octets, alignée. La routine `LAB_0174`
([program.asm#L3294-L3315](program.asm#L3294-L3315)) prend la cellule
courante :

```text
LAB_0174:
    MOVEA.L LAB_00CB,A0         ; source: nom de fichier
    MOVEA.L LAB_00C8,A1         ; destination: zone mémoire
    JSR     LAB_0268+2          ; "ouvrir + lire"
    ...
```

C'est donc le **dispatcher de chargement** : « charger l'asset N° X dans
la zone N° Y ».

#### 6.1.2 Cache de fichiers — `SECSTRT_18` / `SECSTRT_19`

[program.asm#L6736-L6800](program.asm#L6736-L6800)

Calcule un **hash sur le nom** (multiplicateur 13, masque `$07FF`),
puis `DIVU #$0048,D1` → bucket sur 72 entrées. La table de buckets est
en `SECSTRT_19` (BSS). Chaque entrée contient probablement :

- nom (jusqu'à 8 octets)
- pointeur mémoire / longueur
- état (chargé / sale / libre)

→ en C : `struct file_cache { char name[8]; void *data; u32 len; u16 state; };`
avec 72 buckets en hash chainé.

L'API exposée se reconnaît à la signature « A0 = nom string » :

| Routine    | Rôle inféré                                                       |
|------------|-------------------------------------------------------------------|
| `LAB_038F` | reset/initialisation du cache (appelée par S_0)                   |
| `LAB_0390` | charger/ouvrir le fichier nommé en A0, retourne handle en D0      |
| `LAB_03A1` | libérer le fichier                                                |
| `LAB_03B2` | lire `D0` octets depuis le fichier (handle en A0) vers buffer     |
| `LAB_03C5` | seek / skip *(?)*                                                  |

Les codes magiques `$03E9`, `$03EA`, `$03EB`, `$03EC`, `$03F0`, `$03F2`
réapparaissent ici → confirmant que le **cache de fichiers réimplémente
LoadSeg sur les hunks** ([program.asm#L2952-L3014](program.asm#L2952-L3014)
et [program.asm#L3140](program.asm#L3140)).

### 6.2 Affichage / sprite engine

#### 6.2.1 Configuration initiale — `SECSTRT_29`

[program.asm#L9962-L9982](program.asm#L9962-L9982) :

```text
MOVE.W  #$5200,BPLCON0     ; 5 bitplanes, BURST
MOVE.W  #$0000,BPLCON1
MOVE.W  #$0000,BPL1MOD
MOVE.W  #$0000,BPL2MOD
MOVE.W  #$0038,DDFSTRT
MOVE.W  #$00D0,DFFSTOP
MOVE.W  #$2C81,DIWSTRT     ; 320x256 → 320x200 fenêtre
MOVE.W  #$F4C1,DIWSTOP
MOVE.L  #$0007F6AE,COP1LCH ; copper list fixée à $7F6AE
MOVE.W  COPJMP1,D0
MOVE.W  #$8380,DMACON      ; BLIT + COPPER + SPRITE
MOVE.W  #$8040,DMACON      ; +AUD0
MOVE.W  #$8020,DMACON      ; +AUD1
MOVE.W  #$8400,DMACON      ; +BPLEN/DSK
```

Résolution : **320 × 200 × 5 plans = 32 couleurs**, palette dans la
copper list.

Adressage des plans en chip RAM :

```text
base  = SECSTRT_30
plane k (k=0..4) = base + k*$1F40    (8000 octets = 320/8 × 200)
```

Les pointeurs sont injectés à chaque frame dans la copper list à
`EXT_0014..EXT_001f` = `$7F6AA..$7F6D4`
([program.asm#L9933-L9961](program.asm#L9933-L9961)).

#### 6.2.2 Renderer IMAGEXCEL — `SECSTRT_23` / `SECSTRT_25`

Le copyright en clair l'annonce :
[program.asm#L9646-L9672](program.asm#L9646-L9672) :

```text
DC.B "IMAGEXCEL Code Module: SPRITE, Copyright 1988 by IMAGEXCEL Ve"
DC.B "ndor Code: ITE, copyright 1988 by IMAGEXCEL Programmer 1"
```

**IMAGEXCEL** est un middleware Mindscape utilisé pour le rendu sprite.
Les routines clé :

- `SECSTRT_25` (`LAB_046F` + co) — init : alloue les buffers blitter
  ([program.asm#L9378-L9388](program.asm#L9378-L9388))
- `LAB_04A7` — set viewport (clip rect)
- `LAB_04A8`/`LAB_04B4`/`LAB_04B5` — **draw cel** : prend en entrée
  - D0 : index de frame dans le fichier CEL
  - D1, D2 : coordonnées (x*8 + bit_offset, y)
  - A0 : pointeur entête CEL
- `LAB_04B0` — **génération de la table reverse-bits** (256 entrées
  octet → octet bit-réversé) à `LAB_04B3`
- `LAB_04CC`/`LAB_04CF` — kernels blitter : cookie-cut, masque
- `LAB_04D7` — *blitter-wait* (`BTST #6,DMACONR`)

Le mode blitter configuré est typiquement :
- `BLTCON0 = $0FF0 + canaux` (logic `D = (A&B)|(C&!B)` selon code de
  shift / inv)
- `BLTCON1 = 0` (rectangle), parfois mode ligne pour fill
- `BLTAFWM/BLTALWM = $FFFF` (masques bord adaptés au clipping)
- `BLTxMOD = 2` ou modulo de la largeur dest

Le rendu se fait en 5 passes (une par plan), `LAB_0504` = ligne stride en
octets.

#### 6.2.3 Format CEL

Reconstitué d'après [program.asm#L8830-L8870](program.asm#L8830-L8870) :

```c
struct cel_file {
    u16 frame_count;             // nombre de frames
    u32 data_offset;             // décalage début des données pixel
    u8  reserved[4];             // header pad

    struct frame_index {
        u32 data_offset;         // depuis data_offset
        u16 width;               // largeur en pixels
        u16 height;              // hauteur en lignes
        u8  planes_or_flags;     // bits = sélection plans actifs
        u8  draw_flags;          // & 0x01 = "cached" toggle
        u8  blit_minterm;        // logique blitter à appliquer
    } frames[frame_count];
    // ... pixel data, planar, 5 plans interleaved row-by-row
};
```

#### 6.2.4 Inventaire des fichiers — format et compression

> Les magic bytes ont été vérifiés directement sur les fichiers (`xxd -l 4`).
> Les tailles raw attendues sont calculées pour 320×200 pixels.

| Fichier | Taille (oct.) | Magic (hex) | Format | Compressé | Algorithme / routine |
|---------|---------------|-------------|--------|-----------|----------------------|
| `au1.cel` | 31 289 | `005c 0000` | Sprite CEL Mindscape — 0x5c=92 frames | ✅ Oui | LZSS → `LAB_049C` |
| `bg1a.PIV` | 15 742 | `0005 0000` | Fond PIV Mindscape — 5 plans (32 couleurs) | ✅ Oui | PackBits → `LAB_0434` |
| `bg1b.PIV` | 28 190 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg1c.PIV` | 16 828 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg2.PIV` | 17 571 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg2a.PIV` | 21 170 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg3.PIV` | 20 984 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg4.PIV` | 11 082 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg5.piv` | 16 586 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg5a.PIV` | 25 442 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bg7.PIV` | 13 969 | `0004 0000` | Fond PIV Mindscape — 4 plans (16 couleurs) | ✅ Oui | PackBits → `LAB_0434` |
| `bg8.PIV` | 13 194 | `0005 0000` | Fond PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `bold.f` | 12 133 | `004c 0000` | Police bitmap Mindscape — 0x4c=76 glyphes | ✅ Probable | LZSS → `LAB_049C` (même famille que `.cel`) |
| `co.stile` | 960 | `0000 0000` | Tileset décor (combat) | ✅ Oui | Bitplane RLE 2 bits → `LAB_0448` |
| `co1.cel` | 8 652 | `0019 0000` | Sprite CEL Mindscape — 0x19=25 frames | ✅ Oui | LZSS → `LAB_049C` |
| `Crystal` | 17 424 | `0000 03f3` | Amiga Hunk overlay (Crystal engine?) | ❌ Non | binaire brut |
| `da1.cel` | 4 440 | `0034 0000` | Sprite CEL Mindscape — 0x34=52 frames | ✅ Oui | LZSS → `LAB_049C` |
| `dg1.cel` | 22 377 | `0037 0000` | Sprite CEL Mindscape — 0x37=55 frames | ✅ Oui | LZSS → `LAB_049C` |
| `dw1.cel` | 17 820 | `0035 0000` | Sprite CEL Mindscape — 0x35=53 frames | ✅ Oui | LZSS → `LAB_049C` |
| `ha1.cel` | 11 898 | `0016 0000` | Sprite CEL Mindscape — 0x16=22 frames | ✅ Oui | LZSS → `LAB_049C` |
| `intro.stile` | 960 | `0000 0001` | Tileset décor (intro) | ✅ Oui | Bitplane RLE 2 bits → `LAB_0448` |
| `iraosx` | — | — | Outil macOS (désassembleur IRA v2.11) | N/A | hors jeu |
| `Klift1.CEL` | 12 927 | `0037 0000` | Sprite CEL Mindscape — 0x37=55 frames | ✅ Oui | LZSS → `LAB_049C` |
| `kn1.ob` | 5 | `6c697374` (`"list"`) | Données objet chevalier (stub IFF LIST ?) | ❌ Non | binaire brut |
| `li1.cel` | 14 724 | `001e 0000` | Sprite CEL Mindscape — 0x1e=30 frames | ✅ Oui | LZSS → `LAB_049C` |
| `message.piv` | 3 694 | `0004 0000` | Message PIV Mindscape — 4 plans | ✅ Oui | PackBits → `LAB_0434` |
| `mindscape` | 16 473 | `0005 0000` | Logo splash PIV Mindscape — 5 plans | ✅ Oui | PackBits → `LAB_0434` |
| `mog` | 172 260 | `0000 03f3` | Amiga Hunk overlay (jeu interactif) | ❌ Non | binaire brut |
| `music.cmp` | 88 187 | `524e4301` (`"RNC\x01"`) | Module SoundTracker compressé RNC | ✅ Oui | RNC ProPack 1 → `LAB_0190` |
| `nb` | 15 732 | `0000 03f3` | Amiga Hunk bootstrap — trackloader + couche HAL hardware (§6.6) | ❌ Non | binaire brut |
| `ov1.cel` | 11 357 | `0004 0000` | Sprite CEL Mindscape — 0x04=4 frames | ✅ Oui | LZSS → `LAB_049C` |
| `program` | 60 472 | `0000 03f3` | Amiga Hunk principal (intro + loader) | ❌ Non | binaire brut |
| `vmusic.cmp` | 60 994 | `524e4301` (`"RNC\x01"`) | Module SoundTracker compressé RNC | ✅ Oui | RNC ProPack 1 → `LAB_0190` |

**Notes sur les formats :**

- **Magic `0000 03f3`** = magic Amiga Hunk (exécutable/overlay AmigaOS). Ces fichiers sont chargés par le loader hunk de `program` (`SECSTRT_4`) et exécutés directement en mémoire chip/fast — **aucune décompression**.
- **Magic `005x 0000` / `001x 0000`** = en-tête propriétaire Mindscape CEL. Le premier mot (`word[0]`) encode le nombre de frames de l'animation. Format **non standard** (pas IFF). Compressé en LZSS frame par frame.
- **Magic `0005 0000` / `0004 0000`** (fichiers `.PIV`) = en-tête propriétaire Mindscape PIV. Le premier mot encode le **nombre de plans** (4 ou 5). Format **non IFF** malgré l'utilisation de PackBits en body. Une image 320×200×5 plans non compressée ferait ~40 000 octets ; les tailles observées (11–28 Ko) confirment la compression.
- **Magic `524e4301`** = `"RNC\x01"` = signature RNC ProPack type 1 sans ambiguïté. Header 18 octets (taille décompressée, taille compressée, CRC16).
- **`kn1.ob`** (5 octets, `"list"`) = fichier de 5 octets, très probablement un stub ou un résidu de développement (les 5 octets = `6c 69 73 74` + un octet de donnée).

#### 6.2.5 Cycling palette — `SECSTRT_31`

[program.asm#L10475-L10650](program.asm#L10475-L10650). Manipule 32
mots à partir de `LAB_05D2` (palette courante), avec :

- `LAB_05CF`/`LAB_05D0`/`LAB_05D1` : pointeur palette cible + delay + idx
- `LAB_05D3` : 6 slots de **rotations** (cycling) — `[from,to,dir,period,
  cur]`
- `LAB_05D4` : 6 slots de **fades** (ramp couleur par couleur) — la
  routine `LAB_058B` interpole indépendamment les composantes R/G/B
  ([program.asm#L10657-L10696](program.asm#L10657-L10696)).
- À la fin, `MOVE.L (A0)+,(A1)+` écrit dans `COLOR00..COLOR1F` à chaque
  VBL ([program.asm#L10637-L10641](program.asm#L10637-L10641)).

En parallèle, **`LAB_0592`** ([program.asm#L10685-L10720](program.asm#L10685-L10720))
implémente une rampe analogique des 4 volumes audio (`AUDxVOL`), pour
fade-in/fade-out musique.

### 6.3 Audio — `SECSTRT_1` (CHIP)

[program.asm#L1081-L1664](program.asm#L1081-L1664)

Indices forts d'un **lecteur SoundTracker 31-instruments** (NoiseTracker
compatible MOD) :

- Boucle `MOVEQ #30,D0` ([program.asm#L1146](program.asm#L1146)) →
  31 instruments.
- Décalage `#$3B8` ([program.asm#L1130](program.asm#L1130)) = 0x3B8 =
  **952** = taille de l'en-tête MOD (20 + 31×30 + 2 = 952).
- Décalage `#$43C` = `0x43C` = 1084 = offset des patterns (952 + 128 +
  4 = 1084) — signature MOD parfaite.
- 128 *song positions* (`MOVE.L #128,D0`).
- 4 canaux : `LEA AUD0LCH..AUD3LCH` ([program.asm#L1181-L1190](program.asm#L1181-L1190))
- Période note de SoundTracker : `MOVE.W 16(A6),D2` puis comparaison
  avec table `LAB_0095` longue de 37 mots = **table de périodes standard
  Paula** (37 notes : C-1 à B-3).
- Effets : `MOVE.B 3(A6)` puis split haut/bas + `DIVS #3` = identique au
  *« vibrato / portamento / volume slide »* tracker.
- Pas de signature « M.K. » dans le binaire ; les fichiers `.cmp` sont
  donc des dumps mémoire « pré-mâchés » (header propriétaire) plutôt que
  des `.MOD` standard.

API audio :

| Routine    | Rôle                                                                |
|------------|---------------------------------------------------------------------|
| `LAB_0061` | reset moteur audio, calcule pointeurs samples, charge tempo         |
| `LAB_005C` | hook VBL — décrément compteur (50 Hz → tempo), appelle `LAB_0065`   |
| `LAB_0065` | étape : effets sur chaque canal courant                             |
| `LAB_006D` | fetch nouvelle note (toutes les `LAB_0096` VBL)                     |
| `LAB_005B` | stop audio (clear `AUD0..3VOL`)                                     |

`LAB_0124` = pointeur module musical actuellement chargé.

### 6.4 Pilote disquette — `SECSTRT_13`

[program.asm#L5292-L6161](program.asm#L5292-L6161). C'est un **driver
trackdisk maison** qui :

1. **Pilote le moteur** via `CIAB_PRB` ([program.asm#L5310](program.asm#L5310)) :
   - bit 7 : `~MTR` (motor on/off)
   - bit 6 : `~SEL3..0`
   - bit 2 : `~SIDE`
   - bit 1 : `~DIR`
   - bit 0 : `~STEP`
2. **Lit l'ID lecteur** par série (technique standard CIA-A `PA5` =
   `~RDY`) ([program.asm#L6212-L6225](program.asm#L6212-L6225)).
3. Configure **DMA disk** (`DMACON $8210`), réveille la synchro
   (`DSKSYNC = $4489`, `ADKCON = $8100` MFM + WORDSYNC), lit une piste
   complète (`DSKLEN = $A800` = 0x800 mots = 11 secteurs MFM bruts).
4. Décode MFM en clair (routine `LAB_02FC` :
   `D2 = (A>>1)&$5555 | B & $5555`), sync sur motif `$AAAA / $4489 /
   $5555 / $5555`.
5. Cherche en mémoire les **headers de secteur** (offset 6 = numéro
   piste, offset 12 = checksum début).

Le format n'est **pas** AmigaDOS standard (qui aurait `MOVE.L
'AmigaDOS',D0` signature, et utilise OS). C'est un format custom Mindstone
avec encodage MFM brut.

API exposée (point d'entrée probable `LAB_02E9` ou `LAB_02EE`) :

- `LAB_02EE` : initialise le lecteur, calibre le pas (recherche piste 0)
- `LAB_02F3` : seek + read piste courante
- `LAB_02F6` : balaie les secteurs trouvés dans le buffer
- `LAB_02FF` : attente READY (`PA5` du CIAA)
- `LAB_0303` / `LAB_0307` : seek piste `D0`
- `LAB_0309` / `LAB_030C` : step in / step out (1 piste)
- `LAB_0316` : motor on
- `LAB_0315` : motor off / désélection

### 6.5 Interruptions et entrées utilisateur — `SECSTRT_15`

[program.asm#L6214-L6230](program.asm#L6214-L6230) installe :

| Vec     | Source              | Routine     | Rôle                                                |
|---------|---------------------|-------------|-----------------------------------------------------|
| INT1    | TBE/SERIAL/DSKBLK   | `LAB_0326`  | acquitte serial+softint+TBE                          |
| INT2    | **CIA-A keyboard**  | `LAB_032A`  | décode scancode du clavier, met à jour `LAB_036C..` |
| INT3    | COPPER/VBL/BLIT     | `LAB_0331`  | **frame tick** + appelle hooks `LAB_0372`           |
| INT4    | AUDIO 0-3           | `LAB_0337`  | non explicite (probable acquit + advance)           |
| INT5    | DSKSYNC/RBF         | `LAB_033C`  | acquit DSKSYNC                                       |
| INT6    | CIA-B/EXTER         | `LAB_033F`  | timers CIA-B                                         |

#### 6.5.1 Clavier (INT2 — `LAB_032A`)

[program.asm#L6280-L6320](program.asm#L6280-L6320). C'est le code clavier
Amiga canonique :

1. Lire `CIAA_SDR` (le scancode brut sériel arrivé).
2. Forcer SP en sortie + force-low + handshake (75 µs) puis remettre en
   entrée → acknowledge au clavier.
3. Inverser et ROR : `code = ~rotate(code)` → bit 7 = 0/1 release.
4. Stocker l'état dans `LAB_036D[scancode]` (table 128 octets).
5. Mémoriser le dernier scancode pressé dans `LAB_0362`.

#### 6.5.2 Joystick (lecture polling, dans VBL)

[program.asm#L6225-L6230](program.asm#L6225-L6230) :

```text
MOVE.B  JOY0DAT,LAB_0368
MOVE.B  EXT_0035,LAB_0369   ; EXT_0035 = $DFF00B = high byte JOY0DAT
```

Le joystick *joueur 1* est lu via `JOY0DAT`, les boutons via `POTGO`
(en cours de frame).

#### 6.5.3 Variables globales clés

| Symbole       | Adresse       | Rôle                                            |
|---------------|---------------|-------------------------------------------------|
| `LAB_0362`    | (BSS)         | dernier scancode pressé                         |
| `LAB_0363`    |               | flag fire bouton                                |
| `LAB_0364`    |               | sleep counter (VBL countdown)                   |
| `LAB_0367`    |               | flag « écran prêt »                             |
| `LAB_0368/9`  |               | JOY0DAT cached                                  |
| `LAB_0372`    | tab           | callbacks VBL                                   |
| `LAB_036C..D` | tab 128 oct.  | état des touches                                |
| `LAB_0373`    |               | flag « INT3 stack swap fait »                   |
| `LAB_0379`    |               | compteur frames                                 |

---

### 6.6 Module `nb` — Bootstrap, Trackloader et couche HAL

> Source : `nb.asm` — 3 354 lignes, 15 732 octets, magic `0000 03f3` (Amiga Hunk exécutable, non compressé).
> Désassemblé par IRA V2.11 ; 17 sections (S_0..S_16).

#### 6.6.1 Rôle général

`nb` est le **module de bootstrap** du jeu. Il est le **premier binaire
exécuté** après que le boot block Amiga l'a chargé depuis la disquette.
Son rôle est triple :

1. **Détection et initialisation du hardware** — sonde la RAM disponible,
   calcule les adresses des buffers, prend le contrôle du système
   (écrase les vecteurs d'interruption AmigaOS).
2. **Couche d'abstraction hardware (HAL)** — fournit les drivers disque,
   clavier, souris/joystick et audio aux binaires supérieurs.
3. **Trackloader / Hunk loader** — lit le fichier `"program"` depuis la
   disquette (décodage MFM, décodage de secteur), reloue son binaire Hunk
   en mémoire puis lui passe la main.

Séquence de démarrage :

```
Boot block Amiga
  └─► charge nb en mémoire
        └─► SECSTRT_0 : détecte RAM, installe drivers, prend le système
              └─► SECSTRT_14 : lit "program" depuis le disque (Hunk loader)
                    └─► JMP dans program → intro + jeu
```

#### 6.6.2 Détection mémoire et carte de la RAM (SECSTRT_0)

[nb.asm#L86](nb.asm#L86) — `SECSTRT_0` est l'**entry point** du binaire.

**Sonde de RAM** via `exec.library AllocMem()` (`JSR -216(A6)`,
`A6 = ABSEXECBASE = $4`) :

| Valeur `SECSTRT_1` | Signification                          |
|--------------------|----------------------------------------|
| `0`                | Seulement 512 Ko chip RAM              |
| `1`                | 1 Mo chip RAM (extended chip / ECS)    |
| `2`                | > 1 Mo (chip + fast RAM détecté)       |

Selon la configuration, cinq pointeurs de zone mémoire sont calculés en
soustrayant des offsets connus depuis `$0006B900` (512 Ko) ou `$000FB1E0`
(1 Mo+) :

| Symbole     | Rôle                                             |
|-------------|--------------------------------------------------|
| `LAB_0024`  | Buffer screen A (chip RAM, bitmap 320×200×5)     |
| `LAB_0025`  | Base de `SECSTRT_16` — zone fast RAM (~196 Ko BSS) où `program` sera chargé |
| `LAB_0026`  | Buffer screen B                                  |
| `LAB_0027`  | Buffer screen C / zone overlay                   |
| `LAB_0028`  | Buffer DMA disque (32 Ko en haut de chip RAM)    |
| `LAB_001E`  | Base de la zone chip RAM bitmaps                 |
| `LAB_001F`  | Sommet du fast RAM (base de pile initiale)       |

Validation de la mémoire chip disponible : écriture du sentinelle `0x55`
dans une boucle de scan ; en cas d'échec affiche `"FATAL: OUT OF LOAD
MEMORY"` (chaîne à [nb.asm#L350](nb.asm#L350)).

**Prise du système** : copie le stub `RTE` (`LAB_000F`) dans les 6 slots
`AUTO_INT1`–`AUTO_INT6`, puis installe les vrais gestionnaires d'interruptions.
À la fin, `JMP (A3)` = saut vers le code relocalisé en RAM.

#### 6.6.3 Architecture des interruptions

`nb` installe **six niveaux** d'interruptions AmigaOS :

| Niveau | Source hardware         | Handler `nb`  | Rôle                                                     |
|--------|-------------------------|---------------|----------------------------------------------------------|
| INT1   | TBE / DSKBLK / SOFTINT  | `LAB_0034`    | Acquitte INTREQ bits 0–2 ; efface `LAB_006E`             |
| INT2   | CIA-A (kbd, disk step)  | `LAB_0038`    | Lit CIA-A ICR ; si kbd prêt → `LAB_004D` (décodage touche) ; arme CIA-B timer |
| INT3   | VBL + COPPER + BLIT     | `LAB_003C`    | Swap de stack au premier VBL ; efface `LAB_0071` (VBL) ; appelle `LAB_0058` (souris) puis `SECSTRT_4` (callbacks de frame) |
| INT4   | Audio CH 0–3            | `LAB_0042`    | Dispatch vers vtable `LAB_0078..007B` (4 pointeurs, initialement = `LAB_006B` = RTS stub) |
| INT5   | DSKSYNC / RBF           | `LAB_0047`    | Acquitte bits 11–12 ; efface `LAB_0070`                  |
| INT6   | CIA-B SP (disk index)   | `LAB_004A`    | Lit CIA-B ICR timer4 ; efface `LAB_006F`                 |

La vtable audio (`LAB_0078`..`LAB_007B`) est **remplie par `program`** lors de
l'initialisation du player SoundTracker.

#### 6.6.4 Driver clavier (INT2 → `LAB_004D`)

[nb.asm#L530](nb.asm#L530) — déclenché sur CIA-A SDR (serial data ready).

1. Lire `CIAA_SDR` → scancode brut Amiga (8 bits).
2. `ROR.B #1` → code standard Amiga (bit 7 = 0 pression / 1 = relâchement).
3. Lookup dans `LAB_0076` (table 128 entrées, byte, code brut → index key).
4. Mise à jour `LAB_0077` (128 octets d'état : 1 = touche pressée, 0 = relâchée).
5. Clé spéciale `$FF` → bascule `LAB_006D` (flag caps-lock / shift).
6. En cas de pression → stocker l'index dans `SECSTRT_5` (dernière touche pressée).

`LAB_0069` : routine utilitaire — efface les 128 octets de `LAB_0077`
(reset complet du clavier).

| Symbole       | Rôle                                              |
|---------------|---------------------------------------------------|
| `LAB_0076`    | Table de décodage keycode → index (128 octets)    |
| `LAB_0077`    | État courant des touches (128 octets)             |
| `SECSTRT_5`   | Dernière touche pressée (word)                    |
| `LAB_006D`    | Flag caps-lock / shift                            |

#### 6.6.5 Driver souris et joystick (INT3/VBL → `LAB_0058`)

[nb.asm#L620](nb.asm#L620) — appelé à chaque VBL depuis `LAB_003C`.

**Souris (port 0 — `JOY0DAT`)** :

- Lit le registre `JOY0DAT` (encodeur quadrature 8 bits X + 8 bits Y).
- Calcule le delta signé (gestion du débordement modulo 256 : si delta > 128 → soustraire 256).
- Accumule dans `LAB_007F` (delta X) / `SECSTRT_6` (delta Y) puis applique le clamp :
  - X : `[0xFFF9 .. 0x00C7]` ≈ -7..199
  - Y : `[0xFFF9 .. 0x013F]` ≈ -7..319
- Bouton gauche : `BTST #6, CIAA_PRA` → `LAB_0080 = 0` (appuyé).

**Joystick (port 1 — `JOY1DAT`)** :

- Lit `JOY1DAT` + `CIAA_PRA` bit 7 (bouton fire).
- Encode vers `LAB_0082` : bits U/D/L/R/fire.

| Symbole       | Rôle                                                    |
|---------------|---------------------------------------------------------|
| `LAB_007F`    | Curseur souris X courant                                |
| `SECSTRT_6`   | Curseur souris Y courant                                |
| `LAB_0080`    | État bouton gauche (0 = pressé)                         |
| `LAB_0074`    | Flag mouvement souris                                   |
| `LAB_0082`    | État joystick 1 (bits : bas, haut, gauche, droite, feu) |

#### 6.6.6 Driver disque — Trackloader MFM

Le sous-système disque (`SECSTRT_7`, `LAB_00CC`, `LAB_00D2`, `LAB_00DA`..`LAB_00F4`)
est un **trackloader complet** qui opère directement sur le hardware Amiga sans
passer par le *trackdisk.device* AmigaOS.

##### Format de piste (MFM Amiga standard)

- Sync word : `$4489 $4489` (registre `DSKSYNC`)
- Secteur : 0x220 = 544 octets = `32` octets d'en-tête + `512` octets de données + `4` octets de checksum
- 11 secteurs par piste, 2 faces (side select via `CIAB_PRB` bit 2), ≥ 80 pistes

##### Lecture (LAB_00D2 / LAB_00D5 / LAB_00D8)

1. Effacer le buffer DMA (`LAB_0100` → `SECSTRT_2`, 20 Ko).
2. Programmer ADKCON ($8400) + `DSKSYNC = $4489`.
3. Écrire l'adresse du buffer dans `DSKPTH` ; activer DMACON `$8210`.
4. Déclencher deux écritures sur `DSKLEN` (bit 15 = start) ; attendre `LAB_006E = 0` (INT1 acquitte DSKBLK).
5. Vérifier que le sync `$4489` est bien présent dans le buffer → sinon retry.
6. Appeler `LAB_00DA` (décompte des 11 secteurs valides détectés).

##### Décodage MFM (LAB_00E0)

Encodage Amiga MFM = **bits impairs / pairs entrelacés** sur deux mots consécutifs :

```
donnée_décodée = ((mot_impair >> 1) & 0x5555) | (mot_pair & 0x5555)
```

`LAB_00E0(A0, A1, D0)` : décode `D0/2` mots source en `D0/2` mots destination.

##### Seek de piste (LAB_00E7 / LAB_00ED / LAB_00F0)

- `SECSTRT_8` = piste courante (word, `$FFFF` = inconnu / tête non positionnée)
- `EXT_0022` = piste cible
- `LAB_00ED` : step out (CIAB_PRB bit 1 → step, bit 0 = direction)
- `LAB_00F0` : step in
- `LAB_00F3` : attend CIA-A timer (délai entre pas = ~3 ms) via `CIAA_TALO/TAHI`
- `LAB_00E3` : attend disk ready (`CIAA_PRA` bit 5 = `/DSKRDY`)

##### Accès par secteur logique (LAB_0087 / LAB_00A2)

- `LAB_0087` (read) / `LAB_00A2` (write) : calculent face (`D1 = sector/11`), secteur (`EXT_0021 = sector % 22`) et piste (`EXT_0022`) à partir du numéro de secteur absolu passé en D0.
- Cherchent le secteur dans le buffer à `LAB_0100 + 30000` par comparaison de l'octet `6(entry)`.
- Sur checksum invalide : affiche `"Checksums"` (strings à `LAB_0091`/`LAB_0092`) et appelle `LAB_00FB` (long wait).

##### Formatage (LAB_00AE)

- Remplit le buffer de `$AAAA` (signal d'effacement MFM).
- Construit 12 entrées de secteur avec sync `$4489` via le blitter (`LAB_00B1`..`LAB_00B4`).
- Le blitter est utilisé pour l'**encodage MFM** : `LAB_00B3` configure les registres blitter (BLTCON0, BLTAFWM, longueurs, sources/destinations) pour encoder odd/even bits.

#### 6.6.7 Hunk loader — `SECSTRT_14`

[nb.asm#L3048](nb.asm#L3048) — appelé depuis `SECSTRT_0` après initialisation des drivers.

Charge et reloge le fichier `"program"` depuis le disque. Reconnaît les
types de hunks AmigaOS standard :

| Magic word  | Type Hunk           | Traitement                                          |
|-------------|---------------------|-----------------------------------------------------|
| `$000003F3` | HUNK_HEADER         | détermine l'espace à allouer, prépare `LAB_0026`    |
| `$000003E9` | HUNK_CODE           | copie dans la zone code (`LAB_0138`)                |
| `$000003EA` | HUNK_DATA           | copie dans la zone data                             |
| `$000003EB` | HUNK_BSS            | réserve (zeroing) dans la zone BSS                  |
| `$000003EC` | HUNK_RELOC32        | applique les relocations 32 bits (`LAB_01DF`)       |
| `$000003F0` | HUNK_SYMBOL         | ignore (`LAB_014B`)                                 |
| `$000003F2` | HUNK_END            | termine le hunk courant, passe au suivant           |
| autre       | erreur              | affiche `"Relocation Error"` + boucle infinie       |

Le programme cible est identifié par la chaîne `"program"` (LAB_01E9).
Le hash de recherche de fichier utilise le même algorithme que celui de
`program` : `h = (h * 13 + c) & 0x07FF ; h = h % 72`.

Après chargement complet, `SECSTRT_14` appelle `LAB_0160` puis `RTS` pour
rendre la main au code de démarrage qui effectue le `JMP` vers le point
d'entrée de `program`.

#### 6.6.8 Callbacks de frame — `SECSTRT_4`

[nb.asm#L384](nb.asm#L384) — même motif que `program.SECSTRT_10` : liste
de pointeurs de fonctions appelée à chaque VBL depuis `LAB_003C`.

`SECSTRT_4` contient une liste de slots `DC.L 0` ; les slots non nuls sont
appelés en séquence. `program` remplit ces slots lors de son initialisation
pour enregistrer ses propres hooks de frame.

#### 6.6.9 Variables globales clés de `nb`

| Symbole       | Section | Rôle                                                        |
|---------------|---------|-------------------------------------------------------------|
| `SECSTRT_1`   | S_1     | Configuration mémoire (0/1/2)                               |
| `SECSTRT_2`   | S_2 BSS CHIP | Buffer chip RAM — bitmaps 320×200×5 plans (~40 Ko)     |
| `SECSTRT_5`   | S_5     | Dernière touche clavier pressée (word)                      |
| `SECSTRT_6`   | S_6 BSS | Curseur Y souris courant                                    |
| `SECSTRT_8`   | S_8     | Piste disque courante (`$FFFF` = inconnue)                  |
| `SECSTRT_16`  | S_16 BSS | Zone fast RAM (196 Ko) — destination du chargement de `program` |
| `LAB_006D`    | S_5     | Flag caps-lock                                              |
| `LAB_006E`    | S_5     | Flag DSKBLK (mis à `$FFFF` par INT1 à la fin du DMA)       |
| `LAB_007F`    | S_6 BSS | Curseur X souris courant                                    |
| `LAB_0076`    | S_5     | Table de décodage clavier (128 octets)                      |
| `LAB_0077`    | S_5     | État des touches (128 octets, 1 = pressée)                  |
| `LAB_0078`..`LAB_007B` | S_5 | Vtable audio CH 0–3 (remplie par `program`)      |
| `LAB_0080`    | S_6 BSS | Flag bouton gauche souris (0 = appuyé)                      |
| `LAB_0082`    | S_6 BSS | État joystick 1 (bitfield U/D/L/R/feu)                      |
| `LAB_0100`    | S_8     | Pointeur vers `SECSTRT_2` (buffer DMA disque)               |

#### 6.6.10 Impact sur le portage

`nb` est **entièrement à remplacer** : aucune de ses fonctions n'a
d'équivalent direct sous un OS moderne. La substitution cible est :

| Fonction `nb`                       | Remplacement portable                                     |
|-------------------------------------|-----------------------------------------------------------|
| Détection mémoire / allocation      | Supprimé — allocation statique ou `malloc()` à l'init    |
| Interrupt handlers INT1–INT6        | Supprimé — remplacé par la boucle d'événements SDL2       |
| Driver clavier (`LAB_004D`)         | `SDL_Event` (`SDL_KEYDOWN`/`SDL_KEYUP`)                   |
| Driver souris (`LAB_0058`)          | `SDL_MouseMotionEvent` + `SDL_MouseButtonEvent`           |
| Driver joystick (`LAB_0053`–`0057`) | `SDL_JoystickGetAxis` / `SDL_GameControllerGetButton`     |
| Vtable audio (`LAB_0078`–`007B`)    | Callbacks SDL_audio ou libmodplug/openmpt                 |
| Trackloader MFM (`SECSTRT_7`)       | `fopen()` / `fread()` sur l'image disque ADF              |
| Hunk loader (`SECSTRT_14`)          | `libhunk` ou lecture directe du binaire recompilé         |
| VBL callbacks (`SECSTRT_4`)         | Timer SDL (`SDL_AddTimer`) ou boucle à 50 Hz              |

Dans `libmoon_assets` (voir §11), `nb` ne sera **pas porté en tant que
bibliothèque** : seules ses tables de données (`LAB_0076` = keymap Amiga,
`LAB_01BC` = adresses ROM) pourront être extraites comme références
documentaires si nécessaire.

---

## 7. Variables globales — vue d'ensemble

Sections BSS importantes :

- **S_3** : variables d'état du programme principal — `LAB_0123` =
  *current mode*, `LAB_011B..` = vtable courante.
- **S_9** : compteurs/flags du loader (S_8).
- **S_11** : état du job manager (40 acteurs).
- **S_17** : *keyboard state* (128 octets).
- **S_19** : *file cache directory*.
- **S_27** : *blitter work* (chip ram).
- **S_33** : variables divers fin de programme (audio/UI).

Total estimé en BSS : ≈ 4 Kio + bitmap en `S_27`/`S_30`.

---

## 8. Formats compressés et décompresseurs

> **Correction d'analyse initiale :** une première lecture avait conclu à l'absence
> de compression. L'analyse approfondie de `program.asm` a révélé **trois décompresseurs
> distincts** et un parseur IFF. Cette section annule et remplace la conclusion précédente.

### 8.1 Vue d'ensemble par type de fichier

> Pour le détail fichier par fichier (taille, magic bytes, nombre de frames…), voir **§6.2.4**.

| Extension | Format réel | Magic identifier | Compressé | Décompresseur | Taille raw vs compressée |
|-----------|-------------|-----------------|-----------|---------------|--------------------------|
| `.cmp` | RNC ProPack 1 | `524e4301` ("RNC\x01") | ✅ Oui | `LAB_0190` ([program.asm#L3617](program.asm#L3617)) | ~88 Ko → ~140 Ko (music) |
| `.cel` / `.CEL` | CEL Mindscape propriétaire | `word[0]` = nb frames | ✅ Oui | `LAB_049C` LZSS ([program.asm#L9210](program.asm#L9210)) | ratio ~0.4..0.6 selon sprite |
| `.stile` | Tileset RLE bitplane | `0000 0000`/`0001` | ✅ Oui | `LAB_0448` / `SECSTRT_21` ([program.asm#L7923](program.asm#L7923)) | 960 octets (petit dataset) |
| `.PIV` / `.piv` | PIV Mindscape propriétaire | `word[0]` = nb plans (4 ou 5) | ✅ Oui | `LAB_0434` PackBits maison ([program.asm#L7663](program.asm#L7663)) | ~11–28 Ko vs ~40 Ko raw |
| `.f` | Police bitmap Mindscape | `word[0]` = nb glyphes | ✅ Probable | `LAB_049C` LZSS (même famille CEL) | 12 Ko |
| `.ob` | Données objet (stub) | `"list"` | ❌ Non | — | 5 octets |
| sans ext. (hunks) | Amiga Hunk binaire | `0000 03f3` | ❌ Non | — (chargé brut par `SECSTRT_4`) | `program`=60 Ko, `mog`=172 Ko |

> ⚠️ **Correction :** les fichiers `.PIV` **ne sont pas au format IFF/ILBM standard**. Leur magic (`0x0005`/`0x0004`) ne correspond pas à `"FORM"` (`0x464F524D`). Il s'agit d'un format Mindscape propriétaire dont le body est compressé avec un algorithme PackBits maison (byte ≥ 0 : N+1 littéraux ; byte < 0 : répétition ; `$80` = NOP), parsé par `LAB_0434`. La confusion venait d'une analyse initiale du code avant vérification des magic bytes réels.

### 8.2 Pourquoi trois algorithmes ?

- **RNC ProPack** (musique) : outil standard Amiga très répandu en 1992, excellent ratio sur données séquentielles comme les modules tracker.
- **LZSS** (sprites) : compromis vitesse/ratio adapté aux données pixel avec beaucoup de répétitions locales ; décodage rapide sans table.
- **RLE bitplane** (décors) : exploite la nature bitplane des tiles — les plans nuls ou constants se compressent à 2 bits par ligne, gains importants sur les grandes zones vides de décor.
- **IFF/ILBM + PackBits** (fonds) : format standard Amiga produit directement par Deluxe Paint ; aucun décodeur maison requis.

### 8.3 Impact pour le portage

- `music.cmp` / `vmusic.cmp` : décompresser avec un décodeur RNC1 standard (bibliothèques disponibles) avant chargement du module tracker.
- `*.cel` : décompresser via le port de `LAB_049C` (LZSS, ~60 lignes ASM → ~30 lignes C).
- `*.stile` : décompresser via le port de `LAB_0448` (RLE 2 bits, ~80 lignes ASM).
- `*.PIV` : utiliser n'importe quelle bibliothèque IFF/ILBM existante (libiff, etc.).
- `*.ob` : copier directement en mémoire (données brutes structurées, voir §10.17).

---

## 9. Carte hiérarchique des appels (top-down)

```
SECSTRT_0  (entry)
├── LAB_038F                      init cache fichiers
├── SECSTRT_29                    init custom-chip / video / IRQ
│   ├── LAB_0325                  install INT1-6 + INTENA
│   └── LAB_0566/LAB_0567         clear chip RAM screen
├── SECSTRT_25                    init IMAGEXCEL
│   ├── LAB_04E3/LAB_04E4         alloc bitmap travail
│   ├── LAB_04EC/LAB_04EE         init clip viewport
│   └── LAB_046F
├── SECSTRT_10                    init job-manager
├── SECSTRT_31                    init palette cycling / volume ramp
├── LAB_0185                      INTRO / cinematique
│   ├── LAB_054F                  wait(N ticks)
│   └── LAB_05A5                  preload level
└── (boucle principale via LAB_0123 → vtable LAB_0288)
    ├── LAB_001B..LAB_002F        per-frame: input, update entities
    ├── LAB_0174                  load assets via cache
    └── LAB_025F                  wait VBL
        └── INT3 (LAB_0331)
            └── SECSTRT_15        run callbacks(LAB_0372)
                ├── audio LAB_005C → LAB_0065 → LAB_006D
                ├── palette SECSTRT_31 → LAB_058B
                └── input poll JOY0DAT, scancode table
```

Routines blitter (appelées partout depuis S_8 / S_20 / S_21) :

```
draw_cel(D0=frame, D1=x, D2=y, A0=cel)         = LAB_04B4
copy_rect(A0=src,A1=dst,D0=w,D1=h)              = LAB_04E1
blit_clear                                      = LAB_04C0..LAB_04C2
blit_wait                                       = LAB_04D7
```

---

## 10. Logique du jeu — analyse détaillée

### 10.1 Machine à états principale

`LAB_0123` (section S_2, [program.asm#L1803](program.asm#L1803)) est le registre de **mode de jeu** global :

| Valeur | Mode                                   |
|--------|----------------------------------------|
| `0`    | Overworld / écran titre                |
| `4`    | Jeu actif (combat en cours)            |

`LAB_011E` est le **type de scène courant** (nombre de plans bitmap actifs) :

| Valeur | Scène                                              | Nb plans | Nb couleurs |
|--------|----------------------------------------------------|----------|-------------|
| `2`    | Cinématique / cutscene                             | 4        | 16          |
| `3`    | Événement / rencontre                              | 4        | 16          |
| `4`    | Carte overworld ou boutique                        | 4        | 16          |
| `5`    | Combat                                             | 5        | 32          |

`LAB_011D` = pointeur vers l'un des 8 **contextes de palette** (`LAB_01CB`…`LAB_01D2`, section BSS S_9, [program.asm#L3944](program.asm#L3944)). Chaque contexte = 32 mots = 32 couleurs Amiga. Quand on change de scène, on copie le contexte courant dans `COLOR00`…`COLOR1F` via la copper list.

---

### 10.2 Boucles de rendu principales

#### `LAB_0007` — boucle infinie de jeu ([program.asm#L178](program.asm#L178))

```text
LAB_0007:
    BSR  LAB_0017   ; sauvegarder timestamp frame
    JSR  LAB_01EC   ; tick entités actives (scripts + blit)
    JSR  LAB_01E8   ; spawn nouvelles entités depuis queue
    TST.W LAB_011F
    BNE  LAB_000A
    BSR  LAB_000F   ; switch décor si LAB_011F=0
LAB_000A:
    BSR  LAB_0018   ; wait(LAB_00D0 − frames_elapsed)
    JMP  LAB_0007
```

Elle tourne à l'infini jusqu'à ce que les scènes elles-mêmes modifient `LAB_011F` pour court-circuiter le switch, puis retournent (`RTS`) hors de la boucle via le mécanisme de call-stack.

#### `LAB_000B` — boucle limitée N frames ([program.asm#L208](program.asm#L208))

Identique à `LAB_0007` mais sort après `LAB_0028 >= LAB_002B+2` frames (le compteur `LAB_0028` est incrémenté à chaque tour de boucle). Utilisée pour les cinématiques de durée fixe.

#### `LAB_0038` — boucle de 40 frames overworld ([program.asm#L669](program.asm#L669))

```text
MOVEQ #40,D0
LAB_0038_loop:
    PUSH D0
    JSR LAB_0017   ; timestamp
    JSR LAB_01E8   ; spawn
    JSR LAB_01EC   ; update
    JSR LAB_0262   ; flip double-buffer
    JSR LAB_0242   ; blit sprites
    JSR LAB_0018   ; wait
    POP  D0
    DBF  D0,LAB_0038_loop
```

`LAB_0242` ([program.asm#L4500](program.asm#L4500)) est le **blit de sprites** : il lit la liste de sprites compilée par `LAB_01EC` (stockée à `LAB_0279`/`LAB_027C`) et appelle `LAB_04E1` (blitter copy) pour chaque rectangle.

#### `LAB_000F` — aiguillage de décor ([program.asm#L228](program.asm#L228))

```text
CMP.W #2,LAB_011E → BEQ LAB_0011  ; cinématique: viewport + palette anim
CMP.W #4,LAB_011E → BEQ LAB_0010  ; map/shop: blit fond + sprites
CMP.W #5,LAB_011E → BEQ LAB_0012  ; combat: blit fond alternatif
CMP.W #3,LAB_011E → BEQ LAB_0013  ; événement: setup viewport
```

- `LAB_0010` : `JSR LAB_0565` (copier palette du contexte vers COLOR00) + `JSR LAB_025D` (copier palette locale)
- `LAB_0011` : `MOVEQ #2,D0 ; JSR LAB_0576` (ramp palette mode 2)
- `LAB_0012` : `JSR LAB_0258` (black-out palette)
- `LAB_0013` : `JSR LAB_025A` = `JSR LAB_0576` mode 2

---

### 10.3 Format de script d'entité — bytecode 6 octets

Chaque entité (acteur) dispose d'un pointeur `2(A1)` = **PC script**. L'interpréteur tourne dans `LAB_01F1` → `LAB_01F3`.

**Format d'une instruction** : fixe **6 octets** :

| Offset | Taille | Champ           | Description                                              |
|--------|--------|-----------------|----------------------------------------------------------|
| 0      | byte   | `opcode`        | bit7=0 → index CEL (multiple de 4 → 0..28) ; bit7=1 → opcode contrôle |
| 1      | byte   | `param`         | sous-frame dans le fichier CEL, ou paramètre de l'opcode |
| 2      | byte   | `x_delta`       | décalage X signé relatif à la position de l'acteur       |
| 3      | byte   | `flags`         | flags de rendu (bit4 = draw sur les 2 buffers ; bit5 = use_mask_buffer) |
| 4-5    | word   | `y_pos`         | position Y signée (absolue ou relative selon opcode)     |

**Opcodes spéciaux** (bits 7=1, valeur `& $7F` = offset dans `LAB_0288`) :

| Byte hex | Nom inféré          | Description                                                            |
|----------|---------------------|------------------------------------------------------------------------|
| `$FF`    | `END_FRAME`         | fin du frame courant → attendre `speed` ticks avant de continuer       |
| `$FE`    | `LOOP_IDLE`         | retour à l'adresse idle sauvée `8(A5)` → boucle indéfinie              |
| `$FD`    | `DEATH`             | saut à l'animation de mort `32(A5)`                                    |
| `$80`    | `SET_DIR`           | changer flag direction (`22(A1)`)                                      |
| `$84`    | `COND_JUMP`         | saut conditionnel ou changement d'animation si param=3                  |
| `$88`    | `SET_SPEED`         | régler le compteur de vitesse `0(A5)` (ticks par frame)                |
| `$8C`    | `SKIP_8`            | avance le PC de 8 octets (skip 1 instruction + 2 octets)               |
| `$94`    | `LOOP_INIT`         | initialiser compteur de boucle `6(A5)` = param, sauver adresse `8(A5)` |
| `$A0`    | `MOVE_DELTA`        | déplacer l'acteur : delta X/Y avec flags de direction                  |
| `$A4`    | `ADVANCE_4`         | avancer PC de 4 octets (opcode "null" 4 octets)                        |
| `$B4`    | `CALL_EXT`          | appel à une fonction extérieure via table `LAB_0280`                   |
| `$BC`    | `SKIP_6_A`          | avancer PC de 6 octets (no-op)                                         |
| `$C0`    | `KILL`              | mettre `0(A1)` = 0 (désactiver l'entité)                               |
| `$C4`    | `SET_ASSET_TABLE`   | changer la table d'assets `28(A1)` = table N° param                   |
| `$CC`    | `BRANCH_IF_ZERO`    | branchement si variable nulle                                          |
| `$D0`    | `BRANCH_IF_NONZERO` | branchement si variable non nulle                                      |
| `$D4`    | `RESET_FRAME_STATE` | effacer toute la structure A5 (48 octets)                              |

**Opcodes de dessin** (bit7=0, valeur = offset dans la table de CEL de l'entité) :
Valeurs valides : `$00, $04, $08, $0C, $10, $14, $18, $1C` = 8 CEL possibles par acteur.
- byte 0 : `N*4` → `MOVEA.L 0(A0,N*4),A0` = pointer vers le fichier CEL N
- byte 1 : index de frame dans ce CEL (stride 10 dans les méta-données)
- bytes 2..5 : x, flags, y comme décrit ci-dessus

---

### 10.4 Structure d'une entité (pool de 40 entrées)

Pool = `LAB_0282` : `40 entrées × 0x2A = 1680 octets` ([program.asm#L5024](program.asm#L5024)).

| Offset | Taille | Variable     | Rôle                                                          |
|--------|--------|--------------|---------------------------------------------------------------|
| 0      | byte   | `active`     | 1 = entité active                                             |
| 1      | byte   | `running`    | 1 = en cours d'exécution (script en train de tourner)         |
| 2      | long   | `script_pc`  | pointeur courant dans le script bytecode                      |
| 6      | word   | `base_x`     | position X de base de l'acteur                                |
| 8      | word   | `base_y`     | position Y de base                                            |
| 10     | word   | `vel_y`      | vélocité verticale (gravité/saut)                             |
| 12     | word   | `screen_x`   | position X écran calculée ce frame                            |
| 14     | word   | `screen_y`   | position Y écran calculée ce frame                            |
| 16     | word   | `sprite_w`   | largeur du sprite courant (pixels)                            |
| 18     | word   | `sprite_h`   | hauteur du sprite courant (lignes)                            |
| 20     | byte   | `frame_param` | sous-frame index actuel                                      |
| 21     | byte   | `last_param` | sous-frame précédent (pour détection changement)              |
| 22     | byte   | `direction`  | bits : 0=flip_x, 1=flip_y, 2=??                              |
| 24     | long   | `entity_id`  | identifiant unique de l'entité                                |
| 28     | long   | `asset_table`| pointeur vers la table de 8 pointeurs de CEL                  |
| 32     | byte   | `script_type`| index dans `LAB_011B` = type de script (0..?)                 |
| 36     | long   | `frame_state`| pointeur vers le `LAB_0284` de cet acteur (structure 0x30 o.) |
| 40     | word   | `visible`    | 0 = caché (EORI toggle par `LAB_01E1`)                       |
| 42-47  | —      | divers       | champs complémentaires (Z-order, collideur, IA state…)        |

**Structure frame_state** (`LAB_0284`, 480 longwords = 40 slots × 12 longwords = 40 × 0x30 octets) :

| Offset | Taille | Variable     | Rôle                                               |
|--------|--------|--------------|----------------------------------------------------|
| 0      | byte   | `speed`      | compteur de vitesse (décrémenté, reset sur END_FRAME) |
| 1      | byte   | `active`     | 1 = la frame tourne                                |
| 2      | long   | `inner_pc`   | pointeur sauvegardé pour boucle interne            |
| 6      | byte   | `loop_count` | compteur de LOOP_INIT                              |
| 7      | byte   | `looping`    | flag boucle active                                 |
| 8      | long   | `loop_addr`  | adresse de retour pour LOOP_INIT/LOOP_IDLE         |
| 12     | long   | `death_addr` | adresse du script de mort (FD)                     |
| 16     | byte   | `anim_changed` | flag "changer d'anim"                            |
| 20     | long   | `next_anim`  | adresse de la prochaine séquence d'animation        |
| 26     | byte   | `has_callback` | flag callback                                   |
| 27     | byte   | `callback_timer` | décompte callback                              |
| 28     | long   | `target_addr`| cible de mouvement ou callback address             |
| 42     | byte   | `respawn_flag` | flag respawn / continuation                     |

---

### 10.5 Scènes de jeu — table des fonctions

> **Note :** Cette section décrit les scènes du binaire `program` (intro / loader). Pour les écrans du jeu interactif (overworld, combat, boutique, NPC…), voir **§10.19** qui documente la machine à états de `mog.asm` (`LAB_068F`).

#### Scènes `program.asm` — S_0 ([program.asm#L108](program.asm#L108))

| Routine     | Nom inféré               | `LAB_011E` | Fond chargé     | Description                                                             |
|-------------|--------------------------|------------|-----------------|-------------------------------------------------------------------------|
| `LAB_0037`  | `scene_overworld()`      | 4          | `LAB_00CB` (dw1)| Carte overworld interactive : déplacement du chevalier, sélection du nœud destination. Spawn `LAB_00E9` + `LAB_00EA` (UI carte) + `LAB_00E7` × 10 (nœuds). 41 frames non-interactif puis boucle joueur. |
| `LAB_0038`  | `overworld_frame_loop()` | 4          | —               | Boucle de 40 frames pour un "tick" de la carte                          |
| `LAB_001A`  | `combat_init_p1()`       | 4          | `LAB_01CD`      | Initialiser combat joueur 1 : charger CEL, palette                      |
| `LAB_001B`  | `combat_init_p2()`       | 5          | —               | Initialiser combat joueur 2, mode 5 bitplanes (32 couleurs)             |
| `LAB_001C`  | `combat_2p()`            | 4          | `LAB_01CF`      | Combat 2 joueurs : boucle principale de combat                          |
| `LAB_002C`  | `shop_p1()`              | 4          | `LAB_01CB`      | Boutique joueur 1 (`LAB_00D2` = animation joueur 1 dans boutique)       |
| `LAB_002D`  | `tavern_p2()`            | 4          | `LAB_01D2`      | Taverne / événement pour joueur 2                                       |
| `LAB_002E`  | `shop_p2()`              | 4          | `LAB_01CF`      | Boutique joueur 2                                                       |
| `LAB_002F`  | `event_encounter()`      | 4          | `LAB_01CD`      | Événement : séquence de 2 animations + timer                            |
| `LAB_0036`  | `ai_turn_encounter()`    | 2→4        | `LAB_01D2`      | Tour IA : overworld avec `LAB_00E6` (sprites map + score bars)          |
| `LAB_0039`  | `shop_both()`            | 4          | `LAB_01CF/01CE` | Double boutique : les 2 joueurs magasinent                              |
| `LAB_003B`  | `game_over()`            | 0          | `LAB_01CB`      | Écran fin de jeu : palette, crédits, retour au menu                     |

#### Chaîne de scènes typique d'un round

```
scene_overworld()
  → (rencontre) → combat_init_p1() → combat_init_p2() → combat_2p()
  → (boutique)  → shop_p1() ou shop_p2() ou shop_both()
  → (événement) → event_encounter() ou tavern_p2()
  → (IA)        → ai_turn_encounter()
  → scene_overworld() [round suivant]
```

Quand tous les rounds sont finis ou qu'un chevalier obtient la Moonstone → `game_over()`.

---

### 10.6 Carte overworld — graphe de nœuds

`LAB_00E7` ([program.asm#L2215](program.asm#L2215)) contient ~145 entrées de 6 octets = les positions d'affichage des chevaliers sur la carte overworld. Chaque entrée est une instruction de dessin standard :

```
Entrée (6 bytes) :
  byte 0 = 0x00       → CEL index 0 (les icônes de chevalier = 1er asset)
  byte 1 = N          → sous-frame = direction/type d'icône (0..7)
  byte 2 = X_signed   → colonne écran (pixels)
  byte 3 = flags
  word 4 = Y_signed   → ligne écran (pixels)
```

Les positions décodées en pixels 320×200 à partir de l'offset 4 et du byte 2 (+ base X de l'acteur = 0, Y de l'acteur = 0) donnent les **coordonnées des nœuds de la carte**.

Structure du graphe inférée (positions relatives en pixels, +/- 5 px d'incertitude) :
Le fichier contient environ 36 nœuds distincts (lieux sur la carte) et ~70 chemins bidirectionnels. Les données sont dupliquées (une entrée A→B et une entrée B→A) pour permettre le dessin de chaque chevalier quel que soit son sens de déplacement.

`LAB_003A` ([program.asm#L760](program.asm#L760)) référence `LAB_00E7` dans une table de 10 pointeurs :

```text
LAB_003A:
    DC.L LAB_00E7   × 10
```

Cette table est utilisée dans `scene_overworld()` :

```text
MOVE.W #$0005,LAB_00EF   ; 5 joueurs affichés
MOVE.W #$000f,LAB_00F0   ; piste 15
MOVE.L #LAB_003A,LAB_0029+2  ; table d'animation
MOVE.W #$000a,LAB_0029        ; 10 frames
BSR LAB_001F              ; boucle 10 fois sur LAB_00E7
```

Ainsi les chevaliers sont positionnés en les « jouant » comme un script d'animation, en utilisant le contenu de `LAB_00E7` comme une simple liste de positions-frame.

#### Types de nœuds — `mog.asm`

Dans `mog`, la table `LAB_069F` ([mog.asm#L1037](mog.asm#L1037)) contient les positions et types de tous les lieux interactifs de la carte. Format : triplets `(type:word, x:word, y:word)` terminés par `0xFFFF`. La position du chevalier courant est lue dans `126(knight)` (X) et `128(knight)` (Y).

Voir **§10.19** pour le tableau complet des types de nœuds (0x01, 0x02, 0x15..0x1c, 0x1e, 0x21) et leurs effets en jeu.

**Séquence de déclenchement** :
1. À chaque frame overworld, `LAB_006B` balaie `LAB_069F` et appelle `LAB_0067` pour tester si le chevalier est sur un nœud.
2. Si match (D5=2), le type déclenche le gestionnaire correspondant.
3. Le retour se fait toujours vers `LAB_00B2` → `SECSTRT_36` (restauration hardware) → reprise de la carte.

---

### 10.7 Données d'animation — catalogue des séquences

Toutes en section S_2 ([program.asm#L1665](program.asm#L1665)).

#### Scripts d'animation des personnages

| Label      | Nom inféré                          | Description                                                                                  |
|------------|-------------------------------------|----------------------------------------------------------------------------------------------|
| `LAB_00D2` | `anim_knight1_walk`                 | **Joueur 1** — chevalier humain couleur 1 (rouge). Longue séquence : marche × 5 directions, ~250 instructions. Affiché sur fond `bg1a`, palette P1. |
| `LAB_00D3` | `anim_knight2_walk`                 | **Joueur 2** — chevalier humain couleur 2. Même structure que D2 ; blocs `$B4`+`LAB_003F` déclenchent des flashs (coups reçus). Affiché sur fond `bg2`, palette P5. |
| `LAB_00D4` | `anim_knight3_walk`                 | **Joueur 3** — chevalier humain couleur 3. Cycles de marche 4–6 frames + callbacks. Affiché sur fond `bg1b`, palette P2. |
| `LAB_00D5` | `anim_knight4_walk`                 | **Joueur 4** — chevalier humain couleur 4. Cycles 4 frames, attaque, mort. Affiché sur fond `bg3`, palette P5. |
| `LAB_00D6` | `anim_flash_brief`                  | Courte animation d'éclair 2-14 frames : `LAB_003F` (white palette → restore)               |
| `LAB_00D7` | `anim_map_knight_walk_r`            | Marche droite sur la carte overworld (CEL $0C = chevalier map, sub_frames 0x10..0x20)      |
| `LAB_00D8` | `anim_map_knight_walk_l`            | Marche gauche sur la carte overworld                                                        |
| `LAB_00D9` | `anim_map_knight_walk_r2`           | Suite `LAB_00D8` (segment suivant de la courbe de mouvement)                               |
| `LAB_00DA` | `anim_map_knight_path_cont`         | Continuation de parcours de chemin sur la carte                                            |
| `LAB_00DB` | `anim_loop_idle_a`                  | Idle court (2 frames, boucle infinie `$8802…$8400 DC.L LAB_00DB`)                         |
| `LAB_00DC` | `anim_loop_idle_b`                  | Idle court (2 frames boucle infinie)                                                       |
| `LAB_00DD` | `anim_loop_idle_c`                  | Idle très court (boucle infinie)                                                           |
| `LAB_00DE` | `anim_walk_dir1`                    | Marche direction 1 (gauche) : long cycle                                                    |
| `LAB_00E0` | `anim_walk_dir2`                    | Marche direction 2 (droite ou haut) : cycle similaire                                      |
| `LAB_00E1` | `anim_walk_dir3_loop`               | Marche direction 3 avec boucle interne (`$8802…$8400 DC.L LAB_00E1`)                      |
| `LAB_00E2` | `anim_walk_dir3b_loop`              | Variante de `E1` (boucle différente)                                                       |

#### Cinématiques et menus

| Label      | Nom inféré                          | Description                                                                                  |
|------------|-------------------------------------|----------------------------------------------------------------------------------------------|
| `LAB_00E3` | `anim_intro_knights_enter`          | Intro combat : 4 chevaliers descendent depuis le haut de l'écran (CEL `$10 1b`, 25 positions y=`$ffe4...$03`) + boucle terminale `$8808`. Utilisé en début de partie. |
| `LAB_00E4` | `anim_shrine_idol_loop`             | Idole de sanctuaire — boucle infinie statique (`$8400 DC.L LAB_00E4`). Affiché sur fond sanctuaire (`bg3`/`LAB_00CA`). |
| `LAB_00E5` | `anim_shrine_entry_2knights`        | Entrée de 2 chevaliers au sanctuaire (CEL `$10 00` et `$10 02`, positions absolues descendantes) + données HUD Mindscape. Déclenché par `LAB_001A`. |
| `LAB_00E6` | `anim_overworld_hud`                | Scène overworld : layout 12 icônes sur la carte + appel opcode `$B4 → LAB_0032` (calcul 3 barres de score) + `$B4 → LAB_0040` (flash titre). |
| `LAB_00E7` | `map_all_node_positions`            | **Table complète des nœuds de la carte overworld** : ~54 nœuds × 2 hémisphères = ~108 entrées de 6 octets (format script standard). Chaque entrée = 1 position affichage d'une icône de nœud. |
| `LAB_00E8` | `anim_map_selected_node`            | Animation du **nœud actif sélectionné** sur la carte (CEL `$14 01..06` = frames clignotantes du marqueur chevalier + sous-animations `$94 05`, `$8802`, `$880a`). |
| `LAB_00E9` | `anim_map_ui_frame`                 | **Cadre UI de la carte overworld** : 18 sprites positionnels (CEL `$14 07..18`) ; boucle infinie `$8400`. Affiché en fond permanent de la carte. |
| `LAB_00EA` | `anim_map_idle_overlay`             | Overlay idle de la carte : boucle infinie (`$8400`) + 1 frame (CEL `$14 01/02/00` = chevalier courant). |
| `LAB_00EB` | `anim_loot_encounter`               | **Écran de butin post-combat** : créature/monstre (CEL `$14 1c..1b`) suivi des sprites de récompense (`$14 24/25/26/27` = 4 types d'objets). Déclenché après victoire en combat. |
| `LAB_00EC` | `anim_victory_walkoff`              | **Animation victoire post-butin** : chevalier gagnant sort de l'écran (CEL `$14 29..36`, 14 frames). Suit immédiatement `LAB_00EB`. |
| `LAB_00ED` | `anim_endround_trophy`              | **Cinématique fin de round** : présentation des trophées/reliques obtenus (CEL `$10 02/01/03/04/05..0a` = 10 sprites). Affiché sur fond `bg3` en fin de round via `LAB_0036`. |
| `LAB_00EE` | `anim_ending_cinematic`             | **Cinématique de fin de partie** (9 phases). Entre chaque phase : opcode `$B4 → LAB_05AF` (décrémente `LAB_05B8+2`) ; dernière phase : `$B4 → LAB_05AE` (pose `LAB_05E6 = 1` = flag de fin). Sprites `$08 00..18` = scène finale Stonehenge. |

---

### 10.8 Système de tour par tour (overworld)

#### Déroulement d'un tour

Fonction `scene_overworld()` = `LAB_0037` ([program.asm#L648](program.asm#L648)) :

```text
1. Init : JSR LAB_01E4      → vider le pool d'entités (40 × 42 octets)
2. Load : JSR LAB_0258/0263 → black palette + copier fond PIV vers les 2 buffers
3. Spawn : 
   a. MOVEA.L #LAB_00E9,A0 ; JSR LAB_0015   → spawn icônes joueurs sur la carte
   b. MOVEA.L #LAB_00EA,A0 ; JSR LAB_0015   → spawn idle infini
4. Config :
   MOVE.W #$0005,LAB_00EF  ; 5 chevaliers affichés
   MOVE.W #$000f,LAB_00F0  ; offset second groupe
   MOVE.L #LAB_003A,LAB_0029+2  ; table d'animation = LAB_00E7 × 10
   MOVE.W #$000a,LAB_0029        ; 10 frames
5. Animation tour :
   BSR LAB_001F             → boucle interne de sélection de frame
   [40 frames + LAB_0038]   → joueur déplace son chevalier sur la carte
   [8 frames de transition]
6. Événement (si détection collision sur un nœud) :
   → combat / boutique / événement (voir §10.5)
7. Retour à step 1 pour le joueur suivant
```

**Variable `LAB_0028`** = compteur de frames dans la boucle courante.  
**Variable `LAB_0029`** = nombre de frames max de la boucle.  
**Variable `LAB_0026+2`** = index de frame courant (itérateur sur l'animation table).

#### Gestion des joueurs

Le jeu supporte **1 à 4 joueurs** (humains ou IA). Les fonctions `LAB_0015` / `LAB_0016` spawne les entités respectivement pour **joueur 1** (`LAB_00EF`) et **joueur 2** (`LAB_00F0`) :

```text
LAB_0015:
    MOVEA.L #LAB_0276,A2          ; table d'assets joueur 1
    MOVE.W #$00a0,D0              ; x de base = 160
    MOVE.W #$0000,D1              ; y de base = 0
    MOVE.W #$0064,D2              ; y absolu = 100
    ADD.W LAB_00EF,D2             ; + offset joueur 1
    MOVE.W #$0001,D3              ; slot = 1
    JSR LAB_01DA                  ; créer entité
```

`LAB_0276` = **table d'assets** des chevaliers de la carte (pointe vers les 8 CEL).

**Contrôle joystick** (lecture dans `LAB_0049`) :
- Joystick 0 (`JOY0DAT`, `CIAA_PRA` bit 6) → joueur 1
- Joystick 1 (`JOY1DAT`, `CIAA_PRA` bit 7) → joueur 2
- Bits de résultat `LAB_00C0`/`LAB_00C1` :
  - bit 0 = droite
  - bit 1 = gauche
  - bit 2 = haut
  - bit 3 = bas
  - bit 4 = feu / action

---

### 10.9 Combat — séquences et IA

#### Architecture du combat

Le combat est **en temps réel** pour les joueurs humains. Deux entités s'affrontent en utilisant les scripts `LAB_00D2`…`LAB_00D5` (marche/attaque/mort). La structure de base :

```text
combat_init_p1() [LAB_001A]:
    LAB_011E = 4 (4 plans)
    LAB_011D = LAB_01CD
    animation_table = LAB_0023 (4 entrées : DE,DE,E0,E0)
    spawn(LAB_00E3)              → intro flash
    spawn(LAB_00E5)              → intro 2 chevaliers
    JSR LAB_0007                 → boucle infinie jusqu'à trigger

combat_init_p2() [LAB_001B]:
    LAB_00D0 = 8 ticks
    animation_table = LAB_0024 (20 × LAB_00D8 = 20 frames idle)
    LAB_0029 = 5 ; boucle_001D()

combat_2p() [LAB_001C]:
    charger fond LAB_01CF
    LAB_011E = 4
    LAB_011D = LAB_01D1
    animation_table = LAB_0025 (16 × LAB_00D7)
    LAB_00D1 = 1   → mode deux joueurs actif
    boucle_001D()
    LAB_00D1 = 0
    LAB_00D0 = 6   → vitesse réduite après combat
```

#### Animations de combat

Les sprites de combat utilisent le **CEL index `$14`** (offset 20 dans la table d'assets = 5ème CEL). Les sous-frames de LAB_00D8 :
- sub_frame 0x01..0x0c = marchés de droite
- sub_frame 0x0c..0x18 = marchés de gauche
- sub_frame 0x18..0x1f = attaque
- (inféré d'après la structure des données)

#### Tables d'animation par scénario

| Table       | N entrées | Contenu                              | Scénario       |
|-------------|-----------|--------------------------------------|----------------|
| `LAB_0023`  | 4         | DE, DE, E0, E0                       | combat intro   |
| `LAB_0024`  | 20        | 20 × D8 (idle)                       | p2 init        |
| `LAB_0025`  | 16        | 16 × D7 (marche)                     | combat 2p      |
| `LAB_003A`  | 10        | 10 × E7 (positions carte)            | overworld      |

#### Barres de score (`LAB_0032`)

Appelé via opcode `$B4` dans `LAB_00E6` → `LAB_0032` ([program.asm#L571](program.asm#L571)) :

```text
LAB_0032:
    MOVEQ #12,D0 ; id=12 (barre HP joueur courant)
    MOVE.W SECSTRT_9,D1 ; couleur de la barre (rouge/vert/bleu)
    MOVE.W #$0005,D2 ; largeur en unités
    MOVEQ #0,D3
    JSR LAB_057A         → créer fade animation avec ces paramètres
    MOVE.L D0,LAB_0033   ; sauver handle
    ... (×2 pour HP et MP ou pour j1/j2)
    MOVE.L D0,LAB_0035
```

`LAB_057A` ([program.asm#L10504](program.asm#L10504)) crée un **slot de fade couleur** dans `LAB_05D4` (6 slots × 12 octets = la table de fades de S_31). Les barres de vie sont donc des animations de couleur dans la copper list.

#### Résolution du combat — `mog.asm`

Dans `mog`, le combat (états 1, 2, 5, 10) se déroule en temps réel avec la boucle `LAB_04D0`. Après la résolution :

**Après combat PvP (état 1) ou créature (état 2) :**
- Si le perdant a skill > 0 : `LAB_001C` (un item à la fois, voir §10.19)
- Si le perdant a skill == 0 : `LAB_0022` (transfert total de tous les items)
- Si le coup final est type `0x14` (coup décisif) : `LAB_0021` (partage de l'or)

**Après dragon (état 10) :**
- Si victoire : `78(knight) += 2`, séquence de fin (si `LAB_05DC bit 0`)
- Sinon : perte de 2 skill levels, retour vers état 9 (temple)

**Respawn/mort (`LAB_000E`) :** quand `80(knight) ≤ 0`, HP restaurés au max (`84`) et skill decremented de 1.

**Progression de skill (`LAB_052F`) :** quand `84(knight) == 80(knight)` (HP courant = HP max, pleinement restauré), skill +1 (max 5).

**Calcul HP max (`LAB_0013`) :** `Constitution × 10 + Ring_of_Protection × 20 + bonus_armure + 10` → stocké dans `84(knight)`.

---

### 10.10 Boutique / magasin

La boutique (market) est gérée par `LAB_002C` (joueur 1) et `LAB_002E` (joueur 2) :

```text
shop_p1() [LAB_002C]:
    JSR LAB_0258    → black palette
    JSR LAB_01E4    → vider entités
    MOVEA.L LAB_00D2,A0 ; animation du joueur 1
    JSR LAB_0268+2      → copier fond LAB_00C8 → buffer courant
    JSR LAB_0263        → copier fond vers les 2 buffers
    LAB_011D = LAB_01CB
    LAB_011E = 4
    LAB_0120 = 0
    JSR LAB_0007        → boucle boutique
    (retour LAB_00C7 = buffer global)
```

Les interactions boutique utilisent le joystick (`LAB_00C0`) lu dans la boucle `LAB_0007`. Le menu de sélection d'items n'est pas entièrement documenté dans ce passage, mais la palette `LAB_01CB` est celle de la boutique joueur 1 (couleurs spécifiques).

La boutique double `LAB_0039` charge successivement `LAB_01CF` (j2) + `LAB_01CE` (événements) puis termine par :
```text
MOVEA.L LAB_00C6,A0
JSR LAB_054D        → copier le buffer final vers l'écran
```

#### Boutiques et services — `mog.asm`

Les différents services accessibles via les nœuds de la carte correspondent à plusieurs états de `LAB_068F` :

| État | Lieu          | Contenu                                                     | Entrée           |
|------|---------------|-------------------------------------------------------------|------------------|
| `5`  | Arène         | Combat contre adversaire dans l'arène d'une ville           | `LAB_0522` (HUD) |
| `6`  | Boutique/Carte| Inventaire, gemmes, navigation items                        | `LAB_04FE`+`LAB_051F` |
| `8`  | Armurier      | Achat d'armure (`0x1c`/`0x1d`/`0x1e`) et épée              | `LAB_04DB`       |
| `9`  | Temple        | Soins (restaure HP au max) et monte/descend en skill        | `LAB_04D4`       |
| `11` | Taverne       | Jeu de dés, interaction joueurs                             | `LAB_04DB`       |

**Détail armurier (état 8)** — `LAB_052C` avec `D2&0xF` :
- `0xa` → acheter armure
- `0xc` → acheter épée
- `0x5` → déposer item
- `0x1` → vendre item
- `0x3` → ramasser item

**Level-up via temple/marchand (`LAB_052F`)** : lorsque le chevalier est à pleine santé (`hp_current == hp_max`), il reçoit +1 niveau de skill (plafonné à 5). Appelé par le gestionnaire de boutique après achat/soin.

---

### 10.11 Événements et rencontres spéciales

#### Flash d'éclair (`LAB_003F` / `LAB_0040`)

Appelé via `$B4 00 DC.L LAB_003F/LAB_0040` (opcode `$B4` = `CALL_EXT`).

`LAB_003F` ([program.asm#L766](program.asm#L766)) = **flash blanc** rapide (8 ticks) :
```text
LEA LAB_0042,A0  ; palette pleine de $0FFF (blanc)
JSR LAB_0565     → appliquer
MOVEQ #8,D0
JSR LAB_054F     → attendre 8 ticks
MOVEA.L LAB_05D2,A0
JSR LAB_0565     → restaurer palette sauvée
```

`LAB_0040` ([program.asm#L780](program.asm#L780)) = **flash saccadé** (6 étapes) :
```text
MOVEQ #20,D0 ; wait
BSR LAB_0041 ; flash
MOVEQ #2,D0 ; wait
...×6 flashes
```

`LAB_0042` / `LAB_0043` = les palettes de flash (12 mots chacune, valeurs `$0000`/`$0FFF`).

Ces flashes signalent une **collision** (mort d'un chevalier, prise d'objet spécial, entrée dans une zone magique).

#### Rencontres et événements de nœud — `mog.asm`

Les rencontres spéciales sont déclenchées lors du passage sur un nœud de la carte overworld (§10.6 et §10.19). Elles correspondent à des états distincts de `LAB_068F` :

**Sanctuaires des chevaliers (0x15..0x18 — `LAB_00B0`) :**
- Le château correspond à la faction du chevalier courant (`54(knight)` = 0..3)
- Si faction == type_château - 0x15 → skill +1 (plafonné à 3 in-castle, puis via temple)
- Transition automatique vers état 9 (temple de guérison)

**Temple de guérison (0x1e — `LAB_007C`) :**
- Restaure les HP courants au max (`hp_current = hp_max`)
- Propose un gain de skill si HP max == HP courant (via `LAB_052F`)
- Écran correspondant : état 9 (background `LAB_0693`)

**Sorcier Mythral (0x1b — `LAB_00A1`) :**
- Accès à Mythral the Mystic (Witch Doctor)
- Permet échange de moonstones contre bonus de skill
- Écran correspondant : état 3 (background `LAB_0696`)

**Créature aléatoire (0x02 — `LAB_005B`) :**
- Sélectionne une créature depuis `LAB_08C6` (table de structures créatures)
- Incrémente le compteur de rencontres de la créature (`20(creature)`)
- Quand compteur == 3 : avance le cycle global `LAB_06C1` (mod 8) → prochaine créature
- Écran correspondant : état 2 (background `LAB_069A`+`LAB_0698`)

**Duel entre chevaliers (0x01 / 0x21 — `LAB_004F`) :**
- Combat PvP standard entre le chevalier courant et un autre joueur
- Résolution via `LAB_001C` puis `LAB_000E` (mort/respawn)
- Écran correspondant : état 1 (background `LAB_0694`)

**Antre du Dragon (0x1c — `LAB_009D`) :**
- Condition d'accès : `inventaire[20] == 0x0f` (quatrième relique / Dragon key)
- Sans relique : perte de 2 skill levels → temple (état 9)
- Victoire Dragon : `78(knight) += 3`, efface item, séquence de fin (`LAB_0DCA`)
- Écran correspondant : état 10 (background `LAB_0694`+`LAB_0698`)

#### Décompresseur RNC — `LAB_0190` ([program.asm#L3617](program.asm#L3617))

Les fichiers `music.cmp` et `vmusic.cmp` sont compressés avec **RNC ProPack type 1** (Rob Northen) :
- Magic : `$524E4301` = « RNC\x01 »
- Structure : header 18 octets (magic + taille décompressée + taille compressée + CRC + …) + données Huffman
- Décompresseur : `LAB_0190`…`LAB_01B5` = implémentation complète du décodeur RNC1 avec 3 tables Huffman

#### Décompresseur LZSS — `LAB_049C` ([program.asm#L9210](program.asm#L9210))

Utilisé pour les fichiers `.cel` (sprites) :
- Fenêtre coulissante : **11 bits d'offset = 2048 octets**
- Longueur de copie : **5 bits après rotation → 2 à 34 octets**
- Littéraux : 1 octet direct si bit de contrôle = 0
- Table de dispatch à 34 entrées `MOVE.B (A3)+,(A2)+` → copie unrolled

#### Décompresseur bit-planaire — `LAB_0448` / SECSTRT_21 ([program.asm#L7923](program.asm#L7923))

Utilisé pour les données pixel des `.stile` (décors) :
- Opcodes 2 bits (lus bit à bit via `LAB_048A`/`LAB_048D`)
  - `00` = remplissage nul (zéro)
  - `01` = copie 4 octets littéraux
  - `10` = back-reference courte (longueur 5 bits + 8 = 8..38 octets)
  - `11` = back-reference longue variable
- D7 = compteur de bits restants à traiter

#### Décodeur PIV Mindscape — `LAB_0434` ([program.asm#L7663](program.asm#L7663))

> ⚠️ **Correction d'analyse :** les fichiers `.PIV` ne sont **pas** au format IFF/ILBM standard (magic réel = `0x00050000` ou `0x00040000`, pas `"FORM"`). Il s'agit d'un format Mindscape propriétaire.

Structure d'un fichier `.PIV` (vérifiée sur les fichiers du workspace) :
- `word[0]` = nombre de plans (4 ou 5) — le first word `0x0005`/`0x0004` identifie le format
- `word[1]` = flags ou hauteur (à confirmer)
- Palette : 32 mots Amiga 12-bit (`$0RGB`) — conversion depuis les octets RGB source : `(R << 8 | G << 4 | B) >> 5 & 0x0777` *(inféré par analogie — à valider sur le code)*
- Body : PackBits/ByteRun1 maison — même algorithme que IFF mais sans wrapper :
  - byte ≥ 0 → N+1 octets littéraux suivants
  - byte < 0 → répéter l'octet suivant `(1 - byte)` fois
  - byte `$80` → NOP
- Décodage plan par plan vers le bitmap interleaved (N plans × 320/8 octets par ligne)

---

### 10.12 Rendu de texte et interface

#### Moteur de rendu de texte — SECSTRT_12 ([program.asm#L5066](program.asm#L5066))

Entrée : A0 = chaîne ASCII terminée par `$00`.

1. **Table de transposition ASCII** : `LAB_00F8` (64 octets) mappage ASCII→index de glyphe. Caractères `$20`..`$5F` (espace à `_`). Minuscules mappées aux majuscules.
2. **Font CEL** : chargée dans `LAB_011A+16` (via `bold.f`). Chaque glyphe = 10 octets de méta-data : `width(word)`, `height(word)`, `draw_flags(byte)`, … (stride 10).
3. **Rendu proportionnel** : `LAB_00F1` = largeur courante, `LAB_00F2` = hauteur courante. Saut de ligne auto à x ≥ 320 (`$0140`).
4. **Anti-mirror** : option `bit1 = centering` → pré-calcul de la largeur totale via `LAB_0297` puis centrage automatique.

`LAB_028F` ([program.asm#L5192](program.asm#L5192)) = **rendu de liste de texte** (linked list) :

```c
struct text_record {
    char *text;          // offset 0 (long)
    uint16_t x_start;    // offset 4
    uint16_t y_start;    // offset 6
    uint16_t flags;      // offset 8 (bit0=center, bit1=shadow)
    struct text_record *next;   // offset 10
};
```

Les textes de l'intro et des crédits (`LAB_00A2`…`LAB_00B4` / `LAB_00FC`…`LAB_0118`) sont organisés en ces listes chaînées avec délais (`DC.L $00000037` = y position = 55 pixels, timing de scroll).

---

### 10.13 Gestion de la Moonstone — objet central

La Moonstone est l'objectif ultime du jeu. D'après l'analyse du code et la logique de jeu :

#### Chaîne de victoire complète

```text
1. Naviguer sur la carte overworld (LAB_0037)
2. Visiter les 4 sanctuaires (shrine nodes) → collecter les 4 reliques/clés
   → chaque victoire de sanctuaire déclenche : LAB_00EB (loot) + LAB_00EC (victoire)
3. Accéder au repaire final (boss node) avec les 4 reliques
4. Combattre et vaincre le boss final (scène combat LAB_001C étendue)
5. Récupérer la Moonstone (animation LAB_00ED = fin de round trophée)
6. Retourner à Stonehenge pendant la bonne phase de lune
   → indiquée par les druides au début (dans le texte d'intro LAB_00B5/B6)
7. Déclencher la cinématique de fin : LAB_003B → LAB_00EE (9 phases)
   → Texte final LAB_00B0 : "And so, the tale of the Moonstone..."
```

#### Indicateurs de phase de lune

Les variables `LAB_005E` (initialisée à 2) et `LAB_005F` (initialisée à 8) dans la section CHIP sont décrémentées à chaque tick audio (IRQ `SECSTRT_1`) :

```asm
SUBQ.B #1,LAB_005E     ; divider 1 : /2
BNE.S LAB_005D
MOVE.B #$02,LAB_005E
SUBQ.B #1,LAB_005F     ; divider 2 : /8 → phase change toutes les 2×8 = 16 ticks audio
BNE.S LAB_005D
MOVE.B #$08,LAB_005F
```

Ces compteurs servent de diviseurs de tempo pour le SoundTracker (LAB_0096 = compteur de patterns), **pas** de compteur de phase de lune en jeu. La phase de lune effective est probablement stockée dans le pool d'entités (`LAB_0282`) ou dans `SECSTRT_33` (état de la carte), et avance d'un cran à chaque round complet.

#### Cinématique finale (`LAB_003B`)

```text
1. JSR LAB_05AB          → afficher la carte overworld (état final)
2. JSR LAB_0010          → ???
3. Attendre 15 frames
4. Spawn LAB_00EE        → cinématique 9 phases (sprites $08 00..18 = Stonehenge)
5. JSR LAB_05AC          → attendre LAB_05E6 = 1 (fin LAB_00EE via LAB_05AE)
6. JSR LAB_0007          → run boucle finale
7. Fondu vers buffer LAB_00C9 + palette LAB_01CC
8. JSR LAB_028F          → afficher texte LAB_00B0 (5 lignes) :
      "And so, the tale of the Moonstone and the courage
       of the knights that fought for it is passed on from
       one generation to the next."
9. Attendre 500 frames (~10 s) + fondu palette 6 steps + 90 frames
```

#### Variables liées à la Moonstone

| Symbole     | Rôle                                                                   |
|-------------|------------------------------------------------------------------------|
| `LAB_005E`  | Diviseur audio 1 (période : 2 ticks) — timing SoundTracker            |
| `LAB_005F`  | Diviseur audio 2 (période : 8) — timing SoundTracker                  |
| `LAB_05E6`  | Flag fin cinématique (`LAB_05AE` le met à 1 en fin de `LAB_00EE`)     |
| `LAB_05B8`  | Score/progression globale (1000 = max, decremente par ticks)          |
| `LAB_05B8+2`| Numéro d'étape courante de la cinématique (decrementé par LAB_05AF)   |
| `LAB_05BC`  | Nombre de rows affichées sur la carte overworld (initialisé à 8)      |

---

### 10.15 Système d'inventaire et objets (reliques / clés)

#### Structure des données

L'inventaire n'a **pas d'écran dédié** visible dans le code ; les objets collectés font partie de l'état d'entité de chaque chevalier, stocké dans le **pool d'entités** `LAB_0282` (40 entrées × 42 octets = 1 680 octets, section S_11).

Chaque entrée du pool comprend (inféré d'après la taille et le format 6 octets du script) :
- Position XY (4 octets)
- Index CEL + sous-frame courant (2 octets)
- Etat animation (flags + compteurs)
- **Champs d'état de jeu** (HP, reliques possédées, or, …) dans les octets restants (~28 octets)

#### Les 4 reliques

Les reliques sont obtenues en battant les créatures gardant les sanctuaires (`shrine nodes` sur la carte). D'après le code :

```text
Relique obtenue → animation LAB_00EB déclenchée (loot screen)
    Sprites CEL $14 24/25/26/27 = les 4 types de reliques
    Victoire confirmée → animation LAB_00EC (chevalier sort de l'écran)
```

Les 4 indices CEL `$14 24..27` correspondent probablement aux 4 reliques distinctes. La possession d'une relique est vraisemblablement encodée comme un **bit-flag** dans les 42 octets d'état du chevalier.

#### Fichier `kn1.ob`

`kn1.ob` (présent dans le workspace) est le template de données objet des chevaliers. Il est chargé via la table d'assets (`LAB_0276`), offset 12 (`MOVE.L A1,12(A0)` après décompression de `LAB_0165`). Ce fichier définit vraisemblablement :
- Les statistiques initiales des chevaliers (HP max, vitesse, attaque)
- Les emplacements d'inventaire (4 slots reliques + slots équipement)

#### Barres de vie / statut HUD

```text
LAB_0032():
    bar_slot[0] ← D0=12, couleur SECSTRT_9   → HP joueur
    bar_slot[1] ← D0=15, couleur LAB_01C9    → 2e indicateur (reliques ?)
    bar_slot[2] ← D0=23, couleur LAB_01CA    → 3e indicateur (or / énergie ?)
```

Ces 3 barres sont des **fade-animations couleur** dans la copper list (via `LAB_057A`), positionnées au bas de l'écran. Leur valeur numérique est mise à jour à chaque appel de `LAB_003C` (appelé depuis la boucle principale quand `LAB_00D1 = 1`).

---

### 10.16 Écran de loot post-combat

#### Séquence complète

Déclenchée par `LAB_0039` ([program.asm#L641](program.asm#L641)) après chaque combat/rencontre gagnée :

```text
Phase 1 — Knight enters (LAB_002E / LAB_002F context) :
    Fond : bg3 (LAB_00CC)  Palette : P5 (LAB_01CF)
    Spawn : LAB_00D5 (chevalier couleur 4 ou le joueur vainqueur)
    JSR LAB_0007  → run jusqu'à fin d'animation d'entrée

Phase 2 — Loot screen (LAB_0039 → LAB_001F × 5 frames) :
    Fond : dw1 (LAB_00CB)  Palette : P4 (LAB_01CE)
    Spawn : LAB_00E9 (cadre UI) + LAB_00EB (animation loot)
    Table d'animation : LAB_003A (10 × LAB_00E7 = noeuds carte)
    Scroll : LAB_00EF=5, LAB_00F0=15
    BSR LAB_001F  → 5 frames d'animation loot
    [41 frames non-interactif via LAB_0038]
    JSR LAB_0007  → run (joueur confirme le butin)

    Animation LAB_00EB détail :
        Sprites $14 1c..1b  = créature/monstre vaincue (frames de mort)
        Sprites $14 24/25/26/27 = objet(s) droppé(s) (4 types possibles)

Phase 3 — Victory walkoff :
    Spawn : LAB_00E9 + LAB_00E8 (état sélectionné sur la carte)
    JSR LAB_0007  → run (chevalier marche hors de l'écran)

    Animation LAB_00EC détail :
        Sprites $14 29..36  = 14 frames de sortie du chevalier vainqueur

Nettoyage :
    MOVEA.L LAB_00C6,A0 ; JSR LAB_054D  → désallouer buffer courant
    MOVEA.L LAB_056C,A0 ; JSR LAB_054D  → désallouer buffer secondaire
```

#### Différence encounter vs sanctuaire

| Type de nœud       | Fond       | Sprites loot        | Palette |
|--------------------|------------|---------------------|---------|
| Encounter (monstre)| dw1/bg3    | `$14 1c..27`        | P4/P5   |
| Sanctuaire (relique)| bg3       | `$14 24..27` (fixe) | P5      |
| Boss final         | bg5        | `$14 24..27` × 4    | P8      |

---

### 10.17 Progression et condition de victoire

#### Vue d'ensemble du déroulement d'une partie

```text
ROUND N :
  ┌── LAB_001A : combat intro (tutoriel ou scène cinématique)
  │   LAB_001C : combat 2 joueurs
  │   LAB_0174 : copie palettes entre buffers
  │   LAB_001A : scène combat principale (boucle LAB_0007)
  │   LAB_002C : boutique joueur 1 + boutique joueur 2
  │   LAB_002D : rencontre/taverne (fond bg4/5)
  │   LAB_002F : événement/sanctuaire (fond bg3, spawn LAB_00E4+E5)
  └── LAB_002E : boutique joueur 2 supplémentaire

"Loading..." → rechargement via SECSTRT_4 → ROUND N+1
```

#### Les 4 reliques (clés)

Les sanctuaires sont des nœuds spéciaux sur la carte overworld. Chaque sanctuaire est gardé par une créature (`LAB_002F` → spawn `LAB_00E4/E5`). La victoire donne une relique (CEL `$14 24..27`). Collecter les 4 reliques est requis pour accéder au nœud du boss final.

#### Le boss final

Le boss final est vraisemblablement associé au fichier `co1.cel` + `co.stile` (background de boss = `co.stile`). La scène de boss utilise la même structure que les combats normaux (`LAB_001C`) mais avec des sprites différents et une durée plus longue. Après victoire, la Moonstone (objet CEL) est droppée.

#### Phase de lune et victoire

Les druides indiquent la phase de lune correcte au début de la partie (texte d'intro). La phase de lune avance d'un cran à chaque round complet :

```text
Phases : 0..7 (cycle de 8 rounds = 1 cycle lunaire complet)
Condition de victoire :
    - Moonstone en possession du chevalier  ET
    - Phase de lune actuelle = phase cible (annoncée par les druides)
    - Position du chevalier = nœud Stonehenge
→ Déclenchement de LAB_003B (cinématique de fin)
```

Le compteur de phase de lune est probablement un champ dans `SECSTRT_33` (la BSS de carte, 960 longs) ou dans le bloc d'état persistant rechargé via `SECSTRT_4`.

#### Comportement multi-joueurs (1 à 4 joueurs)

- Bits 0..6 de `LAB_0005` (copié de `EXT_0007` = argument CLI/WB) configurent les couleurs de ciel et de sol (3 options ciel × 4 options sol).
- Bit 7 de `LAB_0005` : `1` = **mode démo/attract** (boucle `LAB_0001` avec cinématique automatique), `0` = mode jeu normal.
- Le nombre de joueurs humains (1–4) est encodé dans les bits non encore complètement analysés de `EXT_0007`.
- En mode 2 joueurs (`LAB_003E = 2`), `LAB_003C` n'affiche que 2 des 4 barres HUD.

---

| Symbole      | Section | Rôle                                                               |
|--------------|---------|--------------------------------------------------------------------|
| `LAB_0123`   | S_2     | Mode de jeu courant (0=overworld, 4=combat actif)                  |
| `LAB_011E`   | S_2     | Type de scène (2=cinéma, 3=événement, 4=map, 5=combat)            |
| `LAB_011D`   | S_2     | Pointeur contexte palette courante (→ LAB_01CB..LAB_01D2)          |
| `LAB_011F`   | S_2     | Flag « ne pas switcher décor ce frame » (1 = inhiber LAB_000F)    |
| `LAB_0120`   | S_2     | Flag de réinitialisation de scène                                  |
| `LAB_0121`   | S_2     | Pointeur vers le buffer de sprites actifs (LAB_027C)               |
| `LAB_0123`   | S_2     | Timer/vitesse globale du jeu (LAB_00D0 = délai entre frames)       |
| `LAB_00D0`   | S_2     | Délai cible en ticks par frame (4=vite, 6=normal, 8=lent)         |
| `LAB_00D1`   | S_2     | Flag « mode 2 joueurs » (1 = actif)                                |
| `LAB_003E`   | S_0     | Mode boutique/événement (0=normal, 2=boutique)                     |
| `LAB_0026`   | S_0     | Compteur de frame courant dans l'animation table                   |
| `LAB_0028`   | S_0     | Compteur de frames dans la boucle LAB_000B                         |
| `LAB_0029`   | S_0     | Nombre max de frames d'animation / longueur de la boucle           |
| `LAB_002B`   | S_0     | Paramètre initial du compteur LAB_0028 (copié au démarrage)        |
| `LAB_05E7`   | S_33    | Flag fin de partie / erreur disque (0=normal, ≠0=quitter)          |
| `LAB_0033/4/5` | S_0   | Handles des 3 barres de score (fade animations)                    |
| `SECSTRT_9`  | S_9     | Couleur du fond de score bar (joueur courant)                      |
| `LAB_01C9`   | S_9     | Couleur de score HP                                                |
| `LAB_01CA`   | S_9     | Couleur de score MP                                                |

---

### 10.18 Structure de données du chevalier (`KnightStruct`) — `mog`

#### Localisation dans `mog`

Cinq instances consécutives de 132 octets (33 longs) :

| Label       | Joueur          |
|-------------|-----------------|
| `LAB_0613`  | Chevalier bleu  |
| `LAB_0614`  | Chevalier rouge |
| `LAB_0615`  | Chevalier vert  |
| `LAB_0616`  | Chevalier jaune |
| `LAB_0617`  | Chevalier noir (ennemi IA) |

Tableau des 4 pointeurs actifs : `LAB_05E4` (4 longs → `LAB_0613..0616`).  
Pointeur vers le chevalier courant : `LAB_068B`.

#### Champs de la structure (offsets en octets depuis le début du struct)

| Offset | Taille | Nom C suggéré        | Description |
|--------|--------|----------------------|-------------|
| 0      | —      | —                    | (premier long utilisé par le système d'objets, non modifié directement) |
| 2      | byte   | `item_healing_potion`| Nombre de Potions de Guérison (max 5) |
| 4      | byte   | `item_gem_seeing`    | Nombre de Gemmes de Clairvoyance (max 5) |
| 6      | byte   | `item_ring_protect`  | Nombre d'Anneaux de Protection (max 5 ; donne +40 HP à la collecte) |
| 8      | byte   | `item_sword_sharp`   | Nombre d'Épées d'Acuité (max 5) |
| 10     | byte   | `item_talisman_wyrm` | Nombre de Talismans du Wyrm (max 5) |
| 12     | byte   | `item_scroll_acq`    | Nombre de Parchemins d'Acquisition (max 5) |
| 14     | byte   | `item_scroll_haste`  | Nombre de Parchemins de Rapidité (max 5) |
| 16     | byte   | `item_scroll_hawk`   | Nombre de Parchemins de l'Épervier (max 5) |
| 18     | byte   | `item_scroll_wyrm`   | Nombre de Parchemins du Wyrm (max 5) |
| 54     | long   | `knight_id`          | Type du chevalier : 0=Bleu, 1=Rouge, 2=Vert, 3=Jaune, 4/5=Noir |
| 70     | byte   | `moonstone_strength` | Lune Noire / Force — collecte via items 0x46 (max 5 ; ajoute au dommage en combat) |
| 71     | byte   | `moonstone_constitution` | Demi-Lune / Constitution — items 0x47 (max 5 ; +10 HP à la collecte) |
| 72     | byte   | `moonstone_endurance`| Pleine Lune / Endurance — items 0x48 (max 5 ; ajoute à la défense) |
| 73     | byte   | `skill_level`        | Niveau de Compétence/Agilité 0..5 (augmenté au temple de guérison, coût 15 or ; max 5) |
| 74     | word   | `gold`               | Or possédé (pièces d'or) |
| 76     | byte   | `daggers`            | Nombre de dagues (max ~10, coût 2 or chacune) |
| 78     | word   | `battles_fought`     | Compteur de combats/rencontres cumulées : +1 après chaque combat gagné, +2 après combat de dragon, +3 après victoire sur le Dragon Noir |
| 80     | word   | `hp_current`         | Points de vie actuels (0 = mort → respawn, décrémenté pendant le combat) |
| 83     | byte   | `enemy_flag`         | 0xFF pour les ennemis IA, 0 pour les joueurs |
| 84     | word   | `hp_max`             | Points de vie maximum — calculé par `LAB_0013` : `constitution×10 + ring_of_protection×20 + armor_bonus + 10` |
| 88     | long   | `sword_type`         | Type d'épée : 0x16=Longue, 0x17=Large, 0x18=Claymore, 0x19=Épée d'Acuité |
| 92     | long   | `armor_type`         | Type d'armure : 0x1B=Rembourée, 0x1C=Mailles, 0x1D=Plates, 0x1E=Combat |
| 96     | long*  | `linked_knight`      | Pointeur vers le struct du chevalier lié (adversaire ou équipier) |
| 100    | long   | `flag_100`           | Initialisé à 0 (usage inconnu) |
| 108    | long*  | `ai_data`            | Pointeur vers données IA/NPC (ex: `LAB_08C0` pour les Chevaliers Noirs) |
| 126    | word   | `spawn_x8`           | Position X de spawn × 8 |
| 128    | word   | `spawn_y8`           | Position Y de spawn × 8 |
| 130    | byte   | `dead_flag`          | Flag mort/respawn : 0 = vivant, autre = mort |

#### Initialisation par défaut

**Chevalier joueur** (`LAB_0195`) :
- HP courant = 120, HP max = 120 (valeur initiale : constitution=1 → 10 + 10 base = 20, mais le jeu initialise directement à 120)
- Épée = 0x16 (Longue), Armure = 0x1B (Rembourrée)
- Or = 10, Dagues = 0, Compétence = 0, Lunes = 0

**Formule HP max (`LAB_0013` de mog.asm)** :
```
hp_max = Constitution × 10
       + inv[6] (Ring of Protection) × 20
       + (armor == 0x1c ? +10 : armor == 0x1d ? +20 : armor == 0x1e ? +30 : 0)
       + 10
```
**Mort et respawn (`LAB_000E`)** : quand `hp_current ≤ 0`, le chevalier mourrant voit ses HP restaurés à `hp_max` et perd 1 niveau de skill (`73(knight) -= 1`). Le flag `LAB_05DC` (bit 0 = chevalier 0 mort, bit 1 = chevalier 1 mort) est activé.
**Level-up (`LAB_052F`)** : quand `hp_current == hp_max` (pleinement restauré, e.g. après un soin au temple), skill +1 (cap 5).

**Chevalier ennemi Black Knight** (`LAB_01C6`) :
- Force=1, Constitution=1, Endurance=1 (une lune de chaque)
- Compétence = 5, HP = 20
- Épée = 0x16, Armure = 0x1B
- Or = 10, Dagues = 10, `enemy_flag` = 0xFF

#### Mécanique des Moonstones (Lunes)

Les moonstones collectés sur la carte (items 0x46/0x47/0x48) incrémentent
directement les octets de Force, Constitution et Endurance du chevalier
(table `LAB_090E`). Ce même trio de valeurs est affiché sur l'écran de
statistiques et utilisé dans les calculs de combat :

- **Force** (`70(A0)`) → ajoutée au jet de dommage (`LAB_021B`)
- **Constitution** (`71(A0)`) → +10 HP par lune collectée ; base défense
- **Endurance** (`72(A0)`) → ajoutée à la valeur de résistance (`LAB_0278`)

Le **sorcier** (Witch Doctor, écran type 3) peut modifier aléatoirement
ces stats de ±1 (tables `LAB_095B`/`LAB_095C`), avec les messages :
- Gain Force : *"The cosmos has granted you more strength."*
- Gain Constitution : *"The cosmos has granted you more constitution."*
- Gain Endurance : *"I will grant you more endurance."*

#### Prix des équipements (boutique armurier)

| Objet              | Coût (or) | Effet sur le struct |
|--------------------|-----------|---------------------|
| Armure de Mailles  | 30        | `armor_type = 0x1C`, `hp_max += 10` |
| Armure de Plates   | 50        | `armor_type = 0x1D`, `hp_max += 20` |
| Armure de Combat   | 75        | `armor_type = 0x1E`, `hp_max += 30` |
| Épée Large         | 10        | `sword_type = 0x17` |
| Claymore           | 25        | `sword_type = 0x18` |
| Dague              | 2/pièce   | `daggers++` |

---

### 10.19 Machine à états interactive — `mog.asm`

Le binaire `mog` est le noyau du jeu interactif. Il possède sa propre machine à états, indépendante de `program`, pilotée par la variable `LAB_068F` (long, section BSS de mog).

#### Point d'entrée de scène — `LAB_04CF` ([mog.asm#L10547](mog.asm#L10547))

```text
LAB_04CF:
    LEA  LAB_05E2,A0
    MOVE.L 4(A0),LAB_0986   ; pointer de scène courante
    CLR.W  LAB_0689         ; effacer flag "restored"
    MOVE.L D0,LAB_068F      ; stocker l'état reçu en D0
    CLR.W  LAB_0984         ; effacer flag de sortie
    JSR    LAB_03F0          ; effacer écran
    JSR    LAB_0575          ; fade-in fond
    JSR    LAB_0588          ; init sprites
    JSR    LAB_04D4          ; setup de l'écran → LAB_04D0 (boucle)
```

**Boucle de rendu** `LAB_04D0` :
1. Lecture joystick (`LAB_0451`) → D0 = bouton pressé, D1 = direction
2. Si bouton → `LAB_052A` (dispatch selon item/action survolée)
3. Si `LAB_0984 ≠ 0` → sortie vers l'appelant
4. Sinon → `LAB_0416` (sync VBL), `LAB_039E` (update sprites), retour en 1

#### Registre d'état `LAB_068F`

| Valeur | Écran           | Description                                                                  | Pointeurs actifs             | Layout principal |
|--------|-----------------|------------------------------------------------------------------------------|------------------------------|-----------------|
| `1`    | PvP             | Duel entre deux chevaliers joueurs                                           | LAB_068B=k0, LAB_068D=k1     | `LAB_0694`      |
| `2`    | Créature        | Combat contre créature/NPC (struct `LAB_08C6`)                               | LAB_068B=k0, LAB_068D=ennemi | `LAB_069A`+`LAB_0698` |
| `3`    | Sorcier/Mystic  | Witch Doctor / Mythral the Mystic — échange moonstones ↔ skill              | LAB_068B=k0                  | `LAB_0696`      |
| `5`    | Arène           | Combat dans l'arène d'une ville                                              | LAB_068B=k0                  | `LAB_0695`      |
| `6`    | Carte/Inventaire| Carte overworld + interaction items/NPC                                      | LAB_068B=k0                  | `LAB_0695`+`LAB_0697` |
| `8`    | Armurier        | Boutique armurier (achat épée, armure, dagues)                               | LAB_068B=k0                  | `LAB_0694`      |
| `9`    | Temple          | Temple de guérison (soins, achat skill points)                               | LAB_068B=k0                  | `LAB_0693`      |
| `10`   | Dragon          | Combat contre le Dragon Noir (`LAB_0617`)                                    | LAB_068B=k0, LAB_068D=dragon | `LAB_0694`+`LAB_0698` |
| `11`   | Taverne         | Taverne — jeu de dés ou interaction sociale                                  | LAB_068B=k0, LAB_068D=k1     | `LAB_0694`      |

#### Chevaliers joueurs

- `LAB_05E4[0]` = chevalier principal → toujours `LAB_068B` (+ `LAB_068C = 96(LAB_068B)`)
- `LAB_05E4[1]` = second chevalier (états 1, 8, 11) → `LAB_068D`
- État 2 : `LAB_068D = LAB_08C6` (struct de la créature ennemie rencontrée)
- État 10 : `LAB_068D = LAB_0617` (struct du dragon / Chevalier Noir)

**Noms des chevaliers** (définis en section S_4, [mog.asm#L13056](mog.asm#L13056)) :

| Label      | Nom         | Faction (`54(knight)`) | Couleur  |
|------------|-------------|------------------------|----------|
| `LAB_06B5` | SIR_RICHARD | 0                      | bleu     |
| `LAB_06B6` | SIR_GODBER  | 1                      | rouge    |
| `LAB_06B7` | SIR_JEFFREY | 2                      | vert     |
| `LAB_06B8` | SIR_EDWARD  | 3                      | jaune    |

#### Options du menu initial

Disponibles lors du menu avant la partie (section S_4) :

| Label      | Texte           | Description                               |
|------------|-----------------|-------------------------------------------|
| `LAB_06BC` | `"Players"`     | Nombre de joueurs (1 à 4)                 |
| `LAB_06BD` | `"Gore"`        | Activer / désactiver le mode gore         |
| `LAB_06BE` | `"Practice"`    | Mode entraînement                         |
| `LAB_06BF` | `"Select Knight"` | Choisir le chevalier avant la partie    |

#### Nœuds de la carte overworld — `LAB_069F`

Table de triplets `(type:word, x:word, y:word)` terminée par `$FFFF`. Lue par `LAB_006B`, comparée à la position du chevalier courant (`126(knight) = X overworld`, `128(knight) = Y overworld`). Quand le chevalier est au bon endroit (`LAB_0067` retourne D5=2), le type de nœud déclenche une interaction.

**Dispatcher `LAB_0E45`** (entrée principale de déclenchement) :
- `type == 0x01 || 0x21` → `LAB_004F` (rencontre chevalier → PvP ou échange)
- `type == 0x02` → `LAB_005B` (rencontre créature → état 2)
- autres types → `LAB_007B` (switch étendu, voir tableau ci-dessous)

**Switch étendu `LAB_007B`** :

| Type (dec) | Type (hex) | Gestionnaire  | Déclenchement et condition                                         |
|------------|------------|---------------|--------------------------------------------------------------------|
| 21         | `0x15`     | `LAB_00B0`    | Château de Richard — skill+1 si faction=0, puis état 9             |
| 22         | `0x16`     | `LAB_00B0`    | Château de Godber — skill+1 si faction=1, puis état 9              |
| 23         | `0x17`     | `LAB_00B0`    | Château de Jeffrey — skill+1 si faction=2, puis état 9             |
| 24         | `0x18`     | `LAB_00B0`    | Château d'Edward — skill+1 si faction=3, puis état 9              |
| 25         | `0x19`     | `LAB_0093`    | Ville avec armurier + arène + sorcier (menu 5 boutons)             |
| 26         | `0x1a`     | `LAB_008A`    | Ville avec taverne + arène (menu 5 boutons)                        |
| 27         | `0x1b`     | `LAB_00A1`    | Sorcier Mythral → état 3                                           |
| 28         | `0x1c`     | `LAB_009D`    | Antre du Dragon → état 10 (condition : `inventaire[20] == 0x0f`)   |
| 30         | `0x1e`     | `LAB_007C`    | Temple de guérison → état 9                                        |
| 33         | `0x21`     | `LAB_004F`    | Rencontre chevalier (duel PvP)                                     |

**Menu d'une ville type 0x19 (`LAB_0093`) :**
- Bouton 1 → état 5 (combat dans l'arène)
- Bouton 2 → `LAB_04A6` (achat potion ou soin rapide)
- Bouton 3 → `LAB_048E` (sorcier de guérison)
- Bouton 4 → état 6 (boutique armurier)
- Bouton 5 → quitter

**Menu d'une ville type 0x1a (`LAB_008A`) :**
- Bouton 1 → état 5 (arène)
- Bouton 2 → `LAB_04A6` (achat)
- Bouton 3 → `LAB_048E` (sorcier)
- Bouton 4 → `LAB_047C` (taverne ?)
- Bouton 5 → quitter

**Antre du Dragon (type 0x1c, `LAB_009D`) :**
- Condition d'entrée : `inventaire[20] == 0x0f` (item "clé du dragon / quatrième relique")
- Si `LAB_05DC bit 0` NON activé → perd 2 niveaux de skill → redirigé vers état 9
- Si `LAB_05DC bit 0` activé → victoire : `78(knight) += 3`, efface item 20, séquence de fin de partie (`LAB_0DCA`)

#### Résolution du combat — transfert d'items (`LAB_001C`)

Appelé après chaque combat (PvP ou créature) pour transférer les possessions du perdant au gagnant.

**Table des offsets inventaire `LAB_0028`** : `[0x14, 0x06, 0x0e, 0x08, 0x0c, 0x00, 0x0a, 0xFFFF]`

- Items normaux (compteurs) : `-1` au perdant, `+1` au gagnant
- Item `0x14` (type d'armure) et `0x16` (type d'épée) : OR logique — le gagnant conserve le meilleur équipement
- Item `0x04` : si présent → `88(perdant) = 0x19` (conversion en Sharp Sword ?) puis transfert

**Or après combat (`LAB_0021`)** — déclenché si le coup final = type `0x14` :
```text
gold_gagnant += gold_perdant / 2
gold_perdant  = gold_perdant / 2   (moitié perdue)
```

**Si skill du perdant == 0 (`LAB_0022`) :** TOUS les items sont transférés.

#### Calcul des HP max — `LAB_0013`

```text
HP_max = Constitution × 10
       + inventaire[6] × 20    (Ring of Protection : +20 HP chacun)
       + (armor==0x1c ? 10 : 0) (Mailles : +10)
       + (armor==0x1d ? 20 : 0) (Plates : +20)
       + (armor==0x1e ? 30 : 0) (Combat : +30)
       + 10                     (base)
→ stocké dans 84(knight) = hp_max
; si hp_max > hp_current → ne pas toucher hp_current
```

#### Mort et respawn — `LAB_000E`

```text
; Vérifie les deux chevaliers après le combat
if 80(k0) <= 0:          ; HP knight0 épuisé ?
    LAB_05DC bit 0 = 1   ; flag mort knight0
    80(k0) = 84(k0)      ; restaurer HP au max
    73(k0) -= 1          ; perdre 1 niveau de skill
if 80(k1) <= 0:
    LAB_05DC bit 1 = 1
    80(k1) = 84(k1)
    73(k1) -= 1
```

Les châteaux (types 0x15..0x18) inversent cela en accordant +1 skill à la place d'une pénalité, avant de renvoyer vers le temple.

---

## 11. Stratégie de portage C

### 11.1 Principe directeur : zéro conversion d'assets

Le portage ne doit **jamais nécessiter de convertir les fichiers originaux** en un autre format. Les fichiers `.cel`, `.PIV`, `.stile`, `.cmp` et `.ob` tels que livrés sur la disquette originale sont chargés et décodés à l'exécution par la bibliothèque. Cela garantit :

- **Authenticité** : les pixels, les sons, les palettes sont exactement ceux de l'original.
- **Maintenabilité** : pas de pipeline de build d'assets à maintenir.
- **Débogage direct** : remplacer un fichier sur le disque suffit pour tester une variante.
- **Distribution légale simplifiée** : les fichiers originaux sont les seuls assets nécessaires.

---

### 11.2 `libmoon_assets` — bibliothèque de chargement

#### 11.2.1 Responsabilités

`libmoon_assets` est la première brique à implémenter. Elle encapsule **toute la connaissance des formats de fichiers** et expose une API uniforme indépendante du format sous-jacent.

```
libmoon_assets
├── Décompression in-memory
│   ├── RNC ProPack 1   → music.cmp, vmusic.cmp
│   ├── LZSS Mindscape  → *.cel, *.CEL, bold.f
│   └── RLE 2-bit       → *.stile
├── Parsing de format
│   ├── CEL             → frames indexées + métadonnées
│   ├── PIV             → bitmap planaire + palette Amiga
│   ├── STILE           → tableau de tuiles
│   ├── CMP             → données module (opaque, pour le player audio)
│   └── OB              → données objet brutes
└── Cache fichiers
    └── hash de nom → évite les chargements redondants
```

#### 11.2.2 API publique proposée

```c
/* --- Initialisation ----------------------------------------- */
int moon_init(const char *asset_dir);   /* chemin vers le dossier des fichiers */
void moon_shutdown(void);

/* --- CEL (sprites) ------------------------------------------ */
typedef struct {
    int       frame_count;
    int       width, height;
    uint8_t **frames;   /* pixels indexés 8-bit, frame_count pointeurs */
} MoonCel;

MoonCel  *moon_cel_load(const char *name);   /* ex: "au1.cel" */
void      moon_cel_free(MoonCel *cel);

/* --- PIV (fonds d'écran) ------------------------------------ */
typedef struct {
    int      planes;        /* 4 ou 5 */
    int      width, height; /* 320×200 standard */
    uint8_t *bitmap;        /* données planaires (planes × width/8 × height) */
    uint16_t palette[32];   /* couleurs Amiga 12-bit $0RGB */
} MoonPiv;

MoonPiv  *moon_piv_load(const char *name);   /* ex: "bg1a.PIV" */
void      moon_piv_free(MoonPiv *piv);

/* --- STILE (tilemaps) --------------------------------------- */
typedef struct {
    int      tile_count;
    uint8_t *data;          /* données RLE décompressées */
} MoonStile;

MoonStile *moon_stile_load(const char *name);
void       moon_stile_free(MoonStile *stile);

/* --- CMP (modules audio) ------------------------------------ */
typedef struct {
    size_t   size;
    uint8_t *data;    /* module SoundTracker décompressé, prêt pour le player */
} MoonMod;

MoonMod  *moon_mod_load(const char *name);   /* ex: "music.cmp" */
void      moon_mod_free(MoonMod *mod);

/* --- OB (données objet) ------------------------------------- */
typedef struct {
    size_t   size;
    uint8_t *data;
} MoonOb;

MoonOb   *moon_ob_load(const char *name);
void      moon_ob_free(MoonOb *ob);
```

#### 11.2.3 Implémentation des décompresseurs

Chaque décompresseur est isolé dans son propre fichier source sans dépendance au reste :

| Fichier source | Algorithme | Référence ASM | Taille estimée |
|---------------|------------|---------------|----------------|
| `rnc1.c` | RNC ProPack 1 (Huffman + LZ) | `LAB_0190` | ~150 lignes C |
| `lzss_cel.c` | LZSS Mindscape (fenêtre 2 Ko, longueur 2..34) | `LAB_049C` | ~60 lignes C |
| `rle_stile.c` | RLE 2 bits (opcodes `00`/`01`/`10`/`11`) | `LAB_0448` | ~80 lignes C |
| `packbits_piv.c` | PackBits maison (format PIV Mindscape) | `LAB_0434` | ~50 lignes C |

Chacun expose une fonction `int decompress_xxx(const uint8_t *src, size_t src_len, uint8_t *dst, size_t dst_len)` retournant les octets écrits ou `-1` en cas d'erreur.

#### 11.2.4 Gestion mémoire et cache

```c
/* Cache interne (table de hachage nom → pointeur) */
/* Même logique que LAB_038F/LAB_0390 (§6.1.2) mais triviale en C : */
typedef struct CacheEntry {
    char  name[16];
    void *asset;
    int   refcount;
} CacheEntry;
/* ~72 buckets suffisent (cf. SECSTRT_19 original) */
```

Le cache est optionnel — le moteur peut le bypasser pour les assets volatils (ex : animation one-shot de cinématique).

---

### 11.3 Suite d'outils de débogage (`moon-tools`)

Construite **au-dessus de `libmoon_assets`**, cette suite d'utilitaires en ligne de commande permet d'inspecter chaque fichier directement, sans lancer le jeu complet. Elle sert à valider les décompresseurs et à explorer les assets.

#### Outils prévus

| Outil | Usage | Sortie |
|-------|-------|--------|
| `moon-info <fichier>` | Afficher métadonnées (magic, format, nb frames, taille compressée/décompressée) | texte |
| `moon-view-cel <fichier.cel> [frame]` | Rendre une frame CEL en ASCII-art ou PNG | PNG / terminal |
| `moon-view-piv <fichier.PIV>` | Décoder et afficher un fond d'écran | PNG |
| `moon-view-stile <fichier.stile>` | Afficher les tuiles décompressées | PNG |
| `moon-dump <fichier> <out.bin>` | Décompresser brut vers fichier binaire | fichier |
| `moon-palette <fichier.PIV>` | Afficher la palette en hexa + swatches ANSI | texte |
| `moon-mod-info <fichier.cmp>` | Afficher les métadonnées du module (BPM, instruments, patterns) | texte |

#### Exemple d'invocation

```sh
# Inspecter un fichier
$ moon-info bg3.PIV
Fichier : bg3.PIV  (20984 octets)
Format  : PIV Mindscape propriétaire
Plans   : 5 (32 couleurs)
Raw     : ~40000 octets  →  ratio 0.52
Algo    : PackBits maison (LAB_0434)

# Extraire la frame 0 d'un sprite en PNG (pour comparaison émulateur)
$ moon-view-cel au1.cel 0 --out frame0.png

# Vérifier la palette d'un fond
$ moon-palette bg8.PIV
00: #000  01: #A75  02: #753  ...

# Dumper le module audio décompressé pour l'injecter dans un tracker
$ moon-dump music.cmp music_raw.mod
Décompressé : 88187 → 141320 octets (RNC1)
```

---

### 11.4 Ordre de développement recommandé

| Étape | Composant | Dépendances | Critère de validation |
|-------|-----------|-------------|----------------------|
| 1 | `rnc1.c` | aucune | `moon-dump music.cmp` produit un MOD jouable |
| 2 | `lzss_cel.c` | aucune | `moon-view-cel au1.cel 0` produit la bonne image |
| 3 | `packbits_piv.c` | aucune | `moon-view-piv bg1a.PIV` ≡ capture WinUAE |
| 4 | `rle_stile.c` | aucune | tiles visibles dans `moon-view-stile` |
| 5 | `libmoon_assets` | étapes 1–4 | API complète + cache + `moon-info` fonctionnel |
| 6 | Renderer planar | libmoon_assets + SDL2 | affichage d'un fond PIV + sprite CEL superposé |
| 7 | Player audio | libmoon_assets + SDL2 audio | musique jouée en boucle |
| 8 | Input / VBL | SDL2 events | polling joystick 50 Hz |
| 9 | Job manager | libmoon_assets + renderer | entités animées sur la carte overworld |
| 10 | Logique de jeu | tout le reste | `mog` interactif (§10.19) |

---

### 11.5 Modules par complexité de portage

| Module | Complexité | Décision |
|--------|------------|---------|
| Loader hunks (`SECSTRT_4`) | — | **Supprimer** — remplacé par `libmoon_assets` |
| Pilote disque (`SECSTRT_13`) | — | **Supprimer** — remplacé par `fopen` |
| Cache fichiers (`SECSTRT_18`) | Facile | Porter (hash 72 buckets, ~100 lignes C) |
| Interruptions (`SECSTRT_15`) | Moyenne | SDL timer 50 Hz + event loop |
| Player audio (`SECSTRT_1`) | Moyenne | ProTracker bien documenté ; RNC1 via `libmoon_assets` |
| Renderer IMAGEXCEL (`SECSTRT_23`) | **Difficile** | Bit-planaire + cookie-cut + clip ; conserver le mode planaire pour rester compatible avec les CEL d'origine |
| Palette cycling (`SECSTRT_31`) | Facile | Tables + interpolation R/G/B |
| Job manager (`SECSTRT_10`) | Moyenne | 40 entrées, vtable, scripts bytecode |
| Logique overworld + combat (`mog`) | **Difficile** | §10.19 comme référence principale |
| Init custom-chip (`SECSTRT_29`) | — | **Supprimer** — reconfigurer SDL/OpenGL à la place |

---

### 11.6 Pièges spécifiques au portage 68000 → C

1. **Endianness** — Le 68000 est *big-endian*. Sur toute cible little-endian (x86/ARM), les champs `u16`/`u32` lus depuis les fichiers nécessitent un swap. `libmoon_assets` centralise ces swaps — le reste du code n'y touche pas.
2. **Pointeurs sur chip RAM** — Le code patche des pointeurs absolus directement dans la copper list. En C, isoler dans des structures ; ne pas tenter de reproduire la séparation chip/fast RAM.
3. **Variables dans le segment code** — Plusieurs valeurs « immédiates » (`LAB_0312`, `LAB_038C+2`) sont en réalité des **variables** modifiées au runtime et stockées dans le segment code. Déclarer comme variables C normales.
4. **Conventions d'appel registres** — Chaque routine 68k reçoit ses arguments dans des registres spécifiques (ex : `LAB_04B5` : D0=frame, D1=x, D2=y, A0=cel). Documenter chaque contrat avant de porter.
5. **`MULU`/`DIVU`/`DIVS`** — Opèrent sur 32/16 bits avec sémantique 68k. Traduire en `uint32_t`/`uint16_t` explicites, jamais en `int` bare.
6. **`DBF`/`DBcc`** — Décrémente puis branche si `D > −1` (pas `> 0`). Traduire par `for (i = N; i >= 0; i--)`.
7. **Timing VBL** — Certaines animations comptent les VBL à 50 Hz PAL. Viser exactement 50 Hz (ou adapter les compteurs au taux réel).
8. **`TRAP #15`** — Déclaré mais jamais appelé ; vecteur réservé pour le débogueur d'origine. Ignorer.
9. **Masques CIA** (`ANDI.B #$fb,CIAB_PRB`) — Accès en lecture-modification-écriture avec inversions de bits. Bien commenter chaque masque ; tous sont remplacés par des appels SDL dans le port.

---

## 12. Conventions et conseils méthodologiques

1. **Commencer par `libmoon_assets`** (§11.2) — c'est le prérequis à tout le reste. Valider chaque décompresseur avec `moon-tools` (§11.3) avant d'attaquer le renderer.
2. **Ne jamais convertir les assets** — si une routine a besoin de pixels chunky, elle convertit en mémoire à partir des données PIV/CEL originales. Aucun fichier PNG/WAV ne doit exister dans le dépôt.
3. **Tests de non-régression** : valider chaque décompresseur en comparant la sortie de `moon-view-*` pixel-par-pixel avec une capture émulateur WinUAE / FS-UAE sur le jeu original.
4. **Réécrire par interfaces**, pas instruction par instruction — identifier les contrats d'API entre modules (§9) et réécrire chaque module en C idiomatique en respectant ces contrats.
5. **Émulateur Amiga ouvert en permanence** pendant le portage — poser des breakpoints sur `LAB_04B5` (draw_cel) pour observer les paramètres réels passés au renderer, `LAB_04CF` pour les transitions d'état.
6. **Loader de hunks et pilote disque** — supprimer (`SECSTRT_4`, `SECSTRT_13`) ; remplacés par `moon_init(asset_dir)` + `fopen`/`fread` internes.

---

## Annexe A — Inventaire des constantes hardware référencées

(Voir le compte précis fait par `grep` ; les plus utilisés.)

| Registre | Occurrences | Rôle dans le programme                          |
|----------|------------:|-------------------------------------------------|
| `INTREQ` | 20          | Acquit interruptions                            |
| `CIAB_PRB` | 17        | Pilotage moteur floppy + step / select          |
| `BLTCON0` | 15         | Mode blitter (canaux + minterm)                 |
| `BLTSIZE` | 12         | Déclenchement blit (hauteur*64+largeur/16)      |
| `DMACON` | 11          | Activation DMA (bitplane/blit/audio/disk)       |
| `BLTDPTH/APTH/CPTH/BPTH` | 11+10+6+6 | Pointeurs blitter                       |
| `DSKLEN`  | 9          | Démarrage DMA disque (toujours 2 écritures !)   |
| `BLTAMOD/BMOD/CMOD/DMOD` | 9+5+5+10 | Modulos blitter                          |
| `INTENA`  | 5          | Configuration mask interruptions                |
| `AUD0..3VOL` | 6 × 4   | Volume canaux                                   |
| `AUD0..3LCH` | 3 × 4   | Adresse sample (par canal)                      |
| `BPLCON0/1` | 2+2      | Mode display (5 plans, scrolling)               |
| `COP1LCH/COPJMP1` | 2     | Copper list base                                |
| `JOY0DAT/JOY1DAT` | 4+3   | Joystick poll                                   |
| `DIWSTRT/DIWSTOP` | 3+2   | Fenêtre visible                                 |
| `DDFSTRT/DFFSTOP` | 2+2   | DMA fetch window                                |

---

## Annexe B — Pistes à creuser

Éléments **non analysés** par cette première passe et nécessaires pour un
portage *complet* :

1. **Format exact d'un fichier CEL** — vérifier sur `co1.cel` ouvert en
   binaire et comparer à `LAB_04A8` / `LAB_04B5`.
2. **Format exact d'un fichier PIV** — déterminer si la palette est en
   tête ou en queue ; vérifier le mode interleaved/séquentiel.
3. **Format `.stile`** — examiner via le code de chargement de
   `intro.stile` (recherche `LEA LAB_0274,A0 ; JSR SECSTRT_31` dans
   `SECSTRT_0`).
4. **Format `.cmp`** — comparer un dump `music.cmp` à un MOD ProTracker
   standard (vérifier si les 1084 premiers octets sont identiques).
5. **Logique détaillée S_10** — le job manager (`SECSTRT_10`) popule une vtable de 20 handlers (`LAB_0288`) dont certains ne sont pas encore complètement documentés (handlers 5..20).
6. **Trace de TRAP_15** — vérifier qu'il n'est jamais déclenché. Si oui,
   c'est un vecteur de débogage à ignorer.
7. **Vérifier l'utilisation des registres `EXT_0008..b`** : ce sont
   semble-t-il les *vecteurs de continuation* du loader d'overlays.
8. **`Crystal`, `iraosx`** — fichiers de petite taille à
   identifier (`Crystal` est un binaire Hunk de 17 Ko ; `iraosx` est
   l'outil macOS IRA v2.11, hors jeu). `nb` et `mog` sont désormais
   entièrement documentés (§6.6 et §10.19).

---

## Annexe C — Catalogue complet des fichiers assets

Extrait par analyse des chaînes de caractères des binaires `program` et `mog`.

### C.1 Assets chargés par `program` (intro / chargeur, ~60 Ko)

#### Sprites animés (`.cel`)

| Fichier | Rôle probable |
|---|---|
| `au1.cel` | Écran-titre / intro |
| `co1.cel` | Logo / crédits |
| `da1.cel` | Animation intro |
| `dw1.cel` | Animation intro |
| `ha1.cel` | Animation intro |
| `li1.cel` | Animation intro |
| `ov1.cel` | Fond carte overworld |
| `dg1.cel` | Animation intro |
| `Klift1.CEL` | Animation intro |

#### Fonds bitmap (`.piv`)

| Fichier | Rôle |
|---|---|
| `bg1a.piv`, `bg1b.piv`, `bg1c.piv` | Scène intro (3 plans) |
| `bg2.piv`, `bg2a.piv` | Ville (Highwood / Waterdeep) |
| `bg3.piv`, `bg4.piv` | Autres scènes intro |
| `bg5.piv`, `bg5a.piv` | Autres scènes intro |
| `bg7.piv`, `bg8.piv` | Arène / combat intro |
| `message.piv` | Écran message / score |

#### Tilemaps (`.stile`)

| Fichier | Rôle |
|---|---|
| `intro.stile` | Tilemap écran intro |
| `co.stile` | Tilemap overworld / crédits |

#### Police et musique

| Fichier | Rôle |
|---|---|
| `bold.f` | Police principale (UI) |
| `music.cmp` | Module musique principal |
| `vmusic.cmp` | Module musique victoire |

---

### C.2 Assets chargés par `mog` (overlay jeu, ~172 Ko)

#### Objets personnages (`.ob`)

| Fichier | Label `mog.asm` | Rôle |
|---|---|---|
| `kn1.ob` | `LAB_076F` | Chevalier bleu (joueur 1) |
| `kn2.ob` | `LAB_0770` | Chevalier rouge (joueur 2) |
| `kn3.ob` | `LAB_0771` | Chevalier vert (joueur 3) |
| `kn4.ob` | `LAB_0772` | Chevalier jaune (joueur 4) |
| `Kn5.ob` | `LAB_0773` | Chevalier noir (IA) |
| `He1.ob` | `LAB_0775` | PNJ soigneur type 1 |
| `He2.ob` | `LAB_0776` | PNJ soigneur type 2 |
| `He3.ob` | `LAB_0777` | PNJ soigneur type 3 |

#### Animations ennemis (`.cel`)

| Fichier | Label | Ennemi |
|---|---|---|
| `blo.cel` | `LAB_0774` | Impact / sang |
| `TroggAxe1.cel`, `TroggAxe2.cel` | `LAB_0778-9` | Trogg à la hache |
| `TroggSpear1.cel`, `TroggSpear2.cel` | `LAB_077A-B` | Trogg à la lance |
| `Ratmen1.cel`, `Ratmen2.cel` | `LAB_077C-D` | Hommes-rats |
| `Dragon1.cel`, `Dragon2.cel` | `LAB_0780-1` | Dragon (2 frames) |
| `NuDragon5.cel` | (dans code) | Dragon variante 5 (boss) |
| `Mudmen1.cel`, `Mudmen2.cel` | `LAB_0782-3` | Hommes de boue |
| `ki.cel` | `LAB_0784` | Inconnu (ennemi "ki") |
| `Balok1.cel`, `Balok2.cel`, `Balok3.cel` | `LAB_0785-7` | Balok (3 animations) |
| `Demon1.cel`..`Demon4.cel` | `LAB_07B0-3` | Démon (4 animations) |
| `Troll1.cel`, `Troll2.cel` | `LAB_07B4-5` | Troll |
| `Sel.cel` | `LAB_07AD` | Curseur sélection chevalier |
| `mys.cel` | `LAB_091D` | Math le Mystic (wizard) |
| `dice.cel` | `LAB_0F4F` | Dés (jeu de taverne) |
| `po.cel` | `LAB_0983` | Portail / objet ? |

#### Animations compressées (`.c`) et palettes (`.p`)

Ces fichiers semblent être des CEL courts (`.c`) et palettes séparées (`.p`) pour les créatures à palette dynamique.

| Fichier | Label | Rôle |
|---|---|---|
| `be1.c`, `be2.c` | `LAB_077E-F` | Ours — 2 frames |
| `wi1.c` | `LAB_0788` | Sorcière — CEL |
| `wi1.p`, `wi2.p` | `LAB_0789-A` | Sorcière — 2 palettes |
| `Hen1.c` | `LAB_0F51` | PNJ poule — CEL |
| `Hen1.p` | `LAB_0F50` | PNJ poule — palette |
| `mi.c` | `LAB_070E` | Intro Mindscape — CEL |

#### Fonds de scènes in-game (`.piv`)

| Fichier | Label | Scène |
|---|---|---|
| `bg2.piv` | `LAB_06F4` | Ville (marché / rencontre) |
| `bg8.piv` | (ligne 13169) | Arène de combat principale |
| `HighWood.piv` | `LAB_0715` | Ville Highwood |
| `WaterDeep.piv` | `LAB_0716` | Ville Waterdeep |
| `HEA.piv` | `LAB_091B` | Temple du soigneur |
| `MYS.piv` | `LAB_091C` | Tour de Math le Mystic |
| `tav.piv` | `LAB_0F4D` | Taverne (jeu de dés) |
| `dice.piv` | `LAB_0F4E` | Écran jeu de dés |
| `ch.piv` | `LAB_07AF` | Sélection champion |
| `message.piv` | `LAB_07AE` | Écran message |

#### Tuiles terrain overworld (`.t`)

4 biomes × 8 tuiles haute résolution + 4 biomes × 6 tuiles basse résolution.

| Groupe | Fichiers | Label base | Biome |
|---|---|---|---|
| Haute résolution | `FO1.t`..`FO8.t` | `LAB_078D` | Forêt |
| | `Sw1.t`..`Sw8.t` | `LAB_0795` | Marais |
| | `GL1.t`..`GL8.t` | `LAB_079D` | Clairière / prairie |
| | `Wa1.t`..`Wa8.t` | `LAB_07A5` | Eau |
| Basse résolution | `fol1.t`..`fol6.t` | `LAB_07C1` | Forêt |
| | `swl1.t`..`swl6.t` | `LAB_07C9` | Marais |
| | `gll1.t`..`gll6.t` | `LAB_07D1` | Clairière |
| | `wal1.t`..`wal6.t` | `LAB_07C5` | Eau |

#### Scripts d'animation overworld (`.a`) — section `SECSTRT_17`

Un fichier par type d'entité mobile sur la carte du monde. Contient vraisemblablement les séquences de déplacement, timings et frames.

| Fichier | Label | Entité |
|---|---|---|
| `kn.a` | `SECSTRT_17` | Chevalier (joueur) |
| `Be.a` | `LAB_0AB7` | Ours |
| `Ba.a` | `LAB_0AB8` | Balok |
| `Dr.a` | `LAB_0AB9` | Dragon |
| `To.a` | `LAB_0ABA` | Troll |
| `Tr.a` | `LAB_0ABB` | Trogg |
| `Wn.a` | `LAB_0ABC` | Inconnu (Wyvern ?) |
| `Gu.a` | `LAB_0ABD` | Gardien / Guardian |
| `Wz.a` | `LAB_0ABE` | Wizard (Math) |
| `Ra.a` | `LAB_0ABF` | Hommes-rats |
| `Mu.a` | `LAB_0AC0` | Hommes de boue |
| `Re.a` | `LAB_0AC1` | Inconnu |
| `He.a` | `LAB_0AC2` | Soigneur (PNJ) |

#### Données de collision et polices

| Fichier | Label | Rôle |
|---|---|---|
| `collide.hit` | `LAB_0A57` | Boîtes de collision combat |
| `bold.f` | `LAB_078B` | Police principale (partagée avec `program`) |
| `Small.font` | `LAB_078C` | Police secondaire (UI in-game) |

---

### C.3 Bilan — présence sur disque

Sur **~80 fichiers assets référencés** dans les binaires, seuls **~20** sont présents dans le répertoire courant (les assets du binaire `program`). Les assets requis par `mog` (enemies, terrains, scènes, scripts `.a`) correspondent au disque de jeu (disque 2+) absent de cette extraction.

| Statut | Exemples |
|---|---|
| ✅ Présent | `bg1a-8.piv`, `au1.cel`..`ov1.cel`, `music.cmp`, `kn1.ob`, `bold.f` |
| ❌ Absent | `kn2-5.ob`, tous les `.cel` ennemis, `*.t`, `*.a`, `*.c`, `*.p`, `collide.hit` |

---

*Fin du document — généré par analyse statique de [program.asm](program.asm) et [mog.asm](mog.asm).
Mise à jour recommandée après analyse approfondie du job manager (S_10 handlers 5–20) et du format exact de `kn1.ob`.*
