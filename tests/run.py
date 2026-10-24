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
Procedures named ZLOCK... are locked (encoded), as the verifier decodes them.
Programs a test uses (Include "AMOSTEST:x.AMOS", Run...) are listings in
tests/amos/<name>.d/x.asc, tokenised to AMOSTEST:x.AMOS (a first line Set
Double Precision makes its constants doubles, and is left out).

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

Editor tests: a test whose first line is "Rem EDITOR" checks what the
editor shows. It runs without prologue and epilogue and must end with a
syntax error, which makes AMOS open the editor on the listing. The harness
waits for the screen to settle and takes a few screenshots. The golden
result is <name>.png plus <name>_mask.png, the pixels that changed between
the screenshots (the cursor blinks), which the comparison ignores. Only
the top of the display is compared (EDITOR_AREA): keep the listing short.
With an input script, the screenshots are taken after the script ran.

Input: tests/amos/<name>.input scripts keyboard and mouse events (the
emulator then grabs the input), one command per line:
  sync NAME        wait until the program creates AMOSTEST:out/NAME
  wait SECONDS
  key KEYSYM       press and release an X key ("a", "Return", "Escape"),
                   "shift+", "ctrl+", "alt+", "lamiga+" or "ramiga+" in
                   front for modifiers
  keys TEXT        type the characters of TEXT (letters, digits, space)
  type TEXT        type the rest of the line as it is (any printable ASCII,
                   no comment)
  timeout SECONDS  a longer time limit for this test
  mask X1 Y1 X2 Y2 an editor test's screen and shots compared without
                   this rectangle too, as the memory gauges at the top
                   are (the gauges elsewhere, say)
  runs N           AMOS is started N times, the later times with no
                   program (after the editor quit, say); the test gets a
                   writable copy of the AMOS drive of its own
  move DX DY       move the mouse by DX,DY in one event
  glide DX DY      the same in small steps (large moves)
  button N up|down / click N
  shot NAME        screenshot of the display as AMOSTEST:out/<test>_NAME.png,
                   compared like the other results
Programs can signal with: Open Out 9,"AMOSTEST:out/NAME" : Close 9

