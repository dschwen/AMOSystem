// The editor's start and end (+Edit.s Ed_Cold, Ed_Title, Ed_Computee,
// Ed_LoadAllConfig, Ed_FreeAllConfig, Ed_End, Ed_KillEditor), its screens
// opened and closed (Ed_OpenEditor, Ed_OpenIt, Ed_CloseEditor, Ed_CloseIt),
// the top of the screen (Ed_DrawTop) and the tokeniser's tables (Tok_Init,
// Tok_Del).
//
// The tokeniser's tables (AdTTokens, one for each extension): their
// size, then 27 words: for each letter its list's offset from the table,
// then a 0; the lists of the offsets of the instructions starting with
// the letter (from the extension's tokens), each ending with 0.

#include "editor/editor.h"
#include "core/amiga.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EcFonc = 8, EcEdit = 9;
constexpr uint16_t Hide = 1 << (8 + 7), AWChange = 1 << (8 + 2);
constexpr uint16_t TitleSy = 16, EtatSy = 11, BasSy = 5, YTop = 0;
constexpr uint16_t Pics = 1, ButtonPics = 11, ButtonsZones = 128, ButtonSx = 32, ButtonSy = 16;
constexpr uint16_t TitleSx = ButtonSx * 5, ButtonsX = ButtonSx + TitleSx;
constexpr uint16_t BtInsert = 10;
constexpr uint8_t BtFlagOnOf = 1 << 2;
constexpr uint32_t BufTSize = 512 + 256;        // EdBTT
constexpr int Extensions = 26;

uint32_t run(uint8_t *routine)
{
    Regs r;
    return call(routine, r);
}

uint32_t run_d0(uint8_t *routine, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    return call(routine, r);
}

void ec_fn(int function, uint32_t d1 = 0)
{
    Regs r;
    r.d[1] = d1;
    ec_call(function, r);
}

void use(EditWindow *w)
{
    ctx().a4 = reinterpret_cast<uint8_t *>(w);
    ctx().a6 = w ? ptr<uint8_t>(w->Edt_Prg) : nullptr;
}

void add_path(uint32_t text)
{
    Regs p;
    p.a[0] = text;
    boot::sys_add_path(ptr<char>(p.a[0]));
}

uint32_t system_text(uint16_t n)
{
    Regs m;
    m.d[0] = n;
    call(ed_get_systeme, m);
    return m.a[0];
}

// The tokeniser's tables
bool tokens_init()
{
    IData &d = idata();
    auto end = [](bool ok) {
        Regs t;
        t.d[0] = 0;
        t.a[0] = boot::temp_buffer(t.d[0]);
        return ok;
    };
    if (d.Ed_BufT)
        return end(true);
    d.Ed_BufT = memory(BufTSize, true);
    if (!d.Ed_BufT)
        return end(false);
    Regs t;
    t.d[0] = 1024 * 12;
    t.a[0] = boot::temp_buffer(t.d[0]);
    if (!d.TempBuffer)
        return end(false);
    for (int slot = 0; slot < Extensions; slot++) {
        uint32_t tokens = d.AdTokens[slot];
        if (!tokens)
            continue;
        // the first letters and the offsets of the instructions
        uint8_t *letters = ptr<uint8_t>(d.TempBuffer);
        uint8_t *offsets = letters + 2048;
        uint8_t *a0 = letters, *a2 = offsets;
        const uint8_t *a1 = ptr<uint8_t>(tokens + 6);
        do {
            uint32_t entry = address(a1);
            a1 += 4;
            uint8_t c;
            for (;;) {
                c = *a1++;
                if (c & 0x80) {
                    c &= 0x7F;
                    a1--;
                }
                if (c != '!' && c != ' ')
                    break;
            }
            if (c >= 'A' && c <= 'Z')
                c += 32;
            if (c >= 'a' && c <= 'z') {
                poke_w(a0, c);
                a0 += 2;
                poke_w(a2, uint16_t(entry - tokens));
                a2 += 2;
            }
            // the next instruction: past the name, the parameters, even
            while (!(*a1++ & 0x80))
                ;
            while (!(*a1++ & 0x80))
                ;
            if (address(a1) & 1)
                a1++;
        } while (peek_w(a1));
        poke_w(a0, 0xFFFF);
        a0 += 2;
        // the table of the letters
        uint32_t size = uint32_t(a0 - letters) + 26 * 4 + 2 + 4;
        uint8_t *table = ptr<uint8_t>(memory(size, true));
        if (!table)
            return end(false);
        d.AdTTokens[slot] = address(table);
        poke_l(table, size);
        uint8_t *heads = table + 4;
        uint8_t *lists = heads + 26 * 2 + 2;
        for (uint8_t letter = 'a'; letter <= 'z'; letter++) {
            poke_w(heads, uint16_t(lists - table));
            heads += 2;
            for (const uint8_t *l = letters; peek_w(l) != 0xFFFF; l += 2)
                if (peek_w(l) == letter) {
                    poke_w(lists, peek_w(l + 2048));
                    lists += 2;
                }
            poke_w(lists, 0);
            lists += 2;
        }
        poke_w(heads, 0);
    }
    return end(true);
}

