// The lines of the editor's window and the program (+Edit.s): the edited
// line tokenised and stored (Ed_TokCur, Ed_TokStok), Return splitting the
// line, joining two lines, inserting and deleting a line.
//
// The alerts (Ed_NotEdit, Ed_LToLong, Ed_OofBuf...) do not come back: they
// go to the editor's loop, which resets the stack and turns undo back on.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint8_t SlDelai = 10;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

uint8_t *buffer_line(uint16_t y) { return ptr<uint8_t>(window().Edt_BufE) + int16_t(uint16_t(y << 8)); }
uint8_t *cursor_line() { return buffer_line(window().Edt_YCu) + 2; }
uint16_t cursor_length() { return peek_w(cursor_line() - 2); }
uint16_t program_line() { return window().Edt_YPos + window().Edt_YCu; }

// Ed_TokStok2: the edited line tokenised and stored in the program (a
// line more at its end if extra and the line is not empty), shown as
// detokenised, kept for undo
void store_line(uint16_t extra)
{
    IData &d = idata();
    EditWindow &w = window();
    w.Edt_LEdited = 0;
    uint8_t *text = cursor_line();
    uint16_t length = cursor_length();
    text[length] = 0;
    uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
    int32_t result = tokenise_line(text, buffer);
    if (result < 0) {
        run(ed_l_to_long);
        return;
    }
    extra += uint16_t(result);
    Stored s = store(buffer, program_line(), false);
    if (s.error) {
        run(ed_oof_buf);
        return;
    }
    extra += uint16_t(s.added);
    // the line as it was, for undo
    uint8_t old_length = uint8_t(length);
    for (uint16_t i = 0; i < length; i++)
        buffer[i] = text[i];
    uint16_t more = 0;
    if (program_line() == program().Prg_NLigne && extra) {
        more = 1;
        program().Prg_NLigne++;
    }
    detok(s.line, text - 2);
    show_line(w.Edt_YCu, true);
    if (uint8_t *entry = undo_start()) {
        entry[0] = 0x85;
        uint16_t now = cursor_length();
        uint32_t size = 8 + now + old_length;
        uint8_t *block = ptr<uint8_t>(memory(size, true));
        // (the original wrote at address 0 without memory)
        if (!block) {
            entry[0] = 0;
        } else {
            poke_w(block, uint16_t(size));
            program().Prg_TUndo += uint16_t(size);
            poke_w(block + 2, peek_w(entry + 2));
            poke_l(entry + 2, address(block));
            poke_w(block + 4, more);
            uint8_t *p = block + 6;
            *p++ = old_length;
            for (uint16_t i = 0; i < old_length; i++)
                *p++ = buffer[i];
            *p++ = uint8_t(now);
            for (uint16_t i = 0; i < now; i++)
                *p++ = text[i];
        }
    }
    program().Prg_MathFlags = d.MathFlags;
}

} // namespace

namespace amos::ed {

// Ed_TokCur: the line stored if edited
void tokenise_current_line()
{
    if (!window().Edt_LEdited)
        return;
    store_line(0);
    place_cursor();
    update_split(&window(), 1);
}

// Ed_InsLine: an empty line inserted at the cursor
void insert_line()
{
    IData &d = idata();
    tokenise_current_line();
    uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
    buffer[128] = 0;
    tokenise_line(buffer + 128, buffer);
    if (store(buffer, program_line(), true).error) {
        run(ed_oof_buf);
        return;
    }
    if (uint8_t *entry = undo_start())
        entry[0] = 0x06;
    program().Prg_NLigne++;
    window().Edt_ASlY = SlDelai;
    new_buffer();
}

// Ed_DelLiCu: the cursor's line deleted; 0 if it was, 1 for the end of
// the program, -1 for a closed procedure
int32_t delete_current_line()
{
    EditWindow &w = window();
    uint16_t n = program_line();
    uint8_t *text = cursor_line();
    if (buffer_line(w.Edt_YCu)[255]) {
        run(ed_not_edit);
        return -1;
    }
    uint16_t x = w.Edt_XCu;
    w.Edt_XCu = 0;
    if (uint8_t *entry = undo_text(text, cursor_length()))
        entry[0] = 0x84;
    w.Edt_XCu = x;
    int32_t result = delete_line(n);
    if (!result) {
        Regs r;
        r.d[1] = 23;
        wi_call(wi::ChrOut, r);
        // the lines below up
        uint8_t *line = cursor_line() - 2, *end = ptr<uint8_t>(w.Edt_BufE) + uint32_t(w.Edt_WindTy) * 256;
        if (end > line + 256) {
            for (uint8_t *p = line; p + 256 < end; p++)
                *p = p[256];
        }
        poke_w(end - 256, 0);
    }
    uint16_t last = w.Edt_WindTy - 1;
    untokenise_line(w.Edt_YPos + last, last);
    show_line(last, true);
    w.Edt_ASlY = SlDelai;
    place_cursor();
    return result;
}

} // namespace amos::ed

