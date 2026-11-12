// AMCAF: the files (R067-R069, R0ac): a bank loaded from a file (WLOAD a Work bank, DLOAD a Data bank), a
// bank saved (WSAVE, DSAVE), and the DOS error of the last call.

#include "ext/amcaf.h"
#include "ext/third_party.h"
#include "hal/files.h"
#include "lib/banks.h"
#include "lib/dos.h"
#include "lib/strings.h"

using namespace amos;

namespace {

// R16b: the name given (1-128 characters) in Name1 with the current directory (Dsk.PathIt), the buffer
// of the file name for DOS
const char *file_name(GuestString name)
{
    uint16_t length = str_len(name);
    if (uint16_t(length - 1) >= 128)
        fon_call();
    char *to = ptr<char>(idata().Name1);
    name.copy_to(to, 0, length);
    to[length] = 0;
    path_it();
    return to;
}

// R068, R067: a bank of the number (a negative one: chip memory) reserved the size of the file and read
// from it. The errors: 81 the file will not open, 24 no memory for the bank, 94 the read comes short
void load(GuestString name, int32_t number, uint16_t flags, const char *bank_name)
{
    hal::File file = hal::open(file_name(name), hal::ModeOldFile);
    if (!file)
        error(81);
    // the size: the end, then back to the start (the old position)
    hal::seek(file, 0, hal::SeekEnd);
    int32_t size = hal::seek(file, 0, hal::SeekBeginning);
    if (number < 0) {
        number = -number;
        flags |= BankChip;
    }
    ContextCursor bank = bank_reserve(uint32_t(number), flags, uint32_t(size), bank_name);
    if (!bank) {
        hal::close(file);
        error(24);
    }
    int32_t got = hal::read(file, bank_host_view(bank), size);
    hal::close(file);
    if (got != size)
        error(94);
}

// R069: the data of a bank written to a new file. The banks of sprites and icons are not allowed (the
// extension's error 4); the errors 94 the file does not open or the write falls short
void save(GuestString name, int32_t bank)
{
    ContextCursor b = decode_context_cursor(bank_or_address(bank));
    if (bank_flags(b) & (BankBobs | BankIcons))
        ext3::error(ext3::Id::amcaf, 4);
    hal::File file = hal::open(file_name(name), hal::ModeNewFile);
    if (!file)
        error(94);
    int32_t size = int32_t(peek_l(b - 20) - 16);
    int32_t put = hal::write(file, bank_host_view(b), size);
    hal::close(file);
    if (put != size)
        error(94);
}

} // namespace

extern "C" {

// WLOAD name$,bank; DLOAD name$,bank: the file as a Work / Data bank (a negative number: in chip memory)
void amcaf_wload(GuestString name, int32_t bank) { load(name, bank, 0, "Work    "); }
void amcaf_dload(GuestString name, int32_t bank) { load(name, bank, BankData, "Datas   "); }

// WSAVE name$,bank; DSAVE name$,bank: the bank's data in a file
void amcaf_wsave(GuestString name, int32_t bank) { save(name, bank); }

// =IO ERROR: the DOS error of the last call
int32_t amcaf_io_error() { return hal::io_error(); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/ext_amcaf_dos.inc"
#endif
