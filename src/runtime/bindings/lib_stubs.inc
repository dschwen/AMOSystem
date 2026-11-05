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
uint32_t amos_typed_lib_ad_b_err(RegFrame& r) { return detail::typed_reg_call<&lib_ad_b_err>(r); }
uint32_t amos_typed_lib_ad_bob(RegFrame& r) { return detail::typed_reg_call<&lib_ad_bob>(r); }
uint32_t amos_typed_lib_ad_icon(RegFrame& r) { return detail::typed_reg_call<&lib_ad_icon>(r); }
uint32_t amos_typed_lib_cop_err(RegFrame& r) { return detail::typed_reg_call<&lib_cop_err>(r); }
uint32_t amos_typed_lib_ec_wi_err(RegFrame& r) { return detail::typed_reg_call<&lib_ec_wi_err>(r); }
uint32_t amos_typed_lib_err_adr_err(RegFrame& r) { return detail::typed_reg_call<&lib_err_adr_err>(r); }
uint32_t amos_typed_lib_err_b_fon_call(RegFrame& r) { return detail::typed_reg_call<&lib_err_b_fon_call>(r); }
uint32_t amos_typed_lib_err_bk_al_res(RegFrame& r) { return detail::typed_reg_call<&lib_err_bk_al_res>(r); }
uint32_t amos_typed_lib_err_bk_no_res(RegFrame& r) { return detail::typed_reg_call<&lib_err_bk_no_res>(r); }
uint32_t amos_typed_lib_err_cant_fit(RegFrame& r) { return detail::typed_reg_call<&lib_err_cant_fit>(r); }
uint32_t amos_typed_lib_err_di_form(RegFrame& r) { return detail::typed_reg_call<&lib_err_di_form>(r); }
uint32_t amos_typed_lib_err_eo_fil(RegFrame& r) { return detail::typed_reg_call<&lib_err_eo_fil>(r); }
uint32_t amos_typed_lib_err_fft_e(RegFrame& r) { return detail::typed_reg_call<&lib_err_fft_e>(r); }
uint32_t amos_typed_lib_err_fil_no(RegFrame& r) { return detail::typed_reg_call<&lib_err_fil_no>(r); }
uint32_t amos_typed_lib_err_fil_oo(RegFrame& r) { return detail::typed_reg_call<&lib_err_fil_oo>(r); }
uint32_t amos_typed_lib_err_fil_tm(RegFrame& r) { return detail::typed_reg_call<&lib_err_fil_tm>(r); }
uint32_t amos_typed_lib_err_fon_call(RegFrame& r) { return detail::typed_reg_call<&lib_err_fon_call>(r); }
uint32_t amos_typed_lib_err_iff_cmp(RegFrame& r) { return detail::typed_reg_call<&lib_err_iff_cmp>(r); }
uint32_t amos_typed_lib_err_iff_for(RegFrame& r) { return detail::typed_reg_call<&lib_err_iff_for>(r); }
uint32_t amos_typed_lib_err_iff_for2(RegFrame& r) { return detail::typed_reg_call<&lib_err_iff_for2>(r); }
uint32_t amos_typed_lib_err_ill_sc_n(RegFrame& r) { return detail::typed_reg_call<&lib_err_ill_sc_n>(r); }
uint32_t amos_typed_lib_err_oo_f_mem(RegFrame& r) { return detail::typed_reg_call<&lib_err_oo_f_mem>(r); }
uint32_t amos_typed_lib_err_rain_err(RegFrame& r) { return detail::typed_reg_call<&lib_err_rain_err>(r); }
uint32_t amos_typed_lib_err_sc_n_op(RegFrame& r) { return detail::typed_reg_call<&lib_err_sc_n_op>(r); }
uint32_t amos_typed_lib_err_sp_err(RegFrame& r) { return detail::typed_reg_call<&lib_err_sp_err>(r); }
uint32_t amos_typed_lib_err_stoo_long(RegFrame& r) { return detail::typed_reg_call<&lib_err_stoo_long>(r); }
uint32_t amos_typed_lib_err_type_mis(RegFrame& r) { return detail::typed_reg_call<&lib_err_type_mis>(r); }
uint32_t amos_typed_lib_err_w_fon_call(RegFrame& r) { return detail::typed_reg_call<&lib_err_w_fon_call>(r); }
uint64_t amos_typed_lib_fn_else(const uint32_t* words) { return typed_library_call<&lib_fn_else, 0, 4>(words); }
uint64_t amos_typed_lib_fn_false(const uint32_t* words) { return typed_library_call<&lib_fn_false, 0, 4>(words); }
uint64_t amos_typed_lib_fn_null(const uint32_t* words) { return typed_library_call<&lib_fn_null, 0, 4>(words); }
uint64_t amos_typed_lib_fn_one(const uint32_t* words) { return typed_library_call<&lib_fn_one, 0, 4>(words); }
uint64_t amos_typed_lib_fn_true(const uint32_t* words) { return typed_library_call<&lib_fn_true, 0, 4>(words); }
uint64_t amos_typed_lib_fnf_e(const uint32_t* words) { return typed_library_call<&lib_fnf_e, 0, 0>(words); }
uint32_t amos_typed_lib_go_error(RegFrame& r) { return detail::typed_reg_call<&lib_go_error>(r); }
uint64_t amos_typed_lib_in_end(const uint32_t* words) { return typed_library_call<&lib_in_end, 0, 0>(words); }
uint64_t amos_typed_lib_in_error(const uint32_t* words) { return typed_library_call<&lib_in_error, 1, 0>(words); }
void amos_typed_lib_in_rem() { lib_in_rem(); }
uint64_t amos_typed_lib_in_stop(const uint32_t* words) { return typed_library_call<&lib_in_stop, 0, 0>(words); }
uint32_t amos_typed_lib_init(RegFrame& r) { return detail::typed_reg_call<&lib_init>(r); }
uint32_t amos_typed_lib_load_regs(RegFrame& r) { return detail::typed_reg_call<&lib_load_regs>(r); }
uint32_t amos_typed_lib_maj_d0(RegFrame& r) { return detail::typed_reg_call<&lib_maj_d0>(r); }
uint32_t amos_typed_lib_r_ill_dir(RegFrame& r) { return detail::typed_reg_call<&lib_r_ill_dir>(r); }
uint32_t amos_typed_lib_save_regs(RegFrame& r) { return detail::typed_reg_call<&lib_save_regs>(r); }
uint64_t amos_typed_lib_syntax(const uint32_t* words) { return typed_library_call<&lib_syntax, 0, 0>(words); }
}
}
