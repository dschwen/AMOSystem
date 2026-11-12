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
uint64_t amos_typed_amcaf_aga_detect(const uint32_t* words) { return typed_library_call<&amcaf_aga_detect, 0, 4>(words); }
uint64_t amos_typed_amcaf_alloc_code_bank(const uint32_t* words) { return typed_library_call<&amcaf_alloc_code_bank, 2, 0>(words); }
uint64_t amos_typed_amcaf_alloc_trans_map(const uint32_t* words) { return typed_library_call<&amcaf_alloc_trans_map, 3, 0>(words); }
uint64_t amos_typed_amcaf_alloc_trans_source(const uint32_t* words) { return typed_library_call<&amcaf_alloc_trans_source, 1, 0>(words); }
uint64_t amos_typed_amcaf_amos_cli(const uint32_t* words) { return typed_library_call<&amcaf_amos_cli, 0, 4>(words); }
uint64_t amos_typed_amcaf_amos_task(const uint32_t* words) { return typed_library_call<&amcaf_amos_task, 0, 4>(words); }
uint64_t amos_typed_amcaf_audio_free(const uint32_t* words) { return typed_library_call<&amcaf_audio_free, 0, 0>(words); }
uint64_t amos_typed_amcaf_bcircle(const uint32_t* words) { return typed_library_call<&amcaf_bcircle, 4, 0>(words); }
uint64_t amos_typed_amcaf_bzoom(const uint32_t* words) { return typed_library_call<&amcaf_bzoom, 9, 0>(words); }
uint64_t amos_typed_amcaf_c2p_convert(const uint32_t* words) { return typed_library_call<&amcaf_c2p_convert, 6, 0>(words); }
uint64_t amos_typed_amcaf_c2p_fire(const uint32_t* words) { return typed_library_call<&amcaf_c2p_fire, 5, 0>(words); }
uint64_t amos_typed_amcaf_c2p_shift(const uint32_t* words) { return typed_library_call<&amcaf_c2p_shift, 5, 0>(words); }
uint64_t amos_typed_amcaf_cd_date_s(const uint32_t* words) { return typed_library_call<&amcaf_cd_date_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_cd_day(const uint32_t* words) { return typed_library_call<&amcaf_cd_day, 1, 4>(words); }
uint64_t amos_typed_amcaf_cd_month(const uint32_t* words) { return typed_library_call<&amcaf_cd_month, 1, 4>(words); }
uint64_t amos_typed_amcaf_cd_string(const uint32_t* words) { return typed_library_call<&amcaf_cd_string, 1, 4>(words); }
uint64_t amos_typed_amcaf_cd_weekday(const uint32_t* words) { return typed_library_call<&amcaf_cd_weekday, 1, 4>(words); }
uint64_t amos_typed_amcaf_cd_year(const uint32_t* words) { return typed_library_call<&amcaf_cd_year, 1, 4>(words); }
uint64_t amos_typed_amcaf_change_bank_font(const uint32_t* words) { return typed_library_call<&amcaf_change_bank_font, 1, 0>(words); }
uint64_t amos_typed_amcaf_change_font_1(const uint32_t* words) { return typed_library_call<&amcaf_change_font_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_change_font_2(const uint32_t* words) { return typed_library_call<&amcaf_change_font_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_change_font_3(const uint32_t* words) { return typed_library_call<&amcaf_change_font_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_change_print_font(const uint32_t* words) { return typed_library_call<&amcaf_change_print_font, 1, 0>(words); }
uint64_t amos_typed_amcaf_command_name_s(const uint32_t* words) { return typed_library_call<&amcaf_command_name_s, 0, 4>(words); }
uint64_t amos_typed_amcaf_convert_grey(const uint32_t* words) { return typed_library_call<&amcaf_convert_grey, 2, 0>(words); }
uint64_t amos_typed_amcaf_coords_bank_1(const uint32_t* words) { return typed_library_call<&amcaf_coords_bank_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_coords_bank_2(const uint32_t* words) { return typed_library_call<&amcaf_coords_bank_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_coords_read(const uint32_t* words) { return typed_library_call<&amcaf_coords_read, 8, 0>(words); }
uint64_t amos_typed_amcaf_cop_pos(const uint32_t* words) { return typed_library_call<&amcaf_cop_pos, 0, 4>(words); }
uint64_t amos_typed_amcaf_count_pixels(const uint32_t* words) { return typed_library_call<&amcaf_count_pixels, 6, 4>(words); }
uint64_t amos_typed_amcaf_cpu(const uint32_t* words) { return typed_library_call<&amcaf_cpu, 0, 4>(words); }
uint64_t amos_typed_amcaf_ct_hour(const uint32_t* words) { return typed_library_call<&amcaf_ct_hour, 1, 4>(words); }
uint64_t amos_typed_amcaf_ct_minute(const uint32_t* words) { return typed_library_call<&amcaf_ct_minute, 1, 4>(words); }
uint64_t amos_typed_amcaf_ct_second(const uint32_t* words) { return typed_library_call<&amcaf_ct_second, 1, 4>(words); }
uint64_t amos_typed_amcaf_ct_string(const uint32_t* words) { return typed_library_call<&amcaf_ct_string, 1, 4>(words); }
uint64_t amos_typed_amcaf_ct_tick(const uint32_t* words) { return typed_library_call<&amcaf_ct_tick, 1, 4>(words); }
uint64_t amos_typed_amcaf_ct_time_s(const uint32_t* words) { return typed_library_call<&amcaf_ct_time_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_current_date(const uint32_t* words) { return typed_library_call<&amcaf_current_date, 0, 4>(words); }
uint64_t amos_typed_amcaf_current_time(const uint32_t* words) { return typed_library_call<&amcaf_current_time, 0, 4>(words); }
uint64_t amos_typed_amcaf_disk_state(const uint32_t* words) { return typed_library_call<&amcaf_disk_state, 1, 4>(words); }
uint64_t amos_typed_amcaf_disk_type(const uint32_t* words) { return typed_library_call<&amcaf_disk_type, 1, 4>(words); }
uint64_t amos_typed_amcaf_dload(const uint32_t* words) { return typed_library_call<&amcaf_dload, 2, 0>(words); }
uint64_t amos_typed_amcaf_dos_hash(const uint32_t* words) { return typed_library_call<&amcaf_dos_hash, 1, 4>(words); }
uint64_t amos_typed_amcaf_examine_dir(const uint32_t* words) { return typed_library_call<&amcaf_examine_dir, 1, 0>(words); }
uint64_t amos_typed_amcaf_examine_next_s(const uint32_t* words) { return typed_library_call<&amcaf_examine_next_s, 0, 4>(words); }
uint64_t amos_typed_amcaf_examine_object(const uint32_t* words) { return typed_library_call<&amcaf_examine_object, 1, 0>(words); }
uint64_t amos_typed_amcaf_examine_stop(const uint32_t* words) { return typed_library_call<&amcaf_examine_stop, 0, 0>(words); }
uint64_t amos_typed_amcaf_exchange_bob(const uint32_t* words) { return typed_library_call<&amcaf_exchange_bob, 2, 0>(words); }
uint64_t amos_typed_amcaf_exchange_icon(const uint32_t* words) { return typed_library_call<&amcaf_exchange_icon, 2, 0>(words); }
uint64_t amos_typed_amcaf_extdefault(const uint32_t* words) { return typed_library_call<&amcaf_extdefault, 1, 0>(words); }
uint64_t amos_typed_amcaf_extpath_s(const uint32_t* words) { return typed_library_call<&amcaf_extpath_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_extreinit(const uint32_t* words) { return typed_library_call<&amcaf_extreinit, 1, 0>(words); }
uint64_t amos_typed_amcaf_extremove(const uint32_t* words) { return typed_library_call<&amcaf_extremove, 1, 0>(words); }
uint64_t amos_typed_amcaf_fcircle(const uint32_t* words) { return typed_library_call<&amcaf_fcircle, 3, 0>(words); }
uint64_t amos_typed_amcaf_fellipse(const uint32_t* words) { return typed_library_call<&amcaf_fellipse, 4, 0>(words); }
uint64_t amos_typed_amcaf_file_copy(const uint32_t* words) { return typed_library_call<&amcaf_file_copy, 2, 0>(words); }
uint64_t amos_typed_amcaf_filename_s(const uint32_t* words) { return typed_library_call<&amcaf_filename_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_font_style(const uint32_t* words) { return typed_library_call<&amcaf_font_style, 0, 4>(words); }
uint64_t amos_typed_amcaf_fpu(const uint32_t* words) { return typed_library_call<&amcaf_fpu, 0, 4>(words); }
uint64_t amos_typed_amcaf_ham_fade_out(const uint32_t* words) { return typed_library_call<&amcaf_ham_fade_out, 1, 0>(words); }
uint64_t amos_typed_amcaf_ham_point(const uint32_t* words) { return typed_library_call<&amcaf_ham_point, 2, 4>(words); }
uint64_t amos_typed_amcaf_imploder_load(const uint32_t* words) { return typed_library_call<&amcaf_imploder_load, 2, 0>(words); }
uint64_t amos_typed_amcaf_imploder_unpack(const uint32_t* words) { return typed_library_call<&amcaf_imploder_unpack, 2, 0>(words); }
uint64_t amos_typed_amcaf_io_error(const uint32_t* words) { return typed_library_call<&amcaf_io_error, 0, 4>(words); }
uint64_t amos_typed_amcaf_io_error_s(const uint32_t* words) { return typed_library_call<&amcaf_io_error_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_launch_1(const uint32_t* words) { return typed_library_call<&amcaf_launch_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_launch_2(const uint32_t* words) { return typed_library_call<&amcaf_launch_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_make_bank_font(const uint32_t* words) { return typed_library_call<&amcaf_make_bank_font, 1, 0>(words); }
uint64_t amos_typed_amcaf_make_pix_mask(const uint32_t* words) { return typed_library_call<&amcaf_make_pix_mask, 6, 0>(words); }
uint64_t amos_typed_amcaf_mask_copy_10(const uint32_t* words) { return typed_library_call<&amcaf_mask_copy_10, 10, 0>(words); }
uint64_t amos_typed_amcaf_mask_copy_3(const uint32_t* words) { return typed_library_call<&amcaf_mask_copy_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_mask_copy_9(const uint32_t* words) { return typed_library_call<&amcaf_mask_copy_9, 9, 0>(words); }
uint64_t amos_typed_amcaf_object_blocks_0(const uint32_t* words) { return typed_library_call<&amcaf_object_blocks_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_blocks_1(const uint32_t* words) { return typed_library_call<&amcaf_object_blocks_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_comment_s_0(const uint32_t* words) { return typed_library_call<&amcaf_object_comment_s_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_comment_s_1(const uint32_t* words) { return typed_library_call<&amcaf_object_comment_s_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_date_0(const uint32_t* words) { return typed_library_call<&amcaf_object_date_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_date_1(const uint32_t* words) { return typed_library_call<&amcaf_object_date_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_name_s_0(const uint32_t* words) { return typed_library_call<&amcaf_object_name_s_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_name_s_1(const uint32_t* words) { return typed_library_call<&amcaf_object_name_s_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_protection_0(const uint32_t* words) { return typed_library_call<&amcaf_object_protection_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_protection_1(const uint32_t* words) { return typed_library_call<&amcaf_object_protection_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_protection_s(const uint32_t* words) { return typed_library_call<&amcaf_object_protection_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_size_0(const uint32_t* words) { return typed_library_call<&amcaf_object_size_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_size_1(const uint32_t* words) { return typed_library_call<&amcaf_object_size_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_time_0(const uint32_t* words) { return typed_library_call<&amcaf_object_time_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_time_1(const uint32_t* words) { return typed_library_call<&amcaf_object_time_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_object_type_0(const uint32_t* words) { return typed_library_call<&amcaf_object_type_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_object_type_1(const uint32_t* words) { return typed_library_call<&amcaf_object_type_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_open_workbench(const uint32_t* words) { return typed_library_call<&amcaf_open_workbench, 0, 0>(words); }
uint64_t amos_typed_amcaf_pal_get(const uint32_t* words) { return typed_library_call<&amcaf_pal_get, 2, 4>(words); }
uint64_t amos_typed_amcaf_pal_get_screen(const uint32_t* words) { return typed_library_call<&amcaf_pal_get_screen, 2, 0>(words); }
uint64_t amos_typed_amcaf_pal_set(const uint32_t* words) { return typed_library_call<&amcaf_pal_set, 3, 0>(words); }
uint64_t amos_typed_amcaf_pal_set_screen(const uint32_t* words) { return typed_library_call<&amcaf_pal_set_screen, 2, 0>(words); }
uint64_t amos_typed_amcaf_pal_spread(const uint32_t* words) { return typed_library_call<&amcaf_pal_spread, 4, 0>(words); }
uint64_t amos_typed_amcaf_paste_ptile(const uint32_t* words) { return typed_library_call<&amcaf_paste_ptile, 3, 0>(words); }
uint64_t amos_typed_amcaf_path_s(const uint32_t* words) { return typed_library_call<&amcaf_path_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_pattern_match(const uint32_t* words) { return typed_library_call<&amcaf_pattern_match, 2, 4>(words); }
uint64_t amos_typed_amcaf_pix_brighten_7(const uint32_t* words) { return typed_library_call<&amcaf_pix_brighten_7, 7, 0>(words); }
uint64_t amos_typed_amcaf_pix_brighten_8(const uint32_t* words) { return typed_library_call<&amcaf_pix_brighten_8, 8, 0>(words); }
uint64_t amos_typed_amcaf_pix_darken_7(const uint32_t* words) { return typed_library_call<&amcaf_pix_darken_7, 7, 0>(words); }
uint64_t amos_typed_amcaf_pix_darken_8(const uint32_t* words) { return typed_library_call<&amcaf_pix_darken_8, 8, 0>(words); }
uint64_t amos_typed_amcaf_pix_shift_down_7(const uint32_t* words) { return typed_library_call<&amcaf_pix_shift_down_7, 7, 0>(words); }
uint64_t amos_typed_amcaf_pix_shift_down_8(const uint32_t* words) { return typed_library_call<&amcaf_pix_shift_down_8, 8, 0>(words); }
uint64_t amos_typed_amcaf_pix_shift_up_7(const uint32_t* words) { return typed_library_call<&amcaf_pix_shift_up_7, 7, 0>(words); }
uint64_t amos_typed_amcaf_pix_shift_up_8(const uint32_t* words) { return typed_library_call<&amcaf_pix_shift_up_8, 8, 0>(words); }
uint64_t amos_typed_amcaf_ppfromdisk(const uint32_t* words) { return typed_library_call<&amcaf_ppfromdisk, 2, 0>(words); }
uint64_t amos_typed_amcaf_pptodisk_2(const uint32_t* words) { return typed_library_call<&amcaf_pptodisk_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_pptodisk_3(const uint32_t* words) { return typed_library_call<&amcaf_pptodisk_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_ppunpack(const uint32_t* words) { return typed_library_call<&amcaf_ppunpack, 2, 0>(words); }
uint64_t amos_typed_amcaf_protect_object(const uint32_t* words) { return typed_library_call<&amcaf_protect_object, 2, 0>(words); }
uint64_t amos_typed_amcaf_ptile_bank(const uint32_t* words) { return typed_library_call<&amcaf_ptile_bank, 1, 0>(words); }
uint64_t amos_typed_amcaf_rain_fade_2a(const uint32_t* words) { return typed_library_call<&amcaf_rain_fade_2a, 2, 0>(words); }
uint64_t amos_typed_amcaf_rain_fade_2b(const uint32_t* words) { return typed_library_call<&amcaf_rain_fade_2b, 2, 0>(words); }
uint64_t amos_typed_amcaf_reset_computer(const uint32_t* words) { return typed_library_call<&amcaf_reset_computer, 0, 0>(words); }
uint64_t amos_typed_amcaf_rnc_unpack(const uint32_t* words) { return typed_library_call<&amcaf_rnc_unpack, 2, 0>(words); }
uint64_t amos_typed_amcaf_scrn_bitmap(const uint32_t* words) { return typed_library_call<&amcaf_scrn_bitmap, 0, 4>(words); }
uint64_t amos_typed_amcaf_scrn_layer(const uint32_t* words) { return typed_library_call<&amcaf_scrn_layer, 0, 4>(words); }
uint64_t amos_typed_amcaf_scrn_layerinfo(const uint32_t* words) { return typed_library_call<&amcaf_scrn_layerinfo, 0, 4>(words); }
uint64_t amos_typed_amcaf_scrn_rastport(const uint32_t* words) { return typed_library_call<&amcaf_scrn_rastport, 0, 4>(words); }
uint64_t amos_typed_amcaf_scrn_region(const uint32_t* words) { return typed_library_call<&amcaf_scrn_region, 0, 4>(words); }
uint64_t amos_typed_amcaf_set_ntsc(const uint32_t* words) { return typed_library_call<&amcaf_set_ntsc, 0, 0>(words); }
uint64_t amos_typed_amcaf_set_object_comment(const uint32_t* words) { return typed_library_call<&amcaf_set_object_comment, 2, 0>(words); }
uint64_t amos_typed_amcaf_set_object_date(const uint32_t* words) { return typed_library_call<&amcaf_set_object_date, 3, 0>(words); }
uint64_t amos_typed_amcaf_set_pal(const uint32_t* words) { return typed_library_call<&amcaf_set_pal, 0, 0>(words); }
uint64_t amos_typed_amcaf_set_rain_colour(const uint32_t* words) { return typed_library_call<&amcaf_set_rain_colour, 2, 0>(words); }
uint64_t amos_typed_amcaf_set_sprite_priority(const uint32_t* words) { return typed_library_call<&amcaf_set_sprite_priority, 1, 0>(words); }
uint64_t amos_typed_amcaf_set_trans_map(const uint32_t* words) { return typed_library_call<&amcaf_set_trans_map, 3, 0>(words); }
uint64_t amos_typed_amcaf_set_trans_source(const uint32_t* words) { return typed_library_call<&amcaf_set_trans_source, 1, 0>(words); }
uint64_t amos_typed_amcaf_shade_bob_down(const uint32_t* words) { return typed_library_call<&amcaf_shade_bob_down, 4, 0>(words); }
uint64_t amos_typed_amcaf_shade_bob_mask(const uint32_t* words) { return typed_library_call<&amcaf_shade_bob_mask, 1, 0>(words); }
uint64_t amos_typed_amcaf_shade_bob_planes(const uint32_t* words) { return typed_library_call<&amcaf_shade_bob_planes, 1, 0>(words); }
uint64_t amos_typed_amcaf_shade_bob_up(const uint32_t* words) { return typed_library_call<&amcaf_shade_bob_up, 4, 0>(words); }
uint64_t amos_typed_amcaf_shade_pix_2(const uint32_t* words) { return typed_library_call<&amcaf_shade_pix_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_shade_pix_3(const uint32_t* words) { return typed_library_call<&amcaf_shade_pix_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_sload(const uint32_t* words) { return typed_library_call<&amcaf_sload, 3, 0>(words); }
uint64_t amos_typed_amcaf_splinters_active(const uint32_t* words) { return typed_library_call<&amcaf_splinters_active, 0, 4>(words); }
uint64_t amos_typed_amcaf_splinters_back(const uint32_t* words) { return typed_library_call<&amcaf_splinters_back, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_bank(const uint32_t* words) { return typed_library_call<&amcaf_splinters_bank, 2, 0>(words); }
uint64_t amos_typed_amcaf_splinters_colour(const uint32_t* words) { return typed_library_call<&amcaf_splinters_colour, 2, 0>(words); }
uint64_t amos_typed_amcaf_splinters_double_del(const uint32_t* words) { return typed_library_call<&amcaf_splinters_double_del, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_double_do(const uint32_t* words) { return typed_library_call<&amcaf_splinters_double_do, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_draw(const uint32_t* words) { return typed_library_call<&amcaf_splinters_draw, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_fuel(const uint32_t* words) { return typed_library_call<&amcaf_splinters_fuel, 1, 0>(words); }
uint64_t amos_typed_amcaf_splinters_gravity(const uint32_t* words) { return typed_library_call<&amcaf_splinters_gravity, 2, 0>(words); }
uint64_t amos_typed_amcaf_splinters_init(const uint32_t* words) { return typed_library_call<&amcaf_splinters_init, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_limit_0(const uint32_t* words) { return typed_library_call<&amcaf_splinters_limit_0, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_limit_4(const uint32_t* words) { return typed_library_call<&amcaf_splinters_limit_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_splinters_max(const uint32_t* words) { return typed_library_call<&amcaf_splinters_max, 1, 0>(words); }
uint64_t amos_typed_amcaf_splinters_move(const uint32_t* words) { return typed_library_call<&amcaf_splinters_move, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_single_del(const uint32_t* words) { return typed_library_call<&amcaf_splinters_single_del, 0, 0>(words); }
uint64_t amos_typed_amcaf_splinters_single_do(const uint32_t* words) { return typed_library_call<&amcaf_splinters_single_do, 0, 0>(words); }
uint64_t amos_typed_amcaf_ssave(const uint32_t* words) { return typed_library_call<&amcaf_ssave, 3, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_accelerate_off(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_accelerate_off, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_accelerate_on(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_accelerate_on, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_bank(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_bank, 2, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_double_del(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_double_del, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_double_do(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_double_do, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_draw(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_draw, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_gravity(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_gravity, 2, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_init(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_init, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_limit_0(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_limit_0, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_limit_4(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_limit_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_move_0(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_move_0, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_move_1(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_move_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_origin(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_origin, 2, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_planes(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_planes, 2, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_single_del(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_single_del, 0, 0>(words); }
uint64_t amos_typed_amcaf_td_stars_single_do(const uint32_t* words) { return typed_library_call<&amcaf_td_stars_single_do, 0, 0>(words); }
uint64_t amos_typed_amcaf_tool_types_s(const uint32_t* words) { return typed_library_call<&amcaf_tool_types_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_trans_screen_dynamic(const uint32_t* words) { return typed_library_call<&amcaf_trans_screen_dynamic, 4, 0>(words); }
uint64_t amos_typed_amcaf_trans_screen_runtime(const uint32_t* words) { return typed_library_call<&amcaf_trans_screen_runtime, 4, 0>(words); }
uint64_t amos_typed_amcaf_trans_screen_static(const uint32_t* words) { return typed_library_call<&amcaf_trans_screen_static, 4, 0>(words); }
uint64_t amos_typed_amcaf_turbo_draw_5(const uint32_t* words) { return typed_library_call<&amcaf_turbo_draw_5, 5, 0>(words); }
uint64_t amos_typed_amcaf_turbo_draw_6(const uint32_t* words) { return typed_library_call<&amcaf_turbo_draw_6, 6, 0>(words); }
uint64_t amos_typed_amcaf_turbo_text_3(const uint32_t* words) { return typed_library_call<&amcaf_turbo_text_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_turbo_text_4(const uint32_t* words) { return typed_library_call<&amcaf_turbo_text_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_turbo_text_5(const uint32_t* words) { return typed_library_call<&amcaf_turbo_text_5, 5, 0>(words); }
uint64_t amos_typed_amcaf_vec_rot_angles(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_angles, 3, 0>(words); }
uint64_t amos_typed_amcaf_vec_rot_pos(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_pos, 3, 0>(words); }
uint64_t amos_typed_amcaf_vec_rot_precalc(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_precalc, 0, 0>(words); }
uint64_t amos_typed_amcaf_vec_rot_x_0(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_x_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_vec_rot_x_3(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_x_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_vec_rot_y_0(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_y_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_vec_rot_y_3(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_y_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_vec_rot_z_0(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_z_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_vec_rot_z_3(const uint32_t* words) { return typed_library_call<&amcaf_vec_rot_z_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_wload(const uint32_t* words) { return typed_library_call<&amcaf_wload, 2, 0>(words); }
uint64_t amos_typed_amcaf_write_cli(const uint32_t* words) { return typed_library_call<&amcaf_write_cli, 1, 0>(words); }
uint64_t amos_typed_amcaf_wsave(const uint32_t* words) { return typed_library_call<&amcaf_wsave, 2, 0>(words); }
uint64_t amos_typed_amcaf_x_raster(const uint32_t* words) { return typed_library_call<&amcaf_x_raster, 0, 4>(words); }
uint64_t amos_typed_amcaf_xfire(const uint32_t* words) { return typed_library_call<&amcaf_xfire, 2, 4>(words); }
uint64_t amos_typed_amcaf_y_raster(const uint32_t* words) { return typed_library_call<&amcaf_y_raster, 0, 4>(words); }
}
}
