// Hardware sprites of AMOS.library (SyCall): sprites 0-7 straight to
// the hardware channels, the others shared out over the free channels
// ("columns") by height, the column buffers built with the blitter and
// shown by the VBL interrupt (+W.s HsPCop, through T_HsChange), which
// also moves the mouse (sprite 0) in them.
//
// T_HsTable: an entry of HsLong bytes per sprite, the list of the shared
// ones (by HsYr) linked by offsets in the table, its head 4 bytes before
// the table. For sprites 0-7 HsPrev says it is shown, HsNext counts the
// copies of its image still to make (one per buffer).
// T_HsTAct: per sprite, a flag byte (bit 3 changed, negative off), x, y
// and image, which Sprite and AMAL set and update() takes.

#include "core/amiga.h"
#include "w/w.h"

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::w;

// +W.s
extern "C" uint8_t w_dad_amal[], w_mouse_hide[], w_mouse_show[];

namespace {

constexpr uint16_t HsNb = 64;
constexpr uint16_t HsLong = 20;
constexpr uint16_t BitSprites = 1 << 14;       // T_Actualise

inline void barrier() { asm volatile("" ::: "memory"); }

// A table entry
struct Sprite {
    be_u16 prev, next, x, y, yr, link;
    be_u32 image, control;
};
static_assert(sizeof(Sprite) == HsLong);

uint8_t *table() { return ptr<uint8_t>(wdata().HsTable); }
Sprite *at(int16_t offset) { return reinterpret_cast<Sprite *>(table() + offset); }

// A column of T_HsPosition: its buffer (0 used, -1 the end), the first
// free line and the first free word in it
struct Column {
    be_u32 buffer;
    be_u16 y, used;
};

Column *columns() { return reinterpret_cast<Column *>(wdata().HsPosition); }

// Image header
struct Image {
    be_u16 width, height, planes, hot_x, hot_y;
    uint8_t *data() { return reinterpret_cast<uint8_t *>(this) + 10; }
};

void call_d1(uint8_t *routine, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    amos_call(&r, routine);
}

// The two control words of a sprite at x,y (the hot spot taken off,
// negative positions at 0) of image's height. Returns the y used.
uint16_t control_words(int16_t x, int16_t y, const Image *image, be_u32 *control)
{
    x -= int16_t(image->hot_x << 2) >> 2;
    if (x < 0)
        x = 0;
    y -= image->hot_y;
    if (y < 0)
        y = 0;
    uint16_t hx = uint16_t(uint16_t(x) >> 1 | uint16_t(x) << 15);
    uint16_t first = uint16_t((y & 0xFF) << 8) | (hx & 0xFF);
    uint16_t second = y & 0x100 ? 1 << 2 : 0;
    uint16_t end = y + image->height;
    second |= uint16_t((end & 0xFF) << 8);
    if (end & 0x100)
        second |= 1 << 1;
    if (hx & 0x8000)
        second |= 1;
    *control = uint32_t(first) << 16 | second;
    return y;
}

// HsSet: sprite n at x,y with image
void set(uint16_t n, uint16_t x, uint16_t y, Image *image)
{
    WData &d = wdata();
    int16_t offset = int16_t(n * HsLong);
    Sprite *s = at(offset);
    if (n < 8) {
        if (n == 0 && int16_t(d.MouShow) >= 0)
            return;                     // the mouse is there
        if (s->image != address(image)) {
            if (uint16_t(image->height + 1) >= d.HsPMax)
                return;
            s->image = address(image);
            s->next = 3;
        }
        s->prev = 1;
        s->x = x;
        s->y = y;
        control_words(x, y, image, &s->control);
        return;
    }
    // Out of the list if it moved in y or changed image
    if (s->prev || s->next) {
        if (s->y != y || s->image != address(image)) {
            int16_t prev = s->prev, next = s->next;
            s->prev = 0;
            s->next = 0;
            at(prev)->next = next;
            if (next)
                at(next)->prev = prev;
        }
    }
    s->x = x;
    s->y = y;
    s->image = address(image);
    uint16_t yr = control_words(x, y, image, &s->control);
    s->yr = yr;
    if (s->prev || s->next)
        return;
    // Into the list, by line, then by number
    int16_t before = -4;
    for (int16_t o = at(-4)->next; o; o = at(o)->next) {
        before = o;
        uint16_t oyr = at(o)->yr;
        if (yr < oyr || (yr == oyr && offset < o)) {
            int16_t prev = at(o)->prev;
            s->prev = prev;
            at(o)->prev = offset;
            at(prev)->next = offset;
            s->next = o;
            return;
        }
    }
    at(before)->next = offset;
    s->prev = before;
}

// HsUSet: sprite n off
void unset(uint16_t n)
{
    Sprite *s = at(int16_t(n * HsLong));
    if (n < 8) {
        s->prev = 0;
        s->next = 0;
    } else {
        if (!s->prev && !s->next)
            return;
        int16_t prev = s->prev, next = s->next;
        s->prev = 0;
        s->next = 0;
        at(prev)->next = next;
        if (next)
            at(next)->prev = prev;
    }
    s->x = 0;
    s->y = 0;
    s->image = 0;
}

// HsBlit: one column (word) of two planes of a sprite image (width
// words, height lines) into a sprite buffer (to, after its control
// words), interleaved. Returns past it; from moves on two planes.
uint8_t *copy_column(uint8_t *to, const uint8_t *&from, uint16_t width, uint16_t height)
{
    auto &c = chips();
    hal::wait_blit();
    c.bltcon0 = 0x03AA;
    c.bltcon1 = 0;
    c.bltcmod = uint16_t((width - 1) << 1);
    c.bltdmod = 2;
    uint16_t size = uint16_t(height << 6) | 1;
    uint32_t plane = uint32_t(uint16_t(width << 1)) * height;
    c.dmacon = 0x8040;
    c.bltcpt = const_cast<uint8_t *>(from);
    c.bltdpt = to;
    c.bltsize = size;
    from += plane;
    hal::wait_blit();
    c.bltcpt = const_cast<uint8_t *>(from);
    c.bltdpt = to + 2;
    c.bltsize = size;
    from += plane;
    return to + int16_t(height << 2);
}

// HsAff: the sprite buffers made again in the logic buffer, which then
// goes to the VBL interrupt to show. (Written with the labels of the
// assembler, whose flow it follows.)
void show()
{
    WData &d = wdata();
    d.HsChange = 0;
    hal::own_blitter();
    uint8_t *entry = table();
    Sprite *s;
    uint32_t column_size = int16_t(d.HsTCol);
    uint16_t free = 0;
    Column *const first = columns();
    Column *col = first;
    uint8_t *buffer = ptr<uint8_t>(d.HsLogic);
    int16_t left = 7;
    uint32_t control = 0;
    uint16_t words = 0, height = 0;
    Image *image = nullptr;
    const uint8_t *data = nullptr, *from;
    uint8_t *to;
    int16_t o;
    uint16_t tries, y, end1, end2, start;

    // Sprites 0-7; the mouse takes the first channel
    if (int16_t(d.MouShow) >= 0) {
        col->buffer = 0;
        col++;
        goto HsAd6;
    }
HsAd0:
    s = reinterpret_cast<Sprite *>(entry);
    if (s->prev)
        goto HsAd1;
    col->buffer = address(buffer);
    col->y = 0;
    col->used = 0;
    col++;
    poke_l(buffer, 0);
    free++;
HsAd6:
    buffer += column_size;
    entry += HsLong;
    if (--left != -1)
        goto HsAd0;
    goto HsAd7;
HsAd1:
    control = s->control;
    image = ptr<Image>(s->image);
    if (!s->next)
        goto HsAdP;
    s->next--;
    words = image->width;
    data = image->data();
    if (image->planes >= 4)
        goto HsAd3;
HsAd2:
    col->buffer = 0;
    col++;
    poke_l(buffer, control);
    from = data;
    poke_l(copy_column(buffer + 4, from, image->width, image->height), 0);
    if (!--words)
        goto HsAd6;
    data += 2;
    buffer += column_size;
    control += 0x00080000;
    entry += HsLong;
    if (--left != -1)
        goto HsAd2;
    goto HsAd7;
HsAd3:
    if (!(left & 1)) {
        col->buffer = 0;
        col++;
        poke_l(buffer, 0);
        buffer += column_size;
        entry += HsLong;
        if (--left < 0)
            goto HsAd7;
    }
HsAd4:
    col->buffer = 0;
    col++;
    entry += HsLong;
    left--;
    poke_l(buffer, control);
    from = data;
    poke_l(copy_column(buffer + 4, from, image->width, image->height), 0);
    col->buffer = 0;
    col++;
    buffer += column_size;
    poke_l(buffer, control | 0x80);
    poke_l(copy_column(buffer + 4, from, image->width, image->height), 0);
    if (!--words)
        goto HsAd6;
    data += 2;
    buffer += column_size;
    control += 0x00080000;
    entry += HsLong;
    if (--left != -1)
        goto HsAd4;
    goto HsAd7;
HsAdP:
    // The image is in the buffers already: the control words only
    words = image->width;
    if (image->planes >= 4)
        goto HsAdP2;
HsAdP1:
    col->buffer = 0;
    col++;
    poke_l(buffer, control);
    if (!--words)
        goto HsAd6;
    entry += HsLong;
    buffer += column_size;
    control += 0x00080000;
    if (--left != -1)
        goto HsAdP1;
    goto HsAd7;
HsAdP2:
    if (!(left & 1)) {
        col->buffer = 0;
        col++;
        poke_l(buffer, 0);
        buffer += column_size;
        entry += HsLong;
        if (--left < 0)
            goto HsAd7;
    }
HsAdP3:
    col->buffer = 0;
    col++;
    poke_l(buffer, control);
    buffer += column_size;
    entry += HsLong;
    left--;
    col->buffer = 0;
    col++;
    poke_l(buffer, control | 0x80);
    if (!--words)
        goto HsAd6;
    entry += HsLong;
    buffer += column_size;
    control += 0x00080000;
    if (--left != -1)
        goto HsAdP3;
HsAd7:
    col->buffer = 0xFFFFFFFF;

    // The shared sprites, in the free columns
    if (!free)
        goto HsAFini;
    o = -4;
HsA3:
    col = first - 1;
HsA4:
    col++;
HsA4a:
    if (int32_t(col->buffer) < 0)
        goto HsA3;
    if (!col->buffer)
        goto HsA4;
    o = at(o)->next;
    if (!o)
        goto HsAFini;
    s = at(o);
    image = ptr<Image>(s->image);
    data = image->data();
    words = image->width;
    control = s->control;
    tries = 8;
    height = image->height + 1;
    if (image->planes >= 4)
        goto HsMAff;
HsA6:
    y = s->yr;
    if (y < col->y)
        goto HsA10;
    end1 = col->used + height;
    if (end1 >= d.HsPMax)
        goto HsA10;
    col->y = y + height;
    start = col->used;
    col->used = end1;
    to = ptr<uint8_t>(col->buffer) + int16_t(start << 2);
    poke_l(to, control);
    from = data;
    poke_l(copy_column(to + 4, from, image->width, height - 1), 0);
    if (!--words)
        goto HsA4;
    data += 2;
    tries = 8;
    control += 0x00080000;
    goto HsA11;
HsA10:
    if (!--tries)
        goto HsA4a;
HsA11:
    col++;
HsA12:
    if (!col->buffer)
        goto HsA10;
    if (int32_t(col->buffer) > 0)
        goto HsA6;
    col = first;
    goto HsA12;

    // 16 colours: two columns from an even one
HsMAff:
    tries = 4;
    if ((col - first) & 1) {
        col++;
        goto HsMA7;
    }
HsMA1:
    y = s->yr;
    if (y < col[1].y)
        goto HsMA5;
    end2 = col[1].used + height;
    if (end2 >= d.HsPMax)
        goto HsMA5;
    if (y < col->y)
        goto HsMA5;
    end1 = col->used + height;
    if (end1 >= d.HsPMax)
        goto HsMA5;
    col->y = y + height;
    col[1].y = y + height;
    start = col->used;
    col->used = end1;
    col[1].used = end2;
    to = ptr<uint8_t>(col->buffer) + int16_t(start << 2);
    poke_l(to, control);
    from = data;
    poke_l(copy_column(to + 4, from, image->width, height - 1), 0);
    to = ptr<uint8_t>(col[1].buffer) + int16_t(start << 2);
    poke_l(to, control | 0x80);
    poke_l(copy_column(to + 4, from, image->width, height - 1), 0);
    if (!--words) {
        col++;
        goto HsA4;
    }
    data += 2;
    tries = 4;
    control += 0x00080000;
    goto HsMA1;
HsMA5:
    if (!--tries)
        goto HsA4a;
    col += 2;
HsMA7:
    if (!col->buffer)
        goto HsMA5;
    if (int32_t(col->buffer) < 0) {
        col = first;
        goto HsMA7;
    }
    if (col[1].buffer)
        goto HsMA1;
    // (The assembler went on to the next pair without counting the try:
    // it looped for ever when there was no free pair)
    goto HsMA5;

HsAFini:
    // The new buffer to show; the mouse waits
    if (d.CopON) {
        uint16_t mouse = d.MouShow;
        d.MouShow = 0xFFFF;
        barrier();
        uint32_t physic = d.HsPhysic, logic = d.HsLogic, inter = d.HsInter;
        d.HsPhysic = logic;
        d.HsLogic = inter;
        d.HsInter = physic;
        barrier();
        d.HsChange = logic;
        barrier();
        d.MouShow = mouse;
    }
    hal::wait_blit();
    hal::disown_blitter();
}

// HsAct: the sprites whose T_HsTAct changed are set or taken off
void update()
{
    WData &d = wdata();
    const uint8_t *bank = ptr<uint8_t>(d.SprBank);
    if (!bank)
        return;
    uint16_t count = peek_w(bank);
    uint8_t *a = reinterpret_cast<uint8_t *>(d.HsTAct);
    for (uint16_t n = 0; n < HsNb; n++, a += 8) {
        int8_t flags = int8_t(a[0]);
        if (!flags)
            continue;
        if (flags < 0) {
            poke_w(a, 0);
            poke_w(a + 6, 0);
            unset(n);
            continue;
        }
        poke_w(a, 0);
        uint16_t image = peek_w(a + 6) & 0x3FFF;
        if (!image || image > count)
            continue;
        uint32_t i = peek_l(bank + 2 + (image - 1) * 8);
        if (i)
            set(n, peek_w(a + 2), peek_w(a + 4), ptr<Image>(i));
    }
}

uint8_t *actual(uint16_t n) { return reinterpret_cast<uint8_t *>(wdata().HsTAct) + n * 8; }

// HsXOff without the update request
void off(uint16_t n)
{
    uint8_t *a = actual(n);
    poke_w(a, 0);
    poke_w(a + 6, 0);
    Regs r;
    r.a[0] = address(a);
    amos_call(&r, w_dad_amal);
    unset(n);
}

} // namespace

