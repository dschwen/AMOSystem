// The window of another host: SDL2, loaded when AMOS starts (dlopen: the
// build needs no SDL of its own). Each frame (host/display.cpp) is shown;
// the keyboard and the mouse become the input events of the Amiga (raw
// keys, relative mouse moves, the buttons in the qualifiers) for the input
// handler AMOS.library installs (input.device's IND_ADDHANDLER); the sound
// of the audio's voices (host/audio.cpp) is queued to SDL's. Without a
// display (AMOS_DISPLAY=none, or no SDL) AMOS runs without a window and
// without sound.

#include "runtime/runtime.h"
#include "runtime/glue.h"

#include <devices/inputevent.h>
#include <exec/interrupts.h>

#include <dlfcn.h>
#include <stdio.h>
#include <string.h>

// (stdlib.h's struct timeval clashes with devices/timer.h's)
extern "C" char *getenv(const char *name) noexcept;
extern "C" [[noreturn]] void exit(int status) noexcept;

using namespace amos;

namespace amos::host {

constexpr int FrameWidth = 768, FrameHeight = 288;
void display_frame(uint32_t *out);
void mouse_buttons(bool left, bool right);

namespace {

// ---- SDL, by its functions' addresses ----------------------------------------

struct Sdl {
    int (*Init)(uint32_t);
    void *(*CreateWindow)(const char *, int, int, int, int, uint32_t);
    void *(*CreateRenderer)(void *, int, uint32_t);
    void *(*CreateTexture)(void *, uint32_t, int, int, int);
    int (*UpdateTexture)(void *, const void *, const void *, int);
    int (*RenderClear)(void *);
    int (*RenderCopy)(void *, void *, const void *, const void *);
    void (*RenderPresent)(void *);
    int (*PollEvent)(void *);
    int (*SetRelativeMouseMode)(int);
    const char *(*GetError)();
    int (*ShowCursor)(int);
    int (*InitSubSystem)(uint32_t);
    uint32_t (*OpenAudioDevice)(const char *, int, const void *, void *, int);
    void (*PauseAudioDevice)(uint32_t, int);
    int (*QueueAudio)(uint32_t, const void *, uint32_t);
    uint32_t (*GetQueuedAudioSize)(uint32_t);
    void (*ClearQueuedAudio)(uint32_t);
} sdl;

uint32_t audio_device;

void *window, *renderer, *texture;
bool shown;
uint32_t frame[FrameWidth * FrameHeight];

template <typename F> bool symbol(void *lib, F &f, const char *name)
{
    f = reinterpret_cast<F>(dlsym(lib, name));
    return f != nullptr;
}

bool load()
{
    void *lib = dlopen("libSDL2-2.0.so.0", RTLD_NOW);
    if (!lib)
        return false;
    return symbol(lib, sdl.Init, "SDL_Init") && symbol(lib, sdl.CreateWindow, "SDL_CreateWindow") &&
           symbol(lib, sdl.CreateRenderer, "SDL_CreateRenderer") &&
           symbol(lib, sdl.CreateTexture, "SDL_CreateTexture") &&
           symbol(lib, sdl.UpdateTexture, "SDL_UpdateTexture") && symbol(lib, sdl.RenderClear, "SDL_RenderClear") &&
           symbol(lib, sdl.RenderCopy, "SDL_RenderCopy") && symbol(lib, sdl.RenderPresent, "SDL_RenderPresent") &&
           symbol(lib, sdl.PollEvent, "SDL_PollEvent") &&
           symbol(lib, sdl.SetRelativeMouseMode, "SDL_SetRelativeMouseMode") &&
           symbol(lib, sdl.GetError, "SDL_GetError") && symbol(lib, sdl.ShowCursor, "SDL_ShowCursor") &&
           symbol(lib, sdl.InitSubSystem, "SDL_InitSubSystem") &&
           symbol(lib, sdl.OpenAudioDevice, "SDL_OpenAudioDevice") &&
           symbol(lib, sdl.PauseAudioDevice, "SDL_PauseAudioDevice") &&
           symbol(lib, sdl.QueueAudio, "SDL_QueueAudio") &&
           symbol(lib, sdl.GetQueuedAudioSize, "SDL_GetQueuedAudioSize") &&
           symbol(lib, sdl.ClearQueuedAudio, "SDL_ClearQueuedAudio");
}

// The sound: 44.1kHz, 16 bits, stereo, queued
constexpr int Rate = 44100, FrameBytes = Rate / 50 * 4;
void audio_start()
{
    if (sdl.InitSubSystem(0x10) < 0)        // SDL_INIT_AUDIO
        return;
    struct {
        int freq;
        uint16_t format;
        uint8_t channels, silence;
        uint16_t samples, padding;
        uint32_t size;
        void *callback, *userdata;
    } want = {Rate, 0x8010, 2, 0, 1024, 0, 0, nullptr, nullptr};    // AUDIO_S16LSB
    audio_device = sdl.OpenAudioDevice(nullptr, 0, &want, nullptr, 0);
    if (!audio_device)
        return;
    static const uint8_t silence[FrameBytes * 2] = {};
    sdl.QueueAudio(audio_device, silence, sizeof silence);
    sdl.PauseAudioDevice(audio_device, 0);
}

// ---- the input --------------------------------------------------------------

Interrupt *handler;             // AMOS.library's
uint16_t qualifier;             // the shifts and buttons down

void send(uint8_t event_class, uint16_t code, int16_t x = 0, int16_t y = 0)
{
    if (!handler)
        return;
    static InputEvent e;
    memset(&e, 0, sizeof e);
    e.ie_Class = event_class;
    e.ie_Code = code;
    e.ie_Qualifier = qualifier;
    if (event_class == IECLASS_RAWMOUSE) {
        e.ie_Qualifier |= IEQUALIFIER_RELATIVEMOUSE;
        e.ie_position.ie_xy.ie_x = x;
        e.ie_position.ie_xy.ie_y = y;
    }
    AllRegs r = {};
    r.a[0] = address(&e);
    r.a[1] = address(handler->is_Data);
    glue_call(reinterpret_cast<const void *>(handler->is_Code), r);
}

// SDL's scancodes (USB) to the Amiga's raw keys (0xFF: none)
uint8_t raw_key(int scancode)
{
    static const char letters[] = "\x20\x35\x33\x22\x12\x23\x24\x25\x17\x26\x27\x28\x37\x36\x18\x19\x10\x13\x21\x14\x16\x34\x11\x32\x15\x31";
    if (scancode >= 4 && scancode <= 29)
        return uint8_t(letters[scancode - 4]);                      // a-z
    if (scancode >= 30 && scancode <= 39)
        return uint8_t(0x01 + scancode - 30);                       // 1-9, 0
    if (scancode >= 58 && scancode <= 67)
        return uint8_t(0x50 + scancode - 58);                       // F1-F10
    if (scancode >= 89 && scancode <= 97) {                         // keypad 1-9
        static const uint8_t pad[] = {0x1D, 0x1E, 0x1F, 0x2D, 0x2E, 0x2F, 0x3D, 0x3E, 0x3F};
        return pad[scancode - 89];
    }
    switch (scancode) {
    case 40: return 0x44;       // return
    case 41: return 0x45;       // escape
    case 42: return 0x41;       // backspace
    case 43: return 0x42;       // tab
    case 44: return 0x40;       // space
    case 45: return 0x0B;       // -
    case 46: return 0x0C;       // =
    case 47: return 0x1A;       // [
    case 48: return 0x1B;       // ]
    case 49: return 0x0D;       // backslash
    case 51: return 0x29;       // ;
    case 52: return 0x2A;       // '
    case 53: return 0x00;       // `
    case 54: return 0x38;       // ,
    case 55: return 0x39;       // .
    case 56: return 0x3A;       // /
    case 57: return 0x62;       // caps lock
    case 68: return 0x5F;       // F11: help
    case 76: return 0x46;       // delete
    case 79: return 0x4E;       // right
    case 80: return 0x4F;       // left
    case 81: return 0x4D;       // down
    case 82: return 0x4C;       // up
    case 84: return 0x5C;       // keypad /
    case 85: return 0x5D;       // keypad *
    case 86: return 0x4A;       // keypad -
    case 87: return 0x5E;       // keypad +
    case 88: return 0x43;       // enter
    case 98: return 0x0F;       // keypad 0
    case 99: return 0x3C;       // keypad .
    case 100: return 0x30;      // < > (international)
    case 224: return 0x63;      // control
    case 225: return 0x60;      // left shift
    case 226: return 0x64;      // left alt
    case 227: return 0x66;      // left Amiga
    case 228: return 0x63;      // control
    case 229: return 0x61;      // right shift
    case 230: return 0x65;      // right alt
    case 231: return 0x67;      // right Amiga
    }
    return 0xFF;
}

uint16_t qualifier_of(uint8_t raw)
{
    switch (raw) {
    case 0x60: return IEQUALIFIER_LSHIFT;
    case 0x61: return IEQUALIFIER_RSHIFT;
    case 0x63: return IEQUALIFIER_CONTROL;
    case 0x64: return IEQUALIFIER_LALT;
    case 0x65: return IEQUALIFIER_RALT;
    case 0x66: return IEQUALIFIER_LCOMMAND;
    case 0x67: return IEQUALIFIER_RCOMMAND;
    }
    return 0;
}

void events()
{
    uint8_t e[64];
    while (sdl.PollEvent(e)) {
        uint32_t type;
        memcpy(&type, e, 4);
        if (type == 0x100) {                // SDL_QUIT
            exit(0);
        } else if (type == 0x300 || type == 0x301) {    // SDL_KEYDOWN, SDL_KEYUP
            if (type == 0x300 && e[13])     // (repeats: the Amiga's own)
                continue;
            int32_t scancode;
            memcpy(&scancode, e + 16, 4);
            uint8_t raw = raw_key(scancode);
            if (raw == 0xFF)
                continue;
            bool down = type == 0x300;
            if (raw == 0x62) {              // caps lock: toggled
                if (down)
                    qualifier ^= IEQUALIFIER_CAPSLOCK;
            } else if (uint16_t q = qualifier_of(raw)) {
                qualifier = down ? uint16_t(qualifier | q) : uint16_t(qualifier & ~q);
            }
            send(IECLASS_RAWKEY, down ? raw : uint16_t(raw | 0x80));
        } else if (type == 0x400) {         // SDL_MOUSEMOTION
            int32_t xrel, yrel;
            memcpy(&xrel, e + 28, 4);
            memcpy(&yrel, e + 32, 4);
            // (SDL's first motion has the pointer's position for its move)
            static bool first = true;
            if (first) {
                first = false;
                continue;
            }
            send(IECLASS_RAWMOUSE, IECODE_NOBUTTON, int16_t(xrel), int16_t(yrel));
        } else if (type == 0x401 || type == 0x402) {    // buttons
            bool down = type == 0x401;
            uint16_t code, q;
            switch (e[16]) {
            case 1: code = IECODE_LBUTTON, q = IEQUALIFIER_LEFTBUTTON; break;
            case 3: code = IECODE_RBUTTON, q = IEQUALIFIER_RBUTTON; break;
            case 2: code = IECODE_MBUTTON, q = IEQUALIFIER_MIDBUTTON; break;
            default: continue;
            }
            qualifier = down ? uint16_t(qualifier | q) : uint16_t(qualifier & ~q);
            mouse_buttons(qualifier & IEQUALIFIER_LEFTBUTTON, qualifier & IEQUALIFIER_RBUTTON);
            send(IECLASS_RAWMOUSE, down ? code : uint16_t(code | IECODE_UP_PREFIX));
        }
    }
}

} // namespace

// The window opened, if there is a display
void window_start()
{
    const char *want = getenv("AMOS_DISPLAY");
    if ((want && !strcmp(want, "none")) || !load())
        return;
    audio_start();
    if (!getenv("DISPLAY") || sdl.Init(0x20) < 0)   // SDL_INIT_VIDEO
        return;
    // magnified (AMOS_SCALE, 2 by default): a hires pixel to scale by scale,
    // a line to twice that (the frame's lines are PAL's 288)
    int scale = 2;
    if (const char *s = getenv("AMOS_SCALE"))
        scale = s[0] >= '1' && s[0] <= '9' ? s[0] - '0' : 2;
    window = sdl.CreateWindow("AMOS Professional", 0x2FFF0000, 0x2FFF0000, FrameWidth * scale, FrameHeight * 2 * scale,
                              0x20);
    if (!window)
        return;
    renderer = sdl.CreateRenderer(window, -1, 0);
    texture = renderer ? sdl.CreateTexture(renderer, 0x16362004, 1, FrameWidth, FrameHeight) : nullptr;
    if (!texture)
        return;
    sdl.SetRelativeMouseMode(1);
    shown = true;
}

// A frame: shown, the events of the window to AMOS
void window_frame()
{
    if (!shown)
        return;
    display_frame(frame);
    sdl.UpdateTexture(texture, nullptr, frame, FrameWidth * 4);
    sdl.RenderClear(renderer);
    sdl.RenderCopy(renderer, texture, nullptr, nullptr);
    sdl.RenderPresent(renderer);
    events();
}

void input_handler(Interrupt *is) { handler = is; }

bool window_shown() { return shown; }

// A frame's sound (count stereo samples), unless the sound is behind
// Speech (host/speech.cpp): stereo samples at 44.1kHz queued at once, whether the sound
// is still playing, the sound stopped
void audio_play(const int16_t *samples, int count)
{
    if (audio_device)
        sdl.QueueAudio(audio_device, samples, uint32_t(count) * 4);
}
bool audio_busy() { return audio_device && sdl.GetQueuedAudioSize(audio_device) > FrameBytes * 2; }
void audio_stop()
{
    if (audio_device)
        sdl.ClearQueuedAudio(audio_device);
}

void audio_queue(const int16_t *samples, int count)
{
    if (audio_device && sdl.GetQueuedAudioSize(audio_device) < FrameBytes * 4)
        sdl.QueueAudio(audio_device, samples, uint32_t(count) * 4);
}

} // namespace amos::host
