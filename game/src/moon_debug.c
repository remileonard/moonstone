/*
 * moon_debug.c — mode debug du combat (voir moon_debug.h).
 *
 * Le relevé est pris par MogCombat.debug, appelé par Combat_Collisions
 * avant que les listes de frames soient vidées : l'image montrée ensuite
 * (écrans échangés juste avant) est celle que ces données décrivent.
 *
 *   cyan    boîte de l'entité (OBJ_BOX_*, union de ses frames)
 *   vert    frame « corps » (liste 44) : zone qui peut être touchée
 *   rouge   frame « frappe » (liste 40) : rectangle de collide.hit et,
 *           en jaune, ses points d'impact (testés contre les pixels opaques
 *           des corps, Col_PixelHit)
 *   magenta point d'impact retenu (OBJ_IMPACT_*) sur l'objet touché
 * Au-dessus de chaque entité : son script (label le plus proche avant
 * ENT_PC) et l'opcode qui sera lu à l'étape suivante.
 */
#include "moon_debug.h"
#include "mog_struct.h"
#include "ix_mog_names.h"
#include "ix_mog_labels.h"

#include <stdio.h>
#include <string.h>

#define W 320
#define H 200
#define VM (m->eng.vm)

#define COL_ENTITY 0xFF00E0E0u
#define COL_BODY   0xFF30E030u
#define COL_STRIKE 0xFFFF3030u
#define COL_POINT  0xFFFFFF00u
#define COL_IMPACT 0xFFFF40FFu
#define COL_TEXT   0xFFFFFFFFu
#define COL_SHADOW 0xFF000000u

typedef struct { int x, y, w, h; } Box;

#define MAX_BOXES  64
#define MAX_POINTS 512

static struct {
    int enabled;
    int seen, pending;              /* relevé déjà pris ; pas encore daté */
    uint32_t last_ms;               /* date du dernier relevé             */
    /* relevé */
    int n_ent;
    struct { Box box; char script[40], op[24]; } ent[IX_ENTITY_COUNT];
    int n_body, n_strike, n_points, n_impacts;
    Box body[MAX_BOXES], strike[MAX_BOXES];
    struct { int x, y; } points[MAX_POINTS], impacts[IX_ENTITY_COUNT];
    int n_fighters;                 /* joueur, puis adversaires           */
    struct { int hp, hp_max, lives, knight; } fighter[1 + IX_ENTITY_COUNT];
    /* images par seconde */
    unsigned frames, shows;         /* depuis fps_ms                      */
    uint32_t fps_ms;
    unsigned fps_frames, fps_shows;
} s_dbg;

/* ------------------------------------------------------------ relevé */

static const char *opcode_name(uint8_t b)
{
    static const char *names[] = {
        "SETDIR", "JUMP", "HOLD", "PHYSICS", "?90", "LOOP", "SKIPIFDEBUG", "?9C",
        "MOVE", "SOUND", "SETFIELD", "SHADOW", "CALL", "IFDEAD", "SPAWN", "KILL",
        "SETBANK", "IFSAMEFACING", "IFFIELDZERO", "IFFIELDNONZERO", "RESET",
    };
    if (b >= 0x80 && b <= 0xD0 && !(b & 3))
        return names[(b - 0x80) >> 2];
    return "?";
}

/* Label le plus proche avant `a` : « nom+décalage » */
static void label_of(uint32_t a, char *out, size_t n)
{
    size_t lo = 0, hi = sizeof ix_mog_labels / sizeof ix_mog_labels[0];
    if (!hi || a < ix_mog_labels[0].addr) {
        snprintf(out, n, "%06X", (unsigned)a);
        return;
    }
    while (hi - lo > 1) {                               /* dernier addr <= a */
        size_t mid = (lo + hi) / 2;
        if (ix_mog_labels[mid].addr <= a)
            lo = mid;
        else
            hi = mid;
    }
    const char *name = ix_mog_labels[lo].name;
    if (!strncmp(name, "x_", 2))                        /* script nommé */
        name += 2;
    uint32_t off = a - ix_mog_labels[lo].addr;
    if (off)
        snprintf(out, n, "%s+%u", name, (unsigned)off);
    else
        snprintf(out, n, "%s", name);
}

