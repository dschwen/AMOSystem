// Native C++ library headers must not inherit the guest's -fpack-struct=2.
// Keep this boundary after guest headers and restore the caller's packing.
#pragma once
#pragma pack(push, 16)
#include <algorithm>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>
#pragma pack(pop)
