// The Compact extension (extensions/+Compact.s, slot 2): Pack and Spack
// of a screen's area into a bank, Unpack of it. The packer is the
// original's, byte for byte: a picture is cut into squares Pktcar lines
// high and one byte wide; a bit per byte says whether it differs from the
// one before (the bytes that do follow), and the bits are packed again
// the same way.

#include "core/guest_scratch.h"
#include "core/amos.h"
#include "lib/banks.h"

using namespace amos;

extern "C" {
// control.cpp
[[noreturn]] void interp_error(uint32_t n, uint32_t first, uint32_t extension, uint32_t messages);
}

namespace {

constexpr uint32_t ExtNb = 2 - 1;
constexpr uint32_t SCCode = 0x12031990, BMCode = 0x06071963;
constexpr uint16_t BitDble = 1 << (8 + 5);
constexpr uint16_t TSize[] = {1, 2, 3, 4, 5, 6, 7, 8, 12, 16, 24, 32, 48, 64};

// The extension's error messages
const char ErrMes[] = "Not a packed bitmap\0Not a packed screen";

[[noreturn]] void not_packed() { interp_error(0, 0, ExtNb, guest_constant(ErrMes, sizeof ErrMes)); }

// What Pack and Spack work on (PacPar): the screen and its bitplanes, the
// area in bytes across and lines down, the bank
struct Area {
    Screen *screen;
    be_u32 *planes;
    uint16_t dx, dy, tx, ty;
    uint32_t bank;
};

Area area(uint32_t s, uint32_t b, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    Area a;
    a.dx = uint16_t(x1) >> 3;
    a.dy = uint16_t(y1);
    uint16_t x = uint16_t(x2) >> 3, y = uint16_t(y2);
    Regs r;
    r.d[1] = s;
    lib_call(L_GetEc, r);
    a.screen = ptr<Screen>(r.a[0]);
    a.planes = ptr<be_u32>(r.d[0]);
    if (x > a.screen->EcTLigne)
        x = a.screen->EcTLigne;
    if (y > a.screen->EcTy)
        y = a.screen->EcTy;
    a.tx = uint16_t(x - a.dx);
    if (int16_t(a.tx) <= 0)
        fon_call();
    a.ty = uint16_t(y - a.dy);
    if (int16_t(a.ty) <= 0)
        fon_call();
    if (b >= 0x10000)
        fon_call();
    a.bank = b;
    return a;
}

// The packing of the area into the header at h, in squares car lines
// high (ty of them down). Without store, only the size it takes (the
// header is still written): PacSize.
uint32_t pack(const Area &a, uint16_t car, uint16_t ty, uint8_t *h, bool store)
{
    Screen *s = a.screen;
    auto header = reinterpret_cast<PacPictureHeader *>(h);
    if (store)
        header->Pkcode = BMCode;
    header->Pkdx = a.dx;
    header->Pkdy = a.dy;
    header->Pktx = a.tx;
    header->Pkty = ty;
    header->Pktcar = car;
    if (store)
        header->Pknplan = s->EcNPlan;
    // the bits: a byte for 8 of them (mulu: 16 bits at each step)
    uint32_t size = uint16_t(uint16_t(car) * a.tx) * uint32_t(ty);
    size = uint16_t(size) * uint32_t(s->EcNPlan);
    size = (size >> 3) + 2;
    // (the original did not clear them: the bytes not written, the first
    // one of PacSize and the last ones, went into the packing)
    Regs m;
    m.d[0] = size;
    m.a[0] = w::reserve(m.d[0], w::FastClear);
    uint8_t *bits = ptr<uint8_t>(m.a[0]);
    if (!bits)
        error(24);
    uint8_t *a6 = bits;
    uint8_t *a5 = h + PacPictureHeader::PkDatas1;
    uint16_t line = s->EcTLigne;
    uint32_t square_line = uint32_t(line) * car;
    uint32_t start = uint32_t(a.dy) * line + uint32_t(int16_t(a.dx));
    uint8_t last = 0;
    int bit = 7;
    if (store) {
        *a5 = 0;
        *a6 = 0;
    }
    for (uint16_t p = 0; p < s->EcNPlan; p++) {
        uint8_t *a3 = ptr<uint8_t>(a.planes[p]) + start;
        for (uint16_t y = 0; y < ty; y++) {
            uint8_t *a2 = a3;
            for (uint16_t x = 0; x < a.tx; x++) {
                uint8_t *a0 = a2;
                for (uint16_t k = 0; k < car; k++) {
                    if (*a0 != last) {
                        last = *a0;
                        a5++;
                        if (store)
                            *a5 = last;
                        *a6 |= uint8_t(1 << bit);
                    }
                    if (--bit < 0) {
                        bit = 7;
                        a6++;
                        *a6 = 0;
                    }
                    a0 += int16_t(line);
                }
                a2++;
            }
            a3 += square_line;
        }
    }
    // the bits packed the same way, byte after byte
    a5++;
    if (store)
        header->PkPoint2 = uint32_t(a5 - h);
    a6 = a5;
    a5 += int16_t(uint16_t((uint16_t(size) >> 3) + 2));
    if (store)
        header->PkDatas2 = uint32_t(a5 - h);
    last = 0;
    bit = 7;
    if (store) {
        *a5 = 0;
        *a6 = 0;
    }
    const uint8_t *from = bits;
    for (uint32_t n = uint32_t(uint16_t(size - 1)) + 1; n; n--) {
        uint8_t b = *from++;
        if (b != last) {
            last = b;
            a5++;
            if (store) {
                *a5 = last;
                *a6 |= uint8_t(1 << bit);
            }
        }
        // (PacSize only counts)
        if (store && --bit < 0) {
            bit = 7;
            a6++;
            *a6 = 0;
        }
    }
    Regs f;
    f.a[1] = address(bits);
    f.d[0] = size;
    w::release(f.a[1], f.d[0]);
    return (uint32_t(a5 - h) + 3) & ~1u;
}

// GetSize: the square height that packs smallest (the first of them);
// its size
struct Best {
    uint32_t size;
    uint16_t car, ty;
};

Best best_size(const Area &a)
{
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    Best best = {0x10000000, 0, 0};
    for (uint16_t car : TSize) {
        if (a.ty % car)
            continue;
        uint32_t size = pack(a, car, uint16_t(a.ty / car), buffer, false);
        if (size < best.size) {
            best.size = size;
            best.car = car;
        }
    }
    best.ty = uint16_t(a.ty / best.car);
    return best;
}

// ResBank: a data bank "Pac.Pic." of size bytes
uint8_t *reserve(uint32_t number, uint32_t size)
{
    uint8_t *bank = bank_reserve(number, BankData, size, "Pac.Pic.");
    if (!bank)
        error(24);
    return bank;
}

// UPack: the packed picture of bank b in screen s at x, y (negative: where
// it was packed), around the drawing of Autoback
void unpack(uint32_t b, Screen *s, uint32_t x, uint32_t y)
{
    Regs u;
    u.a[0] = bank_or_address(int32_t(b));
    u.a[1] = address(s);
    u.d[1] = x;
    u.d[2] = y;
    const Regs saved = u;
    auto unpack_bitmap = [&]() {
        Regs r = saved;
        lib_call(L_UnPack_Bitmap, r);
        return uint16_t(r.d[0]);
    };
    auto back = [](int function) {
        Regs r;
        ec_call(function, r);
    };
    if (!s->EcAuto) {
        if (!unpack_bitmap())
            not_packed();
        return;
    }
    back(ec::AutoBack1);
    uint16_t done;
    if (s->EcFlags & BitDble) {
        unpack_bitmap();
        back(ec::AutoBack2);
        done = unpack_bitmap();
        back(ec::AutoBack3);
    } else {
        done = unpack_bitmap();
        back(ec::AutoBack4);
    }
    if (!done)
        not_packed();
}

Screen *current()
{
    Screen *s = ptr<Screen>(idata().ScOnAd);
    if (!s)
        fon_call();
    return s;
}

} // namespace

