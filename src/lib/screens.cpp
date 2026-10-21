// Screen and drawing instructions of AMOSPro.Lib (Ecran.s): screens,
// colours, rainbows, copper, drawing, fonts, zones, blocks, Screen Copy,
// Appear, Zoom

#include "core/amiga.h"
#include "lib/banks.h"
#include "lib/convert.h"
#include "lib/strings.h"

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#define GRAPHICS_BASE_NAME (reinterpret_cast<struct GfxBase *>(amos::wdata().GfxBase))
#include <proto/graphics.h>
#define INTUITION_BASE_NAME (reinterpret_cast<struct IntuitionBase *>(amos::wdata().IntBase))
#include <proto/intuition.h>

// graphics/gfxbase.h defines NTSC, the name of an AMOS.library function
#undef NTSC

using namespace amos;

// AMOS's screen (intuition.library has a struct Screen too)
using EcScreen = amos::Screen;

namespace {

constexpr uint32_t EntNul = 0x80000000;

// graphics.library offsets for GfxFunc
constexpr int16_t LVOWritePixel = -324, LVODraw = -246, LVODrawEllipse = -180, LVOPolyDraw = -336,
                  LVOText = -60, LVORectFill = -306;

[[noreturn]] void screen_not_open() { lib_jump(L_ScNOp, 0); }
[[noreturn]] void ec_wi_error(uint32_t code) { lib_jump(L_EcWiErr, code); }
[[noreturn]] void block_fon_call() { lib_jump(L_BFonCall, 0); }
[[noreturn]] void cop_error(uint32_t code) { lib_jump(L_CopErr, code); }

void need_screen()
{
    if (!idata().ScOn)
        screen_not_open();
}

// GetScreen0: the current screen
EcScreen *screen0()
{
    if (!idata().ScOnAd)
        screen_not_open();
    return ptr<EcScreen>(idata().ScOnAd);
}

// CheckScreenNumber: 0-7 (0-9 for an accessory)
void check_screen_number(uint32_t n)
{
    if (n >= (idata().Prg_Accessory ? 10u : 8u))
        lib_jump(L_IllScN, 0);
}

struct RastPort *rastport() { return ptr<struct RastPort>(wdata().RastPort); }

// EcCall / SyCall / WiCall, then "Rbne L_EcWiErr" (or FonCall)
Regs &ec_check(int function, Regs &r)
{
    ec_call(function, r);
    if (r.d[0])
        ec_wi_error(r.d[0]);
    return r;
}

Regs &ec_fon(int function, Regs &r)
{
    ec_call(function, r);
    if (r.d[0])
        fon_call();
    return r;
}

Regs &ec_d1(int function, uint32_t d1)
{
    static Regs r;
    r = Regs{};
    r.d[1] = d1;
    return ec_check(function, r);
}

// GrXY: move the graphic cursor (either coordinate may be omitted)
struct RastPort *move_cursor(uint32_t x, uint32_t y)
{
    struct RastPort *rp = rastport();
    if (y != EntNul)
        rp->cp_y = static_cast<WORD>(y);
    if (x != EntNul)
        rp->cp_x = static_cast<WORD>(x);
    return rp;
}

// GfxFunc: a graphics.library function on the current RastPort, drawn in
// both buffers with Autoback. The registers are those of the function.
void gfx(int16_t lvo, Regs &r)
{
    r.d[5] = static_cast<uint16_t>(lvo);
    r.a[1] = wdata().RastPort;
    lib_call(L_GfxFunc, r);
}

// AdrEc: a screen by number, or null
EcScreen *screen_address(uint32_t n)
{
    Regs r = {};
    r.d[1] = n;
    ec_call(ec::AdrEc, r);
    return ptr<EcScreen>(r.d[0]);
}

// GetEc: a screen number, or a Logic()/Physic() value: the screen and its
// bitplanes (logic or physic)
EcScreen *get_screen(int32_t n, uint32_t **planes)
{
    EcScreen *s;
    bool physic = false;
    if (n >= 0) {
        if (n >= 8)
            fon_call();
        s = screen_address(n);
        if (!s)
            screen_not_open();
    } else if (static_cast<int16_t>(n) < 0) {
        s = screen0();
        physic = n & 1 << 30;
    } else {
        if (static_cast<uint16_t>(n) >= 8)
            fon_call();
        s = screen_address(n);
        if (!s)
            screen_not_open();
        physic = n & 1 << 30;
    }
    if (planes)
        *planes = physic ? s->EcPhysic : s->EcLogic;
    return s;
}

// Screen Open / Iff: make it the current screen
void set_current(EcScreen *s)
{
    idata().ScOnAd = address(s);
    idata().ScOn = s->EcNumber + 1;
}

} // namespace

// ---- register entries ---------------------------------------------------

// GetEc: d1 = screen. Out: a0 = screen, d0 = its logic or physic planes.
extern "C" uint32_t lib_get_ec(RegFrame &r)
{
    uint32_t *planes;
    r.a[0] = address(get_screen(r.d[1], &planes));
    return r.d[0] = address(planes);
}

// Sco0: the copy of Screen Copy, Scroll, the monitor. d0/d1 = source x/y,
// d2/d3 = destination, d4/d5 = source x2/y2, d6 = minterm, a0/a1 = the
// screens; SccEcO/SccEcD = their bitplanes.
extern "C" uint32_t lib_sco0(RegFrame &r)
{
    EcScreen *from = ptr<EcScreen>(r.a[0]), *to = ptr<EcScreen>(r.a[1]);
    int16_t x1 = static_cast<int16_t>(r.d[0]), y1 = static_cast<int16_t>(r.d[1]);
    int16_t x2 = static_cast<int16_t>(r.d[2]), y2 = static_cast<int16_t>(r.d[3]);
    int16_t w = static_cast<int16_t>(r.d[4]), h = static_cast<int16_t>(r.d[5]);
    if (x1 < 0) {
        x2 -= x1;
        x1 = 0;
    }
    if (y1 < 0) {
        y2 -= y1;
        y1 = 0;
    }
    if (x2 < 0) {
        x1 -= x2;
        x2 = 0;
    }
    if (y2 < 0) {
        y1 -= y2;
        y2 = 0;
    }
    if (static_cast<uint16_t>(x1) >= from->EcTx || static_cast<uint16_t>(y1) >= from->EcTy ||
        static_cast<uint16_t>(x2) >= to->EcTx || static_cast<uint16_t>(y2) >= to->EcTy)
        return 0;
    if (w < 0)
        return 0;
    if (static_cast<uint16_t>(w) > from->EcTx)
        w = from->EcTx;
    if (h < 0)
        return 0;
    if (static_cast<uint16_t>(h) > from->EcTy)
        h = from->EcTy;
    // the size (unsigned word arithmetic, as in the assembler)
    uint16_t sw = static_cast<uint16_t>(w - x1), sh = static_cast<uint16_t>(h - y1);
    if (static_cast<uint16_t>(w) <= static_cast<uint16_t>(x1) || !sw)
        return 0;
    if (static_cast<uint16_t>(h) <= static_cast<uint16_t>(y1) || !sh)
        return 0;
    int16_t over = static_cast<int16_t>(x2 + sw - to->EcTx);
    if (static_cast<uint16_t>(x2 + sw) > to->EcTx && over > 0) {
        if (sw <= static_cast<uint16_t>(over))
            return 0;
        sw -= over;
    }
    over = static_cast<int16_t>(y2 + sh - to->EcTy);
    if (static_cast<uint16_t>(y2 + sh) > to->EcTy && over > 0) {
        if (sh <= static_cast<uint16_t>(over))
            return 0;
        sh -= over;
    }
    // bitmaps for graphics.library, in chip memory
    uint8_t *chip = ptr<uint8_t>(wdata().ChipBuf);
    auto make_bitmap = [](uint8_t *bm, EcScreen *s, const uint32_t *planes) {
        poke_w(bm, s->EcTLigne);
        poke_w(bm + 2, s->EcTy);
        poke_w(bm + 4, s->EcNPlan);
        poke_w(bm + 6, 0);
        for (int i = 0; i < 6; i++)
            poke_l(bm + 8 + 4 * i, planes[i]);
    };
    make_bitmap(chip, from, ptr<uint32_t>(idata().SccEcO));
    make_bitmap(chip + 40, to, ptr<uint32_t>(idata().SccEcD));
    Regs b = {};
    b.d[0] = static_cast<uint16_t>(x1);
    b.d[1] = static_cast<uint16_t>(y1);
    b.d[2] = static_cast<uint16_t>(x2);
    b.d[3] = static_cast<uint16_t>(y2);
    b.d[4] = sw;
    b.d[5] = sh;
    b.d[6] = r.d[6];
    b.a[0] = address(chip);
    b.a[1] = address(chip + 40);
    b.a[2] = address(chip + 80);
    // the library's own blitter copy, for aligned plain copies
    ec_call(ec::ScCpyW, b);
    if (b.d[7])
        BltBitMap(reinterpret_cast<struct BitMap *>(chip), static_cast<int16_t>(x1), static_cast<int16_t>(y1),
                  reinterpret_cast<struct BitMap *>(chip + 40), static_cast<int16_t>(x2), static_cast<int16_t>(y2),
                  sw, sh, static_cast<uint8_t>(r.d[6]), 0xFF, ptr<PLANEPTR>(address(chip + 80)));
    return 0;
}

