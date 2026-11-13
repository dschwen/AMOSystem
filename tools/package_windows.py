#!/usr/bin/env python3
"""Package the experimental Windows host and reproducible headless smoke tests."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT / 'tools'))
import amostok
import ext_config

PROLOGUE = 'Open Out 1,"AMOSTEST:out/{name}.txt"\nOn Error Goto TESTERR\n'
EPILOGUE = 'Goto TESTEND\nTESTEND:\nClose 1\nSystem\nTESTERR:\nPrint #1,"ERROR ";Errn\nResume Next\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, default=ROOT / 'build/windows')
    parser.add_argument('--sdk', type=Path, required=True)
    parser.add_argument('--out', type=Path, default=ROOT / 'build/windows-portable')
    parser.add_argument('--release', metavar='VERSION', help='the clean portable release ZIP (no smoke tests), named for this version')
    args = parser.parse_args()
    if args.release:
        release(args, parser)
        return
    # Validate inputs before touching the package directory.
    for source in (args.build / 'amos.exe', args.sdk / 'bin/SDL2.dll'):
        if not source.is_file():
            parser.error(f'missing {source}')
    out = args.out.resolve()
    # Packaging may replace its own generated output, never arbitrary paths.
    if not out.is_relative_to((ROOT / 'build').resolve()) or out == (ROOT / 'build').resolve():
        parser.error('--out must be a subdirectory of the repository build directory')
    if out == args.build.resolve() or out in args.build.resolve().parents:
        parser.error('--out must not contain the build input directory')
    if out.exists():
        shutil.rmtree(out)
    out.mkdir(parents=True)
    shutil.copy2(args.build / 'amos.exe', out)
    shutil.copy2(args.sdk / 'bin/SDL2.dll', out)
    if (args.build / 'windows-probe.exe').exists():
        shutil.copy2(args.build / 'windows-probe.exe', out)
    shutil.copytree(ROOT / 'AMOS', out / 'AMOS', ignore=shutil.ignore_patterns('*.uaem'))
    ext_config.patch_file(out / 'AMOS' / 'AMOSPro_Interpreter_Config')
    shutil.copy2(ROOT / 'tests/windows/smoke.ps1', out / 'smoke.ps1')
    tables = amostok.Tables([str(ROOT / 'AMOS/APSystem')])
    manifest = []
    for name in ('control', 'control2', 'files', 'banks', 'bankfiles'):
        hd = out / 'smoke' / name
        hd.mkdir(parents=True)
        body = (ROOT / 'tests/amos' / f'{name}.asc').read_text(encoding='latin-1')
        program = amostok.tokenize_program(PROLOGUE.format(name=name) + body.rstrip('\n') + '\n' + EPILOGUE, tables)
        program.save(str(hd / f'{name}.AMOS'))
        expected = {}
        for golden in sorted((ROOT / 'tests/golden').iterdir()):
            if not (golden.name == name + '.txt' or golden.name.startswith(name + '_')):
                continue
            for directory in ('tests/host_only/variants', 'tests/host_only'):
                variant = ROOT / directory / golden.name
                if variant.is_file():
                    golden = variant
            expected[golden.name] = hashlib.sha256(golden.read_bytes()).hexdigest().upper()
        manifest.append({'name': name, 'expected': expected})
    (out / 'smoke.json').write_text(json.dumps(manifest, indent=2) + '\n')
    (out / 'README.txt').write_text('''Experimental AMOS Windows x86-64 build; full runtime validation pending.
Extract the whole ZIP into a writable folder. Keep SDL2.dll and AMOS next to amos.exe.
Run amos.exe to open the editor.
From PowerShell: powershell -ExecutionPolicy Bypass -File .\\smoke.ps1
The script creates test-specific configuration/devices and checks golden hashes.
Send smoke-results.txt and the smoke/*/stdout.txt and stderr.txt logs on failure.
Windows paths containing non-ASCII characters are not fully validated: several
file operations still use MinGW's narrow CRT APIs. Use an ASCII path initially.
SDL2 license: SDL2-LICENSE.txt. Original AMOS notices/resources remain in AMOS.
''')
    for license_name in ('COPYING.txt', 'LICENSE.txt', 'LICENSE'):
        source = args.sdk.parent / license_name
        if source.exists():
            shutil.copy2(source, out / 'SDL2-LICENSE.txt')
            break
    else:
        parser.error('SDL2 SDK license file not found in SDK parent directory')
    archive = shutil.make_archive(str(out), 'zip', root_dir=out.parent, base_dir=out.name)
    print(f'Windows bundle: {archive}')


def tracked(prefix):
    # Only committed files, never a developer's editor files or third-party libraries not in the repository
    names = subprocess.check_output(['git', 'ls-tree', '-rz', '--name-only', 'HEAD', '--', prefix], cwd=ROOT).decode().split('\0')
    return [n for n in names if n]


def release(args, parser):
    """amos-VERSION-windows-x64-portable.zip: unzip anywhere and run amos.exe (settings stay in its folder)."""
    for source in (args.build / 'amos.exe', args.sdk / 'bin/SDL2.dll'):
        if not source.is_file():
            parser.error(f'missing {source}')
    name = f'amos-{args.release}-windows-x64-portable'
    out = args.out.resolve()
    if not out.is_relative_to((ROOT / 'build').resolve()) or out == (ROOT / 'build').resolve():
        parser.error('--out must be a subdirectory of the repository build directory')
    stage = out / name
    if out.exists():
        shutil.rmtree(out)
    stage.mkdir(parents=True)
    shutil.copy2(args.build / 'amos.exe', stage)
    shutil.copy2(args.sdk / 'bin/SDL2.dll', stage)
    resources = stage / 'AMOS'
    for source, base in (('AMOS', 'AMOS'), ('examples', 'examples')):
        for tracked_name in tracked(source):
            relative = Path(tracked_name).relative_to(base)
            if relative.suffix.lower() in ('.info', '.uaem'):
                continue
            dest = resources / relative
            dest.parent.mkdir(parents=True, exist_ok=True)
            dest.write_bytes(subprocess.check_output(['git', 'show', 'HEAD:' + tracked_name], cwd=ROOT))
    ext_config.patch_file(resources / 'AMOSPro_Interpreter_Config')
    shutil.copy2(ROOT / 'LICENSE', stage / 'LICENSE.txt')
    for license_name in ('COPYING.txt', 'LICENSE.txt', 'LICENSE'):
        source = args.sdk.parent / license_name
        if source.exists():
            shutil.copy2(source, stage / 'SDL2-LICENSE.txt')
            break
    else:
        parser.error('SDL2 SDK license file not found in SDK parent directory')
    for font_name in ('LICENSE.AROS', 'README.md'):
        dest = stage / 'licenses/AROS-font' / font_name
        dest.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(ROOT / 'data/fonts/topaz8' / font_name, dest)
    (stage / 'README.txt').write_text(f'''AMOS Professional {args.release}, portable Windows x86-64 build. No installation needed.
Unzip the whole folder anywhere that is writable (not Program Files) and run amos.exe.
Keep SDL2.dll and the AMOS folder next to amos.exe. Settings and your programs are
kept in that folder, so moving or copying it takes them along.
Use an ASCII-only path for now: several file operations still use the narrow CRT APIs.
SDL2 license: SDL2-LICENSE.txt. Original AMOS notices/resources remain in AMOS.
''')
    archive = shutil.make_archive(str(out / name), 'zip', root_dir=out, base_dir=name)
    digest = hashlib.sha256(Path(archive).read_bytes()).hexdigest()
    Path(archive + '.sha256').write_text(f'{digest}  {Path(archive).name}\n')
    print(f'Windows portable release: {archive}')


if __name__ == '__main__':
    main()
