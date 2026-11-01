// The fonts on another host: topaz, the Amiga's font in ROM, taken from
// the user's Kickstart when AMOS is built (tools/extract_topaz.py: build/
// host/topaz.cpp), else AROS's topaz 8 (data/fonts/topaz8); without
// either, a stand-in of the same size (boxes). The
// fonts are TextFonts of the host (native fields), their data below 4GB.

#include "hal/system.h"
#include "host/romfont.h"
#include "runtime/runtime.h"

#include <graphics/text.h>

#include <stdio.h>
#include <string.h>

using namespace amos;



namespace {

struct Font {
    TextFont font;
    char name[32];
    int users;
};

Font *fonts[16];

// A copy below 4GB
template <typename T> T *low_copy(const void *from, size_t n)
{
    T *to = static_cast<T *>(hal::allocate(uint32_t(n), 0x10000));
    if (to && from)
        memcpy(to, from, n);
    return to;
}

Font *font_from(const host::RomFont &r, uint16_t size);

// The topaz of size, from the ROM's (build/host/topaz.cpp), as a font of
// the host
Font *rom_font(uint16_t size)
{
    for (int i = 0; i < host::rom_font_count; i++) {
        const host::RomFont &r = host::rom_fonts[i];
        if (r.ysize != size)
            continue;
        return font_from(r, size);
    }
    // (a size the ROM does not have: topaz 8's characters in its height)
    for (int i = 0; i < host::rom_font_count; i++)
        if (host::rom_fonts[i].ysize == 8)
            return font_from(host::rom_fonts[i], size);
    return nullptr;
}

Font *font_from(const host::RomFont &r, uint16_t size)
{
    {
        Font *f = low_copy<Font>(nullptr, sizeof(Font));
        TextFont &t = f->font;
        t.tf_YSize = size;
        t.tf_Style = r.style;
        t.tf_Flags = r.flags;
        t.tf_XSize = r.xsize;
        t.tf_Baseline = r.baseline;
        t.tf_BoldSmear = r.boldsmear;
        t.tf_LoChar = r.lo;
        t.tf_HiChar = r.hi;
        t.tf_Modulo = r.modulo;
        int chars = r.hi - r.lo + 2;
        uint8_t *data = low_copy<uint8_t>(nullptr, size_t(r.modulo) * size);
        memcpy(data, r.data, size_t(r.modulo) * (size < r.ysize ? size : r.ysize));
        t.tf_CharData = data;
        t.tf_CharLoc = low_copy<ULONG>(r.locs, sizeof(ULONG) * size_t(chars));
        snprintf(f->name, sizeof f->name, "topaz.font");
        t.tf_Message.mn_Node.ln_Name = f->name;
        return f;
    }
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
            fprintf(stderr, "amos: built without a Kickstart's topaz: boxes instead\n");
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
