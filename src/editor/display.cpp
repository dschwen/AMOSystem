// The editor's display of its window (+Edit.s "ROUTINES AFFICHAGE /
// POSITIONNEMENT FENETRES"): the lines of Edt_BufE written through
// AMOS.library's text window functions, the marked block in inverse; the
// cursor placed, the window moved to keep it in sight.
//
// Edt_XPos/Edt_YPos: the column and line of the program at the window's
// corner; Edt_XCu/Edt_YCu: the cursor in the window (Edt_XCu counts from
// the start of the line).

#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_cursor_on_text[], ed_cursor_off_text[], ed_get_systeme[], ed_all_aver_fin[],
    ed_aver_fin[], ed_sl_update_y[];

namespace {

constexpr uint8_t EtAX = 1 << 2, EtAY = 1 << 3, EtABAll = 0x7F;
constexpr uint8_t SlDelai = 10;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void print(const uint8_t *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

void chr_out(uint8_t c)
{
    Regs r;
    r.d[1] = c;
    wi_call(wi::ChrOut, r);
}

void locate(int32_t x, int32_t y)
{
    Regs r;
    r.d[1] = uint32_t(x);
    r.d[2] = uint32_t(y);
    wi_call(wi::Locate, r);
}

// Ed_GetSysteme: system message n (17/18: inverse on/off)
const uint8_t *system_text(uint16_t n)
{
    Regs r;
    r.d[0] = n;
    call(ed_get_systeme, r);
    return ptr<uint8_t>(r.a[0]);
}

void inverse() { print(system_text(17)); }
void normal() { print(system_text(18)); }

// .Print: n characters of text
void print_part(uint8_t *text, uint16_t n)
{
    uint8_t kept = text[n];
    text[n] = 0;
    print(text);
    text[n] = kept;
}

uint8_t *buffer_line(uint16_t y) { return ptr<uint8_t>(window().Edt_BufE) + int16_t(uint16_t(y << 8)); }

// The marked block: from line y1 column x1 to line y2 column x2 (the
// cursor is one of its ends). False if there is none.
struct Block {
    uint16_t y1, y2, x1, x2;
};

bool block(Block &b)
{
    EditWindow &w = window();
    if (int16_t(w.Edt_YBloc) < 0)
        return false;
    uint16_t d0 = w.Edt_YBloc, d1 = w.Edt_YPos + w.Edt_YCu, d2 = w.Edt_XBloc, d3 = w.Edt_XCu;
    bool swap = d1 < d0;
    if (d1 == d0) {
        if (d3 == d2)
            return false;
        swap = d3 < d2;
    }
    if (swap) {
        uint16_t t = d0;
        d0 = d1;
        d1 = t;
        t = d2;
        d2 = d3;
        d3 = t;
    }
    b = {d0, d1, d2, d3};
    return true;
}

} // namespace

namespace amos::ed {

// Ed_Loca: the cursor in the window
void place_cursor()
{
    EditWindow &w = window();
    locate(int16_t(w.Edt_XCu - w.Edt_XPos), int16_t(w.Edt_YCu));
}

// Ed_EALigne / Ed_ALigne: line y of the window, the rest of the window's
// line erased or not
void show_line(uint16_t y, bool erase)
{
    EditWindow &w = window();
    locate(0, int16_t(y));
    uint8_t *a2 = buffer_line(y);
    uint16_t d4 = w.Edt_XPos;
    uint16_t length = peek_w(a2);
    a2 += 2;
    if (d4 < length) {
        a2 += d4;
        uint16_t d5 = d4 + w.Edt_WindTx;
        if (d5 > length)
            d5 = length;
        if (uint16_t(d5 - d4) >= w.Edt_WindTx)
            erase = false;          // the line fills the window
        uint16_t d6 = y + w.Edt_YPos;
        Block b;
        if (!block(b))
            goto NoBloc;
        if (b.x2 > d5)
            b.x2 = d5;
        if (d6 < b.y1)
            goto NoBloc;
        if (d6 == b.y1)
            goto DBloc;
        if (d6 > b.y2)
            goto NoBloc;
        if (d6 == b.y2)
            goto EBloc;
        // inside the block: all of it in inverse
        inverse();
        print_part(a2, d5 - d4);
        normal();
        goto End;
    DEBloc:
        // a block on one line
        if (b.x1 >= d5)
            goto NoBloc;
        if (b.x1 > d4) {
            uint16_t n = b.x1 - d4;
            print_part(a2, n);
            a2 += n;
            d4 += n;
        }
        inverse();
        if (b.x2 > d4) {
            uint16_t n = b.x2 - d4;
            print_part(a2, n);
            d4 += n;
            a2 += n;
        }
        normal();
        goto NoBloc;
    DBloc:
        // its first line: inverse from x1
        if (d6 == b.y2)
            goto DEBloc;
        if (b.x1 >= d5)
            goto NoBloc;
        if (b.x1 > d4) {
            uint16_t n = b.x1 - d4;
            print_part(a2, n);
            a2 += n;
            d4 += n;
        }
        inverse();
        if (d5 > d4)
            print_part(a2, d5 - d4);
        normal();
        goto End;
    EBloc:
        // its last line: inverse up to x2
        if (d6 == b.y1)
            goto DEBloc;
        inverse();
        if (b.x2 > d4) {
            uint16_t n = b.x2 - d4;
            print_part(a2, n);
            a2 += n;
            d4 += n;
        }
        normal();
    NoBloc:
        if (d5 > d4)
            print_part(a2, d5 - d4);
    }
End:
    if (erase)
        chr_out(7);
}

// Ed_AffBuf: all the lines of the window
void show_all()
{
    run(ed_all_aver_fin);
    for (uint16_t y = 0; y < window().Edt_WindTy; y++)
        show_line(y, true);
    place_cursor();
}

// Ed_NewBuf / Ed_NewBufAff: the window's lines detokenised again and
// shown (and all its information)
void new_buffer()
{
    Regs r;
    r.d[1] = window().Edt_Window;
    wi_call(wi::QWindow, r);
    untokenise_window();
    show_all();
}

void new_buffer_all()
{
    window().Edt_EtatAff = EtABAll;
    window().Edt_ASlY = SlDelai;
    new_buffer();
}

// Ed_EALiCu: the cursor's line
void show_current_line()
{
    show_line(window().Edt_YCu, true);
    place_cursor();
}

// Ed_Cent: the window moved sideways if the cursor is out of it
void centre()
{
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAX;
    if (w.Edt_XCu < w.Edt_XPos) {
        w.Edt_XPos = w.Edt_XCu;
        show_all();
        return;
    }
    uint16_t x = w.Edt_XCu - w.Edt_XPos;
    if (x >= w.Edt_WindTx) {
        w.Edt_XPos += x - w.Edt_WindTx + 1;
        show_all();
        return;
    }
    place_cursor();
}

// Ed_SetX: the cursor to column x, the window's left column a multiple of
// 20 keeping it in sight
void set_x(uint16_t x)
{
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAX;
    w.Edt_XCu = x;
    uint16_t left = 0;
    if (x >= 70) {
        do {
            left += 20;
            x -= 20;
        } while (x >= 60);
    }
    w.Edt_XPos = left;
}

// Ed_SetY: the cursor to line y (at most the last), the window moved if
// needed
void set_y(uint16_t y)
{
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAY;
    w.Edt_ASlY = SlDelai;
    if (y > program().Prg_NLigne)
        y = program().Prg_NLigne;
    if (y < w.Edt_YPos) {
        w.Edt_YPos = y;
    } else {
        uint16_t d = y - w.Edt_YPos;
        if (d >= w.Edt_WindTy)
            w.Edt_YPos += d - w.Edt_WindTy + 1;
    }
    w.Edt_YCu = y - w.Edt_YPos;
}

// Ed_GotoX / Ed_GotoY: the same, shown
void goto_x(uint16_t x)
{
    EditWindow &w = window();
    if (x == w.Edt_XCu)
        return;
    uint16_t left = w.Edt_XPos;
    set_x(x);
    if (w.Edt_XPos == left)
        show_current_line();
    else
        show_all();
}

void goto_y(uint16_t y)
{
    EditWindow &w = window();
    if (uint16_t(w.Edt_YPos + w.Edt_YCu) == y)
        return;
    tokenise_current_line();
    uint16_t top = w.Edt_YPos;
    set_y(y);
    if (w.Edt_YPos == top)
        place_cursor();
    else
        new_buffer();
}

} // namespace amos::ed

namespace {

// Ed_ACar: the character of column x of line y of the window
void show_character(uint16_t x, uint16_t y)
{
    EditWindow &w = window();
    locate(int16_t(x), int16_t(y));
    const uint8_t *a0 = buffer_line(y);
    uint16_t d4 = x + w.Edt_XPos;
    uint8_t c = ' ';
    bool in = false;
    if (d4 < peek_w(a0)) {
        c = a0[2 + d4];
        // (the original compared the window's line with the program's lines
        // of the block, and drew the first line of a block of several lines
        // in inverse left of it rather than from it)
        uint16_t d5 = y + w.Edt_YPos;
        Block b;
        if (block(b) && d5 >= b.y1 && d5 <= b.y2) {
            if (b.y1 == b.y2)
                in = d4 >= b.x1 && d4 < b.x2;
            else if (d5 == b.y1)
                in = d4 >= b.x1;
            else if (d5 == b.y2)
                in = d4 < b.x2;
            else
                in = true;
        }
    }
    if (in) {
        inverse();
        chr_out(c);
        normal();
    } else {
        chr_out(c);
    }
}

} // namespace

namespace amos::ed {

// Ed_AffBufCar: column x of all the lines of the window
void show_column(uint16_t x)
{
    for (uint16_t y = 0; y < window().Edt_WindTy; y++)
        show_character(x, y);
    place_cursor();
}

// Ed_NLab: the labels and procedures around the cursor's line, counting
// from line start: the last one before (0 if none), the first after (or
// the end)
Labels labels_around(uint16_t start)
{
    EditWindow &w = window();
    uint16_t here = w.Edt_YPos + w.Edt_YCu;
    uint32_t before = 0, after = 0xFFFFFFFF;
    for (uint8_t *line = find_line(start); peek_w(line); line = next_line()) {
        after++;
        uint16_t t = peek_w(line + 2);
        if (t != tk::Proc && t != tk::Lab)
            continue;
        if (uint16_t(after) == here)
            continue;
        if (uint16_t(after) > here)
            break;
        before = (before & 0xFFFF0000) | uint16_t(after);
    }
    return {before, after, here};
}

} // namespace amos::ed

extern "C" {
// Ed_Loca (d1 the cursor's column in the window)
void ed_place_cursor(AllRegs *r)
{
    place_cursor();
    EditWindow &w = window();
    r->d[1] = uint32_t(int32_t(int16_t(w.Edt_XCu - w.Edt_XPos)));
}

// Ed_EALigne / Ed_ALigne: d1 the line of the window
void ed_show_line(AllRegs *r) { show_line(uint16_t(r->d[1]), true); }
void ed_show_line_only(AllRegs *r) { show_line(uint16_t(r->d[1]), false); }

// Ed_AffBufCar: column d1 of all the lines of the window
void ed_show_column(AllRegs *r) { show_column(uint16_t(r->d[1])); }

void ed_show_all(AllRegs *) { show_all(); }
void ed_new_buffer(AllRegs *) { new_buffer(); }
void ed_new_buffer_all(AllRegs *) { new_buffer_all(); }
void ed_show_current_line(AllRegs *) { show_current_line(); }
void ed_centre(AllRegs *) { centre(); }

// Ed_GotoX / Ed_GotoY / Ed_SetX / Ed_SetY: d0
void ed_goto_x_fn(AllRegs *r) { goto_x(uint16_t(r->d[0])); }
void ed_goto_y_fn(AllRegs *r) { goto_y(uint16_t(r->d[0])); }
void ed_set_x(AllRegs *r) { set_x(uint16_t(r->d[0])); }
void ed_set_y(AllRegs *r) { set_y(uint16_t(r->d[0])); }

// Ed_SetXY: the cursor on address a0 of the program, column d0 (0 in a
// closed procedure, which is kept for showing it)
void ed_set_xy(AllRegs *r)
{
    Program &p = program();
    uint16_t x = uint16_t(r->d[0]);
    p.Prg_AdEProc = 0;
    p.Prg_XEProc = x;
    source::Number n = source::find_address(ptr<uint8_t>(p.Prg_StBas), ptr<uint8_t>(r->a[0]));
    if (n.procedure) {
        p.Prg_AdEProc = address(n.line);
        x = 0;
    }
    set_y(uint16_t(n.number));
    set_x(x);
    Regs a;
    call(ed_aver_fin, a);
    new_buffer();
}

// Ed_NLab: d0 the line to count from; d1 the label before, d2 after
void ed_labels_around(AllRegs *r)
{
    Labels l = labels_around(uint16_t(r->d[0]));
    r->d[1] = l.before;
    r->d[2] = l.after;
    r->d[3] = (r->d[3] & 0xFFFF0000) | l.here;
}

// Ed_AffSlV: the vertical slider
void ed_show_slider(AllRegs *)
{
    EditWindow &w = window();
    if (!w.Edt_WindTy)
        return;
    Regs u;
    call(ed_sl_update_y, u);
    Regs r;
    r.a[0] = address(w.Edt_SlV);
    r.d[0] = 0;
    lib_call(L_Sl_Draw, r);
}

// Ed_CuOn / Ed_CuOff: the cursor shown or not
void ed_cursor_on(AllRegs *) { print(ed_cursor_on_text); }

void ed_cursor_off(AllRegs *)
{
    print(ed_cursor_off_text);
    idata().Ed_CuFlag = 0;
}
}
