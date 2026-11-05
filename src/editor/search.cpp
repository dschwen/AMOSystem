// Search and replace in the editor (+Edit.s Ed_SchBoth, Ed_SchFront,
// Ed_SchBack, SchBuffer, RepBuffer): the lines detokenised one by one into
// Ed_BufT and searched as text. And the commands: Ed_Search...,
// Ed_Replace..., Ed_GotoL.
//
// Ed_SchMode: bit 0 not case sensitive, 1 backwards, 2 in the block, 3
// everywhere (2 and 3: replace all at once). Bit 15 of the flags of a
// search: replace what is found.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s

extern "C" void ed_block_limits_2(AllRegs *r);

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
        be_u16 &actualise = wdata().Actualise;
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

// RepBuffer: the searched text at column x of line (its length in the
// word before) replaced; false if the line would be too long
bool replace_text(uint8_t *line, uint16_t x)
{
    IData &d = idata();
    uint16_t length = peek_w(line - 2);
    line[length] = 0;
    uint8_t *at = line + x;
    uint16_t searched = d.Ed_SchLong, by = d.Ed_RepLong;
    // the searched text out, the end of the line with its 0
    uint8_t *a1 = at;
    const uint8_t *a0 = at + searched;
    do
        *a1++ = *a0;
    while (*a0++);
    uint16_t room = uint16_t(a1 + by - line);
    if (room >= 252)
        return false;
    for (uint8_t *p = a1; p-- > at;)
        p[by] = *p;
    for (uint16_t i = 0; i < by; i++)
        at[i] = d.Ed_RepBuf[i];
    poke_w(line - 2, uint16_t(room - 1));
    return true;
}

} // namespace

extern "C" {
// RepBuffer: a0 the line, d0 the column; d6 past the new text, d1 the
// new length; d0 -1 if too long
void ed_replace_text(AllRegs *r)
{
    uint8_t *line = ptr<uint8_t>(r->a[0]);
    if (!replace_text(line, uint16_t(r->d[0]))) {
        r->d[0] = 0xFFFFFFFF;
        return;
    }
    r->d[6] = (r->d[6] & 0xFFFF0000) | uint16_t(r->d[6] + idata().Ed_RepLong);
    r->d[1] = peek_w(line - 2);
    r->d[0] = 0;
}
}

