// src_ext_ioports_glue_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_ext_ioports_glue_s[868] = {
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,5,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,3,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,3,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,2,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,4,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,7,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,0,0,3,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,1,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,2,0,3,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,3,
    76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,
    71,101,116,80,0,0,0,0,0,0,0,0,0,0,0,0,76,67,97,108,71,101,116,80,
    0,0,0,0,0,0,0,0,0,1,0,2,76,67,97,108,71,101,116,80,0,0,0,0,
    0,0,0,0,0,2,0,2,76,67,97,108,71,101,116,80,0,0,0,0,0,0,0,0,
    0,0,0,0};
}

namespace amos::host {

void relocate_src_ext_ioports_glue_s()
{
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 0, ext_serial_open2);      // src/ext/ioports_glue.s:8
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 20, ext_serial_open5);      // src/ext/ioports_glue.s:9
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 40, ext_serial_close1);      // src/ext/ioports_glue.s:10
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 60, ext_serial_close0);      // src/ext/ioports_glue.s:11
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 80, ext_serial_send);      // src/ext/ioports_glue.s:12
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 100, ext_serial_out);      // src/ext/ioports_glue.s:13
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 120, ext_fn_serial_get);      // src/ext/ioports_glue.s:14
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 140, ext_fn_serial_input);      // src/ext/ioports_glue.s:15
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 160, ext_serial_speed);      // src/ext/ioports_glue.s:16
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 180, ext_serial_bits);      // src/ext/ioports_glue.s:17
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 200, ext_serial_parity);      // src/ext/ioports_glue.s:18
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 220, ext_serial_x);      // src/ext/ioports_glue.s:19
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 240, ext_serial_buf);      // src/ext/ioports_glue.s:20
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 260, ext_serial_fast);      // src/ext/ioports_glue.s:21
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 280, ext_serial_slow);      // src/ext/ioports_glue.s:22
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 300, ext_fn_serial_check);      // src/ext/ioports_glue.s:23
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 320, ext_fn_serial_error);      // src/ext/ioports_glue.s:24
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 340, ext_fn_serial_status);      // src/ext/ioports_glue.s:25
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 360, ext_fn_serial_base);      // src/ext/ioports_glue.s:26
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 380, ext_serial_abort);      // src/ext/ioports_glue.s:27
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 400, ext_fn_printer_base);      // src/ext/ioports_glue.s:28
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 420, ext_fn_printer_check);      // src/ext/ioports_glue.s:29
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 440, ext_printer_open);      // src/ext/ioports_glue.s:30
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 460, ext_printer_close);      // src/ext/ioports_glue.s:31
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 480, ext_fn_printer_error);      // src/ext/ioports_glue.s:32
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 500, ext_printer_send);      // src/ext/ioports_glue.s:33
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 520, ext_printer_out);      // src/ext/ioports_glue.s:34
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 540, ext_printer_abort);      // src/ext/ioports_glue.s:35
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 560, ext_fn_printer_online);      // src/ext/ioports_glue.s:36
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 580, ext_printer_dump0);      // src/ext/ioports_glue.s:37
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 600, ext_printer_dump4);      // src/ext/ioports_glue.s:38
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 620, ext_printer_dump7);      // src/ext/ioports_glue.s:39
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 640, ext_fn_parallel_base);      // src/ext/ioports_glue.s:40
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 660, ext_fn_parallel_check);      // src/ext/ioports_glue.s:41
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 680, ext_parallel_open);      // src/ext/ioports_glue.s:42
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 700, ext_parallel_close);      // src/ext/ioports_glue.s:43
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 720, ext_parallel_send);      // src/ext/ioports_glue.s:44
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 740, ext_parallel_out);      // src/ext/ioports_glue.s:45
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 760, ext_parallel_abort);      // src/ext/ioports_glue.s:46
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 780, ext_fn_parallel_status);      // src/ext/ioports_glue.s:47
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 800, ext_fn_parallel_error);      // src/ext/ioports_glue.s:48
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 820, ext_fn_parallel_input1);      // src/ext/ioports_glue.s:49
    AMOS_LIBRARY_METADATA(host_obj_src_ext_ioports_glue_s + 840, ext_fn_parallel_input2);      // src/ext/ioports_glue.s:50
}

