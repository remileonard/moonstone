#!/usr/bin/env python3
"""
mog_ref.py — le jeu d'origine (mog) exécuté par l'émulateur 68000 Unicorn,
avec le matériel remplacé par des crochets de haut niveau.

Sert de référence pour le portage C : on fait tourner le vrai code (mise en
place d'un combat, contrôleurs, collisions...) sur les vraies données, puis
on compare la mémoire et les appels avec le code C.

Mémoire (espace 24 bits de l'Amiga) :
  0x000000-0x0FFFFF   « chip » : bloc A1 donné par le lanceur (0x5BF18 octets)
  0x100000-...        mog (hunks + relocations, comme game/data/ix_mog.c)
  ensuite             bloc A0 du lanceur (0x5654D octets), pile
  0xBFD000, 0xDFF000  CIA et registres custom : simple RAM

Crochets (routine remplacée par un RTS + fonction Python) :
  fichiers  File_Open / File_Read / File_Close (LAB_0BB5 / 0BD7 / 0BFF)
            lisent le dossier de données ;
  VBL       LAB_0D77 (attente d'une VBL) incrémente v_VblCounter ;
  messages  LAB_0BB3 (traces du jeu) ;
  sons      LAB_0AA2, LAB_0F8C (canal), palette LAB_0D8A (notés dans events) ;
  dessin    LAB_0CDA (noté dans draws).

  python3 tools/mog_ref.py <dossier_données> [--duel | --encounter LAB_0168]
                           [--frames N] [-v]
"""
import os
import re
import struct
import sys

import unicorn as U
import unicorn.m68k_const as M

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(HERE)
sys.path.insert(0, HERE)
import ix_scripts as X  # noqa: E402

CHIP_BLOCK = 0x00008000          # A1 du lanceur (LAB_05BC)
CHIP_SIZE = 0x0005BF18
FAST_SIZE = 0x0005654D           # A0 du lanceur (LAB_05BE)
STACK_SIZE = 0x4000

REGS = {('D%d' % i): getattr(M, 'UC_M68K_REG_D%d' % i) for i in range(8)}
REGS.update({('A%d' % i): getattr(M, 'UC_M68K_REG_A%d' % i) for i in range(8)})


def syms():
    s = {}
    for line in open(os.path.join(ROOT, 'game', 'data', 'ix_mog_syms.h')):
        m = re.match(r'#define MOG_(\w+) 0x([0-9A-F]+)u', line)
        if m:
            s[m.group(1)] = int(m.group(2), 16)
    return s


def load_image():
    """Octets de mog à X.VM_BASE, relocations appliquées."""
    b = X.Binary('mog')
    bases, total = X.hunk_bases(b)
    m = bytearray(total)
    for h, hk in enumerate(b.hunks):
        o = bases[h] - X.VM_BASE
        m[o:o + len(hk['data'])] = hk['data']
    for h, hk in enumerate(b.hunks):
        for pos, t in b.rel[h].items():
            o = bases[h] - X.VM_BASE + pos
            v = struct.unpack('>I', m[o:o + 4])[0] + bases[t]
            m[o:o + 4] = struct.pack('>I', v & 0xFFFFFFFF)
    return m


class Stop(Exception):
    pass


