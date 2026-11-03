#!/usr/bin/env python3
"""Stage platform icons into a Linux install prefix or existing macOS .app."""
import argparse
from pathlib import Path
import plistlib
import shutil
import subprocess

ROOT=Path(__file__).resolve().parents[1]
NAME='io.github.dschwen.amosystem'


def linux(prefix, uninstall=False, refresh=False):
    for source in sorted((ROOT/'data/icons/hicolor').glob('*/apps/*.png')):
        target=prefix/'share/icons/hicolor'/source.relative_to(ROOT/'data/icons/hicolor')
        mime=target.parent.parent/'mimetypes'/('application-x-amos-program.png')
        for dest in (target,mime):
            if uninstall: dest.unlink(missing_ok=True)
            else:
                dest.parent.mkdir(parents=True,exist_ok=True)
                shutil.copy2(source,dest)
    for source, target in ((ROOT/'packaging/linux'/f'{NAME}.desktop',prefix/'share/applications'/f'{NAME}.desktop'),
                           (ROOT/'packaging/linux/amos.xml',prefix/'share/mime/packages/amos.xml')):
        if uninstall: target.unlink(missing_ok=True)
        else:
            target.parent.mkdir(parents=True,exist_ok=True);shutil.copy2(source,target)

    if refresh:
        for tool, directory in (('update-mime-database','share/mime'),
                                ('update-desktop-database','share/applications'),
                                ('gtk-update-icon-cache','share/icons/hicolor')):
            command=shutil.which(tool)
            if command and (prefix/directory).is_dir():
                flags=['-f','-t'] if tool=='gtk-update-icon-cache' else []
                subprocess.run([command,*flags,str(prefix/directory)],check=True)


def macos(app):
    plist=app/'Contents/Info.plist'
    # Require a pre-existing bundle; do not manufacture an incomplete app.
    with plist.open('rb') as f: info=plistlib.load(f)
    resources=app/'Contents/Resources';resources.mkdir(parents=True,exist_ok=True)
    shutil.copy2(ROOT/'data/icons/amos.icns',resources/'amos.icns')
    info['CFBundleIconFile']='amos.icns'
    for doc in info.get('CFBundleDocumentTypes',[]):
        extensions=doc.get('CFBundleTypeExtensions',[])
        if isinstance(extensions,str): extensions=[extensions]
        if any(str(ext).lower()=='amos' for ext in extensions) or 'io.github.dschwen.amosystem.program' in doc.get('LSItemContentTypes',[]):
            doc['CFBundleTypeIconFile']='amos.icns'
    for item in info.get('UTExportedTypeDeclarations',[]):
        extensions=item.get('UTTypeTagSpecification',{}).get('public.filename-extension',[])
        if isinstance(extensions,str): extensions=[extensions]
        if any(str(ext).lower()=='amos' for ext in extensions): item['UTTypeIconFile']='amos.icns'
    with plist.open('wb') as f: plistlib.dump(info,f,sort_keys=False)


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    subs=ap.add_subparsers(dest='platform',required=True)
    p=subs.add_parser('linux');p.add_argument('--prefix',type=Path,required=True);p.add_argument('--uninstall',action='store_true');p.add_argument('--refresh',action='store_true')
    p=subs.add_parser('macos');p.add_argument('--app',type=Path,required=True)
    args=ap.parse_args()
    if args.platform=='linux': linux(args.prefix,args.uninstall,args.refresh)
    else: macos(args.app)

if __name__=='__main__': main()
