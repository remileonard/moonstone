/*
 * moon_assets.c — library lifecycle and file I/O.
 *
 * Implements moon_init, moon_shutdown, and moon_file_read.
 * Format-specific loaders are in their own files:
 *   cel.c    — CEL sprite sheets
 *   piv.c    — PIV background bitmaps (decoder in packbits_piv.c)
 *   stile.c  — STILE tile maps
 *   mod.c    — MOD / CMP ProTracker modules
 *   ob.c     — OB character sprite sheets
 */

#include "moon_private.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Global library context (shared with all format loaders)            */
/* ------------------------------------------------------------------ */

MoonCtx g_ctx;

/* ------------------------------------------------------------------ */
/* Library lifecycle                                                   */
/* ------------------------------------------------------------------ */

int moon_init(const char *asset_dir)
{
    if (!asset_dir)
        return -1;
    memset(&g_ctx, 0, sizeof(g_ctx));
    strncpy(g_ctx.asset_dir, asset_dir, sizeof(g_ctx.asset_dir) - 1);
    /* Remove trailing slash if present */
    size_t len = strlen(g_ctx.asset_dir);
    if (len > 0 && (g_ctx.asset_dir[len - 1] == '/' ||
                    g_ctx.asset_dir[len - 1] == '\\'))
        g_ctx.asset_dir[len - 1] = '\0';
    g_ctx.initialised = 1;
    return 0;
}

void moon_shutdown(void)
{
    memset(&g_ctx, 0, sizeof(g_ctx));
}

/* ------------------------------------------------------------------ */
/* File I/O helper                                                     */
/* ------------------------------------------------------------------ */

uint8_t *moon_file_read(const char *name, size_t *out_size)
{
    char path[768];
    if (g_ctx.asset_dir[0]) {
        snprintf(path, sizeof(path), "%s/%s", g_ctx.asset_dir, name);
    } else {
        strncpy(path, name, sizeof(path) - 1);
    }

    FILE *f = fopen(path, "rb");
    if (!f) {
        /* Try lower-case name */
        char lower[NAME_MAX_LEN];
        strncpy(lower, name, NAME_MAX_LEN - 1);
        for (char *p = lower; *p; p++)
            *p = (char)tolower((unsigned char)*p);
        if (g_ctx.asset_dir[0])
            snprintf(path, sizeof(path), "%s/%s", g_ctx.asset_dir, lower);
        else
            strncpy(path, lower, sizeof(path) - 1);
        f = fopen(path, "rb");
    }
    if (!f)
        return NULL;

    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    if (sz <= 0) {
        fclose(f);
        return NULL;
    }

    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) {
        fclose(f);
        return NULL;
    }
    if ((long)fread(buf, 1, (size_t)sz, f) != sz) {
        free(buf);
        fclose(f);
        return NULL;
    }
    fclose(f);

    if (out_size)
        *out_size = (size_t)sz;
    return buf;
}
