// The editor's menu (+Edit.s EdM_*, Ed_MnGere, Mn_GetOption): the menu
// tree made from the configuration, the user menu, the AMOS menu with the
// hidden programs; and what changes it: the key shortcuts, the programs
// of the options, the user options, the marks.
//
// EdM_Definition: 8 bytes for each object: its function ('0' + n, '0'
// for none), two colours ('0' + n), a byte, its place in the tree (four
// digits, '0' after the last level); "*" ends a part.
// EdM_Messages / EdM_User: a list of texts (see editor/config.cpp), the
// objects' texts in the same order.
// EdM_Table: 8 bytes for each object made: the place in the tree (four
// bytes), the text's offset in EdM_Messages (negated: in EdM_User), the
// function, the number of the definition; 0 at the end.
// Ed_AutoLoad: 3 bytes for each function: its flags (bit 7 a program is
// set), the program's text in Ed_MnPrograms, the command line's.

#include "editor/editor.h"
#include "editor/clipboard.h"
#include "core/amiga.h"
#include "lib/menus.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t HiddenMax = 12;              // EdM_HiddenMax
constexpr uint16_t UserCommands = 115, UserLong = 16, UserMax = 20;
constexpr uint16_t HiddenCommands = 184;
constexpr uint16_t EcEdit = 9;
constexpr uint16_t BtInsert = 10;
constexpr uint8_t EtAIns = 1 << 1;
constexpr uint8_t Shf = 0x03, Ctr = 0x08, Alt = 0x30, Ami = 0xC0;
constexpr uint16_t EdDKyMn1 = 24, EdDKyMnE = 26, EdDKyMn2 = 27, EdDKyMn3 = 28, EdDPrgMn1 = 30, EdDPrgMn2 = 31,
                   EdDPrgMn3 = 32, EdDPrgMnE = 33, EdDMnUsA = 40, EdDMnUsD = 41, EdDMnUs2 = 42, EdDMnUsE = 43,
                   EdDMnUs = 44, EdDASave = 48;
constexpr int KeyText = 11;                     // L_KDef: the length of a key's text

// The names of the shifts and of the keys without ASCII
struct ShiftName {
    uint8_t shifts;
    const char *before, *after;
};
const ShiftName shift_names[] = {
    {0, "[   ", "   ]"},
    {Shf, "[Shift+", "]"},
    {Ctr, "[Contr+", "]"},
    {Ami, "[Amiga+", "]"},
    {Alt, "[Alter+", "]"},
    {Ctr | Shf, "[Ct+Sh+", "]"},
    {Ctr | Ami, "[Ct+Am+", "]"},
    {Ctr | Alt, "[Ct+Al+", "]"},
    {Ami | Shf, "[Am+Sh+", "]"},
    {Ami | Alt, "[Am+Al+", "]"},
    {Alt | Shf, "[Al+Sh+", "]"},
    {Shf | Alt | Ami, "[SAlAm+", "]"},
    {Ctr | Alt | Ami, "[CAlAm+", "]"},
    {Ctr | Shf | Ami, "[CShAm+", "]"},
    {Ctr | Shf | Alt, "[CShAl+", "]"},
    {Ctr | Shf | Alt | Ami, "[CSAAm+", "]"},
};
struct KeyName {
    uint8_t scan;
    const char *name;
};
const KeyName key_names[] = {
    {0x45, "Esc"}, {0x42, "Tab"}, {0x46, "Del"}, {0x41, "Bck"}, {0x5f, "Hlp"}, {0x44, "Ret"}, {0x4c, "Up "},
    {0x4f, "Lft"}, {0x4e, "Rgt"}, {0x4d, "Dwn"}, {0x0f, "N0"},  {0x1d, "N1"},  {0x1e, "N2"},  {0x1f, "N3"},
    {0x2d, "N4"},  {0x2e, "N5"},  {0x2f, "N6"},  {0x3d, "N7"},  {0x3e, "N8"},  {0x3f, "N9"},  {0x3c, "N."},
    {0x43, "Ent"}, {0x50, "F1 "}, {0x51, "F2 "}, {0x52, "F3 "}, {0x53, "F4 "}, {0x54, "F5 "}, {0x55, "F6 "},
    {0x56, "F7 "}, {0x57, "F8 "}, {0x58, "F9 "}, {0x59, "F10"},
};

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

[[noreturn]] void not_done()
{
    run(ed_not_done);
    __builtin_unreachable();
}

[[noreturn]] void out_of_memory()
{
    run(ed_out_of_memory);
    __builtin_unreachable();
}

void sound(uint8_t c) { sound_play(c); }

