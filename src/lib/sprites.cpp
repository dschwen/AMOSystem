// Sprites, bobs, the mouse and collisions (SPRITE.S in AMOSPro.Lib). The
// AMAL instructions of the section are still assembler.

#include "lib/banks.h"
#include "lib/sprites.h"

using namespace amos;

// banks.cpp
extern "C" uint32_t lib_bnk_ad_bob(RegFrame &r);
extern "C" uint32_t lib_bnk_ad_icon(RegFrame &r);
extern "C" uint32_t lib_bnk_eff_bob_a0(RegFrame &r);
extern "C" uint32_t lib_bnk_res_bob(RegFrame &r);
extern "C" uint32_t lib_bnk_res_ico(RegFrame &r);

namespace {

constexpr uint32_t EntNul = 0x80000000;
constexpr uint16_t BitBobs = 1 << 13, BitSprites = 1 << 14;

[[noreturn]] void screen_not_open() { screen_error(3); }

void need_screen()
{
    if (!idata().ScOn)
        screen_not_open();
}

Regs &sy_regs(int function, Regs &r)
{
    sy_call(function, r);
    return r;
}

void sy0(int function)
{
    Regs r = {};
    sy_call(function, r);
}

void sy_d1(int function, uint32_t d1)
{
    Regs r = {};
    r.d[1] = d1;
    sy_call(function, r);
}

// SyCall that reports an error in d0: "Rbne L_FonCall"
Regs &sy_fon(int function, Regs &r)
{
    sy_call(function, r);
    if (r.d[0])
        fon_call();
    return r;
}

// Rbmi L_OOfMem, Rbne L_FonCall
void sy_set_bob(Regs &r)
{
    sy_call(sy::SetBob, r);
    if (static_cast<int32_t>(r.d[0]) < 0)
        error(24);
    if (r.d[0])
        fon_call();
}

void sprites_changed() { wdata().Actualise |= BitSprites; }

// MkMa1 / NoMa1: masks of count images from entry on
void make_masks(uint8_t *entry, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++, entry += 8) {
        if (!peek_l(entry))
            continue;
        Regs r = {};
        r.a[2] = address(entry);
        sy_call(sy::MaskMk, r);
        if (r.d[0])
            error(24);
    }
}

void remove_masks(uint8_t *entry, uint16_t count)
{
    for (uint16_t i = 0; i < count; i++, entry += 8) {
        if (!peek_l(entry))
            continue;
        int32_t mask = static_cast<int32_t>(peek_l(entry + 4));
        if (mask > 0) {
            Regs r = {};
            r.a[1] = static_cast<uint32_t>(mask);
            r.d[0] = peek_l(ptr<void>(mask));
            w::release(r.a[1], r.d[0]);
        }
        poke_l(entry + 4, 0xC0000000u);
    }
}

void mask_all(bool icon, bool make)
{
    uint16_t count;
    uint8_t *entry = image_entry(1, icon, &count);
    make ? make_masks(entry, count) : remove_masks(entry, count);
}

void mask_one(uint32_t n, bool icon, bool make)
{
    uint8_t *entry = image_entry(n, icon);
    make ? make_masks(entry, 1) : remove_masks(entry, 1);
}

// Ritoune: the zone of Get Bob / Get Sprite / Get Icon
struct Zone {
    uint32_t x1, y1, w, h, x2;
};

Zone grab_zone(Screen *s, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    if (y2 < 0 || x2 < 0 || y1 < 0 || x1 < 0)
        fon_call();
    if (static_cast<uint16_t>(x2) > s->EcTx)
        fon_call();
    if (static_cast<uint16_t>(y2) > s->EcTy)
        fon_call();
    uint16_t w = static_cast<uint16_t>(x2 - x1), h = static_cast<uint16_t>(y2 - y1);
    if (static_cast<uint16_t>(x2) <= static_cast<uint16_t>(x1) || static_cast<uint16_t>(y2) <= static_cast<uint16_t>(y1))
        fon_call();
    return {static_cast<uint32_t>(x1), static_cast<uint32_t>(y1), w, h, static_cast<uint32_t>(x2)};
}

