#!/usr/bin/env python3
"""
ix_names.py — noms des labels de l'assembleur utilisés par le C.

Les tables game/data/mog_names.txt et game/data/program_names.txt donnent,
une ligne par label :

    LAB_0CC9  v_CelLastName  nom de la dernière CEL chargée (LAB_0CBB)

(label de l'assembleur, nom, description libre). Conventions, reprises des
labels déjà nommés dans amiga_asm/*.asm :
    Module_Verbe   routine            (Col_LoadHitData, Combat_Run)
    v_Nom          variable           (v_VblCounter)
    t_Nom          table              (t_HitDataByCel)
    s_Nom          chaîne             (s_CollideHit)
    b_Nom          tampon
    x_Nom          script d'animation IMAGEXCEL

  python3 tools/ix_names.py gen          en-têtes game/data/ix_<prog>_names.h
  python3 tools/ix_names.py apply [f.c]  remplace MOG_LAB_xxxx par MOG_<nom>
                                         dans les sources (toutes par défaut)
  python3 tools/ix_names.py check        labels bruts restant dans le C
  python3 tools/ix_names.py rename ancien nouveau [ancien nouveau...]
                                         renomme dans la table et les
                                         sources à la fois (échanges permis)

L'assembleur n'est pas modifié : les commentaires du C (« LAB_0CBB : ... »)
et les bancs de comparaison continuent de s'y référer par ses labels.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
DATA = os.path.join(ROOT, 'game', 'data')
PROGS = {'mog': 'MOG', 'program': 'PROGRAM'}
RAW = r'(?:LAB|L[0-9]+|SECSTRT|EXT)_[0-9A-Fa-f_]+'
NAME = re.compile(r'^(?:[A-Z][A-Za-z0-9]*_[A-Za-z0-9_]+|[vtsbx]_[A-Za-z0-9_]+)$')


def syms(prog):
    """Labels de ix_<prog>_syms.h : nom -> adresse"""
    path = os.path.join(DATA, 'ix_%s_syms.h' % prog)
    out = {}
    for m in re.finditer(r'#define %s_(\w+) (0x[0-9A-F]+)u' % PROGS[prog], open(path).read()):
        out[m.group(1)] = int(m.group(2), 16)
    return out


def table(prog):
    """Table de noms : [(label, nom, description)] dans l'ordre du fichier"""
    path = os.path.join(DATA, '%s_names.txt' % prog)
    rows = []
    if not os.path.exists(path):
        return rows
    for n, line in enumerate(open(path), 1):
        line = line.rstrip()
        if not line.strip() or line.lstrip().startswith('#'):
            continue
        parts = line.split(None, 2)
        if len(parts) < 2:
            sys.exit('%s:%d : ligne incomplète' % (path, n))
        rows.append((parts[0], parts[1], parts[2] if len(parts) > 2 else '', n))
    return rows


def checked(prog):
    """Table vérifiée : label -> (nom, description)"""
    known = syms(prog)
    path = '%s_names.txt' % prog
    out, names = {}, {}
    for lab, name, desc, n in table(prog):
        if lab not in known:
            sys.exit('%s:%d : %s absent de ix_%s_syms.h' % (path, n, lab, prog))
        if not re.fullmatch(RAW, lab):
            sys.exit('%s:%d : %s est déjà nommé dans l\'assembleur' % (path, n, lab))
        if not NAME.match(name):
            sys.exit('%s:%d : nom %s hors conventions' % (path, n, name))
        if name in known:
            sys.exit('%s:%d : %s existe déjà dans l\'assembleur' % (path, n, name))
        if lab in out:
            sys.exit('%s:%d : %s nommé deux fois' % (path, n, lab))
        if name in names:
            sys.exit('%s:%d : %s déjà donné à %s' % (path, n, name, names[name]))
        out[lab] = (name, desc)
        names[name] = lab
    return out


def gen():
    for prog, pre in PROGS.items():
        rows = checked(prog)
        addr = syms(prog)
        H = ['/*',
             ' * ix_%s_names.h — noms des labels de amiga_asm/%s.asm utilisés par le C' % (prog, prog),
             ' * (générés par tools/ix_names.py depuis game/data/%s_names.txt ;' % prog,
             ' * ne pas modifier).',
             ' */',
             '#ifndef IX_%s_NAMES_H' % pre,
             '#define IX_%s_NAMES_H' % pre,
             '',
             '#include "ix_%s_syms.h"' % prog,
             '']
        for lab in sorted(rows, key=lambda l: addr[l]):
            name, desc = rows[lab]
            line = '#define %s_%s %s_%s' % (pre, name, pre, lab)
            if desc:
                line = '%-60s /* %s */' % (line, desc.replace('*/', '* /'))
            H.append(line)
        H += ['', '#endif', '']
        open(os.path.join(DATA, 'ix_%s_names.h' % prog), 'w').write('\n'.join(H))
        print('ix_%s_names.h : %d noms' % (prog, len(rows)))


def sources(args):
    if args:
        return args
    out = []
    for d in ('game/src', 'game/include'):
        for f in sorted(os.listdir(os.path.join(ROOT, d))):
            if f.endswith(('.c', '.h')) and not f.startswith('prog_twin_'):
                out.append(os.path.join(ROOT, d, f))
    return out


def apply(args):
    maps = {pre: checked(prog) for prog, pre in PROGS.items()}
    for path in sources(args):
        code = open(path).read()
        new = code
        for pre, rows in maps.items():
            new = re.sub(r'\b%s_(%s)\b' % (pre, RAW),
                         lambda m: '%s_%s' % (pre, rows[m.group(1)][0]) if m.group(1) in rows else m.group(0),
                         new)
        if new != code:
            open(path, 'w').write(new)
            print('%s : %d remplacements' % (os.path.relpath(path, ROOT),
                                             sum(a != b for a, b in zip(code.split(), new.split()))))


def check():
    total = 0
    for path in sources([]):
        code = open(path).read()
        found = sorted(set(re.findall(r'\b(?:MOG|PROGRAM)_%s\b' % RAW, code)))
        if found:
            total += len(found)
            print('%-32s %4d' % (os.path.relpath(path, ROOT), len(found)))
    print('labels bruts distincts par fichier : %d' % total)


def rename(args):
    if len(args) % 2:
        sys.exit('rename : paires ancien nouveau')
    pairs = dict(zip(args[0::2], args[1::2]))
    for prog, pre in PROGS.items():
        path = os.path.join(DATA, '%s_names.txt' % prog)
        if not os.path.exists(path):
            continue
        lines = open(path).read().split('\n')
        for i, line in enumerate(lines):
            parts = line.split(None, 2)
            if len(parts) >= 2 and not line.lstrip().startswith('#') and parts[1] in pairs:
                lines[i] = line.replace(parts[1], pairs[parts[1]], 1)
        open(path, 'w').write('\n'.join(lines))
        pat = re.compile(r'\b%s_(%s)\b' % (pre, '|'.join(map(re.escape, pairs))))
        for src in sources([]):
            code = open(src).read()
            new = pat.sub(lambda m: '%s_%s' % (pre, pairs[m.group(1)]), code)
            if new != code:
                open(src, 'w').write(new)
    checked('mog'), checked('program')


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else ''
    if cmd == 'gen':
        gen()
    elif cmd == 'apply':
        apply(sys.argv[2:])
    elif cmd == 'check':
        check()
    elif cmd == 'rename':
        rename(sys.argv[2:])
    else:
        sys.exit(__doc__)


if __name__ == '__main__':
    main()
