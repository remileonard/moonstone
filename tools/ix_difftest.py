#!/usr/bin/env python3
"""
ix_difftest.py — test différentiel du moteur IMAGEXCEL C (game/src/ix_engine.c)
contre le code 68000 d'origine de mog, exécuté par l'émulateur Unicorn.

Pour chaque scénario, la même image mémoire de mog (relocations appliquées,
objets, banques, entités préparés) est donnée :
  - à Unicorn, qui exécute Ix_RunEntities d'origine image après image ; les
    routines hors moteur (dessin, dimensions de frame, sons, messages,
    routines natives des $B0) sont remplacées par un RTS qui note l'appel ;
  - à build/tests/ix_trace (moteur C).
Les événements (dessins, sons, appels, messages) et l'empreinte CRC32 de
toute la mémoire sont comparés après chaque image.

  python3 tools/ix_difftest.py [--frames N] [--seed S] [--only LABEL]

Prérequis : pip install unicorn capstone ; cmake --build build --target ix_trace
"""
import argparse
import os
import random
import re
import struct
import subprocess
import sys
import tempfile
import zlib

import unicorn as U
import unicorn.m68k_const as M

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import ix_scripts as X  # noqa: E402

TRACE = os.path.join(ROOT, 'build', 'tests', 'ix_trace')
EXTRA = 0x20000
STACK_ZONE = 0x1000     # fin de la zone libre : pile d'Unicorn, exclue


def syms():
    s = {}
    for line in open(os.path.join(ROOT, 'game', 'data', 'ix_mog_syms.h')):
        m = re.match(r'#define MOG_(\w+) 0x([0-9A-F]+)u', line)
        if m:
            s[m.group(1)] = int(m.group(2), 16)
    return s


def frame_info(cel, frame):
    return 16 + (frame * 7 + (cel & 0xFF) * 3) % 48, 20 + (frame * 5) % 40


class Image:
    def __init__(self, b):
        bases, total = X.hunk_bases(b)
        self.base = X.VM_BASE
        self.size = total + EXTRA
        m = bytearray(self.size)
        for h, hk in enumerate(b.hunks):
            o = bases[h] - self.base
            m[o:o + len(hk['data'])] = hk['data']
        for h, hk in enumerate(b.hunks):
            for pos, t in b.rel[h].items():
                o = bases[h] - self.base + pos
                v = struct.unpack('>I', m[o:o + 4])[0] + bases[t]
                m[o:o + 4] = struct.pack('>I', v & 0xFFFFFFFF)
        self.mem = m
        self.end = self.base + total          # zone libre ensuite

    def wl(self, va, v):
        self.mem[va - self.base:va - self.base + 4] = struct.pack('>I', v & 0xFFFFFFFF)

    def ww(self, va, v):
        self.mem[va - self.base:va - self.base + 2] = struct.pack('>H', v & 0xFFFF)

    def wb(self, va, v):
        self.mem[va - self.base] = v & 0xFF


# ---------------------------------------------------------------------------
# Scénarios
# ---------------------------------------------------------------------------

def build_scenario(img, S, scripts, rng, debug=False):
    """Prépare la mémoire : objets, banques, entités exécutant `scripts`."""
    objs = img.end + 0x0000                    # 20 objets de 132 octets
    banks = img.end + 0x1000                   # 5 tables de 8 banques
    scrap = img.end + 0x1800
    img.wl(S['LAB_05C3'], objs)
    img.wl(S['LAB_05C0'], 0x11111111)
    img.wl(S['LAB_0D92'], 0x22222222)
    img.wl(S['v_VblCounter'], rng.randrange(1 << 16))
    img.wl(S['LAB_06DA'], 1 if debug else 0)
    for t in range(5):
        tb = banks + t * 0x40
        for i in range(8):
            img.wl(tb + 4 * i, 0x00F00000 + t * 0x100 + i * 0x10)
        img.wl(S['LAB_0647'] + 4 * t, tb)
    # table des handlers d'opcodes, remplie par le jeu (LAB_0304)
    for n, v in S.items():
        m = re.match(r'IxOp([0-9A-F]{2})_', n)
        if m:
            img.wl(S['t_IxOpcodes'] + (int(m.group(1), 16) & 0x7F), v)
    # entités reliées (LAB_0305)
    for i in range(10):
        en = S['t_Entities'] + 50 * i
        img.wl(en + 40, S['t_StrikeFrames'] + 80 * i)
        img.wl(en + 44, S['t_BodyFrames'] + 80 * i)
        img.wl(en + 36, S['LAB_064B'] + 36 * i)
    for i, sc in enumerate(scripts):
        o = objs + 132 * i
        for k in range(132):
            img.wb(o + k, rng.randrange(256))   # champs variés : conditions $C8/$CC
        img.wl(o, 1)
        img.ww(o + 80, rng.choice([100, 100, 100, 0, -5]))
        en = S['t_Entities'] + 50 * i
        img.wb(en + 0, 1)
        img.wb(en + 1, 1)
        img.wl(en + 2, sc)
        img.ww(en + 6, rng.randrange(0, 300))
        img.ww(en + 8, rng.choice([0, 0, -10]))
        img.ww(en + 10, rng.randrange(40, 180))
        img.wb(en + 22, rng.choice([1, 3]))
        img.wl(en + 24, o)
        img.wl(en + 28, banks + rng.randrange(5) * 0x40)
        img.wb(en + 32, 12)
    img.wl(S['v_Combatants'], objs)            # joueur = premier objet
    # objets restants libres (Ent_Spawn)
    for i in range(len(scripts), 20):
        img.wl(objs + 132 * i, 0)
    return scrap


