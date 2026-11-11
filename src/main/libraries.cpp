// Loading the libraries: AMOSPro.Lib and the extensions (+B.s
// Libraries_Load...). A library file holds a hunk header, the sizes of its
// routines, its token table, its routines and its title. Loaded, the token
// table has the library's data zone (LB_*) and, below it, the jump table
// of its routines. The routines are relocated by their R-markers (+CEqu.s:
// Rjsr, Rbra, Rdata...), and the "GetP" before an instruction's code
// becomes the call to its parameter collector.

#include "core/guest_scratch.h"
#include "main/main.h"

#include "core/amiga.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::boot;

// +B.s: AMOSJmps, the routines of AMOSPro.Lib that are in AMOSPro (a word,
// the routine, a long, its address, until a 0 routine); ExtJmps, the same
// for the extensions in AMOSPro (a word, the slot, a long, its list: the
// number of routines of the extension, then its routines as AMOSJmps).

// main/library.s: AMOSPro.Lib, linked in (the file without its hunk
// header); src/ext/*_image.s: the extensions' the same way


namespace {

// Ll_Rts: the address of the routines that are empty
alignas(2) const uint16_t empty_routine = 0x4AFC;   // illegal

// The opcodes the markers become
constexpr uint16_t OpJmpAbs = 0x4EF9, OpJsrAbs = 0x4EB9;    // jmp/jsr address
constexpr uint16_t OpJmpAn = 0x4ED0, OpJsrAn = 0x4E90;      // jmp/jsr (an)
constexpr uint16_t OpMoveA4 = 0x206C;                       // move.l d16(a4),an
constexpr uint16_t OpLeaAbs = 0x41F9;                       // lea address,an
constexpr uint16_t ParamsMark = 0x5041;                     // "PA": a collector's number follows
constexpr uint16_t OpNop = 0x4E71;
// Rbra, Rbsr, Rbeq... (codes 2 to 14): bra.w, bsr.w, beq.w...
constexpr uint16_t OpBranch[13] = {0x6000, 0x6100, 0x6700, 0x6600, 0x6500, 0x6400, 0x6D00,
                                   0x6C00, 0x6300, 0x6200, 0x6F00, 0x6A00, 0x6B00};

constexpr uint8_t C_Code1 = 0xFE, C_Code2 = 0x01, C_CodeJ = 0xF7, C_CodeT = 0xF5;
constexpr uint16_t C_CodeD = 0x6543;

// The flags of the token table (+Equ.s)
constexpr uint16_t L_FFloat = 0x1000, L_FAngle = 0x2000, L_FMath = 0x3000, L_VRes = 0x4000;

[[noreturn]] void illegal() { halt(); }

uint8_t *library(uint32_t n) { return ptr<uint8_t>(idata().AdTokens[n]); }

// The extensions linked in: the library's file, its image and, for the extensions of other
// authors (written by tools/ext_gen.py), the list that binds its routines to its jump table, so
// that they can be in any slot. The four of AMOSPro's are bound by slot (main_ext_jmps).
struct Port {
    const char *file;
    const uint8_t *image;
    const uint8_t *jumps;
};

const Port *ports()
{
    static const Port list[] = {{"AMOSPro_Music.Lib", AMOS_DATA(ext_music_image), nullptr},
                                {"AMOSPro_Compact.Lib", AMOS_DATA(ext_compact_image), nullptr},
                                {"AMOSPro_Request.Lib", AMOS_DATA(ext_request_image), nullptr},
                                {"AMOSPro_IOPorts.Lib", AMOS_DATA(ext_ioports_image), nullptr},
#if !defined(__mc68000__) || defined(AMOS_THIRD_PARTY)
                                {"AMOSPro_AMCAF.Lib", AMOS_DATA(ext_amcaf_image), AMOS_DATA(ext_amcaf_jumps)},
                                {"AMOSPro_TURBO_Plus.Lib", AMOS_DATA(ext_turbo_image), AMOS_DATA(ext_turbo_jumps)},
#endif
                                {nullptr, nullptr, nullptr}};
    return list;
}

// What each slot was loaded from: the list of its routines (none: the library's file, or by slot)
const uint8_t *loaded_jumps[Libraries];

// The image linked in of the extension a configuration names (by the name
// of its file, without its path), or none
const Port *extension_port(const char *name)
{
    const char *file = name;
    for (const char *p = name; *p && *p != ' '; p++)
        if (*p == '/' || *p == ':')
            file = p + 1;
    for (const Port *i = ports(); i->file; i++) {
        const char *a = file, *b = i->file;
        while (*b && (*a | 0x20) == (*b | 0x20))
            a++, b++;
        if (!*b && (!*a || *a == ' '))
            return i;
    }
    return nullptr;
}

// The end of a token: its name and its parameters, each ended by a negative
// byte, then even (Ll_TkNext without the 4 bytes of the token)
uint8_t *skip_definition(uint8_t *p)
{
    while (int8_t(*p++) >= 0)
        ;
    while (int8_t(*p++) >= 0)
        ;
    return p + (address(p) & 1);
}

// Where a library comes from: its file (Name1), or its image in memory
struct Source {
    const uint8_t *image;

