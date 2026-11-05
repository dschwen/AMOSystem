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
void amos_typed_lib_close_current() { lib_close_current(); }
uint32_t amos_typed_lib_d_close(RegFrame& r) { return detail::typed_reg_call<&lib_d_close>(r); }
uint32_t amos_typed_lib_d_open(RegFrame& r) { return detail::typed_reg_call<&lib_d_open>(r); }
uint32_t amos_typed_lib_d_open_d1(RegFrame& r) { return detail::typed_reg_call<&lib_d_open_d1>(r); }
uint32_t amos_typed_lib_d_read(RegFrame& r) { return detail::typed_reg_call<&lib_d_read>(r); }
uint32_t amos_typed_lib_d_seek(RegFrame& r) { return detail::typed_reg_call<&lib_d_seek>(r); }
uint32_t amos_typed_lib_d_write(RegFrame& r) { return detail::typed_reg_call<&lib_d_write>(r); }
uint32_t amos_typed_lib_disk_err(RegFrame& r) { return detail::typed_reg_call<&lib_disk_err>(r); }
uint32_t amos_typed_lib_disk_error(RegFrame& r) { return detail::typed_reg_call<&lib_disk_error>(r); }
uint32_t amos_typed_lib_dsk_dnom(RegFrame& r) { return detail::typed_reg_call<&lib_dsk_dnom>(r); }
uint32_t amos_typed_lib_dsk_path_it(RegFrame& r) { return detail::typed_reg_call<&lib_dsk_path_it>(r); }
uint32_t amos_typed_lib_nom_disc(RegFrame& r) { return detail::typed_reg_call<&lib_nom_disc>(r); }
}
}
