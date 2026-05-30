#!/usr/bin/env python3
"""
extract_scripts.py — 3-pass IMAGEXCEL animation script extractor for Moonstone.

Reads program.asm and mog.asm, identifies every valid IMAGEXCEL animation
script, resolves symbolic names via pointer tables, and generates
tools/moon_anim_<name>.c for each script.

Usage:
    python3 tools/extract_scripts.py [--asm-dir amiga_asm] [--out-dir tools]

Output:
    tools/moon_anim_<name>.c   — one file per validated script, containing
                                  a static const uint8_t <name>_script[] array
                                  with IX_DRAW / IX_STEP_NEXT / IX_STEP_SCRIPT_END
                                  macros and decoded per-instruction comments.

Pass overview
─────────────
 1. Parse — read all DC.L / DC.W lines under each label, building a byte
            block per label.  DC.L LAB_YYYY lines either start a pointer-
            table block or provide an embedded address argument.

 2. Validate — run each byte block through the IMAGEXCEL bytecode validator
               (ix_validate.is_valid_ix_script).  Only valid blocks proceed.

 3. Name-resolve — scan all pointer-table blocks to build a reverse map
                   label → [tables that reference it] and assign a human-
                   readable C identifier.
"""

from __future__ import annotations

import argparse
import os
import re
import struct
import textwrap
from collections import defaultdict
from typing import Optional

from ix_validate import is_valid_ix_script, decode_draw_comment, ix_ctrl_op_size


# ---------------------------------------------------------------------------
# Known symbolic names (manually seeded from program.asm / mog.asm analysis)
# Labels whose names we know from the pointer-table cross-reference.
# ---------------------------------------------------------------------------

_KNOWN_NAMES: dict[str, str] = {
    # program.asm — intro / opening sequences
    "LAB_00D7": "kn_walk",   # knight walking  (LAB_0025 dispatch)
    "LAB_00D8": "dw1_walk",  # druid walking   (LAB_0024 dispatch)
}


# ---------------------------------------------------------------------------
# Pass 1 — Parse ASM files into raw byte blocks
# ---------------------------------------------------------------------------

# Match a label definition line: LAB_XXXX:
RE_LABEL   = re.compile(r'^(LAB_[0-9A-Fa-f]+):')
# Match DC.L with one or more hex values: DC.L $HHHHHHHH[,$HHHHHHHH,...]
RE_DCL_HEX = re.compile(r'^\s+DC\.L\s+((?:\$[0-9A-Fa-f]{1,8},?\s*)+)', re.IGNORECASE)
# Match DC.W with one or more hex values
RE_DCW_HEX = re.compile(r'^\s+DC\.W\s+((?:\$[0-9A-Fa-f]{1,4},?\s*)+)', re.IGNORECASE)
# Match DC.L LAB_YYYY (label reference)
RE_DCL_LAB = re.compile(r'^\s+DC\.L\s+(LAB_[0-9A-Fa-f]+)', re.IGNORECASE)
# Match DC.W LAB_YYYY (label reference — rare but possible)
RE_DCW_LAB = re.compile(r'^\s+DC\.W\s+(LAB_[0-9A-Fa-f]+)', re.IGNORECASE)


class Block:
    """Byte block accumulated from DC.L / DC.W lines under a label."""

    __slots__ = ("label", "source", "lineno", "data", "is_ptr_table", "ptr_targets")

    def __init__(self, label: str, source: str, lineno: int) -> None:
        self.label: str          = label
        self.source: str         = source   # filename (basename)
        self.lineno: int         = lineno   # line of the label definition
        self.data: bytearray     = bytearray()
        self.is_ptr_table: bool  = False    # True if block is ONLY label refs
        self.ptr_targets: list[str] = []    # label refs in order (ptr table)


def parse_asm(path: str) -> list[Block]:
    """Parse one ASM file and return all data blocks."""
    source = os.path.basename(path)
    blocks: list[Block] = []
    current: Optional[Block] = None

    with open(path, "r", encoding="latin-1") as fh:
        for lineno, raw_line in enumerate(fh, 1):
            line = raw_line.rstrip()

            # ---- new label ------------------------------------------------
            m = RE_LABEL.match(line)
            if m:
                current = Block(m.group(1), source, lineno)
                blocks.append(current)
                continue

            if current is None:
                continue

            # ---- DC.L hex values -----------------------------------------
            m = RE_DCL_HEX.match(line)
            if m:
                # If the block started with only label refs, hex data makes
                # this NOT a pure pointer table — but the plan says to stop the
                # block at a label ref.  We'll just accumulate data.
                for tok in re.findall(r'\$([0-9A-Fa-f]{1,8})', m.group(1)):
                    val = int(tok, 16)
                    current.data += struct.pack(">I", val)
                continue

            # ---- DC.W hex values -----------------------------------------
            m = RE_DCW_HEX.match(line)
            if m:
                for tok in re.findall(r'\$([0-9A-Fa-f]{1,4})', m.group(1)):
                    val = int(tok, 16)
                    current.data += struct.pack(">H", val)
                continue

            # ---- DC.L LAB_YYYY -------------------------------------------
            m = RE_DCL_LAB.match(line)
            if m:
                target = m.group(1).upper()
                if len(current.data) == 0:
                    # Block has no hex data yet → pointer table entry
                    current.is_ptr_table = True
                    current.ptr_targets.append(target)
                else:
                    # Embedded address argument (e.g. B4 CALL / 84 JUMP target)
                    # Substitute 4 zero bytes so the validator sees the right size
                    current.data += b'\x00\x00\x00\x00'
                continue

            # ---- DC.W LAB_YYYY -------------------------------------------
            m = RE_DCW_LAB.match(line)
            if m:
                target = m.group(1).upper()
                if len(current.data) == 0:
                    current.is_ptr_table = True
                    current.ptr_targets.append(target)
                else:
                    current.data += b'\x00\x00'
                continue

            # ---- Any other non-DC line — stop accumulating for this block -----
            # Animation scripts are contiguous DC.L/DC.W regions.  Mixed code
            # labels with scattered data words would produce false positives in
            # the validator, so we stop here.  The block already collected
            # (possibly empty) stays in the list; it will be validated later.
            current = None

    return blocks


