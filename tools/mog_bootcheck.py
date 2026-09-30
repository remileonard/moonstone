#!/usr/bin/env python3
"""
mog_bootcheck.py — compare la mémoire préparée par game/src/mog_boot.c
(build/tests/mog_boot_dump) à celle de mog d'origine après son
initialisation (tools/mog_ref.py : SECSTRT_0 jusqu'à LAB_0001, puis
LAB_0152 / LAB_0156), zone par zone.

  python3 tools/mog_bootcheck.py <dossier_données>
"""
import os
import subprocess
import sys
import tempfile

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
from mog_ref import MogRef  # noqa: E402

DUMP = os.path.join(ROOT, 'build', 'tests', 'mog_boot_dump')

# (libellé, longueur) ou (libellé, libellé de fin exclu) : zones préparées
# par mog_boot.c
ZONES = [
    ('LAB_05B8', 40), ('LAB_05B9', 96),                 # blocs du lanceur (LAB_0004)
    ('LAB_05BB', 4), ('LAB_05BC', 4), ('LAB_05BE', 4), ('LAB_05C0', 4),
    ('LAB_05C1', 4), ('LAB_05C2', 4), ('LAB_05C3', 4), ('LAB_05C6', 4),
    ('LAB_05C7', 4), ('LAB_05C8', 4), ('LAB_05C9', 4), ('LAB_05CA', 4),
    ('LAB_05CB', 4), ('LAB_0664', 4), ('SECSTRT_14', 4), ('LAB_0A83', 4),
    ('t_IxOpcodes', 84), ('LAB_0647', 20), ('LAB_0648', 20),   # LAB_0303
    ('LAB_063E', 4), ('LAB_063F', 4),
    ('LAB_05F5', 'LAB_0613'),                           # LAB_0152 / LAB_0156
    ('LAB_05E1', 20), ('SECSTRT_10', 4),                # LAB_0115, Col_InitHitFile
    ('LAB_0A4D', 16), ('t_HitDataByCel', 64),
    ('LAB_0CC9', 8), ('LAB_0D1C', 10),
]


def cel_zones(ref):
    """Zones des CEL chargées : de la première banque du chevalier à la
    fin de kn4, blo.cel, points d'impact de kn4."""
    S = ref.S
    z = []
    kn = [ref.rl(S['LAB_05E1'] + 4 * i) for i in range(5)]
    z.append(('kn1..kn3', kn[0], kn[3]))
    z.append(('kn4', kn[3], kn[3] + 0x3000))
    b = ref.rl(S['LAB_05BB'])
    z.append(('blo.cel', b, b + 0x1000))
    h = ref.rl(S['LAB_05B9'] + 88)
    z.append(('points kn4', h, ref.rl(S['LAB_0A4D'])))
    return z


def main():
    data = sys.argv[1]
    ref = MogRef(data)
    ref.boot()
    orig = ref.snapshot()
    with tempfile.NamedTemporaryFile(delete=False) as f:
        path = f.name
    print(subprocess.run([DUMP, path, data], capture_output=True, text=True).stdout.strip())
    c = open(path, 'rb').read()
    os.unlink(path)
    S = ref.S
    bad = 0
    for z in ZONES:
        a = S[z[0]]
        b = a + z[1] if isinstance(z[1], int) else S[z[1]]
        diffs = [i for i in range(a, b) if orig[i] != c[i]]
        print('%-12s %6d octets : %s' % (z[0], b - a, 'identique' if not diffs else
              '%d différences, 1re à %s (orig %02X, C %02X)' % (
                  len(diffs), ref.where(diffs[0]), orig[diffs[0]], c[diffs[0]])))
        bad += bool(diffs)
    for name, a, b in cel_zones(ref):
        diffs = [i for i in range(a, b) if orig[i] != c[i]]
        print('%-12s %6d octets : %s' % (name, b - a, 'identique' if not diffs else
              '%d différences, 1re à %08X (orig %02X, C %02X)' % (
                  len(diffs), diffs[0], orig[diffs[0]], c[diffs[0]])))
        bad += bool(diffs)
    sys.exit(1 if bad else 0)


if __name__ == '__main__':
    main()
