# Linux-to-macOS arm64 cross-compilation plan

Status: proposed, researched 2026-11-05. No container, cross-build or native arm64
support is implemented by this document. The current working Mac target is
Intel; `package-dmg` explicitly builds and validates x86-64. This plan complements
[the native Apple Silicon port plan](APPLE-SILICON.md).

## Recommended toolchain

Use **OSXCross inside a Linux Docker build image**, with Clang, libc++, a pinned
macOS SDK and Mach-O linker/binutils. OSXCross supports Apple arm64 targets;
its current upstream `llvm` flavor is recommended for new toolchain builds.
Pin a tested revision, compiler/linker versions, SDK archive hash and base image
rather than following moving tags. SDK version and minimum deployment version
are separate settings; start probes with a macOS 11 deployment target and
raise it only for a demonstrated requirement.

[crazy-max/docker-osxcross](https://github.com/crazy-max/docker-osxcross) is the
ready-made Docker option the user may have heard of. It publishes toolchain
stages suitable for copying/mounting into a Linux builder; inspect/pin its image
digest and dependency versions before using it. Its documented usage adds a
Linux userspace/compiler around `/osxcross`, rather than assuming the toolchain
image is a ready-to-run AMOS development shell. A project-specific image built
from pinned upstream OSXCross with an SDK extracted from the user's Xcode or
Command Line Tools is the preferred reproducible setup. SDK acquisition and use
must follow the applicable Apple terms; keep SDK files out of this repository
and application releases. See [upstream SDK instructions](https://github.com/tpoechtrager/osxcross/blob/master/README.SDK.md).

The container runs Linux on the build machine's architecture. On this Linux
x86-64 machine it can generate Darwin arm64 directly, without executing ARM
code. `--platform=linux/arm64` selects the container architecture; it does not
select a macOS compilation target or provide a macOS runtime. Linux ARM emulation
would not validate Darwin loader, GUI or context behavior either.

## 1. Establish and pin the container toolchain

Add `packaging/macos-cross/Dockerfile`, a build wrapper and toolchain lock
metadata. The image should contain the selected OSXCross toolchain plus native
make, Python, Git, file-inspection tools and the existing NDK header dependency.
Keep Python resource generation on Linux. Generate fallback font/keymap data;
never mount private ROMs into the release build.

For a prebuilt toolchain prototype, use its upstream multi-stage pattern:

```dockerfile
# Design sketch: supply an inspected tag@sha256 digest when implementing.
ARG OSXCROSS_IMAGE
FROM --platform=$BUILDPLATFORM ${OSXCROSS_IMAGE} AS osxcross
FROM ubuntu:24.04 AS builder
RUN apt-get update && apt-get install -y --no-install-recommends \
    clang lld libc6-dev make python3 git file ca-certificates xz-utils
COPY --from=osxcross /osxcross /osxcross
ENV PATH="/osxcross/bin:$PATH"
ENV LD_LIBRARY_PATH="/osxcross/lib"
WORKDIR /src
```

Pin the base digest/package versions in the actual implementation too. Verify
that copied tools run on the selected base image and that compiler-rt/sysroot
libraries are complete. Discover the arm64 compiler wrapper from the installed
toolchain; do not guess a Darwin version suffix or accidentally use Linux g++.
OSXCross documents target-prefixed `arm64-apple-darwinXX-clang++` commands and
its xcrun wrapper. Record the resolved target triple, sysroot, architecture,
SDK/libc++ versions and linker in the artifact manifest.

Compile/link a C++20 hello-world using the SDK libc++ and deployment target,
then inspect `file`, Mach-O CPU type, load commands and imported libraries.
Verify `__APPLE__` and the ARM64 target macro. Run that tiny executable on the
Apple Silicon Mac. Container setup is complete only when this cross-produced
binary actually runs there. Cache the image/toolchain, not architecture-mixed
AMOS objects. Mount sources read-only and a dedicated output directory writable;
run compilation with the invoking UID where practical, without privileged mode.

## 2. Run the address-model gate before a full port

Cross-build standalone probes for loaded function/global/stack addresses,
page size, low-range arena allocation and context switching. Run them natively
on Apple Silicon, with OS/toolchain versions and signing state recorded.
Inspect both requested Mach-O segment addresses and actual runtime addresses.
Test complete allocation ranges, not just their starting pointers.

This is the largest project-specific risk. `ptr()`/`address()` in
`src/core/amos.h` currently turn AMOS 32-bit values into real native pointers.
This includes program memory, generated tables, callable entries and pointers
to interpreter locals. The Intel reduced-PAGEZERO/non-PIE linker recipe must
not be reused blindly on arm64. Apple's current loader source contains an
explicit 4 GiB hard-PAGEZERO check for 64-bit ARM under its enforcement path;
therefore low-memory success cannot be assumed from a successful link.
[Apple loader source](https://github.com/apple-oss-distributions/xnu/blob/main/bsd/kern/mach_loader.c)

Make a written decision after the probes:

- If a supported layout mechanism genuinely meets every address requirement,
  record the required OS/SDK/linker/signing configuration and supported versions.
  New layout-emulation facilities mentioned in the native plan are research
  candidates, not a working solution demonstrated for this application.
- Otherwise plan the guest-address conversion described in
  [ADDRESSING.md](ADDRESSING.md) and [APPLE-SILICON.md](APPLE-SILICON.md): guest
  memory offsets/ranges, callable-entry/mark registries, and explicit handling of
  guest-visible stack data. Changing only the heap allocator is insufficient.

Do not spend the full port trying to bypass one compiler error at a time if
ordinary arm64 macOS cannot support the current address representation.

## 3. Make build platform and architecture explicit

The current Makefile uses build-machine `uname` for Darwin link flags and
unconditionally includes `-m64`. In a Linux container this selects Linux flags
unless changed. Introduce explicit target OS, CPU, architecture flags and
linker selection, with defaults preserving existing Linux and Amiga targets.
Architecture decisions must follow the compiler target, not container uname.

Proposed targets, **not commands available today**:

```sh
make macos-cross-toolchain
make host-macos-cross MACOS_ARCH=arm64
make host-macos-cross MACOS_ARCH=x86_64   # useful control build
```

Use separate `build/macos-cross/arm64` and `build/macos-cross/x86_64` object
and generated-source directories. Arm64 compilation should use an explicit
Darwin target/sysroot, libc++, deployment target and ordinary arm64 ABI,
without `-m64`, Linux `-no-pie`, or the Intel PAGEZERO workaround. Retain real
link failures and their diagnostics. No target executable may run as a build
step in the Linux container. The Intel control cross-build can prove tooling
and macOS source integration independently of the native arm64 port.

## 4. Resolve compile/runtime blockers deliberately

- Replace the unsupported builtin jumps in `src/lib/dialog.cpp` and
  `src/lib/fsel.cpp` with a target-selected jump abstraction. Their existing
  `void *buf[5]` and `void **quit` storage cannot simply be reinterpreted as a
  Darwin `jmp_buf`. Use correctly sized/aligned storage and keep setjmp at the
  live caller's capture site; a helper that returns before longjmp is invalid.
  Audit modified automatic variables, nested targets and destructor lifetimes.
  Start with an arm64 backend while keeping proven Amiga/Intel behavior until
  the same recovery tests pass for a shared implementation.
- Probe Darwin arm64 ucontext availability and behavior. Startup and file-selector
  tasks currently switch onto low stacks in `src/host/main.cpp` and `tasks.cpp`.
  Test alignment, integer/SIMD/FP register preservation, nested yields and error
  recovery. Evaluate a pinned Boost.Context backend if necessary. SDL video and
  event processing must stay on the main OS thread.
- Audit typed call dispatch in `runtime/glue.cpp` and `interp/chrget.cpp`,
  especially narrow arguments, stack-passed arguments and return values. Use
  fixed signatures matching callees, not variadic substitutes.
- Retain the native C++ alignment boundaries introduced in commit `499bce6`,
  plus explicit guest structure layouts. Extend the native-type boundary to
  any remaining Darwin SDK/context structures instead of removing guest packing
  globally. Run the ABI/layout probe with the cross compiler and on the Mac.

First application milestones are a linked experimental arm64 binary, then
headless interpretation/error recovery, then file-selector tasks and GUI.
Label cross-linked artifacts experimental until native runtime gates pass.

## 5. Stage on Linux; validate and package on macOS

Reuse existing `_NSGetExecutablePath` discovery, bundled-framework lookup,
immutable Resources and per-user Application Support mirror. SDL must contain
an arm64 slice. Because SDL is loaded dynamically, the initial executable
cross-link does not need SDL headers/link libraries; staging and GUI tests do
need a matching framework. Inspect its dependencies and preserve framework
symlinks. Keep resources committed/default-only and ROM-free, with the existing
licenses, icon and file hashes.

Split `tools/package_macos.py` into portable app staging and Mac finalization.
It currently validates only Intel, invokes absolute Apple tool paths and has
Intel metadata. Add an explicit architecture and deployment version; update
binary checks, `LSArchitecturePriority`, filenames and manifests. Do not just
feed an arm64 binary to the present `package-dmg` target.

Transfer the staged app or cross-build artifacts to an Apple Silicon Mac for
native tests and final SDK-tool checks, signing, notarization and `hdiutil`
DMG creation. Keep the existing Intel `package-dmg` behavior intact. Linux
Mach-O tools can inspect cross outputs; they do not replace the Mac runtime
or the established signing/DMG verification steps. Universal binaries come
later, after both slices pass independently.

## 6. Acceptance and delivery order

1. Pinned Docker/SDK toolchain; cross-built hello-world runs on Apple Silicon.
2. Native address/context probe report choosing the viable pointer model.
3. Architecture-aware build and jump adapter; compiler/ABI/layout tests pass.
4. Arm64 executable passes headless interpreter, bank/file/allocation and error
   recovery fixtures on hardware, with optimized and debug builds.
5. GUI, file selector, clipboard, audio, user-state preservation and document
   opening pass natively; source data/goldens remain compatible.
6. Signed arm64 app/DMG passes bundle checks and clean-machine deployment.

Every implementation stage retains Linux's normal regression suite, Windows
cross-build/tests, Intel Mac build/tests and Amiga cross-compilation. Add a
Docker cross-compile CI job and a separate Apple Silicon runtime job. A Linux
container building ARM ELF or a Rosetta test of the Intel binary does not meet
these arm64 acceptance criteria.

Recommended first implementation: the pinned toolchain and standalone probes,
plus an Intel cross-build control. The address-model decision determines the
scope of the full native arm64 port; do not promise a ready-to-run AMOS binary
from the Dockerfile alone.

## Sources

- [OSXCross build flavors, supported targets and usage](https://github.com/tpoechtrager/osxcross)
- [Docker OSXCross images and multi-stage usage](https://github.com/crazy-max/docker-osxcross)
- [OSXCross SDK preparation](https://github.com/tpoechtrager/osxcross/blob/master/README.SDK.md)
- [Apple arm64 ABI guidance](https://developer.apple.com/documentation/xcode/writing-arm64-code-for-apple-platforms)
- [Apple Mach-O loader](https://github.com/apple-oss-distributions/xnu/blob/main/bsd/kern/mach_loader.c)
