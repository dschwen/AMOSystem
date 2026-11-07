"""Live a3/a4/a5 retain guest data addresses with a stable packed context ABI."""
from pathlib import Path
import os
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
class DataZoneTests(unittest.TestCase):
    def setUp(self):
        self.compiler=shlex.split(os.environ.get('CXX','c++'))
        if not shutil.which(self.compiler[0]):self.skipTest('requires C++ compiler')

    def test_register_transfer_capture_and_identity_overlay(self):
        with tempfile.TemporaryDirectory() as temp:
            for opt in ('-O0','-O2'):
                for packing in ([],['-fpack-struct=2']):
                    binary=Path(temp)/'data-zone'
                    subprocess.run([*self.compiler,'-std=c++20',opt,*packing,'-Wall','-Wextra','-Werror','-Isrc',
                                    'tests/guest/data_zone.cpp','src/host/guest_memory.cpp','-o',str(binary)],cwd=ROOT,check=True)
                    subprocess.run([str(binary)],check=True)

    def test_biased_stack_words_use_guest_offsets(self):
        with tempfile.TemporaryDirectory() as temp:
            for opt in ('-O0','-O2'):
                binary=Path(temp)/'biased-cursors'
                subprocess.run([*self.compiler,'-std=c++20',opt,'-fpack-struct=2','-DAMOS_GUEST_BIASED',
                                '-Wall','-Wextra','-Werror','-Isrc','tests/guest/data_zone.cpp',
                                'src/host/guest_memory.cpp','-o',str(binary)],cwd=ROOT,check=True)
                subprocess.run([str(binary)],check=True)

    def test_data_zone_rejects_native_and_code_pointers(self):
        prelude='''
#include "core/amos.h"
#include "lib/menus.h"
using namespace amos;
using namespace amos::guest;
int main() { Ctx context{}; REJECT }
'''
        statements=['context.a5 = (uint8_t*)0;',
                    'context.a3 = (uint8_t*)0;',
                    'menu::find((const be_u32*)0,1);',
                    'menu::find(HostPtr<Byte>{},1);',
                    'menu::find(host::decode_routine(16),1);',
                    'context.a3 = HostPtr<Byte>{};',
                    'context.a3 = host::decode_routine(16);',
                    'context.a3 = uint32_t(16);',
                    'uint8_t* native = context.a3; (void)native;',
                    'address(context.a3);',
                    '*context.a3 = 1;',
                    'context.a4 = (uint8_t*)0;',
                    'context.a4 = HostPtr<Byte>{};',
                    'context.a4 = host::decode_routine(16);',
                    'context.a4 = uint32_t(16);',
                    'uint8_t* native = context.a4; (void)native;',
                    'address(context.a4);',
                    '*context.a4 = 1;',
                    'context.a5 = HostPtr<Byte>{};',
                    'context.a5 = host::decode_routine(16);',
                    'context.a5 = uint32_t(16);',
                    'uint8_t* native = context.a5; (void)native;',
                    'address(context.a5);',
                    'encode_data_zone((uint8_t*)0);',
                    'encode_data_zone(HostPtr<Byte>{});']
        with tempfile.TemporaryDirectory() as temp:
            source=Path(temp)/'contract.cpp'
            for statement in ['context.a5 = decode_data_zone(16);',*statements]:
                source.write_text(prelude.replace('REJECT',statement))
                result=subprocess.run([*self.compiler,'-std=c++20','-Isrc','-fsyntax-only',str(source)],cwd=ROOT,capture_output=True)
                if statement in statements:self.assertNotEqual(result.returncode,0,statement)
                else:self.assertEqual(result.returncode,0,result.stderr.decode())
