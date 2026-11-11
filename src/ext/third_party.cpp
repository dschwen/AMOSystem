// What Turbo Plus and AMCAF share (third_party.h)

#include "ext/third_party.h"
#include "ext/amcaf_pt_tables.h"
#include "core/guest_scratch.h"

using namespace amos;

namespace {

uint32_t numbers[2];

// The extensions' error messages: number 0
const char ErrMes[] = "This command is not available in this version of the extension";

} // namespace

namespace amos::ext3 {

uint32_t number(Id id) { return numbers[uint32_t(id)]; }

uint32_t cold_start(RegFrame &r, Id id, uint16_t version)
{
    if (r.d[1] != 0x41506578) {
        r.d[0] = uint32_t(-1);
        r.a[0] = 0;
        return r.d[0];
    }
    numbers[uint32_t(id)] = r.d[0] & 0xFF;
    r.d[1] = (r.d[1] & 0xFFFF0000) | version;
    return r.d[0] & 0xFF;
}

[[noreturn]] void error(Id id, uint32_t n)
{
    const uint8_t *table = id == Id::amcaf ? messages::Amcaf : messages::Turbo;
    size_t size = id == Id::amcaf ? sizeof messages::Amcaf : sizeof messages::Turbo;
    interp_error(n, 0, number(id), guest_constant(table, size));
}

[[noreturn]] void unsupported(Id id)
{
    interp_error(0, 0, number(id), guest_constant(ErrMes, sizeof ErrMes));
}

} // namespace amos::ext3
