# AMOS platform icons

`AMOS_LOGO.png` is the supplied 256×256 RGBA logo, copied without modification.
All platform assets preserve its transparency and square canvas. Larger entries
are resampled from the supplied image; they contain no extra source detail.

Regenerate with `make icons` (Python/Pillow required). Generated assets are
checked in so regular builds and packaging do not require Pillow.

- `amos.ico`: 16, 24, 32, 48, 64, 128 and 256 pixel entries. Used by Windows
  executable resources, MSI Add/Remove Programs, Start Menu and `.amos` icons.
- `amos.icns`: PNG-backed 16 through 1024 pixel entries for macOS bundles.
- `hicolor/*/apps/io.github.dschwen.amosystem.png`: Linux icons from 16 through
  512 pixels. Installation also places the same image under the AMOS MIME name.

## Installer integration

Windows: `make package-msi VERSION=0.1.2` embeds the ICO in both executables
and the MSI. The resource generator tracks the ICO as an input. Existing MSI
association/shortcut metadata points at those resources. Use a new numeric
VERSION when changing an already installed package.

Linux: `make install-host` installs the hicolor images, desktop entry and MIME
XML alongside the existing host installation. `uninstall-host` removes only
our files. Desktop cache refresh tools are used when available on a real
installation, and skipped under DESTDIR. Neither path sets the user's default
application. DEB/RPM packagers can stage just the metadata/icons with:

```sh
python3 tools/install_icons.py linux --prefix "$pkgroot/usr"
```

Use the distribution's cache triggers on installation/removal, not caches
created inside the staging tree. DEB/RPM package builders are still planned;
this does not create them or address their separate per-user state requirements.

macOS: for a completed app bundle, run before codesigning/notarization and DMG
creation:

```sh
make macos-icons APP_BUNDLE="build/AMOS Professional.app"
```

This installs `Contents/Resources/amos.icns`, sets `CFBundleIconFile`, and adds
`CFBundleTypeIconFile`/`UTTypeIconFile` to existing AMOS document declarations.
Other bundle keys/types are preserved. The helper requires an existing
Info.plist; it does not create a nonfunctional placeholder app. Changing these
resources invalidates an existing signature, so sign the finished bundle after
this step. `make package-dmg` now performs this step automatically; see
[macOS packaging](../../packaging/macos/README.md).
