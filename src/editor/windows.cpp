// The editor's windows (+Edit.s "GESTION WINDOWS", the separations, the
// linked and split windows): Edt_List, a list of edit windows; the shown
// ones stacked down the editor's screen, each a status line (Edt_EtatSy
// high), its text (Edt_WindTy lines of 8 pixels) and a bottom line
// (Edt_BasSy); hidden ones (Edt_Hidden: 1 to close, 2 closed) keep other
// programs. Windows split on a program are linked (Edt_LinkPrev/Next);
// a window may follow another's scrolling (Edt_LinkScroll).
//
// The current window and program are a4 and a6 of the assembler: use()
// makes a window current for the C++ (and the assembler it calls), and the
// entries give it back in the registers.

#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint16_t EtatSy = 11, BasSy = 5, TitreSy = 16, YTop = 0, BtSx = 24;
constexpr int16_t SlVDeltaG = 6, SlVDeltaH = 11, SlVSx = 4, SlVDeltaB = -1, SlVDx = 4;
constexpr uint16_t Pics = 1, BtPics = Pics + 4;
constexpr uint8_t EtAY = 1 << 3;
constexpr uint16_t EcEdit = 9;
constexpr uint32_t EntNul = 0x80000000;
constexpr int EdDWQuit = 62, EdDNAll = 3;   // dialogs (+Edit.s EdD_WQuit, EdD_NAll)

EditWindow *list() { return ptr<EditWindow>(idata().Edt_List); }
EditWindow *next_of(const EditWindow *w) { return ptr<EditWindow>(w->Edt_Next); }
Program *program_of(const EditWindow *w) { return ptr<Program>(w->Edt_Prg); }

// a4/a6: the window (and its program) the code works on
void use(EditWindow *w)
{
    ctx().a4 = reinterpret_cast<uint8_t *>(w);
    ctx().a6 = w ? reinterpret_cast<uint8_t *>(program_of(w)) : nullptr;
}

EditWindow *current() { return ptr<EditWindow>(idata().Edt_Current); }

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void run_d0(uint8_t *routine, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    call(routine, r);
}

// A routine of +Edit.s that may change the current window: a4/a6 taken
// back from it
void run_switching(uint8_t *routine, AllRegs &r)
{
    Ctx &c = ctx();
    r.a[3] = address(c.a3);
    r.a[4] = address(c.a4);
    r.a[5] = address(c.a5);
    r.a[6] = address(c.a6);
    r.d[6] = c.d6;
    r.d[7] = c.d7;
    amos_call_all(&r, routine);
    c.a4 = ptr<uint8_t>(r.a[4]);
    c.a6 = ptr<uint8_t>(r.a[6]);
}

void run_switching(uint8_t *routine)
{
    AllRegs r = {};
    run_switching(routine, r);
}

void wi_d1(int function, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    wi_call(function, r);
}

void copy_bytes(void *to, const void *from, uint32_t n)
{
    auto t = static_cast<uint8_t *>(to);
    auto f = static_cast<const uint8_t *>(from);
    while (n--)
        *t++ = *f++;
}

// Edt_WNext as the assembler used it: the shown window after a0, which
// moves to it, or to the last window of the list if there is none
EditWindow *next_moving(EditWindow *&a0)
{
    for (EditWindow *e = next_of(a0); e; e = next_of(e)) {
        a0 = e;
        if (!e->Edt_Hidden)
            return e;
    }
    return nullptr;
}

} // namespace

namespace amos::ed {

// Edt_WNext / Edt_WPrev: the shown window after / before w, or 0
EditWindow *next_shown(const EditWindow *w)
{
    for (EditWindow *n = next_of(w); n; n = next_of(n))
        if (!n->Edt_Hidden)
            return n;
    return nullptr;
}

EditWindow *previous_shown(const EditWindow *w)
{
    EditWindow *found = nullptr;
    for (EditWindow *n = list(); n && n != w; n = next_of(n))
        if (!n->Edt_Hidden)
            found = n;
    return found;
}

// Edt_GetAd: the window of number n
EditWindow *window_number(uint16_t n)
{
    for (EditWindow *w = list(); w; w = next_of(w))
        if (w->Edt_Window == n)
            return w;
    return nullptr;
}

} // namespace amos::ed

