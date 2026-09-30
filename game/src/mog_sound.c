/*
 * mog_sound.c — pilote de sons de mog (section S_44 de l'original) et
 * puce audio Paula de l'Amiga, traduits de amiga_asm/mog.asm.
 *
 * Pilote : quatre voies (SECSTRT_44, LAB_0F66, LAB_0F67, LAB_0F68 ; 148
 * octets chacune) qui jouent des programmes d'octets (table LAB_1098) :
 * notes (hauteur dans la table de l'instrument), silences, volume, durées,
 * boucles, appels, instruments (LAB_10A2), enveloppes (LAB_0FE1) et
 * vibratos. LAB_0F8C lance un son sur une voie, LAB_0F8F (serveur de VBL
 * LAB_0F73) avance les programmes, LAB_0F7B mène les enveloppes, LAB_0F75
 * écrit les registres ; l'interruption audio (LAB_0F69 / LAB_0F6F) enchaîne
 * les blocs d'échantillons (partie bouclée ou silence).
 *
 * Paula : registres LC / LEN / PER / VOL de chaque voie (verrouillés),
 * DMACON, INTENA, INTREQ ; un bloc lu, l'adresse et la longueur sont
 * rechargées et l'interruption de la voie levée (aussi au départ du DMA).
 * Échantillons 8 bits signés lus dans la mémoire de mog ; horloge PAL.
 *
 * Sans hôte audio (m->audio NULL, bancs de comparaison), rien ne joue et
 * la mémoire n'est pas touchée (le banc remplace aussi ces routines).
 */
#include "mog_sound.h"
#include "mog_private.h"
#include "ix_mog_names.h"

#include <string.h>

#define VM (m->eng.vm)

static uint32_t rl(MogCombat *m, uint32_t a) { return ix_rl(VM, a); }
static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static uint8_t  rb(MogCombat *m, uint32_t a) { return ix_rb(VM, a); }
static void wl(MogCombat *m, uint32_t a, uint32_t v) { ix_wl(VM, a, v); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }
static void wb(MogCombat *m, uint32_t a, uint8_t v)  { ix_wb(VM, a, v); }

#define PAULA_CLOCK 3546895.0                           /* PAL */

static const uint32_t voice_of[4] = {                   /* LAB_0F8B */
    MOG_v_Voice0, MOG_v_Voice1, MOG_v_Voice2, MOG_v_Voice3
};
static const uint32_t stack_of[4] = {                   /* LAB_0F8E */
    MOG_b_SndSeq1, MOG_b_SndSeq2, MOG_b_SndSeq3, MOG_t_SndDefault
};

/* ------------------------------------------------------------------ */
/* Paula                                                               */
/* ------------------------------------------------------------------ */

static void irq(MogCombat *m);

static void dma_start(MogAudio *a, int c)
{
    MogPaulaCh *p = &a->ch[c];
    p->ptr = p->lc;
    p->words = p->len ? p->len : 0x10000u;
    p->pos = 0;
    p->frac = 0;
    p->on = 1;
    p->start_irq = 1;                   /* levée au premier mot lu */
}

/* Écriture d'un registre custom ($DFFxxx) */
static void hw_ww(MogCombat *m, uint32_t reg, uint16_t v)
{
    MogAudio *a = m->audio;
    uint32_t off = reg & 0x1FF;
    if (off == 0x096) {                                 /* DMACON */

        uint16_t old = a->dmacon;
        if (v & 0x8000)
            a->dmacon |= v & 0x7FFF;
        else
            a->dmacon &= (uint16_t)~v;
        for (int c = 0; c < 4; c++) {
            int was = old & (1 << c), now = a->dmacon & (1 << c);
            if (now && !was)
                dma_start(a, c);
            else if (!now)
                a->ch[c].on = 0;
        }
    } else if (off == 0x09A) {                          /* INTENA */
        if (v & 0x8000) a->intena |= v & 0x7FFF;
        else            a->intena &= (uint16_t)~v;
        irq(m);
    } else if (off == 0x09C) {                          /* INTREQ */
        if (v & 0x8000) a->intreq |= v & 0x7FFF;
        else            a->intreq &= (uint16_t)~v;
        irq(m);
    } else if (off >= 0x0A0 && off < 0x0E0) {
        MogPaulaCh *p = &a->ch[(off - 0x0A0) >> 4];
        switch (off & 15) {
        case 0: p->lc = (p->lc & 0xFFFF) | (uint32_t)v << 16; break;
        case 2: p->lc = (p->lc & 0xFFFF0000u) | (v & 0xFFFE); break;
        case 4: p->len = v; break;
        case 6: p->per = v; break;
        case 8: p->vol = v & 0x7F; break;
        }
    }
}

