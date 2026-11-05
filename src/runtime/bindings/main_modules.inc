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
uint32_t amos_typed_main_edit_free(RegFrame& r) { return detail::typed_reg_call<&main_edit_free>(r); }
uint32_t amos_typed_main_edit_load(RegFrame& r) { return detail::typed_reg_call<&main_edit_load>(r); }
uint64_t amos_typed_main_in_close_editor(const uint32_t* words) { return typed_library_call<&main_in_close_editor, 0, 0>(words); }
uint64_t amos_typed_main_in_kill_editor(const uint32_t* words) { return typed_library_call<&main_in_kill_editor, 0, 0>(words); }
uint64_t amos_typed_main_in_monitor(const uint32_t* words) { return typed_library_call<&main_in_monitor, 0, 0>(words); }
uint32_t amos_typed_main_mem_del_banks(RegFrame& r) { return detail::typed_reg_call<&main_mem_del_banks>(r); }
uint32_t amos_typed_main_mem_maximum(RegFrame& r) { return detail::typed_reg_call<&main_mem_maximum>(r); }
uint32_t amos_typed_main_mon_free(RegFrame& r) { return detail::typed_reg_call<&main_mon_free>(r); }
uint32_t amos_typed_main_mon_load(RegFrame& r) { return detail::typed_reg_call<&main_mon_load>(r); }
uint32_t amos_typed_main_run_error_no_editor_reg(RegFrame& r) { return detail::typed_reg_call<&main_run_error_no_editor_reg>(r); }
uint32_t amos_typed_main_run_error_run_only_reg(RegFrame& r) { return detail::typed_reg_call<&main_run_error_run_only_reg>(r); }
}
}
