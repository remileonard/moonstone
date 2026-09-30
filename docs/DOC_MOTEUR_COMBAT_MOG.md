# Moteur de combat de `mog` — boucle, entités et scripts

> Lecture directe de `amiga_asm/mog.asm` (désassemblage vérifié, noms tirés de
> `amiga_asm/mog.sym`). Les anciens noms IRA sont entre crochets.
>
> **Correctifs par rapport aux autres documents** : la boucle de combat est
> `Combat_Run`/`Combat_Loop` [`LAB_0036`/`LAB_0037`], et non la navigation
> overworld ; `LAB_04CF`/`LAB_04D0` est l'écran qui *suit* le combat (butin,
> temple, boutique). Le champ `77` d'un objet est l'**index de contrôleur**,
> pas un type d'arme. Les opcodes de script de `mog` n'ont **pas** la même
> sémantique que ceux de l'intro dans `program`.

## 1. Boucle

```
Combat_Run [LAB_0036]                     appelé par tous les combats
  init (v_Combatants[8] = 1 : combat actif)
  Combat_Loop [LAB_0037]:                 UNE image
    Combat_FrameStart                     note v_VblCounter
    Combat_RunControllers [LAB_0322]      décisions (joystick / IA) -> scripts
    Ix_RunEntities        [LAB_0328]      une étape de script par entité + dessin
    LAB_0416                              affichage (à documenter)
    Combat_Collisions     [LAB_03BE]      contacts frappe/corps (§8)
    LAB_039E                              (à documenter)
    Combat_LowHpIndicator [LAB_003E]
    Combat_CheckEnd       [LAB_004B]      PV joueur < 0 -> fin dans 50 images ; pause
    Combat_FrameWait      [LAB_031F]      complète jusqu'à v_FrameVbls VBL
  tant que combat actif, puis encore v_Combatants[16] images
  Combat_CheckKO [LAB_000E]               perdant, PV restaurés
```

**Cadence** : `v_FrameVbls` [`LAB_05BA`] = **6 VBL par image en combat**
(8,33 images/s en PAL), 2 sur la carte. Une étape de script = une image.

## 2. Entités (`t_Entities` [`LAB_0649`], 10 × 50 octets)

| Off | Taille | Rôle |
|---|---|---|
| 0 | b | active |
| 1 | b | occupée (script en cours) ; remise à 0 à la fin du script → le contrôleur est rappelé |
| 2 | L | pointeur de script (pc) |
| 6 / 8 / 10 | W | X / hauteur (vers le haut) / profondeur |
| 12 / 14 | W | position de dessin calculée |
| 16 / 18 | W | largeur / hauteur de la frame courante |
| 21 | b | numéro de frame |
| 22 | b | direction : bit 1 = tourné à gauche (1 droite, 3 gauche) |
| 24 | L | objet (structure chevalier/créature, 132 octets) |
| 28 | L | table des banques CEL |
| 32 | b | index du contrôleur (`t_Controllers`) |
| 36 | L | contexte de script (36 octets, §4) |
| 40 / 44 | L | listes de frames marquées (drapeaux 1 / 0 du dessin), pour les collisions |
| 48 | W | gelée (non exécutée) |

À la fin de chaque étape, l'objet reçoit X/hauteur/profondeur/direction
(4/6/8/10) et la boîte englobante des frames dessinées (58 X min, 60 X max,
112 Y min, 114 Y max).