// GS / GI: image n of the sprite / icon bank from the zone of the screen
void grab(Screen *s, uint32_t n, int32_t x1, int32_t y1, int32_t x2, int32_t y2, bool icon)
{
    Zone z = grab_zone(s, x1, y1, x2, y2);
    if (static_cast<int32_t>(n) <= 0)
        fon_call();
    RegFrame f = {};
    f.d[0] = n;
    uint32_t entry = icon ? lib_bnk_ad_icon(f) : lib_bnk_ad_bob(f);
    if (entry) {
        RegFrame e = {};
        e.a[0] = entry;
        lib_bnk_eff_bob_a0(e);
    } else {
        // make the bank bigger (or make it)
        RegFrame g = {};
        g.d[0] = 1;
        g.d[1] = n;
        if (icon ? lib_bnk_res_ico(g) : lib_bnk_res_bob(g))
            error(24);
        bank_changed();
        RegFrame h = {};
        h.d[0] = n;
        entry = icon ? lib_bnk_ad_icon(h) : lib_bnk_ad_bob(h);
        if (!entry)
            fon_call();
    }
    Regs r = {};
    r.d[2] = z.x1;
    r.d[3] = z.y1;
    r.d[4] = z.w;
    r.d[5] = z.h;
    r.d[6] = z.x2;
    r.a[1] = address(s);
    r.a[2] = entry;
    sy_call(sy::SprGet, r);
    if (r.d[0])
        error(24);
    if (icon)
        poke_l(ptr<uint8_t>(entry) + 4, 0xC0000000u);
}

Screen *screen_number(uint32_t n)
{
    Regs r = {};
    r.d[1] = n;
    ec_call(ec::AdrEc, r);
    if (!r.d[0])
        screen_not_open();
    return ptr<Screen>(r.d[0]);
}

Screen *current_screen()
{
    if (!idata().ScOnAd)
        screen_not_open();
    return ptr<Screen>(idata().ScOnAd);
}

// XYBob / XYSp: d1 = x, d2 = y, d3 = image
Regs &where(int function, int32_t n)
{
    if (n < 0)
        fon_call();
    static Regs r;
    r = Regs{};
    r.d[1] = n;
    return sy_fon(function, r);
}

uint32_t collision(int function, uint32_t n, uint32_t from, uint32_t to)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = from;
    r.d[3] = to;
    sy_call(function, r);
    return r.d[0];
}

} // namespace

// AdBob / AdIcon: the entry of image n of the sprite / icon bank (bits
// 14-15, the flips, left out). Returns it; *count gets the number of
// images.
uint8_t *amos::image_entry(uint32_t n, bool icon, uint16_t *count)
{
    n &= 0x3FFF;
    if (!n)
        fon_call();
    RegFrame r = {};
    r.d[0] = n;
    uint32_t entry = icon ? lib_bnk_ad_icon(r) : lib_bnk_ad_bob(r);
    if (!entry) {
        // AdBErr: no bank at all, or no such image
        if (!static_cast<uint16_t>(r.d[1]))
            error(36);
        error(45 + 30 - 1);
    }
    if (count)
        *count = static_cast<uint16_t>(r.d[1]);
    return ptr<uint8_t>(entry);
}

// Paste: image n (its bank entry) at x,y of the current screen
void amos::paste(uint32_t x, uint32_t y, uint32_t n, uint8_t *entry)
{
    Regs r = {};
    r.a[2] = address(entry);
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = 0;
    r.d[5] = -1u;
    r.a[1] = idata().BufBob;
    sy_call(sy::Patch, r);
}

// ---- updates ------------------------------------------------------------

extern "C" void lib_in_update_off() { idata().ActuMask &= ~(BitBobs | BitSprites); }
extern "C" void lib_in_update_on() { idata().ActuMask |= BitBobs | BitSprites; }

