// The editor's messages, alerts, warnings and status lines (+Edit.s):
//   Ed_GetMessage / Ed_GetSysteme: message n of the editor's or the
//     system's list (a byte before each, its length)
//   Ed_Alert: a message in the status line and the editor's loop again
//     (it does not come back)
//   Ed_Avertir / Ed_AverFin: a warning box in front of the text, taken
//     away (Ed_Avert counts them; each is a block of the screen)
//   Ed_EtPrint: the status line of a window: what Edt_EtatAff asks for

#include "runtime/static_symbols.h"
#include "editor/editor.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint8_t EtAIns = 1 << 1, EtAX = 1 << 2, EtAY = 1 << 3, EtANom = 1 << 4, EtAFree = 1 << 5, EtAClw = 1 << 6,
                  EtAAlert = 1 << 7, EtABAll = 0x7F;
constexpr uint32_t EntNul = 0x80000000;
constexpr uint32_t AlertMark = 0xFFFE0102;

void wcall(int function, uint32_t d1)
{
    Regs r;
    r.d[1] = d1;
    wi_call(function, r);
}

void print(const uint8_t *text)
{
    Regs r;
    r.a[1] = address(text);
    wi_call(wi::Print, r);
}

void locate(int16_t x, int16_t y)
{
    Regs r;
    r.d[1] = uint32_t(int32_t(x));
    r.d[2] = uint32_t(int32_t(y));
    wi_call(wi::Locate, r);
}

void close_file()
{
    Regs r;
    call(AMOS_ROUTINE(ed_close), r);
}

} // namespace

namespace amos::ed {

// Ed_GetMessageA0: message n (from 1) of the list; its length in *length
const uint8_t *message_of(const uint8_t *list, uint16_t n, uint8_t *length)
{
    const uint8_t *a0 = list + 1;
    while (int16_t(--n) > 0)
        a0 += 2 + a0[0];
    if (length)
        *length = a0[0];
    return a0 + 1;
}

const uint8_t *message(uint16_t n) { return message_of(ptr<uint8_t>(idata().Ed_Messages), n, nullptr); }
const uint8_t *system_message(uint16_t n) { return message_of(ptr<uint8_t>(idata().Ed_Systeme), n, nullptr); }

// Ed_NPrgToBuf: the program's name (or "New project") over the spaces at
// to, up to the first control code; the end of the name
uint8_t *name_into(uint8_t *to, const Program &program)
{
    const uint8_t *name = program.Prg_NamePrg;
    if (*name) {
        Regs r;
        r.a[0] = address(name);
        call(AMOS_ROUTINE(ed_name_part_entry), r);
        name = ptr<uint8_t>(r.a[0]);
    } else {
        name = system_message(7);
    }
    uint8_t *a1 = to;
    while (*name) {
        *a1++ = *name++;
        if (*a1 < 32)
            break;
    }
    uint8_t *end = a1;
    while (*a1 >= 32)
        *a1++ = ' ';
    return end;
}

// Ed_AverFin: the last warning taken away; Ed_AllAverFin: all of them
void warning_end()
{
    IData &d = idata();
    uint16_t n = d.Ed_Avert;
    if (!n)
        return;
    d.Ed_Avert--;
    Regs p;
    p.d[1] = uint32_t(int32_t(int16_t(n + 10)));
    p.d[2] = p.d[3] = p.d[4] = p.d[5] = EntNul;
    p.a[1] = d.Buffer;
    ec_call(ec::BlPut, p);
    Regs l;
    l.d[1] = uint32_t(int32_t(int16_t(n + 10)));
    ec_call(ec::BlDel, l);
}

void all_warnings_end()
{
    while (idata().Ed_Avert)
        warning_end();
}

// Ed_Avertir: text as a warning box in front of the text
void warn(const uint8_t *text)
{
    IData &d = idata();
    Regs b;
    call(AMOS_ROUTINE(ed_set_banks), b);
    d.Ed_Avert++;
    const uint8_t *resource = ptr<uint8_t>(d.Ed_Resource);
    const uint8_t *programs = resource + peek_l(resource + 2 + 8);
    Regs r;
    r.d[0] = EntNul;
    r.d[1] = EntNul;
    r.d[2] = address(text);
    r.d[3] = uint32_t(int32_t(int16_t(d.Ed_Avert + 10)));
    r.d[4] = 0;
    r.d[5] = 0;                 // the program not copied
    r.d[6] = 1024;
    r.a[0] = address(programs + peek_l(programs + 6));      // its second program
    r.a[1] = address(resource + peek_l(resource + 2));      // the graphics
    r.a[2] = d.Ed_Messages;
    lib_call(L_Dia_RunQuick, r);
}

// Ed_AverMess: message n as a warning
void warn_message(uint16_t n) { warn(message(n)); }

} // namespace amos::ed

