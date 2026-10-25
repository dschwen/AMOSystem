// hal/workbench.h on the Amiga: intuition.library, icon.library,
// console.device, rexxsyslib.library

#include "hal/workbench.h"

#include "core/amos.h"
#include "core/amiga.h"

#define INTUITION_BASE_NAME (amos::ptr<struct IntuitionBase>(amos::wdata().IntBase))
#include <proto/intuition.h>
#define ICON_BASE_NAME (amos::ptr<struct Library>(amos::idata().IconBase))
#include <proto/icon.h>
#include <rexx/storage.h>
#define REXXSYSLIB_BASE_NAME (amos::ptr<struct Library>(amos::idata().Arx_Base))
#include <proto/rexxsyslib.h>

namespace {
struct Device *console_base;
}
#define CONSOLE_BASE_NAME console_base
#include <proto/console.h>

// (outside the namespace: Intuition's Screen, not AMOS's)
static ::Screen *intuition_open_screen(NewScreen *ns) { return OpenScreen(ns); }
static void intuition_close_screen(::Screen *s) { CloseScreen(s); }
::Screen *amos::hal::open_screen(NewScreen *ns) { return intuition_open_screen(ns); }
void amos::hal::close_screen(::Screen *s) { intuition_close_screen(s); }

namespace amos::hal {

bool open_workbench() { return OpenWorkBench(); }
bool close_workbench() { return CloseWorkBench(); }

bool put_icon(const char *name, DiskObject *icon) { return PutDiskObject(name, icon); }

int32_t key_to_text(InputEvent *event, char *buffer, int32_t length, void *console)
{
    console_base = static_cast<struct Device *>(console);
    return RawKeyConvert(event, buffer, length, nullptr);
}

bool is_rexx_message(Message *m) { return IsRexxMsg(reinterpret_cast<struct RexxMsg *>(m)); }
uint8_t *rexx_text(const char *text, uint32_t length) { return CreateArgstring(const_cast<STRPTR>(text), length); }

} // namespace amos::hal