void tokens_end()
{
    IData &d = idata();
    if (!d.Ed_BufT)
        return;
    free_memory(d.Ed_BufT, BufTSize);
    d.Ed_BufT = 0;
    for (int slot = 0; slot < Extensions; slot++)
        if (uint32_t t = d.AdTTokens[slot]) {
            d.AdTTokens[slot] = 0;
            free_memory(t, peek_l(ptr<uint8_t>(t)));
        }
}

// Ed_CloseIt: everything the opening made, away
void close_it()
{
    IData &d = idata();
    set_banks();
    run(ed_undo_free_all_entry);
    run(ed_escape_forget_all_entry);
    all_warnings_end();
    run(ed_block_free_entry);
    run(ed_menus_end_entry);
    run(ed_dialogues_close_entry);
    run(ed_delete_all_zones_entry);
    ec_fn(ec::Del, EcEdit);
    ec_fn(ec::Del, EcFonc);
    tokens_end();
    if (uint32_t b = d.Ed_BufE) {
        d.Ed_BufE = 0;
        free_memory(b, uint32_t(d.Ed_Ty) * 256);
    }
}

// Dia_RScOpen: screen n of the resource's pictures; 0 if it failed
Screen *open_screen(uint16_t n, uint16_t width, uint16_t height, uint32_t flash, uint32_t interlace)
{
    IData &d = idata();
    Regs r;
    r.d[0] = n;
    r.d[1] = width;
    r.d[2] = height;
    r.d[3] = flash;
    r.d[4] = interlace;
    r.a[0] = d.Ed_Resource + peek_l(ptr<uint8_t>(d.Ed_Resource + 2));
    lib_call(L_Dia_RScOpen, r);
    if (uint16_t(r.d[0]))
        return nullptr;
    return ptr<Screen>(r.a[0]);
}