namespace amos::w {

// HsRBuf: the three sprite buffers (physical, logical, in between) for
// sprites of lines lines, and the columns. False if no chip memory.
bool sprite_buffers(uint16_t lines)
{
    WData &d = wdata();
    d.HsBuffer = 0;
    d.HsPMax = 0;
    d.HsTCol = 0;
    d.HsNLine = lines;
    // (the original's 16-bit mulu reserved too little from 2046 lines)
    uint32_t one = uint32_t(lines) * 4 * 8;
    d.HsTBuf = one * 3;
    uint32_t buffer = chip_mm(d.HsTBuf);
    if (!buffer)
        return false;
    d.HsBuffer = buffer;
    d.HsPhysic = buffer;
    d.HsLogic = buffer + one;
    d.HsInter = buffer + 2 * one;
    d.HsTCol = uint16_t(one >> 3);
    d.HsPMax = uint16_t((one >> 3 & 0xFFFF) >> 2) - 2;
    return true;
}

// HsEBuf
void sprite_buffers_free()
{
    WData &d = wdata();
    if (!d.HsBuffer)
        return;
    d.HsPMax = 0;
    d.HsTCol = 0;
    uint32_t buffer = d.HsBuffer;
    d.HsBuffer = 0;
    free_mm(buffer, d.HsTBuf);
}

// HsInit: the table of the sprites, the buffers for lines lines
bool sprites_init(uint16_t lines)
{
    uint32_t table = fast_mm(HsNb * HsLong + 4);
    if (!table)
        return false;
    wdata().HsTable = table + 4;
    return sprite_buffers(lines);
}

// HsEnd
void sprites_end()
{
    WData &d = wdata();
    if (d.HsTable)
        free_mm(d.HsTable - 4, HsNb * HsLong + 4);
    sprite_buffers_free();
}

} // namespace amos::w

