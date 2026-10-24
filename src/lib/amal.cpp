// The AMAL instructions of AMOSPro.Lib (+Lib.s): AMAL, ANIM, MOVE X/Y and
// their ON/OFF/FREEZE, AMREG, MOVON, CHANAN, CHANMV, AMAL ERR, AMPLAY,
// SYNCHRO, FREEZE. The channels themselves are AMOS.library's (w/amal.cpp).

#include "core/amos.h"
#include "lib/banks.h"

using namespace amos;

namespace {

constexpr uint32_t EntNul = 0x80000000u;
constexpr uint32_t SpEBase = 79 + 25;
constexpr uint32_t TBuffer = 1024;

uint32_t amal_call(int function, Regs &r)
{
    sy_call(function, r);
    return r.d[0];
}

// MvOnOf: channel n (-1: all) of the types of mask on (1), off (-1),
// frozen (0)
void on_off(uint32_t n, uint32_t mask, int32_t mode)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = mask;
    r.d[3] = uint32_t(mode);
    sy_call(sy::AMALMvO, r);
}

// MvA3: channel n of type (0 AMAL, 1 Anim, 2 Move X, 3 Move Y) runs the
// program (a string, or its number in the AMAL bank, 4), on the object of
// the channel, or the address target
void start(uint32_t type, uint32_t n, uint32_t program, uint32_t target)
{
    IData &d = idata();
    d.PAmalE = 0;
    if (n >= (d.InterOff ? 64u : 16u))
        fon_call();
    uint8_t *bank = bank_find(4);
    if (bank && peek_l(bank - 8) != 0x416D616C)      // "Amal"
        bank = nullptr;
    uint32_t text = program;
    if (program < 1024) {
        // the bank's program
        if (!bank)
            lib_jump(L_BkNoRes, 0);
        if (!peek_l(bank))
            fon_call();
        uint8_t *table = bank + peek_l(bank);
        uint16_t number = uint16_t(program);
        text = d.ChVide;
        if (number > peek_w(table))
            fon_call();
        table += 2;
        if (uint16_t offset = peek_w(table + int16_t(uint16_t(number << 1))))
            text = address(table + int16_t(uint16_t(offset << 1)));
    }
    // the string, ended by a 0 for the time
    uint16_t length = peek_w(ptr<void>(text));
    uint8_t *end = ptr<uint8_t>(text + 2 + length);
    uint8_t kept = *end;
    *end = 0;
    Regs r;
    r.a[1] = text + 2;
    r.a[2] = d.Buffer;
    r.d[2] = TBuffer - 256 - 64;
    r.d[1] = d.Buffer + TBuffer - 256;
    r.d[3] = type;
    if (target) {
        r.d[4] = 5;
        r.d[5] = target;
    } else {
        uint16_t object = d.AnCanaux[n];
        r.d[4] = object >> 8;
        r.d[5] = object & 0xFF;
    }
    r.d[6] = n;
    r.d[7] = address(bank);
    int16_t e = int16_t(amal_call(sy::AMALCre, r));
    *end = kept;
    if (!e)
        return;
    if (e > 0) {
        d.PAmalE = uint16_t(r.a[0]);
        error(SpEBase + 2 + uint32_t(e));
    }
    if (e == -1)
        lib_jump(L_OOfMem, 0);
    lib_jump(L_EcWiErr, uint32_t(-e));
}

// The address of a register: AmRR
uint32_t reg_address(uint32_t channel, uint32_t reg)
{
    Regs r;
    if (channel == EntNul) {
        if (reg >= 26)
            fon_call();
        r.d[1] = uint32_t(-1);
    } else {
        if (channel >= 64 || reg >= 10)
            fon_call();
        r.d[1] = channel;
    }
    r.d[2] = 0;
    r.d[3] = reg;
    if (int32_t(amal_call(sy::AMALReg, r)) < 0)
        fon_call();
    return r.a[0];
}

// FnAm1, then MovOn / ChanA / ChanM
int32_t channel_state(int function, int32_t n)
{
    if (n < 0 || n >= 64)
        fon_call();
    Regs r;
    r.d[1] = uint32_t(n);
    sy_call(function, r);
    return int32_t(r.d[1]);
}

} // namespace

