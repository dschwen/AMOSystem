// The monitor's listing window (+Monitor.s Ttt_*): the program's lines
// detokenised into Ttt_BufE (256 bytes each: a length word, the text,
// byte 255 set if the line is not editable), shown with the marks (the
// next instruction, the breakpoint, the expression being chosen), the
// separation with the information window dragged, the clicks: a
// breakpoint, an expression, help.
//
// Tt_Decoup splits a line into its parts (Tt_Ligne): for each a letter
// (":" a separation, "I" an instruction, "V" a variable, "W" one after a
// ":", "X" a procedure, "0" to "2" constants...), its column in the text
// and its offset in the line; a 0 at the end.

#include "monitor/monitor.h"

using namespace amos;
using namespace amos::monitor;

// +Monitor.s


namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint16_t E2Sx = 640;
constexpr uint32_t InfSize = 88 * 100;
const uint8_t WindowInit[] = {27, 'C', '0', 27, 'B', '2', 27, 'P', '3', 25, 27, 'V', '0', 0};

void print(const uint8_t *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

void wcall(int function, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    wi_call(function, r);
}

uint8_t *buffer_line(uint16_t n) { return ptr<uint8_t>(mon().Ttt_BufE) + 256 * n; }

// Ttt_FindL / Ttt_NextL: line n of the program (the next one), kept with
// the procedure it is in; 0 at the end
uint8_t *find_line(uint16_t n)
{
    MonData &m = mon();
    Regs r;
    r.d[0] = n;
    r.a[0] = program().Prg_StBas;
    lib_call(L_Tk_FindL, r);
    m.Tt_CurLigne = r.a[0];
    m.Tt_DebProc = r.a[1];
    return uint16_t(r.d[0]) ? ptr<uint8_t>(r.a[0]) : nullptr;
}

uint8_t *next_line()
{
    MonData &m = mon();
    Regs r;
    r.a[0] = m.Tt_CurLigne;
    lib_call(L_Tk_FindN, r);
    m.Tt_CurLigne = r.a[0];
    m.Tt_DebProc = r.a[1];
    return uint16_t(r.d[0]) ? ptr<uint8_t>(r.a[0]) : nullptr;
}

// A program's line into a line of the buffer
void line_into(const uint8_t *line, uint8_t *to)
{
    poke_w(to, 0);
    to[255] = 0;
    if (!line)
        return;
    Regs e;
    e.a[0] = address(line);
    lib_call(L_Tk_EditL, e);
    if (!e.d[0])
        to[255] = 0xFF;             // not editable
    Regs d;
    d.a[0] = address(line);
    d.a[1] = address(to);
    d.d[0] = 0;
    lib_call(L_Detok, d);
}

// Ttt_BufUntok / Ttt_Untok
void untokenise_all()
{
    MonData &m = mon();
    uint8_t *line = find_line(m.Ttt_YPos);
    for (uint16_t i = 0; i < m.Ttt_Tlh; i++) {
        if (i)
            line = next_line();
        line_into(line, buffer_line(i));
    }
}

void untokenise(uint16_t n, uint16_t into) { line_into(find_line(n), buffer_line(into)); }

void slider_horizontal()
{
    Regs r;
    r.d[0] = ChannelN;
    r.d[1] = 1;
    r.d[2] = mon().Ttt_XPos;
    r.d[3] = 78;
    r.d[4] = 250;
    lib_call(L_Dia_Update, r);
}

void slider_vertical()
{
    MonData &m = mon();
    Regs r;
    r.d[0] = ChannelN;
    r.d[1] = 2;
    r.d[2] = m.Ttt_YPos;
    r.d[3] = m.Ttt_Tlh;
    r.d[4] = program().Prg_NLigne;
    lib_call(L_Dia_Update, r);
}

// Ttt_ALigne / Ttt_EALigne: line n of the buffer, with the marks; erased
// to the end of the window if asked (and the line is shorter)
void show_line(uint16_t n, bool erase)
{
    MonData &m = mon();
    Regs l;
    l.d[1] = 0;
    l.d[2] = n;
    wi_call(wi::Locate, l);
    uint16_t y = uint16_t(n + m.Ttt_YPos);
    uint8_t *line = buffer_line(n);
    uint16_t length = peek_w(line);
    uint8_t *text = line + 2;
    uint16_t x = m.Ttt_XPos;
    bool attributes = false;
    if (x < length) {
        uint16_t end = uint16_t(x + 78);
        if (end <= length)
            erase = false;
        uint16_t last = end > length ? length : end;
        uint8_t *stop = text + last;
        uint8_t kept = *stop;
        *stop = 0;
        const uint8_t *from = text + x;
        if (y == m.Tt_YNext || y == m.Tt_YInfo || y == m.Tt_YMark) {
            uint8_t *to = ptr<uint8_t>(idata().Buffer);
            auto put = [&](const uint8_t *s) {
                while (*s)
                    *to++ = *s++;
            };
            for (uint16_t column = x;; column++) {
                if (y == m.Tt_YNext && column == m.Tt_XNext)
                    put(NextMark);
                if (y == m.Tt_YMark) {
                    if (column == m.Tt_X1Mark) {
                        put(InverseOn);
                        attributes = true;
                    }
                    if (column == m.Tt_X2Mark)
                        put(InverseOff);
                }
                if (y == m.Tt_YInfo) {
                    if (column == m.Tt_X1Info) {
                        put(UnderOn);
                        attributes = true;
                    }
                    if (column == m.Tt_X2Info)
                        put(UnderOff);
                }
                uint8_t c = *from++;
                *to++ = c;
                if (!c)
                    break;
            }
            from = ptr<uint8_t>(idata().Buffer);
        }
        print(from);
        *stop = kept;
    }
    if (attributes) {
        print(InverseOff);
        print(UnderOff);
    }
    if (erase)
        wcall(wi::ChrOut, 7);
}

} // namespace

