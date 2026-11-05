"""Mach-O inspection and local recovery backends without Docker/SDK dependencies."""
from pathlib import Path
import os
import shutil
import shlex
import struct
import subprocess
import sys
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
sys.path.insert(0,str(ROOT/'tools'))
import macos_cross

class MachOTests(unittest.TestCase):
    def test_arm64_image_reports_high_address_gate(self):
        segment=struct.pack('<II16sQQQQIIII',0x19,72,b'__TEXT',0x100000000,0x4000,0,0x4000,5,5,0,0)
        version=struct.pack('<6I',0x32,24,1,0x000b0000,0x000e0500,0)
        header=struct.pack('<8I',0xfeedfacf,macos_cross.CPUS['arm64'],0,2,2,len(segment+version),0x200000,0)
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp)/'app';p.write_bytes(header+segment+version)
            info=macos_cross.macho(p)
            self.assertEqual(info['architecture'],'arm64');self.assertTrue(info['pie'])
            self.assertFalse(info['image_below_4gb']);self.assertEqual(info['deployment_target'],'11.0.0')
            for bad in (b'\x7fELF',header+segment[:10],header+struct.pack('<II',0x19,0)+bytes(88)):
                p.write_bytes(bad)
                with self.assertRaises(ValueError):macos_cross.macho(p)

class LocalJumpTests(unittest.TestCase):
    def test_builtin_and_native_recovery_optimized_and_debug(self):
        compiler=shlex.split(os.environ.get('CXX','c++'))
        if not shutil.which(compiler[0]):self.skipTest('requires C++ compiler')
        with tempfile.TemporaryDirectory() as temp:
            for flags in ([],['-DAMOS_STANDARD_LOCAL_JUMPS=1']):
                for opt in ('-O0','-O2'):
                    with self.subTest(backend=flags,opt=opt):
                        binary=Path(temp)/'jumps'
                        subprocess.run([*compiler,'-std=c++20','-fpack-struct=2',opt,*flags,'-Isrc','tests/macos/local_jump_probe.cpp','-o',str(binary)],cwd=ROOT,check=True)
                        result=subprocess.check_output([binary],text=True)
                        self.assertIn('"nested_local_jumps":true',result)
