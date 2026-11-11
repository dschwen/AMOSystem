// Generated in the defining TU: the compiler sees the real signature.
#include "runtime/typed_call.h"
#include "interp/interp.h"
namespace amos {
namespace boot {}
namespace dbl {}
namespace dev {}
namespace ed {}
namespace ffp {}
namespace host {}
namespace interp {}
namespace menu {}
namespace monitor {}
namespace music {}
namespace verif {}
namespace w {}
}
namespace amos::host {
using namespace ::amos;
using namespace ::amos::boot;
using namespace ::amos::dbl;
using namespace ::amos::dev;
using namespace ::amos::ed;
using namespace ::amos::ffp;
using namespace ::amos::interp;
using namespace ::amos::menu;
using namespace ::amos::monitor;
using namespace ::amos::music;
using namespace ::amos::verif;
using namespace ::amos::w;
extern "C" {
uint64_t amos_typed_turbo_blit_clear(const uint32_t* words) { return typed_library_call<&turbo_blit_clear, 1, 0>(words); }
uint64_t amos_typed_turbo_blit_erase(const uint32_t* words) { return typed_library_call<&turbo_blit_erase, 1, 0>(words); }
uint64_t amos_typed_turbo_blit_int_change(const uint32_t* words) { return typed_library_call<&turbo_blit_int_change, 2, 0>(words); }
uint64_t amos_typed_turbo_blit_int_off(const uint32_t* words) { return typed_library_call<&turbo_blit_int_off, 0, 0>(words); }
uint64_t amos_typed_turbo_blit_int_on(const uint32_t* words) { return typed_library_call<&turbo_blit_int_on, 2, 0>(words); }
uint64_t amos_typed_turbo_blit_int_wait(const uint32_t* words) { return typed_library_call<&turbo_blit_int_wait, 1, 0>(words); }
uint64_t amos_typed_turbo_blit_left(const uint32_t* words) { return typed_library_call<&turbo_blit_left, 6, 0>(words); }
uint64_t amos_typed_turbo_blit_speed(const uint32_t* words) { return typed_library_call<&turbo_blit_speed, 2, 0>(words); }
uint64_t amos_typed_turbo_blit_store_left(const uint32_t* words) { return typed_library_call<&turbo_blit_store_left, 7, 0>(words); }
uint64_t amos_typed_turbo_blit_store_up(const uint32_t* words) { return typed_library_call<&turbo_blit_store_up, 7, 0>(words); }
uint64_t amos_typed_turbo_blit_up(const uint32_t* words) { return typed_library_call<&turbo_blit_up, 6, 0>(words); }
uint64_t amos_typed_turbo_build_static_block(const uint32_t* words) { return typed_library_call<&turbo_build_static_block, 0, 0>(words); }
uint64_t amos_typed_turbo_check(const uint32_t* words) { return typed_library_call<&turbo_check, 4, 4>(words); }
uint64_t amos_typed_turbo_check_erase(const uint32_t* words) { return typed_library_call<&turbo_check_erase, 0, 0>(words); }
uint64_t amos_typed_turbo_chip_largest(const uint32_t* words) { return typed_library_call<&turbo_chip_largest, 0, 4>(words); }
uint64_t amos_typed_turbo_cpu_info(const uint32_t* words) { return typed_library_call<&turbo_cpu_info, 0, 4>(words); }
uint64_t amos_typed_turbo_debug(const uint32_t* words) { return typed_library_call<&turbo_debug, 0, 0>(words); }
uint64_t amos_typed_turbo_define_attr(const uint32_t* words) { return typed_library_call<&turbo_define_attr, 4, 0>(words); }
uint64_t amos_typed_turbo_define_draw(const uint32_t* words) { return typed_library_call<&turbo_define_draw, 4, 0>(words); }
uint64_t amos_typed_turbo_define_move(const uint32_t* words) { return typed_library_call<&turbo_define_move, 4, 0>(words); }
uint64_t amos_typed_turbo_define_star(const uint32_t* words) { return typed_library_call<&turbo_define_star, 5, 0>(words); }
uint64_t amos_typed_turbo_define_stop(const uint32_t* words) { return typed_library_call<&turbo_define_stop, 2, 0>(words); }
uint64_t amos_typed_turbo_display_stars(const uint32_t* words) { return typed_library_call<&turbo_display_stars, 0, 0>(words); }
uint64_t amos_typed_turbo_eye_3d(const uint32_t* words) { return typed_library_call<&turbo_eye_3d, 2, 0>(words); }
uint64_t amos_typed_turbo_f_16_icon(const uint32_t* words) { return typed_library_call<&turbo_f_16_icon, 3, 0>(words); }
uint64_t amos_typed_turbo_f_16proc_icon(const uint32_t* words) { return typed_library_call<&turbo_f_16proc_icon, 3, 0>(words); }
uint64_t amos_typed_turbo_f_32_icon(const uint32_t* words) { return typed_library_call<&turbo_f_32_icon, 3, 0>(words); }
uint64_t amos_typed_turbo_f_32proc_icon(const uint32_t* words) { return typed_library_call<&turbo_f_32proc_icon, 3, 0>(words); }
uint64_t amos_typed_turbo_f_circle(const uint32_t* words) { return typed_library_call<&turbo_f_circle, 4, 0>(words); }
uint64_t amos_typed_turbo_f_draw_2(const uint32_t* words) { return typed_library_call<&turbo_f_draw_2, 2, 0>(words); }
uint64_t amos_typed_turbo_f_draw_4(const uint32_t* words) { return typed_library_call<&turbo_f_draw_4, 4, 0>(words); }
uint64_t amos_typed_turbo_f_paste_icon(const uint32_t* words) { return typed_library_call<&turbo_f_paste_icon, 3, 0>(words); }
uint64_t amos_typed_turbo_f_put_block(const uint32_t* words) { return typed_library_call<&turbo_f_put_block, 3, 0>(words); }
uint64_t amos_typed_turbo_f_put_static_block(const uint32_t* words) { return typed_library_call<&turbo_f_put_static_block, 3, 0>(words); }
uint64_t amos_typed_turbo_fast_largest(const uint32_t* words) { return typed_library_call<&turbo_fast_largest, 0, 4>(words); }
uint64_t amos_typed_turbo_hit_bob_check(const uint32_t* words) { return typed_library_call<&turbo_hit_bob_check, 5, 4>(words); }
uint64_t amos_typed_turbo_hit_bob_zone(const uint32_t* words) { return typed_library_call<&turbo_hit_bob_zone, 3, 4>(words); }
uint64_t amos_typed_turbo_hit_spr_check(const uint32_t* words) { return typed_library_call<&turbo_hit_spr_check, 5, 4>(words); }
uint64_t amos_typed_turbo_hit_spr_zone(const uint32_t* words) { return typed_library_call<&turbo_hit_spr_zone, 3, 4>(words); }
uint64_t amos_typed_turbo_icon_check(const uint32_t* words) { return typed_library_call<&turbo_icon_check, 1, 4>(words); }
uint64_t amos_typed_turbo_is_raw_key(const uint32_t* words) { return typed_library_call<&turbo_is_raw_key, 0, 4>(words); }
uint64_t amos_typed_turbo_line_3d(const uint32_t* words) { return typed_library_call<&turbo_line_3d, 6, 0>(words); }
uint64_t amos_typed_turbo_math_info(const uint32_t* words) { return typed_library_call<&turbo_math_info, 0, 4>(words); }
uint64_t amos_typed_turbo_multi_bl_ended(const uint32_t* words) { return typed_library_call<&turbo_multi_bl_ended, 0, 4>(words); }
uint64_t amos_typed_turbo_multi_bl_error(const uint32_t* words) { return typed_library_call<&turbo_multi_bl_error, 0, 0>(words); }
uint64_t amos_typed_turbo_multi_blit(const uint32_t* words) { return typed_library_call<&turbo_multi_blit, 2, 0>(words); }
uint64_t amos_typed_turbo_multi_bload(const uint32_t* words) { return typed_library_call<&turbo_multi_bload, 3, 0>(words); }
uint64_t amos_typed_turbo_object_draw(const uint32_t* words) { return typed_library_call<&turbo_object_draw, 1, 0>(words); }
uint64_t amos_typed_turbo_object_erase(const uint32_t* words) { return typed_library_call<&turbo_object_erase, 1, 0>(words); }
uint64_t amos_typed_turbo_object_limit(const uint32_t* words) { return typed_library_call<&turbo_object_limit, 1, 0>(words); }
uint64_t amos_typed_turbo_object_load(const uint32_t* words) { return typed_library_call<&turbo_object_load, 2, 0>(words); }
uint64_t amos_typed_turbo_object_mag_draw(const uint32_t* words) { return typed_library_call<&turbo_object_mag_draw, 2, 0>(words); }
uint64_t amos_typed_turbo_object_save(const uint32_t* words) { return typed_library_call<&turbo_object_save, 3, 0>(words); }
uint64_t amos_typed_turbo_parse_s(const uint32_t* words) { return typed_library_call<&turbo_parse_s, 4, 4>(words); }
uint64_t amos_typed_turbo_plane_offset(const uint32_t* words) { return typed_library_call<&turbo_plane_offset, 4, 0>(words); }
uint64_t amos_typed_turbo_plane_shift_down(const uint32_t* words) { return typed_library_call<&turbo_plane_shift_down, 3, 0>(words); }
uint64_t amos_typed_turbo_plane_shift_up(const uint32_t* words) { return typed_library_call<&turbo_plane_shift_up, 3, 0>(words); }
uint64_t amos_typed_turbo_plane_swap(const uint32_t* words) { return typed_library_call<&turbo_plane_swap, 3, 0>(words); }
uint64_t amos_typed_turbo_plane_update(const uint32_t* words) { return typed_library_call<&turbo_plane_update, 1, 0>(words); }
uint64_t amos_typed_turbo_planes_icon(const uint32_t* words) { return typed_library_call<&turbo_planes_icon, 1, 4>(words); }
uint64_t amos_typed_turbo_r_bar(const uint32_t* words) { return typed_library_call<&turbo_r_bar, 2, 0>(words); }
uint64_t amos_typed_turbo_r_box(const uint32_t* words) { return typed_library_call<&turbo_r_box, 2, 0>(words); }
uint64_t amos_typed_turbo_r_draw(const uint32_t* words) { return typed_library_call<&turbo_r_draw, 2, 0>(words); }
uint64_t amos_typed_turbo_r_home(const uint32_t* words) { return typed_library_call<&turbo_r_home, 2, 0>(words); }
uint64_t amos_typed_turbo_r_move(const uint32_t* words) { return typed_library_call<&turbo_r_move, 2, 0>(words); }
uint64_t amos_typed_turbo_r_object_draw(const uint32_t* words) { return typed_library_call<&turbo_r_object_draw, 3, 0>(words); }
uint64_t amos_typed_turbo_r_object_mag_draw(const uint32_t* words) { return typed_library_call<&turbo_r_object_mag_draw, 4, 0>(words); }
uint64_t amos_typed_turbo_raw_key(const uint32_t* words) { return typed_library_call<&turbo_raw_key, 1, 4>(words); }
uint64_t amos_typed_turbo_reserve_check(const uint32_t* words) { return typed_library_call<&turbo_reserve_check, 1, 0>(words); }
uint64_t amos_typed_turbo_reserve_object(const uint32_t* words) { return typed_library_call<&turbo_reserve_object, 2, 0>(words); }
uint64_t amos_typed_turbo_reserve_scene(const uint32_t* words) { return typed_library_call<&turbo_reserve_scene, 3, 0>(words); }
uint64_t amos_typed_turbo_reserve_stars(const uint32_t* words) { return typed_library_call<&turbo_reserve_stars, 1, 0>(words); }
uint64_t amos_typed_turbo_reserve_static_block(const uint32_t* words) { return typed_library_call<&turbo_reserve_static_block, 1, 0>(words); }
uint64_t amos_typed_turbo_reset_check(const uint32_t* words) { return typed_library_call<&turbo_reset_check, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_16_bottom(const uint32_t* words) { return typed_library_call<&turbo_scene_16_bottom, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_16_change(const uint32_t* words) { return typed_library_call<&turbo_scene_16_change, 3, 0>(words); }
uint64_t amos_typed_turbo_scene_16_check(const uint32_t* words) { return typed_library_call<&turbo_scene_16_check, 2, 4>(words); }
uint64_t amos_typed_turbo_scene_16_def(const uint32_t* words) { return typed_library_call<&turbo_scene_16_def, 8, 0>(words); }
uint64_t amos_typed_turbo_scene_16_do(const uint32_t* words) { return typed_library_call<&turbo_scene_16_do, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_16_draw(const uint32_t* words) { return typed_library_call<&turbo_scene_16_draw, 6, 0>(words); }
uint64_t amos_typed_turbo_scene_16_left(const uint32_t* words) { return typed_library_call<&turbo_scene_16_left, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_16_limit(const uint32_t* words) { return typed_library_call<&turbo_scene_16_limit, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_16_restore(const uint32_t* words) { return typed_library_call<&turbo_scene_16_restore, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_16_right(const uint32_t* words) { return typed_library_call<&turbo_scene_16_right, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_16_top(const uint32_t* words) { return typed_library_call<&turbo_scene_16_top, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_16_view(const uint32_t* words) { return typed_library_call<&turbo_scene_16_view, 5, 0>(words); }
uint64_t amos_typed_turbo_scene_32_bottom(const uint32_t* words) { return typed_library_call<&turbo_scene_32_bottom, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_32_change(const uint32_t* words) { return typed_library_call<&turbo_scene_32_change, 3, 0>(words); }
uint64_t amos_typed_turbo_scene_32_check(const uint32_t* words) { return typed_library_call<&turbo_scene_32_check, 2, 4>(words); }
uint64_t amos_typed_turbo_scene_32_do(const uint32_t* words) { return typed_library_call<&turbo_scene_32_do, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_32_draw(const uint32_t* words) { return typed_library_call<&turbo_scene_32_draw, 6, 0>(words); }
uint64_t amos_typed_turbo_scene_32_left(const uint32_t* words) { return typed_library_call<&turbo_scene_32_left, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_32_right(const uint32_t* words) { return typed_library_call<&turbo_scene_32_right, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_32_top(const uint32_t* words) { return typed_library_call<&turbo_scene_32_top, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_32_view(const uint32_t* words) { return typed_library_call<&turbo_scene_32_view, 5, 0>(words); }
uint64_t amos_typed_turbo_scene_bank(const uint32_t* words) { return typed_library_call<&turbo_scene_bank, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_change(const uint32_t* words) { return typed_library_call<&turbo_scene_change, 3, 0>(words); }
uint64_t amos_typed_turbo_scene_check(const uint32_t* words) { return typed_library_call<&turbo_scene_check, 2, 4>(words); }
uint64_t amos_typed_turbo_scene_convert(const uint32_t* words) { return typed_library_call<&turbo_scene_convert, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_copy(const uint32_t* words) { return typed_library_call<&turbo_scene_copy, 8, 0>(words); }
uint64_t amos_typed_turbo_scene_fill(const uint32_t* words) { return typed_library_call<&turbo_scene_fill, 6, 0>(words); }
uint64_t amos_typed_turbo_scene_icon_bank(const uint32_t* words) { return typed_library_call<&turbo_scene_icon_bank, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_load(const uint32_t* words) { return typed_library_call<&turbo_scene_load, 2, 0>(words); }
uint64_t amos_typed_turbo_scene_mask_palette(const uint32_t* words) { return typed_library_call<&turbo_scene_mask_palette, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_palette(const uint32_t* words) { return typed_library_call<&turbo_scene_palette, 1, 0>(words); }
uint64_t amos_typed_turbo_scene_replace(const uint32_t* words) { return typed_library_call<&turbo_scene_replace, 7, 0>(words); }
uint64_t amos_typed_turbo_scene_scan_x(const uint32_t* words) { return typed_library_call<&turbo_scene_scan_x, 4, 4>(words); }
uint64_t amos_typed_turbo_scene_scan_y(const uint32_t* words) { return typed_library_call<&turbo_scene_scan_y, 4, 4>(words); }
uint64_t amos_typed_turbo_scene_x(const uint32_t* words) { return typed_library_call<&turbo_scene_x, 0, 4>(words); }
uint64_t amos_typed_turbo_scene_y(const uint32_t* words) { return typed_library_call<&turbo_scene_y, 0, 4>(words); }
uint64_t amos_typed_turbo_set_check(const uint32_t* words) { return typed_library_call<&turbo_set_check, 5, 0>(words); }
uint64_t amos_typed_turbo_stars_clip(const uint32_t* words) { return typed_library_call<&turbo_stars_clip, 4, 0>(words); }
uint64_t amos_typed_turbo_stars_compute(const uint32_t* words) { return typed_library_call<&turbo_stars_compute, 2, 0>(words); }
uint64_t amos_typed_turbo_stars_draw(const uint32_t* words) { return typed_library_call<&turbo_stars_draw, 0, 0>(words); }
uint64_t amos_typed_turbo_stars_erase(const uint32_t* words) { return typed_library_call<&turbo_stars_erase, 0, 0>(words); }
uint64_t amos_typed_turbo_stars_int_off(const uint32_t* words) { return typed_library_call<&turbo_stars_int_off, 0, 0>(words); }
uint64_t amos_typed_turbo_stars_int_on(const uint32_t* words) { return typed_library_call<&turbo_stars_int_on, 1, 0>(words); }
uint64_t amos_typed_turbo_stars_speed(const uint32_t* words) { return typed_library_call<&turbo_stars_speed, 4, 0>(words); }
uint64_t amos_typed_turbo_static_block_erase(const uint32_t* words) { return typed_library_call<&turbo_static_block_erase, 0, 0>(words); }
uint64_t amos_typed_turbo_workbench_open(const uint32_t* words) { return typed_library_call<&turbo_workbench_open, 0, 0>(words); }
uint64_t amos_typed_turbo_x_icon(const uint32_t* words) { return typed_library_call<&turbo_x_icon, 1, 4>(words); }
uint64_t amos_typed_turbo_y_icon(const uint32_t* words) { return typed_library_call<&turbo_y_icon, 1, 4>(words); }
}
}
