"""
symbols.py — fichier de symboles d'un binaire (amiga_asm/<nom>.sym).

C'est la référence des noms donnés au fil des découvertes. Il survit aux
régénérations : ira_hints.py le réapplique (noms + commentaires).

Format : une entrée par ligne, champs séparés par des tabulations
    <hunk>:$<offset>\t<nom>\t<ancien nom>\t<commentaire>
Les lignes vides et celles qui commencent par '#' sont ignorées.
"""
import os
import re

HEADER = """\
# Symboles de {name} — noms donnés au fil de l'analyse.
# Format : <hunk>:$<offset> <TAB> <nom> <TAB> <ancien nom IRA> <TAB> <commentaire>
# Géré par tools/disasm/rename.py ; réappliqué par tools/disasm/regen.sh.
"""

NAME_RE = re.compile(r'^[A-Za-z_][A-Za-z0-9_]*$')


def path_for(asm_dir, name):
    return os.path.join(asm_dir, name + '.sym')


def load(path):
    """Renvoie {(hunk, offset): {'name', 'old', 'comment'}}."""
    syms = {}
    if not os.path.exists(path):
        return syms
    for line in open(path, encoding='utf-8'):
        line = line.rstrip('\n')
        if not line.strip() or line.startswith('#'):
            continue
        parts = line.split('\t')
        m = re.match(r'(\d+):\$([0-9A-Fa-f]+)$', parts[0])
        if not m or len(parts) < 2:
            raise ValueError('%s : ligne invalide : %r' % (path, line))
        syms[(int(m.group(1)), int(m.group(2), 16))] = {
            'name': parts[1],
            'old': parts[2] if len(parts) > 2 else '',
            'comment': parts[3] if len(parts) > 3 else '',
        }
    return syms


def save(path, syms, binary_name):
    lines = [HEADER.format(name=binary_name)]
    for (h, off), s in sorted(syms.items()):
        lines.append('%d:$%05X\t%s\t%s\t%s' % (h, off, s['name'], s['old'], s['comment']))
    open(path, 'w', encoding='utf-8').write('\n'.join(lines) + '\n')


def comment_lines(sym):
    """Lignes de commentaire insérées avant la définition du label."""
    head = '; [ex %s]' % sym['old'] if sym['old'] else ';'
    if sym['comment']:
        head += ' ' + sym['comment']
    return [head]


def annotate(text, syms):
    """Insère les commentaires des symboles avant leur définition
    (idempotent : un commentaire déjà présent n'est pas dupliqué)."""
    by_name = {s['name']: s for s in syms.values()}
    out = []
    lines = text.split('\n')
    for i, line in enumerate(lines):
        m = re.match(r'^([A-Za-z_]\w*):', line)
        if m and m.group(1) in by_name:
            for c in comment_lines(by_name[m.group(1)]):
                if not (out and out[-1] == c):
                    out.append(c)
        out.append(line)
    return '\n'.join(out)
