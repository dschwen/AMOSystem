// The main program (+B.s): the start and the end of AMOSPro, the loading
// of the libraries and extensions, of the editor and the monitor, and the
// system routines the library reaches through AMOSJmps.
#pragma once

#include "core/amos.h"

namespace amos::boot {

// The data zone of a loaded library, below its token table
constexpr int LB_MemAd = -4, LB_MemSize = -8, LB_NRout = -10, LB_Flags = -12, LB_Title = -16,
              LB_Command = -20, LB_Verif = -24, LB_LibSizes = -28, LB_DFloatSwap = -30,
              LB_FFloatSwap = -32;
constexpr int LBF_20 = 2;           // a 2.0 library (a bit of the byte LB_Flags)
constexpr int Libraries = 27;       // the main library, then the extensions 1-26

inline be_u32 &lb_long(uint8_t *tokens, int field) { return *reinterpret_cast<be_u32 *>(tokens + field); }
inline be_u16 &lb_word(uint8_t *tokens, int field) { return *reinterpret_cast<be_u16 *>(tokens + field); }
// The jump table entry of routine n, below the data zone
inline be_u32 &lb_jump(uint8_t *tokens, uint32_t n)
{
    return *reinterpret_cast<be_u32 *>(tokens - LB_Size - 4 - int16_t(uint16_t(n << 2)));
}

// ---- libraries.cpp ---------------------------------------------------------

// 0 if all went well, 1 for a disk error, 2 out of memory or a bad extension
int libraries_load();
void library_patch();
int libraries_reloc();
int libraries_init();
void libraries_stop();
void libraries_free();
void libraries_free_sizes();

// ---- system.cpp ------------------------------------------------------------

// GetMessage: message n of a list of the configuration, "" past its end
// (Sys_GetMessage: of the interpreter's configuration)
const char *message(uint32_t list, uint32_t n);
const char *sys_message(uint32_t n);
// Sys_AddPath: the name into Name1, in the APSystem folder unless it has a
// path. Sys_AddPathCom: the same for the name up to its first space, the
// command line after it; returns the command line.
void sys_add_path(const char *name);
const char *sys_add_path_com(const char *name);

// The memory of the AMOS.library: SyCall MemFastClear..., 0 if out of memory
uint32_t ram_fast(uint32_t size);
uint32_t ram_chip(uint32_t size);
void ram_free(uint32_t address, uint32_t size);
// A5_Reserve / A5_Free: memory at a long of the data zone, its size before it
uint32_t a5_reserve(be_u32 &slot, uint32_t size, uint32_t flags);
void a5_free(be_u32 &slot);

// The system file of +B.s (D_Open...): Handle, without the records that
// close the library's current file on errors
bool sys_open(const char *name, uint32_t mode);
bool sys_read(void *buffer, uint32_t length);
int32_t sys_seek(int32_t offset, int32_t mode);
void sys_close();

void clear_cache();         // Sys_ClearCache
void wait_vbl_multi();      // Sys_WaitMul
void recop();               // ReCop
void ask_dir();             // AskDir: the current directory at Buffer+384
void ask_dir2(uint32_t lock);
void w_option();            // WOption: AMOS to the front, if the WB was asked for
bool installed();           // Sys_VerInstall
void math_close();          // Math_Close

// ---- modules.cpp -----------------------------------------------------------

uint32_t edit_load();       // 0, or an error
void edit_free();
int32_t mon_load();         // 0, -1 out of memory, -2 file not found...
void mon_free();
void erase_mouse();         // EffMouse
void erase_fsel();          // EffFSel
void load_mouse();          // LdMouse
void load_fsel();           // LdFSel

// ---- boot.cpp --------------------------------------------------------------

// Panic messages, shown when AMOSPro stops
extern const char panic_oomem[], panic_editor[], panic_install[], panic_cantread[], panic_badext[],
    panic_apsystem[];
// TheEnd: stops all and leaves AMOSPro, with a panic message or none
[[noreturn]] void the_end(const char *panic);
// -r on the command line: the program run, and AMOSPro left when it stops
// (a player: no editor, no direct mode)
extern bool run_only;
int exit_status();          // 1 if AMOSPro stopped with a message

} // namespace amos::boot
