// Memory banks and lists (Bnk.* in +Lib.s, Lst.* and Bnk.PrevProgram /
// Bnk.CurProgram in +B.s)

#include "lib/banks.h"

#include "core/amiga.h"
#include "lib/convert.h"
#include "hal/system.h"

namespace amos {

namespace {

ContextCursor mem_reserve(uint32_t length, uint32_t flags)
{
    Regs r = {};
    r.d[0] = length;
    r.d[1] = flags;
    r.a[0] = w::reserve(r.d[0], r.d[1]);
    return decode_context_cursor(r.a[0]);
}

void mem_free(uint32_t at, uint32_t length)
{
    Regs r = {};
    r.a[1] = at;
    r.d[0] = length;
    w::release(r.a[1], r.d[0]);
}

// The element before element in the list (the list head itself for the
// first), or null if element is not in it
ContextCursor lst_find_previous(ContextCursor list, ContextCursor element)
{
    ContextCursor previous = list;
    while (uint32_t next = peek_l(previous)) {
        if (next == encode_context_cursor(element))
            return previous;
        previous = decode_context_cursor(next);
    }
    return ContextCursor{};
}

constexpr int BankHeader = 24;

ContextCursor bank_of(uint32_t element) { return decode_context_cursor(element) + BankHeader; }

// The bank with this number: its element, or null
ContextCursor bank_element(uint32_t number)
{
    for (uint32_t e = peek_l(bank_list()); e; e = peek_l(decode_context_cursor(e)))
        if (peek_l(decode_context_cursor(e) + 8) == number)
            return decode_context_cursor(e);
    return ContextCursor{};
}

// The image of a sprite: width in words, height, planes...
uint32_t image_size(ContextCursor image)
{
    // mulu on the low word, as in the assembler
    uint32_t size = static_cast<uint16_t>(peek_w(image) * peek_w(image + 2) * 2);
    return size * peek_w(image + 4);
}

// Bnk.EffBobA0: free the image and mask of a sprite bank entry
void erase_image(ContextCursor entry)
{
    if (uint32_t image = peek_l(entry))
        mem_free(image, image_size(decode_context_cursor(image)) + 10);
    int32_t mask = static_cast<int32_t>(peek_l(entry + 4));
    if (mask > 0)
        mem_free(uint32_t(mask), peek_l(decode_context_cursor(uint32_t(mask))));
    poke_l(entry, 0);
    poke_l(entry + 4, 0);
}

} // namespace

// ---- lists ------------------------------------------------------------

ContextCursor lst_cree(ContextCursor list, uint32_t length, uint32_t memflags)
{
    ContextCursor element = mem_reserve(length + 8, memflags);
    if (element) {
        poke_l(element, peek_l(list));
        poke_l(list, encode_context_cursor(element));
        poke_l(element + 4, length);
    }
    return element;
}

ContextCursor lst_new(ContextCursor list, uint32_t length) { return lst_cree(list, length, MemClear | MemPublic); }

void lst_del(ContextCursor list, ContextCursor element)
{
    if (ContextCursor previous = lst_find_previous(list, element)) {
        poke_l(previous, peek_l(element));
        mem_free(encode_context_cursor(element), peek_l(element + 4) + 8);
    }
}

void lst_del_all(ContextCursor list)
{
    while (uint32_t first = peek_l(list))
        lst_del(list, decode_context_cursor(first));
}

void lst_insert(ContextCursor list, ContextCursor element)
{
    poke_l(element, peek_l(list));
    poke_l(list, encode_context_cursor(element));
}

void lst_remove(ContextCursor list, ContextCursor element)
{
    if (ContextCursor previous = lst_find_previous(list, element))
        poke_l(previous, peek_l(element));
}

// ---- banks ------------------------------------------------------------

ContextCursor bank_find(uint32_t number)
{
    ContextCursor e = bank_element(number);
    return e ? bank_of(encode_context_cursor(e)) : ContextCursor{};
}

ContextCursor bank_bobs()
{
    ContextCursor bank = bank_find(1);
    return bank && bank_flags(bank) & BankBobs ? bank : ContextCursor{};
}

ContextCursor bank_icons()
{
    ContextCursor bank = bank_find(2);
    return bank && bank_flags(bank) & BankIcons ? bank : ContextCursor{};
}

uint32_t bank_length(ContextCursor bank)
{
    if (bank_flags(bank) & (BankBobs | BankIcons))
        return peek_w(bank);
    return bank_size(bank);
}

void bank_erase_at(ContextCursor bank)
{
    if (bank_flags(bank) & (BankBobs | BankIcons)) {
        uint16_t count = peek_w(bank);
        ContextCursor entry = bank + 2;
        for (uint16_t i = 0; i < count; i++, entry += 8)
            erase_image(entry);
    }
    poke_l(bank - 8, 0);        // the name
    poke_l(bank - 4, 0);
    lst_del(bank_list(), bank - BankHeader);
}

void bank_erase(uint32_t number)
{
    if (ContextCursor bank = bank_find(number))
        bank_erase_at(bank);
}

void bank_erase_all()
{
    uint32_t e = peek_l(bank_list());
    while (e) {
        uint32_t next = peek_l(decode_context_cursor(e));
        bank_erase_at(bank_of(e));
        e = next;
    }
}

void bank_erase_temp()
{
    uint32_t e = peek_l(bank_list());
    while (e) {
        uint32_t next = peek_l(decode_context_cursor(e));
        if (!(peek_w(decode_context_cursor(e) + 12) & BankData))
            bank_erase_at(bank_of(e));
        e = next;
    }
}

void bank_changed()
{
    // tell the extensions
    for (int i = 0; i < 26; i++) {
        if (uint32_t routine = idata().ExtAdr[i * 4 + 3]) {
            Regs r = {};
            r.a[0] = peek_l(bank_list());
            amos_call(&r, host::decode_routine(routine));
        }
    }
    // the sprite bank: bank 1, whatever it holds (as the assembler did)
    Regs r = {};
    r.a[1] = encode_context_cursor(bank_find(1));
    sy_call(sy::SetSpBank, r);
}

ContextCursor bank_reserve(uint32_t number, uint16_t flags, uint32_t length, const char *name)
{
    number &= 0xFFFF;
    bank_erase(number);
    uint32_t memflags = MemPublic | MemClear | (flags & BankChip ? MemChip : 0);
    ContextCursor e = lst_cree(bank_list(), length + 16, memflags);
    if (!e)
        return ContextCursor{};
    poke_l(e + 8, number);
    poke_w(e + 12, flags);
    poke_w(e + 14, 0);
    for (int i = 0; i < 8; i++)
        host::detail::write_cursor_at(e + (16 + i), uint8_t(name[i]));
    return e + BankHeader;
}

uint32_t bank_or_address(int32_t value)
{
    if (value >= 1024)
        return value;
    ContextCursor bank = bank_find(value);
    if (!bank)
        error(36);
    return encode_context_cursor(bank);
}

bool banks_previous_program()
{
    IData &d = idata();
    if (d.Mon_Base || d.Direct)
        return false;
    auto running = ptr<Program>(d.Prg_Runned);
    uint32_t previous = running->Prg_Previous;
    if (!previous) {
        if (!d.Edt_Current)
            return false;
        previous = ptr<EditWindow>(d.Edt_Current)->Edt_Prg;
        if (!previous || previous == d.Prg_Runned)
            return false;
    }
    d.Cur_Banks = address(&ptr<Program>(previous)->Prg_Banks);
    return true;
}

void banks_current_program()
{
    idata().Cur_Banks = address(&ptr<Program>(idata().Prg_Runned)->Prg_Banks);
}

} // namespace amos

