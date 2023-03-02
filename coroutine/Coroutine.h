#ifndef _COROUTINE_H_
#define _COROUTINE_H_
#include <functional>

namespace MyCoroutine {

enum CoroutineState
{
    SLEEPING = 0,
    RUNNING,
    BLOCKING,
    ENDING,
};

class Scheduler;

class Coroutine {
public:
    template <typename ...Args>
    Coroutine(void* Func, Args... args)
        : stack_(nullptr),
          state_(BLOCKING),
          next_(nullptr),
          before_(nullptr)
          
    {
        function_ = std::bind(Func, args...);
        func_ = &function_();
    }

    void Start(Scheduler* scheduler);

    void Join();

    void Detach();

    ~Coroutine();

    friend class Scheduler;

private:
    char* regs_[16];
    char* stack_;
    void* func_;
    Scheduler* scheduler_;
    CoroutineState state_;
    std::function<void(void)> function_;
    Coroutine* next_;
    Coroutine* before_;
};

}
#endif