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
void amos_typed_interp_fn_c_entier(interp::EvalOut* out) { interp_fn_c_entier(out); }
void amos_typed_interp_fn_cst_ch(interp::EvalOut* out) { interp_fn_cst_ch(out); }
void amos_typed_interp_fn_cst_dfl(interp::EvalOut* out) { interp_fn_cst_dfl(out); }
void amos_typed_interp_fn_cst_fl(interp::EvalOut* out) { interp_fn_cst_fl(out); }
void amos_typed_interp_fn_max(interp::EvalOut* out) { interp_fn_max(out); }
void amos_typed_interp_fn_min(interp::EvalOut* out) { interp_fn_min(out); }
void amos_typed_interp_fn_not(interp::EvalOut* out) { interp_fn_not(out); }
void amos_typed_interp_new_evalue(interp::EvalOut* out) { interp_new_evalue(out); }
}
}
