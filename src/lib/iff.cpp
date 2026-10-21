// IFF pictures and animations: Load Iff, Save Iff, the Form functions,
// Iff Anim (Iff* and Save* in +Lib.s)

#include "core/amiga.h"
#include "lib/banks.h"
#include "lib/dos.h"
#include "lib/strings.h"

#define DOS_BASE_NAME (reinterpret_cast<struct DosLibrary *>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;

// files.cpp
extern "C" uint32_t lib_get_file(RegFrame &r);

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t id(const char (&s)[5])
{
    return static_cast<uint32_t>(static_cast<uint8_t>(s[0])) << 24 | static_cast<uint32_t>(static_cast<uint8_t>(s[1])) << 16 |
           static_cast<uint32_t>(static_cast<uint8_t>(s[2])) << 8 | static_cast<uint8_t>(s[3]);
}

constexpr int32_t OffsetBeginning = -1, OffsetCurrent = 0;

// The chunks AMOS knows, in the order of the bits of IffFlag and IffMask
enum Chunk { BMHD, CAMG, CMAP, CCRT, BODY, AMSC, ANHD, DLTA, ChunkCount };
constexpr uint32_t ChunkIds[ChunkCount] = {id("BMHD"), id("CAMG"), id("CMAP"), id("CCRT"),
                                           id("BODY"), id("AMSC"), id("ANHD"), id("DLTA")};

[[noreturn]] void bad_iff() { lib_jump(L_IffFor, 0); }

uint32_t even(uint32_t n) { return (n + 1) & ~1u; }

// ---- reading forms ------------------------------------------------------

// IffRead / IffSeek on the file handle (d5 in the assembler)
int32_t iff_read(BPTR file, void *to, int32_t length)
{
    int32_t n = Read(file, to, length);
    if (n < 0)
        disk_error();
    return n;
}

// IffFormLoad: up to count FORMs (not counting ANIM headers) into to,
// ended by "AenD". Returns how many were read.
uint32_t form_load(BPTR file, uint8_t *to, uint32_t count)
{
    uint8_t *header = ptr<uint8_t>(idata().Buffer);
    uint32_t loaded = 0;
    do {
        int32_t n = iff_read(file, header, 12);
        if (!n)
            break;
        if (n != 12)
            disk_error();
        uint32_t form = peek_l(header), size = peek_l(header + 4), type = peek_l(header + 8);
        if (form != id("FORM"))
            bad_iff();
        if (type == id("ANIM"))
            continue;
        uint32_t rest = even(size) - 4;
        if (to) {
            for (int i = 0; i < 12; i++)
                to[i] = header[i];
            to += 12;
            if (static_cast<uint32_t>(iff_read(file, to, rest)) != rest)
                disk_error();
            to += rest;
        } else {
            Seek(file, rest, OffsetCurrent);
        }
        loaded++;
    } while (--count);
    if (to)
        poke_l(to, id("AenD"));
    return loaded;
}

// IffFormSize: the memory count FORMs take, and how many there are; the
// file position does not change
uint32_t form_size(BPTR file, uint32_t count, uint32_t *forms = nullptr)
{
    uint8_t *header = ptr<uint8_t>(idata().Buffer);
    int32_t back = 0;
    uint32_t size = 0, found = 0;
    for (;;) {
        if (!iff_read(file, header, 12))
            break;
        if (peek_l(header) != id("FORM"))
            bad_iff();
        back -= 12;
        if (peek_l(header + 8) == id("ANIM"))
            continue;
        size += 12;
        found++;
        uint32_t rest = even(peek_l(header + 4)) - 4;
        size += rest;
        if (!--count)
            break;
        back -= rest;
        Seek(file, rest, OffsetCurrent);
    }
    Seek(file, back, OffsetCurrent);
    if (forms)
        *forms = found;
    return size + 4;
}

// ---- playing forms ------------------------------------------------------

Screen *current_screen()
{
    uint16_t n = idata().ScOn;
    if (!n)
        lib_jump(L_ScNOp, 0);
    Regs r = {};
    r.d[1] = n - 1u;
    ec_call(ec::Active, r);
    return ptr<Screen>(r.a[0]);
}

