#!/usr/bin/env python3
"""
asm_twins.py — routines jumelles de program et de mog (même code, autres
adresses) : les blocs (d'un label au suivant) dont les instructions sont
identiques une fois les labels remplacés par un joker sont appariés, et
les labels cités aux mêmes places sont mis en correspondance.

  python3 tools/asm_twins.py          liste « label_program label_mog »
"""
import collections
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
LABRE = re.compile(r'\b((?:LAB|L\d\d|SECSTRT|EXT)_[0-9A-Fa-f]+|[A-Z][a-z]\w*_\w+)\b')


def blocks(path):
    L = open(path).read().split('\n')
    out = collections.OrderedDict()
    cur = None
    for line in L:
        m = re.match(r'^([A-Za-z_]\w*):', line)
        if m:
            cur = m.group(1)
            out[cur] = []
            continue
        s = line.strip()
        if cur is None or not s or s.startswith(';') or s.startswith('SECTION'):
            continue
        if re.match(r'^[A-Z_0-9]+\s+EQU', s):
            continue
        out[cur].append(s)
    return out


def norm(ins):
    labs = []

    def rep(m):
        labs.append(m.group(1))
        return '@'
    return LABRE.sub(rep, ins), labs


def twin_map():
    """{label de program: label de mog} (correspondances sans conflit)."""
    asm = os.path.join(ROOT, 'amiga_asm')
    mog = blocks(os.path.join(asm, 'mog.asm'))
    prog = blocks(os.path.join(asm, 'program.asm'))

    def key(b):
        n = [norm(i) for i in b]
        return tuple(x[0] for x in n), [x[1] for x in n]
    mogk = collections.defaultdict(list)
    for k, b in mog.items():
        if len(b) >= 3 and not all(i.startswith(('DC.', 'DS.')) for i in b):
            kk, labs = key(b)
            mogk[kk].append((k, labs))
    mp = {}
    for k, b in prog.items():
        if len(b) < 3:
            continue
        kk, labs = key(b)
        c = mogk.get(kk)
        if not c or len(c) != 1:
            continue
        mk, mlabs = c[0]
        pairs = [(k, mk)] + [(x, y) for pl, ml in zip(labs, mlabs) for x, y in zip(pl, ml)]
        for pa, ma in pairs:
            if pa in mp and mp[pa] != ma:
                mp[pa] = None
            elif pa not in mp:
                mp[pa] = ma
    return {k: v for k, v in mp.items() if v}


if __name__ == '__main__':
    for k, v in twin_map().items():
        print(k, v)
