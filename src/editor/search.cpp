// Search and replace in the editor (+Edit.s Ed_SchBoth, Ed_SchFront,
// Ed_SchBack, SchBuffer, RepBuffer): the lines detokenised one by one into
// Ed_BufT and searched as text.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

namespace {

constexpr uint16_t BitControl = 1 << 8;         // T_Actualise: Control-C

// The registers of the search: d7 line, d6 column, d5 flags (bit 0 not
// case sensitive, 1 backwards), d4/d3 the last line and column
struct Search {
    int16_t y, x;
    uint16_t flags;
    int16_t y_max, x_max;
};

uint8_t fold(uint8_t c, uint16_t flags) { return flags & 1 && c >= 'a' && c <= 'z' ? c - 0x20 : c; }

// SchBuffer: where text is in line, -1 if not there
int32_t find_text(const uint8_t *line, const uint8_t *text, uint16_t flags)
{
    uint8_t first = fold(text[0], flags);
    for (const uint8_t *a0 = line; *a0; a0++) {
        if (fold(*a0, flags) != first)
            continue;
        const uint8_t *a = a0 + 1, *t = text + 1;
        while (*t && fold(*t, flags) == fold(*a, flags))
            t++, a++;
        if (!*t)
            return int32_t(a0 - line);
    }
    return -1;
}

// Ed_SchFront: from the column after x of line y, to y_max/x_max
bool search_front(Search &s)
{
    IData &d = idata();
    uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
    s.x++;
    if (!peek_w(find_line(uint16_t(s.y))))
        return false;
    for (;;) {
        if (s.y > s.y_max || (s.y == s.y_max && s.x > s.x_max))
            return false;
        detok(ptr<uint8_t>(window().Edt_CurLigne), buffer);
        uint16_t length = peek_w(buffer);
        buffer[2 + length] = 0;
        if (uint16_t(s.x) < length) {
            int32_t at = find_text(buffer + 2 + s.x, d.Ed_SchBuf, s.flags);
            if (at >= 0) {
                s.x = int16_t(s.x + at);
                return s.y != s.y_max || uint16_t(s.x) <= uint16_t(s.x_max);
            }
        }
        // the next line, unless Control-C
        s.x = 0;
        uint16_t &actualise = wdata().Actualise;
        if (actualise & BitControl) {
            actualise &= ~BitControl;
            return false;
        }
        if (!peek_w(next_line()))
            return false;
        s.y++;
    }
}

// Ed_SchBack: the last one before x, y (searching from the start)
bool search_back(Search &s)
{
    Search from = s;
    s.y_max = s.y;
    s.x_max = int16_t(s.x - 1);
    if (s.x_max < 0) {
        s.x_max = 255;
        if (--s.y_max < 0)
            return false;
    }
    s.x = 0;
    s.y = 0;
    int16_t best_x = -1, best_y = -1;
    while (search_front(s)) {
        best_x = s.x;
        best_y = s.y;
        s.x++;
    }
    if (best_x < 0) {
        s.x = from.x;
        s.y = from.y;
        return false;
    }
    s.x = best_x;
    s.y = best_y;
    return true;
}

Search registers(const AllRegs *r)
{
    return {int16_t(r->d[7]), int16_t(r->d[6]), uint16_t(r->d[5]), int16_t(r->d[4]), int16_t(r->d[3])};
}

void put_back(AllRegs *r, const Search &s, bool found)
{
    r->d[7] = (r->d[7] & 0xFFFF0000) | uint16_t(s.y);
    r->d[6] = (r->d[6] & 0xFFFF0000) | uint16_t(s.x);
    r->d[4] = (r->d[4] & 0xFFFF0000) | uint16_t(s.y_max);
    r->d[3] = (r->d[3] & 0xFFFF0000) | uint16_t(s.x_max);
    r->d[0] = found ? 0xFFFFFFFF : 0;
}

} // namespace

extern "C" {
// Ed_SchFront / Ed_SchBack / Ed_SchBoth (backwards if bit 1 of d5): d7/d6
// where the text is, d0 -1 if found, 0 if not
void ed_search_front(AllRegs *r)
{
    Search s = registers(r);
    bool found = search_front(s);
    put_back(r, s, found);
}

void ed_search_back(AllRegs *r)
{
    Search s = registers(r);
    bool found = search_back(s);
    put_back(r, s, found);
}

void ed_search(AllRegs *r)
{
    if (r->d[5] & 2)
        ed_search_back(r);
    else
        ed_search_front(r);
}

// RepBuffer: the searched text at column d0 of the line a0 (its length in
// the word before) replaced; d6 past the new text; d0 -1 if too long
void ed_replace_text(AllRegs *r)
{
    IData &d = idata();
    uint8_t *line = ptr<uint8_t>(r->a[0]);
    uint16_t length = peek_w(line - 2);
    line[length] = 0;
    uint8_t *at = line + uint16_t(r->d[0]);
    uint16_t searched = d.Ed_SchLong, by = d.Ed_RepLong;
    // the searched text out, the end of the line with its 0
    uint8_t *a1 = at;
    const uint8_t *a0 = at + searched;
    do
        *a1++ = *a0;
    while (*a0++);
    uint16_t room = uint16_t(a1 + by - line);
    if (room >= 252) {
        r->d[0] = 0xFFFFFFFF;
        return;
    }
    for (uint8_t *p = a1; p-- > at;)
        p[by] = *p;
    for (uint16_t i = 0; i < by; i++)
        at[i] = d.Ed_RepBuf[i];
    r->d[6] = (r->d[6] & 0xFFFF0000) | uint16_t(r->d[6] + by);
    r->d[1] = uint32_t(room - 1);
    poke_w(line - 2, uint16_t(room - 1));
    r->d[0] = 0;
}
}
