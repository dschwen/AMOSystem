// Menu instructions of +ILib.s: Menu$(,,)=, Menu Key, Menu Del, Set Menu,
// and MnDim (the coordinates of an item, or a level alone) that the menu
// instructions of lib/menus.cpp use too.

#include "interp/interp.h"

#include "lib/menus.h"

using namespace amos;
using namespace amos::interp;

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t MnNDim = 8;

uint32_t parameter_int()
{
    pc() += 2;
    return integer().v1;
}

// The coordinates (,,,) at pc pushed on the a3 stack, as MnFind wants
// them; their number
uint16_t coordinates()
{
    Ctx &c = ctx();
    uint16_t n = 0;
    do {
        n++;
        uint32_t v = parameter_int();
        c.a3 -= 4;
        poke_l(c.a3, v);
    } while (peek_token() == tk::Vir);
    return n;
}

// MnOob: an object string of the item at *slot ("" deletes it, an empty
// parameter keeps it)
void object(be_u32 *slot)
{
    Value v = evaluate_after();
    if (v.v1 == EntNul)
        return;
    const uint8_t *s = ptr<uint8_t>(v.v1);
    if (uint32_t old = *slot) {
        *slot = 0;
        Regs r;
        r.a[1] = old;
        r.d[0] = peek_w(ptr<void>(old));
        w::release(r.a[1], r.d[0]);
    }
    if (!peek_w(s))
        return;
    uint32_t o = menu::compile_object(s);
    if (!o)
        error(24);
    if (int32_t(o) < 0)
        fon_call();
    *slot = o;
}

} // namespace

namespace amos::interp {

// MnDim: (coordinates) -> the item and its flags; a level alone -> its
// flag in MnDFlags (no item)
MenuDim menu_dim()
{
    IData &d = idata();
    if (peek_token() != tk::Par1) {
        uint32_t level = integer().v1;
        if (!level || level > MnNDim)
            fon_call();
        return {reinterpret_cast<uint8_t *>(d.MnDFlags) + level - 1, nullptr};
    }
    Ctx &c = ctx();
    uint8_t *a3 = c.a3;
    uint16_t n = coordinates();
    MenuItem *m = menu::find(reinterpret_cast<const be_u32 *>(c.a3), n);
    c.a3 = a3;
    if (!m)
        error(39);
    return {reinterpret_cast<uint8_t *>(&m->MnFlag), m};
}

} // namespace amos::interp

extern "C" {

// Menu$(coordinates)=title[,selected[,inactive[,background]]]
void interp_in_menu()
{
    IData &d = idata();
    Ctx &c = ctx();
    uint8_t *a3 = c.a3;
    Regs v;
    lib_call(L_MnClearVar, v);
    uint16_t n = coordinates();
    MenuItem *m = menu::find(reinterpret_cast<const be_u32 *>(c.a3), n);
    if (!m)
        m = menu::insert(reinterpret_cast<const be_u32 *>(c.a3), n);
    c.a3 = a3;
    if (d.ScOnAd != d.MnAdEc) {
        if (d.MnAdEc)
            screen_error(3);
        d.MnAdEc = d.ScOnAd;
    }
    // the colours of the current window
    // (without a screen the original read the first bytes of the memory: zeros)
    const Screen *sc = ptr<Screen>(d.ScOnAd);
    const Window *w = sc ? ptr<Window>(sc->EcWindow) : nullptr;
    uint8_t paper = w ? uint8_t(w->WiPaper) : 0, pen = w ? uint8_t(w->WiPen) : 0;
    m->MnInkA1 = paper;
    m->MnInkB1 = pen;
    m->MnInkC1 = paper;
    m->MnInkA2 = pen;
    m->MnInkB2 = paper;
    m->MnInkC2 = paper;
    object(&m->MnOb1);
    be_u32 *more[] = {&m->MnOb2, &m->MnOb3, &m->MnObF};
    for (be_u32 *slot : more) {
        if (peek_token() != tk::Vir)
            break;
        object(slot);
    }
    d.MnChange++;
}

// Menu Key(coordinates) [To "c" | To scancode[,shift]]
void interp_in_menu_key()
{
    MenuItem *m = menu_dim().item;
    if (m->MnLat)
        fon_call();
    if (peek_token() != tk::To) {
        m->MnKFlag = 0;
        return;
    }
    pc() += 2;
    Value k = evaluate();
    if (kind(k) == TString) {
        const uint8_t *s = ptr<uint8_t>(k.v1);
        if (!peek_w(s))
            fon_call();
        m->MnKAsc = s[2];
        m->MnKFlag = 1;
        return;
    }
    uint32_t shift = 0;
    if (peek_token() == tk::Vir)
        shift = parameter_int();
    if (shift >= 256 || k.v1 >= 128)
        fon_call();
    m->MnKSh = uint8_t(shift);
    m->MnKSc = uint8_t(k.v1);
    m->MnKFlag = 0xFF;
}

// Menu Del [(coordinates)]
void interp_in_menu_del()
{
    if (peek_token() != tk::Par1) {
        menu::erase_all();
        return;
    }
    MenuItem *m = menu_dim().item;
    idata().MnChange++;
    menu::erase(address(m), 0);
}

// Set Menu (coordinates) To x,y
void interp_in_set_menu()
{
    MenuItem *m = menu_dim().item;
    uint32_t x = parameter_int();
    uint32_t y = parameter_int();
    if (y != EntNul) {
        m->MnY = uint16_t(y);
        menu::flags(m) |= menu::MnFixed;
    }
    if (x != EntNul) {
        m->MnX = uint16_t(x);
        menu::flags(m) |= menu::MnFixed;
    }
}
}
