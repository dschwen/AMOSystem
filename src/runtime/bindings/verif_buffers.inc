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
void amos_typed_ver_clear_buffers(AllRegs* r) { ver_clear_buffers(r); }
void amos_typed_ver_clear_variables(AllRegs* r) { ver_clear_variables(r); }
void amos_typed_ver_direct_variables(AllRegs* r) { ver_direct_variables(r); }
void amos_typed_ver_resize_names(AllRegs* r) { ver_resize_names(r); }
void amos_typed_ver_resize_variables(AllRegs* r) { ver_resize_variables(r); }
void amos_typed_ver_stack_reserve(AllRegs* r) { ver_stack_reserve(r); }
}
}
