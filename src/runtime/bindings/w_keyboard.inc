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
void amos_typed_w_sy_clear_key(AllRegs* r) { w_sy_clear_key(r); }
void amos_typed_w_sy_get_disc(AllRegs* r) { w_sy_get_disc(r); }
void amos_typed_w_sy_get_function_key(AllRegs* r) { w_sy_get_function_key(r); }
void amos_typed_w_sy_inkey(AllRegs* r) { w_sy_inkey(r); }
void amos_typed_w_sy_key_shift(AllRegs* r) { w_sy_key_shift(r); }
void amos_typed_w_sy_key_speed(AllRegs* r) { w_sy_key_speed(r); }
void amos_typed_w_sy_key_state(AllRegs* r) { w_sy_key_state(r); }
uint32_t amos_typed_w_sy_key_waiting(AllRegs* r) { return uint32_t(w_sy_key_waiting(r)); }
void amos_typed_w_sy_nothing(AllRegs* r) { w_sy_nothing(r); }
void amos_typed_w_sy_put_key(AllRegs* r) { w_sy_put_key(r); }
void amos_typed_w_sy_set_function_key(AllRegs* r) { w_sy_set_function_key(r); }
}
}
