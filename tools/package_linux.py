#!/usr/bin/env python3
"""Package the Linux x86-64 host: a portable tarball and a .deb.

Both hold the same payload: bin/amos (+ amos-player), share/amos/AMOS (the
system files, the examples, the blue editor palette), the desktop entry, the
MIME type and the icons. The tree is staged from git HEAD, so that developer
settings and untracked files stay out; the executable comes from --build.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import ext_config
import install_icons

NAME = 'amos'


def run(*command, **kw):
    return subprocess.check_output(command, cwd=ROOT, text=True, **kw)


def stage(prefix, executable):
    """The files of an installation under `prefix` (an empty directory)."""
    (prefix / 'bin').mkdir(parents=True)
    shutil.copy2(executable, prefix / 'bin/amos')
    os.chmod(prefix / 'bin/amos', 0o755)
    (prefix / 'bin/amos-player').symlink_to('amos')
    tree = prefix / 'share/amos/AMOS'
    for source in ('AMOS', 'examples'):
        for name in run('git', 'ls-tree', '-rz', '--name-only', 'HEAD', '--', source).split('\0'):
            if not name:
                continue
            relative = name[len('AMOS/'):] if source == 'AMOS' else name[len('examples/'):]
            dest = tree / relative
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(subprocess.check_output(['git', 'show', 'HEAD:' + name], cwd=ROOT))
            os.chmod(dest, 0o644)
    subprocess.check_call([sys.executable, str(ROOT / 'tools/blue_palette.py'),
                           str(tree / 'APSystem/AMOSPro_Editor_Config'),
                           str(tree / 'APSystem/AMOSPro_Editor_Config')])
    ext_config.patch_file(tree / 'AMOSPro_Interpreter_Config')
    install_icons.linux(prefix)
    licenses = prefix / 'share/doc/amos'
    licenses.mkdir(parents=True)
    shutil.copy2(ROOT / 'LICENSE', licenses / 'LICENSE')
    shutil.copy2(ROOT / 'data/fonts/topaz8/LICENSE.AROS', licenses / 'LICENSE.AROS-font')
    (licenses / 'README').write_text('''AMOS Professional for Linux

Run `amos` for the editor; `amos CLI.AMOS` runs a program; `amos-player prog.AMOS` runs
one without the editor. The system files are read-only in share/amos/AMOS: a writable copy is
kept in ${XDG_DATA_HOME:-~/.local/share}/amos/runtime, your disks in .../amos/devices and the
configuration in ${XDG_CONFIG_HOME:-~/.config}/amos.

The window needs SDL2 (libsdl2-2.0-0); speech needs espeak-ng. The original AMOS notices are
in the system files. Original AMOS Professional: MIT licence, Francois Lionet and Europress
Software. The built-in Topaz font is AROS's (LICENSE.AROS-font).
''')


def build_deb(prefix, version, out):
    root = out / f'{NAME}_{version}_amd64'
    shutil.copytree(prefix, root / 'usr', symlinks=True)
    size = sum(f.stat().st_size for f in root.rglob('*') if f.is_file()) // 1024
    (root / 'DEBIAN').mkdir()
    (root / 'DEBIAN/control').write_text(f'''Package: {NAME}
Version: {version}
Section: devel
Priority: optional
Architecture: amd64
Installed-Size: {size}
Depends: libc6 (>= 2.31), libstdc++6, libsdl2-2.0-0
Recommends: libespeak-ng1
Maintainer: Daniel Schwen <daniel@schwen.de>
Homepage: https://github.com/dschwen/AMOSystem
Description: AMOS Professional BASIC editor and runtime
 AMOS Professional, the Amiga BASIC, running on Linux: the original
 editor, interpreter and extensions as C++, with the Amiga's graphics,
 sprites, bobs, AMAL and music.
''')
    (root / 'DEBIAN/postinst').write_text('''#!/bin/sh
set -e
if [ "$1" = configure ]; then
    command -v update-mime-database >/dev/null && update-mime-database /usr/share/mime || true
    command -v update-desktop-database >/dev/null && update-desktop-database -q /usr/share/applications || true
    command -v gtk-update-icon-cache >/dev/null && gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
''')
    (root / 'DEBIAN/postrm').write_text('''#!/bin/sh
set -e
if [ "$1" = remove ] || [ "$1" = purge ]; then
    command -v update-mime-database >/dev/null && update-mime-database /usr/share/mime || true
    command -v update-desktop-database >/dev/null && update-desktop-database -q /usr/share/applications || true
    command -v gtk-update-icon-cache >/dev/null && gtk-update-icon-cache -q -t -f /usr/share/icons/hicolor || true
fi
''')
    for script in ('postinst', 'postrm'):
        os.chmod(root / 'DEBIAN' / script, 0o755)
    for path in [root, *root.rglob('*')]:
        if path.is_dir() and not path.is_symlink():
            os.chmod(path, 0o755)
    deb = out / f'{NAME}_{version}_amd64.deb'
    subprocess.check_call(['fakeroot', 'dpkg-deb', '--build', '-Zxz', str(root), str(deb)],
                          stdout=subprocess.DEVNULL)
    return deb


def tarball(prefix, version, out):
    top = f'{NAME}-{version}-linux-x86_64'
    path = out / f'{top}.tar.xz'

    def reset(info):
        info.uid = info.gid = 0
        info.uname = info.gname = 'root'
        info.mtime = 0
        return info
    with tarfile.open(path, 'w:xz') as t:
        for item in sorted(prefix.rglob('*')):
            t.add(item, arcname=f'{top}/{item.relative_to(prefix)}', recursive=False, filter=reset)
    return path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--version', required=True)
    parser.add_argument('--build', type=Path, default=ROOT / 'build/linux-release')
    parser.add_argument('--out', type=Path, default=ROOT / 'build/packages')
    args = parser.parse_args()
    executable = args.build / 'amos'
    if not executable.is_file():
        parser.error(f'missing {executable}')
    info = subprocess.run(['file', '-L', str(executable)], capture_output=True, text=True).stdout
    if 'x86-64' not in info:
        parser.error(f'{executable} is not an x86-64 executable: {info.strip()}')
    out = args.out.resolve()
    work = out / 'linux-stage'
    if work.exists():
        shutil.rmtree(work)
    prefix = work / 'usr'
    stage(prefix, executable)
    out.mkdir(parents=True, exist_ok=True)
    files = [tarball(prefix, args.version, out)]
    if shutil.which('dpkg-deb') and shutil.which('fakeroot'):
        files.append(build_deb(prefix, args.version, work))
        shutil.move(str(files[-1]), out / files[-1].name)
        files[-1] = out / files[-1].name
    shutil.rmtree(work)
    sums = ''.join(f'{hashlib.sha256(f.read_bytes()).hexdigest()}  {f.name}\n' for f in files)
    (out / f'{NAME}-{args.version}-linux.sha256').write_text(sums)
    manifest = {'version': args.version, 'source_revision': run('git', 'rev-parse', 'HEAD').strip(),
                'source_dirty': bool(run('git', 'status', '--porcelain').strip()),
                'files': {f.name: f.stat().st_size for f in files}}
    (out / f'{NAME}-{args.version}-linux.json').write_text(json.dumps(manifest, indent=2) + '\n')
    for f in files:
        print(f)


if __name__ == '__main__':
    main()
