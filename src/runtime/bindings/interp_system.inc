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
void amos_typed_interp_fn_equ(interp::EvalOut* out) { interp_fn_equ(out); }
void amos_typed_interp_fn_struc(interp::EvalOut* out) { interp_fn_struc(out); }
void amos_typed_interp_fn_struc_d(interp::EvalOut* out) { interp_fn_struc_d(out); }
void amos_typed_interp_in_call() { interp_in_call(); }
void amos_typed_interp_in_struc() { interp_in_struc(); }
void amos_typed_interp_in_struc_d() { interp_in_struc_d(); }
}
}
