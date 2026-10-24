// The stored block (Ed_Block, see editor/block.cpp) saved as a program or
// as ASCII, the program or the block printed (+Edit.s Ed_BlocSave,
// Ed_BlocSaveAscii, Ed_PrgPrint, Ed_BlocPrint, BlToA0/BlToA1).
//
// Files go through Handle(a5), as the editor's Ed_Open... do.

#include "editor/editor.h"
#include "core/source.h"

#include <dos/dos.h>

#define DOS_BASE_NAME (amos::ptr<struct DosLibrary>(amos::idata().DosBase))
#include <proto/dos.h>

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_block_what[], ed_file_selector[], ed_not_done[], ed_save_over[], ed_av_name[], ed_d_error[],
    ed_aver_fin[], ed_warn[], ed_print_error[], ed_header[], ed_get_message[];

namespace {

constexpr uint16_t BitControl = 1 << 8;
constexpr int EdDPBloc = 60, EdDPProg = 61;

void run(uint8_t *routine)
{
    Regs r;
    call(routine, r);
}

void run_d0(uint8_t *routine, uint32_t d0)
{
    Regs r;
    r.d[0] = d0;
    call(routine, r);
}

uint8_t *name1() { return ptr<uint8_t>(idata().Name1); }

bool select_file(uint16_t message)
{
    Regs r;
    r.d[0] = message;
    call(ed_file_selector, r);
    if (!uint16_t(r.d[0]))
        return false;
    path_it();
    return true;
}

// Ed_Open / Ed_Write / Ed_Close on Handle(a5); an error does not return
void open(const uint8_t *name, int32_t mode)
{
    BPTR f = Open(reinterpret_cast<CONST_STRPTR>(name), mode);
    idata().Handle = uint32_t(f);
    if (!f)
        run(ed_d_error);
}

void write(const void *data, int32_t length, uint8_t *error = ed_d_error)
{
    if (Write(BPTR(idata().Handle), const_cast<void *>(data), length) != length)
        run(error);
}

void close()
{
    IData &d = idata();
    if (BPTR f = BPTR(d.Handle)) {
        d.Handle = 0;
        Close(f);
    }
}

bool control_c()
{
    uint16_t &actualise = wdata().Actualise;
    if (!(actualise & BitControl))
        return false;
    actualise &= ~BitControl;
    return true;
}

void the_end()
{
    close();
    run(ed_aver_fin);
    Regs k;
    sy_call(sy::ClearKey, k);
}

// BlToA0 / BlToA1: the lines of the block as text, one by one (in
// Ed_BufT, the first and last line in their own place)
struct BlockText {
    const uint8_t *a2;
    int16_t d4;             // 1 the first line, 2 the lines between, -1 the last, 0 done
    int16_t d5;             // the lines between left
    const uint8_t *text;    // d2
    uint32_t length;        // d3

    explicit BlockText(const uint8_t *block) : a2(block + 10), d4(1), d5(0), text(nullptr), length(0) {}

    // false at the end; text and length the line (end = text + length)
    bool next()
    {
        uint8_t *buffer = ptr<uint8_t>(idata().Ed_BufT);
        for (;;) {
            length = 0;
            if (!d4)
                return finish();
            if (d4 < 2) {
                // the first or the last line, empty ones left out
                uint16_t n = peek_w(a2);
                if (!n) {
                    d4++;
                    a2 += 2;
                    continue;
                }
                for (uint16_t i = 0; i < n; i++)
                    buffer[i] = a2[2 + i];
                text = buffer;
                length = n;
                a2 = ptr<uint8_t>((address(a2) + 2 + n + 1) & ~1u);
                return finish();
            }
            if (d4 == 2) {
                d5 = int16_t(peek_w(a2));
                a2 += 6;
            }
            if (--d5 < 0) {
                d4 = -1;
                continue;
            }
            detok(a2, buffer);
            a2 = source::find_line(const_cast<uint8_t *>(a2), 1).line;
            length = peek_w(buffer);
            text = buffer + 2;
            return finish();
        }
    }

