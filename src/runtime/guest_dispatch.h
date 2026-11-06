// A native metadata index keyed only by typed guest code addresses.
#pragma once
#include "abi/guest_entry_codec.h"
#pragma pack(push,8)
namespace amos::host {
template<class Signature, class Payload> class GuestDispatchIndex {
public:
    using Entry = guest::GuestEntry<Signature>;
    struct Slot { Entry at{}; const Payload* payload = nullptr; };
private:
    Slot* slots_ = nullptr;
    size_t count_ = 0;
public:
    // Storage is native and owned by the caller. Never indexes guest RAM.
    guest::Fault initialize(Slot* slots, size_t count) {
        slots_ = nullptr; count_ = 0;
        if (count && !slots) return guest::Fault::null_pointer;
        for (size_t i = 0; i < count; ++i)
            if (!slots[i].at || !slots[i].payload) return guest::Fault::null_pointer;
        for (size_t gap = count / 2; gap; gap /= 2)
            for (size_t i = gap; i < count; ++i) {
                Slot value = slots[i];
                size_t j = i;
                for (; j >= gap && guest::abi::EntryCodec::encode(slots[j-gap].at) > guest::abi::EntryCodec::encode(value.at); j -= gap)
                    slots[j] = slots[j-gap];
                slots[j] = value;
            }
        for (size_t i = 1; i < count; ++i)
            if (slots[i-1].at == slots[i].at) return guest::Fault::invalid_range;
        slots_ = slots; count_ = count;
        return guest::Fault::none;
    }
    const Payload* find(Entry at) const {
        size_t lo = 0, hi = count_;
        const auto key = guest::abi::EntryCodec::encode(at);
        while (lo < hi) {
            const size_t mid = lo + (hi-lo)/2;
            const auto candidate = guest::abi::EntryCodec::encode(slots_[mid].at);
            if (candidate == key) return slots_[mid].payload;
            if (candidate < key) lo = mid+1;
            else hi = mid;
        }
        return nullptr;
    }
};
}
#pragma pack(pop)