const GlueEntry entries_src_ext_ioports_glue_s[] = {
    {host_obj_src_ext_ioports_glue_s + 16, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_open2), nullptr, 2, 3, "Ext_IO_Ports_InSerialOpen2"},
    {host_obj_src_ext_ioports_glue_s + 36, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_open5), nullptr, 5, 3, "Ext_IO_Ports_InSerialOpen5"},
    {host_obj_src_ext_ioports_glue_s + 56, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_close1), nullptr, 1, 3, "Ext_IO_Ports_InSerialClose1"},
    {host_obj_src_ext_ioports_glue_s + 76, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_close0), nullptr, 0, 3, "Ext_IO_Ports_InSerialClose0"},
    {host_obj_src_ext_ioports_glue_s + 96, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_send), nullptr, 2, 3, "Ext_IO_Ports_InSerialSend"},
    {host_obj_src_ext_ioports_glue_s + 116, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_out), nullptr, 3, 3, "Ext_IO_Ports_InSerialOut"},
    {host_obj_src_ext_ioports_glue_s + 136, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_get), nullptr, 1, 0, "Ext_IO_Ports_FnSerialGet"},
    {host_obj_src_ext_ioports_glue_s + 156, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_input), nullptr, 1, 2, "Ext_IO_Ports_FnSerialInput"},
    {host_obj_src_ext_ioports_glue_s + 176, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_speed), nullptr, 2, 3, "Ext_IO_Ports_InSerialSpeed"},
    {host_obj_src_ext_ioports_glue_s + 196, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_bits), nullptr, 3, 3, "Ext_IO_Ports_InSerialBits"},
    {host_obj_src_ext_ioports_glue_s + 216, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_parity), nullptr, 2, 3, "Ext_IO_Ports_InSerialParity"},
    {host_obj_src_ext_ioports_glue_s + 236, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_x), nullptr, 2, 3, "Ext_IO_Ports_InSerialX"},
    {host_obj_src_ext_ioports_glue_s + 256, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_buf), nullptr, 2, 3, "Ext_IO_Ports_InSerialBuf"},
    {host_obj_src_ext_ioports_glue_s + 276, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_fast), nullptr, 1, 3, "Ext_IO_Ports_InSerialFast"},
    {host_obj_src_ext_ioports_glue_s + 296, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_slow), nullptr, 1, 3, "Ext_IO_Ports_InSerialSlow"},
    {host_obj_src_ext_ioports_glue_s + 316, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_check), nullptr, 1, 0, "Ext_IO_Ports_FnSerialCheck"},
    {host_obj_src_ext_ioports_glue_s + 336, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_error), nullptr, 1, 0, "Ext_IO_Ports_FnSerialError"},
    {host_obj_src_ext_ioports_glue_s + 356, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_status), nullptr, 1, 0, "Ext_IO_Ports_FnSerialStatus"},
    {host_obj_src_ext_ioports_glue_s + 376, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_serial_base), nullptr, 1, 0, "Ext_IO_Ports_FnSerialBase"},
    {host_obj_src_ext_ioports_glue_s + 396, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_serial_abort), nullptr, 1, 3, "Ext_IO_Ports_InSerialAbort"},
    {host_obj_src_ext_ioports_glue_s + 416, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_printer_base), nullptr, 0, 0, "Ext_IO_Ports_FnPrinterBase"},
    {host_obj_src_ext_ioports_glue_s + 436, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_printer_check), nullptr, 0, 0, "Ext_IO_Ports_FnPrinterCheck"},
    {host_obj_src_ext_ioports_glue_s + 456, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_open), nullptr, 0, 3, "Ext_IO_Ports_InPrinterOpen"},
    {host_obj_src_ext_ioports_glue_s + 476, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_close), nullptr, 0, 3, "Ext_IO_Ports_InPrinterClose"},
    {host_obj_src_ext_ioports_glue_s + 496, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_printer_error), nullptr, 0, 0, "Ext_IO_Ports_FnPrinterError"},
    {host_obj_src_ext_ioports_glue_s + 516, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_send), nullptr, 1, 3, "Ext_IO_Ports_InPrinterSend"},
    {host_obj_src_ext_ioports_glue_s + 536, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_out), nullptr, 2, 3, "Ext_IO_Ports_InPrinterOut"},
    {host_obj_src_ext_ioports_glue_s + 556, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_abort), nullptr, 0, 3, "Ext_IO_Ports_InPrinterAbort"},
    {host_obj_src_ext_ioports_glue_s + 576, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_printer_online), nullptr, 0, 0, "Ext_IO_Ports_FnPrinterOnline"},
    {host_obj_src_ext_ioports_glue_s + 596, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_dump0), nullptr, 0, 3, "Ext_IO_Ports_InPrinterDump0"},
    {host_obj_src_ext_ioports_glue_s + 616, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_dump4), nullptr, 4, 3, "Ext_IO_Ports_InPrinterDump4"},
    {host_obj_src_ext_ioports_glue_s + 636, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_printer_dump7), nullptr, 7, 3, "Ext_IO_Ports_InPrinterDump7"},
    {host_obj_src_ext_ioports_glue_s + 656, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_base), nullptr, 0, 0, "Ext_IO_Ports_FnParallelBase"},
    {host_obj_src_ext_ioports_glue_s + 676, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_check), nullptr, 0, 0, "Ext_IO_Ports_FnParallelCheck"},
    {host_obj_src_ext_ioports_glue_s + 696, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_parallel_open), nullptr, 0, 3, "Ext_IO_Ports_InParallelOpen"},
    {host_obj_src_ext_ioports_glue_s + 716, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_parallel_close), nullptr, 0, 3, "Ext_IO_Ports_InParallelClose"},
    {host_obj_src_ext_ioports_glue_s + 736, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_parallel_send), nullptr, 1, 3, "Ext_IO_Ports_InParallelSend"},
    {host_obj_src_ext_ioports_glue_s + 756, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_parallel_out), nullptr, 2, 3, "Ext_IO_Ports_InParallelOut"},
    {host_obj_src_ext_ioports_glue_s + 776, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_parallel_abort), nullptr, 0, 3, "Ext_IO_Ports_InParallelAbort"},
    {host_obj_src_ext_ioports_glue_s + 796, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_status), nullptr, 0, 0, "Ext_IO_Ports_FnParallelStatus"},
    {host_obj_src_ext_ioports_glue_s + 816, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_error), nullptr, 0, 0, "Ext_IO_Ports_FnParallelError"},
    {host_obj_src_ext_ioports_glue_s + 836, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_input1), nullptr, 1, 2, "Ext_IO_Ports_FnParallelInput1"},
    {host_obj_src_ext_ioports_glue_s + 856, GlueKind::libcall, AMOS_BIND_LIBRARY(ext_fn_parallel_input2), nullptr, 2, 2, "Ext_IO_Ports_FnParallelInput2"},
    {host_obj_src_ext_ioports_glue_s + 860, GlueKind::reg, AMOS_BIND_REG(ext_ioports_cold), nullptr, 0, 0, "Ext_IO_Ports_IOCold"},
    {host_obj_src_ext_ioports_glue_s + 864, GlueKind::call, AMOS_BIND_CALL(ext_ioports_end), nullptr, 0, 0, "ext_ioports_end_entry"},
    {nullptr, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
