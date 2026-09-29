/*
 * ix_engine.c — moteur de scripts IMAGEXCEL du combat (mog).
 *
 * Traduction des routines de amiga_asm/mog.asm ; chaque fonction indique la
 * routine d'origine. L'ordre des opérations, la taille des accès et le
 * signe des comparaisons suivent le code 68000 : tools/ix_difftest.py
 * compare ce moteur au code d'origine exécuté par un émulateur 68000.
 */
#include "ix_engine.h"
#include "ix_mog_syms.h"

#include <string.h>

/* Champs d'une entité (t_Entities, 50 octets). */
enum {
    E_ACTIVE = 0, E_BUSY = 1, E_PC = 2, E_X = 6, E_H = 8, E_D = 10,
    E_DRAWX = 12, E_DRAWY = 14, E_W = 16, E_HT = 18, E_FRAME = 21,
    E_DIR = 22, E_OBJ = 24, E_BANKS = 28, E_CTL = 32, E_CTX = 36,
    E_LSTRIKE = 40, E_LBODY = 44, E_FROZEN = 48
};

/* Champs du contexte de script (36 octets). */
enum {
    C_HOLD_N = 0, C_HOLD_ON = 1, C_HOLD_PC = 2,
    C_LOOP_N = 6, C_LOOP_ON = 7, C_LOOP_PC = 8,
    C_JUMP_PC = 12, C_JUMP_ON = 16,
    C_SHADOW_ON = 18, C_SHADOW_PC = 20,
    C_PHY_A = 24, C_PHY_FLAGS = 25, C_PHY_ON = 26, C_PHY_N = 27,
    C_PHY_VY = 28, C_PHY_VYMAX = 29, C_PHY_VX = 30, C_PHY_VXMAX = 31,
    C_RESUME_PC = 32
};

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
    l->temp_entity  = MOG_LAB_064A;
    l->contexts     = MOG_LAB_064B;
    l->shadow_ctx   = MOG_LAB_064C;
    l->strike_lists = MOG_t_StrikeFrames;
    l->body_lists   = MOG_t_BodyFrames;
    l->bank_tables  = MOG_LAB_0647;
    l->debug_flag   = MOG_LAB_06DA;
    l->vbl_counter  = MOG_v_VblCounter;
    l->flag_0d05    = MOG_LAB_0D05;
    l->combatants   = MOG_v_Combatants;
    l->objects_ptr  = MOG_LAB_05C3;
    l->scrap_ptr    = MOG_LAB_0641;
    l->scrap_count  = MOG_LAB_0645;
    l->list_body    = MOG_LAB_0643;
    l->list_strike  = MOG_LAB_0644;
    l->bbox_x0      = MOG_LAB_0639;
    l->bbox_x1      = MOG_LAB_0638;
    l->bbox_y0      = MOG_LAB_063A;
    l->bbox_y1      = MOG_LAB_063B;
    l->bbox_set     = MOG_LAB_063C;
    l->loop_index   = MOG_LAB_063D;
    l->loop_entity  = MOG_LAB_0640;
    l->phys_moved   = MOG_LAB_037F;
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
        ix_wl(VM, en + E_LSTRIKE, l->strike_lists + (uint32_t)i * 80);
        ix_wl(VM, en + E_LBODY,   l->body_lists + (uint32_t)i * 80);
        ix_wl(VM, en + E_CTX,     l->contexts + (uint32_t)i * IX_CTX_SIZE);
    }
}

/* LAB_0315 */
uint32_t ix_find_entity(IxEngine *e, uint32_t object)
{
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = e->lay.entities + (uint32_t)i * IX_ENTITY_SIZE;
        if (ix_rl(VM, en + E_OBJ) == object)
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
        if (ix_rb(VM, en + E_ACTIVE))
            continue;
        fill(e, ix_rl(VM, en + E_CTX), IX_CTX_SIZE, 0);
        ix_wl(VM, en + E_PC, script);
        ix_wl(VM, en + E_OBJ, object);
        ix_wl(VM, en + E_BANKS, banks);
        ix_ww(VM, en + E_X, (uint16_t)x);
        ix_ww(VM, en + E_H, (uint16_t)height);
        ix_ww(VM, en + E_D, (uint16_t)depth);
        ix_wb(VM, en + E_DIR, (uint8_t)dir);
        ix_wb(VM, en + E_CTL, (uint8_t)controller);
        ix_wb(VM, en + E_ACTIVE, 1);
        ix_wb(VM, en + E_BUSY, 1);
        return en;
    }
    msg(e, "Cannot ALLOCATE a TASK");
    e->errors++;
    return 0;
}

