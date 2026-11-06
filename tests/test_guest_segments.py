"""Typed static symbol resolution and full-image relocation with high backing."""
from pathlib import Path
import os
import platform
import concurrent.futures
import importlib.util
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
class SegmentTests(unittest.TestCase):
    def setUp(self):
        self.compiler=shlex.split(os.environ.get('CXX','c++'))
        if not shutil.which(self.compiler[0]):self.skipTest('requires C++ compiler')

    def test_catalog_matches_static_sources(self):
        subprocess.run(['python3','tools/sync_static_segments.py','--check'],cwd=ROOT,check=True)

    def test_importer_rejects_unclassified_pointer_expressions(self):
        spec=importlib.util.spec_from_file_location('static_importer',ROOT/'tools/sync_static_segments.py')
        importer=importlib.util.module_from_spec(spec);spec.loader.exec_module(importer)
        with tempfile.TemporaryDirectory() as temp:
            importer.ROOT=Path(temp)
            tables=Path(temp)/'src/tables';tables.mkdir(parents=True)
            (tables/'generated').mkdir()
            (tables/'labels.h').write_text('extern uint8_t *const start;\n')
            (tables/'labels.cpp').write_text('uint8_t *const start = host_obj_test + 0;\n')
            source=tables/'test.cpp'
            def code(expression):
                return 'uint8_t host_obj_test[16] = {};\nvoid relocate() {\n    put(host_obj_test + 0, 4, '+expression+');\n}\n'
            source.write_text(code('(uint32_t(uintptr_t(host_obj_test + 8)))'))
            importer.generate();importer.generate(check=True)
            for expression in ('(uint32_t(uintptr_t(host_obj_test + 8))) + 1',
                               '(uint32_t(uintptr_t(host_obj_test + 8)) | 1)',
                               '(uint32_t(uintptr_t(host_obj_test + 8)) * 2)',
                               'AMOS_STATIC_ADDRESS(host_obj_test, 8) + 1'):
                source.write_text(code(expression))
                with self.subTest(expression=expression), self.assertRaisesRegex(ValueError,'unclassified'):
                    importer.generate()

    def test_symbols_offsets_and_rejected_native_addresses(self):
        with tempfile.TemporaryDirectory() as temp:
            for opt in ('-O0','-O2'):
                binary=Path(temp)/'segments'
                subprocess.run([*self.compiler,'-std=c++20',opt,'-fno-exceptions','-fno-rtti',
                                '-Wall','-Wextra','-Werror','-Isrc','tests/guest/segments.cpp',
                                'src/host/guest_segments.cpp','src/host/guest_memory.cpp','-o',str(binary)],cwd=ROOT,check=True)
                subprocess.run([str(binary)],check=True)

    def test_symbol_pointer_entry_domains_are_not_convertible(self):
        prelude='''
#include "memory/guest_segments.h"
using namespace amos::guest;
void guest(GuestPtr<Byte>) {}
void host(HostPtr<Byte>) {}
void entry(GuestEntry<void()>) {}
int main() { GuestSymbol<Byte> s(SegmentId(1), 10); GuestEntry<int()> e{}; REJECT }
'''
        statements=['guest(s);','host(s);','entry(GuestPtr<Byte>{});','guest(e);','entry(e);',
                    'GuestSymbol<Byte> x(HostPtr<Byte>{}, 0);',
                    'GuestSymbol<Byte> x(SegmentId(1), 0.5);',
                    'GuestPtr<Byte> p = GuestEntry<void()>{};',
                    'constexpr GuestSymbol<Byte> x(SegmentId(1), UINT64_MAX);',
                    'constexpr GuestSymbol<Byte> x(SegmentId(1), -1);',
                    'constexpr SegmentId x(65536);',
                    'SegmentBindings b; b.bind_identity(SegmentId(1), HostSpan<Byte>{});']
        with tempfile.TemporaryDirectory() as temp:
            p=Path(temp)/'contract.cpp'
            for statement in ['(void)s; (void)e;',*statements]:
                p.write_text(prelude.replace('REJECT',statement))
                result=subprocess.run([*self.compiler,'-std=c++20','-Isrc','-fsyntax-only',str(p)],cwd=ROOT,capture_output=True)
                if statement in statements:self.assertNotEqual(result.returncode,0,statement)
                else:self.assertEqual(result.returncode,0,result.stderr.decode())

    def test_real_image_high_sources_and_identity_reference(self):
        if platform.system() != 'Linux':self.skipTest('dual-placement ELF reference requires Linux')
        with tempfile.TemporaryDirectory() as temp:
            # Compile the byte-source tables directly: no NDK, SDL or host
            # executable is required, and stale build/host objects cannot hide
            # source/header drift. Each compiler verifies its own object ABI.
            sources=sorted((ROOT/'src/tables').glob('*.cpp'))
            def compile_table(source):
                obj=Path(temp)/(source.stem+'.o')
                subprocess.run([*self.compiler,'-std=c++20','-O0','-fno-exceptions','-fno-rtti',
                                '-ffunction-sections','-fdata-sections','-Isrc','-c',str(source),
                                '-o',str(obj)],cwd=ROOT,check=True)
                return obj
            with concurrent.futures.ThreadPoolExecutor(max_workers=4) as pool:
                objects=list(pool.map(compile_table,sources))
            for flags in (['-pie'],['-no-pie','-DAMOS_NATIVE_STATIC_REFERENCE']):
                with self.subTest(flags=flags):
                    binary=Path(temp)/'image'
                    subprocess.run([*self.compiler,'-std=c++20','-O2','-fno-exceptions','-fno-rtti','-Isrc',
                                    'tests/guest/static_image.cpp','src/host/static_symbols.cpp',
                                    'src/host/guest_segments.cpp','src/host/guest_memory.cpp',
                                    *map(str,objects),'-Wl,--gc-sections',*flags,'-o',str(binary)],cwd=ROOT,check=True)
                    subprocess.run([str(binary)],check=True)
