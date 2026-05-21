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

#### Format de la table `LAB_069F`

`LAB_069F` ([mog.asm#L12971](../amiga_asm/mog.asm)) est une table de
triplets `(type:word, x:word, y:word)` terminée par le mot `$FFFF`.
Elle liste les **9 emplacements statiques** de la carte, chacun avec
son type et ses coordonnées overworld (en pixels, carte 320×200).

Le mouvement sur la carte est **libre** (le joystick déplace le sprite
du chevalier en continu) ; il n'y a pas de graphe d'arêtes prédéfini.
La détection d'événement se fait par **proximité** : `LAB_006B` parcourt
`LAB_069F` à chaque frame et appelle `LAB_0067` pour chaque nœud.
Si la distance chevalier-nœud ≤ seuil, le type du nœud est dispatché
vers `LAB_0E45` qui appelle le gestionnaire approprié.

En plus des 9 nœuds statiques, deux catégories de nœuds **dynamiques**
sont ajoutées au runtime dans le buffer `SECSTRT_2` :

- **Chevaliers ennemis / alliés** : position de chaque autre chevalier
  actif (type `0x01` ou `0x21`).
- **Dragon** : si le dragon entre en collision, type implicite dragon.

#### Référence des types de nœuds

| Type   | Nom UI (LAB_08F4)          | Gestionnaire | Description courte               |
|--------|----------------------------|--------------|-----------------------------------|
| `0x01` | —                          | `LAB_004F`   | Duel PvP (chevalier actif)        |
| `0x02` | —                          | `LAB_005B`   | Combat contre créature à **position fixe** (peut porter une Clef de la Vallée) |
| `0x15` | "Enter Village"            | `LAB_00B0`   | Village de Richard (faction 0)    |
| `0x16` | "Enter Village"            | `LAB_00B0`   | Village de Godber (faction 1)     |
| `0x17` | "Enter Village"            | `LAB_00B0`   | Village de Jeffrey (faction 2)    |
| `0x18` | "Enter Village"            | `LAB_00B0`   | Village d'Edward (faction 3)      |
| `0x19` | "Enter the city of Highwood" | `LAB_0093` | Cité de Highwood                  |
| `0x1a` | "Enter the city of Waterdeep" | `LAB_008A` | Cité de Waterdeep                 |
| `0x1b` | "Enter Stonehenge"         | `LAB_00A1`   | Stonehenge — Mythral le Mystique  |
| `0x1c` | "Enter Valley of the Gods" | `LAB_009D`   | Vallée des Dieux                  |
| `0x1e` | "Visit Math the Wizard"    | `LAB_007C`   | Math le Sorcier (guérison + skill)|
| `0x21` | "Pillage knight's grave"   | `LAB_004F`   | Tombe d'un chevalier mort (pillage) |

---

### 1.6 Description complète des nœuds de la carte

La carte overworld fait **320×200 pixels**. Les neuf nœuds statiques sont
répartis comme suit (X=0 = gauche, Y=0 = haut) :

```
  x=  0                  x=160                 x=320
y=  0  [0x15 x=18]  [0x1e x=217]  [0x16 x=286]
       Richard         Math          Godber

y= 28  [0x19 x=82]
       Highwood

y= 97                [0x1c x=152]
                     Vallée des Dieux

y=143                               [0x1a x=277]
                                    Waterdeep

y=155  [0x1b x=88]
       Stonehenge

y=187  [0x17 x=0]
       Jeffrey

y=192                               [0x18 x=303]
                                    Edward
```

---

#### Nœud 1 — Village de Richard (type `0x15`)

| Attribut     | Valeur                        |
|--------------|-------------------------------|
| Type         | `0x15`                        |
| Coordonnées  | X=18, Y=11 (coin NW)          |
| Gestionnaire | `LAB_00B0` [mog.asm#L1653]    |
| Nom UI       | "Enter Village"               |
| Faction      | 0 (SIR RICHARD — bleu)        |

**Comportement** : Le chevalier de la faction 0 peut entrer dans son village
d'origine pour restaurer ses points de vie et augmenter son niveau de
compétence (`skill_level`, offset 73). Les chevaliers d'autres factions ne
reçoivent aucun bonus (`LAB_00B0` vérifie `54(A0) == faction_id`).

```text
LAB_00B0 :
  if skill_level < 3 → skill_level += 1
  → état 9 (retour carte)
```

---

#### Nœud 2 — Village de Godber (type `0x16`)

| Attribut     | Valeur                        |
|--------------|-------------------------------|
| Type         | `0x16`                        |
| Coordonnées  | X=286, Y=11 (coin NE)         |
| Gestionnaire | `LAB_00B0` [mog.asm#L1653]    |
| Nom UI       | "Enter Village"               |
| Faction      | 1 (SIR GODBER — rouge)        |

**Comportement** : identique au Village de Richard, mais réservé à la
faction 1. Un chevalier rouge visite son château d'origine : `skill += 1`
(max 3 via ce chemin ; le maximum absolu de 5 est atteint via Math).

---

#### Nœud 3 — Village de Jeffrey (type `0x17`)

| Attribut     | Valeur                        |
|--------------|-------------------------------|
| Type         | `0x17`                        |
| Coordonnées  | X=0, Y=187 (bord W)           |
| Gestionnaire | `LAB_00B0` [mog.asm#L1653]    |
| Nom UI       | "Enter Village"               |
| Faction      | 2 (SIR JEFFREY — vert)        |

**Comportement** : identique aux autres villages, faction 2.

---

#### Nœud 4 — Village d'Edward (type `0x18`)

| Attribut     | Valeur                        |
|--------------|-------------------------------|
| Type         | `0x18`                        |
| Coordonnées  | X=303, Y=192 (coin SE)        |
| Gestionnaire | `LAB_00B0` [mog.asm#L1653]    |
| Nom UI       | "Enter Village"               |
| Faction      | 3 (SIR EDWARD — jaune)        |

**Comportement** : identique aux autres villages, faction 3. Les quatre
villages occupent les quatre coins/bords de la carte.

---

#### Nœud 5 — Cité de Highwood (type `0x19`)

| Attribut     | Valeur                         |
|--------------|--------------------------------|
| Type         | `0x19`                         |
| Coordonnées  | X=82, Y=28 (quart NW)          |
| Gestionnaire | `LAB_0093` [mog.asm#L1391]     |
| Nom UI       | "Enter the city of Highwood"   |
| Fond PIV     | `bg2.piv` (arrière-plan ville) |

**Comportement** : menu principal `LAB_009B` avec 5 options
(Y=0x1e, 0x42, 0x6a, 0x8c, 0xb7 en pixels verticaux) :

| Option (`16(item)`) | Action                              |
|---------------------|-------------------------------------|
| 1                   | Arène (combat PvE, gain d'or)        |
| 2                   | Acheter/vendre équipement            |
| 3                   | Augmenter le skill chez le sorcier   |
| 4                   | Forge / armurier                     |
| 5                   | Sortir (`LAB_0092`)                  |

---

#### Nœud 6 — Cité de Waterdeep (type `0x1a`)

| Attribut     | Valeur                          |
|--------------|---------------------------------|
| Type         | `0x1a`                          |
| Coordonnées  | X=277, Y=143 (quart E)          |
| Gestionnaire | `LAB_008A` [mog.asm#L1320]      |
| Nom UI       | "Enter the city of Waterdeep"   |
| Fond PIV     | `bg2.piv` (arrière-plan ville)  |

**Comportement** : menu `LAB_009C` avec 5 options similaires à Highwood
mais sans forge ; à la place, une taverne (option 4 = `LAB_047C` = achat
de potions / repos). Les deux cités sont les seuls endroits où acheter
des équipements et des parchemins.

| Option (`16(item)`) | Action                              |
|---------------------|-------------------------------------|
| 1                   | Arène (combat PvE)                   |
| 2                   | Acheter/vendre équipement            |
| 3                   | Sorcier — augmenter skill             |
| 4                   | Taverne (potions, repos)             |
| 5                   | Sortir                               |

---

#### Nœud 7 — Stonehenge — Mythral le Mystique (type `0x1b`)

| Attribut     | Valeur                          |
|--------------|---------------------------------|
| Type         | `0x1b`                          |
| Coordonnées  | X=88, Y=155 (SW)                |
| Gestionnaire | `LAB_00A1` [mog.asm#L1561]      |
| Nom UI       | "Enter Stonehenge"              |
| Fond PIV     | `bg8.piv` (arrière-plan)        |

**Comportement** : Mythral le Mystique (`LAB_0753`) propose d'offrir un
objet magique à la déesse Danu en échange d'une vie supplémentaire (skill
+1). Le joueur choisit un objet de son inventaire parmi ceux listés par
le menu `LAB_06A5..LAB_00A5`. Les objets offerts sont : Parchemin de
Rapidité, Parchemin d'Acquisition, Anneau de Protection ou Talisman du
Wyrm (selon `22(A2)` bits 0-3).

```text
LAB_00A1 :
  if inventaire a un item éligible (bits 0-3 de 22(A2)) :
    → afficher menu de sélection d'objet
    → consommer l'objet choisi
    → skill_level += 1 (cap 5)
  else :
    → message "Offer a magic item within Stonehenge"
    → retour carte
```

Le message de Mythral au premier contact (LAB_0751-LAB_075E) :
> *"Seek the knowledge … Mythral the Mystic … Offer a magic item within
> Stonehenge and Danu will grant you a longer life … Seek the wisdom of
> Math the wizard to aid you in your quest … Visit your home village to
> restore lost lives."*

---

#### Nœud 8 — Vallée des Dieux (type `0x1c`)

| Attribut     | Valeur                          |
|--------------|---------------------------------|
| Type         | `0x1c`                          |
| Coordonnées  | X=152, Y=97 (centre de la carte)|
| Gestionnaire | `LAB_009D` [mog.asm#L1525]      |
| Nom UI       | "Enter Valley of the Gods"      |
| Fond PIV     | chargé par `LAB_0DBD`           |

**Comportement** : c'est l'objectif final du jeu. La condition d'entrée
est que le joueur possède les **4 Clefs de la Vallée** (bitmask
`inventaire[20] == 0x0f` : bits 0-3 tous à 1, une clef par faction de
Chevalier Noir).

```text
LAB_009D :
  A0 = 96(knight)             ; inventaire du chevalier
  if inventaire[20] != 0x0f  ; pas toutes les 4 clefs ?
    → message "You must have all four keys to enter the Valley of the Gods"
    → retour carte (LAB_00B3)
  else                        ; accès accordé
    → LAB_0DBD                ; charger le fond de la Vallée
    → LAB_01A0                ; séquence de combat final (Gardien)
    → JSR LAB_0036 (tour IA)
    → si victoire (LAB_05DC bit 0) :
        battles_fought += 3
        inventaire[20] = 0    ; effacer les 4 clefs
        → LAB_0DCA            ; séquence de fin de partie
    → si défaite :
        skill_level -= 2
        → état 9 (respawn)
```

**Obtenir les Clefs** : chaque Chevalier Noir (IA) garde une clef. Après
avoir battu un Chevalier Noir en duel PvE (`LAB_0083`), le chevalier
gagnant reçoit un bit dans `inventaire[20]`. Il faut en accumuler les 4.

---

#### Nœud 9 — Math le Sorcier / Temple (type `0x1e`)

| Attribut     | Valeur                          |
|--------------|---------------------------------|
| Type         | `0x1e`                          |
| Coordonnées  | X=217, Y=11 (bord N)            |
| Gestionnaire | `LAB_007C` [mog.asm#L1224]      |
| Nom UI       | "Visit Math the Wizard"         |

**Comportement** : Math the Wizard (`LAB_075D`) restaure les HP et
permet d'augmenter le skill contre paiement d'or. C'est le seul endroit
où le skill peut dépasser 3 pour atteindre 5.

```text
LAB_007C :
  → LAB_0456 (restauration HP complète)
  → état 9 (retour carte)
```

Le message de Math (LAB_075C-LAB_075E) :
> *"Seek the wisdom of Math the wizard to aid you in your quest."*

---

#### Nœuds dynamiques (runtime — buffer `SECSTRT_2`)

Ces nœuds ne sont **pas** dans `LAB_069F`. Ils sont construits à chaque
frame par `LAB_006B` / `LAB_0069` à partir de l'état courant des entités.

| Type   | Source                                           | Gestionnaire   | Effet                               |
|--------|--------------------------------------------------|----------------|-------------------------------------|
| `0x01` | Autre chevalier actif (`73(A0) > 0`)             | `LAB_004F`     | Duel PvP immédiat                   |
| `0x21` | Chevalier mort (`73(A0) <= 0`) → tombe pillable  | `LAB_004F`     | Pillage : récupérer l'équipement du défunt |
| `0x02` | Table de créatures **prédéfinie** à positions fixes (`LAB_05C6`) | `LAB_005B` | Combat PvE contre la créature   |
| dragon | Collision avec le dragon volant                  | via `LAB_0E27` | Combat contre le dragon             |

---

### 1.7 Nœuds de créatures (type `0x02`) — structure et mécanique complète

Contrairement aux chevaliers (position variable selon l'IA), les **créatures
sont placées à des positions fixes** dans une table initialisée au démarrage.
Leur position ne change jamais en cours de partie sauf lorsqu'elles sont
vaincues (marquées inactives). Visiter et vaincre ces créatures est l'**objectif
principal** du jeu : certaines d'entre elles portent la **Clef de la Vallée**,
indispensable pour entrer dans la Vallée des Dieux.

#### Structure de la table de créatures

```
LAB_05C6 = pointeur vers le bloc de données créatures (chip RAM)
           initialisé à la chaîne de démarrage (mog.asm L281-282)
Taille    : 24 entrées × 20 octets = 480 octets ($1E0)
```

Chaque entrée de créature (stride 20 octets, `ADDA.L #$00000014,A0`) :

| Offset | Taille | Description                                                            |
|--------|--------|------------------------------------------------------------------------|
| 0      | long   | Pointeur vers le bloc de butin : 24 octets (1 par type d'item)        |
| 4      | word   | Index du type de combat (`4(A0)`, utilisé par `LAB_01A3` pour lancer la bonne séquence d'animation) |
| 8      | word   | **Clef présente** : 0 = pas de clef, ≠0 = la créature porte une clef  |
| 10     | word   | **X overworld** (position fixe ; $FFFF = créature vaincue/inactive)   |
| 12     | word   | **Y overworld** (position fixe)                                        |
| 14     | word   | Puissance de combat / points de vie de la créature                     |
| 16     | long   | Pointeur vers les données d'animation sprite de la créature            |

Le champ aux offsets 10–13 est traité comme un **long** lors de la mise à mort :
```asm
MOVE.L #$ffffffff,10(A0)   ; X=$FFFF, Y=$FFFF → créature inactive
```
Quand `TST.L 10(A0)` est négatif (bit de signe du long = 1), la créature
est ignorée dans la boucle de détection.

#### Construction des nœuds 0x02 dans SECSTRT_2

`LAB_0077` ([mog.asm#L1147](../amiga_asm/mog.asm)) :

```asm
LAB_0077:
  A0 ← MOVEA.L 68(LAB_05B9), A0   ; = LAB_05C6 : début de la table créatures
  D7 ← #$0017                       ; compteur 0..23 → 24 créatures

LAB_0077_loop:
  TST.L 10(A0)          ; X overworld (long)
  BMI  → skip           ; < 0 → créature vaincue : ignorer

  D0 ← #31              ; sprite-index 31 (icône générique de créature)
  D1 ← 10(A0)           ; X créature
  D2 ← 12(A0)           ; Y créature
  D3,D4 ← pos chevalier ; 126(knight), 128(knight)

  BSR LAB_0067          ; test de proximité : D5=2 si dans le rayon de détection
  BNE → skip            ; trop loin : pas de nœud

  ; Afficher l'icône créature à l'écran (sprite 31 aux coords D1,D2)
  JSR LAB_0CDA

  ; Enregistrer dans le buffer SECSTRT_2 :
  MOVE.L A0, (A2)+      ; ptr vers l'entrée créature
  MOVE.L #$00000002, (A2)+  ; type = 0x02

  ADDA.L #$00000014, A0 ; entrée suivante (stride 20)
  DBF D7, LAB_0077_loop
```

Résultat : chaque créature **vivante** dans le rayon de détection du chevalier
est ajoutée comme nœud de type `0x02` dans `SECSTRT_2`.

#### Condition de victoire sur une créature

`LAB_005F` ([mog.asm#L948](../amiga_asm/mog.asm)) — appelé après le combat :

```asm
LAB_005F:
  if multiplayer (LAB_065E ≠ 0) → RTS (pas de kill en multi)

  D0 ← 0               ; flag "créature encore active"

  if 8(A0) ≠ 0          ; créature porte encore une clef ?
    D0 ← 1              ; oui → reste active

  A1 ← 0(A0)            ; ptr vers le bloc de butin 24 octets
  D7 ← 23
  loop: if (A1)+ ≠ 0    ; au moins un item dans le butin ?
    D0 ← 1              ; oui → reste active
  DBF D7

  if D0 = 0             ; clef=0 ET butin vide ?
    MOVE.L #$ffffffff, 10(A0)   ; marquer la créature comme vaincue
```

La créature n'est **définitivement éliminée** de la carte (et son nœud ne
réapparaît plus) que lorsque le joueur a ramassé **tous ses objets ET sa
clef**. Tant qu'un item ou la clef reste dans son butin, la créature
réapparaît sur la carte à sa position fixe.

#### Collecte de la Clef de la Vallée

Quand le joueur entre en collision avec un nœud `0x02`, `LAB_005B` est
appelé avec `A1` = pointeur vers l'entrée créature. Après combat :

1. Si le joueur gagne (`LAB_05DC bit 0 = 0`) :
   - `battles_fought += 1` via `78(knight) + 1`
   - Le menu de pillage est affiché (état 2) listant les items récupérables
   - Le joueur voit `"Take Key to the Valley"` si `8(créature) ≠ 0`
   - Sélectionner la clef : `8(créature) ← 0`, bit correspondant de
     `inventaire[20]` mis à 1 (un bit par faction/creature-group)
2. `LAB_005F` vérifie si la créature doit être marquée comme vaincue.

Pour accéder à la **Vallée des Dieux** (`0x1c`), le joueur doit avoir
`inventaire[20] == 0x0f` : les 4 bits (un par Chevalier Noir / groupe de
créatures) tous à 1, ce qui signifie qu'il a collecté les 4 clefs.

#### Tri des créatures pour l'IA ennemie

`LAB_0DE0` ([mog.asm#L25193](../amiga_asm/mog.asm)) trie en temps réel les
24 créatures par distance Manhattan au chevalier IA, pour que l'ennemi
se dirige vers la créature la plus proche :

```asm
LAB_0DE0:
  A0 ← LAB_05C6         ; table créatures
  D7 ← 23               ; 24 entrées
  pour chaque créature :
    if 10(A0) < 0 : distance ← $FFFF  ; créature morte → loin
    sinon : dist ← |X_creat-X_knight| + |Y_creat-Y_knight|
  → remplir LAB_0672 (table de 24 × 6 octets : dist:word, ptr:long)
  → tri-bulles sur la distance (23 passes)
```

L'IA du Chevalier Noir utilise ensuite cette liste triée (`LAB_0DEB`) pour
choisir sa prochaine cible de déplacement.

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

#### Implémentation C (`game_run_overworld`)

Dans le portage C, les chevaliers (humains **et** chevaliers noirs) sont
tous stockés dans `ctx->knights[0..3]`. L'initialisation s'effectue une
seule fois (`overworld_initialised`) au premier appel :

```c
/* Positions et stats de départ pour les chevaliers humains */
k->map_x = s_start_x[i];  k->map_y = s_start_y[i];
k->endurance = DEFAULT_ENDURANCE;  /* 2 */

/* Chevaliers noirs : slots knights[] non pris par un humain */
black_knight_init(ctx);   /* active=1, human=0, is_black_knight=1 */
```

### 3.2 Déplacement du joueur

#### Lecture du joystick (`LAB_0049`) — ASM

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

#### Déplacement libre sur la carte — ASM

Le mouvement sur la carte overworld est **continu et libre** (pas de
graphe d'arêtes). L'algorithme de déplacement (`LAB_0E0C`) est :

1. Le joueur oriente le joystick dans une direction.
2. Le moteur calcule un vecteur vers la destination courante (autre joueur,
   nœud retenu ou position libre), à partir du Bresenham simplifié de
   `LAB_0E0C`.
3. Une animation de déplacement est jouée :
   - Déplacement vers la droite → `LAB_00D7` (`anim_map_knight_walk_r`, `kn*.ob`)
   - Déplacement vers la gauche → `LAB_00D8` (`anim_map_knight_walk_l`)
   - Continuation → `LAB_00D9` / `LAB_00DA`
4. La position du chevalier est mise à jour :
   - `126(knight)` = X overworld (position courante)
   - `128(knight)` = Y overworld (position courante)
5. À chaque frame, `LAB_006B` balaie `LAB_069F` pour tester si le chevalier
   est proche d'un nœud interactif (`LAB_0067` retourne D5=2 si match).

`LAB_00E7` (table de ~145 entrées en `program.asm`) contient les
**scripts de rendu du fond de carte** (positions d'icônes), non les
chemins navigables. Il n'existe pas de liste d'adjacence.

#### Déplacement — implémentation C

Le portage C conserve le mouvement libre pixel par pixel, mais abandonne le
système de scripts d'animation de l'Amiga :

- **Input** : lu via SDL2 (`hal_poll`) → `ctx->input.joy[0]` + scancodes clavier.
- **Vitesse** : 2 pixels/tick (variable `spd`).
- **Budget de déplacement** : `steps_remaining = endurance × 20` (défaut : 40 pixels/tour).
  Chaque tick de mouvement consomme `spd` steps. Quand le budget est épuisé,
  le joueur ne peut plus se déplacer jusqu'à la fin de son tour.
- **Animations** : non implémentées — le chevalier est représenté par le
  sprite de son fichier `.ob` sans interpolation de frame de marche (un
  cycle de 8 frames commun à tous les chevaliers via `s_kn_frame`).
- **Interaction** : touche FIRE / Entrée → `check_static_node` + `check_pve_node`
  (test de proximité dans un rayon `NODE_PROXIMITY = 16 px`).
- **Fin de tour** : SPACE → `k->turn_done = 1` ; ou automatique quand
  `steps_remaining <= 0`.

#### Durée d'un déplacement

**ASM** : Chaque tour de jeu autorise **40 frames** de déplacement (`LAB_0038` =
`overworld_frame_loop`). Après ces 40 frames, si le joueur est sur un nœud,
l'événement est déclenché ; sinon, la carte repasse au joueur suivant.

**C** : Le budget est exprimé en **pixels** (`endurance × 20`, soit 40 px
par défaut), pas en frames. Il n'y a pas de limite de frames par tour ;
le joueur joue jusqu'à épuisement du budget ou action volontaire (SPACE).

### 3.3 Actions du joueur sur la carte

Quand un chevalier arrive sur un nœud interactif, le dispatcher `LAB_0E45`
est appelé avec le type du nœud :

```text
LAB_0E45 (dispatcher de nœuds — ASM) :
  type 0x01 / 0x21 → LAB_004F  : Rencontre avec un autre chevalier (duel PvP)
  type 0x02        → LAB_005B  : Rencontre créature (combat PvE aléatoire)
  autres types     → LAB_007B  : Switch étendu (châteaux, villes, dragon…)
```

Après le traitement de l'événement, le retour s'effectue toujours vers
`LAB_00B2` → `SECSTRT_36` (restauration du hardware) → reprise de la carte.

**C** : le dispatch est inline dans `game_run_overworld` via
`handle_static_node(ctx, node_idx)` → bascule `ctx->state` vers le state
approprié, puis `return` pour sortir de la boucle. Le retour sur la carte
se fait par le re-entry dans `game_run_overworld` au tour suivant.

#### Résumé des actions déclenchables sur la carte

| Type nœud    | Nom UI                          | Action                                                      | ASM handler  | C (état cible)        |
|--------------|---------------------------------|-------------------------------------------------------------|--------------|-----------------------|
| `0x01`       | —                               | Duel PvP (combat immédiat)                                  | `LAB_004F`   | `STATE_COMBAT`        |
| `0x02`       | —                               | Combat créature prédéfinie (peut porter une Clef de la Vallée) | `LAB_005B` | `STATE_COMBAT`       |
| `0x15..0x18` | "Enter Village"                 | Village natal → `STATE_VILLAGE`                             | `LAB_00B0`   | `STATE_VILLAGE`       |
| `0x19`       | "Enter the city of Highwood"    | Cité de Highwood                                            | `LAB_0093`   | `STATE_TOWN`          |
| `0x1a`       | "Enter the city of Waterdeep"   | Cité de Waterdeep                                           | `LAB_008A`   | `STATE_TOWN`          |
| `0x1b`       | "Enter Stonehenge"              | Stonehenge                                                  | `LAB_00A1`   | `STATE_STONEHENGE`    |
| `0x1c`       | "Enter Valley of the Gods"      | **Vallée des Dieux** : requiert 4 clefs                     | `LAB_009D`   | `STATE_STONEHENGE`    |
| `0x1e`       | "Visit Math the Wizard"         | Math le Sorcier                                             | `LAB_007C`   | `STATE_WIZARD`        |
| `0x21`       | "Pillage knight's grave"        | Pillage d'une tombe *(non implémenté en C)*                 | `LAB_004F`   | *(absent)*            |

---

## 4. Événement du Dragon

Le dragon n'est **pas** un boss final accessible via un nœud de la carte.
C'est une entité autonome qui **vole en permanence sur la carte overworld**
pendant toute la partie. Si le dragon entre en collision avec l'un des
joueurs, un combat contre le dragon est immédiatement déclenché.
Un joueur peut de plus rediriger le dragon vers un autre joueur en utilisant
le **Parchemin du Wyrm** (`item_scroll_wyrm`).

### 4.1 Initialisation du dragon — `LAB_0DCB` (ASM) / `dragon_init` (C)

**ASM** — Le dragon est créé en mémoire après que la partie a progressé d'au moins
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

**C** (`dragon_init`) — Initialise les champs de `GameCtx` :

```c
ctx->dragon_active    = 1;
ctx->dragon_x         = 10;   ctx->dragon_y         = 100;
ctx->dragon_vx        = DG_SPEED_X;   /* +2 */
ctx->dragon_countdown = DG_COUNTDOWN_INIT;  /* 100 */
ctx->dragon_frame     = 0;    ctx->dragon_tick      = 0;
/* Choisit le premier joueur actif comme cible initiale */
```

Différences : pas de pool d'entités ni de handler de mouvement Amiga ;
le dragon est géré par `dragon_update()` appelé à chaque tick de la boucle
principale, hors tour de joueur.

### 4.2 Vol du dragon sur la carte — `LAB_0DCF` (ASM) / `dragon_update` (C)

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

**C** (`dragon_update`) — Logique équivalente :

- `dragon_countdown` décrément de 100 à 0 (même seuil 60 pour la poursuite).
- `dragon_x += dragon_vx` (+2 ou −2) ; rebond aux bords (±350/−20, wrap Y).
- Poursuite verticale (phase 2) : `dragon_y` ± `DG_SPEED_Y` (1 px/tick).
- Animation : cycle `dragon_frame` 0..7 toutes les `DG_ANIM_SPEED` (4) ticks
  → frame `dg1.cel` = `DG_FRAME_BASE + dragon_frame` = 34..41.
- Collision : cercle de rayon `DG_PROXIMITY = 18 px` autour de chaque chevalier.
- Le dragon est mis à jour **chaque tick** de la boucle principale,
  indépendamment du tour en cours (humain ou chevalier noir).

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

### 4.3 Détection de collision — `LAB_0DD8` (ASM) / `dragon_update` (C)

**ASM** :

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

**C** : la détection est un simple test de distance euclidienne sur tous
les chevaliers actifs à chaque tick. Pas de masques de collision ni de
table par joueur :

```c
for (int i = 0; i < MAX_PLAYERS; i++) {
    int dx = dragon_x - k->map_x, dy = dragon_y - k->map_y;
    if (dx*dx + dy*dy <= DG_PROXIMITY * DG_PROXIMITY) return i;
}
```

Quand `dragon_update` retourne `hit >= 0` :
- `node_type = 0x02` (PVE), `current_knight = hit`, `state = STATE_COMBAT`.
- Le dragon est réinitialisé (`dragon_init`) après le combat.

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

> **Non implémenté en C :** le Parchemin du Wyrm et le Talisman du Wyrm ne
> sont pas encore disponibles dans le portage C (pas d'écran d'inventaire).

#### Talisman du Wyrm

L'item **« Talisman du Wyrm »** (`item_talisman_wyrm`, offset 10 dans
l'inventaire, label `LAB_098E`) est une version plus puissante : il
redirige le dragon de façon permanente jusqu'à ce que la cible meure.
Sa gestion suit le même pipeline mais avec `14(A1)` au lieu de `16(A1)`.

### 4.5 Variables de contrôle du dragon

| Variable       | ASM                                                           | C (`GameCtx`)                    |
|----------------|---------------------------------------------------------------|----------------------------------|
| Position X     | `4(LAB_0617)`                                                 | `dragon_x`                       |
| Position Y     | `8(LAB_0617)`                                                 | `dragon_y`                       |
| Vitesse X      | `LAB_0DDC` (+2 ou −2)                                         | `dragon_vx`                      |
| Countdown      | `LAB_0666` (100 → 0)                                          | `dragon_countdown`               |
| Flag actif     | `LAB_0667`                                                    | `dragon_active`                  |
| Frame anim     | `12(LAB_0617)` cycle 0..15                                    | `dragon_frame` cycle 0..7        |
| Cible          | `100(LAB_0617)` (ptr knight)                                  | `dragon_target` (index knights[]) |
| Seuil collision| Table `LAB_08FA` (masques/joueur)                             | `DG_PROXIMITY = 18 px` (rayon)   |

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
`knight_id = 4` ou `5` dans la structure `KnightStruct` (ASM).

**ASM** — Valeurs initiales d'un Chevalier Noir (`LAB_01C6`) :

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

**C** (`black_knight_init`) — Les chevaliers noirs occupent les slots
`knights[]` non pris par des joueurs humains (`is_black_knight=1, human=0`).
Leurs statistiques au démarrage :

| Champ C           | Valeur             |
|-------------------|--------------------|
| `endurance`       | `DEFAULT_ENDURANCE` (2)  |
| `strength`        | 2                  |
| `constitution`    | 2                  |
| `max_hp` / `hp`   | 30                 |
| `steps_remaining` | 0 (réinitialisé au début de chaque tour) |

Le nombre de chevaliers noirs est `bk_count = 4 − human_count`.

### 5.2 Tour IA : `ai_turn_encounter()` = `LAB_0036` (ASM) — vs intégration tour (C)

**ASM** — Après que tous les joueurs humains ont joué leur tour, la boucle
principale appelle `LAB_0036` pour faire jouer chaque Chevalier Noir :

```text
LAB_0036 [program.asm] :
    LAB_011E = 2         → mode cinématique / transition
    Charger fond LAB_01D2
    Spawner LAB_00E6     → sprites HUD overworld + barres de score
    LAB_011E = 4         → mode overworld interactif
    Jouer l'animation du tour IA
    → Détecter collisions sur les nœuds (même pipeline que les joueurs)
```

**C** — Il n'y a **pas** de phase IA séparée. Les chevaliers noirs participent
au **même cycle de tours** que les joueurs humains via `ctx->current_knight`.
La boucle de tour dans `game_run_overworld` itère sur `knights[0..3]` sans
distinction : quand `current_knight` pointe sur un BK, la branche
`is_black_knight` de l'IA est exécutée à la place de l'input humain.

```
Ordre de jeu (C) :
  knights[0] (humain)          → tour joueur 1
  knights[1] (humain ou BK)    → tour joueur 2 / BK
  knights[2] (humain ou BK)    → tour joueur 3 / BK
  knights[3] (humain ou BK)    → tour joueur 4 / BK
  → tous turn_done = 1 → nouveau round (ctx->round++)
```

### 5.3 Algorithme de déplacement IA

**ASM** — Le Chevalier Noir se déplace sur le graphe de nœuds de `LAB_069F` :

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

**C** (`bk_turn_step`) — Algorithme miroir de `LAB_0DAD` / `LAB_0DE0` :

1. **Cible créature** : si aucune cible PVE, appel `bk_pick_creature_target`
   (tri-bulles des 24 nœuds par distance Manhattan, cible aléatoire parmi
   les rangs 1-3, miroir de `LAB_0DE0` / `LAB_0DEB`).
2. **Attaque joueur** : `BK_ATTACK_CHANCE = 25 %` de chance par step de
   basculer sur le joueur humain le plus proche (vs 20 % en ASM, `LAB_0DF8`).
3. **Mouvement** : un pixel par step en Bresenham simplifié (`LAB_0E0C`).
4. **Budget** : `steps_remaining` est décrémenté à chaque step ;
   `turn_done = 1` quand épuisé (budget = `endurance × 20 = 40 steps`).
5. **Déclenchement combat** : si à `BK_PROXIMITY = 14 px` du joueur cible →
   `bk->dead = 1` (le BK quitte la carte), `state = STATE_COMBAT`.
6. **Re-ciblage créature** : si le BK atteint son nœud créature, la cible
   est réinitialisée à −1 (forçant un nouveau choix au step suivant).

### 5.4 Rencontre Chevalier Noir ↔ joueur

**ASM** :

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

**C** : quand `bk_turn_step` retourne `hit >= 0` :

```c
ctx->node_type      = 0x01;   /* PvP */
ctx->current_knight = hit;    /* l'index du joueur attaqué */
ctx->state          = STATE_COMBAT;
```

Le chevalier noir est marqué `dead = 1` (il disparaît de la carte). La
récompense (clef) est gérée par `overworld_pve_take_key` après le combat.

### 5.5 Rencontres avec des créatures (type `0x02`)

**ASM** — En plus des Chevaliers Noirs, des créatures aléatoires sont disposées sur
la carte. Leur gestion est assurée par `LAB_005B` (handler type `0x02`) :

- La créature est sélectionnée depuis `LAB_08C6` (table de structures créatures).
- Compteur de rencontres : `20(creature)` est incrémenté à chaque rencontre.
- Quand `20(creature) == 3` : le cycle global `LAB_06C1` avance de 1 (mod 8),
  provoquant l'apparition de la créature suivante dans la table.

**C** — Les 24 nœuds PVE (`s_pve_nodes[]`) sont à positions fixes. Un BK
se dirige vers un nœud PVE comme cible principale (via `bk_pick_creature_target`).
Il n'y a pas de combat BK-créature : le BK atteint le nœud, puis en choisit
un nouveau. Seul un joueur humain (ou le dragon) peut déclencher un combat
sur un nœud PVE.

---

## 6. Ordre de jeu multi-joueurs (résumé)

### 6.1 ASM (original)

```text
DÉBUT DU TOUR :
   ┌─ Joueur 1 (si humain) : déplacement + événement éventuel
   ├─ Joueur 2 (si humain) : déplacement + événement éventuel
   ├─ Joueur 3 (si humain) : déplacement + événement éventuel
   ├─ Joueur 4 (si humain) : déplacement + événement éventuel
   └─ IA (Chevaliers Noirs) : tour automatique via LAB_0036 (phase séparée)

FIN DU TOUR :
   → Tous les rounds joués OU un joueur obtient la Moonstone
   → game_over() = LAB_003B
```

En mode 1 joueur, les 3 autres chevaliers (SIR GODBER, SIR JEFFREY,
SIR EDWARD) sont contrôlés par l'IA et se comportent comme des Chevaliers
Noirs supplémentaires avec leurs statistiques initiales de joueur.

### 6.2 Implémentation C

```text
DÉBUT D'UN ROUND (game_run_overworld) :
   → reset turn_done=0 et steps_remaining pour tous les knights[] actifs

BOUCLE DE TOUR (par tick) :
   current_knight pointe sur knights[0..3] dans l'ordre :

   knights[i].human == 1 :
     → Input SDL2 : déplace le sprite (2 px/tick, budget endurance×20)
     → FIRE → check_static_node / check_pve_node → STATE_COMBAT ou
               STATE_VILLAGE/TOWN/WIZARD/STONEHENGE
     → SPACE → turn_done = 1

   knights[i].is_black_knight == 1 :
     → bk_turn_step() : 1 px vers cible créature ou joueur par tick
     → si atteint joueur humain → STATE_COMBAT
     → turn_done = 1 quand steps_remaining == 0

   → Avancer current_knight au suivant non turn_done

   Quand tous turn_done → ctx->round++ → retour pour nouveau round

Dragon (indépendant du tour) :
   → dragon_update() appelé à chaque tick
   → collision → STATE_COMBAT (prioritaire sur tout)
```

La différence clé avec l'ASM : les Chevaliers Noirs jouent **dans le même
cycle** que les humains, pas dans une phase séparée. Le tour ne progresse
au round suivant que quand **tous** les `knights[]` (humains + BK) ont
`turn_done = 1`.

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

---

## 8. Tableau de synthèse : ASM original vs portage C

Ce tableau récapitule les différences entre le comportement original de
l'Amiga (ASM) et l'implémentation actuelle en C (`moon_overworld.c`).

| Fonctionnalité                      | ASM (Amiga)                                          | C (portage SDL2)                                      | Statut  |
|-------------------------------------|------------------------------------------------------|-------------------------------------------------------|---------|
| **Fond de carte**                   | `dw1.PIV` double-buffer 5 plans                      | `dw1.PIV` décodé → `uint32_t[GAME_W*GAME_H]`         | ✅ Équivalent |
| **Nœuds statiques**                 | `LAB_069F` : table triplets `(type,x,y)` + `$FFFF`  | `s_nodes[]` : 9 entrées identiques                   | ✅ Équivalent |
| **Nœuds PVE créatures**             | `LAB_05C6` : 24 entrées × 20 octets, stride dynamique | `s_pve_nodes[]` : 24 entrées, positions fixes        | ✅ Équivalent |
| **Mouvement joueur**                | Joystick libre, déplacement pixel par pixel, `LAB_0E0C` | SDL2, 2 px/tick, `steps_remaining` (endurance×20)  | ✅ Équivalent (budget en px vs frames) |
| **Animations de marche**            | Scripts `LAB_00D7/D8/D9/DA`, frames `.ob` interpolées | Cycle simple 8 frames `s_kn_frame`, pas d'interpolation | ⚠️ Partiel |
| **Budget de déplacement**           | 40 frames par tour (`LAB_0038`)                      | `endurance × 20` pixels par tour                     | ✅ Équivalent (sémantique différente) |
| **Fin de tour joueur**              | Après 40 frames ou arrivée sur un nœud               | SPACE / FIRE / steps_remaining = 0                   | ✅ Équivalent |
| **Interaction nœud**                | `LAB_006B` + `LAB_0067` + `LAB_0E45` dispatcher      | `check_static_node` + `check_pve_node` inline        | ✅ Équivalent |
| **HUD overworld**                   | `LAB_00E6` : barres HP/XP/reliques, 12 icônes        | Texte simple `render_text` (nom, HP, or, steps)      | ⚠️ Simplifié |
| **Dragon — apparition**             | Round ≥ 2, `LAB_0DCB`, pool d'entités Amiga          | Round ≥ 2, `dragon_init`, champs `GameCtx`           | ✅ Équivalent |
| **Dragon — vol**                    | `LAB_0DCF` handler/frame, 2 phases, table `LAB_08FC` | `dragon_update()` par tick, même logique 2 phases    | ✅ Équivalent |
| **Dragon — collision**              | Masques `LAB_08FA` par joueur                        | Cercle `DG_PROXIMITY = 18 px` sur tous les knights[] | ✅ Équivalent |
| **Parchemin du Wyrm**               | `LAB_0992` / `LAB_0E26` : redirige dragon            | Non implémenté (pas d'inventaire interactif)         | ❌ Absent |
| **Talisman du Wyrm**                | `LAB_098E`                                           | Non implémenté                                       | ❌ Absent |
| **Chevaliers noirs — init**         | `LAB_01C6` : structs séparés, `enemy_flag=0xFF`      | `black_knight_init` : slots `knights[]` libres       | ✅ Équivalent |
| **Chevaliers noirs — phase de jeu** | `LAB_0036` : phase IA séparée après tous les humains | Intégrés dans le même cycle de tours (`knights[]`)   | ✅ Équivalent (implémentation différente) |
| **Chevaliers noirs — déplacement**  | Graphe de nœuds `LAB_069F`, `LAB_0DE0` tri créatures | `bk_turn_step` : Bresenham, tri-bulles, budget steps | ✅ Équivalent |
| **Chevaliers noirs — attaque**      | `LAB_0DF8` : 20 % chance de poursuivre un joueur     | `BK_ATTACK_CHANCE = 25 %` (légère différence)        | ✅ Approximatif |
| **Chevaliers noirs — stats**        | skill=5, hp=20, épée longue, armure rembourrée        | strength=2, constitution=2, hp=30 (valeurs différentes) | ⚠️ Différent |
| **Combat BK ↔ joueur**              | `LAB_004F`, transfert items après victoire           | `STATE_COMBAT`, `node_type=0x01`                     | ✅ Équivalent |
| **Combat joueur ↔ créature**        | `LAB_005B`, pillage multi-items, cycle créatures     | `STATE_COMBAT`, `node_type=0x02`, clef unique         | ⚠️ Simplifié |
| **Pillage tombe (`0x21`)**          | `LAB_004F` : récupère l'équipement du mort           | Non implémenté                                       | ❌ Absent |
| **Village natal**                   | `LAB_00B0` : skill +1 si bonne faction               | `STATE_VILLAGE`                                      | ✅ Délégué à moon_village.c |
| **Cités (Highwood, Waterdeep)**     | Menus 5 options (arène, achat, sorcier, forge/taverne) | `STATE_TOWN`                                       | ✅ Délégué à moon_town.c |
| **Stonehenge**                      | `LAB_00A1` : offrande → skill +1                     | `STATE_STONEHENGE`                                   | ✅ Délégué à moon_stonehenge.c |
| **Vallée des Dieux**                | `LAB_009D` : requiert 4 clefs, combat final          | `STATE_STONEHENGE` (vérification clefs déléguée)     | ✅ Délégué |
| **Math le Sorcier**                 | `LAB_007C` : restaure HP, skill payant               | `STATE_WIZARD`                                       | ✅ Délégué à moon_wizard.c |
| **Musique**                         | `vmusic.cmp` / `music.cmp` RNC1 → MOD SoundTracker  | Même décompression RNC1, lecture via `hal_music_play_raw` | ✅ Équivalent |
| **Double-buffer Amiga**             | 2 buffers bitmap 5 plans, flip VBL                   | Framebuffer `uint32_t` unique, `hal_present`          | ✅ Équivalent (SDL2 gère le flip) |
