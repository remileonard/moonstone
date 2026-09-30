#!/usr/bin/env python3
"""
ix_scripts.py — identification et extraction des scripts IMAGEXCEL de
Moonstone, à partir des binaires (program, mog) et de leurs relocations.

Remplace extract_scripts.py / ix_validate.py, qui devinaient les scripts en
balayant le texte asm avec la sémantique de l'intro pour tous les opcodes.

Méthode (déterministe)
----------------------
1. Candidats : toute adresse désignée par un pointeur (relocation, ou
   référence relative au PC dans le code) qui n'est pas du code.
2. Validation stricte, avec la sémantique exacte du moteur du binaire
   (tables d'opcodes de `program` et de `mog`, qui diffèrent) :
     - dessin (octet 0 < $80) : 6 octets, octet 0 multiple de 4 et <= $1C,
       aucune relocation dedans ;
     - fin d'étape FF xx ; FF FF termine le script ;
     - $FD / $FE : saut dynamique (contexte), fin du chemin statique ;
     - opcode >= $80 : taille exacte ; les adresses doivent être relogées
       exactement aux positions prévues, et nulle part ailleurs ;
     - opcodes inexistants ou qui n'avancent pas le pc : rejet ;
     - le flot (sauts, conditions) est suivi jusqu'au bout.
3. Rôles : champs de l'objet où le code range le script ou sa table
   (MOVE.L #x,off(An)), entrées de tables, références depuis d'autres
   scripts ($84, $AC ombre, $B4, $B8 création d'entité, $C4, $C8, $CC…),
   références depuis le code (routine la plus proche).

Sorties
-------
  docs/scripts/<bin>_scripts.md   catalogue + décodage de chaque script
  game/data/ix_<bin>.c            hunks de données, relocations, index des
                                  scripts et des tables (données fidèles)
  game/include/ix_data.h          types communs

Usage : python3 tools/ix_scripts.py [program] [mog]
"""
import collections
import os
import re
import struct
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(HERE, 'disasm'))
import hunk as hunkmod  # noqa: E402
import m68kdis  # noqa: E402

VASM = os.path.join(HERE, 'disasm', '.build', 'vasm', 'vasmm68k_mot')

# ---------------------------------------------------------------------------
# Sémantique des moteurs
#   op -> (taille, [(position, genre, obligatoire)], flot, nom)
#   genre : 'script' (cible suivie), 'spawn', 'shadow' (scripts à part),
#           'code' (routine native), 'value' (valeur quelconque)
#   flot  : 'next', 'jump84' (m=3 immédiat, sinon différé), 'cond'
# ---------------------------------------------------------------------------

ENGINE_MOG = {
    0x80: (2, [], 'next', 'SetDir'),
    0x84: (6, [(2, 'script', True)], 'jump84', 'Jump'),
    0x88: (2, [], 'next', 'Hold'),
    0x8C: (8, [], 'next', 'Physics'),
    0x94: (2, [], 'next', 'Loop'),
    0x98: (6, [(2, 'script', True)], 'cond', 'SkipIfDebug'),
    0xA0: (8, [], 'next', 'Move'),
    0xA4: (2, [], 'next', 'Sound'),
    0xA8: (8, [(4, 'value', False)], 'next', 'SetField'),
    0xAC: (6, [(2, 'shadow', False)], 'next', 'Shadow'),   # arg 0 : ombre coupée, adresse ignorée
    0xB0: (6, [(2, 'code', True)], 'next', 'Call'),
    0xB4: (6, [(2, 'script', True)], 'cond', 'IfDead'),
    0xB8: (6, [(2, 'spawn', True)], 'next', 'Spawn'),
    0xBC: (2, [], 'next', 'Kill'),
    0xC0: (2, [], 'next', 'SetBank'),
    0xC4: (6, [(2, 'script', True)], 'cond', 'IfSameFacing'),
    0xC8: (8, [(4, 'script', True)], 'cond', 'IfFieldZero'),
    0xCC: (8, [(4, 'script', True)], 'cond', 'IfFieldNonZero'),
    0xD0: (2, [], 'next', 'Reset'),
}

