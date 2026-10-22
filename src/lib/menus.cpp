// Menus of AMOSPro.Lib (Menus.s): the menu instructions and the menu
// engine, which draws the menu tree, follows the mouse and returns the
// choice.
//
// A menu is a tree of MenuItems: MnNext links the items of a branch,
// MnLat leads to the branch an item opens and MnPrev goes back (to the
// previous item, or to the parent for the first item of a branch, which
// has the MnFlat flag). What an item shows are "objects": small programs
// compiled from the Menu$ strings by MnObjet and run by MnODraw.
//
// The engine works in 16-bit screen coordinates, as the assembler did;
// the int16_t casts keep its word arithmetic.

#include "core/amiga.h"
#include "lib/banks.h"
#include "lib/dos.h"
#include "lib/sprites.h"

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#define GRAPHICS_BASE_NAME (reinterpret_cast<struct GfxBase *>(amos::wdata().GfxBase))
#include <proto/graphics.h>

// graphics/gfxbase.h defines NTSC, the name of an AMOS.library function
#undef NTSC

using namespace amos;

// AMOS's screen (intuition.library has a struct Screen too)
using EcScreen = amos::Screen;

// menus_glue.s
extern "C" uint8_t lib_menu_on_clear[], lib_menu_on_default[], lib_menu_on_error[];
extern "C" uint32_t mn_call_procedure(const void *fn, const Ctx *regs, uint32_t *pile, const uint8_t *name);

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint16_t BitMenu = 1 << 9, BitJump = 1 << 10;
constexpr int MnNDim = 8;
constexpr uint32_t MnLong = MenuItem::MnLong;

// MnFlag: the flags are its high byte; the low byte is set by Menu Called
enum : uint8_t {
    MnFlat = 1 << 0,    // first item of a branch: MnPrev is the parent
    MnFixed = 1 << 1,   // position set by the program or dragged
    MnSep = 1 << 2,     // starts a new box (Menu Separate)
    MnBar = 1 << 3,     // the next item goes below (else to the right)
    MnOff = 1 << 4,     // inactive
    MnTotal = 1 << 5,   // the box is as wide as the screen
    MnTBouge = 1 << 6,  // the whole branch can be dragged
    MnBouge = 1 << 7,   // the item can be dragged in its box
};

// MnDraw flags (bits of d7 in the assembler)
enum : uint32_t {
    DrawCalc = 1u << 31,        // only compute the size
    DrawActive = 1u << 30,      // the highlighted look
    DrawBackground = 1u << 29,  // the background object too
};

[[noreturn]] void screen_not_open() { lib_jump(L_ScNOp, 0); }
[[noreturn]] void out_of_memory() { lib_jump(L_OOfMem, 0); }
// MnNOp: "menu not opened"
[[noreturn]] void menu_not_opened() { error(38); }

MenuItem *item(uint32_t a) { return ptr<MenuItem>(a); }
uint8_t &flags(MenuItem *m) { return reinterpret_cast<uint8_t *>(&m->MnFlag)[0]; }
uint8_t &called(MenuItem *m) { return reinterpret_cast<uint8_t *>(&m->MnFlag)[1]; }
uint32_t &object(MenuItem *m, int n) { return (&m->MnObF)[n]; }   // MnObF, MnOb1-3
uint16_t object_size(uint32_t object) { return peek_w(ptr<void>(object)); }

struct RastPort *rastport() { return ptr<struct RastPort>(wdata().RastPort); }

// ---- AMOS.library -------------------------------------------------------

uint32_t sy_fn(int function, uint32_t d1 = 0)
{
    Regs r = {};
    r.d[1] = d1;
    sy_call(function, r);
    return r.d[0];
}

uint32_t ec_fn(int function, uint32_t d1 = 0)
{
    Regs r = {};
    r.d[1] = d1;
    ec_call(function, r);
    return r.d[0];
}

uint8_t *mem_fast(uint32_t length, bool clear = false)
{
    Regs r = {};
    r.d[0] = length;
    sy_call(clear ? sy::MemFastClear : sy::MemFast, r);
    return ptr<uint8_t>(r.a[0]);
}

void mem_free(uint32_t address, uint32_t length)
{
    Regs r = {};
    r.a[1] = address;
    r.d[0] = length;
    sy_call(sy::MemFree, r);
}

MenuItem *new_item()
{
    uint8_t *m = mem_fast(MnLong, true);
    if (!m)
        out_of_memory();
    return reinterpret_cast<MenuItem *>(m);
}

// XyMou + XyScr: the mouse in the coordinates of the current screen
void mouse_xy(int16_t &x, int16_t &y)
{
    Regs r = {};
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    sy_call(sy::XyScr, r);
    x = static_cast<int16_t>(r.d[1]);
    y = static_cast<int16_t>(r.d[2]);
}

uint32_t mouse_key()
{
    Regs r = {};
    sy_call(sy::MouseKey, r);
    return r.d[1];
}

// Patch_ScFront: the monitor's hook to bring the menu screen to the front
// (offset 0) and back (offset 4)
void monitor_patch(int offset)
{
    if (uint32_t patch = idata().Patch_ScFront) {
        Regs r = {};
        amos_call(&r, ptr<uint8_t>(patch) + offset);
    }
}

// ---- drawing attributes (ObInkA... in the assembler) --------------------

void ink_a(uint32_t pen) { SetAPen(rastport(), pen); }
void ink_b(uint32_t pen) { SetBPen(rastport(), pen); }
void ink_c(uint32_t pen) { rastport()->AOlPen = static_cast<UBYTE>(pen); }
void write_mode(uint32_t mode) { SetDrMd(rastport(), mode); }
void pattern(uint32_t n) { ec_fn(ec::Pattern, n); }
void outline(bool on)
{
    if (on)
        rastport()->Flags |= 0x10;
    else
        rastport()->Flags &= ~0x10;
}

// ---- background save -----------------------------------------------------

// The saved background of a box: this header, then the words of each
// line, plane after plane. The assembler kept the offset in a word, which
// restored at the wrong place on screens of more than 64K per plane.
struct Saved {
    uint32_t size;
    EcScreen *screen;
    uint32_t offset;    // of the first word in each plane
    uint16_t words, lines, planes;
};

// MnSaDel
void save_free(MenuItem *m)
{
    if (m->MnAdSave) {
        mem_free(m->MnAdSave, ptr<Saved>(m->MnAdSave)->size);
        m->MnAdSave = 0;
    }
}

// MnSave: save the screen under x1,y1 to x2,y2 for item m
void save(MenuItem *m, int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
    save_free(m);
    EcScreen *s = ptr<EcScreen>(idata().MnAdEc);
    int16_t tx = static_cast<int16_t>(s->EcTx), ty = static_cast<int16_t>(s->EcTy);
    x2 = static_cast<int16_t>(x2 + 1);
    y2 = static_cast<int16_t>(y2 + 1);
    if (x1 < 0)
        x1 = 0;
    if (y1 < 0)
        y1 = 0;
    if (x1 >= tx || y1 >= ty)
        return;
    if (x2 > tx)
        x2 = tx;
    if (y2 > ty)
        y2 = ty;
    if (x2 < 0 || y2 < 0)
        return;
    uint16_t first = static_cast<uint16_t>(x1) >> 4;
    uint16_t last = static_cast<uint16_t>(x2 + 15) >> 4;
    if (last <= first || static_cast<uint16_t>(y2) <= static_cast<uint16_t>(y1))
        return;
    uint16_t words = static_cast<uint16_t>(last - first);
    uint16_t lines = static_cast<uint16_t>(y2 - y1);
    uint32_t line = s->EcTLigne;
    uint32_t offset = line * static_cast<uint16_t>(y1) + 2 * first;
    uint32_t size = 2u * words * lines * s->EcNPlan + sizeof(Saved);
    Saved *saved = reinterpret_cast<Saved *>(mem_fast(size));
    if (!saved) {
        idata().MnError = 24;
        return;
    }
    m->MnAdSave = address(saved);
    *saved = {size, s, offset, words, lines, s->EcNPlan};
    auto to = reinterpret_cast<uint16_t *>(saved + 1);
    for (uint16_t y = 0; y < lines; y++, offset += line)
        for (uint16_t p = 0; p < s->EcNPlan; p++) {
            auto from = ptr<uint16_t>(s->EcPhysic[p] + offset);
            for (uint16_t w = 0; w < words; w++)
                *to++ = from[w];
        }
}

