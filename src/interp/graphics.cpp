// Graphics instructions of +ILib.s: Palette, Default Palette, Fade,
// Polyline, Polygon, Channel.

#include "interp/interp.h"

#include "core/amiga.h"
#include "lib/banks.h"

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::interp;

namespace {

constexpr int32_t PolyDraw = -336;      // graphics.library
constexpr int AAreaSize = 16;           // AreaDraw's points

[[noreturn]] void screen_not_open() { screen_error(3); }

be_u16 *buffer() { return ptr<be_u16>(idata().Buffer); }

// Fn_New_Expentier: an integer after a token
uint32_t parameter_int()
{
    pc() += 2;
    return integer().v1;
}

// Plt: colours into palette until there is no comma; a negative one
// leaves its entry. (A value that is not an integer gets L_IntToFl1, as
// the original.)
void colours(be_u16 *palette)
{
    for (;;) {
        Value v = evaluate();
        if (kind(v))
            v = to_float(v);
        if (int32_t(v.v1) >= 0)
            *palette = uint16_t(v.v1 & 0xFFF);
        palette++;
        if (next_token() != tk::Vir)
            break;
    }
    pc() -= 2;
}

// A palette of "no change" (-1) in Buffer
be_u16 *unchanged()
{
    be_u16 *b = buffer();
    for (int i = 0; i < 32; i++)
        b[i] = 0xFFFF;
    return b;
}

void palette()
{
    if (!idata().ScOn)
        screen_not_open();
    be_u16 *p = unchanged();
    colours(p);
    Regs r;
    r.a[1] = address(p);
    ec_call(ec::SPal, r);
    if (r.d[0])
        screen_error(r.d[0]);
}

// Fade speed [To screen/-1 for the sprites [,mask] | ,colours...]
void fade()
{
    IData &d = idata();
    int32_t speed = int32_t(integer().v1);
    uint16_t t = peek_token();
    be_u16 *p;
    if (t == tk::To) {
        int32_t from = int32_t(parameter_int());
        const uint8_t *colours_from;
        if (from < 0) {
            uint8_t *bank = bank_bobs();
            if (!bank)
                error(36);
            colours_from = bank + 2 + 8 * peek_w(bank);
        } else {
            Regs e;
            e.d[1] = uint32_t(from);
            lib_call(L_GetEc, e);
            colours_from = ptr<uint8_t>(e.a[0]) + offsetof(Screen, EcPal);
        }
        uint32_t mask = 0xFFFFFFFF;
        if (peek_token() == tk::Vir)
            mask = parameter_int();
        Regs r;
        r.a[0] = address(colours_from);
        r.d[3] = mask;
        lib_call(L_PalRout, r);
        p = ptr<be_u16>(r.a[1]);
    } else if (t == tk::Vir) {
        p = unchanged();
        pc() += 2;
        colours(p);
    } else {
        p = buffer();
        for (int i = 0; i < 32; i++)
            p[i] = 0;
    }
    if (!d.ScOn)
        screen_not_open();
    if (!speed)
        fon_call();
    Regs f;
    f.d[1] = uint32_t(speed);
    f.a[1] = address(p);
    ec_call(ec::FadeOn, f);
    if (f.d[0])
        fon_call();
}

// A point: x,y
void point(int32_t *x, int32_t *y)
{
    *x = int32_t(integer().v1);
    *y = int32_t(parameter_int());
}

// Polyline [x,y] To x,y [To ...]: from the graphic cursor without the
// first point
void polyline()
{
    if (!idata().ScOn)
        screen_not_open();
    RastPort *rp = ptr<RastPort>(wdata().RastPort);
    be_i16 *points = ptr<be_i16>(idata().Buffer);
    int32_t x, y;
    if (peek_token() == tk::To)
        x = rp->cp_x, y = rp->cp_y;
    else
        point(&x, &y);
    points[0] = int16_t(x);
    points[1] = int16_t(y);
    uint16_t n = 1;
    while (next_token() == tk::To) {
        point(&x, &y);
        points[2 * n] = int16_t(x);
        points[2 * n + 1] = int16_t(y);
        n++;
    }
    pc() -= 2;
    if (n <= 1)
        fon_call();
    rp->cp_x = points[0];
    rp->cp_y = points[1];
    Regs r;
    r.d[0] = n;
    r.d[5] = uint32_t(PolyDraw);
    r.a[0] = address(points);
    r.a[1] = address(rp);
    lib_call(L_GfxFunc, r);
}

// Polygon [x,y] To x,y [To ...]: filled
void polygon()
{
    IData &d = idata();
    if (!d.ScOn)
        screen_not_open();
    RastPort *rp = ptr<RastPort>(wdata().RastPort);
    hal::init_area(reinterpret_cast<AreaInfo *>(d.AAreaInfo), d.AAreaBuf, AAreaSize);
    rp->AreaInfo = reinterpret_cast<AreaInfo *>(d.AAreaInfo);
    int32_t x, y;
    if (peek_token() == tk::To) {
        pc() += 2;
        x = rp->cp_x;
        y = rp->cp_y;
    } else {
        point(&x, &y);
        pc() += 2;
    }
    hal::area_move(rp, x, y);
    do {
        point(&x, &y);
        hal::area_draw(rp, x, y);
    } while (next_token() == tk::To);
    pc() -= 2;
    Regs g;
    lib_call(L_GetRas, g);
    hal::area_end(rp);
    Regs f;
    lib_call(L_FreeRas, f);
}

// Channel n To Sprite/Bob/Screen Display/Screen Size/Screen Offset/
// Rainbow m: what AMAL channel n moves
void channel()
{
    uint32_t n = integer().v1;
    if (n >= 64)
        fon_call();
    pc() += 2;
    uint16_t t = next_token();
    uint8_t type;
    uint32_t limit;
    switch (t) {
    case tk::Spr:
        type = 0, limit = 64;
        break;
    case tk::Bob:
        type = 1, limit = 64;
        break;
    case tk::ScD:
        type = 2, limit = 8;
        break;
    case tk::ScS:
        type = 3, limit = 8;
        break;
    case tk::ScO:
        type = 4, limit = 8;
        break;
    default:
        type = 6, limit = 4;    // Rainbow
    }
    uint32_t m = integer().v1;
    if (m >= limit)
        fon_call();
    uint8_t *c = reinterpret_cast<uint8_t *>(&idata().AnCanaux[n]);
    c[0] = type;
    c[1] = uint8_t(m);
}

} // namespace

extern "C" {
void interp_in_default_palette() { colours(idata().DefPal); }
void interp_in_palette() { palette(); }
void interp_in_fade() { fade(); }
void interp_in_polyline() { polyline(); }
void interp_in_polygon() { polygon(); }
void interp_in_channel() { channel(); }
}
