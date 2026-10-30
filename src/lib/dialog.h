// The Interface: dialog channels and their interpreter (lib/dialog.cpp),
// the line editor of the edit zones and Input (lib/lineedit.cpp), the
// Interface instructions (lib/interface.cpp)
#pragma once

#include "core/amos.h"

namespace amos {

// ---- the line editor ---------------------------------------------------------

// Options, the high byte of LEd_Flags
enum : uint8_t {
    LEdKeys = 1 << 0,       // return keys it does not use
    LEdOnce = 1 << 1,       // return when there is no key
    LEdCursor = 1 << 2,     // the real cursor (else a fake one)
    LEdFilter = 1 << 3,     // only the characters of LEd_Mask
    LEdMouse = 1 << 4,      // return a click outside the line
    LEdTests = 1 << 5,      // Control-C and the monitor (Input)
    LEdMulti = 1 << 6,      // wait for the display between keys
    LEdMouCur = 1 << 7,     // a click in the line moves the cursor
};

bool led_init(LineEdit *e, const uint8_t *text, uint8_t *store, uint8_t flags, uint16_t max, int16_t width);
int16_t led_loop(LineEdit *e, uint32_t *key);
void led_new(LineEdit *e, const uint8_t *text, uint16_t length);
void led_print(LineEdit *e);
void led_cursor_off(LineEdit *e);
void led_cursor_on(LineEdit *e);

// ---- small services ------------------------------------------------------------

uint32_t mouse_key();       // SyCall MouseKey: d1
void dia_wait_mul();        // Dia_WaitMul: wait for the display (WaitTOF)
void dia_patch();           // Dia_Patch: the monitor's screen copy hook
void dia_no_mkey();         // Dia_NoMKey: wait until the left button is up
void dia_clear_key();       // Dia_ClearKey: empty the keyboard buffer

// ---- channels ----------------------------------------------------------------

// Interface errors (EDia_*): AMOS errors 120-131
enum : uint16_t {
    EDiaSyntax = 1, EDiaOMem, EDiaLabAD, EDiaLabND, EDiaChanAD, EDiaChanND, EDiaScreen, EDiaVarND,
    EDiaFCall, EDiaType, EDiaOBuffer, EDiaNPar,
};
constexpr uint32_t IDiaErrors = 120 - 1;

// The variables of a channel follow its structure; the three before them
// are the zone position (BP/ZP), the button return (BR) and the zone being
// drawn or changed
inline be_i32 *dia_vars(Dialog *d) { return reinterpret_cast<be_i32 *>(reinterpret_cast<uint8_t *>(d) + Dialog::Dia_Vars); }

// Dia_GetChannel
Dialog *dia_channel(uint32_t number);

struct DiaOpen {
    uint16_t error;
    uint32_t position;          // in the program, of a syntax error
    Dialog *d;
};
// Dia_OpenChannel: program (a length word, then the text), copied if copy
DiaOpen dia_open(uint32_t number, const uint8_t *program, const uint8_t *puzzle, const uint8_t *messages,
                 uint32_t buffer, uint32_t variables, bool copy);
uint16_t dia_close(uint32_t number);            // Dia_CloseChannel
void dia_close_all();                           // Dia_CloseChannels
uint16_t dia_erase(uint32_t number);            // Dia_EffChannel
void dia_freeze_all(bool freeze);               // Dia_FreezeChannels
uint16_t dia_freeze(uint32_t number, bool freeze);

struct DiaResult {
    uint16_t error;
    int32_t value;              // the answer, or the position of an error
};
// Dia_RunProgram: label -1 for the start; x/y EntNul to keep the base
DiaResult dia_run(uint32_t number, int32_t label, uint32_t x, uint32_t y);
// Dia_RunQuick: open, run and close a channel of its own
DiaResult dia_run_quick(const uint8_t *program, const uint8_t *puzzle, const uint8_t *messages, uint32_t x,
                        uint32_t y, uint32_t v0, uint32_t v1, uint32_t v2, bool copy, uint32_t buffer);
// Dia_Update: zone, then up to three values (EntNul: unchanged)
DiaResult dia_update(uint32_t number, uint32_t zone, uint32_t a, uint32_t b, uint32_t c);
DiaResult dia_return(uint32_t number);          // Dia_GetReturn: -1 if erased

struct DiaValue {
    uint16_t error;
    uint32_t value;
    uint8_t type;               // 0 a number, 2 a string (C string)
};
DiaValue dia_value(uint32_t number, uint32_t zone, uint32_t which);     // Dia_GetValue
// Dia_GetVariable: the address of variable n, or null (*error set)
be_i32 *dia_variable(uint32_t number, uint32_t n, uint16_t *error);
DiaResult dia_zone_at(uint32_t number, uint32_t x, uint32_t y);         // Dia_GetZone
uint8_t *dia_zone_address(uint32_t number, uint32_t zone, uint32_t which); // Dia_GetZoneAd
uint16_t dia_auto_test(uint32_t channels);      // Dia_AutoTest2

// Dia_RScOpen: a screen in the format of a resource. Returns it, or null
// (*error set)
uint8_t *dia_resource_screen(const uint8_t *puzzle, uint32_t number, uint32_t width, uint32_t height,
                             uint32_t flash, int32_t interlace, uint32_t *error);

} // namespace amos
