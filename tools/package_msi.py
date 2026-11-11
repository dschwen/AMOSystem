#!/usr/bin/env python3
"""Build and inspect an x64 MSI natively on Linux with wixl/msitools."""
import argparse
import copy
import csv
import hashlib
import io
import json
import os
from pathlib import Path
import re
import shutil
import struct
import subprocess
import sys
import tempfile
import uuid
import xml.etree.ElementTree as ET

import ext_config

ROOT = Path(__file__).resolve().parents[1]
NS = 'http://schemas.microsoft.com/wix/2006/wi'
ET.register_namespace('', NS)
NAMESPACE = uuid.UUID('33b6b9f6-7c37-4c3b-9fb7-682ef79fce14')


def tag(name): return '{'+NS+'}'+name

def identity(kind, name):
    return '{'+str(uuid.uuid5(NAMESPACE, kind+':'+name.casefold())).upper()+'}'


def identifier(kind, name):
    return kind + hashlib.sha256(name.casefold().encode()).hexdigest()[:30]


def version(value):
    if not re.fullmatch(r'\d+\.\d+\.\d+', value):
        raise ValueError('version must be MAJOR.MINOR.PATCH')
    major, minor, patch = map(int, value.split('.'))
    if major > 255 or minor > 255 or patch > 65535:
        raise ValueError('version exceeds MSI limits (255.255.65535)')
    return f'{major}.{minor}.{patch}'


class Tools:
    def __init__(self, local=None):
        self.local = Path(local or ROOT/'build/msi-tools/root').resolve()
        self.env = os.environ.copy()
        libs = list((self.local/'usr/lib').glob('*-linux-gnu'))
        library_paths=list(map(str,libs))+[p for p in self.env.get('LD_LIBRARY_PATH','').split(':') if p]
        if library_paths:self.env['LD_LIBRARY_PATH']=':'.join(library_paths)

    def run(self, tool, *args):
        candidate=self.local/'usr/bin'/tool
        executable = str(candidate) if candidate.is_file() else shutil.which(tool)
        if not executable or not Path(executable).is_file():
            raise ValueError(f'{tool} missing: install wixl/msitools or run make msi-tools')
        result = subprocess.run([executable, *map(str,args)], env=self.env, text=True, capture_output=True)
        if result.returncode or any(word in result.stderr for word in ('CRITICAL', 'WARNING', 'unhandled')):
            raise ValueError(f'{tool} failed or rejected authoring:\n{result.stdout}{result.stderr}')
        return result.stdout

    def table(self, msi, name):
        lines = self.run('msiinfo', 'export', msi, name).splitlines()
        return list(csv.DictReader(io.StringIO('\n'.join([lines[0], *lines[3:]])), delimiter='\t',quoting=csv.QUOTE_NONE))


def sha(path): return hashlib.sha256(path.read_bytes()).hexdigest()


def require_x64(path):
    with path.open('rb') as f:
        header = f.read(64)
        if len(header) != 64 or header[:2] != b'MZ': raise ValueError(f'not a PE file: {path}')
        f.seek(struct.unpack_from('<I', header, 60)[0])
        if f.read(6) != b'PE\0\0\x64\x86': raise ValueError(f'not Windows x86-64: {path}')


SYSTEM_DLLS = {
    'kernel32.dll','msvcrt.dll','ole32.dll','shell32.dll','advapi32.dll',
    'gdi32.dll','imm32.dll','oleaut32.dll','setupapi.dll','user32.dll',
    'version.dll','winmm.dll','ntdll.dll','comdlg32.dll','comctl32.dll',
    'ws2_32.dll','bcrypt.dll','ucrtbase.dll','dwmapi.dll','uxtheme.dll',
}


def pe_imports(path, objdump, bundled=()):
    output=subprocess.check_output([objdump,'-p',str(path)],text=True)
    imports=sorted(set(re.findall(r'DLL Name:\s*(\S+)',output)))
    unexpected=[name for name in imports if name.lower() not in SYSTEM_DLLS | set(bundled)
                and not name.lower().startswith(('api-ms-win-','ext-ms-win-'))]
    if unexpected:raise ValueError(f'unbundled DLL dependencies in {path.name}: {unexpected}')
    return imports


