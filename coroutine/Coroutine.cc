#include "Coroutine.h"
#include "Scheduler.h"
#include <cstdlib>
#include <thread>
#include <iostream>

namespace Common {

// void Coroutine::StartCo(std::function<void()>* fn)
// {
//     (*fn)();
// }

void Coroutine::EventLoop()
{
    scheduler.Eventloop();
}

void Coroutine::Start()
{
    started_ = true;
    stack_ = (char*)malloc(stackSize_);
    char* stack = stack_;
    stack = stack + STACK_BASE_OFFSET;
    void** eip = (void**)(stack - EIP_REGISTER_OFFSET);
    (*eip) = (void*)(Common::Coroutine::StartCo);
    regs_[RBP] = (char*)(stack - EBP_REGISTER_OFFSET);
    regs_[RSP] = (char*)(stack - EBP_REGISTER_OFFSET);
    regs_[RDI] = (char*)(&function_);
    char** ebp = (char**)(stack - EBP_REGISTER_OFFSET);
    (*ebp) = stack;
    scheduler_->Resume(this);
}

bool Coroutine::SetStackSize(uint32_t size)
{
    if (started_ || size >= MAX_STACK)
    {
        return false;
    } else {
        stackSize_ = size;
        return true;
    }
}

Coroutine::~Coroutine()
{
    scheduler_->RemoveCoroutine(this);
    if (nullptr != stack_)
    {
        free(stack_);
    }
}

}