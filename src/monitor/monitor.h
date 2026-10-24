// Shared by the C++ parts of the monitor (src/monitor). The monitor's
// assembler keeps its data (MonData, +MEqu.s) in a4 and the program it
// runs in a3; here they are mon() and program().
#pragma once

#include "core/amos.h"

namespace amos::monitor {

inline MonData &mon() { return *ptr<MonData>(idata().Mon_Base); }
inline Program &program() { return *ptr<Program>(mon().Tt_Program); }

// Calls a routine of +Monitor.s with registers r; returns its d0
inline uint32_t call(const void *routine, Regs &r)
{
    amos_call(&r, routine);
    return r.d[0];
}

// The same with a4 the monitor's data and a6 given (+Monitor.s routines
// that read the program at a6)
inline uint32_t call_a6(const void *routine, AllRegs &r, uint32_t a6)
{
    Ctx &c = ctx();
    r.a[3] = address(c.a3);
    r.a[4] = idata().Mon_Base;
    r.a[5] = address(c.a5);
    r.a[6] = a6;
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    amos_call_all(&r, routine);
    return r.d[0];
}

constexpr uint32_t ChannelN = 0xBBCCDDEE;      // Tt_ChannelN, the monitor's dialogue channel
constexpr uint16_t Ecran1 = 9, Ecran2 = 8;      // Tt_Ecran1 (EcEdit), Tt_Ecran2 (EcFonc)

// The window strings (+Monitor.s Ttt_WInit...)
extern const uint8_t ScrollUp[], ScrollDown[], InverseOn[], InverseOff[], UnderOn[], UnderOff[], NextMark[];

// monitor/listing.cpp
void listing_show();
void listing_new();
void listing_centre();
void listing_size(uint16_t y);
bool listing_screen();
void split(uint16_t n);
struct Chosen {
    uint16_t length;
    const uint8_t *text;
    uint16_t y, x;
};
Chosen choose(uint16_t x, uint16_t y, uint16_t row);

// monitor/view.cpp
void view_update();
void screens_show(bool show);
void mouse_free();
void stop_button();
void picture(uint16_t n, int32_t x, int32_t y);     // Tt_UPack
void buttons_draw();                                // ABoutons
void button_set(uint16_t n, bool on);               // ABouton
void mode_set(uint16_t mode);                       // Tt_SHelp / Tt_SBreak / Tt_SEval
void view_erase();                                  // View_Cls
void screens_refresh();                             // TtReCop
void view_reset();                                  // View_ScUpdate

// monitor/help.cpp
void run_help(uint16_t x, uint16_t y, uint16_t row);
void help_free();                                   // Hlp_FreeMap

// monitor/info.cpp
const uint8_t *message(uint16_t n, uint32_t *d0 = nullptr);
void scroll_up(uint16_t lines);
void banks_monitor();
void banks_program();
void activate(uint16_t screen);
void activate_text();
void activate_info();
uint32_t mouse_keys();
void wait_release();
void mouse_screen(int16_t &x, int16_t &y);
void info_begin(uint16_t kind);
void info_print(const uint8_t *text);
void info_end();
uint16_t info_delete(uint16_t kind);
uint8_t *info_insert(uint16_t size);
void info_show_all();
void info_count_lines();
void info_slider();
void info_centre();
void alert(const uint8_t *text);
void alert_delete();
void delete_show(uint16_t kind);

struct Value {
    bool error;
    uint8_t type;           // 0 integer, 1 float, 2 string
    uint32_t value;
    uint32_t after;         // a6 past the expression
};
Value evaluate(uint32_t tokens);
uint8_t *print_value(uint8_t *to, const Value &v, uint16_t room, uint16_t undefined);

} // namespace amos::monitor