// ---- screens ------------------------------------------------------------

// DEFAULT
extern "C" void lib_in_default()
{
    Regs r = {};
    lib_call(L_DefRun1, r);
    lib_call(L_ReCop, r);
    lib_call(L_DefRun2, r);
    lib_call(L_ReCop, r);
    idata().DefFlag = 1;
}

// CLS [colour[,x,y TO x2,y2]]
extern "C" void lib_in_cls0()
{
    need_screen();
    Regs r = {};
    wi_call(wi::ClsWi, r);
}

static void cls(uint32_t colour, uint32_t x, uint32_t y, uint32_t x2, uint32_t y2)
{
    need_screen();
    Regs r = {};
    r.d[1] = colour;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = x2;
    r.d[5] = y2;
    ec_call(ec::ClsEc, r);
}

extern "C" void lib_in_cls1(uint32_t colour) { cls(colour, 0, 0, 10000, 10000); }
extern "C" void lib_in_cls5(uint32_t colour, uint32_t x, uint32_t y, uint32_t x2, uint32_t y2)
{
    cls(colour, x, y, x2, y2);
}

// =SCREEN HEIGHT[(n)], =SCREEN WIDTH[(n)], =SCREEN BASE, =SCREEN COLOUR,
// =SCREEN MODE
static EcScreen *numbered_screen(uint32_t n)
{
    check_screen_number(n);
    EcScreen *s = screen_address(n);
    if (!s)
        screen_not_open();
    return s;
}

extern "C" uint32_t lib_fn_screen_height0() { return screen0()->EcTy; }
extern "C" uint32_t lib_fn_screen_height1(uint32_t n) { return numbered_screen(n)->EcTy; }
extern "C" uint32_t lib_fn_screen_width0() { return screen0()->EcTx; }
extern "C" uint32_t lib_fn_screen_width1(uint32_t n) { return numbered_screen(n)->EcTx; }
extern "C" uint32_t lib_fn_screen_base() { return address(screen0()); }

extern "C" uint32_t lib_fn_screen_colour()
{
    EcScreen *s = screen0();
    return s->EcCon0 & 0x0800 ? 4096 : s->EcNbCol;
}

extern "C" uint32_t lib_fn_screen_mode() { return screen0()->EcCon0 & 0x8004; }

// =DISPLAY HEIGHT, =NTSC
extern "C" uint32_t lib_fn_display_height()
{
    Regs r = {};
    ec_call(ec::MaxRaw, r);
    return r.d[1];
}

extern "C" uint32_t lib_fn_ntsc()
{
    Regs r = {};
    ec_call(ec::CopForce, r);
    Regs n = {};
    ec_call(ec::NTSC, n);
    return n.d[1];
}

// =LOGBASE(plane), =PHYBASE(plane)
static uint32_t plane_base(uint32_t n, bool physic)
{
    EcScreen *s = screen0();
    if (static_cast<uint16_t>(n) >= s->EcNPlan)
        fon_call();
    uint16_t i = static_cast<uint16_t>(n);
    return physic ? s->EcPhysic[i] : s->EcLogic[i];
}

extern "C" uint32_t lib_fn_log_base(uint32_t n) { return plane_base(n, false); }
extern "C" uint32_t lib_fn_phy_base(uint32_t n) { return plane_base(n, true); }

// DOUBLE BUFFER, SCREEN SWAP [n]
extern "C" void lib_in_double_buffer()
{
    need_screen();
    Regs r = {};
    ec_check(ec::Double, r);
}

extern "C" void lib_in_screen_swap0()
{
    need_screen();
    Regs r = {};
    ec_check(ec::SwapScS, r);
}

extern "C" void lib_in_screen_swap1(uint32_t n)
{
    check_screen_number(n);
    ec_d1(ec::SwapSc, n);
}

// DUAL PLAYFIELD a,b / DUAL PRIORITY a,b
static void dual(int function, uint32_t a, uint32_t b)
{
    check_screen_number(b);
    check_screen_number(a);
    Regs r = {};
    r.d[1] = a;
    r.d[2] = b;
    ec_check(function, r);
}

extern "C" void lib_in_dual_playfield(uint32_t a, uint32_t b) { dual(ec::SetDual, a, b); }
extern "C" void lib_in_dual_priority(uint32_t a, uint32_t b) { dual(ec::PriDual, a, b); }

// SCREEN CLONE n
extern "C" void lib_in_screen_clone(uint32_t n)
{
    need_screen();
    check_screen_number(n);
    ec_d1(ec::CCloEc, n);
}

// SCREEN OPEN n,width,height,colours,mode
extern "C" void lib_in_screen_open(uint32_t n, uint32_t width, uint32_t height, uint32_t colours, uint32_t mode)
{
    Regs r = {};
    uint16_t m = static_cast<uint16_t>(mode & 0x8004);
    uint16_t planes;
    if (colours == 4096) {
        if (m & 0x8000)
            fon_call();
        planes = 6;
        m |= 0x0800;
        colours = 64;
    } else {
        planes = 1;
        uint16_t c = 2;
        while (colours != c) {
            c <<= 1;
            if (++planes >= 7)
                ec_wi_error(5);         // illegal number of colours
        }
    }
    check_screen_number(n);
    if (m & 0x8000 && planes > 4)
        fon_call();
    r.d[1] = n;
    r.d[2] = width;
    r.d[3] = height;
    r.d[4] = planes;
    r.d[5] = m;
    r.d[6] = colours;
    r.a[1] = address(idata().DefPal);
    ec_check(ec::Cree, r);
    set_current(ptr<EcScreen>(r.a[0]));
    // colour 3 flashes, if there is one
    if (planes != 1) {
        Regs g = {};
        g.d[0] = 46;
        lib_call(L_Sys_GetMessage, g);
        Regs f = {};
        f.d[1] = 3;
        f.a[1] = g.a[0];
        ec_call(ec::Flash, f);
    }
}

