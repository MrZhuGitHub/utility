#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include <functional>
#include <cstdint>
#include <memory>
#include <atomic>
#include "Scheduler.h"

#define STACK (1024*1024)
#define MAX_STACK (10*STACK)
// #define STACK_BASE_OFFSET (STACK - 120)
#define EIP_REGISTER_OFFSET 24
#define EBP_REGISTER_OFFSET 32
#define REGISTER_COUNT 16

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

namespace Utility {

extern std::atomic<uint64_t> kCoroutineId;

class Coroutine : public std::enable_shared_from_this<Coroutine>
{
public:
    template <typename F, typename... Args>
    Coroutine(F Func, Args... args)
    :   stack_(nullptr),
        stackSize_(STACK),
        started_(false)
    {   
        uint64_t expect = kCoroutineId.load();
        uint64_t desire = expect + 1;
        while (!kCoroutineId.compare_exchange_strong(expect, desire, std::memory_order_acquire))
        {
            expect = kCoroutineId.load();
            desire = expect + 1;
        }
        coroutineId = expect + 1;

        function_ = [=,this]()->void
        {
            std::function<decltype(Func(args...))()> f = std::bind(Func, args...);
            f();
            auto scheduler = Scheduler::GetCurrentScheduler();
            auto event = std::make_shared<SwitchEvent>();
            event->suspendCoroutine = scheduler->GetCurrentCoroutine();
            event->resumeCoroutine = scheduler->GetMainCoroutine();
            scheduler->SwitchCoroutine(event);
        };
    }

    Coroutine()
    :   stack_(nullptr),
        stackSize_(STACK),
        started_(false)
    {
        uint64_t expect = kCoroutineId.load();
        uint64_t desire = expect + 1;
        while (!kCoroutineId.compare_exchange_strong(expect, desire, std::memory_order_acquire))
        {
            expect = kCoroutineId.load();
            desire = expect + 1;
        }
        coroutineId = expect + 1;
    }

    void Start();

    bool SetStackSize(uint32_t size);

    uint64_t getCoroutineId()
    {
        return coroutineId;
    }

    ~Coroutine();

    friend Scheduler;

private:
    static void StartCo(std::function<void()>* fn);

private:
    char* regs_[REGISTER_COUNT];
    char* stack_;
    std::function<void()> function_;
    uint32_t stackSize_;
    bool started_;
    uint64_t coroutineId;
};

}
#endif