static void hw_wl(MogCombat *m, uint32_t reg, uint32_t v)
{
    hw_ww(m, reg, (uint16_t)(v >> 16));
    hw_ww(m, reg + 2, (uint16_t)v);
}

/* ------------------------------------------------------------------ */
/* Interruption audio (LAB_0F69 / LAB_0F6F)                            */
/* ------------------------------------------------------------------ */

/* LAB_0F6F : bloc suivant de la voie a1 */
static void irq_voice(MogCombat *m, uint32_t a1)
{
    uint32_t a2 = rl(m, a1 + 8);
    ww(m, a1 + 14, (uint16_t)(rw(m, a1 + 14) - 1));
    if ((int16_t)rw(m, a1 + 36) < 0) {                  /* partie bouclée */
        uint16_t d1 = rw(m, a1 + 38);
        hw_wl(m, a2, (uint32_t)d1 + rl(m, a1 + 28));
        hw_ww(m, a2 + 4, (uint16_t)(rw(m, a1 + 40) - (d1 >> 1)));
        hw_ww(m, 0xDFF09A, rw(m, a1 + 6));
    } else if (!rw(m, a1 + 14)) {                       /* LAB_0F70 : fin */
        hw_ww(m, 0xDFF09A, rw(m, a1 + 6));
        hw_ww(m, 0xDFF096, rw(m, a1 + 2));
    } else {                                            /* LAB_0F71 : silence */
        hw_wl(m, a2, MOG_t_SndSilentSample);
        hw_ww(m, a2 + 4, 1);
    }
    hw_ww(m, 0xDFF09C, rw(m, a1 + 6));                  /* LAB_0F72 */
}

void mog_snd_irq_voice(MogCombat *m, int ch)
{
    irq_voice(m, voice_of[ch & 3]);
}

/* LAB_0F69 : voies demandées, dans l'ordre 0, 1, 2, 3 (bits 7 à 10) */
static void irq(MogCombat *m)
{
    MogAudio *a = m->audio;
    if (a->in_irq)
        return;
    a->in_irq = 1;
    for (;;) {
        uint16_t pend = a->intreq & 0x0780;
        if (!(pend & a->intena))
            break;
        int c = pend & 0x80 ? 0 : pend & 0x100 ? 1 : pend & 0x200 ? 2 : 3;
        uint16_t bit = (uint16_t)(0x80 << c);
        hw_ww(m, 0xDFF09C, bit);
        if (a->intena & bit)
            irq_voice(m, voice_of[c]);
    }
    a->in_irq = 0;
}

static void raise_irq(MogCombat *m, int c)
{
    m->audio->intreq |= (uint16_t)(0x80 << c);
    irq(m);
}

/* ------------------------------------------------------------------ */
/* Pilote                                                              */
/* ------------------------------------------------------------------ */

/* LAB_0FD4 : échantillons des instruments (LAB_10A3...) placés dans les
 * banques chargées (LAB_05C7, LAB_05C8, LAB_05C9, LAB_05CA, LAB_05CB) */
