/*
 * mog_boot.c — mise en place de la mémoire de mog au démarrage (partie
 * utile au combat), traduite de amiga_asm/mog.asm.
 *
 * Disposition (comme tools/mog_ref.py, qui sert de référence) :
 *   0x00008000  bloc « chip » donné par le lanceur (A1, 0x5BF18 octets)
 *   0x00100000  mog (hunks + relocations)
 *   ensuite     bloc « fast » du lanceur (A0, 0x5654D octets)
 */
#include "mog_boot.h"
#include "ix_mog_syms.h"
#include "moon_assets.h"

#include <stdlib.h>
#include <string.h>

#define VM (vm)

static void wl(IxVM *vm, uint32_t a, uint32_t v) { ix_wl(vm, a, v); }

static void fill(IxVM *vm, uint32_t a, uint8_t v, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        ix_wb(vm, a + i, v);
}

static void copy(IxVM *vm, uint32_t dst, uint32_t src, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        ix_wb(vm, dst + i, ix_rb(vm, src + i));
}

/* LAB_0004 : découpage des deux blocs du lanceur (tampons d'écran, de
 * décor, de CEL, de sons...) ; pointeurs dans LAB_05B8 et LAB_05B9. */
static void partition(IxVM *vm, uint32_t chip, uint32_t fast)
{
    uint32_t d0 = chip, a0 = MOG_LAB_05B8;
    ix_wl(VM, MOG_LAB_05BC, chip + 0x5BF18);
    ix_wl(VM, a0 + 0, d0);
    ix_wl(VM, MOG_LAB_05C0, d0);
    d0 += 0x9C40;
    ix_wl(VM, a0 + 12, d0);
    ix_wl(VM, MOG_LAB_05C7, d0);
    d0 += 0x59D8;
    ix_wl(VM, a0 + 4, d0);
    d0 += 0x11D28;
    ix_wl(VM, a0 + 8, d0);
    ix_wl(VM, MOG_LAB_05CA, d0 + 0x9C40);
    ix_wl(VM, MOG_LAB_05CB, d0 + 0xEA60);
    d0 += 0x13880;
    ix_wl(VM, a0 + 28, d0);
    ix_wl(VM, MOG_LAB_05C8, d0);
    d0 += 0xC350;
    ix_wl(VM, a0 + 36, d0);
    ix_wl(VM, MOG_LAB_0664, d0);
    d0 += 0x3A98;
    ix_wl(VM, a0 + 16, d0);
    ix_wl(VM, MOG_LAB_05C1, d0);
    d0 += 0x9C40;
    ix_wl(VM, MOG_LAB_05C9, d0);

    d0 = fast;
    a0 = MOG_LAB_05B9;
    ix_wl(VM, MOG_LAB_05BE, fast + 0x5654D);
    ix_wl(VM, a0 + 0, d0);
    ix_wl(VM, MOG_LAB_05C2, d0);
    ix_wl(VM, a0 + 44, d0);
    static const struct { uint32_t add; uint8_t off; } seq[] = {
        { 0xC350, 8 }, { 0x5958, 12 }, { 0x5149, 20 }, { 0x4658, 16 },
        { 0x3A55, 24 }, { 0x6395, 32 }, { 0x51C4, 36 }, { 0x4C0A, 28 },
        { 0x51A5, 92 },
    };
    for (unsigned i = 0; i < sizeof seq / sizeof seq[0]; i++) {
        d0 += seq[i].add;
        ix_wl(VM, a0 + seq[i].off, d0);
    }
    d0 += 0x8A03 + 1;
    ix_wl(VM, a0 + 48, d0);
    d0 += 0x67F8;
    ix_wl(VM, a0 + 40, d0);
    d0 += 0x6328;
    ix_wl(VM, a0 + 52, d0);
    d0 += 0x0E6E;
    ix_wl(VM, a0 + 56, d0);
    d0 += 0x25F6;
    ix_wl(VM, a0 + 68, d0);
    ix_wl(VM, MOG_LAB_05C6, d0);
    d0 += 0x1E0;
    ix_wl(VM, a0 + 72, d0);
    d0 += 0x240;
    ix_wl(VM, a0 + 84, d0);
    d0 += 0x2328;
    ix_wl(VM, a0 + 88, d0);
    d0 += 0x2328;
    ix_wl(VM, MOG_LAB_05BB, d0);
    d0 += 0x2710;
    ix_wl(VM, MOG_SECSTRT_14, d0);
    d0 += 0x960;
    ix_wl(VM, MOG_LAB_0A83, d0);
    ix_wl(VM, MOG_LAB_05C3, d0);
}

/* LAB_0152 / LAB_0156 : tables de l'objet (attaques 34, scripts 30,
 * dégâts 42, marche 46, 50) de chaque type de combattant.
 * Transcription directe des écritures d'origine. */