namespace {

// Edt_WAlone: the only window shown
bool alone(const EditWindow *w) { return w->Edt_First && w->Edt_Last; }

// Edt_WCount: the windows shown up to w (all if 0)
uint16_t count_shown(const EditWindow *up_to)
{
    uint16_t n = 0;
    for (EditWindow *w = list(); w; w = next_of(w)) {
        if (!w->Edt_Hidden)
            n++;
        if (w == up_to)
            break;
    }
    return n;
}

// Edt_WSchrinkAll: the shown windows at most size lines; the lines gained
uint16_t shrink_all(uint16_t size)
{
    uint16_t gained = 0;
    for (EditWindow *w = list(); w; w = next_of(w)) {
        if (w->Edt_Hidden || size >= w->Edt_WindTy)
            continue;
        gained += w->Edt_WindTy - size;
        w->Edt_WindTy = size;
    }
    return gained;
}

// Edt_WMaxSize: the lines the window w could have, the others at most
// limit lines (negative: as they are); its y and height in pixels too
struct Size {
    int16_t lines;
    uint16_t y, height;
};

Size max_size(const EditWindow *w, int16_t limit)
{
    uint16_t y = 0, used = TitreSy;
    for (EditWindow *e = list(); e; e = next_of(e)) {
        if (e == w) {
            y = used;
            used += EtatSy + BasSy;
            continue;
        }
        if (e->Edt_Hidden)
            continue;
        used += EtatSy + BasSy;
        uint16_t lines = e->Edt_WindTy;
        if (limit >= 0 && lines > uint16_t(limit))
            lines = uint16_t(limit);
        used += lines << 3;
    }
    int16_t free = int16_t(idata().Ed_Sy - used);
    return {int16_t(free >> 3), y, uint16_t(y + free + EtatSy + BasSy)};
}

// Edt_WFirstLast
void first_last()
{
    EditWindow *last = nullptr;
    for (EditWindow *w = list(); w; w = next_of(w)) {
        w->Edt_First = 0;
        w->Edt_Last = 0;
        if (w->Edt_Hidden)
            continue;
        if (!last)
            w->Edt_First = 1;
        last = w;
    }
    if (last)
        last->Edt_Last = 1;
}

// Edt_OrderWindows: the shown windows numbered from 1
void order()
{
    uint16_t n = 1;
    for (EditWindow *w = list(); w; w = next_of(w))
        if (!w->Edt_Hidden)
            w->Edt_Order = n++;
}

// Edt_DelZones / Edt_DelAllZones
void delete_zones(EditWindow *w)
{
    if (uint16_t z = w->Edt_Zones) {
        for (int i = 0; i < 8; i++) {
            Regs r;
            r.d[1] = z + i;
            sy_call(sy::RazZone, r);
        }
        w->Edt_Zones = 0;
    }
}

// Edt_EffWindows: the text windows of the shown windows closed (and of the
// windows to close)
void erase_windows()
{
    for (EditWindow *w = list(); w; w = next_of(w)) {
        if (!w->Edt_Window || w->Edt_Hidden == 2)
            continue;
        if (w->Edt_Hidden == 1)
            w->Edt_Hidden = 2;
        wi_d1(wi::QWindow, w->Edt_Window);
        wi_d1(wi::WinDel, w->Edt_Window);
        wi_d1(wi::QWindow, w->Edt_WindEtat);
        wi_d1(wi::WinDel, w->Edt_WindEtat);
        delete_zones(w);
        w->Edt_Window = 0;
    }
}

// Edt_DelLinkScroll: no window follows w any more
void unlink_scroll(const EditWindow *w)
{
    for (EditWindow *e = list(); e; e = next_of(e))
        if (e->Edt_LinkScroll == address(w))
            e->Edt_LinkScroll = 0;
}

void set_zone(uint16_t n, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = x1;
    r.d[3] = y1;
    r.d[4] = x2;
    r.d[5] = y2;
    sy_call(sy::SetZone, r);
}

void enlarge(uint16_t from, uint16_t to)
{
    Regs r;
    r.d[0] = from;
    r.d[1] = to;
    call(ed_enlarge, r);
}

// WindOp: a text window
void window_open(uint16_t n, uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t border)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = width;
    r.d[5] = height;
    r.d[6] = border;
    r.d[7] = 0;
    r.a[1] = 0;
    wi_call(wi::WindOp, r);
}

const uint8_t *system_text(uint16_t n)
{
    Regs r;
    r.d[0] = n;
    call(ed_get_systeme, r);
    return ptr<uint8_t>(r.a[0]);
}

} // namespace