extern "C" {

// AMAL n,program / ANIM / MOVE X / MOVE Y; AMAL n,program,address...
void lib_in_amal2(uint32_t n, uint32_t program) { start(0, n, program, 0); }
void lib_in_anim2(uint32_t n, uint32_t program) { start(1, n, program, 0); }
void lib_in_move_x2(uint32_t n, uint32_t program) { start(2, n, program, 0); }
void lib_in_move_y2(uint32_t n, uint32_t program) { start(3, n, program, 0); }
void lib_in_amal3(uint32_t n, uint32_t program, uint32_t at) { start(0, n, program, at & ~1u); }
void lib_in_anim3(uint32_t n, uint32_t program, uint32_t at) { start(1, n, program, at & ~1u); }
void lib_in_move_x3(uint32_t n, uint32_t program, uint32_t at) { start(2, n, program, at & ~1u); }
void lib_in_move_y3(uint32_t n, uint32_t program, uint32_t at) { start(3, n, program, at & ~1u); }

// AMAL/ANIM/MOVE ON/OFF/FREEZE [n]
void lib_in_amal_on0() { on_off(uint32_t(-1), 0b0001, 1); }
void lib_in_amal_off0() { on_off(uint32_t(-1), 0b0001, -1); }
void lib_in_amal_freeze0() { on_off(uint32_t(-1), 0b0001, 0); }
void lib_in_move_on0() { on_off(uint32_t(-1), 0b1100, 1); }
void lib_in_anim_on0() { on_off(uint32_t(-1), 0b0010, 1); }
void lib_in_move_off0() { on_off(uint32_t(-1), 0b1100, -1); }
void lib_in_anim_off0() { on_off(uint32_t(-1), 0b0010, -1); }
void lib_in_move_freeze0() { on_off(uint32_t(-1), 0b1100, 0); }
void lib_in_anim_freeze0() { on_off(uint32_t(-1), 0b0010, 0); }
void lib_in_amal_on1(uint32_t n) { on_off(n, 0b0001, 1); }
void lib_in_amal_off1(uint32_t n) { on_off(n, 0b0001, -1); }
void lib_in_amal_freeze1(uint32_t n) { on_off(n, 0b0001, 0); }
void lib_in_move_on1(uint32_t n) { on_off(n, 0b1100, 1); }
void lib_in_anim_on1(uint32_t n) { on_off(n, 0b0010, 1); }
void lib_in_move_off1(uint32_t n) { on_off(n, 0b1100, -1); }
void lib_in_anim_off1(uint32_t n) { on_off(n, 0b0010, -1); }
void lib_in_move_freeze1(uint32_t n) { on_off(n, 0b1100, 0); }
void lib_in_anim_freeze1(uint32_t n) { on_off(n, 0b0010, 0); }

// =AMAL ERR
uint32_t lib_fn_amalerr() { return idata().PAmalE; }

// FREEZE, UNFREEZE
void lib_in_freeze()
{
    Regs r;
    sy_call(sy::AMALFrz, r);
}
void lib_in_unfreeze()
{
    Regs r;
    sy_call(sy::AMALUFrz, r);
}

// SYNCHRO ON/OFF, SYNCHRO
void lib_in_synchro_on()
{
    Regs r;
    r.d[1] = 0;
    idata().InterOff = 0;
    sy_call(sy::SetSync, r);
}
void lib_in_synchro_off()
{
    Regs r;
    r.d[1] = uint32_t(-1);
    idata().InterOff = 0xFFFF;
    sy_call(sy::SetSync, r);
}
void lib_in_synchro()
{
    Regs r;
    sy_call(sy::Synchro, r);
}

// =MOVON(n), =CHANAN(n), =CHANMV(n)
int32_t lib_fn_movon(int32_t n) { return channel_state(sy::MovOn, n); }
int32_t lib_fn_chanan(int32_t n) { return channel_state(sy::ChanA, n); }
int32_t lib_fn_chanmv(int32_t n) { return channel_state(sy::ChanM, n); }

// AMREG([channel,]register)=value, =AMREG(...)
void lib_in_amreg1(uint32_t reg, uint32_t value) { poke_w(ptr<void>(reg_address(EntNul, reg)), uint16_t(value)); }
void lib_in_amreg2(uint32_t channel, uint32_t reg, uint32_t value)
{
    poke_w(ptr<void>(reg_address(channel, reg)), uint16_t(value));
}
int32_t lib_fn_amreg1(uint32_t reg) { return int16_t(peek_w(ptr<void>(reg_address(EntNul, reg)))); }
int32_t lib_fn_amreg2(uint32_t channel, uint32_t reg) { return int16_t(peek_w(ptr<void>(reg_address(channel, reg)))); }

// AMPLAY speed,direction[,first TO last]
void lib_in_amplay4(uint32_t speed, uint32_t direction, int32_t first, uint32_t last)
{
    if (last >= 64)
        fon_call();
    if (first < 0 || last < uint32_t(first))
        fon_call();
    Regs r;
    r.d[1] = uint32_t(first);
    r.d[2] = last;
    r.d[3] = speed;
    r.d[4] = direction;
    sy_call(sy::PlaySet, r);
}
void lib_in_amplay2(uint32_t speed, uint32_t direction) { lib_in_amplay4(speed, direction, 0, 63); }
}
