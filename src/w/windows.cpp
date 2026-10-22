// The text windows of AMOS.library (+W.s, "FENETRES"): the WiCall
// functions (w/windows_glue.s), the character output with its control
// codes and escape sequences, borders, and the stacking of windows over
// each other with the saved backgrounds.
//
// A window lives in the list of its screen, the current one first
// (EcWindow). Its position: WiX counts the columns left to the right edge
// (WiTx at the left), WiY is the line; WiAdCur is the offset of the cursor
// in a bitplane. A window has an outer size (R: with the border) and an
// inner one (I); WiTx/WiTy/WiAdhg are those in use, normally the inner.
//
// Register fields keep their assembler bit layout: the high byte of WiSys
// holds the scroll/cursor/inverse flags, its low byte the planes that are
// not written to (bit NPlan-p for plane p).

#include "core/amiga.h"
#include "w/w.h"

#define GRAPHICS_BASE_NAME (reinterpret_cast<struct GfxBase *>(amos::wdata().GfxBase))
#include <proto/graphics.h>

using namespace amos;
using namespace amos::w;

namespace {

// WiSys (high byte)
constexpr uint16_t SysScroll = 1 << 8, SysCursor = 1 << 9, SysInverse = 1 << 10;
// WiFlags: shade, underline, a writing mode other than the normal one
constexpr uint16_t FlagShade = 1 << 1, FlagUnder = 1 << 2, FlagWriting = 1 << 15;

// Errors of the window functions
enum : uint32_t {
    ErrMemory = 1, ErrNotOpen = 10, ErrOpen = 11, ErrSize = 12, ErrTooBig = 13, ErrFont = 14, ErrParam = 16,
    ErrWindow0 = 18, ErrNoBorder = 19,
};

// Writing: how a character with flags is combined with the screen, and
// which of paper and pen it draws. The assembler patched these into its
// code, so they are the same for all windows; Wind Open resets them.
enum : uint8_t { WriteReplace, WriteOr, WriteXor, WriteAnd, WriteNone };
enum : uint8_t { DrawBoth, DrawPaper, DrawPen };
uint8_t write_mode = WriteXor;
uint8_t draw_mode = DrawBoth;

// The fast way of drawing a plane, WiColor (the assembler kept the address
// of the code there)
enum : uint32_t { PlaneZero, PlaneNormal, PlaneInverse, PlaneOne, PlaneSkip };

// Borders 1-16 (Brd): eight strings, the top left corner, top right
// corner, top, right side, bottom left, bottom right, bottom, left side
const uint8_t border0[] = {136, 0, 138, 0, 137, 0, 139, 0, 140, 0, 141, 0, 137, 0, 139, 0};
const uint8_t border1[] = {128, 0, 130, 0, 129, 0, 132, 0, 133, 0, 135, 0, 134, 0, 131, 0};
const uint8_t border2[] = {157, 0, 2, 0, 1, 0, 3, 0, 6, 0, 4, 0, 5, 0, 7, 0};
const uint8_t border3[] = {8, 0, 10, 0, 9, 0, 11, 0, 14, 0, 12, 0, 13, 0, 15, 0};
const uint8_t border4[] = {16, 0, 18, 0, 17, 0, 19, 0, 22, 0, 20, 0, 21, 0, 23, 0};
const uint8_t border5[] = {24, 0, 26, 0, 25, 0, 158, 0, 30, 0, 28, 0, 29, 0, 31, 0};
const uint8_t border15[] = {' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0, ' ', 0};
const uint8_t *const borders[16] = {
    border0, border1, border2, border3, border4, border5, border0, border0,
    border0, border0, border0, border0, border0, border0, border0, border15,
};

// ESC E frames (TEncadre): top left, top, top right, right, bottom right,
// bottom, bottom left, left. Frame 0 (and 8) is blank.
const uint8_t frames[8][8] = {
    {32, 32, 32, 32, 32, 32, 32, 32},
    {136, 137, 138, 139, 141, 137, 140, 139},
    {128, 129, 130, 132, 135, 134, 133, 131},
    {157, 1, 2, 3, 4, 5, 6, 7},
    {8, 9, 10, 11, 12, 13, 14, 15},
    {16, 17, 18, 19, 20, 21, 22, 23},
    {24, 25, 26, 158, 28, 29, 30, 31},
    {32, 32, 32, 32, 32, 32, 32, 32},
};

// The text cursor of a new window (DefCurs)
const uint8_t default_cursor[8] = {0, 0, 0, 0, 0, 0, 0xFF, 0xFF};

// TAbk1-4: the autoback steps around a print. Returns their d0.
uint32_t autoback(uint8_t *step) { return call(step); }

// Compte: the number of characters a string prints (an escape sequence
// is three bytes, none printed), and the end, after its zero
struct Count {
    uint16_t n;
    const uint8_t *end;
};

Count count(const uint8_t *s)
{
    uint16_t n = 0;
    while (*s) {
        n++;
        if (*s++ == 27) {
            n--;
            s += 2;
        }
    }
    return {n, s + 1};
}

Window *find(Screen *s, uint16_t number)
{
    for (Window *w = ptr<Window>(s->EcWindow); w; w = ptr<Window>(w->WiNext))
        if (w->WiNumber == number)
            return w;
    return nullptr;
}

// A window of a screen, what the assembler kept in a5/a4
struct Win {
    Screen *s;
    Window *w;

    uint8_t *plane(int p) const { return ptr<uint8_t>(s->EcCurrent[p]); }
    int32_t line() const { return static_cast<int16_t>(s->EcTLigne); }
    // btst plane,WiSys+1: the plane counted from the last one
    bool masked(int bit) const { return w->WiSys >> bit & 1; }
    uint16_t column() const { return w->WiTx - w->WiX; }
    uint8_t paper_byte(int p) const { return w->WiColFl[p] >> 24; }
    uint16_t paper_word(int p) const { return w->WiColFl[p] >> 16; }

    // ---- positions --------------------------------------------------------

    // AdCurs
    void address_cursor()
    {
        w->WiAdCur = uint32_t(w->WiY) * w->WiTLigne + int32_t(int16_t(column())) + w->WiAdhg;
    }

    // WiInt / WiExt: the inner or the outer size
    void inner()
    {
        w->WiTx = w->WiTxI;
        w->WiTy = w->WiTyI;
        w->WiAdhg = w->WiAdhgI;
    }

    void outer()
    {
        w->WiTx = w->WiTxR;
        w->WiTy = w->WiTyR;
        w->WiAdhg = w->WiAdhgR;
    }

    // Loca: the cursor to x,y
    uint32_t locate(uint16_t x, uint16_t y)
    {
        if (y >= w->WiTy || x >= w->WiTx)
            return ErrParam;
        w->WiX = w->WiTx - x;
        w->WiY = y;
        w->WiAdCur = uint32_t(y) * w->WiTLigne + int32_t(int16_t(x)) + w->WiAdhg;
        return 0;
    }

    uint32_t locate_x(uint16_t x) { return locate(x, w->WiY); }
    uint32_t locate_y(uint16_t y) { return locate(column(), y); }

    uint32_t home()
    {
        w->WiX = w->WiTx;
        w->WiY = 0;
        address_cursor();
        return 0;
    }

    // WiAdr: the addresses for a window at byte x, line y, of tx bytes
    // and ty lines
    uint32_t set_size(uint16_t x, uint16_t y, uint16_t tx, uint16_t ty)
    {
        w->WiDxR = x;
        w->WiDxI = x;
        w->WiDyI = y;
        tx &= 0xFFFE;
        if (!tx)
            return ErrSize;
        uint16_t right = x + tx;
        if (right > s->EcTLigne)
            return ErrTooBig;
        w->WiFxR = right;
        uint16_t height = w->WiTyCar;
        w->WiTLigne = s->EcTLigne * height;
        if (!ty)
            return ErrSize;
        uint16_t pixels = ty * height;
        w->WiTyP = pixels;
        w->WiDyR = y;
        uint16_t bottom = pixels + y;
        w->WiFyR = bottom;
        if (uint32_t(bottom) * s->EcTLigne > s->EcTPlan)
            return ErrTooBig;
        // (The assembler added x to the low word only: wrong addresses on
        // screens of more than 64 KB a bitplane)
        uint32_t start = uint32_t(y) * s->EcTLigne + x;
        w->WiAdhgR = start;
        w->WiTxR = tx;
        w->WiTyR = ty;
        if (w->WiBord) {
            w->WiDxI++;
            w->WiDyI += height;
            tx -= 2;
            if (int16_t(tx) <= 0)
                return ErrSize;
            ty -= 2;
            if (int16_t(ty) <= 0)
                return ErrSize;
            start += uint32_t(height) * s->EcTLigne + 1;
        }
        w->WiAdhgI = start;
        w->WiTxI = tx;
        w->WiTyI = ty;
        return 0;
    }

    // ---- colours ----------------------------------------------------------

    // AdColor: how each plane is drawn (WiColor), the paper and pen masks
    // (WiColFl). The bits of the colours only move on for planes that are
    // written to, the masks for all.
    void colours()
    {
        uint16_t paper = w->WiPaper, pen = w->WiPen;
        uint16_t paper_mask = paper, pen_mask = pen;
        for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
            uint32_t how = PlaneSkip;
            if (!masked(bit)) {
                how = (paper & 1) << 1 | (pen & 1);
                paper >>= 1;
                pen >>= 1;
            }
            w->WiColor[p] = how;
            w->WiColFl[p] = (paper_mask & 1 ? 0xFFFF0000 : 0) | (pen_mask & 1 ? 0xFFFF : 0);
            paper_mask >>= 1;
            pen_mask >>= 1;
        }
    }

    // ---- the cursor -------------------------------------------------------

    // AffCur: draw it, keeping what is under it in EcCurS
    void cursor_on()
    {
        if (!(w->WiSys & SysCursor))
            return;
        uint8_t *save = s->EcCurS;
        auto shape = reinterpret_cast<const uint8_t *>(w->WiCuDraw);
        uint16_t colour = w->WiCuCol;
        for (int p = 0; p <= w->WiNPlan; p++, colour >>= 1) {
            uint8_t *a = plane(p) + w->WiAdCur;
            for (int y = 0; y < 8; y++, a += line()) {
                *save++ = *a;
                if (colour & 1)
                    *a |= shape[y];
                else
                    *a &= ~shape[y];
            }
        }
    }

    // EffCur
    void cursor_off()
    {
        if (!(w->WiSys & SysCursor))
            return;
        const uint8_t *save = s->EcCurS;
        for (int p = 0; p <= w->WiNPlan; p++) {
            uint8_t *a = plane(p) + w->WiAdCur;
            for (int y = 0; y < 8; y++, a += line())
                *a = *save++;
        }
    }

    // ---- blitter: clearing and scrolling ---------------------------------

    // A blit of n bytes as one word column: bytes/2 lines of one word
    static uint16_t blit_size(uint16_t bytes) { return uint16_t(uint16_t(bytes >> 1) << 6) | 1; }

    static void own()
    {
        OwnBlitter();
        WaitBlit();
    }

    // ClFin: clear rows lines of width bytes from offset, in the paper
    uint32_t clear(uint32_t offset, uint16_t rows, uint16_t width)
    {
        uint16_t size = blit_size(width);
        auto &c = chips();
        own();
        c.bltcon0 = 0x01AA;
        c.bltcon1 = 0;
        c.bltdmod = 0;
        c.dmacon = 0x8040;
        uint16_t n = rows - 1;
        do {
            for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
                if (masked(bit))
                    continue;
                WaitBlit();
                c.bltdpt = plane(p) + offset;
                c.bltcdat = paper_word(p);
                c.bltsize = size;
            }
            offset += line();
        } while (n-- != 0);
        WaitBlit();
        DisownBlitter();
        return 0;
    }

