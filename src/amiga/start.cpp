// AMOSPro's start on the Amiga (+B.s Cold_Start, Get_Out): the first code
// of the program (its section first: link/amospro.ld), with the CLI's
// command line in a0/d0. AMOS runs on a stack of its own (64K: the C++
// needs more than the 4K a CLI gives by default); its base (BasSp) is the
// runtime's mark (runtime/marks.h). Get_Out gives the stack back, replies
// to the Workbench's message (d7) and returns to the CLI.

#include "runtime/runtime.h"
#include "runtime/glue.h"
#include "hal/workbench.h"

using namespace amos;

asm(R"(
	.section .text.entry,"ax"
	.even
	.globl	_amiga_entry
_amiga_entry:
	move.l	sp,_amiga_save_sp
	move.l	a0,_amiga_command
	move.l	d0,_amiga_command_length
	move.l	4.w,a6
	move.l	#65536,d0
	moveq	#1,d1
	jsr	-198(a6)
	tst.l	d0
	beq.s	.Lno_stack
	lea	_amiga_stack,a0
	move.l	d0,(a0)
	add.l	#65536,d0
	move.l	d0,4(a0)
	move.l	d0,8(a0)
	jsr	-732(a6)
	move.w	#1,_amiga_stack_on
.Lno_stack:
	move.l	4.w,a6
	move.l	276(a6),_amos_task
	jsr	_amiga_start

| main_get_out(d7): the stack given back, the Workbench's message replied
	.globl	_main_get_out
_main_get_out:
	move.l	4(sp),d7
	move.l	_amiga_save_sp,sp
	tst.w	_amiga_stack_on
	beq.s	.Lold_stack
	clr.w	_amiga_stack_on
	move.l	4.w,a6
	lea	_amiga_stack,a0
	jsr	-732(a6)
	move.l	_amiga_stack,a1
	move.l	#65536,d0
	jsr	-210(a6)
.Lold_stack:
	tst.l	d7
	beq.s	.Lno_message
	move.l	4.w,a6
	jsr	-132(a6)
	move.l	d7,a1
	jsr	-378(a6)
.Lno_message:
	moveq	#0,d0
	rts
	.text
)");

extern "C" {
uint32_t amiga_save_sp;
char *amiga_command;
uint32_t amiga_command_length;
uint32_t amiga_stack[3];        // the StackSwapStruct
uint16_t amiga_stack_on;

// The contexts of the tasks (core/amos.h ctx): the main task, its
// context, the context of any other task
uint8_t amos_task[52];
uint8_t *amos_bridge = amos_task;

// main/boot.cpp
void main_cold_start(char *command, uint32_t length, uint32_t sp);
[[noreturn]] void main_init_fin();
[[noreturn]] void main_get_out(uint32_t message);
}

namespace amos::host {
[[noreturn]] void base_run(void (*start)(uint32_t base));
}

namespace {

void start(uint32_t base)
{
    main_cold_start(amiga_command, amiga_command_length, base);
    main_init_fin();
}

} // namespace

extern "C" [[noreturn]] void amiga_start()
{
    host::glue_start();
    host::base_run(start);
}

namespace amos::host {

// An entry with no C++: a requester, then the way out
[[noreturn]] void missing(const char *what, const void *)
{
    hal::panic_requester(what);
    main_get_out(0);
}

} // namespace amos::host
