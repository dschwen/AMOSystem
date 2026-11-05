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
void amos_typed_ed_block_limits_2(AllRegs* r) { ed_block_limits_2(r); }
void amos_typed_ed_block_save(AllRegs* r) { ed_block_save(r); }
void amos_typed_ed_block_save_ascii(AllRegs* r) { ed_block_save_ascii(r); }
void amos_typed_ed_print_block(AllRegs* r) { ed_print_block(r); }
void amos_typed_ed_print_program(AllRegs* r) { ed_print_program(r); }
void amos_typed_ed_printer_open(AllRegs* r) { ed_printer_open(r); }
void amos_typed_ed_printer_print(AllRegs* r) { ed_printer_print(r); }
}
}
