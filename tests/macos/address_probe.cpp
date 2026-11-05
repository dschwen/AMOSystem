// Native ABI probe. Run on the actual target; no AMOS pointer truncation here.
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <setjmp.h>
#include <string>
#include <vector>
#if !defined(__APPLE__)
#error This probe targets macOS, not the container's Linux kernel.
#endif
static int global_value;
static bool fits(uintptr_t p, size_t n) { return n && p <= UINT32_MAX && n-1 <= UINT32_MAX-p; }
int main()
{
    int local;
    uintptr_t code=reinterpret_cast<uintptr_t>(&main), data=reinterpret_cast<uintptr_t>(&global_value);
    long page=sysconf(_SC_PAGESIZE);
    jmp_buf jump;
    volatile int jumped=0;
    if (!setjmp(jump)) { jumped=1; longjmp(jump,1); }
    std::vector<std::string> native={"native libc++", "C++20"};
    printf("{\"architecture\":\"%s\",\"page_size\":%ld,\"function_address\":%llu,\"global_address\":%llu,\"stack_address\":%llu,\"setjmp\":%s,\"native_cxx\":%s,\"allocations\":[",
#if defined(__aarch64__) || defined(__arm64__)
        "arm64",
#else
        "x86_64",
#endif
        page,(unsigned long long)code,(unsigned long long)data,(unsigned long long)reinterpret_cast<uintptr_t>(&local),jumped ? "true":"false", native.size()==2 ? "true":"false");
    bool arena=false;
    const size_t bytes=512ull*1024*1024;
    const uintptr_t hints[]={0x1000000,0x20000000,0x40000000,0x80000000};
    unsigned count=4;
#ifdef MAP_32BIT
    count=5;
#endif
    for (unsigned i=0;i<count;++i) {
        uintptr_t hint=i<4 ? hints[i]:0;
        int flags=MAP_PRIVATE|MAP_ANON;
#ifdef MAP_32BIT
        if (i==4) flags|=MAP_32BIT;
#endif
        errno=0;
        void *p=mmap(reinterpret_cast<void *>(hint),bytes,PROT_READ|PROT_WRITE,flags,-1,0);
        int error=errno;
        bool low=p!=MAP_FAILED && fits(reinterpret_cast<uintptr_t>(p),bytes);
        arena|=low;
        printf("%s{\"hint\":%llu,\"map_32bit\":%s,\"address\":%llu,\"size\":%zu,\"low_range\":%s,\"errno\":%d}",i ? ",":"",(unsigned long long)hint,i==4 ? "true":"false",p==MAP_FAILED ? 0ull:(unsigned long long)reinterpret_cast<uintptr_t>(p),bytes,low ? "true":"false",error);
        if (p!=MAP_FAILED) munmap(p,bytes);
    }
    bool gate=arena && fits(code,1) && fits(data,sizeof global_value);
    printf("],\"direct_pointer_gate\":%s,\"note\":\"Gate also requires viable interpreter stacks and context/ABI probes\"}\n",gate ? "true":"false");
    return gate ? 0:3; // Distinguish a measured address-model failure from success.
}