using namespace amos;

namespace {

// Bnk.Ric2: a new sprite / icon bank of count entries, taking over the
// images of old. mode 0: the old bank's images are dropped; 1: they are
// kept (as many as fit); -1: the old bank stays and only its palette is
// copied. Returns the new bank, or null if out of memory.
ContextCursor bank_resize_images(ContextCursor old, int16_t mode, uint16_t count, uint16_t flags)
{
    ContextCursor e = lst_new(bank_list(), count * 8u + 8 * 2 + 2 + 64);
    if (!e)
        return ContextCursor{};
    bool icons = flags & BankIcons;
    poke_l(e + 8, icons ? 2 : 1);
    poke_w(e + 12, flags);
    poke_w(e + 14, 0);
    auto name = static_cast<const char *>(lib_address(icons ? L_BkIco : L_BkSpr));
    for (int i = 0; i < 8; i++)
        host::detail::write_cursor_at(e + (16 + i), uint8_t(name[i]));
    ContextCursor bank = e + 24;
    poke_w(bank, count);
    ContextCursor palette;
    if (mode < 0 || (mode > 0 && old)) {
        if (mode > 0) {
            // take over the images that fit; the old bank loses them
            uint16_t n = peek_w(old) < count ? peek_w(old) : count;
            ContextCursor from = old + 2;
            ContextCursor to = bank + 2;
            for (uint32_t i = 0; i < n * 2u; i++, from += 4, to += 4) {
                poke_l(to, peek_l(from));
                poke_l(from, 0);
            }
        }
        palette = old + (2 + peek_w(old) * 8);
    } else if (idata().ScOnAd) {
        palette = decode_context_cursor(idata().ScOnAd + uint32_t(offsetof(Screen, EcPal)));
    } else {
        palette = decode_context_cursor(address(idata().DefPal));
    }
    ContextCursor to = bank + (2 + count * 8);
    for (int i = 0; i < 32; i++)
        poke_w(to + 2 * i, peek_w(palette + 2 * i));
    if (mode >= 0 && old)
        bank_erase_at(old);
    return bank;
}

} // namespace

