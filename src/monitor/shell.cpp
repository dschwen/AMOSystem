// The monitor's command loop (+Monitor.s Tt_Tests, Tt_ReLoop, Tt_Loop):
// the buttons of the view screen, the zones of the text screen and its
// dialogue zones; the keys that step the program, initialise it and quit;
// and the hook the detokeniser calls to split a line into its parts. The
// parts that reset the stack (stepping back into the program, the return
// to the editor or the program) stay in +Monitor.s.

#include "runtime/static_symbols.h"
#include "monitor/monitor.h"

using namespace amos;
using namespace amos::monitor;

extern "C" {
// +Monitor.s: the routines of the buttons 1-16 of the view screen, and
// of the zones of the text screen (1-3) and of its dialogue (1-4)


// A routine of the monitor called with its registers (a3 the program, a4
// the monitor's data); true if it answered "not equal"
int32_t mon_dispatch(host::RoutineArg routine, uint32_t d0, uint32_t d1);
// The way back into the program, one instruction or more (Step_Out,
// Step_Out2: refreshing the screens or not); the end of QUIT (the data
// freed, to the program or the editor)
[[noreturn]] void mon_step_out();
[[noreturn]] void mon_step_out2();
[[noreturn]] void mon_quit_back(uint32_t data);
// Entries of the monitor's routines

}

namespace {

constexpr uint16_t BitControl = 8;

void wait_vbl()
{
    boot::wait_vbl_multi();
}

// GetZone: the screen in the low word, the zone in the high one
uint32_t zone()
{
    Regs z;
    sy_call(sy::GetZone, z);
    return z.d[1];
}

void routine(host::RoutineArg entry)
{
    Regs r;
    call(entry, r);
}

// A message as an alert, the information window shown again
void alert_message(uint16_t n)
{
    alert(message(n));
    activate_info();
    info_show_all();
}

// The dialogue zones of the text screen (Tt_DBL)
void dialogue_zones()
{
    Regs t;
    t.d[0] = 1;
    lib_call(L_Dia_AutoTest2, t);
    Regs g;
    g.d[0] = ChannelN;
    lib_call(L_Dia_GetReturn, g);
    uint16_t n = uint16_t(g.d[1]);
    if (!n || n > 4)
        return;
    activate_text();
    mon_dispatch(host::decode_routine(peek_l(mon_text_zones + 4 * (3 + n - 1))), g.d[0], (g.d[1] & 0xFFFF0000u) | uint16_t(n << 2));
}

} // namespace