// MnRest: put back what save() kept
void restore(MenuItem *m)
{
    if (!m->MnAdSave)
        return;
    Saved *saved = ptr<Saved>(m->MnAdSave);
    EcScreen *s = saved->screen;
    auto from = reinterpret_cast<const uint16_t *>(saved + 1);
    uint32_t offset = saved->offset;
    for (uint16_t y = 0; y < saved->lines; y++, offset += s->EcTLigne)
        for (uint16_t p = 0; p < saved->planes; p++) {
            auto to = ptr<uint16_t>(s->EcPhysic[p] + offset);
            for (uint16_t w = 0; w < saved->words; w++)
                to[w] = *from++;
        }
}

// MnODVar: free the memory a (RE n) object reserved
void free_data(MenuItem *m)
{
    if (m->MnLData) {
        mem_free(m->MnDatas, m->MnLData);
        m->MnLData = 0;
        m->MnDatas = 0;
    }
}

// ---- objects ---------------------------------------------------------------

// Object commands (MnObjet compiles them, MnODraw runs them)
enum : uint16_t {
    ObEnd = 0, ObText = 4, ObBar = 8, ObLine = 12, ObEllipse = 16, ObPattern = 20, ObInk = 24,
    ObBob = 28, ObIcon = 32, ObLocate = 36, ObOutline = 40, ObSetLine = 44, ObFont = 48,
    ObProc = 52, ObReserve = 56, ObStyle = 60,
};

struct Size {
    int16_t x, y;       // cursor: where the next item goes
    int16_t w, h;       // the largest extent
};

// MnOCall: (PR name) calls an AMOS procedure, with the position and the
// state of the item in Dreg(0-4), the item in Areg(0) and its (RE n)
// memory in Areg(1). It can move the cursor through Dreg(0)/Dreg(1).
void call_procedure(const uint8_t *name, int16_t bx, int16_t by, int16_t &x, int16_t &y, uint32_t mode)
{
    IData &d = idata();
    // In direct mode the assembler did not skip the name either and ran it
    // as object commands
    if (d.Direct)
        return;
    d.CallReg[0] = static_cast<uint16_t>(x + bx);
    d.CallReg[1] = static_cast<uint16_t>(y + by);
    d.CallReg[2] = mode & DrawCalc ? 0 : -1u;
    d.CallReg[3] = mode & DrawActive ? -1u : 0;
    d.CallReg[4] = mode & DrawBackground ? -1u : 0;
    d.CallReg[8] = d.MnDAd;
    d.CallReg[9] = item(d.MnDAd)->MnDatas;
    // The procedure runs with the interpreter registers of MnGere / Menu
    // Calc (MnRA3); its End Proc comes back here through MnPile
    Ctx keep = ctx();
    uint32_t failed = mn_call_procedure(lib_address(L_MenuProcedure), ptr<Ctx>(d.MnRA3), &d.MnPile, name);
    ctx() = keep;
    if (!failed) {
        x = static_cast<int16_t>(d.CallReg[0] - bx);
        y = static_cast<int16_t>(d.CallReg[1] - by);
    }
}

// MnODraw: draw (or measure, with DrawCalc) object at bx,by with the
// default inks a, b, c
Size draw_object(uint32_t object, uint16_t a, uint16_t b, uint16_t c, int16_t bx, int16_t by, uint32_t mode)
{
    struct RastPort *rp = rastport();
    bool draw = !(mode & DrawCalc);
    rp->cp_x = bx;
    rp->cp_y = by;
    int16_t x = 0, y = 0, w = 0, h = 0;
    if (draw) {
        ink_a(a);
        ink_b(b);
        ink_c(c);
        write_mode(1);
    }
    const uint16_t *p = ptr<uint16_t>(object) + 1;
    for (;;) {
        uint16_t command = *p++;
        switch (command) {
        case ObEnd:
            return {x, y, w, h};
        case ObText: {
            uint16_t length = *p++;
            auto text = reinterpret_cast<STRPTR>(const_cast<uint16_t *>(p));
            p += (length + 1) / 2;
            if (draw) {
                rp->cp_x = static_cast<WORD>(x + bx);
                rp->cp_y = static_cast<WORD>(y + rp->TxBaseline + by);
                Text(rp, text, length);
            }
            x = static_cast<int16_t>(x + TextLength(rp, text, length));
            y = static_cast<int16_t>(y + rp->TxHeight);
            break;
        }
        case ObLocate: {
            int16_t lx = static_cast<int16_t>(p[0]), ly = static_cast<int16_t>(p[1]);
            p += 2;
            if (lx >= 0 && ly >= 0) {
                x = lx;
                y = ly;
            }
            break;
        }
        case ObBar: {
            int16_t x2 = static_cast<int16_t>(p[0]), y2 = static_cast<int16_t>(p[1]);
            p += 2;
            if (x2 < 0 || y2 < 0 || static_cast<uint16_t>(x2) <= static_cast<uint16_t>(x)
                || static_cast<uint16_t>(y2) <= static_cast<uint16_t>(y))
                break;
            int16_t x1 = x, y1 = y;
            x = static_cast<int16_t>(x2 + 1);
            y = static_cast<int16_t>(y2 + 1);
            if (draw)
                RectFill(rp, static_cast<int16_t>(x1 + bx), static_cast<int16_t>(y1 + by),
                         static_cast<int16_t>(x2 + bx), static_cast<int16_t>(y2 + by));
            break;
        }
        case ObLine: {
            int16_t x2 = static_cast<int16_t>(p[0]), y2 = static_cast<int16_t>(p[1]);
            p += 2;
            if (x2 < 0 || y2 < 0)
                break;
            rp->cp_x = static_cast<WORD>(x + bx);
            rp->cp_y = static_cast<WORD>(y + by);
            x = static_cast<int16_t>(x2 + 1);
            y = static_cast<int16_t>(y2 + 1);
            if (draw)
                Draw(rp, static_cast<int16_t>(x2 + bx), static_cast<int16_t>(y2 + by));
            break;
        }
        case ObEllipse: {
            int16_t rx = static_cast<int16_t>(p[0]), ry = static_cast<int16_t>(p[1]);
            p += 2;
            if (!rx || !ry)
                break;
            int16_t cx = x, cy = y;
            x = static_cast<int16_t>(x + rx + 1);
            y = static_cast<int16_t>(y + ry + 1);
            if (draw)
                DrawEllipse(rp, static_cast<int16_t>(cx + bx), static_cast<int16_t>(cy + by), rx, ry);
            break;
        }
        case ObBob:
        case ObIcon: {
            uint32_t n = *p++ & 0x3FFF;
            uint8_t *entry = image_entry(n, command == ObIcon);
            uint32_t image = peek_l(entry);
            if (!image)
                break;
            if (draw) {
                paste(static_cast<uint16_t>(x + bx), static_cast<uint16_t>(y + by), n, entry);
                rp = rastport();
            }
            x = static_cast<int16_t>(x + (peek_w(ptr<void>(image)) << 4));
            y = static_cast<int16_t>(y + peek_w(ptr<uint8_t>(image) + 2));
            break;
        }
        case ObInk: {
            int16_t which = static_cast<int16_t>(p[0]);
            uint16_t pen = p[1];
            p += 2;
            if (draw)
                which < 2 ? ink_a(pen) : which == 2 ? ink_b(pen) : ink_c(pen);
            break;
        }
        case ObPattern: {
            uint16_t n = *p++;
            if (draw)
                pattern(n);
            break;
        }
        case ObOutline:
            outline(*p++);
            break;
        case ObSetLine:
            rp->LinePtrn = *p++;
            break;
        case ObFont:
            ec_fn(ec::SFont, *p++);
            rp = rastport();
            break;
        case ObStyle:
            rp->AlgoStyle = static_cast<UBYTE>(*p++);
            break;
        case ObProc: {
            auto name = reinterpret_cast<const uint8_t *>(p);
            p += 1 + *p / 2;
            call_procedure(name, bx, by, x, y, mode);
            rp = rastport();
            break;
        }
        case ObReserve: {
            uint16_t length = *p++;
            if (!length || draw)
                break;
            MenuItem *m = item(idata().MnDAd);
            free_data(m);
            if (uint8_t *data = mem_fast(length, true)) {
                m->MnDatas = address(data);
                m->MnLData = length;
            }
            break;
        }
        }
        if (x > w)
            w = x;
        if (y > h)
            h = y;
    }
}

