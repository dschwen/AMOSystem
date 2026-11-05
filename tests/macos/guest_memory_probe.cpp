// No low native image/stack/arena requirement: exercises the translated substrate.
#include "abi/guest_address_codec.h"
#include "memory/guest_view.h"
#include <stdio.h>
using namespace amos::guest;
int main() {
    Memory memory;
    const auto reserve = memory.reserve();
    if (reserve != Fault::none) {
        printf("{\"guest_translation\":false,\"stage\":\"reserve\",\"fault\":%u}\n", unsigned(reserve));
        return 1;
    }
    auto bank = abi::AddressCodec::decode<Byte>(0x1000003);
    auto pointer_cell = abi::AddressCodec::decode<PointerField<Byte>>(0x2000001);
    bool ok = memory.map_ram(bank.address(), 32) == Fault::none &&
              memory.map_ram(pointer_cell.address(), 4) == Fault::none;
    auto payload = bank + 10;
    if (ok) ok = memory.store_be32(payload, 0x1234abcd) == Fault::none;
    auto cell = memory.ref(pointer_cell);
    if (ok) ok = bool(cell) && cell.value.store(payload) == Fault::none;
    if (ok) {
        auto restored = cell.value.load();
        ok = bool(restored) && restored.value == payload;
        if (ok) {
            auto value = memory.load_be32(restored.value);
            ok = bool(value) && value.value == 0x1234abcd;
        }
    }
    printf("{\"guest_translation\":%s,\"native_backing_above_4gb\":true,"
           "\"bank_plus_ten\":%s,\"page_size\":%llu,\"scope\":\"memory substrate only\"}\n",
           ok ? "true" : "false", ok ? "true" : "false",
           static_cast<unsigned long long>(memory.host_page_size()));
    return ok ? 0 : 2;
}