extern "C" void lib_in_bob_update()
{
    sy0(sy::EffBob);
    sy0(sy::ActBob);
    sy0(sy::AffBob);
    Regs r = {};
    ec_call(ec::SwapScS, r);
}

extern "C" void lib_in_sprite_update()
{
    sy0(sy::ActHs);
    sy0(sy::AffHs);
}

extern "C" void lib_in_update()
{
    lib_in_bob_update();
    lib_in_sprite_update();
}

extern "C" void lib_in_bob_update_off() { idata().ActuMask &= ~BitBobs; }
extern "C" void lib_in_bob_update_on() { idata().ActuMask |= BitBobs; }
extern "C" void lib_in_sprite_update_off() { idata().ActuMask &= ~BitSprites; }
extern "C" void lib_in_sprite_update_on() { idata().ActuMask |= BitSprites; }

// UPDATE EVERY n
extern "C" void lib_in_update_every(uint32_t n)
{
    if (n >= 65536)
        fon_call();
    idata().VBLDelai = static_cast<uint16_t>(n);
}

// BOB CLEAR, BOB DRAW
extern "C" void lib_in_bob_clear() { sy0(sy::EffBob); }

extern "C" void lib_in_bob_draw()
{
    sy0(sy::ActBob);
    sy0(sy::AffBob);
}

// LIMIT BOB [[n,]x1,y1 TO x2,y2]
static void limit_bob(uint32_t n, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x1;
    r.d[3] = y1;
    r.d[4] = x2;
    r.d[5] = y2;
    sy_fon(sy::LimBob, r);
}

extern "C" void lib_in_limit_bob0() { limit_bob(-1u, EntNul, EntNul, EntNul, EntNul); }
extern "C" void lib_in_limit_bob4(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    limit_bob(-1u, x1, y1, x2, y2);
}
extern "C" void lib_in_limit_bob5(uint32_t n, uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    limit_bob(n, x1, y1, x2, y2);
}

// PRIORITY ON/OFF, PRIORITY REVERSE ON/OFF
static void priority(uint32_t d1, uint32_t d2)
{
    need_screen();
    Regs r = {};
    r.d[1] = d1;
    r.d[2] = d2;
    sy_call(sy::SPrio, r);
}

extern "C" void lib_in_priority_on() { priority(1, -1u); }
extern "C" void lib_in_priority_off() { priority(0, -1u); }
extern "C" void lib_in_priority_reverse_on() { priority(-1u, 1); }
extern "C" void lib_in_priority_reverse_off() { priority(-1u, 0); }

// ---- positions ------------------------------------------------------------

// =X BOB(n), =Y BOB(n), =I BOB(n), =X SPRITE(n), =Y SPRITE(n), =I SPRITE(n)
extern "C" uint32_t lib_fn_xbob(int32_t n) { return static_cast<int16_t>(where(sy::XYBob, n).d[1]); }
extern "C" uint32_t lib_fn_ybob(int32_t n) { return static_cast<int16_t>(where(sy::XYBob, n).d[2]); }
extern "C" uint32_t lib_fn_ibob(int32_t n) { return static_cast<int16_t>(where(sy::XYBob, n).d[3]); }
extern "C" uint32_t lib_fn_xsprite(int32_t n) { return static_cast<int16_t>(where(sy::XYSp, n).d[1]); }
extern "C" uint32_t lib_fn_ysprite(int32_t n) { return static_cast<int16_t>(where(sy::XYSp, n).d[2]); }
extern "C" uint32_t lib_fn_isprite(int32_t n) { return static_cast<int16_t>(where(sy::XYSp, n).d[3]); }

// X MOUSE=x, =X MOUSE, Y MOUSE=y, =Y MOUSE
extern "C" void lib_in_xmouse(uint32_t x)
{
    Regs r = {};
    r.d[1] = x;
    r.d[2] = EntNul;
    sy_call(sy::SetM, r);
}

extern "C" void lib_in_ymouse(uint32_t y)
{
    Regs r = {};
    r.d[1] = EntNul;
    r.d[2] = y;
    sy_call(sy::SetM, r);
}

