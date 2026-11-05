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
uint32_t amos_typed_main_ask_dir(RegFrame& r) { return detail::typed_reg_call<&main_ask_dir>(r); }
uint32_t amos_typed_main_ask_dir2(RegFrame& r) { return detail::typed_reg_call<&main_ask_dir2>(r); }
uint32_t amos_typed_main_def_get_message(RegFrame& r) { return detail::typed_reg_call<&main_def_get_message>(r); }
void amos_typed_main_free_temp_buffer() { main_free_temp_buffer(); }
uint32_t amos_typed_main_get_message(RegFrame& r) { return detail::typed_reg_call<&main_get_message>(r); }
uint32_t amos_typed_main_math_close(RegFrame& r) { return detail::typed_reg_call<&main_math_close>(r); }
uint32_t amos_typed_main_ram_chip(RegFrame& r) { return detail::typed_reg_call<&main_ram_chip>(r); }
uint32_t amos_typed_main_ram_chip2(RegFrame& r) { return detail::typed_reg_call<&main_ram_chip2>(r); }
uint32_t amos_typed_main_ram_fast(RegFrame& r) { return detail::typed_reg_call<&main_ram_fast>(r); }
uint32_t amos_typed_main_ram_fast2(RegFrame& r) { return detail::typed_reg_call<&main_ram_fast2>(r); }
uint32_t amos_typed_main_ram_free(RegFrame& r) { return detail::typed_reg_call<&main_ram_free>(r); }
uint32_t amos_typed_main_recop(RegFrame& r) { return detail::typed_reg_call<&main_recop>(r); }
uint32_t amos_typed_main_res_temp_buffer(RegFrame& r) { return detail::typed_reg_call<&main_res_temp_buffer>(r); }
uint32_t amos_typed_main_sys_add_path(RegFrame& r) { return detail::typed_reg_call<&main_sys_add_path>(r); }
uint32_t amos_typed_main_sys_clear_cache(RegFrame& r) { return detail::typed_reg_call<&main_sys_clear_cache>(r); }
uint32_t amos_typed_main_sys_get_message(RegFrame& r) { return detail::typed_reg_call<&main_sys_get_message>(r); }
uint32_t amos_typed_main_sys_uncode(RegFrame& r) { return detail::typed_reg_call<&main_sys_uncode>(r); }
uint32_t amos_typed_main_sys_wait_mul(RegFrame& r) { return detail::typed_reg_call<&main_sys_wait_mul>(r); }
uint32_t amos_typed_main_w_option(RegFrame& r) { return detail::typed_reg_call<&main_w_option>(r); }
}
}
