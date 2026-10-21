#!/usr/bin/env python3
"""Resolve AmigaDOS case-insensitive include names on a case-sensitive host.

Scans the given sources (and, recursively, everything they include) for
Include/IncBin directives. Every name that does not exist with its exact
spelling in one of the search directories, but does exist with different
case, gets a symlink under the output directory. Put the output directory
first on the assembler's include path.
"""

import argparse
import os
import re
import sys

DIRECTIVE_RE = re.compile(r'^\S*\s+(include|incbin)\s+"?([^"\s;]+)"?', re.IGNORECASE)


def find_ci(directory, name):
    """Find name below directory, matching each path component without case."""
    path = directory
    for part in name.split('/'):
        try:
            entries = os.listdir(path)
        except OSError:
            return None
        match = next((e for e in entries if e.lower() == part.lower()), None)
        if match is None:
            return None
        path = os.path.join(path, match)
    return path


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--out', required=True, help='directory to create the links in')
    ap.add_argument('-I', dest='dirs', action='append', default=[],
                    help='include search directory (repeatable, in search order)')
    ap.add_argument('sources', nargs='+')
    args = ap.parse_args()

    seen = set()
    todo = list(args.sources)
    links = 0
    while todo:
        src = todo.pop()
        if src in seen:
            continue
        seen.add(src)
        try:
            with open(src, encoding='latin-1') as f:
                lines = f.readlines()
        except OSError:
            continue
        for line in lines:
            m = DIRECTIVE_RE.match(line)
            if not m:
                continue
            kind, name = m.group(1).lower(), m.group(2)
            dirs = [os.path.dirname(src) or '.'] + args.dirs
            exact = next((os.path.join(d, name) for d in dirs
                          if os.path.exists(os.path.join(d, name))), None)
            target = exact or next(filter(None, (find_ci(d, name) for d in dirs)), None)
            if target is None:
                continue        # generated later, or a conditional include
            if exact is None:
                link = os.path.join(args.out, name)
                os.makedirs(os.path.dirname(link), exist_ok=True)
                if os.path.lexists(link):
                    os.remove(link)
                os.symlink(os.path.relpath(target, os.path.dirname(link)), link)
                links += 1
            if kind == 'include':
                todo.append(target)
    print(f'{len(seen)} files scanned, {links} case links created')


if __name__ == '__main__':
    sys.exit(main())
