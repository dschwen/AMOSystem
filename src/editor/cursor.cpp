// Moving the editor's cursor (+Edit.s Ed_CHaut... Ed_NLabel): by
// characters, words, lines and pages, to the ends of the line and the
// text, to the labels; the window scrolled with it. And the lines of the
// window detokenised from the program (Ed_Untok, Ed_BufUntok).

#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_tok_cur[], ed_sam_play[], ed_top_of_text[], ed_bottom_of_text[], ed_auto_marks[];

namespace {

constexpr uint8_t EtAX = 1 << 2, EtAY = 1 << 3, EtABXY = 0x0C;
constexpr uint8_t SlDelai = 10;
// The column (from the window's left one) where moving right scrolls: the
// assembler compared with WiTx - 10, the offset of WiTx in a text window
// (80) and not the edit window's width
constexpr uint16_t RightEdge = 80 - 10;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void tokenise_current() { run(ed_tok_cur); }

void sound()
{
    Regs r;
    r.d[0] = 'F';
    call(ed_sam_play, r);
}

void wi_chr_out(uint8_t c)
{
    Regs r;
    r.d[1] = c;
    wi_call(wi::ChrOut, r);
}

void wi_locate(int32_t x, int32_t y)
{
    Regs r;
    r.d[1] = uint32_t(x);
    r.d[2] = uint32_t(y);
    wi_call(wi::Locate, r);
}

uint8_t *buffer_line(uint16_t y) { return ptr<uint8_t>(window().Edt_BufE) + int16_t(uint16_t(y << 8)); }

// One line of the window from a line of the program (0: none)
void untokenise(uint8_t *line, uint16_t y)
{
    uint8_t *to = buffer_line(y);
    poke_w(to, 0);
    to[255] = 0;
    if (!peek_w(line))
        return;
    if (!source::editable(line))
        to[255] = 0xFF;
    detok(line, to);
}

} // namespace

namespace amos::ed {

// Ed_Untok: line n of the program into line y of the window
void untokenise_line(uint16_t n, uint16_t y) { untokenise(find_line(n), y); }

// Ed_BufUntok: all the window's lines
void untokenise_window()
{
    EditWindow &w = window();
    uint8_t *line = find_line(w.Edt_YPos);
    for (uint16_t y = 0; y < w.Edt_WindTy; y++) {
        if (y)
            line = next_line();
        untokenise(line, y);
    }
}

} // namespace amos::ed

