# Scripts IMAGEXCEL de `program`

Généré par `tools/ix_scripts.py` à partir du binaire et de ses relocations ; ne pas modifier à la main.

- Moteur : intro/fin (`LAB_0288`)
- Scripts identifiés : **36** ; tables de scripts : **5**
- Pointeurs candidats rejetés (données d'un autre type) : 366

## Tables de scripts

| Table | Adresse | Entrées | Rôle |
|---|---|---|---|
| `LAB_0023` | 0:$560 | 4 | code : LAB_001A |
| `LAB_0024` | 0:$570 | 20 | code : LAB_001B |
| `LAB_0025` | 0:$5C0 | 16 | code : LAB_001C |
| `LAB_003A` | 0:$D06 | 10 | code : LAB_0037 |
| `LAB_051E+146` | 26:$33A | 8 | — |

## Tables de scripts remplies par le code

Tables en mémoire (souvent en BSS) que le code remplit avec `MOVE.L #script,d(An)` ; les champs de l'objet (30, 34, 46…) les désignent.


## Scripts

| Script | Adresse | Octets | Étapes | Rôles |
|---|---|---|---|---|
| `LAB_00D2` | 2:$248 | 438 | 7 | référencé par le code dans LAB_002C |
| `LAB_00D3` | 2:$3FE | 1782 | 13 | référencé par le code dans LAB_002E |
| `LAB_00D4` | 2:$AF4 | 1328 | 14 | référencé par le code dans LAB_002C |
| `LAB_00D5` | 2:$1024 | 800 | 10 | référencé par le code dans LAB_0039 |
| `LAB_00D6` | 2:$1344 | 124 | 7 | référencé par le code dans LAB_002D |
| `LAB_00D7` | 2:$13C0 | 580 | 23 | entrée 0 de la table LAB_0025<br>entrée 1 de la table LAB_0025<br>entrée 10 de la table LAB_0025<br>entrée 11 de la table LAB_0025<br>entrée 12 de la table LAB_0025<br>entrée 13 de la table LAB_0025 … |
| `LAB_00D8` | 2:$1604 | 748 | 38 | entrée 0 de la table LAB_0024<br>entrée 1 de la table LAB_0024<br>entrée 10 de la table LAB_0024<br>entrée 11 de la table LAB_0024<br>entrée 12 de la table LAB_0024<br>entrée 13 de la table LAB_0024 … |
| `LAB_00D9` | 2:$19C2 | 402 | 28 | référencé par le code dans LAB_0030 |
| `LAB_00DA` | 2:$1AA2 | 178 | 12 | référencé par le code dans LAB_0030 |
| `LAB_00DB` | 2:$1B2E | 38 | 2 | cible de saut depuis LAB_00D9<br>cible de saut depuis LAB_00DA<br>cible de saut depuis LAB_00DB<br>référencé par le code dans LAB_0031 |
| `LAB_00DC` | 2:$1B54 | 38 | 2 | cible de saut depuis LAB_00DC<br>référencé par le code dans LAB_0030<br>référencé par le code dans LAB_0031 |
| `LAB_00DD` | 2:$1B7A | 44 | 2 | cible de saut depuis LAB_00DD<br>référencé par le code dans LAB_0030<br>référencé par le code dans LAB_0031 |
| `LAB_00DE` | 2:$1BA6 | 514 | 36 | entrée 0 de la table LAB_0023<br>entrée 1 de la table LAB_0023 |
| `LAB_00DF` | 2:$1D82 | 38 | 2 | cible de saut depuis LAB_00DE<br>cible de saut depuis LAB_00DF<br>référencé par le code dans LAB_0031 |
| `LAB_00E0` | 2:$1DA8 | 388 | 27 | entrée 2 de la table LAB_0023<br>entrée 3 de la table LAB_0023 |
| `LAB_00E1` | 2:$1F06 | 38 | 2 | cible de saut depuis LAB_00E0<br>cible de saut depuis LAB_00E1<br>référencé par le code dans LAB_0031 |
| `LAB_00E2` | 2:$2052 | 38 | 2 | cible de saut depuis LAB_00E2 |
| `LAB_00E3` | 2:$2078 | 256 | 31 | référencé par le code dans LAB_001A |
| `LAB_00E4` | 2:$2178 | 14 | 1 | cible de saut depuis LAB_00E4<br>référencé par le code dans LAB_002F |
| `LAB_00E5` | 2:$2186 | 124 | 15 | référencé par le code dans LAB_002F |
| `LAB_00E6` | 2:$221C | 162 | 5 | référencé par le code dans LAB_0036 |
| `LAB_00E7` | 2:$22BE | 878 | 40 | entrée 0 de la table LAB_003A<br>entrée 1 de la table LAB_003A<br>entrée 2 de la table LAB_003A<br>entrée 3 de la table LAB_003A<br>entrée 4 de la table LAB_003A<br>entrée 5 de la table LAB_003A … |
| `LAB_00E8` | 2:$262C | 268 | 14 | référencé par le code dans LAB_0038 |
| `LAB_00E9` | 2:$2738 | 192 | 3 | cible de saut depuis LAB_00E9<br>référencé par le code dans LAB_0037<br>référencé par le code dans LAB_0038<br>référencé par le code dans LAB_0039 |
| `LAB_00EA` | 2:$27F8 | 368 | 16 | cible de saut depuis LAB_00EA<br>référencé par le code dans LAB_0037 |
| `LAB_00EB` | 2:$2812 | 342 | 15 | référencé par le code dans LAB_0039 |
| `LAB_00EC` | 2:$2968 | 118 | 14 | référencé par le code dans LAB_0039 |
| `LAB_00ED` | 2:$29DE | 240 | 26 | référencé par le code dans LAB_0036 |
| `LAB_00EE` | 2:$2ACE | 760 | 11 | référencé par le code dans LAB_003B |
| `LAB_051F` | 26:$35A | 122 | 1 | entrée 0 de la table LAB_051E+146 |
| `LAB_0520` | 26:$364 | 44 | 1 | entrée 1 de la table LAB_051E+146 |
| `LAB_0521` | 26:$376 | 26 | 1 | entrée 2 de la table LAB_051E+146 |
| `LAB_0522` | 26:$390 | 68 | 1 | entrée 3 de la table LAB_051E+146 |
| `LAB_0523` | 26:$3B2 | 68 | 1 | entrée 4 de la table LAB_051E+146 |
| `LAB_0524` | 26:$3D4 | 68 | 1 | entrée 5 de la table LAB_051E+146 |
| `LAB_0525` | 26:$3F6 | 68 | 1 | entrée 6 de la table LAB_051E+146 |

## Décodage

### `LAB_00D2` (2:$248)

Rôles : référencé par le code dans LAB_002C

```
  ; étape 1
    88 0A                      ; $88 Hold
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 01 EF 00 FF DA          ; dessin banque 0 frame 1 dy -17 dx -38
    00 02 FD 00 FF CD          ; dessin banque 0 frame 2 dy -3 dx -51
    00 03 EF 00 00 06          ; dessin banque 0 frame 3 dy -17 dx 6
    00 04 FF 00 00 1A          ; dessin banque 0 frame 4 dy -1 dx 26
    00 05 12 00 00 2C          ; dessin banque 0 frame 5 dy 18 dx 44
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 06 F0 00 FF D1          ; dessin banque 0 frame 6 dy -16 dx -47
    00 07 03 00 FF C1          ; dessin banque 0 frame 7 dy 3 dx -63
    00 08 19 00 FF C4          ; dessin banque 0 frame 8 dy 25 dx -60
    00 09 F1 00 00 07          ; dessin banque 0 frame 9 dy -15 dx 7
    00 0A FD 00 00 1B          ; dessin banque 0 frame 10 dy -3 dx 27
    00 0B 08 00 00 1B          ; dessin banque 0 frame 11 dy 8 dx 27
    00 0C 0D 00 00 1C          ; dessin banque 0 frame 12 dy 13 dx 28
    FF 00                      ; fin d'étape
  ; étape 3
    88 01                      ; $88 Hold
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 0D 00 00 FF B4          ; dessin banque 0 frame 13 dy 0 dx -76
    00 0E F5 00 FF C4          ; dessin banque 0 frame 14 dy -11 dx -60
    00 0F EF 00 FF DA          ; dessin banque 0 frame 15 dy -17 dx -38
    00 10 F1 00 00 07          ; dessin banque 0 frame 16 dy -15 dx 7
    00 11 F6 00 00 16          ; dessin banque 0 frame 17 dy -10 dx 22
    00 12 FB 00 00 2F          ; dessin banque 0 frame 18 dy -5 dx 47
    FF 00                      ; fin d'étape
  ; étape 4
    88 01                      ; $88 Hold
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 13 E9 00 FF 9A          ; dessin banque 0 frame 19 dy -23 dx -102
    00 14 02 00 FF A3          ; dessin banque 0 frame 20 dy 2 dx -93
    00 15 EE 00 FF B1          ; dessin banque 0 frame 21 dy -18 dx -79
    00 16 02 00 FF B2          ; dessin banque 0 frame 22 dy 2 dx -78
    00 17 EE 00 00 07          ; dessin banque 0 frame 23 dy -18 dx 7
    00 18 DA 00 00 4B          ; dessin banque 0 frame 24 dy -38 dx 75
    00 19 EA 00 00 3E          ; dessin banque 0 frame 25 dy -22 dx 62
    00 1A 02 00 00 1F          ; dessin banque 0 frame 26 dy 2 dx 31
    FF 00                      ; fin d'étape
  ; étape 5
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 1B D0 00 FF 96          ; dessin banque 0 frame 27 dy -48 dx -106
    00 1C DF 00 FF 9B          ; dessin banque 0 frame 28 dy -33 dx -101
    00 1D DF 00 FF A2          ; dessin banque 0 frame 29 dy -33 dx -94
    00 1E DF 00 FF B1          ; dessin banque 0 frame 30 dy -33 dx -79
    00 1F E1 00 FF B9          ; dessin banque 0 frame 31 dy -31 dx -71
    00 20 04 00 FF BC          ; dessin banque 0 frame 32 dy 4 dx -68
    00 21 EE 00 FF D8          ; dessin banque 0 frame 33 dy -18 dx -40
    00 22 EC 00 00 07          ; dessin banque 0 frame 34 dy -20 dx 7
    00 23 DF 00 00 19          ; dessin banque 0 frame 35 dy -33 dx 25
    00 24 D7 00 00 35          ; dessin banque 0 frame 36 dy -41 dx 53
    00 25 C2 00 00 42          ; dessin banque 0 frame 37 dy -62 dx 66
    00 26 CC 00 00 42          ; dessin banque 0 frame 38 dy -52 dx 66
    00 27 ED 00 00 42          ; dessin banque 0 frame 39 dy -19 dx 66
    FF 00                      ; fin d'étape
  ; étape 6
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 28 C7 00 FF 97          ; dessin banque 0 frame 40 dy -57 dx -105
    00 29 D4 00 FF 9C          ; dessin banque 0 frame 41 dy -44 dx -100
    00 2A D4 00 FF A4          ; dessin banque 0 frame 42 dy -44 dx -92
    00 2B DA 00 00 18          ; dessin banque 0 frame 43 dy -38 dx 24
    00 2D D6 00 00 31          ; dessin banque 0 frame 45 dy -42 dx 49
    00 2E B7 00 00 37          ; dessin banque 0 frame 46 dy -73 dx 55
    00 2F C2 00 00 3C          ; dessin banque 0 frame 47 dy -62 dx 60
    00 30 C9 00 00 39          ; dessin banque 0 frame 48 dy -55 dx 57
    00 31 DD 00 FF A4          ; dessin banque 0 frame 49 dy -35 dx -92
    00 32 FC 00 FF BE          ; dessin banque 0 frame 50 dy -4 dx -66
    00 33 EE 00 FF DD          ; dessin banque 0 frame 51 dy -18 dx -35
    00 34 E9 00 00 07          ; dessin banque 0 frame 52 dy -23 dx 7
    00 35 E2 00 00 43          ; dessin banque 0 frame 53 dy -30 dx 67
    FF 00                      ; fin d'étape
  ; étape 7
    88 08                      ; $88 Hold
    00 00 D1 00 FF E4          ; dessin banque 0 frame 0 dy -47 dx -28
    00 36 B5 00 FF A3          ; dessin banque 0 frame 54 dy -75 dx -93
    00 37 E3 00 FF AF          ; dessin banque 0 frame 55 dy -29 dx -81
    00 38 D4 00 FF BA          ; dessin banque 0 frame 56 dy -44 dx -70
    00 39 EB 00 FF D9          ; dessin banque 0 frame 57 dy -21 dx -39
    00 3A E5 00 00 07          ; dessin banque 0 frame 58 dy -27 dx 7
    00 3B D2 00 00 18          ; dessin banque 0 frame 59 dy -46 dx 24
    00 3C A9 00 00 22          ; dessin banque 0 frame 60 dy -87 dx 34
    00 3D B5 00 00 2B          ; dessin banque 0 frame 61 dy -75 dx 43
    00 3E BE 00 00 30          ; dessin banque 0 frame 62 dy -66 dx 48
    00 3F E4 00 00 45          ; dessin banque 0 frame 63 dy -28 dx 69
    FF FF                      ; fin du script
```

### `LAB_00D3` (2:$3FE)

Rôles : référencé par le code dans LAB_002E

```
  ; étape 1
    88 06                      ; $88 Hold
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 4E F2 00 00 07          ; dessin banque 0 frame 78 dy -14 dx 7
    00 4F 01 00 00 03          ; dessin banque 0 frame 79 dy 1 dx 3
    00 50 0F 00 FF EC          ; dessin banque 0 frame 80 dy 15 dx -20
    00 51 31 00 FF E9          ; dessin banque 0 frame 81 dy 49 dx -23
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 2
    00 52 FA 00 00 20          ; dessin banque 0 frame 82 dy -6 dx 32
    00 53 14 00 FF FE          ; dessin banque 0 frame 83 dy 20 dx -2
    00 54 24 00 FF E9          ; dessin banque 0 frame 84 dy 36 dx -23
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 3
    00 55 06 00 00 3E          ; dessin banque 0 frame 85 dy 6 dx 62
    00 56 2B 00 FF E9          ; dessin banque 0 frame 86 dy 43 dx -23
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 4
    00 57 22 00 00 50          ; dessin banque 0 frame 87 dy 34 dx 80
    00 58 34 00 FF E9          ; dessin banque 0 frame 88 dy 52 dx -23
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 5
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 6
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 8
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 9
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 10
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 11
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 12
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF 00                      ; fin d'étape
  ; étape 13
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    88 0A                      ; $88 Hold
    00 59 35 00 FF E9          ; dessin banque 0 frame 89 dy 53 dx -23
    00 5B 38 00 00 70          ; dessin banque 0 frame 91 dy 56 dx 112
    00 5A 42 00 00 5E          ; dessin banque 0 frame 90 dy 66 dx 94
    00 47 F1 00 FF 7D          ; dessin banque 0 frame 71 dy -15 dx -131
    00 48 11 00 FF 79          ; dessin banque 0 frame 72 dy 17 dx -135
    00 49 11 00 FF BD          ; dessin banque 0 frame 73 dy 17 dx -67
    00 4A 11 00 FF 88          ; dessin banque 0 frame 74 dy 17 dx -120
    00 4B 1C 00 FF A2          ; dessin banque 0 frame 75 dy 28 dx -94
    00 4C 32 00 FF A2          ; dessin banque 0 frame 76 dy 50 dx -94
    00 4D 51 00 FF A7          ; dessin banque 0 frame 77 dy 81 dx -89
    00 41 0C 00 00 0C          ; dessin banque 0 frame 65 dy 12 dx 12
    00 40 03 00 00 12          ; dessin banque 0 frame 64 dy 3 dx 18
    00 42 33 00 00 49          ; dessin banque 0 frame 66 dy 51 dx 73
    00 43 3E 00 00 49          ; dessin banque 0 frame 67 dy 62 dx 73
    00 45 47 00 FF F4          ; dessin banque 0 frame 69 dy 71 dx -12
    00 46 54 00 00 84          ; dessin banque 0 frame 70 dy 84 dx 132
    00 44 56 00 FF EA          ; dessin banque 0 frame 68 dy 86 dx -22
    FF FF                      ; fin du script
```

### `LAB_00D4` (2:$AF4)

Rôles : référencé par le code dans LAB_002C

```
  ; étape 1
    88 04                      ; $88 Hold
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 2
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 3
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 4
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 5
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 6
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 7
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 8
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 9
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 10
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 11
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 12
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 13
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF 00                      ; fin d'étape
  ; étape 14
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    88 0F                      ; $88 Hold
    04 0B FB 00 FF 96          ; dessin banque 1 frame 11 dy -5 dx -106
    04 0C F0 00 FF A8          ; dessin banque 1 frame 12 dy -16 dx -88
    04 0D 13 00 FF C1          ; dessin banque 1 frame 13 dy 19 dx -63
    04 0E 24 00 FF C1          ; dessin banque 1 frame 14 dy 36 dx -63
    04 0F 11 00 FF DD          ; dessin banque 1 frame 15 dy 17 dx -35
    04 10 F9 00 00 2D          ; dessin banque 1 frame 16 dy -7 dx 45
    04 11 05 00 00 33          ; dessin banque 1 frame 17 dy 5 dx 51
    04 12 10 00 00 37          ; dessin banque 1 frame 18 dy 16 dx 55
    04 13 10 00 00 22          ; dessin banque 1 frame 19 dy 16 dx 34
    04 14 20 00 00 11          ; dessin banque 1 frame 20 dy 32 dx 17
    04 15 40 00 FF C2          ; dessin banque 1 frame 21 dy 64 dx -62
    04 16 57 00 FF B4          ; dessin banque 1 frame 22 dy 87 dx -76
    FF FF                      ; fin du script
```

### `LAB_00D5` (2:$1024)

Rôles : référencé par le code dans LAB_0039

```
  ; étape 1
    88 04                      ; $88 Hold
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 2
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 4
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 5
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 6
    88 08                      ; $88 Hold
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 7
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 8
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    88 0A                      ; $88 Hold
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 9
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    FF 00                      ; fin d'étape
  ; étape 10
    04 03 BA 00 FF EC          ; dessin banque 1 frame 3 dy -70 dx -20
    04 08 BA 00 00 0A          ; dessin banque 1 frame 8 dy -70 dx 10
    04 09 CD 00 00 29          ; dessin banque 1 frame 9 dy -51 dx 41
    04 0A CE 00 00 40          ; dessin banque 1 frame 10 dy -50 dx 64
    04 06 C7 00 FF DE          ; dessin banque 1 frame 6 dy -57 dx -34
    04 07 D4 00 FF EC          ; dessin banque 1 frame 7 dy -44 dx -20
    04 05 D3 00 FF C7          ; dessin banque 1 frame 5 dy -45 dx -57
    04 04 DB 00 FF AC          ; dessin banque 1 frame 4 dy -37 dx -84
    04 01 A0 00 FF C8          ; dessin banque 1 frame 1 dy -96 dx -56
    04 00 A3 00 FF F6          ; dessin banque 1 frame 0 dy -93 dx -10
    04 02 AF 00 00 17          ; dessin banque 1 frame 2 dy -81 dx 23
    04 19 FA 00 00 04          ; dessin banque 1 frame 25 dy -6 dx 4
    04 1A 08 00 00 15          ; dessin banque 1 frame 26 dy 8 dx 21
    04 1B 0E 00 FF B9          ; dessin banque 1 frame 27 dy 14 dx -71
    04 1C 3E 00 FF CB          ; dessin banque 1 frame 28 dy 62 dx -53
    04 1D 53 00 FF B6          ; dessin banque 1 frame 29 dy 83 dx -74
    04 18 F9 00 FF DC          ; dessin banque 1 frame 24 dy -7 dx -36
    04 17 07 00 FF CC          ; dessin banque 1 frame 23 dy 7 dx -52
    B4 00 00 00 0E C4          ; $B4 Call -> LAB_003F
    FF FF                      ; fin du script
```

### `LAB_00D6` (2:$1344)

Rôles : référencé par le code dans LAB_002D

```
  ; étape 1
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 01 EF 00 00 37          ; dessin banque 2 frame 1 dy -17 dx 55
    FF 00                      ; fin d'étape
  ; étape 2
    B4 00 00 00 0E E6          ; $B4 Call -> LAB_0040
    88 02                      ; $88 Hold
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 01 F0 00 00 37          ; dessin banque 2 frame 1 dy -16 dx 55
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 02 EF 00 00 38          ; dessin banque 2 frame 2 dy -17 dx 56
    FF 00                      ; fin d'étape
  ; étape 4
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 03 EF 00 00 34          ; dessin banque 2 frame 3 dy -17 dx 52
    08 04 02 00 00 28          ; dessin banque 2 frame 4 dy 2 dx 40
    FF 00                      ; fin d'étape
  ; étape 5
    88 04                      ; $88 Hold
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 06 EF 00 00 31          ; dessin banque 2 frame 6 dy -17 dx 49
    08 05 00 00 00 1E          ; dessin banque 2 frame 5 dy 0 dx 30
    FF 00                      ; fin d'étape
  ; étape 6
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 02 EF 00 00 38          ; dessin banque 2 frame 2 dy -17 dx 56
    FF 00                      ; fin d'étape
  ; étape 7
    88 14                      ; $88 Hold
    08 00 9C 00 00 44          ; dessin banque 2 frame 0 dy -100 dx 68
    08 01 EF 00 00 37          ; dessin banque 2 frame 1 dy -17 dx 55
    FF FF                      ; fin du script
```

### `LAB_00D7` (2:$13C0)

Rôles : entrée 0 de la table LAB_0025 ; entrée 1 de la table LAB_0025 ; entrée 10 de la table LAB_0025 ; entrée 11 de la table LAB_0025 ; entrée 12 de la table LAB_0025 ; entrée 13 de la table LAB_0025 ; entrée 14 de la table LAB_0025 ; entrée 15 de la table LAB_0025 ; entrée 2 de la table LAB_0025 ; entrée 3 de la table LAB_0025 ; entrée 4 de la table LAB_0025 ; entrée 5 de la table LAB_0025 ; entrée 6 de la table LAB_0025 ; entrée 7 de la table LAB_0025 ; entrée 8 de la table LAB_0025 ; entrée 9 de la table LAB_0025

```
  ; étape 1
    0C 10 23 20 00 82          ; dessin banque 3 frame 16 dy 35 dx 130
    0C 0F 02 20 00 6D          ; dessin banque 3 frame 15 dy 2 dx 109
    0C 0E EE 20 00 87          ; dessin banque 3 frame 14 dy -18 dx 135
    0C 0D D4 20 00 91          ; dessin banque 3 frame 13 dy -44 dx 145
    0C 14 E8 20 00 70          ; dessin banque 3 frame 20 dy -24 dx 112
    0C 13 BF 20 00 68          ; dessin banque 3 frame 19 dy -65 dx 104
    0C 1A B6 20 00 73          ; dessin banque 3 frame 26 dy -74 dx 115
    FF 00                      ; fin d'étape
  ; étape 2
    0C 10 23 20 00 6C          ; dessin banque 3 frame 16 dy 35 dx 108
    0C 0F 02 20 00 57          ; dessin banque 3 frame 15 dy 2 dx 87
    0C 0E EE 20 00 72          ; dessin banque 3 frame 14 dy -18 dx 114
    0C 0D D4 20 00 7A          ; dessin banque 3 frame 13 dy -44 dx 122
    0C 14 E7 20 00 5A          ; dessin banque 3 frame 20 dy -25 dx 90
    0C 13 BE 20 00 52          ; dessin banque 3 frame 19 dy -66 dx 82
    FF 00                      ; fin d'étape
  ; étape 3
    0C 10 26 20 00 58          ; dessin banque 3 frame 16 dy 38 dx 88
    0C 0F 05 20 00 43          ; dessin banque 3 frame 15 dy 5 dx 67
    0C 0E F1 20 00 5E          ; dessin banque 3 frame 14 dy -15 dx 94
    0C 0D D7 20 00 66          ; dessin banque 3 frame 13 dy -41 dx 102
    0C 14 EA 20 00 46          ; dessin banque 3 frame 20 dy -22 dx 70
    0C 13 C1 20 00 3E          ; dessin banque 3 frame 19 dy -63 dx 62
    FF 00                      ; fin d'étape
  ; étape 4
    0C 10 2C 20 00 40          ; dessin banque 3 frame 16 dy 44 dx 64
    0C 0F 0B 20 00 2B          ; dessin banque 3 frame 15 dy 11 dx 43
    0C 0E F7 20 00 46          ; dessin banque 3 frame 14 dy -9 dx 70
    0C 0D DD 20 00 4E          ; dessin banque 3 frame 13 dy -35 dx 78
    0C 14 F0 20 00 2E          ; dessin banque 3 frame 20 dy -16 dx 46
    0C 13 C7 20 00 26          ; dessin banque 3 frame 19 dy -57 dx 38
    FF 00                      ; fin d'étape
  ; étape 5
    0C 18 3F 20 00 32          ; dessin banque 3 frame 24 dy 63 dx 50
    0C 17 23 20 00 36          ; dessin banque 3 frame 23 dy 35 dx 54
    0C 16 09 20 00 24          ; dessin banque 3 frame 22 dy 9 dx 36
    0C 15 E0 20 00 37          ; dessin banque 3 frame 21 dy -32 dx 55
    0C 14 ED 20 00 27          ; dessin banque 3 frame 20 dy -19 dx 39
    0C 1B D4 20 00 21          ; dessin banque 3 frame 27 dy -44 dx 33
    0C 1A CB 20 00 29          ; dessin banque 3 frame 26 dy -53 dx 41
    FF 00                      ; fin d'étape
  ; étape 6
    0C 22 28 20 00 27          ; dessin banque 3 frame 34 dy 40 dx 39
    0C 21 1A 20 00 2D          ; dessin banque 3 frame 33 dy 26 dx 45
    0C 20 04 20 00 1D          ; dessin banque 3 frame 32 dy 4 dx 29
    0C 1F E2 20 00 2E          ; dessin banque 3 frame 31 dy -30 dx 46
    0C 1E F0 20 00 1D          ; dessin banque 3 frame 30 dy -16 dx 29
    0C 1D D2 20 00 18          ; dessin banque 3 frame 29 dy -46 dx 24
    FF 00                      ; fin d'étape
  ; étape 7
    0C 22 2B 20 00 17          ; dessin banque 3 frame 34 dy 43 dx 23
    0C 21 1D 20 00 1D          ; dessin banque 3 frame 33 dy 29 dx 29
    0C 20 07 20 00 0D          ; dessin banque 3 frame 32 dy 7 dx 13
    0C 1F E5 20 00 1E          ; dessin banque 3 frame 31 dy -27 dx 30
    0C 1E F3 20 00 0D          ; dessin banque 3 frame 30 dy -13 dx 13
    0C 1D D5 20 00 08          ; dessin banque 3 frame 29 dy -43 dx 8
    FF 00                      ; fin d'étape
  ; étape 8
    0C 22 2E 20 00 07          ; dessin banque 3 frame 34 dy 46 dx 7
    0C 21 20 20 00 0D          ; dessin banque 3 frame 33 dy 32 dx 13
    0C 20 0A 20 FF FD          ; dessin banque 3 frame 32 dy 10 dx -3
    0C 1F E8 20 00 0E          ; dessin banque 3 frame 31 dy -24 dx 14
    0C 1E F6 20 FF FD          ; dessin banque 3 frame 30 dy -10 dx -3
    0C 1D D8 20 FF F8          ; dessin banque 3 frame 29 dy -40 dx -8
    FF 00                      ; fin d'étape
  ; étape 9
    0C 29 35 20 FF FD          ; dessin banque 3 frame 41 dy 53 dx -3
    0C 28 1B 20 FF FD          ; dessin banque 3 frame 40 dy 27 dx -3
    0C 27 06 20 FF F3          ; dessin banque 3 frame 39 dy 6 dx -13
    0C 26 FB 20 00 03          ; dessin banque 3 frame 38 dy -5 dx 3
    0C 25 EB 20 00 08          ; dessin banque 3 frame 37 dy -21 dx 8
    0C 24 DB 20 FF F2          ; dessin banque 3 frame 36 dy -37 dx -14
    FF 00                      ; fin d'étape
  ; étape 10
    0C 29 38 20 FF F1          ; dessin banque 3 frame 41 dy 56 dx -15
    0C 28 1E 20 FF F1          ; dessin banque 3 frame 40 dy 30 dx -15
    0C 27 09 20 FF E7          ; dessin banque 3 frame 39 dy 9 dx -25
    0C 26 FE 20 FF F7          ; dessin banque 3 frame 38 dy -2 dx -9
    0C 25 EE 20 FF FC          ; dessin banque 3 frame 37 dy -18 dx -4
    0C 24 DE 20 FF E6          ; dessin banque 3 frame 36 dy -34 dx -26
    FF 00                      ; fin d'étape
  ; étape 11
    0C 2C 19 20 FF E4          ; dessin banque 3 frame 44 dy 25 dx -28
    0C 2B F2 20 FF D9          ; dessin banque 3 frame 43 dy -14 dx -39
    0C 2A E2 20 FF D9          ; dessin banque 3 frame 42 dy -30 dx -39
    FF 00                      ; fin d'étape
  ; étape 12
    0C 2C 1C 20 FF D6          ; dessin banque 3 frame 44 dy 28 dx -42
    0C 2B F5 20 FF CB          ; dessin banque 3 frame 43 dy -11 dx -53
    0C 2A E5 20 FF CB          ; dessin banque 3 frame 42 dy -27 dx -53
    FF 00                      ; fin d'étape
  ; étape 13
    0C 2F 18 20 FF CC          ; dessin banque 3 frame 47 dy 24 dx -52
    0C 2E F7 20 FF C4          ; dessin banque 3 frame 46 dy -9 dx -60
    0C 2D EC 20 FF C4          ; dessin banque 3 frame 45 dy -20 dx -60
    FF 00                      ; fin d'étape
  ; étape 14
    0C 32 14 20 FF C4          ; dessin banque 3 frame 50 dy 20 dx -60
    0C 31 F9 20 FF BD          ; dessin banque 3 frame 49 dy -7 dx -67
    0C 30 EF 20 FF BD          ; dessin banque 3 frame 48 dy -17 dx -67
    FF 00                      ; fin d'étape
  ; étape 15
    0C 32 16 20 FF BA          ; dessin banque 3 frame 50 dy 22 dx -70
    0C 31 FB 20 FF B3          ; dessin banque 3 frame 49 dy -5 dx -77
    0C 30 F1 20 FF B3          ; dessin banque 3 frame 48 dy -15 dx -77
    FF 00                      ; fin d'étape
  ; étape 16
    0C 32 17 20 FF B0          ; dessin banque 3 frame 50 dy 23 dx -80
    0C 31 FC 20 FF A9          ; dessin banque 3 frame 49 dy -4 dx -87
    0C 30 F2 20 FF A9          ; dessin banque 3 frame 48 dy -14 dx -87
    FF 00                      ; fin d'étape
  ; étape 17
    0C 33 F6 20 FF A7          ; dessin banque 3 frame 51 dy -10 dx -89
    FF 00                      ; fin d'étape
  ; étape 18
    0C 33 F7 20 FF 9D          ; dessin banque 3 frame 51 dy -9 dx -99
    FF 00                      ; fin d'étape
  ; étape 19
    0C 34 FB 20 FF 99          ; dessin banque 3 frame 52 dy -5 dx -103
    FF 00                      ; fin d'étape
  ; étape 20
    0C 34 FC 20 FF 8D          ; dessin banque 3 frame 52 dy -4 dx -115
    FF 00                      ; fin d'étape
  ; étape 21
    0C 34 FE 20 FF 84          ; dessin banque 3 frame 52 dy -2 dx -124
    FF 00                      ; fin d'étape
  ; étape 22
    0C 34 00 20 FF 7A          ; dessin banque 3 frame 52 dy 0 dx -134
    FF 00                      ; fin d'étape
  ; étape 23
    0C 32 0E 20 FF 54          ; dessin banque 3 frame 50 dy 14 dx -172
    0C 31 F3 20 FF 4D          ; dessin banque 3 frame 49 dy -13 dx -179
    0C 30 E9 20 FF 4D          ; dessin banque 3 frame 48 dy -23 dx -179
    FF FF                      ; fin du script
```

### `LAB_00D8` (2:$1604)

Rôles : entrée 0 de la table LAB_0024 ; entrée 1 de la table LAB_0024 ; entrée 10 de la table LAB_0024 ; entrée 11 de la table LAB_0024 ; entrée 12 de la table LAB_0024 ; entrée 13 de la table LAB_0024 ; entrée 14 de la table LAB_0024 ; entrée 15 de la table LAB_0024 ; entrée 16 de la table LAB_0024 ; entrée 17 de la table LAB_0024 ; entrée 18 de la table LAB_0024 ; entrée 19 de la table LAB_0024 ; entrée 2 de la table LAB_0024 ; entrée 3 de la table LAB_0024 ; entrée 4 de la table LAB_0024 ; entrée 5 de la table LAB_0024 ; entrée 6 de la table LAB_0024 ; entrée 7 de la table LAB_0024 ; entrée 8 de la table LAB_0024 ; entrée 9 de la table LAB_0024

```
  ; étape 1
    0C 01 FB 20 00 91          ; dessin banque 3 frame 1 dy -5 dx 145
    0C 00 DC 20 00 94          ; dessin banque 3 frame 0 dy -36 dx 148
    0C 0C C5 20 00 92          ; dessin banque 3 frame 12 dy -59 dx 146
    FF 00                      ; fin d'étape
  ; étape 2
    0C 02 FB 20 00 8E          ; dessin banque 3 frame 2 dy -5 dx 142
    0C 00 DD 20 00 89          ; dessin banque 3 frame 0 dy -35 dx 137
    0C 0B C6 20 00 86          ; dessin banque 3 frame 11 dy -58 dx 134
    FF 00                      ; fin d'étape
  ; étape 3
    0C 03 FB 20 00 8B          ; dessin banque 3 frame 3 dy -5 dx 139
    0C 00 DD 20 00 80          ; dessin banque 3 frame 0 dy -35 dx 128
    0C 0A C7 20 00 7E          ; dessin banque 3 frame 10 dy -57 dx 126
    FF 00                      ; fin d'étape
  ; étape 4
    0C 04 FB 20 00 76          ; dessin banque 3 frame 4 dy -5 dx 118
    0C 00 DD 20 00 74          ; dessin banque 3 frame 0 dy -35 dx 116
    0C 09 C7 20 00 73          ; dessin banque 3 frame 9 dy -57 dx 115
    FF 00                      ; fin d'étape
  ; étape 5
    0C 05 FB 20 00 6C          ; dessin banque 3 frame 5 dy -5 dx 108
    0C 00 DD 20 00 6D          ; dessin banque 3 frame 0 dy -35 dx 109
    0C 0C C7 20 00 6B          ; dessin banque 3 frame 12 dy -57 dx 107
    FF 00                      ; fin d'étape
  ; étape 6
    0C 06 FB 20 00 6A          ; dessin banque 3 frame 6 dy -5 dx 106
    0C 00 DD 20 00 64          ; dessin banque 3 frame 0 dy -35 dx 100
    0C 0B C5 20 00 62          ; dessin banque 3 frame 11 dy -59 dx 98
    FF 00                      ; fin d'étape
  ; étape 7
    0C 07 FB 20 00 65          ; dessin banque 3 frame 7 dy -5 dx 101
    0C 00 DD 20 00 5A          ; dessin banque 3 frame 0 dy -35 dx 90
    0C 0A C5 20 00 58          ; dessin banque 3 frame 10 dy -59 dx 88
    FF 00                      ; fin d'étape
  ; étape 8
    0C 08 FC 20 00 52          ; dessin banque 3 frame 8 dy -4 dx 82
    0C 00 DE 20 00 50          ; dessin banque 3 frame 0 dy -34 dx 80
    0C 09 C7 20 00 4E          ; dessin banque 3 frame 9 dy -57 dx 78
    FF 00                      ; fin d'étape
  ; étape 9
    0C 01 FC 20 00 4B          ; dessin banque 3 frame 1 dy -4 dx 75
    0C 00 DE 20 00 4C          ; dessin banque 3 frame 0 dy -34 dx 76
    0C 09 C5 20 00 4A          ; dessin banque 3 frame 9 dy -59 dx 74
    FF 00                      ; fin d'étape
  ; étape 10
    0C 02 FC 20 00 49          ; dessin banque 3 frame 2 dy -4 dx 73
    0C 00 DE 20 00 44          ; dessin banque 3 frame 0 dy -34 dx 68
    0C 0A C7 20 00 43          ; dessin banque 3 frame 10 dy -57 dx 67
    FF 00                      ; fin d'étape
  ; étape 11
    0C 03 FC 20 00 46          ; dessin banque 3 frame 3 dy -4 dx 70
    0C 00 DE 20 00 3A          ; dessin banque 3 frame 0 dy -34 dx 58
    0C 0B C6 20 00 38          ; dessin banque 3 frame 11 dy -58 dx 56
    FF 00                      ; fin d'étape
  ; étape 12
    0C 04 FC 20 00 32          ; dessin banque 3 frame 4 dy -4 dx 50
    0C 00 DE 20 00 2F          ; dessin banque 3 frame 0 dy -34 dx 47
    0C 0C C6 20 00 2D          ; dessin banque 3 frame 12 dy -58 dx 45
    FF 00                      ; fin d'étape
  ; étape 13
    0C 05 FC 20 00 27          ; dessin banque 3 frame 5 dy -4 dx 39
    0C 00 DE 20 00 28          ; dessin banque 3 frame 0 dy -34 dx 40
    0C 09 C6 20 00 26          ; dessin banque 3 frame 9 dy -58 dx 38
    FF 00                      ; fin d'étape
  ; étape 14
    0C 06 FC 20 00 24          ; dessin banque 3 frame 6 dy -4 dx 36
    0C 00 DE 20 00 1D          ; dessin banque 3 frame 0 dy -34 dx 29
    0C 0A C7 20 00 1B          ; dessin banque 3 frame 10 dy -57 dx 27
    FF 00                      ; fin d'étape
  ; étape 15
    0C 07 FD 20 00 1F          ; dessin banque 3 frame 7 dy -3 dx 31
    0C 00 DF 20 00 13          ; dessin banque 3 frame 0 dy -33 dx 19
    0C 0B C7 20 00 10          ; dessin banque 3 frame 11 dy -57 dx 16
    FF 00                      ; fin d'étape
  ; étape 16
    0C 08 FC 20 00 0B          ; dessin banque 3 frame 8 dy -4 dx 11
    0C 00 DF 20 00 08          ; dessin banque 3 frame 0 dy -33 dx 8
    0C 0C C7 20 00 06          ; dessin banque 3 frame 12 dy -57 dx 6
    FF 00                      ; fin d'étape
  ; étape 17
    0C 01 FC 20 00 03          ; dessin banque 3 frame 1 dy -4 dx 3
    0C 00 DF 20 00 05          ; dessin banque 3 frame 0 dy -33 dx 5
    0C 09 C5 20 00 03          ; dessin banque 3 frame 9 dy -59 dx 3
    FF 00                      ; fin d'étape
  ; étape 18
    0C 02 FC 20 00 01          ; dessin banque 3 frame 2 dy -4 dx 1
    0C 00 DF 20 FF FC          ; dessin banque 3 frame 0 dy -33 dx -4
    0C 0A C6 20 FF FA          ; dessin banque 3 frame 10 dy -58 dx -6
    FF 00                      ; fin d'étape
  ; étape 19
    0C 03 FC 20 FF FD          ; dessin banque 3 frame 3 dy -4 dx -3
    0C 00 DF 20 FF F0          ; dessin banque 3 frame 0 dy -33 dx -16
    0C 0B C6 20 FF EE          ; dessin banque 3 frame 11 dy -58 dx -18
    FF 00                      ; fin d'étape
  ; étape 20
    0C 04 FD 20 FF EB          ; dessin banque 3 frame 4 dy -3 dx -21
    0C 00 DF 20 FF E8          ; dessin banque 3 frame 0 dy -33 dx -24
    0C 0C C6 20 FF E6          ; dessin banque 3 frame 12 dy -58 dx -26
    FF 00                      ; fin d'étape
  ; étape 21
    0C 05 FC 20 FF E1          ; dessin banque 3 frame 5 dy -4 dx -31
    0C 00 DE 20 FF E2          ; dessin banque 3 frame 0 dy -34 dx -30
    0C 09 C6 20 FF E0          ; dessin banque 3 frame 9 dy -58 dx -32
    FF 00                      ; fin d'étape
  ; étape 22
    0C 06 FC 20 FF DD          ; dessin banque 3 frame 6 dy -4 dx -35
    0C 00 DE 20 FF D7          ; dessin banque 3 frame 0 dy -34 dx -41
    0C 0A C6 20 FF D4          ; dessin banque 3 frame 10 dy -58 dx -44
    FF 00                      ; fin d'étape
  ; étape 23
    0C 07 FC 20 FF DA          ; dessin banque 3 frame 7 dy -4 dx -38
    0C 00 DE 20 FF CF          ; dessin banque 3 frame 0 dy -34 dx -49
    0C 0B C5 20 FF CC          ; dessin banque 3 frame 11 dy -59 dx -52
    FF 00                      ; fin d'étape
  ; étape 24
    0C 08 FC 20 FF C7          ; dessin banque 3 frame 8 dy -4 dx -57
    0C 00 DE 20 FF C5          ; dessin banque 3 frame 0 dy -34 dx -59
    0C 0C C6 20 FF C2          ; dessin banque 3 frame 12 dy -58 dx -62
    FF 00                      ; fin d'étape
  ; étape 25
    0C 01 FD 20 FF BF          ; dessin banque 3 frame 1 dy -3 dx -65
    0C 00 DF 20 FF C1          ; dessin banque 3 frame 0 dy -33 dx -63
    0C 09 C6 20 FF BF          ; dessin banque 3 frame 9 dy -58 dx -65
    FF 00                      ; fin d'étape
  ; étape 26
    0C 02 FD 20 FF BD          ; dessin banque 3 frame 2 dy -3 dx -67
    0C 00 DF 20 FF B9          ; dessin banque 3 frame 0 dy -33 dx -71
    0C 0A C6 20 FF B7          ; dessin banque 3 frame 10 dy -58 dx -73
    FF 00                      ; fin d'étape
  ; étape 27
    0C 03 FD 20 FF B8          ; dessin banque 3 frame 3 dy -3 dx -72
    0C 00 DF 20 FF AC          ; dessin banque 3 frame 0 dy -33 dx -84
    0C 0B C6 20 FF AA          ; dessin banque 3 frame 11 dy -58 dx -86
    FF 00                      ; fin d'étape
  ; étape 28
    0C 04 FD 20 FF A7          ; dessin banque 3 frame 4 dy -3 dx -89
    0C 00 DF 20 FF A4          ; dessin banque 3 frame 0 dy -33 dx -92
    0C 0C C6 20 FF A3          ; dessin banque 3 frame 12 dy -58 dx -93
    FF 00                      ; fin d'étape
  ; étape 29
    0C 05 FD 20 FF 9D          ; dessin banque 3 frame 5 dy -3 dx -99
    0C 00 DE 20 FF 9E          ; dessin banque 3 frame 0 dy -34 dx -98
    0C 09 C5 20 FF 9C          ; dessin banque 3 frame 9 dy -59 dx -100
    FF 00                      ; fin d'étape
  ; étape 30
    0C 06 FC 20 FF 9A          ; dessin banque 3 frame 6 dy -4 dx -102
    0C 00 DE 20 FF 93          ; dessin banque 3 frame 0 dy -34 dx -109
    0C 0A C5 20 FF 91          ; dessin banque 3 frame 10 dy -59 dx -111
    FF 00                      ; fin d'étape
  ; étape 31
    0C 07 FC 20 FF 96          ; dessin banque 3 frame 7 dy -4 dx -106
    0C 00 DE 20 FF 8A          ; dessin banque 3 frame 0 dy -34 dx -118
    0C 0B C5 20 FF 88          ; dessin banque 3 frame 11 dy -59 dx -120
    FF 00                      ; fin d'étape
  ; étape 32
    0C 08 FD 20 FF 84          ; dessin banque 3 frame 8 dy -3 dx -124
    0C 00 DF 20 FF 82          ; dessin banque 3 frame 0 dy -33 dx -126
    0C 0C C7 20 FF 80          ; dessin banque 3 frame 12 dy -57 dx -128
    FF 00                      ; fin d'étape
  ; étape 33
    0C 01 FD 20 FF 7B          ; dessin banque 3 frame 1 dy -3 dx -133
    0C 00 DF 20 FF 7C          ; dessin banque 3 frame 0 dy -33 dx -132
    0C 09 C7 20 FF 7A          ; dessin banque 3 frame 9 dy -57 dx -134
    FF 00                      ; fin d'étape
  ; étape 34
    0C 02 FD 20 FF 79          ; dessin banque 3 frame 2 dy -3 dx -135
    0C 00 E0 20 FF 74          ; dessin banque 3 frame 0 dy -32 dx -140
    0C 0A C5 20 FF 73          ; dessin banque 3 frame 10 dy -59 dx -141
    FF 00                      ; fin d'étape
  ; étape 35
    0C 03 FD 20 FF 76          ; dessin banque 3 frame 3 dy -3 dx -138
    0C 00 DF 20 FF 6A          ; dessin banque 3 frame 0 dy -33 dx -150
    0C 0B C5 20 FF 68          ; dessin banque 3 frame 11 dy -59 dx -152
    FF 00                      ; fin d'étape
  ; étape 36
    0C 04 FD 20 FF 61          ; dessin banque 3 frame 4 dy -3 dx -159
    0C 00 DF 20 FF 5E          ; dessin banque 3 frame 0 dy -33 dx -162
    0C 0C C6 20 FF 5C          ; dessin banque 3 frame 12 dy -58 dx -164
    FF 00                      ; fin d'étape
  ; étape 37
    0C 05 FC 20 FF 53          ; dessin banque 3 frame 5 dy -4 dx -173
    0C 00 DE 20 FF 55          ; dessin banque 3 frame 0 dy -34 dx -171
    FF 00                      ; fin d'étape
  ; étape 38
    0C 06 FC 20 FF 52          ; dessin banque 3 frame 6 dy -4 dx -174
    0C 00 DF 20 FF 4B          ; dessin banque 3 frame 0 dy -33 dx -181
    FF FF                      ; fin du script
```

### `LAB_00D9` (2:$19C2)

Rôles : référencé par le code dans LAB_0030

```
  ; étape 1
    10 19 31 00 FF E0          ; dessin banque 4 frame 25 dy 49 dx -32
    10 2C 2E 00 FF DD          ; dessin banque 4 frame 44 dy 46 dx -35
    FF 00                      ; fin d'étape
  ; étape 2
    10 18 2F 00 FF DC          ; dessin banque 4 frame 24 dy 47 dx -36
    10 27 32 00 FF D7          ; dessin banque 4 frame 39 dy 50 dx -41
    FF 00                      ; fin d'étape
  ; étape 3
    10 17 2D 00 FF D9          ; dessin banque 4 frame 23 dy 45 dx -39
    10 27 33 00 FF D4          ; dessin banque 4 frame 39 dy 51 dx -44
    FF 00                      ; fin d'étape
  ; étape 4
    10 18 2D 00 FF D4          ; dessin banque 4 frame 24 dy 45 dx -44
    10 2C 2D 00 FF D1          ; dessin banque 4 frame 44 dy 45 dx -47
    FF 00                      ; fin d'étape
  ; étape 5
    10 18 2C 00 FF D0          ; dessin banque 4 frame 24 dy 44 dx -48
    10 2C 2C 00 FF CD          ; dessin banque 4 frame 44 dy 44 dx -51
    FF 00                      ; fin d'étape
  ; étape 6
    10 18 2B 00 FF CC          ; dessin banque 4 frame 24 dy 43 dx -52
    10 29 2D 00 FF C9          ; dessin banque 4 frame 41 dy 45 dx -55
    FF 00                      ; fin d'étape
  ; étape 7
    10 18 29 00 FF C7          ; dessin banque 4 frame 24 dy 41 dx -57
    10 2C 28 00 FF C4          ; dessin banque 4 frame 44 dy 40 dx -60
    FF 00                      ; fin d'étape
  ; étape 8
    10 19 26 00 FF C3          ; dessin banque 4 frame 25 dy 38 dx -61
    10 2C 23 00 FF C1          ; dessin banque 4 frame 44 dy 35 dx -63
    FF 00                      ; fin d'étape
  ; étape 9
    10 19 23 00 FF BF          ; dessin banque 4 frame 25 dy 35 dx -65
    10 24 21 00 FF BC          ; dessin banque 4 frame 36 dy 33 dx -68
    FF 00                      ; fin d'étape
  ; étape 10
    10 19 20 00 FF BC          ; dessin banque 4 frame 25 dy 32 dx -68
    10 24 1E 00 FF B9          ; dessin banque 4 frame 36 dy 30 dx -71
    FF 00                      ; fin d'étape
  ; étape 11
    10 19 1D 00 FF B8          ; dessin banque 4 frame 25 dy 29 dx -72
    10 24 1B 00 FF B5          ; dessin banque 4 frame 36 dy 27 dx -75
    FF 00                      ; fin d'étape
  ; étape 12
    10 1A 1A 00 FF B4          ; dessin banque 4 frame 26 dy 26 dx -76
    10 25 16 00 FF B5          ; dessin banque 4 frame 37 dy 22 dx -75
    FF 00                      ; fin d'étape
  ; étape 13
    10 1A 17 00 FF B1          ; dessin banque 4 frame 26 dy 23 dx -79
    10 25 12 00 FF B2          ; dessin banque 4 frame 37 dy 18 dx -78
    FF 00                      ; fin d'étape
  ; étape 14
    10 1A 14 00 FF AF          ; dessin banque 4 frame 26 dy 20 dx -81
    10 24 11 00 FF AE          ; dessin banque 4 frame 36 dy 17 dx -82
    FF 00                      ; fin d'étape
  ; étape 15
    10 1A 11 00 FF AD          ; dessin banque 4 frame 26 dy 17 dx -83
    10 25 0D 00 FF AE          ; dessin banque 4 frame 37 dy 13 dx -82
    FF 00                      ; fin d'étape
  ; étape 16
    10 1A 0D 00 FF AB          ; dessin banque 4 frame 26 dy 13 dx -85
    10 25 09 00 FF AC          ; dessin banque 4 frame 37 dy 9 dx -84
    FF 00                      ; fin d'étape
  LAB_00DA:
  ; étape 17
    10 1A 0A 00 FF A9          ; dessin banque 4 frame 26 dy 10 dx -87
    10 24 07 00 FF A9          ; dessin banque 4 frame 36 dy 7 dx -87
    FF 00                      ; fin d'étape
  ; étape 18
    10 1A 05 00 FF A8          ; dessin banque 4 frame 26 dy 5 dx -88
    10 24 02 00 FF A8          ; dessin banque 4 frame 36 dy 2 dx -88
    FF 00                      ; fin d'étape
  ; étape 19
    10 0B 01 00 FF A5          ; dessin banque 4 frame 11 dy 1 dx -91
    10 25 FB 00 FF AA          ; dessin banque 4 frame 37 dy -5 dx -86
    FF 00                      ; fin d'étape
  ; étape 20
    10 0B FB 00 FF A4          ; dessin banque 4 frame 11 dy -5 dx -92
    10 25 F6 00 FF A9          ; dessin banque 4 frame 37 dy -10 dx -87
    FF 00                      ; fin d'étape
  ; étape 21
    10 0B F6 00 FF A4          ; dessin banque 4 frame 11 dy -10 dx -92
    10 2D F0 00 FF AA          ; dessin banque 4 frame 45 dy -16 dx -86
    FF 00                      ; fin d'étape
  ; étape 22
    10 0B F2 00 FF A4          ; dessin banque 4 frame 11 dy -14 dx -92
    10 2D EC 00 FF AA          ; dessin banque 4 frame 45 dy -20 dx -86
    FF 00                      ; fin d'étape
  ; étape 23
    10 0B EE 00 FF A4          ; dessin banque 4 frame 11 dy -18 dx -92
    10 2D E9 00 FF AA          ; dessin banque 4 frame 45 dy -23 dx -86
    FF 00                      ; fin d'étape
  ; étape 24
    10 0B EA 00 FF A4          ; dessin banque 4 frame 11 dy -22 dx -92
    10 2C E5 00 FF A8          ; dessin banque 4 frame 44 dy -27 dx -88
    FF 00                      ; fin d'étape
  ; étape 25
    10 0C E5 00 FF A4          ; dessin banque 4 frame 12 dy -27 dx -92
    10 2D E0 00 FF AD          ; dessin banque 4 frame 45 dy -32 dx -83
    FF 00                      ; fin d'étape
  ; étape 26
    10 0E E6 00 FF A5          ; dessin banque 4 frame 14 dy -26 dx -91
    10 27 E9 00 FF AE          ; dessin banque 4 frame 39 dy -23 dx -82
    FF 00                      ; fin d'étape
  LAB_00DB:
  ; étape 27
    88 02                      ; $88 Hold
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 2F ED 00 FF AE          ; dessin banque 4 frame 47 dy -19 dx -82
    FF 00                      ; fin d'étape
  ; étape 28
    88 02                      ; $88 Hold
    84 00 00 00 1B 2E          ; $84 Jump -> LAB_00DB
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 27 EC 00 FF AE          ; dessin banque 4 frame 39 dy -20 dx -82
    FF FF                      ; fin du script
```

### `LAB_00DA` (2:$1AA2)

Rôles : référencé par le code dans LAB_0030

```
  ; étape 1
    10 1A 0A 00 FF A9          ; dessin banque 4 frame 26 dy 10 dx -87
    10 24 07 00 FF A9          ; dessin banque 4 frame 36 dy 7 dx -87
    FF 00                      ; fin d'étape
  ; étape 2
    10 1A 05 00 FF A8          ; dessin banque 4 frame 26 dy 5 dx -88
    10 24 02 00 FF A8          ; dessin banque 4 frame 36 dy 2 dx -88
    FF 00                      ; fin d'étape
  ; étape 3
    10 0B 01 00 FF A5          ; dessin banque 4 frame 11 dy 1 dx -91
    10 25 FB 00 FF AA          ; dessin banque 4 frame 37 dy -5 dx -86
    FF 00                      ; fin d'étape
  ; étape 4
    10 0B FB 00 FF A4          ; dessin banque 4 frame 11 dy -5 dx -92
    10 25 F6 00 FF A9          ; dessin banque 4 frame 37 dy -10 dx -87
    FF 00                      ; fin d'étape
  ; étape 5
    10 0B F6 00 FF A4          ; dessin banque 4 frame 11 dy -10 dx -92
    10 2D F0 00 FF AA          ; dessin banque 4 frame 45 dy -16 dx -86
    FF 00                      ; fin d'étape
  ; étape 6
    10 0B F2 00 FF A4          ; dessin banque 4 frame 11 dy -14 dx -92
    10 2D EC 00 FF AA          ; dessin banque 4 frame 45 dy -20 dx -86
    FF 00                      ; fin d'étape
  ; étape 7
    10 0B EE 00 FF A4          ; dessin banque 4 frame 11 dy -18 dx -92
    10 2D E9 00 FF AA          ; dessin banque 4 frame 45 dy -23 dx -86
    FF 00                      ; fin d'étape
  ; étape 8
    10 0B EA 00 FF A4          ; dessin banque 4 frame 11 dy -22 dx -92
    10 2C E5 00 FF A8          ; dessin banque 4 frame 44 dy -27 dx -88
    FF 00                      ; fin d'étape
  ; étape 9
    10 0C E5 00 FF A4          ; dessin banque 4 frame 12 dy -27 dx -92
    10 2D E0 00 FF AD          ; dessin banque 4 frame 45 dy -32 dx -83
    FF 00                      ; fin d'étape
  ; étape 10
    10 0E E6 00 FF A5          ; dessin banque 4 frame 14 dy -26 dx -91
    10 27 E9 00 FF AE          ; dessin banque 4 frame 39 dy -23 dx -82
    FF 00                      ; fin d'étape
  LAB_00DB:
  ; étape 11
    88 02                      ; $88 Hold
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 2F ED 00 FF AE          ; dessin banque 4 frame 47 dy -19 dx -82
    FF 00                      ; fin d'étape
  ; étape 12
    88 02                      ; $88 Hold
    84 00 00 00 1B 2E          ; $84 Jump -> LAB_00DB
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 27 EC 00 FF AE          ; dessin banque 4 frame 39 dy -20 dx -82
    FF FF                      ; fin du script
```

### `LAB_00DB` (2:$1B2E)

Rôles : cible de saut depuis LAB_00D9 ; cible de saut depuis LAB_00DA ; cible de saut depuis LAB_00DB ; référencé par le code dans LAB_0031

```
  ; étape 1
    88 02                      ; $88 Hold
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 2F ED 00 FF AE          ; dessin banque 4 frame 47 dy -19 dx -82
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    84 00 00 00 1B 2E          ; $84 Jump -> LAB_00DB
    10 0F E6 00 FF A6          ; dessin banque 4 frame 15 dy -26 dx -90
    10 27 EC 00 FF AE          ; dessin banque 4 frame 39 dy -20 dx -82
    FF FF                      ; fin du script
```

### `LAB_00DC` (2:$1B54)

Rôles : cible de saut depuis LAB_00DC ; référencé par le code dans LAB_0030 ; référencé par le code dans LAB_0031

```
  ; étape 1
    88 02                      ; $88 Hold
    10 13 B6 10 FF D6          ; dessin banque 4 frame 19 dy -74 dx -42 [décor]
    10 2A C0 10 FF DC          ; dessin banque 4 frame 42 dy -64 dx -36 [décor]
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    84 00 00 00 1B 54          ; $84 Jump -> LAB_00DC
    10 13 B6 00 FF D6          ; dessin banque 4 frame 19 dy -74 dx -42
    10 2A C0 00 FF DC          ; dessin banque 4 frame 42 dy -64 dx -36
    FF FF                      ; fin du script
```

### `LAB_00DD` (2:$1B7A)

Rôles : cible de saut depuis LAB_00DD ; référencé par le code dans LAB_0030 ; référencé par le code dans LAB_0031

```
  ; étape 1
    88 02                      ; $88 Hold
    10 11 C5 00 FF B8          ; dessin banque 4 frame 17 dy -59 dx -72
    10 2F D0 00 FF C1          ; dessin banque 4 frame 47 dy -48 dx -63
    10 2F D1 00 FF C1          ; dessin banque 4 frame 47 dy -47 dx -63
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    84 00 00 00 1B 7A          ; $84 Jump -> LAB_00DD
    10 11 C5 00 FF B8          ; dessin banque 4 frame 17 dy -59 dx -72
    10 2F D0 00 FF C1          ; dessin banque 4 frame 47 dy -48 dx -63
    FF FF                      ; fin du script
```

### `LAB_00DE` (2:$1BA6)

Rôles : entrée 0 de la table LAB_0023 ; entrée 1 de la table LAB_0023

```
  ; étape 1
    10 0B 60 00 FF E4          ; dessin banque 4 frame 11 dy 96 dx -28
    10 2A 5B 00 FF EA          ; dessin banque 4 frame 42 dy 91 dx -22
    FF 00                      ; fin d'étape
  ; étape 2
    10 0B 5B 00 FF E4          ; dessin banque 4 frame 11 dy 91 dx -28
    10 2A 56 00 FF EB          ; dessin banque 4 frame 42 dy 86 dx -21
    FF 00                      ; fin d'étape
  ; étape 3
    10 0B 58 00 FF E4          ; dessin banque 4 frame 11 dy 88 dx -28
    10 2A 55 00 FF EA          ; dessin banque 4 frame 42 dy 85 dx -22
    FF 00                      ; fin d'étape
  ; étape 4
    10 0B 55 00 FF E4          ; dessin banque 4 frame 11 dy 85 dx -28
    10 2A 51 00 FF EA          ; dessin banque 4 frame 42 dy 81 dx -22
    FF 00                      ; fin d'étape
  ; étape 5
    10 0B 53 00 FF E4          ; dessin banque 4 frame 11 dy 83 dx -28
    10 25 4E 00 FF E9          ; dessin banque 4 frame 37 dy 78 dx -23
    FF 00                      ; fin d'étape
  ; étape 6
    10 0B 4F 00 FF E4          ; dessin banque 4 frame 11 dy 79 dx -28
    10 2C 4A 00 FF E8          ; dessin banque 4 frame 44 dy 74 dx -24
    FF 00                      ; fin d'étape
  ; étape 7
    10 0B 4C 00 FF E4          ; dessin banque 4 frame 11 dy 76 dx -28
    10 25 47 00 FF E9          ; dessin banque 4 frame 37 dy 71 dx -23
    FF 00                      ; fin d'étape
  ; étape 8
    10 0B 49 00 FF E4          ; dessin banque 4 frame 11 dy 73 dx -28
    10 2D 43 00 FF EA          ; dessin banque 4 frame 45 dy 67 dx -22
    FF 00                      ; fin d'étape
  ; étape 9
    10 0B 46 00 FF E4          ; dessin banque 4 frame 11 dy 70 dx -28
    10 25 40 00 FF E9          ; dessin banque 4 frame 37 dy 64 dx -23
    FF 00                      ; fin d'étape
  ; étape 10
    10 0B 43 00 FF E4          ; dessin banque 4 frame 11 dy 67 dx -28
    10 2D 3E 00 FF EA          ; dessin banque 4 frame 45 dy 62 dx -22
    FF 00                      ; fin d'étape
  ; étape 11
    10 0B 40 00 FF E4          ; dessin banque 4 frame 11 dy 64 dx -28
    10 25 3A 00 FF E9          ; dessin banque 4 frame 37 dy 58 dx -23
    FF 00                      ; fin d'étape
  ; étape 12
    10 0B 3D 00 FF E4          ; dessin banque 4 frame 11 dy 61 dx -28
    10 2D 37 00 FF EA          ; dessin banque 4 frame 45 dy 55 dx -22
    FF 00                      ; fin d'étape
  ; étape 13
    10 0B 3A 00 FF E4          ; dessin banque 4 frame 11 dy 58 dx -28
    10 25 34 00 FF E9          ; dessin banque 4 frame 37 dy 52 dx -23
    FF 00                      ; fin d'étape
  ; étape 14
    10 0B 37 00 FF E4          ; dessin banque 4 frame 11 dy 55 dx -28
    10 2C 32 00 FF E8          ; dessin banque 4 frame 44 dy 50 dx -24
    FF 00                      ; fin d'étape
  ; étape 15
    10 1A 33 00 FF E4          ; dessin banque 4 frame 26 dy 51 dx -28
    10 24 31 00 FF E3          ; dessin banque 4 frame 36 dy 49 dx -29
    FF 00                      ; fin d'étape
  ; étape 16
    10 19 31 00 FF E0          ; dessin banque 4 frame 25 dy 49 dx -32
    10 2C 2E 00 FF DD          ; dessin banque 4 frame 44 dy 46 dx -35
    FF 00                      ; fin d'étape
  ; étape 17
    10 18 2F 00 FF DC          ; dessin banque 4 frame 24 dy 47 dx -36
    10 27 32 00 FF D7          ; dessin banque 4 frame 39 dy 50 dx -41
    FF 00                      ; fin d'étape
  ; étape 18
    10 17 2D 00 FF D9          ; dessin banque 4 frame 23 dy 45 dx -39
    10 27 33 00 FF D4          ; dessin banque 4 frame 39 dy 51 dx -44
    FF 00                      ; fin d'étape
  ; étape 19
    10 18 2D 00 FF D4          ; dessin banque 4 frame 24 dy 45 dx -44
    10 2C 2D 00 FF D1          ; dessin banque 4 frame 44 dy 45 dx -47
    FF 00                      ; fin d'étape
  ; étape 20
    10 18 2C 00 FF D0          ; dessin banque 4 frame 24 dy 44 dx -48
    10 2C 2C 00 FF CD          ; dessin banque 4 frame 44 dy 44 dx -51
    FF 00                      ; fin d'étape
  ; étape 21
    10 18 2B 00 FF CC          ; dessin banque 4 frame 24 dy 43 dx -52
    10 29 2D 00 FF C9          ; dessin banque 4 frame 41 dy 45 dx -55
    FF 00                      ; fin d'étape
  ; étape 22
    10 18 29 00 FF C7          ; dessin banque 4 frame 24 dy 41 dx -57
    10 2C 28 00 FF C4          ; dessin banque 4 frame 44 dy 40 dx -60
    FF 00                      ; fin d'étape
  ; étape 23
    10 19 26 00 FF C3          ; dessin banque 4 frame 25 dy 38 dx -61
    10 2C 23 00 FF C1          ; dessin banque 4 frame 44 dy 35 dx -63
    FF 00                      ; fin d'étape
  ; étape 24
    10 19 23 00 FF BF          ; dessin banque 4 frame 25 dy 35 dx -65
    10 24 21 00 FF BC          ; dessin banque 4 frame 36 dy 33 dx -68
    FF 00                      ; fin d'étape
  ; étape 25
    10 19 20 00 FF BC          ; dessin banque 4 frame 25 dy 32 dx -68
    10 24 1E 00 FF B9          ; dessin banque 4 frame 36 dy 30 dx -71
    FF 00                      ; fin d'étape
  ; étape 26
    10 19 1D 00 FF B8          ; dessin banque 4 frame 25 dy 29 dx -72
    10 24 1B 00 FF B5          ; dessin banque 4 frame 36 dy 27 dx -75
    FF 00                      ; fin d'étape
  ; étape 27
    10 1A 1A 00 FF B4          ; dessin banque 4 frame 26 dy 26 dx -76
    10 25 16 00 FF B5          ; dessin banque 4 frame 37 dy 22 dx -75
    FF 00                      ; fin d'étape
  ; étape 28
    10 1A 17 00 FF B1          ; dessin banque 4 frame 26 dy 23 dx -79
    10 25 12 00 FF B2          ; dessin banque 4 frame 37 dy 18 dx -78
    FF 00                      ; fin d'étape
  ; étape 29
    10 1A 14 00 FF AF          ; dessin banque 4 frame 26 dy 20 dx -81
    10 24 11 00 FF AE          ; dessin banque 4 frame 36 dy 17 dx -82
    FF 00                      ; fin d'étape
  ; étape 30
    10 1A 11 00 FF AD          ; dessin banque 4 frame 26 dy 17 dx -83
    10 25 0D 00 FF AE          ; dessin banque 4 frame 37 dy 13 dx -82
    FF 00                      ; fin d'étape
  ; étape 31
    10 1A 0D 00 FF AB          ; dessin banque 4 frame 26 dy 13 dx -85
    10 25 09 00 FF AC          ; dessin banque 4 frame 37 dy 9 dx -84
    FF 00                      ; fin d'étape
  ; étape 32
    10 0B 0E 00 FF AA          ; dessin banque 4 frame 11 dy 14 dx -86
    10 2D 09 00 FF B0          ; dessin banque 4 frame 45 dy 9 dx -80
    FF 00                      ; fin d'étape
  ; étape 33
    10 0C 0D 00 FF AA          ; dessin banque 4 frame 12 dy 13 dx -86
    10 2E 0B 00 FF B3          ; dessin banque 4 frame 46 dy 11 dx -77
    FF 00                      ; fin d'étape
  ; étape 34
    10 0D 0E 00 FF AA          ; dessin banque 4 frame 13 dy 14 dx -86
    10 2E 0D 00 FF B4          ; dessin banque 4 frame 46 dy 13 dx -76
    FF 00                      ; fin d'étape
  LAB_00DF:
  ; étape 35
    88 02                      ; $88 Hold
    10 0E 0E 00 FF AA          ; dessin banque 4 frame 14 dy 14 dx -86
    10 2E 0F 00 FF B5          ; dessin banque 4 frame 46 dy 15 dx -75
    FF 00                      ; fin d'étape
  ; étape 36
    88 02                      ; $88 Hold
    84 00 00 00 1D 82          ; $84 Jump -> LAB_00DF
    10 0E 0E 00 FF AA          ; dessin banque 4 frame 14 dy 14 dx -86
    10 2B 11 00 FF B5          ; dessin banque 4 frame 43 dy 17 dx -75
    FF FF                      ; fin du script
```

### `LAB_00DF` (2:$1D82)

Rôles : cible de saut depuis LAB_00DE ; cible de saut depuis LAB_00DF ; référencé par le code dans LAB_0031

```
  ; étape 1
    88 02                      ; $88 Hold
    10 0E 0E 00 FF AA          ; dessin banque 4 frame 14 dy 14 dx -86
    10 2E 0F 00 FF B5          ; dessin banque 4 frame 46 dy 15 dx -75
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    84 00 00 00 1D 82          ; $84 Jump -> LAB_00DF
    10 0E 0E 00 FF AA          ; dessin banque 4 frame 14 dy 14 dx -86
    10 2B 11 00 FF B5          ; dessin banque 4 frame 43 dy 17 dx -75
    FF FF                      ; fin du script
```

### `LAB_00E0` (2:$1DA8)

Rôles : entrée 2 de la table LAB_0023 ; entrée 3 de la table LAB_0023

```
  ; étape 1
    10 0B 60 00 FF E4          ; dessin banque 4 frame 11 dy 96 dx -28
    10 2A 5B 00 FF EA          ; dessin banque 4 frame 42 dy 91 dx -22
    FF 00                      ; fin d'étape
  ; étape 2
    10 0B 5B 00 FF E4          ; dessin banque 4 frame 11 dy 91 dx -28
    10 2A 56 00 FF EB          ; dessin banque 4 frame 42 dy 86 dx -21
    FF 00                      ; fin d'étape
  ; étape 3
    10 0B 58 00 FF E4          ; dessin banque 4 frame 11 dy 88 dx -28
    10 2A 55 00 FF EA          ; dessin banque 4 frame 42 dy 85 dx -22
    FF 00                      ; fin d'étape
  ; étape 4
    10 0B 55 00 FF E4          ; dessin banque 4 frame 11 dy 85 dx -28
    10 2A 51 00 FF EA          ; dessin banque 4 frame 42 dy 81 dx -22
    FF 00                      ; fin d'étape
  ; étape 5
    10 0B 53 00 FF E4          ; dessin banque 4 frame 11 dy 83 dx -28
    10 25 4E 00 FF E9          ; dessin banque 4 frame 37 dy 78 dx -23
    FF 00                      ; fin d'étape
  ; étape 6
    10 0B 4F 00 FF E4          ; dessin banque 4 frame 11 dy 79 dx -28
    10 2C 4A 00 FF E8          ; dessin banque 4 frame 44 dy 74 dx -24
    FF 00                      ; fin d'étape
  ; étape 7
    10 0B 4C 00 FF E4          ; dessin banque 4 frame 11 dy 76 dx -28
    10 25 47 00 FF E9          ; dessin banque 4 frame 37 dy 71 dx -23
    FF 00                      ; fin d'étape
  ; étape 8
    10 0B 49 00 FF E4          ; dessin banque 4 frame 11 dy 73 dx -28
    10 2D 43 00 FF EA          ; dessin banque 4 frame 45 dy 67 dx -22
    FF 00                      ; fin d'étape
  ; étape 9
    10 0B 46 00 FF E4          ; dessin banque 4 frame 11 dy 70 dx -28
    10 25 40 00 FF E9          ; dessin banque 4 frame 37 dy 64 dx -23
    FF 00                      ; fin d'étape
  ; étape 10
    10 0B 43 00 FF E4          ; dessin banque 4 frame 11 dy 67 dx -28
    10 2D 3E 00 FF EA          ; dessin banque 4 frame 45 dy 62 dx -22
    FF 00                      ; fin d'étape
  ; étape 11
    10 0B 40 00 FF E4          ; dessin banque 4 frame 11 dy 64 dx -28
    10 25 3A 00 FF E9          ; dessin banque 4 frame 37 dy 58 dx -23
    FF 00                      ; fin d'étape
  ; étape 12
    10 0B 3D 00 FF E4          ; dessin banque 4 frame 11 dy 61 dx -28
    10 2D 37 00 FF EA          ; dessin banque 4 frame 45 dy 55 dx -22
    FF 00                      ; fin d'étape
  ; étape 13
    10 0B 3A 00 FF E4          ; dessin banque 4 frame 11 dy 58 dx -28
    10 25 34 00 FF E9          ; dessin banque 4 frame 37 dy 52 dx -23
    FF 00                      ; fin d'étape
  ; étape 14
    10 0B 37 00 FF E4          ; dessin banque 4 frame 11 dy 55 dx -28
    10 2C 32 00 FF E8          ; dessin banque 4 frame 44 dy 50 dx -24
    FF 00                      ; fin d'étape
  ; étape 15
    10 1A 33 00 FF E4          ; dessin banque 4 frame 26 dy 51 dx -28
    10 24 31 00 FF E3          ; dessin banque 4 frame 36 dy 49 dx -29
    FF 00                      ; fin d'étape
  ; étape 16
    10 19 31 00 FF E0          ; dessin banque 4 frame 25 dy 49 dx -32
    10 2C 2E 00 FF DD          ; dessin banque 4 frame 44 dy 46 dx -35
    FF 00                      ; fin d'étape
  ; étape 17
    10 18 2F 00 FF DC          ; dessin banque 4 frame 24 dy 47 dx -36
    10 27 32 00 FF D7          ; dessin banque 4 frame 39 dy 50 dx -41
    FF 00                      ; fin d'étape
  ; étape 18
    10 17 2D 00 FF D9          ; dessin banque 4 frame 23 dy 45 dx -39
    10 27 33 00 FF D4          ; dessin banque 4 frame 39 dy 51 dx -44
    FF 00                      ; fin d'étape
  ; étape 19
    10 18 2D 00 FF D4          ; dessin banque 4 frame 24 dy 45 dx -44
    10 2C 2D 00 FF D1          ; dessin banque 4 frame 44 dy 45 dx -47
    FF 00                      ; fin d'étape
  ; étape 20
    10 18 2C 00 FF D0          ; dessin banque 4 frame 24 dy 44 dx -48
    10 2C 2C 00 FF CD          ; dessin banque 4 frame 44 dy 44 dx -51
    FF 00                      ; fin d'étape
  ; étape 21
    10 18 2B 00 FF CC          ; dessin banque 4 frame 24 dy 43 dx -52
    10 29 2D 00 FF C9          ; dessin banque 4 frame 41 dy 45 dx -55
    FF 00                      ; fin d'étape
  ; étape 22
    10 18 29 00 FF C7          ; dessin banque 4 frame 24 dy 41 dx -57
    10 2C 28 00 FF C4          ; dessin banque 4 frame 44 dy 40 dx -60
    FF 00                      ; fin d'étape
  ; étape 23
    10 19 2A 00 FF C7          ; dessin banque 4 frame 25 dy 42 dx -57
    10 24 29 00 FF C5          ; dessin banque 4 frame 36 dy 41 dx -59
    FF 00                      ; fin d'étape
  ; étape 24
    10 1A 2A 00 FF C7          ; dessin banque 4 frame 26 dy 42 dx -57
    10 24 28 00 FF C7          ; dessin banque 4 frame 36 dy 40 dx -57
    FF 00                      ; fin d'étape
  ; étape 25
    10 0B 2B 00 FF C6          ; dessin banque 4 frame 11 dy 43 dx -58
    10 25 26 00 FF CB          ; dessin banque 4 frame 37 dy 38 dx -53
    FF 00                      ; fin d'étape
  LAB_00E1:
  ; étape 26
    88 02                      ; $88 Hold
    10 0C 2B 00 FF C6          ; dessin banque 4 frame 12 dy 43 dx -58
    10 26 29 00 FF D0          ; dessin banque 4 frame 38 dy 41 dx -48
    FF 00                      ; fin d'étape
  ; étape 27
    84 00 00 00 1F 06          ; $84 Jump -> LAB_00E1
    88 02                      ; $88 Hold
    10 0C 2B 00 FF C6          ; dessin banque 4 frame 12 dy 43 dx -58
    10 25 28 00 FF CD          ; dessin banque 4 frame 37 dy 40 dx -51
    FF FF                      ; fin du script
```

### `LAB_00E1` (2:$1F06)

Rôles : cible de saut depuis LAB_00E0 ; cible de saut depuis LAB_00E1 ; référencé par le code dans LAB_0031

```
  ; étape 1
    88 02                      ; $88 Hold
    10 0C 2B 00 FF C6          ; dessin banque 4 frame 12 dy 43 dx -58
    10 26 29 00 FF D0          ; dessin banque 4 frame 38 dy 41 dx -48
    FF 00                      ; fin d'étape
  ; étape 2
    84 00 00 00 1F 06          ; $84 Jump -> LAB_00E1
    88 02                      ; $88 Hold
    10 0C 2B 00 FF C6          ; dessin banque 4 frame 12 dy 43 dx -58
    10 25 28 00 FF CD          ; dessin banque 4 frame 37 dy 40 dx -51
    FF FF                      ; fin du script
```

### `LAB_00E2` (2:$2052)

Rôles : cible de saut depuis LAB_00E2

```
  ; étape 1
    88 02                      ; $88 Hold
    10 0B 30 00 FF D7          ; dessin banque 4 frame 11 dy 48 dx -41
    10 2D 2B 00 FF DD          ; dessin banque 4 frame 45 dy 43 dx -35
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    84 00 00 00 20 52          ; $84 Jump -> LAB_00E2
    10 0B 30 00 FF D7          ; dessin banque 4 frame 11 dy 48 dx -41
    10 2C 2C 00 FF DB          ; dessin banque 4 frame 44 dy 44 dx -37
    FF FF                      ; fin du script
```

### `LAB_00E3` (2:$2078)

Rôles : référencé par le code dans LAB_001A

```
  ; étape 1
    10 1B 5F 00 FF E4          ; dessin banque 4 frame 27 dy 95 dx -28
    FF 00                      ; fin d'étape
  ; étape 2
    10 1B 59 00 FF E4          ; dessin banque 4 frame 27 dy 89 dx -28
    FF 00                      ; fin d'étape
  ; étape 3
    10 1B 54 00 FF E4          ; dessin banque 4 frame 27 dy 84 dx -28
    FF 00                      ; fin d'étape
  ; étape 4
    10 1B 4F 00 FF E4          ; dessin banque 4 frame 27 dy 79 dx -28
    FF 00                      ; fin d'étape
  ; étape 5
    10 1B 4A 00 FF E4          ; dessin banque 4 frame 27 dy 74 dx -28
    FF 00                      ; fin d'étape
  ; étape 6
    10 1B 45 00 FF E4          ; dessin banque 4 frame 27 dy 69 dx -28
    FF 00                      ; fin d'étape
  ; étape 7
    10 1B 40 00 FF E4          ; dessin banque 4 frame 27 dy 64 dx -28
    FF 00                      ; fin d'étape
  ; étape 8
    10 1B 3B 00 FF E4          ; dessin banque 4 frame 27 dy 59 dx -28
    FF 00                      ; fin d'étape
  ; étape 9
    10 1B 36 00 FF E4          ; dessin banque 4 frame 27 dy 54 dx -28
    FF 00                      ; fin d'étape
  ; étape 10
    10 1B 31 00 FF E4          ; dessin banque 4 frame 27 dy 49 dx -28
    FF 00                      ; fin d'étape
  ; étape 11
    10 1B 2C 00 FF E4          ; dessin banque 4 frame 27 dy 44 dx -28
    FF 00                      ; fin d'étape
  ; étape 12
    10 1B 27 00 FF E4          ; dessin banque 4 frame 27 dy 39 dx -28
    FF 00                      ; fin d'étape
  ; étape 13
    10 1B 22 00 FF E4          ; dessin banque 4 frame 27 dy 34 dx -28
    FF 00                      ; fin d'étape
  ; étape 14
    10 1B 1D 00 FF E4          ; dessin banque 4 frame 27 dy 29 dx -28
    10 1B 1D 00 FF E4          ; dessin banque 4 frame 27 dy 29 dx -28
    FF 00                      ; fin d'étape
  ; étape 15
    10 1B 18 00 FF E4          ; dessin banque 4 frame 27 dy 24 dx -28
    FF 00                      ; fin d'étape
  ; étape 16
    10 1B 13 00 FF E4          ; dessin banque 4 frame 27 dy 19 dx -28
    FF 00                      ; fin d'étape
  ; étape 17
    10 1B 0E 00 FF E4          ; dessin banque 4 frame 27 dy 14 dx -28
    FF 00                      ; fin d'étape
  ; étape 18
    10 1B 09 00 FF E4          ; dessin banque 4 frame 27 dy 9 dx -28
    FF 00                      ; fin d'étape
  ; étape 19
    10 1B 04 00 FF E4          ; dessin banque 4 frame 27 dy 4 dx -28
    FF 00                      ; fin d'étape
  ; étape 20
    10 1B FF 00 FF E4          ; dessin banque 4 frame 27 dy -1 dx -28
    FF 00                      ; fin d'étape
  ; étape 21
    10 1B FC 00 FF E4          ; dessin banque 4 frame 27 dy -4 dx -28
    FF 00                      ; fin d'étape
  ; étape 22
    10 1B F7 00 FF E4          ; dessin banque 4 frame 27 dy -9 dx -28
    FF 00                      ; fin d'étape
  ; étape 23
    10 1B F2 00 FF E4          ; dessin banque 4 frame 27 dy -14 dx -28
    FF 00                      ; fin d'étape
  ; étape 24
    10 1C F1 00 FF E4          ; dessin banque 4 frame 28 dy -15 dx -28
    FF 00                      ; fin d'étape
  ; étape 25
    10 1D F1 00 FF E5          ; dessin banque 4 frame 29 dy -15 dx -27
    FF 00                      ; fin d'étape
  ; étape 26
    10 1E F0 00 FF E6          ; dessin banque 4 frame 30 dy -16 dx -26
    FF 00                      ; fin d'étape
  ; étape 27
    10 1F F0 00 FF E7          ; dessin banque 4 frame 31 dy -16 dx -25
    FF 00                      ; fin d'étape
  ; étape 28
    10 20 F0 00 FF E5          ; dessin banque 4 frame 32 dy -16 dx -27
    FF 00                      ; fin d'étape
  ; étape 29
    10 21 F1 00 FF E5          ; dessin banque 4 frame 33 dy -15 dx -27
    FF 00                      ; fin d'étape
  ; étape 30
    10 22 F1 00 FF E4          ; dessin banque 4 frame 34 dy -15 dx -28
    FF 00                      ; fin d'étape
  ; étape 31
    88 08                      ; $88 Hold
    10 23 F2 00 FF E4          ; dessin banque 4 frame 35 dy -14 dx -28
    FF FF                      ; fin du script
```

### `LAB_00E4` (2:$2178)

Rôles : cible de saut depuis LAB_00E4 ; référencé par le code dans LAB_002F

```
  ; étape 1
    84 00 00 00 21 78          ; $84 Jump -> LAB_00E4
    10 23 F2 00 FF E4          ; dessin banque 4 frame 35 dy -14 dx -28
    FF FF                      ; fin du script
```

### `LAB_00E5` (2:$2186)

Rôles : référencé par le code dans LAB_002F

```
  ; étape 1
    88 08                      ; $88 Hold
    10 02 5D 00 FF E3          ; dessin banque 4 frame 2 dy 93 dx -29
    FF 00                      ; fin d'étape
  ; étape 2
    10 01 57 00 FF E4          ; dessin banque 4 frame 1 dy 87 dx -28
    FF 00                      ; fin d'étape
  ; étape 3
    10 02 55 00 FF E4          ; dessin banque 4 frame 2 dy 85 dx -28
    FF 00                      ; fin d'étape
  ; étape 4
    10 00 4E 00 FF E4          ; dessin banque 4 frame 0 dy 78 dx -28
    FF 00                      ; fin d'étape
  ; étape 5
    10 02 4A 00 FF E4          ; dessin banque 4 frame 2 dy 74 dx -28
    FF 00                      ; fin d'étape
  ; étape 6
    10 01 42 00 FF E4          ; dessin banque 4 frame 1 dy 66 dx -28
    FF 00                      ; fin d'étape
  ; étape 7
    10 02 3D 00 FF E4          ; dessin banque 4 frame 2 dy 61 dx -28
    FF 00                      ; fin d'étape
  ; étape 8
    10 00 35 00 FF E4          ; dessin banque 4 frame 0 dy 53 dx -28
    FF 00                      ; fin d'étape
  ; étape 9
    10 02 30 00 FF E4          ; dessin banque 4 frame 2 dy 48 dx -28
    FF 00                      ; fin d'étape
  ; étape 10
    10 01 27 00 FF E4          ; dessin banque 4 frame 1 dy 39 dx -28
    FF 00                      ; fin d'étape
  ; étape 11
    10 02 22 00 FF E4          ; dessin banque 4 frame 2 dy 34 dx -28
    FF 00                      ; fin d'étape
  ; étape 12
    10 00 19 00 FF E4          ; dessin banque 4 frame 0 dy 25 dx -28
    FF 00                      ; fin d'étape
  ; étape 13
    10 02 12 00 FF E4          ; dessin banque 4 frame 2 dy 18 dx -28
    FF 00                      ; fin d'étape
  ; étape 14
    10 01 0A 00 FF E4          ; dessin banque 4 frame 1 dy 10 dx -28
    FF 00                      ; fin d'étape
  ; étape 15
    88 0A                      ; $88 Hold
    10 02 04 10 FF E4          ; dessin banque 4 frame 2 dy 4 dx -28 [décor]
    FF FF                      ; fin du script
```

### `LAB_00E6` (2:$221C)

Rôles : référencé par le code dans LAB_0036

```
  ; étape 1
    88 0A                      ; $88 Hold
    0C 00 9C 00 00 44          ; dessin banque 3 frame 0 dy -100 dx 68
    0C 02 EF 00 00 38          ; dessin banque 3 frame 2 dy -17 dx 56
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    0C 07 9C 00 00 5B          ; dessin banque 3 frame 7 dy -100 dx 91
    0C 08 A9 00 00 3E          ; dessin banque 3 frame 8 dy -87 dx 62
    0C 09 C1 00 00 3C          ; dessin banque 3 frame 9 dy -63 dx 60
    0C 0A D9 00 00 36          ; dessin banque 3 frame 10 dy -39 dx 54
    0C 0B EC 00 00 25          ; dessin banque 3 frame 11 dy -20 dx 37
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 08 DC          ; $B4 Call -> LAB_0032
    88 01                      ; $88 Hold
    0C 0E A6 00 00 36          ; dessin banque 3 frame 14 dy -90 dx 54
    0C 0F A6 00 00 59          ; dessin banque 3 frame 15 dy -90 dx 89
    0C 10 9F 00 00 61          ; dessin banque 3 frame 16 dy -97 dx 97
    0C 0D C5 00 00 27          ; dessin banque 3 frame 13 dy -59 dx 39
    0C 0C DA 00 00 00          ; dessin banque 3 frame 12 dy -38 dx 0
    FF 00                      ; fin d'étape
  ; étape 4
    0C 11 A3 00 00 35          ; dessin banque 3 frame 17 dy -93 dx 53
    0C 12 BA 00 00 27          ; dessin banque 3 frame 18 dy -70 dx 39
    0C 15 D6 00 00 27          ; dessin banque 3 frame 21 dy -42 dx 39
    0C 14 D5 00 00 12          ; dessin banque 3 frame 20 dy -43 dx 18
    0C 13 CA 00 FF F0          ; dessin banque 3 frame 19 dy -54 dx -16
    FF 00                      ; fin d'étape
  ; étape 5
    B4 00 00 00 0E E6          ; $B4 Call -> LAB_0040
    88 14                      ; $88 Hold
    0C 11 A3 00 00 35          ; dessin banque 3 frame 17 dy -93 dx 53
    0C 12 BA 00 00 27          ; dessin banque 3 frame 18 dy -70 dx 39
    0C 15 D6 00 00 27          ; dessin banque 3 frame 21 dy -42 dx 39
    0C 14 D5 00 00 12          ; dessin banque 3 frame 20 dy -43 dx 18
    0C 13 CA 00 FF F0          ; dessin banque 3 frame 19 dy -54 dx -16
    FF FF                      ; fin du script
```

### `LAB_00E7` (2:$22BE)

Rôles : entrée 0 de la table LAB_003A ; entrée 1 de la table LAB_003A ; entrée 2 de la table LAB_003A ; entrée 3 de la table LAB_003A ; entrée 4 de la table LAB_003A ; entrée 5 de la table LAB_003A ; entrée 6 de la table LAB_003A ; entrée 7 de la table LAB_003A ; entrée 8 de la table LAB_003A ; entrée 9 de la table LAB_003A

```
  ; étape 1
    00 03 44 00 00 8D          ; dessin banque 0 frame 3 dy 68 dx 141
    00 02 35 00 00 9A          ; dessin banque 0 frame 2 dy 53 dx 154
    00 01 26 00 00 9C          ; dessin banque 0 frame 1 dy 38 dx 156
    FF 00                      ; fin d'étape
  ; étape 2
    00 05 20 00 00 95          ; dessin banque 0 frame 5 dy 32 dx 149
    00 06 47 00 00 8A          ; dessin banque 0 frame 6 dy 71 dx 138
    FF 00                      ; fin d'étape
  ; étape 3
    00 08 12 00 00 94          ; dessin banque 0 frame 8 dy 18 dx 148
    00 09 26 00 00 8B          ; dessin banque 0 frame 9 dy 38 dx 139
    00 0A 48 00 00 84          ; dessin banque 0 frame 10 dy 72 dx 132
    FF 00                      ; fin d'étape
  ; étape 4
    00 0C 1B 00 00 7F          ; dessin banque 0 frame 12 dy 27 dx 127
    00 0D 2F 00 00 7F          ; dessin banque 0 frame 13 dy 47 dx 127
    00 0E 3C 00 00 74          ; dessin banque 0 frame 14 dy 60 dx 116
    00 0F 48 00 00 74          ; dessin banque 0 frame 15 dy 72 dx 116
    FF 00                      ; fin d'étape
  ; étape 5
    00 10 1D 00 00 73          ; dessin banque 0 frame 16 dy 29 dx 115
    00 12 37 00 00 74          ; dessin banque 0 frame 18 dy 55 dx 116
    00 13 47 00 00 68          ; dessin banque 0 frame 19 dy 71 dx 104
    FF 00                      ; fin d'étape
  ; étape 6
    00 14 1F 00 00 6A          ; dessin banque 0 frame 20 dy 31 dx 106
    00 15 39 00 00 6C          ; dessin banque 0 frame 21 dy 57 dx 108
    00 16 48 00 00 6A          ; dessin banque 0 frame 22 dy 72 dx 106
    00 17 4A 00 00 58          ; dessin banque 0 frame 23 dy 74 dx 88
    FF 00                      ; fin d'étape
  ; étape 7
    00 18 20 00 00 5D          ; dessin banque 0 frame 24 dy 32 dx 93
    00 19 37 00 00 4D          ; dessin banque 0 frame 25 dy 55 dx 77
    00 1A 37 00 00 86          ; dessin banque 0 frame 26 dy 55 dx 134
    00 1B 47 00 00 47          ; dessin banque 0 frame 27 dy 71 dx 71
    FF 00                      ; fin d'étape
  ; étape 8
    00 1C 25 00 00 46          ; dessin banque 0 frame 28 dy 37 dx 70
    00 1D 4C 00 00 5D          ; dessin banque 0 frame 29 dy 76 dx 93
    FF 00                      ; fin d'étape
  ; étape 9
    00 1E 1C 00 00 3D          ; dessin banque 0 frame 30 dy 28 dx 61
    00 1F 26 00 00 3D          ; dessin banque 0 frame 31 dy 38 dx 61
    00 20 4B 00 00 54          ; dessin banque 0 frame 32 dy 75 dx 84
    FF 00                      ; fin d'étape
  ; étape 10
    00 21 1C 00 00 3D          ; dessin banque 0 frame 33 dy 28 dx 61
    00 22 39 00 00 76          ; dessin banque 0 frame 34 dy 57 dx 118
    00 23 2C 00 00 62          ; dessin banque 0 frame 35 dy 44 dx 98
    00 24 45 00 00 38          ; dessin banque 0 frame 36 dy 69 dx 56
    00 25 49 00 00 77          ; dessin banque 0 frame 37 dy 73 dx 119
    FF 00                      ; fin d'étape
  ; étape 11
    00 26 19 00 00 39          ; dessin banque 0 frame 38 dy 25 dx 57
    00 27 2F 00 00 60          ; dessin banque 0 frame 39 dy 47 dx 96
    00 28 43 00 00 35          ; dessin banque 0 frame 40 dy 67 dx 53
    FF 00                      ; fin d'étape
  ; étape 12
    00 29 17 00 00 32          ; dessin banque 0 frame 41 dy 23 dx 50
    00 2A 32 00 00 28          ; dessin banque 0 frame 42 dy 50 dx 40
    00 2B 32 00 00 53          ; dessin banque 0 frame 43 dy 50 dx 83
    00 2C 47 00 00 24          ; dessin banque 0 frame 44 dy 71 dx 36
    FF 00                      ; fin d'étape
  ; étape 13
    00 2D 24 00 00 22          ; dessin banque 0 frame 45 dy 36 dx 34
    00 2E 16 00 00 32          ; dessin banque 0 frame 46 dy 22 dx 50
    FF 00                      ; fin d'étape
  ; étape 14
    00 2F 24 00 00 1D          ; dessin banque 0 frame 47 dy 36 dx 29
    00 30 10 00 00 2C          ; dessin banque 0 frame 48 dy 16 dx 44
    FF 00                      ; fin d'étape
  ; étape 15
    00 31 0E 00 00 25          ; dessin banque 0 frame 49 dy 14 dx 37
    00 32 1F 00 00 1A          ; dessin banque 0 frame 50 dy 31 dx 26
    00 33 45 00 00 0B          ; dessin banque 0 frame 51 dy 69 dx 11
    FF 00                      ; fin d'étape
  ; étape 16
    00 34 0C 00 00 1E          ; dessin banque 0 frame 52 dy 12 dx 30
    00 35 19 00 00 0E          ; dessin banque 0 frame 53 dy 25 dx 14
    00 36 45 00 00 04          ; dessin banque 0 frame 54 dy 69 dx 4
    FF 00                      ; fin d'étape
  ; étape 17
    00 00 06 00 00 0C          ; dessin banque 0 frame 0 dy 6 dx 12
    00 01 26 00 00 08          ; dessin banque 0 frame 1 dy 38 dx 8
    00 02 35 00 00 06          ; dessin banque 0 frame 2 dy 53 dx 6
    00 03 44 00 FF F9          ; dessin banque 0 frame 3 dy 68 dx -7
    FF 00                      ; fin d'étape
  ; étape 18
    00 04 03 00 00 0B          ; dessin banque 0 frame 4 dy 3 dx 11
    00 05 20 00 00 01          ; dessin banque 0 frame 5 dy 32 dx 1
    00 06 47 00 FF F6          ; dessin banque 0 frame 6 dy 71 dx -10
    FF 00                      ; fin d'étape
  ; étape 19
    00 07 07 00 00 23          ; dessin banque 0 frame 7 dy 7 dx 35
    00 08 12 00 00 00          ; dessin banque 0 frame 8 dy 18 dx 0
    00 09 26 00 FF F7          ; dessin banque 0 frame 9 dy 38 dx -9
    00 0A 48 00 FF F0          ; dessin banque 0 frame 10 dy 72 dx -16
    FF 00                      ; fin d'étape
  ; étape 20
    00 0B 15 00 00 27          ; dessin banque 0 frame 11 dy 21 dx 39
    00 0C 1B 00 FF EB          ; dessin banque 0 frame 12 dy 27 dx -21
    00 0D 2F 00 FF EB          ; dessin banque 0 frame 13 dy 47 dx -21
    00 0E 3C 00 FF E0          ; dessin banque 0 frame 14 dy 60 dx -32
    00 0F 48 00 FF E0          ; dessin banque 0 frame 15 dy 72 dx -32
    FF 00                      ; fin d'étape
  ; étape 21
    00 10 1D 00 FF DF          ; dessin banque 0 frame 16 dy 29 dx -33
    00 11 23 00 00 12          ; dessin banque 0 frame 17 dy 35 dx 18
    00 12 37 00 FF E0          ; dessin banque 0 frame 18 dy 55 dx -32
    00 13 47 00 FF D4          ; dessin banque 0 frame 19 dy 71 dx -44
    FF 00                      ; fin d'étape
  ; étape 22
    00 14 1F 00 FF D6          ; dessin banque 0 frame 20 dy 31 dx -42
    00 15 39 00 FF D8          ; dessin banque 0 frame 21 dy 57 dx -40
    00 16 48 00 FF D6          ; dessin banque 0 frame 22 dy 72 dx -42
    00 17 4A 00 FF C4          ; dessin banque 0 frame 23 dy 74 dx -60
    FF 00                      ; fin d'étape
  ; étape 23
    00 18 20 00 FF C9          ; dessin banque 0 frame 24 dy 32 dx -55
    00 19 37 00 FF B9          ; dessin banque 0 frame 25 dy 55 dx -71
    00 1A 37 00 FF F2          ; dessin banque 0 frame 26 dy 55 dx -14
    00 1B 47 00 FF B3          ; dessin banque 0 frame 27 dy 71 dx -77
    FF 00                      ; fin d'étape
  ; étape 24
    00 1C 25 00 FF B2          ; dessin banque 0 frame 28 dy 37 dx -78
    00 1D 4C 00 FF C9          ; dessin banque 0 frame 29 dy 76 dx -55
    FF 00                      ; fin d'étape
  ; étape 25
    00 1E 1C 00 FF A9          ; dessin banque 0 frame 30 dy 28 dx -87
    00 1F 26 00 FF A9          ; dessin banque 0 frame 31 dy 38 dx -87
    00 20 4B 00 FF C0          ; dessin banque 0 frame 32 dy 75 dx -64
    FF 00                      ; fin d'étape
  ; étape 26
    00 21 1C 00 FF A9          ; dessin banque 0 frame 33 dy 28 dx -87
    00 22 39 00 FF E2          ; dessin banque 0 frame 34 dy 57 dx -30
    00 23 2C 00 FF CE          ; dessin banque 0 frame 35 dy 44 dx -50
    00 24 45 00 FF A4          ; dessin banque 0 frame 36 dy 69 dx -92
    00 25 49 00 FF E3          ; dessin banque 0 frame 37 dy 73 dx -29
    FF 00                      ; fin d'étape
  ; étape 27
    00 26 19 00 FF A5          ; dessin banque 0 frame 38 dy 25 dx -91
    00 27 2F 00 FF CC          ; dessin banque 0 frame 39 dy 47 dx -52
    00 28 43 00 FF A1          ; dessin banque 0 frame 40 dy 67 dx -95
    FF 00                      ; fin d'étape
  ; étape 28
    00 29 17 00 FF 9E          ; dessin banque 0 frame 41 dy 23 dx -98
    00 2A 32 00 FF 94          ; dessin banque 0 frame 42 dy 50 dx -108
    00 2B 32 00 FF BF          ; dessin banque 0 frame 43 dy 50 dx -65
    00 2C 47 00 FF 90          ; dessin banque 0 frame 44 dy 71 dx -112
    FF 00                      ; fin d'étape
  ; étape 29
    00 2D 24 00 FF 8E          ; dessin banque 0 frame 45 dy 36 dx -114
    00 2E 16 00 FF 9E          ; dessin banque 0 frame 46 dy 22 dx -98
    FF 00                      ; fin d'étape
  ; étape 30
    00 2F 24 00 FF 89          ; dessin banque 0 frame 47 dy 36 dx -119
    00 30 10 00 FF 98          ; dessin banque 0 frame 48 dy 16 dx -104
    FF 00                      ; fin d'étape
  ; étape 31
    00 31 0E 00 FF 91          ; dessin banque 0 frame 49 dy 14 dx -111
    00 32 1F 00 FF 86          ; dessin banque 0 frame 50 dy 31 dx -122
    00 33 45 00 FF 77          ; dessin banque 0 frame 51 dy 69 dx -137
    FF 00                      ; fin d'étape
  ; étape 32
    00 34 0C 00 FF 8A          ; dessin banque 0 frame 52 dy 12 dx -118
    00 35 19 00 FF 7A          ; dessin banque 0 frame 53 dy 25 dx -134
    00 36 45 00 FF 70          ; dessin banque 0 frame 54 dy 69 dx -144
    FF 00                      ; fin d'étape
  ; étape 33
    00 00 06 00 FF 78          ; dessin banque 0 frame 0 dy 6 dx -136
    00 01 26 00 FF 74          ; dessin banque 0 frame 1 dy 38 dx -140
    00 02 35 00 FF 72          ; dessin banque 0 frame 2 dy 53 dx -142
    00 03 44 00 FF 65          ; dessin banque 0 frame 3 dy 68 dx -155
    FF 00                      ; fin d'étape
  ; étape 34
    00 04 03 00 FF 77          ; dessin banque 0 frame 4 dy 3 dx -137
    00 05 20 00 FF 6D          ; dessin banque 0 frame 5 dy 32 dx -147
    00 06 47 00 FF 62          ; dessin banque 0 frame 6 dy 71 dx -158
    FF 00                      ; fin d'étape
  ; étape 35
    00 07 07 00 FF 8F          ; dessin banque 0 frame 7 dy 7 dx -113
    00 08 12 00 FF 6C          ; dessin banque 0 frame 8 dy 18 dx -148
    00 09 26 00 FF 63          ; dessin banque 0 frame 9 dy 38 dx -157
    00 0A 48 00 FF 5C          ; dessin banque 0 frame 10 dy 72 dx -164
    FF 00                      ; fin d'étape
  ; étape 36
    00 0B 15 00 FF 93          ; dessin banque 0 frame 11 dy 21 dx -109
    00 0C 1B 00 FF 57          ; dessin banque 0 frame 12 dy 27 dx -169
    00 0D 2F 00 FF 57          ; dessin banque 0 frame 13 dy 47 dx -169
    00 0E 3C 00 FF 4C          ; dessin banque 0 frame 14 dy 60 dx -180
    00 0F 48 00 FF 4C          ; dessin banque 0 frame 15 dy 72 dx -180
    FF 00                      ; fin d'étape
  ; étape 37
    00 10 1D 00 FF 4B          ; dessin banque 0 frame 16 dy 29 dx -181
    00 11 23 00 FF 7E          ; dessin banque 0 frame 17 dy 35 dx -130
    00 12 37 00 FF 4C          ; dessin banque 0 frame 18 dy 55 dx -180
    00 13 47 00 FF 40          ; dessin banque 0 frame 19 dy 71 dx -192
    FF 00                      ; fin d'étape
  ; étape 38
    00 14 1F 00 FF 42          ; dessin banque 0 frame 20 dy 31 dx -190
    00 15 39 00 FF 44          ; dessin banque 0 frame 21 dy 57 dx -188
    00 16 48 00 FF 42          ; dessin banque 0 frame 22 dy 72 dx -190
    00 17 4A 00 FF 30          ; dessin banque 0 frame 23 dy 74 dx -208
    FF 00                      ; fin d'étape
  ; étape 39
    00 18 20 00 FF 35          ; dessin banque 0 frame 24 dy 32 dx -203
    00 1A 37 00 FF 5E          ; dessin banque 0 frame 26 dy 55 dx -162
    00 1B 47 00 FF 1F          ; dessin banque 0 frame 27 dy 71 dx -225
    FF 00                      ; fin d'étape
  ; étape 40
    00 1C 25 00 FF 1E          ; dessin banque 0 frame 28 dy 37 dx -226
    FF FF                      ; fin du script
```

### `LAB_00E8` (2:$262C)

Rôles : référencé par le code dans LAB_0038

```
  ; étape 1
    88 0F                      ; $88 Hold
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 02 EB 20 FF EC          ; dessin banque 5 frame 2 dy -21 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    FF 00                      ; fin d'étape
  ; étape 2
    88 02                      ; $88 Hold
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 03 EB 20 FF EC          ; dessin banque 5 frame 3 dy -21 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    FF 00                      ; fin d'étape
  ; étape 3
    88 02                      ; $88 Hold
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    14 04 E6 20 FF EC          ; dessin banque 5 frame 4 dy -26 dx -20
    FF 00                      ; fin d'étape
  ; étape 4
    88 05                      ; $88 Hold
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 05 DE 20 FF EB          ; dessin banque 5 frame 5 dy -34 dx -21
    FF 00                      ; fin d'étape
  ; étape 5
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 6
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 7
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    94 05                      ; $94 Loop
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    14 19 9C 20 FF EC          ; dessin banque 5 frame 25 dy -100 dx -20
    FF 00                      ; fin d'étape
  ; étape 9
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    14 1A 9C 20 FF F1          ; dessin banque 5 frame 26 dy -100 dx -15
    FF 00                      ; fin d'étape
  ; étape 10
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    14 1B 9C 20 FF F7          ; dessin banque 5 frame 27 dy -100 dx -9
    FF 00                      ; fin d'étape
  ; étape 11
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    14 19 9C 20 FF EB          ; dessin banque 5 frame 25 dy -100 dx -21
    FF 00                      ; fin d'étape
  ; étape 12
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    14 1B 9C 20 FF F7          ; dessin banque 5 frame 27 dy -100 dx -9
    FF FE                      ; fin d'étape, boucle
  ; étape 13
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    14 04 E6 20 FF EC          ; dessin banque 5 frame 4 dy -26 dx -20
    FF 00                      ; fin d'étape
  ; étape 14
    88 0A                      ; $88 Hold
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 02 EB 20 FF EC          ; dessin banque 5 frame 2 dy -21 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    FF FF                      ; fin du script
```

### `LAB_00E9` (2:$2738)

Rôles : cible de saut depuis LAB_00E9 ; référencé par le code dans LAB_0037 ; référencé par le code dans LAB_0038 ; référencé par le code dans LAB_0039

```
  ; étape 1
    14 07 F3 20 00 65          ; dessin banque 5 frame 7 dy -13 dx 101
    14 0A EF 20 FF 84          ; dessin banque 5 frame 10 dy -17 dx -124
    14 0D FE 20 00 44          ; dessin banque 5 frame 13 dy -2 dx 68
    14 0E FC 20 FF AA          ; dessin banque 5 frame 14 dy -4 dx -86
    14 10 03 20 00 29          ; dessin banque 5 frame 16 dy 3 dx 41
    14 11 01 20 FF C5          ; dessin banque 5 frame 17 dy 1 dx -59
    14 14 26 20 00 4D          ; dessin banque 5 frame 20 dy 38 dx 77
    14 15 27 20 FF 9F          ; dessin banque 5 frame 21 dy 39 dx -97
    14 16 17 20 00 2A          ; dessin banque 5 frame 22 dy 23 dx 42
    14 18 19 20 FF C5          ; dessin banque 5 frame 24 dy 25 dx -59
    FF 00                      ; fin d'étape
  ; étape 2
    14 08 F2 20 00 63          ; dessin banque 5 frame 8 dy -14 dx 99
    14 0B EC 20 FF 84          ; dessin banque 5 frame 11 dy -20 dx -124
    14 0E FD 20 00 42          ; dessin banque 5 frame 14 dy -3 dx 66
    14 0F FB 20 FF AB          ; dessin banque 5 frame 15 dy -5 dx -85
    14 11 01 20 00 28          ; dessin banque 5 frame 17 dy 1 dx 40
    14 12 00 20 FF C6          ; dessin banque 5 frame 18 dy 0 dx -58
    14 13 26 20 FF 9F          ; dessin banque 5 frame 19 dy 38 dx -97
    14 15 27 20 00 4D          ; dessin banque 5 frame 21 dy 39 dx 77
    14 17 16 20 00 2A          ; dessin banque 5 frame 23 dy 22 dx 42
    14 16 17 20 FF C5          ; dessin banque 5 frame 22 dy 23 dx -59
    FF 00                      ; fin d'étape
  ; étape 3
    84 00 00 00 27 38          ; $84 Jump -> LAB_00E9
    14 09 EF 20 00 65          ; dessin banque 5 frame 9 dy -17 dx 101
    14 0C F3 20 FF 84          ; dessin banque 5 frame 12 dy -13 dx -124
    14 0F FC 20 00 44          ; dessin banque 5 frame 15 dy -4 dx 68
    14 0D FD 20 FF AC          ; dessin banque 5 frame 13 dy -3 dx -84
    14 12 00 20 00 29          ; dessin banque 5 frame 18 dy 0 dx 41
    14 10 03 20 FF C5          ; dessin banque 5 frame 16 dy 3 dx -59
    14 13 26 20 00 4D          ; dessin banque 5 frame 19 dy 38 dx 77
    14 14 26 20 FF 9F          ; dessin banque 5 frame 20 dy 38 dx -97
    14 18 19 20 00 2A          ; dessin banque 5 frame 24 dy 25 dx 42
    14 17 16 20 FF C5          ; dessin banque 5 frame 23 dy 22 dx -59
    FF FF                      ; fin du script
```

### `LAB_00EA` (2:$27F8)

Rôles : cible de saut depuis LAB_00EA ; référencé par le code dans LAB_0037

```
  ; étape 1
    84 00 00 00 27 F8          ; $84 Jump -> LAB_00EA
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 02 EB 20 FF EC          ; dessin banque 5 frame 2 dy -21 dx -20
    14 00 DA 20 FF F1          ; dessin banque 5 frame 0 dy -38 dx -15
    FF 00                      ; fin d'étape
  LAB_00EB:
  ; étape 2
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 3
    14 1E B5 20 FF E6          ; dessin banque 5 frame 30 dy -75 dx -26
    14 1D A8 20 FF ED          ; dessin banque 5 frame 29 dy -88 dx -19
    14 1C 9C 20 FF F1          ; dessin banque 5 frame 28 dy -100 dx -15
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 4
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 5
    14 23 C9 20 FF ED          ; dessin banque 5 frame 35 dy -55 dx -19
    14 22 B4 20 FF FB          ; dessin banque 5 frame 34 dy -76 dx -5
    14 21 9C 20 00 06          ; dessin banque 5 frame 33 dy -100 dx 6
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 6
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 7
    14 1E B5 20 FF E6          ; dessin banque 5 frame 30 dy -75 dx -26
    14 1D A8 20 FF ED          ; dessin banque 5 frame 29 dy -88 dx -19
    14 1C 9C 20 FF F1          ; dessin banque 5 frame 28 dy -100 dx -15
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 8
    14 23 C9 20 FF ED          ; dessin banque 5 frame 35 dy -55 dx -19
    14 22 B4 20 FF FB          ; dessin banque 5 frame 34 dy -76 dx -5
    14 21 9C 20 00 06          ; dessin banque 5 frame 33 dy -100 dx 6
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 9
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 10
    14 26 FE 20 FF EC          ; dessin banque 5 frame 38 dy -2 dx -20
    14 24 DD 20 FF EF          ; dessin banque 5 frame 36 dy -35 dx -17
    14 27 CD 20 FF ED          ; dessin banque 5 frame 39 dy -51 dx -19
    FF 00                      ; fin d'étape
  ; étape 11
    14 26 FA 20 FF EC          ; dessin banque 5 frame 38 dy -6 dx -20
    14 24 D9 20 FF EF          ; dessin banque 5 frame 36 dy -39 dx -17
    14 27 C9 20 FF ED          ; dessin banque 5 frame 39 dy -55 dx -19
    FF 00                      ; fin d'étape
  ; étape 12
    14 26 F2 20 FF EC          ; dessin banque 5 frame 38 dy -14 dx -20
    14 24 D1 20 FF EF          ; dessin banque 5 frame 36 dy -47 dx -17
    14 27 C1 20 FF ED          ; dessin banque 5 frame 39 dy -63 dx -19
    FF 00                      ; fin d'étape
  ; étape 13
    14 26 E2 20 FF EC          ; dessin banque 5 frame 38 dy -30 dx -20
    14 24 C1 20 FF EF          ; dessin banque 5 frame 36 dy -63 dx -17
    14 27 B1 20 FF ED          ; dessin banque 5 frame 39 dy -79 dx -19
    FF 00                      ; fin d'étape
  ; étape 14
    14 26 D2 20 FF EC          ; dessin banque 5 frame 38 dy -46 dx -20
    14 24 B1 20 FF EF          ; dessin banque 5 frame 36 dy -79 dx -17
    14 27 A1 20 FF ED          ; dessin banque 5 frame 39 dy -95 dx -19
    FF 00                      ; fin d'étape
  ; étape 15
    14 26 BD 20 FF EC          ; dessin banque 5 frame 38 dy -67 dx -20
    14 24 9C 20 FF EF          ; dessin banque 5 frame 36 dy -100 dx -17
    FF 00                      ; fin d'étape
  ; étape 16
    14 26 9C 20 FF EC          ; dessin banque 5 frame 38 dy -100 dx -20
    FF FF                      ; fin du script
```

### `LAB_00EB` (2:$2812)

Rôles : référencé par le code dans LAB_0039

```
  ; étape 1
    14 01 F2 20 FF EC          ; dessin banque 5 frame 1 dy -14 dx -20
    14 06 D8 20 FF EF          ; dessin banque 5 frame 6 dy -40 dx -17
    FF 00                      ; fin d'étape
  ; étape 2
    14 1E B5 20 FF E6          ; dessin banque 5 frame 30 dy -75 dx -26
    14 1D A8 20 FF ED          ; dessin banque 5 frame 29 dy -88 dx -19
    14 1C 9C 20 FF F1          ; dessin banque 5 frame 28 dy -100 dx -15
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 3
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 4
    14 23 C9 20 FF ED          ; dessin banque 5 frame 35 dy -55 dx -19
    14 22 B4 20 FF FB          ; dessin banque 5 frame 34 dy -76 dx -5
    14 21 9C 20 00 06          ; dessin banque 5 frame 33 dy -100 dx 6
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 5
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 6
    14 1E B5 20 FF E6          ; dessin banque 5 frame 30 dy -75 dx -26
    14 1D A8 20 FF ED          ; dessin banque 5 frame 29 dy -88 dx -19
    14 1C 9C 20 FF F1          ; dessin banque 5 frame 28 dy -100 dx -15
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 7
    14 23 C9 20 FF ED          ; dessin banque 5 frame 35 dy -55 dx -19
    14 22 B4 20 FF FB          ; dessin banque 5 frame 34 dy -76 dx -5
    14 21 9C 20 00 06          ; dessin banque 5 frame 33 dy -100 dx 6
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 8
    14 20 D3 20 FF EB          ; dessin banque 5 frame 32 dy -45 dx -21
    14 1F 9C 20 FF E6          ; dessin banque 5 frame 31 dy -100 dx -26
    14 24 E5 20 FF EF          ; dessin banque 5 frame 36 dy -27 dx -17
    14 25 07 20 FF EB          ; dessin banque 5 frame 37 dy 7 dx -21
    FF 00                      ; fin d'étape
  ; étape 9
    14 26 FE 20 FF EC          ; dessin banque 5 frame 38 dy -2 dx -20
    14 24 DD 20 FF EF          ; dessin banque 5 frame 36 dy -35 dx -17
    14 27 CD 20 FF ED          ; dessin banque 5 frame 39 dy -51 dx -19
    FF 00                      ; fin d'étape
  ; étape 10
    14 26 FA 20 FF EC          ; dessin banque 5 frame 38 dy -6 dx -20
    14 24 D9 20 FF EF          ; dessin banque 5 frame 36 dy -39 dx -17
    14 27 C9 20 FF ED          ; dessin banque 5 frame 39 dy -55 dx -19
    FF 00                      ; fin d'étape
  ; étape 11
    14 26 F2 20 FF EC          ; dessin banque 5 frame 38 dy -14 dx -20
    14 24 D1 20 FF EF          ; dessin banque 5 frame 36 dy -47 dx -17
    14 27 C1 20 FF ED          ; dessin banque 5 frame 39 dy -63 dx -19
    FF 00                      ; fin d'étape
  ; étape 12
    14 26 E2 20 FF EC          ; dessin banque 5 frame 38 dy -30 dx -20
    14 24 C1 20 FF EF          ; dessin banque 5 frame 36 dy -63 dx -17
    14 27 B1 20 FF ED          ; dessin banque 5 frame 39 dy -79 dx -19
    FF 00                      ; fin d'étape
  ; étape 13
    14 26 D2 20 FF EC          ; dessin banque 5 frame 38 dy -46 dx -20
    14 24 B1 20 FF EF          ; dessin banque 5 frame 36 dy -79 dx -17
    14 27 A1 20 FF ED          ; dessin banque 5 frame 39 dy -95 dx -19
    FF 00                      ; fin d'étape
  ; étape 14
    14 26 BD 20 FF EC          ; dessin banque 5 frame 38 dy -67 dx -20
    14 24 9C 20 FF EF          ; dessin banque 5 frame 36 dy -100 dx -17
    FF 00                      ; fin d'étape
  ; étape 15
    14 26 9C 20 FF EC          ; dessin banque 5 frame 38 dy -100 dx -20
    FF FF                      ; fin du script
```

### `LAB_00EC` (2:$2968)

Rôles : référencé par le code dans LAB_0039

```
  ; étape 1
    14 29 E9 20 FF F3          ; dessin banque 5 frame 41 dy -23 dx -13
    14 28 02 20 FF E7          ; dessin banque 5 frame 40 dy 2 dx -25
    FF 00                      ; fin d'étape
  ; étape 2
    14 2A E1 20 FF E8          ; dessin banque 5 frame 42 dy -31 dx -24
    FF 00                      ; fin d'étape
  ; étape 3
    14 2B DD 20 FF EA          ; dessin banque 5 frame 43 dy -35 dx -22
    FF 00                      ; fin d'étape
  ; étape 4
    14 2C DB 20 FF EC          ; dessin banque 5 frame 44 dy -37 dx -20
    FF 00                      ; fin d'étape
  ; étape 5
    14 2D DA 20 FF EE          ; dessin banque 5 frame 45 dy -38 dx -18
    FF 00                      ; fin d'étape
  ; étape 6
    14 2E D8 20 FF EE          ; dessin banque 5 frame 46 dy -40 dx -18
    FF 00                      ; fin d'étape
  ; étape 7
    14 2F D6 20 FF EF          ; dessin banque 5 frame 47 dy -42 dx -17
    FF 00                      ; fin d'étape
  ; étape 8
    14 30 D4 20 FF EF          ; dessin banque 5 frame 48 dy -44 dx -17
    FF 00                      ; fin d'étape
  ; étape 9
    14 31 D2 20 FF F0          ; dessin banque 5 frame 49 dy -46 dx -16
    FF 00                      ; fin d'étape
  ; étape 10
    14 32 D2 20 FF F1          ; dessin banque 5 frame 50 dy -46 dx -15
    FF 00                      ; fin d'étape
  ; étape 11
    14 33 D1 20 FF F2          ; dessin banque 5 frame 51 dy -47 dx -14
    FF 00                      ; fin d'étape
  ; étape 12
    14 34 D1 20 FF F2          ; dessin banque 5 frame 52 dy -47 dx -14
    FF 00                      ; fin d'étape
  ; étape 13
    14 35 D0 20 FF F3          ; dessin banque 5 frame 53 dy -48 dx -13
    FF 00                      ; fin d'étape
  ; étape 14
    14 36 CE 20 FF F6          ; dessin banque 5 frame 54 dy -50 dx -10
    FF FF                      ; fin du script
```

### `LAB_00ED` (2:$29DE)

Rôles : référencé par le code dans LAB_0036

```
  ; étape 1
    10 02 5D 00 FF E3          ; dessin banque 4 frame 2 dy 93 dx -29
    10 23 B0 10 FF E5          ; dessin banque 4 frame 35 dy -80 dx -27 [décor]
    FF 00                      ; fin d'étape
  ; étape 2
    10 01 57 00 FF E4          ; dessin banque 4 frame 1 dy 87 dx -28
    10 23 B0 10 FF E5          ; dessin banque 4 frame 35 dy -80 dx -27 [décor]
    FF 00                      ; fin d'étape
  ; étape 3
    10 02 55 00 FF E4          ; dessin banque 4 frame 2 dy 85 dx -28
    10 23 B0 00 FF E5          ; dessin banque 4 frame 35 dy -80 dx -27
    FF 00                      ; fin d'étape
  ; étape 4
    10 00 4E 00 FF E4          ; dessin banque 4 frame 0 dy 78 dx -28
    10 23 B0 00 FF E5          ; dessin banque 4 frame 35 dy -80 dx -27
    FF 00                      ; fin d'étape
  ; étape 5
    10 02 4A 00 FF E4          ; dessin banque 4 frame 2 dy 74 dx -28
    FF 00                      ; fin d'étape
  ; étape 6
    10 01 42 00 FF E4          ; dessin banque 4 frame 1 dy 66 dx -28
    FF 00                      ; fin d'étape
  ; étape 7
    10 02 3D 00 FF E4          ; dessin banque 4 frame 2 dy 61 dx -28
    10 23 B0 00 FF E5          ; dessin banque 4 frame 35 dy -80 dx -27
    FF 00                      ; fin d'étape
  ; étape 8
    10 00 35 00 FF E4          ; dessin banque 4 frame 0 dy 53 dx -28
    FF 00                      ; fin d'étape
  ; étape 9
    10 02 30 00 FF E4          ; dessin banque 4 frame 2 dy 48 dx -28
    FF 00                      ; fin d'étape
  ; étape 10
    10 01 27 00 FF E4          ; dessin banque 4 frame 1 dy 39 dx -28
    FF 00                      ; fin d'étape
  ; étape 11
    10 02 22 00 FF E4          ; dessin banque 4 frame 2 dy 34 dx -28
    FF 00                      ; fin d'étape
  ; étape 12
    10 00 19 00 FF E4          ; dessin banque 4 frame 0 dy 25 dx -28
    FF 00                      ; fin d'étape
  ; étape 13
    10 02 12 00 FF E4          ; dessin banque 4 frame 2 dy 18 dx -28
    FF 00                      ; fin d'étape
  ; étape 14
    10 01 0A 00 FF E4          ; dessin banque 4 frame 1 dy 10 dx -28
    FF 00                      ; fin d'étape
  ; étape 15
    10 02 04 00 FF E4          ; dessin banque 4 frame 2 dy 4 dx -28
    FF 00                      ; fin d'étape
  ; étape 16
    10 00 FC 00 FF E4          ; dessin banque 4 frame 0 dy -4 dx -28
    FF 00                      ; fin d'étape
  ; étape 17
    10 02 F7 00 FF E4          ; dessin banque 4 frame 2 dy -9 dx -28
    FF 00                      ; fin d'étape
  ; étape 18
    10 02 F3 00 FF E3          ; dessin banque 4 frame 2 dy -13 dx -29
    FF 00                      ; fin d'étape
  ; étape 19
    10 03 F2 00 FF E4          ; dessin banque 4 frame 3 dy -14 dx -28
    FF 00                      ; fin d'étape
  ; étape 20
    10 04 F1 00 FF E5          ; dessin banque 4 frame 4 dy -15 dx -27
    FF 00                      ; fin d'étape
  ; étape 21
    10 05 F0 00 FF E6          ; dessin banque 4 frame 5 dy -16 dx -26
    FF 00                      ; fin d'étape
  ; étape 22
    10 06 EF 00 FF E8          ; dessin banque 4 frame 6 dy -17 dx -24
    FF 00                      ; fin d'étape
  ; étape 23
    10 07 F1 00 FF E6          ; dessin banque 4 frame 7 dy -15 dx -26
    FF 00                      ; fin d'étape
  ; étape 24
    10 08 F1 00 FF E5          ; dessin banque 4 frame 8 dy -15 dx -27
    FF 00                      ; fin d'étape
  ; étape 25
    10 09 F2 00 FF E4          ; dessin banque 4 frame 9 dy -14 dx -28
    FF 00                      ; fin d'étape
  ; étape 26
    88 0F                      ; $88 Hold
    10 0A F4 00 FF E3          ; dessin banque 4 frame 10 dy -12 dx -29
    FF FF                      ; fin du script
```

### `LAB_00EE` (2:$2ACE)

Rôles : référencé par le code dans LAB_003B

```
  ; étape 1
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 00 1D 00 FF EF          ; dessin banque 2 frame 0 dy 29 dx -17
    FF 00                      ; fin d'étape
  ; étape 2
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 04 19 00 FF EE          ; dessin banque 2 frame 4 dy 25 dx -18
    08 02 06 00 FF E3          ; dessin banque 2 frame 2 dy 6 dx -29
    08 01 FB 00 FF E6          ; dessin banque 2 frame 1 dy -5 dx -26
    08 03 0D 00 00 02          ; dessin banque 2 frame 3 dy 13 dx 2
    FF 00                      ; fin d'étape
  ; étape 3
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 07 F7 00 FF F0          ; dessin banque 2 frame 7 dy -9 dx -16
    08 06 F7 00 FF DE          ; dessin banque 2 frame 6 dy -9 dx -34
    08 05 EB 00 FF E3          ; dessin banque 2 frame 5 dy -21 dx -29
    08 08 18 00 FF E9          ; dessin banque 2 frame 8 dy 24 dx -23
    FF 00                      ; fin d'étape
  ; étape 4
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 0C 0B 00 FF F2          ; dessin banque 2 frame 12 dy 11 dx -14
    08 0B 20 00 FF E7          ; dessin banque 2 frame 11 dy 32 dx -25
    08 0A F5 00 FF DA          ; dessin banque 2 frame 10 dy -11 dx -38
    08 09 DA 00 FF DA          ; dessin banque 2 frame 9 dy -38 dx -38
    08 0E 33 00 00 0E          ; dessin banque 2 frame 14 dy 51 dx 14
    FF 00                      ; fin d'étape
  ; étape 5
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 0F 32 00 00 10          ; dessin banque 2 frame 15 dy 50 dx 16
    08 0F C8 00 FF E5          ; dessin banque 2 frame 15 dy -56 dx -27
    08 0F D5 00 FF D9          ; dessin banque 2 frame 15 dy -43 dx -39
    08 0F E8 00 FF CF          ; dessin banque 2 frame 15 dy -24 dx -49
    08 0F E9 00 FF E5          ; dessin banque 2 frame 15 dy -23 dx -27
    08 0F DF 00 FF F6          ; dessin banque 2 frame 15 dy -33 dx -10
    08 0F EF 00 00 01          ; dessin banque 2 frame 15 dy -17 dx 1
    08 0F F4 00 00 0F          ; dessin banque 2 frame 15 dy -12 dx 15
    08 0F FC 00 00 0D          ; dessin banque 2 frame 15 dy -4 dx 13
    08 0F 05 00 FF FD          ; dessin banque 2 frame 15 dy 5 dx -3
    08 0F 03 00 FF F6          ; dessin banque 2 frame 15 dy 3 dx -10
    08 0F 05 00 FF EE          ; dessin banque 2 frame 15 dy 5 dx -18
    08 0F 1E 00 FF E9          ; dessin banque 2 frame 15 dy 30 dx -23
    08 0F 1C 00 00 07          ; dessin banque 2 frame 15 dy 28 dx 7
    08 0F 34 00 FF DD          ; dessin banque 2 frame 15 dy 52 dx -35
    FF 00                      ; fin d'étape
  ; étape 6
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 0F BE 00 FF F0          ; dessin banque 2 frame 15 dy -66 dx -16
    08 0F CD 00 FF DA          ; dessin banque 2 frame 15 dy -51 dx -38
    08 0F E0 00 FF CD          ; dessin banque 2 frame 15 dy -32 dx -51
    08 0F E4 00 FF E3          ; dessin banque 2 frame 15 dy -28 dx -29
    08 0F D8 00 FF F6          ; dessin banque 2 frame 15 dy -40 dx -10
    08 0F E8 00 00 02          ; dessin banque 2 frame 15 dy -24 dx 2
    08 0F EE 00 00 10          ; dessin banque 2 frame 15 dy -18 dx 16
    08 0F F6 00 00 0E          ; dessin banque 2 frame 15 dy -10 dx 14
    08 0F FE 00 FF FE          ; dessin banque 2 frame 15 dy -2 dx -2
    08 0F FB 00 FF F6          ; dessin banque 2 frame 15 dy -5 dx -10
    08 0F FE 00 FF EC          ; dessin banque 2 frame 15 dy -2 dx -20
    08 0F 1A 00 FF E7          ; dessin banque 2 frame 15 dy 26 dx -25
    08 0F 18 00 00 0A          ; dessin banque 2 frame 15 dy 24 dx 10
    08 0F 31 00 00 14          ; dessin banque 2 frame 15 dy 49 dx 20
    08 0F 34 00 FF D6          ; dessin banque 2 frame 15 dy 52 dx -42
    FF 00                      ; fin d'étape
  ; étape 7
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 0F B2 00 FF FC          ; dessin banque 2 frame 15 dy -78 dx -4
    08 0F C4 00 FF DB          ; dessin banque 2 frame 15 dy -60 dx -37
    08 0F D8 00 FF CC          ; dessin banque 2 frame 15 dy -40 dx -52
    08 0F DD 00 FF E1          ; dessin banque 2 frame 15 dy -35 dx -31
    08 0F CF 00 FF F5          ; dessin banque 2 frame 15 dy -49 dx -11
    08 0F E3 00 00 02          ; dessin banque 2 frame 15 dy -29 dx 2
    08 0F E7 00 00 10          ; dessin banque 2 frame 15 dy -25 dx 16
    08 0F F3 00 00 0E          ; dessin banque 2 frame 15 dy -13 dx 14
    08 0F F7 00 FF FD          ; dessin banque 2 frame 15 dy -9 dx -3
    08 0F F4 00 FF F4          ; dessin banque 2 frame 15 dy -12 dx -12
    08 0F F6 00 FF EA          ; dessin banque 2 frame 15 dy -10 dx -22
    08 0F 17 00 FF E6          ; dessin banque 2 frame 15 dy 23 dx -26
    08 0F 13 00 00 0C          ; dessin banque 2 frame 15 dy 19 dx 12
    08 0F 31 00 00 18          ; dessin banque 2 frame 15 dy 49 dx 24
    08 0F 33 00 FF D2          ; dessin banque 2 frame 15 dy 51 dx -46
    FF 00                      ; fin d'étape
  ; étape 8
    B4 00 00 00 07 88          ; $B4 Call -> LAB_05AF
    08 0F AC 00 00 04          ; dessin banque 2 frame 15 dy -84 dx 4
    08 0F BF 00 FF DC          ; dessin banque 2 frame 15 dy -65 dx -36
    08 0F D2 00 FF CA          ; dessin banque 2 frame 15 dy -46 dx -54
    08 0F DA 00 FF E0          ; dessin banque 2 frame 15 dy -38 dx -32
    08 0F CB 00 FF F5          ; dessin banque 2 frame 15 dy -53 dx -11
    08 0F E1 00 00 02          ; dessin banque 2 frame 15 dy -31 dx 2
    08 0F E5 00 00 10          ; dessin banque 2 frame 15 dy -27 dx 16
    08 0F EF 00 00 0E          ; dessin banque 2 frame 15 dy -17 dx 14
    08 0F F4 00 FF FD          ; dessin banque 2 frame 15 dy -12 dx -3
    08 0F F2 00 FF F4          ; dessin banque 2 frame 15 dy -14 dx -12
    08 0F F2 00 FF E9          ; dessin banque 2 frame 15 dy -14 dx -23
    08 0F 15 00 FF E4          ; dessin banque 2 frame 15 dy 21 dx -28
    08 0F 11 00 00 0D          ; dessin banque 2 frame 15 dy 17 dx 13
    08 0F 30 00 00 1B          ; dessin banque 2 frame 15 dy 48 dx 27
    08 0F 33 00 FF CF          ; dessin banque 2 frame 15 dy 51 dx -49
    FF 00                      ; fin d'étape
  ; étape 9
    88 28                      ; $88 Hold
    08 0F AA 00 00 0A          ; dessin banque 2 frame 15 dy -86 dx 10
    08 0F BB 00 FF DE          ; dessin banque 2 frame 15 dy -69 dx -34
    08 0F CE 00 FF CB          ; dessin banque 2 frame 15 dy -50 dx -53
    08 0F D7 00 FF DF          ; dessin banque 2 frame 15 dy -41 dx -33
    08 0F C8 00 FF F5          ; dessin banque 2 frame 15 dy -56 dx -11
    08 0F DD 00 00 02          ; dessin banque 2 frame 15 dy -35 dx 2
    08 0F E2 00 00 10          ; dessin banque 2 frame 15 dy -30 dx 16
    08 0F EB 00 00 0E          ; dessin banque 2 frame 15 dy -21 dx 14
    08 0F F1 00 FF FE          ; dessin banque 2 frame 15 dy -15 dx -2
    08 0F EF 00 FF F4          ; dessin banque 2 frame 15 dy -17 dx -12
    08 0F F0 00 FF E9          ; dessin banque 2 frame 15 dy -16 dx -23
    08 0F 13 00 FF E5          ; dessin banque 2 frame 15 dy 19 dx -27
    08 0F 0F 00 00 0F          ; dessin banque 2 frame 15 dy 15 dx 15
    08 0F 30 00 00 1D          ; dessin banque 2 frame 15 dy 48 dx 29
    08 0F 34 00 FF CD          ; dessin banque 2 frame 15 dy 52 dx -51
    FF 00                      ; fin d'étape
  ; étape 10
    B4 00 00 00 07 7E          ; $B4 Call -> LAB_05AE
    88 05                      ; $88 Hold
    08 0F AA 00 00 0A          ; dessin banque 2 frame 15 dy -86 dx 10
    08 0F BB 00 FF DE          ; dessin banque 2 frame 15 dy -69 dx -34
    08 0F CE 00 FF CB          ; dessin banque 2 frame 15 dy -50 dx -53
    08 0F D7 00 FF DF          ; dessin banque 2 frame 15 dy -41 dx -33
    08 0F C8 00 FF F5          ; dessin banque 2 frame 15 dy -56 dx -11
    08 0F DD 00 00 02          ; dessin banque 2 frame 15 dy -35 dx 2
    08 0F E2 00 00 10          ; dessin banque 2 frame 15 dy -30 dx 16
    08 0F EB 00 00 0E          ; dessin banque 2 frame 15 dy -21 dx 14
    08 0F F1 00 FF FE          ; dessin banque 2 frame 15 dy -15 dx -2
    08 0F EF 00 FF F4          ; dessin banque 2 frame 15 dy -17 dx -12
    08 0F F0 00 FF E9          ; dessin banque 2 frame 15 dy -16 dx -23
    08 0F 13 00 FF E5          ; dessin banque 2 frame 15 dy 19 dx -27
    08 0F 0F 00 00 0F          ; dessin banque 2 frame 15 dy 15 dx 15
    08 0F 30 00 00 1D          ; dessin banque 2 frame 15 dy 48 dx 29
    08 0F 34 00 FF CD          ; dessin banque 2 frame 15 dy 52 dx -51
    FF 00                      ; fin d'étape
  ; étape 11
    88 28                      ; $88 Hold
    08 10 AA 00 FF E0          ; dessin banque 2 frame 16 dy -86 dx -32
    08 11 CA 00 FF CA          ; dessin banque 2 frame 17 dy -54 dx -54
    08 12 BE 00 FF D3          ; dessin banque 2 frame 18 dy -66 dx -45
    08 13 C4 00 FF E0          ; dessin banque 2 frame 19 dy -60 dx -32
    08 14 E2 00 00 0C          ; dessin banque 2 frame 20 dy -30 dx 12
    08 15 F6 00 FF D9          ; dessin banque 2 frame 21 dy -10 dx -39
    08 16 17 00 FF D2          ; dessin banque 2 frame 22 dy 23 dx -46
    08 17 2E 00 FF CC          ; dessin banque 2 frame 23 dy 46 dx -52
    08 18 2E 00 00 1B          ; dessin banque 2 frame 24 dy 46 dx 27
    FF FF                      ; fin du script
```

### `LAB_051F` (26:$35A)

Rôles : entrée 0 de la table LAB_051E+146

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
  LAB_0522:
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

### `LAB_0520` (26:$364)

Rôles : entrée 1 de la table LAB_051E+146

```
  ; étape 1
    00 00 00 03 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [corps,frappe]
    00 03 00 00 00 03          ; dessin banque 0 frame 3 dy 0 dx 3
    00 00 00 03 FF FF          ; dessin banque 0 frame 0 dy 0 dx -1 [corps,frappe]
  LAB_0521:
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0521` (26:$376)

Rôles : entrée 2 de la table LAB_051E+146

```
  ; étape 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    00 00 00 02 00 00          ; dessin banque 0 frame 0 dy 0 dx 0 [frappe]
    00 02 00 00 00 01          ; dessin banque 0 frame 2 dy 0 dx 1
    FF FF                      ; fin du script
```

### `LAB_0522` (26:$390)

Rôles : entrée 3 de la table LAB_051E+146

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

### `LAB_0523` (26:$3B2)

Rôles : entrée 4 de la table LAB_051E+146

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

### `LAB_0524` (26:$3D4)

Rôles : entrée 5 de la table LAB_051E+146

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

### `LAB_0525` (26:$3F6)

Rôles : entrée 6 de la table LAB_051E+146

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