// SCREEN CLOSE n: the screen behind becomes the current one
extern "C" void lib_in_screen_close(uint32_t n)
{
    check_screen_number(n);
    Regs &r = ec_d1(ec::Del, n);
    idata().ScOn = 0;
    idata().ScOnAd = 0;
    if (EcScreen *s = ptr<EcScreen>(r.a[0])) {
        if (s->EcNumber < 8)
            set_current(s);
    }
}

// SCREEN DISPLAY n,x,y,width,height
extern "C" void lib_in_screen_display(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    check_screen_number(n);
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = w;
    r.d[5] = h;
    ec_check(ec::AView, r);
}

// SCREEN OFFSET n,x,y
extern "C" void lib_in_screen_offset(uint32_t n, uint32_t x, uint32_t y)
{
    need_screen();
    check_screen_number(n);
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    ec_check(ec::OffSet, r);
}

// SCREEN SHOW [n], SCREEN HIDE [n]
static void show_hide(uint32_t n, uint32_t hide)
{
    check_screen_number(n);
    Regs r = {};
    r.d[1] = n;
    r.d[2] = hide;
    ec_check(ec::EHide, r);
}

static uint32_t current_number()
{
    uint16_t n = idata().ScOn;
    if (!n)
        screen_not_open();
    return n - 1u;
}

extern "C" void lib_in_screen_show0() { show_hide(current_number(), 0); }
extern "C" void lib_in_screen_show1(uint32_t n) { show_hide(n, 0); }
extern "C" void lib_in_screen_hide0() { show_hide(current_number(), -1u); }
extern "C" void lib_in_screen_hide1(uint32_t n) { show_hide(n, -1u); }

// AUTO VIEW ON/OFF, VIEW
extern "C" void lib_in_auto_view_on() { idata().ActuMask |= 1 << 12; }
extern "C" void lib_in_auto_view_off() { idata().ActuMask &= ~(1 << 12); }

extern "C" void lib_in_view()
{
    Regs r = {};
    ec_call(ec::CopMake, r);
    wdata().Actualise &= ~(1 << 12);
}

// SCREEN TO FRONT [n], SCREEN TO BACK [n]
extern "C" void lib_in_screen_to_front1(uint32_t n)
{
    check_screen_number(n);
    ec_d1(ec::First, n);
}

extern "C" void lib_in_screen_to_front0() { lib_in_screen_to_front1(current_number()); }

extern "C" void lib_in_screen_to_back1(uint32_t n)
{
    check_screen_number(n);
    ec_d1(ec::Last, n);
}

extern "C" void lib_in_screen_to_back0() { lib_in_screen_to_back1(current_number()); }

// SCREEN n, =SCREEN
extern "C" void lib_in_screen(uint32_t n)
{
    check_screen_number(n);
    set_current(ptr<EcScreen>(ec_d1(ec::Active, n).a[0]));
}

extern "C" uint32_t lib_fn_screen() { return static_cast<int16_t>(idata().ScOn - 1); }

// =LACED, =HIRES, =LOWRES
extern "C" uint32_t lib_fn_laced() { return 4; }
extern "C" uint32_t lib_fn_hires() { return 0x8000; }
extern "C" uint32_t lib_fn_lowres() { return 0; }

// ---- colours ------------------------------------------------------------

// COLOUR n,rgb / =COLOUR(n)
extern "C" void lib_in_colour(uint32_t n, uint32_t rgb)
{
    need_screen();
    Regs r = {};
    r.d[1] = n;
    r.d[2] = rgb;
    ec_check(ec::SCol, r);
}

extern "C" uint32_t lib_fn_colour(uint32_t n)
{
    need_screen();
    return ec_d1(ec::GCol, n).d[1];
}

// COLOUR BACK rgb
extern "C" void lib_in_colour_back(uint32_t rgb)
{
    idata().ColBack = static_cast<uint16_t>(rgb);
    Regs r = {};
    r.d[1] = rgb;
    ec_call(ec::SColB, r);
}

// PalRout / GSPal: the colours of palette whose bit is set in mask
static void set_palette(const uint16_t *palette, uint32_t mask)
{
    need_screen();
    uint16_t *to = ptr<uint16_t>(idata().Buffer);
    for (int i = 0; i < 32; i++)
        to[i] = mask & 1u << i ? palette[i] : 0xFFFF;
    Regs r = {};
    r.a[1] = address(to);
    ec_check(ec::SPal, r);
}

static const uint16_t *bank_palette(uint8_t *bank)
{
    if (!bank)
        lib_jump(L_BkNoRes, 0);
    return reinterpret_cast<const uint16_t *>(bank + 2 + peek_w(bank) * 8u);
}

// GET ICON PALETTE [mask], GET SPRITE PALETTE [mask], GET PALETTE n[,mask]
extern "C" void lib_in_get_icon_palette1(uint32_t mask) { set_palette(bank_palette(bank_icons()), mask); }
extern "C" void lib_in_get_icon_palette0() { lib_in_get_icon_palette1(-1u); }
extern "C" void lib_in_get_sprite_palette1(uint32_t mask) { set_palette(bank_palette(bank_bobs()), mask); }
extern "C" void lib_in_get_sprite_palette0() { lib_in_get_sprite_palette1(-1u); }

extern "C" void lib_in_get_palette2(int32_t n, uint32_t mask)
{
    set_palette(get_screen(n, nullptr)->EcPal, mask);
}

extern "C" void lib_in_get_palette1(int32_t n) { lib_in_get_palette2(n, -1u); }

// FLASH OFF, FLASH n,a$
extern "C" void lib_in_flash_off()
{
    need_screen();
    Regs r = {};
    ec_call(ec::FlRaz, r);
}

extern "C" void lib_in_flash(uint32_t n, const uint8_t *s)
{
    need_screen();
    Regs r = {};
    r.a[1] = address(str_to_buffer(s));
    r.d[1] = n;
    ec_check(ec::Flash, r);
}

// SHIFT OFF, SHIFT UP/DOWN delay,first,last,loop
extern "C" void lib_in_shift_off()
{
    need_screen();
    Regs r = {};
    ec_call(ec::ShRaz, r);
}

static void shift(uint32_t delay, int32_t first, uint32_t last, uint32_t loop, uint32_t down)
{
    need_screen();
    Regs r = {};
    r.d[1] = 1;
    r.d[2] = delay;
    r.d[3] = first < 0 ? 1 : first;
    r.d[4] = last;
    r.d[5] = down;
    r.d[6] = loop;
    ec_check(ec::Shift, r);
}

extern "C" void lib_in_shift_up(uint32_t delay, int32_t first, uint32_t last, uint32_t loop)
{
    shift(delay, first, last, loop, 0);
}

extern "C" void lib_in_shift_down(uint32_t delay, int32_t first, uint32_t last, uint32_t loop)
{
    shift(delay, first, last, loop, 1);
}

// ---- rainbows and copper ------------------------------------------------

// SET RAINBOW n,colour,length,r$,g$,b$
extern "C" void lib_in_set_rainbow7(uint32_t n, int32_t colour, uint32_t length, uint32_t r, uint32_t g, uint32_t b,
                                    uint32_t extra)
{
    if (length < 16 || length >= 32700)
        fon_call();
    if (colour < 0)
        fon_call();
    if (n >= 4)
        fon_call();
    Regs x = {};
    x.d[1] = n;
    x.d[2] = length;
    x.d[3] = colour;
    x.d[4] = r;
    x.d[5] = g;
    x.d[6] = b;
    x.d[7] = extra;
    ec_call(ec::RainSet, x);
    if (static_cast<int32_t>(x.d[0]) < 0)
        lib_jump(L_OOfMem, 0);
    if (x.d[0])
        fon_call();
}

