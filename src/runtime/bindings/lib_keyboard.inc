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
uint64_t amos_typed_lib_fn_fire(const uint32_t* words) { return typed_library_call<&lib_fn_fire, 1, 4>(words); }
uint64_t amos_typed_lib_fn_inkey(const uint32_t* words) { return typed_library_call<&lib_fn_inkey, 0, 4>(words); }
uint64_t amos_typed_lib_fn_jdown(const uint32_t* words) { return typed_library_call<&lib_fn_jdown, 1, 4>(words); }
uint64_t amos_typed_lib_fn_jleft(const uint32_t* words) { return typed_library_call<&lib_fn_jleft, 1, 4>(words); }
uint64_t amos_typed_lib_fn_joy(const uint32_t* words) { return typed_library_call<&lib_fn_joy, 1, 4>(words); }
uint64_t amos_typed_lib_fn_jright(const uint32_t* words) { return typed_library_call<&lib_fn_jright, 1, 4>(words); }
uint64_t amos_typed_lib_fn_jup(const uint32_t* words) { return typed_library_call<&lib_fn_jup, 1, 4>(words); }
uint64_t amos_typed_lib_fn_key(const uint32_t* words) { return typed_library_call<&lib_fn_key, 1, 4>(words); }
uint64_t amos_typed_lib_fn_key_shift(const uint32_t* words) { return typed_library_call<&lib_fn_key_shift, 0, 4>(words); }
uint64_t amos_typed_lib_fn_key_state(const uint32_t* words) { return typed_library_call<&lib_fn_key_state, 1, 4>(words); }
uint64_t amos_typed_lib_fn_scan1(const uint32_t* words) { return typed_library_call<&lib_fn_scan1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_scan2(const uint32_t* words) { return typed_library_call<&lib_fn_scan2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_scancode(const uint32_t* words) { return typed_library_call<&lib_fn_scancode, 0, 4>(words); }
uint64_t amos_typed_lib_fn_scanshift(const uint32_t* words) { return typed_library_call<&lib_fn_scanshift, 0, 4>(words); }
uint64_t amos_typed_lib_in_clear_key(const uint32_t* words) { return typed_library_call<&lib_in_clear_key, 0, 0>(words); }
uint64_t amos_typed_lib_in_key(const uint32_t* words) { return typed_library_call<&lib_in_key, 2, 0>(words); }
uint64_t amos_typed_lib_in_put_key(const uint32_t* words) { return typed_library_call<&lib_in_put_key, 1, 0>(words); }
}
}
