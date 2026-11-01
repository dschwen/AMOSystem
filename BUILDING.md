# Building AMOS Professional on a Unix host

Requirements: the Amiga cross toolchain from
https://github.com/AmigaPorts/m68k-amigaos-gcc (installed in `/opt/amiga`, or
pass `AMIGA_PREFIX=...`), GNU make and python3. AMOS Pro is C++ now: there is
no assembler in the build.

```
make            # build/AMOSPro
make dist       # build/dist/AMOS: the AMOS tree with the new AMOSPro
make host       # build/host/amos: AMOS as a program of this computer
make host-sdl   # the same linked to SDL2 (build/host-sdl/amos)
make install-host [PREFIX=~/.local]    # install it (amos, amos-player); make uninstall-host removes it
make game GAME=prog.AMOS [DATA=dir] [NAME=name]   # build/game/NAME: the program with the player
```

`amos-player prog.AMOS` (or `amos -r prog.AMOS`, AMOSPro's `-r` on the
Amiga too) runs a program only: no editor, no direct mode. The program
starts as the editor's Run starts it (the default screen opened), its
folder is the current one, and AMOS stops when it does: at its end, or at
an error, which is told (the message from the editor's configuration) and
gives the exit status 1. `make game` makes a folder that runs on its own
wherever it is put: `NAME` (the launcher), `bin/amos-player`,
`share/amos/AMOS` (APSystem and the interpreter's configuration, 2MB),
`share/amos/game` (the program and the files of `DATA`). Text files that
`Line Input #` reads need AMOS's line ends, CR LF, as on the Amiga.

The installed `amos` (`PREFIX/bin/amos`, the AMOS tree in `PREFIX/share/amos/AMOS`)
runs from any directory: it finds its tree from where the program is
(`AMOS_HOME` names another), and runs in it. Where it was started is the
volume `START:`: `amos prog.AMOS` or `amos /home/me/prog.AMOS` runs
`START:prog.AMOS`. The disks are the devices directory: `AMOS_DEVICES`, else
`devices` in the directory it was started in, if there is one, else
`~/.local/share/amos/devices` (`$XDG_DATA_HOME/amos/devices`), which `make
uninstall-host` leaves. The topaz font is made at build time from a Kickstart
(`KICKSTART=`, `ROMKEY=`, see `make host`); the installation needs no ROM.

## What replaced the original Amiga tools

AMOS Pro was built on an Amiga with HiSoft GenAm and helper programs in AMOS
BASIC. The conversion moved through these stages, and what is left of each:

| Original (Amiga)              | Stage                            | Now |
|-------------------------------|----------------------------------|-----|
| `c/Genam`, `c/genim2`         | `vasmm68k_mot -devpac`           | the assembler is gone |
| `c/Make_Labels`, `c/Library_Digest` | `tools/make_labels.py`...  | `src/core/lib_routines.h`, `src/tables` |
| `c/Make_Toktable`             | `tools/make_toktable.py`         | the verifier's table is in `src/tables` |
| `c/Get_Chunk`                 | `vasm -Fbin`                     | the editor's configuration of the distribution is patched by `tools/blue_palette.py` |

What the assembler sources held (tables, data, the glue's entries, the
layouts) is C++ in `src/tables` and `src/core`; see below. The original
sources are in the history of the repository.

## How the programs are linked

AMOSPro is C++: `src/lib`, `src/main`, `src/verif`, `src/interp`, `src/ext`,
`src/hal`, `src/w` (AMOS.library), `src/editor`, `src/monitor`, `src/core`,
`src/runtime` and `src/tables`, compiled with `m68k-amigaos-g++` and linked
with gcc and `link/amospro.ld`.

`src/tables` is what the assembler sources were: their tables, data and glue
entries, converted once to C++ (one file for each assembler source: its
bytes as an array, the relocations of its addresses, its entries). Nothing
generates them any more; they are the source.

- each label is a place in a data array, byte for byte what the assembler
  laid out (the jump tables AMOSPro patches, the token tables, the messages,
  the default mouse bank, the libraries' images). The labels that the C++
  uses are constants in `tables/labels.h` (`extern uint8_t *const name`);
- each entry of the assembler (a glue macro, a `bra` table, a routine
  written in assembler) is a record of `GlueEntry` (`src/runtime/glue.h`):
  what it was and the C++ function behind it. `src/runtime/glue.cpp` does
  what the macro did when something calls the entry's address (`amos_call`
  and the others, the interpreter's tables, the programs' routine lists);
- the exits that reset the stack (`BasSp`, the PRUN and editor-zap stacks, a
  menu's procedure, the monitor's step) are marks set with `setjmp`
  (`src/runtime/marks.h`, `runtime.cpp`, `monitor.cpp`); the `BasSp` the data
  zone keeps is the address of the mark.

`src/amiga` is what is machine code by nature: its start (`start.cpp`: the
program's first code, its own 64K stack, `Get_Out`), and `native.cpp`:
`setjmp`/`longjmp`, the call of code at an address with all the registers
(the machine code of programs, the system's libraries), the math and
narrator libraries' calls, and the entries the system calls (the VBL server,
the input handler, the patched requesters and LoadView, the audio interrupts
of the samples). `amiga/start.cpp`'s entry is first: `.text.entry` is first
in the linker script. This `ld` writes no memory flags, so
`tools/chiphunks.py` marks the `.datachip`/`.bsschip` hunks as chip memory
after linking, using the map file.

AMOS.library, the editor and the monitor were programs of their own that
AMOSPro loaded with LoadSeg. They are linked in: "loading" the editor or the
monitor puts their entry tables (`ed_debut`, `mon_debut`) into the library's
jump table (`src/main/modules.cpp`); KILL EDITOR no longer gives their memory
back. AMOSPro.Lib and the four extensions are libraries in the raw format
AMOSPro loads and patches: their images (the token table, the jump table,
the title) are in `src/tables`, their routines C++. The loader takes library
0 from its image, and an extension from its image when the configuration
names its file; any other extension is still loaded from its file. The
`AMOS/APSystem/*.Lib` files of the distribution are what the tools read
(the tokenizer reads their token tables).

`extensions/` holds extensions of other authors (Intuition, Personal, Misc):
their binary libraries, manuals and examples. AMOSPro still loads such a
library from its file when the configuration names it; their assembler
sources were removed with the rest of the assembler (they are in the history
of the repository).

C++ is compiled for the 68000 with `-Os`, without exceptions, RTTI or a C
library startup. libgcc and libnix13 supply arithmetic helpers such as
`__divsi3`.

## Adding or changing a routine

The routines of AMOSPro.Lib are C++ in `src/lib`, `src/interp`, `src/main`
and the other directories, numbered as the library's jump table is
(`src/core/lib_routines.h`: add new routines at its end). An instruction or
function that the tokens table names is called through its entry in
`src/tables`: a record that says what the assembler's glue macro said
(`LibCall`: the C function, the number of its integer parameters, the kind
of result; `WEntry`: a function of `AllRegs *`; `RegRoutine`...) and where
its slot in the library's jump table is. The ChrGet, the loop that runs a
program's instructions, is C++ (`src/interp/chrget.cpp`): it recognises an
entry of kind `libcall` with integer parameters or none, evaluates its
parameters and calls the C function itself; every other routine goes
through `interp_exec`.

The tokens table of the library, the table the `.AMOS` programs' token
numbers are offsets in, is part of `src/tables` (the library image,
`main_library`, and those of the four extensions): the offsets must not
change. `tools/amostok.py` (the tests' tokenizer) reads the same tables
from `AMOS/APSystem/*.Lib`.

### The C++ side (`src/core/amos.h`)

- `amos::idata()` is the interpreter data above a5 (`IData`), and
  `amos::wdata()` the AMOS.library data below it (`WData`, the `T_` fields
  without the prefix). `src/core/layout.h` has these and every other
  structure of AMOS's data (`Screen`, `Bob`, `Window`, `Program`...), the
  offsets of their fields as the assembler had them (`static_assert`ed), in
  big-endian types (`core/endian.h`) on a little-endian host; and the
  AMOS.library function numbers (`amos::sy::WaitVbl`, `amos::wi::Locate`...).
  `src/core/lib_routines.h` has the routine numbers (`amos::L_Demande`).
- The interpreter's registers (d6-d7/a3-a6) are in the task's context,
  `ctx()`: one for the main task, one for the file selector's (it runs
  Interface code in a task of its own while the main task reads the
  directory). On the Amiga the contexts are a block that `amos_bridge` points
  to (`src/amiga/start.cpp`); AMOS.library gets its address in `T_CBridge`.
  `lib_call(L_x, regs)` (`JJsr`), `sy_call`/`ec_call`/`wi_call`
  (`SyCall`/`EcCall`/`WiCall`) with d0-d5/a0-a2 in an `amos::Regs` call the
  routine's entry (`src/runtime/glue.cpp`); the functions of AMOS.library's
  vector tables are called directly (`src/tables/w_direct.cpp`), after the
  monitor's patches (`w::sy_patch`, `w::ec_patch`). The context is put back
  afterwards.
- `error(n)`, `fon_call()` and `lib_jump(L_x, d0)` raise errors. They do not
  return: the interpreter resets its stack to its mark.
- Strings (`src/lib/strings.h`): `str_alloc` (`L_Demande`), `str_finish`
  (`L_FinBin`), `str_empty`.
- Calls may not return: errors reset the stack, and a full string space
  restarts the whole instruction after the garbage collection. So no C++
  object with a destructor may be alive across such a call, and side effects
  belong after the string allocation, as in the original.
- Interrupt code (`w_vbl`, the input handler, the audio interrupts) must not
  use the contexts' saving (`amiga_w_go`): it runs under another code.
- `lib_eval(L_x, regs)` calls a routine that reads parameters from the
  program itself (`MnDim`, the evaluator): the interpreter then continues at
  the a6 (and with the a3) it leaves.

Routines that other code calls with registers (`L_InstrFind`, `Bnk.GetAdr`...)
have a register entry: `RegRoutine` hands the C function all of d0-d7/a0-a2
in a `RegFrame` to read and change, and sets the condition codes from its
return value for callers that branch on them. The functions of AMOS.library
are `WEntry` entries: `void f(AllRegs *)`.

### The portability layer (`src/hal`)

Phase 7 of the plan separates what AMOS needs from the host. The C++
reaches the system only through `src/hal`, each part with an Amiga
backend (`*_amiga.cpp`):

- `hal/files.h` (dos.library): files, locks, directories, delete/rename,
  execute.
- `hal/system.h` (exec.library): memory, tasks and signals, libraries, the
  system's version and caches, message ports, devices and their IO
  requests, interrupt servers.
- `hal/gfx.h` (graphics.library, layers.library, diskfont.library): the
  RastPort drawing AMOS uses, fonts, bitmaps, clipping regions and
  layers, the blitter's ownership, the display's frames.
- `hal/workbench.h` (intuition.library, icon.library, console.device,
  rexxsyslib.library): the Workbench and Intuition's screens, the panic
  requester, icons, the keyboard's characters, ARexx's messages.
- `hal/chips.h`: the custom chips and CIA A, which AMOS.library programs
  directly (copper lists, bitplanes, sprites, the blitter, the audio, the
  joysticks). On the Amiga they are the hardware; another host gives an
  emulation of the registers that its renderer reads to compose each frame
  from the copper lists and bitplanes AMOS builds, as the chips do.

The values stay AmigaDOS's and the structures the Amiga's, which another
host then provides. The start-up gives the file and Workbench backends
their libraries before AMOS's data zone exists (`files_start`,
`workbench_start`).

`make host-check` (`tools/host_check.sh`) compiles every C++ file but the
Amiga backends for a 32-bit host, syntax only, with the NDK's headers for
the Amiga's structures (`-fpack-struct=2`: the 68000 aligns 32-bit fields
on 2 bytes; the generated layouts are packed so themselves). All of them
compile.

`make host` links the C++ into a Linux x86-64 program, `build/host/amos`,
with `src/host` and no assembler:

- the bridge: the same `src/tables` and `src/runtime` as the Amiga's, each
  table label an address the C++ keeps as on the Amiga; the stack-resetting
  exits of the interpreter are `setjmp` marks (`runtime/runtime.cpp`);
- memory below 4GB (`MAP_32BIT`, as AMOS keeps its addresses in 32 bits),
  given out as exec does; AMOS's data (programs, banks, token tables)
  stay big-endian (`core/endian.h`);
- files: POSIX, with AmigaDOS's names; the current directory is
  `AMOSPRO:`. The disks are the **devices directory**: each directory in
  `devices/` (in the directory AMOS starts in if there is one, else in the
  user's data directory; `AMOS_DEVICES` names another)
  is a volume, and `Work:Programs/game.AMOS` is the file
  `devices/Work/Programs/game.AMOS` (names are not case-sensitive, as
  AmigaDOS's). A new devices directory comes with `RAM` (`RAM:`, whose
  volume is "Ram Disk") and `Work`; a directory made in it appears at once
  as a volume, in the file selector's "Devices" too. Assigns (`AMOS_ASSIGNS`,
  `c:assign`, AMOS's own AMOSPro_...) are listed apart, as on the Amiga;
  Load, Save and the editor's file selector use them all;
- the file selector's task, and any other, runs while the program waits
  (`host/tasks.cpp`: coroutines, no pre-emption);
- the custom chips as registers, a copper and a renderer
  (`host/display.cpp`: bitplanes, hires, dual playfield, EHB, HAM,
  sprites) and a blitter (`host/hal_chips.cpp`);
- a window (`host/sdl.cpp`: libSDL2 loaded at run time, or linked to it
  with `make host-sdl` (`build/host-sdl/amos`, no SDL headers needed);
  magnified twice)
  whose keyboard and mouse become the Amiga's input events;
- Paula (`host/audio.cpp`): the four voices played by their DMA from the
  registers, the audio interrupts at each block (exec's vectors,
  `SetIntVector`), mixed each frame into SDL's sound;
- topaz 8 and 9 and the usa keymap (its Alt characters and dead keys)
  from the user's Kickstart (`tools/extract_topaz.py`,
  `tools/extract_keymap.py`; `KICKSTART=` and `ROMKEY=` for an encrypted
  one; the results stay in `build/`; without one, boxes replace the
  characters and a plain table the keymap);
- graphics.library's drawing as the Kickstart's (measured with probes in
  the emulator): the blitter's lines (their rounding, the pattern's counter,
  the first dot in COMPLEMENT mode), AreaEnd's fill, the multi-coloured
  patterns, the algorithmic styles of Text, RectFill's outline;
