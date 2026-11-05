#include "memory/guest_allocator.h"
#include "memory/guest_memory.h"
#include "abi/guest_address_codec.h"
#include <assert.h>
using namespace amos::guest;
using abi::AddressCodec;
int main() {
    Memory memory;
    assert(memory.reserve() == Fault::none);
    const auto base = AddressCodec::decode<Byte>(0x1000000);
    assert(memory.map_ram(base.address(), 4096) == Fault::none);
    RangeAllocator pool;
    assert(pool.initialize(GuestSpan<Byte>(base, 128)) == Fault::none);
    assert(pool.initialize(GuestSpan<Byte>(base, 128)) == Fault::invalid_range);
    assert(pool.contains(base) && pool.contains(base + 127) && !pool.contains(base + 128));
    auto whole = pool.allocate(128); assert(whole && whole.value.data() == base);
    assert(pool.free_bytes() == 0 && pool.allocate(1).fault == Fault::exhausted);
    assert(memory.store_be32(base + 16, 0xa1b2c3d4) == Fault::none);
    assert(pool.release(base + 8, 32) == Fault::none);
    auto middle = pool.allocate_at(8, base + 16); assert(middle && middle.value.data() == base + 16);
    assert(memory.load_be32(middle.value.data()).value == 0xa1b2c3d4); // AllocAbs retains bytes.
    assert(pool.allocate(8).value.data() == base + 8);
    assert(pool.allocate(16).value.data() == base + 24);
    assert(pool.free_bytes() == 0);
    assert(pool.release(base, 128) == Fault::none);
    assert(pool.release(base, 8) == Fault::invalid_range);
    assert(pool.free_bytes() == 128);
    assert(pool.release(base - 1, 8) == Fault::invalid_range);
    assert(pool.release(base + 127, 8) == Fault::overflow);
    assert(pool.allocate(0).fault == Fault::invalid_range);
    assert(pool.allocate(UINT32_MAX).fault == Fault::exhausted);
    assert(pool.allocate_at(UINT32_MAX, base).fault == Fault::overflow);

    RangeAllocator top;
    const auto last = AddressCodec::decode<Byte>(0xfffffff0);
    assert(top.initialize(GuestSpan<Byte>(last, 16)) == Fault::none);
    assert(top.allocate(1).value.data() == last);
    assert(top.allocate(1).value.data() == last + 8);
    assert(top.free_bytes() == 0);
    assert(top.release(last, 16) == Fault::none);
    assert(top.allocate_at(8, last + 8).value.data() == last + 8);

    // Independent cell model checks first-fit, exact allocation, splitting and
    // merging over deterministic randomized operation sequences.
    RangeAllocator random;
    assert(random.initialize(GuestSpan<Byte>(base, 1024)) == Fault::none);
    bool used[128]{};
    uint32_t state = 7;
    for (unsigned step = 0; step < 30000; ++step) {
        state = state * 1664525u + 1013904223u;
        const unsigned at = (state >> 12) % 128;
        const unsigned length = (state >> 20) % 8 + 1;
        if (state & 1) {
            if (used[at]) {
                assert(random.release(base + at * 8, 8) == Fault::none);
                used[at] = false;
            }
        } else {
            unsigned expected = 128;
            const bool exact = state & 2;
            for (unsigned candidate = exact ? at : 0; candidate + length <= 128; ++candidate) {
                bool free = true;
                for (unsigned n = 0; n < length; ++n) free &= !used[candidate + n];
                if (free) { expected = candidate; break; }
                if (exact) break;
            }
            auto result = exact ? random.allocate_at(length * 8, base + at * 8) : random.allocate(length * 8);
            assert(bool(result) == (expected != 128));
            if (result) {
                assert(result.value.data() == base + expected * 8);
                for (unsigned n = 0; n < length; ++n) used[expected + n] = true;
            }
        }
        unsigned free = 0;
        for (bool cell : used) if (!cell) free += 8;
        assert(random.free_bytes() == free);
    }
}
