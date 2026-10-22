// Fonts of AMOS.library's screen table (EcCall): Get Fonts, Set Font and
// the font of a screen, and the screen state kept while a menu is shown.

#include "core/amiga.h"
#include "w/w.h"

#include <diskfont/diskfont.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#define GRAPHICS_BASE_NAME (reinterpret_cast<struct GfxBase *>(amos::wdata().GfxBase))
#include <proto/graphics.h>
#define DISKFONT_BASE_NAME (amos::ptr<struct Library>(amos::wdata().FntBase))
#include <proto/diskfont.h>

using namespace amos;
using namespace amos::w;

namespace {

struct RastPort *rastport() { return ptr<struct RastPort>(wdata().RastPort); }

// TFFonts: free the list of fonts
void free_fonts()
{
    WData &d = wdata();
    if (!d.FontInfos)
        return;
    free_mm(d.FontInfos, int16_t(d.FontILong));
    d.FontInfos = 0;
    d.FontILong = 0;
}

// TGFonts: the list of fonts of type d1 (AFF_MEMORY, AFF_DISK...) in
// T_FontInfos, an AvailFontsHeader. a1: 8 bytes to ask the size in.
// d0: 0, or -1 without diskfont.library or memory, or what AvailFonts
// still wanted.
void get_fonts(AllRegs *r)
{
    WData &d = wdata();
    uint16_t type = r->d[1];
    r->d[0] = 0xFFFFFFFF;
    // (The assembler opened the library again at each call)
    if (!d.FntBase)
        d.FntBase = address(OpenLibrary(const_cast<STRPTR>("diskfont.library"), 0));
    if (!d.FntBase)
        return;
    free_fonts();
    int32_t more = AvailFonts(ptr<char>(r->a[1]), 8, type);
    r->d[0] = more;
    if (!more)
        return;
    uint32_t size = more + 8;
    uint32_t buffer = fast_mm(size);
    if (!buffer) {
        r->d[0] = 0xFFFFFFFF;
        return;
    }
    d.FontInfos = buffer;
    d.FontILong = size;
    // Not enough after all: no list, and no error (TFFonts cleared d0)
    r->d[0] = 0;
    if (AvailFonts(ptr<char>(buffer), size, type))
        free_fonts();
}

// TGFont: a0 = font d1 of the list (an AvailFonts entry). d0: 0, -1 no
// list, 1 no such font.
uint32_t get_font(uint16_t n, uint32_t *entry)
{
    uint32_t list = wdata().FontInfos;
    if (!list)
        return 0xFFFFFFFF;
    if (!n || n > peek_w(ptr<uint8_t>(list)))
        return 1;
    *entry = list + 2 + (n - 1) * 10;
    return 0;
}

// CFont: back to the system font, if the screen has one of its own
void close_font()
{
    WData &d = wdata();
    Screen *s = ptr<Screen>(d.EcCourant);
    if (!s || !s->EcFontFlag)
        return;
    s->EcFontFlag = 0;
    CloseFont(ptr<struct TextFont>(peek_l(s->EcText)));
    SetFont(rastport(), ptr<struct TextFont>(d.DefaultFont));
}

// TSFont: font d1 of the list for the current screen (0: the system
// one). A disc font is taken from memory if it is there already (a font
// of the same height whose name starts with the one in memory).
uint32_t set_font(uint16_t n)
{
    close_font();
    if (!n)
        return 0;
    uint32_t entry_address;
    if (uint32_t error = get_font(n, &entry_address))
        return error;
    const uint8_t *entry = ptr<uint8_t>(entry_address);
    struct TextAttr *attr = ptr<struct TextAttr>(entry_address + 2);
    struct TextFont *font = nullptr;
    bool in_memory = peek_w(entry) == AFF_MEMORY;
    if (!in_memory) {
        uint8_t fonts[502];
        AvailFonts(reinterpret_cast<char *>(fonts), sizeof fonts, AFF_MEMORY);
        int16_t count = peek_w(fonts);
        for (const uint8_t *f = fonts + 2; --count >= 0 && !in_memory; f += 10) {
            if (peek_w(f + 6) != peek_w(entry + 6))
                continue;
            const uint8_t *a = ptr<uint8_t>(peek_l(f + 2)), *b = ptr<uint8_t>(peek_l(entry + 2));
            for (;;) {
                uint8_t c = *a++;
                if (!c) {
                    in_memory = true;
                    break;
                }
                if (c != *b++)
                    break;
            }
        }
    }
    font = in_memory ? OpenFont(attr) : OpenDiskFont(attr);
    if (!font)
        return 1;
    SetFont(rastport(), font);
    current_screen()->EcFontFlag++;
    return 0;
}

// StaMn: before a menu is drawn in the current screen: keep the graphic
// state and the zones, no clipping, JAM2 and outlines
void menu_start()
{
    WData &d = wdata();
    push_state();
    d.PaPeek = 1;
    Screen *s = current_screen();
    d.SaveZo = s->EcAZones;
    d.SaveNZo = s->EcNZones;
    s->EcAZones = 0;
    s->EcNZones = 0;
    set_clip_rect(0, 0, s->EcTx, s->EcTy);
    struct RastPort *rp = rastport();
    SetDrMd(rp, JAM2);
    rp->Flags |= AREAOUTLINE;
    rp->LinePtrn = 0xFFFF;
}

// StoMn: after it
void menu_stop()
{
    WData &d = wdata();
    Screen *s = current_screen();
    s->EcAZones = d.SaveZo;
    s->EcNZones = d.SaveNZo;
    d.PaPeek = 0;
    pull_state();
}

} // namespace

extern "C" {
void w_ec_get_fonts(AllRegs *r) { get_fonts(r); }

void w_ec_free_fonts(AllRegs *r)
{
    free_fonts();
    r->d[0] = 0;
}

// a0 the entry, d1 the number * 10
void w_ec_get_font(AllRegs *r)
{
    uint32_t entry;
    r->d[0] = get_font(r->d[1], &entry);
    if (!r->d[0]) {
        r->a[0] = entry;
        r->d[1] = uint16_t(r->d[1]) * 10;
    }
}

void w_ec_set_font(AllRegs *r) { r->d[0] = set_font(r->d[1]); }

void w_ec_close_font(AllRegs *) { close_font(); }
void w_ec_menu_start(AllRegs *) { menu_start(); }
void w_ec_menu_stop(AllRegs *) { menu_stop(); }
}
