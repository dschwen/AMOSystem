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
uint64_t amos_typed_lib_fn_areg(const uint32_t* words) { return typed_library_call<&lib_fn_areg, 1, 4>(words); }
uint64_t amos_typed_lib_fn_deek(const uint32_t* words) { return typed_library_call<&lib_fn_deek, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dreg(const uint32_t* words) { return typed_library_call<&lib_fn_dreg, 1, 4>(words); }
uint64_t amos_typed_lib_fn_hunt(const uint32_t* words) { return typed_library_call<&lib_fn_hunt, 3, 4>(words); }
uint64_t amos_typed_lib_fn_leek(const uint32_t* words) { return typed_library_call<&lib_fn_leek, 1, 4>(words); }
uint64_t amos_typed_lib_fn_peek(const uint32_t* words) { return typed_library_call<&lib_fn_peek, 1, 4>(words); }
uint64_t amos_typed_lib_fn_peek_string2(const uint32_t* words) { return typed_library_call<&lib_fn_peek_string2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_peek_string3(const uint32_t* words) { return typed_library_call<&lib_fn_peek_string3, 3, 4>(words); }
uint64_t amos_typed_lib_in_areg(const uint32_t* words) { return typed_library_call<&lib_in_areg, 2, 0>(words); }
uint64_t amos_typed_lib_in_copy(const uint32_t* words) { return typed_library_call<&lib_in_copy, 3, 0>(words); }
uint64_t amos_typed_lib_in_doke(const uint32_t* words) { return typed_library_call<&lib_in_doke, 2, 0>(words); }
uint64_t amos_typed_lib_in_dreg(const uint32_t* words) { return typed_library_call<&lib_in_dreg, 2, 0>(words); }
uint64_t amos_typed_lib_in_fill(const uint32_t* words) { return typed_library_call<&lib_in_fill, 3, 0>(words); }
uint64_t amos_typed_lib_in_loke(const uint32_t* words) { return typed_library_call<&lib_in_loke, 2, 0>(words); }
uint64_t amos_typed_lib_in_poke(const uint32_t* words) { return typed_library_call<&lib_in_poke, 2, 0>(words); }
uint64_t amos_typed_lib_in_poke_string(const uint32_t* words) { return typed_library_call<&lib_in_poke_string, 2, 0>(words); }
uint32_t amos_typed_lib_trans_mem(RegFrame& r) { return detail::typed_reg_call<&lib_trans_mem>(r); }
}
}
