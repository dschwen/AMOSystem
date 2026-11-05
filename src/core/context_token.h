// Menu register snapshots are native state, never guest RAM pointers.
#pragma once
#include "core/endian.h"
namespace amos::host {
struct ContextRegistry;
class alignas(uint32_t) ContextHandle {
    uint32_t word_;
    explicit constexpr ContextHandle(uint32_t word) : word_(word) {}
    friend struct ContextRegistry;
    friend constexpr ContextHandle decode_context(uint32_t);
    friend constexpr uint32_t encode_context(ContextHandle);
public:
    constexpr ContextHandle() = default;
    explicit constexpr operator bool() const { return word_ != 0; }
    friend constexpr bool operator==(ContextHandle, ContextHandle) = default;
};
constexpr ContextHandle decode_context(uint32_t word) { return ContextHandle(word); }
constexpr uint32_t encode_context(ContextHandle handle) { return handle.word_; }
}
namespace amos::wire {
class ContextField {
    be_u32 word_;
public:
    host::ContextHandle load() const { return host::decode_context(uint32_t(word_)); }
    void store(host::ContextHandle handle) { word_ = host::encode_context(handle); }
};
static_assert(sizeof(ContextField) == 4 && alignof(ContextField) == alignof(be_u32));
}
