# Windows platform probe

Build on Linux with the installed MinGW-w64 cross compiler:

```sh
make windows-probe
```

Copy `build/windows/windows-probe.exe` to an x86-64 Windows machine. This is
a console executable; it needs neither SDL2 nor the AMOS resource tree.
Its compiler runtimes are statically linked. Run from PowerShell:

```powershell
.\windows-probe.exe > windows-probe.txt 2>&1
$LASTEXITCODE
Get-Content windows-probe.txt
```

It returns zero only if every gate passes. Keep the output and exit code.
It tests the real allocator used by the Windows host build:

- Loaded image/global addresses and a 512 MB arena fit below 4 GB.
- Committed memory is zero-filled and writable at both ends.
- Exact-address allocations do not overwrite occupied mappings; invalid
  sizes and ranges crossing 4 GB are rejected.
- An 8 MB candidate interpreter stack can be allocated below 4 GB.
- A chip mapping at 1 MB is attempted; the arena fallback is allowed.
- Calls through erased function addresses with zero to ten word parameters
  preserve a 64-bit result under the Windows x64 ABI.
- Native Windows fibers yield repeatedly on the main thread, retain stack
  locals, and support same-stack setjmp/longjmp recovery.
- Fiber stack locals and jump buffers fit below 4 GB.

The last check may FAIL even when fiber switching works. CreateFiberEx
chooses its own stack address; allocating a separate low-address stack does
not make the system fiber use it. A high fiber stack is a concrete reason
to implement a caller-supplied context backend or guest-address translation,
not to truncate its pointer or disable the check. The main thread's normal
stack address is informational because AMOS will need its own context.

The word-call checks do not establish the safety of mismatched function
signatures, narrow/signed parameter adaptation, packed structures, or every
actual AMOS glue entry. Those still need dedicated validation.

This probe intentionally uses normal Windows ABI packing. windows_memory.cpp
also has a normally packed compilation rule in the AMOS host build; Windows
headers must not be exposed to the global -fpack-struct=2 option.

No full Windows AMOS executable is produced until the remaining filesystem,
context, startup and optional-library backends have been ported. Wine can
provide additional smoke testing, but real Windows results are required.

The probe now also exercises the production Windows wrappers: full image
bounds, checked fiber stacks, repeated yields/recovery, monotonic timing,
canonical paths, file identity, disk space, binary control characters,
directories with spaces, and private RAM cleanup. Re-run it after updating;
the original passing report predates these extra checks.

`make windows-dist` also packages five full-interpreter smoke tests and a
PowerShell runner. See docs/WINDOWS.md. These require SDL2.dll even with
AMOS_DISPLAY=none because the current Windows executable links SDL at load
time.
