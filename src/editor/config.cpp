// The editor's configuration, resource bank and sounds (+Edit.s EdC_*,
// Ed_ResourceLoad, Ed_SamChanged, Ed_SamPlay, Ed_SetBanks).
//
// A configuration file: the length of the configuration zone of the data
// (Ed_DConfig to Ed_FConfig), the zone, then eight lists of texts, each
// its length (0 if none) and the texts.
// A list of texts (Ed_Systeme, Ed_Messages...): its length in the long
// before it, a zero, then for each text its length, the text and a zero;
// a length with bit 7 set ends it.

#include "editor/editor.h"

#include <stddef.h>
#include <dos/dos.h>

#include <dos/dos.h>

#include "hal/files.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s


namespace {

constexpr uint32_t ConfigSize = offsetof(IData, Ed_Code) + 4 - offsetof(IData, Ed_Sx);   // Ed_FConfig-Ed_DConfig
constexpr uint16_t EdDSvConf = 45, EdDCSaved = 46;
constexpr uint16_t BankSamples = 5, BankResource = 6;

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

[[noreturn]] void not_done()
{
    run(ed_not_done);
    __builtin_unreachable();
}

bool open(int32_t mode)
{
    BPTR f = hal::open(ptr<char>(idata().Name1), mode);
    idata().Handle = uint32_t(f);
    return f;
}

bool read(void *to, int32_t length) { return hal::read(BPTR(idata().Handle), to, length) == length; }

bool write(const void *from, int32_t length)
{
    return hal::write(BPTR(idata().Handle), const_cast<void *>(from), length) == length;
}

void close()
{
    IData &d = idata();
    if (BPTR f = BPTR(d.Handle)) {
        d.Handle = 0;
        hal::close(f);
    }
}

// Sys_AddPath: text with the system's path into Name1
void add_path(uint32_t text)
{
    Regs p;
    p.a[0] = text;
    boot::sys_add_path(ptr<char>(p.a[0]));
}

uint8_t *bank(uint32_t n)
{
    Regs r;
    r.d[0] = n;
    lib_call(L_Bnk_GetAdr, r);
    return ptr<uint8_t>(r.a[0]);
}

void bank_erase(uint32_t n)
{
    Regs r;
    r.d[0] = n;
    lib_call(L_Bnk_Eff, r);
}

// Bnk.Load from Handle(a5); false if it failed
bool bank_load(uint32_t n)
{
    Regs r;
    r.d[0] = n;
    lib_call(L_Bnk_Load, r);
    return !uint16_t(r.d[0]);
}

be_u32 *texts(int i)
{
    IData &d = idata();
    be_u32 *const lists[] = {&d.Ed_Systeme, &d.EdM_Messages, &d.Ed_Messages, &d.Ed_TstMessages,
                               &d.Ed_RunMessages, &d.Ed_MnPrograms, &d.EdM_User, &d.EdM_Definition};
    return lists[i];
}

} // namespace

