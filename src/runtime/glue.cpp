// The assembler's entries on another host (host/glue.h): what each glue
// macro does, in C++.

#include "runtime/glue.h"
#include "runtime/runtime.h"
#include "interp/interp.h"

#include <stdlib.h>

using namespace amos;

namespace amos::host {

namespace {

const GlueEntry **sorted;       // the entries by address
int sorted_count;

int by_address(const void *a, const void *b)
{
    auto x = (*static_cast<const GlueEntry *const *>(a))->at, y = (*static_cast<const GlueEntry *const *>(b))->at;
    return x < y ? -1 : x > y;
}

// CCall: the interpreter registers into the context for the C++, back
// from it afterwards
void to_context(const AllRegs &r)
{
    Ctx &c = ctx();
    c.d6 = r.d[6];
    c.d7 = r.d[7];
    c.a3 = ptr<uint8_t>(r.a[3]);
    c.a4 = ptr<uint8_t>(r.a[4]);
    c.a5 = ptr<uint8_t>(r.a[5]);
    c.a6 = ptr<uint8_t>(r.a[6]);
}
void from_context(AllRegs &r)
{
    const Ctx &c = ctx();
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(c.a6);
}

// A C function of n 32-bit parameters (passed as 64 bits: the pointers
// among them are read whole); its result as 64 bits
uint64_t call_c(const void *fn, int n, const uint64_t *p)
{
    using U = uint64_t;
    switch (n) {
    case 0: return reinterpret_cast<U (*)()>(fn)();
    case 1: return reinterpret_cast<U (*)(U)>(fn)(p[0]);
    case 2: return reinterpret_cast<U (*)(U, U)>(fn)(p[0], p[1]);
    case 3: return reinterpret_cast<U (*)(U, U, U)>(fn)(p[0], p[1], p[2]);
    case 4: return reinterpret_cast<U (*)(U, U, U, U)>(fn)(p[0], p[1], p[2], p[3]);
    case 5: return reinterpret_cast<U (*)(U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4]);
    case 6: return reinterpret_cast<U (*)(U, U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4], p[5]);
    case 7: return reinterpret_cast<U (*)(U, U, U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6]);
    case 8:
        return reinterpret_cast<U (*)(U, U, U, U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6], p[7]);
    case 9:
        return reinterpret_cast<U (*)(U, U, U, U, U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                                                       p[7], p[8]);
    default:
        return reinterpret_cast<U (*)(U, U, U, U, U, U, U, U, U, U)>(fn)(p[0], p[1], p[2], p[3], p[4], p[5], p[6],
                                                                          p[7], p[8], p[9]);
    }
}

// The condition codes of tst.size
int32_t tested(uint32_t v, int size)
{
    return size == 1 ? int8_t(v) : size == 2 ? int16_t(v) : int32_t(v);
}

// WEntry, WiEntry (_w_go): the caller's interpreter registers the
// context during the call, the context as it was afterwards
template <typename F> void w_go(AllRegs &r, F f)
{
    Ctx saved = ctx();
    to_context(r);
    f();
    ctx() = saved;
}

} // namespace

void glue_start()
{
    glue_relocate();
    sorted = static_cast<const GlueEntry **>(malloc(sizeof(GlueEntry *) * glue_entry_count));
    for (int i = 0; i < glue_entry_count; i++)
        sorted[i] = &glue_entries[i];
    sorted_count = glue_entry_count;
    qsort(sorted, sorted_count, sizeof *sorted, by_address);
}

const GlueEntry *glue_entry(const void *at)
{
    int lo = 0, hi = sorted_count - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        const uint8_t *x = sorted[mid]->at;
        if (x == at)
            return sorted[mid];
        if (x < static_cast<const uint8_t *>(at))
            lo = mid + 1;
        else
            hi = mid - 1;
    }
    return nullptr;
}

void poll();
[[noreturn]] void mark_jump(uint32_t mark, const void *target, const AllRegs &r, const void *then = nullptr);

