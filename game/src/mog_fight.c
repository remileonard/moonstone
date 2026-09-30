/*
 * mog_fight.c — un combat de mog complet : démarrage de la mémoire (comme
 * SECSTRT_0), chevaliers remplis d'après le jeu, préparation de la
 * rencontre (mog_encounter.c), boucle (mog_loop.c) et rendu.
 *
 * Rendu : le décor est lu dans les plans de LAB_05C0 (préparés par mog),
 * les sprites sont dessinés d'après les CEL en mémoire de mog au fil des
 * dessins du moteur (ordre de profondeur), couleur 0 transparente, comme
 * LAB_0CDA.
 */
#include "mog_fight.h"
#include "mog_boot.h"
#include "mog_screens.h"
#include "mog_encounter.h"
#include "ix_mog_syms.h"

#include <string.h>

#define VM (&f->vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }

/* ------------------------------------------------------------------ */
/* Rendu                                                               */
/* ------------------------------------------------------------------ */

/* Plans (5 × 8000 octets) -> index */
static void planes_to_index(const IxVM *vm, uint32_t base, uint8_t *pix)
{
    for (int y = 0; y < MOG_SCREEN_H; y++)
        for (int xb = 0; xb < MOG_SCREEN_W / 8; xb++) {
            uint8_t b[5];
            for (int p = 0; p < 5; p++)
                b[p] = ix_rb((IxVM *)vm, base + (uint32_t)p * 0x1F40u + (uint32_t)(y * 40 + xb));
            for (int k = 0; k < 8; k++) {
                uint8_t c = 0;
                for (int p = 0; p < 5; p++)
                    if (b[p] & (0x80 >> k))
                        c |= (uint8_t)(1 << p);
                pix[y * MOG_SCREEN_W + xb * 8 + k] = c;
            }
        }
}

/* LAB_0CDA : frame `frame` de la CEL en mémoire (déjà retournée en place
 * par le moteur) en (x, y) ; plans absents à 0, couleur 0 transparente. */
static void draw_frame(MogFight *f, uint8_t *pix, uint32_t cel, int frame, int x, int y)
{
    if (frame < 0 || frame >= sw(ix_rw(VM, cel)))
        return;
    uint32_t fe = cel + 10 + (uint32_t)frame * 10;
    uint32_t data = ix_rl(VM, cel + 2) + ix_rl(VM, fe);
    int w = ix_rw(VM, fe + 4), h = ix_rw(VM, fe + 6);
    x -= ix_rb(VM, fe + 8) >> 4;                        /* décalage d'une frame retournée */
    uint8_t planes = ix_rb(VM, fe + 9);
    int bpr = ((w + 15) & ~15) >> 3;
    uint32_t plane_size = (uint32_t)(bpr * h);
    for (int r = 0; r < h; r++) {
        int sy = y + r;
        if (sy < 0 || sy >= MOG_SCREEN_H)
            continue;
        for (int c = 0; c < bpr * 8; c++) {
            int sx = x + c;
            if (sx < 0 || sx >= MOG_SCREEN_W)
                continue;
            uint8_t col = 0;
            uint32_t k = 0;
            for (int p = 0; p < 5; p++) {
                if (!(planes & (1 << p)))
                    continue;
                uint8_t b = ix_rb(VM, data + k * plane_size + (uint32_t)(r * bpr + (c >> 3)));
                if (b & (0x80 >> (c & 7)))
                    col |= (uint8_t)(1 << p);
                k++;
            }
            if (col)
                pix[sy * MOG_SCREEN_W + sx] = col;
        }
    }
}

static void h_draw(void *u, uint32_t cel, int frame, int x, int y, int flipped, int bg)
{
    MogFight *f = u;
    (void)flipped;
    draw_frame(f, bg ? f->bg : f->pix, cel, frame, x, y);
    if (bg)
        draw_frame(f, f->pix, cel, frame, x, y);
    f->draws++;
}

