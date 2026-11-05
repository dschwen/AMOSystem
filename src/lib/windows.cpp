// Text window instructions of AMOSPro.Lib (ecrans.s / FENETRES)

#include "lib/strings.h"

using namespace amos;

namespace {

constexpr uint32_t EntNul = 0x80000000;

// Errors of the screen and window functions of AMOS.library
[[noreturn]] void w_fon_call() { screen_error(16); }

// The current screen, or the "screen not opened" error
void check_screen()
{
    if (!idata().ScOn)
        screen_error(3);
}

Regs &wi_check(int function, Regs &r)
{
    wi_call(function, r);
    if (r.d[0])
        screen_error(r.d[0]);
    return r;
}

// GoWn: control codes to the current window
void send(const char *codes)
{
    check_screen();
    Regs r = {};
    r.a[1] = address(codes);
    wi_check(wi::Print, r);
}

void escape(char code, char value)
{
    const char text[4] = {27, code, value, 0};
    send(text);
}

} // namespace

// LOCATE x,y. An omitted coordinate arrives as EntNul, which the window
// library leaves unchanged.
extern "C" void lib_in_locate(int32_t x, int32_t y)
{
    check_screen();
    Regs r = {};
    r.d[1] = x;
    r.d[2] = y;
    wi_check(wi::Locate, r);
}

// WIND OPEN n,x,y,width,height[,border[,set]]
extern "C" void lib_in_windopen7(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t border,
                                 uint32_t set)
{
    check_screen();
    if (n >= 65536)
        w_fon_call();
    Regs r = {};
    r.a[1] = set;
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = w;
    r.d[5] = h;
    r.d[6] = 1;             // clear it
    r.d[7] = border;
    wi_check(wi::WindOp, r);
}

extern "C" void lib_in_windopen6(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t border)
{
    lib_in_windopen7(n, x, y, w, h, border, 0);
}

extern "C" void lib_in_windopen5(uint32_t n, uint32_t x, uint32_t y, uint32_t w, uint32_t h)
{
    lib_in_windopen7(n, x, y, w, h, 0, 0);
}

// WIND SAVE
extern "C" void lib_in_windsave()
{
    if (!idata().ScOnAd)
        screen_error(3);
    ptr<Screen>(idata().ScOnAd)->EcWiDec = 0xFFFF;
}

// WIND MOVE x,y / WIND SIZE w,h
static void move_size(int function, int32_t a, int32_t b)
{
    check_screen();
    if (b < 0 || a < 0)
        fon_call();
    Regs r = {};
    r.d[1] = a;
    r.d[2] = b;
    wi_check(function, r);
}

extern "C" void lib_in_windmove(int32_t x, int32_t y) { move_size(wi::MoveWi, x, y); }
extern "C" void lib_in_windsize(int32_t w, int32_t h) { move_size(wi::SizeWi, w, h); }

// WIND CLOSE, WINDOW n, =WINDON
extern "C" void lib_in_windclose()
{
    check_screen();
    Regs r = {};
    wi_check(wi::WinDel, r);
}

extern "C" void lib_in_window(uint32_t n)
{
    check_screen();
    Regs r = {};
    r.d[1] = n;
    wi_check(wi::QWindow, r);
}

extern "C" uint32_t lib_fn_windon()
{
    check_screen();
    Regs r = {};
    wi_call(wi::GAdr, r);
    return r.d[1];
}

// WRITING mode[,which]
extern "C" void lib_in_writing2(uint32_t mode, uint32_t which)
{
    if (which >= 3)
        w_fon_call();
    if (mode >= 5)
        w_fon_call();
    escape('W', static_cast<char>('0' + (which << 3 | mode)));
}

extern "C" void lib_in_writing1(uint32_t mode) { lib_in_writing2(mode, 0); }

// BORDER n,paper,pen
extern "C" void lib_in_border(uint32_t n, uint32_t paper, uint32_t pen)
{
    check_screen();
    Regs r = {};
    r.d[1] = n;
    r.d[2] = paper;
    r.d[3] = pen;
    wi_check(wi::SBord, r);
}

// TITLE TOP a$, TITLE BOTTOM a$
static void title(const uint8_t *s, bool top)
{
    uint32_t text = address(str_to_buffer(s));
    check_screen();
    Regs r = {};
    r.d[1] = top ? text : 0;
    r.d[2] = top ? 0 : text;
    wi_check(wi::STitle, r);
}

