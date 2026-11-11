// The commands of AMCAF that are not implemented yet: written by tools/ext_gen.py from
// data/extensions/amcaf.json (a command leaves this file when its "status" there says "implemented"
// and its "source" has it). Each raises an AMOS error.

#include "core/amos.h"
#include "ext/third_party.h"
#include "lib/strings.h"

using namespace amos;

extern "C" {

// vec rot pos (I0,0,0)
void amcaf_vec_rot_pos(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vec rot angles (I0,0,0)
void amcaf_vec_rot_angles(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vec rot precalc (I)
void amcaf_vec_rot_precalc() { ext3::unsupported(ext3::Id::amcaf); }
// vec rot x (00,0,0)
int32_t amcaf_vec_rot_x_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vec rot x (0)
int32_t amcaf_vec_rot_x_0() { ext3::unsupported(ext3::Id::amcaf); }
// vec rot y (00,0,0)
int32_t amcaf_vec_rot_y_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vec rot y (0)
int32_t amcaf_vec_rot_y_0() { ext3::unsupported(ext3::Id::amcaf); }
// vec rot z (00,0,0)
int32_t amcaf_vec_rot_z_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vec rot z (0)
int32_t amcaf_vec_rot_z_0() { ext3::unsupported(ext3::Id::amcaf); }
// pjoy (00)
int32_t amcaf_pjoy(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pjleft (00)
int32_t amcaf_pjleft(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pjright (00)
int32_t amcaf_pjright(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pjup (00)
int32_t amcaf_pjup(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pjdown (00)
int32_t amcaf_pjdown(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pfire (00)
int32_t amcaf_pfire(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// xfire (00,0)
int32_t amcaf_xfire(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// nop (I)
void amcaf_nop() { ext3::unsupported(ext3::Id::amcaf); }
// nfn (0)
int32_t amcaf_nfn() { ext3::unsupported(ext3::Id::amcaf); }
// audio free (I)
void amcaf_audio_free() { ext3::unsupported(ext3::Id::amcaf); }
// bank to chip (I0)
void amcaf_bank_to_chip(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank to fast (I0)
void amcaf_bank_to_fast(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank delta encode (I0t0)
void amcaf_bank_delta_encode_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank delta encode (I0)
void amcaf_bank_delta_encode_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank delta decode (I0t0)
void amcaf_bank_delta_decode_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank delta decode (I0)
void amcaf_bank_delta_decode_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code xor.b (I0,0t0)
void amcaf_bank_code_xor_b_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code xor.b (I0,0)
void amcaf_bank_code_xor_b_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code add.b (I0,0t0)
void amcaf_bank_code_add_b_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code add.b (I0,0)
void amcaf_bank_code_add_b_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code mix.b (I0,0t0)
void amcaf_bank_code_mix_b_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code mix.b (I0,0)
void amcaf_bank_code_mix_b_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code rol.b (I0,0t0)
void amcaf_bank_code_rol_b_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code rol.b (I0,0)
void amcaf_bank_code_rol_b_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code ror.b (I0,0t0)
void amcaf_bank_code_ror_b_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code ror.b (I0,0)
void amcaf_bank_code_ror_b_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code xor.w (I0,0t0)
void amcaf_bank_code_xor_w_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code xor.w (I0,0)
void amcaf_bank_code_xor_w_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code add.w (I0,0t0)
void amcaf_bank_code_add_w_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code add.w (I0,0)
void amcaf_bank_code_add_w_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code mix.w (I0,0t0)
void amcaf_bank_code_mix_w_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code mix.w (I0,0)
void amcaf_bank_code_mix_w_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code rol.w (I0,0t0)
void amcaf_bank_code_rol_w_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code rol.w (I0,0)
void amcaf_bank_code_rol_w_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code ror.w (I0,0t0)
void amcaf_bank_code_ror_w_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank code ror.w (I0,0)
void amcaf_bank_code_ror_w_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank stretch (I0t0)
void amcaf_bank_stretch(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank checksum (00t0)
int32_t amcaf_bank_checksum_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank checksum (00)
int32_t amcaf_bank_checksum_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank copy (I0,0t0)
void amcaf_bank_copy_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank copy (I0t0)
void amcaf_bank_copy_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bank name (I0,2)
void amcaf_bank_name(int32_t, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// bank name$ (20)
uint32_t amcaf_bank_name_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy limit (I0)
void amcaf_blitter_copy_limit_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy limit (I0,0t0,0)
void amcaf_blitter_copy_limit_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0t0,0)
void amcaf_blitter_copy_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0t0,0,0)
void amcaf_blitter_copy_5(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0,0,0t0,0)
void amcaf_blitter_copy_6(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0,0,0t0,0,0)
void amcaf_blitter_copy_7(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0,0,0,0,0t0,0)
void amcaf_blitter_copy_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter copy (I0,0,0,0,0,0t0,0,0)
void amcaf_blitter_copy_9(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter busy (0)
int32_t amcaf_blitter_busy() { ext3::unsupported(ext3::Id::amcaf); }
// blitter wait (I)
void amcaf_blitter_wait() { ext3::unsupported(ext3::Id::amcaf); }
// blitter clear (I0,0)
void amcaf_blitter_clear_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter clear (I0,0,0,0t0,0)
void amcaf_blitter_clear_6(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter fill (I0,0,0,0,0,0)
void amcaf_blitter_fill_6(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter fill (I0,0t0,0)
void amcaf_blitter_fill_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter fill (I0,0)
void amcaf_blitter_fill_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blitter fill (I0,0,0,0,0,0t0,0)
void amcaf_blitter_fill_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// c2p fire (I0,0,0t0,0)
void amcaf_c2p_fire(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// c2p shift (I0,0,0t0,0)
void amcaf_c2p_shift(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// c2p convert (I0,0,0t0,0,0)
void amcaf_c2p_convert(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// convert grey (I0t0)
void amcaf_convert_grey(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// amcaf aga notation on (I)
void amcaf_aga_notation_on() { ext3::unsupported(ext3::Id::amcaf); }
// amcaf aga notation off (I)
void amcaf_aga_notation_off() { ext3::unsupported(ext3::Id::amcaf); }
// best pen (00)
int32_t amcaf_best_pen_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// best pen (00,0t0)
int32_t amcaf_best_pen_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// mix colour (00,0)
int32_t amcaf_mix_colour_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// mix colour (00,0,0t0)
int32_t amcaf_mix_colour_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// glue colour (00,0,0)
int32_t amcaf_glue_colour(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// red val (00)
int32_t amcaf_red_val(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// green val (00)
int32_t amcaf_green_val(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// blue val (00)
int32_t amcaf_blue_val(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rrggbb to rgb (00)
int32_t amcaf_rrggbb_to_rgb(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rgb to rrggbb (00)
int32_t amcaf_rgb_to_rrggbb(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// count pixels (00,0,0,0t0,0)
int32_t amcaf_count_pixels(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// coords bank (I0)
void amcaf_coords_bank_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// coords bank (I0,0)
void amcaf_coords_bank_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// coords read (I0,0,0,0t0,0,0,0)
void amcaf_coords_read(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// filename$ (22)
uint32_t amcaf_filename_s(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// path$ (22)
uint32_t amcaf_path_s(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// extpath$ (22)
uint32_t amcaf_extpath_s(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// dos hash (02)
int32_t amcaf_dos_hash(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// disk type (02)
int32_t amcaf_disk_type(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// disk state (02)
int32_t amcaf_disk_state(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// pattern match (02,2)
int32_t amcaf_pattern_match(GuestString, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// dload (I2,0)
void amcaf_dload(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// wload (I2,0)
void amcaf_wload(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// wsave (I2,0)
void amcaf_wsave(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// sload (I0t0,0)
void amcaf_sload(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ssave (I0,0t0)
void amcaf_ssave(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// file copy (I2t2)
void amcaf_file_copy(GuestString, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// examine dir (I2)
void amcaf_examine_dir(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// examine next$ (2)
uint32_t amcaf_examine_next_s() { ext3::unsupported(ext3::Id::amcaf); }
// examine stop (I)
void amcaf_examine_stop() { ext3::unsupported(ext3::Id::amcaf); }
// examine object (I2)
void amcaf_examine_object(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object name$ (22)
uint32_t amcaf_object_name_s_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object name$ (2)
uint32_t amcaf_object_name_s_0() { ext3::unsupported(ext3::Id::amcaf); }
// object type (02)
int32_t amcaf_object_type_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object type (0)
int32_t amcaf_object_type_0() { ext3::unsupported(ext3::Id::amcaf); }
// object size (02)
int32_t amcaf_object_size_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object size (0)
int32_t amcaf_object_size_0() { ext3::unsupported(ext3::Id::amcaf); }
// object blocks (02)
int32_t amcaf_object_blocks_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object blocks (0)
int32_t amcaf_object_blocks_0() { ext3::unsupported(ext3::Id::amcaf); }
// object date (02)
int32_t amcaf_object_date_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object date (0)
int32_t amcaf_object_date_0() { ext3::unsupported(ext3::Id::amcaf); }
// object time (02)
int32_t amcaf_object_time_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object time (0)
int32_t amcaf_object_time_0() { ext3::unsupported(ext3::Id::amcaf); }
// object protection (02)
int32_t amcaf_object_protection_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object protection (0)
int32_t amcaf_object_protection_0() { ext3::unsupported(ext3::Id::amcaf); }
// object protection$ (20)
uint32_t amcaf_object_protection_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// object comment$ (22)
uint32_t amcaf_object_comment_s_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// object comment$ (2)
uint32_t amcaf_object_comment_s_0() { ext3::unsupported(ext3::Id::amcaf); }
// protect object (I2,0)
void amcaf_protect_object(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// set object comment (I2,2)
void amcaf_set_object_comment(GuestString, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// set object date (I2,0,0)
void amcaf_set_object_date(GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// extbase (00)
int32_t amcaf_extbase(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// extdefault (I0)
void amcaf_extdefault(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// extremove (I0)
void amcaf_extremove(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// extreinit (I0)
void amcaf_extreinit(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// imploder unpack (I0t0)
void amcaf_imploder_unpack(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// imploder load (I2,0)
void amcaf_imploder_load(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// make bank font (I0)
void amcaf_make_bank_font(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// change bank font (I0)
void amcaf_change_bank_font(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// change print font (I0)
void amcaf_change_print_font(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// change font (I2)
void amcaf_change_font_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// change font (I2,0)
void amcaf_change_font_2(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// change font (I2,0,0)
void amcaf_change_font_3(GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// font style (0)
int32_t amcaf_font_style() { ext3::unsupported(ext3::Id::amcaf); }
// alloc trans source (I0)
void amcaf_alloc_trans_source(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// set trans source (I0)
void amcaf_set_trans_source(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// alloc trans map (I0,0,0)
void amcaf_alloc_trans_map(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// set trans map (I0,0,0)
void amcaf_set_trans_map(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// alloc code bank (I0,0)
void amcaf_alloc_code_bank(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// trans screen runtime (I0,0,0,0)
void amcaf_trans_screen_runtime(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// trans screen dynamic (I0,0,0,0)
void amcaf_trans_screen_dynamic(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// trans screen static (I0,0,0,0)
void amcaf_trans_screen_static(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ham point (00,0)
int32_t amcaf_ham_point(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ham colour (00,0)
int32_t amcaf_ham_colour(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ham best (00,0)
int32_t amcaf_ham_best(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ham fade out (I0)
void amcaf_ham_fade_out(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// x smouse (0)
int32_t amcaf_x_smouse() { ext3::unsupported(ext3::Id::amcaf); }
// y smouse (0)
int32_t amcaf_y_smouse() { ext3::unsupported(ext3::Id::amcaf); }
// smouse x (I0)
void amcaf_smouse_x(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// smouse y (I0)
void amcaf_smouse_y(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// limit smouse (I)
void amcaf_limit_smouse_0() { ext3::unsupported(ext3::Id::amcaf); }
// limit smouse (I0,0t0,0)
void amcaf_limit_smouse_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// smouse speed (I0)
void amcaf_smouse_speed(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// io error (0)
int32_t amcaf_io_error() { ext3::unsupported(ext3::Id::amcaf); }
// io error$ (20)
uint32_t amcaf_io_error_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// mask copy (I0t0,0)
void amcaf_mask_copy_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// mask copy (I0,0,0,0,0t0,0,0,0)
void amcaf_mask_copy_9(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// mask copy (I0,0,0,0,0t0,0,0,0,0)
void amcaf_mask_copy_10(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// lzstr$ (20,0)
uint32_t amcaf_lzstr_s(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// lsstr$ (20,0)
uint32_t amcaf_lsstr_s(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// chr.w$ (20)
uint32_t amcaf_chr_w_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// chr.l$ (20)
uint32_t amcaf_chr_l_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// asc.w (02)
int32_t amcaf_asc_w(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// asc.l (02)
int32_t amcaf_asc_l(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// vclip (00,0t0)
int32_t amcaf_vclip(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vin (00,0t0)
int32_t amcaf_vin(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vmod (00,0)
int32_t amcaf_vmod_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// vmod (00,0t0)
int32_t amcaf_vmod_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// insstr$ (22,2,0)
uint32_t amcaf_insstr_s(GuestString, GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// cutstr$ (22,0t0)
uint32_t amcaf_cutstr_s(GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// replacestr$ (22,2t2)
uint32_t amcaf_replacestr_s(GuestString, GuestString, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// itemstr$ (22,0)
uint32_t amcaf_itemstr_s_2(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// itemstr$ (22,0,2)
uint32_t amcaf_itemstr_s_3(GuestString, int32_t, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// binexp (00)
int32_t amcaf_binexp(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// binlog (00)
int32_t amcaf_binlog(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// lsl (00,0)
int32_t amcaf_lsl(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// lsr (00,0)
int32_t amcaf_lsr(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// wordswap (00)
int32_t amcaf_wordswap(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// sdeek (00)
int32_t amcaf_sdeek(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// speek (00)
int32_t amcaf_speek(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// set rain colour (I0,0)
void amcaf_set_rain_colour(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rain fade (I0,0)
void amcaf_rain_fade_2a(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rain fade (I0t0)
void amcaf_rain_fade_2b(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// x raster (0)
int32_t amcaf_x_raster() { ext3::unsupported(ext3::Id::amcaf); }
// y raster (0)
int32_t amcaf_y_raster() { ext3::unsupported(ext3::Id::amcaf); }
// set ntsc (I)
void amcaf_set_ntsc() { ext3::unsupported(ext3::Id::amcaf); }
// set pal (I)
void amcaf_set_pal() { ext3::unsupported(ext3::Id::amcaf); }
// set sprite priority (I0)
void amcaf_set_sprite_priority(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// cop pos (0)
int32_t amcaf_cop_pos() { ext3::unsupported(ext3::Id::amcaf); }
// exchange bob (I0,0)
void amcaf_exchange_bob(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// exchange icon (I0,0)
void amcaf_exchange_icon(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// write cli (I2)
void amcaf_write_cli(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// reset computer (I)
void amcaf_reset_computer() { ext3::unsupported(ext3::Id::amcaf); }
// cpu (0)
int32_t amcaf_cpu() { ext3::unsupported(ext3::Id::amcaf); }
// fpu (0)
int32_t amcaf_fpu() { ext3::unsupported(ext3::Id::amcaf); }
// aga detect (0)
int32_t amcaf_aga_detect() { ext3::unsupported(ext3::Id::amcaf); }
// open workbench (I)
void amcaf_open_workbench() { ext3::unsupported(ext3::Id::amcaf); }
// launch (I2)
void amcaf_launch_1(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// launch (I2,0)
void amcaf_launch_2(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// shade pix (I0,0)
void amcaf_shade_pix_2(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// shade pix (I0,0,0)
void amcaf_shade_pix_3(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// make pix mask (I0,0,0t0,0,0)
void amcaf_make_pix_mask(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix shift up (I0,0,0,0,0t0,0,0)
void amcaf_pix_shift_up_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix shift up (I0,0,0,0,0t0,0)
void amcaf_pix_shift_up_7(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix shift down (I0,0,0,0,0t0,0,0)
void amcaf_pix_shift_down_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix shift down (I0,0,0,0,0t0,0)
void amcaf_pix_shift_down_7(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix brighten (I0,0,0,0,0t0,0,0)
void amcaf_pix_brighten_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix brighten (I0,0,0,0,0t0,0)
void amcaf_pix_brighten_7(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix darken (I0,0,0,0,0t0,0,0)
void amcaf_pix_darken_8(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pix darken (I0,0,0,0,0t0,0)
void amcaf_pix_darken_7(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pptodisk (I2,0,0)
void amcaf_pptodisk_3(GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pptodisk (I2,0)
void amcaf_pptodisk_2(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ppunpack (I0t0)
void amcaf_ppunpack(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ppfromdisk (I2,0)
void amcaf_ppfromdisk(GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ptile bank (I0)
void amcaf_ptile_bank(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// paste ptile (I0,0,0)
void amcaf_paste_ptile(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// qsqr (00)
int32_t amcaf_qsqr(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// qrnd (00)
int32_t amcaf_qrnd(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// qcos (00,0)
int32_t amcaf_qcos(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// qsin (00,0)
int32_t amcaf_qsin(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// qarc (00,0)
int32_t amcaf_qarc(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rnc unpack (I0t0)
void amcaf_rnc_unpack(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// rnp (0)
int32_t amcaf_rnp() { ext3::unsupported(ext3::Id::amcaf); }
// scanstr$ (20)
uint32_t amcaf_scanstr_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// scrn rastport (0)
int32_t amcaf_scrn_rastport() { ext3::unsupported(ext3::Id::amcaf); }
// scrn bitmap (0)
int32_t amcaf_scrn_bitmap() { ext3::unsupported(ext3::Id::amcaf); }
// scrn layerinfo (0)
int32_t amcaf_scrn_layerinfo() { ext3::unsupported(ext3::Id::amcaf); }
// scrn layer (0)
int32_t amcaf_scrn_layer() { ext3::unsupported(ext3::Id::amcaf); }
// scrn region (0)
int32_t amcaf_scrn_region() { ext3::unsupported(ext3::Id::amcaf); }
// shade bob mask (I0)
void amcaf_shade_bob_mask(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// shade bob planes (I0)
void amcaf_shade_bob_planes(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// shade bob up (I0,0,0,0)
void amcaf_shade_bob_up(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// shade bob down (I0,0,0,0)
void amcaf_shade_bob_down(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters bank (I0,0)
void amcaf_splinters_bank(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters max (I0)
void amcaf_splinters_max(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters fuel (I0)
void amcaf_splinters_fuel(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters limit (I)
void amcaf_splinters_limit_0() { ext3::unsupported(ext3::Id::amcaf); }
// splinters limit (I0,0t0,0)
void amcaf_splinters_limit_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters gravity (I0,0)
void amcaf_splinters_gravity(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters colour (I0,0)
void amcaf_splinters_colour(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// splinters init (I)
void amcaf_splinters_init() { ext3::unsupported(ext3::Id::amcaf); }
// splinters single do (I)
void amcaf_splinters_single_do() { ext3::unsupported(ext3::Id::amcaf); }
// splinters double do (I)
void amcaf_splinters_double_do() { ext3::unsupported(ext3::Id::amcaf); }
// splinters single del (I)
void amcaf_splinters_single_del() { ext3::unsupported(ext3::Id::amcaf); }
// splinters double del (I)
void amcaf_splinters_double_del() { ext3::unsupported(ext3::Id::amcaf); }
// splinters move (I)
void amcaf_splinters_move() { ext3::unsupported(ext3::Id::amcaf); }
// splinters back (I)
void amcaf_splinters_back() { ext3::unsupported(ext3::Id::amcaf); }
// splinters draw (I)
void amcaf_splinters_draw() { ext3::unsupported(ext3::Id::amcaf); }
// splinters active (0)
int32_t amcaf_splinters_active() { ext3::unsupported(ext3::Id::amcaf); }
// td stars bank (I0,0)
void amcaf_td_stars_bank(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars limit (I)
void amcaf_td_stars_limit_0() { ext3::unsupported(ext3::Id::amcaf); }
// td stars limit (I0,0t0,0)
void amcaf_td_stars_limit_4(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars origin (I0,0)
void amcaf_td_stars_origin(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars init (I)
void amcaf_td_stars_init() { ext3::unsupported(ext3::Id::amcaf); }
// td stars gravity (I0,0)
void amcaf_td_stars_gravity(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars accelerate on (I)
void amcaf_td_stars_accelerate_on() { ext3::unsupported(ext3::Id::amcaf); }
// td stars accelerate off (I)
void amcaf_td_stars_accelerate_off() { ext3::unsupported(ext3::Id::amcaf); }
// td stars planes (I0,0)
void amcaf_td_stars_planes(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars single do (I)
void amcaf_td_stars_single_do() { ext3::unsupported(ext3::Id::amcaf); }
// td stars double do (I)
void amcaf_td_stars_double_do() { ext3::unsupported(ext3::Id::amcaf); }
// td stars single del (I)
void amcaf_td_stars_single_del() { ext3::unsupported(ext3::Id::amcaf); }
// td stars double del (I)
void amcaf_td_stars_double_del() { ext3::unsupported(ext3::Id::amcaf); }
// td stars move (I)
void amcaf_td_stars_move_0() { ext3::unsupported(ext3::Id::amcaf); }
// td stars move (I0)
void amcaf_td_stars_move_1(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// td stars draw (I)
void amcaf_td_stars_draw() { ext3::unsupported(ext3::Id::amcaf); }
// current date (0)
int32_t amcaf_current_date() { ext3::unsupported(ext3::Id::amcaf); }
// current time (0)
int32_t amcaf_current_time() { ext3::unsupported(ext3::Id::amcaf); }
// cd year (00)
int32_t amcaf_cd_year(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// cd month (00)
int32_t amcaf_cd_month(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// cd day (00)
int32_t amcaf_cd_day(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// cd weekday (00)
int32_t amcaf_cd_weekday(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct string (02)
int32_t amcaf_ct_string(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// cd string (02)
int32_t amcaf_cd_string(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// cd date$ (20)
uint32_t amcaf_cd_date_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct hour (00)
int32_t amcaf_ct_hour(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct minute (00)
int32_t amcaf_ct_minute(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct second (00)
int32_t amcaf_ct_second(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct tick (00)
int32_t amcaf_ct_tick(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// ct time$ (20)
uint32_t amcaf_ct_time_s(int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pal spread (I0,0t0,0)
void amcaf_pal_spread(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pal get screen (I0,0)
void amcaf_pal_get_screen(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pal set screen (I0,0)
void amcaf_pal_set_screen(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pal get (00,0)
int32_t amcaf_pal_get(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// pal set (I0,0,0)
void amcaf_pal_set(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// amos task (0)
int32_t amcaf_amos_task() { ext3::unsupported(ext3::Id::amcaf); }
// command name$ (2)
uint32_t amcaf_command_name_s() { ext3::unsupported(ext3::Id::amcaf); }
// amos cli (0)
int32_t amcaf_amos_cli() { ext3::unsupported(ext3::Id::amcaf); }
// tool types$ (22)
uint32_t amcaf_tool_types_s(GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// turbo text (I0,0,2)
void amcaf_turbo_text_3(int32_t, int32_t, GuestString) { ext3::unsupported(ext3::Id::amcaf); }
// turbo text (I0,0,2,0)
void amcaf_turbo_text_4(int32_t, int32_t, GuestString, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// turbo text (I0,0,2,0,0)
void amcaf_turbo_text_5(int32_t, int32_t, GuestString, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// turbo draw (I0,0t0,0,0)
void amcaf_turbo_draw_5(int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// turbo draw (I0,0t0,0,0,0)
void amcaf_turbo_draw_6(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// turbo plot (I0,0,0)
void amcaf_turbo_plot(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// turbo point (00,0)
int32_t amcaf_turbo_point(int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// fcircle (I0,0,0)
void amcaf_fcircle(int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// fellipse (I0,0,0,0)
void amcaf_fellipse(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bzoom (I0,0,0,0,0t0,0,0,0)
void amcaf_bzoom(int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }
// bcircle (I0,0,0,0)
void amcaf_bcircle(int32_t, int32_t, int32_t, int32_t) { ext3::unsupported(ext3::Id::amcaf); }

}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_stubs.inc"
#endif
