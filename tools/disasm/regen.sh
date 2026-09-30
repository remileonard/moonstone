#!/bin/sh
# regen.sh — régénère amiga_asm/<nom>.asm à partir des binaires originaux.
#
# Pour chaque binaire (program, mog, nb) :
#   - analyse code/données (m68kdis.py) -> amiga_asm/<nom>.cnf (config IRA)
#   - IRA -PREPROC -CONFIG, noms repris de amiga_asm/<nom>.sym puis du
#     source précédent (commentaires « ; [ex LAB_xxxx] » réinsérés)
#   - vérification : réassemblage vasm identique au binaire (hunk par hunk)
#   - rapport : amiga_asm/reports/<nom>.md
#
# Usage : tools/disasm/regen.sh [nom...]     (après tools/disasm/setup.sh)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
BUILD="$HERE/.build"
BASELINE=ede2abd   # commit du désassemblage IRA d'origine
ASM="$ROOT/amiga_asm"
mkdir -p "$ASM/reports"
[ $# -eq 0 ] && set -- program mog nb
for n in "$@"; do
    tmp=$(mktemp)
    # référence des rapports : le désassemblage IRA d'origine (sans -PREPROC)
    base=$(mktemp)
    git -C "$ROOT" show "$BASELINE:amiga_asm/$n.asm" > "$base"
    python3 "$HERE/ira_hints.py" "$ASM/$n" -o "$tmp" --old "$ASM/$n.asm" --sym "$ASM/$n.sym" \
        --baseline "$base" \
        --ira "$BUILD/ira/ira" --vasm "$BUILD/vasm/vasmm68k_mot" \
        --cnf "$ASM/$n.cnf" --report "$ASM/reports/$n.md" --heuristic
    mv "$tmp" "$ASM/$n.asm"
    rm -f "$base"
    python3 "$HERE/check_asm.py" "$ASM/$n.asm"
done
