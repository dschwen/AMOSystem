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
uint64_t amos_typed_amcaf_asc_l(const uint32_t* words) { return typed_library_call<&amcaf_asc_l, 1, 4>(words); }
uint64_t amos_typed_amcaf_asc_w(const uint32_t* words) { return typed_library_call<&amcaf_asc_w, 1, 4>(words); }
uint64_t amos_typed_amcaf_binexp(const uint32_t* words) { return typed_library_call<&amcaf_binexp, 1, 4>(words); }
uint64_t amos_typed_amcaf_binlog(const uint32_t* words) { return typed_library_call<&amcaf_binlog, 1, 4>(words); }
uint64_t amos_typed_amcaf_chr_l_s(const uint32_t* words) { return typed_library_call<&amcaf_chr_l_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_chr_w_s(const uint32_t* words) { return typed_library_call<&amcaf_chr_w_s, 1, 4>(words); }
uint64_t amos_typed_amcaf_extbase(const uint32_t* words) { return typed_library_call<&amcaf_extbase, 1, 4>(words); }
uint64_t amos_typed_amcaf_lsl(const uint32_t* words) { return typed_library_call<&amcaf_lsl, 2, 4>(words); }
uint64_t amos_typed_amcaf_lsr(const uint32_t* words) { return typed_library_call<&amcaf_lsr, 2, 4>(words); }
uint64_t amos_typed_amcaf_nfn(const uint32_t* words) { return typed_library_call<&amcaf_nfn, 0, 4>(words); }
uint64_t amos_typed_amcaf_nop(const uint32_t* words) { return typed_library_call<&amcaf_nop, 0, 0>(words); }
uint64_t amos_typed_amcaf_qarc(const uint32_t* words) { return typed_library_call<&amcaf_qarc, 2, 4>(words); }
uint64_t amos_typed_amcaf_qcos(const uint32_t* words) { return typed_library_call<&amcaf_qcos, 2, 4>(words); }
uint64_t amos_typed_amcaf_qrnd(const uint32_t* words) { return typed_library_call<&amcaf_qrnd, 1, 4>(words); }
uint64_t amos_typed_amcaf_qsin(const uint32_t* words) { return typed_library_call<&amcaf_qsin, 2, 4>(words); }
uint64_t amos_typed_amcaf_qsqr(const uint32_t* words) { return typed_library_call<&amcaf_qsqr, 1, 4>(words); }
uint64_t amos_typed_amcaf_rnp(const uint32_t* words) { return typed_library_call<&amcaf_rnp, 0, 4>(words); }
uint64_t amos_typed_amcaf_sdeek(const uint32_t* words) { return typed_library_call<&amcaf_sdeek, 1, 4>(words); }
uint64_t amos_typed_amcaf_speek(const uint32_t* words) { return typed_library_call<&amcaf_speek, 1, 4>(words); }
uint64_t amos_typed_amcaf_vclip(const uint32_t* words) { return typed_library_call<&amcaf_vclip, 3, 4>(words); }
uint64_t amos_typed_amcaf_vin(const uint32_t* words) { return typed_library_call<&amcaf_vin, 3, 4>(words); }
uint64_t amos_typed_amcaf_vmod_2(const uint32_t* words) { return typed_library_call<&amcaf_vmod_2, 2, 4>(words); }
uint64_t amos_typed_amcaf_vmod_3(const uint32_t* words) { return typed_library_call<&amcaf_vmod_3, 3, 4>(words); }
uint64_t amos_typed_amcaf_wordswap(const uint32_t* words) { return typed_library_call<&amcaf_wordswap, 1, 4>(words); }
}
}
