/*
 * ix_engine.c — moteur de scripts IMAGEXCEL du combat (mog).
 *
 * Traduction des routines de amiga_asm/mog.asm ; chaque fonction indique la
 * routine d'origine. L'ordre des opérations, la taille des accès et le
 * signe des comparaisons suivent le code 68000 : tools/ix_difftest.py
 * compare ce moteur au code d'origine exécuté par un émulateur 68000.
 */
#include "ix_engine.h"
#include "mog_struct.h"
#include "ix_mog_names.h"

#include <string.h>

/* Entités (ENT_*) et contextes (CTX_*) : mog_struct.h */

#define VM (e->vm)

static int16_t sw(uint16_t v) { return (int16_t)v; }
static int8_t  sb(uint8_t v)  { return (int8_t)v; }

static void msg(IxEngine *e, const char *t)
{
    if (e->host && e->host->message)
        e->host->message(e->host->user, t);
}

void ix_layout_mog(IxLayout *l)
{
    l->entities     = MOG_t_Entities;
    l->temp_entity  = MOG_b_EntitySwap;
    l->contexts     = MOG_t_Contexts;
    l->shadow_ctx   = MOG_v_ShadowCtx;
    l->strike_lists = MOG_t_StrikeFrames;
    l->body_lists   = MOG_t_BodyFrames;
    l->bank_tables  = MOG_t_Banks;
    l->debug_flag   = MOG_v_Gore;
    l->vbl_counter  = MOG_v_VblCounter;
    l->flag_0d05    = MOG_v_BlitByCpu;
    l->combatants   = MOG_v_Combatants;
    l->objects_ptr  = MOG_v_Objects;
    l->scrap_ptr    = MOG_v_RestoreNext;
    l->scrap_count  = MOG_v_RestoreCount;
    l->list_body    = MOG_v_ListBody;
    l->list_strike  = MOG_v_ListStrike;
    l->bbox_x0      = MOG_v_BBoxX0;
    l->bbox_x1      = MOG_v_BBoxX1;
    l->bbox_y0      = MOG_v_BBoxY0;
    l->bbox_y1      = MOG_v_BBoxY1;
    l->bbox_set     = MOG_v_BBoxSet;
    l->loop_index   = MOG_v_LoopIndex;
    l->loop_entity  = MOG_v_LoopEntity;
    l->phys_moved   = MOG_v_PhysMoved;
    l->flip_buffer  = MOG_v_CelPlanesBuf;
    l->bitrev       = MOG_t_BitReverse;
    l->flip_size    = MOG_v_CelPlaneSize;
}

void ix_engine_init(IxEngine *e, IxVM *vm, const IxHost *host, const IxLayout *lay)
{
    memset(e, 0, sizeof *e);
    e->vm = vm;
    e->host = host;
    e->lay = *lay;
}

static void fill(IxEngine *e, uint32_t va, uint32_t n, uint8_t v)
{
    for (uint32_t i = 0; i < n; i++)
        ix_wb(VM, va + i, v);
}

static void copy(IxEngine *e, uint32_t dst, uint32_t src, uint32_t n)
{
    for (uint32_t i = 0; i < n; i++)
        ix_wb(VM, dst + i, ix_rb(VM, src + i));
}

/* Combat_ClearFrameLists [LAB_03C7] : 800 octets chacune. */
void ix_clear_frame_lists(IxEngine *e)
{
    fill(e, e->lay.strike_lists, 800, 0);
    fill(e, e->lay.body_lists, 800, 0);
}

/* LAB_0305 : partie « moteur » (LAB_03A7 et Combat_ClearHitLinks relèvent
 * des collisions). */
void ix_reset_entities(IxEngine *e)
{
    const IxLayout *l = &e->lay;
    fill(e, l->entities, 500, 0);
    fill(e, l->contexts, 36, 0);          /* comme l'original : 36 octets */
    ix_clear_frame_lists(e);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = l->entities + (uint32_t)i * IX_ENTITY_SIZE;
        ix_wl(VM, en + ENT_LIST_STRIKE, l->strike_lists + (uint32_t)i * 80);
        ix_wl(VM, en + ENT_LIST_BODY,   l->body_lists + (uint32_t)i * 80);
        ix_wl(VM, en + ENT_CTX,     l->contexts + (uint32_t)i * IX_CTX_SIZE);
    }
}

/* LAB_0315 */
uint32_t ix_find_entity(IxEngine *e, uint32_t object)
{
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = e->lay.entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (ix_rl(VM, en + ENT_OBJ) == object)
            return en;
    }
    return 0;
}

