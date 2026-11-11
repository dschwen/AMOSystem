#!/usr/bin/env python3
"""The constant tables of AMCAF's Protracker player, read from AMOSPro_AMCAF.Lib at fixed offsets
(its sine, funk and period tables: the library's data, not code) into src/ext/amcaf_pt_tables.h.

  ext_tables.py [--check]
"""
import argparse
import hashlib
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
LIB = ROOT / 'AMOS/APSystem/AMOSPro_AMCAF.Lib'
SHA = '9894a7e216ff501244bbcdc47d3830abdbcf4382d4f4a6daa43551faa87917e4'
OUT = ROOT / 'src/ext/amcaf_pt_tables.h'


def array(name, kind, values, per_line):
    lines = [f'inline constexpr {kind} {name}[{len(values)}] = {{']
    for i in range(0, len(values), per_line):
        lines.append('    ' + ', '.join(str(v) for v in values[i:i + per_line]) + ',')
    lines.append('};')
    return '\n'.join(lines)


def generate():
    b = LIB.read_bytes()
    if hashlib.sha256(b).hexdigest() != SHA:
        sys.exit(f'{LIB}: not AMCAF V1.50beta4')
    word = lambda at: int.from_bytes(b[at:at + 2], 'big')
    periods = [word(0x974C + 2 * i) for i in range(16 * 36 + 40)]
    sine = list(b[0x972C:0x974C])
    funk = list(b[0x971C:0x972C])
    return ('// The constant tables of AMCAF\'s Protracker player: written by tools/ext_tables.py from the library\n'
            '// (file offsets $972c, $971c and $974c). Do not edit.\n#pragma once\n\n#include <stdint.h>\n\n'
            'namespace amos::amcaf::pt {\n\n'
            '// The vibrato and tremolo waveform (one period, 32 values)\n' + array('SineTable', 'uint8_t', sine, 16) + '\n\n'
            '// The funk repeat\'s speeds (EFx)\n' + array('FunkTable', 'uint8_t', funk, 16) + '\n\n'
            '// The periods of the 36 notes of each finetune (0 to 7, then -8 to -1), a row of 36 words; one\n'
            '// search runs on over the end of a row into the next (and past the last: 40 words more)\n' + array('Periods', 'uint16_t', periods, 12) + '\n\n'
            '} // namespace amos::amcaf::pt\n')


if __name__ == '__main__':
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--check', action='store_true')
    args = ap.parse_args()
    text = generate()
    if args.check:
        sys.exit(0 if OUT.exists() and OUT.read_text() == text else 'stale ' + str(OUT))
    OUT.write_text(text)
