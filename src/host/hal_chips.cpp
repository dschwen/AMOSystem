// hal/chips.h on another host: the custom chips' and CIA A's registers,
// which the renderer of the display (to come) reads

#include "hal/chips.h"

namespace {
volatile struct Custom registers;
volatile uint8_t cia_a = 0xFF;      // no button pressed
}

namespace amos::hal {

volatile struct Custom &custom() { return registers; }
volatile uint8_t &cia_a_pra() { return cia_a; }

} // namespace amos::hal
