#include <stddef.h>
#include "core/layout.h"
#include "host/clipboard_text.h"
struct GuestAfterHeaders { char tag; double value; };
static_assert(offsetof(GuestAfterHeaders,value)==2);
static_assert(alignof(std::pair<char,double>)==alignof(double));
#include <stdio.h>
int main()
{
    std::string text;
    bool ok=amos::host::clipboard::decode("caf\xc3\xa9",text) && text=="caf\xe9";
    printf("{\"guest_layout\":true,\"native_cxx\":%s}\n",ok ? "true":"false");
    return ok ? 0:2;
}
