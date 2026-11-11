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
void amos_typed_amcaf_bank_check() { amcaf_bank_check(); }
void amos_typed_amcaf_default() { amcaf_default(); }
void amos_typed_amcaf_end() { amcaf_end(); }
uint64_t amos_typed_amcaf_pt_bank(const uint32_t* words) { return typed_library_call<&amcaf_pt_bank, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_cia_speed(const uint32_t* words) { return typed_library_call<&amcaf_pt_cia_speed, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_cinstr(const uint32_t* words) { return typed_library_call<&amcaf_pt_cinstr, 1, 4>(words); }
uint64_t amos_typed_amcaf_pt_cnote(const uint32_t* words) { return typed_library_call<&amcaf_pt_cnote, 1, 4>(words); }
uint64_t amos_typed_amcaf_pt_continue(const uint32_t* words) { return typed_library_call<&amcaf_pt_continue, 0, 0>(words); }
uint64_t amos_typed_amcaf_pt_cpattern(const uint32_t* words) { return typed_library_call<&amcaf_pt_cpattern, 0, 4>(words); }
uint64_t amos_typed_amcaf_pt_cpos(const uint32_t* words) { return typed_library_call<&amcaf_pt_cpos, 0, 4>(words); }
uint64_t amos_typed_amcaf_pt_data_base(const uint32_t* words) { return typed_library_call<&amcaf_pt_data_base, 0, 4>(words); }
uint64_t amos_typed_amcaf_pt_free_voice_0(const uint32_t* words) { return typed_library_call<&amcaf_pt_free_voice_0, 0, 4>(words); }
uint64_t amos_typed_amcaf_pt_free_voice_1(const uint32_t* words) { return typed_library_call<&amcaf_pt_free_voice_1, 1, 4>(words); }
uint64_t amos_typed_amcaf_pt_instr_address(const uint32_t* words) { return typed_library_call<&amcaf_pt_instr_address, 1, 4>(words); }
uint64_t amos_typed_amcaf_pt_instr_length(const uint32_t* words) { return typed_library_call<&amcaf_pt_instr_length, 1, 4>(words); }
uint64_t amos_typed_amcaf_pt_instr_play_1(const uint32_t* words) { return typed_library_call<&amcaf_pt_instr_play_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_instr_play_2(const uint32_t* words) { return typed_library_call<&amcaf_pt_instr_play_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_pt_instr_play_3(const uint32_t* words) { return typed_library_call<&amcaf_pt_instr_play_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_pt_play_1(const uint32_t* words) { return typed_library_call<&amcaf_pt_play_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_play_2(const uint32_t* words) { return typed_library_call<&amcaf_pt_play_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_pt_raw_play(const uint32_t* words) { return typed_library_call<&amcaf_pt_raw_play, 4, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_bank(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_bank, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_freq(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_freq, 2, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_play_1(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_play_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_play_2(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_play_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_play_3(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_play_3, 3, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_stop(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_stop, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_volume_1(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_volume_1, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_sam_volume_2(const uint32_t* words) { return typed_library_call<&amcaf_pt_sam_volume_2, 2, 0>(words); }
uint64_t amos_typed_amcaf_pt_signal(const uint32_t* words) { return typed_library_call<&amcaf_pt_signal, 0, 4>(words); }
uint64_t amos_typed_amcaf_pt_stop(const uint32_t* words) { return typed_library_call<&amcaf_pt_stop, 0, 0>(words); }
uint64_t amos_typed_amcaf_pt_voice(const uint32_t* words) { return typed_library_call<&amcaf_pt_voice, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_volume(const uint32_t* words) { return typed_library_call<&amcaf_pt_volume, 1, 0>(words); }
uint64_t amos_typed_amcaf_pt_vu(const uint32_t* words) { return typed_library_call<&amcaf_pt_vu, 1, 4>(words); }
}
}
