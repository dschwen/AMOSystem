#!/usr/bin/env python3
"""Run the AMOS test programs with the host build (build/host/amos) and
compare their text results with the golden ones.

The programs are those of tests/run.py (tests/amos/<name>.asc, the same
prologue and epilogue). Each runs in a directory of its own, the drive
AMOSTEST: (AMOS_ASSIGNS), with build/dist/AMOS as the current directory and
AMOSPRO:. The topaz font comes from the Kickstart the emulator uses
(AMOS_KICKSTART, AMOS_ROMKEY). Editor tests and tests with input scripts are
left out, as are the screenshots (only the .txt results are compared).

  host_run.py [NAME...]
"""

import argparse
import os
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tests'))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

import run  # noqa: E402
import amostok  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'host-test')
AMOS = os.path.join(ROOT, 'build', 'host', 'amos')
TREE = os.path.join(ROOT, 'build', 'dist', 'AMOS')


def prepare(name, tables):
    """The test's drive: its program, the programs it uses, out/"""
    hd = os.path.join(RUN, name)
    shutil.rmtree(hd, ignore_errors=True)
    out = os.path.join(hd, 'out')
    os.makedirs(out)
    with open(os.path.join(run.TESTS, name + '.asc'), encoding='latin-1') as f:
        body = f.read()
    head = ''
    if body.lower().startswith('set double precision'):
        head, _, body = body.partition('\n')
        head += '\n'
    listing = head + run.PROLOGUE.format(name=name) + body.rstrip('\n') + '\n' + run.EPILOGUE
    program = amostok.tokenize_program(listing, tables)
    run.lock_procedures(program)
    program.save(os.path.join(hd, name + '.AMOS'))
    extra = os.path.join(run.TESTS, name + '.d')
    if os.path.isdir(extra):
        for f in sorted(os.listdir(extra)):
            if f.endswith('.asc'):
                with open(os.path.join(extra, f), encoding='latin-1') as src:
                    side = amostok.tokenize_program(src.read(), tables)
                if side.lines and side.header[15] & 0x80:
                    side.lines = side.lines[1:]
                side.save(os.path.join(hd, f[:-4] + '.AMOS'))
    return hd, out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('tests', nargs='*')
    ap.add_argument('--timeout', type=int, default=30)
    args = ap.parse_args()
    tables = amostok.Tables([os.path.join(TREE, 'APSystem')])
    tests = args.tests or sorted(f[:-4] for f in os.listdir(run.TESTS) if f.endswith('.asc'))
    passed = failed = skipped = 0
    for name in tests:
        if run.is_editor_test(name) or os.path.exists(os.path.join(run.TESTS, name + '.input')):
            skipped += 1
            continue
        hd, out = prepare(name, tables)
        env = dict(os.environ)
        ram = os.path.join(hd, 'ram')
        os.makedirs(ram, exist_ok=True)
        env['AMOS_ASSIGNS'] = f'AMOSTEST={hd};AMOSPRO={TREE};RAM={ram}'
        env.setdefault('AMOS_KICKSTART', run.DEFAULT_KICKSTART)
        env.setdefault('AMOS_ROMKEY', run.DEFAULT_ROM_KEY)
        try:
            p = subprocess.run([AMOS, f'AMOSTEST:{name}.AMOS'], cwd=TREE, env=env, timeout=args.timeout,
                               stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
            note = p.stdout.decode('latin-1').strip().splitlines()
            note = note[-1] if note else f'exit {p.returncode}'
        except subprocess.TimeoutExpired:
            note = 'timeout'
        want = os.path.join(run.GOLDEN, name + '.txt')
        got = os.path.join(out, name + '.txt')
        if os.path.exists(want) and os.path.exists(got) and open(want, 'rb').read() == open(got, 'rb').read():
            passed += 1
            print(f'{name}: ok')
        else:
            failed += 1
            why = 'differs' if os.path.exists(got) else 'no output'
            print(f'{name}: {why} ({note})')
    print(f'{passed} passed, {failed} failed, {skipped} skipped')


if __name__ == '__main__':
    main()
