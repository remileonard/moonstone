/*
 * moon_intro.c — Introduction sequence for Moonstone
 *
 * Implements the introduction cinematic as described in
 * DOC_ANIMATIONS_INTRO_FIN.md §3.
 *
 * Sequence summary:
 *   Plan 1  : Mindscape logo
 *   Plan 2  : Moon appears (bg1c.piv)
 *   Plans 3-4: Credits over moon background
 *   Plan 5  : Vertical scroll (bg1a → bg1c → bg1b) — scroll cinematic
 *   Plan 6  : Druids walking (bg2.piv)
 *   Plan 7  : Druids towards Stonehenge (bg1b.piv)
 *   Plans 8-9: Stonehenge from above
 *   Plans 10-12: Plunge, counter-plunge, lightning
 *   Plans 13-15: Knight, hand, entering circle
 *   Plans 16-17: Druid + kneeling knight
 *   Plan 18 : Quest text
 */

#include "moon_intro.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* ------------------------------------------------------------------ */
/* Internal helpers                                                    */
/* ------------------------------------------------------------------ */

/* Show a framebuffer for @frames PAL frames (20 ms each), polling for quit */
static int show_fb(GameCtx *ctx, int frames)
{
    for (int f = 0; f < frames; f++) {
        hal_present(ctx->fb);
        hal_vbl_wait();
        if (hal_poll(&ctx->input))
            return 1; /* quit requested */
        if (ctx->input.escape || ctx->input.space || ctx->input.enter)
            return 1; /* skip */
    }
    return 0;
}

/* Fade in a framebuffer from black over @steps frames */
static int fade_in_fb(GameCtx *ctx, int steps)
{
    for (int s = 1; s <= steps; s++) {
        uint32_t tmp[GAME_W * GAME_H];
        for (int i = 0; i < GAME_W * GAME_H; i++) {
            uint8_t r = (uint8_t)(((ctx->fb[i] >> 16) & 0xFF) * s / steps);
            uint8_t g = (uint8_t)(((ctx->fb[i] >>  8) & 0xFF) * s / steps);
            uint8_t b = (uint8_t)(( ctx->fb[i]        & 0xFF) * s / steps);
            tmp[i] = 0xFF000000u | ((uint32_t)r << 16)
                   | ((uint32_t)g << 8) | b;
        }
        hal_present(tmp);
        hal_vbl_wait();
        if (hal_poll(&ctx->input))
            return 1;
        if (ctx->input.escape || ctx->input.space || ctx->input.enter)
            return 1;
    }
    return 0;
}

/* Fade out to black over @steps frames */
static int fade_out_fb(GameCtx *ctx, int steps)
{
    uint32_t save[GAME_W * GAME_H];
    memcpy(save, ctx->fb, sizeof(save));

    for (int s = steps; s >= 0; s--) {
        for (int i = 0; i < GAME_W * GAME_H; i++) {
            uint8_t r = (uint8_t)(((save[i] >> 16) & 0xFF) * s / steps);
            uint8_t g = (uint8_t)(((save[i] >>  8) & 0xFF) * s / steps);
            uint8_t b = (uint8_t)(( save[i]        & 0xFF) * s / steps);
            ctx->fb[i] = 0xFF000000u | ((uint32_t)r << 16)
                       | ((uint32_t)g << 8) | b;
        }
        hal_present(ctx->fb);
        hal_vbl_wait();
        if (hal_poll(&ctx->input))
            return 1;
        if (ctx->input.escape || ctx->input.space || ctx->input.enter)
            return 1;
    }
    render_clear(ctx->fb, 0xFF000000u);
    hal_present(ctx->fb);
    return 0;
}

/* Load a PIV and render it into ctx->fb, return 0 on success */
static int load_and_show_piv(GameCtx *ctx, const char *name)
{
    MoonPiv *piv = moon_piv_load(name);
    if (!piv) {
        fprintf(stderr, "intro: cannot load '%s'\n", name);
        return -1;
    }
    render_piv_full(piv, ctx->fb);
    moon_piv_free(piv);
    return 0;
}

