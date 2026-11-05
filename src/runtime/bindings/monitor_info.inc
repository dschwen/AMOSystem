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
void amos_typed_mon_activate(AllRegs* r) { mon_activate(r); }
void amos_typed_mon_activate_info(AllRegs* r) { mon_activate_info(r); }
void amos_typed_mon_activate_text(AllRegs* r) { mon_activate_text(r); }
void amos_typed_mon_alert(AllRegs* r) { mon_alert(r); }
void amos_typed_mon_alert_delete(AllRegs* r) { mon_alert_delete(r); }
void amos_typed_mon_banks_monitor(AllRegs* r) { mon_banks_monitor(r); }
void amos_typed_mon_banks_program(AllRegs* r) { mon_banks_program(r); }
void amos_typed_mon_delete_show(AllRegs* r) { mon_delete_show(r); }
void amos_typed_mon_info_begin(AllRegs* r) { mon_info_begin(r); }
void amos_typed_mon_info_centre(AllRegs* r) { mon_info_centre(r); }
void amos_typed_mon_info_click(AllRegs* r) { mon_info_click(r); }
void amos_typed_mon_info_count_lines(AllRegs* r) { mon_info_count_lines(r); }
void amos_typed_mon_info_delete(AllRegs* r) { mon_info_delete(r); }
void amos_typed_mon_info_end(AllRegs* r) { mon_info_end(r); }
void amos_typed_mon_info_insert(AllRegs* r) { mon_info_insert(r); }
void amos_typed_mon_info_print(AllRegs* r) { mon_info_print(r); }
void amos_typed_mon_info_show_all(AllRegs* r) { mon_info_show_all(r); }
void amos_typed_mon_info_show_line(AllRegs* r) { mon_info_show_line(r); }
void amos_typed_mon_info_slider(AllRegs* r) { mon_info_slider(r); }
void amos_typed_mon_info_slider_click(AllRegs* r) { mon_info_slider_click(r); }
void amos_typed_mon_message(AllRegs* r) { mon_message(r); }
void amos_typed_mon_wait_release(AllRegs* r) { mon_wait_release(r); }
}
}
