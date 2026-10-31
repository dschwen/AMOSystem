// The byte order of AMOS's memory: big-endian, as on the 68000 and in its
// files (programs, banks, token tables, configurations). On a
// little-endian host the fields of the layouts (core/layout.h) are kept
// big-endian too, by be_u16/be_u32, which convert on each access; on the
// 68000 they are plain integers.
#pragma once

#include <stdint.h>

namespace amos {

#if defined(__mc68000__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

using be_u16 = uint16_t;
using be_u32 = uint32_t;
using be_i16 = int16_t;
using be_i32 = int32_t;

#else

} // namespace amos
#include <type_traits>
namespace amos {

template <typename T> struct BigEndian {
    uint8_t b[sizeof(T)];

    BigEndian() = default;
    explicit constexpr BigEndian(T value) : b{} { set(value); }

    T get() const
    {
        uint32_t v = 0;
        for (unsigned i = 0; i < sizeof(T); i++)
            v = v << 8 | b[i];
        return T(v);
    }
    T get() const volatile
    {
        uint32_t v = 0;
        for (unsigned i = 0; i < sizeof(T); i++)
            v = v << 8 | b[i];
        return T(v);
    }
    constexpr void set(T value)
    {
        uint32_t v = value;
        for (int i = int(sizeof(T)) - 1; i >= 0; i--, v >>= 8)
            b[i] = uint8_t(v);
    }
    void set(T value) volatile
    {
        uint32_t v = value;
        for (int i = int(sizeof(T)) - 1; i >= 0; i--, v >>= 8)
            b[i] = uint8_t(v);
    }

    operator T() const { return get(); }
    operator T() const volatile { return get(); }
    BigEndian &operator=(T v)
    {
        set(v);
        return *this;
    }
    void operator=(T v) volatile { set(v); }
    BigEndian &operator=(const BigEndian &) = default;

    template <typename U> requires(std::is_integral_v<U>) BigEndian &operator+=(U v) { return *this = T(get() + v); }
    template <typename U> requires(std::is_integral_v<U>) BigEndian &operator-=(U v) { return *this = T(get() - v); }
    template <typename U> requires(std::is_integral_v<U>) BigEndian &operator|=(U v) { return *this = T(get() | v); }
    template <typename U> requires(std::is_integral_v<U>) BigEndian &operator&=(U v) { return *this = T(get() & v); }
    template <typename U> requires(std::is_integral_v<U>) BigEndian &operator^=(U v) { return *this = T(get() ^ v); }
    BigEndian &operator<<=(int n) { return *this = T(get() << n); }
    BigEndian &operator>>=(int n) { return *this = T(get() >> n); }
    template <typename U> requires(std::is_integral_v<U>) void operator+=(U v) volatile { set(T(get() + v)); }
    template <typename U> requires(std::is_integral_v<U>) void operator-=(U v) volatile { set(T(get() - v)); }
    template <typename U> requires(std::is_integral_v<U>) void operator|=(U v) volatile { set(T(get() | v)); }
    template <typename U> requires(std::is_integral_v<U>) void operator&=(U v) volatile { set(T(get() & v)); }
    template <typename U> requires(std::is_integral_v<U>) void operator^=(U v) volatile { set(T(get() ^ v)); }
    BigEndian &operator+=(T v) { return *this = T(get() + v); }
    BigEndian &operator-=(T v) { return *this = T(get() - v); }
    BigEndian &operator|=(T v) { return *this = T(get() | v); }
    BigEndian &operator&=(T v) { return *this = T(get() & v); }
    BigEndian &operator^=(T v) { return *this = T(get() ^ v); }
    void operator+=(T v) volatile { set(T(get() + v)); }
    void operator-=(T v) volatile { set(T(get() - v)); }
    void operator|=(T v) volatile { set(T(get() | v)); }
    void operator&=(T v) volatile { set(T(get() & v)); }
    void operator^=(T v) volatile { set(T(get() ^ v)); }
    BigEndian &operator++() { return *this = T(get() + 1); }
    BigEndian &operator--() { return *this = T(get() - 1); }
    T operator++(int)
    {
        T v = get();
        set(T(v + 1));
        return v;
    }
    T operator--(int)
    {
        T v = get();
        set(T(v - 1));
        return v;
    }
};

using be_u16 = BigEndian<uint16_t>;
using be_u32 = BigEndian<uint32_t>;
using be_i16 = BigEndian<int16_t>;
using be_i32 = BigEndian<int32_t>;

#endif

// The native type of a field: T for T and for BigEndian<T>
template <typename T> struct NativeOf {
    using type = T;
};
#if !(defined(__mc68000__) || (defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__))
template <typename T> struct NativeOf<BigEndian<T>> {
    using type = T;
};
#endif
template <typename T> using native_t = typename NativeOf<T>::type;

} // namespace amos