/* Rectangle et points d'impact de la frame frappe (cel, frame) en (sx, sy),
 * comme Col_PixelHit (mog_col.c) */
static void strike_data(MogCombat *m, uint32_t cel, uint16_t frame, int sx, int sy)
{
    uint32_t fe = cel + 10 + (uint32_t)frame * 10;
    int mirror = (ix_rb(VM, fe + CELF_FLAGS) & 1) ? 0 : (int16_t)ix_rw(VM, fe + CELF_W);
    uint32_t a2 = MOG_t_HitDataByCel;
    for (int guard = 0; ix_rl(VM, a2) != cel; a2 += 8)
        if (++guard > 4096 || !ix_rl(VM, a2))
            return;
    uint32_t a1 = ix_rl(VM, a2 + 4);
    for (uint16_t k = 0; k < frame; k++) {
        uint16_t n = ix_rb(VM, a1++);
        if (n)
            a1 += 2u * n + 3u;
    }
    unsigned n = ix_rb(VM, a1);
    if (!n)
        return;
    int w = ix_rb(VM, a1 + 2), h = ix_rb(VM, a1 + 3);
    if (s_dbg.n_strike < MAX_BOXES)
        s_dbg.strike[s_dbg.n_strike++] = (Box){ mirror ? sx + mirror - w : sx, sy, w, h };
    a1 += 4;
    for (unsigned p = 0; p < n && s_dbg.n_points < MAX_POINTS; p++, a1 += 2) {
        int x = ix_rb(VM, a1), y = ix_rb(VM, a1 + 1);
        if (mirror)
            x = mirror - x;
        s_dbg.points[s_dbg.n_points].x = sx + x;
        s_dbg.points[s_dbg.n_points++].y = sy + y;
    }
}

/* Combattant : chevalier (t_KnightObjects, avec ses vies) ou créature */
static void fighter(MogCombat *m, uint32_t obj)
{
    if (s_dbg.n_fighters >= 1 + IX_ENTITY_COUNT)
        return;
    int i = s_dbg.n_fighters++;
    s_dbg.fighter[i].hp = (int16_t)ix_rw(VM, obj + OBJ_HP);
    s_dbg.fighter[i].hp_max = (int16_t)ix_rw(VM, obj + OBJ_HP_MAX);
    s_dbg.fighter[i].lives = (int8_t)ix_rb(VM, obj + OBJ_LIVES);
    s_dbg.fighter[i].knight = obj >= MOG_t_KnightObjects &&
                              obj < MOG_t_KnightObjects + 4 * IX_OBJECT_SIZE;
}

