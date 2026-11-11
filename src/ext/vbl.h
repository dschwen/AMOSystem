// AMOS's list of routines for each vertical blank, shared by the extensions that have one
#pragma once

namespace amos::ext3 {

// AMOS's list of routines for each vertical blank (VblRout: 8, ended by 0): this one added (once), or
// taken off. The Music extension has its own entry; they stay in one run from the start.
void vbl_add(void (*routine)());
void vbl_remove(void (*routine)());

} // namespace amos::ext3