/* Ent_Spawn [LAB_02D0] avec LAB_0171 (premier des 20 objets libres). */
uint32_t ix_spawn(IxEngine *e, uint32_t script, uint32_t banks, int x,
                  int height, int depth, int dir, int controller)
{
    uint32_t obj = ix_rl(VM, e->lay.objects_ptr);
    int i;
    for (i = 0; i < 20; i++, obj += IX_OBJECT_SIZE)
        if (ix_rl(VM, obj) == 0)
            break;
    if (i == 20) {
        /* LAB_0171 rend alors A1 après le dernier objet, sans le marquer ;
         * Ent_Spawn écrit quand même : on signale l'erreur sans écrire. */
        e->errors++;
        msg(e, "Ent_Spawn : plus d'objet libre");
        return 0;
    }
    ix_wl(VM, obj, 1);
    ix_ww(VM, obj + 4, (uint16_t)x);
    ix_ww(VM, obj + 6, (uint16_t)height);
    ix_ww(VM, obj + 8, (uint16_t)depth);
    ix_wb(VM, obj + 10, (uint8_t)dir);
    ix_wl(VM, obj + 38, banks);
    ix_wl(VM, obj + 14, 0);
    ix_wl(VM, obj + 18, 0);
    ix_wl(VM, obj + 0, 1);
    ix_wb(VM, obj + 77, (uint8_t)controller);
    return ix_start_entity(e, script, obj, banks, x, height, depth, dir, controller);
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
            uint16_t next = ix_rw(VM, a + IX_ENTITY_SIZE + E_D);
            if (next >= ix_rw(VM, a + E_D))
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
    uint32_t c = ix_rl(VM, en + E_CTX);
    uint8_t  on = ix_rb(VM, c + C_SHADOW_ON);
    uint32_t pc = ix_rl(VM, c + C_SHADOW_PC);
    fill(e, c, IX_CTX_SIZE, 0);
    ix_wb(VM, c + C_SHADOW_ON, on);
    ix_wl(VM, c + C_SHADOW_PC, pc);
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
    uint32_t d0 = ix_rb(VM, c + C_PHY_VY);
    uint8_t fl = ix_rb(VM, c + C_PHY_FLAGS);

    if (fl & 0x02) {                                   /* LAB_0382 */
        moved = 1;
        uint16_t h = (uint16_t)(ix_rw(VM, en + E_H) + (uint16_t)d0);
        ix_ww(VM, en + E_H, h);
        if (sw(h) >= 0) {                              /* LAB_0384 */
            ix_ww(VM, en + E_H, 0);
            ix_wb(VM, c + C_PHY_FLAGS, (uint8_t)(ix_rb(VM, c + C_PHY_FLAGS) & ~0x02));
        } else {
            d0 = (d0 & 0xFFFF0000u) | h;
            if (!(ix_rb(VM, c + C_PHY_FLAGS) & 0x20) &&
                !(sb((uint8_t)d0) >= sb(ix_rb(VM, c + C_PHY_VYMAX)))) {
                d0 = (d0 & ~0xFFu) | (uint8_t)(d0 << 1);
                ix_wb(VM, c + C_PHY_VY, (uint8_t)d0);
            }
        }
    } else if (fl & 0x01) {                            /* LAB_0380 */
        moved = 1;
        ix_ww(VM, en + E_H, (uint16_t)(ix_rw(VM, en + E_H) - (uint16_t)d0));
        if (!(ix_rb(VM, c + C_PHY_FLAGS) & 0x20) &&
            !(sb((uint8_t)d0) <= sb(ix_rb(VM, c + C_PHY_VYMAX)))) {
            d0 = (d0 & ~0xFFu) | (uint8_t)((uint8_t)d0 >> 1);
            ix_wb(VM, c + C_PHY_VY, (uint8_t)d0);
        }
    }

    d0 = (d0 & ~0xFFu) | ix_rb(VM, c + C_PHY_VX);      /* LAB_037A */
    for (int pass = 0; pass < 2; pass++) {
        uint8_t bit = pass == 0 ? 0x10 : 0x04;          /* LAB_0385 / LAB_0386 */
        if (!(ix_rb(VM, c + C_PHY_FLAGS) & bit))
            continue;
        int left = (ix_rb(VM, en + E_DIR) & 0x02) != 0;
        int sub = pass == 0 ? left : !left;            /* LAB_0387 : SUB */
        moved = 1;
        uint16_t x = ix_rw(VM, en + E_X);
        ix_ww(VM, en + E_X, (uint16_t)(sub ? x - (uint16_t)d0 : x + (uint16_t)d0));
        if (!(ix_rb(VM, c + C_PHY_FLAGS) & 0x80) &&
            !(sb((uint8_t)d0) <= sb(ix_rb(VM, c + C_PHY_VXMAX)))) {
            d0 = (d0 & ~0xFFu) | (uint8_t)((uint8_t)d0 >> 1);
            ix_wb(VM, c + C_PHY_VX, (uint8_t)d0);
        }
    }

    if (!(ix_rb(VM, c + C_PHY_FLAGS) & 0x40))          /* LAB_037C */
        ix_wl(VM, en + E_PC, ix_rl(VM, c + C_RESUME_PC));
    if (moved)
        ix_ww(VM, e->lay.phys_moved, 1);
    if (!moved)
        ix_wb(VM, c + C_PHY_ON, 0);
}