static void snapshot(MogCombat *m)
{
    s_dbg.frames++;
    s_dbg.pending = 1;                                  /* daté par moon_debug_draw */
    s_dbg.n_ent = s_dbg.n_body = s_dbg.n_strike = s_dbg.n_points = s_dbg.n_impacts = 0;
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (!ix_rb(VM, en + ENT_ACTIVE))
            continue;
        uint32_t obj = ix_rl(VM, en + ENT_OBJ);
        uint32_t pc = ix_rl(VM, en + ENT_PC);
        if (obj) {
            int x0 = (int16_t)ix_rw(VM, obj + OBJ_BOX_X0), x1 = (int16_t)ix_rw(VM, obj + OBJ_BOX_X1);
            int y0 = (int16_t)ix_rw(VM, obj + OBJ_BOX_Y0), y1 = (int16_t)ix_rw(VM, obj + OBJ_BOX_Y1);
            if (ix_rl(VM, obj + OBJ_HIT_BY) && s_dbg.n_impacts < IX_ENTITY_COUNT) {
                s_dbg.impacts[s_dbg.n_impacts].x = (int16_t)ix_rw(VM, obj + OBJ_IMPACT_X);
                s_dbg.impacts[s_dbg.n_impacts++].y = (int16_t)ix_rw(VM, obj + OBJ_IMPACT_Y);
            }
            if (x1 <= x0 || y1 <= y0)                   /* rien dessiné */
                continue;
            s_dbg.ent[s_dbg.n_ent].box = (Box){ x0, y0, x1 - x0, y1 - y0 };
        } else {
            s_dbg.ent[s_dbg.n_ent].box = (Box){
                (int16_t)ix_rw(VM, en + ENT_DRAW_X), (int16_t)ix_rw(VM, en + ENT_DRAW_Y),
                (int16_t)ix_rw(VM, en + ENT_W), (int16_t)ix_rw(VM, en + ENT_H) };
        }
        char *sc = s_dbg.ent[s_dbg.n_ent].script, *op = s_dbg.ent[s_dbg.n_ent].op;
        if (!pc) {
            snprintf(sc, sizeof s_dbg.ent[0].script, "-");
            op[0] = 0;
        } else {
            label_of(pc, sc, sizeof s_dbg.ent[0].script);
            uint8_t b = ix_rb(VM, pc);
            if (b == 0xFF)
                snprintf(op, sizeof s_dbg.ent[0].op, "FF END");
            else if (b == 0xFE || b == 0xFD)
                snprintf(op, sizeof s_dbg.ent[0].op, "%02X %s", b, b == 0xFE ? "LOOP" : "RESUME");
            else if (b & 0x80)
                snprintf(op, sizeof s_dbg.ent[0].op, "%02X %s", b, opcode_name(b));
            else
                snprintf(op, sizeof s_dbg.ent[0].op, "%02X CEL %u", b, ix_rb(VM, pc + 1));
        }
        s_dbg.n_ent++;

        for (uint32_t a = ix_rl(VM, en + ENT_LIST_BODY); a && ix_rl(VM, a + DL_CEL) &&
             s_dbg.n_body < MAX_BOXES; a += DL_SIZE) {
            uint32_t cel = ix_rl(VM, a + DL_CEL);
            uint32_t fe = cel + 10 + (uint32_t)ix_rw(VM, a + DL_FRAME) * 10;
            s_dbg.body[s_dbg.n_body++] = (Box){ (int16_t)ix_rw(VM, a + DL_X),
                (int16_t)ix_rw(VM, a + DL_Y), ix_rw(VM, fe + CELF_W), ix_rw(VM, fe + CELF_H) };
        }
        for (uint32_t a = ix_rl(VM, en + ENT_LIST_STRIKE); a && ix_rl(VM, a + DL_CEL); a += DL_SIZE)
            strike_data(m, ix_rl(VM, a + DL_CEL), ix_rw(VM, a + DL_FRAME),
                        (int16_t)ix_rw(VM, a + DL_X), (int16_t)ix_rw(VM, a + DL_Y));
    }
    /* le joueur, l'adversaire (duel) ou les créatures qui ont des PV */
    uint32_t player = ix_rl(VM, MOG_v_Combatants + CMB_PLAYER);
    uint32_t opp = ix_rl(VM, MOG_v_Combatants + CMB_OPPONENT);
    s_dbg.n_fighters = 0;
    if (player)
        fighter(m, player);
    if (opp)
        fighter(m, opp);
    else
        for (int i = 0; i < IX_ENTITY_COUNT; i++) {
            uint32_t en = MOG_t_Entities + (uint32_t)i * IX_ENTITY_SIZE;
            uint32_t obj = ix_rl(VM, en + ENT_OBJ);
            if (!ix_rb(VM, en + ENT_ACTIVE) || !obj || obj == player ||
                (int16_t)ix_rw(VM, obj + OBJ_HP_MAX) <= 0)
                continue;
            int dup = 0;
            for (int k = 0; k < i; k++)
                dup |= ix_rb(VM, MOG_t_Entities + (uint32_t)k * IX_ENTITY_SIZE + ENT_ACTIVE) &&
                       ix_rl(VM, MOG_t_Entities + (uint32_t)k * IX_ENTITY_SIZE + ENT_OBJ) == obj;
            if (!dup)
                fighter(m, obj);
        }
}