namespace {

// Ed_CHt / Ed_CBs: the window scrolled down / up by a line, the line that
// comes in shown
void scroll_down()
{
    EditWindow &w = window();
    w.Edt_ASlY = SlDelai;
    if (w.Edt_WindTy != 1) {
        wi_chr_out(20);
        uint8_t *buffer = ptr<uint8_t>(w.Edt_BufE);
        for (uint32_t i = uint32_t(w.Edt_WindTy - 1) * 256; i--;)
            buffer[256 + i] = buffer[i];
    }
    untokenise_line(w.Edt_YPos, 0);
    show_line(0, true);
    place_cursor();
}

void scroll_up()
{
    EditWindow &w = window();
    w.Edt_ASlY = SlDelai;
    if (w.Edt_WindTy != 1) {
        wi_chr_out(22);
        uint8_t *buffer = ptr<uint8_t>(w.Edt_BufE);
        uint32_t n = uint32_t(w.Edt_WindTy - 1) * 256;
        for (uint32_t i = 0; i < n; i++)
            buffer[i] = buffer[256 + i];
    }
    uint16_t last = w.Edt_WindTy - 1;
    untokenise_line(w.Edt_YPos + last, last);
    show_line(last, true);
    place_cursor();
}

// Ed_CGo / Ed_CDr: the window scrolled right / left by a column
void scroll_right()
{
    EditWindow &w = window();
    w.Edt_XPos--;
    wi_chr_out(19);
    show_column(0);
    place_cursor();
}

void scroll_left()
{
    EditWindow &w = window();
    w.Edt_XPos++;
    wi_chr_out(17);
    show_column(w.Edt_WindTx - 1);
    place_cursor();
}

// Ed_SHaut2 / Ed_SBas2: the window a line up / down under the cursor
void window_up()
{
    window().Edt_YPos--;
    wi_locate(0, 0);
    scroll_down();
}

void window_down()
{
    EditWindow &w = window();
    w.Edt_YPos++;
    wi_locate(0, w.Edt_WindTy - 1);
    scroll_up();
}

// Ed_ChtT / Ed_CBasT: a line up / down
void line_up()
{
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAY;
    if (w.Edt_YCu) {
        w.Edt_YCu--;
        place_cursor();
        return;
    }
    if (!w.Edt_YPos) {
        run(ed_top_of_text);
        return;
    }
    w.Edt_YPos--;
    scroll_down();
}

void line_down()
{
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAY;
    if (uint16_t(w.Edt_YPos + w.Edt_YCu) >= program().Prg_NLigne) {
        run(ed_bottom_of_text);
        return;
    }
    if (w.Edt_YCu < uint16_t(w.Edt_WindTy - 1)) {
        w.Edt_YCu++;
        place_cursor();
        return;
    }
    w.Edt_YPos++;
    scroll_up();
}

void left()
{
    sound();
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAX;
    if (!w.Edt_XCu)
        return;
    w.Edt_XCu--;
    if (uint16_t(w.Edt_XCu - w.Edt_XPos) >= 15 || !w.Edt_XPos) {
        place_cursor();
        return;
    }
    scroll_right();
}

void right()
{
    sound();
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAX;
    if (w.Edt_XCu >= 250)
        return;
    w.Edt_XCu++;
    if (uint16_t(w.Edt_XCu - w.Edt_XPos) < RightEdge) {
        place_cursor();
        return;
    }
    scroll_left();
}

void page_top()
{
    tokenise_current();
    window().Edt_YCu = 0;
    window().Edt_EtatAff |= EtABXY;
    place_cursor();
}

void page_bottom()
{
    tokenise_current();
    EditWindow &w = window();
    uint16_t y = w.Edt_WindTy - 1;
    uint16_t line = w.Edt_YPos + y;
    if (line >= program().Prg_NLigne)
        y -= line - program().Prg_NLigne;
    w.Edt_YCu = y;
    w.Edt_EtatAff |= EtABXY;
    place_cursor();
}

void auto_marks() { run(ed_auto_marks); }

// Ed_HTex / Ed_BTex
void to_top()
{
    EditWindow &w = window();
    w.Edt_YPos = 0;
    w.Edt_XPos = 0;
    w.Edt_YCu = 0;
    w.Edt_XCu = 0;
    w.Edt_ASlY = SlDelai;
    new_buffer();
    run(ed_top_of_text);
}

void to_bottom()
{
    EditWindow &w = window();
    uint16_t lines = program().Prg_NLigne;
    uint16_t half = w.Edt_WindTy >> 1;
    uint16_t top = lines >= half ? lines - half : 0;
    w.Edt_YPos = top;
    w.Edt_YCu = lines - top;
    w.Edt_XPos = 0;
    w.Edt_XCu = 0;
    w.Edt_ASlY = SlDelai;
    new_buffer();
    run(ed_bottom_of_text);
}

void page_up()
{
    tokenise_current();
    EditWindow &w = window();
    w.Edt_EtatAff |= EtAY;
    w.Edt_ASlY = SlDelai;
    if (!w.Edt_YPos) {
        to_top();
        return;
    }
    int16_t n = int16_t(w.Edt_WindTy - 2);
    if (n < 0)
        n = 1;
    w.Edt_YPos = w.Edt_YPos >= uint16_t(n) ? w.Edt_YPos - n : 0;
    new_buffer();
}

void page_down()
{
    tokenise_current();
    EditWindow &w = window();
    int16_t n = int16_t(w.Edt_WindTy - 2);
    if (n < 0)
        n = 1;
    if (uint16_t(w.Edt_YPos + w.Edt_YCu + n) >= program().Prg_NLigne) {
        to_bottom();
        return;
    }
    w.Edt_YPos += n;
    w.Edt_EtatAff |= EtAY;
    w.Edt_ASlY = SlDelai;
    new_buffer();
}

// NLb1: to line n, a label or procedure
void to_label(uint16_t n)
{
    if (n >= program().Prg_NLigne) {
        tokenise_current();
        auto_marks();
        to_bottom();
        return;
    }
    auto_marks();
    EditWindow &w = window();
    w.Edt_EtatAff |= EtABXY;
    w.Edt_ASlY = SlDelai;
    int16_t top = int16_t(n - w.Edt_YCu);
    if (top < 0) {
        top = 0;
        w.Edt_YCu = n;
    }
    w.Edt_YPos = uint16_t(top);
    w.Edt_XCu = 0;
    new_buffer();
}

} // namespace

