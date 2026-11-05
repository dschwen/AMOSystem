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
uint32_t amos_typed_lib_bnk_load(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_load>(r); }
uint32_t amos_typed_lib_bnk_save_a0(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_save_a0>(r); }
uint32_t amos_typed_lib_bnk_save_all(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_save_all>(r); }
uint32_t amos_typed_lib_bnk_save_vide(RegFrame& r) { return detail::typed_reg_call<&lib_bnk_save_vide>(r); }
uint64_t amos_typed_lib_in_load1(const uint32_t* words) { return typed_library_call<&lib_in_load1, 1, 0>(words); }
uint64_t amos_typed_lib_in_load2(const uint32_t* words) { return typed_library_call<&lib_in_load2, 2, 0>(words); }
uint64_t amos_typed_lib_in_save1(const uint32_t* words) { return typed_library_call<&lib_in_save1, 1, 0>(words); }
uint64_t amos_typed_lib_in_save2(const uint32_t* words) { return typed_library_call<&lib_in_save2, 2, 0>(words); }
}
}