// The menu routines of AMOSPro.Lib (the editor is a program of its own:
// it calls them through the library's table)
uint32_t lib_all(int routine, AllRegs &r)
{
    Ctx &c = ctx();
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(c.a6);
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    amos_call_all(&r, lib_entry(routine));
    return r.d[0];
}

// MnFind / MnIns: the item at coords (the deepest first), depth of them
MenuItem *mn_find(be_u32 *coords, uint16_t depth)
{
    AllRegs r = {};
    r.a[3] = address(coords);
    r.d[5] = depth;
    return lib_all(L_MnFind, r) ? ptr<MenuItem>(r.a[2]) : nullptr;
}

MenuItem *mn_insert(be_u32 *coords, uint16_t depth)
{
    AllRegs r = {};
    r.a[3] = address(coords);
    r.d[5] = depth;
    lib_all(L_MnIns, r);
    return ptr<MenuItem>(r.a[2]);
}

// MnEff: an item and what it opens
void mn_erase(MenuItem *m)
{
    Regs r;
    r.d[0] = address(m);
    r.d[5] = 0;
    lib_call(L_MnEff, r);
}

void mn_erase_all()
{
    Regs r;
    lib_call(L_MnRaz, r);
}

// MnObjet: the object of a string; 0 out of memory, -1 a bad string
uint32_t mn_object(const uint8_t *s)
{
    Regs r;
    r.a[1] = address(s);
    lib_call(L_MnObjet, r);
    return r.d[0];
}

// Ed_Available: the free memory, chip and fast
uint32_t available() { return hal::available_memory(hal::MemoryPublic | hal::MemoryFast) + hal::available_memory(hal::MemoryPublic | hal::MemoryChip); }

// EdM_Editor / EdM_Program: the menu data of the editor or of the
// program in the data zone (the other one kept in EdM_Copie)
void swap_menus()
{
    IData &d = idata();
    uint8_t *a = d.EdM_Copie;
    uint8_t *b = reinterpret_cast<uint8_t *>(&d) + IData::Mn_SSave;
    for (int i = 0; i < IData::Mn_ESave - IData::Mn_SSave; i++) {
        uint8_t t = a[i];
        a[i] = b[i];
        b[i] = t;
    }
}

void editor_menus()
{
    if (!idata().EdM_Flag) {
        idata().EdM_Flag = 1;
        swap_menus();
    }
}

void program_menus()
{
    if (idata().EdM_Flag) {
        idata().EdM_Flag = 0;
        swap_menus();
    }
}

// EdM_End: all the menu erased
void menus_end()
{
    IData &d = idata();
    editor_menus();
    mn_erase_all();
    if (uint32_t table = d.EdM_Table) {
        d.EdM_Table = 0;
        free_memory(table, d.EdM_TableSize);
    }
}

// Ed_MnOb: one object of an item from a string (a length word, the text;
// none if empty)
void set_object(be_u32 &object, const uint8_t *s)
{
    if (uint32_t o = object) {
        object = 0;
        free_memory(o, peek_w(ptr<uint8_t>(o)));
    }
    if (!peek_w(s))
        return;
    uint32_t o = mn_object(s);
    // (the assembler stopped on an illegal instruction when this failed)
    if (o != 0xFFFFFFFF)
        object = o;
}

// The key of function n as text at to (12 characters, from fill); the
// end of it
uint8_t *key_text(uint8_t *to, uint8_t function)
{
    const uint8_t *keys = function ? function_keys(function) : nullptr;
    if (!keys || keys[0] <= 1)
        return to;
    const ShiftName *s = nullptr;
    for (const ShiftName &n : shift_names)
        if (n.shifts == keys[1]) {
            s = &n;
            break;
        }
    if (!s)
        return to;
    for (const char *c = s->before; *c;)
        *to++ = uint8_t(*c++);
    uint8_t code = keys[0];
    if (!(code & 0x80)) {
        *to++ = '"';
        *to++ = code;
        *to++ = '"';
    } else {
        const KeyName *k = nullptr;
        for (const KeyName &n : key_names)
            if ((0x80 | n.scan) == code) {
                k = &n;
                break;
            }
        if (k)
            for (const char *c = k->name; *c;)
                *to++ = uint8_t(*c++);
        else
        {
            Regs h;
            h.d[0] = code & 0x7F;
            h.d[3] = 2;
            h.a[0] = address(to);
            lib_call(L_LongToHex, h);
            to = ptr<uint8_t>(h.a[0]);
        }
    }
    for (const char *c = s->after; *c;)
        *to++ = uint8_t(*c++);
    to[0] = ' ';
    return to + 1;
}

