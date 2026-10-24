// Field #n,length As var$,... (+ILib.s): the record of a random access
// file, for Get and Put (lib/files.cpp): the number of fields, the
// record's length, the file's length, then each field's length and its
// variable's address.

#include "interp/interp.h"

#include "core/amiga.h"

#define DOS_BASE_NAME (reinterpret_cast<struct DosLibrary *>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;
using namespace amos::interp;

namespace {

constexpr uint32_t StringMax = 0xFFC0;

// The fields wrong: the file closed
[[noreturn]] void bad_fields(uint32_t channel)
{
    Regs r;
    r.a[2] = channel;
    lib_call(L_Cloa1, r);
    fon_call();
}

} // namespace

extern "C" void interp_in_field()
{
    Ctx &c = ctx();
    Regs g;
    g.d[0] = integer().v1;
    lib_call(L_GetFile, g);
    if (!g.d[1])
        error(79 + 18);
    uint8_t *channel = ptr<uint8_t>(g.a[2]);
    uint8_t *top = c.a3;
    uint16_t count = 0;
    do {
        pc() += 2;
        uint32_t length = integer().v1;
        c.a3 -= 4;
        poke_l(c.a3, length);
        pc() += 2;
        uint8_t *at = find_variable().at;
        c.a3 -= 4;
        poke_l(c.a3, address(at));
        count++;
    } while (peek_token() == tk::Vir);
    Regs m;
    m.d[0] = count * 6u + 8;
    m.a[0] = w::reserve(m.d[0], w::FastClear);
    if (!m.a[0])
        error(24);
    uint8_t *record = ptr<uint8_t>(m.a[0]);
    poke_l(channel + 6, m.a[0]);
    poke_w(record, count);
    uint8_t *field = record + 8;
    uint32_t total = 0;
    const uint8_t *from = top;
    for (uint16_t i = 0; i < count; i++, field += 6) {
        from -= 4;
        uint32_t length = peek_l(from);
        from -= 4;
        if (!length)
            bad_fields(address(channel));
        total += length;
        if (total >= StringMax)
            bad_fields(address(channel));
        poke_w(field, uint16_t(length));
        poke_l(field + 2, peek_l(from));
    }
    poke_w(record + 2, uint16_t(total));
    c.a3 = top;
    BPTR file = BPTR(peek_l(channel));
    Seek(file, 0, OFFSET_END);
    poke_l(record + 4, uint32_t(Seek(file, 0, OFFSET_BEGINNING)));
}
