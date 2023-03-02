#include "Coroutine.h"
#include "Scheduler.h"
#include <cstdlib>
#include <thread>

#define STACK 1024
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

namespace MyCoroutine {

std::atomic<int> Coroutine::kCoroutineId = 0;

void Coroutine::Start(Scheduler* scheduler)
{
    scheduler_ = scheduler;
    stack_ = (char*)malloc(STACK);
    stack_ = stack_ + STACK_BASE_OFFSET;
    void** eip = (void**)(stack_ - EIP_REGISTER_OFFSET);
    (*eip) = func_;
    regs_[RBP] = (char*)(stack_ - EBP_REGISTER_OFFSET);
    regs_[RSP] = (char*)(stack_ - EBP_REGISTER_OFFSET);
    char** ebp = (char**)(stack_ - EBP_REGISTER_OFFSET);
    (*ebp) = stack_;
    scheduler_->Resume(this);
    state_ = ENDING;
}

void Coroutine::Join()
{

}

void Coroutine::Detach()
{

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