namespace amos::monitor {

// Ttt_AffBuf / Ttt_NewBuf
void listing_show()
{
    MonData &m = mon();
    activate_text();
    for (uint16_t n = 0; n < m.Ttt_Tlh; n++)
        show_line(n, true);
    slider_vertical();
    slider_horizontal();
}

void listing_new()
{
    untokenise_all();
    listing_show();
}

// Ttt_Centre: the next instruction within the window
void listing_centre()
{
    MonData &m = mon();
    uint16_t first = uint16_t(m.Ttt_YPos + 1), last = uint16_t(m.Ttt_YPos + m.Ttt_Tlh - 1);
    uint16_t y = m.Tt_YNext;
    if (y >= first && y < last)
        return;
    int16_t top = int16_t(y - 1);
    if (top < 0)
        top = 0;
    uint16_t bottom = uint16_t(top + m.Ttt_Tlh);
    uint16_t lines = program().Prg_NLigne;
    if (bottom >= lines) {
        top = int16_t(top - (bottom - lines));
        if (top < 0)
            top = 0;
    }
    m.Ttt_YPos = uint16_t(top);
    untokenise_all();
}

// Ttt_SetSize: the listing window y high (the information window below)
void listing_size(uint16_t y)
{
    MonData &m = mon();
    m.Ttt_Tlh = uint16_t((y - 8) >> 3);
    m.Ttt_Tlb = uint16_t(((m.Ttt_Sy - 16) >> 3) - m.Ttt_Tlh);
}

// Ttt_Cls: the text screen drawn again (the dialogue of its background),
// its zones and windows; false if it failed
bool listing_screen()
{
    MonData &m = mon();
    wcall(wi::QWindow, 1);
    Regs d1;
    wi_call(wi::WinDel, d1);
    wcall(wi::QWindow, 2);
    Regs d2;
    wi_call(wi::WinDel, d2);
    uint8_t *v = ptr<uint8_t>(m.Tt_Variables);
    poke_w(v + 2, m.Ttt_Tlh);
    poke_w(v + 6, m.Ttt_Tlb);
    poke_w(v + 10, program().Prg_NLigne);
    poke_w(v + 14, 0);
    Regs r;
    r.d[0] = ChannelN;
    r.d[1] = 0xFFFFFFFF;
    r.d[2] = 0;
    r.d[3] = 0;
    lib_call(L_Dia_RunProgram, r);
    if (r.d[0])
        return false;
    auto zone = [](uint16_t n, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
        Regs z;
        z.d[1] = n;
        sy_call(sy::RazZone, z);
        Regs s;
        s.d[1] = n;
        s.d[2] = x1;
        s.d[3] = y1;
        s.d[4] = x2;
        s.d[5] = y2;
        sy_call(sy::SetZone, s);
    };
    uint16_t y = uint16_t((m.Ttt_Tlh << 3) + 8);
    zone(1, 0, 8, 640 - 16, y);
    zone(2, 0, y, 640, uint16_t(y + 4));
    zone(3, 0, uint16_t(y + 4), 640 - 16, uint16_t(m.Ttt_Sy - 8));
    auto open = [](uint16_t n, uint16_t wy, uint16_t lines) {
        Regs w;
        w.d[1] = n;
        w.d[2] = 0;
        w.d[3] = wy;
        w.d[4] = E2Sx / 8 - 2;
        w.d[5] = lines;
        w.d[6] = 0;
        w.d[7] = 0;
        w.a[1] = 0;
        wi_call(wi::WindOp, w);
        if (w.d[0])
            return false;
        print(WindowInit);
        return true;
    };
    return open(1, 7, m.Ttt_Tlh) && open(2, uint16_t((m.Ttt_Tlh << 3) + 7 + 5), m.Ttt_Tlb);
}

// Tt_Decoup: line n split into its parts
void split(uint16_t n)
{
    MonData &m = mon();
    find_line(n);
    m.Tt_ALine = m.Tt_CurLigne;
    m.Tt_PLine = m.Tt_Ligne;
    poke_l(ptr<uint8_t>(m.Tt_Ligne - 4), 0x3A2D2D2D);      // ":---"
    Regs d;
    d.a[0] = m.Tt_CurLigne;
    d.a[1] = m.Tt_BufT;
    d.d[0] = uint32_t(-int32_t(address(mon_split_hook)));
    lib_call(L_Detok, d);
}

// Tt_MouMou: the column and line of the program under the mouse (*row
// the window's line); in the >>> mark, moved off it
struct Position {
    uint16_t x, y, row;
};

Position mouse_position()
{
    MonData &m = mon();
    int16_t mx, my;
    mouse_screen(mx, my);
    uint16_t row = uint16_t(uint16_t(my - 8) >> 3);
    uint16_t x = uint16_t((uint16_t(mx) >> 3) + m.Ttt_XPos);
    uint16_t y = uint16_t(row + m.Ttt_YPos);
    if (y == m.Tt_YNext && x >= m.Tt_XNext) {
        uint16_t d = uint16_t(m.Tt_XNext + 3 - x);
        if (int16_t(d) <= 0 || d == 0)
            x = uint16_t(x - 3);
        else
            x = uint16_t(x - d);
    }
    return {x, y, row};
}

// Tt_Souligne: an expression chosen by dragging (underlined); its text and
// length, 0 if none
Chosen choose(uint16_t x, uint16_t y, uint16_t row)
{
    MonData &m = mon();
    uint8_t *line = buffer_line(row);
    if (x >= peek_w(line))
        return {0, nullptr, y, x};
    m.Tt_X1Info = x;
    m.Tt_X2Info = uint16_t(x + 1);
    m.Tt_YInfo = y;
    listing_show();
    while (mouse_keys() & 1) {
        boot::wait_vbl_multi();
        if (!(mouse_keys() & 1))
            break;
        Position p = mouse_position();
        uint16_t end = uint16_t(p.x + 1);
        if (end <= m.Tt_X1Info || end > peek_w(line))
            continue;
        m.Tt_X2Info = end;
        show_line(uint16_t(m.Tt_YInfo - m.Ttt_YPos), true);
    }
    int16_t length = int16_t(m.Tt_X2Info - m.Tt_X1Info);
    if (length <= 0)
        return {0, nullptr, m.Tt_YInfo, m.Tt_X1Info};
    return {uint16_t(length), line + 2 + m.Tt_X1Info, m.Tt_YInfo, m.Tt_X1Info};
}

} // namespace amos::monitor

