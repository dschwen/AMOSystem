#!/usr/bin/env python3
"""Run the AMOS test programs in FS-UAE and compare them with golden results.

Each test is a text listing tests/amos/<name>.asc. The harness wraps it in a
prologue that opens AMOSTEST:out/<name>.txt as channel 1 (use Print #1 to
record results) and an epilogue that closes it and quits AMOS with System.
Run-time errors are written to the file as "ERROR <number>" and the test
resumes with the next instruction. A first line "Set Double Precision" stays
the first line of the program, as AMOS requires. Tests may also
save screens, e.g. Save Iff "AMOSTEST:out/<name>_1.iff". Every file a test
writes whose name starts with <name> is compared with tests/golden/.

Every test boots its own emulator from a small boot drive (AMOSTEST:) with
the AMOS tree on a shared read-only drive (AMOSPRO:) and the Workbench drive
for C:, Libs: etc. Tests run in parallel; a test that hangs (a syntax error
drops AMOS into the editor, for instance) times out on its own and leaves a
screenshot in build/test-run/<name>/timeout.png.

  run.py                     run all tests against build/dist (make dist)
  run.py --reference         run them against the prebuilt AMOS/ binaries
  run.py --reference --update-golden
                             record new golden results
  run.py NAME...             run only these tests

Needs fs-uae, Xvfb, scrot, the Kickstart ROM with its rom.key and the
Workbench directory drive (see --kickstart, --rom-key and --workbench).
"""

import argparse
import concurrent.futures
import os
import shutil
import subprocess
import sys
import threading
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

import amostok  # noqa: E402
import uaem     # noqa: E402

CAMOS = os.path.dirname(ROOT)
DEFAULT_KICKSTART = os.path.join(CAMOS, 'FS-UAE', 'Kickstarts', 'amiga-os-310-a1200.rom')
DEFAULT_WORKBENCH = os.path.join(CAMOS, 'Minimal Workbench v3.1')
# The Kickstart is a Cloanto-encrypted image; it needs the matching rom.key
DEFAULT_ROM_KEY = os.path.join(DEFAULT_WORKBENCH, 'Devs', 'Kickstarts', 'rom.key')
TESTS = os.path.join(ROOT, 'tests', 'amos')
GOLDEN = os.path.join(ROOT, 'tests', 'golden')
RUN = os.path.join(ROOT, 'build', 'test-run')

PROLOGUE = '''Open Out 1,"AMOSTEST:out/{name}.txt"
On Error Goto TESTERR
'''
EPILOGUE = '''Goto TESTEND
TESTEND:
Close 1
System
TESTERR:
Print #1,"ERROR ";Errn
Resume Next
'''

STARTUP = '''Workbench:C/Assign C: Workbench:C
C:SetPatch QUIET
Assign L: Workbench:L
Assign LIBS: Workbench:Libs
Assign DEVS: Workbench:Devs
Assign FONTS: Workbench:Fonts
Assign LOCALE: Workbench:Locale
Assign ENVARC: Workbench:Prefs/Env-Archive
MakeDir RAM:T RAM:ENV
Assign T: RAM:T
Assign ENV: RAM:ENV
Copy ENVARC: ENV: ALL QUIET
CD AMOSPRO:
AMOSPRO:AMOSPro AMOSTEST:{name}.AMOS
Echo "done" >AMOSTEST:out/DONE
'''

FS_UAE_CONFIG = '''[fs-uae]
amiga_model = A1200
chip_memory = 2048
fast_memory = 8192
jit_compiler = 0
uae_cpu_speed = max
uae_cpu_cycle_exact = false
uae_sound_output = none
kickstart_file = {kickstart}
floppy_drive_count = 1
hard_drive_0 = {hd}
hard_drive_0_label = AMOSTEST
hard_drive_0_priority = 10
hard_drive_1 = {amos}
hard_drive_1_label = AMOSPRO
hard_drive_1_read_only = 1
hard_drive_1_priority = -128
hard_drive_2 = {workbench}
hard_drive_2_label = Workbench
hard_drive_2_read_only = 1
hard_drive_2_priority = -128
base_dir = {base}
fullscreen = 0
automatic_input_grab = 0
initial_input_grab = 0
'''

