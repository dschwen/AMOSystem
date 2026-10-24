// The WEntry glue's functions (build/gen/wentry_table.cpp), found by the
// address of their entry, or of a jmp to it: sorted on the first search.

#include "core/amos.h"

namespace amos {

extern WEntryPair wentry_table[];
extern const int wentry_count;

WFunction wentry_function(const void *entry)
{
    static bool sorted;
    if (!sorted) {
        for (int i = 1; i < wentry_count; i++) {
            WEntryPair p = wentry_table[i];
            int j = i;
            for (; j > 0 && address(wentry_table[j - 1].entry) > address(p.entry); j--)
                wentry_table[j] = wentry_table[j - 1];
            wentry_table[j] = p;
        }
        sorted = true;
    }
    uint32_t a = address(entry);
    // a forwarder first: jmp entry (+Verif.s, +Edit.s...)
    for (int hop = 0; hop < 2 && peek_w(ptr<void>(a)) == 0x4EF9; hop++)
        a = peek_l(ptr<void>(a + 2));
    int low = 0, high = wentry_count - 1;
    while (low <= high) {
        int mid = (low + high) / 2;
        uint32_t m = address(wentry_table[mid].entry);
        if (m == a)
            return wentry_table[mid].function;
        if (m < a)
            low = mid + 1;
        else
            high = mid - 1;
    }
    return nullptr;
}

} // namespace amos
