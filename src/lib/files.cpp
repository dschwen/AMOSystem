// File channels and disk instructions of AMOSPro.Lib (Open, Close,
// Input$, Get, Put, Lof, Dir$, Kill...)

#include "core/amiga.h"
#include "lib/banks.h"
#include "lib/convert.h"
#include "lib/dos.h"
#include "lib/strings.h"

#include <dos/dos.h>

#include "hal/files.h"
#include "hal/system.h"

// The record that closes every channel when the program is cleared
// (dos_glue.s)
extern "C" uint8_t lib_close_all_on_clear[];
extern "C" uint32_t lib_mulu32(amos::RegFrame &r);

using namespace amos;

namespace {

// A channel (Fichiers, 10 of them): the handle, the type, the fields of
// a random access file
struct Channel {
    uint8_t bytes[10];
    BPTR handle() const { return static_cast<BPTR>(peek_l(bytes)); }
    void set_handle(BPTR h) { poke_l(bytes, static_cast<uint32_t>(h)); }
    uint8_t &type() { return bytes[4]; }
    uint32_t fields() const { return peek_l(bytes + 6); }
    void set_fields(uint32_t f) { poke_l(bytes + 6, f); }
};
static_assert(sizeof(Channel) == 10);

// Channel types (FhT)
constexpr uint8_t ChanOut = 1 << 0, ChanIn = 1 << 1, ChanPort = 1 << 2, ChanRandom = 1 << 7;

constexpr int32_t OffsetBeginning = -1, OffsetCurrent = 0, OffsetEnd = 1;

[[noreturn]] void not_open() { error(79 + 18); }
[[noreturn]] void already_open() { error(79 + 17); }
[[noreturn]] void type_mismatch() { error(79 + 19); }
[[noreturn]] void end_of_file() { error(79 + 21); }

// GetFile: channel n (1-10)
Channel &channel(uint32_t n)
{
    if (n >= 10 || n == 0)
        fon_call();
    return reinterpret_cast<Channel *>(idata().Fichiers)[n - 1];
}

Channel &open_channel(uint32_t n)
{
    Channel &c = channel(n);
    if (!c.handle())
        not_open();
    return c;
}

// RLof: an open channel on a file, not a port
Channel &file_channel(uint32_t n)
{
    Channel &c = open_channel(n);
    if (c.type() & ChanPort)
        type_mismatch();
    return c;
}

// Cloa1
void close_channel(Channel &c)
{
    if (BPTR h = c.handle()) {
        c.set_handle(0);
        hal::close(h);
        if (uint32_t fields = c.fields()) {
            c.set_fields(0);
            Regs r = {};
            r.a[1] = fields;
            r.d[0] = peek_w(ptr<void>(fields)) * 6u + 8;
            w::release(r.a[1], r.d[0]);
        }
    }
}

// CloAll
void close_all()
{
    for (uint32_t n = 1; n <= 10; n++)
        close_channel(reinterpret_cast<Channel *>(idata().Fichiers)[n - 1]);
}

// OpIn / RanApp: open channel n on the file named name
Channel &open_file(uint32_t n, const uint8_t *name)
{
    Regs r = {};
    r.a[1] = address(lib_close_all_on_clear);
    r.a[2] = address(&idata().Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
    name_disc(name);
    Channel &c = channel(n);
    if (c.handle())
        already_open();
    for (uint8_t &b : c.bytes)
        b = 0;
    return c;
}

void open_typed(uint32_t n, const uint8_t *name, uint32_t mode, uint8_t type)
{
    Channel &c = open_file(n, name);
    BPTR h = hal::open(ptr<char>(idata().Name1), mode);
    if (!h)
        disk_error();
    c.set_handle(h);
    c.type() = type;
}

BPTR open_random(uint32_t n, const uint8_t *name, uint8_t type)
{
    Channel &c = open_file(n, name);
    BPTR h = hal::open(ptr<char>(idata().Name1), ModeOldFile);
    if (!h)
        h = hal::open(ptr<char>(idata().Name1), ModeNewFile);
    if (!h)
        disk_error();
    c.set_handle(h);
    c.type() = type;
    return h;
}

// The buffer of GetByte, in DeFloat: the handle it holds, the position and
// the number of bytes in it, then the bytes
struct ByteBuffer {
    uint8_t *p = idata().DeFloat;
    uint32_t handle() const { return peek_l(p); }
    uint16_t pos() const { return peek_w(p + 4); }
    uint16_t count() const { return peek_w(p + 6); }
};
constexpr uint32_t ByteBufferSize = 32 - 8;

// GetByte: the next byte of an input channel
uint8_t get_byte(Channel &c)
{
    if (!(c.type() & ChanIn))
        type_mismatch();
    uint8_t *b = idata().DeFloat;
    if (c.type() & ChanPort) {
        while (!hal::wait_for_char(c.handle(), 50)) {
            Regs r = {};
            lib_call(L_Test_PaSaut, r);
        }
        int32_t n = hal::read(c.handle(), b + 8, 1);
        if (!n)
            end_of_file();
        if (n < 0)
            disk_error();
        return b[8];
    }
    ByteBuffer buf;
    if (buf.handle() != static_cast<uint32_t>(c.handle()) || buf.pos() >= buf.count()) {
        do {
            poke_l(b, static_cast<uint32_t>(c.handle()));
            poke_w(b + 4, 0);
            poke_w(b + 6, 0);
            int32_t n = hal::read(c.handle(), b + 8, ByteBufferSize);
            if (!n)
                end_of_file();
            if (n < 0)
                disk_error();
            poke_w(b + 6, static_cast<uint16_t>(n));
        } while (buf.pos() >= buf.count());
    }
    uint16_t pos = buf.pos();
    poke_w(b + 4, pos + 1);
    return b[8 + pos];
}

// EndByte: give the bytes GetByte read ahead back to the file
void end_byte()
{
    uint8_t *b = idata().DeFloat;
    if (BPTR h = static_cast<BPTR>(peek_l(b))) {
        poke_l(b, 0);
        int16_t back = static_cast<int16_t>(peek_w(b + 4) - peek_w(b + 6));
        if (back)
            hal::seek(h, back, OffsetCurrent);
    }
}

// NoReq / YesReq: no "insert disk" requesters while looking
void no_requesters()
{
    auto task = reinterpret_cast<uint8_t *>(hal::find_task(nullptr));
    uint32_t &window = *reinterpret_cast<uint32_t *>(task + 0xb8);
    if (window != 0xFFFFFFFFu) {
        idata().ReqSave = window;
        window = 0xFFFFFFFFu;
    }
}

void yes_requesters()
{
    auto task = reinterpret_cast<uint8_t *>(hal::find_task(nullptr));
    uint32_t &window = *reinterpret_cast<uint32_t *>(task + 0xb8);
    if (window == 0xFFFFFFFFu)
        window = idata().ReqSave;
}

// RExist: does Name1 exist?
bool exists()
{
    no_requesters();
    BPTR lock = hal::lock(ptr<char>(idata().Name1), ACCESS_READ);
    yes_requesters();
    if (!lock)
        return false;
    hal::unlock(lock);
    return true;
}

// LockFree / LockGet: the lock of Name1 in LockSave
void lock_free()
{
    if (BPTR lock = static_cast<BPTR>(idata().LockSave)) {
        hal::unlock(lock);
        idata().LockSave = 0;
    }
}

BPTR lock_get()
{
    lock_free();
    BPTR lock = hal::lock(ptr<char>(idata().Name1), ACCESS_READ);
    if (!lock)
        disk_error();
    idata().LockSave = static_cast<uint32_t>(lock);
    return lock;
}

// AskDir / AskDir2 (+B.s): the full name of the current directory (or of
// a lock, which they unlock) in Buffer+384
char *ask_dir()
{
    boot::ask_dir();
    return ptr<char>(idata().Buffer) + 384;
}

// CopyPath: Buffer+384 becomes the current directory
void copy_path()
{
    const char *from = ptr<char>(idata().Buffer) + 384;
    char *to = ptr<char>(idata().PathAct);
    while ((*to++ = *from++))
        ;
}

// The free space of a disk: blocks * bytes per block with Mulu32
uint32_t disk_free(const struct InfoData *info)
{
    RegFrame r = {};
    r.d[3] = info->id_NumBlocks - info->id_NumBlocksUsed;
    r.d[2] = info->id_BytesPerBlock;
    return lib_mulu32(r);
}

// AskD3: a string of n characters to fill. Returns it.
uint8_t *ask_string(int32_t n)
{
    if (n <= 0)
        fon_call();
    uint8_t *s = str_alloc(n);
    idata().HiChaine = (address(s) + 2 + n + 1) & ~1u;
    poke_w(s, static_cast<uint16_t>(n));
    return s;
}

// GetPut: go to a record of a random access file, or to the end if it is
// past it. (Get and Put then compared the offset with what was meant to be
// the file size, but GetPut had put -1 there: they never stop with "end of
// file", and neither do these.)
Channel &seek_record(uint32_t n, uint32_t record)
{
    uint32_t index = (record & 0xFFFF0000u) | static_cast<uint16_t>(record - 1);
    if (index >= 65500)
        fon_call();
    Channel &c = open_channel(n);
    if (!(c.type() & ChanRandom))
        type_mismatch();
    uint8_t *fields = ptr<uint8_t>(c.fields());
    uint32_t offset = static_cast<uint16_t>(index) * static_cast<uint32_t>(peek_w(fields + 2));
    uint32_t size = peek_l(fields + 4);
    hal::seek(c.handle(), offset < size ? offset : size, OffsetBeginning);
    return c;
}

uint32_t read_header_long()
{
    uint32_t *buffer = ptr<uint32_t>(idata().Buffer);
    if (!file_read(buffer, 4))
        disk_error();
    return *buffer;
}

} // namespace

// ---- register entries ---------------------------------------------------

// GetFile: d0 = channel. Out: a2 = it, d1 = its handle (BEQ: not open).
extern "C" uint32_t lib_get_file(RegFrame &r)
{
    Channel &c = channel(r.d[0]);
    r.d[0] = (r.d[0] - 1) * 10;
    r.a[2] = address(&c);
    return r.d[1] = static_cast<uint32_t>(c.handle());
}

// Cloa1: a2 = channel. CloAll.
extern "C" uint32_t lib_cloa1(RegFrame &r)
{
    close_channel(*ptr<Channel>(r.a[2]));
    return 0;
}

extern "C" uint32_t lib_clo_all(RegFrame &)
{
    close_all();
    return 0;
}

// GetByte: a2 = channel. Out: d0 = byte. EndByte. Both keep the others.
extern "C" uint32_t lib_get_byte(RegFrame &r) { return r.d[0] = get_byte(*ptr<Channel>(r.a[2])); }

extern "C" uint32_t lib_end_byte(RegFrame &)
{
    end_byte();
    return 0;
}

// RExist: Name1. Out: d3 = -1 if it exists, else 0.
extern "C" uint32_t lib_rexist(RegFrame &r) { return r.d[3] = exists() ? -1u : 0; }

// NoReq, YesReq, LockGet, LockFree, CopyPath
extern "C" uint32_t lib_no_req(RegFrame &)
{
    no_requesters();
    return 0;
}

extern "C" uint32_t lib_yes_req(RegFrame &)
{
    yes_requesters();
    return 0;
}

// LockGet: out: d0 = d1 = the lock (also in LockSave)
extern "C" uint32_t lib_lock_get(RegFrame &r)
{
    r.d[2] = static_cast<uint32_t>(ACCESS_READ);
    return r.d[0] = r.d[1] = static_cast<uint32_t>(lock_get());
}

extern "C" uint32_t lib_lock_free(RegFrame &)
{
    lock_free();
    return 0;
}

extern "C" uint32_t lib_copy_path(RegFrame &)
{
    copy_path();
    return 0;
}

// The code of the clear record
extern "C" void lib_close_all() { close_all(); }

// ---- instructions -------------------------------------------------------

// SET INPUT c1,c2: the characters that end a line for Input #
extern "C" void lib_in_set_input(uint32_t first, uint32_t second)
{
    if (first >= 256)
        fon_call();
    idata().ChrInp = static_cast<uint16_t>(first | second << 8);
}

// =INPUT$(n): n characters from the keyboard
extern "C" uint32_t lib_fn_input1(int32_t n)
{
    uint8_t *s = ask_string(n);
    uint8_t *to = s + 2;
    for (uint16_t left = static_cast<uint16_t>(n); left;) {
        Regs t = {};
        lib_call(L_Test_PaSaut, t);
        Regs r = {};
        sy_call(sy::Inkey, r);
        uint8_t c = static_cast<uint8_t>(r.d[1]);
        if (!r.d[1] || c < 32)
            continue;
        *to++ = c;
        left--;
    }
    return address(s);
}

// =INPUT$(#channel,n): n bytes from a file or port
extern "C" uint32_t lib_fn_input2(uint32_t n, int32_t count)
{
    Channel &c = open_channel(n);
    uint8_t *s = ask_string(count);
    if (!static_cast<uint16_t>(count))
        return address(s);
    if (!(c.type() & ChanIn))
        type_mismatch();
    if (c.type() & ChanPort) {
        uint8_t *to = s + 2;
        for (uint16_t left = static_cast<uint16_t>(count); left; left--)
            *to++ = get_byte(c);
        return address(s);
    }
    int32_t got = hal::read(c.handle(), s + 2, count);
    if (!got)
        end_of_file();
    if (got < 0)
        disk_error();
    return address(s);
}

// DIR$=a$: the current directory, which has to be one
extern "C" void lib_in_dir(const uint8_t *name)
{
    name_disc(name);
    BPTR lock = lock_get();
    auto fib = ptr<struct FileInfoBlock>(idata().Buffer);
    hal::examine(lock, fib);
    if (static_cast<int16_t>(peek_w(ptr<uint8_t>(idata().Buffer) + 4)) < 0)
        disk_error();
    // AskDir2 unlocks the lock (the assembler then freed it again)
    Regs r = {};
    r.d[0] = static_cast<uint32_t>(lock);
    boot::ask_dir2(r.d[0]);
    idata().LockSave = 0;
    copy_path();
}

// =DIR$
extern "C" uint32_t lib_fn_dir()
{
    uint8_t *s = str_alloc(126);
    const char *path = ptr<char>(idata().PathAct);
    if (!*path)
        path = ask_dir();
    uint8_t *to = s + 2;
    while ((*to++ = static_cast<uint8_t>(*path++)))
        ;
    return str_finish(s, to - 1);
}

// PARENT: up one directory
extern "C" void lib_in_parent()
{
    char *path = ptr<char>(idata().PathAct);
    if (!*path) {
        ask_dir();
        copy_path();
    }
    char *p = path;
    while (*p)
        p++;
    if (*--p != '/')
        return;
    char c;
    do
        c = *--p;
    while (c != '/' && c != ':');
    p[1] = 0;
}

// KILL a$, RENAME a$ TO b$, MKDIR a$
extern "C" void lib_in_kill(const uint8_t *name)
{
    if (!hal::delete_file(name_disc(name)))
        disk_error();
}

extern "C" void lib_in_rename(const uint8_t *from, const uint8_t *to)
{
    name_disc(to);
    char *name2 = ptr<char>(idata().Name2);
    for (const char *p = ptr<char>(idata().Name1); (*name2++ = *p++);)
        ;
    if (!hal::rename(name_disc(from), ptr<char>(idata().Name2)))
        disk_error();
}

extern "C" void lib_in_mkdir(const uint8_t *name)
{
    lock_free();
    BPTR lock = hal::create_dir(name_disc(name));
    if (!lock)
        disk_error();
    idata().LockSave = static_cast<uint32_t>(lock);
    lock_free();
}

// =DRIVE(a$): is there a device "a$" (ending with ":")?
extern "C" uint32_t lib_fn_drive(const uint8_t *name)
{
    uint16_t len = str_len(name);
    if (str_chars(name)[static_cast<int16_t>(len) - 1] != ':')
        return 0;
    char *text = str_to_buffer(name);
    no_requesters();
    bool there = hal::device_exists(text);
    yes_requesters();
    return there ? -1u : 0;
}

// =DFREE: the free space of the current disk
extern "C" uint32_t lib_fn_dfree()
{
    const char *from = ptr<char>(idata().PathAct);
    char *to = ptr<char>(idata().Name1);
    while ((*to++ = *from++))
        ;
    BPTR lock = lock_get();
    auto info = ptr<struct InfoData>((idata().Buffer + 4) & ~3u);
    if (!hal::info(lock, info))
        disk_error();
    uint32_t free = disk_free(info);
    lock_free();
    return free;
}

// =DISC INFO$(a$): "Volume:" then the free space, 10 characters
extern "C" uint32_t lib_fn_disc_info(const uint8_t *name)
{
    name_disc(name);
    uint8_t *s = str_alloc(126);
    BPTR lock = lock_get();
    auto info = ptr<struct InfoData>(idata().Buffer);
    if (!hal::info(lock, info))
        disk_error();
    lock_free();
    char *to = reinterpret_cast<char *>(s + 2);
    uint16_t len = 0;
    if (BPTR node = info->id_VolumeNode) {
        if (BSTR bname = peek_l(static_cast<uint8_t *>(BADDR(node)) + 0x28)) {
            const char *p = static_cast<const char *>(BADDR(bname)) + 1;
            do {
                len++;
            } while ((*to++ = *p++));
            to[-1] = ':';
        }
    }
    for (int i = 0; i < 10; i++)
        to[i] = ' ';
    len += 10;
    long_to_dec(to, static_cast<int32_t>(disk_free(info)), -1, false);
    poke_w(s, len);
    idata().HiChaine = address(s) + 2 + ((len + 1u) & ~1u);
    return address(s);
}

// =PORT(channel): the next byte of a port, or -1 if none is waiting
extern "C" uint32_t lib_fn_port(uint32_t n)
{
    Channel &c = open_channel(n);
    if (!(c.type() & ChanPort))
        type_mismatch();
    if (!hal::wait_for_char(c.handle(), 50))
        return -1u;
    uint8_t *b = idata().DeFloat;
    if (hal::read(c.handle(), b, 1) < 0)
        disk_error();
    return b[0];
}

// OPEN PORT / OPEN OUT / OPEN IN channel,a$
extern "C" void lib_in_open_port(uint32_t n, const uint8_t *name)
{
    open_typed(n, name, ModeOldFile, ChanOut | ChanIn | ChanPort);
}

extern "C" void lib_in_open_out(uint32_t n, const uint8_t *name) { open_typed(n, name, ModeNewFile, ChanOut); }
extern "C" void lib_in_open_in(uint32_t n, const uint8_t *name) { open_typed(n, name, ModeOldFile, ChanIn); }

// OPEN RANDOM channel,a$ / APPEND channel,a$
extern "C" void lib_in_open_random(uint32_t n, const uint8_t *name) { open_random(n, name, ChanRandom); }

extern "C" void lib_in_append(uint32_t n, const uint8_t *name)
{
    BPTR h = open_random(n, name, ChanOut);
    hal::seek(h, 0, OffsetEnd);
}

// =LOF(channel), =POF(channel), POF(channel)=n, =EOF(channel)
extern "C" uint32_t lib_fn_lof(uint32_t n)
{
    BPTR h = file_channel(n).handle();
    int32_t pos = hal::seek(h, 0, OffsetEnd);
    return hal::seek(h, pos, OffsetBeginning);
}

extern "C" void lib_in_pof(uint32_t n, int32_t pos)
{
    BPTR h = file_channel(n).handle();
    if (pos < 0)
        fon_call();
    if (hal::seek(h, pos, OffsetBeginning) < 0)
        disk_error();
}

extern "C" uint32_t lib_fn_pof(uint32_t n) { return hal::seek(file_channel(n).handle(), 0, OffsetCurrent); }

extern "C" uint32_t lib_fn_eof(uint32_t n)
{
    BPTR h = file_channel(n).handle();
    uint32_t pos = hal::seek(h, 0, OffsetEnd);
    uint32_t end = hal::seek(h, pos, OffsetBeginning);
    return pos < end ? 0 : -1u;
}

// CLOSE channel, CLOSE
extern "C" void lib_in_close1(uint32_t n) { close_channel(open_channel(n)); }
extern "C" void lib_in_close0() { close_all(); }

// GET channel,record: read the fields
extern "C" void lib_in_get(uint32_t n, uint32_t record)
{
    Channel *c = &seek_record(n, record);
    uint8_t *fields = ptr<uint8_t>(c->fields());
    uint8_t *field = fields + 8;
    for (uint16_t i = peek_w(fields); i; i--, field += 6) {
        uint16_t len = peek_w(field);
        Regs r = {};
        r.d[3] = len;
        lib_call(L_DDemande, r);
        uint8_t *s = ptr<uint8_t>(r.a[0]);
        poke_l(ptr<void>(peek_l(field + 2)), address(s));
        poke_w(s, len);
        idata().HiChaine = (address(s) + 2 + len + 1) & ~1u;
        if (static_cast<uint32_t>(hal::read(c->handle(), s + 2, len)) != len)
            disk_error();
    }
}

// PUT channel,record: write the fields, padded with spaces
extern "C" void lib_in_put(uint32_t n, uint32_t record)
{
    Channel *c = &seek_record(n, record);
    uint8_t *fields = ptr<uint8_t>(c->fields());
    uint8_t *field = fields + 8;
    for (uint16_t i = peek_w(fields); i; i--, field += 6) {
        uint16_t len = peek_w(field);
        const uint8_t *s = ptr<uint8_t>(peek_l(ptr<void>(peek_l(field + 2))));
        uint16_t slen = str_len(s) < len ? str_len(s) : len;
        if (static_cast<uint32_t>(hal::write(c->handle(), const_cast<uint8_t *>(s + 2), slen)) != slen)
            disk_error();
        if (slen < len) {
            uint16_t pad = len - slen;
            Regs r = {};
            r.d[3] = pad;
            lib_call(L_DDemande, r);
            uint8_t *spaces = ptr<uint8_t>(r.a[0]);
            for (uint16_t k = 0; k < pad; k++)
                spaces[k] = ' ';
            if (static_cast<uint32_t>(hal::write(c->handle(), spaces, pad)) != pad)
                disk_error();
        }
    }
    uint32_t pos = hal::seek(c->handle(), 0, OffsetCurrent);
    if (pos > peek_l(fields + 4))
        poke_l(fields + 4, pos);
}

// =EXIST(a$)
extern "C" uint32_t lib_fn_exist(const uint8_t *name)
{
    if (!str_len(name))
        return 0;
    name_disc(name);
    return exists() ? -1u : 0;
}

// PLOAD "file",bank: the code hunk of a program into a bank (in chip
// memory if the number is negative)
extern "C" void lib_in_pload(const uint8_t *name, int32_t number)
{
    if (!number)
        fon_call();
    bool chip = number < 0;
    if (chip)
        number = -number;
    name_disc(name);
    if (!file_open(ModeOldFile))
        disk_error();
    if (read_header_long() != 0x3F3)
        error(79 + 16);
    while (read_header_long() != 0x3E9)
        ;
    uint32_t length = read_header_long() << 2;
    uint8_t *bank = bank_reserve(number, BankData | (chip ? BankChip : 0), length,
                                 static_cast<const char *>(lib_address(L_BkAsm)));
    bank_changed();
    if (!bank)
        error(24);
    if (!file_read(bank, length))
        disk_error();
    file_close();
}

// BLOAD "file",address/bank
extern "C" void lib_in_bload(const uint8_t *name, int32_t where)
{
    uint32_t to = bank_or_address(where);
    name_disc(name);
    if (!file_open(ModeOldFile))
        disk_error();
    if (file_seek(0, OffsetEnd) < 0)
        disk_error();
    int32_t length = file_seek(0, OffsetBeginning);
    if (length < 0)
        disk_error();
    if (!file_read(ptr<void>(to), length))
        disk_error();
    file_close();
}

// BSAVE "file",start TO end
extern "C" void lib_in_bsave(const uint8_t *name, int32_t start, uint32_t end)
{
    uint32_t from = bank_or_address(start);
    if (end <= from)
        fon_call();
    name_disc(name);
    if (!file_open(ModeNewFile))
        disk_error();
    if (!file_write(ptr<void>(from), end - from))
        disk_error();
    file_close();
}
