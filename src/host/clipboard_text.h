// Pure text conversion at the host boundary; no guest pointers or SDL calls.
#pragma once
#include "host/cxx.h"
#include <stdint.h>

namespace amos::host::clipboard {
constexpr size_t Limit = 1024 * 1024;
inline bool decode(const char *utf8, std::string &out)
{
    out.clear();
    for (size_t i = 0; utf8[i];) {
        if (i >= Limit) return false;
        unsigned c = static_cast<uint8_t>(utf8[i++]);
        if (c >= 0xC2 && c <= 0xC3) {
            unsigned next = static_cast<uint8_t>(utf8[i]);
            if (next < 0x80 || next > 0xBF) return false;
            ++i;
            c = ((c & 31) << 6) | (next & 63);
        } else if (c >= 128) return false;
        if (c == '\r') {
            if (utf8[i] == '\n') ++i;
            c = '\n';
        }
        if (c == '\t') c = ' '; // Same policy as ASCII merge: one space.
        if ((c < 32 && c != '\n') || (c >= 127 && c < 160)) return false;
        out += static_cast<char>(c);
    }
    return true;
}
inline bool encode(const std::string &text, std::string &out)
{
    out.clear();
    for (uint8_t c : text) {
        if ((c < 32 && c != '\n' && c != '\t') || (c >= 127 && c < 160)) return false;
        if (c >= 128) {
            out += static_cast<char>(0xC0 | (c >> 6));
            out += static_cast<char>(0x80 | (c & 63));
        } else out += static_cast<char>(c);
        if (out.size() > Limit) return false;
    }
    return true;
}
inline bool command_line(std::string &text)
{
    if (!text.empty() && text.back() == '\n') text.pop_back();
    return text.find('\n') == std::string::npos;
}
}