namespace amos::ed {

// Ed_DrawWindows: all the shown windows drawn again, from the top
void draw_windows()
{
    IData &d = idata();
    EditWindow *const was = &window();
    cursor_off();
    run(ed_all_aver_fin);
    erase_windows();
    order();
    first_last();
    uint32_t buffer = d.Ed_BufE;
    uint16_t top = 16 + YTop;
    for (EditWindow *w = list(); w; w = next_of(w)) {
        use(w);
        Program &p = *program_of(w);
        if (w->Edt_Hidden)
            continue;
        // the cursor within the program and the window
        if (int16_t(w->Edt_YCu + w->Edt_YPos) > int16_t(p.Prg_NLigne)) {
            w->Edt_YCu = 0;
            w->Edt_YPos = 0;
        }
        int16_t last = int16_t(w->Edt_WindTy - 1);
        if (last >= 0 && w->Edt_YCu > uint16_t(last)) {
            w->Edt_YPos += w->Edt_YCu - last;
            w->Edt_YCu = uint16_t(last);
        }
        // its zones and place
        uint16_t zones = uint16_t(w->Edt_Order << 3);
        w->Edt_Zones = zones;
        w->Edt_Window = zones;
        w->Edt_WindEtat = zones + 1;
        uint16_t y = top;
        w->Edt_Y = y;
        w->Edt_WindX = 0;
        w->Edt_WindY = y + EtatSy;
        w->Edt_WindEX = 32;
        w->Edt_WindESx = d.Ed_Sx - 32 - 64;
        w->Edt_WindEY = y + 1;
        w->Edt_WindSx = d.Ed_Sx - 16;
        w->Edt_WindTx = w->Edt_WindSx >> 3;
        w->Edt_WindSy = uint16_t(w->Edt_WindTy << 3);
        w->Edt_Sy = w->Edt_WindSy + EtatSy + BasSy;
        uint16_t lines = w->Edt_WindTy;
        w->Edt_BasY = w->Edt_WindY + w->Edt_WindSy;
        w->Edt_ZEtat = zones + 1;
        w->Edt_ZBas = zones + 2;
        // its slider
        auto s = reinterpret_cast<Slider *>(w->Edt_SlV);
        s->Sl_X = w->Edt_WindX + w->Edt_WindSx + SlVDeltaG;
        s->Sl_Y = y + SlVDeltaH;
        s->Sl_Sx = SlVSx;
        s->Sl_Sy = w->Edt_WindSy + SlVDeltaB;
        s->Sl_Scroll = w->Edt_WindTy - 2;
        s->Sl_ZDx = SlVDx;
        s->Sl_Zone = zones + 3;
        s->Sl_Routines = address(ed_slider_routines);
        reinterpret_cast<uint8_t *>(&s->Sl_Flags)[0] |= 1 << 0;      // bset Sl_FlagVertical
        // the colours inactive, then active
        const uint8_t *colours = system_text(19);
        auto *to = reinterpret_cast<be_u16 *>(w->Edt_SlV + offsetof(Slider, Sl_Inactive));
        for (int i = 0; i < 16; i++)
            to[i] = colours[i];
        // its buttons
        auto b = reinterpret_cast<Button *>(w->Edt_Bt1);
        b[2].Bt_X = d.Ed_Sx - BtSx;
        b[1].Bt_X = d.Ed_Sx - 2 * BtSx;
        b[0].Bt_X = 0;
        b[0].Bt_Y = b[1].Bt_Y = b[2].Bt_Y = w->Edt_Y;
        uint16_t picture = BtPics, zone = zones + 4;
        for (int i = 0; i < 3; i++, picture += 2) {
            b[i].Bt_Number = w->Edt_Window;
            b[i].Bt_Zone = ++zone;
            b[i].Bt_Image = picture;
            b[i].Bt_Sx = 24;
            b[i].Bt_Sy = EtatSy;
            b[i].Bt_Routines = address(ed_button_routines);
            b[i].Bt_RDraw = 6;
            b[i].Bt_RChange = uint8_t(i + 1);
        }
        // its background
        unpack(Pics + 1, 0, y);
        enlarge(y, y + 11);
        y += 11;
        for (uint16_t n = lines; n; n--, y += 8)
            unpack(Pics + 2, int16_t(d.Ed_Sx - 16), y);
        unpack(Pics + 3, 0, y);
        enlarge(y, y + 5);
        y += 5;
        top = y;
        // the status line
        window_open(w->Edt_WindEtat, w->Edt_WindEX, w->Edt_WindEY, w->Edt_WindESx >> 3, 1, 0);
        run(ed_status_print);
        set_zone(w->Edt_ZEtat, w->Edt_WindEX, w->Edt_WindEY, w->Edt_WindEX + w->Edt_WindESx, w->Edt_WindEY + EtatSy);
        set_zone(w->Edt_ZBas, 0, w->Edt_BasY, d.Ed_Sx, w->Edt_BasY + BasSy);
        // the text
        if (w->Edt_WindTy) {
            window_open(w->Edt_Window, w->Edt_WindX, w->Edt_WindY, w->Edt_WindTx, w->Edt_WindTy, 0);
            Regs pr;
            pr.a[1] = address(system_text(20));
            wi_call(wi::Print, pr);
            Regs sh;
            sh.a[1] = address(ed_cursor_normal);
            wi_call(wi::SCurWi, sh);
            set_zone(w->Edt_Zones, w->Edt_WindX, w->Edt_WindY, w->Edt_WindX + w->Edt_WindSx,
                     w->Edt_WindY + w->Edt_WindSy);
            run(ed_sl_update_y);
            Regs sl;
            sl.a[0] = address(s);
            lib_call(L_Sl_Init, sl);
            w->Edt_ASlY = 0;
            w->Edt_BufE = buffer;
            buffer += uint32_t(w->Edt_WindTy) * 256;
            new_buffer();
        }
        Regs bt;
        bt.a[0] = address(b);
        bt.d[0] = 0;
        lib_call(L_Bt_InitList, bt);
    }
    // the current window shown; a4 back as it was, a6 the current program
    use(current());
    wi_d1(wi::QWindow, window().Edt_Window);
    place_cursor();
    ctx().a4 = reinterpret_cast<uint8_t *>(was);
}

} // namespace amos::ed

