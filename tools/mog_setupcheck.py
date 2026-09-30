#!/usr/bin/env python3
"""
mog_setupcheck.py — préparation des rencontres (game/src/mog_encounter.c)
comparée à l'original : après une nouvelle partie (MogRef.prepare_knights),
la mémoire est copiée ; l'original exécute la routine de t_CreatureInit puis
Combat_Run jusqu'à Combat_Loop, la version C (build/tests/mog_step) fait de
même sur la copie. Les zones différentes sont listées.

  python3 tools/mog_setupcheck.py <dossier_données> [LAB_0168 ...]
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef, STACK_SIZE  # noqa: E402

STEP = os.path.join(ROOT, 'build', 'tests', 'mog_step')

# Compteur d'images (le banc compte les attentes de LAB_03F2) ; disque
# demandé (LAB_0100 : sans objet, tous les fichiers sont présents)
IGNORE = ['v_VblCounter', 'LAB_05AC', 'LAB_06FE', 'LAB_0B35']

SCREENS = ['LAB_05C0', 'SECSTRT_35', 'LAB_0D92']


def ranges(diffs):
    out = []
    for i in diffs:
        if out and i <= out[-1][1] + 16:
            out[-1][1] = i
        else:
            out.append([i, i])
    return out


def check(data, init):
    ref = MogRef(data, blitter=BLIT)
    ref.boot()
    ref.prepare_knights()
    S = ref.S
    ref.wl(S['v_Combatants'] + 4, S['LAB_0614'])     # adversaire de LAB_0164
    snap = ref.snapshot()
    ref.events = []
    ref.run_encounter(init)
    om = ref.snapshot()
    with tempfile.NamedTemporaryFile(delete=False) as f:
        f.write(snap)
        src = f.name
    dst = src + '.out'
    r = subprocess.run([STEP, src, init + '+Combat_Run', '0', '0', dst, data],
                       capture_output=True, text=True)
    cm = open(dst, 'rb').read()
    os.unlink(src)
    os.unlink(dst)
    msgs = [l for l in r.stdout.splitlines() if l.startswith(('M ', 'E '))]
    skip = set()
    for n in IGNORE:
        if n in S:
            skip.update(range(S[n], S[n] + 4))
    # écrans : sans l'émulation du blitter, terrain et copies d'écran ne
    # sont pas faits par le banc
    if not BLIT:
        for n in SCREENS:
            a = ref.rl(S[n])
            skip.update(range(a, a + 5 * 0x1F40))
    # palette courante du fondu (rl(LAB_0E93), tenue par l'interruption)
    skip.update(range(ref.rl(S['LAB_0E93']), ref.rl(S['LAB_0E93']) + 66))
    limit = ref.stack_top - STACK_SIZE
    diffs = [i for i in range(min(len(om), limit)) if om[i] != cm[i] and i not in skip]
    print('== %s : %d octets différents %s' % (init, len(diffs), ' | '.join(msgs[-3:])))
    for a, b in ranges(diffs)[:25]:
        name = ref.where(a) if a >= 0x100000 else ('chip+%X' % (a - 0x8000) if a < 0x100000 else '')
        print('  %08X-%08X (%5d) %-24s orig %s  C %s' % (
            a, b, b - a + 1, name, om[a:a + 8].hex(), cm[a:a + 8].hex()))
    return not diffs


BLIT = False


def main():
    global BLIT
    args = sys.argv[1:]
    if '--blitter' in args:
        args.remove('--blitter')
        BLIT = True
    data = args[0]
    inits = args[1:] or ['LAB_0164', 'LAB_0168', 'LAB_016A', 'LAB_0175', 'LAB_0188',
                             'LAB_018C', 'LAB_0192', 'LAB_0196', 'LAB_019A', 'LAB_019E',
                             'LAB_01A0']
    ok = all([check(data, i) for i in inits])
    sys.exit(0 if ok else 1)


if __name__ == '__main__':
    main()