void mog_snd_relocate(MogCombat *m)
{
    uint32_t a0 = MOG_t_SndSamplesKnight;
    do {                                                /* LAB_0FD5 */
        wl(m, a0 + 6, rl(m, a0 + 6) + rl(m, MOG_b_SoundsKnight));
        a0 += 14;
    } while (a0 != MOG_t_SndSamplesKnightEnd);
    do {                                                /* LAB_0FD6 */
        wl(m, a0 + 6, rl(m, a0 + 6) + rl(m, MOG_b_SoundsCreature));
        a0 += 14;
    } while (a0 != MOG_t_SndSamplesKnight2End);
    static const struct { uint32_t end, bank; } r1[] = {
        { MOG_t_SndSampleRatmen, MOG_b_SoundsRatmen }, { MOG_t_SndSampleCreature, MOG_b_SoundsCreature },
    };
    for (int i = 0; i < 2; i++)
        for (; a0 != r1[i].end; a0 += 14)
            wl(m, a0 + 6, rl(m, a0 + 6) + rl(m, r1[i].bank));
    static const struct { uint32_t a; uint32_t off; } fix[] = {   /* LAB_0FD9 */
        { MOG_t_SndSamplesA, 0x31E2 }, { MOG_t_SndSamplesA + 14, 0x3DD2 },
        { MOG_t_SndSamplesB, 0x4452 }, { MOG_t_SndSamplesB + 14, 0x50BE },
        { MOG_t_SndSamplesB + 28, 0x50BE },
    };
    for (int i = 0; i < 5; i++)
        wl(m, fix[i].a + 6, rl(m, MOG_b_SoundsKnight) + fix[i].off);
    a0 = MOG_t_SndSampleCreature;
    static const struct { uint32_t end, bank; } r2[] = {
        { MOG_t_SndSampleReplay, MOG_b_SoundsReplay }, { MOG_t_SndSampleWizard, MOG_b_SoundsWizard },
        { MOG_t_SndSampleCreature2, MOG_b_SoundsCreature }, { MOG_t_SndSampleReplay2, MOG_b_SoundsReplay },
        { MOG_t_SndSampleCreature3, MOG_b_SoundsCreature },
    };
    for (int i = 0; i < 5; i++)
        for (; a0 != r2[i].end; a0 += 14)
            wl(m, a0 + 6, rl(m, a0 + 6) + rl(m, r2[i].bank));
}

/* LAB_0F89 : voies et registres remis à zéro */
void mog_snd_init(MogCombat *m)
{
    wb(m, MOG_t_SndDefault, 0xFF);                          /* ST */
    if (m->audio) {
        hw_ww(m, 0xDFF096, 0x800F);
        hw_ww(m, 0xDFF09A, 0x0780);
        hw_ww(m, 0xDFF09C, 0x0780);
    }
    for (uint32_t i = 0; i < 0x204; i++)                /* L44_00BF0 */
        wb(m, MOG_b_SndSeq0 + i, 0);
    for (int c = 0; c < 4; c++) {
        uint32_t a = voice_of[c], reg = 0xDFF0A0 + 16u * (unsigned)c;
        if (m->audio) {
            hw_ww(m, reg + 8, 0);
            hw_ww(m, reg + 4, 1);
            hw_ww(m, reg + 6, 1);
            hw_wl(m, reg, MOG_t_SndSilentSample);
            hw_ww(m, reg + 4, 8);
            hw_ww(m, reg + 6, 0x100);
        }
        wl(m, a + 28, MOG_t_SndSilentSample);
        wl(m, a + 32, MOG_t_SndSilentSample);
        ww(m, a + 40, 8);
        ww(m, a + 14, 2);
        wl(m, a + 24, stack_of[c]);
        wl(m, a + 8, reg);
        ww(m, a + 2, (uint16_t)(1 << c));
        ww(m, a + 0, (uint16_t)(0x8000 | 1 << c));
        ww(m, a + 6, (uint16_t)(0x80 << c));
        ww(m, a + 4, (uint16_t)(0x8000 | 0x80 << c));
    }
    if (m->audio)
        hw_ww(m, 0xDFF096, 0x000F);
    ww(m, MOG_t_SndDefault, 0);
}

