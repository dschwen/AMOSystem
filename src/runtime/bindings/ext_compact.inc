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
uint32_t amos_typed_ext_compact_cold(RegFrame& r) { return detail::typed_reg_call<&ext_compact_cold>(r); }
uint32_t amos_typed_ext_compact_custom(RegFrame& r) { return detail::typed_reg_call<&ext_compact_custom>(r); }
uint32_t amos_typed_ext_compact_custom2(RegFrame& r) { return detail::typed_reg_call<&ext_compact_custom2>(r); }
uint64_t amos_typed_ext_pack2(const uint32_t* words) { return typed_library_call<&ext_pack2, 2, 0>(words); }
uint64_t amos_typed_ext_pack6(const uint32_t* words) { return typed_library_call<&ext_pack6, 6, 0>(words); }
uint64_t amos_typed_ext_spack2(const uint32_t* words) { return typed_library_call<&ext_spack2, 2, 0>(words); }
uint64_t amos_typed_ext_spack6(const uint32_t* words) { return typed_library_call<&ext_spack6, 6, 0>(words); }
uint64_t amos_typed_ext_unpack1(const uint32_t* words) { return typed_library_call<&ext_unpack1, 1, 0>(words); }
uint64_t amos_typed_ext_unpack2(const uint32_t* words) { return typed_library_call<&ext_unpack2, 2, 0>(words); }
uint64_t amos_typed_ext_unpack3(const uint32_t* words) { return typed_library_call<&ext_unpack3, 3, 0>(words); }
}
}
