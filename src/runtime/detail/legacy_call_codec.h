// Finite migration boundary: legacy entry signatures still accept native T*.
// New signatures use GuestPtr directly. Remove these raw-pointer specializations
// when the corresponding library implementations have migrated.
#pragma once
#include "core/amos.h"
#include "abi/guest_address_codec.h"
namespace amos::host::detail {
template<class T> inline constexpr bool function_type = false;
template<class R, class... A> inline constexpr bool function_type<R(A...)> = true;
template<class R, class... A> inline constexpr bool function_type<R(A...) noexcept> = true;
template<class R, class... A> inline constexpr bool function_type<R(A..., ...)> = true;
template<class R, class... A> inline constexpr bool function_type<R(A..., ...) noexcept> = true;
template<class T> struct CallArgument {
    static T decode(uint32_t word) {
        static_assert((guest::Integer<T> || guest::same_type<T, bool>) && sizeof(T) <= 4,
                      "library arguments require an explicit architectural codec");
        return static_cast<T>(word);
    }
};
template<class T> struct CallArgument<T*> {
    static T* decode(uint32_t word) {
        static_assert(!function_type<T>, "callbacks require a declared guest-entry codec");
        return ptr<T>(word);
    }
};
template<class T> struct CallArgument<guest::GuestPtr<T>> {
    static guest::GuestPtr<T> decode(uint32_t word) { return guest::abi::AddressCodec::decode<T>(word); }
};
template<class T> uint64_t call_result(T value) {
    static_assert(guest::Integer<T> || guest::same_type<T, bool>, "result requires a declared codec");
    if constexpr (sizeof(T) <= 4) return uint32_t(value);
    else return uint64_t(value);
}
template<class T> uint64_t call_result(T* value) {
    static_assert(!function_type<T>, "callback results require a declared guest-entry codec");
    return address(value);
}
template<class T> uint64_t call_result(guest::GuestPtr<T> value) {
    return guest::abi::AddressCodec::encode(value.address());
}
}