/* LAB_0F8C : son n sur la voie ch */
static void play(MogCombat *m, int n, int ch)
{
    wb(m, MOG_t_SndDefault, 0xFF);                          /* ST */
    uint32_t a4 = voice_of[ch & 3];
    for (uint32_t i = 0; i < 128; i++)
        wb(m, a4 + 16 + i, 0);
    hw_ww(m, 0xDFF09A, rw(m, a4 + 6));
    hw_ww(m, 0xDFF09C, rw(m, a4 + 6));
    hw_ww(m, 0xDFF096, rw(m, a4 + 2));
    uint32_t a3 = rl(m, a4 + 8);
    hw_ww(m, a3 + 8, 0);
    hw_wl(m, a3, MOG_t_SndSilentSample);
    hw_ww(m, a3 + 4, 1);
    hw_ww(m, a3 + 6, 1);
    hw_ww(m, 0xDFF096, rw(m, a4 + 0));
    uint32_t a0 = rl(m, MOG_t_SndPrograms + (uint32_t)(uint16_t)(n * 4));
    wl(m, a4 + 54, a0);
    wl(m, a4 + 58, a0);
    wl(m, a4 + 24, stack_of[ch & 3]);
    a0 = MOG_t_SndSilence;
    ww(m, a4 + 36, rw(m, a0));
    ww(m, a4 + 38, rw(m, a0 + 2));
    ww(m, a4 + 40, rw(m, a0 + 4));
    wl(m, a4 + 28, rl(m, a0 + 6));
    wl(m, a4 + 16, rl(m, a0 + 10));
    ww(m, a4 + 70, 0x100);
    ww(m, a4 + 72, 0x100);
    hw_wl(m, a3, rl(m, a4 + 28));
    hw_ww(m, a3 + 4, rw(m, a4 + 40));
    hw_ww(m, a3 + 6, 0x100);
    hw_ww(m, 0xDFF096, 0x0002);         /* (l'original arrête ici la voie 1) */
    ww(m, MOG_t_SndDefault, 0);
}

/* Voie coupée puis relancée sur un bloc neutre (note, arrêt) */
static void mute(MogCombat *m, uint32_t a4, uint32_t lc, int restart)
{
    hw_ww(m, 0xDFF096, rw(m, a4 + 2));
    hw_ww(m, 0xDFF09A, rw(m, a4 + 6));
    hw_ww(m, 0xDFF09C, rw(m, a4 + 6));
    uint32_t a0 = rl(m, a4 + 8);
    hw_ww(m, a0 + 8, 0);
    hw_wl(m, a0, lc);
    hw_ww(m, a0 + 4, 1);
    hw_ww(m, a0 + 6, 1);
    if (restart)
        hw_ww(m, 0xDFF096, rw(m, a4 + 0));
}

/* LAB_0FC0 / LAB_0FBE : vibratos de hauteur et de volume relancés */
static void vib_a(MogCombat *m, uint32_t a4)
{
    for (uint32_t i = 0; i < 2; i++) {
        ww(m, a4 + 74 + 2 * i, rw(m, a4 + 114 + 2 * i));
        ww(m, a4 + 78 + 2 * i, rw(m, a4 + 94 + 2 * i));
    }
}

static void vib_b(MogCombat *m, uint32_t a4)
{
    for (uint32_t i = 0; i < 3; i++) {
        ww(m, a4 + 82 + 2 * i, rw(m, a4 + 118 + 2 * i));
        ww(m, a4 + 88 + 2 * i, rw(m, a4 + 98 + 2 * i));
    }
}

static uint32_t prog(MogCombat *m, uint32_t n)
{
    return rl(m, MOG_t_SndPrograms + (uint32_t)(uint16_t)(n * 4));
}