void mog_boot_tables(IxVM *vm)
{
    /* LAB_0152 */
    fill(vm, MOG_LAB_060C + 0, 0, 672);
    fill(vm, MOG_LAB_05F5 + 0, 0, 828);
    wl(vm, MOG_LAB_05F5 + 4, MOG_LAB_07EF);
    wl(vm, MOG_LAB_05F5 + 8, MOG_LAB_07ED);
    wl(vm, MOG_LAB_05F5 + 32, MOG_LAB_07EE);
    wl(vm, MOG_LAB_05F5 + 24, MOG_LAB_07F1);
    wl(vm, MOG_LAB_05F5 + 20, MOG_LAB_07E9);
    wl(vm, MOG_LAB_05F5 + 12, MOG_LAB_07EA);
    wl(vm, MOG_LAB_05F5 + 0, MOG_LAB_07DB);
    wl(vm, MOG_LAB_05F5 + 28, MOG_LAB_07F3);
    wl(vm, MOG_LAB_05F5 + 16, MOG_LAB_07F4);
    wl(vm, MOG_LAB_05F6 + 4, MOG_LAB_07F6);
    wl(vm, MOG_LAB_05F6 + 8, MOG_LAB_07F6);
    wl(vm, MOG_LAB_05F6 + 32, MOG_LAB_07F5);
    wl(vm, MOG_LAB_05F6 + 12, MOG_LAB_07F5);
    wl(vm, MOG_LAB_05F6 + 20, MOG_LAB_07F6);
    wl(vm, MOG_LAB_05F6 + 0, MOG_LAB_07DB);
    wl(vm, MOG_LAB_05F6 + 24, MOG_LAB_07F5);
    wl(vm, MOG_LAB_05F6 + 28, MOG_LAB_07DC);
    wl(vm, MOG_LAB_05F6 + 16, MOG_LAB_07DC);
    wl(vm, MOG_LAB_05F7 + 8, 0x4);
    wl(vm, MOG_LAB_05F7 + 20, 0x2);
    wl(vm, MOG_LAB_05F7 + 4, 0x3);
    wl(vm, MOG_LAB_05F7 + 32, 0x4);
    wl(vm, MOG_LAB_05F7 + 12, 0x3);
    wl(vm, MOG_LAB_05F7 + 24, 0x3);
    wl(vm, MOG_LAB_0610 + 0, MOG_LAB_07DD);
    wl(vm, MOG_LAB_0610 + 4, MOG_LAB_07DE);
    wl(vm, MOG_LAB_0610 + 8, MOG_LAB_07DF);
    wl(vm, MOG_LAB_0610 + 12, MOG_LAB_07E0);
    wl(vm, MOG_LAB_0610 + 16, 0x0);
    wl(vm, MOG_LAB_0610 + 32, MOG_LAB_07E1);
    wl(vm, MOG_LAB_0610 + 36, MOG_LAB_07E2);
    wl(vm, MOG_LAB_0610 + 40, MOG_LAB_07E3);
    wl(vm, MOG_LAB_0610 + 44, MOG_LAB_07E4);
    wl(vm, MOG_LAB_0610 + 48, 0x0);
    wl(vm, MOG_LAB_0610 + 64, MOG_LAB_07E5);
    wl(vm, MOG_LAB_0610 + 68, MOG_LAB_07E6);
    wl(vm, MOG_LAB_0610 + 72, MOG_LAB_07E7);
    wl(vm, MOG_LAB_0610 + 76, MOG_LAB_07E8);
    wl(vm, MOG_LAB_0610 + 80, 0x0);
    wl(vm, MOG_LAB_05F8 + 32, 0x1C);
    wl(vm, MOG_LAB_05F8 + 8, 0x10);
    wl(vm, MOG_LAB_05F8 + 4, 0x1C);
    wl(vm, MOG_LAB_05F8 + 20, 0x1C);
    /* LAB_0156 */
    wl(vm, MOG_LAB_05F9 + 8, MOG_LAB_0815);
    wl(vm, MOG_LAB_05F9 + 32, MOG_LAB_0814);
    wl(vm, MOG_LAB_05F9 + 4, MOG_LAB_0816);
    wl(vm, MOG_LAB_05F9 + 12, MOG_LAB_0814);
    wl(vm, MOG_LAB_05F9 + 20, MOG_LAB_0816);
    wl(vm, MOG_LAB_05F9 + 24, MOG_LAB_0814);
    wl(vm, MOG_LAB_05F9 + 28, MOG_LAB_0815);
    wl(vm, MOG_LAB_05F9 + 16, MOG_LAB_0815);
    wl(vm, MOG_LAB_05F9 + 0, MOG_LAB_0815);
    wl(vm, MOG_LAB_060D + 0, MOG_LAB_081D);
    wl(vm, MOG_LAB_060D + 4, MOG_LAB_081E);
    wl(vm, MOG_LAB_060D + 8, MOG_LAB_081F);
    wl(vm, MOG_LAB_060D + 12, 0x0);
    wl(vm, MOG_LAB_060D + 32, MOG_LAB_0820);
    wl(vm, MOG_LAB_060D + 36, MOG_LAB_0821);
    wl(vm, MOG_LAB_060D + 40, MOG_LAB_0822);
    wl(vm, MOG_LAB_060D + 44, MOG_LAB_0823);
    wl(vm, MOG_LAB_060D + 48, 0x0);
    wl(vm, MOG_LAB_060D + 64, MOG_LAB_0824);
    wl(vm, MOG_LAB_060D + 68, MOG_LAB_0825);
    wl(vm, MOG_LAB_060D + 72, MOG_LAB_0826);
    wl(vm, MOG_LAB_060D + 76, MOG_LAB_0827);
    wl(vm, MOG_LAB_060D + 80, 0x0);
    wl(vm, MOG_LAB_05FB + 8, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FB + 24, MOG_LAB_0802);
    wl(vm, MOG_LAB_05FB + 20, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FB + 12, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FB + 32, MOG_LAB_0802);
    wl(vm, MOG_LAB_05FB + 4, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FB + 28, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FB + 16, MOG_LAB_0801);
    wl(vm, MOG_LAB_05FC + 8, MOG_LAB_080F);
    wl(vm, MOG_LAB_05FC + 32, MOG_LAB_080E);
    wl(vm, MOG_LAB_05FC + 4, MOG_LAB_0810);
    wl(vm, MOG_LAB_05FC + 12, MOG_LAB_080E);
    wl(vm, MOG_LAB_05FC + 20, MOG_LAB_0810);
    wl(vm, MOG_LAB_05FC + 24, MOG_LAB_080E);
    wl(vm, MOG_LAB_05FA + 8, 0x3);
    wl(vm, MOG_LAB_05FA + 20, 0x3);
    wl(vm, MOG_LAB_05FA + 4, 0x3);
    wl(vm, MOG_LAB_05FA + 32, 0x3);
    wl(vm, MOG_LAB_05FA + 12, 0x3);
    wl(vm, MOG_LAB_05FA + 24, 0x3);
    wl(vm, MOG_LAB_060C + 0, MOG_LAB_0803);
    wl(vm, MOG_LAB_060C + 4, MOG_LAB_0804);
    wl(vm, MOG_LAB_060C + 8, MOG_LAB_0805);
    wl(vm, MOG_LAB_060C + 12, 0x0);
    wl(vm, MOG_LAB_060C + 32, MOG_LAB_0806);
    wl(vm, MOG_LAB_060C + 36, MOG_LAB_0807);
    wl(vm, MOG_LAB_060C + 40, MOG_LAB_0808);
    wl(vm, MOG_LAB_060C + 44, MOG_LAB_0809);
    wl(vm, MOG_LAB_060C + 48, 0x0);
    wl(vm, MOG_LAB_060C + 64, MOG_LAB_080A);
    wl(vm, MOG_LAB_060C + 68, MOG_LAB_080B);
    wl(vm, MOG_LAB_060C + 72, MOG_LAB_080C);
    wl(vm, MOG_LAB_060C + 76, MOG_LAB_080D);
    wl(vm, MOG_LAB_060C + 80, 0x0);
    wl(vm, MOG_LAB_05FE + 8, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FE + 24, MOG_LAB_082A);
    wl(vm, MOG_LAB_05FE + 20, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FE + 12, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FE + 32, MOG_LAB_082A);
    wl(vm, MOG_LAB_05FE + 4, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FE + 28, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FE + 16, MOG_LAB_0829);
    wl(vm, MOG_LAB_05FF + 8, MOG_LAB_0837);
    wl(vm, MOG_LAB_05FF + 32, MOG_LAB_0836);
    wl(vm, MOG_LAB_05FF + 4, MOG_LAB_0838);
    wl(vm, MOG_LAB_05FF + 12, MOG_LAB_0836);
    wl(vm, MOG_LAB_05FF + 20, MOG_LAB_0838);
    wl(vm, MOG_LAB_05FF + 24, MOG_LAB_0836);
    wl(vm, MOG_LAB_05FD + 8, 0x2);
    wl(vm, MOG_LAB_05FD + 20, 0x2);
    wl(vm, MOG_LAB_05FD + 4, 0x2);
    wl(vm, MOG_LAB_05FD + 32, 0x2);
    wl(vm, MOG_LAB_05FD + 12, 0x2);
    wl(vm, MOG_LAB_05FD + 24, 0x2);
    wl(vm, MOG_LAB_060E + 0, MOG_LAB_082B);
    wl(vm, MOG_LAB_060E + 4, MOG_LAB_082C);
    wl(vm, MOG_LAB_060E + 8, MOG_LAB_082D);
    wl(vm, MOG_LAB_060E + 12, 0x0);
    wl(vm, MOG_LAB_060E + 32, MOG_LAB_082E);
    wl(vm, MOG_LAB_060E + 36, MOG_LAB_082F);
    wl(vm, MOG_LAB_060E + 40, MOG_LAB_0830);
    wl(vm, MOG_LAB_060E + 44, MOG_LAB_0831);
    wl(vm, MOG_LAB_060E + 48, 0x0);
    wl(vm, MOG_LAB_060E + 64, MOG_LAB_0832);
    wl(vm, MOG_LAB_060E + 68, MOG_LAB_0833);
    wl(vm, MOG_LAB_060E + 72, MOG_LAB_0834);
    wl(vm, MOG_LAB_060E + 76, MOG_LAB_0835);
    wl(vm, MOG_LAB_060E + 80, 0x0);
    wl(vm, MOG_LAB_0600 + 8, MOG_LAB_0847);
    wl(vm, MOG_LAB_0600 + 32, MOG_LAB_0845);
    wl(vm, MOG_LAB_0600 + 4, MOG_LAB_0847);
    wl(vm, MOG_LAB_0600 + 12, MOG_LAB_0847);
    wl(vm, MOG_LAB_0600 + 20, MOG_LAB_0847);
    wl(vm, MOG_LAB_0600 + 24, MOG_LAB_0845);
    wl(vm, MOG_LAB_0600 + 16, MOG_LAB_0840);
    copy(vm, MOG_LAB_0611 + 0, MOG_LAB_015E + 0, 20);
    copy(vm, MOG_LAB_0611 + 32, MOG_LAB_015E + 20, 20);
    wl(vm, MOG_LAB_0602 + 8, MOG_LAB_086A);
    wl(vm, MOG_LAB_0602 + 4, MOG_LAB_0862);
    wl(vm, MOG_LAB_0602 + 20, MOG_LAB_0862);
    wl(vm, MOG_LAB_0602 + 24, MOG_LAB_0862);
    wl(vm, MOG_LAB_0602 + 12, MOG_LAB_0862);
    wl(vm, MOG_LAB_0602 + 32, MOG_LAB_0860);
    wl(vm, MOG_LAB_0602 + 28, MOG_LAB_0860);
    wl(vm, MOG_LAB_0602 + 16, MOG_LAB_0860);
    wl(vm, MOG_LAB_0612 + 32, MOG_LAB_0852);
    wl(vm, MOG_LAB_0612 + 36, MOG_LAB_0853);
    wl(vm, MOG_LAB_0612 + 40, MOG_LAB_0854);
    wl(vm, MOG_LAB_0612 + 44, MOG_LAB_0855);
    wl(vm, MOG_LAB_0612 + 48, 0x0);
    wl(vm, MOG_LAB_0612 + 0, MOG_LAB_086C);
    wl(vm, MOG_LAB_0612 + 4, MOG_LAB_086D);
    wl(vm, MOG_LAB_0612 + 8, MOG_LAB_086E);
    wl(vm, MOG_LAB_0612 + 12, MOG_LAB_086F);
    wl(vm, MOG_LAB_0612 + 16, 0x0);
    for (int i = 0; i < 8; i++) wl(vm, MOG_LAB_0605 + 0 + 4u * (unsigned)i, 0x4);
    for (int i = 0; i < 8; i++) wl(vm, MOG_LAB_0609 + 0 + 4u * (unsigned)i, 0x3);
    for (int i = 0; i < 8; i++) wl(vm, MOG_LAB_060A + 0 + 4u * (unsigned)i, MOG_LAB_060A);
    wl(vm, MOG_LAB_060B + 0, MOG_LAB_08A6);
    wl(vm, MOG_LAB_060B + 4, MOG_LAB_08A7);
    wl(vm, MOG_LAB_060B + 8, MOG_LAB_08A8);
    wl(vm, MOG_LAB_060B + 12, MOG_LAB_08A9);
    wl(vm, MOG_LAB_060B + 16, 0x0);
    for (int i = 0; i < 8; i++) wl(vm, MOG_LAB_0607 + 0 + 4u * (unsigned)i, MOG_LAB_08A3);
    for (int i = 0; i < 8; i++) wl(vm, MOG_LAB_0606 + 0 + 4u * (unsigned)i, 0x2);
    wl(vm, MOG_LAB_0608 + 0, MOG_LAB_089C);
    wl(vm, MOG_LAB_0608 + 4, MOG_LAB_089B);
    wl(vm, MOG_LAB_0608 + 8, MOG_LAB_089C);
    wl(vm, MOG_LAB_0608 + 12, MOG_LAB_089D);
    wl(vm, MOG_LAB_0608 + 16, 0x0);
    wl(vm, MOG_LAB_0608 + 20, 0x0);
    wl(vm, MOG_LAB_0608 + 24, 0x0);
    wl(vm, MOG_LAB_0608 + 28, 0x0);
    wl(vm, MOG_LAB_0604 + 8, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 4, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 20, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 24, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 12, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 32, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 28, MOG_LAB_087F);
    wl(vm, MOG_LAB_0604 + 16, MOG_LAB_087F);
    wl(vm, MOG_LAB_060F + 32, MOG_LAB_0873);
    wl(vm, MOG_LAB_060F + 36, MOG_LAB_0874);
    wl(vm, MOG_LAB_060F + 40, MOG_LAB_0875);
    wl(vm, MOG_LAB_060F + 44, MOG_LAB_0876);
    wl(vm, MOG_LAB_060F + 48, MOG_LAB_0877);
    wl(vm, MOG_LAB_060F + 52, MOG_LAB_0877);
    wl(vm, MOG_LAB_060F + 56, MOG_LAB_0877);
    wl(vm, MOG_LAB_060F + 60, MOG_LAB_0877);
    wl(vm, MOG_LAB_060F + 64, MOG_LAB_0879);
    wl(vm, MOG_LAB_060F + 68, MOG_LAB_087A);
    wl(vm, MOG_LAB_060F + 72, MOG_LAB_087B);
    wl(vm, MOG_LAB_060F + 76, MOG_LAB_087C);
    wl(vm, MOG_LAB_060F + 80, MOG_LAB_087D);
    wl(vm, MOG_LAB_060F + 84, MOG_LAB_087D);
    wl(vm, MOG_LAB_060F + 88, MOG_LAB_087D);
    wl(vm, MOG_LAB_060F + 92, MOG_LAB_087D);

}

