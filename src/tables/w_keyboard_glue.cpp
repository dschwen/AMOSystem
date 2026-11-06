// src_w_keyboard_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_keyboard_glue_s[44] = {
};
}

namespace amos::host {

void relocate_src_w_keyboard_glue_s()
{
}

const GlueEntry entries_src_w_keyboard_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 0), GlueKind::w, AMOS_BIND_W(w_sy_inkey), nullptr, 0, 4, "w_inkey"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 4), GlueKind::w, AMOS_BIND_W(w_sy_key_shift), nullptr, 0, 4, "w_key_shift"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 8), GlueKind::w, AMOS_BIND_W(w_sy_key_state), nullptr, 0, 4, "w_key_state"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 12), GlueKind::w, AMOS_BIND_W(w_sy_put_key), nullptr, 0, 4, "w_put_key"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 16), GlueKind::w, AMOS_BIND_W(w_sy_set_function_key), nullptr, 0, 4, "w_set_function_key"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 20), GlueKind::w, AMOS_BIND_W(w_sy_get_function_key), nullptr, 0, 4, "w_get_function_key"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 24), GlueKind::w, AMOS_BIND_W(w_sy_key_speed), nullptr, 0, 4, "w_key_speed"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 28), GlueKind::w, AMOS_BIND_W(w_sy_clear_key), nullptr, 0, 4, "w_clear_key"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 32), GlueKind::w_f, AMOS_BIND_W_RESULT(w_sy_key_waiting), nullptr, 0, 0, "w_key_waiting_entry"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 36), GlueKind::w, AMOS_BIND_W(w_sy_get_disc), nullptr, 0, 4, "w_get_disc_entry"},
    {AMOS_ENTRY_AT(host_obj_src_w_keyboard_glue_s, 40), GlueKind::w, AMOS_BIND_W(w_sy_nothing), nullptr, 0, 4, "w_nothing_entry"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