extern "C" {

// PACK s TO b,x1,y1,x2,y2
void ext_pack6(uint32_t s, uint32_t b, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    Area a = area(s, b, x1, y1, x2, y2);
    Best best = best_size(a);
    pack(a, best.car, best.ty, reserve(a.bank, best.size), true);
}
// PACK s TO b
void ext_pack2(uint32_t s, uint32_t b) { ext_pack6(s, b, 0, 0, 10000, 10000); }

// SPACK s TO b,x1,y1,x2,y2: the screen's definition, then the picture
void ext_spack6(uint32_t s, uint32_t b, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    Area a = area(s, b, x1, y1, x2, y2);
    Best best = best_size(a);
    uint8_t *bank = reserve(a.bank, best.size + PacScreenHeader::PsLong);
    Screen *e = a.screen;
    auto h = reinterpret_cast<PacScreenHeader *>(bank);
    h->PsCode = SCCode;
    h->PsTx = e->EcTx;
    h->PsTy = e->EcTy;
    h->PsNbCol = e->EcNbCol;
    h->PsNPlan = e->EcNPlan;
    h->PsCon0 = e->EcCon0;
    h->PsAWx = e->EcAWX;
    h->PsAWy = e->EcAWY;
    h->PsAWTx = e->EcAWTX;
    h->PsAWTy = e->EcAWTY;
    h->PsAVx = e->EcAVX;
    h->PsAVy = e->EcAVY;
    for (int i = 0; i < 32; i++)
        h->PsPal[i] = e->EcPal[i];
    pack(a, best.car, best.ty, bank + PacScreenHeader::PsLong, true);
}
void ext_spack2(uint32_t s, uint32_t b) { ext_spack6(s, b, 0, 0, 10000, 10000); }

// UNPACK b: into the current screen, where it was packed
void ext_unpack1(uint32_t b) { unpack(b, current(), uint32_t(-1), uint32_t(-1)); }
// UNPACK b,x,y
void ext_unpack3(uint32_t b, uint32_t x, uint32_t y) { unpack(b, current(), x, y); }

// UNPACK b TO s: a packed screen opened as screen s
void ext_unpack2(uint32_t b, uint32_t s)
{
    Regs r;
    r.a[0] = bank_or_address(int32_t(b));
    r.d[0] = s;
    lib_call(L_UnPack_Screen, r);
    if (!uint16_t(r.d[0])) {
        if (!uint16_t(r.d[1]))
            not_packed();
        error(24);
    }
    Screen *e = ptr<Screen>(r.a[0]);
    IData &d = idata();
    d.ScOnAd = address(e);
    d.ScOn = uint16_t(e->EcNumber + 1);
}
}

// The extension's cold start (routine 0): d1 "APex" from AMOSPro 1.10 on.
// Out: d0 the extension's number, d1 (low word) its version; d0 = -1 and
// a0 = 0 for another version.
static uint32_t cold_start(RegFrame &r, uint32_t number, uint16_t version, void (*start)())
{
    if (r.d[1] != 0x41506578) {
        r.d[0] = uint32_t(-1);
        r.a[0] = 0;
        return r.d[0];
    }
    if (start)
        start();
    r.d[0] = number;
    r.d[1] = (r.d[1] & 0xFFFF0000) | version;
    return r.d[0];
}

extern "C" {
uint32_t ext_compact_cold(RegFrame &r) { return cold_start(r, ExtNb, 0x0110, nullptr); }
// Custom: error d0 of the extension, its messages; Custom2: with the
// messages in a0
uint32_t ext_compact_custom(RegFrame &r) { interp_error(r.d[0], 0, ExtNb, address(ErrMes)); }
uint32_t ext_compact_custom2(RegFrame &r) { interp_error(r.d[0], 0, ExtNb, r.a[0]); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_compact.inc"
#endif
