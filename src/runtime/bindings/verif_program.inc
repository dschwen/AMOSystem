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
void amos_typed_ver_prg_acc_adr(AllRegs* r) { ver_prg_acc_adr(r); }
void amos_typed_ver_prg_chg_ttexte(AllRegs* r) { ver_prg_chg_ttexte(r); }
void amos_typed_ver_prg_cpt_lines(AllRegs* r) { ver_prg_cpt_lines(r); }
void amos_typed_ver_prg_data_load(AllRegs* r) { ver_prg_data_load(r); }
void amos_typed_ver_prg_data_new(AllRegs* r) { ver_prg_data_new(r); }
void amos_typed_ver_prg_data_save(AllRegs* r) { ver_prg_data_save(r); }
uint32_t amos_typed_ver_prg_deja_runned(AllRegs* r) { return uint32_t(ver_prg_deja_runned(r)); }
void amos_typed_ver_prg_del_structure(AllRegs* r) { ver_prg_del_structure(r); }
void amos_typed_ver_prg_load(AllRegs* r) { ver_prg_load(r); }
void amos_typed_ver_prg_new(AllRegs* r) { ver_prg_new(r); }
void amos_typed_ver_prg_new_structure(AllRegs* r) { ver_prg_new_structure(r); }
void amos_typed_ver_prg_pull(AllRegs* r) { ver_prg_pull(r); }
void amos_typed_ver_prg_push(AllRegs* r) { ver_prg_push(r); }
void amos_typed_ver_prg_reset_banks(AllRegs* r) { ver_prg_reset_banks(r); }
void amos_typed_ver_prg_run_it(AllRegs* r) { ver_prg_run_it(r); }
void amos_typed_ver_prg_save(AllRegs* r) { ver_prg_save(r); }
void amos_typed_ver_prg_set_banks(AllRegs* r) { ver_prg_set_banks(r); }
void amos_typed_ver_prg_test_it(AllRegs* r) { ver_prg_test_it(r); }
}
}
