# Désassemblage de `program` — rapport

- Réassemblage vasm → binaire original : **identique** (contenu et relocations de chaque hunk)
- Racines de code fournies à IRA : 110 (PROBABLE 49, HEURISTIQUE 59, TABLE 2)

## Classification par hunk CODE

Octets de code trouvés par l'analyse (CERTAIN = atteint depuis l'entrée ; PROBABLE = via pointeur validé ; HEURISTIQUE = non atteint mais décodage valide, à vérifier). Les deux dernières colonnes comparent les débuts d'instruction du nouveau source à ceux de l'ancien.

| Hunk | Taille | CERTAIN | PROBABLE | HEURISTIQUE | Données | instr. ancien → données | données ancien → instr. |
|---|---|---|---|---|---|---|---|
| S_0 | 4688 | 3772 | 290 | 206 | 420 | 36 | 0 |
| S_1,CHIP | 1940 | 192 | 1350 | 34 | 364 | 86 | 0 |
| S_4 | 1156 | 1012 | 2 | 142 | 0 | 0 | 0 |
| S_8 | 2756 | 2386 | 0 | 116 | 254 | 58 | 0 |
| S_10 | 3604 | 2462 | 674 | 370 | 98 | 25 | 1 |
| S_12 | 916 | 476 | 216 | 164 | 60 | 16 | 0 |
| S_13 | 3136 | 2854 | 10 | 224 | 48 | 13 | 0 |
| S_15 | 1676 | 176 | 1226 | 272 | 2 | 0 | 0 |
| S_18 | 2184 | 1014 | 0 | 1078 | 92 | 21 | 0 |
| S_20 | 1540 | 234 | 418 | 706 | 182 | 45 | 0 |
| S_21 | 2440 | 500 | 526 | 1156 | 258 | 64 | 0 |
| S_23 | 1856 | 1532 | 0 | 0 | 324 | 73 | 0 |
| S_25 | 1064 | 722 | 0 | 338 | 4 | 1 | 0 |
| S_29 | 2028 | 1480 | 20 | 528 | 0 | 0 | 0 |
| S_31 | 3036 | 2154 | 722 | 54 | 106 | 19 | 0 |
| **Total** | | 20966 | 5454 | 5388 | 2212 | 457 | 1 |

## Sauts calculés

- `21:$930` : `JMP d8(PC,Dn)` → `LAB_049F` (code déroulé)
- `8:$A6E` : `JMP d8(PC,Dn)` → `LAB_01C5` (code déroulé)

## Pointeurs vers un hunk CODE rejetés comme code (64)

Ces cibles sont référencées par adresse mais leur décodage échoue : ce sont des données placées dans un hunk CODE (variables, tables, textes).