[[noreturn]] void ec_wi_error(uint32_t code) { lib_jump(L_EcWiErr, code); }

// IffScreen: open screen IffParam as BMHD/CAMG describe. False without
// a BMHD.
bool iff_screen(Regs &r)
{
    IData &d = idata();
    if (!(d.IffFlag & 1 << BMHD))
        return false;
    const uint8_t *bmhd = ptr<uint8_t>(d.BufBMHD);
    r.d[2] = static_cast<int16_t>((peek_w(bmhd) + 15) & 0xFFF0);
    r.d[3] = static_cast<int16_t>(peek_w(bmhd + 2));
    int16_t planes = static_cast<int8_t>(bmhd[8]);
    r.d[4] = planes;
    uint16_t colours = 2;
    for (uint16_t p = planes; --p;)
        colours <<= 1;
    r.d[6] = colours;
    uint16_t mode = 0;
    if (peek_w(bmhd + 16) >= 640 && planes <= 4)
        mode |= 0x8000;                             // hires
    if (peek_w(bmhd + 18) >= 400)
        mode |= 4;                                  // interlaced
    if (d.IffFlag & 1 << CAMG) {
        uint32_t camg = peek_l(ptr<void>(d.BufCAMG));
        mode = 0;
        if (camg & 1 << 11) {                       // HAM
            r.d[4] = 6;
            mode = 0x0800;
            r.d[6] = 64;
        }
        mode |= camg & 0x8004;
    }
    r.d[5] = mode;
    return true;
}

// IffCentre: the display position of an AMSC chunk
void iff_centre()
{
    Screen *s = current_screen();
    IData &d = idata();
    if (!(d.IffFlag & 1 << AMSC))
        return;
    const uint8_t *amsc = ptr<uint8_t>(d.BufAMSC);
    s->EcAWX = peek_w(amsc);
    s->EcAWY = peek_w(amsc + 2);
    s->EcAWTX = peek_w(amsc + 4);
    s->EcAWTY = peek_w(amsc + 6);
    s->EcAVX = peek_w(amsc + 8);
    s->EcAVY = peek_w(amsc + 10);
    s->EcFlags = peek_w(amsc + 12);
    // byte writes into the flag words, as in the assembler
    reinterpret_cast<uint8_t *>(&s->EcAW)[0] = 6;
    reinterpret_cast<uint8_t *>(&s->EcAWT)[0] = 6;
    reinterpret_cast<uint8_t *>(&s->EcAV)[0] = 6;
    wdata().Actualise |= 1 << 12;                   // BitEcrans
}

// IffPal: the palette of the CMAP chunk over the default one
void iff_palette()
{
    IData &d = idata();
    uint16_t *pal = ptr<uint16_t>(d.Buffer);
    for (int i = 0; i < 32; i++)
        pal[i] = d.DefPal[i];
    if (d.IffFlag & 1 << CMAP) {
        const uint8_t *cmap = ptr<uint8_t>(d.BufCMAP);
        uint16_t count = static_cast<uint16_t>(peek_l(cmap + 4) / 3);
        // (the assembler's dbra loop wrapped around on an empty CMAP)
        if (!count)
            count = 1;
        const uint8_t *rgb = cmap + 8;
        uint16_t *to = pal;
        // a dbra loop on the count minus one
        for (uint16_t i = count - 1;; i--) {
            uint16_t r = rgb[0] & 0xF0, g = rgb[1] >> 4, b = rgb[2] >> 4;
            rgb += 3;
            *to++ = static_cast<uint16_t>(((r | g) << 4) | b);
            if (!i)
                break;
        }
    }
    Regs r = {};
    r.a[1] = address(pal);
    ec_call(ec::SPal, r);
}

