// Data and Read (+ILib.s). PData is the line after the Data line being
// read, AData the next item in it (0: find the next Data line). A Data line
// may start with a label. An empty item reads as 0 (or an empty string).

#include "interp/interp.h"

using namespace amos;
using namespace amos::interp;

namespace {

// Data: the rest of the line skipped (the word after the token goes back
// to the line's start)
void data()
{
    uint8_t *line = pc() - peek_w(pc());
    pc() = line - 2 + 2 * line[0];
}

[[noreturn]] void out_of_data()
{
    idata().PData = 0;
    lib_jump(L_OOfData, 0);
}

// The next item to read, pc there
void next_item()
{
    IData &d = idata();
    if (d.AData) {
        pc() = ptr<uint8_t>(d.AData);
        return;
    }
    uint8_t *line = ptr<uint8_t>(d.PData);
    if (!line)
        out_of_data();
    for (;;) {
        if (!peek_w(line))
            out_of_data();
        uint8_t *t = line + 2;
        uint16_t token = peek_w(t);
        if (token == tk::EndP)
            out_of_data();
        if (token == tk::Proc) {
            line = t + 10 + peek_l(t + 2);
            continue;
        }
        if (token == tk::Lab) {
            t = line + 8 + int8_t(line[6]);
            token = peek_w(t);
        }
        if (token == tk::Data) {
            d.PData = address(line + 2 * line[0]);
            pc() = t + 4;
            return;
        }
        line += 2 * line[0];
    }
}

// Read v,...
void read()
{
    IData &d = idata();
    do {
        FoundVar var = find_variable();
        uint8_t type = uint8_t(var.type);
        uint8_t *back = pc();
        next_item();
        Value v;
        uint16_t t = peek_token();
        if (!t || t == tk::Vir) {
            v = type >= TString ? Value{TString, d.ChVide, 0} : Value{TInt, 0, 0};
        } else {
            v = evaluate();
            t = peek_token();
        }
        pc() += 2;
        d.AData = t ? address(pc()) : 0;
        pc() = back;
        if (kind(v) != type) {
            if (type == TString || kind(v) == TString)
                error(34);
            v = type == TInt ? to_integer(v) : to_float(v);
        }
        if (kind(v) == TFloat && int8_t(d.MathFlags) < 0)
            poke_l(var.at + 4, v.v2);
        poke_l(var.at, v.v1);
    } while (next_token() == tk::Vir);
    pc() -= 2;
}

} // namespace

extern "C" {
void interp_in_data() { data(); }
void interp_in_read() { read(); }
}