namespace {

// Ed_ReturnQuiet: the line split at the cursor (at its start, a line
// inserted before)
void return_key()
{
    IData &d = idata();
    EditWindow &w = window();
    Program &p = program();
    if (!w.Edt_XCu) {
        insert_line();
        line_down_t();
        w.Edt_XCu = 0;
        centre();
        return;
    }
    // the right of the line kept at the bottom of the program's buffer
    uint8_t *kept = ptr<uint8_t>(p.Prg_StMini);
    poke_w(kept, 0);
    poke_w(kept + 2, 0);
    kept += 2;
    if (kept >= ptr<uint8_t>(p.Prg_StBas) - 256) {
        run(ed_oof_buf);
        return;
    }
    if (buffer_line(w.Edt_YCu)[255]) {
        line_down_t();
        w.Edt_XCu = 0;
        centre();
        insert_line();
        return;
    }
    if (uint8_t *entry = undo_text(cursor_line(), cursor_length()))
        entry[0] = 0x87;
    d.Ed_FUndo++;
    uint8_t *text = cursor_line();
    uint16_t length = cursor_length(), x = w.Edt_XCu;
    poke_w(text - 2, x < length ? x : length);
    uint8_t *to = kept;
    for (uint16_t i = x; i < length; i++)
        *to++ = text[i];
    *to = 0;
    uint16_t lines = p.Prg_NLigne;
    store_line(1);
    line_down_t();
    w.Edt_XCu = 0;
    centre();
    if (lines == program().Prg_NLigne)
        insert_line();
    text = cursor_line();
    buffer_line(w.Edt_YCu)[255] = 0;
    uint16_t n = 0;
    for (const uint8_t *from = kept; *from; from++)
        text[n++] = *from;
    poke_w(text - 2, n);
    w.Edt_LEdited++;
    show_current_line();
    d.Ed_FUndo--;
}

// Ed_Join: at the start of a line, the line added to the one above
void join()
{
    IData &d = idata();
    EditWindow &w = window();
    if (!w.Edt_YCu || w.Edt_XCu)
        return;
    d.Ed_FUndo++;
    uint8_t *text = cursor_line();
    uint16_t length = cursor_length();
    uint8_t *above = text - 256;
    if (length && above[253]) {
        run(ed_not_edit);
        return;
    }
    uint16_t above_length = peek_w(above - 2);
    uint16_t total = length + above_length;
    if (total >= 250) {
        run(ed_l_to_long);
        return;
    }
    poke_w(text - 2, 0);
    poke_w(above - 2, total);
    for (uint16_t i = 0; i < length; i++)
        above[above_length + i] = text[i];
    if (delete_current_line()) {
        // the last line, which stays: an empty line before it goes
        if (uint16_t n = program().Prg_NLigne) {
            n--;
            uint8_t *line = find_line(n);
            if (peek_w(line) && peek_l(line) == 0x02000000)
                delete_line(n);
        }
    }
    line_up_t();
    w.Edt_LEdited++;
    w.Edt_ASlY = SlDelai;
    w.Edt_XCu = above_length;
    show_current_line();
    centre();
    d.Ed_SCallFlags |= 0x03;
    d.Ed_FUndo--;
    if (uint8_t *entry = undo_text(cursor_line(), cursor_length()))
        entry[0] = 0x88;
}

} // namespace

namespace amos::ed {

void return_quiet() { return_key(); }
void join_lines() { join(); }

} // namespace amos::ed

extern "C" {
void ed_tok_cur_fn(AllRegs *) { tokenise_current_line(); }

void ed_tok_stok(AllRegs *)
{
    if (window().Edt_LEdited)
        store_line(0);
}

void ed_tok_stok2(AllRegs *r) { store_line(uint16_t(r->d[0])); }
void ed_ins_line_fn(AllRegs *) { insert_line(); }
void ed_del_li_cu_fn(AllRegs *r) { r->d[0] = uint32_t(delete_current_line()); }
void ed_join_fn(AllRegs *) { join(); }
void ed_return_quiet_fn(AllRegs *) { return_key(); }

void ed_return_fn(AllRegs *)
{
    Regs r;
    r.d[0] = 'D';
    call(ed_sam_play, r);
    return_key();
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_edit.inc"
#endif
