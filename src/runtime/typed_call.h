#pragma once
#include "runtime/library_call.h"
#include "runtime/detail/legacy_call_codec.h"
namespace amos::host {
namespace detail {
template<unsigned... I> struct Indices {};
template<unsigned N, unsigned... I> struct MakeIndices : MakeIndices<N-1, N-1, I...> {};
template<unsigned... I> struct MakeIndices<0, I...> { using type = Indices<I...>; };
template<class F> struct Signature;
template<class R, class... A> struct Signature<R (*)(A...)> {
    static constexpr unsigned arity = sizeof...(A);
    template<auto Fn, unsigned Width, unsigned... I>
    static uint64_t invoke(const uint32_t* words, Indices<I...>) {
        if constexpr (guest::same_type<R, void>) {
            static_assert(Width == 0, "void library function used as a value");
            Fn(CallArgument<A>::decode(words[I])...);
            return 0;
        } else {
            static_assert(Width == 0 || Width == sizeof(R), "wrong library result width");
            return call_result(Fn(CallArgument<A>::decode(words[I])...));
        }
    }
};
template<class R, class... A> struct Signature<R (*)(A...) noexcept> : Signature<R (*)(A...)> {};
}
template<auto Fn, unsigned Arity, unsigned Width>
uint64_t typed_library_call(const uint32_t* words) {
    using S = detail::Signature<decltype(Fn)>;
    static_assert(S::arity == Arity && Arity <= 10, "library collector/signature mismatch");
    return S::template invoke<Fn, Width>(words, typename detail::MakeIndices<Arity>::type{});
}
}

#if !defined(__mc68000__)
namespace amos::host::detail {
template<class T> struct Pointee;
template<class T> struct Pointee<T*> { using type = T; };
template<class F> struct Unary;
template<class R, class A> struct Unary<R (*)(A)> { using argument = A; };
template<auto Fn> uint32_t typed_reg_call(RegFrame& registers) {
    using A = typename Unary<decltype(Fn)>::argument;
    static_assert(guest::same_type<A, RegFrame&> || guest::same_type<A, RegFrame*>,
                  "register callback must declare RegFrame, not an unrelated record");
    if constexpr (guest::same_type<A, RegFrame*>) return uint32_t(Fn(&registers));
    else return uint32_t(Fn(registers));
}
// Float glue has a native 3-word out record. Use its actual C++ object type,
// rather than calling a different function-pointer type through a cast.
template<auto Fn> void typed_float_call(uint32_t* words) {
    using A = typename Unary<decltype(Fn)>::argument;
    using Record = typename Pointee<A>::type;
    static_assert(sizeof(Record) == 3 * sizeof(uint32_t));
    static_assert(__is_trivially_copyable(Record));
    Record value{};
    __builtin_memcpy(&value, words, sizeof value);
    Fn(&value);
    __builtin_memcpy(words, &value, sizeof value);
}
}
#endif
