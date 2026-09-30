/*
 * mog_col.c — collisions du combat de mog (Combat_Collisions [LAB_03BE]),
 * traduites de amiga_asm/mog.asm.
 *
 * Chaque frame « frappe » dessinée par une entité (liste 40) est testée
 * contre chaque frame « corps » (liste 44) des autres entités à moins de
 * 10 de profondeur : les points d'impact de collide.hit (t_HitDataByCel)
 * de la frame frappe doivent tomber sur un pixel opaque de la frame corps,
 * lue dans la CEL en mémoire. Premier contact : attaquant.14 = cible,
 * cible.18 = attaquant, cible.122/124 = point d'impact.
 */
#include "mog_combat.h"
#include "ix_mog_syms.h"

#define VM (m->eng.vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }

/* Col_SpanOverlap sur des valeurs 16 bits étendues à zéro (voir mog_ctl.c) */
static int span(uint32_t d0, uint32_t d1, uint32_t d2, uint32_t d3)
{
    if ((d2 - d0) & 0x80000000u)
        return (int32_t)d3 >= (int32_t)d0;
    if ((d2 - d1) & 0x80000000u)
        return 1;
    if ((d3 - d0) & 0x80000000u)
        return 0;
    return !((int32_t)d3 >= (int32_t)d1);
}

/* Combat_ClearHitLinks [LAB_0161] */
void mog_clear_hit_links(MogCombat *m)
{
    ix_wl(VM, MOG_LAB_0617 + 14, 0);
    ix_wl(VM, MOG_LAB_0617 + 18, 0);
    uint32_t a1 = ix_rl(VM, MOG_LAB_05C3);
    for (int i = 0; i < 21; i++, a1 += IX_OBJECT_SIZE) {    /* DBF sur 20 : 21 objets */
        ix_wl(VM, a1 + 14, 0);
        ix_wl(VM, a1 + 18, 0);
    }
    a1 = MOG_LAB_0613;
    for (int i = 0; i < 4; i++, a1 += IX_OBJECT_SIZE) {
        ix_wl(VM, a1 + 14, 0);
        ix_wl(VM, a1 + 18, 0);
    }
}

/* Col_PixelHit [LAB_03DB] : frame corps (bcel, bframe) en (bx, by) contre
 * frame frappe (scel, sframe) en (sx, sy). Renvoie 1 et écrit v_HitX/Y. */
