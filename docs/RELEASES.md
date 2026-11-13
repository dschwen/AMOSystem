# Packaging and distribution plan

Status: mostly planned. The Windows portable ZIP (`amos-VERSION-windows-x64-portable.zip`, unzip and run, built by the Windows MSI workflow with `tools/package_windows.py --release VERSION`) is attached to each `v*` release next to the MSI. Experimental Windows portable ZIP/smoke packaging
(`make windows-dist`) and a Linux-built unsigned MSI (`make package-msi`) now
exist; MSI installation/lifecycle tests and signing are pending. Linux, Intel macOS and Windows x86-64 host code
now builds; native Apple Silicon remains a separate port. `make package-linux` builds the Linux portable tarball and a
`.deb` (see [Linux packages](#linux-packages-implemented)); the RPM remains planned. Windows document
registration/editor activation is implemented but needs installed-payload VM
validation; the other OS document integrations remain planned.
See [Windows](WINDOWS.md), [Apple Silicon](APPLE-SILICON.md), and
[addressing constraints](ADDRESSING.md) for runtime prerequisites.

## 1. Deliverables and build ownership

| Platform | Primary download | Build/package tools | Additional download |
|---|---|---|---|
| Windows x64 | `amos-VERSION-windows-x64.msi` | Linux MinGW-w64 + wixl/msitools; Windows VM installation tests | Portable ZIP |
| Intel macOS | `amos-VERSION-macos-x86_64.dmg` containing `AMOS Professional.app` | Apple Clang/SDK, `hdiutil`, `codesign`, `notarytool`, `stapler` | ZIP for automation |
| Native Apple Silicon | `amos-VERSION-macos-arm64.dmg` (`make package-dmg MACOS_PKG_ARCH=arm64`) | Same Apple tools | Built and text-tested natively in CI (`macos-arm64.yml`); window/audio testing on real hardware still pending |
| Debian/Ubuntu x86-64 | `amos_VERSION-REVISION_amd64.deb` | debhelper and `dpkg-buildpackage` in the supported distribution build environment | Portable tarball |
| Fedora/RHEL-family x86-64 | `amos-VERSION-RELEASE.DIST.x86_64.rpm` | RPM spec and `rpmbuild`, preferably clean `mock` builds for each supported distribution | Source RPM |
| Amiga | optional `AMOS-amiga-VERSION.zip` | Existing cross compiler and `make dist` | Keep separate from host installers |

Do not label Linux arm64 or macOS universal artifacts as supported just because
a compiler accepts the architecture. Package only validated slices. Keep the
current GNU Make host build; introducing a packaging backend does not require
migrating the Amiga build to CMake.

Introduce separate staging/package targets, provisionally `dist-host`,
`package-msi`, `package-dmg`, `package-deb`, and `package-rpm`. These names are
planned, not current commands. Have each consume a versioned payload manifest
and reject missing runtime resources, licenses, or a wrong-architecture library.

## 2. Shared release prerequisites

### Immutable payload and writable user state

The present host changes into the AMOS resource tree and can write configuration
there. A writable portable directory does not establish that a Program Files,
Applications, or `/usr/share` installation works. Fix this before production
installers:

- Keep binaries, bundled libraries, default configurations, and resources
  immutable. Find them relative to the executable or application bundle.
- Separate user configurations, devices, RAM/temp directories, caches, and
  documents. Seed user defaults once, with explicit migration on upgrades.
- Windows: use the user's Known Folder APIs for Local/Roaming AppData as
  appropriate, under `AMOSystem`; never write user data under Program Files.
- macOS: use Application Support, Caches and temporary-directory APIs under
  the app identifier; never mutate a signed `.app` or require a writable DMG.
- Linux: use XDG_CONFIG_HOME, XDG_DATA_HOME and XDG_CACHE_HOME, with their
  standard user-directory fallbacks. Package-owned files stay under `/usr`.
- Preserve AMOS assigns and relative program asset behavior through explicit
  resource/data roots rather than making the system installation writable.
  User overrides such as AMOS_HOME remain available for development.

Uninstall removes package-owned files and registrations, not user programs,
mounted device folders, or preferences. Upgrades preserve user state and
existing default-app choices; reinstall/repair must not reset them.

### Resource provenance and reproducibility

Release font/keymap generation must explicitly use redistributable fallback
data, even if the builder has a Kickstart ROM installed. The current Windows
package is a development/test bundle, not evidence of a ROM-free release build.
Inventory every resource copied from AMOS, include applicable notices, and
exclude downloaded NDK headers, personal configs, developer paths and private
signing material. Optional espeak remains a separately installed dependency
unless a deliberate redistribution decision is made.

Build Linux native packages against supported distro runtimes; generate shared
library dependencies instead of assuming that static libstdc++ makes glibc
portable. Bundle the selected SDL runtime on Windows/macOS; use distribution
SDL packages for DEB/RPM. Pin dependency versions and publish checksums, build
provenance, source/version identifiers, and third-party notices.

## 3. Opening .amos documents: common behavior

Proposed stable application ID: `io.github.dschwen.amosystem`.
Proposed document MIME type: `application/x-amos-program`.
Proposed macOS UTI: `io.github.dschwen.amosystem.program`.
Confirm these identifiers against existing registrations before implementation;
reuse an established type if one is discovered. Never claim generic
`application/octet-stream`, every `.bas` file, or all Amiga bank files.

**The association's Open action should load the program in the AMOS editor.**
Keep running it a distinct action/CLI choice. The existing CLI normally runs a
named program before returning to the editor, so pointing shell registration
at it unchanged is not the complete document-opening implementation.

Add a native document-open entry point, provisionally
`amos --open-document <path>`, parsed before device options. Preserve arguments
as paths, including spaces, quotes and Unicode; do not concatenate them into
an unescaped AMOS command line. Implement the editor-load operation and any
unsaved-document prompt. Existing CLI execution and `amos-player` keep their
behavior. Define multiple-document handling and repeated opens while the
application is already running; process-one-file-per-launch is an acceptable
first Windows/Linux policy, with macOS document events queued in the live app.
All three shell routes below must reach the same editor-load operation.

Registering a supported document type makes AMOS available in Open With. It
is not a promise to overwrite the user's default. Test both a previously
unclaimed `.amos` type and one explicitly assigned to another application.

## 4. Windows MSI and Explorer associations

### Installer

Start with a single-scope, per-machine x64 MSI built on Linux with
`wixl`/msitools, after a feature/upgrade probe establishes that its supported
authoring subset is sufficient. Use full WiX on a Windows runner if a required
feature is missing. See [the detailed MSI plan](MSI.md). MinGW builds the
application on Linux in either case. Install
under `ProgramFiles64Folder/AMOSystem`, with `amos.exe`, `amos-player.exe`,
`SDL2.dll`, immutable resources, icons and notices. Add Start Menu shortcuts
and an ARP entry. Do not change PATH by default. Per-user installation can be
a later separate package/scope; avoid mixing HKCU and HKLM ownership in one
unpredictable installer.

Use a stable UpgradeCode, well-defined product/version rules, stable component
identities, and MSI-managed files/registry entries. Test major upgrade, repair,
rollback, uninstall and a standard user's launch after an elevated install.
No custom action should recursively copy files into each user's profile or
run AMOS with elevated privileges. Keep the portable ZIP independent of MSI
registration. Sign the final executables and MSI in CI and verify signatures.

### Registration specification

Author these as installer-owned entries in the 64-bit registry view. This is
an outline, not a `.reg` file to run:

```text
HKLM\Software\Classes\AMOSystem.Program
  (Default) = AMOS BASIC program
  DefaultIcon\(Default) = "<installed>\amos.exe",<document-icon-index>
  shell\open\command\(Default) = "<installed>\amos.exe" --open-document "%1"
HKLM\Software\Classes\.amos\OpenWithProgids
  AMOSystem.Program = <empty REG_NONE>
HKLM\Software\Classes\Applications\amos.exe
  FriendlyAppName = AMOS Professional
  SupportedTypes\.amos = <empty string>
  shell\open\command\(Default) = "<installed>\amos.exe" --open-document "%1"
HKLM\Software\AMOSystem\Capabilities
  ApplicationName = AMOS Professional
  ApplicationDescription = AMOS BASIC editor and runtime
  FileAssociations\.amos = AMOSystem.Program
HKLM\Software\RegisteredApplications
  AMOS Professional = Software\AMOSystem\Capabilities
```

Provide the actual icon resource before referencing its index. Quote both the
executable and document placeholder. Notify the shell of changed associations
through the installer/shell integration mechanism. MSI Extension/ProgId/Verb
metadata or equivalent explicit registry components must have the same result;
validate WiX's emitted MSI tables rather than assuming an Extension entry alone
establishes a modern per-user default.

On Windows 10/11, installation registers eligibility. It cannot silently set
itself as the user's default handler through the old Default Programs APIs.
Offer an optional, non-elevated Finish/application action that opens Windows
**Settings > Apps > Default apps**, where the user selects `.amos` and AMOS
Professional. Explorer's **Open with > Choose another app > Always** is another
supported route. Do not write, delete or fabricate the protected `UserChoice`
registry entry or use hash bypass utilities. Silent MSI installation must not
launch a settings window. Managed deployments can supply administrator-owned
association policy separately from the MSI.

Uninstall removes only this product's ProgID, capabilities, Open With values,
shortcuts and files; it must not remove the entire `.amos` key or another
application's entries. Let Windows handle a now-unavailable user selection.

Sources: [Microsoft association model](https://learn.microsoft.com/en-us/windows/compatibility/file-type-and-protocol-associations-model),
[Open With registration](https://learn.microsoft.com/en-us/windows/win32/shell/how-to-include-an-application-on-the-open-with-dialog-box),
[application registration](https://learn.microsoft.com/en-us/windows/win32/shell/app-registration),
[WiX Extension schema](https://docs.firegiant.com/wix/schema/wxs/extension/).

## 5. macOS application bundle and DMG

Package `AMOS Professional.app` with `Contents/MacOS/amos`, bundled SDL under
`Contents/Frameworks`, icons and defaults under `Contents/Resources`, and a
complete Info.plist. Use bundle-relative SDL lookup and install names/rpaths;
a release must not need `/Library/Frameworks/SDL2.framework` or Homebrew.
Use executable/bundle discovery rather than `/proc/self/exe` or a Finder
launch's current directory. Copying the `.app` to `/Applications` or the user's
Applications folder should be sufficient; a drag-to-Applications DMG needs no
privileged installer script. The DMG is a transport image, not a writable
resource/configuration directory.

Sign nested code and then the bundle, verify the chosen signing/runtime
settings with the low-address and context tests, notarize/staple the app, and
create the final compressed DMG with `hdiutil`. Sign/notarize/staple the DMG
as appropriate for the pinned release workflow and validate both tickets.
Test a downloaded/quarantined artifact on a clean Mac, including offline
staple validation. Do not make quarantine removal the normal installation
procedure. Publish an Intel DMG first if necessary; a universal DMG requires
native testing of both slices, including the arm64 address-model gate.

### Launch Services document declarations

In Info.plist declare:

```text
CFBundleIdentifier = io.github.dschwen.amosystem
UTExportedTypeDeclarations = [{
  UTTypeIdentifier = io.github.dschwen.amosystem.program
  UTTypeDescription = AMOS BASIC program
  UTTypeConformsTo = [public.data]
  UTTypeTagSpecification = {
    public.filename-extension = [amos]
    public.mime-type = application/x-amos-program
  }
}]
CFBundleDocumentTypes = [{
  CFBundleTypeName = AMOS BASIC program
  CFBundleTypeRole = Editor
  LSItemContentTypes = [io.github.dschwen.amosystem.program]
  LSHandlerRank = Owner
  CFBundleTypeIconFile = amos.icns
}]
```

Use imported rather than exported type declarations if the type already has a
canonical owner. Verify uppercase `.AMOS` files too. `Owner` is a ranking
hint, not an override of an explicit user selection. Installing/discovering
the bundle registers its capabilities with Launch Services; a DMG script must
not reset the user's Launch Services database.

Finder launches and **Open With** deliver documents as application-open Apple events, not
argv. Implemented: `Info.plist` declares the exported type `io.github.dschwen.amosystem.program`
(`.AMOS`/`.amos`, conforming to `public.data`) and an Editor document type for it (Owner rank, the
`amos.icns` icon). SDL2's Cocoa backend turns the open event into an `SDL_DROPFILE` event once the
video is started, so on a launch with no arguments from inside a `.app` the host starts SDL's video
early (`host::startup_document`, `sdl.cpp`) and waits up to 250 ms for such an event; a `.AMOS` file
that arrives is opened as `--open-document` is. A file that arrives while AMOS is running (a second
double click) is dropped on the window like any file: a `.AMOS` one starts another AMOS for it
(`host::open_in_new_instance`, `open_document.cpp`: the program's executable with `--open-document`),
since the running editor has its work in it. Not tested: unsaved work in a running editor is not touched;
Dock drops and several files at once are one new AMOS each. Not tested on a Mac by CI (no Finder): by hand.

If AMOS is not selected by default, document Finder **Get Info > Open with >
AMOS Professional > Change All**. Registering the UTI/editor role alone cannot
guarantee the default when other candidates or user preferences exist.

Sources: [Apple document-type keys](https://developer.apple.com/library/archive/documentation/General/Reference/InfoPlistKeyReference/Articles/CoreFoundationKeys.html),
[SDL2 Cocoa open events](https://github.com/libsdl-org/SDL/blob/SDL2/src/video/cocoa/SDL_cocoaevents.m),
[Apple default-app instructions](https://support.apple.com/guide/mac-help/choose-an-app-to-open-a-file-mh35597/mac),
[notarization workflow](https://developer.apple.com/documentation/security/customizing-the-notarization-workflow).

## 6. Linux DEB/RPM packages and desktop integration

Both packages install the same logical payload:

```text
/usr/bin/amos
/usr/bin/amos-player
/usr/share/amos/AMOS/                  immutable defaults/resources
/usr/share/applications/io.github.dschwen.amosystem.desktop
/usr/share/mime/packages/amos.xml
/usr/share/icons/hicolor/.../apps/io.github.dschwen.amosystem.{png,svg}
/usr/share/icons/hicolor/.../mimetypes/application-x-amos-program.{png,svg}
```

Use standard package documentation/license locations for each distro. Build
DEB with `debian/control`, rules/install files and debhelper; use generated
`${shlibs:Depends}`/`${misc:Depends}` dependencies and shared-mime-info /
desktop integration as needed. `debian/amos.sharedmimeinfo` can be installed
by `dh_installmime`. Build RPM with an `amos.spec`, proper ownership and
automatic library dependencies. Use target-distro cache triggers/macros for
MIME, desktop and icon updates; inspect their ownership before adding any
manual scriptlets, and avoid duplicate or historical unconditional refreshes.
Validate desktop metadata with `desktop-file-validate`; lint DEB/RPM with the
chosen distro tools. Keep debug symbols in separate debug packages.

Use `apt install ./amos_...deb` for dependency-resolving local installation;
`dpkg -i` alone does not fetch missing dependencies. Use `dnf install
./amos-...rpm` for RPM systems. Provide signed APT/RPM repository metadata
if hosting update repositories; direct-download checksums do not replace
repository signature validation. Keep removal, purge and upgrade semantics
explicit and preserve user-owned projects/preferences.

### MIME registration

Proposed `/usr/share/mime/packages/amos.xml`:

```xml
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="application/x-amos-program">
    <comment>AMOS BASIC program</comment>
    <glob pattern="*.amos"/>
    <glob pattern="*.AMOS"/>
  </mime-type>
</mime-info>
```

Add content magic only after checking supported tokenized AMOS/AMOS Pro
headers against fixtures; one version's header must not misclassify other
versions. Proposed desktop entry:

```ini
[Desktop Entry]
Type=Application
Name=AMOS Professional
Comment=Edit AMOS BASIC programs
Exec=amos --open-document %f
TryExec=amos
Icon=io.github.dschwen.amosystem
Terminal=false
Categories=Development;IDE;
MimeType=application/x-amos-program;
```

`%f` delivers one local file per launch and must be passed directly, without
`sh -c` or hand-built shell quoting. Do not use `%u` until remote/file URL
handling exists. Match the identifier to the actual installed `.desktop`
filename and handle filenames containing spaces. Registering MIME/desktop
metadata refreshes available handlers; it does not itself establish a default.

For an explicit user choice, run in that user's graphical session:

```sh
xdg-mime default io.github.dschwen.amosystem.desktop application/x-amos-program
xdg-mime query default application/x-amos-program
xdg-mime query filetype example.AMOS
xdg-open example.AMOS
```

This changes the user's MIME preference (`mimeapps.list`), not a package-owned
file. Do not run it as root from DEB/RPM maintainer scripts or rewrite every
user's home directory. Offer a user-invoked action or desktop settings
instructions. System administrators may set a system default via the standard
MIME Apps policy files; existing user preferences retain their specified
precedence. Uninstall removes our files and refreshes caches using distro
mechanisms without deleting other handlers or rewriting user preferences.

Sources: [MIME Apps specification](https://specifications.freedesktop.org/mime-apps/latest/),
[desktop entry specification](https://specifications.freedesktop.org/desktop-entry-spec/latest/),
[Debian MIME integration](https://wiki.debian.org/MimeTypesSupport),
[dh_installmime](https://manpages.debian.org/testing/debhelper/dh_installmime.1.en.html),
[RPM file triggers](https://rpm.org/docs/6.0.x/manual/file_triggers.html).

## 7. Implementation sequence and release gates

1. Separate immutable resources from per-user writable state. Add ROM-free
   release staging and a dependency/resource manifest.
2. Implement the common editor-load request, Unicode/space-safe native paths,
   repeated document opens and unsaved-document handling. Retain normal CLI
   run behavior. Add icons and canonical type identifiers.
3. Package DEB/RPM metadata and verify MIME/default selection in fresh GNOME
   and KDE sessions. Test installation with `apt`/`dnf`, updates and removal.
4. Build the x64 MSI on Linux (or the WiX fallback runner), sign through
   SignPath (done, see "CI releases"), and test on Windows: standard-user operation,
   Settings/Open With selection, repair, major upgrade and uninstall.
5. Build the Intel `.app`/DMG, integrate cold/warm Cocoa document events, then
   sign/notarize and test installation from quarantine. Extend to native arm64
   or universal only after the architecture gates pass.
6. Tag-based CI builds and tests payloads, stages installers, signs only the
   final inputs, verifies signatures/artifacts, and uploads release files,
   checksums and provenance to GitHub Releases. Use signing secrets only in
   trusted release jobs. Linux and Amiga regression checks remain mandatory.

Every installer must pass the following on a clean supported OS without
compilers, SDKs, global SDL installations, or an AMOS_HOME override:

- Launch from Start/Finder/application menu and open a document by association.
- Exercise cold and already-running opens, spaces/Unicode, uppercase suffixes,
  paths on another drive/volume, and a different launch working directory.
- Verify editor-load versus explicit-run behavior and an unsaved-document prompt.
- Preserve another app's default on install, allow an explicit AMOS default
  choice, keep that choice through upgrade, and cleanly remove owned metadata.
- Run the address/context probes and text/GUI smoke tests from the installed
  payload, including the signed macOS configuration and Windows fiber checks.
- Update, repair where applicable, rollback failed installation, and uninstall
  without destroying user data or changing unrelated associations.

These are release acceptance criteria, not claims that the present development
bundles already satisfy them.

## Logo-derived installer icons

The source logo and generated Windows ICO, macOS ICNS and Linux hicolor PNGs
are checked in under `data/icons`. See [the icon integration notes](../data/icons/README.md).
Windows executable resources and the MSI now use this logo. Linux `install-host`
installs application/MIME icons and desktop metadata; DEB/RPM staging should use
the same `tools/install_icons.py linux` hook. For the planned macOS bundle/DMG,
run `make macos-icons APP_BUNDLE="path/to/AMOS Professional.app"` before signing.
This copies the ICNS and sets application/document icon keys in the existing
bundle's Info.plist. DEB/RPM builders remain future work; the DMG target is documented below.

The macOS app/DMG build target is now implemented as `make package-dmg`
(alias `make dmg`), with bundled SDL, logo, user-state seeding, signing and
mounted-image verification. See [macOS packaging](../packaging/macos/README.md).
Actual Mac runtime/Gatekeeper qualification, notarization and Finder document
event handling remain outstanding.

## Linux packages (implemented)

`make package-linux VERSION=0.1.0` (alias `make dist-host`) builds an optimised, ROM-free
`build/linux-release/amos` (AROS's Topaz and the plain keymap, whatever Kickstart is on the
builder) and `tools/package_linux.py` packs, into `build/packages`:

- `amos-VERSION-linux-x86_64.tar.xz`, a portable tree (`bin/amos`, `bin/amos-player`,
  `share/amos/AMOS`, desktop entry, MIME type, icons, licences). It runs from where it is
  extracted: the host finds `../share/amos/AMOS` from the executable.
- `amos_VERSION_amd64.deb` (needs `dpkg-deb` and `fakeroot`): the same under `/usr`, depending
  on `libsdl2-2.0-0` (SDL is loaded at run time) and recommending `libespeak-ng1`; the
  post-install/removal scripts only refresh the MIME, desktop and icon caches.
- a SHA-256 list and a JSON manifest (version, source revision, dirty flag).

The system tree is staged from git HEAD (`AMOS`, the examples, the blue editor palette), so
developer settings and untracked files stay out. A tree that cannot be written to
(`/usr/share/amos/AMOS`) is not used in place: the host keeps a copy in
`$XDG_DATA_HOME/amos/runtime` (default `~/.local/share/amos/runtime`), refreshed on each
start under a lock; the editor and interpreter configuration, macros and `.AMOS` files of the
copy are the user's and are not overwritten. `AMOS_HOME` and writable trees (a build,
`make install-host` into `~/.local`) run in place as before.

`.github/workflows/linux-release.yml` builds it, installs the `.deb` on the runner, runs a
program with `amos-player` and removes it again; for a `v*` tag it attaches the files to the
GitHub release. Not done: the RPM, a signed APT/RPM repository, fresh GNOME/KDE default-app checks, and lintian cleanliness (the executable is not position independent
until the guest-address migration is finished).

## Releases from a tag

Pushing a tag `vMAJOR.MINOR.PATCH` starts three workflows, each of which attaches its files to
the GitHub release of that tag (the first to finish creates it with generated notes):

| Workflow | Runner | Attached |
|---|---|---|
| `linux-release.yml` | ubuntu-24.04 | `.tar.xz`, `.deb`, SHA-256 list, manifest |
| `windows-msi.yml` | ubuntu-24.04 (MinGW, wixl), tests on windows-2022 | `amos-VERSION-windows-x64.msi`, manifest; the release step waits for the Windows Installer tests; signed through SignPath when it is set up (below), unsigned otherwise |
| `macos-release.yml` | macos-latest | Intel and native Apple Silicon `.dmg` (matrix) (SDL2 2.30.12, checked by SHA-256), manifest; ad-hoc signed, not notarized |

```sh
git tag v0.1.0 && git push origin v0.1.0
```

All three also run by hand (Actions, "Run workflow", with a version) without publishing.

**macOS** packages are ad-hoc signed and not notarized (notarization needs a paid Apple
Developer account, not planned): Gatekeeper blocks the first launch until Open Anyway (the
DMG's Read Me, the README). `MACOS_SIGN_IDENTITY` still signs with a Developer ID identity
(hardened runtime, timestamp) for a local build; notarytool/stapler are not wired in.

**Windows** releases are signed by the SignPath Foundation (free for open source). The
`sign` job of `windows-msi.yml` runs for a `v*` tag once the repository has the variable
`SIGNPATH_ORGANIZATION_ID` (and optionally `SIGNPATH_PROJECT_SLUG`, default `AMOSystem`, and
`SIGNPATH_SIGNING_POLICY_SLUG`, default `release-signing`) and the secret `SIGNPATH_API_TOKEN`.
It submits the artifact `release-msi` (the MSI that passed the installer tests) and waits up
to an hour for the signing request's approval in SignPath. SignPath signs `amos.exe` and
`amos-player.exe` inside the MSI, then the MSI (`.signpath/artifact-configuration.xml`, to be
pasted into the SignPath project as its artifact configuration). The job then checks every
signature with Windows' own `Get-AuthenticodeSignature`, installs and uninstalls the signed MSI;
the release job checks that only the two executables changed (`package_msi.py
--record-signed`, which also updates the manifest's hashes) and publishes it. A failed or
rejected signing publishes no MSI. Without the variable the job is skipped and the MSI is
published unsigned, as before. Setting up:

1. Apply at https://signpath.org (open-source licence, releases built in CI from public source;
   the README's "code signing policy" section is the policy they ask for).
2. In the SignPath project: add the GitHub trusted build system to it, set the artifact
   configuration from `.signpath/artifact-configuration.xml`, create the signing policy
   `release-signing` with an approver.
3. A SignPath CI user's API token as the secret `SIGNPATH_API_TOKEN`, the organization ID as the
   variable `SIGNPATH_ORGANIZATION_ID` (Settings, Secrets and variables, Actions).
