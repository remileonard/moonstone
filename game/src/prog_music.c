/*
 * prog_music.c — musique de l'intro : lecteur de modules de program
 * (section S_1 : SECSTRT_1, LAB_0061, serveur VBL LAB_005C / LAB_0065,
 * format NoiseTracker, 31 instruments) traduit de amiga_asm/program.asm,
 * état dans la mémoire d'origine ; Paula réduite à ce que le lecteur
 * utilise (DMA, LC / LEN / PER / VOL latchés, sans interruption).
 *
 * Voie (28 octets, LAB_009D...) : +0 note (4 octets du motif), +4 début
 * de l'échantillon, +8 longueur (mots), +10 boucle, +14 longueur de
 * boucle, +16 période, +18 volume, +20 bit DMA, +22 sens de la glissade,
 * +23 vitesse, +24 période visée, +26 vibrato, +27 phase.
 */
#include "prog_intro.h"
#include "ix_program_names.h"

#define VM (p->vm)
#define PAULA_CLOCK 3546895.0                           /* PAL */

static uint32_t rl(ProgIntro *p, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(ProgIntro *p, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(ProgIntro *p, uint32_t a) { return ix_rb(VM, a); }
static void wl(ProgIntro *p, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(ProgIntro *p, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(ProgIntro *p, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }

static const uint32_t voice_of[4] = {
    PROGRAM_LAB_009D, PROGRAM_LAB_009E, PROGRAM_LAB_009F, PROGRAM_LAB_00A0
};

/* ------------------------------------------------------------- Paula */

static void dma_start(ProgPaulaCh *c)
{
    c->ptr = c->lc;
    c->words = c->len ? c->len : 0x10000u;
    c->pos = 0;
    c->frac = 0;
    c->on = 1;
}

static void dmacon(ProgIntro *p, uint16_t v)
{
    ProgPaula *a = &p->paula;
    uint16_t old = a->dmacon;
    if (v & 0x8000)
        a->dmacon |= v & 0x7FFF;
    else
        a->dmacon &= (uint16_t)~v;
    for (int c = 0; c < 4; c++) {
        int was = old & (1 << c), now = a->dmacon & (1 << c);
        if (now && !was)
            dma_start(&a->ch[c]);
        else if (!now)
            a->ch[c].on = 0;
    }
}

static void aud_lc(ProgIntro *p, int v, uint32_t x) { p->paula.ch[v].lc = x & 0xFFFFFFFEu; }
static void aud_len(ProgIntro *p, int v, uint16_t x) { p->paula.ch[v].len = x; }
static void aud_per(ProgIntro *p, int v, uint16_t x) { p->paula.ch[v].per = x; }

void prog_music_volume(ProgIntro *p, int v, uint16_t x)
{
    p->paula.ch[v & 3].vol = x & 0x7F;
}

void prog_music_mix(ProgIntro *p, int16_t *out, int frames, int rate)
{
    ProgPaula *a = &p->paula;
    for (int i = 0; i < frames; i++) {
        int s[4] = { 0, 0, 0, 0 };
        for (int c = 0; c < 4; c++) {
            ProgPaulaCh *ch = &a->ch[c];
            if (!ch->on)
                continue;
            int per = ch->per < 124 ? 124 : ch->per;
            int vol = ch->vol > 64 ? 64 : ch->vol;
            s[c] = (int8_t)ix_rb(VM, ch->ptr + ch->pos) * vol;
            ch->frac += PAULA_CLOCK / per / rate;
            while (ch->frac >= 1.0) {
                ch->frac -= 1.0;
                if (++ch->pos >= ch->words * 2) {       /* bloc lu : rechargement */
                    ch->ptr = ch->lc;
                    ch->words = ch->len ? ch->len : 0x10000u;
                    ch->pos = 0;
                }
            }
        }
        int l = s[0] + s[3], r = s[1] + s[2];          /* voies 0 et 3 à gauche */
        int L = (l * 3 + r) / 4, R = (r * 3 + l) / 4;
        out[2 * i] = (int16_t)(L * 2);
        out[2 * i + 1] = (int16_t)(R * 2);
    }
}

/* ------------------------------------------------------------ lecteur */

/* LAB_0061 : instruments placés après le dernier motif utilisé */
static void music_init(ProgIntro *p)
{
    uint32_t a0 = rl(p, PROGRAM_LAB_0124), a1 = a0 + 0x3B8;
    uint16_t d0 = 127;
    uint32_t d1 = 0, d2;
new_max:                                                /* LAB_0062 */
    d2 = d1;
    d0--;
    for (;;) {                                          /* LAB_0063 */
        d1 = (d1 & ~0xFFu) | rb(p, a1++);
        if ((int8_t)d1 > (int8_t)d2)
            goto new_max;
        if (--d0 == 0xFFFF)
            break;
    }
    d2 = (d2 & ~0xFFu) | (uint8_t)(d2 + 1);
    uint32_t a2 = (d2 << 10) + 0x43C + a0;
    a1 = PROGRAM_LAB_009C;
    for (int i = 0; i < 31; i++, a0 += 30) {
        wl(p, a2, 0);
        wl(p, a1, a2);
        a1 += 4;
        a2 += (uint32_t)rw(p, a0 + 42) * 2;
    }
    wb(p, PROGRAM_LAB_0096, 6);
    for (int v = 0; v < 4; v++)
        prog_music_volume(p, v, 0);
    wb(p, PROGRAM_L01_0069F, 0);
    wb(p, PROGRAM_LAB_0099, 0);
    ww(p, PROGRAM_L01_006A0, 0);
}

/* SECSTRT_1 : lecteur ajouté aux serveurs VBL (une fois) */
void prog_music_start(ProgIntro *p)
{
    if (rl(p, PROGRAM_LAB_0060))
        return;
    music_init(p);
    uint32_t a1 = PROGRAM_LAB_0372;
    while (rl(p, a1))
        a1 += 4;
    wl(p, a1, PROGRAM_LAB_005C);
    wl(p, PROGRAM_LAB_0060, a1);
}

/* LAB_0067 : arpège (période, +x, +y demi-tons, table LAB_0095) */
static void arpeggio(ProgIntro *p, uint32_t a6, int v)
{
    unsigned m = rb(p, PROGRAM_LAB_0099) % 3;
    if (m == 0) {
        aud_per(p, v, rw(p, a6 + 16));
        return;
    }
    uint16_t d0 = m == 2 ? (uint16_t)(rb(p, a6 + 3) & 0x0F) : (uint16_t)(rb(p, a6 + 3) >> 4);
    d0 = (uint16_t)(d0 * 2);
    int16_t d1 = (int16_t)rw(p, a6 + 16);
    uint32_t a0 = PROGRAM_LAB_0095;
    for (int d7 = 36; d7 >= 0; d7--, a0 += 2) {
        uint16_t d2 = rw(p, a0 + d0);
        if (d1 >= (int16_t)rw(p, a0)) {
            aud_per(p, v, d2);
            return;
        }
    }
}

/* LAB_0077 : glissade vers la note (cible +24, sens +22) */
static void porta_target(ProgIntro *p, uint32_t a6)
{
    uint16_t d2 = (uint16_t)(rw(p, a6) & 0x0FFF);
    ww(p, a6 + 24, d2);
    uint16_t d0 = rw(p, a6 + 16);
    wb(p, a6 + 22, 0);
    if (d0 == d2)
        ww(p, a6 + 24, 0);
    else if ((int16_t)d2 < (int16_t)d0)
        wb(p, a6 + 22, 1);
}

/* LAB_007A */
static void porta(ProgIntro *p, uint32_t a6, int v)
{
    uint8_t s = rb(p, a6 + 3);
    if (s) {
        wb(p, a6 + 23, s);
        wb(p, a6 + 3, 0);
    }
    if (!rw(p, a6 + 24))
        return;
    uint16_t d0 = rb(p, a6 + 23);
    int16_t target = (int16_t)rw(p, a6 + 24);
    if (!rb(p, a6 + 22)) {
        ww(p, a6 + 16, (uint16_t)(rw(p, a6 + 16) + d0));
        if (!(target > (int16_t)rw(p, a6 + 16))) {
            ww(p, a6 + 16, (uint16_t)target);
            ww(p, a6 + 24, 0);
        }
    } else {
        ww(p, a6 + 16, (uint16_t)(rw(p, a6 + 16) - d0));
        if (!(target < (int16_t)rw(p, a6 + 16))) {
            ww(p, a6 + 16, (uint16_t)target);
            ww(p, a6 + 24, 0);
        }
    }
    aud_per(p, v, rw(p, a6 + 16));
}

/* LAB_007E : vibrato (table LAB_0094) */
static void vibrato(ProgIntro *p, uint32_t a6, int v)
{
    uint8_t s = rb(p, a6 + 3);
    if (s)
        wb(p, a6 + 26, s);
    uint8_t ph = rb(p, a6 + 27);
    uint16_t d2 = rb(p, PROGRAM_LAB_0094 + ((ph >> 2) & 0x1F));
    d2 = (uint16_t)((uint16_t)(d2 * (rb(p, a6 + 26) & 0x0F)) >> 6);
    uint16_t d0 = rw(p, a6 + 16);
    d0 = (int8_t)ph < 0 ? (uint16_t)(d0 - d2) : (uint16_t)(d0 + d2);
    aud_per(p, v, d0);
    wb(p, a6 + 27, (uint8_t)(ph + ((rb(p, a6 + 26) >> 2) & 0x3C)));
}

/* LAB_0083 : effets entre deux lignes */
static void effects(ProgIntro *p, uint32_t a6, int v)
{
    if (!(rw(p, a6 + 2) & 0x0FFF)) {                    /* LAB_0082 */
        aud_per(p, v, rw(p, a6 + 16));
        return;
    }
    uint8_t cmd = rb(p, a6 + 2) & 0x0F, x = rb(p, a6 + 3);
    switch (cmd) {
    case 0: arpeggio(p, a6, v); return;
    case 1:                                             /* LAB_0088 */
        ww(p, a6 + 16, (uint16_t)(rw(p, a6 + 16) - x));
        if ((int16_t)((rw(p, a6 + 16) & 0x0FFF) - 0x71) < 0)
            ww(p, a6 + 16, (uint16_t)((rw(p, a6 + 16) & 0xF000) | 0x71));
        aud_per(p, v, rw(p, a6 + 16) & 0x0FFF);
        return;
    case 2:                                             /* LAB_008A */
        ww(p, a6 + 16, (uint16_t)(rw(p, a6 + 16) + x));
        if ((int16_t)((rw(p, a6 + 16) & 0x0FFF) - 0x358) >= 0)
            ww(p, a6 + 16, (uint16_t)((rw(p, a6 + 16) & 0xF000) | 0x358));
        aud_per(p, v, rw(p, a6 + 16) & 0x0FFF);
        return;
    case 3: porta(p, a6, v); return;
    case 4: vibrato(p, a6, v); return;
    }
    aud_per(p, v, rw(p, a6 + 16));
    if (cmd != 0x0A)
        return;
    uint16_t d0 = x >> 4;                               /* LAB_0084 */
    if (d0) {
        ww(p, a6 + 18, (uint16_t)(rw(p, a6 + 18) + d0));
        if ((int16_t)(rw(p, a6 + 18) - 0x40) >= 0)
            ww(p, a6 + 18, 0x40);
    } else {
        ww(p, a6 + 18, (uint16_t)(rw(p, a6 + 18) - (x & 0x0F)));
        if ((int16_t)rw(p, a6 + 18) < 0)
            ww(p, a6 + 18, 0);
    }
    prog_music_volume(p, v, rw(p, a6 + 18));
}

/* LAB_008C : commandes de ligne (B saut, D fin du motif, C volume,
 * F vitesse) */
static void row_command(ProgIntro *p, uint32_t a6, int v)
{
    uint8_t cmd = rb(p, a6 + 2) & 0x0F;
    switch (cmd) {
    case 0x0D:
        wb(p, PROGRAM_L01_006A3, (uint8_t)~rb(p, PROGRAM_L01_006A3));
        break;
    case 0x0B:
        wb(p, PROGRAM_L01_0069F, (uint8_t)(rb(p, a6 + 3) - 1));
        wb(p, PROGRAM_L01_006A3, (uint8_t)~rb(p, PROGRAM_L01_006A3));
        break;
    case 0x0C:
        if ((int8_t)rb(p, a6 + 3) > 0x40)
            wb(p, a6 + 3, 0x40);
        prog_music_volume(p, v, rb(p, a6 + 3));        /* écriture d'un octet */
        break;
    case 0x0F: {
        uint8_t d0 = rb(p, a6 + 3) & 0x1F;
        if (d0) {
            wb(p, PROGRAM_LAB_0099, 0);
            wb(p, PROGRAM_LAB_0096, d0);
        }
        break;
    }
    }
}

/* LAB_006E : nouvelle note de la voie (instrument, période, DMA) */
static void new_note(ProgIntro *p, uint32_t a6, int v, uint32_t a0, uint32_t a3, uint32_t *d1)
{
    wl(p, a6, rl(p, a0 + *d1));
    *d1 += 4;
    uint32_t d2 = (uint32_t)(((rb(p, a6 + 2) & 0xF0) >> 4) | (rb(p, a6) & 0xF0));
    if (d2) {
        uint32_t d4 = (uint32_t)(uint16_t)d2 * 30u;
        wl(p, a6 + 4, rl(p, PROGRAM_LAB_009C + (d2 - 1) * 4));
        ww(p, a6 + 8, rw(p, a3 + d4));
        ww(p, a6 + 18, rw(p, a3 + d4 + 2));
        uint16_t d3 = rw(p, a3 + d4 + 4);
        if (d3) {
            wl(p, a6 + 10, rl(p, a6 + 4) + (uint16_t)(d3 << 1));
            ww(p, a6 + 8, (uint16_t)(d3 + rw(p, a3 + d4 + 6)));
        } else
            wl(p, a6 + 10, rl(p, a6 + 4));
        ww(p, a6 + 14, rw(p, a3 + d4 + 6));
        prog_music_volume(p, v, rw(p, a6 + 18));
    }
    if (rw(p, a6) & 0x0FFF) {
        if ((rb(p, a6 + 2) & 0x0F) == 3)
            porta_target(p, a6);
        else {                                          /* LAB_0071 */
            ww(p, a6 + 16, (uint16_t)(rw(p, a6) & 0x0FFF));
            dmacon(p, rw(p, a6 + 20));
            wb(p, a6 + 27, 0);
            aud_lc(p, v, rl(p, a6 + 4));
            aud_len(p, v, rw(p, a6 + 8));
            aud_per(p, v, rw(p, a6 + 16) & 0x0FFF);
            ww(p, PROGRAM_L01_006A4, (uint16_t)(rw(p, PROGRAM_L01_006A4) | rw(p, a6 + 20)));
        }
    }
    row_command(p, a6, v);
}

/* LAB_0075 : position suivante de la liste (L01_0069F), bouclée */
static void next_position(ProgIntro *p)
{
    ww(p, PROGRAM_L01_006A0, 0);
    wb(p, PROGRAM_L01_006A3, 0);
    uint8_t pos = (uint8_t)((rb(p, PROGRAM_L01_0069F) + 1) & 0x7F);
    wb(p, PROGRAM_L01_0069F, pos);
    if (pos == rb(p, rl(p, PROGRAM_LAB_0124) + 0x3B6))
        wb(p, PROGRAM_L01_0069F, 0);
}

/* LAB_005C / LAB_0065 : une VBL du lecteur */
void prog_music_vbl(ProgIntro *p)
{
    uint8_t t = (uint8_t)(rb(p, PROGRAM_LAB_0099) + 1);
    wb(p, PROGRAM_LAB_0099, t);
    if ((int8_t)t < (int8_t)rb(p, PROGRAM_LAB_0096)) {
        for (int v = 0; v < 4; v++)                     /* LAB_0066 */
            effects(p, voice_of[v], v);
    } else {
        wb(p, PROGRAM_LAB_0099, 0);                     /* LAB_006D : ligne */
        uint32_t a0 = rl(p, PROGRAM_LAB_0124);
        uint32_t a3 = a0 + 12, a2 = a0 + 0x3B8;
        a0 += 0x43C;
        uint32_t d1 = (uint32_t)rb(p, a2 + rb(p, PROGRAM_L01_0069F)) << 10;
        d1 = (d1 & 0xFFFF0000u) | (uint16_t)(d1 + rw(p, PROGRAM_L01_006A0));
        ww(p, PROGRAM_L01_006A4, 0);
        for (int v = 0; v < 4; v++)
            new_note(p, voice_of[v], v, a0, a3, &d1);
        dmacon(p, (uint16_t)(rw(p, PROGRAM_L01_006A4) | 0x8000));  /* LAB_0072 */
        for (int v = 3; v >= 0; v--) {
            aud_lc(p, v, rl(p, voice_of[v] + 10));
            aud_len(p, v, rw(p, voice_of[v] + 14));
        }
        ww(p, PROGRAM_L01_006A0, (uint16_t)(rw(p, PROGRAM_L01_006A0) + 16));
        if (rw(p, PROGRAM_L01_006A0) == 0x400)
            next_position(p);
    }
    while (rb(p, PROGRAM_L01_006A3))                    /* LAB_0076 */
        next_position(p);
    /* LAB_005C : compteurs sur le bouton droit (POTGOR bit 10), sans usage */
    if (p->potgor & 0x400) {
        uint8_t c = (uint8_t)(rb(p, PROGRAM_LAB_005E) - 1);
        wb(p, PROGRAM_LAB_005E, c);
        if (!c) {
            wb(p, PROGRAM_LAB_005E, 2);
            c = (uint8_t)(rb(p, PROGRAM_LAB_005F) - 1);
            wb(p, PROGRAM_LAB_005F, c);
            if (!c)
                wb(p, PROGRAM_LAB_005F, 8);
        }
    }
}
