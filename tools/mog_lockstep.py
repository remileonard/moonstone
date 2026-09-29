#!/usr/bin/env python3
"""
mog_lockstep.py — le jeu d'origine (banc tools/mog_ref.py, blitter émulé)
et le C (build/tests/mog_run, processus persistant) avancent côte à côte à
partir de la même mémoire ; à chaque début d'image (Combat_FrameStart,
carte comme combats), toute la mémoire est comparée, écrans compris.

  python3 tools/mog_lockstep.py <dossier_données> [--frames N] [--seed S]

Programme : nouvelle partie à un joueur puis la carte (LAB_0DAB) ; le
joueur se déplace sans arrêt (joystick 2, sans feu). Avec --space N, la
barre d'espace est frappée toutes les N images (écran d'inventaire,
LAB_04CF 9). Dans les écrans LAB_04CF, le pointeur erre et clique au
hasard (--wander rendez-vous), puis va sur la sortie (zone 7) et clique.
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
import unicorn.m68k_const as M  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'tests', 'mog_run')
IGNORE = []


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

    def step(self, joy, key=None):
        if key is None:
            self.p.stdin.write('J %d %d\n' % (joy[0], joy[1]))
        else:
            self.p.stdin.write('J %d %d %d\n' % (joy[0], joy[1], key))
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


def to_exit(ref, exit_id=7, field=16):
    """Joystick qui mène le pointeur sur la sortie (zone dont le champ
    `field` vaut exit_id), puis feu."""
    S = ref.S
    x, y = ref.rw(S['LAB_097F']), ref.rw(S['LAB_0980'])
    a0 = ref.rl(S['SECSTRT_14'])
    while ref.rw(a0 + 4):
        v = ref.rl(a0 + 16) if field == 16 else ref.rw(a0 + field)
        if v == exit_id:
            tx = ref.rw(a0 + 12) + ref.rw(a0 + 4) // 2
            ty = ref.rw(a0 + 14) + ref.rw(a0 + 6) // 2
            d = 0
            if x < tx - 2: d |= 1
            elif x > tx + 2: d |= 2
            if y < ty - 2: d |= 4
            elif y > ty + 2: d |= 8
            return d or 0x10
        a0 += 24
    return 0x10


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--frames', type=int, default=500)
    ap.add_argument('--seed', type=int, default=1)
    ap.add_argument('--space', type=int, default=0)
    ap.add_argument('--fire', type=float, default=0.0)
    ap.add_argument('--wander', type=int, default=40)
    ap.add_argument('--replay', help='fichier .joy (MOG_SAVE) à rejouer')
    a = ap.parse_args()
    rng = random.Random(a.seed)

    if a.replay:
        os.environ['MOG_VBLTRACE'] = '1'
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
    replay = open(a.replay).read().split('\n') if a.replay else None
    kinds = {}
    in_screen = 0
    screens = screen_frames = 0
    for f in range(1, a.frames + 1):
        if not ok:
            break
        key = None
        pc = ref.uc.reg_read(M.UC_M68K_REG_PC)
        kinds[ref.where(pc)] = kinds.get(ref.where(pc), 0) + 1
        if pc == S['LAB_0E40']:                         # menu des lieux : « 1 »
            key = 1
        if pc in (S['LAB_00EC'], S['LAB_00ED']):        # attente du feu : appui, relâché
            j = [0x10, 0x10] if pc == S['LAB_00EC'] else [0, 0]
        elif pc in (S['LAB_04D0'], S['LAB_008C'], S['LAB_0095'], S['LAB_0496']):
            # écran LAB_04CF, menu de ville, or proposé : errance, clics,
            # puis sortie (zone 7, enseigne 5, bouton « accepter »)
            town = pc != S['LAB_04D0']
            in_screen += 1
            screen_frames += 1
            screens += in_screen == 1
            if in_screen < a.wander:
                d = rng.choice([1, 2, 4, 8, 5, 6, 9, 10])
                joy = d | (0x10 if rng.random() < 0.15 else 0)
            else:
                if pc == S['LAB_0496']:
                    joy = to_exit(ref, 3, 20)
                else:
                    joy = to_exit(ref, 5 if town else 7)
            j = [joy, joy]
        else:
            in_screen = 0
            if not joy or joy & 0x10 or rng.random() < 0.05:
                joy = rng.choice([1, 2, 4, 8, 5, 6, 9, 10])
            j = [0, joy | (0x10 if rng.random() < a.fire else 0)]
            in_combat = ref.where(ref.rl(ref.r('A7'))).startswith('Combat_Loop')
            if key is None and a.space and f % a.space == 0 and not in_combat:
                key = 0x39
        if replay is not None:
            if f > len(replay) or not replay[f - 1]:
                break
            v = [int(x) for x in replay[f - 1].split()[1:]]
            j, key = v[:2], (v[2] if len(v) > 2 else None)
            ref.vbl_trace = [] if f >= a.frames - 2 else None
        if key is not None:
            ref.ww(S['SECSTRT_21'], key)
        if os.environ.get('MOG_TRACE'):
            print('%d écran %d ptr %d,%d joy %s key %s cur %X' % (
                f, ref.uc.reg_read(M.UC_M68K_REG_PC) == S['LAB_04D0'], ref.rw(S['LAB_097F']), ref.rw(S['LAB_0980']),
                j, key, ref.rl(S['LAB_0633'])))
        ref.joy = list(j)
        c.step(j, key)
        if os.environ.get('MOG_SAVE'):
            with open(os.environ['MOG_SAVE'] + '.joy', 'a') as jf:
                jf.write('J %d %d%s\n' % (j[0], j[1], '' if key is None else ' %d' % key))
        try:
            ref.run_frames(1)
        except Stop as ex:
            print('original arrêté : %s' % ex)
            break
        alive = c.wait()
        ok = compare(ref, ref.snapshot(), c.memory(), 'image %d (joy %d)' % (f, joy), c.lines)
        fails += not ok
        if ref.vbl_trace is not None:
            print('VBL orig %d : %s' % (len(ref.vbl_trace), ' '.join(ref.vbl_trace)))
            print('VBL C : %s' % ' '.join(l for l in c.lines if l.startswith('W')))
        frames = f
        for l in c.lines:
            if l.startswith('M '):
                print('C %s' % l)
        if not alive:
            print('C terminé : %s' % c.lines[-3:])
            break
    c.close()
    print('rendez-vous : %s' % ', '.join('%s %d' % kv for kv in sorted(kinds.items())))
    print('%d images comparées (%d dans %d écrans LAB_04CF), %d échecs' % (
        frames, screen_frames, screens, fails))
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
