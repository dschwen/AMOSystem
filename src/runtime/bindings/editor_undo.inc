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
void amos_typed_ed_redo(AllRegs* r) { ed_redo(r); }
void amos_typed_ed_undo(AllRegs* r) { ed_undo(r); }
void amos_typed_ed_undo_advance(AllRegs* r) { ed_undo_advance(r); }
void amos_typed_ed_undo_clear(AllRegs* r) { ed_undo_clear(r); }
void amos_typed_ed_undo_create(AllRegs* r) { ed_undo_create(r); }
void amos_typed_ed_undo_create_all(AllRegs* r) { ed_undo_create_all(r); }
void amos_typed_ed_undo_free(AllRegs* r) { ed_undo_free(r); }
void amos_typed_ed_undo_free_all(AllRegs* r) { ed_undo_free_all(r); }
uint32_t amos_typed_ed_undo_line(AllRegs* r) { return uint32_t(ed_undo_line(r)); }
void amos_typed_ed_undo_next(AllRegs* r) { ed_undo_next(r); }
void amos_typed_ed_undo_previous(AllRegs* r) { ed_undo_previous(r); }
void amos_typed_ed_undo_reset(AllRegs* r) { ed_undo_reset(r); }
void amos_typed_ed_undo_reset_all(AllRegs* r) { ed_undo_reset_all(r); }
uint32_t amos_typed_ed_undo_start(AllRegs* r) { return uint32_t(ed_undo_start(r)); }
}
}
