// Shared by the C++ parts of the editor (src/editor). The editor's
// assembler keeps the current edit window in a4 and the current program
// in a6; the glue (w/wglue.i) makes them part of the C++ context.
#pragma once

#include "core/amos.h"
#include "core/source.h"

namespace amos::ed {

inline EditWindow &window() { return *reinterpret_cast<EditWindow *>(ctx().a4); }
inline Program &program() { return *reinterpret_cast<Program *>(ctx().a6); }

// Calls an editor routine of +Edit.s with registers r; returns its d0
inline uint32_t call(host::RoutineArg routine, Regs &r)
{
    entry_call(routine, r);
    return r.d[0];
}

// AMOS.library's memory (SyCall MemFast, MemFastClear, MemFree): 0 if none
inline uint32_t memory(uint32_t size, bool clear) { return w::reserve(size, clear ? w::FastClear : w::Fast); }
inline void free_memory(uint32_t address, uint32_t size) { w::release(address, size); }

// TInst (core/source.cpp)
using source::skip_token;
// editor/tokens.cpp: Detok of a line into buffer (the length word, the
// text), its length
uint32_t detok(const uint8_t *line, uint8_t *buffer);
// Tokenise: 1, 0 for an empty line, -1 if too long
int32_t tokenise_line(uint8_t *text, uint8_t *buffer);

// editor/storage.cpp
struct Stored {
    int32_t error;              // 0, 1 out of buffer space, -1 a closed procedure
    uint32_t added;             // 1: a line more
    uint8_t *line;
};
uint8_t *find_line(uint16_t n);
uint8_t *next_line();
Stored store(const uint8_t *line, uint16_t position, bool insert);
Stored store_block(const uint8_t *block, uint32_t length, uint16_t position);
int32_t delete_line(uint16_t n);
void delete_chunk(uint8_t *from, uint8_t *to);
// Ed_DelChunk: bytes from line n out of the program
void delete_lines(uint16_t n, uint32_t bytes);

// editor/undo.cpp: a new undo entry (0 if undo is off), one keeping n
// characters of a line
uint8_t *undo_start();
uint8_t *undo_text(const uint8_t *text, uint16_t n);
void undo_reset();
void undo_free();

// editor/lines.cpp: Ed_PKey, Ed_Delete; what undo and redo do
void type_key(uint8_t c, bool insert, uint8_t *redone);
void delete_key();
void undo_change(uint8_t *entry);
void redo_change(uint8_t *entry);

// editor/display.cpp: Ed_Loca, Ed_EALigne (erase) / Ed_ALigne, Ed_AffBuf,
// Ed_NewBuf, Ed_NewBufAff, Ed_EALiCu, Ed_Cent, Ed_SetX, Ed_SetY, Ed_GotoX,
// Ed_GotoY
void place_cursor();
void show_line(uint16_t y, bool erase);
void show_all();
void new_buffer();
void new_buffer_all();
void show_current_line();
void centre();
void set_x(uint16_t x);
void set_y(uint16_t y);
void goto_x(uint16_t x);
void goto_y(uint16_t y);
void show_column(uint16_t x);
void cursor_off();
void show_slider();

// editor/block.cpp: Ed_BlocOn
void block_on();
struct Labels {
    uint32_t before, after;
    uint16_t here;
};
Labels labels_around(uint16_t start);

// editor/lines.cpp: R_MotGoch, R_MotDroi, the length of the cursor's line
void insert_text_at_cursor(const uint8_t *text, uint16_t n);
void delete_text_at_cursor(uint16_t n);
uint16_t word_left_of_cursor();
uint16_t word_right_of_cursor();
uint16_t current_line_length();

// editor/cursor.cpp: Ed_Untok, Ed_BufUntok, Ed_CGauche, Ed_CDroite
void untokenise_line(uint16_t n, uint16_t y);
void untokenise_window();
void cursor_left();
void cursor_right();
void line_up_t();
void line_down_t();

// editor/edit.cpp: Ed_TokCur, Ed_InsLine, Ed_DelLiCu, Ed_ReturnQuiet,
// Ed_Join
void tokenise_current_line();
void insert_line();
int32_t delete_current_line();
void return_quiet();
void join_lines();

// editor/marks.cpp: Ed_MarksChange (lines inserted or deleted at line
// n), Ed_AutoMarks
void marks_change(uint16_t n, int16_t delta);
void auto_mark();

// editor/windows.cpp
EditWindow *next_shown(const EditWindow *w);
EditWindow *previous_shown(const EditWindow *w);
EditWindow *window_number(uint16_t n);
EditWindow *hidden(uint16_t n);
void draw_windows();
bool activate(EditWindow *w);
int32_t open_window(bool shown, int32_t size);
void delete_window();
void update_split(EditWindow *w, uint16_t flags);
void follow_scroll();

// editor/status.cpp: Ed_GetMessageA0 (text n of a list, from 1),
// Ed_GetMessage, Ed_GetSysteme, Ed_Avertir, Ed_AverMess, Ed_AverFin,
// Ed_AllAverFin
const uint8_t *message_of(const uint8_t *list, uint16_t n, uint8_t *length);
const uint8_t *message(uint16_t n);
const uint8_t *system_message(uint16_t n);
// Ed_NPrgToBuf: the program's name over the spaces at to; the end of it
uint8_t *name_into(uint8_t *to, const Program &program);
void warn(const uint8_t *text);
void warn_message(uint16_t n);
void warning_end();
void all_warnings_end();

// editor/keys.cpp: Ed_Ky2Fonc (from 0, -1 if none), Ed_Fonc2Ky (from 1)
int32_t key_function(uint32_t key);
const uint8_t *function_keys(uint8_t n);

// editor/macros.cpp: Ed_MemReserve, Ed_MemFree, Ed_ListeNew, Ed_ListeDel,
// Ed_ListeDelAll, Ed_IChaine, Ed_DChaine, Ed_AChaine, Ed_Dialogue, EdMa_Adr
uint8_t *memory_reserve(be_u32 *at, uint32_t size);
void memory_release(be_u32 *at);
uint8_t *list_new(be_u32 *list, uint32_t size);
void list_delete(be_u32 *list, uint8_t *e);
void list_delete_all(be_u32 *list);
void dialogue_strings_reset();
uint8_t *dialogue_string(const uint8_t *text, const uint8_t **after = nullptr);
uint8_t *dialogue_string_add(const uint8_t *text, const uint8_t **after = nullptr);
int32_t dialogue(uint16_t n);
int32_t run_dialogue(uint16_t n);
uint8_t *macro_of(uint32_t key);

// editor/config.cpp: Ed_SetBanks, Ed_SamPlay, Ed_GetNbMessage,
// Ed_GetFsMessage, EdC_ChangeTexte
void set_banks();
void sound_play(uint8_t c);
uint16_t text_count(const uint8_t *list);
uint16_t text_free(const uint8_t *list);
bool text_change(be_u32 *at, const uint8_t *text, uint16_t n);

// editor/screen.cpp: Ed_Unpack / Es_Unpack (title: the direct mode's
// title screen), Ed_MemoryDraw, Ed_MemoryAff, Ed_LimM
void unpack(uint16_t n, int16_t x, int16_t y, bool title = false);
void memory_draw(int16_t x, int16_t y, uint16_t width, bool title);
void memory_show();
void limit_mouse();

// editor/direct.cpp: Ed_GetError: the message of an error (a negative
// code: one of the test)
struct Error {
    const uint8_t *message;
    int16_t code;
};
Error get_error(int16_t code, const uint8_t *message);

// editor/disk.cpp: Ed_RLoadHidden
void load_hidden();

// editor/menu.cpp: EdM_Init (false if out of memory), EdM_MarkAll
bool menu_init();
void menu_marks();

// editor/loop.cpp: Ed_FCall
void function_call(uint16_t n);

// editor/indent.cpp
void indent(uint8_t *program, uint16_t tab);

} // namespace amos::ed
