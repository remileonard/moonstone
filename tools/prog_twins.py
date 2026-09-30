#!/usr/bin/env python3
"""
prog_twins.py — en-tête qui fait compiler un fichier C de mog pour program :
chaque MOG_<label> utilisé est redéfini en PROGRAM_<label jumeau>
(tools/asm_twins.py), les fonctions publiques renommées mog_ -> prog_.

  python3 tools/prog_twins.py game/src/mog_blit.c > game/src/prog_twin_blit.h

Un symbole sans jumeau est une erreur (à ajouter dans EXTRA après lecture
des deux codes).
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
from asm_twins import twin_map  # noqa: E402

# Jumeaux vérifiés à la main (blocs presque identiques)
EXTRA = {
    'LAB_0E91': 'LAB_05D0', 'LAB_0E92': 'LAB_05D1', 'LAB_0E94': 'LAB_05D3',
}


def main():
    src = sys.argv[1]
    code = open(src).read()
    # parties propres à mog (#ifndef PROG_TWIN ... #endif) : non traduites
    code = re.sub(r'#ifndef PROG_TWIN.*?#endif', '', code, flags=re.S)
    known = set(re.findall(r'#define MOG_(\w+) ', open(os.path.join(
        HERE, '..', 'game', 'data', 'ix_mog_syms.h')).read()))
    syms = sorted(set(re.findall(r'\bMOG_(\w+)', code)) & known)   # (MOG_COPPER_BPL... : tels quels)
    inv = {}
    for p, m in twin_map().items():
        inv.setdefault(m, set()).add(p)
    missing = []
    lines = ['/* Généré par tools/prog_twins.py %s : ne pas modifier. */' % os.path.basename(src),
             '#include "ix_mog_syms.h"', '#include "ix_program_syms.h"']
    for s in syms:
        p = EXTRA.get(s) or (next(iter(inv[s])) if len(inv.get(s, ())) == 1 else None)
        if not p:
            missing.append(s)
            continue
        lines.append('#undef MOG_%s' % s)
        lines.append('#define MOG_%s PROGRAM_%s' % (s, p))
    funcs = sorted(set(re.findall(r'^(?:[a-z_]+\s+\**)+(mog_\w+)\(', code, re.M)))
    for f in funcs:
        lines.append('#define %s prog_%s' % (f, f[4:]))
    if missing:
        sys.exit('sans jumeau : %s' % ' '.join(missing))
    print('\n'.join(lines))


if __name__ == '__main__':
    main()
