// The Amiga's machine code, where C++ cannot do without it: a call to
// code at an address with all the registers (the programs' machine code,
// the system's libraries), the math libraries' and the narrator's
// functions, and the entries the system calls: the VBL server, the input
// handler, the requesters and LoadView patched, the samples' audio
// interrupts, the file selector's task.

#include "runtime/glue.h"

using namespace amos;

extern "C" {
int32_t amiga_native_call(const void *at, AllRegs *r);
void lib_fs_task();
extern uint32_t lib_fs_task_a5;
uint32_t w_amos_here;           // the address of T_AMOSHere, for LoadView
}

// The registers a5... of the caller become the context of the task for a
// C++ function called with all of them (the glue's WEntry and _w_go)
extern "C" void amiga_w_go(AllRegs *r, void (*fn)(AllRegs *))
{
    Ctx &c = ctx();
    Ctx saved = c;
    c.d6 = r->d[6];
    c.d7 = r->d[7];
    c.a3 = decode_context_cursor(r->a[3]);
    c.a4 = decode_context_cursor(r->a[4]);
    c.a5 = decode_data_zone(r->a[5]);
    c.a6 = ptr<uint8_t>(r->a[6]);
    fn(r);
    ctx() = saved;
}

namespace amos::host {

int32_t native_call(const void *at, AllRegs &r) { return amiga_native_call(at, &r); }

// The entries the host does with C++ (VblIn, IoHandler: the VBL server and the
// input handler): here they are machine code, below, and these are never called
void host_VblIn(AllRegs &) {}
void host_IoHandler(AllRegs &) {}

} // namespace amos::host

// The file selector's task (lib/fsel.cpp): a5 its data zone
extern "C" void lib_fs_task_entry()
{
    Ctx &c = ctx();
    c = Ctx{};
    c.a5 = decode_data_zone(lib_fs_task_a5);
    lib_fs_task();
}

// The C++ the entries below call
extern "C" {
void w_vbl(uint8_t *data);
void w_io_handler(AllRegs *r);
void w_auto_request(AllRegs *r);
void w_easy_request(AllRegs *r);
void ext_music_sami(void *s);
extern uint32_t amos_w_base;

}