/* LAB_0310 */
uint32_t ix_start_entity(IxEngine *e, uint32_t script, uint32_t object,
                         uint32_t banks, int x, int height, int depth,
                         int dir, int controller)
{
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = e->lay.entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (ix_rb(VM, en + ENT_ACTIVE))
            continue;
        fill(e, ix_rl(VM, en + ENT_CTX), IX_CTX_SIZE, 0);
        ix_wl(VM, en + ENT_PC, script);
        ix_wl(VM, en + ENT_OBJ, object);
        ix_wl(VM, en + ENT_BANKS, banks);
        ix_ww(VM, en + ENT_X, (uint16_t)x);
        ix_ww(VM, en + ENT_HEIGHT, (uint16_t)height);
        ix_ww(VM, en + ENT_DEPTH, (uint16_t)depth);
        ix_wb(VM, en + ENT_DIR, (uint8_t)dir);
        ix_wb(VM, en + ENT_CTL, (uint8_t)controller);
        ix_wb(VM, en + ENT_ACTIVE, 1);
        ix_wb(VM, en + ENT_BUSY, 1);
        return en;
    }
    msg(e, "Cannot ALLOCATE a TASK");
    e->errors++;
    return 0;
}

/* Ent_Spawn [LAB_02D0] avec LAB_0171 (premier des 20 objets libres).
 * Renvoie l'objet (A1). Si aucun n'est libre, LAB_0171 rend l'adresse qui
 * suit le 20e objet, sans la marquer, et Ent_Spawn y écrit quand même. */
uint32_t ix_spawn(IxEngine *e, uint32_t script, uint32_t banks, int x,
                  int height, int depth, int dir, int controller)
{
    uint32_t obj = ix_rl(VM, e->lay.objects_ptr);
    int i;
    for (i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (ix_rl(VM, obj) == 0)
            break;
    if (i < 20)
        ix_wl(VM, obj, 1);
    ix_ww(VM, obj + OBJ_X, (uint16_t)x);
    ix_ww(VM, obj + OBJ_HEIGHT, (uint16_t)height);
    ix_ww(VM, obj + OBJ_DEPTH, (uint16_t)depth);
    ix_wb(VM, obj + OBJ_FACING, (uint8_t)dir);
    ix_wl(VM, obj + OBJ_BANKS, banks);
    ix_wl(VM, obj + OBJ_HIT, 0);
    ix_wl(VM, obj + OBJ_HIT_BY, 0);
    ix_wl(VM, obj + OBJ_USED, 1);
    ix_wb(VM, obj + OBJ_CONTROLLER, (uint8_t)controller);
    ix_start_entity(e, script, obj, banks, x, height, depth, dir, controller);
    return obj;
}

/* Cel_FlipFrame [LAB_0CCE] : retourne la frame en place (octets de chaque
 * ligne inversés, bits inversés par la table LAB_0CD9) ; l'octet
 * d'orientation passe de 1 (sens d'origine) à (remplissage << 4) et
 * inversement. */
static void cel_flip_frame(IxEngine *e, uint32_t cel, unsigned frame)
{
    const IxLayout *l = &e->lay;
    if ((int)frame >= sw(ix_rw(VM, cel)))
        return;
    uint32_t fe = cel + 10 + frame * 10;
    uint32_t a2 = ix_rl(VM, cel + 2) + ix_rl(VM, fe);
    uint16_t w = ix_rw(VM, fe + 4);
    uint16_t wr = (uint16_t)((w + 15) & 0xFFF0);
    uint16_t pad = (uint16_t)(wr - w);
    uint16_t bpr = (uint16_t)(wr >> 3);
    uint16_t h = ix_rw(VM, fe + 6);
    if (ix_rb(VM, fe + 8) & 1)
        ix_wb(VM, fe + 8, (uint8_t)(pad << 4));
    else
        ix_wb(VM, fe + 8, 1);
    uint8_t planes = ix_rb(VM, fe + 9);
    ix_wl(VM, l->flip_size, (uint32_t)(uint16_t)(bpr << 1) * h);
    uint32_t tmp = ix_rl(VM, l->flip_buffer);
    for (int p = 0; p < 5; p++) {
        if (!(planes & (1u << p)))
            continue;
        for (unsigned y = 0; y < h; y++, a2 += bpr) {
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, tmp + bpr - 1 - i, ix_rb(VM, l->bitrev + ix_rb(VM, a2 + i)));
            for (unsigned i = 0; i < bpr; i++)
                ix_wb(VM, a2 + i, ix_rb(VM, tmp + i));
        }
    }
}

void ix_flip_frame(IxEngine *e, uint32_t cel, unsigned frame)
{
    cel_flip_frame(e, cel, frame);
}