extern "C" void lib_in_set_rainbow6(uint32_t n, int32_t colour, uint32_t length, uint32_t r, uint32_t g, uint32_t b)
{
    lib_in_set_rainbow7(n, colour, length, r, g, b, 0);
}

// RAINBOW n,base,y,height
extern "C" void lib_in_rainbow(uint32_t n, uint32_t base, uint32_t y, uint32_t h)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = base;
    r.d[3] = y;
    r.d[4] = h;
    ec_check(ec::RainDo, r);
}

// RAINBOW DEL [n]
extern "C" void lib_in_rainbow_del1(uint32_t n) { ec_d1(ec::RainDel, n); }
extern "C" void lib_in_rainbow_del0() { lib_in_rainbow_del1(-1u); }

// RAIN(n,line)=rgb, =RAIN(n,line)
static uint16_t *rain(uint32_t n, uint32_t line)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = line;
    return ptr<uint16_t>(ec_check(ec::RainVar, r).a[0]);
}

extern "C" void lib_in_rain(uint32_t n, uint32_t line, uint32_t rgb) { *rain(n, line) = rgb & 0x0FFF; }
extern "C" uint32_t lib_fn_rain(uint32_t n, uint32_t line) { return *rain(n, line); }

// COPPER ON/OFF, COP SWAP, COP RESET, COP WAIT, COP MOVE, COP MOVEL,
// =COP LOGIC
extern "C" void lib_in_cop_on()
{
    Regs r = {};
    r.d[1] = -1u;
    ec_call(ec::CopOnOff, r);
}

extern "C" void lib_in_cop_off()
{
    Regs r = {};
    ec_call(ec::CopOnOff, r);
}

static Regs &cop(int function, Regs &r)
{
    ec_call(function, r);
    if (r.d[0])
        cop_error(r.d[0]);
    return r;
}

extern "C" void lib_in_cop_swap()
{
    Regs r = {};
    cop(ec::CopSwap, r);
}

extern "C" void lib_in_cop_reset()
{
    Regs r = {};
    cop(ec::CopReset, r);
}

extern "C" void lib_in_cop_wait4(uint32_t x, uint32_t y, uint32_t xmask, uint32_t ymask)
{
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    r.d[3] = xmask;
    r.d[4] = ymask;
    cop(ec::CopWait, r);
}

extern "C" void lib_in_cop_wait2(uint32_t x, uint32_t y) { lib_in_cop_wait4(x, y, -1u, -1u); }

extern "C" void lib_in_cop_move(uint32_t reg, uint32_t value)
{
    Regs r = {};
    r.d[1] = reg;
    r.d[2] = value;
    cop(ec::CopMove, r);
}

extern "C" void lib_in_cop_movel(uint32_t reg, uint32_t value)
{
    Regs r = {};
    r.d[1] = reg;
    r.d[2] = value;
    cop(ec::CopMoveL, r);
}

extern "C" uint32_t lib_fn_cop_logic()
{
    Regs r = {};
    return cop(ec::CopBase, r).d[1];
}

// AUTOBACK n
extern "C" void lib_in_autoback(uint32_t n)
{
    if (n >= 3)
        fon_call();
    screen0()->EcAuto = static_cast<uint16_t>(n);
}

// ---- drawing ------------------------------------------------------------

// PLOT x,y[,colour]
extern "C" void lib_in_plot2(uint32_t x, uint32_t y)
{
    screen0();
    struct RastPort *rp = move_cursor(x, y);
    Regs r = {};
    r.d[0] = static_cast<uint16_t>(rp->cp_x);
    r.d[1] = static_cast<uint16_t>(rp->cp_y);
    gfx(LVOWritePixel, r);
}

extern "C" void lib_in_plot3(uint32_t x, uint32_t y, int32_t colour)
{
    need_screen();
    if (colour < 0)
        fon_call();
    if (static_cast<uint32_t>(colour) != EntNul)
        SetAPen(rastport(), colour);
    lib_in_plot2(x, y);
}

// =POINT(x,y) (RPoint)
static uint32_t point(uint32_t x, uint32_t y)
{
    struct RastPort *rp = move_cursor(x, y);
    return ReadPixel(rp, rp->cp_x, rp->cp_y);
}

extern "C" uint32_t lib_fn_point(uint32_t x, uint32_t y) { return point(x, y); }

// DRAW TO x,y / DRAW x1,y1 TO x2,y2
extern "C" void lib_in_draw_to(uint32_t x, uint32_t y)
{
    screen0();
    Regs r = {};
    r.d[0] = x;
    r.d[1] = y;
    gfx(LVODraw, r);
}

extern "C" void lib_in_draw(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    move_cursor(x1, y1);
    lib_in_draw_to(x2, y2);
}

// EllCir: an ellipse at x,y
static void ellipse(uint32_t x, uint32_t y, uint32_t rx, uint32_t ry)
{
    need_screen();
    struct RastPort *rp = move_cursor(x, y);
    Regs r = {};
    r.d[0] = static_cast<uint16_t>(rp->cp_x);
    r.d[1] = static_cast<uint16_t>(rp->cp_y);
    r.d[2] = rx;
    r.d[3] = ry;
    gfx(LVODrawEllipse, r);
}

// CIRCLE x,y,r: wider in hires
extern "C" void lib_in_circle(uint32_t x, uint32_t y, uint32_t r)
{
    if (!r)
        fon_call();
    EcScreen *s = screen0();
    uint32_t rx = r;
    if (s->EcCon0 & 0x8000)
        rx = (rx & 0xFFFF0000u) | static_cast<uint16_t>(rx << 1);
    ellipse(x, y, rx, r);
}

// ELLIPSE x,y,rx,ry
extern "C" void lib_in_ellipse(uint32_t x, uint32_t y, uint32_t rx, uint32_t ry)
{
    if (!ry || !rx)
        fon_call();
    ellipse(x, y, rx, ry);
}

// =XGR, =YGR, GR LOCATE x,y
extern "C" uint32_t lib_fn_xgr()
{
    need_screen();
    return static_cast<uint16_t>(rastport()->cp_x);
}

extern "C" uint32_t lib_fn_ygr()
{
    need_screen();
    return static_cast<uint16_t>(rastport()->cp_y);
}

extern "C" void lib_in_gr_locate(uint32_t x, uint32_t y)
{
    need_screen();
    move_cursor(x, y);
}

// BOX x1,y1 TO x2,y2
extern "C" void lib_in_box(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    need_screen();
    struct RastPort *rp = rastport();
    rp->cp_x = static_cast<WORD>(x1);
    rp->cp_y = static_cast<WORD>(y1);
    uint16_t *poly = ptr<uint16_t>(idata().Buffer);
    const uint32_t points[] = {x1, y2, x2, y2, x2, y1, x1, y1};
    for (int i = 0; i < 8; i++)
        poly[i] = static_cast<uint16_t>(points[i]);
    Regs r = {};
    r.d[0] = 4;
    r.a[0] = address(poly);
    gfx(LVOPolyDraw, r);
}

// CLIP [x1,y1 TO x2,y2]
extern "C" void lib_in_clip0()
{
    need_screen();
    Regs r = {};
    r.d[1] = -1u;
    ec_fon(ec::SetClip, r);
}

extern "C" void lib_in_clip4(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    need_screen();
    Regs r = {};
    r.d[1] = 0;
    r.d[2] = x1;
    r.d[3] = y1;
    r.d[4] = x2;
    r.d[5] = y2;
    ec_fon(ec::SetClip, r);
}

// ---- fonts and text -----------------------------------------------------

