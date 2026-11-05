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
uint64_t amos_typed_lib_fn_asc(const uint32_t* words) { return typed_library_call<&lib_fn_asc, 1, 4>(words); }
uint64_t amos_typed_lib_fn_at(const uint32_t* words) { return typed_library_call<&lib_fn_at, 2, 4>(words); }
uint64_t amos_typed_lib_fn_bin1(const uint32_t* words) { return typed_library_call<&lib_fn_bin1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_bin2(const uint32_t* words) { return typed_library_call<&lib_fn_bin2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_border(const uint32_t* words) { return typed_library_call<&lib_fn_border, 2, 4>(words); }
uint64_t amos_typed_lib_fn_cdown(const uint32_t* words) { return typed_library_call<&lib_fn_cdown, 0, 4>(words); }
uint64_t amos_typed_lib_fn_chr(const uint32_t* words) { return typed_library_call<&lib_fn_chr, 1, 4>(words); }
uint64_t amos_typed_lib_fn_cleft(const uint32_t* words) { return typed_library_call<&lib_fn_cleft, 0, 4>(words); }
uint64_t amos_typed_lib_fn_cmove(const uint32_t* words) { return typed_library_call<&lib_fn_cmove, 2, 4>(words); }
uint64_t amos_typed_lib_fn_cright(const uint32_t* words) { return typed_library_call<&lib_fn_cright, 0, 4>(words); }
uint64_t amos_typed_lib_fn_cup(const uint32_t* words) { return typed_library_call<&lib_fn_cup, 0, 4>(words); }
uint64_t amos_typed_lib_fn_flip(const uint32_t* words) { return typed_library_call<&lib_fn_flip, 1, 4>(words); }
uint64_t amos_typed_lib_fn_free(const uint32_t* words) { return typed_library_call<&lib_fn_free, 0, 4>(words); }
uint64_t amos_typed_lib_fn_hex1(const uint32_t* words) { return typed_library_call<&lib_fn_hex1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_hex2(const uint32_t* words) { return typed_library_call<&lib_fn_hex2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_instr2(const uint32_t* words) { return typed_library_call<&lib_fn_instr2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_instr3(const uint32_t* words) { return typed_library_call<&lib_fn_instr3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_len(const uint32_t* words) { return typed_library_call<&lib_fn_len, 1, 4>(words); }
uint64_t amos_typed_lib_fn_lower(const uint32_t* words) { return typed_library_call<&lib_fn_lower, 1, 4>(words); }
uint64_t amos_typed_lib_fn_paper(const uint32_t* words) { return typed_library_call<&lib_fn_paper, 1, 4>(words); }
uint64_t amos_typed_lib_fn_pen(const uint32_t* words) { return typed_library_call<&lib_fn_pen, 1, 4>(words); }
uint64_t amos_typed_lib_fn_repeat(const uint32_t* words) { return typed_library_call<&lib_fn_repeat, 2, 4>(words); }
uint64_t amos_typed_lib_fn_space(const uint32_t* words) { return typed_library_call<&lib_fn_space, 1, 4>(words); }
uint64_t amos_typed_lib_fn_string(const uint32_t* words) { return typed_library_call<&lib_fn_string, 2, 4>(words); }
uint64_t amos_typed_lib_fn_tab(const uint32_t* words) { return typed_library_call<&lib_fn_tab, 0, 4>(words); }
uint64_t amos_typed_lib_fn_upper(const uint32_t* words) { return typed_library_call<&lib_fn_upper, 1, 4>(words); }
uint64_t amos_typed_lib_fn_zone(const uint32_t* words) { return typed_library_call<&lib_fn_zone, 2, 4>(words); }
uint32_t amos_typed_lib_instr_find_reg(RegFrame& r) { return detail::typed_reg_call<&lib_instr_find_reg>(r); }
}
}
