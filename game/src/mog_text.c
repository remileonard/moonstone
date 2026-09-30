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
#include "ix_mog_names.h"
#include "mog_struct.h"

#define VM (m->eng.vm)

static uint16_t rw(MogCombat *m, uint32_t a) { return ix_rw(VM, a); }
static void ww(MogCombat *m, uint32_t a, uint16_t v) { ix_ww(VM, a, v); }

/* Largeur et hauteur de la lettre c (frame LAB_08E7[c - 32] de la police) */
static uint16_t glyph(MogCombat *m, uint8_t c)
{
    uint16_t f = ix_rb(VM, MOG_t_FontGlyphFrame + (uint8_t)(c - 0x20));
    uint32_t font = ix_rl(VM, MOG_v_Combatants + CMB_FONT);
    uint32_t e = font + (uint32_t)(uint16_t)(f * 10);
    ww(m, MOG_v_GlyphWidth, rw(m, e + 14));
    ww(m, MOG_v_GlyphHeight, rw(m, e + 16));
    return f;
}

/* LAB_043D : largeur de la chaîne LAB_08E2 */
uint16_t mog_text_width(MogCombat *m, uint8_t flags)
{
    ww(m, MOG_v_TextWidth, 0);
    for (uint32_t a = ix_rl(VM, MOG_v_TextChar); ix_rb(VM, a); a++) {
        glyph(m, ix_rb(VM, a));
        if (flags & 8)
            ww(m, MOG_v_GlyphWidth, (uint16_t)(rw(m, MOG_v_GlyphWidth) - 3));
        ww(m, MOG_v_TextWidth, (uint16_t)(rw(m, MOG_v_TextWidth) + rw(m, MOG_v_GlyphWidth)));
    }
    return rw(m, MOG_v_TextWidth);
}

/* LAB_0447 : zone de la lettre notée dans la pile des zones à restaurer */
static void note_scrap(MogCombat *m, uint16_t x, uint16_t y)
{
    uint32_t a5 = ix_rl(VM, MOG_v_RestoreNext);
    ww(m, a5 + SCR_X, x);
    ww(m, a5 + SCR_Y, y);
    ww(m, a5 + SCR_W, rw(m, MOG_v_GlyphWidth));
    ww(m, a5 + SCR_H, rw(m, MOG_v_GlyphHeight));
    ww(m, a5 + SCR_SIZE + SCR_W, 0xFFFF);
    ix_wl(VM, MOG_v_RestoreNext, a5 + SCR_SIZE);
}

void mog_text_records(MogCombat *m, uint32_t a0)
{
    ww(m, MOG_v_BlitByCpu, 1);
    if (a0) {
        ix_wl(VM, MOG_v_TextRecord, a0);
        do {
            uint32_t a2 = ix_rl(VM, MOG_v_TextRecord);     /* LAB_0433 */
            if (ix_rl(VM, MOG_v_Combatants + CMB_FONT) == ix_rl(VM, MOG_t_FontBank + 16))
                ix_wb(VM, a2 + TXT_FLAGS, ix_rb(VM, a2 + TXT_FLAGS) | 8);       /* LAB_043B */
            ix_wl(VM, MOG_v_TextChar, ix_rl(VM, a2 + TXT_STRING));
            ww(m, MOG_v_TextX, rw(m, a2 + TXT_X));
            ww(m, MOG_v_TextLineX, rw(m, a2 + TXT_X));
            ww(m, MOG_v_TextY, rw(m, a2 + TXT_Y));
            ww(m, MOG_v_TextTopY, rw(m, a2 + TXT_Y));
            uint8_t fl = ix_rb(VM, a2 + TXT_FLAGS);
            if (fl & 1) {
                uint16_t w = mog_text_width(m, fl);
                uint16_t d1 = (uint16_t)((uint16_t)(rw(m, MOG_v_TextRight) - rw(m, MOG_v_TextRightMargin) - w) >> 1);
                ww(m, MOG_v_TextX, d1);
                ww(m, MOG_v_TextLineX, d1);
            } else if (fl & 4) {
                uint16_t w = mog_text_width(m, fl);
                uint16_t d1 = (uint16_t)(rw(m, MOG_v_TextRight) - rw(m, MOG_v_TextRightMargin) - w);
                ww(m, MOG_v_TextX, d1);
                ww(m, MOG_v_TextLineX, d1);
            }
            for (;;) {                                  /* LAB_0435 */
                uint32_t s = ix_rl(VM, MOG_v_TextChar);
                uint8_t c = ix_rb(VM, s);
                if (!c)
                    break;
                ix_wl(VM, MOG_v_TextChar, s + 1);
                uint16_t f = glyph(m, c);
                uint16_t x = rw(m, MOG_v_TextX), y = rw(m, MOG_v_TextY);
                a2 = ix_rl(VM, MOG_v_TextRecord);
                if (ix_rb(VM, a2 + TXT_FLAGS) & 2)
                    note_scrap(m, x, y);
                if (ix_rb(VM, a2 + TXT_FLAGS) & 8)
                    ww(m, MOG_v_GlyphWidth, (uint16_t)(rw(m, MOG_v_GlyphWidth) - 3));
                mog_draw_cel(VM, &m->blt, ix_rl(VM, MOG_v_Combatants + CMB_FONT), f, x, y);
                x = (uint16_t)(rw(m, MOG_v_TextX) + rw(m, MOG_v_GlyphWidth));
                ww(m, MOG_v_TextX, x);
                if (!((int16_t)x < 0x140)) {
                    ww(m, MOG_v_TextX, rw(m, MOG_v_TextLineX));
                    y = (uint16_t)(y + rw(m, MOG_v_GlyphHeight));
                    ww(m, MOG_v_TextY, y);
                    if (!((int16_t)rw(m, MOG_v_TextY) < 0xC8))
                        ww(m, MOG_v_TextY, rw(m, MOG_v_TextTopY));
                }
            }
            ix_wl(VM, MOG_v_TextRecord, ix_rl(VM, ix_rl(VM, MOG_v_TextRecord) + TXT_NEXT));  /* LAB_0439 */
        } while (ix_rl(VM, MOG_v_TextRecord));
    }
    ww(m, MOG_v_BlitByCpu, 0);
}

void mog_text(MogCombat *m, uint32_t str, uint16_t x, uint16_t y, uint16_t flags)
{
    uint32_t a1 = MOG_t_TextRecord;                         /* LAB_0431 */
    ix_wl(VM, a1 + TXT_STRING, str);
    ww(m, a1 + TXT_X, x);
    ww(m, a1 + TXT_Y, y);
    ww(m, a1 + TXT_FLAGS_WORD, flags);
    ix_wl(VM, a1 + TXT_NEXT, 0);
    mog_text_records(m, a1);
}