void moon_debug_attach(MogCombat *m)
{
    m->debug = snapshot;
}

void moon_debug_toggle(void)
{
    s_dbg.enabled = !s_dbg.enabled;
}

int moon_debug_enabled(void)
{
    return s_dbg.enabled;
}

int moon_debug_in_combat(uint32_t now_ms)
{
    return s_dbg.pending || (s_dbg.seen && now_ms - s_dbg.last_ms < 200);
}

/* ------------------------------------------------------------ dessin */

/* Police 3 × 5 (majuscules, chiffres, quelques signes) */
static const char *glyph(int c)
{
    static const char *g[128] = {
        ['0'] = "111101101101111", ['1'] = "010110010010111", ['2'] = "111001111100111",
        ['3'] = "111001111001111", ['4'] = "101101111001001", ['5'] = "111100111001111",
        ['6'] = "111100111101111", ['7'] = "111001001010010", ['8'] = "111101111101111",
        ['9'] = "111101111001111",
        ['A'] = "010101111101101", ['B'] = "110101110101110", ['C'] = "011100100100011",
        ['D'] = "110101101101110", ['E'] = "111100110100111", ['F'] = "111100110100100",
        ['G'] = "011100101101011", ['H'] = "101101111101101", ['I'] = "111010010010111",
        ['J'] = "001001001101010", ['K'] = "101101110101101", ['L'] = "100100100100111",
        ['M'] = "101111111101101", ['N'] = "110101101101101", ['O'] = "010101101101010",
        ['P'] = "110101110100100", ['Q'] = "010101101110011", ['R'] = "110101110101101",
        ['S'] = "011100010001110", ['T'] = "111010010010010", ['U'] = "101101101101111",
        ['V'] = "101101101101010", ['W'] = "101101111111101", ['X'] = "101101010101101",
        ['Y'] = "101101010010010", ['Z'] = "111001010100111",
        [':'] = "000010000010000", ['/'] = "001001010100100", ['+'] = "000010111010000",
        ['-'] = "000000111000000", ['.'] = "000000000000010", ['_'] = "000000000000111",
        ['('] = "001010010010001", [')'] = "100010010010100", ['='] = "000111000111000",
        ['?'] = "111001010000010",
    };
    if (c >= 'a' && c <= 'z')
        c -= 32;
    return c > 0 && c < 128 ? g[c] : NULL;
}

static void pixel(uint32_t *fb, int x, int y, uint32_t c)
{
    if (x >= 0 && x < W && y >= 0 && y < H)
        fb[y * W + x] = c;
}

static void text_at(uint32_t *fb, int x, int y, const char *t, uint32_t c)
{
    for (; *t; t++, x += 4) {
        const char *gl = glyph((unsigned char)*t);
        if (!gl)
            continue;
        for (int r = 0; r < 5; r++)
            for (int k = 0; k < 3; k++)
                if (gl[r * 3 + k] == '1')
                    pixel(fb, x + k, y + r, c);
    }
}

/* Texte avec une ombre noire autour, pour rester lisible sur le décor */
static void text(uint32_t *fb, int x, int y, const char *t, uint32_t c)
{
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++)
            if (dx || dy)
                text_at(fb, x + dx, y + dy, t, COL_SHADOW);
    text_at(fb, x, y, t, c);
}

static void rect(uint32_t *fb, Box b, uint32_t c)
{
    if (b.w <= 0 || b.h <= 0)
        return;
    for (int x = b.x; x < b.x + b.w; x++) {
        pixel(fb, x, b.y, c);
        pixel(fb, x, b.y + b.h - 1, c);
    }
    for (int y = b.y; y < b.y + b.h; y++) {
        pixel(fb, b.x, y, c);
        pixel(fb, b.x + b.w - 1, y, c);
    }
}

static void cross(uint32_t *fb, int x, int y, int r, uint32_t c)
{
    for (int k = -r; k <= r; k++) {
        pixel(fb, x + k, y, c);
        pixel(fb, x, y + k, c);
    }
}

static int clampi(int v, int lo, int hi)
{
    return v < lo ? lo : v > hi ? hi : v;
}

