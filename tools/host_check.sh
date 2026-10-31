#!/bin/sh
# Phase 7: which of the C++ files compile for another host (syntax only,
# 32 bits, the NDK's headers for the Amiga's structures). The Amiga
# backends of src/hal (*_amiga.cpp) are left out. Lists the files that do
# not, with their first error.
NDK=${NDK:-/opt/amiga/m68k-amigaos/ndk-include}
ok=0
bad=0
for f in $(ls src/*/*.cpp | grep -v '_amiga\.cpp$'); do
	err=$(g++ -m32 -ffreestanding -fpack-struct=2 -std=c++20 -fsyntax-only -Isrc \
		-idirafter "$NDK" "$f" 2>&1 | grep -m1 'error' | cut -c1-160)
	if [ -z "$err" ]; then
		ok=$((ok + 1))
	else
		bad=$((bad + 1))
		echo "$f: $err"
	fi
done
echo "$ok files compile for another host, $bad do not"