namespace {

// Edt_WAutre2/3: another shown window of at least min lines made current,
// after w or else before it. The assembler's condition codes (NE) came
// from comparing the window's lines with min: false if none was found or
// it has just min lines.
bool another(EditWindow *w, uint16_t min)
{
    EditWindow *a0 = w;
    for (;;) {
        EditWindow *n = next_moving(a0);
        if (!n)
            break;
        if (int16_t(min) > int16_t(n->Edt_WindTy))
            continue;
        goto found;
    }
    for (;;) {
        EditWindow *p = previous_shown(a0);
        if (!p)
            return false;
        a0 = p;
        if (int16_t(min) > int16_t(p->Edt_WindTy))
            continue;
        goto found;
    }
found:
    idata().Edt_Current = address(a0);
    use(a0);
    return a0->Edt_WindTy != min;
}

// Edt_WVideNext: if the current window has no lines, another
void empty_next()
{
    EditWindow *c = current();
    if (!c->Edt_WindTy)
        another(c, 1);
}

// Edt_WPlaceHaut / Edt_WPlaceBas: lines of room taken from the windows
// above / below w, leaving them at least min; what could not be found
int16_t make_room(EditWindow *w, int16_t lines, int16_t min, bool above)
{
    EditWindow *a = w;
    for (;;) {
        a = above ? previous_shown(a) : next_shown(a);
        if (!a)
            return lines;
        if (min >= int16_t(a->Edt_WindTy))
            continue;
        int16_t rest = int16_t(lines - a->Edt_WindTy);
        if (uint16_t(lines) < a->Edt_WindTy) {
            a->Edt_WindTy = uint16_t(-rest);
            return 0;
        }
        lines = int16_t(rest + min);
        a->Edt_WindTy = uint16_t(min);
    }
}

// Edt_WChange: the window w from y top to bottom; the windows around
// changed to match
void change(EditWindow *w, uint16_t top, uint16_t bottom)
{
    w->Edt_WindTy = uint16_t((bottom - top - EtatSy) >> 3);
    uint16_t old = w->Edt_Y;
    w->Edt_Y = top;
    if (top != old) {
        if (top < old) {
            int16_t rest = make_room(w, int16_t((old - top) >> 3), 1, true);
            if (rest)
                make_room(w, rest, 0, true);
        } else {
            previous_shown(w)->Edt_WindTy += (top - old) >> 3;
        }
    }
    old = w->Edt_BasY;
    w->Edt_BasY = bottom;
    if (bottom == old)
        return;
    if (bottom < old) {
        next_shown(w)->Edt_WindTy += (old - bottom) >> 3;
    } else {
        int16_t rest = make_room(w, int16_t((bottom - old) >> 3), 1, false);
        if (rest)
            make_room(w, rest, 0, false);
    }
}

// Edt_WChangeHaut / Edt_WChangeBas: the top / bottom separation of w to y
bool change_top(EditWindow *w, uint16_t y)
{
    y &= 0xFFF8;
    if (y == w->Edt_Y)
        return false;
    uint16_t bottom;
    if (!w->Edt_Last) {
        int16_t move = int16_t(y - w->Edt_Y);
        if (!move)
            return false;
        bottom = w->Edt_BasY + move;
    } else {
        bottom = w->Edt_BasY;
        if (y + EtatSy > bottom)
            bottom = y + EtatSy;
    }
    change(w, y, bottom);
    return true;
}

bool change_bottom(EditWindow *w, uint16_t y)
{
    y = uint16_t(((y + BasSy) & 0xFFF8) - BasSy);
    if (y == w->Edt_BasY)
        return false;
    uint16_t top = w->Edt_Y;
    if (uint16_t(y - EtatSy) < top)
        top = y - EtatSy;
    change(w, top, y);
    return true;
}

// Ed_RShLimits / Ed_RSbLimits: what a separation may do: the minimum and
// maximum y, the height moving with it, where it is
struct Limits {
    uint16_t min, max, moving, at;
};

Limits top_limits(const EditWindow *w)
{
    Size s = max_size(w, 0);
    if (w->Edt_Last)
        return {s.y, uint16_t(s.height - EtatSy - BasSy), EtatSy, w->Edt_Y};
    uint16_t moving = uint16_t((w->Edt_WindTy << 3) + EtatSy + BasSy);
    return {s.y, uint16_t(s.height - moving), moving, w->Edt_Y};
}

Limits bottom_limits(const EditWindow *w)
{
    Size s = max_size(w, 0);
    return {uint16_t(s.y + EtatSy), s.height, BasSy, w->Edt_BasY};
}

// XYMouEced: the mouse in the editor's screen
void mouse_xy(int16_t &x, int16_t &y)
{
    Regs r;
    sy_call(sy::XyMou, r);
    r.d[3] = 0;
    sy_call(sy::XyScr, r);
    x = int16_t(r.d[1]);
    y = int16_t(r.d[2]);
}

void block_get(uint16_t n, int16_t y, uint16_t width, uint16_t height)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = 0;
    r.d[3] = uint32_t(int32_t(y));
    r.d[4] = width;
    r.d[5] = height;
    r.d[6] = 0;
    r.a[1] = idata().Ed_BufT;
    ec_call(ec::BlGet, r);
}

void block_put(uint16_t n, int16_t y)
{
    Regs r;
    r.d[1] = n;
    r.d[2] = 0;
    r.d[3] = uint32_t(int32_t(y));
    r.d[4] = EntNul;
    r.d[5] = EntNul;
    r.a[1] = idata().Ed_BufT;
    ec_call(ec::BlPut, r);
}

void block_delete(uint16_t n)
{
    Regs r;
    r.d[1] = n;
    ec_call(ec::BlDel, r);
}

bool left_button()
{
    Regs r;
    sy_call(sy::MouseKey, r);
    return r.d[1] & 1;
}

// Ed_MSep: a separation of height moving at y dragged with the mouse
// between min and max: where it ends
uint16_t drag(Limits l)
{
    int16_t mx, my;
    mouse_xy(mx, my);
    int16_t offset = int16_t(my - l.at);
    int16_t now = int16_t(l.at), drawn = now;
    // block 128: the separation, 129: what is under it
    block_get(128, now, idata().Ed_Sx, l.moving);
    do {
        block_get(129, now, idata().Ed_Sx, l.moving);
        drawn = now;
        block_put(128, now);
        for (;;) {
            if (!left_button())
                break;
            mouse_xy(mx, my);
            int16_t y = int16_t(my - offset);
            if (y < int16_t(l.min))
                y = int16_t(l.min);
            if (y > int16_t(l.max))
                y = int16_t(l.max);
            if (y != now) {
                now = y;
                break;
            }
        }
        // (the assembler put back the block from Buffer here, not from
        // Ed_BufT: the same block number 129 of the screen's list)
        block_put(129, drawn);
    } while (left_button());
    block_delete(128);
    block_delete(129);
    return uint16_t(now);
}

// Ed_MSepHaut / Ed_MSepBas: a separation of w dragged with the mouse
bool drag_top(EditWindow *w)
{
    use(w);
    if (w->Edt_First)
        return false;
    if (!change_top(w, drag(top_limits(w))))
        return false;
    empty_next();
    draw_windows();
    return true;
}

bool drag_bottom(EditWindow *w)
{
    use(w);
    if (w->Edt_Last)
        return false;
    if (!change_bottom(w, drag(bottom_limits(w))))
        return false;
    empty_next();
    draw_windows();
    return true;
}

// Ed_EtatMove / Ed_BasMove: a separation of the current window moved by
// delta pixels
void move_top(int16_t delta)
{
    tokenise_current_line();
    EditWindow &w = window();
    if (w.Edt_First)
        return;
    Limits l = top_limits(&w);
    uint16_t y = uint16_t(l.at + delta);
    if (y <= l.min || int16_t(y) >= int16_t(l.max))
        return;
    if (!change_top(&w, y))
        return;
    empty_next();
    draw_windows();
}

void move_bottom(int16_t delta)
{
    tokenise_current_line();
    EditWindow &w = window();
    if (w.Edt_Last)
        return;
    Limits l = bottom_limits(&w);
    uint16_t y = uint16_t(l.at + delta);
    if (y <= l.min || int16_t(y) >= int16_t(l.max))
        return;
    if (!change_bottom(&w, y))
        return;
    empty_next();
    draw_windows();
}

} // namespace

