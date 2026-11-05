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
uint64_t amos_typed_lib_fn_err(const uint32_t* words) { return typed_library_call<&lib_fn_err, 1, 4>(words); }
uint64_t amos_typed_lib_fn_errn(const uint32_t* words) { return typed_library_call<&lib_fn_errn, 0, 4>(words); }
uint64_t amos_typed_lib_fn_param_int(const uint32_t* words) { return typed_library_call<&lib_fn_param_int, 0, 4>(words); }
uint64_t amos_typed_lib_fn_param_string(const uint32_t* words) { return typed_library_call<&lib_fn_param_string, 0, 4>(words); }
uint64_t amos_typed_lib_fn_pi(const uint32_t* words) { return typed_library_call<&lib_fn_pi, 0, 8>(words); }
uint64_t amos_typed_lib_fn_rnd(const uint32_t* words) { return typed_library_call<&lib_fn_rnd, 1, 4>(words); }
uint64_t amos_typed_lib_fn_str_int(const uint32_t* words) { return typed_library_call<&lib_fn_str_int, 1, 4>(words); }
uint64_t amos_typed_lib_fn_timer(const uint32_t* words) { return typed_library_call<&lib_fn_timer, 0, 4>(words); }
uint64_t amos_typed_lib_in_degree(const uint32_t* words) { return typed_library_call<&lib_in_degree, 0, 0>(words); }
uint64_t amos_typed_lib_in_fix(const uint32_t* words) { return typed_library_call<&lib_in_fix, 1, 0>(words); }
uint64_t amos_typed_lib_in_key_speed(const uint32_t* words) { return typed_library_call<&lib_in_key_speed, 2, 0>(words); }
uint64_t amos_typed_lib_in_multi_wait(const uint32_t* words) { return typed_library_call<&lib_in_multi_wait, 0, 0>(words); }
uint64_t amos_typed_lib_in_radian(const uint32_t* words) { return typed_library_call<&lib_in_radian, 0, 0>(words); }
uint64_t amos_typed_lib_in_randomize(const uint32_t* words) { return typed_library_call<&lib_in_randomize, 1, 0>(words); }
uint64_t amos_typed_lib_in_timer(const uint32_t* words) { return typed_library_call<&lib_in_timer, 1, 0>(words); }
uint64_t amos_typed_lib_in_wait(const uint32_t* words) { return typed_library_call<&lib_in_wait, 1, 0>(words); }
uint64_t amos_typed_lib_in_wait_key(const uint32_t* words) { return typed_library_call<&lib_in_wait_key, 0, 0>(words); }
uint64_t amos_typed_lib_in_wait_vbl(const uint32_t* words) { return typed_library_call<&lib_in_wait_vbl, 0, 0>(words); }
uint32_t amos_typed_lib_mulu32(RegFrame& r) { return detail::typed_reg_call<&lib_mulu32>(r); }
uint32_t amos_typed_lib_wait_rout(RegFrame& r) { return detail::typed_reg_call<&lib_wait_rout>(r); }
}
}
