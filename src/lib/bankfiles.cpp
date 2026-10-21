// Saving and loading banks (Bnk.Save*, Bnk.Load, Save, Load)

#include "lib/banks.h"
#include "lib/dos.h"
#include "lib/strings.h"

using namespace amos;

// in banks.cpp
extern "C" uint32_t lib_bnk_un_rev(RegFrame &r);
extern "C" uint32_t lib_bnk_res_bob(RegFrame &r);
extern "C" uint32_t lib_bnk_res_ico(RegFrame &r);

namespace {

constexpr uint32_t EntNul = 0x80000000;

enum class Hunk { Bank = 1, Sprites = 2, Banks = 3, Icons = 4 };

// SHunk: the 4-letter header of a bank file
bool write_hunk(Hunk h)
{
    auto names = static_cast<const char *>(lib_address(L_NHunk));
    return file_write(names + 4 * (static_cast<int>(h) - 1), 4);
}

// Bnk.SaveA0
bool save_bank(uint8_t *bank)
{
    uint16_t flags = bank_flags(bank);
    if (!(flags & (BankBobs | BankIcons))) {
        if (!write_hunk(Hunk::Bank))
            return false;
        uint8_t *header = ptr<uint8_t>(idata().Buffer);
        uint32_t size = peek_l(bank - 20) - 8;      // with the name
        poke_w(header, static_cast<uint16_t>(bank_number(bank)));
        poke_w(header + 2, flags & BankChip ? 0 : 1);
        poke_l(header + 4, size | (flags & BankData ? 0x80000000u : 0));
        return file_write(header, 8) && file_write(bank - 8, size);
    }
    if (!write_hunk(flags & BankBobs ? Hunk::Sprites : Hunk::Icons))
        return false;
    RegFrame r = {};
    r.a[0] = address(bank);
    lib_bnk_un_rev(r);
    if (!file_write(bank, 2))
        return false;
    uint8_t *empty = ptr<uint8_t>(idata().Buffer);
    for (int i = 0; i < 10; i++)
        empty[i] = 0;
    uint16_t count = peek_w(bank);
    uint8_t *entry = bank + 2;
    for (uint16_t i = 0; i < count; i++, entry += 8) {
        uint32_t image = peek_l(entry);
        if (!image) {
            if (!file_write(empty, 10))
                return false;
            continue;
        }
        const uint8_t *p = ptr<uint8_t>(image);
        // mulu on the low word, then a word shift, as in the assembler
        uint32_t size = static_cast<uint16_t>(peek_w(p) * peek_w(p + 2)) * static_cast<uint32_t>(peek_w(p + 4));
        size = (size & 0xFFFF0000u) | static_cast<uint16_t>(size << 1);
        if (!file_write(p, size + 10))
            return false;
    }
    return file_write(entry, 32 * 2);
}

// Bnk.SaveAll
bool save_all()
{
    if (!write_hunk(Hunk::Banks))
        return false;
    uint16_t count = 0;
    for (uint32_t e = *bank_list(); e; e = peek_l(ptr<void>(e)))
        count++;
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    poke_w(buffer, count);
    if (!file_write(buffer, 2))
        return false;
    for (uint32_t e = *bank_list(); e; e = peek_l(ptr<void>(e)))
        if (!save_bank(ptr<uint8_t>(e) + 24))
            return false;
    return true;
}

// Bnk.Load results
enum LoadResult : int16_t { LoadOk = 0, LoadDisk = -1, LoadMemory = -2, LoadNotBank = -3 };

// The images of a sprite or icon file. number: with a sprite bank there,
// a non-zero number (as a word) appends to it.
int16_t load_images(uint32_t number, bool icons)
{
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    if (!file_read(buffer, 2))
        return LoadDisk;
    uint16_t count = peek_w(buffer);
    uint8_t *old = icons ? bank_find(2) : bank_find(1);
    if (old && !(bank_flags(old) & (icons ? BankIcons : BankBobs)))
        old = nullptr;
    uint16_t first = 0;
    RegFrame r = {};
    if (static_cast<uint16_t>(number) && old) {
        first = peek_w(old);
        r.d[0] = 1;
        r.d[1] = static_cast<uint16_t>(count + first);
    } else {
        r.d[0] = 0;
        r.d[1] = count;
    }
    if (icons ? lib_bnk_res_ico(r) : lib_bnk_res_bob(r))
        return LoadMemory;
    uint8_t *entry = ptr<uint8_t>(r.a[0]) + 2 + static_cast<uint16_t>(first << 3);
    for (uint16_t i = 0; i < count; i++, entry += 8) {
        poke_l(entry, 0);
        poke_l(entry + 4, 0);
        if (!file_read(buffer, 10))
            return LoadDisk;
        uint32_t size = static_cast<uint32_t>(peek_w(buffer)) * peek_w(buffer + 2);
        size = (static_cast<uint16_t>(size) * static_cast<uint32_t>(peek_w(buffer + 4))) << 1;
        if (!size)
            continue;
        Regs m = {};
        m.d[0] = size + 10;
        sy_call(sy::MemChip, m);
        uint8_t *image = ptr<uint8_t>(m.a[0]);
        if (!image)
            return LoadMemory;
        poke_l(entry, address(image));
        poke_l(image, peek_l(buffer));                      // width, height
        poke_w(image + 4, peek_w(buffer + 4));              // planes
        poke_w(image + 6, peek_w(buffer + 6) & 0x3FFF);     // hot spot, no flips
        poke_w(image + 8, peek_w(buffer + 8));
        if (!file_read(image + 10, size))
            return LoadDisk;
    }
    return file_read(entry, 32 * 2) ? LoadOk : LoadDisk;
}

// Bnk.Load: the bank(s) of the current file. number: the bank to load
// into (EntNul or negative: the number saved in the file).
int16_t load_bank(uint32_t number)
{
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    if (!file_read(buffer, 4))
        return LoadDisk;
    auto names = static_cast<const uint32_t *>(lib_address(L_NHunk));
    uint32_t id = peek_l(buffer);
    int hunk = 0;
    for (int i = 0; names[i]; i++)
        if (names[i] == id)
            hunk = i + 1;
    switch (static_cast<Hunk>(hunk)) {
    case Hunk::Icons:
        return load_images(number, true);
    case Hunk::Sprites:
        return load_images(number, false);
    case Hunk::Banks: {
        bank_erase_all();
        if (!file_read(buffer, 2))
            return LoadDisk;
        for (uint16_t count = peek_w(buffer); count; count--)
            if (int16_t result = load_bank(EntNul))
                return result;
        return LoadOk;
    }
    case Hunk::Bank: {
        if (!file_read(buffer, 8))
            return LoadDisk;
        if (static_cast<int32_t>(number) < 0) {
            number = peek_w(buffer);
            if (!number)
                number = 5;
        }
        uint32_t length = peek_l(buffer + 4);
        uint16_t flags = 0;
        if (static_cast<int32_t>(length) < 0)
            flags |= BankData;
        if (!peek_w(buffer + 2))
            flags |= BankChip;
        length &= 0x0FFFFFFF;
        uint8_t *bank = bank_reserve(number, flags, length - 8,
                                     static_cast<const char *>(lib_address(L_Bnk_NoLoad)));
        if (!bank)
            return LoadMemory;
        return file_read(bank - 8, length) ? LoadOk : LoadDisk;
    }
    default:
        return LoadNotBank;
    }
}

// Open a file named by an AMOS string, or stop with the disk error
void open_or_error(const uint8_t *name, uint32_t mode)
{
    name_disc(name);
    if (!file_open(mode))
        disk_error();
}

void load(const uint8_t *name, uint32_t number)
{
    open_or_error(name, ModeOldFile);
    int16_t result = load_bank(number);
    file_close();
    bank_changed();
    // every failure is negative, so even out of memory comes out as a
    // disk error, as in the assembler
    if (result)
        disk_error();
}

} // namespace

