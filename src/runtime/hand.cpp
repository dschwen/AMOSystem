// The assembler routines of the Amiga build done in C++ on another host:
// host_<label>(AllRegs &) for the entry <label> (tools/host_glue.py finds
// them here). Each says what its assembler does.

#include "runtime/runtime.h"
#include "runtime/glue.h"

using namespace amos;
using namespace amos::host;

extern "C" uint8_t w_ice_start[];
extern "C" uint32_t amos_w_base;
uint32_t amos_w_base;

namespace amos::host {

// +W.s IceStart: W_Base (the data zone, for the input handler and the
// requesters), the contexts, then w/startup.cpp
void host_IceStart(AllRegs &r)
{
    amos_w_base = r.a[5];
    glue_call(w_ice_start, r);
}

} // namespace amos::host

#include "w/w.h"

extern "C" {
void w_amal_animate_data(uint8_t *data);
int32_t w_collision_get(uint8_t *data, int32_t n);
}

namespace amos::host {

// +W.s FastMm, FastMm2, ChipMm, ChipMm2: d0 bytes; d0 the memory (0: none)
void host_w_fast_mm(AllRegs &r) { r.d[0] = w::fast_mm(r.d[0]); }
void host_w_fast_mm2(AllRegs &r) { r.d[0] = w::fast_mm2(r.d[0]); }
void host_w_chip_mm(AllRegs &r) { r.d[0] = w::chip_mm(r.d[0]); }
void host_w_chip_mm2(AllRegs &r) { r.d[0] = w::chip_mm2(r.d[0]); }

// amal_glue.s: the animator from the VBL, a5 the data zone
void host_w_animator(AllRegs &r) { w_amal_animate_data(ptr<uint8_t>(r.a[5])); }

// collision_glue.s GetCol: d1 the number, d0 the result; a5 the data zone
void host_w_get_col(AllRegs &r) { r.d[0] = uint32_t(w_collision_get(ptr<uint8_t>(r.a[5]), int32_t(r.d[1]))); }

} // namespace amos::host

#include "ext/music.h"

namespace amos::host {

// music_glue.s music_sami_entry: the audio interrupt of a voice's
// sample, a1 its data
void host_music_sami_entry(AllRegs &r) { ext_music_sami(ptr<SampleInt>(r.a[1])); }

} // namespace amos::host

extern "C" [[noreturn]] void main_the_end();
extern "C" [[noreturn]] void main_init_fin();

namespace amos::host {

// +B.s TheEnd, Init_Fin: the general way out; the start's end
void host_TheEnd(AllRegs &) { main_the_end(); }
void host_Init_Fin(AllRegs &) { main_init_fin(); }

} // namespace amos::host

extern "C" uint8_t ver_prg_run_it_entry[];

namespace amos::host {

// +Verif.s Prg_RunIt: the program tested; if it can run, the ChrGet
// (never back), else back with d0 (the registers but d0-d1/a0-a1 kept)
void host_Prg_RunIt(AllRegs &r)
{
    AllRegs kept = r;
    if (glue_call(ver_prg_run_it_entry, r) > 0)
        glue_call(lib_address(L_New_ChrGet), r);
    for (int i = 2; i < 8; i++)
        r.d[i] = kept.d[i];
    for (int i = 2; i < 7; i++)
        r.a[i] = kept.a[i];
}

} // namespace amos::host