// EdM_ObCree: the item of definition def with the text, made or changed
MenuItem *make_item(const uint8_t *def, const uint8_t *text, bool active)
{
    IData &d = idata();
    // the place in the tree, below Ed_BufT+256, the deepest first
    uint8_t *top = ptr<uint8_t>(d.Ed_BufT) + 256;
    auto *coords = reinterpret_cast<be_u32 *>(top);
    uint16_t depth = 0;
    for (int i = 0; i < 4; i++) {
        uint8_t n = uint8_t(def[4 + i] - '0');
        if (!n)
            break;
        *--coords = n;
        depth++;
    }
    MenuItem *m = mn_find(coords, depth);
    if (!m)
        m = mn_insert(coords, depth);
    menu::flags(m) &= ~menu::MnOff;
    if (!active)
        menu::flags(m) |= menu::MnOff;
    uint8_t ink0 = uint8_t(def[2] - '0'), ink1 = uint8_t(def[1] - '0');
    m->MnInkA1 = ink0;
    m->MnInkB1 = ink1;
    m->MnInkC1 = ink0;
    m->MnInkA2 = ink1;
    m->MnInkB2 = ink0;
    m->MnInkC2 = ink0;
    // the string: a length word, the text up to a control code
    uint8_t *s = top;
    uint8_t *a1 = s + 2;
    for (const uint8_t *a0 = text;;) {
        uint8_t c = *a0++;
        *a1++ = c;
        if (c < 32)
            break;
    }
    a1--;
    poke_w(s, uint16_t(a1 - s - 2));
    poke_w(s - 2, 0);
    // the key of the function after it (not in the menu bar, nor for the
    // items that open a branch)
    if (d.EdM_Keys && depth != 1 && def[0] >= '0') {
        poke_w(s, uint16_t(peek_w(s) + KeyText + 1));
        for (int i = 0; i < KeyText + 1; i++)
            a1[i] = text[0];
        key_text(a1, uint8_t(def[0] - '0'));
    }
    set_object(m->MnOb1, s);
    set_object(m->MnOb2, s - 2);
    set_object(m->MnOb3, s - 2);
    set_object(m->MnObF, s - 2);
    d.MnChange = 1;
    return m;
}

// EdM_CreObjet: the item of definition def with the text, and its entry
// at *table; the length of the text's entry in its list
uint32_t make_object(const uint8_t *def, uint8_t *&table, const uint8_t *text)
{
    IData &d = idata();
    if (*text) {
        uint8_t *t = table;
        for (int i = 0; i < 4; i++)
            t[i] = uint8_t(def[4 + i] - '0');
        poke_w(t + 4, uint16_t(address(text) - d.EdM_Messages));
        t[6] = uint8_t(def[0] - '0');
        t[7] = uint8_t((address(def) - d.EdM_Definition) >> 3);
        table += 8;
        make_item(def, text, int16_t(int8_t(def[0] - '0')) != 0);
    }
    return uint8_t(2 + text[-1]);
}

// Edt_CountHidden
uint16_t count_hidden()
{
    uint16_t n = 0;
    for (uint32_t w = idata().Edt_List; w; w = ptr<EditWindow>(w)->Edt_Next)
        if (ptr<EditWindow>(w)->Edt_Hidden)
            n++;
    return n;
}

// The next text of a list after text
const uint8_t *skip_text(const uint8_t *text) { return text + 2 + text[-1]; }