// Ed_OpenIt: the editor's screens and what it needs; false if out of
// memory
bool open_it()
{
    IData &d = idata();
    set_banks();
    run(ed_menus_editor_entry);
    // a new configuration: the windows made to fit
    if (d.EdC_Modified) {
        d.EdC_Modified = 0;
        if (EditWindow *w = ptr<EditWindow>(d.Edt_Current)) {
            ctx().a4 = reinterpret_cast<uint8_t *>(w);
            if (uint16_t(run_d0(ed_max_size_entry, 0xFFFFFFFF)) != w->Edt_WindTy) {
                run_d0(ed_shrink_all_entry, 0);
                uint16_t lines = uint16_t(run_d0(ed_max_size_entry, 0xFFFFFFFF));
                w->Edt_WindTy = lines;
                if (int16_t(lines) <= 0) {
                    // the smallest windows, the screen made as high
                    w->Edt_WindTy = 1;
                    uint16_t height = TitleSy;
                    for (EditWindow *e = ptr<EditWindow>(d.Edt_List); e; e = ptr<EditWindow>(e->Edt_Next))
                        if (!e->Edt_Hidden)
                            height = uint16_t(height + EtatSy + BasSy + (e->Edt_WindTy << 3));
                    d.Ed_Sy = height;
                }
            }
            // the cursors within the width
            uint16_t last = uint16_t((d.Ed_Sx >> 3) - 3);
            for (EditWindow *e = ptr<EditWindow>(d.Edt_List); e; e = ptr<EditWindow>(e->Edt_Next)) {
                if (e->Edt_Hidden || e->Edt_XCu < last)
                    continue;
                e->Edt_XPos = uint16_t(e->Edt_XPos + e->Edt_XCu - last);
                e->Edt_XCu = last;
            }
        }
    }
    run(ed_config_set_palette_entry);
    // the screen of the buttons
    Screen *s = open_screen(EcFonc, d.Ed_Sx, TitleSy, 0, 0);
    if (!s)
        return false;
    s->EcFlags |= Hide;
    s->EcAWX = d.Ed_Wx;
    Regs z;
    z.d[1] = 20;
    sy_call(sy::ResZone, z);
    if (z.d[0])
        return false;
    // the editor's screen
    uint16_t height = uint16_t((d.Ed_Sy + 7) & 0xFFF8);
    d.Ed_Ty = uint16_t((height - 16 - YTop) >> 3);
    s = open_screen(EcEdit, d.Ed_Sx, height, 1, d.Ed_Inter);
    if (!s)
        return false;
    s->EcFlags |= Hide;
    s->EcAWX = d.Ed_Wx;
    s->EcAWY = d.Ed_Wy;
    run_d0(ed_status_text_entry, 10);
    d.Ed_BufE = memory(uint32_t(d.Ed_Ty) * 256, false);
    if (!d.Ed_BufE)
        return false;
    d.Ed_WMax = uint16_t(uint16_t(d.Ed_Ty - 6) / 3);
    if (!tokens_init() || run(ed_dialogues_open_entry) || !menu_init())
        return false;
    run(ed_undo_create_all_entry);
    d.EdC_Modified = 0;
    return true;
}

// Ed_OpenEditor: three tries, with memory made each time (the banks
// erased, if allowed); false if out of memory
bool open_editor()
{
    IData &d = idata();
    if (d.Ed_Opened)
        return true;
    for (int attempt = 0; attempt < 3; attempt++) {
        if (open_it()) {
            d.Ed_Opened = 1;
            return true;
        }
        close_it();
        if (attempt == 2)
            break;
        run(ed_menu_program);
        use(ptr<EditWindow>(d.Edt_Current));
        Regs b;
        lib_call(L_Prg_SetBanks, b);
        Regs m;
        lib_call(attempt ? L_MemDelBanks : L_MemMaximum, m);
        if (attempt) {
            if (!m.d[0])
                run(ed_system);         // not allowed: the end
            bank_erase_all();
            bank_changed();
        }
    }
    return false;
}

// Ed_CloseEditor
void close_editor()
{
    IData &d = idata();
    if (!d.Ed_Opened)
        return;
    d.Ed_Opened = 0;
    close_it();
}

// Ed_LoadAllConfig: the configuration, the resource, the macros, the
// sounds; their error, or 0
uint32_t load_all_config()
{
    Regs m;
    m.d[0] = 7;
    m.a[0] = address(boot::sys_message(m.d[0]));
    add_path(m.a[0]);
    if (uint32_t e = run(ed_config_load_entry))
        return e;
    add_path(system_text(45));
    if (uint32_t e = run(ed_resource_load_entry))
        return e;
    add_path(system_text(46));
    if (uint32_t e = run(ed_macro_load_entry))
        return e;
    run(ed_sounds_changed_entry);
    return 0;
}

// Ed_FreeAllConfig
void free_all_config()
{
    run(ed_sounds_end_entry);
    run(ed_macros_free_entry);
    run(ed_config_free_entry);
    run(ed_resource_free_entry);
}

// Ed_Computee: a window for each program not edited yet; false if out of
// memory
bool make_windows()
{
    IData &d = idata();
    ctx().a4 = ptr<uint8_t>(d.Edt_List);
    d.Ed_WMax = 2;
    for (Program *p = ptr<Program>(d.Prg_List); p; p = ptr<Program>(p->Prg_Next)) {
        if (p->Prg_Edited)
            continue;
        ctx().a6 = reinterpret_cast<uint8_t *>(p);
        if (open_window(true, -1))
            return false;
        window().Edt_Prg = address(p);
        p->Prg_Edited++;
    }
    return true;
}

