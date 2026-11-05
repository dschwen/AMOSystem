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
void amos_typed_ed_block_all(AllRegs* r) { ed_block_all(r); }
void amos_typed_ed_block_copy(AllRegs* r) { ed_block_copy(r); }
void amos_typed_ed_block_cursor(AllRegs* r) { ed_block_cursor(r); }
void amos_typed_ed_block_cut(AllRegs* r) { ed_block_cut(r); }
void amos_typed_ed_block_delete(AllRegs* r) { ed_block_delete(r); }
void amos_typed_ed_block_forget(AllRegs* r) { ed_block_forget(r); }
void amos_typed_ed_block_free(AllRegs* r) { ed_block_free(r); }
void amos_typed_ed_block_free_a0(AllRegs* r) { ed_block_free_a0(r); }
void amos_typed_ed_block_hide(AllRegs* r) { ed_block_hide(r); }
void amos_typed_ed_block_insert(AllRegs* r) { ed_block_insert(r); }
void amos_typed_ed_block_limits(AllRegs* r) { ed_block_limits(r); }
void amos_typed_ed_block_on(AllRegs* r) { ed_block_on(r); }
void amos_typed_ed_block_paste(AllRegs* r) { ed_block_paste(r); }
void amos_typed_ed_block_show(AllRegs* r) { ed_block_show(r); }
void amos_typed_ed_block_store(AllRegs* r) { ed_block_store(r); }
void amos_typed_ed_cursor_normal_fn(AllRegs* r) { ed_cursor_normal_fn(r); }
}
}
