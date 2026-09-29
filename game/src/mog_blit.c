/*
 * mog_blit.c — blitter de l'Amiga (mode copie) sur la mémoire de mog, et
 * LAB_0CDA (dessin d'une frame CEL, module SPRITE d'IMAGEXCEL) traduit de
 * amiga_asm/mog.asm. Même modèle que le blitter de tools/mog_ref.py : les
 * écrans produits sont comparés à ceux de l'original.
 */
#include "mog_blit.h"
#include "mog_boot.h"
#include "ix_mog_syms.h"

#define BUF 0x12C0u             /* tampon d'un plan dans LAB_0D40 */

static int16_t sw(uint16_t v) { return (int16_t)v; }

void mog_blitter_run(IxVM *vm, MogBlitter *b, uint16_t size)
{
    b->count++;
    unsigned h = (unsigned)(size >> 6), w = size & 63u;
    if (!h) h = 1024;
    if (!w) w = 64;
    int ua = (b->con0 >> 11) & 1, ub = (b->con0 >> 10) & 1;
    int uc = (b->con0 >> 9) & 1, ud = (b->con0 >> 8) & 1;
    unsigned ash = b->con0 >> 12, bsh = b->con1 >> 12;
    uint8_t mt = (uint8_t)b->con0;
    int desc = (b->con1 & 2) != 0;
    int32_t step = desc ? -2 : 2;
    int32_t ma = b->amod, mb = b->bmod, mc = b->cmod, md = b->dmod;
    if (desc) { ma = -ma; mb = -mb; mc = -mc; md = -md; }
    uint32_t pa = b->apt, pb = b->bpt, pc = b->cpt, pd = b->dpt;
    for (unsigned r = 0; r < h; r++) {
        for (unsigned i = 0; i < w; i++) {
            uint16_t a = ua ? ix_rw(vm, pa) : b->adat;
            if (i == 0)
                a &= b->afwm;
            if (i == w - 1)
                a &= b->alwm;
            uint16_t bb = ub ? ix_rw(vm, pb) : b->bdat;
            uint16_t c = uc ? ix_rw(vm, pc) : b->cdat;
            uint16_t sa, sb;
            if (desc) {
                sa = ash ? (uint16_t)((a << ash) | (b->olda >> (16 - ash))) : a;
                sb = bsh ? (uint16_t)((bb << bsh) | (b->oldb >> (16 - bsh))) : bb;
            } else {
                sa = (uint16_t)((((uint32_t)b->olda << 16) | a) >> ash);
                sb = (uint16_t)((((uint32_t)b->oldb << 16) | bb) >> bsh);
            }
            b->olda = a;
            b->oldb = bb;
            uint16_t d = 0;
            for (int k = 0; k < 8; k++)
                if (mt >> k & 1)
                    d |= (uint16_t)((k & 4 ? sa : (uint16_t)~sa) & (k & 2 ? sb : (uint16_t)~sb)
                                    & (k & 1 ? c : (uint16_t)~c));
            if (ud)
                ix_ww(vm, pd, d);
            if (ua) pa += (uint32_t)step;
            if (ub) pb += (uint32_t)step;
            if (uc) pc += (uint32_t)step;
            if (ud) pd += (uint32_t)step;
        }
        if (ua) pa += (uint32_t)ma;
        if (ub) pb += (uint32_t)mb;
        if (uc) pc += (uint32_t)mc;
        if (ud) pd += (uint32_t)md;
    }
    b->apt = pa; b->bpt = pb; b->cpt = pc; b->dpt = pd;
}

/* LAB_0CF2 / LAB_0CF5 : un plan de la frame (a2) copié dans le tampon a1
 * (lignes de w + 1 mots), premier / dernier mot masqués au bord découpé. */
