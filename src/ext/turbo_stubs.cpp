// The commands of Turbo Plus that are not implemented yet: written by tools/ext_gen.py from
// data/extensions/turbo.json (a command leaves this file when its "status" there says "implemented"
// and its "source" has it). Each raises an AMOS error.

#include "core/amos.h"
#include "ext/third_party.h"
#include "lib/strings.h"

using namespace amos;

extern "C" {

// reserve check (I0)
void turbo_reserve_check(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// check erase (I)
void turbo_check_erase() { ext3::unsupported(ext3::Id::turbo); }
// check (00t0,0,0)
int32_t turbo_check(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// set check (I0,0,0t0,0)
void turbo_set_check(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// reset check (I0)
void turbo_reset_check(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// hit spr zone (00,0,0)
int32_t turbo_hit_spr_zone(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// hit bob zone (00,0,0)
int32_t turbo_hit_bob_zone(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// hit spr check (00t0,0,0,0)
int32_t turbo_hit_spr_check(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// raw key (00)
int32_t turbo_raw_key(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r move (I0,0)
void turbo_r_move(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r draw (I0,0)
void turbo_r_draw(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r box (I0,0)
void turbo_r_box(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r home (I0,0)
void turbo_r_home(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r bar (I0,0)
void turbo_r_bar(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// reserve object (I0,0)
void turbo_reserve_object(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// define draw (I0,0t0,0)
void turbo_define_draw(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// define move (I0,0t0,0)
void turbo_define_move(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// define stop (I0,0)
void turbo_define_stop(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// define attr (I0,0t0,0)
void turbo_define_attr(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object draw (I0)
void turbo_object_draw(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r object draw (I0,0,0)
void turbo_r_object_draw(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object mag draw (I0,0)
void turbo_object_mag_draw(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// r object mag draw (I0,0,0,0)
void turbo_r_object_mag_draw(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object erase (I0)
void turbo_object_erase(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object save (I2,0t0)
void turbo_object_save(GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object load (I2,0)
void turbo_object_load(GuestString, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// line 3d (I0,0,0t0,0,0)
void turbo_line_3d(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// eye 3d (I0,0)
void turbo_eye_3d(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit store left (I0,0,0,0t0,0,0)
void turbo_blit_store_left(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// multi blit (I0t0)
void turbo_multi_blit(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit erase (I0)
void turbo_blit_erase(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit speed (I0,0)
void turbo_blit_speed(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit left (I0,0,0t0,0,0)
void turbo_blit_left(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit clear (I0)
void turbo_blit_clear(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// reserve stars (I0)
void turbo_reserve_stars(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// define star (I0,0,0,0,0)
void turbo_define_star(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// display stars (I)
void turbo_display_stars() { ext3::unsupported(ext3::Id::turbo); }
// stars erase (I)
void turbo_stars_erase() { ext3::unsupported(ext3::Id::turbo); }
// stars compute (I0t0)
void turbo_stars_compute(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// stars draw (I)
void turbo_stars_draw() { ext3::unsupported(ext3::Id::turbo); }
// stars speed (I0t0,0,0)
void turbo_stars_speed(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// stars clip (I0,0,0,0)
void turbo_stars_clip(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f circle (I0,0,0,0)
void turbo_f_circle(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// stars int on (I0)
void turbo_stars_int_on(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// stars int off (I)
void turbo_stars_int_off() { ext3::unsupported(ext3::Id::turbo); }
// blit int on (I0t0)
void turbo_blit_int_on(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit int off (I)
void turbo_blit_int_off() { ext3::unsupported(ext3::Id::turbo); }
// f draw (I0,0t0,0)
void turbo_f_draw_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// object limit (I0)
void turbo_object_limit(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f draw (I0,0)
void turbo_f_draw_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// plane offset (I0,0,0,0)
void turbo_plane_offset(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// plane swap (I0,0,0)
void turbo_plane_swap(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// plane shift up (I0,0t0)
void turbo_plane_shift_up(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// plane shift down (I0,0t0)
void turbo_plane_shift_down(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// plane update (I0)
void turbo_plane_update(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f paste icon (I0,0,0)
void turbo_f_paste_icon(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f 32 icon (I0,0,0)
void turbo_f_32_icon(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f 16 icon (I0,0,0)
void turbo_f_16_icon(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f 16proc icon (I0,0,0)
void turbo_f_16proc_icon(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// f 32proc icon (I0,0,0)
void turbo_f_32proc_icon(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// x icon (00)
int32_t turbo_x_icon(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// y icon (00)
int32_t turbo_y_icon(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// planes icon (00)
int32_t turbo_planes_icon(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// cpu info (0)
int32_t turbo_cpu_info() { ext3::unsupported(ext3::Id::turbo); }
// math info (0)
int32_t turbo_math_info() { ext3::unsupported(ext3::Id::turbo); }
// f put block (I0,0,0)
void turbo_f_put_block(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// reserve static block (I0)
void turbo_reserve_static_block(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// static block erase (I)
void turbo_static_block_erase() { ext3::unsupported(ext3::Id::turbo); }
// build static block (I)
void turbo_build_static_block() { ext3::unsupported(ext3::Id::turbo); }
// f put static block (I0,0,0)
void turbo_f_put_static_block(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene bank (I0)
void turbo_scene_bank(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene check (00,0)
int32_t turbo_scene_check(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 check (00,0)
int32_t turbo_scene_16_check(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene change (I0,0,0)
void turbo_scene_change(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 change (I0,0,0)
void turbo_scene_16_change(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 draw (I0,0,0,0,0,0)
void turbo_scene_16_draw(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 def (I0,0,0,0,0,0,0,0)
void turbo_scene_16_def(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 restore (I0)
void turbo_scene_16_restore(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 limit (I0)
void turbo_scene_16_limit(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 check (00,0)
int32_t turbo_scene_32_check(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 change (I0,0,0)
void turbo_scene_32_change(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 draw (I0,0,0,0,0,0)
void turbo_scene_32_draw(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 view (I0,0,0t0,0)
void turbo_scene_16_view(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 view (I0,0,0t0,0)
void turbo_scene_32_view(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 do (I0,0)
void turbo_scene_16_do(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 do (I0,0)
void turbo_scene_32_do(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 top (I0,0)
void turbo_scene_16_top(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 top (I0,0)
void turbo_scene_32_top(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 bottom (I0,0)
void turbo_scene_16_bottom(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 bottom (I0,0)
void turbo_scene_32_bottom(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 left (I0,0)
void turbo_scene_16_left(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 left (I0,0)
void turbo_scene_32_left(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 16 right (I0,0)
void turbo_scene_16_right(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene 32 right (I0,0)
void turbo_scene_32_right(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene x (0)
int32_t turbo_scene_x() { ext3::unsupported(ext3::Id::turbo); }
// scene y (0)
int32_t turbo_scene_y() { ext3::unsupported(ext3::Id::turbo); }
// debug (I)
void turbo_debug() { ext3::unsupported(ext3::Id::turbo); }
// hit bob check (00t0,0,0,0)
int32_t turbo_hit_bob_check(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// workbench open (I)
void turbo_workbench_open() { ext3::unsupported(ext3::Id::turbo); }
// scene load (I2,0)
void turbo_scene_load(GuestString, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit int change (I0t0)
void turbo_blit_int_change(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit int wait (I0)
void turbo_blit_int_wait(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit up (I0,0,0t0,0,0)
void turbo_blit_up(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// blit store up (I0,0,0,0t0,0,0)
void turbo_blit_store_up(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// icon check (00)
int32_t turbo_icon_check(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene icon bank (I0)
void turbo_scene_icon_bank(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene palette (I0)
void turbo_scene_palette(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene mask palette (I0)
void turbo_scene_mask_palette(int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene scan x (00,0,0,0)
int32_t turbo_scene_scan_x(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene scan y (00,0,0,0)
int32_t turbo_scene_scan_y(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene copy (I0,0,0,0,0t0,0,0)
void turbo_scene_copy(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene fill (I0,0,0,0,0,0)
void turbo_scene_fill(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// reserve scene (I0,0,0)
void turbo_reserve_scene(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene convert (I0t0)
void turbo_scene_convert(int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// scene replace (I0,0,0,0,0,0,0)
void turbo_scene_replace(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// chip largest (0)
int32_t turbo_chip_largest() { ext3::unsupported(ext3::Id::turbo); }
// fast largest (0)
int32_t turbo_fast_largest() { ext3::unsupported(ext3::Id::turbo); }
// is raw key (0)
int32_t turbo_is_raw_key() { ext3::unsupported(ext3::Id::turbo); }
// multi bload (I2,2,0)
void turbo_multi_bload(GuestString, GuestString, int32_t) { ext3::unsupported(ext3::Id::turbo); }
// multi bl error (I)
void turbo_multi_bl_error() { ext3::unsupported(ext3::Id::turbo); }
// multi bl ended (0)
int32_t turbo_multi_bl_ended() { ext3::unsupported(ext3::Id::turbo); }
// parse$ (02,0,2,0)
int32_t turbo_parse_s(GuestString, int32_t, GuestString, int32_t) { ext3::unsupported(ext3::Id::turbo); }

}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_turbo_stubs.inc"
#endif
