# AMOS Professional in C++

AMOS Professional 2.00, François Lionet's BASIC for the Amiga, converted from
68000 assembler to C++. The same C++ builds two programs:

- **AMOSPro for the Amiga**: the original program (interpreter, editor,
  monitor, AMOS.library, the extensions), compiled for the 68000 with a cross
  compiler. It loads and runs the same `.AMOS` programs as the original.
- **`amos` for Linux**: AMOS as a program of this computer, with an SDL2
  window, sound, and the disks as folders. `amos-player` runs a program on its
  own (no editor), and `make game` bundles a program with it.

Both are checked against results recorded with the original binaries: 134
test programs in an emulator for the Amiga build, the same programs (and the
host's own) for the Linux build.

The original assembler sources are in the history of the repository. The
original README (François Lionet's introduction, marc365's history of the
sources) is [docs/HISTORY.md](docs/HISTORY.md); the details of the
conversion (how the C++ is organised, what the host emulates, what differs
from the original, the tests) are [docs/BUILDING.md](docs/BUILDING.md).

## The repository

| Folder | What is there |
|---|---|
| `src/` | the C++ (the Makefile and docs/BUILDING.md say what each folder holds); `src/amiga` and `src/hal/*_amiga.cpp` are the Amiga's own, `src/host` the Linux host's |
| `AMOS/` | the AMOS tree: the original `AMOSPro` and `APSystem` (the reference binaries of the tests, the configurations, the resources), `Accessories`, `Compiler` (the compiler's shells; its folder is what makes the `AMOSPro_Compiler:` assign) |
| `examples/` | the example programs of AMOS Pro: `Examples`, `Tutorial`, `Productivity1`, `Productivity2` (`make dist` puts them into the AMOS tree) |
| `link/` | the Amiga linker script |
| `tools/` | the build's and the tests' scripts (tokenizer, ROM extractors, NDK fetch and its LHA extractor, hunk flags...) |
| `tests/` | the test programs, their golden results, the runners |
| `amiga/` | the Amiga test environment: the FS-UAE configuration (the emulator's own files are not in the repository, see below) |
| `data/` | data the builds use: `fonts/topaz8`, AROS's topaz 8 as raw data (its README, the AROS Public License) |
| `docs/` | BUILDING.md, HISTORY.md, RELEASES.md (the plan for binary releases) |
| `build/` | everything built (not in the repository) |

## Requirements

### The Amiga build: the cross compiler

- The Amiga cross toolchain **m68k-amigaos-gcc** with C++20, from
  https://github.com/AmigaPorts/m68k-amigaos-gcc (the build was made with
  `m68k-amigaos-gcc (AmigaDev) 16.2`). It must provide:
  - `m68k-amigaos-gcc` / `m68k-amigaos-g++` and binutils (`ld` with the
    `amigahunk` output);
  - **libnix** (`m68k-amigaos/libnix/lib`, with `libnix13`) and libgcc: AMOS
    links no C library startup, only the arithmetic helpers;
  - the **NDK headers** (`m68k-amigaos/ndk-include`): the Linux build uses
    them too.

  It is looked for in `/opt/amiga`; pass `AMIGA_PREFIX=/elsewhere` to `make`
  otherwise.
- GNU make, python3.

### The Linux build

- Linux on x86-64, **g++ with C++20** (GCC 13 or later), GNU make, python3.
- The **NDK's C headers** (46 of them are included): the C++ is the Amiga's,
  the system structures are the NDK's. The cross toolchain's copy
  (`AMIGA_PREFIX/m68k-amigaos/ndk-include`) is used if there is one; without
  the toolchain the build downloads NDK 3.2 from Aminet by itself (`make
  ndk`, `tools/fetch_ndk.py`: the same archive and sha256 the toolchain
  uses; python3 only, no LHA tool). They are Hyperion's, under no stated
  licence, so they are not in this repository.
- To run it:
  - **SDL2** (`libSDL2-2.0.so.0`, Debian/Ubuntu `libsdl2-2.0-0`) for the
    window, the keyboard, the mouse and the sound. Only the library is needed,
    no headers: `amos` loads it when it starts (`make host-sdl` links it
    instead). Without it AMOS runs without a window.
  - Optional: **espeak-ng** (`libespeak-ng.so.1`, `libespeak-ng1` and
    `espeak-ng-data`) for `Say`; without it the narrator is silent.
- Optional at build time: an **Amiga Kickstart 3.1 ROM** (and its `rom.key`
  if it is a Cloanto-encrypted one). The topaz fonts and the usa keymap (Alt
  characters, dead keys) are taken from it into `build/`; without one the
  build uses AROS's topaz 8 (`data/fonts/topaz8`, in the repository) and a
  plain keymap. The ROM is
  looked for in `amiga/kickstarts` (`KICKSTART=`, `ROMKEY=` name others).

### The tests

- For both: python3 with **Pillow**, **Xvfb**, **scrot**, libX11 and libXtst
  (`tests/xinput.py` sends keys and mouse moves through XTest).
- For the Amiga build: **FS-UAE** (3.x) and the Amiga test environment below.

## Building

```
make                  # build/AMOSPro: AMOSPro for the Amiga
make dist             # build/dist/AMOS: the AMOS tree with it and the examples
make host             # build/host/amos: AMOS for Linux (debug build)
make host-release     # build/host-release/amos: optimised
make host-sdl         # build/host-sdl/amos: linked to SDL2
make install-host [PREFIX=~/.local]    # bin/amos, bin/amos-player, share/amos/AMOS
make uninstall-host [PREFIX=~/.local]
make game GAME=prog.AMOS [DATA=folder] [NAME=name]  # build/game/NAME
make clean
```

`make dist` is what an Amiga needs: copy `build/dist/AMOS` to the Amiga (or
`make install-hd` to put the new AMOSPro on the emulator's hard disk).

## Running AMOS on Linux

```
amos                      # the editor
amos prog.AMOS            # runs the program, then the editor
amos-player prog.AMOS     # runs the program only (AMOSPro's -r): no editor, no direct mode
```

`amos` finds its AMOS tree from where it is installed (`AMOS_HOME` names
another). The folder it was started in is the volume `START:`, and the
folder of the program the volume of its own name. **The disks** are folders:
`DF0:`...`DF3:` (floppies) and `DH0:`...`DH9:` (hard disks) hold folders as
disks, labelled with the folder's name or as you say. They are set up in
`amos.conf` (`~/.config/amos/amos.conf`, or a program's `Name.amos.conf`
beside it) or on the command line:

```
amos --df0 "~/disks/Game 1=Game Disk 1" --df1 ~/disks/Game2,ro --dh1 ~/amiga/Work prog.AMOS
```

```
[df0]
path = ~/disks/Game1
label = Game Disk 1
[df1]
path = ~/disks/Game2
readonly = yes
[dh1]
path = ~/amiga/Work
[library]
path = ~/disks          # disks found by their label when a program asks for them
[assign]
FONTS = ~/amiga/Fonts
```

The folders of the devices directory (`AMOS_DEVICES`, else `./devices`, else
`~/.local/share/amos/devices`, made with `Work`) are disks too, and `RAM:` is
empty at every start. **In the window**: drop a folder on it to mount it as a
hard disk, and press **F12** for the Devices panel: the drives and their
disks, put a folder in or take it out, write-protect, mount a hard disk, save
the setup to `amos.conf`. When a program asks for a disk that is not there
("Please insert volume Disk2"), AMOS's requester stays and the window says
what to do; put the disk in and the program goes on by itself. The window is magnified twice (`AMOS_SCALE`).
docs/DEVICES.md has the plan for the rest (disk images, a window to set them
up); docs/BUILDING.md lists the other environment variables and what the host
does not do as the Amiga does.

`make game GAME=prog.AMOS DATA=folder NAME=mygame` makes `build/game/mygame`,
a folder that runs on its own wherever it is put: the launcher `mygame`,
`bin/amos-player`, the system files (2MB), and the program with the files of
`DATA` (its current folder: what it loads by name).

## Testing

```
make test                              # the Amiga build in FS-UAE (tests/run.py)
python3 tests/run.py NAME...           # some of them
python3 tests/host_run.py              # the Linux build (build/host/amos)
python3 tests/host_run.py --build build/host-sdl
make test-reference                    # the original binaries in AMOS/, against the same goldens
python3 tests/run.py --reference --update-golden NAME...   # record goldens with the original binaries
```

The test programs are `tests/amos/*.asc` (AMOS listings in plain text), with
`.input` scripts for the ones that need the keyboard and the mouse. Their
results (text, IFF pictures, screenshots) are compared with `tests/golden/`,
recorded with the original binaries. The Linux build's few known differences
are in `tests/host_only/variants` (README there). A full run takes about 12
minutes for each build. Only one `tests/run.py` can run at a time.

## The Amiga test environment

`tests/run.py` boots one FS-UAE for each test, on an Amiga 1200 with three
drives: the test's own boot drive (made for it), the AMOS tree (`build/dist`,
read only) and a Workbench hard disk for `C:`, `Libs:` and the rest. The
emulator's files are in `amiga/`, and only the configuration is in the
repository:

```
amiga/
  AMOS.fs-uae          the FS-UAE configuration (in the repository)
  kickstarts/          the Kickstart (not in the repository)
    amiga-os-310-a1200.rom
    rom.key
  harddisk/            the Workbench hard disk (not in the repository)
  backup/              the original binaries make install-hd replaced (not in the repository)
```

**`amiga/kickstarts`**: the Kickstart 3.1 ROM of the A1200 (40.68),
`amiga-os-310-a1200.rom`, for instance from Amiga Forever. A Cloanto-encrypted
ROM (`AMIROMTYPE1` at its start) needs its `rom.key` beside it; the tests take
it from the hard disk's `Devs/Kickstarts` and give FS-UAE a copy. The Linux
build takes its fonts and keymap from the same ROM.

**`amiga/harddisk`**: an FS-UAE directory drive (a folder, the AmigaDOS
metadata of each file in a `.uaem` file beside it) with Workbench 3.1
installed. The tests use from it:

- `C/`: `Assign`, `SetPatch`, `MakeDir`, `Copy`, `Wait` (the tests' startup
  sequence, `Exec "Workbench:C/Wait 0"`);
- `L/`, `Libs/` (the libraries AMOS opens that are not in the Kickstart:
  `diskfont`, `asl`, `mathtrans`, `mathieeedoubbas`, `mathieeedoubtrans`,
  `mathieeesingtrans`, `translator`, `rexxsyslib`), `Devs/` (the keymaps,
  `narrator.device`, `serial.device`, `parallel.device`, `printer.device`,
  and `Kickstarts/rom.key`), `Fonts/`, `Locale/`, `Prefs/Env-Archive/`: the
  assigns the startup sequence makes (`LIBS:`, `DEVS:`, `FONTS:`, `LOCALE:`,
  `ENVARC:`);
- `AMOS_Pro/`: an installed AMOS Pro. The tests take
  `APSystem/AMOSPro_Monitor_Resource.Abk` from it (the monitor's resource,
  which is not in the sources); `make install-hd` puts the AMOSPro built here
  into it (its originals saved in `amiga/backup` first).

Its own `S/Startup-Sequence` is not used by the tests (they boot from their
own drive). The volume label is `Workbench`.

For a look at the Amiga build by hand:

```
make dist
fs-uae amiga/AMOS.fs-uae
```

This boots the hard disk's Workbench with the AMOS tree as `AMOSPRO:`; run
`AMOSPRO:AMOSPro` from a shell. (The hard disk used here assigns some
folders from a volume `Programs:` in its `S/user-startup`, which this
configuration does not have: Cancel the requester.)

## Licence

MIT, as the original sources (`LICENSE`). AMOS Professional is © François
Lionet and Europress Software; the copyright notice stays in the sources and
in the program's start-up screen.
