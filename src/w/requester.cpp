// The system requesters (AutoRequest, EasyRequest, patched by AMOS.library):
// shown by AMOS on a screen of its own while AMOS is in front, else the
// system's, AMOS going to the back meanwhile. They come from whatever
// task needs one (a disc handler...).

#include "core/amiga.h"
#include "w/w.h"

#include <intuition/intuition.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

// +W.s: the palette (32 words) then the frame picture


namespace {

constexpr uint16_t EcReq = 11;
constexpr uint16_t ReqWY = 100;
constexpr uint32_t TTit = 0, TText = 80, TGad = TText + 320, TSize = TGad + 128;
constexpr uint16_t PicWidth = 112 / 8, PicHeight = 56, PicPlanes = 3;

const char title[] = "System request";
const uint8_t init[] = {27, 'C', '0', 27, 'V', '0', 27, 'Y', '1', 27, 'B', '2', 27, 'P', '7', 0};
const uint8_t main_text[] = {27, 'B', '2', 27, 'P', '3', 27, 'Y', '4', 0};
const uint8_t pos2[] = {27, 'Z', '1', 0};
const uint8_t neg2[] = {27, 'Z', '2', 0};

uint32_t sy(int function, Regs &r)
{
    sy_call(function, r);
    return r.d[0];
}

uint32_t sy(int function)
{
    Regs r;
    return sy(function, r);
}

void ec_d1(int function, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    ec_call(function, r);
}

void print(const void *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

void centre(const void *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Centre, r);
}

uint16_t length(uint32_t text)
{
    const volatile uint8_t *p = ptr<uint8_t>(text);
    uint16_t n = 0;
    while (*p++)
        n++;
    return n;
}

// The data of a requester: title, text lines, the gadgets
struct Texts {
    uint8_t *base;
    uint8_t *title() { return base + TTit; }
    uint8_t *text() { return base + TText; }
    uint8_t *gadgets() { return base + TGad; }
};

// IT_Print: the texts of a chain of IntuiTexts one after the other, each
// ending with end (if not 0) and a zero. Returns past them.
uint8_t *intuitexts(const struct IntuiText *it, uint8_t *to, uint8_t end)
{
    for (; it; it = it->NextText) {
        const uint8_t *s = reinterpret_cast<const uint8_t *>(it->IText);
        if (!s)
            continue;
        while ((*to++ = *s++))
            ;
        if (end) {
            to[-1] = end;
            *to++ = 0;
        }
    }
    return to;
}

// Format: RawDoFmt, counting the lines (T_TxtCy) and the widest (T_TxtMaxCx)
struct Formatter {
    uint8_t *to;
};

void put_char(uint8_t c, Formatter *f)
{
    WData &d = wdata();
    d.TxtCx++;
    *f->to++ = c;
    if (c == 10) {
        *f->to++ = 0;
        d.TxtCy++;
        if (d.TxtCx > d.TxtMaxCx)
            d.TxtMaxCx = d.TxtCx;
        d.TxtCx = 0;
    }
    *f->to = 0;
}

// Returns the next argument
const void *format(const char *text, uint8_t *to, const void *args)
{
    WData &d = wdata();
    d.TxtCx = 0;
    d.TxtCy = 1;
    Formatter f = {to};
    const void *next = hal::raw_format(
        text, args, [](char c, void *data) { put_char(uint8_t(c), static_cast<Formatter *>(data)); }, &f);
    if (d.TxtCx > d.TxtMaxCx)
        d.TxtMaxCx = d.TxtCx;
    return next;
}

// ---- the AMOS requester ----------------------------------------------------------

// Copy: a piece of the frame picture into the requester's screen; x moves on
uint16_t piece(uint16_t from_x, uint16_t from_y, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    WData &d = wdata();
    const uint8_t *pic = AMOS_DATA(w_req_pal) + 32 * 2 + uint32_t(from_y) * PicWidth + from_x;
    const amos::Screen *s = ptr<amos::Screen>(d.ScAdr);
    uint32_t line = int16_t(d.Req_Sx);
    uint32_t offset = uint32_t(y) * line + x;
    for (uint16_t p = 0; p < PicPlanes; p++, pic += PicWidth * PicHeight) {
        const uint8_t *from = pic;
        uint8_t *to = ptr<uint8_t>(s->EcLogic[p]) + offset;
        for (uint16_t row = 0; row < height; row++, from += PicWidth, to += line)
            for (uint16_t i = 0; i < width; i++)
                to[i] = from[i];
    }
    return x + width;
}

// PrtZone: gadget zone (1 left, 2 right) printed, highlighted ('1') or not
void print_zone(uint16_t zone, uint8_t highlight, uint8_t neg_column)
{
    WData &d = wdata();
    if (!zone)
        return;
    if (zone == 1) {
        if (!d.Req_Pos)
            return;
        uint8_t pos1[] = {27, 'B', '4', 27, 'P', '3', 27, 'I', highlight, 24, 30, 30, 27, 'X', '2', 27, 'Z', '0', 0};
        print(pos1);
        print(ptr<uint8_t>(d.Req_Pos));
        print(pos2);
    } else {
        if (!d.Req_Neg)
            return;
        uint8_t neg1[] = {27, 'B', '4', 27, 'P', '3', 27, 'I', highlight, 24, 30, 30, 27, 'X', neg_column, 27, 'Z', '0', 0};
        print(neg1);
        print(ptr<uint8_t>(d.Req_Neg));
        print(neg2);
    }
}

// AppCentre: the screen opens or closes from its middle, by step lines
void appear(amos::Screen *s, uint16_t middle, int16_t height, int16_t step)
{
    for (;;) {
        int16_t was = height;
        s->EcAWTY = 2 * height;
        reinterpret_cast<uint8_t *>(&s->EcAWT)[0] |= 4;
        s->EcAVY = (s->EcTy >> 1) - height;
        reinterpret_cast<uint8_t *>(&s->EcAV)[0] |= 4;
        s->EcAWY = middle - height;
        reinterpret_cast<uint8_t *>(&s->EcAW)[0] |= 4;
        sy(sy::WaitVbl);
        Regs f;
        ec_call(ec::CopForce, f);
        height += step;
        if (height < 0)
            height = 0;
        uint16_t half = s->EcTy >> 1;
        if (uint16_t(height) >= half)
            height = half;
        if (height == was)
            return;
    }
}

// UnMix1 / UnMix2: the requester's screen in front, multitasking off
void unmix_on()
{
    hal::forbid();
    Regs c;
    ec_call(ec::Current, c);
    wdata().ReqOldScreen = ptr<amos::Screen>(c.a[0])->EcNumber;
    ec_d1(ec::Active, EcReq);
    ec_d1(ec::First, EcReq);
    Regs m;
    ec_call(ec::CopMake, m);
    sy(sy::WaitVbl);
}

void unmix_off()
{
    ec_d1(ec::Active, wdata().ReqOldScreen);
    hal::permit();
}

// Req_AMOS: -1 for the left gadget (Return, Amiga-V), 0 for the right one
// (Escape, Amiga-B)
uint32_t amos_requester(Texts t)
{
    WData &d = wdata();
    d.Req_On = 0xFFFF;
    sy(sy::AMALFrz);
    unmix_on();
    uint32_t answer = 0;
    Regs o;
    o.d[1] = EcReq;
    o.d[2] = int32_t(int16_t(d.Req_Sx << 3));
    o.d[3] = int32_t(int16_t(d.Req_Sy << 3));
    o.d[4] = 3;
    o.d[5] = 0x8000;
    o.d[6] = 8;
    o.d[7] = 0;
    o.a[1] = address(AMOS_DATA(w_req_pal));
    ec_call(ec::Cree, o);
    if (!o.d[0]) {
        d.ScAdr = o.a[0];
        print(init);

        // The frame
        uint16_t sx = d.Req_Sx, x = 0, y = 0;
        x = piece(0, 0, 0, 0, 4, 3 * 8);
        for (uint16_t n = sx;;) {
            x = piece(4, 0, x, 0, 1, 3 * 8);
            if (--n < 9)
                break;
        }
        piece(10, 0, x, 0, 4, 3 * 8);
        y = 3 * 8;
        uint16_t rows = d.Req_Sy - 6;
        do {
            x = piece(0, 3 * 8, 0, y, 2, 12);
            for (uint16_t n = sx;;) {
                x = piece(6, 3 * 8, x, y, 1, 12);
                if (--n < 5)
                    break;
            }
            piece(12, 3 * 8, x, y, 2, 12);
            y += 8;
        } while (--rows);
        y += 4;
        x = piece(0, 4 * 8 + 4, 0, y, 1, 2 * 8 + 4);
        for (uint16_t n = sx;;) {
            x = piece(6, 4 * 8 + 4, x, y, 1, 2 * 8 + 4);
            if (--n < 3)
                break;
        }
        piece(13, 4 * 8 + 4, x, y, 1, 2 * 8 + 4);
        // Under the gadgets
        int16_t n = length(d.Req_Pos) + 1;
        x = piece(1, 4 * 8 + 4, 1, y, 2, 2 * 8);
        for (int16_t i = n - 4; i >= 0; i--)
            x = piece(3, 4 * 8 + 4, x, y, 1, 2 * 8);
        piece(4, 4 * 8 + 4, x, y, 2, 2 * 8);
        uint8_t neg_column = '0';
        if (d.Req_Neg) {
            n = length(d.Req_Neg) + 1;
            neg_column = uint8_t(sx - n + 48 - 1);
            x = piece(8, 4 * 8 + 4, sx - n - 2, y, 2, 2 * 8);
            for (int16_t i = n - 4; i >= 0; i--)
                x = piece(10, 4 * 8 + 4, x, y, 1, 2 * 8);
            piece(11, 4 * 8 + 4, x, y, 2, 2 * 8);
        }

        Regs z;
        z.d[1] = 2;
        sy(sy::ResZone, z);
        d.Req_Sy <<= 3;
        centre(t.title());
        print(main_text);
        const uint8_t *line = t.text();
        do {
            centre(line);
            while (*line++)
                ;
        } while (*line);
        print_zone(1, '0', neg_column);
        if (d.Req_Neg)
            print_zone(2, '0', neg_column);

        amos::Screen *s = ptr<amos::Screen>(d.ScAdr);
        s->EcAWX = 288 - (d.Req_Sx << 1);
        reinterpret_cast<uint8_t *>(&s->EcAW)[0] |= 2;
        uint16_t middle = (d.Req_Sy >> 1) + ReqWY;
        appear(s, middle, 1, 8);
        d.DOld = sy(sy::GetDisc);

        // Until a key or a click
        uint16_t over = 8;
        for (;;) {
            unmix_off();
            hal::wait_frame();
            unmix_on();
            if (d.Req_IDCMP & IDCMP_DISKINSERTED) {
                uint16_t disc = sy(sy::GetDisc);
                if (disc != d.DOld) {
                    d.DOld = disc;
                    if (disc) {
                        answer = 0xFFFFFFFF;
                        break;
                    }
                }
            }
            Regs k;
            sy(sy::Inkey, k);
            uint16_t ascii = k.d[1];
            if (ascii == 13) {
                answer = 0xFFFFFFFF;
                break;
            }
            if (ascii == 27)
                break;
            uint16_t keys = k.d[1] >> 16;
            if (keys & 0xC000) {
                if (uint8_t(keys) == 0x34) {
                    answer = 0xFFFFFFFF;
                    break;
                }
                if (uint8_t(keys) == 0x35)
                    break;
            }
            Regs g;
            sy(sy::GetZone, g);
            uint16_t zone = uint16_t(g.d[1]) == EcReq ? g.d[1] >> 16 : 0;
            if (zone != over) {
                uint16_t old = over;
                over = zone;
                print_zone(old, '0', neg_column);
            }
            print_zone(over, '1', neg_column);
            if (!over)
                continue;
            Regs m;
            sy(sy::MouseKey, m);
            if (!uint16_t(m.d[1]))
                continue;
            if (over != 2)
                answer = 0xFFFFFFFF;
            break;
        }
        appear(s, middle, s->EcTy >> 1, -8);
        ec_d1(ec::Del, EcReq);
    }
    unmix_off();
    free_mm(address(t.base), TSize);
    sy(sy::AMALUFrz);
    d.Req_Pos = answer;
    d.Req_On = 0;
    return answer;
}

// Req_In: 1 the system's requester, -1 AMOS's, 0 none (always "no")
int which()
{
    WData &d = wdata();
    if (d.AMOSHere & 0xFF)
        return 0;               // AMOS and the Workbench are swapping
    int16_t flag = d.ReqFlag;
    if (!flag)
        return 0;
    if (flag > 0 && !idata().WB_Closed)
        return 1;
    if (!(d.AMOSHere >> 8))
        return 1;
    if (d.Req_On)
        return 0;
    return -1;
}

// Req_WB: the system's requester (the function patched over), AMOS behind
// it meanwhile
uint32_t system_requester(AllRegs *r, uint32_t function)
{
    WData &d = wdata();
    d.ScAdr = function;
    uint16_t here = int16_t(int8_t(d.AMOSHere >> 8));
    d.ReqOld = here;
    if (here)
        ec_d1(ec::AMOS_WB, 0);
    AllRegs a = *r;
    amos_call_all(&a, host::decode_routine(function));
    d.Req_Pos = a.d[0];
    if (d.ReqOld)
        ec_d1(ec::AMOS_WB, 1);
    return a.d[0];
}

Texts new_texts() { return {ptr<uint8_t>(fast_mm(TSize))}; }

} // namespace