/* LAB_0303 : banques CEL (LAB_0647, LAB_0648 = blo.cel), table des
 * opcodes du moteur, tampons de restauration du décor. */
void mog_boot_engine(IxVM *vm)
{
    uint32_t a0 = MOG_LAB_0647;
    wl(vm, a0 + 0, MOG_LAB_05E1);
    wl(vm, a0 + 4, MOG_LAB_05E0);
    wl(vm, a0 + 8, MOG_LAB_05E2);
    wl(vm, a0 + 12, MOG_LAB_0648);
    wl(vm, a0 + 16, 0);
    for (int i = 0; i < 5; i++)
        wl(vm, MOG_LAB_0648 + 4u * (unsigned)i, ix_rl(vm, MOG_LAB_05BB));
    static const struct { uint8_t off; uint32_t fn; } ops[] = {
        { 0, MOG_IxOp80_SetDir }, { 4, MOG_IxOp84_Jump }, { 8, MOG_IxOp88_Hold },
        { 12, MOG_IxOp8C_Physics }, { 20, MOG_IxOp94_Loop }, { 24, MOG_IxOp98_SkipIfDebug },
        { 28, MOG_IxOp9C_Nop }, { 36, MOG_IxOpA4_Sound }, { 32, MOG_IxOpA0_Move },
        { 44, MOG_IxOpAC_Shadow }, { 40, MOG_IxOpA8_SetField }, { 48, MOG_IxOpB0_Call },
        { 68, MOG_IxOpC4_IfSameFacing }, { 52, MOG_IxOpB4_IfDead }, { 56, MOG_IxOpB8_Spawn },
        { 60, MOG_IxOpBC_Kill }, { 64, MOG_IxOpC0_SetBank }, { 72, MOG_IxOpC8_IfFieldZero },
        { 76, MOG_IxOpCC_IfFieldNonZero }, { 80, MOG_IxOpD0_Reset },
    };
    for (unsigned i = 0; i < sizeof ops / sizeof ops[0]; i++)
        wl(vm, MOG_t_IxOpcodes + ops[i].off, ops[i].fn);
    wl(vm, MOG_LAB_063F, MOG_LAB_064D);
    wl(vm, MOG_LAB_063E, MOG_LAB_064E);
}

