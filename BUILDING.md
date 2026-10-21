# Building AMOS Professional on a Unix host

Requirements: the Amiga cross toolchain from
https://github.com/AmigaPorts/m68k-amigaos-gcc (installed in `/opt/amiga`, or
pass `AMIGA_PREFIX=...`), GNU make and python3.

```
make            # assemble everything into build/
make verify     # compare with the prebuilt binaries shipped in AMOS/
make dist       # build/dist/AMOS: the AMOS tree with the new binaries
```

## What replaces the original Amiga tools

| Original (Amiga)              | Here                         |
|-------------------------------|------------------------------|
| `c/Genam`, `c/genim2`         | `vasmm68k_mot -devpac`        |
| `c/Make_Labels`               | `tools/make_labels.py`        |
| `c/Make_Toktable`             | `tools/make_toktable.py`      |
| `c/Library_Digest`            | `tools/library_digest.py`     |
| `c/Get_Chunk`                 | `vasm -Fbin` plus longword padding |
| AmigaDOS case-insensitive names | `tools/case_links.py` (symlinks in `build/ci`) |

Generated files go to `build/gen`; nothing is written into the source tree.

## How the programs are linked

Each program is assembled to an object with vasm and linked with
`m68k-amigaos-ld` (through gcc) together with the C++ of its `src/`
directory:

| program | assembler | C++ / glue | linker script |
|---|---|---|---|
| AMOSPro | `+B.s` (+ `+Verif.s`) | `src/lib`, `src/main` | `link/amospro.ld` |
| AMOS.library | `+W.s` | `src/w` | `link/module.ld` |
| AMOSPro_Editor | `+Edit.s` | `src/editor` | `link/module.ld` |
| AMOSPro_Monitor | `+Monitor.s` | `src/monitor` | `link/module.ld` |

`src/common` is an archive linked into all of them. The linker scripts keep
the assembler code in the first hunk: it holds the entry point or entry
table. AMOSPro's init hunk `I` must come second, because `Init_Fin` frees it.
This `ld` writes no memory flags, so `tools/chiphunks.py` marks the
`.datachip`/`.bsschip` hunks as chip memory after linking, using the map
file. AMOSPro.Lib and the extensions are still assembled directly. AMOSPro
loads them raw and patches them, and they cannot contain linked code.

C++ is compiled for the 68000 with `-Os`, without exceptions, RTTI or a C
library startup. libgcc and libnix13 supply arithmetic helpers such as
`__divsi3`.

## Moving an AMOSPro.Lib routine to C++

AMOSPro.Lib is loaded raw, so the code of a routine that moves out of it is
linked into the AMOSPro program instead:

1. In `+Lib.s`/`+ILib.s`, replace the routine's `Lib_Par`/`Lib_Def` and its
   body with `Lib_Int Name`. The routine keeps its number.
   `tools/make_labels.py` lists every `Lib_Int` in `+Internal_Jumps.s`
   (`AMOSJmps`), and `Library_Patch` writes the address from that list into
   the jump table at start-up.
2. Write the entry point in `src/lib/*_glue.s` with the macros in
   `src/glue.i`. `LibEntry` is for instructions and functions: it puts the
   `"GetP"` placeholder in front of the label, which `Library_GetParams`
   replaces with the call to the parameter collector. `LibRoutine` is for
   routines called with `Rjsr`/`JJsr`. The glue moves the AMOS registers onto
   the C stack, calls the C++ function, and puts the result back in d3/d2.
3. Implement the routine in `src/lib/*.cpp` as an `extern "C"` function.

`make_labels.py` refuses two things the loader cannot handle: a
PC-relative `Rbsr`/`Rbra`/`Rbcc` to a `Lib_Int` routine (it cannot reach the
program, so use `Rjsr`/`Rjmp`), and code in the library that could fall
through into a `Lib_Int`. Converted so far: the integer paths of `Abs` and
`Sgn` (`src/lib/maths.cpp`).

