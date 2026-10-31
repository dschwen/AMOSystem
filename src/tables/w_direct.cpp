// The functions of AMOS.library's vector tables, by their number (converted once from
// +W.s and src/w/*_glue.s by tools/w_direct.py)
#include "core/amos.h"

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

namespace amos {

const WFunction sy_direct[101] = {
    w_sy_inkey,
    w_sy_clear_key,
    w_sy_key_shift,
    w_sy_key_state,
    w_sy_nothing,
    w_sy_joy,
    w_sy_put_key,
    w_sy_mouse_hide,
    w_sy_mouse_show,
    nullptr,
    w_sy_mouse_xy,
    w_sy_xy_hard,
    w_sy_xy_screen,
    w_sy_mouse_key,
    w_sy_mouse_set,
    w_sy_screen_at,
    w_sy_xy_window,
    w_sy_mouse_limit,
    w_sy_zone_hard,
    w_sy_zones_reserve,
    w_sy_zone_reset,
    w_sy_zone_set,
    w_sy_zone_mouse,
    w_sy_wait_vbl,
    w_sy_sprite_set,
    w_sy_sprite_unset,
    w_sy_set_function_key,
    w_sy_get_function_key,
    w_sy_sprites_show,
    w_sy_sprite_bank,
    w_sy_sprite,
    w_sy_sprite_off,
    w_sy_sprites_off,
    w_sy_sprites_update,
    w_sy_sprite_buffer,
    w_sy_sprites_stop,
    w_sy_sprites_restart,
    w_sy_mouse_store,
    nullptr,
    w_sy_sprite_priority,
    w_sy_amal_tok,
    w_sy_amal_cre,
    w_sy_amal_mvo,
    w_sy_amal_dall,
    w_sy_amal_animate,
    w_sy_amal_reg,
    w_sy_amal_clr,
    w_sy_amal_frz,
    w_sy_amal_ufrz,
    w_sy_bob_set,
    w_sy_bob_off,
    w_sy_bobs_off,
    w_sy_bob_update,
    w_sy_bob_show,
    w_sy_bob_erase,
    nullptr,
    nullptr,
    w_sy_bob_limit,
    w_sy_zone_graphic,
    w_sy_get_bob,
    w_sy_mask,
    w_sy_hot_spot,
    w_sy_bob_collisions,
    nullptr,
    w_sy_sprite_collisions,
    w_sy_set_sync,
    w_sy_sync,
    w_sy_play_set,
    w_sy_bob_xy,
    w_sy_sprite_xy,
    w_sy_bob_put,
    w_sy_paste,
    w_sy_mouse_click,
    w_sy_mouse_limit_screen,
    nullptr,
    w_sy_hardcol_set,
    w_sy_hardcol_get,
    w_sy_movon,
    w_sy_key_speed,
    w_sy_chan_a,
    w_sy_chan_m,
    w_sy_priority,
    w_sy_get_disc,
    w_sy_add_vbl,
    w_sy_rem_vbl,
    w_sy_key_waiting,
    w_sy_mouse_screen,
    w_sy_mem_reserve,
    w_sy_mem_free,
    w_sy_nothing,
    w_sy_mem_fast_clear,
    w_sy_mem_chip_clear,
    w_sy_mem_fast,
    w_sy_mem_chip,
    w_sy_send_fake_event,
    w_sy_test_cyclic,
    w_sy_add_flush_routine,
    w_sy_mem_flush,
    w_sy_add_routine,
    w_sy_call_routines,
    w_sy_request_on_off,
};

const WFunction ec_direct[77] = {
    w_ec_reset,
    w_ec_copper_make,
    w_ec_copper_make,
    w_ec_open,
    w_ec_close,
    w_ec_to_front,
    w_ec_to_back,
    w_ec_activate,
    w_ec_copper_force,
    w_ec_view,
    w_ec_offset,
    w_ec_end,
    w_ec_close_range,
    w_ec_get_colour,
    w_ec_set_colour,
    w_ec_set_palette,
    w_ec_colour_back,
    w_ec_flash_off,
    w_ec_flash,
    w_ec_shift_off,
    w_ec_shift,
    w_ec_hide,
    w_ec_cblock_get,
    w_ec_cblock_put,
    w_ec_cblock_delete,
    w_ec_cblocks_delete,
    w_ec_free_number,
    w_ec_clone,
    w_ec_current,
    w_ec_double,
    w_ec_swap,
    w_ec_swap_all,
    w_ec_address,
    w_ec_dual,
    w_ec_dual_priority,
    w_ec_cls,
    w_ec_pattern,
    w_ec_get_fonts,
    w_ec_free_fonts,
    w_ec_get_font,
    w_ec_set_font,
    w_ec_set_clip_fn,
    w_ec_block_get,
    w_ec_block_delete,
    w_ec_blocks_delete,
    w_ec_block_put,
    w_ec_slider_v,
    w_ec_slider_h,
    w_ec_slider_set,
    w_ec_menu_start,
    w_ec_menu_stop,
    w_ec_rain_del,
    w_ec_rain_set,
    w_ec_rain_do,
    w_ec_rain_hide,
    w_ec_rain_var,
    w_ec_fade,
    w_ec_fade_off,
    w_ec_copper_on_off,
    w_ec_cop_reset,
    w_ec_cop_swap,
    w_ec_cop_wait,
    w_ec_cop_move,
    w_ec_cop_movel,
    w_ec_cop_base,
    w_ec_autoback1,
    w_ec_autoback2,
    w_ec_autoback3,
    w_ec_autoback4,
    w_ec_paint,
    w_ec_block_flip,
    w_ec_flip,
    w_sy_amos_wb,
    w_ec_screen_copy,
    w_ec_max_raw,
    w_ec_ntsc,
    w_ec_slider_knob,
};

} // namespace amos