/* ------------------------------------------------------------------ */
/* Fichiers (LAB_0BB5 / LAB_0BD7 / LAB_0BFF)                           */
/* ------------------------------------------------------------------ */

typedef struct {
    uint8_t *data;
    size_t   len, pos;
} MogFile;

/* Ouvre le fichier dont le nom (chaîne en mémoire) est à `name`.
 * L23_0001A = 0 / -1, L23_0000E = taille (lue dans le répertoire). */
static int file_open(IxVM *vm, uint32_t name, MogFile *f)
{
    char n[64];
    unsigned i;
    for (i = 0; i < sizeof n - 1 && ix_rb(vm, name + i); i++)
        n[i] = (char)ix_rb(vm, name + i);
    n[i] = 0;
    f->pos = 0;
    f->data = moon_file_read(n, &f->len);
    if (!f->data) {
        f->len = 0;
        ix_ww(vm, MOG_L23_0001A, 0xFFFF);
        return -1;
    }
    ix_ww(vm, MOG_L23_0001A, 0);
    ix_wl(vm, MOG_L23_0000E, (uint32_t)f->len);
    return 0;
}

/* Lit n octets à l'adresse dst ; renvoie le nombre lu. */
static uint32_t file_read(IxVM *vm, MogFile *f, uint32_t dst, uint32_t n)
{
    uint32_t k = 0;
    for (; k < n && f->pos < f->len; k++)
        ix_wb(vm, dst + k, f->data[f->pos++]);
    return k;
}

