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
void amos_typed_ed_button_draw(AllRegs* r) { ed_button_draw(r); }
void amos_typed_ed_button_draw_title(AllRegs* r) { ed_button_draw_title(r); }
void amos_typed_ed_close_fn(AllRegs* r) { ed_close_fn(r); }
void amos_typed_ed_close_name(AllRegs* r) { ed_close_name(r); }
void amos_typed_ed_disk_error(AllRegs* r) { ed_disk_error(r); }
void amos_typed_ed_enlarge_fn(AllRegs* r) { ed_enlarge_fn(r); }
void amos_typed_ed_load_error(AllRegs* r) { ed_load_error(r); }
void amos_typed_ed_print_error_fn(AllRegs* r) { ed_print_error_fn(r); }
void amos_typed_ed_set_tab(AllRegs* r) { ed_set_tab(r); }
void amos_typed_ed_slider_draw(AllRegs* r) { ed_slider_draw(r); }
void amos_typed_ed_slider_update(AllRegs* r) { ed_slider_update(r); }
void amos_typed_ed_workbench(AllRegs* r) { ed_workbench(r); }
}
}