/* Ix_FrameInfo [LAB_034E] : dimensions lues dans la CEL en mémoire ; la
 * frame est retournée si son orientation diffère de celle de l'entité. */
static void frame_info(IxEngine *e, uint32_t cel, unsigned frame, uint32_t en)
{
    uint32_t fe = cel + frame * 10;
    ix_ww(VM, en + ENT_W, ix_rw(VM, fe + 14));
    ix_ww(VM, en + ENT_H, ix_rw(VM, fe + 16));
    uint8_t o = ix_rb(VM, fe + 18);
    if (o != 1)
        o = 3;
    if (o != ix_rb(VM, en + ENT_DIR))
        cel_flip_frame(e, cel, frame);
}

/* Ix_SortByDepth [LAB_0351] : tri à bulles sur la profondeur (mot non
 * signé), échange physique des entités via LAB_064A. */
static void sort_by_depth(IxEngine *e)
{
    const IxLayout *l = &e->lay;
    int swapped;
    do {
        swapped = 0;
        uint32_t a = l->entities;
        for (int i = 0; i < IX_ENTITY_COUNT - 1; i++, a += IX_ENTITY_SIZE) {
            uint16_t next = ix_rw(VM, a + IX_ENTITY_SIZE + ENT_DEPTH);
            if (next >= ix_rw(VM, a + ENT_DEPTH))
                continue;
            copy(e, l->temp_entity, a, IX_ENTITY_SIZE);
            copy(e, a, a + IX_ENTITY_SIZE, IX_ENTITY_SIZE);
            copy(e, a + IX_ENTITY_SIZE, l->temp_entity, IX_ENTITY_SIZE);
            swapped = 1;
        }
    } while (swapped);
}

/* Ix_AddBBox [LAB_03B8] */
static void add_bbox(IxEngine *e, int16_t x, int16_t w, int16_t y, int16_t h)
{
    const IxLayout *l = &e->lay;
    if (ix_rw(VM, l->bbox_set) == 0) {
        ix_ww(VM, l->bbox_x0, (uint16_t)x);
        ix_ww(VM, l->bbox_x1, (uint16_t)x);
        ix_ww(VM, l->bbox_y0, (uint16_t)y);
        ix_ww(VM, l->bbox_y1, (uint16_t)y);
        ix_ww(VM, l->bbox_set, 1);
    }
    if (!(x > sw(ix_rw(VM, l->bbox_x0))))
        ix_ww(VM, l->bbox_x0, (uint16_t)x);
    x = (int16_t)(x + w);
    if (!(x < sw(ix_rw(VM, l->bbox_x1))))
        ix_ww(VM, l->bbox_x1, (uint16_t)x);
    if (!(y > sw(ix_rw(VM, l->bbox_y0))))
        ix_ww(VM, l->bbox_y0, (uint16_t)y);
    y = (int16_t)(y + h);
    if (!(y < sw(ix_rw(VM, l->bbox_y1))))
        ix_ww(VM, l->bbox_y1, (uint16_t)y);
}

/* Ix_ResetCtx [LAB_039C] : 36 octets à zéro sauf le script secondaire. */
static void reset_ctx(IxEngine *e, uint32_t en)
{
    uint32_t c = ix_rl(VM, en + ENT_CTX);
    uint8_t  on = ix_rb(VM, c + CTX_SHADOW_ON);
    uint32_t pc = ix_rl(VM, c + CTX_SHADOW_PC);
    fill(e, c, IX_CTX_SIZE, 0);
    ix_wb(VM, c + CTX_SHADOW_ON, on);
    ix_wl(VM, c + CTX_SHADOW_PC, pc);
}

/* Taille d'un champ d'objet selon le type des opcodes $A8/$C8/$CC. */
static uint32_t read_field(IxEngine *e, uint32_t obj, int16_t off, uint8_t t)
{
    uint32_t a = obj + (uint32_t)(int32_t)off;
    if (t & 1) return ix_rb(VM, a);
    if (t & 2) return ix_rw(VM, a);
    return ix_rl(VM, a);
}

/* Ix_Physics [LAB_0378] — reproduit l'usage du registre D0 (octet/mot)
 * de l'original, y compris ses effets de bord. */
