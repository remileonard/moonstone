#!/usr/bin/env python3
"""
check_asm.py — contrôles de cohérence code/données d'un source IRA.

Pour chaque label, compare ce qu'il y a à sa définition (instruction ou
DC/DS) avec la façon dont il est utilisé.  Chaque compteur doit valoir 0
pour un désassemblage sain :

  ORI.B #0,Dn              deux mots nuls décodés comme instruction :
                           presque toujours des données.
  donnée dans le code      label défini sur une instruction mais lu/écrit
                           par MOVE, TST, CMP, ADD... (hors #LAB, LEA, PEA,
                           sauts) : variable décodée comme du code.
  milieu d'instruction     référence LAB+n qui tombe sur une instruction :
                           donnée, ou décodage désaligné.
  saut vers des données    Bcc/BRA/BSR/JSR/JMP vers un label défini sur des
                           DC : code non désassemblé.

Usage : check_asm.py FICHIER.asm [-v]
"""
import re
import sys

FLOW = {'JSR', 'JMP', 'BSR', 'BRA', 'BEQ', 'BNE', 'BGT', 'BGE', 'BLT', 'BLE',
        'BHI', 'BLS', 'BCC', 'BCS', 'BPL', 'BMI', 'BVC', 'BVS', 'BHS', 'BLO'}


def is_flow(op):
    return op in FLOW or op.startswith('DB')


def check(path, verbose=False):
    lines = open(path, encoding='latin-1').read().split('\n')
    sec = None
    lab = {}          # nom -> (type de section, 'data'|'insn', ligne, texte)
    pending = []
    items = []
    for i, l in enumerate(lines, 1):
        m = re.match(r'\s*SECTION\s+\S+?,(\w+)', l)
        if m:
            sec = m.group(1)
            continue
        m = re.match(r'(\w+):', l)
        if m:
            pending.append(m.group(1))
            continue
        t = l.split(';')[0].strip()
        if not t or sec is None:
            continue
        op = t.split()[0].upper()
        kind = 'data' if op.startswith(('DC.', 'DS.')) else 'insn'
        for n in pending:
            lab[n] = (sec, kind, i, t)
        pending = []
        items.append((i, kind, op, t))

    res = {'ORI.B #0,Dn': [], 'donnée dans le code': [],
           'milieu d\'instruction': [], 'saut vers des données': []}
    for i, kind, op, t in items:
        base = op.split('.')[0]
        if kind == 'insn' and re.match(r'ORI\.B\s+#\$0+,D\d', t, re.I):
            res['ORI.B #0,Dn'].append((i, t))
        for m in re.finditer(r'(#?)\b((?:LAB|SECSTRT|L\d\d)_\w+)(\+\d+)?', t):
            imm, n, off = m.groups()
            if n not in lab:
                continue
            s, k, ln, dt = lab[n]
            if is_flow(base) and k == 'data' and kind == 'insn':
                res['saut vers des données'].append((i, t))
            if (kind == 'insn' and k == 'insn' and not imm and not is_flow(base)
                    and base not in ('LEA', 'PEA')):
                res['donnée dans le code'].append((i, t))
            if off and k == 'insn':
                res['milieu d\'instruction'].append((i, t))
    return res


def main():
    path = sys.argv[1]
    verbose = '-v' in sys.argv
    res = check(path, verbose)
    print('%s : %s' % (path, ', '.join('%s=%d' % (k, len(v)) for k, v in res.items())))
    if verbose:
        for k, v in res.items():
            for i, t in v[:50]:
                print('  [%s] l.%d : %s' % (k, i, t))


if __name__ == '__main__':
    main()