static void copy_plane(IxVM *vm, MogBlitter *b, uint32_t a1, uint32_t a2, uint16_t d0,
                       uint16_t h, uint16_t w)
{
    uint32_t d1 = 0xFFFFu << (ix_rw(vm, MOG_LAB_0D2A) & 63);
    if (ix_rw(vm, MOG_LAB_0D05)) {                      /* LAB_0CF5 : copie processeur */
        uint32_t src = a2, dst = a1;
        for (unsigned r = 0; r < h; r++) {
            for (unsigned i = 0; i < w; i++, src += 2, dst += 2)
                ix_ww(vm, dst, ix_rw(vm, src));
            src += (uint32_t)(int32_t)sw(d0);
            dst += 2;
        }
        int r25 = ix_rw(vm, MOG_LAB_0D25) != 0, r24 = ix_rw(vm, MOG_LAB_0D24) != 0;
        if (!(r25 || r24))
            return;
        uint32_t d2 = 0xFFFFFFFFu;
        if (r25)
            d2 = (d2 & 0xFFFF0000u) | (d1 & 0xFFFF);
        if (r24) {
            d1 = d1 << 16 | d1 >> 16;
            d2 = d2 << 16 | d2 >> 16;
            d2 = (d2 & 0xFFFF0000u) | (d1 & 0xFFFF);
        } else {
            d2 = d2 << 16 | d2 >> 16;
        }
        uint32_t last = (uint32_t)(uint16_t)((w - 1) * 2);
        for (unsigned r = 0; r < h; r++, a1 += last + 4) {
            ix_ww(vm, a1, ix_rw(vm, a1) & (uint16_t)d2);
            ix_ww(vm, a1 + last, ix_rw(vm, a1 + last) & (uint16_t)(d2 >> 16));
        }
        return;
    }
    b->apt = a2;
    b->dpt = a1;
    b->amod = sw(d0);
    b->dmod = 2;
    b->con0 = 0x09F0;
    b->con1 = 0;
    b->afwm = 0xFFFF;
    b->alwm = 0xFFFF;
    if (ix_rw(vm, MOG_LAB_0D25))
        b->alwm = (uint16_t)d1;
    if (ix_rw(vm, MOG_LAB_0D24))
        b->afwm = (uint16_t)(d1 >> 16);
    mog_blitter_run(vm, b, (uint16_t)(h << 6 | w));
}

