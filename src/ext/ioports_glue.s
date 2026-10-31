; ---------------------------------------------------------------------
;	The IO_Ports extension: ext/ioports.cpp
; ---------------------------------------------------------------------
		Include	"glue.i"

		SECTION	.text,CODE

		LibCall	Ext_IO_Ports_InSerialOpen2,_ext_serial_open2,2,None
		LibCall	Ext_IO_Ports_InSerialOpen5,_ext_serial_open5,5,None
		LibCall	Ext_IO_Ports_InSerialClose1,_ext_serial_close1,1,None
		LibCall	Ext_IO_Ports_InSerialClose0,_ext_serial_close0,0,None
		LibCall	Ext_IO_Ports_InSerialSend,_ext_serial_send,2,None
		LibCall	Ext_IO_Ports_InSerialOut,_ext_serial_out,3,None
		LibCall	Ext_IO_Ports_FnSerialGet,_ext_fn_serial_get,1,Int
		LibCall	Ext_IO_Ports_FnSerialInput,_ext_fn_serial_input,1,String
		LibCall	Ext_IO_Ports_InSerialSpeed,_ext_serial_speed,2,None
		LibCall	Ext_IO_Ports_InSerialBits,_ext_serial_bits,3,None
		LibCall	Ext_IO_Ports_InSerialParity,_ext_serial_parity,2,None
		LibCall	Ext_IO_Ports_InSerialX,_ext_serial_x,2,None
		LibCall	Ext_IO_Ports_InSerialBuf,_ext_serial_buf,2,None
		LibCall	Ext_IO_Ports_InSerialFast,_ext_serial_fast,1,None
		LibCall	Ext_IO_Ports_InSerialSlow,_ext_serial_slow,1,None
		LibCall	Ext_IO_Ports_FnSerialCheck,_ext_fn_serial_check,1,Int
		LibCall	Ext_IO_Ports_FnSerialError,_ext_fn_serial_error,1,Int
		LibCall	Ext_IO_Ports_FnSerialStatus,_ext_fn_serial_status,1,Int
		LibCall	Ext_IO_Ports_FnSerialBase,_ext_fn_serial_base,1,Int
		LibCall	Ext_IO_Ports_InSerialAbort,_ext_serial_abort,1,None
		LibCall	Ext_IO_Ports_FnPrinterBase,_ext_fn_printer_base,0,Int
		LibCall	Ext_IO_Ports_FnPrinterCheck,_ext_fn_printer_check,0,Int
		LibCall	Ext_IO_Ports_InPrinterOpen,_ext_printer_open,0,None
		LibCall	Ext_IO_Ports_InPrinterClose,_ext_printer_close,0,None
		LibCall	Ext_IO_Ports_FnPrinterError,_ext_fn_printer_error,0,Int
		LibCall	Ext_IO_Ports_InPrinterSend,_ext_printer_send,1,None
		LibCall	Ext_IO_Ports_InPrinterOut,_ext_printer_out,2,None
		LibCall	Ext_IO_Ports_InPrinterAbort,_ext_printer_abort,0,None
		LibCall	Ext_IO_Ports_FnPrinterOnline,_ext_fn_printer_online,0,Int
		LibCall	Ext_IO_Ports_InPrinterDump0,_ext_printer_dump0,0,None
		LibCall	Ext_IO_Ports_InPrinterDump4,_ext_printer_dump4,4,None
		LibCall	Ext_IO_Ports_InPrinterDump7,_ext_printer_dump7,7,None
		LibCall	Ext_IO_Ports_FnParallelBase,_ext_fn_parallel_base,0,Int
		LibCall	Ext_IO_Ports_FnParallelCheck,_ext_fn_parallel_check,0,Int
		LibCall	Ext_IO_Ports_InParallelOpen,_ext_parallel_open,0,None
		LibCall	Ext_IO_Ports_InParallelClose,_ext_parallel_close,0,None
		LibCall	Ext_IO_Ports_InParallelSend,_ext_parallel_send,1,None
		LibCall	Ext_IO_Ports_InParallelOut,_ext_parallel_out,2,None
		LibCall	Ext_IO_Ports_InParallelAbort,_ext_parallel_abort,0,None
		LibCall	Ext_IO_Ports_FnParallelStatus,_ext_fn_parallel_status,0,Int
		LibCall	Ext_IO_Ports_FnParallelError,_ext_fn_parallel_error,0,Int
		LibCall	Ext_IO_Ports_FnParallelInput1,_ext_fn_parallel_input1,1,String
		LibCall	Ext_IO_Ports_FnParallelInput2,_ext_fn_parallel_input2,2,String

; The cold start; the end, at each run and at the end (ExtAdr+4, +8)
		RegRoutine Ext_IO_Ports_IOCold,_ext_ioports_cold
		RoutineCall _ext_ioports_end_entry,_ext_ioports_end
