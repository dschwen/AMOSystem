// AMOS strings: a length word followed by the characters, kept at even
// addresses in the string space between the variables and TabBas. New
// strings are built at HiChaine.
#pragma once

#include "core/amos.h"
#include "runtime/detail/identity_data_memory.h"
#if !defined(__mc68000__)
#include "runtime/detail/legacy_call_codec.h"
#endif

namespace amos {

// A string in guest memory, held as a guest address (never a native pointer):
// reads go through the checked cursor accessors. On the Amiga it is one pointer,
// passed like one.
class GuestString {
    ContextCursor at_;

public:
    GuestString() = default;
    explicit GuestString(ContextCursor at) : at_(at) {}
    static GuestString from_word(uint32_t word) { return GuestString(decode_context_cursor(word)); }
    uint32_t word() const { return encode_context_cursor(at_); }
    ContextCursor cursor() const { return at_; }
    uint16_t size() const { return host::detail::read_cursor_word(at_); }
    ContextCursor chars() const { return at_ + 2; }
    uint8_t at(uint32_t i) const { return host::detail::read_cursor_byte(at_ + 2 + i); }
    // n characters from position i, to a host buffer
    void copy_to(void *to, uint32_t i, uint32_t n) const
    {
        uint8_t *out = static_cast<uint8_t *>(to);
        for (ContextCursor from = at_ + 2 + i; n; n--)
            *out++ = host::detail::read_cursor_byte(from++);
    }
};

inline uint16_t str_len(GuestString s) { return s.size(); }


// The empty string (L_Ret_ChVide)
inline uint32_t str_empty() { return idata().ChVide; }

// A string being built at HiChaine (L_Demande, L_FinBin): room for len
// characters, the characters put one after the other, then finish. When
// the string space is full the constructor collects garbage and restarts
// the current instruction, or raises "out of string space": build before
// any side effect.
class StringBuilder {
    ContextCursor start_;
    ContextCursor to_;

public:
    explicit StringBuilder(uint32_t len)
    {
        Regs r = {};
        r.d[3] = len;
        lib_call(L_Demande, r);
        start_ = decode_context_cursor(r.a[0]);
        to_ = start_ + 2;
    }
    void put(uint8_t c) { host::detail::write_cursor_byte(to_, c); }
    void put(const void *from, uint32_t n)
    {
        const uint8_t *in = static_cast<const uint8_t *>(from);
        for (; n; n--)
            put(*in++);
    }
    // n characters of s from position i
    void put(GuestString s, uint32_t i, uint32_t n)
    {
        for (ContextCursor from = s.chars() + i; n; n--)
            put(host::detail::read_cursor_byte(from++));
    }
    void put_word(uint16_t v)
    {
        host::detail::write_cursor_word(to_, v);
        to_ += 2;
    }
    void skip(uint32_t n) { to_ += n; }
    // the length word
    ContextCursor start() const { return start_; }
    // where the next character goes, as a buffer for a routine that writes characters
    ContextCursor position() const { return to_; }
    void set_position(ContextCursor at) { to_ = at; }
    // L_FinBin: the length is what was put; HiChaine moves past it (to an even address)
    uint32_t finish()
    {
        uint32_t end = encode_context_cursor(to_);
        uint32_t start = encode_context_cursor(start_);
        idata().HiChaine = (end + 1) & ~1u;
        poke_w(start_, static_cast<uint16_t>(end - start - 2));
        return start;
    }
    // With a length of its own and HiChaine exactly after what was written
    uint32_t finish_raw(uint16_t len)
    {
        poke_w(start_, len);
        idata().HiChaine = encode_context_cursor(to_);
        return encode_context_cursor(start_);
    }
};

// L_InstrFind for characters at cursors (the start counts from 1)
uint32_t instr_find_cursor(ContextCursor hay, uint32_t hay_len, ContextCursor needle, uint32_t needle_len,
                           uint32_t start);

// L_MCSuit: a new string holding len characters from data (len > 0)
inline uint32_t str_from(const void *data, uint16_t len)
{
    StringBuilder b(len);
    b.put(data, len);
    return b.finish();
}

// A new string from n characters (n > 0) of s from position i
inline uint32_t str_from(GuestString s, uint32_t i, uint16_t n)
{
    StringBuilder b(n);
    b.put(s, i, n);
    return b.finish();
}

// L_Str2Chaine: a new string from a zero-terminated one
inline uint32_t str_from_cstr(const char *text)
{
    uint32_t len = 0;
    while (text[len])
        len++;
    return len ? str_from(text, static_cast<uint16_t>(len)) : str_empty();
}

// The same for a GuestString: a view of Buffer for native code (file names,
// text for the host) - the one place the characters are handed over as a host pointer
inline char *str_to_buffer(GuestString s)
{
    char *buffer = ptr<char>(idata().Buffer);
    uint16_t len = s.size();
    if (len > 510)
        len = 510;
    s.copy_to(buffer, 0, len);
    buffer[len] = 0;
    return buffer;
}

} // namespace amos

#if !defined(__mc68000__)
namespace amos::host::detail {
template <> struct CallArgument<amos::GuestString> {
    static amos::GuestString decode(uint32_t word) { return amos::GuestString::from_word(word); }
};
}
#endif
