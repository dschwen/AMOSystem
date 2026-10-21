#!/usr/bin/env python3
"""Remove the assembler of AMOSPro.Lib routines that moved to C++.

  move_to_cpp.py --where src/lib/strings.cpp NAME... [-e NAME...]

Each NAME routine (Lib_Par/Lib_Def) in +Lib.s or +ILib.s loses its body
and becomes "Lib_Int NAME", which keeps its number; the code then has to
be linked into AMOSPro (src/lib). Routines given with -e become Lib_Empty:
helpers that nothing calls any more. A body ends at the next Lib_ macro,
not counting the comment lines in front of it.
"""

import argparse
import re
import sys

FILES = ['+Lib.s', '+ILib.s']
MACRO_RE = re.compile(r'^\s+Lib_(Par|Def|Int|Empty|Ext|End|Pos|Ini)\b\s*([^\s;]*)')


def find(lines, name):
    for i, line in enumerate(lines):
        m = MACRO_RE.match(line)
        if m and m.group(1) in ('Par', 'Def') and m.group(2) == name:
            return i
    return None


def end_of(lines, start):
    i = start + 1
    while i < len(lines) and not MACRO_RE.match(lines[i]):
        i += 1
    # leave the comment block in front of the next routine
    while i > start + 1 and (not lines[i - 1].strip() or lines[i - 1].lstrip().startswith(';')):
        i -= 1
    return i


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('names', nargs='*')
    ap.add_argument('-e', '--empty', nargs='*', default=[])
    ap.add_argument('--where', required=True, help='where the C++ is, for the comment')
    ap.add_argument('--src', default='.')
    args = ap.parse_args()
    contents = {}
    for f in FILES:
        with open(f'{args.src}/{f}', encoding='latin-1') as fh:
            contents[f] = fh.read().split('\n')
    todo = [(n, False) for n in args.names] + [(n, True) for n in args.empty]
    for name, empty in todo:
        for f, lines in contents.items():
            i = find(lines, name)
            if i is not None:
                break
        else:
            sys.exit(f'move_to_cpp: no routine {name}')
        j = end_of(lines, i)
        # the separator line after the header goes too
        if empty:
            new = f'\tLib_Empty\t\t\t{name}, now in {args.where}'
        else:
            new = f'\tLib_Int\t{name}\t\t{args.where}'
        lines[i:j] = [new]
    for f, lines in contents.items():
        with open(f'{args.src}/{f}', 'w', encoding='latin-1') as fh:
            fh.write('\n'.join(lines))


if __name__ == '__main__':
    main()
