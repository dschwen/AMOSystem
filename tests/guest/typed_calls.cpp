#include "runtime/typed_call.h"
#include "runtime/native_binding.h"
#include <assert.h>
using namespace amos;
using namespace amos::host;
namespace amos { extern "C" { uint8_t amos_low_page[4096]{}; } }
uint32_t zero() { return 0xdeadbeef; }
int32_t signed_value(int8_t a, int16_t b, int32_t c) { return int32_t(a) + b + c; }
uint32_t ten(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t e,
             uint32_t f, uint32_t g, uint32_t h, uint32_t i, uint32_t j) {
    return a + 2*b + 3*c + 4*d + 5*e + 6*f + 7*g + 8*h + 9*i + 10*j;
}
uint64_t pair(uint32_t hi, uint32_t lo) { return uint64_t(hi) << 32 | lo; }
uint32_t native_string(const uint8_t* bytes, uint16_t index) { return bytes[index]; }
unsigned calls;
void no_result(uint32_t n) { calls += n; }
guest::GuestPtr<const guest::Byte> guest_result(guest::GuestPtr<const guest::Byte> at, uint32_t offset) { return at + offset; }
uint32_t reg_pointer(RegFrame* r) { return ++r->d[0]; }
uint32_t reg_reference(RegFrame& r) { return ++r.d[0]; }
struct FloatResult { uint32_t kind, high, low; };
void float_record(FloatResult* out) { out->kind = 1; out->high += 3; out->low += 5; }
int main() {
    const uint32_t words[10] = {1,2,3,4,5,6,7,8,9,10};
    assert((typed_library_call<&zero, 0, 4>(nullptr) == 0xdeadbeef));
    assert((typed_library_call<&ten, 10, 4>(words) == 385));
    assert((typed_library_call<&pair, 2, 8>(words) == 0x100000002ull));
    assert((typed_library_call<&no_result, 1, 0>(words) == 0 && calls == 1));
    const uint32_t negatives[3] = {0xff, 0xffff, 0xffffffff};
    assert((typed_library_call<&signed_value, 3, 4>(negatives) == 0xfffffffdu));
    amos_low_page[17] = 93;
    const uint32_t string_args[2] = {16, 1};
    assert((typed_library_call<&native_string, 2, 4>(string_args) == 93));
    const uint32_t guest_args[2] = {0x1000000, 10};
    assert((typed_library_call<&guest_result, 2, 4>(guest_args) == 0x100000a));
    RegFrame registers{};
    assert(detail::typed_reg_call<&reg_pointer>(registers) == 1);
    assert(detail::typed_reg_call<&reg_reference>(registers) == 2);
    uint32_t floating[3] = {0, 7, 11};
    detail::typed_float_call<&float_record>(floating);
    assert(floating[0] == 1 && floating[1] == 10 && floating[2] == 16);
    constexpr auto binding = NativeBinding::from_library(&typed_library_call<&pair, 2, 8>);
    assert(binding.library(words) == 0x100000002ull);
}
