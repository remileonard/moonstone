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
#include "ix_mog_names.h"
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
    uint32_t d0 = chip, a0 = MOG_t_ChipBuffers;
    ix_wl(VM, MOG_v_ChipFree, chip + 0x5BF18);
    ix_wl(VM, a0 + 0, d0);
    ix_wl(VM, MOG_v_BgPlanes, d0);
    d0 += 0x9C40;
    ix_wl(VM, a0 + 12, d0);
    ix_wl(VM, MOG_b_SoundsKnight, d0);
    d0 += 0x59D8;
    ix_wl(VM, a0 + 4, d0);
    d0 += 0x11D28;
    ix_wl(VM, a0 + 8, d0);
    ix_wl(VM, MOG_b_SoundsWizard, d0 + 0x9C40);
    ix_wl(VM, MOG_b_SoundsRatmen, d0 + 0xEA60);
    d0 += 0x13880;
    ix_wl(VM, a0 + 28, d0);
    ix_wl(VM, MOG_b_SoundsCreature, d0);
    d0 += 0xC350;
    ix_wl(VM, a0 + 36, d0);
    ix_wl(VM, MOG_b_MapIcons, d0);
    d0 += 0x3A98;
    ix_wl(VM, a0 + 16, d0);
    ix_wl(VM, MOG_v_SheetPlanes, d0);
    d0 += 0x9C40;
    ix_wl(VM, MOG_b_SoundsReplay, d0);

    d0 = fast;
    a0 = MOG_t_FastBuffers;
    ix_wl(VM, MOG_v_FastFree, fast + 0x5654D);
    ix_wl(VM, a0 + 0, d0);
    ix_wl(VM, MOG_b_Piv, d0);
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
    ix_wl(VM, MOG_v_MapCreatures, d0);
    d0 += 0x1E0;
    ix_wl(VM, a0 + 72, d0);
    d0 += 0x240;
    ix_wl(VM, a0 + 84, d0);
    d0 += 0x2328;
    ix_wl(VM, a0 + 88, d0);
    d0 += 0x2328;
    ix_wl(VM, MOG_b_BloCel, d0);
    d0 += 0x2710;
    ix_wl(VM, MOG_SECSTRT_14, d0);
    d0 += 0x960;
    ix_wl(VM, MOG_LAB_0A83, d0);
    ix_wl(VM, MOG_v_Objects, d0);
}

/* LAB_0155 : tables d'attaques, de scripts, de dégâts et de marche du
 * chevalier (remises en état après chaque passage sur la carte). */
