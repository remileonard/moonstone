# Méthode de portage de Moonstone en C : émulateur de référence et comparaison

Ce document explique **comment** Moonstone est porté en C. Il décrit aussi
comment chaque morceau est vérifié contre l'original : mémoire et écrans
comparés octet par octet.

Le jeu original est fait de deux exécutables :

- `program` : intro, et séquence de fin après une victoire (§8 bis) ;
- `mog` : menu, choix des chevaliers et partie (carte, lieux, écrans,
  combats) ; c'est l'essentiel du jeu.

Les deux sont portés, et c'est désormais **le seul jeu** du dépôt :
l'ancienne réimplémentation non fidèle (`moon_intro.c`, `moon_menu.c`,
`moon_overworld.c`, `moon_town.c`...) a été supprimée (§10).

Le contenu du moteur lui-même (boucle de combat, scripts, entités) est
décrit dans `DOC_MOTEUR_COMBAT_MOG.md`.

---

## 1. Principe : un portage fidèle, sur la mémoire de l'original

On ne réécrit pas « un jeu qui ressemble » : on traduit **routine par
routine** le code 68000 de `amiga_asm/mog.asm` en C.

**La mémoire de `mog` est gardée telle quelle.** Le C travaille sur une
mémoire virtuelle (`IxVM`, `game/include/ix_vm.h`) où se trouvent :

- le programme chargé avec ses relocations (`game/data/ix_mog.c`) ;
- ses données, aux **mêmes adresses** et au **même format** que sur l'Amiga.

Chaque variable de l'original (`LAB_0633`, `v_Combatants`,
`SECSTRT_21`...) est lue et écrite à son adresse, par `ix_rb/rw/rl` et
`ix_wb/ww/wl` (octet, mot, long, gros-boutiste). Les adresses viennent des
symboles de l'original (`ix_mog_syms.h`, `MOG_LAB_xxxx`).

**Une routine = une fonction C.** Chaque fonction porte en commentaire le
nom de sa routine d'origine :

```c
/* LAB_0E3D : feu sur un ou plusieurs lieux ... */
static int places_fire(MogCombat *m)
```

Le C garde l'ordre des lectures et écritures, les comparaisons signées ou
non, les tailles (`TST.W` sur un pointeur ne regarde que le mot bas), et
même **les bogues** (voir §6).

**Pourquoi garder la mémoire d'origine ?** Parce que c'est ce qui rend la
comparaison possible : à tout instant, la mémoire du C doit être
**identique** à celle de l'original. Une différence d'un seul octet est
détectée, située (par symbole) et corrigée.

**Ce qui n'est pas porté tel quel : le matériel de l'Amiga.**

| Matériel | Dans le C |
|---|---|
| Blitter | émulé : `mog_blit.c` |
| Interruption VBL | portée : couleurs `mog_vbl.c`, pointeur `mog_screen_vbl` |
| Copper | l'écran affiché est lu dans ses pointeurs de plans |
| Joystick, clavier | fournis par l'hôte |
| Disquette | accès fichier direct |

L'hôte (SDL : `moon_mog.c`) ne fait qu'une chose, la VBL :

1. présenter l'image ;
2. attendre 1/50 s ;
3. lire les entrées.

---

## 2. Le banc de référence : l'original dans un émulateur 68000

`tools/mog_ref.py` exécute **le vrai code 68000 de `mog`** avec
l'émulateur de processeur **Unicorn**, sur les vraies données du jeu.

C'est un « mini-émulateur Amiga » : seul le processeur est émulé
complètement. Le matériel est remplacé par des crochets écrits en Python.

### 2.1 Mémoire

```
0x000000-0x0FFFFF   « chip » : bloc donné par le lanceur (LAB_05BC)
0x100000-...        mog : hunks chargés, relocations appliquées
ensuite             bloc « fast » du lanceur, puis la pile de l'émulateur
0xBFD000, 0xDFF000  CIA et registres custom : simple RAM
```

- Le programme est chargé au même endroit que dans le C (même
  `VM_BASE`). Les deux mémoires sont donc **directement comparables**,
  octet à octet.
- Au démarrage, `boot()` exécute `SECSTRT_0` de l'original jusqu'à
  `LAB_0001` (chargements, tables), puis `LAB_0152` / `LAB_0156`.

### 2.2 Crochets : le matériel remplacé

Pour faire un crochet, on écrit `RTS` au début de la routine et on appelle
une fonction Python à sa place (`MogRef.hook`).

