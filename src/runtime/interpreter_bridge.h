// One declaration for each interpreter bridge: code addresses retain their type.
#pragma once
#include "core/amos.h"
#include "core/context_token.h"
#include "core/mark_token.h"
namespace amos::interp { struct Value; }
extern "C" {
void interp_exec(amos::host::RoutineArg, uint32_t, uint32_t);
void interp_exec_value(amos::host::RoutineArg, uint32_t, uint32_t, const amos::interp::Value*);
[[noreturn]] void interp_jump(const amos::Regs*, amos::host::RoutineArg);
uint32_t interp_prun_run(uint32_t, amos::host::RoutineArg);
void interp_zap_in(amos::Regs*, amos::host::RoutineArg);
uint32_t mn_call_procedure(amos::host::RoutineArg, amos::host::ContextHandle,
                           amos::wire::MarkField*, const uint8_t*);
}