// GET ROM FONTS, GET DISC FONTS, GET FONTS
static void get_fonts(uint32_t which)
{
    Regs r = {};
    r.d[1] = which;
    r.a[1] = idata().Buffer;
    ec_call(ec::GFonts, r);
    if (r.d[0])
        lib_jump(L_OOfMem, 0);
}

extern "C" void lib_in_get_rom_fonts() { get_fonts(1); }
extern "C" void lib_in_get_disc_fonts() { get_fonts(2); }
extern "C" void lib_in_get_fonts() { get_fonts(3); }

// =FONT$(n): name, size and "Rom "/"Disc", 38 characters
extern "C" uint32_t lib_fn_font(int32_t n)
{
    if (n < 0)
        fon_call();
    Regs r = {};
    r.d[1] = n;
    ec_call(ec::GFont, r);
    if (static_cast<int32_t>(r.d[0]) < 0)
        lib_jump(L_FftE, 0);
    if (r.d[0])
        return str_empty();
    const uint8_t *font = ptr<uint8_t>(r.a[0]);
    uint8_t *s = str_alloc(40);
    poke_w(s, 38);
    char *text = reinterpret_cast<char *>(s + 2);
    for (int i = 0; i < 38; i++)
        text[i] = ' ';
    idata().HiChaine = address(s + 2 + 40);
    char *to = text;
    for (const char *p = ptr<char>(peek_l(font + 2)); (*to++ = *p++);)
        ;
    to[-1] = ' ';
    long_to_dec(text + 30, peek_w(font + 6), -1, false);
    const char *where = peek_w(font) == 1 ? "Rom " : "Disc";
    for (int i = 0; i < 5; i++)
        text[34 + i] = where[i];
    return address(s);
}

// SET FONT n
extern "C" void lib_in_set_font(int32_t n)
{
    need_screen();
    if (n < 0)
        fon_call();
    Regs r = {};
    r.d[1] = n;
    ec_call(ec::SFont, r);
    if (static_cast<int32_t>(r.d[0]) < 0)
        lib_jump(L_FftE, 0);
    if (r.d[0])
        lib_jump(L_FnfE, 0);
}

// TEXT x,y,a$
extern "C" void lib_in_text(uint32_t x, uint32_t y, const uint8_t *s)
{
    need_screen();
    move_cursor(x, y);
    if (uint16_t len = str_len(s)) {
        Regs r = {};
        r.d[0] = len;
        r.a[0] = address(str_chars(s));
        gfx(LVOText, r);
    }
}

// =TEXT BASE, =TEXT LENGTH(a$), =TEXT STYLES, SET TEXT n
extern "C" uint32_t lib_fn_text_base()
{
    need_screen();
    return rastport()->TxBaseline;
}

extern "C" uint32_t lib_fn_text_length(const uint8_t *s)
{
    need_screen();
    return static_cast<uint16_t>(TextLength(rastport(), reinterpret_cast<CONST_STRPTR>(str_chars(s)), str_len(s)));
}

extern "C" uint32_t lib_fn_text_style()
{
    need_screen();
    return rastport()->AlgoStyle;
}

extern "C" void lib_in_set_text(uint32_t n)
{
    need_screen();
    rastport()->AlgoStyle = static_cast<UBYTE>(n);
}

// ---- patterns, paint, bars ----------------------------------------------

// SET PATTERN n, SET PAINT n
extern "C" void lib_in_set_pattern(uint32_t n)
{
    need_screen();
    Regs r = {};
    r.d[1] = n;
    ec_fon(ec::Pattern, r);
}

extern "C" void lib_in_set_paint(uint32_t n)
{
    need_screen();
    struct RastPort *rp = rastport();
    rp->Flags = static_cast<UWORD>((rp->Flags & ~AREAOUTLINE) | (n ? AREAOUTLINE : 0));
}

// PAINT x,y[,mode]: fill from x,y (mode 1: up to the outline colour)
extern "C" void lib_in_paint3(uint32_t x, uint32_t y, uint32_t mode)
{
    need_screen();
    Regs ras = {};
    ras.d[1] = 0;
    lib_call(L_GetRas, ras);
    uint32_t colour = point(x, y);
    struct RastPort *rp = rastport();
    Regs r = {};
    r.d[1] = static_cast<uint16_t>(rp->cp_x);
    r.d[2] = static_cast<uint16_t>(rp->cp_y);
    r.d[3] = colour;
    r.d[4] = (mode & 0xFFFF0000u) | (mode & 1);
    r.d[5] = idata().Buffer;
    r.a[1] = address(rp);
    lib_call(L_GfxPnt, r);
    Regs f = {};
    lib_call(L_FreeRas, f);
}

extern "C" void lib_in_paint2(uint32_t x, uint32_t y) { lib_in_paint3(x, y, -1u); }

// BAR x1,y1 TO x2,y2
extern "C" void lib_in_bar(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    need_screen();
    if (static_cast<int16_t>(x2) <= static_cast<int16_t>(x1) || static_cast<int16_t>(y2) <= static_cast<int16_t>(y1))
        fon_call();
    struct RastPort *rp = rastport();
    rp->cp_x = static_cast<WORD>(x1);
    rp->cp_y = static_cast<WORD>(y1);
    Regs r = {};
    r.d[0] = x1;
    r.d[1] = y1;
    r.d[2] = x2;
    r.d[3] = y2;
    gfx(LVORectFill, r);
}

// SET TEMPRAS [address,]size
extern "C" void lib_in_set_tempras0() { idata().RasLock = 0; }

extern "C" void lib_in_set_tempras1(uint32_t size)
{
    if (size >= 65536 || size < 256)
        fon_call();
    idata().RasSize = static_cast<uint16_t>(size);
    lib_in_set_tempras0();
}

extern "C" void lib_in_set_tempras2(int32_t where, uint32_t size)
{
    idata().RasLock = 0;
    if (size >= 65536 || size < 256)
        fon_call();
    uint32_t a = bank_or_address(where);
    idata().RasSize = static_cast<uint16_t>(size);
    idata().RasLock = a;
}

// INK a[,b[,c]]: the pen, paper and outline colours
extern "C" void lib_in_ink3(uint32_t a, uint32_t b, uint32_t c)
{
    need_screen();
    struct RastPort *rp = rastport();
    if (c != EntNul)
        rp->AOlPen = static_cast<BYTE>(c);
    if (b != EntNul)
        SetBPen(rp, b);
    if (a != EntNul)
        SetAPen(rp, a);
}

extern "C" void lib_in_ink2(uint32_t a, uint32_t b) { lib_in_ink3(a, b, EntNul); }
extern "C" void lib_in_ink1(uint32_t a) { lib_in_ink3(a, EntNul, EntNul); }

// GR WRITING n, SET LINE pattern
extern "C" void lib_in_gr_writing(int32_t n)
{
    need_screen();
    if (n < 0)
        fon_call();
    SetDrMd(rastport(), n);
}

extern "C" void lib_in_set_line(uint32_t pattern)
{
    need_screen();
    rastport()->LinePtrn = static_cast<UWORD>(pattern);
}

// ---- sliders and scrolling ----------------------------------------------

// HSLIDER / VSLIDER x1,y1 TO x2,y2,total,position,size
static void slider(int function, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t total, int32_t pos,
                   int32_t size)
{
    const int32_t values[] = {size, pos, total, y2, x2, y1, x1};
    for (int32_t v : values)
        if (v < 0)
            fon_call();
    if (static_cast<uint32_t>(x2) <= static_cast<uint32_t>(x1) || static_cast<uint32_t>(y2) <= static_cast<uint32_t>(y1))
        fon_call();
    Regs r = {};
    r.d[1] = x2 - x1;
    r.d[2] = y2 - y1;
    r.d[3] = total;
    r.d[4] = pos;
    r.d[5] = size;
    r.d[6] = x1;
    r.d[7] = y1;
    ec_fon(function, r);
}

