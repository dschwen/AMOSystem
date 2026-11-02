# MSI installer implementation plan

Status: the Linux MSI build is implemented. `make package-msi VERSION=0.1.0`
produces an unsigned x64 AMOS installer, generated WXS, payload hashes and MSI
table inspection report under `build/packages`. The two-file feature probe
also builds in versions 0.1.0/0.1.1/0.1.2 for lifecycle/rollback testing.

See [packaging/windows/README.md](../packaging/windows/README.md) for the actual
build, tool bootstrap and Windows test commands. The design below records the
implementation goals; Windows installation/lifecycle results and a signing
identity remain outstanding release gates.

Implemented and checked locally:

- wixl/msitools 0.103 compilation, x64 metadata, component/registry ownership,
  downgrade protection, transactional upgrade sequencing, and extracted-payload
  hashes. The real AMOS MSI contains no custom-action code.
- Separate ROM-free release build, icon/version resources, compiler/SDL/AROS
  notices, and staging from committed resource defaults.
- Installed-mode per-user working resources with preserved settings/scripts,
  native wide-path adapters, and editor-only `--open-document` activation.
- Five authoring/inspection tests, Linux document-open tests, 85 Linux text
  regressions, and Amiga cross-compilation/linking.
- A PowerShell lifecycle/standard-user runner and unsigned build/test CI
  workflow. These Windows runner/VM checks have not yet run from this checkout.

Unsigned artifacts are for validation, not a declaration of production MSI
readiness. [RELEASES.md](RELEASES.md) covers the other platforms and remaining
signing/distribution work.

## Toolchain decision

Use **wixl from GNOME msitools** for the first Linux-native MSI. It consumes a
WiX-like XML definition and creates the MSI/cabinet payload without running
Windows tools through Wine. It supports an x64 target. Use `msiinfo` to inspect
tables and `msiextract` to verify the packaged files.

On Debian/Ubuntu, the intended packaging environment includes `wixl` and
`msitools`, alongside the already installed MinGW compiler and SDL SDK.
Pin the versions in a build container/CI image. The development machine uses a local extraction under `build/msi-tools`
created by `make msi-tools`; no system-wide installation was required. Package generation itself requires
neither Windows nor Wine; runtime installation testing still requires Windows.

**wixl is not the full WiX toolset.** Keep the first installer in its supported
WiX-style subset, with MSI-managed file/registry/shortcut operations and basic
installer progress rather than a custom wizard. Do not assume modern WiX
schemas, WiX UI/extensions, upgrade helpers, or custom-action libraries work.

Before committing to the full authoring pipeline, make a small feature probe:
package two dummy files, an x64 Program Files directory, HKLM registry entries,
a Start Menu shortcut, uninstall metadata and upgrade sequencing. Build two
versions and prove install/repair/upgrade/uninstall on Windows. Inspect the MSI
architecture, component attributes, registry roots and action sequences. If a
required feature is unsupported, use maintained WiX on a Windows packaging
runner for that feature set instead of hiding unsupported authoring behind
post-build table patches. MinGW compilation remains on Linux either way.

