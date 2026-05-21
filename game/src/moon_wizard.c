/*
 * moon_wizard.c — Math the Wizard tower event for Moonstone
 *
 * The wizard tower (node 0x1e, "Visit Math the Wizard") is a random
 * event, NOT a shop.  The assembler source (LAB_007C → LAB_0456,
 * mog.asm §9) shows:
 *
 *   1. An introduction text is displayed.
 *   2. A random outcome is determined based on a corruption counter
 *      (knight->wizard_visited, equiv. to 83(knight) in KnightStruct):
 *      - combined value ≤ 30  : grant a random item / stat            (LAB_0461)
 *      - combined value ≤ 70  : upgrade a stat (str/con/end)          (LAB_0462)
 *      - combined value ≤ 90  : grant gold                            (LAB_045F)
 *      - combined value  > 90 : TOAD transformation (malus)           (LAB_045E frog)
 *   3. The outcome text is displayed.
 *   4. knight->wizard_visited is advanced (set to 70 after first
 *      visit so subsequent visits lean toward malus).
 *
 * Text strings decoded from mog.asm LAB_091E / LAB_092D / LAB_0934.
 */

#include "moon_wizard.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* ------------------------------------------------------------------ */
/* Random helper (simple LCG seeded from time)                        */
/* ------------------------------------------------------------------ */
static unsigned int s_rng_seed = 0;
static unsigned int wizard_rand(void)
{
    if (!s_rng_seed) s_rng_seed = (unsigned int)time(NULL);
    s_rng_seed = s_rng_seed * 1664525u + 1013904223u;
    return s_rng_seed;
}

/* Return a value 0..255 (equiv. to LAB_04A3 in the assembler) */
static int random_byte(void)
{
    return (int)(wizard_rand() & 0xFF);
}

/* ------------------------------------------------------------------ */
/* Outcome constants (LAB_090B values)                                */
/* ------------------------------------------------------------------ */
#define OUTCOME_STAT_ITEM   1  /* grant item/stat bonus      */
#define OUTCOME_GOLD        2  /* grant gold                 */
#define OUTCOME_STAT_UP     3  /* upgrade a combat stat      */
#define OUTCOME_TOAD        4  /* transformation into a toad */

/* ------------------------------------------------------------------ */
/* Drawing                                                             */
/* ------------------------------------------------------------------ */

static void draw_wizard_screen(GameCtx *ctx,
                                const char *title,
                                const char **lines, int n_lines,
                                uint32_t color)
{
    render_clear(ctx->fb, 0xFF000000u);
    render_fill_rect(ctx->fb, 15, 8, GAME_W - 30, GAME_H - 16, 0xCC050520u);
    render_text_centered(ctx->fb, title, 14, 0xFFAA44FFu);

    int y = 28;
    for (int i = 0; i < n_lines; i++) {
        render_text_centered(ctx->fb, lines[i], y, color);
        y += 10;
    }
    render_text_centered(ctx->fb, "Press FIRE to continue",
                         GAME_H - 12, 0xFF555555u);
}