void mog_draw_cel(IxVM *vm, MogBlitter *b, uint32_t cel, uint16_t d0, uint16_t d1, uint16_t d2)
{
    if (!ix_rw(vm, MOG_LAB_0D4D))
        mog_boot_graphics(vm);                          /* SECSTRT_30 */
    if (sw(d0) < 0 || sw(d0) >= sw(ix_rw(vm, cel)))
        return;
    uint32_t a0 = cel + 10 + (uint32_t)(uint16_t)(d0 * 10);
    uint32_t a2 = ix_rl(vm, cel + 2) + ix_rl(vm, a0);
    uint16_t d5 = (uint16_t)(((ix_rw(vm, a0 + 4) + 15) & 0xFFF0) >> 4);
    uint16_t d4 = ix_rw(vm, a0 + 6);
    d1 = (uint16_t)(d1 - (ix_rb(vm, a0 + 8) >> 4));
    uint8_t planes = ix_rb(vm, a0 + 9);
    ix_wb(vm, MOG_LAB_0D21, planes);
    ix_wl(vm, MOG_LAB_0D22, 0);
    ix_wl(vm, MOG_LAB_0D23, 0);
    ix_wl(vm, MOG_LAB_0D29, (uint32_t)(uint16_t)(d5 << 1) * d4);
    if (sw(d2) < 0) {
        d4 = (uint16_t)(d4 + d2);
        if (sw(d4) <= 0)
            return;
        uint16_t d6 = (uint16_t)((uint32_t)d5 * (uint16_t)-d2);
        d6 = (uint16_t)(d6 << 1);
        a2 += (uint32_t)(int32_t)sw(d6);
        d2 = 0;
    }
    /* LAB_0CDC */
    if (sw(d2) >= sw(ix_rw(vm, MOG_LAB_0D26)))
        return;
    int16_t over = (int16_t)(d2 + d4 - ix_rw(vm, MOG_LAB_0D26));
    if (over > 0)
        d4 = (uint16_t)(d4 - over);
    /* LAB_0CDD */
    uint16_t shift = d1 & 15;
    ix_ww(vm, MOG_LAB_0D2A, shift);
    d1 = (uint16_t)(sw((uint16_t)(d1 & 0xFFF0)) >> 3);
    if ((int16_t)(d5 * 2 + d1) < 0)
        return;
    if (sw(d1) >= sw(ix_rw(vm, MOG_LAB_0D27)))
        return;
    ix_ww(vm, MOG_LAB_0D24, 0);
    ix_ww(vm, MOG_LAB_0D25, 0);
    if (sw(d1) < 0) {
        ix_ww(vm, MOG_LAB_0D24, 1);
        d1 = (uint16_t)(d1 + 2);
        ix_ww(vm, MOG_LAB_0D22, d1);
        d5 = (uint16_t)(d5 + (sw(d1) >> 1));
        d1 = 0xFFFE;
    } else {
        int16_t d7 = (int16_t)(d5 * 2 + d1 - ix_rw(vm, MOG_LAB_0D27));
        if (d7 >= 0) {
            ix_ww(vm, MOG_LAB_0D25, 1);
            ix_ww(vm, MOG_LAB_0D23, (uint16_t)d7);
            d5 = (uint16_t)(d5 - (d7 >> 1));
        }
    }
    /* LAB_0CDF */
    uint32_t row = ix_rl(vm, MOG_LAB_0D4C) ? (uint32_t)d2 * ix_rw(vm, MOG_SECSTRT_29) : (uint32_t)d2 * 40u;
    d1 = (uint16_t)(d1 + (uint16_t)row);
    if (!d4 || !d5)
        return;
    uint16_t dest = d1, h = d4, w = d5;

    uint32_t a1 = ix_rl(vm, MOG_LAB_0D40);
    uint32_t psize = ix_rl(vm, MOG_LAB_0D29);
    uint8_t d6 = planes;
    uint16_t np = ix_rw(vm, MOG_LAB_0D04);
    uint16_t c22 = ix_rw(vm, MOG_LAB_0D22);
    a2 += (uint32_t)(int32_t)sw((uint16_t)-c22);
    uint16_t amod = (uint16_t)(-c22 + ix_rw(vm, MOG_LAB_0D23));
    for (uint32_t p = 0; p <= np; p++, a1 += BUF) {     /* LAB_0CE2 */
        int present = d6 & 1;
        d6 >>= 1;
        if (present) {
            copy_plane(vm, b, a1, a2, amod, h, w);
            a2 += psize;
        }
    }
    /* masque : OU des plans présents (deux blits) */
    uint32_t t0 = ix_rl(vm, MOG_LAB_0D40);
    uint32_t t[6];
    for (int i = 0; i < 6; i++)
        t[i] = t0 + (uint32_t)i * BUF;
    uint16_t sz = (uint16_t)(h << 6 | w);
    b->afwm = b->alwm = 0xFFFF;
    b->con1 = 0;
    b->amod = b->bmod = b->cmod = b->dmod = 2;
    uint16_t con = 0x0100;
    if (planes & 1) con |= 0x08F0;
    if (planes & 2) con |= 0x04CC;
    if (planes & 4) con |= 0x02AA;
    b->con0 = con;
    b->apt = t[0]; b->bpt = t[1]; b->cpt = t[2]; b->dpt = t[5];
    mog_blitter_run(vm, b, sz);
    con = 0x03AA;
    if (planes & 8) con |= 0x08F0;
    if (planes & 16) con |= 0x04CC;
    b->con0 = con;
    b->apt = t[3]; b->bpt = t[4]; b->cpt = t[5]; b->dpt = t[5];
    mog_blitter_run(vm, b, sz);
    for (uint32_t r = 0; r < h; r++) {                  /* LAB_0CEB : mot de droite nul */
        for (int i = 0; i < 6; i++) {
            t[i] += (uint32_t)(uint16_t)(w * 2);
            ix_ww(vm, t[i], 0);
            t[i] += 2;
        }
    }
    /* collage dans les plans de l'écran */
    uint16_t bw = w;
    if (!ix_rw(vm, MOG_LAB_0D25))
        bw++;
    uint16_t size = (uint16_t)(h << 6 | bw);
    uint16_t stride = ix_rl(vm, MOG_LAB_0D4C) ? ix_rw(vm, MOG_SECSTRT_29) : 40;
    uint16_t d5b = (uint16_t)(dest + ix_rw(vm, MOG_LAB_0D28));
    uint16_t dmod = (uint16_t)(stride - bw * 2);
    uint16_t abmod = ix_rw(vm, MOG_LAB_0D25) ? 2 : 0;
    b->amod = b->bmod = sw(abmod);
    b->cmod = b->dmod = sw(dmod);
    uint16_t s12 = (uint16_t)(shift << 12);
    uint16_t con_on = (uint16_t)(s12 | 0x0FF2), con_off = (uint16_t)(s12 | 0x0722);
    b->con1 = s12;
    b->afwm = b->alwm = 0xFFFF;
    uint32_t src = ix_rl(vm, MOG_LAB_0D40), mask = src + 0x5DC0;
    uint8_t d7 = planes;
    for (uint32_t p = 0; p <= np; p++, src += BUF) {    /* LAB_0CEF */
        uint32_t a5 = ix_rl(vm, MOG_LAB_0CFF + 4 * p) + (uint32_t)(int32_t)sw(d5b);
        b->con0 = (d7 & 1) ? con_on : con_off;
        d7 >>= 1;
        b->apt = src; b->bpt = mask; b->cpt = a5; b->dpt = a5;
        mog_blitter_run(vm, b, size);
    }
}