_display_lock = threading.Lock()
_next_display = [90]


def start_xvfb():
    """Start Xvfb on a free display; returns (process, display)."""
    with _display_lock:
        while _next_display[0] < 300:
            n = _next_display[0]
            _next_display[0] += 1
            if os.path.exists(f'/tmp/.X11-unix/X{n}') or os.path.exists(f'/tmp/.X{n}-lock'):
                continue
            proc = subprocess.Popen(['Xvfb', f':{n}', '-screen', '0', '1280x1024x24',
                                     '-nolisten', 'tcp'],
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            time.sleep(1)
            if proc.poll() is None:
                return proc, f':{n}'
    raise RuntimeError('could not start Xvfb')


def stop(proc, group=False):
    if proc.poll() is not None:
        return
    if group:
        os.killpg(proc.pid, 15)
    else:
        proc.terminate()
    try:
        proc.wait(10)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.wait()


def prepare(amos_tree, kickstart, rom_key):
    """Shared, read-only parts of a run: the AMOS tree and the Kickstart."""
    if os.path.exists(RUN):
        shutil.rmtree(RUN)
    amos = os.path.join(RUN, 'AMOS')
    shutil.copytree(amos_tree, amos, ignore=shutil.ignore_patterns('*.uaem'))
    config = os.path.join(amos, 'AMOSPro_Interpreter_Config')
    if not os.path.exists(config):
        shutil.copy(os.path.join(ROOT, 'build', 'AMOSPro_Interpreter_Config'), config)
    uaem.add_sidecars(amos)
    # FS-UAE looks for rom.key next to the ROM, so give both a private home
    kickdir = os.path.join(RUN, 'Kickstarts')
    os.makedirs(kickdir)
    shutil.copy(kickstart, kickdir)
    if rom_key and os.path.exists(rom_key):
        shutil.copy(rom_key, os.path.join(kickdir, 'rom.key'))
    return amos, os.path.join(kickdir, os.path.basename(kickstart))


def run_test(name, tables, amos, kickstart, workbench, timeout, visible):
    """Boot an emulator for one test. Returns (finished, seconds, out dir)."""
    base = os.path.join(RUN, name)
    hd = os.path.join(base, 'hd')
    out = os.path.join(hd, 'out')
    os.makedirs(os.path.join(hd, 'S'))
    os.makedirs(out)
    os.makedirs(os.path.join(base, 'fsuae'))

    with open(os.path.join(TESTS, name + '.asc'), encoding='latin-1') as f:
        body = f.read()
    head = ''
    if body.lower().startswith('set double precision'):
        head, _, body = body.partition('\n')
        head += '\n'
    listing = head + PROLOGUE.format(name=name) + body.rstrip('\n') + '\n' + EPILOGUE
    amostok.tokenize_program(listing, tables).save(os.path.join(hd, name + '.AMOS'))
    with open(os.path.join(hd, 'S', 'startup-sequence'), 'w', encoding='latin-1') as f:
        f.write(STARTUP.format(name=name))
    uaem.add_sidecars(hd)

    config = os.path.join(base, 'test.fs-uae')
    with open(config, 'w') as f:
        f.write(FS_UAE_CONFIG.format(kickstart=kickstart, hd=hd, amos=amos,
                                     workbench=workbench, base=os.path.join(base, 'fsuae')))
    env = dict(os.environ)
    env['ALSOFT_DRIVERS'] = 'null'      # no sound device: keep OpenAL quiet
    xvfb = None
    if not visible:
        xvfb, env['DISPLAY'] = start_xvfb()
    start = time.time()
    try:
        with open(os.path.join(base, 'fs-uae.log'), 'w') as log:
            proc = subprocess.Popen(['fs-uae', config], stdout=log, stderr=subprocess.STDOUT,
                                    env=env, start_new_session=True)
            done = os.path.join(out, 'DONE')
            try:
                while time.time() - start < timeout:
                    if os.path.exists(done) or proc.poll() is not None:
                        break
                    time.sleep(0.5)
                if not os.path.exists(done) and xvfb:
                    # leave a picture of the hang behind
                    subprocess.run(['scrot', '-o', os.path.join(base, 'timeout.png')], env=env,
                                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            finally:
                stop(proc, group=True)
    finally:
        if xvfb:
            stop(xvfb)
    return os.path.exists(os.path.join(out, 'DONE')), time.time() - start, out


def outputs(directory, name):
    if not os.path.isdir(directory):
        return {}
    return {f: os.path.join(directory, f) for f in sorted(os.listdir(directory))
            if f.startswith(name) and f[len(name):len(name) + 1] in ('.', '_')
            and not f.endswith('.uaem')}


def compare(name, out):
    got = outputs(out, name)
    want = outputs(GOLDEN, name)
    problems = []
    for f in sorted(set(got) | set(want)):
        if f not in got:
            problems.append(f'{f} missing')
        elif f not in want:
            problems.append(f'{f} not in golden')
        elif open(got[f], 'rb').read() != open(want[f], 'rb').read():
            problems.append(f'{f} differs')
    if not got:
        problems.append('no output')
    return problems


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('tests', nargs='*', help='test names (default: all)')
    ap.add_argument('--reference', action='store_true',
                    help='use the prebuilt binaries in AMOS/ instead of build/dist')
    ap.add_argument('--amos-tree', help='test this AMOS directory instead (e.g. an installed '
                    'AMOS_Pro); without an interpreter config of its own it gets the built one')
    ap.add_argument('--update-golden', action='store_true',
                    help='store the results as the new golden files')
    ap.add_argument('--kickstart', default=DEFAULT_KICKSTART)
    ap.add_argument('--rom-key', default=DEFAULT_ROM_KEY)
    ap.add_argument('--workbench', default=DEFAULT_WORKBENCH)
    ap.add_argument('--timeout', type=int, default=60, help='seconds per test')
    ap.add_argument('-j', '--jobs', type=int, default=4, help='emulators in parallel')
    ap.add_argument('--visible', action='store_true',
                    help='show the emulator windows on $DISPLAY')
    args = ap.parse_args()

    tests = args.tests or sorted(f[:-4] for f in os.listdir(TESTS) if f.endswith('.asc'))
    amos_tree = args.amos_tree or (os.path.join(ROOT, 'AMOS') if args.reference
                                   else os.path.join(ROOT, 'build', 'dist', 'AMOS'))
    if not os.path.isdir(amos_tree):
        sys.exit(f'{amos_tree} missing: run "make dist" first')
    tables = amostok.Tables([os.path.join(amos_tree, 'APSystem')])
    amos, kickstart = prepare(amos_tree, args.kickstart, args.rom_key)

    failed = 0
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        futures = {name: pool.submit(run_test, name, tables, amos, kickstart,
                                     args.workbench, args.timeout, args.visible)
                   for name in tests}
        for name in tests:
            finished, seconds, out = futures[name].result()
            if not finished:
                failed += 1
                print(f'{name}: TIMEOUT after {seconds:.0f}s, see '
                      f'{os.path.relpath(os.path.join(RUN, name, "timeout.png"), ROOT)}')
                continue
            if args.update_golden:
                for f in outputs(GOLDEN, name).values():
                    os.remove(f)
                os.makedirs(GOLDEN, exist_ok=True)
                got = outputs(out, name)
                for f, path in got.items():
                    shutil.copy(path, os.path.join(GOLDEN, f))
                print(f'{name}: recorded {len(got)} files ({seconds:.0f}s)')
                continue
            problems = compare(name, out)
            failed += bool(problems)
            status = 'ok' if not problems else 'FAIL: ' + ', '.join(problems)
            print(f'{name}: {status} ({seconds:.0f}s)')
    print(f'{len(tests) - failed}/{len(tests)} passed' if not args.update_golden
          else f'{len(tests) - failed}/{len(tests)} recorded')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
