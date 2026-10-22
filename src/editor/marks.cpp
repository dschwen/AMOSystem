// The marks of a program (+Edit.s "MARKS"): Prg_Marks, ten longs: the line
// word, $FF, the column byte (0: no mark). Marks 0-3 are also the automatic
// ones, the places the cursor jumped from. While the program is out of the
// editor (run, saved) they hold the offset of the line in the program
// instead, times 256, with the column.

#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_no_mark[], ed_alert_100[];

namespace {

constexpr int Marks = 10, AutoMarks = 4;

uint32_t *marks() { return reinterpret_cast<uint32_t *>(program().Prg_Marks); }

uint32_t here()
{
    EditWindow &w = window();
    return uint32_t(uint16_t(w.Edt_YPos + w.Edt_YCu)) << 16 | 0xFF00 | uint8_t(w.Edt_XCu);
}

} // namespace

namespace amos::ed {

// Ed_MarksChange: lines inserted (delta > 0) or deleted at line n
void marks_change(uint16_t n, int16_t delta)
{
    uint16_t *m = reinterpret_cast<uint16_t *>(marks());
    int16_t end = int16_t(n - delta);
    for (int i = 0; i < Marks; i++, m += 2) {
        if (!peek_l(m))
            continue;
        int16_t line = int16_t(m[0]);
        if (delta >= 0) {
            if (int16_t(n) <= line)
                m[0] += delta;
        } else if (end <= line) {
            m[0] += delta;
        } else if (int16_t(n) <= line) {
            poke_l(m, 0);       // the line is gone
        }
    }
}

// Ed_AutoMarks: the cursor's place first of the automatic marks
void auto_mark()
{
    uint32_t *m = marks();
    uint32_t place = here();
    if (m[0] == place)
        return;
    for (int i = AutoMarks - 1; i > 0; i--)
        m[i] = m[i - 1];
    m[0] = place;
}

} // namespace amos::ed

extern "C" {
void ed_marks_change_fn(AllRegs *r) { marks_change(uint16_t(r->d[0]), int16_t(r->d[1])); }
void ed_auto_marks_fn(AllRegs *) { auto_mark(); }

// Prg_MarkRaz
void ed_marks_clear(AllRegs *)
{
    for (int i = 0; i < Marks; i++)
        marks()[i] = 0;
}

// Ed_SMark0-9: mark d0 on the cursor
void ed_set_mark(AllRegs *r)
{
    tokenise_current_line();
    marks()[uint16_t(r->d[0])] = here();
    Regs a;
    a.d[0] = 64;
    call(ed_alert_100, a);
}

// Ed_GMark0-9: the cursor to mark d0
void ed_goto_mark(AllRegs *r)
{
    tokenise_current_line();
    uint32_t mark = marks()[uint16_t(r->d[0])];
    if (!mark) {
        Regs a;
        call(ed_no_mark, a);
    }
    auto_mark();
    goto_y(uint16_t(mark >> 16));
    goto_x(uint8_t(mark));
}

// Ed_Marks2Adress / Ed_Marks2Number
void ed_marks_to_addresses(AllRegs *)
{
    Program &p = program();
    uint32_t *m = marks();
    for (int i = 0; i < Marks; i++) {
        if (!m[i])
            continue;
        uint8_t *line = find_line(uint16_t(m[i] >> 16));
        if (!peek_w(line))
            m[i] = 0;
        else
            m[i] = (address(line) - p.Prg_StBas) << 8 | uint8_t(m[i]);
    }
}

void ed_marks_to_numbers(AllRegs *)
{
    Program &p = program();
    uint32_t *m = marks();
    for (int i = 0; i < Marks; i++) {
        if (!m[i])
            continue;
        source::Number n = source::find_address(ptr<uint8_t>(p.Prg_StBas), ptr<uint8_t>((m[i] >> 8) + p.Prg_StBas));
        uint8_t *b = reinterpret_cast<uint8_t *>(&m[i]);
        poke_w(b, uint16_t(n.number));
        b[2] = 0xFF;
    }
}
}