// EdM_BranchAMOS: the AMOS menu (the first title) made again, with the
// hidden programs
void branch_amos()
{
    IData &d = idata();
    editor_menus();
    if (!d.MnBase)
        return;
    // the old one erased
    be_u32 *coords = ptr<be_u32>(d.Ed_BufT + 4);
    *coords = 1;
    if (MenuItem *m = mn_find(coords, 1))
        mn_erase(m);
    // the items before the hidden programs
    uint8_t *def = ptr<uint8_t>(d.EdM_MenuAMOS);
    uint8_t *table = ptr<uint8_t>(d.EdM_TableAMOS);
    const uint8_t *text = ptr<uint8_t>(d.EdM_MessAMOS);
    do {
        def[4] = '1';
        text += make_object(def, table, text);
        def += 8;
    } while (*def != '*');
    def += 8;
    uint8_t branch = def[5];
    bool previous = false, next = false;
    uint16_t hidden_count = count_hidden();
    if (hidden_count) {
        int16_t over = int16_t(hidden_count - HiddenMax);
        if (over < 0)
            d.EdM_PosHidden = 0;
        else if (uint16_t(over) < d.EdM_PosHidden)
            d.EdM_PosHidden = uint16_t(over);
        if (d.EdM_PosHidden)
            previous = true;
        if (int16_t(hidden_count) > int16_t(d.EdM_PosHidden + HiddenMax))
            next = true;
        // the title
        def[4] = '1';
        def[5] = branch;
        text += make_object(def, table, text);
        def += 8;
        branch++;
        // ... previous programs ...
        if (previous) {
            def[4] = '1';
            def[5] = branch;
            make_object(def, table, text);
            branch++;
        }
        def += 8;
        text = skip_text(text);
        // the programs, each with its own branch
        uint8_t *def0 = def;
        const uint8_t *text0 = text;
        uint8_t function = '0' + HiddenCommands;
        int16_t left = HiddenMax;
        for (EditWindow *w = hidden(d.EdM_PosHidden); w; w = ptr<EditWindow>(w->Edt_Next)) {
            if (!w->Edt_Hidden)
                continue;
            if (--left < 0)
                break;
            def[4] = '1';
            def[5] = branch;
            // its name
            uint8_t *name = const_cast<uint8_t *>(text);
            *name++ = ' ';
            uint8_t *end = name_into(name, *ptr<Program>(w->Edt_Prg));
            while (*end >= 32)
                end++;
            end[-1] = ' ';
            text += make_object(def, table, text);
            def += 8;
            do {
                def[4] = '1';
                def[5] = branch;
                if (def[0] != '0')
                    def[0] = function++;
                text += make_object(def, table, text);
                def += 8;
            } while (*def != '*');
            def += 8;
            branch++;
            def = def0;
            text = text0;
        }
    }
    // the end of the special part
    do {
        def += 8;
        text = skip_text(text);
    } while (*def != '*');
    def += 8;
    // ... next programs ...
    if (next) {
        def[4] = '1';
        def[5] = branch;
        make_object(def, table, text);
        branch++;
    }
    def += 8;
    text = skip_text(text);
    // the rest
    do {
        def[4] = '1';
        def[5] = branch;
        text += make_object(def, table, text);
        def += 8;
        branch++;
    } while (*def >= 32);
    poke_l(table, 0);
    d.MnChange = 1;
}

// EdM_SetMark: the mark of function n's item on or off
void set_mark(uint8_t function, bool on)
{
    IData &d = idata();
    if (!d.MnBase)
        return;
    const uint8_t *t = ptr<uint8_t>(d.EdM_Table);
    while (t[6] != function)
        t += 8;
    uint8_t *text = ptr<uint8_t>(d.EdM_Messages + int16_t(peek_w(t + 4)));
    const uint8_t *def = ptr<uint8_t>(d.EdM_Definition) + 8 * t[7];
    const char *mark = on ? " ->" : "   ";
    for (int i = 0; mark[i]; i++)
        text[i] = uint8_t(mark[i]);
    make_item(def, text, true);
    d.MnChange = 1;
}

} // namespace

