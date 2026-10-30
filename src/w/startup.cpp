// The start and end of AMOS.library: IceStart/IceEnd (the access to the
// system functions, a second AMOS stopping the first), StartAll (the
// libraries, input handler, sprites, bobs, copper lists, screens, VBL,
// requesters, the default font) and EndAll, which undoes it, also after a
// failed start.
//
// +W.s IceStart sets W_Base and the C++ contexts before anything here
// runs.

#include "core/amiga.h"
#include "w/w.h"

#include <devices/input.h>
#include <exec/execbase.h>
#include <exec/interrupts.h>
#include <exec/io.h>
#include <exec/tasks.h>
#include <graphics/gfxbase.h>
#include <graphics/text.h>

#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::w;

// +W.s and the glue of w/: the vector tables, the entries handed to the
// system, the LoadView patch, the default font's special characters
extern "C" uint8_t w_sy_in[], w_ec_in[], w_wi_in[], w_io_handler_entry[], w_amos_loadview[], w_old_loadview[],
    w_auto_req_entry[], w_easy_req_entry[], w_req_pal[], w_def_font[], w_gfx_base_var[], w_default_mouse[], w_amal_init[],
    w_amal_end[], w_free_fonts[];

namespace {

constexpr uint8_t WFlagAA = 1 << 0, WFlagLoadView = 1 << 4;
constexpr uint16_t Version20 = 0x0200;
constexpr int SwitcherSignal = 24;
constexpr int16_t LVOLoadView = -222, LVOAutoRequest = -0x15C, LVOEasyRequest = -0x1D4 - 20 * 6;

// The name of the task: a later AMOS finds it and writes "S" into it
char task_name[] = " AMOS";

uint16_t version() { return peek_w(&wdata().WVersion); }
uint32_t data_zone() { return address(&wdata()) + sizeof(WData); }

// Send_Switcher: the signal to the AMOS Switcher, if it runs
void signal_switcher(int signal)
{
    if (struct Task *t = hal::find_task(reinterpret_cast<CONST_STRPTR>("_Switcher AMOS_")))
        hal::signal(t, 1UL << signal);
}

void wait_tof(int n)
{
    while (n--)
        hal::wait_frame();
}

// SetJump: the system's AutoRequest (and EasyRequest from V36) to these,
// the previous ones back in auto/easy
void set_requesters(uint32_t &autoreq, uint32_t &easy)
{
    autoreq = hal::set_function(wdata().IntBase, LVOAutoRequest, autoreq);
    if (hal::os_version() >= 36)
        easy = hal::set_function(wdata().IntBase, LVOEasyRequest, easy);
}

// WRequest_Start: AMOS's requesters with the colours 8-31 of the palette
void requester_start(const be_u16 *palette)
{
    WData &d = wdata();
    copy(w_req_pal + 8 * 2, palette + 8, 24 * 2);
    uint32_t a = address(w_auto_req_entry), e = address(w_easy_req_entry);
    set_requesters(a, e);
    d.PrevAuto = a;
    d.PrevEasy = e;
    d.ReqFlag = 0xFFFF;
}

// WRequest_Stop
void requester_stop()
{
    WData &d = wdata();
    if (!d.PrevAuto)
        return;
    uint32_t a = d.PrevAuto, e = d.PrevEasy;
    set_requesters(a, e);
}

// The characters from to to of the current screen's font, drawn in its
// corner, into 8 bytes each from font
void grab_characters(uint8_t *font, uint16_t from, uint16_t to)
{
    WData &d = wdata();
    Screen *s = current_screen();
    for (uint16_t c = from; c < to; c++) {
        auto rp = ptr<struct RastPort>(d.RastPort);
        rp->cp_x = 0;
        rp->cp_y = 6;
        char text = char(c);
        hal::text(rp, reinterpret_cast<CONST_STRPTR>(&text), 1);
        const uint8_t *line = ptr<uint8_t>(s->EcLogic[0]);
        for (int y = 0; y < 8; y++, line += int16_t(s->EcTLigne))
            *font++ = *line;
    }
}

// Wi_MakeFonte: the default font of the text windows (T_JeuDefo), 8x8,
// taken from the system font, with AMOS's own characters 0-31 and
// 128-159. False if no memory.
bool make_font()
{
    WData &d = wdata();
    static const uint16_t palette[32] = {};
    Regs r;
    r.d[1] = 0;
    r.d[2] = 16;
    r.d[3] = 8;
    r.d[4] = 1;
    r.d[5] = 0;
    r.d[6] = 2;
    r.a[1] = address(palette);
    ec_call(ec::Cree, r);
    bool ok = !r.d[0];
    if (ok) {
        uint8_t *font = ptr<uint8_t>(reserve(8 * 256, MemPublic | MemClear));
        d.JeuDefo = address(font);
        ok = font;
        if (ok) {
            grab_characters(font + 32 * 8, 32, 128);
            grab_characters(font + 160 * 8, 160, 256);
            copy(font, w_def_font, 8 * 32);
            copy(font + 128 * 8, w_def_font + 8 * 32, 8 * 32);
        }
    }
    Regs c;
    c.d[1] = 0;
    ec_call(ec::Del, c);
    return ok;
}

// Wi_DelFonte
void free_font()
{
    WData &d = wdata();
    if (d.JeuDefo) {
        release(d.JeuDefo, 8 * 256);
        d.JeuDefo = 0;
    }
}

struct TextFont *open_font(const char *name)
{
    struct TextAttr a = {reinterpret_cast<STRPTR>(const_cast<char *>(name)), 8, 0, 0x41};
    return hal::open_font(&a, false);
}

// The parameters of StartAll: d0 AMOS in front, d1 the Amiga-A key (0:
// the default), a0 the configuration, a1 the mouse bank (0: the one after
// this code), a2 the default palette, a3 the system font's name
bool start(AllRegs *r)
{
    WData &d = wdata();
    const uint8_t *config = ptr<uint8_t>(r->a[0]);
    be_u16 *palette = ptr<be_u16>(r->a[2]);
    // An AMOS stopped by IceStart has 5 seconds to stop
    if (version() >= Version20 && d.Stopped) {
        auto stopped = ptr<volatile char>(d.Stopped);
        for (int n = 50 * 5;; n--) {
            hal::wait_frame();
            if (*stopped != 'S')
                break;
            if (!n)
                return false;
        }
    }
    reinterpret_cast<uint8_t *>(&d.AMOSHere)[0] = uint8_t(r->d[0]);
    d.OldDma = chips().dmaconr | 0x8000;
    // Amiga-A: shifts, two ASCII codes
    uint32_t amiga_a = r->d[1] ? r->d[1] : 0x00406141;
    d.AmigA_Ascii1 = uint8_t(amiga_a);
    d.AmigA_Ascii2 = uint8_t(amiga_a >> 8);
    d.AmigA_Shifts = uint8_t(amiga_a >> 16);
    // The mouse bank, its end marked after the images; the colours 16-31
    // of the palette follow them
    if (version() >= 2) {
        uint8_t *bank = ptr<uint8_t>(r->a[1]);
        if (!bank)
            bank = w_default_mouse;                                 // the default one
        if (peek_l(bank) != 0x416D5370)                             // "AmSp"
            return false;
        uint16_t images = peek_w(bank + 4);
        if (images < 4)
            return false;
        uint8_t *image = bank + 6;
        d.MouBank = address(image);
        for (uint16_t i = 0; i < images; i++)
            image += 10 + uint32_t(peek_w(image)) * peek_w(image + 2) * peek_w(image + 4) * 2;
        poke_w(image, 0xFFFF);
        copy(palette + 16, image + 16 * 2, 16 * 2);
    }
    // The system font, topaz if it fails
    const char *font = "topaz.font";
    if (version() >= Version20 && r->a[3] && *ptr<char>(r->a[3]))
        font = ptr<char>(r->a[3]);
    struct TextFont *f = open_font(font);
    if (!f)
        f = open_font("topaz.font");
    d.DefaultFont = address(f);
    if (!f)
        return false;
    // AA: Amiga-A through LoadView, as from Kickstart 39
    auto gfx = ptr<struct GfxBase>(d.GfxBase);
    poke_l(w_gfx_base_var, d.GfxBase);
    d.WFlags = 0;
    if (gfx->ChipRevBits0 & GFXF_AA_ALICE)
        d.WFlags |= WFlagAA | WFlagLoadView;
    if (hal::os_version() >= 39)
        d.WFlags |= WFlagLoadView;
    d.MyTask = address(hal::find_task(nullptr));
    d.LayBase = address(hal::open_library(reinterpret_cast<CONST_STRPTR>("layers.library"), 0));
    if (!d.LayBase)
        return false;
    // The input handler
    clear_keys();
    open_console();
    open_input(d.IoDevice);
    auto handler = reinterpret_cast<struct Interrupt *>(d.Interrupt);
    handler->is_Code = reinterpret_cast<void (*)()>(w_io_handler_entry);
    handler->is_Data = nullptr;
    handler->is_Node.ln_Pri = 100;
    auto io = reinterpret_cast<struct IOStdReq *>(d.IoDevice);
    io->io_Data = handler;
    io->io_Command = IND_ADDHANDLER;
    hal::do_io(reinterpret_cast<struct IORequest *>(io));
    d.DevHere = 0xFFFF;
    // The default screen position
    d.DefWX = 129;
    d.DefWX2 = 129 - 16;
    d.DefWY = peek_w(config + 10);
    d.DefWY2 = d.DefWY - 8;
    if (!sprites_init(uint16_t(peek_l(config + 16))) || !bobs_init(peek_w(config + 8)) ||
        !copper_init(peek_l(config + 12)) || !screens_init())
        return false;
    call(w_amal_init);
    vbl_init();
    d.WiVect = address(w_wi_in);
    // With LoadView, AMOS's LoadView keeps the system's view from showing
    // while AMOS is in front (it tests T_AMOSHere)
    if (d.WFlags & WFlagLoadView) {
        poke_l(w_amos_loadview + 2, address(&d.AMOSHere));
        poke_l(w_old_loadview, hal::set_function(d.GfxBase, LVOLoadView, address(w_amos_loadview)));
        if (hal::os_version() >= 37)
            hal::clear_cache();
    }
    if (version() >= Version20) {
        requester_start(palette);
        if (!make_font())
            return false;
        ptr<struct Task>(d.MyTask)->tc_UserData = ptr<void>(data_zone());
        signal_switcher(SwitcherSignal);
    }
    // AMOS to the front
    uint8_t &here = reinterpret_cast<uint8_t *>(&d.AMOSHere)[0];
    if (here) {
        here = 0;
        amos_wb(1);
    }
    return true;
}

// EndAll
void end()
{
    WData &d = wdata();
    amos_wb(0);
    wait_tof(2);
    if (version() >= Version20) {
        signal_switcher(SwitcherSignal + 1);
        requester_stop();
        free_font();
    }
    if (d.WFlags & WFlagLoadView && peek_l(w_old_loadview))
        hal::set_function(d.GfxBase, LVOLoadView, peek_l(w_old_loadview));
    if (d.DevHere) {
        auto io = reinterpret_cast<struct IOStdReq *>(d.IoDevice);
        io->io_Data = d.Interrupt;
        io->io_Command = IND_REMHANDLER;
        hal::do_io(reinterpret_cast<struct IORequest *>(io));
        close_input(d.IoDevice);
        // (the original left console.device open)
        close_console();
    }
    call(w_free_fonts);
    call(w_amal_end);
    screens_end();
    bobs_end();
    sprites_end();
    rem_vbl();
    copper_end();
    if (d.FntBase)
        hal::close_library(ptr<struct Library>(d.FntBase));
    if (d.LayBase)
        hal::close_library(ptr<struct Library>(d.LayBase));
    if (d.GfxBase && d.DefaultFont)
        hal::close_font(ptr<struct TextFont>(d.DefaultFont));
}

} // namespace

