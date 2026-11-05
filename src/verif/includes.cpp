// Includes, equates and compiled procedures (+Verif.s): the programs of
// Include lines put in the source before the test (Get_Includes), the
// values of equates read from the equates file (Equ_Verif), procedures
// compiled by APCmp relocated (Ver_APCmp).
//
// The includes: Prg_Includes is a table of 20-byte entries, one per part
// of the source before an Include (and one for the rest):
//   +0 where the part ends: the Include line, +4 the part's length,
//   +8 the file to include (open while getting them), +12 its program's
//   length, +16 the length of the Include line.
// Prg_FullSource is the source with the programs in place of the lines.

#include "verif/verif.h"

#include "core/amiga.h"
#include "core/source.h"

using namespace amos;
using namespace amos::verif;

// lib/strings.cpp: InstrFind
extern "C" uint32_t lib_instr_find(const uint8_t *hay, uint32_t hay_len, const uint8_t *needle, uint32_t needle_len,
                                   uint32_t start);
// includes_glue.s: the routine record of Equ_Free, called when memory is
// short


namespace {

constexpr int EntrySize = 20;
constexpr uint16_t EIncludeDisk = 45, EIncludeNotAmos = 46, EEquatesFile = 52, ENoEquate = 51, EBadEquates = 53;

// Get_Includes failed: VerPos at entry's line, the files closed, no
// includes
[[noreturn]] void include_error(uint16_t n, const uint8_t *entry, int count)
{
    IData &d = idata();
    d.VerPos = peek_l(entry);
    file_close();
    uint8_t *e = ptr<uint8_t>(d.Prg_Includes);
    for (; count; count--, e += EntrySize)
        if (uint32_t f = peek_l(e + 8)) {
            poke_l(e + 8, 0);
            d.Handle = f;
            file_close();
        }
    includes_clear();
    fail(n);
}

// Equ_Load: the equates file in memory (once; freed when memory is
// short). Returns it, *length its length.
const uint8_t *equates_load(uint32_t *length)
{
    IData &d = idata();
    if (!d.Equ_Base) {
        Regs f;
        f.a[1] = address(ver_equ_flush);
        w::add_routine(f.a[1], address(&wdata().MemFlush));
        Regs m;
        m.d[0] = 9;
        m.a[0] = address(boot::sys_message(m.d[0]));
        boot::sys_add_path(ptr<char>(m.a[0]));
        bool ok = file_open(1005);
        if (ok) {
            file_seek(0, 1);
            uint32_t size = file_seek(0, -1);
            ok = a5_reserve(&d.Equ_Base, size, MemFast | MemPublic) && file_read(ptr<void>(d.Equ_Base), size);
        }
        file_close();
        if (!ok) {
            equates_free();
            fail(EEquatesFile);
        }
    }
    *length = peek_l(ptr<uint8_t>(d.Equ_Base) - 4);
    return ptr<uint8_t>(d.Equ_Base);
}

// L_ValRout on the number at p (a4 = AdTokens, as for the editor): false
// if there is none or it is a float, else its value and end
bool integer(const uint8_t *p, uint32_t *value, const uint8_t **end)
{
    Ctx &c = ctx();
    uint8_t *a4 = c.a4;
    c.a4 = ptr<uint8_t>(idata().AdTokens[0]);
    Regs r;
    r.a[0] = address(p);
    r.d[0] = 0;
    amos_call(&r, lib_address(L_ValRout));
    c.a4 = a4;
    if (r.a[0] == address(p))
        return false;
    uint16_t token = uint16_t(r.d[1]);
    if (token == tk::Fl || token == tk::DFl)
        return false;
    *value = r.d[0];
    *end = ptr<uint8_t>(r.a[0]);
    return true;
}

} // namespace