/* Écriture de l'entité dans son objet (LAB_034C). */
static void write_back(IxEngine *e, uint32_t en)
{
    const IxLayout *l = &e->lay;
    uint32_t o = ix_rl(VM, en + E_OBJ);
    ix_ww(VM, o + 4, ix_rw(VM, en + E_X));
    ix_ww(VM, o + 6, ix_rw(VM, en + E_H));
    ix_ww(VM, o + 8, ix_rw(VM, en + E_D));
    ix_wb(VM, o + 10, ix_rb(VM, en + E_DIR));
    ix_ww(VM, o + 58, ix_rw(VM, l->bbox_x0));
    ix_ww(VM, o + 60, ix_rw(VM, l->bbox_x1));
    ix_ww(VM, o + 112, ix_rw(VM, l->bbox_y0));
    ix_ww(VM, o + 114, ix_rw(VM, l->bbox_y1));
}

/* Ix_StepEnd [LAB_0341] ; pc = adresse de l'octet $FF. */
static void step_end(IxEngine *e, uint32_t en, uint32_t pc)
{
    uint32_t c = ix_rl(VM, en + E_CTX);

    if (ix_rb(VM, c + C_HOLD_ON)) {
        uint8_t n = (uint8_t)(ix_rb(VM, c + C_HOLD_N) - 1);
        ix_wb(VM, c + C_HOLD_N, n);
        if (n != 0) {
            ix_wl(VM, en + E_PC, ix_rl(VM, c + C_HOLD_PC));
            write_back(e, en);
            return;
        }
    }
    ix_wb(VM, c + C_HOLD_ON, 0);                       /* LAB_0342 */
    if (ix_rb(VM, c + C_PHY_ON)) {
        uint8_t n = (uint8_t)(ix_rb(VM, c + C_PHY_N) - 1);
        ix_wb(VM, c + C_PHY_N, n);
        if (!(n & 0x80)) {
            physics(e, en, c);
            write_back(e, en);
            return;
        }
    } else {
        ix_wb(VM, c + C_PHY_ON, 0);                    /* LAB_0343 */
    }
    if (ix_rb(VM, c + C_JUMP_ON)) {                    /* LAB_0344 */
        ix_wb(VM, c + C_JUMP_ON, 0);
        ix_wl(VM, en + E_PC, ix_rl(VM, c + C_JUMP_PC));
        write_back(e, en);
        return;
    }
    uint8_t xx = ix_rb(VM, pc + 1);                    /* LAB_0347 */
    if (xx == 0xFF || xx == 0xFE) {
        if (ix_rb(VM, c + C_LOOP_ON)) {
            uint8_t n = (uint8_t)(ix_rb(VM, c + C_LOOP_N) - 1);
            ix_wb(VM, c + C_LOOP_N, n);
            if (n != 0) {
                ix_wl(VM, en + E_PC, ix_rl(VM, c + C_LOOP_PC));
                write_back(e, en);
                return;
            }
        }
        ix_wb(VM, c + C_LOOP_ON, 0);
        if (xx == 0xFF) {                              /* LAB_034B */
            ix_wb(VM, en + E_BUSY, 0);
            write_back(e, en);
            return;
        }
    }
    ix_wl(VM, en + E_PC, pc + 2);                      /* LAB_0349 */
    write_back(e, en);
}

