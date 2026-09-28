#!/bin/sh
# regen.sh — régénère amiga_asm/<nom>.asm à partir des binaires originaux.
#
# Pour chaque binaire (program, mog, nb) :
#   - analyse code/données (m68kdis.py) -> amiga_asm/<nom>.cnf (config IRA)
#   - IRA -PREPROC -CONFIG, noms de labels repris du source précédent
#   - vérification : réassemblage vasm identique au binaire (hunk par hunk)
#   - rapport : amiga_asm/reports/<nom>.md
#
# Usage : tools/disasm/regen.sh [nom...]     (après tools/disasm/setup.sh)
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
ROOT=$(cd "$HERE/../.." && pwd)
BUILD="$HERE/.build"
ASM="$ROOT/amiga_asm"
mkdir -p "$ASM/reports"
[ $# -eq 0 ] && set -- program mog nb
for n in "$@"; do
    tmp=$(mktemp)
    python3 "$HERE/ira_hints.py" "$ASM/$n" -o "$tmp" --old "$ASM/$n.asm" \
        --ira "$BUILD/ira/ira" --vasm "$BUILD/vasm/vasmm68k_mot" \
        --cnf "$ASM/$n.cnf" --report "$ASM/reports/$n.md" --heuristic
    mv "$tmp" "$ASM/$n.asm"
    python3 "$HERE/check_asm.py" "$ASM/$n.asm"
done