ENGINE_PROGRAM = {
    0x80: (2, [], 'next', 'SetDir'),
    0x84: (6, [(2, 'script', True)], 'jump84', 'Jump'),
    0x88: (2, [], 'next', 'Hold'),
    0x8C: (8, [], 'next', 'Skip8'),
    0x94: (2, [], 'next', 'Loop'),
    0xA0: (8, [], 'next', 'Move'),
    0xA4: (4, [], 'next', 'Skip4'),
    0xB4: (6, [(2, 'code', False)], 'next', 'Call'),
    0xB8: (6, [(2, 'value', False)], 'next', 'Skip6'),
    0xBC: (6, [(2, 'value', False)], 'next', 'Skip6'),
    0xC0: (2, [], 'next', 'Kill'),
    0xC4: (2, [], 'next', 'SetBank'),
    0xC8: (6, [(2, 'value', False)], 'next', 'Skip6'),
    0xCC: (8, [(4, 'script', True)], 'cond', 'IfFieldZero'),
    0xD0: (8, [(4, 'script', True)], 'cond', 'IfFieldNonZero'),
    0xD4: (2, [], 'next', 'Reset'),
}

ENGINES = {'mog': ENGINE_MOG, 'program': ENGINE_PROGRAM}

# Champs de l'objet (chevalier / créature) qui reçoivent un script ou une
# table de scripts (lus par les contrôleurs).
OBJ_FIELDS = {
    22: ('script', 'repos'),
    26: ('script', 'réaction'),
    30: ('table', 'scripts objet+30'),
    34: ('table', 'attaques'),
    42: ('table', 'dégâts par attaque (nombres)'),
    46: ('table', 'marche'),
    50: ('table', 'nombres objet+50 (comparés à l\'attaque adverse)'),
}

STICK = [(0, 'D'), (1, 'G'), (2, 'B'), (3, 'H')]   # droite gauche bas haut