def stage_payload(build, sdk, stage, product_version, objdump):
    rc=build/'version.rc'
    if not rc.is_file() or f'VALUE "ProductVersion", "{product_version}\\0"' not in rc.read_text():
        raise ValueError('executable resource version does not match MSI; rebuild with VERSION='+product_version)
    require_x64(build/'amos.exe')
    require_x64(sdk/'bin/SDL2.dll')
    dependencies={'amos.exe':pe_imports(build/'amos.exe',objdump,('sdl2.dll',)),
                  'SDL2.dll':pe_imports(sdk/'bin/SDL2.dll',objdump)}
    # Independent reproduction of the fallback sources prevents packaging a
    # development binary whose generated font/keymap came from a private ROM.
    with tempfile.TemporaryDirectory(prefix='amos-rom-free-') as temp:
        for name, script, args in (
            ('topaz.cpp','extract_topaz.py',['--fallback','data/fonts/topaz8']),
            ('keymap.cpp','extract_keymap.py',[]),
        ):
            expected=Path(temp)/name
            subprocess.run([sys.executable,str(ROOT/'tools'/script),'--out',str(expected),*args],cwd=ROOT,check=True,stdout=subprocess.DEVNULL)
            if not (build/name).is_file() or (build/name).read_bytes()!=expected.read_bytes():
                raise ValueError('MSI requires the ROM-free windows-release build, not build/windows')
    stage.mkdir(parents=True)
    shutil.copy2(build/'amos.exe',stage/'amos.exe')
    shutil.copy2(build/'amos.exe',stage/'amos-player.exe')
    shutil.copy2(sdk/'bin/SDL2.dll',stage/'SDL2.dll')
    resources=stage/'AMOS';resources.mkdir()
    # Stage only committed defaults, never files created by a developer's editor.
    tracked=subprocess.check_output(['git','ls-tree','-rz','--name-only','HEAD','--','AMOS'],cwd=ROOT).decode().split('\0')
    for name in filter(None,tracked):
        relative=Path(name).relative_to('AMOS')
        if relative.as_posix()!='AMOSPro_Interpreter_Config' and relative.parts[0] not in ('APSystem','Accessories','Compiler'):
            continue
        if relative.suffix.lower() in ('.info','.uaem'):continue
        dest=resources/relative;dest.parent.mkdir(parents=True,exist_ok=True)
        dest.write_bytes(subprocess.check_output(['git','show','HEAD:'+name],cwd=ROOT))
    # AMCAF and Turbo Plus in their slots (docs/THIRD-PARTY-EXTENSIONS.md)
    ext_config.patch_file(resources/'AMOSPro_Interpreter_Config')
    # The examples (AMOSPro's disks: Examples, Productivity, Tutorial), as the Linux packages have them
    examples=subprocess.check_output(['git','ls-tree','-rz','--name-only','HEAD','--','examples'],cwd=ROOT).decode().split('\0')
    for name in filter(None,examples):
        relative=Path(name).relative_to('examples')
        if relative.suffix.lower() in ('.info','.uaem'):continue
        dest=resources/relative;dest.parent.mkdir(parents=True,exist_ok=True)
        dest.write_bytes(subprocess.check_output(['git','show','HEAD:'+name],cwd=ROOT))
    (stage/'installed.ini').write_text('AMOSystem MSI payload v1\n')
    shutil.copy2(ROOT/'LICENSE',stage/'LICENSE.txt')
    shutil.copy2(sdk.parent/'LICENSE.txt',stage/'SDL2-LICENSE.txt')
    for name in ('LICENSE.AROS','README.md','chardata.bin','charloc.bin'):
        dest=stage/'licenses/AROS-font'/name;dest.parent.mkdir(parents=True,exist_ok=True)
        shutil.copy2(ROOT/'data/fonts/topaz8'/name,dest)
    for source, target in (
        (Path('/usr/share/doc/g++-mingw-w64-x86-64-win32/copyright'), 'GCC-runtime-copyright.txt'),
        (Path('/usr/share/doc/mingw-w64-common/copyright'), 'MinGW-copyright.txt'),
        (Path('/usr/share/common-licenses/GPL-3'), 'GPL-3.txt'),
    ):
        if not source.is_file(): raise ValueError(f'missing compiler runtime license: {source}')
        (stage/'licenses').mkdir(exist_ok=True)
        shutil.copy2(source,stage/'licenses'/target)
    (stage/'README.txt').write_text('''AMOS Professional for Windows x86-64
Installed resources remain under Program Files. First launch seeds/refreshes a
per-user working resource tree in Local AppData/AMOSystem/runtime, preserving
interpreter/editor configurations and macros on upgrades. AMOS devices/config
also live under that user's AMOSystem data directory; uninstall preserves it.
.amos Open With: select AMOS Professional and Always in Windows Settings or
Explorer. Opening a document loads it into the editor without running it.
Release MSIs (and amos.exe in them) are signed through SignPath; builds made
elsewhere are unsigned. See the repository packaging/windows/README.md.
''')
    (stage/'NOTICE.txt').write_text('''AMOS Professional and C++ conversion: see LICENSE.txt.
SDL2: see SDL2-LICENSE.txt.
Topaz fallback font: AROS Public License 1.1; corresponding unmodified font
sources/data and attribution are included under licenses/AROS-font.
This build uses fallback font/keymap data; no Kickstart ROM was read.
Compiler runtime components are linked statically. Matching Debian/Ubuntu GCC
runtime copyright/exception information, MinGW notices and GPL text are included
under licenses.
''')
    return {p.relative_to(stage).as_posix():sha(p) for p in sorted(stage.rglob('*')) if p.is_file()}, dependencies


