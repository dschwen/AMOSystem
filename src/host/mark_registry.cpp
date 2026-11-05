// Native recovery state is never serialized as a guest RAM address.
#include "runtime/marks.h"
#include "runtime/context_handle.h"
#include <stdio.h>
#include <stdlib.h>
#pragma pack(push, 8)
namespace amos::host {
uint32_t recovery_task_id();
struct MarkRegistry {
    static constexpr unsigned index_bits = 10;
    static constexpr unsigned count = 1u << index_bits;
    static constexpr unsigned tasks = 9; // main + the eight HAL coroutines
    static constexpr uint32_t max_generation = UINT32_MAX >> index_bits;
    struct Slot {
        Mark* mark = nullptr;
        uint32_t generation = 1;
        uint32_t owner = 0;
        uint32_t previous = 0;
        bool active = false;
    };
    Slot slots[count]{};
    uint32_t top[tasks]{};
    unsigned next = 1; // zero is never an index or a valid token
    [[noreturn]] static void invalid() {
        fprintf(stderr, "amos: invalid, expired or wrong-task recovery token\n");
        abort();
    }
    uint32_t owner() const {
        auto task = recovery_task_id();
        if (task >= tasks) invalid();
        return task;
    }
    Slot* find(uint32_t token) {
        const auto index = token & (count - 1);
        if (!index || !token) return nullptr;
        auto& s = slots[index];
        return s.mark && s.generation == token >> index_bits ? &s : nullptr;
    }
    uint32_t acquire(Mark& mark) {
        if (mark.recovery_token) {
            auto* s = find(mark.recovery_token);
            if (!s || s->mark != &mark || s->owner != owner()) invalid();
            return mark.recovery_token;
        }
        for (unsigned n = 0; n < count - 1; ++n) {
            const auto index = next;
            next = next == count - 1 ? 1 : next + 1;
            auto& s = slots[index];
            if (s.mark || s.generation > max_generation) continue;
            s.mark = &mark; s.owner = owner(); s.previous = 0; s.active = false;
            mark.recovery_token = (s.generation << index_bits) | index;
            return mark.recovery_token;
        }
        invalid(); // finite registry; exhausted generations retire instead of wrapping
    }
    void release(uint32_t token) {
        auto* s = find(token);
        if (!s) invalid();
        s->mark->recovery_token = 0;
        s->mark = nullptr; s->active = false; s->previous = 0;
        ++s->generation;
    }
    void enter(Mark& mark) {
        const auto token = acquire(mark);
        auto& s = *find(token);
        if (s.active) invalid();
        mark.context_watermark = context_checkpoint();
        s.previous = top[s.owner]; s.active = true; top[s.owner] = token;
    }
    Mark* lookup(MarkHandle handle) {
        auto* s = find(handle.word_);
        return s && s->active && s->owner == owner() ? s->mark : nullptr;
    }
    void leave(Mark& mark) {
        const auto token = mark.recovery_token;
        auto* s = find(token);
        if (!s || s->mark != &mark || !s->active || s->owner != owner() || top[s->owner] != token)
            invalid();
        context_unwind(mark.context_watermark);
        top[s->owner] = s->previous;
        release(token);
    }
    Mark& unwind(MarkHandle handle) {
        Mark* destination = lookup(handle);
        if (!destination) invalid();
        const auto task = owner();
        // First verify ancestry without changing any state.
        auto cursor = top[task];
        while (cursor && cursor != handle.word_) {
            auto* s = find(cursor);
            if (!s || !s->active || s->owner != task) invalid();
            cursor = s->previous;
        }
        if (!cursor) invalid();
        while (top[task] != handle.word_) {
            const auto token = top[task];
            top[task] = find(token)->previous;
            release(token);
        }
        context_unwind(destination->context_watermark);
        return *destination;
    }
};
namespace { MarkRegistry registry; }
uint32_t mark_word(Mark& mark) { return registry.acquire(mark); }
void mark_enter(Mark& mark) { registry.enter(mark); }
void mark_leave(Mark& mark) { registry.leave(mark); }
Mark* mark_lookup(MarkHandle handle) { return registry.lookup(handle); }
Mark& mark_unwind(MarkHandle handle) { return registry.unwind(handle); }
}
#pragma pack(pop)
