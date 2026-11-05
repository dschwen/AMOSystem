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
uint64_t amos_typed_lib_fn_amalerr(const uint32_t* words) { return typed_library_call<&lib_fn_amalerr, 0, 4>(words); }
uint64_t amos_typed_lib_fn_amreg1(const uint32_t* words) { return typed_library_call<&lib_fn_amreg1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_amreg2(const uint32_t* words) { return typed_library_call<&lib_fn_amreg2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_chanan(const uint32_t* words) { return typed_library_call<&lib_fn_chanan, 1, 4>(words); }
uint64_t amos_typed_lib_fn_chanmv(const uint32_t* words) { return typed_library_call<&lib_fn_chanmv, 1, 4>(words); }
uint64_t amos_typed_lib_fn_movon(const uint32_t* words) { return typed_library_call<&lib_fn_movon, 1, 4>(words); }
uint64_t amos_typed_lib_in_amal2(const uint32_t* words) { return typed_library_call<&lib_in_amal2, 2, 0>(words); }
uint64_t amos_typed_lib_in_amal3(const uint32_t* words) { return typed_library_call<&lib_in_amal3, 3, 0>(words); }
uint64_t amos_typed_lib_in_amal_freeze0(const uint32_t* words) { return typed_library_call<&lib_in_amal_freeze0, 0, 0>(words); }
uint64_t amos_typed_lib_in_amal_freeze1(const uint32_t* words) { return typed_library_call<&lib_in_amal_freeze1, 1, 0>(words); }
uint64_t amos_typed_lib_in_amal_off0(const uint32_t* words) { return typed_library_call<&lib_in_amal_off0, 0, 0>(words); }
uint64_t amos_typed_lib_in_amal_off1(const uint32_t* words) { return typed_library_call<&lib_in_amal_off1, 1, 0>(words); }
uint64_t amos_typed_lib_in_amal_on0(const uint32_t* words) { return typed_library_call<&lib_in_amal_on0, 0, 0>(words); }
uint64_t amos_typed_lib_in_amal_on1(const uint32_t* words) { return typed_library_call<&lib_in_amal_on1, 1, 0>(words); }
uint64_t amos_typed_lib_in_amplay2(const uint32_t* words) { return typed_library_call<&lib_in_amplay2, 2, 0>(words); }
uint64_t amos_typed_lib_in_amplay4(const uint32_t* words) { return typed_library_call<&lib_in_amplay4, 4, 0>(words); }
uint64_t amos_typed_lib_in_amreg1(const uint32_t* words) { return typed_library_call<&lib_in_amreg1, 2, 0>(words); }
uint64_t amos_typed_lib_in_amreg2(const uint32_t* words) { return typed_library_call<&lib_in_amreg2, 3, 0>(words); }
uint64_t amos_typed_lib_in_anim2(const uint32_t* words) { return typed_library_call<&lib_in_anim2, 2, 0>(words); }
uint64_t amos_typed_lib_in_anim3(const uint32_t* words) { return typed_library_call<&lib_in_anim3, 3, 0>(words); }
uint64_t amos_typed_lib_in_anim_freeze0(const uint32_t* words) { return typed_library_call<&lib_in_anim_freeze0, 0, 0>(words); }
uint64_t amos_typed_lib_in_anim_freeze1(const uint32_t* words) { return typed_library_call<&lib_in_anim_freeze1, 1, 0>(words); }
uint64_t amos_typed_lib_in_anim_off0(const uint32_t* words) { return typed_library_call<&lib_in_anim_off0, 0, 0>(words); }
uint64_t amos_typed_lib_in_anim_off1(const uint32_t* words) { return typed_library_call<&lib_in_anim_off1, 1, 0>(words); }
uint64_t amos_typed_lib_in_anim_on0(const uint32_t* words) { return typed_library_call<&lib_in_anim_on0, 0, 0>(words); }
uint64_t amos_typed_lib_in_anim_on1(const uint32_t* words) { return typed_library_call<&lib_in_anim_on1, 1, 0>(words); }
uint64_t amos_typed_lib_in_freeze(const uint32_t* words) { return typed_library_call<&lib_in_freeze, 0, 0>(words); }
uint64_t amos_typed_lib_in_move_freeze0(const uint32_t* words) { return typed_library_call<&lib_in_move_freeze0, 0, 0>(words); }
uint64_t amos_typed_lib_in_move_freeze1(const uint32_t* words) { return typed_library_call<&lib_in_move_freeze1, 1, 0>(words); }
uint64_t amos_typed_lib_in_move_off0(const uint32_t* words) { return typed_library_call<&lib_in_move_off0, 0, 0>(words); }
uint64_t amos_typed_lib_in_move_off1(const uint32_t* words) { return typed_library_call<&lib_in_move_off1, 1, 0>(words); }
uint64_t amos_typed_lib_in_move_on0(const uint32_t* words) { return typed_library_call<&lib_in_move_on0, 0, 0>(words); }
uint64_t amos_typed_lib_in_move_on1(const uint32_t* words) { return typed_library_call<&lib_in_move_on1, 1, 0>(words); }
uint64_t amos_typed_lib_in_move_x2(const uint32_t* words) { return typed_library_call<&lib_in_move_x2, 2, 0>(words); }
uint64_t amos_typed_lib_in_move_x3(const uint32_t* words) { return typed_library_call<&lib_in_move_x3, 3, 0>(words); }
uint64_t amos_typed_lib_in_move_y2(const uint32_t* words) { return typed_library_call<&lib_in_move_y2, 2, 0>(words); }
uint64_t amos_typed_lib_in_move_y3(const uint32_t* words) { return typed_library_call<&lib_in_move_y3, 3, 0>(words); }
uint64_t amos_typed_lib_in_synchro(const uint32_t* words) { return typed_library_call<&lib_in_synchro, 0, 0>(words); }
uint64_t amos_typed_lib_in_synchro_off(const uint32_t* words) { return typed_library_call<&lib_in_synchro_off, 0, 0>(words); }
uint64_t amos_typed_lib_in_synchro_on(const uint32_t* words) { return typed_library_call<&lib_in_synchro_on, 0, 0>(words); }
uint64_t amos_typed_lib_in_unfreeze(const uint32_t* words) { return typed_library_call<&lib_in_unfreeze, 0, 0>(words); }
}
}
