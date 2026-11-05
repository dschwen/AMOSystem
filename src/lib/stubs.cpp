// The small routines of AMOSPro.Lib (lib/stubs_glue.s): the errors, the
// constant functions, the registers kept for the errors, the bob and
// icon addresses, MajD0

#include "lib/banks.h"

using namespace amos;

namespace {

constexpr uint32_t EcEBase = 45, DEBase = EcEBase + 35 - 1, SpEBase = DEBase + 25;

// AdBErr: no bank (count 0), else no such image
[[noreturn]] void bank_image_error(uint16_t count) { error(count ? EcEBase + 30 - 1 : 36); }

// AdBob, AdIcon: d1 an image (its flags above). Out: a0 = a2 its entry,
// d1 the number of images, d5 (low word) that number too, d0 = 0
uint32_t image(RegFrame &r, uint32_t (*entry)(RegFrame &))
{
    r.d[1] &= 0x3FFF;
    if (!r.d[1])
        fon_call();
    r.d[0] = r.d[1];
    if (!entry(r))
        bank_image_error(uint16_t(r.d[1]));
    r.a[2] = r.a[0];
    r.d[5] = (r.d[5] & 0xFFFF0000) | uint16_t(r.d[1]);
    r.d[0] = 0;
    return 0;
}

} // namespace

extern "C" {

uint32_t lib_bnk_ad_bob(RegFrame &r);
uint32_t lib_bnk_ad_icon(RegFrame &r);

// GoError: error d0
uint32_t lib_go_error(RegFrame &r) { error(r.d[0]); }

// The errors of AMOSPro.Lib's routines (Err)
#define ERROR_ROUTINE(name, n) \
    uint32_t lib_err_##name(RegFrame &) { error(n); }
ERROR_ROUTINE(type_mis, 34)
ERROR_ROUTINE(fil_oo, DEBase + 17)
ERROR_ROUTINE(fil_no, DEBase + 18)
ERROR_ROUTINE(fil_tm, DEBase + 19)
ERROR_ROUTINE(eo_fil, DEBase + 21)
ERROR_ROUTINE(iff_for2, 30)
ERROR_ROUTINE(di_form, DEBase + 16)
ERROR_ROUTINE(bk_al_res, 35)
ERROR_ROUTINE(bk_no_res, 36)
ERROR_ROUTINE(oo_f_mem, 24)
ERROR_ROUTINE(stoo_long, 21)
ERROR_ROUTINE(fon_call, 23)
ERROR_ROUTINE(adr_err, 25)
ERROR_ROUTINE(cant_fit, 32)
ERROR_ROUTINE(iff_for, 30)
ERROR_ROUTINE(iff_cmp, 31)
ERROR_ROUTINE(fft_e, 37)
ERROR_ROUTINE(sp_err, SpEBase)
#undef ERROR_ROUTINE

// The errors of the screens (EcErr), through EcWiErr
#define SCREEN_ERROR(name, n) \
    uint32_t lib_err_##name(RegFrame &) { screen_error(n); }
SCREEN_ERROR(sc_n_op, 3)
SCREEN_ERROR(rain_err, 31)
SCREEN_ERROR(ill_sc_n, 6)
SCREEN_ERROR(b_fon_call, 22)
SCREEN_ERROR(w_fon_call, 16)
#undef SCREEN_ERROR

// EcWiErr: d0 an error of the screens (1: out of memory)
uint32_t lib_ec_wi_err(RegFrame &r) { screen_error(r.d[0]); }
// CopErr: d0 an error of the copper
uint32_t lib_cop_err(RegFrame &r) { error((r.d[0] & 0xFFFF0000) | uint16_t(r.d[0] + EcEBase + 32 - 2)); }
// AdBErr: d1 = 0 no bank, else no such image
uint32_t lib_ad_b_err(RegFrame &r) { bank_image_error(uint16_t(r.d[1])); }

// Syntax error, STOP, font not found, ERROR n
void lib_syntax() { error(22); }
void lib_in_stop() { error(9); }
void lib_fnf_e() { error(44); }
void lib_in_error(uint32_t n) { error(n < 256 ? n : 255); }

// The library's start: d0 its number (0: $FF), d1 its version
uint32_t lib_init(RegFrame &r)
{
    r.d[0] = (r.d[0] & 0xFFFF0000) | 0x00FF;
    r.d[1] = (r.d[1] & 0xFFFF0000) | 0x0200;
    return r.d[0];
}
// END; illegal direct mode (as through RunErr: errors of AMOSPro)
void lib_in_end() { error(10); }
uint32_t lib_r_ill_dir(RegFrame &) { error(17); }
// REM: the program after its text
void lib_in_rem()
{
    Ctx &c = ctx();
    int16_t skip = int16_t(peek_w(c.a6));
    c.a6 += 2 + skip;
}
// ELSE as a function (Lib_Def), the null operator (Lib_Par): EntNul; the
// null operator gives its token back
int32_t lib_fn_else() { return int32_t(0x80000000); }
int32_t lib_fn_null()
{
    ctx().a6 -= 2;
    return int32_t(0x80000000);
}

// =TRUE, =FALSE, the functions that are only a token
int32_t lib_fn_true() { return -1; }
int32_t lib_fn_one() { return 1; }
int32_t lib_fn_false() { return 0; }

// The registers of the program kept for the errors
uint32_t lib_save_regs(RegFrame &r)
{
    IData &d = idata();
    d.ErrorSave[0] = r.d[6];
    d.ErrorSave[1] = r.d[7];
    d.ErrorRegs = 1;
    return 0;
}
uint32_t lib_load_regs(RegFrame &r)
{
    IData &d = idata();
    r.d[6] = d.ErrorSave[0];
    r.d[7] = d.ErrorSave[1];
    d.ErrorRegs = 0;
    return 0;
}

// MajD0: the low byte of d0 in capitals
uint32_t lib_maj_d0(RegFrame &r)
{
    uint8_t c = uint8_t(r.d[0]);
    if (c >= 'a' && c <= 'z')
        r.d[0] = (r.d[0] & 0xFFFFFF00) | uint8_t(c - 0x20);
    return 0;
}

uint32_t lib_ad_bob(RegFrame &r) { return image(r, lib_bnk_ad_bob); }
uint32_t lib_ad_icon(RegFrame &r) { return image(r, lib_bnk_ad_icon); }

} // extern "C"

#if !defined(__mc68000__)
#include "runtime/bindings/lib_stubs.inc"
#endif
