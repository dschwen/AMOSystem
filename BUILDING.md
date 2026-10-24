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
| AMOSPro | `+B.s` (+ `+Verif.s`) | `src/lib`, `src/main`, `src/verif`, `src/interp`, `src/ext` | `link/amospro.ld` |
| AMOS.library | `+W.s` | `src/w` | `link/module.ld` |
| AMOSPro_Editor | `+Edit.s` | `src/editor` | `link/module.ld` |
| AMOSPro_Monitor | `+Monitor.s` | `src/monitor` | `link/module.ld` |

`src/core` (the C++/assembler bridge) is an archive linked into all of them. The linker scripts keep
the assembler code in the first hunk: it holds the entry point or entry
table. AMOSPro's init hunk `I` must come second, because `Init_Fin` frees it.
This `ld` writes no memory flags, so `tools/chiphunks.py` marks the
`.datachip`/`.bsschip` hunks as chip memory after linking, using the map
file. The extensions are still assembled directly: AMOSPro loads them raw
and patches them, and they cannot contain linked code. AMOSPro.Lib is
assembled the same way, but also linked into AMOSPro as data
(`src/main/library.s`): the loader takes library 0 from that image, and
the file is only built for the tools (the tokenizer reads its token table).

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
   `+Internal_Jumps.s` (`AMOSJmps`), and `Library_Patch` (src/main/libraries.cpp) writes the address
   from that list into the jump table at start-up.
2. Write the entry point in `src/lib/*_glue.s` with the macros in
   `src/glue.i`. For an instruction or function this is one line,
   `LibCall Name,_c_function,params,Int|Float|String|None`: the parameters
   go to the C function as 32-bit values (a string as the address of its
   length word), and its return value becomes the AMOS result.
3. Implement the routine in `src/lib/*.cpp` as an `extern "C"` function.

The ChrGet, the loop that runs a program's instructions, is C++
(`src/interp/chrget.cpp`). Each `LibCall` entry has a tag before it (its C
function, parameters and result, then `"LCal"`, then the `"GetP"` slot of
the loader). The loop and the evaluator recognise an instruction or
function whose routine is a tagged entry with integer parameters (the
collectors `Ent_1`...`Ent_N` of `L_Parameters`, which the loader put in
front of it) or none, and evaluate its parameters and call the C function
themselves. Every other routine is called through `interp_exec`
(`chrget_glue.s`), with the parameter collector in front of it and the
interpreter registers of the context. `Prg_InsRet`, where instructions
that jump go back to, is `interp_iret`, which enters the loop again at a6.

The extensions (`extensions/+*.s`) are still libraries of their own, but
their routines move into AMOSPro the same way: `Lib_Int` in the
extension, the entry `Ext_<Extension>_<routine>` in `src/ext/*_glue.s`.
`tools/library_digest.py` writes the extension's list
(`+<Extension>_Jumps.s`: its number of routines, then each number and
entry), and `Library_Patch` (`ExtJmps` in +B.s) writes them into the jump
table of the slot the extension is loaded in, when that library has the
same number of routines and those are empty. An extension reaches its own
routines PC-relatively (`Rbsr`), its `Rjsr` goes to the main library: a
call from extension code into one of its routines in AMOSPro goes
through the jump table at `AdTokens+slot*4(a5)`.

