// The editor's keys (+Edit.s Ed_Key, Ed_Ky2Fonc, Ed_Fonc2Ky): a key from
// the keyboard or a macro being played, recorded into a macro, then either
// a function of the editor or a character typed.
//
// A key: shifts << 24 | scan code << 16 | ASCII (as Inkey gives it).
// Ed_KFonc, the keys of the functions: for each function from 1, pairs of
// bytes (the ASCII code in upper case, or $80 + the scan code; the
// shifts), then 0; $FF at the end.
// A macro: a list element (next, ..., the key long at 8), its keys from
// 12, three bytes each (ASCII, scan code, shifts), ending with -1.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_function_call[], ed_sam_play[];

namespace {

constexpr uint16_t BitControl = 1 << 8;
constexpr uint8_t Shf = 0x03, Ctr = 0x08, Alt = 0x30, Ami = 0xC0;

// EdMa_Adr: the macro of key, or 0
uint8_t *macro_of(uint32_t key)
{
    for (uint32_t m = idata().EdMa_List; m; m = peek_l(ptr<uint8_t>(m)))
        if (peek_l(ptr<uint8_t>(m) + 8) == key)
            return ptr<uint8_t>(m);
    return nullptr;
}

} // namespace

namespace amos::ed {

// Ed_Ky2Fonc: the function of a key (from 0), -1 if none. The table's
// shifts must all be there, but either shift (Control, Alt, Amiga) of a
// pair does.
int32_t key_function(uint32_t key)
{
    uint8_t ascii = uint8_t(key);
    if (ascii >= 'a' && ascii <= 'z')
        ascii -= 'a' - 'A';
    uint8_t scan = uint8_t(key >> 16);
    uint8_t shifts = uint8_t(key >> 24) & 0xFB;         // not Caps Lock
    int32_t function = 0;
    for (const uint8_t *a0 = idata().Ed_KFonc;; a0 += 2) {
        uint8_t k = a0[0];
        while (!k) {
            function++;
            k = *++a0;
        }
        if (k == 0xFF)
            return -1;
        if (k & 0x80 ? (k & 0x7F) != scan : k != ascii)
            continue;
        uint8_t want = a0[1], have = shifts;
        if (!want) {
            if (have)
                continue;
            return function;
        }
        static const uint8_t groups[] = {Shf, Ctr, Alt};
        for (uint8_t group : groups) {
            if (!(have & group))
                continue;
            if (!(have & group & want))
                goto next;
            want &= ~group;
            have &= ~group;
            if (!have)
                goto found;
        }
        if (!(have & Ami & want))
            continue;
        want &= ~Ami;
        have &= ~Ami;
        if (have)
            continue;
    found:
        if (have | want)
            continue;
        return function;
    next:;
    }
}

// Ed_Fonc2Ky: the keys of function n (counted from 1 here), 0 if none
const uint8_t *function_keys(uint8_t n)
{
    const uint8_t *a0 = idata().Ed_KFonc;
    while (--n) {
        while (*a0)
            a0 += 2;
        a0++;
        if (*a0 == 0xFF)
            return nullptr;
    }
    return a0;
}

} // namespace amos::ed

namespace {

void sound(uint8_t c)
{
    Regs r;
    r.d[0] = c;
    call(ed_sam_play, r);
}

// Ed_Key: the next key acted on; false if there was none
bool key()
{
    IData &d = idata();
    uint32_t key;
    for (;;) {
        // a macro being played
        if (uint8_t *p = ptr<uint8_t>(d.EdMa_Play)) {
            p += 3;
            d.EdMa_Play = address(p);
            key = uint32_t(p[-1]) << 24 | uint32_t(p[-2]) << 16 | p[-3];
            if (uint8_t(key) != 0xFF)
                break;
            d.EdMa_Play = 0;
        }
        uint16_t &actualise = wdata().Actualise;
        if (actualise & BitControl) {
            actualise &= ~BitControl;
            key = 0x08330043;           // Control-C
        } else {
            Regs r;
            sy_call(sy::Inkey, r);
            key = r.d[1];
        }
        if (!key)
            return false;
        if (uint16_t tape = d.EdMa_Tape) {
            // a macro being recorded
            uint8_t *at = ptr<uint8_t>(peek_l(&d.EdMa_List)) + 8 + 4 - 1 + tape;
            if (*at != 0xFF) {
                at[0] = uint8_t(key);
                at[1] = uint8_t(key >> 16);
                at[2] = uint8_t(key >> 24);
                d.EdMa_Tape += 3;
            }
            break;
        }
        uint8_t *macro = macro_of(key);
        if (!macro)
            break;
        d.EdMa_Play = address(macro + 12);
    }
    int32_t function = key_function(key);
    if (function >= 0) {
        Regs r;
        r.d[2] = uint32_t(function);
        call(ed_function_call, r);
        return true;
    }
    type_key(uint8_t(key), d.Ed_Insert, nullptr);
    sound('C');
    return true;
}

} // namespace

extern "C" {
// Ed_Key: d0 -1 if a key was acted on
void ed_key(AllRegs *r)
{
    if (key())
        r->d[0] = 0xFFFFFFFF;
    else
        r->d[0] = 0;
}

// Ed_Ky2Fonc: d1 the key; d2 its function and d0 -1, or d0 0 if none
void ed_key_function(AllRegs *r)
{
    int32_t f = key_function(r->d[1]);
    if (f >= 0) {
        r->d[2] = (r->d[2] & 0xFFFF0000) | uint16_t(f);
        r->d[0] = 0xFFFFFFFF;
    } else {
        r->d[0] = 0;
    }
}

// Ed_Fonc2Ky: d0 the function; d0 the address of its keys, 0 if none
void ed_function_keys(AllRegs *r)
{
    const uint8_t *keys = function_keys(uint8_t(r->d[0]));
    r->d[0] = address(keys);
    if (keys)
        r->a[0] = address(keys);
}
}
