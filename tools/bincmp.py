#!/usr/bin/env python3
"""Compare two AmigaDOS hunk executables, ignoring equivalent encodings.

GenAm and vasm pick different, but equivalent, encodings for a few
instructions:
  - `and/or/add/sub/cmp #imm,Dn`: GenAm emits the immediate form
    (ANDI, ORI, ...), vasm the <ea>,Dn form with an immediate source;
  - `exg Rx,Ry`: the two assemblers order the registers differently.
Both encodings have the same length, so hunk contents are compared word by
word and any differing word that is one of these equivalent pairs is
accepted.

At the hunk level, memory flags (chip/fast) are compared once per hunk
whether they sit in the header table or in the block id, padding at the end
of a hunk is ignored (vasm pads code with NOP, GenAm with 0), and relocation
tables are compared as sets.

Exit status is 0 when the files are equivalent.
"""

import argparse
import struct
import sys

HUNK_HEADER = 0x3F3
HUNK_CODE, HUNK_DATA, HUNK_BSS = 0x3E9, 0x3EA, 0x3EB
HUNK_RELOC32, HUNK_RELOC32SHORT, HUNK_DREL32 = 0x3EC, 0x3FC, 0x3F7
HUNK_SYMBOL, HUNK_DEBUG, HUNK_END = 0x3F0, 0x3F1, 0x3F2

# Immediate-to-data-register forms: (immediate opcode base, <ea>,Dn base)
IMM_FORMS = {
    'or': (0x0000, 0x8000),
    'and': (0x0200, 0xC000),
    'sub': (0x0400, 0x9000),
    'add': (0x0600, 0xD000),
    'cmp': (0x0C00, 0xB000),
}


def canonical(word):
    """Return a canonical description of an instruction word, or None."""
    for name, (imm_base, ea_base) in IMM_FORMS.items():
        # xxxI.<size> #imm,Dn : base | size<<6 | 000<<3 | reg
        if word & 0xFF00 == imm_base and word & 0x38 == 0 and (word >> 6) & 3 != 3:
            return (name, (word >> 6) & 3, word & 7)
        # xxx.<size> #imm,Dn : base | reg<<9 | size<<6 | 111100
        if word & 0xF000 == ea_base and word & 0x3F == 0x3C and word & 0x100 == 0 \
                and (word >> 6) & 3 != 3:
            return (name, (word >> 6) & 3, (word >> 9) & 7)
    # EXG Rx,Ry : 1100 xxx1 mmmm myyy with opmode 01000, 01001 or 10001
    if word & 0xF100 == 0xC100:
        opmode = (word >> 3) & 0x1F
        rx, ry = (word >> 9) & 7, word & 7
        if opmode == 0x08:
            return ('exg', frozenset((('d', rx), ('d', ry))))
        if opmode == 0x09:
            return ('exg', frozenset((('a', rx), ('a', ry))))
        if opmode == 0x11:
            return ('exg', frozenset((('d', rx), ('a', ry))))
    return None


class Hunk:
    def __init__(self, kind, flags, data, size):
        self.kind = kind
        self.flags = flags
        self.data = data            # bytes (empty for BSS)
        self.size = size            # in bytes
        self.relocs = set()         # (target hunk, offset)


def parse(path):
    with open(path, 'rb') as f:
        raw = f.read()
    pos = 0

    def long():
        nonlocal pos
        value, = struct.unpack_from('>L', raw, pos)
        pos += 4
        return value

    def word():
        nonlocal pos
        value, = struct.unpack_from('>H', raw, pos)
        pos += 2
        return value

    if long() != HUNK_HEADER:
        sys.exit(f'{path}: not a hunk executable')
    while long():           # resident library names, normally none
        pass
    count = long()
    first, last = long(), long()
    header_flags = [long() >> 30 for _ in range(last - first + 1)]
    hunks = []
    current = None
    while pos < len(raw):
        block = long()
        kind = block & 0x3FFFFFFF
        if kind in (HUNK_CODE, HUNK_DATA, HUNK_BSS):
            size = (long() & 0x3FFFFFFF) * 4
            flags = (block >> 30) or header_flags[len(hunks)]
            data = b'' if kind == HUNK_BSS else raw[pos:pos + size]
            if kind != HUNK_BSS:
                pos += size
            current = Hunk(kind, flags, data, size)
            hunks.append(current)
        elif kind == HUNK_RELOC32:
            while n := long():
                target = long()
                for _ in range(n):
                    current.relocs.add((target, long()))
        elif kind in (HUNK_RELOC32SHORT, HUNK_DREL32):
            while n := word():
                target = word()
                for _ in range(n):
                    current.relocs.add((target, word()))
            if pos % 4:
                pos += 2
        elif kind == HUNK_SYMBOL:
            while n := long():
                pos += n * 4 + 4
        elif kind == HUNK_DEBUG:
            pos += long() * 4
        elif kind == HUNK_END:
            current = None
        else:
            sys.exit(f'{path}: unsupported hunk block {kind:#x} at {pos - 4:#x}')
    if len(hunks) != count:
        sys.exit(f'{path}: header announces {count} hunks, found {len(hunks)}')
    return hunks


def compare_data(ref, cand):
    """Word-by-word comparison; returns (equivalent count, [(offset, a, b)])."""
    equivalent = 0
    diffs = []
    for off in range(0, min(len(ref), len(cand)) - 1, 2):
        a = int.from_bytes(ref[off:off + 2], 'big')
        b = int.from_bytes(cand[off:off + 2], 'big')
        if a == b:
            continue
        ca = canonical(a)
        if ca is not None and ca == canonical(b):
            equivalent += 1
        elif a == 0 and b == 0x4E71 and off + 2 >= len(ref) - 2:
            equivalent += 1     # alignment padding at the end of the hunk
        else:
            diffs.append((off, a, b))
    return equivalent, diffs


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('reference')
    ap.add_argument('candidate')
    ap.add_argument('--max', type=int, default=20, help='differences to print')
    args = ap.parse_args()

    ref = parse(args.reference)
    cand = parse(args.candidate)
    problems = []
    equivalent = 0
    if len(ref) != len(cand):
        problems.append(f'{len(ref)} hunks vs {len(cand)}')
    for i, (r, c) in enumerate(zip(ref, cand)):
        if (r.kind, r.flags, r.size) != (c.kind, c.flags, c.size):
            problems.append(f'hunk {i}: type/flags/size {r.kind:#x}/{r.flags}/{r.size} '
                            f'vs {c.kind:#x}/{c.flags}/{c.size}')
            continue
        eq, diffs = compare_data(r.data, c.data)
        equivalent += eq
        problems += [f'hunk {i} offset {off:06x}: {a:04x} | {b:04x}' for off, a, b in diffs]
        for target, off in sorted(r.relocs ^ c.relocs):
            side = 'reference' if (target, off) in r.relocs else 'candidate'
            problems.append(f'hunk {i}: reloc to hunk {target} at {off:06x} only in {side}')

    print(f'{args.candidate}: {len(ref)} hunks, {equivalent} equivalent encodings, '
          f'{len(problems)} real differences')
    for p in problems[:args.max]:
        print('  ' + p)
    sys.exit(1 if problems else 0)


if __name__ == '__main__':
    main()
