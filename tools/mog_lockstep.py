#!/usr/bin/env python3
"""
mog_lockstep.py — le jeu d'origine (banc tools/mog_ref.py, blitter émulé)
et le C (build/tests/mog_run, processus persistant) avancent côte à côte à
partir de la même mémoire ; à chaque début d'image (Combat_FrameStart,
carte comme combats), toute la mémoire est comparée, écrans compris.

  python3 tools/mog_lockstep.py <dossier_données> [--frames N] [--seed S]

Programme : nouvelle partie à un joueur puis la carte (LAB_0DAB) ; le
joueur se déplace sans arrêt (joystick 2, sans feu).
"""
import argparse
import os
import random
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef, STACK_SIZE, Stop  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'tests', 'mog_run')
IGNORE = ['v_VblCounter']


def new_game_map(ref):
    S = ref.S
    ref.left_combat = False
    ref.frame_limit = None
    ref.call(S['LAB_01AE'])
    ref.call(S['LAB_0011'])
    k = S['LAB_0613']
    ref.wl(S['LAB_06B4'], S['LAB_06B6'])
    ref.wl(k + 54, 0)
    ref.wb(k + 11, 2)
    ref.call(S['LAB_01BE'])
    ref.call(S['LAB_020F'])
    ref.call(S['LAB_03F1'])
    ref.call(S['SECSTRT_36'])


class CSide:
    def __init__(self, snap, program, data):
        d = tempfile.mkdtemp()
        self.mem_in = os.path.join(d, 'in.bin')
        self.dump = os.path.join(d, 'frame.bin')
        open(self.mem_in, 'wb').write(snap)
        self.p = subprocess.Popen([RUN, self.mem_in, program, self.dump, data],
                                  stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        self.lines = []

    def wait(self):
        """Jusqu'au prochain début d'image ; renvoie False à la fin."""
        self.lines = []
        while True:
            l = self.p.stdout.readline()
            if not l:
                return False
            l = l.rstrip('\n')
            if l == 'F':
                return True
            self.lines.append(l)
            if l.startswith('END'):
                return False

    def step(self, joy):
        self.p.stdin.write('J %d %d\n' % (joy[0], joy[1]))
        self.p.stdin.flush()

    def memory(self):
        return open(self.dump, 'rb').read()

    def close(self):
        try:
            self.p.stdin.write('Q\n')
            self.p.stdin.flush()
        except BrokenPipeError:
            pass
        self.p.wait()


def compare(ref, om, cm, what, lines):
    S = ref.S
    limit = ref.stack_top - STACK_SIZE
    if om[:limit] == cm[:limit]:
        return True
    skip = set()
    for n in IGNORE:
        skip.update(range(S[n], S[n] + 4))
    diffs = [i for i in range(limit) if om[i] != cm[i] and i not in skip]
    if not diffs:
        return True
    if os.environ.get('MOG_SAVE'):
        open(os.environ['MOG_SAVE'] + '.orig', 'wb').write(om)
        open(os.environ['MOG_SAVE'] + '.c', 'wb').write(cm)
    print('ÉCHEC %s : %d octets | %s' % (what, len(diffs), ' | '.join(
        l for l in lines if l.startswith(('M ', 'END')))[-300:]))
    last = -100
    shown = 0
    for i in diffs:
        if i - last > 16 and shown < 12:
            print('  %08X %-22s orig %s  C %s' % (
                i, ref.where(i) if i >= 0x100000 else 'chip+%X' % (i - 0x8000),
                om[i:i + 6].hex(), cm[i:i + 6].hex()))
            shown += 1
        last = i
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--frames', type=int, default=500)
    ap.add_argument('--seed', type=int, default=1)
    a = ap.parse_args()
    rng = random.Random(a.seed)

    ref = MogRef(a.data, blitter=True)
    ref.boot()
    S = ref.S
    new_game_map(ref)
    c = CSide(ref.snapshot(), 'map', a.data)
    if os.environ.get('MOG_SAVE'):
        open(os.environ['MOG_SAVE'] + '.in', 'wb').write(ref.snapshot())
        open(os.environ['MOG_SAVE'] + '.joy', 'w').close()
    ref.run_frames(0, start=S['LAB_0DAB'])
    fails = 0
    joy = 0
    frames = 0
    if not c.wait():
        print('C terminé : %s' % c.lines)
        sys.exit(1)
    ok = compare(ref, ref.snapshot(), c.memory(), 'image 0', c.lines)
    fails += not ok
    for f in range(1, a.frames + 1):
        if not ok:
            break
        if not joy or rng.random() < 0.05:
            joy = rng.choice([1, 2, 4, 8, 5, 6, 9, 10])
        ref.joy = [0, joy]
        c.step([0, joy])
        if os.environ.get('MOG_SAVE'):
            with open(os.environ['MOG_SAVE'] + '.joy', 'a') as jf:
                jf.write('J 0 %d\n' % joy)
        try:
            ref.run_frames(1)
        except Stop as ex:
            print('original arrêté : %s' % ex)
            break
        alive = c.wait()
        ok = compare(ref, ref.snapshot(), c.memory(), 'image %d (joy %d)' % (f, joy), c.lines)
        fails += not ok
        frames = f
        if not alive:
            print('C terminé : %s' % c.lines[-3:])
            break
    c.close()
    print('%d images comparées, %d échecs' % (frames, fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