    bool read(void *to, uint32_t length)
    {
        if (!image)
            return sys_read(to, length);
        const uint8_t *from = image;
        for (uint8_t *p = static_cast<uint8_t *>(to); length--;)
            *p++ = *from++;
        image = from;
        return true;
    }
    void back(uint32_t length)
    {
        if (image)
            image -= length;
        else
            sys_seek(-int32_t(length), 0);
    }
    void close()
    {
        if (!image)
            sys_close();
    }
};

// Library_Load: library n from the file Name1 (or the image of AMOSPro.Lib
// linked in, for library 0); its command line
int library_load(uint32_t n, const char *command, const uint8_t *image)
{
    IData &d = idata();
    Source source = {image};
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    if (image) {
        // as if the hunk header had been read
        if (!source.read(buffer + 0x20, 18))
            return 1;
    } else {
        if (!sys_open(ptr<char>(d.Name1), 1005))
            return 1;
        if (!sys_read(buffer, 0x20 + 18))
            return 1;
    }
    // after the hunk header: the size of the routines' sizes, of the token
    // table, of the routines, of the title
    const uint8_t *header = buffer + 0x20;
    uint32_t routines = peek_l(header) >> 1;
    uint32_t jumps = (routines << 2) + 4;
    uint32_t size = LB_Size + peek_l(header + 4) + peek_l(header + 8) + peek_l(header + 12) + jumps;
    uint32_t base = ram_fast(size);
    if (!base)
        return 2;
    uint8_t *tokens = ptr<uint8_t>(base + jumps + LB_Size);
    d.AdTokens[n] = address(tokens);
    lb_long(tokens, LB_MemSize) = size;
    lb_long(tokens, LB_MemAd) = base;
    uint32_t sizes = ram_fast(peek_l(header) + 4);
    if (!sizes)
        return 2;
    poke_l(ptr<void>(sizes), peek_l(header));
    lb_long(tokens, LB_LibSizes) = sizes + 4;
    // a new or an old library?
    lb_word(tokens, LB_Flags) = 0;
    tokens[LB_Flags] |= 1 << LBF_20;
    if (!source.read(buffer, 4))
        return 1;
    if (peek_l(buffer) != 0x41503230) {     // "AP20"
        tokens[LB_Flags] &= uint8_t(~(1 << LBF_20));
        source.back(4);
    }
    // the sizes of the routines, then their addresses
    if (!source.read(ptr<void>(sizes + 4), peek_l(ptr<void>(sizes))))
        return 1;
    const be_u16 *length = ptr<be_u16>(sizes + 4);
    uint32_t code = address(tokens) + peek_l(header + 4);
    auto *entry = reinterpret_cast<be_u32 *>(tokens - LB_Size);
    for (uint32_t i = 0; i < (routines & 0xFFFF); i++) {
        *--entry = code;
        uint16_t words = *length++;
        if (!words)
            *entry = guest_constant(&empty_routine, sizeof empty_routine);
        code += 2 * uint32_t(words);
    }
    // the rest
    if (!source.read(tokens, peek_l(header + 4) + peek_l(header + 8) + peek_l(header + 12)))
        return 1;
    source.close();
    lb_word(tokens, LB_NRout) = uint16_t(routines);
    lb_long(tokens, LB_Title) = address(tokens) + peek_l(header + 4) + peek_l(header + 8);
    lb_long(tokens, LB_Command) = address(command);
    return 0;
}

// Ext_OldLabel: the routine of AMOSPro 2.0 an extension for AMOSPro 1.0
// calls by its number of then (0: none)
uint16_t old_label(uint16_t n)
{
    static const uint16_t convert[][2] = {
        {1024, L_Error}, {1025, L_ErrorExt}, {207, L_Test_PaSaut}, {956, L_WaitRout},
        {287, L_GetEc}, {46, L_Demande}, {432, L_RamFast}, {433, L_RamChip},
        {434, L_RamFast2}, {435, L_RamChip2}, {436, L_RamFree}, {1100, L_Bnk_GetAdr},
        {1101, L_Bnk_GetBobs}, {1102, L_Bnk_GetIcons}, {1103, L_Bnk_Reserve}, {1104, L_Bnk_Eff},
        {1105, L_Bnk_EffA0}, {1106, L_Bnk_EffTemp}, {1107, L_Bnk_EffAll}, {1234, L_Bnk_Change},
        {1121, L_Bnk_OrAdr}, {1119, L_Dsk_PathIt}, {1120, L_Dsk_FileSelector}, {1122, L_Dev_Open},
        {1123, L_Dev_Close}, {1124, L_Dev_GetIO}, {1125, L_Dev_AbortIO}, {1126, L_Dev_DoIO},
        {1127, L_Dev_SendIO}, {1128, L_Dev_CheckIO}, {1129, L_Dev_Error}};
    for (const auto &c : convert)
        if (c[0] == n)
            return c[1];
    return 0;
}

// The token table of an old extension: the routine numbers become offsets
// from the table
void reloc_old_tokens(uint8_t *tokens)
{
    uint8_t *p = tokens + 6;
    do {
        // the instruction (none if 0, the function only if negative), then
        // the function
        int16_t n = int16_t(peek_w(p));
        if (n) {
            if (n > 0)
                poke_w(p, uint16_t(lb_jump(tokens, uint16_t(n)) - address(tokens)));
            n = int16_t(peek_w(p + 2));
            if (n > 0)
                poke_w(p + 2, uint16_t(lb_jump(tokens, uint16_t(n)) - address(tokens)));
        }
        p = skip_definition(p + 4);
    } while (peek_w(p));
}

// The token table of a 2.0 library: an instruction points to its routine,
// a function before it; the word after it gets the flags of its parameters
void reloc_tokens(uint8_t *tokens)
{
    uint8_t *p = tokens;
    for (;;) {
        int16_t instruction = int16_t(peek_w(p));
        int16_t function = int16_t(peek_w(p + 2));
        if (!instruction)
            break;
        uint16_t flags = uint16_t(instruction);
        if (instruction > 0 && instruction != 1) {
            // an instruction, or an instruction and a function
            poke_w(p, uint16_t(-int16_t((instruction + 1) << 2)));
            if (function < 0) {
                poke_w(p + 2, 0);
                p = skip_definition(p + 4);
                continue;
            }
            flags = 0x40;
            if (function > 1)
                flags |= 0x80;
        } else if (function > 1) {
            // a function, or a maths function
            poke_w(p, uint16_t(-int16_t(function << 2)));
            if (flags & 0xFF00) {
                flags &= 0x7F00;
                if (!flags) {
                    poke_w(p + 2, 0);
                    p = skip_definition(p + 4);
                    continue;
                }
            } else {
                flags = 0;
            }
            flags |= 0x80;
        }
        // the flags: the number of parameters, their kind
        uint8_t *q = p + 4;
        while (int8_t(*q++) >= 0)
            ;
        int8_t c = int8_t(*q++);
        if (c >= 0) {
            if (c == 'C') {
                // a constant
                while (int8_t(*q++) >= 0)
                    ;
            } else {
                bool more = true;
                if (c == 'V') {
                    // a reserved variable
                    c = int8_t(*q++);
                    if (c < 0)
                        more = false;
                    else
                        flags |= L_VRes;
                }
                while (more) {
                    c = int8_t(*q++);
                    if (c < 0)
                        break;
                    flags++;
                    switch (c - '0') {
                    case 1: flags |= L_FFloat; break;
                    case 4: flags |= L_FMath; break;
                    case 5: flags |= L_FAngle; break;
                    }
                    // the separator
                    c = int8_t(*q++);
                    more = c >= 0;
                }
            }
        }
        poke_w(p + 2, flags);
        p = ptr<uint8_t>((address(q) + 1) & ~1u);
    }
    p += 2;
    // the routines to swap for the other float precision?
    if (peek_l(p) == 0x46537770) {      // "FSwp"
        lb_word(tokens, LB_DFloatSwap) = peek_w(p + 4);
        lb_word(tokens, LB_FFloatSwap) = peek_w(p + 6);
        p += 8;
    }
    // data for the compiler
    if (peek_l(p) == 0x436F6D50)        // "ComP"
        p += 6 + int16_t(peek_w(p + 4));
    // a table for the quick verification
    if (peek_l(p) == 0x4B77694B)        // "KwiK"
        lb_long(tokens, LB_Verif) = address(p + 4);
}

// A call into library ext (Rjsr...): the address of its routine n, 0 if
// there is none
uint32_t called_routine(uint8_t *tokens, uint32_t ext, uint16_t n)
{
    uint8_t *target = library(ext);
    if (!target)
        return 0;
    if (!(tokens[LB_Flags] & (1 << LBF_20))) {
        n = old_label(n);
        if (!n)
            return 0;
    } else if (n >= lb_word(target, LB_NRout)) {
        return 0;
    }
    return lb_jump(target, n);
}

// Library_Reloc: library n's tokens and the markers of its routines
int library_reloc(uint32_t n)
{
    uint8_t *tokens = library(n);
    if (!tokens)
        return 0;
    if (tokens[LB_Flags] & (1 << LBF_20))
        reloc_tokens(tokens);
    else
        reloc_old_tokens(tokens);

    uint16_t routines = lb_word(tokens, LB_NRout);
    const be_u16 *sizes = ptr<be_u16>(lb_long(tokens, LB_LibSizes));
    for (uint16_t i = 0; i < routines; i++) {
        uint8_t *p = ptr<uint8_t>(lb_jump(tokens, i));
        uint16_t words = sizes[i];
        if (!words)
            continue;
        uint8_t *end = p + 2 * uint32_t(words);
        for (;;) {
            // a marker: C_Code1, its code * 16 + C_Code2
            if (p[0] == C_Code1 && (p[1] & 0x0F) == C_Code2) {
                uint8_t code = p[1] >> 4;
                if (code <= 1) {
                    // Rjmp/Rjsr, Rjmpt/Rjsrt, Rlea
                    uint32_t ext = 0;
                    uint16_t routine;
                    if (p[2] == C_CodeJ && p[3] < Libraries) {
                        ext = p[3];
                        poke_w(p, code ? OpJsrAbs : OpJmpAbs);
                        routine = peek_w(p + 4);
                    } else if (p[2] == C_CodeT && p[3] < 8) {
                        // through the jump table: move.l d16(a4),an / jsr (an)
                        uint16_t an = p[3];
                        poke_w(p, uint16_t(OpMoveA4 | an << 9));
                        poke_w(p + 2, uint16_t(-int16_t((peek_w(p + 4) << 2) + LB_Size + 4)));
                        poke_w(p + 4, uint16_t((code ? OpJsrAn : OpJmpAn) | an));
                        p += 6;
                        continue;
                    } else if (p[2] == C_CodeT && p[3] - 8u < 8) {
                        poke_w(p, uint16_t(OpLeaAbs | (p[3] - 8) << 9));
                        routine = peek_w(p + 4);
                    } else {
                        goto next;
                    }
                    uint32_t target = called_routine(tokens, ext, routine);
                    if (!target)
                        return 2;
                    poke_l(p + 2, target);
                    p += 6;
                    continue;
                }
                if (code <= 14) {
                    // Rbra, Rbsr, Rbeq...: a branch in the library
                    uint16_t routine = peek_w(p + 2);
                    if (routine >= routines)
                        goto next;
                    poke_w(p, OpBranch[code - 2]);
                    int32_t offset = int32_t(lb_jump(tokens, routine) - address(p + 2));
                    if (offset <= -32766 || offset >= 32766)
                        illegal();
                    poke_w(p + 2, uint16_t(offset));
                    p += 4;
                    continue;
                }
                // Rdata: the data that follow are not code
                if (peek_w(p + 2) == C_CodeD) {
                    poke_w(p, OpNop);
                    poke_w(p + 2, OpNop);
                    break;
                }
            }
        next:
            p += 2;
            if (p >= end)
                break;
        }
    }
    clear_cache();
    return 0;
}

// Library_GetParams: the "GetP" in front of the instructions and functions
// of library n with parameters becomes "PA" and the number of the
// parameter collector they need (0-5 integers, 6-11 floats, 12-17 a
// reserved variable's, 18 an angle, 19 a maths function; the extensions'
// 20 further on), which interp/params.cpp does. (The loader of the
// original put a jsr d16(a4) to +ILib.s Parameters' collectors there.)
void library_get_params(uint32_t n)
{
    uint8_t *tokens = library(n);
    if (!tokens || !(tokens[LB_Flags] & (1 << LBF_20)))
        return;
    uint32_t extension = n ? 20 : 0;

    auto patch = [&](uint32_t entry, uint16_t kind, uint16_t params, bool function) {
        uint16_t collector;
        switch (kind) {
        case 0:
        integer:
            if (!params)
                return;
            collector = params < 6 ? params : 5;
            break;
        case 1:
            if (!params)
                return;
            [[fallthrough]];
        case 6:
        case 7:
            // a float (6 and 7, flags the table does not make, go there too)
            collector = uint16_t(params + 6) < 12 ? uint16_t(params + 6) : 11;
            break;
        case 2:
            collector = 18;
            break;
        case 3:
            collector = 19;
            break;
        case 4:
            if (function)
                goto integer;
            collector = uint16_t(params + 12) < 18 ? uint16_t(params + 12) : 17;
            break;
        default:
            collector = uint16_t(params + 6) < 12 ? uint16_t(params + 6) : 11;
            break;
        }
        uint8_t *code = ptr<uint8_t>(entry - 4);
        if (peek_l(code) != 0x47657450) {       // "GetP"
            if (peek_w(code) == ParamsMark)     // done already
                return;
            illegal();
        }
        poke_w(code, ParamsMark);
        poke_w(code + 2, uint16_t(collector + extension));
    };

    uint8_t *p = tokens;
    int16_t offset = int16_t(peek_w(p));
    do {
        uint16_t flags = peek_w(p + 2);
        if (flags) {
            uint16_t params = flags & 0x3F;
            poke_w(p + 2, params);
            uint16_t kind = (flags & 0x7000) >> 12;
            if (flags & 0x40)
                patch(*reinterpret_cast<be_u32 *>(tokens - LB_Size + offset), kind, params, false);
            if (flags & 0x80)
                patch(*reinterpret_cast<be_u32 *>(tokens - LB_Size - 4 + offset), kind, peek_w(p + 2),
                      true);
        }
        p = skip_definition(p + 4);
        offset = int16_t(peek_w(p));
    } while (offset);
}

// Library_Init: routine 0 of library n starts it. It answers the number
// of its extension, or a negative number to refuse.
int library_init(uint32_t n)
{
    uint8_t *tokens = library(n);
    if (!tokens)
        return 0;
    Regs r;
    r.a[1] = lb_long(tokens, LB_Command);
    r.d[0] = n - 1;             // (the number it is loaded as: the extensions of other authors take it)
    r.d[1] = 0x41506578;        // "APex"
    r.d[2] = 0x200;             // VerNumber
    amos_call(&r, host::decode_routine(lb_jump(tokens, 0)));
    // (the number is a byte)
    if (int16_t(r.d[0]) < 0 || int8_t(r.d[0]) != int16_t(n - 1))
        return 2;
    return 0;
}

} // namespace

