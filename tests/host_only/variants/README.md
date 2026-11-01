# The host's own results

For six tests the host's result differs from the Amiga's golden (`tests/golden`) in a known way, and
`tests/host_run.py` compares with the file here instead. Each file differs from the golden only in
the lines listed. They are results to check the host against itself: a change in any other line, or
any other test, fails as usual.

| test | line | the Amiga | the host | why |
|------|------|-----------|----------|-----|
| `maths` | 12 | `Log(10)` = 2.30258 | 2.30259 | the Amiga's mathtrans series is less accurate than the host's libm |
| `floats` | 1 | `Sin(30)` = 0.5000001 | 0.5 | the same, in single precision (FFP) |
| `floats2` | 1 | 59.9999999999961 | 59.999999999996 | the same, in double precision |
| `expr2` | 14 | `Sqr(2)` = 1.41421356237309 | 1.4142135623731 | the same: the Amiga's is one unit low |
| `devices` | 20-21 | the serial write gives error 149 and `Serial Check` 0 | -1 | no serial port is behind the host's serial.device: a write completes |
| `ioports` | 4, 14 | peeks at the serial block read 9600 8 8 1 128... and flags 32 | other numbers | the IORequest's block has the host's layout, not the Amiga's |
