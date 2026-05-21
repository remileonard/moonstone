/*
 * moon_valley.c — Valley of the Gods (node 0x1c) for Moonstone
 *
 * Implements LAB_009D (mog.asm#L1525):
 *
 *   1. Require all 4 Valley keys (inventaire[20] == 0x0f).
 *      If the knight is missing at least one key → display refusal
 *      message and return to overworld (LAB_00B3).
 *
 *   2. If all 4 keys present:
 *      a. Load Valley background (LAB_0DBD).
 *      b. Trigger final boss combat (LAB_01A0 → STATE_COMBAT).
 *      c. After combat:
 *         - Defeat (LAB_05DC bit 0 = 1): skill_level -= 2, respawn.
 *         - Victory (LAB_05DC bit 0 = 0): battles_fought += 3,
 *           clear the 4 keys (inventaire[20] = 0), then trigger
 *           the ending sequence (LAB_0DCA → STATE_ENDING).
 *
 * The function is entered once with ctx->node_type == 0x1c.
 * After combat STATE_COMBAT returns here to resolve the outcome.
 */

#include "moon_valley.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>

/* ------------------------------------------------------------------ */
/* Helpers                                                             */
/* ------------------------------------------------------------------ */

static void draw_valley_screen(GameCtx *ctx,
                                const char **lines, int n,
                                uint32_t color)
{
    MoonPiv *piv = moon_piv_load("bg6.PIV");
    if (!piv) piv = moon_piv_load("bg6.piv");
    if (piv) { render_piv_full(piv, ctx->fb); moon_piv_free(piv); }
    else       render_clear(ctx->fb, 0xFF050520u);

    render_fill_rect(ctx->fb, 10, 10, GAME_W - 20, GAME_H - 20, 0xAA000000u);

    int y = (GAME_H - n * 12) / 2;
    for (int i = 0; i < n; i++) {
        render_text_centered(ctx->fb, lines[i], y, color);
        y += 12;
    }
    render_text_centered(ctx->fb, "Press FIRE to continue",
                         GAME_H - 12, 0xFF555555u);
}

static void wait_for_fire(GameCtx *ctx)
{
    for (;;) {
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) return;
        }
        if (ctx->input.joy[0].fire || ctx->input.enter || ctx->input.space)
            return;
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

/*
 * game_run_valley — handle the Valley of the Gods node (0x1c).
 *
 * This function implements LAB_009D.  It is called from game_run()
 * when ctx->state == STATE_VALLEY.
 *
 * Two-phase design (mirrors the ASM):
 *   Phase 1 — entered directly from overworld (ctx->node_type == 0x1c):
 *     check keys, show intro, then set state = STATE_COMBAT.
 *   Phase 2 — re-entered after combat returns STATE_VALLEY:
 *     resolve the combat outcome (victory → ending, defeat → respawn).
 *
 * ctx->node_target_knight carries the result flag set by moon_combat.c:
 *   0 = player won, 1 = player lost  (mirrors LAB_05DC bit 0).
 */
void game_run_valley(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];

    /* ----------------------------------------------------------------
     * Phase 1 — pre-combat: key check + intro screen
     * ---------------------------------------------------------------- */
    if (ctx->node_type == 0x1c) {

        /* LAB_009D: CMPI.B #$0f, 20(A0) — require all 4 Valley keys */
        if ((k->keys & 0x0f) != 0x0f) {
            /* Missing at least one key → refuse entry (LAB_00B3) */
            char key_msg[80];
            snprintf(key_msg, sizeof(key_msg),
                     "You have %d of 4 keys.",
                     __builtin_popcount(k->keys & 0x0f));
            const char *no_key[] = {
                "VALLEY OF THE GODS",
                "",
                "You must have all four keys",
                "to enter the Valley of the Gods.",
                "",
                key_msg,
                "Seek the Black Knights to claim",
                "the missing keys.",
            };
            draw_valley_screen(ctx, no_key, 8, 0xFF888888u);
            hal_present(ctx->fb);
            wait_for_fire(ctx);
            ctx->state = STATE_OVERWORLD;
            return;
        }

        /* All 4 keys present — show intro, then trigger boss combat */
        const char *pre_lines[] = {
            "VALLEY OF THE GODS",
            "",
            "You possess all four keys!",
            "The gates of the Valley",
            "of the Gods swing open...",
            "Only the strongest knight",
            "may claim the Moonstone."
        };
        draw_valley_screen(ctx, pre_lines, 7, 0xFFFFCC44u);
        hal_present(ctx->fb);
        wait_for_fire(ctx);

        /* Trigger final boss combat.
         * node_type stays 0x1c so game_run_combat knows the context.
         * After combat, moon_combat.c sets state = STATE_VALLEY so we
         * are re-entered for phase 2. */
        ctx->node_type = 0x1c;
        ctx->state     = STATE_COMBAT;
        return;
    }

    /* ----------------------------------------------------------------
     * Phase 2 — post-combat resolution
     * Entered when moon_combat.c sets ctx->state = STATE_VALLEY after
     * a 0x1c combat.
     * node_target_knight == 0 → player won
     * node_target_knight == 1 → player lost (mirrors LAB_05DC bit 0)
     * ---------------------------------------------------------------- */
    int player_lost = ctx->node_target_knight; /* 0 = won, 1 = lost */

    if (player_lost) {
        /* Defeat (LAB_009F): skill_level -= 2, return to overworld */
        k->xp -= 2;
        if (k->xp < 0) k->xp = 0;

        const char *defeat_lines[] = {
            "VALLEY OF THE GODS",
            "",
            "You have been defeated...",
            "The Valley's power diminishes you.",
            "Return when you are stronger."
        };
        draw_valley_screen(ctx, defeat_lines, 5, 0xFF884444u);
        hal_present(ctx->fb);
        wait_for_fire(ctx);

        ctx->state = STATE_OVERWORLD;
        return;
    }

    /* Victory (LAB_00A0):
     *   battles_fought += 3  →  ctx->round used as approximation
     *   inventaire[20] = 0   →  k->keys = 0
     *   → LAB_0DCA           →  STATE_ENDING */
    k->keys = 0;   /* clear all 4 Valley keys (MOVE.B #$00, 20(A0)) */

    const char *victory_lines[] = {
        "VALLEY OF THE GODS",
        "",
        "The Gods are pleased!",
        "The Moonstone is revealed...",
        "Victory is yours!"
    };
    draw_valley_screen(ctx, victory_lines, 5, 0xFFFFDD44u);
    hal_present(ctx->fb);
    wait_for_fire(ctx);

    k->has_moonstone = 1;
    ctx->state = STATE_ENDING;
}
