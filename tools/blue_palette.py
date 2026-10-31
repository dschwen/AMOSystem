#!/usr/bin/env python3
"""The editor's configuration with the blue palette:

  blue_palette.py AMOSPro_Editor_Config OUT

The editor's default palette (Ed_Palette, 8 words) is blue instead of teal:
colours 2, 5 and 6 are $037, $5AE and $27C instead of $077, $0DD and $0AA.
A configuration saved by the editor keeps its own palette. The prebuilt
configuration of the distribution is the input; nothing else changes.
"""

import struct
import sys

TEAL = [0x000, 0x06F, 0x077, 0xEEE, 0xF00, 0x0DD, 0x0AA, 0xFF3]
BLUE = [0x000, 0x06F, 0x037, 0xEEE, 0xF00, 0x5AE, 0x27C, 0xFF3]

data = open(sys.argv[1], 'rb').read()
old = struct.pack('>8H', *TEAL)
if data.count(old) != 1:
    sys.exit(f'{sys.argv[1]}: the palette is not there once')
open(sys.argv[2], 'wb').write(data.replace(old, struct.pack('>8H', *BLUE)))