// MnDraw: draw (or measure) item m at x,y: the background object, then the
// normal, highlighted or inactive one. Returns where the next item goes
// and the size.
Size draw_item(MenuItem *m, int16_t x, int16_t y, uint32_t mode)
{
    idata().MnDAd = address(m);
    Size r = {0, 0, 0, 0};
    uint16_t w = 0, h = 0;
    uint32_t object = 0;
    uint16_t a = 0, b = 0, c = 0;
    if (m->MnObF) {
        if (mode & DrawBackground) {
            r = draw_object(m->MnObF, m->MnInkB1, m->MnInkA1, m->MnInkC1, x, y, mode);
            w = static_cast<uint16_t>(r.w);
            h = static_cast<uint16_t>(r.h);
            x = static_cast<int16_t>(x + r.x);
            y = static_cast<int16_t>(y + r.y);
        }
        // Only items with a background show their inactive object
        if (flags(m) & MnOff && m->MnOb3) {
            object = m->MnOb3;
            a = m->MnInkA1, b = m->MnInkB1, c = m->MnInkC1;
            mode &= ~DrawActive;
        }
    }
    if (!object && m->MnOb1) {
        object = m->MnOb1;
        a = m->MnInkA1, b = m->MnInkB1, c = m->MnInkC1;
        if (mode & DrawActive) {
            if (!m->MnOb2) {
                a = m->MnInkA2, b = m->MnInkB2, c = m->MnInkC2;
            } else {
                object = m->MnOb2;
                mode &= ~DrawActive;
            }
        }
    }
    if (object)
        r = draw_object(object, a, b, c, x, y, mode);
    if (static_cast<uint16_t>(r.w) > w)
        w = static_cast<uint16_t>(r.w);
    if (static_cast<uint16_t>(r.h) > h)
        h = static_cast<uint16_t>(r.h);
    return {r.x, r.y, static_cast<int16_t>(w), static_cast<int16_t>(h)};
}

// ---- layout ----------------------------------------------------------------

// MnCa0: the positions and sizes of the branch from m on, at x,y
void calc_branch(MenuItem *m, int16_t x, int16_t y)
{
    bool box = true;
    for (;;) {
        // A box without a background object gets a 2 pixel frame
        if (box && !m->MnObF) {
            x = static_cast<int16_t>(x + 2);
            y = static_cast<int16_t>(y + 2);
        }
        idata().MnNZone++;
        if (!(flags(m) & MnFixed)) {
            m->MnX = x;
            m->MnY = y;
        }
        Size s = draw_item(m, 0, 0, DrawCalc | DrawBackground);
        m->MnTx = s.w;
        m->MnTy = s.h;
        if (m->MnLat) {
            if (flags(m) & MnBar)
                calc_branch(item(m->MnLat), s.x, 0);
            else
                calc_branch(item(m->MnLat), 0, s.y);
        }
        if (flags(m) & MnBar)
            x = 0, y = s.y;
        else
            x = s.x, y = 0;
        if (!m->MnNext)
            return;
        m = item(m->MnNext);
        box = flags(m) & MnSep;
    }
}

void number_zones(uint32_t first, uint16_t &zone);

// The branches the items from m on open, the last one first
void number_laterals(MenuItem *m, uint16_t &zone)
{
    if (m->MnNext)
        number_laterals(item(m->MnNext), zone);
    if (m->MnLat)
        number_zones(m->MnLat, zone);
}

// MnPZo: number the zones of the tree, counting down: a branch, then the
// branches it opens
void number_zones(uint32_t first, uint16_t &zone)
{
    for (uint32_t i = first; i; i = item(i)->MnNext)
        item(i)->MnZone = zone--;
    number_laterals(item(first), zone);
}

// MnCalc
void calc()
{
    IData &d = idata();
    d.MnChange = 0;
    d.MnNZone = 0;
    calc_branch(item(d.MnBase), 0, 0);
    uint16_t zone = d.MnNZone;
    number_zones(d.MnBase, zone);
}

struct Box {
    int16_t x1, y1, x2, y2;
};

// MnMaxi: place the items of the box that starts with m at x,y and return
// the box (with its frame)
Box place_box(MenuItem *first, int16_t x, int16_t y)
{
    Box b = {32766, 32766, 0, 0};
    for (MenuItem *m = first;;) {
        x = static_cast<int16_t>(x + m->MnX);
        y = static_cast<int16_t>(y + m->MnY);
        m->MnXX = x;
        m->MnYY = y;
        if (x < b.x1)
            b.x1 = x;
        if (y < b.y1)
            b.y1 = y;
        if (x > b.x2)
            b.x2 = x;
        if (y > b.y2)
            b.y2 = y;
        int16_t x2 = static_cast<int16_t>(x + m->MnTx), y2 = static_cast<int16_t>(y + m->MnTy);
        if (x2 > b.x2)
            b.x2 = x2;
        if (y2 > b.y2)
            b.y2 = y2;
        if (!m->MnNext)
            break;
        m = item(m->MnNext);
        if (flags(m) & MnSep)
            break;
    }
    first->MnMX = static_cast<uint16_t>(b.x2 - b.x1);
    first->MnMY = static_cast<uint16_t>(b.y2 - b.y1);
    if (!first->MnObF) {
        b.x1 = static_cast<int16_t>(b.x1 - 2);
        b.y1 = static_cast<int16_t>(b.y1 - 2);
        b.x2 = static_cast<int16_t>(b.x2 + 2);
        b.y2 = static_cast<int16_t>(b.y2 + 2);
        if (!(flags(first) & MnBar) && flags(first) & MnTotal) {
            b.x1 = 0;
            b.x2 = static_cast<int16_t>(ptr<EcScreen>(idata().MnAdEc)->EcTx);
        }
    }
    return b;
}

