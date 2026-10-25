// The system routines of the main program (+B.s): the messages of the
// configurations, the system path, memory, the system file, the current
// directory, the caches, the bank change. Most are routines of
// AMOSPro.Lib too (AMOSJmps), with their register entries in main_glue.s.

#include "main/main.h"

#include "core/amiga.h"

#include <dos/dos.h>

#include "hal/files.h"
#include <graphics/gfxbase.h>
#include <graphics/rastport.h>
#include <graphics/text.h>

#include "hal/gfx.h"

using namespace amos;
using namespace amos::boot;

// +B.s: the user name and registration of the installation
extern "C" const uint16_t main_user_reg[], main_user_name[];

namespace {

// The message past the end of a list: a byte, then ""
const uint8_t no_message[4] = {0, 0, 0, 0};

char *name1() { return ptr<char>(idata().Name1); }

// ResTempBuffer's records, to free the buffer when the program stops or is
// cleared (main_glue.s)
extern "C" uint8_t main_temp_on_clear[], main_temp_on_error[];

void add_routine(uint8_t *record, uint32_t *list)
{
    Regs r;
    r.a[1] = address(record);
    r.a[2] = address(list);
    w::add_routine(r.a[1], r.a[2]);
}

uint32_t memory(int function, uint32_t size)
{
    Regs r;
    r.d[0] = size;
    sy_call(function, r);
    return r.a[0];
}

} // namespace

