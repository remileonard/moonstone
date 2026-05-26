#!/usr/bin/env python3
"""
bin2h.py — Convert a binary file to a C header containing a byte array.

Usage:
    python3 bin2h.py <input_file> [output_file]

If output_file is omitted, the header is written to stdout.

The generated header declares:
    static const uint8_t <varname>_data[];
    static const size_t  <varname>_size;

where <varname> is derived from the input filename (lowercase, dots and
hyphens replaced with underscores).

Example — generate test_map.h from the game's "Test" file:
    python3 tools/bin2h.py assets/Test game/include/test_map.h
"""

import os
import sys


def _varname(filename):
    base = os.path.basename(filename)
    name = base.replace(".", "_").replace("-", "_").lower()
    return name


def bin2h(input_path, varname=None, guard=None):
    with open(input_path, "rb") as fh:
        raw = fh.read()

    base = os.path.basename(input_path)
    if varname is None:
        varname = _varname(input_path)
    if guard is None:
        guard = varname.upper() + "_H"

    lines = [
        f"/* Generated from {base} — DO NOT EDIT */",
        f"#ifndef {guard}",
        f"#define {guard}",
        "",
        "#include <stddef.h>",
        "#include <stdint.h>",
        "",
        f"static const uint8_t {varname}_data[] = {{",
    ]

    for i in range(0, len(raw), 16):
        chunk = raw[i : i + 16]
        row = ", ".join(f"0x{b:02x}" for b in chunk)
        comma = "," if i + 16 < len(raw) else ""
        lines.append(f"    {row}{comma}")

    lines += [
        "};",
        "",
        f"static const size_t {varname}_size = {len(raw)};",
        "",
        f"#endif /* {guard} */",
    ]

    return "\n".join(lines) + "\n"


def main():
    if len(sys.argv) < 2:
        print(__doc__, file=sys.stderr)
        sys.exit(1)

    input_path = sys.argv[1]
    output_path = sys.argv[2] if len(sys.argv) >= 3 else None

    result = bin2h(input_path)

    if output_path:
        with open(output_path, "w") as fh:
            fh.write(result)
        print(f"Written {output_path} ({os.path.getsize(input_path)} bytes)")
    else:
        sys.stdout.write(result)


if __name__ == "__main__":
    main()
