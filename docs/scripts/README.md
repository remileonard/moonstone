# Scripts IMAGEXCEL — identification et extraction

Générés par `tools/ix_scripts.py` (remplace `extract_scripts.py` et
`ix_validate.py`) :

```sh
tools/disasm/setup.sh            # une fois : vasm, capstone…
python3 tools/ix_scripts.py      # program + mog
```

| Fichier | Contenu |
|---|---|
| `docs/scripts/mog_scripts.md` | scripts du combat : tables (attaques, marche…), rôles, décodage |
| `docs/scripts/program_scripts.md` | scripts de l'intro et de la fin |
| `game/data/ix_mog.c`, `game/data/ix_program.c` | données fidèles pour le portage |
| `game/include/ix_data.h` | types de ces données |

## Méthode

Le binaire et ses relocations font foi (pas de balayage du texte asm) :

1. **Candidats** : toute adresse désignée par un pointeur (relocation, ou
   `LEA x(PC)`) qui n'est pas du code.
2. **Validation stricte** avec la sémantique exacte du moteur du binaire.
   Les deux moteurs lisent les étapes de la même façon mais **n'ont pas les
   mêmes opcodes** (voir `docs/DOC_MOTEUR_COMBAT_MOG.md` §7 pour `mog`) :
   tailles exactes, adresses relogées exactement là où l'opcode en attend,
   octet de dessin ≤ `$1C` et multiple de 4, fin d'étape `FF xx`, sauts
   suivis.
3. **Usage par le code** : un bloc que seul le code désigne et qu'il lit
   comme des données (`MOVE.W` direct, `LEA` puis `(An)+` ou `(An,Dn)`,
   remplissage `d(An)`) est rejeté ; c'est le cas de `t_AttackStickR/L`,
   `t_WalkStep*`, `LAB_0028`, `LAB_069F` (tables de mots que l'ancien outil
   prenait pour des scripts).
4. **Rôles** :
   - tables de scripts **remplies par le code** (`LEA T,A0` puis
     `MOVE.L #script,d(A0)`, ou via `MOVEA.L 30(A1),A0`), rattachées au champ
     de l'objet qui les désigne et à la routine qui les pose ;
   - entrées nommées : `objet+46` marche (horizontal / haut / bas, phase
     0–3), `objet+34` attaques (index → combinaisons du joystick d'après
     `t_AttackStickR`) ;
   - références depuis d'autres scripts (`$84`, `$AC` ombre, `$B8` entité
     créée, `$A8` écriture d'un script dans l'objet…) ;
   - `v_CtlScript` (script renvoyé par un contrôleur), autres références du code.

## Champs de l'objet

| Champ | Contenu |
|---|---|
| 22 | script de repos |
| 26 | script de réaction |
| 30 | table de scripts |
| 34 | table des attaques (index = `64(objet)`) |
| 42 | table des **dégâts** par attaque (nombres, retirés aux PV de la cible) |
| 46 | table de marche |
| 50 | table de nombres comparés à l'attaque adverse |

## Format des données C

Chaque hunk utile est reproduit tel qu'en mémoire (`IxHunk`), avec ses
relocations (`IxReloc`) : un pointeur relogé est un mot long big-endian
contenant un offset dans le hunk cible, à convertir en adresse au
chargement (comme le chargeur Amiga). Les scripts et tables sont repérés par
(hunk, offset) dans `ix_<bin>_scripts[]` et `ix_<bin>_tables[]`. Un moteur C
fidèle peut ainsi exécuter les scripts sans aucune réinterprétation
(sauts, tables, `$A8`, `$B8` compris).

## Comparaison avec l'ancien outil

Les 162 scripts de `extract_scripts.py` sont retrouvés, sauf `LAB_069F`
(table de mots). Pour une vingtaine, les limites diffèrent : l'ancien outil
s'arrêtait au label suivant (et incluait donc des octets situés après le
`FF FF` final), le nouveau suit le moteur ; il compte aussi les octets des
scripts atteints par des sauts.

Les fichiers `tools/moon_anim_*.c` et `tools/moon_anim_registry.h`
(visualiseur `moon-anim-viewer`) proviennent de l'ancien outil et ne sont
pas régénérés : le visualiseur utilise le moteur de l'intro
(`tools/imagexcel.c`), qui ne sait pas exécuter les opcodes de `mog`.
