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
void amos_typed_lib_close_devs() { lib_close_devs(); }
void amos_typed_lib_close_libs() { lib_close_libs(); }
uint32_t amos_typed_lib_dev_abort_io(RegFrame& r) { return detail::typed_reg_call<&lib_dev_abort_io>(r); }
uint32_t amos_typed_lib_dev_check_io(RegFrame& r) { return detail::typed_reg_call<&lib_dev_check_io>(r); }
uint32_t amos_typed_lib_dev_close(RegFrame& r) { return detail::typed_reg_call<&lib_dev_close>(r); }
uint32_t amos_typed_lib_dev_close_a2(RegFrame& r) { return detail::typed_reg_call<&lib_dev_close_a2>(r); }
uint32_t amos_typed_lib_dev_do_io(RegFrame& r) { return detail::typed_reg_call<&lib_dev_do_io>(r); }
uint32_t amos_typed_lib_dev_error(RegFrame& r) { return detail::typed_reg_call<&lib_dev_error>(r); }
uint32_t amos_typed_lib_dev_get_a2(RegFrame& r) { return detail::typed_reg_call<&lib_dev_get_a2>(r); }
uint32_t amos_typed_lib_dev_get_io(RegFrame& r) { return detail::typed_reg_call<&lib_dev_get_io>(r); }
uint32_t amos_typed_lib_dev_open(RegFrame& r) { return detail::typed_reg_call<&lib_dev_open>(r); }
uint32_t amos_typed_lib_dev_send_io(RegFrame& r) { return detail::typed_reg_call<&lib_dev_send_io>(r); }
uint64_t amos_typed_lib_fn_dev_base(const uint32_t* words) { return typed_library_call<&lib_fn_dev_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dev_check(const uint32_t* words) { return typed_library_call<&lib_fn_dev_check, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dos_call(const uint32_t* words) { return typed_library_call<&lib_fn_dos_call, 1, 4>(words); }
uint64_t amos_typed_lib_fn_exe_call(const uint32_t* words) { return typed_library_call<&lib_fn_exe_call, 1, 4>(words); }
uint64_t amos_typed_lib_fn_gfx_call(const uint32_t* words) { return typed_library_call<&lib_fn_gfx_call, 1, 4>(words); }
uint64_t amos_typed_lib_fn_int_call(const uint32_t* words) { return typed_library_call<&lib_fn_int_call, 1, 4>(words); }
uint64_t amos_typed_lib_fn_lib_base(const uint32_t* words) { return typed_library_call<&lib_fn_lib_base, 1, 4>(words); }
uint64_t amos_typed_lib_fn_lib_call(const uint32_t* words) { return typed_library_call<&lib_fn_lib_call, 2, 4>(words); }
uint64_t amos_typed_lib_in_dev_abort(const uint32_t* words) { return typed_library_call<&lib_in_dev_abort, 1, 0>(words); }
uint64_t amos_typed_lib_in_dev_close0(const uint32_t* words) { return typed_library_call<&lib_in_dev_close0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dev_close1(const uint32_t* words) { return typed_library_call<&lib_in_dev_close1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dev_do(const uint32_t* words) { return typed_library_call<&lib_in_dev_do, 2, 0>(words); }
uint64_t amos_typed_lib_in_dev_open(const uint32_t* words) { return typed_library_call<&lib_in_dev_open, 5, 0>(words); }
uint64_t amos_typed_lib_in_dev_send(const uint32_t* words) { return typed_library_call<&lib_in_dev_send, 2, 0>(words); }
uint64_t amos_typed_lib_in_exec(const uint32_t* words) { return typed_library_call<&lib_in_exec, 1, 0>(words); }
uint64_t amos_typed_lib_in_lib_close0(const uint32_t* words) { return typed_library_call<&lib_in_lib_close0, 0, 0>(words); }
uint64_t amos_typed_lib_in_lib_close1(const uint32_t* words) { return typed_library_call<&lib_in_lib_close1, 1, 0>(words); }
uint64_t amos_typed_lib_in_lib_open(const uint32_t* words) { return typed_library_call<&lib_in_lib_open, 3, 0>(words); }
uint32_t amos_typed_lib_lib_call(RegFrame& r) { return detail::typed_reg_call<&lib_lib_call>(r); }
uint32_t amos_typed_lib_lib_close(RegFrame& r) { return detail::typed_reg_call<&lib_lib_close>(r); }
uint32_t amos_typed_lib_lib_close_d0(RegFrame& r) { return detail::typed_reg_call<&lib_lib_close_d0>(r); }
uint32_t amos_typed_lib_lib_get_a2(RegFrame& r) { return detail::typed_reg_call<&lib_lib_get_a2>(r); }
}
}
