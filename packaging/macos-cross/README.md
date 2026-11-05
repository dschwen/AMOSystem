# Experimental macOS cross-builds from Linux

The prototype cross-compiles both Intel and arm64 AMOS Mach-O executables in
Docker. It does not yet provide a working native arm64 AMOS runtime. The ARM
image uses a 4 GiB PAGEZERO region and loads its text/data above that boundary;
AMOS still stores those native addresses in 32 bits. ARM startup therefore
reports this limitation and exits before truncating pointers.

## Commands

```sh
make macos-cross-toolchain
make macos-cross-probes                         # arm64 by default
make host-macos-cross                          # experimental arm64 executable
make host-macos-cross MACOS_ARCH=x86_64         # Intel control build
make macos-cross-probes MACOS_ARCH=x86_64
```

Outputs are isolated under `build/macos-cross/arm64` and `x86_64`:

- `amos`: the cross-linked executable. ARM currently exits with an address-model
  diagnostic; its existence is not an arm64 support claim.
- `cross-report.json`: link status, CPU type, deployment target, PIE, segment
  addresses, hash and pending runtime validation.
- `toolchain.json`: compiler, SDK/toolchain configuration, installed package
  versions, source revision/dirty status and actual Docker image ID.
- `build.log`/`link.txt`: diagnostics. Failed app builds remove stale executables
  and successful reports. ABI configuration changes discard only cached objects.
- `probes/`: standalone address/allocation, native/guest layout, ucontext and
  nested local-jump executables, their build logs and a Mac runner.

The Dockerfile pins the upstream OSXCross 14.5-r0 toolchain and Ubuntu image by
manifest digest. The SDK is supplied by that upstream toolchain image, not this
Git repository. The initial implementation uses its stable cctools/ld64 711
backend and Ubuntu Clang 18; this differs from the upstream LLVM-flavor option
in the longer plan. `toolchain.json` describes the input images. Apt package
versions are recorded in artifacts rather than snapshot-pinned, so retain the
built image/digest for exact compiler reproducibility. SDK use is subject to
Apple's terms; see upstream's SDK documentation. No SDK files enter AMOS release
or probe artifacts.

The container runs native Linux tools and never runs the target binaries.
Compiler/SDK wrappers are selected by target and the Mach-O CPU type is checked
in Python; a Linux ARM ELF binary is rejected. Probes and the application use an
explicit macOS 11 deployment target. This matters for Intel too: the SDK exposes
`MAP_32BIT` only for sufficiently recent deployment targets. Intel alone retains
the reduced-PAGEZERO/non-PIE flags. Existing Linux/Windows/Amiga defaults remain
unchanged. Sources and NDK headers are mounted read-only, generated output is a
separate writable bind mount, compilation has no network, and no privileged
container mode is used. The toolchain image build needs network access.

`NDK_INCLUDE=/path/to/NDK/Include_H` selects existing NDK headers. Otherwise the
wrapper uses `/opt/amiga/m68k-amigaos/ndk-include` or fetches the existing build
NDK dependency on the host. The build regenerates fallback font/keymap sources
without a ROM. `MACOS_CROSS_IMAGE` can select another locally built compatible
image; compiler target names are discovered rather than hard-coded to a Darwin
version. `tools/macos_cross.py --jobs N` controls application build parallelism.

## Run probes on an actual Mac

The probe target also exports `build/packages/amos-macos-arm64-probes.tar.gz`.
Copy/extract that archive (or `build/macos-cross/arm64/probes`) into a writable
folder on an Apple Silicon Mac and run:

```sh
cd /path/to/probes
sh ./run-probes.sh
```

Downloaded archives may acquire Apple's quarantine attribute. Ad-hoc signing
is not Developer ID signing/notarization, so Gatekeeper can block the probes
before they execute. The runner detects quarantine and reports it without
changing the attribute or repeatedly attempting blocked launches.

For these locally built AMOS test binaries, after checking their origin, approve
them under System Settings > Privacy & Security > Open Anyway, or remove the
download flag from just these four files in the extracted directory:

```sh
xattr -d com.apple.quarantine address-probe layout-probe context-probe jump-probe
sh ./run-probes.sh
```

`No such xattr` means that file already lacks the attribute. No sudo or global
Gatekeeper change is needed. [Apple's explanation of the warning and per-app
authorization](https://support.apple.com/en-us/102445).

The runner ad-hoc signs each executable locally, records OS details, and saves
stdout JSON/stderr plus signing logs. A context-probe failure is recorded while
the remaining probes still run. `direct_pointer_gate: false` produces address-probe exit code 3 and a failed
runner status: the measurement completed but the AMOS address-model gate failed. Ordinary hints and the real SDK `MAP_32BIT` allocation
mode are both measured; do not assume a low arena also makes function/table or
stack addresses fit. A basic ucontext pass does not establish complete ABI or
SIMD/FP-state preservation. Native execution and final signing/runtime behavior
remain untested until these reports come back from hardware.

The Intel executable needs an AMOS resource tree and SDL framework to exercise
the editor, as documented in the main README. It can serve as a cross-toolchain
control on Intel or under Rosetta. It does not validate native ARM operation.
The existing Mac-only `package-dmg` target remains Intel-only; it is not wired
to package the guarded ARM executable as a release.

## What changed and what remains

`core/local_jump.h` uses aligned native `jmp_buf` storage on Darwin arm64 while
retaining GCC/Clang builtins elsewhere. Capture stays at a live stack frame.
Dialog validation state lives outside its setjmp capture function so modified
nonvolatile locals are not read after standard longjmp. Native and builtin
nested recovery are tested at O0/O2; existing Linux dialog/file-selector GUI
fixtures remain regression gates. Runtime marks also receive ARM alignment.

Next: run the native probes, decide the guest-address/layout strategy, then
implement that translation or a demonstrated supported layout backend. The
ordinary ARM Mach-O produced here cannot safely run the current direct-pointer
runtime. Context/register/typed-call validation, GUI tests and ARM packaging
follow that decision. See [the detailed plan](../../docs/MACOS-CROSS.md).
