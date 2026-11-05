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
void amos_typed_mon_all_buttons(AllRegs* r) { mon_all_buttons(r); }
void amos_typed_mon_button(AllRegs* r) { mon_button(r); }
void amos_typed_mon_change_screen(AllRegs* r) { mon_change_screen(r); }
void amos_typed_mon_hide(AllRegs* r) { mon_hide(r); }
void amos_typed_mon_mode_buttons(AllRegs* r) { mon_mode_buttons(r); }
void amos_typed_mon_mouse_free(AllRegs* r) { mon_mouse_free(r); }
void amos_typed_mon_move_down(AllRegs* r) { mon_move_down(r); }
void amos_typed_mon_move_left(AllRegs* r) { mon_move_left(r); }
void amos_typed_mon_move_right(AllRegs* r) { mon_move_right(r); }
void amos_typed_mon_move_up(AllRegs* r) { mon_move_up(r); }
void amos_typed_mon_recop(AllRegs* r) { mon_recop(r); }
void amos_typed_mon_screen_copy(AllRegs* r) { mon_screen_copy(r); }
void amos_typed_mon_see(AllRegs* r) { mon_see(r); }
void amos_typed_mon_set_break(AllRegs* r) { mon_set_break(r); }
void amos_typed_mon_set_evaluate(AllRegs* r) { mon_set_evaluate(r); }
void amos_typed_mon_set_help(AllRegs* r) { mon_set_help(r); }
void amos_typed_mon_show(AllRegs* r) { mon_show(r); }
void amos_typed_mon_stop_button(AllRegs* r) { mon_stop_button(r); }
void amos_typed_mon_tested(AllRegs* r) { mon_tested(r); }
void amos_typed_mon_tests_patch(AllRegs* r) { mon_tests_patch(r); }
void amos_typed_mon_to_back(AllRegs* r) { mon_to_back(r); }
void amos_typed_mon_to_front(AllRegs* r) { mon_to_front(r); }
void amos_typed_mon_unpack(AllRegs* r) { mon_unpack(r); }
void amos_typed_mon_view_clear(AllRegs* r) { mon_view_clear(r); }
void amos_typed_mon_view_info(AllRegs* r) { mon_view_info(r); }
void amos_typed_mon_view_next(AllRegs* r) { mon_view_next(r); }
void amos_typed_mon_view_show(AllRegs* r) { mon_view_show(r); }
void amos_typed_mon_view_sizes(AllRegs* r) { mon_view_sizes(r); }
}
}