A helper that +ILib.s reaches with local branches (`bsr FindVar`) keeps a
one-line `Label Rjmp L_Label` in the library. If it had no number of its
own, it takes the slot of a routine of the dropped compiler: the empty
`Lib_Def`s `PlusF`... and `Cmp...` at the start of +ILib.s are only
referenced by APCmp (`FindVar` took `PlusF`'s).

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

AMOSPro starts on a stack of its own (64K, StackSwap in Cold_Start, given
back in Get_Out): the C++ needs more than the 4K a CLI gives by default,
and overflowed into the memory below (a menu object of the editor, freed
later through its broken header; which block it hit depended on where
things were loaded).

Strings and numbers
- `Flip$` took its length from a register whose high word was not cleared.
  Depending on what ran before, it failed with "out of string space"
  (error 11). It now always works; `tests/golden/strings.txt` has the
  corrected line.
- `Max` and `Min` of two strings compared the address of the second string
  with a leftover register instead of the strings, so `Max("b","a")` gave
  "a". They now compare the strings (`tests/golden/expr.txt`,
  `expr2.txt`).
- `Struc(address,"field")=value` on a byte field (types 0 and 3) wrote the
  type's index (0 or 6) instead of the value. `tests/golden/equates.txt`
  has the corrected line.
- `Struc$(address,"field")=a$` made the field point to the copy's length
  word instead of its characters, so the structure got no C string and
  `=Struc$` read "" back; `=Struc$` of a field holding 0 crashed. The
  original stops on that crash, so `tests/golden/equates2.txt` was recorded
  with the corrected version.

Banks, files and directories
- `=Lib Call(channel,f)` on a channel with no library open crashed AMOS
  instead of giving error 169 ("Library not opened"); it gives the error
  now (`tests/golden/libcalls.txt` has that line from the corrected
  version).
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
- The selector's task has 16K of stack instead of 2K, for the C++ it runs.

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
- Cutting a block of several lines moved the marks (Set Mark) from a line
  number never set (the routine loaded the count twice into the same
  register).
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
- Out of memory, starting to record a macro, or keeping the one recorded,
  went on with no buffer and wrote the macro's key at address 8; it is now
  "out of memory" (the recording buffer is freed).
- A resource bank that failed to load left its file open.
- Deleting a user menu option took away the key shortcut of the function
  before it instead of its own, so an option added later in its place got
  the old shortcut. Test editor_usermenu.
- A menu object that could not be made (out of memory) stopped the editor
  on an illegal instruction; the item is now left without it.
- Restoring the last session (auto-resume): running out of memory while
  making the program or window structures again left the last one linked
  to its old address, which the clean-up then followed; and the editor went
  on with the current window pointing into the freed loading buffer.
- Set Text Buffer to a larger size without the memory for it forgot the
  old buffer and returned with four longs left on the stack (a crash); it
  now goes on as for a smaller size (the program erased, saved first if
  asked).
- Running an accessory (or a menu option's program) that was running
  already said "out of memory": the result was tested on the flags of the
  instruction clearing Edt_Runned. It now says the program runs already.
- Call Editor cleared a flag of the data zone (Ed_YaUTest) through the
  editor window's pointer: one byte 2566 bytes into a 246-byte window
  structure was zeroed, and the flag itself never was.

Monitor
- Each alert in the information window also stored a separation line of
  a kind nothing showed or deleted (its kind was the message's length,
  left over in a register): the information buffer filled up with them,
  and after enough alerts no expression could be added.
- Help's loading of AMOSPro_Help.Map had its two errors swapped: a file
  that could not be opened or read said "Out of memory", and too little
  memory said "Cannot load help .Map file". A failed read also left the
  file open.
- A monitor that failed to start (too little memory, no resource bank)
  freed its data but left Mon_Base pointing at it: a program calling the
  monitor afterwards took it as running and wrote into the freed memory.
  Its clean-up also set the interpreter's error routine (Prg_JError) to
  the one it had not saved yet, 0, so the program's next error jumped to
  address 0; and when even the data could not be reserved, it set the
  current banks from whatever a4 pointed to.
- Splitting a line into its parts (for the next instruction shown, and
  for evaluating an expression dragged over) looked an operator up at a
  negative offset from the instructions' token table. The operators are in
  the interpreter's own table, so the kind of `=`, `+` and so on was
  whatever byte lay in memory before the instructions. When that byte was
  0, the part ended the instruction there ("Next instruction: A$" for
  `A$="hello"`, and "Cannot evaluate expression"), depending on where
  things happened to be loaded. Operators are now always 'O', the type
  their definitions give.
- Closing help's text reader tested the first long word of its screen
  instead of whether the screen was open, so it read address 0 when the
  screen had failed to open.

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

Programs in memory (+Verif.s)
- `Prg_Save` left the file open when writing it failed (a full disk, say),
  so it stayed locked until AMOS quit. It is closed now.
- The table of `Include`s had room for 15 of them, and a program with more
  wrote past its end. The table is now made as large as the program needs.
- The variables of direct mode (`ResDir`), with less than 6 bytes free
  between the strings and the variables, took a size of -6 and cleared
  64K of memory. They are now left out, as with no room at all.

Single precision floats as text
- Printing (or `Str$`) a zero in the exponential form (`Fix` negative)
  crashed AMOS: that path of FloatToAsc left four registers on the stack.
  It now gives the intended `0E+00` (`0.0E+00` with decimals).
- `Val` (and the tokenizer) of a number with 21 digits or more overwrote
  the saved registers in the stack frame of the conversion, and an
  exponent of 4 characters or more (`1e0005`) overwrote the start of the
  mantissa, giving 0. Both now read correctly.
- `Val` of a double with a huge exponent (`1e999999999`) multiplied by 10
  that many times, which took hours. The power of 10 now stops once it
  has overflowed, with the same result.

Compact extension
- `Pack`/`Spack` packed one or two bytes of uninitialised memory at the end
  of the picture (and the size estimate counted an uninitialised byte at
  its start): the temporary buffer was not cleared. It now is, so a bank
  packs the same every time; pictures unpack the same as before.

AMAL
- `Amal n,...` on a channel that existed, whose object (a bob, a screen)
  did not, freed the channel and then linked the freed block into the
  list of the channels, which could crash later; the channel is now
  removed and the error given.
- `BC(...)` and `SC(...)` with Synchro On (in the VBL interrupt) gave 0
  without reading their arguments, so the animator read them as
  instructions; they are now skipped.

Music extension
- After any change of the banks, a MED module playing was always stopped:
  the check compared the second half of the bank's name with the first.
  It now stops only when the module's bank has gone.
- The tracker's arpeggio on a voice without a period read past the table
  of periods, into the player's variables; it now reads zeros there.

IO_Ports extension
- `Printer Dump x1,y1 To x2,y2,columns,rows,special` left three registers
  on the stack and crashed AMOS on its return; it now prints.
- `Printer Dump` without the memory for its colour map took a word off the
  stack that was never put there; it now does nothing.
- `Parallel Out` sent the printer's raw-write command, which for the
  parallel device is a status query: nothing was written. It now writes.

ARexx
- A message arriving at the ARexx port that is not an ARexx command was
  taken off the port and never replied to, so its sender waited for ever;
  it is now replied to.

Start-up and system files (`+B.s`)
- A mouse file named in the interpreter configuration was never loaded:
  the start measured its size by seeking to its end, then stayed there and
  read nothing, so AMOS stopped with "Cannot read system files". It now
  seeks back to the start.
- If the monitor program could not be loaded, `Monitor` (and the editor's
  monitor) went on as if it had been, and jumped into routines that were
  not there; it now gives a disk error. When its resource bank failed to
  load, its file was left open; it is now closed.
- A name on the command line (or `-c` configuration name) longer than 127
  characters overran the start's buffer; it is now cut there.

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