// ---- register entries for the assembler ---------------------------------
//
// Each returns what the callers' branch tests (see RegRoutine).

// Lst.Cree: a0 = list, d0 = length, d1 = memory flags. Out: a1 = d0 =
// element, or 0 (BEQ).
extern "C" uint32_t lib_lst_cree(RegFrame &r)
{
    ContextCursor e = lst_cree(decode_context_cursor(r.a[0]), r.d[0], r.d[1]);
    r.d[1] = r.d[0];
    r.a[1] = r.d[0] = encode_context_cursor(e);
    return r.d[0];
}
extern "C" uint32_t lib_lst_new(RegFrame &r)
{
    r.d[1] = MemClear | MemPublic;
    return lib_lst_cree(r);
}
extern "C" uint32_t lib_lst_chip_new(RegFrame &r)
{
    r.d[1] = MemChip | MemClear | MemPublic;
    return lib_lst_cree(r);
}

// Lst.Del, Lst.Remove, Lst.Insert: a0 = list, a1 = element. Lst.DelAll:
// a0 = list.
extern "C" uint32_t lib_lst_del(RegFrame &r)
{
    lst_del(decode_context_cursor(r.a[0]), decode_context_cursor(r.a[1]));
    return 0;
}
extern "C" uint32_t lib_lst_remove(RegFrame &r)
{
    lst_remove(decode_context_cursor(r.a[0]), decode_context_cursor(r.a[1]));
    return 0;
}
extern "C" uint32_t lib_lst_insert(RegFrame &r)
{
    lst_insert(decode_context_cursor(r.a[0]), decode_context_cursor(r.a[1]));
    return 0;
}
extern "C" uint32_t lib_lst_del_all(RegFrame &r)
{
    lst_del_all(decode_context_cursor(r.a[0]));
    r.d[0] = r.a[1] = 0;
    return 0;
}

// Bnk.PrevProgram (BNE: switched), Bnk.CurProgram
extern "C" uint32_t lib_bnk_prev_program(RegFrame &) { return banks_previous_program(); }
extern "C" uint32_t lib_bnk_cur_program(RegFrame &)
{
    banks_current_program();
    return 0;
}

