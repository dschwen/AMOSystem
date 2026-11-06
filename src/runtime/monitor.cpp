// The monitor's assembler on another host (+Monitor.s): its entries from
// the editor and the program, the stop on an instruction (MonitorChr) and
// the way back into the program (Step_Out), the program's errors
// (Tt_Error), QUIT, and the evaluation of an expression with its own way
// out of errors. The assembler reset the stack to BasSp (or Tt_EvPile);
// here those are marks (host/runtime.h).

#include "runtime/static_symbols.h"
#include "runtime/runtime.h"
#include "runtime/marks.h"

#include <initializer_list>

using namespace amos;
using namespace amos::host;

// The monitor's routines (its glue)


extern "C" void interp_run_instruction(uint32_t t);

namespace {

constexpr uint16_t TkDP = 0x54, TkEndP = 0x390;

MonData &mon() { return *ptr<MonData>(idata().Mon_Base); }

// The program's banks, the monitor's (Tt_PrgBanks, Tt_TtBanks)
void banks(RoutineArg entry, const AllRegs &r)
{
    AllRegs b = r;
    b.a[4] = idata().Mon_Base;
    glue_call(entry, b);
}

// Tt_Message: message n of the monitor
uint32_t message(uint32_t n, const AllRegs &r)
{
    AllRegs m = r;
    m.d[0] = n;
    glue_call(AMOS_ROUTINE(mon_message_entry), m);
    return m.a[0];
}

// The screen the program had active, active again
void program_screen()
{
    if (uint16_t on = idata().ScOn) {
        Regs a;
        a.d[1] = uint16_t(on - 1);
        ec_vector_call(ec::Active, a);
    }
}

// The instruction run (the step's return address, _interp_step_instruction),
// then the ChrGet loop (Prg_InsRet)
void resume(AllRegs &r)
{
    to_context(r);
    interp_run_instruction(r.d[0]);
    r = context_regs();
    glue_call(idata().Prg_InsRet.load(), r);
    missing("return from Prg_InsRet", nullptr);
}

// The registers the stop kept (Step_Data: the return, a3-a6, d0-d1, d6-d7)
AllRegs kept()
{
    const be_u32 *s = mon().Step_Data;
    AllRegs r = {};
    for (int i = 0; i < 4; i++)
        r.a[3 + i] = s[1 + i];
    r.d[0] = s[5];
    r.d[1] = s[6];
    r.d[6] = s[7];
    r.d[7] = s[8];
    return r;
}

// Step_Out, Step_Out2: back into the program at the instruction it stopped
// on, the screens refreshed or not
[[noreturn]] void step_out(bool refresh, const AllRegs &r)
{
    IData &d = idata();
    if (refresh) {
        wdata().Actualise = uint16_t(wdata().Actualise | 2);
        d.ActuMask = uint16_t(d.ActuMask | 2);
    }
    banks(AMOS_ROUTINE(mon_banks_program_entry), r);
    program_screen();
    mark_jump_regs(d.BasSp.load(), resume, kept());
}

// The monitor stopped on an instruction: its display, its loop
void stopped(AllRegs &r)
{
    if (!glue_call(AMOS_ROUTINE(mon_step_show_entry), r))
        glue_call(AMOS_ROUTINE(mon_reloop_entry), r);
    step_out(true, r);
}

// MonitorChr: before each instruction (d0/d1 the instruction's), the
// instruction run unless the monitor stops on it
void monitor_chr(AllRegs &r)
{
    IData &d = idata();
    auto run = [&] {
        to_context(r);
        interp_run_instruction(r.d[0]);
        r = context_regs();
    };
    uint32_t at = r.a[6] - 2;
    if (peek_w(ptr<void>(at)) == TkDP)
        return run();
    uint32_t d4 = r.d[0], d5 = r.d[1];
    auto go_back = [&] {
        r.d[0] = d4;
        r.d[1] = d5;
        run();
    };
    auto go_out = [&] {
        wdata().Actualise = uint16_t(wdata().Actualise | 2);
        d.ActuMask = uint16_t(d.ActuMask | 2);
        go_back();
    };
    MonData &m = mon();
    int32_t stop = 0;
    // fast: only a break point stops it
    if (m.Step_Mode >= 2) {
        if (m.Step_Mode == 2)
            wdata().Actualise = uint16_t(wdata().Actualise | 2);
        if (!m.Step_Stop && m.Step_Break != at)
            return go_back();
        m.Step_Mode = 0;
        stop = -1;
    }
    bool locate = true;
    if (!m.Tt_Init) {
        locate = false;
    } else {
        bool now = int16_t(m.Tt_Init) < 0;
        if (!now) {
            uint16_t actualise = wdata().Actualise;
            wdata().Actualise = uint16_t(actualise & ~0x100);
            if (actualise & 0x100 || m.Step_Stop) {
                now = true;
            } else if (m.Step_Break == at) {
                m.Step_Mess = message(10, r);
                now = true;
            } else if (peek_w(ptr<void>(at)) == TkEndP) {
                glue_call(AMOS_ROUTINE(mon_tests_patch_entry), r);
                return go_back();
            }
        }
        if (now) {
            m.Step_Break = 0;
            m.Step_Mode = 0;
            m.Step_Stop = 0;
            stop = -1;
        }
    }
    if (locate) {
        // the instruction's line, in a closed procedure or not
        if (!r.d[7])
            return go_back();
        AllRegs f = r;
        f.a[1] = d.Prg_Run;
        f.a[0] = r.d[7];
        glue_call(lib_entry(L_Tk_FindA), f);
        if (!stop && f.d[1])
            return go_out();
        if (!f.d[1]) {
            r.d[1] = at;
            r.d[2] = 0;
        } else {
            r.d[1] = f.d[1] + 2;
            r.d[2] = uint32_t(-1);
        }
        r.a[2] = f.a[0];
        r.d[0] = f.d[0];                // the line's number
    } else {
        r.d[2] = uint32_t(stop);
    }
    // the registers kept, the monitor on the interpreter's stack
    be_u32 *s = m.Step_Data;
    s[0] = 0;
    for (int i = 0; i < 4; i++)
        s[1 + i] = r.a[3 + i];
    s[5] = d4;
    s[6] = d5;
    s[7] = r.d[6];
    s[8] = r.d[7];
    AllRegs t = r;
    t.a[4] = d.Mon_Base;
    t.a[3] = m.Tt_Program;
    mark_jump_regs(d.BasSp.load(), stopped, t);
}

// Tt_GetTheMessage: the message of error d0 (a0 if given); d2 the error
void error_message(AllRegs &r)
{
    IData &d = idata();
    r.d[2] = (r.d[2] & 0xFFFF0000u) | uint16_t(r.d[0]);
    if (r.a[0] || int16_t(r.d[0]) >= 256)
        return;
    AllRegs g = r;
    int16_t n = int16_t(-int16_t(r.d[0]));
    g.a[0] = d.Ed_TstMessages;
    if (n < 0) {
        n = int16_t(-n + 1);
        g.a[0] = d.Ed_RunMessages;
    }
    g.d[0] = (r.d[0] & 0xFFFF0000u) | uint16_t(n);
    glue_call(lib_entry(L_GetMessage), g);
    r.d[0] = g.d[0];
    r.a[0] = g.a[0];
}

// Tt_Error, on the interpreter's stack: the error shown, then the monitor
// stops on the instruction; a test's errors go to the editor's display
void error_stop(AllRegs &r)
{
    IData &d = idata();
    error_message(r);
    MonData &m = mon();
    if (!m.Tt_Init) {
        glue_call(AMOS_ROUTINE(mon_test_error_entry), r);
        r.a[4] = d.Mon_Base;
        glue_call(AMOS_ROUTINE(mon_reloop_entry), r);
    } else {
        uint16_t error = uint16_t(r.d[2]);
        if (!r.a[0])
            r.a[0] = message(15, r);
        r.d[1] = (r.d[1] & 0xFFFF0000u) | uint16_t(r.d[0]);
        m.Step_Mess = r.a[0];
        r.a[6] = r.d[7];
        r.a[3] = d.PLoop;
        uint16_t token = peek_w(ptr<void>(r.a[6] - 2));
        r.d[0] = (r.d[0] & 0xFFFF0000u) | token;
        r.d[1] = (r.d[1] & 0xFFFF0000u) | peek_w(ptr<void>(r.a[4] + int16_t(token)));
        glue_call(AMOS_ROUTINE(mon_show_entry), r);
        m.Step_Mode = 0;
        if (error == 1003) {
            glue_call(AMOS_ROUTINE(mon_reinit_entry), r);
        } else {
            if (error != 9)
                m.Tt_Init = 0xFFFF;
            monitor_chr(r);
        }
    }
    glue_call(d.Prg_InsRet.load(), r);
    missing("return from Prg_InsRet", nullptr);
}

// Tt_Evalue's ways out: an error, or the strings' space to be cleaned
enum { EvOk, EvError, EvAgain } evaluation;

} // namespace

