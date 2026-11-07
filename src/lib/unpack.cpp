// The unpacker of packed pictures (the Compact extension's Pack and Spack,
// the resource banks, the editor's and the monitor's graphics) and the
// Screen Copy entry of the editor: the end of Dialog.s in AMOSPro.Lib

#include "core/guest_scratch.h"
#include "core/amos.h"

using namespace amos;

// AMOS's screen (intuition.library has a struct Screen too)
using EcScreen = amos::Screen;

// screens.cpp
extern "C" uint32_t lib_sco0(RegFrame &r);

namespace {

constexpr uint32_t SCCode = 0x12031990;    // a screen header (Spack)
constexpr uint32_t BMCode = 0x06071963;    // a packed bitmap

// UnPack_Bitmap: unpack picture (or the bitmap after a screen header) into
// screen at byte x / line y (negative: where it was packed). Returns the
// size, height << 16 | width in pixels, or 0 if it is no packed bitmap
// or does not fit.
//
// The picture is cut in "squares" of Pktcar lines and one byte across.
// Bytes are stored only when they change: a bit of the bit table (Datas2)
// for each byte, which itself is stored only when a bit of the pointer
// table (Point2) says so.
uint32_t unpack_bitmap(const uint8_t *picture, EcScreen *screen, int32_t x, int32_t y)
{
    if (peek_l(picture) == SCCode)
        picture += PacScreenHeader::PsLong;
    auto header = reinterpret_cast<const PacPictureHeader *>(picture);
    if (header->Pkcode != BMCode)
        return 0;
    uint16_t line = screen->EcTLigne;
    uint16_t planes = screen->EcNPlan;
    if (planes != header->Pknplan)
        return 0;
    uint16_t square = header->Pktcar;
    uint16_t bx = x < 0 ? header->Pkdx : static_cast<uint16_t>(static_cast<uint16_t>(x) >> 3);
    uint16_t by = y < 0 ? header->Pkdy : static_cast<uint16_t>(y);
    uint16_t width = header->Pktx;
    if (static_cast<uint16_t>(width + bx) > line)
        return 0;
    uint16_t height = static_cast<uint16_t>(header->Pkty * square);
    if (static_cast<uint16_t>(height + by) > screen->EcTy)
        return 0;
    uint32_t offset = static_cast<uint32_t>(by) * line + static_cast<int16_t>(bx);
    // Added to an address register as a word, as in the assembler
    int16_t square_line = static_cast<int16_t>(square * line);

    const uint8_t *bytes = picture + PacPictureHeader::PkDatas1;
    const uint8_t *bits = picture + header->PkDatas2;
    const uint8_t *pointers = picture + header->PkPoint2;
    int bit = 7, pointer_bit = 7;
    uint8_t mask = *bits++;
    uint8_t value = *bytes++;
    if (*pointers >> pointer_bit & 1)
        mask = *bits++;
    pointer_bit--;

    for (uint16_t p = 0; p < planes; p++) {
        uint8_t *row = ptr<uint8_t>(screen->EcCurrent[p]) + offset;
        uint16_t rows = header->Pkty;
        do {
            uint8_t *column = row;
            uint16_t columns = static_cast<uint16_t>(width - 1);
            do {
                uint8_t *to = column;
                for (int16_t n = static_cast<int16_t>(square); --n >= 0;) {
                    if (mask >> bit & 1)
                        value = *bytes++;
                    *to = value;
                    to += static_cast<int16_t>(line);
                    if (bit-- == 0) {
                        bit = 7;
                        if (*pointers >> pointer_bit & 1)
                            mask = *bits++;
                        if (pointer_bit-- == 0) {
                            pointer_bit = 7;
                            pointers++;
                        }
                    }
                }
                column++;
            } while (columns-- != 0);
            row += square_line;
        } while (--rows != 0);
    }
    return static_cast<uint32_t>(height) << 16 | static_cast<uint16_t>(width << 3);
}

} // namespace

// UnPack_Bitmap: a0 = packed picture, a1 = screen, d1/d2 = x/y (negative:
// where it was packed). Out: d0 = height << 16 | width, or 0 (BEQ).
extern "C" uint32_t lib_unpack_bitmap(RegFrame &r)
{
    r.d[0] = unpack_bitmap(ptr<uint8_t>(r.a[0]), ptr<EcScreen>(r.a[1]), static_cast<int32_t>(r.d[1]),
                           static_cast<int32_t>(r.d[2]));
    return static_cast<uint16_t>(r.d[0]);
}

// UnPack_Screen: a0 = packed screen (Spack), d0 = screen number. Opens the
// screen and unpacks into it. Out: a0 = the screen, d0 = the size, or
// d0 = 0 with d1 = 0 (not a packed screen, or it does not fit) or 1 (the
// screen could not be opened).
extern "C" uint32_t lib_unpack_screen(RegFrame &r)
{
    auto header = ptr<PacScreenHeader>(r.a[0]);
    if (header->PsCode != SCCode) {
        r.d[0] = r.d[1] = 0;
        return 0;
    }
    Regs c = {};
    c.d[1] = static_cast<uint16_t>(r.d[0]);
    c.d[2] = header->PsTx;
    c.d[3] = header->PsTy;
    c.d[4] = header->PsNPlan;
    c.d[5] = header->PsCon0;
    c.d[6] = header->PsNbCol;
    c.a[1] = address(header->PsPal);
    ec_call(ec::Cree, c);
    if (c.d[0]) {
        r.d[0] = 0;
        r.d[1] = 1;
        return 0;
    }
    EcScreen *s = ptr<EcScreen>(c.a[0]);
    // Cursor off
    Regs w = {};
    w.a[1] = guest_text("\x1b" "C0");
    wi_call(wi::Print, w);
    s->EcAWX = header->PsAWx;
    s->EcAWY = header->PsAWy;
    s->EcAWTX = header->PsAWTx;
    s->EcAWTY = header->PsAWTy;
    s->EcAVX = header->PsAVx;
    s->EcAVY = header->PsAVy;
    // The flag bytes of the view changes (the high bytes of the words)
    reinterpret_cast<uint8_t *>(&s->EcAW)[0] = 0b110;
    reinterpret_cast<uint8_t *>(&s->EcAWT)[0] = 0b110;
    reinterpret_cast<uint8_t *>(&s->EcAV)[0] = 0b110;
    r.d[0] = unpack_bitmap(reinterpret_cast<const uint8_t *>(header + 1), s, 0, 0);
    r.d[1] = 0;
    r.a[0] = address(s);
    return r.d[0];
}

// ScCopy: Screen Copy for the editor. d0/d1 = x1/y1, d4/d5 = x2/y2 of the
// source, d2/d3 = destination, a0/a1 = the screens.
extern "C" uint32_t lib_sc_copy(RegFrame &r)
{
    RegFrame copy = r;
    EcScreen *from = ptr<EcScreen>(r.a[0]), *to = ptr<EcScreen>(r.a[1]);
    idata().SccEcO = address(from->EcCurrent);
    // The assembler gave the screen itself: its first field, EcLogic
    idata().SccEcD = address(to->EcLogic);
    copy.d[6] = 0xCC;
    lib_sco0(copy);
    return r.d[0];
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_unpack.inc"
#endif
