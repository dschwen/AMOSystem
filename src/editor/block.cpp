// The editor's marked block (+Edit.s "BLOCS"): marking, storing, cutting,
// pasting. A copied block (Ed_Block, a size long before it):
//   y1, y2, x1 words; a long: bit 31 a closed procedure on the first line,
//     bit 30 all on one line, the low word x2
//   the end of the first line: its length word, the text (even)
//   unless on one line, the lines between as tokens: their number word,
//     their size long, the lines; then the start of the last line: its
//     length word, the text

#include "runtime/static_symbols.h"
#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint32_t OneLine = 1 << 30, ClosedFirst = 1u << 31;

void run(host::RoutineArg routine)
{
    Regs r;
    call(routine, r);
}

void alert(uint16_t n)
{
    Regs r;
    r.d[0] = n;
    call(AMOS_ROUTINE(ed_pro_alert), r);
}

void set_cursor_shape(uint8_t *shape)
{
    Regs r;
    r.a[1] = address(shape);
    wi_call(wi::SCurWi, r);
}

void count_lines()
{
    Regs r;
    lib_call(L_Prg_CptLines, r);
}

uint8_t *buffer_line(uint16_t y) { return ptr<uint8_t>(window().Edt_BufE) + int16_t(uint16_t(y << 8)); }
uint16_t program_line() { return window().Edt_YPos + window().Edt_YCu; }
uint8_t *even(uint8_t *p) { return ptr<uint8_t>((address(p) + 1) & ~1u); }

// Ed_MemReserve / Ed_MemFree: memory with its size before it, its address
// in *cell
uint8_t *reserve(be_u32 *cell, uint32_t size)
{
    uint8_t *p = ptr<uint8_t>(memory(size + 4, true));
    if (!p)
        return nullptr;
    poke_l(p, size);
    *cell = address(p + 4);
    return p + 4;
}

void release(be_u32 *cell)
{
    if (uint32_t p = *cell) {
        *cell = 0;
        free_memory(p - 4, peek_l(ptr<uint8_t>(p - 4)) + 4);
    }
}

// Ed_BlockLimits: the block's ends in order, the second one at most at the
// end of the program
struct Limits {
    uint16_t y1, y2, x1;
    uint32_t x2;        // and the flags
};

