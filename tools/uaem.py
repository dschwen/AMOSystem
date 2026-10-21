#!/usr/bin/env python3
"""Install files into an FS-UAE directory hard drive, with .uaem sidecars.

FS-UAE keeps the AmigaDOS metadata of every file in a directory drive in a
sidecar file "<name>.uaem" holding one line:

    <protection bits> <yyyy-mm-dd hh:mm:ss.cc> <comment>

e.g. "----rwed 2018-10-09 22:59:38.04 ". The protection bits are hsparwed
(hold, script, pure, archived, read, write, execute, delete); a "-" means the
bit is clear.

  uaem.py install SRC DEST    copy the tree SRC to DEST, writing sidecars
  uaem.py replace SRC DEST [--backup DIR]
                              replace the file DEST by SRC, keeping the
                              protection bits and comment of its sidecar
  uaem.py sidecars DIR        add missing sidecars below DIR
"""

import argparse
import os
import shutil
import sys
import time

DEFAULT_BITS = '----rwed'


def sidecar_text(path, bits=DEFAULT_BITS, comment=''):
    mtime = os.stat(path).st_mtime
    stamp = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(mtime))
    hundredths = int((mtime % 1) * 100)
    return f'{bits} {stamp}.{hundredths:02d} {comment}\n'


def write_sidecar(path, bits=DEFAULT_BITS, comment=''):
    with open(path + '.uaem', 'w', encoding='latin-1') as f:
        f.write(sidecar_text(path, bits, comment))


def add_sidecars(root, overwrite=False):
    """Give every file and directory below root a sidecar (root itself has none)."""
    count = 0
    for dirpath, dirnames, filenames in os.walk(root):
        for name in dirnames + filenames:
            if name.endswith('.uaem'):
                continue
            path = os.path.join(dirpath, name)
            if overwrite or not os.path.exists(path + '.uaem'):
                write_sidecar(path)
                count += 1
    return count


def install(src, dest):
    """Copy src (file or tree) to dest, replacing it, and add sidecars.

    Sidecars that come with src are kept; dest gets a sidecar too."""
    if os.path.isdir(dest) and not os.path.islink(dest):
        shutil.rmtree(dest)
    elif os.path.exists(dest):
        os.remove(dest)
    if os.path.isdir(src):
        shutil.copytree(src, dest, symlinks=False)
        add_sidecars(dest)
    else:
        shutil.copy2(src, dest)
    write_sidecar(dest)


def read_sidecar(path):
    """(bits, comment) from path's sidecar, or the defaults."""
    try:
        with open(path + '.uaem', encoding='latin-1') as f:
            fields = f.readline().rstrip('\n').split(' ', 3)
        return fields[0], fields[3] if len(fields) > 3 else ''
    except (OSError, IndexError):
        return DEFAULT_BITS, ''


def replace(src, dest, backup=None):
    """Replace the file dest by src; its sidecar keeps bits and comment."""
    bits, comment = read_sidecar(dest)
    if backup and os.path.exists(dest):
        os.makedirs(backup, exist_ok=True)
        for p in (dest, dest + '.uaem'):
            if os.path.exists(p) and not os.path.exists(os.path.join(backup, os.path.basename(p))):
                shutil.copy2(p, backup)
    shutil.copyfile(src, dest)
    write_sidecar(dest, bits, comment)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    sub = ap.add_subparsers(dest='cmd', required=True)
    p = sub.add_parser('install')
    p.add_argument('src')
    p.add_argument('dest')
    p = sub.add_parser('replace')
    p.add_argument('src')
    p.add_argument('dest')
    p.add_argument('--backup', help='copy the old file and sidecar here first '
                   '(an existing backup is never overwritten)')
    p = sub.add_parser('sidecars')
    p.add_argument('dir')
    args = ap.parse_args()
    if args.cmd == 'install':
        install(args.src, args.dest)
    elif args.cmd == 'replace':
        replace(args.src, args.dest, args.backup)
    else:
        print(f'{add_sidecars(args.dir)} sidecars written')


if __name__ == '__main__':
    sys.exit(main())