// MnBranch: draw the branch that starts with m at x,y, saving the
// background of its boxes, and set the zones of its items
void draw_branch(MenuItem *m, int16_t x, int16_t y)
{
    for (;;) {
        Box b = place_box(m, x, y);
        save(m, b.x1, b.y1, b.x2, b.y2);
        if (!m->MnObF) {
            struct RastPort *rp = rastport();
            pattern(0);
            outline(true);
            rp->LinePtrn = 0xFFFF;
            ink_a(m->MnInkB1);
            ink_c(m->MnInkC1);
            write_mode(1);
            RectFill(rp, b.x1, b.y1, static_cast<int16_t>(b.x2 - 1), static_cast<int16_t>(b.y2 - 1));
        }
        MenuItem *previous;
        do {
            int16_t ix = static_cast<int16_t>(m->MnXX), iy = static_cast<int16_t>(m->MnYY);
            draw_item(m, ix, iy, DrawBackground);
            Regs r = {};
            r.d[1] = m->MnZone;
            r.d[2] = ix < 0 ? 0 : ix;
            r.d[3] = iy < 0 ? 0 : iy;
            r.d[4] = static_cast<uint16_t>(ix + m->MnTx);
            r.d[5] = static_cast<uint16_t>(iy + m->MnTy);
            sy_call(sy::SetZone, r);
            previous = m;
            if (!m->MnNext)
                return;
            m = item(m->MnNext);
        } while (!(flags(m) & MnSep));
        x = static_cast<int16_t>(previous->MnXX);
        y = static_cast<int16_t>(previous->MnYY);
    }
}

// MnEBranch: remove the zones of the branch from m on and put back the
// screen under its boxes (the last one first)
void erase_branch(MenuItem *first)
{
    MenuItem *m = first;
    for (;;) {
        sy_fn(sy::RazZone, m->MnZone);
        if (!m->MnNext)
            break;
        m = item(m->MnNext);
    }
    for (;;) {
        if (m->MnAdSave) {
            restore(m);
            save_free(m);
        }
        if (m == first || !m->MnPrev)
            break;
        m = item(m->MnPrev);
    }
}

// ---- the menu loop ---------------------------------------------------------

// The open branches: MnTable[0..level-1] are the items whose branches are
// shown
struct Levels {
    uint16_t level = 0;

    // MnDEff: close the branches above level to
    void close_to(uint16_t to)
    {
        IData &d = idata();
        while (level > to) {
            level--;
            MenuItem *m = item(d.MnTable[level]);
            d.MnTable[level] = 0;
            d.MnAct = address(m);
            d.MnZoAct = m->MnZone;
            erase_branch(item(m->MnLat));
        }
    }
};

// MnEnd1: back to normal after the menu (or Menu Calc)
void end1()
{
    sy_fn(sy::ResZone, 0);
    ec_fn(ec::MnStop);
    ec_fn(ec::Active, idata().MnScOn);
    idata().MnProc = 0;
}

// MnEnd: close all the menus
void end()
{
    IData &d = idata();
    if (static_cast<int16_t>(d.MnProc) >= 0) {
        Levels levels;
        while (d.MnTable[levels.level])
            levels.level++;
        levels.close_to(0);
        erase_branch(item(d.MnBase));
        sy_fn(sy::AMALUFrz);
    }
    end1();
}

// Activate the menu screen for drawing (MnGere, Menu Calc)
void start()
{
    IData &d = idata();
    d.MnScOn = ptr<EcScreen>(wdata().EcCourant)->EcNumber;
    if (!d.MnAdEc)
        screen_not_open();
    if (ec_fn(ec::Active, ptr<EcScreen>(d.MnAdEc)->EcNumber))
        screen_not_open();
}

// ZoToMn: the item with this zone, and the level of its branch
MenuItem *zone_item(uint16_t zone, uint16_t open, uint16_t &level)
{
    IData &d = idata();
    level = 0;
    MenuItem *m = item(d.MnBase);
    for (;;) {
        for (;;) {
            if (m->MnZone == zone)
                return m;
            if (!m->MnNext)
                break;
            m = item(m->MnNext);
        }
        if (level == open)
            return m;
        m = item(item(d.MnTable[level])->MnLat);
        level++;
    }
}

// Redraw the highlighted item normally (MnZoAct / MnAct)
void unhighlight()
{
    IData &d = idata();
    if (d.MnZoAct) {
        MenuItem *m = item(d.MnAct);
        draw_item(m, static_cast<int16_t>(m->MnXX), static_cast<int16_t>(m->MnYY), 0);
        d.MnZoAct = 0;
    }
}

// The outline that follows the mouse while an item or branch is dragged
struct Drag {
    int16_t dx, dy;             // from the mouse to the top left corner
    int16_t x1, y1, x2, y2;     // the limits
    int16_t w, h;               // its size

    // MnMgD: draw the outline at the mouse, twice: it is drawn in
    // complement mode, the second one erases the first
    void draw()
    {
        int16_t x, y;
        mouse_xy(x, y);
        x = static_cast<int16_t>(x + dx);
        y = static_cast<int16_t>(y + dy);
        if (x < x1)
            x = x1;
        if (y < y1)
            y = y1;
        int16_t xr = static_cast<int16_t>(x + w), yb = static_cast<int16_t>(y + h);
        if (xr > x2) {
            xr = x2;
            x = static_cast<int16_t>(xr - w);
        }
        if (yb > y2) {
            yb = y2;
            y = static_cast<int16_t>(yb - h);
        }
        xr = static_cast<int16_t>(xr - 1);
        yb = static_cast<int16_t>(yb - 1);
        struct RastPort *rp = rastport();
        rp->cp_x = x;
        rp->cp_y = y;
        WORD *points = ptr<WORD>(idata().MnTDraw);
        const WORD outline[10] = {x, y, xr, y, xr, yb, x, yb, x, y};
        for (int i = 0; i < 10; i++)
            points[i] = outline[i];
        sy_fn(sy::WaitVbl);
        PolyDraw(rp, 5, points);
        sy_fn(sy::WaitVbl);
        PolyDraw(rp, 5, points);
        rp->LinePtrn = static_cast<UWORD>(rp->LinePtrn << 1 | rp->LinePtrn >> 15);
    }
};