    // Scrolle: copy rows lines from offset from to offset to, delta apart
    void copy_lines(uint32_t from, uint32_t to, int32_t delta, uint16_t rows)
    {
        uint16_t n = rows - 1;
        if (int16_t(n) < 0)
            return;
        uint16_t size = blit_size(w->WiTxI);
        auto &c = chips();
        own();
        c.bltcon0 = 0x03AA;
        c.bltcon1 = 0;
        c.bltcmod = 0;
        c.bltdmod = 0;
        c.dmacon = 0x8040;
        do {
            for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
                if (masked(bit))
                    continue;
                WaitBlit();
                c.bltcpt = plane(p) + from;
                c.bltdpt = plane(p) + to;
                c.bltsize = size;
            }
            from += delta;
            to += delta;
        } while (n-- != 0);
        WaitBlit();
        DisownBlitter();
    }

    // ScGFin: rows lines from offset one character to the left. The
    // blitter shifts by a byte; the first byte is the one that comes in
    // from the left of the source, the last becomes paper.
    uint32_t shift_left(uint32_t offset, uint16_t rows)
    {
        uint16_t size = blit_size(w->WiTxI);
        int16_t last = w->WiTxI - 1;
        auto &c = chips();
        own();
        c.bltcon0 = 0x05CC;
        c.bltcon1 = 0x8000;
        c.bltbmod = 0;
        c.bltdmod = 0;
        c.dmacon = 0x8040;
        uint16_t n = rows - 1;
        do {
            for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
                if (masked(bit))
                    continue;
                uint8_t *a = plane(p) + offset;
                c.bltdpt = a;
                uint8_t first = a[1];
                c.bltbpt = a + 2;
                c.bltsize = size;
                WaitBlit();
                a[0] = first;
                a[last] = paper_byte(p);
            }
            offset += line();
        } while (n-- != 0);
        DisownBlitter();
        return 0;
    }

    // ScDFin: one character to the right, paper at the left
    uint32_t shift_right(uint32_t offset, uint16_t rows)
    {
        uint16_t size = blit_size(w->WiTxI);
        auto &c = chips();
        own();
        c.bltcon0 = 0x05CC;
        c.bltcon1 = 0x8000;
        c.bltbmod = 0;
        c.bltdmod = 0;
        c.dmacon = 0x8040;
        uint16_t n = rows - 1;
        do {
            for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
                if (masked(bit))
                    continue;
                uint8_t *a = plane(p) + offset;
                c.bltbpt = a;
                c.bltdpt = a;
                c.bltsize = size;
                WaitBlit();
                a[0] = paper_byte(p);
            }
            offset += line();
        } while (n-- != 0);
        DisownBlitter();
        return 0;
    }

    uint32_t cursor_line() const { return uint32_t(w->WiY) * w->WiTLigne + w->WiAdhgI; }
    uint16_t window_rows() const { return w->WiTyCar * w->WiTyI; }

    // ---- control codes ----------------------------------------------------

    uint32_t clw()          // 25, Clw
    {
        clear(w->WiAdhgI, window_rows(), w->WiTxI);
        return home();
    }

    uint32_t clear_line() { return clear(cursor_line(), w->WiTyCar, w->WiTxI); }       // 26, ClLine
    uint32_t left_line() { return shift_left(cursor_line(), w->WiTyCar); }             // 16, ScGLine
    uint32_t left_window() { return shift_left(w->WiAdhgI, window_rows()); }           // 17, ScGWi
    uint32_t right_line() { return shift_right(cursor_line(), w->WiTyCar); }           // 18, ScDLine
    uint32_t right_window() { return shift_right(w->WiAdhgI, window_rows()); }         // 19, ScDWi

    // 22, ScHaut: the lines above the cursor up, clear the cursor line
    uint32_t scroll_up()
    {
        copy_lines(w->WiAdhgI + int16_t(w->WiTLigne), w->WiAdhgI, line(), w->WiY * w->WiTyCar);
        return clear_line();
    }

    // 20, ScBas: the lines below the cursor down, clear the cursor line
    uint32_t scroll_down()
    {
        uint32_t to = uint32_t(w->WiTyI) * w->WiTLigne - line() + w->WiAdhgI;
        copy_lines(to - int16_t(w->WiTLigne), to, -line(), uint16_t(w->WiTyI - w->WiY - 1) * w->WiTyCar);
        return clear_line();
    }

    // 23, ScHautBas: the lines below the cursor up, clear the last line
    uint32_t scroll_up_below()
    {
        uint32_t to = cursor_line();
        copy_lines(to + int16_t(w->WiTLigne), to, line(), uint16_t(w->WiTyI - w->WiY - 1) * w->WiTyCar);
        return clear(uint32_t(uint16_t(w->WiTyI - 1)) * w->WiTLigne + w->WiAdhgI, w->WiTyCar, w->WiTxI);
    }

    // 21, ScBasHaut: the lines above the cursor down, clear the first line
    uint32_t scroll_down_above()
    {
        uint32_t to = uint32_t(uint16_t(w->WiY + 1)) * w->WiTLigne - line() + w->WiAdhgI;
        copy_lines(to - int16_t(w->WiTLigne), to, -line(), w->WiY * w->WiTyCar);
        return clear(w->WiAdhgI, w->WiTyCar, w->WiTxI);
    }

    // 8 and 29, CLeft
    uint32_t left()
    {
        uint16_t x = w->WiX + 1;
        if (x > w->WiTx) {
            w->WiX = 1;
            return up();
        }
        w->WiX = x;
        address_cursor();
        return 0;
    }

    // 28, CRight
    uint32_t right()
    {
        if (--w->WiX == 0) {
            w->WiX = w->WiTx;
            return down();
        }
        address_cursor();
        return 0;
    }

    // 30, CUp: scrolls at the top if scrolling is on, else wraps
    uint32_t up()
    {
        if (int16_t(--w->WiY) >= 0) {
            address_cursor();
            return 0;
        }
        if (w->WiSys & SysScroll) {
            w->WiY = 0;
            address_cursor();
            return scroll_down();
        }
        w->WiY = w->WiTy - 1;
        address_cursor();
        return 0;
    }

    // 10 and 31, CDown
    uint32_t down()
    {
        uint16_t y = w->WiY + 1;
        if (y >= w->WiTy) {
            if (w->WiSys & SysScroll)
                return scroll_up();
            y = 0;
        }
        w->WiY = y;
        address_cursor();
        return 0;
    }

    // 13, CReturn
    uint32_t carriage_return()
    {
        w->WiX = w->WiTx;
        address_cursor();
        return 0;
    }

    // 9, Tab: to the next multiple of WiTab, if there is one in the line
    uint32_t tab()
    {
        uint16_t x = column();
        uint16_t stop = w->WiTab;
        if (stop) {
            while (stop <= x)
                stop += w->WiTab;
            if (stop < w->WiTx)
                locate(stop, w->WiY);
        }
        return 0;
    }

    // ESC Q n, RazCur: clear n characters from the cursor
    uint32_t clear_chars(uint16_t n)
    {
        if (n >= w->WiX)
            n = w->WiX;
        if (int16_t(--n) < 0)
            return 0;
        uint32_t offset = w->WiAdCur;
        do {
            for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
                if (masked(bit))
                    continue;
                uint8_t paper = uint8_t(paper_word(p));
                uint8_t *a = plane(p) + offset;
                for (int y = 0; y < w->WiTyCar; y++, a += line())
                    *a = paper;
            }
            offset++;
        } while (n-- != 0);
        return 0;
    }

    // 7, ClEol: to the end of the line, the blitter from an even address
    uint32_t clear_to_end()
    {
        uint16_t n = w->WiX;
        uint32_t offset = w->WiAdCur;
        if (offset & 1) {
            clear_chars(1);
            offset++;
            n--;
        }
        if (int16_t(n) > 0)
            clear(offset, w->WiTyCar, n);
        return 0;
    }

    uint32_t control(uint8_t c, uint32_t d0)
    {
        switch (c) {
        case 7: return clear_to_end();
        case 8: return left();
        case 9: return tab();
        case 10: return down();
        case 12: return home();
        case 13: return carriage_return();
        case 16: return left_line();
        case 17: return left_window();
        case 18: return right_line();
        case 19: return right_window();
        case 20: return scroll_down();
        case 21: return scroll_down_above();
        case 22: return scroll_up();
        case 23: return scroll_up_below();
        case 24: return home();
        case 25: return clw();
        case 26: return clear_line();
        case 27: w->WiEsc = 2; return 0;
        case 28: return right();
        case 29: return left();
        case 30: return up();
        case 31: return down();
        default: return d0;         // Rien: d0 as it came
        }
    }

    // ---- escape sequences -------------------------------------------------

    // ESC W: writing mode (bits 0-2), paper/pen only (bits 3-4)
    uint32_t writing(uint16_t mode)
    {
        uint16_t combine = mode & 7;
        if (combine < 5) {
            w->WiFlags &= ~FlagWriting;
            if (combine)
                w->WiFlags |= FlagWriting;
            write_mode = combine;
        }
        uint16_t draw = mode >> 3 & 3;
        if (draw < 3) {
            if (draw)
                w->WiFlags |= FlagWriting;
            draw_mode = draw;
        }
        return 0;
    }

    uint32_t check_colour(uint16_t colour) const { return colour >= s->EcNbCol ? uint32_t(ErrParam) : 0; }

    // ESC B (Paper) and ESC P (Pen): inverse is undone first
    uint32_t paper(uint16_t colour)
    {
        if (check_colour(colour))
            return ErrParam;
        if (w->WiSys & SysInverse) {
            w->WiSys &= ~SysInverse;
            w->WiPen = w->WiPaper;
        }
        w->WiPaper = colour;
        colours();
        return 0;
    }

    uint32_t pen(uint16_t colour)
    {
        if (check_colour(colour))
            return ErrParam;
        if (w->WiSys & SysInverse) {
            w->WiSys &= ~SysInverse;
            w->WiPaper = w->WiPen;
        }
        w->WiPen = colour;
        colours();
        return 0;
    }

    // ESC I: inverse swaps paper and pen
    uint32_t inverse(uint16_t on)
    {
        bool was = w->WiSys & SysInverse;
        if (on)
            w->WiSys |= SysInverse;
        else
            w->WiSys &= ~SysInverse;
        if (bool(on) != was) {
            uint16_t paper = w->WiPaper;
            w->WiPaper = w->WiPen;
            w->WiPen = paper;
            colours();
        }
        return 0;
    }

    // ESC J: the planes written to (bit p for plane p)
    uint32_t planes(uint32_t which)
    {
        uint8_t off = 0;
        for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--)
            if (!(which >> (p & 31) & 1))
                off |= 1 << bit;
        w->WiSys = (w->WiSys & 0xFF00) | off;
        colours();
        return 0;
    }

    uint32_t flag(uint16_t &field, uint16_t bit, uint16_t on)
    {
        if (on)
            field |= bit;
        else
            field &= ~bit;
        return 0;
    }

    // ESC M: 0 keep x, 1 back to it, 2 keep y, 3 back to it
    uint32_t memory(uint16_t what)
    {
        switch (what) {
        case 0:
            w->WiMx = w->WiX;
            break;
        case 1:
            if (w->WiMx && w->WiMx <= w->WiTx) {
                w->WiX = w->WiMx;
                address_cursor();
            }
            break;
        case 2:
            w->WiMy = w->WiY;
            break;
        case 3:
            if (w->WiMy < w->WiTy) {
                w->WiY = w->WiMy;
                address_cursor();
            }
            break;
        }
        return 0;
    }

    // ESC N / ESC O: move by the character - 128
    static int16_t offset(uint16_t value) { return int8_t(uint8_t(value + 48 - 128)); }

    // ESC Z0 marks a corner, ESC Zn sets zone n from it to the cursor
    uint32_t zone(uint16_t value)
    {
        if (!(value & 0xFF)) {
            w->WiZoDx = column();
            w->WiZoDy = w->WiY;
            return 0;
        }
        left();
        uint16_t left_edge = w->WiDxI << 3;
        Regs r;
        r.d[1] = value & 0xFF;
        r.d[2] = uint16_t((w->WiZoDx << 3) + left_edge);
        r.d[3] = uint16_t(w->WiZoDy * w->WiTyCar + w->WiDyI);
        r.d[4] = uint16_t((column() << 3) + 7 + left_edge);
        r.d[5] = uint16_t((w->WiY + 1) * w->WiTyCar + w->WiDyI);
        amos_call(&r, w_sy_set_z);
        right();
        return r.d[0];
    }

    // ESC E0 marks a corner, ESC En draws frame n around the text from it
    // to the cursor
    uint32_t frame(uint16_t value)
    {
        WData &d = wdata();
        if (!(value & 0xFF)) {
            d.WiEncDX = column();
            d.WiEncDY = w->WiY;
            return 0;
        }
        uint16_t x = w->WiX, y = w->WiY;
        w->WiGraph = 0xFFFF;
        const uint8_t *f = frames[value & 7];
        int16_t width = column() - d.WiEncDX;
        int16_t height = w->WiY - d.WiEncDY;
        if (width >= 0 && height >= 0) {
            width--;
            locate(d.WiEncDX, d.WiEncDY);
            left();
            up();
            put(f[0]);
            for (int16_t i = width; i >= 0; i--)
                put(f[1]);
            put(f[2]);
            left();
            down();
            for (int16_t i = height; i >= 0; i--) {
                put(f[3]);
                left();
                down();
            }
            put(f[4]);
            left();
            left();
            for (int16_t i = width; i >= 0; i--) {
                put(f[5]);
                left();
                left();
            }
            put(f[6]);
            left();
            up();
            for (int16_t i = height; i >= 0; i--) {
                put(f[7]);
                left();
                up();
            }
        }
        w->WiGraph = 0;
        w->WiY = y;
        w->WiX = x;
        address_cursor();
        return 0;
    }

    // ESC R0 starts recording what is printed, ESC Rn then prints it n
    // times. While recording, every character comes here as if after
    // ESC R (WiEsc 1).
    uint32_t repeat(uint16_t value)
    {
        WData &d = wdata();
        if (!d.WiRep) {
            if (!value) {
                d.WiRepAd[0] = address(d.WiRepBuf);
                d.WiRep++;
                w->WiEsc = 1;
            }
            return 0;
        }
        uint16_t c = value + 48;
        uint8_t *limit = d.WiRepBuf + 80 - 1;
        uint8_t *at = ptr<uint8_t>(d.WiRepAd[0]);
        if (!(at[-2] == 27 && at[-1] == 'R')) {
            *at++ = uint8_t(c);
            if (at < limit) {
                d.WiRepAd[0] = address(at);
                w->WiEsc = 1;
                return 0;
            }
            // Full: print it once
            at += 2;
            c = 48 + 1;
        }
        at[-2] = 0;
        int16_t times = c - 49;
        if (times < 0)
            times = 0;
        do {
            for (const uint8_t *t = d.WiRepBuf; *t; t++)
                put(*t);
        } while (times-- != 0);
        d.WiRep = 0;
        return 0;
    }

    uint32_t escape(uint16_t letter, uint16_t value, uint32_t d0)
    {
        switch (letter) {
        case 'B': return paper(value);
        case 'C': return flag(w->WiSys, SysCursor, value);
        case 'D':
            if (check_colour(value))
                return ErrParam;
            w->WiCuCol = value;
            return 0;
        case 'E': return frame(value);
        case 'I': return inverse(value);
        case 'J': return planes(value);
        case 'K': w->WiGraph = value; return 0;
        case 'M': return memory(value);
        case 'N': return locate_x(offset(value) + column());
        case 'O': return locate_y(offset(value) + w->WiY);
        case 'P': return pen(value);
        case 'Q': return clear_chars(value);
        case 'R': return repeat(value);
        case 'S': return flag(w->WiFlags, FlagShade, value);
        case 'T':
            if (value >= w->WiTx)
                return ErrParam;
            w->WiTab = value;
            return 0;
        case 'U': return flag(w->WiFlags, FlagUnder, value);
        case 'V': return flag(w->WiSys, SysScroll, value);
        case 'W': return writing(value);
        case 'X': return locate_x(value);
        case 'Y': return locate_y(value);
        case 'Z': return zone(value);
        default: return d0;         // Rien
        }
    }

    // ---- character output -------------------------------------------------

    // One character in the planes at the cursor
    void draw(uint8_t c)
    {
        const uint8_t *glyph = ptr<uint8_t>(w->WiFont) + (c << 3);
        uint16_t flags = w->WiFlags;
        if (!flags) {
            for (int p = 0; p <= w->WiNPlan; p++) {
                uint8_t *a = plane(p) + w->WiAdCur;
                switch (w->WiColor[p]) {
                case PlaneZero:
                    for (int y = 0; y < 8; y++, a += line())
                        *a = 0;
                    break;
                case PlaneNormal:
                    for (int y = 0; y < 8; y++, a += line())
                        *a = glyph[y];
                    break;
                case PlaneInverse:
                    for (int y = 0; y < 8; y++, a += line())
                        *a = ~glyph[y];
                    break;
                case PlaneOne:
                    for (int y = 0; y < 8; y++, a += line())
                        *a = 0xFF;
                    break;
                }
            }
            return;
        }
        // Shade masks every other pixel, rotating each line
        uint16_t shade = flags & FlagShade ? 0xAAAA : 0xFFFF;
        bool under = flags & FlagUnder;
        for (int p = 0, bit = w->WiNPlan; bit >= 0; p++, bit--) {
            uint8_t paper = uint8_t(w->WiColFl[p] >> 16), pen = uint8_t(w->WiColFl[p]);
            uint8_t *a = plane(p) + w->WiAdCur;
            if (masked(bit))
                continue;
            for (int y = 0; y < (under ? 7 : 8); y++, a += line()) {
                uint8_t bits = glyph[y] & uint8_t(shade);
                shade = uint16_t(shade >> 1 | shade << 15);
                uint8_t back = ~bits & paper, fore = bits & pen;
                uint8_t v = draw_mode == DrawBoth ? back | fore : draw_mode == DrawPaper ? back : fore;
                combine(a, v);
            }
            if (under) {
                uint8_t v = pen & uint8_t(shade);
                shade = uint16_t(shade >> 1 | shade << 15);
                combine(a, v);
            }
        }
    }

    static void combine(uint8_t *a, uint8_t v)
    {
        switch (write_mode) {
        case WriteReplace: *a = v; break;
        case WriteOr: *a |= v; break;
        case WriteXor: *a ^= v; break;
        case WriteAnd: *a &= v; break;
        }
    }

    // COut: a character, control code or part of an escape sequence.
    // Returns d0: 0, an error, or for codes that do nothing the d0 it
    // came with (callers stop printing at a non-zero low word).
    uint32_t out(uint8_t c, uint32_t d0)
    {
        if (w->WiEsc) {
            if (--w->WiEsc) {
                w->WiEscPar = c;
                return d0;
            }
            uint16_t letter = w->WiEscPar;
            uint32_t high = d0 & 0xFFFF0000;
            if (letter > 'Z')
                return high | letter;
            if (letter < 'A')
                return high | uint16_t(letter - 'A');
            return escape(letter, uint16_t(c - '0'), high | uint16_t((letter - 'A') << 2));
        }
        if (c < 32 && !w->WiGraph)
            return control(c, d0);
        draw(c);
        // One to the right, to the next line at the end
        w->WiAdCur++;
        if (--w->WiX == 0) {
            w->WiX = w->WiTx;
            uint16_t y = w->WiY + 1;
            if (y < w->WiTy) {
                w->WiY = y;
                address_cursor();
            } else if (w->WiSys & SysScroll) {
                address_cursor();
                scroll_up();
            } else {
                w->WiY = 0;
                address_cursor();
            }
        }
        return 0;
    }

    void put(uint8_t c) { out(c, 0); }

    // Prt: a string up to its zero or a non-zero low word of d0, the
    // cursor drawn afterwards
    uint32_t print(const uint8_t *&text, uint32_t d0)
    {
        while (uint8_t c = *text++) {
            d0 = out(c, d0);
            if (d0 & 0xFFFF)
                break;
        }
        cursor_on();
        return d0;
    }

    // ---- borders ----------------------------------------------------------

    // SetBord / SetNorm: the border is drawn with the graphic characters,
    // without scrolling or writing flags, in the border colours
    struct Saved {
        uint16_t sys, flags, paper, pen;
    };

    Saved border_mode()
    {
        Saved saved = {w->WiSys, w->WiFlags, w->WiPaper, w->WiPen};
        w->WiSys &= ~SysScroll;
        w->WiGraph = 0xFFFF;
        w->WiFlags &= 1;
        w->WiPaper = w->WiBorPap;
        w->WiPen = w->WiBorPen;
        colours();
        return saved;
    }

    void normal_mode(const Saved &saved)
    {
        w->WiPen = saved.pen;
        w->WiPaper = saved.paper;
        w->WiFlags = saved.flags;
        w->WiSys = saved.sys;
        w->WiGraph = 0;
        colours();
    }

    // DHoriz: a horizontal side from the cursor: the left corner, the
    // right corner at the end, the side between, then the title over it.
    // set points to the strings, moved past the three used.
    void horizontal(const uint8_t *&set, const uint8_t *title)
    {
        Saved saved = border_mode();
        uint16_t y = w->WiY;
        for (;;) {
            if (w->WiY != y) {
                set = count(set).end;
                left();
                break;
            }
            uint8_t c = *set++;
            if (!c)
                break;
            put(c);
        }
        uint16_t start = column();
        // The right corner
        Count corner = count(set);
        uint16_t end = w->WiTx - corner.n;
        if (corner.n > w->WiTx)
            end = 0;
        if (end < start)
            end = start;
        locate(end, y);
        while (w->WiY == y) {
            uint8_t c = *set++;
            if (!c)
                break;
            put(c);
        }
        set = corner.end;
        // The side, the string over and over
        const uint8_t *side = set;
        locate(start, y);
        while (column() < end) {
            uint8_t c = *set++;
            if (!c) {
                set = side;
                c = *set++;
                if (!c) {
                    set--;
                    c = ' ';
                }
            }
            put(c);
        }
        set = count(side).end;
        // The title
        locate(start, y);
        while (column() < end) {
            uint8_t c = *title++;
            if (!c)
                break;
            put(c);
        }
        normal_mode(saved);
    }

    // DVert: a vertical side at the cursor column, lines 1 to WiTyI
    void vertical(const uint8_t *&set)
    {
        Saved saved = border_mode();
        uint16_t x = column();
        const uint8_t *side = set;
        for (uint16_t y = 1; y <= w->WiTyI;) {
            locate(x, y);
            uint8_t c = *set++;
            if (!c) {
                set = side;
                c = *set++;
                if (!c) {
                    set--;
                    c = ' ';
                }
            }
            uint16_t before = w->WiX;
            put(c);
            if (w->WiX != before)
                y++;
        }
        set = count(side).end;
        normal_mode(saved);
    }

    // DesBord
    uint32_t draw_border()
    {
        if (!w->WiBord)
            return ErrNoBorder;
        const uint8_t *set = borders[(w->WiBord - 1) & 15];
        outer();
        home();
        horizontal(set, w->WiTitH);
        locate(w->WiTx - 1, 1);
        vertical(set);
        locate(0, w->WiTy - 1);
        horizontal(set, w->WiTitB);
        locate(0, 1);
        vertical(set);
        inner();
        return 0;
    }

    // ReBord: draw it again, the cursor where it was
    void redraw_border()
    {
        uint16_t x = w->WiX, y = w->WiY;
        uint32_t at = w->WiAdCur;
        draw_border();
        w->WiAdCur = at;
        w->WiY = y;
        w->WiX = x;
    }

    // CClw: clear the whole window, its border too
    void clear_all()
    {
        if (w->WiBord) {
            w->WiTitH[0] = 0;
            w->WiTitB[0] = 0;
            w->WiBord = 16;
            w->WiBorPap = w->WiPaper;
            draw_border();
        }
        clw();
    }

    // ---- windows over each other ------------------------------------------

    // WiStore: keep what the current window shows, to put it back when a
    // window in front of it goes away. (Sizes and offsets were 16 bits in
    // the assembler, too small for screens of more than 64 KB a bitplane.)
    static void store(Screen *s)
    {
        if (!s->EcWiDec)
            return;
        Window *w = ptr<Window>(s->EcWindow);
        if (!w)
            return;
        uint32_t size = uint32_t(w->WiTxR) * w->WiTyP * s->EcNPlan;
        if (w->WiDBuf && w->WiTBuf != size) {
            free_mm(w->WiDBuf, w->WiTBuf);
            w->WiDBuf = 0;
            w->WiTBuf = 0;
        }
        if (!w->WiDBuf) {
            uint32_t buffer = fast_mm(size);
            if (!buffer)
                return;
            w->WiDBuf = buffer;
            w->WiTBuf = size;
        }
        uint8_t *to = ptr<uint8_t>(w->WiDBuf);
        int32_t line = int16_t(s->EcTLigne);
        uint32_t offset = uint32_t(w->WiDyR) * s->EcTLigne + w->WiDxR;
        w->WiTxBuf = w->WiTxR;
        for (uint16_t y = 0; y < w->WiTyP; y++, offset += line)
            for (int p = 0; p <= w->WiNPlan; p++, to += w->WiTxR)
                copy(to, ptr<uint8_t>(s->EcLogic[p]) + offset, w->WiTxR);
    }

    // WiClip: the part of x..right on pixel line y that no window in front
    // hides, from x. True if there is none.
    bool clip(uint16_t &x, uint16_t &to, uint16_t y, uint16_t right) const
    {
        uint32_t next = w->WiPrev;
        while (next) {
            Window *f = ptr<Window>(next);
            uint16_t x0 = x, to0 = to;
            if (y >= f->WiDyR && y < f->WiFyR && to > f->WiDxR && x < f->WiFxR) {
                if (x < f->WiDxR)
                    to = f->WiDxR;
                else
                    x = f->WiFxR;
                if (to <= x) {
                    // All hidden: once more after this window, if anything
                    // is left on the line
                    if ((x0 == x && to0 == to) || x >= right)
                        return true;
                    x = to;
                    to = right;
                    next = w->WiPrev;
                    continue;
                }
            }
            next = f->WiPrev;
        }
        return false;
    }

    // WiEf0: put the saved background back, between x and right, on lines
    // y to end (at least one), where no window in front hides it. stride:
    // the width the background was saved with.
    void restore(uint16_t x, uint16_t right, uint16_t y, uint16_t end, uint32_t stride)
    {
        const uint8_t *buffer = ptr<uint8_t>(w->WiDBuf);
        do {
            uint16_t to = x;
            for (;;) {
                uint16_t from = to;
                to = right;
                if (clip(from, to, y, right))
                    break;
                uint32_t screen = uint32_t(y) * s->EcTLigne + from;
                const uint8_t *saved
                    = buffer + uint32_t(y - w->WiDyR) * s->EcNPlan * stride + uint16_t(from - w->WiDxR);
                for (int p = 0; p < s->EcNPlan; p++, saved += stride)
                    copy(ptr<uint8_t>(s->EcLogic[p]) + screen, saved, uint16_t(to - from));
                if (to >= right)
                    break;
            }
            y++;
        } while (y < end);
    }

    // WiEff: the saved background back, on lines top to bottom only
    void erase(uint16_t top, uint16_t bottom)
    {
        if (!s->EcWiDec || !w->WiDBuf)
            return;
        if (w->WiDyR >= bottom || w->WiFyR <= top)
            return;
        if (w->WiDyR > top)
            top = w->WiDyR;
        if (w->WiFyR < bottom)
            bottom = w->WiFyR;
        restore(w->WiDxR, w->WiDxR + w->WiTxR, top, bottom, w->WiTxR);
    }

    // WiEff2: after Wind Size, the part the old size (width bytes, height
    // lines) and the new one share, inside the border
    void erase_resized(uint16_t width, uint16_t height)
    {
        if (!s->EcWiDec || !w->WiDBuf)
            return;
        uint16_t n = width > w->WiTxR ? w->WiTxR : width;
        uint16_t lines = height > w->WiTyP ? w->WiTyP : height;
        uint16_t x = w->WiDxR, right = x + n;
        uint16_t y = w->WiDyR, end = y + lines;
        if (w->WiBord) {
            x++;
            y += 8;
            right--;
            end -= 8;
        }
        restore(x, right, y, end, width);
    }

    // WiEffBuf
    void free_buffer()
    {
        if (w->WiDBuf) {
            free_mm(w->WiDBuf, w->WiTBuf);
            w->WiDBuf = 0;
            w->WiTBuf = 0;
        }
    }

    // The windows after w, behind it, redrawn on lines top to bottom as if
    // it was not there
    void erase_behind(uint16_t top, uint16_t bottom)
    {
        Window *next = ptr<Window>(w->WiNext);
        if (!next)
            return;
        uint32_t prev = next->WiPrev;
        next->WiPrev = 0;
        for (Window *b = next; b; b = ptr<Window>(b->WiNext))
            Win{s, b}.erase(top, bottom);
        next->WiPrev = prev;
    }
};

