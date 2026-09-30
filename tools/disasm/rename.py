#!/usr/bin/env python3
"""
rename.py — renomme un label dans amiga_asm/<nom>.asm et l'enregistre dans
amiga_asm/<nom>.sym (qui survit aux régénérations).

  rename.py <binaire> <ancien> <nouveau> ["commentaire"]
  rename.py mog LAB_0036 CombatArena "Combat en arène : boucle LAB_0037"
  rename.py mog CombatArena CombatArena "nouveau commentaire"   # commentaire seul

Le renommage remplace l'identifiant partout dans le source (mot entier),
ajoute « ; [ex ANCIEN] commentaire » avant la définition, puis vérifie que
le source se réassemble toujours à l'identique du binaire.

Convention de nommage proposée : CamelCase anglais, préfixe de module pour
les routines (Combat_, Ow_ pour l'overworld, Ix_ pour IMAGEXCEL, Snd_, Io_…),
préfixe v_ pour les variables, t_ pour les tables.
"""
import os
import re
import shutil
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, HERE)
import hunk as hunkmod  # noqa: E402
import m68kdis  # noqa: E402
import symbols  # noqa: E402

ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
ASM_DIR = os.path.join(ROOT, 'amiga_asm')
VASM = os.path.join(HERE, '.build', 'vasm', 'vasmm68k_mot')


def reassembles(binary, asm):
    with tempfile.TemporaryDirectory() as tmp:
        exe = os.path.join(tmp, 'x')
        r = subprocess.run([VASM, '-quiet', '-Fhunkexe', '-nosym', '-no-opt', '-m68000',
                            '-o', exe, asm], capture_output=True, text=True)
        if r.returncode != 0:
            print(r.stdout + r.stderr)
            return False
        return not hunkmod.diff_offsets(binary, exe)


def split_comment(line):
    """Sépare code et commentaire (';' hors chaîne entre guillemets)."""
    q = None
    for i, c in enumerate(line):
        if q:
            if c == q:
                q = None
        elif c in '"\'':
            q = c
        elif c == ';':
            return line[:i], line[i:]
    return line, ''


def in_code(text, name):
    pat = re.compile(r'\b%s\b' % re.escape(name))
    return any(pat.search(split_comment(l)[0]) for l in text.split('\n'))


def replace_in_code(text, old, new):
    pat = re.compile(r'\b%s\b' % re.escape(old))
    out = []
    for l in text.split('\n'):
        code, com = split_comment(l)
        out.append(pat.sub(new, code) + com)
    return '\n'.join(out)


def main():
    if len(sys.argv) < 4:
        sys.exit(__doc__)
    name, old, new = sys.argv[1:4]
    comment = sys.argv[4] if len(sys.argv) > 4 else None
    binary = os.path.join(ASM_DIR, name)
    asm = os.path.join(ASM_DIR, name + '.asm')
    sym_path = symbols.path_for(ASM_DIR, name)
    if not symbols.NAME_RE.match(new):
        sys.exit('nom invalide : %s' % new)

    text = open(asm, encoding='latin-1').read()
    if old != new and in_code(text, new):
        sys.exit('%s existe déjà dans %s.asm' % (new, name))

    labels, _, _ = m68kdis.import_labels(VASM, asm)
    addr = next((k for k, n in labels.items() if n == old), None)
    if addr is None:
        sys.exit('label %s introuvable dans %s.asm (label défini par « %s: » attendu)'
                 % (old, name, old))

    syms = symbols.load(sym_path)
    prev = syms.get(addr)
    entry = {
        'name': new,
        # on garde le tout premier nom IRA comme « ancien nom »
        'old': prev['old'] if prev else old,
        'comment': comment if comment is not None else (prev['comment'] if prev else ''),
    }
    syms[addr] = entry

    # retire l'ancien commentaire éventuel, renomme, puis ré-annote
    if prev:
        for c in symbols.comment_lines(prev):
            text = text.replace(c + '\n' + old + ':', old + ':')
    text = replace_in_code(text, old, new)
    text = symbols.annotate(text, {addr: entry})

    backup = asm + '.bak'
    shutil.copy(asm, backup)
    open(asm, 'w', encoding='latin-1').write(text)
    if not reassembles(binary, asm):
        shutil.move(backup, asm)
        sys.exit('ÉCHEC : le source ne se réassemble plus à l\'identique ; annulé.')
    os.remove(backup)
    symbols.save(sym_path, syms, name)
    print('%s : %s -> %s  (%d:$%X)' % (name, old, new, addr[0], addr[1]))


if __name__ == '__main__':
    main()
