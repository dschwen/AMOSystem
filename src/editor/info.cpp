// The editor's information dialogues (+Edit.s Ed_About, Ed_AboutExt,
// Ed_Infos) and the quit options (Ed_QuitOptions).

#include "editor/editor.h"
#include "core/amiga.h"
#include "hal/system.h"

using namespace amos;
using namespace amos::ed;

// +Edit.s
extern "C" uint8_t ed_test_program[];

namespace {

constexpr uint16_t EdDTitle = 0, EdDInfos = 54, EdDAboutE = 55, EdDOQuit = 49;
constexpr int Extensions = 26;
constexpr int LB_Title = -16;

be_u32 *dialogue_variables() { return ptr<be_u32>(idata().Ed_VDialogues); }

// Sys_UnCode: the string at from (a length word, the text) decoded with
// key into to
void uncode(const uint8_t *from, uint8_t *to, uint8_t key)
{
    Regs r;
    r.a[0] = address(from);
    r.a[1] = address(to);
    r.d[0] = key;
    lib_call(L_Sys_UnCode, r);
}

// Ed_About: the version, the number of extensions, the registered user
void about()
{
    IData &d = idata();
    uint8_t *version = dialogue_string(static_cast<const uint8_t *>(lib_address(L_VersionN)));
    be_u32 *v = dialogue_variables();
    v[0] = address(version);
    uint32_t extensions = 0;
    for (int i = 1; i <= Extensions; i++)
        if (d.AdTokens[i])
            extensions++;
    v[1] = extensions;
    const uint8_t *user = static_cast<const uint8_t *>(lib_address(L_UserReg));
    uint8_t *name = ptr<uint8_t>(d.Name2);
    uncode(user, name, 0x73);
    v[3] = address(name);
    uncode(user + 16, name + 16, 0xA5);
    v[2] = address(name + 16);
    dialogue(EdDTitle);
}

// Ed_AboutExt: the loaded extensions, one by one (Previous / Next)
void about_extensions()
{
    IData &d = idata();
    static const uint16_t empty = 0;
    int n = 0;
    auto next = [&] {
        for (int i = n + 1; i <= Extensions; i++)
            if (d.AdTokens[i]) {
                n = i;
                return;
            }
    };
    auto previous = [&] {
        for (int i = n - 1; i >= 1; i--)
            if (d.AdTokens[i]) {
                n = i;
                return;
            }
    };
    next();
    if (!n)
        return;
    for (;;) {
        be_u32 *v = dialogue_variables();
        v[0] = uint32_t(n);
        uint32_t title = peek_l(ptr<uint8_t>(d.AdTokens[n] + LB_Title));
        v[1] = title ? title : address(&empty);
        int16_t answer = int16_t(dialogue(EdDAboutE));
        if (answer == 1)
            previous();
        else if (answer == 2)
            next();
        else
            return;
    }
}

// Ed_Infos: the memory, the sizes of the program and the banks
void information()
{
    tokenise_current_line();
    Regs t;
    call(ed_test_program, t);
    Program &p = program();
    be_u32 *v = dialogue_variables();
    v[0] = hal::available_memory(hal::MemoryChip);
    v[1] = hal::available_memory(hal::MemoryFast);
    v[2] = p.Prg_StHaut - p.Prg_StBas;
    Regs b;
    lib_call(L_Bnk_GetLength, b);
    v[3] = b.d[0] + b.d[1] + b.d[2];
    v[6] = b.d[2];
    v[4] = p.Prg_NLigne;
    // (the bobs' size, put in 5 first, is replaced by the instructions)
    v[5] = idata().VerNInst;
    dialogue(EdDInfos);
}

// Ed_QuitOptions: what quitting asks and saves
void quit_options()
{
    IData &d = idata();
    tokenise_current_line();
    Regs s;
    s.a[0] = d.Ed_VDialogues;
    s.d[0] = d.Ed_QuitFlags;
    s.d[1] = 4;
    lib_call(L_Dia_SetVFlags, s);
    if (int16_t(dialogue(EdDOQuit)) != 1) {
        place_cursor();
        return;
    }
    Regs g;
    g.a[0] = d.Ed_VDialogues;
    g.d[1] = 4;
    lib_call(L_Dia_GetVFlags, g);
    d.Ed_QuitFlags = uint8_t(g.d[0]);
    d.EdC_Changed = 1;
}

} // namespace

extern "C" {
void ed_about(AllRegs *) { about(); }
void ed_about_extensions(AllRegs *) { about_extensions(); }
void ed_information(AllRegs *) { information(); }
void ed_quit_options(AllRegs *) { quit_options(); }
}
