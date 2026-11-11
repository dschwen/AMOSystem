// The host's devices (docs/DEVICES.md): the units (DF0:...DF3:, DH0:...DH9:),
// what is in them (folders, with a label), the assigns, the library of
// disks, the volatile RAM:, and how they are set up (amos.conf, the command
// line, AMOS_DEVICES, AMOS_ASSIGNS). host/hal_files.cpp asks it what a
// name is.
#pragma once

#include <stddef.h>

namespace amos::host::devices {

enum class Status { found, no_disk, not_mounted };

// An option of the command line before the program (--df0 path[=label][,ro],
// --dh3, --assign NAME=path, --library path, --devices path, --config file):
// false if it is not one of ours
bool option(const char *name, const char *value);
// The window: in full screen at the start (configuration, --fullscreen); a change remembered for Save
bool fullscreen_at_start();
void remember_fullscreen(bool on);
// The extension the setup puts in a slot (1-26): [extensions] 8 = AMOSPro_AMCAF.Lib, --extension 8=FILE;
// its file ("" none), null: the interpreter's configuration says
const char *extension_file(unsigned slot);
// The joystick of the Amiga's port 1 (0) or 2 (1): auto, none, keyboard, pad1... (empty: never said)
const char *joystick_setting(int port);
void remember_joystick(int port, const char *source);

// The program AMOS is to run (the host's path of it): its profile
// (Name.amos.conf), and its folder mounted under its own name
void set_program(const char *path);

// What "name:" (n characters of name) is: the host directory of an assign, a
// volume (a label), a unit, or a disk of the library (inserted then). Not a
// disk: status says if the unit is empty (no_disk) or the name unknown
const char *resolve(const char *name, size_t n, Status &status);

// Is there such a device (a unit, even empty, a volume, an assign)
bool exists(const char *name, size_t n);

// The label of the volume that a host directory is the top of (RAM: is
// "Ram Disk"), or null
const char *volume_of(const char *real);

// Is a host path on a write-protected disk
bool read_only(const char *path);

// AssignLock / "assign NAME: dir": the assign, or none (path null) to remove it
bool assign(const char *name, size_t n, const char *path);

// ---- the disks of the units: put in and taken out while AMOS runs ---------------
// (each raises the input event DISKINSERTED or DISKREMOVED, which AMOS.library
// reads: a "Please insert volume" requester ends by itself when the disk comes)

constexpr int Floppies = 4, Hards = 10, Units = Floppies + Hards;

struct UnitInfo {
    char name[8];       // "DF0", "DH3"
    bool exists;        // a floppy drive, or a disk in a hard disk unit
    bool has_disk;
    bool read_only;
    bool saved;         // a unit the configuration has (it is written by save_config)
    char label[64];
    char path[1024];
};

// A unit's number from its name ("df0", n characters), or -1
int unit_of(const char *name, size_t n);
bool unit_info(int unit, UnitInfo &info);

// A folder as the disk of a unit (what was in it is taken out first), with its
// label (the folder's name if none). False, and the reason in message, if it cannot
bool insert(int unit, const char *folder, const char *label, bool read_only, const char *&message);
// A folder as a hard disk, in the next free DH unit (or the first with no disk): its unit, or -1
int mount_folder(const char *folder, const char *label, bool read_only, const char *&message);
// The disk of a unit taken out
bool eject(int unit);
bool set_read_only(int unit, bool read_only);

// Write the disks of the units the user set up (and the library, the assigns and the
// drives the user's file has) to the user's amos.conf, the old one kept as amos.conf.bak
bool save_config(const char *&message);
const char *config_path();
// The library's folders
int library_count();
const char *library_path(int n);

// Each name for Dev First$ / the file selector's Devices list: the assigns,
// the volumes (device true) and the units
void each(void (*f)(const char *name, bool device, void *data), void *data);

} // namespace amos::host::devices
