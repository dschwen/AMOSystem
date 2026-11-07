// Directories: the list of files the Dir instructions, Dir First$ and the
// file selector read (Fill* in +Lib.s), and the Dir instructions

#include "core/amiga.h"
#include "lib/convert.h"
#include "lib/dos.h"
#include "lib/strings.h"

#include <dos/dos.h>

#include "hal/files.h"
#include "hal/system.h"

// The record that frees the list when the program is cleared (dos_glue.s)

// files.cpp, dos.cpp
extern "C" uint32_t lib_lock_get(amos::RegFrame &r);
extern "C" uint32_t lib_dsk_dnom(amos::RegFrame &r);
extern "C" uint32_t lib_lock_free(amos::RegFrame &r);

using namespace amos;

namespace {

// The list (BufFillF): elements of
//   +0 next, +4 the length of the element (word), +6 the file size, or -1
//   +10 "*" for a directory, else " ", then the name
// The first element holds the name of the directory instead.
constexpr int FillName = 10;

uint8_t upper(uint8_t c) { return c >= 'a' && c <= 'z' ? c - 0x20 : c; }

// The sort key of a name character: upper case, and "*" (directories)
// before anything else
uint8_t sort_key(uint8_t c)
{
    c = upper(c);
    return c == '*' ? 1 : c;
}

// FfComp2: true (BEQ) if b sorts before or with a
bool sorts_before(const uint8_t *a, const uint8_t *b)
{
    for (;;) {
        uint8_t kb = sort_key(*b++), ka = sort_key(*a++);
        if (kb > ka)
            return false;
        if (kb != ka || !ka)
            return true;
    }
}

uint8_t *mem_fast_clear(uint32_t length)
{
    Regs r = {};
    r.d[0] = length;
    r.a[0] = w::reserve(r.d[0], w::FastClear);
    return ptr<uint8_t>(r.a[0]);
}

void lock_get()
{
    RegFrame r = {};
    lib_lock_get(r);
}

void lock_free()
{
    RegFrame r = {};
    lib_lock_free(r);
}

// Joker: does the name match the pattern? A pattern is a list of
// alternatives separated by "/", made of letters (any case), "?" (one
// character but "."), "." and "*" (up to the next "." of the name, or its
// end; "**" matches anything).
bool joker(const char *pattern, const char *name)
{
    const char *alternative = pattern;
    const char *p = pattern, *n = name;
    char c, e;
next_char:
    c = *n++;
    if (!c)
        goto end_of_name;
pattern_char:
    e = *p++;
    if (!e)
        return false;
    switch (e) {
    case '/':
        goto next_alternative;
    case '?':
        if (c != '.')
            goto next_char;
        goto next_alternative;
    case '.':
        if (c == '.')
            goto next_char;
        goto next_alternative;
    case '*':
        if (*p == '*')
            return true;
        for (;;) {
            if (c == '.')
                goto pattern_char;
            c = *n++;
            if (!c)
                goto end_of_name;
        }
    default:
        if (upper(static_cast<uint8_t>(e)) == upper(static_cast<uint8_t>(c)))
            goto next_char;
        goto next_alternative;
    }
end_of_name:
    e = *p++;
    if (!e || e == '/')
        return true;
next_alternative:
    p = alternative;
    n = name;
    do {
        e = *p++;
        if (!e)
            return false;
    } while (e != '/');
    alternative = p;
    goto next_char;
}

// FillFPoke: a new element for the entry of fib (a FileInfoBlock in
// Buffer: size at 124, name at 8), sorted in if FillFSorted. Returns it.
uint8_t *fill_poke(const uint8_t *fib, char first)
{
    IData &d = idata();
    d.FillFPosPoke = 0xFFFF;
    uint16_t size = d.FillFSize;
    if (!d.FillF32) {
        const uint8_t *p = fib + 8;
        while (*p++)
            ;
        size = static_cast<uint16_t>(p - fib);
    }
    uint16_t length = size + 4 + 2 + 4 + 2;
    uint8_t *e = mem_fast_clear(length);
    if (!e)
        return nullptr;
    poke_w(e + 4, length);
    poke_l(e + 6, peek_l(fib + 124));
    uint8_t *to = e + FillName;
    *to++ = static_cast<uint8_t>(first);
    const uint8_t *from = fib + 8;
    int16_t left = static_cast<int16_t>(d.FillFSize - 2);
    bool ended = false;
    for (; left >= 0; left--) {
        if (!(*to++ = *from++)) {
            ended = true;
            break;
        }
    }
    if (ended && d.FillF32) {
        to--;
        for (; left >= 0; left--)
            *to++ = ' ';
    }
    d.FillFNb++;
    if (!d.BufFillF) {
        d.BufFillF = address(e);
        return e;
    }
    uint8_t *after = ptr<uint8_t>(d.BufFillF);
    int16_t position = -1;
    if (d.FillFSorted) {
        for (;;) {
            position++;
            uint8_t *next = ptr<uint8_t>(peek_l(after));
            if (!next || sorts_before(next + FillName, e + FillName))
                break;
            after = next;
        }
    } else {
        while (uint32_t next = peek_l(after))
            after = ptr<uint8_t>(next);
    }
    poke_l(e, peek_l(after));
    poke_l(after, address(e));
    d.FillFPosPoke = static_cast<uint16_t>(position);
    return e;
}

// FillFFree
void fill_free()
{
    lock_free();
    IData &d = idata();
    uint32_t e = d.BufFillF;
    while (e) {
        uint32_t next = peek_l(ptr<void>(e));
        Regs r = {};
        r.a[1] = e;
        r.d[0] = static_cast<int16_t>(peek_w(ptr<uint8_t>(e) + 4));
        w::release(r.a[1], r.d[0]);
        e = next;
    }
    if (d.BufFillF) {
        d.BufFillF = 0;
        d.FillFNb = 0;
        d.PosFillF = 0;
        d.FillFSorted = 0;
    }
}

// FillFirst: start reading the directory in Name1 (if it names a file, it
// is the filter in Name2). The first element is the directory's name.
void fill_first()
{
    fill_free();
    Regs r = {};
    r.a[1] = address(AMOS_DATA(lib_fill_free_on_clear));
    r.a[2] = address(&idata().Sys_ClearRoutines);
    w::add_routine(r.a[1], r.a[2]);
    IData &d = idata();
    lock_get();
    auto fib = ptr<struct FileInfoBlock>(d.Buffer);
    hal::examine(static_cast<BPTR>(d.LockSave), fib);
    if (static_cast<int16_t>(peek_w(ptr<uint8_t>(d.Buffer) + 4)) < 0) {
        // a file: its name becomes the filter. (The assembler found the
        // name with a register NomDir had left, which pointed at the wrong
        // place once Dsk.PathIt had put the current directory in front.)
        RegFrame f = {};
        f.a[0] = d.Name1;
        lib_dsk_dnom(f);
        char *name = ptr<char>(f.a[0]);
        char *to = ptr<char>(d.Name2);
        for (const char *p = name; (*to++ = *p++);)
            ;
        *name = 0;
    }
    lock_get();
    Regs a = {};
    a.d[0] = d.LockSave;
    boot::ask_dir2(a.d[0]);
    d.LockSave = 0;
    uint8_t *e = mem_fast_clear(6 + 4 + 128);
    if (!e)
        error(24);
    d.BufFillF = address(e);
    poke_w(e + 4, 6 + 4 + 128);
    const char *from = ptr<char>(d.Buffer) + 384;
    char *to = reinterpret_cast<char *>(e + FillName);
    while ((*to++ = *from++))
        ;
    lock_get();
    hal::examine(static_cast<BPTR>(d.LockSave), fib);
}

// FillNxt: the next entry. Returns 2 (with *element) if it was added, 1
// if the filters left it out, 0 at the end, -1 on error.
int32_t fill_next(uint8_t **element)
{
    IData &d = idata();
    uint8_t *fib = ptr<uint8_t>(d.Buffer);
    if (hal::examine_next(static_cast<BPTR>(d.LockSave), ptr<struct FileInfoBlock>(d.Buffer))) {
        char first = '*';
        if (static_cast<int16_t>(peek_w(fib + 4)) < 0) {
            const char *name = reinterpret_cast<const char *>(fib + 8);
            const char *negative = ptr<char>(d.DirFNeg);
            if (*negative && joker(negative, name))
                return 1;
            const char *positive = ptr<char>(d.Name2);
            if (*positive && !joker(positive, name))
                return 1;
            first = ' ';
        }
        if (uint8_t *e = fill_poke(fib, first)) {
            *element = e;
            return 2;
        }
    }
    return hal::io_error() == 232 ? 0 : -1;
}

// FillSort: bubble sort of the list after its first element
void fill_sort()
{
    IData &d = idata();
    if (!d.BufFillF)
        return;
    bool swapped;
    do {
        swapped = false;
        be_u32 *link = ptr<be_u32>(d.BufFillF);
        uint8_t *a = ptr<uint8_t>(*link);
        if (!a)
            return;
        // a is the first entry; compare it with the one after it
        while (uint32_t bn = peek_l(a)) {
            uint8_t *b = ptr<uint8_t>(bn);
            const uint8_t *pa = a + FillName, *pb = b + FillName;
            bool exchange = false;
            for (;;) {
                uint8_t kb = sort_key(*pb++), ka = sort_key(*pa++);
                if (!(ka | kb))
                    break;
                if (ka != kb) {
                    exchange = kb < ka;
                    break;
                }
            }
            if (exchange) {
                poke_l(a, peek_l(b));
                poke_l(b, address(a));
                *link = address(b);
                swapped = true;
                link = reinterpret_cast<be_u32 *>(b);
            } else {
                link = reinterpret_cast<be_u32 *>(a);
                a = b;
            }
        }
    } while (swapped);
}

// FillGet: element n of the list (0: the directory), or null
uint8_t *fill_get(uint16_t n)
{
    uint32_t e = idata().BufFillF;
    if (!e)
        return nullptr;
    for (int16_t i = -1;;) {
        e = peek_l(ptr<void>(e));
        if (!e)
            return nullptr;
        if (++i == static_cast<int16_t>(n))
            return ptr<uint8_t>(e);
    }
}

// FillDev: the list of the devices (filter "D...") or assigns ("A..."),
// or both
void fill_devices()
{
    fill_free();
    IData &d = idata();
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    poke_w(buffer + 8, 0);
    poke_l(buffer + 124, 0);
    fill_poke(buffer, 0);
    d.FillFNb = 0;
    struct Fill {
        IData &d;
        uint8_t *buffer;
    } fill = {d, buffer};
    hal::each_device(
        [](const char *name, bool device, void *data) {
            Fill &f = *static_cast<Fill *>(data);
            uint8_t kind = upper(*ptr<uint8_t>(f.d.Name1));
            if ((kind == 'D' && !device) || (kind == 'A' && device) || !*name)
                return;
            uint8_t *to = f.buffer + 8;
            while (*name)
                *to++ = uint8_t(*name++);
            *to++ = ':';
            *to = 0;
            const char *filter = ptr<char>(f.d.Name2);
            if (*filter && !joker(filter, reinterpret_cast<char *>(f.buffer + 8)))
                return;
            poke_l(f.buffer + 124, 0xFFFFFFFFu);
            fill_poke(f.buffer, ' ');
        },
        &fill);
    fill_sort();
}

// NomDir: the AMOS string s as a directory in Name1, and the filter (the
// last part, if it has * or ?) in Name2
void nom_dir(const uint8_t *s)
{
    uint16_t len = str_len(s);
    if (len >= 256)
        error(21);
    IData &d = idata();
    char *name = ptr<char>(d.Name1);
    char *filter = ptr<char>(d.Name2);
    *name = *filter = 0;
    char *to = name, *part = name;
    bool joker_seen = false;
    const uint8_t *from = str_chars(s);
    for (uint16_t i = 0; i < len; i++) {
        char c = static_cast<char>(*from++);
        *to++ = c;
        *to = 0;
        if (c == '*' || c == '?')
            joker_seen = true;
        if (c == ':' || c == '/') {
            if (joker_seen)
                fon_call();
            part = to;
        }
    }
    if (joker_seen) {
        char *f = filter;
        for (const char *p = part; (*f++ = *p++);)
            ;
        *part = 0;
    }
    path_it();
}

void print(const char *text)
{
    Regs r = {};
    r.a[0] = address(text);
    lib_call(L_ImpChaine, r);
}

const char *message(uint32_t n)
{
    Regs r = {};
    r.d[0] = n;
    r.a[0] = address(boot::def_message(r.d[0]));
    return ptr<char>(r.a[0]);
}

uint32_t inkey()
{
    Regs t = {};
    lib_call(L_Test_PaSaut, t);
    Regs r = {};
    sy_call(sy::Inkey, r);
    return r.d[1];
}

constexpr uint8_t ScanEscape = 0x45;

// TTDir: Escape stops the listing (true), Space pauses it until the next
// key (Escape then stops it), Control slows it down
bool stop_listing()
{
    if (uint32_t key = inkey()) {
        if (static_cast<uint8_t>(key) == ' ') {
            while (!(key = inkey()))
                ;
            return static_cast<uint8_t>(key >> 16) == ScanEscape;
        }
        if (static_cast<uint8_t>(key >> 16) == ScanEscape)
            return true;
    }
    Regs s = {};
    sy_call(sy::Shifts, s);
    if (s.d[1] & 8) {
        for (int i = 25; i; i--) {
            Regs w = {};
            lib_call(L_Test_PaSaut, w);
            Regs v = {};
            sy_call(sy::WaitVbl, v);
        }
    }
    return false;
}

// Dir3: list the directory in Name1 (filter Name2) on the screen or the
// printer
void list_directory()
{
    IData &d = idata();
    d.FillF32 = 1;
    d.DirLong = 0;
    fill_first();
    print(message(17));
    print(ptr<char>(d.BufFillF) + FillName);
    print("\r\n");
    for (;;) {
        uint8_t *e;
        int32_t got = fill_next(&e);
        if (got < 0)
            disk_error();
        if (!got)
            break;
        if (got == 1)
            continue;
        uint8_t *entry = e + 6;
        if (d.DirComp) {
            print(reinterpret_cast<char *>(entry + 4));
        } else {
            if (entry[4] == '*') {
                print(message(16));
                print(reinterpret_cast<char *>(entry + 5));
            } else {
                print("  ");
                print(reinterpret_cast<char *>(entry + 5));
                print(" ");
                uint32_t size = peek_l(entry) & 0x00FFFFFF;
                d.DirLong += size;
                char *end = long_to_dec(ptr<char>(d.Name1), static_cast<int32_t>(size), -1, false);
                *end = 0;
                print(ptr<char>(d.Name1));
            }
            print("\r\n");
        }
        if (stop_listing())
            break;
    }
    if (d.DirComp) {
        print("\r\n");
    } else {
        char *end = long_to_dec(ptr<char>(d.Buffer), static_cast<int32_t>(d.DirLong), -1, false);
        *end = 0;
        print(ptr<char>(d.Buffer));
        print(message(19));
    }
    fill_free();
}

void dir_here()
{
    *ptr<char>(idata().Name1) = 0;
    *ptr<char>(idata().Name2) = 0;
    path_it();
}

// Dir2 / DirW2: the width of the names
void dir_normal()
{
    idata().DirComp = 0;
    idata().FillFSize = idata().DirLNom;
    list_directory();
}

void dir_wide()
{
    idata().DirComp = 1;
    if (!idata().ScOnAd)
        screen_error(3);
    auto screen = ptr<Screen>(idata().ScOnAd);
    idata().FillFSize = ptr<Window>(screen->EcWindow)->WiTx >> 1;
    list_directory();
}

// Dir Next$: the next name of the list, padded, with its size
uint32_t fill_next_string()
{
    IData &d = idata();
    uint8_t *e = fill_get(d.PosFillF);
    if (!e) {
        fill_free();
        return str_empty();
    }
    uint8_t *entry = e + 6;
    d.PosFillF++;
    int16_t size = static_cast<int16_t>(d.FillFSize);
    uint32_t len = static_cast<uint32_t>(static_cast<int32_t>(size)) + 8;
    Regs r = {};
    r.d[3] = len;
    lib_call(L_DDemande, r);
    uint8_t *s = ptr<uint8_t>(r.a[0]);
    poke_w(s, static_cast<uint16_t>(len));
    d.HiChaine = address(s) + 2 + static_cast<uint16_t>(len);
    uint8_t *to = s + 2;
    const uint8_t *from = entry + 4;
    for (int16_t i = 0; i < size; i++)
        *to++ = *from++;
    for (int i = 0; i < 8; i++)
        to[i] = ' ';
    if (entry[4] != '*' && static_cast<int32_t>(peek_l(entry)) >= 0)
        long_to_dec(reinterpret_cast<char *>(to), static_cast<int32_t>(peek_l(entry)), -1, false);
    return address(s);
}

void check_no_monitor()
{
    if (idata().Patch_Errors.load())
        fon_call();
}

} // namespace

