/*
 * mog_vbl.c — partie « couleurs » de l'interruption d'image de mog
 * (LAB_0E5D) et affichage de l'écran de l'Amiga, traduits de
 * amiga_asm/mog.asm.
 *
 * LAB_0E5D, à chaque VBL : fondu de la palette courante rl(LAB_0E93) vers
 * la palette SECSTRT_39 (un pas toutes les LAB_0E91 VBL, LAB_0E55),
 * rotations de couleurs LAB_0E94 (LAB_0E56), pulsations LAB_0E95
 * (LAB_0E5A) ; les registres de couleur sont recopiés si l'une a changé.
 */
#include "mog_vbl.h"
#include "ix_mog_syms.h"

#define VM vm

/* LAB_0E6A : un pas de chaque composante de d1 vers d2 */
static uint16_t step_colour(uint16_t d1, uint16_t d2)
{
    if ((d1 & 15) != (d2 & 15))
        d1 = (uint16_t)((d1 & 15) > (d2 & 15) ? d1 - 1 : d1 + 1);
    if (((d1 >> 4) & 15) != ((d2 >> 4) & 15))
        d1 = (uint16_t)(((d1 >> 4) & 15) > ((d2 >> 4) & 15) ? d1 - 0x10 : d1 + 0x10);
    if ((d1 >> 8) != (d2 >> 8))
        d1 = (uint16_t)((d1 >> 8) > (d2 >> 8) ? d1 - 0x100 : d1 + 0x100);
    return d1;
}

/* LAB_0FC2 : pendant un fondu marqué (LAB_0FC4), volume des 4 voies */
static void sound_fade(IxVM *vm)
{
    if (!ix_rw(VM, MOG_LAB_0FC4))
        return;
    ix_ww(VM, MOG_SECSTRT_44 + 142, 0x10);
    ix_ww(VM, MOG_LAB_0F66 + 142, 0x10);
    ix_ww(VM, MOG_LAB_0F67 + 142, 0x10);
    ix_ww(VM, MOG_LAB_0F68 + 142, 0x10);
}

void mog_vbl_colours(IxVM *vm, uint16_t colour[32])
{
    int changed = 0;
    uint32_t cur = ix_rl(VM, MOG_LAB_0E93);
    uint32_t a0 = ix_rl(VM, MOG_SECSTRT_39);
    if (a0) {
        uint16_t n = (uint16_t)(ix_rw(VM, MOG_LAB_0E92) - 1);
        ix_ww(VM, MOG_LAB_0E92, n);
        if (!n) {
            sound_fade(vm);
            ix_ww(VM, MOG_LAB_0E92, ix_rw(VM, MOG_LAB_0E91));
            for (uint32_t i = 0; i < 32; i++) {
                uint16_t c = ix_rw(VM, cur + 2 * i);
                uint16_t d = step_colour(c, ix_rw(VM, a0 + 2 * i));
                if (d != c) {
                    ix_ww(VM, cur + 2 * i, d);
                    changed = 1;
                }
            }
            if (!changed)
                ix_wl(VM, MOG_SECSTRT_39, 0);
        }
    }
    /* LAB_0E60 : rotations (6 × 6 octets : début, fin, sens, vitesse, compte) */
    uint32_t s = MOG_LAB_0E94;
    for (int i = 0; i < 6; i++, s += 6) {
        if (!ix_rl(VM, s))
            continue;
        uint8_t k = (uint8_t)(ix_rb(VM, s + 4) - 1);
        ix_wb(VM, s + 4, k);
        if (k)
            continue;
        changed = 1;
        ix_wb(VM, s + 4, ix_rb(VM, s + 3));
        uint32_t a2 = cur + 2u * ix_rb(VM, s), a3 = cur + 2u * ix_rb(VM, s + 1);
        if (!ix_rb(VM, s + 2)) {                        /* vers le bas de la plage */
            uint32_t a4 = a2;
            uint16_t c = ix_rw(VM, a4);
            do {
                a4 += 2;
                ix_ww(VM, a4 - 2, ix_rw(VM, a4));
            } while (a4 != a3);
            ix_ww(VM, a4, c);
        } else {
            uint32_t a4 = a3;
            uint16_t c = ix_rw(VM, a4);
            do {
                a4 -= 2;
                ix_ww(VM, a4 + 2, ix_rw(VM, a4));
            } while (a4 != a2);
            ix_ww(VM, a4, c);
        }
    }
    /* LAB_0E66 : pulsations (6 × 12 octets) */
    s = MOG_LAB_0E95;
    for (int i = 0; i < 6; i++, s += 12) {
        if (!ix_rl(VM, s))
            continue;
        uint16_t k = (uint16_t)(ix_rw(VM, s + 6) - 1);
        ix_ww(VM, s + 6, k);
        if (k)
            continue;
        changed = 1;
        ix_ww(VM, s + 6, ix_rw(VM, s + 4));
        uint32_t a = cur + (uint32_t)(uint16_t)(ix_rw(VM, s) * 2);
        uint16_t target = ix_rw(VM, s + 2);
        uint16_t d = step_colour(ix_rw(VM, a), target);
        ix_ww(VM, a, d);
        if (d != target)
            continue;
        ix_ww(VM, s + 2, ix_rw(VM, s + 8));
        ix_ww(VM, s + 8, target);
        uint16_t rep = ix_rw(VM, s + 10);
        if (rep) {
            ix_ww(VM, s + 10, --rep);
            if (!rep)
                ix_wl(VM, s, 0);
        }
    }
    if (changed && colour)
        for (int i = 0; i < 32; i++)
            colour[i] = ix_rw(VM, cur + 2u * (unsigned)i);
}

