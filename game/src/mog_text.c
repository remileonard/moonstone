/*
 * mog_text.c — textes de mog (LAB_0431, LAB_0432) dessinés lettre par
 * lettre avec la police rl(v_Combatants + 10) par LAB_0CDA, traduits de
 * amiga_asm/mog.asm.
 *
 * Enregistrement de texte : long chaîne, mot X, mot Y, octet, octet
 * drapeaux (bit 0 centré, bit 2 aligné à droite, bit 1 zone à restaurer
 * notée, bit 3 lettres rapprochées de 3), long enregistrement suivant.
 */
#include "mog_private.h"
#include "mog_text.h"
#include "ix_mog_syms.h"

#define VM (m->eng.vm)

static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }

/* Largeur et hauteur de la lettre c (frame LAB_08E7[c - 32] de la police) */
static uint16_t glyph(MogCombat *m, uint8_t c)
{
    uint16_t f = ix_rb(VM, MOG_LAB_08E7 + (uint8_t)(c - 0x20));
    uint32_t font = ix_rl(VM, MOG_v_Combatants + 10);
    uint32_t e = font + (uint32_t)(uint16_t)(f * 10);
    ww(m, MOG_LAB_08DC, rw(m, e + 14));
    ww(m, MOG_LAB_08DD, rw(m, e + 16));
    return f;
}

/* LAB_043D : largeur de la chaîne LAB_08E2 */
uint16_t mog_text_width(MogCombat *m, uint8_t flags)
{
    ww(m, MOG_LAB_0441, 0);
    for (uint32_t a = ix_rl(VM, MOG_LAB_08E2); ix_rb(VM, a); a++) {
        glyph(m, ix_rb(VM, a));
        if (flags & 8)
            ww(m, MOG_LAB_08DC, (uint16_t)(rw(m, MOG_LAB_08DC) - 3));
        ww(m, MOG_LAB_0441, (uint16_t)(rw(m, MOG_LAB_0441) + rw(m, MOG_LAB_08DC)));
    }
    return rw(m, MOG_LAB_0441);
}

/* LAB_0447 : zone de la lettre notée dans la pile des zones à restaurer */
static void note_scrap(MogCombat *m, uint16_t x, uint16_t y)
{
    uint32_t a5 = ix_rl(VM, MOG_LAB_0641);
    ww(m, a5, x);
    ww(m, a5 + 2, y);
    ww(m, a5 + 4, rw(m, MOG_LAB_08DC));
    ww(m, a5 + 6, rw(m, MOG_LAB_08DD));
    ww(m, a5 + 12, 0xFFFF);
    ix_wl(VM, MOG_LAB_0641, a5 + 8);
}

void mog_text_records(MogCombat *m, uint32_t a0)
{
    ww(m, MOG_LAB_0D05, 1);
    if (a0) {
        ix_wl(VM, MOG_LAB_08E3, a0);
        do {
            uint32_t a2 = ix_rl(VM, MOG_LAB_08E3);     /* LAB_0433 */
            if (ix_rl(VM, MOG_v_Combatants + 10) == ix_rl(VM, MOG_LAB_05E3 + 16))
                ix_wb(VM, a2 + 9, ix_rb(VM, a2 + 9) | 8);       /* LAB_043B */
            ix_wl(VM, MOG_LAB_08E2, ix_rl(VM, a2));
            ww(m, MOG_LAB_08DE, rw(m, a2 + 4));
            ww(m, MOG_LAB_08E0, rw(m, a2 + 4));
            ww(m, MOG_LAB_08DF, rw(m, a2 + 6));
            ww(m, MOG_LAB_08E1, rw(m, a2 + 6));
            uint8_t fl = ix_rb(VM, a2 + 9);
            if (fl & 1) {
                uint16_t w = mog_text_width(m, fl);
                uint16_t d1 = (uint16_t)((uint16_t)(rw(m, MOG_LAB_08E5) - rw(m, MOG_LAB_08E6) - w) >> 1);
                ww(m, MOG_LAB_08DE, d1);
                ww(m, MOG_LAB_08E0, d1);
            } else if (fl & 4) {
                uint16_t w = mog_text_width(m, fl);
                uint16_t d1 = (uint16_t)(rw(m, MOG_LAB_08E5) - rw(m, MOG_LAB_08E6) - w);
                ww(m, MOG_LAB_08DE, d1);
                ww(m, MOG_LAB_08E0, d1);
            }
            for (;;) {                                  /* LAB_0435 */
                uint32_t s = ix_rl(VM, MOG_LAB_08E2);
                uint8_t c = ix_rb(VM, s);
                if (!c)
                    break;
                ix_wl(VM, MOG_LAB_08E2, s + 1);
                uint16_t f = glyph(m, c);
                uint16_t x = rw(m, MOG_LAB_08DE), y = rw(m, MOG_LAB_08DF);
                a2 = ix_rl(VM, MOG_LAB_08E3);
                if (ix_rb(VM, a2 + 9) & 2)
                    note_scrap(m, x, y);
                if (ix_rb(VM, a2 + 9) & 8)
                    ww(m, MOG_LAB_08DC, (uint16_t)(rw(m, MOG_LAB_08DC) - 3));
                mog_draw_cel(VM, &m->blt, ix_rl(VM, MOG_v_Combatants + 10), f, x, y);
                x = (uint16_t)(rw(m, MOG_LAB_08DE) + rw(m, MOG_LAB_08DC));
                ww(m, MOG_LAB_08DE, x);
                if (!((int16_t)x < 0x140)) {
                    ww(m, MOG_LAB_08DE, rw(m, MOG_LAB_08E0));
                    y = (uint16_t)(y + rw(m, MOG_LAB_08DD));
                    ww(m, MOG_LAB_08DF, y);
                    if (!((int16_t)rw(m, MOG_LAB_08DF) < 0xC8))
                        ww(m, MOG_LAB_08DF, rw(m, MOG_LAB_08E1));
                }
            }
            ix_wl(VM, MOG_LAB_08E3, ix_rl(VM, ix_rl(VM, MOG_LAB_08E3) + 10));  /* LAB_0439 */
        } while (ix_rl(VM, MOG_LAB_08E3));
    }
    ww(m, MOG_LAB_0D05, 0);
}

void mog_text(MogCombat *m, uint32_t str, uint16_t x, uint16_t y, uint16_t flags)
{
    uint32_t a1 = MOG_LAB_08E4;                         /* LAB_0431 */
    ix_wl(VM, a1, str);
    ww(m, a1 + 4, x);
    ww(m, a1 + 6, y);
    ww(m, a1 + 8, flags);
    ix_wl(VM, a1 + 10, 0);
    mog_text_records(m, a1);
}
