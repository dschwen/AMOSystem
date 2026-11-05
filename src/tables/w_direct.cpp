// The functions of AMOS.library's vector tables, by their number (converted once from
// +W.s and src/w/*_glue.s by tools/w_direct.py)
#include "core/amos.h"

#if defined(__mc68000__)
extern "C" {
void w_ec_activate(amos::AllRegs *);
void w_ec_address(amos::AllRegs *);
void w_ec_autoback1(amos::AllRegs *);
void w_ec_autoback2(amos::AllRegs *);
void w_ec_autoback3(amos::AllRegs *);
void w_ec_autoback4(amos::AllRegs *);
void w_ec_block_delete(amos::AllRegs *);
void w_ec_block_flip(amos::AllRegs *);
void w_ec_block_get(amos::AllRegs *);
void w_ec_block_put(amos::AllRegs *);
void w_ec_blocks_delete(amos::AllRegs *);
void w_ec_cblock_delete(amos::AllRegs *);
void w_ec_cblock_get(amos::AllRegs *);
void w_ec_cblock_put(amos::AllRegs *);
void w_ec_cblocks_delete(amos::AllRegs *);
void w_ec_clone(amos::AllRegs *);
void w_ec_close(amos::AllRegs *);
void w_ec_close_range(amos::AllRegs *);
void w_ec_cls(amos::AllRegs *);
void w_ec_colour_back(amos::AllRegs *);
void w_ec_cop_base(amos::AllRegs *);
void w_ec_cop_move(amos::AllRegs *);
void w_ec_cop_movel(amos::AllRegs *);
void w_ec_cop_reset(amos::AllRegs *);
void w_ec_cop_swap(amos::AllRegs *);
void w_ec_cop_wait(amos::AllRegs *);
void w_ec_copper_force(amos::AllRegs *);
void w_ec_copper_make(amos::AllRegs *);
void w_ec_copper_on_off(amos::AllRegs *);
void w_ec_current(amos::AllRegs *);
void w_ec_double(amos::AllRegs *);
void w_ec_dual(amos::AllRegs *);
void w_ec_dual_priority(amos::AllRegs *);
void w_ec_end(amos::AllRegs *);
void w_ec_fade(amos::AllRegs *);
void w_ec_fade_off(amos::AllRegs *);
void w_ec_flash(amos::AllRegs *);
void w_ec_flash_off(amos::AllRegs *);
void w_ec_flip(amos::AllRegs *);
void w_ec_free_fonts(amos::AllRegs *);
void w_ec_free_number(amos::AllRegs *);
void w_ec_get_colour(amos::AllRegs *);
void w_ec_get_font(amos::AllRegs *);
void w_ec_get_fonts(amos::AllRegs *);
void w_ec_hide(amos::AllRegs *);
void w_ec_max_raw(amos::AllRegs *);
void w_ec_menu_start(amos::AllRegs *);
void w_ec_menu_stop(amos::AllRegs *);
void w_ec_ntsc(amos::AllRegs *);
void w_ec_offset(amos::AllRegs *);
void w_ec_open(amos::AllRegs *);
void w_ec_paint(amos::AllRegs *);
void w_ec_pattern(amos::AllRegs *);
void w_ec_rain_del(amos::AllRegs *);
void w_ec_rain_do(amos::AllRegs *);
void w_ec_rain_hide(amos::AllRegs *);
void w_ec_rain_set(amos::AllRegs *);
void w_ec_rain_var(amos::AllRegs *);
void w_ec_reset(amos::AllRegs *);
void w_ec_screen_copy(amos::AllRegs *);
void w_ec_set_clip_fn(amos::AllRegs *);
void w_ec_set_colour(amos::AllRegs *);
void w_ec_set_font(amos::AllRegs *);
void w_ec_set_palette(amos::AllRegs *);
void w_ec_shift(amos::AllRegs *);
void w_ec_shift_off(amos::AllRegs *);
void w_ec_slider_h(amos::AllRegs *);
void w_ec_slider_knob(amos::AllRegs *);
void w_ec_slider_set(amos::AllRegs *);
void w_ec_slider_v(amos::AllRegs *);
void w_ec_swap(amos::AllRegs *);
void w_ec_swap_all(amos::AllRegs *);
void w_ec_to_back(amos::AllRegs *);
void w_ec_to_front(amos::AllRegs *);
void w_ec_view(amos::AllRegs *);
void w_sy_add_flush_routine(amos::AllRegs *);
void w_sy_add_routine(amos::AllRegs *);
void w_sy_add_vbl(amos::AllRegs *);
void w_sy_amal_animate(amos::AllRegs *);
void w_sy_amal_clr(amos::AllRegs *);
void w_sy_amal_cre(amos::AllRegs *);
void w_sy_amal_dall(amos::AllRegs *);
void w_sy_amal_frz(amos::AllRegs *);
void w_sy_amal_mvo(amos::AllRegs *);
void w_sy_amal_reg(amos::AllRegs *);
void w_sy_amal_tok(amos::AllRegs *);
void w_sy_amal_ufrz(amos::AllRegs *);
void w_sy_amos_wb(amos::AllRegs *);
void w_sy_bob_collisions(amos::AllRegs *);
void w_sy_bob_erase(amos::AllRegs *);
void w_sy_bob_limit(amos::AllRegs *);
void w_sy_bob_off(amos::AllRegs *);
void w_sy_bob_put(amos::AllRegs *);
void w_sy_bob_set(amos::AllRegs *);
void w_sy_bob_show(amos::AllRegs *);
void w_sy_bob_update(amos::AllRegs *);
void w_sy_bob_xy(amos::AllRegs *);
void w_sy_bobs_off(amos::AllRegs *);
void w_sy_call_routines(amos::AllRegs *);
void w_sy_chan_a(amos::AllRegs *);
void w_sy_chan_m(amos::AllRegs *);
void w_sy_clear_key(amos::AllRegs *);
void w_sy_get_bob(amos::AllRegs *);
void w_sy_get_disc(amos::AllRegs *);
void w_sy_get_function_key(amos::AllRegs *);
void w_sy_hardcol_get(amos::AllRegs *);
void w_sy_hardcol_set(amos::AllRegs *);
void w_sy_hot_spot(amos::AllRegs *);
void w_sy_inkey(amos::AllRegs *);
void w_sy_joy(amos::AllRegs *);
void w_sy_key_shift(amos::AllRegs *);
void w_sy_key_speed(amos::AllRegs *);
void w_sy_key_state(amos::AllRegs *);
void w_sy_key_waiting(amos::AllRegs *);
void w_sy_mask(amos::AllRegs *);
void w_sy_mem_chip(amos::AllRegs *);
void w_sy_mem_chip_clear(amos::AllRegs *);
void w_sy_mem_fast(amos::AllRegs *);
void w_sy_mem_fast_clear(amos::AllRegs *);
void w_sy_mem_flush(amos::AllRegs *);
void w_sy_mem_free(amos::AllRegs *);
void w_sy_mem_reserve(amos::AllRegs *);
void w_sy_mouse_click(amos::AllRegs *);
void w_sy_mouse_hide(amos::AllRegs *);
void w_sy_mouse_key(amos::AllRegs *);
void w_sy_mouse_limit(amos::AllRegs *);
void w_sy_mouse_limit_screen(amos::AllRegs *);
void w_sy_mouse_screen(amos::AllRegs *);
void w_sy_mouse_set(amos::AllRegs *);
void w_sy_mouse_show(amos::AllRegs *);
void w_sy_mouse_store(amos::AllRegs *);
void w_sy_mouse_xy(amos::AllRegs *);
void w_sy_movon(amos::AllRegs *);
void w_sy_nothing(amos::AllRegs *);
void w_sy_paste(amos::AllRegs *);
void w_sy_play_set(amos::AllRegs *);
void w_sy_priority(amos::AllRegs *);
void w_sy_put_key(amos::AllRegs *);
void w_sy_rem_vbl(amos::AllRegs *);
void w_sy_request_on_off(amos::AllRegs *);
void w_sy_screen_at(amos::AllRegs *);
void w_sy_send_fake_event(amos::AllRegs *);
void w_sy_set_function_key(amos::AllRegs *);
void w_sy_set_sync(amos::AllRegs *);
void w_sy_sprite(amos::AllRegs *);
void w_sy_sprite_bank(amos::AllRegs *);
void w_sy_sprite_buffer(amos::AllRegs *);
void w_sy_sprite_collisions(amos::AllRegs *);
void w_sy_sprite_off(amos::AllRegs *);
void w_sy_sprite_priority(amos::AllRegs *);
void w_sy_sprite_set(amos::AllRegs *);
void w_sy_sprite_unset(amos::AllRegs *);
void w_sy_sprite_xy(amos::AllRegs *);
void w_sy_sprites_off(amos::AllRegs *);
void w_sy_sprites_restart(amos::AllRegs *);
void w_sy_sprites_show(amos::AllRegs *);
void w_sy_sprites_stop(amos::AllRegs *);
void w_sy_sprites_update(amos::AllRegs *);
void w_sy_sync(amos::AllRegs *);
void w_sy_test_cyclic(amos::AllRegs *);
void w_sy_wait_vbl(amos::AllRegs *);
void w_sy_xy_hard(amos::AllRegs *);
void w_sy_xy_screen(amos::AllRegs *);
void w_sy_xy_window(amos::AllRegs *);
void w_sy_zone_graphic(amos::AllRegs *);
void w_sy_zone_hard(amos::AllRegs *);
void w_sy_zone_mouse(amos::AllRegs *);
void w_sy_zone_reset(amos::AllRegs *);
void w_sy_zone_set(amos::AllRegs *);
void w_sy_zones_reserve(amos::AllRegs *);
}
#define AMOS_DIRECT_W(name) name
#define AMOS_DIRECT_W_RESULT(name) name
#else
#include "runtime/native_bindings.h"
namespace {
template<auto Fn> void direct_ignore_result(amos::AllRegs* registers) { (void)Fn(registers); }
}
#define AMOS_DIRECT_W(name) amos_typed_##name
#define AMOS_DIRECT_W_RESULT(name) direct_ignore_result<&amos_typed_##name>
#endif

