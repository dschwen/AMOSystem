# Plan: Turbo Plus and AMCAF in the C++ AMOS

Status: proposal, nothing built. The two extensions are third-party libraries for AMOS
Professional: **Turbo Plus V2.15** (slot 12 by default) and **AMCAF V1.50beta4** (slot 8 by
default). Programs that use them, such as the Ghostlight game, can neither be edited nor run
on the C++ builds today. This plan says how to change that, host first.

## 1. What there is to work from

The reverse-engineering of both libraries is in `../amoscript/tools/reference` (with the
write-ups in `../amoscript/docs/turbo-plus.md` and `amcaf.md`). It is static analysis of the
original binaries, not captures from a running Amiga:

| | Turbo Plus V2.15 | AMCAF V1.50beta4 |
|---|---|---|
| Machine code | 25,314 bytes, 340 routines | 38,250 bytes, 399 routines |
| Commands | 153 entries | 347 entries, 280 distinct names |
| Catalog (names, signatures, routine numbers, overloads) | complete (`catalog.json`) | complete |
| Disassembly (IRA, relocation-aware) | `ira.s`, `disassembly.txt` | the same |
| Behaviour worked out in detail | integer functions, memory search/fill, checks, cursor graphics, pixels, planes, icons, scenes, objects, stars | integer and string helpers, fast maths (exact tables), C2P Shift/Fire/Convert, Pal Spread, screen transitions, Protracker queries |
| Example programs | 33 tokenized, covering 69 of 153 commands | 2 tokenized (Fire, ScreenTrans), 10 more in the AMOScript repository |
| Manual | in AmigaGuide (AMCAF's transcribed to `manual.md`) | |

The AMOScript project (JavaScript, for browsers) already implements about 80 AMCAF entries
and part of Turbo Plus, with independent test vectors (`check_amcaf_*.py`,
`test/amcaf-*.test.js`). Those are a second source for the semantics and for expected values.

**Missing here:**
- The original `.Lib` files and the manuals are not on this machine (the paths in the
  reference's reproduction commands are gone). The catalogs are enough to build the token
  tables; the binaries are needed to record real results.
- No command has ever been run under real AMOS. The reference says so itself.

## 2. How an extension works in this port today

Facts from `src/main/libraries.cpp`, `src/ext/*` and `src/tables/ext_*`:

- The four extensions (Music 1, Compact 2, Request 3, IO_Ports 6) are C++ in `src/ext`. Each
  has an **image** (`src/tables/ext_X_image.cpp`: header, token table, title) and **glue
  entries** (`ext_X_glue.cpp`: one record per routine with its C++ function and parameter
  kinds), plus generated typed bindings in `src/runtime/bindings`.
- A configuration (`AMOSPro_Interpreter_Config`, messages 16–41) names a file for each slot
  1–26. The loader picks the linked-in image **by file name**; on the host a name without an
  image is skipped ("the host runs only the C++ extensions"), on the Amiga it is loaded from
  its file.
- Three things tie an extension to its slot and must become dynamic for ours:
  1. `main_ext_jmps` (a table in the converted `+B.s`) binds the C++ routines to jump-table
     entries **by slot number**;
  2. each extension has `constexpr ExtNb = N - 1` to find its data in `idata().ExtAdr`;
  3. `library_init` refuses an extension whose cold start does not answer `slot - 1`.
- Tokenized programs hold the **token offset** in the library's table, so each token table
  must be byte for byte what the original has (names, signatures, padding). The slot number
  is in the program too (`$004e`, slot, offset): a program written for slot 12 needs the
  library in slot 12.
- The chips are emulated at register level on the host (`hal::custom()`: blitter on BLTSIZE,
  copper lists and bitplanes composed into frames, Paula DMA, CIA fire buttons) and VBL
  servers exist (`hal::add_vbl_server`). The Music extension already drives Paula this way,
  with the same source on both builds. That makes a literal port of the blitter, copper and
  audio commands possible instead of a re-interpretation.

## 3. Approaches

| | A. Port each command to C++ (**recommended**) | B. Run the original binaries on a 68000 interpreter | C. Translate the binaries to C++ mechanically |
|---|---|---|---|
| Result | readable C++ in `src/ext`, like the four existing extensions | `.Lib` files loaded as on the Amiga | generated C++ |
| Builds on the Amiga | yes, the same source | n/a (originals run natively there) | yes |
| Needs | the references, tests | a 68000 core, memory map hooks for chips, exec/graphics/dos stand-ins (about 40 library calls), host to 68k and back trampolines | a recompiler |
| Fidelity | as good as the analysis and tests | exact, if the OS stand-ins are | exact, if the translator is |
| Redistribution | our code (token names and behaviour are still derived, see section 8) | the original binaries must ship | a derivative of the binaries |
| Maintainability | good | the core is third-party code | poor, unreadable |
| Also gives | nothing beyond these two | every third-party extension, `Call` of machine code, `Doscall`/`Lib Call` | nothing |

A is the project's own approach and what was asked for (build on the Amiga, bundle on the
host). B is a larger, separate feature worth keeping in mind: it would run any extension
anyone has, but it does not give the Amiga build or readable ports. C is not recommended.

**Oracle.** The test environment can run the originals under FS-UAE (that is how the existing
goldens were recorded). Once the `.Lib` files are available there, every ported command gets a
golden from the real thing, which the AMOScript work never had.

## 4. Design

### 4.1 Tables from the catalogs, commands from C++

- `data/extensions/{turbo-plus,amcaf}.json`: per entry the offset, name, signature
  (including terminator and overload recall), routine number and family. Facts about the
  interface only, no code. Extracted from the reference catalogs by a script that checks them
  against the original binary's hash when it is at hand.
- `tools/ext_image.py` writes, for each extension: the image array (token table, title) as
  `src/tables/ext_X_image.cpp`, the glue entries, and the bindings `.inc` (extending
  `tools/sync_runtime_bindings.py`); plus a token-only `.Lib` for `tools/amostok.py`, so test
  programs can be tokenized.
- Every entry starts bound to `ext_unsupported(name)`, which raises an AMOS error ("Extension
  command not supported" with the command's name). **The token tables alone already make
  programs list, edit, save and reload correctly**, with the command names instead of
  `Extension 12 #036E`.
- Routines are then filled in by family (section 5); the commands that are `RTS` or unused in
  the original (Nop, Nfn, Rnp, Trans Screen Static, `Rnc Unpack`) stay as they are, reproducing
  the original's behaviour.

### 4.2 Slots

- Replace the slot-keyed `main_ext_jmps` lookup with a registry keyed by the **image** (a
  list of `{image, routines}` that `library_patch` binds wherever the configuration put it).
  The four existing extensions move to it too, so there is one mechanism.
- The extension learns its slot from the loader (`extension_slot()`), not from a constant.
  Cold start answers the slot it was loaded in. A second library in the same slot is already
  an error at load.
- Assigning a slot, in three layers:
  1. the distribution's `AMOSPro_Interpreter_Config` gets slot 8 `AMOSPro_AMCAF.Lib` and slot
     12 `AMOSPro_TURBO_Plus.Lib` (a patch step in `make dist`, like `tools/blue_palette.py`;
     the emulator tests keep the original configuration);
  2. a user can change it in AMOS's own configuration, since the names are config messages;
  3. host only, for convenience: `[extensions]` in `amos.conf` and `--extension SLOT=FILE`,
     applied after the configuration is read (`12 = AMOSPro_TURBO_Plus.Lib`; an empty value
     frees a slot). The F12 panel can show the slots later.
- Default slots 8 and 12 are free in the stock configuration (taken: 1, 2, 3, 5, 6).

### 4.3 State and lifecycle

- One guest data record per extension in `ExtAdr`, as the originals have (AMCAF: 9,142 bytes at
  `360(A5)`; `AMCAF Base` and `AMCAF Length` return its address and size), allocated at cold
  start, with the warm-start, default (reset) and end hooks. Turbo's R12e/AMCAF's R000 show
  what each releases: interrupts, blit/star/object/check storage, plane offsets, icon bank 2,
  palette and plane masks.
- VBL work (AMCAF mouse accumulator and the tracker's tick, Turbo stars and blitter
  interrupts) goes through `hal::add_vbl_server`, so it runs the same on both builds.

### 4.4 Code rules

- The typed-pointer rules of the guest-addressing migration apply from the start:
  `ContextCursor`, `GuestString`, `StringBuilder`, `bank_host_view` for bank memory; no native
  addresses in guest words.
- Same source for both builds: AMOS services (`lib_call`, banks, screens) and `hal::` only,
  warnings as errors on the m68k build too.
- The originals' quirks are kept where the references found them (Lsr is arithmetic, Vin is
  inclusive, Vmod's negative multiples, Between's strict endpoints, Check returns the zone
  number, ScreenTrans's `$8000` bias). Anywhere the port deliberately differs, it is added to
  the deviations list in `docs/BUILDING.md`.
- Operations that need the real machine (Exec task priority, Launch, Reset Computer, Open
  Workbench, library flushing, CPU/FPU queries, raw hardware reads) get host stand-ins in
  `hal/system` or a clear error, decided per command and listed in the manual.

## 5. Phases

Machine-code sizes below are the entry routines plus the helpers they call (from the catalogs),
a rough guide to the effort. All of this is for the host first; the Amiga build follows in
section 7.

| Phase | Content | Done when |
|---|---|---|
| **0. Inputs and decisions** | the licence question, the original `.Lib` files and manuals located, test configuration for the emulator | answers in section 9 |
| **1. Plumbing** | registry by image, dynamic slot, `ext_image.py`, `[extensions]`/`--extension`, dist config patch, tokenizer support, `ext_unsupported` | both libraries load in a chosen slot; `About Extensions` lists both; every Turbo and AMCAF example lists and round-trips (load, save, same bytes); moving AMCAF from slot 8 to 9 changes the listing as it should; the full host suite is unchanged |
| **2. Ghostlight set** | the 18 commands Ghostlight uses: Bank Temporary, Raster Wait, Flush Libs, Audio Lock, Pt Play/Stop/Volume/Cia Speed/Sam Stop/Raw Play/Sam Bank, Smouse Key, Odd, F Plot, F Point, Set Planes, Amos Pri, Word Hunt | Ghostlight starts and plays |
| **3. Computation** | AMCAF: Qsin, Qcos, Qarc, Qrnd, Qsqr, Vclip, Vin, Vmod, Binexp, Binlog, Lsl, Lsr, Odd, Even, Wordswap, Speek, Sdeek, Chr/Asc.w/l, the string functions (Lzstr$, Lsstr$, Insstr$, Cutstr$, Replacestr$, Itemstr$, Scanstr$, Count), the bank family (35 entries, 1.8 KB: code xor/add/mix/rol/ror, Stretch, Copy, Name, Checksum, Permanent, Temporary, To Chip/Fast). Turbo: shifts, L Swap, Test, Range, Texp, T Clip, Between, F Sqr, Bit Field, Byte/Word/String Hunt, Memory Fill, Move Mem, Bank End | every command has tests with literal expected values (the AMOScript vectors first, goldens from the originals when available) |
| **4. Graphics through AMOS structures** | Turbo: F Plot/Point/Draw/Circle (13 entries, 6.1 KB), R cursor commands, Plane Offset/Update, Set Planes, Icon/Scene (36 entries, 6.1 KB), Define/Reserve/Check/Hit zones, Object. AMCAF: Turbo Plot/Point, FCircle/FEllipse, Pal Spread/RGB/Red-Green-Blue Val, Pix, Ham, Shade, Bzoom, C2P Shift/Fire/Convert (3.4 KB), Trans (screen transitions), Change Font/Bank Font, Make, Examine/Object/Vec (3D) | Turbo and AMCAF example programs run as far as their other commands allow; pixel comparisons against goldens |
| **5. Chips, interrupts, tracker** | AMCAF Blitter (16 entries) and Turbo Blit (11 entries, 3.9 KB) on the emulated blitter; TD (Turbo Draw), Splinters, Stars; Raster/Cop; the Protracker player and samples (`Pt`, 31 entries, the largest family at 7.9 KB) on Paula; joystick, Smouse, Left/Right Click, Raw Key, Multi, Vbl Wait | frame-exact where deterministic; audio verified by captured sample output (as the Music extension's is) |
| **6. Remainder** | DOS commands (Disk State/Type, Dos Hash, Pattern Match, Filename$, Path$), Launch, Open Workbench, Imploder/PP/Rnc, Wload/Dload/Wsave/Dsave/Sload/Ssave, Io, Set Ntsc/Pal, Cpu/Fpu, Protect, extension management (Extdefault/Extremove/Extreinit) | each either implemented, or a documented stand-in/error |

Largest unknowns, by machine code behind them: Pt (7.9 KB), C2P (3.4 KB), Blitter and Pix
(2.5 KB and 2.3 KB), Turbo's F and Scene families (6 KB each). The AMOScript work covers
part of these; the rest has to be derived from the disassembly.

## 6. Testing

- **Unit vectors**: literal expected values for every command with defined semantics, starting
  from the AMOScript vectors (documented as independent transcripts, not Amiga captures).
- **Goldens from the originals**: with the `.Lib` files in the emulator's tree and a test
  configuration naming them (slots 8 and 12), `tests/run.py` records each command's results;
  the Amiga build with the ports linked and the host must reproduce them. Tests skip cleanly
  when the originals are absent, so the suite stays runnable from a checkout.
- **Example programs** (33 Turbo, 12 AMCAF): load, list and round-trip in phase 1; run
  headless to their first input in phases 4 and 5.
- **Tokenizer**: `tools/amostok.py` reads the generated token-only libraries, so test programs
  are written as listings.
- **Slot independence**: one test runs the same program with an extension moved to another
  slot, to catch any remaining constant slot.
- **Memory safety**: the guest-contract suites and sanitizer runs cover the new code like the
  rest; commands that the originals leave unchecked (Move Mem, String Hunt past its end, Alloc
  Code Bank too small) get explicit bounds, listed as deviations.

## 7. Building on the Amiga

- **Default: the original files.** The Amiga loads `AMOSPro_AMCAF.Lib` and
  `AMOSPro_TURBO_Plus.Lib` from disk when the configuration names them, as BUILDING.md says
  today. Nothing to build.
- **Optional: the ports linked into AMOSPro**, like the four existing extensions
  (`make EXTRAS=amcaf,turbo`; an image is used instead of the file when its name matches).
  Ours are larger than those (the originals are 63 KB of code together), so this stays off
  by default: AMOSPro is already big for a stock Amiga. Its main use is the comparison above:
  one AMOSPro build with the ports against one with the originals, same programs, same
  goldens.
- **Not planned: standalone `.Lib` files built from C++.** That needs position-independent
  code with AMOS's relocation markers for every call into AMOSPro.Lib; worth a spike only if
  people ask for ports they can load on a real machine.

## 8. Packaging, documentation, licence

- The releases bundle the **ports**, not the original binaries: the distribution configuration
  names the two libraries in slots 8 and 12, the host provides the code, and nothing has to be
  installed. The macOS, Windows and Linux packages pick the patched configuration up from
  `make dist` (the packagers read the tracked `AMOS/` tree today, so the patch step has to be
  shared, as the blue palette's should be).
- `docs/MANUAL.md` gets an Extensions section (slots, how to reassign, what is and is not
  supported, differences between systems); a per-command reference in our own words, with each
  command's status. The authors' manuals are not copied.
- Example programs from the two distributions are bundled only if their licence allows it;
  otherwise the tests find them when the user supplies them.
- **Licence is the open point.** AMCAF's history calls V1.50beta4 its final freeware release;
  Turbo Plus's terms are not stated in what is here. Implementing the commands from their
  documented behaviour is one thing; shipping their command names, signatures and any manual
  text is another, and the authors should be asked (or a licence found) before a release
  carries the ports.

## 9. Status

Checked against the original libraries with `tests/run.py --reference` (the emulator runs them) and
`tests/host_run.py` (the C++): `amcafpt`, `amcafraw`, `amcafmath`, `extmisc`, `turbomisc`; each
records the original's results, the C++ reproduces them. Done: the Protracker player and all of its
commands (AMCAF `Pt ...`), the integer, bit, memory and quick-maths functions of both (see
`data/extensions/*.json`: `"status": "implemented"`). The rest still raises the extension's
"not available" error.

**Where the C++ differs from the originals** (each one a case in which the original crashed the
machine, read memory by accident, or cannot be reproduced):
- a division by zero of the originals' (`Vmod` with a range of 65536, `T Clip` with a multiple of
  65536) is an illegal function call (error 23), not a crash;
- `Qarc` of a vector wider than 16 bits looks past the end of its ratio table; the C++ reads 0 there;
- `String Hunt` with an empty string answers 0 (the original compares 65536 bytes of whatever follows);
- `Bank End` of a bank that is not there: the error "Bank not reserved" (the original reads address -8);
- `Pt Play` takes a bank that is flagged chip memory (the original wants its address below 2MB);
- on the Amiga the C++ player ticks from the vertical blank, never from a CIA timer (the original
  libraries, which the Amiga uses, do use the CIA).

## 10. Decisions needed

1. **Approach**: A (ports, recommended) as the plan says. B (a 68000 interpreter for the
   original binaries) as a possible separate feature later.
2. **Licence and permission** for bundling, and who asks the authors.
3. **The originals**: where the `.Lib` files and manuals are. Without them phase 1 and the
   unit vectors still go ahead; the goldens wait.
4. **Order**: the plan puts the Ghostlight set (phase 2) right after the plumbing. If other
   programs matter more, the commands they use go first.
