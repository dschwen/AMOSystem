// The editor's screens (+Edit.s): the editor hidden for a program and
// back (Ed_Hide, Ed_Appear, Ed_Redraw, Ed_LimM), the pictures of the
// resource (Ed_Unpack, Es_Unpack), the memory gauges (Ed_MemoryDraw,
// Ed_MemoryAff).
//
// The flags of EcFlags and EcAW/EcAWT/EcAV are in their high bytes.

#include "editor/editor.h"
#include "core/amiga.h"

#include <stddef.h>
#include "hal/system.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcEdit = 9, EcFonc = 8;
constexpr uint16_t Hide = 1 << (8 + 7), AWChange = 1 << (8 + 2);
constexpr uint16_t BitControl = 1 << 8;
constexpr uint16_t BoutonsSx = 32;              // Ed_BoutonsSx
constexpr uint16_t MemoryPics = 35, MemoryY1 = 3, MemoryY2 = 10, MemorySy = 2;   // Ed_MemoryPics...

Screen *screen(uint16_t n) { return ptr<Screen>(wdata().EcAdr[n]); }

uint32_t sy_fn(int function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    sy_call(function, r);
    return r.d[1];
}

void ec_fn(int function, uint32_t d1 = 0, uint32_t d2 = 0)
{
    Regs r;
    r.d[1] = d1;
    r.d[2] = d2;
    ec_call(function, r);
}

// EdReCop
void recop()
{
    sy_fn(sy::WaitVbl);
    ec_fn(ec::CopForce);
}

// L_AppCentre: the screen opening (or closing) from its centre
void app_centre(Screen *s, int16_t centre, int16_t height, int16_t step)
{
    Regs r;
    r.a[2] = address(s);
    r.d[5] = uint32_t(int32_t(centre));
    r.d[6] = uint32_t(int32_t(height));
    r.d[7] = uint32_t(int32_t(step));
    lib_call(L_AppCentre, r);
}

} // namespace

namespace amos::ed {

// Ed_Unpack / Es_Unpack: picture n of the resource at x, y of the editor's
// screen (or the direct mode's title screen)
void unpack(uint16_t n, int16_t x, int16_t y, bool title)
{
    const uint8_t *resource = ptr<uint8_t>(idata().Ed_Resource);
    const uint8_t *pictures = resource + peek_l(resource + 2);
    Regs r;
    r.a[0] = address(pictures + peek_l(pictures + 4 * n - 2));
    r.a[1] = wdata().EcAdr[title ? EcFonc : EcEdit];
    r.d[1] = uint32_t(int32_t(x));
    r.d[2] = uint32_t(int32_t(y));
    lib_call(L_UnPack_Bitmap, r);
}

// Ed_MemoryDraw: the frames of the gauges at x, y, width wide
void memory_draw(int16_t x, int16_t y, uint16_t width, bool title)
{
    IData &d = idata();
    d.Ed_MemorySx = uint16_t(width - (BoutonsSx + 7));
    unpack(MemoryPics, x, y, title);
    x = int16_t(x + BoutonsSx);
    d.Ed_MemoryX = uint16_t(x);
    // (the word quotient of a divu)
    uint16_t middle = uint16_t(uint16_t(width / BoutonsSx) - 2);
    for (uint16_t i = 0; i < middle; i++) {
        unpack(MemoryPics + 1, x, y, title);
        x = int16_t(x + BoutonsSx);
    }
    unpack(MemoryPics + 2, x, y, title);
}

// Ed_MemoryAff: the chip and fast memory used, in the gauges
void memory_show()
{
    IData &d = idata();
    auto gauge = [&](uint32_t total, uint32_t free, uint16_t y) {
        auto s = ptr<Slider>(d.Buffer);
        uint8_t *b = reinterpret_cast<uint8_t *>(s);
        for (uint32_t i = 0; i < sizeof(Slider); i++)
            b[i] = 0;
        s->Sl_Global = uint16_t(total >> 8);
        s->Sl_Window = uint16_t((total - free) >> 8);
        s->Sl_X = d.Ed_MemoryX;
        s->Sl_Y = y;
        s->Sl_Sx = d.Ed_MemorySx;
        s->Sl_Sy = MemorySy;
        // the colours, inactive then active
        const uint8_t *colours = system_message(23);
        auto *to = reinterpret_cast<be_u16 *>(b + offsetof(Slider, Sl_Inactive));
        for (int i = 0; i < 16; i++)
            to[i] = colours[i];
        Regs r;
        r.a[0] = address(s);
        lib_call(L_Sl_Init, r);
    };
    uint32_t chip = hal::available_memory(hal::MemoryChip | hal::MemoryPublic);
    gauge(d.MemChipTotal, chip, MemoryY1);
    uint32_t fast = hal::available_memory(hal::MemoryPublic | hal::MemoryFast);
    gauge(d.MemFastTotal, fast, MemoryY2);
    d.Ed_MemCurrent = chip + fast;
}

// Ed_LimM: the mouse kept within the editor's screen
void limit_mouse()
{
    IData &d = idata();
    Regs a;
    a.d[1] = uint32_t(int32_t(int16_t(d.Ed_Sx - 4)));
    a.d[2] = uint32_t(int32_t(int16_t(d.Ed_Sy - 1)));
    a.d[3] = EcEdit + 1;
    sy_call(sy::XyHard, a);
    Regs b;
    b.d[1] = 2;
    b.d[2] = 2;
    b.d[3] = EcEdit + 1;
    sy_call(sy::XyHard, b);
    Regs l;
    l.d[1] = b.d[1];
    l.d[2] = b.d[2];
    l.d[3] = uint32_t(int32_t(int16_t(a.d[1])));
    l.d[4] = uint32_t(int32_t(int16_t(a.d[2])));
    sy_call(sy::LimitM, l);
    sy_fn(sy::Show, 0xFFFFFFFF);
}

} // namespace amos::ed

