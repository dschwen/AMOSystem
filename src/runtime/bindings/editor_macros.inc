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
void amos_typed_ed_dialogue_fn(AllRegs* r) { ed_dialogue_fn(r); }
void amos_typed_ed_dialogues_close(AllRegs* r) { ed_dialogues_close(r); }
void amos_typed_ed_dialogues_open(AllRegs* r) { ed_dialogues_open(r); }
void amos_typed_ed_do_dialogue(AllRegs* r) { ed_do_dialogue(r); }
void amos_typed_ed_list_delete(AllRegs* r) { ed_list_delete(r); }
void amos_typed_ed_list_delete_all(AllRegs* r) { ed_list_delete_all(r); }
uint32_t amos_typed_ed_list_new(AllRegs* r) { return uint32_t(ed_list_new(r)); }
void amos_typed_ed_macro_erase(AllRegs* r) { ed_macro_erase(r); }
void amos_typed_ed_macro_erase_all(AllRegs* r) { ed_macro_erase_all(r); }
void amos_typed_ed_macro_load(AllRegs* r) { ed_macro_load(r); }
void amos_typed_ed_macro_load_as(AllRegs* r) { ed_macro_load_as(r); }
void amos_typed_ed_macro_load_default(AllRegs* r) { ed_macro_load_default(r); }
void amos_typed_ed_macro_record(AllRegs* r) { ed_macro_record(r); }
void amos_typed_ed_macro_save_as(AllRegs* r) { ed_macro_save_as(r); }
void amos_typed_ed_macro_save_default(AllRegs* r) { ed_macro_save_default(r); }
void amos_typed_ed_macro_stop_fn(AllRegs* r) { ed_macro_stop_fn(r); }
void amos_typed_ed_macros_free(AllRegs* r) { ed_macros_free(r); }
void amos_typed_ed_mem_free(AllRegs* r) { ed_mem_free(r); }
void amos_typed_ed_mem_reserve(AllRegs* r) { ed_mem_reserve(r); }
void amos_typed_ed_string_add(AllRegs* r) { ed_string_add(r); }
void amos_typed_ed_string_new(AllRegs* r) { ed_string_new(r); }
void amos_typed_ed_strings_reset(AllRegs* r) { ed_strings_reset(r); }
}
}
