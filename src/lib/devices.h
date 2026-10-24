// The device channels of Dev Open... (Dev_List), for the instructions of
// lib/devices.cpp and the IO_Ports extension (ext/ioports.cpp)
#pragma once

#include "core/amos.h"

struct IORequest;
struct MsgPort;

namespace amos::dev {

// A device channel (Dev_List, 12 bytes): the IO request, its port, opened,
// what was done (0 nothing, 1 Do, 2 Send), the first error message and the
// number of them. Channel 7 overlaps the libraries, as in the assembler.
struct DeviceChannel {
    uint8_t b[12];
    struct IORequest *io() const { return ptr<struct IORequest>(peek_l(b)); }
    void set_io(struct IORequest *io) { poke_l(b, address(io)); }
    struct MsgPort *port() const { return ptr<struct MsgPort>(peek_l(b + 4)); }
    void set_port(struct MsgPort *p) { poke_l(b + 4, address(p)); }
    uint8_t &opened() { return b[8]; }
    uint8_t &state() { return b[9]; }
    uint8_t &first_error() { return b[10]; }
    uint8_t &errors() { return b[11]; }
};

DeviceChannel &device(uint32_t n);      // Dev.GetA2
void abort_device(DeviceChannel &d);
void close_device(DeviceChannel &d);    // Dev.CloseA2
void close_devices();
[[noreturn]] void device_error(DeviceChannel &d, int16_t code);
void open_device(DeviceChannel &d, const char *name, uint32_t length, uint32_t unit, uint32_t flags,
                 uint8_t first_error, uint8_t errors, uint32_t set);
struct IORequest *device_io(DeviceChannel &d);  // Dev.GetIO
void send_command(DeviceChannel &d, uint16_t command);
void do_command(DeviceChannel &d, uint16_t command);
uint32_t check_device(DeviceChannel &d);

} // namespace amos::dev
