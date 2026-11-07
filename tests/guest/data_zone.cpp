#include "runtime/marks.h"
#include "runtime/detail/identity_data_memory.h"
#include "runtime/detail/legacy_context.h"
#include <assert.h>
#if defined(__linux__)
#include <sys/mman.h>
#endif
extern "C" { uint8_t amos_low_page[4096]{}; }
namespace amos {
namespace { Ctx current{}; }
Ctx& ctx() { return current; }
}
#if defined(AMOS_GUEST_BIASED)
extern "C" {
uint8_t* amos_guest_base = nullptr;
uint64_t amos_guest_size = 0;
uint8_t* amos_handle_native(uint32_t) { return nullptr; }
uint32_t amos_address_escape(uintptr_t, const char*, int) { return AmosHandleBase + 0x100000; }
}
#endif
using namespace amos;
int main() {
    // Logical high guest addresses survive live register transfers unchanged.
    ctx().a3 = decode_context_cursor(0xb0003000);
    ctx().a5 = decode_data_zone(0xe0001000);
    ctx().a4 = decode_context_cursor(0xd0002000);
    ctx().d6 = 13; ctx().d7 = 17;
    auto registers = host::context_regs();
    assert(registers.a[5] == 0xe0001000 && registers.a[4] == 0xd0002000 && registers.a[3] == 0xb0003000);
    ctx().a5 = {}; ctx().a4 = {}; ctx().a3 = {};
    host::to_context(registers);
    assert(encode_data_zone(ctx().a5) == 0xe0001000);
    assert(encode_context_cursor(ctx().a4) == 0xd0002000);
    assert(encode_context_cursor(ctx().a3) == 0xb0003000);
    ctx().a6 = decode_context_cursor(0xc0004000);
    auto snapshot = host::detail::capture_context(ctx());
    assert(host::context_address(snapshot.a5) == 0xe0001000);
    assert(host::context_address(snapshot.a4) == 0xd0002000);
    assert(host::context_address(snapshot.a3) == 0xb0003000);
    assert(host::context_address(snapshot.a6) == 0xc0004000);
    auto shifted = ctx().a5 + 10;
    assert(encode_data_zone(shifted) == 0xe000100a);
    auto high_native = reinterpret_cast<uint8_t*>(&registers);
    assert(reinterpret_cast<uintptr_t>(high_native) > UINT32_MAX);
#if defined(AMOS_GUEST_BIASED)
    // Preserve the existing biased bridge's explicit native-window handoff.
    assert(encode_context_cursor(host::detail::try_capture_cursor(high_native).value) == AmosHandleBase + 0x100000);
#else
    assert(host::detail::try_capture_cursor(high_native).fault == guest::Fault::overflow);
#endif
    assert(!host::detail::try_capture_cursor(nullptr).value);
#if defined(__linux__) && defined(MAP_32BIT)
    const size_t size = sizeof(WData) + sizeof(IData);
#if defined(AMOS_GUEST_BIASED)
    const size_t prefix = 0x10000;
    auto* mapping = static_cast<uint8_t*>(mmap(nullptr,prefix+size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0));
    assert(mapping != MAP_FAILED && reinterpret_cast<uintptr_t>(mapping) > UINT32_MAX);
    amos_guest_base = mapping; amos_guest_size = prefix+size;
    auto* native = mapping+prefix;
#else
    // Exercise the explicitly temporary identity overlay too.
    const size_t prefix = 0;
    auto* mapping = static_cast<uint8_t*>(mmap(nullptr,size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_32BIT,-1,0));
    auto* native = mapping;
    assert(native != MAP_FAILED && reinterpret_cast<uintptr_t>(native)+size <= UINT32_MAX);
#endif
    ctx().a5 = host::detail::capture_cursor(native+sizeof(WData));
    assert(reinterpret_cast<uint8_t*>(&idata()) == native+sizeof(WData));
    assert(reinterpret_cast<uint8_t*>(&wdata()) == native);
    ctx().a4 = host::detail::capture_cursor(native);
    host::detail::write_cursor_byte(ctx().a4,0xab);
    host::detail::write_cursor_byte(ctx().a4,0xcd);
    assert(encode_context_cursor(ctx().a4) == address(native+2));
    assert(native[0] == 0xab && native[1] == 0xcd);
    host::detail::IdentityDataMemory memory;
    auto at = guest_cursor(ctx().a4);
    assert(memory.store_be32(at,0x12345678) == guest::Fault::none);
    assert(memory.load_be32(at).value == 0x12345678);
    ctx().a3 = host::detail::capture_cursor(native+32);
    ctx().a3 -= 4;
    host::detail::write_cursor_long(ctx().a3,0x89abcdef);
    ctx().a3 -= 2;
    host::detail::write_cursor_word(ctx().a3,0x1234);
    assert(host::detail::read_cursor_word(ctx().a3) == 0x1234);
    ctx().a3 += 2;
    assert(host::detail::read_cursor_long(ctx().a3) == 0x89abcdef);
    ctx().a3 += 4;
    assert(encode_context_cursor(ctx().a3) == address(native+32));
    idata().ParamE = 0x1234abcd;
    wdata().Actualise = 0x5678;
    assert(peek_l(native+sizeof(WData)+offsetof(IData,ParamE)) == 0x1234abcd);
    assert(peek_w(native+offsetof(WData,Actualise)) == 0x5678);
    assert(munmap(mapping,prefix+size) == 0);
    ctx().a3 = {}; ctx().a4 = {}; ctx().a5 = {};
#if defined(AMOS_GUEST_BIASED)
    amos_guest_base = nullptr; amos_guest_size = 0;
#endif
#endif
}
