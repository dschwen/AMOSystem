#!/usr/bin/env python3
"""Replacement for c/Make_Labels.AMOS.

Scans +ILib.s and +Lib.s for the library-definition macros (Lib_Ini,
Lib_Def, Lib_Par, Lib_Int, Lib_Empty, Lib_Ext, Lib_End, Lib_Pos) and
numbers the routines exactly the way the macros in +Equ.s do.

Writes into the output directory:
  +LEqu.s            L_<name>: set <n> for every named routine
  +Internal_Jumps.s  dc.w L_x / dc.l x pairs for every Lib_Int, 0-terminated
  +ILib_Size.s       Lib_Size of the internal library
  +Lib_Size.s        Lib_Size of the whole main library
  +ILib_Functions.s  Lib_Ini/Lib_Pos skeleton of the internal library
  lib_routines.h     the routine numbers for C++ (amos::L_<name>)
"""

import argparse
import os
import re
import sys
import time

MACRO_RE = re.compile(r'^\s+(Lib_(?:Ini|Def|Par|Int|Empty|Ext|End|Pos))\b[ \t]*([^\s;]*)')
# PC-relative branches between library routines, patched by the loader
RBRANCH_RE = re.compile(r'^\S*\s+(Rb(?:ra|sr|eq|ne|cs|cc|lt|ge|ls|hi|le|pl|mi))\s+L_([\w.]+)')
# Last instruction of code that cannot fall through into the next routine
NO_FALL_RE = re.compile(r'^\S*\s+(rts|rte|bra(\.[sbwl])?|jmp|illegal|Rbra|Rjmp|Rjmpt|RjmptR|Ljmp|'
                        r'JJmp|JJmpR|Ijmp|IjmpR|Ret_Int|Ret_Float|Ret_String|Lib_\w+)\b', re.I)
RULE = '; ' + '~' * 67 + '\n'


def header(title, stamp):
    return f'{RULE}; {title} on the {stamp}\n;\n{RULE}'


def label_line(name, value):
    label = f'L_{name}:'
    tabs = max(1, (24 - len(label) + 7) // 8)
    return f'{label}' + '\t' * tabs + f'set\t{value}\n'


def lint(paths, internals):
    """Library code may not branch PC-relative to a Lib_Int routine (it lives
    outside the library, out of reach of a 16-bit branch), and no code may
    fall through into one (a Lib_Int has no body, so the code would run into
    the next routine instead). Returns a list of error messages."""
    errors = []
    for path in paths:
        previous = None         # last instruction line before this one
        with open(path, encoding='latin-1') as f:
            for lineno, line in enumerate(f, 1):
                m = RBRANCH_RE.match(line)
                if m and m.group(2) in internals:
                    errors.append(f'{path}:{lineno}: {m.group(1)} to Lib_Int routine '
                                  f'{m.group(2)}; use Rjsr/Rjmp')
                m = MACRO_RE.match(line)
                if m and m.group(1) == 'Lib_Int' and previous is not None \
                        and not NO_FALL_RE.match(previous[1]):
                    errors.append(f'{path}:{lineno}: code at line {previous[0]} may fall '
                                  f'through into Lib_Int {m.group(2)}')
                code = line.split(';')[0].rstrip()
                if code.strip() and not code.lstrip().startswith('*') and code[:1] in (' ', '\t') \
                        and not re.match(r'^\s+(even|cnop|ifeq|ifne|ifd|ifnd|else|endc)\b', code, re.I):
                    previous = (lineno, code)
    return errors


class Numberer:
    def __init__(self):
        self.count = 0
        self.labels = []        # (name, number)
        self.internals = []     # names of Lib_Int routines

    def scan(self, path):
        with open(path, encoding='latin-1') as f:
            for lineno, line in enumerate(f, 1):
                m = MACRO_RE.match(line)
                if not m:
                    continue
                macro, arg = m.groups()
                if macro == 'Lib_Ini':
                    self.count = int(arg, 0)
                elif macro == 'Lib_Pos':
                    target = int(arg, 0)
                    if target > self.count:
                        self.count = target
                elif macro == 'Lib_Empty':
                    self.count += 1
                elif macro == 'Lib_End':
                    # Defines the end label L<count> used by the size table,
                    # but is not a routine itself
                    pass
                else:
                    if not arg:
                        sys.exit(f'{path}:{lineno}: {macro} without a name')
                    self.labels.append((arg, self.count))
                    if macro == 'Lib_Int':
                        self.internals.append(arg)
                    self.count += 1


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--src', default='.', help='directory holding +ILib.s and +Lib.s')
    ap.add_argument('--out', default='.', help='output directory')
    ap.add_argument('--stamp', help='date stamp for the headers (default: now; '
                    'pass a fixed value for reproducible builds)')
    args = ap.parse_args()
    stamp = args.stamp or time.strftime('%d-%m-%Y %H:%M:%S')

    n = Numberer()
    n.scan(os.path.join(args.src, '+ILib.s'))
    ilib_size = n.count
    ilib_labels = len(n.labels)
    main_rule = '; ' + '~' * 68 + '\n'
    n.scan(os.path.join(args.src, '+Lib.s'))
    lib_size = n.count
    errors = lint([os.path.join(args.src, '+ILib.s'), os.path.join(args.src, '+Lib.s')],
                  set(n.internals))
    if errors:
        sys.exit('\n'.join(errors))

    def write(name, text):
        with open(os.path.join(args.out, name), 'w', encoding='latin-1') as f:
            f.write(text)

    write('+LEqu.s', header('AMOSPro Internal Library functions', stamp)
          + ''.join(label_line(name, num) for name, num in n.labels[:ilib_labels])
          + '; Main library\n' + main_rule
          + ''.join(label_line(name, num) for name, num in n.labels[ilib_labels:]))
    write('+Internal_Jumps.s', header('AMOSPro Internal Jump list', stamp)
          + ''.join(f'\tdc.w\tL_{name}\n\tdc.l\t{name}\n' for name in n.internals)
          + '\tdc.w\t0\n\tdc.l\t0\n')
    write('+ILib_Size.s', header('AMOSPro Internal Library Size', stamp)
          + f'Lib_Size\t\tequ\t {ilib_size}\n')
    write('+Lib_Size.s', header('AMOSPro Main Library Size', stamp)
          + f'Lib_Size\t\tequ\t {lib_size}\n')
    write('+ILib_Functions.s', header('AMOSPro Internal Library Definition', stamp)
          + f'\tLib_Ini\t 0\n\tLib_Pos\t{ilib_size}\n')
    numbers = dict(n.labels)
    write('lib_routines.h', '// Generated by tools/make_labels.py from +ILib.s and +Lib.s\n'
          '#pragma once\n\nnamespace amos {\n\n'
          + ''.join(f'constexpr int L_{name.replace(".", "_")} = {num};\n'
                    for name, num in numbers.items())
          + f'constexpr int Lib_Size = {lib_size};\n\n}} // namespace amos\n')
    print(f'{ilib_labels} internal labels, {len(n.labels)} labels total, '
          f'Lib_Size {lib_size}')


if __name__ == '__main__':
    main()