Needs fs-uae, Xvfb, scrot, the Kickstart ROM with its rom.key and the
Workbench directory drive (see --kickstart, --rom-key and --workbench).
"""

import argparse
import concurrent.futures
import fcntl
import os
import shutil
import struct
import subprocess
import sys
import threading
import time

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, 'tools'))

from PIL import Image, ImageChops, ImageFilter  # noqa: E402

import amostok  # noqa: E402
TK_ENDPROC = 0x0390
import xinput   # noqa: E402
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
{again}Echo "done" >AMOSTEST:out/DONE
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
joystick_port_1 = none
hard_drive_0 = {hd}
hard_drive_0_label = AMOSTEST
hard_drive_0_priority = 10
hard_drive_1 = {amos}
hard_drive_1_label = AMOSPRO
hard_drive_1_read_only = {read_only}
hard_drive_1_priority = -128
hard_drive_2 = {workbench}
hard_drive_2_label = Workbench
hard_drive_2_read_only = 1
hard_drive_2_priority = -128
base_dir = {base}
fullscreen = 0
automatic_input_grab = 0
initial_input_grab = {grab}
governor_warning = 0
notification_duration = 1
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


def prepare(amos_tree, kickstart, rom_key, workbench):
    """Shared, read-only parts of a run: the AMOS tree and the Kickstart."""
    if os.path.exists(RUN):
        shutil.rmtree(RUN)
    amos = os.path.join(RUN, 'AMOS')
    shutil.copytree(amos_tree, amos, ignore=shutil.ignore_patterns('*.uaem'))
    config = os.path.join(amos, 'AMOSPro_Interpreter_Config')
    if not os.path.exists(config):
        shutil.copy(os.path.join(ROOT, 'build', 'AMOSPro_Interpreter_Config'), config)
    # the monitor's resource bank is not in the sources: the one of the
    # Workbench drive's AMOS Pro, read only
    resource = os.path.join(amos, 'APSystem', 'AMOSPro_Monitor_Resource.Abk')
    installed = os.path.join(workbench, 'AMOS_Pro', 'APSystem', 'AMOSPro_Monitor_Resource.Abk')
    if not os.path.exists(resource) and os.path.exists(installed):
        shutil.copy(installed, resource)
    uaem.add_sidecars(amos)
    # FS-UAE looks for rom.key next to the ROM, so give both a private home
    kickdir = os.path.join(RUN, 'Kickstarts')
    os.makedirs(kickdir)
    shutil.copy(kickstart, kickdir)
    if rom_key and os.path.exists(rom_key):
        shutil.copy(rom_key, os.path.join(kickdir, 'rom.key'))
    return amos, os.path.join(kickdir, os.path.basename(kickstart))


EDITOR_SHOTS = 6
# The part of the X screen compared: the AMOS display above the FS-UAE
# notifications that stay in the bottom left corner
EDITOR_AREA = (320, 268, 960, 700)


def is_editor_test(name):
    with open(os.path.join(TESTS, name + '.asc'), encoding='latin-1') as f:
        return f.readline().strip().lower() == 'rem editor'


def screenshot(env, path):
    subprocess.run(['scrot', '-o', path], env=env, stdout=subprocess.DEVNULL,
                   stderr=subprocess.DEVNULL, check=True)
    image = Image.open(path).convert('RGB').crop(EDITOR_AREA)
    image.save(path)
    return image


def changed_pixels(a, b):
    return sum(1 for v in ImageChops.difference(a, b).convert('L').getdata() if v)


def editor_shots(env, base, deadline):
    """Wait until the editor screen is still but for the cursor, then take
    EDITOR_SHOTS screenshots. Returns their paths, or None on timeout."""
    previous, quiet = None, 0
    while time.time() < deadline:
        time.sleep(1)
        shot = screenshot(env, os.path.join(base, 'settle.png'))
        quiet = quiet + 1 if previous and changed_pixels(previous, shot) < 400 else 0
        previous = shot
        if quiet >= 3:
            paths = []
            for i in range(EDITOR_SHOTS):
                paths.append(os.path.join(base, f'shot{i}.png'))
                screenshot(env, paths[-1])
                time.sleep(0.4)
            return paths
    return None


def record_editor(name, shots):
    images = [Image.open(p).convert('RGB') for p in shots]
    mask = Image.new('L', images[0].size, 0)
    for image in images[1:]:
        diff = ImageChops.difference(images[0], image).convert('L').point(lambda v: 255 if v else 0)
        mask = ImageChops.lighter(mask, diff)
    mask = mask.filter(ImageFilter.MaxFilter(5))
    images[0].save(os.path.join(GOLDEN, name + '.png'))
    mask.save(os.path.join(GOLDEN, name + '_mask.png'))


# The editor's chip and fast memory gauges (top right): the converted
# programs are larger than the originals, the fast one shows it
MEMORY_GAUGES = (536, 0, 610, 32)


def extra_masks(name):
    """The "mask" rectangles of a test's input script."""
    path = os.path.join(TESTS, name + '.input')
    if not os.path.exists(path):
        return []
    with open(path) as f:
        cmds = [parse_input_line(line) for line in f]
    return [tuple(int(v) for v in cmd[1:5]) for cmd in cmds if cmd and cmd[0] == 'mask']


def compare_editor(name, shots):
    golden = Image.open(os.path.join(GOLDEN, name + '.png')).convert('RGB')
    mask = Image.open(os.path.join(GOLDEN, name + '_mask.png')).convert('L')
    mask.paste(255, MEMORY_GAUGES)
    for box in extra_masks(name):
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
    return [f'screen differs ({best} pixels, see build/test-run/{name}/shot0.png)']


KEYSYMS = {' ': 'space', '.': 'period', ',': 'comma', '$': 'dollar', '"': 'quotedbl', '-': 'minus'}

# "type": any printable ASCII as the keys of a US keyboard (the Amiga's usa
# keymap): the unshifted key's X keysym, with shift where needed
UNSHIFTED = {' ': 'space', '`': 'grave', '-': 'minus', '=': 'equal', '[': 'bracketleft',
             ']': 'bracketright', '\\': 'backslash', ';': 'semicolon', "'": 'apostrophe',
             ',': 'comma', '.': 'period', '/': 'slash'}
SHIFTED = {'~': '`', '!': '1', '@': '2', '#': '3', '$': '4', '%': '5', '^': '6', '&': '7',
           '*': '8', '(': '9', ')': '0', '_': '-', '+': '=', '{': '[', '}': ']', '|': '\\',
           ':': ';', '"': "'", '<': ',', '>': '.', '?': '/'}


def type_keys(text):
    """The key names (for Display.key) that type text."""
    keys = []
    for c in text:
        shift = c.isupper() or c in SHIFTED
        base = SHIFTED.get(c, c).lower()
        name = UNSHIFTED.get(base, base)
        keys.append('shift+' + name if shift else name)
    return keys