namespace {

// Ed_Hide: the editor's screen away, the program's display back
void editor_hide()
{
    IData &d = idata();
    WData &w = wdata();
    if (!d.EsFlag) {
        d.EsFlag = 0xFFFF;
        all_warnings_end();
        Screen *s = screen(EcEdit);
        app_centre(s, int16_t(d.Ed_Wy + (s->EcTy >> 1)), int16_t(s->EcTy >> 1), int16_t(-d.Ed_VScrol));
        s->EcFlags |= Hide;
        recop();
        ec_fn(ec::Active, EcEdit);
        Regs e;
        call(ed_erase_windows_entry, e);
        ec_fn(ec::SColB, d.ColBack);
        ec_fn(ec::RainHide, 0);
        be_u16 *limits = &w.MouXMin;
        for (int i = 0; i < 4; i++)
            limits[i] = d.LimSave[i];
        sy_fn(sy::RecallM);
        sy_fn(sy::ReActHs);
        sy_fn(sy::AMALUFrz);
        sy_fn(sy::WaitVbl);
        sy_fn(sy::ActHs);
        sy_fn(sy::AffHs);
    }
    sy_fn(sy::ClearKey);
    sy_fn(sy::AMALUFrz);
    Regs m;
    call(ed_menu_program, m);
}

// Ed_Appear: the editor's screen back over the program's
void editor_appear()
{
    IData &d = idata();
    WData &w = wdata();
    if (d.EsFlag) {
        d.EsFlag = 0;
        // no Amiga function keys
        uint8_t *buffer = ptr<uint8_t>(d.Buffer);
        for (uint16_t k = 0; k < 20; k++) {
            buffer[0] = 0;
            Regs f;
            f.d[1] = k;
            f.a[1] = address(buffer);
            sy_call(sy::SetFunk, f);
        }
        sy_fn(sy::AMALFrz);
        sy_fn(sy::StActHs);
        sy_fn(sy::StoreM);
        ec_fn(ec::RainHide, 0xFFFFFFFF);
        ec_fn(ec::SColB, d.Ed_ColB);
        ec_fn(ec::Active, EcEdit);
        Regs t;
        call(ed_draw_top, t);
        Ctx &c = ctx();
        uint8_t *a4 = c.a4, *a6 = c.a6;
        EditWindow *current = ptr<EditWindow>(d.Edt_Current);
        c.a4 = reinterpret_cast<uint8_t *>(current);
        c.a6 = ptr<uint8_t>(current->Edt_Prg);
        draw_windows();
        c.a4 = a4;
        c.a6 = a6;
        ec_fn(ec::EHide, EcEdit, 0);
        ec_fn(ec::First, EcEdit);
        Screen *s = screen(EcEdit);
        s->EcAWY = d.Ed_Wy;
        s->EcAW |= AWChange;
        app_centre(s, int16_t(d.Ed_Wy + (d.Ed_Sy >> 1)), 1, int16_t(d.Ed_VScrol));
        be_u16 *limits = &w.MouXMin;
        for (int i = 0; i < 4; i++)
            d.LimSave[i] = limits[i];
        limit_mouse();
        w.Actualise &= ~BitControl;
        Regs m;
        call(ed_menus_editor_entry, m);
        d.Ed_NewAppear = 2;
    }
    sy_fn(sy::ClearKey);
}

// Ed_Redraw: back from an accessory
void editor_redraw()
{
    ec_fn(ec::Active, EcEdit);
    ec_fn(ec::First, EcEdit);
    recop();
    limit_mouse();
}

} // namespace

extern "C" {
void ed_recop_fn(AllRegs *) { recop(); }
void ed_hide_fn(AllRegs *) { editor_hide(); }
void ed_appear_fn(AllRegs *) { editor_appear(); }
void ed_redraw(AllRegs *) { editor_redraw(); }
void ed_limit_mouse(AllRegs *) { limit_mouse(); }
void ed_memory_show(AllRegs *) { memory_show(); }
// d1/d2 x/y, d3 the width
void ed_memory_draw(AllRegs *r) { memory_draw(int16_t(r->d[1]), int16_t(r->d[2]), uint16_t(r->d[3]), false); }
void ed_memory_draw_title(AllRegs *r) { memory_draw(int16_t(r->d[1]), int16_t(r->d[2]), uint16_t(r->d[3]), true); }
void ed_unpack_fn(AllRegs *r) { unpack(uint16_t(r->d[0]), int16_t(r->d[1]), int16_t(r->d[2]), false); }
void ed_unpack_title(AllRegs *r) { unpack(uint16_t(r->d[0]), int16_t(r->d[1]), int16_t(r->d[2]), true); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_screen.inc"
#endif
