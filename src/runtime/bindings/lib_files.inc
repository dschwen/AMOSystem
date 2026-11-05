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
uint32_t amos_typed_lib_clo_all(RegFrame& r) { return detail::typed_reg_call<&lib_clo_all>(r); }
uint32_t amos_typed_lib_cloa1(RegFrame& r) { return detail::typed_reg_call<&lib_cloa1>(r); }
void amos_typed_lib_close_all() { lib_close_all(); }
uint32_t amos_typed_lib_copy_path(RegFrame& r) { return detail::typed_reg_call<&lib_copy_path>(r); }
uint32_t amos_typed_lib_end_byte(RegFrame& r) { return detail::typed_reg_call<&lib_end_byte>(r); }
uint64_t amos_typed_lib_fn_dfree(const uint32_t* words) { return typed_library_call<&lib_fn_dfree, 0, 4>(words); }
uint64_t amos_typed_lib_fn_dir(const uint32_t* words) { return typed_library_call<&lib_fn_dir, 0, 4>(words); }
uint64_t amos_typed_lib_fn_disc_info(const uint32_t* words) { return typed_library_call<&lib_fn_disc_info, 1, 4>(words); }
uint64_t amos_typed_lib_fn_drive(const uint32_t* words) { return typed_library_call<&lib_fn_drive, 1, 4>(words); }
uint64_t amos_typed_lib_fn_eof(const uint32_t* words) { return typed_library_call<&lib_fn_eof, 1, 4>(words); }
uint64_t amos_typed_lib_fn_exist(const uint32_t* words) { return typed_library_call<&lib_fn_exist, 1, 4>(words); }
uint64_t amos_typed_lib_fn_input1(const uint32_t* words) { return typed_library_call<&lib_fn_input1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_input2(const uint32_t* words) { return typed_library_call<&lib_fn_input2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_lof(const uint32_t* words) { return typed_library_call<&lib_fn_lof, 1, 4>(words); }
uint64_t amos_typed_lib_fn_pof(const uint32_t* words) { return typed_library_call<&lib_fn_pof, 1, 4>(words); }
uint64_t amos_typed_lib_fn_port(const uint32_t* words) { return typed_library_call<&lib_fn_port, 1, 4>(words); }
uint32_t amos_typed_lib_get_byte(RegFrame& r) { return detail::typed_reg_call<&lib_get_byte>(r); }
uint32_t amos_typed_lib_get_file(RegFrame& r) { return detail::typed_reg_call<&lib_get_file>(r); }
uint64_t amos_typed_lib_in_append(const uint32_t* words) { return typed_library_call<&lib_in_append, 2, 0>(words); }
uint64_t amos_typed_lib_in_bload(const uint32_t* words) { return typed_library_call<&lib_in_bload, 2, 0>(words); }
uint64_t amos_typed_lib_in_bsave(const uint32_t* words) { return typed_library_call<&lib_in_bsave, 3, 0>(words); }
uint64_t amos_typed_lib_in_close0(const uint32_t* words) { return typed_library_call<&lib_in_close0, 0, 0>(words); }
uint64_t amos_typed_lib_in_close1(const uint32_t* words) { return typed_library_call<&lib_in_close1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dir(const uint32_t* words) { return typed_library_call<&lib_in_dir, 1, 0>(words); }
uint64_t amos_typed_lib_in_get(const uint32_t* words) { return typed_library_call<&lib_in_get, 2, 0>(words); }
uint64_t amos_typed_lib_in_kill(const uint32_t* words) { return typed_library_call<&lib_in_kill, 1, 0>(words); }
uint64_t amos_typed_lib_in_mkdir(const uint32_t* words) { return typed_library_call<&lib_in_mkdir, 1, 0>(words); }
uint64_t amos_typed_lib_in_open_in(const uint32_t* words) { return typed_library_call<&lib_in_open_in, 2, 0>(words); }
uint64_t amos_typed_lib_in_open_out(const uint32_t* words) { return typed_library_call<&lib_in_open_out, 2, 0>(words); }
uint64_t amos_typed_lib_in_open_port(const uint32_t* words) { return typed_library_call<&lib_in_open_port, 2, 0>(words); }
uint64_t amos_typed_lib_in_open_random(const uint32_t* words) { return typed_library_call<&lib_in_open_random, 2, 0>(words); }
uint64_t amos_typed_lib_in_parent(const uint32_t* words) { return typed_library_call<&lib_in_parent, 0, 0>(words); }
uint64_t amos_typed_lib_in_pload(const uint32_t* words) { return typed_library_call<&lib_in_pload, 2, 0>(words); }
uint64_t amos_typed_lib_in_pof(const uint32_t* words) { return typed_library_call<&lib_in_pof, 2, 0>(words); }
uint64_t amos_typed_lib_in_put(const uint32_t* words) { return typed_library_call<&lib_in_put, 2, 0>(words); }
uint64_t amos_typed_lib_in_rename(const uint32_t* words) { return typed_library_call<&lib_in_rename, 2, 0>(words); }
uint64_t amos_typed_lib_in_set_input(const uint32_t* words) { return typed_library_call<&lib_in_set_input, 2, 0>(words); }
uint32_t amos_typed_lib_lock_free(RegFrame& r) { return detail::typed_reg_call<&lib_lock_free>(r); }
uint32_t amos_typed_lib_lock_get(RegFrame& r) { return detail::typed_reg_call<&lib_lock_get>(r); }
uint32_t amos_typed_lib_no_req(RegFrame& r) { return detail::typed_reg_call<&lib_no_req>(r); }
uint32_t amos_typed_lib_rexist(RegFrame& r) { return detail::typed_reg_call<&lib_rexist>(r); }
uint32_t amos_typed_lib_yes_req(RegFrame& r) { return detail::typed_reg_call<&lib_yes_req>(r); }
}
}