namespace amos::ed {

// Ed_SetBanks: the editor's banks and dialogues the current ones
void set_banks()
{
    IData &d = idata();
    d.Cur_Banks = address(&d.Ed_Banks);
    d.Cur_Dialogs = address(&d.Ed_Dialogs);
}

// Ed_SamPlay: the editor's sample of letter c, if sounds are on
void sound_play(uint8_t c)
{
    IData &d = idata();
    if (!d.Ed_Sounds || d.Ed_Zappeuse)
        return;
    set_banks();
    uint8_t *samples = bank(BankSamples);
    if (!samples || peek_l(samples - 8) != 0x53616D70)         // "Samp"
        return;
    uint16_t n = peek_w(samples);
    for (uint16_t i = 0; i < n; i++) {
        uint8_t *s = samples + peek_l(samples + 2 + 4 * i);
        if (s[0] != c)
            continue;
        // the music extension's trap plays it
        uint32_t music = d.ExtAdr[0];
        if (!music)
            return;
        Regs r;
        r.d[1] = 0x0F;
        r.d[2] = peek_l(s + 10);
        r.d[3] = peek_w(s + 8);
        r.a[1] = address(s + 14);
        r.d[0] = r.a[0] = music;
        call(ptr<uint8_t>(music - 4), r);
        return;
    }
}

// Ed_GetNbMessage: the number of texts of a list
uint16_t text_count(const uint8_t *list)
{
    uint16_t n = 0;
    while (!(list[1] & 0x80)) {
        n++;
        list += 2 + list[1];
    }
    return n;
}

// Ed_GetFsMessage: the first empty text of a list (one after the last if
// none)
uint16_t text_free(const uint8_t *list)
{
    const uint8_t *a0 = list;
    for (uint16_t n = 1;; n++) {
        uint8_t length = a0[1];
        if (!length)
            return n;
        a0 += 2 + length;
        if (length & 0x80)
            return text_count(list) + 1;
    }
}

// EdC_ChangeTexte: text n of the list at *at replaced by text (added at
// the end if n is past it); false if out of memory
bool text_change(be_u32 *at, const uint8_t *text, uint16_t n)
{
    const uint8_t *list = ptr<uint8_t>(*at);
    uint32_t length = 0;
    while (text[length])
        length++;
    int32_t delta;
    if (n > text_count(list)) {
        delta = int32_t(2 + length);
    } else {
        uint8_t old;
        message_of(list, n, &old);
        delta = int16_t(uint16_t(length) - old);
    }
    uint32_t size = peek_l(list - 4) + uint32_t(delta);
    uint32_t m = memory(size + 4, true);
    if (!m)
        return false;
    poke_l(ptr<uint8_t>(m), size);
    uint8_t *to = ptr<uint8_t>(m + 4);
    const uint8_t *a1 = list + 1;
    *to++ = 0;
    // (the numbers are compared as bytes)
    int16_t want = int16_t(n);
    for (uint8_t number = 1;; number++) {
        uint8_t l = *a1;
        if (l & 0x80)
            break;
        const uint8_t *from = a1 + 1;
        a1 += 2 + l;
        if (want >= 0 && number == uint8_t(want)) {
            from = text;
            l = uint8_t(length);
            want = -1;
        }
        *to++ = l;
        for (uint8_t i = 0; i < l; i++)
            *to++ = *from++;
        *to++ = 0;
    }
    if (want >= 0) {
        // a new one, the last
        *to++ = uint8_t(length);
        for (uint8_t i = 0; i < uint8_t(length); i++)
            *to++ = text[i];
        *to++ = 0;
    }
    *to = 0xFF;
    memory_release(at);
    *at = m + 4;
    return true;
}

} // namespace amos::ed

