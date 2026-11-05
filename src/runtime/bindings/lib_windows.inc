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
uint64_t amos_typed_lib_fn_windon(const uint32_t* words) { return typed_library_call<&lib_fn_windon, 0, 4>(words); }
uint64_t amos_typed_lib_fn_xcurs(const uint32_t* words) { return typed_library_call<&lib_fn_xcurs, 0, 4>(words); }
uint64_t amos_typed_lib_fn_ycurs(const uint32_t* words) { return typed_library_call<&lib_fn_ycurs, 0, 4>(words); }
uint64_t amos_typed_lib_in_border(const uint32_t* words) { return typed_library_call<&lib_in_border, 3, 0>(words); }
uint64_t amos_typed_lib_in_cdown(const uint32_t* words) { return typed_library_call<&lib_in_cdown, 0, 0>(words); }
uint64_t amos_typed_lib_in_centre(const uint32_t* words) { return typed_library_call<&lib_in_centre, 1, 0>(words); }
uint64_t amos_typed_lib_in_cleft(const uint32_t* words) { return typed_library_call<&lib_in_cleft, 0, 0>(words); }
uint64_t amos_typed_lib_in_cline0(const uint32_t* words) { return typed_library_call<&lib_in_cline0, 0, 0>(words); }
uint64_t amos_typed_lib_in_cline1(const uint32_t* words) { return typed_library_call<&lib_in_cline1, 1, 0>(words); }
uint64_t amos_typed_lib_in_clw(const uint32_t* words) { return typed_library_call<&lib_in_clw, 0, 0>(words); }
uint64_t amos_typed_lib_in_cmove(const uint32_t* words) { return typed_library_call<&lib_in_cmove, 2, 0>(words); }
uint64_t amos_typed_lib_in_cright(const uint32_t* words) { return typed_library_call<&lib_in_cright, 0, 0>(words); }
uint64_t amos_typed_lib_in_cup(const uint32_t* words) { return typed_library_call<&lib_in_cup, 0, 0>(words); }
uint64_t amos_typed_lib_in_curs_off(const uint32_t* words) { return typed_library_call<&lib_in_curs_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_curs_on(const uint32_t* words) { return typed_library_call<&lib_in_curs_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_curs_pen(const uint32_t* words) { return typed_library_call<&lib_in_curs_pen, 1, 0>(words); }
uint64_t amos_typed_lib_in_home(const uint32_t* words) { return typed_library_call<&lib_in_home, 0, 0>(words); }
uint64_t amos_typed_lib_in_hscroll(const uint32_t* words) { return typed_library_call<&lib_in_hscroll, 1, 0>(words); }
uint64_t amos_typed_lib_in_inverse_off(const uint32_t* words) { return typed_library_call<&lib_in_inverse_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_inverse_on(const uint32_t* words) { return typed_library_call<&lib_in_inverse_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_locate(const uint32_t* words) { return typed_library_call<&lib_in_locate, 2, 0>(words); }
uint64_t amos_typed_lib_in_memorize_x(const uint32_t* words) { return typed_library_call<&lib_in_memorize_x, 0, 0>(words); }
uint64_t amos_typed_lib_in_memorize_y(const uint32_t* words) { return typed_library_call<&lib_in_memorize_y, 0, 0>(words); }
uint64_t amos_typed_lib_in_paper(const uint32_t* words) { return typed_library_call<&lib_in_paper, 1, 0>(words); }
uint64_t amos_typed_lib_in_pen(const uint32_t* words) { return typed_library_call<&lib_in_pen, 1, 0>(words); }
uint64_t amos_typed_lib_in_remember_x(const uint32_t* words) { return typed_library_call<&lib_in_remember_x, 0, 0>(words); }
uint64_t amos_typed_lib_in_remember_y(const uint32_t* words) { return typed_library_call<&lib_in_remember_y, 0, 0>(words); }
uint64_t amos_typed_lib_in_scroll_off(const uint32_t* words) { return typed_library_call<&lib_in_scroll_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_scroll_on(const uint32_t* words) { return typed_library_call<&lib_in_scroll_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_set_curs(const uint32_t* words) { return typed_library_call<&lib_in_set_curs, 8, 0>(words); }
uint64_t amos_typed_lib_in_set_tab(const uint32_t* words) { return typed_library_call<&lib_in_set_tab, 1, 0>(words); }
uint64_t amos_typed_lib_in_shade_off(const uint32_t* words) { return typed_library_call<&lib_in_shade_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_shade_on(const uint32_t* words) { return typed_library_call<&lib_in_shade_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_title_bottom(const uint32_t* words) { return typed_library_call<&lib_in_title_bottom, 1, 0>(words); }
uint64_t amos_typed_lib_in_title_top(const uint32_t* words) { return typed_library_call<&lib_in_title_top, 1, 0>(words); }
uint64_t amos_typed_lib_in_under_off(const uint32_t* words) { return typed_library_call<&lib_in_under_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_under_on(const uint32_t* words) { return typed_library_call<&lib_in_under_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_vscroll(const uint32_t* words) { return typed_library_call<&lib_in_vscroll, 1, 0>(words); }
uint64_t amos_typed_lib_in_windclose(const uint32_t* words) { return typed_library_call<&lib_in_windclose, 0, 0>(words); }
uint64_t amos_typed_lib_in_windmove(const uint32_t* words) { return typed_library_call<&lib_in_windmove, 2, 0>(words); }
uint64_t amos_typed_lib_in_windopen5(const uint32_t* words) { return typed_library_call<&lib_in_windopen5, 5, 0>(words); }
uint64_t amos_typed_lib_in_windopen6(const uint32_t* words) { return typed_library_call<&lib_in_windopen6, 6, 0>(words); }
uint64_t amos_typed_lib_in_windopen7(const uint32_t* words) { return typed_library_call<&lib_in_windopen7, 7, 0>(words); }
uint64_t amos_typed_lib_in_window(const uint32_t* words) { return typed_library_call<&lib_in_window, 1, 0>(words); }
uint64_t amos_typed_lib_in_windsave(const uint32_t* words) { return typed_library_call<&lib_in_windsave, 0, 0>(words); }
uint64_t amos_typed_lib_in_windsize(const uint32_t* words) { return typed_library_call<&lib_in_windsize, 2, 0>(words); }
uint64_t amos_typed_lib_in_writing1(const uint32_t* words) { return typed_library_call<&lib_in_writing1, 1, 0>(words); }
uint64_t amos_typed_lib_in_writing2(const uint32_t* words) { return typed_library_call<&lib_in_writing2, 2, 0>(words); }
}
}
