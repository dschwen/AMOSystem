// The Request extension (extensions/+Request.s, slot 3): who answers the
// system's requesters
#include "core/amos.h"

using namespace amos;

namespace {

void requests(uint32_t mode)
{
    Regs r;
    r.d[0] = mode;
    sy_call(sy::Request_OnOff, r);
}

} // namespace

extern "C" {

// REQUEST WB: the Workbench's
void ext_request_wb() { requests(1); }
// REQUEST ON: AMOS's
void ext_request_on() { requests(uint32_t(-1)); }
// REQUEST OFF: none, the answer is always "cancel"
void ext_request_off() { requests(0); }
}

extern "C" {
// The warm start (ExtAdr+4, at each run): AMOS's requesters

void ext_request_warm() { requests(uint32_t(-1)); }
}

// The extension's cold start (routine 0): d1 "APex" from AMOSPro 1.10 on.
// Out: d0 the extension's number, d1 (low word) its version; d0 = -1 and
// a0 = 0 for another version.
static uint32_t cold_start(RegFrame &r, uint32_t number, uint16_t version, void (*start)())
{
    if (r.d[1] != 0x41506578) {
        r.d[0] = uint32_t(-1);
        r.a[0] = 0;
        return r.d[0];
    }
    if (start)
        start();
    r.d[0] = number;
    r.d[1] = (r.d[1] & 0xFFFF0000) | version;
    return r.d[0];
}

extern "C" uint32_t ext_request_cold(RegFrame &r)
{
    constexpr uint32_t ExtNb = 3 - 1;
    idata().ExtAdr[ExtNb * 4 + 1] = address(AMOS_DATA(ext_request_warm_entry));
    return cold_start(r, ExtNb, 0x0200, nullptr);
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_request.inc"
#endif