namespace {

// EdC_SetPalette: the editor's colours into the resource's pictures
void set_palette()
{
    IData &d = idata();
    if (!d.Ed_Resource)
        return;
    uint8_t *graphics = ptr<uint8_t>(d.Ed_Resource) + peek_l(ptr<uint8_t>(d.Ed_Resource) + 2);
    uint8_t *palette = graphics + 6 + 4 * peek_w(graphics);
    for (int i = 0; i < 8; i++)
        poke_w(palette + 2 * i, d.Ed_Palette[i]);
}

// Ed_ResourceLoad: the resource bank of the file Name1; 0, 1 out of
// memory, 2 a disk error, 3 not a resource
uint32_t resource_load()
{
    set_banks();
    if (!open(MODE_OLDFILE))
        return 2;
    bool loaded = bank_load(BankResource);
    // (the assembler left the file open when it failed)
    close();
    if (!loaded)
        return 1;
    uint8_t *resource = bank(BankResource);
    if (!resource || peek_l(resource - 8) != 0x5265736F)       // "Reso"
        return 3;
    idata().Ed_Resource = address(resource);
    set_palette();
    return 0;
}

void resource_free()
{
    set_banks();
    bank_erase(BankResource);
}

// Ed_SamEnd / Ed_SamChanged: the samples' bank loaded or erased as the
// sounds are on or off
void sounds_end()
{
    set_banks();
    bank_erase(BankSamples);
}

void sounds_changed()
{
    IData &d = idata();
    set_banks();
    if (!d.Ed_Sounds) {
        if (bank(BankSamples))
            sounds_end();
        return;
    }
    if (bank(BankSamples))
        return;
    Regs m;
    m.d[0] = 48;
    call(ed_get_systeme, m);
    add_path(m.a[0]);
    if (!open(MODE_OLDFILE))
        return;
    set_banks();
    bank_load(BankSamples);
    close();
}

// EdC_LoadTextes / EdC_SaveTextes: one list of texts; false if it failed
bool load_texts(be_u32 *at)
{
    memory_release(at);
    uint8_t *buffer = ptr<uint8_t>(idata().Buffer);
    if (read(buffer, 4)) {
        uint32_t size = peek_l(buffer);
        if (!size)
            return true;
        if (uint8_t *m = memory_reserve(at, size))
            if (read(m, int32_t(size)))
                return true;
    }
    memory_release(at);
    return false;
}

bool save_texts(be_u32 *at)
{
    if (!*at)
        return write(at, 4);
    return write(ptr<uint8_t>(*at - 4), int32_t(peek_l(ptr<uint8_t>(*at - 4)) + 4));
}

// EdC_Load: the configuration of the file Name1; 0, 1 a disk error, 2 not
// a configuration, -1 a list of texts not loaded
int32_t load()
{
    IData &d = idata();
    int32_t result = 1;
    if (open(MODE_OLDFILE)) {
        uint8_t *buffer = ptr<uint8_t>(d.Buffer);
        uint8_t *zone = reinterpret_cast<uint8_t *>(&d.Ed_Sx);
        if (!read(buffer, 4)) {
            result = 1;
        } else if (peek_l(buffer) != ConfigSize) {
            result = 2;
        } else if (!read(zone, ConfigSize)) {
            result = 1;
        } else {
            d.Ed_Config = address(zone);
            result = 0;
            for (int i = 0; i < 8 && !result; i++)
                if (!load_texts(texts(i)))
                    result = -1;
            if (!result) {
                // the columns of the parts of the status line: the digits
                // of system text 1
                const uint8_t *line = system_message(1);
                for (uint8_t x = 0; line[x]; x++) {
                    uint8_t part = uint8_t(line[x] - '0');
                    if (line[x] != ' ' && part < 8)
                        d.Ed_EtXX[part] = x;
                }
            }
        }
    }
    close();
    return result;
}

// EdC_Save: the configuration into the file Name1; 0, 1 a disk error, -1
// a list of texts not saved
int32_t save()
{
    IData &d = idata();
    int32_t result = 1;
    if (open(MODE_NEWFILE)) {
        uint8_t *buffer = ptr<uint8_t>(d.Buffer);
        poke_l(buffer, ConfigSize);
        if (write(buffer, 4) && write(&d.Ed_Sx, ConfigSize)) {
            result = 0;
            for (int i = 0; i < 8 && !result; i++)
                if (!save_texts(texts(i)))
                    result = -1;
        }
    }
    close();
    return result;
}

// EdC_Free
void free_texts()
{
    for (int i = 0; i < 8; i++)
        memory_release(texts(i));
}

bool select_file(uint16_t message)
{
    Regs r;
    r.d[0] = message;
    if (!uint16_t(call(ed_file_selector, r)))
        return false;
    path_it();
    return true;
}

// Sys_GetMessage 7 (the configuration's name) with the system's path
void default_name()
{
    Regs m;
    m.d[0] = 7;
    m.a[0] = address(boot::sys_message(m.d[0]));
    add_path(m.a[0]);
}

// EdC_SaveIt
void save_it()
{
    run_d0(ed_warn, 152);
    if (save())
        run_d0(ed_alert_100, 139);
    run(ed_aver_fin);
    idata().EdC_Changed = 0;
}

void save_as()
{
    tokenise_current_line();
    if (!select_file(130))
        not_done();
    run(ed_save_over);
    save_it();
}

void save_default_quiet()
{
    default_name();
    save_it();
}

void save_default()
{
    tokenise_current_line();
    if (int16_t(dialogue(EdDSvConf)) != 1)
        not_done();
    save_default_quiet();
}

// EdC_Redraw: the whole editor closed and opened again
void redraw()
{
    sounds_changed();
    run(ed_hide);
    run(ed_close_editor);
    idata().EdC_Modified = 1;
    run(ed_open_editor);
    run(ed_appear);
}

// Ed_TiWait: until a key, a button or n frames
void wait_input(uint16_t n)
{
    do {
        boot::wait_vbl_multi();
        Regs k;
        sy_call(sy::Inkey, k);
        if (k.d[1])
            return;
        Regs m;
        sy_call(sy::MouseKey, m);
        if (m.d[1] & 7)
            return;
    } while (--n);
}

// EdC_LoadIt
void load_it()
{
    run_d0(ed_warn, 153);
    if (load())
        run_d0(ed_alert_100, 139);
    run(ed_aver_fin);
    run_d0(ed_warn, 218);
    wait_input(75);
    redraw();
}

// EdC_Saved: the configuration saved first if it was changed
void save_changed()
{
    if (idata().EdC_Changed != 1)
        return;
    int16_t answer = int16_t(dialogue(EdDCSaved));
    if (answer == 2)
        return;
    if (answer != 1)
        not_done();
    save_as();
}

void load_default_quiet()
{
    default_name();
    load_it();
    idata().EdC_Changed = 2;
}

void load_default()
{
    tokenise_current_line();
    save_changed();
    load_default_quiet();
}

void load_as()
{
    tokenise_current_line();
    save_changed();
    if (!select_file(134))
        not_done();
    load_it();
    idata().EdC_Changed = 2;
}

} // namespace