// Bnk.SaveA0: a0 = bank. Bnk.SaveAll. Bnk.SaveVide: a file without banks.
// Out: d0 = 0 (BEQ), or -1 if writing failed.
extern "C" uint32_t lib_bnk_save_a0(RegFrame &r) { return r.d[0] = save_bank(ptr<uint8_t>(r.a[0])) ? 0 : -1u; }
extern "C" uint32_t lib_bnk_save_all(RegFrame &r) { return r.d[0] = save_all() ? 0 : -1u; }
extern "C" uint32_t lib_bnk_save_vide(RegFrame &r)
{
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    poke_w(buffer, 0);
    return r.d[0] = write_hunk(Hunk::Banks) && file_write(buffer, 2) ? 0 : -1u;
}

// Bnk.Load: d0 = bank number (EntNul: the file's). Out: d0.w = 0, -1
// disk error, -2 out of memory, -3 not a bank (flags: tst.w d0).
extern "C" uint32_t lib_bnk_load(RegFrame &r)
{
    int16_t result = load_bank(r.d[0]);
    r.d[0] = (r.d[0] & 0xFFFF0000u) | static_cast<uint16_t>(result);
    return static_cast<uint16_t>(result);
}

// SAVE "file",n: one bank; SAVE "file": all of them
extern "C" void lib_in_save2(const uint8_t *name, uint32_t n)
{
    open_or_error(name, ModeNewFile);
    uint8_t *bank = bank_find(n);
    if (!bank)
        lib_jump(L_BkNoRes, 0);
    save_bank(bank);
    file_close();
}

extern "C" void lib_in_save1(const uint8_t *name)
{
    open_or_error(name, ModeNewFile);
    save_all();
    file_close();
}

// LOAD "file"[,n]
extern "C" void lib_in_load1(const uint8_t *name) { load(name, EntNul); }

extern "C" void lib_in_load2(const uint8_t *name, int32_t n)
{
    if (n >= 0x10000)
        fon_call();
    load(name, n);
}
