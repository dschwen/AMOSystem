#include "runtime/marks.h"
#include "runtime/context_handle.h"
#include "runtime/detail/legacy_context.h"
#include "core/guest_types.h"
#include <assert.h>
#include <stdint.h>
using namespace amos;
using namespace amos::host;
namespace { uint32_t task; unsigned ran; MarkHandle abandoned; ContextHandle abandoned_context; }
namespace amos::host {
uint32_t recovery_task_id() { return task; }
void run_at(Mark& mark) { ++ran; mark.regs.d[0] = 91; }
}

[[gnu::noinline,noreturn]] static void return_to(MarkHandle token, uint32_t value) {
    auto& destination = mark_unwind(token);
    destination.regs.d[0] = value;
    longjmp(destination.jb, MarkReturn);
}
[[gnu::noinline]] static void child(MarkHandle outer) {
    Mark inner{};
    abandoned = decode_mark(mark_word(inner));
    mark_run(inner, [&] {
        GuestContext snapshot{}; snapshot.d6 = 93;
        abandoned_context = save_context(snapshot);
        assert(context_valid(abandoned_context));
        return_to(outer, 73);
    });
    assert(false);
}
int main(int argc, char**) {
    Ctx legacy{};
    legacy.d6 = 31; legacy.d7 = 37;
    legacy.a5 = decode_data_zone(0xe0000010);
    legacy.a4 = decode_context_cursor(0xd0000010);
    legacy.a3 = decode_context_cursor(0xf0000010);
    legacy.a6 = decode_context_cursor(0xc0000010);
    auto captured = detail::capture_context(legacy);
    assert(captured.d6 == 31 && captured.d7 == 37);
    assert(context_address(captured.a3) == 0xf0000010);
    assert(context_address(captured.a5) == 0xe0000010);
    assert(context_address(captured.a4) == 0xd0000010);
    assert(context_address(captured.a6) == 0xc0000010);
    GuestContext source{}; source.d6 = 11;
    source.a3 = guest::abi::AddressCodec::decode<guest::Byte>(0xf0000010);
    const auto retained = save_context(source);
    source.d6 = 22;
    source.a3 = {};
    assert(context_address(saved_context(retained).a3) == 0xf0000010);
    assert(saved_context(retained).d6 == 11); // Registry owns a value copy.
    Mark outer{};
    assert(reinterpret_cast<uintptr_t>(&outer) > UINT32_MAX);
    const auto outer_token = decode_mark(mark_word(outer));
    assert(!mark_lookup(outer_token)); // Reserved but not yet captured.
    if (argc > 1) mark_unwind(outer_token); // Must fail closed before capture.
    auto result = mark_run(outer, [&] {
        assert(mark_lookup(outer_token) == &outer);
        task = 1;
        assert(!mark_lookup(outer_token));
        assert(!context_valid(retained));
        Mark other{};
        auto other_token = decode_mark(mark_word(other));
        mark_run(other, [&] {
            assert(mark_lookup(other_token) == &other);
            assert(!mark_lookup(outer_token));
        });
        assert(!mark_lookup(other_token));
        task = 0;
        child(outer_token);
    });
    assert(result.d[0] == 73);
    assert(!mark_lookup(outer_token));
    assert(context_valid(retained));
    assert(!context_valid(abandoned_context));
    release_context(retained);
    assert(!context_valid(retained));
    assert(!mark_lookup(abandoned)); // Nested capture discarded before longjmp.
    assert(outer.recovery_token == 0);
    assert(!mark_lookup(MarkHandle{}));
    assert(!mark_lookup(decode_mark(UINT32_MAX)));
    for (unsigned i = 0; i < 4096; ++i) {
        Mark again{};
        const auto fresh = decode_mark(mark_word(again));
        assert(fresh != outer_token && fresh != abandoned);
        ContextHandle transient{};
        auto out = mark_run(again, [&] {
            GuestContext value{}; value.d7 = i;
            transient = save_context(value);
            assert(transient != retained);
            again.regs.d[0] = saved_context(transient).d7;
        });
        assert(!context_valid(transient)); // Normal return also clears scope-owned temporaries.
        assert(out.d[0] == i && !mark_lookup(fresh));
    }
    Mark repeat{};
    auto token = decode_mark(mark_word(repeat));
    auto out = mark_run(repeat, [&] {
        auto& destination = mark_unwind(token);
        longjmp(destination.jb, MarkRun);
    });
    assert(ran == 1 && out.d[0] == 91 && !mark_lookup(token));
}
