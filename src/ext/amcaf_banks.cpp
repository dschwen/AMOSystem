// AMCAF: the bank commands (R019-R03b, R172): the flags, memory kind, size and name of a bank, Bank Code (a
// byte or word of a range changed by a key), Bank Delta, Bank Checksum, Bank Copy. A bank is a number (or its
// address), a range two addresses; the Code, Delta and Checksum of a range do not touch what is after it
// (the library's Delta runs on to half as far again).

#include "ext/amcaf.h"
#include "ext/third_party.h"
#include "lib/banks.h"
#include "lib/strings.h"

using namespace amos;
using namespace amos::amcaf;
using ext3::Id;

namespace {

uint8_t get(uint32_t at) { return host::detail::read_cursor_byte(decode_context_cursor(at)); }
void put(uint32_t at, uint8_t v) { host::detail::write_cursor_at(decode_context_cursor(at), v); }
uint16_t get16(uint32_t at) { return uint16_t(get(at)) << 8 | get(at + 1); }
void put16(uint32_t at, uint16_t v)
{
    put(at, uint8_t(v >> 8));
    put(at + 1, uint8_t(v));
}

struct Range {
    uint32_t start, end;
};

// R172: a bank given as a number or an address: its data (not an icon or sprite bank)
Range bank_range(int32_t bank)
{
    uint32_t at = bank_or_address(bank);
    ContextCursor b = decode_context_cursor(at);
    if (bank_flags(b) & (BankBobs | BankIcons))
        ext3::error(Id::amcaf, 4);
    return {at, at + bank_size(b)};
}

// The bytes of a range, in order; a range that is empty or backwards would run the library's loops for ever
Range checked(Range r)
{
    if (r.end <= r.start)
        fon_call();
    return r;
}

uint8_t rotate_left8(uint8_t v, unsigned n) { n &= 7; return uint8_t(v << n | v >> ((8 - n) & 7)); }
uint8_t rotate_right8(uint8_t v, unsigned n) { n &= 7; return uint8_t(v >> n | v << ((8 - n) & 7)); }
uint16_t rotate_left16(uint16_t v, unsigned n) { n &= 15; return uint16_t(v << n | v >> ((16 - n) & 15)); }
uint16_t rotate_right16(uint16_t v, unsigned n) { n &= 15; return uint16_t(v >> n | v << ((16 - n) & 15)); }

enum class Op { Xor, Add, Mix, Rol, Ror };

void code_bytes(Op op, int32_t value, Range r)
{
    r = checked(r);
    uint8_t v = uint8_t(value);
    uint8_t key = v, step = uint8_t(v ^ 0xAA);
    for (uint32_t at = r.start; at < r.end; at++) {
        uint8_t b = get(at);
        switch (op) {
        case Op::Xor: b ^= v; break;
        case Op::Add: b = uint8_t(b + v); break;
        case Op::Mix:
            key = uint8_t(key + step);
            b ^= key;
            break;
        case Op::Rol: b = rotate_left8(b, uint32_t(value) & 63); break;
        case Op::Ror: b = rotate_right8(b, uint32_t(value) & 63); break;
        }
        put(at, b);
    }
}

void code_words(Op op, int32_t value, Range r)
{
    if (r.end <= r.start || (r.end - r.start) < 2)
        fon_call();
    uint16_t v = uint16_t(value);
    uint16_t key = v, step = uint16_t(v ^ 0xFACE);
    for (uint32_t at = r.start; at + 1 < r.end; at += 2) {
        uint16_t w = get16(at);
        switch (op) {
        case Op::Xor: w ^= v; break;
        case Op::Add: w = uint16_t(w + v); break;
        case Op::Mix:
            key = uint16_t(key + step);
            w ^= key;
            break;
        case Op::Rol: w = rotate_left16(w, uint32_t(value) & 63); break;
        case Op::Ror: w = rotate_right16(w, uint32_t(value) & 63); break;
        }
        put16(at, w);
    }
}

// The bank at `at` made again with another memory kind or size (R01b, R01c, R035): the old one goes, the new
// one has its number. A temporary bank (65535) holds it meanwhile, as in the library
void remake(int32_t number, uint16_t new_flags, uint32_t new_size, uint32_t copy_size)
{
    uint32_t at = bank_or_address(number);
    ContextCursor old = decode_context_cursor(at);
    char name[8];
    for (int i = 0; i < 8; i++)
        name[i] = char(host::detail::read_cursor_byte(old - 8 + i));
    ContextCursor made = bank_reserve(0xFFFF, new_flags, new_size, name);
    if (!made)
        error(24);
    for (uint32_t i = 0; i < copy_size; i++)
        host::detail::write_cursor_at(made + i, host::detail::read_cursor_byte(old + i));
    bank_erase(uint32_t(number));
    poke_l(made - 16, uint32_t(number));
}

}  // namespace

