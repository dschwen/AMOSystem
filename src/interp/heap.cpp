// The string space (+ILib.s String.s): Demande (room for a new string at
// HiChaine), DDemande, and Menage, the garbage collection.
//
// Strings live between LoChaine (the empty string) and HiChaine, below the
// variables. When a new one does not fit, Menage packs the strings the
// variables point to, and Demande starts the instruction again from its
// beginning (d7: where it started; 0 after a restart, so that a second
// collection in the same instruction is "out of string space"; negative
// in DDemande: no restart, Demande comes back).
//
// The variables are walked chunk by chunk from VarLoc: a slot is a header
// word (its length in the high byte, the flags in the low one: bit 1 a
// string, bit 6 an array), then the value; a word $FFFF ends a chunk and
// is followed by the next chunk (a procedure's caller's), 0 at the end.

#include "runtime/interpreter_bridge.h"
#include "interp/interp.h"

using namespace amos;

extern "C" {
// control_glue.s: an instruction run again from its beginning
}

namespace {

constexpr uint32_t TMenage = 160 * 10 - 64;     // the slow collection's table
constexpr uint32_t Moved = 0xFFC00000;          // a string copied already

IData &data() { return idata(); }

// Every string slot of the variables (each element of a string array)
template <typename F> void each_string(F visit)
{
    const uint8_t *v = ptr<uint8_t>(data().VarLoc);
    for (;;) {
        uint16_t header = peek_w(v);
        v += 2;
        if (!header)
            continue;
        if (int16_t(header) < 0) {
            uint32_t next = peek_l(v);
            if (!next)
                return;
            v = ptr<uint8_t>(next);
            continue;
        }
        if (!(header & 2)) {
            v += header >> 8;
            continue;
        }
        uint8_t *slot = const_cast<uint8_t *>(v);
        uint32_t value = peek_l(v);
        v += 4;
        if (!value)
            continue;
        if (!(header & (1 << 6))) {
            visit(slot);
            continue;
        }
        uint8_t *a = ptr<uint8_t>(value);
        uint16_t dims = *a;
        a += 2;
        uint32_t count = 1;
        for (uint16_t i = 0; i < dims; i++, a += 4)
            count = (count & 0xFFFF) * uint16_t(peek_w(a) + 1);
        for (; count; count--, a += 4)
            visit(a);
    }
}

// The fast collection: every string copied in order into a buffer as large
// as the string space, then back
bool fast()
{
    IData &d = data();
    uint32_t low = d.LoChaine, high = d.HiChaine;
    uint32_t size = high - low;
    if (size >= 0x3FFFFE * 2)
        return false;
    Regs m;
    m.d[0] = size;
    m.a[0] = w::reserve(m.d[0], w::Fast);
    if (!m.a[0])
        return false;
    uint8_t *buffer = ptr<uint8_t>(m.a[0]);
    uint8_t *to = buffer;
    poke_w(to, 0);
    to += 2;
    each_string([&](uint8_t *slot) {
        uint8_t *s = ptr<uint8_t>(peek_l(slot));
        if (address(s) < low || address(s) >= high)
            return;
        uint16_t length = peek_w(s);
        if (!length) {
            poke_l(slot, d.ChVide);
            return;
        }
        if ((length & 0xFFC0) == 0xFFC0) {
            poke_l(slot, ((peek_l(s) & 0x3FFFFF) << 1) + low);
            return;
        }
        uint32_t offset = to - buffer;
        poke_l(slot, low + offset);
        poke_w(to, length);
        to += 2;
        for (uint32_t i = 0; i < (length + 1u) / 2; i++, to += 2)
            poke_w(to, peek_w(s + 2 + 2 * i));
        poke_l(s, (offset >> 1) | Moved);
    });
    uint8_t *back = ptr<uint8_t>(low);
    const uint8_t *from = buffer;
    do {
        poke_w(back, peek_w(from));
        back += 2;
        from += 2;
    } while (from < to);
    d.HiChaine = address(back);
    Regs f;
    f.a[1] = m.a[0];
    f.d[0] = size;
    w::release(f.a[1], f.d[0]);
    return true;
}

// The slow collection, without memory: a table of the lowest strings
// (above those packed already), packed in address order, again until the
// table is not full
void slow()
{
    IData &d = data();
    Regs m;
    m.d[0] = TMenage + 16;
    m.a[0] = w::reserve(m.d[0], w::Fast);
    uint32_t table;
    if (m.a[0]) {
        d.BMenage = table = m.a[0];
    } else {
        d.BMenage = 0xFFFFFFFF;
        table = d.Buffer;
    }
    uint32_t table_end = table + TMenage;
    uint32_t minimum = d.LoChaine + 2;
    uint32_t high = d.HiChaine;
    uint32_t to = minimum;
    for (;;) {
        // the table: (string, slot) pairs by string, a sentinel after them
        uint32_t limit = 0xFFFFFFFF;
        uint32_t end = table;
        poke_l(ptr<void>(table), 0x7FFFFFFF);
        each_string([&](uint8_t *slot) {
            uint32_t s = peek_l(slot);
            if (s < minimum || s >= high || s >= limit)
                return;
            uint32_t at = table;
            do
                at += 8;
            while (s >= peek_l(ptr<void>(at - 8)));
            if (at == table_end) {
                limit = s;
                end = table_end;
            } else {
                uint32_t a1 = end;
                if (end >= table_end) {
                    a1 -= 8;
                    limit = peek_l(ptr<void>(a1 - 8));
                } else {
                    end += 8;
                    poke_l(ptr<void>(a1 + 8), 0x7FFFFFFF);
                }
                do {
                    a1 -= 4;
                    poke_l(ptr<void>(a1 + 8), peek_l(ptr<void>(a1)));
                    a1 -= 4;
                    poke_l(ptr<void>(a1 + 8), peek_l(ptr<void>(a1)));
                } while (a1 >= at);
            }
            poke_l(ptr<void>(at - 4), address(slot));
            poke_l(ptr<void>(at - 8), s);
        });
        // packed
        uint32_t previous = 0, moved_to = 0;
        to = minimum;
        for (uint32_t e = table; e < end; e += 8) {
            uint32_t s = peek_l(ptr<void>(e));
            uint8_t *slot = ptr<uint8_t>(peek_l(ptr<void>(e + 4)));
            if (s == previous) {
                poke_l(slot, moved_to);
                continue;
            }
            previous = s;
            if (s == to) {
                moved_to = to;
                to += 2 + peek_w(ptr<void>(to));
                to += to & 1;
                continue;
            }
            poke_l(slot, to);
            moved_to = to;
            uint16_t length = peek_w(ptr<void>(s));
            if (!length) {
                moved_to = d.ChVide;
                poke_l(slot, moved_to);
                continue;
            }
            poke_w(ptr<void>(to), length);
            to += 2;
            for (uint32_t i = 0; i < (length + 1u) / 2; i++, to += 2)
                poke_w(ptr<void>(to), peek_w(ptr<uint8_t>(s) + 2 + 2 * i));
        }
        if (end < table_end)
            break;
        // the table was full: the other variables that pointed to the last
        // string moved, then the next strings
        each_string([&](uint8_t *slot) {
            if (peek_l(slot) == previous)
                poke_l(slot, moved_to);
        });
        minimum = to;
    }
    d.HiChaine = to;
    if (int32_t(d.BMenage) >= 0) {
        Regs f;
        f.a[1] = d.BMenage;
        f.d[0] = TMenage + 16;
        w::release(f.a[1], f.d[0]);
    }
    d.BMenage = 0;
}

bool fits(uint32_t length)
{
    IData &d = data();
    return d.HiChaine + length + 4 < d.TabBas;
}

} // namespace

