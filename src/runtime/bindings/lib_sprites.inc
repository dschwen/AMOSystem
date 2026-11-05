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
uint64_t amos_typed_lib_fn_bob_col1(const uint32_t* words) { return typed_library_call<&lib_fn_bob_col1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_bob_col3(const uint32_t* words) { return typed_library_call<&lib_fn_bob_col3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_bob_sprite_col1(const uint32_t* words) { return typed_library_call<&lib_fn_bob_sprite_col1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_bob_sprite_col3(const uint32_t* words) { return typed_library_call<&lib_fn_bob_sprite_col3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_col(const uint32_t* words) { return typed_library_call<&lib_fn_col, 1, 4>(words); }
uint64_t amos_typed_lib_fn_hardcol(const uint32_t* words) { return typed_library_call<&lib_fn_hardcol, 1, 4>(words); }
uint64_t amos_typed_lib_fn_hrev(const uint32_t* words) { return typed_library_call<&lib_fn_hrev, 1, 4>(words); }
uint64_t amos_typed_lib_fn_ibob(const uint32_t* words) { return typed_library_call<&lib_fn_ibob, 1, 4>(words); }
uint64_t amos_typed_lib_fn_icon_base(const uint32_t* words) { return typed_library_call<&lib_fn_icon_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_isprite(const uint32_t* words) { return typed_library_call<&lib_fn_isprite, 1, 4>(words); }
uint64_t amos_typed_lib_fn_mouse_click(const uint32_t* words) { return typed_library_call<&lib_fn_mouse_click, 0, 4>(words); }
uint64_t amos_typed_lib_fn_mouse_key(const uint32_t* words) { return typed_library_call<&lib_fn_mouse_key, 0, 4>(words); }
uint64_t amos_typed_lib_fn_rev(const uint32_t* words) { return typed_library_call<&lib_fn_rev, 1, 4>(words); }
uint64_t amos_typed_lib_fn_sprite_base(const uint32_t* words) { return typed_library_call<&lib_fn_sprite_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_sprite_bob_col1(const uint32_t* words) { return typed_library_call<&lib_fn_sprite_bob_col1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_sprite_bob_col3(const uint32_t* words) { return typed_library_call<&lib_fn_sprite_bob_col3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_sprite_col1(const uint32_t* words) { return typed_library_call<&lib_fn_sprite_col1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_sprite_col3(const uint32_t* words) { return typed_library_call<&lib_fn_sprite_col3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_vrev(const uint32_t* words) { return typed_library_call<&lib_fn_vrev, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xbob(const uint32_t* words) { return typed_library_call<&lib_fn_xbob, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xmouse(const uint32_t* words) { return typed_library_call<&lib_fn_xmouse, 0, 4>(words); }
uint64_t amos_typed_lib_fn_xsprite(const uint32_t* words) { return typed_library_call<&lib_fn_xsprite, 1, 4>(words); }
uint64_t amos_typed_lib_fn_ybob(const uint32_t* words) { return typed_library_call<&lib_fn_ybob, 1, 4>(words); }
uint64_t amos_typed_lib_fn_ymouse(const uint32_t* words) { return typed_library_call<&lib_fn_ymouse, 0, 4>(words); }
uint64_t amos_typed_lib_fn_ysprite(const uint32_t* words) { return typed_library_call<&lib_fn_ysprite, 1, 4>(words); }
uint64_t amos_typed_lib_in_bob(const uint32_t* words) { return typed_library_call<&lib_in_bob, 4, 0>(words); }
uint64_t amos_typed_lib_in_bob_clear(const uint32_t* words) { return typed_library_call<&lib_in_bob_clear, 0, 0>(words); }
uint64_t amos_typed_lib_in_bob_draw(const uint32_t* words) { return typed_library_call<&lib_in_bob_draw, 0, 0>(words); }
uint64_t amos_typed_lib_in_bob_off0(const uint32_t* words) { return typed_library_call<&lib_in_bob_off0, 0, 0>(words); }
uint64_t amos_typed_lib_in_bob_off1(const uint32_t* words) { return typed_library_call<&lib_in_bob_off1, 1, 0>(words); }
uint64_t amos_typed_lib_in_bob_update(const uint32_t* words) { return typed_library_call<&lib_in_bob_update, 0, 0>(words); }
uint64_t amos_typed_lib_in_bob_update_off(const uint32_t* words) { return typed_library_call<&lib_in_bob_update_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_bob_update_on(const uint32_t* words) { return typed_library_call<&lib_in_bob_update_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_change_mouse(const uint32_t* words) { return typed_library_call<&lib_in_change_mouse, 1, 0>(words); }
uint64_t amos_typed_lib_in_get_icon5(const uint32_t* words) { return typed_library_call<&lib_in_get_icon5, 5, 0>(words); }
uint64_t amos_typed_lib_in_get_icon6(const uint32_t* words) { return typed_library_call<&lib_in_get_icon6, 6, 0>(words); }
uint64_t amos_typed_lib_in_get_sprite5(const uint32_t* words) { return typed_library_call<&lib_in_get_sprite5, 5, 0>(words); }
uint64_t amos_typed_lib_in_get_sprite6(const uint32_t* words) { return typed_library_call<&lib_in_get_sprite6, 6, 0>(words); }
uint64_t amos_typed_lib_in_hide(const uint32_t* words) { return typed_library_call<&lib_in_hide, 0, 0>(words); }
uint64_t amos_typed_lib_in_hide_on(const uint32_t* words) { return typed_library_call<&lib_in_hide_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_hot_spot2(const uint32_t* words) { return typed_library_call<&lib_in_hot_spot2, 2, 0>(words); }
uint64_t amos_typed_lib_in_hot_spot3(const uint32_t* words) { return typed_library_call<&lib_in_hot_spot3, 3, 0>(words); }
uint64_t amos_typed_lib_in_limit_bob0(const uint32_t* words) { return typed_library_call<&lib_in_limit_bob0, 0, 0>(words); }
uint64_t amos_typed_lib_in_limit_bob4(const uint32_t* words) { return typed_library_call<&lib_in_limit_bob4, 4, 0>(words); }
uint64_t amos_typed_lib_in_limit_bob5(const uint32_t* words) { return typed_library_call<&lib_in_limit_bob5, 5, 0>(words); }
uint64_t amos_typed_lib_in_limit_mouse0(const uint32_t* words) { return typed_library_call<&lib_in_limit_mouse0, 0, 0>(words); }
uint64_t amos_typed_lib_in_limit_mouse1(const uint32_t* words) { return typed_library_call<&lib_in_limit_mouse1, 1, 0>(words); }
uint64_t amos_typed_lib_in_limit_mouse4(const uint32_t* words) { return typed_library_call<&lib_in_limit_mouse4, 4, 0>(words); }
uint64_t amos_typed_lib_in_make_icon_mask0(const uint32_t* words) { return typed_library_call<&lib_in_make_icon_mask0, 0, 0>(words); }
uint64_t amos_typed_lib_in_make_icon_mask1(const uint32_t* words) { return typed_library_call<&lib_in_make_icon_mask1, 1, 0>(words); }
uint64_t amos_typed_lib_in_make_mask0(const uint32_t* words) { return typed_library_call<&lib_in_make_mask0, 0, 0>(words); }
uint64_t amos_typed_lib_in_make_mask1(const uint32_t* words) { return typed_library_call<&lib_in_make_mask1, 1, 0>(words); }
uint64_t amos_typed_lib_in_no_icon_mask0(const uint32_t* words) { return typed_library_call<&lib_in_no_icon_mask0, 0, 0>(words); }
uint64_t amos_typed_lib_in_no_icon_mask1(const uint32_t* words) { return typed_library_call<&lib_in_no_icon_mask1, 1, 0>(words); }
uint64_t amos_typed_lib_in_no_mask0(const uint32_t* words) { return typed_library_call<&lib_in_no_mask0, 0, 0>(words); }
uint64_t amos_typed_lib_in_no_mask1(const uint32_t* words) { return typed_library_call<&lib_in_no_mask1, 1, 0>(words); }
uint64_t amos_typed_lib_in_paste_bob(const uint32_t* words) { return typed_library_call<&lib_in_paste_bob, 3, 0>(words); }
uint64_t amos_typed_lib_in_paste_icon(const uint32_t* words) { return typed_library_call<&lib_in_paste_icon, 3, 0>(words); }
uint64_t amos_typed_lib_in_priority_off(const uint32_t* words) { return typed_library_call<&lib_in_priority_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_priority_on(const uint32_t* words) { return typed_library_call<&lib_in_priority_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_priority_reverse_off(const uint32_t* words) { return typed_library_call<&lib_in_priority_reverse_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_priority_reverse_on(const uint32_t* words) { return typed_library_call<&lib_in_priority_reverse_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_put_bob(const uint32_t* words) { return typed_library_call<&lib_in_put_bob, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_bob(const uint32_t* words) { return typed_library_call<&lib_in_set_bob, 4, 0>(words); }
uint64_t amos_typed_lib_in_set_hardcol(const uint32_t* words) { return typed_library_call<&lib_in_set_hardcol, 2, 0>(words); }
uint64_t amos_typed_lib_in_set_sprite_buffer(const uint32_t* words) { return typed_library_call<&lib_in_set_sprite_buffer, 1, 0>(words); }
uint64_t amos_typed_lib_in_show(const uint32_t* words) { return typed_library_call<&lib_in_show, 0, 0>(words); }
uint64_t amos_typed_lib_in_show_on(const uint32_t* words) { return typed_library_call<&lib_in_show_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_sprite(const uint32_t* words) { return typed_library_call<&lib_in_sprite, 4, 0>(words); }
uint64_t amos_typed_lib_in_sprite_off0(const uint32_t* words) { return typed_library_call<&lib_in_sprite_off0, 0, 0>(words); }
uint64_t amos_typed_lib_in_sprite_off1(const uint32_t* words) { return typed_library_call<&lib_in_sprite_off1, 1, 0>(words); }
uint64_t amos_typed_lib_in_sprite_priority(const uint32_t* words) { return typed_library_call<&lib_in_sprite_priority, 1, 0>(words); }
uint64_t amos_typed_lib_in_sprite_update(const uint32_t* words) { return typed_library_call<&lib_in_sprite_update, 0, 0>(words); }
uint64_t amos_typed_lib_in_sprite_update_off(const uint32_t* words) { return typed_library_call<&lib_in_sprite_update_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_sprite_update_on(const uint32_t* words) { return typed_library_call<&lib_in_sprite_update_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_update(const uint32_t* words) { return typed_library_call<&lib_in_update, 0, 0>(words); }
uint64_t amos_typed_lib_in_update_every(const uint32_t* words) { return typed_library_call<&lib_in_update_every, 1, 0>(words); }
uint64_t amos_typed_lib_in_update_off(const uint32_t* words) { return typed_library_call<&lib_in_update_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_update_on(const uint32_t* words) { return typed_library_call<&lib_in_update_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_xmouse(const uint32_t* words) { return typed_library_call<&lib_in_xmouse, 1, 0>(words); }
uint64_t amos_typed_lib_in_ymouse(const uint32_t* words) { return typed_library_call<&lib_in_ymouse, 1, 0>(words); }
}
}
