// Memory banks and the lists they live in.
//
// A list is a pointer to its first element; an element starts with the
// pointer to the next one and the length asked for, then its data (Lst.*
// in +B.s). A bank is such an element with
//   +8  number (long), +12 flags (word), +14 0, +16 name (8 bytes),
//   +24 the data: the bank's address.
// A sprite or icon bank's data is a word count, count entries of two
// longs (image, mask) and a palette of 32 colours.
#pragma once

#include "core/amos.h"
#include "runtime/detail/identity_data_memory.h"

namespace amos {

// Bank flags (Bnk_Bit*)
constexpr uint16_t BankData = 1 << 0;   // kept by Erase Temp
constexpr uint16_t BankChip = 1 << 1;
constexpr uint16_t BankBobs = 1 << 2;
constexpr uint16_t BankIcons = 1 << 3;

// Banks, lists and their elements are guest cursors (null when there is none): a bank is the
// address of its data, a list the address of the cell holding its first element's address.
// Code that has to hand a bank's memory to the host (a file read into it) takes bank_host_view.
inline uint8_t *bank_host_view(ContextCursor at) { return ptr<uint8_t>(encode_context_cursor(at)); }

// Lists (Lst.*)
ContextCursor lst_cree(ContextCursor list, uint32_t length, uint32_t memflags);
ContextCursor lst_new(ContextCursor list, uint32_t length);
void lst_del(ContextCursor list, ContextCursor element);
void lst_del_all(ContextCursor list);
void lst_insert(ContextCursor list, ContextCursor element);
void lst_remove(ContextCursor list, ContextCursor element);

// The banks of the current program (Cur_Banks)
inline ContextCursor bank_list() { return decode_context_cursor(idata().Cur_Banks); }

inline uint32_t bank_number(ContextCursor bank) { return peek_l(bank - 16); }
inline uint16_t bank_flags(ContextCursor bank) { return peek_w(bank - 12); }
// The size of the data
inline uint32_t bank_size(ContextCursor bank) { return peek_l(bank - 20) - 16; }

// Bnk.GetAdr: the bank with this number, or null
ContextCursor bank_find(uint32_t number);
// Bnk.GetBobs, Bnk.GetIcons: banks 1 and 2 if they hold sprites / icons
ContextCursor bank_bobs();
ContextCursor bank_icons();
// The length =Length() gives: the data size, or the number of images
uint32_t bank_length(ContextCursor bank);

void bank_erase(uint32_t number);       // Bnk.Eff
void bank_erase_at(ContextCursor bank);      // Bnk.EffA0
void bank_erase_all();                  // Bnk.EffAll
void bank_erase_temp();                 // Bnk.EffTemp
void bank_changed();                    // Bnk.Change
// Bnk.Reserve: a new bank (replacing one with that number), or null
ContextCursor bank_reserve(uint32_t number, uint16_t flags, uint32_t length, const char *name);
// Bnk.OrAdr: a bank number below 1024 gives the bank's address (error
// "bank not reserved" if there is none), anything else is an address
uint32_t bank_or_address(int32_t value);

// Bnk.PrevProgram: switch to the banks of the program below the current
// one; false (and no switch) if there is none. Bnk.CurProgram switches
// back.
bool banks_previous_program();
void banks_current_program();

} // namespace amos
