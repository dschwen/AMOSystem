"""Typed dispatch calls real signatures; malformed signatures are compile errors."""
from pathlib import Path
import os
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[1]
class CallTests(unittest.TestCase):
    def test_generated_bindings_match_all_runtime_entries(self):
        subprocess.run(['python3', 'tools/sync_runtime_bindings.py', '--check'], cwd=ROOT, check=True)

    def test_exact_signatures_arguments_and_results(self):
        compiler = shlex.split(os.environ.get('CXX', 'c++'))
        if not shutil.which(compiler[0]): self.skipTest('requires C++ compiler')
        with tempfile.TemporaryDirectory() as temp:
            for optimization in ('-O0', '-O2'):
                binary = Path(temp)/'calls'
                obj = Path(temp)/'memory.o'
                flags = ['-std=c++20', optimization, '-fno-exceptions', '-fno-rtti',
                         '-Wall', '-Wextra', '-Werror', '-Isrc']
                subprocess.run([*compiler, *flags, '-c', 'src/host/guest_memory.cpp', '-o', str(obj)],
                               cwd=ROOT, check=True)
                subprocess.run([*compiler, *flags, '-fpack-struct=2',
                                'tests/guest/typed_calls.cpp', str(obj), '-o', str(binary)], cwd=ROOT, check=True)
                subprocess.run([str(binary)], check=True)

    def test_mismatched_signature_is_a_compilation_error(self):
        compiler = shlex.split(os.environ.get('CXX', 'c++'))
        declarations = '''
#include "runtime/typed_call.h"
#include "runtime/native_binding.h"
using namespace amos;
using namespace amos::host;
uint32_t one(uint32_t n) { return n; }
void none(uint32_t) {}
uint32_t host(guest::HostPtr<guest::Byte>) { return 0; }
float floating(float n) { return n; }
void callback(void (*)(uint32_t)) {}
int main() { const uint32_t words[2]{}; REJECT }
'''
        cases = ['typed_library_call<&one, 2, 4>(words);',
                 'typed_library_call<&one, 1, 8>(words);',
                 'typed_library_call<&none, 1, 4>(words);',
                 'typed_library_call<&host, 1, 4>(words);',
                 'typed_library_call<&floating, 1, 4>(words);',
                 'typed_library_call<&callback, 1, 0>(words);',
                 'NativeBinding::from_reg(&one);',
                 'NativeBinding::from_library(guest::GuestPtr<guest::Byte>{});']
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp)/'reject.cpp'
            for statement in ['typed_library_call<&one, 1, 4>(words);', *cases]:
                source.write_text(declarations.replace('REJECT', statement))
                result = subprocess.run([*compiler, '-std=c++20', '-Isrc', '-fsyntax-only', str(source)],
                                        cwd=ROOT, capture_output=True, text=True)
                if statement in cases: self.assertNotEqual(result.returncode, 0, statement)
                else: self.assertEqual(result.returncode, 0, result.stderr)
