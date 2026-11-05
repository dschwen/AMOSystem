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
uint32_t amos_typed_lib_a0_to_chaine(RegFrame& r) { return detail::typed_reg_call<&lib_a0_to_chaine>(r); }
uint32_t amos_typed_lib_ch_ver_buf(RegFrame& r) { return detail::typed_reg_call<&lib_ch_ver_buf>(r); }
uint32_t amos_typed_lib_ch_ver_buf2(RegFrame& r) { return detail::typed_reg_call<&lib_ch_ver_buf2>(r); }
uint32_t amos_typed_lib_fin_bin(RegFrame& r) { return detail::typed_reg_call<&lib_fin_bin>(r); }
uint32_t amos_typed_lib_imp_chaine(RegFrame& r) { return detail::typed_reg_call<&lib_imp_chaine>(r); }
uint32_t amos_typed_lib_mc_suit(RegFrame& r) { return detail::typed_reg_call<&lib_mc_suit>(r); }
uint32_t amos_typed_lib_mes2_chaine(RegFrame& r) { return detail::typed_reg_call<&lib_mes2_chaine>(r); }
uint32_t amos_typed_lib_prt_close(RegFrame& r) { return detail::typed_reg_call<&lib_prt_close>(r); }
void amos_typed_lib_prt_close_fn() { lib_prt_close_fn(); }
uint32_t amos_typed_lib_prt_open(RegFrame& r) { return detail::typed_reg_call<&lib_prt_open>(r); }
uint32_t amos_typed_lib_prt_print(RegFrame& r) { return detail::typed_reg_call<&lib_prt_print>(r); }
uint32_t amos_typed_lib_ret_ch_vide(RegFrame& r) { return detail::typed_reg_call<&lib_ret_ch_vide>(r); }
uint32_t amos_typed_lib_set_hi_chaine(RegFrame& r) { return detail::typed_reg_call<&lib_set_hi_chaine>(r); }
uint32_t amos_typed_lib_str2_chaine(RegFrame& r) { return detail::typed_reg_call<&lib_str2_chaine>(r); }
}
}