// Bnk.GetAdr: d0 = number. Out: BEQ not found (a0 = a1 = 0, d1 = 0); BNE
// found: a0 = a1 = bank, d0.w = its flags, d1 = its element.
extern "C" uint32_t lib_bnk_get_adr(RegFrame &r)
{
    ContextCursor e = bank_element(r.d[0]);
    if (!e) {
        r.a[0] = r.a[1] = r.d[1] = 0;
        return 0;
    }
    ContextCursor bank = bank_of(encode_context_cursor(e));
    r.d[1] = encode_context_cursor(e);
    r.a[0] = r.a[1] = encode_context_cursor(bank);
    r.d[0] = (r.d[0] & 0xFFFF0000u) | bank_flags(bank);
    return 1;
}

// Bnk.GetBobs, Bnk.GetIcons: a0 = a1 = bank 1 / 2, whatever it holds (0
// if none); BNE if it holds sprites / icons. d0/d1 are kept.
static uint32_t get_images(RegFrame &r, uint32_t number, uint16_t flag)
{
    ContextCursor bank = bank_find(number);
    r.a[0] = r.a[1] = encode_context_cursor(bank);
    return bank && bank_flags(bank) & flag;
}
extern "C" uint32_t lib_bnk_get_bobs(RegFrame &r) { return get_images(r, 1, BankBobs); }
extern "C" uint32_t lib_bnk_get_icons(RegFrame &r) { return get_images(r, 2, BankIcons); }

// Bnk.Eff: d0 = number. Bnk.EffA0: a0 = bank. Bnk.EffBobA0: a0 = entry.
// Bnk.EffAll, Bnk.EffTemp.
extern "C" uint32_t lib_bnk_eff(RegFrame &r)
{
    bank_erase(r.d[0]);
    return 0;
}
extern "C" uint32_t lib_bnk_eff_a0(RegFrame &r)
{
    bank_erase_at(decode_context_cursor(r.a[0]));
    return 0;
}
extern "C" uint32_t lib_bnk_eff_bob_a0(RegFrame &r)
{
    erase_image(decode_context_cursor(r.a[0]));
    return 0;
}
extern "C" uint32_t lib_bnk_eff_all(RegFrame &)
{
    bank_erase_all();
    return 0;
}
extern "C" uint32_t lib_bnk_eff_temp(RegFrame &)
{
    bank_erase_temp();
    return 0;
}

// Bnk.OrAdr: d0 = bank number or address. Out: a0 = d0 = address.
extern "C" uint32_t lib_bnk_or_adr(RegFrame &r)
{
    r.a[0] = r.d[0] = bank_or_address(r.d[0]);
    return r.d[0];
}

// Bnk.AdBob, Bnk.AdIcon: d0 = image number. Out: a0 = d0 = its entry, or
// 0 (BEQ: no bank, or past the end); d1 = number of images.
static uint32_t image_entry(RegFrame &r, ContextCursor bank)
{
    r.d[1] = 0;
    r.a[0] = 0;
    if (bank) {
        r.d[1] = peek_w(bank);
        uint16_t n = static_cast<uint16_t>(r.d[0]);
        if (n <= r.d[1])
            r.a[0] = encode_context_cursor(bank - 8 + 2) + static_cast<uint16_t>(n << 3);
    }
    return r.d[0] = r.a[0];
}
extern "C" uint32_t lib_bnk_ad_bob(RegFrame &r) { return image_entry(r, bank_bobs()); }
extern "C" uint32_t lib_bnk_ad_icon(RegFrame &r) { return image_entry(r, bank_icons()); }

