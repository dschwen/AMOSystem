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
uint64_t amos_typed_amcaf_aga_notation_off(const uint32_t* words) { return typed_library_call<&amcaf_aga_notation_off, 0, 0>(words); }
uint64_t amos_typed_amcaf_aga_notation_on(const uint32_t* words) { return typed_library_call<&amcaf_aga_notation_on, 0, 0>(words); }
uint64_t amos_typed_amcaf_best_pen_1(const uint32_t* words) { return typed_library_call<&amcaf_best_pen_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_best_pen_3(const uint32_t* words) { return typed_library_call<&amcaf_best_pen_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_blue_val(const uint32_t* words) { return typed_library_call<&amcaf_blue_val, 1, 4>(words); }
uint64_t amos_typed_amcaf_glue_colour(const uint32_t* words) { return typed_library_call<&amcaf_glue_colour, 3, 4>(words); }
uint64_t amos_typed_amcaf_green_val(const uint32_t* words) { return typed_library_call<&amcaf_green_val, 1, 4>(words); }
uint64_t amos_typed_amcaf_mix_colour_2(const uint32_t* words) { return typed_library_call<&amcaf_mix_colour_2, 2, 4>(words); }
uint64_t amos_typed_amcaf_mix_colour_4(const uint32_t* words) { return typed_library_call<&amcaf_mix_colour_4, 4, 4>(words); }
uint64_t amos_typed_amcaf_red_val(const uint32_t* words) { return typed_library_call<&amcaf_red_val, 1, 4>(words); }
uint64_t amos_typed_amcaf_rgb_to_rrggbb(const uint32_t* words) { return typed_library_call<&amcaf_rgb_to_rrggbb, 1, 4>(words); }
uint64_t amos_typed_amcaf_rrggbb_to_rgb(const uint32_t* words) { return typed_library_call<&amcaf_rrggbb_to_rgb, 1, 4>(words); }
}
}
