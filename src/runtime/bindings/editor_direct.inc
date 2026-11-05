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
void amos_typed_ed_error_direct(AllRegs* r) { ed_error_direct(r); }
void amos_typed_ed_escape(AllRegs* r) { ed_escape(r); }
void amos_typed_ed_escape_appear(AllRegs* r) { ed_escape_appear(r); }
void amos_typed_ed_escape_display(AllRegs* r) { ed_escape_display(r); }
void amos_typed_ed_escape_display_d0(AllRegs* r) { ed_escape_display_d0(r); }
void amos_typed_ed_escape_forget(AllRegs* r) { ed_escape_forget(r); }
void amos_typed_ed_escape_forget_all(AllRegs* r) { ed_escape_forget_all(r); }
void amos_typed_ed_escape_hide(AllRegs* r) { ed_escape_hide(r); }
void amos_typed_ed_escape_leave(AllRegs* r) { ed_escape_leave(r); }
void amos_typed_ed_escape_loop(AllRegs* r) { ed_escape_loop(r); }
void amos_typed_ed_escape_max_mouse(AllRegs* r) { ed_escape_max_mouse(r); }
void amos_typed_ed_escape_output(AllRegs* r) { ed_escape_output(r); }
void amos_typed_ed_get_error(AllRegs* r) { ed_get_error(r); }
}
}
