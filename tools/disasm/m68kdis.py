#!/usr/bin/env python3
"""
m68kdis.py — désassembleur 68000 par descente récursive pour exécutables
Amiga « hunk », guidé par les relocations.

Objectif : produire un source vasm (syntaxe Motorola) dont la séparation
code / données est *justifiée* et dont le réassemblage redonne le binaire
à l'identique (contenu + relocations de chaque hunk).

Principes
---------
1. Le binaire est la seule source de vérité.  Les relocations HUNK_RELOC32
   sont des preuves : chaque mot long relogé est un pointeur vers
   (hunk cible, offset).
2. Le code est ce qui est *atteint* :
     - CERTAIN  : atteint par le flot d'exécution depuis l'entrée
                  (hunk 0, offset 0) — branchements, BSR/JSR/JMP directs,
                  tables de sauts reconnues ;
     - PROBABLE : cible d'un pointeur (#LAB, LEA, PEA, DC.L relogé, vecteur
                  d'interruption…) ou bloc non atteint, validé par un décodage
                  spéculatif strict (voir `_explore`).
   Tout le reste d'un hunk CODE est émis en données (DC.x).
3. Une zone lue/écrite par une instruction de donnée (MOVE, TST, CMP, ADD…)
   depuis du code est une DONNÉE PROUVÉE : le décodage spéculatif ne peut pas
   la traverser.  Si elle tombe dans une instruction CERTAINE, c'est du code
   auto-modifiant (SMC) : on le signale.
4. Les noms de labels d'un désassemblage existant (IRA) peuvent être
   réimportés depuis un listing vasm (-L) pour garder les références des
   documents (LAB_xxxx).

Usage
-----
  m68kdis.py BINAIRE -o SORTIE.asm [--ira ANCIEN.asm --vasm VASM]
             [--report RAPPORT.md]
"""

import argparse
import collections
import re
import struct
import subprocess
import sys
import os
import tempfile

import capstone as cs

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import hunk as hunkmod  # noqa: E402

# ---------------------------------------------------------------------------
# Capstone
# ---------------------------------------------------------------------------

MD = cs.Cs(cs.CS_ARCH_M68K, cs.CS_MODE_M68K_000 | cs.CS_MODE_BIG_ENDIAN)
MD.detail = True

AM_ABS_W, AM_ABS_L, AM_IMM = 16, 17, 18
AM_PCI_DISP, AM_PCI_IDX = 11, 12
AM_BR = 19
OP_MEM, OP_IMM, OP_BR = 3, 2, 8

TERMINATORS = {'rts', 'rte', 'rtr', 'jmp', 'bra', 'stop', 'illegal'}
COND_BRANCH = re.compile(r'^(b(hi|ls|cc|cs|ne|eq|vc|vs|pl|mi|ge|lt|gt|le|hs|lo)'
                         r'|db\w+|dbra)$')
# instructions dont l'opérande mémoire n'est pas un accès donnée
ADDR_ONLY = {'lea', 'pea', 'jsr', 'jmp'}

# Opcodes « impossibles » dans du code normal d'un jeu : deux mots nuls
# (ORI.B #0,D0) sont presque toujours des données.
def _suspicious(opword, ins):
    if opword == 0x0000:
        return 'mot nul ($0000) décodé comme ORI.B #0'
    if ins.mnemonic.startswith('ori') and ins.op_str.endswith(('ccr', 'sr')) is False \
            and ins.op_str.startswith('#$0,'):
        return 'ORI #0 (sans effet)'
    return None


# ---------------------------------------------------------------------------
# Modèle
# ---------------------------------------------------------------------------

class Insn:
    __slots__ = ('h', 'off', 'size', 'mn', 'ops', 'ins', 'targets', 'flow',
                 'relocs', 'data_refs', 'addr_refs')

    def __init__(self, h, off, ins):
        self.h, self.off, self.size = h, off, ins.size
        self.mn, self.ops, self.ins = ins.mnemonic, ins.op_str, ins
        self.targets = []     # (h, off) successeurs de flot
        self.flow = 'next'    # next | stop
        self.relocs = {}      # position absolue dans le hunk -> (h, off)
        self.data_refs = []   # (h, off, taille, écriture?)
        self.addr_refs = []   # (h, off) adresse prise (#LAB, LEA, PEA)