namespace {

// Ttt_Slvclic: the vertical slider moved (one line scrolled)
void slider_vertical_click()
{
    MonData &m = mon();
    Regs v;
    v.d[0] = ChannelN;
    v.d[1] = 2;
    v.d[2] = 0;
    lib_call(L_Dia_GetValue, v);
    int16_t moved = int16_t(m.Ttt_YPos - uint16_t(v.d[1]));
    m.Ttt_YPos = uint16_t(v.d[1]);
    uint16_t lines = m.Ttt_Tlh;
    uint8_t *b = ptr<uint8_t>(m.Ttt_BufE);
    if (moved == 1) {
        print(ScrollDown);
        for (int32_t i = int32_t(lines - 1) * 256 - 1; i >= 0; i--)
            b[256 + i] = b[i];
        untokenise(m.Ttt_YPos, 0);
        show_line(0, false);
    } else if (moved == -1) {
        scroll_up(lines);
        for (uint32_t i = 0; i < uint32_t(lines - 1) * 256; i++)
            b[i] = b[256 + i];
        untokenise(uint16_t(m.Ttt_YPos + lines - 1), uint16_t(lines - 1));
        show_line(uint16_t(lines - 1), false);
    } else {
        listing_new();
    }
}

// Ttt_Slhclic
void slider_horizontal_click()
{
    Regs v;
    v.d[0] = ChannelN;
    v.d[1] = 1;
    v.d[2] = 0;
    lib_call(L_Dia_GetValue, v);
    mon().Ttt_XPos = uint16_t(v.d[1]);
    listing_show();
}

// NoInit: "not initialised"
void no_init()
{
    alert(message(11));
    activate_info();
    info_show_all();
    wait_release();
}

// Ttt_CText: the window on the next instruction
void centre_click()
{
    if (int16_t(mon().Tt_YNext) < 0)
        return no_init();
    listing_centre();
    listing_show();
}

// Ttt_Separ: the separation between the windows dragged (the bar drawn
// with screen blocks 126 and 127)
void separation_drag()
{
    MonData &m = mon();
    IData &d = idata();
    int16_t mx, my;
    mouse_screen(mx, my);
    uint16_t at = uint16_t((m.Ttt_Tlh << 3) + 8);
    int16_t offset = int16_t(my - at);
    uint16_t now = at, old = at;
    auto get = [&](uint16_t block, uint16_t y) {
        Regs r;
        r.d[1] = block;
        r.d[2] = 0;
        r.d[3] = uint32_t(int32_t(int16_t(y - 1)));
        r.d[4] = 640;
        r.d[5] = 5;
        r.d[6] = 0;
        r.a[1] = d.Buffer;
        ec_call(ec::BlGet, r);
    };
    auto put = [&](uint16_t block, uint16_t y) {
        Regs r;
        r.d[1] = block;
        r.d[2] = 0;
        r.d[3] = uint32_t(int32_t(int16_t(y - 1)));
        r.d[4] = r.d[5] = EntNul;
        r.a[1] = d.Buffer;
        ec_call(ec::BlPut, r);
    };
    get(126, now);
    do {
        get(127, now);
        old = now;
        put(126, now);
        while (mouse_keys() & 1) {
            boot::wait_vbl_multi();
            if (!(mouse_keys() & 1))
                break;
            mouse_screen(mx, my);
            int16_t y = int16_t(my - offset);
            if (uint16_t(y) == now || uint16_t(y) < 16 || uint16_t(y) >= uint16_t(m.Ttt_Sy - (16 - 4)))
                continue;
            now = uint16_t(y);
            break;
        }
        put(127, old);
    } while (mouse_keys() & 1);
    Regs b1;
    b1.d[1] = 127;
    ec_call(ec::BlDel, b1);
    Regs b2;
    b2.d[1] = 126;
    ec_call(ec::BlDel, b2);
    listing_size(now);
    listing_screen();
    listing_new();
    info_centre();
}

// Ttt_ListClic .Break: the breakpoint on the instruction clicked (or none)
void set_breakpoint(Position p)
{
    MonData &m = mon();
    m.Tt_YMark = 0xFFFF;
    m.Step_Break = 0;
    split(p.y);
    const uint8_t *a0 = ptr<uint8_t>(m.Tt_Ligne);
    uint8_t x = uint8_t(p.x);
    if (a0[0] && x >= a0[1]) {
        const uint8_t *a1 = a0;
        bool found = false;
        for (;;) {
            a0 += 4;
            uint8_t c = a0[0];
            if (!c) {
                found = x < a0[1];
                break;
            }
            if (c == ':' || c == 'W' || c == 'X' || c == 'I') {
                if (x < a0[1]) {
                    found = true;
                    break;
                }
                a1 = a0;
            }
        }
        if (found && a0 != a1 && a1[0] != ':') {
            m.Tt_YMark = p.y;
            m.Tt_X1Mark = a1[1];
            m.Tt_X2Mark = a0[1];
            m.Step_Break = m.Tt_ALine + peek_w(a1 + 2);
        }
    }
    listing_show();
    wait_release();
}

// .expout and the alerts of the evaluation
void evaluation_end(const uint8_t *alert_text)
{
    MonData &m = mon();
    m.Tt_YInfo = 0xFFFF;
    if (alert_text)
        alert(alert_text);
    listing_show();
    activate_info();
    info_show_all();
    info_slider();
}

// Ttt_ListClic .Eval: the expression dragged over evaluated, kept in the
// information window
void evaluate_click(Position p)
{
    MonData &m = mon();
    Chosen c = choose(p.x, p.y, p.row);
    if (!c.length)
        return evaluation_end(message(8));
    // (the brackets balanced; the quotes were meant to be checked too)
    int16_t brackets = 0;
    for (uint16_t i = 0; i < c.length; i++) {
        if (c.text[i] == '(')
            brackets++;
        if (c.text[i] == ')')
            brackets--;
    }
    if (brackets)
        return evaluation_end(message(8));
    split(c.y);
    const uint8_t *a0 = ptr<uint8_t>(m.Tt_Ligne);
    uint8_t x = uint8_t(c.x);
    if (!a0[0] || x < a0[1])
        return evaluation_end(message(8));
    // the part the expression starts in, the part after its end
    const uint8_t *a1;
    for (;;) {
        a1 = a0;
        if (a1[1] == x)
            break;
        a0 += 4;
        if (x < a0[1])
            break;
        if (!a0[0])
            return evaluation_end(message(8));
    }
    const uint8_t *a2 = a1;
    do
        a2 += 4;
    while (a2[0] && uint8_t(m.Tt_X2Info) > a2[1]);
    uint8_t kind = a1[0];
    if (kind == ':' || kind == 'W' || a1[-4] == ':')
        return evaluation_end(message(8));
    if (!m.Tt_Init)
        return no_init();
    if (kind != '-' && kind != 'V' && (kind < '0' || kind > '2'))
        return evaluation_end(message(8));
    // its tokens
    const uint8_t *line = ptr<uint8_t>(m.Tt_ALine);
    uint16_t first = peek_w(line + 2);
    if (first == tk::Glo || first == tk::Sha)
        return evaluation_end(message(8));
    const uint8_t *from = line + peek_w(a1 + 2), *to = line + peek_w(a2 + 2);
    uint8_t *t = ptr<uint8_t>(m.Tt_BufT);
    do {
        poke_w(t, peek_w(from));
        t += 2;
        from += 2;
    } while (from < to);
    if (peek_w(from) == tk::Par1)
        return evaluation_end(message(8));
    poke_w(t, 0);
    t += 2;
    uint16_t size = uint16_t(t - ptr<uint8_t>(m.Tt_BufT));
    // the same procedure as the next instruction
    const uint8_t *l1 = ptr<uint8_t>(m.Tt_DCurLine), *l2 = line;
    if (l2 < l1) {
        const uint8_t *s = l1;
        l1 = l2;
        l2 = s;
    }
    while (l1 <= l2) {
        uint16_t token = peek_w(l1 + 2);
        if (token == tk::Proc || token == tk::EndP)
            return evaluation_end(message(6));
        l1 += l1[0] * 2;
    }
    if (m.Inf_Pos - m.Inf_Buf >= InfSize - InfSize / 3)
        return evaluation_end(message(9));
    if (evaluate(m.Tt_BufT).error)
        return evaluation_end(message(8));
    uint8_t *e = info_insert(uint16_t(size + 12));
    poke_w(e, 3);
    poke_w(e + 2, 0xFFFF);
    poke_l(e + 4, idata().BasA3);
    const uint8_t *src = ptr<uint8_t>(m.Tt_BufT);
    for (uint16_t i = 0; i < size; i += 2)
        poke_w(e + 8 + i, peek_w(src + i));
    evaluation_end(nullptr);
}

// Ttt_ListClic: a click in the listing: help, a breakpoint (or the right
// button) or an expression, as the mode
void listing_click()
{
    MonData &m = mon();
    Position p = mouse_position();
    if (p.y >= program().Prg_NLigne) {
        listing_show();
        wait_release();
        return;
    }
    if (m.Tt_MKey == 2 || m.Ttt_Mode == 1)
        return set_breakpoint(p);
    if (!m.Ttt_Mode) {
        return run_help(p.x, p.y, p.row);
    }
    evaluate_click(p);
}

} // namespace

