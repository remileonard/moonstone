#!/usr/bin/env python3
"""
prog_ref.py — `program` d'origine (intro, menu, fin de partie) exécuté par
l'émulateur 68000 Unicorn, comme tools/mog_ref.py pour mog : même mémoire
(hunks à X.VM_BASE, relocations appliquées), matériel remplacé par des
crochets, blitter émulé.

Crochets :
  fichiers  LAB_0390 / LAB_03B2 / LAB_03C5 / LAB_03DA (ouvrir, lire, sauter,
            fermer : même bibliothèque que mog, section S_18) ;
  disque    LAB_02A2 (lecture de pistes) : arrêt, ne doit plus servir ;
  VBL       LAB_0552 (attente d'une VBL) -> petit code 68000 : compteur
            LAB_0379 (si LAB_0363) et serveurs SECSTRT_15 (liste LAB_0372 :
            fondus, couleurs...), registres préservés ;
  boutons   CIAA_PRA à $FF (feu relâché), JOY0DAT / JOY1DAT à 0.

Image montrée : plans pointés par la copper list ($7F6B0, comme mog),
couleurs de la palette courante rl(LAB_05D2).

  python3 tools/prog_ref.py <dossier_données> <préfixe> [--vbls N] [--every K]
      (intro : SECSTRT_0 jusqu'au menu LAB_005B ; image PNG toutes les
       K VBL)
"""
import argparse
import os
import re
import struct
import sys
import zlib

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)

import unicorn as U  # noqa: E402
import unicorn.m68k_const as M  # noqa: E402

import ix_scripts as X  # noqa: E402
from mog_ref import Blitter, Stop, REGS, CHIP_BLOCK  # noqa: E402

FAST_SIZE = 0x00060000
STACK_SIZE = 0x4000
COPPER_BPL = 0x7F6B0


def syms():
    s = {}
    for line in open(os.path.join(ROOT, 'game', 'data', 'ix_program_syms.h')):
        m = re.match(r'#define PROGRAM_(\w+) 0x([0-9A-F]+)u', line)
        if m:
            s[m.group(1)] = int(m.group(2), 16)
    return s


def load_image():
    """Octets de program à X.VM_BASE, relocations appliquées."""
    b = X.Binary('program')
    bases, total = X.hunk_bases(b)
    m = bytearray(total)
    for h, hk in enumerate(b.hunks):
        o = bases[h] - X.VM_BASE
        m[o:o + len(hk['data'])] = hk['data']
    for h, hk in enumerate(b.hunks):
        for pos, t in b.rel[h].items():
            o = bases[h] - X.VM_BASE + pos
            v = struct.unpack('>I', m[o:o + 4])[0] + bases[t]
            m[o:o + 4] = struct.pack('>I', v & 0xFFFFFFFF)
    return m


def write_png(path, argb, w, h):
    raw = b''.join(b'\x00' + b''.join(struct.pack('>I', p)[1:] for p in argb[y * w:(y + 1) * w])
                   for y in range(h))

    def chunk(t, d):
        return struct.pack('>I', len(d)) + t + d + struct.pack('>I', zlib.crc32(t + d) & 0xFFFFFFFF)
    open(path, 'wb').write(b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', w, h, 8, 2, 0, 0, 0))
                           + chunk(b'IDAT', zlib.compress(raw)) + chunk(b'IEND', b''))