static void physics(IxEngine *e, uint32_t en, uint32_t c)
{
    int moved = 0;
    ix_ww(VM, e->lay.phys_moved, 0);
    uint32_t d0 = ix_rb(VM, c + CTX_PHY_VY);
    uint8_t fl = ix_rb(VM, c + CTX_PHY_FLAGS);

    if (fl & 0x02) {                                   /* LAB_0382 */
        moved = 1;
        uint16_t h = (uint16_t)(ix_rw(VM, en + ENT_HEIGHT) + (uint16_t)d0);
        ix_ww(VM, en + ENT_HEIGHT, h);
        if (sw(h) >= 0) {                              /* LAB_0384 */
            ix_ww(VM, en + ENT_HEIGHT, 0);
            ix_wb(VM, c + CTX_PHY_FLAGS, (uint8_t)(ix_rb(VM, c + CTX_PHY_FLAGS) & ~0x02));
        } else {
            d0 = (d0 & 0xFFFF0000u) | h;
            if (!(ix_rb(VM, c + CTX_PHY_FLAGS) & 0x20) &&
                !(sb((uint8_t)d0) >= sb(ix_rb(VM, c + CTX_PHY_VYMAX)))) {
                d0 = (d0 & ~0xFFu) | (uint8_t)(d0 << 1);
                ix_wb(VM, c + CTX_PHY_VY, (uint8_t)d0);
            }
        }
    } else if (fl & 0x01) {                            /* LAB_0380 */
        moved = 1;
        ix_ww(VM, en + ENT_HEIGHT, (uint16_t)(ix_rw(VM, en + ENT_HEIGHT) - (uint16_t)d0));
        if (!(ix_rb(VM, c + CTX_PHY_FLAGS) & 0x20) &&
            !(sb((uint8_t)d0) <= sb(ix_rb(VM, c + CTX_PHY_VYMAX)))) {
            d0 = (d0 & ~0xFFu) | (uint8_t)((uint8_t)d0 >> 1);
            ix_wb(VM, c + CTX_PHY_VY, (uint8_t)d0);
        }
    }

    d0 = (d0 & ~0xFFu) | ix_rb(VM, c + CTX_PHY_VX);      /* LAB_037A */
    for (int pass = 0; pass < 2; pass++) {
        uint8_t bit = pass == 0 ? 0x10 : 0x04;          /* LAB_0385 / LAB_0386 */
        if (!(ix_rb(VM, c + CTX_PHY_FLAGS) & bit))
            continue;
        int left = (ix_rb(VM, en + ENT_DIR) & 0x02) != 0;
        int sub = pass == 0 ? left : !left;            /* LAB_0387 : SUB */
        moved = 1;
        uint16_t x = ix_rw(VM, en + ENT_X);
        ix_ww(VM, en + ENT_X, (uint16_t)(sub ? x - (uint16_t)d0 : x + (uint16_t)d0));
        if (!(ix_rb(VM, c + CTX_PHY_FLAGS) & 0x80) &&
            !(sb((uint8_t)d0) <= sb(ix_rb(VM, c + CTX_PHY_VXMAX)))) {
            d0 = (d0 & ~0xFFu) | (uint8_t)((uint8_t)d0 >> 1);
            ix_wb(VM, c + CTX_PHY_VX, (uint8_t)d0);
        }
    }

    if (!(ix_rb(VM, c + CTX_PHY_FLAGS) & 0x40))          /* LAB_037C */
        ix_wl(VM, en + ENT_PC, ix_rl(VM, c + CTX_RESUME_PC));
    if (moved)
        ix_ww(VM, e->lay.phys_moved, 1);
    if (!moved)
        ix_wb(VM, c + CTX_PHY_ON, 0);
}

/* Écriture de l'entité dans son objet (LAB_034C). */
static void write_back(IxEngine *e, uint32_t en)
{
    const IxLayout *l = &e->lay;
    uint32_t o = ix_rl(VM, en + ENT_OBJ);
    ix_ww(VM, o + OBJ_X, ix_rw(VM, en + ENT_X));
    ix_ww(VM, o + OBJ_HEIGHT, ix_rw(VM, en + ENT_HEIGHT));
    ix_ww(VM, o + OBJ_DEPTH, ix_rw(VM, en + ENT_DEPTH));
    ix_wb(VM, o + OBJ_FACING, ix_rb(VM, en + ENT_DIR));
    ix_ww(VM, o + OBJ_BOX_X0, ix_rw(VM, l->bbox_x0));
    ix_ww(VM, o + OBJ_BOX_X1, ix_rw(VM, l->bbox_x1));
    ix_ww(VM, o + OBJ_BOX_Y0, ix_rw(VM, l->bbox_y0));
    ix_ww(VM, o + OBJ_BOX_Y1, ix_rw(VM, l->bbox_y1));
}