class Program:
    def __init__(self, path):
        self.path = path
        self.hunks = hunkmod.parse(path)
        for h in self.hunks:
            # tri des relocs par position
            h['rel'] = dict(sorted(h['relocs'].items()))
        self.code = {}          # (h, off) -> Insn (tout code accepté)
        self.owner = {}         # (h, byte) -> (h, off) instruction propriétaire
        self.level = {}         # (h, off) -> 'CERTAIN' | 'PROBABLE'
        self.proven_data = {}   # (h, byte) -> description
        self.smc = []
        self.anomalies = []     # (h, off, texte)
        self.labels = {}        # (h, off) -> nom
        self.jump_tables = []

    # -- utilitaires -------------------------------------------------------
    def data(self, h):
        return self.hunks[h]['data']

    def is_code_hunk(self, h):
        return self.hunks[h]['type'] == 'CODE'

    def reloc_target(self, h, pos):
        """Si un reloc32 est à `pos` dans le hunk h, renvoie (hcible, off)."""
        t = self.hunks[h]['rel'].get(pos)
        if t is None:
            return None
        return (t, struct.unpack('>I', self.data(h)[pos:pos + 4])[0])

    # -- décodage d'une instruction ---------------------------------------
    def decode(self, h, off):
        """Décode l'instruction en (h, off). Renvoie (Insn, erreur)."""
        d = self.data(h)
        if off & 1:
            return None, 'adresse impaire'
        if off + 2 > len(d):
            return None, 'hors du hunk'
        try:
            ins = next(MD.disasm(d[off:off + 10], off, 1))
        except StopIteration:
            return None, 'opcode invalide $%04X' % struct.unpack('>H', d[off:off + 2])[0]
        opword = struct.unpack('>H', d[off:off + 2])[0]
        if off + ins.size > len(d):
            return None, 'instruction tronquée en fin de hunk'
        I = Insn(h, off, ins)
        sus = _suspicious(opword, ins)
        if sus:
            return I, sus
        if ins.mnemonic == 'illegal':
            return I, 'ILLEGAL'
        # relocations comprises dans l'instruction
        rel_in = [p for p in range(off, off + ins.size)
                  if p in self.hunks[h]['rel']]
        for p in rel_in:
            if p + 4 > off + ins.size or p < off + 2:
                return I, 'relocation à cheval sur l\'instruction (+%d)' % (p - off)
            tgt = self.reloc_target(h, p)
            I.relocs[p] = tgt
        # chaque reloc doit correspondre à un opérande de même valeur
        rel_vals = collections.Counter(v[1] for v in I.relocs.values())
        op_long = collections.Counter(
            int(x, 16) for x in re.findall(r'\$([0-9a-f]+)\.l', ins.op_str))
        if ins.mnemonic.endswith('.l') or ins.mnemonic in ('pea', 'lea'):
            op_long.update(int(x, 16) for x in re.findall(r'#\$([0-9a-f]+)', ins.op_str))
        for v, n in rel_vals.items():
            if op_long[v] < n:
                return I, 'relocation sans opérande correspondant'
        return I, None

    def analyse_insn(self, I):
        """Calcule successeurs, références données et adresses prises."""
        ins, h = I.ins, I.h
        mn = ins.mnemonic.split('.')[0]
        abs_l = [int(x, 16) for x in re.findall(r'\$([0-9a-f]+)\.l', ins.op_str)]
        rel_by_val = collections.defaultdict(list)
        for p, t in I.relocs.items():
            rel_by_val[t[1]].append(t)

        def tgt_of_abs(v):
            lst = rel_by_val.get(v)
            return lst[0] if lst else None

        size = {'b': 1, 'w': 2, 'l': 4}.get(ins.mnemonic.rsplit('.', 1)[-1], 2) \
            if '.' in ins.mnemonic else 2

        # --- flot
        if mn in ('bra', 'bsr') or COND_BRANCH.match(mn):
            br = [o for o in ins.operands if o.type == OP_BR]
            if br:
                I.targets.append((h, br[0].br_disp.disp + I.off + 2))
            if mn == 'bra':
                I.flow = 'stop'
        elif mn in ('jsr', 'jmp'):
            o = ins.operands[0]
            if o.address_mode == AM_ABS_L:
                t = tgt_of_abs(abs_l[0]) if abs_l else None
                if t:
                    I.targets.append(t)
                else:
                    I.targets.append(('ext', abs_l[0] if abs_l else None))
            elif o.address_mode == AM_PCI_DISP:
                I.targets.append((h, o.mem.disp + I.off + 2))
            elif o.address_mode == AM_PCI_IDX:
                I.targets.append(('table', o.mem.disp + I.off + 2))
            else:
                I.targets.append(('indirect', None))
            if mn == 'jmp':
                I.flow = 'stop'
        elif mn in TERMINATORS:
            I.flow = 'stop'

        # --- références mémoire (hors flot)
        if mn not in ('jsr', 'jmp'):
            write_ops = {len(ins.operands) - 1} if len(ins.operands) > 1 else set()
            if mn in ('clr', 'neg', 'not', 'tas', 'scc') or mn.startswith('s') and len(ins.operands) == 1:
                write_ops = {0}
            ai = 0
            for idx, o in enumerate(ins.operands):
                if o.type == OP_MEM and o.address_mode == AM_ABS_L:
                    v = abs_l[ai] if ai < len(abs_l) else None
                    ai += 1
                    t = tgt_of_abs(v)
                    if t is None:
                        continue
                    if mn in ADDR_ONLY:
                        I.addr_refs.append(t)
                    else:
                        I.data_refs.append((t[0], t[1], size, idx in write_ops))
                elif o.type == OP_MEM and o.address_mode == AM_PCI_DISP:
                    t = (h, o.mem.disp + I.off + 2)
                    if mn in ADDR_ONLY:
                        I.addr_refs.append(t)
                    else:
                        I.data_refs.append((t[0], t[1], size, False))
                elif o.type == OP_MEM and o.address_mode == AM_PCI_IDX:
                    I.addr_refs.append((h, o.mem.disp + I.off + 2))
                elif o.type == OP_IMM and I.relocs:
                    t = tgt_of_abs(o.imm & 0xFFFFFFFF)
                    if t:
                        I.addr_refs.append(t)

    # -- exploration ------------------------------------------------------
    def _explore(self, start, strict, max_insn=20000):
        """Décode tout ce qui est atteignable depuis `start`.

        strict=True  : décodage spéculatif ; renvoie (dict, None) si valide,
                       (None, raison) sinon — rien n'est enregistré.
        strict=False : décodage certain ; les erreurs sont notées comme
                       anomalies et l'exploration continue ailleurs.
        """
        found = {}
        owner = {}
        work = [start]
        n = 0
        while work:
            h, off = work.pop()
            if (h, off) in found or (h, off) in self.code:
                continue
            if not (0 <= h < len(self.hunks)) or not self.is_code_hunk(h):
                if strict:
                    return None, 'saut vers un hunk non CODE (%s)' % (h,)
                self.anomalies.append((h, off, 'saut vers un hunk non CODE'))
                continue
            k = (h, off)
            clash = owner.get(k) or self.owner.get(k)
            if clash and clash != k:
                msg = 'chevauche l\'instruction en %s:$%X' % (clash[0], clash[1])
                if strict:
                    return None, msg
                self.anomalies.append((h, off, msg))
                continue
            I, err = self.decode(h, off)
            if err:
                if strict:
                    return None, '$%X: %s' % (off, err)
                self.anomalies.append((h, off, err))
                continue
            for b in range(off, off + I.size):
                if b != off and ((h, b) in self.code or (h, b) in found):
                    msg = 'l\'instruction en $%X recouvre un début d\'instruction en $%X' % (off, b)
                    if strict:
                        return None, msg
                    self.anomalies.append((h, off, msg))
                if strict and (h, b) in self.proven_data:
                    return None, '$%X recouvre une donnée prouvée (%s)' % (
                        off, self.proven_data[(h, b)])
            n += 1
            if n > max_insn:
                return (None, 'trop long') if strict else (found, None)
            self.analyse_insn(I)
            found[k] = I
            for b in range(off, off + I.size):
                owner[(h, b)] = k
            if I.flow == 'next':
                work.append((h, off + I.size))
            for t in I.targets:
                if t[0] in ('ext', 'indirect'):
                    continue
                if t[0] == 'table':
                    tb = self._jump_table(h, off, t[1], strict)
                    if tb is None:
                        continue
                    work.extend(tb)
                    continue
                work.append(t)
        return found, None

    def _jump_table(self, h, off, base, strict):
        """Table de sauts JMP d8(PC,Dn): entrées BRA.W/JMP ou offsets .W."""
        d = self.data(h)
        tgts = []
        # forme 1 : table de BRA.W / JMP abs à base
        p = base
        while p + 4 <= len(d):
            w = struct.unpack('>H', d[p:p + 2])[0]
            if w == 0x6000:
                disp = struct.unpack('>h', d[p + 2:p + 4])[0]
                tgts.append((h, p + 2 + disp)); p += 4
            elif w == 0x4EF9 and self.reloc_target(h, p + 2):
                tgts.append(self.reloc_target(h, p + 2)); p += 6
            else:
                break
        if tgts:
            if not strict:
                self.jump_tables.append((h, off, base, len(tgts), 'BRA/JMP'))
            tgts.append((h, base))
            return tgts
        if not strict:
            self.anomalies.append((h, off, 'table de sauts non résolue en $%X' % base))
        return None

    def commit(self, found, level):
        for k, I in found.items():
            self.code[k] = I
            self.level.setdefault(k, level)
            for b in range(I.off, I.off + I.size):
                self.owner[(I.h, b)] = k
            for (th, to, sz, wr) in I.data_refs:
                if 0 <= th < len(self.hunks):
                    for b in range(to, to + sz):
                        self.proven_data.setdefault(
                            (th, b), '%s en %d:$%X' % (I.mn.upper(), I.h, I.off))

    def check_smc(self):
        for (h, b), why in self.proven_data.items():
            o = self.owner.get((h, b))
            if o and self.level.get(o) == 'CERTAIN':
                self.smc.append((h, b, o, why))

    # -- pipeline ---------------------------------------------------------
    def run(self, extra_labels=()):
        # 1) CERTAIN : depuis l'entrée
        found, _ = self._explore((0, 0), strict=False)
        self.commit(found, 'CERTAIN')

        # 2) PROBABLE : pointeurs vers des hunks CODE, jusqu'au point fixe
        self.rejected = {}
        changed = True
        while changed:
            changed = False
            cands = self._pointer_candidates()
            for c in sorted(cands):
                if c in self.code or c in self.rejected:
                    continue
                f, why = self._explore(c, strict=True)
                if f is None:
                    self.rejected[c] = why
                    continue
                self.commit(f, 'PROBABLE')
                changed = True

        # 3) HEURISTIQUE : trous non atteints — début de trou et labels IRA
        gap_cands = set()
        for hi, hk in enumerate(self.hunks):
            if hk['type'] != 'CODE':
                continue
            prev_code = True
            for off in range(0, len(hk['data']), 2):
                if (hi, off) in self.owner:
                    prev_code = True
                    continue
                if prev_code:
                    gap_cands.add((hi, off))
                prev_code = False
        gap_cands |= {k for k in extra_labels
                      if self.is_code_hunk(k[0]) and k not in self.owner}
        self.gap_rejected = {}
        for c in sorted(gap_cands):
            if c in self.owner:
                continue
            f, why = self._explore(c, strict=True)
            if f is None:
                self.gap_rejected[c] = why
                continue
            # un bloc heuristique doit se terminer proprement (RTS/JMP/BRA/RTE)
            self.commit(f, 'HEURISTIQUE')
        self.check_smc()

    def _pointer_candidates(self):
        c = set()
        for I in self.code.values():
            for t in I.addr_refs:
                if self.is_code_hunk(t[0]) and not (t[1] & 1):
                    c.add(t)
            for t in I.targets:
                if isinstance(t[0], int) and self.is_code_hunk(t[0]):
                    c.add(t)
        # relocs situés en dehors du code (tables DC.L, hunks DATA)
        for hi, hk in enumerate(self.hunks):
            for p in hk['rel']:
                if (hi, p) in self.owner:
                    continue
                t = self.reloc_target(hi, p)
                if self.is_code_hunk(t[0]) and not (t[1] & 1) and \
                        (t[0], t[1]) not in self.proven_data:
                    c.add(t)
        return c


