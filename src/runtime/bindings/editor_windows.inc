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
void amos_typed_ed_activate(AllRegs* r) { ed_activate(r); }
void amos_typed_ed_bottom_down(AllRegs* r) { ed_bottom_down(r); }
void amos_typed_ed_bottom_up(AllRegs* r) { ed_bottom_up(r); }
void amos_typed_ed_button_close(AllRegs* r) { ed_button_close(r); }
void amos_typed_ed_button_hide(AllRegs* r) { ed_button_hide(r); }
void amos_typed_ed_button_size(AllRegs* r) { ed_button_size(r); }
void amos_typed_ed_close_window(AllRegs* r) { ed_close_window(r); }
void amos_typed_ed_close_window_quit(AllRegs* r) { ed_close_window_quit(r); }
void amos_typed_ed_delete_all_zones(AllRegs* r) { ed_delete_all_zones(r); }
void amos_typed_ed_delete_window(AllRegs* r) { ed_delete_window(r); }
void amos_typed_ed_delete_windows(AllRegs* r) { ed_delete_windows(r); }
void amos_typed_ed_drag_bottom(AllRegs* r) { ed_drag_bottom(r); }
void amos_typed_ed_drag_top(AllRegs* r) { ed_drag_top(r); }
void amos_typed_ed_draw_windows(AllRegs* r) { ed_draw_windows(r); }
void amos_typed_ed_edit_hidden(AllRegs* r) { ed_edit_hidden(r); }
void amos_typed_ed_empty_next(AllRegs* r) { ed_empty_next(r); }
void amos_typed_ed_erase_windows(AllRegs* r) { ed_erase_windows(r); }
void amos_typed_ed_flip_size(AllRegs* r) { ed_flip_size(r); }
void amos_typed_ed_follow_scroll(AllRegs* r) { ed_follow_scroll(r); }
void amos_typed_ed_get_hidden(AllRegs* r) { ed_get_hidden(r); }
void amos_typed_ed_hide_window(AllRegs* r) { ed_hide_window(r); }
void amos_typed_ed_link_cursor(AllRegs* r) { ed_link_cursor(r); }
void amos_typed_ed_max_size(AllRegs* r) { ed_max_size(r); }
void amos_typed_ed_new_all_hidden(AllRegs* r) { ed_new_all_hidden(r); }
void amos_typed_ed_new_hidden(AllRegs* r) { ed_new_hidden(r); }
void amos_typed_ed_next_window(AllRegs* r) { ed_next_window(r); }
void amos_typed_ed_open_shown_window(AllRegs* r) { ed_open_shown_window(r); }
uint32_t amos_typed_ed_open_window(AllRegs* r) { return uint32_t(ed_open_window(r)); }
void amos_typed_ed_previous_window(AllRegs* r) { ed_previous_window(r); }
void amos_typed_ed_shrink_all(AllRegs* r) { ed_shrink_all(r); }
void amos_typed_ed_split(AllRegs* r) { ed_split(r); }
void amos_typed_ed_top_down(AllRegs* r) { ed_top_down(r); }
void amos_typed_ed_top_up(AllRegs* r) { ed_top_up(r); }
void amos_typed_ed_unlink_scroll(AllRegs* r) { ed_unlink_scroll(r); }
void amos_typed_ed_update_split(AllRegs* r) { ed_update_split(r); }
uint32_t amos_typed_ed_window_number(AllRegs* r) { return uint32_t(ed_window_number(r)); }
}
}
