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
void amos_typed_ed_delete_chunk(AllRegs* r) { ed_delete_chunk(r); }
void amos_typed_ed_delete_chunk_at(AllRegs* r) { ed_delete_chunk_at(r); }
void amos_typed_ed_delete_line(AllRegs* r) { ed_delete_line(r); }
void amos_typed_ed_find_line(AllRegs* r) { ed_find_line(r); }
void amos_typed_ed_next_line(AllRegs* r) { ed_next_line(r); }
void amos_typed_ed_store(AllRegs* r) { ed_store(r); }
void amos_typed_ed_store_block(AllRegs* r) { ed_store_block(r); }
}
}
