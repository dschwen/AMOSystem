// _Monitor_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj__Monitor_s[820] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,27,67,48,27,
    66,50,27,80,51,25,27,86,48,0,0,0,66,65,9,48,44,48,59,83,73,9,83,87,
    44,83,72,59,76,73,9,48,44,48,44,57,52,44,83,88,49,54,45,59,76,73,9,48,
    44,89,66,32,48,86,65,56,42,32,43,44,49,48,48,44,83,88,59,76,73,9,48,44,
    83,89,52,45,44,49,48,54,44,83,88,59,86,76,9,83,88,49,54,45,44,54,44,57,
    55,44,48,86,65,56,42,32,56,43,59,86,76,9,83,88,49,54,45,44,89,66,32,51,
    43,44,49,48,51,44,83,89,51,45,59,72,83,9,49,44,50,44,49,44,83,88,50,49,
    45,44,52,44,48,44,55,57,44,50,53,48,44,49,48,59,91,93,86,83,9,50,44,83,
    88,49,50,45,44,55,44,49,48,44,48,86,65,56,42,44,48,44,48,86,65,44,50,86,
    65,44,49,59,91,93,86,83,9,51,44,83,88,49,50,45,44,48,86,65,56,42,49,50,
    43,44,49,48,44,49,86,65,56,42,44,48,44,49,86,65,44,51,86,65,44,49,59,91,
    93,66,85,9,52,44,83,88,49,54,45,44,48,44,49,54,44,55,44,48,44,48,44,49,
    59,91,85,78,32,48,44,48,44,49,48,57,66,80,43,59,93,91,66,82,48,59,93,69,
    88,59,0,0};
}

namespace amos::host {

void relocate__Monitor_s()
{
    put(host_obj__Monitor_s + 0, 4, AMOS_STATIC_DIFFERENCE(host_obj__Monitor_s, 20, host_obj__Monitor_s, 0));      // +Monitor.s:136
    put(host_obj__Monitor_s + 4, 4, AMOS_STATIC_DIFFERENCE(host_obj__Monitor_s, 24, host_obj__Monitor_s, 0));      // +Monitor.s:137
    put(host_obj__Monitor_s + 8, 4, AMOS_STATIC_DIFFERENCE(host_obj__Monitor_s, 306, host_obj__Monitor_s, 0));      // +Monitor.s:138
    AMOS_BRIDGE_METADATA(host_obj__Monitor_s + 16);      // +Monitor.s:140
    put(host_obj__Monitor_s + 40, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 40));      // +Monitor.s:222
    put(host_obj__Monitor_s + 44, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 8));      // +Monitor.s:223
    put(host_obj__Monitor_s + 48, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 12));      // +Monitor.s:224
    put(host_obj__Monitor_s + 52, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 16));      // +Monitor.s:225
    put(host_obj__Monitor_s + 56, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 20));      // +Monitor.s:226
    put(host_obj__Monitor_s + 60, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 52));      // +Monitor.s:227
    put(host_obj__Monitor_s + 64, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 56));      // +Monitor.s:228
    put(host_obj__Monitor_s + 68, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 60));      // +Monitor.s:229
    put(host_obj__Monitor_s + 72, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 64));      // +Monitor.s:230
    put(host_obj__Monitor_s + 76, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 68));      // +Monitor.s:231
    put(host_obj__Monitor_s + 80, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 24));      // +Monitor.s:232
    put(host_obj__Monitor_s + 84, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_shell_glue_s, 32));      // +Monitor.s:233
    put(host_obj__Monitor_s + 88, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 72));      // +Monitor.s:234
    put(host_obj__Monitor_s + 92, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 92));      // +Monitor.s:235
    put(host_obj__Monitor_s + 96, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 96));      // +Monitor.s:236
    put(host_obj__Monitor_s + 100, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_view_glue_s, 100));      // +Monitor.s:237
    put(host_obj__Monitor_s + 104, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_listing_glue_s, 40));      // +Monitor.s:238
    put(host_obj__Monitor_s + 108, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_listing_glue_s, 28));      // +Monitor.s:239
    put(host_obj__Monitor_s + 112, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_info_glue_s, 4));      // +Monitor.s:240
    put(host_obj__Monitor_s + 116, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_listing_glue_s, 4));      // +Monitor.s:241
    put(host_obj__Monitor_s + 120, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_listing_glue_s, 8));      // +Monitor.s:242
    put(host_obj__Monitor_s + 124, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_info_glue_s, 40));      // +Monitor.s:243
    put(host_obj__Monitor_s + 128, 4, AMOS_STATIC_ADDRESS(host_obj_src_monitor_listing_glue_s, 0));      // +Monitor.s:244
    put(host_obj__Monitor_s + 538, 2, AMOS_STATIC_DIFFERENCE(host_obj__Monitor_s, 820, host_obj__Monitor_s, 540));      // +Monitor.s:567
}