void mog_boot_tables_0155(IxVM *vm)
{
    wl(vm, MOG_t_KnightAttacks + 4, MOG_x_KnightAtk1);
    wl(vm, MOG_t_KnightAttacks + 8, MOG_x_KnightAtk2);
    wl(vm, MOG_t_KnightAttacks + 32, MOG_x_KnightAtk8);
    wl(vm, MOG_t_KnightAttacks + 24, MOG_x_KnightAtk6);
    wl(vm, MOG_t_KnightAttacks + 20, MOG_x_KnightAtk5);
    wl(vm, MOG_t_KnightAttacks + 12, MOG_x_KnightAtk3);
    wl(vm, MOG_t_KnightAttacks + 0, MOG_x_KnightStand);
    wl(vm, MOG_t_KnightAttacks + 28, MOG_x_KnightAtk7);
    wl(vm, MOG_t_KnightAttacks + 16, MOG_x_KnightAtk4);
    wl(vm, MOG_t_KnightScripts + 4, MOG_x_KnightReact1);
    wl(vm, MOG_t_KnightScripts + 8, MOG_x_KnightReact1);
    wl(vm, MOG_t_KnightScripts + 32, MOG_x_KnightReact8);
    wl(vm, MOG_t_KnightScripts + 12, MOG_x_KnightReact8);
    wl(vm, MOG_t_KnightScripts + 20, MOG_x_KnightReact1);
    wl(vm, MOG_t_KnightScripts + 0, MOG_x_KnightStand);
    wl(vm, MOG_t_KnightScripts + 24, MOG_x_KnightReact8);
    wl(vm, MOG_t_KnightScripts + 28, MOG_x_KnightRecoil);
    wl(vm, MOG_t_KnightScripts + 16, MOG_x_KnightRecoil);
    wl(vm, MOG_t_KnightDamage + 8, 0x4);
    wl(vm, MOG_t_KnightDamage + 20, 0x2);
    wl(vm, MOG_t_KnightDamage + 4, 0x3);
    wl(vm, MOG_t_KnightDamage + 32, 0x4);
    wl(vm, MOG_t_KnightDamage + 12, 0x3);
    wl(vm, MOG_t_KnightDamage + 24, 0x3);
    wl(vm, MOG_t_KnightWalk + 0, MOG_x_KnightWalkH0);
    wl(vm, MOG_t_KnightWalk + 4, MOG_x_KnightWalkH1);
    wl(vm, MOG_t_KnightWalk + 8, MOG_x_KnightWalkH2);
    wl(vm, MOG_t_KnightWalk + 12, MOG_x_KnightWalkH3);
    wl(vm, MOG_t_KnightWalk + 16, 0x0);
    wl(vm, MOG_t_KnightWalk + 32, MOG_x_KnightWalkUp0);
    wl(vm, MOG_t_KnightWalk + 36, MOG_x_KnightWalkUp1);
    wl(vm, MOG_t_KnightWalk + 40, MOG_x_KnightWalkUp2);
    wl(vm, MOG_t_KnightWalk + 44, MOG_x_KnightWalkUp3);
    wl(vm, MOG_t_KnightWalk + 48, 0x0);
    wl(vm, MOG_t_KnightWalk + 64, MOG_x_KnightWalkDown0);
    wl(vm, MOG_t_KnightWalk + 68, MOG_x_KnightWalkDown1);
    wl(vm, MOG_t_KnightWalk + 72, MOG_x_KnightWalkDown2);
    wl(vm, MOG_t_KnightWalk + 76, MOG_x_KnightWalkDown3);
    wl(vm, MOG_t_KnightWalk + 80, 0x0);
    wl(vm, MOG_t_KnightField50 + 32, 0x1C);
    wl(vm, MOG_t_KnightField50 + 8, 0x10);
    wl(vm, MOG_t_KnightField50 + 4, 0x1C);
    wl(vm, MOG_t_KnightField50 + 20, 0x1C);
}

/* LAB_0152 / LAB_0156 : tables de l'objet (attaques 34, scripts 30,
 * dégâts 42, marche 46, 50) de chaque type de combattant.
 * Transcription directe des écritures d'origine. */
