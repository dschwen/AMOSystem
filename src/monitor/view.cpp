// The monitor's top screen (+Monitor.s): the program's screen seen in its
// view (View_*), moved with the arrows, the buttons (ABouton, Aff_Bouton,
// Tt_UPack), the texts drawn (Tt_Text), the monitor's screens shown and
// hidden; and the patches the program's screens call back: Screen To
// Front/Back (Tt_PScFront), Screen Copy (Tt_PScCopy), the tests'
// (TstPatch).
//
// ViewScrols: for each screen 0-7 the view's position in it (x, y) and the
// screen's size (lines halved for a low resolution one).

#include "monitor/monitor.h"

#include <graphics/gfx.h>
#include <graphics/rastport.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

#undef NTSC

using namespace amos;
using namespace amos::monitor;

namespace {

constexpr uint16_t RedX = 16, RedY = 9, RedTX = 320, RedTY = 100;
constexpr int16_t BtX = 384, BtY = 35;
constexpr uint16_t NButtons = 16;

// Tt_Boutons: the pictures (off, on), the place, the zone and its place
// (bit 0: from the offset rather than from the button)
const uint16_t buttons[NButtons][9] = {
    {18, 19, 0 * 48, 2 * 24, 1, 0, 0, 48, 24},  {20, 21, 1 * 48, 2 * 24, 2, 0, 0, 48, 24},
    {22, 23, 2 * 48, 2 * 24, 3, 0, 0, 48, 24},  {24, 25, 3 * 48, 2 * 24, 4, 0, 0, 48, 24},
    {26, 27, 4 * 48, 2 * 24, 5, 0, 0, 48, 24},  {2, 3, 0 * 48, 0 * 24, 6, 32, 0, 32, 16},
    {2, 4, 0 * 48, 0 * 24, 7, 32, 32, 32, 16},  {2, 5, 0 * 48, 0 * 24, 8, 0, 16, 32, 16},
    {2, 6, 0 * 48, 0 * 24, 9, 64, 16, 32, 16},  {2, 7, 0 * 48, 0 * 24, 10, 32, 16, 32, 16},
    {8, 9, 2 * 48, 0 * 24, 11, 0, 0, 96, 24},   {10, 11, 4 * 48, 0 * 24, 12, 0, 0, 48, 24},
    {0, 0, 0 + 1, 0 + 1, 13, 16, 8, 320, 100},  {16, 17, 2 * 48, 1 * 24, 14, 0, 0, 48, 24},
    {14, 15, 3 * 48, 1 * 24, 15, 0, 0, 48, 24}, {12, 13, 4 * 48, 1 * 24, 16, 0, 0, 48, 24},
};

const uint8_t acceleration[] = {1, 2, 4, 8};

// Info_View / Info_Buf / Info_ViewOff: x, y, pens, mode, the text
struct Text {
    int16_t x, y;
    int8_t back, front;
    int16_t mode;
    const char *text;
};
const Text ViewTitle = {134, 6, 0x0D, 0x0E, 1, "Screen # "};
const Text ViewNumber = {134 + 8 * 9 - 3, 6, 0x0D, 0x0E, 1, ""};
const Text ViewNone = {126, 6, 0x0D, 0x0E, 1, " No screens "};

Screen *screen(uint16_t n) { return ptr<Screen>(wdata().EcAdr[n]); }

void recop()
{
    Regs v;
    sy_call(sy::WaitVbl, v);
    Regs c;
    ec_call(ec::CopForce, c);
}

uint8_t *number_text() { return ptr<uint8_t>(idata().Buffer) + 1024 - 16; }

// Tt_Text: a text of the top screen (an empty one: the number made in the
// buffer)
void draw_text(const Text &t)
{
    struct RastPort *rp = ptr<struct RastPort>(wdata().RastPort);
    if (t.x >= 0)
        rp->cp_x = t.x;
    if (t.y >= 0)
        rp->cp_y = t.y;
    if (t.back >= 0)
        hal::set_b_pen(rp, uint32_t(t.back));
    if (t.front >= 0)
        hal::set_a_pen(rp, uint32_t(t.front));
    if (t.mode >= 0)
        hal::set_draw_mode(rp, uint32_t(t.mode));
    const char *s = *t.text ? t.text : reinterpret_cast<const char *>(number_text());
    uint16_t n = 0;
    while (s[n])
        n++;
    hal::text(rp, s, n);
    hal::text_length(rp, s, n);
    // (the assembler added the number of characters, the library's call
    // having kept its d0)
    rp->cp_x = int16_t(rp->cp_x + n - 8);
}

// View_Info: the number of the screen seen
void view_info()
{
    MonData &m = mon();
    if (int16_t(m.Tt_View) < 0) {
        draw_text(ViewNone);
        return;
    }
    draw_text(ViewTitle);
    uint8_t *b = number_text();
    poke_l(b, 0x20202020);
    poke_l(b + 4, 0);
    Regs r;
    r.d[0] = uint32_t(int32_t(int16_t(m.Tt_View)));
    r.a[0] = address(b);
    lib_call(L_LongToDec, r);
    draw_text(ViewNumber);
}

// View_Ec / View_GEc: the next screen open after n (none: -1)
void view_next(int16_t n)
{
    MonData &m = mon();
    int16_t found = -1;
    for (int i = 0; i < 8; i++) {
        if (++n >= 8)
            n = 0;
        if (wdata().EcAdr[n]) {
            found = n;
            break;
        }
    }
    m.Tt_View = uint16_t(found);
    view_info();
}

be_u16 *view_scroll()
{
    MonData &m = mon();
    if (int16_t(m.Tt_View) < 0)
        return nullptr;
    return &m.ViewScrols[4 * m.Tt_View];
}

// View_Cls
void view_clear()
{
    Regs r;
    r.d[1] = 0;
    r.d[2] = RedX;
    r.d[3] = RedY;
    r.d[4] = RedX + RedTX;
    r.d[5] = RedY + RedTY;
    ec_call(ec::ClsEc, r);
}

// View_Aff: the program's screen copied into the view (a low resolution
// one halved in height)
void view_show()
{
    MonData &m = mon();
    IData &d = idata();
    for (;;) {
        int16_t n = int16_t(m.Tt_View);
        Screen *s = n >= 0 ? screen(uint16_t(n)) : nullptr;
        if (!s) {
            view_next(n);
            if (int16_t(m.Tt_View) >= 0)
                continue;
            view_clear();
            m.Tt_ViewAd = 0;
            return;
        }
        if (address(s) != m.Tt_ViewAd || s->EcTx != m.Tt_ViewTX || s->EcTy != m.Tt_ViewTY ||
            s->EcNPlan != m.Tt_ViewNP)
            view_clear();
        m.Tt_ViewTX = s->EcTx;
        m.Tt_ViewTY = s->EcTy;
        m.Tt_ViewNP = s->EcNPlan;
        m.Tt_ViewAd = address(s);
        Regs v;
        sy_call(sy::WaitVbl, v);
        be_u16 *scroll = view_scroll();
        uint16_t x = scroll[0], y = scroll[1];
        d.SccEcO = address(s);
        d.SccEcD = address(screen(Ecran1));
        Regs c;
        c.a[0] = address(s);
        c.a[1] = d.SccEcD;
        c.d[0] = x;
        c.d[1] = y;
        c.d[2] = RedX;
        c.d[3] = RedY;
        c.d[4] = uint16_t(RedTX + x);
        c.d[5] = uint16_t(RedTY + y);
        c.d[6] = 0xCC;
        if (int16_t(s->EcCon0) < 0) {
            lib_call(L_Sco0, c);
        } else {
            // every other line of a low resolution screen
            uint16_t ty = s->EcTy, line = s->EcTLigne;
            s->EcTy = uint16_t(ty >> 1);
            s->EcTLigne = uint16_t(line << 1);
            lib_call(L_Sco0, c);
            s->EcTLigne = line;
            s->EcTy = ty;
        }
        recop();
        return;
    }
}

// Tt_UPack: picture n of the resource at x, y of the current screen
void unpack(uint16_t n, int32_t x, int32_t y)
{
    const uint8_t *g = ptr<uint8_t>(mon().Tt_Resource);
    g += peek_l(g + 2);
    Regs r;
    r.a[0] = address(g + peek_l(g + 4 * n - 2));
    r.a[1] = wdata().EcCourant;
    r.d[1] = uint32_t(x);
    r.d[2] = uint32_t(y);
    lib_call(L_UnPack_Bitmap, r);
}

// Aff_Bouton / ABouton: button n (from 1) drawn, on or off, and its zone
void button(uint16_t n, bool on)
{
    const uint16_t *b = buttons[n - 1];
    int16_t ox = BtX, oy = BtY;
    uint16_t picture = on ? b[1] : b[0];
    int16_t x = int16_t(b[2] & ~1u), y = int16_t(b[3] & ~1u);
    if (!(b[2] & 1))
        x = int16_t(x + ox);
    if (!(b[3] & 1))
        y = int16_t(y + oy);
    if (picture)
        unpack(picture, x, y);
    if (!b[4])
        return;
    int16_t x1 = int16_t((b[5] & 1 ? ox : x) + int16_t(b[5] & ~1u));
    int16_t y1 = int16_t((b[6] & 1 ? oy : y) + int16_t(b[6] & ~1u));
    int16_t x2 = int16_t((b[7] & 1 ? ox : x1) + int16_t(b[7] & ~1u));
    int16_t y2 = int16_t((b[8] & 1 ? oy : y1) + int16_t(b[8] & ~1u));
    Regs z;
    z.d[1] = b[4];
    z.d[2] = uint32_t(int32_t(x1));
    z.d[3] = uint32_t(int32_t(y1));
    z.d[4] = uint32_t(int32_t(x2));
    z.d[5] = uint32_t(int32_t(y2));
    sy_call(sy::SetZone, z);
}

void all_buttons()
{
    for (uint16_t n = NButtons; n; n--)
        button(n, false);
}

// Tt_Show / Tt_Hide
void show_screens(bool show)
{
    MonData &m = mon();
    uint8_t state = show ? 0 : 0xFF;
    if (m.Tt_NotHere == state)
        return;
    m.Tt_NotHere = state;
    Regs a;
    a.d[1] = Ecran1;
    a.d[2] = show ? 0 : 0xFFFFFFFF;
    ec_call(ec::EHide, a);
    Regs b;
    b.d[1] = Ecran2;
    b.d[2] = a.d[2];
    ec_call(ec::EHide, b);
    recop();
}

// Tt_Mouse: the mouse shown, free to go anywhere
void free_mouse()
{
    Regs s;
    s.d[1] = 0xFFFFFFFF;
    sy_call(sy::Show, s);
    Regs l;
    l.d[1] = 64;
    l.d[2] = 30;
    l.d[3] = 510;
    l.d[4] = 312;
    l.d[5] = 0x01234567;            // the monitor's own call
    sy_call(sy::LimitM, l);
}

// Accel: how far the arrows move the view
int16_t accelerate()
{
    MonData &m = mon();
    uint16_t a = m.Tt_Accel;
    if (a < 15)
        m.Tt_Accel = ++a;
    return acceleration[a >> 2];
}

// Tt_Up / Tt_Down / Tt_Left / Tt_Right: 0 (the button kept down)
uint32_t view_move(int direction)
{
    be_u16 *s = view_scroll();
    if (!s)
        return 0;
    int16_t step = accelerate();
    bool moved = false;
    switch (direction) {
    case 0:     // up
        if (int16_t(s[1] - step) >= 0) {
            s[1] = uint16_t(s[1] - step);
            moved = true;
        }
        break;
    case 1:     // down
        if (uint16_t(s[3] - s[1] - step) >= RedTY) {
            s[1] = uint16_t(s[1] + step);
            moved = true;
        }
        break;
    case 2:     // left
        if (int16_t(s[0] - step) >= 0) {
            s[0] = uint16_t(s[0] - step);
            moved = true;
        }
        break;
    default:    // right
        if (uint16_t(s[2] - s[0] - step) >= RedTX) {
            s[0] = uint16_t(s[0] + step);
            moved = true;
        }
    }
    if (moved)
        view_show();
    else
        mon().Tt_Accel = 0;
    return 0;
}

// View_ScUpdate: the screens' sizes, the views at their top
void view_sizes()
{
    MonData &m = mon();
    for (int n = 0; n < 8; n++) {
        be_u16 *s = &m.ViewScrols[4 * n];
        s[0] = s[1] = 0;
        if (Screen *e = screen(uint16_t(n))) {
            s[2] = e->EcTx;
            s[3] = int16_t(e->EcCon0) < 0 ? e->EcTy : uint16_t(e->EcTy >> 1);
        }
    }
}

// BoutStop: the STOP button lit
void light_stop()
{
    MonData &m = mon();
    button(m.Tt_Active, false);
    m.Tt_Active = 1;
    button(1, true);
}

// Ttt_AMod: the buttons of the listing's modes (help, break, evaluate)
void mode_buttons()
{
    uint16_t mode = mon().Ttt_Mode;
    for (uint16_t i = 0; i < 3; i++)
        button(uint16_t(14 + i), i == mode);
}

uint32_t set_mode(uint16_t mode)
{
    mon().Ttt_Mode = mode;
    mode_buttons();
    return 0xFFFFFFFF;
}

bool monitor_banks() { return idata().Cur_Banks == address(&idata().Mon_Banks); }

// Tt_PScCopy: after a Screen Copy of the program, its screen seen
void screen_copy()
{
    MonData &m = mon();
    if (m.Tt_NotHere || monitor_banks())
        return;
    banks_monitor();
    if (Screen *s = ptr<Screen>(wdata().EcCourant)) {
        uint16_t n = s->EcNumber;
        m.Tt_Ecran = 0xFFFF;
        activate(Ecran1);
        view_next(int16_t(n - 1));
        view_show();
        Regs a;
        a.d[1] = n;
        ec_call(ec::Active, a);
    }
    banks_program();
}

// TstPatch: during the tests of a program, the view shown; the STOP
// button pressed stops it
void tests_patch()
{
    MonData &m = mon();
    IData &d = idata();
    banks_monitor();
    m.Tt_Ecran = 0xFFFF;
    activate(Ecran1);
    view_show();
    if (mouse_keys() & 1) {
        Regs z;
        sy_call(sy::GetZone, z);
        uint16_t zone = uint16_t(z.d[1] >> 16);
        if (uint16_t(z.d[1]) == Ecran1 && zone && zone != 13) {
            m.Step_Stop++;
            light_stop();
        }
    }
    if (d.ScOn) {
        Regs a;
        a.d[1] = uint32_t(d.ScOn - 1);
        ec_call(ec::Active, a);
    }
    wdata().Actualise &= ~(1 << 15);     // (bit 7 of its first byte)
    banks_program();
}

} // namespace

