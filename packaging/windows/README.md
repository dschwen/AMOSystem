# Linux-built Windows MSI

This builds unsigned installers (releases are signed in CI, below). Building/extracting/table checks
have run on Linux with wixl/msitools 0.103 and MinGW GCC 13-win32. Windows
installation/lifecycle and standard-user logs are still required. The GitHub
workflow is supplied but has not been run from this checkout.

## Build

System packages on Debian/Ubuntu are `wixl`, `msitools`, and the x86-64 MinGW
C/C++ compiler and windres. On a development host without sudo:

```sh
make msi-tools
```

That downloads/extracts Debian/Ubuntu packages under `build/msi-tools`, without
installing them system-wide, and records package versions. It relies on the
host's compatible base runtime libraries. CI uses apt-installed dependencies
on Ubuntu 24.04. The authoring script prefers the locally extracted tools if
present, otherwise system tools. Set `--tools-root` on tools/package_msi.py to
use another extracted tool set.

```sh
make -j4 package-msi VERSION=0.1.0 \
  SDL2_WINDOWS=/home/daniel/Downloads/SDL2-2.32.8/x86_64-w64-mingw32
make msi-probe VERSION=0.1.0
make msi-probe VERSION=0.1.1
make msi-probe VERSION=0.1.2
python3 -m unittest discover -s tests -p test_package_msi.py
```

`package-msi` builds in `build/windows-release`, explicitly regenerating the
font/keymap without reading a ROM. The authoring script independently verifies
those generated sources against the fallback generators before accepting the
binary. Resource staging requires a Git checkout because it reads committed
defaults from HEAD, excluding local editor changes and untracked files. It checks the executable's version-resource input, requires x64 PE
inputs, copies only committed AMOS defaults, adds licenses/notices, and emits:

- `build/packages/amos-0.1.0-windows-x64.msi`.
- A matching JSON report with ProductCode, payload hashes, inspected tables,
  tool version, MSI hash, source revision/dirty status and validation state.
- The generated WXS and staging directory for review.
- `test_msi.ps1` and an editor-only document test fixture next to the MSI.

Bump VERSION for every changed installer payload. Reusing an installed
version is not a supported way to deploy new binaries with the same MSI/file
version; use the next numeric version or uninstall the earlier development
package first.

The MSI is per-machine x64 with embedded cabinet, Start Menu/ARP metadata,
stable component GUIDs and UpgradeCode, versioned ProductCodes, downgrade
protection and rollback-capable major-upgrade sequencing. It does not modify
PATH or choose the user's default app. Registry entries advertise `.amos`
through ProgID/Open With/application capabilities. Users choose AMOS in Windows
Default apps or Explorer's Open With > Always.

The installed executable has an icon and version resource. `amos-player.exe`
is the same executable under the player name. `--open-document` loads one file
into a new editor process without running it, using an internal document assign
rather than constructing a command string from the filename. Both relative
and absolute native paths are accepted. AMOS's fixed program-name layout still
limits the UTF-8 leaf name (118 bytes with the document assign); longer names
fail explicitly. Native Windows file/stream/directory operations now convert
UTF-8 paths to wide CRT/Win32 calls; remaining guest string/length behavior and
Unicode profiles must be exercised on Windows before claiming broad coverage.

## Installed state

`installed.ini` beside the executable selects installed mode. On launch, a
normally packed Windows wrapper uses Known Folders to prepare
`LocalAppData/AMOSystem/runtime`. This writable working resource mirror retains
the original AMOS relative-path model while leaving Program Files untouched.
Startup refreshes changed application resources, seeds missing defaults, and
preserves interpreter config, editor config/macros, and existing BASIC scripts.
Unrelated user files are not removed. A per-user/session mutex serializes
seeding, and file replacement is atomic. New configuration formats will need
explicit migrations; preserving old bytes is not an automatic schema migration.