extern "C" {
void w_sy_sprite_set(AllRegs *r) { set(r->d[1], r->d[2], r->d[3], ptr<Image>(r->a[1])); }

void w_sy_sprite_unset(AllRegs *r)
{
    unset(r->d[1]);
    r->d[0] = 0;
}

void w_sy_sprites_show(AllRegs *) { show(); }
void w_sy_sprites_update(AllRegs *) { update(); }

// HsBank
void w_sy_sprite_bank(AllRegs *r)
{
    wdata().SprBank = r->a[1];
    r->d[0] = 0;
}

// HsNxya: Sprite n,x,y,image (EntNul: unchanged; an unset x or y is an
// error)
void w_sy_sprite(AllRegs *r)
{
    uint16_t n = r->d[1];
    if (n >= HsNb) {
        r->d[0] = 1;
        return;
    }
    uint8_t *a = actual(n);
    uint16_t x = r->d[2], y = r->d[3], image = r->d[4];
    if (r->d[2] == EntNul && !(x = peek_w(a + 2))) {
        r->d[0] = 1;
        return;
    }
    if (r->d[3] == EntNul && !(y = peek_w(a + 4))) {
        r->d[0] = 1;
        return;
    }
    if (r->d[4] == EntNul)
        image = peek_w(a + 6);
    a[0] |= 8;
    poke_w(a + 2, x);
    poke_w(a + 4, y);
    poke_w(a + 6, image);
    wdata().Actualise |= BitSprites;
    r->d[0] = 0;
}

void w_sy_sprite_off(AllRegs *r)
{
    if (uint16_t(r->d[1]) >= HsNb) {
        r->d[0] = 1;
        return;
    }
    off(r->d[1]);
    wdata().Actualise |= BitSprites;
    r->d[0] = 0;
}

void w_sy_sprites_off(AllRegs *r)
{
    for (uint16_t n = 0; n < HsNb; n++)
        off(n);
    wdata().Actualise |= BitSprites;
    r->d[0] = 0;
}

// HsXY: d1-d3 the x, y and image of sprite d1 (low words)
void w_sy_sprite_xy(AllRegs *r)
{
    if (uint16_t(r->d[1]) >= HsNb) {
        r->d[0] = 1;
        return;
    }
    const uint8_t *a = actual(r->d[1]);
    r->d[1] = (r->d[1] & 0xFFFF0000) | peek_w(a + 2);
    r->d[2] = (r->d[2] & 0xFFFF0000) | peek_w(a + 4);
    r->d[3] = (r->d[3] & 0xFFFF0000) | peek_w(a + 6);
    r->d[0] = 0;
}

// HsStAct: all sprites off the display, their T_HsTAct kept
void w_sy_sprites_stop(AllRegs *r)
{
    for (uint16_t n = 0; n < HsNb; n++)
        unset(n);
    r->d[0] = 0;
}

// HsReAct: all sprites set again at the next update
void w_sy_sprites_restart(AllRegs *)
{
    uint8_t *a = reinterpret_cast<uint8_t *>(wdata().HsTAct);
    for (uint16_t n = 0; n < HsNb; n++, a += 8)
        if (int8_t(a[0]) >= 0)
            a[0] |= 8;
}

// HsPri: Sprite Priority d1 (0-4) of the current screen (of the first
// playfield for the second one of a dual playfield)
void w_sy_sprite_priority(AllRegs *r)
{
    WData &d = wdata();
    Screen *s = ptr<Screen>(d.EcCourant);
    uint16_t priority = r->d[1];
    if (priority >= 5)
        priority = 0;
    uint16_t con2 = s->EcCon2 & 0x78;
    int16_t dual = s->EcDual;
    if (dual < 0) {
        s = ptr<Screen>(d.EcAdr[uint16_t(-dual) - 1]);
        if (!s) {
            r->d[0] = 0;
            return;
        }
        priority <<= 3;
        con2 = s->EcCon2 & 0x47;
    }
    s->EcCon2 = con2 | priority;
    r->d[0] = 0;
}

// HsSBuf: Set Sprite Buffer d1 lines. 1 if there was not enough memory
// (fewer lines then).
void w_sy_sprite_buffer(AllRegs *r)
{
    WData &d = wdata();
    r->d[0] = 0;
    if (!d.CopON)
        return;
    uint16_t lines = r->d[1] + 2;
    if (lines == d.HsNLine)
        return;
    d.MouShow = 0xFFFF;
    w_sy_sprites_off(r);
    call_d1(w_mouse_hide, 0xFFFFFFFF);
    // The sprite registers to an empty sprite, from the next VBL
    d.HsTCol = 0;
    uint8_t *empty = ptr<uint8_t>(d.CopLogic + d.CopLong) - 4;
    poke_l(empty, 0);
    barrier();
    d.HsChange = address(empty);
    while (*static_cast<volatile be_u32 *>(&d.HsChange))
        ;
    sprite_buffers_free();
    r->d[0] = 0;
    if (sprite_buffers(lines)) {
        call_d1(w_mouse_show, 0xFFFFFFFF);
        return;
    }
    // At least 16 lines, else 2
    d.HsNLine = 16;
    if (!sprite_buffers(16) && !sprite_buffers(2)) {
        r->d[0] = 1;
        return;
    }
    d.MouDes = d.MouBank;
    d.MouSpr = 0;
    call_d1(w_mouse_show, 0xFFFFFFFF);
    r->d[0] = 1;
}
}
