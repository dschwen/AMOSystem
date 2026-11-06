// src_w_colours_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_colours_glue_s[44] = {
};
}

namespace amos::host {

void relocate_src_w_colours_glue_s()
{
}

const GlueEntry entries_src_w_colours_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_ec_set_colour), nullptr, 0, 4, "w_set_colour"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_ec_get_colour), nullptr, 0, 4, "w_get_colour"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_ec_set_palette), nullptr, 0, 4, "w_set_palette"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_ec_colour_back), nullptr, 0, 4, "w_colour_back"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_ec_flash_off), nullptr, 0, 4, "w_flash_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_ec_flash), nullptr, 0, 2, "w_flash"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_ec_shift), nullptr, 0, 4, "w_shift"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_ec_shift_off), nullptr, 0, 4, "w_shift_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 32), GlueKind::w, AMOS_BIND_W(w_ec_fade), nullptr, 0, 4, "w_fade"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_ec_fade_off), nullptr, 0, 4, "w_fade_off"},
    {AMOS_ENTRY_AT(host_obj_src_w_colours_glue_s, 40), GlueKind::w, AMOS_BIND_W(w_ec_fade_stop), nullptr, 0, 4, "w_fade_stop"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