// MnBGoch: the left button over item m (whose branch is at level): drag
// the item in its box (Menu Item Movable) or the whole branch (Menu
// Movable)
void drag(MenuItem *m, uint16_t level, Levels &levels)
{
    IData &d = idata();
    levels.close_to(level);
    unhighlight();
    Drag g;
    MenuItem *redraw;
    if (flags(m) & (MnFlat | MnSep)) {
        if (!(flags(m) & MnTBouge))
            return;
        g.w = static_cast<int16_t>(m->MnMX);
        g.h = static_cast<int16_t>(m->MnMY);
        EcScreen *s = ptr<EcScreen>(d.MnAdEc);
        g.x1 = static_cast<int16_t>(1 - g.w);
        g.y1 = static_cast<int16_t>(1 - g.h);
        g.x2 = static_cast<int16_t>(s->EcTx + g.w - 2);
        g.y2 = static_cast<int16_t>(s->EcTy + g.h - 2);
        redraw = m;
    } else {
        if (!(flags(m) & MnBouge))
            return;
        MenuItem *box = m;
        do
            box = item(box->MnPrev);
        while (!(flags(box) & (MnFlat | MnSep)));
        g.x1 = static_cast<int16_t>(box->MnXX);
        g.y1 = static_cast<int16_t>(box->MnYY);
        g.x2 = static_cast<int16_t>(box->MnMX + g.x1);
        g.y2 = static_cast<int16_t>(box->MnMY + g.y1);
        g.w = static_cast<int16_t>(m->MnTx);
        g.h = static_cast<int16_t>(m->MnTy);
        redraw = box;
    }
    int16_t mx, my;
    mouse_xy(mx, my);
    g.dx = static_cast<int16_t>(m->MnXX - mx);
    g.dy = static_cast<int16_t>(m->MnYY - my);
    // MnMgI: complement mode, dashed
    ink_a(31);
    ink_b(0);
    ink_c(0);
    write_mode(3);
    rastport()->LinePtrn = 0x00FF;
    do
        g.draw();
    while ((mouse_key() & 3) == 3);
    // Move the item, and back the next one (positions are relative)
    const WORD *corner = ptr<WORD>(d.MnTDraw);
    int16_t dx = static_cast<int16_t>(corner[0] - m->MnXX), dy = static_cast<int16_t>(corner[1] - m->MnYY);
    m->MnX = static_cast<uint16_t>(m->MnX + dx);
    m->MnY = static_cast<uint16_t>(m->MnY + dy);
    flags(m) |= MnFixed;
    if (m->MnNext) {
        MenuItem *next = item(m->MnNext);
        if (flags(m) & (MnFlat | MnSep)) {
            // The whole box moved: only the next box comes back
            while (next && !(flags(next) & MnSep))
                next = next->MnNext ? item(next->MnNext) : nullptr;
        }
        if (next) {
            next->MnX = static_cast<uint16_t>(next->MnX - dx);
            next->MnY = static_cast<uint16_t>(next->MnY - dy);
            flags(next) |= MnFixed;
        }
    }
    // MnMgF
    write_mode(1);
    rastport()->LinePtrn = 0xFFFF;
    erase_branch(redraw);
    int16_t x = static_cast<int16_t>(d.MnBaseX), y = static_cast<int16_t>(d.MnBaseY);
    if (redraw->MnPrev) {
        x = static_cast<int16_t>(item(redraw->MnPrev)->MnXX);
        y = static_cast<int16_t>(item(redraw->MnPrev)->MnYY);
    }
    draw_branch(redraw, x, y);
}

} // namespace

// ---- routines --------------------------------------------------------------

// MnGere: show the menu and follow the mouse until the right button is
// released. Out: d0 = error number, or 0 (MnChoice / MnChoix get the
// choice).
extern "C" uint32_t lib_mn_gere(RegFrame &frame)
{
    IData &d = idata();
    // The registers for the procedures of (PR) objects
    Ctx regs = ctx();
    d.MnRA3 = address(&regs);
    d.MnError = 0;
    start();
    monitor_patch(0);
    ec_fn(ec::MnStart);
    if (d.MnChange) {
        d.MnProc = -1;
        calc();
    }
    if (sy_fn(sy::ResZone, d.MnNZone))
        d.MnError = 24;
    d.MnProc = 1;
    sy_fn(sy::AMALFrz);
    int16_t bx = 0, by = 0;
    if (d.MnMouse)
        mouse_xy(bx, by);
    d.MnBaseX = bx;
    d.MnBaseY = by;
    draw_branch(item(d.MnBase), bx, by);

    Levels levels;
    d.MnTable[0] = 0;
    d.MnZoAct = 0;
    uint16_t zone = 0, level = 0;
    MenuItem *m = nullptr;
    for (;;) {
        sy_fn(sy::WaitVbl);
        if (d.MnError) {
            // The assembler restored the registers in the wrong order here
            end();
            wdata().Actualise &= ~BitJump;
            d.MnChoice = 0;
            return frame.d[0] = d.MnError;
        }
        // Redraw the items with Menu Called
        const uint32_t *open = d.MnTable;
        for (MenuItem *c = item(d.MnBase);;) {
            if (called(c))
                draw_item(c, static_cast<int16_t>(c->MnXX), static_cast<int16_t>(c->MnYY),
                          *open == address(c) ? static_cast<uint32_t>(DrawActive) : 0u);
            if (c->MnNext) {
                c = item(c->MnNext);
                continue;
            }
            if (!*open)
                break;
            c = item(item(*open++)->MnLat);
        }
        // With shift pressed the mouse moves freely
        bool shift = sy_fn(sy::Shifts) & 3;
        if (!shift) {
            // MnLp0/MnLp1: the zone under the mouse
            Regs r = {};
            sy_call(sy::GetZone, r);
            zone = 0;
            if (r.d[1] && static_cast<uint16_t>(r.d[1]) == ptr<EcScreen>(d.MnAdEc)->EcNumber)
                zone = static_cast<uint16_t>(r.d[1] >> 16);
            if (!zone)
                level = levels.level;
        }
        for (;;) {
            if (!shift) {
                uint32_t keys = mouse_key();
                if (!(keys & 2))
                    goto exit;
                if (keys & 1) {
                    if (zone) {
                        m = zone_item(zone, levels.level, level);
                        drag(m, level, levels);
                    }
                    goto next;
                }
                if (zone) {
                    m = zone_item(zone, levels.level, level);
                    if (flags(m) & MnOff)
                        shift = true;
                }
            }
            if (shift) {
                zone = 0;
                level = levels.level;
            }
            if (zone == d.MnZoAct)
                goto next;
            if (zone) {
                // Already open on a lower level: as if outside
                bool is_open = false;
                for (uint16_t i = 0; i < levels.level; i++)
                    if (item(d.MnTable[i])->MnZone == zone)
                        is_open = true;
                if (is_open) {
                    zone = 0;
                    level = levels.level;
                    shift = false;
                    continue;
                }
            }
            break;
        }
        levels.close_to(level);
        unhighlight();
        d.MnZoAct = zone;
        if (zone) {
            d.MnAct = address(m);
            draw_item(m, static_cast<int16_t>(m->MnXX), static_cast<int16_t>(m->MnYY), DrawActive);
            if (m->MnLat) {
                d.MnTable[levels.level++] = address(m);
                draw_branch(item(m->MnLat), static_cast<int16_t>(m->MnXX), static_cast<int16_t>(m->MnYY));
                d.MnZoAct = 0;
            }
        }
    next:;
    }

exit:
    // MnExit: the choice is the path to the highlighted item
    for (int i = 0; i < MnNDim; i++)
        d.MnChoix[i] = 0;
    d.MnChoice = 0;
    if (d.MnZoAct) {
        uint16_t i = 0;
        for (; i < levels.level; i++)
            d.MnChoix[i] = item(d.MnTable[i])->MnNb;
        d.MnChoix[i] = item(d.MnAct)->MnNb;
        wdata().Actualise |= BitJump;
        d.MnChoice = -1;
    }
    end();
    monitor_patch(4);
    return frame.d[0] = 0;
}

// OMnEff: forget the labels of On Menu
extern "C" uint32_t lib_omn_eff(RegFrame &)
{
    IData &d = idata();
    if (d.OMnNb) {
        mem_free(d.OMnBase, static_cast<uint16_t>(d.OMnNb << 2));
        d.OMnNb = 0;
        d.OMnBase = 0;
    }
    return 0;
}

