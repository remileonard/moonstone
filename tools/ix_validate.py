"""
ix_validate.py — IMAGEXCEL bytecode validator for Moonstone animation scripts.

Implements Pass 2 of the script extraction pipeline described in
docs/DOC_MOTEUR_IMAGEXCEL.md and the tooling plan.

Script format (LAB_01F1 in program.asm):
  - A script is a sequence of one or more steps.
  - Each step contains one or more instructions followed by a step-end marker.
  - Instructions are either:
      DRAW  : byte0 = slot<<2  (bit7=0, byte%4==0), then 5 payload bytes → 6 bytes total
      CTRL  : byte0 >= 0x80, size depends on opcode (see ix_ctrl_op_size)
  - Step-end marker:  FF 00  (NEXT), FF FE (LOOP), FF FF (SCRIPT_END)
  - A complete script ends with FF FF.

Reference: program.asm LAB_0288 dispatch table (sizes from ADDI.L advances).
"""

from __future__ import annotations


# ---------------------------------------------------------------------------
# Control-opcode sizes  (opcode byte → total bytes including the opcode byte)
# ---------------------------------------------------------------------------

_CTRL_SIZE: dict[int, int] = {
    0x80: 2,   # LAB_0215 SET_DIRECTION
    0x84: 6,   # LAB_0218 JUMP_VARIANT  (opcode + 1 + 4-byte address)
    0x88: 2,   # LAB_021A SET_SPEED
    0x8C: 8,   # LAB_021E SKIP8
    0x90: 2,   # (no handler)  safe default
    0x94: 2,   # LAB_021F SET_LOOP_COUNT
    0x98: 2,   # LAB_0220 NOP
    0x9C: 2,   # LAB_0220 NOP
    0xA0: 8,   # LAB_0222 MOVE_DELTA
    0xA4: 4,   # LAB_0221 SKIP4
    0xA8: 2,   # LAB_0241 NOP
    0xAC: 2,   # LAB_022D NOP
    0xB0: 2,   # LAB_022C NOP
    0xB4: 6,   # LAB_022F/0231 CALL  (opcode + 1 + 4-byte address)
    0xB8: 6,   # LAB_0233 SKIP6
    0xBC: 6,   # LAB_0234 SKIP6
    0xC0: 2,   # LAB_0235 KILL
    0xC4: 2,   # LAB_0236 SET_ASSET_TABLE
    0xC8: 6,   # LAB_0232 SKIP6
    0xCC: 8,   # LAB_0237
    0xD0: 8,   # LAB_023B COND_JUMP
    0xD4: 8,   # LAB_023F CLEAR_SCRATCH
}


def ix_ctrl_op_size(op: int) -> int:
    """Return the byte size (including the opcode byte) of a control opcode."""
    # Align to 4-byte boundary for the dispatch table lookup
    key = op & 0xFC
    return _CTRL_SIZE.get(key, 2)


# ---------------------------------------------------------------------------
# Validator
# ---------------------------------------------------------------------------

def is_valid_ix_script(data: bytes | bytearray) -> bool:
    """Return True if *data* is a well-formed IMAGEXCEL animation script.

    Criteria (from the plan):
    • Each group starts with a DRAW opcode: (byte & 0x80 == 0) and (byte % 4 == 0),
      followed by 5 payload bytes.
    • A step ends with 0xFF + flow byte: 0x00=NEXT, 0xFE=LOOP, 0xFF=SCRIPT_END.
    • The whole script ends with 0xFF 0xFF.
    • The script must contain at least one valid step with at least one DRAW instruction.
    • At least one DRAW instruction must have non-trivial coordinates
      (|x| >= 4 or y != 0) to exclude timing/counter tables whose DC.L values
      happen to satisfy the structural rules but encode no meaningful animation.
    """
    n = len(data)
    if n < 2:
        return False

    pos = 0
    found_draw = False          # at least one DRAW seen in current step
    has_nontrivial_draw = False  # at least one DRAW with meaningful coordinates

    while pos < n:
        b = data[pos]

        # ---- step-end marker -----------------------------------------------
        if b == 0xFF:
            if pos + 1 >= n:
                return False
            flow = data[pos + 1]
            if flow == 0xFF:
                if not found_draw:
                    # FF FF without any prior draw is invalid
                    return False
                if not has_nontrivial_draw:
                    # All draws had trivial coords — almost certainly a data table
                    return False
                return True
            elif flow in (0x00, 0xFE):
                if not found_draw:
                    # A step with no DRAW instructions is invalid
                    return False
                pos += 2
                found_draw = False  # reset for next step
            else:
                # Unknown flow byte
                return False

        # ---- DRAW instruction (bit7 clear, divisible by 4) -----------------
        elif (b & 0x80) == 0:
            if (b % 4) != 0:
                return False            # not a valid slot encoding
            if pos + 6 > n:
                return False            # truncated draw instruction
            y_raw = data[pos + 2]
            y_s   = y_raw if y_raw < 128 else y_raw - 256
            x_raw = (data[pos + 4] << 8) | data[pos + 5]
            x_s   = x_raw if x_raw < 32768 else x_raw - 65536
            if abs(x_s) >= 4 or y_s != 0:
                has_nontrivial_draw = True
            pos += 6
            found_draw = True

        # ---- control opcode (bit7 set) -------------------------------------
        else:
            size = ix_ctrl_op_size(b)
            if pos + size > n:
                return False
            pos += size

    return False   # no FF FF terminator found


# ---------------------------------------------------------------------------
# Decode helpers for comment generation
# ---------------------------------------------------------------------------

def decode_draw_comment(data: bytes | bytearray, pos: int) -> str:
    """Return a brief comment for the DRAW instruction at *pos*.

    Format:  slot=S  frame=F  y=Y  x=X
    """
    if pos + 6 > len(data):
        return ""
    op    = data[pos]
    frame = data[pos + 1]
    y_raw = data[pos + 2]
    # y_delta is signed byte
    y_s   = y_raw if y_raw < 128 else y_raw - 256
    x_raw = (data[pos + 4] << 8) | data[pos + 5]
    x_s   = x_raw if x_raw < 32768 else x_raw - 65536
    slot  = op >> 2
    return f"slot={slot} frame={frame} y={y_s:+d} x={x_s:+d}"