namespace amos::ed {

// EdM_MarkAll
void menu_marks()
{
    IData &d = idata();
    editor_menus();
    set_mark(104, d.EdM_Keys);
    set_mark(75, d.Ed_Insert);
    set_mark(148, d.Ed_Sounds);
}

// EdM_Init: the whole menu made; false if out of memory
bool menu_init()
{
    IData &d = idata();
    menus_end();
    if (available() < 40 * 1024) {
        menus_end();
        return false;
    }
    uint32_t size = HiddenMax * 8 * 5 + 16 + UserMax * 8 + 8 + peek_l(ptr<uint8_t>(d.EdM_Definition - 4)) + 16;
#if !defined(__m68k__)
    size += 4 * 8;
#endif
    d.EdM_TableSize = size;
    d.EdM_Table = memory(size, true);
    if (!d.EdM_Table) {
        menus_end();
        return false;
    }
    d.MnMouse = 0;
    d.MnDFlags[0] = menu::MnTotal;
    for (int i = 1; i < 8; i++)
        d.MnDFlags[i] = menu::MnBar;
    for (int i = 0; i < 8; i++)
        d.MnChoix[i] = 0;
    d.MnAdEc = wdata().EcAdr[EcEdit];
    // the titles and their branches up to the AMOS one
    uint8_t *def = ptr<uint8_t>(d.EdM_Definition);
    uint8_t *table = ptr<uint8_t>(d.EdM_Table);
    const uint8_t *text = ptr<uint8_t>(d.EdM_Messages) + 2;
    do {
        text += make_object(def, table, text);
        def += 8;
    } while (*def != '*');
    def += 8;
    d.EdM_MenuAMOS = address(def);
    d.EdM_MessAMOS = address(text);
    // the user menu
    if (d.EdM_User) {
        const uint8_t *user = ptr<uint8_t>(d.EdM_User) + 2;
        for (uint16_t n = 1; n <= UserMax; n++) {
            uint8_t *b = ptr<uint8_t>(d.Ed_BufT);
            b[0] = uint8_t('0' + UserCommands - 1 + n);
            b[1] = '3';
            b[2] = '0';
            b[3] = '1';
            b[4] = '7';
            b[5] = uint8_t('0' + n);
            b[6] = '0';
            b[7] = '0';
            b[8] = 0;
            // the text in a field of its own (an empty one keeps what
            // the buffer had)
            if (*user) {
                for (int i = 0; i < UserLong; i++)
                    b[8 + i] = ' ';
                b[8 + UserLong] = 0;
                const uint8_t *from = user;
                for (int i = 0; i < UserLong - 2 && *from; i++)
                    b[9 + i] = *from++;
            }
            make_object(b, table, b + 8);
            if (*user)
                poke_w(table - 4, uint16_t(-int16_t(address(user) - d.EdM_User)));
            user += 2 + user[-1];
        }
    }
#if !defined(__m68k__)
    // Configurations already use slot 8 for Help. Pick a free root slot;
    // otherwise labels replace Help objects while selection finds Help IDs.
    // Terminate the static table while determining its occupied coordinates.
    poke_l(table, 0);
    const uint8_t root = clipboard_menu_slot(ptr<uint8_t>(d.EdM_Table));
    if (root) {
        const char *labels[] = {"Clipboard", "System Copy", "System Cut", "System Paste"};
        auto keys = d.EdM_Keys; d.EdM_Keys = 0;
        for (unsigned i = 0; i < 4; ++i) {
            uint8_t *b = ptr<uint8_t>(d.Ed_BufT);
            const uint8_t def[8] = {'1','3','0','1',uint8_t('0' + root),uint8_t(i ? '0' + i : '0'),'0','0'};
            for (unsigned j = 0; j < 8; ++j) b[j] = def[j];
            unsigned n = 0; while (labels[i][n]) { b[16 + n] = labels[i][n]; ++n; }
            b[16 + n] = 0; b[15] = uint8_t(n);
            uint8_t *entry = table;
            make_object(b, table, b + 16);
            entry[6] = uint8_t(i); entry[7] = 0xFF;
        }
        d.EdM_Keys = keys;
    }
#endif
    d.EdM_TableAMOS = address(table);
    branch_amos();
    menu_marks();
    d.MnChoice = 0;
    return true;
}

} // namespace amos::ed

