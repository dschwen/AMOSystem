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
- The glue saves the interpreter registers (d6-d7/a3-a6) in the task's
  context, `ctx()`, on every entry into C++: one for the main task, one for
  any other, which is the file selector's (it runs Interface code in a task
  of its own while the main task reads the directory). The contexts are a
  block that `amos_bridge` points to (`src/core/call.s`); AMOSPro records
  its main task in it at startup and passes its address in `T_CBridge` to
  AMOS.library, whose own copy of the bridge then uses it. (`tc_UserData`
  is not free for this: AMOS.library keeps its data zone there.)
  AMOS.library is no longer assembled with `OPT P+`: its code calls C++ in
  other hunks, which LoadSeg relocates. Calls back into the assembler run with them:
  `lib_call(L_x, regs)` (`JJsr`), `sy_call`/`ec_call`/`wi_call`
  (`SyCall`/`EcCall`/`WiCall`) with d0-d5/a0-a2 in an `amos::Regs`.
- `error(n)`, `fon_call()` and `lib_jump(L_x, d0)` raise errors. They do not
  return: the interpreter resets its stack.
- Strings (`src/lib/strings.h`): `str_alloc` (`L_Demande`), `str_finish`
  (`L_FinBin`), `str_empty`.
- A call into the assembler restores the context afterwards. Extension
  code, for one, changes a3 without restoring it, as the original callers
  saved all registers.
- Calls back into the assembler may not return either. Errors reset the
  stack, and a full string space restarts the whole instruction after the
  garbage collection. So no C++ object with a destructor may be alive across
  such a call, and side effects belong after the string allocation, as in
  the assembler.
- Interrupt code must not use `CCall`/`amos_ctx`.
- `lib_eval(L_x, regs)` calls a routine that reads parameters from the
  program itself (`MnDim`, the evaluator): the interpreter then continues at
  the a6 (and with the a3) it leaves.

Routines that other assembler code calls with registers (`L_InstrFind`,
`Bnk.GetAdr`...) keep a register entry. `RegRoutine Name,_c_function` hands
the C function all of d0-d7/a0-a2 in a `RegFrame` to read and change, and
sets the condition codes from its return value for callers that branch on
them. `move_to_cpp.py` also turns the library's PC-relative `Rbsr`/`Rbra`/
`Rbcc` to converted routines into `Rjsr`/`Rjmp`.

AMOS.library functions move to C++ a vector table at a time. The text
windows are the first: `T_WiVect` points to the table in
`src/w/windows_glue.s`, whose entries hand all registers to
`w_window(n, AllRegs *)` and set the condition codes from d0 as before.
The glue makes the caller's d6-d7/a3-a6 the task's context for the C++
code (it needs a5) and puts the old context back afterwards, as callers
are assembler, C++ (through `amos_call`) or other tasks (requesters).
Other functions of the vector tables (the drawing ones of the screen
table: `Cls`, clip, `Set Pattern`, `Paint`, sliders) keep their entry in
`+W.s`, which jumps to a `WEntry` (`src/w/wglue.i`) that does the same for
one C function. Assembler routines the C++ still calls (`TAbk1`-`4`,
`FastMm`, `SySetZ`, `Ec_SetClip`...) are exported from `+W.s` under `_w_`
names; `src/w/w.h` declares them. The writing modes, which the
assembler patched into its own code, are two variables, shared by all
windows as before.

Converted so far: the integer paths of `Abs` and `Sgn`, the string
functions of `String.s` (except `Left$`/`Mid$`/`Right$`, `Str$` and `Val`,
which are in `+ILib.s`), the keyboard and joystick functions, `Locate`,
the banks (`Bnk.*`, the bank instructions) and lists (`Lst.*` of `+B.s`),
the number to text conversions.

### Deliberate differences from the original