extern "C" {
void ed_set_banks_fn(AllRegs *) { set_banks(); }
void ed_sound_play(AllRegs *r) { sound_play(uint8_t(r->d[0])); }
void ed_sounds_changed(AllRegs *) { sounds_changed(); }
void ed_sounds_end(AllRegs *) { sounds_end(); }
void ed_resource_load(AllRegs *r) { r->d[0] = resource_load(); }
void ed_resource_free(AllRegs *) { resource_free(); }
void ed_config_set_palette(AllRegs *) { set_palette(); }
void ed_config_load(AllRegs *r) { r->d[0] = uint32_t(load()); }
void ed_config_save(AllRegs *r) { r->d[0] = uint32_t(save()); }
void ed_config_free(AllRegs *) { free_texts(); }
void ed_config_save_default(AllRegs *) { save_default(); }
void ed_config_save_default_quiet(AllRegs *) { save_default_quiet(); }
void ed_config_save_as(AllRegs *) { save_as(); }
void ed_config_load_default(AllRegs *) { load_default(); }
void ed_config_load_default_quiet(AllRegs *) { load_default_quiet(); }
// The default configuration loaded, quietly (its messages, for run only's
// errors: main/modules.cpp); true if it was
bool ed_config_load_texts()
{
    default_name();
    return !load();
}
void ed_config_load_as(AllRegs *) { load_as(); }
void ed_config_redraw(AllRegs *) { redraw(); }
void ed_config_save_changed(AllRegs *) { save_changed(); }
void ed_wait_input(AllRegs *r) { wait_input(uint16_t(r->d[0])); }

// a0 the list's long, a1 the text, d0 its number; d0 0 or -1
void ed_text_change(AllRegs *r)
{
    r->d[0] = text_change(ptr<be_u32>(r->a[0]), ptr<uint8_t>(r->a[1]), uint16_t(r->d[0])) ? 0 : 0xFFFFFFFF;
}

void ed_text_count(AllRegs *r) { r->d[0] = text_count(ptr<uint8_t>(r->a[0])); }
void ed_text_free(AllRegs *r) { r->d[0] = text_free(ptr<uint8_t>(r->a[0])); }
}

#if !defined(__mc68000__)
#include "runtime/bindings/editor_config.inc"
#endif