// MnFind: the item at the coordinates on the parameter stack (a3, the
// first one deepest), d5 of them. Out: a2 = the item, d0 = -1, or d0 = 0.
extern "C" uint32_t lib_mn_find(RegFrame &r)
{
    uint16_t depth = static_cast<uint16_t>(r.d[5]);
    auto coords = reinterpret_cast<const uint32_t *>(ctx().a3);
    if (!coords[0] || coords[0] >= 1024)
        fon_call();
    r.d[0] = 0;
    uint32_t next = idata().MnBase;
    for (uint16_t i = depth; next;) {
        uint32_t n = coords[--i];
        if (!n || n >= 1024)
            fon_call();
        MenuItem *m;
        for (;;) {
            m = item(next);
            r.a[2] = next;
            if (static_cast<uint16_t>(n) == m->MnNb)
                break;
            if (static_cast<uint16_t>(n) < m->MnNb || !m->MnNext)
                return 0;
            next = m->MnNext;
        }
        if (!i)
            return r.d[0] = -1u;
        next = m->MnLat;
    }
    return 0;
}

// MnIns: insert a new item at the coordinates on the parameter stack (as
// MnFind), with the default flags of its level. Out: a2 = the item.
extern "C" uint32_t lib_mn_ins(RegFrame &r)
{
    IData &d = idata();
    uint16_t depth = static_cast<uint16_t>(r.d[5]);
    auto coords = reinterpret_cast<const uint32_t *>(ctx().a3);
    int16_t left = static_cast<int16_t>(depth - 1);
    uint16_t i = depth;
    MenuItem *m = nullptr;
    MenuItem *n;
    uint32_t next = d.MnBase;
    if (next) {
        for (;;) {
            uint16_t number = static_cast<uint16_t>(coords[--i]);
            for (;;) {
                m = item(next);
                if (number == m->MnNb)
                    break;
                if (number < m->MnNb) {
                    // Before m
                    if (left)
                        fon_call();
                    n = new_item();
                    MenuItem *previous = item(m->MnPrev);
                    m->MnPrev = address(n);
                    n->MnPrev = address(previous);
                    n->MnNext = address(m);
                    if (flags(m) & MnFlat) {
                        flags(m) &= ~MnFlat;
                        flags(n) |= MnFlat;
                        if (previous)
                            previous->MnLat = address(n);
                        else
                            d.MnBase = address(n);
                    } else {
                        previous->MnNext = address(n);
                    }
                    goto done;
                }
                if (!m->MnNext) {
                    // After the last item
                    if (left)
                        fon_call();
                    n = new_item();
                    m->MnNext = address(n);
                    n->MnPrev = address(m);
                    goto done;
                }
                next = m->MnNext;
            }
            if (--left < 0)
                fon_call();
            next = m->MnLat;
            if (!next)
                break;
        }
    }
    // A new branch
    if (left)
        fon_call();
    n = new_item();
    if (d.MnBase) {
        m->MnLat = address(n);
        n->MnPrev = address(m);
    } else {
        d.MnBase = address(n);
    }
    flags(n) |= MnFlat;
done:
    flags(n) |= d.MnDFlags[depth - 1];
    n->MnNb = static_cast<uint16_t>(coords[0]);
    r.a[2] = address(n);
    return 0;
}

// MnDel: free item m, taking it out of its branch
static void delete_item(MenuItem *m)
{
    IData &d = idata();
    if (flags(m) & MnFlat) {
        uint32_t next = m->MnNext, previous = m->MnPrev;
        if (!previous)
            d.MnBase = next;
        else
            item(previous)->MnLat = next;
        if (next) {
            flags(item(next)) |= MnFlat;
            item(next)->MnPrev = previous;
        }
    } else {
        if (m->MnNext)
            item(m->MnNext)->MnPrev = m->MnPrev;
        item(m->MnPrev)->MnNext = m->MnNext;
    }
    // The assembler did not free the inactive object (MnOb3)
    for (int i = 0; i < 4; i++)
        if (uint32_t o = object(m, i))
            mem_free(o, object_size(o));
    save_free(m);
    free_data(m);
    mem_free(address(m), MnLong);
}

// MnEff: delete item m and what it opens; with siblings > 0 the items
// after it too
static void erase_items(uint32_t first, uint16_t &siblings)
{
    for (uint32_t i = first;;) {
        MenuItem *m = item(i);
        if (m->MnLat) {
            siblings++;
            erase_items(m->MnLat, siblings);
        }
        i = m->MnNext;
        delete_item(m);
        if (!siblings)
            return;
        if (!i) {
            siblings--;
            return;
        }
    }
}

// MnEff: d0 = item, d5 = 0 for the item alone, else the items after it too
extern "C" uint32_t lib_mn_eff(RegFrame &r)
{
    uint16_t siblings = static_cast<uint16_t>(r.d[5]);
    erase_items(r.d[0], siblings);
    r.d[5] = (r.d[5] & 0xFFFF0000) | siblings;
    return r.d[0];
}

// MnRaz: delete the whole menu
static void menu_erase()
{
    IData &d = idata();
    while (d.MnBase) {
        uint16_t siblings = 1;
        erase_items(d.MnBase, siblings);
    }
    d.MnAdEc = 0;
    d.ActuMask &= ~BitMenu;
}

extern "C" uint32_t lib_mn_raz(RegFrame &)
{
    menu_erase();
    return 0;
}

// The routine records of MnClearVar (menus_glue.s)
extern "C" void lib_menu_clear() { menu_erase(); }
extern "C" void lib_menu_error()
{
    // An error in a menu procedure stops the program
    if (idata().MnProc)
        idata().ErrorOn = -1;
}

// MnClearVar: delete the menu when the program is cleared
static void menu_clear_var()
{
    IData &d = idata();
    add_routine(lib_menu_on_clear, address(&d.Sys_ClearRoutines));
    add_routine(lib_menu_on_default, address(&d.Sys_DefaultRoutines));
    add_routine(lib_menu_on_error, address(&d.Sys_ErrorRoutines));
    d.GoTest_Menus = address(lib_address(L_MnGere));
    d.GoTest_MenuKey = address(lib_address(L_MenuKeyExplore));
}

extern "C" uint32_t lib_mn_clear_var(RegFrame &)
{
    menu_clear_var();
    return 0;
}

// MenuReset: the default flags and no choice
extern "C" uint32_t lib_menu_reset(RegFrame &)
{
    IData &d = idata();
    d.MnMouse = 0;
    d.MnDFlags[0] = MnTotal | MnTBouge;
    for (int i = 1; i < MnNDim; i++)
        d.MnDFlags[i] = MnBar | MnTBouge;
    for (int i = 0; i < MnNDim; i++)
        d.MnChoix[i] = 0;
    d.ActuMask &= ~BitMenu;
    return 0;
}

// ---- MnObjet: compile a menu object ---------------------------------------

namespace {

// The commands, their two letter names and number of parameters (-1: a
// procedure name)
struct ObjectCommand {
    char name[2];
    int8_t params;
};
constexpr ObjectCommand object_commands[] = {
    {{'B', 'A'}, 2}, {{'L', 'I'}, 2}, {{'E', 'L'}, 2}, {{'P', 'A'}, 1}, {{'I', 'N'}, 2},
    {{'B', 'O'}, 1}, {{'I', 'C'}, 1}, {{'L', 'O'}, 2}, {{'O', 'U'}, 1}, {{'S', 'L'}, 1},
    {{'S', 'F'}, 1}, {{'P', 'R'}, -1}, {{'R', 'E'}, 1}, {{'S', 'S'}, 1},
};

uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? static_cast<uint8_t>(c - 0x20) : c; }

// The object syntax: text, and commands in brackets, "(BA 10,5:IN 1,2)".
// The reading follows the assembler, which counted the characters of a
// text before skipping the control codes in it.
struct ObjectCompiler {
    const uint8_t *s;
    int16_t left;
    uint8_t *out;
    uint8_t c = 0;          // the last character read (d0)
    bool failed = false;

