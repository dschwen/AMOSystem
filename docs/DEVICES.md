# Plan: devices, floppies and hard disks on the host

Status: decided (section 9). **D1 is built** (the model, `amos.conf`, the
command line, the library for folders, the launch folder, a volatile `RAM:`;
as described in docs/BUILDING.md's host list and the README); D2 onward are
not. Section 1 is how it worked before D1.

## 1. Today

The host's disks are the sub-folders of one **devices directory**
(`AMOS_DEVICES`, else `./devices`, else `~/.local/share/amos/devices`). Each
sub-folder is a volume: its folder name is the volume's label (`Work:`), what
is in it the disk's files. `RAM:` is one of them. On top of that:

- assigns: `START:` (where AMOS was started), `AMOSPRO:` (its tree), and
  those of `AMOS_ASSIGNS=NAME=path;...` and `c:assign`;
- a volume that is not there gives AMOS's own "Please insert volume X in any
  drive" requester (Retry, Cancel); `Drive("X:")` says no, `Dev First$` and
  the file selector's Devices list show the volumes.

What it cannot do, and old AMOS programs need:

- **Drives**: `DF0:`, `DF1:`, `DH0:`... do not exist. Programs written for a
  floppy use them (`Load "df0:pic.iff"`, `Dir First$("df1:")`) as often as
  labels.
- **Disks as the Amiga had them**: a floppy is a file (an `.adf`, 880KB) or
  nothing; there is no way to "insert" or "eject" one while a program runs,
  and a program that waits for a disk change waits for ever.
- **Labels different from the folder's name**, `Disk 1`, `Game Disk (2)`
  with characters a Windows folder cannot have.
- **Any UI**: setting this up is a folder and an environment variable. That
  is fine on Linux, and wrong on Windows and macOS, where a program is
  started from an icon.
- RAM: is a folder that stays between runs (on the Amiga it is empty at
  every start).

## 2. What the programs do

From the AMOS commands (`Dir`, `Dir First$`, `Dev First$`, `Drive`,
`Disc Info$`, `Dfree`, `Load`/`Save`/`Open`, `Assign`, the file selector) and
from the programs of the time:

1. **By drive**: `df0:`, `df1:`, `dh0:`: whatever disk is in the drive.
2. **By label**: `Work:`, `MyGame:`, `AMOS_Data:`: the disk with that name,
   wherever it is. A game disk often looks for its own label. Some ask for a
   second disk (`Disk2:`) and show the Amiga's requester: the player changes
   the disk and the game goes on.
3. **By assign**: `AMOSPRO_Examples:`, `FONTS:`.
4. Disk changes: the input events `DISKINSERTED` and `DISKREMOVED`, which
   AMOS.library already reads (its requester ends by itself when the disk
   comes).

## 3. The model

Three layers, as AmigaDOS has them:

| Layer | Is | Example |
|---|---|---|
| **Unit** | a drive: `DF0:`...`DF3:` (floppies), `DH0:`...`DH9:` (hard disks), `RAM:` | `DF0:` |
| **Medium** | what is in it: a *folder*, an *ADF image* (OFS/FFS), later an HDF; with its **label**, read-only or not | `Game Disk 1`, `Work` |
| **Name** | a unit's name, a medium's label, or an assign: all resolve to a medium | `df0:`, `Work:`, `FONTS:` |

Rules: a unit name is the medium in that unit; a label is the medium that
has it, in whichever unit (or in the **library**, 4.2); an assign is a
directory on a medium. `Dev First$` lists the units with a medium and the
labels. A medium put in a unit raises `DISKINSERTED`, taken out `DISKREMOVED`.
Hard disk units (`DH0:`...) are the folders of the devices directory and
those mounted (they get units in the order they were set up). A floppy unit is
empty until a disk is inserted: `df0:` then gives "no disk present".

## 4. Where disks come from

1. **Folders**: a folder is a disk, its label the folder's name or what the
   configuration says. As now, and any folder anywhere (not only below one
   directory).
2. **The library**: one or more folders (default `Documents/AMOS/Disks`,
   `~/.local/share/amos/disks`) scanned for folders and `.adf` files. Each
   has a label (a folder: its name; an ADF: the name in its root block, the
   BSTR at byte 432 of block 880). **When a program asks for a label that is
   not mounted, the host looks in the library and inserts it**: the "please
   insert Disk2" of an old game goes on by itself. Not found: the requester
   (with a note in the window, 5.3).
3. **The launch folder**: `amos C:\Games\MyGame\main.AMOS` (a double click)
   mounts `C:\Games\MyGame` as `MyGame:` too (and as `START:`), so a program that
   looks for its own disk finds it, and its parent folder is searched for
   other disks (`Disk2` next to `Disk1`).
4. **Disk images**: `.adf` (and `.adz`, gzipped ADFs, with zlib). See 6.

## 5. How the user sets it up

### 5.1 Every system: command line and a configuration file

```
amos --df0 Game1.adf --df1 Game2.adf --dh1 ~/amiga/Work=Work prog.AMOS
```

and `amos.conf` (INI style, one parser of ours, no dependency), in the
user's configuration directory (`~/.config/amos`, `%APPDATA%\AMOS`,
`~/Library/Application Support/AMOS`):