/* LAB_0F90 ... LAB_0FA3 : une voie du séquenceur */
static void step_voice(MogCombat *m, uint32_t a4)
{
    uint32_t a6 = MOG_v_SndVoice;
    uint32_t a3 = rl(m, a4 + 24);
    if (!rl(m, a4 + 58))
        return;                                         /* LAB_0FA3 */
    if (!rw(m, a4 + 62)) {
        if (!rw(m, a4 + 38))
            mute(m, a4, MOG_t_SndSilence, 1);
        uint32_t a2 = rl(m, a4 + 58);                   /* LAB_0F91 */
        if (!a2)
            goto vol;                                   /* LAB_0FA1 */
        for (;;) {                                      /* LAB_0F92 */
            uint32_t d0 = rb(m, a2++);
            if (!(d0 & 0x80)) {                         /* LAB_0F94 : note */
                ww(m, a4 + 42, (uint16_t)d0);
                mute(m, a4, MOG_t_SndSilence, 0);
                if (rl(m, a4 + 20)) {
                    wb(m, a4 + 50, 0xFF);
                    ww(m, a4 + 52, 0);
                    ww(m, a4 + 44, 0);
                    wb(m, a4 + 48, 0);
                }
                uint16_t k = (uint16_t)(((uint16_t)(d0 * 4) + rw(m, a4 + 140)) * 2);
                ww(m, a4 + 70, rw(m, rl(m, a4 + 16) + (uint32_t)(int32_t)(int16_t)k));
                wb(m, a4 + 46, 0xFF);
                vib_a(m, a4);
                ww(m, a4 + 134, 0);
                vib_b(m, a4);
                ww(m, a4 + 136, 0);
                break;
            }
            d0 = 0;
            switch ((rb(m, a2 - 1) - 0x80) >> 2) {      /* LAB_0F93 */
            case 0: ww(m, a4 + 68, rb(m, a2++)); continue;          /* volume */
            case 1: a2++; continue;
            case 2: a2 = rl(m, a4 + 54); wl(m, a4 + 58, a2); continue;
            case 3: ww(m, a4 + 64, (uint16_t)(rb(m, a2++) * rw(m, a6))); continue;
            case 4: break;                                          /* repos */
            case 5: {                                               /* tempo */
                uint8_t d = rb(m, a2++);
                if (d)
                    ww(m, a6, (uint16_t)(0x2EE / d));
                continue;
            }
            case 6: {                                               /* durées */
                int n = rb(m, a2++);
                uint16_t s = 0;
                for (int i = 0; i < (n ? n : 256); i++)
                    s = (uint16_t)(s + rb(m, a2++));
                ww(m, a4 + 64, (uint16_t)(s * rw(m, a6)));
                continue;
            }
            case 7: {                                               /* vibrato */
                wb(m, a4 + 138, rb(m, a4 + 138) & 0xFC);
                uint32_t a0 = MOG_t_SndInstruments + (uint32_t)rb(m, a2++) * 15u;
                for (uint32_t i = 0; i < 15; i++)
                    ww(m, a4 + 94 + 2 * i, (uint16_t)(int16_t)(int8_t)rb(m, a0 + i));
                wl(m, a4 + 74, 0);
                wl(m, a4 + 82, 0);
                ww(m, a4 + 86, 0);
                ww(m, a4 + 134, 0);
                ww(m, a4 + 136, 0);
                continue;
            }
            case 8: case 9: a2++; continue;
            case 10: wb(m, a4 + 138, rb(m, a4 + 138) | rb(m, a2++)); continue;
            case 11:                                                /* fin */
                ww(m, a4 + 62, 0);
                a2 = 0;
                wl(m, a4 + 54, 0);
                hw_ww(m, 0xDFF09A, rw(m, a4 + 6));
                hw_ww(m, 0xDFF09C, rw(m, a4 + 6));
                hw_ww(m, 0xDFF096, rw(m, a4 + 2));
                ww(m, a4 + 46, 0);
                break;
            case 12:                                                /* appel */
                d0 = rb(m, a2++);
                a3 -= 4;
                wl(m, a3, a2);
                a2 = prog(m, d0);
                continue;
            case 13: a2 = rl(m, a3); a3 += 4; continue;             /* retour */
            case 14: {
                int8_t d = (int8_t)rb(m, a2++);
                ww(m, a4 + 140, d ? (uint16_t)(rw(m, a4 + 140) + d) : 0);
                continue;
            }
            case 15: ww(m, a4 + 140, (uint16_t)(int16_t)(int8_t)rb(m, a2++)); continue;
            case 16: {                                              /* boucle */
                uint8_t d = rb(m, a2++);
                a3 -= 4;
                wl(m, a3, d ? a2 : 0);
                a3 -= 2;
                ww(m, a3, d);
                continue;
            }
            case 17:                                                /* fin de boucle */
                if (rl(m, a3 + 2)) {
                    a2 = rl(m, a3 + 2);
                    uint16_t c = (uint16_t)(rw(m, a3) - 1);
                    ww(m, a3, c);
                    if (!c)
                        wl(m, a3 + 2, 0);
                } else {
                    a3 += 6;
                }
                continue;
            case 18: wl(m, a4 + 20, MOG_t_SndEnvelopes + (uint32_t)(uint16_t)(rb(m, a2++) << 3)); continue;
            case 19: wl(m, a4 + 20, 0); continue;
            case 20: {                                              /* instrument */
                uint32_t a0 = MOG_t_SndSilence + (uint32_t)rb(m, a2++) * 14u;
                ww(m, a4 + 36, rw(m, a0));
                ww(m, a4 + 38, rw(m, a0 + 2));
                ww(m, a4 + 40, rw(m, a0 + 4));
                wl(m, a4 + 28, rl(m, a0 + 6));
                wl(m, a4 + 16, rl(m, a0 + 10));
                continue;
            }
            case 21: a2 = prog(m, rb(m, a2)); continue;             /* saut */
            default: continue;
            }
            break;
        }
        wl(m, a4 + 58, a2);                             /* LAB_0F96 */
        wl(m, a4 + 24, a3);
        ww(m, a4 + 62, rw(m, a4 + 64));
    }
    ww(m, a4 + 62, (uint16_t)(rw(m, a4 + 62) - 1));     /* LAB_0F97 */
    if (!rl(m, a4 + 20)) {
        int d2 = 0;
        for (uint32_t a1 = a4, i = 0; i < 2; i++, a1 += 2) {
            if (rw(m, a1 + 74)) {
                ww(m, a1 + 74, (uint16_t)(rw(m, a1 + 74) - 1));
                d2 = 1;
                break;
            }
            if (rw(m, a1 + 78)) {
                ww(m, a1 + 78, (uint16_t)(rw(m, a1 + 78) - 1));
                ww(m, a4 + 134, (uint16_t)(rw(m, a4 + 134) + rw(m, a1 + 104)));
                ww(m, a1 + 74, rw(m, a1 + 114));
                d2 = 1;
                break;
            }
        }
        if (!d2) {
            ww(m, a4 + 134, 0);
            if (rb(m, a4 + 138) & 1)
                vib_a(m, a4);
        }
    }
    {                                                   /* LAB_0F9C */
        int d2 = 0;
        for (uint32_t a1 = a4, i = 0; i < 3; i++, a1 += 2) {
            if (rw(m, a1 + 82)) {
                ww(m, a1 + 82, (uint16_t)(rw(m, a1 + 82) - 1));
                d2 = 1;
                break;
            }
            if (rw(m, a1 + 88)) {
                ww(m, a1 + 88, (uint16_t)(rw(m, a1 + 88) - 1));
                int16_t d = (int8_t)rb(m, a1 + 109);    /* EXT.W : octet bas */
                ww(m, a4 + 136, (uint16_t)(rw(m, a4 + 136) + d));
                ww(m, a1 + 82, rw(m, a1 + 118));
                d2 = 1;
                break;
            }
        }
        if (!d2 && (rb(m, a4 + 138) & 2))
            vib_b(m, a4);
    }
vol:
    if (!rl(m, a4 + 20))                                /* LAB_0FA1 */
        ww(m, a4 + 44, (uint16_t)((rw(m, a4 + 68) + rw(m, a4 + 134)) & 0x3F));
    ww(m, a4 + 72, (uint16_t)(rw(m, a4 + 70) + rw(m, a4 + 136)));   /* LAB_0FA2 */
}

