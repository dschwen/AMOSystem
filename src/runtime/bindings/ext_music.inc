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
uint64_t amos_typed_ext_bell0(const uint32_t* words) { return typed_library_call<&ext_bell0, 0, 0>(words); }
uint64_t amos_typed_ext_bell1(const uint32_t* words) { return typed_library_call<&ext_bell1, 1, 0>(words); }
uint64_t amos_typed_ext_boom(const uint32_t* words) { return typed_library_call<&ext_boom, 0, 0>(words); }
uint64_t amos_typed_ext_del_wave(const uint32_t* words) { return typed_library_call<&ext_del_wave, 1, 0>(words); }
uint64_t amos_typed_ext_fn_mouth_height(const uint32_t* words) { return typed_library_call<&ext_fn_mouth_height, 0, 4>(words); }
uint64_t amos_typed_ext_fn_mouth_width(const uint32_t* words) { return typed_library_call<&ext_fn_mouth_width, 0, 4>(words); }
uint64_t amos_typed_ext_fn_mubase(const uint32_t* words) { return typed_library_call<&ext_fn_mubase, 0, 4>(words); }
uint64_t amos_typed_ext_fn_sam_swapped(const uint32_t* words) { return typed_library_call<&ext_fn_sam_swapped, 1, 4>(words); }
uint64_t amos_typed_ext_fn_vumeter(const uint32_t* words) { return typed_library_call<&ext_fn_vumeter, 1, 4>(words); }
uint64_t amos_typed_ext_led_off(const uint32_t* words) { return typed_library_call<&ext_led_off, 0, 0>(words); }
uint64_t amos_typed_ext_led_on(const uint32_t* words) { return typed_library_call<&ext_led_on, 0, 0>(words); }
uint64_t amos_typed_ext_med_cont(const uint32_t* words) { return typed_library_call<&ext_med_cont, 0, 0>(words); }
uint64_t amos_typed_ext_med_load(const uint32_t* words) { return typed_library_call<&ext_med_load, 2, 0>(words); }
uint64_t amos_typed_ext_med_midi_on(const uint32_t* words) { return typed_library_call<&ext_med_midi_on, 0, 0>(words); }
uint64_t amos_typed_ext_med_play0(const uint32_t* words) { return typed_library_call<&ext_med_play0, 0, 0>(words); }
uint64_t amos_typed_ext_med_play1(const uint32_t* words) { return typed_library_call<&ext_med_play1, 1, 0>(words); }
uint64_t amos_typed_ext_med_play2(const uint32_t* words) { return typed_library_call<&ext_med_play2, 2, 0>(words); }
uint64_t amos_typed_ext_med_stop(const uint32_t* words) { return typed_library_call<&ext_med_stop, 0, 0>(words); }
uint64_t amos_typed_ext_mouth_read(const uint32_t* words) { return typed_library_call<&ext_mouth_read, 0, 0>(words); }
uint64_t amos_typed_ext_music(const uint32_t* words) { return typed_library_call<&ext_music, 1, 0>(words); }
void amos_typed_ext_music_bank_check() { ext_music_bank_check(); }
uint32_t amos_typed_ext_music_cold(RegFrame& r) { return detail::typed_reg_call<&ext_music_cold>(r); }
void amos_typed_ext_music_default() { ext_music_default(); }
void amos_typed_ext_music_end() { ext_music_end(); }
uint32_t amos_typed_ext_music_go_sam_reg(RegFrame& r) { return detail::typed_reg_call<&ext_music_go_sam_reg>(r); }
void amos_typed_ext_music_init() { ext_music_init(); }
uint64_t amos_typed_ext_music_off(const uint32_t* words) { return typed_library_call<&ext_music_off, 0, 0>(words); }
uint64_t amos_typed_ext_music_stop(const uint32_t* words) { return typed_library_call<&ext_music_stop, 0, 0>(words); }
uint64_t amos_typed_ext_mvolume(const uint32_t* words) { return typed_library_call<&ext_mvolume, 1, 0>(words); }
uint64_t amos_typed_ext_noise_to(const uint32_t* words) { return typed_library_call<&ext_noise_to, 1, 0>(words); }
uint64_t amos_typed_ext_play2(const uint32_t* words) { return typed_library_call<&ext_play2, 2, 0>(words); }
uint64_t amos_typed_ext_play3(const uint32_t* words) { return typed_library_call<&ext_play3, 3, 0>(words); }
uint64_t amos_typed_ext_play_off0(const uint32_t* words) { return typed_library_call<&ext_play_off0, 0, 0>(words); }
uint64_t amos_typed_ext_play_off1(const uint32_t* words) { return typed_library_call<&ext_play_off1, 1, 0>(words); }
uint64_t amos_typed_ext_sam_bank(const uint32_t* words) { return typed_library_call<&ext_sam_bank, 1, 0>(words); }
uint64_t amos_typed_ext_sam_loop_off0(const uint32_t* words) { return typed_library_call<&ext_sam_loop_off0, 0, 0>(words); }
uint64_t amos_typed_ext_sam_loop_off1(const uint32_t* words) { return typed_library_call<&ext_sam_loop_off1, 1, 0>(words); }
uint64_t amos_typed_ext_sam_loop_on0(const uint32_t* words) { return typed_library_call<&ext_sam_loop_on0, 0, 0>(words); }
uint64_t amos_typed_ext_sam_loop_on1(const uint32_t* words) { return typed_library_call<&ext_sam_loop_on1, 1, 0>(words); }
uint64_t amos_typed_ext_sam_play1(const uint32_t* words) { return typed_library_call<&ext_sam_play1, 1, 0>(words); }
uint64_t amos_typed_ext_sam_play2(const uint32_t* words) { return typed_library_call<&ext_sam_play2, 2, 0>(words); }
uint64_t amos_typed_ext_sam_play3(const uint32_t* words) { return typed_library_call<&ext_sam_play3, 3, 0>(words); }
uint64_t amos_typed_ext_sam_raw(const uint32_t* words) { return typed_library_call<&ext_sam_raw, 4, 0>(words); }
uint64_t amos_typed_ext_sam_stop0(const uint32_t* words) { return typed_library_call<&ext_sam_stop0, 0, 0>(words); }
uint64_t amos_typed_ext_sam_stop1(const uint32_t* words) { return typed_library_call<&ext_sam_stop1, 1, 0>(words); }
uint64_t amos_typed_ext_sam_swap(const uint32_t* words) { return typed_library_call<&ext_sam_swap, 3, 0>(words); }
uint64_t amos_typed_ext_sample_to(const uint32_t* words) { return typed_library_call<&ext_sample_to, 2, 0>(words); }
uint64_t amos_typed_ext_say1(const uint32_t* words) { return typed_library_call<&ext_say1, 1, 0>(words); }
uint64_t amos_typed_ext_say2(const uint32_t* words) { return typed_library_call<&ext_say2, 2, 0>(words); }
uint64_t amos_typed_ext_set_envel(const uint32_t* words) { return typed_library_call<&ext_set_envel, 4, 0>(words); }
uint64_t amos_typed_ext_set_talk(const uint32_t* words) { return typed_library_call<&ext_set_talk, 4, 0>(words); }
uint64_t amos_typed_ext_set_wave(const uint32_t* words) { return typed_library_call<&ext_set_wave, 2, 0>(words); }
uint64_t amos_typed_ext_shoot(const uint32_t* words) { return typed_library_call<&ext_shoot, 0, 0>(words); }
uint64_t amos_typed_ext_sload(const uint32_t* words) { return typed_library_call<&ext_sload, 3, 0>(words); }
uint64_t amos_typed_ext_ssave(const uint32_t* words) { return typed_library_call<&ext_ssave, 3, 0>(words); }
uint64_t amos_typed_ext_talk_misc(const uint32_t* words) { return typed_library_call<&ext_talk_misc, 2, 0>(words); }
uint64_t amos_typed_ext_talk_stop(const uint32_t* words) { return typed_library_call<&ext_talk_stop, 0, 0>(words); }
uint64_t amos_typed_ext_tempo(const uint32_t* words) { return typed_library_call<&ext_tempo, 1, 0>(words); }
uint64_t amos_typed_ext_track_load(const uint32_t* words) { return typed_library_call<&ext_track_load, 2, 0>(words); }
uint64_t amos_typed_ext_track_loop_off(const uint32_t* words) { return typed_library_call<&ext_track_loop_off, 0, 0>(words); }
uint64_t amos_typed_ext_track_loop_on(const uint32_t* words) { return typed_library_call<&ext_track_loop_on, 0, 0>(words); }
uint64_t amos_typed_ext_track_play0(const uint32_t* words) { return typed_library_call<&ext_track_play0, 0, 0>(words); }
uint64_t amos_typed_ext_track_play1(const uint32_t* words) { return typed_library_call<&ext_track_play1, 1, 0>(words); }
uint64_t amos_typed_ext_track_play2(const uint32_t* words) { return typed_library_call<&ext_track_play2, 2, 0>(words); }
uint64_t amos_typed_ext_track_stop(const uint32_t* words) { return typed_library_call<&ext_track_stop, 0, 0>(words); }
uint64_t amos_typed_ext_voice(const uint32_t* words) { return typed_library_call<&ext_voice, 1, 0>(words); }
uint64_t amos_typed_ext_volume1(const uint32_t* words) { return typed_library_call<&ext_volume1, 1, 0>(words); }
uint64_t amos_typed_ext_volume2(const uint32_t* words) { return typed_library_call<&ext_volume2, 2, 0>(words); }
uint64_t amos_typed_ext_wave(const uint32_t* words) { return typed_library_call<&ext_wave, 2, 0>(words); }
}
}