Sources: [GNOME msitools overview and limitations](https://wiki.gnome.org/msitools),
[wixl command line and architecture selection](https://manpages.debian.org/trixie/wixl/wixl.1.en.html).

## 1. Fix installation prerequisites

Before making a production Program Files installer:

- Separate immutable AMOS resources from writable configurations, device
  folders, caches and user programs. Implement per-user Known Folder discovery
  and first-run default migration. Test with the installation directory
  unwritable to a standard user; do not solve this by granting write access
  to Program Files or launching AMOS elevated.
- Make an explicit ROM-free release payload, independent of the builder's
  installed Kickstart. Inventory resource licenses and include notices.
- Add native icon/version resources. Decide whether the initial MSI exposes
  just the editor shortcut or also a separate player executable/shortcut;
  the current ZIP does not yet provide amos-player.exe.
- Implement the common `--open-document` editor-load path before enabling
  the `.amos` Open registration. The current program argument runs code;
  registration alone is not the planned editor-open behavior. Validate Unicode,
  spaces, drive paths, repeated opens and unsaved documents.

The feature-probe MSI can be built before these changes, but a public installer
must not be declared ready while they remain unresolved.

## 2. Author the payload and MSI definition

Implemented source layout:

```text
packaging/windows/Product.wxs       fixed package/directory/feature structure
packaging/windows/Probe.wxs         isolated lifecycle/rollback fixture
packaging/windows/associations.wxi  owned document/application registry entries
packaging/windows/README.md         tool versions, build and VM test procedures
tools/package_msi.py                stage/validate payload, generate file components
```

Reuse the Windows binary/resource staging, not the ZIP file itself. Separate
release contents from developer probes and smoke fixtures; keep a test bundle
available independently. Generate a deterministic, sorted file manifest, with
explicit Source paths and stable component IDs/GUIDs derived from logical
installed paths. Apply Windows Installer component rules when files move or
component ownership changes; do not blindly regenerate every GUID each build.

First production scope: **per-machine x64**, one installation root under
`ProgramFiles64Folder/AMOSystem`, with amos.exe, SDL2.dll, immutable AMOS resources,
icons and notices. Add Start Menu and Add/Remove Programs entries. Embed the
cabinet so the MSI is a single downloadable file. Keep PATH unchanged.

Use a permanent UpgradeCode, product/package identities appropriate to each
release, numeric MSI ProductVersion mapping from the project's version, and
explicit downgrade prevention. Define major-upgrade removal sequencing and
rollback behavior. Verify 64-bit registry/component flags: a 64-bit executable
inside a package does not by itself select the correct registry view.

Make file, shortcut and registry ownership declarative. Avoid custom actions
for file copies, profile initialization, launching the application as admin,
or default-association manipulation. Let the application create its own user
state when each user first starts it.

## 3. Register .amos support without replacing user defaults

Use the registry design in [RELEASES.md section 4](RELEASES.md):

- `HKLM/Software/Classes/AMOSystem.Program`, a document icon, and a quoted open
  command: `"<installed>/amos.exe" --open-document "%1"` (Windows separators
  in the actual registry value).
- The product's own `.amos/OpenWithProgids` value and `Applications/amos.exe`
  SupportedTypes/open-command metadata.
- Application capabilities mapping `.amos` to AMOSystem.Program, plus a
  RegisteredApplications entry. Do not remove other handlers' values.

Windows 10/11 users select the default through **Settings > Apps > Default
apps**, or Explorer **Open with > Choose another app > Always**. The MSI
registers AMOS as eligible; it must not promise to silently become the default.
Do not write/delete UserChoice or use a hash bypass. If offering a post-install
settings link, run it in the interactive user's non-elevated context and never
during a silent installation. A simple first MSI can document that step rather
than implement a custom Finish action.

Test a clean extension and a pre-existing other default, then user selection of
AMOS, upgrade/repair, and uninstall. Remove only our owned registrations and
leave Windows to resolve an unavailable default after removal.

Source: [Microsoft's file-association model](https://learn.microsoft.com/en-us/windows/compatibility/file-type-and-protocol-associations-model).

## 4. Add the Linux build and inspection target

Implemented interface:

```sh
make package-msi VERSION=0.1.0 SDL2_WINDOWS=/path/to/SDL2/x86_64-w64-mingw32
```

The target should build the Windows host, prepare a clean release staging tree,
generate the file components, and invoke the pinned wixl. The compiler and
installer architecture must both be x64. A schematic direct invocation is:

```sh
wixl -a x64 -o build/packages/amos-0.1.0-windows-x64.msi packaging/windows/Product.wxs
```

The generator must resolve version/payload variables and included fragments;
this example does not replace the staging step. Keep the implementation in a
script with structured arguments rather than an increasingly fragile shell
command. Fail on missing inputs or unsupported authoring, not just missing
output files.

Automated Linux checks:

1. Inspect SummaryInformation, Property, Directory, File, Component, Feature,
   Media, Registry, Shortcut, Upgrade and InstallExecuteSequence tables as
   applicable. Check architecture, install root, version and owned keys.
2. Extract the MSI to a temporary directory and compare every payload hash
   against the staging manifest; reject extra or missing files.
3. Check imported DLLs, include SDL2.dll/notices, and reject developer paths,
   ROM-derived resources, probes or signing secrets in the release payload.
4. Rebuild to check deterministic authoring identities; document unavoidable
   package/signature timestamp differences rather than claiming identical
   signed bytes. Retain logs and the source/manifest revision.

## 5. Install and validate in Windows

Copy each MSI to the VM's shared folder. Use disposable VM snapshots and
capture verbose Windows Installer logs, for example:

```powershell
msiexec.exe /i .\amos-0.1.0-windows-x64.msi /qb /L*v install.log
msiexec.exe /x .\amos-0.1.0-windows-x64.msi /qb /L*v uninstall.log
```

Automated execution should start msiexec with a waitable process and collect
its exit status, interpreting reboot-required statuses separately from errors.
No Linux-only inspection substitutes for these Windows tests:

- Elevated install followed by standard-user launch from the Start Menu.
- Open/edit a document through Explorer and verify resources/preferences work
  without AMOS_HOME, an SDK, global SDL, or a writable install directory.
- Run installed-payload headless tests and address/fiber probes; test the GUI
  and file-selector task. Keep probe binaries outside the public MSI.
- Install v1, modify user state, upgrade to v2, repair, reject a downgrade,
  simulate a failed install/rollback, and uninstall. Preserve user data and
  unrelated/default-app settings, with no orphaned components/shortcuts.
- Validate spaced/non-ASCII paths and different user accounts. Do not claim
  full Unicode installation support until the remaining narrow CRT paths
  have been addressed and tested.

Start with unsigned development MSIs. Release Authenticode signing can also
be done on Linux with osslsigncode when the selected certificate/key service
supports that workflow; otherwise use a trusted signing runner/service. Sign
the final executable before building its MSI, then sign the MSI, inspect its
contents/signatures, and test Windows trust on the downloaded artifact. Signing
is a separate step from producing a functional MSI, not a prerequisite for the
local feature probe.

## 6. Acceptance and CI

The first milestone is a Linux-built **feature-probe MSI plus Windows install,
upgrade and uninstall logs**. The second is a development AMOS MSI with a
read-only installation/user-state split. The third adds the working document
association and signed release pipeline. Keep each stage reviewable.

Publish MSI and portable ZIP, checksums, dependency notices and provenance on
tagged releases. Keep Linux and Amiga build/regression checks in the pipeline.
A plan/authoring-only change needs metadata validation; runtime/backend changes
must retain the existing platform build checks.
