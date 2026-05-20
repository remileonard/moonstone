/*
 * moon_overworld.c — Overworld map mode for Moonstone
 *
 * Implements the overworld as described in DOC_MODE_OVERWORLD.md.
 *
 * The map is displayed using dw1.PIV (320×200 background).
 * Nine static nodes are positioned at fixed pixel coordinates.
 * Knights move freely using the joystick; when a knight reaches a
 * node (within a proximity threshold), the appropriate state is
 * triggered.
 *
 * All node coordinates taken from DOC_MODE_OVERWORLD.md §1.6.
 *
 * Sprite assets (DOC_MODE_OVERWORLD.md §1.2):
 *   ov1.cel  — 4 frames — overworld node icons
 *   li1.cel  — 30 frames — location/place icons
 *   dg1.cel  — 55 frames — dragon flying on map
 *   ha1.cel  — 22 frames — hawk / map decoration
 *   co1.cel  — 25 frames — complementary icons
 *   da1.cel  — 52 frames — damage/animated decoration
 *   kn1..4.ob — knight sprites per faction (kn1.ob is a stub)
 */

#include "moon_overworld.h"
#include "moon_game.h"
#include "moon_hal.h"
#include "moon_render.h"
#include "moon_assets.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* ------------------------------------------------------------------ */
/* Node table (LAB_069F)                                               */
/* ------------------------------------------------------------------ */

typedef struct {
    int      type;
    int      x;
    int      y;
    const char *name;
} MapNode;

static const MapNode s_nodes[] = {
    /* type   x    y   name                     */
    { 0x15,  18,  11, "Village of Richard"      },
    { 0x16, 286,  11, "Village of Godber"       },
    { 0x17,   0, 187, "Village of Jeffrey"      },
    { 0x18, 303, 192, "Village of Edward"       },
    { 0x19,  82,  28, "Highwood"                },
    { 0x1a, 277, 143, "Waterdeep"               },
    { 0x1b,  88, 155, "Stonehenge"              },
    { 0x1c, 152,  97, "Valley of the Gods"      },
    { 0x1e, 217,  11, "Math the Wizard"         },
};
#define NUM_NODES ((int)(sizeof(s_nodes)/sizeof(s_nodes[0])))

#define NODE_PROXIMITY  16   /* pixel radius to trigger node */

/* ------------------------------------------------------------------ */
/* Knight sprite colours (small dot to represent each knight)         */
/* ------------------------------------------------------------------ */

static const uint32_t s_knight_dot_colors[MAX_PLAYERS] = {
    0xFF4444FFu,
    0xFFFF4444u,
    0xFF44FF44u,
    0xFFFFFF44u,
};

/* Starting positions (near their respective villages) */
static const int s_start_x[MAX_PLAYERS] = { 18, 286,   0, 303 };
static const int s_start_y[MAX_PLAYERS] = { 11,  11, 187, 192 };

/* ------------------------------------------------------------------ */
/* CEL / OB sprite assets                                              */
/* ------------------------------------------------------------------ */

/* Map of filenames tried for each knight faction (kn1.ob is a stub) */
static const char *s_knight_ob_names[MAX_PLAYERS] = {
    "kn1.ob", "kn2.ob", "kn3.ob", "kn4.ob"
};

/* Loaded CEL/OB pointers — NULL if file unavailable */
static MoonCel *s_ov_cel   = NULL; /* ov1.cel  — node icons (4 frames)  */
static MoonCel *s_li_cel   = NULL; /* li1.cel  — location icons         */
static MoonCel *s_dg_cel   = NULL; /* dg1.cel  — dragon (55 frames)     */
static MoonCel *s_ha_cel   = NULL; /* ha1.cel  — hawk / decoration      */
static MoonCel *s_kn_ob[MAX_PLAYERS]; /* kn1..4.ob — per-knight sprites  */

/* PIV palette shared by all sprites (converted to ARGB8888) */
static uint32_t s_ov_palette[MAX_PALETTE];

/* Animation tick counters */
static int s_dg_frame  = 0;   /* current dragon animation frame */
static int s_dg_tick   = 0;   /* ticks since last dragon frame  */
#define DG_ANIM_SPEED  4      /* advance dragon frame every N ticks */

