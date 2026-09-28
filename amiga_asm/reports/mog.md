# Désassemblage de `mog` — rapport

- Réassemblage vasm → binaire original : **identique** (contenu et relocations de chaque hunk)
- Racines de code fournies à IRA : 219 (PROBABLE 160, HEURISTIQUE 58, TABLE 1)

## Classification par hunk CODE

Octets de code trouvés par l'analyse (CERTAIN = atteint depuis l'entrée ; PROBABLE = via pointeur validé ; HEURISTIQUE = non atteint mais décodage valide, à vérifier). Les deux dernières colonnes comparent les débuts d'instruction du nouveau source à ceux de l'ancien.

| Hunk | Taille | CERTAIN | PROBABLE | HEURISTIQUE | Données | instr. ancien → données | données ancien → instr. |
|---|---|---|---|---|---|---|---|
| S_0 | 52972 | 36576 | 14010 | 1330 | 1056 | 208 | 3 |
| S_5 | 1156 | 1012 | 2 | 142 | 0 | 0 | 0 |
| S_9,CHIP | 2404 | 26 | 76 | 14 | 2288 | 404 | 0 |
| S_12 | 1348 | 1134 | 212 | 0 | 2 | 0 | 0 |
| S_16 | 660 | 456 | 172 | 26 | 6 | 1 | 0 |
| S_18 | 3132 | 2888 | 10 | 188 | 46 | 13 | 0 |
| S_20 | 1676 | 198 | 1226 | 250 | 2 | 0 | 0 |
| S_23 | 2184 | 1070 | 0 | 1022 | 92 | 21 | 0 |
| S_25 | 1540 | 234 | 418 | 706 | 182 | 45 | 0 |
| S_26 | 2440 | 568 | 526 | 1088 | 258 | 64 | 0 |
| S_28 | 1856 | 1532 | 0 | 0 | 324 | 73 | 0 |
| S_30 | 1064 | 618 | 0 | 442 | 4 | 1 | 0 |
| S_34 | 2028 | 1506 | 20 | 502 | 0 | 0 | 0 |
| S_36 | 5684 | 4748 | 618 | 282 | 36 | 8 | 0 |
| S_37 | 508 | 444 | 0 | 18 | 46 | 11 | 0 |
| S_40 | 4828 | 0 | 4734 | 0 | 94 | 17 | 0 |
| S_44 | 9460 | 988 | 1700 | 0 | 6772 | 1418 | 0 |
| **Total** | | 53998 | 23724 | 6010 | 11208 | 2284 | 3 |

## Mode trace / protection anti-copie

- `9:$72` : `ORI.W #$8xxx,SR` active le mode trace. Signature Rob Northen Copylock : la suite du hunk est du code chiffré, non désassemblable statiquement (balisé `[TRACE]` dans le source).

## Sauts calculés

- `26:$930` : `JMP d8(PC,Dn)` → `LAB_0CC5` (code déroulé)

## Code auto-modifiant

- `LAB_0AA7` (`jsr $4ce.l`) est écrit par BTST.L en 16:$AE

## Anomalies sur le chemin CERTAIN

- `9:$1A` : opcode invalide $4E7A

## Pointeurs vers un hunk CODE rejetés comme code (204)

Ces cibles sont référencées par adresse mais leur décodage échoue : ce sont des données placées dans un hunk CODE (variables, tables, textes).