The device/config defaults also use the user's Known Folder location. Explicit
AMOS_HOME remains a development override; it bypasses installed-state seeding.
Portable ZIPs lack installed.ini and retain their existing portable behavior.
Uninstall removes only MSI-owned files/registrations, leaving user state intact.

## Windows validation

Copy the MSI/JSON files, the PowerShell script and document fixture from
`build/packages` to the VM. Use a disposable clean snapshot and 64-bit
PowerShell. The test script copies installer inputs locally before msiexec,
records verbose logs and handles reboot-required statuses.

First, from an elevated PowerShell, run the isolated feature probe:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\test_msi.ps1 -Mode Probe -Packages . -Artifacts .\logs
```

This installs v0.1.0, deletes/repairs a file, upgrades to v0.1.1, rejects a
downgrade, deliberately fails the v0.1.2 upgrade, verifies v0.1.1 files/registry
are restored, then uninstalls and checks cleanup. Its install directory is
`Program Files/AMOS MSI Probe`; the script refuses a dirty existing probe
folder. Only the probe MSI contains a tiny test DLL that returns installer
failure. The real AMOS MSI has no custom-action executable/DLL.

Install the real MSI from the elevated shell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\test_msi.ps1 -Mode Install -Packages . -Version 0.1.0 -Artifacts .\logs
```

From a separate **non-elevated** shell under the intended user:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\test_msi.ps1 -Mode User -Packages . -Artifacts .\user-logs
```

This checks that Program Files is not writable to that token, opens the
space-containing fixture into an editor without executing it, and checks
per-user defaults were seeded. It terminates its headless test editor after
checking it. Also test the GUI manually: Start Menu, Open With, Default apps,
file selector, Unicode/spaced paths, multiple independent editors, save/reopen
and unsaved work. Re-run the updated standalone windows-probe.exe to exercise
wide-path and user-state refresh/preservation tests.

Finally, from the elevated shell:

```powershell
powershell -NoProfile -ExecutionPolicy Bypass -File .\test_msi.ps1 -Mode Uninstall -Packages . -Version 0.1.0 -Artifacts .\logs
```

Check that preferences/projects still exist under the user's profile. Test a
real application upgrade with another numeric VERSION, and preserve another
application's `.amos` choice during install/upgrade/uninstall. Keep all logs;
Linux table inspection does not establish Windows runtime correctness.

## Tool compatibility and release qualification

wixl 0.103 does not support Package.Platform or MajorUpgrade.Schedule. `-a x64`
selects the package architecture, every component is explicitly Win64, and an
explicit sequence places RemoveExistingProducts after InstallInitialize. The
builder rejects unsupported-authoring warnings and verifies the resulting
tables and all extracted file hashes. It never patches tables after authoring.
The feature probe uses a deferred test DLL because this wixl also lacks the
WiX Error custom-action shorthand.

Windows Installer can report VersionNT=603 on Windows 10. The package instead
searches CurrentBuildNumber in the 64-bit registry and requires build 10240+
(or an already installed product for servicing). Do not change this to a
VersionNT>=1000 condition. Validate the search on the supported Windows images.

Local builds are unsigned. Releases are signed by SignPath in CI (docs/RELEASES.md,
"CI releases"): the executables inside the tested MSI, then the MSI, then Windows checks
the signatures and installs it, and `package_msi.py --record-signed` checks that only
`amos.exe` and `amos-player.exe` changed. Do not manually patch an already signed binary
or package. The CI workflow builds unsigned installers, checks Linux regressions,
and defines Windows runner install/repair/upgrade/rollback/uninstall checks;
standard-user GUI tests and local Amiga regression builds remain separate.

CI uses the redistributable fallback font. Its host regression run checks all
text output and image presence, skipping IFF byte comparisons against ROM-font
goldens with `--fallback-font`. The normal local ROM-backed suite retains full
image comparisons. The harness returns failure status when any comparison fails.