// ---- register entries ---------------------------------------------------

extern "C" uint32_t lib_fill_all(RegFrame &)
{
    fill_first();
    uint8_t *e;
    while (fill_next(&e) > 0)
        ;
    return 0;
}

// FillGet: d0 = number. Out: a0 = element, d0 = -1 (BNE) or 0.
extern "C" uint32_t lib_fill_get(RegFrame &r)
{
    uint8_t *e = fill_get(static_cast<uint16_t>(r.d[0]));
    if (e)
        r.a[0] = address(e);
    return r.d[0] = e ? -1u : 0;
}

extern "C" uint32_t lib_fill_dev(RegFrame &)
{
    fill_devices();
    return 0;
}

extern "C" uint32_t lib_fill_first(RegFrame &)
{
    fill_first();
    return 0;
}

// FillNxt: out: d0 (flags), a0 = the new element if d0 = 2
extern "C" uint32_t lib_fill_nxt(RegFrame &r)
{
    uint8_t *e = nullptr;
    int32_t got = fill_next(&e);
    if (got == 2)
        r.a[0] = address(e);
    return r.d[0] = static_cast<uint32_t>(got);
}

extern "C" uint32_t lib_fill_ffree(RegFrame &)
{
    fill_free();
    return 0;
}

extern "C" uint32_t lib_fill_sort(RegFrame &)
{
    fill_sort();
    return 0;
}