void mog_boot_tables(IxVM *vm)
{
    /* LAB_0152 */
    fill(vm, MOG_t_TroggAWalk + 0, 0, 672);
    fill(vm, MOG_t_KnightAttacks + 0, 0, 828);
    mog_boot_tables_0155(vm);
    /* LAB_0156 */
    wl(vm, MOG_t_TroggSpearScripts + 8, MOG_x_TroggSpearReact2);
    wl(vm, MOG_t_TroggSpearScripts + 32, MOG_x_TroggSpearReact8);
    wl(vm, MOG_t_TroggSpearScripts + 4, MOG_x_TroggSpearReact1);
    wl(vm, MOG_t_TroggSpearScripts + 12, MOG_x_TroggSpearReact8);
    wl(vm, MOG_t_TroggSpearScripts + 20, MOG_x_TroggSpearReact1);
    wl(vm, MOG_t_TroggSpearScripts + 24, MOG_x_TroggSpearReact8);
    wl(vm, MOG_t_TroggSpearScripts + 28, MOG_x_TroggSpearReact2);
    wl(vm, MOG_t_TroggSpearScripts + 16, MOG_x_TroggSpearReact2);
    wl(vm, MOG_t_TroggSpearScripts + 0, MOG_x_TroggSpearReact2);
    wl(vm, MOG_t_TroggSpearWalk + 0, MOG_x_TroggSpearWalkH0);
    wl(vm, MOG_t_TroggSpearWalk + 4, MOG_x_TroggSpearWalkH1);
    wl(vm, MOG_t_TroggSpearWalk + 8, MOG_x_TroggSpearWalkH2);
    wl(vm, MOG_t_TroggSpearWalk + 12, 0x0);
    wl(vm, MOG_t_TroggSpearWalk + 32, MOG_x_TroggSpearWalkUp0);
    wl(vm, MOG_t_TroggSpearWalk + 36, MOG_x_TroggSpearWalkUp1);
    wl(vm, MOG_t_TroggSpearWalk + 40, MOG_x_TroggSpearWalkUp2);
    wl(vm, MOG_t_TroggSpearWalk + 44, MOG_x_TroggSpearWalkUp3);
    wl(vm, MOG_t_TroggSpearWalk + 48, 0x0);
    wl(vm, MOG_t_TroggSpearWalk + 64, MOG_x_TroggSpearWalkDown0);
    wl(vm, MOG_t_TroggSpearWalk + 68, MOG_x_TroggSpearWalkDown1);
    wl(vm, MOG_t_TroggSpearWalk + 72, MOG_x_TroggSpearWalkDown2);
    wl(vm, MOG_t_TroggSpearWalk + 76, MOG_x_TroggSpearWalkDown3);
    wl(vm, MOG_t_TroggSpearWalk + 80, 0x0);
    wl(vm, MOG_t_TroggAAttacks + 8, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAAttacks + 24, MOG_x_TroggAAtk6);
    wl(vm, MOG_t_TroggAAttacks + 20, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAAttacks + 12, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAAttacks + 32, MOG_x_TroggAAtk6);
    wl(vm, MOG_t_TroggAAttacks + 4, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAAttacks + 28, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAAttacks + 16, MOG_x_TroggAAtk2);
    wl(vm, MOG_t_TroggAScripts + 8, MOG_x_TroggAReact2);
    wl(vm, MOG_t_TroggAScripts + 32, MOG_x_TroggAReact8);
    wl(vm, MOG_t_TroggAScripts + 4, MOG_x_TroggAReact1);
    wl(vm, MOG_t_TroggAScripts + 12, MOG_x_TroggAReact8);
    wl(vm, MOG_t_TroggAScripts + 20, MOG_x_TroggAReact1);
    wl(vm, MOG_t_TroggAScripts + 24, MOG_x_TroggAReact8);
    wl(vm, MOG_t_TroggADamage + 8, 0x3);
    wl(vm, MOG_t_TroggADamage + 20, 0x3);
    wl(vm, MOG_t_TroggADamage + 4, 0x3);
    wl(vm, MOG_t_TroggADamage + 32, 0x3);
    wl(vm, MOG_t_TroggADamage + 12, 0x3);
    wl(vm, MOG_t_TroggADamage + 24, 0x3);
    wl(vm, MOG_t_TroggAWalk + 0, MOG_x_TroggAWalkH0);
    wl(vm, MOG_t_TroggAWalk + 4, MOG_x_TroggAWalkH1);
    wl(vm, MOG_t_TroggAWalk + 8, MOG_x_TroggAWalkH2);
    wl(vm, MOG_t_TroggAWalk + 12, 0x0);
    wl(vm, MOG_t_TroggAWalk + 32, MOG_x_TroggAWalkUp0);
    wl(vm, MOG_t_TroggAWalk + 36, MOG_x_TroggAWalkUp1);
    wl(vm, MOG_t_TroggAWalk + 40, MOG_x_TroggAWalkUp2);
    wl(vm, MOG_t_TroggAWalk + 44, MOG_x_TroggAWalkUp3);
    wl(vm, MOG_t_TroggAWalk + 48, 0x0);
    wl(vm, MOG_t_TroggAWalk + 64, MOG_x_TroggAWalkDown0);
    wl(vm, MOG_t_TroggAWalk + 68, MOG_x_TroggAWalkDown1);
    wl(vm, MOG_t_TroggAWalk + 72, MOG_x_TroggAWalkDown2);
    wl(vm, MOG_t_TroggAWalk + 76, MOG_x_TroggAWalkDown3);
    wl(vm, MOG_t_TroggAWalk + 80, 0x0);
    wl(vm, MOG_t_TroggBAttacks + 8, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBAttacks + 24, MOG_x_TroggBAtk6);
    wl(vm, MOG_t_TroggBAttacks + 20, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBAttacks + 12, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBAttacks + 32, MOG_x_TroggBAtk6);
    wl(vm, MOG_t_TroggBAttacks + 4, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBAttacks + 28, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBAttacks + 16, MOG_x_TroggBAtk2);
    wl(vm, MOG_t_TroggBScripts + 8, MOG_x_TroggBReact2);
    wl(vm, MOG_t_TroggBScripts + 32, MOG_x_TroggBReact8);
    wl(vm, MOG_t_TroggBScripts + 4, MOG_x_TroggBReact1);
    wl(vm, MOG_t_TroggBScripts + 12, MOG_x_TroggBReact8);
    wl(vm, MOG_t_TroggBScripts + 20, MOG_x_TroggBReact1);
    wl(vm, MOG_t_TroggBScripts + 24, MOG_x_TroggBReact8);
    wl(vm, MOG_t_TroggBDamage + 8, 0x2);
    wl(vm, MOG_t_TroggBDamage + 20, 0x2);
    wl(vm, MOG_t_TroggBDamage + 4, 0x2);
    wl(vm, MOG_t_TroggBDamage + 32, 0x2);
    wl(vm, MOG_t_TroggBDamage + 12, 0x2);
    wl(vm, MOG_t_TroggBDamage + 24, 0x2);
    wl(vm, MOG_t_TroggBWalk + 0, MOG_x_TroggBWalkH0);
    wl(vm, MOG_t_TroggBWalk + 4, MOG_x_TroggBWalkH1);
    wl(vm, MOG_t_TroggBWalk + 8, MOG_x_TroggBWalkH2);
    wl(vm, MOG_t_TroggBWalk + 12, 0x0);
    wl(vm, MOG_t_TroggBWalk + 32, MOG_x_TroggBWalkUp0);
    wl(vm, MOG_t_TroggBWalk + 36, MOG_x_TroggBWalkUp1);
    wl(vm, MOG_t_TroggBWalk + 40, MOG_x_TroggBWalkUp2);
    wl(vm, MOG_t_TroggBWalk + 44, MOG_x_TroggBWalkUp3);
    wl(vm, MOG_t_TroggBWalk + 48, 0x0);
    wl(vm, MOG_t_TroggBWalk + 64, MOG_x_TroggBWalkDown0);
    wl(vm, MOG_t_TroggBWalk + 68, MOG_x_TroggBWalkDown1);
    wl(vm, MOG_t_TroggBWalk + 72, MOG_x_TroggBWalkDown2);
    wl(vm, MOG_t_TroggBWalk + 76, MOG_x_TroggBWalkDown3);
    wl(vm, MOG_t_TroggBWalk + 80, 0x0);
    wl(vm, MOG_t_PassingKnightScripts + 8, MOG_x_PassingKnightReact2);
    wl(vm, MOG_t_PassingKnightScripts + 32, MOG_x_PassingKnightReact8);
    wl(vm, MOG_t_PassingKnightScripts + 4, MOG_x_PassingKnightReact2);
    wl(vm, MOG_t_PassingKnightScripts + 12, MOG_x_PassingKnightReact2);
    wl(vm, MOG_t_PassingKnightScripts + 20, MOG_x_PassingKnightReact2);
    wl(vm, MOG_t_PassingKnightScripts + 24, MOG_x_PassingKnightReact8);
    wl(vm, MOG_t_PassingKnightScripts + 16, MOG_x_PassingKnightStand);
    copy(vm, MOG_t_PassingKnightWalk + 0, MOG_LAB_015E + 0, 20);
    copy(vm, MOG_t_PassingKnightWalk + 32, MOG_LAB_015E + 20, 20);
    wl(vm, MOG_t_RatmenScripts + 8, MOG_x_RatmenReact2);
    wl(vm, MOG_t_RatmenScripts + 4, MOG_x_RatmenReact1);
    wl(vm, MOG_t_RatmenScripts + 20, MOG_x_RatmenReact1);
    wl(vm, MOG_t_RatmenScripts + 24, MOG_x_RatmenReact1);
    wl(vm, MOG_t_RatmenScripts + 12, MOG_x_RatmenReact1);
    wl(vm, MOG_t_RatmenScripts + 32, MOG_x_RatmenReact8);
    wl(vm, MOG_t_RatmenScripts + 28, MOG_x_RatmenReact8);
    wl(vm, MOG_t_RatmenScripts + 16, MOG_x_RatmenReact8);
    wl(vm, MOG_t_RatmenWalk + 32, MOG_x_RatmenWalkUp0);
    wl(vm, MOG_t_RatmenWalk + 36, MOG_x_RatmenWalkUp1);
    wl(vm, MOG_t_RatmenWalk + 40, MOG_x_RatmenWalkUp2);
    wl(vm, MOG_t_RatmenWalk + 44, MOG_x_RatmenWalkUp3);
    wl(vm, MOG_t_RatmenWalk + 48, 0x0);
    wl(vm, MOG_t_RatmenWalk + 0, MOG_x_RatmenWalkH0);
    wl(vm, MOG_t_RatmenWalk + 4, MOG_x_RatmenWalkH1);
    wl(vm, MOG_t_RatmenWalk + 8, MOG_x_RatmenWalkH2);
    wl(vm, MOG_t_RatmenWalk + 12, MOG_x_RatmenWalkH3);
    wl(vm, MOG_t_RatmenWalk + 16, 0x0);
    for (int i = 0; i < 8; i++) wl(vm, MOG_t_BalokDamage + 0 + 4u * (unsigned)i, 0x4);
    for (int i = 0; i < 8; i++) wl(vm, MOG_t_TrollDamage + 0 + 4u * (unsigned)i, 0x3);
    for (int i = 0; i < 8; i++) wl(vm, MOG_t_TrollScripts + 0 + 4u * (unsigned)i, MOG_t_TrollScripts);
    wl(vm, MOG_t_TrollWalk + 0, MOG_x_TrollWalkH0);
    wl(vm, MOG_t_TrollWalk + 4, MOG_x_TrollWalkH1);
    wl(vm, MOG_t_TrollWalk + 8, MOG_x_TrollWalkH2);
    wl(vm, MOG_t_TrollWalk + 12, MOG_x_TrollWalkH3);
    wl(vm, MOG_t_TrollWalk + 16, 0x0);
    for (int i = 0; i < 8; i++) wl(vm, MOG_t_MudmenScripts + 0 + 4u * (unsigned)i, MOG_x_MudmanHit);
    for (int i = 0; i < 8; i++) wl(vm, MOG_t_MudmenDamage + 0 + 4u * (unsigned)i, 0x2);
    wl(vm, MOG_t_MudmenWalk + 0, MOG_x_MudmenWalkH0);
    wl(vm, MOG_t_MudmenWalk + 4, MOG_x_MudmenWalkH1);
    wl(vm, MOG_t_MudmenWalk + 8, MOG_x_MudmenWalkH0);
    wl(vm, MOG_t_MudmenWalk + 12, MOG_x_MudmenWalkH3);
    wl(vm, MOG_t_MudmenWalk + 16, 0x0);
    wl(vm, MOG_t_MudmenWalk + 20, 0x0);
    wl(vm, MOG_t_MudmenWalk + 24, 0x0);
    wl(vm, MOG_t_MudmenWalk + 28, 0x0);
    wl(vm, MOG_t_DragonScripts + 8, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 4, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 20, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 24, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 12, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 32, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 28, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonScripts + 16, MOG_x_DragonReact2);
    wl(vm, MOG_t_DragonWalk + 32, MOG_x_DragonWalkUp0);
    wl(vm, MOG_t_DragonWalk + 36, MOG_x_DragonWalkUp1);
    wl(vm, MOG_t_DragonWalk + 40, MOG_x_DragonWalkUp2);
    wl(vm, MOG_t_DragonWalk + 44, MOG_x_DragonWalkUp3);
    wl(vm, MOG_t_DragonWalk + 48, MOG_x_DragonWalkUp4);
    wl(vm, MOG_t_DragonWalk + 52, MOG_x_DragonWalkUp4);
    wl(vm, MOG_t_DragonWalk + 56, MOG_x_DragonWalkUp4);
    wl(vm, MOG_t_DragonWalk + 60, MOG_x_DragonWalkUp4);
    wl(vm, MOG_t_DragonWalk + 64, MOG_x_DragonWalkDown0);
    wl(vm, MOG_t_DragonWalk + 68, MOG_x_DragonWalkDown1);
    wl(vm, MOG_t_DragonWalk + 72, MOG_x_DragonWalkDown2);
    wl(vm, MOG_t_DragonWalk + 76, MOG_x_DragonWalkDown3);
    wl(vm, MOG_t_DragonWalk + 80, MOG_x_DragonWalkDown4);
    wl(vm, MOG_t_DragonWalk + 84, MOG_x_DragonWalkDown4);
    wl(vm, MOG_t_DragonWalk + 88, MOG_x_DragonWalkDown4);
    wl(vm, MOG_t_DragonWalk + 92, MOG_x_DragonWalkDown4);

}