static void h_sound(void *u, int n)
{
    MogFight *f = u;
    if (f->sound)
        f->sound(f->user, n);
}

static void h_voice(void *u, int ch, int n)
{
    MogFight *f = u;
    if (f->voice)
        f->voice(f->user, ch, n);
}

static void h_palette(void *u, const uint16_t *c)
{
    MogFight *f = u;
    memcpy(f->pal, c, sizeof f->pal);
}

void mog_fight_render(const MogFight *f, uint32_t *fb)
{
    uint32_t pal[32];
    for (int i = 0; i < 32; i++) {
        uint16_t c = f->pal[i];
        pal[i] = 0xFF000000u | (uint32_t)((c >> 8) & 15) * 0x110000u
               | (uint32_t)((c >> 4) & 15) * 0x1100u | (uint32_t)(c & 15) * 0x11u;
    }
    for (int i = 0; i < MOG_SCREEN_W * MOG_SCREEN_H; i++)
        fb[i] = pal[f->pix[i] & 31];
}

/* ------------------------------------------------------------------ */
/* Démarrage                                                           */
/* ------------------------------------------------------------------ */

int mog_fight_boot(MogFight *f)
{
    if (f->booted)
        return 0;
    memset(&f->vm, 0, sizeof f->vm);
    if (mog_boot_memory(VM) < 0)
        return -1;
    mog_boot_graphics(VM);                              /* SECSTRT_30, SECSTRT_28 */
    ix_wl(VM, MOG_LAB_0E93, MOG_LAB_08D6);              /* LAB_0E53 : palette courante */
    mog_boot_engine(VM);                                /* LAB_0303 */
    mog_hit_init(VM);
    mog_boot_knight_cels(VM);                           /* LAB_0115 */
    mog_boot_backgrounds(VM);                           /* LAB_013A */
    ix_ww(VM, MOG_LAB_05C5, 1);
    mog_boot_tables(VM);                                /* LAB_0152 / LAB_0156 */

    static const IxHost host = { NULL, NULL, h_draw, h_sound, NULL, NULL };
    IxHost h = host;
    h.user = f;
    mog_combat_init(&f->m, VM, &h);
    f->m.voice = h_voice;
    f->m.palette = h_palette;
    mog_pointer_boot(&f->m);                            /* LAB_0572 */
    mog_new_game(&f->m);                                /* LAB_01AE, LAB_01BE, LAB_0011 */
    mog_boot_reactions(VM);                             /* LAB_020F */
    f->booted = 1;
    return 0;
}

static uint32_t knight_obj(int i)
{
    return MOG_LAB_0613 + (uint32_t)(i & 3) * IX_OBJECT_SIZE;
}