extern "C" {
// IceStart (after +W.s): d0 "V2.0" and d1 the version of AMOS. A
// previous AMOS 2.0 is told to stop; this task gets AMOS's name and the
// data zone. Returns "W2.0" and $0200.
void w_sy_ice_start(AllRegs *r)
{
    WData &d = wdata();
    poke_w(&d.WVersion, r->d[0] == 0x56322E30 ? uint16_t(r->d[1]) : 0);
    d.SyVect = address(w_sy_in);
    if (version() >= Version20) {
        hal::forbid();
        if (struct Task *t = hal::find_task(reinterpret_cast<CONST_STRPTR>(task_name))) {
            t->tc_Node.ln_Name[0] = 'S';
            d.Stopped = address(t->tc_Node.ln_Name);
        }
        hal::permit();
        struct Task *me = hal::find_task(nullptr);
        d.MyTask = address(me);
        d.OldName = address(me->tc_Node.ln_Name);
        me->tc_Node.ln_Name = task_name;
        me->tc_UserData = ptr<void>(data_zone());
    }
    r->d[0] = 0x57322E30;
    r->d[1] = Version20;
}

// IceEnd: the stopped AMOS goes on, the task gets its name back
void w_sy_ice_end(AllRegs *)
{
    WData &d = wdata();
    if (version() < Version20)
        return;
    if (d.Stopped)
        *ptr<char>(d.Stopped) = ' ';
    if (d.MyTask)
        ptr<struct Task>(d.MyTask)->tc_Node.ln_Name = ptr<char>(d.OldName);
}

// StartAll: 0, or -1 after undoing everything
void w_sy_start_all(AllRegs *r)
{
    if (start(r)) {
        r->d[0] = 0;
        return;
    }
    end();
    r->d[0] = 0xFFFFFFFF;
}

// EndAll
void w_sy_end_all(AllRegs *r)
{
    end();
    r->d[0] = 0;
}
}