/* Try to start background music from music.cmp */
static void start_music(GameCtx *ctx)
{
    (void)ctx;
    size_t raw_len = 0;
    uint8_t *raw = moon_file_read("music.cmp", &raw_len);
    if (!raw) {
        /* Try lower-case variant */
        raw = moon_file_read("Music.cmp", &raw_len);
    }
    if (!raw) return;

    /* RNC1 header is 18 bytes: magic(4) + unpacked_len(4) + packed_len(4) + ... */
    if (raw_len < 18) { free(raw); return; }
    uint32_t unpacked_len = ((uint32_t)raw[4] << 24) | ((uint32_t)raw[5] << 16)
                          | ((uint32_t)raw[6] << 8)  |  (uint32_t)raw[7];

    uint8_t *mod_buf = (uint8_t *)malloc(unpacked_len + 4);
    if (!mod_buf) { free(raw); return; }

    int out = moon_rnc1_decompress(raw, raw_len, mod_buf, unpacked_len + 4);
    free(raw);

    if (out > 0) {
        hal_music_play_raw(mod_buf, (size_t)out, 1);
    }
    free(mod_buf);
}

/* ------------------------------------------------------------------ */
/* Credit strings (matching LAB_05B1 / §3.3)                         */
/* ------------------------------------------------------------------ */

static const char *s_credits[][4] = {
    { "created by", "Rob Anderson", NULL, NULL },
    { "Programmed by", "Rob Anderson", "Kevin Hoare", NULL },
    { "Artwork by", "Rob Anderson", "Dennis Turner", NULL },
    { "Music and Sound by", "Richard Joseph", NULL, NULL },
    { "Additional Art by", "Steve Leney", NULL, NULL },
    { "Design by", "Rob Anderson", "Todd Prescott", NULL },
};
#define NUM_CREDITS 6

/* Quest text (LAB_00AA) */
static const char *s_quest_text[] = {
    "The druids sent their",
    "best knights to Stonehenge",
    "so they may be dubbed",
    "into the",
    "Quest for the",
    "MOONSTONE"
};
#define NUM_QUEST_LINES 6

/* ------------------------------------------------------------------ */
/* Introduction plans                                                  */
/* ------------------------------------------------------------------ */

static int plan_logo(GameCtx *ctx)
{
    /* Plan 1: Mindscape logo */
    if (load_and_show_piv(ctx, "mindscape") != 0 &&
        load_and_show_piv(ctx, "mindscape.PIV") != 0 &&
        load_and_show_piv(ctx, "Mindscape.PIV") != 0) {
        /* No logo asset — show text fallback */
        render_clear(ctx->fb, 0xFF000000u);
        render_text_centered(ctx->fb, "MINDSCAPE", GAME_H / 2 - 4, 0xFFFFFFFFu);
    }

    if (fade_in_fb(ctx, 16)) return 1;
    if (show_fb(ctx, 100))   return 1;
    if (fade_out_fb(ctx, 16)) return 1;
    return 0;
}

static int plan_moon_and_credits(GameCtx *ctx)
{
    /* Plan 2: Moon background (bg1c.piv) */
    uint32_t moon_bg[GAME_W * GAME_H];

    if (load_and_show_piv(ctx, "bg1c.PIV") == 0 ||
        load_and_show_piv(ctx, "bg1c.piv") == 0) {
        memcpy(moon_bg, ctx->fb, sizeof(moon_bg));
    } else {
        /* Fallback: dark blue sky */
        render_clear(ctx->fb, 0xFF000820u);
        memcpy(moon_bg, ctx->fb, sizeof(moon_bg));
    }

    if (fade_in_fb(ctx, 16)) return 1;
    if (show_fb(ctx, 60))    return 1;

    /* Plans 3-4: Title over moon */
    memcpy(ctx->fb, moon_bg, sizeof(moon_bg));
    render_text_centered(ctx->fb, "MOONSTONE", GAME_H / 2 - 16, 0xFFFFFF00u);
    render_text_centered(ctx->fb, "A Hard Day's Knight",
                         GAME_H / 2,    0xFFCCCCCCu);
    render_text_centered(ctx->fb, "Mindscape presents",
                         GAME_H / 2 + 14, 0xFF888888u);
    hal_present(ctx->fb);
    if (show_fb(ctx, 120)) return 1;

    /* Credits 0-5 */
    for (int c = 0; c < NUM_CREDITS; c++) {
        memcpy(ctx->fb, moon_bg, sizeof(moon_bg));
        int y = GAME_H / 2 - 20;
        for (int l = 0; l < 4 && s_credits[c][l]; l++) {
            uint32_t col = (l == 0) ? 0xFF888888u : 0xFFFFFFFFu;
            render_text_centered(ctx->fb, s_credits[c][l], y, col);
            y += 12;
        }
        if (fade_in_fb(ctx, 8))  return 1;
        if (show_fb(ctx, 80))    return 1;
        if (fade_out_fb(ctx, 8)) return 1;
    }
    return 0;
}