int mog_fight_start(MogFight *f, const MogFightSetup *s)
{
    if (mog_fight_boot(f) < 0)
        return -1;
    MogCombat *m = &f->m;
    uint32_t k = knight_obj(s->knight);
    f->player = k;
    ix_wb(VM, k + 77, 12);                              /* Ctl_HumanKnight */
    ix_wb(VM, k + 11, 1);                               /* joystick 1 */
    ix_wl(VM, k + 54, (uint32_t)(s->knight & 3));       /* couleurs */
    ix_wb(VM, k + 70, (uint8_t)(s->strength > 0 ? s->strength : 1));
    ix_wb(VM, k + 71, (uint8_t)(s->constitution > 0 ? s->constitution : 1));
    ix_wb(VM, k + 72, (uint8_t)(s->endurance > 0 ? s->endurance : 1));
    ix_wb(VM, k + 73, (uint8_t)(s->lives > 0 ? s->lives : 1));
    ix_ww(VM, k + 74, (uint16_t)s->gold);
    ix_wb(VM, k + 76, (uint8_t)s->daggers);
    ix_ww(VM, k + 80, 0x7FFF);
    mog_update_knight(m, k);                            /* LAB_0011 : PV max */
    if (s->hp > 0 && s->hp < sw(ix_rw(VM, k + 84)))
        ix_ww(VM, k + 80, (uint16_t)s->hp);
    ix_wl(VM, MOG_v_Combatants, k);
    ix_wl(VM, MOG_LAB_0633, k);

    f->foe = 0;
    if (s->opponent >= 0 && s->opponent != s->knight) {
        uint32_t o = knight_obj(s->opponent);
        f->foe = o;
        if (s->opponent_human) {
            ix_wb(VM, o + 77, 12);
            ix_wb(VM, o + 11, 2);                       /* joystick 2 */
            ix_wl(VM, o + 54, (uint32_t)(s->opponent & 3));
        } else {
            ix_wb(VM, o + 77, 0x10);                    /* LAB_0EFF */
            ix_wb(VM, o + 11, 4);
            ix_wl(VM, o + 54, 4);
        }
        ix_wb(VM, o + 70, (uint8_t)(s->opp_strength > 0 ? s->opp_strength : 1));
        ix_wb(VM, o + 71, (uint8_t)(s->opp_constitution > 0 ? s->opp_constitution : 1));
        ix_wb(VM, o + 72, (uint8_t)(s->opp_endurance > 0 ? s->opp_endurance : 1));
        ix_ww(VM, o + 80, 0x7FFF);
        mog_update_knight(m, o);
        if (s->opp_hp > 0 && s->opp_hp < sw(ix_rw(VM, o + 84)))
            ix_ww(VM, o + 80, (uint16_t)s->opp_hp);
        ix_wl(VM, MOG_v_Combatants + 4, o);
    }

    ix_wl(VM, MOG_LAB_08C4, (uint32_t)(s->place & 12));
    ix_wl(VM, MOG_LAB_076D, 0);
    uint32_t init = ix_rl(VM, MOG_t_CreatureInit + (uint32_t)(s->encounter & 0x7C));
    if (!mog_encounter_init(m, init))
        return -1;
    mog_combat_begin(m);
    planes_to_index(VM, ix_rl(VM, MOG_LAB_05C0), f->bg);
    memcpy(f->pix, f->bg, sizeof f->pix);
    f->running = 1;
    f->frame = 0;
    return 0;
}

int mog_fight_frame(MogFight *f, uint16_t joy0, uint16_t joy1)
{
    if (!f->running)
        return 0;
    memcpy(f->pix, f->bg, sizeof f->pix);
    f->m.joy[0] = joy0;
    f->m.joy[1] = joy1;
    f->m.vhposr = (uint16_t)(f->m.vhposr * 75u + 74u + f->frame);   /* faisceau : hasard */
    /* une image = v_FrameVbls VBL */
    ix_wl(VM, MOG_v_VblCounter, ix_rl(VM, MOG_v_VblCounter) + (uint32_t)mog_fight_frame_vbls(f));
    f->running = mog_combat_frame(&f->m);
    f->frame++;
    return f->running;
}

int mog_fight_frame_vbls(const MogFight *f)
{
    int n = sw(ix_rw((IxVM *)&f->vm, MOG_v_FrameVbls));
    return n > 0 ? n : 1;
}

int mog_fight_player_hp(const MogFight *f)
{
    return sw(ix_rw((IxVM *)&f->vm, f->player + 80));
}

int mog_fight_player_max_hp(const MogFight *f)
{
    return sw(ix_rw((IxVM *)&f->vm, f->player + 84));
}

int mog_fight_player_gold(const MogFight *f)
{
    return ix_rw((IxVM *)&f->vm, f->player + 74);
}

/* Combat_CheckKO : PV <= 0, le chevalier est à terre */
int mog_fight_won(const MogFight *f)
{
    return mog_fight_player_hp(f) > 0;
}

int mog_fight_foe_hp(const MogFight *f)
{
    return f->foe ? sw(ix_rw((IxVM *)&f->vm, f->foe + 80)) : 0;
}
