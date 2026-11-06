#include "runtime/guest_dispatch.h"
#include "memory/guest_segments.h"
#include <assert.h>
using namespace amos::guest;
using namespace amos::host;
using Signature = int(int);
using Index = GuestDispatchIndex<Signature, int>;
int main() {
    // Native payloads are deliberately independent of either logical placement.
    int payloads[] = {10,20,30,40};
    const uint32_t bases[] = {0x100000,0x80000000};
    for (const uint32_t base : bases) {
        SegmentBindings segments;
        assert(segments.bind(SegmentId(1), GuestSpan<Byte>(abi::AddressCodec::decode<Byte>(base), 32)) == Fault::none);
        auto entry = [&](uint32_t offset) { return segments.resolve(EntrySymbol<Signature>(SegmentId(1), offset)).value; };
        Index::Slot slots[] = {{entry(12),payloads+3},{entry(4),payloads+1},{entry(0),payloads},{entry(8),payloads+2}};
        Index index;
        assert(index.initialize(slots,4) == Fault::none);
        for (uint32_t i=0;i<4;++i) assert(index.find(entry(i*4)) == payloads+i);
        assert(!index.find(entry(16)));
        assert(!index.find({}));
        assert(!index.find(abi::EntryCodec::decode<Signature>(0xffffffff)));
        Index::Slot duplicates[] = {{entry(4),payloads},{entry(4),payloads+1}};
        assert(index.initialize(duplicates,2) == Fault::invalid_range);
        assert(!index.find(entry(4)));
        assert(index.initialize(nullptr,1) == Fault::null_pointer);
        assert(index.initialize(nullptr,0) == Fault::none);
        Index::Slot invalid[] = {{entry(0),nullptr}};
        assert(index.initialize(invalid,1) == Fault::null_pointer);
    }
}