static void wait_fire(GameCtx *ctx)
{
    /* Drain any held fire first */
    for (int i = 0; i < 4; i++) {
        hal_poll(&ctx->input);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }
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

void game_run_wizard(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];

    /* ---- 1. Introduction text (LAB_091E) ---- */
    {
        const char *intro_lines[] = {
            "As you ring the bell at the",
            "bottom of the foreboding",
            "wizard's tower, a sense of",
            "unease rises in the air.",
            "The mighty wizard Math speaks:",
        };
        draw_wizard_screen(ctx, "MATH THE WIZARD",
                           intro_lines, 5, 0xFF888888u);
        hal_present(ctx->fb);
        wait_fire(ctx);
        if (ctx->input.quit || ctx->input.escape) {
            ctx->state = STATE_OVERWORLD;
            return;
        }
    }

    /* ---- 2. Determine outcome ---- */
    int rnd = random_byte();
    int combined = rnd + (int)k->wizard_visited;
    if (combined > 255) combined = 255;

    int outcome;
    if      (combined <= 30) outcome = OUTCOME_STAT_ITEM;
    else if (combined <= 70) outcome = OUTCOME_STAT_UP;
    else if (combined <= 90) outcome = OUTCOME_GOLD;
    else                     outcome = OUTCOME_TOAD;

    /* ---- 3. Apply effect and show outcome text ---- */
    char stat_msg[64] = "";

    if (outcome == OUTCOME_STAT_ITEM) {
        /* Grant endurance item bonus (LAB_0461 / LAB_046C) */
        int bonus = 10 + (wizard_rand() & 0x15); /* 10–31 */
        k->xp += bonus;
        snprintf(stat_msg, sizeof(stat_msg),
                 "+%d experience granted!", bonus);
        const char *bonus_lines[] = {
            "The cosmos has granted you",
            "new skills and agility.",
            stat_msg,
        };
        draw_wizard_screen(ctx, "MATH THE WIZARD",
                           bonus_lines, 3, 0xFF44FF44u);
        hal_present(ctx->fb);
        wait_fire(ctx);

    } else if (outcome == OUTCOME_STAT_UP) {
        /* Upgrade a combat stat (str/con/end) (LAB_0462) */
        /* Choose which stat to raise: cycle among strength/constitution/endurance */
        int stat_choice = wizard_rand() % 3;
        const char *stat_name;
        if (stat_choice == 0 && k->strength < 5) {
            k->strength++;
            stat_name = "strength";
        } else if (stat_choice == 1 && k->constitution < 5) {
            k->constitution++;
            k->max_hp += 5;
            if (k->hp < k->max_hp) k->hp = k->max_hp;
            stat_name = "constitution";
        } else if (k->endurance < 5) {
            k->endurance++;
            stat_name = "endurance";
        } else {
            /* All stats at max (LAB_096C) */
            const char *max_lines[] = {
                "You have already reached",
                "skills and agilities that",
                "even I can no longer",
                "raise or improve upon.",
                "Now go and complete your quest",
                "before the Black Knights",
                "fulfil their treachery.",
            };
            draw_wizard_screen(ctx, "MATH THE WIZARD",
                               max_lines, 7, 0xFFCCCC44u);
            hal_present(ctx->fb);
            wait_fire(ctx);
            ctx->state = STATE_OVERWORLD;
            return;
        }
        snprintf(stat_msg, sizeof(stat_msg),
                 "The cosmos has granted you more %s.", stat_name);
        const char *up_lines[] = {
            stat_msg,
        };
        draw_wizard_screen(ctx, "MATH THE WIZARD",
                           up_lines, 1, 0xFF44CCFFu);
        hal_present(ctx->fb);
        wait_fire(ctx);

    } else if (outcome == OUTCOME_GOLD) {
        /* Grant gold (LAB_045F) */
        int gold_gift = 10 + (wizard_rand() & 0x15);
        k->gold += gold_gift;
        snprintf(stat_msg, sizeof(stat_msg),
                 "The wizard donates %d gold.", gold_gift);
        const char *gold_lines[] = {
            "The wizard reaches into his",
            "coat and offers a gift...",
            stat_msg,
        };
        draw_wizard_screen(ctx, "MATH THE WIZARD",
                           gold_lines, 3, 0xFFFFCC00u);
        hal_present(ctx->fb);
        wait_fire(ctx);

    } else {
        /* TOAD transformation (LAB_045E → LAB_092D / LAB_0934) */
        /* Two possible toad texts depending on whether this is a
         * first insult or the full transformation sequence */
        const char **toad_lines;
        int n_lines;

        if (k->wizard_visited == 0) {
            /* First visit gone wrong: insulted (LAB_092D) */
            static const char *insult_lines[] = {
                "You insulant little worm!",
                "How dare you abuse my",
                "abundant warmth!",
                "One who acts as selfish as you",
                "deserves the gifts of a TOAD!",
            };
            toad_lines = insult_lines;
            n_lines    = 5;
        } else {
            /* Repeat offender: full transformation (LAB_0934) */
            static const char *transform_lines[] = {
                "A strange feeling spreads",
                "throughout your body and",
                "the world starts to grow",
                "all about you. When suddenly",
                "you see it is you being",
                "transformed into a TOAD!",
            };
            toad_lines = transform_lines;
            n_lines    = 6;
        }

        draw_wizard_screen(ctx, "MATH THE WIZARD",
                           toad_lines, n_lines, 0xFFFF4444u);
        hal_present(ctx->fb);
        wait_fire(ctx);

        /* Apply malus: mark knight as frog, reduce stats */
        k->is_frog = 1;
        if (k->strength > 1)     k->strength--;
        if (k->constitution > 1) k->constitution--;
        /* HP penalty */
        k->max_hp -= 5;
        if (k->max_hp < 1)   k->max_hp = 1;
        if (k->hp > k->max_hp) k->hp = k->max_hp;
    }

    /* ---- 4. Advance wizard corruption counter ---- */
    /* Set to 0x46 (70) after any visit — same as "MOVE.B #$46,83(A0)"
     * in LAB_045D — so future visits lean towards malus/toad. */
    k->wizard_visited = 0x46;

    ctx->state = STATE_OVERWORLD;
}