namespace {

// EdM_Error: the menu could not be opened
void menu_error()
{
    Regs r;
    r.d[0] = 1;
    r.a[0] = address(message(185));
    call(ed_alert_rts_entry, r);
    Regs n;
    lib_call(L_Dia_NoMKey, n);
}

// Ed_MnGere: the menu; 1 and the entry of the option chosen (function
// from 0 in *function), 0 if none, -1 if it could not open
struct Choice {
    int32_t result;
    const uint8_t *entry;
    uint16_t function;
};

Choice menu_choice(bool clipboard = false)
{
#if defined(__m68k__)
    (void)clipboard;
#endif
    IData &d = idata();
    editor_menus();
    if (!d.MnBase || hal::available_memory(hal::MemoryPublic) < 40 * 1024)
        return {-1, nullptr, 0};
    sound('H');
    Regs g;
    lib_call(L_MnGere, g);
    if (g.d[0])
        return {-1, nullptr, 0};
    sound('H');
    if (!d.MnChoice)
        return {0, nullptr, 0};
    d.MnChoice = 0;
    uint32_t chosen = 0;
    for (int i = 0; i < 4; i++)
        chosen = chosen << 8 | uint8_t(d.MnChoix[i]);
    for (const uint8_t *t = ptr<uint8_t>(d.EdM_Table); peek_l(t); t += 8) {
        if (peek_l(t) != chosen)
            continue;
#if !defined(__m68k__)
        if (t[7] == 0xFF) {
            if (clipboard && t[6]) return {1, t, 0};
            return {0, nullptr, 0};
        }
#endif
        int16_t function = int16_t(t[6]) - 1;
        if (function < 0)
            break;
        return {1, t, uint16_t(function)};
    }
    return {0, nullptr, 0};
}

// Mn_GetOption: dialogue n, then an option chosen in the menu (a key
// aborts); its function (from 0) and text, or false
struct Option {
    bool chosen;
    uint16_t function;
    const uint8_t *text;
};

Option get_option(uint16_t n)
{
    IData &d = idata();
    dialogue(n);
    Regs e;
    e.d[0] = 1;
    lib_call(L_Dia_EffChannel, e);
    Choice c = {0, nullptr, 0};
    for (;;) {
        boot::wait_vbl_multi();
        Regs k;
        sy_call(sy::Inkey, k);
        if (k.d[1])
            break;
        Regs m;
        sy_call(sy::MouseKey, m);
        if (!(m.d[1] & 2))
            continue;
        c = menu_choice();
        if (c.result)
            break;
    }
    draw_windows();
    if (!c.result)
        return {false, 0, nullptr};
    if (c.result < 0) {
        menu_error();
        return {false, 0, nullptr};
    }
    int16_t offset = int16_t(peek_w(c.entry + 4));
    const uint8_t *text =
        offset >= 0 ? ptr<uint8_t>(d.EdM_Messages + offset) : ptr<uint8_t>(d.EdM_User - offset);
    return {true, c.function, text};
}

uint32_t dialogue_value(uint16_t zone)
{
    Regs r;
    r.d[0] = 1;
    r.d[1] = zone;
    r.d[2] = 0xFFFFFFFF;
    lib_call(L_Dia_GetValue, r);
    return r.d[1];
}

be_u32 *dialogue_variables() { return ptr<be_u32>(idata().Ed_VDialogues); }

void change_text(be_u32 *list, const uint8_t *text, uint16_t n)
{
    if (!text_change(list, text, n))
        out_of_memory();
}

// Ed_UserMenu: an option with no program
void user_menu()
{
    tokenise_current_line();
    dialogue(EdDMnUs);
    place_cursor();
}

// Ed_Prg2Menu: a program (and its command line) for a menu option
void program_to_menu()
{
    IData &d = idata();
    tokenise_current_line();
    Option o = get_option(EdDPrgMn1);
    if (!o.chosen)
        not_done();
    // not the hidden programs' commands, nor the help or configuration
    if (o.function >= HiddenCommands - 1 || (o.function >= 152 && o.function < 181)) {
        dialogue(EdDPrgMnE);
        place_cursor();
        return;
    }
    d.EdC_Changed = 1;
    uint8_t *a = d.Ed_AutoLoad + 3 * o.function;
    uint8_t *empty = ptr<uint8_t>(d.Buffer);
    int16_t answer = 0;
    if (a[0] && a[1]) {
        // the program it has: changed, or taken away
        be_u32 *v = dialogue_variables();
        v[0] = address(o.text);
        v[1] = address(message_of(ptr<uint8_t>(d.Ed_MnPrograms), a[1], nullptr));
        answer = int16_t(dialogue(EdDPrgMn2));
        if (answer == 3)
            not_done();
        empty[0] = 0;
        change_text(&d.Ed_MnPrograms, empty, a[1]);
        if (a[2]) {
            empty[0] = 0;
            change_text(&d.Ed_MnPrograms, empty, a[2]);
        }
    }
    a[0] = a[1] = a[2] = 0;
    if (answer == 2) {
        place_cursor();
        return;
    }
    Regs f;
    f.d[0] = 100;
    if (!uint16_t(call(ed_file_selector, f)))
        not_done();
    path_it();
    be_u32 *v = dialogue_variables();
    v[0] = address(o.text);
    v[1] = d.Name1;
    v[2] = 0;
    v[3] = 48;
    Regs s;
    s.a[0] = address(v + 4);
    s.d[0] = 1;
    s.d[1] = 3;
    lib_call(L_Dia_SetVFlags, s);
    if (int16_t(dialogue(EdDPrgMn3)) != 1)
        not_done();
    Regs g;
    g.a[0] = address(dialogue_variables() + 4);
    g.d[1] = 3;
    lib_call(L_Dia_GetVFlags, g);
    a[0] = uint8_t(g.d[0]) | 0x80;
    uint16_t n = text_free(ptr<uint8_t>(d.Ed_MnPrograms));
    a[1] = uint8_t(n);
    change_text(&d.Ed_MnPrograms, ptr<uint8_t>(d.Name1), n);
    // a command line
    const uint8_t *line = ptr<uint8_t>(dialogue_value(7));
    if (*line) {
        n = text_free(ptr<uint8_t>(d.Ed_MnPrograms));
        a[2] = uint8_t(n);
        change_text(&d.Ed_MnPrograms, line, n);
    }
    place_cursor();
}

// Ed_Key2Menu: a key for a menu option
void key_to_menu()
{
    IData &d = idata();
    tokenise_current_line();
    Option o = get_option(EdDKyMn1);
    if (!o.chosen)
        not_done();
    if (o.function >= HiddenCommands - 1) {
        dialogue(EdDKyMnE);
        place_cursor();
        return;
    }
    // its keys in the table, the old one taken away
    uint8_t *a0 = d.Ed_KFonc;
    for (uint16_t n = o.function; n; n--) {
        do
            a0 += 2;
        while (*a0);
        a0++;
        if (*a0 == 0xFF) {
            dialogue(EdDKyMnE);
            place_cursor();
            return;
        }
    }
    a0[0] = 1;
    a0[1] = 0;
    menu_init();
    if (int16_t(dialogue(EdDKyMn2)))
        not_done();
    uint32_t key = ptr<Dialog>(d.Ed_ADialogues)->Dia_LastKey;
    if (!key)
        not_done();
    if (key_function(key) >= 0) {
        // (only a key used already marked the configuration changed)
        d.EdC_Changed = 1;
        if (int16_t(dialogue(EdDKyMn3)) != 1)
            not_done();
    }
    uint8_t ascii = uint8_t(key);
    if (ascii >= 'a' && ascii <= 'z')
        ascii -= 0x20;
    if (ascii >= 'A' && ascii <= 'Z')
        a0[0] = ascii;
    else
        a0[0] = uint8_t(key >> 16) | 0x80;
    uint8_t shifts = uint8_t(key >> 24), groups = 0;
    static const uint8_t shift_groups[] = {Shf, Ami, Ctr, Alt};
    for (uint8_t g : shift_groups)
        if (shifts & g)
            groups |= g;
    a0[1] = groups;
    if (d.EdM_Keys)
        menu_init();
    place_cursor();
}

// Ed_AddUser: a new option in the user menu, then its program and key
void add_user()
{
    IData &d = idata();
    tokenise_current_line();
    uint16_t n = text_free(ptr<uint8_t>(d.EdM_User));
    if (n >= UserMax) {
        dialogue(EdDMnUs2);
        place_cursor();
        return;
    }
    d.EdC_Changed = 1;
    be_u32 *v = dialogue_variables();
    v[2] = 0;
    v[3] = UserLong;
    if (int16_t(dialogue(EdDMnUsA)) != 1)
        not_done();
    const uint8_t *name = ptr<uint8_t>(dialogue_value(3));
    if (!*name)
        not_done();
    change_text(&d.EdM_User, name, n);
    menu_init();
    program_to_menu();
    key_to_menu();
    place_cursor();
}

// Ed_DelUser: an option of the user menu taken away, with its program
// and key
void delete_user()
{
    IData &d = idata();
    tokenise_current_line();
    Option o = get_option(EdDMnUsD);
    if (!o.chosen)
        not_done();
    if (o.function < UserCommands - 1 || o.function >= UserCommands - 1 + UserMax) {
        dialogue(EdDMnUsE);
        Regs l;
        call(ed_loop, l);
        __builtin_unreachable();
    }
    d.EdC_Changed = 1;
    uint8_t *empty = ptr<uint8_t>(d.Buffer);
    poke_w(empty, 0);
    change_text(&d.EdM_User, empty, uint16_t(o.function - (UserCommands - 2)));
    uint8_t *a = d.Ed_AutoLoad + 3 * o.function;
    if (a[0]) {
        a[0] = 0;
        if (a[1])
            change_text(&d.Ed_MnPrograms, empty, a[1]);
        if (a[2])
            change_text(&d.Ed_MnPrograms, empty, a[2]);
    }
    // (the assembler took away the key of the function before it, giving
    // Ed_Fonc2Ky the function counted from 0)
    if (uint8_t *keys = const_cast<uint8_t *>(function_keys(uint8_t(o.function + 1)))) {
        keys[0] = 1;
        keys[1] = 0;
    }
    menu_init();
    place_cursor();
}

// Ed_SetAutoSave: the minutes between the automatic saves
void set_autosave()
{
    IData &d = idata();
    tokenise_current_line();
    dialogue_variables()[2] = d.Ed_AutoSaveMn;
    if (int16_t(dialogue(EdDASave)) != 1)
        not_done();
    uint32_t minutes = dialogue_value(3);
    if (minutes == d.Ed_AutoSaveMn) {
        place_cursor();
        return;
    }
    d.Ed_AutoSaveMn = minutes;
    Regs n;
    ec_call(ec::NTSC, n);
    uint32_t frames = uint16_t(n.d[1]) ? 60 : 50;
    // (the assembler's 16-bit multiplications)
    d.Ed_AutoSave = uint32_t(uint16_t(uint16_t(minutes) * frames)) * 60;
    d.Ed_AutoSaveRef = 0xFFFFFFFF;
    d.EdC_Changed = 1;
    place_cursor();
}

// Ed_SaveAuto: every changed program saved (one asked for each)
void save_all()
{
    Ctx &c = ctx();
    uint8_t *a4 = c.a4, *a6 = c.a6;
    bool first = true;
    for (uint32_t w = idata().Edt_List; w; w = ptr<EditWindow>(w)->Edt_Next) {
        EditWindow *e = ptr<EditWindow>(w);
        if (e->Edt_Hidden || e->Edt_LinkPrev)
            continue;
        if (first)
            sound('I');
        first = false;
        c.a4 = reinterpret_cast<uint8_t *>(e);
        c.a6 = ptr<uint8_t>(e->Edt_Prg);
        run(ed_saved);
    }
    c.a4 = a4;
    c.a6 = a6;
}

// EdK_MarkAll: the Insert button as the mode
void insert_button()
{
    IData &d = idata();
    Button *b = reinterpret_cast<Button *>(d.Ed_Boutons + (BtInsert - 1) * sizeof(Button));
    b->Bt_Pos = uint16_t((d.Ed_Insert & 1) ^ 1);
    Regs r;
    r.d[0] = uint16_t(b->Bt_Image + b->Bt_Pos);
    r.d[1] = b->Bt_X;
    r.d[2] = b->Bt_Y;
    call(ed_unpack, r);
}

// Ed_Ins / Ed_SamOn / Ed_ShowKey
void flip_insert()
{
    IData &d = idata();
    d.Ed_Insert = uint8_t(~d.Ed_Insert);
    d.Ed_EtatAff |= EtAIns;
    menu_marks();
    insert_button();
}

void flip_sounds()
{
    IData &d = idata();
    d.Ed_Sounds = uint8_t(~d.Ed_Sounds);
    menu_marks();
    d.EdC_Changed = 1;
    Regs r;
    call(ed_sounds_changed_entry, r);
}

void flip_keys()
{
    tokenise_current_line();
    idata().EdM_Keys = uint8_t(~idata().EdM_Keys);
    menu_init();
}

// EdM_PrevHidden / EdM_NextHidden: the hidden programs of the AMOS menu
// scrolled
void previous_hidden()
{
    IData &d = idata();
    tokenise_current_line();
    if (!d.EdM_PosHidden)
        not_done();
    int16_t p = int16_t(d.EdM_PosHidden - (HiddenMax - 1));
    d.EdM_PosHidden = uint16_t(p < 0 ? 0 : p);
    branch_amos();
}

void next_hidden()
{
    tokenise_current_line();
    idata().EdM_PosHidden += HiddenMax - 1;
    branch_amos();
}

} // namespace