static int s_kn_frame  = 0;   /* overworld knight walk frame    */
static int s_kn_tick   = 0;
#define KN_ANIM_SPEED  6

/* ------------------------------------------------------------------ */
/* Rendering                                                           */
/* ------------------------------------------------------------------ */

static uint32_t s_map_bg[GAME_W * GAME_H];
static int      s_map_loaded = 0;

static void load_map_background(void)
{
    if (s_map_loaded) return;

    /* ---- Background PIV ---- */
    MoonPiv *piv = moon_piv_load("dw1.PIV");
    if (!piv) piv = moon_piv_load("dw1.piv");
    if (piv) {
        render_piv_full(piv, s_map_bg);
        /* Extract palette for sprite rendering */
        int pal_size = 1 << piv->planes;
        if (pal_size > MAX_PALETTE) pal_size = MAX_PALETTE;
        render_build_palette(piv->palette, pal_size, s_ov_palette);
        moon_piv_free(piv);
    } else {
        /* Fallback: dark green background */
        for (int i = 0; i < GAME_W * GAME_H; i++)
            s_map_bg[i] = 0xFF082808u;
        /* Neutral grey placeholder palette */
        for (int i = 0; i < MAX_PALETTE; i++)
            s_ov_palette[i] = 0xFF808080u | (0xFF000000u);
        /* Draw placeholder node markers */
        for (int n = 0; n < NUM_NODES; n++) {
            int x = s_nodes[n].x;
            int y = s_nodes[n].y;
            render_fill_rect(s_map_bg, x - 3, y - 3, 7, 7, 0xFF888888u);
        }
    }

    /* ---- CEL sprite assets ---- */

    /* ov1.cel — 4 node icons */
    s_ov_cel = moon_cel_load("ov1.cel");
    if (!s_ov_cel) s_ov_cel = moon_cel_load("ov1.CEL");

    /* li1.cel — location icons (30 frames) */
    s_li_cel = moon_cel_load("li1.cel");
    if (!s_li_cel) s_li_cel = moon_cel_load("li1.CEL");

    /* dg1.cel — dragon flying on map (55 frames) */
    s_dg_cel = moon_cel_load("dg1.cel");
    if (!s_dg_cel) s_dg_cel = moon_cel_load("dg1.CEL");

    /* ha1.cel — hawk / map decoration (22 frames) */
    s_ha_cel = moon_cel_load("ha1.cel");
    if (!s_ha_cel) s_ha_cel = moon_cel_load("ha1.CEL");

    /* kn*.ob — per-faction knight sprites */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        s_kn_ob[i] = NULL;
        /* moon_ob_load uses the same decoder as moon_cel_load */
        MoonOb *ob = moon_ob_load(s_knight_ob_names[i]);
        if (ob && ob->frame_count > 0) {
            /* Cast MoonOb* to MoonCel* — they share the same layout */
            s_kn_ob[i] = (MoonCel *)ob;
        } else {
            moon_ob_free(ob);
        }
    }

    s_map_loaded = 1;
}

/*
 * Node type → ov1.cel frame mapping.
 *
 * ov1.cel has 4 frames. Based on DOC_MODE_OVERWORLD.md §1.2 and §1.3:
 *   frame 0: generic / default node icon
 *   frame 1: selected / active node icon
 *   frame 2: town / city icon
 *   frame 3: special site icon (Stonehenge, Valley of Gods, Wizard)
 *
 * li1.cel (30 frames) is used for village/location icons when available.
 */
static int node_icon_frame(int node_type)
{
    switch (node_type) {
    case 0x15: case 0x16: case 0x17: case 0x18: return 0; /* village */
    case 0x19: case 0x1a:                        return 2; /* city    */
    case 0x1b: case 0x1c:                        return 3; /* special */
    case 0x1e:                                   return 3; /* wizard  */
    default:                                     return 0;
    }
}

