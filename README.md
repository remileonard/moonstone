# moonstone

Portage en C de **Moonstone — A Hard Days Knight** (Mindscape, 1991,
Amiga), routine par routine, à partir du code 68000 désassemblé
(`amiga_asm/`).

Le C travaille sur la mémoire de l'original (mêmes adresses, mêmes
données) et chaque partie est vérifiée contre l'original exécuté dans un
émulateur 68000 : mémoire et écrans identiques, image après image.

Porté : intro (générique, scènes, musique), menu du début et choix des
chevaliers, partie complète (carte, villes, repaires, temple, écrans,
combats, dragon, démon), son, séquence de fin.

## Compiler et jouer

```sh
cmake -S . -B build && cmake --build build
build/game/moonstone <dossier_des_fichiers_du_jeu> 3
```

Les fichiers du jeu d'origine (`kn1.ob`, `test`, `*.PIV`, `*.CEL`, `*.t`,
`collide.hit`...) ne sont pas fournis. Options : `--fin [drapeaux]` (la
fin, sans jouer), `--combat rencontre|all [lieu]` (combats seuls).
Windows : `tools/build_windows.sh` (MinGW-w64).

Commandes : flèches + Espace = joystick 1 ; W A S D + F = joystick 2
(second joueur d'un duel) ; lettres, chiffres, Tab (barre d'espace),
Entrée, retour arrière = clavier de l'Amiga ; Échap = quitter.

## Documentation

- `docs/DOC_METHODE_PORTAGE.md` : méthode, bancs de référence,
  comparaisons, état du portage ;
- `docs/DOC_MOTEUR_COMBAT_MOG.md`, `docs/DOC_MOTEUR_IMAGEXCEL.md`,
  `docs/DOC_TECHNIQUE.md` : analyses de l'original.
