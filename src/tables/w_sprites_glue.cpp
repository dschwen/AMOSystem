// src_w_sprites_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_sprites_glue_s[52] = {
};
}

namespace amos::host {

void relocate_src_w_sprites_glue_s()
{
}

const GlueEntry entries_src_w_sprites_glue_s[] = {
    {host_obj_src_w_sprites_glue_s + 0, GlueKind::w, AMOS_BIND_W(w_sy_sprite_set), nullptr, 0, 4, "w_sprite_set"},
    {host_obj_src_w_sprites_glue_s + 4, GlueKind::w, AMOS_BIND_W(w_sy_sprite_unset), nullptr, 0, 4, "w_sprite_unset"},
    {host_obj_src_w_sprites_glue_s + 8, GlueKind::w, AMOS_BIND_W(w_sy_sprites_show), nullptr, 0, 4, "w_sprites_show"},
    {host_obj_src_w_sprites_glue_s + 12, GlueKind::w, AMOS_BIND_W(w_sy_sprites_update), nullptr, 0, 4, "w_sprites_update"},
    {host_obj_src_w_sprites_glue_s + 16, GlueKind::w, AMOS_BIND_W(w_sy_sprite_bank), nullptr, 0, 4, "w_sprite_bank"},
    {host_obj_src_w_sprites_glue_s + 20, GlueKind::w, AMOS_BIND_W(w_sy_sprite), nullptr, 0, 4, "w_sprite"},
    {host_obj_src_w_sprites_glue_s + 24, GlueKind::w, AMOS_BIND_W(w_sy_sprite_off), nullptr, 0, 4, "w_sprite_off"},
    {host_obj_src_w_sprites_glue_s + 28, GlueKind::w, AMOS_BIND_W(w_sy_sprites_off), nullptr, 0, 4, "w_sprites_off"},
    {host_obj_src_w_sprites_glue_s + 32, GlueKind::w, AMOS_BIND_W(w_sy_sprite_xy), nullptr, 0, 4, "w_sprite_xy"},
    {host_obj_src_w_sprites_glue_s + 36, GlueKind::w, AMOS_BIND_W(w_sy_sprites_stop), nullptr, 0, 4, "w_sprites_stop"},
    {host_obj_src_w_sprites_glue_s + 40, GlueKind::w, AMOS_BIND_W(w_sy_sprites_restart), nullptr, 0, 4, "w_sprites_restart"},
    {host_obj_src_w_sprites_glue_s + 44, GlueKind::w, AMOS_BIND_W(w_sy_sprite_priority), nullptr, 0, 4, "w_sprite_priority"},
    {host_obj_src_w_sprites_glue_s + 48, GlueKind::w, AMOS_BIND_W(w_sy_sprite_buffer), nullptr, 0, 4, "w_sprite_buffer"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
