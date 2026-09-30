#!/usr/bin/env python3
"""
ira_hints.py — régénère un source 68000 fiable avec IRA, piloté par
l'analyse code/données de m68kdis.py.

  ira_hints.py BINAIRE -o SORTIE.asm --ira IRA --vasm VASM
               [--old ANCIEN.asm] [--cnf SORTIE.cnf] [--report RAPPORT.md]
               [--heuristic]

Étapes
------
1. Analyse (m68kdis.Program) : code CERTAIN depuis l'entrée, cibles de
   pointeurs validées (PROBABLE), sauts calculés, mode trace (Copylock).
2. Écriture d'un .cnf IRA : CODE <adr> pour l'entrée et chaque racine,
   BANNER pour documenter l'origine de chaque racine.
3. IRA -PREPROC -CONFIG (passe 1) : IRA suit le flot depuis ces racines ;
   tout le reste des hunks CODE devient DC.x.
4. Noms : avec --old, les labels de l'ancien source sont conservés à leur
   adresse (directives SYMBOL / EXTSYM) pour garder les références des
   documents ; les labels nouveaux sont nommés L<hunk>_<offset>.
5. IRA passe 2 avec les noms, puis réassemblage vasm et comparaison hunk par
   hunk (contenu + relocations) avec le binaire original.

Les adresses IRA sont linéaires : hunks bout à bout (taille allouée, BSS
compris), à partir de OFFSET 0.

Nécessite IRA 2.11 patché (voir ira-2.11-keep-cnf.patch).
"""
import argparse
import collections
import os
import re
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import m68kdis  # noqa: E402
import hunk as hunkmod  # noqa: E402
import symbols  # noqa: E402


def linear(prog):
    base, acc = [], 0
    for h in prog.hunks:
        base.append(acc)
        acc += h['size']
    return base


def header_equs(path):
    """EQU absolues de l'en-tête IRA : {nom: valeur}."""
    res = {}
    for line in open(path, encoding='latin-1'):
        if re.match(r'\s*SECTION', line):
            break
        m = re.match(r'(\w+)\s+EQU\s+\$([0-9A-Fa-f]+)', line)
        if m:
            res[m.group(1)] = int(m.group(2), 16)
    return res


