#include "memory/guest_code.h"
#include "memory/guest_view.h"
#include "core/routine_field.h"
#include <assert.h>
using namespace amos::guest;
using Signature = void();
struct Frame;
template<> struct amos::guest::Layout<Frame> { static constexpr uint32_t size = 8; };
using Continuation = Field<Frame, amos::wire::RoutineField, 4>;
int main() {
    Memory memory;
    assert(memory.reserve() == Fault::none);
    auto base = abi::AddressCodec::decode<Byte>(0x80000000);
    assert(memory.map_ram(base.address(),32) == Fault::none);
    assert(memory.store_be16(base+4,3) == Fault::none);
    assert(memory.store_be16(base+6,2) == Fault::none);
    assert(memory.store_be32(base+8,0x4c43616c) == Fault::none);
    assert(memory.store_be16(base+12,0x5041) == Fault::none);
    assert(memory.store_be16(base+14,3) == Fault::none);
    CodeReader<Signature> code(memory,abi::EntryCodec::decode<Signature>(0x80000010));
    assert(code.word_at(-12).value == 3);
    assert(code.word_at(-10).value == 2);
    assert(code.long_at(-8).value == 0x4c43616c);
    assert(code.word_at(-4).value == 0x5041);
    assert(code.word_at(-2).value == 3);
    assert(code.word_at(15).fault == Fault::unmapped);
    assert(code.long_at(13).fault == Fault::unmapped);
    assert(code.word_at(INT32_MIN).fault == Fault::unmapped);
    // Architectural code cells retain exactly four big-endian bytes.
    amos::wire::RoutineField field{};
    const auto target = amos::host::decode_routine(0x80000010);
    field.store(target);
    const auto* bytes = reinterpret_cast<const Byte*>(&field);
    assert(bytes[0] == 0x80 && bytes[1] == 0 && bytes[2] == 0 && bytes[3] == 0x10);
    assert(memory.copy_from_host(GuestSpan<Byte>(base+24,4),HostSpan<const Byte>(bytes,4)) == Fault::none);
    amos::wire::RoutineField restored{};
    assert(memory.copy_to_host(HostSpan<Byte>(reinterpret_cast<Byte*>(&restored),4),GuestSpan<const Byte>(base+24,4)) == Fault::none);
    assert(restored.load() == target);
    auto frame = memory.ref(abi::AddressCodec::decode<Frame>(0x80000018));
    assert(frame);
    auto continuation = frame.value.field<Continuation>();
    assert(continuation);
    assert(continuation.value.store(target) == Fault::none);
    assert(memory.load_be32(base+28).value == 0x80000010);
    auto readonly = memory.ref(abi::AddressCodec::decode<const Frame>(0x80000018));
    assert(readonly && readonly.value.field<Continuation>().value.load().value == target);
    auto protected_cell = memory.ref(abi::AddressCodec::decode<amos::wire::RoutineField>(0x90000000));
    assert(!protected_cell); // not mapped yet, no native alias is exposed
    auto read_only_code = abi::AddressCodec::decode<amos::wire::RoutineField>(0x90001000);
    assert(memory.map_ram(read_only_code.address(),4,Access::read) == Fault::none);
    auto protected_view = memory.ref(read_only_code);
    assert(protected_view && !protected_view.value.load().value);
    assert(protected_view.value.store(target) == Fault::permission);
    assert(!protected_view.value.load().value);
    restored.store({});
    assert(!restored.load());
    CodeReader<Signature> zero(memory,{});
    assert(zero.word_at(-2).fault != Fault::none);
    auto top = abi::AddressCodec::decode<Byte>(0xffffffff);
    assert(memory.store_be16(top,42) == Fault::overflow);
    CodeReader<Signature> end(memory,abi::EntryCodec::decode<Signature>(0xfffffffe));
    assert(end.word_at(2).fault == Fault::overflow);
    auto ro = abi::AddressCodec::decode<Byte>(0x90000000);
    assert(memory.map_ram(ro.address(),2,Access::read) == Fault::none);
    assert(memory.store_be16(ro,42) == Fault::permission);
    // Validate the complete word before writing a writable/read-only boundary.
    assert(memory.map_ram((ro-1).address(),1) == Fault::none);
    assert(memory.store_byte(ro-1,0xab) == Fault::none);
    assert(memory.store_be16(ro-1,0x1234) == Fault::permission);
    assert(memory.load_byte(ro-1).value == 0xab);
}