asm(R"(
	.text
	.even

| setjmp, longjmp (the C library here has none): jmp_buf the return,
| d2-d7/a2-a7
	.globl	_setjmp
_setjmp:
	move.l	4(sp),a0
	move.l	(sp),(a0)+
	movem.l	d2-d7/a2-a7,(a0)
	moveq	#0,d0
	rts

	.globl	_longjmp
_longjmp:
	move.l	4(sp),a0
	move.l	8(sp),d0
	bne.s	.Llj_val
	moveq	#1,d0
.Llj_val:
	move.l	(a0)+,a1
	movem.l	(a0),d2-d7/a2-a7
	move.l	a1,(sp)
	rts

| int32_t amiga_native_call(const void *at, AllRegs *r): the code at with
| d0-d7/a0-a6 from r, which gets them back; its condition codes as -1, 0, 1
	.globl	_amiga_native_call
_amiga_native_call:
	movem.l	d2-d7/a2-a6,-(sp)
	move.l	4+44(sp),a1
	move.l	8+44(sp),a0
	move.l	a0,-(sp)
	pea	.Lnc_back(pc)
	move.l	a1,-(sp)
	movem.l	(a0),d0-d7/a0-a6
	rts
.Lnc_back:
	beq.s	.Lnc_eq
	blt.s	.Lnc_lt
	move.l	#1,-(sp)
	bra.s	.Lnc_store
.Lnc_lt:
	move.l	#-1,-(sp)
	bra.s	.Lnc_store
.Lnc_eq:
	clr.l	-(sp)
.Lnc_store:
	move.l	a0,-(sp)
	move.l	8(sp),a0
	movem.l	d0-d7,(a0)
	move.l	(sp)+,32(a0)
	movem.l	a1-a6,36(a0)
	move.l	(sp)+,d0
	addq.l	#4,sp
	movem.l	(sp)+,d2-d7/a2-a6
	rts

| int32_t interp_lvo(uint32_t *r, uint32_t base, int32_t offset): the math
| library's function with d0-d3 from r, d0/d1 back; its condition codes
	.globl	_interp_lvo
_interp_lvo:
	movem.l	d2-d3/a6,-(sp)
	move.l	16(sp),a0
	move.l	20(sp),a6
	move.l	24(sp),d0
	lea	0(a6,d0.l),a1
	move.l	a0,-(sp)
	movem.l	(a0),d0-d3
	jsr	(a1)
	beq.s	.Llvo_eq
	blt.s	.Llvo_lt
	moveq	#1,d2
	bra.s	.Llvo_out
.Llvo_lt:
	moveq	#-1,d2
	bra.s	.Llvo_out
.Llvo_eq:
	moveq	#0,d2
.Llvo_out:
	move.l	(sp)+,a0
	movem.l	d0-d1,(a0)
	move.l	d2,d0
	movem.l	(sp)+,d2-d3/a6
	rts

| uint32_t music_lvo(uint32_t *r, uint32_t base, int32_t lvo): the library
| function with d0, d1, a0, a1 from r; d0 back (also in r[0])
	.globl	_music_lvo
_music_lvo:
	movem.l	d2-d7/a2-a6,-(sp)
	move.l	4+44(sp),a2
	move.l	8+44(sp),a6
	move.l	12+44(sp),d2
	movem.l	(a2),d0-d1/a0-a1
	jsr	0(a6,d2.l)
	move.l	4+44(sp),a2
	move.l	d0,(a2)
	movem.l	(sp)+,d2-d7/a2-a6
	rts

| The VBL server (+W.s VblIn): a1 the data zone
	.globl	_amiga_w_vbl_in
_amiga_w_vbl_in:
	movem.l	d2-d7/a2-a4,-(sp)
	move.l	a1,-(sp)
	jsr	_w_vbl
	addq.l	#4,sp
	movem.l	(sp)+,d2-d7/a2-a4
	lea	0xdff000,a0
	moveq	#0,d0
	rts

| The samples' audio interrupts (music_glue.s): a1 the voice's SampleInt
	.globl	_amiga_music_sami_entry
_amiga_music_sami_entry:
	move.l	a1,-(sp)
	jsr	_ext_music_sami
	addq.l	#4,sp
	rts

| A C++ function (4(sp)) with all the registers, a5 the caller's (WEntry)
.Lw_entry:
	movem.l	d0-d7/a0-a6,-(sp)
	move.l	sp,a0
	move.l	64(sp),-(sp)
	move.l	a0,-(sp)
	jsr	_amiga_w_go
	addq.l	#8,sp
	movem.l	(sp)+,d0-d7/a0-a6
	tst.l	d0
	rts

| The input handler (+W.s IoHandler): a0 the events
	.globl	_amiga_w_io_handler_entry
_amiga_w_io_handler_entry:
	move.l	a5,-(sp)
	move.l	_amos_w_base,a5
	pea	_w_io_handler
	bsr.s	.Lw_entry
	addq.l	#4,sp
	move.l	(sp)+,a5
	rts

| AutoRequest and EasyRequest patched (+W.s AutoReq, EasyReq)
	.globl	_amiga_w_auto_req_entry
_amiga_w_auto_req_entry:
	move.l	a5,-(sp)
	move.l	_amos_w_base,a5
	pea	_w_auto_request
	bsr.s	.Lw_entry
	addq.l	#4,sp
	move.l	(sp)+,a5
	rts

	.globl	_amiga_w_easy_req_entry
_amiga_w_easy_req_entry:
	move.l	a5,-(sp)
	move.l	_amos_w_base,a5
	pea	_w_easy_request
	bsr.s	.Lw_entry
	addq.l	#4,sp
	move.l	(sp)+,a5
	rts

| LoadView patched (+W.s AMOS_LoadView): nothing while AMOS is in front
| (T_AMOSHere), else the system's
	.globl	_amiga_w_amos_loadview
_amiga_w_amos_loadview:
	move.l	a0,-(sp)
	move.l	_w_amos_here,a0
	tst.b	(a0)
	movem.l	(sp)+,a0
	bne.s	.Llv_wb
	move.l	a0,-(sp)
	move.l	_w_old_loadview,a0
	move.l	(a0),-(sp)
	move.l	4(sp),a0
	move.l	(sp),4(sp)
	addq.l	#4,sp
.Llv_wb:
	rts
)");