// ---- the functions ---------------------------------------------------------

Win current()
{
    Screen *s = ptr<Screen>(wdata().EcCourant);
    return {s, ptr<Window>(s->EcWindow)};
}

using Routine = uint32_t (*)(Win &, const AllRegs &, uint32_t d0);

// AutoPrt: a print on a screen with autoback. With double buffering the
// routine runs in both buffers, from the same window state.
uint32_t autoback_print(Win &t, Routine routine, const AllRegs &r)
{
    if (!(t.s->EcFlags & DoubleBuffer)) {
        uint32_t d0 = routine(t, r, autoback(w_tabk1));
        autoback(w_tabk4);
        return d0;
    }
    // (The assembler kept the cursor background from the window structure,
    // past its first part, instead of the screen's EcCurS)
    uint8_t cursor[sizeof(t.s->EcCurS)];
    uint8_t window[offsetof(Window, WiTitH)];
    copy(cursor, t.s->EcCurS, sizeof cursor);
    copy(window, t.w, sizeof window);
    routine(t, r, autoback(w_tabk1));
    copy(t.s->EcCurS, cursor, sizeof cursor);
    copy(t.w, window, sizeof window);
    uint32_t d0 = routine(t, r, autoback(w_tabk2));
    autoback(w_tabk3);
    return d0;
}

