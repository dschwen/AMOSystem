// Recovery tokens and their guest wire representation, independent of amos.h.
#pragma once
#include "core/endian.h"
namespace amos::host {
struct MarkRegistry;
class alignas(uint32_t) MarkHandle {
    uint32_t word_;
    explicit constexpr MarkHandle(uint32_t word) : word_(word) {}
    friend struct MarkRegistry;
    friend constexpr MarkHandle decode_mark(uint32_t);
    friend constexpr uint32_t encode_mark(MarkHandle);
public:
    constexpr MarkHandle() = default;
    explicit constexpr operator bool() const { return word_ != 0; }
    friend constexpr bool operator==(MarkHandle, MarkHandle) = default;
};
constexpr MarkHandle decode_mark(uint32_t word) { return MarkHandle(word); }
constexpr uint32_t encode_mark(MarkHandle handle) { return handle.word_; }
}
namespace amos::wire {
class MarkField {
    be_u32 word_;
public:
    host::MarkHandle load() const { return host::decode_mark(uint32_t(word_)); }
    void store(host::MarkHandle handle) { word_ = host::encode_mark(handle); }
};
static_assert(sizeof(MarkField) == 4 && alignof(MarkField) == alignof(be_u32));
}
