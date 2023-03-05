#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include <functional>
#include <cstdint>
#include "Scheduler.h"

#define STACK (1024*1024)
#define MAX_STACK (10*STACK)
#define STACK_BASE_OFFSET 1000
#define EIP_REGISTER_OFFSET 24
#define EBP_REGISTER_OFFSET 32

#define RAX 0   //返回值
#define RBX 1   //被调用者保存
#define RCX 2   //第4个参数
#define RDX 3   //第3个参数
#define RSI 4   //第2个参数
#define RDI 5   //第1个参数
#define RBP 6   //被调用者保存
#define RSP 7   //栈指针
#define R8  8   //第5个参数
#define R9  9   //第6个参数
#define R10 10  //调用者保存
#define R11 11  //调用者保存
#define R12 12  //被调用者保存
#define R13 13  //被调用者保存
#define R14 14  //被调用者保存
#define R15 15  //被调用者保存

namespace Common {

class Scheduler;

class Coroutine {
public:
    Coroutine()
    :   stack_(nullptr),
        next(nullptr),
        stackSize_(STACK),
        started_(false),
        scheduler_(&scheduler)
    {

    }

    template <typename F, typename... Args>
    Coroutine(F Func, Args... args)
        : stack_(nullptr),
          next(nullptr),
          stackSize_(STACK),
          started_(false),
          scheduler_(&scheduler)
    {   
        std::function<void()> func = [&]()->void
        {
            std::function<decltype(Func(args...))()> f = std::bind(Func, args...);
            f();
            auto mainCo = scheduler_->GetMainCoroutine();
            scheduler_->Resume(mainCo);
        };
        void (*fn)();
        fn = func.target<void()>();
        func_ = reinterpret_cast<void*>(fn);
    }

    void Start();

    bool SetStackSize(uint32_t size);

    ~Coroutine();

    friend class Scheduler;

private:
    char* regs_[16];
    char* stack_;
    void* func_;
    Scheduler* scheduler_;
    std::function<void(void)> function_;
    Coroutine* next;
    uint32_t stackSize_;
    bool started_;
};

}
#endif