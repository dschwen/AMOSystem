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
uint32_t amos_typed_ext_request_cold(RegFrame& r) { return detail::typed_reg_call<&ext_request_cold>(r); }
uint64_t amos_typed_ext_request_off(const uint32_t* words) { return typed_library_call<&ext_request_off, 0, 0>(words); }
uint64_t amos_typed_ext_request_on(const uint32_t* words) { return typed_library_call<&ext_request_on, 0, 0>(words); }
void amos_typed_ext_request_warm() { ext_request_warm(); }
uint64_t amos_typed_ext_request_wb(const uint32_t* words) { return typed_library_call<&ext_request_wb, 0, 0>(words); }
}
}