def author(stage, destination, product_version, probe=False):
    template=ROOT/'packaging/windows'/('Probe.wxs' if probe else 'Product.wxs')
    product_code=identity('probe-product' if probe else 'product-x64',product_version)
    text=template.read_text().replace('@PRODUCT_CODE@',product_code).replace('@VERSION@',product_version)
    root=ET.fromstring(text)
    for node in root.iter(tag('Icon')):node.set('SourceFile',str(ROOT/'build/windows-release/amos.ico'))
    for node in root.iter(tag('Binary')):node.set('SourceFile',str(destination.parent/'probe-failure.dll'))
    product=root.find(tag('Product'))
    install=next(d for d in root.iter(tag('Directory')) if d.get('Id')=='INSTALLDIR')
    feature=product.find(tag('Feature'))
    directories={'':install};components=[]
    def directory(path):
        if path in directories:return directories[path]
        parent,_,name=path.rpartition('/')
        directories[path]=ET.SubElement(directory(parent),tag('Directory'),Id=identifier('D',path),Name=name)
        return directories[path]
    def component(parent, logical):
        cid=identifier('C',('probe/' if probe else '')+logical)
        result=ET.SubElement(parent,tag('Component'),Id=cid,Guid=identity('probe-component' if probe else 'component-x64',logical),Win64='yes')
        components.append(cid);return result
    seen=set()
    for path in sorted(stage.rglob('*')):
        if path.is_symlink():raise ValueError(f'symlink in payload: {path}')
        if not path.is_file():continue
        relative=path.relative_to(stage).as_posix()
        if relative.casefold() in seen:raise ValueError('case-colliding payload paths')
        seen.add(relative.casefold())
        parent,_,name=relative.rpartition('/')
        node=component(directory(parent),relative)
        fid='MainExecutable' if relative=='amos.exe' else identifier('F',relative)
        ET.SubElement(node,tag('File'),Id=fid,Name=name,Source=str(path),KeyPath='yes')
    for path, node in sorted(directories.items()):
        cleanup=component(node,':directory:'+path)
        ET.SubElement(cleanup,tag('CreateFolder'))
        ET.SubElement(cleanup,tag('RemoveFolder'),Id=identifier('R',path or 'install-root'),On='uninstall')
    menu=next(d for d in root.iter(tag('Directory')) if d.get('Id')=='ApplicationProgramsFolder')
    meta=component(menu,'application-registration')
    if probe:
        ET.SubElement(meta,tag('RegistryValue'),Root='HKLM',Key='Software\\AMOSystem\\InstallerProbe',Name='Version',Type='string',Value='[ProductVersion]',KeyPath='yes')
    else:
        for entry in ET.parse(ROOT/'packaging/windows/associations.wxi').getroot():
            meta.append(copy.deepcopy(entry))
    ET.SubElement(meta,tag('Shortcut'),Id='StartMenuShortcut',Directory='ApplicationProgramsFolder',Name='AMOS MSI Probe' if probe else 'AMOS Professional',Target='[INSTALLDIR]one.txt' if probe else '[#MainExecutable]',WorkingDirectory='INSTALLDIR',**({} if probe else {'Icon':'AMOSIcon'}))
    ET.SubElement(meta,tag('RemoveFolder'),Id='RemoveApplicationMenu',On='uninstall')
    for cid in components:ET.SubElement(feature,tag('ComponentRef'),Id=cid)
    ET.indent(root,space='  ')
    ET.ElementTree(root).write(destination,encoding='utf-8',xml_declaration=True)
    return product_code