/* LAB_0303 : banques CEL (LAB_0647, LAB_0648 = blo.cel), table des
 * opcodes du moteur, tampons de restauration du décor. */
void mog_boot_engine(IxVM *vm)
{
    uint32_t a0 = MOG_t_Banks;
    wl(vm, a0 + 0, MOG_t_BankKnight);
    wl(vm, a0 + 4, MOG_t_BankEnemy);
    wl(vm, a0 + 8, MOG_t_BankMap);
    wl(vm, a0 + 12, MOG_t_BankBlo);
    wl(vm, a0 + 16, 0);
    for (int i = 0; i < 5; i++)
        wl(vm, MOG_t_BankBlo + 4u * (unsigned)i, ix_rl(vm, MOG_b_BloCel));
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
    wl(vm, MOG_v_RestoreBack, MOG_b_RestoreA);
    wl(vm, MOG_v_RestoreFront, MOG_b_RestoreB);
}


/* Col_InitHitFile [LAB_03DA] : collide.hit (texte) en mémoire */
void mog_hit_init(IxVM *vm)
{
    ix_wl(vm, MOG_LAB_0A4D, ix_rl(vm, MOG_t_FastBuffers + 88));
    ix_wl(vm, MOG_LAB_0A4F, ix_rl(vm, MOG_t_FastBuffers + 88));
    ix_wl(vm, MOG_LAB_0A4E, MOG_t_HitDataByCel);
    MogFile f;
    mog_file_open(vm, MOG_s_CollideHit, &f);
    mog_file_read(vm, &f, ix_rl(vm, MOG_t_FastBuffers + 84), 0x2328);
    mog_file_close(&f);
    ix_wl(vm, MOG_SECSTRT_10, ix_rl(vm, MOG_v_FileSize));
}

