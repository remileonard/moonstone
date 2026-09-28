# tools/disasm — désassemblage vérifié de `program`, `mog`, `nb`

Ces outils régénèrent `amiga_asm/*.asm` à partir des exécutables Amiga
originaux (`amiga_asm/program`, `mog`, `nb`) avec une séparation
code / données justifiée et **vérifiée par réassemblage**.

## Pourquoi

L'ancien source avait été produit par IRA **sans `-PREPROC`** : dans ce mode
IRA décode *tout* le contenu des hunks CODE comme des instructions. Les
variables, tables, textes et structures placés dans le code devenaient des
instructions absurdes (`ORI.B #$00,D0` = deux mots nuls, références
`LAB_xxxx+2` au milieu d'instructions, etc.). À l'inverse, `-PREPROC` seul
est trop prudent : il rate tout le code atteint uniquement par pointeur
(handlers d'interruption, callbacks, tables de sauts).

## Méthode

1. **Vérité terrain** : les binaires. Chaque source doit se réassembler
   (vasm) en un exécutable *identique* hunk par hunk, contenu **et**
   relocations (`hunk.py`). Ce test ne suffit pas à valider la séparation
   code/données (`ORI.B #0,D0` se réassemble en `$0000 $0000`), mais il
   garantit qu'aucun octet n'est perdu ou modifié.
2. **Analyse** (`m68kdis.py`, décodeur Capstone) par descente récursive
   depuis l'entrée, guidée par les relocations :
   - **CERTAIN** : atteint par le flot depuis l'entrée (branches, BSR/JSR/JMP,
     sauts calculés `JMP d8(PC,Dn)` dans du code déroulé) ;
   - **PROBABLE** : cible d'un pointeur (`#LAB`, `LEA`, `PEA`, `DC.L` relogé,
     vecteurs `AUTO_INTn`, `TRAP`...) validée par un décodage strict ;
   - **HEURISTIQUE** : bloc jamais référencé mais décodable proprement
     (code mort ou appelé par adresse calculée) — balisé *A VERIFIER*.

   Le décodage strict rejette : opcode invalide, mot nul, relocation ne
   correspondant à aucun opérande, chevauchement d'instructions, zone lue ou
   écrite comme donnée par du code déjà prouvé, texte ASCII, sortie du hunk,
   bloc de moins de 3 instructions, trou commençant par une donnée
   référencée par adresse.
3. **IRA piloté** (`ira_hints.py`) : l'analyse est traduite en fichier de
   configuration IRA (`amiga_asm/<nom>.cnf` : directives `CODE` + `BANNER`)
   puis `IRA -PREPROC -CONFIG` suit le flot depuis ces racines.
4. **Contrôle croisé par réassemblage** : si un bloc HEURISTIQUE produit un
   encodage que vasm ne reproduit pas, ce n'est pas du code sorti d'un
   assembleur — il est retiré et IRA relancé.
5. **Noms conservés** : chaque label garde le nom qu'il avait dans l'ancien
   source à la même adresse (les `LAB_xxxx` cités dans `docs/` et dans le
   code C restent valables). Les labels nouveaux sont nommés
   `L<hunk>_<offset>` (ex. `L13_001CE`), les absolus nouveaux `ABS_<valeur>`.
6. **Contrôles** (`check_asm.py`) : compteurs qui doivent être nuls —
   `ORI.B #0,Dn`, label d'instruction accédé comme donnée, référence au
   milieu d'une instruction, saut vers des données.

Chaque exécution écrit un rapport `amiga_asm/reports/<nom>.md` : octets par
niveau de confiance et par hunk, reclassements par rapport à l'ancien source,
pointeurs rejetés (= données dans le code), sauts calculés, blocs retirés,
labels disparus.

## Nommer au fil des découvertes