class ProgRef:
    def __init__(self, data_dir, log=None):
        self.data_dir = data_dir
        self.log = log or (lambda s: None)
        self.S = S = syms()
        img = load_image()
        self.end = X.VM_BASE + len(img)
        self.fast = (self.end + 0xFFF) & ~0xFFF
        self.stack_top = ((self.fast + FAST_SIZE + 0xFFF) & ~0xFFF) + STACK_SIZE
        uc = self.uc = U.Uc(U.UC_ARCH_M68K, U.UC_MODE_BIG_ENDIAN)
        uc.ctl_set_cpu_model(M.UC_CPU_M68K_M68000)
        uc.mem_map(0, self.stack_top + 0x1000)
        uc.mem_map(0xBFD000, 0x2000)
        uc.mem_map(0xDFF000, 0x1000)
        uc.mem_write(X.VM_BASE, bytes(img))
        uc.mem_write(0xBFE001, b'\xff')                 # CIAA_PRA : feu relâché
        self.hooks = {}
        self.patched = {}
        self.cur = None
        self.vbls = 0
        self.on_vbl = None
        self.hook(S['LAB_0390'], self.h_open)
        self.hook(S['LAB_03B2'], self.h_read)
        self.hook(S['LAB_03C5'], self.h_skip)
        self.hook(S['LAB_03DA'], self.h_close)
        self.hook(S['LAB_02A2'], self.h_disk)
        # VBL : code 68000 qui appelle les vrais serveurs
        stub = self.stack_top - 0x80
        code = (b'\x48\xe7\xff\xfe'                                     # MOVEM.L D0-D7/A0-A6,-(A7)
                + b'\x4a\x79' + struct.pack('>I', S['LAB_0363'])        # TST.W LAB_0363
                + b'\x67\x06'                                           # BEQ.S +6
                + b'\x52\xb9' + struct.pack('>I', S['LAB_0379'])        # ADDQ.L #1,LAB_0379
                + b'\x4e\xb9' + struct.pack('>I', S['SECSTRT_15'])      # JSR SECSTRT_15
                + b'\x4c\xdf\x7f\xff'                                   # MOVEM.L (A7)+,...
                + b'\x4e\x75')                                          # RTS
        uc.mem_write(stub, code)
        uc.mem_write(S['LAB_0552'], b'\x4e\xf9' + struct.pack('>I', stub))
        uc.hook_add(U.UC_HOOK_CODE, self.h_vbl, None, begin=S['LAB_0552'], end=S['LAB_0552'])
        self.blitter = Blitter(uc)
        uc.hook_add(U.UC_HOOK_MEM_WRITE, lambda uc, acc, a, sz, v, u: self.blitter.run(v & 0xFFFF)
                    if a == 0xDFF058 else None, None, begin=0xDFF058, end=0xDFF059)
        uc.hook_add(U.UC_HOOK_MEM_UNMAPPED, self.h_unmapped)

    # -- accès -------------------------------------------------------------
    def r(self, reg):
        return self.uc.reg_read(REGS[reg])

    def w(self, reg, v):
        self.uc.reg_write(REGS[reg], v & 0xFFFFFFFF)

    def rl(self, a):
        return struct.unpack('>I', self.uc.mem_read(a, 4))[0]

    def rw(self, a):
        return struct.unpack('>H', self.uc.mem_read(a, 2))[0]

    def wl(self, a, v):
        self.uc.mem_write(a, struct.pack('>I', v & 0xFFFFFFFF))

    def ww(self, a, v):
        self.uc.mem_write(a, struct.pack('>H', v & 0xFFFF))

    def where(self, pc):
        best = max((v for v in self.S.values() if v <= pc), default=0)
        name = [k for k, v in self.S.items() if v == best][0]
        return '%s+%X' % (name, pc - best) if pc != best else name

    def backtrace(self, depth=16):
        sp, out = self.r('A7'), []
        while sp < self.stack_top - 0x100 and len(out) < depth:
            v = self.rl(sp)
            if X.VM_BASE <= v < self.end and not v & 1:
                out.append(self.where(v))
            sp += 2
        return out

    def cstr(self, a, n=64):
        return bytes(self.uc.mem_read(a, n)).split(b'\0')[0].decode('latin-1')

    def hook(self, addr, fn):
        if addr not in self.patched:
            self.patched[addr] = bytes(self.uc.mem_read(addr, 2))
            self.uc.mem_write(addr, b'\x4e\x75')
            self.uc.hook_add(U.UC_HOOK_CODE, lambda uc, a, sz, u: self.hooks[a](),
                             None, begin=addr, end=addr)
        self.hooks[addr] = fn

    # -- crochets ----------------------------------------------------------
    def find_file(self, name):
        want = name.lower()
        for f in os.listdir(self.data_dir):
            if f.lower() == want:
                return os.path.join(self.data_dir, f)
        return None

    def h_open(self):                    # LAB_0390 : nom en A0
        name = self.cstr(self.r('A0'))
        path = self.find_file(name)
        if path is None:
            self.log('fichier absent : %s' % name)
            self.ww(self.S['L18_0001A'], 0xFFFF)
            self.cur = None
            return
        self.ww(self.S['L18_0001A'], 0)
        self.cur = [open(path, 'rb').read(), 0]
        self.wl(self.S['L18_0000E'], len(self.cur[0]))
        self.log('ouverture %s (%d octets)' % (name, len(self.cur[0])))

    def h_read(self):                    # LAB_03B2 : D0 octets -> A0
        n, dst = self.r('D0'), self.r('A0')
        if self.cur is None:
            self.w('D0', 0)
            return
        data, pos = self.cur
        chunk = data[pos:pos + n]
        self.uc.mem_write(dst, chunk)
        self.cur[1] = pos + len(chunk)
        self.w('D0', len(chunk))

    def h_skip(self):
        if self.cur is not None:
            self.cur[1] += self.r('D0')

    def h_close(self):
        self.cur = None

    def h_disk(self):
        raise Stop('lecture de piste (LAB_02A2) depuis %s' % ' < '.join(self.backtrace()))

    def h_vbl(self, uc, addr, size, user):
        self.vbls += 1
        if self.on_vbl:
            self.on_vbl(self)

    def h_unmapped(self, uc, access, addr, size, value, user):
        self.log('accès non mappé %08X (pc %s)' % (addr, self.where(uc.reg_read(M.UC_M68K_REG_PC))))
        if access == U.UC_MEM_READ_UNMAPPED:
            uc.mem_map(addr & ~0xFFF, 0x2000)
            return True
        return False

    # -- exécution ---------------------------------------------------------
    def call(self, addr, until=None, count=2_000_000_000, **regs):
        sentinel = self.stack_top - 0x10
        sp = self.stack_top - 0x100
        self.wl(sp, sentinel)
        self.w('A7', sp)
        for k, v in regs.items():
            self.w(k, v)
        h = None
        if until is not None:
            h = self.uc.hook_add(U.UC_HOOK_CODE, lambda uc, a, sz, u: uc.emu_stop(),
                                 None, begin=until, end=until)
        try:
            self.uc.emu_start(addr, sentinel, count=count)
        except U.UcError as ex:
            pc = self.uc.reg_read(M.UC_M68K_REG_PC)
            raise Stop('%s à %s (%08X) pile %s' % (ex, self.where(pc), pc, self.backtrace()))
        finally:
            if h is not None:
                self.uc.hook_del(h)

    def screen(self):
        """Image montrée (320 × 200, ARGB) : plans de la copper list,
        palette courante rl(LAB_05D2)."""
        planes = [self.rw(COPPER_BPL + 8 * p) << 16 | self.rw(COPPER_BPL + 8 * p + 4) for p in range(5)]
        pa = self.rl(self.S['LAB_05D2'])
        pal = []
        for i in range(32):
            c = self.rw(pa + 2 * i)
            pal.append(0xFF000000 | ((c >> 8) & 15) * 0x110000 | ((c >> 4) & 15) * 0x1100 | (c & 15) * 0x11)
        bp = [bytes(self.uc.mem_read(p, 8000)) if p + 8000 < self.stack_top else bytes(8000) for p in planes]
        out = []
        for y in range(200):
            for xb in range(40):
                b = [bp[p][y * 40 + xb] for p in range(5)]
                for k in range(8):
                    c = 0
                    for p in range(5):
                        if b[p] & (0x80 >> k):
                            c |= 1 << p
                    out.append(pal[c])
        return out

    def run_intro(self, every=0, prefix=None, limit=None):
        """SECSTRT_0 jusqu'au menu (LAB_005B) : chargements et générique
        (LAB_0185), puis les scènes (LAB_05A5, LAB_001B ... LAB_002E)."""
        S = self.S
        shots = []

        def on_vbl(ref):
            if every and ref.vbls % every == 0:
                path = '%s_%05d.png' % (prefix, ref.vbls)
                write_png(path, ref.screen(), 320, 200)
                shots.append(path)
            if limit and ref.vbls >= limit:
                ref.uc.emu_stop()
        self.on_vbl = on_vbl
        self.call(S['SECSTRT_0'], until=S['LAB_005B'],
                  A1=CHIP_BLOCK, D1=0x6B000 - CHIP_BLOCK, A0=self.fast, D0=FAST_SIZE)
        return shots


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('prefix')
    ap.add_argument('--every', type=int, default=50)
    ap.add_argument('--vbls', type=int, default=0)
    ap.add_argument('-v', action='store_true')
    a = ap.parse_args()
    ref = ProgRef(a.data, log=print if a.v else None)
    try:
        shots = ref.run_intro(a.every, a.prefix, a.vbls or None)
    except Stop as ex:
        print('arrêt : %s' % ex)
        shots = []
    print('%d VBL, pc %s, %d images' % (ref.vbls, ref.where(ref.uc.reg_read(M.UC_M68K_REG_PC)), len(shots)))


if __name__ == '__main__':
    main()
