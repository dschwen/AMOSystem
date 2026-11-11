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
uint64_t amos_typed_amcaf_cutstr_s(const uint32_t* words) { return typed_library_call<&amcaf_cutstr_s, 3, 4>(words); }
uint64_t amos_typed_amcaf_insstr_s(const uint32_t* words) { return typed_library_call<&amcaf_insstr_s, 3, 4>(words); }
uint64_t amos_typed_amcaf_itemstr_s_2(const uint32_t* words) { return typed_library_call<&amcaf_itemstr_s_2, 2, 4>(words); }
uint64_t amos_typed_amcaf_itemstr_s_3(const uint32_t* words) { return typed_library_call<&amcaf_itemstr_s_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_lsstr_s(const uint32_t* words) { return typed_library_call<&amcaf_lsstr_s, 2, 4>(words); }
uint64_t amos_typed_amcaf_lzstr_s(const uint32_t* words) { return typed_library_call<&amcaf_lzstr_s, 2, 4>(words); }
uint64_t amos_typed_amcaf_replacestr_s(const uint32_t* words) { return typed_library_call<&amcaf_replacestr_s, 3, 4>(words); }
uint64_t amos_typed_amcaf_scanstr_s(const uint32_t* words) { return typed_library_call<&amcaf_scanstr_s, 1, 4>(words); }
}
}
