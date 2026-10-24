// The monitor's help (+Monitor.s Tt_RunHelp, Hlp_*): the word clicked in
// the listing looked for in AMOSPro_Help.Map, its page of AMOSPro_Help.Txt
// shown by the default resource's text reader (as Read Text).
//
// The .Map: for each entry its offset in the .Txt (a long word, the first
// byte of which ends the name before it), $FF, the name in capitals.

#include "monitor/monitor.h"

#include <dos/dos.h>

#define DOS_BASE_NAME (amos::ptr<struct DosLibrary>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;
using namespace amos::monitor;

namespace {

constexpr uint16_t EcFsel = 10;
constexpr uint16_t MOutOfMemory = 17, MNoMap = 18, MNoText = 19, MLoading = 20, MNotFound = 21, MTitleLine = 22,
                   MTitle = 23;

uint32_t memory(uint32_t size, bool clear)
{
    Regs r;
    r.d[0] = size;
    sy_call(clear ? sy::MemFastClear : sy::MemFast, r);
    return r.a[0];
}

void free_memory(uint32_t address, uint32_t size)
{
    Regs r;
    r.a[1] = address;
    r.d[0] = size;
    sy_call(sy::MemFree, r);
}

uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? uint8_t(c - 0x20) : c; }

// AddSuffix: the help's name (system message 12) and suffix in the buffer
const char *file_name(const char *suffix)
{
    Regs m;
    m.d[0] = 12;
    m.a[0] = address(boot::sys_message(m.d[0]));
    char *to = ptr<char>(idata().Buffer) + 512;
    char *name = to;
    for (const char *s = ptr<char>(m.a[0]); (*to++ = *s++);) {
    }
    to--;
    while ((*to++ = *suffix++)) {
    }
    return name;
}

// Hlp_FreeTexte / Hlp_FreeMap
void free_text()
{
    MonData &m = mon();
    if (uint32_t t = m.Hlp_Texte) {
        uint32_t size = m.Hlp_LTexte;
        m.Hlp_Texte = 0;
        m.Hlp_LTexte = 0;
        free_memory(t, size);
    }
}

void free_map()
{
    free_text();
    MonData &m = mon();
    if (uint32_t a = m.Hlp_Map) {
        uint32_t size = m.Hlp_LMap;
        m.Hlp_Map = 0;
        m.Hlp_LMap = 0;
        free_memory(a, size);
    }
}

// Hlp_GetMap: the .Map loaded once; 0 or the message of the error
uint16_t get_map()
{
    MonData &m = mon();
    if (m.Hlp_Map)
        return 0;
    BPTR f = Open(file_name(".Map"), MODE_OLDFILE);
    // (the assembler said "out of memory" when the file did not load, and
    // the reverse; and left it open after a failed read)
    if (!f)
        return MNoMap;
    Seek(f, 0, OFFSET_END);
    int32_t length = Seek(f, 0, OFFSET_BEGINNING);
    uint32_t a = memory(uint32_t(length), false);
    if (!a) {
        Close(f);
        return MOutOfMemory;
    }
    m.Hlp_Map = a;
    m.Hlp_LMap = uint32_t(length);
    int32_t read = Read(f, ptr<void>(a), length);
    Close(f);
    if (read != length) {
        free_map();
        return MNoMap;
    }
    return 0;
}

uint32_t peek_3(const uint8_t *p) { return uint32_t(p[0]) << 16 | uint32_t(p[1]) << 8 | p[2]; }

// Hlp_LoadTexte: the page of word (length counting its 0) in Hlp_Texte;
// 0 or the message of the error
uint16_t load_text(const uint8_t *word, uint16_t length)
{
    MonData &m = mon();
    free_text();
    if (uint16_t e = get_map())
        return e;
    const uint8_t *map = ptr<uint8_t>(m.Hlp_Map);
    uint32_t map_length = m.Hlp_LMap;

    // The longest start of the word that is a name of the map (two
    // letters at least, an operator one character)
    uint8_t first = upper(word[0]);
    uint16_t letters = first >= 'A' && first <= 'Z' ? 2 : 1;
    uint32_t found = 0;
    // (the first time the count is the first character, as in the assembler)
    uint16_t count = first;
    uint8_t *key = ptr<uint8_t>(idata().Buffer) + 128;
    for (;;) {
        key[0] = 0x00;
        key[1] = 0xFF;
        uint8_t *k = key + 2;
        uint16_t n = 0;
        const uint8_t *w = word;
        do {
            uint8_t c = upper(*w++);
            if (c != ' ') {
                *k++ = c;
                n++;
            }
        } while (--count && n != letters);
        count = length;
        Regs f;
        f.a[1] = address(map);
        f.d[1] = map_length;
        f.a[2] = address(key);
        f.d[2] = uint16_t(n + 2);
        f.d[4] = 0;
        lib_call(L_InstrFind, f);
        if (!f.d[3])
            break;
        found = f.d[3];
        if (++letters > length)
            break;
    }
    if (!found)
        return MNotFound;

    // Its offset, its length up to the next one
    const uint8_t *e = map + int16_t(found) - 4;
    uint32_t offset = peek_3(e);
    e += 5;
    while (*e++) {
    }
    uint32_t size = peek_3(e) - offset;

    uint32_t text = memory(size + 512, true);
    if (!text) {
        free_map();
        return MOutOfMemory;
    }
    m.Hlp_Texte = text;
    m.Hlp_LTexte = size + 512;
    BPTR f = Open(file_name(".Txt"), MODE_OLDFILE);
    if (!f) {
        free_text();
        return MNoText;
    }
    // Three title lines and an empty one, the title in the middle of the
    // second
    const uint8_t *line = message(MTitleLine);
    uint8_t *to = ptr<uint8_t>(text);
    for (int i = 0; i < 3; i++) {
        for (const uint8_t *s = line; (*to++ = *s++);) {
        }
        to[-1] = 10;
    }
    *to++ = 10;
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    Seek(f, int32_t(offset), OFFSET_BEGINNING);
    Read(f, buffer, 64);
    const uint8_t *t = buffer;
    while (*t++ >= 32) {
    }
    uint32_t title = uint32_t(t - buffer);
    offset += title;
    size -= title;
    uint8_t *centre = ptr<uint8_t>(text) + 132 - uint16_t(title >> 1);
    for (uint32_t i = 0; i + 1 < title; i++)
        centre[i] = buffer[i];
    Seek(f, int32_t(offset), OFFSET_BEGINNING);
    int32_t read = Read(f, to, int32_t(size));
    Close(f);
    if (read != int32_t(size)) {
        free_text();
        return MNoText;
    }
    return 0;
}

