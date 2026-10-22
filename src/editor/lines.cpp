// Editing the current line of the editor (+Edit.s): the line under the
// cursor (Ed_LCourant), typing (Ed_PKey), deleting characters, words and
// parts of the line, tabs, and what undo and redo do for each kind of
// change.
//
// The window's text: Edt_BufE, 256 bytes a line of the window: the length
// word, the characters, byte 255 set if the line cannot be edited (a
// closed procedure). The display and the cursor stay with the assembler.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_sam_play[], ed_cent[], ed_loca[], ed_ea_li_cu[], ed_ea_ligne[], ed_a_ligne[], ed_c_droite[],
    ed_c_gauche[], ed_join[], ed_not_edit[], ed_l_to_long[], ed_goto_x[], ed_goto_y[], ed_ins_line[], ed_del_li_cu[],
    ed_tok_cur[], ed_return_quiet[];

namespace {

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void run_d0(uint8_t *routine, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    call(routine, r);
}

void run_d1(uint8_t *routine, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    call(routine, r);
}

// Ed_SamPlay: the editor's sound for a key
void sound(uint8_t c) { run_d0(ed_sam_play, c); }

// Ed_LCourant: the line under the cursor
struct Line {
    uint8_t *text;          // a0
    uint16_t length;        // d0
    uint16_t x;             // d1
    uint8_t *at;            // a1: the character under the cursor
    bool locked;            // not editable
    void set_length(uint16_t n) { poke_w(text - 2, n); }
};

Line current()
{
    EditWindow &w = window();
    uint8_t *line = ptr<uint8_t>(w.Edt_BufE) + int16_t(uint16_t(w.Edt_YCu << 8));
    uint8_t *text = line + 2;
    return {text, peek_w(line), w.Edt_XCu, text + w.Edt_XCu, line[255] != 0};
}

// Une_Lettre: part of a word
bool letter(uint8_t c) { return (c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c >= 128; }

void edited() { window().Edt_LEdited++; }

// R_InsChar: n characters inserted at the cursor (up to 250 in the line)
void insert_text(const uint8_t *from, uint16_t n)
{
    Line l = current();
    uint16_t length = l.length + n;
    if (length > 250) {
        n -= length - 250;
        length = 250;
    }
    if (!n)
        return;
    l.set_length(l.length + n);
    for (uint8_t *end = l.text + length, *p = end - n; p > l.at;)
        *--end = *--p;
    for (uint16_t i = 0; i < n; i++)
        l.at[i] = from[i];
    edited();
}

// R_DelChar: n characters deleted at the cursor
void delete_text(uint16_t n)
{
    Line l = current();
    if (n > l.length)
        return;
    l.set_length(l.length - n);
    int16_t rest = int16_t(l.length - l.x - n);
    for (int16_t i = 0; i < rest; i++)
        l.at[i] = l.at[i + n];
    edited();
}

// R_MotGoch: the start of the word left of the cursor
uint16_t word_left()
{
    Line l = current();
    if (l.x > l.length)
        return l.length;
    int16_t x = int16_t(l.x);
    const uint8_t *p = l.at;
    if (--x >= 0 && *--p == ' ') {
        while (--x >= 0 && *--p == ' ')
            ;
    }
    if (x >= 0) {
        while (--x >= 0 && letter(*--p))
            ;
    }
    return uint16_t(x + 1);
}

// R_MotDroi: the start of the word right of the cursor
uint16_t word_right()
{
    Line l = current();
    uint16_t x = l.x;
    const uint8_t *p = l.at;
    for (;;) {
        if (++x >= l.length)
            return x;
        if (letter(*p++))
            continue;
        if (*p == ' ')
            continue;
        return x;
    }
}

// Un_CLine: n characters at text kept for undo
void keep_text(const uint8_t *text, uint16_t n) { undo_text(text, n); }

} // namespace

namespace amos::ed {

// Ed_PKey: the character c typed, inserted or over the one under the
// cursor; entry the undo entry being redone, if any
void type_key(uint8_t c, bool insert, uint8_t *redone)
{
    if (c >= 32) {
        Line l = current();
        if (l.locked) {
            run(ed_not_edit);
            return;
        }
        if (l.x > l.length) {
            // past the end: to the end
            l.x = l.length;
            l.at = l.text + l.x;
            window().Edt_XCu = l.x;
            run(ed_cent);
        }
        uint8_t *entry = undo_start();
        if (entry) {
            entry[0] = 0x01;
            entry[4] = 0xFF;
            entry[5] = c;
        } else {
            // (the original wrote the replaced character through a stale
            // register then; while redoing that was the entry)
            entry = redone;
        }
        uint16_t length = l.length;
        if (!insert) {
            if (l.length != l.x) {
                if (entry)
                    entry[4] = *l.at;
                goto put;
            }
        } else {
            if (l.length >= 250) {
                run(ed_l_to_long);
                return;
            }
            if (l.length != l.x) {
                for (uint8_t *p = l.text + l.length; p >= l.at; p--)
                    p[1] = p[0];
            }
        }
        length++;
    put:
        *l.at = c;
        l.set_length(length);
        edited();
        run(ed_c_droite);
        run_d1(ed_a_ligne, window().Edt_YCu);
    }
    run(ed_loca);
    idata().Ed_SCallFlags = 0x01;
}

// Ed_Delete: the character under the cursor
void delete_key()
{
    sound('F');
    Line l = current();
    if (int16_t(l.length - l.x - 1) < 0)
        return;
    if (uint8_t *entry = undo_start()) {
        entry[0] = 0x02;
        entry[5] = *l.at;
    }
    delete_text(1);
    run(ed_ea_li_cu);
}

} // namespace amos::ed

namespace {

// Ed_Back: the character left of the cursor; at the start of the line,
// the line joined to the one above
void backspace()
{
    if (!window().Edt_XCu) {
        run(ed_join);
        return;
    }
    run(ed_c_gauche);
    delete_key();
}

// Ed_EffLigne: the line emptied
void clear_line()
{
    edited();
    window().Edt_XCu = 0;
    Line l = current();
    l.set_length(0);
    keep_text(l.text, l.length);
    run(ed_ea_li_cu);
    run(ed_cent);
}

// Ed_DelFin: up to the end of the line
void delete_to_end()
{
    edited();
    Line l = current();
    if (l.x >= l.length)
        return;
    l.set_length(l.x);
    keep_text(l.at, l.length - l.x);
    run(ed_ea_li_cu);
}

// Ed_DelDebut: up to the cursor
void delete_to_start()
{
    Line l = current();
    if (!l.x)
        return;
    uint16_t n = l.x < l.length ? l.x : l.length;
    window().Edt_XCu = 0;
    keep_text(l.text, n);
    delete_text(n);
    run(ed_ea_li_cu);
}

// Ed_DelMot: up to the next word
void delete_word()
{
    uint16_t to = word_right();
    uint16_t x = window().Edt_XCu;
    if (to <= x)
        return;
    uint16_t n = to - x;
    Line l = current();
    if (l.x >= l.length)
        return;
    keep_text(l.at, n);
    delete_text(n);
    run(ed_ea_li_cu);
}

// Ed_BackMot: back to the start of the word
void delete_word_left()
{
    uint16_t to = word_left();
    if (to >= window().Edt_XCu)
        return;
    window().Edt_XCu = to;
    run(ed_cent);
    delete_word();
}

void tab_done()
{
    edited();
    run_d1(ed_ea_ligne, window().Edt_YCu);
    run(ed_cent);
}

// Ed_Tab: the line moved right by a tab
void tab()
{
    Line l = current();
    uint16_t tab = idata().Ed_Tabs;
    uint16_t length = l.length + tab;
    if (length >= 250) {
        run(ed_l_to_long);
        return;
    }
    l.set_length(length);
    uint8_t *from = l.text + l.length, *to = l.text + length;
    while (from > l.text)
        *--to = *--from;
    window().Edt_XCu += tab;
    for (uint16_t i = 0; i < tab; i++)
        l.text[i] = ' ';
    tab_done();
}

// Ed_ShTab: up to a tab of spaces out of the start of the line
void untab()
{
    Line l = current();
    uint8_t *end = l.text + l.length, *a0 = l.text;
    int16_t left = int16_t(idata().Ed_Tabs - 1);
    if (left < 0)
        return;
    uint16_t n = 0;
    do {
        if (*a0 != ' ')
            break;
        if (++a0 >= end)
            break;
        n++;
    } while (left-- != 0);
    if (!n)
        return;
    l.set_length(l.length - n);
    EditWindow &w = window();
    if (int16_t(w.Edt_XCu -= n) < 0)
        w.Edt_XCu = 0;
    for (uint8_t *a1 = l.text; a0 <= end;)
        *a1++ = *a0++;
    tab_done();
}

// ---- undo and redo ----------------------------------------------------------

// Un_XY / Un_X: the cursor where the entry was made
void to_column(const uint8_t *entry) { run_d0(ed_goto_x, entry[1]); }

void to_place(const uint8_t *entry)
{
    run_d0(ed_goto_y, peek_w(entry + 2));
    to_column(entry);
}

// Un_XYSto: the same with the line of the entry's block; the block
const uint8_t *to_block(const uint8_t *entry)
{
    to_column(entry);
    const uint8_t *block = ptr<uint8_t>(peek_l(entry + 2));
    run_d0(ed_goto_y, peek_w(block + 2));
    return block;
}

// Un_RepLine: the line as kept in the block
void restore_line(const uint8_t *block)
{
    Line l = current();
    uint16_t n = peek_w(block) - 4;
    l.set_length(n);
    for (uint16_t i = 0; i < n; i++)
        l.text[i] = block[4 + i];
    run(ed_ea_li_cu);
    edited();
}

void reinsert_text(const uint8_t *block)
{
    uint16_t n = peek_w(block);
    if (n <= 4)
        return;
    insert_text(block + 4, n - 4);
    run(ed_ea_li_cu);
}

} // namespace

namespace amos::ed {

// The kinds of change: 1 a character typed, 2 deleted, 3 part of a line
// cleared, 4 a line deleted, 5 a line tokenised, 6 inserted, 7 split, 8
// joined
void undo_change(uint8_t *entry)
{
    switch (entry[0] & 0x7F) {
    case 1:
        to_place(entry);
        if (int8_t(entry[4]) < 0) {
            delete_key();
        } else {
            *current().at = entry[4];
            edited();
            run(ed_ea_li_cu);
        }
        break;
    case 2:
        to_place(entry);
        type_key(entry[5], true, entry);
        break;
    case 3:
        reinsert_text(to_block(entry));
        break;
    case 4: {
        const uint8_t *block = to_block(entry);
        run(ed_ins_line);
        reinsert_text(block);
        break;
    }
    case 5: {
        const uint8_t *block = to_block(entry);
        Line l = current();
        uint8_t n = block[6];
        l.set_length(n);
        for (uint8_t i = 0; i < n; i++)
            l.text[i] = block[7 + i];
        program().Prg_NLigne -= peek_w(block + 4);
        edited();
        run(ed_ea_li_cu);
        break;
    }
    case 6:
        to_place(entry);
        run(ed_del_li_cu);
        break;
    case 7:
        to_block(entry);
        run(ed_del_li_cu);
        restore_line(to_block(entry));
        break;
    case 8:
        restore_line(to_block(entry));
        run(ed_return_quiet);
        break;
    }
}

void redo_change(uint8_t *entry)
{
    switch (entry[0] & 0x7F) {
    case 1:
        to_place(entry);
        type_key(entry[5], int8_t(entry[4]) < 0, entry);
        break;
    case 2:
        to_place(entry);
        delete_key();
        break;
    case 3: {
        const uint8_t *block = to_block(entry);
        uint16_t n = peek_w(block);
        if (n > 4)
            delete_text(n - 4);
        run(ed_ea_li_cu);
        break;
    }
    case 4:
        to_block(entry);
        run(ed_del_li_cu);
        break;
    case 5:
        to_block(entry);
        edited();
        run(ed_tok_cur);
        break;
    case 6:
        to_place(entry);
        run(ed_ins_line);
        break;
    case 7:
        restore_line(to_block(entry));
        run(ed_return_quiet);
        break;
    case 8:
        to_block(entry);
        run(ed_del_li_cu);
        restore_line(to_block(entry));
        break;
    }
}

} // namespace amos::ed

extern "C" {
// Ed_LCourant: a0 the line, d0 its length, d1 the cursor's column, a1 the
// character there; NE if the line cannot be edited
uint32_t ed_current_line(AllRegs *r)
{
    Line l = current();
    r->a[0] = address(l.text);
    r->a[1] = address(l.at);
    r->d[0] = (r->d[0] & 0xFFFF0000) | l.length;
    r->d[1] = (r->d[1] & 0xFFFF0000) | l.x;
    return l.locked;
}

// Une_Lettre: d2 -1 for a letter of a word, 0 if not (the condition codes
// too)
uint32_t ed_letter(AllRegs *r)
{
    r->d[2] = letter(uint8_t(r->d[2])) ? 0xFFFFFFFF : 0;
    return r->d[2];
}

// R_InsChar: d0 characters at a0 inserted at the cursor
void ed_insert_text(AllRegs *r) { insert_text(ptr<uint8_t>(r->a[0]), uint16_t(r->d[0])); }

// R_DelChar: d0 characters deleted at the cursor
void ed_delete_text(AllRegs *r) { delete_text(uint16_t(r->d[0])); }

// R_MotGoch / R_MotDroi: d1 the start of the word left / right
void ed_word_left(AllRegs *r) { r->d[1] = (r->d[1] & 0xFFFF0000) | word_left(); }
void ed_word_right(AllRegs *r) { r->d[1] = (r->d[1] & 0xFFFF0000) | word_right(); }

// Ed_PKey: d7 the character, d6 insert
void ed_type_key(AllRegs *r) { type_key(uint8_t(r->d[7]), uint8_t(r->d[6]), nullptr); }

void ed_delete_key(AllRegs *) { delete_key(); }
void ed_backspace(AllRegs *) { backspace(); }
void ed_clear_line(AllRegs *) { clear_line(); }
void ed_delete_to_end(AllRegs *) { delete_to_end(); }
void ed_delete_to_start(AllRegs *) { delete_to_start(); }
void ed_delete_word(AllRegs *) { delete_word(); }
void ed_delete_word_left(AllRegs *) { delete_word_left(); }
void ed_tab(AllRegs *) { tab(); }
void ed_untab(AllRegs *) { untab(); }
}
