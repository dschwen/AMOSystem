#!/usr/bin/env python3
"""Fetch the NDK's C headers for the host build, from where the Amiga
cross toolchain gets them.

  fetch_ndk.py [--out build/ndk] [--cache build/download]

The host build (make host) compiles the Amiga's C++, which includes the NDK's
headers (46 of them): exec, graphics, intuition, dos, devices... They are
Hyperion's NDK 3.2, whose licence is not stated (the toolchain's
COPYING-THIRD-PARTY.md says "unknown"): so they are not in this repository.
This downloads NDK3.2.lha from Aminet (the archive and the sha256 the
toolchain https://github.com/AmigaPorts/m68k-amigaos-gcc pins in its
default-archives), checks the hash, and extracts Include_H with tools/lha.py
(no LHA tool needed). With the Amiga toolchain installed, its own copy
(AMIGA_PREFIX/m68k-amigaos/ndk-include) does as well: the Makefile uses it
first.

Prints the directory to give the compiler (-idirafter).
"""

import argparse
import hashlib
import os
import sys
import urllib.request

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lha  # noqa: E402

NAME = 'NDK3.2.lha'
SHA256 = '96cabd4ad683dced632e147bf86dee0f50dcb1254386216c25c362916a6409bb'
URLS = ['https://aminet.net/dev/misc/NDK3.2.lha', 'https://ftp.fau.de/aminet/dev/misc/NDK3.2.lha']


def download(cache):
    path = os.path.join(cache, NAME)
    if os.path.exists(path) and hashlib.sha256(open(path, 'rb').read()).hexdigest() == SHA256:
        return path
    os.makedirs(cache, exist_ok=True)
    for url in URLS:
        try:
            print(f'fetch_ndk: {url}', file=sys.stderr)
            with urllib.request.urlopen(url, timeout=120) as r:
                data = r.read()
        except OSError as e:
            print(f'fetch_ndk: {e}', file=sys.stderr)
            continue
        if hashlib.sha256(data).hexdigest() != SHA256:
            print('fetch_ndk: wrong sha256, next mirror', file=sys.stderr)
            continue
        with open(path, 'wb') as f:
            f.write(data)
        return path
    sys.exit('fetch_ndk: cannot get ' + NAME)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--out', default='build/ndk')
    ap.add_argument('--cache', default='build/download')
    args = ap.parse_args()
    include = os.path.join(args.out, 'Include_H')
    if os.path.exists(os.path.join(include, 'exec', 'types.h')):
        print(include)
        return
    archive = download(args.cache)
    data = open(archive, 'rb').read()
    count = 0
    for path, method, packed, size, crc in lha.entries(data):
        if not path.startswith('Include_H/'):
            continue
        out = lha.extract_one(method, packed, size, crc)
        if out is None:
            continue
        to = os.path.join(args.out, path)
        os.makedirs(os.path.dirname(to), exist_ok=True)
        with open(to, 'wb') as f:
            f.write(out)
        count += 1
    print(f'fetch_ndk: {count} headers into {include}', file=sys.stderr)
    print(include)


if __name__ == '__main__':
    main()