def run_ira(ira, binary, cnf_lines, tmp):
    name = os.path.basename(binary)
    b = os.path.join(tmp, name)
    shutil.copy(binary, b)
    open(b + '.cnf', 'w').write('\n'.join(cnf_lines) + '\n')
    out = b + '.asm'
    if os.path.exists(out):
        os.remove(out)
    r = subprocess.run([os.path.abspath(ira), '-M68000', '-PREPROC', '-CONFIG', name, name + '.asm'],
                       cwd=tmp, stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if r.returncode != 0 or not os.path.exists(out):
        sys.exit('IRA a échoué :\n' + r.stderr.decode('latin-1')[-2000:])
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('binary')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--ira', required=True)
    ap.add_argument('--vasm', required=True)
    ap.add_argument('--old', help='ancien source (noms de labels à conserver)')
    ap.add_argument('--baseline', help='source de référence pour la comparaison du rapport '
                    '(par défaut : --old)')
    ap.add_argument('--sym', help='fichier de symboles (noms + commentaires, prioritaire)')
    ap.add_argument('--heuristic', action='store_true')
    ap.add_argument('--cnf', help='copie du .cnf final')
    ap.add_argument('--report')
    a = ap.parse_args()
    a.ira, a.vasm = os.path.abspath(a.ira), os.path.abspath(a.vasm)
    a.syms = symbols.load(a.sym) if a.sym else {}

    prog = m68kdis.Program(a.binary)
    prog.run(heuristic=a.heuristic)
    base = linear(prog)

    # ---- 1. racines de code
    roots = [('PROBABLE', r) for r in prog.roots['PROBABLE']]
    roots += [('TABLE', (h, b)) for (h, o, b, n, k) in prog.jump_tables]
    roots += [('HEURISTIQUE', r) for r in prog.roots['HEURISTIQUE']]
    roots.sort(key=lambda x: base[x[1][0]] + x[1][1])
    why = {'PROBABLE': 'cible de pointeur validée (analyse m68kdis)',
           'TABLE': 'cible de saut calculé JMP d8(PC,Dn)',
           'HEURISTIQUE': 'bloc non atteint, décodage valide - A VERIFIER'}
    dropped = []
    old_labels, old_insn = {}, set()
    if a.old:
        old_labels, _, old_insn = m68kdis.import_labels(a.vasm, a.old)
    while True:
        cnf = build_cnf(prog, roots, base, why)
        ok, diffs, new_labels, new_insn = build(a, cnf, old_labels)
        if ok or not diffs:
            break
        # Un bloc HEURISTIQUE dont l'encodage n'est pas reproduit par vasm
        # n'est pas du code produit par l'assembleur d'origine : on le retire.
        bad = set()
        for h, off in diffs:
            cands = [r for lvl, r in roots if lvl == 'HEURISTIQUE' and r[0] == h and r[1] <= off]
            k = prog.owner.get((h, off))
            x = off
            while k is None and x > 0:
                x -= 1
                k = prog.owner.get((h, x))
            r = prog.root_of.get(k) if k else None
            if r is not None and ('HEURISTIQUE', r) in roots:
                bad.add(r)
            elif cands:
                bad.add(max(cands, key=lambda c: c[1]))
        if not bad:
            break
        for r in bad:
            roots.remove(('HEURISTIQUE', r))
            prog.drop_root(r)
            dropped.append(r)

    print('%s : %d racines, réassemblage %s' % (
        a.output, len(roots), 'IDENTIQUE' if ok else 'DIFFÉRENT'))
    if a.report:
        base_labels, base_insn = old_labels, old_insn
        if a.baseline:
            base_labels, _, base_insn = m68kdis.import_labels(a.vasm, a.baseline)
        write_report(a, prog, roots, base_labels, base_insn, new_labels, new_insn, ok, dropped)
    sys.exit(0 if ok else 2)


def build_cnf(prog, roots, base, why):
    cnf = ['MACHINE 68000', 'ENTRY $00000000', 'OFFSET $00000000',
           'CODE $00000000']
    cnf += ['CODE $%08X' % (base[h] + o) for _, (h, o) in roots]
    cnf += ['BANNER $%08X [%s] %s' % (base[h] + o, lvl, why[lvl]) for lvl, (h, o) in roots]
    for (h, o) in sorted(prog.trace_on):
        adr = base[h] + o
        cnf += ['BANNER $%08X [TRACE] Activation du mode trace (protection Rob Northen Copylock).' % adr,
                'BANNER $%08X [TRACE] La suite est du code CHIFFRE, dechiffre a la volee par le' % adr,
                'BANNER $%08X [TRACE] handler TRACE : ce qui suit n\'a PAS de sens statiquement.' % adr]
    return cnf + ['END']


def build(a, cnf, old_labels):
    """IRA + renommage + réassemblage. Renvoie (ok, diffs, labels, insn)."""
    with tempfile.TemporaryDirectory() as tmp:
        out = run_ira(a.ira, a.binary, cnf, tmp)
        new_labels, _, new_insn = m68kdis.import_labels(a.vasm, out)
        if a.old:
            # Renommage textuel (la directive SYMBOL d'IRA 2.11 ignore
            # certains labels) : adresse -> ancien nom, sinon L<hunk>_<off>.
            rename = {}
            for k, nn in new_labels.items():
                if k in a.syms:
                    rename[nn] = a.syms[k]['name']
                elif not nn.startswith('SECSTRT_'):
                    rename[nn] = old_labels.get(k) or 'L%02d_%05X' % k
            old_ext = {v: n for n, v in header_equs(a.old).items() if n.startswith('EXT_')}
            for n, v in header_equs(out).items():
                if n.startswith('EXT_'):
                    rename[n] = old_ext.get(v, 'ABS_%X' % v)
            text = open(out, encoding='latin-1').read()
            text = re.sub(r'\b(?:(?:LAB|EXT)_[0-9A-Fa-f]+|SECSTRT_\d+)\b',
                          lambda m: rename.get(m.group(0), m.group(0)), text)
            text = symbols.annotate(text, a.syms)
            open(out, 'w', encoding='latin-1').write(text)
            new_labels, _, new_insn = m68kdis.import_labels(a.vasm, out)
        shutil.copy(out, a.output)
        if a.cnf:
            open(a.cnf, 'w').write('\n'.join(cnf) + '\n')
        exe = os.path.join(tmp, 're.exe')
        r = subprocess.run([a.vasm, '-quiet', '-Fhunkexe', '-nosym', '-no-opt', '-m68000',
                            '-o', exe, os.path.abspath(a.output)],
                           capture_output=True, text=True, errors='replace')
        if r.returncode != 0:
            print(r.stdout[-2000:] + r.stderr[-2000:])
            return False, [], new_labels, new_insn
        diffs = hunkmod.diff_offsets(a.binary, exe)
        if diffs:
            diffs = first_mismatch(a, tmp)
        return not diffs, diffs, new_labels, new_insn


def first_mismatch(a, tmp):
    """Première ligne du listing dont les octets diffèrent de l'original,
    par hunk, en ignorant les champs relogés (leur valeur suit les
    décalages). Renvoie [(hunk, offset)]."""
    lst = os.path.join(tmp, 'chk.lst')
    subprocess.run([a.vasm, '-quiet', '-Fhunkexe', '-nosym', '-no-opt', '-m68000',
                    '-L', lst, '-Lnf', '-o', os.devnull, os.path.abspath(a.output)],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    hunks = hunkmod.parse(a.binary)
    masks = []
    for hk in hunks:
        m = set()
        for p in hk['relocs']:
            m.update(range(p, p + 4))
        masks.append(m)
    res = {}
    for line in open(lst, encoding='latin-1'):
        mm = re.match(r'^([0-9A-F]{2}):([0-9A-F]{8}) ([0-9A-F]+)\s*\t', line)
        if not mm:
            continue
        h, off = int(mm.group(1), 16), int(mm.group(2), 16)
        if h in res or h >= len(hunks):
            continue
        b = bytes.fromhex(mm.group(3))
        d = hunks[h]['data']
        for i, c in enumerate(b):
            if off + i in masks[h]:
                continue
            if off + i >= len(d) or d[off + i] != c:
                res[h] = off
                break
    return sorted(res.items())


def write_report(a, prog, roots, old_labels, old_insn, new_labels, new_insn, ok, dropped=()):
    name = os.path.basename(a.binary)
    lab = {}
    lab.update(new_labels)

    def L(k):
        return lab.get(k, '%d:$%X' % k)

    R = ['# Désassemblage de `%s` — rapport\n' % name,
         '- Réassemblage vasm → binaire original : **%s** (contenu et relocations de chaque hunk)' %
         ('identique' if ok else 'DIFFÉRENT'),
         '- Racines de code fournies à IRA : %d (%s)' % (
             len(roots), ', '.join('%s %d' % kv for kv in collections.Counter(r[0] for r in roots).items())),
         '']
    R += ['## Classification par hunk CODE\n',
          'Octets de code trouvés par l\'analyse (CERTAIN = atteint depuis l\'entrée ; '
          'PROBABLE = via pointeur validé ; HEURISTIQUE = non atteint mais décodage valide, à vérifier). Les deux dernières colonnes comparent les débuts '
          'd\'instruction du nouveau source à ceux de l\'ancien.\n',
          '| Hunk | Taille | CERTAIN | PROBABLE | HEURISTIQUE | Données | instr. ancien → données | données ancien → instr. |',
          '|---|---|---|---|---|---|---|---|']
    tot = collections.Counter()
    for hi, hk in enumerate(prog.hunks):
        if hk['type'] != 'CODE':
            continue
        by = collections.Counter()
        for off in range(len(hk['data'])):
            o = prog.owner.get((hi, off))
            by[prog.level[o] if o else 'DATA'] += 1
        lost = sum(1 for k in old_insn if k[0] == hi and k not in new_insn)
        gained = sum(1 for k in new_insn if k[0] == hi and k not in old_insn)
        tot.update(by)
        tot['lost'] += lost
        tot['gained'] += gained
        R.append('| S_%d%s | %d | %d | %d | %d | %d | %d | %d |' % (
            hi, ',CHIP' if hk['mem'] == 'CHIP' else '', len(hk['data']), by['CERTAIN'],
            by['PROBABLE'], by['HEURISTIQUE'], by['DATA'], lost, gained))
    R.append('| **Total** | | %d | %d | %d | %d | %d | %d |\n' % (
        tot['CERTAIN'], tot['PROBABLE'], tot['HEURISTIQUE'], tot['DATA'], tot['lost'], tot['gained']))

    if prog.trace_on:
        R.append('## Mode trace / protection anti-copie\n')
        for k in sorted(prog.trace_on):
            R.append('- `%s` : `ORI.W #$8xxx,SR` active le mode trace. Signature Rob Northen '
                     'Copylock : la suite du hunk est du code chiffré, non désassemblable '
                     'statiquement (balisé `[TRACE]` dans le source).' % L(k))
        R.append('')
    if prog.jump_tables:
        R.append('## Sauts calculés\n')
        for h, o, b, n, k in prog.jump_tables:
            R.append('- `%s` : `JMP d8(PC,Dn)` → `%s` (%s)' % (L((h, o)), L((h, b)), k))
        R.append('')
    if prog.smc:
        R.append('## Code auto-modifiant\n')
        seen = set()
        for h, b, o, w in sorted(prog.smc):
            if o in seen:
                continue
            seen.add(o)
            I = prog.code[o]
            R.append('- `%s` (`%s %s`) est écrit par %s' % (L(o), I.mn, I.ops, w))
        R.append('')
    if prog.anomalies:
        R.append('## Anomalies sur le chemin CERTAIN\n')
        for h, o, t in sorted(set((x[0] if isinstance(x[0], int) else -1, x[1] or 0, x[2])
                                  for x in prog.anomalies)):
            R.append('- `%s` : %s' % (L((h, o)), t))
        R.append('')
    if dropped:
        R.append('## Blocs heuristiques retirés au réassemblage (%d)\n' % len(dropped))
        R.append('Décodables, mais leur encodage n\'est pas celui que produit un assembleur : '
                 'ce sont des données qui ressemblent à du code.\n')
        for k in sorted(dropped):
            R.append('- `%d:$%X`' % k)
        R.append('')
    R.append('## Pointeurs vers un hunk CODE rejetés comme code (%d)\n' % len(prog.rejected))
    R.append('Ces cibles sont référencées par adresse mais leur décodage échoue : ce sont des '
             'données placées dans un hunk CODE (variables, tables, textes).\n')
    for k, w in sorted(prog.rejected.items()):
        R.append('- `%s` : %s' % (L(k), w))
    if old_labels:
        kept = sum(1 for k, n in old_labels.items() if new_labels.get(k) == n)
        gone = sorted((k, n) for k, n in old_labels.items() if k not in new_labels)
        R.append('\n## Labels\n')
        renamed = sum(1 for k in old_labels if k in a.syms and new_labels.get(k) == a.syms[k]['name']
                      and old_labels[k] != a.syms[k]['name'])
        R.append('- %d labels de l\'ancien source conservés à la même adresse.' % kept)
        R.append('- %d labels renommés d\'après `amiga_asm/%s.sym`.' % (renamed, name))
        R.append('- %d labels anciens disparus (ils pointaient dans des données '
                 'mal décodées ou au milieu d\'instructions) :\n' % len(gone))
        R.append('  ' + ', '.join('`%s`' % n for k, n in gone[:400]))
    open(a.report, 'w').write('\n'.join(R) + '\n')


if __name__ == '__main__':
    main()