- chip memory below 4MB (24-bit addresses, as the Amiga's), apart from the
  other memory; NIL:; DOS's "Please insert volume" requester (AMOS's, with
  Request On) for a volume that is not there;
- the copper's colour changes during a line where the beam is then.

`build/host/amos` starts the editor in `build/dist/AMOS`; with a program
as its argument (`AMOSPRO:...`, `AMOSTEST:...`) it runs that. The
monitor needs `APSystem/AMOSPro_Monitor_Resource.Abk`, which is not in
the sources (copy it from an installed AMOS Pro). Its stack-resetting
entries (MonitorChr, Step_Out, Tt_Error, QUIT, the evaluator's error
exits) are `src/host/monitor.cpp` on the host.
`tests/host_run.py` runs all the tests of `tests/run.py` with it, and
`tests/host_only` (what only the host has: its devices), comparing every
file they write (the IFF pictures too) with the golden ones. The editor and
input tests run in a window of their own (`AMOS_SCALE=1`: a hires pixel to a
pixel, a line to two, as FS-UAE shows them) on a virtual display (Xvfb),
driven by the same input scripts, their screenshots compared with the
Amiga's. Their test drive has an `S` folder and a `Workbench:` volume as the
emulator's has; RAM: and the devices directory are beside it. A few results
differ from the Amiga's in known ways (the maths libraries' last digit, a
serial write the emulator never completes, the devices and assigns, the
compiler's extension, and four screenshot details, three not understood
yet): `tests/host_only/variants` has the
host's own result, with the reason of each (its README). The environment:

| Variable | Use |
|---|---|
| `AMOS_ASSIGNS` | `NAME=path;...`: assigns |
| `AMOS_HOME` | the AMOS tree (found from the program by default) |
| `AMOS_DEVICES` | the devices directory |
| `AMOS_DISPLAY=none` | no window, no sound |
| `AMOS_SCALE` | the window's magnification (1-9, 2 by default) |
| `AMOS_SHOT=file.ppm` | the last frame into that file; with `AMOS_SHOT_FRAME=n`, frame n |
| `AMOS_WAV=file.wav` | the sound recorded into that file |
| `AMOS_TRAP=seconds` | stops the program there, for a debugger |
| `AMOS_COPPER`, `AMOS_MEMCHECK` | debugging: the copper lists, checked frees |

What the host does not do like the Amiga:
- Libraries and devices are stand-ins. A library opens with a jump table
  whose calls (Doscall, Execall, Lib Call) do nothing and answer 0, but
  exec's FindTask and dos's Delay. input.device and console.device are the
  window's; timer.device, serial.device and narrator.device open and
  complete what they are asked at once (no serial port behind the serial
  device; its settings at OpenDevice and the checks of SDCMD_SETPARAMS are
  Kickstart 3.1's, measured). The narrator speaks when espeak-ng is installed (libespeak-ng,
  loaded when AMOS first says something, as SDL is: `host/speech.cpp`); the
  host's translator.library gives the text back for the phonemes, so it is
  the text that is said; `AMOS_SPEECH_WAV=file` also writes the speech to a
  file. (Tried with the Ubuntu packages libespeak-ng1 and espeak-ng-data, unpacked
  with `dpkg-deb -x` and used through `LD_LIBRARY_PATH` and `ESPEAK_DATA_PATH`:
  a sentence of eight words came out as 2.3 seconds of speech.) Without espeak-ng, or without sound, Say is silent and its mouth
  stays shut. The IO requests are in the 68000's layout on every host
  (`core/ioreq.h`), so a program's Peeks at a device's block (Serial
  Base(n)+n) read what they read on the Amiga.