/* Opcodes $80-$D0 (t_IxOpcodes). Renvoie 0, ou -1 si l'opcode est
 * inexistant ou bloquant ($90, $9C) : l'étape est alors abandonnée. */
static int opcode(IxEngine *e, uint32_t en, uint32_t pc, uint8_t op)
{
    uint32_t c = ix_rl(VM, en + E_CTX);
    uint32_t obj = ix_rl(VM, en + E_OBJ);
    uint8_t a1 = ix_rb(VM, pc + 1);

    switch (op) {
    case 0x80:                                          /* IxOp80_SetDir */
        ix_wb(VM, en + E_DIR, a1 == 0xFF ? (uint8_t)(ix_rb(VM, en + E_DIR) ^ 2) : a1);
        ix_wb(VM, obj + 10, ix_rb(VM, en + E_DIR));
        ix_wl(VM, en + E_PC, pc + 2);
        return 0;
    case 0x84:                                          /* IxOp84_Jump */
        if (a1 == 3) {
            ix_wl(VM, en + E_PC, ix_rl(VM, pc + 2));
            return 0;
        }
        ix_wl(VM, c + C_JUMP_PC, ix_rl(VM, pc + 2));
        ix_wb(VM, c + C_JUMP_ON, 1);
        ix_wl(VM, en + E_PC, pc + 6);
        return 0;
    case 0x88: {                                        /* IxOp88_Hold */
        uint8_t n = a1;
        if (n == 0) {
            n = (uint8_t)(ix_rl(VM, e->lay.vbl_counter) & 0x1F);
            if (n == 0)
                n = 1;
        }
        ix_wb(VM, c + C_HOLD_N, n);
        ix_wb(VM, c + C_HOLD_ON, 1);
        ix_wl(VM, en + E_PC, pc + 2);
        ix_wl(VM, c + C_HOLD_PC, pc + 2);
        return 0;
    }
    case 0x8C:                                          /* IxOp8C_Physics */
        ix_wb(VM, c + C_PHY_ON, 1);
        ix_wb(VM, c + C_PHY_A, a1);
        ix_wb(VM, c + C_PHY_FLAGS, ix_rb(VM, pc + 3));
        ix_wb(VM, c + C_PHY_N, ix_rb(VM, pc + 2));
        ix_wb(VM, c + C_PHY_VY, ix_rb(VM, pc + 4));
        ix_wb(VM, c + C_PHY_VYMAX, ix_rb(VM, pc + 5));
        ix_wb(VM, c + C_PHY_VX, ix_rb(VM, pc + 6));
        ix_wb(VM, c + C_PHY_VXMAX, ix_rb(VM, pc + 7));
        ix_wl(VM, en + E_PC, pc + 8);
        ix_wl(VM, c + C_RESUME_PC, pc + 8);
        return 0;
    case 0x94:                                          /* IxOp94_Loop */
        ix_wb(VM, c + C_LOOP_N, a1);
        ix_wb(VM, c + C_LOOP_ON, 1);
        ix_wl(VM, en + E_PC, pc + 2);
        ix_wl(VM, c + C_LOOP_PC, pc + 2);
        return 0;
    case 0x98:                                          /* IxOp98_SkipIfDebug */
        if (ix_rl(VM, e->lay.debug_flag)) {
            msg(e, "Skipping ....");
            ix_wl(VM, en + E_PC, ix_rl(VM, pc + 2));
        } else {
            ix_wl(VM, en + E_PC, pc + 6);
        }
        return 0;
    case 0xA0: {                                        /* IxOpA0_Move */
        if (a1 & 0x40) {
            ix_ww(VM, en + E_X, ix_rw(VM, pc + 2));
            ix_ww(VM, en + E_H, ix_rw(VM, pc + 4));
            ix_ww(VM, en + E_D, ix_rw(VM, pc + 6));
        } else {
            uint16_t d = ix_rw(VM, pc + 2), x = ix_rw(VM, en + E_X);
            int add = ix_rb(VM, en + E_DIR) == 3 ? !(a1 & 1) : (a1 & 1);
            ix_ww(VM, en + E_X, (uint16_t)(add ? x + d : x - d));
            d = ix_rw(VM, pc + 4);
            uint16_t h = ix_rw(VM, en + E_H);
            ix_ww(VM, en + E_H, (uint16_t)((a1 & 0x08) ? h - d : h + d));
            d = ix_rw(VM, pc + 6);
            uint16_t z = ix_rw(VM, en + E_D);
            ix_ww(VM, en + E_D, (uint16_t)((a1 & 0x20) ? z - d : z + d));
        }
        ix_wl(VM, en + E_PC, pc + 8);
        return 0;
    }
    case 0xA4:                                          /* IxOpA4_Sound */
        if (e->host && e->host->sound)
            e->host->sound(e->host->user, a1);
        ix_wl(VM, en + E_PC, pc + 2);
        return 0;
    case 0xA8: {                                        /* IxOpA8_SetField */
        uint32_t at = obj + (uint32_t)(int32_t)sw(ix_rw(VM, pc + 2));
        uint32_t v = ix_rl(VM, pc + 4);
        if (a1 & 1)      ix_wb(VM, at, (uint8_t)v);
        else if (a1 & 2) ix_ww(VM, at, (uint16_t)v);
        else             ix_wl(VM, at, v);
        ix_wl(VM, en + E_PC, pc + 8);
        return 0;
    }
    case 0xAC:                                          /* IxOpAC_Shadow */
        ix_wl(VM, c + C_SHADOW_PC, ix_rl(VM, pc + 2));
        ix_wb(VM, c + C_SHADOW_ON, a1 ? 1 : 0);
        ix_wl(VM, en + E_PC, pc + 6);
        return 0;
    case 0xB0:                                          /* IxOpB0_Call */
        if (e->host && e->host->call)
            e->host->call(e->host->user, ix_rl(VM, pc + 2), en);
        ix_wl(VM, en + E_PC, pc + 6);
        return 0;
    case 0xB4:                                          /* IxOpB4_IfDead */
        if (sw(ix_rw(VM, obj + 80)) <= 0) {
            ix_wl(VM, en + E_PC, ix_rl(VM, pc + 2));
            reset_ctx(e, en);
        } else {
            ix_wl(VM, en + E_PC, pc + 6);
        }
        return 0;
    case 0xB8:                                          /* IxOpB8_Spawn */
        ix_spawn(e, ix_rl(VM, pc + 2), ix_rl(VM, en + E_BANKS),
                 sw(ix_rw(VM, en + E_X)), sw(ix_rw(VM, en + E_H)),
                 sw(ix_rw(VM, en + E_D)), ix_rb(VM, en + E_DIR), 0x28);
        ix_wl(VM, en + E_PC, pc + 6);
        return 0;
    case 0xBC:                                          /* IxOpBC_Kill */
        ix_wl(VM, obj, 0);
        ix_ww(VM, en + E_ACTIVE, 0);
        ix_wl(VM, en + E_PC, pc + 2);
        return 0;
    case 0xC0: {                                        /* IxOpC0_SetBank */
        uint32_t banks = ix_rl(VM, e->lay.bank_tables + (uint32_t)((int32_t)(int16_t)(a1 - 1) * 4));
        ix_wl(VM, en + E_BANKS, banks);
        ix_wl(VM, en + E_PC, pc + 2);
        ix_wl(VM, obj + 38, banks);
        return 0;
    }
    case 0xC4: {                                        /* IxOpC4_IfSameFacing */
        uint32_t p = ix_find_entity(e, ix_rl(VM, e->lay.combatants));
        if (p && ix_rb(VM, en + E_DIR) == ix_rb(VM, p + E_DIR))
            ix_wl(VM, en + E_PC, ix_rl(VM, pc + 2));
        else
            ix_wl(VM, en + E_PC, pc + 6);
        return 0;
    }
    case 0xC8:                                          /* IxOpC8_IfFieldZero */
    case 0xCC: {                                        /* IxOpCC_IfFieldNonZero */
        uint32_t v = read_field(e, obj, sw(ix_rw(VM, pc + 2)), a1);
        if ((op == 0xC8) == (v == 0))
            ix_wl(VM, en + E_PC, ix_rl(VM, pc + 4));
        else
            ix_wl(VM, en + E_PC, pc + 8);
        return 0;
    }
    case 0xD0:                                          /* IxOpD0_Reset */
        reset_ctx(e, en);
        ix_wl(VM, en + E_PC, pc + 2);
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

    while (ix_rl(VM, en + E_PC) != 0) {                /* LAB_032F */
        uint32_t pc = ix_rl(VM, en + E_PC);            /* LAB_0330 */
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
            ix_wl(VM, en + E_PC, ix_rl(VM, ix_rl(VM, en + E_CTX) + C_RESUME_PC));
            continue;
        }
        if (b == 0xFE) {
            ix_wl(VM, en + E_PC, ix_rl(VM, ix_rl(VM, en + E_CTX) + C_LOOP_PC));
            continue;
        }
        if (b & 0x80) {
            if (opcode(e, en, pc, b) < 0)
                return;
            continue;
        }

        /* dessin */
        uint32_t cel = ix_rl(VM, ix_rl(VM, en + E_BANKS) + (b & 0x1F));
        uint8_t frame = ix_rb(VM, pc + 1);
        ix_wb(VM, en + E_FRAME, frame);
        {                                               /* Ix_FrameInfo */
            int w = 0, h = 0;
            if (e->host && e->host->frame_info)
                e->host->frame_info(e->host->user, cel, frame, &w, &h);
            ix_ww(VM, en + E_W, (uint16_t)w);
            ix_ww(VM, en + E_HT, (uint16_t)h);
        }
        int flipped = (ix_rb(VM, en + E_DIR) & 0x02) != 0;
        int16_t d2 = sb(ix_rb(VM, pc + 2));
        int16_t d1;
        if (!flipped)
            d1 = (int16_t)(ix_rw(VM, pc + 4) + ix_rw(VM, en + E_X));
        else
            d1 = (int16_t)(ix_rw(VM, en + E_X) - ix_rw(VM, pc + 4) - ix_rw(VM, en + E_W));
        d2 = (int16_t)(d2 + ix_rw(VM, en + E_H) + ix_rw(VM, en + E_D));
        ix_ww(VM, en + E_DRAWX, (uint16_t)d1);
        ix_ww(VM, en + E_DRAWY, (uint16_t)d2);

        uint8_t fl = ix_rb(VM, pc + 3);
        if (!(fl & 0x40))
            add_bbox(e, d1, sw(ix_rw(VM, en + E_W)), d2, sw(ix_rw(VM, en + E_HT)));

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
                    ix_ww(VM, sp + 0, ix_rw(VM, en + E_DRAWX));
                    ix_ww(VM, sp + 2, ix_rw(VM, en + E_DRAWY));
                    ix_ww(VM, sp + 4, ix_rw(VM, en + E_W));
                    ix_ww(VM, sp + 6, ix_rw(VM, en + E_HT));
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
        ix_wl(VM, en + E_PC, pc + 6);                   /* LAB_033F */
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
        if (!ix_rb(VM, en + E_ACTIVE) || ix_rw(VM, en + E_FROZEN))
            continue;
        uint32_t c = ix_rl(VM, en + E_CTX);
        if (!ix_rb(VM, c + C_SHADOW_ON))
            continue;
        uint32_t t = l->temp_entity;
        copy(e, t, en, IX_ENTITY_SIZE);
        ix_ww(VM, t + E_H, 0);
        ix_wl(VM, t + E_PC, ix_rl(VM, c + C_SHADOW_PC));
        ix_wl(VM, t + E_CTX, l->shadow_ctx);
        ix_step(e, t);
    }

    ix_ww(VM, l->loop_index, 0);
    for (int i = 0; i < IX_ENTITY_COUNT; i++) {
        uint32_t en = l->entities + (uint32_t)i * IX_ENTITY_SIZE;
        ix_wl(VM, l->loop_entity, en);
        ix_ww(VM, l->loop_index, (uint16_t)(i + 1));
        if (!ix_rb(VM, en + E_ACTIVE) || ix_rw(VM, en + E_FROZEN))
            continue;
        ix_wl(VM, l->list_body, ix_rl(VM, en + E_LBODY));
        ix_wl(VM, l->list_strike, ix_rl(VM, en + E_LSTRIKE));
        ix_ww(VM, l->bbox_set, 0);
        ix_ww(VM, l->bbox_x0, 0);
        ix_ww(VM, l->bbox_x1, 0);
        ix_ww(VM, l->bbox_y0, 0);
        ix_ww(VM, l->bbox_y1, 0);
        ix_step(e, en);
    }
}
