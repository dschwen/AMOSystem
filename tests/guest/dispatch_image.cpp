// Real generated entry metadata, independently relocated above native 4 GiB.
// Callback bodies are test stubs: this qualifies lookup, not interpreter code.
#include "runtime/static_symbols.h"
#include "runtime/guest_dispatch.h"
#include "tables/tables.h"
#include "memory/guest_memory.h"
#include <assert.h>
#include <vector>
using namespace amos;
using namespace amos::guest;
using namespace amos::host;
using Index = GuestDispatchIndex<RoutineSignature, GlueEntry>;
struct Image {
    Memory memory;
    SegmentBindings bindings;
    std::vector<Index::Slot> slots;
    Index index;
    void load(uint32_t fast, uint32_t chip) {
        assert(memory.reserve() == Fault::none);
        assert(load_static_image(memory, bindings, abi::AddressCodec::decode<Byte>(fast), abi::AddressCodec::decode<Byte>(chip)) == Fault::none);
        for (auto table = glue_entry_tables; *table; ++table)
            for (auto entry = *table; entry->at.segment().index(); ++entry) {
                assert(reinterpret_cast<uintptr_t>(entry) > UINT32_MAX);
                auto address = bindings.resolve(entry->at);
                assert(address);
                slots.push_back({address.value, entry});
            }
        assert(index.initialize(slots.data(), slots.size()) == Fault::none);
    }
};
int main() {
    Image first, second;
    first.load(0x4000000, 0x100000);
    second.load(0x80000000, 0x200000);
    assert(first.slots.size() > 1900 && first.slots.size() == second.slots.size());
    size_t aliases = 0;
    for (const auto& slot : first.slots) {
        const auto* entry = slot.payload;
        assert(first.index.find(slot.at) == entry);
        auto relocated = second.bindings.resolve(entry->at);
        assert(relocated && relocated.value != slot.at);
        assert(second.index.find(relocated.value) == entry);
        assert(!second.index.find(slot.at));
        if (entry->kind == GlueKind::alias || entry->kind == GlueKind::stack) {
            auto target = first.bindings.resolve(entry->target);
            auto other_target = second.bindings.resolve(entry->target);
            assert(target && other_target && target.value != other_target.value);
            assert(first.index.find(target.value));
            assert(first.index.find(target.value) == second.index.find(other_target.value));
            ++aliases;
        }
    }
    assert(aliases > 700);
}