# ---------------------------------------------------------------------------
# Exécution d'origine (Unicorn)
# ---------------------------------------------------------------------------

class Original:
    def __init__(self, img, S, b0_targets):
        self.img, self.S = img, S
        self.uc = U.Uc(U.UC_ARCH_M68K, U.UC_MODE_BIG_ENDIAN)
        self.uc.ctl_set_cpu_model(M.UC_CPU_M68K_M68000)
        size = (img.size + 0xFFF) & ~0xFFF
        self.uc.mem_map(img.base, size)
        self.uc.mem_write(img.base, bytes(img.mem))
        self.events = []
        self.bg = 0
        self.patched = {}
        hooks = {
            S['Ix_FrameInfo']: self.h_frameinfo,
            S['LAB_0D1B']: lambda: None,
            S['LAB_0CDA']: self.h_draw,
            S['L00_0908E']: self.h_buffer,
            S['LAB_0AA2']: self.h_sound,
            S['LAB_0BB3']: self.h_message,
        }
        for t in b0_targets:
            hooks.setdefault(t, (lambda t=t: self.events.append('C %08X' % t)))
        for a, fn in hooks.items():
            self.patched[a] = bytes(self.uc.mem_read(a, 2))
            self.uc.mem_write(a, b'\x4e\x75')              # RTS
            self.uc.hook_add(U.UC_HOOK_CODE, lambda uc, addr, sz, fn: fn(),
                             fn, begin=a, end=a)
        self.stack = img.base + img.size - 0x100      # dans STACK_ZONE
        self.sentinel = img.base + img.size - 0x10

    def r(self, reg):
        return self.uc.reg_read(reg)

    def rl(self, va):
        return struct.unpack('>I', self.uc.mem_read(va, 4))[0]

    def h_frameinfo(self):
        a1 = self.r(M.UC_M68K_REG_A1)
        w, h = frame_info(self.r(M.UC_M68K_REG_A0), self.r(M.UC_M68K_REG_D0) & 0xFF)
        self.uc.mem_write(a1 + 16, struct.pack('>HH', w, h))

    def h_draw(self):
        a1 = self.r(M.UC_M68K_REG_A1)
        flip = 1 if self.uc.mem_read(a1 + 22, 1)[0] & 2 else 0
        s16 = lambda v: v - 0x10000 if v & 0x8000 else v
        self.events.append('D %08X %d %d %d %d %d' % (
            self.r(M.UC_M68K_REG_A0), self.r(M.UC_M68K_REG_D0) & 0xFFFF,
            s16(self.r(M.UC_M68K_REG_D1) & 0xFFFF), s16(self.r(M.UC_M68K_REG_D2) & 0xFFFF),
            flip, self.bg))

    def h_buffer(self):
        self.bg = 1 if self.r(M.UC_M68K_REG_D0) == 0x11111111 else 0

    def h_sound(self):
        self.events.append('S %d' % (self.r(M.UC_M68K_REG_D0) & 0xFF))

    def h_message(self):
        a0 = self.r(M.UC_M68K_REG_A0)
        s = bytes(self.uc.mem_read(a0, 80)).split(b'\0')[0].decode('latin-1')
        self.events.append('M %s' % s)

    def frame(self, scrap):
        S, uc = self.S, self.uc
        uc.mem_write(S['t_StrikeFrames'], bytes(800))
        uc.mem_write(S['t_BodyFrames'], bytes(800))
        uc.mem_write(S['LAB_0641'], struct.pack('>I', scrap))
        uc.mem_write(S['LAB_0645'], b'\0\0')
        uc.reg_write(M.UC_M68K_REG_A7, self.stack - 4)
        uc.mem_write(self.stack - 4, struct.pack('>I', self.sentinel))
        self.events = []
        uc.emu_start(S['Ix_RunEntities'], self.sentinel, count=5_000_000)
        return self.events

    def crc(self):
        m = bytearray(self.uc.mem_read(self.img.base, self.img.size))
        for a, orig in self.patched.items():
            m[a - self.img.base:a - self.img.base + 2] = orig
        m[len(m) - STACK_ZONE:] = bytes(STACK_ZONE)     # pile de l'émulateur
        return zlib.crc32(bytes(m)) & 0xFFFFFFFF, m