uint32_t print_routine(Win &t, const AllRegs &r, uint32_t d0)
{
    t.cursor_off();
    const uint8_t *text = ptr<uint8_t>(r.a[1]);
    return t.print(text, d0);
}

uint32_t print_n_routine(Win &t, const AllRegs &r, uint32_t d0)
{
    t.cursor_off();
    const uint8_t *text = ptr<uint8_t>(r.a[1]);
    uint16_t n = uint16_t(r.d[1]) - 1;
    if (int16_t(n) >= 0) {
        do {
            d0 = t.out(*text++, d0);
            if (d0 & 0xFFFF)
                break;
        } while (n-- != 0);
    }
    t.cursor_on();
    return d0;
}

uint32_t centre_routine(Win &t, const AllRegs &r, uint32_t)
{
    t.cursor_off();
    const uint8_t *text = ptr<uint8_t>(r.a[1]);
    return t.print(text, t.locate_x(r.d[1]));
}

uint32_t locate_routine(Win &t, const AllRegs &r, uint32_t)
{
    t.cursor_off();
    uint16_t x = r.d[1] == EntNul ? t.column() : r.d[1];
    uint16_t y = r.d[2] == EntNul ? t.w->WiY : r.d[2];
    uint32_t d0 = t.locate(x, y);
    t.cursor_on();
    return d0;
}

