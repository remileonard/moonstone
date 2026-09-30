# Désassemblage de `nb` — rapport

- Réassemblage vasm → binaire original : **identique** (contenu et relocations de chaque hunk)
- Racines de code fournies à IRA : 40 (PROBABLE 14, HEURISTIQUE 26)

## Classification par hunk CODE

Octets de code trouvés par l'analyse (CERTAIN = atteint depuis l'entrée ; PROBABLE = via pointeur validé ; HEURISTIQUE = non atteint mais décodage valide, à vérifier). Les deux dernières colonnes comparent les débuts d'instruction du nouveau source à ceux de l'ancien.

| Hunk | Taille | CERTAIN | PROBABLE | HEURISTIQUE | Données | instr. ancien → données | données ancien → instr. |
|---|---|---|---|---|---|---|---|
| S_0 | 1012 | 764 | 88 | 158 | 2 | 0 | 0 |
| S_4 | 1492 | 128 | 1256 | 106 | 2 | 0 | 0 |
| S_7 | 3132 | 2852 | 10 | 224 | 46 | 13 | 0 |
| S_9 | 2184 | 1012 | 0 | 1022 | 150 | 31 | 0 |
| S_12 | 2028 | 1318 | 20 | 690 | 0 | 0 | 0 |
| S_14 | 884 | 884 | 0 | 0 | 0 | 0 | 0 |
| **Total** | | 6958 | 1374 | 2200 | 200 | 44 | 0 |

## Pointeurs vers un hunk CODE rejetés comme code (4)

Ces cibles sont référencées par adresse mais leur décodage échoue : ce sont des données placées dans un hunk CODE (variables, tables, textes).

- `0:$3D090` : $3D090: hors du hunk
- `LAB_0091` : texte ASCII 'No Mat'
- `LAB_0092` : texte ASCII 'Checks'
- `L09_00032` : $32: mot nul ($0000) décodé comme ORI.B #0

## Labels

- 478 labels de l'ancien source conservés à la même adresse.
- 0 labels renommés d'après `amiga_asm/nb.sym`.
- 16 labels anciens disparus (ils pointaient dans des données mal décodées ou au milieu d'instructions) :

  `LAB_009A`, `LAB_009B`, `LAB_009C`, `LAB_009E`, `LAB_0104`, `LAB_0105`, `LAB_0106`, `LAB_0107`, `LAB_0108`, `LAB_0109`, `LAB_010F`, `LAB_0110`, `LAB_0111`, `LAB_0112`, `LAB_0113`, `LAB_0117`