/* Col_LoadHitData [LAB_03CE] : points d'impact de la CEL `name` (lus dans
 * collide.hit, en mémoire à LAB_05B9 + 84) rangés pour la poignée `dest`,
 * puis chargement de la CEL à `dest` (LAB_0CBB). Renvoie -1 si le nom est
 * absent de collide.hit. Le texte est lu par libmoon_assets
 * (moon_hit_parse) ; les octets rangés sont ceux de LAB_03D2 :
 * [n] ou [n][type][max x][max y][x y]... par frame. */
int mog_load_hit_cel(IxVM *vm, uint32_t name, uint32_t dest)
{
    uint32_t text = ix_rl(vm, MOG_t_FastBuffers + 84);
    uint32_t len = ix_rl(vm, MOG_SECSTRT_10);
    char key[64];
    unsigned i;
    for (i = 0; i < sizeof key - 1 && ix_rb(vm, name + i); i++)
        key[i] = (char)ix_rb(vm, name + i);
    key[i] = 0;
    if (!ix_vm_ok(vm, text, len))
        return -1;
    MoonHit *hit = moon_hit_parse(ix_vm_ptr(vm, text), len);
    const MoonHitSprite *sp = moon_hit_find(hit, key);
    if (!sp) {
        moon_hit_free(hit);
        return -1;
    }
    uint32_t a3 = ix_rl(vm, MOG_LAB_0A4D), a4 = ix_rl(vm, MOG_LAB_0A4E);
    ix_wl(vm, a4, dest);
    ix_wl(vm, a4 + 4, a3);
    ix_wl(vm, MOG_LAB_0A4E, a4 + 8);
    for (int f = 0; f < sp->frame_count; f++) {
        const MoonHitFrame *fr = &sp->frames[f];
        ix_wb(vm, a3++, fr->n_points);
        if (!fr->n_points)
            continue;
        ix_wb(vm, a3++, fr->type);
        ix_wb(vm, a3++, fr->max_dx);
        ix_wb(vm, a3++, fr->max_dy);
        for (int k = 0; k < fr->n_points; k++) {
            ix_wb(vm, a3++, fr->points[k].dx);
            ix_wb(vm, a3++, fr->points[k].dy);
        }
    }
    moon_hit_free(hit);
    ix_wl(vm, MOG_LAB_0A4D, a3);                        /* LAB_03D6 */
    mog_load_cel(vm, name, dest);
    return 0;
}