namespace amos {

const WFunction sy_direct[101] = {
    AMOS_DIRECT_W(w_sy_inkey),
    AMOS_DIRECT_W(w_sy_clear_key),
    AMOS_DIRECT_W(w_sy_key_shift),
    AMOS_DIRECT_W(w_sy_key_state),
    AMOS_DIRECT_W(w_sy_nothing),
    AMOS_DIRECT_W(w_sy_joy),
    AMOS_DIRECT_W(w_sy_put_key),
    AMOS_DIRECT_W(w_sy_mouse_hide),
    AMOS_DIRECT_W(w_sy_mouse_show),
    nullptr,
    AMOS_DIRECT_W(w_sy_mouse_xy),
    AMOS_DIRECT_W(w_sy_xy_hard),
    AMOS_DIRECT_W(w_sy_xy_screen),
    AMOS_DIRECT_W(w_sy_mouse_key),
    AMOS_DIRECT_W(w_sy_mouse_set),
    AMOS_DIRECT_W(w_sy_screen_at),
    AMOS_DIRECT_W(w_sy_xy_window),
    AMOS_DIRECT_W(w_sy_mouse_limit),
    AMOS_DIRECT_W(w_sy_zone_hard),
    AMOS_DIRECT_W(w_sy_zones_reserve),
    AMOS_DIRECT_W(w_sy_zone_reset),
    AMOS_DIRECT_W(w_sy_zone_set),
    AMOS_DIRECT_W(w_sy_zone_mouse),
    AMOS_DIRECT_W(w_sy_wait_vbl),
    AMOS_DIRECT_W(w_sy_sprite_set),
    AMOS_DIRECT_W(w_sy_sprite_unset),
    AMOS_DIRECT_W(w_sy_set_function_key),
    AMOS_DIRECT_W(w_sy_get_function_key),
    AMOS_DIRECT_W(w_sy_sprites_show),
    AMOS_DIRECT_W(w_sy_sprite_bank),
    AMOS_DIRECT_W(w_sy_sprite),
    AMOS_DIRECT_W(w_sy_sprite_off),
    AMOS_DIRECT_W(w_sy_sprites_off),
    AMOS_DIRECT_W(w_sy_sprites_update),
    AMOS_DIRECT_W(w_sy_sprite_buffer),
    AMOS_DIRECT_W(w_sy_sprites_stop),
    AMOS_DIRECT_W(w_sy_sprites_restart),
    AMOS_DIRECT_W(w_sy_mouse_store),
    nullptr,
    AMOS_DIRECT_W(w_sy_sprite_priority),
    AMOS_DIRECT_W(w_sy_amal_tok),
    AMOS_DIRECT_W(w_sy_amal_cre),
    AMOS_DIRECT_W(w_sy_amal_mvo),
    AMOS_DIRECT_W(w_sy_amal_dall),
    AMOS_DIRECT_W(w_sy_amal_animate),
    AMOS_DIRECT_W(w_sy_amal_reg),
    AMOS_DIRECT_W(w_sy_amal_clr),
    AMOS_DIRECT_W(w_sy_amal_frz),
    AMOS_DIRECT_W(w_sy_amal_ufrz),
    AMOS_DIRECT_W(w_sy_bob_set),
    AMOS_DIRECT_W(w_sy_bob_off),
    AMOS_DIRECT_W(w_sy_bobs_off),
    AMOS_DIRECT_W(w_sy_bob_update),
    AMOS_DIRECT_W(w_sy_bob_show),
    AMOS_DIRECT_W(w_sy_bob_erase),
    nullptr,
    nullptr,
    AMOS_DIRECT_W(w_sy_bob_limit),
    AMOS_DIRECT_W(w_sy_zone_graphic),
    AMOS_DIRECT_W(w_sy_get_bob),
    AMOS_DIRECT_W(w_sy_mask),
    AMOS_DIRECT_W(w_sy_hot_spot),
    AMOS_DIRECT_W(w_sy_bob_collisions),
    nullptr,
    AMOS_DIRECT_W(w_sy_sprite_collisions),
    AMOS_DIRECT_W(w_sy_set_sync),
    AMOS_DIRECT_W(w_sy_sync),
    AMOS_DIRECT_W(w_sy_play_set),
    AMOS_DIRECT_W(w_sy_bob_xy),
    AMOS_DIRECT_W(w_sy_sprite_xy),
    AMOS_DIRECT_W(w_sy_bob_put),
    AMOS_DIRECT_W(w_sy_paste),
    AMOS_DIRECT_W(w_sy_mouse_click),
    AMOS_DIRECT_W(w_sy_mouse_limit_screen),
    nullptr,
    AMOS_DIRECT_W(w_sy_hardcol_set),
    AMOS_DIRECT_W(w_sy_hardcol_get),
    AMOS_DIRECT_W(w_sy_movon),
    AMOS_DIRECT_W(w_sy_key_speed),
    AMOS_DIRECT_W(w_sy_chan_a),
    AMOS_DIRECT_W(w_sy_chan_m),
    AMOS_DIRECT_W(w_sy_priority),
    AMOS_DIRECT_W(w_sy_get_disc),
    AMOS_DIRECT_W(w_sy_add_vbl),
    AMOS_DIRECT_W(w_sy_rem_vbl),
    AMOS_DIRECT_W_RESULT(w_sy_key_waiting),
    AMOS_DIRECT_W(w_sy_mouse_screen),
    AMOS_DIRECT_W(w_sy_mem_reserve),
    AMOS_DIRECT_W(w_sy_mem_free),
    AMOS_DIRECT_W(w_sy_nothing),
    AMOS_DIRECT_W(w_sy_mem_fast_clear),
    AMOS_DIRECT_W(w_sy_mem_chip_clear),
    AMOS_DIRECT_W(w_sy_mem_fast),
    AMOS_DIRECT_W(w_sy_mem_chip),
    AMOS_DIRECT_W(w_sy_send_fake_event),
    AMOS_DIRECT_W(w_sy_test_cyclic),
    AMOS_DIRECT_W(w_sy_add_flush_routine),
    AMOS_DIRECT_W(w_sy_mem_flush),
    AMOS_DIRECT_W(w_sy_add_routine),
    AMOS_DIRECT_W(w_sy_call_routines),
    AMOS_DIRECT_W(w_sy_request_on_off),
};

const WFunction ec_direct[77] = {
    AMOS_DIRECT_W(w_ec_reset),
    AMOS_DIRECT_W(w_ec_copper_make),
    AMOS_DIRECT_W(w_ec_copper_make),
    AMOS_DIRECT_W(w_ec_open),
    AMOS_DIRECT_W(w_ec_close),
    AMOS_DIRECT_W(w_ec_to_front),
    AMOS_DIRECT_W(w_ec_to_back),
    AMOS_DIRECT_W(w_ec_activate),
    AMOS_DIRECT_W(w_ec_copper_force),
    AMOS_DIRECT_W(w_ec_view),
    AMOS_DIRECT_W(w_ec_offset),
    AMOS_DIRECT_W(w_ec_end),
    AMOS_DIRECT_W(w_ec_close_range),
    AMOS_DIRECT_W(w_ec_get_colour),
    AMOS_DIRECT_W(w_ec_set_colour),
    AMOS_DIRECT_W(w_ec_set_palette),
    AMOS_DIRECT_W(w_ec_colour_back),
    AMOS_DIRECT_W(w_ec_flash_off),
    AMOS_DIRECT_W(w_ec_flash),
    AMOS_DIRECT_W(w_ec_shift_off),
    AMOS_DIRECT_W(w_ec_shift),
    AMOS_DIRECT_W(w_ec_hide),
    AMOS_DIRECT_W(w_ec_cblock_get),
    AMOS_DIRECT_W(w_ec_cblock_put),
    AMOS_DIRECT_W(w_ec_cblock_delete),
    AMOS_DIRECT_W(w_ec_cblocks_delete),
    AMOS_DIRECT_W(w_ec_free_number),
    AMOS_DIRECT_W(w_ec_clone),
    AMOS_DIRECT_W(w_ec_current),
    AMOS_DIRECT_W(w_ec_double),
    AMOS_DIRECT_W(w_ec_swap),
    AMOS_DIRECT_W(w_ec_swap_all),
    AMOS_DIRECT_W(w_ec_address),
    AMOS_DIRECT_W(w_ec_dual),
    AMOS_DIRECT_W(w_ec_dual_priority),
    AMOS_DIRECT_W(w_ec_cls),
    AMOS_DIRECT_W(w_ec_pattern),
    AMOS_DIRECT_W(w_ec_get_fonts),
    AMOS_DIRECT_W(w_ec_free_fonts),
    AMOS_DIRECT_W(w_ec_get_font),
    AMOS_DIRECT_W(w_ec_set_font),
    AMOS_DIRECT_W(w_ec_set_clip_fn),
    AMOS_DIRECT_W(w_ec_block_get),
    AMOS_DIRECT_W(w_ec_block_delete),
    AMOS_DIRECT_W(w_ec_blocks_delete),
    AMOS_DIRECT_W(w_ec_block_put),
    AMOS_DIRECT_W(w_ec_slider_v),
    AMOS_DIRECT_W(w_ec_slider_h),
    AMOS_DIRECT_W(w_ec_slider_set),
    AMOS_DIRECT_W(w_ec_menu_start),
    AMOS_DIRECT_W(w_ec_menu_stop),
    AMOS_DIRECT_W(w_ec_rain_del),
    AMOS_DIRECT_W(w_ec_rain_set),
    AMOS_DIRECT_W(w_ec_rain_do),
    AMOS_DIRECT_W(w_ec_rain_hide),
    AMOS_DIRECT_W(w_ec_rain_var),
    AMOS_DIRECT_W(w_ec_fade),
    AMOS_DIRECT_W(w_ec_fade_off),
    AMOS_DIRECT_W(w_ec_copper_on_off),
    AMOS_DIRECT_W(w_ec_cop_reset),
    AMOS_DIRECT_W(w_ec_cop_swap),
    AMOS_DIRECT_W(w_ec_cop_wait),
    AMOS_DIRECT_W(w_ec_cop_move),
    AMOS_DIRECT_W(w_ec_cop_movel),
    AMOS_DIRECT_W(w_ec_cop_base),
    AMOS_DIRECT_W(w_ec_autoback1),
    AMOS_DIRECT_W(w_ec_autoback2),
    AMOS_DIRECT_W(w_ec_autoback3),
    AMOS_DIRECT_W(w_ec_autoback4),
    AMOS_DIRECT_W(w_ec_paint),
    AMOS_DIRECT_W(w_ec_block_flip),
    AMOS_DIRECT_W(w_ec_flip),
    AMOS_DIRECT_W(w_sy_amos_wb),
    AMOS_DIRECT_W(w_ec_screen_copy),
    AMOS_DIRECT_W(w_ec_max_raw),
    AMOS_DIRECT_W(w_ec_ntsc),
    AMOS_DIRECT_W(w_ec_slider_knob),
};

} // namespace amos