extern "C" {

#define CODE(name, op, kind, size)                                                                  \
    void amcaf_bank_code_##name##_2(int32_t value, int32_t bank) { code_##kind(Op::op, value, bank_range(bank)); } \
    void amcaf_bank_code_##name##_3(int32_t value, int32_t start, int32_t end)                      \
    {                                                                                               \
        code_##kind(Op::op, value, {uint32_t(start), uint32_t(end)});                               \
    }

// BANK CODE XOR.B / ADD.B / MIX.B / ROL.B / ROR.B / XOR.W ... value,bank  or  value,start TO end
CODE(xor_b, Xor, bytes, 1)
CODE(add_b, Add, bytes, 1)
CODE(mix_b, Mix, bytes, 1)
CODE(rol_b, Rol, bytes, 1)
CODE(ror_b, Ror, bytes, 1)
CODE(xor_w, Xor, words, 2)
CODE(add_w, Add, words, 2)
CODE(mix_w, Mix, words, 2)
CODE(rol_w, Rol, words, 2)
CODE(ror_w, Ror, words, 2)

// BANK DELTA ENCODE / DECODE bank  or  start TO end (R01d-R020): each byte the difference from the one before
// it, or back
static void delta(Range r, bool encode)
{
    r = checked(r);
    if (encode) {
        uint8_t previous = get(r.start);
        for (uint32_t at = r.start + 1; at < r.end; at++) {
            uint8_t b = get(at);
            put(at, uint8_t(b - previous));
            previous = b;
        }
    } else {
        uint8_t sum = get(r.start);
        for (uint32_t at = r.start + 1; at < r.end; at++) {
            sum = uint8_t(sum + get(at));
            put(at, sum);
        }
    }
}
void amcaf_bank_delta_encode_1(int32_t bank) { delta(bank_range(bank), true); }
void amcaf_bank_delta_encode_2(int32_t start, int32_t end) { delta({uint32_t(start), uint32_t(end)}, true); }
void amcaf_bank_delta_decode_1(int32_t bank) { delta(bank_range(bank), false); }
void amcaf_bank_delta_decode_2(int32_t start, int32_t end) { delta({uint32_t(start), uint32_t(end)}, false); }

// =BANK CHECKSUM(bank), =BANK CHECKSUM(start TO end) (R036, R037): the sum of the longs, with a key
static int32_t checksum(Range r)
{
    if (r.end <= r.start || (r.end - r.start) < 4)
        fon_call();
    uint32_t sum = 0;
    for (uint32_t at = r.start; at + 3 < r.end; at += 4)
        sum += uint32_t(get16(at)) << 16 | get16(at + 2);
    return int32_t(sum ^ 0xFACEFACEu);
}
int32_t amcaf_bank_checksum_1(int32_t bank) { return checksum(bank_range(bank)); }
int32_t amcaf_bank_checksum_2(int32_t start, int32_t end) { return checksum({uint32_t(start), uint32_t(end)}); }

// BANK TO CHIP bank, BANK TO FAST bank (R01b, R01c): the bank in the other kind of memory (not icons or sprites)
void amcaf_bank_to_chip(int32_t bank)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    uint16_t flags = bank_flags(b);
    if (flags & (BankBobs | BankIcons))
        ext3::error(Id::amcaf, 4);
    if (flags & BankChip)
        return;
    uint32_t size = bank_size(b);
    remake(bank, uint16_t(flags | BankChip), size, size);
}
void amcaf_bank_to_fast(int32_t bank)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    uint16_t flags = bank_flags(b);
    if (flags & (BankBobs | BankIcons))
        ext3::error(Id::amcaf, 4);
    if (!(flags & BankChip))
        return;
    uint32_t size = bank_size(b);
    remake(bank, uint16_t(flags & ~BankChip), size, size);
}

// BANK STRETCH bank TO size (R035): the bank made that long (what fits is kept)
void amcaf_bank_stretch(int32_t bank, int32_t size)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    uint16_t flags = bank_flags(b);
    if (flags & (BankBobs | BankIcons))
        ext3::error(Id::amcaf, 4);
    uint32_t old = bank_size(b);
    uint32_t kept = old <= uint32_t(size) ? old : uint32_t(size);
    // (words of what is kept, and a byte more if the old size was odd, as the library does)
    if (kept < 2)
        fon_call();                                 // (its loop would run for ever)
    remake(bank, flags, uint32_t(size), (kept >> 1) * 2 + (old & 1));
}

// BANK COPY bank TO number, BANK COPY start,end TO number (R039, R038): a new bank (a bank that is already
// one keeps its kind and name) with a copy of the bytes
static void copy_bank(uint32_t start, uint32_t end, int32_t number)
{
    uint16_t flags = 0;
    char name[8] = {'W', 'o', 'r', 'k', ' ', ' ', ' ', ' '};
    ContextCursor source = decode_context_cursor(start);
    uint16_t head = peek_w(source - 12);
    if (!(head & 0xFFF0)) {
        if (peek_l(source - 16) == uint32_t(number))
            fon_call();
        flags = head;
        for (int i = 0; i < 8; i++)
            name[i] = char(host::detail::read_cursor_byte(source - 8 + i));
    }
    uint32_t size = end - start;
    ContextCursor made = bank_reserve(uint32_t(number), flags, size, name);
    if (!made)
        error(24);
    for (uint32_t i = 0; i < size; i++)
        host::detail::write_cursor_at(made + i, host::detail::read_cursor_byte(source + i));
}
void amcaf_bank_copy_2(int32_t bank, int32_t number)
{
    Range r = bank_range(bank);
    copy_bank(r.start, r.end, number);
}
void amcaf_bank_copy_3(int32_t start, int32_t end, int32_t number) { copy_bank(uint32_t(start), uint32_t(end), number); }

// BANK NAME bank,a$ (R03a): the name, up to 8 characters, padded with spaces
void amcaf_bank_name(int32_t bank, GuestString name)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    for (int i = 0; i < 8; i++)
        host::detail::write_cursor_at(b - 8 + i, ' ');
    uint16_t length = str_len(name);
    if (length > 8)
        length = 8;
    for (uint16_t i = 0; i < length; i++)
        host::detail::write_cursor_at(b - 8 + i, name.at(i));
}

// =BANK NAME$(bank) (R03b): the name, 8 characters
uint32_t amcaf_bank_name_s(int32_t bank)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    uint8_t text[8];
    for (int i = 0; i < 8; i++)
        text[i] = host::detail::read_cursor_byte(b - 8 + i);
    return str_from(text, 8);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_banks.inc"
#endif
