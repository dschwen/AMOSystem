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
void amos_typed_ed_add_user(AllRegs* r) { ed_add_user(r); }
void amos_typed_ed_branch_amos_fn(AllRegs* r) { ed_branch_amos_fn(r); }
void amos_typed_ed_clear_var_fn(AllRegs* r) { ed_clear_var_fn(r); }
void amos_typed_ed_delete_user(AllRegs* r) { ed_delete_user(r); }
void amos_typed_ed_flip_insert(AllRegs* r) { ed_flip_insert(r); }
void amos_typed_ed_flip_keys(AllRegs* r) { ed_flip_keys(r); }
void amos_typed_ed_flip_sounds(AllRegs* r) { ed_flip_sounds(r); }
void amos_typed_ed_insert_button(AllRegs* r) { ed_insert_button(r); }
void amos_typed_ed_key_to_menu(AllRegs* r) { ed_key_to_menu(r); }
void amos_typed_ed_menu_error_fn(AllRegs* r) { ed_menu_error_fn(r); }
void amos_typed_ed_menu_fn(AllRegs* r) { ed_menu_fn(r); }
void amos_typed_ed_menu_init(AllRegs* r) { ed_menu_init(r); }
void amos_typed_ed_menu_marks(AllRegs* r) { ed_menu_marks(r); }
void amos_typed_ed_menus_editor(AllRegs* r) { ed_menus_editor(r); }
void amos_typed_ed_menus_end(AllRegs* r) { ed_menus_end(r); }
void amos_typed_ed_menus_program(AllRegs* r) { ed_menus_program(r); }
void amos_typed_ed_next_hidden(AllRegs* r) { ed_next_hidden(r); }
void amos_typed_ed_previous_hidden(AllRegs* r) { ed_previous_hidden(r); }
void amos_typed_ed_program_to_menu(AllRegs* r) { ed_program_to_menu(r); }
void amos_typed_ed_save_all(AllRegs* r) { ed_save_all(r); }
void amos_typed_ed_set_autosave(AllRegs* r) { ed_set_autosave(r); }
void amos_typed_ed_user_menu(AllRegs* r) { ed_user_menu(r); }
}
}