namespace amos::verif {

// Includes_Clear
void includes_clear()
{
    IData &d = idata();
    a5_free(&d.Prg_FullSource);
    a5_free(&d.Prg_Includes);
}

// Includes_Adr: an address in the full source as in the source, or the
// Include line of an included part
uint32_t includes_address(uint32_t at)
{
    IData &d = idata();
    if (!d.Prg_Includes)
        return at;
    const uint8_t *e = ptr<uint8_t>(d.Prg_Includes) - EntrySize;
    uint32_t offset = at - d.Prg_FullSource;
    uint32_t removed = 0, end = 0;
    for (;;) {
        e += EntrySize;
        end += peek_l(e + 4);
        if (offset < end)
            return offset - removed + d.Prg_Source;
        uint32_t included = peek_l(e + 12);
        if (!included)
            return offset;      // as the original: past everything
        removed += included - peek_l(e + 16);
        end += included;
        if (offset < end)
            return peek_l(e);
    }
}

// Get_Includes: the programs of the Include lines into Prg_FullSource
// (none: no includes)
void includes_get()
{
    IData &d = idata();
    includes_clear();
    // The original had room for 15 includes and went on past it: the
    // table is made to fit
    int lines = 0;
    for (uint8_t *l = ptr<uint8_t>(d.Prg_Source); peek_w(l); l = source::find_line(l, 1).line)
        if (peek_w(l + 2) == tk::Incl)
            lines++;
    if (!a5_reserve(&d.Prg_Includes, EntrySize * (lines < 15 ? 16 : lines + 1), MemPublic | MemClear))
        return;
    uint8_t *e = ptr<uint8_t>(d.Prg_Includes);
    uint8_t *l = ptr<uint8_t>(d.Prg_Source);
    poke_l(e, address(l));
    uint32_t total = 0;
    int count = 0;
    while (peek_w(l)) {
        if (peek_w(l + 2) != tk::Incl) {
            l = source::find_line(l, 1).line;
            continue;
        }
        not_13();
        count++;
        uint32_t part = address(l) - peek_l(e);
        poke_l(e, address(l));
        total += part;
        poke_l(e + 4, part);
        // the file's name, a string constant
        if (peek_w(l + 4) != tk::Ch1)
            include_error(ESyntax, e, count);
        uint16_t length = peek_w(l + 6);
        if (!length || length > 107)
            include_error(ESyntax, e, count);
        char *name = ptr<char>(d.Name1);
        for (uint16_t i = 0; i < length; i++)
            name[i] = l[8 + i];
        name[length] = 0;
        if (!file_open(1005) || !file_read(ptr<void>(d.Buffer), 16 + 4))
            include_error(EIncludeDisk, e, count);
        const char *header = ptr<char>(d.Buffer);
        if (program_kind(header) < 0)
            include_error(EIncludeNotAmos, e, count);
        uint32_t size = peek_l(header + 16);
        total += size;
        poke_l(e + 12, size);
        poke_l(e + 8, d.Handle);
        d.Handle = 0;
        l = source::find_line(l, 1).line;
        poke_l(e + 16, address(l) - peek_l(e));
        e += EntrySize;
        poke_l(e, address(l));
    }
    uint32_t part = address(l) - peek_l(e);
    poke_l(e, address(l));
    poke_l(e + 4, part);
    if (!count) {
        includes_clear();
        return;
    }
    if (!a5_reserve(&d.Prg_FullSource, part + total + 4, MemPublic))
        include_error(EOutOfMemory, e, count);
    uint8_t *to = ptr<uint8_t>(d.Prg_FullSource);
    const uint8_t *from = ptr<uint8_t>(d.Prg_Source);
    for (e = ptr<uint8_t>(d.Prg_Includes);; e += EntrySize) {
        uint32_t n = peek_l(e + 4) / 2;
        for (uint32_t i = 0; i < n; i++, to += 2, from += 2)
            poke_w(to, peek_w(from));
        uint32_t file = peek_l(e + 8);
        if (!file)
            break;
        d.Handle = file;
        uint32_t size = peek_l(e + 12);
        if (!file_read(to, size))
            include_error(EIncludeDisk, e, count);
        to += size;
        file_close();
        poke_l(e + 8, 0);
        from += peek_l(e + 16);
    }
    poke_l(to, 0);
}

// Equ_Free
void equates_free() { a5_free(&idata().Equ_Base); }

// Ver_APCmp: a procedure compiled by APCmp (at its APCmp token) relocated
// to where it is; its maths and accessory flags taken
void relocate_compiled(uint8_t *procedure)
{
    IData &d = idata();
    uint8_t *header = procedure + 2;
    uint16_t flags = peek_w(header);
    d.MathFlags |= uint8_t(flags);
    d.Prg_Accessory = uint8_t(flags >> 8);
    const uint8_t *table = header + 2 + peek_l(header + 2);
    uint8_t *at = header + 16;
    uint32_t delta = address(at) - peek_l(header + 10);
    if (!delta)
        return;
    poke_l(header + 10, address(at));
    while (uint8_t b = *table++) {
        if (b == 1) {
            at += 508;
            continue;
        }
        at += 2 * b;
        poke_l(at, peek_l(at) + delta);
    }
}

// Equ_Verif: the equate (a string constant at pc, header before its name)
// of data (its value, its type, a flag) found in the equates' file once,
// then kept; pc past the string
void equate(const uint8_t *header, uint8_t *data)
{
    not_13();
    if (!(data[5] & 0x80)) {
        // header, the name and ':' to find in the file
        IData &d = idata();
        uint8_t *key = ptr<uint8_t>(d.Buffer);
        uint8_t *k = key;
        while ((*k++ = *header++))
            ;
        k--;
        uint16_t t = word(pc);
        if (t != tk::Ch1 && t != tk::Ch2)
            fail(ESyntax);
        uint16_t length = word(pc + 2);
        if (!length || length >= 127)
            fail(ESyntax);
        for (uint16_t i = 0; i < length; i++)
            *k++ = pc[4 + i];
        *k++ = ':';
        uint32_t size;
        const uint8_t *file = equates_load(&size);
        uint32_t at = lib_instr_find(file, size, key, k - key, 0);
        if (!at)
            fail(ENoEquate);
        const uint8_t *p = file + at - 1;
        while (*p++ != ':')
            ;
        bool negative = *p == '-';
        if (negative)
            p++;
        uint32_t value;
        if (!integer(p, &value, &p))
            fail(EBadEquates);
        if (negative)
            value = -value;
        poke_l(data, value);
        if (*p++ != ',')
            fail(EBadEquates);
        uint8_t type = uint8_t(*p - '0');
        if (type > 7)
            fail(EBadEquates);
        data[4] = type;
        data[5] |= 0x80;
    }
    uint16_t length = word(pc + 2);
    pc += 4 + length + (length & 1);
}

} // namespace amos::verif

extern "C" {
void ver_includes_clear(AllRegs *) { includes_clear(); }
// a0 = the address
void ver_includes_address(AllRegs *r) { r->a[0] = includes_address(r->a[0]); }
void ver_equ_free(AllRegs *) { equates_free(); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/verif_includes.inc"
#endif