namespace amos::ed {

// Edt_Active: the window w made current, if it has lines
bool activate(EditWindow *w)
{
    tokenise_current_line();
    if (!w->Edt_WindTy)
        return false;
    idata().Edt_Current = address(w);
    window().Edt_ASlY = 0;
    show_slider();
    run(ed_status_print);
    use(w);
    new_buffer_all();
    return true;
}

// Edt_OpWindow: a new window after the current one (or the first), shown
// or hidden, with a program of size bytes (negative: none); 0, 1 too many
// windows, -1 out of memory
int32_t open_window(bool shown, int32_t size)
{
    IData &d = idata();
    if (shown && count_shown(nullptr) >= d.Ed_WMax)
        return 1;
    EditWindow *w = ptr<EditWindow>(memory(EditWindow::Edt_Long, true));
    if (!w)
        return -1;
    EditWindow *c = ptr<EditWindow>(address(ctx().a4));
    if (c) {
        w->Edt_Next = c->Edt_Next;
        c->Edt_Next = address(w);
    } else {
        w->Edt_Next = d.Edt_List;
        d.Edt_List = address(w);
    }
    // (without a program of its own, a6 stays the program of the window
    // it splits: Edt_New works on it)
    ctx().a4 = reinterpret_cast<uint8_t *>(w);
    if (size >= 0) {
        Regs r;
        r.d[0] = uint32_t(size);
        lib_call(L_Prg_NewStructure, r);
        if (!r.d[0]) {
            delete_window();
            return -1;
        }
        w->Edt_Prg = r.d[0];
        ctx().a6 = ptr<uint8_t>(r.d[0]);
        program().Prg_Edited++;
    }
    w->Edt_Hidden = 2;          // no zones yet
    if (shown) {
        w->Edt_Hidden = 0;
        d.Edt_Current = address(w);
        shrink_all(1);
        w->Edt_WindTy = uint16_t(max_size(w, -1).lines);
    }
    run(ed_new_window);
    return 0;
}

// Edt_DelWindow: the current window closed (and its program if no other
// window shows it); the current one after it
void delete_window()
{
    IData &d = idata();
    EditWindow *w = &window();
    if (Program *p = program_of(w)) {
        ctx().a6 = reinterpret_cast<uint8_t *>(p);
        if (!--p->Prg_Edited) {
            run(ed_menu_program);
            Regs r;
            lib_call(L_Prg_DelStructure, r);
            undo_free();
        }
    }
    if (current() == w) {
        EditWindow *n = next_shown(w);
        if (!n)
            n = previous_shown(w);
        uint32_t link = w->Edt_LinkPrev ? w->Edt_LinkPrev : w->Edt_LinkNext;
        d.Edt_Current = link ? link : address(n);
    }
    // out of the split windows
    if (EditWindow *n = ptr<EditWindow>(w->Edt_LinkNext))
        n->Edt_LinkPrev = w->Edt_LinkPrev;
    if (EditWindow *p = ptr<EditWindow>(w->Edt_LinkPrev))
        p->Edt_LinkNext = w->Edt_LinkNext;
    unlink_scroll(w);
    delete_zones(w);
    if (list() == w) {
        d.Edt_List = w->Edt_Next;
    } else {
        EditWindow *a = list();
        while (next_of(a) != w)
            a = next_of(a);
        a->Edt_Next = w->Edt_Next;
    }
    free_memory(address(w), EditWindow::Edt_Long);
    use(current());
}

} // namespace amos::ed

namespace {

// Ed_OpenWindow
void open_shown_window()
{
    tokenise_current_line();
    int32_t r = open_window(true, int32_t(idata().PI_DefSize));
    if (r < 0)
        run(ed_out_of_memory);
    if (r > 0)
        run(ed_too_many_windows);
    draw_windows();
}

// Ed_CloseWindow(Quit): the current window closed; the last one quits
// AMOSPro (if asked)
void close_window(bool quit)
{
    IData &d = idata();
    if (d.Ed_Zappeuse)
        quit = false;
    tokenise_current_line();
    EditWindow *w = &window();
    if (!w->Edt_LinkPrev && !w->Edt_LinkNext)
        run_switching(ed_new);
    w = &window();
    if (w->Edt_First && w->Edt_Last) {
        if (!quit)
            return;
        if (!(d.Ed_QuitFlags & 1) || int16_t(dialogue(EdDWQuit)) == 1)
            run_switching(ed_do_quit);
        return;
    }
    erase_windows();
    delete_window();
    window().Edt_WindTy = uint16_t(max_size(&window(), -1).lines);
    draw_windows();
}

// Ed_WindowHide: the current window hidden (its split windows closed)
void hide_window()
{
    tokenise_current_line();
    for (;;) {
        EditWindow *w = &window();
        uint32_t other = w->Edt_LinkPrev ? w->Edt_LinkPrev : w->Edt_LinkNext;
        if (!other)
            break;
        use(ptr<EditWindow>(other));
        close_window(false);
        use(w);
        idata().Edt_Current = address(w);
    }
    EditWindow *w = &window();
    if (alone(w))
        run(ed_no_hide);
    unlink_scroll(w);
    w->Edt_Hidden = 1;
    if (!another(w, 1))
        another(&window(), 0);
    window().Edt_WindTy = uint16_t(max_size(&window(), -1).lines);
    draw_windows();
    run_switching(ed_branch_amos);
}

} // namespace

namespace amos::ed {

// Ed_GetHidden: hidden window number n
EditWindow *hidden(uint16_t n)
{
    for (EditWindow *w = list(); w; w = next_of(w))
        if (w->Edt_Hidden && int16_t(--n) < 0)
            return w;
    return nullptr;
}

} // namespace amos::ed