/* Ix_StepEnd [LAB_0341] ; pc = adresse de l'octet $FF. */
static void step_end(IxEngine *e, uint32_t en, uint32_t pc)
{
    uint32_t c = ix_rl(VM, en + ENT_CTX);

    if (ix_rb(VM, c + CTX_HOLD_ON)) {
        uint8_t n = (uint8_t)(ix_rb(VM, c + CTX_HOLD_N) - 1);
        ix_wb(VM, c + CTX_HOLD_N, n);
        if (n != 0) {
            ix_wl(VM, en + ENT_PC, ix_rl(VM, c + CTX_HOLD_PC));
            write_back(e, en);
            return;
        }
    }
    ix_wb(VM, c + CTX_HOLD_ON, 0);                       /* LAB_0342 */
    if (ix_rb(VM, c + CTX_PHY_ON)) {
        uint8_t n = (uint8_t)(ix_rb(VM, c + CTX_PHY_N) - 1);
        ix_wb(VM, c + CTX_PHY_N, n);
        if (!(n & 0x80)) {
            physics(e, en, c);
            write_back(e, en);
            return;
        }
    } else {
        ix_wb(VM, c + CTX_PHY_ON, 0);                    /* LAB_0343 */
    }
    if (ix_rb(VM, c + CTX_JUMP_ON)) {                    /* LAB_0344 */
        ix_wb(VM, c + CTX_JUMP_ON, 0);
        ix_wl(VM, en + ENT_PC, ix_rl(VM, c + CTX_JUMP_PC));
        write_back(e, en);
        return;
    }
    uint8_t xx = ix_rb(VM, pc + 1);                    /* LAB_0347 */
    if (xx == 0xFF || xx == 0xFE) {
        if (ix_rb(VM, c + CTX_LOOP_ON)) {
            uint8_t n = (uint8_t)(ix_rb(VM, c + CTX_LOOP_N) - 1);
            ix_wb(VM, c + CTX_LOOP_N, n);
            if (n != 0) {
                ix_wl(VM, en + ENT_PC, ix_rl(VM, c + CTX_LOOP_PC));
                write_back(e, en);
                return;
            }
        }
        ix_wb(VM, c + CTX_LOOP_ON, 0);
        if (xx == 0xFF) {                              /* LAB_034B */
            ix_wb(VM, en + ENT_BUSY, 0);
            write_back(e, en);
            return;
        }
    }
    ix_wl(VM, en + ENT_PC, pc + 2);                      /* LAB_0349 */
    write_back(e, en);
}

/* Opcodes $80-$D0 (t_IxOpcodes). Renvoie 0, ou -1 si l'opcode est
 * inexistant ou bloquant ($90, $9C) : l'étape est alors abandonnée. */