namespace {

constexpr uint16_t EdDSearch = 4, EdDReplace = 6, EdDWBlock = 8, EdDWText = 9, EdDChanges = 10, EdDGotoL = 35;
constexpr uint8_t EtABAll = 0x7F, SlDelai = 10;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

[[noreturn]] void alert(uint8_t *routine)
{
    run(routine);
    __builtin_unreachable();
}

be_u32 *dialogue_variables() { return ptr<be_u32>(idata().Ed_VDialogues); }

uint32_t dialogue_value(uint16_t zone, uint32_t which)
{
    Regs r;
    r.d[0] = 1;
    r.d[1] = zone;
    r.d[2] = which;
    lib_call(L_Dia_GetValue, r);
    return r.d[1];
}

// Ed_GotoL: the line asked for (or given by a program driving the editor)
void goto_line()
{
    IData &d = idata();
    tokenise_current_line();
    int32_t n;
    if (d.Ed_Zappeuse) {
        n = int32_t(d.Ed_ZapParam);
    } else {
        if (int16_t(dialogue(EdDGotoL)) != 1)
            alert(ed_not_done);
        n = int32_t(dialogue_value(3, 0xFFFFFFFF));
    }
    if (--n < 0)
        alert(ed_not_done);
    uint16_t y = uint16_t(n);
    if (y >= program().Prg_NLigne)
        y = program().Prg_NLigne;
    auto_mark();
    goto_y(y);
}

// Ed_DiaS: the search dialogue n (the text and the mode); its answer
int16_t search_dialogue(uint16_t n)
{
    IData &d = idata();
    uint8_t *text = dialogue_string(d.Ed_SchBuf);
    be_u32 *v = dialogue_variables();
    v[0] = address(text);
    v[1] = 32;
    Regs f;
    f.d[0] = d.Ed_SchMode & 1;
    f.d[1] = 4;
    f.a[0] = address(v + 2);
    lib_call(L_Dia_SetVFlags, f);
    int16_t answer = int16_t(dialogue(n));
    Regs g;
    g.d[1] = 4;
    g.a[0] = address(dialogue_variables() + 2);
    lib_call(L_Dia_GetVFlags, g);
    d.Ed_SchMode = uint16_t(g.d[0]);
    const uint8_t *from = ptr<uint8_t>(dialogue_value(3, 0xFFFFFFFF));
    uint8_t *to = d.Ed_SchBuf;
    while ((*to++ = *from++))
        ;
    return answer;
}

// Ed_SRCompte
void count_lengths()
{
    IData &d = idata();
    uint8_t n = 0;
    while (d.Ed_SchBuf[n])
        n++;
    d.Ed_SchLong = n;
    n = 0;
    while (d.Ed_RepBuf[n])
        n++;
    d.Ed_RepLong = n;
}

// Ed_SR: the next (or previous) one found from the cursor, and replaced
// with bit 15
void search_replace(uint16_t flags)
{
    EditWindow &w = window();
    count_lengths();
    Search s = {int16_t(w.Edt_YPos + w.Edt_YCu), int16_t(w.Edt_XCu), flags, 32000, 32000};
    warn_message(36);
    if (!(flags & 2 ? search_back(s) : search_front(s)))
        alert(ed_no_found);
    auto_mark();
    goto_y(uint16_t(s.y));
    goto_x(uint16_t(s.x));
    if (flags & 0x8000) {
        uint8_t *line = ptr<uint8_t>(w.Edt_BufE) + int16_t(uint16_t(w.Edt_YCu << 8)) + 2;
        if (!replace_text(line, uint16_t(s.x)))
            alert(ed_l_to_long);
        w.Edt_LEdited++;
        tokenise_current_line();
        goto_x(uint16_t(s.x + idata().Ed_RepLong));
    }
    warning_end();
    place_cursor();
}

// Ed_Search / Ed_SearchPrev / Ed_SearchNext
void search_new()
{
    tokenise_current_line();
    if (search_dialogue(EdDSearch) != 1)
        alert(ed_not_done);
    search_replace(idata().Ed_SchMode & 3);
}

void search_next(bool back)
{
    tokenise_current_line();
    if (!idata().Ed_SchBuf[0])
        return search_new();
    search_replace(uint16_t((idata().Ed_SchMode & 1) | (back ? 2 : 0)));
}

void replace_new();

// Ed_ReplaceNext / Ed_ReplacePrev
void replace_next(bool back)
{
    IData &d = idata();
    uint16_t flags = uint16_t((d.Ed_SchMode & 1) | 0x8000 | (back ? 2 : 0));
    tokenise_current_line();
    if (!d.Ed_SchBuf[0] || !d.Ed_RepBuf[0])
        return replace_new();
    search_replace(flags);
}

// Ed_Replace: the dialogue, then one replaced, or all of them in the text
// or the block
void replace_new()
{
    IData &d = idata();
    for (;;) {
        tokenise_current_line();
        uint8_t *text = dialogue_string(d.Ed_RepBuf);
        dialogue_variables()[8] = address(text);
        int16_t answer = search_dialogue(EdDReplace);
        const uint8_t *from = ptr<uint8_t>(dialogue_value(4, 1));
        uint8_t *to = d.Ed_RepBuf;
        while ((*to++ = *from++))
            ;
        if (answer != 1)
            alert(ed_loop);
        uint16_t mode = d.Ed_SchMode;
        if (!(mode & 0x0C))
            return replace_next(mode & 2);
        if (int16_t(dialogue(mode & 4 ? EdDWBlock : EdDWText)) != 1)
            alert(ed_not_done);
        if (d.Ed_SchBuf[0] && d.Ed_RepBuf[0])
            break;
    }
    uint16_t mode = d.Ed_SchMode;
    count_lengths();
    Search s = {0, 0, mode, 32000, 32000};
    if (mode & 4) {
        AllRegs b = {};
        ed_block_limits_2(&b);
        s.y = int16_t(b.d[1]);
        s.x = int16_t(b.d[0]);
        s.y_max = int16_t(b.d[3]);
        s.x_max = int16_t(b.d[2]);
        if (s.x_max)
            s.x_max = int16_t((s.x_max & 0xFF00) | uint8_t(s.x_max - 1));
    }
    warn_message(37);
    uint16_t changes = 0;
    uint8_t *line = ptr<uint8_t>(d.Ed_BufT) + 2;
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    for (;;) {
        // on the last letter of what replaced
        if (--s.x < 0)
            s.x = 0;
        if (!search_front(s))
            break;
        if (!replace_text(line, uint16_t(s.x)))
            alert(ed_l_to_long);
        s.x = int16_t(s.x + d.Ed_RepLong);
        // (an empty line too is "line too long")
        if (!tokenise_line(line, buffer)) {
            new_buffer_all();
            alert(ed_l_to_long);
        }
        if (store(buffer, uint16_t(s.y), false).error) {
            new_buffer_all();
            alert(ed_oof_buf);
        }
        changes++;
    }
    new_buffer();
    warning_end();
    EditWindow &w = window();
    w.Edt_EtatAff = EtABAll;
    w.Edt_ASlY = SlDelai;
    if (!changes)
        alert(ed_no_found);
    dialogue_variables()[0] = changes;
    dialogue(EdDChanges);
}

} // namespace

extern "C" {
void ed_goto_line(AllRegs *) { goto_line(); }
void ed_search_new(AllRegs *) { search_new(); }
void ed_search_previous(AllRegs *) { search_next(true); }
void ed_search_next(AllRegs *) { search_next(false); }
void ed_replace_new(AllRegs *) { replace_new(); }
void ed_replace_next(AllRegs *) { replace_next(false); }
void ed_replace_previous(AllRegs *) { replace_next(true); }
void ed_search_dialogue(AllRegs *r) { r->d[0] = (r->d[0] & 0xFFFF0000) | uint16_t(search_dialogue(uint16_t(r->d[0]))); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_search.inc"
#endif