// IffShift: colour cycling of a CCRT chunk
void iff_shift()
{
    IData &d = idata();
    if (!(d.IffFlag & 1 << CCRT))
        return;
    const uint8_t *ccrt = ptr<uint8_t>(d.BufCCRT);
    int16_t direction = static_cast<int16_t>(peek_w(ccrt));
    if (!direction)
        return;
    if (direction < 0)
        direction = 0;
    int8_t start = static_cast<int8_t>(ccrt[2]), end = static_cast<int8_t>(ccrt[3]);
    if (start < 0 || end < 0 || start >= end)
        return;
    // microseconds -> VBLs, with divu: on an overflow the value stays
    uint32_t speed = peek_l(ccrt + 8);
    if (speed / 20 <= 0xFFFF)
        speed = (speed % 20) << 16 | speed / 20;
    if (!static_cast<uint16_t>(speed))
        return;
    Regs r = {};
    r.d[1] = 1;
    r.d[2] = speed;
    r.d[3] = start;
    r.d[4] = end;
    r.d[5] = static_cast<uint16_t>(direction);
    r.d[6] = 1;
    ec_call(ec::Shift, r);
    if (r.d[0])
        ec_wi_error(r.d[0]);
}

// The BODY chunk: the picture, in IffParam's new screen or the current one
void iff_body(const uint8_t *data)
{
    IData &d = idata();
    if (d.IffParam != EntNul) {
        Regs r = {};
        if (!iff_screen(r))
            bad_iff();
        r.d[1] = d.IffParam;
        if (r.d[1] >= 8)
            lib_jump(L_IllScN, 0);
        r.a[1] = address(d.DefPal);
        ec_call(ec::Cree, r);
        if (r.d[0])
            ec_wi_error(r.d[0]);
        Screen *s = ptr<Screen>(r.a[0]);
        d.ScOnAd = address(s);
        d.ScOn = s->EcNumber + 1;
        iff_centre();
    }
    if (!d.ScOn)
        lib_jump(L_ScNOp, 0);
    iff_palette();
    iff_shift();
    Screen *s = current_screen();
    if (!(d.IffFlag & 1 << BMHD))
        fon_call();
    const uint8_t *bmhd = ptr<uint8_t>(d.BufBMHD);
    uint16_t width = peek_w(bmhd);
    if (width > s->EcTx)
        lib_jump(L_CantFit, 0);
    uint16_t height = peek_w(bmhd + 2);
    if (height > s->EcTy)
        height = s->EcTy;
    int16_t planes = static_cast<int8_t>(bmhd[8]);
    if (static_cast<uint16_t>(planes) > s->EcNPlan)
        lib_jump(L_CantFit, 0);
    uint16_t row = (width + 7) >> 3;
    // cursor off
    Regs c = {};
    c.a[1] = address("\x1b" "C0");
    wi_call(wi::Print, c);
    uint8_t compression = bmhd[10];
    if (!compression) {
        uint16_t words = row >> 1;
        uint32_t offset = 0;
        for (uint16_t y = 0; y < height; y++, offset += static_cast<int16_t>(s->EcTLigne)) {
            for (int16_t p = 0; p < planes; p++) {
                uint8_t *to = ptr<uint8_t>(s->EcLogic[p]) + offset;
                for (uint16_t w = 0; w < words; w++, to += 2, data += 2)
                    poke_w(to, peek_w(data));
            }
        }
        return;
    }
    if (compression != 1)
        lib_jump(L_IffCmp, 0);
    // ByteRun1
    uint32_t offset = 0;
    for (uint16_t y = 0; y < height; y++, offset += static_cast<int16_t>(s->EcTLigne)) {
        for (int16_t p = 0; p < planes; p++) {
            uint8_t *to = ptr<uint8_t>(s->EcLogic[p]) + offset;
            uint16_t done = 0;
            do {
                int8_t n = static_cast<int8_t>(*data++);
                if (n >= 0) {
                    done += n + 1;
                    for (int i = 0; i <= n; i++)
                        *to++ = *data++;
                } else if (n != -128) {
                    uint8_t v = *data++;
                    done += -n + 1;
                    for (int i = 0; i <= -n; i++)
                        *to++ = v;
                }
            } while (done < row);
        }
    }
}

// The columns of one plane of a DLTA chunk (by Jim Kent)
void decode_plane(const uint8_t *src, uint8_t *plane, uint32_t line, uint32_t columns)
{
    for (uint32_t col = 0; col < columns; col++, plane++) {
        uint8_t *to = plane;
        for (uint8_t ops = *src++; ops; ops--) {
            uint8_t op = *src++;
            if (op & 0x80) {
                for (op &= 0x7F; op; op--, to += static_cast<int16_t>(line))
                    *to = *src++;
            } else if (!op) {
                uint8_t count = *src++, value = *src++;
                for (; count; count--, to += static_cast<int16_t>(line))
                    *to = value;
            } else {
                to += op * line;
            }
        }
    }
}

