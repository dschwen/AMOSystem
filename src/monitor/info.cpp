// The monitor's information window (+Monitor.s Inf_*, Ttt_Alert...), its
// messages (Tt_Message) and small helpers: the screens activated, the
// banks, the mouse.
//
// The information buffer (Inf_Buf, up to Inf_Pos): its lines, each a word
// with its size (even), a word with its kind, a word with its line in the
// window (-1 if not shown), then its text. The kinds: 0 an alert, 1 and 2
// information (2 separates them from the rest, shown only after a 1),
// 3 an expression (a long, BasA3 when it was made, then its tokens), 4 a
// separation shown after a 3, 5 the next instruction and its parameters.

#include "monitor/monitor.h"

using namespace amos;
using namespace amos::monitor;

// +Monitor.s


namespace amos::monitor {

const uint8_t ScrollUp[] = {27, 'Y', '0', 22, 0};
const uint8_t ScrollDown[] = {12, 20, 0};
const uint8_t InverseOn[] = {27, 'I', '1', 0};
const uint8_t InverseOff[] = {27, 'I', '0', 0};
const uint8_t UnderOn[] = {27, 'U', '1', 0};
const uint8_t UnderOff[] = {27, 'U', '0', 0};
const uint8_t NextMark[] = {27, 'B', '0', 27, 'P', '1', '>', '>', '>', 27, 'B', '2', 27, 'P', '3', 0};

} // namespace amos::monitor

