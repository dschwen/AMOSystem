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

// Each name for Dev First$ / the file selector's Devices list: the assigns,
// the volumes (device true) and the units
void each(void (*f)(const char *name, bool device, void *data), void *data);

} // namespace amos::host::devices
