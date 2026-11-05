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
uint32_t amos_typed_lib_led_init(RegFrame& r) { return detail::typed_reg_call<&lib_led_init>(r); }
uint32_t amos_typed_lib_led_loop(RegFrame& r) { return detail::typed_reg_call<&lib_led_loop>(r); }
uint32_t amos_typed_lib_led_new(RegFrame& r) { return detail::typed_reg_call<&lib_led_new>(r); }
}
}