# ---------------------------------------------------------------------------

def run_c(img, frames, scrap, dump=None):
    with tempfile.NamedTemporaryFile(delete=False) as f:
        f.write(bytes(img.mem))
        path = f.name
    args = [TRACE, path, str(frames), '0x%X' % scrap] + ([dump] if dump else [])
    out = subprocess.run(args, capture_output=True, text=True).stdout
    os.unlink(path)
    per, cur, crcs, tail = [], None, [], ''
    for line in out.splitlines():
        if line.startswith('F '):
            cur = []
            per.append(cur)
        elif line.startswith('H '):
            crcs.append(int(line[2:], 16))
        elif line.startswith('E '):
            tail = line
        elif cur is not None:
            cur.append(line)
    return per, crcs, tail


def scenario(b, S, scripts, frames, seed, b0, debug=False, verbose=False):
    rng = random.Random(seed)
    img = Image(b)
    scrap = build_scenario(img, S, scripts, rng, debug)
    orig = Original(img, S, b0)
    c_events, c_crcs, tail = run_c(img, frames, scrap)
    for f in range(frames):
        try:
            o = orig.frame(scrap)
        except U.UcError as ex:
            return 'image %d : erreur émulateur %s' % (f, ex)
        oc, mem = orig.crc()
        if o != c_events[f]:
            return 'image %d : événements différents\n  original : %s\n  C        : %s' % (
                f, o[:8], c_events[f][:8])
        if oc != c_crcs[f]:
            # localiser la première différence
            with tempfile.NamedTemporaryFile(delete=False) as d:
                dump = d.name
            run_c(img, f + 1, scrap, dump)
            cm = open(dump, 'rb').read()
            os.unlink(dump)
            diffs = [i for i in range(len(mem)) if mem[i] != cm[i]][:8]
            where = ', '.join('%08X (orig %02X, C %02X)' % (img.base + i, mem[i], cm[i])
                              for i in diffs)
            return 'image %d : mémoire différente : %s' % (f, where)
    if tail and tail != 'E 0 0':
        return 'moteur C : erreurs/accès hors image %s' % tail
    return None


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--frames', type=int, default=60)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--only')
    ap.add_argument('--mixed', type=int, default=40, help='scénarios à 4 entités')
    a = ap.parse_args()

    b = X.Binary('mog')
    S = syms()
    scripts, _ = X.identify(b)
    bases, _ = X.hunk_bases(b)
    vas = {b.name(k): bases[k[0]] + k[1] for k in scripts}
    b0 = sorted({bases[t[0]] + t[1] for s in scripts.values()
                 for _, kind, t in s['refs'] if kind == 'code'})
    names = sorted(vas) if not a.only else [a.only]
    fails = 0
    for i, n in enumerate(names):
        for debug in (False, True):
            err = scenario(b, S, [vas[n]], a.frames, a.seed + i, b0, debug)
            if err:
                fails += 1
                print('ÉCHEC %s%s : %s' % (n, ' (debug)' if debug else '', err))
    rng = random.Random(a.seed)
    for j in range(a.mixed if not a.only else 0):
        pick = rng.sample(names, 4)
        err = scenario(b, S, [vas[n] for n in pick], a.frames, 1000 + j, b0)
        if err:
            fails += 1
            print('ÉCHEC mélange %s : %s' % (','.join(pick), err))
    total = len(names) * 2 + (a.mixed if not a.only else 0)
    print('%d scénarios, %d échecs' % (total, fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
