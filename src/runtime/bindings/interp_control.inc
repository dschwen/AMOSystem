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
void amos_typed_interp_call_proc() { interp_call_proc(); }
uint32_t amos_typed_interp_error_ext_reg(RegFrame& r) { return detail::typed_reg_call<&interp_error_ext_reg>(r); }
uint32_t amos_typed_interp_error_reg(RegFrame& r) { return detail::typed_reg_call<&interp_error_reg>(r); }
void amos_typed_interp_fn_err_trap(interp::EvalOut* out) { interp_fn_err_trap(out); }
void amos_typed_interp_goto2() { interp_goto2(); }
void amos_typed_interp_in_apml() { interp_in_apml(); }
void amos_typed_interp_in_break_off() { interp_in_break_off(); }
void amos_typed_interp_in_break_on() { interp_in_break_on(); }
void amos_typed_interp_in_direct() { interp_in_direct(); }
void amos_typed_interp_in_do() { interp_in_do(); }
void amos_typed_interp_in_edit() { interp_in_edit(); }
void amos_typed_interp_in_else() { interp_in_else(); }
void amos_typed_interp_in_else_if() { interp_in_else_if(); }
void amos_typed_interp_in_end_proc() { interp_in_end_proc(); }
void amos_typed_interp_in_every() { interp_in_every(); }
void amos_typed_interp_in_every_off() { interp_in_every_off(); }
void amos_typed_interp_in_every_on() { interp_in_every_on(); }
void amos_typed_interp_in_exit() { interp_in_exit(); }
void amos_typed_interp_in_exit_if() { interp_in_exit_if(); }
void amos_typed_interp_in_for() { interp_in_for(); }
void amos_typed_interp_in_gosub() { interp_in_gosub(); }
void amos_typed_interp_in_goto() { interp_in_goto(); }
void amos_typed_interp_in_if() { interp_in_if(); }
void amos_typed_interp_in_lab() { interp_in_lab(); }
void amos_typed_interp_in_loop() { interp_in_loop(); }
void amos_typed_interp_in_next() { interp_in_next(); }
void amos_typed_interp_in_on() { interp_in_on(); }
void amos_typed_interp_in_on_break() { interp_in_on_break(); }
void amos_typed_interp_in_on_error() { interp_in_on_error(); }
void amos_typed_interp_in_on_menu() { interp_in_on_menu(); }
void amos_typed_interp_in_pop() { interp_in_pop(); }
void amos_typed_interp_in_pop_proc() { interp_in_pop_proc(); }
void amos_typed_interp_in_proc() { interp_in_proc(); }
void amos_typed_interp_in_procedure() { interp_in_procedure(); }
void amos_typed_interp_in_repeat() { interp_in_repeat(); }
void amos_typed_interp_in_restore() { interp_in_restore(); }
void amos_typed_interp_in_resume() { interp_in_resume(); }
void amos_typed_interp_in_resume_label() { interp_in_resume_label(); }
void amos_typed_interp_in_resume_next() { interp_in_resume_next(); }
void amos_typed_interp_in_return() { interp_in_return(); }
void amos_typed_interp_in_system() { interp_in_system(); }
void amos_typed_interp_in_trap() { interp_in_trap(); }
void amos_typed_interp_in_until() { interp_in_until(); }
void amos_typed_interp_in_wend() { interp_in_wend(); }
void amos_typed_interp_in_while() { interp_in_while(); }
uint32_t amos_typed_interp_menu_procedure_reg(RegFrame& r) { return detail::typed_reg_call<&interp_menu_procedure_reg>(r); }
void amos_typed_interp_omn_eff() { interp_omn_eff(); }
void amos_typed_interp_test_force() { interp_test_force(); }
void amos_typed_interp_test_normal() { interp_test_normal(); }
void amos_typed_interp_test_pasaut() { interp_test_pasaut(); }
}
}