namespace amos::ed {

void cursor_left() { left(); }
void cursor_right() { right(); }

} // namespace amos::ed

extern "C" {
// Ed_Untok: line d0 of the program into line d1 of the window
void ed_untok(AllRegs *r) { untokenise_line(uint16_t(r->d[0]), uint16_t(r->d[1])); }
void ed_buf_untok_fn(AllRegs *) { untokenise_window(); }

void ed_cursor_up(AllRegs *)
{
    tokenise_current();
    sound();
    line_up();
}

void ed_cursor_up_t(AllRegs *) { line_up(); }
void ed_cursor_down_t(AllRegs *) { line_down(); }

void ed_cursor_down(AllRegs *)
{
    tokenise_current();
    sound();
    line_down();
}

void ed_cursor_left(AllRegs *) { left(); }
void ed_cursor_right(AllRegs *) { right(); }
void ed_page_top(AllRegs *) { page_top(); }
void ed_page_bottom(AllRegs *) { page_bottom(); }

void ed_word_left_key(AllRegs *)
{
    window().Edt_XCu = word_left_of_cursor();
    centre();
}

void ed_word_right_key(AllRegs *)
{
    window().Edt_XCu = word_right_of_cursor();
    centre();
}

void ed_line_start(AllRegs *)
{
    window().Edt_XCu = 0;
    centre();
}

void ed_line_end(AllRegs *)
{
    window().Edt_XCu = current_line_length();
    centre();
}

void ed_page_up(AllRegs *) { page_up(); }
void ed_page_down(AllRegs *) { page_down(); }

// Ed_SHaut / Ed_SBas / Ed_SGauche / Ed_SDroite: the window scrolled under
// the cursor
void ed_scroll_up_key(AllRegs *)
{
    tokenise_current();
    EditWindow &w = window();
    if (!w.Edt_YPos)
        return;
    window_up();
}

void ed_scroll_down_key(AllRegs *)
{
    tokenise_current();
    EditWindow &w = window();
    if (uint16_t(w.Edt_YPos + w.Edt_YCu) >= program().Prg_NLigne)
        return;
    window_down();
}

void ed_scroll_left_key(AllRegs *)
{
    tokenise_current();
    EditWindow &w = window();
    if (!w.Edt_XPos)
        return;
    w.Edt_XCu--;
    scroll_right();
}

void ed_scroll_right_key(AllRegs *)
{
    tokenise_current();
    EditWindow &w = window();
    if (w.Edt_XCu >= 250)
        return;
    w.Edt_XCu++;
    scroll_left();
}

// Ed_CGo / Ed_CDr / Ed_SHaut2 / Ed_SBas2: the scrolls alone (the mouse at
// the window's edges)
void ed_scroll_right(AllRegs *) { scroll_right(); }
void ed_scroll_left(AllRegs *) { scroll_left(); }

void ed_window_up(AllRegs *) { window_up(); }
void ed_window_down(AllRegs *) { window_down(); }

void ed_text_top(AllRegs *)
{
    tokenise_current();
    auto_marks();
    to_top();
}

void ed_text_top_t(AllRegs *) { to_top(); }

void ed_text_bottom(AllRegs *)
{
    tokenise_current();
    auto_marks();
    to_bottom();
}

void ed_text_bottom_t(AllRegs *) { to_bottom(); }

// Ed_PLabel / Ed_NLabel: to the label or procedure before / after
void ed_label_previous(AllRegs *)
{
    tokenise_current();
    Labels l = labels_around(0);
    if (!uint16_t(l.before)) {
        tokenise_current();
        auto_marks();
        to_top();
        return;
    }
    to_label(uint16_t(l.before));
}

void ed_label_next(AllRegs *)
{
    tokenise_current();
    Labels l = labels_around(0);
    if (!uint16_t(l.after)) {
        tokenise_current();
        auto_marks();
        to_top();
        return;
    }
    to_label(uint16_t(l.after));
}
}
