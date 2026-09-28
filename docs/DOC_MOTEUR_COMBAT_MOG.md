# Moteur de combat de `mog` — boucle, entités et scripts

> Lecture directe de `amiga_asm/mog.asm` (désassemblage vérifié, noms tirés de
> `amiga_asm/mog.sym`). Les anciens noms IRA sont entre crochets.
>
> **Correctifs par rapport aux autres documents** : la boucle de combat est
> `Combat_Run`/`Combat_Loop` [`LAB_0036`/`LAB_0037`], et non la navigation
> overworld ; `LAB_04CF`/`LAB_04D0` est l'écran qui *suit* le combat (butin,
> temple, boutique). Le champ `77` d'un objet est l'**index de contrôleur**,
> pas un type d'arme. Les opcodes de script de `mog` n'ont **pas** la même
> sémantique que ceux de l'intro dans `program` (sur lesquels
> `tools/imagexcel.c` est calqué).

## 1. Boucle

```
Combat_Run [LAB_0036]                     appelé par tous les combats
  init (v_Combatants[8] = 1 : combat actif)
  Combat_Loop [LAB_0037]:                 UNE image
    Combat_FrameStart                     note v_VblCounter
    Combat_RunControllers [LAB_0322]      décisions (joystick / IA) -> scripts
    Ix_RunEntities        [LAB_0328]      une étape de script par entité + dessin
    LAB_0416, LAB_03BE, LAB_039E          affichage, collisions (à documenter)
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
l'objet a `14`/`18` non nuls : coup reçu), `t_Controllers[32(entité)]`.
Le contrôleur renvoie : A0 = script (−1 : rien, 0 : détruire l'entité),
D0 X, D1 hauteur, D2 profondeur, D3 direction.

`Ctl_HumanKnight` [`LAB_01CA`] (index 12) : joystick dans `62(objet)`
(bit 0 droite, 1 gauche, 2 bas, 3 haut, 4 feu).
- sans feu : marche en 4 phases (`12(objet)`), script
  `46(objet)[groupe + phase×4]` (groupe 0 horizontal, $20 haut, $40 bas),
  pas `t_WalkStepX/Up/Down` ; sans mouvement : script de repos `22(objet)` ;
- feu : `Ctl_HumanAttack` → index = `t_AttackStickR/L[bits]` (selon la
  direction), script `34(objet)[index]`, index gardé dans `64(objet)` ;
- `14(objet)` non nul (touché) : réaction selon le contrôleur de l'attaquant
  (table `LAB_0622`).

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

## 8. Conséquences pour le portage C

- Cadence fixe : une étape de script toutes les 6 VBL (≈ 120 ms), pas à
  chaque image affichée.
- Le moteur de `tools/imagexcel.c` suit l'intro (`program`) : en combat,
  `$A8`, `$AC`, `$B0`, `$B4`, `$B8`, `$C4`, `$C8`, `$CC`, `$D0` ont un effet
  réel (écriture de champs, conditions sur l'état, créations d'entités,
  mort). Les tailles diffèrent aussi (`$8C` fait 8 octets, `$A0` 8, etc.).
- Actions du joueur, de l'IA, déplacements, dégâts et réactions sont portés
  par les scripts et les contrôleurs : le combat C doit reproduire cette
  architecture (entités + contrôleurs + moteur de scripts) plutôt qu'une
  machine à états codée en dur.
