// String helpers and printing of AMOSPro.Lib (+Lib.s): ChVerBuf(2),
// Str2Chaine, Mes2Chaine, MCSuit, Set_HiChaine, Ret_ChVide, A0ToChaine,
// FinBin; ImpChaine (Print's output: the window, or the printer) and the
// printer (PRT_Open, PRT_Close, PRT_Print). Register routines: their
// register outputs are those of the original.

#include "core/amiga.h"
#include "lib/dos.h"
#include "lib/strings.h"

#define DOS_BASE_NAME (reinterpret_cast<struct DosLibrary *>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;

// textout_glue.s: the routine record that closes the printer at ClearVar
extern "C" uint8_t lib_prt_close_record[];

namespace {

constexpr uint16_t EcEdit = 9;      // the editor's screen

// Set_HiChaine: HiChaine past string s; s the result (d2 = 2, d3 = s)
uint32_t set_hi(RegFrame &r, uint32_t s)
{
    uint32_t length = (peek_w(ptr<void>(s)) + 1u) & 0xFFFE;
    r.a[0] = s + 2 + length;
    idata().HiChaine = r.a[0];
    r.d[0] = length;
    r.d[2] = 2;
    r.d[3] = s;
    return s;
}

// MCSuit: a new string of the d3 characters at a2
uint32_t mcsuit(RegFrame &r)
{
    uint32_t length = r.d[3] & 0xFFFF;
    uint8_t *s = str_alloc(r.d[3]);
    poke_w(s, uint16_t(length));
    uint8_t *to = s + 2;
    const uint8_t *from = ptr<uint8_t>(r.a[2]);
    uint16_t n = uint16_t(length);
    do
        *to++ = *from++;
    while (--n);
    r.a[1] = address(s);
    r.a[2] = address(from);
    return set_hi(r, address(s));
}

uint32_t empty(RegFrame &r)
{
    r.d[3] = idata().ChVide;
    r.d[2] = 2;
    return r.d[3];
}

void prt_close()
{
    IData &d = idata();
    if (BPTR h = BPTR(d.PrtHandle)) {
        d.PrtHandle = 0;
        Close(h);
    }
}

uint32_t prt_open()
{
    IData &d = idata();
    if (d.PrtHandle)
        return d.PrtHandle;
    Regs m;
    m.d[0] = 43;
    m.a[0] = address(boot::sys_message(m.d[0]));
    d.PrtHandle = uint32_t(Open(ptr<char>(m.a[0]), 1005));
    if (!d.PrtHandle)
        disk_error();
    Regs r;
    r.a[1] = address(lib_prt_close_record);
    r.a[2] = address(&d.Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
    return d.PrtHandle;
}

// PRT_Print: text to the printer, CR LF made LF (unless PI_PrtRet)
void prt_print(char *text)
{
    IData &d = idata();
    BPTR h = BPTR(prt_open());
    char *from = text, *to = text;
    for (;;) {
        char c = *from++;
        *to++ = c;
        if (!c)
            break;
        if (c == 13 && !d.PI_PrtRet && *from == 10)
            to[-1] = *from++;
    }
    int32_t n = from - text - 1;
    if (n && Write(h, text, n) < 0)
        disk_error();
}

} // namespace

extern "C" {

// ChVerBuf2: the d2 characters at a2 (at most 510) zero-terminated at a0
uint32_t lib_ch_ver_buf2(RegFrame &r)
{
    uint8_t *to = ptr<uint8_t>(r.a[0]);
    const uint8_t *from = ptr<uint8_t>(r.a[2]);
    uint16_t n = uint16_t(r.d[2]);
    if (n) {
        if (n > 510)
            n = 510;
        for (uint16_t i = 0; i < n; i++)
            *to++ = *from++;
        r.d[0] = (r.d[0] & 0xFFFF0000) | 0xFFFF;
    } else {
        r.d[0] &= 0xFFFF0000;
    }
    *to++ = 0;
    r.a[0] = address(to);
    r.a[1] = address(from);
    return 0;
}

// ChVerBuf: the same into Buffer
uint32_t lib_ch_ver_buf(RegFrame &r)
{
    r.a[0] = idata().Buffer;
    return lib_ch_ver_buf2(r);
}

// Str2Chaine: the zero-terminated text at a2
uint32_t lib_str2_chaine(RegFrame &r)
{
    const char *s = ptr<char>(r.a[2]);
    uint32_t n = 0;
    while (s[n])
        n++;
    r.d[3] = n;
    return n ? mcsuit(r) : empty(r);
}

// Mes2Chaine: the message at a2, its length before it (0 or 255: empty)
uint32_t lib_mes2_chaine(RegFrame &r)
{
    uint8_t n = ptr<uint8_t>(r.a[2])[-1];
    r.d[3] = n;
    return n && n != 0xFF ? mcsuit(r) : empty(r);
}

uint32_t lib_mc_suit(RegFrame &r) { return mcsuit(r); }
uint32_t lib_set_hi_chaine(RegFrame &r) { return set_hi(r, r.d[3]); }
uint32_t lib_ret_ch_vide(RegFrame &r) { return empty(r); }

// A0ToChaine: the zero-terminated text at a0 as a string (a0); without the
// restart of DDemande
uint32_t lib_a0_to_chaine(RegFrame &r)
{
    const uint8_t *from = ptr<uint8_t>(r.a[0]);
    uint32_t n = 0;
    while (from[n])
        n++;
    Regs a;
    a.d[3] = n;
    lib_call(L_DDemande, a);
    uint8_t *s = ptr<uint8_t>(a.a[1]);
    poke_w(s, uint16_t(n));
    uint8_t *to = s + 2;
    for (uint32_t i = 0; i <= (n & 0xFFFF); i++)
        *to++ = *from++;
    idata().HiChaine = (address(to) + 1) & ~1u;
    r.d[3] = n;
    r.a[0] = address(s);
    r.a[1] = address(to);
    r.a[2] = address(from);
    return 0;
}

// FinBin: the string built at a1, its end a0
uint32_t lib_fin_bin(RegFrame &r)
{
    r.d[3] = str_finish(ptr<uint8_t>(r.a[1]), ptr<uint8_t>(r.a[0]));
    r.d[2] = 2;
    r.a[0] = r.a[0] - r.a[1] - 2;
    return r.d[3];
}

// ImpChaine: Print's text a0 to the printer, or the current window (in
// direct mode with the output to the editor, its screen's)
uint32_t lib_imp_chaine(RegFrame &r)
{
    IData &d = idata();
    if (d.ImpFlg) {
        prt_print(ptr<char>(r.a[0]));
        return 0;
    }
    Regs p;
    p.a[1] = r.a[0];
    if (d.Direct && d.Esc_Output) {
        Regs a;
        a.d[1] = EcEdit;
        ec_call(ec::Active, a);
        wi_call(wi::Print, p);
        if (d.ScOn) {
            Regs b;
            b.d[1] = d.ScOn - 1u;
            ec_call(ec::Active, b);
        }
        return 0;
    }
    if (!d.ScOn)
        screen_error(3);
    wi_call(wi::Print, p);
    return 0;
}

uint32_t lib_prt_open(RegFrame &r) { return r.d[0] = r.d[1] = prt_open(); }
uint32_t lib_prt_close(RegFrame &)
{
    prt_close();
    return 0;
}
uint32_t lib_prt_print(RegFrame &r)
{
    prt_print(ptr<char>(r.a[0]));
    return 0;
}
void lib_prt_close_fn() { prt_close(); }
}