class MogRef:
    def __init__(self, data_dir, log=None):
        self.data_dir = data_dir
        self.log = log or (lambda s: None)
        self.S = syms()
        img = load_image()
        self.end = X.VM_BASE + len(img)
        self.fast = (self.end + 0xFFF) & ~0xFFF
        self.stack_top = ((self.fast + FAST_SIZE + 0xFFF) & ~0xFFF) + STACK_SIZE
        uc = self.uc = U.Uc(U.UC_ARCH_M68K, U.UC_MODE_BIG_ENDIAN)
        uc.ctl_set_cpu_model(M.UC_CPU_M68K_M68000)
        uc.mem_map(0, self.stack_top + 0x1000)
        uc.mem_map(0xBFD000, 0x2000)
        uc.mem_map(0xDFF000, 0x1000)
        uc.mem_write(X.VM_BASE, bytes(img))
        self.messages = []
        self.files = {}                  # nom -> octets
        self.cur = None                  # fichier ouvert : [données, position]
        self.hooks = {}
        self.patched = {}
        S = self.S
        self.hook(S['LAB_0BB5'], self.h_open)
        self.hook(S['LAB_0BD7'], self.h_read)
        self.hook(S['LAB_0BFF'], self.h_close)
        self.hook(S['LAB_0BEA'], self.h_skip)           # avance de D0 octets
        self.hook(S['LAB_0BB4'], lambda: None)          # lecture du répertoire disque
        self.hook(S['LAB_0AC4'], self.h_disk)           # accès disque bas niveau
        self.hook(S['LAB_0D77'], self.h_vbl)
        self.hook(S['LAB_0BB3'], self.h_message)
        self.hook(S['LAB_0AA2'], self.h_sound)
        self.hook(S['LAB_0F8C'], self.h_voice)          # son sur un canal (D1)
        self.hook(S['LAB_0D8A'], self.h_palette)        # palette -> registres couleur
        self.hook(S['LAB_0CDA'], self.h_draw)           # blit d'une frame CEL
        self.hook(S['LAB_0D07'], lambda: None)          # copie de décor (restauration)
        self.draws = []
        self.events = []                 # sons joués (« S n »)
        self.hook(S['LAB_00EE'], self.h_joy)
        self.hook(S['LAB_00EC'], lambda: None)          # attente « appuyez sur feu »
        self.hook(S['LAB_0137'], lambda: None)          # écran de message (texte)
        self.joy = [0, 0]                # bits ports 0 / 1 : 0 D, 1 G, 2 B, 3 H, 4 feu
        self.frame_no = 0
        self.frame_limit = None
        self.on_frame = None
        uc.hook_add(U.UC_HOOK_CODE, self.h_frame, None,
                    begin=S['Combat_FrameStart'], end=S['Combat_FrameStart'])
        self.left_combat = False         # retour à la boucle principale (LAB_0001)
        uc.hook_add(U.UC_HOOK_CODE, self.h_main_loop, None,
                    begin=S['LAB_0001'], end=S['LAB_0001'])
        # adresse de retour de Combat_Run lancé par start_encounter
        self.combat_exit = self.stack_top - 0x40
        uc.mem_write(self.combat_exit, b'\x60\xfe')        # BRA.S *
        uc.hook_add(U.UC_HOOK_CODE, self.h_main_loop, None,
                    begin=self.combat_exit, end=self.combat_exit)
        uc.hook_add(U.UC_HOOK_MEM_UNMAPPED, self.h_unmapped)

    # -- accès -------------------------------------------------------------
    def r(self, reg):
        return self.uc.reg_read(REGS[reg])

    def w(self, reg, v):
        self.uc.reg_write(REGS[reg], v & 0xFFFFFFFF)

    def rl(self, a):
        return struct.unpack('>I', self.uc.mem_read(a, 4))[0]

    def rw(self, a):
        return struct.unpack('>H', self.uc.mem_read(a, 2))[0]

    def rb(self, a):
        return self.uc.mem_read(a, 1)[0]

    def wl(self, a, v):
        self.uc.mem_write(a, struct.pack('>I', v & 0xFFFFFFFF))

    def ww(self, a, v):
        self.uc.mem_write(a, struct.pack('>H', v & 0xFFFF))

    def wb(self, a, v):
        self.uc.mem_write(a, bytes([v & 0xFF]))

    def where(self, pc):
        """Libellé le plus proche avant `pc` (ex. « LAB_0D77+4 »)."""
        best = max((v for v in self.S.values() if v <= pc), default=0)
        name = [k for k, v in self.S.items() if v == best][0]
        return '%s+%X' % (name, pc - best) if pc != best else name

    def backtrace(self, depth=64):
        """Adresses de retour plausibles sur la pile (dans le code de mog)."""
        sp, out = self.r('A7'), []
        top = self.stack_top - 0x100
        while sp < top and len(out) < depth:
            v = self.rl(sp)
            if X.VM_BASE <= v < self.end and v & 1 == 0:
                out.append(self.where(v))
            sp += 2
        return out

    def cstr(self, a, n=64):
        return bytes(self.uc.mem_read(a, n)).split(b'\0')[0].decode('latin-1')

    # -- crochets ----------------------------------------------------------
    def hook(self, addr, fn):
        """Remplace la routine à `addr` par RTS et appelle fn() avant."""
        if addr not in self.patched:
            self.patched[addr] = bytes(self.uc.mem_read(addr, 2))
            self.uc.mem_write(addr, b'\x4e\x75')
            self.uc.hook_add(U.UC_HOOK_CODE, lambda uc, a, sz, u: self.hooks[a](),
                             None, begin=addr, end=addr)
        self.hooks[addr] = fn

    def h_unmapped(self, uc, access, addr, size, value, user):
        self.log('accès non mappé %08X (pc %08X)' % (addr, uc.reg_read(M.UC_M68K_REG_PC)))
        return False

    def find_file(self, name):
        want = name.lower()
        for f in os.listdir(self.data_dir):
            if f.lower() == want:
                return os.path.join(self.data_dir, f)
        return None

    # fichiers ouverts seulement pour vérifier la disquette insérée (LAB_010C-010E)
    DISK_MARKERS = {'be1.c'}

    def h_open(self):                    # File_Open [LAB_0BB5] : nom en A0
        name = self.cstr(self.r('A0'))
        path = self.find_file(name)
        if path is None and name.lower() in self.DISK_MARKERS:
            self.ww(self.S['L23_0001A'], 0)             # témoin de disquette : présent
            self.cur = [b'', 0]
            self.wl(self.S['L23_0000E'], 0)
            return
        if path is None:
            self.log('fichier absent : %s' % name)
            self.ww(self.S['L23_0001A'], 0xFFFF)
            self.cur = None
            return
        self.ww(self.S['L23_0001A'], 0)
        self.cur = [open(path, 'rb').read(), 0]
        self.wl(self.S['L23_0000E'], len(self.cur[0]))     # taille (lue dans le répertoire)
        self.log('ouverture %s (%d octets)' % (name, len(self.cur[0])))

    def h_read(self):                    # File_Read [LAB_0BD7] : D0 octets -> A0
        n, dst = self.r('D0'), self.r('A0')
        if self.cur is None:
            self.w('D0', 0)
            return
        data, pos = self.cur
        chunk = data[pos:pos + n]
        self.uc.mem_write(dst, chunk)
        self.cur[1] = pos + len(chunk)
        self.w('D0', len(chunk))

    def h_skip(self):                    # LAB_0BEA : saute D0 octets du fichier
        if self.cur is not None:
            self.cur[1] += self.r('D0')

    def h_disk(self):
        raise Stop('accès disque bas niveau (LAB_0AC4) depuis %s' % ' < '.join(self.backtrace()))

    def h_joy(self):                     # LAB_00EE : D0 = port 0, D1 = port 1
        self.ww(self.S['LAB_062F'], self.joy[0])
        self.ww(self.S['LAB_0630'], self.joy[1])
        self.w('D0', self.joy[0])
        self.w('D1', self.joy[1])

    def h_frame(self, uc, addr, size, user):   # Combat_FrameStart : nouvelle image
        self.draws = []
        if self.on_frame:
            self.on_frame(self)
        if self.frame_limit is not None and self.frame_no >= self.frame_limit:
            uc.emu_stop()
            return
        self.frame_no += 1

    def h_main_loop(self, uc, addr, size, user):
        if self.frame_limit is not None:        # pendant run_frames : combat fini
            self.left_combat = True
            uc.emu_stop()

    def h_close(self):                   # File_Close [LAB_0BFF]
        self.cur = None

    def h_vbl(self):                     # LAB_0D77 : une VBL
        a = self.S['v_VblCounter']
        self.wl(a, self.rl(a) + 1)

    def h_sound(self):                   # LAB_0AA2 : effet sonore D0
        self.events.append('S %d' % (self.r('D0') & 0xFF))

    def h_draw(self):                    # LAB_0CDA : A0 CEL, D0 frame, D1/D2 x/y
        s16 = lambda v: v - 0x10000 if v & 0x8000 else v
        self.draws.append((self.r('A0'), self.r('D0') & 0xFFFF,
                           s16(self.r('D1') & 0xFFFF), s16(self.r('D2') & 0xFFFF)))

    def h_voice(self):                   # LAB_0F8C : son D0 sur le canal D1
        self.events.append('V %d %d' % (self.r('D1') & 3, self.r('D0') & 0xFFFF))

    def h_palette(self):                 # LAB_0D8A : 32 couleurs depuis A0
        a = self.r('A0')
        self.events.append('P ' + ' '.join('%03X' % self.rw(a + 2 * i) for i in range(32)))

    def h_message(self):
        s = self.cstr(self.r('A0'), 80)
        self.messages.append(s)
        self.log('message : %s' % s)

    # -- exécution ---------------------------------------------------------
    def call(self, addr, until=None, count=50_000_000, **regs):
        """Appelle la routine `addr` (JSR) ; s'arrête à son RTS ou à `until`."""
        sentinel = self.stack_top - 0x10
        sp = self.stack_top - 0x100
        self.wl(sp, sentinel)
        self.w('A7', sp)
        for k, v in regs.items():
            self.w(k, v)
        # `until` d'Unicorn n'est vu qu'en début de bloc traduit : on arrête
        # par un crochet sur l'adresse.
        h = None
        if until is not None:
            h = self.uc.hook_add(U.UC_HOOK_CODE, lambda uc, a, sz, u: uc.emu_stop(),
                                 None, begin=until, end=until)
        try:
            self.uc.emu_start(addr, sentinel, count=count)
        except U.UcError as ex:
            pc = self.uc.reg_read(M.UC_M68K_REG_PC)
            raise Stop('%s à %s (%08X)' % (ex, self.where(pc), pc))
        finally:
            if h is not None:
                self.uc.hook_del(h)
        pc = self.uc.reg_read(M.UC_M68K_REG_PC)
        if pc not in (sentinel, until):
            raise Stop('limite d\'instructions atteinte à %s (%08X)\n  pile : %s' % (
                self.where(pc), pc, ' < '.join(self.backtrace())))

    def run_frames(self, n, start=None):
        """Exécute n images de Combat_Loop ; s'arrête au début de la suivante
        (sur Combat_FrameStart). `start` : adresse de départ (sinon le pc)."""
        self.frame_limit = self.frame_no + n
        pc = start if start is not None else self.uc.reg_read(M.UC_M68K_REG_PC)
        try:
            self.uc.emu_start(pc, 0xFFFFFFFF, count=200_000_000)
        except U.UcError as ex:
            pc = self.uc.reg_read(M.UC_M68K_REG_PC)
            raise Stop('%s à %s (%08X)' % (ex, self.where(pc), pc))
        if self.left_combat:
            return
        if self.frame_no < self.frame_limit:
            pc = self.uc.reg_read(M.UC_M68K_REG_PC)
            raise Stop('image %d : limite d\'instructions à %s' % (self.frame_no, self.where(pc)))

    @property
    def mem_size(self):
        return self.stack_top + 0x1000

    def snapshot(self):
        return bytes(self.uc.mem_read(0, self.mem_size))

    def restore(self, snap):
        self.uc.mem_write(0, snap)

    def boot(self):
        """Début de SECSTRT_0 jusqu'à LAB_0001 (après les chargements), puis
        LAB_0152 / LAB_0156 (tables de scripts, de marche, d'attaques...)."""
        S = self.S
        self.call(S['SECSTRT_0'], until=S['LAB_0001'],
                  A1=CHIP_BLOCK, D1=CHIP_SIZE, A0=self.fast, D0=FAST_SIZE)
        self.call(S['LAB_0152'])
        self.call(S['LAB_0156'])

    # Rencontres du menu de débogage LAB_007D (touche -> routine d'init)
    ENCOUNTERS = ['LAB_0168', 'LAB_019A', 'LAB_018C', 'LAB_0188', 'LAB_0175',
                  'LAB_016A', 'LAB_0192', 'LAB_0196', 'LAB_019E', 'LAB_01A0']

    def start_encounter(self, init):
        """Chevalier 1 (LAB_0613, joystick port 0) contre la rencontre
        `init` (routine de t_CreatureInit), jusqu'à Combat_Loop."""
        S = self.S
        self.left_combat = False
        self.frame_limit = None
        self.call(S['LAB_01AE'])                  # nouvelle partie
        k = S['LAB_0613']
        self.wb(k + 77, 12)                       # Ctl_HumanKnight
        self.wb(k + 11, 1)                        # port 0
        self.wl(k + 54, 0)
        self.wl(S['v_Combatants'], k)
        self.call(S['LAB_020F'])
        self.call(S['LAB_01BE'])
        self.call(S['LAB_0011'])
        self.wl(S['LAB_0633'], k)
        self.call(S[init])
        sp = self.stack_top - 0x100
        self.wl(sp, self.combat_exit)                   # JSR Combat_Run
        self.w('A7', sp)
        self.run_frames(0, start=S['Combat_Run'])

    def start_duel(self, cpu=False):
        """Duel (LAB_0002) jusqu'à Combat_Loop : deux joueurs humains, ou
        (cpu) le second chevalier géré par l'ordinateur (contrôleur 16,
        LAB_0EFF, comme un chevalier non joueur de Combat_StartPvP)."""
        S = self.S
        self.left_combat = False
        self.frame_limit = None
        self.call(S['LAB_0002'], until=S['Combat_Loop'])
        if cpu:
            k = self.rl(S['v_Combatants'] + 4)
            self.wb(k + 77, 0x10)
            self.wl(k + 54, 4)
            for i in range(10):
                en = S['t_Entities'] + 50 * i
                if self.rb(en) and self.rl(en + 24) == k:
                    self.wb(en + 32, 0x10)
        self.run_frames(0, start=S['Combat_Loop'])


