/*
 * moon_combat.c — Combat mode for Moonstone
 *
 * Implements the real-time combat as described in DOC_MODE_COMBAT.md.
 *
 * Combat states (LAB_068F):
 *   1 = PvP knight vs knight
 *   2 = PvE knight vs creature
 *   3 = Mystic (Mythral)
 *   5 = Arena in city
 *  10 = Valley of Gods (boss)
 *
 * Controls (player):
 *   Left/Right  — move
 *   Up          — jump
 *   Fire        — attack (sword swing)
 *   Fire2       — block / special
 */

#include "moon_combat.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Combat constants                                                    */
/* ------------------------------------------------------------------ */

#define COMBAT_GROUND_Y  150  /* Y pixel of the ground plane         */
#define GRAVITY          1    /* pixels/frame downward acceleration   */
#define JUMP_VEL        -8    /* initial jump velocity                */
#define MOVE_SPEED       2    /* horizontal move speed                */
#define ATTACK_RANGE    32    /* horizontal range of sword swing      */
#define ATTACK_DAMAGE   10    /* hit point loss per sword hit         */

/* ------------------------------------------------------------------ */
/* Combatant state                                                     */
/* ------------------------------------------------------------------ */

typedef enum {
    CSTATE_IDLE   = 0,
    CSTATE_WALK,
    CSTATE_JUMP,
    CSTATE_ATTACK,
    CSTATE_BLOCK,
    CSTATE_HIT,
    CSTATE_DEAD
} CombatState;

typedef struct {
    int          hp;
    int          max_hp;
    int          x;          /* screen X of combatant                */
    int          y;          /* screen Y of combatant                */
    int          vel_y;      /* vertical velocity for jump           */
    int          facing;     /* 1 = right, -1 = left                 */
    CombatState  state;
    int          state_timer;/* frames remaining in current state     */
    int          attack_timer;
    int          hit_flash;  /* frames to show hit flash             */
    int          human;      /* 1 = player-controlled                */
    int          knight_idx; /* index into ctx->knights[]            */
    const char  *name;
} Combatant;

/* ------------------------------------------------------------------ */
/* Combat backgrounds                                                  */
/* ------------------------------------------------------------------ */

/* Maps node_type to a background PIV file (from DOC_MODE_COMBAT.md §2) */
static const char *combat_bg_for_type(int node_type)
{
    switch (node_type) {
    case 0x01: case 0x21: return "bg5a.PIV"; /* PvP */
    case 0x02:             return "bg3.PIV";  /* creature */
    case 0x1b:             return "bg4.PIV";  /* Stonehenge/Mythral */
    case 0x1c:             return "bg4.PIV";  /* Valley of Gods */
    default:               return "bg2a.PIV"; /* default/arena */
    }
}

/* ------------------------------------------------------------------ */
/* Draw a simple combatant rectangle (sprite placeholder)             */
/* ------------------------------------------------------------------ */

static void draw_combatant(uint32_t *fb, const Combatant *c,
                           uint32_t color)
{
    int w = 16, h = 32;
    int x = c->x - w / 2;
    int y = c->y - h;

    /* Body */
    render_fill_rect(fb, x, y, w, h, color);

    /* Head */
    render_fill_rect(fb, x + 2, y - 12, 12, 12, color);

    /* Hit flash: white overlay */
    if (c->hit_flash > 0)
        render_fill_rect(fb, x - 2, y - 14, w + 4, h + 14, 0x88FFFFFFu);
}

static void draw_hp_bar(uint32_t *fb, int x, int y, int hp, int max_hp,
                        uint32_t color, const char *name)
{
    int bar_w = 80;
    int filled = (max_hp > 0) ? bar_w * hp / max_hp : 0;
    if (filled < 0) filled = 0;
    if (filled > bar_w) filled = bar_w;

    /* Background */
    render_fill_rect(fb, x, y, bar_w, 6, 0xFF222222u);
    /* Filled portion */
    render_fill_rect(fb, x, y, filled, 6, color);
    /* Border */
    render_fill_rect(fb, x, y, bar_w, 1, 0xFF888888u);
    render_fill_rect(fb, x, y + 5, bar_w, 1, 0xFF888888u);

    /* Name */
    render_text(fb, name, x, y - 10, color);
}

/* ------------------------------------------------------------------ */
/* Combat logic helpers                                                */
/* ------------------------------------------------------------------ */

static void apply_gravity(Combatant *c)
{
    c->y += c->vel_y;
    c->vel_y += GRAVITY;
    if (c->y >= COMBAT_GROUND_Y) {
        c->y      = COMBAT_GROUND_Y;
        c->vel_y  = 0;
        if (c->state == CSTATE_JUMP)
            c->state = CSTATE_IDLE;
    }
}

