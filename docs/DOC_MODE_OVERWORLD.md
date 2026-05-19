# Documentation — Mode de jeu : Carte Overworld

> Document de conception destiné à guider l'implémentation de la phase
> « carte overworld » du portage C de Moonstone — A Hard Days Knight.
>
> Seul ce qui se passe sur la carte est décrit ici. Les phases de combat,
> les villes, les villages, l'inventaire, la tour du magicien, le cercle
> des dieux et Stonehenge sont hors périmètre de ce document.

---

## 1. Assets utilisés

### 1.1 Fond d'écran (PIV)

| Label       | Fichier      | Rôle                                              |
|-------------|--------------|---------------------------------------------------|
| `LAB_00CB`  | `dw1.PIV`    | **Fond principal de la carte overworld** (320×200, 5 plans, 32 couleurs). Copié dans les deux buffers de double-buffer au démarrage de chaque tour. |
| `LAB_00C7`  | —            | Pointeur vers le premier buffer de fond (double-buffer A). |
| `LAB_00C8`  | —            | Pointeur vers le second buffer de fond (double-buffer B). |

Les fichiers `.PIV` sont dans le format propriétaire Mindscape :
- `word[0]` = nombre de plans (4 ou 5)
- `long[1]` = taille compressée du body
- 32 mots de palette Amiga 12-bit (`$0RGB`)
- Body : stream **LZSS** (même algorithme que les sprites `.CEL`)

### 1.2 Fichiers CEL et OB — inventaire overworld

#### Table d'assets `LAB_0276` — chargée par la boucle de jeu overworld

La table `LAB_0276` (10 pointeurs longs) est peuplée en mémoire au
chargement. Le second chargement (contexte mog / overworld interactif,
`program.asm#L3544-L3620`) place les fichiers suivants :

| Slot (offset) | Fichier     | Frames | Rôle                                                       |
|---------------|-------------|--------|------------------------------------------------------------|
| `[0]`         | `dg1.cel`   | 55     | **Dragon volant sur la carte** — toutes les frames de vol du dragon overworld |
| `[4]`         | `li1.cel`   | 30     | Sprites icônes des lieux (villages, temples, châteaux)     |
| `[8]`         | `ha1.cel`   | 22     | Sprites de type Hawk / décoration de carte                 |
| `[12]`        | `co1.cel`   | 25     | Sprites icônes complémentaires (co-icônes, repères)        |
| `[16]`        | `da1.cel`   | 52     | Sprites de dégâts / décoration animée                      |
| `[20]`        | `ov1.cel`   | 4      | **Icônes de nœuds de la carte** (overworld node icons)     |

> **Note :** le fichier `ov1.cel` (label `LAB_016A`, préfixe ASCII `0x6F 0x76` = `'ov'`)
> ne contient que 4 frames ; il représente les icônes statiques des nœuds
> interactifs de la carte. `dg1.cel` (55 frames) fournit l'animation complète
> du dragon en vol.

#### Fichiers `.ob` — sprites des chevaliers (carte et combat)

| Fichier   | Label      | Joueur          | Rôle                                  |
|-----------|------------|-----------------|---------------------------------------|
| `kn1.ob`  | `LAB_076F` | SIR RICHARD (bleu)  | Sprite chevalier sur la carte     |
| `kn2.ob`  | `LAB_0770` | SIR GODBER (rouge)  | Sprite chevalier sur la carte     |
| `kn3.ob`  | `LAB_0771` | SIR JEFFREY (vert)  | Sprite chevalier sur la carte     |
| `kn4.ob`  | `LAB_0772` | SIR EDWARD (jaune)  | Sprite chevalier sur la carte     |
| `Kn5.ob`  | `LAB_0773` | Chevalier Noir IA   | Sprite chevalier ennemi sur carte |

Format `.ob` = identique à `.cel` : en-tête 10 octets + table frames +
corps LZSS (`LAB_049C`).

#### Fichiers CEL de combat du Dragon — chargés par `LAB_0121`

Ces fichiers sont chargés **uniquement** lors du déclenchement d'un combat
contre le dragon (appel `JSR LAB_0121`, `mog.asm#L3969`) :

| Fichier        | Label      | Frames | Rôle                                     |
|----------------|------------|--------|------------------------------------------|
| `Dragon1.cel`  | `LAB_0780` | —      | Sprites d'attaque / phase de combat dragon (animation 1) |
| `Dragon2.cel`  | `LAB_0781` | —      | Sprites d'attaque / phase de combat dragon (animation 2) |

Ces deux fichiers sont distincts de `dg1.cel` : ils ne servent que pour
la scène de combat, pas pour le vol du dragon sur la carte.