/* LAB_0115 (sans les sons) : CEL du chevalier dans les banques LAB_05E1
 * (kn1, kn2, kn3, puis kn4 avec ses points d'impact), blo.cel. */
static void read_file(IxVM *vm, uint32_t name, uint32_t dst, uint32_t skip, uint32_t n);

void mog_boot_knight_cels(IxVM *vm)
{
    static const uint32_t names[3] = { MOG_s_Kn1Ob, MOG_s_Kn2Ob, MOG_s_Kn3Ob };
    uint32_t a1 = ix_rl(vm, MOG_t_ChipBuffers + 4);
    ix_wl(vm, MOG_t_BankKnight, a1);
    for (int i = 0; i < 3; i++) {
        mog_load_cel(vm, names[i], a1);
        a1 = ix_rl(vm, MOG_t_BankKnight + 4u * (unsigned)i) + mog_cel_size(vm, names[i]);
        ix_wl(vm, MOG_t_BankKnight + 4u * (unsigned)i + 4, a1);
    }
    mog_load_hit_cel(vm, MOG_s_Kn4Ob, a1);
    ix_wl(vm, MOG_LAB_0A4F, ix_rl(vm, MOG_LAB_0A4D));
    ix_wl(vm, MOG_LAB_0A50, ix_rl(vm, MOG_LAB_0A4E));
    mog_load_cel(vm, MOG_s_BloCel, ix_rl(vm, MOG_b_BloCel));
    read_file(vm, MOG_s_KnA, ix_rl(vm, MOG_b_SoundsKnight), 0x20, 0x57F8);   /* LAB_0AAA */
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
        wl(vm, MOG_t_HitByFn + hit_by[i].off, hit_by[i].fn);
    static const uint8_t knights[] = { 0, 4, 8, 12, 16, 20, 24, 28, 32, 48, 52, 36, 44, 64 };
    for (unsigned i = 0; i < sizeof knights; i++)
        wl(vm, MOG_t_HitFn + knights[i], MOG_LAB_01E1);
    wl(vm, MOG_t_HitFn + 40, MOG_LAB_0201);
}