extern "C" void lib_in_hslider(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t total, int32_t pos,
                               int32_t size)
{
    slider(ec::HorSli, x1, y1, x2, y2, total, pos, size);
}

extern "C" void lib_in_vslider(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t total, int32_t pos,
                               int32_t size)
{
    slider(ec::VerSli, x1, y1, x2, y2, total, pos, size);
}

// SET SLIDER (eight colours and patterns)
extern "C" void lib_in_set_slider(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g,
                                  uint32_t h)
{
    Regs r = {};
    const uint32_t v[] = {a, b, c, d, e, f, g, h};
    for (int i = 0; i < 8; i++)
        r.d[i] = v[i];
    ec_fon(ec::SetSli, r);
}

// DEF SCROLL n,x1,y1 TO x2,y2,dx,dy
extern "C" void lib_in_def_scroll(uint32_t n, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, uint32_t dx,
                                  uint32_t dy)
{
    if (!n || n > 10)
        fon_call();
    uint16_t *entry = idata().DScrolls + (n - 1) * 6;
    const uint32_t v[] = {dy, dx, y2, x2, y1, x1};
    for (int i = 0; i < 6; i++)
        entry[i] = static_cast<uint16_t>(v[i]);
}

// DoScroll: a Def Scroll zone moves within its screen
static void do_scroll(const uint16_t *entry, EcScreen *s)
{
    RegFrame r = {};
    int16_t dy = static_cast<int16_t>(entry[0]), dx = static_cast<int16_t>(entry[1]);
    int16_t x1 = static_cast<int16_t>(entry[5]), y1 = static_cast<int16_t>(entry[4]);
    r.d[0] = static_cast<uint16_t>(x1);
    r.d[1] = static_cast<uint16_t>(y1);
    r.d[2] = static_cast<uint16_t>(dx + x1);
    r.d[3] = static_cast<uint16_t>(dy + y1);
    r.d[4] = entry[3];
    r.d[5] = entry[2];
    r.d[6] = 0xCC;
    r.a[0] = r.a[1] = address(s);
    idata().SccEcO = idata().SccEcD = address(s->EcCurrent);
    lib_sco0(r);
}

// SCROLL n
extern "C" void lib_in_scroll(uint32_t n)
{
    if (n - 1 >= 10)
        fon_call();
    const uint16_t *entry = idata().DScrolls + (n - 1) * 6;
    if (entry[0] == 0x8000)
        ec_wi_error(28);
    EcScreen *s = screen0();
    if (!s->EcAuto) {
        do_scroll(entry, s);
        return;
    }
    Regs r = {};
    ec_call(ec::AutoBack1, r);
    if (s->EcFlags & 1 << 5) {
        do_scroll(entry, s);
        ec_call(ec::AutoBack2, r);
        do_scroll(entry, s);
        ec_call(ec::AutoBack3, r);
    } else {
        do_scroll(entry, s);
        ec_call(ec::AutoBack4, r);
    }
}

// SCREEN COPY s1[,x1,y1,x2,y2] TO s2[,x3,y3][,minterm]
static void screen_copy(int32_t from, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, int32_t to, uint32_t x3,
                        uint32_t y3, uint32_t minterm)
{
    uint32_t *dplanes, *splanes;
    EcScreen *d = get_screen(to, &dplanes);
    idata().SccEcD = address(dplanes);
    EcScreen *s = get_screen(from, &splanes);
    idata().SccEcO = address(splanes);
    RegFrame r = {};
    r.d[0] = x1;
    r.d[1] = y1;
    r.d[2] = x3;
    r.d[3] = y3;
    r.d[4] = x2;
    r.d[5] = y2;
    r.d[6] = minterm;
    r.a[0] = address(s);
    r.a[1] = address(d);
    lib_sco0(r);
}

extern "C" void lib_in_screen_copy3(int32_t from, int32_t to, uint32_t minterm)
{
    screen_copy(from, 0, 0, 10000, 10000, to, 0, 0, minterm);
}

extern "C" void lib_in_screen_copy2(int32_t from, int32_t to) { lib_in_screen_copy3(from, to, 0xCC); }

extern "C" void lib_in_screen_copy9(int32_t from, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, int32_t to,
                                    uint32_t x3, uint32_t y3, uint32_t minterm)
{
    screen_copy(from, x1, y1, x2, y2, to, x3, y3, minterm);
}

extern "C" void lib_in_screen_copy8(int32_t from, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2, int32_t to,
                                    uint32_t x3, uint32_t y3)
{
    screen_copy(from, x1, y1, x2, y2, to, x3, y3, 0xCC);
}

// =PHYSIC[(n)], =LOGIC[(n)]: screen numbers for Screen Copy and friends
extern "C" uint32_t lib_fn_physic0() { return -1u; }
extern "C" uint32_t lib_fn_physic1(uint32_t n) { return n | 1u << 31 | 1u << 30; }
extern "C" uint32_t lib_fn_logic0() { return ~(1u << 30); }
extern "C" uint32_t lib_fn_logic1(uint32_t n) { return (n | 1u << 31) & ~(1u << 30); }

// ---- appear and zoom ----------------------------------------------------

// APPEAR s1 TO s2,step[,pixels]: s2 gets s1's pixels one by one, step
// apart
extern "C" void lib_in_appear4(int32_t from, int32_t to, uint32_t step, int32_t pixels)
{
    if (pixels < 0 || !step)
        fon_call();
    uint32_t *dplanes, *splanes;
    EcScreen *d = get_screen(to, &dplanes);
    EcScreen *s = get_screen(from, &splanes);
    IData &data = idata();
    data.SccEcD = address(dplanes);
    data.SccEcO = address(splanes);
    uint16_t planes = s->EcNPlan <= d->EcNPlan ? s->EcNPlan : d->EcNPlan;
    data.AppNPlan = planes - 1;
    uint32_t total = (static_cast<uint32_t>(s->EcTLigne) * s->EcTy) << 3;
    uint32_t count = pixels ? static_cast<uint32_t>(pixels) : total;
    while (step >= count)
        step -= count;
    uint32_t pos = 0;
    for (;;) {
        pos += step;
        if (pos >= total)
            pos -= total;
        uint32_t byte = pos >> 3;
        uint16_t line = static_cast<uint16_t>(byte / s->EcTLigne);
        uint16_t column = static_cast<uint16_t>(byte % s->EcTLigne);
        if (line < d->EcTy && column < d->EcTLigne) {
            uint32_t dest = static_cast<uint32_t>(line) * d->EcTLigne + column;
            uint8_t bit = static_cast<uint8_t>(7 - (pos & 7));
            for (int16_t p = 0; p <= static_cast<int16_t>(data.AppNPlan); p++) {
                uint8_t *sp = ptr<uint8_t>(splanes[p]), *dp = ptr<uint8_t>(dplanes[p]);
                if (sp[byte] & 1 << bit)
                    dp[dest] |= 1 << bit;
                else
                    dp[dest] &= ~(1 << bit);
            }
        }
        if (!--count)
            break;
        if (static_cast<int8_t>(*reinterpret_cast<volatile uint8_t *>(&wdata().Actualise)) < 0) {
            Regs t = {};
            lib_call(L_Test_PaSaut, t);
        }
    }
}

extern "C" void lib_in_appear3(int32_t from, int32_t to, uint32_t step) { lib_in_appear4(from, to, step, 0); }

