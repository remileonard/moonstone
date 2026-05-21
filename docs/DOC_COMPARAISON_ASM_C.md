# Analyse comparative : code ASM original vs réimplémentation C

> Moonstone — A Hard Days Knight  
> Comparaison entre `amiga_asm/mog.asm` (code 68000 Amiga original) et
> `game/src/moon_combat.c` (réimplémentation C moderne).  
> Document généré à partir d'une lecture directe des deux sources.

---

## Table des matières

1. [Machine à états / entrée dans le combat](#1-machine-à-états--entrée-dans-le-combat)
2. [Positionnement initial des combattants](#2-positionnement-initial-des-combattants)
3. [Boucle de jeu principale](#3-boucle-de-jeu-principale)
4. [Gestion des inputs joystick](#4-gestion-des-inputs-joystick)
5. [Animation des sprites / FSM de combat](#5-animation-des-sprites--fsm-de-combat)
6. [Système de dégâts / détection de collision](#6-système-de-dégâts--détection-de-collision)
7. [Fin de combat / transition](#7-fin-de-combat--transition)
8. [Écran de butin / inventaire post-combat — absence de HUD pendant le combat](#8-écran-de-butin--inventaire-post-combat--absence-de-hud-pendant-le-combat)
9. [Tableau récapitulatif](#9-tableau-récapitulatif)
10. [Écarts non conformes](#10-écarts-non-conformes)

---

## 1. Machine à états / entrée dans le combat

### Code ASM original

**Point d'entrée : `LAB_04CF` (mog.asm, ligne 10547)**

```asm
LAB_04CF:
    MOVE.W  #$0001,LAB_0D05     ; flag "combat actif"
    LEA     LAB_05E2,A0
    MOVE.L  4(A0),LAB_0986      ; sauvegarder contexte sprites
    MOVE.W  #$0000,LAB_0689     ; réinitialiser flag adversaire
    MOVE.L  D0,LAB_068F         ; stocker l'état reçu dans D0
    MOVE.W  #$0000,LAB_0984     ; réinitialiser flag fin de combat
    JSR     LAB_03F0             ; fondu palette → noir
    JSR     LAB_0575             ; activer lecture joystick (joueur)
    JSR     LAB_0588             ; initialiser toutes les tables d'animation
    JSR     LAB_04D4             ; initialiser les combattants
```

La variable `LAB_068F` (long) encode l'**état courant** :

| `LAB_068F` | Type de combat                    | Routine de dispatch |
|------------|-----------------------------------|---------------------|
| `1`        | PvP chevalier vs chevalier        | `LAB_04D7`          |
| `2`        | PvE créature                      | `LAB_04D6`          |
| `3`        | Sorcier Mythral (Mystic)          | `LAB_058F`          |
| `5`        | Arène en ville                    | `LAB_0522`          |
| `9`        | Temple de soin                    | `LAB_0591`          |
| `10` (0xa) | Vallée des Dieux (boss)           | `LAB_0590`          |
| `11` (0xb) | PvP alternatif (même table 1)     | `LAB_058C`          |

`LAB_04D4` (ligne 10587) initialise les pointeurs combattants :
- `LAB_068B` = chevalier joueur
- `LAB_068C` = stats joueur
- `LAB_068D` = adversaire
- `LAB_068E` = stats adversaire

### Code C réimplémenté

**Fonction `game_run_combat(GameCtx *ctx)` (moon_combat.c, ligne 627)**

```c
void game_run_combat(GameCtx *ctx)
{
    // ctx->node_type encode le type de combat
    int enemy_is_knight = (ctx->node_type == 0x01 || ctx->node_type == 0x21);
    ...
    while (ctx->state == STATE_COMBAT) { ... }
}
```

Le C utilise `ctx->node_type` (entier) à la place de `LAB_068F` (long en BSS).
L'initialisation est intégrée dans `game_run_combat`, sans routine dédiée distincte.

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| États PvP/PvE/Mystic/Valley traduits | ✅ Fidèle |
| Fondu palette → noir au démarrage | ⚠️ Absent — pas de fondu en C |
| Lecture joystick via handler ISR (`LAB_0575`) | ⚠️ Simplifié — polling direct dans la boucle |
| Tables d'animation initialisées (`LAB_0588`) | ⚠️ Tables de frames codées en dur dans les struct C |
| État `9` (Temple) géré en combat   | ❌ Absent du C — `game_run_combat` ne couvre pas le temple |
| État `5` (Arène en ville)          | ❌ Absent du C — pas de branche `node_type == 5` |

---

## 2. Positionnement initial des combattants

### Code ASM original

**Routine `LAB_04E1` (mog.asm, lignes 10679–10721)**

`LAB_04E1` lit `54(A1)` (type du chevalier) et sélectionne les coordonnées
initiales du curseur de menu dans la table `LAB_09F1` selon une table de cas :

```asm
LAB_04E2:
    CMPI.L  #$00000000,54(A1)   ; type 0 ?
    BNE.S   LAB_04E3
    MOVE.W  #$003f,(A0)+        ; X = 0x3F
    MOVE.W  #$0028,(A0)+        ; Y = 0x28
    BRA.S   LAB_04E7
LAB_04E3:
    CMPI.L  #$00000001,54(A1)   ; type 1 ?
    ...
    MOVE.W  #$0fb0,(A0)+        ; X = 0x0FB0 (overworld)
    MOVE.W  #$0b60,(A0)+        ; Y = 0x0B60
    ...
LAB_04E4:
    CMPI.L  #$00000003,54(A1)   ; type 3 ?
    ...
    MOVE.W  #$0f00,(A0)+        ; X = 0x0F00
    MOVE.W  #$0800,(A0)+        ; Y = 0x0800
LAB_04E5:
    CMPI.L  #$00000002,54(A1)   ; type 2 ?
    ...
    MOVE.W  #$04c3,(A0)+        ; X = 0x04C3
    MOVE.W  #$0160,(A0)+        ; Y = 0x0160
```

Ces coordonnées sont celles du **curseur overworld**, pas des positions
de sprite dans l'arène de combat. Le positionement de combat réel est géré
par les structures de sprites/tiles via `LAB_0451` (détection de collision).

`LAB_04E7–LAB_04E9` (lignes 10710–10721) : boucle DBF pour initialiser
le joueur ET l'adversaire si `LAB_068F` est 1, 8 ou 11 (combat PvP).

### Code C réimplémenté

```c
// Position initiale du joueur (moon_combat.c, ligne 641)
player.x = 80;
player.y = COMBAT_GROUND_Y;   // 150

// Position initiale des ennemis (spawn_enemy, ligne 277)
e->x = 220 + spawn_idx * 20;
e->y = COMBAT_GROUND_Y;
```

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| Joueur côté gauche, ennemi côté droit | ✅ Fidèle (principe) |
| Coordonnées numériques exactes     | ⚠️ Valeurs inventées — l'ASM initialise le curseur overworld (ex : X=0x3F, Y=0x28), pas des positions d'arène directes |
| Étalement des ennemis (`+spawn_idx * 20`) | ❌ Absent — l'ASM positionne un seul adversaire à la fois |
| `COMBAT_GROUND_Y = 150`            | ⚠️ Valeur approximative, non vérifiée dans l'ASM |

---

## 3. Boucle de jeu principale

### Code ASM original

**Boucle `LAB_04D0` (mog.asm, lignes 10558–10574)**

```asm
LAB_04D0:
    MOVEQ   #0,D0
    MOVEQ   #0,D1
    MOVE.W  LAB_097F,D0         ; X curseur
    MOVE.W  LAB_0980,D1         ; Y curseur
    JSR     LAB_0451             ; détecter collision/action (sprite hit ?)
    TST.L   D0
    BEQ.S   LAB_04D1             ; aucun événement → frame suivante
    TST.W   LAB_0981             ; input verrouillé ?
    BNE.S   LAB_04D1
    BSR.W   LAB_052A             ; évaluer la fin du combat
    TST.W   LAB_0984             ; combat terminé ?
    BNE.S   LAB_04D2             ; oui → sortie
LAB_04D1:
    JSR     LAB_0416             ; afficher sprites (blit frame)
    JSR     LAB_039E             ; sync VBL (attendre interruption VBL)
    BRA.S   LAB_04D0             ; reboucler
```

La boucle tourne au rythme VBL (50 Hz PAL). Le rendu est un blit de sprites
sur le double-buffer chip memory. L'action est déclenchée uniquement quand
un sprite du joueur entre en collision avec un sprite ennemi (`LAB_0451`).

### Code C réimplémenté

```c
while (ctx->state == STATE_COMBAT) {
    hal_poll(&ctx->input);         // lire clavier/joystick
    // ... logique joueur + IA ...
    hal_present(ctx->fb);          // afficher le framebuffer
    hal_vbl_wait();                // sync VBL
}
```

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| Sync VBL à chaque frame            | ✅ Fidèle (`hal_vbl_wait`) |
| Rendu par blit sprite              | ⚠️ Rendu par `render_cel_frame` (différent du blitter 68000) |
| Fin de combat via `LAB_052A`       | ⚠️ Fidèle en esprit, mais logique déplacée dans le corps de la boucle C |
| Double-buffer chip memory          | ❌ Absent — le C utilise un seul framebuffer logiciel |
| Verrouillage d'input `LAB_0981`    | ✅ Fidèle — le C détecte le rising edge du bouton fire (`prev_fire`) |
| Boucle d'événements Blitter Amiga  | ❌ Absent — remplacé par `hal_poll` générique |

---

## 4. Gestion des inputs joystick

### Code ASM original

**Activation : `LAB_0575` (mog.asm, lignes 12020–12046)**

```asm
LAB_0575:
    TST.W   LAB_097C             ; déjà actif ?
    BNE.W   LAB_057A
    MOVE.W  #$0001,LAB_097C      ; marquer actif
    JSR     LAB_0E75             ; activer le hardware joystick Amiga
    MOVEA.L LAB_097D,A0
    MOVEA.L LAB_097E,A2
    LEA     EXT_0023,A1
    JSR     LAB_0E77             ; attacher le handler VBL
    ...
    MOVE.L  #LAB_057D,-4(A0)     ; installer handler dans table VBL
    MOVE.L  A0,LAB_0982
```

**Handler VBL : `LAB_057D` (mog.asm, lignes 12058–12106)**

Lit le hardware joystick Amiga (`LAB_00EE`) et met à jour les positions :

```asm
LAB_057D:
    MOVE.W  #$0001,LAB_0981
    JSR     LAB_00EE             ; lire joystick hardware
    MOVEA.L LAB_068B,A0
    CMPI.B  #$01,11(A0)          ; le chevalier fait-il face à gauche ?
    BNE.S   LAB_057E
    MOVE.W  D0,D1                ; si oui : utiliser D0 tel quel (non inversé)
LAB_057E:
    BTST    #0,D1                ; bit 0 = droite → +2 sur X
    BEQ.S   LAB_057F
    ADDI.W  #$0002,LAB_097F
LAB_057F:
    BTST    #1,D1                ; bit 1 = gauche → -2 sur X
    ...
    BTST    #2,D1                ; bit 2 = bas → +2 sur Y
    ...
    BTST    #3,D1                ; bit 3 = haut → -2 sur Y
    ...
    BTST    #4,D1                ; bit 4 = fire → LAB_0981 = 0
    BEQ.S   LAB_0583
    MOVE.W  #$0000,LAB_0981      ; déverrouiller input
```

Limites de position (lignes 12086–12100) :
- X : `[0x0000, 0x013A]`
- Y : `[0x0000, 0x00C3]`

Vitesse de déplacement : **±2 par tick VBL**.

Inversion horizontale selon orientation du chevalier (`11(A0) == 1`).

### Code C réimplémenté

```c
// lecture directe des flags d'entrée (moon_combat.c, lignes 823–827)
int joy_up    = ctx->input.joy[0].up    || keys[82];
int joy_down  = ctx->input.joy[0].down  || keys[81];
int joy_left  = ctx->input.joy[0].left  || keys[80];
int joy_right = ctx->input.joy[0].right || keys[79];
int cur_fire  = ctx->input.joy[0].fire  || ctx->input.space;

// rising edge fire (équivalent LAB_0981 déverrouillage)
if (!prev_fire) { ... attaque ... }

// déplacement
if (joy_left)  { player.x -= MOVE_SPEED; player.facing = -1; }
if (joy_right) { player.x += MOVE_SPEED; player.facing =  1; }
if (joy_up)    { player.y -= MOVE_SPEED; }
if (joy_down)  { player.y += MOVE_SPEED; }
```

Inversion des axes selon `facing` délégué à `decode_attack()`.

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| Vitesse ±2 par tick                | ✅ Fidèle (`MOVE_SPEED = 2`) |
| Inversion axe horizontal selon facing | ✅ Fidèle (`decode_attack`, lignes 544–545) |
| Rising edge du bouton fire (LAB_0981) | ✅ Fidèle (`!prev_fire`) |
| Handler VBL installé en ISR       | ❌ Absent — le C utilise un polling bloquant |
| Lecture hardware registres Amiga  | ❌ Absent — remplacé par `hal_poll` |
| Limites X `[0, 0x13A]`, Y `[0, 0xC3]` | ⚠️ Différentes — le C utilise `ARENA_Y_MIN=90`, `ARENA_Y_MAX=170`, `GAME_W` pour X |
| Support clavier en plus du joystick | ⚠️ Ajout — l'ASM ne lit pas le clavier dans la boucle de combat |

---

## 5. Animation des sprites / FSM de combat

### Code ASM original

**Initialisation des tables : `LAB_0588` (mog.asm, lignes 12107–12287)**

`LAB_0588` initialise six tables de 27 pointeurs longs chacune (`LAB_0692`
à `LAB_0698`). Ces tables contiennent des **pointeurs vers des chaînes
d'animation en mémoire** (pointeurs de frames dans les fichiers `.cel`/`.ob`).

**Sélection d'animation :** la routine `LAB_058A` (lignes 12288–12419) sélectionne
la table active selon `LAB_068F` et peuple `LAB_0699` (table de 27 tuiles) pour
le rendu.

**Affichage d'un sprite : `LAB_051A` / `LAB_0516` (lignes 11226–11275)**

```asm
LAB_051A:
    MOVEM.L D0-D7/A0-A6,-(A7)
    LEA     LAB_0A58,A1           ; structure de sprite
    MOVEA.L LAB_0986,A0
    ; calcul offset dans table frames
    MOVE.W  LAB_0681,D0           ; index de frame
    LSL.W   #3,D0                 ; ×8
    LSL.W   #1,D1
    ADD.W   D1,D0                 ; D0 = index × 10
    MOVE.W  14(A0,D0.W),4(A1)     ; largeur sprite
    MOVE.W  16(A0,D0.W),6(A1)     ; hauteur sprite
    ...
    JSR     LAB_0448               ; blitter le sprite
```

La FSM de l'adversaire est encodée dans les tables de données (`LAB_04F3`,
`LAB_04F4`, lignes 10776–10812) : séquences de tuples
`(frame_id, durée, flags, Y_offset)` terminées par `$FFFF`.

**Remarque — `LAB_04F8` n'est PAS un HUD pendant le combat.**
Cette routine (ligne 10823) affiche les statistiques du chevalier (nom, skill,
items, or, armure, épée). Elle est appelée dans `LAB_04D4` mais **uniquement
dans le contexte de l'écran de butin post-combat** (state 2 réentré via
`LAB_005D` après la victoire) et dans l'écran d'inventaire/boutique.
Elle n'est jamais appelée pendant la boucle de combat active.
**Il n'y a aucun HUD affiché pendant le combat dans l'original.**

### Code C réimplémenté

```c
// Tables de frames par état (moon_combat.c, lignes 314–332)
static const FrameRange s_knight_ranges[] = {
    /* CSTATE_IDLE    */ { 0,  6 },
    /* CSTATE_WALK    */ { 6,  6 },
    /* CSTATE_ATTACK  */ { 12, 6 },
    /* CSTATE_BLOCK   */ { 21, 3 },
    /* CSTATE_HIT     */ { 24, 4 },
    /* CSTATE_STAGGER */ { 24, 4 },
    /* CSTATE_DEAD    */ { 28, 5 },
};
```

L'animation avance via `s_anim_tick` / `s_anim_frame` à une vitesse de
4 ticks par frame (`COMBAT_ANIM_SPEED = 4`).

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| FSM basée sur des tables de tuples | ⚠️ Simplifié — le C utilise des ranges de frames indexées par état C |
| Blitter Amiga (DMA bitplane)       | ❌ Absent — remplacé par `render_cel_frame` logiciel |
| Tables d'animation par type de combat | ❌ Absent — le C a une seule table commune pour tous types |
| Vitesse d'animation (ticks par frame) | ⚠️ Non vérifiable sans timing précis de l'original |
| Affichage portrait/stats en combat | ⚠️ Partiellement — `draw_combatant_cel` dessine le sprite mais pas le portrait |
| Oscillation "stagger" basse HP     | ✅ Présent (±2 px oscillation via `stagger_tick`) |

---

## 6. Système de dégâts / détection de collision

### Code ASM original

**Routine `LAB_052A` (mog.asm, lignes 11482–11630)**

La détection de collision repose sur le **test de superposition de sprites**
via `LAB_0451` (comparaison des positions `LAB_097F`/`LAB_0980` du curseur
avec les hitboxes enregistrées dans la table de sprites de la chip memory).

Quand une collision est détectée, `LAB_052A` lit `16(A0)` (type d'action)
et `8(A0)` (pointeur de frame) pour déterminer le type d'interaction :

```asm
LAB_052A:
    CMPI.L  #$00000007,16(A0)    ; action "fin de combat" ?
    BNE.S   LAB_052B
    MOVE.W  #$0001,LAB_0984      ; déclencher fin de combat
    RTS
LAB_052B:
    CMPI.L  #LAB_09EF,8(A0)      ; frame "mort" ?
    BNE.S   LAB_052C
    BSR.W   LAB_0528             ; changer d'adversaire actif
    BSR.W   LAB_04D4             ; réinitialiser les combattants
    RTS
```

Les dégâts réels sont encodés **dans les données de frame** de l'adversaire
(`offset 0x12 = offset de dégâts dans la structure`). La routine `LAB_052D`
(ligne 11516) gère le cas principal (vol d'item / réduction stats) selon
l'item touché.

**Structure de dégâts dans `LAB_052C` (lignes 11493–11515) :**

```asm
LAB_052C:
    MOVEA.L A0,A2
    MOVEA.L 8(A2),A3             ; frame courante de l'adversaire
    MOVE.W  8(A3),D0             ; type de frame
    MOVE.W  22(A2),D1            ; index slot d'item
    MOVE.W  20(A2),D2            ; flags
    MOVE.W  D2,D3
    ANDI.W  #$000f,D2            ; low nibble = type d'action
    ANDI.W  #$00f0,D3
    LSR.W   #4,D3                ; high nibble = flags
    ; dispatch selon D2 (type d'action)
    CMP.W   #$0005,D2 → LAB_052D (item "spell")
    CMP.W   #$0001,D2 → LAB_053F (item "transfer")
    CMP.W   #$0003,D2 → LAB_0544 (item "buy")
    ...
```

### Code C réimplémenté

```c
// check_hit — distance euclidienne simplifiée (moon_combat.c, ligne 442)
static int check_hit(const Combatant *attacker, const Combatant *defender)
{
    int ax = attacker->x + fwd_dir * reach;
    int dx = defender->x - ax;
    int dy = defender->y - attacker->y;
    return (dx > -reach && dx < reach && dy > y_lo && dy < y_hi);
}

// Résolution des dégâts par type d'attaque (ligne 496)
static int resolve_attack_damage(AttackType t) {
    switch (t) {
    case ATTACK_AXE:      return DMG_AXE;      // 20
    case ATTACK_FORWARD:  return DMG_FORWARD;  // 12
    ...
    }
}
```

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| Collision par superposition sprite Amiga | ❌ Absent — remplacé par test de distance X/Y |
| Dégâts encodés dans les frames    | ❌ Absent — le C utilise des constantes fixes par type d'attaque |
| Système de vol d'items à la victoire | ❌ Absent du code de combat C (géré après victoire seulement) |
| Blocage d'attaque (CSTATE_BLOCK)  | ✅ Présent |
| Dégâts de l'axe = 2× base        | ✅ Fidèle (`DMG_AXE = 20`, soit 2× `DMG_SWING = 10`) |
| Hitbox Y variable selon type d'attaque | ✅ Présent (`y_hi = 60` pour la hache) |
| Multi-frames de contact (durée d'attaque) | ⚠️ Simplifié — le C n'applique les dégâts qu'une fois par activation |

---

## 7. Fin de combat / transition

### Code ASM original

**Conditions de victoire : `LAB_052A` + `LAB_04D2` (lignes 11482–10586)**

La fin de combat est déclenchée quand `LAB_0984 != 0` (flag positionné par
`LAB_052A`). Les conditions sont :

- **Action 7** dans la frame courante → fin immédiate (`LAB_052A`, ligne 11484)
- **Frame `LAB_09EF`** (mort de l'adversaire) → changement d'adversaire ou victoire
- **HP ≤ 0** (chevalier joueur) → mort du joueur (`LAB_052F`, ligne 11548)

Après `LAB_04D2` :

```asm
LAB_04D2:
    JSR     LAB_03F0             ; fondu → noir
    JSR     LAB_057B             ; désactiver joystick
    TST.W   LAB_053C             ; transition armurier/taverne ?
    BEQ.S   LAB_04D3
    LEA     LAB_0617,A0
    LEA     LAB_05E4,A1
    MOVE.L  4(A1),100(A0)        ; stocker destination
    MOVE.W  #$0000,LAB_053C
LAB_04D3:
    MOVE.W  #$0000,LAB_0D05
    RTS
```

La mort du chevalier joueur (`LAB_052F`) :
- Copie `84(A0)` (HP max) → `80(A0)` (HP courant) : **restaure les HP**
- Incrémente `73(A0)` (skill) si HP max en sortie

### Code C réimplémenté

```c
// Mort du joueur (moon_combat.c, ligne 1068)
if (player.state == CSTATE_DEAD) {
    pk->hp   = 0;
    pk->dead = 1;
    pk->gold /= 2;   // ← pénalité de gold
} else {
    // Victoire
    pk->hp    = player.hp;
    pk->gold += 20;
    pk->xp   += 50;
}
```

### Différences

| Point                              | Statut |
|------------------------------------|--------|
| Fondu → noir à la fin              | ❌ Absent en C |
| Restauration HP à la mort          | ❌ Absent — le C met `pk->hp = 0` et `pk->dead = 1` |
| Progression skill si victoire nette | ❌ Absent — le C ajoute `+50 XP` (concept inexistant dans l'ASM) |
| Perte de gold à la mort (`/= 2`)   | ❌ Inventé — l'ASM ne divise pas le gold |
| Transition spéciale armurier/taverne (`LAB_053C`) | ❌ Absent en C |
| `STATE_VALLEY` après Vallée des Dieux | ✅ Présent — `ctx->state = STATE_VALLEY` |
| Réinitialisation à mort (`LAB_04CF` + état 8) | ❌ Absent — le C sort directement vers l'overworld |

---

## 8. Écran de butin / inventaire post-combat — absence de HUD pendant le combat

### Principe fondamental

> **Il n'y a aucun HUD pendant le combat dans `mog.asm`.**  
> Ni barre de HP, ni affichage de skill, ni indicateur d'or ne sont
> superposés à l'action de combat. L'écran de combat est entièrement
> réservé aux sprites des combattants et au décor.

### Code ASM original — `LAB_04F8` (mog.asm, lignes 10823–11006)

`LAB_04F8` est la routine d'**affichage des statistiques du chevalier**
pour l'**écran de butin et d'inventaire post-combat**.

**Quand est-elle appelée ?**

Après la fin du combat, `LAB_005D` ré-entre dans la machine à états avec
`D0 = 2` via `JSR LAB_04CF`. Cette ré-entrée dans `LAB_04D4` (via état 2)
déclenche `BSR.W LAB_04F8` dans le contexte de l'**écran de butin**, où
le joueur peut consulter les items de la créature vaincue et les prendre.
C'est ici, et seulement ici, que les stats du chevalier sont affichées.

La même routine sert aussi pour les états 1 (PvP), 8 et 11 (duels), et
l'écran d'inventaire/boutique (état 6).

**Ce qu'affiche `LAB_04F8` :**

- Nom du chevalier (offsets `70`, `72`, `71` dans la structure chevalier)
- Level/Skill (`73(A0)`)
- Items portés (boucle sur offsets `0x46..0x48` de la structure)
- Or (`78(A0)`) et kills/or-gain (`74(A0)`)
- Potions (`76(A0)`)
- Niveau d'armure (`88(A0)`) et niveau d'épée (`92(A0)`) — affichés comme
  longueur de sprite proportionnelle

**→ Aucune barre de HP n'est dessinée.** La santé (`80(A0)` / `84(A0)`)
est une variable interne jamais affichée sous forme de barre, ni pendant
le combat, ni dans l'écran de butin.

### Code C réimplémenté

`moon_combat.c` ne dessine aucune barre de HP (conforme) et n'implémente
pas l'écran de butin/inventaire post-combat (qui appartient à la transition
vers l'overworld, hors périmètre du fichier). Il n'y a donc aucun équivalent
de `LAB_04F8` dans le C actuel.

Les seuls indicateurs visuels de santé dans le C sont des ajouts non
présents dans l'original :
1. Le texte `"STAGGERING!"` quand `player.hp <= LOW_HP_THRESHOLD`
2. L'oscillation visuelle `stagger_tick` (wobble ±2 px)

### Différences

| Point                                         | Statut |
|-----------------------------------------------|--------|
| Barres de HP dans l'ASM                       | ❌ **N'existent pas** dans `mog.asm` |
| Barres de HP dans le C                        | ✅ **Absentes aussi** — conforme |
| HUD pendant le combat                        | ❌ **N'existe pas** dans `mog.asm` |
| Écran de butin (skill/items/or) après combat | ❌ Absent du C (non implémenté) |
| Affichage armure/épée (écran de butin)       | ❌ Absent du C |
| Texte "STAGGERING!" basse HP                 | ⚠️ Ajout non-original — aucun équivalent dans l'ASM |

---

## 9. Tableau récapitulatif

| Fonctionnalité                           | ASM original           | Code C               | Statut |
|------------------------------------------|------------------------|----------------------|--------|
| Machine à états `LAB_068F`               | `LAB_04CF` (l.10547)   | `ctx->node_type`     | ✅ Fidèle |
| Fondu palette → noir                     | `LAB_03F0`             | —                    | ❌ Absent |
| Initialisation joystick ISR              | `LAB_0575` (l.12020)   | `hal_poll` polling   | ⚠️ Simplifié |
| Tables d'animation sprites               | `LAB_0588` (l.12107)   | `s_knight_ranges[]`  | ⚠️ Simplifié |
| Positionnement curseur par type          | `LAB_04E1` (l.10679)   | constantes `x=80`    | ⚠️ Approximatif |
| Boucle VBL                               | `LAB_04D0` + `LAB_039E`| `hal_vbl_wait()`     | ✅ Fidèle |
| Vitesse déplacement ±2                   | `LAB_057D` (l.12058)   | `MOVE_SPEED=2`       | ✅ Fidèle |
| Inversion axe selon facing               | `LAB_057D` (l.12062)   | `decode_attack()`    | ✅ Fidèle |
| Rising edge fire (anti-répétition)       | `LAB_0981`             | `!prev_fire`         | ✅ Fidèle |
| Limites déplacement X/Y                  | X:0–0x13A, Y:0–0xC3    | `ARENA_Y_MIN/MAX`    | ⚠️ Valeurs différentes |
| Détection collision par sprite           | `LAB_0451`             | distance X/Y         | ❌ Différent |
| Dégâts encodés dans frames               | `offset 0x12` frame    | constantes `DMG_*`   | ❌ Simplifié |
| Blocage d'attaque                        | flags frame `LAB_052C` | `CSTATE_BLOCK`       | ✅ Présent |
| Dégâts axe = 2× base                    | implicite dans frames  | `DMG_AXE=20`         | ✅ Fidèle |
| Mort joueur → restaure HP                | `LAB_052F` (l.11548)   | `pk->hp=0; dead=1`   | ❌ Comportement différent |
| Fin de combat → fondu + transition       | `LAB_04D2` (l.10575)   | `goto combat_cleanup`| ⚠️ Simplifié |
| Écran de butin/inventaire post-combat (skill/or/items) | `LAB_04F8` (l.10823) | —                    | ❌ Non implémenté |
| Barres de HP                             | **N'existent PAS**     | **N'existent PAS**   | ✅ Conforme |
| État arène en ville (5)                  | `LAB_0522`             | —                    | ❌ Absent |
| État temple de soin (9)                  | `LAB_0591`             | —                    | ❌ Absent |
| État Mystic (3) en combat               | `LAB_058F`             | —                    | ❌ Absent |
| Stagger oscillation basse HP             | —                      | `stagger_tick`       | ⚠️ Ajout C |
| IA enemis (waves PvE)                    | `LAB_01B7`, `LAB_08C5` | `ai_update()`        | ⚠️ Simplifié |
| Progression skill à victoire             | `LAB_052F` HP check    | `pk->xp += 50`       | ❌ Mécanisme différent |
| Perte gold à la mort                     | —                      | `pk->gold /= 2`      | ❌ Inventé |
| Support clavier pendant combat           | —                      | `keys[79..82]`       | ⚠️ Ajout C |

---

## 10. Écarts non conformes

Cette section liste les comportements présents dans `moon_combat.c` qui
**n'ont pas de base dans `mog.asm`**, ou dont le comportement est sensiblement
différent de l'original.

### 10.1 Perte de gold à la mort (`pk->gold /= 2`)

**Code C (ligne 1071) :**
```c
pk->gold /= 2;
```
**Dans l'ASM :** La routine de mort `LAB_000E` / `LAB_052F` ne divise pas le
gold du chevalier. La mort entraîne la restauration des HP et la perte d'un
niveau de skill, pas une pénalité monétaire.

**→ Comportement inventé, non présent dans `mog.asm`.**

### 10.2 Gain d'XP à la victoire (`pk->xp += 50`)

**Code C (ligne 1077) :**
```c
pk->xp += 50;
```
**Dans l'ASM :** Le jeu original n'a pas de système d'XP numérique explicite.
La progression se fait via l'incrémentation du skill (`73(A0)`) sous conditions
précises (`LAB_052F`), et via l'acquisition d'items.

**→ Variable `xp` et valeur `+50` inventées, absentes de `mog.asm`.**

### 10.3 Texte "STAGGERING!" à l'écran en basse HP

**Code C (ligne 1027) :**
```c
render_text_centered(ctx->fb, "STAGGERING!", 28, 0xFFFF8800u);
```
**Dans l'ASM :** L'écran de butin post-combat (`LAB_04F8`) n'affiche aucun
message textuel en cas de basse HP. L'effet "vacille" est purement visuel
(oscillation sprite). Il n'y a pas d'annotation textuelle, ni pendant le
combat, ni dans l'écran de butin.

**→ Message textuel ajouté, absent de l'original.**

### 10.4 Texte "LEFT: X/Y" compteur de kills PvE

**Code C (lignes 1016–1023) :**
```c
snprintf(wave_buf, sizeof(wave_buf), "LEFT: %d/%d",
         enemies_killed, enemies_total);
render_text(ctx->fb, wave_buf, 10, 24, 0xFFFFDD88u);
```
**Dans l'ASM :** Aucun compteur textuel de kills n'est affiché pendant le
combat. La progression est encodée dans des variables internes.

**→ Compteur de kills textuel inventé, absent de `mog.asm`.**

### 10.5 Texte nom de l'attaque à l'écran

**Code C (lignes 1030–1039) :**
```c
render_text_centered(ctx->fb, attack_names[idx], GAME_H - 20, 0xFFFFDD44u);
```
**Dans l'ASM :** Le nom de l'attaque n'est jamais affiché en temps réel
pendant le combat.

**→ Label textuel d'attaque inventé, absent de `mog.asm`.**

### 10.6 Messages texte "YOU DIED" / "VICTORY!"

**Code C (lignes 1043–1048) :**
```c
render_text_centered(ctx->fb, "YOU DIED", GAME_H / 2, 0xFFFF2222u);
render_text_centered(ctx->fb, "VICTORY!", GAME_H / 2, 0xFF44FF44u);
```
**Dans l'ASM :** La fin de combat retourne directement à `LAB_04D2` (fondu
et transition). Aucun message textuel "YOU DIED" ou "VICTORY!" n'est affiché
dans `mog.asm` — le retour au menu ou à l'overworld est immédiat.

**→ Messages de fin inventés, absents de `mog.asm`.**

### 10.7 Mort du joueur : comportement différent

**Code C :**
```c
pk->hp   = 0;
pk->dead = 1;
```
**Dans l'ASM (`LAB_052F`, ligne 11548) :**
```asm
; Si hp_current == hp_max → incrémenter skill
MOVE.W  84(A0),80(A0)    ; restaurer HP (HP courant = HP max)
```
La mort dans l'original **restaure les HP** du chevalier et décrémente son
skill de 1 (via `LAB_000E`). Le C marque le chevalier comme mort et lui
laisse 0 HP.

**→ Mécanisme de mort fondamentalement différent.**

### 10.8 Etats de combat non couverts

Les états suivants existent dans `mog.asm` mais **n'ont aucun équivalent**
dans `game_run_combat` :

| État ASM (`LAB_068F`) | Description                  | Absent du C |
|-----------------------|------------------------------|-------------|
| `3` (Mystic)          | Sorcier Mythral              | ❌ |
| `5` (Arena)           | Arène en ville               | ❌ |
| `9` (Temple)          | Temple de soin               | ❌ |
| `6` (Shop)            | Boutique/inventaire          | ❌ |
| `8` (PvP rematch)     | Duel rejoué                  | ❌ |

---

*Fin du document — `DOC_COMPARAISON_ASM_C.md`*
