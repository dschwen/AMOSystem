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
void amos_typed_interp_fn_left(interp::EvalOut* out) { interp_fn_left(out); }
void amos_typed_interp_fn_mid2(interp::EvalOut* out) { interp_fn_mid2(out); }
void amos_typed_interp_fn_mid3(interp::EvalOut* out) { interp_fn_mid3(out); }
void amos_typed_interp_fn_resource(interp::EvalOut* out) { interp_fn_resource(out); }
void amos_typed_interp_fn_right(interp::EvalOut* out) { interp_fn_right(out); }
void amos_typed_interp_fn_val(interp::EvalOut* out) { interp_fn_val(out); }
void amos_typed_interp_in_left() { interp_in_left(); }
void amos_typed_interp_in_mid2() { interp_in_mid2(); }
void amos_typed_interp_in_mid3() { interp_in_mid3(); }
void amos_typed_interp_in_right() { interp_in_right(); }
}
}
