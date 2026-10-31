// The fonts on another host: topaz, the Amiga's font in ROM, read from the
// Kickstart image the user gives (AMOS_KICKSTART, a Cloanto-encrypted one
// with its key in AMOS_ROMKEY); without it a stand-in of the same size
// (boxes). The fonts are TextFonts of the host (native fields), their
// data below 4GB.

#include "hal/system.h"
#include "host/host.h"

#include <graphics/text.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace amos;

namespace {

struct Font {
    TextFont font;
    char name[32];
    int users;
};

Font *fonts[16];

uint8_t *read_file(const char *name, long &size)
{
    FILE *f = name ? fopen(name, "rb") : nullptr;
    if (!f)
        return nullptr;
    fseek(f, 0, SEEK_END);
    size = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t *data = static_cast<uint8_t *>(malloc(size_t(size)));
    if (data && fread(data, 1, size_t(size), f) != size_t(size)) {
        free(data);
        data = nullptr;
    }
    fclose(f);
    return data;
}

// The Kickstart image, decrypted, and its base address
uint8_t *rom;
long rom_size;
uint32_t rom_base;

bool load_rom()
{
    static bool tried;
    if (tried)
        return rom != nullptr;
    tried = true;
    long size;
    uint8_t *data = read_file(getenv("AMOS_KICKSTART"), size);
    if (!data)
        return false;
    if (size > 11 && !memcmp(data, "AMIROMTYPE1", 11)) {
        long key_size;
        uint8_t *key = read_file(getenv("AMOS_ROMKEY"), key_size);
        if (!key || !key_size) {
            free(data);
            return false;
        }
        memmove(data, data + 11, size_t(size - 11));
        size -= 11;
        for (long i = 0; i < size; i++)
            data[i] ^= key[i % key_size];
        free(key);
    }
    rom = data;
    rom_size = size;
    rom_base = 0x1000000 - uint32_t(size);         // $F80000 for 512K
    return true;
}

uint32_t rom_l(uint32_t at) { return uint32_t(rom[at] << 24 | rom[at + 1] << 16 | rom[at + 2] << 8 | rom[at + 3]); }
uint16_t rom_w(uint32_t at) { return uint16_t(rom[at] << 8 | rom[at + 1]); }
bool in_rom(uint32_t address) { return address >= rom_base && address - rom_base < uint32_t(rom_size); }

// A copy below 4GB
template <typename T> T *low_copy(const void *from, size_t n)
{
    T *to = static_cast<T *>(hal::allocate(uint32_t(n), 0x10000));
    if (to && from)
        memcpy(to, from, n);
    return to;
}

// The font size of the ROM (its TextFont, named "topaz.font"), as a font
// of the host
Font *rom_font(uint16_t size)
{
    if (!load_rom())
        return nullptr;
    for (uint32_t at = 0; at + 52 < uint32_t(rom_size); at += 2) {
        if (rom_w(at + 20) != size || rom[at + 32] >= rom[at + 33])  // tf_YSize, tf_LoChar < tf_HiChar
            continue;
        uint32_t name = rom_l(at + 10);
        if (!in_rom(name) || strcmp(reinterpret_cast<const char *>(rom + name - rom_base), "topaz.font"))
            continue;
        uint32_t data = rom_l(at + 34), loc = rom_l(at + 40);
        if (!in_rom(data) || !in_rom(loc))
            continue;
        Font *f = low_copy<Font>(nullptr, sizeof(Font));
        TextFont &t = f->font;
        t.tf_YSize = size;
        t.tf_Style = rom[at + 22];
        t.tf_Flags = rom[at + 23];
        t.tf_XSize = rom_w(at + 24);
        t.tf_Baseline = rom_w(at + 26);
        t.tf_BoldSmear = rom_w(at + 28);
        t.tf_LoChar = rom[at + 32];
        t.tf_HiChar = rom[at + 33];
        t.tf_Modulo = rom_w(at + 38);
        int chars = t.tf_HiChar - t.tf_LoChar + 2;
        t.tf_CharData = low_copy<uint8_t>(rom + data - rom_base, size_t(t.tf_Modulo) * size);
        ULONG *locs = low_copy<ULONG>(nullptr, sizeof(ULONG) * size_t(chars));
        for (int i = 0; i < chars; i++)
            locs[i] = rom_l(loc - rom_base + 4 * uint32_t(i));
        t.tf_CharLoc = locs;
        snprintf(f->name, sizeof f->name, "topaz.font");
        t.tf_Message.mn_Node.ln_Name = f->name;
        return f;
    }
    return nullptr;
}

// Without the ROM: boxes of the size asked for
Font *stand_in(uint16_t size)
{
    Font *f = low_copy<Font>(nullptr, sizeof(Font));
    TextFont &t = f->font;
    t.tf_YSize = size;
    t.tf_XSize = 8;
    t.tf_Baseline = uint16_t(size - 2);
    t.tf_LoChar = 32;
    t.tf_HiChar = 255;
    int chars = 255 - 32 + 2;
    t.tf_Modulo = uint16_t(chars);
    uint8_t *data = low_copy<uint8_t>(nullptr, size_t(chars) * size);
    ULONG *locs = low_copy<ULONG>(nullptr, sizeof(ULONG) * size_t(chars));
    for (int c = 0; c < chars; c++) {
        locs[c] = uint32_t(c * 8) << 16 | 8;
        if (c == 0)
            continue;       // the space
        for (int y = 1; y < size - 1; y++)
            data[y * chars + c] = y == 1 || y == size - 2 ? 0x7C : 0x44;
    }
    t.tf_CharData = data;
    t.tf_CharLoc = locs;
    snprintf(f->name, sizeof f->name, "topaz.font");
    t.tf_Message.mn_Node.ln_Name = f->name;
    return f;
}

} // namespace

namespace amos::host {

TextFont *font_open(const char *name, uint16_t size)
{
    if (!name || strcasecmp(name, "topaz.font"))
        return nullptr;
    if (size != 9)
        size = 8;
    for (Font *f : fonts)
        if (f && f->font.tf_YSize == size) {
            f->users++;
            return &f->font;
        }
    Font *f = rom_font(size);
    if (!f) {
        static bool told;
        if (!told)
            fprintf(stderr, "amos: no topaz font (AMOS_KICKSTART, AMOS_ROMKEY): boxes instead\n");
        told = true;
        f = stand_in(size);
    }
    for (Font *&slot : fonts)
        if (!slot) {
            slot = f;
            break;
        }
    f->users = 1;
    return &f->font;
}

void font_close(TextFont *) {}

} // namespace amos::host
