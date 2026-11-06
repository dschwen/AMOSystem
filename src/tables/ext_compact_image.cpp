// src_ext_compact_image_s: converted from the assembler (see tables.h)
#include "tables/tables.h"

using namespace amos;
using namespace amos::host;

extern "C" {
alignas(4) uint8_t host_obj_src_ext_compact_image_s[226] = {
    0,0,0,42,0,0,0,114,0,0,0,0,0,0,0,46,0,0,65,80,50,48,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,128,255,0,2,
    0,1,33,112,97,99,235,73,48,116,48,254,0,3,0,1,128,73,48,116,48,44,48,44,
    48,44,48,44,48,255,0,4,0,1,33,115,112,97,99,235,73,48,116,48,254,0,0,5,
    0,1,128,73,48,116,48,44,48,44,48,44,48,44,48,255,0,6,0,1,33,117,110,112,
    97,99,235,73,48,254,0,9,0,1,128,73,48,116,48,254,0,7,0,1,128,73,48,44,
    48,44,48,255,0,0,0,0,0,0,65,77,79,83,80,114,111,32,80,105,99,116,117,114,
    101,32,67,111,109,112,97,99,116,111,114,32,86,32,50,46,48,48,0,0,36,86,69,82,
    58,32,50,46,48,48,0,0,0,0};
}

namespace amos::host {

void relocate_src_ext_compact_image_s()
{
}

const GlueEntry entries_src_ext_compact_image_s[] = {
    {AMOS_ENTRY_END, GlueKind::missing, nullptr, nullptr, 0, 0, nullptr},
};

} // namespace amos::host
