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
uint64_t amos_typed_amcaf_turbo_draw_5(const uint32_t* words) { return typed_library_call<&amcaf_turbo_draw_5, 5, 0>(words); }
uint64_t amos_typed_amcaf_turbo_draw_6(const uint32_t* words) { return typed_library_call<&amcaf_turbo_draw_6, 6, 0>(words); }
uint64_t amos_typed_amcaf_turbo_plot(const uint32_t* words) { return typed_library_call<&amcaf_turbo_plot, 3, 0>(words); }
uint64_t amos_typed_amcaf_turbo_point(const uint32_t* words) { return typed_library_call<&amcaf_turbo_point, 2, 4>(words); }
}
}