def parse_input_line(line):
    """The words of a script line; "type" keeps the rest of the line as it
    is (one argument, comments included)."""
    line = line.rstrip('\n')
    if line.split()[:1] == ['type']:
        return ['type', line.split('type', 1)[1][1:]]
    return line.split('#')[0].split()


def play_input(script, display, out, deadline, name=None, env=None, stopped=None):
    """Send the events of an input script to the emulator window. Stops when
    stopped is set: the display goes away then, and Xlib would exit the
    whole process on the next call."""
    stopped = stopped or threading.Event()
    d = xinput.Display(display)
    try:
        d.move(640, 512)            # into the window: it gets the keyboard
        for cmd in script:
            if stopped.is_set():
                return
            op, args = cmd[0], cmd[1:]
            if op == 'sync':
                path = os.path.join(out, args[0])
                while not os.path.exists(path):
                    if time.time() > deadline or stopped.wait(0.2):
                        return
                time.sleep(0.3)
            elif op == 'wait':
                if stopped.wait(float(args[0])):
                    return
            elif op == 'key':
                d.key(args[0])
            elif op == 'keys':
                d.keys([KEYSYMS.get(c, c) for c in ' '.join(args)])
            elif op == 'type':
                d.keys(type_keys(args[0]), hold=0.03, gap=0.03)
            elif op in ('timeout', 'runs', 'mask'):
                pass
            elif op == 'move':
                d.move_by(int(args[0]), int(args[1]))
            elif op == 'glide':
                # small steps: a large relative move is clipped at the edge
                # of the X screen, the emulator only sees the clipped part
                dx, dy = int(args[0]), int(args[1])
                steps = max(1, (max(abs(dx), abs(dy)) + 19) // 20)
                done_x = done_y = 0
                for i in range(1, steps + 1):
                    x, y = dx * i // steps, dy * i // steps
                    d.move_by(x - done_x, y - done_y)
                    done_x, done_y = x, y
                    time.sleep(0.02)
            elif op == 'button':
                d.button(int(args[0]), args[1] == 'down')
            elif op == 'shot':
                time.sleep(0.5)
                screenshot(env, os.path.join(out, f'{name}_{args[0]}.png'))
            elif op == 'click':
                d.button(int(args[0]), True)
                time.sleep(0.1)
                d.button(int(args[0]), False)
            else:
                raise ValueError(f'unknown input command {op}')
            time.sleep(0.05)
    finally:
        d.close()


def lock_procedures(program):
    """Lock (encode) the procedures whose names start with ZLOCK, as the
    verifier's ProCode decodes them: each line's words after its first
    token XORed with a running key, flags 6 and 5 set."""
    lines = program.lines
    for n, line in enumerate(lines):
        if len(line) < 20 or struct.unpack_from('>H', line, 2)[0] != amostok.TK_PROC \
                or not line[18:].lower().startswith(b'zlock'):
            continue
        end = n + 1
        while struct.unpack_from('>H', lines[end], 2)[0] != TK_ENDPROC:
            end += 1
        body = b''.join(lines[n:end + 1])
        head = bytearray(line)
        size = len(body) - 14
        struct.pack_into('>LH', head, 4, size, 0x0321)
        head[11] = 0x5A
        key = ((size << 8) | (size >> 24)) & 0xFFFFFF00 | head[11]
        step, step2 = 1, 0x0321
        coded = [bytes(head)]
        for l in lines[n + 1:end + 1]:
            l = bytearray(l)
            for i in range(4, len(l), 2):
                w = struct.unpack_from('>H', l, i)[0] ^ (key & 0xFFFF)
                struct.pack_into('>H', l, i, w)
                key = (key & 0xFFFF0000) | ((key + step) & 0xFFFF)
                step = (step + step2) & 0xFFFF
                key = ((key >> 1) | (key << 31)) & 0xFFFFFFFF
            coded.append(bytes(l))
        coded[0] = coded[0][:10] + bytes([coded[0][10] | 0x60]) + coded[0][11:]
        lines[n:end + 1] = coded


def run_test(name, tables, amos, kickstart, workbench, timeout, visible):
    """Boot an emulator for one test. Returns (finished, seconds, out dir,
    screenshots of an editor test)."""
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
    editor = is_editor_test(name)
    script = None
    runs = 1
    if os.path.exists(os.path.join(TESTS, name + '.input')):
        with open(os.path.join(TESTS, name + '.input')) as f:
            script = [parse_input_line(line) for line in f]
            script = [cmd for cmd in script if cmd]
        for cmd in script:
            if cmd[0] == 'timeout':
                timeout = max(timeout, int(cmd[1]))
            elif cmd[0] == 'runs':
                runs = int(cmd[1])
    if editor:
        listing = body
    else:
        listing = head + PROLOGUE.format(name=name) + body.rstrip('\n') + '\n' + EPILOGUE
    program = amostok.tokenize_program(listing, tables)
    lock_procedures(program)
    program.save(os.path.join(hd, name + '.AMOS'))
    # programs the test uses (Include, Run...): tests/amos/<name>.d/*.asc
    extra = os.path.join(TESTS, name + '.d')
    if os.path.isdir(extra):
        for f in sorted(os.listdir(extra)):
            if f.endswith('.asc'):
                with open(os.path.join(extra, f), encoding='latin-1') as src:
                    side = amostok.tokenize_program(src.read(), tables)
                # a first line Set Double Precision: double constants, the
                # line itself left out (an Include could not have it)
                if side.lines and side.header[15] & 0x80:
                    side.lines = side.lines[1:]
                side.save(os.path.join(hd, f[:-4] + '.AMOS'))
    with open(os.path.join(hd, name + '.AMOS'), 'rb') as f:
        listing_bytes = f.read()
    with open(os.path.join(hd, 'S', 'startup-sequence'), 'w', encoding='latin-1') as f:
        f.write(STARTUP.format(name=name, again='AMOSPRO:AMOSPro\n' * (runs - 1)))
    uaem.add_sidecars(hd)
    read_only = 1
    if runs > 1:
        # AMOS writes to its own drive then (the configuration, the session)
        private = os.path.join(base, 'AMOS')
        shutil.copytree(amos, private)
        amos, read_only = private, 0

    config = os.path.join(base, 'test.fs-uae')
    with open(config, 'w') as f:
        f.write(FS_UAE_CONFIG.format(kickstart=kickstart, hd=hd, amos=amos,
                                     workbench=workbench, base=os.path.join(base, 'fsuae'), read_only=read_only,
                                     grab=1 if script else 0))
    env = dict(os.environ)
    env['ALSOFT_DRIVERS'] = 'null'      # no sound device: keep OpenAL quiet
    xvfb = None
    if not visible:
        xvfb, env['DISPLAY'] = start_xvfb()
    start = time.time()
    shots = None
    stopped = threading.Event()
    player = None
    try:
        with open(os.path.join(base, 'fs-uae.log'), 'w') as log:
            proc = subprocess.Popen(['fs-uae', config], stdout=log, stderr=subprocess.STDOUT,
                                    env=env, start_new_session=True)
            done = os.path.join(out, 'DONE')
            player = None
            if script:
                player = threading.Thread(target=play_input, args=(script, env['DISPLAY'], out,
                                                                   start + timeout, name, env, stopped),
                                          daemon=True)
                player.start()
            try:
                if editor:
                    if not xvfb:
                        raise RuntimeError('editor tests need the virtual display')
                    if player:
                        # the screenshots show the editor after the input
                        player.join(max(0, start + timeout - time.time()))
                    shots = editor_shots(env, base, start + timeout)
                    # a program the editor saved over the test's own
                    program = os.path.join(hd, name + '.AMOS')
                    if open(program, 'rb').read() != listing_bytes:
                        shutil.copy(program, os.path.join(out, name + '_saved.AMOS'))
                    return shots is not None, time.time() - start, out, shots
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
        # the input player lets go of the display first
        stopped.set()
        if player:
            player.join(10)
        if xvfb:
            stop(xvfb)
    return os.path.exists(os.path.join(out, 'DONE')), time.time() - start, out, shots


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
        elif f.endswith('.png'):
            a, b = Image.open(got[f]).convert('RGB'), Image.open(want[f]).convert('RGB')
            if a.size != b.size or a.tobytes() != b.tobytes():
                problems.append(f'{f} differs')
        elif open(got[f], 'rb').read() != open(want[f], 'rb').read():
            problems.append(f'{f} differs')
    if not got:
        problems.append('no output')
    return problems


def compare_files(name, out):
    """The files an editor test leaves (a saved program), against the golden
    ones but its screen."""
    screen = (name + '.png', name + '_mask.png')
    got = {f: p for f, p in outputs(out, name).items()}
    want = {f: p for f, p in outputs(GOLDEN, name).items() if f not in screen}
    boxes = [MEMORY_GAUGES] + extra_masks(name)     # (the gauges change with the build)
    problems = []
    for f in sorted(set(got) | set(want)):
        if f not in got:
            problems.append(f'{f} missing')
        elif f not in want:
            problems.append(f'{f} not in golden')
        elif f.endswith('.png') and boxes:
            if not same_image(got[f], want[f], boxes):
                problems.append(f'{f} differs')
        elif open(got[f], 'rb').read() != open(want[f], 'rb').read():
            problems.append(f'{f} differs')
    return problems


def same_image(a, b, boxes):
    """Two screenshots alike but in the "mask" rectangles and in one
    character cell at most (the editor's cursor blinks)."""
    a, b = Image.open(a).convert('RGB'), Image.open(b).convert('RGB')
    if a.size != b.size:
        return False
    for box in boxes:
        a.paste((0, 0, 0), box)
        b.paste((0, 0, 0), box)
    diff = ImageChops.difference(a, b).getbbox()
    return not diff or (diff[2] - diff[0] <= 8 and diff[3] - diff[1] <= 16)


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
    # one run at a time: they share build/test-run and the virtual display
    os.makedirs(os.path.dirname(RUN), exist_ok=True)
    lock = open(os.path.join(os.path.dirname(RUN), 'test-run.lock'), 'w')
    try:
        fcntl.flock(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
    except BlockingIOError:
        sys.exit('another tests/run.py is running')

    tests = args.tests or sorted(f[:-4] for f in os.listdir(TESTS) if f.endswith('.asc'))
    amos_tree = args.amos_tree or (os.path.join(ROOT, 'AMOS') if args.reference
                                   else os.path.join(ROOT, 'build', 'dist', 'AMOS'))
    if not os.path.isdir(amos_tree):
        sys.exit(f'{amos_tree} missing: run "make dist" first')
    if not args.amos_tree and not args.reference:
        # build/dist is a copy: refuse to test a stale one, or one whose
        # sources changed since (a failed build leaves the old one)
        if subprocess.run(['make', '-q', 'all'], cwd=ROOT, stdout=subprocess.DEVNULL,
                          stderr=subprocess.DEVNULL).returncode != 0:
            sys.exit('build/dist is not up to date: run "make dist" first')
        for built, copy in (('AMOSPro', 'AMOSPro'), ('AMOSPro.Lib', 'APSystem/AMOSPro.Lib'),
                            ('AMOS.library', 'APSystem/AMOS.library')):
            a, b = os.path.join(ROOT, 'build', built), os.path.join(amos_tree, copy)
            if os.path.exists(a) and (not os.path.exists(b) or open(a, 'rb').read() != open(b, 'rb').read()):
                sys.exit(f'{b} is older than build/{built}: run "make dist" first')
    tables = amostok.Tables([os.path.join(amos_tree, 'APSystem')])
    amos, kickstart = prepare(amos_tree, args.kickstart, args.rom_key, args.workbench)

    failed = 0
    with concurrent.futures.ThreadPoolExecutor(args.jobs) as pool:
        futures = {name: pool.submit(run_test, name, tables, amos, kickstart,
                                     args.workbench, args.timeout, args.visible)
                   for name in tests}
        for name in tests:
            finished, seconds, out, shots = futures[name].result()
            if not finished:
                failed += 1
                print(f'{name}: TIMEOUT after {seconds:.0f}s, see '
                      f'{os.path.relpath(os.path.join(RUN, name, "timeout.png"), ROOT)}')
                continue
            if args.update_golden:
                for f in outputs(GOLDEN, name).values():
                    os.remove(f)
                os.makedirs(GOLDEN, exist_ok=True)
                if shots:
                    record_editor(name, shots)
                    for f, path in outputs(out, name).items():
                        shutil.copy(path, os.path.join(GOLDEN, f))
                    print(f'{name}: recorded the editor screen ({seconds:.0f}s)')
                    continue
                got = outputs(out, name)
                for f, path in got.items():
                    shutil.copy(path, os.path.join(GOLDEN, f))
                print(f'{name}: recorded {len(got)} files ({seconds:.0f}s)')
                continue
            if shots:
                problems = compare_editor(name, shots) + compare_files(name, out)
            else:
                problems = compare(name, out)
            failed += bool(problems)
            status = 'ok' if not problems else 'FAIL: ' + ', '.join(problems)
            print(f'{name}: {status} ({seconds:.0f}s)')
    print(f'{len(tests) - failed}/{len(tests)} passed' if not args.update_golden
          else f'{len(tests) - failed}/{len(tests)} recorded')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