// The DLTA chunk: an ANIM frame (operation 5 only) into the current screen
void iff_delta(const uint8_t *data)
{
    IData &d = idata();
    if (!(d.IffFlag & 1 << ANHD))
        bad_iff();
    const uint8_t *anhd = ptr<uint8_t>(d.BufAMSC);
    if (anhd[0] != 5)
        bad_iff();
    if (!d.ScOn)
        lib_jump(L_ScNOp, 0);
    Screen *s = ptr<Screen>(wdata().EcCourant);
    uint32_t line = static_cast<int16_t>(s->EcTLigne);
    d.IffReturn = peek_l(anhd + 14);
    for (uint16_t p = 0; p < s->EcNPlan; p++) {
        uint32_t offset = peek_l(data + 4 * p);
        if (offset)
            decode_plane(data + offset, ptr<uint8_t>(s->EcLogic[p]), line, line);
    }
}

// IffFormPlay: the chunks of count FORMs at form (bit 30 of count: skip
// them). Returns where it stopped.
uint8_t *form_play(uint8_t *form, uint32_t count)
{
    IData &d = idata();
    d.IffFlag = 0;
    d.IffReturn = 0;
    bool in_form = false;
    bool skip = count & 1 << 30;
    int16_t left = static_cast<int16_t>(count);
    for (;;) {
        uint32_t what = peek_l(form);
        if (what == id("FORM")) {
            if (--left < 0)
                break;
            in_form = true;
            uint32_t type = peek_l(form + 8);
            if (type == id("ILBM") || type == id("ANIM")) {
                form += 12;
                continue;
            }
        } else if (what == id("AenD")) {
            break;
        } else {
            if (!in_form)
                bad_iff();
            int chunk = -1;
            for (int i = 0; i < ChunkCount; i++)
                if (ChunkIds[i] == what)
                    chunk = i;
            if (chunk >= 0 && !skip && d.IffMask & 1u << chunk) {
                d.IffFlag |= 1u << chunk;
                switch (chunk) {
                case BMHD: d.BufBMHD = address(form + 8); break;
                case CMAP: d.BufCMAP = address(form); break;
                case CAMG: d.BufCAMG = address(form + 8); break;
                case CCRT: d.BufCCRT = address(form + 8); break;
                case AMSC:
                case ANHD: d.BufAMSC = address(form + 8); break;
                case BODY: iff_body(form + 8); break;
                case DLTA: iff_delta(form + 8); break;
                }
            }
        }
        form += even(peek_l(form + 4)) + 8;
    }
    return form;
}

// ResTempBuffer (+B.s): the temporary buffer, freed with a size of 0
uint8_t *temp_buffer(uint32_t size)
{
    Regs r = {};
    r.d[0] = size;
    lib_call(L_ResTempBuffer, r);
    return ptr<uint8_t>(idata().TempBuffer);
}

// IffForm: read count FORMs from the file and play them
void form_from_file(BPTR file, uint32_t count)
{
    uint8_t *buffer = temp_buffer(form_size(file, count) + 16);
    if (!buffer)
        return;
    if (static_cast<uint16_t>(form_load(file, buffer, count)) != static_cast<uint16_t>(count))
        disk_error();
    form_play(ptr<uint8_t>(idata().TempBuffer), count);
    temp_buffer(0);
}

// The file of a channel open for input, for the Form functions
BPTR form_channel(uint32_t n)
{
    RegFrame r = {};
    r.d[0] = n;
    if (!lib_get_file(r))
        lib_jump(L_FilNO, 0);
    if (ptr<uint8_t>(r.a[2])[4] & 4)
        lib_jump(L_FilTM, 0);
    return static_cast<BPTR>(r.d[1]);
}

void check_no_monitor()
{
    if (idata().Patch_Errors)
        fon_call();
}