namespace {

constexpr uint32_t InfSize = 88 * 100;
constexpr uint32_t EntNul = 0x80000000;
const uint8_t Shade[] = {27, 'S', '1', 0};
const uint8_t ShadeOff[] = {27, 'S', '0', 0};
const uint8_t Mark[] = ">>> ";

void print(const uint8_t *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

void locate(int16_t x, int16_t y)
{
    Regs r;
    r.d[1] = uint32_t(int32_t(x));
    r.d[2] = uint32_t(int32_t(y));
    wi_call(wi::Locate, r);
}

void clear_to_end()
{
    Regs r;
    r.d[1] = 7;
    wi_call(wi::ChrOut, r);
}

uint8_t *buffer() { return ptr<uint8_t>(mon().Inf_Buf); }

// The info lines shown (ginf): from Inf_YPos, all of them or only the
// window's line only (>= 0); counting only fills Inf_NLigne
struct Show {
    bool count;             // bit 31 of d6
    int16_t only;           // d3
    uint16_t top, bottom;   // d6, d7
    bool info, expressions; // bits 31 and 30 of d7: a 1, a 3 shown
    uint16_t n;             // d5
};

// Iparam: an expression and its value
void show_expression(Show &s, uint8_t *line)
{
    MonData &m = mon();
    if (s.count)
        return;
    bool invalid = false;
    if (!m.Tt_Init || idata().BasA3 != peek_l(line + 6)) {
        print(Shade);
        invalid = true;
    }
    uint8_t *text = ptr<uint8_t>(m.Tt_BufT);
    Regs d;
    d.a[0] = address(line + 10);
    d.a[1] = address(text);
    d.d[0] = 0;
    lib_call(L_Mon_Detok, d);
    uint16_t length = peek_w(text);
    if (length >= 60)
        length = 60;
    uint8_t *to = text + 2 + length;
    if (!invalid) {
        Value v = evaluate(address(line + 10));
        if (!v.error) {
            *to++ = ':';
            *to++ = ' ';
            to = print_value(to, v, uint16_t(72 - length), 13);
        }
    }
    if (invalid)
        for (const uint8_t *c = ShadeOff; *c;)
            *to++ = *c++;
    *to++ = 7;
    *to = 0;
    print(text + 2);
}

// InfA: one line at the window's line y
void show_line(Show &s, uint8_t *line, int16_t y)
{
    locate(0, y);
    uint16_t kind = peek_w(line + 2);
    if (kind == 3)
        return show_expression(s, line);
    if (kind == 2 || kind == 4) {
        // a separation, only after what it separates
        bool &shown = kind == 2 ? s.info : s.expressions;
        bool was = shown;
        shown = false;
        if (!was) {
            poke_w(line + 4, 0xFFFF);
            s.n--;
            return;
        }
    }
    if (!s.count)
        print(line + 6);
}

// InfAf: the lines of one kind
void show_kind(Show &s, uint16_t kind)
{
    for (uint8_t *line = buffer(); peek_w(line); line += peek_w(line)) {
        if (peek_w(line + 2) != kind)
            continue;
        poke_w(line + 4, 0xFFFF);
        if (kind == 1)
            s.info = true;
        if (kind == 3)
            s.expressions = true;
        if (s.n >= s.top && s.n < s.bottom) {
            int16_t y = int16_t(s.n - s.top);
            if (s.only < 0 || y == s.only) {
                poke_w(line + 4, uint16_t(y));
                show_line(s, line, y);
            }
        }
        s.n++;
    }
}

void show(bool count, int16_t only)
{
    MonData &m = mon();
    Show s = {count, only, m.Inf_YPos, uint16_t(m.Inf_YPos + m.Ttt_Tlb), false, false, 0};
    for (uint16_t kind = 0; kind < 6; kind++)
        show_kind(s, kind);
    m.Inf_NLigne = s.n;
    if (count)
        return;
    for (; s.n < s.bottom; s.n++) {
        locate(0, int16_t(s.n - s.top));
        clear_to_end();
    }
}

} // namespace

namespace amos::monitor {

// Tt_Message: message n of the monitor's resource (GetMessage's d0 in
// *d0)
const uint8_t *message(uint16_t n, uint32_t *d0)
{
    const uint8_t *resource = ptr<uint8_t>(mon().Tt_Resource);
    Regs r;
    r.d[0] = n;
    r.a[0] = address(resource + peek_l(resource + 2 + 4));
    r.a[0] = address(boot::get_message(ptr<uint8_t>(r.a[0]), r.d[0]));
    if (d0)
        *d0 = r.d[0];
    return ptr<uint8_t>(r.a[0]);
}

// The window scrolled up, its last line (of lines) cleared
void scroll_up(uint16_t lines)
{
    uint8_t up[sizeof(ScrollUp)];
    for (unsigned i = 0; i < sizeof(ScrollUp); i++)
        up[i] = ScrollUp[i];
    up[2] = uint8_t('0' + lines - 1);
    print(up);
}

// Tt_TtBanks / Tt_PrgBanks: the monitor's banks and dialogues, the
// program's
void banks_monitor()
{
    IData &d = idata();
    d.Cur_Banks = address(&d.Mon_Banks);
    d.Cur_Dialogs = address(&mon().Tt_Dialogs);
}

void banks_program()
{
    IData &d = idata();
    d.Cur_Banks = mon().Tt_CurBanks;
    d.Cur_Dialogs = mon().Tt_CurDialogs;
}

// Tt_EcAct / Ttt_Act / View_Act; Ttt_TttAct / Ttt_InfAct: the windows of
// the text screen
void activate(uint16_t screen)
{
    MonData &m = mon();
    if (m.Tt_Ecran == screen)
        return;
    m.Tt_Ecran = screen;
    Regs r;
    r.d[1] = screen;
    ec_call(ec::Active, r);
}

static void window(uint16_t n)
{
    activate(Ecran2);
    Regs r;
    r.d[1] = n;
    wi_call(wi::QWindow, r);
}

void activate_text() { window(1); }
void activate_info() { window(2); }

// Tt_MBout / Wmky / XYMouEc
uint32_t mouse_keys()
{
    Regs r;
    sy_call(sy::MouseKey, r);
    return r.d[1];
}

void wait_release()
{
    while (mouse_keys() & 1) {
        boot::wait_vbl_multi();
    }
}

void mouse_screen(int16_t &x, int16_t &y)
{
    Regs r;
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    sy_call(sy::XyScr, r);
    x = int16_t(r.d[1]);
    y = int16_t(r.d[2]);
}

// Inf_PrDeb / Inf_Print / Inf_PrFin: a new line of the buffer, of kind
void info_begin(uint16_t kind)
{
    uint8_t *line = ptr<uint8_t>(mon().Inf_Pos);
    poke_w(line, 0);
    poke_w(line + 2, kind);
    poke_w(line + 4, 0xFFFF);
}

void info_print(const uint8_t *text)
{
    uint8_t *line = ptr<uint8_t>(mon().Inf_Pos);
    uint16_t n = peek_w(line);
    uint8_t *to = line + 6 + n;
    for (;;) {
        if (uint8_t(n) >= 76)
            break;
        uint8_t c = *text++;
        *to++ = c;
        if (!c)
            break;
        n++;
    }
    *to = 0;
    poke_w(line, n);
}

void info_end()
{
    MonData &m = mon();
    uint8_t *line = ptr<uint8_t>(m.Inf_Pos);
    uint16_t n = peek_w(line);
    if (n < 76) {
        line[6 + n] = 7;
        line[7 + n] = 0;
        n++;
    }
    n = uint16_t((n + 8) & 0xFFFE);
    poke_w(line, n);
    line += n;
    m.Inf_Pos = address(line);
    poke_w(line, 0);
}

// Inf_Del: the first lines of kind (one after the other) out; 1, or 0 if
// there were none
uint16_t info_delete(uint16_t kind)
{
    MonData &m = mon();
    uint8_t *a0 = buffer();
    uint32_t end = m.Inf_Pos;
    while (address(a0) < end && peek_w(a0 + 2) != kind)
        a0 += peek_w(a0);
    if (address(a0) >= end)
        return 0;
    uint16_t gone = 0;
    uint8_t *a1 = a0;
    do {
        gone = uint16_t(gone + peek_w(a1));
        a1 = a0 + gone;
    } while (address(a1) < end && peek_w(a1 + 2) == kind);
    end -= gone;
    for (uint8_t *to = a0; address(to) <= end; to += 2, a1 += 2)
        poke_w(to, peek_w(a1));
    m.Inf_Pos = end;
    return 1;
}

// Inf_Ins: size bytes at the start of the buffer (the first line's header
// made empty); the start
uint8_t *info_insert(uint16_t size)
{
    MonData &m = mon();
    uint8_t *start = buffer();
    uint8_t *a1 = ptr<uint8_t>(m.Inf_Pos);
    uint8_t *a2 = a1 + size;
    m.Inf_Pos = address(a2);
    poke_w(a2, 0);
    do {
        a1 -= 2;
        a2 -= 2;
        poke_w(a2, peek_w(a1));
    } while (a1 >= start);
    poke_w(start, size);
    poke_l(start + 2, 0);
    poke_w(start + 6, 0);
    return start + 2;
}

// Inf_AffAll / Inf_CptLines
void info_show_all() { show(false, -1); }
void info_count_lines() { show(true, -1); }

// Inf_Slider
void info_slider()
{
    MonData &m = mon();
    Regs r;
    r.d[0] = ChannelN;
    r.d[1] = 3;
    r.d[2] = m.Inf_YPos;
    r.d[3] = m.Ttt_Tlb;
    r.d[4] = m.Inf_NLigne;
    lib_call(L_Dia_Update, r);
}

// Inf_Centre: the lines kept within the window
void info_centre()
{
    MonData &m = mon();
    info_count_lines();
    uint16_t bottom = uint16_t(m.Inf_YPos + m.Ttt_Tlb);
    if (m.Inf_NLigne < bottom) {
        int16_t y = int16_t(m.Inf_YPos - (bottom - m.Inf_NLigne));
        m.Inf_YPos = uint16_t(y < 0 ? 0 : y);
    }
    activate_info();
    info_show_all();
    info_slider();
}

// Ttt_Alert: text as the alert, at the top
void alert(const uint8_t *text)
{
    MonData &m = mon();
    info_delete(0);
    m.Inf_AlertOn++;
    m.Inf_YPos = 0;
    info_begin(0);
    info_print(Mark);
    info_print(text);
    info_end();
    // (the assembler added a separation line, message 5, of a kind
    // nothing shows or deletes: its length, left in d0. The buffer filled
    // up with them.)
}

// Ttt_AlertDel
void alert_delete()
{
    MonData &m = mon();
    if (!m.Inf_AlertOn)
        return;
    m.Inf_AlertOn = 0;
    info_delete(0);
    activate_info();
    info_show_all();
    info_slider();
}

// Ttt_DelAff
void delete_show(uint16_t kind)
{
    if (!info_delete(kind))
        return;
    activate_info();
    info_show_all();
    info_slider();
}

// Tt_Evalue: the expression at tokens
Value evaluate(uint32_t tokens)
{
    AllRegs r = {};
    call_a6(mon_evaluate, r, tokens);
    return {r.d[0] != 0, uint8_t(r.d[2]), r.d[3], r.a[6]};
}

// A value as text at to: a string in quotes, up to room characters (a >
// then); an undefined integer as message undefined; the end
uint8_t *print_value(uint8_t *to, const Value &v, uint16_t room, uint16_t undefined)
{
    if (v.type == 0) {
        if (v.value == EntNul) {
            for (const uint8_t *c = message(undefined); *c;)
                *to++ = *c++;
            return to;
        }
        Regs r;
        r.d[0] = v.value;
        r.a[0] = address(to);
        lib_call(L_LongToDec, r);
        return ptr<uint8_t>(r.a[0]);
    }
    if (v.type == 1) {
        Regs r;
        r.d[3] = v.value;
        r.a[0] = address(to);
        lib_call(L_Float2Ascii, r);
        return ptr<uint8_t>(r.a[0]);
    }
    uint8_t close = '"';
    *to++ = '"';
    const uint8_t *s = ptr<uint8_t>(v.value);
    uint16_t n = peek_w(s);
    s += 2;
    if (n >= room) {
        n = room;
        close = '>';
    }
    for (uint16_t i = 0; i < n; i++) {
        uint8_t c = *s++;
        *to++ = c >= 32 ? c : '.';
    }
    *to++ = close;
    return to;
}

} // namespace amos::monitor

namespace {

// Inf_Sliclic: the information slider moved
void info_slider_click()
{
    MonData &m = mon();
    activate_info();
    Regs v;
    v.d[0] = ChannelN;
    v.d[1] = 3;
    v.d[2] = 0;
    lib_call(L_Dia_GetValue, v);
    int16_t moved = int16_t(m.Inf_YPos - uint16_t(v.d[1]));
    m.Inf_YPos = uint16_t(v.d[1]);
    if (moved == 1) {
        print(ScrollDown);
        show(false, 0);
    } else if (moved == -1) {
        scroll_up(m.Ttt_Tlb);
        show(false, int16_t(m.Ttt_Tlb - 1));
    } else {
        info_show_all();
    }
}

// Inf_Clic: a click on a line: an expression taken away, or all the
// information
void info_click()
{
    MonData &m = mon();
    int16_t x, y;
    mouse_screen(x, y);
    int16_t line = int16_t((uint16_t(y) >> 3) - m.Ttt_Tlh - 2);
    for (uint8_t *l = buffer(); peek_w(l); l += peek_w(l)) {
        if (int16_t(peek_w(l + 4)) != line)
            continue;
        uint16_t kind = peek_w(l + 2);
        if (kind != 3 && kind != 1)
            break;
        // the line marked as the one to delete
        uint16_t gone = kind == 3 ? 100 : 1;
        poke_w(l + 2, gone);
        info_delete(gone);
        info_centre();
        break;
    }
    wait_release();
}

} // namespace

extern "C" {
void mon_message(AllRegs *r)
{
    uint32_t d0 = r->d[0];
    r->a[0] = address(message(uint16_t(r->d[0]), &d0));
    r->d[0] = d0;
}
void mon_banks_monitor(AllRegs *) { banks_monitor(); }
void mon_banks_program(AllRegs *) { banks_program(); }
void mon_activate(AllRegs *r) { activate(uint16_t(r->d[1])); }
void mon_activate_text(AllRegs *) { activate_text(); }
void mon_activate_info(AllRegs *) { activate_info(); }
void mon_wait_release(AllRegs *) { wait_release(); }
void mon_info_begin(AllRegs *r) { info_begin(uint16_t(r->d[0])); }
void mon_info_print(AllRegs *r) { info_print(ptr<uint8_t>(r->a[0])); }
void mon_info_end(AllRegs *) { info_end(); }
void mon_info_delete(AllRegs *r) { r->d[0] = info_delete(uint16_t(r->d[0])); }
void mon_info_insert(AllRegs *r) { r->a[0] = address(info_insert(uint16_t(r->d[0]))); }
void mon_info_show_all(AllRegs *) { info_show_all(); }
void mon_info_count_lines(AllRegs *) { info_count_lines(); }
void mon_info_show_line(AllRegs *r) { show(false, int16_t(r->d[0])); }
void mon_info_slider(AllRegs *) { info_slider(); }
void mon_info_slider_click(AllRegs *) { info_slider_click(); }
void mon_info_click(AllRegs *) { info_click(); }
void mon_info_centre(AllRegs *) { info_centre(); }
void mon_alert(AllRegs *r) { alert(ptr<uint8_t>(r->a[0])); }
void mon_alert_delete(AllRegs *) { alert_delete(); }
void mon_delete_show(AllRegs *r) { delete_show(uint16_t(r->d[0])); }
}
