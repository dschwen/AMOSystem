// The undo buffer of a program (+Edit.s "GESTION UNDO MULTIPLE"): a ring of
// 6-byte entries (Prg_Undo, Prg_LUndo bytes, $FF at both ends), Prg_PUndo
// the next one to write. An entry: the kind of change (bit 7: with a block
// of data), the column, the line; with data, bytes 2-5 point to the block
// instead: its size word, the line word, then what the change needs.
// Undo and Redo call the kind's routine of +Edit.s (JUndo, JRedo) with a2
// on the entry.

#include "editor/editor.h"
#include "editor/clipboard.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

// Un_Del: the entry emptied, its block freed
void clear(Program &p, uint8_t *entry)
{
    if (entry[0] & 0x80) {
        if (uint32_t block = peek_l(entry + 2)) {
            uint32_t size = peek_w(ptr<uint8_t>(block));
#if !defined(__m68k__)
            if (entry[0] == 0x89) size = peek_l(ptr<uint8_t>(block));
#endif
            p.Prg_TUndo -= size;
            free_memory(block, size);
        }
    }
    entry[0] = 0;
}

// Un_Prev / Un_Next: around the ring
uint8_t *previous(Program &p, uint8_t *entry)
{
    entry -= 6;
    if (entry[0] == 0xFF)
        entry = ptr<uint8_t>(p.Prg_Undo + p.Prg_LUndo) - 12;
    return entry;
}

uint8_t *next(Program &p, uint8_t *entry)
{
    entry += 6;
    if (entry[0] == 0xFF)
        entry = ptr<uint8_t>(p.Prg_Undo) + 6;
    return entry;
}

// Un_Avance: the next entry emptied and made the current one
void advance(Program &p)
{
    uint8_t *entry = next(p, ptr<uint8_t>(p.Prg_PUndo));
    clear(p, entry);
    p.Prg_PUndo = address(entry);
}

void reset(Program &p)
{
    if (!p.Prg_Undo)
        return;
    uint8_t *entry = ptr<uint8_t>(p.Prg_Undo) + 6;
    p.Prg_PUndo = address(entry);
    do {
        clear(p, entry);
        entry += 6;
    } while (entry[0] != 0xFF);
}

void free_buffer(Program &p)
{
    reset(p);
    if (uint32_t buffer = p.Prg_Undo) {
        p.Prg_Undo = 0;
        free_memory(buffer, p.Prg_LUndo);
    }
}

void create(Program &p)
{
    free_buffer(p);
    uint32_t n = idata().Ed_NUndo;
    if (!n)
        return;
    uint32_t size = uint32_t(uint16_t(n + 3)) * 6;
    uint8_t *buffer = ptr<uint8_t>(memory(size, true));
    if (!buffer)
        return;
    p.Prg_LUndo = size;
    p.Prg_Undo = address(buffer);
    buffer[0] = 0xFF;
    p.Prg_PUndo = address(buffer + 6);
    buffer[size - 12] = 0xFF;
    p.Prg_TUndo = 0;
}

// The same for all the programs
void for_all(void (*f)(Program &))
{
    for (uint32_t p = idata().Prg_List; p; p = ptr<Program>(p)->Prg_Next)
        f(*ptr<Program>(p));
}

} // namespace

namespace amos::ed {

// Un_Debut: a new entry for the cursor's place, unless undo is off (while
// undoing) or there is no buffer
uint8_t *undo_start()
{
    IData &d = idata();
    Program &p = program();
    if (d.Ed_FUndo || !p.Prg_Undo)
        return nullptr;
    uint8_t *entry = ptr<uint8_t>(p.Prg_PUndo);
    clear(p, entry);
    EditWindow &w = window();
    entry[1] = uint8_t(w.Edt_XCu);
    poke_w(entry + 2, uint16_t(w.Edt_YPos + w.Edt_YCu));
    advance(p);
    return entry;
}

// Prg_UndoRaz / Prg_UndoFree of the current program
void undo_reset() { reset(program()); }
void undo_free() { free_buffer(program()); }

// Un_CLine: the n characters at text kept in a new entry (kind $83: a
// cleared part of a line); the entry, or 0
uint8_t *undo_text(const uint8_t *text, uint16_t n)
{
    uint8_t *entry = undo_start();
    if (!entry)
        return nullptr;
    uint16_t size = n + 4;
    uint8_t *block = ptr<uint8_t>(memory(size, false));
    // (the original said it kept them without memory, and its caller
    // marked the entry as having a block it did not have)
    if (!block)
        return nullptr;
    Program &p = program();
    poke_w(block, size);
    p.Prg_TUndo += size;
    poke_w(block + 2, peek_w(entry + 2));
    entry[0] = 0x83;
    poke_l(entry + 2, address(block));
    for (uint16_t i = 0; i < n; i++)
        block[4 + i] = text[i];
    return entry;
}

} // namespace amos::ed

namespace {

// Undo, Redo of an entry (editor/lines.cpp), undo off meanwhile
void act(void (*change)(uint8_t *), uint8_t *entry)
{
    IData &d = idata();
    d.Ed_FUndo++;
    change(entry);
    d.Ed_FUndo--;
}

} // namespace

extern "C" {
// Un_Debut: the condition codes EQ and a2 the entry if there is one
uint32_t ed_undo_start(AllRegs *r)
{
    uint8_t *entry = undo_start();
    if (!entry)
        return 1;
    r->a[2] = address(entry);
    return 0;
}

// Un_CLine: the d0 characters at a0 kept for undo; EQ if kept, a2 kept
uint32_t ed_undo_line(AllRegs *r) { return !undo_text(ptr<uint8_t>(r->a[0]), uint16_t(r->d[0])); }

void ed_undo_advance(AllRegs *) { advance(program()); }
void ed_undo_clear(AllRegs *r) { clear(program(), ptr<uint8_t>(r->a[2])); }
void ed_undo_previous(AllRegs *r) { r->a[2] = address(previous(program(), ptr<uint8_t>(r->a[2]))); }
void ed_undo_next(AllRegs *r) { r->a[2] = address(next(program(), ptr<uint8_t>(r->a[2]))); }

// Prg_UndoCreate / Prg_UndoRaz / Prg_UndoFree of the program a6, and the
// same for all of them
void ed_undo_create(AllRegs *) { create(program()); }
void ed_undo_reset(AllRegs *) { reset(program()); }
void ed_undo_free(AllRegs *) { free_buffer(program()); }
void ed_undo_create_all(AllRegs *) { for_all(create); }
void ed_undo_reset_all(AllRegs *) { for_all(reset); }
void ed_undo_free_all(AllRegs *) { for_all(free_buffer); }

// Ed_Undo / Ed_Redo
void ed_undo(AllRegs *)
{
    Program &p = program();
    uint8_t *entry = previous(p, ptr<uint8_t>(p.Prg_PUndo));
    if (!entry[0]) {
        Regs r;
        call(ed_no_undo, r);
        return;
    }
    p.Prg_PUndo = address(entry);
    act(undo_change, entry);
}

void ed_redo(AllRegs *)
{
    Program &p = program();
    uint8_t *entry = ptr<uint8_t>(p.Prg_PUndo);
    if (!entry[0]) {
        Regs r;
        call(ed_no_redo, r);
        return;
    }
    act(redo_change, entry);
    Program &q = program();
    q.Prg_PUndo = address(next(q, ptr<uint8_t>(q.Prg_PUndo)));
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_undo.inc"
#endif