```
[library]
path = ~/Documents/AMOS/Disks
[df0]
image = ~/Disks/Game1.adf
readonly = yes
[dh1]
path = ~/amiga/Work
label = Work
[assign]
FONTS = ~/amiga/Fonts
```

A **profile** is the same file for one program (`MyGame.amos.conf` beside it,
or in a bundle made by `make game`), which wins over the user's. Order: command
line, profile, user, defaults. `AMOS_DEVICES` and `AMOS_ASSIGNS` keep
working (they are what the tests use).

### 5.2 The window: drag and drop

The window takes **files dropped on it** (SDL's drop events: the same on all
three systems):

- an `.adf`: inserted into `DF0:` (`DF1:` with Shift; if one is in, it is
  replaced, which is the Amiga's eject and insert);
- a folder: mounted as a hard disk, labelled with its name;
- an `.AMOS` program: run (a program is loaded as a program is).

### 5.3 The window: the Devices panel (F12)

An overlay of the host over AMOS's picture (so it works on every system and
in the player, and a program keeps running behind it; the AMOS pointer is
released while it is open, as for the Workbench): the list of units with
their media and labels; for each: **Insert...**, **Eject**, a write-protect
toggle; for hard disks **Mount folder...**; **New blank floppy**; the
library's folders; the assigns; **Save** (writes `amos.conf`). Drawn with
the topaz font the host already has (a list, buttons, a text line: about 400
lines, no UI library). The file choice is a small file browser in the same
panel (the same on every system, usable with the keyboard), with the
system's own dialog (tinyfiledialogs, zlib licence) as an option later.

While AMOS shows "Please insert volume X", the window says it too (the title
and a line at the bottom of the picture: "Drop the disk X here, or F12"): the
requester stays AMOS's, but the user sees what to do.

### 5.4 Windows and macOS: first run and files

- The installer (or the first start) makes `Documents\AMOS` with `Disks`
  (the library), `Hard Disks` and `Programs`, and the configuration. `.AMOS`
  and `.adf` open with AMOS: a program runs (4.3 for its folder); a double
  click on an ADF starts AMOS with that disk in `DF0:`.
- No drive letters are shown to AMOS: a Windows disk is mounted as a
  folder when the user wants it.

## 6. Disk images

A floppy image is what old programs came on (and what is on the web). Two
ways to use it:

| | **A driver of our own** (OFS/FFS in C++, over the file) | **Unpack when inserted** (into a cache folder) |
|---|---|---|
| Directory order | the disk's own (the hash table's: what `Dir` showed on a real floppy) | the host's |
| Free space, `Dfree`, `Disc Info$` | the disk's | the folder's |
| Names, comments, protection, dates | all kept | lost or in side files |
| Writes | to the image, as on the Amiga | to the folder; "Save as ADF" to keep them |
| Work | 1000-1500 lines | 300 (a reader and the folder) |

