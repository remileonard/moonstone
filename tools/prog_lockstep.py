#!/usr/bin/env python3
"""
prog_lockstep.py — une scène de l'intro de program exécutée en même temps
par l'original (tools/prog_ref.py) et par le C (build/tests/prog_run),
depuis la même mémoire : l'original est mené jusqu'à l'entrée de la scène,
sa mémoire donnée au C, puis les deux mémoires sont comparées à la fin de
chaque VBL (serveurs passés) et à la fin de la scène.

SECSTRT_1 (départ de la musique) ne fait rien des deux côtés : son appel
est seulement noté (« M »).

  python3 tools/prog_lockstep.py <dossier_données> [--scene 05a5] [--png préfixe]
"""
import argparse
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)

import unicorn as U  # noqa: E402
import unicorn.m68k_const as M  # noqa: E402

from prog_ref import ProgRef, STACK_SIZE, CHIP_BLOCK, FAST_SIZE, write_png  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'tests', 'prog_run')
SCENES = {k: 'LAB_' + k.upper() for k in
          ('05a5', '001b', '001c', '0174', '001a', '002c', '002d', '002f', '002e', '0054')}


class Bench:
    """L'original, arrêté à la fin de chaque VBL (RTS du code de VBL)."""

    def __init__(self, data):
        self.ref = ref = ProgRef(data)
        S = ref.S
        self.events = []
        self.rts = ref.stack_top - 0x80 + 28          # RTS du code de VBL
        self.skip = False
        self.stopped = False
        self.active = False
        ref.uc.hook_add(U.UC_HOOK_CODE, self.h_rts, None, begin=self.rts, end=self.rts)
        self.sentinel = ref.stack_top - 0x10

    def h_rts(self, uc, addr, size, user):
        if not self.active:
            return
        if self.skip:
            self.skip = False
            return
        self.stopped = True
        uc.emu_stop()

    def boot(self):
        """SECSTRT_0 depuis son entrée (registres du lanceur) ; fin : LAB_0000."""
        ref = self.ref
        ref.w('A1', CHIP_BLOCK)
        ref.w('D1', 0x6B000 - CHIP_BLOCK)
        ref.w('A0', ref.fast)
        ref.w('D0', FAST_SIZE)
        ref.uc.hook_add(U.UC_HOOK_CODE, lambda uc, a, sz, u: uc.emu_stop(), None,
                        begin=ref.S['LAB_0000'], end=ref.S['LAB_0000'])

    def to_scene(self, label):
        ref = self.ref
        self.active = False
        ref.call(ref.S['SECSTRT_0'], until=ref.S[label],
                 A1=CHIP_BLOCK, D1=0x6B000 - CHIP_BLOCK, A0=ref.fast, D0=FAST_SIZE)

    def start(self, label):
        """Scène appelée depuis la pile propre de call()."""
        ref = self.ref
        sp = ref.stack_top - 0x100
        ref.wl(sp, self.sentinel)
        ref.w('A7', sp)
        self.pc = ref.S[label]
        self.active = True
        self.events = []

    def step(self):
        """Jusqu'à la fin de la VBL suivante ; False : scène finie."""
        ref = self.ref
        self.stopped = False
        try:
            ref.uc.emu_start(self.pc, self.sentinel)
        except U.UcError as ex:
            pc = ref.uc.reg_read(M.UC_M68K_REG_PC)
            raise SystemExit('erreur %s à %s' % (ex, ref.where(pc)))
        self.pc = ref.uc.reg_read(M.UC_M68K_REG_PC)
        if not self.stopped or self.pc == ref.S['LAB_0000']:
            return False
        self.skip = self.pc == self.rts
        return True

    def memory(self):
        return bytes(self.ref.uc.mem_read(0, self.ref.stack_top))


def compare(ref, om, cm, what):
    limit = ref.stack_top - STACK_SIZE
    if om[:limit] == cm[:limit]:
        return True
    diffs = [i for i in range(limit) if om[i] != cm[i]]
    print('ÉCHEC %s : %d octets' % (what, len(diffs)))
    last, shown = -100, 0
    for i in diffs:
        if i - last > 16 and shown < 12:
            print('  %08X %-22s orig %s  C %s' % (i, ref.where(i), om[i:i + 8].hex(), cm[i:i + 8].hex()))
            shown += 1
        last = i
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--scene', default='05a5')
    ap.add_argument('--png', help='préfixe : image montrée toutes les 50 VBL')
    a = ap.parse_args()
    label = SCENES.get(a.scene)

    b = Bench(a.data)
    ref = b.ref
    if a.scene == 'intro':
        label = 'SECSTRT_0'
        b.boot()
    else:
        b.to_scene(label)
    b.start(label)
    d = tempfile.mkdtemp()
    snap, cdump = os.path.join(d, 'm.bin'), os.path.join(d, 'c.bin')
    open(snap, 'wb').write(b.memory())
    p = subprocess.Popen([RUN, snap, a.scene, cdump, '%x' % ref.r('A1'), a.data, '%x' % ref.fast], stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE, text=True)
    vbl = 0
    cev = []
    while True:
        line = p.stdout.readline().strip()
        while line == 'M':
            cev.append('M')
            line = p.stdout.readline().strip()
        more = b.step()
        if (line == 'V') != more:
            print('ÉCHEC VBL %d : C %s, original %s' % (vbl + 1, line or 'fin', 'VBL' if more else 'fin'))
            sys.exit(1)
        vbl += 1
        if cev != b.events:
            print('ÉCHEC VBL %d : événements C %s, original %s' % (vbl, cev, b.events))
            sys.exit(1)
        if not compare(ref, b.memory(), open(cdump, 'rb').read(), 'VBL %d' % vbl if more else 'fin'):
            p.kill()
            sys.exit(1)
        if a.png and more and vbl % 50 == 0:
            write_png('%s_%04d.png' % (a.png, vbl), ref.screen(), 320, 200)
        if not more:
            break
        p.stdin.write('C\n')
        p.stdin.flush()
    p.wait()
    print('scène %s : %d VBL identiques' % (label, vbl - 1))


if __name__ == '__main__':
    main()