// Tt_IMes: a message in the information window
void info_message(uint16_t n)
{
    alert(message(n));
    activate_info();
    info_show_all();
}

// The text reader on the page loaded; 0 or the message of the error
uint16_t show_text()
{
    IData &d = idata();
    const uint8_t *resource = ptr<uint8_t>(d.Sys_Resource);
    const uint8_t *graphics = resource + peek_l(resource + 2);
    const uint8_t *messages = resource + peek_l(resource + 2 + 4);
    const uint8_t *programs = resource + peek_l(resource + 2 + 8);
    Regs s;
    s.a[0] = address(graphics);
    s.d[0] = EcFsel;
    s.d[1] = uint32_t(int32_t(int16_t(d.PI_RtSx)));
    s.d[2] = uint32_t(int32_t(int16_t(d.PI_RtSy)));
    s.d[3] = 0;
    s.d[4] = 0xFFFFFFFF;
    lib_call(L_Dia_RScOpen, s);
    uint32_t channel = 0;
    uint16_t error = MOutOfMemory;
    if (!uint16_t(s.d[0])) {
        for (channel = 65536;; channel++) {
            Regs g;
            g.d[0] = channel;
            lib_call(L_Dia_GetChannel, g);
            if (!g.a[0])
                break;
        }
        Regs o;
        o.d[0] = channel;
        o.d[1] = 1024 * 8;
        o.d[2] = 8;
        o.d[3] = 0;
        o.a[0] = address(programs + peek_l(programs + 2));
        o.a[1] = address(graphics);
        o.a[2] = address(messages);
        lib_call(L_Dia_OpenChannel, o);
        if (!o.d[0]) {
            uint32_t *v = ptr<uint32_t>(o.a[1]);
            v[1] = address(message(MTitle)) - 2;
            v[2] = 4;                                   // buttons on a line
            v[0] = mon().Hlp_Texte;
            Regs r;
            r.d[0] = channel;
            r.d[1] = 0xFFFFFFFF;
            r.d[2] = 0;
            r.d[3] = 0;
            lib_call(L_Dia_RunProgram, r);
            if (!r.d[0]) {
                Regs a;
                a.a[2] = wdata().EcAdr[EcFsel];
                a.d[7] = d.PI_RtSpeed;
                a.d[6] = 1;
                a.d[5] = uint16_t(d.PI_RtWy + (d.PI_RtSy >> 1));
                lib_call(L_AppCentre, a);
                for (;;) {
                    boot::wait_vbl_multi();
                    Regs t;
                    lib_call(L_Test_PaSaut, t);
                    Regs g;
                    g.d[0] = channel;
                    lib_call(L_Dia_GetReturn, g);
                    if (g.d[0])
                        break;
                    if (int32_t(g.d[1]) < 0) {
                        error = 0;
                        break;
                    }
                }
            }
        }
        Regs c;
        c.d[0] = channel;
        lib_call(L_Dia_CloseChannel, c);
    }
    // (the assembler tested the screen's first long word rather than the
    // screen, reading address 0 when it had not opened)
    if (Screen *e = ptr<Screen>(wdata().EcAdr[EcFsel])) {
        Regs a;
        a.a[2] = address(e);
        a.d[7] = uint32_t(-int16_t(d.PI_RtSpeed));
        a.d[6] = uint16_t(e->EcTy >> 1);
        a.d[5] = uint16_t(e->EcAWY + a.d[6]);
        lib_call(L_AppCentre, a);
        Regs del;
        del.d[1] = EcFsel;
        ec_call(ec::Del, del);
    }
    return error;
}

} // namespace

namespace amos::monitor {

// Tt_RunHelp: help on the word at x, y (row) of the listing
void run_help(uint16_t x, uint16_t y, uint16_t row)
{
    MonData &m = mon();
    Chosen c = choose(x, y, row);
    if (!c.length) {
        wait_release();
        return;
    }
    uint16_t error = MNotFound;
    if (c.length >= 2) {
        uint8_t *word = ptr<uint8_t>(idata().Buffer);
        uint16_t length = 0;
        do
            word[length] = c.text[length];
        while (word[length++]);
        info_message(MLoading);
        error = load_text(word, length);
        if (!error) {
            alert_delete();
            error = show_text();
            free_text();
        }
    }
    if (error)
        alert(message(error));
    m.Tt_Ecran = 0xFFFF;
    m.Tt_YInfo = 0xFFFF;
    activate_text();
    listing_show();
    activate_info();
    info_show_all();
    info_slider();
}

void help_free() { free_map(); }

} // namespace amos::monitor

extern "C" {
void mon_free_map(AllRegs *) { free_map(); }
}
