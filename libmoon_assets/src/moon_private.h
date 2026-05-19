/*
 * moon_private.h — internal state shared between libmoon_assets source files.
 *
 * Not part of the public API; only included by src/ translation units.
 */

#ifndef MOON_PRIVATE_H
#define MOON_PRIVATE_H

#include "moon_assets.h"

#define NAME_MAX_LEN 64

/* Library-global context (defined in moon_assets.c) */
typedef struct {
    char asset_dir[512];
    int  initialised;
} MoonCtx;

extern MoonCtx g_ctx;

/*
 * cel_decode — decode a raw CEL/OB buffer into a MoonCel.
 * Defined in cel.c; used by both cel.c and ob.c.
 */
MoonCel *cel_decode(const uint8_t *buf, size_t len);

#endif /* MOON_PRIVATE_H */
