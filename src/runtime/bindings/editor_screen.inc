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
void amos_typed_ed_appear_fn(AllRegs* r) { ed_appear_fn(r); }
void amos_typed_ed_hide_fn(AllRegs* r) { ed_hide_fn(r); }
void amos_typed_ed_limit_mouse(AllRegs* r) { ed_limit_mouse(r); }
void amos_typed_ed_memory_draw(AllRegs* r) { ed_memory_draw(r); }
void amos_typed_ed_memory_draw_title(AllRegs* r) { ed_memory_draw_title(r); }
void amos_typed_ed_memory_show(AllRegs* r) { ed_memory_show(r); }
void amos_typed_ed_recop_fn(AllRegs* r) { ed_recop_fn(r); }
void amos_typed_ed_redraw(AllRegs* r) { ed_redraw(r); }
void amos_typed_ed_unpack_fn(AllRegs* r) { ed_unpack_fn(r); }
void amos_typed_ed_unpack_title(AllRegs* r) { ed_unpack_title(r); }
}
}
