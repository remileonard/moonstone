#!/usr/bin/env python3
"""
m68kdis.py — analyse code/données d'un exécutable Amiga « hunk » (68000)
par descente récursive guidée par les relocations.

Ce module ne produit pas de source : il fournit la carte code/données
utilisée par ira_hints.py pour piloter IRA.

Principes
---------
1. Le binaire est la seule source de vérité.  Les relocations HUNK_RELOC32
   sont des preuves : chaque mot long relogé est un pointeur vers
   (hunk cible, offset).
2. Le code est ce qui est *atteint* :
     - CERTAIN  : atteint par le flot d'exécution depuis l'entrée
                  (hunk 0, offset 0) : branchements, BSR/JSR/JMP directs,
                  sauts calculés JMP d8(PC,Dn) ;
     - PROBABLE : cible d'un pointeur (#LAB, LEA, PEA, DC.L relogé, vecteur
                  d'interruption...) validée par un décodage spéculatif
                  strict (voir `_explore`) ;
     - HEURISTIQUE (option) : bloc non atteint mais décodable proprement.
3. Une zone lue/écrite par une instruction de donnée (MOVE, TST, CMP, ADD...)
   depuis du code est une DONNÉE PROUVÉE : le décodage spéculatif ne peut pas
   la traverser.  Si elle tombe dans une instruction CERTAINE, c'est du code
   auto-modifiant (SMC) : on le signale.
4. Le décodage spéculatif est rejeté sur : opcode invalide, mot nul ($0000 =
   ORI.B #0,D0), relocation qui ne correspond à aucun opérande, chevauchement
   d'instructions, donnée prouvée, texte ASCII, sortie du hunk.
5. ORI.W #$8xxx,SR (activation du mode trace, protection Copylock) arrête
   l'exploration : le code qui suit est chiffré.
"""

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
        self.trace_on = set()     # (h, off) des ORI #$8xxx,SR (mode trace)
        self.root_of = {}         # (h, off) instruction -> racine qui l'a atteinte

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
        if off < 0 or off + 2 > len(d):
            return None, 'hors du hunk'
        try:
            ins = next(MD.disasm(d[off:off + 10], off, 1))
        except StopIteration:
            return None, 'opcode invalide $%04X' % struct.unpack('>H', d[off:off + 2])[0]
        opword = struct.unpack('>H', d[off:off + 2])[0]
        if ins.id == 0:
            return None, 'opcode invalide $%04X' % opword
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
        # ORI.W #$8xxx,SR : active le mode TRACE -> ce qui suit est exécuté
        # sous le contrôle d'un handler de trace (Rob Northen Copylock : code
        # chiffré, déchiffré instruction par instruction). Indécodable
        # statiquement : on s'arrête.
        m = re.match(r'#\$([0-9a-f]+), sr$', ins.op_str)
        if mn == 'ori' and m and int(m.group(1), 16) & 0x8000:
            I.flow = 'stop'
            I.targets = []
            self.trace_on.add((h, I.off))

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
        if strict:
            head = self.data(start[0])[start[1]:start[1] + 6]
            if len(head) == 6 and all(32 <= c < 127 for c in head):
                return None, 'texte ASCII %r' % head.decode('latin-1')
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
        # forme 2 : saut calculé dans du code déroulé (Duff's device) —
        # la base est elle-même du code (validé par le décodage qui suit)
        if (h, base) not in self.proven_data:
            if not strict:
                self.jump_tables.append((h, off, base, 0, 'code déroulé'))
            return [(h, base)]
        if not strict:
            self.anomalies.append((h, off, 'table de sauts non résolue en $%X' % base))
        return None

    def commit(self, found, level, root=None):
        for k in found:
            self.root_of.setdefault(k, root)
        known = {(h, b) for (h, o, b, n, t) in self.jump_tables}
        for k, I in found.items():
            for t in I.targets:
                if t[0] == 'table' and (k[0], t[1]) in found and (k[0], t[1]) not in known:
                    self.jump_tables.append((k[0], k[1], t[1], 0, 'code déroulé'))
                    known.add((k[0], t[1]))
            self.code[k] = I
            self.level.setdefault(k, level)
            for b in range(I.off, I.off + I.size):
                self.owner[(I.h, b)] = k
            for (th, to, sz, wr) in I.data_refs:
                if 0 <= th < len(self.hunks):
                    for b in range(to, to + sz):
                        self.proven_data.setdefault(
                            (th, b), '%s en %d:$%X' % (I.mn.upper(), I.h, I.off))

    def drop_root(self, root):
        """Oublie le code atteint depuis `root` (bloc rejeté)."""
        for k in [k for k, r in self.root_of.items() if r == root]:
            I = self.code.pop(k, None)
            self.level.pop(k, None)
            self.root_of.pop(k, None)
            if I:
                for b in range(I.off, I.off + I.size):
                    if self.owner.get((I.h, b)) == k:
                        del self.owner[(I.h, b)]
        for lvl in self.roots.values():
            if root in lvl:
                lvl.remove(root)

    def check_smc(self):
        for (h, b), why in self.proven_data.items():
            o = self.owner.get((h, b))
            if o and self.level.get(o) == 'CERTAIN':
                self.smc.append((h, b, o, why))

    # -- pipeline ---------------------------------------------------------
    def run(self, extra_labels=(), heuristic=True):
        # 1) CERTAIN : depuis l'entrée
        found, _ = self._explore((0, 0), strict=False)
        self.commit(found, 'CERTAIN')

        # 2) PROBABLE : pointeurs vers des hunks CODE, jusqu'au point fixe
        self.rejected = {}
        self.roots = {'PROBABLE': [], 'HEURISTIQUE': []}
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
                self.commit(f, 'PROBABLE', c)
                self.roots['PROBABLE'].append(c)
                changed = True

        # 3) HEURISTIQUE : octets non atteints des hunks CODE.  On essaie
        #    chaque mot pair d'un trou, dans l'ordre ; un bloc est accepté
        #    s'il passe le décodage strict et compte au moins 3 instructions.
        #    (Couvre le code mort et le code appelé via une adresse calculée.)
        self.gap_rejected = {}
        if not heuristic:
            self.check_smc()
            return
        for hi, hk in enumerate(self.hunks):
            if hk['type'] != 'CODE':
                continue
            gap_start = None
            gap_is_data = False
            for off in range(0, len(hk['data']), 2):
                c = (hi, off)
                if c in self.owner:
                    gap_start = None
                    continue
                if gap_start is None:
                    gap_start = c
                    gap_is_data = False
                # une cible de pointeur rejetée (table, variable référencée
                # par adresse) : le reste du trou est traité comme données
                if c in self.rejected or (hi, off + 1) in self.rejected:
                    gap_is_data = True
                if gap_is_data:
                    continue
                f, why = self._explore(c, strict=True, max_insn=5000)
                if f is None or len(f) < 3:
                    self.gap_rejected.setdefault(gap_start, why or 'bloc trop court')
                    continue
                self.gap_rejected.pop(gap_start, None)
                self.commit(f, 'HEURISTIQUE', c)
                self.roots['HEURISTIQUE'].append(c)
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
