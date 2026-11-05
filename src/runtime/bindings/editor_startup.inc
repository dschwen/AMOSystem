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
void amos_typed_ed_close_editor_fn(AllRegs* r) { ed_close_editor_fn(r); }
void amos_typed_ed_cold(AllRegs* r) { ed_cold(r); }
void amos_typed_ed_draw_top_fn(AllRegs* r) { ed_draw_top_fn(r); }
void amos_typed_ed_end(AllRegs* r) { ed_end(r); }
void amos_typed_ed_kill_editor(AllRegs* r) { ed_kill_editor(r); }
void amos_typed_ed_open_editor_fn(AllRegs* r) { ed_open_editor_fn(r); }
void amos_typed_ed_title(AllRegs* r) { ed_title(r); }
void amos_typed_ed_tokens_end(AllRegs* r) { ed_tokens_end(r); }
void amos_typed_ed_tokens_init(AllRegs* r) { ed_tokens_init(r); }
}
}