/* LAB_0F7C : enveloppe de volume (attaque, déclin, maintien, relâche) */
static void envelope(MogCombat *m, uint32_t a4)
{
    if (!rl(m, a4 + 58))
        return;
    uint32_t a3 = rl(m, a4 + 20);
    if (!a3)
        return;
    if (rb(m, a4 + 48)) {
        wb(m, a4 + 48, (uint8_t)(rb(m, a4 + 48) - 1));
        if (rb(m, a4 + 48))
            return;
    }
    uint16_t d0;                                        /* LAB_0F7F */
    if (!rw(m, a4 + 50)) {                              /* LAB_0F86 : relâche */
        ww(m, a4 + 52, 0);
        if (!rw(m, a4 + 44))
            return;
        wb(m, a4 + 48, rb(m, a3 + 7));
        d0 = rw(m, a4 + 44);
        uint8_t lo = (uint8_t)d0, s = rb(m, a3 + 6);
        d0 = lo < s ? 0 : (uint16_t)((d0 & 0xFF00) | (uint8_t)(lo - s));
        ww(m, a4 + 44, d0);
        return;
    }
    uint16_t phase = rw(m, a4 + 52);
    if (phase == 0) {                                   /* attaque */
        wb(m, a4 + 48, rb(m, a3 + 1));
        d0 = rw(m, a4 + 44);
        d0 = (uint16_t)((d0 & 0xFF00) | (uint8_t)(d0 + rb(m, a3)));
        if (!(d0 < rw(m, a4 + 68))) {
            ww(m, a4 + 52, (uint16_t)(rw(m, a4 + 52) + 1));
            d0 = rw(m, a4 + 68);
        }
        ww(m, a4 + 44, d0);                             /* LAB_0F80 */
        return;
    }
    if (phase == 1) {                                   /* LAB_0F84 : déclin */
        wb(m, a4 + 48, rb(m, a3 + 3));
        d0 = rw(m, a4 + 44);
        d0 = (uint16_t)((d0 & 0xFF00) | (uint8_t)(d0 - rb(m, a3 + 2)));
        if (!((uint8_t)d0 < 0x40))
            d0 = 0;
        if ((uint8_t)d0 >= rb(m, a3 + 4)) {
            ww(m, a4 + 44, d0);
            return;
        }
        ww(m, a4 + 52, (uint16_t)(rw(m, a4 + 52) + 1));
    }
    ww(m, a4 + 44, rb(m, a3 + 4));                      /* LAB_0F82 : maintien */
    if ((int8_t)rw(m, a4 + 62) > (int8_t)rb(m, a3 + 5))
        return;
    ww(m, a4 + 50, 0);
}

