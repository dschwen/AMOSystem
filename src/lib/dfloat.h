// The software IEEE double arithmetic of the double <-> text conversions
// (see dfloat.cpp)
#pragma once
#include <stdint.h>

namespace amos::dbl {

struct D {
    uint32_t hi, lo;
};

int tst(D a);            // -1, 0, 1 (0 for a zero exponent)
int cmp(D a, D b);       // -1, 0, 1: a against b
D neg(D a);
D add(D a, D b);
D sub(D a, D b);
D mul(D a, D b);
D div(D a, D b);
D from_int(int32_t v);
int32_t to_int(D a);     // truncated

} // namespace amos::dbl