extern "C" {
void mon_no_init(AllRegs *) { no_init(); }
void mon_listing_show(AllRegs *) { listing_show(); }
void mon_listing_new(AllRegs *) { listing_new(); }
void mon_listing_centre(AllRegs *) { listing_centre(); }
void mon_listing_untokenise(AllRegs *r) { untokenise(uint16_t(r->d[0]), uint16_t(r->d[1])); }
void mon_listing_size(AllRegs *r) { listing_size(uint16_t(r->d[0])); }
void mon_listing_screen(AllRegs *r) { r->d[0] = listing_screen() ? 0 : 1; }
void mon_split(AllRegs *r) { split(uint16_t(r->d[0])); }
void mon_slider_vertical_click(AllRegs *) { slider_vertical_click(); }
void mon_slider_horizontal_click(AllRegs *) { slider_horizontal_click(); }
void mon_centre_click(AllRegs *) { centre_click(); }
void mon_separation_drag(AllRegs *) { separation_drag(); }
void mon_listing_click(AllRegs *) { listing_click(); }

// Tt_MouMou: d1 the column, d2 the line, d4 the window's line
void mon_mouse_position(AllRegs *r)
{
    Position p = mouse_position();
    r->d[1] = (r->d[1] & 0xFFFF0000) | p.x;
    r->d[2] = (r->d[2] & 0xFFFF0000) | p.y;
    r->d[4] = (r->d[4] & 0xFFFF0000) | p.row;
}

// Tt_Souligne: d1 the column, d2 the line, d4 the window's line; d0 the
// length (0: none), a0 the text, d1/d2 the start
void mon_choose(AllRegs *r)
{
    Chosen c = choose(uint16_t(r->d[1]), uint16_t(r->d[2]), uint16_t(r->d[4]));
    r->d[0] = c.length;
    if (c.length) {
        r->a[0] = address(c.text);
        r->d[1] = (r->d[1] & 0xFFFF0000) | c.x;
        r->d[2] = (r->d[2] & 0xFFFF0000) | c.y;
    }
}
}