uint32_t print(Routine routine, const AllRegs &r, uint32_t d0)
{
    Win t = current();
    if (t.s->EcAuto)
        return autoback_print(t, routine, r);
    return routine(t, r, d0);
}

// WPrint3: a line scrolled to the left, for the Interface: a1 the text, d1
// the characters to skip (bit 31: obey control codes), d2/d3 the columns
// to print between. Returns d1 = the column reached.
uint32_t print_part(AllRegs *r)
{
    Win t = current();
    t.cursor_off();
    const uint8_t *text = ptr<uint8_t>(r->a[1]);
    uint16_t column = t.column();
    int16_t first = r->d[2], last = r->d[3];
    uint32_t skip = r->d[1];
    uint32_t d0 = r->d[0];
    for (;;) {
        uint8_t c = *text++;
        if (!c) {
            d0 = 0;
            r->d[1] = column;
            break;
        }
        if (c >= 32) {
            skip = (skip & 0xFFFF0000) | uint16_t(skip - 1);
            if (int16_t(skip) >= 0)
                continue;
            if (int16_t(column) >= first) {
                if (int16_t(column) >= last) {
                    d0 = 0;
                    r->d[1] = column;
                    break;
                }
                d0 = t.out(c, d0);
                if (d0) {
                    r->d[1] = c;
                    break;
                }
            }
            column++;
            continue;
        }
        if (c != 9) {
            if (int32_t(skip) >= 0) {
                if (c == 27)
                    text += 2;
                continue;
            }
            if (c == 27) {
                d0 = t.out(c, d0);
                d0 = t.out(*text++, d0);
                c = *text++;
            }
        }
        d0 = t.out(c, d0);
    }
    r->a[1] = address(text);
    t.cursor_on();
    return d0;
}

