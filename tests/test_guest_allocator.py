"""The production free-list engine uses logical addresses, never native pointers."""
from pathlib import Path
import os
import shlex
import shutil
import subprocess
import tempfile
import unittest
ROOT = Path(__file__).resolve().parents[1]
class AllocatorTests(unittest.TestCase):
    def test_partial_free_allocabs_and_firstfit_with_high_backing(self):
        compiler = shlex.split(os.environ.get('CXX', 'c++'))
        if not shutil.which(compiler[0]): self.skipTest('requires C++ compiler')
        with tempfile.TemporaryDirectory() as temp:
            for optimization in ('-O0', '-O2'):
                with self.subTest(optimization=optimization):
                    binary = Path(temp)/'allocator'
                    subprocess.run([*compiler, '-std=c++20', optimization, '-fno-exceptions', '-fno-rtti',
                                    '-Wall', '-Wextra', '-Werror', '-Isrc', 'tests/guest/allocator.cpp',
                                    'src/host/guest_memory.cpp', 'src/host/guest_allocator.cpp',
                                    '-o', str(binary)], cwd=ROOT, check=True)
                    subprocess.run([str(binary)], check=True)
