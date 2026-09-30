#!/usr/bin/env python3
"""
mog_gamecheck.py — la nouvelle partie préparée en C (mog_game_boot :
démarrage, LAB_01AE, LAB_01BE, LAB_020F, LAB_03F1, SECSTRT_36) comparée
à celle de mog d'origine (tools/mog_ref.py, comme mog_lockstep.py), sur
toute la mémoire hors pile ; différences regroupées par plage.

  python3 tools/mog_gamecheck.py <dossier_données> [--all]
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef, STACK_SIZE  # noqa: E402
from mog_lockstep import new_game_map, RUN  # noqa: E402


def main():
    data = sys.argv[1]
    show_all = '--all' in sys.argv
    ref = MogRef(data, blitter=True)
    ref.boot()
    new_game_map(ref)
    orig = ref.snapshot()
    with tempfile.NamedTemporaryFile(delete=False) as f:
        path = f.name
    r = subprocess.run([RUN, '-', 'newgame', path, data], capture_output=True, text=True)
    print(r.stdout.strip(), r.stderr.strip())
    c = open(path, 'rb').read()
    os.unlink(path)
    limit = min(len(c), ref.stack_top - STACK_SIZE)
    diffs = [i for i in range(limit) if orig[i] != c[i]]
    runs = []
    for i in diffs:
        if runs and i - runs[-1][1] <= 16:
            runs[-1][1] = i
        else:
            runs.append([i, i])
    print('%d octets différents en %d plages' % (len(diffs), len(runs)))
    for a, b in runs if show_all else runs[:60]:
        print('  %08X-%08X %6d  %-26s orig %s  C %s' % (
            a, b, b - a + 1, ref.where(a) if a >= 0x100000 else 'chip+%X' % (a - 0x8000),
            orig[a:a + 8].hex(), c[a:a + 8].hex()))
    sys.exit(1 if diffs else 0)


if __name__ == '__main__':
    main()
