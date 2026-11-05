#define _XOPEN_SOURCE 700
#define _DARWIN_C_SOURCE 1
#include <ucontext.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <setjmp.h>
static ucontext_t caller, child;
static volatile int stage;
static void entry()
{
    jmp_buf jump;
    if (!setjmp(jump)) longjmp(jump,1);
    stage=1;
    swapcontext(&child,&caller);
    stage=2;
}
int main()
{
    constexpr size_t bytes=256*1024;
    void *stack=mmap(nullptr,bytes,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
    if (stack==MAP_FAILED || getcontext(&child)!=0) {
        printf("{\"ucontext\":false,\"errno\":%d}\n",errno);return 2;
    }
    child.uc_stack.ss_sp=stack;child.uc_stack.ss_size=bytes;child.uc_link=&caller;
    makecontext(&child,entry,0);
    bool ok=swapcontext(&caller,&child)==0 && stage==1;
    ok=ok && swapcontext(&caller,&child)==0 && stage==2;
    munmap(stack,bytes);
    printf("{\"ucontext\":%s,\"note\":\"Basic switching only; full SIMD/FP preservation not yet tested\"}\n",ok ? "true":"false");
    return ok ? 0:2;
}
