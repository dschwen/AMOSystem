// Sort a(0) and =Match(a(0),value) (+ILib.s): an array of numbers or
// strings sorted in place, and searched (sorted) for a value.

#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

namespace {

struct Array {
    ContextCursor elements;
    uint8_t type;
    uint8_t shift;      // an element's size
    uint32_t count;
};

// GTablo: the array after the token at pc (its indices evaluated, and not
// used)
Array array()
{
    IData &d = idata();
    pc() += 2;
    int16_t offset = int16_t(next_token());
    ContextCursor at = offset < 0 ? decode_context_cursor(d.VarGlo) + (1 - offset) : decode_context_cursor(d.VarLoc) + (2 + offset);
    int8_t length = int8_t(next_byte());
    uint8_t flags = next_byte();
    pc() += length;
    ContextCursor block = decode_context_cursor(peek_l(at));
    if (!block)
        error(ENonDim);
    uint8_t dimensions = byte_at(block);
    Array a = {ContextCursor(), uint8_t(flags & 0x0F), byte_at(block, 1), 1};
    ContextCursor p = block + 2;
    for (uint16_t n = dimensions; n; n--, p += 4)
        a.count = (a.count & 0xFFFF) * ((peek_w(p) + 1u) & 0xFFFF);
    a.elements = p;
    ContextCursor a3 = ctx().a3;
    for (uint16_t n = dimensions; n; n--) {
        Value i = evaluate_after();
        if (kind(i) == TFloat)
            to_integer(i);
    }
    ctx().a3 = a3;
    return a;
}

ContextCursor element(const Array &a, uint32_t i) { return a.elements + (i << a.shift); }
Value value_at(const Array &a, uint32_t i)
{
    ContextCursor e = element(a, i);
    return {a.type, peek_l(e), peek_l(e + 4)};
}

// CpBis: an element against a value (<0, 0, >0), by the value's type
int compare(const Value &element, const Value &v)
{
    uint8_t t = kind(v);
    if (t == TFloat)
        return float_compare(element, v);
    if (t < TFloat)
        return int32_t(element.v1) < int32_t(v.v1) ? -1 : element.v1 != v.v1;
    return compare_strings(GuestString::from_word(element.v1), GuestString::from_word(v.v1));
}

// Sort: a gap sort, from half the size down
void sort()
{
    Array a = array();
    for (uint32_t gap = a.count >> 1; gap; gap >>= 1) {
        for (uint32_t na = 1; na <= a.count - gap; na++) {
            uint32_t nr = na;
            for (;;) {
                ContextCursor x = element(a, nr - 1), y = element(a, nr - 1 + gap);
                Value vy = value_at(a, nr - 1 + gap);
                vy.type = a.type;
                if (compare(value_at(a, nr - 1), vy) <= 0)
                    break;
                uint32_t t = peek_l(x);
                poke_l(x, peek_l(y));
                poke_l(y, t);
                if (a.shift == 3) {
                    t = peek_l(x + 4);
                    poke_l(x + 4, peek_l(y + 4));
                    poke_l(y + 4, t);
                }
                if (nr <= gap)
                    break;
                nr -= gap;
            }
        }
    }
}

// =Match: the index of the value, or -(the index of the first greater one
// + 1)
int32_t match()
{
    pc() += 2;
    Array a = array();
    Value v = evaluate_after();
    if (kind(v) != a.type) {
        if (a.type == TFloat)
            v = to_float(v);
        else if (a.type > TFloat)
            error(34);
        else
            v = to_integer(v);
    }
    uint32_t base = 0, step = a.count >> 1;
    for (;;) {
        int c = compare(value_at(a, base + step), v);
        if (!c)
            return int32_t(base + step);
        if (c < 0)
            base += step;
        if (!step)
            break;
        step >>= 1;
    }
    for (; base < a.count; base++) {
        int c = compare(value_at(a, base), v);
        if (!c)
            return int32_t(base);
        if (c > 0)
            break;
    }
    return -int32_t(base + 1);
}

} // namespace

extern "C" {
void interp_in_sort() { sort(); }
void interp_fn_match(EvalOut *o) { *o = {0, 0, TInt, uint32_t(match()), 0, 0}; }
}

#if !defined(__mc68000__)
#include "runtime/bindings/interp_sort.inc"
#endif