/* Charge une suite de CEL à la suite dans la banque `bank` (5 poignées),
 * à partir de rl(LAB_05B8 + 8) ; comme LAB_0116 et suivantes. */
static uint32_t load_bank(IxVM *vm, uint32_t bank, const uint32_t *names, int n)
{
    uint32_t a1 = ix_rl(vm, MOG_t_ChipBuffers + 8);
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
    if (ix_rb(vm, MOG_v_CreatureLoaded) == 0x0C)
        return;
    ix_wb(vm, MOG_v_CreatureLoaded, 0x0C);
    static const uint32_t he[3] = { MOG_s_He1Ob, MOG_s_He2Ob, MOG_s_He3Ob };
    load_bank(vm, MOG_t_BankEnemy, he, 3);
    ix_wl(vm, MOG_t_BankEnemy + 12, ix_rl(vm, MOG_t_BankKnight + 12));
    ix_wl(vm, MOG_t_BankEnemy + 16, ix_rl(vm, MOG_t_BankKnight + 16));
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
    ix_wl(VM, MOG_v_ChipFree, MOG_CHIP_BLOCK);
    ix_wl(VM, MOG_v_ChipSize, MOG_CHIP_SIZE);
    ix_wl(VM, MOG_v_FastFree, fast);
    ix_wl(VM, MOG_v_FastSize, MOG_FAST_SIZE);
    partition(vm, MOG_CHIP_BLOCK, fast);
    return 0;
}