// Ed_Cold: the editor started (again after Kill Editor); its error or 0
uint32_t cold()
{
    IData &d = idata();
    d.Ed_Ok = 0;
    uint32_t error = 1;
    uint32_t chip = hal::available_memory(hal::MemoryPublic | hal::MemoryChip), fast = hal::available_memory(hal::MemoryPublic | hal::MemoryFast);
    if (chip >= 50 * 1024 && chip + fast >= 110 * 1024) {
        if (d.Ed_Warm) {
            error = load_all_config();
            if (!error)
                return 0;
        } else {
            error = load_all_config();
            if (!error)
                error = make_windows() ? 0 : 1;
            if (!error)
                error = open_editor() ? 0 : 1;
            if (!error) {
                d.Ed_Warm = 1;
                d.EsFlag = 0xFFFF;
                d.Direct = 0;
                return 0;
            }
        }
    }
    close_editor();
    free_all_config();
    return error;
}

// Ed_End
void end()
{
    IData &d = idata();
    if (d.Ed_Ok)
        run(ed_delete_windows_entry);
    memory_release(&d.Ed_Prg2ReLoad);
    close_editor();
    free_all_config();
}

// Ed_DrawTop: the logo, the buttons and the memory gauges
void draw_top()
{
    IData &d = idata();
    Regs z;
    z.d[1] = ButtonsZones + 15;
    sy_call(sy::ResZone, z);
    unpack(Pics, ButtonSx, YTop);
    int16_t left = int16_t(d.Ed_Sx - TitleSx);
    Button *b = reinterpret_cast<Button *>(d.Ed_Boutons);
    uint16_t x = ButtonsX, picture = ButtonPics, zone = ButtonsZones;
    for (uint16_t n = 1; n <= 12; n++, b++) {
        b->Bt_Number = n;
        if (n == 1) {
            b->Bt_X = 0;                        // DIRECT
        } else if (n == 2) {
            b->Bt_X = uint16_t(d.Ed_Sx - ButtonSx);   // WB
        } else {
            b->Bt_X = x;
            x += ButtonSx;
        }
        b->Bt_Y = YTop;
        b->Bt_Image = picture;
        b->Bt_Zone = zone;
        b->Bt_Routines = address(ed_button_routines);
        b->Bt_RDraw = 6;
        b->Bt_RPos = 0;
        b->Bt_RChange = 0;
        b->Bt_Flags = 0;
        b->Bt_Pos = 0;
        if (n == BtInsert) {
            b->Bt_Flags |= BtFlagOnOf;
            if (!d.Ed_Insert)
                b->Bt_Pos = 1;
        }
        b->Bt_Sx = ButtonSx;
        b->Bt_Sy = ButtonSy;
        picture += 2;
        zone++;
        Regs i;
        i.a[0] = address(b);
        i.d[0] = 1;
        lib_call(L_Bt_Init, i);
        left = int16_t(left - ButtonSx);
    }
    b->Bt_Number = 0;
    memory_draw(int16_t(x), YTop, uint16_t(left), false);
    memory_show();
}

} // namespace

extern "C" {
void ed_cold(AllRegs *r) { r->d[0] = cold(); }

// Ed_Title: the editor shown, the last session, the title
void ed_title(AllRegs *)
{
    run(ed_appear);
    run(ed_warm_start_entry);
    sound_play('A');
    run(ed_about_entry);
    place_cursor();
}

void ed_end(AllRegs *) { end(); }
void ed_kill_editor(AllRegs *)
{
    close_editor();
    free_all_config();
    idata().Ed_Ok = 0;
}
void ed_open_editor_fn(AllRegs *r) { r->d[0] = open_editor() ? 0 : 1; }
void ed_close_editor_fn(AllRegs *) { close_editor(); }
void ed_draw_top_fn(AllRegs *) { draw_top(); }
void ed_tokens_init(AllRegs *r) { r->d[0] = tokens_init() ? 0 : 1; }
void ed_tokens_end(AllRegs *) { tokens_end(); }
}