namespace amos::monitor {
void view_update() { view_show(); }
void screens_show(bool show) { show_screens(show); }
void mouse_free() { free_mouse(); }
void stop_button() { light_stop(); }
void picture(uint16_t n, int32_t x, int32_t y) { unpack(n, x, y); }
void buttons_draw() { all_buttons(); }
void button_set(uint16_t n, bool on) { button(n, on); }
void mode_set(uint16_t mode) { set_mode(mode); }
void view_erase() { view_clear(); }
void screens_refresh() { recop(); }
void view_reset() { view_sizes(); }
} // namespace amos::monitor

// (the data zone, for the VBL)
uint32_t amos::monitor::view_data_zone;

extern "C" {
void mon_view_show(AllRegs *) { view_show(); }
void mon_view_next(AllRegs *r) { view_next(int16_t(r->d[1])); }
void mon_view_clear(AllRegs *) { view_clear(); }
void mon_view_info(AllRegs *) { view_info(); }
void mon_view_sizes(AllRegs *) { view_sizes(); }
void mon_unpack(AllRegs *r) { unpack(uint16_t(r->d[0]), int32_t(r->d[1]), int32_t(r->d[2])); }
void mon_button(AllRegs *r) { button(uint16_t(r->d[0]), uint16_t(r->d[1])); }
void mon_all_buttons(AllRegs *) { all_buttons(); }
void mon_stop_button(AllRegs *) { light_stop(); }
void mon_show(AllRegs *) { show_screens(true); }
void mon_hide(AllRegs *) { show_screens(false); }
void mon_mouse_free(AllRegs *) { free_mouse(); }
void mon_recop(AllRegs *) { recop(); }
void mon_move_up(AllRegs *r) { r->d[0] = view_move(0); }
void mon_move_down(AllRegs *r) { r->d[0] = view_move(1); }
void mon_move_left(AllRegs *r) { r->d[0] = view_move(2); }
void mon_move_right(AllRegs *r) { r->d[0] = view_move(3); }

// Tt_CEc: the next screen seen
void mon_change_screen(AllRegs *r)
{
    view_next(int16_t(mon().Tt_View));
    view_show();
    r->d[0] = 0xFFFFFFFF;
}

// Tt_See: the program's screens, while the button is down
void mon_see(AllRegs *r)
{
    show_screens(false);
    wait_release();
    show_screens(true);
    r->d[0] = 0xFFFFFFFF;
}

void mon_to_front(AllRegs *)
{
    if (!monitor_banks())
        show_screens(false);
}

void mon_to_back(AllRegs *)
{
    if (!monitor_banks())
        show_screens(true);
}

void mon_screen_copy(AllRegs *) { screen_copy(); }
void mon_mode_buttons(AllRegs *r)
{
    mode_buttons();
    r->d[0] = 0xFFFFFFFF;
}
void mon_set_help(AllRegs *r) { r->d[0] = set_mode(0); }
void mon_set_break(AllRegs *r) { r->d[0] = set_mode(1); }
void mon_set_evaluate(AllRegs *r) { r->d[0] = set_mode(2); }

// Tt_Tst2: the program tested, about to run: the message removed, the
// program's screens to be shown
void mon_tested(AllRegs *)
{
    MonData &m = mon();
    banks_monitor();
    alert_delete();
    m.Tt_Ecran = 0xFFFF;
    m.Tt_MFlag = 0;
    m.Tt_Init = 1;
    banks_program();
}
void mon_tests_patch(AllRegs *) { tests_patch(); }

// View_Inter, in the VBL: the colours of the screen the view shows into
// the view's copper list and screen
void mon_view_inter()
{
    uint8_t *a5 = ptr<uint8_t>(view_data_zone);
    const IData &d = *reinterpret_cast<const IData *>(a5);
    WData &w = *reinterpret_cast<WData *>(a5 - sizeof(WData));
    int16_t view = int16_t(ptr<MonData>(d.Mon_Base)->Tt_View);
    if (view < 0 || !w.EcAdr[view])
        return;
    const Screen *shown = ptr<Screen>(w.EcAdr[view]);
    Screen *own = ptr<Screen>(w.EcAdr[Ecran1]);
    uint32_t mark = peek_l(&w.CopMark[Ecran1 * 128]);
    if (!mark)
        return;
    uint8_t *copper = ptr<uint8_t>(mark) + 2;
    for (int i = 0; i < 11; i++) {
        poke_w(copper + 4 * i, shown->EcPal[i]);
        own->EcPal[i] = shown->EcPal[i];
    }
}
}

#if !defined(__mc68000__)
#include "runtime/bindings/monitor_view.inc"
#endif
