#include "Coroutine.h"
#include "Scheduler.h"
#include <cstdlib>
#include <thread>
#include <iostream>
#include <string.h>

namespace Common {

void Coroutine::StartCo(std::function<void()>* fn)
{
    (*fn)();
}

void Coroutine::EventLoop()
{
    scheduler.Eventloop();
}

void Coroutine::Start()
{
    started_ = true;
    stack_ = (char*)malloc(stackSize_);
    memset(stack_, 0, stackSize_);
    char* stack = stack_;
    stack = stack + STACK - 100;
    //@note : Stack memory needs to be aligned, otherwise would result to Segmentation fault.
    stack = (char*)(((uint64_t)(stack) & 0xffffffffff00) + 8);
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