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
void amos_typed_ed_detok_editor(AllRegs* r) { ed_detok_editor(r); }
void amos_typed_ed_detok_monitor(AllRegs* r) { ed_detok_monitor(r); }
void amos_typed_ed_skip_token(AllRegs* r) { ed_skip_token(r); }
void amos_typed_ed_tokenise(AllRegs* r) { ed_tokenise(r); }
}
}