- ARexx ports are in-process only: Arexx Open/Close/Exist work, no message
  arrives.
- A command run with Exec: only `Wait n` and `c:assign`.
- The last digit of transcendental functions: the Amiga's math libraries
  (mathtrans, mathieeedoubtrans) have their own series, the host uses its
  libm; the results differ in the last digit of `expr2`, `floats`, `floats2`
  and `maths`.
- Addresses 1 to 4095 are a page of their own (ExecBase at 4), so that
  `Leek(4)` and `Btst(n,4)` work; the Amiga's vectors are not there.
- The memory is exec-like (8-byte blocks, parts of a block can be freed and
  taken again: `Bank Shrink`).

### Deliberate differences from the original

Bugs of the assembler that the C++ fixes. Everything else keeps the
original behaviour, quirks included (the tests record the original
binaries' results). Where a fix changes a recorded result, the golden file
has the corrected one.

The editor's default palette is blue instead of teal (`Ed_Palette` of the
editor's configuration: colours 2, 5 and 6 are $037, $5AE and $27C instead of
$077, $0DD and $0AA; `make dist` makes it with `tools/blue_palette.py`). It is a change of taste, not a fix: a config saved
by the editor keeps its own palette, and `make install-hd` leaves the
installed configs alone. The emulator tests run with the original config,
which their editor screenshots were recorded with.

