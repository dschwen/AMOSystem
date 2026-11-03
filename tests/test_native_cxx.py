"""Native C++ headers must retain their ABI within packed guest translation units."""
import os
from pathlib import Path
import shutil
import shlex
import subprocess
import tempfile
import unittest

ROOT=Path(__file__).resolve().parents[1]

class NativeCxxTests(unittest.TestCase):
    def test_native_layout_and_guest_packing_restoration(self):
        compiler=shlex.split(os.environ.get('CXX','c++'))
        if not shutil.which(compiler[0]): self.skipTest('requires C++ compiler')
        code=r'''
#include <stddef.h>
struct Before { char tag; double value; };
#include "core/endian.h" // Earliest C++ standard-library include in guest code.
#include "core/layout.h"
#include "host/clipboard_text.h"
#include "host/cxx.h"
struct After { char tag; double value; };
static_assert(offsetof(Before,value)==2);
static_assert(offsetof(After,value)==2); // Header boundary must restore -fpack-struct.
static_assert(alignof(std::pair<char,double>)==alignof(double));
using Pair=std::pair<char,double>; static_assert(offsetof(Pair,second)==8);
static_assert(alignof(std::pair<char,long double>)==alignof(long double));
// The layout header also enforces thousands of guest field offsets and sizes.
#pragma pack(push,16)
struct NativeOwner { char tag; std::string text; std::vector<int> numbers; };
#pragma pack(pop)
static_assert(offsetof(NativeOwner,text)%alignof(std::string)==0);
int main() {
    NativeOwner owner={'x',"native ABI",{1,2,3}};
    auto copied=owner;
    std::string decoded;
    return copied.text=="native ABI" && copied.numbers.size()==3 &&
        amos::host::clipboard::decode("caf\xc3\xa9",decoded) && decoded=="caf\xe9" ? 0 : 1;
}
'''
        with tempfile.TemporaryDirectory() as temp:
            source=Path(temp)/'probe.cpp';source.write_text(code)
            binary=Path(temp)/'probe'
            subprocess.run([*compiler,'-std=c++20','-fpack-struct=2','-Isrc',str(source),'-o',str(binary)],cwd=ROOT,check=True)
            subprocess.run([str(binary)],check=True)