namespace {

// Ed_AvName: message n then a name (up to 72 characters) as a warning
void warn_name(uint16_t n, const uint8_t *name)
{
    uint8_t *buffer = ptr<uint8_t>(idata().Ed_BufT);
    uint8_t *to = buffer;
    for (const uint8_t *from = message(n); (*to++ = *from++);)
        ;
    to--;
    while ((*to++ = *name++))
        ;
    buffer[72] = 0;
    warn(buffer);
}

// Et_Print: system message n into the status line
void status_text(uint16_t n) { print(system_message(n)); }

// Et_Chiffre: value at column x of the status line, digits wide
void status_number(uint32_t value, int8_t x, uint16_t digits)
{
    locate(x, 0);
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    for (int i = 0; i < 12; i++)
        buffer[i] = ' ';
    Regs r;
    r.d[0] = value;
    r.a[0] = address(buffer);
    lib_call(L_LongToDec, r);
    buffer[digits] = 0;
    print(buffer);
}


// Ed_EtOrder / Ed_EtIns / Ed_EtLine / Ed_EtCol / Ed_EtFree / Ed_EtNom: one
// part of the current window's status line
void status_order()
{
    status_number(uint32_t(int32_t(int16_t(window().Edt_Order))), int8_t(idata().Ed_EtXX[1]), 2);
}

void status_insert()
{
    locate(int8_t(idata().Ed_EtXX[2]), 0);
    status_text(idata().Ed_Insert ? 6 : 5);
}

void status_y()
{
    EditWindow &w = window();
    status_number(uint32_t(int32_t(int16_t(w.Edt_YPos + w.Edt_YCu + 1))), int8_t(idata().Ed_EtXX[3]), 5);
}

void status_x() { status_number(uint16_t(window().Edt_XCu + 1), int8_t(idata().Ed_EtXX[4]), 3); }

void status_free()
{
    Program &p = program();
    status_number(p.Prg_StBas - p.Prg_StMini, int8_t(idata().Ed_EtXX[5]), 7);
}

void status_name()
{
    IData &d = idata();
    EditWindow &w = window();
    locate(int8_t(d.Ed_EtXX[6]), 0);
    status_text(w.Edt_LinkPrev || w.Edt_LinkNext ? 4 : 3);
    int16_t x = int8_t(d.Ed_EtXX[7]);
    locate(x, 0);
    uint8_t *buffer = ptr<uint8_t>(d.Buffer);
    int16_t n = int16_t((w.Edt_WindESx >> 3) - x - 2);
    uint8_t *p = buffer;
    do
        *p++ = ' ';
    while (n-- > 0);
    *p = 0;
    name_into(buffer, program());
    print(buffer);
}

// Ed_EtPrintD0: the parts of the status line of the current window
// Edt_EtatAff asks for
void status_line(EditWindow &w)
{
    IData &d = idata();
    if (!w.Edt_EtatAff || w.Edt_Hidden)
        return;
    if (w.Edt_EtatAff & EtAAlert) {
        // an alert
        w.Edt_EtatAff = 0;
        if (!w.Edt_EtAlert)
            return;
        wcall(wi::QWindow, w.Edt_WindEtat);
        status_text(8);
        Regs c;
        c.a[1] = w.Edt_EtAlert;
        wi_call(wi::Centre, c);
        w.Edt_EtAlert = 0;
        wcall(wi::QWindow, w.Edt_Window);
        return;
    }
    if (w.Edt_EtMess)
        return;
    wcall(wi::QWindow, w.Edt_WindEtat);
    if (d.EdMa_Tape && d.Edt_Current == address(&w)) {
        // a macro being recorded
        status_text(8);
        Regs c;
        c.a[1] = address(message(30));
        wi_call(wi::Centre, c);
        w.Edt_EtatAff = 0;
        return;
    }
    if (w.Edt_EtatAff & EtAClw) {
        w.Edt_EtatAff &= ~EtAClw;
        status_text(d.Edt_Current == address(&w) ? 10 : 11);
        status_text(2);
        status_text(12);
        status_order();
    }
    if (w.Edt_EtatAff & EtAIns) {
        w.Edt_EtatAff &= ~EtAIns;
        status_insert();
    }
    if (w.Edt_EtatAff & EtAY) {
        w.Edt_EtatAff &= ~EtAY;
        status_y();
    }
    if (w.Edt_EtatAff & EtAX) {
        w.Edt_EtatAff &= ~EtAX;
        status_x();
    }
    if (w.Edt_EtatAff & EtAFree) {
        w.Edt_EtatAff &= ~EtAFree;
        status_free();
    }
    if (w.Edt_EtatAff & EtANom) {
        w.Edt_EtatAff &= ~EtANom;
        status_name();
    }
}

// Ed_RazAlert
void alert_clean()
{
    Regs t;
    t.d[0] = 0;
    t.a[0] = boot::temp_buffer(t.d[0]);
    close_file();
}

// Ed_Alert: text in the status line for time, then the editor's loop
[[noreturn]] void alert(const uint8_t *text, uint16_t time)
{
    IData &d = idata();
    EditWindow &w = window();
    // (the assembler stored the d0 of its sound, 'G', and not the time)
    (void)time;
    w.Edt_EtMess = 'G';
    w.Edt_EtAlert = address(text);
    w.Edt_EtatAff |= EtAAlert;
    if (d.Ed_Zappeuse) {
        w.Edt_EtMess = time;
        d.Ed_ZapMessage = address(text);
        d.Ed_ZapError = 0xFFFF;
        alert_clean();
    } else {
        Regs s;
        s.d[0] = 'G';
        call(AMOS_ROUTINE(ed_sam_play), s);
        // kept for Ed_RAlert
        uint8_t *kept = ptr<uint8_t>(d.Ed_BufT) + 256;
        poke_l(kept, AlertMark);
        kept += 4;
        while ((*kept++ = *text++))
            ;
        alert_clean();
    }
    Regs l;
    call(AMOS_ROUTINE(ed_loop), l);
    __builtin_unreachable();
}

} // namespace