namespace {

// ZooTab: how many source pixels each destination pixel steps over (a
// reduction) or whether it moves on (a zoom); -1 ends the table
uint16_t *zoom_table(uint16_t *t, uint16_t a1, uint16_t a2, uint16_t b1, uint16_t b2)
{
    uint16_t from = a2 - a1, to = b2 - b1;
    if (to < from) {
        int16_t n = -1;
        uint16_t acc = from - 1;
        for (uint16_t i = 0; i < from; i++) {
            n++;
            uint16_t before = acc;
            acc -= to;
            if (before < to) {
                acc += from;
                *t++ = static_cast<uint16_t>(n);
                n = 0;
            }
        }
        *t++ = 0xFFFF;
        return t;
    }
    *t++ = 0;
    uint16_t acc = to - 1;
    int16_t steps = static_cast<int16_t>(to - 2);
    if (steps >= 0) {
        for (int16_t i = 0; i <= steps; i++) {
            uint16_t before = acc;
            acc -= from;
            if (before < from) {
                acc += to;
                *t++ = 1;
            } else {
                *t++ = 0;
            }
        }
    }
    *t++ = 0xFFFF;
    return t;
}

} // namespace

// ZOOM s1,x1,y1,x2,y2 TO s2,x3,y3,x4,y4
extern "C" void lib_in_zoom(int32_t from, int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t to, int32_t x3,
                            int32_t y3, int32_t x4, int32_t y4)
{
    uint32_t *splanes, *dplanes;
    EcScreen *s = get_screen(from, &splanes);
    EcScreen *d = get_screen(to, &dplanes);
    int16_t nplanes = static_cast<int16_t>((s->EcNPlan <= d->EcNPlan ? s->EcNPlan : d->EcNPlan) - 1);
    Regs m = {};
    m.d[0] = 4096;
    sy_call(sy::MemFast, m);
    uint16_t *table_x = ptr<uint16_t>(m.a[0]);
    if (!table_x)
        lib_jump(L_OOfMem, 0);
    auto fail = [&]() {
        Regs f = {};
        f.a[1] = address(table_x);
        f.d[0] = 4096;
        sy_call(sy::MemFree, f);
        fon_call();
    };
    if (x4 < 0)
        fon_call();
    if (static_cast<uint16_t>(x4) > d->EcTx || static_cast<uint32_t>(x3) >= static_cast<uint32_t>(x4) || x2 < 0 ||
        static_cast<uint16_t>(x2) > s->EcTx || static_cast<uint32_t>(x1) >= static_cast<uint32_t>(x2))
        fail();
    uint32_t src = static_cast<uint16_t>(x1) >> 3, dst = static_cast<uint16_t>(x3) >> 3;
    int16_t sbit = static_cast<int16_t>(7 - (x1 & 7)), dbit = static_cast<int16_t>(7 - (x3 & 7));
    uint16_t *table_y = zoom_table(table_x, x1, x2, x3, x4);
    if (y4 < 0 || static_cast<uint16_t>(y4) > d->EcTy || static_cast<uint32_t>(y3) >= static_cast<uint32_t>(y4) ||
        y2 < 0 || static_cast<uint16_t>(y2) > s->EcTy || static_cast<uint32_t>(y1) >= static_cast<uint32_t>(y2))
        fail();
    src += static_cast<uint32_t>(static_cast<uint16_t>(y1)) * s->EcTLigne;
    dst += static_cast<uint32_t>(static_cast<uint16_t>(y3)) * d->EcTLigne;
    zoom_table(table_y, y1, y2, y3, y4);
    uint32_t sline = s->EcTLigne, dline = d->EcTLigne;
    const uint16_t *tx = table_x;
    uint16_t step = *tx++;
    if (step & 0x8000)
        goto done;
    for (;;) {
        if (step) {
            // move on in the source by step pixels
            uint32_t bytes = step >> 3;
            int16_t bits = static_cast<int16_t>(step & 7);
            sbit -= bits;
            if (sbit < 0) {
                bytes++;
                sbit += 8;
            }
            src += bytes;
        }
        for (int16_t p = 0; p <= nplanes; p++) {
            const uint8_t *sp = ptr<uint8_t>(splanes[p]);
            uint8_t *dp = ptr<uint8_t>(dplanes[p]);
            uint32_t s_off = src, d_off = dst;
            const uint16_t *ty = table_y;
            uint16_t ys = *ty++;
            while (!(ys & 0x8000)) {
                s_off += ys * sline;
                bool on = sp[s_off] & 1 << sbit;
                do {
                    if (on)
                        dp[d_off] |= 1 << dbit;
                    else
                        dp[d_off] &= ~(1 << dbit);
                    d_off += dline;
                    ys = *ty++;
                } while (!ys);
            }
        }
        if (--dbit < 0) {
            dbit = 7;
            dst++;
        }
        step = *tx++;
        if (step & 0x8000)
            break;
    }
done:
    Regs f = {};
    f.a[1] = address(table_x);
    f.d[0] = 4096;
    sy_call(sy::MemFree, f);
}

// ---- coordinates ----------------------------------------------------------

static uint32_t xy(int function, uint32_t d1, uint32_t d2, uint32_t screen, bool want_x)
{
    Regs r = {};
    r.d[1] = d1;
    r.d[2] = d2;
    r.d[3] = screen;
    sy_call(function, r);
    if (r.d[0])
        ec_wi_error(r.d[0]);
    return want_x ? r.d[1] : r.d[2];
}

// =X HARD([screen,]x), =Y HARD, =X SCREEN, =Y SCREEN
extern "C" uint32_t lib_fn_xhard1(uint32_t x)
{
    need_screen();
    return xy(sy::XyHard, x, 0, 0, true);
}

extern "C" uint32_t lib_fn_xhard2(uint32_t s, uint32_t x)
{
    return xy(sy::XyHard, x, 0, (s & 0xFFFF0000u) | static_cast<uint16_t>(s + 1), true);
}

extern "C" uint32_t lib_fn_yhard1(uint32_t y)
{
    need_screen();
    return xy(sy::XyHard, 0, y, 0, false);
}

extern "C" uint32_t lib_fn_yhard2(uint32_t s, uint32_t y) { return xy(sy::XyHard, 0, y, s + 1, false); }

extern "C" uint32_t lib_fn_xscreen1(uint32_t x)
{
    need_screen();
    return xy(sy::XyScr, x, 0, 0, true);
}

extern "C" uint32_t lib_fn_xscreen2(uint32_t s, uint32_t x) { return xy(sy::XyScr, x, 0, s + 1, true); }

extern "C" uint32_t lib_fn_yscreen1(uint32_t y)
{
    need_screen();
    return xy(sy::XyScr, 0, y, 0, false);
}

extern "C" uint32_t lib_fn_yscreen2(uint32_t s, uint32_t y) { return xy(sy::XyScr, 0, y, s + 1, false); }

// =X TEXT(x), =Y TEXT(y)
extern "C" uint32_t lib_fn_xtext(uint32_t x)
{
    need_screen();
    Regs r = {};
    r.d[1] = x;
    sy_call(sy::XyWin, r);
    return r.d[1];
}

extern "C" uint32_t lib_fn_ytext(uint32_t y)
{
    need_screen();
    Regs r = {};
    r.d[2] = y;
    sy_call(sy::XyWin, r);
    return r.d[2];
}

// =X GRAPHIC(x), =Y GRAPHIC(y)
static uint32_t graphic(int function, int32_t v)
{
    need_screen();
    if (v < 0)
        fon_call();
    Regs r = {};
    r.d[1] = v;
    wi_call(function, r);
    return r.d[1];
}

extern "C" uint32_t lib_fn_xgraphic(int32_t x) { return graphic(wi::XGrWi, x); }
extern "C" uint32_t lib_fn_ygraphic(int32_t y) { return graphic(wi::YGrWi, y); }

