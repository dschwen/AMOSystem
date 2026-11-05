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
void amos_typed_ed_centre(AllRegs* r) { ed_centre(r); }
void amos_typed_ed_cursor_off(AllRegs* r) { ed_cursor_off(r); }
void amos_typed_ed_cursor_on(AllRegs* r) { ed_cursor_on(r); }
void amos_typed_ed_goto_x_fn(AllRegs* r) { ed_goto_x_fn(r); }
void amos_typed_ed_goto_y_fn(AllRegs* r) { ed_goto_y_fn(r); }
void amos_typed_ed_labels_around(AllRegs* r) { ed_labels_around(r); }
void amos_typed_ed_new_buffer(AllRegs* r) { ed_new_buffer(r); }
void amos_typed_ed_new_buffer_all(AllRegs* r) { ed_new_buffer_all(r); }
void amos_typed_ed_place_cursor(AllRegs* r) { ed_place_cursor(r); }
void amos_typed_ed_set_x(AllRegs* r) { ed_set_x(r); }
void amos_typed_ed_set_xy(AllRegs* r) { ed_set_xy(r); }
void amos_typed_ed_set_y(AllRegs* r) { ed_set_y(r); }
void amos_typed_ed_show_all(AllRegs* r) { ed_show_all(r); }
void amos_typed_ed_show_column(AllRegs* r) { ed_show_column(r); }
void amos_typed_ed_show_current_line(AllRegs* r) { ed_show_current_line(r); }
void amos_typed_ed_show_line(AllRegs* r) { ed_show_line(r); }
void amos_typed_ed_show_line_only(AllRegs* r) { ed_show_line_only(r); }
void amos_typed_ed_show_slider(AllRegs* r) { ed_show_slider(r); }
}
}