extern "C" {
// AutoRequest(window a0, body a1, positive a2, negative a3, d2/d3 size)
void w_auto_request(AllRegs *r)
{
    int w = which();
    if (!w) {
        r->d[0] = 0;
        return;
    }
    if (w > 0) {
        r->d[0] = system_requester(r, wdata().PrevAuto);
        return;
    }
    WData &d = wdata();
    Texts t = new_texts();
    d.Req_IDCMP = 0x8000;
    d.Req_Sx = (uint16_t(uint16_t(r->d[2]) >> 3) + 3) & 0xFFFC;
    d.Req_Sy = (uint16_t(r->d[3]) >> 3) + 2;
    intuitexts(ptr<struct IntuiText>(r->a[1]), t.text(), 10);
    d.Req_Pos = 0;
    if (r->a[2]) {
        d.Req_Pos = address(t.gadgets());
        intuitexts(ptr<struct IntuiText>(r->a[2]), t.gadgets(), 0);
    }
    d.Req_Neg = 0;
    if (r->a[3]) {
        d.Req_Neg = address(t.gadgets() + 64);
        intuitexts(ptr<struct IntuiText>(r->a[3]), t.gadgets() + 64, 0);
    }
    for (unsigned i = 0; i < sizeof title; i++)
        t.title()[i] = title[i];
    r->d[0] = amos_requester(t);
}

// EasyRequestArgs(window a0, easy a1, IDCMP a2, arguments a3)
void w_easy_request(AllRegs *r)
{
    int w = which();
    if (!w) {
        r->d[0] = 0;
        return;
    }
    if (w > 0) {
        r->d[0] = system_requester(r, wdata().PrevEasy);
        return;
    }
    WData &d = wdata();
    Texts t = new_texts();
    d.Req_IDCMP = peek_l(ptr<uint8_t>(r->a[2]));
    const struct EasyStruct *easy = ptr<struct EasyStruct>(r->a[1]);
    d.TxtMaxCx = 0;
    const void *args = ptr<void>(r->a[3]);
    const char *t1 = reinterpret_cast<const char *>(easy->es_Title);
    args = format(t1 ? t1 : title, t.title(), args);
    args = format(reinterpret_cast<const char *>(easy->es_TextFormat), t.text(), args);
    d.Req_Sy = d.TxtCy;
    format(reinterpret_cast<const char *>(easy->es_GadgetFormat), t.gadgets(), args);
    d.Req_Pos = address(t.gadgets());
    d.Req_Neg = 0;
    for (uint8_t *g = t.gadgets(); *g; g++)
        if (*g == '|') {
            *g = 0;
            d.Req_Neg = address(g + 1);
            break;
        }
    uint16_t width = d.TxtMaxCx < 32 ? 32 : d.TxtMaxCx;
    d.Req_Sx = (width + 9) & 0xFFFC;
    d.Req_Sy += 8;
    r->d[0] = amos_requester(t);
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/w_requester.inc"
#endif
