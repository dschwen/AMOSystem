// The AMCAF extension (AMOSPro_AMCAF.Lib V1.50beta4, 11-Jan-98; default slot 8) in C++:
// docs/THIRD-PARTY-EXTENSIONS.md. The tables are written by tools/ext_gen.py
// (data/extensions/amcaf.json says which commands are implemented).

#include "core/guest_scratch.h"
#include "ext/amcaf.h"
#include "ext/third_party.h"
#include "lib/strings.h"

using namespace amos;
using ext3::Id;

namespace {

constexpr uint32_t StateSize = 9142;

} // namespace

namespace amos::amcaf {

uint8_t *state()
{
    static uint8_t *zone = guest_object<uint8_t>(StateSize);
    return zone;
}

uint8_t *state_at(uint32_t at) { return state() + at; }

} // namespace amos::amcaf

extern "C" {

// AMCAF BASE: the extension's data
int32_t amcaf_base() { return int32_t(address(amcaf::state())); }

// AMCAF LENGTH: its size (not the size of the library's file)
int32_t amcaf_length() { return int32_t(StateSize); }

// AMCAF VERSION$
uint32_t amcaf_version_s()
{
    static const char text[] = "AMCAF extension V1.50beta4 11-Jan-98 by Chris Hodges.";
    return str_from(text, sizeof text - 1);
}

// The cold start (routine 0)
uint32_t amcaf_cold(RegFrame &r)
{
    uint32_t result = ext3::cold_start(r, Id::amcaf, 0x0200);
    if (int32_t(result) >= 0)
        idata().ExtAdr[result * 4] = address(amcaf::state());
    return result;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf.inc"
#endif
