#include "runtime/context_handle.h"
#include <stdio.h>
#include <stdlib.h>
#pragma pack(push,8)
namespace amos::host {
uint32_t recovery_task_id();
struct ContextRegistry {
    static constexpr unsigned count = 256, tasks = 9;
    static constexpr uint32_t last_generation = UINT32_MAX >> 8;
    struct Slot {
        Ctx value{};
        uint64_t sequence = 0;
        uint32_t generation = 1, owner = 0;
        bool occupied = false;
    } slots[count]{};
    uint64_t sequences[tasks]{};
    unsigned next = 1;
    [[noreturn]] static void invalid() {
        fprintf(stderr, "amos: invalid, expired or wrong-task register snapshot\n");
        abort();
    }
    unsigned owner() const {
        const auto id = recovery_task_id();
        if (id >= tasks) invalid();
        return id;
    }
    Slot* find(ContextHandle handle) {
        auto index = handle.word_ & 255;
        if (!index) return nullptr;
        auto& slot = slots[index];
        return slot.occupied && slot.generation == handle.word_ >> 8 && slot.owner == owner() ? &slot : nullptr;
    }
    ContextHandle save(const Ctx& value) {
        const auto task = owner();
        if (sequences[task] == UINT64_MAX) invalid();
        for (unsigned n = 0; n < count - 1; ++n) {
            auto index = next; next = next == count - 1 ? 1 : next + 1;
            auto& slot = slots[index];
            if (slot.occupied || slot.generation > last_generation) continue;
            slot.value = value; slot.owner = task; slot.sequence = ++sequences[task]; slot.occupied = true;
            return ContextHandle((slot.generation << 8) | index);
        }
        invalid();
    }
    static void release(Slot& slot) {
        slot.occupied = false; ++slot.generation;
    }
    void unwind(uint64_t checkpoint) {
        const auto task = owner();
        if (checkpoint > sequences[task]) invalid();
        for (auto& slot : slots)
            if (slot.occupied && slot.owner == task && slot.sequence > checkpoint) release(slot);
    }
};
namespace { ContextRegistry registry; }
ContextHandle save_context(const Ctx& value) { return registry.save(value); }
const Ctx& saved_context(ContextHandle handle) {
    auto* slot = registry.find(handle);
    if (!slot) ContextRegistry::invalid();
    return slot->value;
}
void release_context(ContextHandle handle) {
    auto* slot = registry.find(handle);
    if (!slot) ContextRegistry::invalid();
    ContextRegistry::release(*slot);
}
bool context_valid(ContextHandle handle) { return registry.find(handle) != nullptr; }
uint64_t context_checkpoint() { return registry.sequences[registry.owner()]; }
void context_unwind(uint64_t checkpoint) { registry.unwind(checkpoint); }
}
#pragma pack(pop)
