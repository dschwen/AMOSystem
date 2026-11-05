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
void amos_typed_interp_fn_btst(interp::EvalOut* out) { interp_fn_btst(out); }
void amos_typed_interp_in_bchg() { interp_in_bchg(); }
void amos_typed_interp_in_bclr() { interp_in_bclr(); }
void amos_typed_interp_in_bset() { interp_in_bset(); }
void amos_typed_interp_in_rol_b() { interp_in_rol_b(); }
void amos_typed_interp_in_rol_l() { interp_in_rol_l(); }
void amos_typed_interp_in_rol_w() { interp_in_rol_w(); }
void amos_typed_interp_in_ror_b() { interp_in_ror_b(); }
void amos_typed_interp_in_ror_l() { interp_in_ror_l(); }
void amos_typed_interp_in_ror_w() { interp_in_ror_w(); }
}
}
