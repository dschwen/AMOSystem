// Walking the lines of a program in memory: see core/source.h

#include "core/source.h"
#include "core/amos.h"

namespace amos::source {

namespace {

bool is_procedure(const uint8_t *line) { return peek_w(line + 2) == tk::Proc; }
bool closed(const uint8_t *line) { return int16_t(peek_w(line + 10)) < 0; }
uint8_t *after_closed(uint8_t *line) { return line + 12 + 2 + peek_l(line + 4); }

} // namespace

Found find_line(uint8_t *a0, uint16_t n)
{
    uint8_t *a1 = nullptr;
    for (; n; n--) {
        uint8_t length = a0[0];
        if (!length)
            break;
        if (!is_procedure(a0)) {
            a0 += 2 * length;
            continue;
        }
        if (closed(a0)) {
            a0 = after_closed(a0);
            continue;
        }
        // An open procedure: the lines up to End Proc
        a1 = a0;
        a0 += 2 * length;
        for (;;) {
            if (!--n)
                goto found;
            length = a0[0];
            if (!length)
                goto found;
            if (peek_w(a0 + 2) == tk::EndP)
                break;
            a0 += 2 * length;
        }
        a1 = nullptr;
        a0 += 2 * length;
    }
found:
    if (peek_w(a0) && is_procedure(a0))
        a1 = a0;
    return {a0, a1};
}

uint32_t line_size(const uint8_t *line)
{
    uint32_t length = line[0];
    if (length && is_procedure(line) && closed(line))
        return 12 + 2 + peek_l(line + 4);
    return length * 2;
}

bool editable(const uint8_t *line) { return !line[0] || !is_procedure(line) || !(line[10] & 0x80); }

Number find_address(uint8_t *a2, const uint8_t *a0)
{
    uint32_t d0 = 0xFFFFFFFF;
    uint8_t *d1 = nullptr, *a1 = a2;
    uint16_t d2;
FdA1:
    d0++;
    a1 = a2;
    d2 = a2[0];
    if (!d2)
        goto FdAT;
    if (is_procedure(a2) && closed(a2)) {
        d1 = a2;
        if (a2[10] & 1 << 4) {
            // compiled: its contents skipped
            a2 = after_closed(a2);
            d2 = 0;
        }
        goto FdA6;
    }
FdA2:
    a2 += 2 * d2;
    if (a2 <= a0)
        goto FdA1;
    goto FdAT;
FdA5:
    a1 = a2;
    d2 = a2[0];
    if (!d2)
        goto FdAT;
    if (peek_w(a2 + 2) == tk::EndP) {
        d1 = nullptr;
        goto FdA2;
    }
FdA6:
    a2 += 2 * d2;
    if (a2 <= a0)
        goto FdA5;
FdAT:
    return {d0, a1, d1};
}

// TInst: past the data of token t at a (0: the end of the line)
const uint8_t *skip_token(uint16_t t, const uint8_t *a)
{
    if (!t)
        return a;
    if (t <= tk::LGo)
        return a + 4 + a[2];
    if (t == tk::Ch1 || t == tk::Ch2 || t == tk::Rem1 || t == tk::Rem2) {
        a += 2 + int16_t(peek_w(a));
        if (address(a) & 1)
            a++;
        return a;
    }
    if (t == tk::DFl)
        return a + 8;
    if (t <= tk::Fl || t == tk::Ext)
        return a + 4;
    if (t == tk::For || t == tk::Rpt || t == tk::Whl || t == tk::Do)
        return a + 2;
    if (t == tk::Exit || t == tk::ExIf)
        return a + 4;
    if (t == tk::If || t == tk::Else || t == tk::ElsI || t == tk::Data)
        return a + 2;
    if (t == tk::Proc)
        return a + 8;
    if (t == tk::On)
        return a + 4;
    if (t >= tk::Equ && t <= tk::StruS)
        return a + 6;
    return a;
}

} // namespace amos::source
