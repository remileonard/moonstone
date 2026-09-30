/*
 * lib_audit_ref.c — voir lib_audit_ref.h. Copie des décodeurs de
 * game/src/mog_files.c (mog_unpack), game/src/prog_intro.c (rnc_unpack)
 * et game/src/mog_boot.c (mog_load_hit_cel) avant leur remplacement par
 * libmoon_assets.
 */
#include "lib_audit_ref.h"
#include "ix_mog_syms.h"
#include "ix_program_syms.h"

uint32_t ref_unpack(IxVM *vm, uint32_t src, uint32_t n, uint32_t dst)
{
    uint32_t end = src + n, out = dst;
    for (;;) {
        uint8_t ctl = ix_rb(vm, src++);
        int d3 = 8;
        for (;;) {
            if (!(src < end))
                return out - dst;
            if (--d3 == -1)
                break;
            int ref = ctl & 0x80;
            ctl = (uint8_t)(ctl << 1);
            if (!ref) {
                ix_wb(vm, out++, ix_rb(vm, src++));
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

typedef struct { IxVM *vm; uint32_t a6, a3; uint8_t d3; } Rnc;

static int rnc_bit(Rnc *r)
{
    int c = r->d3 >> 7;
    r->d3 = (uint8_t)(r->d3 << 1);
    if (r->d3)
        return c;
    uint8_t b = ix_rb(r->vm, --r->a6);
    int c2 = b >> 7;
    r->d3 = (uint8_t)((b << 1) | c);
    return c2;
}

static uint16_t rnc_bits(Rnc *r, uint16_t v, int n)
{
    while (n--)
        v = (uint16_t)((v << 1) | rnc_bit(r));
    return v;
}

uint32_t ref_rnc_unpack(IxVM *vm, uint32_t a0)
{
    uint32_t a1 = a0, d0 = 0;
    Rnc r = { vm, 0, 0, 0 };
    if (ix_rl(vm, a0) == 0x524E4301) {
        uint32_t a4 = a0 + 12;
        uint32_t a2 = a4 + ix_rl(vm, a0 + 4) + 0x100;
        r.a3 = a2;
        r.a6 = a4 + ix_rl(vm, a0 + 8);
        r.d3 = ix_rb(vm, --r.a6);
        for (;;) {
            if (rnc_bit(&r)) {
                uint16_t d5 = 0;
                if (rnc_bit(&r)) {
                    int16_t d1 = 3;
                    for (;;) {
                        int8_t nb = (int8_t)ix_rb(vm, PROGRAM_LAB_019F + (uint32_t)d1);
                        uint16_t d2 = (uint16_t)~(uint16_t)(0xFFFF << nb);
                        d5 = rnc_bits(&r, 0, nb);
                        if (!d1 || d5 != d2)
                            break;
                        d1--;
                    }
                    d5 = (uint16_t)(d5 + (int8_t)ix_rb(vm, PROGRAM_LAB_01A0 + (uint32_t)d1));
                }
                for (uint32_t k = 0; k <= d5; k++)
                    ix_wb(vm, --r.a3, ix_rb(vm, --r.a6));
            }
            if ((int32_t)(r.a6 - a4) <= 0)
                break;
            int16_t c = 3;
            while (c >= 0 && rnc_bit(&r))
                c--;
            uint16_t i0 = (uint16_t)(c + 1);
            uint16_t d6 = 0;
            int8_t nb = (int8_t)ix_rb(vm, PROGRAM_LAB_01A8 + i0);
            if (nb)
                d6 = rnc_bits(&r, 0, nb);
            d6 = (uint16_t)(d6 + (int8_t)ix_rb(vm, PROGRAM_L08_008B3 + i0));
            uint16_t d7 = 0;
            if (d6 == 2) {
                int n = 6;
                uint16_t add = 0;
                if (rnc_bit(&r)) {
                    n = 9;
                    add = 64;
                }
                d7 = (uint16_t)(rnc_bits(&r, 0, n) + add);
            } else {
                int16_t e = 1;
                while (e >= 0 && rnc_bit(&r))
                    e--;
                uint16_t j = (uint16_t)(e + 1);
                int8_t m = (int8_t)ix_rb(vm, PROGRAM_LAB_01B0 + j);
                d7 = rnc_bits(&r, 0, m + 1);
                d7 = (uint16_t)(d7 + ix_rw(vm, PROGRAM_LAB_01B1 + 2u * j));
            }
            d6 = (uint16_t)(d6 - 1);
            uint32_t src = r.a3 + (uint32_t)(int32_t)(int16_t)d7 + (uint32_t)(int32_t)(int16_t)d6;
            if (!d7)
                src = r.a3 + 1;
            for (uint32_t k = 0; k <= d6; k++)
                ix_wb(vm, --r.a3, ix_rb(vm, --src));
        }
        d0 = a2 - r.a3;
        a0 = r.a3;
    }
    uint32_t d2 = a0 - a1;
    if (d0) {
        for (uint32_t k = 0; k < d0; k++)
            ix_wb(vm, a1++, ix_rb(vm, a0++));
        do
            ix_wb(vm, a1++, 0);
        while (--d2);
    }
    return d0;
}

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

int ref_hit_parse(IxVM *vm, uint32_t name, uint32_t dest)
{
    uint32_t a2 = ix_rl(vm, MOG_LAB_05B9 + 84);
    int32_t d7 = (int32_t)ix_rl(vm, MOG_SECSTRT_10);
    for (;;) {
        if (--d7 < 0)
            return -1;
        uint32_t a3 = name;
        int found = 0;
        for (;;) {
            uint8_t c = ix_rb(vm, a3++);
            if (!c) {
                found = ix_rb(vm, a2++) == 0x0A;
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
    for (;;) {
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
        for (uint32_t k = 0; k <= (uint16_t)(count - 1); k++) {
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
    ix_wl(vm, MOG_LAB_0A4D, a3);
    return 0;
}
