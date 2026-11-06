// src_w_windows_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_w_windows_glue_s[168] = {
};
}

namespace amos::host {

void relocate_src_w_windows_glue_s()
{
}

const GlueEntry entries_src_w_windows_glue_s[] = {
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 0), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 80), 0, 0, "w_wi_in"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 4), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 84), 0, 0, "WPrint"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 8), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 88), 0, 0, "WCentre"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 12), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 92), 0, 0, "w_wopen"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 16), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 96), 0, 0, "WLocate"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 20), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 100), 0, 0, "WQWind"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 24), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 104), 0, 0, "WDel"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 28), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 108), 0, 0, "WSBor"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 32), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 112), 0, 0, "WSTit"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 36), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 116), 0, 0, "WAdr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 40), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 120), 0, 0, "WiMove"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 44), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 124), 0, 0, "WiCls"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 48), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 128), 0, 0, "WiSize"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 52), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 132), 0, 0, "WiSCur"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 56), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 136), 0, 0, "WiXYCu"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 60), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 140), 0, 0, "WiXGr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 64), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 144), 0, 0, "WiYGr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 68), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 148), 0, 0, "WPrint2"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 72), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 152), 0, 0, "WPrint3"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 76), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 156), 0, 0, "WiXYWi"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 80), GlueKind::window, nullptr, nullptr, 0, 4, "WOutC"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 84), GlueKind::window, nullptr, nullptr, 1, 4, "WPrint"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 88), GlueKind::window, nullptr, nullptr, 2, 2, "WCentre"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 92), GlueKind::window, nullptr, nullptr, 3, 4, "w_wopen"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 96), GlueKind::window, nullptr, nullptr, 4, 4, "WLocate"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 100), GlueKind::window, nullptr, nullptr, 5, 4, "WQWind"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 104), GlueKind::window, nullptr, nullptr, 6, 4, "WDel"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 108), GlueKind::window, nullptr, nullptr, 7, 4, "WSBor"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 112), GlueKind::window, nullptr, nullptr, 8, 4, "WSTit"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 116), GlueKind::window, nullptr, nullptr, 9, 4, "WAdr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 120), GlueKind::window, nullptr, nullptr, 10, 4, "WiMove"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 124), GlueKind::window, nullptr, nullptr, 11, 4, "WiCls"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 128), GlueKind::window, nullptr, nullptr, 12, 4, "WiSize"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 132), GlueKind::window, nullptr, nullptr, 13, 4, "WiSCur"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 136), GlueKind::window, nullptr, nullptr, 14, 4, "WiXYCu"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 140), GlueKind::window, nullptr, nullptr, 15, 4, "WiXGr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 144), GlueKind::window, nullptr, nullptr, 16, 4, "WiYGr"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 148), GlueKind::window, nullptr, nullptr, 17, 4, "WPrint2"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 152), GlueKind::window, nullptr, nullptr, 18, 2, "WPrint3"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 156), GlueKind::window, nullptr, nullptr, 19, 4, "WiXYWi"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 160), GlueKind::window, nullptr, nullptr, 20, 4, "w_widela"},
    {AMOS_ENTRY_AT(host_obj_src_w_windows_glue_s, 164), GlueKind::missing, nullptr, nullptr, 0, 0, "WiGo"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
