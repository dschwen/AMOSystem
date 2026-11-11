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
uint64_t amos_typed_turbo_bank_end(const uint32_t* words) { return typed_library_call<&turbo_bank_end, 1, 4>(words); }
uint64_t amos_typed_turbo_between(const uint32_t* words) { return typed_library_call<&turbo_between, 3, 4>(words); }
uint64_t amos_typed_turbo_bit_field_ext(const uint32_t* words) { return typed_library_call<&turbo_bit_field_ext, 3, 4>(words); }
uint64_t amos_typed_turbo_bit_field_ins(const uint32_t* words) { return typed_library_call<&turbo_bit_field_ins, 4, 4>(words); }
uint64_t amos_typed_turbo_byte_hunt(const uint32_t* words) { return typed_library_call<&turbo_byte_hunt, 5, 4>(words); }
uint64_t amos_typed_turbo_f_sqr(const uint32_t* words) { return typed_library_call<&turbo_f_sqr, 1, 4>(words); }
uint64_t amos_typed_turbo_left_click(const uint32_t* words) { return typed_library_call<&turbo_left_click, 0, 4>(words); }
uint64_t amos_typed_turbo_lsl_b(const uint32_t* words) { return typed_library_call<&turbo_lsl_b, 2, 4>(words); }
uint64_t amos_typed_turbo_lsl_l(const uint32_t* words) { return typed_library_call<&turbo_lsl_l, 2, 4>(words); }
uint64_t amos_typed_turbo_lsl_w(const uint32_t* words) { return typed_library_call<&turbo_lsl_w, 2, 4>(words); }
uint64_t amos_typed_turbo_lsr_b(const uint32_t* words) { return typed_library_call<&turbo_lsr_b, 2, 4>(words); }
uint64_t amos_typed_turbo_lsr_l(const uint32_t* words) { return typed_library_call<&turbo_lsr_l, 2, 4>(words); }
uint64_t amos_typed_turbo_lsr_w(const uint32_t* words) { return typed_library_call<&turbo_lsr_w, 2, 4>(words); }
uint64_t amos_typed_turbo_memory_fill(const uint32_t* words) { return typed_library_call<&turbo_memory_fill, 3, 0>(words); }
uint64_t amos_typed_turbo_move_mem(const uint32_t* words) { return typed_library_call<&turbo_move_mem, 3, 0>(words); }
uint64_t amos_typed_turbo_multi_no(const uint32_t* words) { return typed_library_call<&turbo_multi_no, 0, 0>(words); }
uint64_t amos_typed_turbo_multi_yes(const uint32_t* words) { return typed_library_call<&turbo_multi_yes, 0, 0>(words); }
uint64_t amos_typed_turbo_range(const uint32_t* words) { return typed_library_call<&turbo_range, 3, 4>(words); }
uint64_t amos_typed_turbo_right_click(const uint32_t* words) { return typed_library_call<&turbo_right_click, 0, 4>(words); }
uint64_t amos_typed_turbo_string_hunt(const uint32_t* words) { return typed_library_call<&turbo_string_hunt, 5, 4>(words); }
uint64_t amos_typed_turbo_t_clip(const uint32_t* words) { return typed_library_call<&turbo_t_clip, 2, 4>(words); }
uint64_t amos_typed_turbo_test_b(const uint32_t* words) { return typed_library_call<&turbo_test_b, 2, 4>(words); }
uint64_t amos_typed_turbo_test_w(const uint32_t* words) { return typed_library_call<&turbo_test_w, 2, 4>(words); }
uint64_t amos_typed_turbo_texp(const uint32_t* words) { return typed_library_call<&turbo_texp, 3, 4>(words); }
uint64_t amos_typed_turbo_vbl_wait(const uint32_t* words) { return typed_library_call<&turbo_vbl_wait, 1, 0>(words); }
}
}
