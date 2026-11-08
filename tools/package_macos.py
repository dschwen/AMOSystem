#!/usr/bin/env python3
"""Build an Intel or Apple Silicon AMOS .app and compressed DMG on macOS (no ROM required)."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import plistlib
import re
import shutil
import subprocess
import sys
import tempfile
import time
from install_icons import macos as install_icons

ROOT = Path(__file__).resolve().parents[1]
APP = 'AMOS Professional.app'
UTI = 'io.github.dschwen.amosystem.program'


def run(*args):
    command=list(map(str,args))
    result=subprocess.run(command,text=True,capture_output=True,cwd=ROOT)
    if result.returncode:
        raise subprocess.CalledProcessError(result.returncode,command,output=result.stdout+result.stderr)
    # hdiutil -plist must stay pure XML; do not merge progress from stderr.
    return result.stdout.strip()


def validate_version(value):
    if not re.fullmatch(r'\d+\.\d+\.\d+',value): raise ValueError('version must be MAJOR.MINOR.PATCH')
    return '.'.join(str(int(n)) for n in value.split('.'))


def validate_framework(framework):
    if not (framework/'SDL2').is_file(): raise ValueError('missing SDL2.framework/SDL2')
    base=framework.resolve()
    for path in framework.rglob('*'):
        if path.is_symlink() and (not path.exists() or not path.resolve().is_relative_to(base)):
            raise ValueError(f'broken or external framework symlink: {path}')


def verify_binary(path, framework=False, arch='x86_64'):
    archs=run('/usr/bin/lipo','-archs',path).split()
    if arch not in archs or (not framework and archs != [arch]):
        raise ValueError(f'{path} must contain {arch} (the app is {arch}-only)')
    imports=[]
    for line in run('/usr/bin/otool','-arch',arch,'-L',path).splitlines()[1:]:
        name=line.strip().split(' (',1)[0]
        if framework and name.startswith('@rpath/SDL2.framework/'): continue # framework's own ID
        if not name.startswith(('/System/Library/','/usr/lib/')):
            raise ValueError(f'unbundled dependency in {path}: {name}')
        imports.append(name)
    return imports


def verify_fallback(build):
    with tempfile.TemporaryDirectory(prefix='amos-rom-free-') as temp:
        for name, script, options in (
            ('topaz.cpp','extract_topaz.py',['--fallback','data/fonts/topaz8']),
            ('keymap.cpp','extract_keymap.py',[])):
            expected=Path(temp)/name
            subprocess.run([sys.executable,str(ROOT/'tools'/script),'--out',str(expected),*options],cwd=ROOT,check=True,stdout=subprocess.DEVNULL)
            if not (build/name).is_file() or expected.read_bytes()!=(build/name).read_bytes():
                raise ValueError('use the ROM-free macos-release build')


def stage_bundle(build, framework, stage, version, arch='x86_64'):
    validate_framework(framework)
    app=stage/APP
    binary=app/'Contents/MacOS/amos';binary.parent.mkdir(parents=True)
    shutil.copy2(build/'amos',binary);binary.chmod(0o755)
    shutil.copy2(binary,binary.with_name('amos-player'))
    resources=app/'Contents/Resources';resources.mkdir()
    defaults=resources/'AMOS';defaults.mkdir()
    # Never ship local editor state, untracked user files or the Amiga binary.
    tracked=run('git','ls-tree','-rz','--name-only','HEAD','--','AMOS').split('\0')
    for name in filter(None,tracked):
        rel=Path(name).relative_to('AMOS')
        if rel.as_posix()!='AMOSPro_Interpreter_Config' and rel.parts[0] not in ('APSystem','Accessories','Compiler'): continue
        if rel.suffix.lower() in ('.info','.uaem'): continue
        dest=defaults/rel;dest.parent.mkdir(parents=True,exist_ok=True)
        dest.write_bytes(subprocess.check_output(['git','show','HEAD:'+name],cwd=ROOT))
    # The examples (AMOSPro's disks: Examples, Productivity, Tutorial), as the Linux packages have them
    for name in filter(None,run('git','ls-tree','-rz','--name-only','HEAD','--','examples').split('\0')):
        rel=Path(name).relative_to('examples')
        if rel.suffix.lower() in ('.info','.uaem'): continue
        dest=defaults/rel;dest.parent.mkdir(parents=True,exist_ok=True)
        dest.write_bytes(subprocess.check_output(['git','show','HEAD:'+name],cwd=ROOT))
    frameworks=app/'Contents/Frameworks';frameworks.mkdir()
    shutil.copytree(framework,frameworks/'SDL2.framework',symlinks=True)
    licenses=resources/'licenses';licenses.mkdir()
    shutil.copy2(ROOT/'LICENSE',licenses/'AMOS-LICENSE.txt')
    shutil.copy2(ROOT/'packaging/macos/SDL2-LICENSE.txt',licenses/'SDL2-LICENSE.txt')
    shutil.copytree(ROOT/'data/fonts/topaz8',licenses/'AROS-font')
    info={
        'CFBundleIdentifier':'io.github.dschwen.amosystem',
        'CFBundleName':'AMOS Professional', 'CFBundleDisplayName':'AMOS Professional',
        'CFBundleExecutable':'amos', 'CFBundlePackageType':'APPL',
        'CFBundleShortVersionString':version, 'CFBundleVersion':version,
        'LSMinimumSystemVersion':'11.0', 'LSArchitecturePriority':[arch],
        'NSHighResolutionCapable':True,
        # .AMOS (and .amos) programs open in the editor: Finder's double click, Open With, the Dock
        # (a running AMOS starts another for the file, host/open_document.cpp). The type is ours.
        'CFBundleDocumentTypes':[{
            'CFBundleTypeName':'AMOS BASIC program', 'CFBundleTypeRole':'Editor', 'LSHandlerRank':'Owner',
            'LSItemContentTypes':[UTI], 'CFBundleTypeExtensions':['AMOS','amos'],
        }],
        'UTExportedTypeDeclarations':[{
            'UTTypeIdentifier':UTI, 'UTTypeDescription':'AMOS BASIC program',
            'UTTypeConformsTo':['public.data'],
            'UTTypeTagSpecification':{'public.filename-extension':['AMOS','amos']},
        }],
    }
    (app/'Contents/Info.plist').write_bytes(plistlib.dumps(info))
    install_icons(app)
    (stage/'Applications').symlink_to('/Applications',target_is_directory=True)
    machine='Apple Silicon' if arch=='arm64' else 'Intel'
    caveat=('This bundle is Apple Silicon-only (native arm64); Intel Macs need the x86_64 DMG.\n' if arch=='arm64' else
            'This bundle is Intel-only. Apple Silicon requires Rosetta and remains unqualified.\n')
    (stage/'Read Me.txt').write_text(f'''AMOS Professional for {machine} macOS 11 or later
Drag AMOS Professional.app to Applications, then launch it from Finder.
SDL2 is bundled; no global SDL installation, Python or compiler is required.
Default resources are mirrored into ~/Library/Application Support/AMOSystem/runtime
on launch. Settings, macros and existing BASIC scripts in that mirror are retained
on upgrade. Deleting the app does not delete your projects or preferences.

{caveat}A locally ad-hoc signed build is not notarized and may be blocked by Gatekeeper.
Use a Developer ID signed/notarized build for public distribution.
Double-clicking an .AMOS program opens it in the editor (Open With > AMOS Professional;
to make AMOS the default: Get Info > Open with > AMOS Professional > Change All).
From Terminal: amos --open-document file.AMOS.
Licenses and redistributable AROS font sources/data are inside Contents/Resources.
''')
    return app


def bundle_manifest(app):
    return {p.relative_to(app).as_posix():hashlib.sha256(p.read_bytes()).hexdigest()
            for p in sorted(app.rglob('*')) if p.is_file() and not p.is_symlink()}


def verify_image(image, app):
    attached=plistlib.loads(run('/usr/bin/hdiutil','attach','-readonly','-nobrowse','-plist',image).encode())
    mount=next((Path(item['mount-point']) for item in attached.get('system-entities',[]) if 'mount-point' in item),None)
    if mount is None: raise ValueError('disk image did not expose a mounted volume')
    try:
        if bundle_manifest(mount/APP)!=bundle_manifest(app): raise ValueError('mounted DMG app differs from staged app')
        for p in app.rglob('*'):
            if p.is_symlink() and os.readlink(mount/APP/p.relative_to(app))!=os.readlink(p):
                raise ValueError('mounted framework symlink differs from staged app')
        if os.readlink(mount/'Applications')!='/Applications': raise ValueError('Applications shortcut missing')
        run('/usr/bin/codesign','--verify','--deep','--strict',mount/APP)
    finally: run('/usr/bin/hdiutil','detach',mount)


def build(args):
    if platform.system()!='Darwin': raise ValueError('DMG creation requires macOS with Command Line Tools')
    version=validate_version(args.version)
    out=args.out.resolve()
    if not out.is_relative_to((ROOT/'build').resolve()) or out==(ROOT/'build').resolve():
        raise ValueError('--out must be inside the repository build directory')
    if out==args.build.resolve() or out in args.build.resolve().parents:
        raise ValueError('--out must not contain the input build')
    validate_framework(args.framework)
    verify_fallback(args.build)
    arch=args.arch
    dependencies={'amos':verify_binary(args.build/'amos',arch=arch), 'SDL2':verify_binary(args.framework/'SDL2',True,arch)}
    load_commands=run('/usr/bin/otool','-l',args.build/'amos')
    minimum=re.search(r'\bminos\s+([0-9.]+)',load_commands)
    if minimum and tuple(map(int,minimum[1].split('.'))) > (11,0,0):
        raise ValueError('app deployment target exceeds macOS 11; rebuild with MACOSX_DEPLOYMENT_TARGET=11.0')
    out.mkdir(parents=True,exist_ok=True)
    stem=f'amos-{version}-macos-{arch}';stage=out/(stem+'-stage')
    for path in (args.build.resolve(),args.framework.resolve()):
        if stage==path or stage in path.parents: raise ValueError('stage must not contain packaging inputs')
    if stage.exists(): shutil.rmtree(stage)
    stage.mkdir()
    app=stage_bundle(args.build,args.framework,stage,version,arch)
    identity=args.sign_identity
    signing=['--force','--sign',identity]
    if identity!='-':signing+=['--timestamp','--options','runtime']
    else:signing+=['--timestamp=none']
    framework=app/'Contents/Frameworks/SDL2.framework'
    run('/usr/bin/codesign',*signing,framework)
    run('/usr/bin/codesign',*signing,app/'Contents/MacOS/amos-player')
    run('/usr/bin/codesign',*signing,app)
    run('/usr/bin/codesign','--verify','--deep','--strict',app)
    dmg=out/(stem+'.dmg');pending=out/(stem+'.pending.dmg')
    pending.unlink(missing_ok=True)
    try:
        # (hdiutil now and then fails with "Resource busy" on a busy machine: tried three times)
        for attempt in range(3):
            try:
                run('/usr/bin/hdiutil','create','-volname','AMOS Professional','-srcfolder',stage,'-format','UDZO','-fs','HFS+','-ov',pending)
                break
            except subprocess.CalledProcessError:
                if attempt == 2: raise
                time.sleep(5)
        if identity!='-':run('/usr/bin/codesign','--force','--sign',identity,'--timestamp',pending)
        run('/usr/bin/hdiutil','verify',pending)
        verify_image(pending,app)
        os.replace(pending,dmg)
    finally: pending.unlink(missing_ok=True)
    manifest=bundle_manifest(app)
    report={'version':version,'sha256':hashlib.sha256(dmg.read_bytes()).hexdigest(),
            'source_revision':run('git','rev-parse','HEAD'),'source_dirty':bool(run('git','status','--porcelain')),
            'architecture':arch,'sign_identity':identity,'notarized':False,
            'dependencies':dependencies,'app_files':manifest,'runtime_validation':'pending'}
    (out/(stem+'.json')).write_text(json.dumps(report,indent=2)+'\n')
    print(f'Created {dmg}; app signature and disk image verified; runtime/notarization pending')


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--version',required=True)
    ap.add_argument('--build',type=Path,default=ROOT/'build/macos-release-x86_64')
    ap.add_argument('--framework',type=Path,default=Path('/Library/Frameworks/SDL2.framework'))
    ap.add_argument('--out',type=Path,default=ROOT/'build/packages')
    ap.add_argument('--sign-identity',default='-')
    ap.add_argument('--arch',choices=('x86_64','arm64'),default='x86_64')
    args=ap.parse_args()
    try:build(args)
    except subprocess.CalledProcessError as e:ap.exit(1,f'{e}\n{e.output or ""}\n')
    except (ValueError,OSError) as e:ap.exit(1,f'{e}\n')

if __name__=='__main__':main()