static int opcode(IxEngine *e, uint32_t en, uint32_t pc, uint8_t op)
{
    uint32_t c = ix_rl(VM, en + ENT_CTX);
    uint32_t obj = ix_rl(VM, en + ENT_OBJ);
    uint8_t a1 = ix_rb(VM, pc + 1);

    switch (op) {
    case 0x80:                                          /* IxOp80_SetDir */
        ix_wb(VM, en + ENT_DIR, a1 == 0xFF ? (uint8_t)(ix_rb(VM, en + ENT_DIR) ^ 2) : a1);
        ix_wb(VM, obj + OBJ_FACING, ix_rb(VM, en + ENT_DIR));
        ix_wl(VM, en + ENT_PC, pc + 2);
        return 0;
    case 0x84:                                          /* IxOp84_Jump */
        if (a1 == 3) {
            ix_wl(VM, en + ENT_PC, ix_rl(VM, pc + 2));
            return 0;
        }
        ix_wl(VM, c + CTX_JUMP_PC, ix_rl(VM, pc + 2));
        ix_wb(VM, c + CTX_JUMP_ON, 1);
        ix_wl(VM, en + ENT_PC, pc + 6);
        return 0;
    case 0x88: {                                        /* IxOp88_Hold */
        uint8_t n = a1;
        if (n == 0) {
            n = (uint8_t)(ix_rl(VM, e->lay.vbl_counter) & 0x1F);
            if (n == 0)
                n = 1;
        }
        ix_wb(VM, c + CTX_HOLD_N, n);
        ix_wb(VM, c + CTX_HOLD_ON, 1);
        ix_wl(VM, en + ENT_PC, pc + 2);
        ix_wl(VM, c + CTX_HOLD_PC, pc + 2);
        return 0;
    }
    case 0x8C:                                          /* IxOp8C_Physics */
        ix_wb(VM, c + CTX_PHY_ON, 1);
        ix_wb(VM, c + CTX_PHY_A, a1);
        ix_wb(VM, c + CTX_PHY_FLAGS, ix_rb(VM, pc + 3));
        ix_wb(VM, c + CTX_PHY_N, ix_rb(VM, pc + 2));
        ix_wb(VM, c + CTX_PHY_VY, ix_rb(VM, pc + 4));
        ix_wb(VM, c + CTX_PHY_VYMAX, ix_rb(VM, pc + 5));
        ix_wb(VM, c + CTX_PHY_VX, ix_rb(VM, pc + 6));
        ix_wb(VM, c + CTX_PHY_VXMAX, ix_rb(VM, pc + 7));
        ix_wl(VM, en + ENT_PC, pc + 8);
        ix_wl(VM, c + CTX_RESUME_PC, pc + 8);
        return 0;
    case 0x94:                                          /* IxOp94_Loop */
        ix_wb(VM, c + CTX_LOOP_N, a1);
        ix_wb(VM, c + CTX_LOOP_ON, 1);
        ix_wl(VM, en + ENT_PC, pc + 2);
        ix_wl(VM, c + CTX_LOOP_PC, pc + 2);
        return 0;
    case 0x98:                                          /* IxOp98_SkipIfDebug */
        if (ix_rl(VM, e->lay.debug_flag)) {
            msg(e, "Skipping ....");
            ix_wl(VM, en + ENT_PC, ix_rl(VM, pc + 2));
        } else {
            ix_wl(VM, en + ENT_PC, pc + 6);
        }
        return 0;
    case 0xA0: {                                        /* IxOpA0_Move */
        if (a1 & 0x40) {
            ix_ww(VM, en + ENT_X, ix_rw(VM, pc + 2));
            ix_ww(VM, en + ENT_HEIGHT, ix_rw(VM, pc + 4));
            ix_ww(VM, en + ENT_DEPTH, ix_rw(VM, pc + 6));
        } else {
            uint16_t d = ix_rw(VM, pc + 2), x = ix_rw(VM, en + ENT_X);
            int add = ix_rb(VM, en + ENT_DIR) == 3 ? !(a1 & 1) : (a1 & 1);
            ix_ww(VM, en + ENT_X, (uint16_t)(add ? x + d : x - d));
            d = ix_rw(VM, pc + 4);
            uint16_t h = ix_rw(VM, en + ENT_HEIGHT);
            ix_ww(VM, en + ENT_HEIGHT, (uint16_t)((a1 & 0x08) ? h - d : h + d));
            d = ix_rw(VM, pc + 6);
            uint16_t z = ix_rw(VM, en + ENT_DEPTH);
            ix_ww(VM, en + ENT_DEPTH, (uint16_t)((a1 & 0x20) ? z - d : z + d));
        }
        ix_wl(VM, en + ENT_PC, pc + 8);
        return 0;
    }
    case 0xA4:                                          /* IxOpA4_Sound */
        if (e->host && e->host->sound)
            e->host->sound(e->host->user, a1);
        ix_wl(VM, en + ENT_PC, pc + 2);
        return 0;
    case 0xA8: {                                        /* IxOpA8_SetField */
        uint32_t at = obj + (uint32_t)(int32_t)sw(ix_rw(VM, pc + 2));
        uint32_t v = ix_rl(VM, pc + 4);
        if (a1 & 1)      ix_wb(VM, at, (uint8_t)v);
        else if (a1 & 2) ix_ww(VM, at, (uint16_t)v);
        else             ix_wl(VM, at, v);
        ix_wl(VM, en + ENT_PC, pc + 8);
        return 0;
    }
    case 0xAC:                                          /* IxOpAC_Shadow */
        ix_wl(VM, c + CTX_SHADOW_PC, ix_rl(VM, pc + 2));
        ix_wb(VM, c + CTX_SHADOW_ON, a1 ? 1 : 0);
        ix_wl(VM, en + ENT_PC, pc + 6);
        return 0;
    case 0xB0:                                          /* IxOpB0_Call */
        if (e->host && e->host->call)
            e->host->call(e->host->user, ix_rl(VM, pc + 2), en);
        ix_wl(VM, en + ENT_PC, pc + 6);
        return 0;
    case 0xB4:                                          /* IxOpB4_IfDead */
        if (sw(ix_rw(VM, obj + OBJ_HP)) <= 0) {
            ix_wl(VM, en + ENT_PC, ix_rl(VM, pc + 2));
            reset_ctx(e, en);
        } else {
            ix_wl(VM, en + ENT_PC, pc + 6);
        }
        return 0;
    case 0xB8:                                          /* IxOpB8_Spawn */
        ix_spawn(e, ix_rl(VM, pc + 2), ix_rl(VM, en + ENT_BANKS),
                 sw(ix_rw(VM, en + ENT_X)), sw(ix_rw(VM, en + ENT_HEIGHT)),
                 sw(ix_rw(VM, en + ENT_DEPTH)), ix_rb(VM, en + ENT_DIR), 0x28);
        ix_wl(VM, en + ENT_PC, pc + 6);
        return 0;
    case 0xBC:                                          /* IxOpBC_Kill */
        ix_wl(VM, obj, 0);
        ix_ww(VM, en + ENT_ACTIVE, 0);
        ix_wl(VM, en + ENT_PC, pc + 2);
        return 0;
    case 0xC0: {                                        /* IxOpC0_SetBank */
        uint32_t banks = ix_rl(VM, e->lay.bank_tables + (uint32_t)((int32_t)(int16_t)(a1 - 1) * 4));
        ix_wl(VM, en + ENT_BANKS, banks);
        ix_wl(VM, en + ENT_PC, pc + 2);
        ix_wl(VM, obj + OBJ_BANKS, banks);
        return 0;
    }
    case 0xC4: {                                        /* IxOpC4_IfSameFacing */
        uint32_t p = ix_find_entity(e, ix_rl(VM, e->lay.combatants));
        if (p && ix_rb(VM, en + ENT_DIR) == ix_rb(VM, p + ENT_DIR))
            ix_wl(VM, en + ENT_PC, ix_rl(VM, pc + 2));
        else
            ix_wl(VM, en + ENT_PC, pc + 6);
        return 0;
    }
    case 0xC8:                                          /* IxOpC8_IfFieldZero */
    case 0xCC: {                                        /* IxOpCC_IfFieldNonZero */
        uint32_t v = read_field(e, obj, sw(ix_rw(VM, pc + 2)), a1);
        if ((op == 0xC8) == (v == 0))
            ix_wl(VM, en + ENT_PC, ix_rl(VM, pc + 4));
        else
            ix_wl(VM, en + ENT_PC, pc + 8);
        return 0;
    }
    case 0xD0:                                          /* IxOpD0_Reset */
        reset_ctx(e, en);
        ix_wl(VM, en + ENT_PC, pc + 2);
        return 0;
    default:
        e->errors++;
        msg(e, "opcode IMAGEXCEL inexistant ou bloquant");
        return -1;
    }
}

