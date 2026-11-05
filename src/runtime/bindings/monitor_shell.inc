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
void amos_typed_mon_initialise(AllRegs* r) { mon_initialise(r); }
void amos_typed_mon_nothing(AllRegs* r) { mon_nothing(r); }
void amos_typed_mon_quit(AllRegs* r) { mon_quit(r); }
void amos_typed_mon_reinit(AllRegs* r) { mon_reinit(r); }
void amos_typed_mon_reloop(AllRegs* r) { mon_reloop(r); }
void amos_typed_mon_split_token(AllRegs* r) { mon_split_token(r); }
void amos_typed_mon_step0(AllRegs* r) { mon_step0(r); }
void amos_typed_mon_step1(AllRegs* r) { mon_step1(r); }
void amos_typed_mon_step2(AllRegs* r) { mon_step2(r); }
void amos_typed_mon_step3(AllRegs* r) { mon_step3(r); }
void amos_typed_mon_tests(AllRegs* r) { mon_tests(r); }
}
}
