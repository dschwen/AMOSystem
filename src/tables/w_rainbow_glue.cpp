// src_w_rainbow_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_rainbow_glue_s[20] = {
};
}

namespace amos::host {

void relocate_src_w_rainbow_glue_s()
{
}

const GlueEntry entries_src_w_rainbow_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_rainbow_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_ec_rain_set), nullptr, 0, 4, "w_rain_set"},
    {AMOS_ENTRY_AT(host_obj_src_w_rainbow_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_ec_rain_do), nullptr, 0, 4, "w_rain_do"},
    {AMOS_ENTRY_AT(host_obj_src_w_rainbow_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_ec_rain_var), nullptr, 0, 4, "w_rain_var"},
    {AMOS_ENTRY_AT(host_obj_src_w_rainbow_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_ec_rain_del), nullptr, 0, 4, "w_rain_del"},
    {AMOS_ENTRY_AT(host_obj_src_w_rainbow_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_ec_rain_hide), nullptr, 0, 4, "w_rain_hide"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