extern "C" uint32_t lib_fn_xmouse()
{
    Regs r = {};
    return static_cast<int16_t>(sy_regs(sy::XyMou, r).d[1]);
}

extern "C" uint32_t lib_fn_ymouse()
{
    Regs r = {};
    return static_cast<int16_t>(sy_regs(sy::XyMou, r).d[2]);
}

// =MOUSE KEY, =MOUSE CLICK
extern "C" uint32_t lib_fn_mouse_key()
{
    Regs r = {};
    return static_cast<uint16_t>(sy_regs(sy::MouseKey, r).d[1]);
}

extern "C" uint32_t lib_fn_mouse_click()
{
    Regs r = {};
    return static_cast<uint16_t>(sy_regs(sy::MouRel, r).d[1]);
}

// HIDE [ON], SHOW [ON]
extern "C" void lib_in_hide() { sy_d1(sy::Hide, 0); }
extern "C" void lib_in_hide_on() { sy_d1(sy::Hide, -1u); }
extern "C" void lib_in_show() { sy_d1(sy::Show, 0); }
extern "C" void lib_in_show_on() { sy_d1(sy::Show, -1u); }

// LIMIT MOUSE [screen | x1,y1 TO x2,y2]
static void limit_mouse_screen(uint32_t d1)
{
    need_screen();
    Regs r = {};
    r.d[1] = d1;
    sy_call(sy::LimitMEc, r);
    if (r.d[0])
        screen_error(r.d[0]);
}

extern "C" void lib_in_limit_mouse0() { limit_mouse_screen(0); }

extern "C" void lib_in_limit_mouse1(int32_t n)
{
    if (n < 0)
        fon_call();
    limit_mouse_screen(n + 1);
}

extern "C" void lib_in_limit_mouse4(uint32_t x1, uint32_t y1, uint32_t x2, uint32_t y2)
{
    need_screen();
    Regs r = {};
    r.d[1] = x1;
    r.d[2] = y1;
    r.d[3] = x2;
    r.d[4] = y2;
    sy_call(sy::LimitM, r);
}

// CHANGE MOUSE n
extern "C" void lib_in_change_mouse(int32_t n)
{
    if (n - 1 < 0)
        fon_call();
    sy_d1(sy::ChangeM, n - 1);
}

// ---- bobs and sprites -----------------------------------------------------

// SET BOB n,background,planes,minterm (omitted: 0, all, default)
extern "C" void lib_in_set_bob(uint32_t n, uint32_t back, uint32_t planes, uint32_t minterm)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = r.d[3] = r.d[4] = EntNul;
    r.d[5] = back == EntNul ? 0 : back;
    r.d[6] = planes == EntNul ? -1u : planes;
    r.d[7] = minterm == EntNul ? 0 : minterm;
    need_screen();
    sy_set_bob(r);
}

// BOB n,x,y,image
extern "C" void lib_in_bob(uint32_t n, uint32_t x, uint32_t y, uint32_t image)
{
    need_screen();
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = image;
    r.d[5] = 0;
    r.d[6] = -1u;
    r.d[7] = 0;
    sy_set_bob(r);
}

// BOB OFF [n]
extern "C" void lib_in_bob_off0() { sy0(sy::OffBobS); }
extern "C" void lib_in_bob_off1(uint32_t n) { sy_d1(sy::OffBob, n); }

// SET SPRITE BUFFER lines
extern "C" void lib_in_set_sprite_buffer(uint32_t lines)
{
    if (lines < 16)
        fon_call();
    Regs r = {};
    r.d[1] = lines;
    sy_call(sy::SBufHs, r);
    if (r.d[0])
        error(79 + 25);
}

// SPRITE PRIORITY n
extern "C" void lib_in_sprite_priority(uint32_t n)
{
    need_screen();
    if (n > 4)
        fon_call();
    sy_d1(sy::PriHs, n);
}