def entry_name(field, off, attack_names):
    """Nom d'une entrée de table d'objet (offset en octets)."""
    if field == 46:
        g = {0: 'horizontal', 1: 'haut', 2: 'bas'}.get(off // 32, 'groupe %d' % (off // 32))
        return 'marche %s phase %d' % (g, (off % 32) // 4)
    if field == 34:
        return 'attaque %d%s' % (off // 4, attack_names.get(off, ''))
    return '[%d]' % (off // 4)


def attack_names(b):
    """Index d'attaque (offset) -> combinaisons du joystick (feu +)."""
    res = collections.defaultdict(list)
    for side, lab in (('regard à droite', 't_AttackStickR'), ('regard à gauche', 't_AttackStickL')):
        k = next((a for a, n in b.labels.items() if n == lab), None)
        if k is None:
            continue
        d = b.data(k[0])
        for bits in range(16):
            v = struct.unpack('>H', d[k[1] + 2 * bits:k[1] + 2 * bits + 2])[0]
            combo = '+'.join(n for i, n in STICK if bits >> i & 1) or 'neutre'
            res[v].append('%s (%s)' % (combo, side))
    out = {}
    for v, cs in res.items():
        right = [c.split(' (')[0] for c in cs if 'droite' in c]
        if right:
            out[v] = ' : feu + ' + ' ou '.join(right)
    return out


def runtime_tables(b, scripts):
    """Tables de scripts remplies par le code (MOVE.L #script,d(An)) :
    {(genre, clé): {offset: script}} ; genre 'label' (table à une adresse)
    ou 'field' (table désignée par le champ F d'un objet)."""
    tabs = collections.defaultdict(dict)
    field_of_table = collections.defaultdict(set)   # table -> champs objet
    b.table_setters = {}
    for hi in range(len(b.hunks)):
        keys = sorted(k for k in b.prog.code if k[0] == hi)
        regs = {}
        for k in keys:
            I = b.prog.code[k]
            if k in b.names:
                regs = {}
            mn = I.mn.split('.')[0]
            ops = I.ops
            dst = re.search(r',\s*(a[0-7])$', ops)
            # An := table (LEA T / MOVEA.L #T)
            if dst and mn in ('lea', 'movea') and I.relocs:
                t = list(I.relocs.values())[0]
                regs[dst.group(1)] = ('label', t)
                continue
            # An := table désignée par le champ F d'un objet
            m = re.match(r'(-?)\$([0-9a-f]+)\((a[0-7])\),\s*(a[0-7])$', ops)
            if mn == 'movea' and m:
                f = int(m.group(2), 16)
                if OBJ_FIELDS.get(f, ('',))[0] == 'table' and not m.group(1):
                    regs[m.group(4)] = ('field', f)
                else:
                    regs.pop(m.group(4), None)
                continue
            # MOVE.L #X,d(An)
            m = re.match(r'#\$([0-9a-f]+),\s*(?:(-?)\$([0-9a-f]+))?\((a[0-7])\)$', ops)
            if I.mn == 'move.l' and m and m.group(4) in regs:
                off = int(m.group(3), 16) if m.group(3) else 0
                src = list(I.relocs.values())
                reg = regs[m.group(4)]
                if src and src[0] in scripts:
                    tabs[reg][off] = src[0]
                elif src and reg[0] == 'label' and off in OBJ_FIELDS:
                    field_of_table[src[0]].add(off)
                continue
            if dst:
                regs.pop(dst.group(1), None)
            if mn in ('rts', 'jmp', 'bra', 'jsr', 'bsr', 'rte'):
                regs = {}          # un appel peut modifier les registres
    # champs d'objet posés directement : MOVE.L #T,F(An) (An = objet)
    for I in b.prog.code.values():
        m = re.match(r'#\$([0-9a-f]+),\s*\$([0-9a-f]+)\((a[0-7])\)$', I.ops)
        if I.mn == 'move.l' and m and I.relocs:
            off = int(m.group(2), 16)
            if off in OBJ_FIELDS:
                t = list(I.relocs.values())[0]
                field_of_table[t].add(off)
                b.table_setters.setdefault(t, set()).add(b.routine_of((I.h, I.off)))
    return tabs, field_of_table


class ScriptError(Exception):
    pass


class Binary:
    def __init__(self, name):
        self.bin = name
        self.path = os.path.join(ROOT, 'amiga_asm', name)
        self.engine = ENGINES[name]
        self.hunks = hunkmod.parse(self.path)
        self.rel = [dict(h['relocs']) for h in self.hunks]
        self.labels, _, _ = m68kdis.import_labels(VASM, self.path + '.asm')
        self.names = {}
        for k, n in self.labels.items():
            self.names.setdefault(k, n)
        self.prog = m68kdis.Program(self.path)
        self.prog.run(heuristic=True)

    def data(self, h):
        return self.hunks[h]['data']

    def target(self, h, pos):
        t = self.rel[h].get(pos)
        if t is None:
            return None
        return (t, struct.unpack('>I', self.data(h)[pos:pos + 4])[0])

    def is_code(self, k):
        return k in self.prog.owner

    def name(self, k):
        if k in self.names:
            return self.names[k]
        prev = [x for x in self.names if x[0] == k[0] and x[1] < k[1]]
        if prev:
            p = max(prev)
            return '%s+%d' % (self.names[p], k[1] - p[1])
        return 'S_%d+$%X' % k

    def routine_of(self, k):
        """Label de code le plus proche avant k (routine englobante)."""
        prev = [x for x in self.names if x[0] == k[0] and x[1] <= k[1]
                and self.is_code(x)]
        return self.names[max(prev)] if prev else 'S_%d' % k[0]

    # -- analyse d'un script -------------------------------------------------
    def parse(self, root):
        """Suit le flot depuis root. Renvoie dict(records, bytes, refs) ou
        lève ScriptError."""
        h0 = root[0]
        records = {}      # (h, off) -> (genre, taille, détails)
        owned = set()
        refs = []         # (off_record, genre, cible)
        work = [root]
        while work:
            k = work.pop()
            h, pc = k
            d = self.data(h)
            while True:
                if (h, pc) in records:
                    break
                if pc < 0 or pc >= len(d):
                    raise ScriptError('sort du hunk en $%X' % pc)
                if self.is_code((h, pc)):
                    raise ScriptError('entre dans du code en $%X' % pc)
                b = d[pc]
                if b == 0xFF:
                    if pc + 2 > len(d):
                        raise ScriptError('FF tronqué')
                    self._own(h, pc, 2, owned)
                    xx = d[pc + 1]
                    records[(h, pc)] = ('end', 2, xx)
                    if xx == 0xFF:
                        break
                    pc += 2
                    continue
                if b in (0xFD, 0xFE):
                    self._own(h, pc, 1, owned)
                    records[(h, pc)] = ('dyn', 1, b)
                    break
                if b >= 0x80:
                    spec = self.engine.get(b)
                    if spec is None:
                        raise ScriptError('opcode $%02X inexistant en $%X' % (b, pc))
                    size, args, flow, _ = spec
                    if pc + size > len(d):
                        raise ScriptError('opcode tronqué en $%X' % pc)
                    allowed = {pc + p for p, _, _ in args}
                    for q in range(pc, pc + size):
                        if q in self.rel[h] and q not in allowed:
                            raise ScriptError('relocation inattendue en $%X' % q)
                    tgts = []
                    for p, kind, req in args:
                        t = self.target(h, pc + p)
                        if t is None:
                            if req:
                                raise ScriptError('adresse non relogée (op $%02X en $%X)'
                                                  % (b, pc))
                            continue
                        tgts.append((kind, t))
                        refs.append(((h, pc), kind, t))
                    self._own(h, pc, size, owned)
                    records[(h, pc)] = ('op', size, b)
                    nxt = pc + size
                    for kind, t in tgts:
                        if kind == 'script':
                            work.append(t)
                    if flow == 'jump84' and d[pc + 1] == 3:
                        break
                    pc = nxt
                    continue
                # dessin
                if b & 0x63:
                    raise ScriptError('octet de dessin invalide $%02X en $%X' % (b, pc))
                if pc + 6 > len(d):
                    raise ScriptError('dessin tronqué')
                for q in range(pc, pc + 6):
                    if q in self.rel[h]:
                        raise ScriptError('relocation dans un dessin en $%X' % q)
                self._own(h, pc, 6, owned)
                records[(h, pc)] = ('draw', 6, b)
                pc += 6
        if not any(r[0] == 'end' for r in records.values()):
            raise ScriptError('aucune fin d\'étape')
        return {'root': root, 'records': records, 'owned': owned, 'refs': refs}

    @staticmethod
    def _own(h, pc, n, owned):
        for q in range(pc, pc + n):
            owned.add((h, q))


# ---------------------------------------------------------------------------
# Identification
# ---------------------------------------------------------------------------

def identify(b):
    # candidats : cibles de relocations + références PC-relatives du code
    cands = set()
    for hi, rel in enumerate(b.rel):
        for pos in rel:
            t = b.target(hi, pos)
            if t[0] < len(b.hunks) and b.hunks[t[0]]['type'] != 'BSS' \
                    and not b.is_code(t):
                cands.add(t)
    for I in b.prog.code.values():
        for t in I.addr_refs:
            if isinstance(t[0], int) and not b.is_code(t):
                cands.add(t)
    scripts, rejected = {}, {}
    pending = sorted(cands)
    while pending:
        c = pending.pop()
        if c in scripts or c in rejected:
            continue
        if c[1] >= len(b.data(c[0])):
            rejected[c] = 'hors données'
            continue
        try:
            s = b.parse(c)
        except ScriptError as e:
            rejected[c] = str(e)
            continue
        scripts[c] = s
        for _, kind, t in s['refs']:
            if kind in ('shadow', 'spawn', 'script', 'value') and t not in scripts:
                pending.append(t)
    # filtre sur l'usage : un bloc que seul le code désigne, et qu'il traite
    # comme des données, n'est pas un script
    from_scripts = set()
    for s in scripts.values():
        for _, kind, t in s['refs']:
            if kind in ('shadow', 'spawn', 'script', 'value'):
                from_scripts.add(t)
    changed = True
    while changed:
        changed = False
        for k in list(scripts):
            if k in from_scripts:
                continue
            use = code_usage(b, k)
            kinds = {u[0] for u in use}
            empty = not any(r[0] in ('draw', 'op') for r in scripts[k]['records'].values())
            if ('data' in kinds and 'script' not in kinds) or \
                    (empty and 'script' not in kinds):
                why = next((u[1] for u in use if u[0] == 'data'), 'script vide')
                rejected[k] = 'données selon le code (%s)' % why
                del scripts[k]
                changed = True
    return scripts, rejected


def code_usage(b, k):
    """Comment le code utilise l'adresse k : ('script', motif) ou
    ('data', motif) pour chaque référence depuis le code."""
    out = []
    ctl = next((a for a, n in b.labels.items() if n == 'v_CtlScript'), None)
    for I in b.prog.code.values():
        hits = [t for t in I.relocs.values() if t == k] + \
               [t for t in I.addr_refs if t == k and not I.relocs]
        if not hits:
            continue
        mn = I.mn.split('.')[0]
        ops = I.ops
        if (I.h, I.off) and k in b.prog.proven_data:
            out.append(('data', 'accès direct'))
            continue
        if mn == 'move' and ops.startswith('#'):
            dst = ops.split(',', 1)[1].strip()
            if ctl and re.fullmatch(r'\$%x\.l' % ctl[1], dst) and ctl[0] == ctl[0]:
                out.append(('script', 'v_CtlScript'))
            elif re.fullmatch(r'-?\$[0-9a-f]+\(a[0-7]\)|\(a[0-7]\)\+?', dst):
                out.append(('script', 'rangé dans une table/objet'))
            else:
                out.append(('?', 'move'))
            continue
        m = re.match(r'(?:lea|movea)\.?l?$', I.mn) or mn in ('lea', 'movea')
        reg = re.search(r',\s*(a[0-7])$', ops)
        if m and reg:
            r = reg.group(1)
            nxt = (I.h, I.off + I.size)
            verdict = ('?', 'registre')
            for _ in range(6):
                J = b.prog.code.get(nxt)
                if J is None:
                    break
                jm = J.mn.split('.')[0]
                if re.search(r'\(%s(?:\)|,)' % r, J.ops) and jm not in ('lea', 'pea', 'jsr', 'jmp'):
                    src_first = J.ops.find('(' + r) < J.ops.find(',') if ',' in J.ops else True
                    if J.mn.endswith(('.w', '.b')) or not src_first:
                        verdict = ('data', 'table lue/écrite via %s' % r)
                        break
                if jm in ('jsr', 'bsr', 'jmp', 'bra'):
                    verdict = ('script?', 'passé à une routine')
                    break
                if re.search(r',\s*%s$' % r, J.ops):
                    break
                nxt = (J.h, J.off + J.size)
            out.append(verdict)
            continue
        out.append(('?', mn))
    return out


def roles(b, scripts):
    """Rôles de chaque script et tables de scripts."""
    role = collections.defaultdict(list)
    # 1) références depuis les scripts
    for k, s in scripts.items():
        for at, kind, t in s['refs']:
            if t in scripts and kind == 'value':
                h, pc = at
                off = struct.unpack('>H', b.data(h)[pc + 2:pc + 4])[0]
                f = OBJ_FIELDS.get(off)
                role[t].append('écrit dans objet+%d%s par $A8 depuis %s' % (
                    off, ' (%s)' % f[1] if f else '', b.name(k)))
                continue
            if t in scripts and kind in ('shadow', 'spawn', 'script'):
                what = {'shadow': 'ombre ($AC)', 'spawn': 'entité créée ($B8)',
                        'script': 'cible de saut'}[kind]
                role[t].append('%s depuis %s' % (what, b.name(k)))
    # 2) tables : suites de pointeurs relogés vers des scripts
    tables = {}
    in_script = set()
    for s in scripts.values():
        in_script |= s['owned']
    for hi, rel in enumerate(b.rel):
        pos = sorted(p for p in rel if not b.is_code((hi, p))
                     and (hi, p) not in in_script)
        i = 0
        while i < len(pos):
            j = i
            while j + 1 < len(pos) and pos[j + 1] == pos[j] + 4:
                j += 1
            run = pos[i:j + 1]
            ents = [b.target(hi, p) for p in run]
            if any(e in scripts for e in ents):
                # découpe aux labels : chaque label démarre une table
                cur = None
                for p, e in zip(run, ents):
                    if cur is None or (hi, p) in b.names:
                        cur = (hi, p)
                        tables[cur] = []
                    tables[cur].append(e)
            i = j + 1
    # 3) références depuis le code
    obj_refs = collections.defaultdict(list)
    for I in b.prog.code.values():
        for t in I.addr_refs:
            if not I.relocs and t in scripts:
                obj_refs[t].append((None, b.routine_of((I.h, I.off))))
        for pos, t in I.relocs.items():
            if t not in scripts and t not in tables:
                continue
            m = re.match(r'move\.l', I.mn)
            disp = re.search(r',\s*(-?)\$([0-9a-f]+)\((a[0-7])\)$', I.ops)
            where = b.routine_of((I.h, I.off))
            if m and I.ops.startswith('#') and disp:
                off = int(disp.group(2), 16) * (-1 if disp.group(1) else 1)
                obj_refs[t].append((off, where))
            else:
                obj_refs[t].append((None, where))
    for t, lst in obj_refs.items():
        for off, where in lst:
            f = OBJ_FIELDS.get(off)
            if t in scripts:
                if f and f[0] == 'script':
                    role[t].append('objet+%d (%s) dans %s' % (off, f[1], where))
                else:
                    role[t].append('référencé par le code dans %s' % where)
    # 4) tables construites à l'exécution
    rt, field_of_table = runtime_tables(b, scripts)
    anames = attack_names(b)
    b.rt_tables = {}
    for (kind, key), ents in rt.items():
        fields = sorted(field_of_table.get(key, ())) if kind == 'label' else [key]
        fdesc = ', '.join('objet+%d (%s)' % (f, OBJ_FIELDS[f][1]) for f in fields
                          if f in OBJ_FIELDS) or ('table %s' % b.name(key) if kind == 'label'
                                                  else 'objet+%d' % key)
        tname = b.name(key) if kind == 'label' else 'objet+%d' % key
        if kind == 'label' and key in b.table_setters:
            fdesc += ' ; posée par ' + ', '.join(sorted(b.table_setters[key]))
        b.rt_tables[(kind, key)] = (tname, fdesc, ents)
        for off, sc in sorted(ents.items()):
            f = fields[0] if fields else None
            role[sc].append('%s [%s] : %s' % (tname, fdesc, entry_name(f, off, anames)))
    tab_role = {}
    for t, ents in tables.items():
        rs = []
        for off, where in obj_refs.get(t, []):
            f = OBJ_FIELDS.get(off)
            if f and f[0] == 'table':
                rs.append('objet+%d : %s (%s)' % (off, f[1], where))
            else:
                rs.append('code : %s' % where)
        tab_role[t] = rs
        for i, e in enumerate(ents):
            if e in scripts:
                role[e].append('entrée %d de la table %s' % (i, b.name(t)))
    return role, tables, tab_role


# ---------------------------------------------------------------------------
# Décodage lisible
# ---------------------------------------------------------------------------

def s8(x):
    return x - 256 if x > 127 else x


def s16(x):
    return x - 65536 if x > 32767 else x


def decode(b, s):
    out = []
    recs = sorted(s['records'].items())
    step = 1
    new_step = True
    for (h, pc), (kind, size, v) in recs:
        d = b.data(h)
        lab = b.names.get((h, pc))
        if lab and (h, pc) != s['root']:
            out.append('  %s:' % lab)
        if new_step and kind != 'end':
            out.append('  ; étape %d' % step)
            new_step = False
        raw = ' '.join('%02X' % x for x in d[pc:pc + size])
        if kind == 'draw':
            fl = d[pc + 3]
            tags = []
            if fl & 1:
                tags.append('corps')
            if fl & 2:
                tags.append('frappe')
            if fl & 0x10:
                tags.append('décor')
            if fl & 0x40:
                tags.append('hors-boîte')
            txt = 'dessin banque %d frame %d dy %d dx %d%s' % (
                v >> 2, d[pc + 1], s8(d[pc + 2]), s16(struct.unpack('>H', d[pc + 4:pc + 6])[0]),
                (' [' + ','.join(tags) + ']') if tags else '')
        elif kind == 'end':
            xx = v
            txt = {0xFF: 'fin du script', 0xFE: 'fin d\'étape, boucle'}.get(
                xx, 'fin d\'étape')
            step += 1
            new_step = True
        elif kind == 'dyn':
            txt = 'saut au début de boucle' if v == 0xFE else 'saut au pc de reprise'
        else:
            name = b.engine[v][3]
            args = ''
            for p, k2, _ in b.engine[v][1]:
                t = b.target(h, pc + p)
                if t:
                    args += ' -> %s' % b.name(t)
            txt = '$%02X %s%s' % (v, name, args)
        out.append('    %-26s ; %s' % (raw, txt))
    return out


# ---------------------------------------------------------------------------
# Sorties
# ---------------------------------------------------------------------------

def c_ident(s):
    return re.sub(r'\W', '_', s)


def write_markdown(b, scripts, rejected, role, tables, tab_role, path):
    L = ['# Scripts IMAGEXCEL de `%s`\n' % b.bin,
         'Généré par `tools/ix_scripts.py` à partir du binaire et de ses '
         'relocations ; ne pas modifier à la main.\n',
         '- Moteur : %s' % ('combat (`t_IxOpcodes`)' if b.bin == 'mog'
                            else 'intro/fin (`LAB_0288`)'),
         '- Scripts identifiés : **%d** ; tables de scripts : **%d**' % (
             len(scripts), len(tables)),
         '- Pointeurs candidats rejetés (données d\'un autre type) : %d\n' % len(rejected)]
    L.append('## Tables de scripts\n')
    L.append('| Table | Adresse | Entrées | Rôle |')
    L.append('|---|---|---|---|')
    for t, ents in sorted(tables.items()):
        L.append('| `%s` | %d:$%X | %d | %s |' % (
            b.name(t), t[0], t[1], len(ents), '<br>'.join(tab_role[t]) or '—'))
    L.append('\n## Tables de scripts remplies par le code\n')
    L.append('Tables en mémoire (souvent en BSS) que le code remplit avec '
             '`MOVE.L #script,d(An)` ; les champs de l\'objet (30, 34, 46…) les '
             'désignent.\n')
    anames = attack_names(b)
    for (kind, key), (tname, fdesc, ents) in sorted(getattr(b, 'rt_tables', {}).items(),
                                                    key=lambda x: str(x[0])):
        fields = [int(x) for x in re.findall(r'objet\+(\d+)', fdesc)]
        f = fields[0] if fields else None
        L.append('**`%s`** — %s\n' % (tname, fdesc))
        L.append('| Offset | Entrée | Script |')
        L.append('|---|---|---|')
        for off, sc in sorted(ents.items()):
            L.append('| %d | %s | `%s` |' % (off, entry_name(f, off, anames), b.name(sc)))
        L.append('')
    L.append('\n## Scripts\n')
    L.append('| Script | Adresse | Octets | Étapes | Rôles |')
    L.append('|---|---|---|---|---|')
    for k, s in sorted(scripts.items()):
        steps = sum(1 for r in s['records'].values() if r[0] == 'end')
        rs = sorted(set(role.get(k, [])))
        L.append('| `%s` | %d:$%X | %d | %d | %s |' % (
            b.name(k), k[0], k[1], len(s['owned']), steps,
            '<br>'.join(rs[:6]) + (' …' if len(rs) > 6 else '') or '—'))
    L.append('\n## Décodage\n')
    for k, s in sorted(scripts.items()):
        L.append('### `%s` (%d:$%X)\n' % (b.name(k), k[0], k[1]))
        rs = sorted(set(role.get(k, [])))
        if rs:
            L.append('Rôles : ' + ' ; '.join(rs) + '\n')
        L.append('```')
        L.extend(decode(b, s))
        L.append('```\n')
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, 'w', encoding='utf-8').write('\n'.join(L) + '\n')


VM_BASE = 0x00100000     # adresse virtuelle du premier hunk (0 reste « nul »)


def hunk_bases(b):
    """Adresses virtuelles des hunks : bout à bout (taille allouée), comme IRA."""
    base, acc = [], VM_BASE
    for h in b.hunks:
        base.append(acc)
        acc += h['size']
    return base, acc - VM_BASE


def va(b, k):
    return hunk_bases(b)[0][k[0]] + k[1]


def write_c(b, scripts, tables, path):
    """Image mémoire complète du binaire + index des scripts et tables."""
    pre = 'ix_%s' % b.bin
    bases, total = hunk_bases(b)
    L = ['/*',
         ' * %s.c — image mémoire de `%s` pour le moteur IMAGEXCEL' % (pre, b.bin),
         ' * (générée par tools/ix_scripts.py ; ne pas modifier).',
         ' *',
         ' * Chaque hunk est placé à l\'adresse virtuelle IX_VM_BASE + somme des',
         ' * tailles allouées des hunks précédents ; ix_vm_load() copie les données,',
         ' * met le reste (BSS) à zéro et applique les relocations.',
         ' */', '', '#include "ix_data.h"', '']
    types = {'CODE': 'IX_HUNK_CODE', 'DATA': 'IX_HUNK_DATA', 'BSS': 'IX_HUNK_BSS'}
    for h, hk in enumerate(b.hunks):
        d = hk['data']
        if d:
            L.append('static const uint8_t %s_h%d[%d] = {' % (pre, h, len(d)))
            for i in range(0, len(d), 16):
                L.append('    ' + ', '.join('0x%02X' % x for x in d[i:i + 16]) + ',')
            L.append('};')
        rel = sorted(b.rel[h].items())
        if rel:
            L.append('static const IxReloc %s_h%d_rel[%d] = {' % (pre, h, len(rel)))
            for pos, t in rel:
                L.append('    { 0x%05X, %d },' % (pos, t))
            L.append('};')
        L.append('')
    L.append('static const IxHunk %s_hunk_list[%d] = {' % (pre, len(b.hunks)))
    for h, hk in enumerate(b.hunks):
        L.append('    { %s, 0x%08X, %d, %s, %d, %s, %d },' % (
            types[hk['type']], bases[h], hk['size'],
            ('%s_h%d' % (pre, h)) if hk['data'] else 'NULL', len(hk['data']),
            ('%s_h%d_rel' % (pre, h)) if b.rel[h] else 'NULL', len(b.rel[h])))
    L.append('};')
    L.append('')
    L.append('static const uint32_t %s_script_list[%d] = {' % (pre, max(1, len(scripts))))
    for k in sorted(scripts):
        L.append('    0x%08X,  /* %s */' % (va(b, k), b.name(k)))
    L.append('};')
    L.append('')
    L.append('const IxImage %s_image = {' % pre)
    L.append('    "%s", %s_hunk_list, %d, 0x%X,' % (b.bin, pre, len(b.hunks), total))
    L.append('    %s_script_list, %d' % (pre, len(scripts)))
    L.append('};')
    os.makedirs(os.path.dirname(path), exist_ok=True)
    open(path, 'w').write('\n'.join(L) + '\n')

    # en-tête des symboles : adresse virtuelle de chaque label
    up = b.bin.upper()
    H = ['/*', ' * %s_syms.h — adresses virtuelles des labels de amiga_asm/%s.asm' % (pre, b.bin),
         ' * (générées par tools/ix_scripts.py ; ne pas modifier).', ' */',
         '#ifndef IX_%s_SYMS_H' % up, '#define IX_%s_SYMS_H' % up, '']
    seen = set()
    for k, n in sorted(b.labels.items()):
        if n in seen or not re.match(r'^[A-Za-z_]\w*$', n):
            continue
        seen.add(n)
        H.append('#define %s_%s 0x%08Xu' % (up, n, va(b, k)))
    H += ['', '#endif', '']
    open(os.path.join(os.path.dirname(path), '%s_syms.h' % pre), 'w').write('\n'.join(H))


HEADER = """/*
 * ix_data.h — images mémoire des binaires Amiga pour le moteur IMAGEXCEL
 * (générées par tools/ix_scripts.py dans game/data/).
 */
#ifndef IX_DATA_H
#define IX_DATA_H

#include <stddef.h>
#include <stdint.h>

#define IX_VM_BASE 0x00100000u   /* adresse virtuelle du premier hunk */

enum { IX_HUNK_CODE, IX_HUNK_DATA, IX_HUNK_BSS };

/* Relocation : le mot long big-endian à `offset` (dans le hunk) est un
 * offset dans le hunk `target` ; ix_vm_load() y ajoute l'adresse virtuelle
 * du hunk cible. */
typedef struct {
    uint32_t offset;
    uint16_t target;
} IxReloc;

typedef struct {
    int             type;        /* IX_HUNK_CODE / DATA / BSS            */
    uint32_t        va;          /* adresse virtuelle                     */
    uint32_t        size;        /* taille allouée (BSS compris)          */
    const uint8_t  *data;        /* contenu initial (NULL pour un BSS)    */
    uint32_t        data_size;
    const IxReloc  *relocs;
    int             reloc_count;
} IxHunk;

typedef struct {
    const char      *name;
    const IxHunk    *hunks;
    int              hunk_count;
    uint32_t         total_size; /* octets à partir de IX_VM_BASE         */
    const uint32_t  *scripts;    /* adresses des scripts identifiés       */
    int              script_count;
} IxImage;

extern const IxImage ix_program_image;
extern const IxImage ix_mog_image;

#endif /* IX_DATA_H */
"""


def main():
    names = sys.argv[1:] or ['program', 'mog']
    open(os.path.join(ROOT, 'game', 'include', 'ix_data.h'), 'w').write(HEADER)
    for n in names:
        b = Binary(n)
        scripts, rejected = identify(b)
        role, tables, tab_role = roles(b, scripts)
        write_markdown(b, scripts, rejected, role, tables, tab_role,
                       os.path.join(ROOT, 'docs', 'scripts', '%s_scripts.md' % n))
        write_c(b, scripts, tables, os.path.join(ROOT, 'game', 'data', 'ix_%s.c' % n))
        unrole = sum(1 for k in scripts if not role.get(k))
        print('%s : %d scripts, %d tables, %d candidats rejetés, %d scripts sans rôle'
              % (n, len(scripts), len(tables), len(rejected), unrole))
        reasons = collections.Counter(re.sub(r'\$[0-9A-F]+', '$…', r)
                                      for r in rejected.values())
        for r, c in reasons.most_common(8):
            print('   rejet : %4d  %s' % (c, r))


if __name__ == '__main__':
    main()