extern "C" {

// Tt_Tests (tests: the active button forgotten first) and Tt_ReLoop
[[noreturn]] void mon_loop(bool tests)
{
    MonData &m = mon();
    if (tests)
        m.Tt_Active = 0;
    for (;;) {
        // Tt_ReLoop: a button held down is released first
        banks_monitor();
        activate(Ecran1);
        m.Tt_Accel = 0;
        if (m.Tt_Active) {
            for (;;) {
                wait_vbl();
                if (!(mouse_keys() & 1))
                    break;
                uint32_t z = zone();
                if (uint16_t(z) != Ecran1 || uint16_t(z >> 16) != m.Tt_Active)
                    break;
            }
            uint16_t active = m.Tt_Active;
            m.Tt_Active = 0;
            button_set(active, false);
        }
        // Tt_Loop
        for (;;) {
            wait_vbl();
            uint32_t keys = mouse_keys();
            m.Tt_MKey = uint16_t(keys);
            if (!(keys & 3))
                break;
            activate_info();
            alert_delete();
            activate(Ecran1);
            uint32_t z = zone();
            uint16_t n = uint16_t(z >> 16);
            if (uint16_t(z) == Ecran2) {
                // the text screen's zones
                if (n && n <= 7) {
                    activate_text();
                    mon_dispatch(host::decode_routine(peek_l(mon_text_zones + 4 * (n - 1))), 0, uint32_t(n - 1) << 2);
                }
                break;
            }
            if (uint16_t(z) != Ecran1 || !n) {
                dialogue_zones();
                break;
            }
            // a button: those below HELP stay down while pressed
            uint32_t d1 = (z << 16) | n;
            if (n < 14 && n != m.Tt_Active) {
                if (m.Tt_Active)
                    break;
                m.Tt_Active = n;
                button_set(n, true);
                d1 = 1;
            }
            if (n > 16 || mon_dispatch(host::decode_routine(peek_l(mon_keys + 4 * (n - 1))), uint32_t(n) << 2, d1))
                break;
        }
    }
}

void mon_tests(AllRegs *) { mon_loop(true); }
void mon_reloop(AllRegs *) { mon_loop(false); }

// The step keys: one instruction, slow, fast, very fast (the screens
// hidden)
void step_key(AllRegs *r, uint16_t mode)
{
    MonData &m = mon();
    wait_release();
    if (int16_t(m.Tt_Init) <= 0) {
        routine(AMOS_ROUTINE(mon_no_init_entry));
        r->d[0] = 1;
        return;
    }
    if (!mode)
        mon_step_out();
    m.Step_Mode = mode;
    if (mode < 3)
        mon_step_out();
    screens_show(false);
    mon_step_out2();
}

void mon_step0(AllRegs *r) { step_key(r, 0); }
void mon_step1(AllRegs *r) { step_key(r, 1); }
void mon_step2(AllRegs *r) { step_key(r, 2); }
void mon_step3(AllRegs *r) { step_key(r, 3); }

// INIT: the program run from its start (stopped first if running: the
// error 1003 stops it, and comes back here)
void mon_reinit(AllRegs *r);
void mon_initialise(AllRegs *r)
{
    if (int16_t(mon().Tt_Init) > 0) {
        if (idata().ActuMask & (1 << BitControl))
            error(1003);
        alert_message(16);              // no initialise in this mode
        r->d[0] = 1;
        return;
    }
    mon_reinit(r);
}

void mon_reinit(AllRegs *r)
{
    MonData &m = mon();
    m.Step_Mess = 0;
    m.Tt_Init = 0;
    info_delete(1);
    info_delete(3);
    alert_message(12);                  // testing the program
    info_slider();
    banks_program();
    AllRegs p = {};
    p.d[0] = 0;
    p.a[1] = address(mon_error);
    p.a[2] = address(mon_testing);
    call_a6(lib_entry(L_Prg_RunIt), p, m.Tt_Program);
    // back: out of memory
    alert_message(17);
    r->d[0] = 1;
}

// QUIT: not while the program runs with Break Off
void mon_quit(AllRegs *r)
{
    MonData &m = mon();
    if (int16_t(m.Tt_Init) > 0 && !(idata().ActuMask & (1 << BitControl))) {
        alert_message(7);
        r->d[0] = 1;
        return;
    }
    routine(AMOS_ROUTINE(mon_free_entry));
    screens_refresh();
    mon_quit_back(address(&m));
}

// Tt_Decoupe: called by the detokeniser at each token (a6) with a4 where
// its text goes; notes the token's kind, its column and its offset in the
// line
void mon_split_token(AllRegs *r)
{
    const uint8_t *token = ptr<uint8_t>(r->a[6]);
    uint16_t t = peek_w(token);
    char kind;
    const uint8_t *definition = nullptr;
    if (!t)
        kind = 0;
    else if (t == uint16_t(tk::M))
        kind = '-';
    else if (t == tk::DP || t == tk::Then || t == tk::Else)
        kind = ':';
    else if (t == tk::Var)
        kind = 'V';
    else if (t == tk::Pro)
        kind = 'X';
    else if (t == tk::LGo)
        kind = 'G';
    else if (t == tk::Ch1 || t == tk::Ch2)
        kind = '2';
    else if (t == tk::Fl)
        kind = '1';
    else if (t == tk::Par1)
        kind = '(';
    else if (t == tk::Par2)
        kind = ')';
    else if (t == tk::Vir || t == tk::PVir || t == tk::To || t == tk::As)
        kind = ',';
    else if (t < tk::Ext)
        kind = '0';
    else if (t == tk::Ext) {
        // an extension's: its own table
        uint32_t table = idata().AdTokens[int8_t(token[2])];
        kind = 'I';
        if (table)
            definition = ptr<uint8_t>(table + 4 + int16_t(peek_w(token + 4)));
    } else if (int16_t(t) < 0) {
        // an operator: its definition is in the interpreter's table, not
        // before the instructions' (the original read whatever was there)
        kind = 'O';
    } else {
        definition = ptr<uint8_t>(idata().AdTokens[0] + 4 + int16_t(t));
    }
    if (definition) {
        // the first character after the name: its kind ('V': the next)
        while (int8_t(*definition++) >= 0)
            ;
        kind = char(*definition++);
        if (int8_t(kind) < 0)
            kind = 'I';
        else if (kind == 'V')
            kind = char(*definition);
    }
    MonData &m = mon();
    uint8_t *to = ptr<uint8_t>(m.Tt_PLine);
    if ((kind == 'V' || kind == '0' || kind == '1' || kind == '2') && to[-4] == ':')
        kind = 'W';
    to[0] = uint8_t(kind);
    to[1] = uint8_t(r->a[4] - m.Tt_BufT - 2);
    poke_w(to + 2, uint16_t(r->a[6] - m.Tt_ALine));
    m.Tt_PLine = address(to + 4);
}

// _mon_dispatch
int32_t mon_dispatch(host::RoutineArg routine, uint32_t d0, uint32_t d1)
{
    const Ctx &c = ctx();
    AllRegs r = {};
    r.d[0] = d0;
    r.d[1] = d1;
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    r.a[3] = mon().Tt_Program;
    r.a[4] = idata().Mon_Base;
    r.a[5] = encode_data_zone(c.a5);
    r.a[6] = address(c.a6);
    amos_call_all(&r, routine);
    return r.d[0] != 0;
}

// Tt_Rien: the STOP key, tested apart
void mon_nothing(AllRegs *r) { r->d[0] = 1; }

} // extern "C"

#if !defined(__mc68000__)
#include "runtime/bindings/monitor_shell.inc"
#endif