static void file_close(MogFile *f)
{
    free(f->data);
    f->data = NULL;
}

/* LAB_0CC2 : décompression (LZSS Mindscape) de n octets de src vers dst ;
 * renvoie le nombre d'octets écrits. Un octet de contrôle pour 8 jetons
 * (bit à 1 : copie arrière de 34 - (mot >> 11) octets depuis sortie -
 * (mot & $7FF) ; bit à 0 : octet littéral) ; fin testée avant chaque jeton
 * (vérifié contre l'original par tools/mog_bootcheck.py). */
static uint32_t unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst)
{
    uint32_t end = src + n, out = dst;
    for (;;) {
        uint8_t ctl = ix_rb(vm, src++);                 /* LAB_0CC3 */
        int d3 = 8;                                     /* 8 jetons */
        for (;;) {                                      /* LAB_0CC7 : DBCC */
            if (!(src < end))
                return out - dst;
            if (--d3 == -1)
                break;
            int ref = ctl & 0x80;                       /* LAB_0CC4 */
            ctl = (uint8_t)(ctl << 1);
            if (!ref) {
                ix_wb(vm, out++, ix_rb(vm, src++));     /* LAB_0CC6 */
                continue;
            }
            uint16_t w = (uint16_t)(ix_rb(vm, src) << 8 | ix_rb(vm, src + 1));
            src += 2;
            uint32_t from = out - (w & 0x07FF);
            unsigned len = 34u - (w >> 11);
            for (unsigned k = 0; k < len; k++)
                ix_wb(vm, out++, ix_rb(vm, from++));
        }
    }
}

