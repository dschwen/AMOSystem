#include "memory/guest_code.h"
#include <assert.h>
using namespace amos::guest;
using Signature = void();
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
