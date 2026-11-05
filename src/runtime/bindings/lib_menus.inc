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
uint64_t amos_typed_lib_fn_choice0(const uint32_t* words) { return typed_library_call<&lib_fn_choice0, 0, 4>(words); }
uint64_t amos_typed_lib_fn_choice1(const uint32_t* words) { return typed_library_call<&lib_fn_choice1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_xmenu(const uint32_t* words) { return typed_library_call<&lib_fn_xmenu, 0, 4>(words); }
uint64_t amos_typed_lib_fn_ymenu(const uint32_t* words) { return typed_library_call<&lib_fn_ymenu, 0, 4>(words); }
uint64_t amos_typed_lib_in_bank_to_menu(const uint32_t* words) { return typed_library_call<&lib_in_bank_to_menu, 1, 0>(words); }
uint64_t amos_typed_lib_in_menu_active(const uint32_t* words) { return typed_library_call<&lib_in_menu_active, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_bar(const uint32_t* words) { return typed_library_call<&lib_in_menu_bar, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_base(const uint32_t* words) { return typed_library_call<&lib_in_menu_base, 2, 0>(words); }
uint64_t amos_typed_lib_in_menu_calc(const uint32_t* words) { return typed_library_call<&lib_in_menu_calc, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_called(const uint32_t* words) { return typed_library_call<&lib_in_menu_called, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_inactive(const uint32_t* words) { return typed_library_call<&lib_in_menu_inactive, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_item_movable(const uint32_t* words) { return typed_library_call<&lib_in_menu_item_movable, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_item_static(const uint32_t* words) { return typed_library_call<&lib_in_menu_item_static, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_line(const uint32_t* words) { return typed_library_call<&lib_in_menu_line, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_link(const uint32_t* words) { return typed_library_call<&lib_in_menu_link, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_mouse_off(const uint32_t* words) { return typed_library_call<&lib_in_menu_mouse_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_mouse_on(const uint32_t* words) { return typed_library_call<&lib_in_menu_mouse_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_movable(const uint32_t* words) { return typed_library_call<&lib_in_menu_movable, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_off(const uint32_t* words) { return typed_library_call<&lib_in_menu_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_on(const uint32_t* words) { return typed_library_call<&lib_in_menu_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_once(const uint32_t* words) { return typed_library_call<&lib_in_menu_once, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_separate(const uint32_t* words) { return typed_library_call<&lib_in_menu_separate, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_static(const uint32_t* words) { return typed_library_call<&lib_in_menu_static, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_tline(const uint32_t* words) { return typed_library_call<&lib_in_menu_tline, 0, 0>(words); }
uint64_t amos_typed_lib_in_menu_to_bank(const uint32_t* words) { return typed_library_call<&lib_in_menu_to_bank, 1, 0>(words); }
uint64_t amos_typed_lib_in_on_menu_del(const uint32_t* words) { return typed_library_call<&lib_in_on_menu_del, 0, 0>(words); }
uint64_t amos_typed_lib_in_on_menu_on(const uint32_t* words) { return typed_library_call<&lib_in_on_menu_on, 0, 0>(words); }
void amos_typed_lib_menu_clear() { lib_menu_clear(); }
void amos_typed_lib_menu_error() { lib_menu_error(); }
uint32_t amos_typed_lib_menu_key_explore(RegFrame& r) { return detail::typed_reg_call<&lib_menu_key_explore>(r); }
uint32_t amos_typed_lib_menu_reset(RegFrame& r) { return detail::typed_reg_call<&lib_menu_reset>(r); }
uint32_t amos_typed_lib_mn_clear_var(RegFrame& r) { return detail::typed_reg_call<&lib_mn_clear_var>(r); }
uint32_t amos_typed_lib_mn_eff(RegFrame& r) { return detail::typed_reg_call<&lib_mn_eff>(r); }
uint32_t amos_typed_lib_mn_find(RegFrame& r) { return detail::typed_reg_call<&lib_mn_find>(r); }
uint32_t amos_typed_lib_mn_gere(RegFrame& r) { return detail::typed_reg_call<&lib_mn_gere>(r); }
uint32_t amos_typed_lib_mn_ins(RegFrame& r) { return detail::typed_reg_call<&lib_mn_ins>(r); }
uint32_t amos_typed_lib_mn_objet(RegFrame& r) { return detail::typed_reg_call<&lib_mn_objet>(r); }
uint32_t amos_typed_lib_mn_raz(RegFrame& r) { return detail::typed_reg_call<&lib_mn_raz>(r); }
uint32_t amos_typed_lib_omn_eff(RegFrame& r) { return detail::typed_reg_call<&lib_omn_eff>(r); }
}
}