- `LAB_0002` : $176: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0003` : texte ASCII 'The En'
- `LAB_0004` : $18C: opcode invalide $4D6F
- `LAB_0023` : $560: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0024` : $570: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0025` : $5C0: mot nul ($0000) décodé comme ORI.B #0
- `LAB_003A` : $D06: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0042` : $F4E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0043` : $F8E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0052` : texte ASCII 'messag'
- `LAB_0053` : texte ASCII 'bold.f'
- `LAB_005E` : $84 recouvre une donnée prouvée (SUBQ.B en 1:$5E)
- `LAB_005F` : $86 recouvre une donnée prouvée (SUBQ.B en 1:$6E)
- `LAB_0094` : $6B0: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0095` : $658: opcode invalide $02D0
- `L01_006A4` : $6A4: mot nul ($0000) décodé comme ORI.B #0
- `LAB_009C` : $6A6: mot nul ($0000) décodé comme ORI.B #0
- `LAB_009D` : $722: mot nul ($0000) décodé comme ORI.B #0
- `LAB_009E` : $73E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_009F` : $75A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_00A0` : $776: mot nul ($0000) décodé comme ORI.B #0
- `SECSTRT_8` : texte ASCII 'au1.ce'
- `LAB_0163` : texte ASCII 'li1.ce'
- `LAB_0164` : texte ASCII 'da1.ce'
- `LAB_0165` : texte ASCII 'dw1.ce'
- `LAB_0166` : texte ASCII 'ha1.ce'
- `LAB_0167` : texte ASCII 'ov1.ce'
- `LAB_0168` : texte ASCII 'co1.ce'
- `LAB_0169` : texte ASCII 'dg1.ce'
- `LAB_016A` : texte ASCII 'klift1'
- `LAB_016D` : texte ASCII 'bg3.pi'
- `LAB_016E` : texte ASCII 'bg2a.p'
- `LAB_017E` : texte ASCII 'bg1a.p'
- `LAB_0181` : texte ASCII 'bg1b.p'
- `LAB_0184` : texte ASCII 'mindsc'
- `LAB_018D` : texte ASCII 'music.'
- `LAB_019F` : $876: opcode invalide $0E07
- `LAB_01A0` : $876: opcode invalide $0E07
- `LAB_01A8` : $8B2: opcode invalide $000A
- `LAB_01B0` : $900: mot nul ($0000) décodé comme ORI.B #0
- `LAB_01B1` : $900: mot nul ($0000) décodé comme ORI.B #0
- `LAB_01B6` : texte ASCII 'vmusic'
- `LAB_022C` : texte ASCII 'NuNuNu'
- `LAB_026D` : $DCA: mot nul ($0000) décodé comme ORI.B #0
- `LAB_028D` : $12C: mot nul ($0000) décodé comme ORI.B #0
- `LAB_028E` : $130: mot nul ($0000) décodé comme ORI.B #0
- `LAB_029E` : texte ASCII '012345'
- `LAB_02AD` : texte ASCII 'No Mat'
- `LAB_02AE` : texte ASCII 'Checks'
- `L18_00032` : $32: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0442` : $554: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0443` : $558: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0444` : $55C: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0445` : $560: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0446` : $600: mot nul ($0000) décodé comme ORI.B #0
- `LAB_046C` : $1BE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_046D` : $1CE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_046E` : $1DE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_04B3` : $12C: mot nul ($0000) décodé comme ORI.B #0
- `LAB_059D` : $31E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_05A9` : $65E: opcode invalide $000A
- `L31_00674` : l'instruction en $688 recouvre un début d'instruction en $68A
- `LAB_05B1` : $79E: mot nul ($0000) décodé comme ORI.B #0
- `L31_0090C` : $90C: mot nul ($0000) décodé comme ORI.B #0

## Labels

- 1450 labels de l'ancien source conservés à la même adresse.
- 49 labels anciens disparus (ils pointaient dans des données mal décodées ou au milieu d'instructions) :

  `LAB_002B`, `LAB_0055`, `LAB_0057`, `LAB_016B`, `LAB_016C`, `LAB_016F`, `LAB_0170`, `LAB_0171`, `LAB_0172`, `LAB_0175`, `LAB_0176`, `LAB_0177`, `LAB_0178`, `LAB_0179`, `LAB_017A`, `LAB_017B`, `LAB_017C`, `LAB_017D`, `LAB_017F`, `LAB_0180`, `LAB_0182`, `LAB_0183`, `LAB_0187`, `LAB_0188`, `LAB_0189`, `LAB_018A`, `LAB_018B`, `LAB_018F`, `LAB_01A9`, `LAB_01BB`, `LAB_0268`, `LAB_029F`, `LAB_02B6`, `LAB_02B7`, `LAB_02B8`, `LAB_02BA`, `LAB_037D`, `LAB_037E`, `LAB_037F`, `LAB_0380`, `LAB_0381`, `LAB_0382`, `LAB_0388`, `LAB_0389`, `LAB_038A`, `LAB_038B`, `LAB_038C`, `LAB_0391`, `LAB_05AA`