// WOpen: window d1 at x d2 (pixels), y d3, d4 x d5 characters; d6 bit 0:
// clear it, d7 the border, a1 the font (must be 0)
uint32_t open(const AllRegs &r)
{
    Screen *s = ptr<Screen>(wdata().EcCourant);
    uint32_t memory = fast_mm(Window::WiLong);
    if (!memory)
        return ErrMemory;
    Window *w = ptr<Window>(memory);
    Win t = {s, w};
    auto fail = [&](uint32_t error) {
        free_mm(memory, Window::WiLong);
        return error;
    };
    if (find(s, r.d[1]))
        return fail(ErrOpen);
    w->WiNumber = r.d[1];
    w->WiNPlan = s->EcNPlan - 1;
    w->WiTyCar = 8;
    if (r.a[1])
        return fail(ErrFont);
    w->WiFont = wdata().JeuDefo;
    uint16_t x = uint16_t(uint16_t(r.d[2]) >> 4) << 1;
    if (uint16_t(r.d[7]) > 16)
        return fail(ErrParam);
    w->WiBord = r.d[7];
    if (w->WiBord)
        x++;
    if (uint32_t error = t.set_size(x, r.d[3], r.d[4], r.d[5]))
        return fail(error);

    w->WiSys = 0;
    w->WiEsc = 0;
    t.writing(0);
    Window *previous = ptr<Window>(s->EcWindow);
    if (previous) {
        w->WiPaper = previous->WiPaper;
        w->WiPen = previous->WiPen;
        w->WiCuCol = previous->WiCuCol;
        w->WiBorPap = previous->WiBorPap;
        w->WiBorPen = previous->WiBorPen;
        w->WiTab = previous->WiTab;
    } else if (s->EcNPlan == 1) {
        w->WiPaper = 0;
        w->WiPen = 1;
        w->WiCuCol = 1;
        w->WiTab = 4;
        w->WiBorPap = 0;
        w->WiBorPen = 1;
    } else {
        w->WiPaper = 1;
        w->WiPen = 2;
        w->WiCuCol = 3;
        w->WiTab = 4;
        w->WiBorPap = 1;
        w->WiBorPen = 2;
    }
    t.colours();
    w->WiSys |= SysScroll;
    if (previous) {
        Win{s, previous}.cursor_off();
        Win::store(s);
    }
    w->WiTitH[0] = w->WiTitH[1] = 0;
    w->WiTitB[0] = w->WiTitB[1] = 0;
    if (w->WiBord)
        t.draw_border();
    t.inner();
    if (r.d[6] & 1)
        t.clw();
    t.home();
    copy(w->WiCuDraw, default_cursor, sizeof default_cursor);
    w->WiSys |= SysCursor;
    t.cursor_on();

    s->EcWindow = memory;
    w->WiPrev = 0;
    w->WiNext = address(previous);
    if (previous)
        previous->WiPrev = memory;
    return 0;
}

