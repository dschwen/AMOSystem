// Exercise the host dispatcher's word-sized call shape across register and
// stack arguments. These signatures match; mixed-signature adapters remain
// a separate requirement for the real glue entries.
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <utility>

namespace {
template <size_t> using Word = uintptr_t;
constexpr uint64_t Result = 0x1234567800000000ull;

template <size_t... I>
__attribute__((noinline)) uint64_t native(Word<I>... args)
{
    return (Result + ... + uint64_t(args));
}

template <size_t... I> bool arity(std::index_sequence<I...>)
{
    using Fn = uint64_t (*)(Word<I>...);
    void *volatile stored = reinterpret_cast<void *>(&native<I...>);
    auto fn = reinterpret_cast<Fn>(stored);
    uint64_t got = fn(Word<I>(I + 1)...);
    return got == Result + sizeof...(I) * (sizeof...(I) + 1) / 2;
}

template <size_t... N> int all(std::index_sequence<N...>)
{
    int failed = 0;
    auto test = [&](size_t n, bool ok) {
        printf("%s native word dispatch, %zu parameters, 64-bit result\n", ok ? "PASS" : "FAIL", n);
        if (!ok)
            failed++;
    };
    (test(N, arity(std::make_index_sequence<N>{})), ...);
    return failed;
}
}

int native_probe() { return all(std::make_index_sequence<11>{}); }
