// Host system clipboard: text adapters and a compound undo record. Guest layouts
// and the original Amiga block commands remain unchanged.
#if !defined(__m68k__)
#include "editor/editor.h"
#include "editor/clipboard.h"
#include "host/clipboard_text.h"
#include <string>
#include <vector>
#include <string.h>

using namespace amos;
using namespace amos::ed;
namespace {
constexpr unsigned Header = 60;
struct Line { std::string text; const uint8_t *tokens; uint32_t bytes; bool editable; };
void tell(const char *s) { hal::clipboard_message(s); }
void snapshot(uint8_t *to, const uint8_t *tokens, uint32_t size, uint16_t count,
              uint16_t y, uint16_t x)
{
    memset(to, 0, Header);
    poke_l(to, size); poke_w(to + 4, count); poke_w(to + 6, y); poke_w(to + 8, x);
    poke_w(to + 10, window().Edt_XBloc); poke_w(to + 12, window().Edt_YBloc);
    memcpy(to + 16, program().Prg_Marks, 40);
    memcpy(to + Header, tokens, size);
}
void apply(const uint8_t *s)
{
    Program &p = program(); EditWindow &w = window();
    const uint32_t size = peek_l(s);
    p.Prg_StBas = p.Prg_StHaut - size;
    memcpy(ptr<uint8_t>(p.Prg_StBas), s + Header, size);
    p.Prg_NLigne = peek_w(s + 4);
    memcpy(p.Prg_Marks, s + 16, 40);
    p.Prg_Change = 1; p.Prg_StModif = 1;
    w.Edt_LEdited = 0; w.Edt_EtatAff |= 1 << 5;
    w.Edt_YBloc = peek_w(s + 12); w.Edt_XBloc = peek_w(s + 10);
    w.Edt_YOldBloc = 0xFFFF;
    goto_y(peek_w(s + 6)); goto_x(peek_w(s + 8));
    new_buffer_all();
}
}
namespace amos::ed {
bool clipboard_mirror_block();
void clipboard_restore(uint8_t *block, bool redo)
{
    const uint8_t *s = block + 4;
    if (redo) s += Header + peek_l(s);
    // Program buffer may have been resized by another editor operation.
    if (peek_l(s) >= uint32_t(program().Prg_StHaut - program().Prg_StMini)) {
        tell("Clipboard undo: program buffer is too small"); return;
    }
    apply(s);
}
bool system_clipboard(uint32_t key)
{
    auto command = hal::clipboard_command(key);
    if (command == hal::ClipboardCommand::None) return false;
    system_clipboard_command(command);
    return true;
}
void system_clipboard_command(hal::ClipboardCommand command)
{
    using hal::ClipboardCommand;
    if (command == ClipboardCommand::None || command == ClipboardCommand::Ignored) return;
    std::string pasted;
    if (command == ClipboardCommand::Paste) {
        char *text;
        if (!hal::clipboard_read(text)) { tell("System clipboard has no available text"); return; }
        bool valid = host::clipboard::decode(text, pasted);
        hal::clipboard_release(text);
        if (!valid) { tell("Clipboard: invalid/unrepresentable text or more than 1 MiB"); return; }
        if (pasted.empty()) return;
    }
    EditWindow &w = window(); Program &p = program();
    unsigned y = uint16_t(w.Edt_YPos + w.Edt_YCu), x = w.Edt_XCu;
    unsigned y1 = y, y2 = y, x1 = x, x2 = x;
    bool selected = int16_t(w.Edt_YBloc) >= 0;
    if (selected) {
        y1 = w.Edt_YBloc; x1 = w.Edt_XBloc;
        if (y1 > y2 || (y1 == y2 && x1 > x2)) {
            unsigned t = y1; y1 = y2; y2 = t; t = x1; x1 = x2; x2 = t;
        }
    }
    if (command != ClipboardCommand::Paste && (!selected || (y1 == y2 && x1 == x2))) return;
    // Flush the current edit buffer using the editor's established tokenizer.
    tokenise_current_line();
    uint32_t scratch_addr = memory(2048, true);
    if (!scratch_addr) { tell("Clipboard: not enough temporary memory"); return; }
    // No guest calls that can abandon this scope while scratch is owned.
    uint8_t *scratch = ptr<uint8_t>(scratch_addr);
    std::vector<Line> lines;
    const uint8_t *at = ptr<uint8_t>(p.Prg_StBas);
    size_t text_size = 0;
    while (peek_w(at)) {
        detok(at, scratch);
        unsigned length = peek_w(scratch);
        text_size += length + 1;
        if (length > 250 || text_size > host::clipboard::Limit) break;
        uint32_t bytes = source::line_size(at);
        lines.push_back({std::string(reinterpret_cast<char *>(scratch + 2), length), at, bytes, source::editable(at)});
        at += bytes;
    }
    lines.push_back({"", at, 2, true}); // Editable end-of-program line.
    auto finish = [&] { free_memory(scratch_addr, 2048); };
    if (text_size > host::clipboard::Limit || peek_w(at) || y2 >= lines.size()) {
        finish(); tell("Clipboard: source is too large or selection is invalid"); return;
    }
    x1 = unsigned(x1 < lines[y1].text.size() ? x1 : lines[y1].text.size());
    x2 = unsigned(x2 < lines[y2].text.size() ? x2 : lines[y2].text.size());
    for (unsigned i = y1; i <= y2; ++i) {
        if (!lines[i].editable) { finish(); tell("Clipboard: unfold closed procedures before copying or editing"); return; }
    }
    if (command != ClipboardCommand::Paste) {
        std::string selected_text, utf8;
        for (unsigned i = y1; i <= y2; ++i) {
            unsigned start = i == y1 ? x1 : 0;
            unsigned end = i == y2 ? x2 : unsigned(lines[i].text.size());
            selected_text.append(lines[i].text, start, end - start);
            if (i != y2) selected_text += '\n';
        }
        if (!host::clipboard::encode(selected_text, utf8)) {
            finish(); tell("Clipboard: selection contains unsupported control bytes or is too large"); return;
        }
        if (!hal::clipboard_write(utf8.c_str())) { finish(); tell("System clipboard write failed; source unchanged"); return; }
        clipboard_mirror_block();
        if (command == ClipboardCommand::Copy) { finish(); return; }
    }
    if (!p.Prg_Undo || idata().Ed_FUndo) {
        finish(); tell("Clipboard edit needs an enabled undo buffer"); return;
    }
    std::string replacement = lines[y1].text.substr(0, x1) + pasted + lines[y2].text.substr(x2);
    std::vector<uint8_t> tokens;
    unsigned added_lines = 0, cursor_y = y1, cursor_x = x1;
    for (char c : pasted) {
        if (c == '\n') { ++cursor_y; cursor_x = 0; } else ++cursor_x;
    }
    size_t start = 0;
    for (;;) {
        size_t end = replacement.find('\n', start);
        if (end == std::string::npos) end = replacement.size();
        size_t length = end - start;
        if (length > 250) { finish(); tell("Clipboard: resulting line exceeds 250 bytes"); return; }
        memcpy(scratch + 1024, replacement.data() + start, length); scratch[1024 + length] = 0;
        if (tokenise_line(scratch + 1024, scratch) < 0) {
            finish(); tell("Clipboard: resulting tokenized line is too long"); return;
        }
        // The final empty end-of-program line is represented by the terminator.
        if (!(y2 == lines.size() - 1 && end == replacement.size() && !length)) {
            tokens.insert(tokens.end(), scratch, scratch + unsigned(scratch[0]) * 2);
            ++added_lines;
        }
        if (end == replacement.size()) break;
        start = end + 1;
    }
    const uint8_t *old = ptr<uint8_t>(p.Prg_StBas);
    const uint8_t *last = lines[y2].tokens + (y2 == lines.size() - 1 ? 0 : lines[y2].bytes);
    const uint8_t *old_end = ptr<uint8_t>(p.Prg_StHaut);
    std::vector<uint8_t> candidate(old, lines[y1].tokens);
    candidate.insert(candidate.end(), tokens.begin(), tokens.end());
    candidate.insert(candidate.end(), last, old_end);
    unsigned removed = y2 - y1 + (y2 < lines.size() - 1 ? 1 : 0);
    int count = int(p.Prg_NLigne) + int(added_lines) - int(removed);
    if (count < 0 || count > 32760 || candidate.size() >= uint32_t(p.Prg_StHaut - p.Prg_StMini)) {
        finish(); tell("Clipboard: program buffer or line count limit exceeded"); return;
    }
    uint32_t old_size = p.Prg_StHaut - p.Prg_StBas;
    uint32_t record_size = 4 + Header * 2 + old_size + candidate.size();
    uint32_t record_addr = memory(record_size, true);
    if (!record_addr) { finish(); tell("Clipboard: insufficient memory for undo; source unchanged"); return; }
    uint8_t *record = ptr<uint8_t>(record_addr);
    poke_l(record, record_size);
    snapshot(record + 4, old, old_size, p.Prg_NLigne, y, x);
    uint8_t *after = record + 4 + Header + old_size;
    snapshot(after, candidate.data(), candidate.size(), uint16_t(count), cursor_y, cursor_x);
    poke_w(after + 12, 0xFFFF);
    // Compute mark changes without touching the live document.
    for (unsigned i = 0; i < 10; ++i) {
        uint32_t mark = peek_l(after + 16 + i * 4);
        if (!mark) continue;
        unsigned line = mark >> 16;
        if (line >= y1 && line <= y2) poke_l(after + 16 + i * 4, 0);
        else if (line > y2) poke_w(after + 16 + i * 4, uint16_t(int(line) + int(added_lines) - int(removed)));
    }
    finish();
    uint8_t *entry = undo_start();
    if (!entry) { free_memory(record_addr, record_size); tell("Clipboard: undo unavailable; source unchanged"); return; }
    entry[0] = 0x89; poke_l(entry + 2, record_addr); p.Prg_TUndo += record_size;
    apply(after);
}
}
#endif