/* LAB_013A : les huit décors (PIV compressés) du fichier « Test » à
 * rl(LAB_05B9 + 8) ; compteurs de terrain à zéro, aucune créature
 * chargée (LAB_05DF). */
void mog_boot_backgrounds(IxVM *vm)
{
    ix_ww(vm, MOG_v_TerrainNextGlade, 0);
    ix_ww(vm, MOG_v_TerrainNextForest, 0);
    ix_ww(vm, MOG_v_TerrainNextWater, 0);
    ix_ww(vm, MOG_v_TerrainNextSwamp, 0);
    ix_wb(vm, MOG_v_Unused05DD, 0xFF);
    ix_wb(vm, MOG_v_Unused05DE, 0xFF);
    ix_wb(vm, MOG_v_CreatureLoaded, 0xFF);
    MogFile f;
    mog_file_open(vm, MOG_s_Test, &f);
    mog_file_read(vm, &f, ix_rl(vm, MOG_t_FastBuffers + 8), 0x30859);
    mog_file_close(&f);
}


/* Lecture brute de n octets du fichier `name` (après `skip` octets) */
static void read_file(IxVM *vm, uint32_t name, uint32_t dst, uint32_t skip, uint32_t n)
{
    MogFile f;
    mog_file_open(vm, name, &f);
    f.pos = f.len < skip ? f.len : skip;
    mog_file_read(vm, &f, dst, n);
    mog_file_close(&f);
}

/* LAB_012C (ressources) : écran d'attente (PIV LAB_07AE en LAB_05B9+52),
 * polices LAB_078B / LAB_078C (LAB_05E3+16, LAB_05E3), sons LAB_0AA7
 * (LAB_0AC1 en LAB_05C9), écran « jour suivant » (LAB_07AF en
 * LAB_05B9+56). L'écran de présentation dessiné ensuite est laissé. */
void mog_boot_ui(IxVM *vm)
{
    read_file(vm, MOG_s_MessagePiv, ix_rl(vm, MOG_t_FastBuffers + 52), 0, 0xE6F);
    ix_wl(vm, MOG_t_FontBank + 16, ix_rl(vm, MOG_t_FastBuffers + 40));
    mog_load_cel(vm, MOG_s_BoldF, ix_rl(vm, MOG_t_FontBank + 16));
    read_file(vm, MOG_s_ReA, ix_rl(vm, MOG_b_SoundsReplay), 0x20, 0xD924);   /* LAB_0AA7 */
    ix_ww(vm, MOG_LAB_0AA6, 0);
    ix_wl(vm, MOG_t_FontBank, ix_rl(vm, MOG_t_FontBank + 16) + mog_cel_size(vm, MOG_s_BoldF));
    mog_load_cel(vm, MOG_s_SmallFont, ix_rl(vm, MOG_t_FontBank));
    read_file(vm, MOG_s_ChPiv, ix_rl(vm, MOG_t_FastBuffers + 56), 0, 0x25F6);
    ix_wl(vm, MOG_v_BoldFont, ix_rl(vm, MOG_t_FontBank + 16));
    ix_wl(vm, MOG_v_Combatants + 10, ix_rl(vm, MOG_t_FontBank + 16));
}

/* LAB_0128 : icônes de la carte (LAB_070E en LAB_0664), ki.cel (LAB_0784) */
void mog_boot_map(IxVM *vm)
{
    uint32_t a1 = ix_rl(vm, MOG_t_FastBuffers + 48);
    ix_wl(vm, MOG_t_BankMap, ix_rl(vm, MOG_b_MapIcons));
    for (uint32_t i = 1; i < 5; i++)
        ix_wl(vm, MOG_t_BankMap + 4 * i, a1);
    mog_load_cel(vm, MOG_s_KiCel, a1);
    mog_load_cel(vm, MOG_s_MiC, ix_rl(vm, MOG_b_MapIcons));
}
