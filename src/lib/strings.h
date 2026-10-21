// AMOS strings: a length word followed by the characters, kept at even
// addresses in the string space between the variables and TabBas. New
// strings are built at HiChaine.
#pragma once

#include "core/amos.h"

namespace amos {

inline uint16_t str_len(const uint8_t *s) { return peek_w(s); }
inline const uint8_t *str_chars(const uint8_t *s) { return s + 2; }

// The empty string (L_Ret_ChVide)
inline uint32_t str_empty() { return idata().ChVide; }

// L_Demande: room for a string of len characters at HiChaine. Returns
// where its length word goes. When the string space is full this collects
// garbage and restarts the current instruction, or raises "out of string
// space": allocate before any side effect.
inline uint8_t *str_alloc(uint32_t len)
{
    Regs r = {};
    r.d[3] = len;
    lib_call(L_Demande, r);
    return ptr<uint8_t>(r.a[0]);
}

// L_FinBin: ends the string s built at HiChaine just before end. Sets its
// length, moves HiChaine past it and returns its address.
inline uint32_t str_finish(uint8_t *s, uint8_t *end)
{
    idata().HiChaine = (address(end) + 1) & ~1u;
    poke_w(s, static_cast<uint16_t>(end - s - 2));
    return address(s);
}

// L_MCSuit: a new string holding len characters from data (len > 0)
inline uint32_t str_from(const void *data, uint16_t len)
{
    uint8_t *s = str_alloc(len);
    poke_w(s, len);
    const uint8_t *from = static_cast<const uint8_t *>(data);
    uint8_t *to = s + 2;
    for (uint16_t i = 0; i < len; i++)
        *to++ = *from++;
    idata().HiChaine = (address(to) + 1) & ~1u;
    return address(s);
}

// L_Str2Chaine: a new string from a zero-terminated one
inline uint32_t str_from_cstr(const char *text)
{
    uint32_t len = 0;
    while (text[len])
        len++;
    return len ? str_from(text, static_cast<uint16_t>(len)) : str_empty();
}

// L_ChVerBuf: copy of string s into Buffer, zero-terminated, at most 510
// characters. Returns Buffer.
inline char *str_to_buffer(const uint8_t *s)
{
    char *buffer = ptr<char>(idata().Buffer);
    uint16_t len = str_len(s);
    if (len > 510)
        len = 510;
    const uint8_t *from = str_chars(s);
    char *to = buffer;
    for (uint16_t i = 0; i < len; i++)
        *to++ = *from++;
    *to = 0;
    return buffer;
}

} // namespace amos
