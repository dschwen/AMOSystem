#include "runtime/marks.h"
#include "runtime/detail/legacy_context.h"
#include <assert.h>
#if defined(__linux__)
#include <sys/mman.h>
#endif
namespace amos {
extern "C" { uint8_t amos_low_page[4096]{}; }
namespace { Ctx current{}; }
Ctx& ctx() { return current; }
}
using namespace amos;
int main() {
    // Logical high guest addresses survive live register transfers unchanged.
    ctx().a5 = decode_data_zone(0xe0001000);
    ctx().d6 = 13; ctx().d7 = 17;
    auto registers = host::context_regs();
    assert(registers.a[5] == 0xe0001000);
    ctx().a5 = {};
    host::to_context(registers);
    assert(encode_data_zone(ctx().a5) == 0xe0001000);
    auto snapshot = host::detail::try_capture_context(ctx());
    assert(snapshot && host::context_address(snapshot.value.a5) == 0xe0001000);
    auto shifted = ctx().a5 + 10;
    assert(encode_data_zone(shifted) == 0xe000100a);
    auto high_native = reinterpret_cast<uint8_t*>(&registers);
    assert(reinterpret_cast<uintptr_t>(high_native) > UINT32_MAX);
    assert(host::detail::try_capture_data_zone(high_native).fault == guest::Fault::overflow);
    assert(!host::detail::try_capture_data_zone(nullptr).value);
#if defined(__linux__) && defined(MAP_32BIT)
    // This tests the explicitly temporary identity overlay, not high backing.
    const size_t size = sizeof(WData) + sizeof(IData);
    auto* native = static_cast<uint8_t*>(mmap(nullptr,size,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_32BIT,-1,0));
    assert(native != MAP_FAILED && reinterpret_cast<uintptr_t>(native)+size <= UINT32_MAX);
    ctx().a5 = host::detail::capture_data_zone(native+sizeof(WData));
    assert(reinterpret_cast<uint8_t*>(&idata()) == native+sizeof(WData));
    assert(reinterpret_cast<uint8_t*>(&wdata()) == native);
    idata().ParamE = 0x1234abcd;
    wdata().Actualise = 0x5678;
    assert(peek_l(native+sizeof(WData)+offsetof(IData,ParamE)) == 0x1234abcd);
    assert(peek_w(native+offsetof(WData,Actualise)) == 0x5678);
    assert(munmap(native,size) == 0);
    ctx().a5 = {};
#endif
}