- `LAB_0028` : $600: opcode invalide $000E
- `LAB_0122` : texte ASCII 'Dragon'
- `LAB_013B` : $30F8: opcode invalide $7374
- `LAB_015E` : $3B5A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0185` : $41DC: opcode invalide $00FF
- `LAB_0186` : $4218: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0187` : $421A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0199` : $4762: opcode invalide $FFC4
- `LAB_01D8` : $52D8: mot nul ($0000) décodé comme ORI.B #0
- `LAB_01EB` : $547A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0225` : texte ASCII 'KNIGHT'
- `LAB_0234` : $5C1E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_024E` : $5E82: mot nul ($0000) décodé comme ORI.B #0
- `LAB_024F` : $5E9A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0250` : $5EBE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0271` : texte ASCII 'Rat hi'
- `LAB_0295` : $695E: mot nul ($0000) décodé comme ORI.B #0
- `L00_06960` : $6960: mot nul ($0000) décodé comme ORI.B #0
- `LAB_02DA` : $71C6: mot nul ($0000) décodé comme ORI.B #0
- `LAB_02EC` : $72F8: mot nul ($0000) décodé comme ORI.B #0
- `LAB_02EE` : $7302: opcode invalide $003B
- `LAB_0301` : $74D8: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0302` : $7550: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0314` : texte ASCII 'Cannot'
- `LAB_0327` : texte ASCII 'TASK &'
- `LAB_0340` : texte ASCII '***** '
- `LAB_0365` : texte ASCII 'Skippi'
- `LAB_03B6` : $8670: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0415` : texte ASCII 'Turnin'
- `LAB_042C` : $9154: mot nul ($0000) décodé comme ORI.B #0
- `LAB_042D` : $9158 recouvre une donnée prouvée (MOVE.W en 0:$90D0)
- `L00_0915A` : $915A: ORI #0 (sans effet)
- `L00_0915E` : $915E: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0430` : $9166: opcode invalide $F800
- `LAB_048D` : $9DB8: opcode invalide $0009
- `LAB_04F3` : $AFB0: ORI #0 (sans effet)
- `L00_0AFE4` : $AFE4: ORI #0 (sans effet)
- `LAB_04F5` : $B02C: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0525` : $BD04: mot nul ($0000) décodé comme ORI.B #0
- `LAB_054D` : $C1C4: mot nul ($0000) décodé comme ORI.B #0
- `LAB_056C` : texte ASCII 'Taking'
- `LAB_056D` : texte ASCII 'Increa'
- `0:$7604957A` : $7604957A: hors du hunk
- `0:$B0241D10` : $B0241D10: hors du hunk
- `0:$B0243192` : $B0243192: hors du hunk
- `0:$BB02AB3A` : $BB02AB3A: hors du hunk
- `0:$C08ECB6A` : $C08ECB6A: hors du hunk
- `0:$D811F636` : $D811F636: hors du hunk
- `0:$D81246C0` : $D81246C0: hors du hunk
- `0:$EC091586` : $EC091586: hors du hunk
- `0:$EC095B98` : $EC095B98: hors du hunk
- `LAB_0A4B` : $926: mot nul ($0000) décodé comme ORI.B #0
- `9:$60476254` : $60476254: hors du hunk
- `9:$76047EA8` : $76047EA8: hors du hunk
- `LAB_0ACF` : texte ASCII 'No Mat'
- `LAB_0AD0` : texte ASCII 'Checks'
- `L23_00032` : $32: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C67` : $554: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C68` : $558: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C69` : $55C: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C6A` : $560: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C6B` : $600: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C91` : $1BE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C92` : $1CE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0C93` : $1DE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0CD9` : $12C: mot nul ($0000) décodé comme ORI.B #0
- `34:$60476002` : $60476002: hors du hunk
- `L36_008E8` : $8E8: ORI #0 (sans effet)
- `LAB_0E8C` : $1D4: mot nul ($0000) décodé comme ORI.B #0
- `L40_0040C` : $40C: opcode invalide $000C
- `LAB_0EC0` : $4DE: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0EC1` : $4DE: mot nul ($0000) décodé comme ORI.B #0
- `L40_00662` : $662: ORI #0 (sans effet)
- `LAB_0ECF` : $674: opcode invalide $8889
- `LAB_0EEA` : $A02: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0EFD` : texte ASCII 'DEMON '
- `LAB_0EFE` : texte ASCII 'DEMON '
- `SECSTRT_44` : $0: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F66` : $94: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F67` : $128: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F68` : $1BC: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F8B` : $688: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F8E` : $75A: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0F93` : $7F0: mot nul ($0000) décodé comme ORI.B #0
- `L44_00BEE` : $BEE: mot nul ($0000) décodé comme ORI.B #0
- `L44_00BF0` : $BF0: mot nul ($0000) décodé comme ORI.B #0
- `L44_00C70` : $C70: mot nul ($0000) décodé comme ORI.B #0
- `L44_00CF0` : $CF0: mot nul ($0000) décodé comme ORI.B #0
- `L44_00D70` : $D70: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0FCA` : $DF0: opcode invalide $FFFF
- `LAB_0FCD` : $E3E: opcode invalide $7D4B
- `L44_00EB4` : $ED6: opcode invalide $29D0
- `LAB_0FD3` : $F24: opcode invalide $17CF
- `L44_013B6` : $13B6: mot nul ($0000) décodé comme ORI.B #0
- `LAB_0FE1` : $13F6: opcode invalide $32FF
- `LAB_0FE2` : $140E: opcode invalide $D009
- `L44_01416` : $141C: opcode invalide $D009
- `LAB_0FE4` : $142A: opcode invalide $D009
- `LAB_0FE5` : $1438: opcode invalide $D00A
- `LAB_0FE6` : $1446: opcode invalide $D00C
- `LAB_0FE7` : $1454: opcode invalide $D00C
- `L44_0145C` : $1462: opcode invalide $D00A
- `LAB_0FE9` : $1470: opcode invalide $D00C
- `LAB_0FEA` : $147E: opcode invalide $D00D
- `L44_01486` : $148E: opcode invalide $803F
- `LAB_0FEC` : $149A: opcode invalide $D00F
- `L44_014A2` : $14A8: opcode invalide $D00F
- `LAB_0FF5` : $1514: opcode invalide $8C08
- `LAB_0FF6` : $1524: opcode invalide $8C08
- `LAB_0FF7` : $1534: opcode invalide $8C08
- `LAB_0FF8` : $1544: opcode invalide $8C08
- `LAB_0FF9` : $1592: opcode invalide $00D0
- `L44_0155C` : $1592: opcode invalide $00D0
- `L44_0156A` : $1592: opcode invalide $00D0
- `L44_01578` : $157E: opcode invalide $8C08
- `LAB_0FFE` : $15EA: opcode invalide $27D0
- `L44_015DE` : $15EA: opcode invalide $27D0
- `L44_0165A` : $1752: opcode invalide $8C08
- `LAB_1005` : $1752: opcode invalide $8C08
- `LAB_1007` : $1770: opcode invalide $8C08
- `LAB_1009` : $178E: opcode invalide $8C08
- `LAB_100B` : $17AC: opcode invalide $8C08
- `LAB_100D` : $17CA: opcode invalide $8C08
- `LAB_100E` : $17D8: opcode invalide $8C08
- `LAB_100F` : $17E6: opcode invalide $8C08
- `LAB_1010` : $17F4: opcode invalide $8C08
- `LAB_1011` : $1802: opcode invalide $8C08
- `LAB_1012` : $1810: opcode invalide $8C08
- `L44_01818` : $184A: opcode invalide $D03D
- `L44_01826` : $184A: opcode invalide $D03D
- `L44_01834` : $184A: opcode invalide $D03D
- `L44_01840` : $184A: opcode invalide $D03D
- `LAB_1017` : $1854: opcode invalide $8C0F
- `LAB_1018` : $1862: opcode invalide $8C0A
- `L44_0186A` : $18D2: opcode invalide $8CFF
- `LAB_101A` : $18D2: opcode invalide $8CFF
- `L44_01886` : $18D2: opcode invalide $8CFF
- `LAB_101C` : $18D2: opcode invalide $8CFF
- `L44_018A2` : $18D2: opcode invalide $8CFF
- `LAB_101E` : $18D2: opcode invalide $8CFF
- `LAB_101F` : $18D2: opcode invalide $8CFF
- `L44_018CC` : $18D2: opcode invalide $8CFF
- `L44_0190C` : $1912: opcode invalide $8C08
- `L44_0191A` : $192A: opcode invalide $0880
- `LAB_1028` : $1958: opcode invalide $8C08
- `L44_01960` : $1970: opcode invalide $0880
- `L44_01998` : $19B2: opcode invalide $4D8C
- `LAB_102D` : $19B2: opcode invalide $4D8C
- `LAB_102F` : $19D2: opcode invalide $8C0F
- `LAB_1031` : $1A14: opcode invalide $D00F
- `LAB_1032` : $1A28: opcode invalide $37D0
- `LAB_1033` : $1C60: opcode invalide $D00F
- `LAB_1034` : $1C60: opcode invalide $D00F
- `L44_01AA0` : $1C60: opcode invalide $D00F
- `L44_01AAE` : $1C60: opcode invalide $D00F
- `L44_01ABC` : $1C60: opcode invalide $D00F
- `L44_01ACA` : $1C60: opcode invalide $D00F
- `LAB_1039` : $1C60: opcode invalide $D00F
- `L44_01AE6` : $1C60: opcode invalide $D00F
- `L44_01AF4` : $1C60: opcode invalide $D00F
- `L44_01B06` : $1C60: opcode invalide $D00F
- `LAB_103D` : $1C60: opcode invalide $D00F
- `LAB_103E` : $1C60: opcode invalide $D00F
- `LAB_103F` : $1C60: opcode invalide $D00F
- `L44_01B42` : $1C60: opcode invalide $D00F
- `L44_01B50` : $1C60: opcode invalide $D00F
- `LAB_1042` : $1C60: opcode invalide $D00F
- `LAB_1043` : $1C60: opcode invalide $D00F
- `LAB_1044` : $1C60: opcode invalide $D00F
- `L44_01B88` : $1C60: opcode invalide $D00F
- `L44_01B96` : $1C60: opcode invalide $D00F
- `L44_01BA4` : $1C60: opcode invalide $D00F
- `L44_01BB2` : $1C60: opcode invalide $D00F
- `L44_01BC0` : $1C60: opcode invalide $D00F
- `L44_01BCE` : $1C60: opcode invalide $D00F
- `LAB_104B` : $1C60: opcode invalide $D00F
- `LAB_104C` : $1C60: opcode invalide $D00F
- `LAB_104D` : $1C60: opcode invalide $D00F
- `L44_01C06` : $1C60: opcode invalide $D00F
- `LAB_1050` : $1C60: opcode invalide $D00F
- `LAB_1051` : $1C60: opcode invalide $D00F
- `LAB_1053` : $1C60: opcode invalide $D00F
- `LAB_1054` : $1C60: opcode invalide $D00F
- `L44_01C4C` : $1C60: opcode invalide $D00F
- `L44_01C5A` : $1C60: opcode invalide $D00F
- `L44_01C68` : $1C60: opcode invalide $D00F
- `LAB_105B` : $1D4C: opcode invalide $8009
- `LAB_105C` : $1D44: opcode invalide $00D0
- `LAB_1061` : $1E06: opcode invalide $35D0
- `LAB_1069` : $1E8A: opcode invalide $1A8C
- `L44_01EA6` : $1EB6: opcode invalide $088C
- `L44_01F16` : $1F34: opcode invalide $00D0
- `LAB_106E` : $1F5A: opcode invalide $00D0
- `LAB_1076` : $205A: opcode invalide $37D4
- `LAB_1077` : $205A: opcode invalide $37D4
- `LAB_1079` : $2078: opcode invalide $37D4
- `LAB_107D` : $20B6: opcode invalide $BCFE
- `LAB_108E` : $21B6: opcode invalide $0830
- `LAB_108F` : $21CE: opcode invalide $8C08
- `LAB_1090` : $21CE: opcode invalide $8C08
- `L44_021EA` : $21F8: opcode invalide $3C3E
- `L44_021FE` : $220C: opcode invalide $0E8C
- `L44_02230` : $2236: opcode invalide $8C09
- `LAB_1098` : $2254: mot nul ($0000) décodé comme ORI.B #0