/* LAB_0F76 : registres de la voie (hauteur, volume, départ d'une note) */
static void apply(MogCombat *m, uint32_t a4)
{
    uint32_t a5 = rl(m, a4 + 8);
    hw_ww(m, a5 + 6, rw(m, a4 + 72));
    int16_t v = (int16_t)(rw(m, a4 + 44) - rw(m, a4 + 142));
    hw_ww(m, a5 + 8, (uint16_t)(v < 0 ? 0 : v));
    if (!rw(m, a4 + 46))
        return;
    ww(m, a4 + 46, 0);
    hw_wl(m, a5, rl(m, a4 + 28));
    hw_ww(m, a5 + 4, rw(m, a4 + 40));
    ww(m, a4 + 14, 2);
    hw_ww(m, 0xDFF096, rw(m, a4 + 0));
    hw_ww(m, 0xDFF09C, rw(m, a4 + 6));
    if ((int16_t)rw(m, a4 + 36) < 0) {
        if (!rw(m, a4 + 38)) {
            hw_ww(m, 0xDFF09A, rw(m, a4 + 6));
            return;
        }
        ww(m, a4 + 14, 1);                              /* LAB_0F78 */
    }
    hw_ww(m, 0xDFF09A, rw(m, a4 + 4));                  /* LAB_0F79 */
}