/* Ix_Step [LAB_032E] */
void ix_step(IxEngine *e, uint32_t en)
{
    const IxLayout *l = &e->lay;
    int guard = 0;

    while (ix_rl(VM, en + ENT_PC) != 0) {                /* LAB_032F */
        uint32_t pc = ix_rl(VM, en + ENT_PC);            /* LAB_0330 */
        uint8_t b = ix_rb(VM, pc);

        if (++guard > 10000) {                         /* script qui boucle */
            e->errors++;
            msg(e, "étape de script sans fin");
            return;
        }
        if (b == 0xFF) {
            step_end(e, en, pc);
            return;
        }
        if (b == 0xFD) {
            ix_wl(VM, en + ENT_PC, ix_rl(VM, ix_rl(VM, en + ENT_CTX) + CTX_RESUME_PC));
            continue;
        }
        if (b == 0xFE) {
            ix_wl(VM, en + ENT_PC, ix_rl(VM, ix_rl(VM, en + ENT_CTX) + CTX_LOOP_PC));
            continue;
        }
        if (b & 0x80) {
            if (opcode(e, en, pc, b) < 0)
                return;
            continue;
        }

        /* dessin */
        uint32_t cel = ix_rl(VM, ix_rl(VM, en + ENT_BANKS) + (b & 0x1F));
        uint8_t frame = ix_rb(VM, pc + 1);
        ix_wb(VM, en + ENT_FRAME, frame);
        if (e->host && e->host->frame_info) {           /* dimensions fournies par l'hôte */
            int w = 0, h = 0;
            e->host->frame_info(e->host->user, cel, frame, &w, &h);
            ix_ww(VM, en + ENT_W, (uint16_t)w);
            ix_ww(VM, en + ENT_H, (uint16_t)h);
        } else {
            frame_info(e, cel, frame, en);
        }
        int flipped = (ix_rb(VM, en + ENT_DIR) & 0x02) != 0;
        int16_t d2 = sb(ix_rb(VM, pc + 2));
        int16_t d1;
        if (!flipped)
            d1 = (int16_t)(ix_rw(VM, pc + 4) + ix_rw(VM, en + ENT_X));
        else
            d1 = (int16_t)(ix_rw(VM, en + ENT_X) - ix_rw(VM, pc + 4) - ix_rw(VM, en + ENT_W));
        d2 = (int16_t)(d2 + ix_rw(VM, en + ENT_HEIGHT) + ix_rw(VM, en + ENT_DEPTH));
        ix_ww(VM, en + ENT_DRAW_X, (uint16_t)d1);
        ix_ww(VM, en + ENT_DRAW_Y, (uint16_t)d2);

        uint8_t fl = ix_rb(VM, pc + 3);
        if (!(fl & 0x40))
            add_bbox(e, d1, sw(ix_rw(VM, en + ENT_W)), d2, sw(ix_rw(VM, en + ENT_H)));

        if (!(ix_rl(VM, l->debug_flag) && (fl & 0x80))) {
            ix_ww(VM, l->flag_0d05, (fl & 0x20) ? 1 : 0);
            if (fl & 0x10) {
                if (e->host && e->host->draw)
                    e->host->draw(e->host->user, cel, frame, d1, d2, flipped, 1);
            } else {                                    /* LAB_033A */
                uint32_t sp = ix_rl(VM, l->scrap_ptr);
                uint16_t n = (uint16_t)(ix_rw(VM, l->scrap_count) + 1);
                ix_ww(VM, l->scrap_count, n);
                if (sw(n) > 0x2D) {
                    msg(e, "***** Scrap pile has reached maximum *******");
                } else {
                    ix_ww(VM, sp + 0, ix_rw(VM, en + ENT_DRAW_X));
                    ix_ww(VM, sp + 2, ix_rw(VM, en + ENT_DRAW_Y));
                    ix_ww(VM, sp + 4, ix_rw(VM, en + ENT_W));
                    ix_ww(VM, sp + 6, ix_rw(VM, en + ENT_H));
                    ix_ww(VM, sp + 12, 0xFFFF);
                    ix_wl(VM, l->scrap_ptr, sp + 8);
                }
            }
            if (fl & 0x02) {                            /* LAB_033C */
                uint32_t a = ix_rl(VM, l->list_strike);
                ix_wl(VM, a, cel);
                ix_ww(VM, a + 4, frame);
                ix_ww(VM, a + 6, (uint16_t)d1);
                ix_ww(VM, a + 8, (uint16_t)d2);
                ix_wl(VM, a + 10, 0);
                ix_wl(VM, l->list_strike, a + 10);
            }
            if (fl & 0x01) {                            /* LAB_033D */
                uint32_t a = ix_rl(VM, l->list_body);
                ix_wl(VM, a, cel);
                ix_ww(VM, a + 4, frame);
                ix_ww(VM, a + 6, (uint16_t)d1);
                ix_ww(VM, a + 8, (uint16_t)d2);
                ix_wl(VM, a + 10, 0);
                ix_wl(VM, l->list_body, a + 10);
            }
            if (e->host && e->host->draw)               /* LAB_033E */
                e->host->draw(e->host->user, cel, frame, d1, d2, flipped, 0);
        }
        ix_wl(VM, en + ENT_PC, pc + 6);                   /* LAB_033F */
    }
}

