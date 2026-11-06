"""Recovery uses typed native-state handles, including on high native stacks."""
from pathlib import Path
import os
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[1]

class RecoveryTests(unittest.TestCase):
    def test_high_stack_nested_unwind_task_ownership_and_reuse(self):
        compiler = shlex.split(os.environ.get('CXX', 'c++'))
        if not shutil.which(compiler[0]): self.skipTest('requires C++ compiler')
        with tempfile.TemporaryDirectory() as temp:
            for optimization in ('-O0', '-O2'):
                with self.subTest(optimization=optimization):
                    binary = Path(temp)/'recovery'
                    subprocess.run([*compiler, '-std=c++20', optimization, '-fno-exceptions',
                                    '-fno-rtti', '-fpack-struct=2', '-Wall', '-Wextra', '-Werror',
                                    '-Isrc', 'tests/guest/recovery.cpp', 'src/host/mark_registry.cpp', 'src/host/context_registry.cpp',
                                    '-o', str(binary)], cwd=ROOT, check=True)
                    subprocess.run([str(binary)], check=True)

    def test_wrong_pointer_domains_are_compilation_errors(self):
        compiler = shlex.split(os.environ.get('CXX', 'c++'))
        code = '''
#include "runtime/mark_handle.h"
#include "runtime/context_handle.h"
#include "core/guest_types.h"
using namespace amos;
using namespace amos::host;
using namespace amos::guest;
void consume(MarkHandle) {}
int main() { consume(decode_mark(1)); REJECT }
'''
        statements = ['consume(uint32_t(1));', 'consume(GuestPtr<Byte>{});',
                      'consume(HostPtr<Byte>{});', 'consume(static_cast<Mark*>(nullptr));',
                      'uint32_t word = decode_mark(1); (void)word;',
                      'wire::MarkField f; f.store(GuestPtr<Byte>{});',
                      'wire::MarkField f; f.store(HostPtr<Byte>{});',
                      'wire::MarkField f; f.store(uint32_t(1));',
                      'wire::MarkField f; uint32_t word = f.load(); (void)word;',
                      'wire::MarkField f; f.store(decode_context(1));',
                      'wire::ContextField f; f.store(decode_mark(1));',
                      'consume(decode_context(1));',
                      'save_context(Ctx{});',
                      'GuestContext c{}; c.a3 = HostPtr<Byte>{};',
                      'GuestContext c{}; c.a3 = (uint8_t*)0;',
                      'GuestContext c{}; c.a6 = decode_routine(16);',
                      'GuestContext c{}; uint8_t* p = c.a3; (void)p;',
                      'GuestContext c{}; address(c.a3);']
        with tempfile.TemporaryDirectory() as temp:
            source = Path(temp)/'types.cpp'
            command = [*compiler, '-std=c++20', '-Isrc', '-fsyntax-only', str(source)]
            source.write_text(code.replace('REJECT', ''))
            subprocess.run(command, cwd=ROOT, check=True, capture_output=True)
            for statement in statements:
                source.write_text(code.replace('REJECT', statement))
                result = subprocess.run(command, cwd=ROOT, capture_output=True)
                self.assertNotEqual(result.returncode, 0, statement)