Limits limits()
{
    EditWindow &w = window();
    if (int16_t(w.Edt_YBloc) < 0)
        run(AMOS_ROUTINE(ed_block_what));
    uint16_t y1 = w.Edt_YBloc, y2 = program_line(), x1 = w.Edt_XBloc, x2 = w.Edt_XCu;
    if (!(y2 > y1 || (y2 == y1 && x2 >= x1))) {
        uint16_t t = y1;
        y1 = y2;
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    if (y2 >= program().Prg_NLigne) {
        y2 = program().Prg_NLigne;
        x2 = 0;
    }
    return {y1, y2, x1, x2};
}

// The text of line n (the cursor's, or the program's detokenised): its
// length word and characters; 0 if past the end, closed if it cannot be
// edited
const uint8_t *line_text(uint16_t n, bool &closed, bool &none)
{
    closed = none = false;
    EditWindow &w = window();
    if (n == program_line() && !buffer_line(w.Edt_YCu)[255]) {
        const uint8_t *line = buffer_line(w.Edt_YCu);
        if (!peek_w(line))
            none = true;
        return line;
    }
    uint8_t *line = find_line(n);
    if (!peek_w(line)) {
        none = true;
        return nullptr;
    }
    if (!source::editable(line)) {
        closed = true;
        return nullptr;
    }
    uint8_t *buffer = ptr<uint8_t>(idata().Ed_BufT);
    detok(line, buffer);
    return buffer;
}

// Ed_BlockCopyA0: the block from y1 x1 to y2 x2 copied to *cell; 0 done, 1
// no block, -1 out of memory
int32_t copy(be_u32 *cell, Limits l)
{
    IData &d = idata();
    uint8_t *const texts = ptr<uint8_t>(d.Ed_BufT) + 256;
    uint8_t *a3 = texts;
    uint32_t middle = 0;
    uint32_t flags = l.x2 & 0xFFFF;
    uint16_t x1 = l.x1;
    bool closed, none;
    // The end of the first line
    const uint8_t *text = line_text(l.y1, closed, none);
    if (none)
        return 1;
    if (closed) {
        // a closed procedure: from its start, more than one line
        if (x1 || l.y2 <= l.y1)
            run(AMOS_ROUTINE(ed_not_edit));
        flags |= ClosedFirst;
        *a3++ = 0;
    } else {
        uint16_t length = peek_w(text);
        text += 2;
        if (x1 >= length)
            x1 = length;
        if (l.y1 == l.y2) {
            // all on one line
            uint16_t x2 = uint16_t(flags);
            if (x2 >= length)
                x2 = length;
            flags = (flags & 0xFFFF0000) | x2;
            uint16_t n = x2 - x1;
            if (!n)
                return 1;
            *a3++ = uint8_t(n);
            for (uint16_t i = 0; i < n; i++)
                *a3++ = text[x1 + i];
            flags |= OneLine;
            goto Reserve;
        }
        uint8_t n = uint8_t(length - x1);
        *a3++ = n;
        for (uint16_t i = 0; i < n; i++)
            *a3++ = text[x1 + i];
    }
    // The start of the last line
    *a3++ = 0;
    text = line_text(l.y2, closed, none);
    if (closed) {
        if (uint16_t(flags))
            run(AMOS_ROUTINE(ed_not_edit));
    } else if (!none) {
        uint16_t length = peek_w(text);
        text += 2;
        if (uint16_t(flags) >= length)
            flags = (flags & 0xFFFF0000) | length;
        uint8_t n = uint8_t(flags);
        a3[-1] = n;
        for (uint16_t i = 0; i < n; i++)
            *a3++ = text[i];
    }
    // The size of the lines between
    {
        uint16_t y = l.y1 + (flags & ClosedFirst ? 0 : 1);
        if (y != l.y2 && peek_w(find_line(y))) {
            for (;;) {
                middle += source::line_size(ptr<uint8_t>(window().Edt_CurLigne));
                if (!peek_w(next_line()))
                    break;
                if (++y >= l.y2)
                    break;
            }
        }
        middle += 6;
    }
Reserve:
    uint32_t size = uint32_t(a3 - texts) + middle;
    if (!size)
        return 1;
    release(cell);
    uint8_t *a1 = reserve(cell, size + 24);
    if (!a1)
        return -1;
    poke_w(a1, l.y1);
    poke_w(a1 + 2, l.y2);
    poke_w(a1 + 4, x1);
    poke_l(a1 + 6, flags);
    a1 += 10;
    const uint8_t *a2 = texts;
    uint16_t n = *a2++;
    poke_w(a1, n);
    a1 += 2;
    if (n) {
        while (n--)
            *a1++ = *a2++;
        a1 = even(a1);
    }
    if (flags & OneLine)
        return 0;
    // the lines between
    uint8_t *header = a1;
    poke_w(a1, 0);
    poke_l(a1 + 2, 0);
    a1 += 6;
    poke_w(a1, 0);
    uint16_t y = l.y1 + (flags & ClosedFirst ? 0 : 1);
    if (y < l.y2 && peek_w(find_line(y))) {
        for (;;) {
            poke_w(header, peek_w(header) + 1);
            const uint8_t *line = ptr<uint8_t>(window().Edt_CurLigne);
            uint32_t bytes = source::line_size(line);
            poke_l(header + 2, peek_l(header + 2) + bytes);
            for (uint32_t i = 0; i < bytes; i++)
                *a1++ = line[i];
            if (!peek_w(next_line()))
                break;
            if (++y >= l.y2)
                break;
        }
    }
    // the last line
    n = *a2++;
    poke_w(a1, n);
    a1 += 2;
    while (n--)
        *a1++ = *a2++;
    return 0;
}

// Ed_BlockInsertA0: the block *cell inserted at the cursor
bool insert(be_u32 *cell)
{
    IData &d = idata();
    EditWindow &w = window();
    d.Ed_FUndo++;
    const uint8_t *block = ptr<uint8_t>(*cell);
    if (!block) {
        d.Ed_FUndo--;
        return false;
    }
    uint32_t flags = peek_l(block + 6);
    const uint8_t *a2 = block + 10;
    const uint8_t *line = buffer_line(w.Edt_YCu);
    if (line[255] && peek_w(a2))
        run(AMOS_ROUTINE(ed_not_edit));
    if (w.Edt_XCu > peek_w(line))
        w.Edt_XCu = peek_w(line);
    uint16_t y = program_line();
    if (uint16_t n = peek_w(a2)) {
        insert_text_at_cursor(a2 + 2, n);
        show_current_line();
        goto_x(n + w.Edt_XCu);
    }
    if (!(flags & OneLine)) {
        if (!(flags & ClosedFirst)) {
            return_quiet();
            tokenise_current_line();
            y++;
        }
        a2 = even(const_cast<uint8_t *>(a2) + 2 + peek_w(a2));
        if (uint16_t lines = peek_w(a2)) {
            if (store_block(a2 + 6, peek_l(a2 + 2), y).error)
                run(AMOS_ROUTINE(ed_oof_buf));
            marks_change(y, int16_t(lines));
            y += lines;
        }
        a2 += 6 + peek_l(a2 + 2);
        count_lines();
        goto_y(y);
        new_buffer_all();
        if (uint16_t n = peek_w(a2)) {
            insert_text_at_cursor(a2 + 2, n);
            show_current_line();
            tokenise_current_line();
        }
        goto_x(uint16_t(flags));
    }
    d.Ed_FUndo--;
    return true;
}

// Ed_BlockDeleteA0: the text of the block *cell (where it was copied
// from) deleted
bool remove(be_u32 *cell)
{
    IData &d = idata();
    d.Ed_FUndo++;
    const uint8_t *block = ptr<uint8_t>(*cell);
    if (!block) {
        d.Ed_FUndo--;
        return false;
    }
    uint16_t y1 = peek_w(block), x1 = peek_w(block + 4);
    uint32_t flags = peek_l(block + 6);
    const uint8_t *a2 = block + 10;
    goto_x(x1);
    goto_y(y1);
    const uint8_t *a3 = even(const_cast<uint8_t *>(a2) + 2 + peek_w(a2));
    if (flags & ClosedFirst) {
        delete_lines(y1, peek_l(a3 + 2));
        marks_change(y1, int16_t(-peek_w(a3)));
        count_lines();
        new_buffer_all();
        a3 += 6 + peek_l(a3 + 2);
        goto_x(0);
        if (uint16_t n = peek_w(a3)) {
            delete_text_at_cursor(n);
            tokenise_current_line();
        }
    } else {
        if (!(flags & OneLine) && peek_w(a3)) {
            delete_lines(y1 + 1, peek_l(a3 + 2));
            // (the original passed the marks the line in another register)
            marks_change(y1 + 1, int16_t(-peek_w(a3)));
            count_lines();
            new_buffer_all();
        }
        if (uint16_t n = peek_w(a2)) {
            delete_text_at_cursor(n);
            show_current_line();
        }
        if (!(flags & OneLine)) {
            tokenise_current_line();
            a3 += 6 + peek_l(a3 + 2);
            goto_x(0);
            goto_y(y1 + 1);
            if (uint16_t n = peek_w(a3)) {
                delete_text_at_cursor(n);
                tokenise_current_line();
            }
            join_lines();
            tokenise_current_line();
        }
    }
    d.Ed_FUndo--;
    return true;
}

void block_cursor() { set_cursor_shape(ed_cursor_block); }

void hide()
{
    window().Edt_YBloc = 0xFFFF;
    show_all();
    set_cursor_shape(ed_cursor_normal);
}

} // namespace

