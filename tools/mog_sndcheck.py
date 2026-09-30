#!/usr/bin/env python3
"""
mog_sndcheck.py — pilote de sons de mog (game/src/mog_sound.c) comparé à
l'original : chaque son de la table LAB_1098 est lancé (LAB_0F8C) sur une
voie, puis joué VBL par VBL (LAB_0F73 : séquenceur, enveloppes, registres)
par l'original (tools/mog_ref.py) et par le C (build/tests/mog_run snd),
depuis la même mémoire. Après chaque pas : voies (SECSTRT_44...), variables
du pilote (L44_00BEE...LAB_0FCA) et registres Paula (LC, LEN, PER, VOL).
L'interruption audio (LAB_0F6F) est appelée des deux côtés toutes les 7 VBL
sur la voie du son (sa cadence réelle dépend du mixage).

  python3 tools/mog_sndcheck.py <dossier_données> [--vbls N] [--sounds a-b]
"""
import argparse
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'tests', 'mog_run')


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('data')
    ap.add_argument('--vbls', type=int, default=80)
    ap.add_argument('--sounds', default='0-167')
    a = ap.parse_args()
    lo, hi = (int(x, 0) for x in a.sounds.split('-'))

    ref = MogRef(a.data)
    ref.boot()
    S = ref.S
    f8c = S['LAB_0F8C']                             # routine réelle, plus de crochet
    ref.uc.mem_write(f8c, ref.patched[f8c])
    ref.uc.ctl_remove_cache(f8c, f8c + 4)          # code déjà traduit (RTS)
    ref.hooks[f8c] = lambda: None
    ref.call(S['LAB_0F89'])

    d = tempfile.mkdtemp()
    mem = os.path.join(d, 'm.bin')
    open(mem, 'wb').write(ref.snapshot())
    p = subprocess.Popen([RUN, mem, 'snd', '-', a.data], stdin=subprocess.PIPE,
                         stdout=subprocess.PIPE, text=True)

    def c(cmd):
        p.stdin.write(cmd + '\n')
        p.stdin.flush()
        return p.stdout.readline().strip() if cmd[0] in 'DR' else None

    zones = [('voies', S['SECSTRT_44'], 4 * 148),
             ('pilote', S['L44_00BEE'], S['LAB_0FCA'] + 2 - S['L44_00BEE'])]

    def check(what):
        for name, addr, n in zones:
            o = bytes(ref.uc.mem_read(addr, n)).hex()
            cm = c('D %x %d' % (addr, n))
            if o != cm:
                i = next(k for k in range(0, len(o), 2) if o[k:k + 2] != cm[k:k + 2]) // 2
                print('ÉCHEC %s : %s, 1re différence %s (orig %s, C %s)' % (
                    what, name, ref.where(addr + i), o[2 * i:2 * i + 8], cm[2 * i:2 * i + 8]))
                return False
        regs = ' '.join('%08x %04x %04x %04x' % (
            ref.rl(0xDFF0A0 + 16 * k) & ~1, ref.rw(0xDFF0A4 + 16 * k),   # Paula : bit 0 ignoré
            ref.rw(0xDFF0A6 + 16 * k), ref.rw(0xDFF0A8 + 16 * k)) for k in range(4))
        cr = c('R')
        if regs != cr:
            print('ÉCHEC %s : registres\n  orig %s\n  C    %s' % (what, regs, cr))
            return False
        return True

    steps = 0
    for n in range(lo, hi + 1):
        ch = n % 4
        ref.call(f8c, D0=n, D1=ch)
        c('P %d %d' % (n, ch))
        if not check('son %d voie %d (départ)' % (n, ch)):
            sys.exit(1)
        for v in range(a.vbls):
            ref.call(S['LAB_0F73'])
            c('V')
            steps += 1
            if v % 7 == 6:                          # interruption audio de la voie
                ref.call(S['LAB_0F6F'], A0=0xDFF000,
                         A1=S[['SECSTRT_44', 'LAB_0F66', 'LAB_0F67', 'LAB_0F68'][ch]])
                c('I %d' % ch)
            if not check('son %d voie %d, VBL %d' % (n, ch, v + 1)):
                sys.exit(1)
    c('Q')
    print('%d sons, %d VBL : identiques (voies, pilote, registres)' % (hi - lo + 1, steps))


if __name__ == '__main__':
    main()