namespace amos::interp {

// Menage
void collect()
{
    if (!fast())
        slow();
}

// Demande: room for length bytes at HiChaine (the string's start)
uint32_t demande(uint32_t length)
{
    IData &d = data();
    Ctx &c = ctx();
    for (;;) {
        if (fits(length))
            return d.HiChaine;
        if (d.ErrorRegs) {
            c.d6 = d.ErrorSave[0];
            c.d7 = d.ErrorSave[1];
        }
        collect();
        if (!fits(length))
            break;
        if (d.Patch_Menage.load()) {
            Regs r;
            r.d[3] = length;
            interp_jump(&r, d.Patch_Menage.load());
        }
        int32_t at = int32_t(c.d7);
        if (at < 0)
            continue;           // DDemande: room now
        if (!at)
            break;              // twice in the same instruction
        // the instruction again, from its start
        c.a3 = ptr<uint8_t>(d.PLoop);
        c.a6 = ptr<uint8_t>(c.d7);
        d.MenA4 = c.d7;
        c.d7 = 0;
        d.ErrorSave[0] = c.d6;
        d.ErrorSave[1] = 0;
        uint16_t token = peek_w(c.a6 - 2);
        int16_t offset = int16_t(peek_w(c.a4 + token));
        AllRegs a = {};
        a.d[0] = token;
        a.d[1] = uint16_t(offset);
        a.d[6] = c.d6;
        a.d[7] = 0;
        a.a[0] = peek_l(c.a4 + offset - LB_Size);
        a.a[3] = address(c.a3);
        a.a[4] = address(c.a4);
        a.a[5] = encode_data_zone(c.a5);
        a.a[6] = address(c.a6);
        interp_restart(d.BasSp.load(), d.Prg_InsRet.load(), &a);
    }
    if (int32_t(c.d7) <= 0)
        c.d7 = d.MenA4;
    error(EOofBuf);
}

// DDemande: the same without the restart
uint32_t demande_here(uint32_t length)
{
    IData &d = data();
    Ctx &c = ctx();
    d.MenA4 = c.d7;
    c.d7 = 0xFFFFFFFF;
    d.ErrorSave[0] = c.d6;
    d.ErrorSave[1] = c.d7;
    uint32_t s = demande(length);
    c.d7 = d.MenA4;
    d.ErrorSave[0] = c.d6;
    d.ErrorSave[1] = c.d7;
    return s;
}

} // namespace amos::interp

using namespace amos::interp;

extern "C" {
// Demande / DDemande: d3 the length. Out: a0 = a1 = HiChaine.
uint32_t interp_demande(RegFrame &r)
{
    r.a[0] = r.a[1] = demande(r.d[3]);
    return 0;
}
uint32_t interp_ddemande(RegFrame &r)
{
    r.a[0] = r.a[1] = demande_here(r.d[3]);
    return 0;
}
uint32_t interp_menage(RegFrame &)
{
    collect();
    return 0;
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_heap.inc"
#endif