extern "C" {
void ed_menus_editor(AllRegs *) { editor_menus(); }
void ed_menus_program(AllRegs *) { program_menus(); }

// Edt_ClearVar: the variables of the current program cleared
void ed_clear_var_fn(AllRegs *)
{
    program_menus();
    Regs r;
    lib_call(L_Prg_SetBanks, r);
    Regs c;
    lib_call(L_ClearVar, c);
}
void ed_menus_end(AllRegs *) { menus_end(); }
void ed_menu_error_fn(AllRegs *) { menu_error(); }
void ed_menu_init(AllRegs *r) { r->d[0] = menu_init() ? 0 : 1; }
void ed_menu_marks(AllRegs *) { menu_marks(); }
void ed_branch_amos_fn(AllRegs *) { branch_amos(); }
void ed_previous_hidden(AllRegs *) { previous_hidden(); }
void ed_next_hidden(AllRegs *) { next_hidden(); }
void ed_flip_keys(AllRegs *) { flip_keys(); }
void ed_flip_insert(AllRegs *) { flip_insert(); }
void ed_flip_sounds(AllRegs *) { flip_sounds(); }
void ed_insert_button(AllRegs *) { insert_button(); }
void ed_user_menu(AllRegs *) { user_menu(); }
void ed_add_user(AllRegs *) { add_user(); }
void ed_delete_user(AllRegs *) { delete_user(); }
void ed_program_to_menu(AllRegs *) { program_to_menu(); }
void ed_key_to_menu(AllRegs *) { key_to_menu(); }
void ed_set_autosave(AllRegs *) { set_autosave(); }
void ed_save_all(AllRegs *) { save_all(); }

// Ed_MnGere: d0 1 (d2 the function, from 0; a0 its entry), 0 or -1
void ed_menu_fn(AllRegs *r)
{
    Choice c = menu_choice(true);
#if !defined(__m68k__)
    if (c.result > 0 && c.entry[7] == 0xFF) {
        system_clipboard_command(static_cast<hal::ClipboardCommand>(c.entry[6]));
        r->d[0] = 0;
        return;
    }
#endif
    r->d[0] = uint32_t(c.result);
    if (c.result > 0) {
        r->d[2] = c.function;
        r->a[0] = address(c.entry);
    }
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_menu.inc"
#endif
