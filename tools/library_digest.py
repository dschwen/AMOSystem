#!/usr/bin/env python3
"""Replacement for c/Library_Digest.AMOS.

Numbers the routines of an extension source (Lib_Ini/Lib_Def/Lib_Par/...)
and writes, next to the source or into --out:
  <Source>_Size.s    Lib_Size of the extension
  <Source>_Labels.s  L_<name>: set <n> for every named routine
  <Source>_Jumps.s   for AMOSPro (+B.s Library_Patch): Lib_Size, then the
                     number and the address (Ext_<Name>_<routine>) of every
                     Lib_Int routine, 0-terminated
"""

import argparse
import os
import time

from make_labels import Numberer, header, label_line


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('source', help='extension source, e.g. extensions/+Request.s')
    ap.add_argument('--out', help='output directory (default: next to the source)')
    ap.add_argument('--stamp', help='date stamp for the headers (default: now)')
    args = ap.parse_args()
    stamp = args.stamp or time.strftime('%d-%m-%Y %H:%M:%S')

    out = args.out or os.path.dirname(args.source)
    name = os.path.basename(args.source)
    base = name[:-2] if name.lower().endswith('.s') else name

    n = Numberer()
    n.scan(args.source)

    with open(os.path.join(out, f'{base}_Size.s'), 'w', encoding='latin-1') as f:
        f.write(header(f'{name}, library size', stamp) + f'Lib_Size\t\tequ\t {n.count}\n')
    with open(os.path.join(out, f'{base}_Labels.s'), 'w', encoding='latin-1') as f:
        f.write(header(f'{name}, list of the library functions', stamp)
                + ''.join(label_line(lab, num) for lab, num in n.labels))
    ext = base.lstrip('+')
    numbers = dict(n.labels)
    with open(os.path.join(out, f'{base}_Jumps.s'), 'w', encoding='latin-1') as f:
        f.write(header(f'{name}, routines in AMOSPro', stamp) + f'\tdc.w\t{n.count}\n'
                + ''.join(f'\tdc.w\t{numbers[r]}\n\tdc.l\tExt_{ext}_{r.replace(".", "_")}\n'
                          for r in n.internals)
                + '\tdc.w\t0\n')
    print(f'{name}: {len(n.labels)} labels, Lib_Size {n.count}')


if __name__ == '__main__':
    main()