def inspect(tools, msi, manifest, product_version, probe=False):
    summary=tools.run('msiinfo','suminfo',msi)
    if 'Template: x64;1033' not in summary:raise ValueError('MSI is not x64')
    tables={name:tools.table(msi,name) for name in ('Property','Directory','File','Component','Registry','Shortcut','Upgrade','InstallExecuteSequence','LaunchCondition','Media','RemoveFile','CreateFolder')}
    props={r['Property']:r['Value'] for r in tables['Property']}
    if props.get('ProductVersion')!=product_version or props.get('ALLUSERS')!='1':raise ValueError('wrong version/scope')
    if not all(int(c['Attributes'])&256 for c in tables['Component']):raise ValueError('non-x64 component')
    sequence={r['Action']:int(r['Sequence']) for r in tables['InstallExecuteSequence']}
    if not sequence['InstallInitialize']<sequence['RemoveExistingProducts']<sequence['InstallFinalize']:raise ValueError('upgrade removal is outside transaction')
    if not any('WIX_DOWNGRADE_DETECTED' in r['Condition'] for r in tables['LaunchCondition']):raise ValueError('downgrade not blocked')
    if not any(r['DirProperty']=='INSTALLDIR' and not r['FileName'] and r['InstallMode']=='2' for r in tables['RemoveFile']):
        raise ValueError('missing nonrecursive installation-folder cleanup')
    if not tables['Upgrade'] or not tables['Shortcut']:raise ValueError('missing upgrade/shortcut metadata')
    if len(tables['File'])!=len(manifest):raise ValueError('MSI file count differs')
    if not probe:
        present=set(tools.run('msiinfo','tables',msi).splitlines())
        if any(name in present and tools.table(msi,name) for name in ('CustomAction','Binary')):
            raise ValueError('unexpected custom-action code in AMOS installer')
        regs=tables['Registry']
        if any('UserChoice' in r['Key'] for r in regs) or any(r['Name'] in ('*','-') for r in regs):raise ValueError('unsafe registry ownership')
        command=[r['Value'] for r in regs if r['Key'].endswith('AMOSystem.Program\\shell\\open\\command')]
        if command!=['"[INSTALLDIR]amos.exe" --open-document "%1"']:raise ValueError('wrong document handler')
    with tempfile.TemporaryDirectory(prefix='amos-msi-extract-') as temp:
        tools.run('msiextract','-C',temp,msi)
        install=Path(temp)/('AMOS MSI Probe' if probe else 'AMOSystem')
        actual={p.relative_to(install).as_posix():sha(p) for p in install.rglob('*') if p.is_file()}
        if actual!=manifest:raise ValueError('extracted MSI payload differs from manifest')
    return tables


# The files the release signing (SignPath, .github/workflows/windows-msi.yml) signs inside the MSI;
# the rest of the payload (SDL2.dll, the resources) must come back unchanged
SIGNED_PAYLOAD = ('amos.exe', 'amos-player.exe')


def record_signed(tools, msi, report_path):
    """The report of a signed MSI: its payload against the unsigned build's, then the new hashes."""
    report=json.loads(report_path.read_text())
    with tempfile.TemporaryDirectory(prefix='amos-msi-signed-') as temp:
        tools.run('msiextract','-C',temp,msi)
        install=Path(temp)/'AMOSystem'
        actual={p.relative_to(install).as_posix():sha(p) for p in install.rglob('*') if p.is_file()}
    before=report['payload']
    if set(actual)!=set(before):raise ValueError('signed MSI has a different file list')
    changed={name for name in actual if actual[name]!=before[name]}
    if changed!=set(SIGNED_PAYLOAD):raise ValueError(f'signing changed {sorted(changed)}, expected {list(SIGNED_PAYLOAD)}')
    report.update(sha256=sha(msi),payload=actual,signed=True,unsigned_sha256=report['sha256'])
    report_path.write_text(json.dumps(report,indent=2)+'\n')
    print(f'Signed MSI recorded: {msi} ({", ".join(SIGNED_PAYLOAD)} signed inside)')


