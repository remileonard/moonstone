/*
 * moon-palette — display the colour palette of a PIV background file.
 *
 * Usage: moon-palette <file.PIV> [file.PIV ...]
 *
 * Prints each palette entry as Amiga 12-bit hex and approximated RGB.
 * Uses ANSI escape codes for colour swatches when stdout is a terminal.
 */

#include "moon_assets.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static int use_ansi = 0;

/* Convert Amiga 12-bit $0RGB to 8-bit per channel */
static void amiga_to_rgb(uint16_t c, uint8_t *r, uint8_t *g, uint8_t *b)
{
    uint8_t rv = (uint8_t)((c >> 8) & 0xF);
    uint8_t gv = (uint8_t)((c >> 4) & 0xF);
    uint8_t bv = (uint8_t)( c       & 0xF);
    /* Expand 4-bit → 8-bit: replicate nibble (e.g. 0xA → 0xAA) */
    *r = (uint8_t)((rv << 4) | rv);
    *g = (uint8_t)((gv << 4) | gv);
    *b = (uint8_t)((bv << 4) | bv);
}

static void print_palette(const char *path)
{
    /* Use moon library to load the PIV */
    moon_init(".");  /* init with current dir; path is absolute or relative */

    /* Load raw bytes to avoid path confusion */
    FILE *f = fopen(path, "rb");
    if (!f) {
        fprintf(stderr, "Error: cannot open '%s'\n", path);
        return;
    }
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    rewind(f);
    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf || (long)fread(buf, 1, (size_t)sz, f) != sz) {
        fprintf(stderr, "Error: read failed\n");
        fclose(f);
        free(buf);
        return;
    }
    fclose(f);

    MoonPiv *piv = moon_piv_load_from_buffer(buf, (size_t)sz);
    free(buf);

    if (!piv) {
        fprintf(stderr, "Error: failed to parse '%s' as PIV\n", path);
        return;
    }

    int colours = 1 << piv->planes;
    printf("File    : %s  (%d planes, %d colours, %dx%d)\n",
           path, piv->planes, colours, piv->width, piv->height);
    printf("%-3s  %-6s  %-8s  Swatch\n", "Idx", "Amiga", "RGB");

    for (int i = 0; i < colours && i < 32; i++) {
        uint16_t c = piv->palette[i];
        uint8_t r, g, b;
        amiga_to_rgb(c, &r, &g, &b);

        printf("%3d  #%03X   #%02X%02X%02X", i, (unsigned)c, r, g, b);
        if (use_ansi) {
            /* Print a small colour block using 24-bit ANSI */
            printf("  \033[48;2;%d;%d;%dm   \033[0m", r, g, b);
        }
        printf("\n");
    }
    printf("\n");

    moon_piv_free(piv);
    moon_shutdown();
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: moon-palette <file.PIV> [file.PIV ...]\n");
        return 1;
    }
    use_ansi = isatty(STDOUT_FILENO);
    for (int i = 1; i < argc; i++)
        print_palette(argv[i]);
    return 0;
}
