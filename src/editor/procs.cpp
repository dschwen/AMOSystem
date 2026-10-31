// Opening and closing procedures in the editor (+Edit.s "PROCEDURES"): bit
// 7 of byte 10 of a Procedure line closes it (its lines are then shown as
// one), bit 6 locks it so. Closing tests the program first. The marks are
// kept as addresses meanwhile, as the line numbers change.

#include "editor/editor.h"
#include "core/source.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

// Ed_VaTester: the program tested if changed (an error does not come back)
void test_program()
{
    Regs r;
    r.d[0] = 0xFFFFFFFF;
    call(ed_va_tester, r);
}

void count_lines()
{
    Regs r;
    lib_call(L_Prg_CptLines, r);
}

uint16_t number_of(const uint8_t *line)
{
    return uint16_t(source::find_address(ptr<uint8_t>(program().Prg_StBas), line).number);
}

// Ed_Procs: all the procedures opened or closed
void all(bool close)
{
    tokenise_current_line();
    if (close)
        test_program();
    run(ed_marks_to_addresses_entry);
    EditWindow &w = window();
    uint8_t *here = find_line(w.Edt_YPos + w.Edt_YCu);
    for (uint8_t *line = find_line(0); peek_w(line); line = next_line()) {
        if (peek_w(line + 2) != tk::Proc || line[10] & 0x40)
            continue;
        line[10] &= ~0x80;
        if (close)
            line[10] |= 0x80;
    }
    count_lines();
    run(ed_marks_to_numbers_entry);
    set_y(number_of(here));
    new_buffer();
}

// Ed_ProcOpen: the procedure the cursor is in opened or closed
void flip()
{
    tokenise_current_line();
    EditWindow &w = window();
    Program &p = program();
    find_line(w.Edt_YPos + w.Edt_YCu);
    uint8_t *proc = ptr<uint8_t>(w.Edt_DebProc);
    if (!proc)
        run(ed_fo_e);
    if (!(proc[10] & 0x40)) {
        if (!(proc[10] & 0x80))
            test_program();
        run(ed_marks_to_addresses_entry);
        proc[10] ^= 0x80;
        run(ed_marks_to_numbers_entry);
        count_lines();
        const uint8_t *at = proc;
        bool place = true;
        if (int16_t(peek_w(proc + 10)) >= 0) {
            // opened: on the error found in it, if any
            if (!p.Prg_AdEProc) {
                place = false;
            } else {
                run(ed_r_alert);
                set_x(p.Prg_XEProc);
                at = ptr<uint8_t>(p.Prg_AdEProc & 0x7FFFFFFF);
            }
        }
        if (place)
            set_y(number_of(at));
    }
    new_buffer();
}

} // namespace

extern "C" {
void ed_procs_open(AllRegs *) { all(false); }
void ed_procs_close(AllRegs *) { all(true); }
void ed_procs(AllRegs *r) { all(uint16_t(r->d[2])); }
void ed_proc_open(AllRegs *) { flip(); }

// Ed_ClEProc: no more centring on an error in a procedure
void ed_clear_error_proc(AllRegs *)
{
    Program &p = program();
    if (!p.Prg_AdEProc)
        return;
    if (int32_t(p.Prg_AdEProc) >= 0)
        p.Prg_AdEProc |= 0x80000000;
    else
        p.Prg_AdEProc = 0;
}
}
