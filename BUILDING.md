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

`src/core` (the C++/assembler bridge) is an archive linked into all of them. The linker scripts keep
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
   body with `Lib_Int Name` (`tools/move_to_cpp.py` does this). The routine keeps its number. A helper routine
   that nothing else uses any more becomes `Lib_Empty`, which keeps the
   numbering. `tools/make_labels.py` lists every `Lib_Int` in
   `+Internal_Jumps.s` (`AMOSJmps`), and `Library_Patch` writes the address
   from that list into the jump table at start-up.
2. Write the entry point in `src/lib/*_glue.s` with the macros in
   `src/glue.i`. For an instruction or function this is one line,
   `LibCall Name,_c_function,params,Int|Float|String|None`: the parameters
   go to the C function as 32-bit values (a string as the address of its
   length word), and its return value becomes the AMOS result.
3. Implement the routine in `src/lib/*.cpp` as an `extern "C"` function.

`make_labels.py` refuses two things the loader cannot handle: a
PC-relative `Rbsr`/`Rbra`/`Rbcc` to a `Lib_Int` routine (it cannot reach the
program, so use `Rjsr`/`Rjmp`), and code in the library that could fall
through into a `Lib_Int`.

### The C++ side (`src/core/amos.h`)

- `amos::idata()` is the interpreter data above a5 (`IData`), and
  `amos::wdata()` the AMOS.library data below it (`WData`, the `T_` fields
  without the prefix). `tools/gen_layout.py` generates these and every other
  RS structure of `+Equ.s`/`+CEqu.s`/`+WEqu.s` (`Screen`, `Bob`, `Window`,
  `Program`...) into `build/gen/core/layout.h`. vasm computes the offsets,
  and a `static_assert` checks each field. The header also has the
  AMOS.library function numbers (`amos::sy::WaitVbl`, `amos::wi::Locate`...).
  `build/gen/lib_routines.h` has the routine numbers (`amos::L_Demande`).
- The glue saves the interpreter registers (d6-d7/a3-a6) in `amos_ctx` on
  every entry into C++. Calls back into the assembler run with them:
  `lib_call(L_x, regs)` (`JJsr`), `sy_call`/`ec_call`/`wi_call`
  (`SyCall`/`EcCall`/`WiCall`) with d0-d5/a0-a2 in an `amos::Regs`.
- `error(n)`, `fon_call()` and `lib_jump(L_x, d0)` raise errors. They do not
  return: the interpreter resets its stack.
- Strings (`src/lib/strings.h`): `str_alloc` (`L_Demande`), `str_finish`
  (`L_FinBin`), `str_empty`.
- A call into the assembler restores `amos_ctx` afterwards. Extension
  code, for one, changes a3 without restoring it, as the original callers
  saved all registers.
- Calls back into the assembler may not return either. Errors reset the
  stack, and a full string space restarts the whole instruction after the
  garbage collection. So no C++ object with a destructor may be alive across
  such a call, and side effects belong after the string allocation, as in
  the assembler.
- Interrupt code must not use `CCall`/`amos_ctx`.

Routines that other assembler code calls with registers (`L_InstrFind`,
`Bnk.GetAdr`...) keep a register entry. `RegRoutine Name,_c_function` hands
the C function all of d0-d7/a0-a2 in a `RegFrame` to read and change, and
sets the condition codes from its return value for callers that branch on
them. `move_to_cpp.py` also turns the library's PC-relative `Rbsr`/`Rbra`/
`Rbcc` to converted routines into `Rjsr`/`Rjmp`.

Converted so far: the integer paths of `Abs` and `Sgn`, the string
functions of `String.s` (except `Left$`/`Mid$`/`Right$`, `Str$` and `Val`,
which are in `+ILib.s`), the keyboard and joystick functions, `Locate`,
the banks (`Bnk.*`, the bank instructions) and lists (`Lst.*` of `+B.s`),
the number to text conversions.

### Deliberate differences from the original

- `Flip$` took its length from a register whose high word was not cleared.
  Depending on what ran before, it failed with "out of string space"
  (error 11). It now always works; `tests/golden/strings.txt` has the
  corrected line.
- `Dir`/`Dir First$` on a file name relative to the current directory: the
  assembler took the file part from a stale register and lost the filter.
- Some crashes on bad input become errors or are avoided: `Bank Swap` of
  two banks that do not exist, `Dir/W` without a screen, `PLoad` when the
  bank address happened to end in $0000 (reported "out of memory").

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

Editor tests (first line `Rem EDITOR`) check what the editor displays: the
listing ends with a syntax error, AMOS opens the editor on it, and the
harness compares screenshots with `tests/golden/<name>.png`, ignoring the
pixels that change while it watches (the cursor). `editor_listing` covers
the detokenizer.

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
