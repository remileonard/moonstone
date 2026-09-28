#!/bin/sh
# setup.sh — installe les outils de désassemblage dans tools/disasm/.build/
#
#   - IRA 2.11 (aminet.net/dev/asm/ira.lha) + patch ira-2.11-keep-cnf.patch
#   - vasm (miroir GitHub StarWolf3000/vasm-mirror), CPU m68k, syntaxe mot
#   - modules Python : capstone (décodage 68000), lhafile (archive .lha)
#
# Usage : tools/disasm/setup.sh
set -e
HERE=$(cd "$(dirname "$0")" && pwd)
BUILD="$HERE/.build"
mkdir -p "$BUILD"
cd "$BUILD"

python3 -m pip install -q capstone lhafile

if [ ! -x "$BUILD/ira/ira" ]; then
    curl -sSfL -o ira.lha https://aminet.net/dev/asm/ira.lha
    python3 - <<'EOF'
import lhafile, os
f = lhafile.LhaFile('ira.lha')
for n in f.namelist():
    p = n.replace('\\', '/')
    if p.endswith('/'):
        os.makedirs(p, exist_ok=True)
        continue
    os.makedirs(os.path.dirname(p), exist_ok=True)
    open(p, 'wb').write(f.read(n))
EOF
    patch -p0 < "$HERE/ira-2.11-keep-cnf.patch"
    (cd ira && make >/dev/null 2>&1)
fi

if [ ! -x "$BUILD/vasm/vasmm68k_mot" ]; then
    rm -rf vasm
    git clone -q --depth 1 https://github.com/StarWolf3000/vasm-mirror.git vasm
    (cd vasm && make CPU=m68k SYNTAX=mot >/dev/null 2>&1)
fi

echo "IRA  : $BUILD/ira/ira"
echo "vasm : $BUILD/vasm/vasmm68k_mot"