// Bnk.Ric: a1 = old bank (or 0), d0 = mode, d1 = count, d2 = flags.
// Bnk.ResBob / Bnk.ResIco find the old bank and the flags themselves. Out:
// BEQ: a0 = new bank, a1 = old one, d0 = 0; BNE out of memory: a0 = 0,
// d0 = -1.
static uint32_t reserve_images(RegFrame &r, ContextCursor old, uint16_t flags)
{
    ContextCursor bank = bank_resize_images(old, static_cast<int16_t>(r.d[0]),
                                       static_cast<uint16_t>(r.d[1]), flags);
    r.a[0] = encode_context_cursor(bank);
    r.a[1] = encode_context_cursor(old);
    r.d[0] = bank ? 0 : -1u;
    return static_cast<uint16_t>(r.d[0]);
}
extern "C" uint32_t lib_bnk_ric(RegFrame &r)
{
    return reserve_images(r, decode_context_cursor(r.a[1]), static_cast<uint16_t>(r.d[2]));
}
extern "C" uint32_t lib_bnk_res_bob(RegFrame &r) { return reserve_images(r, bank_find(1), BankBobs | BankData); }
extern "C" uint32_t lib_bnk_res_ico(RegFrame &r) { return reserve_images(r, bank_find(2), BankIcons | BankData); }

// Bnk.Schrink: d0 = number, d1 = new size. Out: d0 = error number or 0.
extern "C" uint32_t lib_bnk_schrink(RegFrame &r)
{
    int32_t size = static_cast<int32_t>(r.d[1]);
    ContextCursor bank = size > 0 ? bank_find(r.d[0]) : ContextCursor{};
    if (size <= 0)
        return r.d[0] = 23;
    if (!bank)
        return r.d[0] = 36;
    if (bank_flags(bank) & (BankBobs | BankIcons))
        return r.d[0] = 23;
    ContextCursor e = bank - 24;
    int32_t old_size = static_cast<int32_t>(peek_l(e + 4) + 8);
    int32_t new_size = size + 3 * 8;
    if (new_size > old_size)
        return r.d[0] = 23;
    // give back the end of the memory: free it all and take the start
    // again, without letting another task in between
    uint32_t next = peek_l(e);
    hal::forbid();
    hal::release(bank_host_view(e), old_size);
    hal::allocate_at(new_size, bank_host_view(e));
    hal::permit();
    poke_l(e, next);
    poke_l(e + 4, new_size - 8);
    return r.d[0] = 0;
}

// Bnk.Reserve: d0 = number, d1 = flags, d2 = length, a0 = name. Out:
// a0 = d0 = bank, or 0 (BEQ).
extern "C" uint32_t lib_bnk_reserve(RegFrame &r)
{
    ContextCursor bank = bank_reserve(r.d[0], static_cast<uint16_t>(r.d[1]), r.d[2],
                                      ptr<const char>(r.a[0]));
    return r.a[0] = r.d[0] = encode_context_cursor(bank);
}

// Bnk.Change: tell the extensions and the sprites. Keeps all registers;
// the flags test the caller's d0.w.
extern "C" uint32_t lib_bnk_change(RegFrame &r)
{
    bank_changed();
    return static_cast<uint16_t>(r.d[0]);
}

// Bnk.Length: a0 = bank. Out: d0 = memory it uses, with its images.
extern "C" uint32_t lib_bnk_length(RegFrame &r)
{
    ContextCursor bank = decode_context_cursor(r.a[0]);
    uint32_t size = bank_size(bank);
    if (bank_flags(bank) & (BankBobs | BankIcons)) {
        uint16_t count = peek_w(bank);
        ContextCursor entry = bank + 2;
        for (uint16_t i = 0; i < count; i++, entry += 8) {
            if (uint32_t image = peek_l(entry)) {
                // mulu on the low word, as in the assembler
                ContextCursor p = decode_context_cursor(image);
                uint32_t wh = static_cast<uint32_t>(peek_w(p)) * peek_w(p + 2);
                size += (static_cast<uint16_t>(wh) * static_cast<uint32_t>(peek_w(p + 4))) << 1;
            }
        }
    }
    return r.d[0] = size;
}

// Bnk.GetLength: out: d0 = normal banks, d1 = sprite banks, d2 = icon
// banks: the memory they use
extern "C" uint32_t lib_bnk_get_length(RegFrame &r)
{
    uint32_t normal = 0, bobs = 0, icons = 0;
    for (uint32_t e = peek_l(bank_list()); e; e = peek_l(decode_context_cursor(e))) {
        RegFrame f = {};
        f.a[0] = encode_context_cursor(bank_of(e));
        uint32_t size = lib_bnk_length(f);
        uint16_t flags = peek_w(decode_context_cursor(e) + 12);
        if (flags & BankBobs)
            bobs += size;
        else if (flags & BankIcons)
            icons += size;
        else
            normal += size;
    }
    r.d[1] = bobs;
    r.d[2] = icons;
    return r.d[0] = normal;
}

