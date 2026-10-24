// Programs that run programs (+ILib.s): RUN, PRUN, ASK EDITOR, CALL
// EDITOR, =PRG UNDER, =PRG STATE. A program PRUN runs, and the editor CALL
// EDITOR drives, run nested on the stack: the frames that save the
// interpreter's registers around them are in run_glue.s.

#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

extern "C" {
[[noreturn]] void interp_jump(const Regs *r, const void *fn);
// run_glue.s: Prg_RunIt for PRUN, nested (back with the error that
// stopped it, out of memory 36); Ed_ZapIn nested (d0, a0 its answer)
uint32_t interp_prun_run(uint32_t program, const void *run_it);
void interp_zap_in(Regs *r, const void *zap_in);
}

namespace {

constexpr uint32_t EntNul = 0x80000000u;

// RIllDir: "illegal direct mode"
[[noreturn]] void illegal_direct()
{
    Regs r;
    r.d[0] = 17;
    r.d[1] = 19;
    r.d[2] = uint32_t(-1);
    interp_jump(&r, lib_address(L_ErrorExt));
}

// The name of a program (an AMOS string) into Name1
void program_name(uint32_t name)
{
    Regs r;
    r.a[2] = name + 2;
    r.d[2] = peek_w(ptr<void>(name));
    r.a[0] = idata().Name1;
    lib_call(L_ChVerBuf2, r);
}

void lib(int routine)
{
    Regs r;
    lib_call(routine, r);
}

// A routine of the program management (a6 the program)
uint32_t program_call(int routine, uint32_t program, uint32_t d0)
{
    Ctx &c = ctx();
    AllRegs r = {};
    r.d[0] = d0;
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = program;
    amos_call_all(&r, lib_address(routine));
    return r.d[0];
}

// The file of the program in Name1 must be there (D_Open, closed again)
void check_file()
{
    path_it();
    Regs o;
    o.d[2] = 1005;
    lib_call(L_D_Open, o);
    if (!o.d[0])
        disk_error();
    lib(L_D_Close);
}

// ZapReturn: the answer of the editor or of a program, as =PARAM (d0, a
// word) and =PARAM$ (a0, a text, if d0)
void zap_return(uint32_t d0, uint32_t a0)
{
    IData &d = idata();
    d.ParamC = d.ChVide;
    d.ParamE = uint32_t(int32_t(int16_t(d0)));
    if (d.ParamE) {
        Regs r;
        r.a[0] = a0;
        lib_call(L_A0ToChaine, r);
        d.ParamC = r.a[0];
    }
}

// Ed_Par: the function, its parameter, its text (EntNul: none) for the
// editor: the text into Name1 (and its length into Name2)
void editor_parameters(uint32_t text)
{
    IData &d = idata();
    uint8_t *name2 = ptr<uint8_t>(d.Name2);
    poke_w(name2, 0);
    if (text != EntNul) {
        poke_w(name2, peek_w(ptr<void>(text)));
        program_name(text);
    }
}

} // namespace