static int check_hit(const Combatant *attacker, const Combatant *defender)
{
    int ax = attacker->x + (attacker->facing > 0 ? ATTACK_RANGE : -ATTACK_RANGE);
    int dx = defender->x - ax;
    int dy = defender->y - attacker->y;
    return (dx > -ATTACK_RANGE && dx < ATTACK_RANGE &&
            dy > -48 && dy < 16);
}

static void ai_update(Combatant *ai, const Combatant *player)
{
    if (ai->state == CSTATE_DEAD || ai->state == CSTATE_HIT) return;
    if (ai->state_timer > 0) { ai->state_timer--; return; }

    int dx = player->x - ai->x;
    ai->facing = (dx > 0) ? 1 : -1;
    int dist = dx > 0 ? dx : -dx;

    if (dist > 40) {
        /* Move towards player */
        ai->x += ai->facing * MOVE_SPEED;
        ai->state = CSTATE_WALK;
    } else {
        /* Attack */
        ai->state       = CSTATE_ATTACK;
        ai->state_timer = 20;
    }
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_combat(GameCtx *ctx)
{
    /* Set up combatants */
    Combatant player, enemy;
    memset(&player, 0, sizeof(player));
    memset(&enemy,  0, sizeof(enemy));

    Knight *pk = &ctx->knights[ctx->current_knight];
    player.hp         = pk->hp;
    player.max_hp     = pk->max_hp;
    player.x          = 80;
    player.y          = COMBAT_GROUND_Y;
    player.facing     = 1;
    player.state      = CSTATE_IDLE;
    player.human      = 1;
    player.knight_idx = ctx->current_knight;
    player.name       = (const char *[]){ "RICHARD","GODBER","JEFFREY","EDWARD" }[pk->id];

    /* Enemy HP depends on combat type */
    int enemy_hp = 80;
    const char *enemy_name = "CREATURE";
    if (ctx->node_type == 0x01 || ctx->node_type == 0x21) {
        enemy_name = "KNIGHT";
        enemy_hp   = 100;
    } else if (ctx->node_type == 0x1c) {
        enemy_name = "VALLEY GOD";
        enemy_hp   = 200;
    }
    enemy.hp      = enemy_hp;
    enemy.max_hp  = enemy_hp;
    enemy.x       = 240;
    enemy.y       = COMBAT_GROUND_Y;
    enemy.facing  = -1;
    enemy.state   = CSTATE_IDLE;
    enemy.human   = 0;
    enemy.name    = enemy_name;

    /* Load background */
    const char *bg_name = combat_bg_for_type(ctx->node_type);
    uint32_t bg[GAME_W * GAME_H];
    MoonPiv *piv = moon_piv_load(bg_name);
    if (!piv) {
        const char *alt = bg_name;
        /* Try lowercase variant */
        char lc_name[64];
        int li = 0;
        for (; alt[li] && li < 63; li++)
            lc_name[li] = (char)(alt[li] >= 'A' && alt[li] <= 'Z'
                           ? alt[li] + 32 : alt[li]);
        lc_name[li] = '\0';
        piv = moon_piv_load(lc_name);
    }
    if (piv) {
        render_piv_full(piv, bg);
        moon_piv_free(piv);
    } else {
        for (int i = 0; i < GAME_W * GAME_H; i++)
            bg[i] = 0xFF080808u;
    }

    /* Darken ground */
    render_fill_rect(bg, 0, COMBAT_GROUND_Y, GAME_W, GAME_H - COMBAT_GROUND_Y,
                     0xFF111111u);

    static const uint32_t knight_colors[MAX_PLAYERS] = {
        0xFF4444FFu, 0xFFFF4444u, 0xFF44FF44u, 0xFFFFFF44u
    };

    int prev_fire = 0, prev_fire2 = 0;

    while (ctx->state == STATE_COMBAT) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_OVERWORLD;
                return;
            }
        }

        /* ---- Player input ---- */
        int cur_fire  = ctx->input.joy[0].fire  || ctx->input.space;
        int cur_fire2 = ctx->input.joy[0].fire2;

        if (player.state != CSTATE_DEAD && player.state != CSTATE_HIT) {
            if (player.state_timer > 0) {
                player.state_timer--;
            } else {
                player.state = CSTATE_IDLE;

                /* Movement */
                if (ctx->input.joy[0].left || (ctx->input.keys &&
                        ctx->input.keys[80])) {
                    player.x -= MOVE_SPEED;
                    player.facing = -1;
                    player.state  = CSTATE_WALK;
                }
                if (ctx->input.joy[0].right || (ctx->input.keys &&
                        ctx->input.keys[79])) {
                    player.x += MOVE_SPEED;
                    player.facing = 1;
                    player.state  = CSTATE_WALK;
                }

                /* Jump */
                if ((ctx->input.joy[0].up || (ctx->input.keys &&
                        ctx->input.keys[82])) && player.y >= COMBAT_GROUND_Y) {
                    player.vel_y = JUMP_VEL;
                    player.state = CSTATE_JUMP;
                }

                /* Attack */
                if (cur_fire && !prev_fire) {
                    player.state       = CSTATE_ATTACK;
                    player.state_timer = 16;
                    /* Damage enemy if in range */
                    if (check_hit(&player, &enemy)) {
                        enemy.hp -= ATTACK_DAMAGE;
                        enemy.hit_flash = 4;
                        if (enemy.hp < 0) enemy.hp = 0;
                    }
                }

                /* Block */
                if (cur_fire2 && !prev_fire2) {
                    player.state       = CSTATE_BLOCK;
                    player.state_timer = 12;
                }
            }
        }

        prev_fire  = cur_fire;
        prev_fire2 = cur_fire2;

        /* ---- AI update ---- */
        if (enemy.state != CSTATE_DEAD) {
            ai_update(&enemy, &player);
            /* Enemy attacks player */
            if (enemy.state == CSTATE_ATTACK) {
                if (check_hit(&enemy, &player) && player.state != CSTATE_BLOCK) {
                    player.hp -= 5;
                    player.hit_flash = 4;
                    if (player.hp < 0) player.hp = 0;
                    player.state       = CSTATE_HIT;
                    player.state_timer = 8;
                }
            }
        }

        /* ---- Physics ---- */
        apply_gravity(&player);
        apply_gravity(&enemy);

        /* Clamp X to screen */
        if (player.x < 8)          player.x = 8;
        if (player.x > GAME_W - 8) player.x = GAME_W - 8;
        if (enemy.x  < 8)          enemy.x  = 8;
        if (enemy.x  > GAME_W - 8) enemy.x  = GAME_W - 8;

        /* Decrement hit flash */
        if (player.hit_flash > 0) player.hit_flash--;
        if (enemy.hit_flash  > 0) enemy.hit_flash--;

        /* ---- Death check ---- */
        if (player.hp <= 0) player.state = CSTATE_DEAD;
        if (enemy.hp  <= 0) enemy.state  = CSTATE_DEAD;

        /* ---- Render ---- */
        memcpy(ctx->fb, bg, sizeof(bg));

        draw_combatant(ctx->fb, &enemy, 0xFFCC4444u);
        draw_combatant(ctx->fb, &player,
                       knight_colors[ctx->current_knight]);

        /* HP bars */
        draw_hp_bar(ctx->fb, 10, 10, player.hp, player.max_hp,
                    knight_colors[ctx->current_knight], player.name);
        draw_hp_bar(ctx->fb, GAME_W - 90, 10, enemy.hp, enemy.max_hp,
                    0xFFCC4444u, enemy.name);

        /* Combat result messages */
        if (player.state == CSTATE_DEAD) {
            render_text_centered(ctx->fb, "YOU DIED", GAME_H / 2,     0xFFFF2222u);
            render_text_centered(ctx->fb, "Press any key", GAME_H / 2 + 12, 0xFF888888u);
        } else if (enemy.state == CSTATE_DEAD) {
            render_text_centered(ctx->fb, "VICTORY!", GAME_H / 2,     0xFF44FF44u);
            render_text_centered(ctx->fb, "Press any key", GAME_H / 2 + 12, 0xFF888888u);
        }

        hal_present(ctx->fb);
        hal_vbl_wait();

        /* ---- Combat end ---- */
        if (player.state == CSTATE_DEAD || enemy.state == CSTATE_DEAD) {
            /* Wait for keypress */
            int waited = 0;
            while (waited < 150) {
                if (hal_poll(&ctx->input)) break;
                if (ctx->input.enter || ctx->input.space ||
                    ctx->input.joy[0].fire) break;
                hal_present(ctx->fb);
                hal_vbl_wait();
                waited++;
            }

            /* Apply combat results */
            if (player.state == CSTATE_DEAD) {
                pk->hp = 0;
                pk->dead = 1;
                /* Gold penalty */
                pk->gold /= 2;
            } else {
                /* Victory: restore some HP, gain gold */
                pk->hp = player.hp;
                pk->gold += 20;
                pk->xp   += 50;
            }

            ctx->state = STATE_OVERWORLD;
            return;
        }
    }
}