namespace amos::boot {

// GetMessage: a list is a byte, then the messages: a byte, the length of
// the text, the text and a 0. Message n (from 1); d0 gets the length of
// it in its low byte, the count of the search above.
const uint8_t *get_message(const uint8_t *list, uint32_t &d0)
{
    const uint8_t *p = no_message;
    if (list) {
        p = list + 1;
        for (;;) {
            d0 = (d0 & 0xFFFF0000u) | uint16_t(d0 - 1);
            if (int16_t(d0) <= 0)
                break;
            if (*p == 0xFF) {
                p = no_message;
                break;
            }
            p += 2 + *p;
        }
    }
    d0 = (d0 & 0xFFFFFF00u) | *p;
    return p + 1;
}

// ResTempBuffer: the temporary buffer of size bytes (after a long, its
// size), freed with a size of 0. Returns its address, 0 if out of memory
// (or freed).
uint32_t temp_buffer(uint32_t size)
{
    IData &d = idata();
    if (uint32_t old = d.TempBuffer) {
        ram_free(old - 4, peek_l(ptr<void>(old - 4)) + 4);
        d.TempBuffer = 0;
    }
    if (!size)
        return 0;
    uint32_t at = ram_fast(size + 4);
    if (!at)
        return 0;
    poke_l(ptr<void>(at), size);
    d.TempBuffer = at + 4;
    add_routine(main_temp_on_clear, &d.Sys_ClearRoutines);
    add_routine(main_temp_on_error, &d.Sys_ErrorRoutines);
    return d.TempBuffer;
}

const char *message(uint32_t list, uint32_t n)
{
    uint32_t d0 = n;
    return reinterpret_cast<const char *>(get_message(ptr<uint8_t>(list), d0));
}

const char *sys_message(uint32_t n) { return message(idata().Sys_Messages, n); }

const uint8_t *def_message(uint32_t n)
{
    uint32_t resource = idata().Sys_Resource;
    uint32_t d0 = n;
    return get_message(ptr<uint8_t>(resource + peek_l(ptr<void>(resource + 6))), d0);
}

void sys_add_path(const char *name)
{
    char *to = name1();
    bool path = false;
    for (const char *p = name; *p; p++)
        if (*p == ':')
            path = true;
    if (!path) {
        for (const uint8_t *p = idata().Sys_Pathname; *p;)
            *to++ = char(*p++);
    }
    while ((*to++ = *name++))
        ;
}

const char *sys_add_path_com(const char *name)
{
    char *p = const_cast<char *>(name);
    while (*p && *p != ' ')
        p++;
    char kept = *p;
    *p = 0;
    sys_add_path(name);
    *p = kept;
    return kept ? p + 1 : p;
}

uint32_t ram_fast(uint32_t size) { return memory(sy::MemFastClear, size); }
uint32_t ram_chip(uint32_t size) { return memory(sy::MemChipClear, size); }

void ram_free(uint32_t address, uint32_t size)
{
    Regs r;
    r.d[0] = size;
    r.a[1] = address;
    w::release(r.a[1], r.d[0]);
}

uint32_t a5_reserve(uint32_t &slot, uint32_t size, uint32_t flags)
{
    Regs r;
    r.d[0] = size + 4;
    r.d[1] = flags;
    r.a[0] = w::reserve(r.d[0], r.d[1]);
    if (!r.a[0])
        return 0;
    poke_l(ptr<void>(r.a[0]), size);
    slot = r.a[0] + 4;
    return slot;
}

void a5_free(uint32_t &slot)
{
    if (uint32_t at = slot) {
        slot = 0;
        ram_free(at - 4, peek_l(ptr<void>(at - 4)) + 4);
    }
}

bool sys_open(const char *name, uint32_t mode)
{
    return (idata().Handle = uint32_t(hal::open(name, int32_t(mode)))) != 0;
}

bool sys_read(void *buffer, uint32_t length)
{
    return uint32_t(hal::read(BPTR(idata().Handle), buffer, int32_t(length))) == length;
}

int32_t sys_seek(int32_t offset, int32_t mode) { return hal::seek(BPTR(idata().Handle), offset, mode); }

void sys_close()
{
    IData &d = idata();
    if (BPTR handle = BPTR(d.Handle)) {
        d.Handle = 0;
        hal::close(handle);
    }
}

// Sys_ClearCache: CacheClearU from V37 on (below, a call to FindTask, for
// the time it takes)
void clear_cache()
{
    if (idata().WB2_0 >= 37)
        CacheClearU();
    else
        FindTask(nullptr);
}

// Sys_WaitMul: the tests of the AMOS.library, then the next frame
void wait_vbl_multi()
{
    Regs r;
    sy_call(sy::Test_Cyclique, r);
    hal::wait_frame();
}

// ReCop: the copper lists computed again
void recop()
{
    Regs r;
    sy_call(sy::WaitVbl, r);
    Regs c;
    ec_call(ec::CopForce, c);
}

void ask_dir()
{
    IData &d = idata();
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    poke_w(buffer, 0);
    poke_w(buffer + 384, 0);
    if (BPTR lock = hal::lock(reinterpret_cast<const char *>(buffer), ACCESS_READ))
        ask_dir2(uint32_t(lock));
}

// AskDir2: the full name of the directory of a lock (unlocked) at
// Buffer+384: up to the root, then down again
void ask_dir2(uint32_t lock)
{
    struct Walk {
        uint8_t *buffer;
        char *to;
        char separator;
        void down(BPTR l)
        {
            if (BPTR parent = hal::parent_dir(l))
                down(parent);
            if (hal::examine(l, reinterpret_cast<FileInfoBlock *>(buffer))) {
                for (const char *p = reinterpret_cast<const char *>(buffer + 8); (*to = *p++); to++)
                    ;
                *to++ = separator;
                *to = 0;
                separator = '/';
            }
            hal::unlock(l);
        }
    };
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    Walk w = {buffer, reinterpret_cast<char *>(buffer + 384), ':'};
    *w.to = 0;
    if (lock)
        w.down(BPTR(lock));
}

void w_option()
{
    IData &d = idata();
    if (d.WBench) {
        Regs r;
        r.d[1] = 1;
        ec_call(ec::AMOS_WB, r);
        d.WBench = 0;
    }
}

// Math_Close: the maths libraries closed
void math_close()
{
    IData &d = idata();
    uint32_t *bases[] = {&d.MathBase, &d.DFloatBase, &d.DMathBase};
    for (uint32_t *base : bases) {
        if (*base) {
            CloseLibrary(ptr<Library>(*base));
            *base = 0;
        }
    }
}

// Sys_VerInstall: installed if there is a user name and a registration
bool installed() { return main_user_name[0] && main_user_reg[0]; }

} // namespace amos::boot

// ---- register entries (main_glue.s) -------------------------------------------