extern "C" {

// =PRG STATE
int32_t interp_fn_prg_state() { return int16_t(wdata().AMOState); }

// =PRG UNDER: 1 if the editor's program is under this one, -1 if another
// program is (PRUN), 0 if none (or under the monitor)
int32_t interp_fn_prg_under()
{
    IData &d = idata();
    if (d.Mon_Base)
        return 0;
    const Program &p = *ptr<Program>(d.Prg_Runned);
    if (p.Prg_Previous)
        return -1;
    if (!d.Edt_Current || ptr<EditWindow>(d.Edt_Current)->Edt_Prg == d.Prg_Runned)
        return 0;
    return d.Edit_Segment ? 1 : -1;
}

// RUN in direct mode: the editor's program
void interp_in_run0()
{
    if (!idata().Direct)
        error(22);
    lib_jump(L_Ed_RunDirect, 0);
}

// RunII: the program in Name1 in place of this one (also from a compiled
// procedure)
[[noreturn]] void interp_run_ii()
{
    IData &d = idata();
    uint32_t program = d.Prg_Runned;
    program_call(L_Prg_New, program, 0);            // the screens kept
    lib(L_Prg_Pull);
    ptr<Program>(program)->Prg_Reloaded = 1;
    int16_t loaded = int16_t(program_call(L_Prg_Load, program, uint32_t(-1)));
    uint32_t failure = 36;                          // out of memory
    if (loaded == -1) {
        failure = 101;
    } else if (loaded > -2) {
        AllRegs r = {};
        Ctx &c = ctx();
        r.d[0] = uint32_t(-1);                      // half a graphic init
        r.a[1] = d.Prg_JError;
        r.d[6] = c.d6;
        r.d[7] = c.d7;
        r.a[3] = address(c.a3);
        r.a[4] = address(c.a4);
        r.a[5] = address(c.a5);
        r.a[6] = program;
        amos_call_all(&r, lib_address(L_Prg_RunIt));
    }
    Regs e;
    e.d[0] = failure;
    e.a[0] = 0;
    interp_jump(&e, ptr<void>(d.Prg_JError));
}

// RUN "program"
void interp_in_run1(uint32_t name)
{
    IData &d = idata();
    if (d.Direct)
        illegal_direct();
    if (d.Mon_Base || d.Prg_Accessory)
        error(102);
    program_name(name);
    check_file();
    Regs i;
    i.a[0] = address(ctx().a4) - 2;
    lib_call(L_Includes_Adr, i);
    d.VerPos = i.a[0];
    lib(L_Includes_Clear);
    interp_run_ii();
}

// PRUN "program": runs it on top of this one (loaded if it is not there
// already), then comes back
void interp_in_prun(uint32_t name)
{
    IData &d = idata();
    if (d.Direct)
        illegal_direct();
    if (d.Mon_Base)
        error(102);
    program_name(name);
    Regs a;
    lib_call(L_Prg_AccAdr, a);
    uint32_t program = a.a[0];
    bool run = false;
    if (program) {
        run = !program_call(L_Prg_DejaRunned, program, 0);
    }
    uint32_t failure;
    if (!run) {
        check_file();
        program = program_call(L_Prg_NewStructure, 0, 0);
        if (!program)
            error(24);
        int16_t loaded = int16_t(program_call(L_Prg_Load, program, uint32_t(-1)));
        if (loaded) {
            failure = loaded == -1 ? 101 : 36;
            goto failed;
        }
        program_call(L_Prg_SetBanks, d.Prg_Runned, 0);
    }
    {
        uint32_t e = interp_prun_run(program, lib_address(L_Prg_RunIt));
        if (e != 0xFFFFFFFFu) {
            // stopped (PRun_Errors): the error, if it is one
            d.ErrorSave[0] = ctx().d6;
            d.ErrorSave[1] = ctx().d7;
            Regs m;
            lib_call(L_Open_MathLibraries, m);
            uint16_t code = uint16_t(e);
            if (code == 10 || int16_t(code) >= 1000)
                e = 0;
            zap_return(e, d.ChVide);
            return;
        }
    }
    failure = 36;
failed:
    if (!ptr<Program>(program)->Prg_Edited)
        program_call(L_Prg_DelStructure, program, 0);
    program_call(L_Prg_SetBanks, d.Prg_Runned, 0);
    lib(L_Bnk_Change);
    Regs m;
    m.d[0] = failure;
    m.a[0] = d.Ed_RunMessages;
    m.a[0] = address(boot::get_message(ptr<uint8_t>(m.a[0]), m.d[0]));
    zap_return(m.d[0], m.a[0]);
}

// ASK EDITOR function[,parameter][,text$]: an answer from the editor
void interp_in_ask_editor3(uint32_t function, uint32_t parameter, uint32_t text)
{
    IData &d = idata();
    editor_parameters(text);
    if (!d.Edit_Segment)
        error(23);
    Regs r;
    r.d[0] = function;
    r.d[1] = parameter;
    r.a[0] = d.Name1;
    r.a[1] = d.Name2;
    lib_call(L_Ed_ZapFonction, r);
    d.ParamE = r.d[0];
    d.ParamC = d.ChVide;
    if (uint16_t(r.d[2])) {
        Regs c;
        c.a[0] = r.a[0];
        lib_call(L_A0ToChaine, c);
        d.ParamC = c.a[0];
    }
}
void interp_in_ask_editor2(uint32_t function, uint32_t parameter) { interp_in_ask_editor3(function, parameter, EntNul); }
void interp_in_ask_editor1(uint32_t function) { interp_in_ask_editor3(function, EntNul, EntNul); }

// CALL EDITOR function[,parameter][,text$]: the editor does something
void interp_in_call_editor3(uint32_t function, uint32_t parameter, uint32_t text)
{
    IData &d = idata();
    editor_parameters(text);
    if (!d.Edit_Segment)
        error(23);
    Regs r;
    r.d[0] = function;
    r.d[1] = parameter;
    r.a[0] = d.Name1;
    r.a[1] = d.Name2;
    interp_zap_in(&r, lib_address(L_Ed_ZapIn));
    zap_return(r.d[0], r.a[0]);
}
void interp_in_call_editor2(uint32_t function, uint32_t parameter) { interp_in_call_editor3(function, parameter, EntNul); }
void interp_in_call_editor1(uint32_t function) { interp_in_call_editor3(function, EntNul, EntNul); }

} // extern "C"
