#!/usr/bin/env python3
"""
mog_drawcheck.py — LAB_0CDA (dessin d'une frame CEL, game/src/mog_blit.c)
comparé à l'original, blitter émulé des deux côtés.

Un combat tourne dans le banc avec le blitter (tools/mog_ref.py,
blitter=True) ; des appels de LAB_0CDA pris au fil des images sont rejoués
depuis la même mémoire par l'original et par le C (build/tests/mog_step
« Draw:cel:frame:x:y »), puis la mémoire est comparée (écrans compris).

  python3 tools/mog_drawcheck.py <dossier_données> [--frames N] [--every K]
                                 [--encounter LAB_xxxx]
"""
import argparse
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef, STACK_SIZE  # noqa: E402
import unicorn as U  # noqa: E402

STEP = os.path.join(ROOT, 'build', 'tests', 'mog_step')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--frames', type=int, default=40)
    ap.add_argument('--every', type=int, default=5)
    ap.add_argument('--encounter')
    a = ap.parse_args()

    ref = MogRef(a.data, blitter=True)
    ref.boot()
    S = ref.S
    calls = []
    n = [0]

    def on_draw(uc, addr, size, user):
        n[0] += 1
        if n[0] % a.every == 0:
            calls.append((ref.snapshot(), ref.r('A0'), ref.r('D0') & 0xFFFF,
                          ref.r('D1') & 0xFFFF, ref.r('D2') & 0xFFFF))
    ref.uc.hook_add(U.UC_HOOK_CODE, on_draw, None, begin=S['LAB_0CDA'], end=S['LAB_0CDA'])
    if a.encounter:
        ref.start_encounter(a.encounter)
    else:
        ref.start_duel(cpu=True)
    ref.joy = [0x11, 0]
    ref.run_frames(a.frames)
    print('%d dessins, %d rejoués' % (n[0], len(calls)))

    solo = MogRef(a.data, blitter=True)
    limit = ref.stack_top - STACK_SIZE
    fails = 0
    for snap, cel, fr, x, y in calls:
        solo.restore(snap)
        solo.blitter.olda = solo.blitter.oldb = 0
        solo.call(S['LAB_0CDA'], A0=cel, D0=fr, D1=x, D2=y)
        om = solo.snapshot()
        with tempfile.NamedTemporaryFile(delete=False) as f:
            f.write(snap)
            src = f.name
        dst = src + '.out'
        subprocess.run([STEP, src, 'Draw:%x:%x:%x:%x' % (cel, fr, x, y), '0', '0', dst],
                       capture_output=True)
        cm = open(dst, 'rb').read()
        os.unlink(src)
        os.unlink(dst)
        diffs = [i for i in range(limit) if om[i] != cm[i]]
        if diffs:
            fails += 1
            print('ÉCHEC cel %06X frame %d (%d,%d) : %d octets, premier %06X (%s) orig %02X C %02X'
                  % (cel, fr, x - 0x10000 if x & 0x8000 else x, y - 0x10000 if y & 0x8000 else y,
                     len(diffs), diffs[0], ref.where(diffs[0]) if diffs[0] >= 0x100000 else 'chip',
                     om[diffs[0]], cm[diffs[0]]))
    print('%d dessins comparés, %d échecs' % (len(calls), fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