    // MnOChr / MnOChS: the next character (from 32 on, from 33 on to skip
    // the spaces); false at the end or before a "("
    bool get(uint8_t from)
    {
        if (failed) {
            c = 0;
            return false;
        }
        for (;;) {
            if (!left)
                return false;
            c = *s++;
            left--;
            if (c == '(') {
                s--;
                left++;
                c = 0;
                return false;
            }
            if (c == 27) {
                s += 2;
                left = static_cast<int16_t>(left - 2);
                if (left < 0) {
                    failed = true;
                    return false;
                }
                continue;
            }
            if (c >= from)
                return true;
        }
    }
    bool chr() { return get(32); }
    bool chs() { return get(33); }
    void unget()
    {
        s--;
        left++;
    }
    void word(uint16_t w)
    {
        poke_w(out, w);
        out += 2;
    }

    // MnOLong: a decimal number, maybe negative. c holds the digit
    // values, as d0 did: at the end of the string it is not changed.
    int32_t number()
    {
        bool minus = false;
        c = 0;
        chs();
        if (c == '-') {
            minus = true;
            chs();
        }
        bool digit = c >= '0' && c - '0' < 10;
        c = static_cast<uint8_t>(c - '0');
        if (!digit) {
            failed = true;
            return 0;
        }
        int32_t n = c;
        for (;;) {
            chs();
            digit = c >= '0' && c - '0' < 10;
            c = static_cast<uint8_t>(c - '0');
            if (!digit)
                break;
            n = n * 10 + c;
        }
        unget();
        return minus ? -n : n;
    }

    // MnOTok: one command
    void command()
    {
        if (!chs())
            return void(failed = true);
        uint16_t name = static_cast<uint16_t>(upper(c) << 8);
        if (!chr())
            return void(failed = true);
        name |= upper(c);
        // The rest of the name
        do {
            if (!chr())
                return void(failed = true);
            c = upper(c);
        } while (c >= 'A' && c <= 'Z');
        unget();
        uint16_t token = 8;
        const ObjectCommand *command = nullptr;
        for (const ObjectCommand &o : object_commands) {
            if ((o.name[0] << 8 | o.name[1]) == name) {
                command = &o;
                break;
            }
            token = static_cast<uint16_t>(token + 4);
        }
        if (!command)
            return void(failed = true);
        word(token);
        if (command->params < 0) {
            // A procedure: its name as an AMOS string
            uint8_t *length = out;
            word(0);
            uint16_t n = 0;
            for (;;) {
                if (!chs())
                    break;
                if (c == ')' || c == ':') {
                    unget();
                    break;
                }
                *out++ = c;
                n++;
            }
            if (!n)
                return void(failed = true);
            if (n & 1) {
                *out++ = 0;
                n++;
            }
            poke_w(length, n);
            return;
        }
        for (int8_t i = command->params;;) {
            word(static_cast<uint16_t>(number()));
            if (failed || !--i)
                return;
            if (!chs() || c != ',')
                return void(failed = true);
        }
    }

    // Returns the end of the compiled object, or null on a syntax error
    uint8_t *compile()
    {
        for (;;) {
            if (address(out) & 1)
                *out++ = 0;
            // Text up to the next "("
            const uint8_t *start = s;
            int16_t count = left;
            while (chr()) {
            }
            if (failed)
                return nullptr;
            if (uint16_t n = static_cast<uint16_t>(count - left)) {
                word(ObText);
                word(n);
                s = start;
                left = count;
                while (chr())
                    *out++ = c;
                if (failed)
                    return nullptr;
                continue;
            }
            if (!left)
                break;
            // Commands between brackets
            left--;
            s++;
            for (;;) {
                command();
                if (failed || !chs())
                    return nullptr;
                if (c == ')')
                    break;
                if (c != ':')
                    return nullptr;
            }
        }
        word(ObEnd);
        return out;
    }
};

} // namespace

// MnObjet: a1 = the AMOS string of an object. Out: d0 = the compiled
// object (its size first), 0 out of memory (the assembler returned the
// length instead), -1 syntax error.
extern "C" uint32_t lib_mn_objet(RegFrame &r)
{
    const uint8_t *s = ptr<uint8_t>(r.a[1]);
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    ObjectCompiler oc{s + 2, static_cast<int16_t>(peek_w(s)), buffer};
    uint8_t *end = oc.compile();
    if (!end)
        return r.d[0] = -1u;
    uint16_t size = static_cast<uint16_t>(end - buffer + 2);
    uint8_t *object = mem_fast(size, true);
    if (object) {
        poke_w(object, size);
        for (uint16_t i = 0; i < size - 2; i++)
            object[2 + i] = buffer[i];
    }
    return r.d[0] = address(object);
}

// ---- Menu Key --------------------------------------------------------------

// MnKE0: look for the item with the key in branch first; path holds the
// numbers of the items above it
static bool key_search(uint32_t first, uint16_t *path, int depth, uint8_t ascii, uint8_t scan, uint8_t shift)
{
    for (uint32_t i = first; i; i = item(i)->MnNext) {
        MenuItem *m = item(i);
        if (m->MnLat) {
            if (depth < MnNDim - 1) {
                path[depth] = m->MnNb;
                if (key_search(m->MnLat, path, depth + 1, ascii, scan, shift))
                    return true;
            }
            continue;
        }
        if (!m->MnKFlag)
            continue;
        if (static_cast<int8_t>(m->MnKFlag) > 0 ? m->MnKAsc == ascii
                                                : m->MnKSc == scan && m->MnKSh == shift) {
            IData &d = idata();
            for (int k = 0; k < MnNDim; k++)
                d.MnChoix[k] = 0;
            for (int k = 0; k < depth; k++)
                d.MnChoix[k] = path[k];
            d.MnChoix[depth] = m->MnNb;
            d.MnChoice = -1;
            wdata().Actualise |= BitJump;
            return true;
        }
    }
    return false;
}

// MenuKeyExplore: the last key (ClLast) chooses the item with that Menu Key
extern "C" uint32_t lib_menu_key_explore(RegFrame &)
{
    WData &w = wdata();
    auto key = reinterpret_cast<const uint8_t *>(&w.ClLast);
    uint16_t path[MnNDim];
    key_search(idata().MnBase, path, 0, key[3], key[1], key[0]);
    w.ClLast = 0;
    return 0;
}

// ---- instructions ----------------------------------------------------------

// ON MENU ON / ON MENU OFF (both turn the branches on, as in the assembler)
extern "C" void lib_in_on_menu_on()
{
    if (idata().OMnNb)
        idata().ActuMask |= BitJump;
}

// ON MENU DEL
extern "C" void lib_in_on_menu_del()
{
    RegFrame r = {};
    lib_omn_eff(r);
}

