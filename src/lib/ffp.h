// Motorola fast floating point arithmetic (see ffp.cpp)
#pragma once
#include <stdint.h>

namespace amos::ffp {

uint32_t neg(uint32_t a);
int cmp(uint32_t a, uint32_t b);  // <0, 0, >0: a against b
uint32_t add(uint32_t a, uint32_t b);
uint32_t sub(uint32_t a, uint32_t b);
uint32_t mul(uint32_t a, uint32_t b);
uint32_t div(uint32_t a, uint32_t b);
uint32_t from_int(int32_t v);
int32_t to_int(uint32_t f);

} // namespace amos::ffp
