#include "core/local_jump.h"
#include <stdio.h>
struct State { amos::local_jump::Buffer *target; int count; };
static void raise(State &s) { ++s.count; amos::local_jump::restore(s.target); }
static void nested(State &s, int depth)
{
    amos::local_jump::Buffer local;
    auto *outer=s.target;
    s.target=&local;
    if (!AMOS_LOCAL_SETJMP(local)) {
        if (depth) nested(s,depth-1);
        raise(s);
    }
    s.target=outer;
}
int main()
{
    State state={nullptr,0};
    for (int n=0;n<10;++n) nested(state,3);
    bool ok=state.count==40 && state.target==nullptr;
    printf("{\"nested_local_jumps\":%s,\"count\":%d,\"buffer_size\":%zu,\"alignment\":%zu}\n",ok ? "true":"false",state.count,sizeof(amos::local_jump::Buffer),alignof(amos::local_jump::Buffer));
    return ok ? 0:2;
}