/* LAB_0CBB : charge la CEL `name` à `dest` : en-tête (10 octets), table
 * des frames, pixels décompressés ; 2(dest) = adresse des pixels. */
void mog_load_cel(IxVM *vm, uint32_t name, uint32_t dest)
{
    MogFile f;
    ix_wl(vm, MOG_LAB_0CC9, name);
    ix_wl(vm, MOG_LAB_0CCA, dest);
    file_open(vm, name, &f);
    file_read(vm, &f, dest, 10);
    copy(vm, MOG_LAB_0D1C, dest, 10);
    uint32_t table = (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u;
    uint32_t data = dest + 10 + table;
    file_read(vm, &f, dest + 10, table);
    ix_wl(vm, dest + 2, data);
    uint32_t packed = ix_rl(vm, MOG_LAB_0D1D);
    file_read(vm, &f, data, packed);
    file_close(&f);
    ix_ww(vm, MOG_LAB_0D4D, 1);
    copy(vm, MOG_LAB_0D4F, data, packed);
    uint32_t pix = dest + 10 + (uint32_t)(int32_t)(int16_t)table;
    ix_wl(vm, dest + 2, pix);
    uint32_t n = unpack(vm, MOG_LAB_0D4F, packed, pix);
    uint32_t d1 = (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u;
    d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + 10);
    ix_wl(vm, MOG_LAB_0CCA, n + d1);
}

/* LAB_0CB6 : place occupée par la CEL `name` une fois chargée. */
uint32_t mog_cel_size(IxVM *vm, uint32_t name)
{
    if (name == ix_rl(vm, MOG_LAB_0CC9))                /* LAB_0CB7 */
        return (ix_rl(vm, MOG_LAB_0CCA) + 1) & 0xFFFFFFFEu;
    MogFile f;
    file_open(vm, name, &f);
    file_read(vm, &f, MOG_LAB_0D1C, 10);
    file_close(&f);
    return (ix_rl(vm, MOG_LAB_0D1F) >> 3) + 0x168 + (uint32_t)ix_rw(vm, MOG_LAB_0D1C) * 10u + 10;
}

/* Col_InitHitFile [LAB_03DA] : collide.hit (texte) en mémoire */
void mog_hit_init(IxVM *vm)
{
    ix_wl(vm, MOG_LAB_0A4D, ix_rl(vm, MOG_LAB_05B9 + 88));
    ix_wl(vm, MOG_LAB_0A4F, ix_rl(vm, MOG_LAB_05B9 + 88));
    ix_wl(vm, MOG_LAB_0A4E, MOG_t_HitDataByCel);
    MogFile f;
    file_open(vm, MOG_s_CollideHit, &f);
    file_read(vm, &f, ix_rl(vm, MOG_LAB_05B9 + 84), 0x2328);
    file_close(&f);
    ix_wl(vm, MOG_SECSTRT_10, ix_rl(vm, MOG_L23_0000E));
}

/* LAB_03D8 / LAB_03D9 : nombre décimal à 3 / 2 chiffres (ADD.B : l'octet
 * de poids faible est ajouté sans retenue). */
static uint32_t digits(IxVM *vm, uint32_t *a2, int n)
{
    uint32_t d0 = (uint32_t)ix_rb(vm, (*a2)++);
    d0 = (d0 & 0xFFFF0000u) | (uint16_t)(d0 - 0x30);
    for (int i = 1; i < n; i++) {
        d0 = (uint32_t)(uint16_t)d0 * 10u;
        d0 = (d0 & 0xFFFFFF00u) | (uint8_t)(d0 + ix_rb(vm, (*a2)++));
        d0 = (d0 & 0xFFFF0000u) | (uint16_t)(d0 - 0x30);
    }
    return d0;
}

/* Col_LoadHitData [LAB_03CE] : points d'impact de la CEL `name` (lus dans
 * collide.hit) rangés pour la poignée `dest`, puis chargement de la CEL
 * à `dest` (LAB_0CBB). Renvoie -1 si le nom est absent de collide.hit. */
int mog_load_hit_cel(IxVM *vm, uint32_t name, uint32_t dest)
{
    uint32_t a2 = ix_rl(vm, MOG_LAB_05B9 + 84);
    int32_t d7 = (int32_t)ix_rl(vm, MOG_SECSTRT_10);
    for (;;) {                                          /* LAB_03CF */
        if (--d7 < 0)
            return -1;
        uint32_t a3 = name;
        int found = 0;
        for (;;) {                                      /* LAB_03D0 */
            uint8_t c = ix_rb(vm, a3++);
            if (!c) {
                found = ix_rb(vm, a2++) == 0x0A;        /* LAB_03D1 */
                break;
            }
            if (c != ix_rb(vm, a2++))
                break;
            if (--d7 < 0)
                return -1;
        }
        if (found)
            break;
    }
    uint32_t a3 = ix_rl(vm, MOG_LAB_0A4D), a4 = ix_rl(vm, MOG_LAB_0A4E);
    ix_wl(vm, a4, dest);
    ix_wl(vm, a4 + 4, a3);
    ix_wl(vm, MOG_LAB_0A4E, a4 + 8);
    for (;;) {                                          /* LAB_03D2 */
        uint32_t d0 = digits(vm, &a2, 2);
        if ((uint16_t)d0 == 0x63)
            break;
        ix_wb(vm, a3++, (uint8_t)d0);
        a2++;
        if (!(uint16_t)d0)
            continue;
        uint16_t count = (uint16_t)d0;
        d0 = digits(vm, &a2, 2);
        a2++;
        ix_wb(vm, a3++, (uint8_t)d0);
        uint32_t ext = a3;
        a3 += 2;
        uint16_t mx = 0, my = 0;
        for (uint32_t k = 0; k <= (uint16_t)(count - 1); k++) {   /* DBF */
            d0 = digits(vm, &a2, 3);
            ix_wb(vm, a3++, (uint8_t)d0);
            if ((int16_t)d0 > (int16_t)mx)
                mx = (uint16_t)d0;
            d0 = digits(vm, &a2, 3);
            ix_wb(vm, a3++, (uint8_t)d0);
            if ((int16_t)d0 > (int16_t)my)
                my = (uint16_t)d0;
        }
        a2++;
        ix_wb(vm, ext, (uint8_t)mx);
        ix_wb(vm, ext + 1, (uint8_t)my);
    }
    ix_wl(vm, MOG_LAB_0A4D, a3);                        /* LAB_03D6 */
    mog_load_cel(vm, name, dest);
    return 0;
}

/* LAB_0115 (sans les sons) : CEL du chevalier dans les banques LAB_05E1
 * (kn1, kn2, kn3, puis kn4 avec ses points d'impact), blo.cel. */
void mog_boot_knight_cels(IxVM *vm)
{
    static const uint32_t names[3] = { MOG_LAB_076F, MOG_LAB_0770, MOG_LAB_0771 };
    uint32_t a1 = ix_rl(vm, MOG_LAB_05B8 + 4);
    ix_wl(vm, MOG_LAB_05E1, a1);
    for (int i = 0; i < 3; i++) {
        mog_load_cel(vm, names[i], a1);
        a1 = ix_rl(vm, MOG_LAB_05E1 + 4u * (unsigned)i) + mog_cel_size(vm, names[i]);
        ix_wl(vm, MOG_LAB_05E1 + 4u * (unsigned)i + 4, a1);
    }
    mog_load_hit_cel(vm, MOG_LAB_0772, a1);
    ix_wl(vm, MOG_LAB_0A4F, ix_rl(vm, MOG_LAB_0A4D));
    ix_wl(vm, MOG_LAB_0A50, ix_rl(vm, MOG_LAB_0A4E));
    mog_load_cel(vm, MOG_LAB_0774, ix_rl(vm, MOG_LAB_05BB));
}

/* LAB_020F : réactions du chevalier humain, selon le contrôleur de
 * l'adversaire (LAB_0621 : touché par ; LAB_0622 : a touché). */
void mog_boot_reactions(IxVM *vm)
{
    static const struct { uint8_t off; uint32_t fn; } hit_by[] = {
        { 0, MOG_LAB_0206 }, { 4, MOG_LAB_020B }, { 8, MOG_LAB_01F9 }, { 12, MOG_LAB_0205 },
        { 16, MOG_LAB_0205 }, { 20, MOG_LAB_0200 }, { 24, MOG_LAB_01F2 }, { 28, MOG_LAB_01F2 },
        { 32, MOG_LAB_01F6 }, { 48, MOG_LAB_01ED }, { 52, MOG_LAB_020B }, { 36, MOG_LAB_01EF },
        { 44, MOG_LAB_0203 }, { 64, MOG_LAB_01FD }, { 40, MOG_LAB_0201 },
    };
    for (unsigned i = 0; i < sizeof hit_by / sizeof hit_by[0]; i++)
        wl(vm, MOG_LAB_0621 + hit_by[i].off, hit_by[i].fn);
    static const uint8_t knights[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 48, 52, 36, 44, 64 };
    for (unsigned i = 0; i < sizeof knights; i++)
        wl(vm, MOG_LAB_0622 + knights[i], MOG_LAB_01E1);
    wl(vm, MOG_LAB_0622 + 40, MOG_LAB_0201);
}

/* Charge une suite de CEL à la suite dans la banque `bank` (5 poignées),
 * à partir de rl(LAB_05B8 + 8) ; comme LAB_0116 et suivantes. */
static uint32_t load_bank(IxVM *vm, uint32_t bank, const uint32_t *names, int n)
{
    uint32_t a1 = ix_rl(vm, MOG_LAB_05B8 + 8);
    ix_wl(vm, bank, a1);
    for (int i = 0; i < n; i++) {
        mog_load_cel(vm, names[i], a1);
        uint32_t size = mog_cel_size(vm, names[i]);
        if (i + 1 < n) {
            a1 = ix_rl(vm, bank + 4u * (unsigned)i) + size;
            ix_wl(vm, bank + 4u * (unsigned)i + 4, a1);
        }
    }
    return a1;
}

/* LAB_0116 : chevalier adverse (He1..He3.ob) dans LAB_05E0 ; armes
 * (banques 3 et 4) partagées avec le chevalier du joueur. */
void mog_load_enemy_knight(IxVM *vm)
{
    if (ix_rb(vm, MOG_LAB_05DF) == 0x0C)
        return;
    ix_wb(vm, MOG_LAB_05DF, 0x0C);
    static const uint32_t he[3] = { MOG_LAB_0775, MOG_LAB_0776, MOG_LAB_0777 };
    load_bank(vm, MOG_LAB_05E0, he, 3);
    ix_wl(vm, MOG_LAB_05E0 + 12, ix_rl(vm, MOG_LAB_05E1 + 12));
    ix_wl(vm, MOG_LAB_05E0 + 16, ix_rl(vm, MOG_LAB_05E1 + 16));
}

int mog_boot_memory(IxVM *vm)
{
    /* même disposition que tools/mog_ref.py */
    uint32_t end = IX_VM_BASE + ix_mog_image.total_size;
    uint32_t fast = (end + 0xFFF) & ~0xFFFu;
    uint32_t top = ((fast + MOG_FAST_SIZE + 0xFFF) & ~0xFFFu) + MOG_STACK_SIZE + 0x1000;
    if (ix_vm_load_at(vm, &ix_mog_image, 0, top - end) < 0)
        return -1;
    vm->heap = top;                                     /* rien à allouer ici */
    ix_wl(VM, MOG_LAB_05BC, MOG_CHIP_BLOCK);
    ix_wl(VM, MOG_LAB_05BD, MOG_CHIP_SIZE);
    ix_wl(VM, MOG_LAB_05BE, fast);
    ix_wl(VM, MOG_LAB_05BF, MOG_FAST_SIZE);
    partition(vm, MOG_CHIP_BLOCK, fast);
    return 0;
}