// FillFPoke: a2 = FileInfoBlock, d2 = first character. Out: d0 = element
// or 0.
extern "C" uint32_t lib_fill_fpoke(RegFrame &r)
{
    return r.d[0] = address(fill_poke(ptr<uint8_t>(r.a[2]), static_cast<char>(r.d[2])));
}

// FfComp: a2, a3 = elements; FfComp2: a0, a1 = names. BEQ if the second
// sorts before or with the first. (a3 is not in the frame: ctx() has it.)
extern "C" uint32_t lib_ff_comp(RegFrame &r)
{
    r.a[0] = r.a[2] + FillName;
    r.a[1] = encode_context_cursor(ctx().a3) + FillName;
    return !sorts_before(ptr<uint8_t>(r.a[0]), ptr<uint8_t>(r.a[1]));
}

extern "C" uint32_t lib_ff_comp2(RegFrame &r) { return !sorts_before(ptr<uint8_t>(r.a[0]), ptr<uint8_t>(r.a[1])); }

// FillFFind: a0 = name, d0 = where to start. Out: d0 = its position or -1.
extern "C" uint32_t lib_fill_ffind(RegFrame &r)
{
    const uint8_t *name = ptr<uint8_t>(r.a[0]);
    uint16_t start = static_cast<uint16_t>(r.d[0]);
    IData &d = idata();
    int32_t pos = 0;
    uint32_t e = d.BufFillF;
    if (e && (e = peek_l(ptr<void>(e)))) {
        for (;;) {
            if (static_cast<uint16_t>(pos) >= start)
                break;
            pos++;
            e = peek_l(ptr<void>(e));
            if (!e) {
                pos = -1;
                break;
            }
        }
        if (pos >= 0) {
            for (;;) {
                const uint8_t *entry = ptr<uint8_t>(e) + FillName;
                if (d.FillFSorted) {
                    // the first entry after the name
                    if (!sorts_before(name, entry))
                        break;
                } else {
                    const uint8_t *a = name, *b = entry;
                    bool found = false;
                    for (;;) {
                        uint8_t kb = sort_key(*b++);
                        uint8_t ca = *a++;
                        if (!ca) {
                            found = true;
                            break;
                        }
                        if (sort_key(ca) != kb)
                            break;
                    }
                    if (found)
                        break;
                }
                pos++;
                e = peek_l(ptr<void>(e));
                if (!e) {
                    pos = -1;
                    break;
                }
            }
        }
    }
    return r.d[0] = static_cast<uint32_t>(pos);
}