// ---- zones ----------------------------------------------------------------

// RESERVE ZONE [n], RESET ZONE [n], SET ZONE n,x1,y1 TO x2,y2
extern "C" void lib_in_reserve_zone1(int32_t n)
{
    if (n < 0)
        fon_call();
    need_screen();
    Regs r = {};
    r.d[1] = n;
    sy_call(sy::ResZone, r);
    if (r.d[0])
        lib_jump(L_OOfMem, 0);
}

extern "C" void lib_in_reserve_zone0() { lib_in_reserve_zone1(0); }

extern "C" void lib_in_reset_zone1(int32_t n)
{
    if (n < 0)
        fon_call();
    need_screen();
    Regs r = {};
    r.d[1] = n;
    sy_call(sy::RazZone, r);
    if (r.d[0])
        error(73);
}

extern "C" void lib_in_reset_zone0() { lib_in_reset_zone1(0); }

extern "C" void lib_in_set_zone(uint32_t n, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    if (!n)
        fon_call();
    need_screen();
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x1;
    r.d[3] = y1;
    r.d[4] = x2;
    r.d[5] = y2;
    sy_call(sy::SetZone, r);
    if (r.d[0])
        fon_call();
}

// =ZONE([screen,]x,y), =HZONE, =SCIN
static uint32_t zone(int function, uint32_t screen, uint32_t x, uint32_t y, int count_reg)
{
    screen++;
    need_screen();
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    r.d[3] = screen;
    r.d[count_reg] = 8;
    sy_call(function, r);
    if (function == sy::ScIn ? r.d[0] != 0 : static_cast<uint16_t>(r.d[0]) != 0)
        ec_wi_error(r.d[0]);
    return r.d[1];
}

extern "C" uint32_t lib_fn_zone2(uint32_t x, uint32_t y) { return zone(sy::ZoGr, -1u, x, y, 5); }
extern "C" uint32_t lib_fn_zone3(uint32_t s, uint32_t x, uint32_t y) { return zone(sy::ZoGr, s, x, y, 5); }
extern "C" uint32_t lib_fn_hzone2(uint32_t x, uint32_t y) { return zone(sy::ZoHd, -1u, x, y, 5); }
extern "C" uint32_t lib_fn_hzone3(uint32_t s, uint32_t x, uint32_t y) { return zone(sy::ZoHd, s, x, y, 5); }
extern "C" uint32_t lib_fn_scin2(uint32_t x, uint32_t y) { return zone(sy::ScIn, -1u, x, y, 4); }
extern "C" uint32_t lib_fn_scin3(uint32_t s, uint32_t x, uint32_t y) { return zone(sy::ScIn, s, x, y, 4); }

// =MOUSE SCREEN, =MOUSE ZONE
extern "C" uint32_t lib_fn_mouse_screen()
{
    need_screen();
    Regs r = {};
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    r.d[4] = 8;
    sy_call(sy::ScIn, r);
    return r.d[1];
}

extern "C" uint32_t lib_fn_mouse_zone()
{
    need_screen();
    Regs r = {};
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    r.d[5] = 8;
    sy_call(sy::ZoHd, r);
    return r.d[1];
}

// ---- blocks -------------------------------------------------------------

static void check_block(uint32_t n)
{
    if (!n || n >= 65536)
        block_fon_call();
}

// GET CBLOCK n,x,y,w,h / PUT CBLOCK n[,x,y] / DEL CBLOCK [n]
extern "C" void lib_in_get_cblock(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    check_block(n);
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    r.d[3] = w;
    r.d[4] = h;
    r.d[5] = n;
    ec_check(ec::CBlGet, r);
}

extern "C" void lib_in_put_cblock3(uint32_t n, uint32_t x, uint32_t y)
{
    check_block(n);
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    r.d[5] = n;
    ec_check(ec::CBlPut, r);
}

extern "C" void lib_in_put_cblock1(uint32_t n) { lib_in_put_cblock3(n, -1u, -1u); }

extern "C" void lib_in_del_cblock0()
{
    Regs r = {};
    ec_call(ec::CBlRaz, r);
}

extern "C" void lib_in_del_cblock1(uint32_t n)
{
    check_block(n);
    Regs r = {};
    r.d[5] = n;
    ec_check(ec::CBlDel, r);
}

// GET BLOCK n,x,y,w,h[,mask] / PUT BLOCK n[,x,y[,planes[,minterm]]] /
// DEL BLOCK [n]
extern "C" void lib_in_get_block6(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t mask)
{
    check_block(n);
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = w;
    r.d[5] = h;
    r.d[6] = mask;
    ec_check(ec::BlGet, r);
}

extern "C" void lib_in_get_block5(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    lib_in_get_block6(n, x, y, w, h, 0);
}

extern "C" void lib_in_put_block5(uint32_t n, uint32_t x, uint32_t y, uint32_t planes, uint32_t minterm)
{
    check_block(n);
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = planes;
    r.d[5] = minterm;
    r.a[1] = idata().Buffer;
    ec_check(ec::BlPut, r);
}

extern "C" void lib_in_put_block4(uint32_t n, uint32_t x, uint32_t y, uint32_t planes)
{
    lib_in_put_block5(n, x, y, planes, EntNul);
}

extern "C" void lib_in_put_block3(uint32_t n, uint32_t x, uint32_t y) { lib_in_put_block5(n, x, y, EntNul, EntNul); }
extern "C" void lib_in_put_block1(uint32_t n) { lib_in_put_block5(n, EntNul, EntNul, EntNul, EntNul); }

extern "C" void lib_in_del_block0()
{
    Regs r = {};
    ec_call(ec::BlRaz, r);
}

extern "C" void lib_in_del_block1(uint32_t n)
{
    check_block(n);
    ec_d1(ec::BlDel, n);
}

// HREV BLOCK n, VREV BLOCK n
static void reverse_block(uint32_t n, uint16_t how)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = how;
    ec_check(ec::BlRev, r);
}

extern "C" void lib_in_hrev_block(uint32_t n) { reverse_block(n, 0x8000); }
extern "C" void lib_in_vrev_block(uint32_t n) { reverse_block(n, 0x4000); }

// ---- AMOS and the Workbench ---------------------------------------------

// WB_Close / WB_Open
extern "C" uint32_t lib_wb_close(RegFrame &)
{
    IData &d = idata();
    if (!d.WB_Closed) {
        wdata().WFlags &= ~(1 << 3);
        d.WB_Closed = static_cast<uint8_t>(CloseWorkBench());
        if (d.WB_Closed)
            wdata().WFlags |= 1 << 3;
    }
    return 0;
}

extern "C" uint32_t lib_wb_open(RegFrame &r) { return r.d[0] = OpenWorkBench(); }

// CLOSE WORKBENCH (if the configuration allows it)
extern "C" void lib_in_close_workbench()
{
    if (idata().PI_AllowWB) {
        RegFrame r = {};
        lib_wb_close(r);
    }
}

// AMOS TO FRONT, AMOS TO BACK, =AMOS HERE, AMOS LOCK, AMOS UNLOCK
static uint32_t amos_wb(uint32_t how)
{
    Regs r = {};
    r.d[1] = how;
    ec_call(ec::AMOS_WB, r);
    return r.d[1];
}

extern "C" void lib_in_amos_to_front() { amos_wb(1); }
extern "C" void lib_in_amos_to_back() { amos_wb(0); }
extern "C" uint32_t lib_fn_amos_here() { return amos_wb(-1u); }

extern "C" void lib_in_amos_lock()
{
    amos_wb(1);
    wdata().NoFlip = 0xFFFF;
}

extern "C" void lib_in_amos_unlock() { wdata().NoFlip = 0; }