// ---- saving -------------------------------------------------------------

// The chunks are built in Buffer and written with SaveA1
struct Writer {
    uint8_t *start = ptr<uint8_t>(idata().Buffer);
    uint8_t *p = start;
    void l(uint32_t v) { poke_l(p, v), p += 4; }
    void w(uint16_t v) { poke_w(p, v), p += 2; }
    void b(uint8_t v) { *p++ = v; }
    void flush()
    {
        if (p != start && !file_write(start, p - start))
            disk_error();
        p = start;
    }
};

void save_bmhd(Writer &o, Screen *s, uint8_t compression)
{
    o.l(id("BMHD"));
    o.l(20);
    o.w(s->EcTx);
    o.w(s->EcTy);
    o.w(0);
    o.w(0);
    o.b(static_cast<uint8_t>(s->EcNPlan));
    o.b(0);
    o.b(compression);
    o.b(0);
    o.w(0);
    uint8_t ax = 20, ay = 22;
    uint16_t pw = s->EcWTx, ph = s->EcWTy;
    if (s->EcCon0 & 0x8000) {
        ax >>= 1;
        pw <<= 1;
    }
    if (s->EcCon0 & 4) {
        ay >>= 1;
        ph <<= 1;
    }
    o.b(ax);
    o.b(ay);
    o.w(pw);
    o.w(ph);
    o.flush();
}

void save_camg(Writer &o, Screen *s)
{
    o.l(id("CAMG"));
    o.l(4);
    uint32_t mode = s->EcCon0 & 0x8806;
    if (s->EcNbCol == 64 && !(mode & 0x800))
        mode |= 0x80;                               // extra half-brite
    o.l(mode);
    o.flush();
}

void save_amsc(Writer &o, Screen *s)
{
    o.l(id("AMSC"));
    o.l(7 * 2);
    o.w(s->EcAWX);
    o.w(s->EcAWY);
    o.w(s->EcAWTX);
    o.w(s->EcAWTY);
    o.w(s->EcAVX);
    o.w(s->EcAVY);
    o.w(s->EcFlags & 0x8000);
    o.flush();
}

void save_cmap(Writer &o, Screen *s)
{
    o.l(id("CMAP"));
    o.l(32 * 3);
    for (int i = 0; i < 32; i++) {
        uint16_t c = s->EcPal[i];
        o.b(static_cast<uint8_t>((c >> 4) & 0xF0));
        o.b(static_cast<uint8_t>(c & 0xF0));
        o.b(static_cast<uint8_t>((c << 4) & 0xF0));
    }
    o.flush();
}

// One row of a plane in the ByteRun1 of the assembler, which can read (and
// on a run of 128, pack) a byte past the end of the row
uint8_t *pack_row(const uint8_t *src, uint16_t length, uint8_t *to)
{
    uint16_t left = length;
    for (;;) {
        uint16_t repeats = 0;
        uint8_t c = *src++;
        if (!--left) {
            *to++ = 0;
            *to++ = c;
            return to;
        }
        for (;;) {
            if (*src != c)
                break;
            repeats++;
            src++;
            if (repeats >= 127)
                break;
            if (!--left)
                break;
        }
        if (repeats) {
            *to++ = static_cast<uint8_t>(-static_cast<int8_t>(repeats));
            *to++ = c;
            if (!left)
                return to;
            continue;
        }
        uint8_t *count = to;
        uint16_t literals = 0;
        *to++ = 0;
        *to++ = c;
        for (;;) {
            c = src[0];
            if (c == src[1] && c == src[2])
                break;
            *to++ = *src++;
            literals++;
            if (!--left)
                break;
            if (literals >= 127)
                break;
        }
        *count = static_cast<uint8_t>(literals);
        if (!left)
            return to;
    }
}