#### Scripts d'animation de la carte (bytecode)

| Label      | Nom inféré                  | Fichier CEL source | Usage overworld                                      |
|------------|-----------------------------|---------------------|------------------------------------------------------|
| `LAB_00E7` | `map_all_node_positions`    | ~145 entrées × 6 octets | Table complète des positions des nœuds/icônes |
| `LAB_00E8` | `anim_map_selected_node`    | `ov1.cel` frames 1..6   | Animation du nœud de destination sélectionné  |
| `LAB_00E9` | `anim_map_ui_frame`         | `ov1.cel` frames 7..18  | Cadre UI permanent de la carte, affiché en fond |
| `LAB_00EA` | `anim_map_idle_overlay`     | `ov1.cel` frames 0/1/2  | Overlay idle : icône du chevalier courant       |
| `LAB_00E6` | `anim_overworld_hud`        | 12 icônes + barres      | HUD overworld (barres HP/XP/reliques)           |
| `LAB_00D7` | `anim_map_knight_walk_r`    | `kn*.ob` sub_frames `0x10..0x20` | Animation de marche droite du chevalier |
| `LAB_00D8` | `anim_map_knight_walk_l`    | `kn*.ob`                | Animation de marche gauche sur la carte         |
| `LAB_00D9` | `anim_map_knight_walk_r2`   | `kn*.ob`                | Suite du parcours de chemin (segment suivant)   |
| `LAB_00DA` | `anim_map_knight_path_cont` | `kn*.ob`                | Continuation de déplacement sur un chemin       |

#### Format d'une entrée de `LAB_00E7` (6 octets)

```
byte 0 = 0x00        → index CEL (0 = premier slot : icône chevalier)
byte 1 = N           → sous-frame N (direction/type d'icône, 0..7)
byte 2 = X_signed    → colonne écran en pixels (signé)
byte 3 = flags       → drapeaux d'affichage
word 4 = Y_signed    → ligne écran en pixels (signée)
```

La table contient environ **36 nœuds distincts** (lieux) et ~70 chemins
bidirectionnels. Chaque chemin est dupliqué (A→B et B→A) pour dessiner le
chevalier quel que soit son sens de déplacement.

### 1.3 Animation table des nœuds

`LAB_003A` est une table de 10 pointeurs vers `LAB_00E7` :

```asm
LAB_003A:
    DC.L LAB_00E7   ; × 10
```

Cette table est jouée comme un script d'animation dans `scene_overworld()`
pour positionner les icônes de nœuds sur la carte.

### 1.4 Musique

| Fichier       | Format              | Rôle                        |
|---------------|---------------------|-----------------------------|
| `music.cmp`   | MOD 31 instruments compressé **RNC1** | Musique principale de la carte overworld |
| `vmusic.cmp`  | MOD 31 instruments compressé **RNC1** | Variante musicale (voix / FX) |

La décompression RNC1 est assurée par `LAB_0190`. Magic : `$524E4301`
(« RNC\x01 »). Le lecteur audio est de type SoundTracker / NoiseTracker
31 instruments.

### 1.5 Données de nœuds interactifs (mog.asm)

