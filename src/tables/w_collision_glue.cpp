// src_w_collision_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_collision_glue_s[12] = {
};
}

namespace amos::host {

void relocate_src_w_collision_glue_s()
{
}

const GlueEntry entries_src_w_collision_glue_s[] = {
    {host_obj_src_w_collision_glue_s + 0, GlueKind::w, AMOS_BIND_W(w_sy_bob_collisions), nullptr, 0, 4, "w_bob_col"},
    {host_obj_src_w_collision_glue_s + 4, GlueKind::w, AMOS_BIND_W(w_sy_sprite_collisions), nullptr, 0, 4, "w_sprite_col"},
    {host_obj_src_w_collision_glue_s + 8, GlueKind::hand, AMOS_BIND_HAND(host_w_get_col), nullptr, 0, 0, "w_get_col"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
