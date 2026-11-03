# Build an AMOS DMG on macOS

From a checkout containing the packaging changes, install Apple's Command Line
Tools, Python 3 for building, and SDL2.framework from the official
[SDL2 2.30.12 disk image](https://www.libsdl.org/release/SDL2-2.30.12.dmg)
under `/Library/Frameworks`. Then run on the Mac:

```sh
make package-dmg VERSION=0.1.3
# Alias: make dmg VERSION=0.1.3
```

This creates `build/packages/amos-0.1.3-macos-x86_64.dmg` and a matching JSON
manifest. Open the DMG and drag **AMOS Professional.app** onto **Applications**.
The end user needs neither Python nor an SDL installation. The Applications
shortcut and instructions are on the image; the app has the AMOS logo icon.

The target builds an optimized Intel executable in `build/macos-release`,
explicitly regenerating fallback Topaz/keymap data without a private Kickstart
ROM. Both executable and bundled framework must support x86-64; native arm64
and universal app binaries are rejected until the Apple Silicon work is ready.
The minimum target is macOS 11.0. On Apple Silicon, the Intel build additionally
needs Rosetta for runtime testing; this is not native arm64 support.

The framework defaults to `/Library/Frameworks/SDL2.framework`; override with
`MACOS_SDL_FRAMEWORK=/path/to/SDL2.framework`. `MACOS_CXX` defaults to
`/usr/bin/clang++ -arch x86_64`, selecting Apple Command Line Tools even when
a Conda/Miniforge compiler is on PATH. Override `MACOS_BUILD` to use another
object directory when switching toolchains (for example
`MACOS_BUILD=build/macos-apple`). Framework symlinks are preserved and must stay within
the framework. `lipo` and `otool` checks reject wrong architectures and
unbundled non-system dependencies. Sources/default resources are staged from
Git HEAD, excluding developer settings, untracked files and Amiga binaries.
The current executable and icon files come from the working tree; the manifest
records the Git revision and whether that tree was dirty.

## Bundle runtime behavior

The native executable finds `Contents/Resources/AMOS` through
`_NSGetExecutablePath`, and loads the included framework from
`Contents/Frameworks/SDL2.framework`. SDL2 still uses the existing dynamic loader;
it now tries the bundled framework before development/global locations.

On bundled launch, defaults are copied/refreshed into
`~/Library/Application Support/AMOSystem/runtime` under a per-user file lock.
Interpreter/editor config, editor macros and existing `.AMOS` scripts are
preserved; other packaged resources are refreshed atomically. Unknown user files
are retained. The app can therefore run from a read-only DMG or signed app
without writing into its bundle. Existing host disk/config options still apply;
an explicit AMOS_HOME bypasses bundle resource seeding. Deleting the app leaves
user state and projects intact.

The bundle includes SDL, project and AROS font notices/data under
`Contents/Resources/licenses`. No ROM is distributed. The SDL license comes
from upstream release-2.30.12/LICENSE.txt, unmodified.

Finder `.amos` document events and default-app registration remain separate
work. This bundle deliberately does not advertise a handler that cannot yet
receive those events. Open files using the AMOS editor, or from Terminal:

```sh
"/Applications/AMOS Professional.app/Contents/MacOS/amos" --open-document /path/to/program.AMOS
```

## Signing and public distribution

By default the framework and app are ad-hoc signed. The builder verifies the
app signature, creates a compressed UDZO image, verifies its checksum, mounts
it read-only, compares app file hashes/framework symlinks to staging, verifies
the mounted signature, and detaches it. A JSON manifest accompanies the image.
These checks do not establish editor/runtime correctness or Gatekeeper trust.

For a configured Developer ID identity/keychain:

```sh
make package-dmg VERSION=0.1.3 \
  MACOS_SIGN_IDENTITY='Developer ID Application: YOUR NAME (TEAMID)'
```

This signs inside-out with hardened runtime and timestamps, then signs the DMG.
It does not obtain certificates, upload to Apple, or claim notarization. For
public release, use your configured notarytool profile to submit the finished
DMG, wait for acceptance, staple the ticket, and assess Gatekeeper trust. Keep
notary logs and runtime-test the signed app. Stapling changes the DMG bytes;
regenerate the final checksum/provenance after that step. Do not modify the
app's executable/resources/Info.plist after signing.

## Validation status

Portable tests validate bundle staging, symlinks, metadata/licenses, committed
defaults and user-state refresh/preservation. Linux regressions and the Amiga
build must continue to pass. Actual macOS DMG creation and GUI/Gatekeeper tests
are required on the Mac: the virtual Mac connection was refused during local
implementation, so those results are not claimed here.

On a clean Mac, test Finder launch, window/audio/clipboard, editing/saving,
first-run seeding, upgrades preserving preferences/projects, launch from the
read-only image, and explicit `--open-document` with Unicode/spaced paths.
The Intel executable retains the low-address/non-PIE linker requirements described
in `docs/ADDRESSING.md`; changing signing or linker settings needs fresh address
and context validation. Do not enable PIE merely to satisfy a signing check.

Repeat without the global SDL framework and without AMOS_HOME. Run the
address/context probes under the final signing configuration. Test notarized
release artifacts from quarantine before publishing.

## C++ library packing

The guest build uses `-fpack-struct=2` for AMOS/NDK data. Native C++ headers and
private host structs containing library objects are explicitly restored to
native alignment; the include boundary then restores guest packing. This
avoids newer libc++ format-layout assertions such as `14 == 16`. Do not remove
the guest packing flag globally or disable those library assertions.