static int plan_scroll(GameCtx *ctx)
{
    /* Plan 5: Vertical scroll — bg1a → bg1c → bg1b */
    uint32_t buf_a[GAME_W * GAME_H]; /* sky / dusk        */
    uint32_t buf_b[GAME_W * GAME_H]; /* dark forest/moon  */
    uint32_t buf_c[GAME_W * GAME_H]; /* medieval plain    */

    MoonPiv *p;

    p = moon_piv_load("bg1a.PIV");
    if (!p) p = moon_piv_load("bg1a.piv");
    if (p) { render_piv_full(p, buf_a); moon_piv_free(p); }
    else     render_clear(buf_a, 0xFF220011u);

    p = moon_piv_load("bg1c.PIV");
    if (!p) p = moon_piv_load("bg1c.piv");
    if (p) { render_piv_full(p, buf_b); moon_piv_free(p); }
    else     render_clear(buf_b, 0xFF001122u);

    p = moon_piv_load("bg1b.PIV");
    if (!p) p = moon_piv_load("bg1b.piv");
    if (p) { render_piv_full(p, buf_c); moon_piv_free(p); }
    else     render_clear(buf_c, 0xFF112200u);

    /* Start music */
    start_music(ctx);

    /* Scroll from position 0 to 400 with variable speed */
    static const int speeds[] = { 1, 2, 5, 5, 3, 1, 0 };
    int scroll = 0;
    for (int s = 0; speeds[s] != 0; s++) {
        while (scroll < (s + 1) * (400 / 6)) {
            scroll += speeds[s];
            if (scroll > 399) scroll = 399;
            render_scroll_vertical(buf_a, buf_b, buf_c, ctx->fb, scroll);
            hal_present(ctx->fb);
            hal_vbl_wait();
            if (hal_poll(&ctx->input)) return 1;
            if (ctx->input.escape || ctx->input.space) return 1;
        }
    }
    /* Reach end */
    render_scroll_vertical(buf_a, buf_b, buf_c, ctx->fb, 399);
    hal_present(ctx->fb);
    return show_fb(ctx, 30);
}

