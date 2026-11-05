// src_w_fonts_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_fonts_glue_s[28] = {
};
}

namespace amos::host {

void relocate_src_w_fonts_glue_s()
{
}

const GlueEntry entries_src_w_fonts_glue_s[] = {
    {host_obj_src_w_fonts_glue_s + 0, GlueKind::w, AMOS_BIND_W(w_ec_get_fonts), nullptr, 0, 2, "w_get_fonts"},
    {host_obj_src_w_fonts_glue_s + 4, GlueKind::w, AMOS_BIND_W(w_ec_free_fonts), nullptr, 0, 4, "w_free_fonts"},
    {host_obj_src_w_fonts_glue_s + 8, GlueKind::w, AMOS_BIND_W(w_ec_get_font), nullptr, 0, 4, "w_get_font"},
    {host_obj_src_w_fonts_glue_s + 12, GlueKind::w, AMOS_BIND_W(w_ec_set_font), nullptr, 0, 4, "w_set_font"},
    {host_obj_src_w_fonts_glue_s + 16, GlueKind::w, AMOS_BIND_W(w_ec_close_font), nullptr, 0, 4, "w_close_font"},
    {host_obj_src_w_fonts_glue_s + 20, GlueKind::w, AMOS_BIND_W(w_ec_menu_start), nullptr, 0, 4, "w_menu_start"},
    {host_obj_src_w_fonts_glue_s + 24, GlueKind::w, AMOS_BIND_W(w_ec_menu_stop), nullptr, 0, 4, "w_menu_stop"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
