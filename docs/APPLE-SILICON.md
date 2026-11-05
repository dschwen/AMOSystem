# Apple Silicon host port plan

Status: native arm64 execution remains unsupported. Linux/Docker cross-compilation
now produces a guarded arm64 executable and native probes. Hardware probe
execution confirms that the current direct-pointer address model fails.
The existing macOS port targets x86-64. This plan covers native arm64 execution,
not running the Intel executable through Rosetta.

The Linux/Docker build route is now planned in
[MACOS-CROSS.md](MACOS-CROSS.md), including OSXCross setup, compiler blockers,
address-model gates and Mac-side runtime/signing validation.

## Toolchain and target

Use Apple Clang with C++20, the macOS SDK and linker from Xcode/Command Line
Tools, GNU-compatible make, and host Python 3. Start with a native build on
an Apple Silicon Mac. Intel Macs with the appropriate SDK can compile an
arm64 executable, but execution tests require Apple Silicon. Linux-hosted
Mach-O cross-compilation with Docker/OSXCross is the proposed repeatable build
route; native Apple Silicon remains necessary for runtime validation. See the
cross-compilation plan for the container and implementation sequence.

Use explicit `-arch arm64` compilation and linking with a separate
build/host-arm64 directory. Target ordinary arm64, not arm64e. Choose and
document the minimum supported macOS/SDK versions after the probes below.

The hardware probes have now failed the direct-pointer address gate while
passing layout, native C++, nested jumps and basic context switching. The next
implementation is planned in [GUEST-ADDRESSING.md](GUEST-ADDRESSING.md), with
compiler-enforced guest/native pointer separation.

## 1. Resolve the address model before porting the full program

AMOS currently treats many 32-bit addresses as real host pointers, including
pointers to generated tables, native entry points, interpreter stack objects,
and setjmp marks. Its memory arena and stacks are deliberately below 4 GB.
This is a fundamental constraint, not just an allocator implementation detail.

Build small standalone probes on the actual target OS/SDK:

- Inspect the arm64 Mach-O PAGEZERO/text/data layout and actual loaded
  addresses. Test linker options rather than copying Intel's -no_pie and
  reduced -pagezero_size settings. Ordinary arm64 macOS configurations have
  historically enforced a 4 GB PAGEZERO reservation. A recent Wine developer
  report describes a newer x86_64_layout_emulation linker/OS facility; confirm
  availability, intended use, deployment restrictions, and behavior directly
  with the installed Apple tools before considering it.
- Test whether a 512 MB arena, chip region, executable data, and explicit
  interpreter stacks can all satisfy the required address bounds. Use the
  system page size, not a hard-coded 4 KB value. MAP_32BIT is not a portable
  arm64 allocation strategy. Validate complete ranges and reject truncation.
- Check whether function/table addresses actually used in 32-bit fields fit;
  allocating only the arena at a low address is insufficient.

Gate: keep the direct-pointer model only if supported mechanisms satisfy all
requirements on every intended deployment version. Do not depend on private
APIs, disabling SIP, or an unsigned-only development configuration.

If the direct-pointer gate fails, use an explicit guest-address model:

- Represent Amiga memory addresses as 32-bit offsets into guest memory;
  keep native addresses as uintptr_t/pointers.
- Inventory ptr/address, BPTR conversion, raw casts, pointer arithmetic,
  serialized tables, native entry registration, and stack addresses. Assign
  guest ranges to generated data and preserve interior-pointer arithmetic;
  arbitrary handle lookup alone cannot emulate memory-address semantics.
- Give native callable entries and runtime marks explicit registries/IDs
  where they are opaque references, preserving a separate path for guest
  code/data addresses. Make guest-visible stack storage explicit where
  ordinary C++ locals currently escape as 32-bit addresses.
- Keep file formats, Amiga layouts, and guest-visible address arithmetic
  unchanged. Introduce the new host mode incrementally, with round-trip and
  bounds tests. This is a broader portability project, useful for Windows
  and other 64-bit platforms too, not a one-line arm64 fix.

## 2. Replace or verify context switching

main.cpp and tasks.cpp currently use deprecated Darwin ucontext routines.
Verify both symbol availability and actual operation on arm64; compiling a
header is insufficient. Prefer a supported, tested context backend with
caller-supplied stacks, evaluating Boost.Context's Darwin arm64 backend.
Keep SDL video/event work on the main OS thread.

Test stack alignment, callee-saved integer/SIMD registers, floating-point
state, nested task yields, and the existing setjmp/longjmp error and restart
paths. Avoid switching across live objects requiring destruction. Preserve
register/platform requirements and test optimized builds; do not implement
context switching by merely assigning a stack pointer. If guest-address
translation removes the need for low native stacks, simplify startup while
retaining cooperative task semantics.

## 3. Audit calls and layouts for the Apple arm64 ABI

The arity dispatchers in interp/chrget.cpp and runtime/glue.cpp call entries
through synthesized function types. Apple arm64 argument extension, stack
argument layout, and return rules must match the real callee. Existing
function-pointer reinterpret_casts are not proof of ABI correctness.

