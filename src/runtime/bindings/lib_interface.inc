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
uint32_t amos_typed_lib_dia_get_default(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_default>(r); }
uint32_t amos_typed_lib_dia_get_puzzle(RegFrame& r) { return detail::typed_reg_call<&lib_dia_get_puzzle>(r); }
uint32_t amos_typed_lib_dia_warm_init(RegFrame& r) { return detail::typed_reg_call<&lib_dia_warm_init>(r); }
uint64_t amos_typed_lib_fn_dialog(const uint32_t* words) { return typed_library_call<&lib_fn_dialog, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_box1(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_box1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_box2(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_box2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_box3(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_box3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_box5(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_box5, 5, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_run1(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_run1, 1, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_run2(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_run2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_dialog_run4(const uint32_t* words) { return typed_library_call<&lib_fn_dialog_run4, 4, 4>(words); }
uint64_t amos_typed_lib_fn_edialog(const uint32_t* words) { return typed_library_call<&lib_fn_edialog, 0, 4>(words); }
uint64_t amos_typed_lib_fn_rdialog2(const uint32_t* words) { return typed_library_call<&lib_fn_rdialog2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_rdialog3(const uint32_t* words) { return typed_library_call<&lib_fn_rdialog3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_rdialogd2(const uint32_t* words) { return typed_library_call<&lib_fn_rdialogd2, 2, 4>(words); }
uint64_t amos_typed_lib_fn_rdialogd3(const uint32_t* words) { return typed_library_call<&lib_fn_rdialogd3, 3, 4>(words); }
uint64_t amos_typed_lib_fn_vdialog(const uint32_t* words) { return typed_library_call<&lib_fn_vdialog, 2, 4>(words); }
uint64_t amos_typed_lib_fn_zdialog(const uint32_t* words) { return typed_library_call<&lib_fn_zdialog, 3, 4>(words); }
uint64_t amos_typed_lib_in_dialog_close0(const uint32_t* words) { return typed_library_call<&lib_in_dialog_close0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dialog_close1(const uint32_t* words) { return typed_library_call<&lib_in_dialog_close1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dialog_clr(const uint32_t* words) { return typed_library_call<&lib_in_dialog_clr, 1, 0>(words); }
uint64_t amos_typed_lib_in_dialog_freeze0(const uint32_t* words) { return typed_library_call<&lib_in_dialog_freeze0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dialog_freeze1(const uint32_t* words) { return typed_library_call<&lib_in_dialog_freeze1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dialog_open2(const uint32_t* words) { return typed_library_call<&lib_in_dialog_open2, 2, 0>(words); }
uint64_t amos_typed_lib_in_dialog_open3(const uint32_t* words) { return typed_library_call<&lib_in_dialog_open3, 3, 0>(words); }
uint64_t amos_typed_lib_in_dialog_open4(const uint32_t* words) { return typed_library_call<&lib_in_dialog_open4, 4, 0>(words); }
uint64_t amos_typed_lib_in_dialog_unfreeze0(const uint32_t* words) { return typed_library_call<&lib_in_dialog_unfreeze0, 0, 0>(words); }
uint64_t amos_typed_lib_in_dialog_unfreeze1(const uint32_t* words) { return typed_library_call<&lib_in_dialog_unfreeze1, 1, 0>(words); }
uint64_t amos_typed_lib_in_dialog_update2(const uint32_t* words) { return typed_library_call<&lib_in_dialog_update2, 2, 0>(words); }
uint64_t amos_typed_lib_in_dialog_update3(const uint32_t* words) { return typed_library_call<&lib_in_dialog_update3, 3, 0>(words); }
uint64_t amos_typed_lib_in_dialog_update4(const uint32_t* words) { return typed_library_call<&lib_in_dialog_update4, 4, 0>(words); }
uint64_t amos_typed_lib_in_dialog_update5(const uint32_t* words) { return typed_library_call<&lib_in_dialog_update5, 5, 0>(words); }
uint64_t amos_typed_lib_in_read_text1(const uint32_t* words) { return typed_library_call<&lib_in_read_text1, 1, 0>(words); }
uint64_t amos_typed_lib_in_read_text3(const uint32_t* words) { return typed_library_call<&lib_in_read_text3, 3, 0>(words); }
uint64_t amos_typed_lib_in_resource_bank(const uint32_t* words) { return typed_library_call<&lib_in_resource_bank, 1, 0>(words); }
uint64_t amos_typed_lib_in_resource_screen_open(const uint32_t* words) { return typed_library_call<&lib_in_resource_screen_open, 4, 0>(words); }
uint64_t amos_typed_lib_in_resource_unpack(const uint32_t* words) { return typed_library_call<&lib_in_resource_unpack, 3, 0>(words); }
uint64_t amos_typed_lib_in_vdialog(const uint32_t* words) { return typed_library_call<&lib_in_vdialog, 3, 0>(words); }
}
}
