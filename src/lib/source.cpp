// The register entries of the line walking routines (core/source.cpp) for
// the assembler: +Verif.s Tk_FindL, Tk_SizeL, Tk_EditL, Tk_FindA, which
// the library and the editor reach through Lib_Int.

#include "core/amos.h"
#include "core/source.h"

using namespace amos;

extern "C" {
// Tk_FindL: d0 the line, a0 the start; a0 the line, a1 the open procedure,
// d0.w its first word (the condition codes: 0 at the end)
uint32_t lib_tk_find_line(RegFrame *r)
{
    source::Found f = source::find_line(ptr<uint8_t>(r->a[0]), uint16_t(r->d[0]));
    r->a[0] = address(f.line);
    r->a[1] = address(f.procedure);
    uint16_t first = peek_w(f.line);
    r->d[0] = (r->d[0] & 0xFFFF0000) | first;
    return uint32_t(int32_t(int16_t(first)));
}

// Tk_FindN: the first line
uint32_t lib_tk_find_first(RegFrame *r)
{
    r->d[0] = 1;
    return lib_tk_find_line(r);
}

// Tk_SizeL: d0 the size of the line at a0
uint32_t lib_tk_size_line(RegFrame *r)
{
    r->d[0] = source::line_size(ptr<uint8_t>(r->a[0]));
    return r->d[0];
}

// Tk_EditL: the condition codes NE if the line at a0 can be edited
uint32_t lib_tk_editable(RegFrame *r) { return source::editable(ptr<uint8_t>(r->a[0])); }

// Tk_FindA: a1 the start, a0 an address; d0 the line number, a0 its start,
// d1 the closed procedure it is in
uint32_t lib_tk_find_address(RegFrame *r)
{
    source::Number n = source::find_address(ptr<uint8_t>(r->a[1]), ptr<uint8_t>(r->a[0]));
    r->d[0] = n.number;
    r->d[1] = address(n.procedure);
    r->a[0] = address(n.line);
    r->a[1] = address(n.line);
    return n.number;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_source.inc"
#endif
