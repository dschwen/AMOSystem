// The extensions of other authors that are C++ here (docs/THIRD-PARTY-EXTENSIONS.md): Turbo Plus
// and AMCAF. Their tables are written by tools/ext_gen.py; this is what they share.
#pragma once

#include "core/amos.h"
#include "ext/vbl.h"

namespace amos::ext3 {

enum class Id : uint32_t { amcaf, turbo };

// The extension's number: its slot in the configuration - 1. The loader tells it at the cold start
// (d0), so the extension can be put in any slot.
uint32_t number(Id id);

// The cold start (routine 0): d1 "APex" from AMOSPro 1.10 on, d0 the number the loader gives.
// Out: d0 the number, d1 (low word) the version; d0 = -1 and a0 = 0 for another version.
uint32_t cold_start(RegFrame &r, Id id, uint16_t version);

// An error of the extension: the message n of its list (AMCAF's start at 1; Turbo Plus's at 0)
[[noreturn]] void error(Id id, uint32_t n);

// A command that is not in this version of the port: an AMOS error of the extension
[[noreturn]] void unsupported(Id id);

} // namespace amos::ext3