extern "C" {
// Ed_GetMessage / Ed_GetSysteme / Ed_GetMessageA0: message d0 (of the list
// a0); a0 the text, d0 its length
static void got(AllRegs *r, const uint8_t *list)
{
    uint8_t length;
    r->a[0] = address(message_of(list, uint16_t(r->d[0]), &length));
    r->d[0] = (r->d[0] & 0xFFFFFF00) | length;
}

void ed_get_message_fn(AllRegs *r) { got(r, ptr<uint8_t>(idata().Ed_Messages)); }
void ed_get_systeme_fn(AllRegs *r) { got(r, ptr<uint8_t>(idata().Ed_Systeme)); }
void ed_get_message_a0(AllRegs *r) { got(r, ptr<uint8_t>(r->a[0])); }

// Ed_GetNMessage: the message after a0; a0 the text, d0 its length
void ed_get_next_message(AllRegs *r)
{
    const uint8_t *a0 = ptr<uint8_t>(r->a[0]) - 1;
    a0 += 2 + a0[0];
    r->d[0] = a0[0];
    r->a[0] = address(a0 + 1);
}

void ed_alert_fn(AllRegs *r) { alert(ptr<uint8_t>(r->a[0]), uint16_t(r->d[0])); }

// The alerts of the editor's messages (+Edit.s): message n for time
[[noreturn]] static void message_alert(uint16_t n, uint16_t time)
{
    uint8_t length;
    alert(message_of(ptr<uint8_t>(idata().Ed_Messages), n, &length), time);
}

void ed_not_edit_fn(AllRegs *) { message_alert(183, 100); }         // Ed_NotEdit
void ed_line_too_long(AllRegs *) { message_alert(199, 50); }        // Ed_LToLong
void ed_top_of_text_fn(AllRegs *) { message_alert(200, 25); }       // Ed_CHtE
void ed_bottom_of_text_fn(AllRegs *) { message_alert(201, 25); }    // Ed_CBasE
void ed_no_mark_fn(AllRegs *) { message_alert(65, 100); }           // Ed_NoMark
void ed_fold_error(AllRegs *) { message_alert(203, 100); }          // Ed_FoE
void ed_alert_100_fn(AllRegs *r) { message_alert(uint16_t(r->d[0]), 100); }   // Ed_Al100
void ed_out_of_memory_fn(AllRegs *) { message_alert(204, 120); }    // Ed_OMm
void ed_not_done2_fn(AllRegs *) { message_alert(206, 100); }        // Ed_NotDone2
void ed_block_what_fn(AllRegs *) { message_alert(6, 127); }         // Ed_BlocWhat
void ed_too_many_windows_fn(AllRegs *) { message_alert(3, 127); }   // Ed_2ManyWindow
void ed_no_hide_fn(AllRegs *) { message_alert(2, 100); }            // Ed_NoHide
void ed_already_runned_fn(AllRegs *) { message_alert(12, 100); }    // Ed_AlRunned
void ed_no_undo_fn(AllRegs *) { message_alert(4, 100); }            // Ed_NoUndo
void ed_no_redo_fn(AllRegs *) { message_alert(5, 100); }            // Ed_NoRedo
void ed_pro_alert_fn(AllRegs *r) { message_alert(uint16_t(r->d[0]), 127); }   // Ed_ProAlert

// Ed_OofBuf, Ed_NoFound, Ed_NotDone: the line shown again first
void ed_out_of_buffer(AllRegs *)
{
    new_buffer();
    message_alert(202, 200);
}
void ed_no_found_fn(AllRegs *)
{
    new_buffer();
    message_alert(205, 100);
}
void ed_not_done_fn(AllRegs *)
{
    new_buffer();
    message_alert(206, 100);
}

// Ed_PaAMOS: the file closed first
void ed_close_fn(AllRegs *r);
void ed_not_amos_fn(AllRegs *r)
{
    ed_close_fn(r);
    message_alert(207, 100);
}

// Ed_ZapAlert: the same when a program drives the editor
void ed_zap_alert(AllRegs *r)
{
    IData &d = idata();
    EditWindow &w = window();
    const uint8_t *text = ptr<uint8_t>(r->a[0]);
    w.Edt_EtMess = uint16_t(r->d[0]);
    w.Edt_EtAlert = address(text);
    w.Edt_EtatAff |= EtAAlert;
    d.Ed_ZapMessage = address(text);
    d.Ed_ZapError = 0xFFFF;
    alert_clean();
    Regs l;
    call(AMOS_ROUTINE(ed_loop), l);
}

void ed_alert_clean(AllRegs *) { alert_clean(); }

// Ed_AlertRts: an alert in the current window's status line, and back
void ed_alert_rts(AllRegs *r)
{
    IData &d = idata();
    if (d.Ed_Zappeuse) {
        ed_zap_alert(r);
        return;
    }
    ctx().a4 = decode_context_cursor(d.Edt_Current);
    ctx().a6 = ptr<uint8_t>(window().Edt_Prg);
    EditWindow &w = window();
    w.Edt_EtMess = uint16_t(r->d[0]);
    w.Edt_EtAlert = r->a[0];
    w.Edt_EtatAff |= EtAAlert;
    status_line(w);
    r->a[4] = encode_context_cursor(ctx().a4);
    r->a[6] = address(ctx().a6);
}

// Ed_RAlert: the last alert again
void ed_recall_alert(AllRegs *r)
{
    r->d[0] = 127;
    const uint8_t *kept = ptr<uint8_t>(idata().Ed_BufT) + 256;
    if (peek_l(kept) != AlertMark)
        return;
    EditWindow &w = window();
    w.Edt_EtAlert = address(kept + 4);
    w.Edt_EtMess = 150;
    w.Edt_EtatAff |= EtAAlert;
    status_line(w);
}

void ed_warn_name(AllRegs *r) { warn_name(uint16_t(r->d[0]), ptr<uint8_t>(r->a[0])); }
void ed_warn_message(AllRegs *r) { warn_message(uint16_t(r->d[0])); }
void ed_warn_fn(AllRegs *r) { warn(ptr<uint8_t>(r->a[0])); }

// Ed_AverFin: the condition codes on d0 as the caller left it
void ed_warning_end(AllRegs *) { warning_end(); }
void ed_all_warnings_end(AllRegs *) { all_warnings_end(); }

void ed_status_line(AllRegs *)
{
    window().Edt_EtatAff = EtABAll;
    status_line(window());
}

void ed_status_line_d0(AllRegs *) { status_line(window()); }
void ed_status_text(AllRegs *r) { status_text(uint16_t(r->d[0])); }
void ed_status_order(AllRegs *) { status_order(); }
void ed_status_insert(AllRegs *) { status_insert(); }
void ed_status_y(AllRegs *) { status_y(); }
void ed_status_x(AllRegs *) { status_x(); }
void ed_status_free(AllRegs *) { status_free(); }
void ed_status_name(AllRegs *) { status_name(); }
void ed_name_to_buffer(AllRegs *r) { r->a[0] = address(name_into(ptr<uint8_t>(r->a[0]), program())); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_status.inc"
#endif