int32_t glue_call(const void *at, AllRegs &r)
{
    poll();
    const GlueEntry *e = glue_entry(at);
    if (!e)
        missing("address", at);
    switch (e->kind) {
    case GlueKind::libcall: {
        uint64_t p[10];
        int n = e->a1;
        if (n) {
            p[n - 1] = r.d[3];
            for (int i = n - 2; i >= 0; i--) {
                p[i] = peek_l(ptr<void>(r.a[3]));
                r.a[3] += 4;
            }
        }
        to_context(r);
        uint64_t v = call_c(e->fn, n, p);
        from_context(r);
        switch (e->a2) {
        case 0:     // Int
            r.d[0] = r.d[3] = uint32_t(v);
            r.d[2] = 0;
            break;
        case 1:     // Float
            r.d[0] = r.d[3] = uint32_t(v);
            r.d[2] = 1;
            break;
        case 2:     // String
            r.d[0] = r.d[3] = uint32_t(v);
            r.d[2] = 2;
            break;
        case 4:     // Double: d0 (the high half), d1
            r.d[0] = r.d[3] = uint32_t(v >> 32);
            r.d[1] = r.d[4] = uint32_t(v);
            r.d[2] = 1;
            break;
        }
        return int32_t(r.d[0]);
    }
    case GlueKind::nop:
        return int32_t(r.d[0]);
    case GlueKind::reg: {
        RegFrame f;
        for (int i = 0; i < 8; i++)
            f.d[i] = r.d[i];
        for (int i = 0; i < 3; i++)
            f.a[i] = r.a[i];
        to_context(r);
        uint32_t v = reinterpret_cast<uint32_t (*)(RegFrame &)>(e->fn)(f);
        from_context(r);
        for (int i = 0; i < 8; i++)
            r.d[i] = f.d[i];
        for (int i = 0; i < 3; i++)
            r.a[i] = f.a[i];
        return int32_t(v);
    }
    case GlueKind::call:
        to_context(r);
        reinterpret_cast<void (*)()>(e->fn)();
        from_context(r);
        return int32_t(r.d[0]);
    case GlueKind::test:
    case GlueKind::record: {
        uint32_t d6 = r.d[6], d7 = r.d[7];
        to_context(r);
        reinterpret_cast<void (*)()>(e->fn)();
        from_context(r);
        r.d[6] = d6;
        r.d[7] = d7;
        return int32_t(r.d[0]);
    }
    case GlueKind::eval:
    case GlueKind::eval_d3: {
        interp::EvalOut o = {};
        if (e->kind == GlueKind::eval_d3)
            o.d3 = r.d[3];
        to_context(r);
        reinterpret_cast<void (*)(interp::EvalOut *)>(e->fn)(&o);
        from_context(r);
        r.d[0] = o.d0;
        r.d[1] = o.d1;
        r.d[2] = o.d2;
        r.d[3] = o.d3;
        r.d[4] = o.d4;
        r.a[0] = o.a0;
        return int32_t(r.d[0]);
    }
    case GlueKind::float_: {
        uint32_t o[3] = {0, r.d[3], r.d[4]};
        to_context(r);
        reinterpret_cast<void (*)(uint32_t *)>(e->fn)(o);
        from_context(r);
        r.d[2] = o[0];
        r.d[3] = o[1];
        r.d[4] = o[2];
        return int32_t(r.d[0]);
    }
    case GlueKind::w:
        w_go(r, [&] { reinterpret_cast<void (*)(AllRegs *)>(e->fn)(&r); });
        return tested(r.d[0], e->a2);
    case GlueKind::w_a0:
        w_go(r, [&] { reinterpret_cast<void (*)(AllRegs *)>(e->fn)(&r); });
        return r.a[0] ? 1 : 0;
    case GlueKind::w_f: {
        uint32_t v = 0;
        w_go(r, [&] { v = reinterpret_cast<uint32_t (*)(AllRegs *)>(e->fn)(&r); });
        return int32_t(v);
    }
    case GlueKind::window:
        w_go(r, [&] { w_window(uint32_t(e->a1), &r); });
        return tested(r.d[0], e->a2);
    case GlueKind::alias:
        return glue_call(e->target, r);
    case GlueKind::stack:
        mark_jump(idata().BasSp, e->target, r);
    case GlueKind::lib_jump:
        return glue_call(lib_address(e->a1), r);
    case GlueKind::hand:
        reinterpret_cast<void (*)(AllRegs &)>(e->fn)(r);
        return int32_t(r.d[0]);
    case GlueKind::c_routine:
    case GlueKind::missing:
        break;
    }
    missing(e->name, at);
}

} // namespace amos::host