static int plan_druids_walking(GameCtx *ctx)
{
    /* Plan 6: Druids walking (bg2.piv) */
    if (load_and_show_piv(ctx, "bg2.PIV") != 0 &&
        load_and_show_piv(ctx, "bg2.piv") != 0) {
        render_clear(ctx->fb, 0xFF111111u);
    }
    render_text_centered(ctx->fb, "~ druids walking ~",
                         GAME_H - 20, 0xFF666666u);
    if (fade_in_fb(ctx, 8))  return 1;
    if (show_fb(ctx, 150))   return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_stonehenge_approach(GameCtx *ctx)
{
    /* Plan 7: Druids towards Stonehenge (bg1b.piv) */
    if (load_and_show_piv(ctx, "bg1b.PIV") != 0 &&
        load_and_show_piv(ctx, "bg1b.piv") != 0) {
        render_clear(ctx->fb, 0xFF111118u);
    }
    render_text_centered(ctx->fb, "~ towards Stonehenge ~",
                         GAME_H - 20, 0xFF666666u);
    if (fade_in_fb(ctx, 8))  return 1;
    if (show_fb(ctx, 150))   return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_stonehenge_circle(GameCtx *ctx)
{
    /* Plans 8-9: Stonehenge from above + druids circle */
    if (load_and_show_piv(ctx, "bg4.PIV") != 0 &&
        load_and_show_piv(ctx, "bg4.piv") != 0) {
        render_clear(ctx->fb, 0xFF111111u);
    }
    render_text_centered(ctx->fb, "~ Stonehenge from above ~",
                         GAME_H - 20, 0xFF666666u);
    if (fade_in_fb(ctx, 8))  return 1;
    if (show_fb(ctx, 150))   return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_lightning(GameCtx *ctx)
{
    /* Plans 10-12: Plunge, counter-plunge, lightning */
    if (load_and_show_piv(ctx, "bg2a.PIV") != 0 &&
        load_and_show_piv(ctx, "bg2a.piv") != 0) {
        render_clear(ctx->fb, 0xFF080808u);
    }

    uint32_t base[GAME_W * GAME_H];
    memcpy(base, ctx->fb, sizeof(base));

    if (fade_in_fb(ctx, 8)) return 1;
    if (show_fb(ctx, 60))   return 1;

    /* Lightning flashes (6 flashes matching LAB_0040 / §3.4) */
    for (int flash = 0; flash < 6; flash++) {
        /* White flash */
        render_clear(ctx->fb, 0xFFFFFFFFu);
        hal_present(ctx->fb);
        hal_vbl_wait();
        hal_vbl_wait();
        /* Restore */
        memcpy(ctx->fb, base, sizeof(base));
        hal_present(ctx->fb);
        if (show_fb(ctx, (flash < 3) ? 20 : 8)) return 1;
    }

    if (show_fb(ctx, 50))    return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_ceremony(GameCtx *ctx)
{
    /* Plans 13-15: Knight, hand, entering circle */
    if (load_and_show_piv(ctx, "bg1b.PIV") != 0 &&
        load_and_show_piv(ctx, "bg1b.piv") != 0) {
        render_clear(ctx->fb, 0xFF111118u);
    }
    render_text_centered(ctx->fb, "~ the ceremony ~",
                         GAME_H - 20, 0xFF666666u);
    if (fade_in_fb(ctx, 8))  return 1;
    if (show_fb(ctx, 200))   return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_knighting(GameCtx *ctx)
{
    /* Plans 16-17: Druid + kneeling knight */
    if (load_and_show_piv(ctx, "bg5a.PIV") != 0 &&
        load_and_show_piv(ctx, "bg5a.piv") != 0) {
        render_clear(ctx->fb, 0xFF111118u);
    }
    render_text_centered(ctx->fb, "~ the knighting ~",
                         GAME_H - 20, 0xFF666666u);
    if (fade_in_fb(ctx, 8))  return 1;
    if (show_fb(ctx, 200))   return 1;
    return fade_out_fb(ctx, 8);
}

static int plan_quest_text(GameCtx *ctx)
{
    /* Plan 18: Quest text ("The druids sent their best knights…") */
    render_clear(ctx->fb, 0xFF000000u);
    int y = (GAME_H - (int)NUM_QUEST_LINES * 12) / 2;
    for (int i = 0; i < NUM_QUEST_LINES; i++) {
        uint32_t col = (i == NUM_QUEST_LINES - 1) ? 0xFFFFFF00u : 0xFFCCCCCCu;
        render_text_centered(ctx->fb, s_quest_text[i], y, col);
        y += 12;
    }
    if (fade_in_fb(ctx, 12))  return 1;
    if (show_fb(ctx, 420))    return 1; /* ~8.4 s @ 50 Hz */
    return fade_out_fb(ctx, 12);
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_intro(GameCtx *ctx)
{
    render_clear(ctx->fb, 0xFF000000u);
    hal_present(ctx->fb);

    /* Run each plan; if any returns non-zero the player skipped — jump
     * straight to quest text before going to the menu. */

    if (plan_logo(ctx))               goto quest;
    if (plan_moon_and_credits(ctx))   goto quest;
    if (plan_scroll(ctx))             goto quest;
    if (plan_druids_walking(ctx))     goto quest;
    if (plan_stonehenge_approach(ctx))goto quest;
    if (plan_stonehenge_circle(ctx))  goto quest;
    if (plan_lightning(ctx))          goto quest;
    if (plan_ceremony(ctx))           goto quest;
    if (plan_knighting(ctx))          goto quest;

quest:
    plan_quest_text(ctx);

    hal_music_stop();

    /* Transition to menu */
    ctx->state = STATE_MENU;
}
