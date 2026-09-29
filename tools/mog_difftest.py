#!/usr/bin/env python3
"""
mog_difftest.py — test différentiel des routines portées du combat
(game/src/mog_*.c) contre le code 68000 d'origine, sur un vrai combat.

Le jeu d'origine tourne dans tools/mog_ref.py (vraies données) : duel à deux
joueurs humains, joysticks pilotés par un plan pseudo-aléatoire (repos,
marche, attaques ; voir Players). Au début de chaque image, la mémoire est copiée ; pour
chaque routine testée :
  - l'original est exécuté seul sur cette copie (second émulateur) ;
  - la version C (build/tests/mog_step) aussi ;
puis mémoire (hors pile de l'émulateur) et sons sont comparés.

  python3 tools/mog_difftest.py <dossier_données> [--frames N] [--seed S]
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
from mog_ref import MogRef, Stop, STACK_SIZE  # noqa: E402

STEP = os.path.join(ROOT, 'build', 'tests', 'mog_step')
ROUTINES = ['Combat_RunControllers']


class Players:
    """Deux joueurs simples : s'alignent en profondeur sur l'adversaire,
    s'approchent, puis attaquent (direction au hasard) ou esquivent. Un
    geste est tenu quelques images."""
    MOVES = [0, 1, 2, 4, 8, 5, 9, 6, 10]

    def __init__(self, rng):
        self.rng = rng
        self.hold = [0, 0]
        self.cur = [0, 0]

    def joy(self, ref):
        S, rng = ref.S, self.rng
        objs = [ref.rl(S['v_Combatants']), ref.rl(S['v_Combatants'] + 4)]
        s16 = lambda v: v - 0x10000 if v & 0x8000 else v
        for p in range(2):
            me, other = objs[p], objs[1 - p]
            if not me or not other:
                continue
            port = 0 if ref.rb(me + 11) == 1 else 1
            if self.hold[port] > 0:
                self.hold[port] -= 1
                continue
            dx = s16(ref.rw(other + 4)) - s16(ref.rw(me + 4))
            dd = s16(ref.rw(other + 8)) - s16(ref.rw(me + 8))
            r = rng.random()
            if r < 0.1:
                j = rng.choice(self.MOVES)
            elif abs(dd) > 3 and r < 0.6:
                j = 4 if dd > 0 else 8
            elif abs(dx) > 70 and r < 0.8:
                j = 1 if dx > 0 else 2
            else:
                j = 0x10 | rng.choice(self.MOVES)
            self.cur[port] = j
            self.hold[port] = rng.randint(0, 6)
        return tuple(self.cur)


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
    ev = [l for l in out.splitlines() if l.startswith('S ')]
    tail = [l for l in out.splitlines() if l.startswith('E ')]
    msgs = [l[2:] for l in out.splitlines() if l.startswith('M ')]
    return mem, ev, tail[0] if tail else '', msgs


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--frames', type=int, default=300)
    ap.add_argument('--seed', type=int, default=1)
    a = ap.parse_args()

    rng = random.Random(a.seed)
    ref = MogRef(a.data)
    ref.boot()
    ref.start_duel()
    solo = MogRef(a.data)
    S = ref.S
    ignore_from = ref.stack_top - STACK_SIZE
    players = Players(rng)
    fails = 0
    duels = 0
    cover = {}                           # réactions rencontrées (contrôleur -> nombre)
    for f in range(a.frames):
        joy = players.joy(ref)
        snap = ref.snapshot()
        for i in range(10):
            en = S['t_Entities'] + 50 * i
            if ref.rb(en):
                o = ref.rl(en + 24)
                for off, kind in ((18, 'touché'), (14, 'a touché')):
                    other = ref.rl(o + off)
                    if other:
                        k = '%s par ctl %d' % (kind, ref.rb(other + 77))
                        cover[k] = cover.get(k, 0) + 1
        for routine in ROUTINES:
            solo.restore(snap)
            solo.joy = list(joy)
            solo.events = []
            try:
                solo.call(S[routine])
            except Stop as ex:
                print('image %d %s : original arrêté : %s' % (f, routine, ex))
                fails += 1
                continue
            om = solo.snapshot()
            cm, cev, tail, msgs = run_c(routine, snap, joy)
            errs = [t for t in msgs if 'non port' in t]
            diffs = [i for i in range(min(len(om), ignore_from)) if om[i] != cm[i]]
            if diffs or cev != solo.events or tail != 'E 0 0':
                fails += 1
                print('ÉCHEC image %d %s joy=%s : %s' % (f, routine, joy, '; '.join(errs) or tail))
                if cev != solo.events:
                    print('  sons : original %s, C %s' % (solo.events, cev))
                for i in diffs[:8]:
                    print('  %08X %-20s orig %02X  C %02X' % (i, ref.where(i) if i >= 0x100000 else '', om[i], cm[i]))
                if fails > 5:
                    sys.exit(1)
        ref.joy = list(joy)
        ref.run_frames(1)
        if ref.left_combat:
            duels += 1
            ref.start_duel()
    print('contacts : %s' % (', '.join('%s ×%d' % kv for kv in sorted(cover.items())) or 'aucun'))
    print('duels terminés : %d' % duels)
    print('%d images, %d échecs' % (a.frames, fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
