// Storing lines in the program (+Edit.s "STOCKAGE DU PROGRAMME"): the
// program lies at the top of its buffer, from Prg_StBas up; it grows and
// shrinks downwards, Prg_StMini the lowest it may reach.

#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

namespace {

constexpr uint8_t EtAFree = 1 << 5;     // Edt_EtatAff: the variables to clear

void changed()
{
    window().Edt_EtatAff |= EtAFree;
    program().Prg_Change = 1;
    program().Prg_StModif = 1;
}

// Moves the words from..to (exclusive) down by n bytes
void move_down(be_u16 *from, const be_u16 *to, uint32_t n)
{
    be_u16 *a3 = from - n / 2;
    while (from < to)
        *a3++ = *from++;
}

} // namespace

namespace amos::ed {

// Ed_FindL: line n of the program, kept as the current one with the start
// of the open procedure it is in
uint8_t *find_line(uint16_t n)
{
    source::Found f = source::find_line(ptr<uint8_t>(program().Prg_StBas), n);
    window().Edt_CurLigne = address(f.line);
    window().Edt_DebProc = address(f.procedure);
    return f.line;
}

// Ed_NextL: the line after the current one
uint8_t *next_line()
{
    source::Found f = source::find_line(ptr<uint8_t>(window().Edt_CurLigne), 1);
    window().Edt_CurLigne = address(f.line);
    window().Edt_DebProc = address(f.procedure);
    return f.line;
}

// Ed_StDelChunk: the bytes from..to out of the program
void delete_chunk(uint8_t *from, uint8_t *to)
{
    Program &p = program();
    auto a0 = reinterpret_cast<be_u16 *>(from), a1 = reinterpret_cast<be_u16 *>(to);
    const be_u16 *bottom = ptr<be_u16>(p.Prg_StBas);
    while (a0 > bottom)
        *--a1 = *--a0;
    p.Prg_StBas = address(a1);
    changed();
}

// Ed_Stocke / Ed_StoBlock: the tokenised line (or a block of length bytes)
// at line position: inserted, or in place of the line there. At the end of
// the program an empty line is only stored when inserted.
namespace {

Stored put(const uint8_t *a1, uint32_t d2, uint16_t position, bool insert, bool block)
{
    Program &p = program();
    uint8_t *a0 = find_line(position);
    uint32_t added = 0, d0;
    if (!block) {
        uint32_t d1 = uint32_t(a0[0]) * 2;
        if (!d1) {
            // the end of the program
            if (d2 == 4 && !insert)
                return {0, 0, a0};
            goto StoI;
        }
        if (insert)
            goto StoI;
        // In place of the line
        if (peek_w(a0 + 2) == tk::Proc && int16_t(peek_w(a0 + 10)) < 0)
            return {-1, 0, a0};
        if (d1 == d2)
            goto StoCop;
        if (d2 > d1) {
            d0 = d2 - d1;
            goto StoI0;
        }
        // shorter: the program before it moves up
        {
            uint32_t delta = d1 - d2;
            auto a2 = reinterpret_cast<be_u16 *>(a0);
            a0 += delta;
            auto a3 = reinterpret_cast<be_u16 *>(a0);
            const be_u16 *bottom = ptr<be_u16>(p.Prg_StBas);
            while (a2 > bottom)
                *--a3 = *--a2;
            p.Prg_StBas = address(a3);
        }
        goto StoCop;
    }
StoI:
    d0 = d2;
    added = 1;
StoI0:
    {
        auto a2 = ptr<be_u16>(p.Prg_StBas);
        uint32_t a3 = p.Prg_StBas - d0;
        if (a3 <= p.Prg_StMini)
            return {1, 0, a0};
        p.Prg_StBas = a3;
        move_down(a2, reinterpret_cast<be_u16 *>(a0), d0);
        a0 -= d0;
    }
StoCop:
    {
        auto from = reinterpret_cast<const be_u16 *>(a1);
        auto to = reinterpret_cast<be_u16 *>(a0);
        for (uint32_t n = d2 >> 1; n; n--)
            *to++ = *from++;
    }
    if (insert)
        marks_change(position, 1);
    changed();
    return {0, added, a0};
}

} // namespace

Stored store(const uint8_t *line, uint16_t position, bool insert)
{
    return put(line, uint32_t(line[0]) * 2, position, insert, false);
}

Stored store_block(const uint8_t *block, uint32_t length, uint16_t position)
{
    return put(block, length, position, false, true);
}

void delete_lines(uint16_t n, uint32_t bytes)
{
    uint8_t *line = find_line(n);
    if (peek_w(line))
        delete_chunk(line, line + bytes);
}

// Ed_DeLigne: line n out of the program: 0 done, 1 none there, -1 a closed
// procedure
int32_t delete_line(uint16_t n)
{
    uint8_t *a0 = find_line(n);
    if (!peek_w(a0))
        return 1;
    if (peek_w(a0 + 2) == tk::Proc && int16_t(peek_w(a0 + 10)) < 0)
        return -1;
    delete_chunk(a0, a0 + uint32_t(a0[0]) * 2);
    marks_change(n, -1);
    program().Prg_NLigne--;
    return 0;
}

} // namespace amos::ed

extern "C" {
// Ed_Stocke: a1 the line, d1 the position, d0.w insert; d0 the error, d1 a
// line more, a0 the line in the program
void ed_store(AllRegs *r)
{
    Stored s = store(ptr<uint8_t>(r->a[1]), uint16_t(r->d[1]), uint16_t(r->d[0]));
    r->d[0] = uint32_t(s.error);
    if (!s.error) {
        r->d[1] = s.added;
        r->a[0] = address(s.line);
    }
}

// Ed_StoBlock: a0 the block, d0 its length, d1 the line
void ed_store_block(AllRegs *r)
{
    Stored s = store_block(ptr<uint8_t>(r->a[0]), r->d[0], uint16_t(r->d[1]));
    r->d[0] = uint32_t(s.error);
    if (!s.error) {
        r->d[1] = s.added;
        r->a[0] = address(s.line);
    }
}

// Ed_DeLigne: d1 the line; d0 0, 1 or -1
void ed_delete_line(AllRegs *r) { r->d[0] = uint32_t(delete_line(uint16_t(r->d[1]))); }

// Ed_DelChunk: d0 bytes from line d1
void ed_delete_chunk_at(AllRegs *r) { delete_lines(uint16_t(r->d[1]), r->d[0]); }

// Ed_StDelChunk: a0..a1 out of the program (d1 = a0)
void ed_delete_chunk(AllRegs *r)
{
    r->d[1] = r->a[0];
    delete_chunk(ptr<uint8_t>(r->a[0]), ptr<uint8_t>(r->a[1]));
}

// Ed_FindL / Ed_NextL: a0 the line (d0: line n), d0.w its first word
void ed_find_line(AllRegs *r)
{
    uint8_t *line = find_line(uint16_t(r->d[0]));
    r->a[0] = address(line);
    r->d[0] = (r->d[0] & 0xFFFF0000) | peek_w(line);
}

void ed_next_line(AllRegs *r)
{
    uint8_t *line = next_line();
    r->a[0] = address(line);
    r->d[0] = (r->d[0] & 0xFFFF0000) | peek_w(line);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_storage.inc"
#endif
