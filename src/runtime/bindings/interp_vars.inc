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
uint32_t amos_typed_interp_find_var_reg(RegFrame& r) { return detail::typed_reg_call<&interp_find_var_reg>(r); }
void amos_typed_interp_fn_array(interp::EvalOut* out) { interp_fn_array(out); }
void amos_typed_interp_fn_fn(interp::EvalOut* out) { interp_fn_fn(out); }
void amos_typed_interp_fn_var(interp::EvalOut* out) { interp_fn_var(out); }
void amos_typed_interp_fn_varptr(interp::EvalOut* out) { interp_fn_varptr(out); }
uint32_t amos_typed_interp_get_tablo_reg(RegFrame& r) { return detail::typed_reg_call<&interp_get_tablo_reg>(r); }
void amos_typed_interp_in_add2() { interp_in_add2(); }
void amos_typed_interp_in_add4() { interp_in_add4(); }
void amos_typed_interp_in_dec() { interp_in_dec(); }
void amos_typed_interp_in_dfn() { interp_in_dfn(); }
void amos_typed_interp_in_dim() { interp_in_dim(); }
void amos_typed_interp_in_inc() { interp_in_inc(); }
void amos_typed_interp_in_shared() { interp_in_shared(); }
void amos_typed_interp_in_swap() { interp_in_swap(); }
void amos_typed_interp_in_var() { interp_in_var(); }
}
}