| Original | Remplacé par |
|---|---|
| `File_Open` / `File_Read` / `File_Close` | lecture du dossier de données (plus de disquette, plus de DOS) |
| `LAB_0100` (changement de disquette) | rien : tout est présent |
| `LAB_00EE` (joysticks) | renvoie `ref.joy` (piloté par le test) |
| `LAB_0D77` (attente d'une VBL) | compteur `v_VblCounter` + serveur du pointeur (§2.4) |
| `LAB_0AA2`, `LAB_0F8C` (sons) | notés dans `events` |
| `LAB_0BB3` (traces du jeu) | notées dans `messages` |

### 2.3 Le blitter émulé

Les routines de dessin de l'original (`LAB_0CDA`, copies d'écran
`LAB_0419`...) **s'exécutent vraiment**. Elles programment le blitter, et
c'est l'écriture de `BLTSIZE` (`$DFF058`) qui lance l'opération.

Le banc intercepte cette écriture (`UC_HOOK_MEM_WRITE`). La classe
`Blitter` effectue alors la copie, comme le matériel :

- canaux A, B, C, D ;
- décalages de A et B ;
- masques du premier et du dernier mot ;
- modulos ;
- minterm ;
- mode descendant ;
- mots précédents de A et B (barillet) conservés d'un blit à l'autre.

Le **même modèle** est écrit en C (`mog_blit.c`, `mog_blitter_run`).
Résultat : les deux côtés dessinent dans les **plans de bits en mémoire**.
Les écrans font donc partie de la mémoire comparée (§4).

### 2.4 Le temps : VBL et attentes actives

Sur l'Amiga, l'interruption VBL tourne 50 fois par seconde, quoi que fasse
le programme. Le banc n'a pas d'interruptions. On fixe donc une règle
**identique des deux côtés** :

**Une VBL = un appel à `LAB_0D77`.**
- Côté banc, `LAB_0D77` saute vers un petit code 68000 écrit en mémoire.
  Ce code :
  1. sauve les registres ;
  2. appelle le **vrai** `LAB_057D` (déplacement du pointeur au joystick)
     si le pointeur est actif ;
  3. restaure les registres.
- Côté C, c'est `mog_wait_vbls` → `mog_screen_vbl`.

**Les attentes actives sans VBL.** L'original attend parfois en boucle,
sans jamais appeler `LAB_0D77`, pendant que l'interruption fait bouger le
pointeur. C'est le cas dans les menus des villes (`LAB_008C`, `LAB_0095`)
et dans le choix de l'or (`LAB_0496`).
- Côté banc, un **trampoline** fait une VBL réelle à chaque tour : le
  crochet redirige le PC vers `JSR LAB_0D77 ; JMP boucle`.
- Côté C, chaque tour fait `mog_wait_vbls(m, 1)`.

**Les lectures hors mémoire.** Elles lisent des zéros des deux côtés.
Sur Amiga, elles liraient la ROM (bus 24 bits). Le seul cas connu est un
bogue de l'original (§6).

### 2.5 Points de rendez-vous

Pour avancer l'original et le C **pas à pas ensemble**, on définit des
points de rendez-vous où les deux s'arrêtent. À chacun d'eux, les entrées
de l'image suivante sont données aux deux.

| Point | Où |
|---|---|
| `Combat_FrameStart` | chaque image de combat et de carte |
| `LAB_04D0` | chaque tour d'un écran à pointeur (inventaire, boutiques...) |
| `LAB_00EC` / `LAB_00ED` | attente « appuyez sur feu » (appui, relâché) |
| `LAB_0E40` | attente d'une touche (menu des lieux) |
| `LAB_008C`, `LAB_0095`, `LAB_0496`, `LAB_04A9`, `LAB_0457`, `LAB_0458`, `LAB_045D` | boucles des villes, du jeu de dés, de la sorcière |
| `LAB_00B5`, `LAB_00D4`, `LAB_00C9`, `LAB_00CC` | menu du début, choix des chevaliers, saisie du nom (§7 bis) |

- Côté banc : un crochet de code à l'adresse, qui arrête l'émulation.
- Côté C : l'appel `m->frame_start(user)` au même endroit du code.

Ce rappel ne fait rien dans le jeu réel. Dans les attentes actives,
`mog_idle` fait en plus passer une VBL (`m->idle`).

---

## 3. Les outils de comparaison, du plus petit au plus grand

Ils ont été construits dans cet ordre, chacun s'appuyant sur le précédent :