// WQWind: window d1 to the front
uint32_t activate(uint16_t number)
{
    Win t = current();
    Window *w = find(t.s, number);
    if (!w)
        return ErrNotOpen;
    if (w->WiPrev) {
        t.cursor_off();
        Win::store(t.s);
        Window *prev = ptr<Window>(w->WiPrev), *next = ptr<Window>(w->WiNext);
        prev->WiNext = address(next);
        if (next)
            next->WiPrev = address(prev);
        Window *first = ptr<Window>(t.s->EcWindow);
        t.s->EcWindow = address(w);
        w->WiPrev = 0;
        w->WiNext = address(first);
        first->WiPrev = address(w);
        t.w = w;
        t.erase(w->WiDyR, w->WiFyR);
        t.free_buffer();
        t.cursor_on();
    }
    t.w->WiEsc = 0;
    return 0;
}

// WiD1: close the current window, show the next one
uint32_t close(Screen *s)
{
    Window *w = ptr<Window>(s->EcWindow);
    if (!w)
        return ErrNotOpen;
    Win t = {s, w};
    t.cursor_off();
    t.clear_all();
    Window *next = ptr<Window>(w->WiNext);
    uint16_t top = w->WiDyR, bottom = w->WiFyR;
    free_mm(address(w), Window::WiLong);
    s->EcWindow = address(next);
    if (!next)
        return 0;
    next->WiPrev = 0;
    Win n = {s, next};
    n.erase(top, bottom);
    n.free_buffer();
    for (Window *b = ptr<Window>(next->WiNext); b; b = ptr<Window>(b->WiNext))
        Win{s, b}.erase(top, bottom);
    n.cursor_on();
    return 0;
}

// WiCls: close all windows but window 0
uint32_t close_all_but_0()
{
    Win t = current();
    t.cursor_off();
    Window *zero = nullptr;
    for (Window *w = t.w, *next; w; w = next) {
        next = ptr<Window>(w->WiNext);
        if (!w->WiNumber) {
            zero = w;
            continue;
        }
        Win{t.s, w}.free_buffer();
        free_mm(address(w), Window::WiLong);
    }
    t.s->EcWindow = address(zero);
    zero->WiPrev = 0;
    zero->WiNext = 0;
    Win z = {t.s, zero};
    z.clw();
    z.free_buffer();
    z.cursor_on();
    return 0;
}

