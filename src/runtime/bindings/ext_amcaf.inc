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
uint64_t amos_typed_amcaf_base(const uint32_t* words) { return typed_library_call<&amcaf_base, 0, 4>(words); }
uint32_t amos_typed_amcaf_cold(RegFrame& r) { return detail::typed_reg_call<&amcaf_cold>(r); }
uint64_t amos_typed_amcaf_length(const uint32_t* words) { return typed_library_call<&amcaf_length, 0, 4>(words); }
uint64_t amos_typed_amcaf_version_s(const uint32_t* words) { return typed_library_call<&amcaf_version_s, 0, 4>(words); }
}
}