extern "C" void lib_in_title_top(const uint8_t *s) { title(s, true); }
extern "C" void lib_in_title_bottom(const uint8_t *s) { title(s, false); }

// =X CURS, =Y CURS
extern "C" uint32_t lib_fn_xcurs()
{
    check_screen();
    Regs r = {};
    wi_call(wi::XYCuWi, r);
    return r.d[1];
}

extern "C" uint32_t lib_fn_ycurs()
{
    check_screen();
    Regs r = {};
    wi_call(wi::XYCuWi, r);
    return r.d[2];
}

// SET CURS l1,...,l8: the cursor's eight lines
extern "C" void lib_in_set_curs(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e, uint32_t f, uint32_t g,
                                uint32_t h)
{
    uint8_t *lines = ptr<uint8_t>(idata().Buffer);
    const uint32_t v[] = {a, b, c, d, e, f, g, h};
    for (int i = 0; i < 8; i++)
        lines[i] = static_cast<uint8_t>(v[i]);
    Regs r = {};
    r.a[1] = address(lines);
    wi_call(wi::SCurWi, r);
}

// CENTRE a$
extern "C" void lib_in_centre(const uint8_t *s)
{
    check_screen();
    Regs r = {};
    r.a[1] = address(str_to_buffer(s));
    wi_check(wi::Centre, r);
}

// CMOVE dx,dy
extern "C" void lib_in_cmove(uint32_t dx, uint32_t dy)
{
    if (dy == EntNul)
        dy = 0;
    if (dx == EntNul)
        dx = 0;
    dy += 128;
    dx += 128;
    if (dy > 255 || dx > 255)
        w_fon_call();
    const char text[] = {27, 'N', static_cast<char>(dx), 27, 'O', static_cast<char>(dy), 0};
    send(text);
}

// CURS PEN n, PAPER n, PEN n
extern "C" void lib_in_curs_pen(uint32_t n) { escape('D', static_cast<char>('0' + n)); }
extern "C" void lib_in_paper(uint32_t n) { escape('B', static_cast<char>('0' + n)); }
extern "C" void lib_in_pen(uint32_t n) { escape('P', static_cast<char>('0' + n)); }

// The single control codes
extern "C" void lib_in_clw() { send("\x19"); }
extern "C" void lib_in_home() { send("\x0c"); }
extern "C" void lib_in_cleft() { send("\x1d"); }
extern "C" void lib_in_cright() { send("\x1c"); }
extern "C" void lib_in_cup() { send("\x1e"); }
extern "C" void lib_in_cdown() { send("\x1f"); }
extern "C" void lib_in_cline0() { send("\x1a"); }
extern "C" void lib_in_curs_off() { escape('C', '0'); }
extern "C" void lib_in_curs_on() { escape('C', '1'); }
extern "C" void lib_in_inverse_on() { escape('I', '1'); }
extern "C" void lib_in_inverse_off() { escape('I', '0'); }
extern "C" void lib_in_under_on() { escape('U', '1'); }
extern "C" void lib_in_under_off() { escape('U', '0'); }
extern "C" void lib_in_scroll_on() { escape('V', '1'); }
extern "C" void lib_in_scroll_off() { escape('V', '0'); }
extern "C" void lib_in_shade_on() { escape('S', '1'); }
extern "C" void lib_in_shade_off() { escape('S', '0'); }
extern "C" void lib_in_memorize_x() { escape('M', '0'); }
extern "C" void lib_in_memorize_y() { escape('M', '2'); }
extern "C" void lib_in_remember_x() { escape('M', '1'); }
extern "C" void lib_in_remember_y() { escape('M', '3'); }

// CLINE n: clear n characters of the line
extern "C" void lib_in_cline1(uint32_t n)
{
    if (!n || n >= 255 - 48)
        w_fon_call();
    escape('Q', static_cast<char>(n + 48));
}

// HSCROLL n, VSCROLL n (1-4)
static void scroll(uint32_t n, char first)
{
    if (n > 4 || !n)
        w_fon_call();
    const char text[] = {static_cast<char>(first + n - 1), 0};
    send(text);
}

extern "C" void lib_in_hscroll(uint32_t n) { scroll(n, 16); }
extern "C" void lib_in_vscroll(uint32_t n) { scroll(n, 20); }

// SET TAB n
extern "C" void lib_in_set_tab(uint32_t n)
{
    if (n > 255 - 48)
        w_fon_call();
    escape('T', static_cast<char>(n + 48));
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_windows.inc"
#endif
