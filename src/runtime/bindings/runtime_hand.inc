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
void amos_typed_host_IceStart(AllRegs& r) { host_IceStart(r); }
void amos_typed_host_Init_Fin(AllRegs& r) { host_Init_Fin(r); }
void amos_typed_host_Prg_RunIt(AllRegs& r) { host_Prg_RunIt(r); }
void amos_typed_host_TheEnd(AllRegs& r) { host_TheEnd(r); }
void amos_typed_host_music_sami_entry(AllRegs& r) { host_music_sami_entry(r); }
void amos_typed_host_w_animator(AllRegs& r) { host_w_animator(r); }
void amos_typed_host_w_chip_mm(AllRegs& r) { host_w_chip_mm(r); }
void amos_typed_host_w_chip_mm2(AllRegs& r) { host_w_chip_mm2(r); }
void amos_typed_host_w_fast_mm(AllRegs& r) { host_w_fast_mm(r); }
void amos_typed_host_w_fast_mm2(AllRegs& r) { host_w_fast_mm2(r); }
void amos_typed_host_w_get_col(AllRegs& r) { host_w_get_col(r); }
}
}