static int pixel_hit(MogCombat *m, uint32_t bcel, uint16_t bframe, uint16_t bx, uint16_t by,
                     uint32_t scel, uint16_t sframe, uint16_t sx, uint16_t sy)
{
    ix_ww(VM, MOG_v_HitMirrorW, 0);
    uint32_t se = scel + (uint32_t)(int32_t)sw((uint16_t)(sframe * 10));
    if (!(ix_rb(VM, se + 18) & 1))
        ix_ww(VM, MOG_v_HitMirrorW, ix_rw(VM, se + 14));
    uint16_t mirror = ix_rw(VM, MOG_v_HitMirrorW);

    uint32_t a2 = MOG_t_HitDataByCel;                   /* LAB_03DD */
    for (int guard = 0; ix_rl(VM, a2) != scel; a2 += 8) {
        if (++guard > 4096) {                           /* l'original boucle sans fin */
            m->errors++;
            return 0;
        }
    }
    uint32_t a1 = ix_rl(VM, a2 + 4);
    for (uint16_t k = 0; k < sframe; k++) {             /* enregistrements précédents */
        uint16_t n = ix_rb(VM, a1++);
        if (n)
            a1 += 2u * n + 3u;
    }
    if (!ix_rb(VM, a1))                                 /* LAB_03E1 */
        return 0;

    uint32_t be = bcel + 10 + (uint32_t)(int32_t)sw((uint16_t)(bframe * 10));
    uint16_t bw = ix_rw(VM, be + 4), bh = ix_rw(VM, be + 6);
    uint16_t d2 = sx, d3 = ix_rb(VM, a1 + 2);
    if (mirror)
        d2 = (uint16_t)(d2 + mirror - d3);
    int d5 = span(bx, (uint16_t)(bx + bw), d2, (uint16_t)(d2 + d3));
    d5 += span(by, (uint16_t)(by + bh), sy, (uint16_t)(sy + ix_rb(VM, a1 + 3)));
    if (d5 != 2)
        return 0;

    /* LAB_03E4 : points d'impact contre les pixels du corps */
    uint32_t a3 = ix_rl(VM, bcel + 2) + ix_rl(VM, be);
    uint16_t row = (uint16_t)(((uint16_t)(bw + 15) >> 4) * 2);
    ix_ww(VM, MOG_LAB_0A56, (uint16_t)(row * bh));
    unsigned n = ix_rb(VM, a1);
    a1 += 4;
    for (unsigned p = 0; p < n; p++) {
        uint16_t x = ix_rb(VM, a1++);
        if (mirror)
            x = (uint16_t)(mirror - x);
        x = (uint16_t)(x + sx);
        if (sw(x) < sw(bx) || sw((uint16_t)(x - bx)) >= sw(bw)) {   /* LAB_03E9 */
            a1++;
            continue;
        }
        uint16_t y = (uint16_t)(ix_rb(VM, a1++) + sy);
        if (sw(y) < sw(by) || sw((uint16_t)(y - by)) >= sw(bh))
            continue;                                   /* LAB_03EA */
        uint16_t hx = (uint16_t)(ix_rb(VM, a1 - 2) + sx);
        ix_ww(VM, MOG_v_HitX, hx);
        uint16_t dx = (uint16_t)(hx - bx);
        uint16_t hy = (uint16_t)(ix_rb(VM, a1 - 1) + sy);
        ix_ww(VM, MOG_v_HitY, hy);
        uint32_t dy = (uint32_t)(uint16_t)(hy - by) * row;
        uint16_t off = (uint16_t)(((dx >> 4) * 2) + dy);
        unsigned bit = (dx & 15u) ^ 15u;
        uint32_t a4 = a3;
        uint16_t planes = ix_rb(VM, be + 9);
        uint16_t np = (uint16_t)(ix_rw(VM, MOG_t_Popcount4 + 2u * planes) - 1);
        for (uint32_t q = 0; q <= np; q++) {            /* DBF */
            if (ix_rw(VM, a4 + (uint32_t)(int32_t)sw(off)) & (1u << bit))
                return 1;                               /* LAB_03E8 */
            a4 += (uint32_t)(int32_t)sw(ix_rw(VM, MOG_LAB_0A56));
        }
    }
    return 0;
}

/* Combat_Collisions [LAB_03BE], puis Combat_ClearFrameLists [LAB_03C7]
 * (l'original enchaîne sans RTS). */
void mog_collisions(MogCombat *m)
{
    mog_clear_hit_links(m);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t a6 = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, a6))
            continue;
        int hit = 0;
        for (uint32_t a4 = ix_rl(VM, a6 + 40); !hit && ix_rl(VM, a4); a4 += 10) {
            for (int j = 0; !hit && j < IX_ENTITY_COUNT; j++) {
                uint32_t a5 = MOG_t_Entities + (uint32_t)j * IX_ENTITY_SIZE;
                if (a5 == a6 || !ix_rb(VM, a5))
                    continue;
                int16_t d = (int16_t)(ix_rw(VM, a5 + 10) - ix_rw(VM, a6 + 10));
                if (d < 0)
                    d = (int16_t)-d;
                if (d > 10)
                    continue;
                for (uint32_t a3 = ix_rl(VM, a5 + 44); ix_rl(VM, a3); a3 += 10) {
                    if (!pixel_hit(m, ix_rl(VM, a3), ix_rw(VM, a3 + 4), ix_rw(VM, a3 + 6),
                                   ix_rw(VM, a3 + 8), ix_rl(VM, a4), ix_rw(VM, a4 + 4),
                                   ix_rw(VM, a4 + 6), ix_rw(VM, a4 + 8)))
                        continue;
                    uint32_t o6 = ix_rl(VM, a6 + 24), o5 = ix_rl(VM, a5 + 24);
                    ix_wl(VM, o6 + 14, o5);
                    ix_wl(VM, o5 + 18, o6);
                    ix_ww(VM, o5 + 122, ix_rw(VM, MOG_v_HitX));
                    ix_ww(VM, o5 + 124, ix_rw(VM, MOG_v_HitY));
                    hit = 1;
                    break;
                }
            }
        }
    }
    ix_clear_frame_lists(&m->eng);
}