// NomDir: a2 = AMOS string
extern "C" uint32_t lib_nom_dir(RegFrame &r)
{
    nom_dir(ptr<uint8_t>(r.a[2]));
    return 0;
}

// Joker: a0 = pattern, a1 = name. Out: d0 = 1 if it matches.
extern "C" uint32_t lib_joker(RegFrame &r) { return r.d[0] = joker(ptr<char>(r.a[0]), ptr<char>(r.a[1])); }

// The code of the clear record
extern "C" void lib_fill_free() { fill_free(); }

// ---- instructions -------------------------------------------------------

// DIR, LDIR [a$], DIR/W, LDIR/W [a$]
extern "C" void lib_in_dir0()
{
    idata().ImpFlg = 0;
    dir_here();
    dir_normal();
}

extern "C" void lib_in_ldir0()
{
    idata().ImpFlg = 1;
    dir_here();
    dir_normal();
}

extern "C" void lib_in_dir1(const uint8_t *name)
{
    idata().ImpFlg = 0;
    nom_dir(name);
    dir_normal();
}

extern "C" void lib_in_ldir1(const uint8_t *name)
{
    idata().ImpFlg = 1;
    nom_dir(name);
    dir_normal();
}

extern "C" void lib_in_dirw0()
{
    idata().ImpFlg = 0;
    dir_here();
    dir_wide();
}

