// src_w_requester_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_requester_glue_s[8] = {
};
}

namespace amos::host {

void relocate_src_w_requester_glue_s()
{
}

const GlueEntry entries_src_w_requester_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_requester_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_auto_request), nullptr, 0, 4, "w_auto_req"},
    {AMOS_ENTRY_AT(host_obj_src_w_requester_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_easy_request), nullptr, 0, 4, "w_easy_req"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
