// The monitor stopped on an instruction (+Monitor.s MonitorChr, after the
// stack is back at its base): the program's screen and the listing
// brought up to date, the instruction and its parameters (Tt_ImpParam)
// in the information window, then the monitor's loop or the next step.

#include "monitor/monitor.h"

using namespace amos;
using namespace amos::monitor;

namespace {

constexpr uint16_t BitStep = 1 << 1;            // T_Actualise / ActuMask: the tests' call
constexpr uint16_t LBF_20 = 2;                  // LB_Flags: a library with the parameters counted
constexpr int LB_Flags = -12;

// Finie: the end of an instruction
bool finished(uint16_t token) { return !token || token == tk::DP || token == tk::Then || token == tk::Else; }

// Tt_ImpParam: "Param #n = value" for the expression at a6
uint32_t show_parameter(uint32_t a6, uint32_t &n)
{
    info_begin(5);
    info_print(message(3));
    uint8_t *b = ptr<uint8_t>(idata().Buffer);
    Regs r;
    r.d[0] = ++n;
    r.a[0] = address(b);
    lib_call(L_LongToDec, r);
    *ptr<uint8_t>(r.a[0]) = 0;
    info_print(b);
    info_print(reinterpret_cast<const uint8_t *>(" = "));
    Value v = evaluate(a6);
    a6 = v.after;
    uint8_t *to = b;
    if (v.error) {
        for (const uint8_t *s = message(4); *s;)
            *to++ = *s++;
    } else {
        a6 += 2;
        to = print_value(b, v, 60, 14);
    }
    *to = 0;
    info_print(b);
    info_end();
    return a6;
}

// The name of the instruction and its parameters
void show_instruction()
{
    MonData &m = mon();
    IData &d = idata();
    info_begin(5);
    uint8_t *text = ptr<uint8_t>(m.Tt_BufT) + 2;
    if (m.Tt_Locked) {
        // in a closed procedure
        info_print(message(2));
        text[text[-1]] = 0;
        info_print(text);
        info_end();
        return;
    }
    info_print(message(1));
    const uint8_t *part = ptr<uint8_t>(m.Tt_Ligne);
    uint16_t at = uint16_t(m.Tt_Adress - m.Tt_ALine);
    while (peek_w(part + 2) != at) {
        part += 4;
        if (!*part)
            break;
    }
    uint16_t start = part[1];
    do
        part += 4;
    while (*part && *part != ':');
    text[part[1]] = 0;
    info_print(text + start);
    info_end();

    uint32_t a6 = m.Tt_Adress;
    uint16_t token = peek_w(ptr<uint8_t>(a6));
    a6 += 2;
    const uint8_t *tokens = ptr<uint8_t>(d.AdTokens[0]);
    const uint8_t *p = tokens + 4 + token;
    uint32_t n = 0;
    if (token == tk::Rem1 || token == tk::Rem2)
        return;
    if (token == tk::Fade || token == tk::Pal) {
        do
            a6 = show_parameter(a6, n);
        while (!finished(peek_w(ptr<uint8_t>(a6 - 2))));
        return;
    }
    if (token == tk::Ext) {
        int8_t slot = int8_t(*ptr<uint8_t>(a6));
        uint16_t offset = peek_w(ptr<uint8_t>(a6 + 2));
        a6 += 4;
        tokens = ptr<uint8_t>(d.AdTokens[slot]);
        p = tokens + 4 + offset;
    }
    if (tokens[LB_Flags] & (1 << LBF_20) && !peek_w(p - 2))
        return;
    while (int8_t(*p++) >= 0) {
    }
    if (*p++ != 'I')
        return;
    for (;;) {
        if (int8_t(*p++) < 0)
            return;
        a6 = show_parameter(a6, n);
        if (int8_t(*p++) < 0)
            return;
    }
}

// From MonitorChr: d0 the line, d1 the instruction, d2 locked, a2 the
// line's address. 0: back to the monitor's loop, else the next step
uint32_t step_show(uint16_t line, uint32_t instruction, uint16_t locked, uint32_t line_address)
{
    MonData &m = mon();
    banks_monitor();
    m.Tt_YNext = line;
    m.Tt_Adress = instruction;
    m.Tt_Locked = locked;
    m.Tt_DCurLine = line_address;
    // the tests no longer called
    wdata().Actualise &= ~BitStep;
    idata().ActuMask &= ~BitStep;
    m.Step_Stop = 0;
    m.Tt_Ecran = 0xFFFF;
    activate(Ecran1);
    view_update();
    screens_show(true);

    info_delete(0);
    info_delete(5);
    if (m.Tt_Init) {
        split(m.Tt_YNext);
        uint16_t at = uint16_t(m.Tt_Adress - m.Tt_ALine);
        const uint8_t *part = ptr<uint8_t>(m.Tt_Ligne);
        while (at > peek_w(part + 2))
            part += 4;
        m.Tt_XNext = part[1];
        activate_text();
        listing_centre();
        listing_show();
        if (int16_t(m.Tt_Init) >= 0)
            show_instruction();
    }
    if (uint32_t text = m.Step_Mess) {
        m.Step_Mess = 0;
        alert(ptr<uint8_t>(text));
    }
    activate_info();
    info_show_all();
    info_slider();

    activate(Ecran1);
    if (!m.Step_Mode) {
        mouse_free();
        return 0;
    }
    // Running: the STOP button (any button but the view) pressed?
    if (!(mouse_keys() & 1))
        return 1;
    Regs z;
    sy_call(sy::GetZone, z);
    uint16_t zone = uint16_t(z.d[1] >> 16);
    if (uint16_t(z.d[1]) != Ecran1 || !zone || zone == 13)
        return 1;
    mouse_free();
    m.Step_Mode = 0;
    activate(Ecran1);
    stop_button();
    for (int i = 0; i < 11; i++) {
        Regs v;
        sy_call(sy::WaitVbl, v);
    }
    return 0;
}

// Tt_Error, an error found by the tests: the listing on its line, the
// message in the information window
void test_error(const uint8_t *text)
{
    MonData &m = mon();
    IData &d = idata();
    banks_monitor();
    m.Tt_XNext = 0;
    Regs f;
    f.a[0] = d.VerPos;
    f.a[1] = program().Prg_StBas;
    lib_call(L_Tk_FindA, f);
    m.Tt_YNext = uint16_t(f.d[0]);
    m.Tt_Ecran = 0xFFFF;
    activate_text();
    listing_centre();
    listing_show();
    alert(text);
    activate_info();
    info_show_all();
    info_slider();
}

} // namespace

extern "C" {
void mon_test_error(AllRegs *r) { test_error(ptr<uint8_t>(r->a[0])); }

void mon_step_show(AllRegs *r)
{
    r->d[0] = step_show(uint16_t(r->d[0]), r->d[1], uint16_t(r->d[2]), r->a[2]);
}
}