namespace amos::host {

// In_Editor: the monitor on the program the editor runs, its loop
void host_In_Editor(AllRegs &r)
{
    IData &d = idata();
    r.a[3] = ptr<EditWindow>(d.Edt_Runned)->Edt_Prg;
    glue_call(AMOS_ROUTINE(mon_cold_start_entry), r);
    if (r.d[0])
        return;
    r.a[4] = d.Mon_Base;
    mon().Tt_Run = 0;
    glue_call(AMOS_ROUTINE(mon_tests_entry), r);
}

// In_Program: the monitor started by the program (or told to stop it)
void host_In_Program(AllRegs &r)
{
    IData &d = idata();
    if (d.Mon_Base) {
        mon().Step_Stop = uint16_t(mon().Step_Stop + 1);
        r.d[0] = 0;
        return;
    }
    AllRegs s = r;
    s.a[3] = d.Prg_Runned;
    glue_call(AMOS_ROUTINE(mon_cold_start_entry), s);
    r.d[0] = s.d[0];
    if (s.d[0])
        return;
    mon().Tt_Init = 1;
    mon().Tt_Run = 1;
    d.Prg_JError.store(AMOS_ROUTINE(mon_error));
}

void host_MonitorChr(AllRegs &r) { monitor_chr(r); }

void host_mon_error(AllRegs &r) { mark_jump_regs(idata().BasSp.load(), error_stop, r); }

// Tt_Tst1, Tt_Tst3: nothing
void host_mon_test_nothing(AllRegs &) {}
void host_Tt_Tst3(AllRegs &) {}

// Tt_Evalue: the expression at a6 (a4 the monitor's data); d0 0 and its
// value in d2/d3, or -1 if it failed
void host_mon_evaluate(AllRegs &r)
{
    IData &d = idata();
    MonData &m = mon();
    AllRegs entry = r;
    m.Tt_EvDebut = r.a[6];
    d.Patch_Errors.store(AMOS_ROUTINE(mon_ev_error));
    d.Patch_Menage = address(mon_menage);
    banks(AMOS_ROUTINE(mon_banks_program_entry), r);
    AllRegs out;
    for (;;) {
        Mark mark;
        m.Tt_EvPile.store(mark_token(mark));
        evaluation = EvOk;
        out = mark_run(mark, [&] {
            AllRegs e = r;
            e.a[4] = d.AdTokens[0];
            e.d[7] = 0;
            e.a[3] = d.PLoop;
            glue_call(lib_entry(L_New_Evalue), e);
            mark.regs = e;
        });
        if (evaluation != EvAgain)
            break;
        r.a[6] = m.Tt_EvDebut;
        m.Tt_EvDebut = 0;
    }
    uint32_t a6 = out.a[6];
    r = out;
    for (int i : {1, 4, 5, 6, 7})
        r.d[i] = entry.d[i];
    for (int i = 0; i < 6; i++)
        r.a[i] = entry.a[i];
    r.a[6] = a6;
    banks(AMOS_ROUTINE(mon_banks_monitor_entry), r);
    d.Patch_Errors.store({});
    d.Patch_Menage = 0;
    if (evaluation == EvError) {
        r.d[3] = 0;
        r.d[2] = 0;
        r.d[0] = uint32_t(-1);
    } else {
        r.d[0] = 0;
    }
}

// Tt_EvError: an error in the expression
void host_mon_ev_error(AllRegs &r)
{
    evaluation = EvError;
    mark_return(mon().Tt_EvPile.load(), r);
}

// Tt_Menage: the strings' space cleaned, the expression again from its
// start (once)
void host_mon_menage(AllRegs &r)
{
    if (!mon().Tt_EvDebut)
        error(11);
    evaluation = EvAgain;
    mark_return(mon().Tt_EvPile.load(), r);
}

} // namespace amos::host

