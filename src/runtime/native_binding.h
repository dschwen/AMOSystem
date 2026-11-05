// Native callable metadata. Never stored in guest memory or as a guest address.
#pragma once
#include "runtime/library_call.h"
#include "core/amos.h"
namespace amos::interp { struct EvalOut; }
namespace amos::host {
#if !defined(__mc68000__)
#pragma pack(push,8)
union alignas(void*) NativeBinding {
    LibraryCall library;
    uint32_t (*reg)(RegFrame&);
    void (*call)();
    void (*eval)(interp::EvalOut*);
    void (*float_call)(uint32_t*);
    void (*w)(AllRegs*);
    uint32_t (*w_result)(AllRegs*);
    void (*hand)(AllRegs&);
    constexpr NativeBinding(decltype(nullptr)) : call(nullptr) {}
    static constexpr NativeBinding from_library(LibraryCall fn) { return NativeBinding(fn); }
    static constexpr NativeBinding from_reg(uint32_t (*fn)(RegFrame&)) { return NativeBinding(fn); }
    static constexpr NativeBinding from_call(void (*fn)()) { return NativeBinding(fn); }
    static constexpr NativeBinding from_eval(void (*fn)(interp::EvalOut*)) { return NativeBinding(fn); }
    static constexpr NativeBinding from_float(void (*fn)(uint32_t*)) { return NativeBinding(fn); }
    static constexpr NativeBinding from_w(void (*fn)(AllRegs*)) { return NativeBinding(fn); }
    static constexpr NativeBinding from_w_result(uint32_t (*fn)(AllRegs*)) { return NativeBinding(fn); }
    static constexpr NativeBinding from_hand(void (*fn)(AllRegs&)) { return NativeBinding(fn); }
private:
    constexpr NativeBinding(LibraryCall fn) : library(fn) {}
    constexpr NativeBinding(uint32_t (*fn)(RegFrame&)) : reg(fn) {}
    constexpr NativeBinding(void (*fn)()) : call(fn) {}
    constexpr NativeBinding(void (*fn)(interp::EvalOut*)) : eval(fn) {}
    constexpr NativeBinding(void (*fn)(uint32_t*)) : float_call(fn) {}
    constexpr NativeBinding(void (*fn)(AllRegs*)) : w(fn) {}
    constexpr NativeBinding(uint32_t (*fn)(AllRegs*)) : w_result(fn) {}
    constexpr NativeBinding(void (*fn)(AllRegs&)) : hand(fn) {}
};
#pragma pack(pop)
#endif
}
#if defined(__mc68000__)
#define AMOS_LIBRARY_METADATA(at, name) ::amos::host::put(at, 4, (uint32_t(uintptr_t(&name))))
#define AMOS_BIND_LIBRARY(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_REG(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_CALL(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_EVAL(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_FLOAT(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_W(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_W_RESULT(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_HAND(name) reinterpret_cast<const void *>(&name)
#define AMOS_BIND_UNSUPPORTED(name) reinterpret_cast<const void *>(&name)
#else
#define AMOS_LIBRARY_METADATA(at, name) ::amos::host::put(at, 4, 0)
#define AMOS_BIND_LIBRARY(name) ::amos::host::NativeBinding::from_library(amos_typed_##name)
#define AMOS_BIND_REG(name) ::amos::host::NativeBinding::from_reg(amos_typed_##name)
#define AMOS_BIND_CALL(name) ::amos::host::NativeBinding::from_call(amos_typed_##name)
#define AMOS_BIND_EVAL(name) ::amos::host::NativeBinding::from_eval(amos_typed_##name)
#define AMOS_BIND_FLOAT(name) ::amos::host::NativeBinding::from_float(amos_typed_##name)
#define AMOS_BIND_W(name) ::amos::host::NativeBinding::from_w(amos_typed_##name)
#define AMOS_BIND_W_RESULT(name) ::amos::host::NativeBinding::from_w_result(amos_typed_##name)
#define AMOS_BIND_HAND(name) ::amos::host::NativeBinding::from_hand(amos_typed_##name)
#define AMOS_BIND_UNSUPPORTED(name) nullptr
#endif
