#!/usr/bin/env python3
"""Install the built AMOS Pro binaries into an FS-UAE directory hard drive.

Only programs and libraries are replaced. The config files are left alone
because they hold the user's settings. Every replaced file is first copied,
with its .uaem sidecar, to the backup directory (an existing backup is never
overwritten, so the first backup keeps the true originals). The new files
keep the protection bits and comments of the old sidecars.

  install_hd.py --build build --hd "../Minimal Workbench v3.1" \\
                [--amos AMOS_Pro] [--backup ../AMOS_Pro-original-binaries]
"""

import argparse
import os
import sys

import uaem

# built file -> path on the drive; "{amos}" is the AMOS Pro directory
FILES = [
    ('AMOSPro', '{amos}/AMOSPro'),
    ('AMOSPro.Lib', '{amos}/APSystem/AMOSPro.Lib'),
    ('AMOSPro_Music.Lib', '{amos}/APSystem/AMOSPro_Music.Lib'),
    ('AMOSPro_Compact.Lib', '{amos}/APSystem/AMOSPro_Compact.Lib'),
    ('AMOSPro_Request.Lib', '{amos}/APSystem/AMOSPro_Request.Lib'),
    ('AMOSPro_IOPorts.Lib', '{amos}/APSystem/AMOSPro_IOPorts.Lib'),
    # AMOS.library, the editor and the monitor are linked into AMOSPro
]


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--build', required=True)
    ap.add_argument('--hd', required=True)
    ap.add_argument('--amos', default='AMOS_Pro', help='AMOS Pro directory on the drive')
    ap.add_argument('--backup', required=True)
    args = ap.parse_args()

    if not os.path.isdir(os.path.join(args.hd, args.amos)):
        sys.exit(f'no AMOS Pro in {os.path.join(args.hd, args.amos)}')
    for src, dest in FILES:
        rel = dest.format(amos=args.amos)
        target = os.path.join(args.hd, rel)
        if not os.path.exists(target) and src == 'AMOS.library' and rel.startswith(args.amos):
            continue        # only replace an APSystem copy that is already there
        uaem.replace(os.path.join(args.build, src), target,
                     os.path.join(args.backup, os.path.dirname(rel)))
        print(f'installed {rel}')


if __name__ == '__main__':
    main()
