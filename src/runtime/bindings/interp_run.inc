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
uint64_t amos_typed_interp_fn_prg_state(const uint32_t* words) { return typed_library_call<&interp_fn_prg_state, 0, 4>(words); }
uint64_t amos_typed_interp_fn_prg_under(const uint32_t* words) { return typed_library_call<&interp_fn_prg_under, 0, 4>(words); }
uint64_t amos_typed_interp_in_ask_editor1(const uint32_t* words) { return typed_library_call<&interp_in_ask_editor1, 1, 0>(words); }
uint64_t amos_typed_interp_in_ask_editor2(const uint32_t* words) { return typed_library_call<&interp_in_ask_editor2, 2, 0>(words); }
uint64_t amos_typed_interp_in_ask_editor3(const uint32_t* words) { return typed_library_call<&interp_in_ask_editor3, 3, 0>(words); }
uint64_t amos_typed_interp_in_call_editor1(const uint32_t* words) { return typed_library_call<&interp_in_call_editor1, 1, 0>(words); }
uint64_t amos_typed_interp_in_call_editor2(const uint32_t* words) { return typed_library_call<&interp_in_call_editor2, 2, 0>(words); }
uint64_t amos_typed_interp_in_call_editor3(const uint32_t* words) { return typed_library_call<&interp_in_call_editor3, 3, 0>(words); }
uint64_t amos_typed_interp_in_prun(const uint32_t* words) { return typed_library_call<&interp_in_prun, 1, 0>(words); }
uint64_t amos_typed_interp_in_run0(const uint32_t* words) { return typed_library_call<&interp_in_run0, 0, 0>(words); }
uint64_t amos_typed_interp_in_run1(const uint32_t* words) { return typed_library_call<&interp_in_run1, 1, 0>(words); }
}
}