## Labels

- 4065 labels de l'ancien source conservés à la même adresse.
- 4 labels renommés d'après `amiga_asm/mog.sym`.
- 178 labels anciens disparus (ils pointaient dans des données mal décodées ou au milieu d'instructions) :

  `LAB_0272`, `LAB_0275`, `LAB_0317`, `LAB_0345`, `LAB_0417`, `LAB_041B`, `LAB_041D`, `LAB_0426`, `LAB_042F`, `LAB_04F4`, `LAB_056E`, `LAB_056F`, `LAB_0570`, `LAB_0571`, `LAB_0573`, `LAB_0574`, `LAB_0576`, `LAB_0577`, `LAB_0578`, `LAB_0A3C`, `LAB_0A3D`, `LAB_0A3F`, `LAB_0A40`, `LAB_0A41`, `LAB_0A42`, `LAB_0A43`, `LAB_0A44`, `LAB_0A45`, `LAB_0A46`, `LAB_0A47`, `LAB_0A48`, `LAB_0A4A`, `LAB_0AD8`, `LAB_0AD9`, `LAB_0ADA`, `LAB_0ADC`, `LAB_0BA1`, `LAB_0BA2`, `LAB_0BA3`, `LAB_0BA4`, `LAB_0BA5`, `LAB_0BA6`, `LAB_0BAC`, `LAB_0BAD`, `LAB_0BAE`, `LAB_0BAF`, `LAB_0BB0`, `LAB_0BB6`, `LAB_0ECE`, `LAB_0FC5`, `LAB_0FC7`, `LAB_0FC8`, `LAB_0FC9`, `LAB_0FCB`, `LAB_0FCC`, `LAB_0FCE`, `LAB_0FCF`, `LAB_0FD0`, `LAB_0FD1`, `LAB_0FD2`, `LAB_0FE0`, `LAB_0FE3`, `LAB_0FE8`, `LAB_0FEB`, `LAB_0FED`, `LAB_0FEE`, `LAB_0FEF`, `LAB_0FF0`, `LAB_0FF1`, `LAB_0FF2`, `LAB_0FF3`, `LAB_0FF4`, `LAB_0FFA`, `LAB_0FFB`, `LAB_0FFC`, `LAB_0FFD`, `LAB_0FFF`, `LAB_1000`, `LAB_1001`, `LAB_1002`, `LAB_1003`, `LAB_1004`, `LAB_1006`, `LAB_1008`, `LAB_100A`, `LAB_100C`, `LAB_1013`, `LAB_1014`, `LAB_1015`, `LAB_1016`, `LAB_1019`, `LAB_101B`, `LAB_101D`, `LAB_1020`, `LAB_1021`, `LAB_1022`, `LAB_1023`, `LAB_1024`, `LAB_1025`, `LAB_1026`, `LAB_1027`, `LAB_1029`, `LAB_102A`, `LAB_102B`, `LAB_102C`, `LAB_102E`, `LAB_1030`, `LAB_1035`, `LAB_1036`, `LAB_1037`, `LAB_1038`, `LAB_103A`, `LAB_103B`, `LAB_103C`, `LAB_1040`, `LAB_1041`, `LAB_1045`, `LAB_1046`, `LAB_1047`, `LAB_1048`, `LAB_1049`, `LAB_104A`, `LAB_104E`, `LAB_104F`, `LAB_1055`, `LAB_1056`, `LAB_1057`, `LAB_1058`, `LAB_1059`, `LAB_105A`, `LAB_105D`, `LAB_105E`, `LAB_105F`, `LAB_1062`, `LAB_1063`, `LAB_1064`, `LAB_1065`, `LAB_1066`, `LAB_1067`, `LAB_1068`, `LAB_106A`, `LAB_106B`, `LAB_106C`, `LAB_106D`, `LAB_106F`, `LAB_1070`, `LAB_1071`, `LAB_1072`, `LAB_1073`, `LAB_1074`, `LAB_1075`, `LAB_1078`, `LAB_107A`, `LAB_107B`, `LAB_107C`, `LAB_107E`, `LAB_107F`, `LAB_1080`, `LAB_1081`, `LAB_1082`, `LAB_1083`, `LAB_1084`, `LAB_1085`, `LAB_1086`, `LAB_1087`, `LAB_1088`, `LAB_1089`, `LAB_108A`, `LAB_108B`, `LAB_108C`, `LAB_108D`, `LAB_1091`, `LAB_1092`, `LAB_1093`, `LAB_1094`, `LAB_1095`, `LAB_1096`, `LAB_1097`