/* li1.cel frame for a given node type (30 frames available) */
static int node_li_frame(int node_type)
{
    switch (node_type) {
    case 0x15: return  0; /* village Richard */
    case 0x16: return  4; /* village Godber  */
    case 0x17: return  8; /* village Jeffrey */
    case 0x18: return 12; /* village Edward  */
    case 0x19: return 16; /* Highwood         */
    case 0x1a: return 20; /* Waterdeep        */
    case 0x1b: return 24; /* Stonehenge       */
    case 0x1c: return 28; /* Valley of Gods   */
    case 0x1e: return  2; /* Wizard           */
    default:   return  0;
    }
}

static void draw_overworld(GameCtx *ctx)
{
    /* Copy background */
    memcpy(ctx->fb, s_map_bg, sizeof(s_map_bg));

    /* ---- Advance dragon animation ---- */
    s_dg_tick++;
    if (s_dg_tick >= DG_ANIM_SPEED) {
        s_dg_tick = 0;
        s_dg_frame++;
        if (s_dg_cel && s_dg_frame >= s_dg_cel->frame_count)
            s_dg_frame = 0;
    }

    /* ---- Advance knight walk animation ---- */
    s_kn_tick++;
    if (s_kn_tick >= KN_ANIM_SPEED) {
        s_kn_tick  = 0;
        s_kn_frame = (s_kn_frame + 1) & 7; /* 8-frame walk cycle */
    }

    /* ---- Draw dragon on the map ---- */
    if (s_dg_cel && s_dg_cel->frame_count > 0) {
        /* Dragon roams near the centre of the map */
        int dg_x = GAME_W / 2 - (int)s_dg_cel->frames[s_dg_frame].width  / 2;
        int dg_y = GAME_H / 3 - (int)s_dg_cel->frames[s_dg_frame].height / 2;
        render_cel(s_dg_cel, s_dg_frame, s_ov_palette, ctx->fb,
                   dg_x, dg_y, BLIT_MASK);
    }

    /* ---- Draw node icons ---- */
    for (int n = 0; n < NUM_NODES; n++) {
        int nx = s_nodes[n].x;
        int ny = s_nodes[n].y;
        int type = s_nodes[n].type;

        /* Prefer li1.cel for location flavour; fall back to ov1.cel */
        if (s_li_cel && s_li_cel->frame_count > 0) {
            int fr = node_li_frame(type);
            if (fr >= s_li_cel->frame_count) fr = 0;
            int icon_w = (int)s_li_cel->frames[fr].width;
            int icon_h = (int)s_li_cel->frames[fr].height;
            render_cel(s_li_cel, fr, s_ov_palette, ctx->fb,
                       nx - icon_w / 2, ny - icon_h / 2, BLIT_MASK);
        } else if (s_ov_cel && s_ov_cel->frame_count > 0) {
            int fr = node_icon_frame(type);
            if (fr >= s_ov_cel->frame_count) fr = 0;
            int icon_w = (int)s_ov_cel->frames[fr].width;
            int icon_h = (int)s_ov_cel->frames[fr].height;
            render_cel(s_ov_cel, fr, s_ov_palette, ctx->fb,
                       nx - icon_w / 2, ny - icon_h / 2, BLIT_MASK);
        } else {
            /* Fallback placeholder */
            render_fill_rect(ctx->fb, nx - 2, ny - 2, 5, 5, 0xFF888844u);
        }
    }

    /* ---- Draw active knights ---- */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!ctx->knights[i].active || ctx->knights[i].dead) continue;
        int kx = ctx->knights[i].map_x;
        int ky = ctx->knights[i].map_y;

        if (s_kn_ob[i] && s_kn_ob[i]->frame_count > 0) {
            /* Use walk animation frames (8-frame cycle) */
            int fr = s_kn_frame % s_kn_ob[i]->frame_count;
            int flip = (ctx->knights[i].map_x < GAME_W / 2) ? 0 : BLIT_FLIP_X;
            int icon_w = (int)s_kn_ob[i]->frames[fr].width;
            int icon_h = (int)s_kn_ob[i]->frames[fr].height;
            render_cel(s_kn_ob[i], fr, s_ov_palette, ctx->fb,
                       kx - icon_w / 2, ky - icon_h, flip | BLIT_MASK);
        } else {
            /* Fallback: 5×5 coloured dot */
            uint32_t col = s_knight_dot_colors[i];
            render_fill_rect(ctx->fb, kx - 2, ky - 2, 5, 5, col);
        }
    }

    /* HUD: show current knight info */
    Knight *k = &ctx->knights[ctx->current_knight];
    char hud[64];
    snprintf(hud, sizeof(hud), "%s  HP:%d  GOLD:%d  REL:%d",
             (const char *[]){ "RICHARD","GODBER","JEFFREY","EDWARD" }[k->id],
             k->hp, k->gold, k->relics);
    render_fill_rect(ctx->fb, 0, 0, GAME_W, 9, 0xAA000000u);
    render_text(ctx->fb, hud, 2, 1, s_knight_dot_colors[ctx->current_knight]);

    /* If hovering over a node, show its name */
    for (int n = 0; n < NUM_NODES; n++) {
        int dx = k->map_x - s_nodes[n].x;
        int dy = k->map_y - s_nodes[n].y;
        if (dx * dx + dy * dy <= NODE_PROXIMITY * NODE_PROXIMITY) {
            render_fill_rect(ctx->fb, 0, GAME_H - 10, GAME_W, 10, 0xAA000000u);
            render_text_centered(ctx->fb, s_nodes[n].name, GAME_H - 9, 0xFFFFFF88u);
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/* AI movement — CPU knights wander the map                           */
/* ------------------------------------------------------------------ */

static void ai_move_knight(Knight *k)
{
    /* Simple AI: move randomly, with bias towards centre */
    int dx = 0, dy = 0;
    int cx = GAME_W / 2, cy = GAME_H / 2;
    /* Bias towards random node */
    int target_node = k->node_idx % NUM_NODES;
    int tx = s_nodes[target_node].x;
    int ty = s_nodes[target_node].y;

    if (k->map_x < tx) dx = 1;
    else if (k->map_x > tx) dx = -1;
    if (k->map_y < ty) dy = 1;
    else if (k->map_y > ty) dy = -1;

    (void)cx; (void)cy;

    k->map_x += dx;
    k->map_y += dy;

    /* Clamp to map */
    if (k->map_x < 0)        k->map_x = 0;
    if (k->map_x >= GAME_W)  k->map_x = GAME_W - 1;
    if (k->map_y < 0)        k->map_y = 0;
    if (k->map_y >= GAME_H)  k->map_y = GAME_H - 1;

    /* Check proximity to nodes */
    for (int n = 0; n < NUM_NODES; n++) {
        int px = k->map_x - s_nodes[n].x;
        int py = k->map_y - s_nodes[n].y;
        if (px * px + py * py <= NODE_PROXIMITY * NODE_PROXIMITY) {
            k->node_idx = (k->node_idx + 1) % NUM_NODES;
            break;
        }
    }
}

/* ------------------------------------------------------------------ */
/* Node interaction                                                    */
/* ------------------------------------------------------------------ */

/*
 * check_node_interaction — check if the current knight is on a node.
 * Returns the node index if triggered, or -1 if none.
 */
static int check_node_interaction(GameCtx *ctx)
{
    Knight *k = &ctx->knights[ctx->current_knight];
    for (int n = 0; n < NUM_NODES; n++) {
        int dx = k->map_x - s_nodes[n].x;
        int dy = k->map_y - s_nodes[n].y;
        if (dx * dx + dy * dy <= (NODE_PROXIMITY / 2) * (NODE_PROXIMITY / 2)) {
            return n;
        }
    }
    return -1;
}

static void handle_node(GameCtx *ctx, int node_idx)
{
    int type = s_nodes[node_idx].type;
    ctx->node_type = type;

    switch (type) {
    /* Villages — own faction: heal + skill up; other: nothing */
    case 0x15: case 0x16: case 0x17: case 0x18:
        ctx->state = STATE_VILLAGE;
        break;

    /* Cities */
    case 0x19: case 0x1a:
        ctx->state = STATE_TOWN;
        break;

    /* Stonehenge / Valley of Gods */
    case 0x1b:
        ctx->state = STATE_STONEHENGE;
        break;
    case 0x1c:
        ctx->state = STATE_STONEHENGE; /* reuse stonehenge state */
        break;

    /* Math the Wizard */
    case 0x1e:
        ctx->state = STATE_WIZARD;
        break;

    /* PvP / PvE */
    case 0x01: case 0x21: case 0x02:
        ctx->state = STATE_COMBAT;
        break;

    default:
        break;
    }
}

/* ------------------------------------------------------------------ */
/* Public entry point                                                  */
/* ------------------------------------------------------------------ */

void game_run_overworld(GameCtx *ctx)
{
    /* Initialise knight positions to starting nodes */
    for (int i = 0; i < MAX_PLAYERS; i++) {
        if (!ctx->knights[i].active) continue;
        if (ctx->knights[i].map_x == 0 && ctx->knights[i].map_y == 0) {
            ctx->knights[i].map_x = s_start_x[i];
            ctx->knights[i].map_y = s_start_y[i];
        }
    }

    load_map_background();
    /* s_map_loaded is set inside load_map_background() */

    /* Start overworld music (vmusic.cmp or music.cmp) */
    {
        size_t raw_len = 0;
        uint8_t *raw = moon_file_read("vmusic.cmp", &raw_len);
        if (!raw) raw = moon_file_read("music.cmp", &raw_len);
        if (raw && raw_len > 18) {
            uint32_t ulen = ((uint32_t)raw[4] << 24) | ((uint32_t)raw[5] << 16)
                          | ((uint32_t)raw[6] << 8)  |  (uint32_t)raw[7];
            uint8_t *mod = (uint8_t *)malloc(ulen + 4);
            if (mod) {
                int n = moon_rnc1_decompress(raw, raw_len, mod, ulen + 4);
                if (n > 0) hal_music_play_raw(mod, (size_t)n, 1);
                free(mod);
            }
        }
        free(raw);
    }

    int ai_tick = 0;

    while (ctx->state == STATE_OVERWORLD) {
        /* Poll input */
        if (hal_poll(&ctx->input)) {
            if (ctx->input.quit || ctx->input.escape) {
                ctx->state = STATE_QUIT;
                return;
            }
        }

        Knight *k = &ctx->knights[ctx->current_knight];

        /* Move current (human-controlled) knight */
        if (k->human) {
            int spd = 2;
            if (ctx->input.joy[0].left  || (ctx->input.keys &&
                    ctx->input.keys[80])) k->map_x -= spd;
            if (ctx->input.joy[0].right || (ctx->input.keys &&
                    ctx->input.keys[79])) k->map_x += spd;
            if (ctx->input.joy[0].up    || (ctx->input.keys &&
                    ctx->input.keys[82])) k->map_y -= spd;
            if (ctx->input.joy[0].down  || (ctx->input.keys &&
                    ctx->input.keys[81])) k->map_y += spd;

            /* Clamp */
            if (k->map_x < 0)        k->map_x = 0;
            if (k->map_x >= GAME_W)  k->map_x = GAME_W - 1;
            if (k->map_y < 0)        k->map_y = 0;
            if (k->map_y >= GAME_H)  k->map_y = GAME_H - 1;

            /* Fire: interact with current node */
            if (ctx->input.joy[0].fire || ctx->input.enter || ctx->input.space) {
                int node = check_node_interaction(ctx);
                if (node >= 0) {
                    handle_node(ctx, node);
                    if (ctx->state != STATE_OVERWORLD) {
                        hal_music_stop();
                        return;
                    }
                }
            }
        }

        /* AI movement every 4 frames */
        ai_tick++;
        if (ai_tick >= 4) {
            ai_tick = 0;
            for (int i = 0; i < MAX_PLAYERS; i++) {
                if (!ctx->knights[i].active || ctx->knights[i].dead) continue;
                if (!ctx->knights[i].human)
                    ai_move_knight(&ctx->knights[i]);
            }
        }

        /* Check automatic node interaction for human (proximity trigger) */
        {
            int node = check_node_interaction(ctx);
            if (node >= 0 && k->human) {
                /* Show node prompt — enter on fire */
                (void)node; /* interaction via explicit fire button above */
            }
        }

        draw_overworld(ctx);
        hal_present(ctx->fb);
        hal_vbl_wait();
    }

    hal_music_stop();
}