`Dev First$` and `Prg First$` listed only the devices, or only the assigns,
when the name of the current directory began with `D`, respectively `A`:
they took that letter for the kind of list the file selector asks for
(its "Devices" and "Assigns" buttons). They list both now.

`Save Iff`'s packer (ByteRun1) looks up to two bytes past the end of the row
it packs. On the last row of the last plane that is past the screen's
planes (each of the others is followed by the next), in whatever memory
follows them, and those bytes could change how the row was packed (not the
picture). They are read as zeros now, as they were on the Amiga when the
golden files were recorded.

A menu's procedure (the `(PR name)` object) was called with no program
pointer, and the call looked for its parameters in the word at address 0
(0 on the Amiga, so none). It does not read there now.

AMOSPro's command line has an option of its own, `-r` (run only): the
program given is run as the editor's Run runs it, and AMOSPro stops when it
does, an error told, instead of going to the editor (`amos-player` on the
host). Without it, nothing changes.

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

Programs in memory (the verifier)
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

Start-up and system files (the main program)
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

## Verification

The emulator tests (below) check AMOSPro against the results recorded from
the original binaries. The editor's configuration in `build/dist` is the
prebuilt one with the palette made blue (`tools/blue_palette.py`: the only
change, see the deliberate differences); the tests run with the original
configuration their screenshots were recorded with.

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
