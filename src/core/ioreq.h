// The IO requests of the devices in the 68000's layout (exec/io.h,
// devices/serial.h, devices/parallel.h): big-endian, addresses of 32 bits,
// at the Amiga's offsets. AMOS programs read them (Serial Base, Printer
// Base, Parallel Base); on the Amiga they are the NDK's structures as they
// are, on another host they are what its devices read.

#pragma once

#include "core/endian.h"

#include <stddef.h>
#include <stdint.h>

namespace amos {

struct IONode {
    be_u32 ln_Succ, ln_Pred;
    uint8_t ln_Type;
    int8_t ln_Pri;
    be_u32 ln_Name;
};

struct IOMessage {
    IONode mn_Node;
    be_u32 mn_ReplyPort;
    be_u16 mn_Length;
};

// IORequest and IOStdReq
struct IOReq {
    IOMessage io_Message;
    be_u32 io_Device, io_Unit;
    be_u16 io_Command;
    uint8_t io_Flags;
    int8_t io_Error;
    be_u32 io_Actual, io_Length, io_Data, io_Offset;
};

struct IOExtSer {
    IOReq IOSer;
    be_u32 io_CtlChar, io_RBufLen, io_ExtFlags, io_Baud, io_BrkTime;
    uint8_t io_TermArray[8];
    uint8_t io_ReadLen, io_WriteLen, io_StopBits, io_SerFlags;
    be_u16 io_Status;
};

struct IOExtPar {
    IOReq IOPar;
    be_u32 io_PExtFlags;
    uint8_t io_Status, io_ParFlags;
    uint8_t io_PTermArray[8];
};

static_assert(offsetof(IOReq, io_Message.mn_ReplyPort) == 14 && offsetof(IOReq, io_Device) == 20);
static_assert(offsetof(IOReq, io_Command) == 28 && offsetof(IOReq, io_Error) == 31);
static_assert(offsetof(IOReq, io_Actual) == 32 && offsetof(IOReq, io_Data) == 40 && sizeof(IOReq) == 48);
static_assert(offsetof(IOExtSer, io_Baud) == 60 && offsetof(IOExtSer, io_ReadLen) == 76);
static_assert(offsetof(IOExtSer, io_SerFlags) == 79 && sizeof(IOExtSer) == 82);
static_assert(offsetof(IOExtPar, io_Status) == 52 && sizeof(IOExtPar) == 62);

} // namespace amos