**Recommended: the driver, in three steps**: read-only (OFS and FFS,
international and dir-cache variants, bad files reported as the Amiga did),
then writing to existing files and creating new ones (a full FFS writer:
bitmap, hash chains, extension blocks), then formatting a new blank floppy.
Images are **write-protected by default** (a program that saves a game says
so, and the panel's toggle lets it write), and the image is changed only in
whole, checked steps (a copy beside it, renamed on eject) so a crash cannot
leave half a disk. A tool `tools/adf.py` (list, extract, pack) makes the
test images and is useful by itself. Hard disk images (`.hdf`, with RDB or
without) are not in this plan: a folder is the hard disk.

## 7. Names on the host's systems

- **Characters**: an Amiga label or file can have `: ? * " < > | \` and
  trailing dots and spaces, which a Windows (and partly a macOS) name cannot.
  Reversible escaping (`%3F` for `?`), the same in folders and in the files
  of an unpacked disk; reserved names (`CON`, `NUL`...) too. Labels with them
  appear in the configuration as they are.
- **Case**: AmigaDOS ignores it; Linux does not. The host looks names up
  without case already (by reading the directory: slow for a big directory,
  and a cache by directory fixes it).
- **Long names** (more than 107 characters) and paths longer than the
  Windows limit (`\\?\` names): reported as the Amiga reports "name too
  long".
- **Dates and protection bits**: the file's own; the protection bits and
  comments are kept in `.uaem` files (as FS-UAE does, `tools/uaem.py` knows
  the format), only if asked.
- **`RAM:`** becomes truly volatile: a temporary folder, emptied at every
  start.

## 8. Steps

| # | Step | Size (a guess, days) |
|---|---|---|
| D1 | The model: units, media, labels, library; `df0:`...`dh9:` resolve; `Dev First$`, `Drive`, `Disc Info$`; a volatile `RAM:`; the command line and `amos.conf`; the launch folder mounted | 4-5 |
| D2 | Disk changes: insert/eject while running (`DISKINSERTED`/`REMOVED`), the requester finishes by itself when the disk comes, the library finds a label that was asked for | 2-3 |
| D3 | The window: drag and drop, the F12 panel with its file browser, the "insert disk X" hint, saving the configuration | 5-7 |
| D4 | ADF, read-only (the driver, the label, the library's ADFs, `tools/adf.py`, tests with images made by it) | 4-5 |
| D5 | ADF, writing and formatting (the FFS writer, write-protect, safe write-back) | 5-7 |
| D6 | Windows and macOS: default places, name escaping, file associations, first-run folders (together with the releases, `docs/RELEASES.md`) | 3-4 |
| D7 | Later: `.adz`, HDF, native file dialogs | |

Tests: the model with folders (D1-D2) in `tests/host_only` (a program that
uses `df0:`, a label found in the library, a disk change); the ADF driver
with images `tools/adf.py` writes and reads back, checked against the files
and against the structure of real disks; the window's parts by hand (the
host has no way to drop a file: a small `AMOS_DROP=file` environment
variable fakes one for the tests).

## 9. Decisions (made)

1. **Images**: the driver of our own, in the three steps of 6 (not unpacking).
2. **The panel**: our own overlay with its own file browser (one code for the
   three systems); the systems' own dialogs only if that is ever wanted.
3. **The configuration**: INI-style `amos.conf`.
4. **The launch folder** is mounted under its own name (4.3).
5. **Images are write-protected** until the user allows writing.
6. **Hard disks are folders only**: no HDF images (D7 keeps `.adz` and native
   dialogs).
7. **Unit names for folders**: `DH0:`, `DH1:`... in the order they were set up
   (the folders of the devices directory and the library, by name, take
   the numbers the configuration leaves free).
