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
uint32_t amos_typed_lib_ff_comp(RegFrame& r) { return detail::typed_reg_call<&lib_ff_comp>(r); }
uint32_t amos_typed_lib_ff_comp2(RegFrame& r) { return detail::typed_reg_call<&lib_ff_comp2>(r); }
uint32_t amos_typed_lib_fill_all(RegFrame& r) { return detail::typed_reg_call<&lib_fill_all>(r); }
uint32_t amos_typed_lib_fill_dev(RegFrame& r) { return detail::typed_reg_call<&lib_fill_dev>(r); }
uint32_t amos_typed_lib_fill_ffind(RegFrame& r) { return detail::typed_reg_call<&lib_fill_ffind>(r); }
uint32_t amos_typed_lib_fill_ffree(RegFrame& r) { return detail::typed_reg_call<&lib_fill_ffree>(r); }
uint32_t amos_typed_lib_fill_first(RegFrame& r) { return detail::typed_reg_call<&lib_fill_first>(r); }
uint32_t amos_typed_lib_fill_fpoke(RegFrame& r) { return detail::typed_reg_call<&lib_fill_fpoke>(r); }
void amos_typed_lib_fill_free() { lib_fill_free(); }
uint32_t amos_typed_lib_fill_get(RegFrame& r) { return detail::typed_reg_call<&lib_fill_get>(r); }
uint32_t amos_typed_lib_fill_nxt(RegFrame& r) { return detail::typed_reg_call<&lib_fill_nxt>(r); }
uint32_t amos_typed_lib_fill_sort(RegFrame& r) { return detail::typed_reg_call<&lib_fill_sort>(r); }
uint64_t amos_typed_lib_fn_dev_first(const uint32_t* words) { return typed_library_call<&lib_fn_dev_first, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dir_first(const uint32_t* words) { return typed_library_call<&lib_fn_dir_first, 1, 4>(words); }
uint64_t amos_typed_lib_fn_fill_next(const uint32_t* words) { return typed_library_call<&lib_fn_fill_next, 0, 4>(words); }
uint64_t amos_typed_lib_in_assign(const uint32_t* words) { return typed_library_call<&lib_in_assign, 2, 0>(words); }
uint64_t amos_typed_lib_in_dir0(const uint32_t* words) { return typed_library_call<&lib_in_dir0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dir1(const uint32_t* words) { return typed_library_call<&lib_in_dir1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dirw0(const uint32_t* words) { return typed_library_call<&lib_in_dirw0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dirw1(const uint32_t* words) { return typed_library_call<&lib_in_dirw1, 1, 0>(words); }
uint64_t amos_typed_lib_in_ldir0(const uint32_t* words) { return typed_library_call<&lib_in_ldir0, 0, 0>(words); }
uint64_t amos_typed_lib_in_ldir1(const uint32_t* words) { return typed_library_call<&lib_in_ldir1, 1, 0>(words); }
uint64_t amos_typed_lib_in_ldirw0(const uint32_t* words) { return typed_library_call<&lib_in_ldirw0, 0, 0>(words); }
uint64_t amos_typed_lib_in_ldirw1(const uint32_t* words) { return typed_library_call<&lib_in_ldirw1, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_dir1(const uint32_t* words) { return typed_library_call<&lib_in_set_dir1, 1, 0>(words); }
uint64_t amos_typed_lib_in_set_dir2(const uint32_t* words) { return typed_library_call<&lib_in_set_dir2, 2, 0>(words); }
uint32_t amos_typed_lib_joker(RegFrame& r) { return detail::typed_reg_call<&lib_joker>(r); }
uint32_t amos_typed_lib_nom_dir(RegFrame& r) { return detail::typed_reg_call<&lib_nom_dir>(r); }
}
}
