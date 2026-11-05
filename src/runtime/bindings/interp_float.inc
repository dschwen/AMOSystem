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
uint32_t amos_typed_interp_aangle(RegFrame& r) { return detail::typed_reg_call<&interp_aangle>(r); }
uint32_t amos_typed_interp_aangle_d(RegFrame& r) { return detail::typed_reg_call<&interp_aangle_d>(r); }
uint32_t amos_typed_interp_ascii2float(RegFrame& r) { return detail::typed_reg_call<&interp_ascii2float>(r); }
uint32_t amos_typed_interp_ascii2float_d(RegFrame& r) { return detail::typed_reg_call<&interp_ascii2float_d>(r); }
uint32_t amos_typed_interp_dfl_to_int1(RegFrame& r) { return detail::typed_reg_call<&interp_dfl_to_int1>(r); }
uint32_t amos_typed_interp_dfl_to_int2(RegFrame& r) { return detail::typed_reg_call<&interp_dfl_to_int2>(r); }
uint32_t amos_typed_interp_dfloat_compare(RegFrame& r) { return detail::typed_reg_call<&interp_dfloat_compare>(r); }
uint32_t amos_typed_interp_dfloat_fonction(RegFrame& r) { return detail::typed_reg_call<&interp_dfloat_fonction>(r); }
uint32_t amos_typed_interp_dfloat_operation(RegFrame& r) { return detail::typed_reg_call<&interp_dfloat_operation>(r); }
uint32_t amos_typed_interp_dint_to_fl1(RegFrame& r) { return detail::typed_reg_call<&interp_dint_to_fl1>(r); }
uint32_t amos_typed_interp_dint_to_fl2(RegFrame& r) { return detail::typed_reg_call<&interp_dint_to_fl2>(r); }
uint32_t amos_typed_interp_dmath_fonction(RegFrame& r) { return detail::typed_reg_call<&interp_dmath_fonction>(r); }
uint32_t amos_typed_interp_dmath_operation(RegFrame& r) { return detail::typed_reg_call<&interp_dmath_operation>(r); }
uint32_t amos_typed_interp_fangle_d(RegFrame& r) { return detail::typed_reg_call<&interp_fangle_d>(r); }
uint32_t amos_typed_interp_ffangle(RegFrame& r) { return detail::typed_reg_call<&interp_ffangle>(r); }
uint32_t amos_typed_interp_fl_pos(RegFrame& r) { return detail::typed_reg_call<&interp_fl_pos>(r); }
uint32_t amos_typed_interp_fl_pos_d(RegFrame& r) { return detail::typed_reg_call<&interp_fl_pos_d>(r); }
uint32_t amos_typed_interp_fl_to_int1(RegFrame& r) { return detail::typed_reg_call<&interp_fl_to_int1>(r); }
uint32_t amos_typed_interp_fl_to_int2(RegFrame& r) { return detail::typed_reg_call<&interp_fl_to_int2>(r); }
uint32_t amos_typed_interp_float2ascii(RegFrame& r) { return detail::typed_reg_call<&interp_float2ascii>(r); }
uint32_t amos_typed_interp_float2ascii_d(RegFrame& r) { return detail::typed_reg_call<&interp_float2ascii_d>(r); }
uint32_t amos_typed_interp_float_compare(RegFrame& r) { return detail::typed_reg_call<&interp_float_compare>(r); }
uint32_t amos_typed_interp_float_fonction(RegFrame& r) { return detail::typed_reg_call<&interp_float_fonction>(r); }
uint32_t amos_typed_interp_float_operation(RegFrame& r) { return detail::typed_reg_call<&interp_float_operation>(r); }
uint32_t amos_typed_interp_float_test(RegFrame& r) { return detail::typed_reg_call<&interp_float_test>(r); }
uint32_t amos_typed_interp_float_testf(RegFrame& r) { return detail::typed_reg_call<&interp_float_testf>(r); }
uint32_t amos_typed_interp_fn_param_d(RegFrame& r) { return detail::typed_reg_call<&interp_fn_param_d>(r); }
uint32_t amos_typed_interp_fn_param_f(RegFrame& r) { return detail::typed_reg_call<&interp_fn_param_f>(r); }
uint32_t amos_typed_interp_int_to_fl1(RegFrame& r) { return detail::typed_reg_call<&interp_int_to_fl1>(r); }
uint32_t amos_typed_interp_int_to_fl2(RegFrame& r) { return detail::typed_reg_call<&interp_int_to_fl2>(r); }
uint32_t amos_typed_interp_math_fonction(RegFrame& r) { return detail::typed_reg_call<&interp_math_fonction>(r); }
uint32_t amos_typed_interp_math_operation(RegFrame& r) { return detail::typed_reg_call<&interp_math_operation>(r); }
void amos_typed_interp_open_math_libraries() { interp_open_math_libraries(); }
}
}