# ---------------------------------------------------------------------------
# Import des labels d'un désassemblage existant (listing vasm)
# ---------------------------------------------------------------------------

def import_labels(vasm, asm_path):
    """Assemble l'ancien source avec listing ; renvoie
    (labels {(h,off): nom}, en-tête EQU, insn_ira {(h,off)})."""
    with tempfile.TemporaryDirectory() as tmp:
        lst = os.path.join(tmp, 'o.lst')
        subprocess.run([vasm, '-quiet', '-Fhunkexe', '-nosym', '-no-opt', '-m68000',
                        '-L', lst, '-Lnf', '-o', os.devnull, os.path.abspath(asm_path)],
                       cwd=os.path.dirname(os.path.abspath(asm_path)),
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=True)
        text = open(lst, encoding='latin-1').read().split('\n')
    src = open(asm_path, encoding='latin-1').read().split('\n')
    labels, insn_ira = {}, set()
    pending = []
    for line in text:
        m = re.match(r'^([0-9A-F]{2}):([0-9A-F]{8}) (\S*)\s*\t\s*(\d+): (.*)$', line)
        if m:
            h, off = int(m.group(1), 16), int(m.group(2), 16)
            body = m.group(5).split(';')[0].strip()
            for name in pending:
                labels[(h, off)] = labels.get((h, off)) or name
            pending = []
            if body and not body.upper().startswith(('DC.', 'DS.')):
                insn_ira.add((h, off))
            continue
        m = re.match(r'^\s+\t\s*(\d+): (\w+):', line)
        if m:
            pending.append(m.group(2))
    header = []
    for l in src:
        if re.match(r'\s*SECTION', l):
            break
        header.append(l)
    return labels, header, insn_ira


