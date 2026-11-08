#!/usr/bin/env python3
"""The screenshots of docs/MANUAL.md (docs/images): the editor, direct mode and the Devices
panel, taken from build/host/amos in a window on a virtual display (Xvfb, as the tests do).
Run after `make host` (and `make dist`, for the AMOS tree with the examples):

    python3 tools/doc_screenshots.py [--program PATH.AMOS] [--out docs/images]
"""
import argparse
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import time

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tests'))
import run          # noqa: E402 (tests/run.py: Xvfb, the X input, type_keys)

AMOS = ROOT / 'build/host/amos'
PROGRAM = ROOT / 'examples/Examples/Games/Roller_II/Roller_II.AMOS'
# the window (768x576 at AMOS_SCALE=1) in the middle of Xvfb's 1280x1024
AREA = (256, 224, 1024, 800)


def shot(display, path):
    env = dict(os.environ, DISPLAY=display)
    subprocess.run(['scrot', '-o', str(path)], env=env, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    from PIL import Image
    Image.open(path).convert('RGB').crop(AREA).save(path, optimize=True)
    print(path.relative_to(ROOT) if path.is_relative_to(ROOT) else path)


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--program', type=Path, default=PROGRAM, help='the program shown in the editor')
    ap.add_argument('--out', type=Path, default=ROOT / 'docs/images')
    args = ap.parse_args()
    args.out.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='amos-shots-') as temp:
        temp = Path(temp)
        # a setup worth showing in the panel: two floppies, a hard disk, a library
        # (in a home folder of its own: the panel shows it as ~)
        home = temp / 'home'
        for folder in ('Disks/Game Disk 1', 'Disks/Extras', 'Projects', '.local/share/amos/devices/Work'):
            (home / folder).mkdir(parents=True)
        (home / '.config/amos').mkdir(parents=True)
        (home / '.config/amos/amos.conf').write_text('''[df0]
path = ~/Disks/Game Disk 1
[df1]
path = ~/Disks/Extras
readonly = yes
[dh1]
path = ~/Projects
[library]
path = ~/Disks
''')
        env = {k: v for k, v in os.environ.items() if not k.startswith(('AMOS_', 'XDG_'))}
        env.update(HOME=str(home), AMOS_SCALE='1', SDL_AUDIODRIVER='dummy', TMPDIR=str(temp))
        xvfb, env['DISPLAY'] = run.start_xvfb()
        amos = None
        try:
            d = run.xinput.Display(env['DISPLAY'])
            d.move(640, 512)
            amos = subprocess.Popen([str(AMOS), '--open-document', str(args.program)], cwd=home, env=env,
                                    stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            time.sleep(8)               # the editor up, the program loaded
            d.key('Escape')             # (the About box away)
            time.sleep(1.5)
            shot(env['DISPLAY'], args.out / 'editor.png')
            d.key('Escape')             # direct mode
            time.sleep(1.5)
            for line in ('Screen Open 0,640,200,16,Hires : Curs Off : Flash Off : Cls 0',
                         'For I=1 To 15 : Ink I : Circle 320,100,I*6 : Next I',
                         'Print Screen Width,Screen Height'):
                d.keys(run.type_keys(line), hold=0.03, gap=0.03)
                d.key('Return')
                time.sleep(0.5)
            time.sleep(1)
            shot(env['DISPLAY'], args.out / 'direct.png')
            d.key('Escape')             # the editor again, then the panel over it
            time.sleep(1.5)
            d.key('F12')
            time.sleep(1)
            shot(env['DISPLAY'], args.out / 'panel.png')
            d.key('F12')
            d.close()
        finally:
            if amos:
                run.stop(amos)
            run.stop(xvfb)


if __name__ == '__main__':
    main()
