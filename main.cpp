#include <type_traits>

#pragma mark Stack
template <int i, int ...j>
struct Stack {
    template <int next>
    using push = Stack<next, i, j ...>;
    using pop = Stack<j ...>;
    constexpr static int const top = i;
};


template <int i>
struct Stack<i> {
    template <int next>
    using push = Stack<next, i>;
    
    //no pop
    //make use of C++ type system to check bound!
    
    constexpr static int const top = i;
};

#pragma mark Inst

typedef enum : int {
    //0 - no value with inst
    //1 - one value with inst
    ADD, //0
    SUB, //0
    MUL, //0
    DIV, //0
    PUSH,//1
    POP, //0
    JXX, //0, conditional jump. jump if stack top != 0. eat the top and the address.
    JMP, //1, unconditional jump to address.
    CALL, //1, save returning ip for RET
    RET //0, if ret_ip == -1, exit. else jump to ret_ip
    
    //  PRINT,//0, a constexpr VM don't print
} InstType;

struct Inst {
    InstType typ;
    int val;
    constexpr Inst(InstType typ, int val = 0) : typ(typ), val(val) {}
};



constexpr Inst insts[] = {
    {PUSH, 3}, //incrementByTen(3*2) = 16
    {PUSH, 2},
    {PUSH, 6},
    {PUSH, 1},
    {JXX}, // if 1 != 0, jump to 6
    {ADD},
    {MUL},
    {CALL, 9}, //call the func
    {RET},
    {PUSH, 100}, //func incrementByTen(int i) -> int
    {ADD},
    {RET},
    
};



template <InstType typ, int ip, typename stack, typename call_stack>
struct VM;

#define a (stack::top)
#define b (stack::pop::top)

template <int ip = 0, typename stack = Stack<0>, typename call_stack = Stack<-1> >
constexpr auto run() -> int {
    return VM<insts[ip].typ, ip, stack, call_stack>::run_impl;
}

template <int ip, typename stack, typename call_stack>
struct VM<ADD, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::pop::pop::template push<(a+b)>,
    call_stack
    >();
};

template <int ip, typename stack, typename call_stack>
struct VM<SUB, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::pop::pop::template push<(a-b)>,
    call_stack
    >();
};

template <int ip, typename stack, typename call_stack>
struct VM<MUL, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::pop::pop::template push<(a*b)>,
    call_stack
    >();
};

template <int ip, typename stack, typename call_stack>
struct VM<DIV, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::pop::pop::template push<(a/b)>,
    call_stack
    >();
};

#undef a
#undef b

template <int ip, typename stack, typename call_stack>
struct VM<PUSH, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::template push<insts[ip].val>,
    call_stack
    >();
};

template <int ip, typename stack, typename call_stack>
struct VM<POP, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    ip + 1,
    typename stack::pop,
    call_stack
    >();
};


template <int ip, typename stack, typename call_stack>
struct VM<JMP, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    insts[ip].val,
    stack,
    call_stack
    >();
};


template <int ip, typename stack, typename call_stack>
struct VM<JXX, ip, stack, call_stack> {
    constexpr static int const next_ip  = std::conditional<
    stack::top != 0,
    Stack<stack::pop::top>,//jump!
    Stack<ip + 1>
    >::type::top;
    
    constexpr static int const run_impl =
    run<
    next_ip,
    typename stack::pop::pop, //eat the condition and the address
    call_stack
    >();
};

template <int ip, typename stack, typename call_stack>
struct VM<CALL, ip, stack, call_stack> {
    constexpr static int const run_impl =
    run<
    insts[ip].val,
    stack,
    typename call_stack::template push<ip + 1>
    >();
};


template <int ip, typename stack, typename call_stack>
struct VM<RET, ip, stack, call_stack> {
    struct RET_Helper {
        constexpr static int const top = run<call_stack::top, stack, typename call_stack::pop>();
    };
    constexpr static int const run_impl =
    std::conditional<
        call_stack::top == -1,
        Stack<stack::top>,
        RET_Helper
    >::type::top;
};


constexpr int result = run();

#include <stdio.h>

int main(int argc, const char * argv[]) {
    printf("%d\n", result);
    return 0;
}