// ---- more register entries ---------------------------------------------

namespace {

// Bnk.InsBob: a new empty image n in the sprite / icon bank (images from n
// on move up; past the end, it goes at the end). Returns the new entry, or
// null if out of memory.
ContextCursor bank_insert_image(ContextCursor old, uint32_t n)
{
    uint16_t count = peek_w(old);
    ContextCursor bank = bank_resize_images(old, -1, count + 1, bank_flags(old));
    if (!bank)
        return ContextCursor{};
    ContextCursor from = old + 2;
    ContextCursor to = bank + 2;
    ContextCursor hole = ContextCursor{};
    bool holed = false;
    for (uint16_t i = 1; i <= count + (holed ? 1 : 0); i++) {
        if (i == static_cast<uint16_t>(n) && !holed) {
            hole = to;
            holed = true;
            to += 8;
            continue;
        }
        for (int k = 0; k < 2; k++, to += 4, from += 4) {
            poke_l(to, peek_l(from));
            poke_l(from, 0);
        }
    }
    if (!holed)
        hole = to;
    bank_erase_at(old);
    return hole;
}

// Bnk.DelBob: erase images first to last (counted from 1) of the bank;
// the bank goes when none is left
void bank_delete_images(ContextCursor bank, int32_t first, int32_t last)
{
    uint16_t count = peek_w(bank);
    uint16_t left = 0;
    ContextCursor entry = bank + 2;
    for (int32_t i = 1; i <= count; i++, entry += 8) {
        if (i >= first && i <= last)
            erase_image(entry);
        else
            left++;
    }
    if (left) {
        ContextCursor smaller = bank_resize_images(bank, -1, left, bank_flags(bank));
        if (!smaller)
            return;
        ContextCursor from = bank + 2;
        ContextCursor to = smaller + 2;
        for (int32_t i = 1; i <= count; i++, from += 8) {
            if (i < first || i > last) {
                poke_l(to, peek_l(from));
                poke_l(to + 4, peek_l(from + 4));
                to += 8;
            }
            poke_l(from, 0);
            poke_l(from + 4, 0);
        }
    }
    bank_erase_at(bank);
}

} // namespace

// Bnk.InsBob: d0 = number, a0 = bank. Out: BEQ: a0 = the new entry; BNE
// out of memory.
extern "C" uint32_t lib_bnk_ins_bob(RegFrame &r)
{
    ContextCursor entry = bank_insert_image(decode_context_cursor(r.a[0]), r.d[0]);
    if (!entry)
        return r.d[0] = -1u;
    r.a[0] = encode_context_cursor(entry);
    return r.d[0] = 0;
}

// Bnk.DelBob: d0 = first, d1 = last, a0 = bank
extern "C" uint32_t lib_bnk_del_bob(RegFrame &r)
{
    bank_delete_images(decode_context_cursor(r.a[0]), r.d[0], r.d[1]);
    return r.d[0] = 0;
}

// Bnk.UnRev: a0 = sprite bank; puts every image the right way round.
// Keeps all registers.
extern "C" uint32_t lib_bnk_un_rev(RegFrame &r)
{
    ContextCursor bank = decode_context_cursor(r.a[0]);
    uint16_t count = peek_w(bank);
    ContextCursor entry = bank + 2;
    for (uint16_t i = 0; i < count; i++, entry += 8) {
        Regs e = {};
        e.a[1] = encode_context_cursor(entry);
        ec_call(ec::DoRev, e);
        if (uint32_t image = peek_l(entry)) {
            // the hot spot: drop the flip bits 14-15
            ContextCursor p = decode_context_cursor(image);
            poke_w(p + 6, static_cast<uint16_t>(static_cast<int16_t>(peek_w(p + 6) << 2) >> 2));
        }
    }
    return 0;
}

