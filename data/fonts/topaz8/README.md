# topaz 8 (AROS)

The topaz 8 of AROS, a redrawn look-alike of the Amiga's ROM font, as raw
data. The Linux build uses it when it is built without a Kickstart (with
one, the Kickstart's own topaz 8 and 9 are taken from it instead:
`tools/extract_topaz.py`).

Source: `rom/graphics/default_font.c` of AROS
(https://github.com/aros-development-team/AROS, commit
8335ca3e13c6c388fa861e2929a2c45ad22d35f7), Copyright (C) 1995-2026, The AROS
Development Team. All rights reserved. It is under the AROS Public License
1.1 (`LICENSE.AROS`, based on the MPL 1.1); the data is unmodified, only
written out of its C arrays.

The files, as the font's TextFont has them:

| File | Content |
|---|---|
| `chardata.bin` | `tf_CharData`: 8 rows of 192 bytes (`tf_Modulo`), the glyphs side by side, one bit a pixel, the leftmost the high bit |
| `charloc.bin` | `tf_CharLoc`: for characters 32 to 255 and the "unknown" one after them, 225 big-endian longs: the glyph's bit offset in a row (high word) and its width (low word) |

The rest of the TextFont: `tf_YSize` 8, `tf_XSize` 8, `tf_Baseline` 6,
`tf_BoldSmear` 1, `tf_LoChar` 32, `tf_HiChar` 255, `tf_Flags`
`FPF_ROMFONT | FPF_DESIGNED`, no `tf_CharSpace` or `tf_CharKern` (it is
monospaced). Character 164 is the euro sign (ISO 8859-15).