namespace {

// MnLg: the size of the tree from item first in a bank
uint32_t tree_size(uint32_t first)
{
    uint32_t size = 0;
    for (uint32_t i = first; i; i = item(i)->MnNext) {
        MenuItem *m = item(i);
        size += MnLong;
        for (int o = 0; o < 4; o++)
            if (object(m, o))
                size += object_size(object(m, o));
        if (m->MnLat)
            size += tree_size(m->MnLat);
    }
    return size;
}

// MnTb: copy the tree from item first to out; the links become offsets
// from the start of the bank
uint8_t *tree_to_bank(uint32_t first, uint8_t *out, uint8_t *bank)
{
    for (uint32_t i = first; i;) {
        MenuItem *m = item(i);
        MenuItem *copy = reinterpret_cast<MenuItem *>(out);
        *copy = *m;
        out += MnLong;
        for (int o = 0; o < 4; o++)
            if (uint32_t from = object(copy, o)) {
                object(copy, o) = static_cast<uint32_t>(out - bank);
                uint16_t size = object_size(from);
                for (uint16_t k = 0; k < size; k++)
                    *out++ = ptr<uint8_t>(from)[k];
            }
        if (m->MnLat) {
            copy->MnLat = static_cast<uint32_t>(out - bank);
            out = tree_to_bank(m->MnLat, out, bank);
        }
        i = m->MnNext;
        if (i)
            copy->MnNext = static_cast<uint32_t>(out - bank);
    }
    return out;
}

// MnTMn: rebuild item m and what follows it from item from of the bank
void tree_from_bank(MenuItem *m, MenuItem *from, uint8_t *bank)
{
    for (;;) {
        *m = *from;
        // Left over from the menu the bank was made from (the assembler
        // freed them later)
        m->MnAdSave = 0;
        m->MnDatas = 0;
        m->MnLData = 0;
        for (int o = 0; o < 4; o++)
            if (uint32_t offset = object(m, o)) {
                object(m, o) = 0;
                const uint8_t *source = bank + offset;
                uint16_t size = peek_w(source);
                uint8_t *copy = mem_fast(size);
                if (!copy)
                    out_of_memory();
                object(m, o) = address(copy);
                for (uint16_t k = 0; k < size; k++)
                    copy[k] = source[k];
            }
        if (uint32_t offset = m->MnLat) {
            m->MnLat = 0;
            MenuItem *source = reinterpret_cast<MenuItem *>(bank + offset);
            source->MnPrev = address(m);
            MenuItem *lateral = reinterpret_cast<MenuItem *>(mem_fast(MnLong));
            if (!lateral)
                out_of_memory();
            m->MnLat = address(lateral);
            tree_from_bank(lateral, source, bank);
        }
        uint32_t offset = m->MnNext;
        if (!offset)
            return;
        m->MnNext = 0;
        MenuItem *source = reinterpret_cast<MenuItem *>(bank + offset);
        MenuItem *next = reinterpret_cast<MenuItem *>(mem_fast(MnLong));
        if (!next)
            out_of_memory();
        source->MnPrev = address(m);
        m->MnNext = address(next);
        m = next;
        from = source;
    }
}

} // namespace

// MENU TO BANK n
extern "C" void lib_in_menu_to_bank(uint32_t n)
{
    uint32_t base = idata().MnBase;
    if (!base)
        fon_call();
    uint32_t size = tree_size(base);
    uint8_t *bank = bank_reserve(n, BankData, size, "Menu    ");
    if (!bank)
        out_of_memory();
    bank_changed();
    tree_to_bank(base, bank, bank);
}

// BANK TO MENU n
extern "C" void lib_in_bank_to_menu(uint32_t n)
{
    IData &d = idata();
    menu_erase();
    menu_clear_var();
    if (!d.ScOn)
        screen_not_open();
    uint8_t *bank = bank_find(n);
    if (!bank)
        lib_jump(L_BkNoRes, 0);
    if (peek_l(bank - 8) != 0x4D656E75)     // "Menu"
        fon_call();
    MenuItem *base = reinterpret_cast<MenuItem *>(mem_fast(MnLong));
    if (!base)
        out_of_memory();
    d.MnBase = address(base);
    tree_from_bank(base, reinterpret_cast<MenuItem *>(bank), bank);
    d.MnAdEc = d.ScOnAd;
    d.MnChange++;
}

// MENU ON / MENU OFF
extern "C" void lib_in_menu_on()
{
    if (idata().MnBase) {
        idata().ActuMask |= BitMenu;
        wdata().ClLast = 0;
    }
}
extern "C" void lib_in_menu_off() { idata().ActuMask &= ~BitMenu; }

// MENU MOUSE ON / OFF
extern "C" void lib_in_menu_mouse_on() { idata().MnMouse = 1; }
extern "C" void lib_in_menu_mouse_off() { idata().MnMouse = 0; }

// MENU BASE x,y
extern "C" void lib_in_menu_base(uint32_t x, uint32_t y)
{
    if (!idata().MnBase)
        menu_not_opened();
    MenuItem *m = item(idata().MnBase);
    if (y != EntNul) {
        m->MnY = static_cast<uint16_t>(y);
        flags(m) |= MnFixed;
    }
    if (x != EntNul) {
        m->MnX = static_cast<uint16_t>(x);
        flags(m) |= MnFixed;
    }
}

// =CHOICE, =CHOICE(n)
extern "C" uint32_t lib_fn_choice0()
{
    int16_t choice = static_cast<int16_t>(idata().MnChoice);
    idata().MnChoice = 0;
    return static_cast<uint32_t>(static_cast<int32_t>(choice));
}

extern "C" uint32_t lib_fn_choice1(uint32_t n)
{
    if (!n || n > MnNDim)
        fon_call();
    return idata().MnChoix[n - 1];
}

namespace {

// MnDim: read menu coordinates, "(1,2,...)", or a level alone from the
// program. Gives the flags of the item, or the default flags of the level
// (and then no item).
struct Dim {
    uint8_t *flags;
    MenuItem *item;
};

Dim dim()
{
    Regs r = {};
    lib_eval(L_MnDim, r);
    return {ptr<uint8_t>(r.a[0]), ptr<MenuItem>(r.a[2])};
}

} // namespace

// =MENU X(,,,), =MENU Y(,,,)
extern "C" uint32_t lib_fn_xmenu()
{
    MenuItem *m = dim().item;
    return m ? m->MnX : 0;
}
extern "C" uint32_t lib_fn_ymenu()
{
    MenuItem *m = dim().item;
    return m ? m->MnY : 0;
}

extern "C" void lib_in_menu_bar() { *dim().flags |= MnBar; }
extern "C" void lib_in_menu_line() { *dim().flags &= ~(MnBar | MnTotal); }
extern "C" void lib_in_menu_tline()
{
    uint8_t *f = dim().flags;
    *f = static_cast<uint8_t>((*f & ~MnBar) | MnTotal);
}
extern "C" void lib_in_menu_movable() { *dim().flags |= MnTBouge; }
extern "C" void lib_in_menu_static() { *dim().flags &= ~MnTBouge; }
extern "C" void lib_in_menu_item_movable() { *dim().flags |= MnBouge; }
extern "C" void lib_in_menu_item_static() { *dim().flags &= ~MnBouge; }
extern "C" void lib_in_menu_active() { *dim().flags &= ~MnOff; }
extern "C" void lib_in_menu_inactive() { *dim().flags |= MnOff; }
extern "C" void lib_in_menu_separate() { *dim().flags |= MnSep; }
extern "C" void lib_in_menu_link() { *dim().flags &= ~MnSep; }
// For a level alone these wrote at whatever a2 held
extern "C" void lib_in_menu_called()
{
    if (MenuItem *m = dim().item)
        called(m) = 0xFF;
}
extern "C" void lib_in_menu_once()
{
    if (MenuItem *m = dim().item)
        called(m) = 0;
}

// MENU CALC
extern "C" void lib_in_menu_calc()
{
    IData &d = idata();
    if (!d.MnBase)
        menu_not_opened();
    start();
    // The assembler ran (PR) objects with the registers of the last MnGere
    Ctx regs = ctx();
    d.MnRA3 = address(&regs);
    d.MnProc = -1;
    ec_fn(ec::MnStart);
    calc();
    end1();
}
