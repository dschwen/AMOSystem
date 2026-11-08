# AMOS Professional in C++

AMOS Professional 2.00, François Lionet's BASIC for the Amiga, converted from
68000 assembler to C++. The same C++ builds two programs:

- **AMOSPro for the Amiga**: the original program (interpreter, editor,
  monitor, AMOS.library, the extensions), compiled for the 68000 with a cross
  compiler. It loads and runs the same `.AMOS` programs as the original.
- **`amos` for Linux and macOS (x86-64)**: AMOS as a program of this computer, with an SDL2
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

### The macOS build

The macOS host build targets **x86-64** and needs Apple's Command Line
Tools (Clang with C++20 and make) and **python3**. Install the Command Line
Tools with `xcode-select --install` if needed. The NDK headers are downloaded
automatically, as for the Linux host build; no Amiga cross compiler is needed.

For the window and sound, download
[SDL2-2.30.12.dmg](https://www.libsdl.org/release/SDL2-2.30.12.dmg), open it,
and copy **SDL2.framework** into **/Library/Frameworks**. The resulting
library path is `/Library/Frameworks/SDL2.framework/SDL2`. AMOS loads it at
runtime; no SDL headers or Homebrew packages are required.

From the repository directory:

```sh
make host
AMOS_HOME="$PWD/AMOS" ./build/host/amos
```

`AMOS_HOME` points to the checked-in AMOS tree and avoids depending on the
current installation layout. `make host-release` builds the optimised
executable in `build/host-release/amos`. Use `make host` or `host-release`
on macOS; `host-sdl` currently uses Linux-specific library link flags.
Native Apple Silicon builds are not yet supported. The
[type-safe guest address translation plan](docs/GUEST-ADDRESSING.md) describes
the required runtime refactor. The experimental Docker/OSXCross
arm64 cross-build and remaining runtime gates are described in [docs/MACOS-CROSS.md](docs/MACOS-CROSS.md).

To distribute an Intel app with SDL2 and the AMOS icon included, run
`make package-dmg VERSION=0.1.3` on the Mac. The DMG and manifest appear under
`build/packages`. See [macOS packaging](packaging/macos/README.md) for signing,
user-state handling and the remaining runtime/notarization checks.

### The Windows build (experimental)

Cross-compile on Linux with **x86_64-w64-mingw32-g++** (C++20), make,
python3, and the **x86-64 MinGW SDL2 SDK**:

```sh
make host-windows SDL2_WINDOWS=/path/to/SDL2/x86_64-w64-mingw32
make windows-dist SDL2_WINDOWS=/path/to/SDL2/x86_64-w64-mingw32
make package-msi VERSION=0.1.0 SDL2_WINDOWS=/path/to/SDL2/x86_64-w64-mingw32
```

The default SDK path is `~/Downloads/SDL2-2.32.8/x86_64-w64-mingw32`.
The executable is `build/windows/amos.exe`; the portable bundle is
`build/windows-portable.zip`. Extract the entire bundle into a writable
folder on Windows 10/11 and run `amos.exe`, keeping `SDL2.dll` and the
`AMOS` resource directory beside it. The bundle includes a `smoke.ps1`
PowerShell runner for five headless regression tests.

The unsigned MSI in `build/packages` installs under Program Files and prepares
per-user working resources/settings in Local AppData. `make msi-tools` can
extract Debian/Ubuntu installer tools locally without sudo. See
[the MSI README](packaging/windows/README.md) for build/inspection and Windows
lifecycle test commands. Installation/upgrade/signing qualification is pending.

`amos --open-document path` opens one document in the editor without running
it; normal CLI program execution is unchanged. The MSI advertises this handler
for `.amos`; choosing the default remains a Windows user action.

The address/context probe has passed on Windows; full interpreter and GUI
runtime validation is still pending. Start with an ASCII installation path.
See [docs/WINDOWS.md](docs/WINDOWS.md) for the implementation status, test
commands, and remaining work.

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
make package-linux VERSION=0.1.0     # build/packages: tar.xz and .deb (ROM-free), see docs/RELEASES.md
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
empty at every start. **Disk images**: a `.adf` file (`--df0 disk.adf`, `path =` in
`amos.conf`, dropped on the window, or in the library) is a floppy, write-protected, with the
disk's own label; `tools/adf.py` lists, extracts and packs them. **In the window**: drop a
folder on it to mount it as a hard disk (a `.adf` goes into `DF0:`), and press **F12** for the Devices panel: the drives and their
disks, put a folder in or take it out, write-protect, mount a hard disk, save
the setup to `amos.conf`. When a program asks for a disk that is not there
("Please insert volume Disk2"), AMOS's requester stays and the window says
what to do; put the disk in and the program goes on by itself. The window is magnified twice (`AMOS_SCALE`).
**Full screen**: **Alt+Enter** (Option+Return on a Mac), or **Full screen** at the top right of the
F12 panel, switches between the window and the whole screen (the picture keeps its shape, with black
bars); `--fullscreen` starts that way, and the panel's Save keeps the choice (`[window] fullscreen = yes`
in `amos.conf`). On a Mac the system's own ways work too: the green button, Control+Command+F, Globe+F.
F11 is not used for it: it is the Amiga's Help key here.
**Joysticks**: game controllers (anything SDL knows as one: Xbox, PlayStation, Switch pads and most
others) are the Amiga's joysticks: the d-pad or the left stick, and A, B, X or Y for fire. By default
the first one is in port 2 (`Joy(1)`, where games expect it) and a second one in port 1 (`Joy(0)`, the
mouse's port). In the F12 panel, **Port 1** / **Port 2** (keys `1`, `2`) choose for each: auto, none,
the keyboard (the cursor keys, either Ctrl for fire; the keys still reach AMOS too), or a controller;
Save keeps it (`[joystick] port1 = ...`, `port2 = ...`: `auto`, `none`, `keyboard`, `pad1`...), and
`--port2 keyboard` sets it for one start.
docs/DEVICES.md has the plan for the rest (writing to images, Windows and macOS places); docs/BUILDING.md lists the other environment variables and what the host
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

## Installing a release, and code signing

The releases (GitHub, from the `v*` tags) are built by the workflows in
`.github/workflows` and nowhere else.

- **Windows**: the MSI and the `amos.exe` / `amos-player.exe` in it are signed.
  Free code signing provided by [SignPath.io](https://about.signpath.io),
  certificate by [SignPath Foundation](https://signpath.org). SmartScreen can
  still warn about a new release until it has been downloaded often enough
  (More info, Run anyway). `SDL2.dll` is libsdl.org's binary, unchanged.
- **macOS**: the app is not notarized by Apple (that needs a paid developer
  account), so macOS blocks its first launch. Once: System Settings, Privacy &
  Security, "AMOS Professional was blocked", Open Anyway (macOS 14 and older:
  Control-click the app, Open). The DMG's Read Me says the same.
- **Linux**: the `.tar.xz` and `.deb` come with their SHA-256 list.

Code signing policy (Windows): the committers and reviewers, and the approver
of each signing, are the repository's maintainers
([dschwen](https://github.com/dschwen)). Only what the release workflow built
from this repository's tagged source is signed. Privacy: AMOS makes no network
connections and sends no information anywhere; it reads and writes only the
files and folders you give it and its own settings.

## Licence

MIT, as the original sources (`LICENSE`). AMOS Professional is © François
Lionet and Europress Software; the copyright notice stays in the sources and
in the program's start-up screen.

Host system copy/paste is described in [docs/CLIPBOARD.md](docs/CLIPBOARD.md),
covering system copy/paste in the source editor and direct-mode command line.