void save_body(Writer &o, Screen *s, uint8_t compression)
{
    o.l(id("BODY"));
    uint16_t planes = s->EcNPlan;
    if (!compression) {
        // mulu: the low word of the plane size
        o.l(static_cast<uint16_t>(s->EcTPlan) * static_cast<uint32_t>(planes));
        o.flush();
        uint32_t offset = 0;
        for (uint16_t y = s->EcTy; y; y--, offset += s->EcTLigne)
            for (uint16_t p = 0; p < planes; p++)
                if (!file_write(ptr<uint8_t>(s->EcLogic[p]) + offset, s->EcTLigne))
                    disk_error();
        return;
    }
    o.l(0);
    o.flush();
    int32_t start = file_seek(0, OffsetCurrent);
    uint32_t written = 0, offset = 0;
    uint16_t line = s->EcTLigne;
    for (uint16_t y = s->EcTy; y; y--, offset += line) {
        uint8_t *to = o.start;
        for (uint16_t p = 0; p < planes; p++)
            to = pack_row(ptr<uint8_t>(s->EcLogic[p]) + offset, line, to);
        uint32_t n = to - o.start;
        written += n;
        if (!file_write(o.start, n))
            disk_error();
    }
    if (written & 1) {
        *o.start = 0;
        if (!file_write(o.start, 1))
            disk_error();
        written++;
    }
    int32_t end = file_seek(start - 4, OffsetBeginning);
    poke_l(o.start, written);
    if (!file_write(o.start, 4))
        disk_error();
    file_seek(end, OffsetBeginning);
}

// IffSaveScreen: the current screen to the open file
void save_screen(uint8_t compression)
{
    Screen *s = ptr<Screen>(idata().ScOnAd);
    Writer o;
    o.l(id("FORM"));
    o.l(0);
    o.l(id("ILBM"));
    o.flush();
    save_bmhd(o, s, compression);
    save_camg(o, s);
    save_amsc(o, s);
    save_cmap(o, s);
    save_body(o, s, compression);
    int32_t end = file_seek(4, OffsetBeginning);
    o.l(end - 8);
    o.flush();
}

} // namespace

// ---- instructions -------------------------------------------------------

// IFF MASK mask: the chunks Load Iff and Form Play use
extern "C" void lib_in_mask_iff(uint32_t mask) { idata().IffMask = mask; }

// =PICTURE: all of them
extern "C" uint32_t lib_fn_picture() { return 0x7F; }

// LOAD IFF "file"[,screen]
extern "C" void lib_in_load_iff2(const uint8_t *name, uint32_t screen)
{
    IData &d = idata();
    d.IffFlag = 0;
    d.IffParam = screen;
    name_disc(name);
    if (!file_open(ModeOldFile))
        disk_error();
    form_from_file(static_cast<BPTR>(d.Handle), 1);
    file_close();
}

extern "C" void lib_in_load_iff1(const uint8_t *name) { lib_in_load_iff2(name, EntNul); }

// =FORM LOAD(channel,address or bank[,count]): the FORMs of an open file
// into memory. Returns how many were read.
extern "C" uint32_t lib_fn_form_load3(uint32_t channel, int32_t where, int32_t count)
{
    check_no_monitor();
    if (count <= 0 || where < 0)
        fon_call();
    BPTR file = form_channel(channel);
    if (!where)
        fon_call();
    uint8_t *to = ptr<uint8_t>(where);
    if (where < 1024) {
        bank_erase(where);
        uint32_t size = form_size(file, count);
        uint8_t *bank = bank_reserve(where, 0, size, static_cast<const char *>(lib_address(L_BkIff)));
        bank_changed();
        if (!bank)
            lib_jump(L_OOfMem, 0);
        to = bank;
    }
    return form_load(file, to, count);
}

extern "C" uint32_t lib_fn_form_load2(uint32_t channel, int32_t where) { return lib_fn_form_load3(channel, where, 1); }

// =FORM LENGTH(channel[,count])
extern "C" uint32_t lib_fn_form_length2(uint32_t channel, uint32_t count)
{
    check_no_monitor();
    if (count >= 32768)
        fon_call();
    return form_size(form_channel(channel), count);
}

extern "C" uint32_t lib_fn_form_length1(uint32_t channel) { return lib_fn_form_length2(channel, 1); }

// =FORM PLAY(address,count[,screen]), =FORM SKIP(address[,count]):
// returns the address after them
extern "C" uint32_t lib_fn_form_play3(int32_t where, uint32_t count, uint32_t screen)
{
    check_no_monitor();
    idata().IffParam = screen;
    if (count >= 32768)
        fon_call();
    return address(form_play(ptr<uint8_t>(bank_or_address(where)), count));
}