// WiMove: the current window to x (pixels) d1, y d2
uint32_t move(const AllRegs &r)
{
    Win t = current();
    Window *w = t.w;
    if (!w->WiNumber)
        return ErrWindow0;
    t.cursor_off();
    Win::store(t.s);
    t.erase_behind(w->WiDyR, w->WiFyR);
    uint16_t x = uint16_t(uint16_t(r.d[1]) >> 4) << 1;
    if (w->WiBord)
        x++;
    // (The assembler compared x with EntNul after making it a word, so an
    // omitted x never kept the old position; AMOSPro.Lib never omits it)
    uint32_t y = (r.d[1] & 0xFFFF0000) | uint16_t(r.d[2]);
    if (y == EntNul)
        y = w->WiDyR;
    uint16_t x0 = w->WiDxR, y0 = w->WiDyR, tx = w->WiTxR, ty = w->WiTyR;
    uint32_t error = t.set_size(x, y, tx, ty);
    if (error)
        t.set_size(x0, y0, tx, ty);
    t.inner();
    t.address_cursor();
    t.erase(0, 10000);
    t.free_buffer();
    t.cursor_on();
    return error;
}

// WiSize: the current window to d1 x d2 characters
uint32_t resize(const AllRegs &r)
{
    Win t = current();
    Window *w = t.w;
    if (!w->WiNumber)
        return ErrWindow0;
    t.cursor_off();
    Win::store(t.s);
    uint16_t width = w->WiTxR, height = w->WiTyP;
    t.erase_behind(w->WiDyR, w->WiFyR);
    uint32_t ty = (r.d[1] & 0xFFFF0000) | uint16_t(r.d[2]);
    if (ty == EntNul)
        ty = w->WiTyR;
    uint16_t x0 = w->WiDxR, y0 = w->WiDyR, tx0 = w->WiTxR, ty0 = w->WiTyR;
    uint16_t error = t.set_size(x0, y0, r.d[1], ty);
    if (error)
        t.set_size(x0, y0, tx0, ty0);
    t.inner();
    t.address_cursor();
    if (w->WiBord)
        t.draw_border();
    t.clw();
    t.erase_resized(width, height);
    t.free_buffer();
    t.cursor_on();
    return error;
}

// WSBor: Border n,paper,pen (EntNul: unchanged)
uint32_t set_border(const AllRegs &r)
{
    Win t = current();
    Window *w = t.w;
    if (r.d[1] != EntNul) {
        if (r.d[1] >= 16)
            return ErrParam;
        if (uint16_t(r.d[1]))
            w->WiBord = r.d[1];
    }
    if (r.d[2] != EntNul) {
        if (t.check_colour(r.d[2]))
            return ErrParam;
        w->WiBorPap = r.d[2];
    }
    if (r.d[3] != EntNul) {
        if (t.check_colour(r.d[3]))
            return ErrParam;
        w->WiBorPen = r.d[3];
    }
    t.redraw_border();
    return 0;
}

// SsWti: up to 79 characters
void set_title(uint8_t *to, const uint8_t *from)
{
    for (int i = 0; i < 79; i++)
        if (!(*to++ = *from++))
            return;
    *to = 0;
}

// WSTit: titles d1 (top) and d2 (bottom), if not null
uint32_t set_titles(const AllRegs &r)
{
    Win t = current();
    if (!t.w->WiBord)
        return ErrNoBorder;
    if (r.d[1])
        set_title(t.w->WiTitH, ptr<uint8_t>(r.d[1]));
    if (r.d[2])
        set_title(t.w->WiTitB, ptr<uint8_t>(r.d[2]));
    t.redraw_border();
    return 0;
}

} // namespace

// The WiCall functions (w/windows_glue.s): r holds all the registers, as
// they come and as they go back
extern "C" void w_window(uint32_t function, AllRegs *r)
{
    uint32_t &d0 = r->d[0];
    switch (function) {
    case wi::ChrOut: {
        Win t = current();
        t.cursor_off();
        d0 = t.out(r->d[1], d0);
        t.cursor_on();
        break;
    }
    case wi::Print:
        d0 = print(print_routine, *r, d0);
        break;
    case wi::Centre: {
        Win t = current();
        r->d[1] = uint16_t(uint16_t(t.w->WiTx - count(ptr<uint8_t>(r->a[1])).n) >> 1);
        if (t.s->EcAuto) {
            d0 = autoback_print(t, centre_routine, *r);
        } else {
            t.cursor_off();
            const uint8_t *text = ptr<uint8_t>(r->a[1]);
            d0 = t.print(text, t.locate_x(r->d[1]));
            r->a[1] = address(text);
        }
        break;
    }
    case wi::WindOp:
        d0 = open(*r);
        break;
    case wi::Locate:
        d0 = print(locate_routine, *r, d0);
        break;
    case wi::QWindow:
        d0 = activate(r->d[1]);
        break;
    case wi::WinDel: {
        Screen *s = ptr<Screen>(wdata().EcCourant);
        d0 = ptr<Window>(s->EcWindow)->WiNumber ? close(s) : ErrWindow0;
        break;
    }
    case wi::SBord:
        d0 = set_border(*r);
        break;
    case wi::STitle:
        d0 = set_titles(*r);
        break;
    case wi::GAdr: {
        Win t = current();
        r->a[0] = address(t.w);
        r->d[1] = t.w->WiNumber;
        d0 = 0;
        break;
    }
    case wi::MoveWi:
        d0 = move(*r);
        break;
    case wi::ClsWi:
        d0 = close_all_but_0();
        break;
    case wi::SizeWi:
        d0 = resize(*r);
        break;
    case wi::SCurWi: {
        Win t = current();
        t.cursor_off();
        copy(t.w->WiCuDraw, ptr<uint8_t>(r->a[1]), 8);
        t.cursor_on();
        d0 = 0;
        break;
    }
    case wi::XYCuWi: {
        Win t = current();
        r->a[0] = address(t.w);
        r->d[1] = t.column();
        r->d[2] = t.w->WiY;
        d0 = 0;
        break;
    }
    case wi::XGrWi:
    case wi::YGrWi: {
        // The pixel position of a column or line; d1 = -1 (d0 unchanged)
        // if outside the window
        Win t = current();
        r->a[0] = address(t.w);
        uint16_t v = r->d[1];
        if (function == wi::XGrWi) {
            if (v >= t.w->WiTx) {
                r->d[1] = 0xFFFFFFFF;
                break;
            }
            r->d[1] = int32_t(int16_t((v + t.w->WiDxI) << 3));
        } else {
            if (v >= t.w->WiTy) {
                r->d[1] = 0xFFFFFFFF;
                break;
            }
            r->d[1] = int32_t(int16_t((v << 3) + t.w->WiDyI));
        }
        d0 = 0;
        break;
    }
    case wi::Print2:
        d0 = print(print_n_routine, *r, d0);
        break;
    case wi::Print3:
        d0 = print_part(r);
        break;
    case wi::SXSYCuWi: {
        // The size of the window, d0 its flags (WiSys)
        Win t = current();
        r->a[0] = address(t.w);
        d0 = (d0 & 0xFFFF0000) | t.w->WiSys;
        r->d[1] = t.w->WiTx;
        r->d[2] = t.w->WiTy;
        break;
    }
    case wi::SXSYCuWi + 1: {
        // WiDelA: close all windows of the current screen
        Screen *s = ptr<Screen>(wdata().EcCourant);
        while (!close(s))
            ;
        d0 = 0;
        break;
    }
    }
}