Bugs of the assembler that the C++ fixes. Everything else keeps the
original behaviour, quirks included (the tests record the original
binaries' results). Where a fix changes a recorded result, the golden file
has the corrected one.

Strings and numbers
- `Flip$` took its length from a register whose high word was not cleared.
  Depending on what ran before, it failed with "out of string space"
  (error 11). It now always works; `tests/golden/strings.txt` has the
  corrected line.

Banks, files and directories
- `Bank Swap` of two banks that do not exist crashed; it is now an error.
- `Dir`/`Dir First$` on a file name relative to the current directory took
  the file part from a stale register and lost the filter.
- `Dir/W` without a screen crashed.
- `PLoad` reported "out of memory" when the bank address happened to end in
  $0000.

IFF
- `Iff Anim` stopped on an `illegal` instruction left in the code (a
  debugger break).
- An IFF picture with an empty `CMAP` chunk made the palette loop wrap
  around (a `dbra` with a zero count) and overwrite memory.

Menus
- An error inside the menu (no memory for a background) restored the
  registers in the wrong order and crashed.
- A `(PR)` object crashed `Menu Calc`, which ran the procedure with the
  registers of the last menu shown; `Menu Calc` now saves them too.
- In direct mode a `(PR)` object was not skipped: its name ran as object
  commands.
- `Bank To Menu` kept the background and `(RE)` memory pointers of the menu
  the bank was made from, and freed them later.
- `Menu Called`/`Menu Once` with a level instead of coordinates wrote to a
  random address; they now do nothing.
- The inactive object of an item was never freed.
- A background save on a screen of more than 64K per plane was put back at
  the wrong place (the offset was kept in a word).
- `Menu$` reported an object as compiled when there was no memory for it
  (it returned the length); it is now "out of memory".

Interface (dialogs)
- `VText` with an empty string left the stack unbalanced and crashed.
- A division whose divisor has a zero low word (`65536`) caused a
  division-by-zero trap; it is now a syntax error, like a zero divisor.
- `IL` (an "illegal" debugger trap, which also hides the Inactive List
  instruction of the same name) is a syntax error instead of a crash.
- `Dialog Run` of a program without `RUn` returned whatever a register
  held (often 1000 plus the number of the first edit zone); it now
  returns 0.
- While `RUn` waits, an action restarted its timer depending on a long word
  on the stack instead of the channel's timer.
- A button with `NW` held down decided on stopping the dialog from a flag
  read past the end of the button (`Dia_Flags` of the channel structure
  applied to the zone); it now uses the zone's `BQ` flag.
- An error of the program outside the interpreter loop (in `Dialog Update`
  or in the zone handling between instructions) jumped back to a stale
  stack pointer; it now ends that call with the error.
- A click in an edit zone while none was being edited (after `Dialog Clr`)
  wrote through a null pointer.
- With the mouse over no screen, the line editor compared a leftover
  register with its screen number; this now counts as outside the line.
- The zone lists were also searched at their end marker, reading past it.
- `Dialog Open` of an empty program reported its syntax error at a
  meaningless position; it is now 0.
- `Read Text` of a file with no memory for it read the file into whatever
  address a register held; it is now "out of memory".

File selector
- With no memory for its data, the selector "cancelled" through a null
  structure and an undefined stack pointer; it now returns no file.
- When the dialog failed in the selector's own task, that task jumped to
  the cancel code with the main task's stack pointer; it now stops, and the
  selector cancels.
- The two tasks could lose each other's commands (show the files, quit...):
  the task read and cleared the command word in two steps.
- A zone number outside the selector's 20 ran code past the end of the jump
  table; it is ignored.

Text windows (AMOS.library)
- Printing with autoback on a double-buffered screen saved and restored
  the cursor background from the window structure (`EcCurS(a5)`, a5 being
  the window) instead of the screen; it now keeps the screen's.
- Window addresses, background buffer sizes and offsets were partly
  computed in 16 bits, wrong on screens of more than 64 KB a bitplane.

Drawing (AMOS.library)
- `Paint` kept the offset of the clip rectangle in a word and added in 16
  bits: wrong on screens of more than 64 KB a bitplane.
- `Get Fonts`/`Get Disc Fonts`/`Get Rom Fonts` opened diskfont.library
  again at each call, never closing those openings.

Screens (AMOS.library)
- `Double Buffer` copied the bitmaps with a 16-bit count: on planes of
  more than 64 KB it wrote past the new planes, and it left the end of
  planes whose size is not a multiple of 16 bytes uncopied.
- A `Screen Open` that ran out of memory cleared the pattern through a
  null RastPort while freeing the screen, writing to address 8 (an
  exception vector).

Bobs and blocks (AMOS.library)
- `Get Cblock` stopped a run of equal bytes after 65 bytes but recorded
  64: the rest of that line of the block came out shifted by a byte, with
  a byte from past its end (blocks wider than 512 pixels).
- `Put Cblock` on a screen with fewer bitplanes than the block lost its
  place in the data after the first line; the extra planes are now
  skipped.
- `Vrev` of a one-line image swapped 65536 lines, overwriting memory.
- `Get Cblock`/`Put Cblock` added line offsets in 16 bits (screens of
  more than 64 KB a bitplane).

`tests/amos/blocks2.asc` covers these; its golden results come from the
C++ build.

Hardware sprites (AMOS.library)
- A 16-colour sprite above number 7 hung AMOS when no two neighbouring
  free sprite channels were left: the search skipped half-free pairs
  without counting the try.
- `Set Sprite Buffer` of 2046 lines or more sized the three buffers with
  a 16-bit multiplication: it reserved too little chip memory and the
  buffers ran past it.

Mouse (AMOS.library)
- `Limit Mouse n` for a screen that is not opened set limits read from
  the screen table taken for a screen; it now returns "screen not
  opened" without changing them.

Editor
- Listing variable names with capitals (the editor's "first capital"
  setting for variables) took the capital from the token instead of the
  name and lost its place in the line, garbling the rest of it.
- Without memory for its undo block, storing a line wrote the block's
  header at address 0.
- Redrawing one column of the window (scrolling sideways by one) with a
  marked block compared the window's line numbers with the program's,
  and drew the first line of a block of several lines in inverse left of
  the block's start instead of from it.
- With no undo buffer (an undo size of 0), typing over a character wrote
  the old one through a register left from other code.
- Out of memory, keeping a cleared part of a line for undo still said it
  was kept: the entry was marked as having a block of data, and undoing
  it (or the entry being reused) freed whatever its line number pointed
  to.
- Hexadecimal and binary constants were listed with as many digits as a
  register left over from other code said; they are now always listed
  without leading zeros, as that register usually made them.

Collisions (AMOS.library)
- The offsets into the masks of the two images were 16-bit: an image whose
  mask row times the lines above the crossing reached 32K (a 320-pixel
  wide image, below line 819) was tested at the wrong place, and
  collisions were missed. Test collide2.
- `Spritebob Col` with no bob at all walked a list starting at address 0.

Input handler (AMOS.library)
- The console.device request `ConIo` was 40 bytes, but its opening clears
  80 bytes and the device uses 48: it overran into the key conversion
  buffer that followed. The handler converts into a buffer of its own
  now, so `ConIo` has its full size.
- console.device was opened at the start and never closed.

ARexx
- A message arriving at the ARexx port that is not an ARexx command was
  taken off the port and never replied to, so its sender waited for ever;
  it is now replied to.

## Assembler compatibility notes

- Fixed in the sources for the build only, without changing the code:
  `bne.` and `cmp.s` typos that GenAm tolerated, and two Latin-1 characters
  in `+Editor_Config.s` strings that had been replaced by U+FFFD.

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

Interactive tests: `tests/amos/<name>.input` scripts keyboard and mouse
events (`tests/xinput.py` sends them through the X server's XTest
extension; the emulator grabs the input for such tests). `sync NAME` waits
until the program writes `AMOSTEST:out/NAME`, and `shot NAME` saves a
screenshot that is compared pixel for pixel. `menus` drives the menu bar
with the right mouse button. Mouse positions are hardware coordinates: a
lowres screen at its default position starts at 128,50 (`X Mouse=`/`Y
Mouse=` put the pointer there first).

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
