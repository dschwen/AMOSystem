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
uint32_t amos_typed_lib_bnk_ad_bob(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_ad_bob>(r); }
uint32_t amos_typed_lib_bnk_ad_icon(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_ad_icon>(r); }
uint32_t amos_typed_lib_bnk_change(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_change>(r); }
uint32_t amos_typed_lib_bnk_cur_program(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_cur_program>(r); }
uint32_t amos_typed_lib_bnk_del_bob(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_del_bob>(r); }
uint32_t amos_typed_lib_bnk_eff(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_eff>(r); }
uint32_t amos_typed_lib_bnk_eff_a0(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_eff_a0>(r); }
uint32_t amos_typed_lib_bnk_eff_all(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_eff_all>(r); }
uint32_t amos_typed_lib_bnk_eff_bob_a0(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_eff_bob_a0>(r); }
uint32_t amos_typed_lib_bnk_eff_temp(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_eff_temp>(r); }
uint32_t amos_typed_lib_bnk_get_adr(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_get_adr>(r); }
uint32_t amos_typed_lib_bnk_get_bobs(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_get_bobs>(r); }
uint32_t amos_typed_lib_bnk_get_icons(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_get_icons>(r); }
uint32_t amos_typed_lib_bnk_get_length(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_get_length>(r); }
uint32_t amos_typed_lib_bnk_ins_bob(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_ins_bob>(r); }
uint32_t amos_typed_lib_bnk_length(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_length>(r); }
uint32_t amos_typed_lib_bnk_or_adr(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_or_adr>(r); }
uint32_t amos_typed_lib_bnk_prev_program(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_prev_program>(r); }
uint32_t amos_typed_lib_bnk_res_bob(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_res_bob>(r); }
uint32_t amos_typed_lib_bnk_res_ico(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_res_ico>(r); }
uint32_t amos_typed_lib_bnk_reserve(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_reserve>(r); }
uint32_t amos_typed_lib_bnk_ric(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_ric>(r); }
uint32_t amos_typed_lib_bnk_schrink(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_schrink>(r); }
uint32_t amos_typed_lib_bnk_un_rev(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_un_rev>(r); }
uint64_t amos_typed_lib_fn_blength(const uint32_t* words) { return typed_library_call<&lib_fn_blength, 1, 4>(words); }
uint64_t amos_typed_lib_fn_bstart(const uint32_t* words) { return typed_library_call<&lib_fn_bstart, 1, 4>(words); }
uint64_t amos_typed_lib_fn_chip_free(const uint32_t* words) { return typed_library_call<&lib_fn_chip_free, 0, 4>(words); }
uint64_t amos_typed_lib_fn_fast_free(const uint32_t* words) { return typed_library_call<&lib_fn_fast_free, 0, 4>(words); }
uint64_t amos_typed_lib_fn_length(const uint32_t* words) { return typed_library_call<&lib_fn_length, 1, 4>(words); }
uint64_t amos_typed_lib_fn_start(const uint32_t* words) { return typed_library_call<&lib_fn_start, 1, 4>(words); }
uint64_t amos_typed_lib_in_bank_schrink(const uint32_t* words) { return typed_library_call<&lib_in_bank_schrink, 2, 0>(words); }
uint64_t amos_typed_lib_in_bank_swap(const uint32_t* words) { return typed_library_call<&lib_in_bank_swap, 2, 0>(words); }
uint64_t amos_typed_lib_in_bgrab(const uint32_t* words) { return typed_library_call<&lib_in_bgrab, 1, 0>(words); }
uint64_t amos_typed_lib_in_bsend(const uint32_t* words) { return typed_library_call<&lib_in_bsend, 1, 0>(words); }
uint64_t amos_typed_lib_in_del_icon1(const uint32_t* words) { return typed_library_call<&lib_in_del_icon1, 1, 0>(words); }
uint64_t amos_typed_lib_in_del_icon2(const uint32_t* words) { return typed_library_call<&lib_in_del_icon2, 2, 0>(words); }
uint64_t amos_typed_lib_in_del_sprite1(const uint32_t* words) { return typed_library_call<&lib_in_del_sprite1, 1, 0>(words); }
uint64_t amos_typed_lib_in_del_sprite2(const uint32_t* words) { return typed_library_call<&lib_in_del_sprite2, 2, 0>(words); }
uint64_t amos_typed_lib_in_erase(const uint32_t* words) { return typed_library_call<&lib_in_erase, 1, 0>(words); }
uint64_t amos_typed_lib_in_erase_all(const uint32_t* words) { return typed_library_call<&lib_in_erase_all, 0, 0>(words); }
uint64_t amos_typed_lib_in_erase_temp(const uint32_t* words) { return typed_library_call<&lib_in_erase_temp, 0, 0>(words); }
uint64_t amos_typed_lib_in_ins_icon(const uint32_t* words) { return typed_library_call<&lib_in_ins_icon, 1, 0>(words); }
uint64_t amos_typed_lib_in_ins_sprite(const uint32_t* words) { return typed_library_call<&lib_in_ins_sprite, 1, 0>(words); }
uint64_t amos_typed_lib_in_list_bank(const uint32_t* words) { return typed_library_call<&lib_in_list_bank, 0, 0>(words); }
uint64_t amos_typed_lib_in_res_chip_data(const uint32_t* words) { return typed_library_call<&lib_in_res_chip_data, 2, 0>(words); }
uint64_t amos_typed_lib_in_res_chip_work(const uint32_t* words) { return typed_library_call<&lib_in_res_chip_work, 2, 0>(words); }
uint64_t amos_typed_lib_in_res_data(const uint32_t* words) { return typed_library_call<&lib_in_res_data, 2, 0>(words); }
uint64_t amos_typed_lib_in_res_work(const uint32_t* words) { return typed_library_call<&lib_in_res_work, 2, 0>(words); }
uint32_t amos_typed_lib_lst_chip_new(RegFrame& r) { return detail::typed_reg_call<&lib_lst_chip_new>(r); }
uint32_t amos_typed_lib_lst_cree(RegFrame& r) { return detail::typed_reg_call<&lib_lst_cree>(r); }
uint32_t amos_typed_lib_lst_del(RegFrame& r) { return detail::typed_reg_call<&lib_lst_del>(r); }
uint32_t amos_typed_lib_lst_del_all(RegFrame& r) { return detail::typed_reg_call<&lib_lst_del_all>(r); }
uint32_t amos_typed_lib_lst_insert(RegFrame& r) { return detail::typed_reg_call<&lib_lst_insert>(r); }
uint32_t amos_typed_lib_lst_new(RegFrame& r) { return detail::typed_reg_call<&lib_lst_new>(r); }
uint32_t amos_typed_lib_lst_remove(RegFrame& r) { return detail::typed_reg_call<&lib_lst_remove>(r); }
}
}