def describe(ref):
    """Entités actives : contrôleur, script, position."""
    S, out = ref.S, []
    for i in range(10):
        en = S['t_Entities'] + 50 * i
        if ref.rb(en):
            out.append('ctl%d %s x=%d d=%d' % (ref.rb(en + 32), ref.where(ref.rl(en + 2)),
                                               ref.rw(en + 6), ref.rw(en + 10)))
    return ' | '.join(out)


def main():
    import argparse
    ap = argparse.ArgumentParser(description='mog d\'origine sous émulation')
    ap.add_argument('data')
    ap.add_argument('--duel', action='store_true', help='duel à deux joueurs')
    ap.add_argument('--cpu', action='store_true', help='duel contre un chevalier IA')
    ap.add_argument('--encounter', help='rencontre (%s)' % ', '.join(MogRef.ENCOUNTERS))
    ap.add_argument('--frames', type=int, default=40)
    ap.add_argument('-v', action='store_true', help='journal (fichiers, messages)')
    a = ap.parse_args()
    ref = MogRef(a.data, log=print if a.v else None)
    try:
        ref.boot()
        print('initialisation terminée')
        if a.duel or a.cpu or a.encounter:
            if a.encounter:
                ref.start_encounter(a.encounter)
            else:
                ref.start_duel(cpu=a.cpu)
            print('début :', describe(ref))
            ref.run_frames(a.frames)
            print('image %d :' % ref.frame_no, describe(ref), '(combat fini)' if ref.left_combat else '')
    except Stop as ex:
        print('arrêt : %s' % ex)
        sys.exit(1)


if __name__ == '__main__':
    main()