Les noms donnés aux routines, variables et tables sont enregistrés dans
`amiga_asm/<nom>.sym` (adresse `hunk:$offset`, nom, ancien nom IRA,
commentaire). Ce fichier fait référence : `regen.sh` le réapplique, et
chaque définition renommée est précédée de `; [ex LAB_xxxx] commentaire`,
ce qui permet de retrouver les noms cités dans `docs/`.

```sh
python3 tools/disasm/rename.py mog LAB_0036 Combat_Arena "Boucle de combat en arene"
python3 tools/disasm/rename.py mog Combat_Arena Combat_Arena "commentaire corrige"
```

`rename.py` remplace le nom partout, insère le commentaire, vérifie que le
source se réassemble toujours à l'identique (sinon il annule) et met à jour
le `.sym`.

Convention : CamelCase anglais avec un préfixe de module — `Combat_`, `Ow_`
(overworld), `Ix_` (IMAGEXCEL), `Snd_`, `Io_`, `Unpack_`, `Hw_`, `Prot_` —
et `v_` pour les variables, `t_` pour les tables. Ne renommer que ce qui est
vérifié ; en cas de doute, un commentaire seul (même nom) suffit.

## Utilisation

```sh
tools/disasm/setup.sh        # IRA 2.11 (aminet) + patch, vasm, capstone, lhafile
tools/disasm/regen.sh        # régénère program, mog, nb (+ .cnf + rapports, .sym réappliqué)
python3 tools/disasm/check_asm.py amiga_asm/mog.asm -v
```

`setup.sh` a besoin d'un accès réseau à `aminet.net`, `github.com` et PyPI.
Les outils compilés vont dans `tools/disasm/.build/` (ignoré par git).
La régénération est idempotente.

Patch IRA (`ira-2.11-keep-cnf.patch`) : avec `-PREPROC -CONFIG`, IRA 2.11
refuse de s'exécuter si le `.cnf` existe déjà (il veut en écrire un
nouveau) ; le patch conserve le `.cnf` fourni. À noter aussi : sans `.cnf`,
`-PREPROC -CONFIG` ne déclare pas le point d'entrée (d'où `CODE $0`), et la
directive `SYMBOL` est ignorée pour certains labels (d'où le renommage
textuel).

## Résultats

| | ancien (IRA sans PREPROC) | nouveau |
|---|---|---|
| `ORI.B #0,Dn` (program / mog / nb) | 299 / 542 / 17 | 0 / 0 / 0 |
| donnée décodée en code | 299 / 341 / 149 | 0 / 1\* / 0 |
| référence au milieu d'une instruction | 217 / 323 / 127 | 0 / 7\* / 1\*\* |
| saut vers des données | 0 / 17 / 0 | 0 / 4\* / 0 |

\* tous dans le hunk `S_9` de `mog` : protection **Rob Northen Copylock**
(`ORI.W #$A71F,SR` active le mode trace ; la suite est du code chiffré,
déchiffré instruction par instruction par le handler TRACE — impossible à
désassembler statiquement ; balisé `[TRACE]` dans le source).
\*\* `LEA SECSTRT_0+250000,A0` : calcul d'adresse d'un tampon, légitime.

Les 162 scripts IMAGEXCEL extraits par `tools/extract_scripts.py` sont
identiques octet pour octet avec le nouveau source, et 4 scripts
supplémentaires apparaissent (ex. `mog` `LAB_0028`, auparavant décodé comme
`ORI.B #$14,(A6)`).

## Limites

- Le code HEURISTIQUE (quelques Ko par fichier) n'est prouvé par aucune
  référence statique ; seule une trace d'exécution (WinUAE/FS-UAE) peut
  trancher. Exemple : le décodeur RLE `LAB_0448` (`program` `S_21`) n'est
  appelé que par une routine sans aucune référence dans le binaire.
- Les données restent en `DC.x` bruts : leur typage (tables, structures,
  textes) est le travail suivant.
- Les numéros de ligne cités dans `docs/` (liens `program.asm#Lnnn`)
  renvoient à l'ancien source (commit `ede2abd`) ; les noms de labels, eux,
  restent valables.
