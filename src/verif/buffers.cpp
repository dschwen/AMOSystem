// The interpreter's buffers (+Verif.s): ClearVar, the buffer of variables
// and strings (Set Buffer), the variables' names, the variables of direct
// mode and the stack of the loops and procedures.

#include "verif/verif.h"

using namespace amos;

namespace {

constexpr uint16_t StackProcSize = 42;   // Stack_ProcSize: one level

void lib(int routine)
{
    Regs r;
    lib_call(routine, r);
}

// SyCall MemFastClear: 0 if out of memory
uint32_t reserve(uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[0] = w::reserve(r.d[0], w::FastClear);
    return r.a[0];
}

void release(uint32_t address, uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[1] = address;
    w::release(r.a[1], r.d[0]);
}

} // namespace

namespace amos::verif {

// Stack_Reserve: the stack of Stack_Size levels (none if 0). Returns the
// levels reserved, 0 if out of memory.
uint16_t stack_reserve()
{
    IData &d = idata();
    if (d.Stack_CSize == d.Stack_Size)
        return d.Stack_CSize;
    if (d.Stack_CSize) {
        release(d.BaLoop, uint32_t(d.Stack_CSize + 1) * StackProcSize);
        d.Stack_CSize = 0;
    }
    if (!d.Stack_Size)
        return 0;
    d.Stack_CSize = d.Stack_Size;
    uint32_t size = uint32_t(d.Stack_Size + 1) * StackProcSize;
    uint32_t m = reserve(size);
    if (!m)
        return 0;
    d.BaLoop = m;
    d.HoLoop = m + size;
    return d.Stack_CSize;
}

// ResDir: the variables of direct mode, at the bottom of the variables
// (PI_TVDirect bytes at most, what is free between the strings and them)
void direct_variables()
{
    IData &d = idata();
    if (d.TVMax || d.TabBas < d.HiChaine)
        return;
    uint32_t room = d.TabBas - d.HiChaine;
    uint32_t n = d.PI_TVDirect;
    if (n > room)
        n = room;
    // six bytes a variable, one of them for the header. The original went
    // on with less than 6 bytes free (a size of -6): it cleared 64K.
    int32_t size = int32_t(n / 6 * 6) - 6;
    if (size <= 0)
        return;
    uint8_t *a0 = ptr<uint8_t>(d.TabBas) - 4;
    poke_l(a0, d.VarLoc);
    a0 -= 2;
    poke_w(a0, 0xFFFF);
    a0 -= size;
    d.VarLoc = address(a0);
    d.TabBas = address(a0);
    d.TVMax = uint16_t(size);
    for (int32_t i = 0; i < size; i += 2)
        poke_w(a0 + i, 0);
}

// ResVarBuf: the buffer of variables and strings, size bytes (0: none):
// the strings from its start, the labels and variables from its end
void resize_variables(uint32_t size)
{
    IData &d = idata();
    if (uint32_t m = d.VarBuf) {
        d.VarBuf = 0;
        release(m, d.VarBufL);
    }
    if (size) {
        uint32_t m = reserve(size);
        if (!m)
            fail(EOutOfMemory);
        d.VarBuf = m;
        uint8_t *top = ptr<uint8_t>(m + size);
        d.LabHaut = address(top);
        top -= 2;
        poke_w(top, 0);
        d.LabBas = address(top);
        top -= 4;
        poke_l(top, 0);
        top -= 2;
        poke_w(top, 0xFFFF);
        d.VarGlo = d.TabBas = d.VarLoc = address(top);
        d.LoChaine = d.ChVide = d.ParamC = m;
        poke_w(ptr<void>(m), 0);
        d.HiChaine = d.LabMini = m + 2;
    }
    d.VarLong = 0;
    d.GloLong = 0;
    d.TVMax = 0;
    d.VarBufL = size;
}

// ResVNom: the buffer of the variables' names, size bytes (0: none): the
// direct mode's names at its end, the program's below
void resize_names(uint32_t size)
{
    IData &d = idata();
    if (uint32_t m = d.VNmMini) {
        d.VNmMini = 0;
        uint32_t length = d.VNmLong;
        d.VNmLong = 0;
        release(m, length);
    }
    if (!size)
        return;
    uint32_t m = reserve(size);
    if (!m)
        fail(EOutOfMemory);
    d.VNmMini = m;
    d.VNmLong = size;
    uint8_t *top = ptr<uint8_t>(m + size);
    d.DVNmHaut = address(top);
    top -= 2;
    poke_w(top, 0);
    d.DVNmBas = d.VNmHaut = address(top);
    top -= 2;
    poke_w(top, 0);
    d.VNmBas = address(top);
}

// ClearBuffers
void clear_buffers()
{
    IData &d = idata();
    resize_variables(0);
    d.VarBufFlg = 0;
    resize_names(0);
    d.Stack_Size = 0;
    stack_reserve();
}

// ClearVar: no more variables, files, menus, dialogs, temporary banks...
void clear_variables()
{
    IData &d = idata();
    WData &w = wdata();
    uint8_t *base = reinterpret_cast<uint8_t *>(&d);
    for (int i = IData::DebRaz; i < IData::FinRaz; i += 2)
        poke_w(base + i, 0);
    d.Test_Flags = 0;
    // the disk: the files not shown by the selector, the directory's width
    d.IffMask = 0xFFFFFFFF;
    Regs m;
    m.d[0] = 47;
    m.a[0] = address(boot::sys_message(m.d[0]));
    const char *from = ptr<char>(m.a[0]);
    char *to = ptr<char>(d.DirFNeg);
    while ((*to++ = *from++))
        ;
    d.DirLNom = d.PI_DirSize;
    w.ClLast = 0;
    d.ChrInp = 0x0A0D;
    // the registers of Call: a5 the data, a4 the keyboard, a3 the program,
    // a0 the rastport, d7-d5 Dos, Gfx and Intuition, d4 BasSp, d3 the
    // editor's configuration
    d.CallReg[8 + 5] = address(&d);
    d.CallReg[8 + 4] = w.ClAsc;
    d.CallReg[8 + 3] = d.Prg_Source;
    d.CallReg[8 + 0] = w.RastPort;
    d.CallReg[7] = d.DosBase;
    d.CallReg[6] = w.GfxBase;
    d.CallReg[5] = w.IntBase;
    d.CallReg[4] = d.BasSp;
    d.CallReg[3] = address(&d.Ed_Config);
    Regs c;
    c.a[1] = address(&d.Sys_ClearRoutines);
    w::call_routines(ptr<uint32_t>(c.a[1]));
    lib(L_Bnk_EffTemp);
    bank_changed();
    lib(L_MenuReset);
    lib(L_Dia_WarmInit);
    clear_buffers();
    d.FixFlg = 0xFFFF;
    d.ExpFlg = 0;
}

} // namespace amos::verif

using namespace amos::verif;

extern "C" {
void ver_clear_variables(AllRegs *) { clear_variables(); }
void ver_clear_buffers(AllRegs *) { clear_buffers(); }
void ver_stack_reserve(AllRegs *r) { r->d[0] = (r->d[0] & 0xFFFF0000u) | stack_reserve(); }
void ver_direct_variables(AllRegs *) { direct_variables(); }
// d1 = the size
void ver_resize_variables(AllRegs *r) { resize_variables(r->d[1]); }
void ver_resize_names(AllRegs *r) { resize_names(r->d[1]); }
}
