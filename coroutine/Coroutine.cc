#include "Coroutine.h"
#include "Scheduler.h"
#include <cstdlib>
#include <thread>
#include <iostream>
#include <string.h>

namespace Utility {

std::atomic<uint64_t> kCoroutineId(0);

void Coroutine::StartCo(std::function<void()>* fn)
{
    (*fn)();
}

void Coroutine::Start()
{
    started_ = true;
    stack_ = (char*)malloc(stackSize_);
    memset(stack_, 0, stackSize_);
    char* stack = stack_;
    stack = stack + STACK - 100;
    //@note : Stack memory needs to be aligned, otherwise would result to segmentation fault, which depends on cpu.
    stack = (char*)(((uint64_t)(stack) & 0xffffffffff00) + 8);
    void** eip = (void**)(stack - EIP_REGISTER_OFFSET);
    (*eip) = (void*)(Utility::Coroutine::StartCo);
    regs_[RBP] = (char*)(stack - EBP_REGISTER_OFFSET);
    regs_[RSP] = (char*)(stack - EBP_REGISTER_OFFSET);
    regs_[RDI] = (char*)(&function_);
    char** ebp = (char**)(stack - EBP_REGISTER_OFFSET);
    (*ebp) = stack;

    auto scheduler = Scheduler::GetCurrentScheduler();
    auto event = std::make_shared<SwitchEvent>();
    event->suspendCoroutine = scheduler->GetCurrentCoroutine();
    event->resumeCoroutine = shared_from_this();
    scheduler->SwitchCoroutine(event);
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
    if (nullptr != stack_)
    {
        free(stack_);
    }
}

}