extern "C" {

[[noreturn]] void mon_step_out() { step_out(true, context_regs()); }
[[noreturn]] void mon_step_out2() { step_out(false, context_regs()); }

// QUIT's end: the monitor's data freed; back into the program if it was
// running, else to the editor (Prg_JError, d0 1000)
[[noreturn]] void mon_quit_back(uint32_t data)
{
    IData &d = idata();
    MonData &m = *ptr<MonData>(data);
    d.Mon_Base = 0;
    if (m.Tt_Run && int16_t(m.Tt_Init) > 0) {
        AllRegs r = {};
        const be_u32 *s = m.Step_Data;
        for (int i = 0; i < 4; i++)
            r.a[3 + i] = s[1 + i];
        r.d[0] = s[5];
        r.d[1] = s[6];
        r.d[6] = s[7];
        r.d[7] = s[8];
        w::release(data, MonData::Tt_DzLong);
        program_screen();
        mark_jump_regs(d.BasSp.load(), resume, r);
    }
    w::release(data, MonData::Tt_DzLong);
    d.Prg_Runned = 0;
    Regs c;
    lib_call(L_Includes_Clear, c);
    Regs v;
    lib_call(L_ClearVar, v);
    AllRegs e = context_regs();
    e.d[0] = 1000;
    e.a[0] = 0;
    glue_call(d.Prg_JError.load(), e);
    missing("return from Prg_JError", nullptr);
}

} // extern "C"

#if !defined(__mc68000__)
#include "runtime/bindings/runtime_monitor.inc"
#endif