namespace {

// Ed_FlipSizeWindow / Ed_BiggerWindow / Ed_ExpandWindow /
// Ed_SchrinkWindow
void shrink_window(uint16_t lines)
{
    tokenise_current_line();
    EditWindow &w = window();
    if (alone(&w))
        return;
    uint16_t had = w.Edt_WindTy;
    if (lines >= had)
        return;
    w.Edt_WindTy = lines;
    uint16_t freed = had - lines;
    if (EditWindow *n = next_shown(&w))
        n->Edt_WindTy += freed;
    else
        previous_shown(&w)->Edt_WindTy += freed;
    empty_next();
    draw_windows();
}

void flip_size()
{
    tokenise_current_line();
    EditWindow &w = window();
    if (uint16_t lines = w.Edt_WindTy) {
        w.Edt_WindOldTy = lines;
        shrink_window(0);
        return;
    }
    int16_t lines = int16_t(w.Edt_WindOldTy);
    if (!lines)
        lines = max_size(&w, 1).lines;
    w.Edt_WindTy = uint16_t(lines);
    if (int16_t rest = make_room(&w, lines, 1, false)) {
        if (int16_t rest2 = make_room(&w, rest, 1, true))
            w.Edt_WindTy -= rest2;
    }
    draw_windows();
}

void grow(uint16_t others)
{
    tokenise_current_line();
    EditWindow &w = window();
    if (alone(&w))
        return;
    w.Edt_WindTy += shrink_all(others);
    draw_windows();
}

// Ed_NextWindow / Ed_PrevWindow
void next_window()
{
    tokenise_current_line();
    EditWindow *const c = &window();
    EditWindow *a = c;
    for (;;) {
        a = next_shown(a);
        if (!a)
            a = list();
        if (a == c || activate(a))
            return;
    }
}

void previous_window()
{
    tokenise_current_line();
    EditWindow *const c = &window();
    EditWindow *a = c;
    for (;;) {
        EditWindow *p = previous_shown(a);
        if (!p) {
            // around, to the last of the list
            if (c->Edt_Last)
                return;
            p = c;
            while (next_moving(p))
                ;
        }
        if (p == c)
            return;
        a = p;
        if (activate(a))
            return;
    }
}

// Ed_SplitWindow: a second view of the program, linked to the current
// window
void split()
{
    tokenise_current_line();
    EditWindow *w = &window();
    int32_t r = open_window(true, -1);
    if (r < 0)
        run(ed_out_of_memory);
    if (r > 0)
        run(ed_too_many_windows);
    EditWindow *n = &window();
    n->Edt_LinkPrev = address(w);
    n->Edt_LinkNext = w->Edt_LinkNext;
    w->Edt_LinkNext = address(n);
    if (EditWindow *after = ptr<EditWindow>(n->Edt_LinkNext))
        after->Edt_LinkPrev = address(n);
    n->Edt_Prg = w->Edt_Prg;
    use(n);
    program().Prg_Edited++;
    copy_bytes(reinterpret_cast<uint8_t *>(n) + EditWindow::Edt_SSplit, reinterpret_cast<uint8_t *>(w) + EditWindow::Edt_SSplit,
               EditWindow::Edt_ESplit - EditWindow::Edt_SSplit);
    draw_windows();
}

} // namespace

namespace amos::ed {

// Ed_Splitted: the other windows split on the program of window w brought
// up to date: bit 1 of flags all their text, bit 0 the line changed
void update_split(EditWindow *w, uint16_t flags)
{
    if (!w->Edt_LinkPrev && !w->Edt_LinkNext)
        return;
    EditWindow *a = w;
    while (a->Edt_LinkPrev)
        a = ptr<EditWindow>(a->Edt_LinkPrev);
    for (; a; a = ptr<EditWindow>(a->Edt_LinkNext)) {
        if (a == w || !a->Edt_WindTy)
            continue;
        use(a);
        Program &p = program();
        a->Edt_EtatAff = w->Edt_EtatAff;
        if (flags & 2) {
            if (uint16_t(a->Edt_YPos + a->Edt_YCu) >= p.Prg_NLigne) {
                int16_t top = int16_t(p.Prg_NLigne - a->Edt_YCu);
                if (top < 0) {
                    a->Edt_YCu = p.Prg_NLigne;
                    top = 0;
                }
                a->Edt_YPos = uint16_t(top);
            }
            new_buffer();
        }
        if (flags & 1) {
            int16_t y = int16_t(w->Edt_YPos + w->Edt_YCu - a->Edt_YPos);
            if (y >= 0 && uint16_t(y) < a->Edt_WindTy) {
                const uint8_t *from = ptr<uint8_t>(w->Edt_BufE) + int16_t(uint16_t(w->Edt_YCu << 8));
                uint8_t *to = ptr<uint8_t>(a->Edt_BufE) + int16_t(uint16_t(y << 8));
                copy_bytes(to, from, 256);
                wi_d1(wi::QWindow, a->Edt_Window);
                show_line(uint16_t(y), true);
            }
        }
    }
    use(w);
    wi_d1(wi::QWindow, w->Edt_Window);
}

// Ed_LinkeScroll: the windows following the current window's scrolling
// moved with it
void follow_scroll()
{
    EditWindow *w = &window();
    if (!w->Edt_LinkScroll)
        return;
    struct Rec {
        static void go(EditWindow *w)
        {
            EditWindow *n = ptr<EditWindow>(w->Edt_LinkScroll);
            if (!n)
                return;
            uint16_t line = w->Edt_YCu + w->Edt_YPos;
            if (line == w->Edt_LinkYOld)
                return;
            int16_t delta = int16_t(line - w->Edt_LinkYOld);
            w->Edt_LinkYOld = line;
            w->Edt_LinkFlag = 1;
            if (!n->Edt_LinkFlag) {
                use(n);
                Program &p = program();
                int16_t top = int16_t(n->Edt_YPos + delta);
                if (top < 0)
                    top = 0;
                if (uint16_t(top + n->Edt_YCu) >= p.Prg_NLigne) {
                    top = int16_t(p.Prg_NLigne - n->Edt_YCu);
                    if (top < 0) {
                        n->Edt_YCu = p.Prg_NLigne;
                        top = 0;
                    }
                }
                n->Edt_YPos = uint16_t(top);
                if (n->Edt_WindTy)
                    new_buffer();
                n->Edt_EtatAff |= EtAY;
                go(n);
            }
            use(w);
            w->Edt_LinkFlag = 0;
        }
    };
    Rec::go(w);
    wi_d1(wi::QWindow, w->Edt_Window);
}

} // namespace amos::ed

