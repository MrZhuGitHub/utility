#include "Coroutine.h"
#include "Scheduler.h"
#include <cstdlib>
#include <thread>

namespace Common {

void Coroutine::Start()
{
    started_ = true;
    stack_ = (char*)malloc(stackSize_);
    stack_ = stack_ + STACK_BASE_OFFSET;
    void** eip = (void**)(stack_ - EIP_REGISTER_OFFSET);
    (*eip) = func_;
    regs_[RBP] = (char*)(stack_ - EBP_REGISTER_OFFSET);
    regs_[RSP] = (char*)(stack_ - EBP_REGISTER_OFFSET);
    char** ebp = (char**)(stack_ - EBP_REGISTER_OFFSET);
    (*ebp) = stack_;
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