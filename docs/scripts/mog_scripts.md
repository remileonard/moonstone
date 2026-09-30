# Scripts IMAGEXCEL de `mog`

Généré par `tools/ix_scripts.py` à partir du binaire et de ses relocations ; ne pas modifier à la main.

- Moteur : combat (`t_IxOpcodes`)
- Scripts identifiés : **259** ; tables de scripts : **8**
- Pointeurs candidats rejetés (données d'un autre type) : 1240

## Tables de scripts

| Table | Adresse | Entrées | Rôle |
|---|---|---|---|
| `LAB_015E` | 0:$3B5A | 4 | code : LAB_0156 |
| `LAB_015E+20` | 0:$3B6E | 4 | — |
| `LAB_084E+554` | 4:$3938 | 1 | — |
| `LAB_084E+672` | 4:$39AE | 1 | — |
| `LAB_0881+50` | 4:$4A92 | 1 | — |
| `LAB_08FC` | 4:$7B68 | 16 | objet+46 : marche (LAB_0DCB)<br>code : LAB_0DCB<br>code : LAB_0DD7 |
| `LAB_0D43+146` | 31:$33A | 8 | — |
| `LAB_0F8E` | 44:$75A | 4 | — |

## Tables de scripts remplies par le code

Tables en mémoire (souvent en BSS) que le code remplit avec `MOVE.L #script,d(An)` ; les champs de l'objet (30, 34, 46…) les désignent.

**`objet+30`** — objet+30 (scripts objet+30)

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_07F5` |
| 8 | [2] | `LAB_07FB` |
| 20 | [5] | `LAB_07FB` |
| 28 | [7] | `LAB_084A` |
| 32 | [8] | `LAB_07FB` |

**`objet+34`** — objet+34 (attaques)

| Offset | Entrée | Script |
|---|---|---|
| 16 | attaque 4 : feu + G+B | `LAB_07F2` |
| 28 | attaque 7 : feu + B ou D+G+B+H | `LAB_07F0` |

**`LAB_05FB`** — objet+34 (attaques) ; posée par LAB_0169

| Offset | Entrée | Script |
|---|---|---|
| 4 | attaque 1 : feu + D+B | `LAB_0801` |
| 8 | attaque 2 : feu + D ou D+B+H | `LAB_0801` |
| 12 | attaque 3 : feu + G+H | `LAB_0801` |
| 16 | attaque 4 : feu + G+B | `LAB_0801` |
| 20 | attaque 5 : feu + G ou B+H | `LAB_0801` |
| 24 | attaque 6 : feu + D+H | `LAB_0802` |
| 28 | attaque 7 : feu + B ou D+G+B+H | `LAB_0801` |
| 32 | attaque 8 : feu + H | `LAB_0802` |

**`LAB_05FC`** — objet+30 (scripts objet+30) ; posée par LAB_0169

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_0810` |
| 8 | [2] | `LAB_080F` |
| 12 | [3] | `LAB_080E` |
| 20 | [5] | `LAB_0810` |
| 24 | [6] | `LAB_080E` |
| 32 | [8] | `LAB_080E` |

**`LAB_05FE`** — objet+34 (attaques) ; posée par LAB_0170

| Offset | Entrée | Script |
|---|---|---|
| 4 | attaque 1 : feu + D+B | `LAB_0829` |
| 8 | attaque 2 : feu + D ou D+B+H | `LAB_0829` |
| 12 | attaque 3 : feu + G+H | `LAB_0829` |
| 16 | attaque 4 : feu + G+B | `LAB_0829` |
| 20 | attaque 5 : feu + G ou B+H | `LAB_0829` |
| 24 | attaque 6 : feu + D+H | `LAB_082A` |
| 28 | attaque 7 : feu + B ou D+G+B+H | `LAB_0829` |
| 32 | attaque 8 : feu + H | `LAB_082A` |

**`LAB_05FF`** — objet+30 (scripts objet+30) ; posée par LAB_0170

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_0838` |
| 8 | [2] | `LAB_0837` |
| 12 | [3] | `LAB_0836` |
| 20 | [5] | `LAB_0838` |
| 24 | [6] | `LAB_0836` |
| 32 | [8] | `LAB_0836` |

**`LAB_0600`** — objet+30 (scripts objet+30) ; posée par LAB_018B

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_0847` |
| 8 | [2] | `LAB_0847` |
| 12 | [3] | `LAB_0847` |
| 16 | [4] | `LAB_0840` |
| 20 | [5] | `LAB_0847` |
| 24 | [6] | `LAB_0845` |
| 32 | [8] | `LAB_0845` |

**`LAB_0602`** — objet+30 (scripts objet+30) ; posée par LAB_018F

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_0862` |
| 8 | [2] | `LAB_086A` |
| 12 | [3] | `LAB_0862` |
| 16 | [4] | `LAB_0860` |
| 20 | [5] | `LAB_0862` |
| 24 | [6] | `LAB_0862` |
| 28 | [7] | `LAB_0860` |
| 32 | [8] | `LAB_0860` |

**`LAB_0604`** — objet+30 (scripts objet+30) ; posée par LAB_0195

| Offset | Entrée | Script |
|---|---|---|
| 4 | [1] | `LAB_087F` |
| 8 | [2] | `LAB_087F` |
| 12 | [3] | `LAB_087F` |
| 16 | [4] | `LAB_087F` |
| 20 | [5] | `LAB_087F` |
| 24 | [6] | `LAB_087F` |
| 28 | [7] | `LAB_087F` |
| 32 | [8] | `LAB_087F` |

**`LAB_060C`** — objet+46 (marche) ; posée par LAB_0169

| Offset | Entrée | Script |
|---|---|---|
| 0 | marche horizontal phase 0 | `LAB_0803` |
| 4 | marche horizontal phase 1 | `LAB_0804` |
| 8 | marche horizontal phase 2 | `LAB_0805` |
| 32 | marche haut phase 0 | `LAB_0806` |
| 36 | marche haut phase 1 | `LAB_0807` |
| 40 | marche haut phase 2 | `LAB_0808` |
| 44 | marche haut phase 3 | `LAB_0809` |
| 64 | marche bas phase 0 | `LAB_080A` |
| 68 | marche bas phase 1 | `LAB_080B` |
| 72 | marche bas phase 2 | `LAB_080C` |
| 76 | marche bas phase 3 | `LAB_080D` |

**`LAB_060D`** — objet+46 (marche) ; posée par LAB_0176

| Offset | Entrée | Script |
|---|---|---|
| 0 | marche horizontal phase 0 | `LAB_081D` |
| 4 | marche horizontal phase 1 | `LAB_081E` |
| 8 | marche horizontal phase 2 | `LAB_081F` |
| 32 | marche haut phase 0 | `LAB_0820` |
| 36 | marche haut phase 1 | `LAB_0821` |
| 40 | marche haut phase 2 | `LAB_0822` |
| 44 | marche haut phase 3 | `LAB_0823` |
| 64 | marche bas phase 0 | `LAB_0824` |
| 68 | marche bas phase 1 | `LAB_0825` |
| 72 | marche bas phase 2 | `LAB_0826` |
| 76 | marche bas phase 3 | `LAB_0827` |

**`LAB_060E`** — objet+46 (marche) ; posée par LAB_0170

| Offset | Entrée | Script |
|---|---|---|
| 0 | marche horizontal phase 0 | `LAB_082B` |
| 4 | marche horizontal phase 1 | `LAB_082C` |
| 8 | marche horizontal phase 2 | `LAB_082D` |
| 32 | marche haut phase 0 | `LAB_082E` |
| 36 | marche haut phase 1 | `LAB_082F` |
| 40 | marche haut phase 2 | `LAB_0830` |
| 44 | marche haut phase 3 | `LAB_0831` |
| 64 | marche bas phase 0 | `LAB_0832` |
| 68 | marche bas phase 1 | `LAB_0833` |
| 72 | marche bas phase 2 | `LAB_0834` |
| 76 | marche bas phase 3 | `LAB_0835` |

**`LAB_060F`** — objet+46 (marche) ; posée par LAB_0195

| Offset | Entrée | Script |
|---|---|---|
| 32 | marche haut phase 0 | `LAB_0873` |
| 36 | marche haut phase 1 | `LAB_0874` |
| 40 | marche haut phase 2 | `LAB_0875` |
| 44 | marche haut phase 3 | `LAB_0876` |
| 48 | marche haut phase 4 | `LAB_0877` |
| 52 | marche haut phase 5 | `LAB_0877` |
| 56 | marche haut phase 6 | `LAB_0877` |
| 60 | marche haut phase 7 | `LAB_0877` |
| 64 | marche bas phase 0 | `LAB_0879` |
| 68 | marche bas phase 1 | `LAB_087A` |
| 72 | marche bas phase 2 | `LAB_087B` |
| 76 | marche bas phase 3 | `LAB_087C` |
| 80 | marche bas phase 4 | `LAB_087D` |
| 84 | marche bas phase 5 | `LAB_087D` |
| 88 | marche bas phase 6 | `LAB_087D` |
| 92 | marche bas phase 7 | `LAB_087D` |

**`LAB_0610`** — objet+46 (marche) ; posée par LAB_0167

| Offset | Entrée | Script |
|---|---|---|
| 0 | marche horizontal phase 0 | `LAB_07DD` |
| 4 | marche horizontal phase 1 | `LAB_07DE` |
| 8 | marche horizontal phase 2 | `LAB_07DF` |
| 12 | marche horizontal phase 3 | `LAB_07E0` |
| 32 | marche haut phase 0 | `LAB_07E1` |
| 36 | marche haut phase 1 | `LAB_07E2` |
| 40 | marche haut phase 2 | `LAB_07E3` |
| 44 | marche haut phase 3 | `LAB_07E4` |
| 64 | marche bas phase 0 | `LAB_07E5` |
| 68 | marche bas phase 1 | `LAB_07E6` |
| 72 | marche bas phase 2 | `LAB_07E7` |
| 76 | marche bas phase 3 | `LAB_07E8` |

**`LAB_0612`** — objet+46 (marche) ; posée par LAB_018F

| Offset | Entrée | Script |
|---|---|---|
| 0 | marche horizontal phase 0 | `LAB_086C` |
| 4 | marche horizontal phase 1 | `LAB_086D` |
| 8 | marche horizontal phase 2 | `LAB_086E` |
| 12 | marche horizontal phase 3 | `LAB_086F` |
| 32 | marche haut phase 0 | `LAB_0852` |
| 36 | marche haut phase 1 | `LAB_0853` |
| 40 | marche haut phase 2 | `LAB_0854` |
| 44 | marche haut phase 3 | `LAB_0855` |

**`LAB_05F5`** — objet+34 (attaques) ; posée par LAB_0167

| Offset | Entrée | Script |
|---|---|---|
| 0 | attaque 0 : feu + neutre ou D+G ou D+G+B ou D+G+H ou G+B+H | `LAB_07DB` |
| 4 | attaque 1 : feu + D+B | `LAB_07EF` |
| 8 | attaque 2 : feu + D ou D+B+H | `LAB_07ED` |
| 12 | attaque 3 : feu + G+H | `LAB_07EA` |
| 16 | attaque 4 : feu + G+B | `LAB_07F4` |
| 20 | attaque 5 : feu + G ou B+H | `LAB_07E9` |
| 24 | attaque 6 : feu + D+H | `LAB_07F1` |
| 28 | attaque 7 : feu + B ou D+G+B+H | `LAB_07F3` |
| 32 | attaque 8 : feu + H | `LAB_07EE` |

**`LAB_05F6`** — objet+30 (scripts objet+30) ; posée par LAB_0167

| Offset | Entrée | Script |
|---|---|---|
| 0 | [0] | `LAB_07DB` |
| 4 | [1] | `LAB_07F6` |
| 8 | [2] | `LAB_07F6` |
| 12 | [3] | `LAB_07F5` |
| 16 | [4] | `LAB_07DC` |
| 20 | [5] | `LAB_07F6` |
| 24 | [6] | `LAB_07F5` |
| 28 | [7] | `LAB_07DC` |
| 32 | [8] | `LAB_07F5` |

**`LAB_05F9`** — objet+30 (scripts objet+30) ; posée par LAB_0176

| Offset | Entrée | Script |
|---|---|---|
| 0 | [0] | `LAB_0815` |
| 4 | [1] | `LAB_0816` |
| 8 | [2] | `LAB_0815` |
| 12 | [3] | `LAB_0814` |
| 16 | [4] | `LAB_0815` |
| 20 | [5] | `LAB_0816` |
| 24 | [6] | `LAB_0814` |
| 28 | [7] | `LAB_0815` |
| 32 | [8] | `LAB_0814` |

**`SECSTRT_44`** — table SECSTRT_44

| Offset | Entrée | Script |
|---|---|---|
| 24 | [6] | `L44_00C70` |


## Scripts

| Script | Adresse | Octets | Étapes | Rôles |
|---|---|---|---|---|
| `LAB_024E` | 0:$5E82 | 38 | 1 | référencé par le code dans LAB_023B<br>référencé par le code dans LAB_023C |
| `LAB_024F` | 0:$5E9A | 14 | 1 | référencé par le code dans LAB_0239 |
| `LAB_04F5` | 0:$B026 | 26 | 1 | référencé par le code dans LAB_04F8 |
| `LAB_07DB` | 4:$1064 | 20 | 1 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 0 : feu + neutre ou D+G ou D+G+B ou D+G+H ou G+B+H<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [0]<br>cible de saut depuis LAB_07E9<br>cible de saut depuis LAB_07EA<br>cible de saut depuis LAB_07ED<br>cible de saut depuis LAB_07EE … |
| `LAB_07DC` | 4:$1080 | 44 | 2 | LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [4]<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [7]<br>objet+26 (réaction) dans LAB_0167<br>référencé par le code dans LAB_0155 |
| `LAB_07DD` | 4:$10AC | 20 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 0<br>référencé par le code dans LAB_0155 |
| `LAB_07DE` | 4:$10C0 | 20 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 1<br>référencé par le code dans LAB_0155 |
| `LAB_07DF` | 4:$10D4 | 20 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 2<br>référencé par le code dans LAB_0155 |
| `LAB_07E0` | 4:$10E8 | 20 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 3<br>référencé par le code dans LAB_0155 |
| `LAB_07E1` | 4:$10FC | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 0<br>référencé par le code dans LAB_0155 |
| `LAB_07E2` | 4:$110A | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 1<br>référencé par le code dans LAB_0155 |
| `LAB_07E3` | 4:$1118 | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 2<br>référencé par le code dans LAB_0155 |
| `LAB_07E4` | 4:$1126 | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 3<br>référencé par le code dans LAB_0155 |
| `LAB_07E5` | 4:$1134 | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 0<br>référencé par le code dans LAB_0155 |
| `LAB_07E6` | 4:$1142 | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 1<br>référencé par le code dans LAB_0155 |
| `LAB_07E7` | 4:$1150 | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 2<br>référencé par le code dans LAB_0155 |
| `LAB_07E8` | 4:$115E | 14 | 1 | LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 3<br>référencé par le code dans LAB_0155 |
| `LAB_07E9` | 4:$116C | 154 | 6 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 5 : feu + G ou B+H<br>référencé par le code dans LAB_0155 |
| `LAB_07EA` | 4:$11F2 | 130 | 4 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 3 : feu + G+H<br>référencé par le code dans LAB_0155 |
| `LAB_07EB` | 4:$1260 | 24 | 1 | référencé par le code dans LAB_02CA |
| `LAB_07EC` | 4:$1278 | 16 | 1 | référencé par le code dans LAB_02CB |
| `LAB_07ED` | 4:$1288 | 222 | 8 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 2 : feu + D ou D+B+H<br>référencé par le code dans LAB_0155 |
| `LAB_07EE` | 4:$1352 | 218 | 8 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 8 : feu + H<br>référencé par le code dans LAB_0155 |
| `LAB_07EF` | 4:$1418 | 166 | 6 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 1 : feu + D+B<br>référencé par le code dans LAB_0155 |
| `LAB_07F0` | 4:$14BE | 64 | 3 | objet+34 [objet+34 (attaques)] : attaque 7 : feu + B ou D+G+B+H<br>référencé par le code dans LAB_018C |
| `LAB_07F1` | 4:$14FE | 104 | 4 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 6 : feu + D+H<br>référencé par le code dans LAB_0155 |
| `LAB_07F2` | 4:$1566 | 114 | 4 | objet+34 [objet+34 (attaques)] : attaque 4 : feu + G+B<br>référencé par le code dans LAB_018C |
| `LAB_07F3` | 4:$15D8 | 26 | 1 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 7 : feu + B ou D+G+B+H<br>référencé par le code dans LAB_0155<br>référencé par le code dans LAB_0175<br>référencé par le code dans LAB_0188<br>référencé par le code dans LAB_01F6 |
| `LAB_07F4` | 4:$15F2 | 26 | 1 | LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 4 : feu + G+B<br>référencé par le code dans LAB_0155 |
| `LAB_07F5` | 4:$160C | 264 | 8 | LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [3]<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [6]<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [8]<br>objet+30 [objet+30 (scripts objet+30)] : [1]<br>référencé par le code dans LAB_0155<br>référencé par le code dans LAB_0192 |
| `LAB_07F6` | 4:$169C | 252 | 8 | LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [1]<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [2]<br>LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [5]<br>référencé par le code dans LAB_0155 |
| `LAB_07F7` | 4:$1720 | 120 | 4 | cible de saut depuis LAB_07F5<br>cible de saut depuis LAB_07F6<br>cible de saut depuis LAB_07FA<br>cible de saut depuis LAB_07FB<br>cible de saut depuis LAB_0849<br>cible de saut depuis LAB_084A … |
| `LAB_07F8` | 4:$173A | 94 | 3 | cible de saut depuis LAB_07F9<br>référencé par le code dans LAB_01F4<br>référencé par le code dans LAB_020B |
| `LAB_07F9` | 4:$1798 | 600 | 18 | référencé par le code dans LAB_01F5<br>référencé par le code dans LAB_020B<br>référencé par le code dans LAB_0F15 |
| `LAB_07FA` | 4:$1992 | 172 | 6 | cible de saut depuis LAB_07FB<br>cible de saut depuis LAB_0849<br>cible de saut depuis LAB_084A<br>cible de saut depuis LAB_084B<br>cible de saut depuis LAB_084C<br>cible de saut depuis LAB_084D … |
| `LAB_07FB` | 4:$19B2 | 258 | 9 | objet+30 [objet+30 (scripts objet+30)] : [2]<br>objet+30 [objet+30 (scripts objet+30)] : [5]<br>objet+30 [objet+30 (scripts objet+30)] : [8]<br>référencé par le code dans LAB_0192<br>référencé par le code dans LAB_0196<br>référencé par le code dans LAB_019E … |
| `LAB_07FC` | 4:$1A08 | 256 | 13 | référencé par le code dans LAB_0164<br>référencé par le code dans LAB_0165<br>référencé par le code dans LAB_01A4 |
| `LAB_07FD` | 4:$1B08 | 620 | 10 | référencé par le code dans LAB_01FF<br>référencé par le code dans LAB_02A9 |
| `LAB_07FE` | 4:$1D74 | 130 | 8 | référencé par le code dans LAB_0192 |
| `LAB_07FF` | 4:$1D9E | 68 | 5 | cible de saut depuis LAB_07FE |
| `LAB_0800` | 4:$1DE2 | 20 | 1 | objet+22 (repos) dans LAB_0169<br>objet+26 (réaction) dans LAB_0169 |
| `LAB_0801` | 4:$1DF6 | 124 | 5 | LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 1 : feu + D+B<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 2 : feu + D ou D+B+H<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 3 : feu + G+H<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 4 : feu + G+B<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 5 : feu + G ou B+H<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 7 : feu + B ou D+G+B+H … |
| `LAB_0802` | 4:$1E72 | 196 | 7 | LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 6 : feu + D+H<br>LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 8 : feu + H<br>référencé par le code dans LAB_0156 |
| `LAB_0803` | 4:$1F36 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_0804` | 4:$1F44 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0805` | 4:$1F52 | 20 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0806` | 4:$1F66 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_0807` | 4:$1F74 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0808` | 4:$1F82 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0809` | 4:$1F90 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_080A` | 4:$1F9E | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_080B` | 4:$1FAC | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_080C` | 4:$1FBA | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_080D` | 4:$1FC8 | 14 | 1 | LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_080E` | 4:$1FD6 | 232 | 9 | LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [3]<br>LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [6]<br>LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [8]<br>référencé par le code dans LAB_0156 |
| `LAB_080F` | 4:$2060 | 406 | 17 | LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [2]<br>référencé par le code dans LAB_0156 |
| `LAB_0810` | 4:$20D4 | 364 | 14 | LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [1]<br>LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [5]<br>référencé par le code dans LAB_0156 |
| `LAB_0811` | 4:$2150 | 240 | 10 | cible de saut depuis LAB_0810 |
| `LAB_0812` | 4:$2240 | 94 | 4 | cible de saut depuis LAB_080E<br>cible de saut depuis LAB_080F<br>cible de saut depuis LAB_0813 |
| `LAB_0813` | 4:$229E | 290 | 13 | cible de saut depuis LAB_080F |
| `LAB_0814` | 4:$2362 | 282 | 9 | LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [3]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [6]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [8]<br>référencé par le code dans LAB_0156 |
| `LAB_0815` | 4:$23F4 | 584 | 17 | LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [0]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [2]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [4]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [7]<br>référencé par le code dans LAB_0156 |
| `LAB_0816` | 4:$246C | 574 | 14 | LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [1]<br>LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [5]<br>référencé par le code dans LAB_0156 |
| `LAB_0817` | 4:$24FE | 428 | 10 | cible de saut depuis LAB_0816 |
| `LAB_0818` | 4:$26AA | 136 | 4 | cible de saut depuis LAB_0814<br>cible de saut depuis LAB_0815<br>cible de saut depuis LAB_0819 |
| `LAB_0819` | 4:$2732 | 464 | 13 | cible de saut depuis LAB_0815 |
| `LAB_081A` | 4:$287A | 28 | 1 | objet+22 (repos) dans LAB_0176<br>objet+26 (réaction) dans LAB_0176 |
| `LAB_081B` | 4:$2896 | 86 | 3 | référencé par le code dans LAB_0242 |
| `LAB_081C` | 4:$28EC | 660 | 15 | référencé par le code dans LAB_01F7<br>référencé par le code dans LAB_024B |
| `LAB_081D` | 4:$2B80 | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_081E` | 4:$2B94 | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_081F` | 4:$2BA8 | 26 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0820` | 4:$2BC2 | 14 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_0821` | 4:$2BD0 | 14 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0822` | 4:$2BDE | 14 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0823` | 4:$2BEC | 14 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_0824` | 4:$2BFA | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_0825` | 4:$2C0E | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0826` | 4:$2C22 | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0827` | 4:$2C36 | 20 | 1 | LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_0828` | 4:$2C4A | 20 | 1 | cible de saut depuis LAB_0836<br>objet+22 (repos) dans LAB_0170<br>objet+26 (réaction) dans LAB_0170 |
| `LAB_0829` | 4:$2C5E | 124 | 5 | LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 1 : feu + D+B<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 2 : feu + D ou D+B+H<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 3 : feu + G+H<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 4 : feu + G+B<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 5 : feu + G ou B+H<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 7 : feu + B ou D+G+B+H … |
| `LAB_082A` | 4:$2CDA | 196 | 7 | LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 6 : feu + D+H<br>LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 8 : feu + H<br>référencé par le code dans LAB_0156 |
| `LAB_082B` | 4:$2D9E | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_082C` | 4:$2DAC | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_082D` | 4:$2DBA | 20 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_082E` | 4:$2DCE | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_082F` | 4:$2DDC | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0830` | 4:$2DEA | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0831` | 4:$2DF8 | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_0832` | 4:$2E06 | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 0<br>référencé par le code dans LAB_0156 |
| `LAB_0833` | 4:$2E14 | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 1<br>référencé par le code dans LAB_0156 |
| `LAB_0834` | 4:$2E22 | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 2<br>référencé par le code dans LAB_0156 |
| `LAB_0835` | 4:$2E30 | 14 | 1 | LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 3<br>référencé par le code dans LAB_0156 |
| `LAB_0836` | 4:$2E3E | 258 | 10 | LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [3]<br>LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [6]<br>LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [8]<br>référencé par le code dans LAB_0156 |
| `LAB_0837` | 4:$2ED0 | 408 | 17 | LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [2]<br>référencé par le code dans LAB_0156 |
| `LAB_0838` | 4:$2F46 | 352 | 14 | LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [1]<br>LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [5]<br>référencé par le code dans LAB_0156 |
| `LAB_0839` | 4:$2FBC | 234 | 10 | cible de saut depuis LAB_0838 |
| `LAB_083A` | 4:$30A6 | 94 | 4 | cible de saut depuis LAB_0836<br>cible de saut depuis LAB_0837<br>cible de saut depuis LAB_083B |
| `LAB_083B` | 4:$3104 | 290 | 13 | cible de saut depuis LAB_0837 |
| `LAB_083C` | 4:$31C8 | 16 | 1 | entrée 0 de la table LAB_015E |
| `LAB_083D` | 4:$31D8 | 20 | 1 | entrée 1 de la table LAB_015E |
| `LAB_083E` | 4:$31EC | 20 | 1 | entrée 2 de la table LAB_015E |
| `LAB_083F` | 4:$3200 | 14 | 1 | entrée 3 de la table LAB_015E |
| `LAB_0840` | 4:$320E | 64 | 4 | LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [4]<br>entrée 0 de la table LAB_015E+20<br>objet+22 (repos) dans LAB_018B<br>référencé par le code dans LAB_0156 |
| `LAB_0841` | 4:$321E | 48 | 3 | entrée 1 de la table LAB_015E+20 |
| `LAB_0842` | 4:$322E | 32 | 2 | entrée 2 de la table LAB_015E+20 |
| `LAB_0843` | 4:$323E | 16 | 1 | entrée 3 de la table LAB_015E+20 |
| `LAB_0844` | 4:$324E | 34 | 2 | cible de saut depuis LAB_0845<br>cible de saut depuis LAB_0847<br>cible de saut depuis LAB_0849<br>objet+26 (réaction) dans LAB_018B |
| `LAB_0845` | 4:$3270 | 436 | 17 | LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [6]<br>LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [8]<br>référencé par le code dans LAB_0156 |
| `LAB_0846` | 4:$3318 | 234 | 9 | cible de saut depuis LAB_0845<br>cible de saut depuis LAB_0847<br>cible de saut depuis LAB_0848 |
| `LAB_0847` | 4:$3402 | 652 | 25 | LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [1]<br>LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [2]<br>LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [3]<br>LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [5]<br>entrée 0 de la table LAB_084E+672<br>référencé par le code dans LAB_0156 |
| `LAB_0848` | 4:$34D2 | 410 | 17 | cible de saut depuis LAB_0847 |
| `LAB_0849` | 4:$3582 | 544 | 17 | référencé par le code dans LAB_0207<br>référencé par le code dans LAB_0232 |
| `LAB_084A` | 4:$3666 | 282 | 9 | cible de saut depuis LAB_0849<br>cible de saut depuis LAB_084D<br>cible de saut depuis LAB_084E<br>entité créée ($B8) depuis LAB_0849<br>objet+30 [objet+30 (scripts objet+30)] : [7]<br>référencé par le code dans LAB_0188 … |
| `LAB_084B` | 4:$36BA | 248 | 9 | référencé par le code dans LAB_0209 |
| `LAB_084C` | 4:$36EC | 198 | 7 | cible de saut depuis LAB_0849<br>cible de saut depuis LAB_084A<br>cible de saut depuis LAB_084D<br>cible de saut depuis LAB_084E |
| `LAB_084D` | 4:$3706 | 830 | 23 | entrée 0 de la table LAB_084E+554<br>ombre ($AC) depuis LAB_0849<br>ombre ($AC) depuis LAB_084A<br>ombre ($AC) depuis LAB_084D<br>ombre ($AC) depuis LAB_084E |
| `LAB_084E` | 4:$370E | 822 | 22 | référencé par le code dans LAB_0206<br>référencé par le code dans LAB_0231 |
| `LAB_084F` | 4:$3A8C | 20 | 1 | objet+22 (repos) dans LAB_018F<br>objet+26 (réaction) dans LAB_018F |
| `LAB_0850` | 4:$3AA0 | 40 | 2 | référencé par le code dans LAB_0259 |
| `LAB_0851` | 4:$3AA8 | 32 | 1 | référencé par le code dans LAB_0268 |
| `LAB_0852` | 4:$3AC8 | 14 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 0<br>référencé par le code dans LAB_0158 |
| `LAB_0853` | 4:$3AD6 | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 1<br>référencé par le code dans LAB_0158 |
| `LAB_0854` | 4:$3ADE | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 2<br>référencé par le code dans LAB_0158 |
| `LAB_0855` | 4:$3AE6 | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 3<br>référencé par le code dans LAB_0158 |
| `LAB_0856` | 4:$3AEE | 84 | 5 | ombre ($AC) depuis LAB_084F<br>ombre ($AC) depuis LAB_0852 |
| `LAB_0857` | 4:$3AF6 | 76 | 4 | référencé par le code dans LAB_0254 |
| `LAB_0858` | 4:$3B42 | 162 | 7 | référencé par le code dans LAB_0276 |
| `LAB_0859` | 4:$3BE4 | 80 | 4 | référencé par le code dans LAB_0253 |
| `LAB_085A` | 4:$3C34 | 128 | 6 | référencé par le code dans LAB_0274 |
| `LAB_085B` | 4:$3CB4 | 20 | 1 | référencé par le code dans LAB_0265<br>référencé par le code dans LAB_0278 |
| `LAB_085C` | 4:$3CC8 | 286 | 8 | référencé par le code dans LAB_0267 |
| `LAB_085D` | 4:$3DE6 | 156 | 4 | référencé par le code dans LAB_0266 |
| `LAB_085E` | 4:$3E82 | 268 | 9 | référencé par le code dans LAB_01F0 |
| `LAB_085F` | 4:$3F04 | 194 | 7 | référencé par le code dans LAB_01EF |
| `LAB_0860` | 4:$3F3C | 200 | 9 | LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [4]<br>LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [7]<br>LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [8]<br>référencé par le code dans LAB_0158<br>référencé par le code dans LAB_026D |
| `LAB_0861` | 4:$3FC8 | 60 | 4 | cible de saut depuis LAB_0860<br>cible de saut depuis LAB_0862 |
| `LAB_0862` | 4:$4004 | 176 | 8 | LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [1]<br>LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [3]<br>LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [5]<br>LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [6]<br>référencé par le code dans LAB_0158<br>référencé par le code dans LAB_026F |
| `LAB_0863` | 4:$4078 | 20 | 1 | référencé par le code dans LAB_025E |
| `LAB_0864` | 4:$408C | 20 | 1 | référencé par le code dans LAB_025F |
| `LAB_0865` | 4:$40A0 | 136 | 4 | référencé par le code dans LAB_0279 |
| `LAB_0866` | 4:$4128 | 40 | 1 | cible de saut depuis LAB_0867<br>référencé par le code dans LAB_0262 |
| `LAB_0867` | 4:$4150 | 200 | 4 | référencé par le code dans LAB_0261 |
| `LAB_0868` | 4:$41F0 | 80 | 3 | référencé par le code dans LAB_0261 |
| `LAB_0869` | 4:$4240 | 32 | 1 | référencé par le code dans LAB_0262 |
| `LAB_086A` | 4:$4260 | 134 | 5 | LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [2]<br>référencé par le code dans LAB_0158 |
| `LAB_086B` | 4:$42B4 | 50 | 2 | cible de saut depuis LAB_086A<br>référencé par le code dans LAB_0270 |
| `LAB_086C` | 4:$42E6 | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 0<br>référencé par le code dans LAB_0158 |
| `LAB_086D` | 4:$42EE | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 1<br>référencé par le code dans LAB_0158 |
| `LAB_086E` | 4:$42F6 | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 2<br>référencé par le code dans LAB_0158 |
| `LAB_086F` | 4:$42FE | 8 | 1 | LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 3<br>référencé par le code dans LAB_0158 |
| `LAB_0870` | 4:$4306 | 26 | 1 | cible de saut depuis LAB_0870<br>référencé par le code dans LAB_018C |
| `LAB_0871` | 4:$4320 | 574 | 10 | cible de saut depuis LAB_087F |
| `LAB_0872` | 4:$455E | 370 | 6 | référencé par le code dans LAB_0286 |
| `LAB_0873` | 4:$46D0 | 58 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 0<br>référencé par le code dans LAB_015D |
| `LAB_0874` | 4:$470A | 56 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 1<br>référencé par le code dans LAB_015D |
| `LAB_0875` | 4:$4742 | 56 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 2<br>référencé par le code dans LAB_015D |
| `LAB_0876` | 4:$477A | 50 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 3<br>référencé par le code dans LAB_015D |
| `LAB_0877` | 4:$47AC | 46 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 4<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 5<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 6<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 7<br>référencé par le code dans LAB_015D |
| `LAB_0878` | 4:$47DA | 52 | 1 | écrit dans objet+22 (repos) par $A8 depuis LAB_0873 |
| `LAB_0879` | 4:$480E | 54 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 0<br>référencé par le code dans LAB_015D |
| `LAB_087A` | 4:$4844 | 50 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 1<br>référencé par le code dans LAB_015D |
| `LAB_087B` | 4:$4876 | 56 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 2<br>référencé par le code dans LAB_015D |
| `LAB_087C` | 4:$48AE | 56 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 3<br>référencé par le code dans LAB_015D |
| `LAB_087D` | 4:$48E6 | 50 | 1 | LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 4<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 5<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 6<br>LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 7<br>référencé par le code dans LAB_015D |
| `LAB_087E` | 4:$4918 | 260 | 3 | référencé par le code dans LAB_0284 |
| `LAB_087F` | 4:$4A1C | 628 | 11 | LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [1]<br>LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [2]<br>LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [3]<br>LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [4]<br>LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [5]<br>LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [6] … |
| `LAB_0880` | 4:$4A52 | 14 | 1 | entrée 0 de la table LAB_0881+50<br>objet+22 (repos) dans LAB_0192<br>objet+26 (réaction) dans LAB_0192 |
| `LAB_0881` | 4:$4A60 | 46 | 3 | référencé par le code dans LAB_0299 |
| `LAB_0882` | 4:$4AA4 | 62 | 1 | objet+22 (repos) dans LAB_0195<br>objet+26 (réaction) dans LAB_0195<br>écrit dans objet+22 (repos) par $A8 depuis LAB_0879 |
| `LAB_0883` | 4:$4B1A | 190 | 4 | référencé par le code dans LAB_0285 |
| `LAB_0884` | 4:$4BD8 | 466 | 7 | référencé par le code dans LAB_028D |
| `LAB_0885` | 4:$4DAA | 8 | 1 | ombre ($AC) depuis LAB_0878<br>ombre ($AC) depuis LAB_0882 |
| `LAB_0886` | 4:$4DB2 | 162 | 12 | référencé par le code dans LAB_0297 |
| `LAB_0887` | 4:$4E54 | 14 | 1 | référencé par le code dans LAB_0298 |
| `LAB_0888` | 4:$4E62 | 116 | 3 | cible de saut depuis LAB_0888<br>cible de saut depuis LAB_0889<br>cible de saut depuis LAB_088C<br>cible de saut depuis LAB_088D<br>cible de saut depuis LAB_088E<br>cible de saut depuis LAB_088F … |
| `LAB_0889` | 4:$4E90 | 116 | 3 | cible de saut depuis LAB_0888<br>cible de saut depuis LAB_0889<br>cible de saut depuis LAB_088C<br>cible de saut depuis LAB_088D<br>cible de saut depuis LAB_088E<br>cible de saut depuis LAB_088F … |
| `LAB_088A` | 4:$4EBC | 26 | 1 | référencé par le code dans LAB_02A8<br>référencé par le code dans LAB_02AA |
| `LAB_088B` | 4:$4ED6 | 32 | 1 | référencé par le code dans LAB_02A9 |
| `LAB_088C` | 4:$4EF6 | 260 | 9 | ombre ($AC) depuis LAB_0888<br>ombre ($AC) depuis LAB_0889<br>ombre ($AC) depuis LAB_088A<br>ombre ($AC) depuis LAB_088C<br>ombre ($AC) depuis LAB_088D<br>ombre ($AC) depuis LAB_088E … |
| `LAB_088D` | 4:$4EFE | 252 | 8 | référencé par le code dans LAB_02A2 |
| `LAB_088E` | 4:$4F4E | 172 | 5 | référencé par le code dans LAB_02B2 |
| `LAB_088F` | 4:$4F6A | 144 | 4 | cible de saut depuis LAB_0890<br>objet+26 (réaction) dans LAB_0198 |
| `LAB_0890` | 4:$4F86 | 274 | 8 | référencé par le code dans LAB_02A4 |
| `LAB_0891` | 4:$5008 | 486 | 14 | référencé par le code dans LAB_02B1 |
| `LAB_0892` | 4:$5042 | 428 | 12 | cible de saut depuis LAB_0891 |
| `LAB_0893` | 4:$51EE | 176 | 3 | référencé par le code dans LAB_02B4 |
| `LAB_0894` | 4:$529E | 112 | 2 | référencé par le code dans LAB_02B5 |
| `LAB_0895` | 4:$530E | 616 | 11 | référencé par le code dans LAB_02B6 |
| `LAB_0896` | 4:$55D6 | 978 | 12 | référencé par le code dans LAB_02B7 |
| `LAB_0897` | 4:$59A8 | 166 | 4 | référencé par le code dans LAB_0EA7 |
| `LAB_0898` | 4:$5A02 | 76 | 1 | référencé par le code dans LAB_0EA9 |
| `LAB_0899` | 4:$5A4E | 72 | 2 | référencé par le code dans LAB_0EAB |
| `LAB_089A` | 4:$5A70 | 38 | 1 | objet+22 (repos) dans LAB_019D<br>objet+26 (réaction) dans LAB_019D |
| `LAB_089B` | 4:$5A96 | 22 | 1 | référencé par le code dans LAB_015D |
| `LAB_089C` | 4:$5AAC | 34 | 1 | référencé par le code dans LAB_015D |
| `LAB_089D` | 4:$5ACE | 22 | 1 | référencé par le code dans LAB_015D |
| `LAB_089E` | 4:$5AE4 | 296 | 7 | référencé par le code dans LAB_0EA5 |
| `LAB_089F` | 4:$5C0C | 74 | 1 | référencé par le code dans LAB_0EAE<br>référencé par le code dans LAB_0EB4 |
| `LAB_08A0` | 4:$5C56 | 220 | 3 | référencé par le code dans LAB_0EAC |
| `LAB_08A1` | 4:$5D72 | 260 | 3 | référencé par le code dans LAB_0EB0 |
| `LAB_08A2` | 4:$5E76 | 276 | 6 | référencé par le code dans LAB_0EAA |
| `LAB_08A3` | 4:$5F8A | 156 | 5 | référencé par le code dans LAB_015C<br>référencé par le code dans LAB_0EB2<br>référencé par le code dans LAB_0EB5 |
| `LAB_08A4` | 4:$5FB4 | 114 | 4 | cible de saut depuis LAB_08A3 |
| `LAB_08A5` | 4:$6026 | 20 | 1 | cible de saut depuis LAB_08AA<br>objet+22 (repos) dans LAB_019F<br>objet+26 (réaction) dans LAB_019F |
| `LAB_08A6` | 4:$603A | 20 | 1 | référencé par le code dans LAB_015B |
| `LAB_08A7` | 4:$604E | 20 | 1 | référencé par le code dans LAB_015B |
| `LAB_08A8` | 4:$6062 | 20 | 1 | référencé par le code dans LAB_015B |
| `LAB_08A9` | 4:$6076 | 20 | 1 | référencé par le code dans LAB_015B |
| `LAB_08AA` | 4:$608A | 102 | 4 | référencé par le code dans LAB_0EC9 |
| `LAB_08AB` | 4:$60DC | 246 | 6 | référencé par le code dans LAB_0ECA |
| `LAB_08AC` | 4:$61D2 | 144 | 5 | référencé par le code dans LAB_0ECC |
| `LAB_08AD` | 4:$6226 | 60 | 3 | cible de saut depuis LAB_08AC |
| `LAB_08AE` | 4:$6262 | 462 | 11 | objet+22 (repos) dans LAB_01A0 |
| `LAB_08AF` | 4:$6404 | 168 | 5 | cible de saut depuis LAB_08AF<br>référencé par le code dans LAB_0EEB |
| `LAB_08B0` | 4:$647E | 46 | 1 | cible de saut depuis LAB_08AE<br>cible de saut depuis LAB_08B4<br>cible de saut depuis LAB_08B5<br>objet+26 (réaction) dans LAB_01A0<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08B3 |
| `LAB_08B1` | 4:$64AC | 46 | 1 | cible de saut depuis LAB_08B4<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08AE<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08AF<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08B0<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08B4<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08B5 |
| `LAB_08B2` | 4:$64DA | 46 | 1 | écrit dans objet+22 (repos) par $A8 depuis LAB_08B1<br>écrit dans objet+22 (repos) par $A8 depuis LAB_08B4 |
| `LAB_08B3` | 4:$6508 | 46 | 1 | écrit dans objet+22 (repos) par $A8 depuis LAB_08B2 |
| `LAB_08B4` | 4:$6536 | 1236 | 19 | référencé par le code dans LAB_0EDD |
| `LAB_08B5` | 4:$6640 | 924 | 13 | référencé par le code dans LAB_0EDE |
| `LAB_08B6` | 4:$68B4 | 250 | 5 | référencé par le code dans LAB_0EDF |
| `LAB_08B7` | 4:$69AE | 52 | 1 | référencé par le code dans LAB_0EE5 |
| `LAB_08B8` | 4:$69E2 | 152 | 3 | référencé par le code dans LAB_0EE1 |
| `LAB_08B9` | 4:$6A7A | 58 | 1 | référencé par le code dans LAB_0EE7 |
| `LAB_08BA` | 4:$6AB4 | 158 | 3 | référencé par le code dans LAB_0EE3 |
| `LAB_08BB` | 4:$6B52 | 76 | 1 | référencé par le code dans LAB_0EE7 |
| `LAB_08BC` | 4:$6B9E | 138 | 2 | référencé par le code dans LAB_0EE5 |
| `LAB_08BD` | 4:$6C28 | 360 | 10 | référencé par le code dans LAB_0EF2 |
| `LAB_08BE` | 4:$6CC0 | 208 | 7 | cible de saut depuis LAB_08BD |
| `LAB_08BF` | 4:$6D90 | 258 | 5 | référencé par le code dans LAB_0F34 |
| `LAB_08CC` | 4:$6F82 | 16 | 2 | référencé par le code dans LAB_0203<br>référencé par le code dans LAB_029C<br>référencé par le code dans LAB_02A2<br>référencé par le code dans LAB_0EC9<br>référencé par le code dans LAB_0EDD |
| `LAB_08FD` | 4:$7BA8 | 8 | 1 | entrée 0 de la table LAB_08FC<br>entrée 1 de la table LAB_08FC |
| `LAB_08FE` | 4:$7BB0 | 8 | 1 | entrée 2 de la table LAB_08FC<br>entrée 3 de la table LAB_08FC |
| `LAB_08FF` | 4:$7BB8 | 8 | 1 | entrée 4 de la table LAB_08FC<br>entrée 5 de la table LAB_08FC |
| `LAB_0900` | 4:$7BC0 | 8 | 1 | entrée 6 de la table LAB_08FC<br>entrée 7 de la table LAB_08FC<br>référencé par le code dans LAB_0DD0 |
| `LAB_0901` | 4:$7BC8 | 8 | 1 | entrée 8 de la table LAB_08FC<br>entrée 9 de la table LAB_08FC |
| `LAB_0902` | 4:$7BD0 | 8 | 1 | entrée 10 de la table LAB_08FC<br>entrée 11 de la table LAB_08FC |
| `LAB_0903` | 4:$7BD8 | 8 | 1 | entrée 12 de la table LAB_08FC<br>entrée 13 de la table LAB_08FC |
| `LAB_0904` | 4:$7BE0 | 8 | 1 | entrée 14 de la table LAB_08FC<br>entrée 15 de la table LAB_08FC |
| `LAB_0975` | 4:$8F20 | 160 | 4 | référencé par le code dans LAB_049B |
| `LAB_0978` | 4:$8F2C | 148 | 4 | cible de saut depuis LAB_0975<br>cible de saut depuis LAB_0978<br>référencé par le code dans LAB_0457 |
| `LAB_0979` | 4:$8FC0 | 30 | 3 | cible de saut depuis LAB_0979<br>référencé par le code dans LAB_0457 |
| `LAB_097A` | 4:$8FDE | 48 | 3 | cible de saut depuis LAB_097A<br>référencé par le code dans LAB_0456<br>référencé par le code dans LAB_0458 |
| `LAB_0D44` | 31:$35A | 122 | 1 | entrée 0 de la table LAB_0D43+146 |
| `LAB_0D45` | 31:$364 | 44 | 1 | entrée 1 de la table LAB_0D43+146 |
| `LAB_0D46` | 31:$376 | 26 | 1 | entrée 2 de la table LAB_0D43+146 |
| `LAB_0D47` | 31:$390 | 68 | 1 | entrée 3 de la table LAB_0D43+146 |
| `LAB_0D48` | 31:$3B2 | 68 | 1 | entrée 4 de la table LAB_0D43+146 |
| `LAB_0D49` | 31:$3D4 | 68 | 1 | entrée 5 de la table LAB_0D43+146 |
| `LAB_0D4A` | 31:$3F6 | 68 | 1 | entrée 6 de la table LAB_0D43+146 |
| `LAB_0F54` | 42:$116 | 16 | 2 | référencé par le code dans LAB_04AB<br>référencé par le code dans LAB_04AD |
| `LAB_0F55` | 42:$126 | 222 | 14 | référencé par le code dans LAB_04B0 |
| `LAB_0F56` | 42:$204 | 276 | 14 | référencé par le code dans LAB_04BF |
| `LAB_0F57` | 42:$318 | 192 | 3 | cible de saut depuis LAB_0F57<br>référencé par le code dans LAB_04BF |
| `L44_00C70` | 44:$C70 | 386 | 1 | SECSTRT_44 [table SECSTRT_44] : [6]<br>entrée 0 de la table LAB_0F8E<br>référencé par le code dans LAB_0F8A |
| `LAB_10A2` | 45:$A8 | 2 | 1 | référencé par le code dans LAB_0F8D<br>référencé par le code dans LAB_0F90<br>référencé par le code dans LAB_0F94<br>référencé par le code dans LAB_0FBC |
| `LAB_10AE` | 45:$7D2 | 8 | 1 | référencé par le code dans LAB_0FDE |

## Décodage

### `LAB_024E` (0:$5E82)

Rôles : référencé par le code dans LAB_023B ; référencé par le code dans LAB_023C

```
  ; étape 1
    00 00 FF FF 00 07          ; dessin banque 0 frame 0 dy -1 dx 7 [corps,frappe,décor,hors-boîte]
    00 01 00 17 00 00          ; dessin banque 0 frame 1 dy 0 dx 0 [corps,frappe,décor]
    00 00 FF FF 00 07          ; dessin banque 0 frame 0 dy -1 dx 7 [corps,frappe,décor,hors-boîte]
    00 01 00 17 00 00          ; dessin banque 0 frame 1 dy 0 dx 0 [corps,frappe,décor]
  LAB_024F:
    00 00 00 0A 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 03 00 01 00 07          ; dessin banque 0 frame 3 dy 0 dx 7 [corps]
    FF FF                      ; fin du script
```

### `LAB_024F` (0:$5E9A)

Rôles : référencé par le code dans LAB_0239

```
  ; étape 1
    00 00 00 0A 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 03 00 01 00 07          ; dessin banque 0 frame 3 dy 0 dx 7 [corps]
    FF FF                      ; fin du script
```

### `LAB_04F5` (0:$B026)

Rôles : référencé par le code dans LAB_04F8

```
  ; étape 1
    00 29 00 59 00 23          ; dessin banque 0 frame 41 dy 0 dx 35 [corps,décor,hors-boîte]
    00 00 00 2A 00 59          ; dessin banque 0 frame 0 dy 0 dx 89 [frappe]
    00 2A 00 00 00 2B          ; dessin banque 0 frame 42 dy 0 dx 43
    00 59 00 31 00 00          ; dessin banque 0 frame 89 dy 0 dx 0 [corps,décor]
    FF FF                      ; fin du script
```

### `LAB_07DB` (4:$1064)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 0 : feu + neutre ou D+G ou D+G+B ou D+G+H ou G+B+H ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [0] ; cible de saut depuis LAB_07E9 ; cible de saut depuis LAB_07EA ; cible de saut depuis LAB_07ED ; cible de saut depuis LAB_07EE ; cible de saut depuis LAB_07FA ; cible de saut depuis LAB_07FB ; cible de saut depuis LAB_07FE ; cible de saut depuis LAB_0849 ; cible de saut depuis LAB_084A ; cible de saut depuis LAB_084B ; cible de saut depuis LAB_084C ; cible de saut depuis LAB_084D ; cible de saut depuis LAB_084E ; cible de saut depuis LAB_085E ; cible de saut depuis LAB_085F ; objet+22 (repos) dans LAB_0167 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
```

### `LAB_07DC` (4:$1080)

Rôles : LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [4] ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [7] ; objet+26 (réaction) dans LAB_0167 ; référencé par le code dans LAB_0155

```
  ; étape 1
    D0 00                      ; $D0 Reset
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 00 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 01 2D 00 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 00 00 04          ; dessin banque 3 frame 1 dy -6 dx 4
    FF FF                      ; fin du script
```

### `LAB_07DD` (4:$10AC)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 0 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 03 2E 00 FF F3          ; dessin banque 0 frame 3 dy 46 dx -13
    00 02 F7 01 FF F7          ; dessin banque 0 frame 2 dy -9 dx -9 [corps]
    0C 01 FA 00 00 03          ; dessin banque 3 frame 1 dy -6 dx 3
    FF FF                      ; fin du script
```

### `LAB_07DE` (4:$10C0)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 1 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 05 23 01 FF F3          ; dessin banque 0 frame 5 dy 35 dx -13 [corps]
    00 04 F7 01 FF FD          ; dessin banque 0 frame 4 dy -9 dx -3 [corps]
    0C 01 FA 00 00 06          ; dessin banque 3 frame 1 dy -6 dx 6
    FF FF                      ; fin du script
```

### `LAB_07DF` (4:$10D4)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 2 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 07 18 01 FF F3          ; dessin banque 0 frame 7 dy 24 dx -13 [corps]
    00 06 F8 01 FF F7          ; dessin banque 0 frame 6 dy -8 dx -9 [corps]
    0C 01 FA 00 00 01          ; dessin banque 3 frame 1 dy -6 dx 1
    FF FF                      ; fin du script
```

### `LAB_07E0` (4:$10E8)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche horizontal phase 3 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 09 1B 01 FF F3          ; dessin banque 0 frame 9 dy 27 dx -13 [corps]
    00 08 F7 01 FF FC          ; dessin banque 0 frame 8 dy -9 dx -4 [corps]
    0C 01 F7 00 00 0C          ; dessin banque 3 frame 1 dy -9 dx 12
    FF FF                      ; fin du script
```

### `LAB_07E1` (4:$10FC)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 0 ; référencé par le code dans LAB_0155

```
  ; étape 1
    0C 00 ED 00 00 02          ; dessin banque 3 frame 0 dy -19 dx 2
    00 0A F8 01 FF F6          ; dessin banque 0 frame 10 dy -8 dx -10 [corps]
    FF FF                      ; fin du script
```

### `LAB_07E2` (4:$110A)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 1 ; référencé par le code dans LAB_0155

```
  ; étape 1
    0C 00 EC 00 00 03          ; dessin banque 3 frame 0 dy -20 dx 3
    00 0B F6 01 FF F7          ; dessin banque 0 frame 11 dy -10 dx -9 [corps]
    FF FF                      ; fin du script
```

### `LAB_07E3` (4:$1118)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 2 ; référencé par le code dans LAB_0155

```
  ; étape 1
    0C 00 EE 00 00 03          ; dessin banque 3 frame 0 dy -18 dx 3
    00 0C F8 01 FF F7          ; dessin banque 0 frame 12 dy -8 dx -9 [corps]
    FF FF                      ; fin du script
```

### `LAB_07E4` (4:$1126)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche haut phase 3 ; référencé par le code dans LAB_0155

```
  ; étape 1
    0C 00 EE 00 00 02          ; dessin banque 3 frame 0 dy -18 dx 2
    00 0D F8 01 FF F7          ; dessin banque 0 frame 13 dy -8 dx -9 [corps]
    FF FF                      ; fin du script
```

### `LAB_07E5` (4:$1134)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 0 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 0E F8 01 FF F4          ; dessin banque 0 frame 14 dy -8 dx -12 [corps]
    0C 00 F2 00 FF FB          ; dessin banque 3 frame 0 dy -14 dx -5
    FF FF                      ; fin du script
```

### `LAB_07E6` (4:$1142)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 1 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 0F F9 01 FF F6          ; dessin banque 0 frame 15 dy -7 dx -10 [corps]
    0C 00 F2 00 FF FE          ; dessin banque 3 frame 0 dy -14 dx -2
    FF FF                      ; fin du script
```

### `LAB_07E7` (4:$1150)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 2 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 10 F8 01 FF F3          ; dessin banque 0 frame 16 dy -8 dx -13 [corps]
    0C 00 F1 00 FF FB          ; dessin banque 3 frame 0 dy -15 dx -5
    FF FF                      ; fin du script
```

### `LAB_07E8` (4:$115E)

Rôles : LAB_0610 [objet+46 (marche) ; posée par LAB_0167] : marche bas phase 3 ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 11 F8 01 FF F4          ; dessin banque 0 frame 17 dy -8 dx -12 [corps]
    0C 00 EF 00 FF FA          ; dessin banque 3 frame 0 dy -17 dx -6
    FF FF                      ; fin du script
```

### `LAB_07E9` (4:$116C)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 5 : feu + G ou B+H ; référencé par le code dans LAB_0155

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    A4 0B                      ; $A4 Sound
    00 16 2D 00 FF ED          ; dessin banque 0 frame 22 dy 45 dx -19
    00 15 13 01 FF EF          ; dessin banque 0 frame 21 dy 19 dx -17 [corps]
    00 14 F8 01 FF FA          ; dessin banque 0 frame 20 dy -8 dx -6 [corps]
    0C 02 09 02 00 0E          ; dessin banque 3 frame 2 dy 9 dx 14 [frappe]
    0C 0B FA 02 00 1B          ; dessin banque 3 frame 11 dy -6 dx 27 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    00 18 2E 00 FF D5          ; dessin banque 0 frame 24 dy 46 dx -43
    00 17 F9 01 FF E9          ; dessin banque 0 frame 23 dy -7 dx -23 [corps]
    0C 0C 13 02 FF CB          ; dessin banque 3 frame 12 dy 19 dx -53 [frappe]
    0C 18 11 02 FF CA          ; dessin banque 3 frame 24 dy 17 dx -54 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    88 03                      ; $88 Hold
    00 18 2E 00 FF D5          ; dessin banque 0 frame 24 dy 46 dx -43
    00 17 F9 01 FF E9          ; dessin banque 0 frame 23 dy -7 dx -23 [corps]
    0C 18 11 02 FF CA          ; dessin banque 3 frame 24 dy 17 dx -54 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    84 00 00 00 10 64          ; $84 Jump -> LAB_07DB
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 00 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12
    FF FF                      ; fin du script
```

### `LAB_07EA` (4:$11F2)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 3 : feu + G+H ; référencé par le code dans LAB_0155

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  ; étape 2
    C8 01 00 4C 00 00 10 64    ; $C8 IfFieldZero -> LAB_07DB
    88 03                      ; $88 Hold
    00 1B 10 01 FF F2          ; dessin banque 0 frame 27 dy 16 dx -14 [corps]
    00 19 F2 01 FF EC          ; dessin banque 0 frame 25 dy -14 dx -20 [corps]
    00 1A 06 00 00 00          ; dessin banque 0 frame 26 dy 6 dx 0
    0C 00 EB 00 00 0B          ; dessin banque 3 frame 0 dy -21 dx 11
    0C 09 E9 00 FF E3          ; dessin banque 3 frame 9 dy -23 dx -29
    FF 00                      ; fin d'étape
  ; étape 3
    88 01                      ; $88 Hold
    00 1B 10 01 FF F2          ; dessin banque 0 frame 27 dy 16 dx -14 [corps]
    00 19 F2 01 FF EC          ; dessin banque 0 frame 25 dy -14 dx -20 [corps]
    00 1A 06 00 00 00          ; dessin banque 0 frame 26 dy 6 dx 0
    0C 00 EB 00 00 0B          ; dessin banque 3 frame 0 dy -21 dx 11
    0C 09 E9 00 FF E3          ; dessin banque 3 frame 9 dy -23 dx -29
    FF 00                      ; fin d'étape
  ; étape 4
    A4 05                      ; $A4 Sound
    B0 00 00 00 6F FC          ; $B0 Call -> LAB_02CA
    00 1E 2D 00 FF F6          ; dessin banque 0 frame 30 dy 45 dx -10
    00 1C 14 01 FF F8          ; dessin banque 0 frame 28 dy 20 dx -8 [corps]
    00 1D FB 01 00 03          ; dessin banque 0 frame 29 dy -5 dx 3 [corps]
    0C 0A 07 02 00 3F          ; dessin banque 3 frame 10 dy 7 dx 63 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07EB` (4:$1260)

Rôles : référencé par le code dans LAB_02CA

```
  ; étape 1
    A4 0B                      ; $A4 Sound
    A0 01 00 05 00 00 00 00    ; $A0 Move
    0C 0A 08 02 00 42          ; dessin banque 3 frame 10 dy 8 dx 66 [frappe]
    0C 0D 08 00 00 0C          ; dessin banque 3 frame 13 dy 8 dx 12
    FF FF                      ; fin du script
```

### `LAB_07EC` (4:$1278)

Rôles : référencé par le code dans LAB_02CB

```
  ; étape 1
    A0 01 00 14 00 00 00 00    ; $A0 Move
    0C 0A 08 02 00 3D          ; dessin banque 3 frame 10 dy 8 dx 61 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07ED` (4:$1288)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 2 : feu + D ou D+B+H ; référencé par le code dans LAB_0155

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    88 02                      ; $88 Hold
    00 20 2C 01 FF EA          ; dessin banque 0 frame 32 dy 44 dx -22 [corps]
    00 1F FC 01 FF EE          ; dessin banque 0 frame 31 dy -4 dx -18 [corps]
    0C 03 19 02 FF EA          ; dessin banque 3 frame 3 dy 25 dx -22 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    A4 0B                      ; $A4 Sound
    88 01                      ; $88 Hold
    00 22 FD 01 00 04          ; dessin banque 0 frame 34 dy -3 dx 4 [corps]
    00 21 18 01 FF F2          ; dessin banque 0 frame 33 dy 24 dx -14 [corps]
    0C 04 15 02 00 0F          ; dessin banque 3 frame 4 dy 21 dx 15 [frappe]
    0C 0E 22 00 00 0A          ; dessin banque 3 frame 14 dy 34 dx 10
    FF 00                      ; fin d'étape
  ; étape 4
    00 23 1B 01 FF F2          ; dessin banque 0 frame 35 dy 27 dx -14 [corps]
    00 24 07 01 00 11          ; dessin banque 0 frame 36 dy 7 dx 17 [corps]
    00 25 16 01 00 23          ; dessin banque 0 frame 37 dy 22 dx 35 [corps]
    00 26 2D 00 00 21          ; dessin banque 0 frame 38 dy 45 dx 33
    0C 05 17 02 00 3C          ; dessin banque 3 frame 5 dy 23 dx 60 [frappe]
    0C 0F 19 02 00 29          ; dessin banque 3 frame 15 dy 25 dx 41 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    00 27 14 01 FF F4          ; dessin banque 0 frame 39 dy 20 dx -12 [corps]
    0C 01 F9 02 00 12          ; dessin banque 3 frame 1 dy -7 dx 18 [frappe]
    00 28 02 01 00 0F          ; dessin banque 0 frame 40 dy 2 dx 15 [corps]
    0C 10 FA 02 00 22          ; dessin banque 3 frame 16 dy -6 dx 34 [frappe]
    0C 11 03 02 00 3D          ; dessin banque 3 frame 17 dy 3 dx 61 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    88 01                      ; $88 Hold
    00 29 FF 01 FF F3          ; dessin banque 0 frame 41 dy -1 dx -13 [corps]
    00 2A 15 01 00 0F          ; dessin banque 0 frame 42 dy 21 dx 15 [corps]
    0C 12 EB 02 FF E9          ; dessin banque 3 frame 18 dy -21 dx -23 [frappe]
    0C 06 EA 02 FF E7          ; dessin banque 3 frame 6 dy -22 dx -25 [frappe]
    FF 00                      ; fin d'étape
  ; étape 7
    00 29 FF 01 FF F3          ; dessin banque 0 frame 41 dy -1 dx -13 [corps]
    00 2A 15 01 00 0F          ; dessin banque 0 frame 42 dy 21 dx 15 [corps]
    0C 06 EA 02 FF E7          ; dessin banque 3 frame 6 dy -22 dx -25 [frappe]
    FF 00                      ; fin d'étape
  ; étape 8
    84 00 00 00 10 64          ; $84 Jump -> LAB_07DB
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07EE` (4:$1352)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 8 : feu + H ; référencé par le code dans LAB_0155

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  ; étape 2
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    A4 13                      ; $A4 Sound
    88 03                      ; $88 Hold
    00 2B F8 01 FF F0          ; dessin banque 0 frame 43 dy -8 dx -16 [corps]
    0C 07 EF 02 FF D3          ; dessin banque 3 frame 7 dy -17 dx -45 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    A4 0B                      ; $A4 Sound
    00 2D 19 01 FF F2          ; dessin banque 0 frame 45 dy 25 dx -14 [corps]
    00 2C EF 01 FF FE          ; dessin banque 0 frame 44 dy -17 dx -2 [corps]
    0C 00 D0 02 00 04          ; dessin banque 3 frame 0 dy -48 dx 4 [frappe]
    0C 14 D0 02 FF F9          ; dessin banque 3 frame 20 dy -48 dx -7 [frappe]
    0C 13 D4 02 FF E0          ; dessin banque 3 frame 19 dy -44 dx -32 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    00 2E 15 01 FF F4          ; dessin banque 0 frame 46 dy 21 dx -12 [corps]
    00 25 0A 01 00 1A          ; dessin banque 0 frame 37 dy 10 dx 26 [corps]
    00 2F FB 01 00 0C          ; dessin banque 0 frame 47 dy -5 dx 12 [corps]
    0C 05 0B 02 00 31          ; dessin banque 3 frame 5 dy 11 dx 49 [frappe]
    0C 16 EB 02 00 47          ; dessin banque 3 frame 22 dy -21 dx 71 [frappe]
    0C 15 D9 02 00 2D          ; dessin banque 3 frame 21 dy -39 dx 45 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    A4 03                      ; $A4 Sound
    00 30 19 01 FF FA          ; dessin banque 0 frame 48 dy 25 dx -6 [corps]
    00 31 0D 01 00 15          ; dessin banque 0 frame 49 dy 13 dx 21 [corps]
    00 32 03 01 00 24          ; dessin banque 0 frame 50 dy 3 dx 36 [corps]
    0C 08 25 02 00 34          ; dessin banque 3 frame 8 dy 37 dx 52 [frappe]
    FF 00                      ; fin d'étape
  ; étape 7
    00 30 19 01 FF FA          ; dessin banque 0 frame 48 dy 25 dx -6 [corps]
    00 31 0D 01 00 15          ; dessin banque 0 frame 49 dy 13 dx 21 [corps]
    00 33 05 01 00 24          ; dessin banque 0 frame 51 dy 5 dx 36 [corps]
    0C 08 25 02 00 34          ; dessin banque 3 frame 8 dy 37 dx 52 [frappe]
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    84 00 00 00 10 64          ; $84 Jump -> LAB_07DB
    00 30 19 01 FF FA          ; dessin banque 0 frame 48 dy 25 dx -6 [corps]
    00 31 0D 01 00 15          ; dessin banque 0 frame 49 dy 13 dx 21 [corps]
    00 34 0C 01 00 22          ; dessin banque 0 frame 52 dy 12 dx 34 [corps]
    0C 08 25 02 00 34          ; dessin banque 3 frame 8 dy 37 dx 52 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07EF` (4:$1418)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 1 : feu + D+B ; référencé par le code dans LAB_0155

```
  ; étape 1
    88 04                      ; $88 Hold
    00 35 F9 01 FF EE          ; dessin banque 0 frame 53 dy -7 dx -18 [corps]
    0C 05 1A 02 FF F9          ; dessin banque 3 frame 5 dy 26 dx -7 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    00 23 1B 01 FF F2          ; dessin banque 0 frame 35 dy 27 dx -14 [corps]
    00 24 07 01 00 11          ; dessin banque 0 frame 36 dy 7 dx 17 [corps]
    00 25 16 01 00 23          ; dessin banque 0 frame 37 dy 22 dx 35 [corps]
    00 26 2D 00 00 21          ; dessin banque 0 frame 38 dy 45 dx 33
    0C 05 17 02 00 3C          ; dessin banque 3 frame 5 dy 23 dx 60 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    00 36 19 01 FF F4          ; dessin banque 0 frame 54 dy 25 dx -12 [corps]
    00 37 0A 01 00 1A          ; dessin banque 0 frame 55 dy 10 dx 26 [corps]
    00 38 19 00 00 32          ; dessin banque 0 frame 56 dy 25 dx 50
    00 26 2E 00 00 28          ; dessin banque 0 frame 38 dy 46 dx 40
    0C 05 1A 02 00 42          ; dessin banque 3 frame 5 dy 26 dx 66 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    00 36 19 01 FF F4          ; dessin banque 0 frame 54 dy 25 dx -12 [corps]
    00 39 0A 01 00 1A          ; dessin banque 0 frame 57 dy 10 dx 26 [corps]
    00 3A 19 00 00 3A          ; dessin banque 0 frame 58 dy 25 dx 58
    00 26 2E 00 00 28          ; dessin banque 0 frame 38 dy 46 dx 40
    0C 05 1B 02 00 42          ; dessin banque 3 frame 5 dy 27 dx 66 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    00 36 19 01 FF F4          ; dessin banque 0 frame 54 dy 25 dx -12 [corps]
    00 3B 12 01 00 1A          ; dessin banque 0 frame 59 dy 18 dx 26 [corps]
    00 38 1A 00 00 31          ; dessin banque 0 frame 56 dy 26 dx 49
    00 26 2E 00 00 28          ; dessin banque 0 frame 38 dy 46 dx 40
    0C 05 1B 02 00 41          ; dessin banque 3 frame 5 dy 27 dx 65 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    88 03                      ; $88 Hold
    00 35 F9 01 FF EE          ; dessin banque 0 frame 53 dy -7 dx -18 [corps]
    0C 05 1A 02 FF F9          ; dessin banque 3 frame 5 dy 26 dx -7 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07F0` (4:$14BE)

Rôles : objet+34 [objet+34 (attaques)] : attaque 7 : feu + B ou D+G+B+H ; référencé par le code dans LAB_018C

```
  ; étape 1
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    04 00 F3 01 FF EF          ; dessin banque 1 frame 0 dy -13 dx -17 [corps]
    04 01 26 01 FF EF          ; dessin banque 1 frame 1 dy 38 dx -17 [corps]
    0C 02 F4 02 FF F8          ; dessin banque 3 frame 2 dy -12 dx -8 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    04 00 F3 01 FF EF          ; dessin banque 1 frame 0 dy -13 dx -17 [corps]
    04 01 26 01 FF EF          ; dessin banque 1 frame 1 dy 38 dx -17 [corps]
    0C 02 F4 02 FF F8          ; dessin banque 3 frame 2 dy -12 dx -8 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    88 03                      ; $88 Hold
    04 02 0E 01 FF F1          ; dessin banque 1 frame 2 dy 14 dx -15 [corps]
    0C 19 1E 02 00 12          ; dessin banque 3 frame 25 dy 30 dx 18 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07F1` (4:$14FE)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 6 : feu + D+H ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    88 01                      ; $88 Hold
    04 03 16 01 FF F1          ; dessin banque 1 frame 3 dy 22 dx -15 [corps]
    04 04 FE 01 00 05          ; dessin banque 1 frame 4 dy -2 dx 5 [corps]
    04 05 F7 01 00 14          ; dessin banque 1 frame 5 dy -9 dx 20 [corps]
    0C 01 DD 02 00 1D          ; dessin banque 3 frame 1 dy -35 dx 29 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    88 04                      ; $88 Hold
    04 03 16 01 FF F1          ; dessin banque 1 frame 3 dy 22 dx -15 [corps]
    04 04 FE 01 00 05          ; dessin banque 1 frame 4 dy -2 dx 5 [corps]
    04 05 F7 01 00 14          ; dessin banque 1 frame 5 dy -9 dx 20 [corps]
    0C 01 DD 02 00 1D          ; dessin banque 3 frame 1 dy -35 dx 29 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    88 01                      ; $88 Hold
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07F2` (4:$1566)

Rôles : objet+34 [objet+34 (attaques)] : attaque 4 : feu + G+B ; référencé par le code dans LAB_018C

```
  ; étape 1
    88 01                      ; $88 Hold
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    04 0F 08 00 FF FD          ; dessin banque 1 frame 15 dy 8 dx -3
    04 0B E1 01 00 02          ; dessin banque 1 frame 11 dy -31 dx 2 [corps]
    0C 00 C8 02 00 03          ; dessin banque 3 frame 0 dy -56 dx 3 [frappe]
    04 10 1E 00 FF F1          ; dessin banque 1 frame 16 dy 30 dx -15
    04 0C F8 00 FF F2          ; dessin banque 1 frame 12 dy -8 dx -14
    FF 00                      ; fin d'étape
  ; étape 3
    88 03                      ; $88 Hold
    04 0F 08 00 FF FD          ; dessin banque 1 frame 15 dy 8 dx -3
    04 0B E1 01 00 02          ; dessin banque 1 frame 11 dy -31 dx 2 [corps]
    0C 00 C8 02 00 03          ; dessin banque 3 frame 0 dy -56 dx 3 [frappe]
    04 10 1E 00 FF F1          ; dessin banque 1 frame 16 dy 30 dx -15
    04 0C F8 00 FF F2          ; dessin banque 1 frame 12 dy -8 dx -14
    FF 00                      ; fin d'étape
  ; étape 4
    00 13 2C 01 FF EB          ; dessin banque 0 frame 19 dy 44 dx -21 [corps]
    00 12 F9 01 FF F2          ; dessin banque 0 frame 18 dy -7 dx -14 [corps]
    0C 01 F1 02 00 0C          ; dessin banque 3 frame 1 dy -15 dx 12 [frappe]
    FF FF                      ; fin du script
```

### `LAB_07F3` (4:$15D8)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 7 : feu + B ou D+G+B+H ; référencé par le code dans LAB_0155 ; référencé par le code dans LAB_0175 ; référencé par le code dans LAB_0188 ; référencé par le code dans LAB_01F6

```
  ; étape 1
    04 08 2D 00 FF E9          ; dessin banque 1 frame 8 dy 45 dx -23
    04 07 14 01 FF EF          ; dessin banque 1 frame 7 dy 20 dx -17 [corps]
    04 06 FB 01 FF E0          ; dessin banque 1 frame 6 dy -5 dx -32 [corps]
    0C 00 EA 00 FF E0          ; dessin banque 3 frame 0 dy -22 dx -32
    FF FF                      ; fin du script
```

### `LAB_07F4` (4:$15F2)

Rôles : LAB_05F5 [objet+34 (attaques) ; posée par LAB_0167] : attaque 4 : feu + G+B ; référencé par le code dans LAB_0155

```
  ; étape 1
    00 16 2D 01 FF ED          ; dessin banque 0 frame 22 dy 45 dx -19 [corps]
    00 15 13 01 FF EF          ; dessin banque 0 frame 21 dy 19 dx -17 [corps]
    00 14 F8 01 FF FA          ; dessin banque 0 frame 20 dy -8 dx -6 [corps]
    0C 02 09 00 00 0E          ; dessin banque 3 frame 2 dy 9 dx 14
    FF FF                      ; fin du script
```

### `LAB_07F5` (4:$160C)

Rôles : LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [3] ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [6] ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [8] ; objet+30 [objet+30 (scripts objet+30)] : [1] ; référencé par le code dans LAB_0155 ; référencé par le code dans LAB_0192

```
  ; étape 1
    D0 00                      ; $D0 Reset
    A4 12                      ; $A4 Sound
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    08 00 F8 00 FF E7          ; dessin banque 2 frame 0 dy -8 dx -25
    08 01 03 80 FF DF          ; dessin banque 2 frame 1 dy 3 dx -33
    0C 00 F2 00 00 02          ; dessin banque 3 frame 0 dy -14 dx 2
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    08 03 2E 00 FF E7          ; dessin banque 2 frame 3 dy 46 dx -25
    08 02 05 00 FF F1          ; dessin banque 2 frame 2 dy 5 dx -15
    08 04 02 80 FF DD          ; dessin banque 2 frame 4 dy 2 dx -35
    0C 01 FE 00 FF F9          ; dessin banque 3 frame 1 dy -2 dx -7
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    08 03 2E 00 FF E7          ; dessin banque 2 frame 3 dy 46 dx -25
    08 02 05 00 FF F1          ; dessin banque 2 frame 2 dy 5 dx -15
    08 05 13 80 FF D3          ; dessin banque 2 frame 5 dy 19 dx -45
    08 06 00 80 FF F1          ; dessin banque 2 frame 6 dy 0 dx -15
    08 11 31 90 FF C3          ; dessin banque 2 frame 17 dy 49 dx -61 [décor]
    0C 01 FE 00 FF F9          ; dessin banque 3 frame 1 dy -2 dx -7
    FF 00                      ; fin d'étape
  ; étape 4
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    88 02                      ; $88 Hold
    08 03 2E 00 FF E7          ; dessin banque 2 frame 3 dy 46 dx -25
    08 02 05 00 FF F1          ; dessin banque 2 frame 2 dy 5 dx -15
    08 07 04 80 FF E9          ; dessin banque 2 frame 7 dy 4 dx -23
    08 08 FE 80 FF F1          ; dessin banque 2 frame 8 dy -2 dx -15
    08 11 31 90 FF C3          ; dessin banque 2 frame 17 dy 49 dx -61 [décor]
    0C 01 FE 00 FF F9          ; dessin banque 3 frame 1 dy -2 dx -7
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 5
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 6
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 7
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
```

### `LAB_07F6` (4:$169C)

Rôles : LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [1] ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [2] ; LAB_05F6 [objet+30 (scripts objet+30) ; posée par LAB_0167] : [5] ; référencé par le code dans LAB_0155

```
  ; étape 1
    D0 00                      ; $D0 Reset
    A4 12                      ; $A4 Sound
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    08 0A FE 00 FF E6          ; dessin banque 2 frame 10 dy -2 dx -26
    08 09 F4 00 FF F0          ; dessin banque 2 frame 9 dy -12 dx -16
    08 0B 0D 80 FF E6          ; dessin banque 2 frame 11 dy 13 dx -26
    0C 00 E7 00 00 05          ; dessin banque 3 frame 0 dy -25 dx 5
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    08 0C 00 00 FF DA          ; dessin banque 2 frame 12 dy 0 dx -38
    08 0D 15 80 FF E5          ; dessin banque 2 frame 13 dy 21 dx -27
    08 0E 18 80 FF FD          ; dessin banque 2 frame 14 dy 24 dx -3
    0C 00 F1 00 FF FF          ; dessin banque 3 frame 0 dy -15 dx -1
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    08 0C 00 00 FF DA          ; dessin banque 2 frame 12 dy 0 dx -38
    08 0F 16 80 FF E8          ; dessin banque 2 frame 15 dy 22 dx -24
    08 10 27 80 00 13          ; dessin banque 2 frame 16 dy 39 dx 19
    08 11 31 90 00 11          ; dessin banque 2 frame 17 dy 49 dx 17 [décor]
    0C 00 F1 00 FF FF          ; dessin banque 3 frame 0 dy -15 dx -1
    FF 00                      ; fin d'étape
  ; étape 4
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    88 02                      ; $88 Hold
    08 12 08 00 FF DB          ; dessin banque 2 frame 18 dy 8 dx -37
    08 13 16 80 FF EE          ; dessin banque 2 frame 19 dy 22 dx -18
    08 11 31 90 00 11          ; dessin banque 2 frame 17 dy 49 dx 17 [décor]
    0C 06 FB 00 FF F1          ; dessin banque 3 frame 6 dy -5 dx -15
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 5
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 6
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 7
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
```

### `LAB_07F7` (4:$1720)

Rôles : cible de saut depuis LAB_07F5 ; cible de saut depuis LAB_07F6 ; cible de saut depuis LAB_07FA ; cible de saut depuis LAB_07FB ; cible de saut depuis LAB_0849 ; cible de saut depuis LAB_084A ; cible de saut depuis LAB_084B ; cible de saut depuis LAB_084C ; cible de saut depuis LAB_084D ; cible de saut depuis LAB_084E ; cible de saut depuis LAB_085E ; cible de saut depuis LAB_085F ; référencé par le code dans LAB_0268 ; référencé par le code dans LAB_0EFB

```
  ; étape 1
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 2
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 3
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
```

### `LAB_07F8` (4:$173A)

Rôles : cible de saut depuis LAB_07F9 ; référencé par le code dans LAB_01F4 ; référencé par le code dans LAB_020B

```
  ; étape 1
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 2
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
```

### `LAB_07F9` (4:$1798)

Rôles : référencé par le code dans LAB_01F5 ; référencé par le code dans LAB_020B ; référencé par le code dans LAB_0F15

```
  LAB_07F8:
  ; étape 1
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 2
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  ; étape 4
    B0 00 00 00 71 C8          ; $B0 Call -> LAB_02DB
    D0 00                      ; $D0 Reset
    A4 09                      ; $A4 Sound
    98 FF 00 00 17 3A          ; $98 SkipIfDebug -> LAB_07F8
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    08 1C ED 00 FF E7          ; dessin banque 2 frame 28 dy -19 dx -25
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    08 1D E0 00 FF E9          ; dessin banque 2 frame 29 dy -32 dx -23
    08 1E F0 00 FF FA          ; dessin banque 2 frame 30 dy -16 dx -6
    08 1F 0B 00 FF E2          ; dessin banque 2 frame 31 dy 11 dx -30
    08 1F 06 00 FF F9          ; dessin banque 2 frame 31 dy 6 dx -7
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 20 D9 00 FF E9          ; dessin banque 2 frame 32 dy -39 dx -23
    08 24 35 00 FF F7          ; dessin banque 2 frame 36 dy 53 dx -9
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    08 21 DF 00 FF EF          ; dessin banque 2 frame 33 dy -33 dx -17
    08 22 F6 00 FF E4          ; dessin banque 2 frame 34 dy -10 dx -28
    08 23 03 00 FF E9          ; dessin banque 2 frame 35 dy 3 dx -23
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 24 37 00 FF FA          ; dessin banque 2 frame 36 dy 55 dx -6
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    08 27 37 00 FF FA          ; dessin banque 2 frame 39 dy 55 dx -6
    08 26 EE 00 FF F4          ; dessin banque 2 frame 38 dy -18 dx -12
    08 25 03 00 FF E9          ; dessin banque 2 frame 37 dy 3 dx -23
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    08 29 37 00 FF F9          ; dessin banque 2 frame 41 dy 55 dx -7
    08 28 FC 00 FF E9          ; dessin banque 2 frame 40 dy -4 dx -23
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    08 2A 0C 00 FF E8          ; dessin banque 2 frame 42 dy 12 dx -24
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 2C 37 00 FF F9          ; dessin banque 2 frame 44 dy 55 dx -7
    08 2B 13 00 FF F8          ; dessin banque 2 frame 43 dy 19 dx -8
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 11
    A4 10                      ; $A4 Sound
    08 2D 05 00 FF E9          ; dessin banque 2 frame 45 dy 5 dx -23
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 2E 31 00 FF EA          ; dessin banque 2 frame 46 dy 49 dx -22
    08 2F 2A 00 FF FB          ; dessin banque 2 frame 47 dy 42 dx -5
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 12
    08 30 05 00 FF E8          ; dessin banque 2 frame 48 dy 5 dx -24
    08 31 28 00 FF F8          ; dessin banque 2 frame 49 dy 40 dx -8
    08 32 32 00 FF E5          ; dessin banque 2 frame 50 dy 50 dx -27
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 13
    08 33 05 00 FF E7          ; dessin banque 2 frame 51 dy 5 dx -25
    08 34 2D 00 00 0A          ; dessin banque 2 frame 52 dy 45 dx 10
    08 35 37 10 FF F8          ; dessin banque 2 frame 53 dy 55 dx -8 [décor]
    08 36 34 00 FF E1          ; dessin banque 2 frame 54 dy 52 dx -31
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 14
    A4 10                      ; $A4 Sound
    08 37 08 00 FF E8          ; dessin banque 2 frame 55 dy 8 dx -24
    08 38 2F 00 00 15          ; dessin banque 2 frame 56 dy 47 dx 21
    08 35 37 10 FF F8          ; dessin banque 2 frame 53 dy 55 dx -8 [décor]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    08 16 15 00 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28
    FF 00                      ; fin d'étape
  ; étape 15
    A4 0A                      ; $A4 Sound
    08 39 31 00 00 13          ; dessin banque 2 frame 57 dy 49 dx 19
    08 3A 13 00 FF CE          ; dessin banque 2 frame 58 dy 19 dx -50
    08 3B 2D 00 FF B8          ; dessin banque 2 frame 59 dy 45 dx -72
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 39 00 FF 9E          ; dessin banque 2 frame 27 dy 57 dx -98
    FF 00                      ; fin d'étape
  ; étape 16
    08 3C 2D 00 FF B5          ; dessin banque 2 frame 60 dy 45 dx -75
    08 39 31 00 00 13          ; dessin banque 2 frame 57 dy 49 dx 19
    08 3D 34 00 00 10          ; dessin banque 2 frame 61 dy 52 dx 16
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 39 00 FF 9E          ; dessin banque 2 frame 27 dy 57 dx -98
    FF 00                      ; fin d'étape
  ; étape 17
    88 02                      ; $88 Hold
    08 39 31 10 00 13          ; dessin banque 2 frame 57 dy 49 dx 19 [décor]
    08 3E 2D 10 FF B0          ; dessin banque 2 frame 62 dy 45 dx -80 [décor]
    08 3F 34 10 00 11          ; dessin banque 2 frame 63 dy 52 dx 17 [décor]
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 39 10 FF 9E          ; dessin banque 2 frame 27 dy 57 dx -98 [décor]
    FF 00                      ; fin d'étape
  ; étape 18
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_07FA` (4:$1992)

Rôles : cible de saut depuis LAB_07FB ; cible de saut depuis LAB_0849 ; cible de saut depuis LAB_084A ; cible de saut depuis LAB_084B ; cible de saut depuis LAB_084C ; cible de saut depuis LAB_084D ; cible de saut depuis LAB_084E ; référencé par le code dans LAB_0261

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
```

### `LAB_07FB` (4:$19B2)

Rôles : objet+30 [objet+30 (scripts objet+30)] : [2] ; objet+30 [objet+30 (scripts objet+30)] : [5] ; objet+30 [objet+30 (scripts objet+30)] : [8] ; référencé par le code dans LAB_0192 ; référencé par le code dans LAB_0196 ; référencé par le code dans LAB_019E ; référencé par le code dans LAB_01A0 ; référencé par le code dans LAB_0203 ; référencé par le code dans LAB_0EE1 ; référencé par le code dans LAB_0EE3

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
    A4 12                      ; $A4 Sound
    A4 08                      ; $A4 Sound
    B0 00 00 00 58 B8          ; $B0 Call -> LAB_0210
    88 02                      ; $88 Hold
    B0 00 00 00 58 C2          ; $B0 Call -> LAB_0211
    04 11 EC 00 FF D8          ; dessin banque 1 frame 17 dy -20 dx -40
    04 12 2E 00 FF DF          ; dessin banque 1 frame 18 dy 46 dx -33
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    B0 00 00 00 58 C2          ; $B0 Call -> LAB_0211
    04 13 F4 00 FF EB          ; dessin banque 1 frame 19 dy -12 dx -21
    04 14 07 00 00 0E          ; dessin banque 1 frame 20 dy 7 dx 14
    04 15 2F 00 FF EB          ; dessin banque 1 frame 21 dy 47 dx -21
    FF 00                      ; fin d'étape
  ; étape 9
    84 00 00 00 19 92          ; $84 Jump -> LAB_07FA
    A4 0E                      ; $A4 Sound
    A4 0F                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 17 22 00 FF EE          ; dessin banque 1 frame 23 dy 34 dx -18
    04 16 29 00 FF C8          ; dessin banque 1 frame 22 dy 41 dx -56
    FF FF                      ; fin du script
```

### `LAB_07FC` (4:$1A08)

Rôles : référencé par le code dans LAB_0164 ; référencé par le code dans LAB_0165 ; référencé par le code dans LAB_01A4

```
  ; étape 1
    00 03 2E 00 FF 6D          ; dessin banque 0 frame 3 dy 46 dx -147
    00 02 F7 00 FF 71          ; dessin banque 0 frame 2 dy -9 dx -143
    0C 01 F9 00 FF 7C          ; dessin banque 3 frame 1 dy -7 dx -132
    FF 00                      ; fin d'étape
  ; étape 2
    00 05 22 00 FF 70          ; dessin banque 0 frame 5 dy 34 dx -144
    00 04 F6 00 FF 7A          ; dessin banque 0 frame 4 dy -10 dx -134
    0C 01 F9 00 FF 84          ; dessin banque 3 frame 1 dy -7 dx -124
    FF 00                      ; fin d'étape
  ; étape 3
    00 07 18 00 FF 86          ; dessin banque 0 frame 7 dy 24 dx -122
    00 06 F8 00 FF 8A          ; dessin banque 0 frame 6 dy -8 dx -118
    0C 01 FA 00 FF 93          ; dessin banque 3 frame 1 dy -6 dx -109
    FF 00                      ; fin d'étape
  ; étape 4
    00 09 1B 00 FF 87          ; dessin banque 0 frame 9 dy 27 dx -121
    00 08 F7 00 FF 90          ; dessin banque 0 frame 8 dy -9 dx -112
    0C 01 F7 00 FF A0          ; dessin banque 3 frame 1 dy -9 dx -96
    FF 00                      ; fin d'étape
  ; étape 5
    00 03 2E 00 FF 9C          ; dessin banque 0 frame 3 dy 46 dx -100
    00 02 F7 00 FF A1          ; dessin banque 0 frame 2 dy -9 dx -95
    0C 01 F8 00 FF AD          ; dessin banque 3 frame 1 dy -8 dx -83
    FF 00                      ; fin d'étape
  ; étape 6
    00 05 22 00 FF 9F          ; dessin banque 0 frame 5 dy 34 dx -97
    00 04 F6 00 FF A9          ; dessin banque 0 frame 4 dy -10 dx -87
    0C 01 F9 00 FF B3          ; dessin banque 3 frame 1 dy -7 dx -77
    FF 00                      ; fin d'étape
  ; étape 7
    00 07 18 00 FF B5          ; dessin banque 0 frame 7 dy 24 dx -75
    00 06 F8 00 FF B9          ; dessin banque 0 frame 6 dy -8 dx -71
    0C 01 F9 00 FF C1          ; dessin banque 3 frame 1 dy -7 dx -63
    FF 00                      ; fin d'étape
  ; étape 8
    00 09 1B 00 FF B6          ; dessin banque 0 frame 9 dy 27 dx -74
    00 08 F7 00 FF BF          ; dessin banque 0 frame 8 dy -9 dx -65
    0C 01 F7 00 FF CF          ; dessin banque 3 frame 1 dy -9 dx -49
    FF 00                      ; fin d'étape
  ; étape 9
    00 03 2D 00 FF CB          ; dessin banque 0 frame 3 dy 45 dx -53
    00 02 F6 00 FF D0          ; dessin banque 0 frame 2 dy -10 dx -48
    0C 01 F7 00 FF DC          ; dessin banque 3 frame 1 dy -9 dx -36
    FF 00                      ; fin d'étape
  ; étape 10
    00 05 22 00 FF CC          ; dessin banque 0 frame 5 dy 34 dx -52
    00 04 F6 00 FF D6          ; dessin banque 0 frame 4 dy -10 dx -42
    0C 01 F9 00 FF E0          ; dessin banque 3 frame 1 dy -7 dx -32
    FF 00                      ; fin d'étape
  ; étape 11
    00 07 18 00 FF E2          ; dessin banque 0 frame 7 dy 24 dx -30
    00 06 F8 00 FF E6          ; dessin banque 0 frame 6 dy -8 dx -26
    0C 01 F9 00 FF EF          ; dessin banque 3 frame 1 dy -7 dx -17
    FF 00                      ; fin d'étape
  ; étape 12
    00 09 1B 00 FF E3          ; dessin banque 0 frame 9 dy 27 dx -29
    00 08 F7 00 FF EC          ; dessin banque 0 frame 8 dy -9 dx -20
    0C 01 F7 00 FF FC          ; dessin banque 3 frame 1 dy -9 dx -4
    FF 00                      ; fin d'étape
  ; étape 13
    B0 00 00 00 03 BA          ; $B0 Call -> LAB_000A
    88 02                      ; $88 Hold
    04 0D F7 00 FF F3          ; dessin banque 1 frame 13 dy -9 dx -13
    FF FF                      ; fin du script
```

### `LAB_07FD` (4:$1B08)

Rôles : référencé par le code dans LAB_01FF ; référencé par le code dans LAB_02A9

```
  ; étape 1
    D0 00                      ; $D0 Reset
    A4 87                      ; $A4 Sound
    88 02                      ; $88 Hold
    10 00 0D 20 FF D8          ; dessin banque 4 frame 0 dy 13 dx -40
    10 01 0C 20 FF FE          ; dessin banque 4 frame 1 dy 12 dx -2
    10 02 24 20 FF B8          ; dessin banque 4 frame 2 dy 36 dx -72
    10 03 FB 20 FF FF          ; dessin banque 4 frame 3 dy -5 dx -1
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    10 04 17 20 FF E6          ; dessin banque 4 frame 4 dy 23 dx -26
    10 05 25 20 00 02          ; dessin banque 4 frame 5 dy 37 dx 2
    10 06 0D 20 00 08          ; dessin banque 4 frame 6 dy 13 dx 8
    10 07 22 20 FF D2          ; dessin banque 4 frame 7 dy 34 dx -46
    10 08 22 20 FF AE          ; dessin banque 4 frame 8 dy 34 dx -82
    10 09 19 20 FF D4          ; dessin banque 4 frame 9 dy 25 dx -44
    10 0A F5 20 FF CB          ; dessin banque 4 frame 10 dy -11 dx -53
    10 0B 02 20 FF DA          ; dessin banque 4 frame 11 dy 2 dx -38
    10 0C F2 20 FF F2          ; dessin banque 4 frame 12 dy -14 dx -14
    10 0D F5 20 00 10          ; dessin banque 4 frame 13 dy -11 dx 16
    10 0E 0A 20 00 1A          ; dessin banque 4 frame 14 dy 10 dx 26
    10 0F F2 20 00 27          ; dessin banque 4 frame 15 dy -14 dx 39
    10 10 FF 20 00 29          ; dessin banque 4 frame 16 dy -1 dx 41
    10 11 0A 20 00 3E          ; dessin banque 4 frame 17 dy 10 dx 62
    10 12 1D 20 00 2E          ; dessin banque 4 frame 18 dy 29 dx 46
    10 13 2F 20 00 2E          ; dessin banque 4 frame 19 dy 47 dx 46
    10 36 2F 20 FF F4          ; dessin banque 4 frame 54 dy 47 dx -12
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 15 2B 20 FF 8C          ; dessin banque 4 frame 21 dy 43 dx -116
    10 16 31 20 FF C9          ; dessin banque 4 frame 22 dy 49 dx -55
    10 17 2E 20 00 0E          ; dessin banque 4 frame 23 dy 46 dx 14
    10 18 27 20 00 2E          ; dessin banque 4 frame 24 dy 39 dx 46
    10 19 24 20 FF BD          ; dessin banque 4 frame 25 dy 36 dx -67
    10 1A 18 20 FF B9          ; dessin banque 4 frame 26 dy 24 dx -71
    10 1B F2 20 FF B4          ; dessin banque 4 frame 27 dy -14 dx -76
    10 1C F7 20 FF C2          ; dessin banque 4 frame 28 dy -9 dx -62
    10 1D EF 20 FF E1          ; dessin banque 4 frame 29 dy -17 dx -31
    10 1E 12 20 FF E7          ; dessin banque 4 frame 30 dy 18 dx -25
    10 1F 06 20 00 00          ; dessin banque 4 frame 31 dy 6 dx 0
    10 20 FF 20 00 0F          ; dessin banque 4 frame 32 dy -1 dx 15
    10 21 E9 20 00 1C          ; dessin banque 4 frame 33 dy -23 dx 28
    10 22 F3 20 00 3C          ; dessin banque 4 frame 34 dy -13 dx 60
    10 23 13 20 00 0F          ; dessin banque 4 frame 35 dy 19 dx 15
    10 24 14 20 00 5A          ; dessin banque 4 frame 36 dy 20 dx 90
    10 25 1C 20 00 3A          ; dessin banque 4 frame 37 dy 28 dx 58
    10 26 20 20 00 12          ; dessin banque 4 frame 38 dy 32 dx 18
    10 1D E0 20 FF EF          ; dessin banque 4 frame 29 dy -32 dx -17
    10 1D 05 20 FF D4          ; dessin banque 4 frame 29 dy 5 dx -44
    10 1D EB 20 FF F2          ; dessin banque 4 frame 29 dy -21 dx -14
    10 36 2F 20 FF F4          ; dessin banque 4 frame 54 dy 47 dx -12
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 29 F4 20 FF 9C          ; dessin banque 4 frame 41 dy -12 dx -100
    10 2A FE 20 FF AE          ; dessin banque 4 frame 42 dy -2 dx -82
    10 2B 16 20 FF B0          ; dessin banque 4 frame 43 dy 22 dx -80
    10 2B 0F 20 FF DF          ; dessin banque 4 frame 43 dy 15 dx -33
    10 2B 03 20 FF E7          ; dessin banque 4 frame 43 dy 3 dx -25
    10 2B EC 20 FF DC          ; dessin banque 4 frame 43 dy -20 dx -36
    10 2B D9 20 FF CF          ; dessin banque 4 frame 43 dy -39 dx -49
    10 2B 06 20 00 02          ; dessin banque 4 frame 43 dy 6 dx 2
    10 2B 11 20 00 0F          ; dessin banque 4 frame 43 dy 17 dx 15
    10 2B 15 20 00 2B          ; dessin banque 4 frame 43 dy 21 dx 43
    10 2B 01 20 00 1D          ; dessin banque 4 frame 43 dy 1 dx 29
    10 2B 1A 20 FF BF          ; dessin banque 4 frame 43 dy 26 dx -65
    10 2C FB 20 00 51          ; dessin banque 4 frame 44 dy -5 dx 81
    10 2D 09 20 00 6D          ; dessin banque 4 frame 45 dy 9 dx 109
    10 36 2F 20 FF F4          ; dessin banque 4 frame 54 dy 47 dx -12
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 2E FB 20 FF 87          ; dessin banque 4 frame 46 dy -5 dx -121
    10 2F FB 20 FF 99          ; dessin banque 4 frame 47 dy -5 dx -103
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 36 2F 20 FF F4          ; dessin banque 4 frame 54 dy 47 dx -12
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 30 16 20 FF 6D          ; dessin banque 4 frame 48 dy 22 dx -147
    10 31 0A 20 FF 7B          ; dessin banque 4 frame 49 dy 10 dx -133
    10 32 1D 20 FF 93          ; dessin banque 4 frame 50 dy 29 dx -109
    10 36 2F 20 FF F4          ; dessin banque 4 frame 54 dy 47 dx -12
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    10 14 25 30 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17 [décor]
    10 27 2B 30 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116 [décor]
    10 28 2D 30 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15 [décor]
    10 37 2F 30 FF F2          ; dessin banque 4 frame 55 dy 47 dx -14 [décor]
    10 34 2D 30 FF 63          ; dessin banque 4 frame 52 dy 45 dx -157 [décor]
    10 35 31 30 FF 71          ; dessin banque 4 frame 53 dy 49 dx -143 [décor]
    10 33 31 30 FF 54          ; dessin banque 4 frame 51 dy 49 dx -172 [décor]
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 38 2F 20 FF F2          ; dessin banque 4 frame 56 dy 47 dx -14
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 39 2F 20 FF F2          ; dessin banque 4 frame 57 dy 47 dx -14
    FF 00                      ; fin d'étape
  ; étape 10
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 32                      ; $88 Hold
    10 14 25 20 FF EF          ; dessin banque 4 frame 20 dy 37 dx -17
    10 27 2B 20 FF 8C          ; dessin banque 4 frame 39 dy 43 dx -116
    10 28 2D 20 00 0F          ; dessin banque 4 frame 40 dy 45 dx 15
    10 39 2F 20 FF F2          ; dessin banque 4 frame 57 dy 47 dx -14
    FF FF                      ; fin du script
```

### `LAB_07FE` (4:$1D74)

Rôles : référencé par le code dans LAB_0192

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  ; étape 2
    D0 00                      ; $D0 Reset
    A4 1B                      ; $A4 Sound
    94 05                      ; $94 Loop
    B0 00 00 00 71 D2          ; $B0 Call -> LAB_02DC
    10 3A EC 20 FF EE          ; dessin banque 4 frame 58 dy -20 dx -18
    FF 00                      ; fin d'étape
  ; étape 3
    10 3B F2 20 FF F1          ; dessin banque 4 frame 59 dy -14 dx -15
    FF FE                      ; fin d'étape, boucle
  ; étape 4
    B4 00 00 00 1D 9E          ; $B4 IfDead -> LAB_07FF
    C0 01                      ; $C0 SetBank
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
  LAB_07FF:
    88 02                      ; $88 Hold
    10 3C 09 20 FF E6          ; dessin banque 4 frame 60 dy 9 dx -26
    FF 00                      ; fin d'étape
  ; étape 5
    A4 12                      ; $A4 Sound
    94 04                      ; $94 Loop
    88 02                      ; $88 Hold
    10 3D 0B 20 FF E6          ; dessin banque 4 frame 61 dy 11 dx -26
    FF FE                      ; fin d'étape, boucle
  ; étape 6
    88 02                      ; $88 Hold
    10 3E 0C 20 FF E6          ; dessin banque 4 frame 62 dy 12 dx -26
    FF 00                      ; fin d'étape
  ; étape 7
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    94 64                      ; $94 Loop
    88 02                      ; $88 Hold
    10 40 18 20 FF D8          ; dessin banque 4 frame 64 dy 24 dx -40
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    10 3F 19 20 FF D7          ; dessin banque 4 frame 63 dy 25 dx -41
    FF FF                      ; fin du script
```

### `LAB_07FF` (4:$1D9E)

Rôles : cible de saut depuis LAB_07FE

```
  ; étape 1
    88 02                      ; $88 Hold
    10 3C 09 20 FF E6          ; dessin banque 4 frame 60 dy 9 dx -26
    FF 00                      ; fin d'étape
  ; étape 2
    A4 12                      ; $A4 Sound
    94 04                      ; $94 Loop
    88 02                      ; $88 Hold
    10 3D 0B 20 FF E6          ; dessin banque 4 frame 61 dy 11 dx -26
    FF FE                      ; fin d'étape, boucle
  ; étape 3
    88 02                      ; $88 Hold
    10 3E 0C 20 FF E6          ; dessin banque 4 frame 62 dy 12 dx -26
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    94 64                      ; $94 Loop
    88 02                      ; $88 Hold
    10 40 18 20 FF D8          ; dessin banque 4 frame 64 dy 24 dx -40
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    10 3F 19 20 FF D7          ; dessin banque 4 frame 63 dy 25 dx -41
    FF FF                      ; fin du script
```

### `LAB_0800` (4:$1DE2)

Rôles : objet+22 (repos) dans LAB_0169 ; objet+26 (réaction) dans LAB_0169

```
  ; étape 1
    00 00 04 01 FF E6          ; dessin banque 0 frame 0 dy 4 dx -26 [corps]
    00 01 1E 41 00 0B          ; dessin banque 0 frame 1 dy 30 dx 11 [corps,hors-boîte]
    04 37 F4 00 FF F2          ; dessin banque 1 frame 55 dy -12 dx -14
    FF FF                      ; fin du script
```

### `LAB_0801` (4:$1DF6)

Rôles : LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 1 : feu + D+B ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 2 : feu + D ou D+B+H ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 3 : feu + G+H ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 4 : feu + G+B ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 5 : feu + G ou B+H ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 7 : feu + B ou D+G+B+H ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    88 01                      ; $88 Hold
    00 02 04 01 FF E9          ; dessin banque 0 frame 2 dy 4 dx -23 [corps]
    00 03 1D 41 00 0A          ; dessin banque 0 frame 3 dy 29 dx 10 [corps,hors-boîte]
    04 38 0A 02 FF C9          ; dessin banque 1 frame 56 dy 10 dx -55 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    A4 0D                      ; $A4 Sound
    00 04 06 01 FF F9          ; dessin banque 0 frame 4 dy 6 dx -7 [corps]
    04 32 17 40 FF E8          ; dessin banque 1 frame 50 dy 23 dx -24 [hors-boîte]
    04 39 1B 02 00 0F          ; dessin banque 1 frame 57 dy 27 dx 15 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    00 06 0C 01 00 13          ; dessin banque 0 frame 6 dy 12 dx 19 [corps]
    00 05 23 01 FF F8          ; dessin banque 0 frame 5 dy 35 dx -8 [corps]
    04 33 1D 00 00 1E          ; dessin banque 1 frame 51 dy 29 dx 30
    04 3A 1C 02 00 3C          ; dessin banque 1 frame 58 dy 28 dx 60 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    00 08 09 01 00 17          ; dessin banque 0 frame 8 dy 9 dx 23 [corps]
    00 07 1E 01 FF FC          ; dessin banque 0 frame 7 dy 30 dx -4 [corps]
    04 34 02 40 00 33          ; dessin banque 1 frame 52 dy 2 dx 51 [hors-boîte]
    04 3B 02 02 00 2B          ; dessin banque 1 frame 59 dy 2 dx 43 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    88 01                      ; $88 Hold
    00 08 09 01 00 17          ; dessin banque 0 frame 8 dy 9 dx 23 [corps]
    00 07 1E 01 FF FC          ; dessin banque 0 frame 7 dy 30 dx -4 [corps]
    04 3B 02 00 00 2B          ; dessin banque 1 frame 59 dy 2 dx 43
    FF FF                      ; fin du script
```

### `LAB_0802` (4:$1E72)

Rôles : LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 6 : feu + D+H ; LAB_05FB [objet+34 (attaques) ; posée par LAB_0169] : attaque 8 : feu + H ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 09 00 01 FF FD          ; dessin banque 0 frame 9 dy 0 dx -3 [corps]
    04 3C E1 02 00 16          ; dessin banque 1 frame 60 dy -31 dx 22 [frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    A4 0C                      ; $A4 Sound
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    00 18 2F 01 FF F8          ; dessin banque 0 frame 24 dy 47 dx -8 [corps]
    00 0B 14 01 00 08          ; dessin banque 0 frame 11 dy 20 dx 8 [corps]
    00 0A FD 01 FF EE          ; dessin banque 0 frame 10 dy -3 dx -18 [corps]
    04 3D EF 42 FF CD          ; dessin banque 1 frame 61 dy -17 dx -51 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 3
    00 0E 2E 01 FF F8          ; dessin banque 0 frame 14 dy 46 dx -8 [corps]
    00 0D 0D 01 00 0B          ; dessin banque 0 frame 13 dy 13 dx 11 [corps]
    00 0C F8 01 FF FE          ; dessin banque 0 frame 12 dy -8 dx -2 [corps]
    04 3E E4 42 FF DD          ; dessin banque 1 frame 62 dy -28 dx -35 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    00 10 16 01 00 0C          ; dessin banque 0 frame 16 dy 22 dx 12 [corps]
    00 0F F7 01 00 1F          ; dessin banque 0 frame 15 dy -9 dx 31 [corps]
    00 11 30 00 00 2D          ; dessin banque 0 frame 17 dy 48 dx 45
    00 12 32 00 FF FF          ; dessin banque 0 frame 18 dy 50 dx -1
    04 35 CD 40 FF FE          ; dessin banque 1 frame 53 dy -51 dx -2 [hors-boîte]
    04 3F DD 42 00 45          ; dessin banque 1 frame 63 dy -35 dx 69 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 5
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 15 13 01 00 2A          ; dessin banque 0 frame 21 dy 19 dx 42 [corps]
    04 40 2B 02 00 51          ; dessin banque 1 frame 64 dy 43 dx 81 [frappe]
    04 36 FF 42 00 6B          ; dessin banque 1 frame 54 dy -1 dx 107 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 6
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 16 14 01 00 24          ; dessin banque 0 frame 22 dy 20 dx 36 [corps]
    04 40 2B 40 00 51          ; dessin banque 1 frame 64 dy 43 dx 81 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 7
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 17 16 01 00 24          ; dessin banque 0 frame 23 dy 22 dx 36 [corps]
    04 40 2B 40 00 51          ; dessin banque 1 frame 64 dy 43 dx 81 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_0803` (4:$1F36)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 19 00 01 FF EF          ; dessin banque 0 frame 25 dy 0 dx -17 [corps]
    04 42 FD 00 00 00          ; dessin banque 1 frame 66 dy -3 dx 0
    FF FF                      ; fin du script
```

### `LAB_0804` (4:$1F44)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 1A 00 01 FF EE          ; dessin banque 0 frame 26 dy 0 dx -18 [corps]
    04 42 FC 00 00 04          ; dessin banque 1 frame 66 dy -4 dx 4
    FF FF                      ; fin du script
```

### `LAB_0805` (4:$1F52)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche horizontal phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 1B 00 01 FF EF          ; dessin banque 0 frame 27 dy 0 dx -17 [corps]
    00 1C 21 01 FF E2          ; dessin banque 0 frame 28 dy 33 dx -30 [corps]
    04 42 FD 00 00 01          ; dessin banque 1 frame 66 dy -3 dx 1
    FF FF                      ; fin du script
```

### `LAB_0806` (4:$1F66)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 43 F3 00 00 06          ; dessin banque 1 frame 67 dy -13 dx 6
    00 1D FC 01 FF F3          ; dessin banque 0 frame 29 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0807` (4:$1F74)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 43 F3 00 00 06          ; dessin banque 1 frame 67 dy -13 dx 6
    00 1E FC 01 FF F3          ; dessin banque 0 frame 30 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0808` (4:$1F82)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 43 F3 00 00 03          ; dessin banque 1 frame 67 dy -13 dx 3
    00 1F FC 01 FF F3          ; dessin banque 0 frame 31 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0809` (4:$1F90)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche haut phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 43 F3 00 00 03          ; dessin banque 1 frame 67 dy -13 dx 3
    00 20 FC 01 FF F3          ; dessin banque 0 frame 32 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_080A` (4:$1F9E)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 21 01 01 FF EF          ; dessin banque 0 frame 33 dy 1 dx -17 [corps]
    04 41 FC 00 FF F3          ; dessin banque 1 frame 65 dy -4 dx -13
    FF FF                      ; fin du script
```

### `LAB_080B` (4:$1FAC)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 22 01 01 FF ED          ; dessin banque 0 frame 34 dy 1 dx -19 [corps]
    04 41 FA 00 FF F3          ; dessin banque 1 frame 65 dy -6 dx -13
    FF FF                      ; fin du script
```

### `LAB_080C` (4:$1FBA)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 23 01 01 FF ED          ; dessin banque 0 frame 35 dy 1 dx -19 [corps]
    04 41 FC 00 FF F4          ; dessin banque 1 frame 65 dy -4 dx -12
    FF FF                      ; fin du script
```

### `LAB_080D` (4:$1FC8)

Rôles : LAB_060C [objet+46 (marche) ; posée par LAB_0169] : marche bas phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 24 01 01 FF EE          ; dessin banque 0 frame 36 dy 1 dx -18 [corps]
    04 41 FC 00 FF F4          ; dessin banque 1 frame 65 dy -4 dx -12
    FF FF                      ; fin du script
```

### `LAB_080E` (4:$1FD6)

Rôles : LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [3] ; LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [6] ; LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [8] ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 3C FE 00 00 19          ; dessin banque 1 frame 60 dy -2 dx 25
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 01 11 80 FF FC          ; dessin banque 1 frame 1 dy 17 dx -4
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 3C FE 00 00 19          ; dessin banque 1 frame 60 dy -2 dx 25
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 03 17 80 00 22          ; dessin banque 1 frame 3 dy 23 dx 34
    04 04 1D 80 FF F8          ; dessin banque 1 frame 4 dy 29 dx -8
    04 05 1E 80 00 24          ; dessin banque 1 frame 5 dy 30 dx 36
    04 02 15 80 00 0C          ; dessin banque 1 frame 2 dy 21 dx 12
    FF 00                      ; fin d'étape
  ; étape 3
    04 3C FE 00 00 19          ; dessin banque 1 frame 60 dy -2 dx 25
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 07 31 80 00 30          ; dessin banque 1 frame 7 dy 49 dx 48
    04 06 14 80 00 0B          ; dessin banque 1 frame 6 dy 20 dx 11
    FF 00                      ; fin d'étape
  ; étape 4
    04 3C FE 00 00 19          ; dessin banque 1 frame 60 dy -2 dx 25
    04 08 2E 80 00 2B          ; dessin banque 1 frame 8 dy 46 dx 43
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    B4 01 00 00 22 40          ; $B4 IfDead -> LAB_0812
    04 3C FE 00 00 19          ; dessin banque 1 frame 60 dy -2 dx 25
    04 0A 36 90 00 2B          ; dessin banque 1 frame 10 dy 54 dx 43 [décor]
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    FF FF                      ; fin du script
  LAB_0812:
  ; étape 6
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 3F 0D 00 00 3E          ; dessin banque 1 frame 63 dy 13 dx 62
    FF 00                      ; fin d'étape
  ; étape 7
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 40 2F 00 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 40 2F 50 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 9
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_080F` (4:$2060)

Rôles : LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [2] ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    B4 01 00 00 22 9E          ; $B4 IfDead -> LAB_0813
    04 43 FB 00 00 1B          ; dessin banque 1 frame 67 dy -5 dx 27
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 0C 1A 80 FF F7          ; dessin banque 1 frame 12 dy 26 dx -9
    FF 00                      ; fin d'étape
  ; étape 2
    04 43 FB 00 00 1B          ; dessin banque 1 frame 67 dy -5 dx 27
    04 0D 1E 80 FF E4          ; dessin banque 1 frame 13 dy 30 dx -28
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 05 22 80 00 28          ; dessin banque 1 frame 5 dy 34 dx 40
    04 0E 1C 80 00 08          ; dessin banque 1 frame 14 dy 28 dx 8
    FF 00                      ; fin d'étape
  ; étape 3
    04 43 FB 00 00 1B          ; dessin banque 1 frame 67 dy -5 dx 27
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 08 30 80 00 34          ; dessin banque 1 frame 8 dy 48 dx 52
    04 0F 1C 80 00 08          ; dessin banque 1 frame 15 dy 28 dx 8
    FF 00                      ; fin d'étape
  ; étape 4
    04 43 FB 00 00 1B          ; dessin banque 1 frame 67 dy -5 dx 27
    04 0A 38 90 00 33          ; dessin banque 1 frame 10 dy 56 dx 51 [décor]
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 07 1D 80 00 0A          ; dessin banque 1 frame 7 dy 29 dx 10
    FF FF                      ; fin du script
  LAB_0812:
  ; étape 5
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 3F 0D 00 00 3E          ; dessin banque 1 frame 63 dy 13 dx 62
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 40 2F 00 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 40 2F 50 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  LAB_0813:
  ; étape 9
    98 80 00 00 22 40          ; $98 SkipIfDebug -> LAB_0812
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 03 00 FF F7          ; dessin banque 1 frame 35 dy 3 dx -9
    04 22 14 00 FF EA          ; dessin banque 1 frame 34 dy 20 dx -22
    04 3F 02 00 00 2B          ; dessin banque 1 frame 63 dy 2 dx 43
    FF 00                      ; fin d'étape
  ; étape 10
    04 24 EB 00 FF F3          ; dessin banque 1 frame 36 dy -21 dx -13
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 11
    04 26 01 00 FF F1          ; dessin banque 1 frame 38 dy 1 dx -15
    04 25 E5 00 FF FA          ; dessin banque 1 frame 37 dy -27 dx -6
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 12
    04 2A 24 00 00 00          ; dessin banque 1 frame 42 dy 36 dx 0
    04 29 23 00 FF D9          ; dessin banque 1 frame 41 dy 35 dx -39
    04 28 10 00 FF F3          ; dessin banque 1 frame 40 dy 16 dx -13
    04 27 F0 00 FF FE          ; dessin banque 1 frame 39 dy -16 dx -2
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 13
    A4 37                      ; $A4 Sound
    04 2B 30 00 FF D1          ; dessin banque 1 frame 43 dy 48 dx -47
    04 2C 25 00 FF F6          ; dessin banque 1 frame 44 dy 37 dx -10
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 14
    04 2E 2F 00 FF F5          ; dessin banque 1 frame 46 dy 47 dx -11
    04 2B 30 00 FF D0          ; dessin banque 1 frame 43 dy 48 dx -48
    04 2D 27 00 FF E5          ; dessin banque 1 frame 45 dy 39 dx -27
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 15
    04 31 2B 00 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48
    04 2F 32 00 FF D2          ; dessin banque 1 frame 47 dy 50 dx -46
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 16
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 31 2B 10 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48 [décor]
    04 21 3A 10 FF CE          ; dessin banque 1 frame 33 dy 58 dx -50 [décor]
    04 40 31 10 00 26          ; dessin banque 1 frame 64 dy 49 dx 38 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0810` (4:$20D4)

Rôles : LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [1] ; LAB_05FC [objet+30 (scripts objet+30) ; posée par LAB_0169] : [5] ; référencé par le code dans LAB_0156

```
  ; étape 1
    A4 35                      ; $A4 Sound
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 44 08 40 00 27          ; dessin banque 1 frame 68 dy 8 dx 39 [hors-boîte]
    04 11 13 80 FF EE          ; dessin banque 1 frame 17 dy 19 dx -18
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    FF 00                      ; fin d'étape
  ; étape 2
    04 44 08 40 00 27          ; dessin banque 1 frame 68 dy 8 dx 39 [hors-boîte]
    04 12 11 80 FF E8          ; dessin banque 1 frame 18 dy 17 dx -24
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    04 13 18 80 00 01          ; dessin banque 1 frame 19 dy 24 dx 1
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 21 50          ; $B4 IfDead -> LAB_0811
    04 44 08 40 00 27          ; dessin banque 1 frame 68 dy 8 dx 39 [hors-boîte]
    04 14 2E 80 FF CE          ; dessin banque 1 frame 20 dy 46 dx -50
    04 15 1A 80 FF F8          ; dessin banque 1 frame 21 dy 26 dx -8
    04 04 1D 80 FF D7          ; dessin banque 1 frame 4 dy 29 dx -41
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    04 19 35 D0 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    04 19 35 D0 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor,hors-boîte]
    04 44 08 40 00 27          ; dessin banque 1 frame 68 dy 8 dx 39 [hors-boîte]
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    04 13 19 80 00 03          ; dessin banque 1 frame 19 dy 25 dx 3
    FF FF                      ; fin du script
  LAB_0811:
  ; étape 5
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 44 12 40 00 3A          ; dessin banque 1 frame 68 dy 18 dx 58 [hors-boîte]
    04 18 1A 80 FF F0          ; dessin banque 1 frame 24 dy 26 dx -16
    04 19 35 D0 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor,hors-boîte]
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 0D 26 80 FF DD          ; dessin banque 1 frame 13 dy 38 dx -35
    04 1B 2B 80 00 36          ; dessin banque 1 frame 27 dy 43 dx 54
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    FF 00                      ; fin d'étape
  ; étape 7
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    04 1C 23 80 00 37          ; dessin banque 1 frame 28 dy 35 dx 55
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1D 1D 80 00 38          ; dessin banque 1 frame 29 dy 29 dx 56
    FF 00                      ; fin d'étape
  ; étape 9
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1E 1D 80 00 38          ; dessin banque 1 frame 30 dy 29 dx 56
    FF 00                      ; fin d'étape
  ; étape 10
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1F 23 80 00 37          ; dessin banque 1 frame 31 dy 35 dx 55
    FF 00                      ; fin d'étape
  ; étape 11
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 20 29 80 00 35          ; dessin banque 1 frame 32 dy 41 dx 53
    FF 00                      ; fin d'étape
  ; étape 12
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 21 39 90 00 38          ; dessin banque 1 frame 33 dy 57 dx 56 [décor]
    04 1B 2C 80 00 37          ; dessin banque 1 frame 27 dy 44 dx 55
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    88 02                      ; $88 Hold
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 21 39 D0 00 38          ; dessin banque 1 frame 33 dy 57 dx 56 [décor,hors-boîte]
    04 1B 2C D0 00 37          ; dessin banque 1 frame 27 dy 44 dx 55 [décor,hors-boîte]
    04 40 30 50 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 14
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0811` (4:$2150)

Rôles : cible de saut depuis LAB_0810

```
  ; étape 1
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 44 12 40 00 3A          ; dessin banque 1 frame 68 dy 18 dx 58 [hors-boîte]
    04 18 1A 80 FF F0          ; dessin banque 1 frame 24 dy 26 dx -16
    04 19 35 D0 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor,hors-boîte]
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 0D 26 80 FF DD          ; dessin banque 1 frame 13 dy 38 dx -35
    04 1B 2B 80 00 36          ; dessin banque 1 frame 27 dy 43 dx 54
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    04 1C 23 80 00 37          ; dessin banque 1 frame 28 dy 35 dx 55
    FF 00                      ; fin d'étape
  ; étape 4
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1D 1D 80 00 38          ; dessin banque 1 frame 29 dy 29 dx 56
    FF 00                      ; fin d'étape
  ; étape 5
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1E 1D 80 00 38          ; dessin banque 1 frame 30 dy 29 dx 56
    FF 00                      ; fin d'étape
  ; étape 6
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 1F 23 80 00 37          ; dessin banque 1 frame 31 dy 35 dx 55
    FF 00                      ; fin d'étape
  ; étape 7
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    04 20 29 80 00 35          ; dessin banque 1 frame 32 dy 41 dx 53
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 21 39 90 00 38          ; dessin banque 1 frame 33 dy 57 dx 56 [décor]
    04 1B 2C 80 00 37          ; dessin banque 1 frame 27 dy 44 dx 55
    04 40 30 00 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77
    FF FE                      ; fin d'étape, boucle
  ; étape 9
    88 02                      ; $88 Hold
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 21 39 D0 00 38          ; dessin banque 1 frame 33 dy 57 dx 56 [décor,hors-boîte]
    04 1B 2C D0 00 37          ; dessin banque 1 frame 27 dy 44 dx 55 [décor,hors-boîte]
    04 40 30 50 00 4D          ; dessin banque 1 frame 64 dy 48 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 10
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0812` (4:$2240)

Rôles : cible de saut depuis LAB_080E ; cible de saut depuis LAB_080F ; cible de saut depuis LAB_0813

```
  ; étape 1
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 3F 0D 00 00 3E          ; dessin banque 1 frame 63 dy 13 dx 62
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 40 2F 00 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 40 2F 50 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0813` (4:$229E)

Rôles : cible de saut depuis LAB_080F

```
  LAB_0812:
  ; étape 1
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 3F 0D 00 00 3E          ; dessin banque 1 frame 63 dy 13 dx 62
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 40 2F 00 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 40 2F 50 00 4D          ; dessin banque 1 frame 64 dy 47 dx 77 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 5
    98 80 00 00 22 40          ; $98 SkipIfDebug -> LAB_0812
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 03 00 FF F7          ; dessin banque 1 frame 35 dy 3 dx -9
    04 22 14 00 FF EA          ; dessin banque 1 frame 34 dy 20 dx -22
    04 3F 02 00 00 2B          ; dessin banque 1 frame 63 dy 2 dx 43
    FF 00                      ; fin d'étape
  ; étape 6
    04 24 EB 00 FF F3          ; dessin banque 1 frame 36 dy -21 dx -13
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 7
    04 26 01 00 FF F1          ; dessin banque 1 frame 38 dy 1 dx -15
    04 25 E5 00 FF FA          ; dessin banque 1 frame 37 dy -27 dx -6
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 8
    04 2A 24 00 00 00          ; dessin banque 1 frame 42 dy 36 dx 0
    04 29 23 00 FF D9          ; dessin banque 1 frame 41 dy 35 dx -39
    04 28 10 00 FF F3          ; dessin banque 1 frame 40 dy 16 dx -13
    04 27 F0 00 FF FE          ; dessin banque 1 frame 39 dy -16 dx -2
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 9
    A4 37                      ; $A4 Sound
    04 2B 30 00 FF D1          ; dessin banque 1 frame 43 dy 48 dx -47
    04 2C 25 00 FF F6          ; dessin banque 1 frame 44 dy 37 dx -10
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 10
    04 2E 2F 00 FF F5          ; dessin banque 1 frame 46 dy 47 dx -11
    04 2B 30 00 FF D0          ; dessin banque 1 frame 43 dy 48 dx -48
    04 2D 27 00 FF E5          ; dessin banque 1 frame 45 dy 39 dx -27
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 11
    04 31 2B 00 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48
    04 2F 32 00 FF D2          ; dessin banque 1 frame 47 dy 50 dx -46
    04 40 31 00 00 26          ; dessin banque 1 frame 64 dy 49 dx 38
    FF 00                      ; fin d'étape
  ; étape 12
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 31 2B 10 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48 [décor]
    04 21 3A 10 FF CE          ; dessin banque 1 frame 33 dy 58 dx -50 [décor]
    04 40 31 10 00 26          ; dessin banque 1 frame 64 dy 49 dx 38 [décor]
    FF 00                      ; fin d'étape
  ; étape 13
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0814` (4:$2362)

Rôles : LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [3] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [6] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [8] ; référencé par le code dans LAB_0156

```
  ; étape 1
    D0 00                      ; $D0 Reset
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 3E D8 00 00 1C          ; dessin banque 1 frame 62 dy -40 dx 28
    04 00 04 00 FF FD          ; dessin banque 1 frame 0 dy 4 dx -3
    04 01 0D 80 FF FC          ; dessin banque 1 frame 1 dy 13 dx -4
    FF 00                      ; fin d'étape
  ; étape 2
    04 3E D8 00 00 1C          ; dessin banque 1 frame 62 dy -40 dx 28
    04 00 04 00 FF FD          ; dessin banque 1 frame 0 dy 4 dx -3
    04 03 13 80 00 22          ; dessin banque 1 frame 3 dy 19 dx 34
    04 04 19 80 FF F8          ; dessin banque 1 frame 4 dy 25 dx -8
    04 05 1A 80 00 24          ; dessin banque 1 frame 5 dy 26 dx 36
    04 02 11 80 00 0C          ; dessin banque 1 frame 2 dy 17 dx 12
    FF 00                      ; fin d'étape
  ; étape 3
    04 3E D8 00 00 1C          ; dessin banque 1 frame 62 dy -40 dx 28
    04 00 04 00 FF FD          ; dessin banque 1 frame 0 dy 4 dx -3
    04 07 2D 80 00 30          ; dessin banque 1 frame 7 dy 45 dx 48
    04 06 10 80 00 0B          ; dessin banque 1 frame 6 dy 16 dx 11
    FF 00                      ; fin d'étape
  ; étape 4
    04 3E D8 00 00 1C          ; dessin banque 1 frame 62 dy -40 dx 28
    04 08 2A 80 00 2B          ; dessin banque 1 frame 8 dy 42 dx 43
    04 00 04 00 FF FD          ; dessin banque 1 frame 0 dy 4 dx -3
    FF 00                      ; fin d'étape
  ; étape 5
    B4 01 00 00 26 AA          ; $B4 IfDead -> LAB_0818
    88 02                      ; $88 Hold
    04 3E D8 00 00 1C          ; dessin banque 1 frame 62 dy -40 dx 28
    04 0A 32 90 00 2B          ; dessin banque 1 frame 10 dy 50 dx 43 [décor]
    04 00 04 00 FF FD          ; dessin banque 1 frame 0 dy 4 dx -3
    04 0A 33 90 00 26          ; dessin banque 1 frame 10 dy 51 dx 38 [décor]
    FF FF                      ; fin du script
  LAB_0818:
  ; étape 6
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 18 00 00 31          ; dessin banque 1 frame 64 dy 24 dx 49
    04 15 11 80 00 1B          ; dessin banque 1 frame 21 dy 17 dx 27
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F F9 00 00 4B          ; dessin banque 1 frame 63 dy -7 dx 75
    FF 00                      ; fin d'étape
  ; étape 7
    A4 37                      ; $A4 Sound
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 07 2C 80 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 07 2C 90 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 3C 36 10 00 78          ; dessin banque 1 frame 60 dy 54 dx 120 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0815` (4:$23F4)

Rôles : LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [0] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [2] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [4] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [7] ; référencé par le code dans LAB_0156

```
  ; étape 1
    D0 00                      ; $D0 Reset
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    B4 01 00 00 27 32          ; $B4 IfDead -> LAB_0819
    04 3E D2 00 00 1C          ; dessin banque 1 frame 62 dy -46 dx 28
    04 0B FF 00 00 00          ; dessin banque 1 frame 11 dy -1 dx 0
    04 0C 16 80 FF F7          ; dessin banque 1 frame 12 dy 22 dx -9
    FF 00                      ; fin d'étape
  ; étape 2
    04 3E D2 00 00 1C          ; dessin banque 1 frame 62 dy -46 dx 28
    04 0D 1A 80 FF E4          ; dessin banque 1 frame 13 dy 26 dx -28
    04 0B FF 00 00 00          ; dessin banque 1 frame 11 dy -1 dx 0
    04 05 1E 80 00 28          ; dessin banque 1 frame 5 dy 30 dx 40
    04 0E 18 80 00 08          ; dessin banque 1 frame 14 dy 24 dx 8
    FF 00                      ; fin d'étape
  ; étape 3
    04 3E D2 00 00 1C          ; dessin banque 1 frame 62 dy -46 dx 28
    04 0B FF 00 00 00          ; dessin banque 1 frame 11 dy -1 dx 0
    04 08 2C 80 00 34          ; dessin banque 1 frame 8 dy 44 dx 52
    04 0F 18 80 00 08          ; dessin banque 1 frame 15 dy 24 dx 8
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 3E D2 00 00 1C          ; dessin banque 1 frame 62 dy -46 dx 28
    04 0A 34 90 00 33          ; dessin banque 1 frame 10 dy 52 dx 51 [décor]
    04 0B FF 00 00 00          ; dessin banque 1 frame 11 dy -1 dx 0
    04 07 19 80 00 0A          ; dessin banque 1 frame 7 dy 25 dx 10
    FF FF                      ; fin du script
  LAB_0818:
  ; étape 5
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 18 00 00 31          ; dessin banque 1 frame 64 dy 24 dx 49
    04 15 11 80 00 1B          ; dessin banque 1 frame 21 dy 17 dx 27
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F F9 00 00 4B          ; dessin banque 1 frame 63 dy -7 dx 75
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 07 2C 80 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    FF 00                      ; fin d'étape
  ; étape 7
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 07 2C 90 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 3C 36 10 00 78          ; dessin banque 1 frame 60 dy 54 dx 120 [décor]
    FF 00                      ; fin d'étape
  ; étape 8
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  LAB_0819:
  ; étape 9
    98 80 00 00 26 AA          ; $98 SkipIfDebug -> LAB_0818
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 FF 00 FF F7          ; dessin banque 1 frame 35 dy -1 dx -9
    04 22 10 00 FF EA          ; dessin banque 1 frame 34 dy 16 dx -22
    04 40 04 00 00 1E          ; dessin banque 1 frame 64 dy 4 dx 30
    04 3F E5 00 00 38          ; dessin banque 1 frame 63 dy -27 dx 56
    FF 00                      ; fin d'étape
  ; étape 10
    04 24 E7 00 FF F3          ; dessin banque 1 frame 36 dy -25 dx -13
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 11
    04 26 FD 00 FF F1          ; dessin banque 1 frame 38 dy -3 dx -15
    04 25 E1 00 FF FA          ; dessin banque 1 frame 37 dy -31 dx -6
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 12
    04 2A 20 00 00 00          ; dessin banque 1 frame 42 dy 32 dx 0
    04 29 1F 00 FF D9          ; dessin banque 1 frame 41 dy 31 dx -39
    04 28 0C 00 FF F3          ; dessin banque 1 frame 40 dy 12 dx -13
    04 27 EC 00 FF FE          ; dessin banque 1 frame 39 dy -20 dx -2
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 13
    04 2B 2C 00 FF D1          ; dessin banque 1 frame 43 dy 44 dx -47
    04 2C 21 00 FF F6          ; dessin banque 1 frame 44 dy 33 dx -10
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 14
    A4 37                      ; $A4 Sound
    04 2E 2B 00 FF F5          ; dessin banque 1 frame 46 dy 43 dx -11
    04 2B 2C 00 FF D0          ; dessin banque 1 frame 43 dy 44 dx -48
    04 2D 23 00 FF E5          ; dessin banque 1 frame 45 dy 35 dx -27
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 15
    04 31 27 00 FF D0          ; dessin banque 1 frame 49 dy 39 dx -48
    04 2F 2E 00 FF D2          ; dessin banque 1 frame 47 dy 46 dx -46
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 16
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 31 27 10 FF D0          ; dessin banque 1 frame 49 dy 39 dx -48 [décor]
    04 21 36 10 FF CE          ; dessin banque 1 frame 33 dy 54 dx -50 [décor]
    04 3C 3B 10 00 50          ; dessin banque 1 frame 60 dy 59 dx 80 [décor]
    04 3B 3C 10 00 39          ; dessin banque 1 frame 59 dy 60 dx 57 [décor]
    04 3B 3C 10 00 22          ; dessin banque 1 frame 59 dy 60 dx 34 [décor]
    04 3B 3C 10 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0816` (4:$246C)

Rôles : LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [1] ; LAB_05F9 [objet+30 (scripts objet+30) ; posée par LAB_0176] : [5] ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    A4 34                      ; $A4 Sound
    D0 00                      ; $D0 Reset
    04 40 0F 00 00 1C          ; dessin banque 1 frame 64 dy 15 dx 28
    04 11 0F 80 FF EE          ; dessin banque 1 frame 17 dy 15 dx -18
    04 10 05 00 FF FD          ; dessin banque 1 frame 16 dy 5 dx -3
    04 3F F0 00 00 36          ; dessin banque 1 frame 63 dy -16 dx 54
    FF 00                      ; fin d'étape
  ; étape 2
    04 3F F0 00 00 37          ; dessin banque 1 frame 63 dy -16 dx 55
    04 12 0D 80 FF E8          ; dessin banque 1 frame 18 dy 13 dx -24
    04 40 0F 00 00 1D          ; dessin banque 1 frame 64 dy 15 dx 29
    04 13 14 80 00 01          ; dessin banque 1 frame 19 dy 20 dx 1
    04 10 05 00 FF FD          ; dessin banque 1 frame 16 dy 5 dx -3
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 24 FE          ; $B4 IfDead -> LAB_0817
    04 3F F0 00 00 38          ; dessin banque 1 frame 63 dy -16 dx 56
    04 14 2A 80 FF CE          ; dessin banque 1 frame 20 dy 42 dx -50
    04 15 16 80 FF F8          ; dessin banque 1 frame 21 dy 22 dx -8
    04 04 19 80 FF D7          ; dessin banque 1 frame 4 dy 25 dx -41
    04 40 0F 00 00 1E          ; dessin banque 1 frame 64 dy 15 dx 30
    04 10 05 00 FF FD          ; dessin banque 1 frame 16 dy 5 dx -3
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 40 0F 00 00 1E          ; dessin banque 1 frame 64 dy 15 dx 30
    04 3F F0 00 00 38          ; dessin banque 1 frame 63 dy -16 dx 56
    04 10 05 00 FF FD          ; dessin banque 1 frame 16 dy 5 dx -3
    04 13 15 80 00 03          ; dessin banque 1 frame 19 dy 21 dx 3
    04 19 31 90 FF BD          ; dessin banque 1 frame 25 dy 49 dx -67 [décor]
    FF FF                      ; fin du script
  LAB_0817:
  ; étape 5
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 1A 00 00 2E          ; dessin banque 1 frame 64 dy 26 dx 46
    04 18 16 80 FF F0          ; dessin banque 1 frame 24 dy 22 dx -16
    04 19 31 90 FF BD          ; dessin banque 1 frame 25 dy 49 dx -67 [décor]
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F FB 00 00 48          ; dessin banque 1 frame 63 dy -5 dx 72
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 0D 22 80 FF DD          ; dessin banque 1 frame 13 dy 34 dx -35
    04 1B 27 80 00 36          ; dessin banque 1 frame 27 dy 39 dx 54
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 7
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1C 1F 80 00 37          ; dessin banque 1 frame 28 dy 31 dx 55
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1D 19 80 00 38          ; dessin banque 1 frame 29 dy 25 dx 56
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1E 19 80 00 38          ; dessin banque 1 frame 30 dy 25 dx 56
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 10
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1F 1F 80 00 37          ; dessin banque 1 frame 31 dy 31 dx 55
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 11
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 20 25 80 00 35          ; dessin banque 1 frame 32 dy 37 dx 53
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 12
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 21 35 90 00 38          ; dessin banque 1 frame 33 dy 53 dx 56 [décor]
    04 1B 28 80 00 37          ; dessin banque 1 frame 27 dy 40 dx 55
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 21 35 90 00 38          ; dessin banque 1 frame 33 dy 53 dx 56 [décor]
    04 1B 28 90 00 37          ; dessin banque 1 frame 27 dy 40 dx 55 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3C 36 10 00 79          ; dessin banque 1 frame 60 dy 54 dx 121 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 14
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0817` (4:$24FE)

Rôles : cible de saut depuis LAB_0816

```
  ; étape 1
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 1A 00 00 2E          ; dessin banque 1 frame 64 dy 26 dx 46
    04 18 16 80 FF F0          ; dessin banque 1 frame 24 dy 22 dx -16
    04 19 31 90 FF BD          ; dessin banque 1 frame 25 dy 49 dx -67 [décor]
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F FB 00 00 48          ; dessin banque 1 frame 63 dy -5 dx 72
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 0D 22 80 FF DD          ; dessin banque 1 frame 13 dy 34 dx -35
    04 1B 27 80 00 36          ; dessin banque 1 frame 27 dy 39 dx 54
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1C 1F 80 00 37          ; dessin banque 1 frame 28 dy 31 dx 55
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1D 19 80 00 38          ; dessin banque 1 frame 29 dy 25 dx 56
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 5
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1E 19 80 00 38          ; dessin banque 1 frame 30 dy 25 dx 56
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 6
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 1F 1F 80 00 37          ; dessin banque 1 frame 31 dy 31 dx 55
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 7
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 20 25 80 00 35          ; dessin banque 1 frame 32 dy 37 dx 53
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 21 35 90 00 38          ; dessin banque 1 frame 33 dy 53 dx 56 [décor]
    04 1B 28 80 00 37          ; dessin banque 1 frame 27 dy 40 dx 55
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF FE                      ; fin d'étape, boucle
  ; étape 9
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 21 35 90 00 38          ; dessin banque 1 frame 33 dy 53 dx 56 [décor]
    04 1B 28 90 00 37          ; dessin banque 1 frame 27 dy 40 dx 55 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3C 36 10 00 79          ; dessin banque 1 frame 60 dy 54 dx 121 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 19 31 90 FF BC          ; dessin banque 1 frame 25 dy 49 dx -68 [décor]
    FF 00                      ; fin d'étape
  ; étape 10
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0818` (4:$26AA)

Rôles : cible de saut depuis LAB_0814 ; cible de saut depuis LAB_0815 ; cible de saut depuis LAB_0819

```
  ; étape 1
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 18 00 00 31          ; dessin banque 1 frame 64 dy 24 dx 49
    04 15 11 80 00 1B          ; dessin banque 1 frame 21 dy 17 dx 27
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F F9 00 00 4B          ; dessin banque 1 frame 63 dy -7 dx 75
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 07 2C 80 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 07 2C 90 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 3C 36 10 00 78          ; dessin banque 1 frame 60 dy 54 dx 120 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0819` (4:$2732)

Rôles : cible de saut depuis LAB_0815

```
  LAB_0818:
  ; étape 1
    D0 00                      ; $D0 Reset
    04 16 1E 00 00 04          ; dessin banque 1 frame 22 dy 30 dx 4
    04 40 18 00 00 31          ; dessin banque 1 frame 64 dy 24 dx 49
    04 15 11 80 00 1B          ; dessin banque 1 frame 21 dy 17 dx 27
    04 17 07 00 00 23          ; dessin banque 1 frame 23 dy 7 dx 35
    04 3F F9 00 00 4B          ; dessin banque 1 frame 63 dy -7 dx 75
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 00 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14
    04 07 2C 80 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78
    04 3B 37 00 00 62          ; dessin banque 1 frame 59 dy 55 dx 98
    04 3C 36 00 00 79          ; dessin banque 1 frame 60 dy 54 dx 121
    04 3B 37 00 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75
    04 3B 37 00 00 34          ; dessin banque 1 frame 59 dy 55 dx 52
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 32 90 00 43          ; dessin banque 1 frame 25 dy 50 dx 67 [décor]
    04 1A 24 10 00 0E          ; dessin banque 1 frame 26 dy 36 dx 14 [décor]
    04 07 2C 90 00 4E          ; dessin banque 1 frame 7 dy 44 dx 78 [décor]
    04 3B 37 10 00 62          ; dessin banque 1 frame 59 dy 55 dx 98 [décor]
    04 3B 37 10 00 4B          ; dessin banque 1 frame 59 dy 55 dx 75 [décor]
    04 3B 37 10 00 34          ; dessin banque 1 frame 59 dy 55 dx 52 [décor]
    04 3C 36 10 00 78          ; dessin banque 1 frame 60 dy 54 dx 120 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 5
    98 80 00 00 26 AA          ; $98 SkipIfDebug -> LAB_0818
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 FF 00 FF F7          ; dessin banque 1 frame 35 dy -1 dx -9
    04 22 10 00 FF EA          ; dessin banque 1 frame 34 dy 16 dx -22
    04 40 04 00 00 1E          ; dessin banque 1 frame 64 dy 4 dx 30
    04 3F E5 00 00 38          ; dessin banque 1 frame 63 dy -27 dx 56
    FF 00                      ; fin d'étape
  ; étape 6
    04 24 E7 00 FF F3          ; dessin banque 1 frame 36 dy -25 dx -13
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 7
    04 26 FD 00 FF F1          ; dessin banque 1 frame 38 dy -3 dx -15
    04 25 E1 00 FF FA          ; dessin banque 1 frame 37 dy -31 dx -6
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 8
    04 2A 20 00 00 00          ; dessin banque 1 frame 42 dy 32 dx 0
    04 29 1F 00 FF D9          ; dessin banque 1 frame 41 dy 31 dx -39
    04 28 0C 00 FF F3          ; dessin banque 1 frame 40 dy 12 dx -13
    04 27 EC 00 FF FE          ; dessin banque 1 frame 39 dy -20 dx -2
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 9
    04 2B 2C 00 FF D1          ; dessin banque 1 frame 43 dy 44 dx -47
    04 2C 21 00 FF F6          ; dessin banque 1 frame 44 dy 33 dx -10
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 10
    A4 37                      ; $A4 Sound
    04 2E 2B 00 FF F5          ; dessin banque 1 frame 46 dy 43 dx -11
    04 2B 2C 00 FF D0          ; dessin banque 1 frame 43 dy 44 dx -48
    04 2D 23 00 FF E5          ; dessin banque 1 frame 45 dy 35 dx -27
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 11
    04 31 27 00 FF D0          ; dessin banque 1 frame 49 dy 39 dx -48
    04 2F 2E 00 FF D2          ; dessin banque 1 frame 47 dy 46 dx -46
    04 3C 3B 00 00 50          ; dessin banque 1 frame 60 dy 59 dx 80
    04 3B 3C 00 00 39          ; dessin banque 1 frame 59 dy 60 dx 57
    04 3B 3C 00 00 22          ; dessin banque 1 frame 59 dy 60 dx 34
    04 3B 3C 00 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11
    FF 00                      ; fin d'étape
  ; étape 12
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 31 27 10 FF D0          ; dessin banque 1 frame 49 dy 39 dx -48 [décor]
    04 21 36 10 FF CE          ; dessin banque 1 frame 33 dy 54 dx -50 [décor]
    04 3C 3B 10 00 50          ; dessin banque 1 frame 60 dy 59 dx 80 [décor]
    04 3B 3C 10 00 39          ; dessin banque 1 frame 59 dy 60 dx 57 [décor]
    04 3B 3C 10 00 22          ; dessin banque 1 frame 59 dy 60 dx 34 [décor]
    04 3B 3C 10 00 0B          ; dessin banque 1 frame 59 dy 60 dx 11 [décor]
    FF 00                      ; fin d'étape
  ; étape 13
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_081A` (4:$287A)

Rôles : objet+22 (repos) dans LAB_0176 ; objet+26 (réaction) dans LAB_0176

```
  ; étape 1
    D0 00                      ; $D0 Reset
    04 35 32 40 FF F2          ; dessin banque 1 frame 53 dy 50 dx -14 [hors-boîte]
    04 33 1A 01 FF FE          ; dessin banque 1 frame 51 dy 26 dx -2 [corps]
    04 32 01 01 FF F1          ; dessin banque 1 frame 50 dy 1 dx -15 [corps]
    04 34 19 40 00 24          ; dessin banque 1 frame 52 dy 25 dx 36 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_081B` (4:$2896)

Rôles : référencé par le code dans LAB_0242

```
  ; étape 1
    04 39 30 40 FF FC          ; dessin banque 1 frame 57 dy 48 dx -4 [hors-boîte]
    04 36 04 01 00 00          ; dessin banque 1 frame 54 dy 4 dx 0 [corps]
    04 37 18 00 00 2F          ; dessin banque 1 frame 55 dy 24 dx 47
    04 38 17 42 00 51          ; dessin banque 1 frame 56 dy 23 dx 81 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 2
    88 03                      ; $88 Hold
    04 41 2B 40 FF FE          ; dessin banque 1 frame 65 dy 43 dx -2 [hors-boîte]
    04 3D 1E 01 00 03          ; dessin banque 1 frame 61 dy 30 dx 3 [corps]
    04 3A 05 01 00 13          ; dessin banque 1 frame 58 dy 5 dx 19 [corps]
    04 3B 1A 00 00 4E          ; dessin banque 1 frame 59 dy 26 dx 78
    04 3C 19 42 00 65          ; dessin banque 1 frame 60 dy 25 dx 101 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 3
    04 39 30 40 FF FC          ; dessin banque 1 frame 57 dy 48 dx -4 [hors-boîte]
    04 36 04 01 00 00          ; dessin banque 1 frame 54 dy 4 dx 0 [corps]
    04 37 18 00 00 2F          ; dessin banque 1 frame 55 dy 24 dx 47
    04 38 17 42 00 51          ; dessin banque 1 frame 56 dy 23 dx 81 [frappe,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_081C` (4:$28EC)

Rôles : référencé par le code dans LAB_01F7 ; référencé par le code dans LAB_024B

```
  ; étape 1
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    A4 36                      ; $A4 Sound
    A4 12                      ; $A4 Sound
    88 03                      ; $88 Hold
    00 0D 18 00 00 01          ; dessin banque 0 frame 13 dy 24 dx 1
    00 0C FB 00 00 14          ; dessin banque 0 frame 12 dy -5 dx 20
    00 10 06 00 00 30          ; dessin banque 0 frame 16 dy 6 dx 48
    00 0E E2 00 00 31          ; dessin banque 0 frame 14 dy -30 dx 49
    00 0F D6 00 00 49          ; dessin banque 0 frame 15 dy -42 dx 73
    00 12 2E 00 00 36          ; dessin banque 0 frame 18 dy 46 dx 54
    00 11 2D 00 00 30          ; dessin banque 0 frame 17 dy 45 dx 48
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    00 18 31 00 00 0A          ; dessin banque 0 frame 24 dy 49 dx 10
    00 17 15 00 00 01          ; dessin banque 0 frame 23 dy 21 dx 1
    00 16 F9 00 00 0C          ; dessin banque 0 frame 22 dy -7 dx 12
    00 15 E7 00 00 1D          ; dessin banque 0 frame 21 dy -25 dx 29
    00 13 B3 00 00 12          ; dessin banque 0 frame 19 dy -77 dx 18
    00 14 A7 00 00 0D          ; dessin banque 0 frame 20 dy -89 dx 13
    04 25 B1 00 00 34          ; dessin banque 1 frame 37 dy -79 dx 52
    04 25 EF 00 00 6E          ; dessin banque 1 frame 37 dy -17 dx 110
    04 27 D6 00 00 5B          ; dessin banque 1 frame 39 dy -42 dx 91
    FF 00                      ; fin d'étape
  ; étape 3
    88 01                      ; $88 Hold
    00 18 31 00 00 0A          ; dessin banque 0 frame 24 dy 49 dx 10
    00 17 15 00 00 01          ; dessin banque 0 frame 23 dy 21 dx 1
    00 16 F9 00 00 0C          ; dessin banque 0 frame 22 dy -7 dx 12
    00 15 E7 00 00 1D          ; dessin banque 0 frame 21 dy -25 dx 29
    00 13 B3 00 00 12          ; dessin banque 0 frame 19 dy -77 dx 18
    00 19 A9 00 00 06          ; dessin banque 0 frame 25 dy -87 dx 6
    FF 00                      ; fin d'étape
  ; étape 4
    88 01                      ; $88 Hold
    00 18 31 00 00 0A          ; dessin banque 0 frame 24 dy 49 dx 10
    00 17 15 00 00 01          ; dessin banque 0 frame 23 dy 21 dx 1
    00 16 F9 00 00 0C          ; dessin banque 0 frame 22 dy -7 dx 12
    00 15 E7 00 00 1D          ; dessin banque 0 frame 21 dy -25 dx 29
    00 13 B3 00 00 12          ; dessin banque 0 frame 19 dy -77 dx 18
    00 1A A5 00 00 13          ; dessin banque 0 frame 26 dy -91 dx 19
    04 04 B9 00 FF FD          ; dessin banque 1 frame 4 dy -71 dx -3
    04 25 AF 00 FF EE          ; dessin banque 1 frame 37 dy -81 dx -18
    FF 00                      ; fin d'étape
  ; étape 5
    88 01                      ; $88 Hold
    00 18 31 00 00 0A          ; dessin banque 0 frame 24 dy 49 dx 10
    00 17 15 00 00 01          ; dessin banque 0 frame 23 dy 21 dx 1
    00 16 F9 00 00 0C          ; dessin banque 0 frame 22 dy -7 dx 12
    00 15 E7 00 00 1D          ; dessin banque 0 frame 21 dy -25 dx 29
    00 13 B3 00 00 12          ; dessin banque 0 frame 19 dy -77 dx 18
    00 1B A6 00 00 0D          ; dessin banque 0 frame 27 dy -90 dx 13
    FF 00                      ; fin d'étape
  ; étape 6
    88 01                      ; $88 Hold
    00 1C EA 00 00 04          ; dessin banque 0 frame 28 dy -22 dx 4
    00 18 30 00 00 0A          ; dessin banque 0 frame 24 dy 48 dx 10
    00 13 BC 00 00 12          ; dessin banque 0 frame 19 dy -68 dx 18
    00 20 F0 00 00 1F          ; dessin banque 0 frame 32 dy -16 dx 31
    00 1D A6 00 00 13          ; dessin banque 0 frame 29 dy -90 dx 19
    FF 00                      ; fin d'étape
  ; étape 7
    88 01                      ; $88 Hold
    00 1C EA 00 00 04          ; dessin banque 0 frame 28 dy -22 dx 4
    00 18 30 00 00 0A          ; dessin banque 0 frame 24 dy 48 dx 10
    00 13 CB 00 00 12          ; dessin banque 0 frame 19 dy -53 dx 18
    00 20 FF 00 00 1F          ; dessin banque 0 frame 32 dy -1 dx 31
    00 1E A6 00 00 19          ; dessin banque 0 frame 30 dy -90 dx 25
    FF 00                      ; fin d'étape
  ; étape 8
    A4 12                      ; $A4 Sound
    88 01                      ; $88 Hold
    00 1C EA 00 00 04          ; dessin banque 0 frame 28 dy -22 dx 4
    00 18 30 00 00 0A          ; dessin banque 0 frame 24 dy 48 dx 10
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 20 10 00 00 1F          ; dessin banque 0 frame 32 dy 16 dx 31
    00 21 DC 00 00 17          ; dessin banque 0 frame 33 dy -36 dx 23
    00 1F 32 00 00 16          ; dessin banque 0 frame 31 dy 50 dx 22
    04 25 BB 00 00 18          ; dessin banque 1 frame 37 dy -69 dx 24
    FF 00                      ; fin d'étape
  ; étape 9
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    94 14                      ; $94 Loop
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 18 30 00 00 11          ; dessin banque 0 frame 24 dy 48 dx 17
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 10 00 00 1F          ; dessin banque 0 frame 32 dy 16 dx 31
    00 1F 32 00 00 16          ; dessin banque 0 frame 31 dy 50 dx 22
    00 22 DF 00 00 1B          ; dessin banque 0 frame 34 dy -33 dx 27
    FF 00                      ; fin d'étape
  ; étape 10
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 23 DF 00 00 1B          ; dessin banque 0 frame 35 dy -33 dx 27
    FF 00                      ; fin d'étape
  ; étape 11
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 24 DF 00 00 1A          ; dessin banque 0 frame 36 dy -33 dx 26
    FF 00                      ; fin d'étape
  ; étape 12
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 26 26 00 00 12          ; dessin banque 0 frame 38 dy 38 dx 18
    00 25 DF 00 00 1A          ; dessin banque 0 frame 37 dy -33 dx 26
    FF 00                      ; fin d'étape
  ; étape 13
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 27 2C 00 00 0F          ; dessin banque 0 frame 39 dy 44 dx 15
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 25 DF 00 00 1A          ; dessin banque 0 frame 37 dy -33 dx 26
    FF 00                      ; fin d'étape
  ; étape 14
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 28 35 10 00 0E          ; dessin banque 0 frame 40 dy 53 dx 14 [décor]
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 25 DF 00 00 1A          ; dessin banque 0 frame 37 dy -33 dx 26
    FF 00                      ; fin d'étape
  ; étape 15
    00 08 FD 00 00 04          ; dessin banque 0 frame 8 dy -3 dx 4
    00 20 0D 00 00 1F          ; dessin banque 0 frame 32 dy 13 dx 31
    00 1F 31 00 00 16          ; dessin banque 0 frame 31 dy 49 dx 22
    00 18 30 00 00 0F          ; dessin banque 0 frame 24 dy 48 dx 15
    00 13 DC 00 00 12          ; dessin banque 0 frame 19 dy -36 dx 18
    00 28 35 10 00 0E          ; dessin banque 0 frame 40 dy 53 dx 14 [décor]
    00 21 DB 00 00 17          ; dessin banque 0 frame 33 dy -37 dx 23
    FF FF                      ; fin du script
```

### `LAB_081D` (4:$2B80)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 00 FB 01 FF F0          ; dessin banque 0 frame 0 dy -5 dx -16 [corps]
    04 3E DE 00 00 04          ; dessin banque 1 frame 62 dy -34 dx 4
    04 42 11 20 00 02          ; dessin banque 1 frame 66 dy 17 dx 2
    FF FF                      ; fin du script
```

### `LAB_081E` (4:$2B94)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 01 FB 01 FF F0          ; dessin banque 0 frame 1 dy -5 dx -16 [corps]
    04 3E DE 00 00 09          ; dessin banque 1 frame 62 dy -34 dx 9
    04 42 11 20 00 06          ; dessin banque 1 frame 66 dy 17 dx 6
    FF FF                      ; fin du script
```

### `LAB_081F` (4:$2BA8)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche horizontal phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 02 FB 01 FF F0          ; dessin banque 0 frame 2 dy -5 dx -16 [corps]
    00 03 1C 01 FF E3          ; dessin banque 0 frame 3 dy 28 dx -29 [corps]
    04 3E DE 00 00 05          ; dessin banque 1 frame 62 dy -34 dx 5
    04 42 11 20 00 02          ; dessin banque 1 frame 66 dy 17 dx 2
    FF FF                      ; fin du script
```

### `LAB_0820` (4:$2BC2)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 3E D8 00 00 07          ; dessin banque 1 frame 62 dy -40 dx 7
    00 04 F7 01 FF F3          ; dessin banque 0 frame 4 dy -9 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0821` (4:$2BD0)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 3E D8 00 00 07          ; dessin banque 1 frame 62 dy -40 dx 7
    00 05 F7 01 FF F3          ; dessin banque 0 frame 5 dy -9 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0822` (4:$2BDE)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 3E D8 00 00 03          ; dessin banque 1 frame 62 dy -40 dx 3
    00 06 F7 01 FF F3          ; dessin banque 0 frame 6 dy -9 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0823` (4:$2BEC)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche haut phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 3E D8 00 00 06          ; dessin banque 1 frame 62 dy -40 dx 6
    00 07 F7 01 FF F3          ; dessin banque 0 frame 7 dy -9 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0824` (4:$2BFA)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 08 FC 01 FF EF          ; dessin banque 0 frame 8 dy -4 dx -17 [corps]
    04 3E DB 00 FF F5          ; dessin banque 1 frame 62 dy -37 dx -11
    04 42 13 20 FF F2          ; dessin banque 1 frame 66 dy 19 dx -14
    FF FF                      ; fin du script
```

### `LAB_0825` (4:$2C0E)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 09 FC 01 FF ED          ; dessin banque 0 frame 9 dy -4 dx -19 [corps]
    04 3E D9 00 FF F5          ; dessin banque 1 frame 62 dy -39 dx -11
    04 42 13 20 FF F1          ; dessin banque 1 frame 66 dy 19 dx -15
    FF FF                      ; fin du script
```

### `LAB_0826` (4:$2C22)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 0A FC 01 FF EC          ; dessin banque 0 frame 10 dy -4 dx -20 [corps]
    04 3E D9 00 FF F4          ; dessin banque 1 frame 62 dy -39 dx -12
    04 42 15 20 FF F1          ; dessin banque 1 frame 66 dy 21 dx -15
    FF FF                      ; fin du script
```

### `LAB_0827` (4:$2C36)

Rôles : LAB_060D [objet+46 (marche) ; posée par LAB_0176] : marche bas phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 0B FC 01 FF ED          ; dessin banque 0 frame 11 dy -4 dx -19 [corps]
    04 3E D9 00 FF F4          ; dessin banque 1 frame 62 dy -39 dx -12
    04 42 15 20 FF F1          ; dessin banque 1 frame 66 dy 21 dx -15
    FF FF                      ; fin du script
```

### `LAB_0828` (4:$2C4A)

Rôles : cible de saut depuis LAB_0836 ; objet+22 (repos) dans LAB_0170 ; objet+26 (réaction) dans LAB_0170

```
  ; étape 1
    00 00 04 01 FF E6          ; dessin banque 0 frame 0 dy 4 dx -26 [corps]
    00 01 1E 01 00 0B          ; dessin banque 0 frame 1 dy 30 dx 11 [corps]
    04 45 F4 00 FF EF          ; dessin banque 1 frame 69 dy -12 dx -17
    FF FF                      ; fin du script
```

### `LAB_0829` (4:$2C5E)

Rôles : LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 1 : feu + D+B ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 2 : feu + D ou D+B+H ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 3 : feu + G+H ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 4 : feu + G+B ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 5 : feu + G ou B+H ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 7 : feu + B ou D+G+B+H ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    88 01                      ; $88 Hold
    00 02 04 01 FF E9          ; dessin banque 0 frame 2 dy 4 dx -23 [corps]
    00 03 1D 01 00 0A          ; dessin banque 0 frame 3 dy 29 dx 10 [corps]
    04 46 09 00 FF CB          ; dessin banque 1 frame 70 dy 9 dx -53
    FF 00                      ; fin d'étape
  ; étape 2
    A4 0C                      ; $A4 Sound
    00 04 06 01 FF F9          ; dessin banque 0 frame 4 dy 6 dx -7 [corps]
    04 32 17 42 FF E8          ; dessin banque 1 frame 50 dy 23 dx -24 [frappe,hors-boîte]
    04 47 1B 02 00 0F          ; dessin banque 1 frame 71 dy 27 dx 15 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    00 06 0C 01 00 13          ; dessin banque 0 frame 6 dy 12 dx 19 [corps]
    00 05 23 41 FF F8          ; dessin banque 0 frame 5 dy 35 dx -8 [corps,hors-boîte]
    04 33 1F 02 00 21          ; dessin banque 1 frame 51 dy 31 dx 33 [frappe]
    04 48 1A 02 00 3C          ; dessin banque 1 frame 72 dy 26 dx 60 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    00 08 0A 01 00 16          ; dessin banque 0 frame 8 dy 10 dx 22 [corps]
    00 07 1F 41 FF FB          ; dessin banque 0 frame 7 dy 31 dx -5 [corps,hors-boîte]
    04 34 03 42 00 32          ; dessin banque 1 frame 52 dy 3 dx 50 [frappe,hors-boîte]
    04 49 FE 02 00 2B          ; dessin banque 1 frame 73 dy -2 dx 43 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    88 01                      ; $88 Hold
    00 08 0A 01 00 16          ; dessin banque 0 frame 8 dy 10 dx 22 [corps]
    00 07 1F 01 FF FB          ; dessin banque 0 frame 7 dy 31 dx -5 [corps]
    04 49 FE 40 00 2B          ; dessin banque 1 frame 73 dy -2 dx 43 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_082A` (4:$2CDA)

Rôles : LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 6 : feu + D+H ; LAB_05FE [objet+34 (attaques) ; posée par LAB_0170] : attaque 8 : feu + H ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 4A E3 02 00 16          ; dessin banque 1 frame 74 dy -29 dx 22 [frappe]
    00 09 00 01 FF FD          ; dessin banque 0 frame 9 dy 0 dx -3 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    A4 0C                      ; $A4 Sound
    00 18 2F 00 FF F8          ; dessin banque 0 frame 24 dy 47 dx -8
    00 0B 14 01 00 08          ; dessin banque 0 frame 11 dy 20 dx 8 [corps]
    00 0A FD 01 FF EE          ; dessin banque 0 frame 10 dy -3 dx -18 [corps]
    04 4B F0 00 FF C6          ; dessin banque 1 frame 75 dy -16 dx -58
    FF 00                      ; fin d'étape
  ; étape 3
    00 0E 2E 00 FF F8          ; dessin banque 0 frame 14 dy 46 dx -8
    00 0D 0D 01 00 0B          ; dessin banque 0 frame 13 dy 13 dx 11 [corps]
    00 0C F8 01 FF FE          ; dessin banque 0 frame 12 dy -8 dx -2 [corps]
    04 4C E5 40 FF D8          ; dessin banque 1 frame 76 dy -27 dx -40 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    00 10 16 01 00 0C          ; dessin banque 0 frame 16 dy 22 dx 12 [corps]
    00 0F F7 01 00 1F          ; dessin banque 0 frame 15 dy -9 dx 31 [corps]
    00 11 30 00 00 2D          ; dessin banque 0 frame 17 dy 48 dx 45
    00 12 32 00 FF FF          ; dessin banque 0 frame 18 dy 50 dx -1
    04 35 CD 02 FF FE          ; dessin banque 1 frame 53 dy -51 dx -2 [frappe]
    04 4D D8 43 00 42          ; dessin banque 1 frame 77 dy -40 dx 66 [corps,frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 5
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 15 13 01 00 2A          ; dessin banque 0 frame 21 dy 19 dx 42 [corps]
    04 4E 2A 42 00 51          ; dessin banque 1 frame 78 dy 42 dx 81 [frappe,hors-boîte]
    04 36 FF 42 00 72          ; dessin banque 1 frame 54 dy -1 dx 114 [frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 6
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 16 14 01 00 24          ; dessin banque 0 frame 22 dy 20 dx 36 [corps]
    04 4E 2A 43 00 51          ; dessin banque 1 frame 78 dy 42 dx 81 [corps,frappe,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 7
    00 13 1A 01 00 09          ; dessin banque 0 frame 19 dy 26 dx 9 [corps]
    00 14 1F 01 00 2A          ; dessin banque 0 frame 20 dy 31 dx 42 [corps]
    00 17 16 01 00 24          ; dessin banque 0 frame 23 dy 22 dx 36 [corps]
    04 4E 2A 41 00 51          ; dessin banque 1 frame 78 dy 42 dx 81 [corps,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_082B` (4:$2D9E)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 19 00 01 FF EF          ; dessin banque 0 frame 25 dy 0 dx -17 [corps]
    04 4A FA 01 FF FF          ; dessin banque 1 frame 74 dy -6 dx -1 [corps]
    FF FF                      ; fin du script
```

### `LAB_082C` (4:$2DAC)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 1A 00 01 FF EE          ; dessin banque 0 frame 26 dy 0 dx -18 [corps]
    04 4A FC 00 00 02          ; dessin banque 1 frame 74 dy -4 dx 2
    FF FF                      ; fin du script
```

### `LAB_082D` (4:$2DBA)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche horizontal phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 1B 00 01 FF EF          ; dessin banque 0 frame 27 dy 0 dx -17 [corps]
    00 1C 21 01 FF E2          ; dessin banque 0 frame 28 dy 33 dx -30 [corps]
    04 4A FC 00 FF FE          ; dessin banque 1 frame 74 dy -4 dx -2
    FF FF                      ; fin du script
```

### `LAB_082E` (4:$2DCE)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 4F EF 00 00 05          ; dessin banque 1 frame 79 dy -17 dx 5
    00 1D FC 01 FF F3          ; dessin banque 0 frame 29 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_082F` (4:$2DDC)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 4F EF 00 00 03          ; dessin banque 1 frame 79 dy -17 dx 3
    00 1E FC 01 FF F3          ; dessin banque 0 frame 30 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0830` (4:$2DEA)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 4F EF 00 00 00          ; dessin banque 1 frame 79 dy -17 dx 0
    00 1F FC 01 FF F3          ; dessin banque 0 frame 31 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0831` (4:$2DF8)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche haut phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    04 4F EF 00 00 02          ; dessin banque 1 frame 79 dy -17 dx 2
    00 20 FC 01 FF F3          ; dessin banque 0 frame 32 dy -4 dx -13 [corps]
    FF FF                      ; fin du script
```

### `LAB_0832` (4:$2E06)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 0 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 21 01 01 FF EF          ; dessin banque 0 frame 33 dy 1 dx -17 [corps]
    04 4F FE 00 FF F1          ; dessin banque 1 frame 79 dy -2 dx -15
    FF FF                      ; fin du script
```

### `LAB_0833` (4:$2E14)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 1 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 22 01 01 FF ED          ; dessin banque 0 frame 34 dy 1 dx -19 [corps]
    04 4F FE 00 FF F1          ; dessin banque 1 frame 79 dy -2 dx -15
    FF FF                      ; fin du script
```

### `LAB_0834` (4:$2E22)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 2 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 23 01 01 FF ED          ; dessin banque 0 frame 35 dy 1 dx -19 [corps]
    04 4F FE 00 FF F3          ; dessin banque 1 frame 79 dy -2 dx -13
    FF FF                      ; fin du script
```

### `LAB_0835` (4:$2E30)

Rôles : LAB_060E [objet+46 (marche) ; posée par LAB_0170] : marche bas phase 3 ; référencé par le code dans LAB_0156

```
  ; étape 1
    00 24 01 01 FF EE          ; dessin banque 0 frame 36 dy 1 dx -18 [corps]
    04 4F FE 00 FF F2          ; dessin banque 1 frame 79 dy -2 dx -14
    FF FF                      ; fin du script
```

### `LAB_0836` (4:$2E3E)

Rôles : LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [3] ; LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [6] ; LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [8] ; référencé par le code dans LAB_0156

```
  LAB_0828:
  ; étape 1
    00 00 04 01 FF E6          ; dessin banque 0 frame 0 dy 4 dx -26 [corps]
    00 01 1E 01 00 0B          ; dessin banque 0 frame 1 dy 30 dx 11 [corps]
    04 45 F4 00 FF EF          ; dessin banque 1 frame 69 dy -12 dx -17
    FF FF                      ; fin du script
  ; étape 2
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 4A 04 00 00 17          ; dessin banque 1 frame 74 dy 4 dx 23
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 01 11 80 FF FC          ; dessin banque 1 frame 1 dy 17 dx -4
    FF 00                      ; fin d'étape
  ; étape 3
    04 4A 04 00 00 17          ; dessin banque 1 frame 74 dy 4 dx 23
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 03 17 80 00 22          ; dessin banque 1 frame 3 dy 23 dx 34
    04 04 1D 80 FF F8          ; dessin banque 1 frame 4 dy 29 dx -8
    04 05 1E 80 00 24          ; dessin banque 1 frame 5 dy 30 dx 36
    04 02 15 80 00 0C          ; dessin banque 1 frame 2 dy 21 dx 12
    FF 00                      ; fin d'étape
  ; étape 4
    04 4A 04 00 00 17          ; dessin banque 1 frame 74 dy 4 dx 23
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    04 07 31 80 00 30          ; dessin banque 1 frame 7 dy 49 dx 48
    04 06 14 80 00 0B          ; dessin banque 1 frame 6 dy 20 dx 11
    FF 00                      ; fin d'étape
  ; étape 5
    04 4A 04 00 00 17          ; dessin banque 1 frame 74 dy 4 dx 23
    04 08 2E 80 00 2B          ; dessin banque 1 frame 8 dy 46 dx 43
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 4A 04 00 00 17          ; dessin banque 1 frame 74 dy 4 dx 23
    04 0A 36 90 00 2B          ; dessin banque 1 frame 10 dy 54 dx 43 [décor]
    04 00 08 00 FF FD          ; dessin banque 1 frame 0 dy 8 dx -3
    FF 00                      ; fin d'étape
  ; étape 7
    B4 01 00 00 30 A6          ; $B4 IfDead -> LAB_083A
    84 03 00 00 2C 4A          ; $84 Jump -> LAB_0828
  LAB_083A:
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    FF 00                      ; fin d'étape
  ; étape 8
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    FF 00                      ; fin d'étape
  ; étape 9
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 4E 31 50 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 10
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0837` (4:$2ED0)

Rôles : LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [2] ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    B4 01 00 00 31 04          ; $B4 IfDead -> LAB_083B
    04 4F FC 00 00 18          ; dessin banque 1 frame 79 dy -4 dx 24
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 0C 1A 80 FF F7          ; dessin banque 1 frame 12 dy 26 dx -9
    FF 00                      ; fin d'étape
  ; étape 2
    04 4F FC 00 00 18          ; dessin banque 1 frame 79 dy -4 dx 24
    04 0D 1E 80 FF E4          ; dessin banque 1 frame 13 dy 30 dx -28
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 05 22 80 00 28          ; dessin banque 1 frame 5 dy 34 dx 40
    04 0E 1C 80 00 08          ; dessin banque 1 frame 14 dy 28 dx 8
    FF 00                      ; fin d'étape
  ; étape 3
    04 4F FC 00 00 18          ; dessin banque 1 frame 79 dy -4 dx 24
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 08 30 80 00 34          ; dessin banque 1 frame 8 dy 48 dx 52
    04 0F 1C 80 00 08          ; dessin banque 1 frame 15 dy 28 dx 8
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 4F FC 00 00 18          ; dessin banque 1 frame 79 dy -4 dx 24
    04 0A 38 90 00 33          ; dessin banque 1 frame 10 dy 56 dx 51 [décor]
    04 0B 03 00 00 00          ; dessin banque 1 frame 11 dy 3 dx 0
    04 07 1D 80 00 0A          ; dessin banque 1 frame 7 dy 29 dx 10
    FF FF                      ; fin du script
  LAB_083A:
  ; étape 5
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    FF 00                      ; fin d'étape
  ; étape 7
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 4E 31 50 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 8
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  LAB_083B:
  ; étape 9
    98 80 00 00 30 A6          ; $98 SkipIfDebug -> LAB_083A
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 03 00 FF F7          ; dessin banque 1 frame 35 dy 3 dx -9
    04 22 14 00 FF EA          ; dessin banque 1 frame 34 dy 20 dx -22
    04 4D FB 00 00 29          ; dessin banque 1 frame 77 dy -5 dx 41
    FF 00                      ; fin d'étape
  ; étape 10
    04 24 EB 00 FF F3          ; dessin banque 1 frame 36 dy -21 dx -13
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 11
    04 26 01 00 FF EF          ; dessin banque 1 frame 38 dy 1 dx -17
    04 25 E5 00 FF FA          ; dessin banque 1 frame 37 dy -27 dx -6
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 12
    04 2A 24 00 00 00          ; dessin banque 1 frame 42 dy 36 dx 0
    04 29 23 00 FF D9          ; dessin banque 1 frame 41 dy 35 dx -39
    04 28 10 00 FF F3          ; dessin banque 1 frame 40 dy 16 dx -13
    04 27 F0 00 FF FE          ; dessin banque 1 frame 39 dy -16 dx -2
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 13
    A4 37                      ; $A4 Sound
    04 2B 30 00 FF D1          ; dessin banque 1 frame 43 dy 48 dx -47
    04 2C 25 00 FF F6          ; dessin banque 1 frame 44 dy 37 dx -10
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 14
    04 2E 2F 00 FF F5          ; dessin banque 1 frame 46 dy 47 dx -11
    04 2B 30 00 FF D0          ; dessin banque 1 frame 43 dy 48 dx -48
    04 2D 27 00 FF E5          ; dessin banque 1 frame 45 dy 39 dx -27
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 15
    04 31 2B 00 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48
    04 2F 32 00 FF D2          ; dessin banque 1 frame 47 dy 50 dx -46
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 16
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 06                      ; $88 Hold
    04 31 2B 10 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48 [décor]
    04 21 3A 10 FF CE          ; dessin banque 1 frame 33 dy 58 dx -50 [décor]
    04 4E 31 10 00 36          ; dessin banque 1 frame 78 dy 49 dx 54 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0838` (4:$2F46)

Rôles : LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [1] ; LAB_05FF [objet+30 (scripts objet+30) ; posée par LAB_0170] : [5] ; référencé par le code dans LAB_0156

```
  ; étape 1
    B0 00 00 00 72 C8          ; $B0 Call -> LAB_02E9
    04 4D 00 00 00 28          ; dessin banque 1 frame 77 dy 0 dx 40
    04 11 13 80 FF EE          ; dessin banque 1 frame 17 dy 19 dx -18
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    FF 00                      ; fin d'étape
  ; étape 2
    04 4D 00 00 00 28          ; dessin banque 1 frame 77 dy 0 dx 40
    04 12 11 80 FF E8          ; dessin banque 1 frame 18 dy 17 dx -24
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    04 13 18 80 00 01          ; dessin banque 1 frame 19 dy 24 dx 1
    FF 00                      ; fin d'étape
  ; étape 3
    04 4D 00 40 00 28          ; dessin banque 1 frame 77 dy 0 dx 40 [hors-boîte]
    04 14 2E 80 FF CE          ; dessin banque 1 frame 20 dy 46 dx -50
    04 15 1A 80 FF F8          ; dessin banque 1 frame 21 dy 26 dx -8
    04 04 1D 80 FF D7          ; dessin banque 1 frame 4 dy 29 dx -41
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    FF 00                      ; fin d'étape
  ; étape 4
    B4 01 00 00 2F BC          ; $B4 IfDead -> LAB_0839
    88 02                      ; $88 Hold
    04 4D 00 40 00 28          ; dessin banque 1 frame 77 dy 0 dx 40 [hors-boîte]
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    04 15 1A 80 FF F8          ; dessin banque 1 frame 21 dy 26 dx -8
    04 10 09 00 FF FD          ; dessin banque 1 frame 16 dy 9 dx -3
    FF FF                      ; fin du script
  LAB_0839:
  ; étape 5
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    04 18 1A 80 FF F0          ; dessin banque 1 frame 24 dy 26 dx -16
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    FF 00                      ; fin d'étape
  ; étape 6
    A4 37                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 0D 26 80 FF DD          ; dessin banque 1 frame 13 dy 38 dx -35
    04 1B 2C 80 00 36          ; dessin banque 1 frame 27 dy 44 dx 54
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    04 19 35 00 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67
    FF 00                      ; fin d'étape
  ; étape 7
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1C 25 80 00 38          ; dessin banque 1 frame 28 dy 37 dx 56
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1D 1F 80 00 39          ; dessin banque 1 frame 29 dy 31 dx 57
    FF 00                      ; fin d'étape
  ; étape 9
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1E 1F 80 00 39          ; dessin banque 1 frame 30 dy 31 dx 57
    FF 00                      ; fin d'étape
  ; étape 10
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1F 25 80 00 38          ; dessin banque 1 frame 31 dy 37 dx 56
    FF 00                      ; fin d'étape
  ; étape 11
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 20 2A 80 00 35          ; dessin banque 1 frame 32 dy 42 dx 53
    FF 00                      ; fin d'étape
  ; étape 12
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 21 3B 90 00 38          ; dessin banque 1 frame 33 dy 59 dx 56 [décor]
    04 1B 2C 80 00 37          ; dessin banque 1 frame 27 dy 44 dx 55
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 1A 28 10 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor]
    04 21 3B 90 00 38          ; dessin banque 1 frame 33 dy 59 dx 56 [décor]
    04 1B 2C 90 00 37          ; dessin banque 1 frame 27 dy 44 dx 55 [décor]
    04 4E 31 10 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor]
    FF 00                      ; fin d'étape
  ; étape 14
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0839` (4:$2FBC)

Rôles : cible de saut depuis LAB_0838

```
  ; étape 1
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    04 18 1A 80 FF F0          ; dessin banque 1 frame 24 dy 26 dx -16
    04 19 35 90 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67 [décor]
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 0D 26 80 FF DD          ; dessin banque 1 frame 13 dy 38 dx -35
    04 1B 2C 80 00 36          ; dessin banque 1 frame 27 dy 44 dx 54
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    04 19 35 00 FF BD          ; dessin banque 1 frame 25 dy 53 dx -67
    FF 00                      ; fin d'étape
  ; étape 3
    94 05                      ; $94 Loop
    A4 34                      ; $A4 Sound
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1C 25 80 00 38          ; dessin banque 1 frame 28 dy 37 dx 56
    FF 00                      ; fin d'étape
  ; étape 4
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1D 1F 80 00 39          ; dessin banque 1 frame 29 dy 31 dx 57
    FF 00                      ; fin d'étape
  ; étape 5
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1E 1F 80 00 39          ; dessin banque 1 frame 30 dy 31 dx 57
    FF 00                      ; fin d'étape
  ; étape 6
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 1F 25 80 00 38          ; dessin banque 1 frame 31 dy 37 dx 56
    FF 00                      ; fin d'étape
  ; étape 7
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    04 20 2A 80 00 35          ; dessin banque 1 frame 32 dy 42 dx 53
    FF 00                      ; fin d'étape
  ; étape 8
    04 1A 28 40 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [hors-boîte]
    04 21 3B 90 00 38          ; dessin banque 1 frame 33 dy 59 dx 56 [décor]
    04 1B 2C 80 00 37          ; dessin banque 1 frame 27 dy 44 dx 55
    04 4E 31 40 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [hors-boîte]
    FF FE                      ; fin d'étape, boucle
  ; étape 9
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 1A 28 10 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor]
    04 21 3B 90 00 38          ; dessin banque 1 frame 33 dy 59 dx 56 [décor]
    04 1B 2C 90 00 37          ; dessin banque 1 frame 27 dy 44 dx 55 [décor]
    04 4E 31 10 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor]
    FF 00                      ; fin d'étape
  ; étape 10
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_083A` (4:$30A6)

Rôles : cible de saut depuis LAB_0836 ; cible de saut depuis LAB_0837 ; cible de saut depuis LAB_083B

```
  ; étape 1
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 4E 31 50 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_083B` (4:$3104)

Rôles : cible de saut depuis LAB_0837

```
  LAB_083A:
  ; étape 1
    A4 3D                      ; $A4 Sound
    04 16 22 00 00 04          ; dessin banque 1 frame 22 dy 34 dx 4
    04 17 0B 00 00 23          ; dessin banque 1 frame 23 dy 11 dx 35
    04 15 15 80 00 1B          ; dessin banque 1 frame 21 dy 21 dx 27
    04 4D 09 00 00 3B          ; dessin banque 1 frame 77 dy 9 dx 59
    FF 00                      ; fin d'étape
  ; étape 2
    A4 37                      ; $A4 Sound
    04 19 36 90 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor]
    04 1A 28 00 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14
    04 07 30 80 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78
    04 4E 31 00 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 19 36 D0 00 43          ; dessin banque 1 frame 25 dy 54 dx 67 [décor,hors-boîte]
    04 1A 28 50 00 0E          ; dessin banque 1 frame 26 dy 40 dx 14 [décor,hors-boîte]
    04 07 30 D0 00 4E          ; dessin banque 1 frame 7 dy 48 dx 78 [décor,hors-boîte]
    04 4E 31 50 00 4B          ; dessin banque 1 frame 78 dy 49 dx 75 [décor,hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 5
    98 80 00 00 30 A6          ; $98 SkipIfDebug -> LAB_083A
    A4 35                      ; $A4 Sound
    A4 34                      ; $A4 Sound
    04 23 03 00 FF F7          ; dessin banque 1 frame 35 dy 3 dx -9
    04 22 14 00 FF EA          ; dessin banque 1 frame 34 dy 20 dx -22
    04 4D FB 00 00 29          ; dessin banque 1 frame 77 dy -5 dx 41
    FF 00                      ; fin d'étape
  ; étape 6
    04 24 EB 00 FF F3          ; dessin banque 1 frame 36 dy -21 dx -13
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 7
    04 26 01 00 FF EF          ; dessin banque 1 frame 38 dy 1 dx -17
    04 25 E5 00 FF FA          ; dessin banque 1 frame 37 dy -27 dx -6
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 8
    04 2A 24 00 00 00          ; dessin banque 1 frame 42 dy 36 dx 0
    04 29 23 00 FF D9          ; dessin banque 1 frame 41 dy 35 dx -39
    04 28 10 00 FF F3          ; dessin banque 1 frame 40 dy 16 dx -13
    04 27 F0 00 FF FE          ; dessin banque 1 frame 39 dy -16 dx -2
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 9
    A4 37                      ; $A4 Sound
    04 2B 30 00 FF D1          ; dessin banque 1 frame 43 dy 48 dx -47
    04 2C 25 00 FF F6          ; dessin banque 1 frame 44 dy 37 dx -10
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 10
    04 2E 2F 00 FF F5          ; dessin banque 1 frame 46 dy 47 dx -11
    04 2B 30 00 FF D0          ; dessin banque 1 frame 43 dy 48 dx -48
    04 2D 27 00 FF E5          ; dessin banque 1 frame 45 dy 39 dx -27
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 11
    04 31 2B 00 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48
    04 2F 32 00 FF D2          ; dessin banque 1 frame 47 dy 50 dx -46
    04 4E 31 00 00 36          ; dessin banque 1 frame 78 dy 49 dx 54
    FF 00                      ; fin d'étape
  ; étape 12
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 06                      ; $88 Hold
    04 31 2B 10 FF D0          ; dessin banque 1 frame 49 dy 43 dx -48 [décor]
    04 21 3A 10 FF CE          ; dessin banque 1 frame 33 dy 58 dx -50 [décor]
    04 4E 31 10 00 36          ; dessin banque 1 frame 78 dy 49 dx 54 [décor]
    FF 00                      ; fin d'étape
  ; étape 13
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_083C` (4:$31C8)

Rôles : entrée 0 de la table LAB_015E

```
  ; étape 1
    A4 92                      ; $A4 Sound
    00 00 0B 03 FF E7          ; dessin banque 0 frame 0 dy 11 dx -25 [corps,frappe]
    00 0E 0B 00 FF D4          ; dessin banque 0 frame 14 dy 11 dx -44
    FF FF                      ; fin du script
```

### `LAB_083D` (4:$31D8)

Rôles : entrée 1 de la table LAB_015E

```
  ; étape 1
    00 01 07 03 FF D4          ; dessin banque 0 frame 1 dy 7 dx -44 [corps,frappe]
    00 02 06 03 00 12          ; dessin banque 0 frame 2 dy 6 dx 18 [corps,frappe]
    00 10 16 00 FF BD          ; dessin banque 0 frame 16 dy 22 dx -67
    FF FF                      ; fin du script
```

### `LAB_083E` (4:$31EC)

Rôles : entrée 2 de la table LAB_015E

```
  ; étape 1
    00 03 19 01 FF CE          ; dessin banque 0 frame 3 dy 25 dx -50 [corps]
    00 04 0B 03 FF F2          ; dessin banque 0 frame 4 dy 11 dx -14 [corps,frappe]
    00 12 29 00 FF B9          ; dessin banque 0 frame 18 dy 41 dx -71
    FF FF                      ; fin du script
```

### `LAB_083F` (4:$3200)

Rôles : entrée 3 de la table LAB_015E

```
  ; étape 1
    00 05 07 03 FF DE          ; dessin banque 0 frame 5 dy 7 dx -34 [corps,frappe]
    00 0E 02 00 FF D4          ; dessin banque 0 frame 14 dy 2 dx -44
    FF FF                      ; fin du script
```

### `LAB_0840` (4:$320E)

Rôles : LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [4] ; entrée 0 de la table LAB_015E+20 ; objet+22 (repos) dans LAB_018B ; référencé par le code dans LAB_0156

```
  ; étape 1
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 1E 1C 00 00 1A          ; dessin banque 0 frame 30 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0841:
  ; étape 2
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 1F 1C 00 00 1A          ; dessin banque 0 frame 31 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0842:
  ; étape 3
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 20 1C 00 00 1A          ; dessin banque 0 frame 32 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0843:
  ; étape 4
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 21 1B 00 00 1A          ; dessin banque 0 frame 33 dy 27 dx 26
    FF FF                      ; fin du script
```

### `LAB_0841` (4:$321E)

Rôles : entrée 1 de la table LAB_015E+20

```
  ; étape 1
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 1F 1C 00 00 1A          ; dessin banque 0 frame 31 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0842:
  ; étape 2
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 20 1C 00 00 1A          ; dessin banque 0 frame 32 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0843:
  ; étape 3
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 21 1B 00 00 1A          ; dessin banque 0 frame 33 dy 27 dx 26
    FF FF                      ; fin du script
```

### `LAB_0842` (4:$322E)

Rôles : entrée 2 de la table LAB_015E+20

```
  ; étape 1
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 20 1C 00 00 1A          ; dessin banque 0 frame 32 dy 28 dx 26
    FF 00                      ; fin d'étape
  LAB_0843:
  ; étape 2
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 21 1B 00 00 1A          ; dessin banque 0 frame 33 dy 27 dx 26
    FF FF                      ; fin du script
```

### `LAB_0843` (4:$323E)

Rôles : entrée 3 de la table LAB_015E+20

```
  ; étape 1
    88 03                      ; $88 Hold
    00 1D 0C 01 FF DF          ; dessin banque 0 frame 29 dy 12 dx -33 [corps]
    00 21 1B 00 00 1A          ; dessin banque 0 frame 33 dy 27 dx 26
    FF FF                      ; fin du script
```

### `LAB_0844` (4:$324E)

Rôles : cible de saut depuis LAB_0845 ; cible de saut depuis LAB_0847 ; cible de saut depuis LAB_0849 ; objet+26 (réaction) dans LAB_018B

```
  ; étape 1
    A4 8F                      ; $A4 Sound
    00 09 0C 01 FF F5          ; dessin banque 0 frame 9 dy 12 dx -11 [corps]
    00 08 12 01 FF E2          ; dessin banque 0 frame 8 dy 18 dx -30 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    A0 10 00 00 00 00 00 0F    ; $A0 Move
    00 14 13 03 FF C0          ; dessin banque 0 frame 20 dy 19 dx -64 [corps,frappe]
    80 FF                      ; $80 SetDir
    FF FF                      ; fin du script
```

### `LAB_0845` (4:$3270)

Rôles : LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [6] ; LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [8] ; référencé par le code dans LAB_0156

```
  LAB_0844:
  ; étape 1
    A4 8F                      ; $A4 Sound
    00 09 0C 01 FF F5          ; dessin banque 0 frame 9 dy 12 dx -11 [corps]
    00 08 12 01 FF E2          ; dessin banque 0 frame 8 dy 18 dx -30 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    A0 10 00 00 00 00 00 0F    ; $A0 Move
    00 14 13 03 FF C0          ; dessin banque 0 frame 20 dy 19 dx -64 [corps,frappe]
    80 FF                      ; $80 SetDir
    FF FF                      ; fin du script
  ; étape 3
    A4 93                      ; $A4 Sound
    00 1C 14 00 FF D3          ; dessin banque 0 frame 28 dy 20 dx -45
    00 59 1A 80 00 01          ; dessin banque 0 frame 89 dy 26 dx 1
    00 58 08 80 FF EA          ; dessin banque 0 frame 88 dy 8 dx -22
    FF 00                      ; fin d'étape
  ; étape 4
    A0 01 00 18 00 00 00 00    ; $A0 Move
    00 1C 14 00 FF D3          ; dessin banque 0 frame 28 dy 20 dx -45
    00 5B 0F 80 00 05          ; dessin banque 0 frame 91 dy 15 dx 5
    00 5A 07 80 FF D7          ; dessin banque 0 frame 90 dy 7 dx -41
    FF 00                      ; fin d'étape
  ; étape 5
    A0 01 00 10 00 00 00 00    ; $A0 Move
    00 1C 14 00 FF D3          ; dessin banque 0 frame 28 dy 20 dx -45
    00 5D 19 80 00 0A          ; dessin banque 0 frame 93 dy 25 dx 10
    00 5C 0D 80 FF D0          ; dessin banque 0 frame 92 dy 13 dx -48
    FF 00                      ; fin d'étape
  ; étape 6
    A0 01 00 0A 00 00 00 00    ; $A0 Move
    00 1C 14 00 FF D3          ; dessin banque 0 frame 28 dy 20 dx -45
    00 5E 1B 80 00 0B          ; dessin banque 0 frame 94 dy 27 dx 11
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 33 18          ; $B4 IfDead -> LAB_0846
    88 03                      ; $88 Hold
    00 19 18 00 FF F5          ; dessin banque 0 frame 25 dy 24 dx -11
    00 18 20 00 FF CD          ; dessin banque 0 frame 24 dy 32 dx -51
    00 5F 1F 80 00 0B          ; dessin banque 0 frame 95 dy 31 dx 11
    00 60 32 80 FF FE          ; dessin banque 0 frame 96 dy 50 dx -2
    00 3F 21 00 00 03          ; dessin banque 0 frame 63 dy 33 dx 3
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    84 00 00 00 32 4E          ; $84 Jump -> LAB_0844
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 45 32 90 FF EE          ; dessin banque 0 frame 69 dy 50 dx -18 [décor]
    FF FF                      ; fin du script
  LAB_0846:
  ; étape 9
    88 03                      ; $88 Hold
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 46 0C 80 00 0C          ; dessin banque 0 frame 70 dy 12 dx 12
    FF 00                      ; fin d'étape
  ; étape 10
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 34 08 80 00 0C          ; dessin banque 0 frame 52 dy 8 dx 12
    00 4F 36 00 00 02          ; dessin banque 0 frame 79 dy 54 dx 2
    00 48 1D 00 00 0A          ; dessin banque 0 frame 72 dy 29 dx 10
    FF 00                      ; fin d'étape
  ; étape 11
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4C 2C 80 00 0A          ; dessin banque 0 frame 76 dy 44 dx 10
    00 4D 2C 80 00 0B          ; dessin banque 0 frame 77 dy 44 dx 11
    FF 00                      ; fin d'étape
  ; étape 12
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4E 2C 80 00 0B          ; dessin banque 0 frame 78 dy 44 dx 11
    00 4F 35 80 00 03          ; dessin banque 0 frame 79 dy 53 dx 3
    FF 00                      ; fin d'étape
  ; étape 13
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 50 2D 80 00 0C          ; dessin banque 0 frame 80 dy 45 dx 12
    00 51 36 80 FF FF          ; dessin banque 0 frame 81 dy 54 dx -1
    FF 00                      ; fin d'étape
  ; étape 14
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 52 2D 80 00 0C          ; dessin banque 0 frame 82 dy 45 dx 12
    00 53 36 80 FF FD          ; dessin banque 0 frame 83 dy 54 dx -3
    FF 00                      ; fin d'étape
  ; étape 15
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 54 2D 80 00 0B          ; dessin banque 0 frame 84 dy 45 dx 11
    00 55 36 80 FF FE          ; dessin banque 0 frame 85 dy 54 dx -2
    FF 00                      ; fin d'étape
  ; étape 16
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    00 56 2D 90 00 12          ; dessin banque 0 frame 86 dy 45 dx 18 [décor]
    00 57 35 90 FF FD          ; dessin banque 0 frame 87 dy 53 dx -3 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0846` (4:$3318)

Rôles : cible de saut depuis LAB_0845 ; cible de saut depuis LAB_0847 ; cible de saut depuis LAB_0848

```
  ; étape 1
    88 03                      ; $88 Hold
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 46 0C 80 00 0C          ; dessin banque 0 frame 70 dy 12 dx 12
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 34 08 80 00 0C          ; dessin banque 0 frame 52 dy 8 dx 12
    00 4F 36 00 00 02          ; dessin banque 0 frame 79 dy 54 dx 2
    00 48 1D 00 00 0A          ; dessin banque 0 frame 72 dy 29 dx 10
    FF 00                      ; fin d'étape
  ; étape 3
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4C 2C 80 00 0A          ; dessin banque 0 frame 76 dy 44 dx 10
    00 4D 2C 80 00 0B          ; dessin banque 0 frame 77 dy 44 dx 11
    FF 00                      ; fin d'étape
  ; étape 4
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4E 2C 80 00 0B          ; dessin banque 0 frame 78 dy 44 dx 11
    00 4F 35 80 00 03          ; dessin banque 0 frame 79 dy 53 dx 3
    FF 00                      ; fin d'étape
  ; étape 5
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 50 2D 80 00 0C          ; dessin banque 0 frame 80 dy 45 dx 12
    00 51 36 80 FF FF          ; dessin banque 0 frame 81 dy 54 dx -1
    FF 00                      ; fin d'étape
  ; étape 6
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 52 2D 80 00 0C          ; dessin banque 0 frame 82 dy 45 dx 12
    00 53 36 80 FF FD          ; dessin banque 0 frame 83 dy 54 dx -3
    FF 00                      ; fin d'étape
  ; étape 7
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 54 2D 80 00 0B          ; dessin banque 0 frame 84 dy 45 dx 11
    00 55 36 80 FF FE          ; dessin banque 0 frame 85 dy 54 dx -2
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    00 56 2D 90 00 12          ; dessin banque 0 frame 86 dy 45 dx 18 [décor]
    00 57 35 90 FF FD          ; dessin banque 0 frame 87 dy 53 dx -3 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0847` (4:$3402)

Rôles : LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [1] ; LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [2] ; LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [3] ; LAB_0600 [objet+30 (scripts objet+30) ; posée par LAB_018B] : [5] ; entrée 0 de la table LAB_084E+672 ; référencé par le code dans LAB_0156

```
  LAB_0844:
  ; étape 1
    A4 8F                      ; $A4 Sound
    00 09 0C 01 FF F5          ; dessin banque 0 frame 9 dy 12 dx -11 [corps]
    00 08 12 01 FF E2          ; dessin banque 0 frame 8 dy 18 dx -30 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    A0 10 00 00 00 00 00 0F    ; $A0 Move
    00 14 13 03 FF C0          ; dessin banque 0 frame 20 dy 19 dx -64 [corps,frappe]
    80 FF                      ; $80 SetDir
    FF FF                      ; fin du script
  LAB_0846:
  ; étape 3
    88 03                      ; $88 Hold
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 46 0C 80 00 0C          ; dessin banque 0 frame 70 dy 12 dx 12
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 34 08 80 00 0C          ; dessin banque 0 frame 52 dy 8 dx 12
    00 4F 36 00 00 02          ; dessin banque 0 frame 79 dy 54 dx 2
    00 48 1D 00 00 0A          ; dessin banque 0 frame 72 dy 29 dx 10
    FF 00                      ; fin d'étape
  ; étape 5
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4C 2C 80 00 0A          ; dessin banque 0 frame 76 dy 44 dx 10
    00 4D 2C 80 00 0B          ; dessin banque 0 frame 77 dy 44 dx 11
    FF 00                      ; fin d'étape
  ; étape 6
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4E 2C 80 00 0B          ; dessin banque 0 frame 78 dy 44 dx 11
    00 4F 35 80 00 03          ; dessin banque 0 frame 79 dy 53 dx 3
    FF 00                      ; fin d'étape
  ; étape 7
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 50 2D 80 00 0C          ; dessin banque 0 frame 80 dy 45 dx 12
    00 51 36 80 FF FF          ; dessin banque 0 frame 81 dy 54 dx -1
    FF 00                      ; fin d'étape
  ; étape 8
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 52 2D 80 00 0C          ; dessin banque 0 frame 82 dy 45 dx 12
    00 53 36 80 FF FD          ; dessin banque 0 frame 83 dy 54 dx -3
    FF 00                      ; fin d'étape
  ; étape 9
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 54 2D 80 00 0B          ; dessin banque 0 frame 84 dy 45 dx 11
    00 55 36 80 FF FE          ; dessin banque 0 frame 85 dy 54 dx -2
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    00 56 2D 90 00 12          ; dessin banque 0 frame 86 dy 45 dx 18 [décor]
    00 57 35 90 FF FD          ; dessin banque 0 frame 87 dy 53 dx -3 [décor]
    FF 00                      ; fin d'étape
  ; étape 11
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 12
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    00 15 ED 00 FF DF          ; dessin banque 0 frame 21 dy -19 dx -33
    04 0D 1A 80 FF CC          ; dessin banque 1 frame 13 dy 26 dx -52
    FF 00                      ; fin d'étape
  ; étape 13
    A4 8F                      ; $A4 Sound
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    04 0C EE 00 FF DE          ; dessin banque 1 frame 12 dy -18 dx -34
    04 0F 1A 80 FF D7          ; dessin banque 1 frame 15 dy 26 dx -41
    04 10 21 80 00 08          ; dessin banque 1 frame 16 dy 33 dx 8
    04 11 22 80 FF AF          ; dessin banque 1 frame 17 dy 34 dx -81
    FF 00                      ; fin d'étape
  ; étape 14
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    04 0C EE 00 FF DE          ; dessin banque 1 frame 12 dy -18 dx -34
    04 12 1B 80 FF DD          ; dessin banque 1 frame 18 dy 27 dx -35
    04 13 2A 80 00 18          ; dessin banque 1 frame 19 dy 42 dx 24
    04 14 2E 80 FF AD          ; dessin banque 1 frame 20 dy 46 dx -83
    FF 00                      ; fin d'étape
  ; étape 15
    88 02                      ; $88 Hold
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    04 0C EE 00 FF DE          ; dessin banque 1 frame 12 dy -18 dx -34
    04 15 1D 80 FF E4          ; dessin banque 1 frame 21 dy 29 dx -28
    04 16 31 90 00 13          ; dessin banque 1 frame 22 dy 49 dx 19 [décor]
    00 45 30 90 FF 93          ; dessin banque 0 frame 69 dy 48 dx -109 [décor]
    FF 00                      ; fin d'étape
  ; étape 16
    B4 00 00 00 34 D2          ; $B4 IfDead -> LAB_0848
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    04 17 22 80 FF F3          ; dessin banque 1 frame 23 dy 34 dx -13
    04 18 32 90 FF E3          ; dessin banque 1 frame 24 dy 50 dx -29 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    84 00 00 00 32 4E          ; $84 Jump -> LAB_0844
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    04 17 22 80 FF F3          ; dessin banque 1 frame 23 dy 34 dx -13
    04 18 32 90 FF E3          ; dessin banque 1 frame 24 dy 50 dx -29 [décor]
    FF FF                      ; fin du script
  LAB_0848:
  ; étape 18
    B4 00 00 00 33 18          ; $B4 IfDead -> LAB_0846
    88 03                      ; $88 Hold
    00 19 18 00 FF F5          ; dessin banque 0 frame 25 dy 24 dx -11
    00 18 20 00 FF CD          ; dessin banque 0 frame 24 dy 32 dx -51
    04 19 25 80 FF ED          ; dessin banque 1 frame 25 dy 37 dx -19
    04 1A 32 80 FF E1          ; dessin banque 1 frame 26 dy 50 dx -31
    FF 00                      ; fin d'étape
  ; étape 19
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1B 32 80 FF CF          ; dessin banque 1 frame 27 dy 50 dx -49
    FF 00                      ; fin d'étape
  ; étape 20
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1C 32 80 FF CE          ; dessin banque 1 frame 28 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 21
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1D 32 80 FF CE          ; dessin banque 1 frame 29 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 22
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1E 32 80 FF CE          ; dessin banque 1 frame 30 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 23
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1F 32 80 FF CD          ; dessin banque 1 frame 31 dy 50 dx -51
    FF 00                      ; fin d'étape
  ; étape 24
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    04 20 32 90 FF CB          ; dessin banque 1 frame 32 dy 50 dx -53 [décor]
    FF 00                      ; fin d'étape
  ; étape 25
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0848` (4:$34D2)

Rôles : cible de saut depuis LAB_0847

```
  LAB_0846:
  ; étape 1
    88 03                      ; $88 Hold
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 46 0C 80 00 0C          ; dessin banque 0 frame 70 dy 12 dx 12
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    00 1B 0C 00 FF F4          ; dessin banque 0 frame 27 dy 12 dx -12
    00 1A 1D 00 FF CF          ; dessin banque 0 frame 26 dy 29 dx -49
    00 0A 08 00 FF F6          ; dessin banque 0 frame 10 dy 8 dx -10
    00 34 08 80 00 0C          ; dessin banque 0 frame 52 dy 8 dx 12
    00 4F 36 00 00 02          ; dessin banque 0 frame 79 dy 54 dx 2
    00 48 1D 00 00 0A          ; dessin banque 0 frame 72 dy 29 dx 10
    FF 00                      ; fin d'étape
  ; étape 3
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4C 2C 80 00 0A          ; dessin banque 0 frame 76 dy 44 dx 10
    00 4D 2C 80 00 0B          ; dessin banque 0 frame 77 dy 44 dx 11
    FF 00                      ; fin d'étape
  ; étape 4
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 4E 2C 80 00 0B          ; dessin banque 0 frame 78 dy 44 dx 11
    00 4F 35 80 00 03          ; dessin banque 0 frame 79 dy 53 dx 3
    FF 00                      ; fin d'étape
  ; étape 5
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 50 2D 80 00 0C          ; dessin banque 0 frame 80 dy 45 dx 12
    00 51 36 80 FF FF          ; dessin banque 0 frame 81 dy 54 dx -1
    FF 00                      ; fin d'étape
  ; étape 6
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 52 2D 80 00 0C          ; dessin banque 0 frame 82 dy 45 dx 12
    00 53 36 80 FF FD          ; dessin banque 0 frame 83 dy 54 dx -3
    FF 00                      ; fin d'étape
  ; étape 7
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    00 54 2D 80 00 0B          ; dessin banque 0 frame 84 dy 45 dx 11
    00 55 36 80 FF FE          ; dessin banque 0 frame 85 dy 54 dx -2
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    00 56 2D 90 00 12          ; dessin banque 0 frame 86 dy 45 dx 18 [décor]
    00 57 35 90 FF FD          ; dessin banque 0 frame 87 dy 53 dx -3 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 10
    B4 00 00 00 33 18          ; $B4 IfDead -> LAB_0846
    88 03                      ; $88 Hold
    00 19 18 00 FF F5          ; dessin banque 0 frame 25 dy 24 dx -11
    00 18 20 00 FF CD          ; dessin banque 0 frame 24 dy 32 dx -51
    04 19 25 80 FF ED          ; dessin banque 1 frame 25 dy 37 dx -19
    04 1A 32 80 FF E1          ; dessin banque 1 frame 26 dy 50 dx -31
    FF 00                      ; fin d'étape
  ; étape 11
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1B 32 80 FF CF          ; dessin banque 1 frame 27 dy 50 dx -49
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1C 32 80 FF CE          ; dessin banque 1 frame 28 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 13
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1D 32 80 FF CE          ; dessin banque 1 frame 29 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 14
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1E 32 80 FF CE          ; dessin banque 1 frame 30 dy 50 dx -50
    FF 00                      ; fin d'étape
  ; étape 15
    88 02                      ; $88 Hold
    00 0C 22 00 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8
    00 0B 23 00 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51
    04 1F 32 80 FF CD          ; dessin banque 1 frame 31 dy 50 dx -51
    FF 00                      ; fin d'étape
  ; étape 16
    88 02                      ; $88 Hold
    00 0C 22 10 FF F8          ; dessin banque 0 frame 12 dy 34 dx -8 [décor]
    00 0B 23 10 FF CD          ; dessin banque 0 frame 11 dy 35 dx -51 [décor]
    04 20 32 90 FF CB          ; dessin banque 1 frame 32 dy 50 dx -53 [décor]
    FF 00                      ; fin d'étape
  ; étape 17
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0849` (4:$3582)

Rôles : référencé par le code dans LAB_0207 ; référencé par le code dans LAB_0232

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
  LAB_0844:
    A4 8F                      ; $A4 Sound
    00 09 0C 01 FF F5          ; dessin banque 0 frame 9 dy 12 dx -11 [corps]
    00 08 12 01 FF E2          ; dessin banque 0 frame 8 dy 18 dx -30 [corps]
    FF 00                      ; fin d'étape
  ; étape 8
    A0 10 00 00 00 00 00 0F    ; $A0 Move
    00 14 13 03 FF C0          ; dessin banque 0 frame 20 dy 19 dx -64 [corps,frappe]
    80 FF                      ; $80 SetDir
    FF FF                      ; fin du script
  ; étape 9
    D0 00                      ; $D0 Reset
    A4 93                      ; $A4 Sound
    98 07 00 00 36 66          ; $98 SkipIfDebug -> LAB_084A
    94 05                      ; $94 Loop
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 2E E4 00 FF E1          ; dessin banque 0 frame 46 dy -28 dx -31
    00 2F EA 00 00 0D          ; dessin banque 0 frame 47 dy -22 dx 13
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 2E E4 00 FF E1          ; dessin banque 0 frame 46 dy -28 dx -31
    00 30 DA 00 00 0B          ; dessin banque 0 frame 48 dy -38 dx 11
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 11
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 2E E4 00 FF E1          ; dessin banque 0 frame 46 dy -28 dx -31
    00 31 DA 00 00 0D          ; dessin banque 0 frame 49 dy -38 dx 13
    00 34 ED 00 00 0C          ; dessin banque 0 frame 52 dy -19 dx 12
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 2E E4 00 FF E1          ; dessin banque 0 frame 46 dy -28 dx -31
    00 32 EA 00 00 34          ; dessin banque 0 frame 50 dy -22 dx 52
    00 35 EC 00 00 0C          ; dessin banque 0 frame 53 dy -20 dx 12
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    A4 93                      ; $A4 Sound
    B8 0C 00 00 36 66          ; $B8 Spawn -> LAB_084A
    88 02                      ; $88 Hold
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    00 15 ED 00 FF DF          ; dessin banque 0 frame 21 dy -19 dx -33
    00 2B ED 00 FF E9          ; dessin banque 0 frame 43 dy -19 dx -23
    FF 00                      ; fin d'étape
  ; étape 14
    84 00 00 00 32 4E          ; $84 Jump -> LAB_0844
    00 17 20 00 FF CF          ; dessin banque 0 frame 23 dy 32 dx -49
    00 16 05 00 FF CC          ; dessin banque 0 frame 22 dy 5 dx -52
    00 15 ED 00 FF DF          ; dessin banque 0 frame 21 dy -19 dx -33
    00 2B ED 00 FF E9          ; dessin banque 0 frame 43 dy -19 dx -23
    FF FF                      ; fin du script
  LAB_084A:
  ; étape 15
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    8C 40 07 00 02 10 00 00    ; $8C Physics
    AC 01 00 00 37 06          ; $AC Shadow -> LAB_084D
    00 24 D6 00 FF F4          ; dessin banque 0 frame 36 dy -42 dx -12
    00 22 D1 00 FF E4          ; dessin banque 0 frame 34 dy -47 dx -28
    00 23 D2 00 FF DF          ; dessin banque 0 frame 35 dy -46 dx -33
    00 37 EB 00 FF EF          ; dessin banque 0 frame 55 dy -21 dx -17
    FF 00                      ; fin d'étape
  ; étape 16
    8C 40 07 02 02 10 00 00    ; $8C Physics
    00 37 0F 00 FF EC          ; dessin banque 0 frame 55 dy 15 dx -20
    00 26 E6 00 FF F7          ; dessin banque 0 frame 38 dy -26 dx -9
    00 22 F5 00 FF E2          ; dessin banque 0 frame 34 dy -11 dx -30
    00 23 F6 00 FF DD          ; dessin banque 0 frame 35 dy -10 dx -35
    FF 00                      ; fin d'étape
  ; étape 17
    84 03 00 00 36 EC          ; $84 Jump -> LAB_084C
  LAB_084C:
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 18
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
```

### `LAB_084A` (4:$3666)

Rôles : cible de saut depuis LAB_0849 ; cible de saut depuis LAB_084D ; cible de saut depuis LAB_084E ; entité créée ($B8) depuis LAB_0849 ; objet+30 [objet+30 (scripts objet+30)] : [7] ; référencé par le code dans LAB_0188 ; référencé par le code dans LAB_020A

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    8C 40 07 00 02 10 00 00    ; $8C Physics
    AC 01 00 00 37 06          ; $AC Shadow -> LAB_084D
    00 24 D6 00 FF F4          ; dessin banque 0 frame 36 dy -42 dx -12
    00 22 D1 00 FF E4          ; dessin banque 0 frame 34 dy -47 dx -28
    00 23 D2 00 FF DF          ; dessin banque 0 frame 35 dy -46 dx -33
    00 37 EB 00 FF EF          ; dessin banque 0 frame 55 dy -21 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    8C 40 07 02 02 10 00 00    ; $8C Physics
    00 37 0F 00 FF EC          ; dessin banque 0 frame 55 dy 15 dx -20
    00 26 E6 00 FF F7          ; dessin banque 0 frame 38 dy -26 dx -9
    00 22 F5 00 FF E2          ; dessin banque 0 frame 34 dy -11 dx -30
    00 23 F6 00 FF DD          ; dessin banque 0 frame 35 dy -10 dx -35
    FF 00                      ; fin d'étape
  ; étape 9
    84 03 00 00 36 EC          ; $84 Jump -> LAB_084C
  LAB_084C:
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 10
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
```

### `LAB_084B` (4:$36BA)

Rôles : référencé par le code dans LAB_0209

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    00 28 EF 00 FF DB          ; dessin banque 0 frame 40 dy -17 dx -37
    00 27 DC 00 FF E6          ; dessin banque 0 frame 39 dy -36 dx -26
    00 37 0B 00 FF F9          ; dessin banque 0 frame 55 dy 11 dx -7
    FF 00                      ; fin d'étape
  ; étape 8
    00 38 E7 00 FF E9          ; dessin banque 0 frame 56 dy -25 dx -23
    00 2A 00 00 FF F4          ; dessin banque 0 frame 42 dy 0 dx -12
    00 25 F5 00 FF E6          ; dessin banque 0 frame 37 dy -11 dx -26
    00 29 E0 00 FF DE          ; dessin banque 0 frame 41 dy -32 dx -34
    FF 00                      ; fin d'étape
  LAB_084C:
  ; étape 9
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 10
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
```

### `LAB_084C` (4:$36EC)

Rôles : cible de saut depuis LAB_0849 ; cible de saut depuis LAB_084A ; cible de saut depuis LAB_084D ; cible de saut depuis LAB_084E

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 8
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
```

### `LAB_084D` (4:$3706)

Rôles : entrée 0 de la table LAB_084E+554 ; ombre ($AC) depuis LAB_0849 ; ombre ($AC) depuis LAB_084A ; ombre ($AC) depuis LAB_084D ; ombre ($AC) depuis LAB_084E

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
  LAB_084A:
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    8C 40 07 00 02 10 00 00    ; $8C Physics
    AC 01 00 00 37 06          ; $AC Shadow -> LAB_084D
    00 24 D6 00 FF F4          ; dessin banque 0 frame 36 dy -42 dx -12
    00 22 D1 00 FF E4          ; dessin banque 0 frame 34 dy -47 dx -28
    00 23 D2 00 FF DF          ; dessin banque 0 frame 35 dy -46 dx -33
    00 37 EB 00 FF EF          ; dessin banque 0 frame 55 dy -21 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    8C 40 07 02 02 10 00 00    ; $8C Physics
    00 37 0F 00 FF EC          ; dessin banque 0 frame 55 dy 15 dx -20
    00 26 E6 00 FF F7          ; dessin banque 0 frame 38 dy -26 dx -9
    00 22 F5 00 FF E2          ; dessin banque 0 frame 34 dy -11 dx -30
    00 23 F6 00 FF DD          ; dessin banque 0 frame 35 dy -10 dx -35
    FF 00                      ; fin d'étape
  ; étape 9
    84 03 00 00 36 EC          ; $84 Jump -> LAB_084C
  LAB_084C:
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 10
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
    00 36 2E 00 FF E0          ; dessin banque 0 frame 54 dy 46 dx -32
    FF 00                      ; fin d'étape
  LAB_084E:
  ; étape 11
    A4 93                      ; $A4 Sound
    98 07 00 00 36 66          ; $98 SkipIfDebug -> LAB_084A
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 3E E1 00 00 14          ; dessin banque 0 frame 62 dy -31 dx 20
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    00 3F D4 00 00 22          ; dessin banque 0 frame 63 dy -44 dx 34
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 40 DB 00 00 34          ; dessin banque 0 frame 64 dy -37 dx 52
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 13
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 41 FE 00 00 5B          ; dessin banque 0 frame 65 dy -2 dx 91
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 14
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 42 1B 00 00 5D          ; dessin banque 0 frame 66 dy 27 dx 93
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 15
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 43 2A 00 00 49          ; dessin banque 0 frame 67 dy 42 dx 73
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 16
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 44 2C 00 00 50          ; dessin banque 0 frame 68 dy 44 dx 80
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 17
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 45 30 90 00 49          ; dessin banque 0 frame 69 dy 48 dx 73 [décor]
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 18
    94 07                      ; $94 Loop
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 46 EB 00 00 03          ; dessin banque 0 frame 70 dy -21 dx 3
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 19
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 47 F1 00 00 00          ; dessin banque 0 frame 71 dy -15 dx 0
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 20
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 48 19 00 00 02          ; dessin banque 0 frame 72 dy 25 dx 2
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 21
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 49 2A 00 FF FE          ; dessin banque 0 frame 73 dy 42 dx -2
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 22
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 4A 2F 00 FF F7          ; dessin banque 0 frame 74 dy 47 dx -9
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 23
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 4B 33 00 FF FA          ; dessin banque 0 frame 75 dy 51 dx -6
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF FF                      ; fin du script
```

### `LAB_084E` (4:$370E)

Rôles : référencé par le code dans LAB_0206 ; référencé par le code dans LAB_0231

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  LAB_07FA:
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 01                      ; $C0 SetBank
    88 03                      ; $88 Hold
    08 14 1B 01 FF DD          ; dessin banque 2 frame 20 dy 27 dx -35 [corps]
    0C 03 34 00 FF FB          ; dessin banque 3 frame 3 dy 52 dx -5
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
  LAB_084A:
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    8C 40 07 00 02 10 00 00    ; $8C Physics
    AC 01 00 00 37 06          ; $AC Shadow -> LAB_084D
    00 24 D6 00 FF F4          ; dessin banque 0 frame 36 dy -42 dx -12
    00 22 D1 00 FF E4          ; dessin banque 0 frame 34 dy -47 dx -28
    00 23 D2 00 FF DF          ; dessin banque 0 frame 35 dy -46 dx -33
    00 37 EB 00 FF EF          ; dessin banque 0 frame 55 dy -21 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    8C 40 07 02 02 10 00 00    ; $8C Physics
    00 37 0F 00 FF EC          ; dessin banque 0 frame 55 dy 15 dx -20
    00 26 E6 00 FF F7          ; dessin banque 0 frame 38 dy -26 dx -9
    00 22 F5 00 FF E2          ; dessin banque 0 frame 34 dy -11 dx -30
    00 23 F6 00 FF DD          ; dessin banque 0 frame 35 dy -10 dx -35
    FF 00                      ; fin d'étape
  ; étape 9
    84 03 00 00 36 EC          ; $84 Jump -> LAB_084C
  LAB_084C:
    AC 00 00 00 00 00          ; $AC Shadow
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 3A 23 00 FF D5          ; dessin banque 0 frame 58 dy 35 dx -43
    FF 00                      ; fin d'étape
  ; étape 10
    C0 01                      ; $C0 SetBank
    84 03 00 00 19 92          ; $84 Jump -> LAB_07FA
    A4 93                      ; $A4 Sound
    98 07 00 00 36 66          ; $98 SkipIfDebug -> LAB_084A
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 3E E1 00 00 14          ; dessin banque 0 frame 62 dy -31 dx 20
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    00 3F D4 00 00 22          ; dessin banque 0 frame 63 dy -44 dx 34
    FF 00                      ; fin d'étape
  ; étape 11
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 40 DB 00 00 34          ; dessin banque 0 frame 64 dy -37 dx 52
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 41 FE 00 00 5B          ; dessin banque 0 frame 65 dy -2 dx 91
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 13
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 42 1B 00 00 5D          ; dessin banque 0 frame 66 dy 27 dx 93
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 14
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 43 2A 00 00 49          ; dessin banque 0 frame 67 dy 42 dx 73
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 15
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 44 2C 00 00 50          ; dessin banque 0 frame 68 dy 44 dx 80
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 16
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3C DE 00 FF FF          ; dessin banque 0 frame 60 dy -34 dx -1
    00 45 30 90 00 49          ; dessin banque 0 frame 69 dy 48 dx 73 [décor]
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 17
    94 07                      ; $94 Loop
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 46 EB 00 00 03          ; dessin banque 0 frame 70 dy -21 dx 3
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 18
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 47 F1 00 00 00          ; dessin banque 0 frame 71 dy -15 dx 0
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 19
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 48 19 00 00 02          ; dessin banque 0 frame 72 dy 25 dx 2
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 20
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 49 2A 00 FF FE          ; dessin banque 0 frame 73 dy 42 dx -2
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 21
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 4A 2F 00 FF F7          ; dessin banque 0 frame 74 dy 47 dx -9
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF 00                      ; fin d'étape
  ; étape 22
    88 02                      ; $88 Hold
    00 36 2E 00 FF E4          ; dessin banque 0 frame 54 dy 46 dx -28
    00 3B EF 00 FF DF          ; dessin banque 0 frame 59 dy -17 dx -33
    00 3D E3 00 FF F8          ; dessin banque 0 frame 61 dy -29 dx -8
    00 4B 33 00 FF FA          ; dessin banque 0 frame 75 dy 51 dx -6
    00 0E 0F 00 FF CA          ; dessin banque 0 frame 14 dy 15 dx -54
    00 07 17 00 FF DC          ; dessin banque 0 frame 7 dy 23 dx -36
    FF FF                      ; fin du script
```

### `LAB_084F` (4:$3A8C)

Rôles : objet+22 (repos) dans LAB_018F ; objet+26 (réaction) dans LAB_018F

```
  ; étape 1
    AC 00 00 00 3A EE          ; $AC Shadow -> LAB_0856
    00 0A 16 01 FF EE          ; dessin banque 0 frame 10 dy 22 dx -18 [corps]
    00 09 08 00 FF EA          ; dessin banque 0 frame 9 dy 8 dx -22
    FF FF                      ; fin du script
```

### `LAB_0850` (4:$3AA0)

Rôles : référencé par le code dans LAB_0259

```
  ; étape 1
    00 00 25 01 FF CD          ; dessin banque 0 frame 0 dy 37 dx -51 [corps]
    FF 00                      ; fin d'étape
  LAB_0851:
  ; étape 2
    B0 00 00 00 72 4C          ; $B0 Call -> LAB_02E2
    00 01 0A 00 FF D8          ; dessin banque 0 frame 1 dy 10 dx -40
    00 02 F7 01 FF F3          ; dessin banque 0 frame 2 dy -9 dx -13 [corps]
    00 03 EF 01 00 10          ; dessin banque 0 frame 3 dy -17 dx 16 [corps]
    00 08 2E 00 FF F5          ; dessin banque 0 frame 8 dy 46 dx -11
    FF FF                      ; fin du script
```

### `LAB_0851` (4:$3AA8)

Rôles : référencé par le code dans LAB_0268

```
  ; étape 1
    B0 00 00 00 72 4C          ; $B0 Call -> LAB_02E2
    00 01 0A 00 FF D8          ; dessin banque 0 frame 1 dy 10 dx -40
    00 02 F7 01 FF F3          ; dessin banque 0 frame 2 dy -9 dx -13 [corps]
    00 03 EF 01 00 10          ; dessin banque 0 frame 3 dy -17 dx 16 [corps]
    00 08 2E 00 FF F5          ; dessin banque 0 frame 8 dy 46 dx -11
    FF FF                      ; fin du script
```

### `LAB_0852` (4:$3AC8)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 0 ; référencé par le code dans LAB_0158

```
  ; étape 1
    AC 01 00 00 3A EE          ; $AC Shadow -> LAB_0856
    00 04 F3 03 FF F4          ; dessin banque 0 frame 4 dy -13 dx -12 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0853` (4:$3AD6)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 1 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 05 F3 03 FF F4          ; dessin banque 0 frame 5 dy -13 dx -12 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0854` (4:$3ADE)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 2 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 06 F5 03 FF F4          ; dessin banque 0 frame 6 dy -11 dx -12 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0855` (4:$3AE6)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche haut phase 3 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 07 F0 03 FF F4          ; dessin banque 0 frame 7 dy -16 dx -12 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0856` (4:$3AEE)

Rôles : ombre ($AC) depuis LAB_084F ; ombre ($AC) depuis LAB_0852

```
  ; étape 1
    00 08 2E 00 FF F5          ; dessin banque 0 frame 8 dy 46 dx -11
    FF 00                      ; fin d'étape
  LAB_0857:
  ; étape 2
    00 0A 16 01 FF EE          ; dessin banque 0 frame 10 dy 22 dx -18 [corps]
    00 09 08 00 FF EA          ; dessin banque 0 frame 9 dy 8 dx -22
    FF 00                      ; fin d'étape
  ; étape 3
    A4 5D                      ; $A4 Sound
    00 0B 1C 01 FF EC          ; dessin banque 0 frame 11 dy 28 dx -20 [corps]
    00 3D 1D 03 00 0D          ; dessin banque 0 frame 61 dy 29 dx 13 [corps,frappe]
    00 3C 11 00 FF EB          ; dessin banque 0 frame 60 dy 17 dx -21
    FF 00                      ; fin d'étape
  ; étape 4
    00 0D 23 03 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12 [corps,frappe]
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 3E 23 02 00 17          ; dessin banque 0 frame 62 dy 35 dx 23 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    00 0B 1C 01 FF EC          ; dessin banque 0 frame 11 dy 28 dx -20 [corps]
    00 3D 1D 03 00 0D          ; dessin banque 0 frame 61 dy 29 dx 13 [corps,frappe]
    00 3C 11 00 FF EB          ; dessin banque 0 frame 60 dy 17 dx -21
    FF FF                      ; fin du script
```

### `LAB_0857` (4:$3AF6)

Rôles : référencé par le code dans LAB_0254

```
  ; étape 1
    00 0A 16 01 FF EE          ; dessin banque 0 frame 10 dy 22 dx -18 [corps]
    00 09 08 00 FF EA          ; dessin banque 0 frame 9 dy 8 dx -22
    FF 00                      ; fin d'étape
  ; étape 2
    A4 5D                      ; $A4 Sound
    00 0B 1C 01 FF EC          ; dessin banque 0 frame 11 dy 28 dx -20 [corps]
    00 3D 1D 03 00 0D          ; dessin banque 0 frame 61 dy 29 dx 13 [corps,frappe]
    00 3C 11 00 FF EB          ; dessin banque 0 frame 60 dy 17 dx -21
    FF 00                      ; fin d'étape
  ; étape 3
    00 0D 23 03 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12 [corps,frappe]
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 3E 23 02 00 17          ; dessin banque 0 frame 62 dy 35 dx 23 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    00 0B 1C 01 FF EC          ; dessin banque 0 frame 11 dy 28 dx -20 [corps]
    00 3D 1D 03 00 0D          ; dessin banque 0 frame 61 dy 29 dx 13 [corps,frappe]
    00 3C 11 00 FF EB          ; dessin banque 0 frame 60 dy 17 dx -21
    FF FF                      ; fin du script
```

### `LAB_0858` (4:$3B42)

Rôles : référencé par le code dans LAB_0276

```
  ; étape 1
    A4 5F                      ; $A4 Sound
    00 0D 23 00 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 0E 0F 80 00 1B          ; dessin banque 0 frame 14 dy 15 dx 27
    FF 00                      ; fin d'étape
  ; étape 2
    00 0D 23 00 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 0F 17 80 00 1C          ; dessin banque 0 frame 15 dy 23 dx 28
    FF 00                      ; fin d'étape
  ; étape 3
    00 0D 23 00 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 10 28 80 00 1A          ; dessin banque 0 frame 16 dy 40 dx 26
    00 11 26 80 00 35          ; dessin banque 0 frame 17 dy 38 dx 53
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    00 0D 23 00 FF F4          ; dessin banque 0 frame 13 dy 35 dx -12
    00 0C 13 00 FF C8          ; dessin banque 0 frame 12 dy 19 dx -56
    00 12 28 80 00 18          ; dessin banque 0 frame 18 dy 40 dx 24
    00 13 32 90 00 46          ; dessin banque 0 frame 19 dy 50 dx 70 [décor]
    FF 00                      ; fin d'étape
  ; étape 5
    00 0B 1C 01 FF EC          ; dessin banque 0 frame 11 dy 28 dx -20 [corps]
    00 15 25 80 00 13          ; dessin banque 0 frame 21 dy 37 dx 19
    00 3D 1D 01 00 0D          ; dessin banque 0 frame 61 dy 29 dx 13 [corps]
    00 3C 11 00 FF EB          ; dessin banque 0 frame 60 dy 17 dx -21
    FF 00                      ; fin d'étape
  ; étape 6
    00 0A 16 01 FF EE          ; dessin banque 0 frame 10 dy 22 dx -18 [corps]
    00 09 08 00 FF EA          ; dessin banque 0 frame 9 dy 8 dx -22
    00 15 22 80 00 0E          ; dessin banque 0 frame 21 dy 34 dx 14
    FF 00                      ; fin d'étape
  ; étape 7
    00 0A 16 01 FF EE          ; dessin banque 0 frame 10 dy 22 dx -18 [corps]
    00 09 08 00 FF EA          ; dessin banque 0 frame 9 dy 8 dx -22
    00 16 22 80 00 0F          ; dessin banque 0 frame 22 dy 34 dx 15
    FF FF                      ; fin du script
```

### `LAB_0859` (4:$3BE4)

Rôles : référencé par le code dans LAB_0253

```
  ; étape 1
    88 02                      ; $88 Hold
    00 27 17 01 FF ED          ; dessin banque 0 frame 39 dy 23 dx -19 [corps]
    00 26 24 00 FF DB          ; dessin banque 0 frame 38 dy 36 dx -37
    00 3A 0F 00 FF ED          ; dessin banque 0 frame 58 dy 15 dx -19
    FF 00                      ; fin d'étape
  ; étape 2
    A4 6D                      ; $A4 Sound
    00 29 1B 01 FF F1          ; dessin banque 0 frame 41 dy 27 dx -15 [corps]
    00 28 1A 00 FF CE          ; dessin banque 0 frame 40 dy 26 dx -50
    00 38 0F 02 00 01          ; dessin banque 0 frame 56 dy 15 dx 1 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    00 2B 15 01 FF F2          ; dessin banque 0 frame 43 dy 21 dx -14 [corps]
    00 2A 17 00 FF DB          ; dessin banque 0 frame 42 dy 23 dx -37
    00 39 15 02 00 0E          ; dessin banque 0 frame 57 dy 21 dx 14 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    88 03                      ; $88 Hold
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    FF FF                      ; fin du script
```

### `LAB_085A` (4:$3C34)

Rôles : référencé par le code dans LAB_0274

```
  ; étape 1
    A4 6C                      ; $A4 Sound
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 2E 23 80 FF F3          ; dessin banque 0 frame 46 dy 35 dx -13
    FF 00                      ; fin d'étape
  ; étape 2
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 2F 1C 80 FF DF          ; dessin banque 0 frame 47 dy 28 dx -33
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    FF 00                      ; fin d'étape
  ; étape 3
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 30 1A 80 FF CC          ; dessin banque 0 frame 48 dy 26 dx -52
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    FF 00                      ; fin d'étape
  ; étape 4
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 31 1E 80 FF BE          ; dessin banque 0 frame 49 dy 30 dx -66
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    FF 00                      ; fin d'étape
  ; étape 5
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 32 28 80 FF B9          ; dessin banque 0 frame 50 dy 40 dx -71
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    00 33 30 90 FF B8          ; dessin banque 0 frame 51 dy 48 dx -72 [décor]
    FF 00                      ; fin d'étape
  ; étape 6
    00 33 30 90 FF B8          ; dessin banque 0 frame 51 dy 48 dx -72 [décor]
    00 2D 1A 01 FF F1          ; dessin banque 0 frame 45 dy 26 dx -15 [corps]
    00 3C 0F 00 00 0D          ; dessin banque 0 frame 60 dy 15 dx 13
    FF FF                      ; fin du script
```

### `LAB_085B` (4:$3CB4)

Rôles : référencé par le code dans LAB_0265 ; référencé par le code dans LAB_0278

```
  ; étape 1
    04 2F F5 00 FF EF          ; dessin banque 1 frame 47 dy -11 dx -17
    04 30 F2 00 00 0A          ; dessin banque 1 frame 48 dy -14 dx 10
    00 17 E1 00 FF DA          ; dessin banque 0 frame 23 dy -31 dx -38
    FF FF                      ; fin du script
```

### `LAB_085C` (4:$3CC8)

Rôles : référencé par le code dans LAB_0267

```
  ; étape 1
    A4 5D                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 2F F5 00 FF EF          ; dessin banque 1 frame 47 dy -11 dx -17
    04 30 F2 00 00 0A          ; dessin banque 1 frame 48 dy -14 dx 10
    00 18 D6 00 FF D1          ; dessin banque 0 frame 24 dy -42 dx -47
    00 18 D6 00 FF D1          ; dessin banque 0 frame 24 dy -42 dx -47
    FF 00                      ; fin d'étape
  ; étape 2
    A4 6D                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 2F F5 00 FF EF          ; dessin banque 1 frame 47 dy -11 dx -17
    04 30 F2 00 00 0A          ; dessin banque 1 frame 48 dy -14 dx 10
    00 19 D4 00 FF D4          ; dessin banque 0 frame 25 dy -44 dx -44
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 2F F5 00 FF EF          ; dessin banque 1 frame 47 dy -11 dx -17
    04 30 F2 00 00 0A          ; dessin banque 1 frame 48 dy -14 dx 10
    00 1A D4 00 FF D4          ; dessin banque 0 frame 26 dy -44 dx -44
    FF 00                      ; fin d'étape
  ; étape 4
    A4 60                      ; $A4 Sound
    A4 5E                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 2B 2C 00 FF E8          ; dessin banque 1 frame 43 dy 44 dx -24
    04 2C F4 00 FF E8          ; dessin banque 1 frame 44 dy -12 dx -24
    00 1B DE 00 FF CD          ; dessin banque 0 frame 27 dy -34 dx -51
    00 1D E0 00 FF FA          ; dessin banque 0 frame 29 dy -32 dx -6
    00 1E F4 80 FF F8          ; dessin banque 0 frame 30 dy -12 dx -8
    04 32 ED 00 FF D5          ; dessin banque 1 frame 50 dy -19 dx -43
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    04 2B 2C 00 FF E8          ; dessin banque 1 frame 43 dy 44 dx -24
    04 2A F5 00 FF E4          ; dessin banque 1 frame 42 dy -11 dx -28
    00 1C E1 00 FF C6          ; dessin banque 0 frame 28 dy -31 dx -58
    00 1F F7 80 FF EF          ; dessin banque 0 frame 31 dy -9 dx -17
    00 20 00 80 00 18          ; dessin banque 0 frame 32 dy 0 dx 24
    04 2D EC 00 FF E9          ; dessin banque 1 frame 45 dy -20 dx -23
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 2B 2C 00 FF E8          ; dessin banque 1 frame 43 dy 44 dx -24
    04 2A F5 00 FF E4          ; dessin banque 1 frame 42 dy -11 dx -28
    00 22 1C 80 00 2E          ; dessin banque 0 frame 34 dy 28 dx 46
    00 21 03 80 00 0F          ; dessin banque 0 frame 33 dy 3 dx 15
    00 1B DE 00 FF C4          ; dessin banque 0 frame 27 dy -34 dx -60
    04 2D EC 00 FF E9          ; dessin banque 1 frame 45 dy -20 dx -23
    00 21 FB 80 FF FC          ; dessin banque 0 frame 33 dy -5 dx -4
    00 23 13 80 00 24          ; dessin banque 0 frame 35 dy 19 dx 36
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    04 2B 2C 00 FF E8          ; dessin banque 1 frame 43 dy 44 dx -24
    04 2A F5 00 FF E4          ; dessin banque 1 frame 42 dy -11 dx -28
    00 1C E1 00 FF C6          ; dessin banque 0 frame 28 dy -31 dx -58
    00 24 1D 80 00 31          ; dessin banque 0 frame 36 dy 29 dx 49
    00 25 30 90 00 31          ; dessin banque 0 frame 37 dy 48 dx 49 [décor]
    04 2D EC 00 FF E9          ; dessin banque 1 frame 45 dy -20 dx -23
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    04 2B 2C 00 FF E8          ; dessin banque 1 frame 43 dy 44 dx -24
    04 2A F5 00 FF E4          ; dessin banque 1 frame 42 dy -11 dx -28
    00 1B DE 00 FF C4          ; dessin banque 0 frame 27 dy -34 dx -60
    00 25 30 90 00 31          ; dessin banque 0 frame 37 dy 48 dx 49 [décor]
    04 2D EC 00 FF E9          ; dessin banque 1 frame 45 dy -20 dx -23
    FF FF                      ; fin du script
```

### `LAB_085D` (4:$3DE6)

Rôles : référencé par le code dans LAB_0266

```
  ; étape 1
    04 2F F5 00 FF EF          ; dessin banque 1 frame 47 dy -11 dx -17
    04 30 F2 00 00 0A          ; dessin banque 1 frame 48 dy -14 dx 10
    00 17 E1 00 FF DA          ; dessin banque 0 frame 23 dy -31 dx -38
    FF 00                      ; fin d'étape
  ; étape 2
    A4 05                      ; $A4 Sound
    A4 0D                      ; $A4 Sound
    A4 5C                      ; $A4 Sound
    04 2B 2D 00 FF EA          ; dessin banque 1 frame 43 dy 45 dx -22
    04 2C F5 00 FF EA          ; dessin banque 1 frame 44 dy -11 dx -22
    04 32 EB 00 FF D3          ; dessin banque 1 frame 50 dy -21 dx -45
    04 31 DE 00 FF D5          ; dessin banque 1 frame 49 dy -34 dx -43
    04 29 E6 00 FF C7          ; dessin banque 1 frame 41 dy -26 dx -57
    04 1C D3 80 FF B0          ; dessin banque 1 frame 28 dy -45 dx -80
    FF 00                      ; fin d'étape
  ; étape 3
    04 2B 2D 00 FF EA          ; dessin banque 1 frame 43 dy 45 dx -22
    04 2C F5 00 FF EA          ; dessin banque 1 frame 44 dy -11 dx -22
    04 32 EB 00 FF D3          ; dessin banque 1 frame 50 dy -21 dx -45
    04 15 ED 00 FF CC          ; dessin banque 1 frame 21 dy -19 dx -52
    04 13 06 00 FF AF          ; dessin banque 1 frame 19 dy 6 dx -81
    04 0B 18 80 FF AA          ; dessin banque 1 frame 11 dy 24 dx -86
    04 06 CC 80 FF 8A          ; dessin banque 1 frame 6 dy -52 dx -118
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    A4 10                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 16 24 10 FF A5          ; dessin banque 1 frame 22 dy 36 dx -91 [décor]
    04 2F F6 00 FF F0          ; dessin banque 1 frame 47 dy -10 dx -16
    04 30 F4 00 00 09          ; dessin banque 1 frame 48 dy -12 dx 9
    04 0F 2A 90 FF A4          ; dessin banque 1 frame 15 dy 42 dx -92 [décor]
    04 0D 34 90 FF AB          ; dessin banque 1 frame 13 dy 52 dx -85 [décor]
    00 23 C7 80 FF 7E          ; dessin banque 0 frame 35 dy -57 dx -130
    FF FF                      ; fin du script
```

### `LAB_085E` (4:$3E82)

Rôles : référencé par le code dans LAB_01F0

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    A4 6C                      ; $A4 Sound
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 33 1C 80 FF E5          ; dessin banque 1 frame 51 dy 28 dx -27
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    FF 00                      ; fin d'étape
  ; étape 7
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 34 21 80 FF F3          ; dessin banque 1 frame 52 dy 33 dx -13
    04 35 2F 80 FF E1          ; dessin banque 1 frame 53 dy 47 dx -31
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    FF 00                      ; fin d'étape
  ; étape 8
    88 01                      ; $88 Hold
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 36 21 80 FF F8          ; dessin banque 1 frame 54 dy 33 dx -8
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    04 38 33 90 FF F1          ; dessin banque 1 frame 56 dy 51 dx -15 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    88 01                      ; $88 Hold
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 37 21 80 FF FA          ; dessin banque 1 frame 55 dy 33 dx -6
    04 38 33 90 FF F1          ; dessin banque 1 frame 56 dy 51 dx -15 [décor]
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    FF 00                      ; fin d'étape
  ; étape 10
    C0 01                      ; $C0 SetBank
    B4 03 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
```

### `LAB_085F` (4:$3F04)

Rôles : référencé par le code dans LAB_01EF

```
  LAB_07DB:
  ; étape 1
    00 01 2D 01 FF F3          ; dessin banque 0 frame 1 dy 45 dx -13 [corps]
    00 00 F7 01 FF F7          ; dessin banque 0 frame 0 dy -9 dx -9 [corps]
    0C 01 FA 01 00 04          ; dessin banque 3 frame 1 dy -6 dx 4 [corps]
    FF FF                      ; fin du script
  LAB_07F7:
  ; étape 2
    80 FF                      ; $80 SetDir
    D0 00                      ; $D0 Reset
    88 14                      ; $88 Hold
    08 15 07 01 FF E4          ; dessin banque 2 frame 21 dy 7 dx -28 [corps]
    08 16 15 01 FF E4          ; dessin banque 2 frame 22 dy 21 dx -28 [corps]
    08 17 28 00 FF C5          ; dessin banque 2 frame 23 dy 40 dx -59
    FF 00                      ; fin d'étape
  LAB_07F8:
  ; étape 3
    A4 0A                      ; $A4 Sound
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    08 3A 13 00 FF CD          ; dessin banque 2 frame 58 dy 19 dx -51
    FF 00                      ; fin d'étape
  ; étape 4
    08 19 2A 00 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52
    08 18 2C 00 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80
    08 1A 33 00 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62
    08 1B 38 00 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98
    FF 00                      ; fin d'étape
  ; étape 5
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    08 19 2A 10 FF CC          ; dessin banque 2 frame 25 dy 42 dx -52 [décor]
    08 18 2C 10 FF B0          ; dessin banque 2 frame 24 dy 44 dx -80 [décor]
    08 1A 33 10 FF C2          ; dessin banque 2 frame 26 dy 51 dx -62 [décor]
    08 1B 38 10 FF 9E          ; dessin banque 2 frame 27 dy 56 dx -98 [décor]
    FF FF                      ; fin du script
  ; étape 6
    D0 00                      ; $D0 Reset
    C0 02                      ; $C0 SetBank
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    04 3A 00 00 FF DC          ; dessin banque 1 frame 58 dy 0 dx -36
    04 2D F6 00 FF E5          ; dessin banque 1 frame 45 dy -10 dx -27
    FF 00                      ; fin d'étape
  ; étape 8
    C0 01                      ; $C0 SetBank
    B4 03 00 00 17 20          ; $B4 IfDead -> LAB_07F7
    84 03 00 00 10 64          ; $84 Jump -> LAB_07DB
```

### `LAB_0860` (4:$3F3C)

Rôles : LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [4] ; LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [7] ; LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [8] ; référencé par le code dans LAB_0158 ; référencé par le code dans LAB_026D

```
  ; étape 1
    B0 00 00 00 72 64          ; $B0 Call -> LAB_02E3
    04 01 1C 00 FF CD          ; dessin banque 1 frame 1 dy 28 dx -51
    04 00 04 00 FF D2          ; dessin banque 1 frame 0 dy 4 dx -46
    04 02 11 80 FF EF          ; dessin banque 1 frame 2 dy 17 dx -17
    04 03 29 80 00 08          ; dessin banque 1 frame 3 dy 41 dx 8
    FF 00                      ; fin d'étape
  ; étape 2
    04 01 1C 00 FF CD          ; dessin banque 1 frame 1 dy 28 dx -51
    04 00 04 00 FF D2          ; dessin banque 1 frame 0 dy 4 dx -46
    04 02 11 80 FF EF          ; dessin banque 1 frame 2 dy 17 dx -17
    04 03 29 80 00 08          ; dessin banque 1 frame 3 dy 41 dx 8
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 01 1C 00 FF CD          ; dessin banque 1 frame 1 dy 28 dx -51
    04 00 04 00 FF D2          ; dessin banque 1 frame 0 dy 4 dx -46
    04 04 08 80 FF D7          ; dessin banque 1 frame 4 dy 8 dx -41
    FF 00                      ; fin d'étape
  ; étape 4
    88 01                      ; $88 Hold
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 05 10 80 FF CB          ; dessin banque 1 frame 5 dy 16 dx -53
    04 06 1C 80 FF E3          ; dessin banque 1 frame 6 dy 28 dx -29
    04 07 29 80 FF DB          ; dessin banque 1 frame 7 dy 41 dx -37
    04 09 19 80 FF BC          ; dessin banque 1 frame 9 dy 25 dx -68
    FF 00                      ; fin d'étape
  ; étape 5
    B4 00 00 00 3F C8          ; $B4 IfDead -> LAB_0861
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 0B 1B 80 FF B6          ; dessin banque 1 frame 11 dy 27 dx -74
    04 0C 26 80 FF FD          ; dessin banque 1 frame 12 dy 38 dx -3
    FF FF                      ; fin du script
  LAB_0861:
  ; étape 6
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 0F 29 80 FF FA          ; dessin banque 1 frame 15 dy 41 dx -6
    04 10 24 80 FF F4          ; dessin banque 1 frame 16 dy 36 dx -12
    FF 00                      ; fin d'étape
  ; étape 7
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 11 27 80 FF F3          ; dessin banque 1 frame 17 dy 39 dx -13
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    04 0E 27 10 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63 [décor]
    04 12 2B 90 FF F2          ; dessin banque 1 frame 18 dy 43 dx -14 [décor]
    FF 00                      ; fin d'étape
  ; étape 9
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0861` (4:$3FC8)

Rôles : cible de saut depuis LAB_0860 ; cible de saut depuis LAB_0862

```
  ; étape 1
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 0F 29 80 FF FA          ; dessin banque 1 frame 15 dy 41 dx -6
    04 10 24 80 FF F4          ; dessin banque 1 frame 16 dy 36 dx -12
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 11 27 80 FF F3          ; dessin banque 1 frame 17 dy 39 dx -13
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 0E 27 10 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63 [décor]
    04 12 2B 90 FF F2          ; dessin banque 1 frame 18 dy 43 dx -14 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0862` (4:$4004)

Rôles : LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [1] ; LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [3] ; LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [5] ; LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [6] ; référencé par le code dans LAB_0158 ; référencé par le code dans LAB_026F

```
  LAB_0861:
  ; étape 1
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 0F 29 80 FF FA          ; dessin banque 1 frame 15 dy 41 dx -6
    04 10 24 80 FF F4          ; dessin banque 1 frame 16 dy 36 dx -12
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    04 0E 27 00 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63
    04 11 27 80 FF F3          ; dessin banque 1 frame 17 dy 39 dx -13
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 0E 27 10 FF C1          ; dessin banque 1 frame 14 dy 39 dx -63 [décor]
    04 12 2B 90 FF F2          ; dessin banque 1 frame 18 dy 43 dx -14 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 5
    B0 00 00 00 72 7C          ; $B0 Call -> LAB_02E4
    04 19 07 00 FF DD          ; dessin banque 1 frame 25 dy 7 dx -35
    04 17 06 00 FF F0          ; dessin banque 1 frame 23 dy 6 dx -16
    04 1C 00 00 FF D9          ; dessin banque 1 frame 28 dy 0 dx -39
    FF 00                      ; fin d'étape
  ; étape 6
    04 1B 15 00 FF D1          ; dessin banque 1 frame 27 dy 21 dx -47
    04 17 06 00 FF F0          ; dessin banque 1 frame 23 dy 6 dx -16
    04 1D FF 00 FF CD          ; dessin banque 1 frame 29 dy -1 dx -51
    FF 00                      ; fin d'étape
  ; étape 7
    04 1A 0D 00 FF D2          ; dessin banque 1 frame 26 dy 13 dx -46
    04 17 06 00 FF F0          ; dessin banque 1 frame 23 dy 6 dx -16
    04 21 01 00 00 18          ; dessin banque 1 frame 33 dy 1 dx 24
    04 1F 0B 00 FF E9          ; dessin banque 1 frame 31 dy 11 dx -23
    04 1E 08 00 FF C7          ; dessin banque 1 frame 30 dy 8 dx -57
    04 21 03 00 FF C6          ; dessin banque 1 frame 33 dy 3 dx -58
    FF 00                      ; fin d'étape
  ; étape 8
    B4 00 00 00 3F C8          ; $B4 IfDead -> LAB_0861
    04 1A 0D 00 FF D2          ; dessin banque 1 frame 26 dy 13 dx -46
    04 17 06 00 FF F0          ; dessin banque 1 frame 23 dy 6 dx -16
    04 20 04 00 FF CD          ; dessin banque 1 frame 32 dy 4 dx -51
    04 21 FF 00 00 25          ; dessin banque 1 frame 33 dy -1 dx 37
    FF FF                      ; fin du script
```

### `LAB_0863` (4:$4078)

Rôles : référencé par le code dans LAB_025E

```
  ; étape 1
    00 37 0F 42 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1 [frappe,hors-boîte]
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 34 F8 00 00 11          ; dessin banque 0 frame 52 dy -8 dx 17
    FF FF                      ; fin du script
```

### `LAB_0864` (4:$408C)

Rôles : référencé par le code dans LAB_025F

```
  ; étape 1
    00 37 0F 42 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1 [frappe,hors-boîte]
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 35 F8 00 00 11          ; dessin banque 0 frame 53 dy -8 dx 17
    FF FF                      ; fin du script
```

### `LAB_0865` (4:$40A0)

Rôles : référencé par le code dans LAB_0279

```
  ; étape 1
    A4 6A                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 37 08 00 FF FF          ; dessin banque 0 frame 55 dy 8 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 38 00 FF F5          ; dessin banque 1 frame 59 dy 56 dx -11
    04 2D 35 00 00 0A          ; dessin banque 1 frame 45 dy 53 dx 10
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 30 00 FF F5          ; dessin banque 1 frame 59 dy 48 dx -11
    04 2D 2D 00 00 0A          ; dessin banque 1 frame 45 dy 45 dx 10
    FF 00                      ; fin d'étape
  ; étape 3
    A4 6B                      ; $A4 Sound
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 27 00 FF F5          ; dessin banque 1 frame 59 dy 39 dx -11
    04 2D 24 00 00 0A          ; dessin banque 1 frame 45 dy 36 dx 10
    FF 00                      ; fin d'étape
  ; étape 4
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 27 00 FF F5          ; dessin banque 1 frame 59 dy 39 dx -11
    04 2D 24 00 00 0A          ; dessin banque 1 frame 45 dy 36 dx 10
    FF FF                      ; fin du script
```

### `LAB_0866` (4:$4128)

Rôles : cible de saut depuis LAB_0867 ; référencé par le code dans LAB_0262

```
  ; étape 1
    B0 00 00 00 72 34          ; $B0 Call -> LAB_02E1
    88 02                      ; $88 Hold
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 27 00 FF F5          ; dessin banque 1 frame 59 dy 39 dx -11
    04 2D 24 00 00 0A          ; dessin banque 1 frame 45 dy 36 dx 10
    FF FF                      ; fin du script
```

### `LAB_0867` (4:$4150)

Rôles : référencé par le code dans LAB_0261

```
  LAB_0866:
  ; étape 1
    B0 00 00 00 72 34          ; $B0 Call -> LAB_02E1
    88 02                      ; $88 Hold
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3B 27 00 FF F5          ; dessin banque 1 frame 59 dy 39 dx -11
    04 2D 24 00 00 0A          ; dessin banque 1 frame 45 dy 36 dx 10
    FF FF                      ; fin du script
  ; étape 2
    B0 00 00 00 72 96          ; $B0 Call -> LAB_02E6
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    04 2D F7 00 00 05          ; dessin banque 1 frame 45 dy -9 dx 5
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3C 17 00 FF F7          ; dessin banque 1 frame 60 dy 23 dx -9
    04 3D 3B 00 FF F8          ; dessin banque 1 frame 61 dy 59 dx -8
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    04 26 0F 80 00 06          ; dessin banque 1 frame 38 dy 15 dx 6
    04 1F 0A 80 FF F4          ; dessin banque 1 frame 31 dy 10 dx -12
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    B0 00 00 00 72 1C          ; $B0 Call -> LAB_02E0
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    04 2D F7 00 00 05          ; dessin banque 1 frame 45 dy -9 dx 5
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3C 17 00 FF F7          ; dessin banque 1 frame 60 dy 23 dx -9
    04 3D 3B 00 FF F8          ; dessin banque 1 frame 61 dy 59 dx -8
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    04 20 05 80 FF DA          ; dessin banque 1 frame 32 dy 5 dx -38
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    84 00 00 00 41 28          ; $84 Jump -> LAB_0866
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    04 2D F7 00 00 05          ; dessin banque 1 frame 45 dy -9 dx 5
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3C 17 00 FF F7          ; dessin banque 1 frame 60 dy 23 dx -9
    04 3D 3B 00 FF F8          ; dessin banque 1 frame 61 dy 59 dx -8
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    04 22 17 80 FF F2          ; dessin banque 1 frame 34 dy 23 dx -14
    FF FF                      ; fin du script
```

### `LAB_0868` (4:$41F0)

Rôles : référencé par le code dans LAB_0261

```
  ; étape 1
    A4 5B                      ; $A4 Sound
    A4 5C                      ; $A4 Sound
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    04 17 F7 00 FF FD          ; dessin banque 1 frame 23 dy -9 dx -3
    04 1B 05 00 FF D4          ; dessin banque 1 frame 27 dy 5 dx -44
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    04 26 02 80 00 10          ; dessin banque 1 frame 38 dy 2 dx 16
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    04 24 FF 10 00 04          ; dessin banque 1 frame 36 dy -1 dx 4 [décor]
    00 3F F2 10 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23 [décor]
    04 27 02 90 00 10          ; dessin banque 1 frame 39 dy 2 dx 16 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    BC 00                      ; $BC Kill
    04 24 FF 10 00 04          ; dessin banque 1 frame 36 dy -1 dx 4 [décor]
    00 3F F2 10 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23 [décor]
    04 27 02 90 00 10          ; dessin banque 1 frame 39 dy 2 dx 16 [décor]
    FF FF                      ; fin du script
```

### `LAB_0869` (4:$4240)

Rôles : référencé par le code dans LAB_0262

```
  ; étape 1
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    00 37 0F 00 FF FF          ; dessin banque 0 frame 55 dy 15 dx -1
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 36 F8 00 00 11          ; dessin banque 0 frame 54 dy -8 dx 17
    04 3E 26 00 FF F7          ; dessin banque 1 frame 62 dy 38 dx -9
    FF FF                      ; fin du script
```

### `LAB_086A` (4:$4260)

Rôles : LAB_0602 [objet+30 (scripts objet+30) ; posée par LAB_018F] : [2] ; référencé par le code dans LAB_0158

```
  ; étape 1
    B0 00 00 00 72 1C          ; $B0 Call -> LAB_02E0
    04 29 0E 00 FF F5          ; dessin banque 1 frame 41 dy 14 dx -11
    04 1C 08 80 FF DE          ; dessin banque 1 frame 28 dy 8 dx -34
    FF 00                      ; fin d'étape
  ; étape 2
    A4 09                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 16 24 00 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49
    04 1D 17 80 FF B0          ; dessin banque 1 frame 29 dy 23 dx -80
    04 38 33 90 FF D0          ; dessin banque 1 frame 56 dy 51 dx -48 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 42 B4          ; $B4 IfDead -> LAB_086B
    88 02                      ; $88 Hold
    04 16 24 00 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49
    04 0C 26 80 FF D6          ; dessin banque 1 frame 12 dy 38 dx -42
    04 0B 21 80 FF 99          ; dessin banque 1 frame 11 dy 33 dx -103
    04 21 17 80 00 07          ; dessin banque 1 frame 33 dy 23 dx 7
    04 38 33 90 FF D0          ; dessin banque 1 frame 56 dy 51 dx -48 [décor]
    FF FF                      ; fin du script
  LAB_086B:
  ; étape 4
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 16 24 10 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49 [décor]
    04 39 32 90 FF D2          ; dessin banque 1 frame 57 dy 50 dx -46 [décor]
    00 12 28 90 FF D7          ; dessin banque 0 frame 18 dy 40 dx -41 [décor]
    FF 00                      ; fin d'étape
  ; étape 5
    BC 00                      ; $BC Kill
    04 16 24 10 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49 [décor]
    04 39 32 90 FF D2          ; dessin banque 1 frame 57 dy 50 dx -46 [décor]
    00 12 28 90 FF D7          ; dessin banque 0 frame 18 dy 40 dx -41 [décor]
    FF FF                      ; fin du script
```

### `LAB_086B` (4:$42B4)

Rôles : cible de saut depuis LAB_086A ; référencé par le code dans LAB_0270

```
  ; étape 1
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 16 24 10 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49 [décor]
    04 39 32 90 FF D2          ; dessin banque 1 frame 57 dy 50 dx -46 [décor]
    00 12 28 90 FF D7          ; dessin banque 0 frame 18 dy 40 dx -41 [décor]
    FF 00                      ; fin d'étape
  ; étape 2
    BC 00                      ; $BC Kill
    04 16 24 10 FF CF          ; dessin banque 1 frame 22 dy 36 dx -49 [décor]
    04 39 32 90 FF D2          ; dessin banque 1 frame 57 dy 50 dx -46 [décor]
    00 12 28 90 FF D7          ; dessin banque 0 frame 18 dy 40 dx -41 [décor]
    FF FF                      ; fin du script
```

### `LAB_086C` (4:$42E6)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 0 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 04 18 01 FF F4          ; dessin banque 0 frame 4 dy 24 dx -12 [corps]
    FF FF                      ; fin du script
```

### `LAB_086D` (4:$42EE)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 1 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 05 18 01 FF F4          ; dessin banque 0 frame 5 dy 24 dx -12 [corps]
    FF FF                      ; fin du script
```

### `LAB_086E` (4:$42F6)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 2 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 06 18 01 FF F4          ; dessin banque 0 frame 6 dy 24 dx -12 [corps]
    FF FF                      ; fin du script
```

### `LAB_086F` (4:$42FE)

Rôles : LAB_0612 [objet+46 (marche) ; posée par LAB_018F] : marche horizontal phase 3 ; référencé par le code dans LAB_0158

```
  ; étape 1
    00 07 18 01 FF F4          ; dessin banque 0 frame 7 dy 24 dx -12 [corps]
    FF FF                      ; fin du script
```

### `LAB_0870` (4:$4306)

Rôles : cible de saut depuis LAB_0870 ; référencé par le code dans LAB_018C

```
  ; étape 1
    84 00 00 00 43 06          ; $84 Jump -> LAB_0870
    00 3F F2 00 FF E9          ; dessin banque 0 frame 63 dy -14 dx -23
    00 3F F5 00 FF FB          ; dessin banque 0 frame 63 dy -11 dx -5
    00 3F EE 00 00 0D          ; dessin banque 0 frame 63 dy -18 dx 13
    FF FF                      ; fin du script
```

### `LAB_0871` (4:$4320)

Rôles : cible de saut depuis LAB_087F

```
  ; étape 1
    A4 1C                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 00 F3 00 FF CD          ; dessin banque 1 frame 0 dy -13 dx -51
    04 01 00 00 FF BC          ; dessin banque 1 frame 1 dy 0 dx -68
    04 02 0A 00 FF A7          ; dessin banque 1 frame 2 dy 10 dx -89
    04 03 09 00 FF 91          ; dessin banque 1 frame 3 dy 9 dx -111
    00 0C E3 00 FF C0          ; dessin banque 0 frame 12 dy -29 dx -64
    00 0D C2 00 00 00          ; dessin banque 0 frame 13 dy -62 dx 0
    00 13 EB 00 FF A2          ; dessin banque 0 frame 19 dy -21 dx -94
    04 03 09 00 FF 72          ; dessin banque 1 frame 3 dy 9 dx -142
    FF 00                      ; fin d'étape
  ; étape 2
    8C FF 14 02 08 40 00 00    ; $8C Physics
    88 02                      ; $88 Hold
    04 03 09 00 FF AF          ; dessin banque 1 frame 3 dy 9 dx -81
    04 03 09 00 FF 92          ; dessin banque 1 frame 3 dy 9 dx -110
    04 03 09 00 FF 74          ; dessin banque 1 frame 3 dy 9 dx -140
    04 03 09 00 FF CD          ; dessin banque 1 frame 3 dy 9 dx -51
    04 02 0A 00 FF E1          ; dessin banque 1 frame 2 dy 10 dx -31
    00 0C 08 00 FF D5          ; dessin banque 0 frame 12 dy 8 dx -43
    00 0D E9 00 00 15          ; dessin banque 0 frame 13 dy -23 dx 21
    00 13 10 00 FF B7          ; dessin banque 0 frame 19 dy 16 dx -73
    FF 00                      ; fin d'étape
  ; étape 3
    A4 21                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 18 33 A0 00 22          ; dessin banque 4 frame 24 dy 51 dx 34
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 19 33 A0 00 1E          ; dessin banque 4 frame 25 dy 51 dx 30
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1A 33 A0 00 1D          ; dessin banque 4 frame 26 dy 51 dx 29
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1B 33 A0 00 1A          ; dessin banque 4 frame 27 dy 51 dx 26
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1C 33 A0 00 17          ; dessin banque 4 frame 28 dy 51 dx 23
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1D 32 A0 00 19          ; dessin banque 4 frame 29 dy 50 dx 25
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    04 03 09 14 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35 [décor]
    04 03 0A 14 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5 [décor]
    04 03 09 14 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97 [décor]
    04 03 09 14 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66 [décor]
    00 14 13 10 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56 [décor]
    00 06 13 14 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34 [décor]
    10 17 2D B0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42 [décor]
    10 1E 33 B0 00 14          ; dessin banque 4 frame 30 dy 51 dx 20 [décor]
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 10
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0872` (4:$455E)

Rôles : référencé par le code dans LAB_0286

```
  ; étape 1
    04 03 09 01 FF F2          ; dessin banque 1 frame 3 dy 9 dx -14 [corps]
    04 03 09 01 FF D4          ; dessin banque 1 frame 3 dy 9 dx -44 [corps]
    04 03 09 01 FF B6          ; dessin banque 1 frame 3 dy 9 dx -74 [corps]
    04 03 09 01 FF 97          ; dessin banque 1 frame 3 dy 9 dx -105 [corps]
    00 07 14 01 FF D6          ; dessin banque 0 frame 7 dy 20 dx -42 [corps]
    00 08 36 00 FF F3          ; dessin banque 0 frame 8 dy 54 dx -13
    04 10 22 01 00 20          ; dessin banque 1 frame 16 dy 34 dx 32 [corps]
    04 0E 1B 00 00 20          ; dessin banque 1 frame 14 dy 27 dx 32
    FF 00                      ; fin d'étape
  ; étape 2
    04 02 0B 01 FF 90          ; dessin banque 1 frame 2 dy 11 dx -112 [corps]
    04 02 00 01 FF AB          ; dessin banque 1 frame 2 dy 0 dx -85 [corps]
    04 03 F6 01 FF C9          ; dessin banque 1 frame 3 dy -10 dx -55 [corps]
    04 04 FA 01 FF E1          ; dessin banque 1 frame 4 dy -6 dx -31 [corps]
    00 07 0D 01 FF C4          ; dessin banque 0 frame 7 dy 13 dx -60 [corps]
    00 08 2F 00 FF E1          ; dessin banque 0 frame 8 dy 47 dx -31
    04 10 1B 01 00 0E          ; dessin banque 1 frame 16 dy 27 dx 14 [corps]
    04 0F 14 00 00 0E          ; dessin banque 1 frame 15 dy 20 dx 14
    FF 00                      ; fin d'étape
  ; étape 3
    88 04                      ; $88 Hold
    04 01 0D 01 FF 8A          ; dessin banque 1 frame 1 dy 13 dx -118 [corps]
    04 01 FE 01 FF A3          ; dessin banque 1 frame 1 dy -2 dx -93 [corps]
    04 03 F0 01 FF C3          ; dessin banque 1 frame 3 dy -16 dx -61 [corps]
    04 04 F5 01 FF D9          ; dessin banque 1 frame 4 dy -11 dx -39 [corps]
    00 07 08 01 FF BF          ; dessin banque 0 frame 7 dy 8 dx -65 [corps]
    00 08 2A 00 FF DC          ; dessin banque 0 frame 8 dy 42 dx -36
    04 10 16 01 00 09          ; dessin banque 1 frame 16 dy 22 dx 9 [corps]
    04 0F 0F 01 00 09          ; dessin banque 1 frame 15 dy 15 dx 9 [corps]
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 6D 28          ; $B0 Call -> LAB_02AC
    94 05                      ; $94 Loop
    B0 00 00 00 68 82          ; $B0 Call -> LAB_028E
    04 03 09 00 FF F2          ; dessin banque 1 frame 3 dy 9 dx -14
    04 03 09 00 FF D4          ; dessin banque 1 frame 3 dy 9 dx -44
    04 03 09 01 FF B6          ; dessin banque 1 frame 3 dy 9 dx -74 [corps]
    04 03 09 01 FF 97          ; dessin banque 1 frame 3 dy 9 dx -105 [corps]
    00 09 04 01 FF D1          ; dessin banque 0 frame 9 dy 4 dx -47 [corps]
    00 0A 1A 01 00 1A          ; dessin banque 0 frame 10 dy 26 dx 26 [corps]
    00 0B 3C 00 00 00          ; dessin banque 0 frame 11 dy 60 dx 0
    04 17 21 02 00 2B          ; dessin banque 1 frame 23 dy 33 dx 43 [frappe]
    04 18 24 02 00 86          ; dessin banque 1 frame 24 dy 36 dx 134 [frappe]
    04 19 24 02 00 AF          ; dessin banque 1 frame 25 dy 36 dx 175 [frappe]
    04 18 24 02 00 D9          ; dessin banque 1 frame 24 dy 36 dx 217 [frappe]
    04 19 24 02 01 01          ; dessin banque 1 frame 25 dy 36 dx 257 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    04 03 09 00 FF F2          ; dessin banque 1 frame 3 dy 9 dx -14
    04 03 09 00 FF D4          ; dessin banque 1 frame 3 dy 9 dx -44
    04 03 09 01 FF B6          ; dessin banque 1 frame 3 dy 9 dx -74 [corps]
    04 03 09 01 FF 97          ; dessin banque 1 frame 3 dy 9 dx -105 [corps]
    00 09 04 01 FF D1          ; dessin banque 0 frame 9 dy 4 dx -47 [corps]
    00 0A 1A 01 00 1A          ; dessin banque 0 frame 10 dy 26 dx 26 [corps]
    00 0B 3C 00 00 00          ; dessin banque 0 frame 11 dy 60 dx 0
    04 16 21 02 00 2B          ; dessin banque 1 frame 22 dy 33 dx 43 [frappe]
    04 19 24 02 00 86          ; dessin banque 1 frame 25 dy 36 dx 134 [frappe]
    04 18 24 02 00 B0          ; dessin banque 1 frame 24 dy 36 dx 176 [frappe]
    04 19 24 02 00 D9          ; dessin banque 1 frame 25 dy 36 dx 217 [frappe]
    04 18 24 02 01 03          ; dessin banque 1 frame 24 dy 36 dx 259 [frappe]
    FF FE                      ; fin d'étape, boucle
  ; étape 6
    B0 00 00 00 00 40          ; $B0 Call -> LAB_0A9E
    04 02 0B 01 FF 90          ; dessin banque 1 frame 2 dy 11 dx -112 [corps]
    04 02 00 01 FF AB          ; dessin banque 1 frame 2 dy 0 dx -85 [corps]
    04 03 F6 01 FF C9          ; dessin banque 1 frame 3 dy -10 dx -55 [corps]
    04 04 FA 01 FF E1          ; dessin banque 1 frame 4 dy -6 dx -31 [corps]
    00 07 0D 01 FF C4          ; dessin banque 0 frame 7 dy 13 dx -60 [corps]
    00 08 2F 00 FF E1          ; dessin banque 0 frame 8 dy 47 dx -31
    04 10 1B 01 00 0E          ; dessin banque 1 frame 16 dy 27 dx 14 [corps]
    04 0E 14 00 00 0E          ; dessin banque 1 frame 14 dy 20 dx 14
    FF FF                      ; fin du script
```

### `LAB_0873` (4:$46D0)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 0 ; référencé par le code dans LAB_015D

```
  ; étape 1
    A8 04 00 16 00 00 47 DA    ; $A8 SetField -> LAB_0878
    04 03 09 01 FF F2          ; dessin banque 1 frame 3 dy 9 dx -14 [corps]
    04 03 09 01 FF D4          ; dessin banque 1 frame 3 dy 9 dx -44 [corps]
    04 03 09 01 FF B6          ; dessin banque 1 frame 3 dy 9 dx -74 [corps]
    04 03 09 01 FF 97          ; dessin banque 1 frame 3 dy 9 dx -105 [corps]
    00 07 14 01 FF D6          ; dessin banque 0 frame 7 dy 20 dx -42 [corps]
    00 08 36 00 FF F3          ; dessin banque 0 frame 8 dy 54 dx -13
    04 10 22 01 00 20          ; dessin banque 1 frame 16 dy 34 dx 32 [corps]
    04 0E 1B 00 00 20          ; dessin banque 1 frame 14 dy 27 dx 32
    FF FF                      ; fin du script
```

### `LAB_0874` (4:$470A)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 1 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 02 0B 00 FF 82          ; dessin banque 1 frame 2 dy 11 dx -126
    04 02 00 01 FF 9C          ; dessin banque 1 frame 2 dy 0 dx -100 [corps]
    04 03 F6 01 FF BA          ; dessin banque 1 frame 3 dy -10 dx -70 [corps]
    04 03 F6 01 FF D7          ; dessin banque 1 frame 3 dy -10 dx -41 [corps]
    04 04 FB 01 FF ED          ; dessin banque 1 frame 4 dy -5 dx -19 [corps]
    00 07 0F 00 FF D1          ; dessin banque 0 frame 7 dy 15 dx -47
    00 08 31 00 FF EE          ; dessin banque 0 frame 8 dy 49 dx -18
    04 10 1E 01 00 1B          ; dessin banque 1 frame 16 dy 30 dx 27 [corps]
    04 0E 17 00 00 1B          ; dessin banque 1 frame 14 dy 23 dx 27
    FF FF                      ; fin du script
```

### `LAB_0875` (4:$4742)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 2 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 01 0C 01 FF 80          ; dessin banque 1 frame 1 dy 12 dx -128 [corps]
    04 01 FD 01 FF 98          ; dessin banque 1 frame 1 dy -3 dx -104 [corps]
    04 02 EE 01 FF B4          ; dessin banque 1 frame 2 dy -18 dx -76 [corps]
    04 03 E4 01 FF D2          ; dessin banque 1 frame 3 dy -28 dx -46 [corps]
    04 04 E9 01 FF E8          ; dessin banque 1 frame 4 dy -23 dx -24 [corps]
    00 07 FB 00 FF C8          ; dessin banque 0 frame 7 dy -5 dx -56
    00 08 1C 00 FF E6          ; dessin banque 0 frame 8 dy 28 dx -26
    04 10 08 01 00 12          ; dessin banque 1 frame 16 dy 8 dx 18 [corps]
    04 0F 01 00 00 12          ; dessin banque 1 frame 15 dy 1 dx 18
    FF FF                      ; fin du script
```

### `LAB_0876` (4:$477A)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 3 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 00 0F 00 FF 80          ; dessin banque 1 frame 0 dy 15 dx -128
    04 00 FB 01 FF 96          ; dessin banque 1 frame 0 dy -5 dx -106 [corps]
    04 01 E7 01 FF AF          ; dessin banque 1 frame 1 dy -25 dx -81 [corps]
    04 03 D8 00 FF D0          ; dessin banque 1 frame 3 dy -40 dx -48
    04 05 E2 01 FF E2          ; dessin banque 1 frame 5 dy -30 dx -30 [corps]
    00 0F DF 00 FF FD          ; dessin banque 0 frame 15 dy -33 dx -3
    00 0E C4 00 FF D2          ; dessin banque 0 frame 14 dy -60 dx -46
    00 15 14 01 FF FD          ; dessin banque 0 frame 21 dy 20 dx -3 [corps]
    FF FF                      ; fin du script
```

### `LAB_0877` (4:$47AC)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 4 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 5 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 6 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche haut phase 7 ; référencé par le code dans LAB_015D

```
  ; étape 1
    88 02                      ; $88 Hold
    04 00 0E 00 FF 76          ; dessin banque 1 frame 0 dy 14 dx -138
    04 00 F9 01 FF 8C          ; dessin banque 1 frame 0 dy -7 dx -116 [corps]
    04 00 E5 01 FF A2          ; dessin banque 1 frame 0 dy -27 dx -94 [corps]
    04 02 D1 00 FF BE          ; dessin banque 1 frame 2 dy -47 dx -66
    00 15 FD 01 FF F4          ; dessin banque 0 frame 21 dy -3 dx -12 [corps]
    00 0F C8 01 FF F4          ; dessin banque 0 frame 15 dy -56 dx -12 [corps]
    00 0E AF 01 FF C9          ; dessin banque 0 frame 14 dy -81 dx -55 [corps]
    FF FF                      ; fin du script
```

### `LAB_0878` (4:$47DA)

Rôles : écrit dans objet+22 (repos) par $A8 depuis LAB_0873

```
  ; étape 1
    88 02                      ; $88 Hold
    AC 01 00 00 4D AA          ; $AC Shadow -> LAB_0885
    04 00 0E 00 FF 76          ; dessin banque 1 frame 0 dy 14 dx -138
    04 00 F9 01 FF 8C          ; dessin banque 1 frame 0 dy -7 dx -116 [corps]
    04 00 E5 01 FF A2          ; dessin banque 1 frame 0 dy -27 dx -94 [corps]
    04 02 D1 00 FF BE          ; dessin banque 1 frame 2 dy -47 dx -66
    00 15 FD 01 FF F4          ; dessin banque 0 frame 21 dy -3 dx -12 [corps]
    00 0F C8 00 FF F4          ; dessin banque 0 frame 15 dy -56 dx -12
    00 0E AF 01 FF C9          ; dessin banque 0 frame 14 dy -81 dx -55 [corps]
    FF FF                      ; fin du script
```

### `LAB_0879` (4:$480E)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 0 ; référencé par le code dans LAB_015D

```
  ; étape 1
    88 02                      ; $88 Hold
    A8 04 00 16 00 00 4A A4    ; $A8 SetField -> LAB_0882
    04 00 0E 01 FF 76          ; dessin banque 1 frame 0 dy 14 dx -138 [corps]
    04 00 F9 01 FF 8C          ; dessin banque 1 frame 0 dy -7 dx -116 [corps]
    04 00 E5 01 FF A2          ; dessin banque 1 frame 0 dy -27 dx -94 [corps]
    04 02 D1 00 FF BE          ; dessin banque 1 frame 2 dy -47 dx -66
    00 15 FD 01 FF F4          ; dessin banque 0 frame 21 dy -3 dx -12 [corps]
    00 0F C8 00 FF F4          ; dessin banque 0 frame 15 dy -56 dx -12
    00 0E AF 01 FF C9          ; dessin banque 0 frame 14 dy -81 dx -55 [corps]
    FF FF                      ; fin du script
```

### `LAB_087A` (4:$4844)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 1 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 00 0F 00 FF 80          ; dessin banque 1 frame 0 dy 15 dx -128
    04 00 FB 01 FF 96          ; dessin banque 1 frame 0 dy -5 dx -106 [corps]
    04 01 E7 00 FF AF          ; dessin banque 1 frame 1 dy -25 dx -81
    04 03 D8 00 FF D0          ; dessin banque 1 frame 3 dy -40 dx -48
    04 05 E2 00 FF E2          ; dessin banque 1 frame 5 dy -30 dx -30
    00 0F DF 00 FF FD          ; dessin banque 0 frame 15 dy -33 dx -3
    00 0E C4 01 FF D2          ; dessin banque 0 frame 14 dy -60 dx -46 [corps]
    00 15 14 01 FF FD          ; dessin banque 0 frame 21 dy 20 dx -3 [corps]
    FF FF                      ; fin du script
```

### `LAB_087B` (4:$4876)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 2 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 01 0C 01 FF 80          ; dessin banque 1 frame 1 dy 12 dx -128 [corps]
    04 01 FD 00 FF 98          ; dessin banque 1 frame 1 dy -3 dx -104
    04 02 EE 00 FF B4          ; dessin banque 1 frame 2 dy -18 dx -76
    04 03 E4 00 FF D2          ; dessin banque 1 frame 3 dy -28 dx -46
    04 04 E9 00 FF E8          ; dessin banque 1 frame 4 dy -23 dx -24
    00 07 FB 01 FF C8          ; dessin banque 0 frame 7 dy -5 dx -56 [corps]
    00 08 1C 00 FF E6          ; dessin banque 0 frame 8 dy 28 dx -26
    04 10 08 01 00 12          ; dessin banque 1 frame 16 dy 8 dx 18 [corps]
    04 0E 01 00 00 12          ; dessin banque 1 frame 14 dy 1 dx 18
    FF FF                      ; fin du script
```

### `LAB_087C` (4:$48AE)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 3 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 02 0B 01 FF 82          ; dessin banque 1 frame 2 dy 11 dx -126 [corps]
    04 02 00 01 FF 9C          ; dessin banque 1 frame 2 dy 0 dx -100 [corps]
    04 03 F6 00 FF BA          ; dessin banque 1 frame 3 dy -10 dx -70
    04 03 F6 00 FF D7          ; dessin banque 1 frame 3 dy -10 dx -41
    04 04 FB 00 FF ED          ; dessin banque 1 frame 4 dy -5 dx -19
    00 07 0F 01 FF D1          ; dessin banque 0 frame 7 dy 15 dx -47 [corps]
    00 08 31 00 FF EE          ; dessin banque 0 frame 8 dy 49 dx -18
    04 10 1D 01 00 1A          ; dessin banque 1 frame 16 dy 29 dx 26 [corps]
    04 0E 16 00 00 1A          ; dessin banque 1 frame 14 dy 22 dx 26
    FF FF                      ; fin du script
```

### `LAB_087D` (4:$48E6)

Rôles : LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 4 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 5 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 6 ; LAB_060F [objet+46 (marche) ; posée par LAB_0195] : marche bas phase 7 ; référencé par le code dans LAB_015D

```
  ; étape 1
    04 03 09 01 FF F2          ; dessin banque 1 frame 3 dy 9 dx -14 [corps]
    04 03 09 01 FF D4          ; dessin banque 1 frame 3 dy 9 dx -44 [corps]
    04 03 09 01 FF B6          ; dessin banque 1 frame 3 dy 9 dx -74 [corps]
    04 03 09 01 FF 97          ; dessin banque 1 frame 3 dy 9 dx -105 [corps]
    00 07 14 01 FF D6          ; dessin banque 0 frame 7 dy 20 dx -42 [corps]
    00 08 36 00 FF F3          ; dessin banque 0 frame 8 dy 54 dx -13
    04 10 22 01 00 20          ; dessin banque 1 frame 16 dy 34 dx 32 [corps]
    04 0E 1B 00 00 20          ; dessin banque 1 frame 14 dy 27 dx 32
    FF FF                      ; fin du script
```

### `LAB_087E` (4:$4918)

Rôles : référencé par le code dans LAB_0284

```
  ; étape 1
    B0 00 00 00 6D 28          ; $B0 Call -> LAB_02AC
    94 05                      ; $94 Loop
    B0 00 00 00 68 82          ; $B0 Call -> LAB_028E
    04 00 0E 01 FF 80          ; dessin banque 1 frame 0 dy 14 dx -128 [corps]
    04 00 F9 01 FF 96          ; dessin banque 1 frame 0 dy -7 dx -106 [corps]
    04 01 E5 01 FF AF          ; dessin banque 1 frame 1 dy -27 dx -81 [corps]
    04 03 D6 00 FF D0          ; dessin banque 1 frame 3 dy -42 dx -48
    04 05 E0 00 FF E2          ; dessin banque 1 frame 5 dy -32 dx -30
    00 11 D6 01 FF CF          ; dessin banque 0 frame 17 dy -42 dx -49 [corps]
    00 10 B1 00 FF E5          ; dessin banque 0 frame 16 dy -79 dx -27
    10 0F 08 21 FF ED          ; dessin banque 4 frame 15 dy 8 dx -19 [corps]
    10 10 32 22 00 05          ; dessin banque 4 frame 16 dy 50 dx 5 [frappe]
    10 12 F0 20 00 24          ; dessin banque 4 frame 18 dy -16 dx 36
    10 15 4A 22 00 19          ; dessin banque 4 frame 21 dy 74 dx 25 [frappe]
    10 25 22 22 00 07          ; dessin banque 4 frame 37 dy 34 dx 7 [frappe]
    10 26 22 20 FF F6          ; dessin banque 4 frame 38 dy 34 dx -10
    10 28 12 20 00 16          ; dessin banque 4 frame 40 dy 18 dx 22
    10 29 08 20 00 23          ; dessin banque 4 frame 41 dy 8 dx 35
    FF 00                      ; fin d'étape
  ; étape 2
    04 00 0E 00 FF 80          ; dessin banque 1 frame 0 dy 14 dx -128
    04 00 F9 01 FF 96          ; dessin banque 1 frame 0 dy -7 dx -106 [corps]
    04 01 E5 00 FF AF          ; dessin banque 1 frame 1 dy -27 dx -81
    04 03 D6 00 FF D0          ; dessin banque 1 frame 3 dy -42 dx -48
    04 05 E0 00 FF E2          ; dessin banque 1 frame 5 dy -32 dx -30
    00 11 D6 01 FF CF          ; dessin banque 0 frame 17 dy -42 dx -49 [corps]
    00 10 B1 00 FF E5          ; dessin banque 0 frame 16 dy -79 dx -27
    10 13 08 21 FF ED          ; dessin banque 4 frame 19 dy 8 dx -19 [corps]
    10 14 32 22 00 05          ; dessin banque 4 frame 20 dy 50 dx 5 [frappe]
    10 16 E5 20 00 24          ; dessin banque 4 frame 22 dy -27 dx 36
    10 11 47 22 00 14          ; dessin banque 4 frame 17 dy 71 dx 20 [frappe]
    10 15 62 22 00 2B          ; dessin banque 4 frame 21 dy 98 dx 43 [frappe]
    10 24 22 22 00 07          ; dessin banque 4 frame 36 dy 34 dx 7 [frappe]
    10 27 22 20 FF F6          ; dessin banque 4 frame 39 dy 34 dx -10
    10 2A 12 20 00 16          ; dessin banque 4 frame 42 dy 18 dx 22
    10 2B 08 20 00 23          ; dessin banque 4 frame 43 dy 8 dx 35
    FF FE                      ; fin d'étape, boucle
  ; étape 3
    B0 00 00 00 00 40          ; $B0 Call -> LAB_0A9E
    04 00 0E 00 FF 76          ; dessin banque 1 frame 0 dy 14 dx -138
    04 00 F9 00 FF 8C          ; dessin banque 1 frame 0 dy -7 dx -116
    04 00 E5 01 FF A2          ; dessin banque 1 frame 0 dy -27 dx -94 [corps]
    04 02 D1 01 FF BE          ; dessin banque 1 frame 2 dy -47 dx -66 [corps]
    04 04 CD 01 FF D6          ; dessin banque 1 frame 4 dy -51 dx -42 [corps]
    00 0F C8 01 FF F4          ; dessin banque 0 frame 15 dy -56 dx -12 [corps]
    00 0E AF 00 FF C9          ; dessin banque 0 frame 14 dy -81 dx -55
    00 15 FD 01 FF F4          ; dessin banque 0 frame 21 dy -3 dx -12 [corps]
    FF FF                      ; fin du script
```

### `LAB_087F` (4:$4A1C)

Rôles : LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [1] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [2] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [3] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [4] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [5] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [6] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [7] ; LAB_0604 [objet+30 (scripts objet+30) ; posée par LAB_0195] : [8] ; référencé par le code dans LAB_015D

```
  LAB_0871:
  ; étape 1
    A4 1C                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 00 F3 00 FF CD          ; dessin banque 1 frame 0 dy -13 dx -51
    04 01 00 00 FF BC          ; dessin banque 1 frame 1 dy 0 dx -68
    04 02 0A 00 FF A7          ; dessin banque 1 frame 2 dy 10 dx -89
    04 03 09 00 FF 91          ; dessin banque 1 frame 3 dy 9 dx -111
    00 0C E3 00 FF C0          ; dessin banque 0 frame 12 dy -29 dx -64
    00 0D C2 00 00 00          ; dessin banque 0 frame 13 dy -62 dx 0
    00 13 EB 00 FF A2          ; dessin banque 0 frame 19 dy -21 dx -94
    04 03 09 00 FF 72          ; dessin banque 1 frame 3 dy 9 dx -142
    FF 00                      ; fin d'étape
  ; étape 2
    8C FF 14 02 08 40 00 00    ; $8C Physics
    88 02                      ; $88 Hold
    04 03 09 00 FF AF          ; dessin banque 1 frame 3 dy 9 dx -81
    04 03 09 00 FF 92          ; dessin banque 1 frame 3 dy 9 dx -110
    04 03 09 00 FF 74          ; dessin banque 1 frame 3 dy 9 dx -140
    04 03 09 00 FF CD          ; dessin banque 1 frame 3 dy 9 dx -51
    04 02 0A 00 FF E1          ; dessin banque 1 frame 2 dy 10 dx -31
    00 0C 08 00 FF D5          ; dessin banque 0 frame 12 dy 8 dx -43
    00 0D E9 00 00 15          ; dessin banque 0 frame 13 dy -23 dx 21
    00 13 10 00 FF B7          ; dessin banque 0 frame 19 dy 16 dx -73
    FF 00                      ; fin d'étape
  ; étape 3
    A4 21                      ; $A4 Sound
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 18 33 A0 00 22          ; dessin banque 4 frame 24 dy 51 dx 34
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 19 33 A0 00 1E          ; dessin banque 4 frame 25 dy 51 dx 30
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1A 33 A0 00 1D          ; dessin banque 4 frame 26 dy 51 dx 29
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1B 33 A0 00 1A          ; dessin banque 4 frame 27 dy 51 dx 26
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1C 33 A0 00 17          ; dessin banque 4 frame 28 dy 51 dx 23
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    04 03 09 04 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35
    04 03 0A 04 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5
    04 03 09 04 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97
    04 03 09 04 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66
    00 14 13 00 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56
    00 06 13 04 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34
    10 17 2D A0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42
    10 1D 32 A0 00 19          ; dessin banque 4 frame 29 dy 50 dx 25
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    04 03 09 14 FF DD          ; dessin banque 1 frame 3 dy 9 dx -35 [décor]
    04 03 0A 14 FF FB          ; dessin banque 1 frame 3 dy 10 dx -5 [décor]
    04 03 09 14 FF 9F          ; dessin banque 1 frame 3 dy 9 dx -97 [décor]
    04 03 09 14 FF BE          ; dessin banque 1 frame 3 dy 9 dx -66 [décor]
    00 14 13 10 FF C8          ; dessin banque 0 frame 20 dy 19 dx -56 [décor]
    00 06 13 14 FF DE          ; dessin banque 0 frame 6 dy 19 dx -34 [décor]
    10 17 2D B0 00 2A          ; dessin banque 4 frame 23 dy 45 dx 42 [décor]
    10 1E 33 B0 00 14          ; dessin banque 4 frame 30 dy 51 dx 20 [décor]
    04 03 09 00 FF 80          ; dessin banque 1 frame 3 dy 9 dx -128
    04 03 09 00 FF 62          ; dessin banque 1 frame 3 dy 9 dx -158
    FF 00                      ; fin d'étape
  ; étape 10
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
  ; étape 11
    A4 1C                      ; $A4 Sound
    88 02                      ; $88 Hold
    B4 00 00 00 43 20          ; $B4 IfDead -> LAB_0871
    04 00 F3 00 FF CD          ; dessin banque 1 frame 0 dy -13 dx -51
    04 01 00 00 FF BC          ; dessin banque 1 frame 1 dy 0 dx -68
    04 02 0A 00 FF A7          ; dessin banque 1 frame 2 dy 10 dx -89
    04 03 09 00 FF 91          ; dessin banque 1 frame 3 dy 9 dx -111
    00 0C E3 00 FF C0          ; dessin banque 0 frame 12 dy -29 dx -64
    00 0D C2 00 00 00          ; dessin banque 0 frame 13 dy -62 dx 0
    00 13 EB 00 FF A2          ; dessin banque 0 frame 19 dy -21 dx -94
    FF FF                      ; fin du script
```

### `LAB_0880` (4:$4A52)

Rôles : entrée 0 de la table LAB_0881+50 ; objet+22 (repos) dans LAB_0192 ; objet+26 (réaction) dans LAB_0192

```
  ; étape 1
    04 07 E6 01 FF E2          ; dessin banque 1 frame 7 dy -26 dx -30 [corps]
    04 08 22 01 FF E2          ; dessin banque 1 frame 8 dy 34 dx -30 [corps]
    FF FF                      ; fin du script
```

### `LAB_0881` (4:$4A60)

Rôles : référencé par le code dans LAB_0299

```
  ; étape 1
    88 02                      ; $88 Hold
    04 08 22 01 FF E3          ; dessin banque 1 frame 8 dy 34 dx -29 [corps]
    04 07 E6 01 FF E2          ; dessin banque 1 frame 7 dy -26 dx -30 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    04 09 DE 01 FF E7          ; dessin banque 1 frame 9 dy -34 dx -25 [corps]
    04 0A EF 03 00 10          ; dessin banque 1 frame 10 dy -17 dx 16 [corps,frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    88 04                      ; $88 Hold
    04 0B DB 01 FF F2          ; dessin banque 1 frame 11 dy -37 dx -14 [corps]
    04 0C E7 03 00 21          ; dessin banque 1 frame 12 dy -25 dx 33 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0882` (4:$4AA4)

Rôles : objet+22 (repos) dans LAB_0195 ; objet+26 (réaction) dans LAB_0195 ; écrit dans objet+22 (repos) par $A8 depuis LAB_0879

```
  ; étape 1
    AC 01 00 00 4D AA          ; $AC Shadow -> LAB_0885
    04 02 0B 00 FF 82          ; dessin banque 1 frame 2 dy 11 dx -126
    04 02 00 01 FF 9C          ; dessin banque 1 frame 2 dy 0 dx -100 [corps]
    04 03 F6 01 FF BA          ; dessin banque 1 frame 3 dy -10 dx -70 [corps]
    04 03 F6 00 FF D7          ; dessin banque 1 frame 3 dy -10 dx -41
    04 04 FB 00 FF ED          ; dessin banque 1 frame 4 dy -5 dx -19
    00 07 0F 01 FF D1          ; dessin banque 0 frame 7 dy 15 dx -47 [corps]
    00 08 31 00 FF EE          ; dessin banque 0 frame 8 dy 49 dx -18
    04 10 1E 01 00 1B          ; dessin banque 1 frame 16 dy 30 dx 27 [corps]
    04 0E 17 00 00 1B          ; dessin banque 1 frame 14 dy 23 dx 27
    FF FF                      ; fin du script
```

### `LAB_0883` (4:$4B1A)

Rôles : référencé par le code dans LAB_0285

```
  ; étape 1
    04 00 0F 01 FF 80          ; dessin banque 1 frame 0 dy 15 dx -128 [corps]
    04 00 FB 00 FF 96          ; dessin banque 1 frame 0 dy -5 dx -106
    04 01 E7 01 FF AF          ; dessin banque 1 frame 1 dy -25 dx -81 [corps]
    04 03 D8 00 FF D0          ; dessin banque 1 frame 3 dy -40 dx -48
    00 11 D8 01 FF CF          ; dessin banque 0 frame 17 dy -40 dx -49 [corps]
    00 10 B3 00 FF E5          ; dessin banque 0 frame 16 dy -77 dx -27
    00 12 0A 03 FF EB          ; dessin banque 0 frame 18 dy 10 dx -21 [corps,frappe]
    FF 00                      ; fin d'étape
  ; étape 2
    04 01 0C 01 FF 84          ; dessin banque 1 frame 1 dy 12 dx -124 [corps]
    04 01 FC 01 FF 9D          ; dessin banque 1 frame 1 dy -4 dx -99 [corps]
    04 02 ED 01 FF B9          ; dessin banque 1 frame 2 dy -19 dx -71 [corps]
    04 03 E3 00 FF D8          ; dessin banque 1 frame 3 dy -29 dx -40
    00 11 E5 01 FF D8          ; dessin banque 0 frame 17 dy -27 dx -40 [corps]
    00 12 17 03 FF F3          ; dessin banque 0 frame 18 dy 23 dx -13 [corps,frappe]
    00 10 BD 00 FF ED          ; dessin banque 0 frame 16 dy -67 dx -19
    FF 00                      ; fin d'étape
  ; étape 3
    04 02 0B 01 FF 88          ; dessin banque 1 frame 2 dy 11 dx -120 [corps]
    04 03 01 01 FF A5          ; dessin banque 1 frame 3 dy 1 dx -91 [corps]
    04 03 00 01 FF C2          ; dessin banque 1 frame 3 dy 0 dx -62 [corps]
    04 03 01 00 FF DD          ; dessin banque 1 frame 3 dy 1 dx -35
    04 04 05 00 FF F4          ; dessin banque 1 frame 4 dy 5 dx -12
    00 11 05 01 FF E4          ; dessin banque 0 frame 17 dy 5 dx -28 [corps]
    00 12 37 03 FF FF          ; dessin banque 0 frame 18 dy 55 dx -1 [corps,frappe]
    00 10 DD 00 FF F9          ; dessin banque 0 frame 16 dy -35 dx -7
    FF 00                      ; fin d'étape
  ; étape 4
    88 04                      ; $88 Hold
    04 02 0C 01 FF 88          ; dessin banque 1 frame 2 dy 12 dx -120 [corps]
    04 03 02 01 FF A5          ; dessin banque 1 frame 3 dy 2 dx -91 [corps]
    04 03 02 01 FF C2          ; dessin banque 1 frame 3 dy 2 dx -62 [corps]
    04 03 02 00 FF DD          ; dessin banque 1 frame 3 dy 2 dx -35
    04 04 07 00 FF F3          ; dessin banque 1 frame 4 dy 7 dx -13
    00 0F 04 01 00 0F          ; dessin banque 0 frame 15 dy 4 dx 15 [corps]
    00 0E E9 01 FF E4          ; dessin banque 0 frame 14 dy -23 dx -28 [corps]
    00 15 39 03 00 0F          ; dessin banque 0 frame 21 dy 57 dx 15 [corps,frappe]
    FF FF                      ; fin du script
```

### `LAB_0884` (4:$4BD8)

Rôles : référencé par le code dans LAB_028D

```
  ; étape 1
    94 06                      ; $94 Loop
    B0 00 00 00 71 D2          ; $B0 Call -> LAB_02DC
    04 02 E9 01 FF 88          ; dessin banque 1 frame 2 dy -23 dx -120 [corps]
    04 03 DF 01 FF A5          ; dessin banque 1 frame 3 dy -33 dx -91 [corps]
    04 03 DF 01 FF C2          ; dessin banque 1 frame 3 dy -33 dx -62 [corps]
    04 03 DF 00 FF DD          ; dessin banque 1 frame 3 dy -33 dx -35
    04 04 E4 00 FF F3          ; dessin banque 1 frame 4 dy -28 dx -13
    00 0F E1 01 00 0F          ; dessin banque 0 frame 15 dy -31 dx 15 [corps]
    00 0E C6 01 FF E4          ; dessin banque 0 frame 14 dy -58 dx -28 [corps]
    10 1F 16 20 00 08          ; dessin banque 4 frame 31 dy 22 dx 8
    10 20 34 20 00 25          ; dessin banque 4 frame 32 dy 52 dx 37
    10 21 43 20 00 3F          ; dessin banque 4 frame 33 dy 67 dx 63
    FF 00                      ; fin d'étape
  ; étape 2
    04 02 E9 01 FF 88          ; dessin banque 1 frame 2 dy -23 dx -120 [corps]
    04 03 DF 01 FF A5          ; dessin banque 1 frame 3 dy -33 dx -91 [corps]
    04 03 DF 01 FF C2          ; dessin banque 1 frame 3 dy -33 dx -62 [corps]
    04 03 DF 00 FF DD          ; dessin banque 1 frame 3 dy -33 dx -35
    04 04 E4 00 FF F3          ; dessin banque 1 frame 4 dy -28 dx -13
    00 0F E1 01 00 0F          ; dessin banque 0 frame 15 dy -31 dx 15 [corps]
    00 0E C6 01 FF E4          ; dessin banque 0 frame 14 dy -58 dx -28 [corps]
    10 1F 16 20 00 08          ; dessin banque 4 frame 31 dy 22 dx 8
    10 20 34 20 00 25          ; dessin banque 4 frame 32 dy 52 dx 37
    10 21 43 20 00 3F          ; dessin banque 4 frame 33 dy 67 dx 63
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 02 E9 01 FF 88          ; dessin banque 1 frame 2 dy -23 dx -120 [corps]
    04 03 DF 01 FF A5          ; dessin banque 1 frame 3 dy -33 dx -91 [corps]
    04 03 DF 01 FF C2          ; dessin banque 1 frame 3 dy -33 dx -62 [corps]
    04 03 DF 00 FF DD          ; dessin banque 1 frame 3 dy -33 dx -35
    04 04 E4 00 FF F3          ; dessin banque 1 frame 4 dy -28 dx -13
    00 0F E1 01 00 0F          ; dessin banque 0 frame 15 dy -31 dx 15 [corps]
    00 0E C6 01 FF E4          ; dessin banque 0 frame 14 dy -58 dx -28 [corps]
    10 22 16 20 00 08          ; dessin banque 4 frame 34 dy 22 dx 8
    10 23 34 20 00 25          ; dessin banque 4 frame 35 dy 52 dx 37
    FF FE                      ; fin d'étape, boucle
  ; étape 4
    88 02                      ; $88 Hold
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    04 03 D5 00 FF F2          ; dessin banque 1 frame 3 dy -43 dx -14
    04 03 D5 00 FF D4          ; dessin banque 1 frame 3 dy -43 dx -44
    04 03 D5 00 FF B6          ; dessin banque 1 frame 3 dy -43 dx -74
    04 03 D5 00 FF 97          ; dessin banque 1 frame 3 dy -43 dx -105
    00 09 D0 00 FF D1          ; dessin banque 0 frame 9 dy -48 dx -47
    04 11 E9 00 00 1A          ; dessin banque 1 frame 17 dy -23 dx 26
    00 0B 08 00 00 00          ; dessin banque 0 frame 11 dy 8 dx 0
    04 12 E8 00 00 41          ; dessin banque 1 frame 18 dy -24 dx 65
    04 13 E6 00 00 55          ; dessin banque 1 frame 19 dy -26 dx 85
    04 03 D5 00 FF 78          ; dessin banque 1 frame 3 dy -43 dx -136
    FF 00                      ; fin d'étape
  ; étape 5
    94 06                      ; $94 Loop
    88 02                      ; $88 Hold
    B0 00 00 00 71 EC          ; $B0 Call -> LAB_02DE
    04 03 D9 00 00 07          ; dessin banque 1 frame 3 dy -39 dx 7
    04 03 D9 00 FF E9          ; dessin banque 1 frame 3 dy -39 dx -23
    04 03 D9 00 FF CB          ; dessin banque 1 frame 3 dy -39 dx -53
    04 03 D9 00 FF AC          ; dessin banque 1 frame 3 dy -39 dx -84
    00 07 E4 00 FF EB          ; dessin banque 0 frame 7 dy -28 dx -21
    00 08 06 00 00 08          ; dessin banque 0 frame 8 dy 6 dx 8
    04 15 F2 00 00 34          ; dessin banque 1 frame 21 dy -14 dx 52
    04 0F EB 00 00 35          ; dessin banque 1 frame 15 dy -21 dx 53
    04 03 D9 00 FF 8D          ; dessin banque 1 frame 3 dy -39 dx -115
    04 03 D9 00 FF 6E          ; dessin banque 1 frame 3 dy -39 dx -146
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 03 D9 01 00 07          ; dessin banque 1 frame 3 dy -39 dx 7 [corps]
    04 03 D9 01 FF E9          ; dessin banque 1 frame 3 dy -39 dx -23 [corps]
    04 03 D9 01 FF CB          ; dessin banque 1 frame 3 dy -39 dx -53 [corps]
    04 03 D9 01 FF AC          ; dessin banque 1 frame 3 dy -39 dx -84 [corps]
    00 07 E4 01 FF EB          ; dessin banque 0 frame 7 dy -28 dx -21 [corps]
    00 08 06 00 00 08          ; dessin banque 0 frame 8 dy 6 dx 8
    04 10 F2 00 00 35          ; dessin banque 1 frame 16 dy -14 dx 53
    04 0F EB 00 00 35          ; dessin banque 1 frame 15 dy -21 dx 53
    04 03 D9 00 FF 8D          ; dessin banque 1 frame 3 dy -39 dx -115
    04 03 D9 00 FF 6E          ; dessin banque 1 frame 3 dy -39 dx -146
    FF FE                      ; fin d'étape, boucle
  ; étape 7
    88 02                      ; $88 Hold
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    04 03 D9 00 00 07          ; dessin banque 1 frame 3 dy -39 dx 7
    04 03 D9 00 FF E9          ; dessin banque 1 frame 3 dy -39 dx -23
    04 03 D9 00 FF CB          ; dessin banque 1 frame 3 dy -39 dx -53
    04 03 D9 00 FF AC          ; dessin banque 1 frame 3 dy -39 dx -84
    00 07 E4 00 FF EB          ; dessin banque 0 frame 7 dy -28 dx -21
    00 08 06 00 00 08          ; dessin banque 0 frame 8 dy 6 dx 8
    04 10 F2 00 00 35          ; dessin banque 1 frame 16 dy -14 dx 53
    04 0E EB 00 00 35          ; dessin banque 1 frame 14 dy -21 dx 53
    04 03 D9 00 FF 8E          ; dessin banque 1 frame 3 dy -39 dx -114
    04 03 D9 00 FF 6F          ; dessin banque 1 frame 3 dy -39 dx -145
    FF FF                      ; fin du script
```

### `LAB_0885` (4:$4DAA)

Rôles : ombre ($AC) depuis LAB_0878 ; ombre ($AC) depuis LAB_0882

```
  ; étape 1
    04 1B 2C 00 FF 9F          ; dessin banque 1 frame 27 dy 44 dx -97
    FF FF                      ; fin du script
```

### `LAB_0886` (4:$4DB2)

Rôles : référencé par le code dans LAB_0297

```
  ; étape 1
    B0 00 00 00 6D 32          ; $B0 Call -> LAB_02AD
    94 02                      ; $94 Loop
    10 01 E9 02 FF EB          ; dessin banque 4 frame 1 dy -23 dx -21 [frappe]
    10 00 CE 00 FF F1          ; dessin banque 4 frame 0 dy -50 dx -15
    FF 00                      ; fin d'étape
  ; étape 2
    10 03 2B 02 FF E8          ; dessin banque 4 frame 3 dy 43 dx -24 [frappe]
    10 02 CA 02 FF ED          ; dessin banque 4 frame 2 dy -54 dx -19 [frappe]
    FF 00                      ; fin d'étape
  ; étape 3
    10 05 10 02 FF E6          ; dessin banque 4 frame 5 dy 16 dx -26 [frappe]
    10 04 DD 02 FF EC          ; dessin banque 4 frame 4 dy -35 dx -20 [frappe]
    FF FE                      ; fin d'étape, boucle
  ; étape 4
    10 06 2B 02 FF E6          ; dessin banque 4 frame 6 dy 43 dx -26 [frappe]
    10 09 FB 02 FF F8          ; dessin banque 4 frame 9 dy -5 dx -8 [frappe]
    10 08 E2 02 FF EF          ; dessin banque 4 frame 8 dy -30 dx -17 [frappe]
    FF 00                      ; fin d'étape
  ; étape 5
    10 09 FB 02 FF F2          ; dessin banque 4 frame 9 dy -5 dx -14 [frappe]
    10 08 E2 02 FF F4          ; dessin banque 4 frame 8 dy -30 dx -12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    10 0B 2E 02 FF EE          ; dessin banque 4 frame 11 dy 46 dx -18 [frappe]
    10 0A DA 02 FF F7          ; dessin banque 4 frame 10 dy -38 dx -9 [frappe]
    FF 00                      ; fin d'étape
  ; étape 7
    10 06 2C 02 FF EE          ; dessin banque 4 frame 6 dy 44 dx -18 [frappe]
    10 08 E2 02 FF F6          ; dessin banque 4 frame 8 dy -30 dx -10 [frappe]
    FF 00                      ; fin d'étape
  ; étape 8
    10 0B 2F 02 FF F0          ; dessin banque 4 frame 11 dy 47 dx -16 [frappe]
    10 09 FC 02 FF F4          ; dessin banque 4 frame 9 dy -4 dx -12 [frappe]
    FF 00                      ; fin d'étape
  ; étape 9
    B0 00 00 00 00 52          ; $B0 Call -> LAB_0A9F
    94 04                      ; $94 Loop
    10 0C FC 00 FF F9          ; dessin banque 4 frame 12 dy -4 dx -7
    FF 00                      ; fin d'étape
  ; étape 10
    10 0D FA 00 FF FC          ; dessin banque 4 frame 13 dy -6 dx -4
    FF 00                      ; fin d'étape
  ; étape 11
    10 0E FA 00 FF F9          ; dessin banque 4 frame 14 dy -6 dx -7
    FF FE                      ; fin d'étape, boucle
  ; étape 12
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0887` (4:$4E54)

Rôles : référencé par le code dans LAB_0298

```
  ; étape 1
    04 0B 06 00 FF E2          ; dessin banque 1 frame 11 dy 6 dx -30
    04 0C 12 00 00 11          ; dessin banque 1 frame 12 dy 18 dx 17
    FF FF                      ; fin du script
```

### `LAB_0888` (4:$4E62)

Rôles : cible de saut depuis LAB_0888 ; cible de saut depuis LAB_0889 ; cible de saut depuis LAB_088C ; cible de saut depuis LAB_088D ; cible de saut depuis LAB_088E ; cible de saut depuis LAB_088F ; cible de saut depuis LAB_0890 ; objet+22 (repos) dans LAB_0198

```
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_0889` (4:$4E90)

Rôles : cible de saut depuis LAB_0888 ; cible de saut depuis LAB_0889 ; cible de saut depuis LAB_088C ; cible de saut depuis LAB_088D ; cible de saut depuis LAB_088E ; cible de saut depuis LAB_088F ; cible de saut depuis LAB_0890

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_088A` (4:$4EBC)

Rôles : référencé par le code dans LAB_02A8 ; référencé par le code dans LAB_02AA

```
  ; étape 1
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_088B` (4:$4ED6)

Rôles : référencé par le code dans LAB_02A9

```
  ; étape 1
    00 19 13 01 FF F1          ; dessin banque 0 frame 25 dy 19 dx -15 [corps]
    00 18 06 03 FF EE          ; dessin banque 0 frame 24 dy 6 dx -18 [corps,frappe]
    00 17 F5 41 FF E8          ; dessin banque 0 frame 23 dy -11 dx -24 [corps,hors-boîte]
    00 16 D0 41 FF E9          ; dessin banque 0 frame 22 dy -48 dx -23 [corps,hors-boîte]
    00 15 C6 00 FF F0          ; dessin banque 0 frame 21 dy -58 dx -16
    FF FF                      ; fin du script
```

### `LAB_088C` (4:$4EF6)

Rôles : ombre ($AC) depuis LAB_0888 ; ombre ($AC) depuis LAB_0889 ; ombre ($AC) depuis LAB_088A ; ombre ($AC) depuis LAB_088C ; ombre ($AC) depuis LAB_088D ; ombre ($AC) depuis LAB_088E ; ombre ($AC) depuis LAB_088F ; ombre ($AC) depuis LAB_0890

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
  ; étape 4
    00 1A 2A 00 FF DD          ; dessin banque 0 frame 26 dy 42 dx -35
    FF 00                      ; fin d'étape
  LAB_088D:
  ; étape 5
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    00 1B ED 01 FF EA          ; dessin banque 0 frame 27 dy -19 dx -22 [corps]
    00 1C 11 00 FF DA          ; dessin banque 0 frame 28 dy 17 dx -38
    FF 00                      ; fin d'étape
  ; étape 6
    A4 2C                      ; $A4 Sound
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1D D6 02 00 31          ; dessin banque 0 frame 29 dy -42 dx 49 [frappe]
    00 1E D6 02 00 29          ; dessin banque 0 frame 30 dy -42 dx 41 [frappe]
    FF 00                      ; fin d'étape
  ; étape 7
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1E D6 02 00 29          ; dessin banque 0 frame 30 dy -42 dx 41 [frappe]
    FF 00                      ; fin d'étape
  LAB_088E:
  ; étape 8
    88 03                      ; $88 Hold
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1E D6 00 00 29          ; dessin banque 0 frame 30 dy -42 dx 41
    FF 00                      ; fin d'étape
  LAB_088F:
  ; étape 9
    88 03                      ; $88 Hold
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF FF                      ; fin du script
```

### `LAB_088D` (4:$4EFE)

Rôles : référencé par le code dans LAB_02A2

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
  ; étape 4
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    00 1B ED 01 FF EA          ; dessin banque 0 frame 27 dy -19 dx -22 [corps]
    00 1C 11 00 FF DA          ; dessin banque 0 frame 28 dy 17 dx -38
    FF 00                      ; fin d'étape
  ; étape 5
    A4 2C                      ; $A4 Sound
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1D D6 02 00 31          ; dessin banque 0 frame 29 dy -42 dx 49 [frappe]
    00 1E D6 02 00 29          ; dessin banque 0 frame 30 dy -42 dx 41 [frappe]
    FF 00                      ; fin d'étape
  ; étape 6
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1E D6 02 00 29          ; dessin banque 0 frame 30 dy -42 dx 41 [frappe]
    FF 00                      ; fin d'étape
  LAB_088E:
  ; étape 7
    88 03                      ; $88 Hold
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1E D6 00 00 29          ; dessin banque 0 frame 30 dy -42 dx 41
    FF 00                      ; fin d'étape
  LAB_088F:
  ; étape 8
    88 03                      ; $88 Hold
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF FF                      ; fin du script
```

### `LAB_088E` (4:$4F4E)

Rôles : référencé par le code dans LAB_02B2

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
  ; étape 4
    88 03                      ; $88 Hold
    00 21 2B 00 FF DB          ; dessin banque 0 frame 33 dy 43 dx -37
    00 20 0A 01 FF E5          ; dessin banque 0 frame 32 dy 10 dx -27 [corps]
    00 1F E8 01 FF DB          ; dessin banque 0 frame 31 dy -24 dx -37 [corps]
    00 1E D6 00 00 29          ; dessin banque 0 frame 30 dy -42 dx 41
    FF 00                      ; fin d'étape
  LAB_088F:
  ; étape 5
    88 03                      ; $88 Hold
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF FF                      ; fin du script
```

### `LAB_088F` (4:$4F6A)

Rôles : cible de saut depuis LAB_0890 ; objet+26 (réaction) dans LAB_0198

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
  ; étape 4
    88 03                      ; $88 Hold
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF FF                      ; fin du script
```

### `LAB_0890` (4:$4F86)

Rôles : référencé par le code dans LAB_02A4

```
  LAB_0888:
  ; étape 1
    B0 00 00 00 50 FC          ; $B0 Call -> LAB_01C8
    C8 01 00 0D 00 00 4E 90    ; $C8 IfFieldZero -> LAB_0889
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 40 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2 [hors-boîte]
    FF FF                      ; fin du script
  LAB_0889:
  ; étape 2
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 01 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40 [corps]
    00 01 EB 01 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35 [corps]
    00 00 F1 41 00 05          ; dessin banque 0 frame 0 dy -15 dx 5 [corps,hors-boîte]
    00 0E 0C 00 FF FE          ; dessin banque 0 frame 14 dy 12 dx -2
    00 0C F3 00 00 0A          ; dessin banque 0 frame 12 dy -13 dx 10
    FF 00                      ; fin d'étape
  LAB_088A:
  ; étape 3
    AC 01 00 00 4E F6          ; $AC Shadow -> LAB_088C
    00 14 2D 00 FF DA          ; dessin banque 0 frame 20 dy 45 dx -38
    00 13 FB 01 FF D5          ; dessin banque 0 frame 19 dy -5 dx -43 [corps]
    00 12 07 41 00 14          ; dessin banque 0 frame 18 dy 7 dx 20 [corps,hors-boîte]
    FF FF                      ; fin du script
  LAB_088F:
  ; étape 4
    88 03                      ; $88 Hold
    84 00 00 00 4E 62          ; $84 Jump -> LAB_0888
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF FF                      ; fin du script
  ; étape 5
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 03                      ; $88 Hold
    00 24 2A 00 FF E0          ; dessin banque 0 frame 36 dy 42 dx -32
    00 23 12 01 FF E7          ; dessin banque 0 frame 35 dy 18 dx -25 [corps]
    00 22 EC 01 FF E4          ; dessin banque 0 frame 34 dy -20 dx -28 [corps]
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    00 06 0E 01 FF DE          ; dessin banque 0 frame 6 dy 14 dx -34 [corps]
    00 0F 02 42 00 1A          ; dessin banque 0 frame 15 dy 2 dx 26 [frappe,hors-boîte]
    00 05 EB 01 FF E5          ; dessin banque 0 frame 5 dy -21 dx -27 [corps]
    00 04 EA 01 00 17          ; dessin banque 0 frame 4 dy -22 dx 23 [corps]
    FF 00                      ; fin d'étape
  ; étape 7
    88 03                      ; $88 Hold
    00 0B 2A 00 FF DD          ; dessin banque 0 frame 11 dy 42 dx -35
    00 0A 15 01 FF E3          ; dessin banque 0 frame 10 dy 21 dx -29 [corps]
    00 08 EA 01 FF FB          ; dessin banque 0 frame 8 dy -22 dx -5 [corps]
    00 07 F7 00 FF D6          ; dessin banque 0 frame 7 dy -9 dx -42
    00 09 F5 01 00 2D          ; dessin banque 0 frame 9 dy -11 dx 45 [corps]
    00 10 08 02 00 28          ; dessin banque 0 frame 16 dy 8 dx 40 [frappe]
    FF 00                      ; fin d'étape
  ; étape 8
    88 03                      ; $88 Hold
    84 00 00 00 4F 6A          ; $84 Jump -> LAB_088F
    00 06 0D 01 FF DA          ; dessin banque 0 frame 6 dy 13 dx -38 [corps]
    00 11 07 00 00 13          ; dessin banque 0 frame 17 dy 7 dx 19
    00 05 EA 01 FF E0          ; dessin banque 0 frame 5 dy -22 dx -32 [corps]
    00 04 E9 41 00 12          ; dessin banque 0 frame 4 dy -23 dx 18 [corps,hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_0891` (4:$5008)

Rôles : référencé par le code dans LAB_02B1

```
  ; étape 1
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    00 28 2A 00 FF E2          ; dessin banque 0 frame 40 dy 42 dx -30
    00 27 0F 00 FF E5          ; dessin banque 0 frame 39 dy 15 dx -27
    00 26 E4 00 FF D1          ; dessin banque 0 frame 38 dy -28 dx -47
    00 25 D0 00 FF F7          ; dessin banque 0 frame 37 dy -48 dx -9
    FF 00                      ; fin d'étape
  ; étape 2
    B4 00 00 00 50 42          ; $B4 IfDead -> LAB_0892
    00 2B 2B 00 FF E3          ; dessin banque 0 frame 43 dy 43 dx -29
    00 2A 15 00 FF EC          ; dessin banque 0 frame 42 dy 21 dx -20
    00 29 E3 00 FF ED          ; dessin banque 0 frame 41 dy -29 dx -19
    FF FF                      ; fin du script
  LAB_0892:
  ; étape 3
    A4 2F                      ; $A4 Sound
    AC 00 00 00 00 00          ; $AC Shadow
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 33 22 80 00 35          ; dessin banque 0 frame 51 dy 34 dx 53
    00 34 1E 80 00 45          ; dessin banque 0 frame 52 dy 30 dx 69
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 35 22 80 00 34          ; dessin banque 0 frame 53 dy 34 dx 52
    00 36 1E 80 00 45          ; dessin banque 0 frame 54 dy 30 dx 69
    FF 00                      ; fin d'étape
  ; étape 6
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 37 1F 80 00 35          ; dessin banque 0 frame 55 dy 31 dx 53
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 37 1F 80 00 35          ; dessin banque 0 frame 55 dy 31 dx 53
    FF 00                      ; fin d'étape
  ; étape 8
    A4 2D                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 39 30 80 00 88          ; dessin banque 0 frame 57 dy 48 dx 136
    00 3A 34 80 00 50          ; dessin banque 0 frame 58 dy 52 dx 80
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3B 31 80 00 86          ; dessin banque 0 frame 59 dy 49 dx 134
    00 3C 34 80 00 4B          ; dessin banque 0 frame 60 dy 52 dx 75
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3D 30 80 00 83          ; dessin banque 0 frame 61 dy 48 dx 131
    00 3E 34 80 00 49          ; dessin banque 0 frame 62 dy 52 dx 73
    FF 00                      ; fin d'étape
  ; étape 11
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3F 36 80 00 80          ; dessin banque 0 frame 63 dy 54 dx 128
    00 40 34 80 00 46          ; dessin banque 0 frame 64 dy 52 dx 70
    00 45 30 80 00 88          ; dessin banque 0 frame 69 dy 48 dx 136
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 41 36 80 00 7C          ; dessin banque 0 frame 65 dy 54 dx 124
    00 42 33 80 00 44          ; dessin banque 0 frame 66 dy 51 dx 68
    00 45 30 80 00 88          ; dessin banque 0 frame 69 dy 48 dx 136
    FF 00                      ; fin d'étape
  ; étape 13
    88 08                      ; $88 Hold
    00 32 27 10 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2 [décor]
    00 31 1E 10 00 37          ; dessin banque 0 frame 49 dy 30 dx 55 [décor]
    00 30 0F 10 00 48          ; dessin banque 0 frame 48 dy 15 dx 72 [décor]
    00 2F 19 10 00 76          ; dessin banque 0 frame 47 dy 25 dx 118 [décor]
    00 43 36 90 00 7A          ; dessin banque 0 frame 67 dy 54 dx 122 [décor]
    00 44 34 90 00 40          ; dessin banque 0 frame 68 dy 52 dx 64 [décor]
    00 45 30 90 00 88          ; dessin banque 0 frame 69 dy 48 dx 136 [décor]
    FF 00                      ; fin d'étape
  ; étape 14
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0892` (4:$5042)

Rôles : cible de saut depuis LAB_0891

```
  ; étape 1
    A4 2F                      ; $A4 Sound
    AC 00 00 00 00 00          ; $AC Shadow
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 33 22 80 00 35          ; dessin banque 0 frame 51 dy 34 dx 53
    00 34 1E 80 00 45          ; dessin banque 0 frame 52 dy 30 dx 69
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 35 22 80 00 34          ; dessin banque 0 frame 53 dy 34 dx 52
    00 36 1E 80 00 45          ; dessin banque 0 frame 54 dy 30 dx 69
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 37 1F 80 00 35          ; dessin banque 0 frame 55 dy 31 dx 53
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    00 2D FC 00 00 03          ; dessin banque 0 frame 45 dy -4 dx 3
    00 2E 22 00 FF F0          ; dessin banque 0 frame 46 dy 34 dx -16
    00 2C 04 00 00 39          ; dessin banque 0 frame 44 dy 4 dx 57
    00 37 1F 80 00 35          ; dessin banque 0 frame 55 dy 31 dx 53
    FF 00                      ; fin d'étape
  ; étape 6
    A4 2D                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 39 30 80 00 88          ; dessin banque 0 frame 57 dy 48 dx 136
    00 3A 34 80 00 50          ; dessin banque 0 frame 58 dy 52 dx 80
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3B 31 80 00 86          ; dessin banque 0 frame 59 dy 49 dx 134
    00 3C 34 80 00 4B          ; dessin banque 0 frame 60 dy 52 dx 75
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3D 30 80 00 83          ; dessin banque 0 frame 61 dy 48 dx 131
    00 3E 34 80 00 49          ; dessin banque 0 frame 62 dy 52 dx 73
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 3F 36 80 00 80          ; dessin banque 0 frame 63 dy 54 dx 128
    00 40 34 80 00 46          ; dessin banque 0 frame 64 dy 52 dx 70
    00 45 30 80 00 88          ; dessin banque 0 frame 69 dy 48 dx 136
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    00 32 27 00 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2
    00 31 1E 00 00 37          ; dessin banque 0 frame 49 dy 30 dx 55
    00 30 0F 00 00 48          ; dessin banque 0 frame 48 dy 15 dx 72
    00 2F 19 00 00 76          ; dessin banque 0 frame 47 dy 25 dx 118
    00 41 36 80 00 7C          ; dessin banque 0 frame 65 dy 54 dx 124
    00 42 33 80 00 44          ; dessin banque 0 frame 66 dy 51 dx 68
    00 45 30 80 00 88          ; dessin banque 0 frame 69 dy 48 dx 136
    FF 00                      ; fin d'étape
  ; étape 11
    88 08                      ; $88 Hold
    00 32 27 10 FF FE          ; dessin banque 0 frame 50 dy 39 dx -2 [décor]
    00 31 1E 10 00 37          ; dessin banque 0 frame 49 dy 30 dx 55 [décor]
    00 30 0F 10 00 48          ; dessin banque 0 frame 48 dy 15 dx 72 [décor]
    00 2F 19 10 00 76          ; dessin banque 0 frame 47 dy 25 dx 118 [décor]
    00 43 36 90 00 7A          ; dessin banque 0 frame 67 dy 54 dx 122 [décor]
    00 44 34 90 00 40          ; dessin banque 0 frame 68 dy 52 dx 64 [décor]
    00 45 30 90 00 88          ; dessin banque 0 frame 69 dy 48 dx 136 [décor]
    FF 00                      ; fin d'étape
  ; étape 12
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_0893` (4:$51EE)

Rôles : référencé par le code dans LAB_02B4

```
  ; étape 1
    A4 08                      ; $A4 Sound
    88 03                      ; $88 Hold
    00 0B 2A 00 FF DD          ; dessin banque 0 frame 11 dy 42 dx -35
    00 0A 15 00 FF E3          ; dessin banque 0 frame 10 dy 21 dx -29
    00 08 EA 00 FF FB          ; dessin banque 0 frame 8 dy -22 dx -5
    00 07 F7 00 FF D6          ; dessin banque 0 frame 7 dy -9 dx -42
    00 09 F5 00 00 2D          ; dessin banque 0 frame 9 dy -11 dx 45
    08 0C 0A 00 00 28          ; dessin banque 2 frame 12 dy 10 dx 40
    08 0D FA 00 00 4C          ; dessin banque 2 frame 13 dy -6 dx 76
    08 0B 15 00 00 54          ; dessin banque 2 frame 11 dy 21 dx 84
    08 0A 2A 00 00 5C          ; dessin banque 2 frame 10 dy 42 dx 92
    08 0F F6 00 00 61          ; dessin banque 2 frame 15 dy -10 dx 97
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 03                      ; $88 Hold
    00 06 0D 00 FF D9          ; dessin banque 0 frame 6 dy 13 dx -39
    08 08 F6 00 00 16          ; dessin banque 2 frame 8 dy -10 dx 22
    00 05 EA 00 FF E0          ; dessin banque 0 frame 5 dy -22 dx -32
    00 04 E9 00 00 12          ; dessin banque 0 frame 4 dy -23 dx 18
    08 09 F2 00 00 37          ; dessin banque 2 frame 9 dy -14 dx 55
    08 07 12 00 00 3A          ; dessin banque 2 frame 7 dy 18 dx 58
    08 10 0C 00 00 4C          ; dessin banque 2 frame 16 dy 12 dx 76
    FF 00                      ; fin d'étape
  ; étape 3
    88 05                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    00 04 E9 00 00 05          ; dessin banque 0 frame 4 dy -23 dx 5
    08 15 06 00 00 11          ; dessin banque 2 frame 21 dy 6 dx 17
    08 16 F1 00 00 2A          ; dessin banque 2 frame 22 dy -15 dx 42
    08 13 11 00 00 2F          ; dessin banque 2 frame 19 dy 17 dx 47
    08 14 17 00 00 3D          ; dessin banque 2 frame 20 dy 23 dx 61
    08 10 09 00 00 36          ; dessin banque 2 frame 16 dy 9 dx 54
    FF FF                      ; fin du script
```

### `LAB_0894` (4:$529E)

Rôles : référencé par le code dans LAB_02B5

```
  ; étape 1
    94 08                      ; $94 Loop
    B0 00 00 00 6D 3C          ; $B0 Call -> LAB_02AE
    88 01                      ; $88 Hold
    00 0B 2A 00 FF DD          ; dessin banque 0 frame 11 dy 42 dx -35
    00 0A 15 00 FF E3          ; dessin banque 0 frame 10 dy 21 dx -29
    00 08 EA 00 FF FB          ; dessin banque 0 frame 8 dy -22 dx -5
    00 07 F7 00 FF D6          ; dessin banque 0 frame 7 dy -9 dx -42
    00 09 F5 00 00 2D          ; dessin banque 0 frame 9 dy -11 dx 45
    08 0C 0A 00 00 28          ; dessin banque 2 frame 12 dy 10 dx 40
    08 12 EE 00 00 46          ; dessin banque 2 frame 18 dy -18 dx 70
    08 11 15 00 00 47          ; dessin banque 2 frame 17 dy 21 dx 71
    08 0F ED 00 00 49          ; dessin banque 2 frame 15 dy -19 dx 73
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    00 06 0D 00 FF D9          ; dessin banque 0 frame 6 dy 13 dx -39
    08 19 01 00 00 11          ; dessin banque 2 frame 25 dy 1 dx 17
    00 05 EA 00 FF E0          ; dessin banque 0 frame 5 dy -22 dx -32
    08 17 F5 00 00 32          ; dessin banque 2 frame 23 dy -11 dx 50
    08 18 ED 00 00 39          ; dessin banque 2 frame 24 dy -19 dx 57
    00 04 E9 00 00 12          ; dessin banque 0 frame 4 dy -23 dx 18
    08 10 04 00 00 4A          ; dessin banque 2 frame 16 dy 4 dx 74
    FF FF                      ; fin du script
```

### `LAB_0895` (4:$530E)

Rôles : référencé par le code dans LAB_02B6

```
  ; étape 1
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    00 04 E9 00 00 05          ; dessin banque 0 frame 4 dy -23 dx 5
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 19 F0 20 00 2A          ; dessin banque 1 frame 25 dy -16 dx 42
    04 1A 10 20 00 2F          ; dessin banque 1 frame 26 dy 16 dx 47
    04 1B 15 20 00 3D          ; dessin banque 1 frame 27 dy 21 dx 61
    08 10 07 00 00 37          ; dessin banque 2 frame 16 dy 7 dx 55
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 03                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 07 EA 20 00 05          ; dessin banque 1 frame 7 dy -22 dx 5
    04 19 F4 20 00 2A          ; dessin banque 1 frame 25 dy -12 dx 42
    04 1A 13 20 00 2F          ; dessin banque 1 frame 26 dy 19 dx 47
    04 1B 19 20 00 3D          ; dessin banque 1 frame 27 dy 25 dx 61
    08 10 0B 00 00 37          ; dessin banque 2 frame 16 dy 11 dx 55
    FF 00                      ; fin d'étape
  ; étape 3
    88 05                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 05 16 20 00 32          ; dessin banque 1 frame 5 dy 22 dx 50
    04 06 EA 20 00 06          ; dessin banque 1 frame 6 dy -22 dx 6
    08 0E EC 00 00 38          ; dessin banque 2 frame 14 dy -20 dx 56
    FF 00                      ; fin d'étape
  ; étape 4
    A4 31                      ; $A4 Sound
    A4 32                      ; $A4 Sound
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 04 ED 20 FF FE          ; dessin banque 1 frame 4 dy -19 dx -2
    04 02 EA 20 00 28          ; dessin banque 1 frame 2 dy -22 dx 40
    04 03 08 20 00 3C          ; dessin banque 1 frame 3 dy 8 dx 60
    04 01 E7 20 00 21          ; dessin banque 1 frame 1 dy -25 dx 33
    04 00 FF 20 00 12          ; dessin banque 1 frame 0 dy -1 dx 18
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 04 ED 20 FF FE          ; dessin banque 1 frame 4 dy -19 dx -2
    04 02 EA 20 00 28          ; dessin banque 1 frame 2 dy -22 dx 40
    04 03 08 20 00 3C          ; dessin banque 1 frame 3 dy 8 dx 60
    04 0E DF 20 00 35          ; dessin banque 1 frame 14 dy -33 dx 53
    04 0D EC 20 00 19          ; dessin banque 1 frame 13 dy -20 dx 25
    04 0C 00 20 00 12          ; dessin banque 1 frame 12 dy 0 dx 18
    04 0B 0F 20 00 1C          ; dessin banque 1 frame 11 dy 15 dx 28
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 04 ED 20 FF FE          ; dessin banque 1 frame 4 dy -19 dx -2
    04 02 EA 20 00 28          ; dessin banque 1 frame 2 dy -22 dx 40
    04 03 08 20 00 3C          ; dessin banque 1 frame 3 dy 8 dx 60
    04 08 FC 20 00 41          ; dessin banque 1 frame 8 dy -4 dx 65
    04 09 E1 20 00 09          ; dessin banque 1 frame 9 dy -31 dx 9
    04 0A DB 20 00 48          ; dessin banque 1 frame 10 dy -37 dx 72
    04 16 FC 20 00 12          ; dessin banque 1 frame 22 dy -4 dx 18
    04 15 23 20 00 41          ; dessin banque 1 frame 21 dy 35 dx 65
    04 14 1D 20 00 12          ; dessin banque 1 frame 20 dy 29 dx 18
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 1D ED 20 00 04          ; dessin banque 1 frame 29 dy -19 dx 4
    04 13 F7 20 00 2A          ; dessin banque 1 frame 19 dy -9 dx 42
    04 12 F6 20 00 13          ; dessin banque 1 frame 18 dy -10 dx 19
    04 11 F7 20 00 2D          ; dessin banque 1 frame 17 dy -9 dx 45
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    94 08                      ; $94 Loop
    A4 31                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 1C EF 20 00 04          ; dessin banque 1 frame 28 dy -17 dx 4
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 13 F7 20 00 2A          ; dessin banque 1 frame 19 dy -9 dx 42
    04 10 EB 20 00 30          ; dessin banque 1 frame 16 dy -21 dx 48
    FF 00                      ; fin d'étape
  ; étape 9
    A4 32                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 1D EE 20 00 04          ; dessin banque 1 frame 29 dy -18 dx 4
    04 13 F7 20 00 2A          ; dessin banque 1 frame 19 dy -9 dx 42
    04 0F E7 20 00 2F          ; dessin banque 1 frame 15 dy -25 dx 47
    FF 00                      ; fin d'étape
  ; étape 10
    A4 33                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 1C EF 20 00 04          ; dessin banque 1 frame 28 dy -17 dx 4
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 13 F7 20 00 2A          ; dessin banque 1 frame 19 dy -9 dx 42
    04 17 F0 20 00 2C          ; dessin banque 1 frame 23 dy -16 dx 44
    FF 00                      ; fin d'étape
  ; étape 11
    A4 31                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D8          ; dessin banque 0 frame 3 dy 43 dx -40
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DD          ; dessin banque 0 frame 1 dy -21 dx -35
    04 18 06 20 00 11          ; dessin banque 1 frame 24 dy 6 dx 17
    04 1D EF 20 00 04          ; dessin banque 1 frame 29 dy -17 dx 4
    04 13 F7 20 00 2A          ; dessin banque 1 frame 19 dy -9 dx 42
    04 11 F9 20 00 2E          ; dessin banque 1 frame 17 dy -7 dx 46
    FF FF                      ; fin du script
```

### `LAB_0896` (4:$55D6)

Rôles : référencé par le code dans LAB_02B7

```
  ; étape 1
    A4 2B                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 1F F2 20 00 29          ; dessin banque 1 frame 31 dy -14 dx 41
    04 20 13 20 00 34          ; dessin banque 1 frame 32 dy 19 dx 52
    04 1E D2 20 00 35          ; dessin banque 1 frame 30 dy -46 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 11 F0 00 00 36          ; dessin banque 1 frame 17 dy -16 dx 54
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 2C F3 20 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 20 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    04 27 B5 20 00 38          ; dessin banque 1 frame 39 dy -75 dx 56
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 10 E3 20 00 38          ; dessin banque 1 frame 16 dy -29 dx 56
    04 2A D2 20 00 43          ; dessin banque 1 frame 42 dy -46 dx 67
    04 29 C6 20 00 38          ; dessin banque 1 frame 41 dy -58 dx 56
    04 28 CD 20 00 29          ; dessin banque 1 frame 40 dy -51 dx 41
    04 25 CB 20 00 40          ; dessin banque 1 frame 37 dy -53 dx 64
    04 25 11 20 00 65          ; dessin banque 1 frame 37 dy 17 dx 101
    04 24 0D 20 00 46          ; dessin banque 1 frame 36 dy 13 dx 70
    04 23 0B 20 00 2A          ; dessin banque 1 frame 35 dy 11 dx 42
    04 22 FF 20 00 4C          ; dessin banque 1 frame 34 dy -1 dx 76
    04 25 09 20 00 57          ; dessin banque 1 frame 37 dy 9 dx 87
    04 11 EF 00 00 35          ; dessin banque 1 frame 17 dy -17 dx 53
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 72 C0          ; $B0 Call -> LAB_02E8
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    04 07 EB 00 00 08          ; dessin banque 1 frame 7 dy -21 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 2D 0E 20 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    04 2C F3 20 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2E 06 20 00 3A          ; dessin banque 1 frame 46 dy 6 dx 58
    04 11 EF 00 00 35          ; dessin banque 1 frame 17 dy -17 dx 53
    04 2F 0E 20 00 4C          ; dessin banque 1 frame 47 dy 14 dx 76
    04 30 1C 20 00 64          ; dessin banque 1 frame 48 dy 28 dx 100
    04 31 1D 20 00 15          ; dessin banque 1 frame 49 dy 29 dx 21
    04 32 12 20 00 26          ; dessin banque 1 frame 50 dy 18 dx 38
    04 33 AC 20 00 3F          ; dessin banque 1 frame 51 dy -84 dx 63
    04 34 B4 20 00 3D          ; dessin banque 1 frame 52 dy -76 dx 61
    04 17 E6 00 00 36          ; dessin banque 1 frame 23 dy -26 dx 54
    04 2A D2 20 00 43          ; dessin banque 1 frame 42 dy -46 dx 67
    04 28 CE 20 00 1E          ; dessin banque 1 frame 40 dy -50 dx 30
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    04 07 EB 00 00 08          ; dessin banque 1 frame 7 dy -21 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 2D 0E 20 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    04 2C F3 20 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 25 22 20 00 6A          ; dessin banque 1 frame 37 dy 34 dx 106
    04 32 AB 00 00 45          ; dessin banque 1 frame 50 dy -85 dx 69
    04 26 24 20 00 54          ; dessin banque 1 frame 38 dy 36 dx 84
    04 28 18 20 00 5C          ; dessin banque 1 frame 40 dy 24 dx 92
    04 2A 21 20 00 1B          ; dessin banque 1 frame 42 dy 33 dx 27
    04 2A 1A 20 00 21          ; dessin banque 1 frame 42 dy 26 dx 33
    04 2A D1 20 00 13          ; dessin banque 1 frame 42 dy -47 dx 19
    04 35 06 20 00 3A          ; dessin banque 1 frame 53 dy 6 dx 58
    04 34 CC 20 00 3B          ; dessin banque 1 frame 52 dy -52 dx 59
    04 17 E6 20 00 36          ; dessin banque 1 frame 23 dy -26 dx 54
    04 3A 96 20 00 4F          ; dessin banque 1 frame 58 dy -106 dx 79
    04 32 B9 00 00 3D          ; dessin banque 1 frame 50 dy -71 dx 61
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 3A A1 20 00 70          ; dessin banque 1 frame 58 dy -95 dx 112
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 35 06 00 00 3A          ; dessin banque 1 frame 53 dy 6 dx 58
    04 10 E1 00 00 38          ; dessin banque 1 frame 16 dy -31 dx 56
    04 14 AD 00 00 41          ; dessin banque 1 frame 20 dy -83 dx 65
    04 32 B0 00 00 46          ; dessin banque 1 frame 50 dy -80 dx 70
    04 32 AA 00 00 4F          ; dessin banque 1 frame 50 dy -86 dx 79
    04 34 A7 00 00 54          ; dessin banque 1 frame 52 dy -89 dx 84
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 38 B8 20 00 81          ; dessin banque 1 frame 56 dy -72 dx 129
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 35 06 00 00 3A          ; dessin banque 1 frame 53 dy 6 dx 58
    04 11 F0 00 00 36          ; dessin banque 1 frame 17 dy -16 dx 54
    04 0F DC 00 00 38          ; dessin banque 1 frame 15 dy -36 dx 56
    04 08 AE 00 00 72          ; dessin banque 1 frame 8 dy -82 dx 114
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 38 E9 20 00 87          ; dessin banque 1 frame 56 dy -23 dx 135
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 35 06 00 00 3A          ; dessin banque 1 frame 53 dy 6 dx 58
    04 11 F0 00 00 36          ; dessin banque 1 frame 17 dy -16 dx 54
    04 17 E7 00 00 35          ; dessin banque 1 frame 23 dy -25 dx 53
    04 09 DB 00 00 7D          ; dessin banque 1 frame 9 dy -37 dx 125
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    A4 12                      ; $A4 Sound
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 38 1D 20 00 89          ; dessin banque 1 frame 56 dy 29 dx 137
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 11 EF 00 00 35          ; dessin banque 1 frame 17 dy -17 dx 53
    04 0D 1C 00 00 7C          ; dessin banque 1 frame 13 dy 28 dx 124
    04 36 0E 00 00 86          ; dessin banque 1 frame 54 dy 14 dx 134
    FF 00                      ; fin d'étape
  ; étape 9
    94 1E                      ; $94 Loop
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 3B 2D 20 00 85          ; dessin banque 1 frame 59 dy 45 dx 133
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 10 E2 00 00 37          ; dessin banque 1 frame 16 dy -30 dx 55
    04 0A 17 00 00 5B          ; dessin banque 1 frame 10 dy 23 dx 91
    04 08 20 00 00 71          ; dessin banque 1 frame 8 dy 32 dx 113
    FF 00                      ; fin d'étape
  ; étape 10
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 3B 2D 20 00 85          ; dessin banque 1 frame 59 dy 45 dx 133
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 0F DD 00 00 38          ; dessin banque 1 frame 15 dy -35 dx 56
    04 31 22 00 00 40          ; dessin banque 1 frame 49 dy 34 dx 64
    FF 00                      ; fin d'étape
  ; étape 11
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 3B 2D 20 00 85          ; dessin banque 1 frame 59 dy 45 dx 133
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 17 E6 00 00 36          ; dessin banque 1 frame 23 dy -26 dx 54
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    00 03 2B 00 FF D9          ; dessin banque 0 frame 3 dy 43 dx -39
    00 04 EA 00 00 08          ; dessin banque 0 frame 4 dy -22 dx 8
    00 02 0D 00 FF D8          ; dessin banque 0 frame 2 dy 13 dx -40
    00 01 EB 00 FF DF          ; dessin banque 0 frame 1 dy -21 dx -33
    04 3B 2D 20 00 85          ; dessin banque 1 frame 59 dy 45 dx 133
    04 2C F3 00 00 2D          ; dessin banque 1 frame 44 dy -13 dx 45
    04 2D 0E 00 00 35          ; dessin banque 1 frame 45 dy 14 dx 53
    08 08 F7 00 00 0C          ; dessin banque 2 frame 8 dy -9 dx 12
    04 11 EF 00 00 35          ; dessin banque 1 frame 17 dy -17 dx 53
    FF FF                      ; fin du script
```

### `LAB_0897` (4:$59A8)

Rôles : référencé par le code dans LAB_0EA7

```
  ; étape 1
    B0 00 00 00 04 28          ; $B0 Call -> LAB_0EB9
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    00 06 2F 40 00 12          ; dessin banque 0 frame 6 dy 47 dx 18 [hors-boîte]
    00 05 27 00 00 02          ; dessin banque 0 frame 5 dy 39 dx 2
    00 04 23 00 FF DC          ; dessin banque 0 frame 4 dy 35 dx -36
    FF 00                      ; fin d'étape
  ; étape 2
    00 02 FC 00 FF DF          ; dessin banque 0 frame 2 dy -4 dx -33
    00 00 30 40 00 25          ; dessin banque 0 frame 0 dy 48 dx 37 [hors-boîte]
    00 01 2A 40 FF D3          ; dessin banque 0 frame 1 dy 42 dx -45 [hors-boîte]
    00 03 EB 00 FF E3          ; dessin banque 0 frame 3 dy -21 dx -29
    FF 00                      ; fin d'étape
  ; étape 3
    00 0B E0 01 FF D9          ; dessin banque 0 frame 11 dy -32 dx -39 [corps]
    00 0A D6 00 FF F6          ; dessin banque 0 frame 10 dy -42 dx -10
    00 10 07 41 FF D9          ; dessin banque 0 frame 16 dy 7 dx -39 [corps,hors-boîte]
    00 08 1C 40 00 25          ; dessin banque 0 frame 8 dy 28 dx 37 [hors-boîte]
    00 07 32 40 00 25          ; dessin banque 0 frame 7 dy 50 dx 37 [hors-boîte]
    FF 00                      ; fin d'étape
  LAB_0898:
  ; étape 4
    88 04                      ; $88 Hold
    00 0D ED 01 FF DE          ; dessin banque 0 frame 13 dy -19 dx -34 [corps]
    00 0E DE 01 FF E0          ; dessin banque 0 frame 14 dy -34 dx -32 [corps]
    00 0F D0 00 FF E0          ; dessin banque 0 frame 15 dy -48 dx -32
    00 0C 11 01 FF EB          ; dessin banque 0 frame 12 dy 17 dx -21 [corps]
    00 1A 29 41 FF D8          ; dessin banque 0 frame 26 dy 41 dx -40 [corps,hors-boîte]
    00 19 2D 40 00 20          ; dessin banque 0 frame 25 dy 45 dx 32 [hors-boîte]
    00 18 31 40 FF CA          ; dessin banque 0 frame 24 dy 49 dx -54 [hors-boîte]
    00 14 04 40 00 1A          ; dessin banque 0 frame 20 dy 4 dx 26 [hors-boîte]
    00 13 D6 40 00 17          ; dessin banque 0 frame 19 dy -42 dx 23 [hors-boîte]
    00 12 05 40 FF CF          ; dessin banque 0 frame 18 dy 5 dx -49 [hors-boîte]
    00 11 19 40 FF DA          ; dessin banque 0 frame 17 dy 25 dx -38 [hors-boîte]
    00 11 CC 40 FF D5          ; dessin banque 0 frame 17 dy -52 dx -43 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_0898` (4:$5A02)

Rôles : référencé par le code dans LAB_0EA9

```
  ; étape 1
    88 04                      ; $88 Hold
    00 0D ED 01 FF DE          ; dessin banque 0 frame 13 dy -19 dx -34 [corps]
    00 0E DE 01 FF E0          ; dessin banque 0 frame 14 dy -34 dx -32 [corps]
    00 0F D0 00 FF E0          ; dessin banque 0 frame 15 dy -48 dx -32
    00 0C 11 01 FF EB          ; dessin banque 0 frame 12 dy 17 dx -21 [corps]
    00 1A 29 41 FF D8          ; dessin banque 0 frame 26 dy 41 dx -40 [corps,hors-boîte]
    00 19 2D 40 00 20          ; dessin banque 0 frame 25 dy 45 dx 32 [hors-boîte]
    00 18 31 40 FF CA          ; dessin banque 0 frame 24 dy 49 dx -54 [hors-boîte]
    00 14 04 40 00 1A          ; dessin banque 0 frame 20 dy 4 dx 26 [hors-boîte]
    00 13 D6 40 00 17          ; dessin banque 0 frame 19 dy -42 dx 23 [hors-boîte]
    00 12 05 40 FF CF          ; dessin banque 0 frame 18 dy 5 dx -49 [hors-boîte]
    00 11 19 40 FF DA          ; dessin banque 0 frame 17 dy 25 dx -38 [hors-boîte]
    00 11 CC 40 FF D5          ; dessin banque 0 frame 17 dy -52 dx -43 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_0899` (4:$5A4E)

Rôles : référencé par le code dans LAB_0EAB

```
  ; étape 1
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    88 01                      ; $88 Hold
    00 17 EC 01 FF DB          ; dessin banque 0 frame 23 dy -20 dx -37 [corps]
    00 16 12 01 FF E8          ; dessin banque 0 frame 22 dy 18 dx -24 [corps]
    00 15 28 41 FF E2          ; dessin banque 0 frame 21 dy 40 dx -30 [corps,hors-boîte]
    00 1B 30 40 FF D1          ; dessin banque 0 frame 27 dy 48 dx -47 [hors-boîte]
    FF 00                      ; fin d'étape
  LAB_089A:
  ; étape 2
    B0 00 00 00 04 1C          ; $B0 Call -> LAB_0EB8
    00 1E F6 01 FF E4          ; dessin banque 0 frame 30 dy -10 dx -28 [corps]
    00 1D 33 40 00 24          ; dessin banque 0 frame 29 dy 51 dx 36 [hors-boîte]
    00 1C 32 40 FF D5          ; dessin banque 0 frame 28 dy 50 dx -43 [hors-boîte]
    00 21 2D 41 FF E1          ; dessin banque 0 frame 33 dy 45 dx -31 [corps,hors-boîte]
    00 20 37 40 FF DF          ; dessin banque 0 frame 32 dy 55 dx -33 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089A` (4:$5A70)

Rôles : objet+22 (repos) dans LAB_019D ; objet+26 (réaction) dans LAB_019D

```
  ; étape 1
    B0 00 00 00 04 1C          ; $B0 Call -> LAB_0EB8
    00 1E F6 01 FF E4          ; dessin banque 0 frame 30 dy -10 dx -28 [corps]
    00 1D 33 40 00 24          ; dessin banque 0 frame 29 dy 51 dx 36 [hors-boîte]
    00 1C 32 40 FF D5          ; dessin banque 0 frame 28 dy 50 dx -43 [hors-boîte]
    00 21 2D 41 FF E1          ; dessin banque 0 frame 33 dy 45 dx -31 [corps,hors-boîte]
    00 20 37 40 FF DF          ; dessin banque 0 frame 32 dy 55 dx -33 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089B` (4:$5A96)

Rôles : référencé par le code dans LAB_015D

```
  ; étape 1
    88 02                      ; $88 Hold
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 40 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089C` (4:$5AAC)

Rôles : référencé par le code dans LAB_015D

```
  ; étape 1
    88 02                      ; $88 Hold
    00 1E F6 01 FF E4          ; dessin banque 0 frame 30 dy -10 dx -28 [corps]
    00 1D 33 40 00 24          ; dessin banque 0 frame 29 dy 51 dx 36 [hors-boîte]
    00 1C 32 40 FF D5          ; dessin banque 0 frame 28 dy 50 dx -43 [hors-boîte]
    00 21 2D 41 FF E1          ; dessin banque 0 frame 33 dy 45 dx -31 [corps,hors-boîte]
    00 20 37 40 FF DF          ; dessin banque 0 frame 32 dy 55 dx -33 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089D` (4:$5ACE)

Rôles : référencé par le code dans LAB_015D

```
  ; étape 1
    88 02                      ; $88 Hold
    00 29 F6 01 FF E2          ; dessin banque 0 frame 41 dy -10 dx -30 [corps]
    00 28 2C 41 FF D0          ; dessin banque 0 frame 40 dy 44 dx -48 [corps,hors-boîte]
    00 27 38 40 FF F8          ; dessin banque 0 frame 39 dy 56 dx -8 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089E` (4:$5AE4)

Rôles : référencé par le code dans LAB_0EA5

```
  ; étape 1
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    A4 56                      ; $A4 Sound
    88 01                      ; $88 Hold
    00 26 14 00 00 0A          ; dessin banque 0 frame 38 dy 20 dx 10
    00 1E F6 01 FF E4          ; dessin banque 0 frame 30 dy -10 dx -28 [corps]
    00 1D 33 40 00 25          ; dessin banque 0 frame 29 dy 51 dx 37 [hors-boîte]
    00 1C 32 40 FF D5          ; dessin banque 0 frame 28 dy 50 dx -43 [hors-boîte]
    00 21 2D 41 FF E1          ; dessin banque 0 frame 33 dy 45 dx -31 [corps,hors-boîte]
    00 20 37 40 FF DF          ; dessin banque 0 frame 32 dy 55 dx -33 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 2
    88 01                      ; $88 Hold
    00 25 ED 00 00 27          ; dessin banque 0 frame 37 dy -19 dx 39
    00 24 0A 01 00 0E          ; dessin banque 0 frame 36 dy 10 dx 14 [corps]
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 00 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    FF 00                      ; fin d'étape
  ; étape 3
    00 2C EF 42 00 31          ; dessin banque 0 frame 44 dy -17 dx 49 [frappe,hors-boîte]
    00 2B 06 01 00 11          ; dessin banque 0 frame 43 dy 6 dx 17 [corps]
    00 2A 08 00 00 03          ; dessin banque 0 frame 42 dy 8 dx 3
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 00 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    FF 00                      ; fin d'étape
  ; étape 4
    00 33 F5 42 00 47          ; dessin banque 0 frame 51 dy -11 dx 71 [frappe,hors-boîte]
    00 32 F8 40 00 3D          ; dessin banque 0 frame 50 dy -8 dx 61 [hors-boîte]
    00 31 FF 01 00 21          ; dessin banque 0 frame 49 dy -1 dx 33 [corps]
    00 30 01 01 00 03          ; dessin banque 0 frame 48 dy 1 dx 3 [corps]
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 00 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    00 20 37 00 FF D2          ; dessin banque 0 frame 32 dy 55 dx -46
    FF 00                      ; fin d'étape
  ; étape 5
    88 01                      ; $88 Hold
    00 2C EF 42 00 31          ; dessin banque 0 frame 44 dy -17 dx 49 [frappe,hors-boîte]
    00 2B 06 41 00 11          ; dessin banque 0 frame 43 dy 6 dx 17 [corps,hors-boîte]
    00 2A 08 00 00 03          ; dessin banque 0 frame 42 dy 8 dx 3
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 00 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    00 20 37 00 FF D2          ; dessin banque 0 frame 32 dy 55 dx -46
    FF 00                      ; fin d'étape
  ; étape 6
    88 01                      ; $88 Hold
    00 25 ED 00 00 27          ; dessin banque 0 frame 37 dy -19 dx 39
    00 24 0A 01 00 0E          ; dessin banque 0 frame 36 dy 10 dx 14 [corps]
    00 23 2D 40 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37 [hors-boîte]
    00 22 39 00 FF DB          ; dessin banque 0 frame 34 dy 57 dx -37
    00 1F F6 01 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28 [corps]
    00 20 37 00 FF D2          ; dessin banque 0 frame 32 dy 55 dx -46
    FF 00                      ; fin d'étape
  ; étape 7
    88 01                      ; $88 Hold
    00 26 14 00 00 0A          ; dessin banque 0 frame 38 dy 20 dx 10
    00 1E F6 01 FF E4          ; dessin banque 0 frame 30 dy -10 dx -28 [corps]
    00 1D 33 40 00 24          ; dessin banque 0 frame 29 dy 51 dx 36 [hors-boîte]
    00 1C 32 40 FF D5          ; dessin banque 0 frame 28 dy 50 dx -43 [hors-boîte]
    00 21 2D 41 FF E1          ; dessin banque 0 frame 33 dy 45 dx -31 [corps,hors-boîte]
    00 20 37 40 FF DF          ; dessin banque 0 frame 32 dy 55 dx -33 [hors-boîte]
    FF FF                      ; fin du script
```

### `LAB_089F` (4:$5C0C)

Rôles : référencé par le code dans LAB_0EAE ; référencé par le code dans LAB_0EB4

```
  ; étape 1
    B0 00 00 00 04 BC          ; $B0 Call -> LAB_0EBE
    00 34 F6 00 00 3C          ; dessin banque 0 frame 52 dy -10 dx 60
    04 11 33 20 00 44          ; dessin banque 1 frame 17 dy 51 dx 68
    00 36 E7 00 00 6D          ; dessin banque 0 frame 54 dy -25 dx 109
    04 11 33 20 00 63          ; dessin banque 1 frame 17 dy 51 dx 99
    00 35 FA 00 00 53          ; dessin banque 0 frame 53 dy -6 dx 83
    00 31 FF 00 00 16          ; dessin banque 0 frame 49 dy -1 dx 22
    00 32 F8 00 00 32          ; dessin banque 0 frame 50 dy -8 dx 50
    00 30 01 00 FF F8          ; dessin banque 0 frame 48 dy 1 dx -8
    00 23 2D 00 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37
    00 1F F6 00 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28
    00 20 38 00 FF D2          ; dessin banque 0 frame 32 dy 56 dx -46
    FF FF                      ; fin du script
```

### `LAB_08A0` (4:$5C56)

Rôles : référencé par le code dans LAB_0EAC

```
  ; étape 1
    A4 0C                      ; $A4 Sound
    A4 04                      ; $A4 Sound
    00 37 F8 00 00 3C          ; dessin banque 0 frame 55 dy -8 dx 60
    04 11 31 20 00 40          ; dessin banque 1 frame 17 dy 49 dx 64
    00 39 F5 00 00 76          ; dessin banque 0 frame 57 dy -11 dx 118
    00 31 FF 00 00 16          ; dessin banque 0 frame 49 dy -1 dx 22
    00 30 01 00 FF F8          ; dessin banque 0 frame 48 dy 1 dx -8
    00 32 F8 00 00 32          ; dessin banque 0 frame 50 dy -8 dx 50
    04 11 33 20 00 61          ; dessin banque 1 frame 17 dy 51 dx 97
    00 38 F3 00 00 4A          ; dessin banque 0 frame 56 dy -13 dx 74
    00 3A 18 00 00 76          ; dessin banque 0 frame 58 dy 24 dx 118
    00 1F F6 00 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28
    00 23 2D 00 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37
    00 20 38 00 FF D2          ; dessin banque 0 frame 32 dy 56 dx -46
    FF 00                      ; fin d'étape
  ; étape 2
    00 30 01 00 00 01          ; dessin banque 0 frame 48 dy 1 dx 1
    00 3B E5 00 00 3F          ; dessin banque 0 frame 59 dy -27 dx 63
    00 3C D8 00 00 4D          ; dessin banque 0 frame 60 dy -40 dx 77
    04 10 2D 20 00 42          ; dessin banque 1 frame 16 dy 45 dx 66
    00 3D E9 00 00 46          ; dessin banque 0 frame 61 dy -23 dx 70
    04 30 2B 20 FF D8          ; dessin banque 1 frame 48 dy 43 dx -40
    04 31 20 20 FF ED          ; dessin banque 1 frame 49 dy 32 dx -19
    04 32 07 20 FF E7          ; dessin banque 1 frame 50 dy 7 dx -25
    00 32 F9 00 00 3B          ; dessin banque 0 frame 50 dy -7 dx 59
    04 11 33 20 00 61          ; dessin banque 1 frame 17 dy 51 dx 97
    00 31 FF 00 00 1F          ; dessin banque 0 frame 49 dy -1 dx 31
    04 33 F1 20 FF E7          ; dessin banque 1 frame 51 dy -15 dx -25
    00 3E 27 00 00 5C          ; dessin banque 0 frame 62 dy 39 dx 92
    FF 00                      ; fin d'étape
  ; étape 3
    04 30 2B 20 FF D8          ; dessin banque 1 frame 48 dy 43 dx -40
    04 31 20 20 FF ED          ; dessin banque 1 frame 49 dy 32 dx -19
    04 32 07 20 FF E7          ; dessin banque 1 frame 50 dy 7 dx -25
    00 24 FF 00 00 19          ; dessin banque 0 frame 36 dy -1 dx 25
    04 33 F1 20 FF E7          ; dessin banque 1 frame 51 dy -15 dx -25
    04 00 E4 20 00 32          ; dessin banque 1 frame 0 dy -28 dx 50
    04 01 EA 20 00 51          ; dessin banque 1 frame 1 dy -22 dx 81
    04 10 2D 20 00 42          ; dessin banque 1 frame 16 dy 45 dx 66
    04 11 33 20 00 61          ; dessin banque 1 frame 17 dy 51 dx 97
    04 03 15 20 00 5B          ; dessin banque 1 frame 3 dy 21 dx 91
    FF FF                      ; fin du script
```

### `LAB_08A1` (4:$5D72)

Rôles : référencé par le code dans LAB_0EB0

```
  ; étape 1
    A4 5A                      ; $A4 Sound
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    88 03                      ; $88 Hold
    04 06 F6 20 00 3D          ; dessin banque 1 frame 6 dy -10 dx 61
    04 07 15 20 00 49          ; dessin banque 1 frame 7 dy 21 dx 73
    04 10 2E 20 00 42          ; dessin banque 1 frame 16 dy 46 dx 66
    04 09 27 20 00 7F          ; dessin banque 1 frame 9 dy 39 dx 127
    04 11 33 20 00 63          ; dessin banque 1 frame 17 dy 51 dx 99
    04 08 FE 20 00 57          ; dessin banque 1 frame 8 dy -2 dx 87
    00 32 F9 00 00 33          ; dessin banque 0 frame 50 dy -7 dx 51
    00 31 00 00 00 17          ; dessin banque 0 frame 49 dy 0 dx 23
    00 30 02 00 FF F9          ; dessin banque 0 frame 48 dy 2 dx -7
    00 1F F6 00 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28
    00 23 2D 00 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37
    00 20 38 00 FF D2          ; dessin banque 0 frame 32 dy 56 dx -46
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    04 0A FA 20 00 3D          ; dessin banque 1 frame 10 dy -6 dx 61
    04 0B FA 20 00 4A          ; dessin banque 1 frame 11 dy -6 dx 74
    04 10 2E 20 00 3F          ; dessin banque 1 frame 16 dy 46 dx 63
    04 09 27 20 00 7F          ; dessin banque 1 frame 9 dy 39 dx 127
    04 11 33 20 00 63          ; dessin banque 1 frame 17 dy 51 dx 99
    00 32 FC 00 00 33          ; dessin banque 0 frame 50 dy -4 dx 51
    00 31 03 00 00 17          ; dessin banque 0 frame 49 dy 3 dx 23
    00 30 05 00 FF F9          ; dessin banque 0 frame 48 dy 5 dx -7
    00 1F F6 00 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28
    00 23 2D 00 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37
    00 20 38 00 FF D2          ; dessin banque 0 frame 32 dy 56 dx -46
    04 0C 13 20 00 4A          ; dessin banque 1 frame 12 dy 19 dx 74
    00 14 1B 00 00 70          ; dessin banque 0 frame 20 dy 27 dx 112
    00 12 1D 00 00 53          ; dessin banque 0 frame 18 dy 29 dx 83
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    88 50                      ; $88 Hold
    04 0A FA 20 00 3D          ; dessin banque 1 frame 10 dy -6 dx 61
    04 0B FA 20 00 4A          ; dessin banque 1 frame 11 dy -6 dx 74
    04 10 2E 20 00 3F          ; dessin banque 1 frame 16 dy 46 dx 63
    04 09 27 20 00 7F          ; dessin banque 1 frame 9 dy 39 dx 127
    04 11 33 20 00 63          ; dessin banque 1 frame 17 dy 51 dx 99
    00 32 FC 00 00 33          ; dessin banque 0 frame 50 dy -4 dx 51
    00 31 03 00 00 17          ; dessin banque 0 frame 49 dy 3 dx 23
    00 30 05 00 FF F9          ; dessin banque 0 frame 48 dy 5 dx -7
    00 1F F6 00 FF E4          ; dessin banque 0 frame 31 dy -10 dx -28
    00 23 2D 00 FF DB          ; dessin banque 0 frame 35 dy 45 dx -37
    00 20 38 00 FF D2          ; dessin banque 0 frame 32 dy 56 dx -46
    04 0C 13 20 00 4A          ; dessin banque 1 frame 12 dy 19 dx 74
    FF FF                      ; fin du script
```

### `LAB_08A2` (4:$5E76)

Rôles : référencé par le code dans LAB_0EAA

```
  ; étape 1
    B0 00 00 00 04 28          ; $B0 Call -> LAB_0EB9
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    88 03                      ; $88 Hold
    00 2F FB 00 00 45          ; dessin banque 0 frame 47 dy -5 dx 69
    00 2E 33 00 00 33          ; dessin banque 0 frame 46 dy 51 dx 51
    04 1B EF 20 FF F4          ; dessin banque 1 frame 27 dy -17 dx -12
    04 1A 15 20 FF EA          ; dessin banque 1 frame 26 dy 21 dx -22
    04 19 2B 20 FF D1          ; dessin banque 1 frame 25 dy 43 dx -47
    04 1D CB 20 FF F5          ; dessin banque 1 frame 29 dy -53 dx -11
    04 1C E1 20 FF EC          ; dessin banque 1 frame 28 dy -31 dx -20
    04 3C 32 20 00 32          ; dessin banque 1 frame 60 dy 50 dx 50
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    04 21 E4 20 00 29          ; dessin banque 1 frame 33 dy -28 dx 41
    04 22 2A 20 FF D6          ; dessin banque 1 frame 34 dy 42 dx -42
    04 23 FF 20 FF FF          ; dessin banque 1 frame 35 dy -1 dx -1
    04 24 FA 20 00 28          ; dessin banque 1 frame 36 dy -6 dx 40
    04 25 FD 20 00 4D          ; dessin banque 1 frame 37 dy -3 dx 77
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    B0 00 00 00 03 F4          ; $B0 Call -> LAB_000D
    04 2D 0E 20 00 22          ; dessin banque 1 frame 45 dy 14 dx 34
    04 2C 11 20 00 07          ; dessin banque 1 frame 44 dy 17 dx 7
    04 2B 30 20 FF E3          ; dessin banque 1 frame 43 dy 48 dx -29
    04 2E 12 20 00 39          ; dessin banque 1 frame 46 dy 18 dx 57
    04 1F EE 20 00 5C          ; dessin banque 1 frame 31 dy -18 dx 92
    04 1E 04 20 00 43          ; dessin banque 1 frame 30 dy 4 dx 67
    04 20 05 20 00 75          ; dessin banque 1 frame 32 dy 5 dx 117
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    04 2F 24 20 FF EA          ; dessin banque 1 frame 47 dy 36 dx -22
    04 29 1F 20 00 4C          ; dessin banque 1 frame 41 dy 31 dx 76
    04 28 2A 20 00 26          ; dessin banque 1 frame 40 dy 42 dx 38
    04 26 2B 20 FF FF          ; dessin banque 1 frame 38 dy 43 dx -1
    04 2A 29 20 00 59          ; dessin banque 1 frame 42 dy 41 dx 89
    04 27 33 20 00 12          ; dessin banque 1 frame 39 dy 51 dx 18
    00 09 1B 00 00 07          ; dessin banque 0 frame 9 dy 27 dx 7
    00 09 14 00 00 7D          ; dessin banque 0 frame 9 dy 20 dx 125
    00 0A 1C 00 00 6E          ; dessin banque 0 frame 10 dy 28 dx 110
    00 0A 25 00 00 14          ; dessin banque 0 frame 10 dy 37 dx 20
    00 09 10 00 00 4D          ; dessin banque 0 frame 9 dy 16 dx 77
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    04 3F 32 30 FF DC          ; dessin banque 1 frame 63 dy 50 dx -36 [décor]
    04 29 1F 30 00 4C          ; dessin banque 1 frame 41 dy 31 dx 76 [décor]
    04 3F 32 30 00 09          ; dessin banque 1 frame 63 dy 50 dx 9 [décor]
    04 2B 31 30 00 31          ; dessin banque 1 frame 43 dy 49 dx 49 [décor]
    04 41 31 30 00 53          ; dessin banque 1 frame 65 dy 49 dx 83 [décor]
    04 27 33 30 00 12          ; dessin banque 1 frame 39 dy 51 dx 18 [décor]
    04 27 2D 30 00 3A          ; dessin banque 1 frame 39 dy 45 dx 58 [décor]
    FF 00                      ; fin d'étape
  ; étape 6
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08A3` (4:$5F8A)

Rôles : référencé par le code dans LAB_015C ; référencé par le code dans LAB_0EB2 ; référencé par le code dans LAB_0EB5

```
  ; étape 1
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    A4 51                      ; $A4 Sound
    88 03                      ; $88 Hold
    B4 00 00 00 5F B4          ; $B4 IfDead -> LAB_08A4
    04 33 F0 20 FF DC          ; dessin banque 1 frame 51 dy -16 dx -36
    04 32 06 20 FF DC          ; dessin banque 1 frame 50 dy 6 dx -36
    04 31 1F 20 FF E3          ; dessin banque 1 frame 49 dy 31 dx -29
    04 30 2A 60 FF CD          ; dessin banque 1 frame 48 dy 42 dx -51 [hors-boîte]
    FF FF                      ; fin du script
  LAB_08A4:
  ; étape 2
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    A4 52                      ; $A4 Sound
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 34 F9 20 00 02          ; dessin banque 1 frame 52 dy -7 dx 2
    04 35 08 20 00 15          ; dessin banque 1 frame 53 dy 8 dx 21
    04 36 2D 60 00 2A          ; dessin banque 1 frame 54 dy 45 dx 42 [hors-boîte]
    04 3E 08 20 FF E3          ; dessin banque 1 frame 62 dy 8 dx -29
    04 3D 2D 60 FF D4          ; dessin banque 1 frame 61 dy 45 dx -44 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    04 39 15 20 FF FD          ; dessin banque 1 frame 57 dy 21 dx -3
    04 38 22 20 FF D8          ; dessin banque 1 frame 56 dy 34 dx -40
    04 37 2E 60 FF C9          ; dessin banque 1 frame 55 dy 46 dx -55 [hors-boîte]
    04 3A 24 20 00 16          ; dessin banque 1 frame 58 dy 36 dx 22
    04 3B 2E 60 00 2D          ; dessin banque 1 frame 59 dy 46 dx 45 [hors-boîte]
    04 3C 35 60 00 40          ; dessin banque 1 frame 60 dy 53 dx 64 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    88 03                      ; $88 Hold
    04 3F 34 30 FF BA          ; dessin banque 1 frame 63 dy 52 dx -70 [décor]
    04 40 29 30 FF F8          ; dessin banque 1 frame 64 dy 41 dx -8 [décor]
    04 41 34 30 00 14          ; dessin banque 1 frame 65 dy 52 dx 20 [décor]
    FF 00                      ; fin d'étape
  ; étape 5
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08A4` (4:$5FB4)

Rôles : cible de saut depuis LAB_08A3

```
  ; étape 1
    B0 00 00 00 04 9E          ; $B0 Call -> LAB_0EBD
    A4 52                      ; $A4 Sound
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    88 02                      ; $88 Hold
    04 34 F9 20 00 02          ; dessin banque 1 frame 52 dy -7 dx 2
    04 35 08 20 00 15          ; dessin banque 1 frame 53 dy 8 dx 21
    04 36 2D 60 00 2A          ; dessin banque 1 frame 54 dy 45 dx 42 [hors-boîte]
    04 3E 08 20 FF E3          ; dessin banque 1 frame 62 dy 8 dx -29
    04 3D 2D 60 FF D4          ; dessin banque 1 frame 61 dy 45 dx -44 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    04 39 15 20 FF FD          ; dessin banque 1 frame 57 dy 21 dx -3
    04 38 22 20 FF D8          ; dessin banque 1 frame 56 dy 34 dx -40
    04 37 2E 60 FF C9          ; dessin banque 1 frame 55 dy 46 dx -55 [hors-boîte]
    04 3A 24 20 00 16          ; dessin banque 1 frame 58 dy 36 dx 22
    04 3B 2E 60 00 2D          ; dessin banque 1 frame 59 dy 46 dx 45 [hors-boîte]
    04 3C 35 60 00 40          ; dessin banque 1 frame 60 dy 53 dx 64 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 3
    88 03                      ; $88 Hold
    04 3F 34 30 FF BA          ; dessin banque 1 frame 63 dy 52 dx -70 [décor]
    04 40 29 30 FF F8          ; dessin banque 1 frame 64 dy 41 dx -8 [décor]
    04 41 34 30 00 14          ; dessin banque 1 frame 65 dy 52 dx 20 [décor]
    FF 00                      ; fin d'étape
  ; étape 4
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08A5` (4:$6026)

Rôles : cible de saut depuis LAB_08AA ; objet+22 (repos) dans LAB_019F ; objet+26 (réaction) dans LAB_019F

```
  ; étape 1
    00 08 2A 00 FF EB          ; dessin banque 0 frame 8 dy 42 dx -21
    00 07 06 01 FF F8          ; dessin banque 0 frame 7 dy 6 dx -8 [corps]
    00 06 D5 01 FF E6          ; dessin banque 0 frame 6 dy -43 dx -26 [corps]
    FF FF                      ; fin du script
```

### `LAB_08A6` (4:$603A)

Rôles : référencé par le code dans LAB_015B

```
  ; étape 1
    00 02 FC 01 FF E9          ; dessin banque 0 frame 2 dy -4 dx -23 [corps]
    00 00 D4 01 FF EE          ; dessin banque 0 frame 0 dy -44 dx -18 [corps]
    00 01 FE 00 00 0F          ; dessin banque 0 frame 1 dy -2 dx 15
    FF FF                      ; fin du script
```

### `LAB_08A7` (4:$604E)

Rôles : référencé par le code dans LAB_015B

```
  ; étape 1
    00 03 FC 01 FF E5          ; dessin banque 0 frame 3 dy -4 dx -27 [corps]
    00 00 D3 01 FF ED          ; dessin banque 0 frame 0 dy -45 dx -19 [corps]
    00 01 FD 00 00 0E          ; dessin banque 0 frame 1 dy -3 dx 14
    FF FF                      ; fin du script
```

### `LAB_08A8` (4:$6062)

Rôles : référencé par le code dans LAB_015B

```
  ; étape 1
    00 04 FC 01 FF EC          ; dessin banque 0 frame 4 dy -4 dx -20 [corps]
    00 00 D4 01 FF ED          ; dessin banque 0 frame 0 dy -44 dx -19 [corps]
    00 01 FE 00 00 0E          ; dessin banque 0 frame 1 dy -2 dx 14
    FF FF                      ; fin du script
```

### `LAB_08A9` (4:$6076)

Rôles : référencé par le code dans LAB_015B

```
  ; étape 1
    00 05 FC 01 FF E8          ; dessin banque 0 frame 5 dy -4 dx -24 [corps]
    00 00 D3 01 FF ED          ; dessin banque 0 frame 0 dy -45 dx -19 [corps]
    00 01 FD 00 00 0E          ; dessin banque 0 frame 1 dy -3 dx 14
    FF FF                      ; fin du script
```

### `LAB_08AA` (4:$608A)

Rôles : référencé par le code dans LAB_0EC9

```
  LAB_08A5:
  ; étape 1
    00 08 2A 00 FF EB          ; dessin banque 0 frame 8 dy 42 dx -21
    00 07 06 01 FF F8          ; dessin banque 0 frame 7 dy 6 dx -8 [corps]
    00 06 D5 01 FF E6          ; dessin banque 0 frame 6 dy -43 dx -26 [corps]
    FF FF                      ; fin du script
  ; étape 2
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    88 01                      ; $88 Hold
    00 08 2A 00 FF EB          ; dessin banque 0 frame 8 dy 42 dx -21
    00 07 06 01 FF F8          ; dessin banque 0 frame 7 dy 6 dx -8 [corps]
    00 06 D5 01 FF E6          ; dessin banque 0 frame 6 dy -43 dx -26 [corps]
    FF 00                      ; fin d'étape
  ; étape 3
    00 0B 1C 01 FF F3          ; dessin banque 0 frame 11 dy 28 dx -13 [corps]
    00 0A 01 01 FF FD          ; dessin banque 0 frame 10 dy 1 dx -3 [corps]
    00 09 DA 01 00 03          ; dessin banque 0 frame 9 dy -38 dx 3 [corps]
    00 01 00 02 00 21          ; dessin banque 0 frame 1 dy 0 dx 33 [frappe]
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    84 00 00 00 60 26          ; $84 Jump -> LAB_08A5
    00 0D D9 01 FF F8          ; dessin banque 0 frame 13 dy -39 dx -8 [corps]
    00 0E F3 01 00 25          ; dessin banque 0 frame 14 dy -13 dx 37 [corps]
    00 0F FA 02 00 41          ; dessin banque 0 frame 15 dy -6 dx 65 [frappe]
    FF FF                      ; fin du script
```

### `LAB_08AB` (4:$60DC)

Rôles : référencé par le code dans LAB_0ECA

```
  ; étape 1
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    00 22 2A 00 FF E5          ; dessin banque 0 frame 34 dy 42 dx -27
    00 21 F4 01 FF FD          ; dessin banque 0 frame 33 dy -12 dx -3 [corps]
    00 20 D6 01 FF F4          ; dessin banque 0 frame 32 dy -42 dx -12 [corps]
    00 1F BE 01 FF FA          ; dessin banque 0 frame 31 dy -66 dx -6 [corps]
    00 1E A9 00 FF E8          ; dessin banque 0 frame 30 dy -87 dx -24
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 1D 28 00 FF D7          ; dessin banque 0 frame 29 dy 40 dx -41
    00 17 00 01 FF FA          ; dessin banque 0 frame 23 dy 0 dx -6 [corps]
    00 16 D8 01 FF EF          ; dessin banque 0 frame 22 dy -40 dx -17 [corps]
    00 15 D1 01 FF C3          ; dessin banque 0 frame 21 dy -47 dx -61 [corps]
    00 14 C8 00 FF 94          ; dessin banque 0 frame 20 dy -56 dx -108
    FF 00                      ; fin d'étape
  ; étape 3
    A4 0D                      ; $A4 Sound
    00 22 2A 00 FF E5          ; dessin banque 0 frame 34 dy 42 dx -27
    00 21 F4 01 FF FD          ; dessin banque 0 frame 33 dy -12 dx -3 [corps]
    00 1C CF 01 FF F2          ; dessin banque 0 frame 28 dy -49 dx -14 [corps]
    00 1B C5 01 FF DF          ; dessin banque 0 frame 27 dy -59 dx -33 [corps]
    00 1A B3 01 FF CB          ; dessin banque 0 frame 26 dy -77 dx -53 [corps]
    00 19 A3 00 FF BA          ; dessin banque 0 frame 25 dy -93 dx -70
    00 18 9B 00 FF A8          ; dessin banque 0 frame 24 dy -101 dx -88
    FF 00                      ; fin d'étape
  ; étape 4
    00 28 2B 00 FF F1          ; dessin banque 0 frame 40 dy 43 dx -15
    00 26 F1 01 FF FD          ; dessin banque 0 frame 38 dy -15 dx -3 [corps]
    00 27 F9 01 00 14          ; dessin banque 0 frame 39 dy -7 dx 20 [corps]
    00 25 D2 01 00 14          ; dessin banque 0 frame 37 dy -46 dx 20 [corps]
    00 2B 98 00 00 27          ; dessin banque 0 frame 43 dy -104 dx 39
    00 23 A6 00 00 43          ; dessin banque 0 frame 35 dy -90 dx 67
    00 2A 8D 00 FF FC          ; dessin banque 0 frame 42 dy -115 dx -4
    00 29 8D 00 FF B2          ; dessin banque 0 frame 41 dy -115 dx -78
    FF 00                      ; fin d'étape
  ; étape 5
    04 00 17 01 FF FA          ; dessin banque 1 frame 0 dy 23 dx -6 [corps]
    04 01 01 01 00 13          ; dessin banque 1 frame 1 dy 1 dx 19 [corps]
    04 02 20 02 00 69          ; dessin banque 1 frame 2 dy 32 dx 105 [frappe]
    00 24 F4 02 00 8C          ; dessin banque 0 frame 36 dy -12 dx 140 [frappe]
    00 2F D9 02 00 81          ; dessin banque 0 frame 47 dy -39 dx 129 [frappe]
    00 2E C4 00 00 77          ; dessin banque 0 frame 46 dy -60 dx 119
    00 2D B7 00 00 68          ; dessin banque 0 frame 45 dy -73 dx 104
    00 2C AF 00 00 5A          ; dessin banque 0 frame 44 dy -81 dx 90
    B0 00 00 00 90 B8          ; $B0 Call -> LAB_0427
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    04 00 17 01 FF FA          ; dessin banque 1 frame 0 dy 23 dx -6 [corps]
    04 01 01 01 00 13          ; dessin banque 1 frame 1 dy 1 dx 19 [corps]
    04 02 20 00 00 69          ; dessin banque 1 frame 2 dy 32 dx 105
    FF FF                      ; fin du script
```

### `LAB_08AC` (4:$61D2)

Rôles : référencé par le code dans LAB_0ECC

```
  ; étape 1
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    04 06 2B 00 FF F1          ; dessin banque 1 frame 6 dy 43 dx -15
    04 05 EF 01 FF F1          ; dessin banque 1 frame 5 dy -17 dx -15 [corps]
    04 04 E0 01 FF F1          ; dessin banque 1 frame 4 dy -32 dx -15 [corps]
    04 03 D3 01 FF F6          ; dessin banque 1 frame 3 dy -45 dx -10 [corps]
    00 11 DB 00 00 0E          ; dessin banque 0 frame 17 dy -37 dx 14
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    B4 00 00 00 62 26          ; $B4 IfDead -> LAB_08AD
    04 06 2B 00 FF F1          ; dessin banque 1 frame 6 dy 43 dx -15
    04 09 03 01 FF FC          ; dessin banque 1 frame 9 dy 3 dx -4 [corps]
    04 07 D6 01 FF F7          ; dessin banque 1 frame 7 dy -42 dx -9 [corps]
    04 08 E8 01 00 1B          ; dessin banque 1 frame 8 dy -24 dx 27 [corps]
    00 11 D1 00 00 2F          ; dessin banque 0 frame 17 dy -47 dx 47
    FF FF                      ; fin du script
  LAB_08AD:
  ; étape 3
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    88 04                      ; $88 Hold
    04 0B 25 00 00 00          ; dessin banque 1 frame 11 dy 37 dx 0
    04 0C 00 00 00 29          ; dessin banque 1 frame 12 dy 0 dx 41
    04 0D 31 00 00 68          ; dessin banque 1 frame 13 dy 49 dx 104
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    04 0E 25 10 00 1C          ; dessin banque 1 frame 14 dy 37 dx 28 [décor]
    04 0F 24 10 00 6C          ; dessin banque 1 frame 15 dy 36 dx 108 [décor]
    04 0D 31 10 00 68          ; dessin banque 1 frame 13 dy 49 dx 104 [décor]
    FF 00                      ; fin d'étape
  ; étape 5
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08AD` (4:$6226)

Rôles : cible de saut depuis LAB_08AC

```
  ; étape 1
    B0 00 00 00 03 5A          ; $B0 Call -> LAB_0005
    B0 00 00 00 06 82          ; $B0 Call -> LAB_0ED0
    88 04                      ; $88 Hold
    04 0B 25 00 00 00          ; dessin banque 1 frame 11 dy 37 dx 0
    04 0C 00 00 00 29          ; dessin banque 1 frame 12 dy 0 dx 41
    04 0D 31 00 00 68          ; dessin banque 1 frame 13 dy 49 dx 104
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    04 0E 25 10 00 1C          ; dessin banque 1 frame 14 dy 37 dx 28 [décor]
    04 0F 24 10 00 6C          ; dessin banque 1 frame 15 dy 36 dx 108 [décor]
    04 0D 31 10 00 68          ; dessin banque 1 frame 13 dy 49 dx 104 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08AE` (4:$6262)

Rôles : objet+22 (repos) dans LAB_01A0

```
  ; étape 1
    88 02                      ; $88 Hold
    10 00 E9 20 FF FF          ; dessin banque 4 frame 0 dy -23 dx -1
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    10 01 DB 20 00 08          ; dessin banque 4 frame 1 dy -37 dx 8
    10 02 EB 20 FF FB          ; dessin banque 4 frame 2 dy -21 dx -5
    10 03 F8 20 FF F5          ; dessin banque 4 frame 3 dy -8 dx -11
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    10 04 C8 20 00 02          ; dessin banque 4 frame 4 dy -56 dx 2
    10 05 DF 20 FF F9          ; dessin banque 4 frame 5 dy -33 dx -7
    10 06 ED 20 FF F1          ; dessin banque 4 frame 6 dy -19 dx -15
    10 07 F8 20 FF EA          ; dessin banque 4 frame 7 dy -8 dx -22
    10 08 04 20 FF F2          ; dessin banque 4 frame 8 dy 4 dx -14
    10 09 0E 20 FF FB          ; dessin banque 4 frame 9 dy 14 dx -5
    10 0A 1D 20 FF F8          ; dessin banque 4 frame 10 dy 29 dx -8
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    10 0B AD 20 00 03          ; dessin banque 4 frame 11 dy -83 dx 3
    10 0C C7 20 FF F7          ; dessin banque 4 frame 12 dy -57 dx -9
    10 0D DC 20 FF EA          ; dessin banque 4 frame 13 dy -36 dx -22
    10 0E EC 20 FF F1          ; dessin banque 4 frame 14 dy -20 dx -15
    10 0F FA 20 FF EE          ; dessin banque 4 frame 15 dy -6 dx -18
    10 10 06 20 FF F2          ; dessin banque 4 frame 16 dy 6 dx -14
    10 11 23 20 FF F6          ; dessin banque 4 frame 17 dy 35 dx -10
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    10 12 AC 20 00 04          ; dessin banque 4 frame 18 dy -84 dx 4
    10 13 BD 20 FF F6          ; dessin banque 4 frame 19 dy -67 dx -10
    10 14 DB 20 FF EE          ; dessin banque 4 frame 20 dy -37 dx -18
    10 15 EB 20 FF EA          ; dessin banque 4 frame 21 dy -21 dx -22
    10 16 FB 20 FF E9          ; dessin banque 4 frame 22 dy -5 dx -23
    10 17 09 20 FF F2          ; dessin banque 4 frame 23 dy 9 dx -14
    10 18 17 20 FF F3          ; dessin banque 4 frame 24 dy 23 dx -13
    10 19 29 20 FF E7          ; dessin banque 4 frame 25 dy 41 dx -25
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    10 1A AC 20 00 04          ; dessin banque 4 frame 26 dy -84 dx 4
    10 1B BE 20 FF F5          ; dessin banque 4 frame 27 dy -66 dx -11
    10 1C E5 20 FF FA          ; dessin banque 4 frame 28 dy -27 dx -6
    10 1D F6 20 FF E5          ; dessin banque 4 frame 29 dy -10 dx -27
    10 1E 07 20 FF EB          ; dessin banque 4 frame 30 dy 7 dx -21
    10 1F 12 20 FF F6          ; dessin banque 4 frame 31 dy 18 dx -10
    10 20 1B 20 FF F0          ; dessin banque 4 frame 32 dy 27 dx -16
    10 21 2B 20 FF E2          ; dessin banque 4 frame 33 dy 43 dx -30
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    10 22 AE 20 FF F6          ; dessin banque 4 frame 34 dy -82 dx -10
    10 23 C6 20 FF F6          ; dessin banque 4 frame 35 dy -58 dx -10
    10 24 F1 20 FF E2          ; dessin banque 4 frame 36 dy -15 dx -30
    10 25 01 20 FF ED          ; dessin banque 4 frame 37 dy 1 dx -19
    10 26 11 20 FF F4          ; dessin banque 4 frame 38 dy 17 dx -12
    10 27 1D 20 FF ED          ; dessin banque 4 frame 39 dy 29 dx -19
    10 28 2A 20 FF E6          ; dessin banque 4 frame 40 dy 42 dx -26
    FF 00                      ; fin d'étape
  ; étape 8
    10 29 AE 20 FF F4          ; dessin banque 4 frame 41 dy -82 dx -12
    10 2A B6 20 00 1F          ; dessin banque 4 frame 42 dy -74 dx 31
    10 2B E0 20 FF F1          ; dessin banque 4 frame 43 dy -32 dx -15
    10 2C F5 20 FF E4          ; dessin banque 4 frame 44 dy -11 dx -28
    10 2D 08 20 FF ED          ; dessin banque 4 frame 45 dy 8 dx -19
    10 2E 19 20 FF EB          ; dessin banque 4 frame 46 dy 25 dx -21
    10 2F 26 20 FF E6          ; dessin banque 4 frame 47 dy 38 dx -26
    FF 00                      ; fin d'étape
  ; étape 9
    10 29 AE 20 FF F4          ; dessin banque 4 frame 41 dy -82 dx -12
    10 2A B6 20 00 1F          ; dessin banque 4 frame 42 dy -74 dx 31
    10 2B E0 20 FF F1          ; dessin banque 4 frame 43 dy -32 dx -15
    10 2C F5 20 FF E4          ; dessin banque 4 frame 44 dy -11 dx -28
    10 2D 08 20 FF ED          ; dessin banque 4 frame 45 dy 8 dx -19
    10 2E 19 20 FF EB          ; dessin banque 4 frame 46 dy 25 dx -21
    10 2F 26 20 FF E6          ; dessin banque 4 frame 47 dy 38 dx -26
    B0 00 00 00 0A 0E          ; $B0 Call -> LAB_0EEB
    FF 00                      ; fin d'étape
  ; étape 10
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 1B E4 00 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF 00                      ; fin d'étape
  ; étape 11
    A4 46                      ; $A4 Sound
    A4 47                      ; $A4 Sound
    84 03 00 00 64 7E          ; $84 Jump -> LAB_08B0
  LAB_08B0:
    A8 04 00 16 00 00 64 AC    ; $A8 SetField -> LAB_08B1
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08AF` (4:$6404)

Rôles : cible de saut depuis LAB_08AF ; référencé par le code dans LAB_0EEB

```
  ; étape 1
    B0 00 00 00 0A 52          ; $B0 Call -> LAB_0EEC
    00 00 FD 00 FF E0          ; dessin banque 0 frame 0 dy -3 dx -32
    00 01 0B 00 00 11          ; dessin banque 0 frame 1 dy 11 dx 17
    00 02 24 00 FF E5          ; dessin banque 0 frame 2 dy 36 dx -27
    FF 00                      ; fin d'étape
  ; étape 2
    B0 00 00 00 0A 52          ; $B0 Call -> LAB_0EEC
    00 03 0A 00 FF D9          ; dessin banque 0 frame 3 dy 10 dx -39
    00 04 FE 00 FF EA          ; dessin banque 0 frame 4 dy -2 dx -22
    00 05 0E 00 00 11          ; dessin banque 0 frame 5 dy 14 dx 17
    00 06 1E 00 FF E7          ; dessin banque 0 frame 6 dy 30 dx -25
    FF 00                      ; fin d'étape
  ; étape 3
    B0 00 00 00 0A 52          ; $B0 Call -> LAB_0EEC
    00 07 07 00 FF D8          ; dessin banque 0 frame 7 dy 7 dx -40
    00 08 FE 00 FF EB          ; dessin banque 0 frame 8 dy -2 dx -21
    00 09 04 00 00 15          ; dessin banque 0 frame 9 dy 4 dx 21
    00 0A 25 00 FF E3          ; dessin banque 0 frame 10 dy 37 dx -29
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 0A 52          ; $B0 Call -> LAB_0EEC
    84 00 00 00 64 04          ; $84 Jump -> LAB_08AF
    00 0B FD 00 FF E2          ; dessin banque 0 frame 11 dy -3 dx -30
    00 0C 19 00 FF EE          ; dessin banque 0 frame 12 dy 25 dx -18
    00 0D 2A 00 FF E2          ; dessin banque 0 frame 13 dy 42 dx -30
    FF FE                      ; fin d'étape, boucle
  LAB_08B0:
  ; étape 5
    A8 04 00 16 00 00 64 AC    ; $A8 SetField -> LAB_08B1
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08B0` (4:$647E)

Rôles : cible de saut depuis LAB_08AE ; cible de saut depuis LAB_08B4 ; cible de saut depuis LAB_08B5 ; objet+26 (réaction) dans LAB_01A0 ; écrit dans objet+22 (repos) par $A8 depuis LAB_08B3

```
  ; étape 1
    A8 04 00 16 00 00 64 AC    ; $A8 SetField -> LAB_08B1
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08B1` (4:$64AC)

Rôles : cible de saut depuis LAB_08B4 ; écrit dans objet+22 (repos) par $A8 depuis LAB_08AE ; écrit dans objet+22 (repos) par $A8 depuis LAB_08AF ; écrit dans objet+22 (repos) par $A8 depuis LAB_08B0 ; écrit dans objet+22 (repos) par $A8 depuis LAB_08B4 ; écrit dans objet+22 (repos) par $A8 depuis LAB_08B5

```
  ; étape 1
    A8 04 00 16 00 00 64 DA    ; $A8 SetField -> LAB_08B2
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08B2` (4:$64DA)

Rôles : écrit dans objet+22 (repos) par $A8 depuis LAB_08B1 ; écrit dans objet+22 (repos) par $A8 depuis LAB_08B4

```
  ; étape 1
    A8 04 00 16 00 00 65 08    ; $A8 SetField -> LAB_08B3
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F0          ; dessin banque 0 frame 21 dy -49 dx -16
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08B3` (4:$6508)

Rôles : écrit dans objet+22 (repos) par $A8 depuis LAB_08B2

```
  ; étape 1
    A8 04 00 16 00 00 64 7E    ; $A8 SetField -> LAB_08B0
    00 17 B3 40 FF EE          ; dessin banque 0 frame 23 dy -77 dx -18 [hors-boîte]
    00 18 CF 00 FF F1          ; dessin banque 0 frame 24 dy -49 dx -15
    00 19 E4 00 FF EB          ; dessin banque 0 frame 25 dy -28 dx -21
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
```

### `LAB_08B4` (4:$6536)

Rôles : référencé par le code dans LAB_0EDD

```
  LAB_08B0:
  ; étape 1
    A8 04 00 16 00 00 64 AC    ; $A8 SetField -> LAB_08B1
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
  LAB_08B1:
  ; étape 2
    A8 04 00 16 00 00 64 DA    ; $A8 SetField -> LAB_08B2
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
  ; étape 3
    A4 46                      ; $A4 Sound
    A4 47                      ; $A4 Sound
    00 10 E5 00 FF EE          ; dessin banque 0 frame 16 dy -27 dx -18
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    00 1F F7 40 00 12          ; dessin banque 0 frame 31 dy -9 dx 18 [hors-boîte]
    00 20 F7 40 00 1B          ; dessin banque 0 frame 32 dy -9 dx 27 [hors-boîte]
    00 0F D0 00 FF F5          ; dessin banque 0 frame 15 dy -48 dx -11
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    00 0E B0 40 FF EF          ; dessin banque 0 frame 14 dy -80 dx -17 [hors-boîte]
    FF 00                      ; fin d'étape
  ; étape 4
    00 13 E5 00 FF F0          ; dessin banque 0 frame 19 dy -27 dx -16
    00 12 D0 00 FF F7          ; dessin banque 0 frame 18 dy -48 dx -9
    00 23 F7 02 00 23          ; dessin banque 0 frame 35 dy -9 dx 35 [frappe]
    00 22 F7 00 00 0E          ; dessin banque 0 frame 34 dy -9 dx 14
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 11 B7 40 FF F3          ; dessin banque 0 frame 17 dy -73 dx -13 [hors-boîte]
    00 1A D0 01 00 10          ; dessin banque 0 frame 26 dy -48 dx 16 [corps]
    FF 00                      ; fin d'étape
  ; étape 5
    00 15 D3 00 FF FD          ; dessin banque 0 frame 21 dy -45 dx -3
    00 25 05 00 FF FE          ; dessin banque 0 frame 37 dy 5 dx -2
    00 26 EB 40 00 24          ; dessin banque 0 frame 38 dy -21 dx 36 [hors-boîte]
    00 27 04 40 00 24          ; dessin banque 0 frame 39 dy 4 dx 36 [hors-boîte]
    00 28 FC 42 00 36          ; dessin banque 0 frame 40 dy -4 dx 54 [frappe,hors-boîte]
    00 29 DD 42 00 4E          ; dessin banque 0 frame 41 dy -35 dx 78 [frappe,hors-boîte]
    00 24 E3 01 FF EF          ; dessin banque 0 frame 36 dy -29 dx -17 [corps]
    00 14 B6 40 FF FA          ; dessin banque 0 frame 20 dy -74 dx -6 [hors-boîte]
    00 1A D3 01 00 18          ; dessin banque 0 frame 26 dy -45 dx 24 [corps]
    FF 00                      ; fin d'étape
  ; étape 6
    00 2B DE 42 00 34          ; dessin banque 0 frame 43 dy -34 dx 52 [frappe,hors-boîte]
    00 2A DE 42 00 19          ; dessin banque 0 frame 42 dy -34 dx 25 [frappe,hors-boîte]
    00 2C E4 40 00 5B          ; dessin banque 0 frame 44 dy -28 dx 91 [hors-boîte]
    00 19 E6 00 FF F0          ; dessin banque 0 frame 25 dy -26 dx -16
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 17 B5 40 FF F3          ; dessin banque 0 frame 23 dy -75 dx -13 [hors-boîte]
    00 1A D1 01 00 0F          ; dessin banque 0 frame 26 dy -47 dx 15 [corps]
    00 18 D1 00 FF F6          ; dessin banque 0 frame 24 dy -47 dx -10
    FF 00                      ; fin d'étape
  ; étape 7
    84 00 00 00 64 AC          ; $84 Jump -> LAB_08B1
    00 2F E3 40 00 2E          ; dessin banque 0 frame 47 dy -29 dx 46 [hors-boîte]
    00 2E E6 42 00 1E          ; dessin banque 0 frame 46 dy -26 dx 30 [frappe,hors-boîte]
    00 2D E4 42 00 0E          ; dessin banque 0 frame 45 dy -28 dx 14 [frappe,hors-boîte]
    00 10 E6 00 FF ED          ; dessin banque 0 frame 16 dy -26 dx -19
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 0E B1 40 FF EE          ; dessin banque 0 frame 14 dy -79 dx -18 [hors-boîte]
    00 0F D1 00 FF F4          ; dessin banque 0 frame 15 dy -47 dx -12
    00 1A D1 01 00 0C          ; dessin banque 0 frame 26 dy -47 dx 12 [corps]
    FF 00                      ; fin d'étape
  LAB_08B5:
  ; étape 8
    A4 48                      ; $A4 Sound
    A4 49                      ; $A4 Sound
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 30 DA 01 FF EC          ; dessin banque 0 frame 48 dy -38 dx -20 [corps]
    00 31 F8 40 00 1C          ; dessin banque 0 frame 49 dy -8 dx 28 [hors-boîte]
    00 32 FF 00 FF F4          ; dessin banque 0 frame 50 dy -1 dx -12
    00 1A CF 01 00 0A          ; dessin banque 0 frame 26 dy -49 dx 10 [corps]
    FF 00                      ; fin d'étape
  ; étape 9
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 01 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10 [corps]
    00 36 F8 40 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30 [hors-boîte]
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    FF 00                      ; fin d'étape
  ; étape 10
    A4 4E                      ; $A4 Sound
    B0 00 00 00 0B A8          ; $B0 Call -> LAB_0EF6
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F0          ; dessin banque 0 frame 21 dy -49 dx -16
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 01 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10 [corps]
    00 36 F8 40 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30 [hors-boîte]
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 37 BF 00 FF EE          ; dessin banque 0 frame 55 dy -65 dx -18
    00 38 CC 00 FF FF          ; dessin banque 0 frame 56 dy -52 dx -1
    00 39 DA 00 00 04          ; dessin banque 0 frame 57 dy -38 dx 4
    00 3A E5 40 00 18          ; dessin banque 0 frame 58 dy -27 dx 24 [hors-boîte]
    00 3B EC 40 00 2D          ; dessin banque 0 frame 59 dy -20 dx 45 [hors-boîte]
    00 3C F3 00 00 6D          ; dessin banque 0 frame 60 dy -13 dx 109
    FF 00                      ; fin d'étape
  ; étape 11
    94 04                      ; $94 Loop
    00 17 B3 00 FF EE          ; dessin banque 0 frame 23 dy -77 dx -18
    00 18 CF 00 FF F1          ; dessin banque 0 frame 24 dy -49 dx -15
    00 19 E4 00 FF EB          ; dessin banque 0 frame 25 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 3D C1 00 FF F0          ; dessin banque 0 frame 61 dy -63 dx -16
    00 3E D4 00 00 00          ; dessin banque 0 frame 62 dy -44 dx 0
    00 3F DB 00 00 0D          ; dessin banque 0 frame 63 dy -37 dx 13
    00 40 F2 00 00 27          ; dessin banque 0 frame 64 dy -14 dx 39
    00 41 F4 00 00 38          ; dessin banque 0 frame 65 dy -12 dx 56
    00 43 EC 00 00 88          ; dessin banque 0 frame 67 dy -20 dx 136
    00 44 11 00 00 88          ; dessin banque 0 frame 68 dy 17 dx 136
    00 45 F0 00 00 59          ; dessin banque 0 frame 69 dy -16 dx 89
    00 46 1F 00 00 61          ; dessin banque 0 frame 70 dy 31 dx 97
    00 42 E3 00 00 88          ; dessin banque 0 frame 66 dy -29 dx 136
    FF 00                      ; fin d'étape
  ; étape 12
    00 0E AF 00 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 47 BE 00 FF EE          ; dessin banque 0 frame 71 dy -66 dx -18
    00 48 CD 00 FF FB          ; dessin banque 0 frame 72 dy -51 dx -5
    00 49 DB 00 00 03          ; dessin banque 0 frame 73 dy -37 dx 3
    00 4A E3 00 00 18          ; dessin banque 0 frame 74 dy -29 dx 24
    00 4B F3 00 00 27          ; dessin banque 0 frame 75 dy -13 dx 39
    00 4C F2 00 00 42          ; dessin banque 0 frame 76 dy -14 dx 66
    00 4D EA 00 00 60          ; dessin banque 0 frame 77 dy -22 dx 96
    00 4E 1F 00 00 65          ; dessin banque 0 frame 78 dy 31 dx 101
    00 4F FE 00 00 A8          ; dessin banque 0 frame 79 dy -2 dx 168
    FF 00                      ; fin d'étape
  ; étape 13
    00 11 B6 00 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 50 C0 00 FF EE          ; dessin banque 0 frame 80 dy -64 dx -18
    00 51 CE 00 00 01          ; dessin banque 0 frame 81 dy -50 dx 1
    00 52 E4 00 00 0D          ; dessin banque 0 frame 82 dy -28 dx 13
    00 53 EC 00 00 1F          ; dessin banque 0 frame 83 dy -20 dx 31
    00 54 EE 00 00 2C          ; dessin banque 0 frame 84 dy -18 dx 44
    00 55 F0 00 00 3C          ; dessin banque 0 frame 85 dy -16 dx 60
    00 56 EE 00 00 4D          ; dessin banque 0 frame 86 dy -18 dx 77
    00 57 E6 00 00 7C          ; dessin banque 0 frame 87 dy -26 dx 124
    00 58 F5 00 00 8B          ; dessin banque 0 frame 88 dy -11 dx 139
    00 59 1C 00 00 5B          ; dessin banque 0 frame 89 dy 28 dx 91
    00 5A 21 00 00 9D          ; dessin banque 0 frame 90 dy 33 dx 157
    00 5B 08 00 00 AC          ; dessin banque 0 frame 91 dy 8 dx 172
    FF FE                      ; fin d'étape, boucle
  ; étape 14
    B0 00 00 00 0B D2          ; $B0 Call -> LAB_0EF7
    84 00 00 00 64 7E          ; $84 Jump -> LAB_08B0
    00 14 B2 00 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19
    00 15 CF 00 FF F0          ; dessin banque 0 frame 21 dy -49 dx -16
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    00 30 DA 00 FF EC          ; dessin banque 0 frame 48 dy -38 dx -20
    00 31 F8 00 00 1C          ; dessin banque 0 frame 49 dy -8 dx 28
    00 32 FF 00 FF F4          ; dessin banque 0 frame 50 dy -1 dx -12
    00 1A CF 00 00 0A          ; dessin banque 0 frame 26 dy -49 dx 10
    00 5C FF 00 00 3D          ; dessin banque 0 frame 92 dy -1 dx 61
    00 5D FF 00 00 48          ; dessin banque 0 frame 93 dy -1 dx 72
    00 5E E2 00 00 48          ; dessin banque 0 frame 94 dy -30 dx 72
    00 5F F0 00 00 70          ; dessin banque 0 frame 95 dy -16 dx 112
    00 60 EB 00 00 7A          ; dessin banque 0 frame 96 dy -21 dx 122
    00 61 F6 00 00 8C          ; dessin banque 0 frame 97 dy -10 dx 140
    00 62 26 00 00 5E          ; dessin banque 0 frame 98 dy 38 dx 94
    FF 00                      ; fin d'étape
  LAB_08B6:
  ; étape 15
    08 00 E0 00 00 0D          ; dessin banque 2 frame 0 dy -32 dx 13
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F4          ; dessin banque 0 frame 15 dy -49 dx -12
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    FF 00                      ; fin d'étape
  ; étape 16
    08 01 D3 00 FF E0          ; dessin banque 2 frame 1 dy -45 dx -32
    08 02 DE 00 FF E0          ; dessin banque 2 frame 2 dy -34 dx -32
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F3          ; dessin banque 0 frame 18 dy -49 dx -13
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    FF 00                      ; fin d'étape
  ; étape 17
    08 03 C3 00 FF C6          ; dessin banque 2 frame 3 dy -61 dx -58
    08 04 C3 00 FF D9          ; dessin banque 2 frame 4 dy -61 dx -39
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F1          ; dessin banque 0 frame 21 dy -49 dx -15
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    FF 00                      ; fin d'étape
  ; étape 18
    08 05 C0 00 FF C7          ; dessin banque 2 frame 5 dy -64 dx -57
    08 06 C0 00 FF D1          ; dessin banque 2 frame 6 dy -64 dx -47
    00 19 E3 00 FF F1          ; dessin banque 0 frame 25 dy -29 dx -15
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    08 07 D3 00 00 14          ; dessin banque 2 frame 7 dy -45 dx 20
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 CF 00 FF F3          ; dessin banque 0 frame 24 dy -49 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 19
    08 08 C3 40 00 35          ; dessin banque 2 frame 8 dy -61 dx 53 [hors-boîte]
    08 09 D4 40 00 6F          ; dessin banque 2 frame 9 dy -44 dx 111 [hors-boîte]
    08 0A E7 40 00 19          ; dessin banque 2 frame 10 dy -25 dx 25 [hors-boîte]
    00 10 E5 00 FF F1          ; dessin banque 0 frame 16 dy -27 dx -15
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 0E B0 40 FF F3          ; dessin banque 0 frame 14 dy -80 dx -13 [hors-boîte]
    00 0F D0 00 FF F9          ; dessin banque 0 frame 15 dy -48 dx -7
    00 1A D0 01 00 10          ; dessin banque 0 frame 26 dy -48 dx 16 [corps]
    FF FF                      ; fin du script
```

### `LAB_08B5` (4:$6640)

Rôles : référencé par le code dans LAB_0EDE

```
  LAB_08B0:
  ; étape 1
    A8 04 00 16 00 00 64 AC    ; $A8 SetField -> LAB_08B1
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    FF FF                      ; fin du script
  ; étape 2
    A4 48                      ; $A4 Sound
    A4 49                      ; $A4 Sound
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 30 DA 01 FF EC          ; dessin banque 0 frame 48 dy -38 dx -20 [corps]
    00 31 F8 40 00 1C          ; dessin banque 0 frame 49 dy -8 dx 28 [hors-boîte]
    00 32 FF 00 FF F4          ; dessin banque 0 frame 50 dy -1 dx -12
    00 1A CF 01 00 0A          ; dessin banque 0 frame 26 dy -49 dx 10 [corps]
    FF 00                      ; fin d'étape
  ; étape 3
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 01 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10 [corps]
    00 36 F8 40 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30 [hors-boîte]
    00 1A CF 01 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11 [corps]
    FF 00                      ; fin d'étape
  ; étape 4
    A4 4E                      ; $A4 Sound
    B0 00 00 00 0B A8          ; $B0 Call -> LAB_0EF6
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F0          ; dessin banque 0 frame 21 dy -49 dx -16
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 01 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10 [corps]
    00 36 F8 40 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30 [hors-boîte]
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 37 BF 00 FF EE          ; dessin banque 0 frame 55 dy -65 dx -18
    00 38 CC 00 FF FF          ; dessin banque 0 frame 56 dy -52 dx -1
    00 39 DA 00 00 04          ; dessin banque 0 frame 57 dy -38 dx 4
    00 3A E5 40 00 18          ; dessin banque 0 frame 58 dy -27 dx 24 [hors-boîte]
    00 3B EC 40 00 2D          ; dessin banque 0 frame 59 dy -20 dx 45 [hors-boîte]
    00 3C F3 00 00 6D          ; dessin banque 0 frame 60 dy -13 dx 109
    FF 00                      ; fin d'étape
  ; étape 5
    94 04                      ; $94 Loop
    00 17 B3 00 FF EE          ; dessin banque 0 frame 23 dy -77 dx -18
    00 18 CF 00 FF F1          ; dessin banque 0 frame 24 dy -49 dx -15
    00 19 E4 00 FF EB          ; dessin banque 0 frame 25 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 3D C1 00 FF F0          ; dessin banque 0 frame 61 dy -63 dx -16
    00 3E D4 00 00 00          ; dessin banque 0 frame 62 dy -44 dx 0
    00 3F DB 00 00 0D          ; dessin banque 0 frame 63 dy -37 dx 13
    00 40 F2 00 00 27          ; dessin banque 0 frame 64 dy -14 dx 39
    00 41 F4 00 00 38          ; dessin banque 0 frame 65 dy -12 dx 56
    00 43 EC 00 00 88          ; dessin banque 0 frame 67 dy -20 dx 136
    00 44 11 00 00 88          ; dessin banque 0 frame 68 dy 17 dx 136
    00 45 F0 00 00 59          ; dessin banque 0 frame 69 dy -16 dx 89
    00 46 1F 00 00 61          ; dessin banque 0 frame 70 dy 31 dx 97
    00 42 E3 00 00 88          ; dessin banque 0 frame 66 dy -29 dx 136
    FF 00                      ; fin d'étape
  ; étape 6
    00 0E AF 00 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 47 BE 00 FF EE          ; dessin banque 0 frame 71 dy -66 dx -18
    00 48 CD 00 FF FB          ; dessin banque 0 frame 72 dy -51 dx -5
    00 49 DB 00 00 03          ; dessin banque 0 frame 73 dy -37 dx 3
    00 4A E3 00 00 18          ; dessin banque 0 frame 74 dy -29 dx 24
    00 4B F3 00 00 27          ; dessin banque 0 frame 75 dy -13 dx 39
    00 4C F2 00 00 42          ; dessin banque 0 frame 76 dy -14 dx 66
    00 4D EA 00 00 60          ; dessin banque 0 frame 77 dy -22 dx 96
    00 4E 1F 00 00 65          ; dessin banque 0 frame 78 dy 31 dx 101
    00 4F FE 00 00 A8          ; dessin banque 0 frame 79 dy -2 dx 168
    FF 00                      ; fin d'étape
  ; étape 7
    00 11 B6 00 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 33 C0 00 FF E7          ; dessin banque 0 frame 51 dy -64 dx -25
    00 34 CF 00 FF E9          ; dessin banque 0 frame 52 dy -49 dx -23
    00 35 D6 00 FF F6          ; dessin banque 0 frame 53 dy -42 dx -10
    00 36 F8 00 00 1E          ; dessin banque 0 frame 54 dy -8 dx 30
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 50 C0 00 FF EE          ; dessin banque 0 frame 80 dy -64 dx -18
    00 51 CE 00 00 01          ; dessin banque 0 frame 81 dy -50 dx 1
    00 52 E4 00 00 0D          ; dessin banque 0 frame 82 dy -28 dx 13
    00 53 EC 00 00 1F          ; dessin banque 0 frame 83 dy -20 dx 31
    00 54 EE 00 00 2C          ; dessin banque 0 frame 84 dy -18 dx 44
    00 55 F0 00 00 3C          ; dessin banque 0 frame 85 dy -16 dx 60
    00 56 EE 00 00 4D          ; dessin banque 0 frame 86 dy -18 dx 77
    00 57 E6 00 00 7C          ; dessin banque 0 frame 87 dy -26 dx 124
    00 58 F5 00 00 8B          ; dessin banque 0 frame 88 dy -11 dx 139
    00 59 1C 00 00 5B          ; dessin banque 0 frame 89 dy 28 dx 91
    00 5A 21 00 00 9D          ; dessin banque 0 frame 90 dy 33 dx 157
    00 5B 08 00 00 AC          ; dessin banque 0 frame 91 dy 8 dx 172
    FF FE                      ; fin d'étape, boucle
  ; étape 8
    B0 00 00 00 0B D2          ; $B0 Call -> LAB_0EF7
    84 00 00 00 64 7E          ; $84 Jump -> LAB_08B0
    00 14 B2 00 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19
    00 15 CF 00 FF F0          ; dessin banque 0 frame 21 dy -49 dx -16
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    00 30 DA 00 FF EC          ; dessin banque 0 frame 48 dy -38 dx -20
    00 31 F8 00 00 1C          ; dessin banque 0 frame 49 dy -8 dx 28
    00 32 FF 00 FF F4          ; dessin banque 0 frame 50 dy -1 dx -12
    00 1A CF 00 00 0A          ; dessin banque 0 frame 26 dy -49 dx 10
    00 5C FF 00 00 3D          ; dessin banque 0 frame 92 dy -1 dx 61
    00 5D FF 00 00 48          ; dessin banque 0 frame 93 dy -1 dx 72
    00 5E E2 00 00 48          ; dessin banque 0 frame 94 dy -30 dx 72
    00 5F F0 00 00 70          ; dessin banque 0 frame 95 dy -16 dx 112
    00 60 EB 00 00 7A          ; dessin banque 0 frame 96 dy -21 dx 122
    00 61 F6 00 00 8C          ; dessin banque 0 frame 97 dy -10 dx 140
    00 62 26 00 00 5E          ; dessin banque 0 frame 98 dy 38 dx 94
    FF 00                      ; fin d'étape
  LAB_08B6:
  ; étape 9
    08 00 E0 00 00 0D          ; dessin banque 2 frame 0 dy -32 dx 13
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F4          ; dessin banque 0 frame 15 dy -49 dx -12
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    FF 00                      ; fin d'étape
  ; étape 10
    08 01 D3 00 FF E0          ; dessin banque 2 frame 1 dy -45 dx -32
    08 02 DE 00 FF E0          ; dessin banque 2 frame 2 dy -34 dx -32
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F3          ; dessin banque 0 frame 18 dy -49 dx -13
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    FF 00                      ; fin d'étape
  ; étape 11
    08 03 C3 00 FF C6          ; dessin banque 2 frame 3 dy -61 dx -58
    08 04 C3 00 FF D9          ; dessin banque 2 frame 4 dy -61 dx -39
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F1          ; dessin banque 0 frame 21 dy -49 dx -15
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    FF 00                      ; fin d'étape
  ; étape 12
    08 05 C0 00 FF C7          ; dessin banque 2 frame 5 dy -64 dx -57
    08 06 C0 00 FF D1          ; dessin banque 2 frame 6 dy -64 dx -47
    00 19 E3 00 FF F1          ; dessin banque 0 frame 25 dy -29 dx -15
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    08 07 D3 00 00 14          ; dessin banque 2 frame 7 dy -45 dx 20
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 CF 00 FF F3          ; dessin banque 0 frame 24 dy -49 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 13
    08 08 C3 40 00 35          ; dessin banque 2 frame 8 dy -61 dx 53 [hors-boîte]
    08 09 D4 40 00 6F          ; dessin banque 2 frame 9 dy -44 dx 111 [hors-boîte]
    08 0A E7 40 00 19          ; dessin banque 2 frame 10 dy -25 dx 25 [hors-boîte]
    00 10 E5 00 FF F1          ; dessin banque 0 frame 16 dy -27 dx -15
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 0E B0 40 FF F3          ; dessin banque 0 frame 14 dy -80 dx -13 [hors-boîte]
    00 0F D0 00 FF F9          ; dessin banque 0 frame 15 dy -48 dx -7
    00 1A D0 01 00 10          ; dessin banque 0 frame 26 dy -48 dx 16 [corps]
    FF FF                      ; fin du script
```

### `LAB_08B6` (4:$68B4)

Rôles : référencé par le code dans LAB_0EDF

```
  ; étape 1
    08 00 E0 00 00 0D          ; dessin banque 2 frame 0 dy -32 dx 13
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F4          ; dessin banque 0 frame 15 dy -49 dx -12
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    FF 00                      ; fin d'étape
  ; étape 2
    08 01 D3 00 FF E0          ; dessin banque 2 frame 1 dy -45 dx -32
    08 02 DE 00 FF E0          ; dessin banque 2 frame 2 dy -34 dx -32
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 11 B6 40 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18 [hors-boîte]
    00 12 CF 00 FF F3          ; dessin banque 0 frame 18 dy -49 dx -13
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    FF 00                      ; fin d'étape
  ; étape 3
    08 03 C3 00 FF C6          ; dessin banque 2 frame 3 dy -61 dx -58
    08 04 C3 00 FF D9          ; dessin banque 2 frame 4 dy -61 dx -39
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 14 B2 40 FF ED          ; dessin banque 0 frame 20 dy -78 dx -19 [hors-boîte]
    00 15 CF 00 FF F1          ; dessin banque 0 frame 21 dy -49 dx -15
    00 16 E4 00 FF ED          ; dessin banque 0 frame 22 dy -28 dx -19
    FF 00                      ; fin d'étape
  ; étape 4
    08 05 C0 00 FF C7          ; dessin banque 2 frame 5 dy -64 dx -57
    08 06 C0 00 FF D1          ; dessin banque 2 frame 6 dy -64 dx -47
    00 19 E3 00 FF F1          ; dessin banque 0 frame 25 dy -29 dx -15
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    08 07 D3 00 00 14          ; dessin banque 2 frame 7 dy -45 dx 20
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 CF 00 FF F3          ; dessin banque 0 frame 24 dy -49 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 5
    08 08 C3 40 00 35          ; dessin banque 2 frame 8 dy -61 dx 53 [hors-boîte]
    08 09 D4 40 00 6F          ; dessin banque 2 frame 9 dy -44 dx 111 [hors-boîte]
    08 0A E7 40 00 19          ; dessin banque 2 frame 10 dy -25 dx 25 [hors-boîte]
    00 10 E5 00 FF F1          ; dessin banque 0 frame 16 dy -27 dx -15
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 0E B0 40 FF F3          ; dessin banque 0 frame 14 dy -80 dx -13 [hors-boîte]
    00 0F D0 00 FF F9          ; dessin banque 0 frame 15 dy -48 dx -7
    00 1A D0 01 00 10          ; dessin banque 0 frame 26 dy -48 dx 16 [corps]
    FF FF                      ; fin du script
```

### `LAB_08B7` (4:$69AE)

Rôles : référencé par le code dans LAB_0EE5

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 17 0A 00 00 4D          ; dessin banque 2 frame 23 dy 10 dx 77
    08 11 03 00 00 33          ; dessin banque 2 frame 17 dy 3 dx 51
    08 10 F6 00 00 12          ; dessin banque 2 frame 16 dy -10 dx 18
    00 16 E4 00 FF F2          ; dessin banque 0 frame 22 dy -28 dx -14
    00 21 E3 00 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12
    00 14 B3 00 FF F4          ; dessin banque 0 frame 20 dy -77 dx -12
    00 15 D0 00 FF F5          ; dessin banque 0 frame 21 dy -48 dx -11
    00 1A D0 00 00 10          ; dessin banque 0 frame 26 dy -48 dx 16
    FF FF                      ; fin du script
```

### `LAB_08B8` (4:$69E2)

Rôles : référencé par le code dans LAB_0EE1

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 1C 08 00 00 29          ; dessin banque 2 frame 28 dy 8 dx 41
    08 18 F8 40 00 0E          ; dessin banque 2 frame 24 dy -8 dx 14 [hors-boîte]
    00 19 E5 00 FF EF          ; dessin banque 0 frame 25 dy -27 dx -17
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 D0 00 FF F3          ; dessin banque 0 frame 24 dy -48 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    08 1D 19 00 FF F6          ; dessin banque 2 frame 29 dy 25 dx -10
    FF 00                      ; fin d'étape
  ; étape 2
    08 19 0A 40 FF E4          ; dessin banque 2 frame 25 dy 10 dx -28 [hors-boîte]
    08 18 F8 40 00 0E          ; dessin banque 2 frame 24 dy -8 dx 14 [hors-boîte]
    00 10 E5 00 FF EE          ; dessin banque 0 frame 16 dy -27 dx -18
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 0E B0 40 FF EF          ; dessin banque 0 frame 14 dy -80 dx -17 [hors-boîte]
    00 0F D0 00 FF F5          ; dessin banque 0 frame 15 dy -48 dx -11
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 3
    08 1A F7 00 00 0E          ; dessin banque 2 frame 26 dy -9 dx 14
    08 1B FE 40 00 24          ; dessin banque 2 frame 27 dy -2 dx 36 [hors-boîte]
    08 1C 0D 40 00 4F          ; dessin banque 2 frame 28 dy 13 dx 79 [hors-boîte]
    08 1D 1E 40 00 1A          ; dessin banque 2 frame 29 dy 30 dx 26 [hors-boîte]
    00 13 E5 00 FF F1          ; dessin banque 0 frame 19 dy -27 dx -15
    00 21 E3 01 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12 [corps]
    00 11 B7 40 FF F4          ; dessin banque 0 frame 17 dy -73 dx -12 [hors-boîte]
    00 12 D0 00 FF F8          ; dessin banque 0 frame 18 dy -48 dx -8
    00 1A D0 01 00 10          ; dessin banque 0 frame 26 dy -48 dx 16 [corps]
    FF FF                      ; fin du script
```

### `LAB_08B9` (4:$6A7A)

Rôles : référencé par le code dans LAB_0EE7

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 1E E5 00 00 10          ; dessin banque 2 frame 30 dy -27 dx 16
    08 1F E9 00 00 23          ; dessin banque 2 frame 31 dy -23 dx 35
    08 20 F3 00 00 56          ; dessin banque 2 frame 32 dy -13 dx 86
    08 1D 08 00 00 72          ; dessin banque 2 frame 29 dy 8 dx 114
    00 16 E5 00 FF F2          ; dessin banque 0 frame 22 dy -27 dx -14
    00 21 E3 00 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12
    00 14 B3 00 FF F2          ; dessin banque 0 frame 20 dy -77 dx -14
    00 15 D0 00 FF F5          ; dessin banque 0 frame 21 dy -48 dx -11
    00 1A D0 00 00 10          ; dessin banque 0 frame 26 dy -48 dx 16
    FF FF                      ; fin du script
```

### `LAB_08BA` (4:$6AB4)

Rôles : référencé par le code dans LAB_0EE3

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 23 C4 00 FF F8          ; dessin banque 2 frame 35 dy -60 dx -8
    08 22 B1 00 FF EC          ; dessin banque 2 frame 34 dy -79 dx -20
    08 24 BB 00 00 20          ; dessin banque 2 frame 36 dy -69 dx 32
    08 25 B1 00 FF F8          ; dessin banque 2 frame 37 dy -79 dx -8
    00 19 E5 00 FF EF          ; dessin banque 0 frame 25 dy -27 dx -17
    00 1E F8 00 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 D0 00 FF F3          ; dessin banque 0 frame 24 dy -48 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 2
    08 05 C0 00 FF C7          ; dessin banque 2 frame 5 dy -64 dx -57
    08 06 C0 00 FF D1          ; dessin banque 2 frame 6 dy -64 dx -47
    00 19 E3 00 FF F1          ; dessin banque 0 frame 25 dy -29 dx -15
    00 1E F8 40 00 1D          ; dessin banque 0 frame 30 dy -8 dx 29 [hors-boîte]
    08 07 D3 00 00 13          ; dessin banque 2 frame 7 dy -45 dx 19
    00 1D E2 01 FF F7          ; dessin banque 0 frame 29 dy -30 dx -9 [corps]
    00 17 B4 40 FF F0          ; dessin banque 0 frame 23 dy -76 dx -16 [hors-boîte]
    00 18 CF 00 FF F3          ; dessin banque 0 frame 24 dy -49 dx -13
    00 1A D0 01 00 0D          ; dessin banque 0 frame 26 dy -48 dx 13 [corps]
    FF 00                      ; fin d'étape
  ; étape 3
    00 1B E4 01 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6 [corps]
    00 1C F6 40 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26 [hors-boîte]
    00 1A CF 01 00 0C          ; dessin banque 0 frame 26 dy -49 dx 12 [corps]
    00 0E AF 40 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19 [hors-boîte]
    00 0F CF 00 FF F4          ; dessin banque 0 frame 15 dy -49 dx -12
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    FF FF                      ; fin du script
```

### `LAB_08BB` (4:$6B52)

Rôles : référencé par le code dans LAB_0EE7

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 1E E5 00 00 10          ; dessin banque 2 frame 30 dy -27 dx 16
    08 1F E9 00 00 23          ; dessin banque 2 frame 31 dy -23 dx 35
    08 20 F3 00 00 56          ; dessin banque 2 frame 32 dy -13 dx 86
    08 21 08 00 00 74          ; dessin banque 2 frame 33 dy 8 dx 116
    08 15 0B 00 00 80          ; dessin banque 2 frame 21 dy 11 dx 128
    08 14 F8 00 00 97          ; dessin banque 2 frame 20 dy -8 dx 151
    08 13 ED 00 00 8C          ; dessin banque 2 frame 19 dy -19 dx 140
    00 16 E5 00 FF F2          ; dessin banque 0 frame 22 dy -27 dx -14
    00 21 E3 00 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12
    00 14 B3 00 FF F2          ; dessin banque 0 frame 20 dy -77 dx -14
    00 15 D0 00 FF F5          ; dessin banque 0 frame 21 dy -48 dx -11
    00 1A D0 00 00 10          ; dessin banque 0 frame 26 dy -48 dx 16
    FF FF                      ; fin du script
```

### `LAB_08BC` (4:$6B9E)

Rôles : référencé par le code dans LAB_0EE5

```
  ; étape 1
    A4 3F                      ; $A4 Sound
    08 0B EC 00 00 24          ; dessin banque 2 frame 11 dy -20 dx 36
    08 0C FD 00 00 4D          ; dessin banque 2 frame 12 dy -3 dx 77
    08 0D 07 00 00 68          ; dessin banque 2 frame 13 dy 7 dx 104
    08 0E 13 00 00 87          ; dessin banque 2 frame 14 dy 19 dx 135
    00 3C F3 00 00 87          ; dessin banque 0 frame 60 dy -13 dx 135
    08 0F 14 00 00 91          ; dessin banque 2 frame 15 dy 20 dx 145
    00 12 D4 00 FF FF          ; dessin banque 0 frame 18 dy -44 dx -1
    00 25 06 00 FF FE          ; dessin banque 0 frame 37 dy 6 dx -2
    00 24 E4 00 FF EF          ; dessin banque 0 frame 36 dy -28 dx -17
    00 11 BB 00 FF FB          ; dessin banque 0 frame 17 dy -69 dx -5
    00 1A D4 00 00 17          ; dessin banque 0 frame 26 dy -44 dx 23
    FF 00                      ; fin d'étape
  ; étape 2
    08 14 F8 00 00 97          ; dessin banque 2 frame 20 dy -8 dx 151
    08 15 0B 00 00 80          ; dessin banque 2 frame 21 dy 11 dx 128
    08 13 ED 00 00 8C          ; dessin banque 2 frame 19 dy -19 dx 140
    08 12 0F 00 00 5D          ; dessin banque 2 frame 18 dy 15 dx 93
    08 11 04 00 00 39          ; dessin banque 2 frame 17 dy 4 dx 57
    08 10 F6 00 00 12          ; dessin banque 2 frame 16 dy -10 dx 18
    00 16 E4 00 FF F2          ; dessin banque 0 frame 22 dy -28 dx -14
    00 21 E3 00 FF F4          ; dessin banque 0 frame 33 dy -29 dx -12
    00 14 B3 00 FF F4          ; dessin banque 0 frame 20 dy -77 dx -12
    00 15 D0 00 FF F5          ; dessin banque 0 frame 21 dy -48 dx -11
    00 1A D0 00 00 10          ; dessin banque 0 frame 26 dy -48 dx 16
    FF FF                      ; fin du script
```

### `LAB_08BD` (4:$6C28)

Rôles : référencé par le code dans LAB_0EF2

```
  ; étape 1
    D0 00                      ; $D0 Reset
    B4 00 00 00 6C C0          ; $B4 IfDead -> LAB_08BE
    B0 00 00 00 0A AE          ; $B0 Call -> LAB_0EEF
    08 26 D0 20 FF EA          ; dessin banque 2 frame 38 dy -48 dx -22
    08 27 F7 20 FF F0          ; dessin banque 2 frame 39 dy -9 dx -16
    00 14 B3 00 FF ED          ; dessin banque 0 frame 20 dy -77 dx -19
    FF 00                      ; fin d'étape
  ; étape 2
    00 0E AF 00 FF ED          ; dessin banque 0 frame 14 dy -81 dx -19
    00 0F CF 00 FF F3          ; dessin banque 0 frame 15 dy -49 dx -13
    00 10 E4 00 FF EA          ; dessin banque 0 frame 16 dy -28 dx -22
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 1B E4 00 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    08 2B D2 20 00 13          ; dessin banque 2 frame 43 dy -46 dx 19
    08 2A FB 20 00 19          ; dessin banque 2 frame 42 dy -5 dx 25
    08 29 DD 20 FF F8          ; dessin banque 2 frame 41 dy -35 dx -8
    08 28 E5 20 FF E7          ; dessin banque 2 frame 40 dy -27 dx -25
    FF 00                      ; fin d'étape
  ; étape 3
    00 11 B6 00 FF EE          ; dessin banque 0 frame 17 dy -74 dx -18
    00 12 CF 00 FF F2          ; dessin banque 0 frame 18 dy -49 dx -14
    00 13 E4 00 FF EB          ; dessin banque 0 frame 19 dy -28 dx -21
    00 1A CF 00 00 0B          ; dessin banque 0 frame 26 dy -49 dx 11
    00 1B E4 00 FF FA          ; dessin banque 0 frame 27 dy -28 dx -6
    00 1C F6 00 00 1A          ; dessin banque 0 frame 28 dy -10 dx 26
    08 2D DB 20 FF ED          ; dessin banque 2 frame 45 dy -37 dx -19
    08 2E F8 20 00 0C          ; dessin banque 2 frame 46 dy -8 dx 12
    08 2C E3 20 FF E5          ; dessin banque 2 frame 44 dy -29 dx -27
    FF FF                      ; fin du script
  LAB_08BE:
  ; étape 4
    B0 00 00 00 0A 76          ; $B0 Call -> LAB_0EED
    B0 00 00 00 0A 84          ; $B0 Call -> LAB_0EEE
    88 02                      ; $88 Hold
    0C 00 BE 20 FF E9          ; dessin banque 3 frame 0 dy -66 dx -23
    0C 01 C7 20 FF E7          ; dessin banque 3 frame 1 dy -57 dx -25
    0C 02 E8 20 FF D4          ; dessin banque 3 frame 2 dy -24 dx -44
    0C 03 D2 20 FF EB          ; dessin banque 3 frame 3 dy -46 dx -21
    0C 04 28 20 FF DE          ; dessin banque 3 frame 4 dy 40 dx -34
    0C 05 27 20 FF FA          ; dessin banque 3 frame 5 dy 39 dx -6
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    0C 06 E2 20 FF D3          ; dessin banque 3 frame 6 dy -30 dx -45
    0C 07 C0 20 00 05          ; dessin banque 3 frame 7 dy -64 dx 5
    0C 08 DB 20 00 2B          ; dessin banque 3 frame 8 dy -37 dx 43
    0C 09 C8 20 FF ED          ; dessin banque 3 frame 9 dy -56 dx -19
    0C 0A F3 20 FF ED          ; dessin banque 3 frame 10 dy -13 dx -19
    0C 0B 1C 20 FF E5          ; dessin banque 3 frame 11 dy 28 dx -27
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    0C 0C BA 20 00 00          ; dessin banque 3 frame 12 dy -70 dx 0
    0C 0D C3 20 FF DF          ; dessin banque 3 frame 13 dy -61 dx -33
    0C 0E F5 20 FF E5          ; dessin banque 3 frame 14 dy -11 dx -27
    0C 0F 18 20 FF E9          ; dessin banque 3 frame 15 dy 24 dx -23
    FF 00                      ; fin d'étape
  ; étape 7
    88 02                      ; $88 Hold
    0C 11 D4 20 FF E3          ; dessin banque 3 frame 17 dy -44 dx -29
    0C 12 BF 20 FF EF          ; dessin banque 3 frame 18 dy -65 dx -17
    0C 13 B5 20 00 12          ; dessin banque 3 frame 19 dy -75 dx 18
    0C 10 04 20 FF FA          ; dessin banque 3 frame 16 dy 4 dx -6
    FF 00                      ; fin d'étape
  ; étape 8
    88 02                      ; $88 Hold
    0C 14 C5 20 00 09          ; dessin banque 3 frame 20 dy -59 dx 9
    0C 15 CC 20 FF E6          ; dessin banque 3 frame 21 dy -52 dx -26
    0C 16 E0 20 FF F8          ; dessin banque 3 frame 22 dy -32 dx -8
    0C 17 00 20 FF ED          ; dessin banque 3 frame 23 dy 0 dx -19
    0C 18 0F 20 FF FD          ; dessin banque 3 frame 24 dy 15 dx -3
    FF 00                      ; fin d'étape
  ; étape 9
    88 02                      ; $88 Hold
    0C 19 CC 20 FF E7          ; dessin banque 3 frame 25 dy -52 dx -25
    0C 1A C2 20 00 0D          ; dessin banque 3 frame 26 dy -62 dx 13
    FF 00                      ; fin d'étape
  ; étape 10
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08BE` (4:$6CC0)

Rôles : cible de saut depuis LAB_08BD

```
  ; étape 1
    B0 00 00 00 0A 76          ; $B0 Call -> LAB_0EED
    B0 00 00 00 0A 84          ; $B0 Call -> LAB_0EEE
    88 02                      ; $88 Hold
    0C 00 BE 20 FF E9          ; dessin banque 3 frame 0 dy -66 dx -23
    0C 01 C7 20 FF E7          ; dessin banque 3 frame 1 dy -57 dx -25
    0C 02 E8 20 FF D4          ; dessin banque 3 frame 2 dy -24 dx -44
    0C 03 D2 20 FF EB          ; dessin banque 3 frame 3 dy -46 dx -21
    0C 04 28 20 FF DE          ; dessin banque 3 frame 4 dy 40 dx -34
    0C 05 27 20 FF FA          ; dessin banque 3 frame 5 dy 39 dx -6
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    0C 06 E2 20 FF D3          ; dessin banque 3 frame 6 dy -30 dx -45
    0C 07 C0 20 00 05          ; dessin banque 3 frame 7 dy -64 dx 5
    0C 08 DB 20 00 2B          ; dessin banque 3 frame 8 dy -37 dx 43
    0C 09 C8 20 FF ED          ; dessin banque 3 frame 9 dy -56 dx -19
    0C 0A F3 20 FF ED          ; dessin banque 3 frame 10 dy -13 dx -19
    0C 0B 1C 20 FF E5          ; dessin banque 3 frame 11 dy 28 dx -27
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    0C 0C BA 20 00 00          ; dessin banque 3 frame 12 dy -70 dx 0
    0C 0D C3 20 FF DF          ; dessin banque 3 frame 13 dy -61 dx -33
    0C 0E F5 20 FF E5          ; dessin banque 3 frame 14 dy -11 dx -27
    0C 0F 18 20 FF E9          ; dessin banque 3 frame 15 dy 24 dx -23
    FF 00                      ; fin d'étape
  ; étape 4
    88 02                      ; $88 Hold
    0C 11 D4 20 FF E3          ; dessin banque 3 frame 17 dy -44 dx -29
    0C 12 BF 20 FF EF          ; dessin banque 3 frame 18 dy -65 dx -17
    0C 13 B5 20 00 12          ; dessin banque 3 frame 19 dy -75 dx 18
    0C 10 04 20 FF FA          ; dessin banque 3 frame 16 dy 4 dx -6
    FF 00                      ; fin d'étape
  ; étape 5
    88 02                      ; $88 Hold
    0C 14 C5 20 00 09          ; dessin banque 3 frame 20 dy -59 dx 9
    0C 15 CC 20 FF E6          ; dessin banque 3 frame 21 dy -52 dx -26
    0C 16 E0 20 FF F8          ; dessin banque 3 frame 22 dy -32 dx -8
    0C 17 00 20 FF ED          ; dessin banque 3 frame 23 dy 0 dx -19
    0C 18 0F 20 FF FD          ; dessin banque 3 frame 24 dy 15 dx -3
    FF 00                      ; fin d'étape
  ; étape 6
    88 02                      ; $88 Hold
    0C 19 CC 20 FF E7          ; dessin banque 3 frame 25 dy -52 dx -25
    0C 1A C2 20 00 0D          ; dessin banque 3 frame 26 dy -62 dx 13
    FF 00                      ; fin d'étape
  ; étape 7
    B0 00 00 00 03 80          ; $B0 Call -> LAB_0006
    BC 00                      ; $BC Kill
    FF FF                      ; fin du script
```

### `LAB_08BF` (4:$6D90)

Rôles : référencé par le code dans LAB_0F34

```
  ; étape 1
    10 00 E7 A0 FF EE          ; dessin banque 4 frame 0 dy -25 dx -18
    10 02 F3 A0 FF E1          ; dessin banque 4 frame 2 dy -13 dx -31
    10 03 F8 A0 FF EE          ; dessin banque 4 frame 3 dy -8 dx -18
    10 04 09 A0 FF E9          ; dessin banque 4 frame 4 dy 9 dx -23
    10 05 F2 A0 FF F9          ; dessin banque 4 frame 5 dy -14 dx -7
    10 06 00 A0 00 10          ; dessin banque 4 frame 6 dy 0 dx 16
    10 07 09 A0 00 05          ; dessin banque 4 frame 7 dy 9 dx 5
    10 01 E8 A0 00 0A          ; dessin banque 4 frame 1 dy -24 dx 10
    FF 00                      ; fin d'étape
  ; étape 2
    10 09 E2 A0 FF FD          ; dessin banque 4 frame 9 dy -30 dx -3
    10 0A DF A0 FF E8          ; dessin banque 4 frame 10 dy -33 dx -24
    10 0A D7 A0 FF FA          ; dessin banque 4 frame 10 dy -41 dx -6
    10 0B E0 A0 00 35          ; dessin banque 4 frame 11 dy -32 dx 53
    10 0C E1 A0 00 46          ; dessin banque 4 frame 12 dy -31 dx 70
    10 0D EE A0 FF F9          ; dessin banque 4 frame 13 dy -18 dx -7
    10 0E F7 A0 00 1A          ; dessin banque 4 frame 14 dy -9 dx 26
    10 0F 0D A0 00 0C          ; dessin banque 4 frame 15 dy 13 dx 12
    10 10 0B A0 FF EE          ; dessin banque 4 frame 16 dy 11 dx -18
    10 11 ED A0 FF F3          ; dessin banque 4 frame 17 dy -19 dx -13
    FF 00                      ; fin d'étape
  ; étape 3
    10 12 EB A0 FF ED          ; dessin banque 4 frame 18 dy -21 dx -19
    10 13 ED A0 FF FD          ; dessin banque 4 frame 19 dy -19 dx -3
    10 14 F4 A0 00 14          ; dessin banque 4 frame 20 dy -12 dx 20
    10 15 01 A0 00 3C          ; dessin banque 4 frame 21 dy 1 dx 60
    10 16 E8 A0 00 29          ; dessin banque 4 frame 22 dy -24 dx 41
    10 17 CF A0 FF DD          ; dessin banque 4 frame 23 dy -49 dx -35
    10 18 0D A0 00 4A          ; dessin banque 4 frame 24 dy 13 dx 74
    10 15 CE A0 FF F4          ; dessin banque 4 frame 21 dy -50 dx -12
    10 17 EC A0 00 43          ; dessin banque 4 frame 23 dy -20 dx 67
    10 18 F2 A0 00 5A          ; dessin banque 4 frame 24 dy -14 dx 90
    10 0B E5 A0 00 3E          ; dessin banque 4 frame 11 dy -27 dx 62
    10 10 E4 A0 00 14          ; dessin banque 4 frame 16 dy -28 dx 20
    10 16 D5 A0 00 1A          ; dessin banque 4 frame 22 dy -43 dx 26
    FF 00                      ; fin d'étape
  ; étape 4
    10 1A FB A0 00 10          ; dessin banque 4 frame 26 dy -5 dx 16
    10 1B 03 A0 00 29          ; dessin banque 4 frame 27 dy 3 dx 41
    10 18 D8 A0 FF F2          ; dessin banque 4 frame 24 dy -40 dx -14
    10 18 FE A0 FF F9          ; dessin banque 4 frame 24 dy -2 dx -7
    10 08 FB A0 00 6C          ; dessin banque 4 frame 8 dy -5 dx 108
    10 07 F5 A0 00 05          ; dessin banque 4 frame 7 dy -11 dx 5
    10 0C D1 A0 00 0A          ; dessin banque 4 frame 12 dy -47 dx 10
    10 0C D1 A0 FF DD          ; dessin banque 4 frame 12 dy -47 dx -35
    10 17 E0 A0 00 43          ; dessin banque 4 frame 23 dy -32 dx 67
    FF 00                      ; fin d'étape
  ; étape 5
    BC 00                      ; $BC Kill
    10 17 FA A0 FF FF          ; dessin banque 4 frame 23 dy -6 dx -1
    FF FF                      ; fin du script
```

### `LAB_08CC` (4:$6F82)

Rôles : référencé par le code dans LAB_0203 ; référencé par le code dans LAB_029C ; référencé par le code dans LAB_02A2 ; référencé par le code dans LAB_0EC9 ; référencé par le code dans LAB_0EDD

```
  ; étape 1
    00 1E 00 19 00 14          ; dessin banque 0 frame 30 dy 0 dx 20 [corps,décor]
    00 14 00 14 FF F9          ; dessin banque 0 frame 20 dy 0 dx -7 [décor]
    FF FD                      ; fin d'étape
    FF FF                      ; fin du script
```

### `LAB_08FD` (4:$7BA8)

Rôles : entrée 0 de la table LAB_08FC ; entrée 1 de la table LAB_08FC

```
  ; étape 1
    00 22 FB 00 FF EF          ; dessin banque 0 frame 34 dy -5 dx -17
    FF FF                      ; fin du script
```

### `LAB_08FE` (4:$7BB0)

Rôles : entrée 2 de la table LAB_08FC ; entrée 3 de la table LAB_08FC

```
  ; étape 1
    00 23 F8 00 FF EF          ; dessin banque 0 frame 35 dy -8 dx -17
    FF FF                      ; fin du script
```

### `LAB_08FF` (4:$7BB8)

Rôles : entrée 4 de la table LAB_08FC ; entrée 5 de la table LAB_08FC

```
  ; étape 1
    00 24 F6 00 FF EF          ; dessin banque 0 frame 36 dy -10 dx -17
    FF FF                      ; fin du script
```

### `LAB_0900` (4:$7BC0)

Rôles : entrée 6 de la table LAB_08FC ; entrée 7 de la table LAB_08FC ; référencé par le code dans LAB_0DD0

```
  ; étape 1
    00 25 F7 00 FF EF          ; dessin banque 0 frame 37 dy -9 dx -17
    FF FF                      ; fin du script
```

### `LAB_0901` (4:$7BC8)

Rôles : entrée 8 de la table LAB_08FC ; entrée 9 de la table LAB_08FC

```
  ; étape 1
    00 26 FA 00 FF EF          ; dessin banque 0 frame 38 dy -6 dx -17
    FF FF                      ; fin du script
```

### `LAB_0902` (4:$7BD0)

Rôles : entrée 10 de la table LAB_08FC ; entrée 11 de la table LAB_08FC

```
  ; étape 1
    00 27 FA 00 FF EF          ; dessin banque 0 frame 39 dy -6 dx -17
    FF FF                      ; fin du script
```

### `LAB_0903` (4:$7BD8)

Rôles : entrée 12 de la table LAB_08FC ; entrée 13 de la table LAB_08FC

```
  ; étape 1
    00 28 FB 00 FF EF          ; dessin banque 0 frame 40 dy -5 dx -17
    FF FF                      ; fin du script
```

### `LAB_0904` (4:$7BE0)

Rôles : entrée 14 de la table LAB_08FC ; entrée 15 de la table LAB_08FC

```
  ; étape 1
    00 29 FC 00 FF EF          ; dessin banque 0 frame 41 dy -4 dx -17
    FF FF                      ; fin du script
```

### `LAB_0975` (4:$8F20)

Rôles : référencé par le code dans LAB_049B

```
  ; étape 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
  LAB_0978:
    94 0A                      ; $94 Loop
    00 05 CD 00 FF C2          ; dessin banque 0 frame 5 dy -51 dx -62
    00 06 C9 00 00 0E          ; dessin banque 0 frame 6 dy -55 dx 14
    00 07 E5 00 FF F0          ; dessin banque 0 frame 7 dy -27 dx -16
    00 08 1D 00 FF 78          ; dessin banque 0 frame 8 dy 29 dx -136
    00 09 1F 00 00 26          ; dessin banque 0 frame 9 dy 31 dx 38
    FF 00                      ; fin d'étape
  ; étape 2
    00 0A CD 00 FF C2          ; dessin banque 0 frame 10 dy -51 dx -62
    00 0B C9 00 00 0E          ; dessin banque 0 frame 11 dy -55 dx 14
    00 0C E5 00 FF F0          ; dessin banque 0 frame 12 dy -27 dx -16
    00 0D 1D 00 FF 78          ; dessin banque 0 frame 13 dy 29 dx -136
    00 0E 1F 00 00 25          ; dessin banque 0 frame 14 dy 31 dx 37
    FF FE                      ; fin d'étape, boucle
  ; étape 3
    00 05 CD 00 FF C2          ; dessin banque 0 frame 5 dy -51 dx -62
    00 06 C9 00 00 0E          ; dessin banque 0 frame 6 dy -55 dx 14
    00 07 E5 00 FF F0          ; dessin banque 0 frame 7 dy -27 dx -16
    00 08 1D 00 FF 78          ; dessin banque 0 frame 8 dy 29 dx -136
    00 09 1F 00 00 26          ; dessin banque 0 frame 9 dy 31 dx 38
    00 04 D1 00 FF ED          ; dessin banque 0 frame 4 dy -47 dx -19
    FF 00                      ; fin d'étape
  ; étape 4
    84 00 00 00 8F 2C          ; $84 Jump -> LAB_0978
    00 0A CD 00 FF C2          ; dessin banque 0 frame 10 dy -51 dx -62
    00 0B C9 00 00 0E          ; dessin banque 0 frame 11 dy -55 dx 14
    00 0C E5 00 FF F0          ; dessin banque 0 frame 12 dy -27 dx -16
    00 0D 1D 00 FF 78          ; dessin banque 0 frame 13 dy 29 dx -136
    00 0E 1F 00 00 25          ; dessin banque 0 frame 14 dy 31 dx 37
    00 04 D1 00 FF ED          ; dessin banque 0 frame 4 dy -47 dx -19
    FF FF                      ; fin du script
```

### `LAB_0978` (4:$8F2C)

Rôles : cible de saut depuis LAB_0975 ; cible de saut depuis LAB_0978 ; référencé par le code dans LAB_0457

```
  ; étape 1
    94 0A                      ; $94 Loop
    00 05 CD 00 FF C2          ; dessin banque 0 frame 5 dy -51 dx -62
    00 06 C9 00 00 0E          ; dessin banque 0 frame 6 dy -55 dx 14
    00 07 E5 00 FF F0          ; dessin banque 0 frame 7 dy -27 dx -16
    00 08 1D 00 FF 78          ; dessin banque 0 frame 8 dy 29 dx -136
    00 09 1F 00 00 26          ; dessin banque 0 frame 9 dy 31 dx 38
    FF 00                      ; fin d'étape
  ; étape 2
    00 0A CD 00 FF C2          ; dessin banque 0 frame 10 dy -51 dx -62
    00 0B C9 00 00 0E          ; dessin banque 0 frame 11 dy -55 dx 14
    00 0C E5 00 FF F0          ; dessin banque 0 frame 12 dy -27 dx -16
    00 0D 1D 00 FF 78          ; dessin banque 0 frame 13 dy 29 dx -136
    00 0E 1F 00 00 25          ; dessin banque 0 frame 14 dy 31 dx 37
    FF FE                      ; fin d'étape, boucle
  ; étape 3
    00 05 CD 00 FF C2          ; dessin banque 0 frame 5 dy -51 dx -62
    00 06 C9 00 00 0E          ; dessin banque 0 frame 6 dy -55 dx 14
    00 07 E5 00 FF F0          ; dessin banque 0 frame 7 dy -27 dx -16
    00 08 1D 00 FF 78          ; dessin banque 0 frame 8 dy 29 dx -136
    00 09 1F 00 00 26          ; dessin banque 0 frame 9 dy 31 dx 38
    00 04 D1 00 FF ED          ; dessin banque 0 frame 4 dy -47 dx -19
    FF 00                      ; fin d'étape
  ; étape 4
    84 00 00 00 8F 2C          ; $84 Jump -> LAB_0978
    00 0A CD 00 FF C2          ; dessin banque 0 frame 10 dy -51 dx -62
    00 0B C9 00 00 0E          ; dessin banque 0 frame 11 dy -55 dx 14
    00 0C E5 00 FF F0          ; dessin banque 0 frame 12 dy -27 dx -16
    00 0D 1D 00 FF 78          ; dessin banque 0 frame 13 dy 29 dx -136
    00 0E 1F 00 00 25          ; dessin banque 0 frame 14 dy 31 dx 37
    00 04 D1 00 FF ED          ; dessin banque 0 frame 4 dy -47 dx -19
    FF FF                      ; fin du script
```

### `LAB_0979` (4:$8FC0)

Rôles : cible de saut depuis LAB_0979 ; référencé par le code dans LAB_0457

```
  ; étape 1
    00 00 9D 00 FF 61          ; dessin banque 0 frame 0 dy -99 dx -159
    FF 00                      ; fin d'étape
  ; étape 2
    00 01 9D 00 FF 61          ; dessin banque 0 frame 1 dy -99 dx -159
    FF 00                      ; fin d'étape
  ; étape 3
    84 00 00 00 8F C0          ; $84 Jump -> LAB_0979
    00 02 9F 00 FF 61          ; dessin banque 0 frame 2 dy -97 dx -159
    FF FF                      ; fin du script
```

### `LAB_097A` (4:$8FDE)

Rôles : cible de saut depuis LAB_097A ; référencé par le code dans LAB_0456 ; référencé par le code dans LAB_0458

```
  ; étape 1
    00 0F 29 00 00 5D          ; dessin banque 0 frame 15 dy 41 dx 93
    00 10 5C 00 00 36          ; dessin banque 0 frame 16 dy 92 dx 54
    FF 00                      ; fin d'étape
  ; étape 2
    00 11 29 00 00 5C          ; dessin banque 0 frame 17 dy 41 dx 92
    00 12 5B 00 00 2B          ; dessin banque 0 frame 18 dy 91 dx 43
    FF 00                      ; fin d'étape
  ; étape 3
    84 00 00 00 8F DE          ; $84 Jump -> LAB_097A
    00 13 29 00 00 59          ; dessin banque 0 frame 19 dy 41 dx 89
    00 14 5A 00 00 2A          ; dessin banque 0 frame 20 dy 90 dx 42
    FF FF                      ; fin du script
```

### `LAB_0D44` (31:$35A)

Rôles : entrée 0 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 07 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps,frappe]
    00 07 FF FF 00 00          ; dessin banque 0 frame 7 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 03 00 00 00 03          ; dessin banque 0 frame 3 dy 0 dx 3
    00 00 00 03 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps,frappe]
    00 03 FF FF 00 00          ; dessin banque 0 frame 3 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 02 00 00 00 02          ; dessin banque 0 frame 2 dy 0 dx 2
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 02 00 00 00 02          ; dessin banque 0 frame 2 dy 0 dx 2
    00 00 00 01 FF FF          ; dessin banque 0 frame 0 dy 0 dx -1 [corps]
  LAB_0D47:
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 FF FF 00 00          ; dessin banque 0 frame 1 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0D45` (31:$364)

Rôles : entrée 1 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 03 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps,frappe]
    00 03 00 00 00 03          ; dessin banque 0 frame 3 dy 0 dx 3
    00 00 00 03 FF FF          ; dessin banque 0 frame 0 dy 0 dx -1 [corps,frappe]
  LAB_0D46:
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0D46` (31:$376)

Rôles : entrée 2 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0D47` (31:$390)

Rôles : entrée 3 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 FF FF 00 00          ; dessin banque 0 frame 1 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0D48` (31:$3B2)

Rôles : entrée 4 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 00          ; dessin banque 0 frame 1 dy 0 dx 0
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 01 00 00 00 01          ; dessin banque 0 frame 1 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 FF FF 00 00          ; dessin banque 0 frame 1 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 00 00 00 00          ; dessin banque 0 frame 1 dy 0 dx 0
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0D49` (31:$3D4)

Rôles : entrée 5 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 00 00 00 00          ; dessin banque 0 frame 1 dy 0 dx 0
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 FF FF 00 00          ; dessin banque 0 frame 1 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 01 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps]
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    FF FF                      ; fin du script
```

### `LAB_0D4A` (31:$3F6)

Rôles : entrée 6 de la table LAB_0D43+146

```
  ; étape 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 01 00 00 00 00          ; dessin banque 0 frame 1 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 01          ; dessin banque 0 frame 0 dy 0 dx 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 FF FF 00 00          ; dessin banque 0 frame 0 dy -1 dx 0 [corps,frappe,décor,hors-boîte]
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    FF FF                      ; fin du script
```

### `LAB_0F54` (42:$116)

Rôles : référencé par le code dans LAB_04AB ; référencé par le code dans LAB_04AD

```
  ; étape 1
    00 06 02 00 FF D7          ; dessin banque 0 frame 6 dy 2 dx -41
    FF 00                      ; fin d'étape
  ; étape 2
    00 07 02 00 FF D7          ; dessin banque 0 frame 7 dy 2 dx -41
    FF FF                      ; fin du script
```

### `LAB_0F55` (42:$126)

Rôles : référencé par le code dans LAB_04B0

```
  ; étape 1
    00 08 F9 00 FF D6          ; dessin banque 0 frame 8 dy -7 dx -42
    FF 00                      ; fin d'étape
  ; étape 2
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0A F7 00 FF D9          ; dessin banque 0 frame 10 dy -9 dx -39
    FF 00                      ; fin d'étape
  ; étape 3
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0B F4 00 FF D4          ; dessin banque 0 frame 11 dy -12 dx -44
    FF 00                      ; fin d'étape
  ; étape 4
    B0 00 00 00 A8 C2          ; $B0 Call -> LAB_04BA
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0C F3 00 FF CC          ; dessin banque 0 frame 12 dy -13 dx -52
    FF 00                      ; fin d'étape
  ; étape 5
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0D F8 00 FF C9          ; dessin banque 0 frame 13 dy -8 dx -55
    FF 00                      ; fin d'étape
  ; étape 6
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0E F3 00 FF C7          ; dessin banque 0 frame 14 dy -13 dx -57
    FF 00                      ; fin d'étape
  ; étape 7
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 0F F0 00 FF C3          ; dessin banque 0 frame 15 dy -16 dx -61
    FF 00                      ; fin d'étape
  ; étape 8
    B0 00 00 00 A8 C2          ; $B0 Call -> LAB_04BA
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 10 F0 00 FF BF          ; dessin banque 0 frame 16 dy -16 dx -65
    FF 00                      ; fin d'étape
  ; étape 9
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 11 F2 00 FF BC          ; dessin banque 0 frame 17 dy -14 dx -68
    FF 00                      ; fin d'étape
  ; étape 10
    B0 00 00 00 A8 C2          ; $B0 Call -> LAB_04BA
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 12 EE 00 FF B8          ; dessin banque 0 frame 18 dy -18 dx -72
    FF 00                      ; fin d'étape
  ; étape 11
    B0 00 00 00 A8 C2          ; $B0 Call -> LAB_04BA
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 13 EE 00 FF B5          ; dessin banque 0 frame 19 dy -18 dx -75
    FF 00                      ; fin d'étape
  ; étape 12
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 14 ED 00 FF B6          ; dessin banque 0 frame 20 dy -19 dx -74
    FF 00                      ; fin d'étape
  ; étape 13
    B0 00 00 00 A8 C2          ; $B0 Call -> LAB_04BA
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 15 EE 00 FF B2          ; dessin banque 0 frame 21 dy -18 dx -78
    FF 00                      ; fin d'étape
  ; étape 14
    88 0A                      ; $88 Hold
    00 09 F6 00 FF DA          ; dessin banque 0 frame 9 dy -10 dx -38
    00 16 EF 00 FF B1          ; dessin banque 0 frame 22 dy -17 dx -79
    FF FF                      ; fin du script
```

### `LAB_0F56` (42:$204)

Rôles : référencé par le code dans LAB_04BF

```
  ; étape 1
    88 1E                      ; $88 Hold
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 02 EB 20 FF EC          ; dessin banque 0 frame 2 dy -21 dx -20
    00 00 DA 20 FF F1          ; dessin banque 0 frame 0 dy -38 dx -15
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 03 EB 20 FF EC          ; dessin banque 0 frame 3 dy -21 dx -20
    00 00 DA 20 FF F1          ; dessin banque 0 frame 0 dy -38 dx -15
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 00 DA 20 FF F1          ; dessin banque 0 frame 0 dy -38 dx -15
    00 04 E6 20 FF EC          ; dessin banque 0 frame 4 dy -26 dx -20
    FF 00                      ; fin d'étape
  ; étape 4
    88 05                      ; $88 Hold
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 05 DE 20 FF EB          ; dessin banque 0 frame 5 dy -34 dx -21
    FF 00                      ; fin d'étape
  ; étape 5
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 6
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 7
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    94 05                      ; $94 Loop
    A4 9D                      ; $A4 Sound
    B0 00 00 00 AA A8          ; $B0 Call -> LAB_04C2
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    00 19 9C 00 FF EC          ; dessin banque 0 frame 25 dy -100 dx -20
    FF 00                      ; fin d'étape
  ; étape 9
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    00 1A 9C 00 FF F1          ; dessin banque 0 frame 26 dy -100 dx -15
    FF 00                      ; fin d'étape
  ; étape 10
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    00 1B 9C 00 FF F7          ; dessin banque 0 frame 27 dy -100 dx -9
    FF 00                      ; fin d'étape
  ; étape 11
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    00 19 9C 00 FF EB          ; dessin banque 0 frame 25 dy -100 dx -21
    FF 00                      ; fin d'étape
  ; étape 12
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 06 D8 20 FF EF          ; dessin banque 0 frame 6 dy -40 dx -17
    00 1B 9C 00 FF F7          ; dessin banque 0 frame 27 dy -100 dx -9
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 00 DA 20 FF F1          ; dessin banque 0 frame 0 dy -38 dx -15
    00 04 E6 20 FF EC          ; dessin banque 0 frame 4 dy -26 dx -20
    FF 00                      ; fin d'étape
  ; étape 14
    88 14                      ; $88 Hold
    00 01 F2 20 FF EC          ; dessin banque 0 frame 1 dy -14 dx -20
    00 02 EB 20 FF EC          ; dessin banque 0 frame 2 dy -21 dx -20
    00 00 DA 20 FF F1          ; dessin banque 0 frame 0 dy -38 dx -15
    FF FF                      ; fin du script
```

### `LAB_0F57` (42:$318)

Rôles : cible de saut depuis LAB_0F57 ; référencé par le code dans LAB_04BF

```
  ; étape 1
    00 07 F3 20 00 65          ; dessin banque 0 frame 7 dy -13 dx 101
    00 0A EF 20 FF 84          ; dessin banque 0 frame 10 dy -17 dx -124
    00 0D FE 20 00 44          ; dessin banque 0 frame 13 dy -2 dx 68
    00 0E FC 20 FF AA          ; dessin banque 0 frame 14 dy -4 dx -86
    00 10 03 20 00 29          ; dessin banque 0 frame 16 dy 3 dx 41
    00 11 01 20 FF C5          ; dessin banque 0 frame 17 dy 1 dx -59
    00 14 26 20 00 4D          ; dessin banque 0 frame 20 dy 38 dx 77
    00 15 27 20 FF 9F          ; dessin banque 0 frame 21 dy 39 dx -97
    00 16 17 20 00 2A          ; dessin banque 0 frame 22 dy 23 dx 42
    00 18 19 20 FF C5          ; dessin banque 0 frame 24 dy 25 dx -59
    FF 00                      ; fin d'étape
  ; étape 2
    00 08 F2 20 00 63          ; dessin banque 0 frame 8 dy -14 dx 99
    00 0B EC 20 FF 84          ; dessin banque 0 frame 11 dy -20 dx -124
    00 0E FD 20 00 42          ; dessin banque 0 frame 14 dy -3 dx 66
    00 0F FB 20 FF AB          ; dessin banque 0 frame 15 dy -5 dx -85
    00 11 01 20 00 28          ; dessin banque 0 frame 17 dy 1 dx 40
    00 12 00 20 FF C6          ; dessin banque 0 frame 18 dy 0 dx -58
    00 13 26 20 FF 9F          ; dessin banque 0 frame 19 dy 38 dx -97
    00 15 27 20 00 4D          ; dessin banque 0 frame 21 dy 39 dx 77
    00 17 16 20 00 2A          ; dessin banque 0 frame 23 dy 22 dx 42
    00 16 17 20 FF C5          ; dessin banque 0 frame 22 dy 23 dx -59
    FF 00                      ; fin d'étape
  ; étape 3
    84 00 00 00 03 18          ; $84 Jump -> LAB_0F57
    00 09 EF 20 00 65          ; dessin banque 0 frame 9 dy -17 dx 101
    00 0C F3 20 FF 84          ; dessin banque 0 frame 12 dy -13 dx -124
    00 0F FC 20 00 44          ; dessin banque 0 frame 15 dy -4 dx 68
    00 0D FD 20 FF AC          ; dessin banque 0 frame 13 dy -3 dx -84
    00 12 00 20 00 29          ; dessin banque 0 frame 18 dy 0 dx 41
    00 10 03 20 FF C5          ; dessin banque 0 frame 16 dy 3 dx -59
    00 13 26 20 00 4D          ; dessin banque 0 frame 19 dy 38 dx 77
    00 14 26 20 FF 9F          ; dessin banque 0 frame 20 dy 38 dx -97
    00 18 19 20 00 2A          ; dessin banque 0 frame 24 dy 25 dx 42
    00 17 16 20 FF C5          ; dessin banque 0 frame 23 dy 22 dx -59
    FF FF                      ; fin du script
```

### `L44_00C70` (44:$C70)

Rôles : SECSTRT_44 [table SECSTRT_44] : [6] ; entrée 0 de la table LAB_0F8E ; référencé par le code dans LAB_0F8A

```
  ; étape 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
  LAB_0FCA:
    FF FF                      ; fin du script
```

### `LAB_10A2` (45:$A8)

Rôles : référencé par le code dans LAB_0F8D ; référencé par le code dans LAB_0F90 ; référencé par le code dans LAB_0F94 ; référencé par le code dans LAB_0FBC

```
    FF FF                      ; fin du script
```

### `LAB_10AE` (45:$7D2)

Rôles : référencé par le code dans LAB_0FDE

```
  ; étape 1
    00 00 00 00 00 00          ; dessin banque 0 frame 0 dy 0 dx 0
    FF FF                      ; fin du script
```

