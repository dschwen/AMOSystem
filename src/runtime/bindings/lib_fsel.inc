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
uint32_t amos_typed_lib_dsk_file_selector(RegFrame& r) { return detail::typed_reg_call<&lib_dsk_file_selector>(r); }
uint64_t amos_typed_lib_fn_file_selector1(const uint32_t* words) { return typed_library_call<&lib_fn_file_selector1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_file_selector2(const uint32_t* words) { return typed_library_call<&lib_fn_file_selector2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_file_selector3(const uint32_t* words) { return typed_library_call<&lib_fn_file_selector3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_file_selector4(const uint32_t* words) { return typed_library_call<&lib_fn_file_selector4, 4, 4>(words); }
void amos_typed_lib_fs_end() { lib_fs_end(); }
void amos_typed_lib_fs_error() { lib_fs_error(); }
void amos_typed_lib_fs_flush() { lib_fs_flush(); }
uint32_t amos_typed_lib_fs_get_name_c(RegFrame& r) { return detail::typed_reg_call<&lib_fs_get_name_c>(r); }
}
}
