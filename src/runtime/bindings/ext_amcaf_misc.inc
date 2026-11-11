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
uint64_t amos_typed_amcaf_audio_lock(const uint32_t* words) { return typed_library_call<&amcaf_audio_lock, 0, 0>(words); }
uint64_t amos_typed_amcaf_bank_permanent(const uint32_t* words) { return typed_library_call<&amcaf_bank_permanent, 1, 0>(words); }
uint64_t amos_typed_amcaf_bank_temporary(const uint32_t* words) { return typed_library_call<&amcaf_bank_temporary, 1, 0>(words); }
uint64_t amos_typed_amcaf_even(const uint32_t* words) { return typed_library_call<&amcaf_even, 1, 4>(words); }
uint64_t amos_typed_amcaf_flush_libs(const uint32_t* words) { return typed_library_call<&amcaf_flush_libs, 0, 0>(words); }
uint64_t amos_typed_amcaf_odd(const uint32_t* words) { return typed_library_call<&amcaf_odd, 1, 4>(words); }
uint64_t amos_typed_amcaf_raster_wait_1(const uint32_t* words) { return typed_library_call<&amcaf_raster_wait_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_raster_wait_2(const uint32_t* words) { return typed_library_call<&amcaf_raster_wait_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_smouse_key(const uint32_t* words) { return typed_library_call<&amcaf_smouse_key, 0, 4>(words); }
}
}
