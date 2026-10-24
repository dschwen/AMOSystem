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
