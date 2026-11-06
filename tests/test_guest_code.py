"""Code-address ABI and metadata accesses through real translated memory."""
from pathlib import Path
import os
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT=Path(__file__).resolve().parents[1]
class CodeTests(unittest.TestCase):
    def setUp(self):
        self.compiler=shlex.split(os.environ.get('CXX','c++'))
        if not shutil.which(self.compiler[0]):self.skipTest('requires C++ compiler')

    def test_high_backed_code_metadata_and_word_access_faults(self):
        with tempfile.TemporaryDirectory() as temp:
            for opt in ('-O0','-O2'):
                binary=Path(temp)/'code'
                subprocess.run([*self.compiler,'-std=c++20',opt,'-Wall','-Wextra','-Werror','-Isrc',
                                'tests/guest/code.cpp','src/host/guest_memory.cpp','-o',str(binary)],cwd=ROOT,check=True)
                subprocess.run([str(binary)],check=True)

    def test_interpreter_and_metadata_reject_wrong_pointer_domains(self):
        prelude='''
#include "memory/guest_code.h"
#include "memory/guest_view.h"
#include "runtime/interpreter_bridge.h"
#include "interp/interp.h"
using namespace amos;
using namespace amos::guest;
int main() { Memory m; Regs r{}; REJECT }
'''
        statements=['interp_exec((void*)0, 0, 0);',
                    'amos_call(&r, (void*)0);',
                    'amos_call(&r, GuestPtr<Byte>{});',
                    'amos_call_all((AllRegs*)0, HostPtr<Byte>{});',
                    'amos_eval(&r, (void*)0);',
                    'amos_call_cc(&r, (void*)0);',
                    'amos_goto((void*)0, 0);',
                    'entry_call((void*)0, r);',
                    'wentry_function((void*)0);',
                    'wire::RoutineField f; f.store((void*)0);',
                    'wire::RoutineField f; f.store(GuestPtr<Byte>{});',
                    'wire::RoutineField f; f.store(HostPtr<Byte>{});',
                    'wire::RoutineField f; f.store(uint32_t(16));',
                    'wire::RoutineField f{}; GuestPtr<Byte> p = f.load();',
                    'GuestRef<wire::RoutineField> f; f.store(GuestPtr<Byte>{});',
                    'GuestRef<wire::RoutineField> f; f.store(HostPtr<Byte>{});',
                    'GuestRef<const wire::RoutineField> f; f.store(host::decode_routine(16));',
                    'interp_continue(host::decode_mark(1), uint32_t(16));',
                    'interp_exec(GuestPtr<Byte>{}, 0, 0);',
                    'interp_jump(&r, HostPtr<Byte>{});',
                    'interp_prun_run(0, (void*)0);',
                    'interp_zap_in(&r, (void*)0);',
                    'interp::collector_of((void*)0);',
                    'interp::collect((uint8_t*)0, 0, {}, false);',
                    'interp::collect(HostPtr<Byte>{}, 0, {}, false);',
                    'interp::collect(GuestPtr<const Byte>{}, 0, GuestPtr<Byte>{}, false);',
                    'CodeReader<void()> c(m, GuestPtr<Byte>{});',
                    'CodeReader<void()> c(m, HostPtr<Byte>{});',
                    'CodeReader<void()> c(m, abi::EntryCodec::decode<int()>(16));']
        with tempfile.TemporaryDirectory() as temp:
            source=Path(temp)/'contract.cpp'
            for statement in ['(void)m; (void)r;',*statements]:
                source.write_text(prelude.replace('REJECT',statement))
                result=subprocess.run([*self.compiler,'-std=c++20','-Isrc','-fsyntax-only',str(source)],cwd=ROOT,capture_output=True)
                if statement in statements:self.assertNotEqual(result.returncode,0,statement)
                else:self.assertEqual(result.returncode,0,result.stderr.decode())