def build(args):
    product_version=version(args.version)
    tools=Tools(args.tools_root)
    args.out.mkdir(parents=True,exist_ok=True)
    stem=('amos-msi-probe-' if args.feature_probe else 'amos-')+product_version+'-windows-x64'
    stage=args.out/(stem+'-stage')
    source=args.out/(stem+'.wxs');msi=args.out/(stem+'.msi')
    report_path=args.out/(stem+'.json')
    report_path.unlink(missing_ok=True);msi.unlink(missing_ok=True)
    if stage.exists():shutil.rmtree(stage)
    if args.feature_probe:
        subprocess.run([args.compiler, '-shared', '-nostdlib', '-Wl,--entry,0', '-o',str(args.out/'probe-failure.dll'),str(ROOT/'packaging/windows/probe_failure.c')],check=True)
        require_x64(args.out/'probe-failure.dll')
        stage.mkdir()
        for name in ('one.txt','two.txt'):(stage/name).write_text('AMOS MSI probe '+product_version+'\n'+name+'\n')
        manifest={p.name:sha(p) for p in stage.iterdir()};dependencies={}
    else:manifest,dependencies=stage_payload(args.build,args.sdk,stage,product_version,args.objdump)
    product_code=author(stage,source,product_version,args.feature_probe)
    try:
        tools.run('wixl','-a','x64','-o',msi,source)
        tables=inspect(tools,msi,manifest,product_version,args.feature_probe)
    except (ValueError,OSError):
        msi.unlink(missing_ok=True)
        raise
    try:revision=subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip()
    except subprocess.CalledProcessError:revision='unknown'
    report={'version':product_version,'product_code':product_code,'sha256':sha(msi),'payload':manifest,'tables':tables,'tool':tools.run('wixl','--version').strip(),'source_revision':revision,'runtime_validation':'pending Windows VM logs','signed':False}
    if args.feature_probe:report['probe_failure_dll_sha256']=sha(args.out/'probe-failure.dll')
    report['imports']=dependencies
    report['source_dirty']=bool(subprocess.check_output(['git','status','--porcelain'],cwd=ROOT,text=True).strip())
    report_path.write_text(json.dumps(report,indent=2)+'\n')
    shutil.copy2(ROOT/'tests/windows/test_msi.ps1',args.out/'test_msi.ps1')
    if not args.feature_probe:
        import amostok
        tables=amostok.Tables([str(ROOT/'AMOS/APSystem')])
        listing='Open Out 1,"START:document-executed.txt" : Print #1,"This must not run when opening a document" : Close 1\nSystem\n'
        amostok.tokenize_program(listing,tables).save(str(args.out/'MSI document with spaces.AMOS'))
    print(f'MSI built and extracted payload verified: {msi}')
    print(f'ProductCode: {product_code}; {len(manifest)} files; Windows install/upgrade tests pending')


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--version',default='0.1.0')
    ap.add_argument('--feature-probe',action='store_true')
    ap.add_argument('--build',type=Path,default=ROOT/'build/windows-release')
    ap.add_argument('--sdk',type=Path,default=Path.home()/'Downloads/SDL2-2.32.8/x86_64-w64-mingw32')
    ap.add_argument('--out',type=Path,default=ROOT/'build/packages')
    ap.add_argument('--tools-root',type=Path)
    ap.add_argument('--compiler',default='x86_64-w64-mingw32-gcc')
    ap.add_argument('--objdump',default='x86_64-w64-mingw32-objdump')
    ap.add_argument('--record-signed',type=Path,metavar='MSI',help='update the report beside a signed MSI (no build)')
    args=ap.parse_args()
    if args.record_signed:
        try:record_signed(Tools(args.tools_root),args.record_signed,args.record_signed.with_suffix('.json'))
        except (ValueError,OSError,KeyError) as exc:ap.exit(1,str(exc)+'\n')
        return
    args.build=args.build.resolve();args.out=args.out.resolve();args.sdk=args.sdk.resolve()
    if not args.out.is_relative_to((ROOT/'build').resolve()) or args.out==(ROOT/'build').resolve():ap.error('--out must be a subdirectory of build')
    try:build(args)
    except (ValueError,OSError,subprocess.CalledProcessError) as exc:ap.exit(1,str(exc)+'\n')

if __name__=='__main__':main()
