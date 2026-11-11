// The extensions' routines of the vertical blank (vbl.h)

#include "core/guest_scratch.h"
#include "ext/vbl.h"

using namespace amos;

namespace amos::ext3 {

void vbl_add(void (*routine)())
{
    be_u32 *list = idata().VblRout;
    uint32_t entry = guest_callback(routine);
    for (int i = 0; i < 8; i++) {
        if (list[i] == entry)
            return;
        if (!list[i]) {
            list[i] = entry;
            if (i + 1 < 8)
                list[i + 1] = 0;
            return;
        }
    }
}

void vbl_remove(void (*routine)())
{
    be_u32 *list = idata().VblRout;
    uint32_t entry = guest_callback(routine);
    for (int i = 0; i < 8; i++) {
        if (list[i] == entry) {
            for (int j = i; j + 1 < 8; j++)
                list[j] = list[j + 1];
            list[7] = 0;
            return;
        }
    }
}

} // namespace amos::ext3
