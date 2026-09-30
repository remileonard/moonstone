#!/usr/bin/env python3
"""
mog_mapcheck.py — carte du monde (game/src/mog_map.c) comparée à l'original,
blitter émulé des deux côtés (écrans compris).

Nouvelle partie à un joueur (MogRef.start_map) : l'entrée sur la carte
(LAB_0DAB) puis chaque image (LAB_0DAD...) sont exécutées par l'original et
par le C (build/tests/mog_step « Map_Enter » / « Map_Frame ») depuis la même
mémoire ; le joueur se déplace sans arrêt (joystick 2, direction changée de temps
en temps, sans feu) pour que les tours et les manches passent.

  python3 tools/mog_mapcheck.py <dossier_données> [--frames N] [--seed S]
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
from mog_ref import MogRef, STACK_SIZE  # noqa: E402

STEP = os.path.join(ROOT, 'build', 'tests', 'mog_step')
IGNORE = ['v_VblCounter']


def run_c(routine, snap, joy):
    with tempfile.NamedTemporaryFile(delete=False) as f:
        f.write(snap)
        src = f.name
    dst = src + '.out'
    out = subprocess.run([STEP, src, routine, str(joy[0]), str(joy[1]), dst],
                         capture_output=True, text=True).stdout
    mem = open(dst, 'rb').read()
    os.unlink(src)
    os.unlink(dst)
    return mem, out


def compare(ref, om, cm, what, out):
    S = ref.S
    skip = set()
    for n in IGNORE:
        skip.update(range(S[n], S[n] + 4))
    limit = ref.stack_top - STACK_SIZE
    diffs = [i for i in range(limit) if om[i] != cm[i] and i not in skip]
    msgs = [l for l in out.splitlines() if l.startswith(('M ', 'R ', 'E '))]
    if not diffs and 'non port' not in out:
        return True
    print('ÉCHEC %s : %d octets | %s' % (what, len(diffs), ' | '.join(msgs[-4:])))
    shown = 0
    last = -100
    for i in diffs:
        if i - last > 16 and shown < 12:
            print('  %08X %-22s orig %s  C %s' % (i, ref.where(i) if i >= 0x100000 else 'chip+%X' % (i - 0x8000),
                                                 om[i:i + 6].hex(), cm[i:i + 6].hex()))
            shown += 1
        last = i
    return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--frames', type=int, default=100)
    ap.add_argument('--seed', type=int, default=1)
    a = ap.parse_args()
    rng = random.Random(a.seed)

    ref = MogRef(a.data, blitter=True)
    ref.boot()
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
    snap = ref.snapshot()
    ref.run_frames(0, start=S['LAB_0DAB'])
    cm, out = run_c('Map_Enter', snap, [0, 0])
    ok = compare(ref, ref.snapshot(), cm, 'entrée', out)
    fails = 0 if ok else 1
    joy = 0
    turns = 0
    for f in range(a.frames):
        if not joy or rng.random() < 0.05:              # toujours en mouvement
            joy = rng.choice([1, 2, 4, 8, 5, 6, 9, 10])
        ref.joy = [0, joy]
        snap = ref.snapshot()
        who = ref.rl(S['LAB_0633'])
        ref.run_frames(1)
        cm, out = run_c('Map_Frame', snap, [0, joy])
        if ref.rl(S['LAB_0633']) != who:
            turns += 1
        if not compare(ref, ref.snapshot(), cm, 'image %d (joy %d)' % (f, joy), out):
            fails += 1
            if fails > 3:
                break
    print('%d images, %d changements de chevalier, %d échecs' % (a.frames, turns, fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
