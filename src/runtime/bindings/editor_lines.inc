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
void amos_typed_ed_backspace(AllRegs* r) { ed_backspace(r); }
void amos_typed_ed_clear_line(AllRegs* r) { ed_clear_line(r); }
uint32_t amos_typed_ed_current_line(AllRegs* r) { return uint32_t(ed_current_line(r)); }
void amos_typed_ed_delete_key(AllRegs* r) { ed_delete_key(r); }
void amos_typed_ed_delete_text(AllRegs* r) { ed_delete_text(r); }
void amos_typed_ed_delete_to_end(AllRegs* r) { ed_delete_to_end(r); }
void amos_typed_ed_delete_to_start(AllRegs* r) { ed_delete_to_start(r); }
void amos_typed_ed_delete_word(AllRegs* r) { ed_delete_word(r); }
void amos_typed_ed_delete_word_left(AllRegs* r) { ed_delete_word_left(r); }
void amos_typed_ed_insert_text(AllRegs* r) { ed_insert_text(r); }
uint32_t amos_typed_ed_letter(AllRegs* r) { return uint32_t(ed_letter(r)); }
void amos_typed_ed_tab(AllRegs* r) { ed_tab(r); }
void amos_typed_ed_type_key(AllRegs* r) { ed_type_key(r); }
void amos_typed_ed_untab(AllRegs* r) { ed_untab(r); }
void amos_typed_ed_word_left(AllRegs* r) { ed_word_left(r); }
void amos_typed_ed_word_right(AllRegs* r) { ed_word_right(r); }
}
}
