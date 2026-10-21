#!/usr/bin/env python3
"""Replacement for c/Make_Toktable.AMOS.

Every token-table entry in +Lib.s (between the TOKEN_START and TOKEN_END
comment lines) carries a comment of the form //$xx$xx$xx$xx giving the
verifier's quick-test data for that token. The four bytes of each entry are
written in order to +Toktab_Verif.Bin, which +Lib.s includes as the "KwiK"
block.
"""

import argparse
import re
import sys

ENTRY_RE = re.compile(r'//\$([0-9A-Fa-f]{2})\$([0-9A-Fa-f]{2})\$([0-9A-Fa-f]{2})\$([0-9A-Fa-f]{2})')


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('lib', help='path to +Lib.s')
    ap.add_argument('out', help='path of the +Toktab_Verif.Bin to write')
    args = ap.parse_args()

    data = bytearray()
    inside = False
    with open(args.lib, encoding='latin-1') as f:
        for lineno, line in enumerate(f, 1):
            if 'TOKEN_START' in line:
                inside = True
            elif 'TOKEN_END' in line:
                break
            elif inside and '//' in line:
                m = ENTRY_RE.search(line)
                if not m:
                    sys.exit(f'{args.lib}:{lineno}: malformed token comment')
                data += bytes(int(b, 16) for b in m.groups())
    if not inside:
        sys.exit(f'{args.lib}: no TOKEN_START marker')
    with open(args.out, 'wb') as f:
        f.write(data)
    print(f'{len(data) // 4} tokens')


if __name__ == '__main__':
    main()
