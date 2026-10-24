// Files: names, the current file and disk errors (Dsk.*, NomDisc, D_*,
// DiskError in +Lib.s)

#include "lib/dos.h"

#include "core/amiga.h"
#include "lib/strings.h"

#define DOS_BASE_NAME (reinterpret_cast<struct DosLibrary *>(amos::idata().DosBase))
#include <proto/dos.h>

// The records that close the current file when the program stops on an
// error or is cleared (Sys_ErrorRoutines / Sys_ClearRoutines), in dos_glue.s
extern "C" uint8_t lib_close_on_error[], lib_close_on_clear[];

namespace amos {

namespace {

char *name1() { return ptr<char>(idata().Name1); }
char *path_act() { return ptr<char>(idata().PathAct); }

char *string_end(char *s)
{
    while (*s)
        s++;
    return s;
}

char *copy(char *to, const char *from)
{
    while ((*to++ = *from++))
        ;
    return to - 1;
}

} // namespace

void add_routine(uint8_t *record, uint32_t list)
{
    Regs r = {};
    r.a[1] = address(record);
    r.a[2] = list;
    w::add_routine(r.a[1], r.a[2]);
}

void path_it()
{
    char *name = name1();
    if (*name == ':') {
        // ":file": the volume of the current directory
        char *rest = string_end(path_act()) + 1;
        copy(rest, name);
        char *to = name;
        for (const char *p = path_act(); *p && *p != ':'; p++)
            *to++ = *p;
        copy(to, rest);
        return;
    }
    for (const char *p = name; *p; p++)
        if (*p == ':')
            return;
    // a relative name: the current directory goes in front (the name
    // waits after the end of the directory)
    char *saved = string_end(path_act()) + 1;
    copy(saved, name);
    char *to = copy(name, path_act());
    copy(to, saved);
}

char *name_disc(const uint8_t *s)
{
    uint16_t len = str_len(s);
    if (!len || len >= 108)
        fon_call();
    char *to = name1();
    const uint8_t *from = str_chars(s);
    for (uint16_t i = 0; i < len; i++)
        *to++ = static_cast<char>(*from++);
    *to = 0;
    path_it();
    return name1();
}

uint32_t file_open(const char *name, uint32_t mode)
{
    IData &d = idata();
    d.Handle = static_cast<uint32_t>(Open(name, mode));
    add_routine(lib_close_on_error, address(&d.Sys_ErrorRoutines));
    add_routine(lib_close_on_clear, address(&d.Sys_ClearRoutines));
    return d.Handle;
}

uint32_t file_open(uint32_t mode) { return file_open(name1(), mode); }

void file_close()
{
    IData &d = idata();
    if (BPTR handle = static_cast<BPTR>(d.Handle)) {
        d.Handle = 0;
        Close(handle);
    }
}

bool file_read(void *buffer, uint32_t length)
{
    return static_cast<uint32_t>(Read(static_cast<BPTR>(idata().Handle), buffer, length)) == length;
}

bool file_write(const void *buffer, uint32_t length)
{
    return static_cast<uint32_t>(Write(static_cast<BPTR>(idata().Handle), const_cast<void *>(buffer), length)) == length;
}

int32_t file_seek(int32_t offset, int32_t mode)
{
    return Seek(static_cast<BPTR>(idata().Handle), offset, mode);
}

void disk_error() { disk_err(IoErr()); }

void disk_err(int32_t code)
{
    static const int16_t known[] = {203, 204, 205, 210, 213, 214, 216, 218,
                                    220, 221, 222, 223, 224, 225, 226};
    for (unsigned i = 0; i < sizeof known / sizeof known[0]; i++) {
        if (known[i] == static_cast<int16_t>(code)) {
            uint32_t n = DEBase + i;
            if (uint32_t patch = idata().Fs_ErrPatch)
                amos_goto(ptr<void>(patch), n);
            error(n);
        }
    }
    error(DEBase + 15);
}

} // namespace amos

using namespace amos;

// ---- register entries ---------------------------------------------------

// Dsk.DNom: a0 = a file name. Out: a0 = the name after its path.
extern "C" uint32_t lib_dsk_dnom(RegFrame &r)
{
    const char *start = ptr<char>(r.a[0]);
    const char *p = string_end(const_cast<char *>(start));
    char c = 0;
    // the assembler stops one character before the start, as here
    do {
        c = *--p;
    } while (c != '/' && c != ':' && p >= start);
    r.d[0] = (r.d[0] & 0xFFFFFF00u) | static_cast<uint8_t>(c);
    r.a[1] = address(start);
    r.a[0] = address(p + 1);
    return 0;
}

// Dsk.PathIt: Name1 with the current directory. Keeps all registers.
extern "C" uint32_t lib_dsk_path_it(RegFrame &)
{
    path_it();
    return 0;
}

// NomDisc: a2 = AMOS string. Out: Name1 and the current directory; d2 =
// its length, a2 after the length word, a0/a1 as ChVerBuf2 left them.
extern "C" uint32_t lib_nom_disc(RegFrame &r)
{
    const uint8_t *s = ptr<uint8_t>(r.a[2]);
    name_disc(s);
    uint16_t len = str_len(s);
    r.d[2] = (r.d[2] & 0xFFFF0000u) | len;
    r.a[2] = address(s + 2);
    r.a[1] = address(s + 2 + len);
    r.a[0] = idata().Name1 + len + 1;
    return 0;
}

// D_Open: d2 = mode, Name1. D_OpenD1: d1 = name, d2 = mode. Out: d0 =
// Handle (BEQ: failed).
extern "C" uint32_t lib_d_open(RegFrame &r) { return r.d[0] = file_open(r.d[2]); }
extern "C" uint32_t lib_d_open_d1(RegFrame &r) { return r.d[0] = file_open(ptr<char>(r.d[1]), r.d[2]); }

// D_Close. Keeps all registers.
extern "C" uint32_t lib_d_close(RegFrame &)
{
    file_close();
    return 0;
}

// D_Read, D_Write: d2 = buffer, d3 = length. Out: d0 = bytes done; BNE
// if not all of them.
extern "C" uint32_t lib_d_read(RegFrame &r)
{
    r.d[0] = Read(static_cast<BPTR>(idata().Handle), ptr<void>(r.d[2]), r.d[3]);
    return r.d[0] != r.d[3];
}

extern "C" uint32_t lib_d_write(RegFrame &r)
{
    r.d[0] = Write(static_cast<BPTR>(idata().Handle), ptr<void>(r.d[2]), r.d[3]);
    return r.d[0] != r.d[3];
}

// D_Seek: d2 = offset, d3 = mode. Out: d0 = old position (BMI: error).
extern "C" uint32_t lib_d_seek(RegFrame &r)
{
    r.d[1] = idata().Handle;
    return r.d[0] = file_seek(r.d[2], r.d[3]);
}

// DiskError, DiskErr (d0 = DOS error): never return
extern "C" uint32_t lib_disk_error(RegFrame &) { disk_error(); }
extern "C" uint32_t lib_disk_err(RegFrame &r) { disk_err(r.d[0]); }

// The code of the close records
extern "C" void lib_close_current() { file_close(); }
