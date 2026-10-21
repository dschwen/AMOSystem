#!/usr/bin/env python3
"""Mark the chip-memory hunks of an executable linked by m68k-amigaos-ld.

This ld writes no memory flags into the hunk header, so data that must be
in chip memory (AMOS.library's "m" section, gcc's __chip variables) would
be loaded anywhere. The linker scripts in link/ collect such data in the
output sections .datachip and .bsschip. This tool reads the link map to
find which hunks those became (every non-empty output section is one hunk,
in map order) and sets MEMF_CHIP on them, both in the header's size table
and in the hunk's block id, as GenAm did.

  chiphunks.py EXECUTABLE MAPFILE
"""

import re
import struct
import sys

CHIP_SECTIONS = ('.datachip', '.bsschip')
HUNKF_CHIP = 1 << 30
# a top-level output section: name, then address and size (possibly on the
# next line when the name is long)
SECTION_RE = re.compile(r'^([^\s*][^\s]*)\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)', re.M)
SECTION_WRAPPED_RE = re.compile(r'^([^\s*][^\s]*)\n\s+0x([0-9a-fA-F]+)\s+0x([0-9a-fA-F]+)', re.M)


def hunk_sections(mapfile):
    """Names of the non-empty output sections, in hunk order."""
    with open(mapfile, encoding='latin-1') as f:
        text = f.read()
    start = text.find('Linker script and memory map')
    text = text[start:] if start >= 0 else text
    found = [(m.start(), m.group(1), int(m.group(3), 16))
             for r in (SECTION_RE, SECTION_WRAPPED_RE) for m in r.finditer(text)]
    return [name for _, name, size in sorted(found) if size > 0 and not name.startswith('/')]


def main():
    if len(sys.argv) != 3:
        sys.exit(__doc__)
    exe, mapfile = sys.argv[1:]
    sections = hunk_sections(mapfile)
    with open(exe, 'rb') as f:
        data = bytearray(f.read())
    if struct.unpack_from('>L', data, 0)[0] != 0x3F3 or struct.unpack_from('>L', data, 4)[0]:
        sys.exit(f'{exe}: not a hunk executable without resident libraries')
    count, first, last = struct.unpack_from('>LLL', data, 8)
    if count != len(sections):
        sys.exit(f'{exe}: {count} hunks but {len(sections)} sections in {mapfile}: {sections}')
    chip = [i for i, name in enumerate(sections) if name in CHIP_SECTIONS]
    # header size table
    for i in chip:
        off = 20 + 4 * i
        struct.pack_into('>L', data, off, struct.unpack_from('>L', data, off)[0] | HUNKF_CHIP)
    # block ids: walk the hunks
    pos = 20 + 4 * count
    index = 0
    while pos < len(data):
        block = struct.unpack_from('>L', data, pos)[0]
        kind = block & 0x3FFFFFFF
        if kind in (0x3E9, 0x3EA, 0x3EB):
            if index in chip:
                struct.pack_into('>L', data, pos, block | HUNKF_CHIP)
            size = struct.unpack_from('>L', data, pos + 4)[0] & 0x3FFFFFFF
            pos += 8 + (0 if kind == 0x3EB else size * 4)
        elif kind == 0x3EC:
            pos += 4
            while n := struct.unpack_from('>L', data, pos)[0]:
                pos += 8 + 4 * n
            pos += 4
        elif kind in (0x3FC, 0x3F7):
            pos += 4
            while n := struct.unpack_from('>H', data, pos)[0]:
                pos += 4 + 2 * n
            pos += 2
            pos += pos % 4
        elif kind == 0x3F0:
            pos += 4
            while n := struct.unpack_from('>L', data, pos)[0]:
                pos += 4 + 4 * n + 4
            pos += 4
        elif kind == 0x3F1:
            pos += 8 + 4 * struct.unpack_from('>L', data, pos + 4)[0]
        elif kind == 0x3F2:
            pos += 4
            index += 1
        else:
            sys.exit(f'{exe}: unsupported hunk block {kind:#x} at {pos:#x}')
    with open(exe, 'wb') as f:
        f.write(data)
    if chip:
        print(f'{exe}: chip memory for hunk(s) {", ".join(map(str, chip))}')


if __name__ == '__main__':
    main()
