// Strong address domains. This API deliberately does not include core/amos.h:
// the legacy engine remains separate until its ABI and call sites are migrated.
#pragma once
#include <stddef.h>
#include <stdint.h>

#pragma pack(push, 8)
namespace amos::guest {
using Byte = uint8_t;
// Like native pointers, default-initialization leaves scalar storage untouched.
// Value-initialize with {} for null/zero; this keeps default construction a no-op.
#if defined(__mc68000__)
// Native pointer arithmetic on the 68000: no translation or automatic checks.
using PointerDifference = int32_t;
using SpanSize = uint32_t;
inline constexpr unsigned pointer_alignment = alignof(uint32_t);
inline constexpr unsigned offset_alignment = alignof(int32_t);
#else
using PointerDifference = int64_t;
using SpanSize = uint64_t;
inline constexpr unsigned pointer_alignment = 4;
inline constexpr unsigned offset_alignment = 8;
#endif
enum class Fault : uint8_t { none, null_pointer, overflow, misaligned, unmapped,
    permission, exhausted, invalid_range, platform };
// Operator arithmetic cannot return an error value. Before runtime integration,
// host callers needing recovery use try_advance; the host failure hook is fatal.
// Ordinary Amiga operators use native preconditions and never call this hook.
[[noreturn]] void pointer_fault(Fault);

template<class T> struct Result {
    T value{};
    Fault fault = Fault::none;
    explicit constexpr operator bool() const { return fault == Fault::none; }
};
template<class T, class U> inline constexpr bool same_type = false;
template<class T> inline constexpr bool same_type<T, T> = true;
template<class T> struct Unconst { using type = T; };
template<class T> struct Unconst<const T> { using type = T; };
template<class T> using UnconstT = typename Unconst<T>::type;
template<class From, class To> inline constexpr bool const_compatible =
    same_type<From, To> || same_type<const From, To>;

// Only schema-defined guest types have an element stride. No native sizeof(T).
template<class T> struct Layout;
template<> struct Layout<uint8_t> { static constexpr uint32_t size = 1; };
template<> struct Layout<int8_t> { static constexpr uint32_t size = 1; };
template<> struct Layout<uint16_t> { static constexpr uint32_t size = 2; };
template<> struct Layout<int16_t> { static constexpr uint32_t size = 2; };
template<> struct Layout<uint32_t> { static constexpr uint32_t size = 4; };
template<> struct Layout<int32_t> { static constexpr uint32_t size = 4; };
template<class T> concept Sized = requires { Layout<UnconstT<T>>::size; };

// Limit arithmetic to integers and reject unsigned values that cannot fit the
// signed intermediate before any conversion (UINT64_MAX must not become -1).
template<class T> inline constexpr bool integer = false;
template<> inline constexpr bool integer<char> = true;
template<> inline constexpr bool integer<signed char> = true;
template<> inline constexpr bool integer<unsigned char> = true;
template<> inline constexpr bool integer<short> = true;
template<> inline constexpr bool integer<unsigned short> = true;
template<> inline constexpr bool integer<int> = true;
template<> inline constexpr bool integer<unsigned int> = true;
template<> inline constexpr bool integer<long> = true;
template<> inline constexpr bool integer<unsigned long> = true;
template<> inline constexpr bool integer<long long> = true;
template<> inline constexpr bool integer<unsigned long long> = true;
template<class T> concept Integer = integer<T>;
template<Integer N> constexpr Result<int64_t> signed_offset(N n) {
    if constexpr (N(-1) > N(0)) {
        if (uint64_t(n) > uint64_t(INT64_MAX)) return {{}, Fault::overflow};
    }
    return {int64_t(n), Fault::none};
}

class Memory;
class SegmentBindings;
namespace abi { struct AddressCodec; }
template<class T> class PointerField;
template<class T> class GuestPtr;
template<class T> class GuestRef;

class alignas(pointer_alignment) GuestAddress32 {
    uint32_t bits_;
    explicit constexpr GuestAddress32(uint32_t bits) : bits_(bits) {}
    friend struct abi::AddressCodec;
    friend class Memory;
    friend class SegmentBindings;
    template<class> friend class GuestPtr;
public:
    constexpr GuestAddress32() = default;
    explicit constexpr operator bool() const { return bits_ != 0; }
    friend constexpr bool operator==(GuestAddress32, GuestAddress32) = default;
};

class alignas(offset_alignment) GuestByteDelta {
    PointerDifference bytes_;
public:
    template<Integer N> explicit constexpr GuestByteDelta(N bytes) : bytes_(0) {
#if defined(__mc68000__)
        bytes_ = PointerDifference(bytes); // Native ptrdiff_t preconditions apply.
#else
        auto normalized = signed_offset(bytes);
        if (!normalized) pointer_fault(normalized.fault);
        bytes_ = normalized.value;
#endif
    }
    constexpr PointerDifference bytes() const { return bytes_; }
};
template<class T> class alignas(offset_alignment) GuestDistance {
    PointerDifference elements_;
    explicit constexpr GuestDistance(PointerDifference n) : elements_(n) {}
    template<class> friend class GuestPtr;
public:
    constexpr GuestDistance() = default;
    constexpr PointerDifference elements() const { return elements_; }
};

template<class T> class alignas(pointer_alignment) GuestPtr {
    uint32_t bits_;
    explicit constexpr GuestPtr(uint32_t bits) : bits_(bits) {}
    template<class> friend class GuestPtr;
    template<class> friend class PointerField;
    template<class> friend class GuestRef;
    friend struct abi::AddressCodec;
    friend class Memory;
    friend class SegmentBindings;
public:
    constexpr GuestPtr() = default;
    template<class U> requires const_compatible<U, T>
    constexpr GuestPtr(GuestPtr<U> other) : bits_(other.bits_) {}
    explicit constexpr operator bool() const { return bits_ != 0; }
    constexpr GuestAddress32 address() const { return GuestAddress32(bits_); }

    constexpr Result<GuestPtr> try_offset_bytes(GuestByteDelta delta) const {
        if (!bits_) return {{}, Fault::null_pointer};
        const int64_t n = delta.bytes();
        // Compare before adding, so even INT64_MIN/MAX are well-defined.
        if (n < -int64_t(bits_) || n > int64_t(UINT32_MAX) - bits_)
            return {{}, Fault::overflow};
        if (int64_t(bits_) + n == 0) return {{}, Fault::null_pointer};
        return {GuestPtr(uint32_t(int64_t(bits_) + n)), Fault::none};
    }
    template<Integer N> constexpr Result<GuestPtr> try_advance(N count) const requires Sized<T> {
        auto normalized = signed_offset(count);
        if (!normalized) return {{}, normalized.fault};
        const int64_t elements = normalized.value;
        constexpr int64_t stride = Layout<UnconstT<T>>::size;
        static_assert(stride > 0);
        if (elements > INT64_MAX / stride || elements < INT64_MIN / stride)
            return {{}, Fault::overflow};
#if defined(__mc68000__)
        if (elements * stride < INT32_MIN || elements * stride > INT32_MAX)
            return {{}, Fault::overflow};
#endif
        return try_offset_bytes(GuestByteDelta(elements * stride));
    }
    GuestPtr offset_bytes(GuestByteDelta delta) const {
#if defined(__mc68000__)
        return GuestPtr(bits_ + uint32_t(delta.bytes()));
#else
        auto r = try_offset_bytes(delta);
        if (!r) pointer_fault(r.fault);
        return r.value;
#endif
    }
    template<Integer N> GuestPtr operator+(N n) const requires Sized<T> {
#if defined(__mc68000__)
        return GuestPtr(bits_ + uint32_t(n) * Layout<UnconstT<T>>::size);
#else
        auto r = try_advance(n); if (!r) pointer_fault(r.fault); return r.value;
#endif
    }
    template<Integer N> GuestPtr operator-(N count) const requires Sized<T> {
#if defined(__mc68000__)
        return GuestPtr(bits_ - uint32_t(count) * Layout<UnconstT<T>>::size);
#else
        auto normalized = signed_offset(count);
        if (!normalized) pointer_fault(normalized.fault);
        const int64_t n = normalized.value;
        if (n == INT64_MIN) pointer_fault(Fault::overflow);
        return *this + (-n);
#endif
    }
    template<Integer N> friend GuestPtr operator+(N n, GuestPtr p) requires Sized<T> { return p + n; }
    template<Integer N> GuestPtr& operator+=(N n) requires Sized<T> { return *this = *this + n; }
    template<Integer N> GuestPtr& operator-=(N n) requires Sized<T> { return *this = *this - n; }
    GuestPtr& operator++() requires Sized<T> { return *this += 1; }
    GuestPtr operator++(int) requires Sized<T> { auto p = *this; ++*this; return p; }
    GuestPtr& operator--() requires Sized<T> { return *this -= 1; }
    GuestPtr operator--(int) requires Sized<T> { auto p = *this; --*this; return p; }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr Result<GuestDistance<UnconstT<T>>> try_distance(GuestPtr<U> other) const
        requires Sized<T> {
        if (!bits_ || !other.bits_) return {{}, Fault::null_pointer};
        const int64_t bytes = int64_t(bits_) - other.bits_;
        constexpr int64_t stride = Layout<UnconstT<T>>::size;
        if (bytes % stride) return {{}, Fault::misaligned};
#if defined(__mc68000__)
        if (bytes / stride < INT32_MIN || bytes / stride > INT32_MAX)
            return {{}, Fault::overflow};
#endif
        return {GuestDistance<UnconstT<T>>(PointerDifference(bytes / stride)), Fault::none};
    }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    auto operator-(GuestPtr<U> other) const requires Sized<T> {
#if defined(__mc68000__)
        // Native pointer subtraction tells GCC division is exact; ordinary
        // signed division would add negative-remainder correction instructions.
        struct Element { Byte bytes[Layout<UnconstT<T>>::size]; };
        return GuestDistance<UnconstT<T>>(reinterpret_cast<const Element*>(bits_) -
                                         reinterpret_cast<const Element*>(other.bits_));
#else
        auto r = try_distance(other); if (!r) pointer_fault(r.fault); return r.value;
#endif
    }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr bool operator==(GuestPtr<U> other) const { return bits_ == other.bits_; }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr bool operator<(GuestPtr<U> other) const {
#if defined(__mc68000__)
        return reinterpret_cast<const Byte*>(bits_) < reinterpret_cast<const Byte*>(other.bits_);
#else
        return bits_ < other.bits_;
#endif
    }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr bool operator>(GuestPtr<U> other) const { return other < *this; }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr bool operator<=(GuestPtr<U> other) const { return !(other < *this); }
    template<class U> requires same_type<UnconstT<T>, UnconstT<U>>
    constexpr bool operator>=(GuestPtr<U> other) const { return !(*this < other); }
};

template<class T> class alignas(offset_alignment) GuestSpan {
    GuestPtr<T> start_{};
    SpanSize count_ = 0;
public:
    constexpr GuestSpan() = default;
    explicit constexpr GuestSpan(GuestPtr<T> start, SpanSize count) : start_(start), count_(count) {}
    template<class U> requires const_compatible<U, T>
    constexpr GuestSpan(GuestSpan<U> other) : start_(other.data()), count_(other.size()) {}
    constexpr GuestPtr<T> data() const { return start_; }
    constexpr SpanSize size() const { return count_; }
    constexpr Result<GuestSpan> try_subspan(SpanSize offset, SpanSize count) const requires Sized<T> {
        if (offset > count_ || count > count_ - offset || offset > uint64_t(INT64_MAX))
            return {{}, Fault::invalid_range};
        if (!offset) return {GuestSpan(start_, count), Fault::none};
        auto next = start_.try_advance(int64_t(offset));
        if (!next) return {{}, next.fault};
        return {GuestSpan(next.value, count), Fault::none};
    }
    constexpr Result<GuestSpan> subspan(SpanSize offset, SpanSize count) const requires Sized<T> {
#if defined(__mc68000__)
        return {GuestSpan(start_ + offset, count), Fault::none};
#else
        return try_subspan(offset, count);
#endif
    }
};

template<class T> class alignas(
#if defined(__mc68000__)
    alignof(void*)
#else
    sizeof(void*)
#endif
) HostPtr {
    T* value_;
    friend class Memory;
    friend class SegmentBindings;
    template<class> friend class HostPtr;
public:
    constexpr HostPtr() = default;
    explicit constexpr HostPtr(T* pointer) : value_(pointer) {}
    template<class U> requires const_compatible<U, T>
    constexpr HostPtr(HostPtr<U> other) : value_(other.value_) {}
    explicit constexpr operator bool() const { return value_ != nullptr; }
    // Trusted bootstrap only: copying a native source array into guest memory.
    constexpr T* unsafe_native() const { return value_; }
};
template<class T> class alignas(offset_alignment) HostSpan {
    HostPtr<T> start_{};
    SpanSize count_ = 0;
    friend class Memory;
    friend class SegmentBindings;
public:
    constexpr HostSpan() = default;
    explicit constexpr HostSpan(HostPtr<T> start, SpanSize count) : start_(start), count_(count) {}
    explicit constexpr HostSpan(T* start, SpanSize count) : start_(HostPtr<T>(start)), count_(count) {}
    template<class U> requires const_compatible<U, T>
    constexpr HostSpan(HostSpan<U> other) : start_(other.data()), count_(other.size()) {}
    constexpr HostPtr<T> data() const { return start_; }
    constexpr SpanSize size() const { return count_; }
};

// Guest wire storage is four big-endian bytes, never a native C++ pointer.
template<class T> class PointerField {
#if defined(__mc68000__)
    uint32_t bits_; // Native big-endian long, native two-byte alignment.
#else
    Byte bytes_[4];
#endif
public:
    constexpr GuestPtr<T> load() const {
#if defined(__mc68000__)
        return GuestPtr<T>(bits_);
#else
        return GuestPtr<T>((uint32_t(bytes_[0]) << 24) | (uint32_t(bytes_[1]) << 16) |
                           (uint32_t(bytes_[2]) << 8) | bytes_[3]);
#endif
    }
    constexpr void store(GuestPtr<T> value) {
#if defined(__mc68000__)
        bits_ = value.bits_;
#else
        bytes_[0] = Byte(value.bits_ >> 24); bytes_[1] = Byte(value.bits_ >> 16);
        bytes_[2] = Byte(value.bits_ >> 8); bytes_[3] = Byte(value.bits_);
#endif
    }
};
template<class T> struct Layout<PointerField<T>> { static constexpr uint32_t size = 4; };
static_assert(sizeof(GuestAddress32) == 4 && alignof(GuestAddress32) == pointer_alignment);
static_assert(sizeof(GuestPtr<Byte>) == 4 && alignof(GuestPtr<Byte>) == pointer_alignment);
static_assert(sizeof(PointerField<Byte>) == 4);
#if defined(__mc68000__)
static_assert(alignof(PointerField<Byte>) == alignof(uint32_t));
#else
static_assert(alignof(PointerField<Byte>) == 1);
#endif
#if defined(__mc68000__)
static_assert(sizeof(GuestSpan<Byte>) == 8 && alignof(GuestSpan<Byte>) == alignof(uint32_t));
static_assert(sizeof(HostSpan<Byte>) == 8 && alignof(HostSpan<Byte>) == alignof(void*));
#else
static_assert(sizeof(GuestSpan<Byte>) == 16 && alignof(GuestSpan<Byte>) == 8);
#endif
} // namespace amos::guest
#pragma pack(pop)
