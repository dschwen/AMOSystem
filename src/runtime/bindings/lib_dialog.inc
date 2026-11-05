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
uint32_t amos_typed_lib_bt_end(RegFrame& r) { return detail::typed_reg_call<&lib_bt_end>(r); }
uint32_t amos_typed_lib_bt_gere(RegFrame& r) { return detail::typed_reg_call<&lib_bt_gere>(r); }
uint32_t amos_typed_lib_bt_init(RegFrame& r) { return detail::typed_reg_call<&lib_bt_init>(r); }
uint32_t amos_typed_lib_bt_init_list(RegFrame& r) { return detail::typed_reg_call<&lib_bt_init_list>(r); }
uint32_t amos_typed_lib_dia_auto_test(RegFrame& r) { return detail::typed_reg_call<&lib_dia_auto_test>(r); }
uint32_t amos_typed_lib_dia_auto_test2(RegFrame& r) { return detail::typed_reg_call<&lib_dia_auto_test2>(r); }
uint32_t amos_typed_lib_dia_clear_key(RegFrame& r) { return detail::typed_reg_call<&lib_dia_clear_key>(r); }
void amos_typed_lib_dia_close_all() { lib_dia_close_all(); }
uint32_t amos_typed_lib_dia_close_channel(RegFrame& r) { return detail::typed_reg_call<&lib_dia_close_channel>(r); }
uint32_t amos_typed_lib_dia_close_channels(RegFrame& r) { return detail::typed_reg_call<&lib_dia_close_channels>(r); }
uint32_t amos_typed_lib_dia_eff_channel(RegFrame& r) { return detail::typed_reg_call<&lib_dia_eff_channel>(r); }
uint32_t amos_typed_lib_dia_freeze_channel(RegFrame& r) { return detail::typed_reg_call<&lib_dia_freeze_channel>(r); }
uint32_t amos_typed_lib_dia_freeze_channels(RegFrame& r) { return detail::typed_reg_call<&lib_dia_freeze_channels>(r); }
uint32_t amos_typed_lib_dia_get_channel(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_channel>(r); }
uint32_t amos_typed_lib_dia_get_return(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_return>(r); }
uint32_t amos_typed_lib_dia_get_value(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_value>(r); }
uint32_t amos_typed_lib_dia_get_variable(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_variable>(r); }
uint32_t amos_typed_lib_dia_get_vflags(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_vflags>(r); }
uint32_t amos_typed_lib_dia_get_zone(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_zone>(r); }
uint32_t amos_typed_lib_dia_get_zone_ad(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_zone_ad>(r); }
uint32_t amos_typed_lib_dia_no_mkey(RegFrame& r) { return detail::typed_reg_call<&lib_dia_no_mkey>(r); }
uint32_t amos_typed_lib_dia_open_channel(RegFrame& r) { return detail::typed_reg_call<&lib_dia_open_channel>(r); }
uint32_t amos_typed_lib_dia_rsc_open(RegFrame& r) { return detail::typed_reg_call<&lib_dia_rsc_open>(r); }
uint32_t amos_typed_lib_dia_run_program(RegFrame& r) { return detail::typed_reg_call<&lib_dia_run_program>(r); }
uint32_t amos_typed_lib_dia_run_quick(RegFrame& r) { return detail::typed_reg_call<&lib_dia_run_quick>(r); }
uint32_t amos_typed_lib_dia_set_vflags(RegFrame& r) { return detail::typed_reg_call<&lib_dia_set_vflags>(r); }
uint32_t amos_typed_lib_dia_update(RegFrame& r) { return detail::typed_reg_call<&lib_dia_update>(r); }
uint32_t amos_typed_lib_dia_wait_mul(RegFrame& r) { return detail::typed_reg_call<&lib_dia_wait_mul>(r); }
uint32_t amos_typed_lib_sl_clic(RegFrame& r) { return detail::typed_reg_call<&lib_sl_clic>(r); }
uint32_t amos_typed_lib_sl_draw(RegFrame& r) { return detail::typed_reg_call<&lib_sl_draw>(r); }
uint32_t amos_typed_lib_sl_init(RegFrame& r) { return detail::typed_reg_call<&lib_sl_init>(r); }
}
}
