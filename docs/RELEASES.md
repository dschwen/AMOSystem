# Plan: binary releases for Linux, macOS and Windows

Status: a plan, nothing of it is built yet. Today the host (`amos`,
`amos-player`) builds and runs on x86-64 Linux only.

## 1. What a release is

One archive for each platform, made by a tag on the repository (GitHub
Releases of `dschwen/AMOSystem`):

| Archive | Contents |
|---|---|
| `amos-VERSION-linux-x86_64.tar.gz` (and `aarch64` if a runner is there) | `bin/amos`, `bin/amos-player` (a link), `share/amos/AMOS` (the AMOS tree), `LICENSE`, `NOTICE` |
| `amos-VERSION-macos-universal.zip` | `AMOS Professional.app` (x86-64 and arm64 in one binary, the tree in `Contents/Resources`), `amos-player` inside |
| `amos-VERSION-windows-x64.zip` | `amos.exe`, `amos-player.exe`, `SDL2.dll`, `AMOS\` (the tree), licences; an installer (Inno Setup) later |
| `AMOS-amiga-VERSION.zip` (optional, from the Linux job) | `build/dist/AMOS`: AMOSPro for the Amiga with the tree |

The tree is the host's own: `AMOSPro_Interpreter_Config`, `APSystem`
(only the data files the host reads: the default and editor resources, the
editor's configuration and macros), `Accessories`, and the examples. No
Kickstart-derived data goes into a release: the fonts are AROS's topaz 8
(`data/fonts/topaz8`) and the keymap is the plain one.

What a release lacks because it has no Kickstart, and should say so in its
notes: topaz 9 (drawn with topaz 8's characters), the Alt characters and
dead keys of the keymap, and the monitor (`AMOSPro_Monitor_Resource.Abk` is
not in the sources). Both of the first two can be fixed later with data of
our own (a hand-written usa keymap table: step 6 of section 3).

Not bundled: espeak-ng (GPL; `Say` stays optional, the user installs it),
and no SDL2 *headers* are needed anywhere, only the library.

## 2. What stands in the way

The host is written for the one machine it runs on. Everything below is in
`src/host` (4900 lines) and `src/runtime`; the shared code (95% of the
repository) does not change.

| Area | Today (Linux) | Needed |
|---|---|---|
| **AMOS's 32-bit addresses** | AMOS keeps pointers in 32 bits, so everything it can see is below 4GB: the program is linked non-PIE at 0x400000, the memory is `mmap(MAP_32BIT)` in a 512MB arena (chip memory fixed at 1MB), the stack is allocated in the arena | see 2.1: the one real risk |
| **Coroutines** (the file selector's task, `host/tasks.cpp`) and the **stack** AMOS runs on (`pthread_attr_setstack`) | `ucontext`, pthreads | Windows has neither: Fibers cannot take a stack of our own. macOS has `ucontext` (deprecated) |
| **Files** (`hal_files.cpp`) | POSIX: `opendir`, `realpath`, `statx` (Linux only, for the creation time), `/proc/self/exe`, XDG directories | an `os` layer: macOS `stat`'s birth time and `_NSGetExecutablePath`, `~/Library/Application Support`; Windows `FindFirstFile`, `GetModuleFileName`, `%APPDATA%`, drive letters and case-insensitive names |
| **Libraries** (`sdl.cpp`, `speech.cpp`) | `dlopen("libSDL2-2.0.so.0")` | the library's name for each system, `LoadLibrary`/`GetProcAddress` on Windows; or link SDL2 (`make host-sdl` already does) and ship it |
| **Time and sleep** | `clock_gettime`, `usleep` | `std::chrono`, `std::this_thread` |
| **Maths** (`host/math.cpp`) | glibc's `sin`, `exp`... (the results of `Sin`, `Log`... come from the host's libm) | the last digit differs between glibc, Apple's libm and the Windows CRT: the tests would differ by platform. See 2.2 |
| **Build** | `Makefile` with `g++ -m64 -no-pie`, the NDK headers from the Amiga cross toolchain (`/opt/amiga`) | a build that needs neither the cross compiler nor a Linux shell: see 2.3 |
| **Tests** | Xvfb, scrot and XTest drive the window tests; the goldens are Amiga/Kickstart results | text tests (82) everywhere; window tests only on Linux; goldens made without a ROM: see 4 |

### 2.1 The address space

The cheapest way (A) keeps pointers as they are and puts everything below
4GB:

- **Linux**: done.
- **Windows x64**: link with a base below 4GB (`-Wl,--image-base=0x400000
  -Wl,--disable-dynamicbase`, MinGW-w64) and take the memory and the stack
  with `VirtualAlloc` at fixed addresses (it has no `MAP_32BIT`, but a hint
  address works if the range is free). Memory is reserved at once and
  committed as the allocator grows.
- **macOS x86-64**: `-Wl,-pagezero_size,0x1000 -Wl,-image_base,...` and
  `mmap` at fixed low addresses.
- **macOS arm64**: **unknown**. Apple Silicon wants a 4GB `__PAGEZERO` in
  64-bit programs, which takes the very range we need. A one-day spike
  decides it: a small program with the pagezero reduced, mapping memory at
  0x10000000, run natively. If it works, A is the plan. If not, either ship
  the x86-64 build to Apple Silicon (it runs under Rosetta 2, if the spike
  shows that mapping works there) or do B.

B removes the whole problem: AMOS's 32-bit addresses become offsets in the
arena (`ptr<T>(a)` is `base + a`, `address(p)` is `p - base`). Everything AMOS
can see then has to be in the arena: the host's structures (already
allocated there), the glue tables' data (copied to it once at start), and the
stack. `address()` would check that its argument is in the arena, so a
missed object fails at once instead of being cut to 32 bits. It is the
cleaner design, it makes PIE and hardened binaries possible, and it is a
medium-sized change (`ptr`/`address` are in one header, the work is in what
the check finds). Recommended only if the spike fails.

### 2.2 The maths

Replace the host's use of libm in `host/math.cpp` by our own implementations
(port fdlibm, the same code on every platform), so the results are the
same everywhere, and the maths golden does not need a variant per system.
Until then, keep the platform's variants for the few tests concerned
(`maths`, `floats`, `floats2`, `expr2`).

### 2.3 The build

- A target `make dist-host` that makes the tree without the Amiga binary
  (today `make dist` needs the cross compiler to build `AMOSPro`; the host
  does not read it).
- The NDK headers: the host build includes 46 of them (428KB) from the
  cross toolchain. The Mac and Windows jobs cannot have the toolchain
  cheaply. Either the Linux job builds (or caches) the toolchain and hands
  the `ndk-include` directory to the others as an artifact, or the 46
  headers are copied into the repository. The second is simpler and faster,
  **if their licence allows it** (to be checked: the NDK's terms depend on
  its version). Decision 1 in section 8.
- Compilers: MinGW-w64 g++ (UCRT) on Windows, Apple clang on macOS, g++ on
  Linux. The code uses `-fpack-struct=2` and GCC's `__builtin_longjmp`; both
  clang and MinGW g++ have them. A first step is to see that clang++ compiles
  the host (`make host CXX=clang++`) with no changes or only small ones.
- Move from the hand-written `Makefile` rules for the host to CMake (one
  `CMakeLists.txt` for the host: the Amiga build stays in the `Makefile`),
  or keep GNU make and use it on all three (MSYS2 has it). CMake is the
  more usual in CI and gives the universal macOS binary and the Windows
  resources (icon, version) cleanly. Recommended.

## 3. Steps

Each step ends with the Linux suite passing as it does now (Amiga 134/134,
host 135/135).

1. **Linux release** (small). `make dist-host`; a release build that runs on
   older systems (build in the oldest supported Ubuntu, link libstdc++ and
   libgcc statically); `make release-linux` makes the tarball and checks it
   (unpack in an empty directory, `amos-player` a program, exit status).
   `NOTICE` with the AROS font's attribution and the SDL2 notice.
2. **The `os` layer** (medium, no change in behaviour on Linux). One small
   interface in `src/host/os.h`, with `os_posix.cpp` (Linux and macOS) and
   `os_win32.cpp` (later): the arena (reserve, commit, fixed low addresses),
   the coroutine (with the stack AMOS runs on), the executable's path, the
   user's data directory, the directory listing with creation times, sleep
   and the clock, loading a library. `hal_files.cpp`, `tasks.cpp`, `main.cpp`,
   `hal_system.cpp`, `sdl.cpp` and `speech.cpp` stop calling the system
   directly. One piece needs a decision: the coroutine. `ucontext` is
   enough for Linux and macOS; for Windows either a context switch of our own
   (a few instructions of inline assembler for each of x86-64 and arm64,
   which sits badly with "no assembler" but is 30 lines) or threads that pass
   control to each other (portable C++, but pthreads with a stack of our own
   have to exist on Windows: to be tried first).
3. **The spike for macOS arm64** (small, do it at the start, in parallel with
   1): decides A or B in 2.1.
4. **macOS** (medium). Build for x86-64 and arm64 with clang, join them
   with `lipo`, make the `.app` (Info.plist, icon from the AMOS logo, the
   tree in `Resources`), `SDL2.framework` or `libSDL2.dylib` inside
   (`@rpath`), `make host-sdl` style link or `dlopen` of the bundled copy.
   Sign (see 5). Text tests on arm64 and on x86-64 (Rosetta).
5. **Windows** (medium to large; the most code). `os_win32.cpp`, MinGW-w64
   build (static libstdc++), `SDL2.dll` beside the executable, console and
   window subsystem (`amos.exe` a window program that also prints to the
   console when started from one), the file layer's `START:`, devices
   directory and drive letters (a `devices` folder in `%APPDATA%\AMOS`, as
   the other systems), long and case-insensitive names. Text tests, then the
   window tests by hand.
6. **Smaller improvements that make the releases better**: our own usa keymap
   table (Alt characters, dead keys) instead of the Kickstart's; a topaz 9
   of our own (drawn, public domain); our own maths (2.2).
7. **CI and release automation** (small to medium, can start with step 1):
   one GitHub Actions workflow with a job for each platform:
   build, text tests, package, and on a tag `v*` upload to a GitHub Release
   with checksums and generated notes. The Linux job also builds the Amiga
   binary (it only has to compile; the Amiga suite needs a Kickstart, which
   cannot be in CI, so it stays a local step: `docs/BUILDING.md`).

## 4. Testing a release without a Kickstart

The goldens of `tests/golden` were made with the original binaries on an
emulator with the Kickstart's fonts. A build with AROS's topaz 8 draws
text differently, so every picture with text would fail. For CI:

- Make a second set of goldens, **`tests/golden-rom-free`**, from the Linux
  host built without a ROM (the Amiga's are already proof that the Linux
  build is right with the ROM; this set only tells us that the other
  platforms produce the same as Linux). A platform that differs byte for
  byte is a bug in the port.
- The text tests (82) run on all platforms; the editor and input tests (53)
  run in a window on Linux only (XTest has no Mac or Windows counterpart
  worth the trouble); on the other systems a short smoke test: start the
  window, run a program that draws, save a screenshot with `AMOS_SHOT`,
  compare with Linux's.
- Packaging tests: unpack the archive in an empty directory, run
  `amos-player` on a program from a different directory, check the output
  file and the exit status; run `amos` with `AMOS_DISPLAY=none`.

## 5. Signing and distribution

- **macOS**: an unsigned application is stopped by Gatekeeper (right click >
  Open, or `xattr -d com.apple.quarantine`). Signing and notarising needs
  an Apple Developer ID (99 USD a year) and the certificate as a repository
  secret. Without it, say how to open it in the notes.
- **Windows**: an unsigned program triggers SmartScreen's warning for the
  first downloads. Signing costs a certificate (or Azure Trusted Signing).
  Without it, say so in the notes.
- **Linux**: no signing; a checksum file, and later an AppImage (a single
  file that runs anywhere) if wanted.
- Versions: `v0.1.0`... from the tag, shown by `amos --version` and in the
  window's title.

## 6. Licences of what is in an archive

MIT for AMOS and this conversion (`LICENSE`); the AROS Public License for
topaz 8 (`LICENSE.AROS`, in the archive with its notice); zlib for SDL2 (its
text in the archive). The AMOS example programs and the resources are
François Lionet's, under the repository's MIT licence. No Commodore,
Cloanto or other ROM data, so no tool of ours reads the user's Kickstart in
a release build (the extractors stay for builds from source).

## 7. Order and size

| Step | Size (days of work, a guess) | Needs |
|---|---|---|
| 1 Linux release | 1-2 | decision 1 (the NDK headers) |
| 3 macOS arm64 spike | 1 | a Mac to run it, or a runner |
| 2 the `os` layer | 3-4 | |
| 7 CI | 2 | step 1 |
| 4 macOS | 3-5 | 2, 3 (and B if 3 fails: +4-6) |
| 5 Windows | 4-6 | 2 |
| 6 own keymap, topaz 9, maths | 3-5 | any time after 1 |

A first public Linux release after step 1 and 7 (Linux job); macOS and
Windows follow as steps 4 and 5 finish.

## 8. Decisions needed

1. **NDK headers**: copy the 46 into the repository (fast CI; check the
   licence first), or build and cache the Amiga toolchain in CI.
2. **Windows toolchain**: MinGW-w64 (recommended: the same g++, the same flags
   as Linux) or MSVC/clang-cl (needs more changes: `-fpack-struct`, GNU
   builtins).
3. **macOS signing**: sign and notarise (Developer ID) or ship unsigned with
   instructions.
4. **If the arm64 spike fails**: ship x86-64 for Apple Silicon under Rosetta
   2, or do B (a translated address space).
5. **Coroutines on Windows**: a small inline-assembler context switch, or the
   threads way (to be tried first).
6. **Build system for the host**: CMake (recommended) or stay with GNU make.
7. **The Amiga archive** (`AMOS-amiga`) as a fourth download: yes or no.
8. **Where**: GitHub Releases of `dschwen/AMOSystem`, and the first version
   number.
