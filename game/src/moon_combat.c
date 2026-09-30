/*
 * moon_combat.c — combats seuls (--combat) : le combat de mog porté en C
 * (mog_fight.c), rencontre après rencontre, chevalier Sir Richard (force,
 * constitution, endurance 2), joystick = flèches + Espace ou manette.
 * Chaque image dure v_FrameVbls VBL, comme l'original.
 */
#include "moon_game.h"
#include "mog_fight.h"

#include <string.h>

static MogFight s_fight;

static uint16_t read_joy(const MoonInput *in, int port)
{
    int up = in->joy[port].up, down = in->joy[port].down;
    int left = in->joy[port].left, right = in->joy[port].right;
    int fire = in->joy[port].fire;
    if (port == 0 && in->keys) {                        /* flèches, espace */
        up |= in->keys[82];
        down |= in->keys[81];
        left |= in->keys[80];
        right |= in->keys[79];
        fire |= in->space;
    }
    uint16_t j = 0;
    if (right) j |= MOG_JOY_RIGHT;
    if (left)  j |= MOG_JOY_LEFT;
    if (down)  j |= MOG_JOY_DOWN;
    if (up)    j |= MOG_JOY_UP;
    if (fire)  j |= MOG_JOY_FIRE;
    return j;
}

void game_run_combats(GameCtx *ctx, int encounter, int all, int place)
{
    static const int list[] = { 12, 24, 20, 36, 48, 4, 64, 32, 0, 28, 8 };
    static const int places[4] = { MOG_PLACE_FOREST, MOG_PLACE_WASTE,
                                   MOG_PLACE_SWAMP, MOG_PLACE_GRASS };
    int lives = 5;
    for (int n = 0;; n++) {
        MogFightSetup s;
        memset(&s, 0, sizeof s);
        s.knight = 0;
        s.strength = s.constitution = s.endurance = 2;
        s.gold = 50;
        s.lives = lives;
        s.daggers = 10;
        s.opponent = -1;
        s.encounter = all ? list[n % 11] : encounter;
        s.place = places[(place >= 0 ? place : n) & 3];
        if (s.encounter == MOG_ENC_KNIGHT || s.encounter == 16 || s.encounter == 56) {
            s.opponent = 1;                             /* chevalier noir */
            s.opp_strength = s.opp_constitution = s.opp_endurance = 2;
        }
        if (mog_fight_start(&s_fight, &s) < 0)
            return;
        for (;;) {
            if (hal_poll(&ctx->input) && (ctx->input.quit || ctx->input.escape))
                return;
            int running = mog_fight_frame(&s_fight, read_joy(&ctx->input, 0),
                                          read_joy(&ctx->input, 1));
            mog_fight_render(&s_fight, ctx->fb);
            hal_present(ctx->fb);
            for (int v = mog_fight_frame_vbls(&s_fight); v > 0; v--)
                hal_vbl_wait();
            if (!running)
                break;
        }
        if (!mog_fight_won(&s_fight) && --lives <= 0)
            lives = 5;
    }
}
