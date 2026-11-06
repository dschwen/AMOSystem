// A floppy image (ADF) read as a disk: OFS and FFS, international or not
// (docs/DEVICES.md, section 6; tools/adf.py is the reference and makes the
// test images). Read-only.
#pragma once

#include <stddef.h>

namespace amos::host::adf {

// The disk's label (its root block's name), without reading the rest of the
// file: false if the file is not a disk image
bool label_of(const char *image, char *label, size_t size);

// The files of an image into an empty folder (made if it is not there). The
// folder lists as the disk does: each directory in its hash table's order. Files
// with a bad block are written as far as they can be read, and counted in
// bad. False, and the reason in message, if the image is not a disk
bool unpack(const char *image, const char *folder, char *label, size_t size, int &bad, const char *&message);

} // namespace amos::host::adf
