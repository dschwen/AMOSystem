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
uint32_t amos_typed_lib_app_centre(RegFrame& r) { return detail::typed_reg_call<&lib_app_centre>(r); }
uint64_t amos_typed_lib_fn_amos_here(const uint32_t* words) { return typed_library_call<&lib_fn_amos_here, 0, 4>(words); }
uint64_t amos_typed_lib_fn_colour(const uint32_t* words) { return typed_library_call<&lib_fn_colour, 1, 4>(words); }
uint64_t amos_typed_lib_fn_cop_logic(const uint32_t* words) { return typed_library_call<&lib_fn_cop_logic, 0, 4>(words); }
uint64_t amos_typed_lib_fn_display_height(const uint32_t* words) { return typed_library_call<&lib_fn_display_height, 0, 4>(words); }
uint64_t amos_typed_lib_fn_font(const uint32_t* words) { return typed_library_call<&lib_fn_font, 1, 4>(words); }
uint64_t amos_typed_lib_fn_hires(const uint32_t* words) { return typed_library_call<&lib_fn_hires, 0, 4>(words); }
uint64_t amos_typed_lib_fn_hzone2(const uint32_t* words) { return typed_library_call<&lib_fn_hzone2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_hzone3(const uint32_t* words) { return typed_library_call<&lib_fn_hzone3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_laced(const uint32_t* words) { return typed_library_call<&lib_fn_laced, 0, 4>(words); }
uint64_t amos_typed_lib_fn_log_base(const uint32_t* words) { return typed_library_call<&lib_fn_log_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_logic0(const uint32_t* words) { return typed_library_call<&lib_fn_logic0, 0, 4>(words); }
uint64_t amos_typed_lib_fn_logic1(const uint32_t* words) { return typed_library_call<&lib_fn_logic1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_lowres(const uint32_t* words) { return typed_library_call<&lib_fn_lowres, 0, 4>(words); }
uint64_t amos_typed_lib_fn_mouse_screen(const uint32_t* words) { return typed_library_call<&lib_fn_mouse_screen, 0, 4>(words); }
uint64_t amos_typed_lib_fn_mouse_zone(const uint32_t* words) { return typed_library_call<&lib_fn_mouse_zone, 0, 4>(words); }
uint64_t amos_typed_lib_fn_ntsc(const uint32_t* words) { return typed_library_call<&lib_fn_ntsc, 0, 4>(words); }
uint64_t amos_typed_lib_fn_phy_base(const uint32_t* words) { return typed_library_call<&lib_fn_phy_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_physic0(const uint32_t* words) { return typed_library_call<&lib_fn_physic0, 0, 4>(words); }
uint64_t amos_typed_lib_fn_physic1(const uint32_t* words) { return typed_library_call<&lib_fn_physic1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_point(const uint32_t* words) { return typed_library_call<&lib_fn_point, 2, 4>(words); }
uint64_t amos_typed_lib_fn_rain(const uint32_t* words) { return typed_library_call<&lib_fn_rain, 2, 4>(words); }
uint64_t amos_typed_lib_fn_scin2(const uint32_t* words) { return typed_library_call<&lib_fn_scin2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_scin3(const uint32_t* words) { return typed_library_call<&lib_fn_scin3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_screen(const uint32_t* words) { return typed_library_call<&lib_fn_screen, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_base(const uint32_t* words) { return typed_library_call<&lib_fn_screen_base, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_colour(const uint32_t* words) { return typed_library_call<&lib_fn_screen_colour, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_height0(const uint32_t* words) { return typed_library_call<&lib_fn_screen_height0, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_height1(const uint32_t* words) { return typed_library_call<&lib_fn_screen_height1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_screen_mode(const uint32_t* words) { return typed_library_call<&lib_fn_screen_mode, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_width0(const uint32_t* words) { return typed_library_call<&lib_fn_screen_width0, 0, 4>(words); }
uint64_t amos_typed_lib_fn_screen_width1(const uint32_t* words) { return typed_library_call<&lib_fn_screen_width1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_text_base(const uint32_t* words) { return typed_library_call<&lib_fn_text_base, 0, 4>(words); }
uint64_t amos_typed_lib_fn_text_length(const uint32_t* words) { return typed_library_call<&lib_fn_text_length, 1, 4>(words); }
uint64_t amos_typed_lib_fn_text_style(const uint32_t* words) { return typed_library_call<&lib_fn_text_style, 0, 4>(words); }
uint64_t amos_typed_lib_fn_xgr(const uint32_t* words) { return typed_library_call<&lib_fn_xgr, 0, 4>(words); }
uint64_t amos_typed_lib_fn_xgraphic(const uint32_t* words) { return typed_library_call<&lib_fn_xgraphic, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xhard1(const uint32_t* words) { return typed_library_call<&lib_fn_xhard1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xhard2(const uint32_t* words) { return typed_library_call<&lib_fn_xhard2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_xscreen1(const uint32_t* words) { return typed_library_call<&lib_fn_xscreen1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xscreen2(const uint32_t* words) { return typed_library_call<&lib_fn_xscreen2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_xtext(const uint32_t* words) { return typed_library_call<&lib_fn_xtext, 1, 4>(words); }
uint64_t amos_typed_lib_fn_ygr(const uint32_t* words) { return typed_library_call<&lib_fn_ygr, 0, 4>(words); }
uint64_t amos_typed_lib_fn_ygraphic(const uint32_t* words) { return typed_library_call<&lib_fn_ygraphic, 1, 4>(words); }
uint64_t amos_typed_lib_fn_yhard1(const uint32_t* words) { return typed_library_call<&lib_fn_yhard1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_yhard2(const uint32_t* words) { return typed_library_call<&lib_fn_yhard2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_yscreen1(const uint32_t* words) { return typed_library_call<&lib_fn_yscreen1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_yscreen2(const uint32_t* words) { return typed_library_call<&lib_fn_yscreen2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_ytext(const uint32_t* words) { return typed_library_call<&lib_fn_ytext, 1, 4>(words); }
uint64_t amos_typed_lib_fn_zone2(const uint32_t* words) { return typed_library_call<&lib_fn_zone2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_zone3(const uint32_t* words) { return typed_library_call<&lib_fn_zone3, 3, 4>(words); }
uint32_t amos_typed_lib_free_ras(RegFrame& r) { return detail::typed_reg_call<&lib_free_ras>(r); }
uint32_t amos_typed_lib_get_ec(RegFrame& r) { return detail::typed_reg_call<&lib_get_ec>(r); }
uint32_t amos_typed_lib_get_ras(RegFrame& r) { return detail::typed_reg_call<&lib_get_ras>(r); }
uint32_t amos_typed_lib_gfx_func(RegFrame& r) { return detail::typed_reg_call<&lib_gfx_func>(r); }
uint64_t amos_typed_lib_in_amos_lock(const uint32_t* words) { return typed_library_call<&lib_in_amos_lock, 0, 0>(words); }
uint64_t amos_typed_lib_in_amos_to_back(const uint32_t* words) { return typed_library_call<&lib_in_amos_to_back, 0, 0>(words); }
uint64_t amos_typed_lib_in_amos_to_front(const uint32_t* words) { return typed_library_call<&lib_in_amos_to_front, 0, 0>(words); }
uint64_t amos_typed_lib_in_amos_unlock(const uint32_t* words) { return typed_library_call<&lib_in_amos_unlock, 0, 0>(words); }
uint64_t amos_typed_lib_in_appear3(const uint32_t* words) { return typed_library_call<&lib_in_appear3, 3, 0>(words); }
uint64_t amos_typed_lib_in_appear4(const uint32_t* words) { return typed_library_call<&lib_in_appear4, 4, 0>(words); }
uint64_t amos_typed_lib_in_auto_view_off(const uint32_t* words) { return typed_library_call<&lib_in_auto_view_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_auto_view_on(const uint32_t* words) { return typed_library_call<&lib_in_auto_view_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_autoback(const uint32_t* words) { return typed_library_call<&lib_in_autoback, 1, 0>(words); }
uint64_t amos_typed_lib_in_bar(const uint32_t* words) { return typed_library_call<&lib_in_bar, 4, 0>(words); }
uint64_t amos_typed_lib_in_box(const uint32_t* words) { return typed_library_call<&lib_in_box, 4, 0>(words); }
uint64_t amos_typed_lib_in_circle(const uint32_t* words) { return typed_library_call<&lib_in_circle, 3, 0>(words); }
uint64_t amos_typed_lib_in_clip0(const uint32_t* words) { return typed_library_call<&lib_in_clip0, 0, 0>(words); }
uint64_t amos_typed_lib_in_clip4(const uint32_t* words) { return typed_library_call<&lib_in_clip4, 4, 0>(words); }
uint64_t amos_typed_lib_in_close_workbench(const uint32_t* words) { return typed_library_call<&lib_in_close_workbench, 0, 0>(words); }
uint64_t amos_typed_lib_in_cls0(const uint32_t* words) { return typed_library_call<&lib_in_cls0, 0, 0>(words); }
uint64_t amos_typed_lib_in_cls1(const uint32_t* words) { return typed_library_call<&lib_in_cls1, 1, 0>(words); }
uint64_t amos_typed_lib_in_cls5(const uint32_t* words) { return typed_library_call<&lib_in_cls5, 5, 0>(words); }
uint64_t amos_typed_lib_in_colour(const uint32_t* words) { return typed_library_call<&lib_in_colour, 2, 0>(words); }
uint64_t amos_typed_lib_in_colour_back(const uint32_t* words) { return typed_library_call<&lib_in_colour_back, 1, 0>(words); }
uint64_t amos_typed_lib_in_cop_move(const uint32_t* words) { return typed_library_call<&lib_in_cop_move, 2, 0>(words); }
uint64_t amos_typed_lib_in_cop_movel(const uint32_t* words) { return typed_library_call<&lib_in_cop_movel, 2, 0>(words); }
uint64_t amos_typed_lib_in_cop_off(const uint32_t* words) { return typed_library_call<&lib_in_cop_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_cop_on(const uint32_t* words) { return typed_library_call<&lib_in_cop_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_cop_reset(const uint32_t* words) { return typed_library_call<&lib_in_cop_reset, 0, 0>(words); }
uint64_t amos_typed_lib_in_cop_swap(const uint32_t* words) { return typed_library_call<&lib_in_cop_swap, 0, 0>(words); }
uint64_t amos_typed_lib_in_cop_wait2(const uint32_t* words) { return typed_library_call<&lib_in_cop_wait2, 2, 0>(words); }
uint64_t amos_typed_lib_in_cop_wait4(const uint32_t* words) { return typed_library_call<&lib_in_cop_wait4, 4, 0>(words); }
uint64_t amos_typed_lib_in_def_scroll(const uint32_t* words) { return typed_library_call<&lib_in_def_scroll, 7, 0>(words); }
uint64_t amos_typed_lib_in_default(const uint32_t* words) { return typed_library_call<&lib_in_default, 0, 0>(words); }
uint64_t amos_typed_lib_in_del_block0(const uint32_t* words) { return typed_library_call<&lib_in_del_block0, 0, 0>(words); }
uint64_t amos_typed_lib_in_del_block1(const uint32_t* words) { return typed_library_call<&lib_in_del_block1, 1, 0>(words); }
uint64_t amos_typed_lib_in_del_cblock0(const uint32_t* words) { return typed_library_call<&lib_in_del_cblock0, 0, 0>(words); }
uint64_t amos_typed_lib_in_del_cblock1(const uint32_t* words) { return typed_library_call<&lib_in_del_cblock1, 1, 0>(words); }
uint64_t amos_typed_lib_in_double_buffer(const uint32_t* words) { return typed_library_call<&lib_in_double_buffer, 0, 0>(words); }
uint64_t amos_typed_lib_in_draw(const uint32_t* words) { return typed_library_call<&lib_in_draw, 4, 0>(words); }
uint64_t amos_typed_lib_in_draw_to(const uint32_t* words) { return typed_library_call<&lib_in_draw_to, 2, 0>(words); }
uint64_t amos_typed_lib_in_dual_playfield(const uint32_t* words) { return typed_library_call<&lib_in_dual_playfield, 2, 0>(words); }
uint64_t amos_typed_lib_in_dual_priority(const uint32_t* words) { return typed_library_call<&lib_in_dual_priority, 2, 0>(words); }
uint64_t amos_typed_lib_in_ellipse(const uint32_t* words) { return typed_library_call<&lib_in_ellipse, 4, 0>(words); }
uint64_t amos_typed_lib_in_flash(const uint32_t* words) { return typed_library_call<&lib_in_flash, 2, 0>(words); }
uint64_t amos_typed_lib_in_flash_off(const uint32_t* words) { return typed_library_call<&lib_in_flash_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_block5(const uint32_t* words) { return typed_library_call<&lib_in_get_block5, 5, 0>(words); }
uint64_t amos_typed_lib_in_get_block6(const uint32_t* words) { return typed_library_call<&lib_in_get_block6, 6, 0>(words); }
uint64_t amos_typed_lib_in_get_cblock(const uint32_t* words) { return typed_library_call<&lib_in_get_cblock, 5, 0>(words); }
uint64_t amos_typed_lib_in_get_disc_fonts(const uint32_t* words) { return typed_library_call<&lib_in_get_disc_fonts, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_fonts(const uint32_t* words) { return typed_library_call<&lib_in_get_fonts, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_icon_palette0(const uint32_t* words) { return typed_library_call<&lib_in_get_icon_palette0, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_icon_palette1(const uint32_t* words) { return typed_library_call<&lib_in_get_icon_palette1, 1, 0>(words); }
uint64_t amos_typed_lib_in_get_palette1(const uint32_t* words) { return typed_library_call<&lib_in_get_palette1, 1, 0>(words); }
uint64_t amos_typed_lib_in_get_palette2(const uint32_t* words) { return typed_library_call<&lib_in_get_palette2, 2, 0>(words); }
uint64_t amos_typed_lib_in_get_rom_fonts(const uint32_t* words) { return typed_library_call<&lib_in_get_rom_fonts, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_sprite_palette0(const uint32_t* words) { return typed_library_call<&lib_in_get_sprite_palette0, 0, 0>(words); }
uint64_t amos_typed_lib_in_get_sprite_palette1(const uint32_t* words) { return typed_library_call<&lib_in_get_sprite_palette1, 1, 0>(words); }
uint64_t amos_typed_lib_in_gr_locate(const uint32_t* words) { return typed_library_call<&lib_in_gr_locate, 2, 0>(words); }
uint64_t amos_typed_lib_in_gr_writing(const uint32_t* words) { return typed_library_call<&lib_in_gr_writing, 1, 0>(words); }
uint64_t amos_typed_lib_in_hrev_block(const uint32_t* words) { return typed_library_call<&lib_in_hrev_block, 1, 0>(words); }
uint64_t amos_typed_lib_in_hslider(const uint32_t* words) { return typed_library_call<&lib_in_hslider, 7, 0>(words); }
uint64_t amos_typed_lib_in_ink1(const uint32_t* words) { return typed_library_call<&lib_in_ink1, 1, 0>(words); }
uint64_t amos_typed_lib_in_ink2(const uint32_t* words) { return typed_library_call<&lib_in_ink2, 2, 0>(words); }
uint64_t amos_typed_lib_in_ink3(const uint32_t* words) { return typed_library_call<&lib_in_ink3, 3, 0>(words); }
uint64_t amos_typed_lib_in_paint2(const uint32_t* words) { return typed_library_call<&lib_in_paint2, 2, 0>(words); }
uint64_t amos_typed_lib_in_paint3(const uint32_t* words) { return typed_library_call<&lib_in_paint3, 3, 0>(words); }
uint64_t amos_typed_lib_in_plot2(const uint32_t* words) { return typed_library_call<&lib_in_plot2, 2, 0>(words); }
uint64_t amos_typed_lib_in_plot3(const uint32_t* words) { return typed_library_call<&lib_in_plot3, 3, 0>(words); }
uint64_t amos_typed_lib_in_put_block1(const uint32_t* words) { return typed_library_call<&lib_in_put_block1, 1, 0>(words); }
uint64_t amos_typed_lib_in_put_block3(const uint32_t* words) { return typed_library_call<&lib_in_put_block3, 3, 0>(words); }
uint64_t amos_typed_lib_in_put_block4(const uint32_t* words) { return typed_library_call<&lib_in_put_block4, 4, 0>(words); }
uint64_t amos_typed_lib_in_put_block5(const uint32_t* words) { return typed_library_call<&lib_in_put_block5, 5, 0>(words); }
uint64_t amos_typed_lib_in_put_cblock1(const uint32_t* words) { return typed_library_call<&lib_in_put_cblock1, 1, 0>(words); }
uint64_t amos_typed_lib_in_put_cblock3(const uint32_t* words) { return typed_library_call<&lib_in_put_cblock3, 3, 0>(words); }
uint64_t amos_typed_lib_in_rain(const uint32_t* words) { return typed_library_call<&lib_in_rain, 3, 0>(words); }
uint64_t amos_typed_lib_in_rainbow(const uint32_t* words) { return typed_library_call<&lib_in_rainbow, 4, 0>(words); }
uint64_t amos_typed_lib_in_rainbow_del0(const uint32_t* words) { return typed_library_call<&lib_in_rainbow_del0, 0, 0>(words); }
uint64_t amos_typed_lib_in_rainbow_del1(const uint32_t* words) { return typed_library_call<&lib_in_rainbow_del1, 1, 0>(words); }
uint64_t amos_typed_lib_in_reserve_zone0(const uint32_t* words) { return typed_library_call<&lib_in_reserve_zone0, 0, 0>(words); }
uint64_t amos_typed_lib_in_reserve_zone1(const uint32_t* words) { return typed_library_call<&lib_in_reserve_zone1, 1, 0>(words); }
uint64_t amos_typed_lib_in_reset_zone0(const uint32_t* words) { return typed_library_call<&lib_in_reset_zone0, 0, 0>(words); }
uint64_t amos_typed_lib_in_reset_zone1(const uint32_t* words) { return typed_library_call<&lib_in_reset_zone1, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen(const uint32_t* words) { return typed_library_call<&lib_in_screen, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_clone(const uint32_t* words) { return typed_library_call<&lib_in_screen_clone, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_close(const uint32_t* words) { return typed_library_call<&lib_in_screen_close, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_copy2(const uint32_t* words) { return typed_library_call<&lib_in_screen_copy2, 2, 0>(words); }
uint64_t amos_typed_lib_in_screen_copy3(const uint32_t* words) { return typed_library_call<&lib_in_screen_copy3, 3, 0>(words); }
uint64_t amos_typed_lib_in_screen_copy8(const uint32_t* words) { return typed_library_call<&lib_in_screen_copy8, 8, 0>(words); }
uint64_t amos_typed_lib_in_screen_copy9(const uint32_t* words) { return typed_library_call<&lib_in_screen_copy9, 9, 0>(words); }
uint64_t amos_typed_lib_in_screen_display(const uint32_t* words) { return typed_library_call<&lib_in_screen_display, 5, 0>(words); }
uint64_t amos_typed_lib_in_screen_hide0(const uint32_t* words) { return typed_library_call<&lib_in_screen_hide0, 0, 0>(words); }
uint64_t amos_typed_lib_in_screen_hide1(const uint32_t* words) { return typed_library_call<&lib_in_screen_hide1, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_offset(const uint32_t* words) { return typed_library_call<&lib_in_screen_offset, 3, 0>(words); }
uint64_t amos_typed_lib_in_screen_open(const uint32_t* words) { return typed_library_call<&lib_in_screen_open, 5, 0>(words); }
uint64_t amos_typed_lib_in_screen_show0(const uint32_t* words) { return typed_library_call<&lib_in_screen_show0, 0, 0>(words); }
uint64_t amos_typed_lib_in_screen_show1(const uint32_t* words) { return typed_library_call<&lib_in_screen_show1, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_swap0(const uint32_t* words) { return typed_library_call<&lib_in_screen_swap0, 0, 0>(words); }
uint64_t amos_typed_lib_in_screen_swap1(const uint32_t* words) { return typed_library_call<&lib_in_screen_swap1, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_to_back0(const uint32_t* words) { return typed_library_call<&lib_in_screen_to_back0, 0, 0>(words); }
uint64_t amos_typed_lib_in_screen_to_back1(const uint32_t* words) { return typed_library_call<&lib_in_screen_to_back1, 1, 0>(words); }
uint64_t amos_typed_lib_in_screen_to_front0(const uint32_t* words) { return typed_library_call<&lib_in_screen_to_front0, 0, 0>(words); }
uint64_t amos_typed_lib_in_screen_to_front1(const uint32_t* words) { return typed_library_call<&lib_in_screen_to_front1, 1, 0>(words); }
uint64_t amos_typed_lib_in_scroll(const uint32_t* words) { return typed_library_call<&lib_in_scroll, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_font(const uint32_t* words) { return typed_library_call<&lib_in_set_font, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_line(const uint32_t* words) { return typed_library_call<&lib_in_set_line, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_paint(const uint32_t* words) { return typed_library_call<&lib_in_set_paint, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_pattern(const uint32_t* words) { return typed_library_call<&lib_in_set_pattern, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_rainbow6(const uint32_t* words) { return typed_library_call<&lib_in_set_rainbow6, 6, 0>(words); }
uint64_t amos_typed_lib_in_set_rainbow7(const uint32_t* words) { return typed_library_call<&lib_in_set_rainbow7, 7, 0>(words); }
uint64_t amos_typed_lib_in_set_slider(const uint32_t* words) { return typed_library_call<&lib_in_set_slider, 8, 0>(words); }
uint64_t amos_typed_lib_in_set_tempras0(const uint32_t* words) { return typed_library_call<&lib_in_set_tempras0, 0, 0>(words); }
uint64_t amos_typed_lib_in_set_tempras1(const uint32_t* words) { return typed_library_call<&lib_in_set_tempras1, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_tempras2(const uint32_t* words) { return typed_library_call<&lib_in_set_tempras2, 2, 0>(words); }
uint64_t amos_typed_lib_in_set_text(const uint32_t* words) { return typed_library_call<&lib_in_set_text, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_zone(const uint32_t* words) { return typed_library_call<&lib_in_set_zone, 5, 0>(words); }
uint64_t amos_typed_lib_in_shift_down(const uint32_t* words) { return typed_library_call<&lib_in_shift_down, 4, 0>(words); }
uint64_t amos_typed_lib_in_shift_off(const uint32_t* words) { return typed_library_call<&lib_in_shift_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_shift_up(const uint32_t* words) { return typed_library_call<&lib_in_shift_up, 4, 0>(words); }
uint64_t amos_typed_lib_in_text(const uint32_t* words) { return typed_library_call<&lib_in_text, 3, 0>(words); }
uint64_t amos_typed_lib_in_view(const uint32_t* words) { return typed_library_call<&lib_in_view, 0, 0>(words); }
uint64_t amos_typed_lib_in_vrev_block(const uint32_t* words) { return typed_library_call<&lib_in_vrev_block, 1, 0>(words); }
uint64_t amos_typed_lib_in_vslider(const uint32_t* words) { return typed_library_call<&lib_in_vslider, 7, 0>(words); }
uint64_t amos_typed_lib_in_zoom(const uint32_t* words) { return typed_library_call<&lib_in_zoom, 10, 0>(words); }
uint32_t amos_typed_lib_pal_rout(RegFrame& r) { return detail::typed_reg_call<&lib_pal_rout>(r); }
uint32_t amos_typed_lib_sco0(RegFrame& r) { return detail::typed_reg_call<&lib_sco0>(r); }
uint32_t amos_typed_lib_wb_close(RegFrame& r) { return detail::typed_reg_call<&lib_wb_close>(r); }
uint32_t amos_typed_lib_wb_open(RegFrame& r) { return detail::typed_reg_call<&lib_wb_open>(r); }
}
}