/* Écran montré : plans pointés par la copper list (EXT_0024...) */
void mog_screen(const IxVM *vm, const uint16_t colour[32], uint32_t *argb)
{
    IxVM *v = (IxVM *)vm;
    uint32_t plane[5];
    for (uint32_t p = 0; p < 5; p++)
        plane[p] = (uint32_t)ix_rw(v, MOG_COPPER_BPL + 8 * p) << 16
                 | ix_rw(v, MOG_COPPER_BPL + 8 * p + 4);
    uint32_t pal[32];
    for (int i = 0; i < 32; i++) {
        uint16_t c = colour[i];
        pal[i] = 0xFF000000u | (uint32_t)((c >> 8) & 15) * 0x110000u
               | (uint32_t)((c >> 4) & 15) * 0x1100u | (uint32_t)(c & 15) * 0x11u;
    }
    for (uint32_t y = 0; y < 200; y++)
        for (uint32_t xb = 0; xb < 40; xb++) {
            uint8_t b[5];
            for (int p = 0; p < 5; p++)
                b[p] = ix_rb(v, plane[p] + y * 40 + xb);
            for (int k = 0; k < 8; k++) {
                unsigned c = 0;
                for (int p = 0; p < 5; p++)
                    if (b[p] & (0x80 >> k))
                        c |= 1u << p;
                argb[y * 320 + xb * 8 + (uint32_t)k] = pal[c];
            }
        }
}

/* LAB_0E5A : pulsation de la couleur d0 vers d1 (vitesse d2, d3 fois ;
 * 0 : sans fin) ; renvoie l'emplacement (D0 inchangé si tout est pris). */
uint32_t mog_glow(IxVM *vm, uint16_t d0, uint16_t d1, uint16_t d2, uint16_t d3)
{
    uint32_t a0 = MOG_LAB_0E95;
    for (int i = 0; i < 6; i++, a0 += 12) {
        if (ix_rl(VM, a0))
            continue;
        ix_ww(VM, a0, d0);
        ix_ww(VM, a0 + 2, d1);
        ix_ww(VM, a0 + 4, d2);
        ix_ww(VM, a0 + 6, d2);
        ix_ww(VM, a0 + 8, ix_rw(VM, ix_rl(VM, MOG_LAB_0E93) + (uint32_t)(uint16_t)(d0 * 2)));
        ix_ww(VM, a0 + 10, d3);
        return a0;
    }
    return d0;
}

/* LAB_0E56 : rotation des couleurs d0..d1 (sens d2, vitesse d3) */
uint32_t mog_cycle(IxVM *vm, uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
    uint32_t a0 = MOG_LAB_0E94;
    for (int i = 0; i < 6; i++, a0 += 6) {
        if (ix_rl(VM, a0))
            continue;
        ix_wb(VM, a0, d0);
        ix_wb(VM, a0 + 1, d1);
        ix_wb(VM, a0 + 2, d2);
        ix_wb(VM, a0 + 3, d3);
        ix_wb(VM, a0 + 4, d3);
        return a0;
    }
    return d0;
}
