"""Strong domain contract and real high-address translated-memory tests."""
from pathlib import Path
import os
import json
import shlex
import re
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]
PRELUDE = '''
#include "abi/guest_address_codec.h"
#include "memory/guest_view.h"
using namespace amos::guest;
void takes_guest(GuestPtr<Byte>) {}
void takes_host(HostPtr<Byte>) {}
void guest_span(GuestSpan<Byte>) {}
void host_span(HostSpan<Byte>) {}
int main() {
  Byte buffer[4]{};
  auto g = abi::AddressCodec::decode<Byte>(0x10000);
  HostPtr<Byte> h(buffer);
  GuestSpan<Byte> gs(g, 4);
  HostSpan<Byte> hs(buffer, 4);
  GuestPtr<const Byte> cg(g);
'''
POSITIVE = '''
 takes_guest(g + 1); takes_host(h);
 guest_span(gs); host_span(hs);
 PointerField<Byte> field; field.store(g);
 (void)cg; (void)field;
'''
NEGATIVE = {
    'host_to_guest': 'takes_guest(h);',
    'guest_to_host': 'takes_host(g);',
    'arithmetic_to_host': 'takes_host(g + 10);',
    'host_span_to_guest': 'guest_span(hs);',
    'guest_span_to_host': 'host_span(gs);',
    'raw_pointer_to_guest': 'takes_guest(buffer);',
    'guest_to_raw_pointer': 'Byte* p = g; (void)p;',
    'guest_to_void': 'void* p = g; (void)p;',
    'guest_to_integer': 'uint32_t bits = g; (void)bits;',
    'explicit_guest_to_host': 'HostPtr<Byte> p(g); (void)p;',
    'explicit_host_to_guest': 'GuestPtr<Byte> p(h); (void)p;',
    'explicit_raw_to_guest': 'GuestPtr<Byte> p(buffer); (void)p;',
    'explicit_integer_to_guest': 'GuestPtr<Byte> p(0x10000u); (void)p;',
    'explicit_guest_to_raw': 'auto p = static_cast<Byte*>(g); (void)p;',
    'const_removal': 'takes_guest(cg);',
    'wire_host_pointer': 'PointerField<Byte> p; p.store(h);',
    'wire_integer': 'PointerField<Byte> p; p.store(0x10000u);',
    'host_guest_subtract': 'auto d = g - h; (void)d;',
    'host_guest_compare': 'bool b = g == h; (void)b;',
    'float_offset': 'auto p = g + 0.5; (void)p;',
    'float_byte_delta': 'auto d = GuestByteDelta(0.5); (void)d;',
    'bool_offset': 'auto p = g + true; (void)p;',
    'host_as_offset': 'auto p = g + h; (void)p;',
    'guest_as_offset': 'auto p = g + g; (void)p;',
    'implicit_distance': 'int64_t n = g - g; (void)n;',
    'void_arithmetic': 'auto p = abi::AddressCodec::decode<void>(1) + 1; (void)p;',
    'dereference': 'Byte b = *g; (void)b;',
    'native_get': 'auto p = g.get(); (void)p;',
    'native_span_data': 'Byte* p = gs.data(); (void)p;',
    'view_to_native_reference': 'Memory m; uint32_t& n = m.ref(abi::AddressCodec::decode<uint32_t>(1)).value; (void)n;',
    'view_const_store': 'Memory m; m.ref(abi::AddressCodec::decode<const uint32_t>(1)).value.store(1);',
    'pointer_view_host_store': 'Memory m; m.ref(abi::AddressCodec::decode<PointerField<Byte>>(1)).value.store(h);',
    'wrong_copy_domain': 'Memory m; m.copy_from_host(hs, gs);',
}