# ---------------------------------------------------------------------------
# Pass 2 — Validate blocks as IMAGEXCEL scripts
# ---------------------------------------------------------------------------

def validate_blocks(blocks: list[Block]) -> list[Block]:
    """Return only those blocks that are valid IMAGEXCEL animation scripts."""
    valid: list[Block] = []
    for b in blocks:
        if b.is_ptr_table:
            continue
        if len(b.data) < 8:
            continue
        if is_valid_ix_script(b.data):
            valid.append(b)
    return valid


# ---------------------------------------------------------------------------
# Pass 3 — Name resolution
# ---------------------------------------------------------------------------

def build_name_map(
    all_blocks: list[Block],
    valid_labels: set[str],
) -> dict[str, str]:
    """Return a map  label → C identifier  for all valid script labels.

    Strategy:
    1. Use _KNOWN_NAMES if available.
    2. Build a reverse map: script_label → pointer tables that reference it.
       Use the pointer-table label as a hint for the name.
    3. Fall back to a sanitised version of the label itself.
    """
    # Build reverse map: script_label → [ptr_table_label, ...]
    rev: dict[str, list[str]] = defaultdict(list)
    for b in all_blocks:
        if b.is_ptr_table:
            for tgt in b.ptr_targets:
                if tgt in valid_labels:
                    rev[tgt].append(b.label)

    name_map: dict[str, str] = {}
    used: set[str] = set()

    for lbl in sorted(valid_labels):
        if lbl in _KNOWN_NAMES:
            cname = _KNOWN_NAMES[lbl]
        else:
            # Try to derive a name from the pointer-table context
            ptr_tables = rev.get(lbl, [])
            if ptr_tables:
                # Use the first pointer-table label, lower-cased and stripped
                hint = ptr_tables[0].lower().replace("lab_", "anim_")
                cname = hint
            else:
                # Last resort: use the label itself
                cname = lbl.lower().replace("lab_", "lab")

        # Make unique by appending a counter if collision
        base = cname
        counter = 2
        while cname in used:
            cname = f"{base}_{counter}"
            counter += 1
        used.add(cname)
        name_map[lbl] = cname

    return name_map


# ---------------------------------------------------------------------------
# C code generation helpers
# ---------------------------------------------------------------------------

# Flow byte → macro suffix
_FLOW_MACRO = {
    0x00: "IX_STEP_NEXT",
    0xFE: "IX_STEP_LOOP",
    0xFF: "IX_STEP_SCRIPT_END",
}


def disassemble_script(data: bytes | bytearray) -> list[str]:
    """Disassemble IMAGEXCEL bytecode to a list of C initialiser tokens.

    Returns a list of strings suitable for joining with commas + newlines
    to form the body of a uint8_t array.  Each DRAW instruction is followed
    by a comment; step separators are on their own line.
    """
    tokens: list[str] = []
    n = len(data)
    pos = 0
    step = 1

    while pos < n:
        b = data[pos]

        # ---- step-end marker -------------------------------------------
        if b == 0xFF:
            if pos + 1 >= n:
                break
            flow = data[pos + 1]
            macro = _FLOW_MACRO.get(flow, f"0xFF, 0x{flow:02X}")
            if flow in (0x00, 0xFE, 0xFF):
                tokens.append(f"    {macro},  /* end of step {step} */\n")
            else:
                tokens.append(f"    0xFF, 0x{flow:02X},\n")
            pos += 2
            if flow == 0xFF:
                break
            step += 1

        # ---- DRAW instruction ------------------------------------------
        elif (b & 0x80) == 0 and (b % 4) == 0:
            if pos + 6 > n:
                break
            slot  = b >> 2
            frame = data[pos + 1]
            y_raw = data[pos + 2]
            flags = data[pos + 3]
            x_raw = (data[pos + 4] << 8) | data[pos + 5]
            y_s   = y_raw if y_raw < 128 else y_raw - 256
            x_s   = x_raw if x_raw < 32768 else x_raw - 65536
            b3    = data[pos + 3]
            b4    = data[pos + 4]
            b5    = data[pos + 5]
            tokens.append(
                f"    IX_DRAW({slot}), 0x{frame:02X},"
                f" 0x{y_raw:02X}, 0x{b3:02X},"
                f" 0x{b4:02X}, 0x{b5:02X},"
                f"  /* frame={frame} y={y_s:+d} x={x_s:+d} */\n"
            )
            pos += 6

        # ---- control opcode --------------------------------------------
        else:
            size = ix_ctrl_op_size(b)
            chunk = data[pos:pos + size]
            hex_vals = ", ".join(f"0x{v:02X}" for v in chunk)
            tokens.append(f"    {hex_vals},\n")
            pos += size

    return tokens


