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
uint64_t amos_typed_amcaf_limit_smouse_0(const uint32_t* words) { return typed_library_call<&amcaf_limit_smouse_0, 0, 0>(words); }
uint64_t amos_typed_amcaf_limit_smouse_4(const uint32_t* words) { return typed_library_call<&amcaf_limit_smouse_4, 4, 0>(words); }
uint64_t amos_typed_amcaf_pfire(const uint32_t* words) { return typed_library_call<&amcaf_pfire, 1, 4>(words); }
uint64_t amos_typed_amcaf_pjdown(const uint32_t* words) { return typed_library_call<&amcaf_pjdown, 1, 4>(words); }
uint64_t amos_typed_amcaf_pjleft(const uint32_t* words) { return typed_library_call<&amcaf_pjleft, 1, 4>(words); }
uint64_t amos_typed_amcaf_pjoy(const uint32_t* words) { return typed_library_call<&amcaf_pjoy, 1, 4>(words); }
uint64_t amos_typed_amcaf_pjright(const uint32_t* words) { return typed_library_call<&amcaf_pjright, 1, 4>(words); }
uint64_t amos_typed_amcaf_pjup(const uint32_t* words) { return typed_library_call<&amcaf_pjup, 1, 4>(words); }
uint64_t amos_typed_amcaf_smouse_speed(const uint32_t* words) { return typed_library_call<&amcaf_smouse_speed, 1, 0>(words); }
uint64_t amos_typed_amcaf_smouse_x(const uint32_t* words) { return typed_library_call<&amcaf_smouse_x, 1, 0>(words); }
uint64_t amos_typed_amcaf_smouse_y(const uint32_t* words) { return typed_library_call<&amcaf_smouse_y, 1, 0>(words); }
uint64_t amos_typed_amcaf_x_smouse(const uint32_t* words) { return typed_library_call<&amcaf_x_smouse, 0, 4>(words); }
uint64_t amos_typed_amcaf_y_smouse(const uint32_t* words) { return typed_library_call<&amcaf_y_smouse, 0, 4>(words); }
}
}
