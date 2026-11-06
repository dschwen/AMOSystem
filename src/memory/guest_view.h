// Typed live views: field selection preserves address domains and constness.
#pragma once
#include "memory/guest_memory.h"
#include "core/routine_field.h"
#pragma pack(push, 8)
namespace amos::guest {
template<class Owner, class Value, uint32_t Offset> struct Field {
    using owner_type = Owner;
    using value_type = Value;
    static constexpr uint32_t offset = Offset;
    static_assert(Sized<Owner> && Sized<Value>);
    static_assert(Offset <= Layout<Owner>::size);
    static_assert(Layout<Value>::size <= Layout<Owner>::size - Offset);
};
template<class T> struct PointerValue;
template<class T> struct PointerValue<PointerField<T>> { using type = T; };
template<class T> concept PointerStorage = requires { typename PointerValue<UnconstT<T>>::type; };
template<class T> inline constexpr bool is_const = !same_type<T, UnconstT<T>>;
template<class T, bool Const> struct WithConst { using type = T; };
template<class T> struct WithConst<T, true> { using type = const T; };

template<class T> class alignas(
#if defined(__mc68000__)
    pointer_alignment
#else
    8
#endif
) GuestRef {
#if !defined(__mc68000__)
    Memory* memory_ = nullptr;
#endif
    GuestPtr<T> address_{};
#if defined(__mc68000__)
    explicit GuestRef(Memory&, GuestPtr<T> address) : address_(address) {}
    explicit GuestRef(GuestPtr<T> address) : address_(address) {}
    template<class> friend class GuestRef;
#else
    explicit GuestRef(Memory& memory, GuestPtr<T> address) : memory_(&memory), address_(address) {}
#endif
    friend class Memory;
public:
    GuestRef() = default; // Null native view on Amiga; faulting empty view on hosts.
    GuestPtr<T> address() const { return address_; }
    template<class F> requires same_type<typename F::owner_type, UnconstT<T>>
    auto field() const {
        using V = typename WithConst<typename F::value_type, is_const<T>>::type;
#if defined(__mc68000__)
        return Result<GuestRef<V>>{GuestRef<V>(GuestPtr<V>(address_.bits_ + F::offset)), Fault::none};
#else
        if (!memory_) return Result<GuestRef<V>>{{}, Fault::unmapped};
        auto offset = address_.try_offset_bytes(GuestByteDelta(F::offset));
        if (!offset) return Result<GuestRef<V>>{{}, offset.fault};
        return memory_->ref(GuestPtr<V>(offset.value.bits_));
#endif
    }
    auto load() const requires PointerStorage<T> {
        using Pointee = typename PointerValue<UnconstT<T>>::type;
#if defined(__mc68000__)
        return Memory{}.load_pointer(GuestPtr<const PointerField<Pointee>>(address_));
#else
        if (!memory_) return Result<GuestPtr<Pointee>>{{}, Fault::unmapped};
        return memory_->load_pointer(GuestPtr<const PointerField<Pointee>>(address_));
#endif
    }
    template<class Pointee> requires (PointerStorage<T> && !is_const<T> &&
        same_type<UnconstT<T>, PointerField<Pointee>>)
    Fault store(GuestPtr<Pointee> value) const {
#if defined(__mc68000__)
        return Memory{}.store_pointer(address_, value);
#else
        if (!memory_) return Fault::unmapped;
        return memory_->store_pointer(address_, value);
#endif
    }
    Result<host::RoutineArg> load() const requires same_type<UnconstT<T>, wire::RoutineField> {
#if defined(__mc68000__)
        auto word = Memory{}.load_be32(GuestPtr<const Byte>(address_.bits_));
#else
        if (!memory_) return {{}, Fault::unmapped};
        auto word = memory_->load_be32(GuestPtr<const Byte>(address_.bits_));
#endif
        if (!word) return {{}, word.fault};
        return {host::decode_routine(word.value), Fault::none};
    }
    Fault store(host::RoutineArg entry) const requires same_type<T, wire::RoutineField> {
#if defined(__mc68000__)
        return Memory{}.store_be32(GuestPtr<Byte>(address_.bits_), host::encode_routine(entry));
#else
        if (!memory_) return Fault::unmapped;
        return memory_->store_be32(GuestPtr<Byte>(address_.bits_), host::encode_routine(entry));
#endif
    }
    Result<uint32_t> load() const requires same_type<UnconstT<T>, uint32_t> {
#if defined(__mc68000__)
        return Memory{}.load_be32(GuestPtr<const Byte>(address_.bits_));
#else
        if (!memory_) return {{}, Fault::unmapped};
        return memory_->load_be32(GuestPtr<const Byte>(address_.bits_));
#endif
    }
    Fault store(uint32_t value) const requires same_type<T, uint32_t> {
#if defined(__mc68000__)
        return Memory{}.store_be32(GuestPtr<Byte>(address_.bits_), value);
#else
        if (!memory_) return Fault::unmapped;
        return memory_->store_be32(GuestPtr<Byte>(address_.bits_), value);
#endif
    }
};
template<class T> requires Sized<T>
Result<GuestRef<T>> Memory::ref(GuestPtr<T> address) {
#if !defined(__mc68000__)
    const auto fault = check(address.bits_, Layout<UnconstT<T>>::size, false);
    if (fault != Fault::none) return {{}, fault};
#endif
    return {GuestRef<T>(*this, address), Fault::none};
}
}
#pragma pack(pop)