// ---- instructions -------------------------------------------------------

namespace {

[[noreturn]] void bank_not_reserved() { error(36); }
[[noreturn]] void out_of_memory() { error(24); }

// Bnk.List: the next bank after number min, as a line of text in Buffer.
// Returns its number, or 0 if there is none.
uint32_t bank_list_line(uint32_t min)
{
    ContextCursor best = ContextCursor{};
    uint32_t number = 65536;
    for (uint32_t e = peek_l(bank_list()); e; e = peek_l(decode_context_cursor(e))) {
        uint32_t n = peek_l(decode_context_cursor(e) + 8);
        if (n > min && n <= number) {
            best = decode_context_cursor(e);
            number = n;
        }
    }
    if (number == 65536)
        return 0;
    char *out = ptr<char>(idata().Buffer);
    if (static_cast<uint16_t>(number) < 10)
        *out++ = ' ';
    out = long_to_dec(out, number, -1, false);
    *out++ = ' ';
    *out++ = '-';
    *out++ = ' ';
    for (int i = 0; i < 8; i++)
        *out++ = host::detail::read_cursor_byte(best + (16 + i));
    for (const char *t = " S: "; *t;)
        *out++ = *t++;
    out = long_to_hex(out, encode_context_cursor(best + 24), 8);
    for (const char *t = " L: "; *t;)
        *out++ = *t++;
    out = long_to_dec(out, bank_length(best + 24), -1, false);
    *out++ = 13;
    *out++ = 10;
    *out = 0;
    return number;
}

// The bank of the program under this one (Bstart, Blength)
ContextCursor previous_program_bank(uint32_t number)
{
    if (!banks_previous_program())
        return ContextCursor{};
    ContextCursor bank = bank_find(number);
    banks_current_program();
    return bank;
}

void reserve(int32_t number, int32_t length, uint16_t flags, int name)
{
    if (length <= 0 || number <= 0 || number >= 0x10000)
        fon_call();
    if (!bank_reserve(number, flags, length, static_cast<const char *>(lib_address(name))))
        out_of_memory();
    bank_changed();
}

void check_number(int32_t n)
{
    if (n <= 0 || n >= 0x10000)
        fon_call();
}

void insert_image(ContextCursor bank, int32_t n)
{
    if (!bank)
        bank_not_reserved();
    if (n <= 0)
        fon_call();
    ContextCursor entry = bank_insert_image(bank, n);
    bank_changed();
    if (!entry)
        out_of_memory();
}

void delete_images(ContextCursor bank, int32_t a, int32_t b)
{
    if (!bank)
        bank_not_reserved();
    if (a <= 0 || b <= 0 || static_cast<uint32_t>(b) > static_cast<uint32_t>(a))
        fon_call();
    bank_delete_images(bank, b, a);
    bank_changed();
}

} // namespace

// LIST BANK
extern "C" void lib_in_list_bank()
{
    uint32_t number = 0;
    while ((number = bank_list_line(number))) {
        Regs r = {};
        r.a[0] = idata().Buffer;
        lib_call(L_ImpChaine, r);
    }
}

// ERASE n, ERASE TEMP, ERASE ALL
extern "C" void lib_in_erase(uint32_t n)
{
    bank_erase(n);
    bank_changed();
}

extern "C" void lib_in_erase_temp()
{
    bank_erase_temp();
    bank_changed();
}

extern "C" void lib_in_erase_all()
{
    bank_erase_all();
    bank_changed();
}

// BANK SWAP a,b: the banks exchange their numbers
extern "C" void lib_in_bank_swap(int32_t a, int32_t b)
{
    check_number(a);
    ContextCursor bank_a = bank_find(a);
    check_number(b);
    ContextCursor bank_b = bank_find(b);
    if (bank_a && bank_b) {
        poke_l(bank_a - 16, b);
        poke_l(bank_b - 16, a);
    } else if (bank_a) {
        poke_l(bank_a - 16, b);
    } else if (bank_b) {
        poke_l(bank_b - 16, a);
    }
    bank_changed();
}

