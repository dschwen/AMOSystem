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
void amos_typed_ed_goto_line(AllRegs* r) { ed_goto_line(r); }
void amos_typed_ed_replace_new(AllRegs* r) { ed_replace_new(r); }
void amos_typed_ed_replace_next(AllRegs* r) { ed_replace_next(r); }
void amos_typed_ed_replace_previous(AllRegs* r) { ed_replace_previous(r); }
void amos_typed_ed_replace_text(AllRegs* r) { ed_replace_text(r); }
void amos_typed_ed_search(AllRegs* r) { ed_search(r); }
void amos_typed_ed_search_back(AllRegs* r) { ed_search_back(r); }
void amos_typed_ed_search_dialogue(AllRegs* r) { ed_search_dialogue(r); }
void amos_typed_ed_search_front(AllRegs* r) { ed_search_front(r); }
void amos_typed_ed_search_new(AllRegs* r) { ed_search_new(r); }
void amos_typed_ed_search_next(AllRegs* r) { ed_search_next(r); }
void amos_typed_ed_search_previous(AllRegs* r) { ed_search_previous(r); }
}
}
