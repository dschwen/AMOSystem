// The mouse, coordinate conversions, zones and the hardware collision
// registers of AMOS.library (SyCall). The input handler and the VBL
// interrupt (+W.s) move the mouse; T_MouShow (negative: hidden) keeps the
// VBL from writing the mouse sprite while it changes. The collisions, which
// AMAL may test, keep their own copy of the screen to hardware
// conversion (w/collision.cpp).

#include "w/w.h"

using namespace amos;
using namespace amos::w;

// +W.s / w/sprites_glue.s
extern "C" uint8_t w_sprite_set[], w_sprite_unset[], w_sprites_show[];

namespace {

constexpr uint16_t EcYBase = 0x1000;
constexpr uint16_t Hide = 1 << (8 + 7);         // EcFlags
constexpr int LeftButton = 14, RightButton = 13, MiddleButton = 12;     // IEQUALIFIERB_

inline void barrier() { asm volatile("" ::: "memory"); }

volatile uint16_t &mouse_shown() { return *static_cast<volatile uint16_t *>(&wdata().MouShow); }

bool hires(const Screen *s) { return s->EcCon0 & 0x8000; }
bool interlaced(const Screen *s) { return s->EcCon0 & 4; }

// ---- showing the mouse -------------------------------------------------------------

// HAa3: the sprites made three times, once in each buffer
void show_sprites_3()
{
    for (int i = 0; i < 3; i++) {
        while (*static_cast<volatile uint32_t *>(&wdata().HsChange))
            ;
        call(w_sprites_show);
    }
}

// HiSho1: the mouse as sprite 0
void mouse_on()
{
    WData &d = wdata();
    mouse_shown() = 0xFFFF;
    Regs r;
    r.d[1] = 0;
    r.d[2] = d.XMouse;
    r.d[3] = d.YMouse;
    r.a[1] = d.MouDes;
    amos_call(&r, w_sprite_set);
    show_sprites_3();
    mouse_shown() = 0;
}

// HiHi
void mouse_off()
{
    Regs r;
    r.d[1] = 0;
    amos_call(&r, w_sprite_unset);
    show_sprites_3();
}

// HiSho: the new count, shown at 0, hidden at -1
void hide_show(uint16_t count)
{
    mouse_shown() = count;
    if (!count)
        mouse_on();
    else if (count == 0xFFFF)
        mouse_off();
}

// MChange: Change Mouse n: 0-2 the mouse bank, 3+ image n-3 of the sprite
// bank (16 pixels, 4 colours, else mouse 0)
void change(uint16_t n)
{
    WData &d = wdata();
    uint16_t shown = mouse_shown();
    mouse_shown() = 0xFFFF;
    const uint8_t *image;
    for (;;) {
        if (n < 3) {
            image = ptr<uint8_t>(d.MouBank);
            for (uint16_t i = 0; i < n; i++) {
                uint16_t words = uint16_t(uint32_t(peek_w(image)) * peek_w(image + 2) * peek_w(image + 4));
                image += 6 + 4 + int16_t(words << 1);
            }
            break;
        }
        const uint8_t *bank = ptr<uint8_t>(d.SprBank);
        if (bank && uint16_t(n - 3) < peek_w(bank)) {
            image = ptr<uint8_t>(peek_l(bank + 2 + uint16_t(n - 3) * 8));
            if (image && peek_w(image) == 1 && peek_w(image + 4) == 2)
                break;
        }
        n = 0;
    }
    d.MouSpr = n;
    d.MouDes = address(image);
    d.MouTy = peek_w(image + 2);
    d.MouHotX = peek_w(image + 6);
    d.MouHotY = peek_w(image + 8);
    if (int16_t(shown) >= 0)
        mouse_on();
    else
        mouse_off();
    mouse_shown() = shown;
}

// ---- coordinates ------------------------------------------------------------------

// EcToD1: the screen of d3 (0 the current one, n screen n-1). False with
// d0 3 if not opened; with d3 < 0, d1/d2 EntNul and d0 0.
bool screen_of(AllRegs *r, Screen **s)
{
    int16_t which = r->d[3];
    if (which < 0) {
        r->d[1] = EntNul;
        r->d[2] = EntNul;
        r->d[0] = 0;
        return false;
    }
    if (!which) {
        *s = current_screen();
        return true;
    }
    *s = ptr<Screen>(wdata().EcAdr[which - 1]);
    if (!*s) {
        r->d[0] = 3;
        return false;
    }
    return true;
}

// CXyScr: hardware coordinates to those of a screen
void to_screen(Screen *s, AllRegs *r)
{
    int16_t y = int16_t(r->d[2] + EcYBase - s->EcWY);
    if (interlaced(s))
        y <<= 1;
    y += s->EcVY;
    int16_t x = int16_t(r->d[1] - s->EcWX);
    if (hires(s))
        x <<= 1;
    x += s->EcVX;
    r->d[1] = int32_t(x);
    r->d[2] = int32_t(y);
    r->d[0] = 0;
}

// CXyHard (CXyS): screen coordinates to hardware ones
void to_hardware(Screen *s, AllRegs *r)
{
    int16_t x = r->d[1], y = r->d[2];
    if (hires(s))
        x >>= 1;
    r->d[1] = int32_t(int16_t(x + s->EcWX));
    if (interlaced(s))
        y >>= 1;
    r->d[2] = int32_t(int16_t(y + s->EcWY - EcYBase));
    r->d[0] = 0;
}

// GetSIn: d1 the screen (number < d4, from screen d3 in the display
// order) at hardware x,y, or EntNul
void screen_at(AllRegs *r)
{
    WData &d = wdata();
    const uint32_t *list = d.EcPri;
    uint16_t x = r->d[1], y = r->d[2] + EcYBase;
    if (int16_t(r->d[3]) > 0) {
        Screen *from;
        if (!screen_of(r, &from))
            return;
        for (const uint32_t *p = d.EcPri; int32_t(*p) >= 0; p++)
            if (*p == address(from)) {
                list = p;
                break;
            }
    }
    for (; int32_t(*list) >= 0; list++) {
        const Screen *s = ptr<Screen>(*list);
        if (uint16_t(r->d[4]) <= s->EcNumber || (s->EcFlags & Hide))
            continue;
        if (x < s->EcWX || uint16_t(x - s->EcWX) >= s->EcWTx)
            continue;
        if (y < s->EcWY || uint16_t(y - s->EcWY) >= s->EcWTy)
            continue;
        r->d[1] = s->EcNumber;
        r->d[0] = 0;
        return;
    }
    r->d[1] = EntNul;
    r->d[0] = 0;
}

// ---- zones ------------------------------------------------------------------------

// GZone: the zone of screen s at x,y (screen coordinates), 0 if none (or
// the screen's number is not below max)
uint32_t zone_at(const Screen *s, uint16_t x, uint16_t y, uint16_t max)
{
    if (max <= s->EcNumber || !s->EcAZones)
        return 0;
    const uint16_t *z = ptr<uint16_t>(s->EcAZones);
    for (uint16_t n = 0; n < s->EcNZones; n++, z += 4) {
        if (!z[2] && !z[3])
            continue;
        if (x >= z[0] && y >= z[1] && x <= z[2] && y <= z[3])
            return n + 1;
    }
    return 0;
}

// ZoEc: the zone at hardware x,y in screen s
uint32_t zone_hardware(const Screen *s, uint16_t hx, uint16_t hy, uint16_t max)
{
    if (max <= s->EcNumber)
        return 0;
    if (hx < s->EcWX)
        return 0;
    uint16_t x = hx - s->EcWX;
    if (x >= s->EcWTx)
        return 0;
    uint16_t y0 = hy + EcYBase;
    if (y0 < s->EcWY)
        return 0;
    uint16_t y = y0 - s->EcWY;
    if (y >= s->EcWTy)
        return 0;
    if (hires(s))
        x <<= 1;
    if (interlaced(s))
        y <<= 1;
    return zone_at(s, x + s->EcVX, y + s->EcVY, max);
}

} // namespace

