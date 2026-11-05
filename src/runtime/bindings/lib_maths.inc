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
uint64_t amos_typed_lib_fn_abs(const uint32_t* words) { return typed_library_call<&lib_fn_abs, 1, 4>(words); }
void amos_typed_lib_fn_abs_f(uint32_t* words) { detail::typed_float_call<&lib_fn_abs_f>(words); }
void amos_typed_lib_fn_acos(uint32_t* words) { detail::typed_float_call<&lib_fn_acos>(words); }
void amos_typed_lib_fn_asin(uint32_t* words) { detail::typed_float_call<&lib_fn_asin>(words); }
void amos_typed_lib_fn_atan(uint32_t* words) { detail::typed_float_call<&lib_fn_atan>(words); }
void amos_typed_lib_fn_cos(uint32_t* words) { detail::typed_float_call<&lib_fn_cos>(words); }
void amos_typed_lib_fn_exp(uint32_t* words) { detail::typed_float_call<&lib_fn_exp>(words); }
void amos_typed_lib_fn_hcos(uint32_t* words) { detail::typed_float_call<&lib_fn_hcos>(words); }
void amos_typed_lib_fn_hsin(uint32_t* words) { detail::typed_float_call<&lib_fn_hsin>(words); }
void amos_typed_lib_fn_htan(uint32_t* words) { detail::typed_float_call<&lib_fn_htan>(words); }
void amos_typed_lib_fn_int_f(uint32_t* words) { detail::typed_float_call<&lib_fn_int_f>(words); }
void amos_typed_lib_fn_ln(uint32_t* words) { detail::typed_float_call<&lib_fn_ln>(words); }
void amos_typed_lib_fn_log(uint32_t* words) { detail::typed_float_call<&lib_fn_log>(words); }
uint64_t amos_typed_lib_fn_sgn(const uint32_t* words) { return typed_library_call<&lib_fn_sgn, 1, 4>(words); }
void amos_typed_lib_fn_sgn_f(uint32_t* words) { detail::typed_float_call<&lib_fn_sgn_f>(words); }
void amos_typed_lib_fn_sin(uint32_t* words) { detail::typed_float_call<&lib_fn_sin>(words); }
void amos_typed_lib_fn_sqr(uint32_t* words) { detail::typed_float_call<&lib_fn_sqr>(words); }
void amos_typed_lib_fn_str_f(uint32_t* words) { detail::typed_float_call<&lib_fn_str_f>(words); }
void amos_typed_lib_fn_tan(uint32_t* words) { detail::typed_float_call<&lib_fn_tan>(words); }
}
}