extern "C" void lib_in_ldirw0()
{
    idata().ImpFlg = 1;
    dir_here();
    dir_wide();
}

extern "C" void lib_in_dirw1(const uint8_t *name)
{
    idata().ImpFlg = 0;
    nom_dir(name);
    dir_wide();
}

extern "C" void lib_in_ldirw1(const uint8_t *name)
{
    idata().ImpFlg = 1;
    nom_dir(name);
    dir_wide();
}

// SET DIR length[,filter$]: the width of the names, and the files to leave
// out
extern "C" void lib_in_set_dir1(uint32_t length)
{
    if (length == 0x80000000u)
        return;
    length &= ~1u;
    if (!length || length >= 106)
        fon_call();
    idata().DirLNom = static_cast<uint16_t>(length);
}

extern "C" void lib_in_set_dir2(uint32_t length, const uint8_t *filter)
{
    if (address(filter) != 0x80000000u) {
        uint16_t len = str_len(filter);
        if (len >= 106)
            error(21);
        char *to = ptr<char>(idata().DirFNeg);
        const uint8_t *from = str_chars(filter);
        if (len > 510)
            len = 510;
        for (uint16_t i = 0; i < len; i++)
            *to++ = static_cast<char>(*from++);
        *to = 0;
    }
    lib_in_set_dir1(length);
}