/* LAB_0F73 : serveur de VBL du pilote */
void mog_snd_vbl(MogCombat *m)
{
    if (!m->audio || rw(m, MOG_t_SndDefault))
        return;
    wb(m, MOG_t_SndDefault, 0xFF);                          /* LAB_0F8F */
    for (int c = 0; c < 4; c++)
        step_voice(m, voice_of[c]);
    for (int c = 0; c < 4; c++)                         /* LAB_0F7B */
        envelope(m, voice_of[c]);
    for (int c = 0; c < 4; c++)                         /* LAB_0F75 */
        apply(m, voice_of[c]);
    ww(m, MOG_t_SndDefault, 0);
}

/* ------------------------------------------------------------------ */
/* Entrées du jeu                                                      */
/* ------------------------------------------------------------------ */

void mog_snd_play(MogCombat *m, int n, int ch)
{
    if (m->audio)
        play(m, n, ch);
    else if (m->voice)
        m->voice(m->out.user, ch, n);
}

/* LAB_0AA2 : effet n sur la voie libre suivante (LAB_0AA5, LAB_0AA6) */
void mog_snd_effect(MogCombat *m, int n)
{
    if (!m->audio) {
        if (m->out.sound)
            m->out.sound(m->out.user, n);
        return;
    }
    if ((rb(m, MOG_v_SndMuted) & 0x0F) == 0x0F)
        return;                                         /* LAB_0AA4 */
    uint16_t c;
    do {
        c = (uint16_t)((rw(m, MOG_v_SndNextVoice) + 1) & 3);
        ww(m, MOG_v_SndNextVoice, c);
    } while (rb(m, MOG_v_SndMuted) & (1u << c));
    play(m, n, c);
}

/* ------------------------------------------------------------------ */
/* Mixage                                                              */
/* ------------------------------------------------------------------ */

void mog_snd_mix(MogCombat *m, int16_t *out, int frames, int rate)
{
    MogAudio *a = m->audio;
    for (int i = 0; i < frames; i++) {
        int s[4] = { 0, 0, 0, 0 };
        for (int c = 0; c < 4; c++) {
            MogPaulaCh *p = &a->ch[c];
            if (!p->on)
                continue;
            if (p->start_irq) {
                p->start_irq = 0;
                raise_irq(m, c);
                if (!p->on)
                    continue;
            }
            int per = p->per < 124 ? 124 : p->per;
            int vol = p->vol > 64 ? 64 : p->vol;
            s[c] = (int8_t)ix_rb(VM, p->ptr + p->pos) * vol;
            p->frac += PAULA_CLOCK / per / rate;
            while (p->frac >= 1.0 && p->on) {
                p->frac -= 1.0;
                if (++p->pos >= p->words * 2) {         /* bloc lu : rechargement */
                    p->ptr = p->lc;
                    p->words = p->len ? p->len : 0x10000u;
                    p->pos = 0;
                    raise_irq(m, c);
                }
            }
        }
        int l = s[0] + s[3], r = s[1] + s[2];          /* voies 0-3 à gauche */
        int L = (l * 3 + r) / 4, R = (r * 3 + l) / 4;
        out[2 * i] = (int16_t)(L * 2);
        out[2 * i + 1] = (int16_t)(R * 2);
    }
}
