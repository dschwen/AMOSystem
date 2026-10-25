// The ARexx instructions of AMOSPro.Lib (IO.s): a message port that ARexx
// scripts send commands to, and the answers.

#include "core/amiga.h"
#include "lib/dos.h"
#include "lib/strings.h"

#include <rexx/errors.h>
#include <rexx/storage.h>

#define REXXSYSLIB_BASE_NAME (amos::ptr<struct Library>(amos::idata().Arx_Base))
#include <proto/rexxsyslib.h>
#include "hal/system.h"

using namespace amos;

// devices.cpp, dos_glue.s
extern "C" struct MsgPort *lib_create_port(const char *name, int32_t priority);
extern "C" void lib_delete_port(struct MsgPort *port);
extern "C" uint8_t lib_arexx_on_clear[];

namespace {

struct RexxMsg *answer() { return ptr<struct RexxMsg>(idata().Arx_Answer); }

// Arx_RegisterPort: is there a port of that name? If not and create, make
// it (*port).
bool port_exists(const char *name, bool create, struct MsgPort **port)
{
    hal::forbid();
    bool exists = hal::find_port(const_cast<STRPTR>(name));
    if (!exists && create)
        *port = lib_create_port(name, 0);
    hal::permit();
    return exists;
}

// Arx_Message: true when a command waits (Arx_Answer)
bool message()
{
    IData &d = idata();
    if (!d.Arx_Base)
        return false;
    if (d.Arx_Answer)
        return true;
    struct Message *m = hal::get_message(ptr<struct MsgPort>(d.Arx_Port));
    if (!m)
        return false;
    d.Arx_Answer = address(m);
    if (IsRexxMsg(reinterpret_cast<struct RexxMsg *>(m))
        && (reinterpret_cast<struct RexxMsg *>(m)->rm_Action & RXCODEMASK) == RXCOMM)
        return true;
    // (The assembler dropped other messages without replying: their sender
    // waited for ever)
    d.Arx_Answer = 0;
    hal::reply_message(m);
    return false;
}

void fatal_reply(struct Message *m)
{
    reinterpret_cast<struct RexxMsg *>(m)->rm_Result1 = RC_FATAL;
    reinterpret_cast<struct RexxMsg *>(m)->rm_Result2 = 0;
    hal::reply_message(m);
}

} // namespace

// Arx_Close: answer what waits, remove the port, close rexxsyslib
extern "C" void lib_arx_close()
{
    IData &d = idata();
    if (!d.Arx_Base)
        return;
    if (d.Arx_Port) {
        if (struct Message *m = ptr<struct Message>(d.Arx_Answer)) {
            d.Arx_Answer = 0;
            fatal_reply(m);
        }
        hal::forbid();
        while (struct Message *m = hal::get_message(ptr<struct MsgPort>(d.Arx_Port)))
            fatal_reply(m);
        struct MsgPort *port = ptr<struct MsgPort>(d.Arx_Port);
        d.Arx_Port = 0;
        lib_delete_port(port);
        hal::permit();
    }
    struct Library *base = ptr<struct Library>(d.Arx_Base);
    d.Arx_Base = 0;
    CloseLibrary(base);
}

// =AREXX EXIST(port$)
extern "C" uint32_t lib_fn_arexx_exist(const uint8_t *name)
{
    struct MsgPort *unused = nullptr;
    return port_exists(str_to_buffer(name), false, &unused) ? -1u : 0;
}

// AREXX OPEN port$
extern "C" void lib_in_arexx_open(const uint8_t *name)
{
    IData &d = idata();
    uint16_t length = str_len(name);
    if (length >= 32)
        error(21);
    char *to = reinterpret_cast<char *>(d.Arx_PortName);
    for (uint16_t i = 0; i < length; i++) {
        int8_t c = static_cast<int8_t>(str_chars(name)[i]);
        to[i] = c;
        if (c <= ' ')
            fon_call();
    }
    to[length] = 0;
    if (d.Arx_Base)
        error(193);         // already open
    d.Arx_Base = address(OpenLibrary(const_cast<STRPTR>("rexxsyslib.library"), 0));
    if (!d.Arx_Base)
        error(194);         // no rexxsyslib.library
    struct MsgPort *port = nullptr;
    port_exists(to, true, &port);
    d.Arx_Port = address(port);
    if (!port) {
        lib_arx_close();
        error(195);         // the port is there already
    }
    add_routine(lib_arexx_on_clear, address(&d.Sys_ClearRoutines));
}

// =AREXX: 0 nothing, 1 a command, 2 a command that wants an answer string
extern "C" uint32_t lib_fn_arexx()
{
    if (!message())
        return 0;
    return answer()->rm_Action & RXFF_RESULT ? 2 : 1;
}

// AREXX WAIT
extern "C" void lib_in_arexx_wait()
{
    do {
        boot::wait_vbl_multi();
        Regs t = {};
        lib_call(L_Test_Normal, t);
    } while (!message());
}

// AREXX CLOSE
extern "C" void lib_in_arexx_close()
{
    if (idata().Arx_Answer)
        error(198);         // a command still waits for its answer
    lib_arx_close();
}

// =AREXX$(n): argument n of the command
extern "C" uint32_t lib_fn_arexxd(uint32_t n)
{
    if (n >= 16)
        fon_call();
    struct RexxMsg *m = answer();
    if (!m || !m->rm_Args[n])
        return str_empty();
    const uint8_t *arg = reinterpret_cast<const uint8_t *>(m->rm_Args[n]);
    uint16_t length = peek_w(arg - 4);      // ra_Length
    return length ? str_from(arg, length) : str_empty();
}

// AREXX ANSWER code[,answer$]
extern "C" void lib_in_arexx_answer2(uint32_t code, const uint8_t *text)
{
    struct RexxMsg *m = answer();
    if (!m)
        error(197);         // no command
    if (m->rm_Action & RXFF_RESULT) {
        UBYTE *s = CreateArgstring(const_cast<STRPTR>(reinterpret_cast<const char *>(str_chars(text))), str_len(text));
        if (!s)
            error(24);
        m->rm_Result2 = reinterpret_cast<LONG>(s);
    }
    m->rm_Result1 = static_cast<LONG>(code);
    hal::reply_message(&m->rm_Node);
    idata().Arx_Answer = 0;
}

extern "C" void lib_in_arexx_answer1(uint32_t code) { lib_in_arexx_answer2(code, ptr<uint8_t>(idata().ChVide)); }