def generate_c_file(
    name: str,
    label: str,
    source: str,
    lineno: int,
    data: bytes | bytearray,
) -> str:
    """Return the full text of a moon_anim_<name>.c file."""

    tokens = disassemble_script(data)
    body = "".join(tokens)

    # Count steps
    n_steps = body.count("end of step")

    # Count bytes
    n_bytes = len(data)

    header = textwrap.dedent(f"""\
        /*
         * moon_anim_{name}.c — IMAGEXCEL animation script "{name}".
         *
         * Auto-generated by tools/extract_scripts.py from {source}:{lineno}
         * ({label}).  {n_bytes} bytes, {n_steps} step(s).
         *
         * Macros: IX_DRAW(slot), IX_STEP_NEXT, IX_STEP_LOOP, IX_STEP_SCRIPT_END
         * are defined in tools/imagexcel.h.
         *
         * DO NOT EDIT — re-run tools/extract_scripts.py to regenerate.
         */

        #include <stdint.h>
        #include "imagexcel.h"

        """)

    array = (
        f"/* {label} — {n_bytes} bytes, {n_steps} step(s) */\n"
        f"static const uint8_t {name}_script[] = {{\n"
        + body
        + f"}}; /* {name}_script */\n"
    )

    return header + array


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> None:
    parser = argparse.ArgumentParser(
        description="Extract and generate IMAGEXCEL animation scripts from Moonstone ASM.",
    )
    parser.add_argument(
        "--asm-dir", default="amiga_asm",
        help="Directory containing program.asm and mog.asm (default: amiga_asm)",
    )
    parser.add_argument(
        "--out-dir", default="tools",
        help="Directory for generated moon_anim_*.c files (default: tools)",
    )
    parser.add_argument(
        "--list", action="store_true",
        help="List found scripts without generating files",
    )
    args = parser.parse_args()

    asm_dir = args.asm_dir
    out_dir = args.out_dir

    # ---- Pass 1: parse both ASM files ------------------------------------
    all_blocks: list[Block] = []
    for fname in ("program.asm", "mog.asm"):
        path = os.path.join(asm_dir, fname)
        if not os.path.exists(path):
            print(f"Warning: {path} not found, skipping.")
            continue
        blocks = parse_asm(path)
        all_blocks.extend(blocks)
        n_ptr = sum(1 for b in blocks if b.is_ptr_table)
        n_dat = sum(1 for b in blocks if not b.is_ptr_table and len(b.data) > 0)
        print(f"  {fname}: {len(blocks)} labels → {n_ptr} pointer tables, {n_dat} data blocks")

    # ---- Pass 2: validate -----------------------------------------------
    valid_blocks = validate_blocks(all_blocks)
    valid_labels = {b.label for b in valid_blocks}
    print(f"\nPass 2: {len(valid_blocks)} valid IMAGEXCEL scripts found\n")

    # ---- Pass 3: name resolution ----------------------------------------
    name_map = build_name_map(all_blocks, valid_labels)

    # ---- Report & generate -----------------------------------------------
    os.makedirs(out_dir, exist_ok=True)

    for blk in valid_blocks:
        name  = name_map[blk.label]
        fname = f"moon_anim_{name}.c"
        fpath = os.path.join(out_dir, fname)

        print(
            f"  {blk.label:12s}  {blk.source}:{blk.lineno:5d}"
            f"  {len(blk.data):5d} bytes  →  {fname}"
        )

        if args.list:
            continue

        content = generate_c_file(
            name   = name,
            label  = blk.label,
            source = blk.source,
            lineno = blk.lineno,
            data   = blk.data,
        )

        # Never overwrite existing hand-written files.  The known-names set
        # maps labels that already have a carefully crafted dw1.c file, etc.
        # For those, we skip generation so as not to clobber them.
        if os.path.exists(fpath):
            # Check for the auto-generated marker
            with open(fpath) as fh:
                first_lines = fh.read(512)
            if "DO NOT EDIT" in first_lines or "Auto-generated" in first_lines:
                pass   # safe to overwrite auto-generated files
            else:
                print(f"    ↳ skipping {fpath} (hand-written file already exists)")
                continue

        with open(fpath, "w", encoding="utf-8") as fh:
            fh.write(content)


if __name__ == "__main__":
    main()
