/*
 * piv.c — PIV background bitmap loader for libmoon_assets.
 *
 * The PIV decoder (moon_piv_load_from_buffer) is implemented in
 * packbits_piv.c.  This file provides the file-level load/free wrapper.
 */

#include "moon_private.h"

#include <stdlib.h>

MoonPiv *moon_piv_load(const char *name)
{
    if (!g_ctx.initialised)
        return NULL;

    size_t len;
    uint8_t *buf = moon_file_read(name, &len);
    if (!buf)
        return NULL;

    MoonPiv *piv = moon_piv_load_from_buffer(buf, len);
    free(buf);
    return piv;
}

void moon_piv_free(MoonPiv *piv)
{
    if (!piv)
        return;
    free(piv->bitmap);
    free(piv);
}