/* Ix_RunEntities [LAB_0328] */
void ix_run_entities(IxEngine *e)
{
    const IxLayout *l = &e->lay;
    sort_by_depth(e);

    ix_ww(VM, l->loop_index, 0);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {        /* scripts secondaires */
        uint32_t en = l->entities + (uint32_t)i * IX_ENTITY_SIZE;
        ix_wl(VM, l->loop_entity, en);
        ix_ww(VM, l->loop_index, (uint16_t)(i + 1));
        if (!ix_rb(VM, en + ENT_ACTIVE) || ix_rw(VM, en + ENT_FROZEN))
            continue;
        uint32_t c = ix_rl(VM, en + ENT_CTX);
        if (!ix_rb(VM, c + CTX_SHADOW_ON))
            continue;
        uint32_t t = l->temp_entity;
        copy(e, t, en, IX_ENTITY_SIZE);
        ix_ww(VM, t + ENT_HEIGHT, 0);
        ix_wl(VM, t + ENT_PC, ix_rl(VM, c + CTX_SHADOW_PC));
        ix_wl(VM, t + ENT_CTX, l->shadow_ctx);
        ix_step(e, t);
    }

    ix_ww(VM, l->loop_index, 0);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = l->entities + (uint32_t)i * IX_ENTITY_SIZE;
        ix_wl(VM, l->loop_entity, en);
        ix_ww(VM, l->loop_index, (uint16_t)(i + 1));
        if (!ix_rb(VM, en + ENT_ACTIVE) || ix_rw(VM, en + ENT_FROZEN))
            continue;
        ix_wl(VM, l->list_body, ix_rl(VM, en + ENT_LIST_BODY));
        ix_wl(VM, l->list_strike, ix_rl(VM, en + ENT_LIST_STRIKE));
        ix_ww(VM, l->bbox_set, 0);
        ix_ww(VM, l->bbox_x0, 0);
        ix_ww(VM, l->bbox_x1, 0);
        ix_ww(VM, l->bbox_y0, 0);
        ix_ww(VM, l->bbox_y1, 0);
        ix_step(e, en);
    }
}