# ---------------------------------------------------------------------------
# Génération du source
# ---------------------------------------------------------------------------

class Emitter:
    def __init__(self, prog, labels, header, equ_by_val):
        self.p = prog
        self.labels = dict(labels)       # (h, off) -> nom
        self.header = header
        self.equ = equ_by_val            # valeur -> nom (EQU absolues)
        self.used = set()

    def name(self, h, off):
        """Nom d'une adresse (h, off) ; crée un label si besoin."""
        k = (h, off)
        if k in self.labels:
            return self.labels[k]
        o = self.p.owner.get(k)
        if o and o != k:                 # milieu d'une instruction
            return '%s+%d' % (self.name(*o), off - o[1])
        # base = label précédent dans le même hunk, pour garder la parenté
        prev = [x for x in self.labels if x[0] == h and x[1] < off]
        base = self.labels[max(prev)] if prev else 'H%02d' % h
        nm = '%s_%X' % (base, off)
        self.labels[k] = nm
        return nm

    def fmt_insn(self, I):
        ins = I.ins
        mn = ins.mnemonic
        ops = ins.op_str
        h = I.h
        rel_by_val = collections.defaultdict(list)
        for p, t in I.relocs.items():
            rel_by_val[t[1]].append(t)
        base = mn.split('.')[0]

        if base == 'illegal':
            return 'ILLEGAL'
        if base in ('btst', 'bset', 'bclr', 'bchg', 'swap', 'exg', 'unlk', 'link',
                    'scc', 'scs', 'seq', 'sne', 'st', 'sf', 'shi', 'sls', 'spl', 'smi',
                    'sge', 'slt', 'sgt', 'sle', 'svc', 'svs', 'tas', 'nbcd', 'trap'):
            mn = base
        if base == 'moveq':
            v = int(re.match(r'#\$([0-9a-f]+)', ops).group(1), 16)
            v = v - 256 if v > 127 else v
            ops = re.sub(r'^#\$[0-9a-f]+', '#%d' % v, ops)

        # branchements : cible en label, taille explicite
        if base in ('bra', 'bsr') or COND_BRANCH.match(base):
            t = I.targets[0]
            if base.startswith('db'):
                reg = ops.split(',')[0].strip()
                mn = 'dbf' if base == 'dbra' else base
                return '%s\t%s,%s' % (mn.upper(), reg.upper(), self.name(*t))
            sz = '.S' if I.size == 2 else '.W'
            return '%s%s\t%s' % (base.upper(), sz, self.name(*t))

        def repl_abs(m):
            v = int(m.group(1), 16)
            ext = m.group(2)
            lst = rel_by_val.get(v)
            if ext == 'l' and lst:
                return self.name(*lst[0])
            nm = self.equ.get(v)
            if nm:
                return '%s.%s' % (nm, ext.upper())
            return '($%X).%s' % (v, ext.upper())
        ops = re.sub(r'\$([0-9a-f]+)\.([wl])', repl_abs, ops)

        def repl_pc(m):
            tgt = int(m.group(1), 16)
            rest = m.group(2)
            if 0 <= tgt <= len(self.p.data(h)):
                return '%s(PC%s)' % (self.name(h, tgt), rest.upper().replace(' ', ''))
            return '%d(PC%s)' % (tgt - (I.off + 2), rest.upper())
        ops = re.sub(r'(?<![\w$])\$?([0-9a-f]+)\(pc((?:,\s*[ad]\d\.[wl])?)\)', repl_pc, ops)

        def repl_imm(m):
            v = int(m.group(1), 16)
            lst = rel_by_val.get(v)
            if lst and mn.endswith('.l'):
                return '#' + self.name(*lst[0])
            return '#$%X' % v
        ops = re.sub(r'#\$([0-9a-f]+)', repl_imm, ops)
        ops = re.sub(r'\(([^)]*)\)', lambda m: '(' + m.group(1).replace(' ', '') + ')', ops)
        ops = ops.replace(', ', ',')
        ops = re.sub(r'\b([ad][0-7]|sp|pc|sr|ccr|usp)\b',
                     lambda m: m.group(1).upper(), ops)
        ops = re.sub(r'\.([wl])\)', lambda m: '.' + m.group(1).upper() + ')', ops)
        ops = re.sub(r'(?<=[\w)])\.([wl])\b', lambda m: '.' + m.group(1).upper(), ops)
        if base == 'dbra':
            mn = 'dbf'
        return ('%s\t%s' % (mn.upper(), ops)).rstrip()

    def emit(self, forced_raw=frozenset()):
        p = self.p
        out = list(self.header)
        # pré-passe : noms nécessaires (créés pendant la génération)
        lines_by_hunk = []
        for hi, hk in enumerate(p.hunks):
            lines = []
            d = hk['data']
            size = hk['size']
            flag = ',' + hk['mem'] if hk['mem'] else ''
            lines.append(('cmt', '\tSECTION S_%d,%s%s' % (hi, hk['type'], flag), None))
            if hk['type'] == 'BSS':
                lines.append(('bss', None, (hi, size)))
                lines_by_hunk.append(lines)
                continue
            off = 0
            cur_level = None
            while off < len(d):
                k = (hi, off)
                I = p.code.get(k)
                if I is not None and k not in forced_raw:
                    lvl = p.level[k]
                    if lvl != cur_level and lvl != 'CERTAIN':
                        lines.append(('cmt', '; --- code %s ---' % lvl, None))
                    elif lvl != cur_level and cur_level is not None:
                        lines.append(('cmt', '; --- code CERTAIN ---', None))
                    cur_level = lvl
                    lines.append(('insn', I, k))
                    off += I.size
                    continue
                if I is not None:  # instruction forcée en DC.W
                    lines.append(('raw', I, k))
                    off += I.size
                    continue
                if cur_level is not None:
                    lines.append(('cmt', '; --- données ---', None))
                    cur_level = None
                # bloc de données jusqu'au prochain code
                end = off
                while end < len(d) and (hi, end) not in p.code:
                    end += 1
                lines.append(('data', None, (hi, off, end)))
                off = end
            if size > len(d):
                lines.append(('bss', None, (hi, size - len(d), len(d))))
            lines_by_hunk.append(lines)

        # rendu (les appels à name() peuvent créer des labels : deux passes)
        nlab = -1
        while nlab != len(self.labels):
            nlab = len(self.labels)
            rendered = []
            for lines in lines_by_hunk:
                for kind, a, b in lines:
                    if kind == 'insn':
                        rendered.append((b, '\t' + self.fmt_insn(a)))
                    elif kind == 'raw':
                        words = [a.ins.bytes[i:i + 2] for i in range(0, a.size, 2)]
                        rendered.append((b, '\tDC.W\t' + ','.join(
                            '$%04X' % struct.unpack('>H', w)[0] for w in words) +
                            '\t; ' + self.fmt_insn(a).replace('\t', ' ')))
                    elif kind == 'data':
                        rendered.extend(self.fmt_data(*b))
                    elif kind == 'bss':
                        rendered.extend(self.fmt_bss(*b))
                    else:
                        rendered.append((None, a if kind == 'cmt' else b))
        # labels
        final = []
        self.line_keys = [None] * len(out)
        placed = set()
        for key, text in rendered:
            if key is not None and key in self.labels and key not in placed:
                final.append(self.labels[key] + ':')
                self.line_keys.append(None)
                placed.add(key)
            final.append(text)
            self.line_keys.append(key)
        # labels non placés (milieu d'instruction / hors limites) -> EQU
        for key, nm in sorted(self.labels.items()):
            if key in placed:
                continue
            o = self.p.owner.get(key)
            if o and o != key and o in self.labels:
                final.append('%s\tEQU\t%s+%d' % (nm, self.labels[o], key[1] - o[1]))
            else:
                final.append('%s\tEQU\t%s+%d' % (nm, 'SECSTRT_%d' % key[0], key[1]))
        return out + final + ['\tEND', '']

    def fmt_data(self, h, start, end):
        p = self.p
        d = p.data(h)
        rel = p.hunks[h]['rel']
        res = []
        # points de coupe : labels connus, relocs, cibles de données
        off = start
        chunk = []

        def flush():
            nonlocal chunk
            if not chunk:
                return
            o0 = chunk[0][0]
            bs = bytes(c[1] for c in chunk)
            # texte ?
            printable = all(32 <= c < 127 or c in (0, 10, 13) for c in bs)
            if len(bs) >= 4 and printable and sum(32 <= c < 127 for c in bs) >= len(bs) * 0.75:
                parts, s = [], ''
                for c in bs:
                    if 32 <= c < 127 and c != 39:
                        s += chr(c)
                    else:
                        if s:
                            parts.append("'%s'" % s); s = ''
                        parts.append('$%02X' % c)
                if s:
                    parts.append("'%s'" % s)
                res.append(((h, o0), '\tDC.B\t' + ','.join(parts)))
            else:
                i = 0
                first = True
                while i < len(bs):
                    o = o0 + i
                    if (o & 1) == 0 and len(bs) - i >= 2:
                        n = min(8, (len(bs) - i) // 2)
                        ws = ['$%04X' % struct.unpack('>H', bs[i + 2 * j:i + 2 * j + 2])[0] for j in range(n)]
                        res.append(((h, o) if first else None, '\tDC.W\t' + ','.join(ws)))
                        i += 2 * n
                    else:
                        res.append(((h, o) if first else None, '\tDC.B\t$%02X' % bs[i]))
                        i += 1
                    first = False
            chunk = []

        while off < end:
            if off in rel and off + 4 <= end:
                flush()
                t = p.reloc_target(h, off)
                res.append(((h, off), '\tDC.L\t' + self.name(*t)))
                off += 4
                continue
            if (h, off) in self.labels and chunk:
                flush()
            chunk.append((off, d[off]))
            off += 1
        flush()
        return res

    def fmt_bss(self, h, n, base=0):
        cuts = sorted(o for (hh, o) in self.labels if hh == h and base <= o < base + n)
        res = []
        pts = [base] + [c for c in cuts if c > base] + [base + n]
        for a, b in zip(pts, pts[1:]):
            if b > a:
                res.append(((h, a), '\tDS.B\t%d' % (b - a)))
        return res


# ---------------------------------------------------------------------------
# Vérification (réassemblage)
# ---------------------------------------------------------------------------

def assemble(vasm, src, out, listing=None):
    cmd = [vasm, '-quiet', '-Fhunkexe', '-nosym', '-no-opt', '-m68000', '-o', out]
    if listing:
        cmd += ['-L', listing, '-Lnf']
    r = subprocess.run(cmd + [src], capture_output=True, text=True, errors='replace')
    return r.returncode, r.stdout + r.stderr


def mismatched_insns(prog, listing):
    """Compare octets du listing avec le binaire ; renvoie les (h,off) fautifs."""
    bad = set()
    for line in open(listing, encoding='latin-1'):
        m = re.match(r'^([0-9A-F]{2}):([0-9A-F]{8}) ([0-9A-F]+)\s*\t', line)
        if not m:
            continue
        h, off = int(m.group(1), 16), int(m.group(2), 16)
        b = bytes.fromhex(m.group(3))
        if h < len(prog.hunks) and (h, off) in prog.code:
            if prog.data(h)[off:off + len(b)] != b or len(b) != prog.code[(h, off)].size:
                bad.add((h, off))
    return bad


def error_lines(msg):
    return [int(x) for x in re.findall(r'error \d+ in line (\d+)', msg)]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('binary')
    ap.add_argument('-o', '--output', required=True)
    ap.add_argument('--ira', help='ancien source IRA (pour les noms de labels)')
    ap.add_argument('--vasm', default='vasmm68k_mot')
    ap.add_argument('--report')
    a = ap.parse_args()

    prog = Program(a.binary)
    labels, header, insn_ira = {}, [], set()
    if a.ira:
        labels, header, insn_ira = import_labels(a.vasm, a.ira)
    equ = {}
    for l in header:
        m = re.match(r'(\w+)\s+EQU\s+\$([0-9A-Fa-f]+)', l)
        if m:
            equ.setdefault(int(m.group(2), 16), m.group(1))
    # SECSTRT_n : début de chaque hunk
    for i in range(len(prog.hunks)):
        labels.setdefault((i, 0), 'SECSTRT_%d' % i)

    prog.run(extra_labels=[k for k in labels if k in insn_ira])

    forced = set()
    with tempfile.TemporaryDirectory() as tmp:
        for it in range(200):
            em = Emitter(prog, labels, header, equ)
            text = em.emit(forced)
            open(a.output, 'w', encoding='latin-1').write('\n'.join(text))
            lst = os.path.join(tmp, 'o.lst')
            obj = os.path.join(tmp, 'o.exe')
            rc, msg = assemble(a.vasm, a.output, obj, lst)
            if rc != 0:
                # instructions refusées par vasm -> DC.W
                bad_lines = error_lines(msg)
                if not bad_lines:
                    print(msg[:3000]); sys.exit(1)
                # retrouver les clés par numéro de ligne via le texte
                new = {em.line_keys[ln - 1] for ln in bad_lines
                       if ln - 1 < len(em.line_keys)} - {None}
                new &= set(prog.code)
                if not new:
                    print(msg[:3000]); sys.exit(1)
                forced |= new
                continue
            bad = mismatched_insns(prog, lst)
            if not bad:
                break
            forced |= bad
        ok = hunkmod.compare(a.binary, obj)
    print('%s : réassemblage %s (%d instructions forcées en DC.W)' % (
        a.output, 'IDENTIQUE' if ok else 'DIFFÉRENT', len(forced)))
    if a.report:
        write_report(a, prog, insn_ira, labels, forced, ok)
    sys.exit(0 if ok else 2)


def _keys_for_line(prog, em, text, ln):
    """Retrouve l'instruction (h,off) correspondant à la ligne ln du source."""
    # on relit le rendu : chaque instruction est précédée éventuellement d'un label
    idx = ln - 1
    target = text[idx].strip()
    # correspondance par recherche : premier Insn dont le rendu est identique
    for k, I in prog.code.items():
        try:
            if em.fmt_insn(I).strip() == target:
                return {k}
        except Exception:
            pass
    return set()


def write_report(a, prog, insn_ira, labels, forced, ok):
    L = []
    name = os.path.basename(a.binary)
    L.append('# Rapport de désassemblage — `%s`\n' % name)
    L.append('Réassemblage vasm : **%s**\n' % ('identique (contenu + relocations)' if ok else 'DIFFÉRENT'))
    L.append('| Hunk | Type | Taille | Code CERTAIN | PROBABLE | HEURISTIQUE | Données | IRA: code→données | IRA: données→code |')
    L.append('|---|---|---|---|---|---|---|---|---|')
    tot = collections.Counter()
    for hi, hk in enumerate(prog.hunks):
        if hk['type'] != 'CODE':
            continue
        n = len(hk['data'])
        by = collections.Counter()
        for off in range(n):
            o = prog.owner.get((hi, off))
            by[prog.level[o] if o else 'DATA'] += 1
        # comparaison IRA : par instruction de départ
        ira_code_now_data = sum(1 for (h, o) in insn_ira if h == hi and (h, o) not in prog.owner)
        ira_data_now_code = sum(1 for k in prog.code if k[0] == hi and k not in insn_ira)
        tot.update(by); tot['c2d'] += ira_code_now_data; tot['d2c'] += ira_data_now_code
        L.append('| %d | %s%s | %d | %d | %d | %d | %d | %d | %d |' % (
            hi, hk['type'], ',' + hk['mem'] if hk['mem'] else '', n, by['CERTAIN'],
            by['PROBABLE'], by['HEURISTIQUE'], by['DATA'], ira_code_now_data, ira_data_now_code))
    L.append('| **Total** | | | %d | %d | %d | %d | %d | %d |\n' % (
        tot['CERTAIN'], tot['PROBABLE'], tot['HEURISTIQUE'], tot['DATA'], tot['c2d'], tot['d2c']))
    L.append('Colonnes « IRA : » = nombre d\'instructions dont la classification change par rapport à l\'ancien source.\n')

    def lab(h, o):
        return labels.get((h, o), '%d:$%X' % (h, o))

    L.append('## Code auto-modifiant détecté (%d octets)\n' % len(prog.smc))
    seen = set()
    for h, b, o, why in sorted(prog.smc):
        if o in seen:
            continue
        seen.add(o)
        L.append('- instruction `%s` (%s) modifiée par %s' % (
            prog.code[o].mn.upper() + ' ' + prog.code[o].ops, lab(*o), why))
    L.append('\n## Anomalies dans le code CERTAIN (%d)\n' % len(prog.anomalies))
    for h, o, t in sorted(set((x[0] if isinstance(x[0], int) else -1, x[1] or 0, x[2]) for x in prog.anomalies))[:200]:
        L.append('- %s : %s' % (lab(h, o) if h >= 0 else str(o), t))
    L.append('\n## Tables de sauts reconnues (%d)\n' % len(prog.jump_tables))
    for h, o, b, n, k in prog.jump_tables:
        L.append('- %s → table %s, %d entrées (%s)' % (lab(h, o), lab(h, b), n, k))
    L.append('\n## Pointeurs vers du code rejetés (%d)\n' % len(prog.rejected))
    L.append('Cibles de pointeurs dans un hunk CODE dont le décodage spéculatif échoue : ce sont des **données** (ou du code atteint autrement, à vérifier).\n')
    for k, why in sorted(prog.rejected.items())[:300]:
        L.append('- %s : %s' % (lab(*k), why))
    L.append('\n## Instructions émises en DC.W (%d)\n' % len(forced))
    L.append('Encodages que vasm ne reproduit pas à l\'identique (encodage non canonique) : conservés en DC.W avec l\'instruction en commentaire.\n')
    for k in sorted(forced):
        I = prog.code[k]
        L.append('- %s : `%s %s`' % (lab(*k), I.mn, I.ops))
    open(a.report, 'w').write('\n'.join(L) + '\n')


if __name__ == '__main__':
    main()
