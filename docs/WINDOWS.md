# Windows host port plan

Status: the full Windows x86-64 host now cross-compiles and links with GCC
13-win32 and SDL2 2.32.8. The original address/context probe passed on the
user's Windows machine. Full AMOS startup, GUI behavior, and the new backend
probe tests still need Windows runtime validation.

## Build and test the experimental port

```sh
make -j4 host-windows
make windows-probe
make windows-dist
# Override the SDK directory if needed:
make windows-dist SDL2_WINDOWS=/path/to/SDL2/x86_64-w64-mingw32
```

Outputs:

- `build/windows/amos.exe`: full host executable.
- `build/windows/windows-probe.exe`: standalone address, context, dispatch,
  filesystem, and production-wrapper checks (no SDL required).
- `build/windows-portable.zip`: executable, SDL2.dll, AMOS resources, probe,
  and five headless regression tests with golden-file SHA256 checks.

Extract the entire ZIP into a writable folder on Windows 10/11. Run
`amos.exe` for the editor. The executable finds the adjacent AMOS resource
tree without an AMOS_HOME override. Windows defaults to `AMOS_SCALE=1`; set
AMOS_SCALE explicitly to choose another magnification. To test the bundle in PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File .\smoke.ps1
Get-Content .\smoke-results.txt
```

The smoke runner tests control flow, error recovery, files, banks and bank
serialization, enforcing a 30-second per-process timeout and checking every
expected output. Its settings are process-local and restored afterward.
Failure logs are in `smoke/<test>/stdout.txt` and `stderr.txt`.

The port uses Windows fibers for startup and cooperative tasks, with full
stack-range checks at entry, and an executable-image address check before
initializing interpreter pointers. A high-address stack/image fails with a
diagnostic instead of pointer truncation. Native wrappers for virtual memory,
paths, file identities/timestamps, disk space, private RAM cleanup, optional
speech DLL loading, and monotonic time compile with normal Windows packing.
Interpreter jump marks explicitly retain 16-byte alignment.

File data opens are binary. Canonical paths use normalized forward slashes;
Windows drive prefixes on command-line program paths are distinct from Amiga
assign names. The initial bundle links compiler runtimes statically and
imports only SDL2.dll and Windows system libraries. Optional speech uses an
application-local espeak-ng DLL if present; it is not bundled.

Known remaining validation/portability work:

- Run the updated probe and smoke suite on Windows; then test editor input,
  the file-selector task, video, audio, mouse capture, and drag/drop.
- Audit each real glue signature beyond the passing word-call probe.
- Several filesystem operations still use MinGW narrow CRT APIs. Use an
  ASCII installation path initially; full Unicode and long-path support
  remain work, despite wide APIs in the new native wrappers.
- Validate resource discovery and case/root behavior on UNC paths and
  multiple drives. The Unix AMOS_TRAP debug option is not ported.
- Package tests include only initial headless coverage, not the X11 GUI
  automation. No release-support claim until Windows runtime testing passes.

Linux host regression tests and Amiga compilation/linking remain required
checks. The staged design below records the broader plan and remaining work.

## Toolchain and target

Start with a native Windows x86-64 executable cross-compiled on Linux using
GCC/MinGW-w64 (`x86_64-w64-mingw32-g++`, C++20), GNU make, and host Python 3.
Keep the existing Makefile and Amiga compiler settings. LLVM-MinGW is an
alternative if GCC exposes a blocker. Native Windows development can use
MSYS2 UCRT64; the delivered program should not require an MSYS/Cygwin runtime.
Pin the compiler/runtime version for release builds.

## 1. Prove the address and ABI model first

Before porting the full host, build and run a small Windows probe that:

- Places the executable's referenced code and data below 4 GB. Evaluate a
  low PE image base with appropriate relocation/ASLR settings and verify
  actual loaded addresses, rather than relying on linker settings alone.
- Reserves and commits the 512 MB arena below 4 GB using VirtualAlloc2 address
  requirements (or a VirtualQuery/VirtualAlloc search for older systems).
  Also exercises low chip memory and the existing fallback.
- Runs interpreter and cooperative task contexts on explicitly allocated
  low-address stacks. Windows fibers do not provide a caller-supplied stack
  address; do not assume their stacks fit. Evaluate a Windows context backend
  with caller-supplied stacks, such as Boost.Context's fcontext backend, in an
  isolated wrapper. Verify stack bounds, alignment, TEB stack metadata,
  floating-point state, and setjmp/longjmp behavior.
- Tests native dispatch with zero through ten parameters and both 32- and
  64-bit results using the Windows x64 calling convention.
- Checks all Windows/CRT structures used across library boundaries. The
  current global -fpack-struct=2 must not change their ABI. Isolate platform
  code in normally packed translation units with a small fixed-width API;
  keep the Amiga layouts and their assertions intact.

Do not assume that a 32-bit Windows target is an immediate shortcut: the
host currently explicitly passes 64-bit native arguments outside m68k, so
an i686 target would need its own dispatch and layout audit.

## 2. Add an explicit Windows build target

Introduce HOST_CXX, target-platform selection independent of `uname`,
platform-specific source selection, linker options, and `.exe` output.
Use a separate build/windows directory. Keep font/keymap generators running
with Linux Python. Make linker failures return a nonzero status and retain
full diagnostics (the current recipe masks errors and understands GNU ELF
undefined-symbol wording only).

Initial interface (currently stops at the compilation blockers above):

```sh
make host-windows HOST_CXX=x86_64-w64-mingw32-g++
```

## 3. Port platform services

Keep shared interpreter code and existing Linux/macOS behavior; add a small
Windows backend for:

- Arena allocation and task contexts from the probe.
- Library loading through LoadLibraryExW/GetProcAddress.
- Executable discovery through GetModuleFileNameW instead of /proc/self/exe.
- Monotonic timing, waits, environment variables, and debug traps.
- Directory enumeration, creation/modified timestamps, free space, temporary
  directories, recursive cleanup, canonical paths, and error mapping.

Audit hal_files.cpp, devices.cpp, overlay.cpp, drops.cpp, interrupts.cpp,
main.cpp, tasks.cpp, hal_system.cpp, sdl.cpp, and speech.cpp. Preserve binary
file I/O, seek semantics, and Amiga file errors. Use Unicode Windows APIs
behind UTF-8 paths; distinguish Windows drive prefixes from Amiga assigns,
handle separators and drive roots, and test case-insensitive lookup.

## 4. Bring up SDL

Start with an x86-64 SDL2.dll beside amos.exe and explicit application-local
loading. Implement Windows SDL startup requirements and remove the Unix
DISPLAY gate on Windows. Keep console diagnostics during bring-up. Test
window creation, rendering, keyboard, mouse capture, drag/drop, and audio.
Leave speech optional until the core host works.

After the port is stable, add an optional static SDL build if desired,
including Windows system libraries and required third-party notices.

## 5. Validate and distribute

Run the address/ABI probes on real Windows first, then headless interpreter
fixtures and Windows GUI tests. Existing X11/XTest-based automation cannot
be assumed to work on Windows. Use Wine as an additional quick smoke test,
not the only runtime validation. Add a Windows CI runner and keep Linux,
macOS, and Amiga builds in the regression matrix.

Ship a portable ZIP containing amos.exe, SDL2.dll, any required compiler
runtime DLLs (or selectively statically link those runtimes), the AMOS
resource tree, and notices. Discover resources relative to the executable;
do not require AMOS_HOME. Replace Unix shell launchers with Windows launchers.
Verify DLL dependencies and run from a path containing spaces and non-ASCII
characters on a Windows installation without developer tools.

## References

- MinGW-w64 toolchains: https://www.mingw-w64.org/downloads/
- GNU PE linker options: https://sourceware.org/binutils/docs/ld.html
- VirtualAlloc2: https://learn.microsoft.com/en-us/windows/win32/api/memoryapi/nf-memoryapi-virtualalloc2
- Boost.Context: https://github.com/boostorg/context
- SDL Windows guidance: https://wiki.libsdl.org/SDL2/FAQWindows
