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
uint64_t amos_typed_amcaf_dload(const uint32_t* words) { return typed_library_call<&amcaf_dload, 2, 0>(words); }
uint64_t amos_typed_amcaf_io_error(const uint32_t* words) { return typed_library_call<&amcaf_io_error, 0, 4>(words); }
uint64_t amos_typed_amcaf_wload(const uint32_t* words) { return typed_library_call<&amcaf_wload, 2, 0>(words); }
uint64_t amos_typed_amcaf_wsave(const uint32_t* words) { return typed_library_call<&amcaf_wsave, 2, 0>(words); }
}
}