// SPRITE n,x,y,image, SPRITE OFF [n]
extern "C" void lib_in_sprite(uint32_t n, uint32_t x, uint32_t y, uint32_t image)
{
    Regs r = {};
    r.d[1] = n;
    r.d[2] = x;
    r.d[3] = y;
    r.d[4] = image;
    sy_fon(sy::NXYAHs, r);
    sprites_changed();
}

extern "C" void lib_in_sprite_off0()
{
    sy0(sy::OffHs);
    sprites_changed();
}

extern "C" void lib_in_sprite_off1(uint32_t n)
{
    Regs r = {};
    r.d[1] = n;
    sy_fon(sy::XOffHs, r);
    sprites_changed();
}

// ---- collisions -----------------------------------------------------------

// SET HARDCOL a,b, =HARDCOL(n)
extern "C" void lib_in_set_hardcol(uint32_t a, uint32_t b)
{
    Regs r = {};
    r.d[1] = 0x0F;
    r.d[2] = a;
    r.d[3] = b;
    sy_call(sy::SetHCol, r);
}

extern "C" uint32_t lib_fn_hardcol(int32_t n)
{
    if (n >= 0 && n >= 8)
        fon_call();
    Regs r = {};
    r.d[1] = static_cast<uint16_t>(n);
    return sy_regs(sy::GetHCol, r).d[1];
}

// =BOB SPRITE COL(n[,from TO to]), =BOB COL, =SPRITE BOB COL, =SPRITE COL
static void check_positive(int32_t v)
{
    if (v < 0)
        fon_call();
}

extern "C" uint32_t lib_fn_bob_sprite_col1(int32_t n)
{
    check_positive(n);
    return collision(sy::ColBob, n, 1u << 31, 63);
}

extern "C" uint32_t lib_fn_bob_sprite_col3(int32_t n, int32_t from, uint32_t to)
{
    if (to > 63)
        fon_call();
    check_positive(from);
    check_positive(n);
    return collision(sy::ColBob, n, static_cast<uint32_t>(from) | 1u << 31, to);
}

extern "C" uint32_t lib_fn_bob_col1(int32_t n)
{
    check_positive(n);
    return collision(sy::ColBob, n, 0, 10000);
}

extern "C" uint32_t lib_fn_bob_col3(int32_t n, int32_t from, int32_t to)
{
    check_positive(to);
    check_positive(from);
    check_positive(n);
    return collision(sy::ColBob, n, from, to);
}

extern "C" uint32_t lib_fn_sprite_bob_col1(int32_t n)
{
    check_positive(n);
    return collision(sy::ColSpr, n, 1u << 31, 10000);
}

extern "C" uint32_t lib_fn_sprite_bob_col3(int32_t n, int32_t from, int32_t to)
{
    check_positive(to);
    check_positive(from);
    check_positive(n);
    return collision(sy::ColSpr, n, static_cast<uint32_t>(from) | 1u << 31, to);
}

extern "C" uint32_t lib_fn_sprite_col1(int32_t n)
{
    check_positive(n);
    return collision(sy::ColSpr, n, 0, 63);
}

extern "C" uint32_t lib_fn_sprite_col3(int32_t n, int32_t from, uint32_t to)
{
    if (to > 63)
        fon_call();
    check_positive(from);
    check_positive(n);
    return collision(sy::ColSpr, n, from, to);
}

// =COL(n)
extern "C" uint32_t lib_fn_col(uint32_t n)
{
    Regs r = {};
    r.d[1] = n;
    return sy_regs(sy::ColGet, r).d[0];
}

// ---- images -------------------------------------------------------------

// MAKE [ICON] MASK [n], NO [ICON] MASK [n]
extern "C" void lib_in_make_icon_mask0() { mask_all(true, true); }
extern "C" void lib_in_make_icon_mask1(uint32_t n) { mask_one(n, true, true); }
extern "C" void lib_in_no_icon_mask0() { mask_all(true, false); }
extern "C" void lib_in_no_icon_mask1(uint32_t n) { mask_one(n, true, false); }
extern "C" void lib_in_make_mask0() { mask_all(false, true); }
extern "C" void lib_in_make_mask1(uint32_t n) { mask_one(n, false, true); }
extern "C" void lib_in_no_mask0() { mask_all(false, false); }
extern "C" void lib_in_no_mask1(uint32_t n) { mask_one(n, false, false); }