void moon_debug_draw(uint32_t *fb, uint32_t now_ms)
{
    if (s_dbg.pending) {                                /* date du relevé */
        s_dbg.pending = 0;
        s_dbg.seen = 1;
        s_dbg.last_ms = now_ms;
    }
    s_dbg.shows++;
    if (now_ms - s_dbg.fps_ms >= 1000) {
        uint32_t dt = now_ms - s_dbg.fps_ms;
        s_dbg.fps_frames = s_dbg.frames * 1000u / dt;
        s_dbg.fps_shows = s_dbg.shows * 1000u / dt;
        s_dbg.frames = s_dbg.shows = 0;
        s_dbg.fps_ms = now_ms;
    }
    if (!fb || !s_dbg.enabled || !moon_debug_in_combat(now_ms))
        return;

    for (int i = 0; i < s_dbg.n_body; i++)
        rect(fb, s_dbg.body[i], COL_BODY);
    for (int i = 0; i < s_dbg.n_strike; i++)
        rect(fb, s_dbg.strike[i], COL_STRIKE);
    for (int i = 0; i < s_dbg.n_points; i++)
        pixel(fb, s_dbg.points[i].x, s_dbg.points[i].y, COL_POINT);
    for (int i = 0; i < s_dbg.n_ent; i++)
        rect(fb, s_dbg.ent[i].box, COL_ENTITY);
    for (int i = 0; i < s_dbg.n_impacts; i++)
        cross(fb, s_dbg.impacts[i].x, s_dbg.impacts[i].y, 3, COL_IMPACT);

    char t[64];
    snprintf(t, sizeof t, "FPS %u  IMG/S %u", s_dbg.fps_shows, s_dbg.fps_frames);
    text(fb, 2, 2, t, COL_TEXT);
    Box placed[1 + IX_ENTITY_COUNT];                    /* zones de texte prises */
    placed[0] = (Box){ 0, 0, 4 * (int)strlen(t) + 4, 9 + 7 * s_dbg.n_fighters };
    for (int i = 0; i < s_dbg.n_fighters; i++) {
        int n = snprintf(t, sizeof t, "%s PV %d/%d", i ? "ENNEMI" : "JOUEUR",
                         s_dbg.fighter[i].hp, s_dbg.fighter[i].hp_max);
        if (s_dbg.fighter[i].knight && n > 0 && n < (int)sizeof t)
            snprintf(t + n, sizeof t - (size_t)n, " VIES %d", s_dbg.fighter[i].lives);
        text(fb, 2, 9 + 7 * i, t, i ? COL_STRIKE : COL_BODY);
        if (4 * (int)strlen(t) + 4 > placed[0].w)
            placed[0].w = 4 * (int)strlen(t) + 4;
    }
    for (int i = 0; i < s_dbg.n_ent; i++) {             /* coin haut gauche */
        Box b = s_dbg.ent[i].box;
        int l1 = (int)strlen(s_dbg.ent[i].script), l2 = (int)strlen(s_dbg.ent[i].op);
        int len = l1 > l2 ? l1 : l2;
        Box lb = { clampi(b.x, 1, W - 4 * len - 1), 0, 4 * len, 12 };
        /* au-dessus de la boîte, plus haut si la place est prise, sinon
         * dedans en descendant */
        for (int k = 0; k < 16; k++) {
            lb.y = clampi(k < 4 ? b.y - 13 * (k + 1) : b.y + 1 + 13 * (k - 4), 1, H - 13);
            int free = 1;
            for (int j = 0; j <= i && free; j++)
                free = lb.x >= placed[j].x + placed[j].w || placed[j].x >= lb.x + lb.w ||
                       lb.y >= placed[j].y + placed[j].h || placed[j].y >= lb.y + lb.h;
            if (free)
                break;
        }
        placed[i + 1] = lb;
        text(fb, lb.x, lb.y, s_dbg.ent[i].script, COL_ENTITY);
        text(fb, lb.x, lb.y + 6, s_dbg.ent[i].op, COL_TEXT);
    }
}
