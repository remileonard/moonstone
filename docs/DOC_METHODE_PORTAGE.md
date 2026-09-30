# Méthode de portage de `mog` en C : émulateur de référence et comparaison

Ce document explique **comment** le jeu (`mog`, le programme principal de
Moonstone) est porté en C. Il décrit aussi comment chaque morceau est
vérifié contre l'original : mémoire et écrans comparés octet par octet.

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
| **`tools/mog_lockstep.py`** | **le jeu entier**, original et C côte à côte (§4) |
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

À chaque VBL :

1. compteur de VBL ;
2. serveur des couleurs (`LAB_0E5D`) ;
3. entrées : joystick, touches → `SECSTRT_21` / `LAB_0B91` ;
4. image affichée : plans de la copper list et sprite du pointeur ;
5. 1/50 s chez l'hôte.

`moon_mog.c` relie cela à SDL, avec `moonstone <données> 3 --mog`.

`tests/mog_game_shot` fait tourner le même jeu **sans écran**. Il est mené
par un script d'entrées (« 150 VBL à droite, touche I... ») et enregistre
des captures PNG : c'est la vérification rapide du branchement.

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

## 9. Commandes

```sh
cmake --build build

# jeu complet (fenêtre SDL)
build/game/moonstone <données> 3 --mog

# original et C côte à côte
python3 tools/mog_lockstep.py <données> --frames 2000 --fire 0.15 --wander 60 --place 0x19
python3 tools/mog_lockstep.py <données> --replay dm.joy --frames 1320   # rejouer

# démarrage C contre original
python3 tools/mog_gamecheck.py <données>

# captures sans écran, script d'entrées
build/tests/mog_game_shot <données> <préfixe> script.txt
```

Les fichiers du jeu d'origine ne sont pas dans le dépôt. Le dossier
`<données>` doit les contenir : `kn1.ob`, `test`, `*.PIV`, `*.CEL`, `*.t`,
`collide.hit`...
