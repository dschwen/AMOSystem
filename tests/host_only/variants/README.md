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
| `devices` | 21 | `Serial Check` after a write: 0 | -1 | the write never completes in FS-UAE, which has no serial port attached; the host's serial.device completes it at once, as a real Amiga's port sends it |

## Screenshots

Each differs from the golden only in the part named. Some are not understood yet: the Amiga's
result there is reproducible, the host does not reproduce it, and the cause has not been found.

| file | the part | why |
|------|----------|-----|
| `editor_about.png` | "4 extensions loaded" (the Amiga: 5) | the fifth is the compiler's (`AMOSPro_Compiler.Lib`, 68000 code): the host runs only the extensions that are C++ |
| `fsel2_devices.png`, `fsel2_assigns.png` | the lists | each machine's own devices and assigns (the emulator's DF0:, DH0:-DH2:, its Workbench's assigns) |
| `display_s5.png` | a short dashed line in colour 10 at line 100 | partly understood (probes in the emulator): the screens' memory is the same on both, and the dash is playfield 2's data from 5 bytes past the end of screen 2's second plane (the bytes are there in chip memory): on that one line the Amiga's bitplane DMA fetches beyond the plane, which the host's display does not model; the copper list does not explain it |
| `menus2_dropped.png` | two pixels of the top row of the item's box after it was dragged | not understood: the Amiga has colour 3 there, the host the fill's colour 2; the rest of the box (its outline in the line pattern, its fill) is the same |
| `editor_monitor6_h1.png` | the three lines between the help's screen and the next | not understood: on the Amiga they keep the help screen's colours, on the host the next screen's are already there (the copper's timing) |
