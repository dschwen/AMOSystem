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
uint64_t amos_typed_lib_fn_command_line(const uint32_t* words) { return typed_library_call<&lib_fn_command_line, 0, 4>(words); }
uint64_t amos_typed_lib_fn_form_length1(const uint32_t* words) { return typed_library_call<&lib_fn_form_length1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_form_length2(const uint32_t* words) { return typed_library_call<&lib_fn_form_length2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_form_load2(const uint32_t* words) { return typed_library_call<&lib_fn_form_load2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_form_load3(const uint32_t* words) { return typed_library_call<&lib_fn_form_load3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_form_param(const uint32_t* words) { return typed_library_call<&lib_fn_form_param, 0, 4>(words); }
uint64_t amos_typed_lib_fn_form_play2(const uint32_t* words) { return typed_library_call<&lib_fn_form_play2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_form_play3(const uint32_t* words) { return typed_library_call<&lib_fn_form_play3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_form_skip1(const uint32_t* words) { return typed_library_call<&lib_fn_form_skip1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_form_skip2(const uint32_t* words) { return typed_library_call<&lib_fn_form_skip2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_picture(const uint32_t* words) { return typed_library_call<&lib_fn_picture, 0, 4>(words); }
uint64_t amos_typed_lib_in_command_line(const uint32_t* words) { return typed_library_call<&lib_in_command_line, 1, 0>(words); }
uint64_t amos_typed_lib_in_iff_anim2(const uint32_t* words) { return typed_library_call<&lib_in_iff_anim2, 2, 0>(words); }
uint64_t amos_typed_lib_in_iff_anim3(const uint32_t* words) { return typed_library_call<&lib_in_iff_anim3, 3, 0>(words); }
uint64_t amos_typed_lib_in_load_iff1(const uint32_t* words) { return typed_library_call<&lib_in_load_iff1, 1, 0>(words); }
uint64_t amos_typed_lib_in_load_iff2(const uint32_t* words) { return typed_library_call<&lib_in_load_iff2, 2, 0>(words); }
uint64_t amos_typed_lib_in_mask_iff(const uint32_t* words) { return typed_library_call<&lib_in_mask_iff, 1, 0>(words); }
uint64_t amos_typed_lib_in_save_iff1(const uint32_t* words) { return typed_library_call<&lib_in_save_iff1, 1, 0>(words); }
uint64_t amos_typed_lib_in_save_iff2(const uint32_t* words) { return typed_library_call<&lib_in_save_iff2, 2, 0>(words); }
}
}