extern "C" uint32_t lib_fn_form_play2(int32_t where, uint32_t count) { return lib_fn_form_play3(where, count, EntNul); }

extern "C" uint32_t lib_fn_form_skip2(int32_t where, uint32_t count)
{
    check_no_monitor();
    if (count >= 32768)
        fon_call();
    return address(form_play(ptr<uint8_t>(bank_or_address(where)), count | 1 << 30));
}

extern "C" uint32_t lib_fn_form_skip1(int32_t where) { return lib_fn_form_skip2(where, 1); }

// =FRAME PARAM: the delay of the last animation frame
extern "C" uint32_t lib_fn_form_param() { return idata().IffReturn; }

// IFF ANIM "file",screen[,times]. (The assembler stopped on a forgotten
// "illegal" instruction.)
extern "C" void lib_in_iff_anim3(const uint8_t *name, uint32_t screen, int32_t times)
{
    if (times < 0)
        fon_call();
    IData &d = idata();
    d.IffParam = screen;
    name_disc(name);
    if (!file_open(ModeOldFile))
        disk_error();
    if (file_seek(0, 1) < 0)
        disk_error();
    int32_t length = file_seek(0, OffsetBeginning);
    if (length < 0)
        disk_error();
    if (!temp_buffer(length + 8))
        lib_jump(L_OOfMem, 0);
    form_load(static_cast<BPTR>(d.Handle), ptr<uint8_t>(d.TempBuffer), 32767);
    file_close();
    uint8_t *anim = ptr<uint8_t>(d.TempBuffer);
    if (peek_l(anim + 8) != id("ILBM"))
        lib_jump(L_IffFor2, 0);
    uint8_t *next = form_play(anim, 1);
    Regs dbl = {};
    ec_call(ec::Double, dbl);
    uint8_t *first = next;
    for (;;) {
        Regs sw = {};
        sw.d[1] = d.ScOn - 1u;
        ec_call(ec::SwapSc, sw);
        do {
            Regs t = {};
            lib_call(L_Test_PaSaut, t);
            Regs v = {};
            sy_call(sy::WaitVbl, v);
        } while (static_cast<int32_t>(--d.IffReturn) >= 0);
        if (peek_l(next) == id("AenD")) {
            if (!--times)
                break;
            next = first;
            continue;
        }
        next = form_play(next, 1);
    }
    temp_buffer(0);
}

extern "C" void lib_in_iff_anim2(const uint8_t *name, uint32_t screen) { lib_in_iff_anim3(name, screen, 1); }

// SAVE IFF "file"[,compression]
extern "C" void lib_in_save_iff2(const uint8_t *name, uint32_t compression)
{
    if (!idata().ScOnAd)
        lib_jump(L_ScNOp, 0);
    if (compression >= 3)
        fon_call();
    name_disc(name);
    if (!file_open(ModeNewFile))
        disk_error();
    save_screen(static_cast<uint8_t>(compression));
    file_close();
}

extern "C" void lib_in_save_iff1(const uint8_t *name) { lib_in_save_iff2(name, 1); }

// COMMAND LINE$=a$, =COMMAND LINE$: kept at the end of Buffer
extern "C" void lib_in_command_line(const uint8_t *s)
{
    uint16_t len = str_len(s);
    if (len >= 256)
        fon_call();
    uint8_t *to = ptr<uint8_t>(idata().Buffer) + 1024 - 256 - 6;
    poke_l(to, id("CmdL"));
    poke_w(to + 4, len);
    const uint8_t *from = str_chars(s);
    for (uint16_t i = 0; i < ((len + 1) & ~1); i++)
        to[6 + i] = from[i];
}

extern "C" uint32_t lib_fn_command_line()
{
    const uint8_t *from = ptr<uint8_t>(idata().Buffer) + 1024 - 256 - 6;
    if (peek_l(from) != id("CmdL"))
        return str_empty();
    uint16_t len = peek_w(from + 4);
    if (!len || len >= 256)
        return str_empty();
    return str_from(from + 6, len);
}
