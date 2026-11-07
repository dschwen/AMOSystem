// Shared by the C++ parts of the verifier (+Verif.s, included by +B.s):
// the tests of a program before it runs. The assembler keeps its place in
// the program in a6; here it is verif::pc, taken from a6 when the
// assembler calls C++ and handed back when C++ calls the assembler.
#pragma once

#include "core/amos.h"
#include "runtime/detail/identity_data_memory.h"

namespace amos::verif {

extern uint8_t *pc;

// The verifier's errors (the message numbers of VerErr)
enum : uint16_t {
    EBadStructure = 1, EUserFunction, ENoBuffer, EDataStart, ENoExtension, ETooManyDirect, EIllegalDirect,
    EVariableBuffer, EGotoLoop, ETooLong, EShared, EAlreadyGlobal, ENotGlobal, ENoParameter, ESharedLine,
    EProcedureTwice, EProcedureOpen, EProcedureNotOpen, EIllegalParameters, EProcedureUndefined, EElseIf,
    EIfElse, EEndIf, EElseEnd, ENoThen, ENotEnoughLoops, EDoLoop, ELoopDo, EWhileWend, EWendWhile,
    ERepeatUntil, EUntilRepeat, EForNext, ENextFor, ESyntax, EOutOfMemory, ENameSpace, ENotDimensioned,
    EAlreadyDimensioned, EType, ELabelUndefined, ELabelTwice,
};

// VerErr: the error at VerPos (never returns)
[[noreturn]] void fail(uint16_t n);

inline uint16_t word(const uint8_t *p) { return peek_w(p); }
inline uint16_t next_word()
{
    uint16_t w = peek_w(pc);
    pc += 2;
    return w;
}

// Finie / FinieB: the end of an instruction
bool finished(uint16_t token);

// SetNot1.3: not compatible with AMOS 1.3 (an error if checked)
void not_13();

// A routine of the assembler, with a6 = pc (and pc = a6 after). The
// assembler keeps state in registers through the verification (a3/a4 for
// New_Reloc): what it changes goes to the context, and from there back
// to the assembler that called C++ (enter / leave in each entry).
void call(host::RoutineArg routine, AllRegs &r);
void enter(AllRegs *r);
void leave(AllRegs *r);

// What comes after an instruction's routine (0: the first instruction of
// a line, after its header)
enum : int32_t { LineStart = 0, AtLine, AtInstruction, AfterInstruction, PassTwo };
using Routine = int32_t (*)(uint16_t token);
Routine routine_of(int8_t kind, bool line_start);       // verif/handlers.cpp
Routine loop_routine(int8_t kind);                      // verif/loops.cpp
uint8_t variable_alone();                               // VerGV: a variable alone, its type
void procedure_no_parameter();                          // V1_GoProcedureNoParam

// verif/expr.cpp
uint8_t evaluate();                              // Ver_Evalue: the type ('0' a number, '2' a string)
uint8_t revalue();                               // Ver_REvalue (in parentheses)
uint8_t expression();                            // Ver_Expression
uint16_t instruction(const uint8_t *definition); // VerI: the number of parameters
uint16_t function(const uint8_t *definition);    // VerF
uint16_t last_token();                           // the token after the last expression
// Ver_DInst / Ver_OlDInst: the parameters of token's definition in table;
// its type in *kind
const uint8_t *definition(const uint8_t *table, uint16_t token, uint8_t *kind);
const uint8_t *old_definition(const uint8_t *table, uint16_t token, uint8_t *kind);
void instruction_tested(const uint8_t *definition);     // VerI_DejaTestee
void equate(const uint8_t *header, uint8_t *data);      // Equ_Verif
uint8_t structure();                                    // VStru: Struc(,), the equate's type

// verif/tables.cpp: TablA, an entry per loop or structure (and its
// variable part after it)
struct Entry {
    be_u32 Vta_Next, Vta_Prev;
    be_u16 Vta_Token;
    uint8_t Vta_Flag, Vta_UFlag;
    be_u32 Vta_Prog;
    be_u16 Vta_NBoucles, Vta_PBoucles;
    be_u32 Vta_Jump;
};
static_assert(sizeof(Entry) == 24);
void tables_free();                              // Free_TablA
void tables_reserve();                           // Reserve_TablA
Entry *table_add(uint32_t jump, uint16_t token, int16_t size, uint8_t flag);     // Init_TablA
Entry *table_find(Entry *&e, uint8_t flag, uint16_t level, int16_t token);      // Find_TablA
Entry *table_find_test(Entry *&e, uint16_t t1, uint16_t t2, uint16_t t3, uint16_t *token);
void poke_distance(uint32_t to);                 // Doke_Distance
uint32_t instruction_end();                      // Find_End
void goto_loops(Entry *e, uint32_t to);          // Goto_Loops
ContextCursor label_store();                          // V1_StockLabel
uint32_t label_use(uint16_t phase, ContextCursor *entry);    // V2_FindLabel(P)
void label_or_expression();                      // V1_GoLabel
uint32_t label_address();                        // V2_GoLabel
void label_skip();                               // V1_SautLGoto

// verif/vars.cpp
struct Name {
    const uint8_t *after;       // past the name
    int16_t length;
    uint8_t flags;
    uint8_t type;               // '0' or '2'
};
Name name();                                     // VarA0
bool store(uint8_t **entry);                     // V1_StoVar: true if new
uint16_t dimensions();                           // VerTablo
void locals();                                   // Locale
void globals();                                  // Globale
void reloc_free();                               // Free_Reloc
void reloc_reserve();                            // Reserve_Reloc
void reloc(uint8_t code);                        // New_Reloc
void reloc_byte(uint8_t b);                      // Out_Reloc

// verif/program.cpp: memory at a long of the data zone (A5_Reserve: after
// a long, the size; false if out of memory; A5_Free), the current file
// (D_Open on Name1, D_Read... of +B.s), the kind of a program file's
// header (0 AMOS 1.3, 1 AMOS Pro, -1 neither)
bool a5_reserve(be_u32 *slot, uint32_t size, uint32_t flags);
void a5_free(be_u32 *slot);
bool file_open(uint32_t mode);
bool file_read(void *to, uint32_t length);
bool file_write(const void *from, uint32_t length);
int32_t file_seek(int32_t offset, int32_t mode);
void file_close();
int program_kind(const char *header);

void tables_swap(bool verify);                   // Ver_Verif / Ver_Run (verif/main.cpp)
void decode_procedure(uint8_t *proc);            // ProCode (verif/handlers.cpp)

// verif/buffers.cpp
uint16_t stack_reserve();                        // Stack_Reserve
void direct_variables();                         // ResDir
void resize_variables(uint32_t size);            // ResVarBuf
void resize_names(uint32_t size);                // ResVNom
void clear_buffers();                            // ClearBuffers
void clear_variables();                          // ClearVar

// verif/includes.cpp
void includes_get();                             // Get_Includes
void includes_clear();                           // Includes_Clear
uint32_t includes_address(uint32_t at);          // Includes_Adr
void equates_free();                             // Equ_Free
void relocate_compiled(uint8_t *procedure);      // Ver_APCmp

} // namespace amos::verif