## Assembler compatibility notes

- `+W.s` (AMOS.library) was built with GenIm2, which matches symbols without
  regard to case, so it is assembled with `-nocase`. All other sources need
  case-sensitive symbols (they contain e.g. both `.skip` and `.Skip`).
- `EzFlag` (Easy AMOS variant of AMOS.library) is not defined in the sources
  and is passed as `-DEzFlag=0`.
- `-kick1hunks` keeps relocations in `HUNK_RELOC32`, as GenAm did, so the
  binaries still load on Kickstart 1.3.

## Verification

`tools/bincmp.py` compares hunk files semantically. `make verify` covers
the programs that are still pure assembler: AMOSPro and AMOSPro.Lib differ
from the originals now, and the emulator tests are what checks them. It accepts the encoding
choices on which GenAm and vasm differ (`andi` vs `and #imm`, the order of `exg`
operands, padding, memory flags in the hunk id, reloc order) and reports
everything else. All nine programs and libraries are equivalent to the
prebuilt ones, and the two config files are byte-identical.

Three source fixes were needed:
- `+B.s`: `bne.` is now `bne`, assembled as `bne.w` as before.
- `+Edit.s`: `cmp.s` is now `cmp.b`.
- `+Editor_Config.s`: the `ç`/`©` bytes in two strings had been replaced by
  U+FFFD in an earlier re-encoding of the repository, and are restored.

## Tests in the emulator

```
make test             # build/dist against the golden results
make test-reference   # the prebuilt binaries in AMOS/ against the same goldens
python3 tests/run.py --reference --update-golden [NAME...]   # record goldens
python3 tests/run.py --amos-tree "../Minimal Workbench v3.1/AMOS_Pro"
```

Each `tests/amos/<name>.asc` is a plain-text AMOS listing. `tests/run.py`
tokenizes it with `tools/amostok.py`, adds a prologue that opens
`AMOSTEST:out/<name>.txt` as channel 1 and an epilogue that ends with
`System`, and boots one FS-UAE per test from a generated boot drive. The AMOS
tree sits on a read-only drive, and the Workbench drive provides `C:`, `Libs:`
and the rest. Results go to channel 1 (`Print #1,...`) or to IFF files
(`Save Iff "AMOSTEST:out/<name>_1.iff"`). Everything is compared byte for byte
with `tests/golden/`, which was recorded with the original binaries.

Things to know when writing tests:
- A syntax error drops AMOS into the editor. The test then times out, and
  `build/test-run/<name>/timeout.png` shows the editor with the cursor on the
  error.
- Run-time errors are logged as `ERROR <n>` and the test carries on
  (`Resume Next`).
- Procedure names must not collide with keywords (`Say`, for instance).
- `Rnd` is not reproducible, even after `Randomize`.

Requirements: fs-uae, Xvfb, scrot, python3 with Pillow, the A1200 Kickstart
3.1 ROM (`../FS-UAE/Kickstarts`) with its `rom.key` (the ROM is
Cloanto-encrypted; the key comes from `Devs/Kickstarts` on the Workbench
drive), and the Workbench directory drive `../Minimal Workbench v3.1`.
fs-uae runs in a non-cycle-exact, maximum-speed CPU mode without sound. A test
takes about 2 seconds, and four run in parallel.

Other tools:
- `tools/amostok.py detok|tok|check`: the tokenizer and detokenizer.
  `check` round-trips programs: 45807 of the 45818 lines in the shipped
  `.AMOS` files are reproduced exactly. The other 11 come from programs saved
  by older AMOS versions.
- `tools/ilbm2png.py`: view an IFF screenshot.
- `tools/uaem.py`: write the `.uaem` sidecar files of FS-UAE directory drives.
- `make install-hd`: put the built binaries into the AMOS Pro on the Workbench
  drive. The originals are backed up to `../AMOS_Pro-original-binaries`, and
  the config files are left alone.