namespace {

// Ed_LinkCursor: the window clicked on follows this one's scrolling
void link_cursor()
{
    tokenise_current_line();
    run_d0(ed_warn, 1);
    EditWindow &w = window();
    w.Edt_LinkScroll = 0;
    for (;;) {
        boot::wait_vbl_multi();
        Regs k;
        sy_call(sy::MouseKey, k);
        if (k.d[1] & 1)
            break;
    }
    Regs z;
    sy_call(sy::GetZone, z);
    if (uint16_t(z.d[1]) != EcEdit || uint16_t(z.d[1] >> 16) == w.Edt_Window)
        run(ed_not_done);
    EditWindow *other = window_number(uint16_t(z.d[1] >> 16));
    if (!other)
        run(ed_not_done);
    unlink_scroll(other);
    w.Edt_LinkScroll = address(other);
    w.Edt_LinkYOld = w.Edt_YPos + w.Edt_YCu;
    run(ed_aver_fin);
    Regs r;
    lib_call(L_Dia_NoMKey, r);
}

void keep_current(AllRegs *r)
{
    r->a[4] = address(ctx().a4);
    r->a[6] = address(ctx().a6);
}

} // namespace

extern "C" {
void ed_draw_windows(AllRegs *r)
{
    draw_windows();
    r->a[6] = address(ctx().a6);
}
void ed_erase_windows(AllRegs *) { erase_windows(); }
void ed_order_windows(AllRegs *) { order(); }
void ed_first_last(AllRegs *) { first_last(); }

void ed_delete_all_zones(AllRegs *)
{
    for (EditWindow *w = list(); w; w = next_of(w))
        w->Edt_Zones = 0;
}

void ed_delete_zones(AllRegs *) { delete_zones(&window()); }
void ed_unlink_scroll(AllRegs *r) { unlink_scroll(ptr<EditWindow>(r->a[0])); }

// Edt_WVideNext / Edt_WAutre / Edt_WAutre2 (a0) / Edt_WAutre3 (a0, d1)
void ed_empty_next(AllRegs *r)
{
    empty_next();
    keep_current(r);
}

void ed_another(AllRegs *r)
{
    another(&window(), 1);
    keep_current(r);
}

void ed_another_2(AllRegs *r)
{
    another(ptr<EditWindow>(r->a[0]), 1);
    keep_current(r);
}

void ed_another_3(AllRegs *r)
{
    another(ptr<EditWindow>(r->a[0]), uint16_t(r->d[1]));
    keep_current(r);
}

// Edt_GetAd: d0 the window number; a0 the window, NE if found
uint32_t ed_window_number(AllRegs *r)
{
    EditWindow *w = window_number(uint16_t(r->d[0]));
    r->a[0] = address(w);
    if (!w)
        r->d[0] = 0;
    else
        r->d[1] = 0xFFFFFFFF;
    return w != nullptr;
}

// Edt_WNext / Edt_WPrev: a0 the window; d0 the next / previous shown, the
// condition codes on it
uint32_t ed_next_shown(AllRegs *r)
{
    EditWindow *n = next_shown(ptr<EditWindow>(r->a[0]));
    r->d[0] = address(n);
    if (n)
        r->a[0] = address(n);
    else
        while (EditWindow *e = next_of(ptr<EditWindow>(r->a[0])))
            r->a[0] = address(e);
    return r->d[0];
}

uint32_t ed_previous_shown(AllRegs *r)
{
    r->d[0] = address(previous_shown(ptr<EditWindow>(r->a[0])));
    return r->d[0];
}

// Edt_WAlone: NE when the only window shown
uint32_t ed_alone(AllRegs *) { return alone(&window()); }

// Edt_WCount: a0 the last window counted (0 all); d0 the shown ones
void ed_count_shown(AllRegs *r) { r->d[0] = (r->d[0] & 0xFFFF0000) | count_shown(ptr<EditWindow>(r->a[0])); }

// Edt_WSchrinkAll: d0 the size; d0 the lines gained
void ed_shrink_all(AllRegs *r) { r->d[0] = (r->d[0] & 0xFFFF0000) | shrink_all(uint16_t(r->d[0])); }

// Edt_WMaxSize: d0 the limit; d0 the lines, d1 the y, d2 the height
void ed_max_size(AllRegs *r)
{
    Size s = max_size(&window(), int16_t(r->d[0]));
    r->d[0] = (r->d[0] & 0xFFFF0000) | uint16_t(s.lines);
    r->d[1] = s.y;
    r->d[2] = s.height;
}

// Edt_WChangeHaut / Bas (d0), Edt_WPlaceHaut / Bas (d0 lines, d1 min)
uint32_t ed_change_top(AllRegs *r) { return change_top(&window(), uint16_t(r->d[0])) ? 0xFFFFFFFF : 0; }
uint32_t ed_change_bottom(AllRegs *r) { return change_bottom(&window(), uint16_t(r->d[0])) ? 0xFFFFFFFF : 0; }

uint32_t ed_room_above(AllRegs *r)
{
    int16_t rest = make_room(&window(), int16_t(r->d[0]), int16_t(r->d[1]), true);
    r->d[0] = (r->d[0] & 0xFFFF0000) | uint16_t(rest);
    r->d[1] = (r->d[1] & 0xFFFF0000) | uint16_t(rest);
    return uint16_t(rest);
}

uint32_t ed_room_below(AllRegs *r)
{
    int16_t rest = make_room(&window(), int16_t(r->d[0]), int16_t(r->d[1]), false);
    r->d[0] = (r->d[0] & 0xFFFF0000) | uint16_t(rest);
    r->d[1] = (r->d[1] & 0xFFFF0000) | uint16_t(rest);
    return uint16_t(rest);
}

// Ed_MSepHaut / Ed_MSepBas: a0 the window; d0 -1 if moved
void ed_drag_top(AllRegs *r) { r->d[0] = drag_top(ptr<EditWindow>(r->a[0])) ? 0xFFFFFFFF : 0; }
void ed_drag_bottom(AllRegs *r) { r->d[0] = drag_bottom(ptr<EditWindow>(r->a[0])) ? 0xFFFFFFFF : 0; }

void ed_top_up(AllRegs *) { move_top(-8); }
void ed_top_down(AllRegs *) { move_top(8); }
void ed_bottom_up(AllRegs *) { move_bottom(-8); }
void ed_bottom_down(AllRegs *) { move_bottom(8); }

// Edt_Active: a0 the window; d0 -1 if made current
void ed_activate(AllRegs *r)
{
    if (activate(ptr<EditWindow>(r->a[0])))
        r->d[0] = 0xFFFFFFFF;
    keep_current(r);
}

// Edt_OpWindow: d0 shown, d1 the program's size (negative: none); d0 and
// the condition codes 0, 1 or -1
uint32_t ed_open_window(AllRegs *r)
{
    int32_t result = open_window(uint16_t(r->d[0]), int32_t(r->d[1]));
    r->d[0] = uint32_t(result);
    keep_current(r);
    return uint32_t(result);
}

void ed_delete_window(AllRegs *r)
{
    delete_window();
    keep_current(r);
}

// Edt_DelWindows: all of them
void ed_delete_windows(AllRegs *)
{
    Ctx &c = ctx();
    uint8_t *a4 = c.a4, *a6 = c.a6;
    while (EditWindow *w = list()) {
        use(w);
        delete_window();
    }
    c.a4 = a4;
    c.a6 = a6;
}

void ed_open_shown_window(AllRegs *r)
{
    open_shown_window();
    keep_current(r);
}

void ed_close_window(AllRegs *r)
{
    close_window(false);
    keep_current(r);
}

void ed_close_window_quit(AllRegs *r)
{
    close_window(true);
    keep_current(r);
}

void ed_hide_window(AllRegs *r)
{
    hide_window();
    keep_current(r);
}

// Ed_BtWindowClose / Hide / Size: a2 the button
void ed_button_close(AllRegs *r)
{
    if (EditWindow *w = window_number(peek_w(ptr<uint8_t>(r->a[2])))) {
        use(w);
        close_window(true);
        keep_current(r);
    }
}

void ed_button_hide(AllRegs *r)
{
    tokenise_current_line();
    if (EditWindow *w = window_number(peek_w(ptr<uint8_t>(r->a[2])))) {
        use(w);
        hide_window();
        keep_current(r);
    }
}

void ed_button_size(AllRegs *r)
{
    tokenise_current_line();
    if (EditWindow *w = window_number(peek_w(ptr<uint8_t>(r->a[2])))) {
        use(w);
        flip_size();
        keep_current(r);
    }
}

// Ed_NewHidden / Ed_NewAllHidden / Ed_EditHidden: d1 the hidden program
void ed_new_hidden(AllRegs *r)
{
    tokenise_current_line();
    EditWindow *w = hidden(uint16_t(r->d[1]));
    if (!w)
        run(ed_not_done);
    use(w);
    run(ed_saved);
    delete_window();
    run_switching(ed_branch_amos);
    keep_current(r);
}

void ed_new_all_hidden(AllRegs *r)
{
    tokenise_current_line();
    if (int16_t(dialogue(EdDNAll)) != 1)
        run(ed_loop);

    while (EditWindow *w = hidden(0)) {
        use(w);
        run(ed_saved);
        delete_window();
    }
    run_switching(ed_branch_amos);
    keep_current(r);
}

void ed_edit_hidden(AllRegs *r)
{
    tokenise_current_line();
    EditWindow *w = hidden(uint16_t(r->d[1]));
    if (!w)
        run(ed_not_done);
    if (count_shown(nullptr) >= idata().Ed_WMax)
        run(ed_too_many_windows);
    idata().Edt_Current = address(w);
    use(w);
    w->Edt_Hidden = 0;
    shrink_all(1);
    w->Edt_WindTy = uint16_t(max_size(w, -1).lines);
    run(ed_undo_create_entry);
    draw_windows();
    run_switching(ed_branch_amos);
    keep_current(r);
}

// Ed_GetHidden: d1 the number; d0 and a0 the window
void ed_get_hidden(AllRegs *r)
{
    EditWindow *w = hidden(uint16_t(r->d[1]));
    r->d[0] = address(w);
    if (w)
        r->a[0] = address(w);
}

void ed_flip_size(AllRegs *) { flip_size(); }
void ed_bigger(AllRegs *) { grow(0); }
void ed_expand(AllRegs *) { grow(1); }
void ed_shrink(AllRegs *r) { shrink_window(uint16_t(r->d[0])); }

void ed_next_window(AllRegs *r)
{
    next_window();
    keep_current(r);
}

void ed_previous_window(AllRegs *r)
{
    previous_window();
    keep_current(r);
}

void ed_split(AllRegs *r)
{
    split();
    keep_current(r);
}

// Ed_Splitted: a0 the window, d0 the flags
void ed_update_split(AllRegs *r) { update_split(ptr<EditWindow>(r->a[0]), uint16_t(r->d[0])); }
void ed_follow_scroll(AllRegs *) { follow_scroll(); }
void ed_link_cursor(AllRegs *) { link_cursor(); }
}
