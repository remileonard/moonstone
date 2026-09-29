/*
 * moon_combat.c — mode combat : passerelle entre le jeu (GameCtx) et le
 * combat de mog porté en C (mog_fight.c, docs/DOC_MOTEUR_COMBAT_MOG.md).
 *
 * Le chevalier du joueur est rempli d'après GameCtx (force, constitution,
 * endurance, PV, or, vies), la rencontre choisie comme dans mog :
 *   - créature (node_type 0x02) : routine t_CreatureInit[pve_creature_type],
 *     décor et terrain du groupe du lieu (pve_node_group) ;
 *   - chevalier contre chevalier (0x01, 0x21) : LAB_0164, adversaire
 *     humain (joystick 2) ou chevalier noir (IA LAB_0EFF) ;
 *   - Vallée des Dieux (0x1c) : Démon (LAB_01A0).
 * Chaque image dure v_FrameVbls VBL (6, soit un peu plus de 8 images par
 * seconde, comme l'original). Fin : PV <= 0, le chevalier perd une vie et
 * retrouve ses PV (Combat_CheckKO).
 */
#include "moon_combat.h"
#include "moon_hal.h"
#include "mog_fight.h"

#include <string.h>

static MogFight s_fight;

/* Groupe du lieu (0 fol, 1 wal, 2 swl, 3 gll) -> LAB_08C4 */
static int place_of_group(int g)
{
    static const int place[4] = { MOG_PLACE_FOREST, MOG_PLACE_WASTE,
                                  MOG_PLACE_SWAMP, MOG_PLACE_GRASS };
    return place[g & 3];
}

static uint16_t read_joy(const MoonInput *in, int port)
{
    uint16_t j = 0;
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
    if (right) j |= MOG_JOY_RIGHT;
    if (left)  j |= MOG_JOY_LEFT;
    if (down)  j |= MOG_JOY_DOWN;
    if (up)    j |= MOG_JOY_UP;
    if (fire)  j |= MOG_JOY_FIRE;
    return j;
}

/* Adversaire d'un combat entre chevaliers */
static int pick_opponent(const GameCtx *ctx)
{
    int t = ctx->node_target_knight;
    if (t >= 0 && t < MAX_PLAYERS && t != ctx->current_knight && ctx->knights[t].active)
        return t;
    for (int i = 0; i < MAX_PLAYERS; i++)
        if (i != ctx->current_knight && ctx->knights[i].active && !ctx->knights[i].dead)
            return i;
    return (ctx->current_knight + 1) % MAX_PLAYERS;
}

void game_run_combat(GameCtx *ctx)
{
    Knight *pk = &ctx->knights[ctx->current_knight];
    MogFightSetup s;
    memset(&s, 0, sizeof s);
    s.knight = pk->id;
    s.strength = pk->strength;
    s.constitution = pk->constitution;
    s.endurance = pk->endurance;
    s.hp = pk->hp;
    s.gold = pk->gold;
    s.lives = pk->lives > 0 ? pk->lives : 5;
    s.daggers = 10;
    s.opponent = -1;
    s.place = place_of_group(ctx->pve_node_group);

    int opp = -1;
    if (ctx->node_type == 0x01 || ctx->node_type == 0x21) {
        opp = pick_opponent(ctx);
        const Knight *ok = &ctx->knights[opp];
        s.encounter = MOG_ENC_KNIGHT;
        s.opponent = ok->id;
        s.opponent_human = ok->human && !ok->is_black_knight;
        s.opp_strength = ok->strength;
        s.opp_constitution = ok->constitution;
        s.opp_endurance = ok->endurance;
        s.opp_hp = ok->hp;
    } else if (ctx->node_type == 0x1c) {
        s.encounter = MOG_ENC_DEMON;
        s.place = MOG_PLACE_WASTE;
    } else {
        s.encounter = ctx->pve_creature_type & 0x7C;
        if (s.encounter == MOG_ENC_KNIGHT || s.encounter == 16 || s.encounter == 56) {
            s.opponent = (pk->id + 1) & 3;              /* chevalier noir */
            s.opp_strength = s.opp_constitution = s.opp_endurance = 2;
        }
    }

    if (mog_fight_start(&s_fight, &s) < 0) {
        ctx->state = STATE_OVERWORLD;
        return;
    }

    int quit = 0;
    while (ctx->state == STATE_COMBAT) {
        if (hal_poll(&ctx->input) && (ctx->input.quit || ctx->input.escape)) {
            quit = 1;
            break;
        }
        int running = mog_fight_frame(&s_fight, read_joy(&ctx->input, 0),
                                      read_joy(&ctx->input, 1));
        mog_fight_render(&s_fight, ctx->fb);
        hal_present(ctx->fb);
        for (int v = mog_fight_frame_vbls(&s_fight); v > 0; v--)
            hal_vbl_wait();
        if (!running)
            break;
    }
    if (quit) {
        ctx->state = STATE_OVERWORLD;
        return;
    }

    /* Résultats (Combat_CheckKO) */
    int won = mog_fight_won(&s_fight);
    pk->gold = mog_fight_player_gold(&s_fight);
    pk->max_hp = mog_fight_player_max_hp(&s_fight);
    if (won) {
        pk->hp = mog_fight_player_hp(&s_fight);
    } else {
        pk->hp = pk->max_hp;
        pk->lives = s.lives - 1;
        if (pk->lives <= 0)
            pk->dead = 1;
    }
    if (opp >= 0) {
        Knight *ok = &ctx->knights[opp];
        int hp = mog_fight_foe_hp(&s_fight);
        if (hp > 0) {
            ok->hp = hp;
        } else {
            ok->lives = (ok->lives > 0 ? ok->lives : 5) - 1;
            if (ok->lives <= 0)
                ok->dead = 1;
        }
    }
    if (won && ctx->node_type == 0x02) {
        /* créature vaincue : clé de la Vallée si elle en gardait une */
        int pve_idx = ctx->node_target_knight;
        if (pve_idx >= 0 && pve_idx < 24) {
            extern int overworld_pve_take_key(int node_idx, Knight *k);
            overworld_pve_take_key(pve_idx, pk);
        }
    }

    if (ctx->node_type == 0x1c) {
        ctx->node_target_knight = won ? 0 : 1;
        ctx->state = STATE_VALLEY;
    } else {
        ctx->state = STATE_OVERWORLD;
    }
}