// =DIR FIRST$(a$), =DIR NEXT$
extern "C" uint32_t lib_fn_dir_first(const uint8_t *name)
{
    check_no_monitor();
    nom_dir(name);
    lock_get();
    IData &d = idata();
    d.FillF32 = 1;
    d.DirComp = 0;
    d.FillFSize = d.DirLNom;
    RegFrame r = {};
    lib_fill_all(r);
    fill_sort();
    check_no_monitor();
    return fill_next_string();
}

extern "C" uint32_t lib_fn_fill_next()
{
    check_no_monitor();
    return fill_next_string();
}

// =DEV FIRST$(a$), =PRG FIRST$(a$) (which lists the devices too)
extern "C" uint32_t lib_fn_dev_first(const uint8_t *filter)
{
    check_no_monitor();
    nom_dir(filter);
    IData &d = idata();
    d.FillF32 = 1;
    d.DirComp = 0;
    d.FillFSize = d.DirLNom;
    // (both the devices and the assigns: the original took the first letter
    // of the current directory, "D" or "A", for the kind wanted)
    *ptr<char>(d.Name1) = 0;
    fill_devices();
    check_no_monitor();
    return fill_next_string();
}

// ---- ASSIGN "name:" To "path" ------------------------------------------

namespace {

void lib(int routine)
{
    Regs r;
    lib_call(routine, r);
}

// The string s into to, zero-terminated (ChVerBuf2: at most 510)
void to_text(char *to, const uint8_t *s)
{
    Regs r;
    r.a[0] = address(to);
    r.a[2] = address(s + 2);
    r.d[2] = peek_w(s);
    lib_call(L_ChVerBuf2, r);
}

// Workbench 2.0: AssignLock with a lock on the path (none: the assign
// removed)
void assign_lock(const uint8_t *name, const uint8_t *path)
{
    IData &d = idata();
    lib(L_LockFree);
    if (uint16_t n = peek_w(path)) {
        if (n >= 108)
            fon_call();
        to_text(ptr<char>(d.Name1), path);
        path_it();
        lib(L_LockGet);
    }
    uint16_t n = peek_w(name);
    if (!n || n >= 108)
        fon_call();
    char *b = ptr<char>(d.Buffer);
    to_text(b, name);
    char *end = b;
    while (*end)
        end++;
    if (end[-1] != ':')
        fon_call();
    end[-1] = 0;
    if (!hal::assign_lock(b, BPTR(d.LockSave))) {
        lib(L_LockFree);
        disk_error();
    }
    d.LockSave = 0;
}

// Workbench 1.3: the command c:assign, output to NIL:
void assign_command(const uint8_t *name, const uint8_t *path)
{
    IData &d = idata();
    static const char command[] = "c:assign";
    char *n1 = ptr<char>(d.Name1);
    for (int i = 0; (n1[i] = command[i]); i++)
        ;
    Regs e;
    lib_call(L_RExist, e);
    if (!e.d[3])
        disk_error();
    *n1 = 0;
    if (uint16_t n = peek_w(path)) {
        if (n >= 108)
            fon_call();
        to_text(n1, path);
        path_it();
        lib(L_LockGet);
        lib(L_LockFree);
    }
    char *b = ptr<char>(d.Buffer);
    char *to = b;
    for (const char *c = command; *c; c++)
        *to++ = *c;
    *to++ = ' ';
    uint16_t n = peek_w(name);
    if (!n || n >= 64)
        fon_call();
    const uint8_t *s = name + 2;
    for (;;) {
        uint8_t c = *s++;
        *to++ = char(c);
        if (c <= 32)
            fon_call();
        if (c == ':')
            break;
        if (!--n)
            fon_call();
    }
    *to++ = ' ';
    while ((*to++ = *n1++))
        ;
    Regs o;
    o.d[1] = address("NIL:");
    o.d[2] = 1005;
    lib_call(L_D_OpenD1, o);
    if (!d.Handle)
        disk_error();
    BPTR nil = BPTR(d.Handle);
    int32_t ok = hal::execute(b, nil, nil);
    lib(L_D_Close);
    if (!ok)
        disk_error();
}

} // namespace

extern "C" void lib_in_assign(const uint8_t *name, const uint8_t *path)
{
    if (idata().WB2_0)
        assign_lock(name, path);
    else
        assign_command(name, path);
}

#if !defined(__mc68000__)
#include "runtime/bindings/lib_dirs.inc"
#endif