`LAB_069F` ([mog.asm#L1037](../amiga_asm/mog.asm)) : table de triplets
`(type:word, x:word, y:word)` terminée par `$FFFF`. Elle liste tous les
lieux interactifs de la carte avec leur type et leurs coordonnées overworld.

| Type (hex) | Lieu                          |
|------------|-------------------------------|
| `0x01`     | Position d'un chevalier joueur (duel PvP) |
| `0x02`     | Rencontre créature (monstre aléatoire) |
| `0x15`     | Château de Richard (faction 0) |
| `0x16`     | Château de Godber (faction 1)  |
| `0x17`     | Château de Jeffrey (faction 2) |
| `0x18`     | Château d'Edward (faction 3)   |
| `0x19`     | Ville avec armurier + arène + sorcier |
| `0x1a`     | Ville avec taverne + arène     |
| `0x1b`     | Sorcier Mythral               |
| `0x1c`     | **Antre du Dragon** (accès conditionnel) |
| `0x1e`     | Temple de guérison            |
| `0x21`     | Rencontre chevalier (variante) |

---

## 2. Boucle de rendu

### 2.1 Entrée de scène : `scene_overworld()` = `LAB_0037`

```text
scene_overworld() [program.asm#L648] :

1. Init pool d'entités :
   JSR LAB_01E4          → vider les 40 slots d'entités (40 × 42 octets)

2. Charger le fond :
   JSR LAB_0258          → appliquer palette noire (fondu)
   JSR LAB_0263          → copier le PIV dw1 dans les 2 buffers de double-buffer

3. Spawner les entités de carte :
   a. MOVEA.L #LAB_00E9,A0 ; JSR LAB_0015
      → spawner le cadre UI de la carte (18 sprites permanents)
   b. MOVEA.L #LAB_00EA,A0 ; JSR LAB_0015
      → spawner l'overlay idle (icône chevalier courant, boucle infinie)

4. Configurer l'animation de nœuds :
   MOVE.W #$0005, LAB_00EF    ; afficher 5 icônes chevaliers
   MOVE.W #$000f, LAB_00F0    ; offset du second groupe (track 15)
   MOVE.L #LAB_003A, LAB_0029+2   ; table = 10 × LAB_00E7 (positions nœuds)
   MOVE.W #$000a, LAB_0029        ; 10 frames d'animation à jouer

5. Boucle d'animation non-interactive (41 frames) :
   BSR LAB_001F          → jouer la boucle interne de sélection de frame
   BSR LAB_0038          → 40 frames additionnels (overworld_frame_loop)

6. Phase interactive — déplacement du joueur courant :
   → voir §3 Gestion des joueurs

7. Détection d'événement (collision sur un nœud) :
   LAB_006B parcourt LAB_069F → appelle LAB_0067 pour chaque nœud
   Si D5=2 (correspondance position) → dispatcher LAB_0E45
   → Déclenchement de l'événement selon le type de nœud
   → Retour via LAB_00B2 → SECSTRT_36 → reprise de la carte

8. Passer au joueur suivant → retour à l'étape 1
```

### 2.2 Boucle interne de frame : `overworld_frame_loop()` = `LAB_0038`

Cette sous-boucle joue exactement **40 frames** de mise à jour de la carte
avant de rendre la main au contrôle interactif du joueur. Elle appelle :
- `LAB_01E8` — mise à jour des scripts d'animation (tick de tous les sprites)
- `LAB_01EC` — rendu des sprites via le blitter
- `LAB_0262` — flip du double-buffer (affichage à l'écran)

### 2.3 Double-buffer

Le rendu utilise deux buffers bitmap identiques au format Amiga 5-plans :
- `LAB_00C7` = buffer A (affiché)
- `LAB_00C8` = buffer B (en cours de dessin)

À chaque VBL, les pointeurs sont échangés (`LAB_0416` = sync VBL). Le fond
PIV est copié dans les deux buffers au démarrage pour que le premier flip
n'affiche pas de frame vide.

### 2.4 Variables de timing

| Variable    | Rôle                                                     |
|-------------|----------------------------------------------------------|
| `LAB_00D0`  | Délai cible en ticks par frame (4 = rapide, 6 = normal, 8 = lent) |
| `LAB_0028`  | Compteur de frames dans la boucle courante               |
| `LAB_0029`  | Nombre max de frames de la boucle (longueur d'animation) |
| `LAB_0026+2`| Index de frame courant (itérateur sur la table d'animation) |
| `LAB_0123`  | Mode courant : 0 = overworld, 4 = combat actif           |

---

## 3. Boucle de gestion des joueurs

Le jeu supporte de **1 à 4 joueurs** humains (option « Players » du menu
principal, `LAB_06BC`). Chaque joueur contrôle un chevalier nommé :

| Label      | Nom          | Faction (`54(knight)`) | Couleur |
|------------|--------------|------------------------|---------|
| `LAB_0613` | SIR RICHARD  | 0                      | Bleu    |
| `LAB_0614` | SIR GODBER   | 1                      | Rouge   |
| `LAB_0615` | SIR JEFFREY  | 2                      | Vert    |
| `LAB_0616` | SIR EDWARD   | 3                      | Jaune   |
| `LAB_0617` | Chevalier Noir (IA ennemie) | 4/5         | —       |

Le tableau `LAB_05E4` contient 4 pointeurs longs → `LAB_0613..0616`.
Le chevalier courant est désigné par `LAB_068B`.

### 3.1 Sélection du joueur

Avant la partie, l'option **« Select Knight »** (`LAB_06BF`) permet à chaque
joueur de choisir son chevalier parmi les quatre disponibles.

Au début de chaque tour overworld, le jeu détermine quel joueur doit jouer
en consultant l'ordre de tour interne. Les joueurs jouent séquentiellement,
l'un après l'autre. Les chevaliers contrôlés par l'IA (` enemy_flag = 0xFF`)
sont traités dans le tour IA `LAB_0036` (`ai_turn_encounter()`).

**Spawn des entités joueurs** dans `scene_overworld()` :

```asm
LAB_0015:                          ; spawn joueur 1
    MOVEA.L #LAB_0276, A2          ; table d'assets (8 CEL de chevalier)
    MOVE.W  #$00a0, D0             ; X de base = 160
    MOVE.W  #$0000, D1             ; Y de base = 0
    MOVE.W  #$0064, D2             ; Y absolu = 100
    ADD.W   LAB_00EF, D2           ; + offset joueur courant
    MOVE.W  #$0001, D3             ; slot = 1
    JSR     LAB_01DA               ; créer l'entité dans le pool
```

`LAB_0016` fait de même pour le joueur 2 avec `LAB_00F0`.

### 3.2 Déplacement du joueur

#### Lecture du joystick (`LAB_0049`)

| Port         | Registre hardware  | Joueur |
|--------------|--------------------|--------|
| Joystick 0   | `JOY0DAT`, `CIAA_PRA` bit 6 | Joueur 1 |
| Joystick 1   | `JOY1DAT`, `CIAA_PRA` bit 7 | Joueur 2 |

Résultats stockés dans `LAB_00C0` (joueur 1) et `LAB_00C1` (joueur 2) :

| Bit | Direction / Action |
|-----|--------------------|
| 0   | Droite             |
| 1   | Gauche             |
| 2   | Haut               |
| 3   | Bas                |
| 4   | Feu / Action       |

> **Note multi-joueurs :** les joueurs 3 et 4 sont contrôlés au clavier
> (lecture via la table de scancodes `LAB_036C`, 128 octets, section S_17)
> ou par un partage de joystick selon la configuration.

#### Déplacement sur le graphe de nœuds

Le déplacement du chevalier se fait **nœud par nœud** sur un graphe de
chemins prédéfinis. L'algorithme est :

1. Le joueur déplace le joystick dans une direction.
2. Le moteur sélectionne le nœud voisin le plus proche dans cette direction
   en consultant `LAB_00E7` (table des ~145 entrées de 6 octets).
3. Une animation de déplacement est jouée :
   - Déplacement vers la droite → `LAB_00D7` (`anim_map_knight_walk_r`, CEL `$0C`)
   - Déplacement vers la gauche → `LAB_00D8` (`anim_map_knight_walk_l`)
   - Continuation de chemin → `LAB_00D9` / `LAB_00DA`
4. La position du chevalier est mise à jour :
   - `126(knight)` = X overworld (position courante)
   - `128(knight)` = Y overworld (position courante)
5. À chaque frame, `LAB_006B` balaie `LAB_069F` pour tester si le chevalier
   est arrivé sur un nœud interactif (`LAB_0067` retourne D5=2 si match).

#### Durée d'un déplacement

Chaque tour de jeu autorise **40 frames** de déplacement (`LAB_0038` =
`overworld_frame_loop`). Après ces 40 frames, si le joueur est sur un nœud,
l'événement est déclenché ; sinon, la carte repasse au joueur suivant.

### 3.3 Actions du joueur sur la carte

Quand un chevalier arrive sur un nœud interactif, le dispatcher `LAB_0E45`
est appelé avec le type du nœud :

```text
LAB_0E45 (dispatcher de nœuds) :
  type 0x01 / 0x21 → LAB_004F  : Rencontre avec un autre chevalier (duel PvP)
  type 0x02        → LAB_005B  : Rencontre créature (combat PvE aléatoire)
  autres types     → LAB_007B  : Switch étendu (châteaux, villes, dragon…)
```

Après le traitement de l'événement, le retour s'effectue toujours vers
`LAB_00B2` → `SECSTRT_36` (restauration du hardware) → reprise de la carte.

#### Résumé des actions déclenchables sur la carte

| Type nœud | Action                                           | Sortie de carte |
|-----------|--------------------------------------------------|-----------------|
| `0x01`/`0x21` | Duel PvP (combat immédiat)                  | Oui (combat)    |
| `0x02`    | Combat contre créature aléatoire                 | Oui (combat)    |
| `0x15..0x18` | Visite d'un château allié → skill +1          | Non (retour carte) |
| `0x19`    | Ville : arène / achat / sorcier / armurier       | Non (menu ville)|
| `0x1a`    | Ville : arène / achat / sorcier / taverne        | Non (menu ville)|
| `0x1b`    | Sorcier Mythral → échange moonstones / skill     | Non (menu)      |
| `0x1c`    | **Antre du Dragon** — lair accessible sous conditions (Moonstone + reliques) | Non (menu)      |
| `0x1e`    | Temple de guérison → restaure HP                 | Non (menu)      |

---

## 4. Événement du Dragon

Le dragon n'est **pas** un boss final accessible via un nœud de la carte.
C'est une entité autonome qui **vole en permanence sur la carte overworld**
pendant toute la partie. Si le dragon entre en collision avec l'un des
joueurs, un combat contre le dragon est immédiatement déclenché.
Un joueur peut de plus rediriger le dragon vers un autre joueur en utilisant
le **Parchemin du Wyrm** (`item_scroll_wyrm`).

### 4.1 Initialisation du dragon — `LAB_0DCB`

Le dragon est créé en mémoire après que la partie a progressé d'au moins
**2 rounds** (`LAB_06C0 >= 2`). La fonction `LAB_0DCB` :

```text
LAB_0DCB [mog.asm#L25049] :
  1. Vérifie LAB_06C0 >= 2 (seuil de ronde d'apparition)
  2. Vérifie 73(LAB_0617) >= 0 (dragon non mort)
  3. Appelle LAB_0305 (initialisation d'entité dragon)
  4. Installe LAB_0DCF comme handler de mouvement : LAB_08C7.move = LAB_0DCF
  5. Initialise la position de départ dans LAB_0671 (5 longs)
  6. Remplit la struct dragon (LAB_0617) :
       4(dragon)  = 0x000A  (X initial)
       6(dragon)  = 0x0000  (X velocity high byte)
       8(dragon)  = 0x0064  (Y initial = 100)
      10(dragon)  = 0x03    (state = 3 = vol)
      38(dragon)  = LAB_0671 (pointeur buffer de rendu)
      77(dragon)  = 0x28    (animation frame stride)
      12(dragon)  = 0x00    (frame counter = 0)
      46(dragon)  = LAB_08FC (table d'animation = 16 frames × 2 entrées)
  7. Lance le premier draw via LAB_0310 (initialise le sprite dragon)
  8. Initialise LAB_0DDC = 2 (vitesse X = +2 pixels/tick)
  9. LAB_0666 = 100 (countdown avant activation complète)
 10. LAB_0667 = 1 (flag dragon actif)
 11. Choisit aléatoirement un joueur cible parmi les joueurs actifs (non IA)
     → stocké dans 100(LAB_0617)
```

### 4.2 Vol du dragon sur la carte — `LAB_0DCF`

`LAB_0DCF` est le handler de mouvement appelé à chaque frame de la carte
(`JMP LAB_02BA` en fin de handler = retour au renderer). Il gère le vol
en deux phases :

#### Phase d'approche (LAB_0666 > 60 = décrémente de 100 à 61)

```text
LAB_0DCF → LAB_0DD0 :
  Décrémenter LAB_0666
  if (LAB_0666 <= 60) → basculer en phase de poursuite

  Pendant l'approche :
    X += LAB_0DDC (vitesse X courante)
    → appel LAB_02BA (rendu)
```

#### Phase de poursuite active (LAB_0666 <= 60)

```text
LAB_0DCF → LAB_0DD1 :
  X += LAB_0DDC                    ; déplacement horizontal
  D5 = LAB_0DDC+2                  ; composante verticale (signe = direction)
  A1 = 100(LAB_0617)               ; cible = joueur visé
  D0 = 128(A1) - 8(dragon)        ; écart Y entre dragon et joueur cible
  if D0 > 0 → dragon monte (Y += |D5|)
  if D0 < 0 → dragon descend (Y -= |D5|)
  if D0 == 0 → Y inchangé

  Rebonds sur les bords :
    if 4(dragon) > 0x015E (=350) → X = 0x0159, inverser LAB_0DDC (flipX)
    if 4(dragon) < 0xFFEC (=-20) → X = 0xFFF6, inverser LAB_0DDC (flipX)
    if 8(dragon) > 0x00C8 (=200) → Y = 0
    if 8(dragon) < 0          → Y = 200

  Animation cycling :
    12(dragon) = (12(dragon) + 1) & 0x0F   ; cycle 0..15
    LAB_061D = LAB_08FC[12(dragon)]        ; sélectionner la frame dg1.cel
```

#### Table d'animation du dragon `LAB_08FC`

```text
LAB_08FC: 16 paires d'entrées (32 pointeurs) = frames 0..15 × 2

LAB_08FD : $0022 FB 00 $FFEF FFFF  → frame dg1.cel #0x22 (34), Y-offset $FB
LAB_08FE : $0023 F8 00 $FFEF FFFF  → frame dg1.cel #0x23 (35)
LAB_08FF : $0024 F6 00 $FFEF FFFF  → frame dg1.cel #0x24 (36)
LAB_0900 : $0025 F7 00 $FFEF FFFF  → frame dg1.cel #0x25 (37)
LAB_0901 : $0026 FA 00 $FFEF FFFF  → frame dg1.cel #0x26 (38)
LAB_0902 : $0027 FA 00 $FFEF FFFF  → frame dg1.cel #0x27 (39)
LAB_0903 : $0028 FB 00 $FFEF FFFF  → frame dg1.cel #0x28 (40)
LAB_0904 : $0029 FC 00 $FFEF FFFF  → frame dg1.cel #0x29 (41)
(frames 8..15 répètent les mêmes 8 entrées → animation en boucle)
```

Le dragon utilise donc les **frames 34 à 41** du fichier `dg1.cel`
(sur 55 frames totales) pour son animation de vol sur la carte.

### 4.3 Détection de collision — `LAB_0DD8`

À chaque frame, `LAB_0DD8` teste si le dragon touche l'un des joueurs.
Il consulte la table `LAB_08FA` (octets de masque de collision) :

```text
LAB_0DD8 [mog.asm#L25163] :
  Initialise flag collision = 0
  if LAB_065E != 0 → RTS (combat en cours, skip)
  if LAB_065C != 0 → RTS (transition, skip)
  BSR LAB_0E20   ; obtenir l'index du joueur courant en D1
  A0 = LAB_08FA  ; table de masques (bytes par joueur)
  D7 = LAB_08FA[D1] ; masque pour ce joueur
  if D7 == 0 → pas de collision possible → RTS
  EXT.W D7
  LAB_0DDA++
  D6 = LAB_0DDA & D7
  if D6 == 0 → RTS (pas de collision ce tick)
  else → LAB_0DDA+2 = 1 (flag collision = VRAI)
```

Quand `LAB_0DDA+2 = 1` (collision confirmée) :
- La boucle principale (`LAB_0DB0`) déclenche le combat contre le dragon
- `LAB_0E27` : appelle `LAB_001C` (handler de mort du joueur / début combat)

### 4.4 Parchemin du Wyrm — redirection du dragon

L'item **« Parchemin du Wyrm »** (`item_scroll_wyrm`, offset 18 dans la
struct inventaire du chevalier) permet à un joueur de rediriger le dragon
vers un **autre** joueur de son choix.

#### Mécanisme

Depuis le menu d'inventaire de la carte, l'option
**« Cast scroll of the Wyrm »** (`LAB_0992`) est disponible si le joueur
possède au moins un parchemin. Quand il l'utilise :

1. Le joueur sélectionne une cible parmi les joueurs actifs sur la carte
   via un mini-menu de sélection.
2. La cible choisie est stockée dans `100(current_player)` (pointeur vers
   la struct du joueur cible).
3. À chaque frame, `LAB_0E26` surveille ce pointeur :

```text
LAB_0E26 [mog.asm#L25617] :
  A0 = LAB_0633 (joueur courant)
  if 100(A0) == 0 → RTS (pas de cible choisie)
  A1 = 96(A0) (struct inventaire du joueur)
  if 16(A1) == 0 → RTS (pas de scroll actif)
  A2 = LAB_0617 (struct dragon)
  100(A2) = 100(A0)        ; dragon.target = knight.chosen_target
  16(A1) -= 1              ; consommer le scroll
  100(A0) = 0              ; effacer la cible du joueur
  JSR LAB_05A1             ; effet sonore
  JSR LAB_0DCB             ; recalcul du dragon (reset timer, re-init)
```

La **cible du dragon** (`100(LAB_0617)`) est ainsi changée instantanément.
Le dragon change de trajectoire à la frame suivante.

#### Talisman du Wyrm

L'item **« Talisman du Wyrm »** (`item_talisman_wyrm`, offset 10 dans
l'inventaire, label `LAB_098E`) est une version plus puissante : il
redirige le dragon de façon permanente jusqu'à ce que la cible meure.
Sa gestion suit le même pipeline mais avec `14(A1)` au lieu de `16(A1)`.

### 4.5 Variables de contrôle du dragon

| Variable       | Rôle                                                          |
|----------------|---------------------------------------------------------------|
| `LAB_0617`     | Struct dragon (même format `KnightStruct` 132 octets)         |
| `LAB_08C7`     | Entité dragon dans le pool d'entités ; `.move = LAB_0DCF`     |
| `LAB_0666`     | Countdown dragon (100 → 0 = phase approche puis poursuite)    |
| `LAB_0667`     | Flag dragon actif (1 = actif, 0 = inactif)                    |
| `LAB_0DDC`     | Vitesse X du dragon (`+2` ou `-2`), inversée aux rebonds      |
| `LAB_0DDC+2`   | Vitesse Y courante (signée)                                   |
| `LAB_0DDA`     | Compteur de frames depuis dernière collision                  |
| `LAB_0DDA+2`   | Flag collision active (0 = non, 1 = oui)                      |
| `LAB_08FA`     | Table de masques de collision par joueur (80 octets)          |
| `LAB_08FC`     | Table d'animation dragon (16 entrées × 2 ptrs)                |
| `LAB_06C0`     | Compteur de rounds global (dragon apparaît quand >= 2)        |

### 4.6 Structure de la struct dragon `LAB_0617`

Le dragon utilise la même structure que les chevaliers (`KnightStruct`,
132 octets), avec les valeurs initiales suivantes :

| Champ (offset) | Valeur init | Description                                    |
|----------------|-------------|------------------------------------------------|
| `4(dragon)`    | `0x000A`    | Position X courante (pixels)                   |
| `6(dragon)`    | `0x0000`    | High-word X velocity                           |
| `8(dragon)`    | `0x0064`    | Position Y courante (pixels)                   |
| `10(dragon)`   | `0x03`      | State = 3 (vol actif)                          |
| `12(dragon)`   | `0x00`      | Frame counter animation (0..15, cycle)         |
| `38(dragon)`   | `LAB_0671`  | Pointeur buffer de rendu (5 longs)             |
| `46(dragon)`   | `LAB_08FC`  | Table d'animation (frames `dg1.cel` 34..41)    |
| `54(dragon)`   | `4`         | knight_id = 4 (dragon = IA faction 4)          |
| `73(dragon)`   | `≥ 0`       | Vie (< 0 = mort, dragon inactif)               |
| `77(dragon)`   | `0x28`      | Stride animation                               |
| `100(dragon)`  | ptr joueur  | **Joueur cible** (vers qui le dragon vole)     |



---

## 5. Déplacement des Chevaliers Noirs (IA ennemie)

### 5.1 Identité et initialisation

Les **Chevaliers Noirs** sont les adversaires IA qui patrouillent la carte
overworld. Ils correspondent aux chevaliers avec `enemy_flag = 0xFF` et
`knight_id = 4` ou `5` dans la structure `KnightStruct`.

Valeurs initiales d'un Chevalier Noir (`LAB_01C6`) :

| Champ          | Valeur | Description                            |
|----------------|--------|----------------------------------------|
| `Force`        | 1      | Une lune de Force                      |
| `Constitution` | 1      | Une lune de Constitution               |
| `Endurance`    | 1      | Une lune d'Endurance                   |
| `skill_level`  | 5      | Compétence maximale (adversaire dur)   |
| `hp_current`   | 20     | Points de vie faibles (mais skill = 5) |
| `sword_type`   | `0x16` | Épée Longue                            |
| `armor_type`   | `0x1B` | Armure Rembourrée                      |
| `gold`         | 10     | Or initial                             |
| `daggers`      | 10     | Dagues (10 projectiles)                |
| `enemy_flag`   | `0xFF` | Marqueur IA ennemie                    |
| `ai_data`      | `LAB_08C0` | Pointeur vers les données IA       |

### 5.2 Tour IA : `ai_turn_encounter()` = `LAB_0036`

Après que tous les joueurs humains ont joué leur tour, la boucle principale
appelle `LAB_0036` pour faire jouer chaque Chevalier Noir :

```text
LAB_0036 [program.asm] :
    LAB_011E = 2         → mode cinématique / transition
    Charger fond LAB_01D2
    Spawner LAB_00E6     → sprites HUD overworld + barres de score
    LAB_011E = 4         → mode overworld interactif
    Jouer l'animation du tour IA
    → Détecter collisions sur les nœuds (même pipeline que les joueurs)
```

### 5.3 Algorithme de déplacement IA

Le Chevalier Noir se déplace sur le graphe de nœuds de `LAB_069F` en
utilisant les données de l'IA stockées dans `LAB_08C0` :

1. **Sélection du prochain nœud cible** : l'IA consulte le graphe de nœuds
   et sélectionne un nœud adjacent selon une heuristique de poursuite
   (se rapproche du chevalier joueur le plus proche).
2. **Déplacement** : même animation de marche que les joueurs
   (`LAB_00D7` / `LAB_00D8` / `LAB_00D9` / `LAB_00DA`), mais déclenchée
   automatiquement sans input joystick.
3. **Détection de collision** : à chaque frame, `LAB_006B` + `LAB_0067`
   testent si le Chevalier Noir est sur un nœud. Si son nœud coïncide avec
   la position d'un joueur (type `0x01` ou `0x21`), un duel PvP est
   immédiatement déclenché.

### 5.4 Rencontre Chevalier Noir ↔ joueur

Lorsque le Chevalier Noir arrive sur le même nœud qu'un joueur :

```text
type nœud = 0x01 ou 0x21
→ LAB_004F (handler de rencontre chevalier)
   LAB_068B = chevalier joueur (défenseur)
   LAB_068D = Chevalier Noir (attaquant IA)
   → état 1 (duel PvP, fond LAB_0694)
```

Après le combat :
- **Victoire joueur** → `LAB_001C` : transfert d'items du Chevalier Noir au joueur
- **Défaite joueur** → `LAB_000E` : respawn du joueur (HP max restauré, skill -1)
  Le Chevalier Noir reprend sa patrouille.

### 5.5 Rencontres avec des créatures (type `0x02`)

En plus des Chevaliers Noirs, des créatures aléatoires sont disposées sur
la carte. Leur gestion est assurée par `LAB_005B` (handler type `0x02`) :

- La créature est sélectionnée depuis `LAB_08C6` (table de structures créatures).
- Compteur de rencontres : `20(creature)` est incrémenté à chaque rencontre.
- Quand `20(creature) == 3` : le cycle global `LAB_06C1` avance de 1 (mod 8),
  provoquant l'apparition de la créature suivante dans la table.

---

## 6. Ordre de jeu multi-joueurs (résumé)

```text
DÉBUT DU TOUR :
   ┌─ Joueur 1 (si humain) : déplacement + événement éventuel
   ├─ Joueur 2 (si humain) : déplacement + événement éventuel
   ├─ Joueur 3 (si humain) : déplacement + événement éventuel
   ├─ Joueur 4 (si humain) : déplacement + événement éventuel
   └─ IA (Chevaliers Noirs) : tour automatique via LAB_0036

FIN DU TOUR :
   → Tous les rounds joués OU un joueur obtient la Moonstone
   → game_over() = LAB_003B
```

En mode 1 joueur, les 3 autres chevaliers (SIR GODBER, SIR JEFFREY,
SIR EDWARD) sont contrôlés par l'IA et se comportent comme des Chevaliers
Noirs supplémentaires avec leurs statistiques initiales de joueur.

---

## 7. Références techniques

| Symbole / Label  | Fichier                | Rôle dans l'overworld                          |
|------------------|------------------------|------------------------------------------------|
| `LAB_0037`       | `program.asm#L648`     | Point d'entrée `scene_overworld()`             |
| `LAB_0038`       | `program.asm`          | Boucle de 40 frames overworld                  |
| `LAB_0036`       | `program.asm`          | Tour IA (`ai_turn_encounter`)                  |
| `LAB_006B`       | `mog.asm`              | Scanner de nœuds (collision détection)         |
| `LAB_0067`       | `mog.asm`              | Test de position sur un nœud (retourne D5=2)   |
| `LAB_0E45`       | `mog.asm`              | Dispatcher d'événements de nœud                |
| `LAB_069F`       | `mog.asm#L1037`        | Table des nœuds : `(type, x, y)` × N + `$FFFF`|
| `LAB_00E7`       | `program.asm#L2215`    | Positions d'affichage des ~145 nœuds           |
| `LAB_003A`       | `program.asm#L760`     | Table d'animation : 10 × `LAB_00E7`           |
| `LAB_0276`       | `program.asm`          | Table d'assets des chevaliers (8 CEL)          |
| `LAB_0613..0617` | `mog.asm`              | Structs des 5 chevaliers (4 joueurs + IA boss) |
| `LAB_05E4`       | `mog.asm`              | Tableau des 4 pointeurs chevaliers actifs      |
| `LAB_068B`       | `mog.asm`              | Pointeur vers le chevalier courant             |
| `LAB_068F`       | `mog.asm`              | Registre d'état de la machine à états          |
| `LAB_08C0`       | `mog.asm`              | Données comportement IA des Chevaliers Noirs   |
| `LAB_08C6`       | `mog.asm`              | Table de structs des créatures ennemies        |
| `LAB_01E4`       | `program.asm`          | Reset du pool d'entités (40 × 42 octets)       |
| `LAB_0049`       | `program.asm`          | Lecture joystick (JOY0DAT / JOY1DAT)           |
| `LAB_00C0/C1`    | `program.asm`          | Résultats joystick joueurs 1 et 2              |
| `LAB_00CB`       | `program.asm`          | Pointeur vers `dw1.PIV` (fond de la carte)     |
| `LAB_009D`       | `mog.asm`              | Gestionnaire nœud Dragon (type `0x1c`)         |
| `LAB_0DCA`       | `mog.asm`              | Séquence de fin de partie (victoire dragon)    |
