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
void amos_typed_ed_load(AllRegs* r) { ed_load(r); }
void amos_typed_ed_load_hidden_fn(AllRegs* r) { ed_load_hidden_fn(r); }
void amos_typed_ed_load_hidden_key(AllRegs* r) { ed_load_hidden_key(r); }
void amos_typed_ed_make_bak(AllRegs* r) { ed_make_bak(r); }
void amos_typed_ed_merge(AllRegs* r) { ed_merge(r); }
void amos_typed_ed_merge_ascii(AllRegs* r) { ed_merge_ascii(r); }
void amos_typed_ed_name_part(AllRegs* r) { ed_name_part(r); }
void amos_typed_ed_reload(AllRegs* r) { ed_reload(r); }
void amos_typed_ed_rename(AllRegs* r) { ed_rename(r); }
void amos_typed_ed_save(AllRegs* r) { ed_save(r); }
void amos_typed_ed_save_as(AllRegs* r) { ed_save_as(r); }
void amos_typed_ed_save_as_name(AllRegs* r) { ed_save_as_name(r); }
void amos_typed_ed_save_icon(AllRegs* r) { ed_save_icon(r); }
void amos_typed_ed_save_it(AllRegs* r) { ed_save_it(r); }
void amos_typed_ed_save_program(AllRegs* r) { ed_save_program(r); }
void amos_typed_ed_save_program2(AllRegs* r) { ed_save_program2(r); }
}
}