const GlueEntry entries__Monitor_s[] = {
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 20), GlueKind::hand, AMOS_BIND_HAND(host_In_Editor), nullptr, 0, 0, "In_Editor"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 24), GlueKind::hand, AMOS_BIND_HAND(host_In_Program), nullptr, 0, 0, "In_Program"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 28), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_shell_glue_s, 0), 0, 0, "Tt_Tests"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 34), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_shell_glue_s, 4), 0, 0, "Tt_ReLoop"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 132), GlueKind::missing, nullptr, nullptr, 0, 0, "Tt_ColdStart"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 136), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_start_glue_s, 4), 0, 0, "Tt_Free"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 142), GlueKind::missing, nullptr, nullptr, 0, 0, "mon_quit_back"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 146), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 48), 0, 0, "TtReCop"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 152), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 36), 0, 0, "Tt_Show"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 158), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 40), 0, 0, "Tt_Hide"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 164), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 44), 0, 0, "Tt_Mouse"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 170), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj__Monitor_s, 178), 0, 0, "mon_screen_front"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 174), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj__Monitor_s, 184), 0, 0, "ToBack"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 178), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 76), 0, 0, "ToFront"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 184), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 80), 0, 0, "ToBack"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 190), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 84), 0, 0, "Tt_PScCopy"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 196), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 88), 0, 0, "TstPatch"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 202), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 32), 0, 0, "BoutStop"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 208), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 12), 0, 0, "Ttt_Centre"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 214), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 16), 0, 0, "Ttt_AffBuf"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 220), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 20), 0, 0, "Ttt_NewBuf"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 226), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 24), 0, 0, "Ttt_Untok"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 232), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 0), 0, 0, "Inf_Centre"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 238), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 32), 0, 0, "Ttt_Cls"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 244), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 36), 0, 0, "Ttt_SetSize"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 250), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 60), 0, 0, "Ttt_InfAct"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 256), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 64), 0, 0, "Ttt_TttAct"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 262), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 104), 0, 0, "Ttt_AMod"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 268), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 56), 0, 0, "NoInit"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 274), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_shell_glue_s, 28), 0, 0, "Tt_ReInit"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 280), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj__Monitor_s, 292), 0, 0, "mon_testing"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 284), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj__Monitor_s, 296), 0, 0, "Tt_Tst2"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 288), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj__Monitor_s, 302), 0, 0, "Tt_Tst3"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 292), GlueKind::hand, AMOS_BIND_HAND(host_mon_test_nothing), nullptr, 0, 0, "mon_test_nothing"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 296), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 108), 0, 0, "Tt_Tst2"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 302), GlueKind::hand, AMOS_BIND_HAND(host_Tt_Tst3), nullptr, 0, 0, "Tt_Tst3"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 306), GlueKind::hand, AMOS_BIND_HAND(host_MonitorChr), nullptr, 0, 0, "MonitorChr"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 310), GlueKind::missing, nullptr, nullptr, 0, 0, "mon_step_out"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 314), GlueKind::missing, nullptr, nullptr, 0, 0, "mon_step_out2"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 318), GlueKind::missing, nullptr, nullptr, 0, 0, "Step_Out"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 322), GlueKind::missing, nullptr, nullptr, 0, 0, "Step_Out2"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 326), GlueKind::hand, AMOS_BIND_HAND(host_mon_error), nullptr, 0, 0, "mon_error"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 330), GlueKind::missing, nullptr, nullptr, 0, 0, "Tt_GetTheMessage"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 334), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 44), 0, 0, "Tt_Souligne"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 340), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 48), 0, 0, "Tt_MouMou"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 346), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_help_glue_s, 0), 0, 0, "Hlp_FreeMap"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 352), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_listing_glue_s, 52), 0, 0, "Tt_Decoup"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 358), GlueKind::hand, AMOS_BIND_HAND(host_mon_evaluate), nullptr, 0, 0, "mon_evaluate"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 362), GlueKind::missing, nullptr, nullptr, 0, 0, "Tt_ReEv"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 366), GlueKind::hand, AMOS_BIND_HAND(host_mon_ev_error), nullptr, 0, 0, "mon_ev_error"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 370), GlueKind::hand, AMOS_BIND_HAND(host_mon_menage), nullptr, 0, 0, "mon_menage"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 374), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 8), 0, 0, "Inf_AffLine"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 380), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 12), 0, 0, "Inf_CptLines"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 386), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 16), 0, 0, "Inf_AffAll"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 392), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 20), 0, 0, "Inf_PrDeb"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 398), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 24), 0, 0, "Inf_Print"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 404), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 28), 0, 0, "Inf_PrFin"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 410), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 32), 0, 0, "Inf_Del"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 416), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 36), 0, 0, "Inf_Ins"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 422), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 44), 0, 0, "Inf_Slider"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 428), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 48), 0, 0, "Ttt_Alert"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 434), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 52), 0, 0, "Ttt_AlertDel"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 440), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 56), 0, 0, "Ttt_DelAff"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 446), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 0), 0, 0, "View_Aff"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 452), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 4), 0, 0, "View_Ec"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 458), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 16), 0, 0, "View_ScUpdate"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 464), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 68), 0, 0, "Tt_EcAct"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 470), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 8), 0, 0, "View_Cls"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 476), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 12), 0, 0, "View_Info"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 482), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 28), 0, 0, "ABoutons"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 488), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 24), 0, 0, "ABouton"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 494), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_view_glue_s, 20), 0, 0, "Tt_UPack"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 500), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 72), 0, 0, "Tt_Message"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 506), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 76), 0, 0, "Wmky"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 512), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 80), 0, 0, "Tt_TtBanks"},
    {AMOS_ENTRY_AT(host_obj__Monitor_s, 518), GlueKind::alias, nullptr, AMOS_ENTRY_AT(host_obj_src_monitor_info_glue_s, 84), 0, 0, "Tt_PrgBanks"},
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