// Sys_GetMessage: d0 = message. GetMessage: a0 = list too. Def_GetMessage:
// of the default resource. Out: a0 = the text, d0 = its length in the low
// byte.
extern "C" uint32_t main_get_message(RegFrame &r)
{
    r.a[0] = address(get_message(ptr<uint8_t>(r.a[0]), r.d[0]));
    return r.d[0] & 0xFF;
}

extern "C" uint32_t main_sys_get_message(RegFrame &r)
{
    r.a[0] = idata().Sys_Messages;
    return main_get_message(r);
}

extern "C" uint32_t main_def_get_message(RegFrame &r)
{
    uint32_t resource = idata().Sys_Resource;
    r.a[0] = resource + peek_l(ptr<void>(resource + 6));
    return main_get_message(r);
}

// Sys_AddPath: a0 = name, into Name1. Out: a0 after the name; d0 = ':' in
// its low byte if it has a path, else 0.
extern "C" uint32_t main_sys_add_path(RegFrame &r)
{
    const char *name = ptr<char>(r.a[0]);
    sys_add_path(name);
    bool path = false;
    while (*name)
        path |= *name++ == ':';
    r.a[0] = address(name + 1);
    r.d[0] = (r.d[0] & 0xFFFFFF00u) | (path ? ':' : 0);
    return 0;
}

// RamFast, RamChip, RamFast2, RamChip2: d0 = size. Out: d0 = address (BEQ:
// out of memory).
extern "C" uint32_t main_ram_fast(RegFrame &r) { return r.d[0] = memory(sy::MemFastClear, r.d[0]); }
extern "C" uint32_t main_ram_chip(RegFrame &r) { return r.d[0] = memory(sy::MemChipClear, r.d[0]); }
extern "C" uint32_t main_ram_fast2(RegFrame &r) { return r.d[0] = memory(sy::MemFast, r.d[0]); }
extern "C" uint32_t main_ram_chip2(RegFrame &r) { return r.d[0] = memory(sy::MemChip, r.d[0]); }

// RamFree: a1 = address, d0 = size
extern "C" uint32_t main_ram_free(RegFrame &r)
{
    ram_free(r.a[1], r.d[0]);
    return 0;
}

// ResTempBuffer: d0 = size. Out: d0 = size (BEQ: none), a0 = the buffer.
extern "C" uint32_t main_res_temp_buffer(RegFrame &r)
{
    uint32_t size = r.d[0];
    if (uint32_t buffer = temp_buffer(size)) {
        r.a[0] = buffer;
        return r.d[0] = size;
    }
    return r.d[0] = 0;
}

extern "C" void main_free_temp_buffer() { temp_buffer(0); }

// Sys_UnCode: a0 = coded string (a length word, then the bytes), a1 = to
// the uncoded one, d0 = the key. Out: d0.w = the sum of the bytes.
extern "C" uint32_t main_sys_uncode(RegFrame &r)
{
    const uint8_t *from = ptr<uint8_t>(r.a[0]);
    uint8_t *to = ptr<uint8_t>(r.a[1]);
    uint16_t length = peek_w(from);
    poke_w(to, length);
    uint16_t sum = 0;
    for (uint16_t i = 0; i < length; i++) {
        uint8_t c = uint8_t(from[2 + i] ^ r.d[0]);
        sum = uint8_t(sum + c);
        to[2 + i] = c;
    }
    r.d[0] = (r.d[0] & 0xFFFF0000u) | sum;
    return sum;
}

extern "C" uint32_t main_sys_clear_cache(RegFrame &)
{
    clear_cache();
    return 0;
}

extern "C" uint32_t main_sys_wait_mul(RegFrame &)
{
    wait_vbl_multi();
    return 0;
}

extern "C" uint32_t main_recop(RegFrame &)
{
    recop();
    return 0;
}

extern "C" uint32_t main_w_option(RegFrame &)
{
    w_option();
    return 0;
}

// AskDir, AskDir2 (d0 = lock). Out: d0 = 0.
extern "C" uint32_t main_ask_dir(RegFrame &r)
{
    ask_dir();
    return r.d[0] = 0;
}

extern "C" uint32_t main_ask_dir2(RegFrame &r)
{
    ask_dir2(r.d[0]);
    return r.d[0] = 0;
}

extern "C" uint32_t main_math_close(RegFrame &)
{
    math_close();
    return 0;
}
