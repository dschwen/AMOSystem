#!/usr/bin/env python3
"""Build Darwin probes/AMOS with native Linux tools in a pinned OSXCross image."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import struct
import subprocess
import sys
import tarfile

ROOT=Path(__file__).resolve().parents[1]
IMAGE='amos-macos-cross:14.5'
CPUS={'arm64':0x0100000c,'x86_64':0x01000007}


def macho(path):
    data=path.read_bytes()
    if len(data)<32 or struct.unpack_from('<I',data)[0]!=0xfeedfacf: raise ValueError('expected thin 64-bit Mach-O: '+str(path))
    _,cpu,subtype,kind,count,command_bytes,flags,_=struct.unpack_from('<8I',data)
    if 32+command_bytes>len(data): raise ValueError('truncated Mach-O load commands')
    arch=next((name for name,value in CPUS.items() if value==cpu),None)
    if not arch: raise ValueError('unexpected Mach-O CPU type')
    if kind!=2: raise ValueError('expected executable Mach-O')
    if arch=='arm64' and subtype&0xffffff==2: raise ValueError('arm64e is not the ordinary arm64 target')
    segments=[];offset=32;minimum=None
    for _ in range(count):
        if offset+8>32+command_bytes: raise ValueError('truncated load command')
        cmd,size=struct.unpack_from('<II',data,offset)
        if size<8 or offset+size>32+command_bytes: raise ValueError('invalid load command size')
        if cmd==0x19:
            if size<72: raise ValueError('invalid segment command')
            name=data[offset+8:offset+24].split(b'\0')[0].decode()
            address,length=struct.unpack_from('<QQ',data,offset+24)
            segments.append({'name':name,'address':address,'size':length})
        if cmd==0x32:
            if size<24: raise ValueError('invalid build-version command')
            packed=struct.unpack_from('<I',data,offset+12)[0]
            minimum=f'{packed>>16}.{(packed>>8)&255}.{packed&255}'
        offset+=size
    guest_segments=[s for s in segments if s['name']=='__TEXT' or s['name'].startswith('__DATA')]
    return {'architecture':arch,'file_type':kind,'pie':bool(flags&0x200000),
            'deployment_target':minimum,'segments':segments,
            'image_below_4gb':bool(guest_segments) and all(s['address']+s['size']<=0x100000000 for s in guest_segments),
            'sha256':hashlib.sha256(data).hexdigest()}


def command(*args):
    return subprocess.check_output(list(map(str,args)),text=True,stderr=subprocess.STDOUT,cwd=ROOT).strip()


def compiler(arch):
    prefix='arm64' if arch=='arm64' else 'x86_64'
    candidates=sorted(Path('/osxcross/bin').glob(prefix+'-apple-darwin*-clang++'))
    if len(candidates)!=1: raise ValueError('cannot uniquely identify '+prefix+' OSXCross compiler')
    return str(candidates[0])


def inside(args):
    out=ROOT/'build/macos-cross'/args.arch;out.mkdir(parents=True,exist_ok=True)
    cxx=compiler(args.arch)
    layout=['-Wl,-pie'] if args.arch=='arm64' else ['-Wl,-no_pie,-pagezero_size,0x1000']
    base=[cxx,'-arch',args.arch,'-std=c++20','-mmacosx-version-min=11.0',*layout]
    metadata={'image':args.image,'image_id':os.environ.get('AMOS_CROSS_IMAGE_ID','unknown'),'architecture':args.arch,'compiler':cxx,
              'deployment_target':'11.0','compiler_version':command(cxx,'--version'),'compiler_target':command(cxx,'-dumpmachine'),
              'osxcross':command('osxcross-conf'),'runtime_validation':'pending',
              'source_revision':command('git','rev-parse','HEAD'),
              'source_dirty':bool(command('git','status','--porcelain')),
              'packages':command('dpkg-query','-W','clang','lld','llvm','libc6-dev')}
    (out/'toolchain.json').write_text(json.dumps(metadata,indent=2)+'\n')
    if args.mode=='probes':
        dest=out/'probes';dest.mkdir(exist_ok=True);results={};failures=0
        (dest/'cross-report.json').unlink(missing_ok=True)
        for name,source in (('address-probe','address_probe.cpp'),('layout-probe','layout_probe.cpp'),('context-probe','context_probe.cpp'),('jump-probe','local_jump_probe.cpp'),('guest-memory-probe','guest_memory_probe.cpp')):
            flags=['-fpack-struct=2'] if name in ('layout-probe','jump-probe') else []
            binary=dest/name;binary.unlink(missing_ok=True)
            log=dest/(name+'-build.log')
            extra_sources = [str(ROOT/'src/host/guest_memory.cpp')] if name=='guest-memory-probe' else []
            # Translated memory needs no low native image, even for the Intel control.
            probe_base = [cxx,'-arch',args.arch,'-std=c++20','-mmacosx-version-min=11.0','-Wl,-pie'] if name=='guest-memory-probe' else base
            with log.open('w') as stream:
                result=subprocess.run([*probe_base,'-O2',*flags,'-Isrc',str(ROOT/'tests/macos'/source),*extra_sources,'-o',str(binary)],cwd=ROOT,stdout=stream,stderr=subprocess.STDOUT)
            if result.returncode:
                failures+=1;results[name]={'compiled':False,'log':log.name};print(log.read_text()[-12000:])
            else:
                info=macho(binary)
                if info['architecture']!=args.arch: raise ValueError('wrong probe architecture')
                results[name]={'compiled':True,**info}
        shutil.copy2(ROOT/'tests/macos/run-probes.sh',dest/'run-probes.sh')
        (dest/'cross-report.json').write_text(json.dumps({'toolchain':metadata,'probes':results},indent=2)+'\n')
        print('Probe artifacts:',dest)
        return bool(failures)
    # Do not mix compiler/SDK/deployment settings in cached objects. Source
    # edits still use ordinary Make dependencies; only ABI changes clear objects.
    configuration={key:metadata[key] for key in ('image_id','architecture','compiler','compiler_version','osxcross','deployment_target')}
    stamp=out/'build-configuration.json'
    if not stamp.exists() or json.loads(stamp.read_text())!=configuration:
        if (out/'obj').exists():shutil.rmtree(out/'obj')
    stamp.write_text(json.dumps(configuration,indent=2)+'\n')
    (out/'amos').unlink(missing_ok=True)
    (out/'cross-report.json').unlink(missing_ok=True)
    log=out/'build.log'
    ld='-Wl,-pie' if args.arch=='arm64' else '-Wl,-no_pie,-pagezero_size,0x1000'
    build=['make',f'-j{args.jobs}','host',f'HOST=build/macos-cross/{args.arch}',
           f'HOST_CXX={cxx}','HOST_TARGET_OS=Darwin',f'HOST_TARGET_ARCH={args.arch}',
           f'HOST_ARCHFLAGS=-arch {args.arch}',f'HOST_LDFLAGS={ld}',
           'HOST_OPT=-O2','KICKSTART=','ROMKEY=','NDK_INCLUDE=/ndk']
    with log.open('w') as stream:
        environment=os.environ.copy();environment['MACOSX_DEPLOYMENT_TARGET']='11.0'
        result=subprocess.run(build,cwd=ROOT,env=environment,stdout=stream,stderr=subprocess.STDOUT)
    report={'toolchain':metadata,'linked':result.returncode==0,'build_log':log.name}
    if result.returncode:
        print(log.read_text()[-16000:])
    else:
        report['executable']=macho(out/'amos')
        if report['executable']['architecture']!=args.arch: raise ValueError('wrong application architecture')
        if not report['executable']['image_below_4gb']:
            print('WARNING: Mach-O text/data exceed 32 bits; current AMOS pointer model cannot execute this image safely.')
        print('Experimental cross-linked executable:',out/'amos')
    (out/'cross-report.json').write_text(json.dumps(report,indent=2)+'\n')
    return result.returncode


def main():
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('mode',choices=('toolchain','probes','host'))
    ap.add_argument('--arch',choices=tuple(CPUS),default='arm64')
    ap.add_argument('--image',default=IMAGE)
    ap.add_argument('--inside',action='store_true',help=argparse.SUPPRESS)
    ap.add_argument('--jobs',type=int,default=4)
    args=ap.parse_args()
    if args.jobs<1:ap.error('--jobs must be positive')
    try:
        if args.inside:return inside(args)
        if args.mode=='toolchain':
            return subprocess.call(['docker','build','--progress=plain','-t',args.image,'-f',str(ROOT/'packaging/macos-cross/Dockerfile'),str(ROOT/'packaging/macos-cross')])
        out=ROOT/'build/macos-cross';out.mkdir(parents=True,exist_ok=True)
        image_id=command('docker','image','inspect',args.image,'--format','{{.Id}}')
        docker=['docker','run','--rm','--network','none','--user',f'{os.getuid()}:{os.getgid()}',
                '-e','AMOS_CROSS_IMAGE_ID='+image_id,'-e','GIT_CONFIG_COUNT=1','-e','GIT_CONFIG_KEY_0=safe.directory','-e','GIT_CONFIG_VALUE_0=/src',
                '-v',f'{ROOT}:/src:ro','-v',f'{out}:/src/build/macos-cross:rw']
        if args.mode=='host':
            ndk=Path(os.environ.get('NDK_INCLUDE','/opt/amiga/m68k-amigaos/ndk-include'))
            if not (ndk/'exec/types.h').is_file():
                ndk=ROOT/'build/ndk/Include_H'
                if not (ndk/'exec/types.h').is_file(): subprocess.run(['make','ndk'],cwd=ROOT,check=True)
            docker+=['-v',f'{ndk.resolve()}:/ndk:ro']
        docker += [args.image,'python3','tools/macos_cross.py',args.mode,'--inside','--arch',args.arch,'--image',args.image,'--jobs',str(args.jobs)]
        archive_path=ROOT/'build/packages'/f'amos-macos-{args.arch}-probes.tar.gz'
        if args.mode=='probes': archive_path.unlink(missing_ok=True)
        status=subprocess.call(docker)
        if status==0 and args.mode=='probes':
            folder=out/args.arch/'probes'
            report=json.loads((folder/'cross-report.json').read_text())
            archive_path.parent.mkdir(parents=True,exist_ok=True)
            names=['cross-report.json','run-probes.sh']
            for name in report['probes']:names.extend((name,name+'-build.log'))
            with tarfile.open(archive_path,'w:gz') as archive:
                for name in names:archive.add(folder/name,arcname=f'macos-{args.arch}-probes/{name}')
            print('Native Mac probe bundle:',archive_path)
        return status
    except subprocess.CalledProcessError as e:
        print(e.output or str(e),file=sys.stderr);return 1
    except (OSError,ValueError) as e: print(e,file=sys.stderr);return 1

if __name__=='__main__':sys.exit(main())
