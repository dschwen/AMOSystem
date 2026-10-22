// Shared by the C++ parts of the editor (src/editor). The editor's
// assembler keeps the current edit window in a4 and the current program
// in a6; the glue (w/wglue.i) makes them part of the C++ context.
#pragma once

#include "core/amos.h"

namespace amos::ed {

inline EditWindow &window() { return *reinterpret_cast<EditWindow *>(ctx().a4); }
inline Program &program() { return *reinterpret_cast<Program *>(ctx().a6); }

// Calls an editor routine of +Edit.s with registers r; returns its d0
inline uint32_t call(uint8_t *routine, Regs &r)
{
    amos_call(&r, routine);
    return r.d[0];
}

// AMOS.library's memory (SyCall MemFast, MemFastClear, MemFree): 0 if none
inline uint32_t memory(uint32_t size, bool clear)
{
    Regs r;
    r.d[0] = size;
    sy_call(clear ? sy::MemFastClear : sy::MemFast, r);
    return r.a[0];
}

inline void free_memory(uint32_t address, uint32_t size)
{
    Regs r;
    r.a[1] = address;
    r.d[0] = size;
    sy_call(sy::MemFree, r);
}

// editor/tokens.cpp: TInst; Detok of a line into buffer (the length word,
// the text), its length
const uint8_t *skip_token(uint16_t t, const uint8_t *a);
uint32_t detok(const uint8_t *line, uint8_t *buffer);

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

// editor/indent.cpp
void indent(uint8_t *program, uint16_t tab);

} // namespace amos::ed
