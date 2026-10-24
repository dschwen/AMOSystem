// The graphic set-up before a program runs (+ILib.s DefRun1, DefRun2,
// DefRunAcc, DefRunExtensions): no animations, rainbows, screens, blocks,
// fonts or scrollings, the default screen opened (unless DefFlag is -2:
// the system's), the mouse limits, the sprites. An accessory (DefRunAcc)
// keeps the screens.

#include "interp/interp.h"

using namespace amos;

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr int NDScrolls = 10;

IData &data() { return idata(); }

void sy_do(int f, uint32_t d1 = 0, uint32_t d2 = 0)
{
    Regs r;
    r.d[1] = d1;
    r.d[2] = d2;
    sy_call(f, r);
}

void ec_do(int f, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    ec_call(f, r);
}

// What DefRun1 and DefRunAcc share: the default routines of the
// extensions, the AMAL channels, the priority, the scrollings, the
// interrupts
void clear_animations()
{
    IData &d = data();
    Regs r;
    r.a[1] = address(&d.Sys_DefaultRoutines);
    w::call_routines(ptr<uint32_t>(r.a[1]));
}

void reset_channels()
{
    IData &d = data();
    uint8_t *c = reinterpret_cast<uint8_t *>(d.AnCanaux);
    for (int i = 0; i < 64; i++) {
        c[2 * i] = 0;
        c[2 * i + 1] = uint8_t(i);
    }
    sy_do(sy::SPrio, 0, 0);
    for (int i = 0; i < NDScrolls; i++)
        d.DScrolls[i * 6] = 0x8000;
    d.InterOff = 0;
    sy_do(sy::SetSync, d.InterOff);
    d.ActuMask = 0x7100;
    d.VBLDelai = 0;
    d.VBLOCount = 0;
}

} // namespace

namespace amos::interp {

// DefRunExtensions: the DefRun routine of each extension
void defrun_extensions()
{
    IData &d = data();
    for (int i = 0; i < 26; i++) {
        uint32_t routine = d.ExtAdr[4 * i + 1];
        if (routine) {
            Regs r;
            r.d[0] = uint32_t(25 - i);
            r.a[0] = address(&d.ExtAdr[4 * i]);
            r.a[1] = routine;
            amos_call(&r, ptr<void>(routine));
        }
    }
}

void defrun1()
{
    IData &d = data();
    if (!d.DefFlag)
        return;
    sy_do(sy::AMALClr);
    d.PAmalE = 0;
    ec_do(ec::RainDel, uint32_t(-1));
    clear_animations();
    d.ColBack = d.PI_DefEBa;
    Regs del;
    del.d[1] = 0;
    del.d[2] = 7;
    ec_call(ec::DelAll, del);
    d.ScOn = 0;
    d.ScOnAd = 0;
    d.CurTab = 8;
    d.RasLock = 0;
    Regs f;
    lib_call(L_FreeRas, f);
    ec_do(ec::CBlRaz);
    ec_do(ec::BlRaz);
    ec_do(ec::FFonts);
    reset_channels();
    for (int i = 0; i < 32; i++)
        d.DefPal[i] = d.PI_DefEPa[i];
    if (int16_t(d.DefFlag) == -2)
        return;
    Regs s;
    s.d[1] = 0;
    s.d[2] = uint32_t(int32_t(int16_t(d.PI_DefETx)));
    s.d[3] = uint32_t(int32_t(int16_t(d.PI_DefETy)));
    s.d[4] = uint32_t(int32_t(int16_t(d.PI_DefECo)));
    s.d[5] = d.PI_DefEMo;
    s.d[6] = d.PI_DefECoN;
    s.d[7] = 0;
    s.a[1] = address(d.DefPal);
    ec_call(ec::Cree, s);
    if (s.d[0])
        return;
    d.ScOnAd = s.a[0];
    d.ScOn = 1;
    Regs v;
    v.d[1] = 0;
    v.d[2] = d.PI_DefEWx ? d.PI_DefEWx : EntNul;
    v.d[3] = d.PI_DefEWy ? d.PI_DefEWy : EntNul;
    v.d[4] = EntNul;
    v.d[5] = EntNul;
    ec_call(ec::AView, v);
    if (d.PI_DefECo == 1)
        return;
    // colour 3 flashes
    Regs m;
    m.d[0] = 46;
    m.a[0] = address(boot::sys_message(m.d[0]));
    Regs fl;
    fl.d[1] = 3;
    fl.a[1] = m.a[0];
    ec_call(ec::Flash, fl);
    defrun_extensions();
}

void defrun2()
{
    IData &d = data();
    WData &w = wdata();
    if (!d.DefFlag)
        return;
    d.DefFlag = 0;
    // the mouse inside the default screen
    uint16_t x1 = w.DefWX, y1 = w.DefWY;
    uint16_t x2 = uint16_t(x1 + d.PI_DefETx - 1), y2 = uint16_t(y1 + d.PI_DefETy - 1);
    uint16_t limits[4] = {uint16_t(x1 << 1), uint16_t(y1 << 1), uint16_t(x2 << 1), uint16_t(y2 << 1)};
    for (int i = 0; i < 4; i++)
        d.LimSave[i] = limits[i];
    uint16_t *mouse = &w.MouXMin;
    if (!peek_l(mouse))
        for (int i = 0; i < 4; i++)
            mouse[i] = limits[i];
    // the sprites' buffer: PI_ParaTrap + 16, the lines
    sy_do(sy::SBufHs, peek_l(reinterpret_cast<uint8_t *>(&d.PI_ParaTrap) + 16));
    sy_do(sy::OffHs);
    sy_do(sy::StoreM);
    sy_do(sy::StoreM);
    sy_do(sy::AffHs);
}

void defrun_accessory()
{
    IData &d = data();
    clear_animations();
    sy_do(sy::AMALClr);
    d.PAmalE = 0;
    reset_channels();
    defrun_extensions();
    sy_do(sy::OffHs);
    sy_do(sy::AffHs);
    d.DefFlag = 1;
}

} // namespace amos::interp

using namespace amos::interp;

extern "C" {
uint32_t interp_defrun1(RegFrame &)
{
    defrun1();
    return 0;
}
uint32_t interp_defrun2(RegFrame &)
{
    defrun2();
    return 0;
}
uint32_t interp_defrun_acc(RegFrame &)
{
    defrun_accessory();
    return 0;
}
uint32_t interp_defrun_extensions(RegFrame &)
{
    defrun_extensions();
    return 0;
}
}
