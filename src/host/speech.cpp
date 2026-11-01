// The narrator of the host: espeak-ng, loaded when AMOS asks to speak (dlopen, as SDL is: the
// build needs none), says the text; its 22kHz samples become 44.1kHz stereo for the window's
// sound. Without espeak-ng, or without sound, the narrator is silent and completes at once.
// What AMOS gives the narrator is the translator's phonemes; the host's translator gives the
// text back, so it is the text that is spoken.

#include "runtime/runtime.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <type_traits>
#include <vector>

namespace amos::host {

void audio_play(const int16_t *samples, int count);
bool audio_busy();
void audio_stop();

namespace {

struct Espeak {
    int (*Initialize)(int output, int buflength, const char *path, int options);
    void (*SetSynthCallback)(int (*cb)(short *, int, void *));
    int (*SetVoiceByName)(const char *name);
    int (*Synth)(const void *text, size_t size, unsigned position, int position_type, unsigned end,
                 unsigned flags, unsigned *id, void *user);
    int (*Synchronize)();
    int (*Cancel)();
    int rate;
} es;

bool tried, ready;
std::vector<int16_t> mono;          // the utterance at espeak's rate
std::vector<int16_t> speech;        // the same as stereo samples, 44.1kHz

// (the callback of espeak: samples at its rate, null at the end)
int collect(short *wav, int count, void *)
{
    if (wav && count > 0)
        mono.insert(mono.end(), wav, wav + count);
    return 0;
}

bool load()
{
    if (tried)
        return ready;
    tried = true;
    void *lib = dlopen("libespeak-ng.so.1", RTLD_NOW);
    if (!lib)
        lib = dlopen("libespeak.so.1", RTLD_NOW);
    if (!lib)
        return false;
    auto get = [&](auto &f, const char *name) {
        f = reinterpret_cast<std::remove_reference_t<decltype(f)>>(dlsym(lib, name));
        return f != nullptr;
    };
    if (!(get(es.Initialize, "espeak_Initialize") && get(es.SetSynthCallback, "espeak_SetSynthCallback") &&
          get(es.SetVoiceByName, "espeak_SetVoiceByName") && get(es.Synth, "espeak_Synth") &&
          get(es.Synchronize, "espeak_Synchronize") && get(es.Cancel, "espeak_Cancel")))
        return false;
    es.rate = es.Initialize(2, 0, nullptr, 1);       // AUDIO_OUTPUT_SYNCHRONOUS, no startup messages
    if (es.rate <= 0)
        return false;
    es.SetSynthCallback(collect);
    es.SetVoiceByName("en");
    return ready = true;
}

} // namespace

// Say the text (up to its end or length): its sound queued; true if there is some
bool speak(const char *text, uint32_t length)
{
    if (!load())
        return false;
    size_t n = strnlen(text, length);
    if (!n)
        return false;
    mono.clear();
    es.Synth(text, n + 1, 0, 1, 0, 1, nullptr, nullptr);     // POS_CHARACTER, espeakCHARS_UTF8
    es.Synchronize();
    if (mono.empty())
        return false;
    // up to 44.1kHz by a line between the samples, to both sides
    size_t count = size_t(double(mono.size()) * 44100.0 / es.rate);
    speech.assign(count * 2, 0);
    for (size_t i = 0; i < count; i++) {
        double at = double(i) * es.rate / 44100.0;
        size_t k = size_t(at);
        double f = at - double(k);
        double a = mono[k], b = k + 1 < mono.size() ? mono[k + 1] : mono[k];
        speech[2 * i] = speech[2 * i + 1] = int16_t(a + (b - a) * f);
    }
    audio_play(speech.data(), int(speech.size() / 2));
    if (const char *path = getenv("AMOS_SPEECH_WAV")) {
        // (debugging: the speech in a file)
        if (FILE *f = fopen(path, "wb")) {
            uint32_t bytes = uint32_t(speech.size() * 2);
            uint8_t h[44] = {'R', 'I', 'F', 'F', 0, 0, 0, 0, 'W', 'A', 'V', 'E', 'f', 'm', 't', ' ', 16, 0, 0, 0, 1, 0, 2, 0,
                             0x44, 0xAC, 0, 0, 0x10, 0xB1, 2, 0, 4, 0, 16, 0, 'd', 'a', 't', 'a'};
            uint32_t riff = 36 + bytes;
            memcpy(h + 4, &riff, 4);
            fwrite(h, 1, 40, f);
            fwrite(&bytes, 4, 1, f);
            fwrite(speech.data(), 1, bytes, f);
            fclose(f);
        }
    }
    return true;
}

bool speaking() { return ready && audio_busy(); }
void speech_stop()
{
    if (ready)
        es.Cancel();
    audio_stop();
}

} // namespace amos::host