namespace amos::ed {

#if !defined(__m68k__)
// Preserve interoperability with the original private block paste command.
bool clipboard_mirror_block() { return copy(&idata().Ed_Block, limits()) == 0; }
#endif

// Ed_BlocOn: marking starts, or the block is forgotten
void block_on()
{
    EditWindow &w = window();
    if (int16_t(w.Edt_YBloc) >= 0) {
        hide();
        return;
    }
    w.Edt_YBloc = program_line();
    w.Edt_XBloc = w.Edt_XCu;
    w.Edt_YOldBloc = 0;
    block_cursor();
}

} // namespace amos::ed

extern "C" {
// Ed_BlocAff: while marking, the lines between the cursor's old and new
// line shown again
void ed_block_show(AllRegs *)
{
    EditWindow &w = window();
    if (int16_t(w.Edt_YBloc) < 0 || int16_t(w.Edt_YOldBloc) < 0)
        return;
    uint16_t a = w.Edt_YOldBloc, b = program_line();
    w.Edt_YOldBloc = 0xFFFF;
    if (b < a) {
        uint16_t t = a;
        a = b;
        b = t;
    }
    for (; a <= b; a++) {
        int16_t y = int16_t(a - w.Edt_YPos);
        if (y >= 0 && uint16_t(y) < w.Edt_WindTy) {
            show_line(uint16_t(y), false);
            place_cursor();
        }
    }
}

void ed_block_on(AllRegs *) { block_on(); }

void ed_block_cursor(AllRegs *) { block_cursor(); }
void ed_block_hide(AllRegs *) { hide(); }
void ed_cursor_normal_fn(AllRegs *) { set_cursor_shape(ed_cursor_normal); }

// Ed_BlocAll: all the text
void ed_block_all(AllRegs *)
{
    EditWindow &w = window();
    block_cursor();
    w.Edt_XCu = 0;
    w.Edt_YCu = 0;
    w.Edt_XPos = 0;
    w.Edt_YPos = 0;
    w.Edt_XBloc = 0;
    w.Edt_YBloc = program().Prg_NLigne;
    new_buffer_all();
}

// Ed_BlocStore / Ed_BlocCut
void ed_block_store(AllRegs *)
{
    int32_t r = copy(&idata().Ed_Block, limits());
    if (r < 0)
        run(AMOS_ROUTINE(ed_out_of_memory));
    if (r > 0)
        run(AMOS_ROUTINE(ed_block_what));
    hide();
    alert(7);
}

void ed_block_cut(AllRegs *)
{
    int32_t r = copy(&idata().Ed_Block, limits());
    if (r < 0)
        run(AMOS_ROUTINE(ed_out_of_memory));
    if (r > 0)
        run(AMOS_ROUTINE(ed_block_what));
    hide();
    remove(&idata().Ed_Block);
    undo_reset();
    alert(7);
}

// Ed_BlocForget / Ed_BlocPaste / Ed_BlocFree
void ed_block_forget(AllRegs *)
{
    if (!idata().Ed_Block)
        run(AMOS_ROUTINE(ed_block_what));
    release(&idata().Ed_Block);
    alert(8);
}

void ed_block_paste(AllRegs *)
{
    if (!idata().Ed_Block)
        run(AMOS_ROUTINE(ed_block_what));
    insert(&idata().Ed_Block);
    undo_reset();
}

void ed_block_free(AllRegs *) { release(&idata().Ed_Block); }

// Ed_BlockFreeA0: the block *a0 freed
void ed_block_free_a0(AllRegs *r) { release(ptr<be_u32>(r->a[0])); }

// Ed_BlockLimits: d4/d6 the start, d5/d7 the end
void ed_block_limits(AllRegs *r)
{
    Limits l = limits();
    r->d[4] = l.y1;
    r->d[5] = (r->d[5] & 0xFFFF0000) | l.y2;
    r->d[6] = (r->d[6] & 0xFFFF0000) | l.x1;
    r->d[7] = (r->d[7] & 0xFFFF0000) | uint16_t(l.x2);
}

// Ed_BlockCopyA0 / Ed_BlockInsertA0 / Ed_BlockDeleteA0: a0 the block's
// address; d0 (and the condition codes) as described above, -1 for
// inserted/deleted
void ed_block_copy(AllRegs *r)
{
    Limits l = {uint16_t(r->d[4]), uint16_t(r->d[5]), uint16_t(r->d[6]), r->d[7]};
    r->d[0] = uint32_t(copy(ptr<be_u32>(r->a[0]), l));
}

void ed_block_insert(AllRegs *r) { r->d[0] = insert(ptr<be_u32>(r->a[0])) ? 0xFFFFFFFF : 0; }
void ed_block_delete(AllRegs *r) { r->d[0] = remove(ptr<be_u32>(r->a[0])) ? 0xFFFFFFFF : 0; }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_block.inc"
#endif
