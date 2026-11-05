// The automatic indentation of the editor (+Edit.s Indent): each line's
// indentation byte from the loops, If blocks and procedures around it.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

namespace amos::ed {

// Indent: the lines from start, tab columns a level
void indent(uint8_t *a0, uint16_t tab)
{
    int16_t next = 0;           // d6: the next line's indentation
    for (;;) {
        uint8_t *line = a0;
        bool then = false;      // d4: Then on the line, Else stays
        int16_t open = 0;       // d7: blocks opened on the line
        int16_t here = next;    // d5: this line's
        if (!peek_w(a0))
            return;
        a0 += 2;
        for (;;) {
            uint16_t t = peek_w(a0);
            a0 += 2;
            if (!t)
                break;
            a0 = const_cast<uint8_t *>(skip_token(t, a0));
            if (t == tk::Proc) {
                here = 0;
                next = tab;
                if (line[10] & 0x80) {
                    // closed: skipped
                    next = 0;
                    a0 = line + 12 + 2 + peek_l(line + 4);
                    break;
                }
            } else if (t == tk::EndP) {
                here = 0;
                next = 0;
            } else if (t == tk::For || t == tk::Rpt || t == tk::Whl || t == tk::Do || t == tk::If) {
                open++;
                next += tab;
            } else if (t == tk::Then || t == tk::EndI || t == tk::Nxt || t == tk::Unt || t == tk::Wnd ||
                       t == tk::Loo) {
                if (t == tk::Then)
                    then = true;
                // the end of a block: this line too if it was opened before
                if (--open < 0) {
                    open = 0;
                    here -= tab;
                }
                next -= tab;
            } else if (t == tk::Else || t == tk::ElsI) {
                if (!then)
                    here -= tab;
            }
        }
        if (here < 0)
            here = 0;
        if (uint16_t(here) >= 127)
            here = 127;
        line[1] = uint8_t(here + 1);
    }
}

} // namespace amos::ed

extern "C" {
// Indent: a0 the program, d0 the tab
void ed_indent(AllRegs *r) { indent(ptr<uint8_t>(r->a[0]), uint16_t(r->d[0])); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_indent.inc"
#endif