| Outil | Ce qui est comparé |
|---|---|
| `tools/ix_difftest.py` | moteur de scripts IMAGEXCEL : mêmes dessins, sons, CRC de la mémoire, image après image |
| `tools/mog_bootcheck.py` | mémoire préparée au démarrage (`mog_boot.c`), zone par zone |
| `tools/mog_difftest.py` | chaque routine de combat portée, exécutée seule sur une copie de la mémoire, pendant un vrai duel |
| `tools/mog_setupcheck.py` | la préparation des 11 rencontres (`t_CreatureInit`) jusqu'à `Combat_Loop`, écrans compris |
| `tools/mog_drawcheck.py` | `LAB_0CDA` (dessin d'une frame CEL) rejoué des deux côtés, blitter émulé |
| `tools/mog_mapcheck.py` | la carte, image par image |
| **`tools/mog_lockstep.py`** | **le jeu entier**, original et C côte à côte (§4) ; `--menu` : depuis le menu du début (§7 bis) |
| `tools/mog_gamecheck.py` | la nouvelle partie démarrée par le C seul, contre l'original |

Le programme C de test est `build/tests/mog_run`. C'est un processus
persistant qui exécute le même programme que l'original et qui communique
par l'entrée et la sortie standard.

---

## 4. Le « lockstep » : original et C côte à côte, écrans compris

`tools/mog_lockstep.py` est l'outil principal.

1. L'original démarre dans le banc et crée une nouvelle partie à un
   joueur.
2. Sa **mémoire complète** est donnée au processus C (`mog_run`), qui
   repart exactement du même état.
3. Les deux avancent jusqu'au prochain point de rendez-vous. Pour le C :
   - il écrit sa mémoire dans un fichier ;
   - il écrit `F` ;
   - il attend une ligne `J joy0 joy1 [touche]`.
4. **Toute la mémoire est comparée** (hors pile de l'émulateur). Cela
   inclut :
   - les variables du jeu ;
   - les objets et les entités ;
   - **les écrans** (plans de bits), puisque les deux côtés dessinent
     avec le même blitter ;
   - le compteur de VBL.
5. Les mêmes entrées sont données aux deux, et on recommence.

À la moindre différence, l'outil affiche les adresses, le symbole le plus
proche (`LAB_0617+4B`), et les octets de l'original et du C :

```
ÉCHEC image 364 (joy 1) : 2 octets
  00110CC0 LAB_07FC+14   orig 03052200ff70  C 00052200ff70
```

### 4.1 La comparaison d'écran

L'écran de l'Amiga, c'est de la mémoire :
- 5 plans de bits de 8000 octets ;
- pointés par la copper list (`MOG_COPPER_BPL`, `$7F6B0`) ;
- des couleurs tenues par l'interruption VBL (`LAB_0E5D`).

Comme le blitter est émulé **des deux côtés**, un dessin faux d'un seul
pixel se voit comme une différence d'un octet dans un plan. La
comparaison mémoire **est** donc une comparaison d'écran, au pixel près.

Pour regarder une image, `mog_screen` (`mog_vbl.c`) convertit les plans
affichés et la palette en pixels. C'est ce qu'utilisent les captures PNG
de `mog_fight_shot` et `mog_game_shot`, ainsi que le jeu réel.

### 4.2 Piloter les parties

Les entrées sont décidées par le script de test, identiquement pour les
deux côtés :

- joystick au hasard (graine `--seed`), feu au hasard (`--fire`) ;
- barre d'espace périodique (`--space`) ;
- dans les écrans à pointeur, errance puis **pointeur mené vers la
  sortie** (zone d'identifiant connu), puis clic ;
- dans les menus, la touche `1` ; aux attentes du feu, appui puis relâché.

Options de mise en place :
- `--place TYPE` : le chevalier est posé sur un lieu (ville, repaire,
  temple...) ;
- `--inv IDX=VAL` : impose un objet (ex. l'objet 20 requis par le Démon).

Ces changements sont faits **avant** de copier la mémoire, donc des deux
côtés.

Pour enquêter :
- `MOG_SAVE=préfixe` enregistre la mémoire de départ, les entrées et les
  deux mémoires au moment de l'échec ;
- `--replay fichier.joy` rejoue une partie à l'identique ;
- le suivi des VBL (appelants de `LAB_0D77` côté original) montre **où**
  une attente diffère.

### 4.3 Ce que ça a trouvé (exemples)

| Symptôme | Cause |
|---|---|
| pointeur décalé de 2 pixels à l'entrée d'un écran | `LAB_0D8A` (palette) attend une VBL ; le C ne le faisait pas |
| 5 VBL de trop après une barre d'espace en combat | c'est la **pause** de l'original (attente d'une touche) ; le test ne doit pas appuyer sur Espace en combat |
| le sort du Démon modifie un script | bogue de l'original, reproduit (§6) |
| trésors différents en début de partie | le C ne tirait pas la graine du hasard (`LAB_04A5`) |
| icônes de la carte absentes au démarrage C | `LAB_0128` / `LAB_012C` n'étaient pas portés |

---

## 5. Le démarrage en C seul (`tools/mog_gamecheck.py`)

Le lockstep part de la mémoire de l'original. Il ne vérifie donc pas le
démarrage du C. `mog_gamecheck.py` compare :

- la nouvelle partie créée **entièrement en C** : `mog_game_boot`, via
  `mog_run newgame` ;
- la même nouvelle partie dans l'original.

Toute la mémoire est comparée, et les différences sont regroupées par
plage.

Restent différents, sans effet sur le jeu :
- tampons de décompression ;
- écran de présentation non dessiné ;
- musique et sons non chargés en mémoire « chip » ;
- code où le banc a posé ses `RTS`.

---

## 6. Bogues de l'original, reproduits

Le but est d'être identique. Les bogues de l'original sont donc gardés et
documentés dans le code.

**Le Démon (`LAB_0EE1` / `LAB_0EE3`).** À la fin d'une prise, l'original
écrit `106(A0)`. Or `A0` vient d'être chargé avec le script `LAB_07FB`.
L'écriture touche donc l'octet `LAB_07FC+20` d'un script d'animation du
chevalier, pas le Démon.

Le C fait la même chose. Plus tard, ce script abîmé fait lire à
l'original une adresse hors mémoire. Sur Amiga, c'est de la ROM ; dans le
banc et dans le C, c'est zéro.

**D0 au retour des routines.** Le résultat testé est parfois la valeur
laissée dans `D0` par une routine précédente. Par exemple :
- après `SECSTRT_36`, `D0 = $FFFF` (reste du `DBF` de `LAB_0011`), donc
  « la carte est à redessiner » ;
- la barre d'espace dans la ville `$1A` ouvre l'écran de genre
  `touche << 16 | $20`.

Ces valeurs sont reproduites.

---

## 7. Le jeu jouable

`mog_game.c` assemble le tout :

- démarrage (`SECSTRT_0` et ressources) ;
- nouvelle partie (`LAB_01AE`, `LAB_01BE`...) ;
- boucle de la carte (`mog_map_frame`), d'où partent les lieux, les
  écrans et les combats.

Sans écran (outils), la nouvelle partie est à un joueur, sans menu ; avec
l'hôte, le menu de l'original la précède (§7 bis).

À chaque VBL :

1. compteur de VBL ;
2. serveur des couleurs (`LAB_0E5D`) ;
3. entrées : joystick, touches → `SECSTRT_21` / `LAB_0B91` ;
4. image affichée : plans de la copper list et sprite du pointeur ;
5. 1/50 s chez l'hôte.

`moon_mog.c` relie cela à SDL (`moonstone <données> 3`). Il joue d'abord
program (l'intro), puis mog ; après une victoire, program de nouveau (la
fin), puis mog (le menu).

Les entrées de l'hôte :

| Amiga | Clavier / manette |
|---|---|
| joystick, port 1 (`LAB_0630`, les joueurs) | flèches + Espace ou Ctrl, 1re manette |
| joystick, port 0 (`LAB_062F`) | W A S D + F, 2e manette |
| clavier (`SECSTRT_21`, codes de `LAB_0D99`) | lettres, chiffres, Tab (barre d'espace), Entrée, retour arrière |

Le port 0 est à part : dans un duel entre deux humains
(`Combat_StartPvP`) et à l'entraînement, le second chevalier y est mis
(`11(objet) = 1`) ; il le garde ensuite pour son pointeur dans les
écrans (`mog_screen_vbl`).

`tests/mog_game_shot` fait tourner le même jeu **sans écran**. Il est mené
par un script d'entrées (« 150 VBL à droite, touche I... ») et enregistre
des captures PNG : c'est la vérification rapide du branchement.

---

## 7 bis. Le menu du début et le choix des chevaliers

Dans `SECSTRT_0`, après les chargements, `LAB_0001` appelle
`Prot_CopylockCheck` : la protection de la disquette, puis le menu. Porté
dans `mog_menu.c`.

- **La protection Copylock** (Rob Northen) est chiffrée : elle se
  déchiffre elle-même en mode trace et ne peut pas être traduite. Sur une
  disquette d'origine, elle laisse sur la pile la valeur `LAB_029F`, que
  le menu range dans `LAB_0714` (le contrôleur du chevalier humain) ; une
  copie y mettrait autre chose, et le jeu deviendrait injouable. Le C fait
  comme une disquette d'origine.
- **Le menu** (`LAB_00B5`) : ligne `LAB_06DC` 0 « Players » (1 à 4,
  `LAB_05C5`), 1 « Gore » (`LAB_06DA`), 2 « Practice », 3 « Select
  Knight ».
- **Le choix des chevaliers** (`LAB_00D3`) : chaque joueur prend un des
  quatre chevaliers (`LAB_00E5`) et tape son nom (`LAB_00C9` : 13 lettres,
  éclair rouge sur `COLOR00` au-delà, retour arrière, Retour ou feu).
- **L'entraînement** (`LAB_0002` + `LAB_0165`, `mog_practice`) : duel des
  chevaliers 1 (port 1) et 2 (port 0), puis retour à `LAB_0001` et au
  menu.

Vérification : `mog_lockstep.py --menu game|players2|practice`. Le banc
remplace la Copylock par `MOVE.L #LAB_029F,-(A7) ; JMP` menu (le patch est
fait avant la copie de la mémoire, donc des deux côtés), et laisse passer
`LAB_0001` au retour de l'entraînement. Résultats : 400 images identiques
pour une partie à un et à deux joueurs (menu, chevaliers, noms, carte),
1500 pour l'entraînement (duel, retour au menu, second duel).

---

## 8. Le son

Le pilote de sons de l'original est porté comme le reste, sur sa mémoire
(`mog_sound.c`) :

- quatre voies de 148 octets ;
- des programmes d'octets (table `LAB_1098`, 22 codes) ;
- des instruments (`LAB_10A2`), des enveloppes et des vibratos ;
- le serveur de VBL `LAB_0F73` et l'interruption audio `LAB_0F6F`.

La puce **Paula** est émulée :
- registres LC / LEN / PER / VOL verrouillés ;
- DMACON, INTENA, INTREQ ;
- interruption au départ du DMA et à chaque fin de bloc ;
- échantillons 8 bits lus dans la mémoire de mog, horloge PAL.

Le mixage produit 1/50 s de son par VBL, stéréo à la manière de
l'Amiga (voies 0 et 3 à gauche, 1 et 2 à droite).

`tools/mog_sndcheck.py` compare le pilote à l'original, pour chacun des
168 sons :
- les voies, les variables du pilote et les registres Paula ;
- après le départ, puis à chaque VBL ;
- avec l'interruption appelée des deux côtés.

Sans hôte audio (`m->audio` NULL), rien ne joue et la mémoire n'est pas
touchée : le lockstep reste valable.

Trouvé en route : le sprite du pointeur (`LAB_0572`) déborde de 2 octets
du bloc `SECSTRT_43`. Sur l'Amiga, c'est un bloc de mémoire chip à part.
Dans l'image mémoire, les sections se suivent, et ces 2 octets
effaçaient la voie 0 du son. La fin du sprite est donc coupée (C), et le
banc remet la voie en état après le démarrage.

---

## 8 bis. L'intro et la fin (program)

`program` est l'autre exécutable de l'original. Il démarre, puis :

- d'ordinaire, charge les décors, joue le générique et les scènes de
  l'intro, et charge `mog` ;
- si mog a été gagné, joue la fin (`LAB_0001`) : mog, au temple, écrit à
  `$3E0` (`EXT_000e` pour mog, `EXT_0007` pour program) `$80` + un bit du
  chevalier (3 à 6) + un bit du lieu (0 à 2), puis relance program. Les
  couleurs de la fin en dépendent (`LAB_01B9`, `LAB_01BE`).

Il est porté de la même façon, sur sa propre image mémoire
(`prog_boot_memory`, identique à celle du banc).

- Banc : `tools/prog_ref.py`, avec les mêmes crochets que pour mog
  (fichiers, blitter émulé). L'attente d'une VBL (`LAB_0552`) appelle les
  vrais serveurs VBL.
- Code partagé : une partie du code de program est la même que celle de
  mog, à d'autres adresses.
  - `tools/asm_twins.py` apparie les blocs identiques.
  - `tools/prog_twins.py` écrit des en-têtes (`prog_twin_*.h`) qui
    redéfinissent chaque `MOG_x` en `PROGRAM_y`.
  - Ainsi `mog_blit.c`, `mog_gfx.c`, `mog_vbl.c` et `mog_files.c` sont
    recompilés pour program (`prog_*.c`) sans être recopiés.
- Le reste est traduit à la main dans `prog_intro.c` et `prog_music.c` :
  - le moteur d'entités à scripts (et les routines appelées par les
    scripts : éclairs, pulsations, contrôle du défilement) ;
  - le défilement de tuiles, vers le bas (intro) et vers le haut (fin) ;
  - le générique ;
  - le texte ;
  - la décompression RNC (`music.cmp`, `vmusic.cmp`) ;
  - le lecteur de musique (format NoiseTracker).
- Registre A1 : chaque entité garde le registre A1 de son appelant. Le C
  suit donc A1 (`ProgIntro.a1`) pour rester identique octet pour octet.
- Vérification : `tools/prog_lockstep.py --scene intro` compare la
  mémoire VBL par VBL, de `SECSTRT_0` jusqu'au chargement de mog. Le vrai
  lecteur de musique tourne des deux côtés. Résultat : 5118 VBL
  identiques. `--scene 001b` (et les autres scènes) part de l'entrée d'une
  seule scène.
- La fin se vérifie **sans jouer de partie** : `--flags 0x91` écrit les
  drapeaux de mog à `$3E0` avant le départ, des deux côtés. Résultat :
  4125 VBL identiques, pour `$91`, `$A2`, `$C4` et `$8A`.

---

## 9. Commandes

```sh
cmake --build build

# le jeu (fenêtre SDL) : intro, menu, partie, fin
build/game/moonstone <données> 3
# la fin seule, sans partie ; les combats seuls
build/game/moonstone <données> 3 --fin 0x91
build/game/moonstone <données> 3 --combat all

# original et C côte à côte
python3 tools/mog_lockstep.py <données> --frames 2000 --fire 0.15 --wander 60 --place 0x19
python3 tools/mog_lockstep.py <données> --menu players2 --frames 400    # menu, chevaliers
python3 tools/mog_lockstep.py <données> --replay dm.joy --frames 1320   # rejouer

# démarrage C contre original
python3 tools/mog_gamecheck.py <données>

# captures sans écran, script d'entrées
build/tests/mog_game_shot <données> <préfixe> script.txt

# intro : original et C côte à côte ; C seul (images + musique .wav)
python3 tools/prog_lockstep.py <données> --scene intro
# la fin (partie gagnée : EXT_0007 = $80 | chevalier | lieu), sans jouer
python3 tools/prog_lockstep.py <données> --scene intro --flags 0x91
build/tests/prog_intro_shot <données> <préfixe> 100 [0x91]

# libmoon_assets contre les décodeurs du portage, fichier par fichier
build/tests/lib_audit <données>
```

Les fichiers du jeu d'origine ne sont pas dans le dépôt. Le dossier
`<données>` doit les contenir : `kn1.ob`, `test`, `*.PIV`, `*.CEL`, `*.t`,
`collide.hit`...

---

## 10. Carte des fichiers

| Fichiers | Contenu |
|---|---|
| `game/src/ix_vm.c`, `game/data/ix_*.c`, `ix_*_syms.h` | mémoire virtuelle, images de mog et de program, symboles |
| `game/src/ix_engine.c` | moteur de scripts IMAGEXCEL de mog |
| `game/src/mog_*.c` | mog : démarrage (`mog_boot`, `mog_files`, `mog_gfx`), menu (`mog_menu`), carte (`mog_map`), villes (`mog_town`), écrans (`mog_screens`), rencontres (`mog_encounter`, `mog_setup`), combat (`mog_loop`, `mog_ctl`, `mog_col`, `mog_ai`, `mog_native`, `mog_fight`), dessin (`mog_blit`, `mog_vbl`, `mog_text`), son (`mog_sound`), assemblage (`mog_game`) |
| `game/src/prog_*.c` | program : intro et fin (`prog_intro`), musique (`prog_music`), jumeaux de mog (`prog_blit`, `prog_gfx`, `prog_vbl`, `prog_files`) |
| `game/src/moon_hal.c` | SDL : fenêtre, entrées, flux audio |
| `game/src/moon_mog.c`, `moon_game.c`, `moon_combat.c`, `main.c` | hôte : VBL, entrées, enchaînement program / mog, mode combats seuls |
| `tools/mog_ref.py`, `tools/prog_ref.py` | bancs de référence (Unicorn) |
| `tools/*check.py`, `tools/*lockstep.py` | comparaisons (§3, §4, §7 bis, §8 bis) |
| `tests/mog_run.c`, `tests/prog_run.c` | côté C des comparaisons |
| `tests/mog_game_shot.c`, `tests/prog_intro_shot.c` | jeu et intro sans écran (PNG, WAV) |
| `game/data/*_names.txt`, `tools/ix_names.py` | noms des labels utilisés par le C (§10 ter) |
| `game/include/mog_struct.h` | champs des enregistrements en mémoire émulée (§10 quater) |
| `libmoon_assets/` | lecture des fichiers du jeu (CEL, PIV et `.p`, LZSS, RNC, MOD, stile, `.t`, `.a`, collide.hit) |
| `tests/lib_audit.c`, `tests/lib_audit_ref.c` | audit de la bibliothèque : chaque fichier comparé aux décodeurs d'origine du portage, figés (§10 bis) |

L'ancienne version non fidèle (moteur de rendu et d'entités à part,
modules par lieu) est supprimée. Les documents d'analyse écrits pour elle
(`DOC_MODE_OVERWORLD.md`, `DOC_MODE_COMBAT.md`,
`DOC_ANIMATIONS_INTRO_FIN.md`, `DOC_COMPARAISON_ASM_C.md`) restent pour
leur lecture de l'assembleur ; la référence est désormais le code porté.

### 10 bis. libmoon_assets

La bibliothèque lit les formats du jeu hors de la mémoire émulée.
`tests/lib_audit.c` la compare, fichier par fichier, aux décodeurs du
portage (eux-mêmes vérifiés contre l'original) : les 221 vérifications
sont identiques. L'audit a corrigé :

- CEL : une frame de masque 0 n'a aucun plan (et non 5) ; la taille des
  pixels vient de l'en-tête (+6, en bits, comme `LAB_0CB6`) ; masque et
  décalage de chaque frame sont exposés ;
- LZSS : décalage 0 fidèle (l'octet est recopié sur lui-même), fenêtre
  avant la sortie (`moon_lzss_decompress_window`) ;
- `.p` : ce sont des PIV (`LAB_0C27`), pas des CEL ;
- terrain `.t` : un long (taille compressée) précède le flux LZSS ;
- RNC `.cmp` : décodeur remplacé par `Unpack_Rnc1` de program (en-tête de
  12 octets, flux lu à rebours) ; les MOD se chargent ;
- stile : pas de compression, carte de tuiles brute (`moon_stile_tile`) ;
- collide.hit : les 14 sprites sont lus (fin de section `99`), chiffres
  lus comme `LAB_03D8` / `LAB_03D9` (maximum signé).

Code mort retiré (blocs jamais atteints de l'original) : le codeur à bits
`LAB_0408`, le RLE `Unpack_StileRle` (`LAB_0448`), la variante IFF/PackBits
(`LAB_0434`) et l'outil `moon-view-stile` qui s'appuyait sur ce RLE.

Le jeu décode par la bibliothèque, directement dans la mémoire émulée :

| Routine du portage | Original | Bibliothèque |
|---|---|---|
| `mog_unpack` (et `prog_unpack`, son jumeau) : CEL, PIV, `.p`, `.t` | `LAB_0CC2` / `LAB_049C` | `moon_lzss_decompress_window` (fenêtre = mémoire sous la sortie) |
| `rnc_unpack` (`prog_intro.c`) : `music.cmp`, `vmusic.cmp` | `Unpack_Rnc1` | `moon_rnc1_decompress`, puis recopie en place et mise à zéro comme l'original |
| `mog_load_hit_cel` (`mog_boot.c`) : collide.hit | `LAB_03CE` / `LAB_03D2` | `moon_hit_parse` + `moon_hit_find` |
| palettes des PIV (`mog_encounter.c`, `prog_intro.c`) | `LAB_03F4` / `LAB_03FF` | `moon_piv_colour` |

La lecture des fichiers en mémoire (`mog_file_read`) reste celle de
l'original : elle fait partie de l'état comparé. Seule différence connue :
l'original cherche le nom d'une CEL n'importe où dans collide.hit (suivi
d'un saut de ligne), la bibliothèque compare des lignes entières ; les
noms du jeu sont tous des lignes entières.

Les décodeurs d'avant la bibliothèque restent, figés, dans
`tests/lib_audit_ref.c` : ils servent de référence indépendante à
`lib_audit`. Vérifications après le passage : `mog_bootcheck.py`,
`mog_gamecheck.py` (mêmes résultats qu'avant), lockstep de l'intro, de la
fin et d'un combat Practice.

### 10 ter. Noms des labels

Le C ne cite plus aucun label brut (`LAB_xxxx`, `L00_xxxxx`, `SECSTRT_n`) :
chaque adresse de mog ou de program utilisée a un nom, donné dans
`game/data/mog_names.txt` et `game/data/program_names.txt` (label, nom,
description). `tools/ix_names.py` en tire `game/data/ix_mog_names.h` et
`ix_program_names.h` (`#define MOG_v_PlayerObj MOG_LAB_05F2 /* ... */`), que
les sources incluent à la place de `ix_*_syms.h`.

Conventions (celles des labels déjà nommés dans l'assembleur) :

| Préfixe | Genre | Exemples |
|---|---|---|
| `Module_Verbe` | routine | `Ctl_Dragon`, `Kit_Troll`, `Enc_Balok`, `React_Parry`, `Call_Shake`, `IxOpA0_Move` |
| `v_` | variable | `v_PlayerObj`, `v_TurnKnight`, `v_ScrollPos` |
| `t_` | table | `t_KnightAttacks`, `t_PalForest`, `t_LairTerrain` |
| `s_` | chaîne | `s_Dragon1Cel`, `s_EnterLair` |
| `b_` | tampon | `b_Unpack`, `b_TerrainObjects` |
| `x_` | script IMAGEXCEL | `x_KnightAtk3`, `x_RatBite`, `x_DruidKnighting` |

Les jumeaux de program (code de mog compilé pour program) portent le nom
de leur label de mog ; `tools/prog_twins.py` passe par les tables.

```sh
python3 tools/ix_names.py gen            # en-têtes
python3 tools/ix_names.py apply          # MOG_LAB_xxxx -> MOG_<nom> dans les sources
python3 tools/ix_names.py rename a b     # renommer (table et sources à la fois)
python3 tools/ix_names.py check          # labels bruts restants
```

L'assembleur n'est pas touché : les commentaires du C (« LAB_0CBB : ... »)
et les bancs de comparaison continuent de s'y référer par ses labels ; la
table fait le lien. Un renommage ne change pas le code : chaque lot a été
vérifié en comparant le code objet de tout le jeu avant et après
(identique octet pour octet).


### 10 quater. Structures

Les données du jeu restent dans la mémoire émulée, à leurs adresses
d'origine (les bancs de comparaison lisent cette mémoire) ; ce sont les
décalages qui sont nommés. `game/include/mog_struct.h` décrit chaque
enregistrement par un `enum` (taille du champ en commentaire : b, w, l) :

| Préfixe | Enregistrement |
|---|---|
| `OBJ_` | objet combattant (132 octets : position, scripts, caractéristiques, inventaire, IA, carte) |
| `INV_` | inventaire (24 octets : un compte par objet, clés et pierres de lune en bits) |
| `LAIR_` | repaire de la carte (20 octets) |
| `CMB_` | `v_Combatants` (joueur, adversaire, police, lune, jour) |
| `ENT_`, `CTX_` | entité et contexte du moteur IMAGEXCEL (ceux de program aussi, avec `PENT_`, `PCTX_`) |
| `TRAJ_`, `FLY_` | demande de vol et vols en cours (`t_Trajectory`, `t_Trajectories`) |
| `CEL_`, `CELF_` | en-tête d'un fichier CEL et entrée de frame |
| `ZONE_`, `TXT_` | zone d'un écran à pointeur, enregistrement de texte |
| `SCR_`, `DL_` | zone à restaurer, frame dessinée (listes de corps et de frappe) |
| `VOX_`, `INS_`, `ENV_`, `AUD_`, `HW_` | pilote son : voie, instrument, enveloppe, registres de Paula |

Le lecteur de modules de program garde sa voie (`CH_`) dans
`prog_music.c`. Les tailles servent aux pas des boucles (`LAIR_SIZE`,
`INV_SIZE`, `ZONE_SIZE`...) et les numéros de champ passés comme données
reçoivent aussi leur nom (case d'une zone d'écran : `OBJ_GOLD`,
`INV_KEYS` ; objets du butin : `INV_MOONSTONES`...).

Comme pour les noms de labels, chaque lot a été vérifié par le code objet
de tout le jeu (identique) ; un décalage mal attribué (même nombre, autre
enregistrement) ne se voit pas ainsi : les variables ont été classées
par leur origine (`me(m)`, `rl(v_CurObj)`, `+ OBJ_INVENTORY`...) et les
cas ambigus relus un par un. Les réutilisations d'origine restent
visibles : champs de l'objet écrits dans l'entité par le Dragon
(`mog_ai.c`), `OBJ_MAP_X` / `OBJ_MAP_Y` servant de cible du saut en
combat.

---

## 11. Ce qui reste

- **Essais réels** : la fenêtre SDL, le clavier, les manettes et le son
  n'ont été vérifiés que sans écran (captures, lancements courts) ; le
  paquet Windows n'a pas été essayé sur Windows.
- **Parties complètes** : les comparaisons couvrent des centaines à
  quelques milliers d'images par scénario, pas une partie entière ; une
  routine rare pourrait manquer (le C le signale : « … non portée »).
- **Approximations** de l'intro : toute touche compte pour sauter le
  générique (le gestionnaire clavier `LAB_0342` et sa touche de pause ne
  sont pas repris) ; la souris (`LAB_034D`) n'est pas portée (inutile à
  l'intro) ; Entrée ou le bouton de la manette font défiler l'intro sans
  attendre (ajout de l'hôte).
- **Démarrage de mog** : quelques tables de `SECSTRT_30` (conversion de
  pixels) ne sont pas construites ; aucune routine portée ne les lit.
