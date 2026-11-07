#!/usr/bin/env python3
"""Run the AMOS test programs with the host build (build/host/amos) and
compare their text results with the golden ones.

The programs are those of tests/run.py (tests/amos/<name>.asc, the same
prologue and epilogue). Each runs in a directory of its own, the drive
AMOSTEST: (AMOS_ASSIGNS), with build/dist/AMOS as the current directory and
AMOSPRO:. Every file a test writes is compared, as tests/run.py does.

Editor tests and tests with input scripts run in a window of their own
(AMOS_SCALE=1: a hires pixel to a pixel, a line to two, as FS-UAE shows
them) on a virtual display (Xvfb), driven by the same input scripts; their
screenshots are compared with the Amiga's golden ones. "runs N" starts
AMOS N times, the later ones with no program.

  host_run.py [NAME...]          all tests, or these
  host_run.py --text-only        without the editor and input tests
  host_run.py --build build/host-sdl   the build linked to SDL2 (make host-sdl)
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
sys.path.insert(0, os.path.join(ROOT, 'tests'))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

import run  # noqa: E402
import amostok  # noqa: E402

RUN = os.path.join(ROOT, 'build', 'host-test')
AMOS = os.path.join(ROOT, 'build', 'host', 'amos')
TREE = os.path.join(ROOT, 'build', 'dist', 'AMOS')

# Where the window's AMOS display is on the virtual display (1280x1024, the
# window of 768x576 in its middle): the part tests/run.py compares
run.EDITOR_AREA = (330, 272, 970, 704)
run.RUN = RUN                   # (where its messages say the screenshots are)


HOST_ONLY = os.path.join(ROOT, 'tests', 'host_only')    # tests of what only the host has (its devices)
run.INPUT_DIRS.append(HOST_ONLY)


def source_of(name):
    p = os.path.join(HOST_ONLY, name + '.asc')
    return p if os.path.exists(p) else os.path.join(run.TESTS, name + '.asc')


def golden_of(f):
    """The golden file f (a test's result file): the host's own where there
    is one (host_only: its tests; host_only/variants: where it differs from
    the Amiga's)"""
    for d in (HOST_ONLY, os.path.join(HOST_ONLY, 'variants')):
        p = os.path.join(d, f)
        if os.path.exists(p):
            return p
    return os.path.join(run.GOLDEN, f)


def prepare(name, tables):
    """The test's drive: its program, the programs it uses, out/, and S/
    as the emulator's drive has it (the file selector shows it); its RAM:
    and devices directory beside it"""
    hd = os.path.join(RUN, name, 'hd')
    shutil.rmtree(os.path.join(RUN, name), ignore_errors=True)
    out = os.path.join(hd, 'out')
    os.makedirs(out)
    os.makedirs(os.path.join(hd, 'S'))
    with open(os.path.join(hd, 'S', 'startup-sequence'), 'w') as f:
        f.write('; the emulator\'s drive has one\n')
    with open(source_of(name), encoding='latin-1') as f:
        body = f.read()
    head = ''
    if body.lower().startswith('set double precision'):
        head, _, body = body.partition('\n')
        head += '\n'
    if body.split('\n', 1)[0].strip().lower() == 'rem editor':
        listing = head + body
    else:
        listing = head + run.PROLOGUE.format(name=name) + body.rstrip('\n') + '\n' + run.EPILOGUE
    program = amostok.tokenize_program(listing, tables)
    run.lock_procedures(program)
    program.save(os.path.join(hd, name + '.AMOS'))
    # the host's own tests of the devices (docs/DEVICES.md): their disks (name.files, copied), the
    # configuration (name.conf: @FILES@ the copy) and the options of the command line (name.args)
    files = os.path.join(HOST_ONLY, name + '.files')
    if os.path.isdir(files):
        shutil.copytree(files, os.path.join(os.path.dirname(hd), 'files'))
        # a folder NAME.adfdir is a floppy image NAME.adf made by tools/adf.py (the label NAME)
        import adf
        for here, dirs, names in os.walk(os.path.join(os.path.dirname(hd), 'files')):
            for d in [d for d in dirs if d.endswith('.adfdir')]:
                folder = os.path.join(here, d)
                with open(folder[:-len('.adfdir')] + '.adf', 'wb') as image:
                    image.write(adf.pack(folder, d[:-len('.adfdir')], ffs=not d.startswith('Ofs')))
                shutil.rmtree(folder)
            dirs[:] = [d for d in dirs if not d.endswith('.adfdir')]
    conf = os.path.join(HOST_ONLY, name + '.conf')
    if os.path.exists(conf):
        with open(conf) as f:
            text = f.read().replace('@FILES@', os.path.join(os.path.dirname(hd), 'files'))
        with open(os.path.join(os.path.dirname(hd), 'amos.conf'), 'w') as f:
            f.write(text)
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


def display_tree():
    """The AMOS tree of the window tests: build/dist's with the original
    editor configuration (the golden screens have its colours) and the
    monitor's resource, as tests/run.py prepares it"""
    tree = os.path.join(RUN, 'AMOS')
    shutil.rmtree(tree, ignore_errors=True)
    shutil.copytree(TREE, tree, ignore=shutil.ignore_patterns('*.uaem'))
    shutil.copy(os.path.join(ROOT, 'AMOS', 'APSystem', 'AMOSPro_Editor_Config'),
                os.path.join(tree, 'APSystem', 'AMOSPro_Editor_Config'))
    resource = os.path.join(tree, 'APSystem', 'AMOSPro_Monitor_Resource.Abk')
    installed = os.path.join(run.DEFAULT_WORKBENCH, 'AMOS_Pro', 'APSystem', 'AMOSPro_Monitor_Resource.Abk')
    if not os.path.exists(resource) and os.path.exists(installed):
        shutil.copy(installed, resource)
    return tree


def environment(hd, tree):
    env = dict(os.environ)
    ram = os.path.join(os.path.dirname(hd), 'ram')
    os.makedirs(ram, exist_ok=True)
    env['AMOS_ASSIGNS'] = f'AMOSTEST={hd};AMOSPRO={tree};RAM={ram}'
    env['AMOS_DEVICES'] = os.path.join(os.path.dirname(hd), 'devices')
    # (not the user's own: the test's, if it has one)
    conf = os.path.join(os.path.dirname(hd), 'amos.conf')
    env['AMOS_CONFIG'] = conf if os.path.exists(conf) else os.devnull
    env['TMPDIR'] = os.path.dirname(hd)
    # the test's own environment (name.env: KEY=value lines, @FILES@ its disks)
    envfile = os.path.join(HOST_ONLY, os.path.basename(os.path.dirname(hd)) + '.env')
    if os.path.exists(envfile):
        with open(envfile) as f:
            for line in f.read().splitlines():
                if '=' in line and not line.startswith('#'):
                    key, value = line.split('=', 1)
                    env[key] = value.replace('@FILES@', os.path.join(os.path.dirname(hd), 'files'))
    # the Amiga's tests: a volume Workbench, as the emulator has (its C:
    # without Echo, a command of the shell's), with the host's own ones
    if not os.path.exists(os.path.join(HOST_ONLY, os.path.basename(os.path.dirname(hd)) + '.asc')):
        for volume in ('Workbench/C', 'Work', 'RAM'):
            os.makedirs(os.path.join(env['AMOS_DEVICES'], volume), exist_ok=True)
    return env


def run_text(name, hd, timeout):
    """A test with no window: its note (the last line AMOS printed)"""
    env = environment(hd, TREE)
    env['AMOS_DISPLAY'] = 'none'           # no window, no sound
    try:
        args = []
        argfile = os.path.join(HOST_ONLY, name + '.args')
        if os.path.exists(argfile):
            import shlex
            with open(argfile) as f:
                args = shlex.split(f.read().replace('@FILES@', os.path.join(os.path.dirname(hd), 'files')))
        p = subprocess.run([AMOS] + args + [f'AMOSTEST:{name}.AMOS'], cwd=TREE, env=env, timeout=timeout,
                           stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        # (kept, for what a failing test did: CI prints it)
        with open(os.path.join(os.path.dirname(hd), 'stdout.log'), 'wb') as f:
            f.write(p.stdout)
        note = p.stdout.decode('latin-1').strip().splitlines()
        trace = [line for line in note if line.startswith('amos: ') and line.split()[1] in ('signal', 'pc', 'lr', 'frame', 'loaded')]
        if trace:        # (AMOS_CRASH_TRACE: where it was when it crashed or was stopped)
            import re
            files = [re.sub(r"amos: file '(.*?)' -> '.*?' mode \d+ error (\d+)", r"\1=\2", line)
                     for line in note if line.startswith('amos: file')]
            return ' | '.join(files[:14] + trace[:2])
        return note[-1] if note else f'exit {p.returncode}'
    except subprocess.TimeoutExpired:
        return 'timeout'


def run_window(name, hd, out, tree, timeout):
    """A test in a window on a virtual display, its input script played:
    (finished, the editor's screenshots)"""
    script, runs = [], 1
    path = run.input_path(name)
    if os.path.exists(path):
        with open(path) as f:
            script = [cmd for cmd in (run.parse_input_line(line) for line in f) if cmd]
    for cmd in script:
        if cmd[0] == 'timeout':
            timeout = max(timeout, int(cmd[1]))
        elif cmd[0] == 'runs':
            runs = int(cmd[1])
    editor = not os.path.exists(os.path.join(HOST_ONLY, name + '.asc')) and run.is_editor_test(name)
    if runs > 1:
        # AMOS writes to its own drive then (the configuration, the session)
        private = os.path.join(os.path.dirname(hd), 'AMOS')
        shutil.rmtree(private, ignore_errors=True)
        shutil.copytree(tree, private)
        tree = private
    env = environment(hd, tree)
    env['AMOS_SCALE'] = '1'
    env['SDL_AUDIODRIVER'] = 'dummy'
    if os.path.exists(os.path.join(HOST_ONLY, name + '.asc')):
        env['SHOT_AREA'] = '256,224,1024,800'      # (the whole window, on the 1280x1024 display)
    xvfb, env['DISPLAY'] = run.start_xvfb()
    # the pointer where the scripts start it (its move there is no motion
    # for AMOS then: FS-UAE only grabs the mouse after it)
    d = run.xinput.Display(env['DISPLAY'])
    d.move(640, 512)
    d.close()
    with open(os.path.join(hd, name + '.AMOS'), 'rb') as f:
        listing = f.read()
    start = time.time()
    deadline = start + timeout
    stopped = threading.Event()
    player, shots, finished = None, None, False
    log = open(os.path.join(os.path.dirname(hd), 'amos.log'), 'w')
    proc = None
    try:
        if script:
            player = threading.Thread(target=run.play_input, args=(script, env['DISPLAY'], out, deadline,
                                                                   name, env, stopped), daemon=True)
        for n in range(runs):
            args = [AMOS] + ([f'AMOSTEST:{name}.AMOS'] if n == 0 else [])
            proc = subprocess.Popen(args, cwd=tree, env=env, stdout=log, stderr=subprocess.STDOUT)
            if n == 0 and player:
                time.sleep(1)           # the window open, before the first event
                player.start()
            if editor and n == runs - 1:
                if player:
                    player.join(max(0, deadline - time.time()))
                shots = run.editor_shots(env, os.path.dirname(hd), deadline)
                program = os.path.join(hd, name + '.AMOS')
                if open(program, 'rb').read() != listing:
                    shutil.copy(program, os.path.join(out, name + '_saved.AMOS'))
                finished = shots is not None
                break
            try:
                proc.wait(max(0, deadline - time.time()))
            except subprocess.TimeoutExpired:
                subprocess.run(['scrot', '-o', os.path.join(os.path.dirname(hd), 'timeout.png')], env=env,
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                break
            finished = n == runs - 1
    finally:
        if proc and proc.poll() is None:
            run.stop(proc)
        stopped.set()
        if player and player.is_alive():
            player.join(10)
        run.stop(xvfb)
        log.close()
    return finished, shots


def compare_editor(name, shots):
    """tests/run.py's comparison of an editor test's screen, against the
    host's variant of it if there is one"""
    from PIL import Image, ImageChops
    golden = Image.open(golden_of(name + '.png')).convert('RGB')
    mask = Image.open(golden_of(name + '_mask.png')).convert('L')
    mask.paste(255, run.MEMORY_GAUGES)
    for box in run.extra_masks(name):
        mask.paste(255, box)
    keep = mask.point(lambda v: 0 if v else 255)
    best = None
    for path in shots:
        diff = ImageChops.difference(golden, Image.open(path).convert('RGB')).convert('L')
        diff = ImageChops.multiply(diff.point(lambda v: 255 if v else 0), keep)
        count = sum(1 for v in diff.getdata() if v)
        best = count if best is None else min(best, count)
        if not count:
            return []
    return [f'screen differs ({best} pixels, see {os.path.relpath(os.path.join(RUN, name), ROOT)}/shot0.png)']


def compare_files(name, out):
    """tests/run.py's comparison of the files an editor test leaves (but its
    screen), against the host's variants where there are"""
    screen = (name + '.png', name + '_mask.png')
    got = run.outputs(out, name)
    want = {f for f in run.outputs(run.GOLDEN, name) if f not in screen}
    boxes = [run.MEMORY_GAUGES] + run.extra_masks(name)
    problems = []
    for f in sorted(set(got) | want):
        if f not in got:
            problems.append(f'{f} missing')
        elif f not in want:
            problems.append(f'{f} not in golden')
        elif f.endswith('.png'):
            if not run.same_image(got[f], golden_of(f), boxes):
                problems.append(f'{f} differs')
        elif open(got[f], 'rb').read() != open(golden_of(f), 'rb').read():
            problems.append(f'{f} differs')
    return problems


def compare(name, out, fallback_font=False):
    """tests/run.py's comparison, the host's variants of the text results"""
    got = run.outputs(out, name)
    problems = []
    for f in sorted(set(got) | set(run.outputs(run.GOLDEN, name)) | {f for f in run.outputs(HOST_ONLY, name) if not f.endswith(('.asc', '.conf', '.args', '.files', '.env', '.input'))}):
        want = golden_of(f)
        if f not in got:
            problems.append(f'{f} missing')
        elif not os.path.exists(want):
            problems.append(f'{f} not in golden')
        elif f.endswith('.png'):
            if not run.same_image(got[f], want, run.extra_masks(name)):
                problems.append(f'{f} differs')
        elif fallback_font and f.endswith('.iff'):
            # ROM-free Topaz glyphs differ from the ROM-based image goldens.
            # Still require every image to exist and compare all other output.
            continue
        elif open(got[f], 'rb').read() != open(want, 'rb').read():
            problems.append(f'{f} differs')
    if not got:
        problems.append('no output')
    return problems


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('tests', nargs='*')
    ap.add_argument('--timeout', type=int, default=30)
    ap.add_argument('--text-only', action='store_true', help='leave out the editor and input tests')
    ap.add_argument('--fallback-font', action='store_true', help='require --text-only; check IFF presence but skip ROM-font image byte comparisons')
    ap.add_argument('-j', '--jobs', type=int, default=4)
    ap.add_argument('--build', help='the host build to test (default build/host; build/host-sdl: make host-sdl)')
    args = ap.parse_args()
    if args.fallback_font and not args.text_only:
        ap.error("--fallback-font requires --text-only")
    global AMOS
    if args.build:
        AMOS = os.path.join(os.path.abspath(args.build), 'amos')
    tables = amostok.Tables([os.path.join(TREE, 'APSystem')])
    tests = args.tests or sorted(f[:-4] for d in (run.TESTS, HOST_ONLY) for f in os.listdir(d) if f.endswith('.asc'))
    unknown = [t for t in tests if not os.path.exists(source_of(t))]
    if unknown:
        sys.exit('no such test: ' + ' '.join(unknown))
    tree = None

    def one(name):
        own = os.path.exists(os.path.join(HOST_ONLY, name + '.asc'))
        window = (not own and (run.is_editor_test(name) or os.path.exists(os.path.join(run.TESTS, name + '.input')))) or \
                 (own and os.path.exists(os.path.join(HOST_ONLY, name + '.input')))
        if window and args.text_only:
            return None
        hd, out = prepare(name, tables)
        if not window:
            note = run_text(name, hd, args.timeout)
            problems = compare(name, out, args.fallback_font)
            return [f'{p} ({note})' for p in problems[:1]] + problems[1:]
        finished, shots = run_window(name, hd, out, tree, max(args.timeout, 60))
        if not finished:
            return [f'timeout, see {os.path.relpath(os.path.dirname(hd), ROOT)}/timeout.png']
        if shots:
            return compare_editor(name, shots) + compare_files(name, out)
        return compare(name, out, args.fallback_font)

    if not args.text_only:
        tree = display_tree()
    passed = failed = skipped = 0
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        futures = {name: pool.submit(one, name) for name in tests}
        for name in tests:
            problems = futures[name].result()
            if problems is None:
                skipped += 1
            elif problems:
                failed += 1
                print(f'{name}: ' + ', '.join(problems))
            else:
                passed += 1
                print(f'{name}: ok')
    print(f'{passed} passed, {failed} failed' + (f', {skipped} skipped' if skipped else ''))
    return bool(failed)


if __name__ == '__main__':
    sys.exit(main())
