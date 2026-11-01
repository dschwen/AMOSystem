# The host's own results

For some tests the host's result differs from the Amiga's golden (`tests/golden`) in a known way, and
`tests/host_run.py` compares with the file here instead. Each file differs from the golden only in
what is listed. They are results to check the host against itself: a change anywhere else, or in
any other test, fails as usual.

## Text results

| test | line | the Amiga | the host | why |
|------|------|-----------|----------|-----|
| `maths` | 12 | `Log(10)` = 2.30258 | 2.30259 | the Amiga's mathtrans series is less accurate than the host's libm |
| `floats` | 1 | `Sin(30)` = 0.5000001 | 0.5 | the same, in single precision (FFP) |
| `floats2` | 1 | 59.9999999999961 | 59.999999999996 | the same, in double precision |
| `expr2` | 14 | `Sqr(2)` = 1.41421356237309 | 1.4142135623731 | the same: the Amiga's is one unit low |
| `devices` | 20-21 | the serial write gives error 149 and `Serial Check` 0 | -1 | no serial port is behind the host's serial.device: a write completes |
| `ioports` | 4, 14 | peeks at the serial block read 9600 8 8 1 128... and flags 32 | other numbers | the IORequest's block has the host's layout, not the Amiga's |

## Screenshots

Each differs from the golden only in the part named. Three are not understood yet: the Amiga's
result there is reproducible, the host does not reproduce it, and the cause has not been found.

| file | the part | why |
|------|----------|-----|
| `editor_about.png` | "4 extensions loaded" (the Amiga: 5) | the fifth is the compiler's (`AMOSPro_Compiler.Lib`, 68000 code): the host runs only the extensions that are C++ |
| `fsel2_devices.png`, `fsel2_assigns.png` | the lists | each machine's own devices and assigns (the emulator's DF0:, DH0:-DH2:, its Workbench's assigns) |
| `hsprites_s4.png` | one line under two computed sprites | where a hardware sprite is used again for the next computed sprite, the Amiga shows a line of colour 15; the host's sprite DMA does not model that line |
| `display_s5.png` | a short dashed line in colour 10 at line 100 | not understood: no copper write there; pixels the Amiga has in a bitplane of the dual playfield |
| `menus2_open.png`, `menus2_items.png`, `menus2_dropped.png` | the box of the item a procedure draws (`(PR MPROC)`) | not understood: the Amiga's box has stray pixels in it, the host's is clean |
| `editor_monitor6_h1.png` | the three lines between the help's screen and the next | not understood: on the Amiga they keep the help screen's colours, on the host the next screen's are already there (the copper's timing) |