namespace amos::boot {

int libraries_load()
{
    // the main library (linked in: its name in the configuration only
    // gives its command line), then the extensions, from the configuration
    const char *command = sys_add_path_com(sys_message(14));
    if (int e = library_load(0, command, AMOS_DATA(main_library)))
        return e;
    for (uint32_t n = 1; n < Libraries; n++) {
        // (a host's setup can put an extension in a slot: amos.conf's [extensions])
        const char *name = hal::extension_file(n);
        if (!name)
            name = sys_message(16 + n - 1);
        if (!*name)
            continue;
        command = sys_add_path_com(name);
        const Port *port = extension_port(name);
        const uint8_t *image = port ? port->image : nullptr;
        loaded_jumps[n] = port ? port->jumps : nullptr;
#if !defined(__mc68000__)
        // another host cannot run the 68000 code of an extension's file:
        // only the extensions linked in
        if (!image)
            continue;
#endif
        if (int e = library_load(n, command, image))
            return e;
    }
    return 0;
}

// Library_Patch: the routines of AMOSPro.Lib in AMOSPro, into its jump
// table. Their size goes, so that they are not relocated.
void library_patch()
{
    uint8_t *tokens = library(0);
    be_u16 *sizes = ptr<be_u16>(lb_long(tokens, LB_LibSizes));
    const uint8_t *p = AMOS_DATA(main_amos_jmps);
    uint16_t n = peek_w(p);
    p += 2;
    do {
        sizes[n] = 0;
        lb_jump(tokens, n) = peek_l(p);
        n = peek_w(p + 4);
        p += 6;
    } while (n);
    // the extensions' routines in AMOSPro (src/ext), for the slots that hold
    // them: the same number of routines, the routines empty
    auto bind = [](uint32_t slot, const uint8_t *list) {
        uint8_t *ext = library(slot);
        if (!ext)
            return;
        if (peek_w(list) != lb_word(ext, LB_NRout))
            return;
        // (the list ends with routine 0, which may also come first: the
        // cold start)
        auto each = [list](auto f) {
            for (const uint8_t *q = list + 2; q == list + 2 || peek_w(q); q += 6)
                f(peek_w(q), peek_l(q + 2));
        };
        const be_u16 *ext_sizes = ptr<be_u16>(lb_long(ext, LB_LibSizes));
        bool empty = true;
        each([&](uint16_t r, uint32_t) {
            if (ext_sizes[r])
                empty = false;
        });
        if (!empty)
            return;
        each([&](uint16_t r, uint32_t routine) { lb_jump(ext, r) = routine; });
    };
    for (const uint8_t *e = AMOS_DATA(main_ext_jmps); uint16_t slot = peek_w(e); e += 6)
        if (!loaded_jumps[slot])
            bind(slot, ptr<uint8_t>(peek_l(e + 2)));
    for (uint32_t slot = 1; slot < Libraries; slot++)
        if (loaded_jumps[slot])
            bind(slot, loaded_jumps[slot]);
}

// Libraries_Reloc: relocation, then the parameter collectors
int libraries_reloc()
{
    for (uint32_t n = 0; n < Libraries; n++)
        if (int e = library_reloc(n))
            return e;
    libraries_free_sizes();
    for (uint32_t n = 0; n < Libraries; n++)
        library_get_params(n);
    return 0;
}

int libraries_init()
{
    for (uint32_t n = 0; n < Libraries; n++)
        if (int e = library_init(n))
            return e;
    return 0;
}

// Libraries_Stop: the end routines of the extensions, the last first
void libraries_stop()
{
    IData &d = idata();
    for (int32_t i = 26 - 1; i >= 0; i--) {
        if (uint32_t end = d.ExtAdr[4 * i + 2]) {
            Regs r;
            r.d[2] = uint32_t(i);
            r.a[2] = address(&d.ExtAdr[4 * i]);
            amos_call(&r, host::decode_routine(end));
        }
    }
}

void libraries_free()
{
    libraries_free_sizes();
    IData &d = idata();
    for (uint32_t n = 0; n < Libraries; n++) {
        if (uint8_t *tokens = library(n)) {
            d.AdTokens[n] = 0;
            ram_free(lb_long(tokens, LB_MemAd), lb_long(tokens, LB_MemSize));
        }
    }
}

// The sizes of the routines are needed only to relocate them
void libraries_free_sizes()
{
    for (uint32_t n = 0; n < Libraries; n++) {
        uint8_t *tokens = library(n);
        if (!tokens)
            continue;
        if (uint32_t sizes = lb_long(tokens, LB_LibSizes)) {
            lb_long(tokens, LB_LibSizes) = 0;
            ram_free(sizes - 4, peek_l(ptr<void>(sizes - 4)) + 4);
        }
    }
}

} // namespace amos::boot
