// The copper lists of AMOS.library: the two lists reserved at the start,
// Copper On/Off, and the user copper list of the screen table (EcCall):
// Cop Swap, Cop Reset, Cop Wait, Cop Move, Cop Movel, Cop Logic. They work
// while AMOS's own copper lists are off.

#include "w/w.h"

using namespace amos;
using namespace amos::w;

// w/sprites_glue.s, +W.s


namespace {

// Errors: 1 copper not off, 2 list too long, 3 out of range
enum : uint32_t { CopperOn = 1, TooLong = 2, Range = 3 };

// TCpSw + TCopRes: the logic list is shown, the physic one is written to
uint32_t swap_lists()
{
    WData &d = wdata();
    uint32_t logic = d.CopLogic;
    d.CopLogic = d.CopPhysic;
    d.CopPhysic = logic;
    chips().cop1lc = logic;
    if (d.CopON)
        return CopperOn;
    d.CopPos = d.CopLogic;
    d.Cop255 = 0;
    return 0;
}

// TCopSw: end the list being written and show it
uint32_t swap()
{
    WData &d = wdata();
    if (d.CopON)
        return CopperOn;
    poke_l(ptr<uint8_t>(d.CopPos), 0xFFFFFFFE);
    return swap_lists();
}

// TCopRes: write the list again from its start
uint32_t reset()
{
    WData &d = wdata();
    if (d.CopON)
        return CopperOn;
    d.CopPos = d.CopLogic;
    d.Cop255 = 0;
    return 0;
}

// CopFin: the list goes on at to (also when it is too long)
uint32_t advance(uint8_t *to)
{
    WData &d = wdata();
    d.CopPos = address(to);
    return address(to) - d.CopLogic >= d.CopLong ? uint32_t(TooLong) : 0;
}

// TCopWt: wait for x (d1), y (d2), with the masks d3/d4. The first wait
// below line 255 is preceded by one for the end of line 255.
uint32_t wait(const AllRegs *r)
{
    WData &d = wdata();
    if (d.CopON)
        return CopperOn;
    uint16_t x = r->d[1], y = r->d[2];
    if (x >= 313 || y >= 313)
        return Range;
    uint8_t *to = ptr<uint8_t>(d.CopPos);
    if (y >= 256 && !d.Cop255) {
        poke_w(to, 0xFFE1);
        poke_w(to + 2, 0xFFFE);
        to += 4;
        d.Cop255++;
    }
    poke_w(to, uint16_t(y << 8) | ((x >> 1) & 0xFE) | 1);
    poke_w(to + 2, uint16_t(uint16_t(r->d[4]) << 8) | ((uint16_t(r->d[3]) >> 1) & 0xFE));
    return advance(to + 4);
}

// TCopMv: move value to register *reg (masked as written)
uint32_t move(uint32_t *reg, uint16_t value)
{
    WData &d = wdata();
    if (d.CopON)
        return CopperOn;
    uint8_t *to = ptr<uint8_t>(d.CopPos);
    if (uint16_t(*reg) >= 512)
        return Range;
    *reg = (*reg & 0xFFFF0000) | (*reg & 0x01FE);
    poke_w(to, *reg);
    poke_w(to + 2, value);
    return advance(to + 4);
}

// HsCop: the start of a list: a short wait, then the moves of the sprite
// pointers (the VBL writes their values)
void sprite_moves(uint8_t *list)
{
    poke_l(list, 0x1003FFFE);
    be_u16 *m = ptr<be_u16>(address(list) + 4);
    for (uint16_t reg = 0x120; reg <= 0x13E; reg += 2, m += 2)
        *m = reg;
}

} // namespace

namespace amos::w {

// CpInit: the two lists of length bytes, copper on
bool copper_init(uint32_t length)
{
    WData &d = wdata();
    d.CopLong = length;
    d.CopLogic = chip_mm(length);
    if (!d.CopLogic)
        return false;
    d.CopPhysic = chip_mm(d.CopLong);
    if (!d.CopPhysic)
        return false;
    d.CopON = 0xFFFF;
    sprite_moves(ptr<uint8_t>(d.CopLogic));
    sprite_moves(ptr<uint8_t>(d.CopPhysic));
    return true;
}

// CpEnd
void copper_end()
{
    WData &d = wdata();
    if (d.CopLogic)
        free_mm(d.CopLogic, d.CopLong);
    if (d.CopPhysic)
        free_mm(d.CopPhysic, d.CopLong);
}

} // namespace amos::w

extern "C" {
// TCopOn: Copper Off (d1 = 0): AMOS's lists stop, the user list starts
// empty; Copper On: AMOS's lists back, the sprites and screens remade
void w_ec_copper_on_off(AllRegs *r)
{
    WData &d = wdata();
    r->d[0] = 0;
    if (!uint16_t(r->d[1])) {
        if (!d.CopON)
            return;
        d.CopON = 0;
        call(w_ec_force_cop);
        d.HsChange = 0;
        d.MouShow = 0xFFFF;
        d.CopPos = d.CopLogic;
        wait_vbl();
        r->d[0] = swap();
        return;
    }
    if (d.CopON)
        return;
    wait_vbl();
    sprite_moves(ptr<uint8_t>(d.CopLogic));
    swap_lists();
    wait_vbl();
    sprite_moves(ptr<uint8_t>(d.CopLogic));
    swap_lists();
    wait_vbl();
    d.CopON = 0xFFFF;
    call(w_sprites_show);
    d.MouShow = 0;
    call(w_ec_force_cop);
    wait_vbl();
}

void w_ec_cop_swap(AllRegs *r) { r->d[0] = swap(); }
void w_ec_cop_swap_lists(AllRegs *r) { r->d[0] = swap_lists(); }
void w_ec_cop_reset(AllRegs *r) { r->d[0] = reset(); }
void w_ec_cop_wait(AllRegs *r) { r->d[0] = wait(r); }
void w_ec_cop_move(AllRegs *r) { r->d[0] = move(&r->d[1], r->d[2]); }

// TCopMl: the high word to reg, the low one to reg+2 (the first error is
// not looked at)
void w_ec_cop_movel(AllRegs *r)
{
    move(&r->d[1], r->d[2] >> 16);
    r->d[1] = (r->d[1] & 0xFFFF0000) | uint16_t(r->d[1] + 2);
    r->d[0] = move(&r->d[1], r->d[2]);
}

void w_ec_cop_base(AllRegs *r)
{
    r->d[1] = wdata().CopLogic;
    r->d[0] = 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_copper.inc"
#endif
