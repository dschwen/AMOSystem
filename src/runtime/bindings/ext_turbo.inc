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
uint64_t amos_typed_turbo_amos_pri(const uint32_t* words) { return typed_library_call<&turbo_amos_pri, 1, 0>(words); }
uint32_t amos_typed_turbo_cold(RegFrame& r) { return detail::typed_reg_call<&turbo_cold>(r); }
void amos_typed_turbo_default() { turbo_default(); }
void amos_typed_turbo_end() { turbo_end(); }
uint64_t amos_typed_turbo_f_plot(const uint32_t* words) { return typed_library_call<&turbo_f_plot, 3, 0>(words); }
uint64_t amos_typed_turbo_f_point(const uint32_t* words) { return typed_library_call<&turbo_f_point, 2, 4>(words); }
uint64_t amos_typed_turbo_l_swap(const uint32_t* words) { return typed_library_call<&turbo_l_swap, 1, 4>(words); }
uint64_t amos_typed_turbo_set_planes(const uint32_t* words) { return typed_library_call<&turbo_set_planes, 1, 0>(words); }
uint64_t amos_typed_turbo_word_hunt(const uint32_t* words) { return typed_library_call<&turbo_word_hunt, 5, 4>(words); }
}
}
