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
void amos_typed_lib_arx_close() { lib_arx_close(); }
uint64_t amos_typed_lib_fn_arexx(const uint32_t* words) { return typed_library_call<&lib_fn_arexx, 0, 4>(words); }
uint64_t amos_typed_lib_fn_arexx_exist(const uint32_t* words) { return typed_library_call<&lib_fn_arexx_exist, 1, 4>(words); }
uint64_t amos_typed_lib_fn_arexxd(const uint32_t* words) { return typed_library_call<&lib_fn_arexxd, 1, 4>(words); }
uint64_t amos_typed_lib_in_arexx_answer1(const uint32_t* words) { return typed_library_call<&lib_in_arexx_answer1, 1, 0>(words); }
uint64_t amos_typed_lib_in_arexx_answer2(const uint32_t* words) { return typed_library_call<&lib_in_arexx_answer2, 2, 0>(words); }
uint64_t amos_typed_lib_in_arexx_close(const uint32_t* words) { return typed_library_call<&lib_in_arexx_close, 0, 0>(words); }
uint64_t amos_typed_lib_in_arexx_open(const uint32_t* words) { return typed_library_call<&lib_in_arexx_open, 1, 0>(words); }
uint64_t amos_typed_lib_in_arexx_wait(const uint32_t* words) { return typed_library_call<&lib_in_arexx_wait, 0, 0>(words); }
}
}