class GuestAddressTests(unittest.TestCase):
    def setUp(self):
        self.compiler = shlex.split(os.environ.get('CXX', 'c++'))
        if not shutil.which(self.compiler[0]):
            self.skipTest('requires C++ compiler')

    def test_domain_errors_are_compilation_errors(self):
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / 'contract.cpp'
            compilers = [self.compiler]
            amiga = Path(os.environ.get('AMIGA_CXX', '/opt/amiga/bin/m68k-amigaos-g++'))
            if amiga.exists():
                compilers.append([str(amiga), '-m68000'])
            for compiler in compilers:
                command = [*compiler, '-std=c++20', '-fno-exceptions', '-fno-rtti',
                           '-Wall', '-Wextra', '-Werror', '-pedantic-errors', '-Isrc', '-fsyntax-only', str(source)]
                source.write_text(PRELUDE + POSITIVE + '}')
                subprocess.run(command, cwd=ROOT, check=True, capture_output=True, text=True)
                for name, statement in NEGATIVE.items():
                    with self.subTest(compiler=compiler, name=name):
                        source.write_text(PRELUDE + '#line 100 "invalid_domain.cpp"\n' + statement + '\n}')
                        result = subprocess.run(command, cwd=ROOT, capture_output=True, text=True)
                        self.assertNotEqual(result.returncode, 0, name)
                        self.assertIn('invalid_domain.cpp:100:', result.stderr)

    def test_amiga_wrappers_add_no_native_code(self):
        compiler = Path(os.environ.get('AMIGA_CXX', '/opt/amiga/bin/m68k-amigaos-g++'))
        if not compiler.exists():
            self.skipTest('requires m68k-amigaos-g++ for machine-code comparison')
        objdump = compiler.with_name('m68k-amigaos-objdump')
        self.assertTrue(objdump.exists(), 'requires matching objdump')
        with tempfile.TemporaryDirectory() as temp:
            for optimization in ('-Os', '-O2'):
                with self.subTest(optimization=optimization):
                    outputs = []
                    for variant in ('raw', 'wrapped'):
                        obj = Path(temp) / (variant + '.o')
                        flags = ['-DAMOS_TEST_WRAPPED'] if variant == 'wrapped' else []
                        subprocess.run([str(compiler), '-m68000', optimization, '-std=c++20',
                                        '-fomit-frame-pointer', '-fno-exceptions', '-fno-rtti',
                                        '-ffreestanding', '-fno-threadsafe-statics', '-fno-ipa-icf',
                                        '-Wall', '-Wextra', '-Werror', '-Isrc', *flags,
                                        '-c', 'tests/guest/amiga_codegen.cpp', '-o', str(obj)],
                                       cwd=ROOT, check=True)
                        disassembly = subprocess.check_output([str(objdump), '-dr', str(obj)], text=True)
                        # Ignore only the input filename/file-format heading. Keep
                        # instruction bytes, operands, offsets, symbols and relocations.
                        outputs.append(disassembly[disassembly.index('Disassembly of section'):])
                    # Addition is commutative: GCC can swap which stack
                    # operand is loaded first, with exactly the same instructions.
                    slice_pattern = r'(?m)^[0-9a-f]+ <_slice_bank>:\n(.*?)(?=\n[0-9a-f]+ <|\Z)'
                    slices = [re.search(slice_pattern, output, re.S).group(1) for output in outputs]
                    for body in slices:
                        self.assertEqual(len(re.findall(r'(?m)^\s*[0-9a-f]+:\s+[0-9a-f]{4}', body)), 3)
                        self.assertEqual(re.findall(r'\t(movel|addl|rts)\b', body), ['movel', 'addl', 'rts'])
                        self.assertEqual(sorted(re.findall(r'sp@\((\d+)\)', body)), ['12', '4'])
                    outputs = [re.sub(slice_pattern, '<slice: equivalent native addition>\n', output, flags=re.S)
                               for output in outputs]
                    if optimization == '-Os':
                        self.assertMultiLineEqual(outputs[0], outputs[1])
                    else:
                        # GCC may choose a smaller loop prologue for integer-
                        # represented cursors. All other functions remain exact.
                        split = r'\n[0-9a-f]+ <_sum_bytes>:\n'
                        raw, wrapped = [re.split(split, output) for output in outputs]
                        self.assertMultiLineEqual(raw[0], wrapped[0])
                        pattern = r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{4}\s+)+)\s*(.*)$'
                        instructions = [re.findall(pattern, body, re.M) for body in (raw[1], wrapped[1])]
                        sizes = [sum(len(opcode.split()) * 2 for _, opcode, _ in code) for code in instructions]
                        self.assertLessEqual(sizes[1], sizes[0])
                        loops = []
                        for code in instructions:
                            self.assertFalse(any('jsr' in asm or 'bsr' in asm for _, _, asm in code))
                            back = [(i, int(re.search(r'bnes ([0-9a-f]+)', asm)[1], 16))
                                    for i, (_, _, asm) in enumerate(code) if re.search(r'bnes ([0-9a-f]+)', asm)]
                            self.assertEqual(len(back), 1)
                            end, target = back[0]
                            start = next(i for i, (address, _, _) in enumerate(code) if int(address, 16) == target)
                            loops.append([opcode.split() for _, opcode, _ in code[start:end+1]])
                        self.assertEqual(loops[0], loops[1])

    def test_high_memory_arithmetic_access_and_packed_boundary(self):
        with tempfile.TemporaryDirectory() as temp:
            for optimization in ('-O0', '-O2'):
                with self.subTest(optimization=optimization):
                    obj = Path(temp) / 'memory.o'
                    binary = Path(temp) / 'addressing'
                    flags = ['-std=c++20', '-fno-exceptions', '-fno-rtti', '-Wall', '-Wextra',
                             '-Werror', '-Isrc', optimization]
                    subprocess.run([*self.compiler, *flags, '-c', 'src/host/guest_memory.cpp', '-o', str(obj)], cwd=ROOT, check=True)
                    subprocess.run([*self.compiler, *flags, '-fpack-struct=2', 'tests/guest/addressing.cpp',
                                    str(obj), '-o', str(binary)], cwd=ROOT, check=True)
                    subprocess.run([str(binary)], check=True)
                    probe = Path(temp) / 'memory-probe'
                    subprocess.run([*self.compiler, *flags, 'tests/macos/guest_memory_probe.cpp',
                                    str(obj), '-o', str(probe)], cwd=ROOT, check=True)
                    result = json.loads(subprocess.check_output([str(probe)], text=True))
                    self.assertTrue(result['guest_translation'])
                    self.assertTrue(result['native_backing_above_4gb'])
                    self.assertTrue(result['bank_plus_ten'])

    def test_packing_restoration_and_amiga_headers(self):
        code = '''
#include <stddef.h>
struct Before { char tag; int value; };
#include "abi/guest_address_codec.h"
#include "memory/guest_view.h"
struct After { char tag; int value; };
static_assert(offsetof(Before, value) == 2);
static_assert(offsetof(After, value) == 2);
static_assert(sizeof(amos::guest::PointerField<unsigned char>) == 4);
static_assert(alignof(amos::guest::GuestPtr<unsigned char>) == amos::guest::pointer_alignment);
'''
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp) / 'packing.cpp'
            source.write_text(code)
            compilers = [self.compiler]
            amiga = '/opt/amiga/bin/m68k-amigaos-g++'
            if Path(amiga).exists():
                compilers.append([amiga])
            for compiler in compilers:
                with self.subTest(compiler=compiler):
                    subprocess.run([*compiler, '-std=c++20', '-fpack-struct=2', '-fno-exceptions',
                                    '-fno-rtti', '-Isrc', '-fsyntax-only', str(source)], cwd=ROOT, check=True)