`Ix_SortByDepth` trie les entités par profondeur avant exécution (ordre
d'affichage).

## 3. Contrôleurs

`Combat_RunControllers` appelle, pour chaque entité non occupée (ou dont
l'objet a `14`/`18` non nuls : contact à l'image précédente),
`t_Controllers[32(entité)]`.
Le contrôleur renvoie : A0 = script (−1 : rien, 0 : détruire l'entité),
D0 X, D1 hauteur, D2 profondeur, D3 direction.

`Ctl_HumanKnight` [`LAB_01CA`] (index 12) : joystick dans `62(objet)`
(bit 0 droite, 1 gauche, 2 bas, 3 haut, 4 feu).
- sans feu : marche en 4 phases (`12(objet)`), script
  `46(objet)[groupe + phase×4]` (groupe 0 horizontal, $20 haut, $40 bas),
  pas `t_WalkStepX/Up/Down` ; sans mouvement : script de repos `22(objet)` ;
- feu : `Ctl_HumanAttack` → index = `t_AttackStickR/L[bits]` (selon la
  direction), script `34(objet)[index]`, index gardé dans `64(objet)` ;
- tables de l'objet (catalogue complet : `docs/scripts/mog_scripts.md`) :
  `30` scripts, `34` attaques, `42` **dégâts par attaque** (nombres retirés
  aux PV de la cible, ex. `MOVE.L 4(A2),D0 / SUB.W D0,80(A1)`), `46` marche,
  `50` nombres comparés à l'attaque adverse ; `22` repos, `26` réaction ;
- `18(objet)` non nul (touché par quelqu'un) : réaction au coup reçu
  (`LAB_01EC`) ;
- `14(objet)` non nul (a touché quelqu'un) : réaction selon le contrôleur de
  la cible (table `LAB_0622`) — voir §8.3.

## 4. Contexte de script (36 octets, `36(entité)`)

| Off | Rôle |
|---|---|
| 0 / 1 / 2 | Hold : compteur / actif / pc de reprise |
| 6 / 7 / 8 | Boucle : compteur / active / pc de début |
| 12 / 16 | Saut différé : adresse / actif |
| 18 / 20 | Script secondaire (ombre) : actif / adresse |
| 24–31 | Physique : 24 param, 25 drapeaux, 26 active, 27 compteur, 28 vy, 29 vy limite, 30 vx, 31 vx limite |
| 32 | pc de reprise de la physique (aussi cible de l'octet `$FD`) |

## 5. Format d'une étape

Une étape est une suite d'enregistrements lus à `pc`, terminée par `FF xx`.

- `$FF` : fin d'étape (§6).
- `$FD` : saut à `32(ctx)` ; `$FE` : saut à `8(ctx)` (début de boucle).
- `>= $80` : opcode de contrôle, handler `t_IxOpcodes[op & $7F]` (§7).
- sinon **dessin**, 6 octets :

| Octet | Rôle |
|---|---|
| 0 | banque CEL (`b & $1F`, multiple de 4) |
| 1 | frame |
| 2 | décalage vertical signé |
| 3 | drapeaux |
| 4–5 | décalage horizontal (mot) |

Position : X = `X + dx` (si tourné à gauche : `X − dx − largeur`),
Y = `dy + hauteur + profondeur`.
Drapeaux : bit 0 → liste `44`, bit 1 → liste `40` (collisions),
bit 4 → dessin permanent dans le décor, bit 5 → `LAB_0D05`,
bit 6 → pas dans la boîte englobante, bit 7 → ignoré en mode debug.
Une étape peut dessiner plusieurs frames (sprite composite).

## 6. Fin d'étape (`Ix_StepEnd`), dans l'ordre

1. Hold actif : compteur − 1 ; s'il reste des images, **rejouer l'étape**
   (pc = reprise du `$88`) et s'arrêter là.
2. Physique active : compteur − 1 ; si ≥ 0, appliquer `Ix_Physics` et
   (sauf drapeau bit 6) revenir au pc de reprise ; s'arrêter là.
3. Saut différé actif : pc = adresse.
4. Sinon selon `xx` : `00` étape suivante ; `FE` boucle (compteur du `$94`) ;
   `FF` fin du script — boucle éventuelle, sinon **entité libre** (le
   contrôleur choisira le prochain script à l'image suivante).

## 7. Opcodes (moteur de `mog`)

| Op | Taille | Handler | Effet |
|---|---|---|---|
| `$80 d` | 2 | `IxOp80_SetDir` | direction (`$FF` : inverser) |
| `$84 m a.L` | 6 | `IxOp84_Jump` | m=3 : saut immédiat ; sinon saut en fin d'étape |
| `$88 n` | 2 | `IxOp88_Hold` | l'étape dure n images (0 : aléatoire 1–31) |
| `$8C a n f vy vymax vx vxmax` | 8 | `IxOp8C_Physics` | mouvement balistique n images (f : bit1 montée, bit0 chute, bit4 avant, bit2 arrière, bit5/7 pas d'accélération, bit6 pas de retour) |
| `$90` | — | — | non installé |
| `$94 n` | 2 | `IxOp94_Loop` | boucle n fois |
| `$98 x a.L` | 6 | `IxOp98_SkipIfDebug` | saut si mode debug (`LAB_06DA`) |
| `$9C` | — | `IxOp9C_Nop` | RTS sans avancer : inutilisable |
| `$A0 f dx.W dh.W dd.W` | 8 | `IxOpA0_Move` | déplacement relatif (dx selon la direction ; f bit0 sens X, bit3 dh négatif, bit5 dd négatif) ou absolu (f bit6) |
| `$A4 n` | 2 | `IxOpA4_Sound` | effet sonore n |
| `$A8 t off.W v.L` | 8 | `IxOpA8_SetField` | écrit dans l'objet (t bit0 octet, bit1 mot, sinon long) |
| `$AC on a.L` | 6 | `IxOpAC_Shadow` | script secondaire (ombre) |
| `$B0 x a.L` | 6 | `IxOpB0_Call` | appelle une routine native |
| `$B4 x a.L` | 6 | `IxOpB4_IfDead` | si PV ≤ 0 : saut + remise à zéro |
| `$B8 x a.L` | 6 | `IxOpB8_Spawn` | crée une entité enfant (contrôleur 40) |
| `$BC` | 2 | `IxOpBC_Kill` | détruit l'entité |
| `$C0 n` | 2 | `IxOpC0_SetBank` | banque CEL n |
| `$C4 x a.L` | 6 | `IxOpC4_IfSameFacing` | saut si même direction que le joueur |
| `$C8 t off.W a.L` | 8 | `IxOpC8_IfFieldZero` | saut si champ de l'objet = 0 |
| `$CC t off.W a.L` | 8 | `IxOpCC_IfFieldNonZero` | saut si champ ≠ 0 |
| `$D0` | 2 | `IxOpD0_Reset` | remise à zéro du contexte |

## 8. Collisions (`Combat_Collisions` [`LAB_03BE`])

Exécutée **après** `Ix_RunEntities`, donc sur les frames réellement
dessinées dans l'image.

### 8.1 Données

- `Ix_Step` range chaque frame dessinée dont les drapeaux ont :
  - bit 1 → **frappe** : liste `40(entité)` → `t_StrikeFrames` [`LAB_064F`] ;
  - bit 0 → **corps** (peut être touché) : liste `44(entité)` →
    `t_BodyFrames` [`LAB_0650`].

  Chaque liste : 8 enregistrements de 10 octets (CEL.L, frame.W, X.W, Y.W,
  position de dessin), terminée par un pointeur nul ; les deux tables sont
  vidées à chaque image (`Combat_ClearFrameLists`).
- `collide.hit` (texte, `Col_InitHitFile` / `Col_LoadHitData`) donne pour
  chaque CEL et chaque frame les **points d'impact** de l'arme :
  `n, type, largeur, hauteur, n × (x, y)` ; enregistrement de `2n + 4`
  octets, ou 1 octet si `n = 0` (pas de frappe possible sur cette frame).
  Table `t_HitDataByCel` [`LAB_0A51`] : paires (CEL, données).

### 8.2 Algorithme

```
Combat_ClearHitLinks          14 et 18 de tous les objets remis à 0
pour chaque entité A active :
  pour chaque frame F de A marquée « frappe » :
    pour chaque autre entité B active avec |profondeur A − profondeur B| ≤ 10 :
      pour chaque frame G de B marquée « corps » :
        si Col_PixelHit(F, G) :
          objet(A).14  = objet(B)      « a touché »
          objet(B).18  = objet(A)      « touché par »
          objet(B).122/124 = point d'impact (v_HitX, v_HitY)
          passer à l'entité suivante   (un seul contact par attaquant et par image)
```

`Col_PixelHit` [`LAB_03DB`] :
1. Points de la frame F dans `collide.hit` ; aucun point → pas de contact.
2. Si la frame F est retournée, les points sont en miroir :
   `x' = largeur − x` (`v_HitMirrorW`). L'orientation est celle de l'image
   en mémoire : chaque frame CEL a un enregistrement de 10 octets (offset
   données.L, largeur.W, hauteur.W, orientation.B, masque de plans.B) ;
   `LAB_0CCE` retourne l'image **en place** quand la direction de l'entité
   change, et met l'orientation à 1 (sens d'origine) ou à
   `décalage d'alignement << 4` (retournée, bit 0 à 0).
3. Test grossier : rectangle (largeur, hauteur) des points de F contre le
   rectangle de G (`Col_SpanOverlap` sur X et Y).
4. Test fin : chaque point, placé à la position de dessin de F, doit tomber
   dans G (intervalle `[x, x + largeur[`) **sur un pixel opaque** : au moins
   un des plans actifs de G a le bit à 1 (`t_Popcount4` donne le nombre de
   plans à tester).
5. Le premier point qui touche est renvoyé (`v_HitX`, `v_HitY`).

### 8.3 Suite : qui réagit

Les liens 14/18 ne restent valables que jusqu'aux collisions de l'image
suivante. Entre les deux, `Combat_RunControllers` appelle les contrôleurs
des objets dont 14 ou 18 est non nul, **même si leur script est en cours**
(interruption). Pour le chevalier humain : 18 → réaction au coup reçu
(`LAB_01EC`), 14 → réaction à un coup porté selon le type de la cible
(table `LAB_0622`). Les dégâts ne sont pas calculés ici : ils sont appliqués
par ces réactions (à documenter).

### 8.4 Écarts de `check_hit_hitdata` (moon_combat.c)

Le test au pixel du C suit `Col_PixelHit`, mais :
- il teste **une** frame par combattant, déduite d'un état codé en dur, au
  lieu de **toutes** les frames dessinées marquées frappe/corps (un sprite
  composite a souvent l'arme sur une frame séparée) ;
- il place les sprites par centre/bas au lieu des positions de dessin
  exactes du script (`X + dx`, ou `X − dx − largeur` si retourné) ;
- le retournement vient de `facing` et non de l'orientation de la frame ;
- il n'y a pas de condition de profondeur (≤ 10) ;
- le résultat est traité sur place (constantes de dégâts) au lieu de poser
  les liens 14/18 lus par les contrôleurs à l'image suivante.

## 9. Conséquences pour le portage C

- Cadence fixe : une étape de script toutes les 6 VBL (≈ 120 ms), pas à
  chaque image affichée.
- Un moteur calqué sur l'intro (`program`) ne suffit pas : en combat,
  `$A8`, `$AC`, `$B0`, `$B4`, `$B8`, `$C4`, `$C8`, `$CC`, `$D0` ont un effet
  réel (écriture de champs, conditions sur l'état, créations d'entités,
  mort). Les tailles diffèrent aussi (`$8C` fait 8 octets, `$A0` 8, etc.).
- Actions du joueur, de l'IA, déplacements, dégâts et réactions sont portés
  par les scripts et les contrôleurs : le combat C doit reproduire cette
  architecture (entités + contrôleurs + moteur de scripts) plutôt qu'une
  machine à états codée en dur.

## 10. Moteur C (`game/src/ix_engine.c`)

Portage routine par routine des §1–7 : chaque fonction C indique la routine
de `mog.asm` qu'elle traduit.

- **Mémoire** : `game/src/ix_vm.c` charge l'image de `mog` (hunks +
  relocations, générée par `tools/ix_scripts.py` dans `game/data/ix_mog.c`)
  à `IX_VM_BASE`. Entités, contextes, objets, scripts et tables y sont aux
  mêmes adresses et au même format (big-endian) que sur l'Amiga ; les
  adresses sont dans `game/data/ix_mog_syms.h` (`MOG_<label>`).
- **Hôte** (`IxHost`) : ce qui sort du moteur — dimensions et dessin d'une
  frame CEL (la « CEL » est une poignée : l'adresse rangée dans la table de
  banques), sons (`$A4`), routines natives (`$B0`), messages.
- **Hors moteur** (à porter à part) : contrôleurs (`t_Controllers`),
  collisions (§8), restauration du décor (pile `LAB_0641`).

### Validation

`tools/ix_difftest.py` exécute le code 68000 d'origine (`Ix_RunEntities`)
sous Unicorn et le moteur C (`tests/ix_trace.c`) sur les mêmes scénarios,
puis compare à chaque image les appels (dessins, sons, `$B0`) et une
empreinte CRC32 de toute la mémoire. État actuel : chacun des 259 scripts
seul puis en paire, et 40 scénarios à 4 entités, 60 images chacun :
**558 scénarios, 0 écart**.

```
cmake --build build --target ix_trace
python3 tools/ix_difftest.py [--frames 60] [--only LAB_07F2] [--seed 1]
```

### Visionneuse

`moon-ix-view <dossier_données>` monte le chevalier comme `mog` (banques
`LAB_05E1` = kn1, kn2, kn3, kn4, Kn5.ob — `LAB_0115`, `LAB_0120` ; objet
comme `LAB_0167`) et joue ses scripts : 1–8 attaques (`LAB_05F5`), flèches
marche (`LAB_0610`), espace change de sens, N parcourt les 259 scripts.
`--png fichier.png [images]` produit une planche sans écran. La marche
anime sur place : le déplacement est fait par le contrôleur natif
(`LAB_0DCB`), pas par le script.

## 11. Contrôleurs, collisions et banc de référence

### Code C

| Fichier | Contenu (routines d'origine) |
|---|---|
| `game/src/mog_ctl.c` | `Combat_RunControllers`, `Ctl_HumanKnight` (marche, attaques, obstacles `LAB_03A9`, bords `LAB_0215`, décor `LAB_0A71`), réactions aux coups (`LAB_0621`/`LAB_0622`), `LAB_02CB` (objet lancé), `LAB_02D2` |
| `game/src/mog_ai.c` | IA : 0 `LAB_0226` chevalier qui traverse ; 4 `SECSTRT_40` Mudmen ; 8 `LAB_0ED2` Démon ; 16/56 `LAB_0EFF` chevalier géré par l'ordinateur ; 20 `LAB_027A` Dragon ; 24/28/32 `LAB_0236` Troggs ; 36 `LAB_0251` hommes-rats ; 44 `LAB_0298` ; 48 `LAB_029F` ; 64 `LAB_0EC2` ; déplacement commun `LAB_0F1A`-`LAB_0F32`, trajectoires `LAB_02D3`/`LAB_02F6`/`LAB_02FD` |
| `game/src/mog_col.c` | `Combat_Collisions`, `Col_PixelHit`, `Combat_ClearHitLinks` |
| `game/src/mog_native.c` | routines appelées par `$B0` (sons, recul, lancer, fin de combat, tremblement d'écran...), aléatoire `LAB_04A1` |
| `game/src/mog_setup.c` | adversaire suivant (`LAB_05F0` -> `LAB_0174`), équipement des créatures (`LAB_05F1`), entrée `LAB_01A8` |

Le contrôleur 68 (`LAB_04AC`) n'est pas un contrôleur de combat (mini-jeu
à part). Les routines passent parfois des valeurs par registres d'une
routine à l'autre (ex. `D1` de `LAB_0F24`) : `mog_ai.c` les suit dans une
petite structure `Regs`. Les bizarreries d'origine sont reproduites et
signalées en commentaire (champs effacés sur la cible dans `LAB_0237`,
arguments inversés de `LAB_030D` dans `LAB_0EB2`, lecture des vecteurs
68000 dans `LAB_02F8`...).

### Banc de référence (`tools/mog_ref.py`)

`mog` d'origine exécuté par Unicorn avec les vraies données : fichiers
(`LAB_0BB5`/`0BD7`/`0BEA`/`0BFF`), VBL, joystick (`LAB_00EE`), sons
(`LAB_0AA2`, `LAB_0F8C`), palette (`LAB_0D8A`), dessin (`LAB_0CDA`),
restauration du décor et écrans de message remplacés par des crochets.

```
python3 tools/mog_ref.py <données> --duel | --cpu | --encounter LAB_0168 [--frames N]
```

Rencontres du menu de débogage `LAB_007D` : `LAB_0168` (Troggs à la hache),
`LAB_016A`, `LAB_0175`, `LAB_018C` (hommes-rats), `LAB_0188`, `LAB_0192`
(Dragon), `LAB_0196`, `LAB_019A` (Mudmen), `LAB_019E`, `LAB_01A0` (Démon).

### Test différentiel (`tools/mog_difftest.py`)

À chaque image d'un vrai combat (joueurs simulés qui s'approchent et
attaquent), la mémoire est copiée ; `Combat_RunControllers`, puis
contrôleurs + `Ix_RunEntities` + `Combat_Collisions`, sont exécutés par
l'original et par le C (`tests/mog_step.c`), et toute la mémoire (hors pile
de l'émulateur) ainsi que les sons et palettes sont comparés.

```
python3 tools/mog_difftest.py <données> [--cpu | --encounter LAB_xxxx] [--frames N]   (défaut : duel)
```

### Bilan (données complètes, 600 images par rencontre)

| Rencontre / mode | Contrôleurs | Résultat |
|---|---|---|
| duel (`LAB_0002`) | 12, 12, 52 | 0 écart (jusqu'à 2000 images, 7 duels) |
| duel contre chevalier IA | 12, 16 | 0 écart |
| `LAB_0188` chevalier qui traverse | 0 | 0 écart |
| `LAB_0168` Troggs à la hache | 24 | 0 écart |
| `LAB_016A` | 28 | 0 écart |
| `LAB_0175` | 32 | 0 écart |
| `LAB_018C` hommes-rats | 36, 40 | 0 écart |
| `LAB_019A` Mudmen | 4 | 0 écart |
| `LAB_01A0` Démon | 8 | 0 écart |
| `LAB_0192` Dragon | 20, 44 | 0 écart |
| `LAB_0196` | 48 | 0 écart |
| `LAB_019E` | 64 | 0 écart |

### Démarrage (`game/src/mog_boot.c`)

Déjà porté et identique à l'original (`tools/mog_bootcheck.py`) :
découpage mémoire `LAB_0004`, tables `LAB_0152`/`LAB_0156`, `LAB_0303`,
tables de réaction `LAB_020F`, chargeur de CEL `LAB_0CBB` (LZSS : un octet
de contrôle pour 8 jetons, copie arrière de 34 − (mot >> 11) octets),
`Col_InitHitFile`, `Col_LoadHitData` (qui charge aussi la CEL), CEL du
chevalier `LAB_0115`, tampons graphiques de `SECSTRT_30` (dont `LAB_0D40`,
tampon de retournement des frames) et table d'inversion des bits `LAB_0CD9`.

## 12. Préparation des rencontres et passerelle avec le jeu

### Préparation (`game/src/mog_encounter.c`)

Nouvelle partie (`LAB_01AE` pour la partie combat, `LAB_01BE`, `LAB_0011`),
les 11 routines de `t_CreatureInit`, écran de chargement `LAB_0134`, décors
`LAB_013C` (PIV du fichier « Test » décodés par `LAB_0C21` dans les plans
de `LAB_05C0`, `LAB_05C1`, `LAB_0D92`), terrain `.t` (`LAB_0A6D` :
obstacles dans `SECSTRT_14`, objets posés par `SECSTRT_12` / `LAB_0A64`,
reproduction exacte du blit « D = A | ¬B & C »), chargeurs de créatures
(`LAB_0116` à `LAB_0126`, banques de sons `*.a`), nombre d'adversaires
`LAB_0177`, palette `LAB_03F3`.

`tools/mog_setupcheck.py <données> [LAB_xxxx ...]` : après une nouvelle
partie dans le banc, la routine de rencontre puis `Combat_Run` sont
exécutées par l'original et par le C ; **mémoire identique pour les 11
rencontres**, hors écrans (le banc n'émule pas le blitter : terrain et
copies d'écran n'y sont pas faits), palette courante du fondu (tenue par
l'interruption d'image) et variables sans objet (disque demandé,
compteur d'images, affichage des textes de `LAB_0432`).

### Combat complet (`game/src/mog_fight.c`)

`mog_fight_boot` (démarrage comme `SECSTRT_0`), `mog_fight_start`
(chevalier rempli d'après le jeu, rencontre, `Combat_Run`),
`mog_fight_frame` (une image de `Combat_Loop`, joysticks), `mog_fight_render`
(décor lu dans les plans de `LAB_05C0`, sprites décodés des CEL en mémoire
de mog au fil des dessins du moteur, couleur 0 transparente, plans absents
à 0 comme `LAB_0CDA`).

`game/src/moon_combat.c` (`game_run_combat`) : passerelle GameCtx ↔ mog.
Créature : `t_CreatureInit[pve_creature_type]`, lieu d'après
`pve_node_group` (fol → forêt `FO?.t`, wal → `Wa?.t`, swl → marais
`Sw?.t`, gll → `GL?.t`) ; chevaliers (0x01, 0x21) : `LAB_0164`, adversaire
humain au joystick 2 ou chevalier noir (IA `LAB_0EFF`) ; Vallée (0x1c) :
Démon `LAB_01A0`. 6 VBL par image (`v_FrameVbls`). Fin : PV <= 0, une vie
de moins et PV rendus (`Combat_CheckKO`).

`build/tests/mog_fight_shot <données> <préfixe> <rencontre> <lieu>
[images] [pas]` : même combat sans écran, joueur piloté, images PNG.

Reste à faire : sons (banques `*.a` en mémoire de mog), fondus et
pulsations de couleurs (`LAB_0E5D`, indicateur de PV faibles), fin de
combat de l'original (`LAB_0048`, `LAB_0114`, butin `LAB_001C`),
inventaire du jeu vers celui de mog.