// =BSTART(n), =BLENGTH(n): a bank of the program under this one
extern "C" uint32_t lib_fn_bstart(uint32_t n)
{
    if (!banks_previous_program())
        bank_not_reserved();
    ContextCursor bank = bank_find(n);
    banks_current_program();
    if (!bank)
        bank_not_reserved();
    return encode_context_cursor(bank);
}

extern "C" uint32_t lib_fn_blength(uint32_t n)
{
    ContextCursor bank = previous_program_bank(n);
    return bank ? bank_length(bank) : 0;
}

// BGRAB n: take bank n of the program under this one
extern "C" void lib_in_bgrab(int32_t n)
{
    if (n <= 0)
        fon_call();
    bank_erase(n);
    ContextCursor bank = ContextCursor{};
    if (banks_previous_program())
        bank = bank_find(n);
    if (!bank) {
        banks_current_program();
        bank_changed();
        bank_not_reserved();
    }
    lst_remove(bank_list(), bank - 24);
    banks_current_program();
    lst_insert(bank_list(), bank - 24);
    bank_changed();
}

// BSEND n: give bank n to the program under this one
extern "C" void lib_in_bsend(int32_t n)
{
    if (n <= 0 || !banks_previous_program())
        fon_call();
    bank_erase(n);
    banks_current_program();
    if (ContextCursor bank = bank_find(n)) {
        lst_remove(bank_list(), bank - 24);
        banks_previous_program();
        lst_insert(bank_list(), bank - 24);
        banks_current_program();
    }
    bank_changed();
}

// INS BOB n, INS ICON n: insert an empty image
extern "C" void lib_in_ins_sprite(int32_t n) { insert_image(bank_bobs(), n); }
extern "C" void lib_in_ins_icon(int32_t n) { insert_image(bank_icons(), n); }

// DEL BOB a[,b], DEL ICON a[,b]
extern "C" void lib_in_del_sprite2(int32_t a, int32_t b) { delete_images(bank_bobs(), a, b); }
extern "C" void lib_in_del_sprite1(int32_t a) { delete_images(bank_bobs(), a, a); }
extern "C" void lib_in_del_icon2(int32_t a, int32_t b) { delete_images(bank_icons(), a, b); }
extern "C" void lib_in_del_icon1(int32_t a) { delete_images(bank_icons(), a, a); }

// RESERVE AS WORK / CHIP WORK / DATA / CHIP DATA n,length
extern "C" void lib_in_res_work(int32_t n, int32_t length) { reserve(n, length, 0, L_BkWrk); }
extern "C" void lib_in_res_chip_work(int32_t n, int32_t length) { reserve(n, length, BankChip, L_BkWrk); }
extern "C" void lib_in_res_data(int32_t n, int32_t length) { reserve(n, length, BankData, L_BkDat); }
extern "C" void lib_in_res_chip_data(int32_t n, int32_t length)
{
    reserve(n, length, BankData | BankChip, L_BkDat);
}

// BANK SHRINK n To length
extern "C" void lib_in_bank_schrink(uint32_t n, uint32_t length)
{
    RegFrame r = {};
    r.d[0] = n;
    r.d[1] = length;
    if (uint32_t err = lib_bnk_schrink(r))
        error(err);
}

// =START(n), =LENGTH(n)
extern "C" uint32_t lib_fn_start(uint32_t n)
{
    ContextCursor bank = bank_find(n);
    if (!bank)
        bank_not_reserved();
    return encode_context_cursor(bank);
}

extern "C" uint32_t lib_fn_length(uint32_t n)
{
    ContextCursor bank = bank_find(n);
    return bank ? bank_length(bank) : 0;
}

// =CHIP FREE, =FAST FREE
extern "C" uint32_t lib_fn_chip_free() { return hal::available_memory(hal::MemoryChip); }
extern "C" uint32_t lib_fn_fast_free() { return hal::available_memory(hal::MemoryFast); }

#if !defined(__mc68000__)
#include "runtime/bindings/lib_banks.inc"
#endif