    // .Rien: d4 counts on, the line returned unless it was 0
    bool finish()
    {
        int16_t was = d4++;
        return was != 0;
    }
};

// Ed_BlocSaveAscii: the block as text, a line feed after each line
void save_ascii()
{
    IData &d = idata();
    tokenise_current_line();
    if (!d.Ed_Block)
        run(ed_block_what);
    if (!select_file(82))
        run(ed_not_done);
    run(ed_save_over);
    open(name1(), MODE_NEWFILE);
    Regs w;
    w.d[0] = 156;
    w.a[0] = d.Name1;
    call(ed_av_name, w);
    BlockText t(ptr<uint8_t>(d.Ed_Block));
    while (t.next()) {
        uint8_t *end = const_cast<uint8_t *>(t.text) + t.length;
        *end = 10;
        write(t.text, int32_t(t.length + 1));
    }
    close();
    run(ed_aver_fin);
}

// Ed_BlocSave: the block as a program (empty banks after it)
void save_program()
{
    IData &d = idata();
    tokenise_current_line();
    if (!d.Ed_Block)
        run(ed_block_what);
    if (!select_file(90))
        run(ed_not_done);
    run(ed_save_over);
    Regs w;
    w.d[0] = 154;
    w.a[0] = d.Name1;
    call(ed_av_name, w);
    open(name1(), MODE_NEWFILE);
    // the header (of the editor's own version), the length to come
    ed_header[11] = 'v';
    write(ed_header, 16);
    uint8_t *buffer = ptr<uint8_t>(d.Ed_BufT);
    write(buffer, 4);
    uint32_t size = 0;
    const uint8_t *a2 = ptr<uint8_t>(d.Ed_Block) + 10;
    auto line = [&](const uint8_t *text) {
        // (the text has no zero of its own: what follows it ends it)
        tokenise_line(const_cast<uint8_t *>(text), buffer);
        uint32_t n = uint32_t(buffer[0]) * 2;
        if (n) {
            size += n;
            write(buffer, int32_t(n));
        }
    };
    if (peek_w(a2))
        line(a2 + 2);
    a2 = ptr<uint8_t>((address(a2) + 2 + peek_w(a2) + 1) & ~1u);
    if (uint32_t middle = peek_l(a2 + 2)) {
        size += middle;
        write(a2 + 6, int32_t(middle));
    }
    a2 += 6 + peek_l(a2 + 2);
    if (peek_w(a2))
        line(a2 + 2);
    Regs b;
    lib_call(L_Bnk_SaveVide, b);
    Seek(BPTR(d.Handle), 16, OFFSET_BEGINNING);
    poke_l(buffer, size);
    write(buffer, 4);
    close();
    run(ed_aver_fin);
}

// Ed_PRTOpen: the printer (PRT:)
void printer_open()
{
    Regs m;
    m.d[0] = 43;
    m.a[0] = address(boot::sys_message(m.d[0]));
    BPTR f = Open(ptr<char>(m.a[0]), MODE_OLDFILE);
    idata().Handle = uint32_t(f);
    if (!f)
        run(ed_print_error);
}

// Ed_PRTPrint: a line to the printer; CR LF as just LF unless set so (the
// length written is the line's before that)
void printer_print(uint8_t *text)
{
    uint8_t *a1 = text;
    const uint8_t *a0 = text;
    for (;;) {
        uint8_t c = *a0++;
        *a1++ = c;
        if (!c)
            break;
        if (c == 13 && !idata().PI_PrtRet && *a0 == 10)
            a1[-1] = *a0++;
    }
    write(text, int32_t(a0 - text - 1), ed_print_error);
}

// Ed_PrgPrint
void print_program()
{
    tokenise_current_line();
    if (int16_t(dialogue(EdDPProg)) != 1)
        run(ed_not_done);
    run_d0(ed_warn, 217);
    printer_open();
    uint8_t *buffer = ptr<uint8_t>(idata().Ed_BufT);
    for (uint8_t *line = find_line(0); peek_w(line); line = next_line()) {
        if (control_c())
            break;
        detok(line, buffer);
        uint8_t *end = buffer + 2 + peek_w(buffer);
        end[0] = 13;
        end[1] = 10;
        end[2] = 0;
        printer_print(buffer + 2);
    }
    the_end();
}

// Ed_BlocPrint
void print_block()
{
    IData &d = idata();
    tokenise_current_line();
    if (!d.Ed_Block)
        run(ed_block_what);
    if (int16_t(dialogue(EdDPBloc)) != 1)
        run(ed_not_done);
    run_d0(ed_warn, 157);
    printer_open();
    BlockText t(ptr<uint8_t>(d.Ed_Block));
    while (t.next()) {
        if (control_c())
            break;
        uint8_t *end = const_cast<uint8_t *>(t.text) + t.length;
        end[0] = 13;
        end[1] = 10;
        end[2] = 0;
        printer_print(const_cast<uint8_t *>(t.text));
    }
    the_end();
}

} // namespace

extern "C" {
void ed_block_save(AllRegs *) { save_program(); }
void ed_block_save_ascii(AllRegs *) { save_ascii(); }
void ed_print_program(AllRegs *) { print_program(); }
void ed_print_block(AllRegs *) { print_block(); }
void ed_printer_open(AllRegs *) { printer_open(); }
void ed_printer_print(AllRegs *r) { printer_print(ptr<uint8_t>(r->a[0])); }

// Ed_BlocLimits: d1/d0 the block's start (line, column), d3/d2 its end
void ed_block_limits_2(AllRegs *r)
{
    EditWindow &w = window();
    if (int16_t(w.Edt_YBloc) < 0)
        run(ed_block_what);
    uint16_t y1 = w.Edt_YBloc, x1 = w.Edt_XBloc, y2 = w.Edt_YPos + w.Edt_YCu, x2 = w.Edt_XCu;
    if (!(y2 > y1 || (y2 == y1 && x2 >= x1))) {
        uint16_t t = y1;
        y1 = y2;
        y2 = t;
        t = x1;
        x1 = x2;
        x2 = t;
    }
    r->d[0] = (r->d[0] & 0xFFFF0000) | x1;
    r->d[1] = (r->d[1] & 0xFFFF0000) | y1;
    r->d[2] = (r->d[2] & 0xFFFF0000) | x2;
    r->d[3] = (r->d[3] & 0xFFFF0000) | y2;
}
}
