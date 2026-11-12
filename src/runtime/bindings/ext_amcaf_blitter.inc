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
uint64_t amos_typed_amcaf_blitter_busy(const uint32_t* words) { return typed_library_call<&amcaf_blitter_busy, 0, 4>(words); }
uint64_t amos_typed_amcaf_blitter_clear_2(const uint32_t* words) { return typed_library_call<&amcaf_blitter_clear_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_blitter_clear_6(const uint32_t* words) { return typed_library_call<&amcaf_blitter_clear_6, 6, 0>(words); }
uint64_t amos_typed_amcaf_blitter_copy_limit_1(const uint32_t* words) { return typed_library_call<&amcaf_blitter_copy_limit_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_blitter_copy_limit_4(const uint32_t* words) { return typed_library_call<&amcaf_blitter_copy_limit_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_blitter_fill_2(const uint32_t* words) { return typed_library_call<&amcaf_blitter_fill_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_blitter_fill_4(const uint32_t* words) { return typed_library_call<&amcaf_blitter_fill_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_blitter_fill_6(const uint32_t* words) { return typed_library_call<&amcaf_blitter_fill_6, 6, 0>(words); }
uint64_t amos_typed_amcaf_blitter_fill_8(const uint32_t* words) { return typed_library_call<&amcaf_blitter_fill_8, 8, 0>(words); }
uint64_t amos_typed_amcaf_blitter_wait(const uint32_t* words) { return typed_library_call<&amcaf_blitter_wait, 0, 0>(words); }
}
}