extern "C" {
// MBout: d1 the buttons (1 left, 2 right, 4 middle)
void w_sy_mouse_key(AllRegs *r)
{
    WData &d = wdata();
    uint16_t keys = 0;
    if (d.AMOSHere >> 8) {
        uint16_t q = d.MouXOld;
        if (q & 1 << LeftButton)
            keys |= 1;
        if (q & 1 << RightButton)
            keys |= 2;
        if (q & 1 << MiddleButton)
            keys |= 4;
    }
    r->d[1] = (r->d[1] & 0xFFFF0000) | keys;
    r->d[0] = 0;
}

// MRout: d1 the buttons pressed since the last call, d2 those down then,
// d3 those down now
void w_sy_mouse_click(AllRegs *r)
{
    WData &d = wdata();
    uint16_t clicked = 0, before = 0, now = 0;
    if (d.AMOSHere >> 8) {
        before = d.OldMk;
        uint16_t q = d.MouXOld;
        const int bits[3] = {LeftButton, RightButton, MiddleButton};
        for (int i = 0; i < 3; i++) {
            if (!(q & 1 << bits[i]))
                continue;
            now |= 1 << i;
            if (!(before & 1 << i))
                clicked |= 1 << i;
        }
        d.OldMk = now;
    }
    r->d[1] = (r->d[1] & 0xFFFF0000) | clicked;
    r->d[2] = (r->d[2] & 0xFFFF0000) | before;
    r->d[3] = (r->d[3] & 0xFFFF0000) | now;
    r->d[0] = 0;
}

void w_sy_mouse_change(AllRegs *r)
{
    change(r->d[1]);
    r->d[0] = 0;
}

// MStore: keep the mouse, then the default one shown
void w_sy_mouse_store(AllRegs *r)
{
    WData &d = wdata();
    d.OMouShow = mouse_shown();
    d.OMouSpr = d.MouSpr;
    mouse_shown() = 0;
    change(0);
    r->d[0] = 0;
}

// MRecall
void w_sy_mouse_recall(AllRegs *r)
{
    WData &d = wdata();
    mouse_shown() = d.OMouShow;
    change(d.OMouSpr);
    r->d[0] = 0;
}

// MHide: Hide (d1 0: one level more), Hide On (d1 not 0)
void w_sy_mouse_hide(AllRegs *r)
{
    r->d[0] = 0;
    if (!wdata().CopON)
        return;
    hide_show(uint16_t(r->d[1]) ? 0xFFFF : uint16_t(mouse_shown() - 1));
}

// MShow: Show (one level less), Show On
void w_sy_mouse_show(AllRegs *r)
{
    r->d[0] = 0;
    if (!wdata().CopON)
        return;
    hide_show(uint16_t(r->d[1]) ? 0 : uint16_t(mouse_shown() + 1));
}

// MXy: the mouse in hardware coordinates (d0 unchanged)
void w_sy_mouse_xy(AllRegs *r)
{
    r->d[1] = wdata().XMouse;
    r->d[2] = wdata().YMouse;
    r->d[3] = 0;
}

void w_sy_xy_screen(AllRegs *r)
{
    Screen *s;
    if (screen_of(r, &s))
        to_screen(s, r);
}

void w_sy_xy_hard(AllRegs *r)
{
    Screen *s;
    if (screen_of(r, &s))
        to_hardware(s, r);
}

// CXyWi: screen coordinates to the column and line of the current
// window (EntNul outside), d0 negative if either is outside
void w_sy_xy_window(AllRegs *r)
{
    const Window *w = ptr<Window>(current_screen()->EcWindow);
    uint32_t line = EntNul, column = EntNul;
    int16_t y = int16_t(r->d[2] - w->WiDyI);
    if (y >= 0) {
        uint32_t dividend = (r->d[2] & 0xFFFF0000) | uint16_t(y);
        uint32_t q = dividend / w->WiTyCar;
        if (q <= 0xFFFF && uint16_t(q) < w->WiTyI)
            line = int32_t(int16_t(q));
    }
    int16_t x = int16_t((uint16_t(r->d[1]) >> 3) - w->WiDxI);
    if (x >= 0 && uint16_t(x) < w->WiTxI)
        column = int32_t(x);
    r->d[1] = column;
    r->d[2] = line;
    r->d[0] = line | column;
}

void w_sy_screen_at(AllRegs *r) { screen_at(r); }

// WMouScrFront: the screen under the mouse (d0), the mouse in it (d1/d2)
void w_sy_mouse_screen(AllRegs *r)
{
    WData &d = wdata();
    AllRegs a = *r;
    a.d[1] = d.XMouse;
    a.d[2] = d.YMouse;
    a.d[3] = 0;
    a.d[4] = 16;
    screen_at(&a);
    r->d[0] = a.d[0];
    r->d[1] = a.d[1];
    r->d[3] = a.d[1];
    if (int32_t(a.d[1]) < 0)
        return;
    uint32_t number = a.d[1];
    a.d[3] = number + 1;
    a.d[1] = d.XMouse;
    a.d[2] = d.YMouse;
    Screen *s;
    if (screen_of(&a, &s))
        to_screen(s, &a);
    r->d[1] = a.d[1];
    r->d[2] = a.d[2];
    r->d[3] = a.d[3];
    r->d[0] = number;
}

// MSetAb: X Mouse=, Y Mouse= (hardware; EntNul unchanged), within the
// limits
void w_sy_mouse_set(AllRegs *r)
{
    WData &d = wdata();
    if (r->d[1] != EntNul) {
        uint16_t x = uint16_t(r->d[1] << 1);
        if (x < d.MouXMin)
            x = d.MouXMin;
        if (x >= d.MouXMax)
            x = d.MouXMax;
        d.MouseX = x;
        d.XMouse = x >> 1;
    }
    if (r->d[2] != EntNul) {
        uint16_t y = uint16_t(r->d[2] << 1);
        if (y < d.MouYMin)
            y = d.MouYMin;
        if (y >= d.MouYMax)
            y = d.MouYMax;
        d.MouseY = y;
        d.YMouse = y >> 1;
    }
    r->d[0] = 0;
}

// MLimA: Limit Mouse x1,y1 To x2,y2 (hardware coordinates)
void w_sy_mouse_limit(AllRegs *r)
{
    WData &d = wdata();
    uint16_t x1 = r->d[1], y1 = r->d[2], x2 = r->d[3], y2 = r->d[4];
    if (x2 > 458)
        x2 = 458;
    if (y2 > 312)
        y2 = 312;
    if (int16_t(x1) < 0)
        x1 = 0;
    if (int16_t(y1) < 0)
        y1 = 0;
    if (x1 > x2) {
        uint16_t t = x1;
        x1 = x2;
        x2 = t;
    }
    if (y1 > y2) {
        uint16_t t = y1;
        y1 = y2;
        y2 = t;
    }
    d.MouXMin = x1 << 1;
    d.MouXMax = x2 << 1;
    d.MouYMin = y1 << 1;
    d.MouYMax = y2 << 1;
    r->d[0] = 0;
}

// MLimEc: Limit Mouse n: to screen d1 (0 the current one). (The
// assembler set limits from garbage when the screen was not opened; d0 is
// 3 then.)
void w_sy_mouse_limit_screen(AllRegs *r)
{
    AllRegs a = *r;
    a.d[3] = uint16_t(r->d[1]);
    a.d[1] = 0;
    a.d[2] = 0;
    Screen *s;
    if (!screen_of(&a, &s)) {
        r->d[0] = a.d[0];
        if (r->d[0])
            return;
        s = nullptr;
    }
    uint16_t x1 = 0, y1 = 0, x2 = 0, y2 = 0;
    if (s) {
        to_hardware(s, &a);
        x1 = a.d[1];
        y1 = a.d[2];
        a.d[1] = s->EcTx - 1;
        a.d[2] = s->EcTy - 1;
        to_hardware(s, &a);
        x2 = a.d[1];
        y2 = a.d[2];
    }
    r->d[1] = x1;
    r->d[2] = y1;
    r->d[3] = x2;
    r->d[4] = y2;
    w_sy_mouse_limit(r);
}

// SyResZ: Reserve Zone d1 for the current screen (0: none)
void w_sy_zones_reserve(AllRegs *r)
{
    Screen *s = current_screen();
    if (s->EcAZones) {
        free_mm(s->EcAZones, uint32_t(s->EcNZones) * 8);
        s->EcAZones = 0;
        s->EcNZones = 0;
    }
    r->d[0] = 0;
    uint16_t n = r->d[1];
    if (!n)
        return;
    uint32_t m = fast_mm(uint32_t(n) * 8);
    if (!m) {
        r->d[0] = 1;
        return;
    }
    s->EcAZones = m;
    s->EcNZones = n;
}

// SyRazZ: Reset Zone d1 (0: all)
void w_sy_zone_reset(AllRegs *r)
{
    Screen *s = current_screen();
    if (!s->EcAZones) {
        r->d[0] = 29;
        return;
    }
    uint32_t *z = ptr<uint32_t>(s->EcAZones);
    uint16_t n = r->d[1];
    if (!n) {
        uint16_t count = s->EcNZones - 1;
        do {
            *z++ = 0;
            *z++ = 0;
        } while (count-- != 0);
    } else {
        if (n > s->EcNZones) {
            r->d[0] = 16;
            return;
        }
        z[(n - 1) * 2] = 0;
        z[(n - 1) * 2 + 1] = 0;
    }
    r->d[0] = 0;
}

// SySetZ: Set Zone d1,x1,y1 To x2,y2 (d2-d5)
void w_sy_zone_set(AllRegs *r)
{
    Screen *s = current_screen();
    if (!s->EcAZones) {
        r->d[0] = 29;
        return;
    }
    uint16_t n = r->d[1];
    uint16_t x1 = r->d[2], y1 = r->d[3], x2 = r->d[4], y2 = r->d[5];
    if (!n || n > s->EcNZones || x1 >= x2 || y1 >= y2) {
        r->d[0] = 1;
        return;
    }
    uint16_t *z = ptr<uint16_t>(s->EcAZones) + (n - 1) * 4;
    z[0] = x1;
    z[1] = y1;
    z[2] = x2;
    z[3] = y2;
    r->d[0] = 0;
}

// SyZoGr: d1 the zone at x,y (screen coordinates) of screen d3 (below d5)
void w_sy_zone_graphic(AllRegs *r)
{
    Screen *s;
    if (!screen_of(r, &s))
        return;
    uint16_t x = r->d[1], y = r->d[2];
    r->d[3] = (r->d[3] & 0xFFFF0000) | x;
    r->d[4] = (r->d[4] & 0xFFFF0000) | y;
    r->d[0] = 0;
    r->d[1] = zone_at(s, x, y, r->d[5]);
}

// SyZoHd: the same at hardware x,y
void w_sy_zone_hard(AllRegs *r)
{
    Screen *s;
    if (!screen_of(r, &s))
        return;
    uint16_t x = r->d[1], y = r->d[2];
    r->d[3] = (r->d[3] & 0xFFFF0000) | x;
    r->d[4] = (r->d[4] & 0xFFFF0000) | y;
    r->d[0] = 0;
    r->d[1] = zone_hardware(s, x, y, r->d[5]);
}

// SyMouZ: the zone under the mouse: d1 its number in the high word, the
// screen in the low one; 0 if none
void w_sy_zone_mouse(AllRegs *r)
{
    WData &d = wdata();
    r->d[1] = 0;
    r->d[0] = 0;
    for (const uint32_t *p = d.EcPri; int32_t(*p) >= 0; p++) {
        const Screen *s = ptr<Screen>(*p);
        uint32_t z = zone_hardware(s, d.XMouse, d.YMouse, 16);
        if (z) {
            r->d[1] = z << 16 | s->EcNumber;
            return;
        }
    }
}

// HColSet: Set Hardcol sprites,enable,compare (CLXCON)
void w_sy_hardcol_set(AllRegs *r)
{
    chips().clxcon = uint16_t((r->d[1] & 15) << 12) | uint16_t((r->d[2] & 63) << 6) | (r->d[3] & 63);
    r->d[0] = 0;
}

// HColGet: =Hardcol(n) from CLXDAT (read once, which clears it): the
// sprites sprite pair n/2 touches into T_TColl, d1 -1 if any; n < 0 the
// playfields (d0 unchanged then)
void w_sy_hardcol_get(AllRegs *r)
{
    static const int8_t bits[4][8] = {
        {-1, 9, 10, 11, 1, 5, 0, 0},
        {9, -1, 12, 13, 2, 6, 0, 0},
        {10, 12, -1, 14, 3, 7, 0, 0},
        {11, 13, 14, -1, 4, 8, 0, 0},
    };
    WData &d = wdata();
    for (int i = 0; i < 8; i++)
        d.TColl[i] = 0;
    uint16_t clx = chips().clxdat;
    if (int16_t(r->d[1]) < 0) {
        r->d[1] = clx & 1 ? 0xFFFFFFFF : 0;
        return;
    }
    const int8_t *b = bits[(r->d[1] & 6) >> 1];
    uint16_t found = 0, pair = 3;
    r->d[1] = 0;
    for (int8_t bit = *b++;; pair <<= 2) {
        if (bit >= 0 && (clx >> (bit & 31) & 1)) {
            found |= pair;
            if (found < 0x100)
                r->d[1] = 0xFFFFFFFF;
        }
        bit = *b++;
        if (!bit)
            break;
    }
    poke_w(d.TColl, uint16_t(found >> 8 | found << 8));
    r->d[0] = 0;
}
}