Create probes for zero through ten parameters, signed/unsigned narrow
values, pointers, 32/64-bit results, and each glue entry kind. Generate typed
adapters from entry signatures where needed, especially for mixed widths
and pointer arguments. Do not replace typed calls with a variadic shortcut:
Apple arm64 passes variadic arguments differently from fixed arguments.

Audit the global -fpack-struct=2 setting. Preserve Amiga layouts, but keep
Darwin/libc/SDL/context-library types at their native ABI packing, preferably
behind normally compiled platform wrappers with fixed-width interfaces.
Check alignment of setjmp buffers, contexts, native pointers, and any atomic
objects. Use memcpy/explicit guest accessors for unaligned data where needed;
do not assume every access tolerated on x86 is valid on arm64.

## 4. Make the build architecture-aware

Add configurable host compiler/architecture/flags, per-architecture output
directories, and linker options selected by OS and architecture. Remove -m64
for arm64. Do not apply Intel-only linker workarounds to every Darwin build.
Make the host link recipe propagate real failures and preserve diagnostics.
Leave Amiga cross-compiler flags and Linux/x86-64 outputs unchanged.

Intended interface, not currently implemented:

```sh
make host-arm64 HOST_CXX=clang++
```

Keep Python resource generation native to the build machine. After each
architecture works separately, optionally combine Mach-O slices with lipo;
never mix x86-64 and arm64 object files in one ordinary link step.

## 5. Reuse macOS integration and package native dependencies

Reuse framework/dylib discovery, filesystem integration, and main-thread SDL
handling from the Intel Mac port. Verify the installed SDL2 framework has an
arm64 slice with lipo/file. Test video, audio, keyboard, mouse capture, and
file drop on hardware. Leave optional speech independent of core startup.

Use _NSGetExecutablePath-based resource discovery instead of /proc/self/exe.
For distribution, bundle native/universal SDL and resources in an .app,
configure install names/rpaths, and sign the finished bundle. Verify the
address/context strategy still works after signing and under the intended
release security settings. Test a release package without developer tools
or an AMOS_HOME override before claiming distribution support.

## 6. Validate and preserve existing targets

Run address, context, ABI, and layout probes first; then headless interpreter
fixtures, allocation/bank tests, error recovery, file-selector task tests,
and GUI/audio checks. Include Debug and optimized builds. Compare floating
point results with existing tolerances; investigate architecture-dependent
rounding rather than silently updating golden results.

The current X11/XTest GUI runner needs a macOS automation backend. Add an
arm64 macOS CI runner, retaining Linux, Intel macOS, and Amiga build checks.
A successful cross-link or Rosetta run does not validate the native port.

First milestone: a native arm64 probe report deciding whether direct host
addresses are viable, followed by either minimal platform work or the explicit
guest-address refactor. No support claim before hardware runtime validation.

## 7. DMG distribution and Finder document opening

The packaging design is shared with the Intel Mac port; native arm64 and a
universal DMG remain conditional on the address/context/ABI gates above.
[RELEASES.md sections 2, 3 and 5](RELEASES.md) specify the common state split,
editor-load operation, Info.plist metadata and release checks.

1. Stage `AMOS Professional.app`, bundling the matching SDL framework/dylib
   under Contents/Frameworks and immutable resources under Contents/Resources.
   Resolve bundle-relative paths and put writable state in Application
   Support/Caches/temp. No global SDL installation or writable app is needed.
2. Declare the proposed UTI `io.github.dschwen.amosystem.program`, extension
   `.amos`, and Editor document role in Info.plist, with a document icon.
   Owner rank advertises capability; it does not override a user's choice.
3. Integrate Finder's cold/warm document-open events into the common editor-load
   queue. SDL2 delivers Cocoa file opens as drop events, which the current
   host treats as device mounts; distinguish document opens and preserve the
   existing folder-mount behavior. Test queued startup events and unsaved work.
4. Sign nested code and the completed app, validate the actual signed runtime's
   address/context behavior, notarize/staple, and create a drag-to-Applications
   DMG using hdiutil. Complete and validate the final image's signing/notary
   workflow; test a downloaded artifact with normal Gatekeeper settings.
5. Document Finder Get Info > Open with > AMOS Professional > Change All for
   users who want AMOS as their `.amos` default. Test competing handlers,
   reinstall/upgrade, installation in both Applications locations, and cleanup
   without resetting Launch Services or deleting user preferences.

Offer a universal DMG only after both architectures pass native tests; use
lipo on matching-version slices and bundle universal/matching dependencies.
The first Intel DMG need not wait for the native arm64 port. A DMG is a delivery
container, not a substitute for an application bundle or document event handling.

## References

- Apple architectural differences: https://developer.apple.com/documentation/apple-silicon/addressing-architectural-differences-in-your-macos-code
- Apple arm64 ABI: https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms
- Apple loader source: https://github.com/apple/darwin-xnu/blob/main/bsd/kern/mach_loader.c
- Wine developer report on arm64 macOS constraints and newer facilities: https://list.winehq.org/hyperkitty/list/wine-devel@winehq.org/thread/CKG5CEN2BE5VRXZ7O7NX4YUSBH3247WH/
- Boost.Context: https://github.com/boostorg/context
