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
uint64_t amos_typed_ext_fn_parallel_base(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_base, 0, 4>(words); }
uint64_t amos_typed_ext_fn_parallel_check(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_check, 0, 4>(words); }
uint64_t amos_typed_ext_fn_parallel_error(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_error, 0, 4>(words); }
uint64_t amos_typed_ext_fn_parallel_input1(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_input1, 1, 4>(words); }
uint64_t amos_typed_ext_fn_parallel_input2(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_input2, 2, 4>(words); }
uint64_t amos_typed_ext_fn_parallel_status(const uint32_t* words) { return typed_library_call<&ext_fn_parallel_status, 0, 4>(words); }
uint64_t amos_typed_ext_fn_printer_base(const uint32_t* words) { return typed_library_call<&ext_fn_printer_base, 0, 4>(words); }
uint64_t amos_typed_ext_fn_printer_check(const uint32_t* words) { return typed_library_call<&ext_fn_printer_check, 0, 4>(words); }
uint64_t amos_typed_ext_fn_printer_error(const uint32_t* words) { return typed_library_call<&ext_fn_printer_error, 0, 4>(words); }
uint64_t amos_typed_ext_fn_printer_online(const uint32_t* words) { return typed_library_call<&ext_fn_printer_online, 0, 4>(words); }
uint64_t amos_typed_ext_fn_serial_base(const uint32_t* words) { return typed_library_call<&ext_fn_serial_base, 1, 4>(words); }
uint64_t amos_typed_ext_fn_serial_check(const uint32_t* words) { return typed_library_call<&ext_fn_serial_check, 1, 4>(words); }
uint64_t amos_typed_ext_fn_serial_error(const uint32_t* words) { return typed_library_call<&ext_fn_serial_error, 1, 4>(words); }
uint64_t amos_typed_ext_fn_serial_get(const uint32_t* words) { return typed_library_call<&ext_fn_serial_get, 1, 4>(words); }
uint64_t amos_typed_ext_fn_serial_input(const uint32_t* words) { return typed_library_call<&ext_fn_serial_input, 1, 4>(words); }
uint64_t amos_typed_ext_fn_serial_status(const uint32_t* words) { return typed_library_call<&ext_fn_serial_status, 1, 4>(words); }
uint32_t amos_typed_ext_ioports_cold(RegFrame& r) { return detail::typed_reg_call<&ext_ioports_cold>(r); }
void amos_typed_ext_ioports_end() { ext_ioports_end(); }
uint64_t amos_typed_ext_parallel_abort(const uint32_t* words) { return typed_library_call<&ext_parallel_abort, 0, 0>(words); }
uint64_t amos_typed_ext_parallel_close(const uint32_t* words) { return typed_library_call<&ext_parallel_close, 0, 0>(words); }
uint64_t amos_typed_ext_parallel_open(const uint32_t* words) { return typed_library_call<&ext_parallel_open, 0, 0>(words); }
uint64_t amos_typed_ext_parallel_out(const uint32_t* words) { return typed_library_call<&ext_parallel_out, 2, 0>(words); }
uint64_t amos_typed_ext_parallel_send(const uint32_t* words) { return typed_library_call<&ext_parallel_send, 1, 0>(words); }
uint64_t amos_typed_ext_printer_abort(const uint32_t* words) { return typed_library_call<&ext_printer_abort, 0, 0>(words); }
uint64_t amos_typed_ext_printer_close(const uint32_t* words) { return typed_library_call<&ext_printer_close, 0, 0>(words); }
uint64_t amos_typed_ext_printer_dump0(const uint32_t* words) { return typed_library_call<&ext_printer_dump0, 0, 0>(words); }
uint64_t amos_typed_ext_printer_dump4(const uint32_t* words) { return typed_library_call<&ext_printer_dump4, 4, 0>(words); }
uint64_t amos_typed_ext_printer_dump7(const uint32_t* words) { return typed_library_call<&ext_printer_dump7, 7, 0>(words); }
uint64_t amos_typed_ext_printer_open(const uint32_t* words) { return typed_library_call<&ext_printer_open, 0, 0>(words); }
uint64_t amos_typed_ext_printer_out(const uint32_t* words) { return typed_library_call<&ext_printer_out, 2, 0>(words); }
uint64_t amos_typed_ext_printer_send(const uint32_t* words) { return typed_library_call<&ext_printer_send, 1, 0>(words); }
uint64_t amos_typed_ext_serial_abort(const uint32_t* words) { return typed_library_call<&ext_serial_abort, 1, 0>(words); }
uint64_t amos_typed_ext_serial_bits(const uint32_t* words) { return typed_library_call<&ext_serial_bits, 3, 0>(words); }
uint64_t amos_typed_ext_serial_buf(const uint32_t* words) { return typed_library_call<&ext_serial_buf, 2, 0>(words); }
uint64_t amos_typed_ext_serial_close0(const uint32_t* words) { return typed_library_call<&ext_serial_close0, 0, 0>(words); }
uint64_t amos_typed_ext_serial_close1(const uint32_t* words) { return typed_library_call<&ext_serial_close1, 1, 0>(words); }
uint64_t amos_typed_ext_serial_fast(const uint32_t* words) { return typed_library_call<&ext_serial_fast, 1, 0>(words); }
uint64_t amos_typed_ext_serial_open2(const uint32_t* words) { return typed_library_call<&ext_serial_open2, 2, 0>(words); }
uint64_t amos_typed_ext_serial_open5(const uint32_t* words) { return typed_library_call<&ext_serial_open5, 5, 0>(words); }
uint64_t amos_typed_ext_serial_out(const uint32_t* words) { return typed_library_call<&ext_serial_out, 3, 0>(words); }
uint64_t amos_typed_ext_serial_parity(const uint32_t* words) { return typed_library_call<&ext_serial_parity, 2, 0>(words); }
uint64_t amos_typed_ext_serial_send(const uint32_t* words) { return typed_library_call<&ext_serial_send, 2, 0>(words); }
uint64_t amos_typed_ext_serial_slow(const uint32_t* words) { return typed_library_call<&ext_serial_slow, 1, 0>(words); }
uint64_t amos_typed_ext_serial_speed(const uint32_t* words) { return typed_library_call<&ext_serial_speed, 2, 0>(words); }
uint64_t amos_typed_ext_serial_x(const uint32_t* words) { return typed_library_call<&ext_serial_x, 2, 0>(words); }
}
}