// HOT SPOT n,x,y / HOT SPOT n,code (the words go through a movem.w,
// which sign-extends them)
static void hot_spot(uint32_t n, uint32_t d1, uint32_t d2, uint32_t d3)
{
    uint8_t *entry = image_entry(n, false);
    Regs r = {};
    r.d[1] = static_cast<uint32_t>(static_cast<int16_t>(d1));
    r.d[2] = static_cast<uint32_t>(static_cast<int16_t>(d2));
    r.d[3] = static_cast<uint32_t>(static_cast<int16_t>(d3));
    r.a[2] = address(entry);
    sy_fon(sy::SpotHot, r);
}

extern "C" void lib_in_hot_spot3(uint32_t n, uint32_t x, uint32_t y) { hot_spot(n, 0, x, y); }

extern "C" void lib_in_hot_spot2(uint32_t n, uint32_t code)
{
    hot_spot(n, (code & 0xFFFF0000u) | static_cast<uint16_t>((code & 0x77) + 1), 0, code);
}

// GET SPRITE / GET BOB [screen,]n,x1,y1 TO x2,y2, GET ICON
extern "C" void lib_in_get_sprite6(uint32_t screen, uint32_t n, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    grab(screen_number(screen), n, x1, y1, x2, y2, false);
}

extern "C" void lib_in_get_sprite5(uint32_t n, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    grab(current_screen(), n, x1, y1, x2, y2, false);
}

extern "C" void lib_in_get_icon6(uint32_t screen, uint32_t n, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    grab(screen_number(screen), n, x1, y1, x2, y2, true);
}

extern "C" void lib_in_get_icon5(uint32_t n, int32_t x1, int32_t y1, int32_t x2, int32_t y2)
{
    grab(current_screen(), n, x1, y1, x2, y2, true);
}

// PUT BOB n
extern "C" void lib_in_put_bob(int32_t n)
{
    check_positive(n);
    Regs r = {};
    r.d[1] = n;
    sy_fon(sy::PutBob, r);
}

// =HREV(n), =VREV(n), =REV(n): image numbers with flip bits
extern "C" uint32_t lib_fn_hrev(uint32_t n) { return n | 0x8000; }
extern "C" uint32_t lib_fn_vrev(uint32_t n) { return n | 0x4000; }
extern "C" uint32_t lib_fn_rev(uint32_t n) { return n | 0xC000; }

// PASTE BOB x,y,n, PASTE ICON x,y,n
extern "C" void lib_in_paste_bob(uint32_t x, uint32_t y, int32_t n)
{
    check_positive(n);
    uint8_t *entry = image_entry(n, false);
    if (!peek_l(entry))
        screen_error(24);
    paste(x, y, n, entry);
}

extern "C" void lib_in_paste_icon(uint32_t x, uint32_t y, int32_t n)
{
    check_positive(n);
    uint8_t *entry = image_entry(n, true);
    if (!peek_l(entry))
        screen_error(30);
    if (!peek_l(entry + 4))
        poke_l(entry + 4, 0xC0000000u);
    paste(x, y, n, entry);
}

// =SPRITE BASE(n), =ICON BASE(n): the image, or (negative n) its mask
extern "C" uint32_t lib_fn_sprite_base(int32_t n)
{
    uint8_t *entry = image_entry(n < 0 ? -n : n, false);
    return peek_l(entry + (n < 0 ? 4 : 0));
}

extern "C" uint32_t lib_fn_icon_base(int32_t n)
{
    uint8_t *entry = image_entry(n < 0 ? -n : n, true);
    return peek_l(entry + (n < 0 ? 4 : 0));
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_sprites.inc"
#endif
