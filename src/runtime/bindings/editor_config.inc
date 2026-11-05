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
void amos_typed_ed_config_free(AllRegs* r) { ed_config_free(r); }
void amos_typed_ed_config_load(AllRegs* r) { ed_config_load(r); }
void amos_typed_ed_config_load_as(AllRegs* r) { ed_config_load_as(r); }
void amos_typed_ed_config_load_default(AllRegs* r) { ed_config_load_default(r); }
void amos_typed_ed_config_load_default_quiet(AllRegs* r) { ed_config_load_default_quiet(r); }
void amos_typed_ed_config_redraw(AllRegs* r) { ed_config_redraw(r); }
void amos_typed_ed_config_save(AllRegs* r) { ed_config_save(r); }
void amos_typed_ed_config_save_as(AllRegs* r) { ed_config_save_as(r); }
void amos_typed_ed_config_save_changed(AllRegs* r) { ed_config_save_changed(r); }
void amos_typed_ed_config_save_default(AllRegs* r) { ed_config_save_default(r); }
void amos_typed_ed_config_save_default_quiet(AllRegs* r) { ed_config_save_default_quiet(r); }
void amos_typed_ed_config_set_palette(AllRegs* r) { ed_config_set_palette(r); }
void amos_typed_ed_resource_free(AllRegs* r) { ed_resource_free(r); }
void amos_typed_ed_resource_load(AllRegs* r) { ed_resource_load(r); }
void amos_typed_ed_set_banks_fn(AllRegs* r) { ed_set_banks_fn(r); }
void amos_typed_ed_sound_play(AllRegs* r) { ed_sound_play(r); }
void amos_typed_ed_sounds_changed(AllRegs* r) { ed_sounds_changed(r); }
void amos_typed_ed_sounds_end(AllRegs* r) { ed_sounds_end(r); }
void amos_typed_ed_text_change(AllRegs* r) { ed_text_change(r); }
void amos_typed_ed_text_count(AllRegs* r) { ed_text_count(r); }
void amos_typed_ed_text_free(AllRegs* r) { ed_text_free(r); }
void amos_typed_ed_wait_input(AllRegs* r) { ed_wait_input(r); }
}
}
