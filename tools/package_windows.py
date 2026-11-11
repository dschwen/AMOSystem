#!/usr/bin/env python3
"""Package the experimental Windows host and reproducible headless smoke tests."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
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
    args = parser.parse_args()
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


if __name__ == '__main__':
    main()
